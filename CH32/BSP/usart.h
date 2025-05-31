
#ifndef USART_H_
#define USART_H_


#include "bsp.h"

void USART2_Init(uint32_t baudrate);
void USART2_IRQHandler(void);
void uart2_printf(const uint8_t*fomat,...);
void USART2_DMA_SendBuff(uint8_t *buf, uint8_t len);

void USART3_Init(uint32_t baudrate);
void USART3_DMA_SendBuff(uint8_t *buf, uint8_t len);
void uart3_printf(const uint8_t *format, ...);
void USART3_IRQHandler(void);

void USART4_Init(uint32_t baudrate);
void USART4_DMA_SendBuff(uint8_t *buf, uint8_t len);
void uart4_printf(const uint8_t *format, ...);
void UART4_IRQHandler(void);


#define UART6_RX_BUFFER_SIZE 1024
#define UART6_TX_BUFFER_SIZE 64
void UART6_Init(uint32_t baudrate);
void UART6_DMA_SendBuff(uint8_t *buf, uint8_t len);
void uart6_printf(const char *format, ...);
void UART6_IRQHandler(void);


void uart_proc(void);



#endif /* USART_H_ */
