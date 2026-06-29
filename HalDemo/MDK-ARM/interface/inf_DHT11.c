#include "inf_DHT11.h"
#include "inf_DS1302Z.h" //用一下us延时函数
#include "Com_debug.h"
#include "main.h"
#include "stm32f1xx_hal.h"
uint8_t data[5] = {0}; // 用于存储接收到的5个字节数据

void Inf_DHT11_Init(void)
{
    // 确保DHT11_DATA线开始为高电平
    DHT11_DATA_H;
    vTaskDelay(1000); // DHT11启动时要等待1ms以上
}
void Inf_DHT11_Get_Data(int8_t *temperature, int8_t *humidity)
{
    int8_t temp = 0;
    int8_t hum = 0;
    // 读取温湿度，不可以打断
    //  添加临界区
    taskENTER_CRITICAL(); // 进入临界区
    // 1.启动拉低DHT11_DATA线至少18-30ms
    DHT11_DATA_L;
    vTaskDelay(20);
    DHT11_DATA_H; // 拉高DHT11_DATA线

    // 2.等待DHT11响应,为了CPU更好的工作，等待响应是没有意义的,可以直接长时等待到DHT11快结束时
    // 在读数据，这样可以节省时间并更好的利用CPU资源
    // 在这里等待DHT11响应是1-100us，不够1ms，没有必要释放CPU，可以选择等完后直接读数据
    // 使用等待方法要有一个超时机制，防止死等

    uint32_t count_max = 0xfffff;
    while (DHT11_DATA_READ == GPIO_PIN_SET && count_max--)
    {
        // 等待DHT11拉低数据线响应
    }
    while (DHT11_DATA_READ == GPIO_PIN_RESET && count_max--)
    {
        // 等待DHT11拉高数据线响应，大约83us
    }
    while (DHT11_DATA_READ == GPIO_PIN_SET && count_max--)
    {
        // 等待DHT11再拉低数据线响应，大约87us
    }
    // 3.判断是否超时
    if (count_max == 0)
    {
        debug_printf("DHT11 response timeout\r\n");
        return; // 超时，直接返回
    }

    // 4.接收40位数据，5个字节，高位再前
    for (uint8_t i = 0; i < 8; i++)
    {
        data[i] = 0;
        for (uint8_t j = 0; j < 8; j++)
        {
            // 54us的低电平信号
            while (DHT11_DATA_READ == GPIO_PIN_RESET)
            {
                // 等待高电平
            }
            DS1302Z_Delay_us(40);
            if (DHT11_DATA_READ == GPIO_PIN_SET)
            {                              // 高电平为1
                data[i] |= (1 << (7 - j)); // 设置对应位为1
            }
            else
            {     // 低电平为0
                ; // 不用操作，默认就是0
            }
        }
    }
    // 5.校验数据
    uint32_t sum = data[0] + data[1] + data[2] + data[3];
    if ((uint8_t)sum == data[4])
    {
        hum = data[0];      // 整数部分
        temp = data[2];     // 整数部分(负数时最高位为1)
        if (data[3] & 0x80) // 判断小数部分最高位是否为1，表示负数
        {
            temp = -temp; // 取负
        }
        // 赋值返回
        *temperature = temp;
        *humidity = hum;
    }
    else
    {
        debug_printf("DHT11 checksum error\r\n");
    }
    // 退出临界区
    taskEXIT_CRITICAL();
}
