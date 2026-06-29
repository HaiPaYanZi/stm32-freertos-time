#include "inf_led.h"

extern uint8_t clock_flag;
extern Show_time_set show_time_set;
// 引入亮度调节参数
extern uint8_t touch_count;
uint16_t LED_CLK_H_S[3] = {0x0000, 0x0006, 0x005B};                                                          // 小时的十位
uint16_t LED_CLK_H_G[10] = {0x1F80, 0x0300, 0x2D80, 0x2780, 0x3300, 0x3680, 0x3E80, 0x0380, 0x3F80, 0x3780}; // 小时个位

uint16_t LED_CLK_M_S[10] = {0x3F, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
uint16_t LED_CLK_M_G[10] = {0x1f80, 0x0300, 0x2d80, 0x2780, 0x3300, 0x3680, 0x3e80, 0x0380, 0x3f80, 0x3780};

uint16_t led_th_s[10] = {0x3F, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
uint16_t led_th_g[10] = {0x1f80, 0x0300, 0x2d80, 0x2780, 0x3300, 0x3680, 0x3e80, 0x0380, 0x3f80, 0x3780};

uint16_t led_hu_s[10] = {0x3F, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
uint16_t led_hu_g[10] = {0x1f80, 0x0300, 0x2d80, 0x2780, 0x3300, 0x3680, 0x3e80, 0x0380, 0x3f80, 0x3780};

void Inf_LED_start()
{
    HAL_GPIO_WritePin(P04_GPIO_Port, P04_Pin, GPIO_PIN_RESET);
}
void Inf_LED_stop()
{
    LED_OEA_H;
    LED_OEB_H;
}
void Inf_LED_Write(uint8_t P00, uint8_t P01, uint8_t P02, uint32_t data)
{

    // 高位在前
    for (int i = 0; i < 32; i++)
    {
        if (data & (0x80000000 >> i))
        {
            LED_SDI_H;
        }
        else
        {
            LED_SDI_L;
        }
        LED_CLK_H;
        LED_CLK_L;
        // 关闭所有灯//主要是为了清除所存数据,避免数据混乱
        // LED_OEA_H;
        // LED_OEB_H;
        // 锁存数据
        LED_LE_H;
        LED_LE_L;
        HAL_GPIO_WritePin(VCCP00_GPIO_Port, VCCP00_Pin, (GPIO_PinState)P00);
        HAL_GPIO_WritePin(VCCP01_GPIO_Port, VCCP01_Pin, (GPIO_PinState)P01);
        HAL_GPIO_WritePin(VCCP02_GPIO_Port, VCCP02_Pin, (GPIO_PinState)P02);
        // OE 使能
        LED_OEA_L;
        LED_OEB_L;
    }
}

void Inf_LED_Set_clock(uint8_t h, uint8_t m, uint8_t flag)
{
    uint8_t h_shi = h / 10;
    uint8_t h_ge = h % 10;
    uint16_t date = 0;
    if (flag == 0)
    {
        date = LED_CLK_H_S[h_shi] + LED_CLK_H_G[h_ge] + 0x20;
    }
    else
    {
        date = LED_CLK_H_S[h_shi] + LED_CLK_H_G[h_ge];
    }
    Inf_LED_Write(0, 1, 1, date);
    vTaskDelay(5);
    uint8_t m_shi = m / 10;
    uint8_t m_ge = m % 10;
    Inf_LED_Write(1, 0, 1, LED_CLK_M_S[m_shi] + LED_CLK_M_G[m_ge]);
    vTaskDelay(5);
}

void Inf_LED_Set_all(uint8_t h, uint8_t m, uint8_t flag, int8_t temperature, int8_t humidity, show_type_struct *show_type)
{
    // 一次性写入32位数据

    // VCCP02显示一些符号
    uint32_t VCCP02_data = 0x38; // 0x38常亮的分割线

    // 1. 拼接VCCP00，32位data
    // 1.1 湿度
    uint16_t data_right = led_hu_s[humidity / 10] + led_hu_g[humidity % 10] + 0x4000;
    // 1.2 时间小时
    uint16_t am_pm_date = 0;
    if (show_type->is_12_hour) // 上下午
    {
        if (show_type->is_pm)
        {
            am_pm_date = 0x8000;
        }
        else
        {
            am_pm_date = 0x4000;
        }
    }
    // 1.3 时间分号
    uint16_t data_left = 0;
    if (flag)
    {
        if (h > 99)
        {
            data_left = am_pm_date + 0x20;
        }
        else
        {
            data_left = LED_CLK_H_S[h / 10] + LED_CLK_H_G[h % 10] + am_pm_date + 0x20;
        }
    }
    else
    {
        if (h > 99)
        {
            data_left = am_pm_date;
        }
        else
        {
            data_left = LED_CLK_H_S[h / 10] + LED_CLK_H_G[h % 10] + am_pm_date;
        }
    }
    // 1.4 5天闹钟标识
    if (show_type->is_alarm_five_day) // 上下午
    {
        data_right += 0x8000;
    }

    Inf_LED_Write(0, 1, 1, (data_left << 16) | data_right);
    vTaskDelay(5 - touch_count);

    // 拼接VCCP01,32位数据 0x4000对应的是 %
    // 修正温度,默认是摄氏度
    uint8_t temp_now = temperature;

    // 2.0区分摄氏度与华氏度
    uint16_t temperature_data = 0;
    if (show_type->celsius)
    {
        temperature_data = 0x4000;
    }
    else
    {
        temperature_data = 0x8000;
        temp_now = (temperature * 9 / 5) + 32; // 华氏度
    }
    // 2.1 温度
    if (temp_now < 0)
    {
        temp_now = -temp_now;
        VCCP02_data += 0x02; // 负号
    }
    if (temp_now > 99)
    {
        temp_now = temp_now - 100;
        VCCP02_data += 0x05;
    }
    data_right = led_th_s[temp_now / 10] + led_th_g[temp_now % 10] + temperature_data;
    // 2.2 时间分钟
    if (m > 99)
    {
        data_left = 0;
    }
    else
    {
        data_left = LED_CLK_M_S[m / 10] + LED_CLK_M_G[m % 10];
    }
    // 2.3添加闹钟标识
    uint16_t alarm_date = 0;
    if (show_type->is_12_hour) // 上下午
    {
        if (show_type->is_alarm_1)
        {
            alarm_date += 0x4000;
        }
        if (show_type->is_alarm_2)
        {
            alarm_date += 0x8000;
        }
    }
    data_left += alarm_date;
    // 这里关闭LED，从而达到一个亮度调节的效果
    HAL_GPIO_WritePin(VCCP00_GPIO_Port, VCCP00_Pin, GPIO_PIN_SET);
    vTaskDelay(touch_count);

    Inf_LED_Write(1, 0, 1, (data_left << 16) | data_right);
    vTaskDelay(5 - touch_count);
    // 这里关闭LED，从而达到一个亮度调节的效果
    HAL_GPIO_WritePin(VCCP01_GPIO_Port, VCCP01_Pin, GPIO_PIN_SET);
    vTaskDelay(touch_count);
    // 3.VCCP02
    Inf_LED_Write(1, 1, 0, (VCCP02_data << 16));
    vTaskDelay(5 - touch_count);
    // 这里关闭LED，从而达到一个亮度调节的效果
    HAL_GPIO_WritePin(VCCP02_GPIO_Port, VCCP02_Pin, GPIO_PIN_SET);
    vTaskDelay(touch_count);
    // 充电标识
    if (show_type->is_charging)
    {
        HAL_GPIO_WritePin(P03_GPIO_Port, P03_Pin, GPIO_PIN_RESET);
    }
}
uint32_t time_set_last_time = 0;
uint8_t led_flag = 0;
// num1 大于99表示不亮
// num2 大于99表示不亮
// point 1表示亮 0表示不亮
void Inf_led_time_set_show_num(uint16_t num1, uint16_t num2, uint8_t point)
{
    uint16_t vcc_p00_data = 0;
    uint16_t vcc_p01_data = 0;
    uint8_t point_data = 0;
    if (point) // 标记点是否亮
    {
        point_data = 0x20;
    }
    if (num1 > 99)
    {
        vcc_p00_data = point_data + 0;
    }
    else
    {
        vcc_p00_data = point_data + LED_CLK_H_S[num1 / 10] + LED_CLK_H_G[num1 % 10];
    }

    if (num2 > 99)
    {
        vcc_p01_data = 0;
    }
    else
    {
        vcc_p01_data = LED_CLK_M_S[num2 / 10] + LED_CLK_M_G[num2 % 10];
    }
    Inf_LED_Write(0, 1, 1, vcc_p00_data);
    vTaskDelay(5);
    Inf_LED_Write(1, 0, 1, vcc_p01_data);
    vTaskDelay(5);
}

void Inf_LED_TIME_SET(Show_time_set time_set, uint16_t value)
{
    if (xTaskGetTickCount() - time_set_last_time >= 500)
    {
        led_flag = (led_flag + 1) % 2;
        time_set_last_time = xTaskGetTickCount();
    }
    switch (time_set)
    {
    case TIME_SET_YEAR:
        // 显示年份->前面固定20,后面可以修改
        if (led_flag)
        {
            Inf_led_time_set_show_num(value / 100, value % 100, 0);
        }
        else
        {
            Inf_led_time_set_show_num(value / 100, 0xff, 0);
        }
        break;
    case TIME_SET_MONTH:
        // 显示月份
        if (led_flag)
        {
            Inf_led_time_set_show_num(value, 0xff, 0);
        }
        else
        {
            Inf_led_time_set_show_num(0xff, 0xff, 0);
        }
        break;
    case TIME_SET_DAY:
        // 显示日期
        if (led_flag)
        {
            Inf_led_time_set_show_num(0xff, value, 0);
        }
        else
        {
            Inf_led_time_set_show_num(0xff, 0xff, 0);
        }
        break;
    case TIME_SET_HOUR:
        if (led_flag)
        {
            Inf_led_time_set_show_num(value, 0xff, 1);
        }
        else
        {
            Inf_led_time_set_show_num(0xff, 0xff, 1);
        }
        break;
    case TIME_SET_MIN:
        if (led_flag)
        {
            Inf_led_time_set_show_num(0xff, value, 1);
        }
        else
        {
            Inf_led_time_set_show_num(0xff, 0xff, 1);
        }
        break;
    default:
        break;
    }
}
