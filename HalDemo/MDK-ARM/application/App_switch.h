#ifndef __APP_SWITCH_H_
#define __APP_SWITCH_H_
#include "inf_key.h"
#include "inf_led.h"
#include "inf_mic.h"
#include "FreeRTOS.h"
#include "task.h"
#include "App_show.h"
void App_switch_key_process(void);
void App_switch_rotary_process(void);
void App_switch_time_set_process(void);
void App_show_alarm_set_process(void);
#endif
