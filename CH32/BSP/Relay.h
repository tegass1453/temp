#ifndef __RELAY_H_
#define __RELAY_H_

#include "bsp.h"


#define Relay_GPIO_Port GPIOB
#define Relay_GPIO_Pin GPIO_Pin_1
#define Relay_On()  GPIO_SetBits(Relay_GPIO_Port, Relay_GPIO_Pin)
#define Relay_Off() GPIO_ResetBits(Relay_GPIO_Port, Relay_GPIO_Pin)


void Relay_init(void);




#endif
