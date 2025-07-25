#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

// 注册一个日志模块
LOG_MODULE_REGISTER(blinky, LOG_LEVEL_INF);

// 获取led0 的设备节点信息
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
	bool led_state = true;

	if (!gpio_is_ready_dt(&led)) {
		LOG_ERR("LED GPIO device not ready");
		return 0;
	}

	//配置引脚初始化状态
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	while (1) {
		gpio_pin_toggle_dt(&led);
		LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
		led_state = !led_state;
		k_msleep(500);// Zephyr 的内核延时函数，睡眠 500ms
		
	}
}
// build:   west build -p always -b esp32s3_devkitm/esp32s3/procpu app
// west espressif monitor
// west build -t menuconfig
