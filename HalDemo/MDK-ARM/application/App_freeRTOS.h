#ifndef __APP_FREERTOS_H
#define __APP_FREERTOS_H
#include "FreeRTOS.h"
#include "task.h"
#include "Com_debug.h"
/*
 *启动FreeRTOS在mian()中调用，后续代码不再生效
 *
 */
void App_freeRTOS_Start(void);

#endif /* __APP_FREERTOS_H */
