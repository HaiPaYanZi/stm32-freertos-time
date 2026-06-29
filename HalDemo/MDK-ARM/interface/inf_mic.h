#ifndef __INF_MIC_H
#define __INF_MIC_H

#include "gpio.h"
#include "main.h"

typedef enum
{
    MIC_ON,
    MIC_OFF,
} Mic_value;

Mic_value Inf_get_mic_value(void);

#endif // __INF_MIC_H
