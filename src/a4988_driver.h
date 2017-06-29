#ifndef A4988_DRIVER_H
#define A4988_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

enum step_dir_type {
  CW,
  CCW
};

struct a4988_driver;
struct sys_config_a4988;

struct a4988_driver* a4988_driver_create(const struct sys_config_a4988* cfg);

void a4988_driver_enable(struct a4988_driver* driver);
void a4988_driver_disable(struct a4988_driver* driver);
void a4988_driver_set_position(struct a4988_driver *driver, int position);
int a4988_driver_get_position(struct a4988_driver *driver);
void a4988_driver_step(struct a4988_driver* driver, enum step_dir_type dir);

#ifdef __cplusplus
}
#endif


#endif //A4988_DRIVER_H
