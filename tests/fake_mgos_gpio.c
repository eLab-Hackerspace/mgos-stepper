#include "fake_mgos_gpio.h"
#include "fw/src/mgos_gpio.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"

#ifndef GPIO_PIN_COUNT
#define GPIO_PIN_COUNT 16
#endif


#ifndef FAKE_MGOS_GPIO_MAX_EVENTS
#define FAKE_MGOS_GPIO_MAX_EVENTS 32
#endif

uint8_t pin_config[GPIO_PIN_COUNT];

struct gpio_write_event write_events[FAKE_MGOS_GPIO_MAX_EVENTS];
int write_events_count;

bool set_mode_will_fail;

bool gpio_write_event_is_equal(struct gpio_write_event a,
                               struct gpio_write_event b){
  return (a.pin == b.pin) && (a.level == b.level);
}

void fake_mgos_gpio_set_up(){
  int i;
  for(i = 0; i < GPIO_PIN_COUNT; i++){
    pin_config[i] = MGOS_GPIO_MODE_INPUT;
  }
  write_events_count = 0;
  set_mode_will_fail = false;
}

void fake_mgos_gpio_reset_write_events(){
  write_events_count = 0;
}

int fake_mgos_gpio_get_write_events_count(){
  return write_events_count;
}

struct gpio_write_event* fake_mgos_gpio_get_write_events(){
  return write_events;
}


void fake_mgos_gpio_set_mode_will_fail(bool s){
  set_mode_will_fail = s;
}

bool mgos_gpio_set_mode(int pin, enum mgos_gpio_mode mode){
  pin_config[pin] = mode;
  return !set_mode_will_fail;
}

void mgos_gpio_write(int pin, bool level){
  if (pin_config[pin] != MGOS_GPIO_MODE_OUTPUT) {
    fprintf(stderr, "mgos_gpio_write: pin=%d not configured as output\n", pin);
    exit(EXIT_FAILURE);
  }
  if (write_events_count >= FAKE_MGOS_GPIO_MAX_EVENTS) {
    fprintf(stderr, "mgos_gpio_write: max events reached (%d)\n",
            FAKE_MGOS_GPIO_MAX_EVENTS);
        exit(EXIT_FAILURE);
  }
  write_events[write_events_count].pin = pin;
  write_events[write_events_count].level = level;
  write_events_count++;
}
