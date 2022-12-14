#include "i2c.h"
#include "stm32f4xx.h"
#include "delay.h"
//IIC_SCL  --- 》 PB8
//IIC_SDA  --- 》 PB9

#define SCL_H  (GPIOB->ODR |= 1<<8)
#define SCL_L  (GPIOB->ODR &=~(1<<8))
#define SDA_H  (GPIOB->ODR |= 1<<9)
#define SDA_L  (GPIOB->ODR &=~(1<<9))

#define SDA_IN !!(GPIOB->IDR&(1<<9))


void IIC_PinInit(void)
{
  
  /*****IIC_SCL配置为开漏输出****/
  RCC->AHB1ENR |= 1<<1;//使能PB时钟
  GPIOB->MODER &=~(3<<16);//清零
  GPIOB->MODER |= 1<<16;//GPIOB8配置通用输出模式
  GPIOB->OTYPER |= 1<<8;//GPIOB8配置为开漏模式
  /****IIC_SDA配置为开漏输出****/
  GPIOB->MODER &=~(3<<18);//清零
  GPIOB->MODER |= 1<<18;//GPIOB9配置普通模式
  GPIOB->OTYPER |= 1<<9;//GPIOB9配置为通用输出模式
  
  SCL_H;
  SDA_H;
}

void IIC_Start(void)
{
  Delayus(2);
  SCL_H;
  SDA_H;
  Delayus(2);//延时5us
  SDA_L;
  Delayus(2);//延时5us
  SCL_L;
}

void IIC_Stop(void)
{
  SDA_L;
  Delayus(2);
  SCL_H;
  Delayus(2);//延时5us
  SDA_H;
  Delayus(2);//延时5us
}

//data ：待发送的数据
//返回值：0表示应答，1表示非应答
u8 IIC_WriteByte(u8 data)
{
  u8 i;
  u8 ack = 2;
  for(i=0;i<8;i++)  //8个时钟传输8位数据，从最高位开始传输
  {
    if(data &(1<<(7-i)))  
    {
      SDA_H;
    }
    else
    {
      SDA_L;
    }
    Delayus(2);//延时5us
    SCL_H; 
    Delayus(2);//延时5us
    SCL_L;
  }
  SDA_H;//SDA输出高低电平切换为输入
  Delayus(2);//延时5us
  SCL_H;
  if(SDA_IN)//SDA输入的电平为高电平表示非应答
  {
    ack = 1;
  }
  else  //SDA输入的电平为低电平表示应答
  {
    ack = 0;
  }
  Delayus(2);//延时5us
  SCL_L;
  return ack;
}

//返回值表示读取到的数据
//形参ack：表示读取一个字节数据后给出的应答，0：应答，1：非应答
u8  IIC_ReadByte(u8 ack)
{
  u8 i;
  u8 data = 0;
  SDA_H;//SDA输出高电平，切换为输入
  for(i=0;i<8;i++)  //产生8个时钟，读取8位数据
  {
    Delayus(2);//延时5us
    SCL_H;
    data = data <<1;
    if(SDA_IN)
    {
      data |= 1<<0;
    }
    else   //SDA输入的低电平电平
    {
      data &=~(1<<0);
    }	
    Delayus(2); //延时5us
    SCL_L;		
  }
  //产生第九个时钟，发送应答信号。
  if(ack == 0)
  {
    SDA_L;   
  }
  else
  {
    SDA_H;
  }
  Delayus(2);//延时5us
  SCL_H;
  Delayus(2);//延时5us
  SCL_L;
  return data;
}
