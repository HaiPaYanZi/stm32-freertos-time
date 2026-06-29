#include "App_show.h"
uint8_t clock_flag = 0; // 这个用来设置分号1s闪一次
uint8_t clock_last_time = 0;
extern Show_time_set show_time_set;
extern Show_alarm_set show_alarm_set;
extern Alarm_Time_Type alarm_time1;
extern Alarm_Time_Type alarm_time2;
void App_show_start(void)
{
    Inf_LED_start();
    clock_last_time = xTaskGetTickCount();
}

void App_show_normal(Clock_Date_Time_Type *date_time, int8_t temperature, int8_t humidity, show_type_struct *show_type)
{
    // Inf_LED_Set_clock(date_time->hour, date_time->min, clock_flag);
    if (xTaskGetTickCount() - clock_last_time >= 500)
    {
        clock_flag = (clock_flag + 1) % 2;
        clock_last_time = xTaskGetTickCount();
    }

    // 根据展示类型，显示不同数据
    // 1.在12/24小时中的小时显示
    uint8_t hour = 0;
    if (date_time->is_12_hour && date_time->is_pm)
    {
        hour = 12 + date_time->hour;
    }
    else
    {
        hour = date_time->hour;
    }
    if (show_type->is_12_hour)
    {
        if (hour >= 12)
        {
            hour = hour - 12;
            show_type->is_pm = 1;
        }
        else
        {
            show_type->is_pm = 0;
        }
    }

    Inf_LED_Set_all(hour, date_time->min, clock_flag, temperature, humidity, show_type);
}

void App_show_time_set(Clock_Date_Time_Type *date_time, int8_t temperature, int8_t humidity, show_type_struct *show_type)
{
    if (xTaskGetTickCount() - clock_last_time >= 500) // 闪烁
    {
        clock_flag = (clock_flag + 1) % 2;
        clock_last_time = xTaskGetTickCount();
    }
    // 1判断当前处于的页面
    switch (show_time_set)
    {
    case TIME_SET_YEAR:
        // Inf_LED_TIME_SET(TIME_SET_YEAR, date_time->year);
        if (clock_flag)
        {
            // 亮
            Inf_LED_Set_all(date_time->year / 100, date_time->year % 100, 0, temperature, humidity, show_type);
        }
        else
        {
            // 不亮
            Inf_LED_Set_all(date_time->year / 100, 0xff, 0, temperature, humidity, show_type);
        }
        break;
    case TIME_SET_MONTH:
        // Inf_LED_TIME_SET(TIME_SET_MONTH, date_time->month);
        if (clock_flag)
        {
            Inf_LED_Set_all(date_time->month / 100, 0xff, 0, temperature, humidity, show_type);
        }
        else
        {
            Inf_LED_Set_all(0xff, 0xff, 0, temperature, humidity, show_type);
        }
        break;
    case TIME_SET_DAY:
        // Inf_LED_TIME_SET(TIME_SET_DAY, date_time->day);
        if (clock_flag)
        {
            Inf_LED_Set_all(0xff, date_time->day, 0, temperature, humidity, show_type);
        }
        else
        {
            Inf_LED_Set_all(0xff, 0xff, 0, temperature, humidity, show_type);
        }
        break;
    case TIME_SET_HOUR:
        // Inf_LED_TIME_SET(TIME_SET_HOUR, date_time->hour);
        if (clock_flag)
        {
            Inf_LED_Set_all(date_time->hour, 0xff, 1, temperature, humidity, show_type);
        }
        else
        {
            Inf_LED_Set_all(0xff, 0xff, 1, temperature, humidity, show_type);
        }
        break;
    case TIME_SET_MIN:
        // Inf_LED_TIME_SET(TIME_SET_MIN, date_time->min);
        if (clock_flag)
        {
            Inf_LED_Set_all(0xff, date_time->min, 1, temperature, humidity, show_type);
        }
        else
        {
            Inf_LED_Set_all(0xff, 0xff, 1, temperature, humidity, show_type);
        }
        break;
    default:

        break;
    }
}

void App_show_alarm_set(Clock_Date_Time_Type *date_time, int8_t temperature, int8_t humidity, show_type_struct *show_type)
{
    if (xTaskGetTickCount() - clock_last_time >= 500) // 闪烁
    {
        clock_flag = (clock_flag + 1) % 2;
        clock_last_time = xTaskGetTickCount();
    }
    switch (show_alarm_set)
    {

    case ALARM_SET_HOUR:
        if (clock_flag)
        {
            if (show_type->is_alarm_1 == 1)
            {
                Inf_LED_Set_all(alarm_time1.hour, 0xff, 1, temperature, humidity, show_type);
            }
            else if (show_type->is_alarm_2 == 1)
            {
                Inf_LED_Set_all(alarm_time2.hour, 0xff, 1, temperature, humidity, show_type);
            }
        }
        else
        {
            Inf_LED_Set_all(0xff, 0xff, 1, temperature, humidity, show_type);
        }
        break;
    case ALARM_SET_MIN:
        if (clock_flag)
        {
            if (show_type->is_alarm_1 == 1)
            {
                Inf_LED_Set_all(0xff, alarm_time1.min, 1, temperature, humidity, show_type);
            }
            else if (show_type->is_alarm_2 == 1)
            {
                Inf_LED_Set_all(0xff, alarm_time2.min, 1, temperature, humidity, show_type);
            }
        }
        else
        {
            Inf_LED_Set_all(0xff, 0xff, 1, temperature, humidity, show_type);
        }
        break;
    case ALARM_SET_END:
        break;
    }
}
