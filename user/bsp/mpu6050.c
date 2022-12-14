#include "mpu6050.h"
#include "delay.h"  

SensorData GMeter;
unsigned short m = 0;
unsigned char n = 0;
DATATYPE DateBufferX[10] = {9000, 9000, 9000, 9000, 9000, 9000, 9000, 9000, 9000, 9000};
DATATYPE DateBufferY[10] = {9000, 9000, 9000, 9000, 9000, 9000, 9000, 9000, 9000, 9000};
DATATYPE DateBufferZ[10] = {9000, 9000, 9000, 9000, 9000, 9000, 9000, 9000, 9000, 9000};
unsigned short StepCount = 0;
unsigned char StepFlag  = 0;
unsigned char InitFlag = 0;
unsigned char m_count = 0;
unsigned char GMeterAmax = 0;
unsigned char DataSelect(SensorData *GMeter);
/***********************************************************************************************************************
Copyright 2008 - 2016 ��������ӯ��������޹�˾. All rights reserved.
�ļ���:        mpu6050.c
����   :       mpu6050�����ļ�
����   :       Jahol Fan
�汾   :       V1.0
�޸�   :   
������ڣ�     2016.10.13
Notice    :������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
��ӯ�������http://www.edu118.com/
��ӯ����У��http://www.edu118.cn/
��Ȩ���У�����ؾ���
************************************************************************************************************************/
//��ʼ��MPU6050
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU_Init(void)
{ 
  u8 res;
  //IIC_init();//��ʼ��IIC����
  MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//��λMPU6050
  Delayms(100);  //Jahol Fan ����ֵ����⣬��ʱ�ĳ��ˣ��Ͳ��ܼ���HP-6
  MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);	//����MPU6050 
  //MPU_Set_Gyro_Fsr(3);					//�����Ǵ�����,��2000dps
  MPU_Set_Accel_Fsr(0);					//���ٶȴ�����,��2g
  MPU_Set_Rate(50);						//���ò�����50Hz
  MPU_Write_Byte(MPU_INT_EN_REG,0X00);	//�ر������ж�
  MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//I2C��ģʽ�ر�
  MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);	//�ر�FIFO
  MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	//INT���ŵ͵�ƽ��Ч
  res=MPU_Read_Byte(MPU_DEVICE_ID_REG);
  if(res==MPU_ADDR)//����ID��ȷ
  {
    MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);	//����CLKSEL,PLL X��Ϊ�ο�
    MPU_Write_Byte(MPU_PWR_MGMT2_REG,0x07);	//ֻ�м��ٶȶ�����
    MPU_Set_Rate(50);						            //���ò�����Ϊ50Hz
    
  }else return 1;
  return 0;
}
//����MPU6050�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
  return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);//���������������̷�Χ  
}
//����MPU6050���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
  return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);//���ü��ٶȴ����������̷�Χ  
}
//����MPU6050�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_LPF(u16 lpf)
{
  u8 data=0;
  if(lpf>=188)data=1;
  else if(lpf>=98)data=2;
  else if(lpf>=42)data=3;
  else if(lpf>=20)data=4;
  else if(lpf>=10)data=5;
  else data=6; 
  return MPU_Write_Byte(MPU_CFG_REG,data);//�������ֵ�ͨ�˲���  
}
//����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Rate(u16 rate)
{
  u8 data;
  if(rate>1000)rate=1000;
  if(rate<4)rate=4;
  data=1000/rate-1;
  data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
  return MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}

//�õ��¶�ֵ
//����ֵ:�¶�ֵ(������100��)
short MPU_Get_Temperature(void)
{
  u8 buf[2]; 
  short raw;
  float temp;
  MPU_Read_Len(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
  raw=((u16)buf[0]<<8)|buf[1];  
  temp=36.53+((double)raw)/340;  
  return temp*100;
}
//�õ�������ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
  u8 buf[6],res;  
  res=MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
  if(res==0)
  {
    *gx=((u16)buf[0]<<8)|buf[1];  
    *gy=((u16)buf[2]<<8)|buf[3];  
    *gz=((u16)buf[4]<<8)|buf[5];
  } 	
  return res;;
}
//�õ����ٶ�ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
  u8 buf[6],res;  
  res=MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
  if(res==0)
  {
    *ax=((u16)buf[0]<<8)|buf[1];  
    *ay=((u16)buf[2]<<8)|buf[3];  
    *az=((u16)buf[4]<<8)|buf[5];
  } 	
  return res;;
}
//IIC����д
//addr:������ַ 
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
//����ֵ:0,����
//    ����,�������
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
  u8 i; 
  IIC_Start(); 
  IIC_WriteByte((addr<<1)|0);//����������ַ+д����	
  IIC_WriteByte(reg);	//д�Ĵ�����ַ
  for(i=0;i<len;i++)
  {
    IIC_WriteByte(buf[i]);	//��������	
  }    
  IIC_Stop();	 
  return 0;	
} 
//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//    ����,�������
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
  IIC_Start(); 
  IIC_WriteByte((addr<<1)|0);//����������ַ+д����	
  IIC_WriteByte(reg);	//д�Ĵ�����ַ
  IIC_Start();
  IIC_WriteByte((addr<<1)|1);//����������ַ+������	
  while(len)
  {
    //if(len==1)*buf=IIC_ReadByte(0);//������,����nACK 
    //else *buf=IIC_ReadByte(1);		//������,����ACK  
    if(len==1)*buf=IIC_ReadByte(1);//������,����nACK 
    else *buf=IIC_ReadByte(0);		//������,����ACK  
    
    len--;
    buf++; 
  }    
  IIC_Stop();	//����һ��ֹͣ���� 
  return 0;	
}
//IICдһ���ֽ� 
//reg:�Ĵ�����ַ
//data:����
//����ֵ:0,����
//    ����,�������
u8 MPU_Write_Byte(u8 reg,u8 data) 				 
{ 
  IIC_Start(); 
  IIC_WriteByte((MPU_ADDR<<1)|0);//����������ַ+д����	
  IIC_WriteByte(reg);	//д�Ĵ�����ַ
  IIC_WriteByte(data);//�������� 
  IIC_Stop();	 
  return 0;
}
//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
u8 MPU_Read_Byte(u8 reg)
{
  u8 res;
  IIC_Start(); 
  IIC_WriteByte((MPU_ADDR<<1)|0);//����������ַ+д����	
  IIC_WriteByte(reg);	//д�Ĵ�����ַ
  IIC_Start();
  IIC_WriteByte((MPU_ADDR<<1)|1);//����������ַ+������	
  res=IIC_ReadByte(1);//��ȡ����,����nACK 
  IIC_Stop();			//����һ��ֹͣ���� 
  return res;		
}

//�ҳ����ĸ�����Ϊ�ǲ��ı�׼�ᣨ����·ʱ��̧��ͷ��£���ֱ�ڵ����ϵļ��ٶ�ֵ�仯���
unsigned char DataSelect(SensorData *GMeter)
{
    DATATYPE tempX = 0, tempY = 0, tempZ = 0, tempMax;
    unsigned char flag = 0;
    tempX = GMeter->X.Max - GMeter->X.Min;
    tempY = GMeter->Y.Max - GMeter->Y.Min;
    tempZ = GMeter->Z.Max - GMeter->Z.Min;
    if(tempX > tempY) {
        if(tempX > tempZ) {
            flag = 1;
            tempMax = tempX;
        } else {
            flag = 3;
            tempMax = tempZ;
        }
    } else {
        if(tempY > tempZ) {
            flag = 2;
            tempMax = tempY;
        } else {
            flag = 3;
            tempMax = tempZ;
        }
    }
    if(tempMax > 1000) {
        return flag;
    } else {
        return 0;
    }
}

/*********************************************************************************************************
** Function name:           main
** Descriptions:            �Ʋ�
**                          ���󣺼�¼����·���ܲ�ʱ�Ĳ������������ݴ��ͨ�����ں�����ģ�鷢�ͳ�ȥ
** input parameters:        none
** output parameters:       none
** Returned value:          none
** Created by:              smallmount
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
*********************************************************************************************************/
u8 CountStep(signed short X,signed short Y,signed short Z)
{
	unsigned char i=0;
 /*********************************************************///��� X Y Z�����ϵļ��ٶ�ֵ                            
//   X = X / 4;                                                 //Ϊʲô����4������
//   Y = Y / 4;
//   Z = Z / 4;
   if(m == DATASIZE)   //����⵽50�κ�
   {
      m = 0;
      if(m_count == 0) {
         m_count = 1;
      }
    }
    if(n == 10) n = 0; //��������n��  ��10�κ󣬴��±���
    DateBufferX[n] = X;
    DateBufferY[n] = Y;
    DateBufferZ[n] = Z;
    if(InitFlag < 9) 
	{
         GMeter.X.Data[m] = X;
         GMeter.Y.Data[m] = Y;
         GMeter.Z.Data[m] = Z;
         InitFlag++;
     } 
	 else 
	 {
         GMeter.X.Data[m] = (DateBufferX[0] + DateBufferX[1] + DateBufferX[2] + DateBufferX[3] + DateBufferX[4] + DateBufferX[5] + DateBufferX[6] + DateBufferX[7] + DateBufferX[8] + DateBufferX[9]) / 10;
         GMeter.Y.Data[m] = (DateBufferY[0] + DateBufferY[1] + DateBufferY[2] + DateBufferY[3] + DateBufferY[4] + DateBufferY[5] + DateBufferY[6] + DateBufferY[7] + DateBufferY[8] + DateBufferY[9]) / 10;
         GMeter.Z.Data[m] = (DateBufferZ[0] + DateBufferZ[1] + DateBufferZ[2] + DateBufferZ[3] + DateBufferZ[4] + DateBufferZ[5] + DateBufferZ[6] + DateBufferZ[7] + DateBufferZ[8] + DateBufferZ[9]) / 10;
     }
	 if(m_count == 1)
	 {
            if(GMeter.X.MaxMark == m || GMeter.X.MinMark == m || GMeter.Y.MaxMark == m || GMeter.Y.MinMark == m || GMeter.Z.MaxMark == m || GMeter.Z.MinMark == m) 
			{
                unsigned char tempXMaxMark = GMeter.X.MaxMark;
                unsigned char tempXMinMark = GMeter.X.MinMark;
                unsigned char tempYMaxMark = GMeter.Y.MaxMark;
                unsigned char tempYMinMark = GMeter.Y.MinMark;
                unsigned char tempZMaxMark = GMeter.Z.MaxMark;
                unsigned char tempZMinMark = GMeter.Z.MinMark;
                if(GMeter.X.MaxMark == m) {
                    GMeter.X.Max = -8192;
                }
                if(GMeter.X.MinMark == m) {
                    GMeter.X.Min = +8192;
                }
                if(GMeter.Y.MaxMark == m) {
                    GMeter.Y.Max = -8192;
                }
                if(GMeter.Y.MinMark == m) {
                    GMeter.Y.Min = +8192;
                }
                if(GMeter.Z.MaxMark == m) {
                    GMeter.Z.Max = -8192;
                }
                if(GMeter.Z.MinMark == m) {
                    GMeter.Z.Min = +8192;
                }
				//��¼���ɼ��������е����ֵ������Сֵ��Ҳ���Ǽ�¼������߲���
                for(i = 0; i < DATASIZE; i++) {
                    if(GMeter.X.MaxMark == m) {
                        if(GMeter.X.Data[i] >= GMeter.X.Max) {
                            GMeter.X.Max = GMeter.X.Data[i];
                            tempXMaxMark = i;
                        }
                    }
                    if(GMeter.X.MinMark == m) {
                        if(GMeter.X.Data[i] <= GMeter.X.Min) {
                            GMeter.X.Min = GMeter.X.Data[i];
                            tempXMinMark = i;
                        }
                    }
                    if(GMeter.Y.MaxMark == m) {
                        if(GMeter.Y.Data[i] >= GMeter.Y.Max) {
                            GMeter.Y.Max = GMeter.Y.Data[i];
                            tempYMaxMark = i;
                        }
                    }
                    if(GMeter.Y.MinMark == m) {
                        if(GMeter.Y.Data[i] <= GMeter.Y.Min) {
                            GMeter.Y.Min = GMeter.Y.Data[i];
                            tempYMinMark = i;
                        }
                    }
                    if(GMeter.Z.MaxMark == m) {
                        if(GMeter.Z.Data[i] >= GMeter.Z.Max) {
                            GMeter.Z.Max = GMeter.Z.Data[i];
                            tempZMaxMark = i;
                        }
                    }
                    if(GMeter.Z.MinMark == m) {
                        if(GMeter.Z.Data[i] <= GMeter.Z.Min) {
                            GMeter.Z.Min = GMeter.Z.Data[i];
                            tempZMinMark = i;
                        }
                    }
                }
                GMeter.X.MaxMark = tempXMaxMark;//�洢������߲��ȶ�Ӧ���±�
                GMeter.X.MinMark = tempXMinMark;
                GMeter.Y.MaxMark = tempYMaxMark;
                GMeter.Y.MinMark = tempYMinMark;
                GMeter.Z.MaxMark = tempZMaxMark;
                GMeter.Z.MinMark = tempZMinMark;
            }
     }
     if(GMeter.X.Data[m] >= GMeter.X.Max)//Ѱ�Ҳ�����߲���
	 {
            GMeter.X.Max = GMeter.X.Data[m];
            GMeter.X.MaxMark = m;
     }
     if(GMeter.X.Data[m] <= GMeter.X.Min) 
	 {
            GMeter.X.Min = GMeter.X.Data[m];
            GMeter.X.MaxMark = m;
     }
     if(GMeter.Y.Data[m] >= GMeter.Y.Max)
	 {
            GMeter.Y.Max = GMeter.Y.Data[m];
            GMeter.Y.MaxMark = m;
     }
     if(GMeter.Y.Data[m] <= GMeter.Y.Min) 
	 {
            GMeter.Y.Min = GMeter.Y.Data[m];
            GMeter.Y.MinMark = m;
     }
     if(GMeter.Z.Data[m] >= GMeter.Z.Max) 
	 {
            GMeter.Z.Max = GMeter.Z.Data[m];
            GMeter.Z.MaxMark = m;
     }
     if(GMeter.Z.Data[m] <= GMeter.Z.Min) 
	 {
            GMeter.Z.Min = GMeter.Z.Data[m];
            GMeter.Z.MinMark = m;
     }
	//��������߲��ȵ�ƽ��ֵ��Ϊ�Ƚϵ���ֵ
	GMeter.X.Base      = (GMeter.X.Max + GMeter.X.Min) / 2;
	GMeter.Y.Base      = (GMeter.Y.Max + GMeter.Y.Min) / 2;
	GMeter.Z.Base      = (GMeter.Z.Max + GMeter.Z.Min) / 2;
	GMeter.X.UpLimit   = (GMeter.X.Base + GMeter.X.Max * 2) / 3;
	GMeter.Y.UpLimit   = (GMeter.Y.Base + GMeter.Y.Max * 2) / 3;
	GMeter.Z.UpLimit   = (GMeter.Z.Base + GMeter.Z.Max * 2) / 3;
	GMeter.X.DownLimit = (GMeter.X.Base + GMeter.X.Min * 2) / 3;
	GMeter.Y.DownLimit = (GMeter.Y.Base + GMeter.Y.Min * 2) / 3;
	GMeter.Z.DownLimit = (GMeter.Z.Base + GMeter.Z.Min * 2) / 3;
	GMeterAmax = DataSelect(&GMeter);
	switch(GMeterAmax)//�ж����ĸ���Ϊ��׼���мƲ�
	{
		case 1:
		if((GMeter.X.Data[m] > GMeter.X.UpLimit) && StepFlag == 0) StepFlag = 1;
		if((GMeter.X.Data[m] < GMeter.X.DownLimit) && StepFlag ==1) 
		{
			StepFlag = 0;
			StepCount++;
		}
		break;
	case 2:
		if((GMeter.Y.Data[m] > GMeter.Y.UpLimit) && StepFlag == 0) {
			StepFlag = 1;
		}
		if((GMeter.Y.Data[m] < GMeter.Y.DownLimit) && StepFlag ==1) {
			StepFlag = 0;
			StepCount++;
		}
		break;
	case 3:
		if((GMeter.Z.Data[m] > GMeter.Z.UpLimit) && StepFlag == 0) {
			StepFlag = 1;
		}
		if((GMeter.Z.Data[m] < GMeter.Z.DownLimit) && StepFlag ==1) {
			StepFlag = 0;
			StepCount++;
		}
		break;
	default:  break;
	}
	m++;
	n++;
		return StepCount;
}
