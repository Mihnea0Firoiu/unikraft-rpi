/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * FreeBSD sys/kernel.h shim.
 *
 * KASSERT  → compile-away assertion (bare-metal: no kernel panic infra)
 * SYSINIT  → no-op (constructor-based init not needed here)
 */
#pragma once
#include <strings.h>   /* ffs() */

/* Suppress KASSERT in shim mode */
#define KASSERT(exp, msg)  do { (void)(exp); } while (0)

/* SYSINIT is a no-op — the shim uses __attribute__((constructor)) instead */
#define SYSINIT(ident, subsystem, order, func, arg)  /* nothing */

/* Convenient integer types used across the kernel */
#ifndef __bitcount32
#define __bitcount32(x) __builtin_popcount((unsigned)(x))
#endif
