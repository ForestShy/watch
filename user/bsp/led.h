#ifndef __LED_H
#define __LED_H

#include "stm32f4xx.h"

void LedInit(void);

#define LEDON GPIO_ResetBits(GPIOA,GPIO_Pin_7)
#define LEDOFF GPIO_SetBits(GPIOA,GPIO_Pin_7)
#define LEDTOGGLE GPIO_ToggleBits(GPIOA,GPIO_Pin_7)

#endif
