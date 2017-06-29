/*
 * fake_hw_timer.h
 *
 *  Created on: Mar 21, 2017
 *      Author: dlobato
 */

#ifndef TEST_FAKE_HW_TIMER_H_
#define TEST_FAKE_HW_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

void fake_hw_timer_trigger();
uint32_t fake_hw_timer_get_loaded_ticks();
bool fake_hw_timer_is_enabled();

#endif /* TEST_FAKE_HW_TIMER_H_ */
