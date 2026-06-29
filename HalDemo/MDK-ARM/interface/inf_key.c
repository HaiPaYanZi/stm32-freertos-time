#include "inf_key.h"

void Inf_key_init(void)
{
    // 这个初始化函数无用
    // 已在main()中调用MX_GPIO_Init();
}

// 获取指定按键的状态
uint8_t Inf_get_key_press(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);                                          // 消抖
        if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET) // 确定按下
        {
            while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(1); // 等待按键释放
            }
            return 1; // 返回按键状态
        }
    }
    return 0; // 没有按键被按下
}

// 获取当前按键的状态
Key_Type_value Inf_get_key_value(void)
{
    if (Inf_get_key_press(UP_GPIO_Port, UP_Pin) == 1)
    {
        return KEY_UP;
    }
    else if (Inf_get_key_press(DOWN_GPIO_Port, DOWN_Pin) == 1)
    {
        return KEY_DOWN;
    }
    else if (Inf_get_key_press(ALARM_EN_GPIO_Port, ALARM_EN_Pin) == 1)
    {
        return KEY_ALARM_EN;
    }
    else if (Inf_get_key_press(ALARM_5_GPIO_Port, ALARM_5_Pin) == 1)
    {
        return KEY_ALARM_5;
    }
    // 长按逻辑
    // 时间设置
    if (HAL_GPIO_ReadPin(TIM_SET_GPIO_Port, TIM_SET_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10); // 消抖
        if (HAL_GPIO_ReadPin(TIM_SET_GPIO_Port, TIM_SET_Pin) == GPIO_PIN_RESET)
        {
            TickType_t start_tick = xTaskGetTickCount();
            while (HAL_GPIO_ReadPin(TIM_SET_GPIO_Port, TIM_SET_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(1); // 等待按键释放
            }
            TickType_t end_tick = xTaskGetTickCount(); // 函数是系统节拍计数器的值，单位是系统节拍的周期（通常是1ms），可以用来计算按键按下的持续时间。
            if (end_tick - start_tick >= 3000)
            {
                return KEY_TIM_SET_LONG;
            }
            else
            {
                return KEY_TIM_SET;
            }
        }
    }
    // 闹钟设置
    if (HAL_GPIO_ReadPin(ALARM_SET_GPIO_Port, ALARM_SET_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10); // 消抖
        if (HAL_GPIO_ReadPin(ALARM_SET_GPIO_Port, ALARM_SET_Pin) == GPIO_PIN_RESET)
        {
            TickType_t start_tick = xTaskGetTickCount();
            while (HAL_GPIO_ReadPin(ALARM_SET_GPIO_Port, ALARM_SET_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(1); // 等待按键释放
            }
            TickType_t end_tick = xTaskGetTickCount();
            if (end_tick - start_tick >= 3000)
            {
                return KEY_ALARM_SET_LONG;
            }
            else
            {
                return KEY_ALARM_SET;
            }
        }
    }
    return KEY_NONE; // 没有按键被按下
}

// 获取拨动开关的状态
LED_ON_Type_value Inf_get_led_on_value(void)
{
    if (HAL_GPIO_ReadPin(LED_ON_GPIO_Port, LED_ON_Pin) == GPIO_PIN_RESET)
    {
        return LED_OFF;
    }
    return LED_ON;
}

// 常亮与声控
LIGHT_ON_Type_value Inf_get_light_on_value(void)
{
    if (HAL_GPIO_ReadPin(LIGHT_GPIO_Port, LIGHT_Pin) == GPIO_PIN_RESET)
    {
        return LIGHT_OFF;
    }
    return LIGHT_ON;
}
