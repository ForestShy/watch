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
//初始化MPU6050
//返回值:0,成功
//    其他,错误代码
u8 MPU_Init(void)
{ 
  u8 res;
  //IIC_init();//初始化IIC总线
  MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//复位MPU6050
  Delayms(100);  //Jahol Fan ：奇怪的问题，延时改长了，就不能兼容HP-6
  MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);	//唤醒MPU6050 
  //MPU_Set_Gyro_Fsr(3);					//陀螺仪传感器,±2000dps
  MPU_Set_Accel_Fsr(0);					//加速度传感器,±2g
  MPU_Set_Rate(50);						//设置采样率50Hz
  MPU_Write_Byte(MPU_INT_EN_REG,0X00);	//关闭所有中断
  MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//I2C主模式关闭
  MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);	//关闭FIFO
  MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	//INT引脚低电平有效
  res=MPU_Read_Byte(MPU_DEVICE_ID_REG);
  if(res==MPU_ADDR)//器件ID正确
  {
    MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);	//设置CLKSEL,PLL X轴为参考
    MPU_Write_Byte(MPU_PWR_MGMT2_REG,0x07);	//只有加速度都工作
    MPU_Set_Rate(50);						            //设置采样率为50Hz
    
  }else return 1;
  return 0;
}
//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
  return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围  
}
//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
  return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围  
}
//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_LPF(u16 lpf)
{
  u8 data=0;
  if(lpf>=188)data=1;
  else if(lpf>=98)data=2;
  else if(lpf>=42)data=3;
  else if(lpf>=20)data=4;
  else if(lpf>=10)data=5;
  else data=6; 
  return MPU_Write_Byte(MPU_CFG_REG,data);//设置数字低通滤波器  
}
//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Rate(u16 rate)
{
  u8 data;
  if(rate>1000)rate=1000;
  if(rate<4)rate=4;
  data=1000/rate-1;
  data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
  return MPU_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}

//得到温度值
//返回值:温度值(扩大了100倍)
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
//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
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
//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
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
//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
  u8 i; 
  IIC_Start(); 
  IIC_WriteByte((addr<<1)|0);//发送器件地址+写命令	
  IIC_WriteByte(reg);	//写寄存器地址
  for(i=0;i<len;i++)
  {
    IIC_WriteByte(buf[i]);	//发送数据	
  }    
  IIC_Stop();	 
  return 0;	
} 
//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
  IIC_Start(); 
  IIC_WriteByte((addr<<1)|0);//发送器件地址+写命令	
  IIC_WriteByte(reg);	//写寄存器地址
  IIC_Start();
  IIC_WriteByte((addr<<1)|1);//发送器件地址+读命令	
  while(len)
  {
    //if(len==1)*buf=IIC_ReadByte(0);//读数据,发送nACK 
    //else *buf=IIC_ReadByte(1);		//读数据,发送ACK  
    if(len==1)*buf=IIC_ReadByte(1);//读数据,发送nACK 
    else *buf=IIC_ReadByte(0);		//读数据,发送ACK  
    
    len--;
    buf++; 
  }    
  IIC_Stop();	//产生一个停止条件 
  return 0;	
}
//IIC写一个字节 
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Byte(u8 reg,u8 data) 				 
{ 
  IIC_Start(); 
  IIC_WriteByte((MPU_ADDR<<1)|0);//发送器件地址+写命令	
  IIC_WriteByte(reg);	//写寄存器地址
  IIC_WriteByte(data);//发送数据 
  IIC_Stop();	 
  return 0;
}
//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
u8 MPU_Read_Byte(u8 reg)
{
  u8 res;
  IIC_Start(); 
  IIC_WriteByte((MPU_ADDR<<1)|0);//发送器件地址+写命令	
  IIC_WriteByte(reg);	//写寄存器地址
  IIC_Start();
  IIC_WriteByte((MPU_ADDR<<1)|1);//发送器件地址+读命令	
  res=IIC_ReadByte(1);//读取数据,发送nACK 
  IIC_Stop();			//产生一个停止条件 
  return res;		
}

//找出以哪个方向为记步的标准轴（人走路时，抬起和放下，垂直于地面上的加速度值变化最大）
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
** Descriptions:            计步
**                          现象：记录人走路或跑步时的步数，并将数据打包通过串口和无线模块发送出去
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
 /*********************************************************///求出 X Y Z方向上的加速度值                            
//   X = X / 4;                                                 //为什么除于4？？？
//   Y = Y / 4;
//   Z = Z / 4;
   if(m == DATASIZE)   //当检测到50次后？
   {
      m = 0;
      if(m_count == 0) {
         m_count = 1;
      }
    }
    if(n == 10) n = 0; //保存数据n次  当10次后，从新保存
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
				//记录所采集的数据中的最大值或者最小值，也就是记录波峰或者波谷
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
                GMeter.X.MaxMark = tempXMaxMark;//存储波峰或者波谷对应的下标
                GMeter.X.MinMark = tempXMinMark;
                GMeter.Y.MaxMark = tempYMaxMark;
                GMeter.Y.MinMark = tempYMinMark;
                GMeter.Z.MaxMark = tempZMaxMark;
                GMeter.Z.MinMark = tempZMinMark;
            }
     }
     if(GMeter.X.Data[m] >= GMeter.X.Max)//寻找波峰或者波谷
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
	//将波峰或者波谷的平均值作为比较的阈值
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
	switch(GMeterAmax)//判断以哪个轴为基准进行计步
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
