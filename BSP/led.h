#ifndef LED_H
#define LED_H

#include "stm32f10x.h"

/*
 * 板上 LED 连接：
 *   LED1：PC13，低电平点亮（Blue Pill 板载 LED）
 *   LED2：PB0 ，高电平点亮（外接 LED，串 330 Ω 到 GND）
 *
 * 如果你的板子 LED 极性相反，只需要修改 led.c 中的 LED1_ACTIVE_LOW /
 * LED2_ACTIVE_LOW 两个宏，不要改任务代码。
 */
#define LED1                        1u
#define LED2                        2u

void LED_Init(void);
void LED_On(uint8_t led);
void LED_Off(uint8_t led);
void LED_Toggle(uint8_t led);

#endif
