/*******************************************************************************
Copyright 2008 - 2016 ��������ӯ��Ƽ����޹�˾. All rights reserved.
�ļ���:        ii.h
����   :       ģ��iicͷ�ļ�
����   :       Jahol Fan
�汾   :       V1.0
�޸�   :   
������ڣ�     2016.9.19
*******************************************************************************/
#ifndef _I2C_H
#define _I2C_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "stm32f4xx.h"
/*********************************************************************
 * TYPEDEFS
 */
/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */



/*********************************************************************
 * MACROS
 */

/*********************************************************************
 *PUBLIC FUNCTIONS DECLARE
 */
void IIC_PinInit(void);
void IIC_Start(void);
void IIC_Stop(void);
u8 IIC_WriteByte(u8 data);
u8  IIC_ReadByte(u8 ack);




/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _IIC_H */