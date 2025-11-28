#include "HAL.h"
#include "minmea.h"
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

LOG_MODULE_REGISTER(hal_gps, CONFIG_LOG_DEFAULT_LEVEL);

// static const struct device *gps_uart = DEVICE_DT_GET(DT_ALIAS(gps_uart));

#define GPS_RX_BUF_SIZE 256
#define NMEA_LINE_MAX 128

// 消息队列
static uint8_t gps_rx_buf[GPS_RX_BUF_SIZE];
static struct k_msgq gps_msgq;

// NMEA 行缓冲
static char nmea_line[NMEA_LINE_MAX];
static int nmea_line_idx = 0;

// GPS 数据缓存
static struct {
    // RMC 数据
    struct minmea_sentence_rmc rmc;
    bool rmc_valid;
    uint32_t rmc_timestamp;
    
    // GGA 数据
    struct minmea_sentence_gga gga;
    bool gga_valid;
    uint32_t gga_timestamp;
    
    // GSV 数据 (卫星信息)
    struct minmea_sentence_gsv gsv;
    bool gsv_valid;
    
    // 统计
    uint32_t total_sentences;
    uint32_t valid_sentences;
    uint32_t invalid_sentences;
} gps_data = {0};

static bool gps_initialized = false;

/**************************************************************************
    \brief   UART 接收中断回调
**************************************************************************/
static void gps_uart_callback(const struct device *dev, void *user_data)
{
    uint8_t c;
    
    // if (!uart_irq_update(gps_uart)) {
    //     return;
    // }

    // if (uart_irq_rx_ready(gps_uart)) {
    //     while (uart_fifo_read(gps_uart, &c, 1) == 1) {
    //         k_msgq_put(&gps_msgq, &c, K_NO_WAIT);
    //     }
    // }
}

/**************************************************************************
    \brief   处理完整的 NMEA 句子
**************************************************************************/
static void process_nmea_line(const char *line)
{
    LOG_INF("NMEA: %s", line);
    gps_data.total_sentences++;
    
    // 识别句子类型
    enum minmea_sentence_id id = minmea_sentence_id(line, false);
    
    switch (id) {
        case MINMEA_SENTENCE_RMC: {
            // 解析 RMC (推荐最小定位信息)
            struct minmea_sentence_rmc frame;
            if (minmea_parse_rmc(&frame, line)) {
                gps_data.rmc = frame;
                gps_data.rmc_valid = frame.valid;
                gps_data.rmc_timestamp = k_uptime_get_32();
                gps_data.valid_sentences++;
                
                #if 0  // 调试输出
                LOG_INF("RMC: Lat=%.6f, Lon=%.6f, Speed=%.1f knots, Valid=%d",
                        minmea_tocoord(&frame.latitude),
                        minmea_tocoord(&frame.longitude),
                        minmea_tofloat(&frame.speed),
                        frame.valid);
                #endif
            } else {
                gps_data.invalid_sentences++;
                LOG_WRN("Failed to parse RMC");
            }
        } break;
        
        case MINMEA_SENTENCE_GGA: {
            // 解析 GGA (全球定位系统固定数据)
            struct minmea_sentence_gga frame;
            if (minmea_parse_gga(&frame, line)) {
                gps_data.gga = frame;
                gps_data.gga_valid = (frame.fix_quality > 0);
                gps_data.gga_timestamp = k_uptime_get_32();
                gps_data.valid_sentences++;
                
                #if 0  // 调试输出
                LOG_INF("GGA: Lat=%.6f, Lon=%.6f, Alt=%.1fm, Sats=%d, Quality=%d",
                        minmea_tocoord(&frame.latitude),
                        minmea_tocoord(&frame.longitude),
                        minmea_tofloat(&frame.altitude),
                        frame.satellites_tracked,
                        frame.fix_quality);
                #endif
            } else {
                gps_data.invalid_sentences++;
                LOG_WRN("Failed to parse GGA");
            }
        } break;
        
        case MINMEA_SENTENCE_GSV: {
            // 解析 GSV (卫星可见信息)
            struct minmea_sentence_gsv frame;
            if (minmea_parse_gsv(&frame, line)) {
                gps_data.gsv = frame;
                gps_data.gsv_valid = true;
                gps_data.valid_sentences++;
            } else {
                gps_data.invalid_sentences++;
            }
        } break;
        
        case MINMEA_SENTENCE_GLL:
        case MINMEA_SENTENCE_GSA:
        case MINMEA_SENTENCE_VTG:
        case MINMEA_SENTENCE_ZDA:
        case MINMEA_SENTENCE_GST:
            // 支持但暂不解析的句子类型
            gps_data.valid_sentences++;
            break;
            
        case MINMEA_INVALID:
            gps_data.invalid_sentences++;
            break;
            
        default:
            break;
    }
}

/**************************************************************************
    \brief   初始化 GPS
**************************************************************************/
void HAL::GPS_Init()
{
    // if (!device_is_ready(gps_uart)) {
    //     LOG_ERR("GPS UART device not ready");
    //     return;
    // }

    // // 清空数据
    // memset(&gps_data, 0, sizeof(gps_data));
    // memset(nmea_line, 0, sizeof(nmea_line));
    // nmea_line_idx = 0;

    // // 初始化消息队列
    // k_msgq_init(&gps_msgq, (char*)gps_rx_buf, 1, GPS_RX_BUF_SIZE);
    
    // // 配置 UART 中断
    // uart_irq_callback_set(gps_uart, gps_uart_callback);
    // uart_irq_rx_enable(gps_uart);

    // gps_initialized = true;
    // LOG_INF("GPS initialized successfully");
}

/**************************************************************************
    \brief   更新 GPS 数据 (必须在主循环中调用)
**************************************************************************/
void HAL::GPS_Update()
{
    char c;

    if (!gps_initialized) {
        return;
    }

    while (k_msgq_get(&gps_msgq, &c, K_NO_WAIT) == 0) {
        
        // NMEA 句子以 '$' 开头
        if (c == '$') {
            nmea_line_idx = 0;
            nmea_line[nmea_line_idx++] = c;
        }
        // 句子以 \r 或 \n 结束
        else if (c == '\r' || c == '\n') {
            if (nmea_line_idx > 0) {
                nmea_line[nmea_line_idx] = '\0';
                
                // 处理完整的句子
                process_nmea_line(nmea_line);
                
                nmea_line_idx = 0;
            }
        }
        // 累积字符
        else if (nmea_line_idx > 0 && nmea_line_idx < NMEA_LINE_MAX - 1) {
            nmea_line[nmea_line_idx++] = c;
        }
    }
}

/**************************************************************************
    \brief   获取 GPS 信息
    \param   info 输出数据结构
    \return  true=定位有效, false=定位无效
**************************************************************************/
bool HAL::GPS_GetInfo(GPS_Info_t* info)
{
    if (!gps_initialized || !info) {
        return false;
    }

    // 清空结构
    memset(info, 0, sizeof(GPS_Info_t));

    // 检查数据是否有效（优先使用 RMC）
    bool position_valid = false;
    
    if (gps_data.rmc_valid) {
        // 使用 RMC 数据
        info->latitude = minmea_tocoord(&gps_data.rmc.latitude);
        info->longitude = minmea_tocoord(&gps_data.rmc.longitude);
        info->speed = minmea_tofloat(&gps_data.rmc.speed) * 1.852f; // 节 -> km/h
        info->course = minmea_tofloat(&gps_data.rmc.course);
        
        // 直接使用 minmea 的日期和时间结构
        info->clock.year = gps_data.rmc.date.year + 2000;  // minmea 存储 YY 格式
        info->clock.month = gps_data.rmc.date.month;
        info->clock.day = gps_data.rmc.date.day;
        info->clock.hour = gps_data.rmc.time.hours;
        info->clock.minute = gps_data.rmc.time.minutes;
        info->clock.second = gps_data.rmc.time.seconds;
        info->clock.millisecond = gps_data.rmc.time.microseconds / 1000;
        
        // 计算星期（可选，使用 minmea_getdatetime）
        struct tm time_tm;
        if (minmea_getdatetime(&time_tm, &gps_data.rmc.date, &gps_data.rmc.time) == 0) {
            info->clock.week = time_tm.tm_wday;  // 0=Sunday, 1=Monday, ...
        }
        
        position_valid = true;
    }
    
    if (gps_data.gga_valid) {
        // 补充 GGA 数据
        if (!position_valid) {
            info->latitude = minmea_tocoord(&gps_data.gga.latitude);
            info->longitude = minmea_tocoord(&gps_data.gga.longitude);
            position_valid = true;
        }
        
        info->altitude = minmea_tofloat(&gps_data.gga.altitude);
        info->satellites = gps_data.gga.satellites_tracked;
        
        // 如果 RMC 没有时间，使用 GGA 的时间
        if (info->clock.hour == 0 && info->clock.minute == 0) {
            info->clock.hour = gps_data.gga.time.hours;
            info->clock.minute = gps_data.gga.time.minutes;
            info->clock.second = gps_data.gga.time.seconds;
            info->clock.millisecond = gps_data.gga.time.microseconds / 1000;
        }
    }
    
    info->isVaild = position_valid;

    return info->isVaild;
}

/**************************************************************************
    \brief   检查 GPS 定位是否有效
    \return  true=有效, false=无效
**************************************************************************/
bool HAL::GPS_LocationIsValid()
{
    if (!gps_initialized) {
        return false;
    }

    return gps_data.rmc_valid || gps_data.gga_valid;
}

/**************************************************************************
    \brief   计算两点间距离 (米)
    \param   info 当前位置
    \param   preLong 上一个位置经度
    \param   preLat 上一个位置纬度
    \return  距离 (米)
**************************************************************************/
double HAL::GPS_GetDistanceOffset(GPS_Info_t* info, double preLong, double preLat)
{
    if (!info || !info->isVaild) {
        return 0.0;
    }

    // Haversine 公式
    const double R = 6371000.0; // 地球半径 (米)
    
    double lat1 = preLat * M_PI / 180.0;
    double lat2 = info->latitude * M_PI / 180.0;
    double dlat = (info->latitude - preLat) * M_PI / 180.0;
    double dlon = (info->longitude - preLong) * M_PI / 180.0;
    
    double a = sin(dlat / 2.0) * sin(dlat / 2.0) +
               cos(lat1) * cos(lat2) *
               sin(dlon / 2.0) * sin(dlon / 2.0);
    
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    
    return R * c;
}

/**************************************************************************
    \brief   获取原始 minmea RMC 数据 (扩展功能)
    \param   rmc 输出 RMC 结构
    \return  true=成功, false=失败
**************************************************************************/
bool GPS_GetRMC(struct minmea_sentence_rmc* rmc)
{
    if (!gps_initialized || !rmc) {
        return false;
    }
    
    if (!gps_data.rmc_valid) {
        return false;
    }
    
    memcpy(rmc, &gps_data.rmc, sizeof(struct minmea_sentence_rmc));
    return true;
}

/**************************************************************************
    \brief   获取原始 minmea GGA 数据 (扩展功能)
    \param   gga 输出 GGA 结构
    \return  true=成功, false=失败
**************************************************************************/
bool GPS_GetGGA(struct minmea_sentence_gga* gga)
{
    if (!gps_initialized || !gga) {
        return false;
    }
    
    if (!gps_data.gga_valid) {
        return false;
    }
    
    memcpy(gga, &gps_data.gga, sizeof(struct minmea_sentence_gga));
    return true;
}

/**************************************************************************
    \brief   获取卫星信息 (扩展功能)
    \param   gsv 输出 GSV 结构
    \return  true=成功, false=失败
**************************************************************************/
bool GPS_GetGSV(struct minmea_sentence_gsv* gsv)
{
    if (!gps_initialized || !gsv) {
        return false;
    }
    
    if (!gps_data.gsv_valid) {
        return false;
    }
    
    memcpy(gsv, &gps_data.gsv, sizeof(struct minmea_sentence_gsv));
    return true;
}

/**************************************************************************
    \brief   获取 GPS 统计信息
    \param   total 总句子数
    \param   valid 有效句子数
    \param   invalid 无效句子数
**************************************************************************/
void GPS_GetStats(uint32_t* total, uint32_t* valid, uint32_t* invalid)
{
    if (!gps_initialized) {
        return;
    }
    
    if (total) *total = gps_data.total_sentences;
    if (valid) *valid = gps_data.valid_sentences;
    if (invalid) *invalid = gps_data.invalid_sentences;
}

/**************************************************************************
    \brief   打印 GPS 状态 (调试用)
**************************************************************************/
void HAL::GPS_PrintStatus()
{
    if (!gps_initialized) {
        LOG_ERR("GPS not initialized");
        return;
    }

    LOG_INF("========== GPS Status ==========");
    
    // RMC 状态
    if (gps_data.rmc_valid) {
        LOG_INF("RMC Valid (age: %u ms)", 
                k_uptime_get_32() - gps_data.rmc_timestamp);
        LOG_INF("  Position: %.6f, %.6f",
                minmea_tocoord(&gps_data.rmc.latitude),
                minmea_tocoord(&gps_data.rmc.longitude));
        LOG_INF("  Speed: %.1f knots (%.1f km/h)",
                minmea_tofloat(&gps_data.rmc.speed),
                minmea_tofloat(&gps_data.rmc.speed) * 1.852f);
        LOG_INF("  Course: %.1f°",
                minmea_tofloat(&gps_data.rmc.course));
        LOG_INF("  Date: %04d-%02d-%02d",
                gps_data.rmc.date.year + 2000,
                gps_data.rmc.date.month,
                gps_data.rmc.date.day);
        LOG_INF("  Time: %02d:%02d:%02d.%06d UTC",
                gps_data.rmc.time.hours,
                gps_data.rmc.time.minutes,
                gps_data.rmc.time.seconds,
                gps_data.rmc.time.microseconds);
    } else {
        LOG_INF("RMC: Invalid");
    }
    
    // GGA 状态
    if (gps_data.gga_valid) {
        LOG_INF("GGA Valid (age: %u ms)",
                k_uptime_get_32() - gps_data.gga_timestamp);
        LOG_INF("  Position: %.6f, %.6f",
                minmea_tocoord(&gps_data.gga.latitude),
                minmea_tocoord(&gps_data.gga.longitude));
        LOG_INF("  Altitude: %.1f m",
                minmea_tofloat(&gps_data.gga.altitude));
        LOG_INF("  Fix Quality: %d", gps_data.gga.fix_quality);
        LOG_INF("  Satellites: %d", gps_data.gga.satellites_tracked);
        LOG_INF("  HDOP: %.1f", minmea_tofloat(&gps_data.gga.hdop));
    } else {
        LOG_INF("GGA: Invalid");
    }
    
    // GSV 状态
    if (gps_data.gsv_valid) {
        LOG_INF("GSV: Msg %d/%d, Total sats=%d",
                gps_data.gsv.msg_nr,
                gps_data.gsv.total_msgs,
                gps_data.gsv.total_sats);
        
        for (int i = 0; i < 4; i++) {
            if (gps_data.gsv.sats[i].nr != 0) {
                LOG_INF("  Sat %d: Elev=%d° Azim=%d° SNR=%d dB",
                        gps_data.gsv.sats[i].nr,
                        gps_data.gsv.sats[i].elevation,
                        gps_data.gsv.sats[i].azimuth,
                        gps_data.gsv.sats[i].snr);
            }
        }
    }
    
    // 统计
    LOG_INF("Statistics:");
    LOG_INF("  Total sentences: %u", gps_data.total_sentences);
    LOG_INF("  Valid sentences: %u", gps_data.valid_sentences);
    LOG_INF("  Invalid sentences: %u", gps_data.invalid_sentences);
    
    if (gps_data.total_sentences > 0) {
        float success_rate = (float)gps_data.valid_sentences * 100.0f / 
                            (float)gps_data.total_sentences;
        LOG_INF("  Success rate: %.1f%%", success_rate);
    }
    
    LOG_INF("================================");
}

/**************************************************************************
    \brief   重置 GPS 数据
**************************************************************************/
void HAL::GPS_Reset()
{
    if (gps_initialized) {
        memset(&gps_data, 0, sizeof(gps_data));
        nmea_line_idx = 0;
        LOG_INF("GPS data reset");
    }
}