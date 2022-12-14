#include "delay.h"

uint32_t utim_delay=0;
uint32_t mtim_delay=0;

void SystickInit(void)//��શ�ʱ������
{
	SysTick_Config(100000);
}

void Tim3Init(void)
{
	/*
	��ʱ��˵��
	TIM3 -- APB1
	��ʱ��TIM3��84MHZ
	16λ��ʱ����ֵ��Χ��0~65535
	*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;        //��ʼ����ʱ���ṹ��
	NVIC_InitTypeDef  NVIC_InitStruct;			//����NVIC�ṹ��
	
	//1��ʹ�ܶ�ʱ��ʱ�ӡ�
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_Prescaler    = 10-1;    		//5��Ƶ 100MHZ/10 = 10MHZ
	TIM_TimeBaseInitStruct.TIM_Period		= 10-1;  		//��10���� ��10MHZ�£���ʱ1us
	TIM_TimeBaseInitStruct.TIM_CounterMode  = TIM_CounterMode_Up; 	//���ϼ���
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1; 	//��Ƶ����
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	//2����ʼ����ʱ��������ARR,PSC��
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
		//4������ TIM3_DIER  ��������ж�
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //����жϱ�־λ
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel= TIM3_IRQn;//NVICͨ������stm32f4xx.h�ɲ鿴ͨ�� ���ɱ䣩
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority= 0x01;	    //��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority= 0x01;	    //��Ӧ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd= ENABLE;   //ʹ��
	NVIC_Init(&NVIC_InitStruct);		

	//5��ʧ�ܶ�ʱ����
	TIM_Cmd(TIM3, DISABLE);
}

void SysTick_Handler(void)
{
	if(mtim_delay!=0)
		mtim_delay--;

}

//ÿ��һ���жϾ�TimingDelay��ֵ�ͼ�һ
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //����жϱ�־λ
		if(utim_delay!=0)
			utim_delay--;
	}
	
}

//�ӳٺ���
void Delayus(uint32_t time)
{ 
	TIM_Cmd(TIM3, ENABLE);
	utim_delay = time;
	while(utim_delay != 0);
	TIM_Cmd(TIM3, DISABLE);
}

void Delayms(uint32_t time)
{
	mtim_delay = time+10;
	while(mtim_delay >=10);
}
