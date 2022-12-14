#include "main.h"


int num=0;
uint8_t Mode=0;
uint16_t power_time=0;
void Tim4Init(void);
void time_page(void);
void th_page(void);
void rate_page(void);
void bpress_page(void);
void step_page(void);
void Low_Power(void);
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组：第2组
	LedInit();
	UsartInit(115200);
	SystickInit();
	Tim3Init();//um延时
	Tim4Init();//按键扫描
	Key_Init();
	KEY_ADC_Init();//五项按件初始化
	OLED_Init();
	RtcInit();
	Delayms(100);
	IIC_PinInit();
	Hp_6_init();       //初始化HP6心率血压 模块
	kaiji();
	MPU_Init();
	while(1)
	{
		if(num<0)
			num=4;
		if(num>4)
			num=0;
		switch(num)
		{
			case 0:time_page();break;
			case 1:th_page();break;
			case 2:rate_page();break;
			case 3:bpress_page();break;
			case 4:step_page();break;
			default:break;
		}
	}
}

void time_page(void)
{
	OLED_clear(0);
	LEDON;
	while(num==0)
	{
		GetTime();
		GetDate();
		Delayms(100);
		if(Mode==2)
		{
			LEDOFF;
			Low_Power();
			Mode=0;
		}
	}
}

void th_page(void)
{
	OLED_clear(0);
		LEDON;
	TempHumiValue_t * temphumivalue={0};
	char temper[20]={0};
	char hum[20]={0};
	while(num==1)
	{
		temphumivalue=SHT20_readTemAndHum();
		sprintf(temper,"温度:%.1f℃",temphumivalue->temperature);
		sprintf(hum,"湿度:%.2f%%",temphumivalue->humidity);
		OLED_Chin_Eng(2,20,16,16,temper);
		OLED_Chin_Eng(5,20,16,16,hum);
		Delayms(100);
		if(Mode==2)
		{
			LEDOFF;
			Low_Power();
			Mode=0;
		}
	}
}
void rate_page(void)
{
	OLED_clear(0);
		LEDON;
	static u8 oldPulseVal = 0;   //上一次的心率值
	u8 uiBuf[40];
	u8 pulseBuf[24] = {0};
	OLED_Chin_Eng(3,32,16,16,"心率:---");
	HP_6_OpenRate();   //打开心率测量

	while(1)
	{		
		if(pulseBuf[7] && pulseBuf[7]!= oldPulseVal)  //如果可以获得心率结果，表明测量成功，则关闭心率测量
		{
			oldPulseVal = pulseBuf[7];
			HP_6_CloseRate();
			sprintf((char*)uiBuf,"心率:%-3d",pulseBuf[7]);//获取心率检测结果 // //byte 7是心率结果
			OLED_Chin_Eng(3,32,16,16,(char*)uiBuf);
		}
		else //如果获取不到心率结果，则每隔100毫秒钟获取心率值
		{
			HP_6_GetRateResult();
			HP_6_GetResultData(pulseBuf);
			Delayms(100);
		}
		if(Mode==2)
		{
			LEDOFF;
			Low_Power();
			Mode=0;
		}
		
		
		if(num!=2)
		{
			HP_6_CloseRate();   //关闭心率测量
			IIC_Stop();
			break;  //跳出当前页面
		}	
	}
}
void bpress_page(void)
{
	OLED_clear(0);
		LEDON;
	u8 uiBuf[40];
	u8 BpBuf[24] = {0};
	OLED_Chin_Eng(3,32,16,16,"血压测量中");
	HP_6_OpenBp();   //打开血压测量
	while(1)
	{
		HP_6_GetBpResult();                //获取血压测量状态
		HP_6_GetResultData(BpBuf);
		if(0 == BpBuf[7])                   //byte 7是血压测量的状态:0测量中，1测量完成、2测量失败
			OLED_Chin_Eng(3,32,16,16,"血压测量中"); 
		else if(1 == BpBuf[7])  
		{
			OLED_Chin_Eng(1,32,16,16,"测量成功");
			sprintf((char*)uiBuf,"G/L:%-3d/%-3dmmHg", BpBuf[10], BpBuf[11]);//byte 10是血压测量结果的高压,byte 11是血压测量结果的低压	
			OLED_Chin_Eng(3,4,16,16,(char*)uiBuf); 
			HP_6_CloseBp();	 
		}		 
		else if(2 == BpBuf[7])         
		{
			HP_6_CloseBp();
			OLED_Chin_Eng(3,32,16,16,"测量失败");	 
		}		 
		else
			Delayms(50);
		if(Mode==2)
		{
			LEDOFF;
			Low_Power();
			Mode=0;
		}
		if(num!=3)
		{
			HP_6_CloseBp();   //关闭血压测量
			IIC_Stop();
			break;  //跳出当前页面
		}	
	}
}

void step_page(void)
{
	OLED_clear(0);
	LEDON;
	char uiBuf[40];
	uint8_t temp=0;
	short aacx,aacy,aacz;									//加速度传感器原始数据
	MPU_Init();

	while(1)
	{
		Delayms(100);
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
		printf("aacx:%5d,aacy:%5d,aacz:%5d\r\n",aacx,aacy,aacz);
		temp=CountStep(aacx,aacy,aacz);
		sprintf(uiBuf,"步数:%-3d",temp);
		OLED_Chin_Eng(3,40,16,16,uiBuf);
		if(Mode==2)
		{
			LEDOFF;
			Low_Power();
			Mode=0;
		}
		if(num!=4)
		{
			HP_6_CloseBp();   //关闭血压测量
			IIC_Stop();
			break;  //跳出当前页面
		}
	}
}
void Low_Power(void)
{
	while(1)					//进入低功耗模式
	{
		OLED_clear(0);		//关闭OLED
		if(Mode==1)		//退出低功耗
		{
			Delayms(500);
					LEDON;
			break;				
		}	
	}
}
void Tim4Init(void)
{
	/*
	定时器说明
	TIM4 -- APB1
	定时器TIM4：100MHZ
	16位定时器：值范围：0~65535
	*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;        		//初始化定时器结构体
	NVIC_InitTypeDef  NVIC_InitStruct;								//配置NVIC结构体
	//1、使能定时器时钟。
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	//2、初始化定时器，配置ARR,PSC。
	TIM_TimeBaseInitStruct.TIM_Prescaler    = 50000-1;    			//50000分频 100MHZ/50000 = 2kHZ 0.5ms
	TIM_TimeBaseInitStruct.TIM_Period		= 100-1;  				//计100个数 在2KHZ下，用时50ms
	TIM_TimeBaseInitStruct.TIM_CounterMode  = TIM_CounterMode_Up; 	//向上计数
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1; 		//分频因子
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	//3、设置 TIM4_DIER  允许更新中断
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update); 						//清除中断标志位
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	//3、中断配置
	NVIC_InitStruct.NVIC_IRQChannel= TIM4_IRQn;						//NVIC通道，在stm32f4xx.h可查看通道 （可变）
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority= 0x03;	    //抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority= 0x03;	    		//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd= ENABLE;   					//使能
	NVIC_Init(&NVIC_InitStruct);		
	//5、使能定时器。
	TIM_Cmd(TIM4, ENABLE);
}


uint8_t key_flag=0;

//左：4  右：3
void TIM4_IRQHandler(void)
{
	uint8_t key=0;
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //溢出中断
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update); //清除中断标志位
		if(power_time>100)
			Mode=2;
		key=Key_Scan();
		if(!key)
			key=Key_AdcScan();
		switch(key)
		{
			case 3:
				if(Mode==2)
				{
					power_time=0;
					Mode=1;
				}
				if(key_flag==0)num++;key_flag=1;break;
			case 4:if(Mode==2)
				{
					power_time=0;
					Mode=1;
				}if(key_flag==0)num--;key_flag=1;break;
			case 5:if(Mode==2)
				{
					power_time=0;
					Mode=1;
				}break;
			case 6:if(Mode==2)
				{
					power_time=0;
					Mode=1;
				}break;
			default:break;
		}
		if(key==0)
		{
			power_time++;
			key_flag=0;
		}
	}
}
