#include "stm32f4xx.h"
#include "rtc.h"
#include "delay.h"
#include "stdio.h"
#include "oled.h"

#define LSE_ON
//RTC进入初始化模式
//返回值:0,成功;1,失败;

//RTC初始化
//返回值:0,初始化成功;
//       1,LSE开启失败;
//       2,进入初始化模式失败;
uint8_t RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);
uint8_t RTC_Set_Date(u8 year,u8 month,u8 date,u8 week);

RTC_TimeTypeDef RTC_TimeStruct={0};
RTC_DateTypeDef RTC_DateStruct={0};

u8 RTC_Init_Mode(void)
{ 
	u32 retry=0X10000; 
	if(RTC->ISR&(1<<6))return 0; 
	RTC->ISR|=1<<7;	//进入RTC初始化模式
	while(retry&&((RTC->ISR&(1<<6))==0x00))//等待进入RTC初始化模式成功
	{
		retry--;
	}
    if(retry==0)return 1;	//同步失败
	else return 0; 			//同步成功 
}


u8 RtcInit(void)
{
	#ifdef LSE_ON 
	u16 retry=0xFF; 
	RCC->APB1ENR|=1<<28;			//使能电源接口时钟
	PWR->CR|=1<<8;					//后备区域访问使能(RTC+SRAM)
	RCC->BDCR|=1<<0;				//LSE 开启  
//	if(RTC->BKP0R != 0x1234)		//是否第一次配置?
	{
		while(retry&&((RCC->BDCR&0X02)==0))//等待LSE准备好
		{
			retry--;
			Delayms(5);
		}
		if(retry==0)return 1;		//LSE 开启失败. 
		RCC->BDCR&=~(1<<9);			//选择LSE,作为RTC的时钟
		RCC->BDCR|=1<<8;			//选择LSE,作为RTC的时钟
		RCC->BDCR|=1<<15;			//使能RTC时钟
	#else
		u16 retry=0xFF; 
		RCC->APB1ENR|=1<<28;			//使能电源接口时钟
		PWR->CR|=1<<8;					//后备区域访问使能(RTC+SRAM)
		RCC->CSR |= 1<<0;			//LSI 开启  
	if(RTC->BKP0R != 0x1234)		//是否第一次配置?
	{
		while(retry&&((RCC->CSR&0X02)==0))//等待LSI准备好
		{
			retry--;
			Delayms(5);
		}
		if(retry==0)return 1;		//LSE 开启失败. 
		RCC->BDCR|=2<<8;			//选择LSI,作为RTC的时钟
		RCC->BDCR|=1<<15;			//使能RTC时钟
	#endif
 		//关闭RTC寄存器写保护
		RTC->WPR=0xCA;
		RTC->WPR=0x53; 
		if(RTC_Init_Mode())return 2;//进入RTC初始化模式
		RTC->PRER=0XFF;				//RTC同步分频系数(0~7FFF),必须先设置同步分频,再设置异步分频,Frtc=Fclks/((Sprec+1)*(Asprec+1))
		RTC->PRER|=0X75<<16;		//RTC异步分频系数(1~0X7F)
		RTC->CR&=~(1<<6);			//RTC设置为,24小时格式
		RTC->ISR&=~(1<<7);			//退出RTC初始化模式
		RTC->WPR=0xFF;				//使能RTC寄存器写保护  
		RTC_Set_Time(12,00,00,RTC_H12_AM);	//设置时间
		RTC_Set_Date(22,01,01,6);		//设置日期
		//RTC_Set_AlarmA(7,0,0,10);	//设置闹钟时间
//		RTC->BKP0R = 0x1234;	//标记已经初始化过了
	} 
	//RTC_Set_WakeUp(4,0);			//配置WAKE UP中断,1秒钟中断一次 
	return 0;
}

//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//返回值:0,成功
//       1,进入初始化模式失败 

uint8_t RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;    //小时
	RTC_TimeTypeInitStructure.RTC_Minutes=min;   //分钟
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;   //秒
	RTC_TimeTypeInitStructure.RTC_H12=ampm;      //时间格式
	
	RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
	return  0;
	
}

//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//返回值:0,成功
//       1,进入初始化模式失败 

uint8_t RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	
	RTC_DateTypeInitStructure.RTC_Date=date;         //日
	RTC_DateTypeInitStructure.RTC_Month=month;       //月
	RTC_DateTypeInitStructure.RTC_WeekDay=week;      //星期
	RTC_DateTypeInitStructure.RTC_Year=year;         //年
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
	sprintf(current_date,"20%02d年%02d月%02d日",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
	OLED_Chin_Eng(1,10,16,16,current_date);
	switch(RTC_DateStruct.RTC_WeekDay)
	{
		case 0:OLED_Chin_Eng(5,40,16,16,"星期日");break;
		case 1:OLED_Chin_Eng(5,40,16,16,"星期一");break;
		case 2:OLED_Chin_Eng(5,40,16,16,"星期二");break;
		case 3:OLED_Chin_Eng(5,40,16,16,"星期三");break;
		case 4:OLED_Chin_Eng(5,40,16,16,"星期四");break;
		case 5:OLED_Chin_Eng(5,40,16,16,"星期五");break;
		case 6:OLED_Chin_Eng(5,40,16,16,"星期六");break;
	}                                        
}
