/*
 * Based on AVR446 application note code
 */

#ifndef SPEED_CNTR_H
#define SPEED_CNTR_H

#include "a4988_driver.h"

/*
 * Number of steps per round on stepper motor in use.
 * User must define according to motor specs.
 */
#ifndef SPR
#define SPR 200
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Ramp states
 */
enum run_state {
  STOP,
  ACCEL,
  DECEL,
  RUN
};

struct speed_cntr_driver;

struct speed_cntr_driver* speed_cntr_driver_create(
    struct a4988_driver* sm_driver);

/*! \brief Move the stepper motor a given number of steps.
 *
 *  Makes the stepper motor move the given number of steps.
 *  It accelrate with given accelration up to maximum speed and decelerate
 *  with given deceleration so it stops at the given step.
 *  If accel/decel is to small and steps to move is to few, speed might not
 *  reach the max speed limit before deceleration starts.
 *
 *  \param step  Number of steps to move (pos - CW, neg - CCW).
 *  \param accel  Accelration to use, in 0.01*rad/sec^2.
 *  \param decel  Decelration to use, in 0.01*rad/sec^2.
 *  \param speed  Max speed, in 0.01*rad/sec.
 */
void speed_cntr_move(struct speed_cntr_driver* d, signed int step,
                     unsigned int accel, unsigned int decel, unsigned int speed);

void speed_cntr_stop(struct speed_cntr_driver* d);

enum run_state speed_cntr_get_run_state(struct speed_cntr_driver* d);

#ifdef __cplusplus
}
#endif

#endif
