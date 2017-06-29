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

#ifndef __HW_TIMER_H__
#define __HW_TIMER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * T_FREQ Frequency of timer in [Hz].
 * User must define with current frequency.
 */
#ifndef T_FREQ
#define T_FREQ 312500 //312.5KHz (3.2us)
#endif

/**
 * Initialize timer
 */
void hw_timer_init(void (* user_hw_timer_cb_set)(void*), void* arg);

/**
 * Load timer
 *
 * @param uint32_t ticks: ticks to load onto timer
 *    - range : 0 ~ 0x7fffff
 */
void hw_timer_load(uint32_t ticks);

/**
 * Enable timer (start)
 */
void hw_timer_enable();

/**
 * Disable timer (stop)
 */
void hw_timer_disable();


#ifdef __cplusplus
}
#endif

#endif
