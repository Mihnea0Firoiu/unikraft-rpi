# BCM2835/7 GPIO Driver — FreeBSD KOBJ Shim

## Overview

Ports the **unmodified** FreeBSD `bcm2835_gpio.c` driver to Unikraft/RPi3 via
a minimal KOBJ compatibility shim.  No edits to the upstream FreeBSD source
are required.

## Architecture

```
  ┌──────────────────────────────────────────────┐
  │  Application  (measurements/gpio-shim/main.c) │
  └───────────────────┬──────────────────────────┘
                      │  uk_gpio_init / uk_gpio_set / uk_gpio_get / …
  ┌───────────────────▼──────────────────────────┐
  │  drivers/bcm_gpio_shim.c   (Unikraft API)     │
  │  drivers/kobj_descriptors.c  (KOBJ tables)    │
  └───────────────────┬──────────────────────────┘
                      │  DEVICE_PROBE / DEVICE_ATTACH (via kobj_lookup)
  ┌───────────────────▼──────────────────────────┐
  │  drivers/freebsd/bcm2835_gpio.c  (unmodified) │
  └──────────────────────────────────────────────┘
```

Key bridging header: **`drivers/include/bcm_gpio_internal.h`** — the single
source of truth for all types shared between the Unikraft-side shim files
and the FreeBSD include-path headers.

## File Layout

```
drivers/
  include/
    bcm_gpio_internal.h      — shared types: KOBJ, bus, resource, device, …
    uk/gpio.h                — public Unikraft GPIO API
    freebsd/                 — FreeBSD shim headers (isolated include path)
      sys/
        cdefs.h kernel.h kobj.h bus.h module.h rman.h proc.h
        param.h gpio.h sysctl.h interrupt.h lock.h mutex.h systm.h
      machine/
        bus.h intr.h
      dev/
        ofw/ofw_bus.h ofw_bus_subr.h
        gpio/gpio.h gpiobusvar.h
        fdt/fdt_pinctrl.h
      opt_platform.h
  freebsd/
    bcm2835_gpio.c           — upstream FreeBSD driver (unmodified)
    gpio_if.h                — GPIO KOBJ interface declarations
    pic_if.h                 — PIC KOBJ interface declarations
  bcm_gpio_shim.c            — bus_alloc_resources, bus_setup_intr, uk_gpio_*
  kobj_descriptors.c         — 24 kobjop_desc instances + shim_gpio_driver_reg
```

## Kconfig

Enable the driver in your defconfig or Kconfig fragment:

```
CONFIG_LIBBCM2835_GPIO=y
```

Requires `CONFIG_ARCH_ARM_64=y`.

## Build

```sh
cd measurements/build
bash build-gpio-shim.sh
```

Output: `measurements/build/kernel8.img`

## Public API (`<uk/gpio.h>`)

```c
int  uk_gpio_init(void);
void uk_gpio_set_func(unsigned int pin, unsigned int func);
void uk_gpio_set(unsigned int pin, int val);
int  uk_gpio_get(unsigned int pin);
void uk_gpio_set_pud(unsigned int pin, unsigned int pud);
```

Function selector constants: `UK_GPIO_FUNC_INPUT`, `UK_GPIO_FUNC_OUTPUT`,
`UK_GPIO_FUNC_ALT0` … `UK_GPIO_FUNC_ALT5`.

Pull-up/down constants: `UK_GPIO_PUD_OFF`, `UK_GPIO_PUD_DOWN`,
`UK_GPIO_PUD_UP`.

## Hardware Test (RPi 3B / 3B+)

### Loopback wiring

```
GPIO 4  (physical pin 7)  ──jumper──  GPIO 17 (physical pin 11)
```

The test app drives GPIO 4 LOW and reads GPIO 17 through the pull-up; a HIGH
reading confirms the pull-up works, a LOW reading confirms the output path.

### UART / serial console

Connect a **3.3 V** USB-to-UART adapter:

| Adapter | RPi header | Signal |
|---------|-----------|--------|
| GND     | Pin 6     | Ground |
| TX      | Pin 10    | GPIO 15 / RXD |
| RX      | Pin 8     | GPIO 14 / TXD |
| VCC     | Pin 1     | **3.3 V only** |

> ⚠️  **Do not use a 5 V adapter or connect to the 5 V header pin.**
> The RPi 3 GPIO lines are 3.3 V tolerant.  Most CP2102 / CH340G adapters
> have a VCC selection jumper — verify it is set to 3.3 V before connecting.

Open a terminal at **115200 8N1**.  Expected output:

```
PASS: GPIO driver initialised
INFO: GPIO 17 = 0
```

## Design Notes

### KOBJ dispatch without modifying FreeBSD source

`EARLY_DRIVER_MODULE` expands to `__attribute__((constructor))`, so
`bcm_gpio_driver_methods` is registered into `shim_gpio_driver_reg` before
`uk_main()`.  `uk_gpio_init()` reads that table, allocates `softc` via
`uk_calloc`, then calls `DEVICE_PROBE` / `DEVICE_ATTACH` through
`kobj_lookup()`.  `bcm_gpio_probe` and `bcm_gpio_attach` remain `static`
inside the FreeBSD file; no symbol-level references are needed.

### OFW / FDT without a real device tree

`OF_getencprop_alloc_multi()` returns `-1` (no FDT), causing
`bcm_gpio_get_reserved_pins()` to take the early-return path and return `0`
(success with zero read-only pins).  BCM2835 peripheral base (`0x3F200000`)
and IRQ numbers (bank0 = 49, bank1 = 50) are hardcoded in
`drivers/include/freebsd/dev/ofw/ofw_bus_subr.h`.

### Include-path isolation

The FreeBSD driver is compiled with its own isolated include path
(`-I drivers/include/freebsd …`) so that shim headers intercept every
`#include <sys/...>` before the system headers are searched.  Shim-side C
files (`bcm_gpio_shim.c`, `kobj_descriptors.c`) are compiled with the normal
Unikraft path and include only `<bcm_gpio_internal.h>` for shared types.
