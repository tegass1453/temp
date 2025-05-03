#include "SD.h"
#include <string.h>

void DMA1_Channel1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


SPI3_DMA_State spi3_dma = {1, 1, NULL, NULL, 0, NULL};

/******************** 硬件控制函数 ********************/
void SPI3_NSS_High(void) {
    GPIO_SetBits(SPI3_NSS_PORT, SPI3_NSS_PIN);
}

void SPI3_NSS_Low(void) {
    GPIO_ResetBits(SPI3_NSS_PORT, SPI3_NSS_PIN);
}

/******************** SPI3初始化 ********************/
void SPI3_SD_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    // 启用时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

    // 配置NSS引脚
    GPIO_InitStructure.GPIO_Pin = SPI3_NSS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI3_NSS_PORT, &GPIO_InitStructure);
    SPI3_NSS_High();

    // 配置SCK/MOSI
    GPIO_InitStructure.GPIO_Pin = SPI3_SCK_PIN | SPI3_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPI3_SCK_PORT, &GPIO_InitStructure);

    // 配置MISO
    GPIO_InitStructure.GPIO_Pin = SPI3_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(SPI3_MISO_PORT, &GPIO_InitStructure);

    // SPI参数配置
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI3, &SPI_InitStructure);

    // 启用SPI DMA请求
    SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);
    SPI_Cmd(SPI3, ENABLE);

    // 初始化DMA
    SPI3_DMA_Init();
}

/******************** DMA初始化 ********************/
void SPI3_DMA_Init(void) {
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // TX DMA配置（内存->外设）
    DMA_DeInit(SPI3_TX_DMA_CHANNEL);
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&SPI3->DATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = 0;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(SPI3_TX_DMA_CHANNEL, &DMA_InitStructure);

    // RX DMA配置（外设->内存）
    DMA_DeInit(SPI3_RX_DMA_CHANNEL);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_Init(SPI3_RX_DMA_CHANNEL, &DMA_InitStructure);

    // 配置中断
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    // 使能DMA中断
    DMA_ITConfig(SPI3_TX_DMA_CHANNEL, DMA_IT_TC, ENABLE);
    DMA_ITConfig(SPI3_RX_DMA_CHANNEL, DMA_IT_TC, ENABLE);
}

/******************** DMA传输函数 ********************/
int SPI3_StartDMA(const uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, void (*callback)(void)) {
    if (!spi3_dma.tx_complete || !spi3_dma.rx_complete) return -1;

    static uint8_t dummyTx[256];
    if (!pTxData) {
        memset(dummyTx, 0xFF, Size > 256 ? 256 : Size);
        pTxData = dummyTx;
    }

    spi3_dma.tx_complete = 0;
    spi3_dma.rx_complete = pRxData ? 0 : 1;
    spi3_dma.tx_buf = (uint8_t*)pTxData;
    spi3_dma.rx_buf = pRxData;
    spi3_dma.size = Size;
    spi3_dma.callback = callback;

    // 配置TX DMA
    DMA_SetCurrDataCounter(SPI3_TX_DMA_CHANNEL, Size);
    SPI3_TX_DMA_CHANNEL->MADDR=(uint32_t)pTxData;

    // 配置RX DMA
    if(pRxData) {
        DMA_SetCurrDataCounter(SPI3_RX_DMA_CHANNEL, Size);
        SPI3_RX_DMA_CHANNEL->MADDR=(uint32_t)pRxData;
        DMA_Cmd(SPI3_RX_DMA_CHANNEL, ENABLE);

    }

    DMA_Cmd(SPI3_TX_DMA_CHANNEL, ENABLE);
    return 0;
}

/******************** 阻塞等待函数 ********************/
int SPI3_WaitDMAComplete(uint32_t timeout_ms) {
    uint32_t start = TIM2_GetTick();
    while(1) {
        if(spi3_dma.tx_complete && spi3_dma.rx_complete) return 0;
        if(TIM2_GetTick() - start > timeout_ms) return -1;
        __WFI();  // 进入低功耗等待
    }
}

/******************** 中断处理函数 ********************/


void DMA1_Channel2_IRQHandler(void) {
    if(DMA_GetITStatus(DMA1_IT_TC2)) {
        DMA_ClearITPendingBit(DMA1_IT_TC2);
        spi3_dma.tx_complete = 1;
        DMA_Cmd(SPI3_TX_DMA_CHANNEL, DISABLE);
        if(spi3_dma.callback) spi3_dma.callback();
    }
}



void DMA1_Channel1_IRQHandler(void) {
    if(DMA_GetITStatus(DMA1_IT_TC1)) {
        DMA_ClearITPendingBit(DMA1_IT_TC1);
        spi3_dma.rx_complete = 1;
        DMA_Cmd(SPI3_RX_DMA_CHANNEL, DISABLE);
    }
}
