#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

// 注册一个日志模块
LOG_MODULE_REGISTER(blinky, LOG_LEVEL_INF);

// 定义led0的设备节点信息
// #define LED0_NODE DT_ALIAS(led0)
// static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
	// bool led_state = true;

	// // 检查led0的设备节点是否存在
	// if (!gpio_is_ready_dt(&led)) {
	// 	LOG_ERR("LED GPIO device not ready");
	// 	return 0;
	// }

	// // 配置引脚为输出模式
	// gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	while (1) {
		// gpio_pin_toggle_dt(&led);
		//LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
		LOG_INF("hello world");
		// led_state = !led_state;
		k_msleep(500);// Zephyr 的内核延时函数，睡眠 500ms
		
	}
}
// west build -p always -b lc_shizhanpai/esp32s3/procpu app
// west espressif monitor
// west build -t menuconfig
