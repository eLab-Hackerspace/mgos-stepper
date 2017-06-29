/*
 * dlobato: Based on ESPRESSIF hw_timer
 *
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "hw_timer.h"
#include "esp_common.h"
#include <stdio.h>
#include <inttypes.h>

#define FRC1_ENABLE_TIMER   BIT7
#define FRC1_AUTO_LOAD      BIT6


#define RTC_REG_WRITE(addr, val)    WRITE_PERI_REG(addr, val)

typedef enum {          // timer provided mode
  DIVDED_BY_1   = 0,  // timer clock
  DIVDED_BY_16  = 4,  // divided by 16
  DIVDED_BY_256 = 8,  // divided by 256
} TIMER_PREDIVED_MODE;


typedef enum {          // timer interrupt mode
  TM_LEVEL_INT = 1,   // level interrupt
  TM_EDGE_INT  = 0,   // edge interrupt
} TIMER_INT_MODE;

static void (* user_hw_timer_cb)(void*) = NULL;

static void hw_timer_isr_cb(void *arg)
{
  if (user_hw_timer_cb != NULL) {
    (*(user_hw_timer_cb))(arg);
  }
}

void hw_timer_init(void (* user_hw_timer_cb_set)(void*), void* arg)
{
  RTC_REG_WRITE(FRC1_CTRL_ADDRESS, DIVDED_BY_256 | FRC1_ENABLE_TIMER | TM_EDGE_INT);

  user_hw_timer_cb = user_hw_timer_cb_set;

  _xt_isr_attach(ETS_FRC_TIMER1_INUM, hw_timer_isr_cb, arg);
}

void hw_timer_load(uint32_t ticks)
{
  RTC_REG_WRITE(FRC1_LOAD_ADDRESS, ticks);
}

void hw_timer_enable()
{
  TM1_EDGE_INT_ENABLE();
  _xt_isr_unmask(1 << ETS_FRC_TIMER1_INUM);
}

void hw_timer_disable()
{
  _xt_isr_mask(1 << ETS_FRC_TIMER1_INUM);
  TM1_EDGE_INT_DISABLE();
}
