#ifndef BSP_H
#define BSP_H

#include <stdio.h>
#include <stdarg.h>

#include <cpu.h>
#include <cpu_core.h>
#include <lib_def.h>
#include <stm32f10x_conf.h>

/* 板级初始化：NVIC 分组、USART1、SysTick、LED */
void BSP_Init(void);

/* 读取 HCLK 频率，供 uC/OS-III 时间戳使用 */
CPU_INT32U BSP_CPU_ClkFreq(void);

#endif
