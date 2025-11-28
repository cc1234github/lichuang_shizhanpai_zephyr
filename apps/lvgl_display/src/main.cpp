/*
 * Copyright (c) 2015-2016 Wind River Systems, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file C++ Synchronization demo.  Uses basic C++ functionality.
 */

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>

#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

#include <zephyr/logging/log.h>
#include <lvgl.h>
#include <lvgl_input_device.h>

#include "DataProc.h"
#include "DataCenter.h"
#include "PageManager.h"
#include "AppFactory.h"
#include "ResourcePool.h"
#include "StatusBar/StatusBar.h"
#include "HAL.h"

LOG_MODULE_REGISTER(app,CONFIG_LOG_DEFAULT_LEVEL);
static const struct gpio_dt_spec backlight = GPIO_DT_SPEC_GET(DT_ALIAS(backlight), gpios);

/**************************************************************************
    \brief   
**************************************************************************/
static lv_indev_t *indev_encoder;
static lv_group_t *group;

/**************************************************************************
    \brief   
**************************************************************************/
void lv_port_indev_init(void)
{
    // 创建默认组
    group = lv_group_create();
    lv_group_set_default(group);

    // 注册编码器输入设备 (LVGL 9.x 新 API)
    indev_encoder = lv_indev_create();
    lv_indev_set_type(indev_encoder, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(indev_encoder, Encoder_Read);
    lv_indev_set_group(indev_encoder, group);

    LOG_INF("LVGL encoder input device registered");
}

#define ACCOUNT_SEND_CMD(ACT, CMD) \
do{ \
    DataProc::ACT##_Info_t info; \
    DATA_PROC_INIT_STRUCT(info); \
    info.cmd = DataProc::CMD; \
    DataProc::Center()->AccountMain.Notify(#ACT, &info, sizeof(info)); \
}while(0)




void gui_init() {

    const struct device *display_dev;
    static AppFactory factory;
    static PageManager manager(&factory);

    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return ;
	}
	LOG_INF("Display device %s is ready", display_dev->name);

    DataProc_Init();
    // ACCOUNT_SEND_CMD(Storage, STORAGE_CMD_LOAD);
    // ACCOUNT_SEND_CMD(SysConfig, SYSCONFIG_CMD_LOAD);

    lv_port_indev_init();

    lv_obj_t *scr = lv_scr_act();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    
    /* Set root default style */
    static lv_style_t rootStyle;
    lv_style_init(&rootStyle);
    lv_style_set_width(&rootStyle, LV_HOR_RES);
    lv_style_set_height(&rootStyle, LV_VER_RES);
    lv_style_set_bg_opa(&rootStyle, LV_OPA_COVER);
    lv_style_set_bg_color(&rootStyle, lv_color_black());

    manager.SetRootDefaultStyle(&rootStyle);
    ResourcePool::Init();
    Page::StatusBar_Create(lv_layer_top());
    manager.Install("Template",    "Pages/_Template");
    manager.Install("LiveMap",     "Pages/LiveMap");
    manager.Install("Dialplate",   "Pages/Dialplate");
    manager.Install("SystemInfos", "Pages/SystemInfos");
    manager.Install("Startup",     "Pages/Startup");
    manager.SetGlobalLoadAnimType(PageManager::LOAD_ANIM_OVER_TOP);
    manager.Push("Pages/Startup");

    gpio_pin_configure_dt(&backlight, GPIO_OUTPUT);
    gpio_pin_set(backlight.port, backlight.pin, 1);

	
	display_blanking_off(display_dev);
    lv_timer_handler();
}




/**************************************************************************
    \brief   
**************************************************************************/

#define STACKSIZE       4096
#define GUI_PRIORITY        5
#define DATA_PRIORITY       7

int main(void)
{
    HAL::HAL_Init();

    gui_init();
    return 0;
}

void gui_task(void)
{
    while(1) {
		
        lv_timer_handler();
        k_msleep(10);
    }
}

static void data_update(void)
{
    static uint32_t encoder_update_cnt = 0;
    static uint32_t power_update_cnt = 0;
    static uint32_t gps_status_update_cnt = 0;
    while (1) {
        HAL::GPS_Update();

        if(++encoder_update_cnt >= 10){
            encoder_update_cnt = 0;
            HAL::Encoder_Update();
        }

        if(++power_update_cnt >= 100){
            power_update_cnt = 0;
            HAL::Power_Update();
        }

        if(++gps_status_update_cnt >= 5000){
            gps_status_update_cnt = 0;
            // HAL::GPS_PrintStatus();
        }
        k_msleep(1);
    }
}

K_THREAD_DEFINE(gui_task_id, STACKSIZE, gui_task, NULL, NULL, NULL,
                GUI_PRIORITY, 0, 0);
K_THREAD_DEFINE(data_update_id, STACKSIZE, data_update, NULL, NULL, NULL,
                DATA_PRIORITY, 0, 0);


// west build -p always -b lc_shizhanpai/esp32s3/procpu apps/lvgl_display
// west espressif monitor
// west build -t menuconfig
// west build -t guiconfig
// west flash
// west build -t initlevels
