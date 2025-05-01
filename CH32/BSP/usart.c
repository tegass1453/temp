#include "usart.h"

/* 全局变量和宏定义 */
// USART处理标志位
#define USER_UART_PROC 0X01    // 接收数据处理标志
#define USER_UART_FULL 0X02    // 接收缓冲区满标志(当前代码未使用)
#define UART2_RX_BUFFER_SIZE 256 // 接收缓冲区大小
#define UART2_TX_BUFFER_SIZE 256 // 发送缓冲区大小

DMA_InitTypeDef UART2_DMA_InitStruct;        // DMA初始化结构体
uint8_t uart2_rx_buffer[UART2_RX_BUFFER_SIZE]; // USART2 DMA接收缓冲区
uint8_t uart2_tx_buffer[UART2_TX_BUFFER_SIZE]; // USART2 DMA发送缓冲区
uint8_t uart2_flag;                          // USART2状态标志位
ringbuffer_t uart2_ringbuffer;               // USART2接收数据环形缓冲区
uint8_t USART2_Revnum = 0;                   // 当前接收数据字节数

/* DMA通道初始化结构体 */
DMA_InitTypeDef DMA_InitStructure_USART2_TX; // USART2发送DMA配置
DMA_InitTypeDef DMA_InitStructure_USART2_RX; // USART2接收DMA配置

/**
  * @brief  USART2 DMA通道配置
  * @param  baudtate: 串口通信波特率
  * @retval 无
  * @note   配置USART2的GPIO、DMA通道、USART参数，并启用相关中断
  */
void USART2_DMAConfig(uint32_t baudtate)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    /* 使能相关外设时钟 */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);     // DMA1时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // USART2时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // GPIOA时钟

    /*--------------------- 发送DMA配置 ---------------------*/
    DMA_DeInit(DMA1_Channel7); // 复位DMA1通道7(USART2_TX)
    DMA_InitStructure_USART2_TX.DMA_PeripheralBaseAddr = (u32)(&USART2->DATAR); // 外设地址(USART2数据寄存器)
    DMA_InitStructure_USART2_TX.DMA_MemoryBaseAddr = (u32)uart2_tx_buffer;      // 内存基地址(发送缓冲区)
    DMA_InitStructure_USART2_TX.DMA_DIR = DMA_DIR_PeripheralDST;                // 传输方向:内存->外设
    DMA_InitStructure_USART2_TX.DMA_BufferSize = UART2_TX_BUFFER_SIZE;          // 数据传输量
    DMA_InitStructure_USART2_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // 外设地址不递增
    DMA_InitStructure_USART2_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;          // 内存地址递增
    DMA_InitStructure_USART2_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据单位:字节
    DMA_InitStructure_USART2_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;   // 内存数据单位:字节
    DMA_InitStructure_USART2_TX.DMA_Mode = DMA_Mode_Normal;                    // DMA模式:普通模式(单次传输)
    DMA_InitStructure_USART2_TX.DMA_Priority = DMA_Priority_VeryHigh;           // 通道优先级:最高
    DMA_InitStructure_USART2_TX.DMA_M2M = DMA_M2M_Disable;                      // 禁用内存到内存模式
    DMA_Init(DMA1_Channel7, &DMA_InitStructure_USART2_TX);                      // 初始化DMA通道7

    /*--------------------- 接收DMA配置 ---------------------*/
    DMA_DeInit(DMA1_Channel6); // 复位DMA1通道6(USART2_RX)
    DMA_InitStructure_USART2_RX.DMA_PeripheralBaseAddr = (u32)(&USART2->DATAR); // 外设地址
    DMA_InitStructure_USART2_RX.DMA_MemoryBaseAddr = (u32)uart2_rx_buffer;      // 内存基地址(接收缓冲区)
    DMA_InitStructure_USART2_RX.DMA_DIR = DMA_DIR_PeripheralSRC;                // 传输方向:外设->内存
    DMA_InitStructure_USART2_RX.DMA_BufferSize = UART2_RX_BUFFER_SIZE;          // 数据传输量
    DMA_InitStructure_USART2_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // 外设地址不递增
    DMA_InitStructure_USART2_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;           // 内存地址递增
    DMA_InitStructure_USART2_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure_USART2_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure_USART2_RX.DMA_Mode = DMA_Mode_Normal;                     // 普通模式
    DMA_InitStructure_USART2_RX.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure_USART2_RX.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel6, &DMA_InitStructure_USART2_RX);                      // 初始化DMA通道6

    /*--------------------- USART2 GPIO配置 ---------------------*/
    // PA2(TX):复用推挽输出, PA3(RX):浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);            // 初始化PA2

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);            // 初始化PA3

    /*--------------------- USART2参数配置 ---------------------*/
    USART_InitStructure.USART_BaudRate = baudtate;    // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;      // 1位停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;         // 无校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // 使能发送和接收
    USART_Init(USART2, &USART_InitStructure);         // 初始化USART2

    /* 使能USART2空闲中断 */
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);    // 启用空闲线路中断
    USART_Cmd(USART2, ENABLE);                        // 使能USART2

    /* 配置USART2中断优先级 */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);                // 初始化NVIC

    /* 使能DMA通道 */
    DMA_Cmd(DMA1_Channel7, ENABLE);  // 使能USART2发送DMA
    DMA_Cmd(DMA1_Channel6, ENABLE);  // 使能USART2接收DMA

    /* 使能USART的DMA请求 */
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); // 使能接收DMA
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); // 使能发送DMA
}

/**
  * @brief  USART2初始化函数
  * @param  baudrate: 串口通信波特率
  * @retval 无
  * @note   初始化USART2外设，并配置环形缓冲区
  */
void USART2_Init(uint32_t baudrate)
{
    USART2_DMAConfig(baudrate);               // 调用DMA配置函数
    ringbuffer_init(&uart2_ringbuffer, UART2_RX_BUFFER_SIZE); // 初始化环形缓冲区
}

/**
  * @brief  通过DMA发送数据
  * @param  buf: 待发送数据缓冲区指针
  * @param  len: 发送数据长度
  * @retval 无
  * @note   等待上一次DMA传输完成，拷贝数据到发送缓冲区后重新配置DMA
  */
void USART2_DMA_SendBuff(uint8_t *buf, uint8_t len)
{
    /* 等待上一次DMA传输完成 */
    while(DMA_GetFlagStatus(DMA1_FLAG_TC7) == RESET) {;}

    memcpy(uart2_tx_buffer, buf, len * sizeof(unsigned char)); // 拷贝数据到发送缓冲区

    /* 重新配置DMA发送通道 */
    DMA_DeInit(DMA1_Channel7);  // 复位DMA通道
    DMA_InitStructure_USART2_TX.DMA_MemoryBaseAddr = (u32)uart2_tx_buffer; // 更新内存地址
    DMA_InitStructure_USART2_TX.DMA_BufferSize = len;           // 设置传输数据长度
    DMA_Init(DMA1_Channel7, &DMA_InitStructure_USART2_TX);      // 重新初始化DMA
    DMA_Cmd(DMA1_Channel7, ENABLE);  // 使能DMA通道开始传输
}

/**
  * @brief  USART2格式化输出函数
  * @param  format: 格式化字符串
  * @retval 无
  * @note   使用可变参数实现格式化输出，通过DMA发送数据
  */
void uart2_printf(const uint8_t *format, ...)
{
    /* 等待最后一次发送完成 */
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);

    char buffer[128];
    va_list arg;
    va_start(arg, format);
    vsnprintf(buffer, sizeof(buffer), format, arg); // 格式化字符串到缓冲区
    va_end(arg);

    int len = strlen(buffer);
    USART2_DMA_SendBuff((uint8_t*)buffer, len);     // 调用DMA发送函数
}

/**
  * @brief  USART2中断服务函数
  * @retval 无
  * @note   处理USART2空闲中断，将DMA接收数据存入环形缓冲区
  */
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_IDLE) == SET) // 检测到空闲中断
    {
        USART_ClearITPendingBit(USART2, USART_IT_IDLE);  // 清除空闲中断标志

        /* 通过读USART状态寄存器和数据寄存器清除空闲中断标志 */
        USART2_Revnum = USART2->STATR;  // 读状态寄存器
        USART2_Revnum = USART2->DATAR;  // 读数据寄存器

        /* 计算本次接收数据长度 */
        DMA_Cmd(DMA1_Channel6, DISABLE); // 禁用DMA通道
        USART2_Revnum = UART2_RX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6); // 计算接收字节数
        DMA1_Channel6->CNTR = UART2_RX_BUFFER_SIZE; // 重置DMA计数器

        /* 将接收数据写入环形缓冲区 */
        ringbuffer_write(&uart2_ringbuffer, uart2_rx_buffer, USART2_Revnum);

        DMA_Cmd(DMA1_Channel6, ENABLE); // 重新使能DMA接收
        uart2_flag |= USER_UART_PROC;    // 设置数据处理标志
    }
}

/**
  * @brief  USART接收数据处理函数
  * @retval 无
  * @note   从环形缓冲区读取数据并进行处理，需用户完善数据解析部分
  */
void uart_proc(void)
{
    if (uart2_flag & USER_UART_PROC) // 检查数据处理标志
    {
        uart2_flag &= (~USER_UART_PROC); // 清除标志位

        uint8_t uart2_rb[UART2_RX_BUFFER_SIZE] = {0};
        ringbuffer_read(&uart2_ringbuffer, uart2_rb, uart2_ringbuffer.itemCount); // 读取环形缓冲区数据

        /* 数据解析区域(需用户完善) */
        // 示例: 处理uart2_rb中的数据...
        uart2_printf("%s",uart2_rb);
    }
}
