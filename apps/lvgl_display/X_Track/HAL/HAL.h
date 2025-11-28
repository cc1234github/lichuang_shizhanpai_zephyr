/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef __HAL_INTERNAL_H
#define __HAL_INTERNAL_H

// #include "App/Common/HAL/HAL.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/adc.h>

#include <zephyr/logging/log.h>
#include <lvgl.h>

// #include "Arduino.h"
#include "HAL_Config.h"
#include "CommonMacro.h"
#include "HAL_Def.h"

typedef enum {
    BTN_EVENT_NONE = 0,
    BTN_EVENT_CLICK,       
    BTN_EVENT_DOUBLE_CLICK, 
    BTN_EVENT_LONG_PRESS,   
    BTN_EVENT_LONG_PRESS_HOLD 
} BtnEvent_t;


void Encoder_Read(lv_indev_t *indev, lv_indev_data_t *data);
BtnEvent_t Encoder_GetBtnEvent(void);


#endif
