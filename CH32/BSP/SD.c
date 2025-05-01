#include "SD.h"

/**
  * @brief  SPI3初始化（主模式，用于SD卡）
  */
void SPI3_SD_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

    GPIO_InitStructure.GPIO_Pin = SPI3_NSS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI3_NSS_PORT, &GPIO_InitStructure);
    SPI3_NSS_High();  // 初始状态取消片选

    // 4. 配置SCK和MOSI为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = SPI3_SCK_PIN | SPI3_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    // 复用推挽
    GPIO_Init(SPI3_SCK_PORT, &GPIO_InitStructure);

    // 5. 配置MISO为浮空输入
    GPIO_InitStructure.GPIO_Pin = SPI3_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(SPI3_MISO_PORT, &GPIO_InitStructure);

    // 6. 配置SPI3参数
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;       // 主模式
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   // 8位数据
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;          // CPOL=0
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;        // CPHA=0（SPI模式0）
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;           // 软件控制NSS
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // 初始低速
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  // 高位先行
    SPI_InitStructure.SPI_CRCPolynomial = 7;            // CRC多项式（未使用）
    SPI_Init(SPI3, &SPI_InitStructure);

    // 7. 使能SPI3
    SPI_Cmd(SPI3, ENABLE);
}

void SPI3_NSS_High(void) {
    GPIO_SetBits(SPI3_NSS_PORT, SPI3_NSS_PIN);
}

void SPI3_NSS_Low(void) {
    GPIO_ResetBits(SPI3_NSS_PORT, SPI3_NSS_PIN);
}

uint8_t SPI3_ReadWriteByte(uint8_t data) {
    // 等待发送缓冲区空
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI3, data);  // 发送数据

    // 等待接收完成
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI3); // 返回接收的数据
}

void SPI3_ReadWriteMulti(const uint8_t *pTxData, uint8_t *pRxData, uint16_t Size) {
    for (uint16_t i = 0; i < Size; i++) {
        // 发送数据（若pTxData为NULL，发送0xFF）
        uint8_t tx_byte = (pTxData != NULL) ? pTxData[i] : 0xFF;
        SPI_I2S_SendData(SPI3, tx_byte);

        // 等待发送完成（TXE标志置位）
        while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);

        // 等待接收完成（RXNE标志置位）
        while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);

        // 读取接收数据（若pRxData非空则保存）
        uint8_t rx_byte = SPI_I2S_ReceiveData(SPI3);
        if (pRxData != NULL) {
            pRxData[i] = rx_byte;
        }
    }
}
