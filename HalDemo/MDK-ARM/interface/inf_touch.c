#include "inf_touch.h"
Touch_value Inf_get_touch_value(void)
{
    if (HAL_GPIO_ReadPin(TOUCH_GPIO_Port, TOUCH_Pin) == GPIO_PIN_RESET)
    {
        return TOUCH_NONE;
    }
    return TOUCH_UP;
}
