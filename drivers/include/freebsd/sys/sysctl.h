/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * FreeBSD sys/sysctl.h shim — all sysctl operations are no-ops.
 *
 * SYSCTL_ADD_NODE  returns (struct sysctl_oid *)NULL  (result is assigned)
 * SYSCTL_ADD_PROC  returns (void)0                   (result is discarded)
 *
 * The bcm_gpio_sysctl_init() function will compile and link without error,
 * but will do nothing at runtime.
 */
#pragma once
#include <stddef.h>

/* Flag constants used by the driver */
#define OID_AUTO            (-1)
#define CTLFLAG_RD          0x80000000
#define CTLFLAG_RW          0x40000000
#define CTLTYPE_STRING      2
#define CTLFLAG_MPSAFE      0
#define CTLFLAG_NEEDGIANT   0

/* Opaque sysctl types */
struct sysctl_oid;
struct sysctl_oid_list;
struct sysctl_ctx_list;

struct sysctl_req {
    void *newptr;   /* NULL means read-only query */
};

/*
 * SYSCTL_HANDLER_ARGS: the macro used as the parameter list for
 * sysctl handler functions (bcm_gpio_func_proc).
 */
#define SYSCTL_HANDLER_ARGS \
    struct sysctl_oid *oidp __attribute__((unused)), \
    void *arg1, \
    size_t arg2 __attribute__((unused)), \
    struct sysctl_req *req

/* ADD_NODE: returns NULL (caller assigns result to a local variable) */
#define SYSCTL_ADD_NODE(ctx, parent, nbr, name, access, handler, descr) \
    ((struct sysctl_oid *)NULL)

/* ADD_PROC: result is discarded — cast to void explicitly */
#define SYSCTL_ADD_PROC(ctx, parent, nbr, name, access, ptr, arg, \
                        handler, fmt, descr) \
    ((void)0)

#define SYSCTL_CHILDREN(node)  ((struct sysctl_oid_list *)NULL)

static inline struct sysctl_ctx_list *
device_get_sysctl_ctx(void *dev __attribute__((unused))) { return NULL; }

static inline struct sysctl_oid *
device_get_sysctl_tree(void *dev __attribute__((unused))) { return NULL; }

static inline int
sysctl_handle_string(struct sysctl_oid *oidp __attribute__((unused)),
                     void *arg1 __attribute__((unused)),
                     size_t arg2 __attribute__((unused)),
                     struct sysctl_req *req __attribute__((unused)))
{
    return 0;
}
