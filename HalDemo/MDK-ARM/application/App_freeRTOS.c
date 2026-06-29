#include "App_freeRTOS.h"
#include "inf_key.h"   //按键
#include "inf_touch.h" //触摸
#include "inf_mic.h"   //声控
// #include "inf_DS1302Z.h" //DS1302Z
#include "inf_DHT11.h" //DHT11
#include "inf_NV02D.h" //NV02D
#include "inf_led.h"   //LED
#include "App_dateTime.h"
#include "App_show.h"
#include "App_switch.h"
// TaskHandle_t task1_handle; // 任务句柄，可以用来操作任务，例如删除任务、挂起任务等
void task_collect(void *pvParameters);
#define COLLECT_TASK_STACK_SIZE 128
#define COLLECT_TASK_PRIORITY 1
TaskHandle_t task_collect_handle;
// 采集任务使用的量，一般采集任务进行修改
int8_t temperature = 0;
int8_t humidity = 0;
Clock_Date_Time_Type clock_date_time = {0};

void task_show(void *pvParameters);
#define SHOW_TASK_STACK_SIZE 128
#define SHOW_TASK_PRIORITY 1
TaskHandle_t task_show_handle;
// 显示模式
show_mode_type show_mode = SHOW_MODE_NORMAL;
// 表示12/24 表示am/pm  表示摄氏度/华氏度 表示闹钟 表示正负号 充电标识 5天闹钟标识
show_type_struct show_type = {.is_12_hour = 0,
                              .is_pm = 0,
                              .celsius = 1,
                              .is_alarm_1 = 0,
                              .is_alarm_2 = 0,
                              .is_alarm_five_day = 0,
                              .is_charging = 0};

Show_time_set show_time_set = TIME_SET_YEAR;
Show_alarm_set show_alarm_set = ALARM_SET_HOUR;

void task_switch(void *pvParameters);
#define SWITCH_TASK_STACK_SIZE 128
#define SWITCH_TASK_PRIORITY 1
TaskHandle_t task_switch_handle;

// 闹钟任务定义
void task_alarm(void *pvParameters);
#define ALARM_TASK_STACK_SIZE 128
#define ALARM_TASK_PRIORITY 4 // 这个要使用音频，为了适应底层驱动，要最高级
TaskHandle_t task_alarm_handle;

// 调节音量任务
void task_volume(void *pvParameters);
#define VOLUME_TASK_STACK_SIZE 128
#define VOLUME_TASK_PRIORITY 4 // 这个要使用nv02d，为了适应底层驱动，要最高级
TaskHandle_t task_volume_handle;

uint8_t nv02d_volume = 5;

// 触摸开关
uint8_t touch_count = 0;
// LED总开关
uint8_t led_en = 0;
/*
 *启动FreeRTOS在mian()中调用，后续代码不再生效
 *
 *
 */
void App_freeRTOS_Start(void)
{
    // 创建任务
    // 参数: 1. 任务函数指针
    // 2. 任务名称
    // 3. 任务堆栈大小
    // 4. 任务参数
    // 5. 任务优先级 数字越大优先级越高 范围0~configMAX_PRIORITIES-1
    // 6. 任务句柄 可以为空
    // xTaskCreate(task1, "task1", 128, NULL, 1, &task1_handle);

    xTaskCreate(task_collect, "collect", COLLECT_TASK_STACK_SIZE, NULL, COLLECT_TASK_PRIORITY, &task_collect_handle);
    xTaskCreate(task_show, "show", SHOW_TASK_STACK_SIZE, NULL, SHOW_TASK_PRIORITY, &task_show_handle);
    xTaskCreate(task_switch, "switch", SWITCH_TASK_STACK_SIZE, NULL, SWITCH_TASK_PRIORITY, &task_switch_handle);
    xTaskCreate(task_alarm, "alarm", ALARM_TASK_STACK_SIZE, NULL, ALARM_TASK_PRIORITY, &task_alarm_handle);
    xTaskCreate(task_volume, "alarm", VOLUME_TASK_STACK_SIZE, NULL, VOLUME_TASK_PRIORITY, &task_volume_handle);

    // 启动freeRTOS调度器
    vTaskStartScheduler();
}

void task_collect(void *pvParameters)
{
    Inf_DHT11_Init(); // 初始化温度传感器
    uint8_t collect_count = 9;
    while (1)
    {
        collect_count++;
        if (collect_count >= 10)
        {
            collect_count = 0;
            // 采集温湿度
            Inf_DHT11_Get_Data(&temperature, &humidity);
        }
        if (show_mode != SHOW_MODE_TIME_SET)
        {
            // 采集时间
            App_dateTime_get(&clock_date_time);
        }

        vTaskDelay(1000);
    }
}

void task_show(void *pvParameters)
{
    void App_show_start();
    while (1)
    {
        if (led_en)
        {
            if (show_mode == SHOW_MODE_NORMAL || show_mode == SHOW_MODE_VOLUME)
            {
                App_show_normal(&clock_date_time, temperature, humidity, &show_type);
            }
            else if (show_mode == SHOW_MODE_TIME_SET)
            {
                App_show_time_set(&clock_date_time, temperature, humidity, &show_type);
            }
            else if (show_mode == SHOW_MODE_ALARM_SET)
            {
                App_show_alarm_set(&clock_date_time, temperature, humidity, &show_type);
            }
        }
        else
        {
            Inf_LED_stop();
        }
    }
}

void task_switch(void *pvParameters)
{
    while (1)
    {
        if (show_mode == SHOW_MODE_NORMAL)
        {
            // 按键开关
            App_switch_key_process();
        }
        else if (show_mode == SHOW_MODE_TIME_SET)
        {
            App_switch_time_set_process();
        }
        else if (show_mode == SHOW_MODE_ALARM_SET)
        {
            App_show_alarm_set_process();
        }

        // 拨动开关
        App_switch_rotary_process();
        // 触摸开关
        Touch_value touch_value = Inf_get_touch_value();
        if (touch_value == TOUCH_UP)
        {
            touch_count++;
            if (touch_count >= 5)
            {
                touch_count = 0;
            }
        }
        vTaskDelay(50);
    }
}
// 闹钟结构体
Alarm_Time_Type alarm_time1 = {0};
Alarm_Time_Type alarm_time2 = {0};

// 闹钟任务
void task_alarm(void *pvParameters)
{
    Inf_NV02D_Init();
    uint8_t alarm_count = 0;
    uint8_t alarm_touch_count = 0;
    while (1)
    {
        // 只有在正常模式下才有闹钟响应
        // 5天闹钟
        if (show_type.is_alarm_five_day == 1 && clock_date_time.week >= 6)
        {
        }
        else
        {
            if (show_mode == SHOW_MODE_NORMAL)
            {
                if (show_type.is_alarm_1 == 1)
                {
                    if (alarm_time1.hour == clock_date_time.hour && alarm_time1.min == clock_date_time.min)
                    {
                        // 触发闹钟
                        if (alarm_time1.is_on == 0)
                        {
                            alarm_time1.is_on = 1;
                            Inf_NV02D_send_cmd(0x00);
                            alarm_touch_count = touch_count; // 保持触摸的计数值，当不同是表示被触摸了
                        }
                        else if (alarm_time1.is_on == 1 && alarm_time1.is_done == 0)
                        {
                            alarm_count++;
                            if (alarm_count >= 5)
                            {
                                Inf_NV02D_send_cmd(0x00);
                                alarm_count = 0;
                            }
                        }

                        if (alarm_touch_count != touch_count)
                        {
                            Inf_NV02D_send_cmd(0xfe);
                            // 触摸后记录一下已经响过了
                            alarm_time1.is_done = 1;
                        }
                    }
                    else
                    {
                        // 过了1分钟，就关闭
                        if (alarm_time1.is_on == 1)
                        {
                            alarm_time1.is_on = 0;
                            alarm_time1.is_done = 0;
                            Inf_NV02D_send_cmd(0xfe);
                        }
                    }
                }
                else if (show_type.is_alarm_2 == 1)
                {
                    if (alarm_time2.hour == clock_date_time.hour && alarm_time2.min == clock_date_time.min)
                    {
                        // 触发闹钟
                        if (alarm_time2.is_on == 0)
                        {
                            alarm_time2.is_on = 1;
                            Inf_NV02D_send_cmd(0x00);
                            alarm_touch_count = touch_count; // 保持触摸的计数值，当不同是表示被触摸了
                        }
                        else if (alarm_time2.is_on == 1 && alarm_time2.is_done == 0)
                        {
                            alarm_count++;
                            if (alarm_count >= 5)
                            {
                                Inf_NV02D_send_cmd(0x00);
                                alarm_count = 0;
                            }
                        }

                        if (alarm_touch_count != touch_count)
                        {
                            Inf_NV02D_send_cmd(0xfe);
                            // 触摸后记录一下已经响过了
                            alarm_time2.is_done = 1;
                        }
                    }
                    else
                    {
                        // 过了1分钟，就关闭
                        if (alarm_time2.is_on == 1)
                        {
                            alarm_time2.is_on = 0;
                            alarm_time2.is_done = 0;
                            Inf_NV02D_send_cmd(0xfe);
                        }
                    }
                }
            }
        }

        vTaskDelay(1000);
    }
}

void task_volume(void *pvParameters)
{
    // 初始化
    Inf_NV02D_Init();
    Inf_NV02D_send_cmd(0xE0 + nv02d_volume);
    uint8_t volume_count = 99;
    while (1)
    {
        if (show_mode == SHOW_MODE_VOLUME)
        {
            volume_count++;
            if (volume_count >= 100)
            {
                Inf_NV02D_send_cmd(0x00);
                volume_count = 0;
            }
            // 接收按键值
            uint8_t key_value = Inf_get_key_value();
            switch (key_value)
            {
            case KEY_UP:
                nv02d_volume++;
                if (nv02d_volume > 7)
                {
                    nv02d_volume = 7;
                }
                Inf_NV02D_send_cmd(0xE0 + nv02d_volume); // 设置音量
                break;
            case KEY_DOWN:
                nv02d_volume--;
                if (nv02d_volume < 0)
                {
                    nv02d_volume = 0;
                }
                Inf_NV02D_send_cmd(0xE0 + nv02d_volume);
                break;
            case KEY_ALARM_SET:
                Inf_NV02D_send_cmd(0xFE); // 停止播放音乐
                volume_count = 99;
                show_mode = SHOW_MODE_NORMAL;
                break;
            default:
                break;
            }
        }
        vTaskDelay(50);
    }
}