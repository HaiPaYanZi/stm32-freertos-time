#include "App_switch.h"

extern show_type_struct show_type;
extern uint8_t led_en;
extern uint8_t switch_alarm_en = 0;
extern show_mode_type show_mode;
extern Show_time_set show_time_set;
extern Show_alarm_set show_alarm_set;
extern Alarm_Time_Type alarm_time1;
extern Alarm_Time_Type alarm_time2;
extern Clock_Date_Time_Type clock_date_time;
uint8_t sound_control_flag = 0;
uint32_t sound_control_last_time = 0;
void App_switch_key_process(void)
{
    // 得到按键值
    Key_Type_value key_value = Inf_get_key_value();
    // 处理开关逻辑
    switch (key_value)
    {
    case KEY_TIM_SET:
        show_type.is_12_hour = ((show_type.is_12_hour + 1) % 2);
        break;
    case KEY_TIM_SET_LONG:
        // 进入时间设置模式
        show_mode = SHOW_MODE_TIME_SET;
        // 统一使用24h
        show_type.is_12_hour = 0;
        if (clock_date_time.is_12_hour == 1 && clock_date_time.is_pm)
        {
            clock_date_time.hour += 12;
            clock_date_time.is_12_hour = 0;
        }
        break;
    case KEY_UP:
        show_type.celsius = (show_type.celsius + 1) % 2;
        break;
    case KEY_DOWN:
        show_mode = SHOW_MODE_VOLUME;
        break;
    case KEY_ALARM_SET:
        show_mode = SHOW_MODE_VOLUME;
        break;
    case KEY_ALARM_SET_LONG:
        // 闹钟设置统一使用24h
        show_mode = SHOW_MODE_ALARM_SET;
        show_type.is_12_hour = 0;
        show_type.is_alarm_1 = 1;
        show_type.is_alarm_2 = 0;
        break;
    case KEY_ALARM_EN:
        switch_alarm_en++;
        if (switch_alarm_en % 2 == 0)
        {
            show_type.is_alarm_1 = 0;
            show_type.is_alarm_2 = 0;
        }
        else if (switch_alarm_en == 1)
        {
            show_type.is_alarm_1 = 1;
        }
        else if (switch_alarm_en == 3)
        {
            show_type.is_alarm_2 = 1;
        }
        else if (switch_alarm_en == 5)
        {
            show_type.is_alarm_1 = 1;
            show_type.is_alarm_2 = 1;
        }
        break;
    case KEY_ALARM_5:

        break;
    default:
        break;
    }
}

void App_switch_rotary_process(void)
{
    // 读取当前的拨动开关的值
    LED_ON_Type_value led_on_value = Inf_get_led_on_value();
    // light_off时为声控
    // light_on时为常亮
    LIGHT_ON_Type_value light_type_value = Inf_get_light_on_value();
    // 读取声控开关的值
    Mic_value mic_value = Inf_get_mic_value();
    if (mic_value == MIC_OFF)
    {
        sound_control_flag = 1;
        sound_control_last_time = xTaskGetTickCount();
    }
    if (led_on_value == LED_ON)
    {
        // 判断声控/常亮模式
        if (light_type_value)
        {
            led_en = 1;
        }
        else if (sound_control_flag == 1)
        {
            // 声控模式
            if (xTaskGetTickCount() - sound_control_last_time <= 5000)
            {
                led_en = 1;
            }
            else
            {
                led_en = 0;
                sound_control_flag = 0;
            }
        }
        else
        {
            led_en = 0;
        }
    }
    else
    {
        led_en = 0;
    }
}

void App_switch_time_set_process(void)
{
    Key_Type_value key_value = Inf_get_key_value();
    switch (key_value)
    {
    case KEY_UP:
        if (show_time_set == TIME_SET_YEAR)
        {
            clock_date_time.year++;
            if (clock_date_time.year > 2099)
            {
                clock_date_time.year = 2000;
            }
        }
        else if (TIME_SET_MONTH)
        {
            clock_date_time.month++;
            if (clock_date_time.month > 12)
            {
                clock_date_time.month = 1;
            }
        }
        else if (TIME_SET_DAY)
        {
            clock_date_time.day++;
            if (clock_date_time.day > 31)
            {
                clock_date_time.day = 1;
            }
        }
        else if (TIME_SET_HOUR)
        {
            clock_date_time.hour++;
            if (clock_date_time.hour > 23)
            {
                clock_date_time.hour = 0;
            }
        }
        else if (TIME_SET_MIN)
        {
            clock_date_time.min++;
            if (clock_date_time.min > 59)
            {
                clock_date_time.min = 0;
            }
        }
        break;
    case KEY_DOWN:
        if (show_time_set == TIME_SET_YEAR)
        {
            clock_date_time.year--;
            if (clock_date_time.year < 2000)
            {
                clock_date_time.year = 2099;
            }
        }
        else if (TIME_SET_MONTH)
        {
            clock_date_time.month--;
            if (clock_date_time.month < 1)
            {
                clock_date_time.month = 12;
            }
        }
        else if (TIME_SET_DAY)
        {
            clock_date_time.day--;
            if (clock_date_time.day < 1)
            {
                clock_date_time.day = 31;
            }
        }
        else if (TIME_SET_HOUR)
        {
            clock_date_time.hour--;
            if (clock_date_time.hour < 1)
            {
                clock_date_time.hour = 23;
            }
        }
        else if (TIME_SET_MIN)
        {
            clock_date_time.min--;
            if (clock_date_time.min < 0)
            {
                clock_date_time.min = 59;
            }
        }
        break;
    case KEY_TIM_SET:
        show_time_set++;
        if (show_time_set == TIME_SET_END)
        {
            // 退出，切换状态
            show_mode = SHOW_MODE_NORMAL;
            // 设置时间
            clock_date_time.is_pm = show_type.is_pm;
            App_dateTime_set(&clock_date_time);
            show_time_set = TIME_SET_YEAR;
        }
        break;
    default:
        break;
    }
}

void App_show_alarm_set_process(void)
{
    // 得到按键值
    Key_Type_value key_value = Inf_get_key_value();
    switch (key_value)
    {
    case KEY_UP: // 加1
        if (show_alarm_set == ALARM_SET_HOUR)
        {
            if (show_type.is_alarm_1 == 1)
            {
                alarm_time1.hour++;
                if (alarm_time1.hour > 23)
                {
                    alarm_time1.hour = 0;
                }
            }
            else if (show_type.is_alarm_2 == 1)
            {
                alarm_time2.hour++;
                if (alarm_time2.hour > 23)
                {
                    alarm_time2.hour = 0;
                }
            }
        }
        if (show_alarm_set == ALARM_SET_MIN)
        {
            if (show_type.is_alarm_1 == 1)
            {
                alarm_time1.min++;
                if (alarm_time1.min > 59)
                {
                    alarm_time1.min = 0;
                }
            }
            else if (show_type.is_alarm_2 == 1)
            {
                alarm_time2.min++;
                if (alarm_time2.min > 59)
                {
                    alarm_time2.min = 0;
                }
            }
        }
        break;

    case KEY_DOWN: // 减1
        if (show_alarm_set == ALARM_SET_HOUR)
        {
            if (show_type.is_alarm_1 == 1)
            {
                alarm_time1.hour--;
                if (alarm_time1.hour < 0)
                {
                    alarm_time1.hour = 23;
                }
            }
            else if (show_type.is_alarm_2 == 1)
            {
                alarm_time2.hour--;
                if (alarm_time2.hour < 0)
                {
                    alarm_time2.hour = 23;
                }
            }
        }
        if (show_alarm_set == ALARM_SET_MIN)
        {
            if (show_type.is_alarm_1 == 1)
            {
                alarm_time1.min--;
                if (alarm_time1.min < 0)
                {
                    alarm_time1.min = 59;
                }
            }
            else if (show_type.is_alarm_2 == 1)
            {
                alarm_time2.min--;
                if (alarm_time2.min < 0)
                {
                    alarm_time2.min = 59;
                }
            }
        }
        break;
    case KEY_ALARM_SET: // 确认当前设置并进入下一个设置
        show_alarm_set++;
        if (show_alarm_set == ALARM_SET_END)
        {
            if (switch_alarm_en % 2 == 0)
            {
                if (show_type.is_alarm_1 == 1)
                {
                    switch_alarm_en = 1;
                }
                else if (show_type.is_alarm_2 == 1)
                {
                    switch_alarm_en = 3;
                }
            }
            else if (switch_alarm_en % 2 == 1)
            {
                if (show_type.is_alarm_2 == 1)
                {
                    switch_alarm_en = 5; // 两个都开启
                    show_type.is_alarm_1 = 1;
                }
            }
            else if (switch_alarm_en % 2 == 3)
            {
                if (show_type.is_alarm_1 == 1)
                {
                    switch_alarm_en = 5; // 两个都开启
                    show_type.is_alarm_2 = 1;
                }
            }
            // 切换回normal正常界面
            show_mode = SHOW_MODE_NORMAL;
            show_alarm_set = ALARM_SET_HOUR;
        }
        break;
    case KEY_ALARM_EN: // 切换闹钟1-2
        if (show_type.is_alarm_1 == 1)
        {
            show_type.is_alarm_1 = 0;
            show_type.is_alarm_2 = 1;
        }
        else if (show_type.is_alarm_2 == 1)
        {
            show_type.is_alarm_1 = 1;
            show_type.is_alarm_2 = 0;
        }

        break;
    case KEY_ALARM_5:
        break;
    }
}
