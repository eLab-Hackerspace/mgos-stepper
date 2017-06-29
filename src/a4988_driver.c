#include "a4988_driver.h"
#include "fw/src/mgos_gpio.h"
#include "fw/src/mgos_hal.h"
#include "common/cs_dbg.h"

#include "sys_config.h"
struct a4988_driver {
  int enable_gpio;
  int dir_gpio;
  int step_gpio;
  int step_position;
};

struct a4988_driver* a4988_driver_create(const struct sys_config_a4988 *cfg) {
  struct a4988_driver *c = NULL;

  c = calloc(1, sizeof(*c));

  if (c == NULL) return NULL;

  c->enable_gpio = cfg->enable_gpio;
  c->dir_gpio    = cfg->dir_gpio;
  c->step_gpio   = cfg->step_gpio;

  if (!mgos_gpio_set_mode(c->enable_gpio, MGOS_GPIO_MODE_OUTPUT)) {
    goto out_err;
  }

  if (!mgos_gpio_set_mode(c->dir_gpio, MGOS_GPIO_MODE_OUTPUT)) {
    goto out_err;
  }

  if (!mgos_gpio_set_mode(c->step_gpio, MGOS_GPIO_MODE_OUTPUT)) {
    goto out_err;
  }
  mgos_gpio_write(c->enable_gpio, 1);
  mgos_gpio_write(c->dir_gpio,    0);
  mgos_gpio_write(c->step_gpio,   0);
  c->step_position = 0;

  LOG(LL_INFO,
      (
          "A4988 GPIO init ok (enable: %d, dir: %d, step: %d)",
          c->enable_gpio, c->dir_gpio, c->step_gpio
      )
  );

  return c;
out_err:
  free(c);
  LOG(LL_ERROR, ("Invalid A49888 GPIO settings"));
  return NULL;
}

void a4988_driver_enable(struct a4988_driver *driver) {
  mgos_gpio_write(driver->enable_gpio, 0);
}

void a4988_driver_disable(struct a4988_driver *driver) {
  mgos_gpio_write(driver->enable_gpio, 1);
}
void a4988_driver_set_position(struct a4988_driver *driver, int position){
  driver->step_position = position;
}

int a4988_driver_get_position(struct a4988_driver *driver){
  return driver->step_position;
}

void a4988_driver_step(struct a4988_driver *driver, enum step_dir_type dir) {
  if (dir == CW) {
    driver->step_position++;
  } else { // CCW
    driver->step_position--;
  }
  mgos_gpio_write(driver->dir_gpio,  dir);

  // step pulse
  mgos_gpio_write(driver->step_gpio, 1);
  //mgos_usleep(1);
  mgos_gpio_write(driver->step_gpio, 0);
}
