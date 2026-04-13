/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * FreeBSD sys/mutex.h shim.
 *
 * mtx_lock_spin / mtx_unlock_spin  → save/restore IRQ flags (critical section).
 * On AArch64 Unikraft we use local_irq_save / local_irq_restore assembled
 * inline.  The flags are stored inside struct mtx so the matching unlock
 * can restore exactly the state captured by lock.
 */
#pragma once
#include <stdint.h>

#define MTX_SPIN    0x1
#define MTX_DEF     0x0

struct mtx {
    unsigned long irq_flags;
    int           initialised;
};

/* Save DAIF and disable IRQs (AArch64) */
static inline unsigned long __mtx_save_irqf(void)
{
    unsigned long flags;
    __asm__ volatile(
        "mrs %0, daif\n\t"
        "msr daifset, #2\n\t"   /* set IRQ mask bit */
        : "=r"(flags)
        :
        : "memory");
    return flags;
}

static inline void __mtx_restore_irqf(unsigned long flags)
{
    __asm__ volatile(
        "msr daif, %0\n\t"
        :
        : "r"(flags)
        : "memory");
}

static inline void mtx_init(struct mtx *m,
    const char *name __attribute__((unused)),
    const char *type __attribute__((unused)),
    int opts __attribute__((unused)))
{
    m->irq_flags   = 0;
    m->initialised = 1;
}

static inline void mtx_destroy(struct mtx *m) { m->initialised = 0; }

static inline void mtx_lock_spin(struct mtx *m)
{
    m->irq_flags = __mtx_save_irqf();
}

static inline void mtx_unlock_spin(struct mtx *m)
{
    __mtx_restore_irqf(m->irq_flags);
}

/* MA_OWNED assertion — no-op in shim */
#define mtx_assert(m, what)  do { (void)(m); (void)(what); } while (0)
