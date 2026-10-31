#ifndef AHT20_H
#define AHT20_H

#include "stm32f10x.h"

/* AHT20 的 7 位 I2C 地址 */
#define AHT20_I2C_ADDR              0x38u

/* 采集结果：温度 ℃，相对湿度 %RH */
typedef struct
{
    float temperature;
    float humidity;
} AHT20_Data_t;

/*
 * 初始化 AHT20：
 *   上电延时 -> 读状态 -> 若未校准则发送 0xBE 校准命令
 * 返回 0 成功；非 0 为错误码。
 */
uint8_t AHT20_Init(void);

/*
 * 触发一次测量并读取结果：
 *   0xAC 0x33 0x00 -> 等待 80 ms -> 读 7 字节
 * 返回 0 成功；非 0 为错误码。
 */
uint8_t AHT20_Read(AHT20_Data_t *data);

/* 读状态字节；bit3=1 表示已校准，bit7=1 表示忙 */
uint8_t AHT20_ReadStatus(uint8_t *status);

#endif
