#ifndef UART_H
#define UART_H

#include "stm32f10x.h"

/*
 * USART1：
 *   PA9  -> TX（接 DAP-Link 的 RXD）
 *   PA10 <- RX（接 DAP-Link 的 TXD）
 *   115200 bps，8 数据位，1 停止位，无校验
 */
void Uart1_Init(void);
void Uart1_SendChar(uint8_t ch);
void Uart1_SendString(const char *str);
void Uart1_Printf(const char *fmt, ...);

#endif
