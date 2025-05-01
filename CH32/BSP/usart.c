#include "usart.h"

/* ȫ�ֱ����ͺ궨�� */
// USART�����־λ
#define USER_UART_PROC 0X01    // �������ݴ����־
#define USER_UART_FULL 0X02    // ���ջ���������־(��ǰ����δʹ��)
#define UART2_RX_BUFFER_SIZE 256 // ���ջ�������С
#define UART2_TX_BUFFER_SIZE 256 // ���ͻ�������С

DMA_InitTypeDef UART2_DMA_InitStruct;        // DMA��ʼ���ṹ��
uint8_t uart2_rx_buffer[UART2_RX_BUFFER_SIZE]; // USART2 DMA���ջ�����
uint8_t uart2_tx_buffer[UART2_TX_BUFFER_SIZE]; // USART2 DMA���ͻ�����
uint8_t uart2_flag;                          // USART2״̬��־λ
ringbuffer_t uart2_ringbuffer;               // USART2�������ݻ��λ�����
uint8_t USART2_Revnum = 0;                   // ��ǰ���������ֽ���

/* DMAͨ����ʼ���ṹ�� */
DMA_InitTypeDef DMA_InitStructure_USART2_TX; // USART2����DMA����
DMA_InitTypeDef DMA_InitStructure_USART2_RX; // USART2����DMA����

/**
  * @brief  USART2 DMAͨ������
  * @param  baudtate: ����ͨ�Ų�����
  * @retval ��
  * @note   ����USART2��GPIO��DMAͨ����USART����������������ж�
  */
void USART2_DMAConfig(uint32_t baudtate)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    /* ʹ���������ʱ�� */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);     // DMA1ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // USART2ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // GPIOAʱ��

    /*--------------------- ����DMA���� ---------------------*/
    DMA_DeInit(DMA1_Channel7); // ��λDMA1ͨ��7(USART2_TX)
    DMA_InitStructure_USART2_TX.DMA_PeripheralBaseAddr = (u32)(&USART2->DATAR); // �����ַ(USART2���ݼĴ���)
    DMA_InitStructure_USART2_TX.DMA_MemoryBaseAddr = (u32)uart2_tx_buffer;      // �ڴ����ַ(���ͻ�����)
    DMA_InitStructure_USART2_TX.DMA_DIR = DMA_DIR_PeripheralDST;                // ���䷽��:�ڴ�->����
    DMA_InitStructure_USART2_TX.DMA_BufferSize = UART2_TX_BUFFER_SIZE;          // ���ݴ�����
    DMA_InitStructure_USART2_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // �����ַ������
    DMA_InitStructure_USART2_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;          // �ڴ��ַ����
    DMA_InitStructure_USART2_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݵ�λ:�ֽ�
    DMA_InitStructure_USART2_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;   // �ڴ����ݵ�λ:�ֽ�
    DMA_InitStructure_USART2_TX.DMA_Mode = DMA_Mode_Normal;                    // DMAģʽ:��ͨģʽ(���δ���)
    DMA_InitStructure_USART2_TX.DMA_Priority = DMA_Priority_VeryHigh;           // ͨ�����ȼ�:���
    DMA_InitStructure_USART2_TX.DMA_M2M = DMA_M2M_Disable;                      // �����ڴ浽�ڴ�ģʽ
    DMA_Init(DMA1_Channel7, &DMA_InitStructure_USART2_TX);                      // ��ʼ��DMAͨ��7

    /*--------------------- ����DMA���� ---------------------*/
    DMA_DeInit(DMA1_Channel6); // ��λDMA1ͨ��6(USART2_RX)
    DMA_InitStructure_USART2_RX.DMA_PeripheralBaseAddr = (u32)(&USART2->DATAR); // �����ַ
    DMA_InitStructure_USART2_RX.DMA_MemoryBaseAddr = (u32)uart2_rx_buffer;      // �ڴ����ַ(���ջ�����)
    DMA_InitStructure_USART2_RX.DMA_DIR = DMA_DIR_PeripheralSRC;                // ���䷽��:����->�ڴ�
    DMA_InitStructure_USART2_RX.DMA_BufferSize = UART2_RX_BUFFER_SIZE;          // ���ݴ�����
    DMA_InitStructure_USART2_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // �����ַ������
    DMA_InitStructure_USART2_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;           // �ڴ��ַ����
    DMA_InitStructure_USART2_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure_USART2_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure_USART2_RX.DMA_Mode = DMA_Mode_Normal;                     // ��ͨģʽ
    DMA_InitStructure_USART2_RX.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure_USART2_RX.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel6, &DMA_InitStructure_USART2_RX);                      // ��ʼ��DMAͨ��6

    /*--------------------- USART2 GPIO���� ---------------------*/
    // PA2(TX):�����������, PA3(RX):��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // �����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);            // ��ʼ��PA2

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);            // ��ʼ��PA3

    /*--------------------- USART2�������� ---------------------*/
    USART_InitStructure.USART_BaudRate = baudtate;    // ������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8λ����λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;      // 1λֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;         // ��У��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // ʹ�ܷ��ͺͽ���
    USART_Init(USART2, &USART_InitStructure);         // ��ʼ��USART2

    /* ʹ��USART2�����ж� */
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);    // ���ÿ�����·�ж�
    USART_Cmd(USART2, ENABLE);                        // ʹ��USART2

    /* ����USART2�ж����ȼ� */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // �����ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);                // ��ʼ��NVIC

    /* ʹ��DMAͨ�� */
    DMA_Cmd(DMA1_Channel7, ENABLE);  // ʹ��USART2����DMA
    DMA_Cmd(DMA1_Channel6, ENABLE);  // ʹ��USART2����DMA

    /* ʹ��USART��DMA���� */
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); // ʹ�ܽ���DMA
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); // ʹ�ܷ���DMA
}

/**
  * @brief  USART2��ʼ������
  * @param  baudrate: ����ͨ�Ų�����
  * @retval ��
  * @note   ��ʼ��USART2���裬�����û��λ�����
  */
void USART2_Init(uint32_t baudrate)
{
    USART2_DMAConfig(baudrate);               // ����DMA���ú���
    ringbuffer_init(&uart2_ringbuffer, UART2_RX_BUFFER_SIZE); // ��ʼ�����λ�����
}

/**
  * @brief  ͨ��DMA��������
  * @param  buf: ���������ݻ�����ָ��
  * @param  len: �������ݳ���
  * @retval ��
  * @note   �ȴ���һ��DMA������ɣ��������ݵ����ͻ���������������DMA
  */
void USART2_DMA_SendBuff(uint8_t *buf, uint8_t len)
{
    /* �ȴ���һ��DMA������� */
    while(DMA_GetFlagStatus(DMA1_FLAG_TC7) == RESET) {;}

    memcpy(uart2_tx_buffer, buf, len * sizeof(unsigned char)); // �������ݵ����ͻ�����

    /* ��������DMA����ͨ�� */
    DMA_DeInit(DMA1_Channel7);  // ��λDMAͨ��
    DMA_InitStructure_USART2_TX.DMA_MemoryBaseAddr = (u32)uart2_tx_buffer; // �����ڴ��ַ
    DMA_InitStructure_USART2_TX.DMA_BufferSize = len;           // ���ô������ݳ���
    DMA_Init(DMA1_Channel7, &DMA_InitStructure_USART2_TX);      // ���³�ʼ��DMA
    DMA_Cmd(DMA1_Channel7, ENABLE);  // ʹ��DMAͨ����ʼ����
}

/**
  * @brief  USART2��ʽ���������
  * @param  format: ��ʽ���ַ���
  * @retval ��
  * @note   ʹ�ÿɱ����ʵ�ָ�ʽ�������ͨ��DMA��������
  */
void uart2_printf(const uint8_t *format, ...)
{
    /* �ȴ����һ�η������ */
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);

    char buffer[128];
    va_list arg;
    va_start(arg, format);
    vsnprintf(buffer, sizeof(buffer), format, arg); // ��ʽ���ַ�����������
    va_end(arg);

    int len = strlen(buffer);
    USART2_DMA_SendBuff((uint8_t*)buffer, len);     // ����DMA���ͺ���
}

/**
  * @brief  USART2�жϷ�����
  * @retval ��
  * @note   ����USART2�����жϣ���DMA�������ݴ��뻷�λ�����
  */
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_IDLE) == SET) // ��⵽�����ж�
    {
        USART_ClearITPendingBit(USART2, USART_IT_IDLE);  // ��������жϱ�־

        /* ͨ����USART״̬�Ĵ��������ݼĴ�����������жϱ�־ */
        USART2_Revnum = USART2->STATR;  // ��״̬�Ĵ���
        USART2_Revnum = USART2->DATAR;  // �����ݼĴ���

        /* ���㱾�ν������ݳ��� */
        DMA_Cmd(DMA1_Channel6, DISABLE); // ����DMAͨ��
        USART2_Revnum = UART2_RX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6); // ��������ֽ���
        DMA1_Channel6->CNTR = UART2_RX_BUFFER_SIZE; // ����DMA������

        /* ����������д�뻷�λ����� */
        ringbuffer_write(&uart2_ringbuffer, uart2_rx_buffer, USART2_Revnum);

        DMA_Cmd(DMA1_Channel6, ENABLE); // ����ʹ��DMA����
        uart2_flag |= USER_UART_PROC;    // �������ݴ����־
    }
}

/**
  * @brief  USART�������ݴ�����
  * @retval ��
  * @note   �ӻ��λ�������ȡ���ݲ����д������û��������ݽ�������
  */
void uart_proc(void)
{
    if (uart2_flag & USER_UART_PROC) // ������ݴ����־
    {
        uart2_flag &= (~USER_UART_PROC); // �����־λ

        uint8_t uart2_rb[UART2_RX_BUFFER_SIZE] = {0};
        ringbuffer_read(&uart2_ringbuffer, uart2_rb, uart2_ringbuffer.itemCount); // ��ȡ���λ���������

        /* ���ݽ�������(���û�����) */
        // ʾ��: ����uart2_rb�е�����...
        uart2_printf("%s",uart2_rb);
    }
}
