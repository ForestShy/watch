#include "stm32f4xx.h"
#include "rtc.h"
#include "delay.h"
#include "stdio.h"
#include "oled.h"

#define LSE_ON
//RTC�����ʼ��ģʽ
//����ֵ:0,�ɹ�;1,ʧ��;

//RTC��ʼ��
//����ֵ:0,��ʼ���ɹ�;
//       1,LSE����ʧ��;
//       2,�����ʼ��ģʽʧ��;
uint8_t RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);
uint8_t RTC_Set_Date(u8 year,u8 month,u8 date,u8 week);

RTC_TimeTypeDef RTC_TimeStruct={0};
RTC_DateTypeDef RTC_DateStruct={0};

u8 RTC_Init_Mode(void)
{ 
	u32 retry=0X10000; 
	if(RTC->ISR&(1<<6))return 0; 
	RTC->ISR|=1<<7;	//����RTC��ʼ��ģʽ
	while(retry&&((RTC->ISR&(1<<6))==0x00))//�ȴ�����RTC��ʼ��ģʽ�ɹ�
	{
		retry--;
	}
    if(retry==0)return 1;	//ͬ��ʧ��
	else return 0; 			//ͬ���ɹ� 
}


u8 RtcInit(void)
{
	#ifdef LSE_ON 
	u16 retry=0xFF; 
	RCC->APB1ENR|=1<<28;			//ʹ�ܵ�Դ�ӿ�ʱ��
	PWR->CR|=1<<8;					//���������ʹ��(RTC+SRAM)
	RCC->BDCR|=1<<0;				//LSE ����  
//	if(RTC->BKP0R != 0x1234)		//�Ƿ��һ������?
	{
		while(retry&&((RCC->BDCR&0X02)==0))//�ȴ�LSE׼����
		{
			retry--;
			Delayms(5);
		}
		if(retry==0)return 1;		//LSE ����ʧ��. 
		RCC->BDCR&=~(1<<9);			//ѡ��LSE,��ΪRTC��ʱ��
		RCC->BDCR|=1<<8;			//ѡ��LSE,��ΪRTC��ʱ��
		RCC->BDCR|=1<<15;			//ʹ��RTCʱ��
	#else
		u16 retry=0xFF; 
		RCC->APB1ENR|=1<<28;			//ʹ�ܵ�Դ�ӿ�ʱ��
		PWR->CR|=1<<8;					//���������ʹ��(RTC+SRAM)
		RCC->CSR |= 1<<0;			//LSI ����  
	if(RTC->BKP0R != 0x1234)		//�Ƿ��һ������?
	{
		while(retry&&((RCC->CSR&0X02)==0))//�ȴ�LSI׼����
		{
			retry--;
			Delayms(5);
		}
		if(retry==0)return 1;		//LSE ����ʧ��. 
		RCC->BDCR|=2<<8;			//ѡ��LSI,��ΪRTC��ʱ��
		RCC->BDCR|=1<<15;			//ʹ��RTCʱ��
	#endif
 		//�ر�RTC�Ĵ���д����
		RTC->WPR=0xCA;
		RTC->WPR=0x53; 
		if(RTC_Init_Mode())return 2;//����RTC��ʼ��ģʽ
		RTC->PRER=0XFF;				//RTCͬ����Ƶϵ��(0~7FFF),����������ͬ����Ƶ,�������첽��Ƶ,Frtc=Fclks/((Sprec+1)*(Asprec+1))
		RTC->PRER|=0X75<<16;		//RTC�첽��Ƶϵ��(1~0X7F)
		RTC->CR&=~(1<<6);			//RTC����Ϊ,24Сʱ��ʽ
		RTC->ISR&=~(1<<7);			//�˳�RTC��ʼ��ģʽ
		RTC->WPR=0xFF;				//ʹ��RTC�Ĵ���д����  
		RTC_Set_Time(12,00,00,RTC_H12_AM);	//����ʱ��
		RTC_Set_Date(22,01,01,6);		//��������
		//RTC_Set_AlarmA(7,0,0,10);	//��������ʱ��
//		RTC->BKP0R = 0x1234;	//����Ѿ���ʼ������
	} 
	//RTC_Set_WakeUp(4,0);			//����WAKE UP�ж�,1�����ж�һ�� 
	return 0;
}

//RTCʱ������
//hour,min,sec:Сʱ,����,����
//����ֵ:0,�ɹ�
//       1,�����ʼ��ģʽʧ�� 

uint8_t RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;    //Сʱ
	RTC_TimeTypeInitStructure.RTC_Minutes=min;   //����
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;   //��
	RTC_TimeTypeInitStructure.RTC_H12=ampm;      //ʱ���ʽ
	
	RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
	return  0;
	
}

//RTC��������
//year,month,date:��(0~99),��(1~12),��(0~31)
//����ֵ:0,�ɹ�
//       1,�����ʼ��ģʽʧ�� 

uint8_t RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	
	RTC_DateTypeInitStructure.RTC_Date=date;         //��
	RTC_DateTypeInitStructure.RTC_Month=month;       //��
	RTC_DateTypeInitStructure.RTC_WeekDay=week;      //����
	RTC_DateTypeInitStructure.RTC_Year=year;         //��
	RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
	return 0;
}




char current_time[20]={0};
void GetTime(void)
{
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	sprintf(current_time,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
	printf("%s\r\n",current_time);
	OLED_Chin_Eng(3,32,16,16,current_time);
}

char current_date[20]={0};
void GetDate(void)
{

	RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);
	sprintf(current_date,"20%02d��%02d��%02d��",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
	OLED_Chin_Eng(1,10,16,16,current_date);
	switch(RTC_DateStruct.RTC_WeekDay)
	{
		case 0:OLED_Chin_Eng(5,40,16,16,"������");break;
		case 1:OLED_Chin_Eng(5,40,16,16,"����һ");break;
		case 2:OLED_Chin_Eng(5,40,16,16,"���ڶ�");break;
		case 3:OLED_Chin_Eng(5,40,16,16,"������");break;
		case 4:OLED_Chin_Eng(5,40,16,16,"������");break;
		case 5:OLED_Chin_Eng(5,40,16,16,"������");break;
		case 6:OLED_Chin_Eng(5,40,16,16,"������");break;
	}                                        
}
