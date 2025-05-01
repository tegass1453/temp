#ifndef _TIM_H_
#define _TIM_H_

#include "bsp.h"

uint32_t TIM2_GetTick(void);
void TIM2_init(void);
void TIM2_IRQHandler(void);

#endif
