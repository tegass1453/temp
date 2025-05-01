#ifndef _SD_H
#define _SD_H

#include "bsp.h"

// 硬件定义
#define SPI3_NSS_PIN    GPIO_Pin_15
#define SPI3_NSS_PORT   GPIOA
#define SPI3_SCK_PIN    GPIO_Pin_3
#define SPI3_SCK_PORT   GPIOB
#define SPI3_MOSI_PIN   GPIO_Pin_4
#define SPI3_MOSI_PORT  GPIOB
#define SPI3_MISO_PIN   GPIO_Pin_5
#define SPI3_MISO_PORT  GPIOB

// 函数原型
void SPI3_SD_Init(void);          // 初始化SPI3
void SPI3_NSS_High(void);         // 拉高片选
void SPI3_NSS_Low(void);          // 拉低片选
uint8_t SPI3_ReadWriteByte(uint8_t data); // SPI单字节收发
void SPI3_ReadWriteMulti(const uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);

#endif
