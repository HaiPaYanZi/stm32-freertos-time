#ifndef __INF_LED_H
#define __INF_LED_H

#include "gpio.h"
#include "main.h"
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

// 展示符号
// 表示12/24 表示am/pm  表示摄氏度/华氏度 表示闹钟 表示正负号 充电标识 5天闹钟标识

typedef struct
{
    uint8_t is_pm;
    uint8_t is_12_hour;
    uint8_t celsius;
    uint8_t is_alarm_1;
    uint8_t is_alarm_2;
    uint8_t is_alarm_five_day;
    uint8_t is_charging;
} show_type_struct;

typedef enum
{
    TIME_SET_YEAR,
    TIME_SET_MONTH,
    TIME_SET_DAY,
    TIME_SET_HOUR,
    TIME_SET_MIN,
    TIME_SET_END,
} Show_time_set;

typedef enum
{
    ALARM_SET_HOUR = 0,
    ALARM_SET_MIN,
    ALARM_SET_END,
} Show_alarm_set;

extern uint16_t LED_CLK_H_S[3];
extern uint16_t LED_CLK_H_G[10];
extern uint16_t LED_CLK_M_S[10];
extern uint16_t LED_CLK_M_G[10];

#define LED_CLK_H HAL_GPIO_WritePin(LED_CLK_GPIO_Port, LED_CLK_Pin, GPIO_PIN_SET)
#define LED_CLK_L HAL_GPIO_WritePin(LED_CLK_GPIO_Port, LED_CLK_Pin, GPIO_PIN_RESET)
#define LED_SDI_H HAL_GPIO_WritePin(LED_SDI_GPIO_Port, LED_SDI_Pin, GPIO_PIN_SET)
#define LED_SDI_L HAL_GPIO_WritePin(LED_SDI_GPIO_Port, LED_SDI_Pin, GPIO_PIN_RESET)
#define LED_LE_H HAL_GPIO_WritePin(LED_LE_GPIO_Port, LED_LE_Pin, GPIO_PIN_SET)
#define LED_LE_L HAL_GPIO_WritePin(LED_LE_GPIO_Port, LED_LE_Pin, GPIO_PIN_RESET)
#define LED_OEA_H HAL_GPIO_WritePin(LED_OEA_GPIO_Port, LED_OEA_Pin, GPIO_PIN_SET)
#define LED_OEA_L HAL_GPIO_WritePin(LED_OEA_GPIO_Port, LED_OEA_Pin, GPIO_PIN_RESET)
#define LED_OEB_H HAL_GPIO_WritePin(LED_OEB_GPIO_Port, LED_OEB_Pin, GPIO_PIN_SET)
#define LED_OEB_L HAL_GPIO_WritePin(LED_OEB_GPIO_Port, LED_OEB_Pin, GPIO_PIN_RESET)

void Inf_LED_start(void); // 开
void Inf_LED_stop(void);  // 关
// p00 - vccp00 电源控制
// p01 - vccp01 电源控制
// p02 - vccp02 电源控制
// data是写入两个驱动芯片的数据
void Inf_LED_Write(uint8_t P00, uint8_t P01, uint8_t P02, uint32_t data);
void Inf_LED_Set_clock(uint8_t h, uint8_t m, uint8_t flag);
// 同时show湿度温度与时间
void Inf_LED_Set_all(uint8_t h, uint8_t m, uint8_t flag, int8_t temperature, int8_t humidity, show_type_struct *show_type);
void Inf_LED_TIME_SET(Show_time_set time_set, uint16_t value);
#endif /* __INF_LED_H */
