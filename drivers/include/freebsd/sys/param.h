/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * FreeBSD sys/param.h shim — pulls in the standard C headers that
 * bcm2835_gpio.c expects to find transitively, and defines the few
 * FreeBSD constants used outside the bus/KOBJ subsystem.
 */
#pragma once
#include <string.h>
#include <strings.h>   /* ffs(), strcasecmp() */
#include <stdio.h>
#include <errno.h>
#include <sys/types.h> /* FreeBSD types (our shim) */

/* ENOTSUP: not universally defined by glibc errno.h */
#ifndef ENOTSUP
#  ifdef EOPNOTSUPP
#    define ENOTSUP EOPNOTSUPP
#  else
#    define ENOTSUP 95
#  endif
#endif

/* Silence "unused variable" warnings from the probe path */
#define bootverbose  0

/* Resource allocation flags */
#define RF_ACTIVE    1

/* Resource types */
#define SYS_RES_MEMORY  1
#define SYS_RES_IRQ     3

/* Mutex assertion flag (used by BCM_GPIO_LOCK_ASSERT) */
#define MA_OWNED   0

/* Bus probe return values */
#define BUS_PROBE_DEFAULT   0
#define BUS_PROBE_NOWILDCARD (-2000000000)

/* BUS_PASS_* constants for EARLY_DRIVER_MODULE */
#define BUS_PASS_INTERRUPT  (1 << 2)
#define BUS_PASS_ORDER_LATE 9

#ifndef nitems
#define nitems(x)  (sizeof((x)) / sizeof((x)[0]))
#endif
