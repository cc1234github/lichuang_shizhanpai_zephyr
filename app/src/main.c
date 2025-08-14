/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#if 1
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <lvgl_input_device.h>
 
#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);
 
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));
static const struct device *spi3_dev = DEVICE_DT_GET(DT_NODELABEL(spi3));

#define PCA9557_ADDR        0x19
#define PCA9557_OUTPUT_PORT 0x01
#define PCA9557_CONFIG_PORT 0x03
#define LCD_CS_IO           0   // IO0 ���� CS

const struct device *i2c_dev;

static uint8_t pca_output = 0xFF; // ��ʼȫ��

/* ���� CS */
static void lcd_cs_low(void)
{
    pca_output &= ~(1 << LCD_CS_IO);
    i2c_reg_write_byte(i2c_dev, PCA9557_ADDR, PCA9557_OUTPUT_PORT, pca_output);
}

/* ���� CS */
static void lcd_cs_high(void)
{
    pca_output |= (1 << LCD_CS_IO);
    i2c_reg_write_byte(i2c_dev, PCA9557_ADDR, PCA9557_OUTPUT_PORT, pca_output);
}

/* ��ʼ�� PCA9557 IO */
static void pca9557_init(void)
{
    int ret;
    i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));
    if (!device_is_ready(i2c_dev)) {
        printk("I2C not ready\n");
        return;
    }

    // ȫ�������
    pca_output = 0xFF;
    ret = i2c_reg_write_byte(i2c_dev, PCA9557_ADDR, PCA9557_OUTPUT_PORT, pca_output);
    if (ret) {
        printk("Failed to set output: %d\n", ret);
    }

    // IO0 ���������Ĭ������
    ret = i2c_reg_write_byte(i2c_dev, PCA9557_ADDR, PCA9557_CONFIG_PORT, 0xFE);
    if (ret) {
        printk("Failed to set config: %d\n", ret);
    }
}

/* ���� LCD ����Զ����� CS */
static void lcd_write_cmd(uint8_t cmd)
{
    lcd_cs_low();
    // SPI д������������� spi_tx ������
    spi_write(spi3_dev, &cmd, 1);
    lcd_cs_high();
}

int main(void)
{
    pca9557_init();

    printk("Initializing LCD...\n");

    // LCD ��ʼ������
    lcd_write_cmd(0x01); // �����λ
    k_sleep(K_MSEC(10));
    lcd_write_cmd(0x11); // �˳�˯��ģʽ
    k_sleep(K_MSEC(120));

    printk("LCD initialized\n");

    // LVGL ��ʾʾ��
    const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        printk("Display not ready\n");
        return 0;
    }

    lv_init();
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello World!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    display_blanking_off(display_dev);

    while (1) {
        lv_timer_handler();
        k_sleep(K_MSEC(10));
    }
}
// west build -p always -b lc_shizhanpai/esp32s3/procpu app
// west espressif monitor
// west build -t menuconfig
// west build -t guiconfig
// west flash

#endif 


