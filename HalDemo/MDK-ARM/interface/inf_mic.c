#include "inf_mic.h"

Mic_value Inf_get_mic_value(void)
{
    // 调试时0为有声
    // 1为无声
    if (HAL_GPIO_ReadPin(MIC_IN_GPIO_Port, MIC_IN_Pin) == GPIO_PIN_SET)
    {
        return MIC_ON;
    }
    else
    {
        return MIC_OFF;
    }
}
