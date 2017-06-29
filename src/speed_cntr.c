/*
 * Based on AVR446 application note code
 */

#include "speed_cntr.h"
#include <stdlib.h>
#include "hw_timer.h"
#include "common/cs_dbg.h"

/*
 * T_FREQ Frequency of timer in [Hz].
 * User must define with current frequency.
 */
#ifndef T_FREQ
#error T_FREQ not defined!
#endif

// Maths constants. To simplify maths when calculating in speed_cntr_Move().
#define ALPHA (2 * 3.14159 / SPR) // 2*pi/spr

static const unsigned int A_T_x100 = ALPHA * T_FREQ * 100;   // (ALPHA / T)*100
static const unsigned int T_FREQ_148 = (T_FREQ * 0.676) / 100;  // T_FREQ divided by 100
                                                       // and scaled by 0.676
static const unsigned int A_SQ = ALPHA * 2 * 10000000000;  // ALPHA*2*10000000000
static const unsigned int A_x20000 = ALPHA * 20000;            // ALPHA*20000

/*
 * Holding data used by timer interrupt for speed ramp calculation.
 *
 *  Contains data used by timer interrupt to calculate speed profile.
 *  Data is written to it by move(), when stepper motor is moving (timer
 *  interrupt running) data is read/updated when calculating a new step_delay
 */
struct speed_ramp_data {
  // ! What part of the speed ramp we are in.
  enum run_state run_state;

  // ! Direction stepper motor should move.
  unsigned char dir :1;

  // ! Period of next timer delay. At start this value set the acceleration rate.
  unsigned int step_delay;

  // ! What step_pos to start deceleration
  unsigned int decel_start;

  // ! Sets deceleration rate.
  signed int decel_val;

  // ! Minimum time delay (max speed)
  unsigned int min_delay;

  // ! Counter used when accelerating/decelerating to calculate step_delay.
  signed int accel_count;
};

struct speed_cntr_driver {
  struct a4988_driver *sm_driver;
  struct speed_ramp_data srd;
};

// forward declarations
unsigned long fast_sqrt(unsigned long v);
void speed_cntr_cb(void *arg);


struct speed_cntr_driver* speed_cntr_driver_create(
    struct a4988_driver *sm_driver) {
  struct speed_cntr_driver *d = NULL;

  if (sm_driver == NULL) {
    return NULL;
  }

  d = calloc(1, sizeof(*d));

  if (d == NULL)
    return NULL;

  d->sm_driver = sm_driver;
  d->srd.run_state = STOP;

  hw_timer_init(speed_cntr_cb, (void*)d);

  LOG(LL_INFO, ("Speed Controller init ok"));

  return d;
}

void speed_cntr_move(struct speed_cntr_driver* d, signed int step,
                     unsigned int accel, unsigned int decel, unsigned int speed) {
  // ! Number of steps before we hit max speed.
  unsigned int max_s_lim;

  // ! Number of steps before we must start deceleration (if accel does not hit
  // max speed).
  unsigned int accel_lim;

  // Set direction from sign on step value.
  if (step < 0) {
    d->srd.dir = CCW;
    step = -step;
  } else {
    d->srd.dir = CW;
  }

  //FIXME: optional
  a4988_driver_enable(d->sm_driver);

  // If moving only 1 step.
  if (step == 1) {
    // Move one step...
    d->srd.accel_count = -1;

    // ...in DECEL state.
    d->srd.run_state = DECEL;

    d->srd.step_delay = 1;//min delay to jump to STOP

    speed_cntr_cb((void*) d);
    hw_timer_enable();
  }

  // Only move if number of steps to move is not zero.
  else if (step != 0) {
    // Refer to documentation for detailed information about these calculations.

    // Set max speed limit, by calc min_delay to use in timer.
    // min_delay = (alpha / tt)/ w
    d->srd.min_delay = A_T_x100 / speed;

    // Set accelration by calc the first (c0) step delay .
    // step_delay = 1/tt * sqrt(2*alpha/accel)
    // step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) /
    // (accel*100) )/10000
    d->srd.step_delay = (T_FREQ_148 * fast_sqrt(A_SQ / accel)) / 100;

    // Find out after how many steps does the speed hit the max speed limit.
    // max_s_lim = speed^2 / (2*alpha*accel)
    max_s_lim = ((long) speed * speed) / (((long) A_x20000 * accel) / 100);

    // If we hit max speed limit before 0,5 step it will round to 0.
    // But in practice we need to move atleast 1 step to get any speed at all.
    if (max_s_lim == 0) {
      max_s_lim = 1;
    }

    // Find out after how many steps we must start deceleration.
    // n1 = (n1+n2)decel / (accel + decel)
    accel_lim = ((long) step * decel) / (accel + decel);

    // We must accelrate at least 1 step before we can start deceleration.
    if (accel_lim == 0) {
      accel_lim = 1;
    }

    // Use the limit we hit first to calc decel.
    if (accel_lim <= max_s_lim) {
      d->srd.decel_val = accel_lim - step;
    } else {
      d->srd.decel_val = -(((long) max_s_lim * accel) / decel);
    }

    // We must decelrate at least 1 step to stop.
    if (d->srd.decel_val == 0) {
      d->srd.decel_val = -1;
    }

    // Find step to start decleration.
    d->srd.decel_start = step + d->srd.decel_val;

    // If the maximum speed is so low that we dont need to go via accelration
    // state.
    if (d->srd.step_delay <= d->srd.min_delay) {
      d->srd.step_delay = d->srd.min_delay;
      d->srd.run_state = RUN;
    } else {
      d->srd.run_state = ACCEL;
    }

    // Reset counter.
    d->srd.accel_count = 0;

    speed_cntr_cb((void*) d);
    hw_timer_enable();
  }
}

void speed_cntr_stop(struct speed_cntr_driver* d) {
  d->srd.run_state = STOP;
}

enum run_state speed_cntr_get_run_state(struct speed_cntr_driver* d){
  return d->srd.run_state;
}

void speed_cntr_cb(void *arg) {
  struct speed_cntr_driver* d = (struct speed_cntr_driver*) arg;

  // Holds next delay period.
  unsigned int new_step_delay = 0;

  // Remember the last step delay used when accelerating.
  static unsigned int last_accel_delay;

  // Counting steps when moving.
  static unsigned int step_count = 0;

  // Keep track of remainder from new_step-delay calculation to increase
  // accuracy
  static unsigned int rest = 0;

  hw_timer_load(d->srd.step_delay);

  switch (d->srd.run_state) {
    case STOP:
      step_count = 0;
      rest = 0;

      hw_timer_disable();
      //TODO: optional
      a4988_driver_disable(d->sm_driver);
      break;

    case ACCEL:
      a4988_driver_step(d->sm_driver, d->srd.dir);
      step_count++;
      d->srd.accel_count++;

      {
        int32_t a = ((2 * d->srd.step_delay) + rest);
        int32_t b = (4 * d->srd.accel_count + 1);

        new_step_delay = d->srd.step_delay - a / b;
        rest = a % b;
      }


      // Check if we should start deceleration.
      if (step_count >= d->srd.decel_start) {
        d->srd.accel_count = d->srd.decel_val;
        d->srd.run_state = DECEL;
      }
      // Check if we hit max speed.
      else if (new_step_delay <= d->srd.min_delay) {
        last_accel_delay = new_step_delay;
        new_step_delay = d->srd.min_delay;
        rest = 0;
        d->srd.run_state = RUN;
      }
      break;

    case RUN:
      a4988_driver_step(d->sm_driver, d->srd.dir);
      step_count++;
      new_step_delay = d->srd.min_delay;

      // Check if we should start deceleration.
      if (step_count >= d->srd.decel_start) {
        d->srd.accel_count = d->srd.decel_val;

        // Start deceleration with same delay as accel ended with.
        new_step_delay = last_accel_delay;
        d->srd.run_state = DECEL;
      }
      break;

    case DECEL:
      a4988_driver_step(d->sm_driver, d->srd.dir);
      step_count++;

      {
        int32_t a = ((2 * d->srd.step_delay) + rest);
        int32_t b = (4 * d->srd.accel_count + 1);

        new_step_delay = d->srd.step_delay - a / b;
        rest = a % b;
      }

      d->srd.accel_count++;
      // Check if we at last step
      if (d->srd.accel_count >= 0) {
        d->srd.run_state = STOP;
      }
      break;
  }
  d->srd.step_delay = new_step_delay;
}

/*! \brief Square root routine.
 *
 * sqrt routine 'grupe', from comp.sys.ibm.pc.programmer
 * Subject: Summary: SQRT(int) algorithm (with profiling)
 *    From: warwick@cs.uq.oz.au (Warwick Allison)
 *    Date: Tue Oct 8 09:16:35 1991
 *
 *  \param x  Value to find square root of.
 *  \return  Square root of x.
 */
unsigned long fast_sqrt(unsigned long x) {
  register unsigned long xr;  // result register
  register unsigned long q2;  // scan-bit register
  register unsigned char f;  // flag (one bit)

  xr = 0;                    // clear result
  q2 = 0x40000000L;          // higest possible result bit

  do {
    if ((xr + q2) <= x) {
      x -= xr + q2;
      f = 1;  // set flag
    } else {
      f = 0;  // clear flag
    }
    xr >>= 1;

    if (f) {
      xr += q2;       // test flag
    }
  } while (q2 >>= 2);  // shift twice

  if (xr < x) {
    return xr + 1;    // add for rounding
  } else {
    return xr;
  }
}
