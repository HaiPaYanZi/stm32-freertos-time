#ifndef __INF_KEY_H
#define __INF_KEY_H
#include "gpio.h"
#include "Com_debug.h"
#include "FreeRTOS.h"
#include "task.h"
typedef enum
{
    KEY_NONE = 0,
    KEY_TIM_SET,        // 表示短按
    KEY_TIM_SET_LONG,   // 表示长按
    KEY_UP,             // 上调
    KEY_DOWN,           // 下调
    KEY_ALARM_SET,      // 时间设置短按
    KEY_ALARM_SET_LONG, // 时间设置长按
    KEY_ALARM_EN,       // 闹钟使能
    KEY_ALARM_5,        // 5天闹钟
} Key_Type_value;

typedef enum
{
    LED_ON,
    LED_OFF = 0,
} LED_ON_Type_value;

typedef enum
{
    LIGHT_ON,
    LIGHT_OFF = 0,
} LIGHT_ON_Type_value;

void Inf_key_init(void);
Key_Type_value Inf_get_key_value(void);           // 获取按键值，返回一个枚举类型的值，表示按键的状态
LED_ON_Type_value Inf_get_led_on_value(void);     // 获取LED状态，返回一个枚举类型的值，表示LED的状态
LIGHT_ON_Type_value Inf_get_light_on_value(void); // 获取灯状态，返回一个枚举类型的值，表示灯的状态

#endif /* __INF_KEY_H */
