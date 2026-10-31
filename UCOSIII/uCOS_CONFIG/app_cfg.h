#ifndef APP_CFG_H
#define APP_CFG_H

/*
 * 应用任务配置
 * 说明：uC/OS-III 中优先级数值越小，优先级越高。
 *       Tick 任务优先级为 1，Timer 任务为 2，应用任务从 3 开始。
 */
#define APP_CFG_TASK_START_PRIO             3u
#define APP_CFG_TASK_LED1_PRIO              4u
#define APP_CFG_TASK_LED2_PRIO              5u
#define APP_CFG_TASK_UART_PRIO              6u
#define APP_CFG_TASK_SENSOR_PRIO            6u

/*
 * 任务堆栈大小，单位为 CPU_STK（STM32 上为 4 字节）。
 * 128  => 512 字节
 * 256  => 1024 字节
 * STM32F103C8T6 只有 20 KB RAM，堆栈不宜盲目加大。
 */
#define APP_CFG_TASK_START_STK_SIZE         128u
#define APP_CFG_TASK_LED1_STK_SIZE          128u
#define APP_CFG_TASK_LED2_STK_SIZE          128u
#define APP_CFG_TASK_UART_STK_SIZE          256u
#define APP_CFG_TASK_SENSOR_STK_SIZE        256u

/* 堆栈使用率检查阈值：剩余空间低于 10% 时认为接近溢出 */
#define APP_CFG_TASK_STK_LIMIT_PCT_EMPTY    10u

#endif
