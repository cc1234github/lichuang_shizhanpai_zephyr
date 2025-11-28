#ifndef __HAL_DEF_H
#define __HAL_DEF_H

#include <stdint.h>


namespace HAL
{
    /* Clock */
    typedef struct
    {
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t week;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint16_t millisecond;
    } Clock_Info_t;

    /* GPS */
    typedef struct
    {
        double longitude;
        double latitude;
        float altitude;
        float course;
        float speed;
        int16_t satellites;
        bool isVaild;
        Clock_Info_t clock;
    } GPS_Info_t;

    /* MAG */
    typedef struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
    } MAG_Info_t;

    /* IMU */
    typedef struct
    {
        int16_t ax;
        int16_t ay;
        int16_t az;
        int16_t gx;
        int16_t gy;
        int16_t gz;
        int16_t steps;
    } IMU_Info_t;

    /* SportStatus */
    typedef struct
    {
        uint32_t lastTick;

        float weight;

        float speedKph;
        float speedMaxKph;
        float speedAvgKph;

        union
        {
            uint32_t totalTimeUINT32[2];
            uint64_t totalTime;
        };

        float totalDistance;

        union
        {
            uint32_t singleTimeUINT32[2];
            uint64_t singleTime;
        };

        float singleDistance;
        float singleCalorie;
        
    } SportStatus_Info_t;

    /* Power */
    typedef struct
    {
        uint16_t voltage;
        uint8_t usage;
        bool isCharging;
    } Power_Info_t;


    void GPS_Init();
    void GPS_Update();
    bool GPS_GetInfo(GPS_Info_t* info);
    bool GPS_LocationIsValid();
    double GPS_GetDistanceOffset(GPS_Info_t* info, double preLong, double preLat);
    void GPS_GetStats(uint32_t* total, uint32_t* valid, uint32_t* invalid);
    void GPS_PrintStatus();
    void GPS_Reset();

        /* Clock */
    void Clock_Init();
    void Clock_GetInfo(Clock_Info_t* info);
    void Clock_SetInfo(const Clock_Info_t* info);
    const char* Clock_GetWeekString(uint8_t week);

    int  Encoder_Init();
    void Encoder_Update();

    void Power_Init(void);
    void Power_GetInfo(Power_Info_t* info);
    void Power_Update();


    void Buzz_Init(void);

    void  HAL_Init();
}   




#endif
