#include "app.h"
#include "led.h"
#include "uart.h"
#include "i2c_soft.h"
#include "aht20.h"

/*
 * 任务二：在任务一的基础上，把第 3 个任务改成 AHT20 采集任务
 *   LED1_Task   ：每 1 s 翻转 PC13
 *   LED2_Task   ：每 3 s 翻转 PB0
 *   SENSOR_Task ：每 2 s 采集一次 AHT20，并通过 USART1 发送
 *
 * 任务优先级：LED1 > LED2 > SENSOR
 * 所有任务都用 OSTimeDlyHMSM() 主动阻塞，AHT20 的 80 ms 测量等待
 * 也在 delay_ms() 中转化为 OSTimeDly()，不会空转占用 CPU。
 */

OS_TCB  AppTaskStartTCB;
CPU_STK AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static OS_TCB  AppTaskLED1TCB;
static CPU_STK AppTaskLED1Stk[APP_CFG_TASK_LED1_STK_SIZE];

static OS_TCB  AppTaskLED2TCB;
static CPU_STK AppTaskLED2Stk[APP_CFG_TASK_LED2_STK_SIZE];

static OS_TCB  AppTaskSensorTCB;
static CPU_STK AppTaskSensorStk[APP_CFG_TASK_SENSOR_STK_SIZE];

static void AppTaskLED1(void *p_arg);
static void AppTaskLED2(void *p_arg);
static void AppTaskSensor(void *p_arg);

/*
 * 函数名：AppTaskCreate
 * 功能  ：创建三个应用任务
 */
void AppTaskCreate(void)
{
    OS_ERR err;

    OSTaskCreate(&AppTaskLED1TCB,
                 "LED1_1s",
                 AppTaskLED1,
                 (void *)0,
                 APP_CFG_TASK_LED1_PRIO,
                 &AppTaskLED1Stk[0],
                 APP_CFG_TASK_LED1_STK_SIZE / 10u,
                 APP_CFG_TASK_LED1_STK_SIZE,
                 0u,
                 0u,
                 (void *)0,
                 OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 &err);
    if (err != OS_ERR_NONE)
    {
        while (1) { }
    }

    OSTaskCreate(&AppTaskLED2TCB,
                 "LED2_3s",
                 AppTaskLED2,
                 (void *)0,
                 APP_CFG_TASK_LED2_PRIO,
                 &AppTaskLED2Stk[0],
                 APP_CFG_TASK_LED2_STK_SIZE / 10u,
                 APP_CFG_TASK_LED2_STK_SIZE,
                 0u,
                 0u,
                 (void *)0,
                 OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 &err);
    if (err != OS_ERR_NONE)
    {
        while (1) { }
    }

    OSTaskCreate(&AppTaskSensorTCB,
                 "AHT20_2s",
                 AppTaskSensor,
                 (void *)0,
                 APP_CFG_TASK_SENSOR_PRIO,
                 &AppTaskSensorStk[0],
                 APP_CFG_TASK_SENSOR_STK_SIZE / 10u,
                 APP_CFG_TASK_SENSOR_STK_SIZE,
                 0u,
                 0u,
                 (void *)0,
                 OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 &err);
    if (err != OS_ERR_NONE)
    {
        while (1) { }
    }
}

/*
 * 函数名：AppTaskStart
 * 功能  ：启动任务：初始化 CPU，创建应用任务，然后挂起自己
 */
void AppTaskStart(void *p_arg)
{
    OS_ERR err;

    (void)p_arg;

    CPU_Init();
    AppTaskCreate();

    OSTaskSuspend(&AppTaskStartTCB, &err);
    while (1) { }
}

/*
 * 函数名：AppTaskLED1
 * 功能  ：每 1 s 翻转一次 LED1
 */
static void AppTaskLED1(void *p_arg)
{
    OS_ERR err;

    (void)p_arg;

    while (1)
    {
        LED_Toggle(LED1);
        OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

/*
 * 函数名：AppTaskLED2
 * 功能  ：每 3 s 翻转一次 LED2
 */
static void AppTaskLED2(void *p_arg)
{
    OS_ERR err;

    (void)p_arg;

    while (1)
    {
        LED_Toggle(LED2);
        OSTimeDlyHMSM(0u, 0u, 3u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

/*
 * 函数名：AppTaskSensor
 * 功能  ：每 2 s 采集一次 AHT20，并通过串口输出
 * 输出格式：
 *   AHT20: T=25.31 C, RH=48.72 %
 * 上位机可以按 "T=" 和 "RH=" 解析。
 */
static void AppTaskSensor(void *p_arg)
{
    OS_ERR err;
    AHT20_Data_t data;
    uint8_t ret;

    (void)p_arg;

    I2C_Soft_Init();            /* PB6/PB7 开漏输出，总线恢复 */

    Uart1_Printf("\r\nAHT20 温湿度采集任务启动\r\n");

    ret = AHT20_Init();
    if (ret != 0u)
    {
        Uart1_Printf("AHT20 init failed, code=%u\r\n", ret);
    }

    while (1)
    {
        ret = AHT20_Read(&data);

        if (ret == 0u)
        {
            Uart1_Printf("AHT20: T=%.2f C, RH=%.2f %%\r\n",
                         data.temperature,
                         data.humidity);
        }
        else
        {
            Uart1_Printf("AHT20 read error, code=%u\r\n", ret);
        }

        OSTimeDlyHMSM(0u, 0u, 2u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}
