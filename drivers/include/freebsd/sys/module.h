/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * FreeBSD sys/module.h shim — re-exports from bcm_gpio_internal.h.
 *
 * driver_t, shim_driver_reg, and EARLY_DRIVER_MODULE all live in
 * bcm_gpio_internal.h so they are accessible from both include paths.
 */
#pragma once
#include <bcm_gpio_internal.h>
