#include "i2c_soft.h"
#include "delay.h"
#include "dwt.h"

/*
 * GPIO 操作宏。
 * 开漏输出 + 外部上拉：写 1 释放总线，写 0 拉低总线。
 * 读取 SDA 时直接读 IDR，开漏模式下仍然可以正确反映引脚电平。
 */
#define I2C_SCL_H()                 GPIO_SetBits(I2C_SOFT_PORT, I2C_SOFT_SCL_PIN)
#define I2C_SCL_L()                 GPIO_ResetBits(I2C_SOFT_PORT, I2C_SOFT_SCL_PIN)
#define I2C_SDA_H()                 GPIO_SetBits(I2C_SOFT_PORT, I2C_SOFT_SDA_PIN)
#define I2C_SDA_L()                 GPIO_ResetBits(I2C_SOFT_PORT, I2C_SOFT_SDA_PIN)
#define I2C_SDA_READ()              GPIO_ReadInputDataBit(I2C_SOFT_PORT, I2C_SOFT_SDA_PIN)

/*
 * 100 kHz 标准模式：半周期约 5 us。
 * 使用 DWT 周期计数器，不依赖 SysTick，也不会被 OS 节拍打断计时。
 */
static void I2C_DelayUs(uint16_t us)
{
    uint32_t start;
    uint32_t ticks;

    start = DWT->CYCCNT;
    ticks = (uint32_t)us * (SystemCoreClock / 1000000u);

    while ((DWT->CYCCNT - start) < ticks)
    {
        __NOP();
    }
}

/*
 * I2C 起始条件：
 *   SCL 高电平期间，SDA 由高变低。
 */
static void I2C_Start(void)
{
    I2C_SDA_H();
    I2C_SCL_H();
    I2C_DelayUs(5);

    I2C_SDA_L();
    I2C_DelayUs(5);

    I2C_SCL_L();
    I2C_DelayUs(5);
}

/*
 * I2C 停止条件：
 *   SCL 高电平期间，SDA 由低变高。
 */
static void I2C_Stop(void)
{
    I2C_SDA_L();
    I2C_DelayUs(5);

    I2C_SCL_H();
    I2C_DelayUs(5);

    I2C_SDA_H();
    I2C_DelayUs(5);
}

/*
 * 写一个字节，返回从机应答：
 *   0 = ACK（SDA 低）
 *   1 = NACK（SDA 高）
 */
static uint8_t I2C_WriteByte(uint8_t byte)
{
    uint8_t i;
    uint8_t ack;

    for (i = 0u; i < 8u; i++)
    {
        if ((byte & 0x80u) != 0u)
        {
            I2C_SDA_H();
        }
        else
        {
            I2C_SDA_L();
        }
        byte <<= 1;

        I2C_DelayUs(5);
        I2C_SCL_H();
        I2C_DelayUs(5);
        I2C_SCL_L();
        I2C_DelayUs(5);
    }

    /* 第 9 个时钟：释放 SDA，让从机拉低表示 ACK */
    I2C_SDA_H();
    I2C_DelayUs(5);
    I2C_SCL_H();
    I2C_DelayUs(5);

    ack = I2C_SDA_READ();

    I2C_SCL_L();
    I2C_DelayUs(5);

    return ack;
}

/*
 * 读一个字节。
 *   ack = 1：读完这个字节后回 ACK，继续读
 *   ack = 0：读完这个字节后回 NACK，准备停止
 */
static uint8_t I2C_ReadByte(uint8_t ack)
{
    uint8_t i;
    uint8_t byte;

    byte = 0u;
    I2C_SDA_H();                 /* 释放 SDA，由从机驱动 */

    for (i = 0u; i < 8u; i++)
    {
        byte <<= 1;

        I2C_DelayUs(5);
        I2C_SCL_H();
        I2C_DelayUs(5);

        if (I2C_SDA_READ() != 0u)
        {
            byte |= 0x01u;
        }

        I2C_SCL_L();
        I2C_DelayUs(5);
    }

    /* 主机应答位 */
    if (ack != 0u)
    {
        I2C_SDA_L();             /* ACK */
    }
    else
    {
        I2C_SDA_H();             /* NACK */
    }
    I2C_DelayUs(5);

    I2C_SCL_H();
    I2C_DelayUs(5);
    I2C_SCL_L();
    I2C_DelayUs(5);

    I2C_SDA_H();                 /* 释放总线 */

    return byte;
}

/*
 * 总线恢复：
 *   如果从机把 SDA 一直拉低（例如上次复位时正在读数据），
 *   主机发送 9 个 SCL 脉冲，让从机把剩余字节发完并释放 SDA，
 *   然后补一个 STOP 条件。
 */
static void I2C_BusRecover(void)
{
    uint8_t i;

    I2C_SDA_H();
    I2C_DelayUs(5);

    if (I2C_SDA_READ() == 0u)
    {
        for (i = 0u; i < 9u; i++)
        {
            I2C_SCL_L();
            I2C_DelayUs(5);
            I2C_SCL_H();
            I2C_DelayUs(5);
        }
        I2C_Stop();
    }
}

/*
 * 函数名：I2C_Soft_Init
 * 功能  ：初始化 PB6/PB7 为开漏输出，并使能 DWT
 */
void I2C_Soft_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    gpio.GPIO_Pin   = I2C_SOFT_SCL_PIN | I2C_SOFT_SDA_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_Out_OD;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_SOFT_PORT, &gpio);

    /* 释放总线，空闲状态 SCL/SDA 都为高 */
    GPIO_SetBits(I2C_SOFT_PORT, I2C_SOFT_SCL_PIN | I2C_SOFT_SDA_PIN);
    I2C_DelayUs(10);

    I2C_BusRecover();
}

/*
 * 函数名：I2C_Soft_Write
 * 功能  ：向 7 位地址从机写 len 个字节
 * 返回  ：0 成功；1 地址 NACK；2 数据 NACK
 */
uint8_t I2C_Soft_Write(uint8_t dev_addr, const uint8_t *buf, uint16_t len)
{
    uint16_t i;

    if ((buf == 0) && (len != 0u))
    {
        return 3u;
    }

    I2C_Start();

    if (I2C_WriteByte((uint8_t)(dev_addr << 1)) != 0u)
    {
        I2C_Stop();
        return 1u;
    }

    for (i = 0u; i < len; i++)
    {
        if (I2C_WriteByte(buf[i]) != 0u)
        {
            I2C_Stop();
            return 2u;
        }
    }

    I2C_Stop();
    return 0u;
}

/*
 * 函数名：I2C_Soft_Read
 * 功能  ：从 7 位地址从机读 len 个字节
 * 返回  ：0 成功；1 地址 NACK；3 参数错误
 */
uint8_t I2C_Soft_Read(uint8_t dev_addr, uint8_t *buf, uint16_t len)
{
    uint16_t i;

    if ((buf == 0) || (len == 0u))
    {
        return 3u;
    }

    I2C_Start();

    if (I2C_WriteByte((uint8_t)((dev_addr << 1) | 0x01u)) != 0u)
    {
        I2C_Stop();
        return 1u;
    }

    for (i = 0u; i < len; i++)
    {
        /* 最后一个字节回 NACK，其余回 ACK */
        if (i == (uint16_t)(len - 1u))
        {
            buf[i] = I2C_ReadByte(0u);
        }
        else
        {
            buf[i] = I2C_ReadByte(1u);
        }
    }

    I2C_Stop();
    return 0u;
}
