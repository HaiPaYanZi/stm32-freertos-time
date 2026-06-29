#ifndef __INF_TOUCH_H
#define __INF_TOUCH_H

#include "gpio.h"
#include "main.h"
typedef enum
{
    TOUCH_NONE = 0,
    TOUCH_UP,
} Touch_value;

Touch_value Inf_get_touch_value(void);
#endif /* __INF_TOUCH_H */
