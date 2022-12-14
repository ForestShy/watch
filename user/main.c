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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ��飺��2��
	LedInit();
	UsartInit(115200);
	SystickInit();
	Tim3Init();//um��ʱ
	Tim4Init();//����ɨ��
	Key_Init();
	KEY_ADC_Init();//�������ʼ��
	OLED_Init();
	RtcInit();
	Delayms(100);
	IIC_PinInit();
	Hp_6_init();       //��ʼ��HP6����Ѫѹ ģ��
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
		sprintf(temper,"�¶�:%.1f��",temphumivalue->temperature);
		sprintf(hum,"ʪ��:%.2f%%",temphumivalue->humidity);
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
	static u8 oldPulseVal = 0;   //��һ�ε�����ֵ
	u8 uiBuf[40];
	u8 pulseBuf[24] = {0};
	OLED_Chin_Eng(3,32,16,16,"����:---");
	HP_6_OpenRate();   //�����ʲ���

	while(1)
	{		
		if(pulseBuf[7] && pulseBuf[7]!= oldPulseVal)  //������Ի�����ʽ�������������ɹ�����ر����ʲ���
		{
			oldPulseVal = pulseBuf[7];
			HP_6_CloseRate();
			sprintf((char*)uiBuf,"����:%-3d",pulseBuf[7]);//��ȡ���ʼ���� // //byte 7�����ʽ��
			OLED_Chin_Eng(3,32,16,16,(char*)uiBuf);
		}
		else //�����ȡ�������ʽ������ÿ��100�����ӻ�ȡ����ֵ
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
			HP_6_CloseRate();   //�ر����ʲ���
			IIC_Stop();
			break;  //������ǰҳ��
		}	
	}
}
void bpress_page(void)
{
	OLED_clear(0);
		LEDON;
	u8 uiBuf[40];
	u8 BpBuf[24] = {0};
	OLED_Chin_Eng(3,32,16,16,"Ѫѹ������");
	HP_6_OpenBp();   //��Ѫѹ����
	while(1)
	{
		HP_6_GetBpResult();                //��ȡѪѹ����״̬
		HP_6_GetResultData(BpBuf);
		if(0 == BpBuf[7])                   //byte 7��Ѫѹ������״̬:0�����У�1������ɡ�2����ʧ��
			OLED_Chin_Eng(3,32,16,16,"Ѫѹ������"); 
		else if(1 == BpBuf[7])  
		{
			OLED_Chin_Eng(1,32,16,16,"�����ɹ�");
			sprintf((char*)uiBuf,"G/L:%-3d/%-3dmmHg", BpBuf[10], BpBuf[11]);//byte 10��Ѫѹ��������ĸ�ѹ,byte 11��Ѫѹ��������ĵ�ѹ	
			OLED_Chin_Eng(3,4,16,16,(char*)uiBuf); 
			HP_6_CloseBp();	 
		}		 
		else if(2 == BpBuf[7])         
		{
			HP_6_CloseBp();
			OLED_Chin_Eng(3,32,16,16,"����ʧ��");	 
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
			HP_6_CloseBp();   //�ر�Ѫѹ����
			IIC_Stop();
			break;  //������ǰҳ��
		}	
	}
}

void step_page(void)
{
	OLED_clear(0);
	LEDON;
	char uiBuf[40];
	uint8_t temp=0;
	short aacx,aacy,aacz;									//���ٶȴ�����ԭʼ����
	MPU_Init();

	while(1)
	{
		Delayms(100);
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
		printf("aacx:%5d,aacy:%5d,aacz:%5d\r\n",aacx,aacy,aacz);
		temp=CountStep(aacx,aacy,aacz);
		sprintf(uiBuf,"����:%-3d",temp);
		OLED_Chin_Eng(3,40,16,16,uiBuf);
		if(Mode==2)
		{
			LEDOFF;
			Low_Power();
			Mode=0;
		}
		if(num!=4)
		{
			HP_6_CloseBp();   //�ر�Ѫѹ����
			IIC_Stop();
			break;  //������ǰҳ��
		}
	}
}
void Low_Power(void)
{
	while(1)					//����͹���ģʽ
	{
		OLED_clear(0);		//�ر�OLED
		if(Mode==1)		//�˳��͹���
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
	��ʱ��˵��
	TIM4 -- APB1
	��ʱ��TIM4��100MHZ
	16λ��ʱ����ֵ��Χ��0~65535
	*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;        		//��ʼ����ʱ���ṹ��
	NVIC_InitTypeDef  NVIC_InitStruct;								//����NVIC�ṹ��
	//1��ʹ�ܶ�ʱ��ʱ�ӡ�
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	//2����ʼ����ʱ��������ARR,PSC��
	TIM_TimeBaseInitStruct.TIM_Prescaler    = 50000-1;    			//50000��Ƶ 100MHZ/50000 = 2kHZ 0.5ms
	TIM_TimeBaseInitStruct.TIM_Period		= 100-1;  				//��100���� ��2KHZ�£���ʱ50ms
	TIM_TimeBaseInitStruct.TIM_CounterMode  = TIM_CounterMode_Up; 	//���ϼ���
	TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1; 		//��Ƶ����
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	//3������ TIM4_DIER  ��������ж�
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update); 						//����жϱ�־λ
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	//3���ж�����
	NVIC_InitStruct.NVIC_IRQChannel= TIM4_IRQn;						//NVICͨ������stm32f4xx.h�ɲ鿴ͨ�� ���ɱ䣩
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority= 0x03;	    //��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority= 0x03;	    		//��Ӧ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd= ENABLE;   					//ʹ��
	NVIC_Init(&NVIC_InitStruct);		
	//5��ʹ�ܶ�ʱ����
	TIM_Cmd(TIM4, ENABLE);
}


uint8_t key_flag=0;

//��4  �ң�3
void TIM4_IRQHandler(void)
{
	uint8_t key=0;
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //����ж�
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update); //����жϱ�־λ
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
