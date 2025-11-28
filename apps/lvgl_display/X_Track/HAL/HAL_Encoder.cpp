#include "HAL.h"

/**************************************************************************
 * 日志模块
**************************************************************************/
LOG_MODULE_REGISTER(halEncoder, CONFIG_LOG_DEFAULT_LEVEL);

// static const struct gpio_dt_spec encoderA = GPIO_DT_SPEC_GET(DT_NODELABEL(encoder_a), gpios);
// static const struct gpio_dt_spec encoderB = GPIO_DT_SPEC_GET(DT_NODELABEL(encoder_b), gpios);
static const struct gpio_dt_spec encoderBtn = GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios);

static struct gpio_callback encoderACbData;
static volatile int32_t encoderCount = 0;

/**************************************************************************
 * 按钮事件和状态定义
**************************************************************************/

typedef enum {
    BTN_STATE_IDLE = 0,
    BTN_STATE_PRESSED,
    BTN_STATE_WAIT_RELEASE,
    BTN_STATE_WAIT_DOUBLE,
    BTN_STATE_LONG_PRESS
} BtnState_t;

typedef struct {
    BtnState_t state;
    BtnEvent_t event;
    int64_t pressTime;
    int64_t releaseTime;
    uint8_t clickCount;
    bool isPressed;
} BtnCtrl_t;


static BtnCtrl_t btnCtrl = {BTN_STATE_IDLE, BTN_EVENT_NONE, 0, 0, 0, false};

#define BTN_DEBOUNCE_MS       200       // 100ms
#define BTN_LONG_PRESS_MS     3000      // 5000ms
#define BTN_DOUBLE_CLICK_MS   1000      // 2000ms

/**************************************************************************
 * 按钮状态机
**************************************************************************/
static void btnStateMachine(void)
{
    int64_t now = k_uptime_get();
    int64_t pressDuration = now - btnCtrl.pressTime;
    int64_t releaseDuration = now - btnCtrl.releaseTime;

    if (pressDuration < BTN_DEBOUNCE_MS || releaseDuration < BTN_DEBOUNCE_MS) {
        return;
    }
    btnCtrl.isPressed = gpio_pin_get_dt(&encoderBtn);

    switch (btnCtrl.state) {
    case BTN_STATE_IDLE:
        if (btnCtrl.isPressed) {
            btnCtrl.state = BTN_STATE_PRESSED;
            btnCtrl.pressTime = now;
            btnCtrl.event = BTN_EVENT_NONE;
        }
        break;

    case BTN_STATE_PRESSED:
        if (!btnCtrl.isPressed) {
            btnCtrl.releaseTime = now;
            btnCtrl.clickCount++;
            btnCtrl.state = BTN_STATE_WAIT_DOUBLE;
        } else if (pressDuration >= BTN_LONG_PRESS_MS) {
            btnCtrl.event = BTN_EVENT_LONG_PRESS;
            btnCtrl.state = BTN_STATE_LONG_PRESS;
            // LOG_INF("Button: LONG PRESS");
        }
        break;

    case BTN_STATE_WAIT_DOUBLE:
        if (btnCtrl.isPressed) {
            // 检测到第二次按下
            if (releaseDuration < BTN_DOUBLE_CLICK_MS) {
                btnCtrl.state = BTN_STATE_WAIT_RELEASE;
            } else {
                // 超时，视为单击
                btnCtrl.event = BTN_EVENT_CLICK;
                btnCtrl.state = BTN_STATE_PRESSED;
                btnCtrl.pressTime = now;
                btnCtrl.clickCount = 0;
                // LOG_INF("Button: CLICK");
            }
        } else if (releaseDuration >= BTN_DOUBLE_CLICK_MS) {
            // 等待超时，确认为单击
            btnCtrl.event = BTN_EVENT_CLICK;
            btnCtrl.state = BTN_STATE_IDLE;
            btnCtrl.clickCount = 0;
            // LOG_INF("Button: CLICK");
        }
        break;

    case BTN_STATE_WAIT_RELEASE:
        if (!btnCtrl.isPressed) {
            // 双击确认
            btnCtrl.event = BTN_EVENT_DOUBLE_CLICK;
            btnCtrl.state = BTN_STATE_IDLE;
            btnCtrl.clickCount = 0;
            // LOG_INF("Button: DOUBLE CLICK");
        }
        break;

    case BTN_STATE_LONG_PRESS:
        if (!btnCtrl.isPressed) {
            // 长按释放
            btnCtrl.state = BTN_STATE_IDLE;
            btnCtrl.event = BTN_EVENT_NONE;
            // LOG_INF("Button: LONG PRESS Released");
        } else {
            // 长按保持
            btnCtrl.event = BTN_EVENT_LONG_PRESS_HOLD;
        }
        break;
    }
}

/**************************************************************************
 * 编码器A中断服务函数
**************************************************************************/
void encoderAIsr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    // int bLevel = gpio_pin_get_raw(encoderB.port, encoderB.pin);
    
    // if (bLevel == 0) {
    //     encoderCount++; 
    // } else {
    //     encoderCount--; 
    // }
}

/**************************************************************************
 * 编码器初始化
**************************************************************************/
int HAL::Encoder_Init(void)
{
    int ret;

    // if (!gpio_is_ready_dt(&encoderA)) {
    //     LOG_ERR("Encoder A GPIO not ready");
    //     return -ENODEV;
    // }
    // if (!gpio_is_ready_dt(&encoderB)) {
    //     LOG_ERR("Encoder B GPIO not ready");
    //     return -ENODEV;
    // }
    if (!gpio_is_ready_dt(&encoderBtn)) {
        LOG_ERR("Encoder Button GPIO not ready");
        return -ENODEV;
    }

    // ret = gpio_pin_configure_dt(&encoderA, GPIO_INPUT);
    // if (ret != 0) {
    //     LOG_ERR("Failed to configure encoder A: %d", ret);
    //     return ret;
    // }
    // ret = gpio_pin_configure_dt(&encoderB, GPIO_INPUT);
    // if (ret != 0) {
    //     LOG_ERR("Failed to configure encoder B: %d", ret);
    //     return ret;
    // }
    ret = gpio_pin_configure_dt(&encoderBtn, GPIO_INPUT);
    if (ret != 0) {
        LOG_ERR("Failed to configure encoder button: %d", ret);
        return ret;
    }

    // ret = gpio_pin_interrupt_configure_dt(&encoderA, GPIO_INT_EDGE_FALLING);
    // if (ret != 0) {
    //     LOG_ERR("Failed to configure encoder A interrupt: %d", ret);
    //     return ret;
    // }

    // gpio_init_callback(&encoderACbData, encoderAIsr, BIT(encoderA.pin));

    // ret = gpio_add_callback(encoderA.port, &encoderACbData);
    // if (ret != 0) {
    //     LOG_ERR("Failed to add encoder A callback: %d", ret);
    //     return ret;
    // }

    LOG_INF("Encoder initialized successfully");
    return 0;
}

/**************************************************************************
 * 编码器按钮更新（在线程中定期调用）
**************************************************************************/
void HAL::Encoder_Update(void)
{
    
    btnStateMachine();
}

/**************************************************************************
 * LVGL输入设备读取函数
**************************************************************************/
void Encoder_Read(lv_indev_t *indev, lv_indev_data_t *data)
{
    if(btnCtrl.event == BTN_EVENT_CLICK){
        data->enc_diff = 1;
        btnCtrl.event = BTN_EVENT_NONE;
    }else if(btnCtrl.event == BTN_EVENT_DOUBLE_CLICK){
        data->enc_diff = -1;
        btnCtrl.event = BTN_EVENT_NONE;
    }else if(btnCtrl.event == BTN_EVENT_NONE){
        data->enc_diff = 0;
    }

    if(btnCtrl.event == BTN_EVENT_LONG_PRESS){
        data->state = LV_INDEV_STATE_PRESSED;
    }else{
        data->state =  LV_INDEV_STATE_RELEASED;
    }

}

// /**************************************************************************
//  * 获取按钮事件
// **************************************************************************/
BtnEvent_t Encoder_GetBtnEvent(void)
{
    // BtnEvent_t event = btnCtrl.event;
    return BTN_EVENT_NONE;
}
