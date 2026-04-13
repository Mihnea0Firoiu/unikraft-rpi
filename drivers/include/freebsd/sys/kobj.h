/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * FreeBSD sys/kobj.h shim — re-exports from bcm_gpio_internal.h.
 *
 * The actual struct/typedef/macro definitions live in bcm_gpio_internal.h
 * so they can be shared between the FreeBSD-side driver (compiled with the
 * isolated include path) and the shim-side files (kobj_descriptors.c,
 * bcm_gpio_shim.c) compiled with the normal Unikraft include path.
 */
#pragma once
#include <bcm_gpio_internal.h>
