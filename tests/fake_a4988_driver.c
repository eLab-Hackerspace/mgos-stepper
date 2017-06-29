/*
 * fake_a4988_driver.c
 *
 *  Created on: Mar 21, 2017
 *      Author: dlobato
 */

#include "a4988_driver.h"
#include "sys_config.h"

struct a4988_driver {
  int step_position;
};

struct a4988_driver* a4988_driver_create(const struct sys_config_a4988* cfg){
  struct a4988_driver *c = NULL;

  c = calloc(1, sizeof(*c));

  if (c == NULL) return NULL;
  c->step_position = 0;
  (void)cfg;
  return c;
}

void a4988_driver_enable(struct a4988_driver* driver){
  (void)driver;
}

void a4988_driver_disable(struct a4988_driver* driver){
  (void)driver;
}

void a4988_driver_set_position(struct a4988_driver *driver, int position){
  (void)driver;
  driver->step_position = position;
}

int a4988_driver_get_position(struct a4988_driver *driver){
  return driver->step_position;
}

void a4988_driver_step(struct a4988_driver* driver, enum step_dir_type dir){
  if (dir == CW) {
    driver->step_position++;
  } else { // CCW
    driver->step_position--;
  }
}
