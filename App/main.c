#include "includes.h"
#include "app.h"

/*
 * 任务二主程序：uC/OS-III + AHT20 温湿度采集
 *
 * 硬件：
 *   LED1 = PC13（低电平点亮）
 *   LED2 = PB0 （高电平点亮）
 *   AHT20：SCL=PB6，SDA=PB7，VCC=3.3 V，GND=GND
 *   USART1：PA9=TX，PA10=RX，115200-8-N-1
 *
 * 注意：
 *   AHT20 的 SCL/SDA 需要 4.7 kΩ 上拉到 3.3 V。
 *   如果模块自带上拉，可以先不外加；通信不稳定时再补。
 */
int main(void)
{
    OS_ERR err;

    BSP_Init();                 /* 时钟、GPIO、USART1、SysTick */

    OSInit(&err);
    if (err != OS_ERR_NONE)
    {
        while (1) { }
    }

    OSTaskCreate(&AppTaskStartTCB,
                 "START",
                 AppTaskStart,
                 (void *)0,
                 APP_CFG_TASK_START_PRIO,
                 &AppTaskStartStk[0],
                 APP_CFG_TASK_START_STK_SIZE / 10u,
                 APP_CFG_TASK_START_STK_SIZE,
                 0u,
                 0u,
                 (void *)0,
                 OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                 &err);
    if (err != OS_ERR_NONE)
    {
        while (1) { }
    }

    OSStart(&err);

    while (1) { }
}
