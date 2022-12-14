#include "key.h"

void Key_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_KEY_Init;
	GPIO_KEY_Init.GPIO_Pin=GPIO_Pin_0;
	GPIO_KEY_Init.GPIO_Mode=GPIO_Mode_IN;//���ö�ӦIO��Ϊ����ģʽ
	GPIO_KEY_Init.GPIO_PuPd=GPIO_PuPd_NOPULL;//���ö�ӦIO��Ϊ����ģʽ
	GPIO_Init(GPIOA,&GPIO_KEY_Init);//��ʼ��GPIOA��
}
/*
�������� Key_Scan 
�������ܣ�����ɨ�躯��
*/
uint8_t Key_Scan(void)
{
	static int count=0;
	if(KEY==1) count++;
	else if(count>=12)//����
	{
		count=0;
		return 2;
	}
	else if(count>1)//�̰�
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
	RCC->APB2ENR|=1<<8;//ADC1ʱ��ʹ��
	GPIOA->MODER &=~(3<<6);//����
	GPIOA->MODER |= 0X3<<6;//PA3����ģ������ģʽ
	RCC->APB2RSTR|=1<<8;//ADCs��λ
	RCC->APB2RSTR &=~(1<<8);//��λ����
	ADC->CCR=3<<16;//ADCCLK=PCLK/8=100/8=12MHZ,��ò�Ҫ����36MHZ
	ADC1->CR1=0;//CR1��������
	ADC1->CR2=0;//CR2��������
	ADC1->CR1|=0<<24;//12λģʽ
	ADC1->CR1|=0<<8;//��ɨ��ģʽ
	
	ADC1->CR2 &=~(1<<1);//����ת��ģʽ
	ADC1->CR2 &=~(1<<11);//�Ҷ���
	ADC1->CR2 |=0<<28;//�������
	ADC1->SQR1 &=~(0XF<<20);
	ADC1->SQR1 |=0<<20;//1��ת���ڹ���ͨ���У�Ҳ����ֻת����������1
	//����ͨ��3�Ĳ���ʱ��
	ADC1->SMPR2 &=~(7<<9);//ͨ��3����ʱ�����
	ADC1->SMPR2 |=(7<<9);//ͨ��3��480�����ڣ����ת������
	ADC1->CR2 |=1<<0;//����ADת����
}
u16 KEY_Get_ADC(void)
{
	//����ת������
	ADC1->SQR3 &=~(0X1F<<0);//����ADC1ͨ��1����ADCIN3�ŵ���һ��ת��������
	ADC1->SQR3 |=(3<<0);//�����ǵ�һ��ͨ��
	ADC1->CR2 |=1<<30;//����ת��ͨ��
	while(!(ADC1->SR&(1<<1)));//�ȴ�ת������
	return ADC1->DR;//����ADCֵ
}

//���򰴼��ж�
//����ֵ��
//�ϣ�5   �£�6  ��4  �ң�3
uint8_t Key_AdcScan(void)
{
	u16 key_adc=0;
	u8 Key=0;
	key_adc=KEY_Get_ADC();
	if(key_adc>2030 && key_adc<2070)//��  1.65v
	{
		Delayus(50000);
		if(key_adc>2030 && key_adc<2070)
		Key=3;
	}
	if(key_adc>4080 && key_adc<4100)//��  3.3v
	{
		Delayus(50000);
		if(key_adc>4080 && key_adc<4100) 
		Key=4;
	}
	if(key_adc>1010 && key_adc<1040)//�� 0.825v
	{
		Delayus(50000);
		if(key_adc>1010 && key_adc<1040)
		Key=5;
	}
	if(key_adc>1350 && key_adc<1380)//��  1.1v
	{
		Delayus(50000);
		if(key_adc>1350 && key_adc<1380)
		Key=6;
	}
	return  Key;
}
