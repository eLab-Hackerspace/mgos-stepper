/*
 * fake_mgos_gpio.h
 *
 *  Created on: Mar 21, 2017
 *      Author: dlobato
 */

#ifndef TEST_FAKE_MGOS_GPIO_H_
#define TEST_FAKE_MGOS_GPIO_H_

#include "stdbool.h"

struct gpio_write_event{
  int pin;
  bool level;
};

bool gpio_write_event_is_equal(struct gpio_write_event a,
                               struct gpio_write_event b);

void fake_mgos_gpio_set_up();
void fake_mgos_gpio_set_mode_will_fail(bool s);
void fake_mgos_gpio_reset_write_events();

int fake_mgos_gpio_get_write_events_count();
struct gpio_write_event* fake_mgos_gpio_get_write_events();

#endif /* TEST_FAKE_MGOS_GPIO_H_ */
