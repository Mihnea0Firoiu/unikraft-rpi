/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * FreeBSD machine/bus.h shim — redirects to sys/bus_space.h.
 * The driver includes <machine/bus.h> to get bus_space_* types on ARM.
 */
#pragma once
#include <sys/bus_space.h>
