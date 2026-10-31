#ifndef DELAY_H
#define DELAY_H

#include "stm32f10x.h"

/* 初始化 SysTick 作为 uC/OS-III 系统节拍，并使能 DWT 周期计数器 */
void delay_init(void);

/* 微秒级忙等待，使用 DWT->CYCCNT，不依赖 SysTick */
void delay_us(uint32_t nus);

/* 毫秒级延时：OS 运行时让出 CPU；OS 未运行时忙等待 */
void delay_ms(uint16_t nms);

#endif
