#ifndef __SD_H
#define __SD_H

#include "bsp.h"


// SPI3引脚定义（根据实际硬件修改）
#define SPI3_NSS_PORT       GPIOA
#define SPI3_NSS_PIN        GPIO_Pin_15
#define SPI3_SCK_PORT       GPIOB
#define SPI3_SCK_PIN        GPIO_Pin_3
#define SPI3_MISO_PORT      GPIOB
#define SPI3_MISO_PIN       GPIO_Pin_4
#define SPI3_MOSI_PORT      GPIOB
#define SPI3_MOSI_PIN       GPIO_Pin_5

// DMA通道定义(STM32F103)
#define SPI3_TX_DMA_CHANNEL     DMA1_Channel2
#define SPI3_RX_DMA_CHANNEL     DMA1_Channel1
#define SPI3_TX_DMA_FLAG_TC     DMA1_FLAG_TC2
#define SPI3_RX_DMA_FLAG_TC     DMA1_FLAG_TC1

// DMA状态管理结构体
typedef struct {
    volatile uint8_t tx_complete;
    volatile uint8_t rx_complete;
    uint8_t *tx_buf;
    uint8_t *rx_buf;
    uint16_t size;
    void (*callback)(void);
} SPI3_DMA_State;

extern SPI3_DMA_State spi3_dma;

// 函数声明
void SPI3_SD_Init(void);
void SPI3_DMA_Init(void);
int SPI3_StartDMA(const uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, void (*callback)(void));
int SPI3_WaitDMAComplete(uint32_t timeout_ms);
void SPI3_NSS_High(void);
void SPI3_NSS_Low(void);

void DMA1_Channel1_IRQHandler(void);
void DMA1_Channel2_IRQHandler(void);

#endif
