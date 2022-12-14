#include "i2c.h"
#include "stm32f4xx.h"
#include "delay.h"
//IIC_SCL  --- �� PB8
//IIC_SDA  --- �� PB9

#define SCL_H  (GPIOB->ODR |= 1<<8)
#define SCL_L  (GPIOB->ODR &=~(1<<8))
#define SDA_H  (GPIOB->ODR |= 1<<9)
#define SDA_L  (GPIOB->ODR &=~(1<<9))

#define SDA_IN !!(GPIOB->IDR&(1<<9))


void IIC_PinInit(void)
{
  
  /*****IIC_SCL����Ϊ��©���****/
  RCC->AHB1ENR |= 1<<1;//ʹ��PBʱ��
  GPIOB->MODER &=~(3<<16);//����
  GPIOB->MODER |= 1<<16;//GPIOB8����ͨ�����ģʽ
  GPIOB->OTYPER |= 1<<8;//GPIOB8����Ϊ��©ģʽ
  /****IIC_SDA����Ϊ��©���****/
  GPIOB->MODER &=~(3<<18);//����
  GPIOB->MODER |= 1<<18;//GPIOB9������ͨģʽ
  GPIOB->OTYPER |= 1<<9;//GPIOB9����Ϊͨ�����ģʽ
  
  SCL_H;
  SDA_H;
}

void IIC_Start(void)
{
  Delayus(2);
  SCL_H;
  SDA_H;
  Delayus(2);//��ʱ5us
  SDA_L;
  Delayus(2);//��ʱ5us
  SCL_L;
}

void IIC_Stop(void)
{
  SDA_L;
  Delayus(2);
  SCL_H;
  Delayus(2);//��ʱ5us
  SDA_H;
  Delayus(2);//��ʱ5us
}

//data �������͵�����
//����ֵ��0��ʾӦ��1��ʾ��Ӧ��
u8 IIC_WriteByte(u8 data)
{
  u8 i;
  u8 ack = 2;
  for(i=0;i<8;i++)  //8��ʱ�Ӵ���8λ���ݣ������λ��ʼ����
  {
    if(data &(1<<(7-i)))  
    {
      SDA_H;
    }
    else
    {
      SDA_L;
    }
    Delayus(2);//��ʱ5us
    SCL_H; 
    Delayus(2);//��ʱ5us
    SCL_L;
  }
  SDA_H;//SDA����ߵ͵�ƽ�л�Ϊ����
  Delayus(2);//��ʱ5us
  SCL_H;
  if(SDA_IN)//SDA����ĵ�ƽΪ�ߵ�ƽ��ʾ��Ӧ��
  {
    ack = 1;
  }
  else  //SDA����ĵ�ƽΪ�͵�ƽ��ʾӦ��
  {
    ack = 0;
  }
  Delayus(2);//��ʱ5us
  SCL_L;
  return ack;
}

//����ֵ��ʾ��ȡ��������
//�β�ack����ʾ��ȡһ���ֽ����ݺ������Ӧ��0��Ӧ��1����Ӧ��
u8  IIC_ReadByte(u8 ack)
{
  u8 i;
  u8 data = 0;
  SDA_H;//SDA����ߵ�ƽ���л�Ϊ����
  for(i=0;i<8;i++)  //����8��ʱ�ӣ���ȡ8λ����
  {
    Delayus(2);//��ʱ5us
    SCL_H;
    data = data <<1;
    if(SDA_IN)
    {
      data |= 1<<0;
    }
    else   //SDA����ĵ͵�ƽ��ƽ
    {
      data &=~(1<<0);
    }	
    Delayus(2); //��ʱ5us
    SCL_L;		
  }
  //�����ھŸ�ʱ�ӣ�����Ӧ���źš�
  if(ack == 0)
  {
    SDA_L;   
  }
  else
  {
    SDA_H;
  }
  Delayus(2);//��ʱ5us
  SCL_H;
  Delayus(2);//��ʱ5us
  SCL_L;
  return data;
}
