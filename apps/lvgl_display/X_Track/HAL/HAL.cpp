#include "HAL.h"

void HAL::HAL_Init()
{
    HAL::Power_Init();
    HAL::Encoder_Init();
    HAL::Buzz_Init();
    HAL::GPS_Init();
}