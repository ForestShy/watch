#ifndef _KEY_H_
#define _KEY_H_

#include "stm32f4xx.h"
//#include "bitband.h"
#include "delay.h"
#include "usart.h"

typedef enum
{
    KEY_CHECK = 0,//���
    KEY_COMFIRM = 1,//ȷ��
    KEY_RELEASE = 2,//�ͷ�
}KEY_STATE;

#define KEY GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//���������Ǹߵ�ƽ
void Key_Init(void);
void KEY_ADC_Init(void);
u16 KEY_Get_ADC(void);
uint8_t Key_Scan(void);
uint8_t Key_AdcScan(void);

#endif
