
#include "HAL.h"

/**************************************************************************
    \brief  
**************************************************************************/
LOG_MODULE_REGISTER(hal_power, CONFIG_LOG_DEFAULT_LEVEL);

// #define POWERON_NODE DT_ALIAS(poweron)
// #define BAT_CH_NODE DT_ALIAS(bat_ch)
// static const struct gpio_dt_spec PowerOn_DT = GPIO_DT_SPEC_GET(POWERON_NODE, gpios);
// static const struct gpio_dt_spec BatCh_DT = GPIO_DT_SPEC_GET(BAT_CH_NODE, gpios);

// static const struct adc_dt_spec battery_adc = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);  // 通道5
// static const struct adc_dt_spec vdd_adc     = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 1);  // 通道1

typedef struct {
    bool is_Poweron;
    bool is_Charge;
    int BatteryVoltage_mV;
    int VddVoltage_mV;
} Power_Ctrl_t;
static Power_Ctrl_t powerCtrl = {false, false, 0};
/**************************************************************************
    \brief  
**************************************************************************/

void Power_Off(void)
{
    //gpio_pin_set_dt(&PowerOn_DT, 0);
}

void Power_On(void)
{
    //gpio_pin_set_dt(&PowerOn_DT, 1);
}

void HAL::Power_Init(void)
{
    // int err;

    // if (!gpio_is_ready_dt(&PowerOn_DT)) {
    //     LOG_ERR("Power on gpio not ready");
	// 	return ;
    // }

    // if (!gpio_is_ready_dt(&BatCh_DT)) {
    //     LOG_ERR("Battery charge gpio not ready");
	// 	return ;
	// }

    // if (!adc_is_ready_dt(&battery_adc)) {
    //     LOG_ERR("ADC controller device %s not ready", battery_adc.dev->name);
    //     return;
	// }
    // err = adc_channel_setup_dt(&battery_adc);
    // if (err < 0) {
    //     LOG_ERR("Could not setup channel battery_adc");
    //     return;
    // }

    // if (!adc_is_ready_dt(&vdd_adc)) {
	// 		LOG_ERR("ADC controller device %s not ready\n", vdd_adc.dev->name);
	// 		return;
	// 	}
    // err = adc_channel_setup_dt(&vdd_adc);
    // if (err < 0) {
    //     LOG_ERR("Could not setup channel vdd_adc");
    //     return;
    // }

    // if(gpio_pin_get_dt(&BatCh_DT)){
    //     powerCtrl.is_Charge = true;
    // } else {
    //     powerCtrl.is_Charge = false;
    // }
    // gpio_pin_configure_dt(&PowerOn_DT, GPIO_OUTPUT_ACTIVE);
    // Power_On();
    // powerCtrl.is_Poweron = true;
    LOG_INF("Power initialized OK, isPoweron: %d, isCharging: %d", (int)powerCtrl.is_Poweron, (int)powerCtrl.is_Charge);
}

int read_battery_voltage() {
    uint16_t buf = 0;
    int32_t val_mv = 0;
    // int err = 0;
    
    // struct adc_sequence sequence = {
    //     .buffer = &buf,
    //     .buffer_size = sizeof(buf),
    // };
    
    // adc_sequence_init_dt(&battery_adc, &sequence);
    
    // err = adc_read_dt(&battery_adc, &sequence);
    // if (err < 0) {
    //     LOG_ERR("Could not read ADC (%d)", err);
    //     return -1;
    // }
    
    // val_mv = (int32_t)buf;
    // adc_raw_to_millivolts_dt(&battery_adc, &val_mv);
    // LOG_INF("Battery ADC raw: %d, voltage: %d mV", buf, val_mv);
    return val_mv;
}

int read_vdd_voltage() {
    uint16_t buf = 0;
    int32_t val_mv = 0;
    // int err = 0;
    
    // struct adc_sequence sequence = {
    //     .buffer = &buf,
    //     .buffer_size = sizeof(buf),
    // };
    
    // adc_sequence_init_dt(&vdd_adc, &sequence);
    // err = adc_read_dt(&vdd_adc, &sequence);
    // if (err < 0) {
    //     LOG_ERR("Could not read ADC (%d)", err);
    //     return -1;
    // }

    // val_mv = (int32_t)buf;
    // adc_raw_to_millivolts_dt(&vdd_adc, &val_mv);

    // LOG_INF("Vdd Voltage: %d mV", val_mv);
    return val_mv;
}

int map_voltage_to_percent(int mv)
{
    const int min_mv = 1500;  // 3.0V = 0%
    const int max_mv = 2100;  // 4.2V = 100%

    if (mv <= min_mv) return 0;
    if (mv >= max_mv) return 100;

    return (mv - min_mv) * 100 / (max_mv - min_mv);
}


void HAL::Power_GetInfo(Power_Info_t* info)
{
    // info->voltage = powerCtrl.BatteryVoltage_mV;
    // info->usage = map_voltage_to_percent(powerCtrl.BatteryVoltage_mV);
    // info->isCharging = powerCtrl.is_Charge;
}



void HAL::Power_Update()
{
    // powerCtrl.BatteryVoltage_mV = read_battery_voltage();
    // powerCtrl.VddVoltage_mV = read_vdd_voltage();
    // powerCtrl.is_Charge = (gpio_pin_get_dt(&BatCh_DT));

    // if(powerCtrl.is_Poweron){
    //     if(Encoder_GetBtnEvent()==BTN_EVENT_LONG_PRESS_HOLD){
    //         Power_Off();
            
    //         powerCtrl.is_Poweron = false;
    //         LOG_INF("Power off by long press hold");
    //     }
    // }else{
    //     if(Encoder_GetBtnEvent()==BTN_EVENT_CLICK){
    //         Power_On();
    //         powerCtrl.is_Poweron = true;
    //         LOG_INF("Power on by click");
    //     }
    // }
    
    // LOG_INF("Battery Voltage: %d mV, Vdd Voltage: %d mV, isCharging: %d, isPoweron: %d",
    //         powerCtrl.BatteryVoltage_mV, powerCtrl.VddVoltage_mV, (int)powerCtrl.is_Charge, (int)powerCtrl.is_Poweron);
}



// void HAL::Power_Update(void)
// {
    
// }