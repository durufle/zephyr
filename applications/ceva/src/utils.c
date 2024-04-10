/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>



/* device tree for trigger */
#if !DT_NODE_EXISTS(DT_NODELABEL(trigger))
#error "Overlay for trigger output node not properly defined."
#endif

#if !DT_NODE_EXISTS(DT_NODELABEL(sub_trigger))
#error "Overlay for sub trigger output node not properly defined."
#endif

static const struct gpio_dt_spec trigger = GPIO_DT_SPEC_GET_OR(DT_NODELABEL(trigger), gpios, {0});
static const struct gpio_dt_spec sub_trigger = GPIO_DT_SPEC_GET_OR(DT_NODELABEL(sub_trigger), gpios, {0});

int trigger_init(void) {
    int ret;
    
	ret = gpio_is_ready_dt(&trigger);
    if (ret < 0) {
		return ret;
	}
    ret = gpio_pin_configure_dt(&trigger, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        return ret;
	}
	ret = gpio_is_ready_dt(&sub_trigger);
    if (ret < 0) {
		return ret;
	}
    ret = gpio_pin_configure_dt(&sub_trigger, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        return ret;
	}
    return 0;
}

/**
 * @brief set trigger high
 */
int trigger_high(void) {
    return gpio_pin_set_dt(&trigger,1);
}

/**
 * @brief set trigger low
 */
int trigger_low(void) {
    return gpio_pin_set_dt(&trigger,0);
}

/**
 * @brief set sub trigger high
 */
int sub_trigger_high(void) {
    return gpio_pin_set_dt(&sub_trigger,1);
}

/**
 * @brief set sub trigger low
 */
int sub_trigger_low(void) {
    return gpio_pin_set_dt(&sub_trigger,0);
}
