/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * FreeBSD sys/bus_space.h shim.
 *
 * bus_space_read_4 / bus_space_write_4 → volatile 32-bit MMIO.
 * On Unikraft/RPi3 the peripheral physical address space is identity-mapped,
 * so the bus_space_handle_t IS the virtual address.
 */
#pragma once
#include <stdint.h>

typedef uintptr_t bus_space_tag_t;
typedef uintptr_t bus_space_handle_t;

static inline uint32_t
bus_space_read_4(bus_space_tag_t t __attribute__((unused)),
                 bus_space_handle_t h,
                 unsigned long offset)
{
    return *(volatile uint32_t *)(h + offset);
}

static inline void
bus_space_write_4(bus_space_tag_t t __attribute__((unused)),
                  bus_space_handle_t h,
                  unsigned long offset,
                  uint32_t val)
{
    *(volatile uint32_t *)(h + offset) = val;
}
