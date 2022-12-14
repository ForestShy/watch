#include "delay.h"

uint32_t utim_delay=0;
uint32_t mtim_delay=0;

void SystickInit(void)//嘀嗒定时器配置
{
	SysTick_Config(100000);
}

void Tim3Init(void)
{
	/*
	定时器说明
	TIM3 -- APB1
	定时器TIM3：84MHZ
	16位定时器：值范围：0~65535
	*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;        //初始化定时器结构体
	NVIC_InitTypeDef  NVIC_InitStruct;			//配置NVIC结构体
	
	//1、使能定时器时钟。
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_Prescaler    = 10-1;    		//5分频 100MHZ/10 = 10MHZ
	TIM_TimeBaseInitStruct.TIM_Period		= 10-1;  		//计10个数 在10MHZ下，用时1us
	TIM_TimeBaseInitStruct.TIM_CounterMode  = TIM_CounterMode_Up; 	//向上计数
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1; 	//分频因子
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	//2、初始化定时器，配置ARR,PSC。
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
		//4、设置 TIM3_DIER  允许更新中断
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除中断标志位
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel= TIM3_IRQn;//NVIC通道，在stm32f4xx.h可查看通道 （可变）
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority= 0x01;	    //抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority= 0x01;	    //响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd= ENABLE;   //使能
	NVIC_Init(&NVIC_InitStruct);		

	//5、失能定时器。
	TIM_Cmd(TIM3, DISABLE);
}

void SysTick_Handler(void)
{
	if(mtim_delay!=0)
		mtim_delay--;

}

//每来一次中断就TimingDelay的值就减一
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除中断标志位
		if(utim_delay!=0)
			utim_delay--;
	}
	
}

//延迟函数
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
