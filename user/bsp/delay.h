#ifndef __DELAY_H_
#define __DELAY_H_

#include "stm32f4xx.h"

void SystickInit(void);//àÖàª¶¨Ê±Æ÷ÅäÖÃ
void Tim3Init(void);

void Delayus(uint32_t time);
void Delayms(uint32_t time);

#endif
