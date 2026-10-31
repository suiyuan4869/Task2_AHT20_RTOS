#ifndef I2C_SOFT_H
#define I2C_SOFT_H

#include "stm32f10x.h"

/*
 * 软件 I2C 引脚：
 *   SCL = PB6
 *   SDA = PB7
 *
 * 外部需要 4.7 kΩ 上拉电阻到 3.3 V。
 * AHT20 模块通常自带 10 kΩ 上拉；如果模块没有，必须外接。
 */
#define I2C_SOFT_SCL_PIN            GPIO_Pin_6
#define I2C_SOFT_SDA_PIN            GPIO_Pin_7
#define I2C_SOFT_PORT               GPIOB

/* 初始化 GPIO 为开漏输出，并做一次总线恢复 */
void I2C_Soft_Init(void);

/*
 * 写 N 个字节：
 *   dev_addr：7 位从机地址（AHT20 为 0x38）
 *   返回 0 成功；1 地址无应答；2 数据无应答
 */
uint8_t I2C_Soft_Write(uint8_t dev_addr, const uint8_t *buf, uint16_t len);

/*
 * 读 N 个字节：
 *   返回 0 成功；1 地址无应答
 *   最后一个字节回 NACK，其余字节回 ACK
 */
uint8_t I2C_Soft_Read(uint8_t dev_addr, uint8_t *buf, uint16_t len);

#endif
