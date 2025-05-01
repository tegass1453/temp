#include "SD.h"

/**
  * @brief  SPI3��ʼ������ģʽ������SD����
  */
void SPI3_SD_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

    GPIO_InitStructure.GPIO_Pin = SPI3_NSS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI3_NSS_PORT, &GPIO_InitStructure);
    SPI3_NSS_High();  // ��ʼ״̬ȡ��Ƭѡ

    // 4. ����SCK��MOSIΪ�����������
    GPIO_InitStructure.GPIO_Pin = SPI3_SCK_PIN | SPI3_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    // ��������
    GPIO_Init(SPI3_SCK_PORT, &GPIO_InitStructure);

    // 5. ����MISOΪ��������
    GPIO_InitStructure.GPIO_Pin = SPI3_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
    GPIO_Init(SPI3_MISO_PORT, &GPIO_InitStructure);

    // 6. ����SPI3����
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;       // ��ģʽ
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   // 8λ����
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;          // CPOL=0
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;        // CPHA=0��SPIģʽ0��
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;           // �������NSS
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // ��ʼ����
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  // ��λ����
    SPI_InitStructure.SPI_CRCPolynomial = 7;            // CRC����ʽ��δʹ�ã�
    SPI_Init(SPI3, &SPI_InitStructure);

    // 7. ʹ��SPI3
    SPI_Cmd(SPI3, ENABLE);
}

void SPI3_NSS_High(void) {
    GPIO_SetBits(SPI3_NSS_PORT, SPI3_NSS_PIN);
}

void SPI3_NSS_Low(void) {
    GPIO_ResetBits(SPI3_NSS_PORT, SPI3_NSS_PIN);
}

uint8_t SPI3_ReadWriteByte(uint8_t data) {
    // �ȴ����ͻ�������
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI3, data);  // ��������

    // �ȴ��������
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI3); // ���ؽ��յ�����
}

void SPI3_ReadWriteMulti(const uint8_t *pTxData, uint8_t *pRxData, uint16_t Size) {
    for (uint16_t i = 0; i < Size; i++) {
        // �������ݣ���pTxDataΪNULL������0xFF��
        uint8_t tx_byte = (pTxData != NULL) ? pTxData[i] : 0xFF;
        SPI_I2S_SendData(SPI3, tx_byte);

        // �ȴ�������ɣ�TXE��־��λ��
        while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);

        // �ȴ�������ɣ�RXNE��־��λ��
        while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);

        // ��ȡ�������ݣ���pRxData�ǿ��򱣴棩
        uint8_t rx_byte = SPI_I2S_ReceiveData(SPI3);
        if (pRxData != NULL) {
            pRxData[i] = rx_byte;
        }
    }
}
