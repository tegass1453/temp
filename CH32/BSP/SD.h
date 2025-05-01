#ifndef _SD_H
#define _SD_H

#include "bsp.h"

// Ӳ������
#define SPI3_NSS_PIN    GPIO_Pin_15
#define SPI3_NSS_PORT   GPIOA
#define SPI3_SCK_PIN    GPIO_Pin_3
#define SPI3_SCK_PORT   GPIOB
#define SPI3_MOSI_PIN   GPIO_Pin_4
#define SPI3_MOSI_PORT  GPIOB
#define SPI3_MISO_PIN   GPIO_Pin_5
#define SPI3_MISO_PORT  GPIOB

// ����ԭ��
void SPI3_SD_Init(void);          // ��ʼ��SPI3
void SPI3_NSS_High(void);         // ����Ƭѡ
void SPI3_NSS_Low(void);          // ����Ƭѡ
uint8_t SPI3_ReadWriteByte(uint8_t data); // SPI���ֽ��շ�
void SPI3_ReadWriteMulti(const uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);

#endif
