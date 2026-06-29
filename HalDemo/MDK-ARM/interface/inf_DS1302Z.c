#include "inf_DS1302Z.h"

// us延时函数
void DS1302Z_Delay_us(uint32_t us)
{
    uint32_t count = us * 8; // 大约循环8次可以产生1微秒的延时，具体值需要根据系统时钟频率进行调整
    while (count--)
    {
        __NOP(); // 空操作，防止优化//这个__NOP函数可以补全一个时钟周期
    }
}

// 读寄存器
uint8_t DS1302Z_Read_Byte(uint8_t reg_addr)
{
    reg_addr |= 0x01; // 确保最低位为1，表示读操作
    uint8_t data = 0;
    // 1.初始化引脚
    DS_RST_L; // 复位线拉低
    DS_CLK_L; // 时钟线拉低

    // 2.拉高复位线，准备通信
    DS_RST_H;
    DS1302Z_Delay_us(5); // 等待复位线稳定

    // 3.循环8次,低位优先,发送寄存器地址
    for (int i = 0; i < 8; i++)
    {
        if (reg_addr & (1 << i))
        {
            DS_IO_H; // 数据线拉高
        }
        else
        {
            DS_IO_L; // 数据线拉低
        }
        DS_CLK_H;            // 时钟线拉高，数据被DS1302Z读取
        DS1302Z_Delay_us(1); // 等待时钟线稳定

        // 4. 时钟线拉低，准备发送下一位
        DS_CLK_L;
    }
    // 5.循环8次,低位优先,读取寄存器数据
    // 读之前一定要释放IO线
    DS_IO_H; // 将数据线设置为输入模式，DS1302Z会驱动数据线输出数据
    for (int i = 0; i < 8; i++)
    {

        data |= (DS_IO_READ << i); // 将对应位设置为1
        if (i < 7)                 // 细节:是写完命令后，的第一个下降沿才是数据的第一位，所以这里需要在时钟线拉高之前读取数据
        {
            DS_CLK_H; // 时钟线拉高，DS1302Z输出数据
            DS_CLK_L; // 时钟线拉低，准备读取下一位
        }
    }
    // 6.通信结束，拉低复位线
    DS_RST_L;
    return data; // 这里返回0只是占位，实际应该根据通信协议读取数据并返回
}
// 写寄存器
void DS1302Z_Write_Byte(uint8_t reg_addr, uint8_t data)
{
    reg_addr &= 0xFE; // 确保最低位为0，表示写操作

    // 1.初始化引脚
    DS_RST_L; // 复位线拉低
    DS_CLK_L; // 时钟线拉低

    // 2.拉高复位线，准备通信
    DS_RST_H;
    DS1302Z_Delay_us(5); // 等待复位线稳定

    // 3.循环8次,低位优先,发送寄存器地址
    for (int i = 0; i < 8; i++)
    {
        if (reg_addr & (1 << i))
        {
            DS_IO_H; // 数据线拉高
        }
        else
        {
            DS_IO_L; // 数据线拉低
        }
        DS_CLK_H;            // 时钟线拉高，数据被DS1302Z读取
        DS1302Z_Delay_us(1); // 等待时钟线稳定

        // 4. 时钟线拉低，准备发送下一位
        DS_CLK_L;
    }

    for (int i = 0; i < 8; i++)
    {
        if (data & (1 << i))
        {
            DS_IO_H; // 数据线拉高
        }
        else
        {
            DS_IO_L; // 数据线拉低
        }
        DS_CLK_H;            // 时钟线拉高，数据被DS1302Z读取
        DS1302Z_Delay_us(1); // 等待时钟线稳定

        // 4. 时钟线拉低，准备发送下一位
        DS_CLK_L;
    }
}
