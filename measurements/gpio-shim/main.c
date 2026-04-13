/* GPIO shim smoke-test for Unikraft on Raspberry Pi 3
 *
 * Tests:
 *   1. Init   — run DEVICE_PROBE + DEVICE_ATTACH via KOBJ shim
 *   2. Output — set GPIO 4 (pin 7) as output and drive it high then low
 *   3. Input  — read GPIO 17 (pin 11) with pull-up enabled
 *   4. Alt fn — set GPIO 11 (SPI0 SCLK) to ALT0
 */

#include <stdio.h>
#include <uk/print.h>
#include <uk/gpio.h>

int main(void)
{
    int ret, level;

    setvbuf(stdout, NULL, _IONBF, 0);
    printf("=== GPIO shim smoke-test start ===\n");

    /* 1. Init */
    ret = uk_gpio_init();
    if (ret != 0) {
        uk_pr_err("FAIL: uk_gpio_init() = %d\n", ret);
        return 1;
    }
    printf("PASS: GPIO driver initialised\n");

    /* 2. Output: drive GPIO 4 high */
    uk_gpio_set_func(4, UK_GPIO_FUNC_OUTPUT);
    uk_gpio_set(4, 1);
    printf("INFO: GPIO 4 set to OUTPUT HIGH\n");

    /* Drive low */
    uk_gpio_set(4, 0);
    printf("INFO: GPIO 4 set LOW\n");

    /* 3. Input: read GPIO 17 with pull-up */
    uk_gpio_set_func(17, UK_GPIO_FUNC_INPUT);
    uk_gpio_set_pud(17, UK_GPIO_PUD_UP);
    level = uk_gpio_get(17);
    printf("INFO: GPIO 17 = %d (expected 1 with pull-up, no load)\n", level);

    /* 4. Alt function: GPIO 11 → SPI0 SCLK (ALT0) */
    uk_gpio_set_func(11, UK_GPIO_FUNC_ALT0);
    printf("INFO: GPIO 11 configured as ALT0 (SPI0 SCLK)\n");

    printf("=== GPIO shim smoke-test done ===\n");
    return 0;
}
