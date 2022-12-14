#include "oled.h"

void OLED_GPIO_Init(void)
{
	RCC->AHB1ENR |=(1<<0)|(1<<1);//串口1时钟使能
	RCC->APB2ENR |=(1<<12);
	//设置PB7,PB13,PA15工作模式-通用功能模式
	GPIOB->MODER &=~(3<<14);
	GPIOB->MODER |=(1<<14);
	GPIOB->MODER &=~(3<<26);
	GPIOB->MODER |=(1<<26);
	GPIOA->MODER &=~((u32)3<<30);
	GPIOA->MODER |=(1<<30);
	//设置PB3,PB5工作模式-复用功能模式
	GPIOB->MODER &=~(3<<6);
	GPIOB->MODER |=(2<<6);
	GPIOB->MODER &=~(3<<10);
	GPIOB->MODER |=(2<<10);
	//设置复用功能模式-SPI1
	GPIOB->AFR[0] &=~(0XF<<12);//--F技术参考手册P192
	GPIOB->AFR[0] |=(0X5<<12);
	GPIOB->AFR[0] &=~(0XF<<20);//--F技术参考手册P192
	GPIOB->AFR[0] |=(0X5<<20);
	//输出模式配置-推挽输出
	GPIOA->OTYPER &=~(1<<15);
	GPIOB->OTYPER &=~(1<<3);
	GPIOB->OTYPER &=~(1<<5);
	GPIOB->OTYPER &=~(1<<7);
	GPIOB->OTYPER &=~(1<<13);
	//输出速率配置-50MHZ
	GPIOA->OSPEEDR &=~((u32)3<<30);
	GPIOA->OSPEEDR |=((u32)2<<30);
	GPIOB->OSPEEDR &=~(3<<6);
	GPIOB->OSPEEDR |=(2<<6);
	GPIOB->OSPEEDR &=~(3<<10);
	GPIOB->OSPEEDR |=(2<<10);
	GPIOB->OSPEEDR &=~(3<<14);
	GPIOB->OSPEEDR |=(2<<14);
	GPIOB->OSPEEDR &=~(3<<26);
	GPIOB->OSPEEDR |=(2<<26);
	//上拉下拉模式配置-浮空模式
	GPIOA->PUPDR &=~((u32)3<<30);
	GPIOA->PUPDR |=(0<<30);
	GPIOB->PUPDR &=~(3<<6);
	GPIOB->PUPDR |=(0<<6);
	GPIOB->PUPDR &=~(3<<10);
	GPIOB->PUPDR |=(0<<10);
	GPIOB->PUPDR &=~(3<<14);
	GPIOB->PUPDR |=(0<<14);
	GPIOB->PUPDR &=~(3<<26);
	GPIOB->PUPDR |=(0<<26);

	SPI1->CR1 |= (1<<9)|(1<<8);//软件从设备管理
	SPI1->CR1 &=~ (7<<3);
	SPI1->CR1 |= (4<<3);//fclk/32
	SPI1->CR1 |= (1<<2);//主设备
	SPI1->CR1 |= (1<<6);//使能SPI
}
uint8_t SPI1_ReadWriteByte (uint8_t Data)
{
	while (!(SPI1->SR & (1<<1)));
	SPI1->DR = Data;
	while (!(SPI1->SR & (1<<0)));
	return SPI1->DR;
}
void OLED_REST(void)
{
		OLED_RES(Bit_SET); 
		Delayms(200); 
		OLED_RES(Bit_RESET); 
		Delayms(200); 
		OLED_RES(Bit_SET); 
		Delayms(200);
}
void OLED_ReadWriteByte(u8 data,u8 com_data) 
{ 
  com_data ?(OLED_DC(Bit_SET)):(OLED_DC(Bit_RESET));
  OLED_CS(Bit_RESET);
  SPI1_ReadWriteByte (data); 
  OLED_CS(Bit_SET);
}
void OLED_Config(void)
{
	OLED_ReadWriteByte(0xAE,OLED_COM); /*display off*/
	OLED_ReadWriteByte(0x00,OLED_COM); /*set lower column address*/
	OLED_ReadWriteByte(0x10,OLED_COM); /*set higher column address*/
	
	OLED_ReadWriteByte(0x40,OLED_COM); /*set display start line*/
	
	OLED_ReadWriteByte(0xB0,OLED_COM); /*set page address*/
	OLED_ReadWriteByte(0x81,OLED_COM); /*contract control*/
	OLED_ReadWriteByte(0x66,OLED_COM); /*128*/
	OLED_ReadWriteByte(0xA1,OLED_COM); /*set segment remap*/
	OLED_ReadWriteByte(0xA6,OLED_COM); /*normal / reverse--换颜色*/
	OLED_ReadWriteByte(0xA8,OLED_COM); /*multiplex ratio*/
	OLED_ReadWriteByte(0x3F,OLED_COM); /*duty = 1/64*/
	OLED_ReadWriteByte(0xC8,OLED_COM); /*Com scan direction*/
	
	OLED_ReadWriteByte(0xD3,OLED_COM); /*set display offset-设置偏移量*/
	OLED_ReadWriteByte(0x00,OLED_COM);
	
	OLED_ReadWriteByte(0xD5,OLED_COM); /*set osc division*/
	OLED_ReadWriteByte(0x80,OLED_COM);
	OLED_ReadWriteByte(0xD9,OLED_COM); /*set pre-charge period*/
	OLED_ReadWriteByte(0x1f,OLED_COM);
	OLED_ReadWriteByte(0xDA,OLED_COM); /*set COM pins*/
	OLED_ReadWriteByte(0x12,OLED_COM);
	OLED_ReadWriteByte(0xdb,OLED_COM); /*set vcomh*/
	OLED_ReadWriteByte(0x30,OLED_COM);
	OLED_ReadWriteByte(0x8d,OLED_COM); /*set charge pump enable*/
	OLED_ReadWriteByte(0x14,OLED_COM);
	OLED_ReadWriteByte(0xAF,OLED_COM); /*display ON*/
	OLED_clear(0);
 } 
void OLED_Init(void)
{
	OLED_GPIO_Init();
	OLED_CS(Bit_SET);
	OLED_REST();
	OLED_Config();
}
void OLED_clear(u8 data)
{
   uint16_t i=0,j=0;
   for(i=0;i<8;i++)
  {
//	  OLED_ReadWriteByte(0x2F,OLED_COM);
//	  OLED_ReadWriteByte(0x29,OLED_COM);
	  OLED_ReadWriteByte(0XB0+i,OLED_COM); //设置页地址
	  OLED_ReadWriteByte(0X00+0,OLED_COM); //设置列地址低四位
	  OLED_ReadWriteByte(0X10+0,OLED_COM); //设置列地址高四位--出错
    for(j=0;j<132;j++)
	  {
		 OLED_ReadWriteByte(data,OLED_Data);
    }
   } 
 }
//设置光标
u8 OLED_Set_Pos(u8 page, u8 column)
{
	 if(page>=8|column>=132)
	 return 0;
	 OLED_ReadWriteByte(0XB0+page,OLED_COM); //设置页地址
	 OLED_ReadWriteByte((0X00+(column & 0xf)),OLED_COM); //设置列地址低四位
	 OLED_ReadWriteByte((0X10+((column>>4)&0xf)),OLED_COM);
	 return 1;
 }
//page--页,column--列,size_x--字库列,size_y--字库行,p--字库
 
void Show_XXx(u8 page,u8 column,u32 size_x,u32 size_y,char *p)
{
	u16 i,j=0;
	for(i=0;i<(size_y/8);i++)
	{
		  if(OLED_Set_Pos(page+i,column))
		  for(j=0;j<size_x;j++)
	   {
			 OLED_ReadWriteByte(p[(i*size_x)+j],OLED_Data);
		 }
	 }
}
//显示任意汉字
void Show_Hanz(u8 page,u8 column,u32 size_x,u32 size_y,char *p)
{
		u16 i,j=0,k=0;
	  u32 offset=0;
	  u32 lenth=strlen(HanZ_list);
	  k=size_x*size_y/8;
	  for(i=0;i<lenth;i+=2)//注意lenth此处不要除以2
	  {
			if((p[0]==HanZ_list[i])&&(p[1]==HanZ_list[i+1]))
			{
				i=i/2;
				break;
			}
		}
		offset=i*k;//计算偏移量
	  for(i=0;i<(size_y/8);i++)
	 {
		  if(OLED_Set_Pos(page+i,column))
		  for(j=0;j<size_x;j++)
	   {
			 OLED_ReadWriteByte(HanZ_16X16[offset+(i*size_x)+j],OLED_Data);//16*16--汉字
		 }
	 }
}
//显示任意一个ASCII字符
void Show_Char(u8 page,u8 column,u32 size_x,u32 size_y,char *p)
{
		u16 i,j=0,k=0;
	  k=((*p)-32)*(size_x*size_y/8);//计算偏移量
	  for(i=0;i<(size_y/8);i++)
	 {
		  if(OLED_Set_Pos(page+i,column))
		  for(j=0;j<size_x;j++)
	   {
			 if(size_y==16)
			 OLED_ReadWriteByte(ASII_8X16[k+(i*size_x)+j],OLED_Data);//8*16--ASCII值，重大错误，多加了一个元素，忽略了第0个元素
			 else if(size_y==24)
			 OLED_ReadWriteByte(ASII_12X24[k+(i*size_x)+j],OLED_Data);//12*24--ASCII值
		 }
	 }
// Show_XXx(page,column,size_x,size_y,(u8*)&ASII_8X16[(k+1)]);
 }
//显示任意一串ASCII字符
void Show_Chars(u8 page,u8 column,u32 size_x,u32 size_y,char *q)
{
	int i;
  int len=strlen((char *)q);
	for(i=0;i<len;i++)
	{
     Show_Char(page,column+(i*size_x),size_x,size_y,&q[i]);
	}
}
//中英文混合显示
void OLED_Chin_Eng(u8 page,u8 column,u32 size_x,u32 size_y,char *q)
{
	 while(*q!=NULL)
	 {  
		 if(*q>(char)0xa0)//此处注意，经查某些中文的标点符号后一个机内码刚好等于0xa1
	    {
			  Show_Hanz(page,column,size_x,size_y,q);
				column+=size_x;
				q+=2;
		  }
		  else 
		  {
		
		    Show_Char(page,column,size_x/2,size_y,q);
				column+=(size_x/2);
        q+=1;
		  }
			if((132-column)<0)
			{
				column=0;
//			  page+=(size_y/8);
			}
	  }
}
void kaiji(void)
{
	//开机界面
	u8 i=0;
	OLED_clear(0);
	//实现滚动显示
		OLED_Chin_Eng(0,20,16,16,"欢 迎 使 用");
	for(i=0;i<=128;i+=16)
	{
//		OLED_Chin_Eng(0,i,16,16,".3");
//		OLED_Chin_Eng(0,i-16,16,16,"V1");
		OLED_Chin_Eng(3,i-(16*1),16,16,"环");
		OLED_Chin_Eng(3,i-(16*2),16,16,"手");
		OLED_Chin_Eng(3,i-(16*3),16,16,"能");
		OLED_Chin_Eng(3,i-(16*4),16,16,"智");
		OLED_Chin_Eng(3,i-(16*5),16,16,"  ");
		Delayms(500);
		if(i==96) break;
//		Show_XXx(0,i,16,16,(char *)kong16_16);
	}
	Delayms(500);
//	OLED_Chin_Eng(0,0,16,16,"                 ");
//	Show_XXx(0,0,64,64,(char*)"GIFT_03");
//	Delayms(800);
//	Show_XXx(0,0,64,64,(char*)"GIFT_02");

	OLED_Chin_Eng(6,44,16,16,"=^ ^=");
	OLED_Chin_Eng(6,60,16,16,"_");
	Delayms(2000);
	OLED_clear(0);
}
void guanji(void)
{
	OLED_Chin_Eng(0,16,16,16,"  生");
	Delayms(300);
	OLED_Chin_Eng(0,48,16,16,"活");
	Delayms(300);
	OLED_Chin_Eng(0,64,16,16,"不");
	Delayms(300);
	OLED_Chin_Eng(0,80,16,16,"只");
	Delayms(300);
	OLED_Chin_Eng(0,96,16,16,"眼");
	Delayms(300);
	OLED_Chin_Eng(0,112,16,16,"前");
	Delayms(300);
	OLED_Chin_Eng(2,2,16,16,"的");
	Delayms(300);
	OLED_Chin_Eng(2,18,16,16,"苟");
	Delayms(300);
	OLED_Chin_Eng(2,34,16,16,"且");
	Delayms(300);
	OLED_Chin_Eng(2,50,16,16,",");
	Delayms(300);
	OLED_Chin_Eng(2,66,16,16,"还");
	Delayms(300);
	OLED_Chin_Eng(2,82,16,16,"有");
	Delayms(300);
	OLED_Chin_Eng(2,98,16,16,"诗");
	Delayms(300);
	OLED_Chin_Eng(2,114,16,16,"和");
	Delayms(300);
	OLED_Chin_Eng(4,2,16,16,"远");
	Delayms(300);
	OLED_Chin_Eng(4,18,16,16,"方");
	Delayms(300);
	OLED_Chin_Eng(4,34,16,16,"!");
	OLED_Chin_Eng(6,10,16,16,"谢谢使用,再见!");
	Delayms(1500);
}
	


