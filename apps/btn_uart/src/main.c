#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(Button, LOG_LEVEL_INF);

/*
 * Get button configuration from the devicetree sw2 alias. This is mandatory.
 */
#define SW2_NODE   DT_ALIAS(sw2)

#if !DT_NODE_HAS_STATUS_OKAY(SW2_NODE)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW2_NODE, gpios,{0});
static struct gpio_callback button_cb_data;

void button_pressed(const struct device *dev, struct gpio_callback *cb,
            uint32_t pins)
{
    int val = gpio_pin_get(dev, button.pin);  
    LOG_INF("Button interrupt! pin=%s.%d, level=%d, time=%" PRIu32,
            button.port->name, button.pin, val, k_cycle_get_32());
}

int main(void)
{
    int ret;

    if (!gpio_is_ready_dt(&button)) {
        LOG_INF("Error: button device %s is not ready\n", button.port->name);
        return 0;
    }

    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_BOTH);
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


/**************************************************************************
 * \brief 

1. 进入工作目录
cd /home/c/workspace/lc_shizhanpai_zephyr/apps/btn_uart

2. 初次构建
west build -p always -b lc_shizhanpai/esp32s3/procpu -d build .

3. 再次编译
west build -b lc_shizhanpai/esp32s3/procpu -d build .

4. 烧录
west flash -d build

6. 工具
west espressif monitor
west build -t menuconfig
west build -t guiconfig

west build -t initlevels
**************************************************************************/