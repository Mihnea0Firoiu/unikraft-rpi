/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * bcm_gpio_internal.h — shared KOBJ/shim types for the BCM2835 GPIO driver.
 *
 * This header is the single source of truth for:
 *   - struct kobjop_desc / kobj_method_t / kobj_lookup()
 *   - DEVMETHOD / DEVMETHOD_END macros
 *   - struct shim_driver_reg / EARLY_DRIVER_MODULE macro
 *   - struct device / device_t
 *   - struct resource / resource_spec
 *   - bus function declarations
 *   - DEVICE_PROBE / DEVICE_ATTACH dispatch macros
 *   - extern declarations for all 24 kobjop_desc instances
 *
 * It uses ONLY standard C headers so it is includable from both:
 *   - shim-side files (kobj_descriptors.c, bcm_gpio_shim.c) compiled with
 *     the normal Unikraft include path
 *   - FreeBSD-side shim headers (sys/kobj.h, sys/bus.h, sys/module.h)
 *     compiled with the isolated FreeBSD shim include path
 *
 * IMPORTANT: Do NOT include any Unikraft-specific or FreeBSD-specific
 * headers here — only <stdint.h>, <stddef.h>.
 */
#ifndef _BCM_GPIO_INTERNAL_H_
#define _BCM_GPIO_INTERNAL_H_

#include <stdint.h>
#include <stddef.h>

/* =========================================================
 * KOBJ method descriptor and table
 * ========================================================= */

struct kobjop_desc {
    const char *name;
    void       *default_func;
};

typedef struct kobj_method {
    const struct kobjop_desc *desc;
    void                     *func;
} kobj_method_t;

typedef kobj_method_t device_method_t;

#define DEVMETHOD_END   { NULL, NULL }
#define DEVMETHOD(method, func)  { &method##_desc, (void *)(func) }

static inline void *
kobj_lookup(const kobj_method_t *methods, const struct kobjop_desc *desc)
{
    const kobj_method_t *m;
    for (m = methods; m->desc != NULL; m++)
        if (m->desc == desc)
            return m->func;
    return desc->default_func;
}

/* =========================================================
 * driver_t — the FreeBSD driver descriptor
 * ========================================================= */

typedef struct driver {
    const char          *name;
    const kobj_method_t *methods;
    size_t               size;
} driver_t;

/* =========================================================
 * shim_driver_reg — written by constructor, read by uk_gpio_init
 * ========================================================= */

struct shim_driver_reg {
    const kobj_method_t *sdr_methods;
    size_t               sdr_softc_size;
};

extern struct shim_driver_reg shim_gpio_driver_reg;

#define EARLY_DRIVER_MODULE(name, busname, driver_var, evh, arg, pass)  \
    static void __attribute__((constructor))                             \
    __shim_register_##name(void) {                                       \
        shim_gpio_driver_reg.sdr_methods    = (driver_var).methods;     \
        shim_gpio_driver_reg.sdr_softc_size = (driver_var).size;        \
    }

/* =========================================================
 * device_t — minimal struct for KOBJ dispatch
 * ========================================================= */

struct device {
    void                   *d_softc;
    const kobj_method_t    *d_methods;
    const char             *d_nameunit;
};
typedef struct device *device_t;

static inline void *
device_get_softc(device_t dev) { return dev ? dev->d_softc : NULL; }

static inline void
device_set_desc(device_t dev __attribute__((unused)),
                const char *desc __attribute__((unused))) {}

static inline const char *
device_get_nameunit(device_t dev)
{
    return (dev && dev->d_nameunit) ? dev->d_nameunit : "gpio0";
}

/* =========================================================
 * struct resource / resource_spec — MMIO and IRQ resources
 * ========================================================= */

#define SYS_RES_MEMORY  1
#define SYS_RES_IRQ     3
#define RF_ACTIVE       1

typedef uintptr_t bus_space_tag_t;
typedef uintptr_t bus_space_handle_t;

struct resource {
    bus_space_tag_t    r_bustag;
    bus_space_handle_t r_bushandle;
    int                r_type;
    int                r_rid;
    uintptr_t          r_start;
};

struct resource_spec {
    int type;
    int rid;
    int flags;
};

static inline bus_space_tag_t
rman_get_bustag(struct resource *r) { return r->r_bustag; }

static inline bus_space_handle_t
rman_get_bushandle(struct resource *r) { return r->r_bushandle; }

/* =========================================================
 * Bus function declarations (defined in bcm_gpio_shim.c)
 * ========================================================= */

typedef int  (*driver_filter_t)(void *);
typedef void (*driver_intr_t)(void *);

#define INTR_TYPE_MISC  0
#define INTR_MPSAFE     0

int  bus_alloc_resources(device_t dev, struct resource_spec *spec,
                         struct resource **res);
void bus_release_resources(device_t dev, struct resource_spec *spec,
                           struct resource **res);
int  bus_setup_intr(device_t dev, struct resource *r, int flags,
                    driver_filter_t filter, driver_intr_t ithread,
                    void *arg, void **cookiep);
int  bus_teardown_intr(device_t dev, struct resource *r, void *cookie);

static inline void
bus_attach_children(device_t dev __attribute__((unused))) {}

static inline int
bus_generic_setup_intr(device_t dev __attribute__((unused)),
                       struct resource *r __attribute__((unused)),
                       int flags __attribute__((unused)),
                       driver_filter_t filter __attribute__((unused)),
                       driver_intr_t ithread __attribute__((unused)),
                       void *arg __attribute__((unused)),
                       void **cookiep __attribute__((unused)))
{ return 0; }

static inline int
bus_generic_teardown_intr(device_t dev __attribute__((unused)),
                          struct resource *r __attribute__((unused)),
                          void *cookie __attribute__((unused)))
{ return 0; }

/* =========================================================
 * KOBJ dispatch macros
 * ========================================================= */

extern struct kobjop_desc device_probe_desc;
extern struct kobjop_desc device_attach_desc;
extern struct kobjop_desc device_detach_desc;
extern struct kobjop_desc bus_setup_intr_desc;
extern struct kobjop_desc bus_teardown_intr_desc;
extern struct kobjop_desc gpio_get_bus_desc;
extern struct kobjop_desc gpio_pin_max_desc;
extern struct kobjop_desc gpio_pin_getname_desc;
extern struct kobjop_desc gpio_pin_getflags_desc;
extern struct kobjop_desc gpio_pin_getcaps_desc;
extern struct kobjop_desc gpio_pin_setflags_desc;
extern struct kobjop_desc gpio_pin_get_desc;
extern struct kobjop_desc gpio_pin_set_desc;
extern struct kobjop_desc gpio_pin_toggle_desc;
extern struct kobjop_desc pic_disable_intr_desc;
extern struct kobjop_desc pic_enable_intr_desc;
extern struct kobjop_desc pic_map_intr_desc;
extern struct kobjop_desc pic_post_filter_desc;
extern struct kobjop_desc pic_post_ithread_desc;
extern struct kobjop_desc pic_pre_ithread_desc;
extern struct kobjop_desc pic_setup_intr_desc;
extern struct kobjop_desc pic_teardown_intr_desc;
extern struct kobjop_desc ofw_bus_get_node_desc;
extern struct kobjop_desc fdt_pinctrl_configure_desc;

#define DEVICE_PROBE(dev)  \
    (((int (*)(device_t)) \
        kobj_lookup((dev)->d_methods, &device_probe_desc))(dev))

#define DEVICE_ATTACH(dev) \
    (((int (*)(device_t)) \
        kobj_lookup((dev)->d_methods, &device_attach_desc))(dev))

/* =========================================================
 * struct thread / curthread stub (used by intr_isrc_dispatch)
 * ========================================================= */

struct thread {
    void *td_intr_frame;
};

extern struct thread _shim_thread;
#define curthread  (&_shim_thread)

#endif /* _BCM_GPIO_INTERNAL_H_ */
