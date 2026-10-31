#include "led.h"

#define LED1_ACTIVE_LOW             1u
#define LED2_ACTIVE_LOW             0u

/*
 * 函数名：LED_Init
 * 功能  ：初始化 PC13 和 PB0 为推挽输出，并默认熄灭
 */
void LED_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB, ENABLE);

    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;

    gpio.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &gpio);

    LED_Off(LED1);
    LED_Off(LED2);
}

/*
 * 函数名：LED_On
 * 功能  ：点亮指定 LED
 * 参数  ：led = LED1 或 LED2
 */
void LED_On(uint8_t led)
{
    if (led == LED1)
    {
#if (LED1_ACTIVE_LOW == 1u)
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
#else
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
#endif
    }
    else if (led == LED2)
    {
#if (LED2_ACTIVE_LOW == 1u)
        GPIO_ResetBits(GPIOB, GPIO_Pin_0);
#else
        GPIO_SetBits(GPIOB, GPIO_Pin_0);
#endif
    }
}

/*
 * 函数名：LED_Off
 * 功能  ：熄灭指定 LED
 */
void LED_Off(uint8_t led)
{
    if (led == LED1)
    {
#if (LED1_ACTIVE_LOW == 1u)
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
#else
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
#endif
    }
    else if (led == LED2)
    {
#if (LED2_ACTIVE_LOW == 1u)
        GPIO_SetBits(GPIOB, GPIO_Pin_0);
#else
        GPIO_ResetBits(GPIOB, GPIO_Pin_0);
#endif
    }
}

/*
 * 函数名：LED_Toggle
 * 功能  ：翻转指定 LED 的亮灭状态
 * 说明  ：读 ODR 的当前输出电平，再取反，避免额外维护一个状态变量。
 */
void LED_Toggle(uint8_t led)
{
    uint8_t level;

    if (led == LED1)
    {
        level = GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13);
        if (level == 0u)
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_13);
        }
        else
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        }
    }
    else if (led == LED2)
    {
        level = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_0);
        if (level == 0u)
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_0);
        }
        else
        {
            GPIO_ResetBits(GPIOB, GPIO_Pin_0);
        }
    }
}
