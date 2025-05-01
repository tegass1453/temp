#include "tim.h"

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

uint32_t TIM2_Tick=0;
uint32_t TIM2_GetTick(void)
{
    return TIM2_Tick;
}
void TIM2_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_InternalClockConfig(TIM2);

    TIM_TimeBaseInitTypeDef tim_timebase_initstruct;
    tim_timebase_initstruct.TIM_ClockDivision=TIM_CKD_DIV1;
    tim_timebase_initstruct.TIM_CounterMode=TIM_CounterMode_Up;
    tim_timebase_initstruct.TIM_Period=1000-1;
    tim_timebase_initstruct.TIM_Prescaler=144-1;
    tim_timebase_initstruct.TIM_RepetitionCounter=0;
    TIM_TimeBaseInit(TIM2, &tim_timebase_initstruct);


    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef nvic_initstruct;
    nvic_initstruct.NVIC_IRQChannel=TIM2_IRQn;
    nvic_initstruct.NVIC_IRQChannelCmd=ENABLE;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority=0;
    nvic_initstruct.NVIC_IRQChannelSubPriority=0;
    NVIC_Init(&nvic_initstruct);

    TIM_Cmd(TIM2, ENABLE);
}
void TIM2_IRQHandler(void) {

    if (TIM_GetFlagStatus(TIM2,TIM_FLAG_Update) == SET) {

        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        TIM_ClearFlag(TIM2, TIM_FLAG_Update);
        TIM2_Tick++;

    }
}



