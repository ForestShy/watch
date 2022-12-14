#ifndef _OLED_H_
#define _OLED_H_
#include "stm32f4xx.h"
#include "string.h"
//#include "bitband.h"
#include "delay.h"
#include "usart.h"
#include "ZIMO.h"
//#include "UI.h"
#define  OLED_DC(value)  GPIO_WriteBit(GPIOA,GPIO_Pin_15,value)//PA_OUT(15)
#define  OLED_RES(value) GPIO_WriteBit(GPIOB,GPIO_Pin_13,value)//PB_OUT(13)
#define  OLED_CS(value)  GPIO_WriteBit(GPIOB,GPIO_Pin_7,value)//PB_OUT(7)
#define  OLED_SCL(value) GPIO_WriteBit(GPIOB,GPIO_Pin_3,value)//PB_OUT(3)
#define  OLED_SI(value)  GPIO_WriteBit(GPIOB,GPIO_Pin_5,value)//PB_OUT(5)
#define SIZE 16
#define Max_Column	128
#define OLED_COM  0
#define OLED_Data 1
extern void OLED_GPIO_Init(void);
extern uint8_t SPI1_ReadWriteByte (uint8_t Data);
extern void OLED_REST(void);
extern void OLED_ReadWriteByte(u8 data,u8 com_data);
extern void OLED_Config(void);
extern void OLED_Init(void);
extern void OLED_clear(u8 data);
extern u8 OLED_Set_Pos(u8 page, u8 column);
extern void gund_wensdu(u8 opo);
extern void gundong(u8 flag,u8 page,u8 column,u32 size_x,u32 size_y);
extern void Show_XXx(u8 page,u8 column,u32 size_x,u32 size_y,char *p);
extern void Show_Char(u8 page,u8 column,u32 size_x,u32 size_y,char *p);
extern void Show_Chars(u8 page,u8 column,u32 size_x,u32 size_y,char *q);
void OLED_Chin_Eng(u8 page,u8 column,u32 size_x,u32 size_y,char *q);
void kaiji(void);
void guanji(void);
#endif
