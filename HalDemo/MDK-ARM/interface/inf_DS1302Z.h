#ifndef __INF_DS1302Z_H
#define __INF_DS1302Z_H

#include "gpio.h"
#include "main.h"
#include "stdint.h"

// ds1302z寄存器地址
#define DS1302Z_SECOND_REG 0x80 // 秒寄存器地址
#define DS1302Z_MINUTE_REG 0x82 // 分寄存器地址
#define DS1302Z_HOUR_REG 0x84   // 时寄存器地址
#define DS1302Z_DAY_REG 0x86    // 日寄存器地址
#define DS1302Z_MONTH_REG 0x88  // 月寄存器地址
#define DS1302Z_WEEK_REG 0x8A   // 星期寄存器地址
#define DS1302Z_YEAR_REG 0x8C   // 年寄存器地址

#define DS1302Z_CONTROL_REG 0x8E // 控制寄存器地址

#define DS1302Z_READ_REG 0x01  // 读寄存器命令
#define DS1302Z_WRITE_REG 0x00 // 写寄存器命令

#define DS_CLK_H HAL_GPIO_WritePin(DS_CLK_GPIO_Port, DS_CLK_Pin, GPIO_PIN_SET)   // 时钟线拉高;
#define DS_CLK_L HAL_GPIO_WritePin(DS_CLK_GPIO_Port, DS_CLK_Pin, GPIO_PIN_RESET) // 时钟线拉低;

#define DS_IO_H HAL_GPIO_WritePin(DS_IO_GPIO_Port, DS_IO_Pin, GPIO_PIN_SET)   // 数据线拉高;
#define DS_IO_L HAL_GPIO_WritePin(DS_IO_GPIO_Port, DS_IO_Pin, GPIO_PIN_RESET) // 数据线拉低;
#define DS_IO_READ HAL_GPIO_ReadPin(DS_IO_GPIO_Port, DS_IO_Pin)               // 读取数据线状态

#define DS_RST_H HAL_GPIO_WritePin(DS_RST_GPIO_Port, DS_RST_Pin, GPIO_PIN_SET)   // 复位线拉高;
#define DS_RST_L HAL_GPIO_WritePin(DS_RST_GPIO_Port, DS_RST_Pin, GPIO_PIN_RESET) // 复位线拉低;

void DS1302Z_Delay_us(uint32_t us);
uint8_t DS1302Z_Read_Byte(uint8_t reg_addr);
void DS1302Z_Write_Byte(uint8_t reg_addr, uint8_t data);
#endif /* __INF_TOUCH_H */
