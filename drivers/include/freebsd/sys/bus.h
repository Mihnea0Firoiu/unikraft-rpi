/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * FreeBSD sys/bus.h shim — re-exports from bcm_gpio_internal.h plus
 * device_printf (needs stdio.h).
 */
#pragma once
#include <bcm_gpio_internal.h>
#include <stdio.h>

#define device_printf(dev, fmt, ...)  printf(fmt, ##__VA_ARGS__)
