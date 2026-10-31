#ifndef __STM32F10x_CONF_H
#define __STM32F10x_CONF_H

/*
 * STM32F10x 标准外设库配置文件。
 * 本工程只使用 GPIO、RCC、USART 和 NVIC（misc）四个模块，
 * 因此这里只包含对应的头文件，避免把无关外设编进工程。
 */
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"

/*
 * 标准外设库的断言宏。
 * 不定义 USE_FULL_ASSERT 时展开为空，Release 构建不产生额外代码；
 * 如果定义 USE_FULL_ASSERT，需要用户实现 assert_failed()。
 */
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line);
#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
#else
#define assert_param(expr) ((void)0)
#endif

#endif
