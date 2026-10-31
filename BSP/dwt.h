#ifndef DWT_H
#define DWT_H

#include "stm32f10x.h"

/*
 * 部分老版本 CMSIS（例如 STM32F10x 标准库里带的 core_cm3.h）
 * 没有定义 DWT 结构体，这里补一个最小定义。
 * DWT 基地址：0xE0001000
 *   +0x00 DWT_CTRL
 *   +0x04 DWT_CYCCNT
 */
#ifndef DWT
typedef struct
{
    volatile uint32_t CTRL;
    volatile uint32_t CYCCNT;
    volatile uint32_t CPICNT;
    volatile uint32_t EXCCNT;
    volatile uint32_t SLEEPCNT;
    volatile uint32_t LSUCNT;
    volatile uint32_t FOLDCNT;
    volatile uint32_t PCSR;
} DWT_Type;

#define DWT                         ((DWT_Type *)0xE0001000UL)
#endif

#ifndef DWT_CTRL_CYCCNTENA_Msk
#define DWT_CTRL_CYCCNTENA_Msk      (1UL << 0)
#endif

#endif
