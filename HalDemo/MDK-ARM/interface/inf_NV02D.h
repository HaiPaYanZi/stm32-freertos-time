#ifndef __INF_NV02D_H
#define __INF_NV02D_H

#include "gpio.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdint.h"
#include "main.h"

// 双线
#define NV02D_CLK_H HAL_GPIO_WritePin(NVD_CLK_GPIO_Port, NVD_CLK_Pin, GPIO_PIN_SET)
#define NV02D_CLK_L HAL_GPIO_WritePin(NVD_CLK_GPIO_Port, NVD_CLK_Pin, GPIO_PIN_RESET)
#define NV02D_SDA_H HAL_GPIO_WritePin(NVD_SDA_GPIO_Port, NVD_SDA_Pin, GPIO_PIN_SET)
#define NV02D_SDA_L HAL_GPIO_WritePin(NVD_SDA_GPIO_Port, NVD_SDA_Pin, GPIO_PIN_RESET)
#define NV02D_BUSY_READ HAL_GPIO_ReadPin(NVD_BUSY_GPIO_Port, NVD_BUSY_Pin)
// 单线 IOA2是控制线
#define NV02D_CTR_H HAL_GPIO_WritePin(NVD_CLK_GPIO_Port, NVD_CLK_Pin, GPIO_PIN_SET)
#define NV02D_CTR_L HAL_GPIO_WritePin(NVD_CLK_GPIO_Port, NVD_CLK_Pin, GPIO_PIN_RESET)

void Inf_NV02D_Init(void);                 // 初始化NV02D音频模块，延时，等待模块启动完成
void Inf_NV02D_send_cmd(uint8_t cmd);      // 发送命令给NV02D模块，具体命令格式需要根据NV02D模块的通信协议来定义
void Inf_NV02D_set_volume(uint8_t volume); // 控制音量
void Inf_NV02D_stop(void);                 // 停止播放，发送一个空指令
#endif                                     /* __INF_NV02D_H */
