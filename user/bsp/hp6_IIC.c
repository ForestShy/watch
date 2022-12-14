#include "stm32f4xx.h"
#include "delay.h"


#define HP6_SDA_H   (GPIOA->BSRRL = 1<<1)
#define HP6_SDA_L   (GPIOA->BSRRH = 1<<1)  //PA1

#define HP6_SCL_H   (GPIOA->BSRRL = 1<<2)  //PA2
#define HP6_SCL_L   (GPIOA->BSRRH = 1<<2)

#define HP6_SDA_IN   ( !!(GPIOA->IDR & (1<<1)) )//PA1



//SCL  ---  PA2
//SDA  ---  PA1
void HP6IIC_Init()
{
	RCC->AHB1ENR |= 1<<0;//PA
	
	GPIOA->MODER &= ~(0XF<<2);
	GPIOA->MODER |= (0X5<<2);  //通用输出
	    
	GPIOA->OTYPER &= ~(1<<2);// PA2  SCL 推挽输出
	GPIOA->OTYPER |= (1<<1);//  PA1  SDA 开漏输出
	
//  /*****IIC_SCL配置为开漏输出****/
//  RCC->AHB1ENR |= 1<<0;//使能PA时钟
//  GPIOA->MODER &=~(3<<4);//清零
//  GPIOA->MODER |= 1<<4;//GPIOA2配置通用输出模式
//  GPIOA->OTYPER |= 1<<2;//GPIOA2配置为开漏模式
//  /****IIC_SDA配置为开漏输出****/
//  GPIOA->MODER &=~(3<<2);//清零
//  GPIOA->MODER |= 1<<2;//GPIOA1配置普通模式
//  GPIOA->OTYPER |= 1<<1;//GPIOA1配置为通用输出模式
	
	HP6_SDA_H;
	HP6_SCL_H;	
}

void HP6IIC_Start(void)
{
  Delayus(5);
  HP6_SCL_H;
  HP6_SDA_H;
  Delayus(5);//延时5us
  HP6_SDA_L;
  Delayus(5);//延时5us
  HP6_SCL_L;

}

void HP6IIC_Stop(void)
{
  HP6_SDA_L;
  Delayus(5);
  HP6_SCL_H;
  Delayus(5);//延时5us
  HP6_SDA_H;
  Delayus(5);//延时5us
	
}

//data ：待发送的数据
//返回值：0表示应答，1表示非应答
u8 HP6IIC_WriteByte(u8 data)
{
  u8 i;
  u8 ack = 2;
  for(i=0;i<8;i++)  //8个时钟传输8位数据，从最高位开始传输
  {
    if(data &(1<<(7-i)))  
    {
      HP6_SDA_H;
    }
    else
    {
      HP6_SDA_L;
    }
    Delayus(10);//延时
    HP6_SCL_H; 
    Delayus(10);//延时
    HP6_SCL_L;
  }
  HP6_SDA_H;//SDA输出高低电平切换为输入
  Delayus(10);
  HP6_SCL_H;
  if(HP6_SDA_IN)//SDA输入的电平为高电平表示非应答
  {
    ack = 1;
  }
  else  //SDA输入的电平为低电平表示应答
  {
    ack = 0;
  }
  Delayus(10);
  HP6_SCL_L;
  return ack;
}

//返回值表示读取到的数据
//形参ack：表示读取一个字节数据后给出的应答，0：应答，1：非应答
u8  HP6IIC_ReadByte(u8 ack)
{
  u8 i;
  u8 data = 0;
  HP6_SDA_H;//SDA输出高电平，切换为输入
  for(i=0;i<8;i++)  //产生8个时钟，读取8位数据
  {
    Delayus(10);
    HP6_SCL_H;
    data = data <<1;
    if(HP6_SDA_IN)
    {
      data |= 1<<0;
    }
    else   //SDA输入的低电平电平
    {
      data &=~(1<<0);
    }	
    Delayus(10); 
    HP6_SCL_L;		
  }
  //产生第九个时钟，发送应答信号。
  if(ack == 0)
  {
    HP6_SDA_L;   
  }
  else
  {
    HP6_SDA_H;
  }
  Delayus(10);
  HP6_SCL_H;
  Delayus(10);
  HP6_SCL_L;
  return data;
}
