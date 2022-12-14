#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx.h"

uint8_t RtcInit(void);

void GetDate(void);
void GetTime(void);

#endif


