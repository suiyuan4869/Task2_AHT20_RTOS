#include "aht20.h"
#include "i2c_soft.h"
#include "delay.h"

/*
 * 是否校验 AHT20 返回的第 7 个字节（CRC）。
 * 如果买到兼容模块、CRC 不稳定，可以把这里改成 0 先验证数据趋势。
 */
#define AHT20_CRC_EN                1u

/*
 * CRC-8：
 *   多项式 x^8 + x^5 + x^4 + 1，即 0x31
 *   初值 0xFF，覆盖前 6 个数据字节。
 */
static uint8_t AHT20_CRC8(const uint8_t *data, uint8_t len)
{
    uint8_t crc;
    uint8_t i;
    uint8_t bit;

    crc = 0xFFu;

    for (i = 0u; i < len; i++)
    {
        crc ^= data[i];
        for (bit = 0u; bit < 8u; bit++)
        {
            if ((crc & 0x80u) != 0u)
            {
                crc = (uint8_t)((crc << 1) ^ 0x31u);
            }
            else
            {
                crc = (uint8_t)(crc << 1);
            }
        }
    }

    return crc;
}

/*
 * 函数名：AHT20_ReadStatus
 * 功能  ：发送 0x71，读回 1 个状态字节
 * 返回  ：0 成功；1/2 为 I2C 错误
 */
uint8_t AHT20_ReadStatus(uint8_t *status)
{
    uint8_t cmd;

    if (status == 0)
    {
        return 3u;
    }

    cmd = 0x71u;

    if (I2C_Soft_Write(AHT20_I2C_ADDR, &cmd, 1u) != 0u)
    {
        return 1u;
    }

    if (I2C_Soft_Read(AHT20_I2C_ADDR, status, 1u) != 0u)
    {
        return 2u;
    }

    return 0u;
}

/*
 * 函数名：AHT20_Init
 * 功能  ：AHT20 上电初始化
 * 说明  ：上电后至少等待 40 ms；若状态 bit3 为 0，说明未校准，
 *         发送 0xBE 0x08 0x00 命令，再等待 10 ms。
 */
uint8_t AHT20_Init(void)
{
    uint8_t status;
    uint8_t cmd[3];

    delay_ms(40u);              /* 上电稳定时间 */

    if (AHT20_ReadStatus(&status) != 0u)
    {
        return 1u;
    }

    if ((status & 0x08u) == 0u)
    {
        cmd[0] = 0xBEu;
        cmd[1] = 0x08u;
        cmd[2] = 0x00u;

        if (I2C_Soft_Write(AHT20_I2C_ADDR, cmd, 3u) != 0u)
        {
            return 2u;
        }

        delay_ms(10u);
    }

    /* 再读一次状态，确认校准完成 */
    if (AHT20_ReadStatus(&status) != 0u)
    {
        return 3u;
    }

    if ((status & 0x08u) == 0u)
    {
        return 4u;
    }

    return 0u;
}

/*
 * 函数名：AHT20_Read
 * 功能  ：触发一次温湿度测量并换算
 *
 * 数据格式（7 字节）：
 *   buf[0]     状态
 *   buf[1..2]  湿度高 16 位
 *   buf[3]     湿度低 4 位（高半字节）+ 温度高 4 位（低半字节）
 *   buf[4..5]  温度低 16 位
 *   buf[6]     CRC
 *
 * 换算：
 *   RH  = raw_h / 2^20 * 100 %
 *   T   = raw_t / 2^20 * 200 - 50 ℃
 */
uint8_t AHT20_Read(AHT20_Data_t *data)
{
    uint8_t cmd[3];
    uint8_t buf[7];
    uint32_t raw_h;
    uint32_t raw_t;

    if (data == 0)
    {
        return 0xFFu;
    }

    cmd[0] = 0xACu;             /* 触发测量 */
    cmd[1] = 0x33u;
    cmd[2] = 0x00u;

    if (I2C_Soft_Write(AHT20_I2C_ADDR, cmd, 3u) != 0u)
    {
        return 1u;
    }

    delay_ms(80u);              /* 测量时间，手册要求 ≥ 80 ms */

    if (I2C_Soft_Read(AHT20_I2C_ADDR, buf, 7u) != 0u)
    {
        return 2u;
    }

    if ((buf[0] & 0x80u) != 0u)
    {
        return 3u;              /* 忙，数据无效 */
    }

#if (AHT20_CRC_EN == 1u)
    if (AHT20_CRC8(buf, 6u) != buf[6])
    {
        return 4u;              /* CRC 错误 */
    }
#endif

    /* 湿度：20 bit */
    raw_h = ((uint32_t)buf[1] << 12) |
            ((uint32_t)buf[2] << 4)  |
            ((uint32_t)buf[3] >> 4);

    /* 温度：20 bit */
    raw_t = (((uint32_t)buf[3] & 0x0Fu) << 16) |
            ((uint32_t)buf[4] << 8)            |
            ((uint32_t)buf[5]);

    data->humidity    = ((float)raw_h * 100.0f) / 1048576.0f;
    data->temperature = ((float)raw_t * 200.0f) / 1048576.0f - 50.0f;

    return 0u;
}
