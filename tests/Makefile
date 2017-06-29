A4988_DRIVER_TEST_PROG = a4988_driver_test
SPEED_CNTR_TEST_PROG = speed_cntr_test
MGOS_PATH ?= ../..
SRC_PATH ?= ../src
PYTHON ?= python
BUILD_DIR = .build
SYS_CONFIG_C = $(BUILD_DIR)/sys_config.c
SYS_RO_VARS_C = $(BUILD_DIR)/sys_ro_vars.c
APP_CONF_SCHEMA = conf_a4988.yaml conf_speed_cntr.yaml

A4988_DRIVER_TEST_SOURCES = a4988_driver_test.c \
          fake_mgos_gpio.c \
          $(SRC_PATH)/a4988_driver.c \
          $(SYS_CONFIG_C) \
          $(SYS_RO_VARS_C) \
          $(MGOS_PATH)/frozen/frozen.c \
          $(MGOS_PATH)/fw/src/mgos_config.c \
          $(MGOS_PATH)/mongoose/mongoose.c \
          $(MGOS_PATH)/common/json_utils.c \
          $(MGOS_PATH)/common/cs_file.c \
          $(MGOS_PATH)/common/test_util.c
          
SPEED_CNTR_TEST_SOURCES = speed_cntr_test.c \
          fake_a4988_driver.c \
          fake_hw_timer.c \
          $(SRC_PATH)/speed_cntr.c \
          $(SYS_CONFIG_C) \
          $(SYS_RO_VARS_C) \
          $(MGOS_PATH)/frozen/frozen.c \
          $(MGOS_PATH)/fw/src/mgos_config.c \
          $(MGOS_PATH)/mongoose/mongoose.c \
          $(MGOS_PATH)/common/json_utils.c \
          $(MGOS_PATH)/common/cs_file.c \
          $(MGOS_PATH)/common/test_util.c

INCS = -I$(MGOS_PATH)/fw/src \
       -I$(MGOS_PATH) \
       -I$(MGOS_PATH)/common \
       -I $(SRC_PATH)/ \
       $(CFLAGS_EXTRA)

CFLAGS = -W -Wall -Werror -g -O0 -Wno-multichar -I$(BUILD_DIR) $(INCS)


$(BUILD_DIR):
	mkdir $@

all: $(BUILD_DIR) $(A4988_DRIVER_TEST_PROG) $(SPEED_CNTR_TEST_PROG)
	./$(A4988_DRIVER_TEST_PROG)
	./$(SPEED_CNTR_TEST_PROG)

$(A4988_DRIVER_TEST_PROG): $(A4988_DRIVER_TEST_SOURCES)
	$(CC) -o $@ $^ $(CFLAGS)
	
$(SPEED_CNTR_TEST_PROG): $(SPEED_CNTR_TEST_SOURCES)
	$(CC) -o $@ $^ $(CFLAGS)

include $(MGOS_PATH)/fw/common.mk
include $(MGOS_PATH)/fw/src/sys_config.mk

clean:
	rm -rf $(A4988_DRIVER_TEST_PROG) $(SPEED_CNTR_TEST_PROG) $(BUILD_DIR)
