/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Shim: include the real glibc <sys/cdefs.h> via #include_next so that
 * downstream system headers (string.h, stdio.h, …) still find it, then
 * overlay the FreeBSD-specific macros that bcm2835_gpio.c relies on.
 */
#pragma once
#include_next <sys/cdefs.h>

#ifndef __FBSDID
#define __FBSDID(s)   /* nothing */
#endif

#ifndef __unused
#define __unused        __attribute__((__unused__))
#endif

#ifndef __packed
#define __packed        __attribute__((__packed__))
#endif

#ifndef __aligned
#define __aligned(x)    __attribute__((__aligned__(x)))
#endif

#ifndef __printflike
#define __printflike(fmtarg, firstvararg)
#endif

#ifndef __nonnull
#define __nonnull(args)
#endif

#ifndef __pure2
#define __pure2         __attribute__((__const__))
#endif

#ifndef __dead2
#define __dead2         __attribute__((__noreturn__))
#endif
