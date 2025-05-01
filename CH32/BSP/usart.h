
#ifndef USART_H_
#define USART_H_


#include "bsp.h"

void USART2_Init(uint32_t baudrate);
void USART2_IRQHandler(void);
void DMA1_Channel7_IRQHandler(void);
void uart_proc(void);
void uart2_printf(const uint8_t*fomat,...);
void USART2_DMA_SendBuff(uint8_t *buf, uint8_t len);
#endif /* USART_H_ */
