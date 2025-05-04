#include "Relay.h"





void Relay_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin=Relay_GPIO_Pin;
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(Relay_GPIO_Port, &GPIO_InitStruct);

    GPIO_ResetBits(Relay_GPIO_Port, Relay_GPIO_Pin);
}


