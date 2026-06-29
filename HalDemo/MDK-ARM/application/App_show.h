#ifndef __APP_SHOW_H_
#define __APP_SHOW_H_
#include "inf_led.h"
#include "inf_DS1302Z.h"
#include "App_dateTime.h"
#include "FreeRTOS.h"
#include "task.h"
// 展示模式
typedef enum
{
    SHOW_MODE_NORMAL,    // 正常模式
    SHOW_MODE_TIME_SET,  // 时间设置模式
    SHOW_MODE_ALARM_SET, // 闹钟设置模式
    SHOW_MODE_VOLUME,    // 音量设置模式
} show_mode_type;

void App_show_start(void);                                                                                               // 初始化LED灯
void App_show_normal(Clock_Date_Time_Type *date_time, int8_t temperature, int8_t humidity, show_type_struct *show_type); // 正常显示模式
void App_show_time_set(Clock_Date_Time_Type *date_time, int8_t temperature, int8_t humidity, show_type_struct *show_type);
void App_show_alarm_set(Clock_Date_Time_Type *date_time, int8_t temperature, int8_t humidity, show_type_struct *show_type);
#endif
