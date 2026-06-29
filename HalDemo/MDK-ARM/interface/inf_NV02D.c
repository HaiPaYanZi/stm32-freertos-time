#include "inf_NV02D.h"
#include "inf_DS1302Z.h"
void Inf_NV02D_Init(void)
{
    vTaskDelay(500); // NV02D模块启动时要等待
}
void Inf_NV02D_send_byte(uint8_t byte)
{
    // 1.初始化，拉高CLK,SDA线
    NV02D_CLK_H;

    if (byte & (1)) // 从最低位开始发送
    {
        NV02D_SDA_H; // 数据线拉高，表示1
    }
    else
    {
        NV02D_SDA_L; // 数据线拉低，表示0
    }

    // 2.发送起始命令
    NV02D_CLK_L;   // CLK拉低，准备发送数据
    vTaskDelay(4); // 等待CLK稳定
    for (int i = 0; i < 8; i++)
    {
        if (byte & (1 << i)) // 从最低位开始发送
        {
            NV02D_SDA_H; // 数据线拉高，表示1
        }
        else
        {
            NV02D_SDA_L; // 数据线拉低，表示0
        }
        NV02D_CLK_L;           // CLK拉低，NV02D模块读取数据
        DS1302Z_Delay_us(400); // 等待CLK稳定
        NV02D_CLK_H;           // CLK拉高，准备发送下一位
        DS1302Z_Delay_us(400); // 等待CLK稳定
    }
}
/*
void Inf_NV02D_send_cmd(uint8_t cmd) // 双线通信
{
    while (NV02D_BUSY_READ == GPIO_PIN_RESET)
    {
        // 等待NV02D模块空闲
        vTaskDelay(10);
    }
    vTaskDelay(100); // 等待NV02D模块稳定
    // 1.发起始码
    Inf_NV02D_send_byte(0xF1);
    // 2.发送命令
    // 0x00-0xdf播放对应地址的音频
    Inf_NV02D_send_byte(cmd);
    // 3.发结束码
    Inf_NV02D_send_byte(0xF3);
    // 4.校验
    uint8_t sum = (uint8_t)(0xF1 + cmd + 0xF3); // 校验和，简单的加法校验
    Inf_NV02D_send_byte(sum);
}
*/
void Inf_NV02D_send_cmd(uint8_t cmd) // 单线通信
{
    // 1.初始化
    NV02D_CTR_H;
    // 2.拉低4ms，准备开始通信
    NV02D_CLK_L;
    vTaskDelay(4);
    // 3.发送数据,低位先发 //使用vTaskdelay()发送数据时间长，但是可以让出cpu
    // 但为了这个代码的执行，他的优先级一定要最高，不然让出cpu后，不能及时响应，导致NV02D芯片数据错误
    for (int i = 0; i < 8; i++)
    {
        if (cmd & (1 << i))
        {
            // 1,高3:低1
            NV02D_CTR_H;
            // DS1302Z_Delay_us(1200);
            vTaskDelay(3);
            NV02D_CTR_L;
            // DS1302Z_Delay_us(400);
            vTaskDelay(1);
        }
        else
        {
            // 0,高1:低3
            NV02D_CTR_H;
            // DS1302Z_Delay_us(400);
            vTaskDelay(1);
            NV02D_CTR_L;
            // DS1302Z_Delay_us(1200);
            vTaskDelay(3);
        }
    }
    NV02D_CTR_H;     // 结束后拉高
    vTaskDelay(100); // 确保两个命令的间隔
}

void Inf_NV02D_set_volume(uint8_t volume)
{
    if (volume > 7)
    {
        volume = 0;
    }
    Inf_NV02D_send_cmd(0xE0 + volume); // 0xE0-0xE7//8个等级
}

void Inf_NV02D_stop(void)
{
    Inf_NV02D_send_cmd(0xFE);
}
