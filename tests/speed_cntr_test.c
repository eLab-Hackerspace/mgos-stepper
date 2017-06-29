/*
 * Copyright (c) 2014-2016 Cesanta Software Limited
 * All rights reserved
 */

#include "test_util.h"
#include "mgos_config.h"
#include "cs_dbg.h"
#include "cs_file.h"
#include "sys_config.h"
#include "speed_cntr.h"
#include "a4988_driver.h"
#include "mgos_gpio.h"
#include "hw_timer.h"
#include "fake_hw_timer.h"

struct sys_config conf;

static const char *test_config(void) {
  return NULL;
}


static const char *test_speed_cntr_create(void) {
  struct a4988_driver* sm_driver;
  struct speed_cntr_driver* d;

  sm_driver = a4988_driver_create(&conf.a4988);
  d = speed_cntr_driver_create(sm_driver);

  ASSERT(d != NULL);
  ASSERT_EQ(speed_cntr_get_run_state(d), STOP);

  free(d);
  free(sm_driver);

  return NULL;
}

static const char *test_speed_cntr_create_fail(void) {
  struct speed_cntr_driver* d;

  d = speed_cntr_driver_create(NULL);

  ASSERT(d == NULL);

  return NULL;
}

static const char *test_speed_cntr_move_one_step(void) {
  struct a4988_driver* sm_driver;
  struct speed_cntr_driver* d;

  sm_driver = a4988_driver_create(&conf.a4988);
  d = speed_cntr_driver_create(sm_driver);

  ASSERT(d != NULL);
  ASSERT_EQ(a4988_driver_get_position(sm_driver), 0);

  speed_cntr_move(d, 1, 20, 20, 1000);

  while(fake_hw_timer_is_enabled()) {
    fake_hw_timer_trigger();
  }

  ASSERT_EQ(a4988_driver_get_position(sm_driver), 1);

  free(d);
  free(sm_driver);

  return NULL;
}

static const char *test_speed_cntr_move_step(void) {
  struct a4988_driver* sm_driver;
  struct speed_cntr_driver* d;
  float elapsed_time = 0;
  const float Tt = (1.0/T_FREQ);
  const float ALPHA = (2 * 3.14159) / SPR;
  const float accel = 1.0;
  const float decel = accel;
  const float max_speed = 10.0;
  const int steps = 100;

  sm_driver = a4988_driver_create(&conf.a4988);
  d = speed_cntr_driver_create(sm_driver);

  printf("Tt=%f\n", Tt);
  ASSERT(d != NULL);
  ASSERT_EQ(a4988_driver_get_position(sm_driver), 0);

  speed_cntr_move(d, steps, (unsigned int)(accel*100), (unsigned int)(decel*100), (unsigned int)(max_speed*100));

  while(fake_hw_timer_is_enabled()) {
    float step_delay = fake_hw_timer_get_loaded_ticks() * Tt;
    elapsed_time += step_delay;
    printf("elapsed_time=%f\n", elapsed_time);
    printf("w=ALPHA/step_delay=%f\n", ALPHA/step_delay);
    if (speed_cntr_get_run_state(d) == ACCEL) {
    } else if (speed_cntr_get_run_state(d) == RUN) {
    } else if (speed_cntr_get_run_state(d) == DECEL) {
    }
    fake_hw_timer_trigger();
  }

  ASSERT_EQ(a4988_driver_get_position(sm_driver), steps);

  free(d);
  free(sm_driver);

  return NULL;
}

static const char *set_up() {
  size_t size;
  char *json1 = cs_read_file(".build/sys_config_defaults.json", &size);

  const struct mgos_conf_entry *schema = sys_config_schema();

  memset(&conf, 0, sizeof(conf));

  ASSERT_EQ(mgos_conf_parse(mg_mk_str(json1), "*", schema, &conf), true);

  free(json1);

  return NULL;
}

static const char *run_tests(const char *filter, double *total_elapsed) {
  RUN_TEST(test_config);
  RUN_TEST(test_speed_cntr_create);
  RUN_TEST(test_speed_cntr_create_fail);
  RUN_TEST(test_speed_cntr_move_one_step);
  RUN_TEST(test_speed_cntr_move_step);
  return NULL;
}

int __cdecl main(int argc, char *argv[]) {
  const char *fail_msg;
  const char *filter = argc > 1 ? argv[1] : "";
  double total_elapsed = 0.0;

  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  cs_log_set_level(LL_NONE);
  fail_msg = set_up();
  if (fail_msg != NULL) {
    printf("set up failed: %s\n", fail_msg);
    return EXIT_FAILURE;
  }

  fail_msg = run_tests(filter, &total_elapsed);
  printf("%s, run %d in %.3lfs\n", fail_msg ? "FAIL" : "PASS", num_tests,
         total_elapsed);
  return fail_msg == NULL ? EXIT_SUCCESS : EXIT_FAILURE;
}

