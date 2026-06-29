#ifndef __APP_DATETIME_
#define __APP_DATETIME_
#include "inf_DS1302Z.h"

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    int8_t hour;
    int8_t min;
    uint8_t sec;
    uint8_t week;
    uint8_t is_pm;
    uint8_t is_12_hour;
} Clock_Date_Time_Type;

typedef struct
{
    uint8_t hour;
    uint8_t min;
    uint8_t is_on;
    uint8_t is_done; // 响过
} Alarm_Time_Type;

void App_dateTime_get(Clock_Date_Time_Type *date_time); // 读取所有时间
void App_dateTime_set(Clock_Date_Time_Type *date_time); // 设置时间
#endif
