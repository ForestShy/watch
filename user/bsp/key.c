#include "key.h"

void Key_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_KEY_Init;
	GPIO_KEY_Init.GPIO_Pin=GPIO_Pin_0;
	GPIO_KEY_Init.GPIO_Mode=GPIO_Mode_IN;//设置对应IO口为输入模式
	GPIO_KEY_Init.GPIO_PuPd=GPIO_PuPd_NOPULL;//设置对应IO口为浮空模式
	GPIO_Init(GPIOA,&GPIO_KEY_Init);//初始化GPIOA口
}
/*
函数名： Key_Scan 
函数功能：按键扫描函数
*/
uint8_t Key_Scan(void)
{
	static int count=0;
	if(KEY==1) count++;
	else if(count>=12)//长按
	{
		count=0;
		return 2;
	}
	else if(count>1)//短按
	{
		count=0;
		return 1;
	}
	else count=0;
	return 0;
}
void KEY_ADC_Init(void)
{
	RCC->AHB1ENR |= 1<<0;
	RCC->APB2ENR|=1<<8;//ADC1时钟使能
	GPIOA->MODER &=~(3<<6);//清零
	GPIOA->MODER |= 0X3<<6;//PA3配置模拟输入模式
	RCC->APB2RSTR|=1<<8;//ADCs复位
	RCC->APB2RSTR &=~(1<<8);//复位结束
	ADC->CCR=3<<16;//ADCCLK=PCLK/8=100/8=12MHZ,最好不要超过36MHZ
	ADC1->CR1=0;//CR1设置清零
	ADC1->CR2=0;//CR2设置清零
	ADC1->CR1|=0<<24;//12位模式
	ADC1->CR1|=0<<8;//非扫描模式
	
	ADC1->CR2 &=~(1<<1);//单次转换模式
	ADC1->CR2 &=~(1<<11);//右对齐
	ADC1->CR2 |=0<<28;//软件触发
	ADC1->SQR1 &=~(0XF<<20);
	ADC1->SQR1 |=0<<20;//1个转换在规则通道中，也就是只转换规则序列1
	//设置通道3的采样时间
	ADC1->SMPR2 &=~(7<<9);//通道3采样时间清空
	ADC1->SMPR2 |=(7<<9);//通道3，480个周期，提高转换精度
	ADC1->CR2 |=1<<0;//开启AD转换器
}
u16 KEY_Get_ADC(void)
{
	//设置转换序列
	ADC1->SQR3 &=~(0X1F<<0);//设置ADC1通道1，将ADCIN3放到第一个转换序列中
	ADC1->SQR3 |=(3<<0);//必须是第一个通道
	ADC1->CR2 |=1<<30;//启动转换通道
	while(!(ADC1->SR&(1<<1)));//等待转换结束
	return ADC1->DR;//返回ADC值
}

//五向按键判断
//返回值：
//上：5   下：6  左：4  右：3
uint8_t Key_AdcScan(void)
{
	u16 key_adc=0;
	u8 Key=0;
	key_adc=KEY_Get_ADC();
	if(key_adc>2030 && key_adc<2070)//右  1.65v
	{
		Delayus(50000);
		if(key_adc>2030 && key_adc<2070)
		Key=3;
	}
	if(key_adc>4080 && key_adc<4100)//左  3.3v
	{
		Delayus(50000);
		if(key_adc>4080 && key_adc<4100) 
		Key=4;
	}
	if(key_adc>1010 && key_adc<1040)//上 0.825v
	{
		Delayus(50000);
		if(key_adc>1010 && key_adc<1040)
		Key=5;
	}
	if(key_adc>1350 && key_adc<1380)//下  1.1v
	{
		Delayus(50000);
		if(key_adc>1350 && key_adc<1380)
		Key=6;
	}
	return  Key;
}
