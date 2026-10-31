#include "uart.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/*
 * 函数名：Uart1_Init
 * 功能  ：配置 USART1 为 115200-8-N-1，仅使用查询发送
 * 说明  ：本实验不需要串口接收中断；如果上位机要下发命令，
 *         可以在此基础上增加 USART1_IRQHandler 和环形缓冲区。
 */
void Uart1_Init(void)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    /* PA9：复用推挽输出，作为 USART1_TX */
    gpio.GPIO_Pin   = GPIO_Pin_9;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    /* PA10：浮空输入，作为 USART1_RX */
    gpio.GPIO_Pin  = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    usart.USART_BaudRate            = 115200;
    usart.USART_WordLength          = USART_WordLength_8b;
    usart.USART_StopBits            = USART_StopBits_1;
    usart.USART_Parity              = USART_Parity_No;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &usart);
    USART_Cmd(USART1, ENABLE);
}

/*
 * 函数名：Uart1_SendChar
 * 功能  ：查询 TXE 标志，发送一个字节
 */
void Uart1_SendChar(uint8_t ch)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
        /* 等待发送数据寄存器空 */
    }
    USART_SendData(USART1, (uint16_t)ch);
}

/*
 * 函数名：Uart1_SendString
 * 功能  ：发送以 '\0' 结尾的字符串
 */
void Uart1_SendString(const char *str)
{
    while (*str != '\0')
    {
        Uart1_SendChar((uint8_t)*str);
        str++;
    }
}

/*
 * 函数名：Uart1_Printf
 * 功能  ：格式化输出到串口
 * 说明  ：缓冲区 160 字节，足够本实验的温湿度行；
 *         如果格式串更长，请适当增大 buffer。
 */
void Uart1_Printf(const char *fmt, ...)
{
    char buf[160];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    Uart1_SendString(buf);
}
