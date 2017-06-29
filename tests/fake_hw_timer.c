/*
 * fake_hw_timer.c
 *
 *  Created on: Mar 21, 2017
 *      Author: dlobato
 */

#include "fake_hw_timer.h"
#include "hw_timer.h"
#include <stdio.h>
#include <inttypes.h>

static void* user_hw_timer_cb_arg = NULL;
static void (* user_hw_timer_cb)(void*) = NULL;
static bool enabled = false;
static uint32_t ticks = 0;

void fake_hw_timer_trigger(){
  if (user_hw_timer_cb != NULL) {
    (*(user_hw_timer_cb))(user_hw_timer_cb_arg);
  }
}

uint32_t fake_hw_timer_get_loaded_ticks(){
  return ticks;
}

bool fake_hw_timer_is_enabled(){
  return enabled;
}

void hw_timer_init(void (* user_hw_timer_cb_set)(void*), void* arg){
  user_hw_timer_cb = user_hw_timer_cb_set;
  user_hw_timer_cb_arg = arg;

  printf("hw_timer_init: cb=%p, arg=%p\n", (void*)user_hw_timer_cb_set, arg);
}

void hw_timer_load(uint32_t _ticks){
  printf("hw_timer_load: ticks=%" PRIu32 "\n", _ticks);
  ticks = _ticks;
}

void hw_timer_enable(){
  enabled = true;
  printf("hw_timer_enable:\n");
}

void hw_timer_disable(){
  enabled = false;
  printf("hw_timer_disable:\n");
}
