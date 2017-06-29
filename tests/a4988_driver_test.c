/*
 * Copyright (c) 2014-2016 Cesanta Software Limited
 * All rights reserved
 */

#include "test_util.h"
#include "mgos_config.h"
#include "cs_dbg.h"
#include "cs_file.h"
#include "sys_config.h"
#include "a4988_driver.h"
#include "mgos_gpio.h"
#include "fake_mgos_gpio.h"

struct sys_config conf;

static const char *test_config(void) {
  ASSERT_EQ(conf.a4988.enable, false);
  ASSERT_EQ(conf.a4988.enable_gpio, 4);
  ASSERT_EQ(conf.a4988.dir_gpio, 13);
  ASSERT_EQ(conf.a4988.step_gpio, 15);

  return NULL;
}

static const char *test_a4988_driver_enable(void) {
  fake_mgos_gpio_set_up();

  struct a4988_driver* d = a4988_driver_create(&conf.a4988);

  ASSERT(d != NULL);

  fake_mgos_gpio_reset_write_events();
  a4988_driver_enable(d);

  struct gpio_write_event expected_write_events[] = {
      { .pin = conf.a4988.enable_gpio, .level = 0 }
  };
  ASSERT_EQ(fake_mgos_gpio_get_write_events_count(),
            (sizeof(expected_write_events) / sizeof(expected_write_events[0])));
  int i;
  for (i = 0; i < fake_mgos_gpio_get_write_events_count(); i++) {
    ASSERT(
        gpio_write_event_is_equal(expected_write_events[i],
                                  fake_mgos_gpio_get_write_events()[i]));

  }

  free(d);

  return NULL;
}

static const char *test_a4988_driver_disable(void) {
  fake_mgos_gpio_set_up();

  struct a4988_driver* d = a4988_driver_create(&conf.a4988);

  ASSERT(d != NULL);

  fake_mgos_gpio_reset_write_events();
  a4988_driver_disable(d);

  struct gpio_write_event expected_write_events[] = {
      { .pin = conf.a4988.enable_gpio, .level = 1 }
  };
  ASSERT_EQ(fake_mgos_gpio_get_write_events_count(),
            (sizeof(expected_write_events) / sizeof(expected_write_events[0])));
  int i;
  for (i = 0; i < fake_mgos_gpio_get_write_events_count(); i++) {
    ASSERT(
        gpio_write_event_is_equal(expected_write_events[i],
                                  fake_mgos_gpio_get_write_events()[i]));

  }

  free(d);

  return NULL;
}

static const char *test_a4988_driver_create(void) {
  fake_mgos_gpio_set_up();

  struct a4988_driver* d = a4988_driver_create(&conf.a4988);

  ASSERT(d != NULL);
  ASSERT_EQ(a4988_driver_get_position(d), 0);
  struct gpio_write_event expected_write_events[] = {
      { .pin = conf.a4988.enable_gpio, .level = 1 },
      { .pin = conf.a4988.dir_gpio, .level = 0 },
      { .pin = conf.a4988.step_gpio, .level = 0 }
  };
  ASSERT_EQ(fake_mgos_gpio_get_write_events_count(),
            (sizeof(expected_write_events) / sizeof(expected_write_events[0])));
  int i;
  for (i = 0; i < fake_mgos_gpio_get_write_events_count(); i++) {
    ASSERT(
        gpio_write_event_is_equal(expected_write_events[i],
                                  fake_mgos_gpio_get_write_events()[i]));

  }

  free(d);

  return NULL;
}

static const char *test_a4988_driver_create_fail(void) {
  fake_mgos_gpio_set_up();
  fake_mgos_gpio_set_mode_will_fail(true);

  struct a4988_driver* d = a4988_driver_create(&conf.a4988);

  ASSERT(d == NULL);

  return NULL;
}

static const char *test_a4988_driver_set_get_position(void) {
  fake_mgos_gpio_set_up();

  struct a4988_driver* d = a4988_driver_create(&conf.a4988);

  ASSERT(d != NULL);
  ASSERT_EQ(a4988_driver_get_position(d), 0);
  a4988_driver_set_position(d, 100);
  ASSERT_EQ(a4988_driver_get_position(d), 100);

  free(d);

  return NULL;
}

static const char *test_a4988_driver_step(void) {
  fake_mgos_gpio_set_up();

  struct a4988_driver* d = a4988_driver_create(&conf.a4988);

  ASSERT(d != NULL);
  ASSERT_EQ(a4988_driver_get_position(d), 0);

  fake_mgos_gpio_reset_write_events();
  a4988_driver_step(d, CW);
  {
    struct gpio_write_event expected_write_events[] = {
        { .pin = conf.a4988.dir_gpio, .level = CW },
        { .pin = conf.a4988.step_gpio, .level = 1 },
        { .pin = conf.a4988.step_gpio, .level = 0 }
    };
    ASSERT_EQ(fake_mgos_gpio_get_write_events_count(),
              (sizeof(expected_write_events) / sizeof(expected_write_events[0])));
    int i;
    for (i = 0; i < fake_mgos_gpio_get_write_events_count(); i++) {
      ASSERT(
          gpio_write_event_is_equal(expected_write_events[i],
                                    fake_mgos_gpio_get_write_events()[i]));

    }
  }

  ASSERT_EQ(a4988_driver_get_position(d), 1);

  fake_mgos_gpio_reset_write_events();
  a4988_driver_step(d, CCW);
  {
    struct gpio_write_event expected_write_events[] = {
        { .pin = conf.a4988.dir_gpio, .level = CCW },
        { .pin = conf.a4988.step_gpio, .level = 1 },
        { .pin = conf.a4988.step_gpio, .level = 0 }
    };
    ASSERT_EQ(fake_mgos_gpio_get_write_events_count(),
              (sizeof(expected_write_events) / sizeof(expected_write_events[0])));
    int i;
    for (i = 0; i < fake_mgos_gpio_get_write_events_count(); i++) {
      ASSERT(
          gpio_write_event_is_equal(expected_write_events[i],
                                    fake_mgos_gpio_get_write_events()[i]));

    }
  }
  ASSERT_EQ(a4988_driver_get_position(d), 0);

  free(d);

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
  RUN_TEST(test_a4988_driver_create);
  RUN_TEST(test_a4988_driver_create_fail);
  RUN_TEST(test_a4988_driver_enable);
  RUN_TEST(test_a4988_driver_disable);
  RUN_TEST(test_a4988_driver_set_get_position);
  RUN_TEST(test_a4988_driver_step);
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

