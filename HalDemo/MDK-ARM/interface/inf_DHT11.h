#ifndef __INF_DHT11_H__
#define __INF_DHT11_H__

#include "gpio.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"

#define DHT11_DATA_H HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin, GPIO_PIN_SET)
#define DHT11_DATA_L HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin, GPIO_PIN_RESET)
#define DHT11_DATA_READ HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin)
// DHT11启用时要等待1ms以上
void Inf_DHT11_Init(void);
// 获取DHT11温湿度数据，温度和湿度均为整数形式
void Inf_DHT11_Get_Data(int8_t *temperature, int8_t *humidity);

#endif // __INF_DHT11_H__
