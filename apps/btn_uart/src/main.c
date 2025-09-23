#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

// 注册一个日志模块
LOG_MODULE_REGISTER(Button, LOG_LEVEL_INF);

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define SW0_NODE    DT_ALIAS(sw2)
#if !DT_NODE_HAS_STATUS_OKAY(SW0_NODE)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
                                  {0});
static struct gpio_callback button_cb_data;


void button_pressed(const struct device *dev, struct gpio_callback *cb,
            uint32_t pins)
{
    LOG_INF("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}

int main(void)
{
    int ret;

    if (!gpio_is_ready_dt(&button)) {
        LOG_INF("Error: button device %s is not ready\n",
               button.port->name);
        return 0;
    }

    ret = gpio_pin_interrupt_configure_dt(&button,
                          GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        LOG_INF("Error %d: failed to configure interrupt on %s pin %d\n",
            ret, button.port->name, button.pin);
        return 0;
    }

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
    LOG_INF("Set up button at %s pin %d\n", button.port->name, button.pin);

    while (1) {
        k_msleep(1);
    }
}