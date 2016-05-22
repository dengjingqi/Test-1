/*=======================================
*  Date    :           2014-05-27   
*  Author  :           longweiguo
*  FileName:         BMP180.c
*  MDK Versions:   uVision4 V4.73.0.0
*  Brief   :    
I2C2 Hardware config
PA9 = I2C2_SCL =30 = AF4
PA10 = I2C2_SDA =31 = AF4
PB6 = I2C1_SCL =42 = AF4
PB7 = I2C1_SDA =43 = AF4
STM32F302x = Master mode
RCC_I2CCLK:  I2C clock source = System clock =72M.
I2CCLK = 400K;
=========================================*/
#include "BMP180.h"
#include "stm32f30x_I2C_Init.h"
#include "stm32f30x.h"
#include "Config.h"
#include <math.h>
ErrorStatus BMP180_Status = ERROR ;
short AC1,AC2,AC3,B1,B2,MB,MC,MD;
unsigned short AC4,AC5,AC6;
long UT,UP;
unsigned char OSS = 0;
float Pressure,Temperature;


//Read Only Calibration Data From The E2PROM of The BMP180
//Start read out E2PROM registers, 16 bit, MSB first
/*====================================================
Pressure range: 300 ... 1100hPa (+9000m ... -500m relating to sea level)
The E2PROM has stored 176 bit of individual calibration data.  
UP = pressure data (16 to 19 bit)
UT = temperature data (16 bit)
=====================================================*/
ErrorStatus BMP180_Init(unsigned int *tmpreg)
{
	unsigned char tmpreg_DXRA = 0,tmpreg_DXRB = 0;	
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_AC1M,&tmpreg_DXRA,1);     //Read AC1
	if(ERROR == BMP180_Status) return BMP180_Status;
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_AC1L,&tmpreg_DXRB,1);
	if(ERROR == BMP180_Status) return BMP180_Status;
	if(SUCCESS == BMP180_Status) 
	{
		tmpreg[0] = tmpreg_DXRA;
		tmpreg[0] = (tmpreg[0]<<8) | tmpreg_DXRB;
		AC1 = tmpreg[0]; 
	}	
	
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_AC2M,&tmpreg_DXRA,1);   //Read AC2
	if(ERROR == BMP180_Status) return BMP180_Status;
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_AC2L,&tmpreg_DXRB,1);
	if(ERROR == BMP180_Status) return BMP180_Status;
	if(SUCCESS == BMP180_Status) 
	{
		tmpreg[1] = tmpreg_DXRA;
		tmpreg[1] = (tmpreg[1]<<8) | tmpreg_DXRB;
		AC2 = tmpreg[1]; 
	}
	
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_AC3M,&tmpreg_DXRA,1);   //Read AC3
	if(ERROR == BMP180_Status) return BMP180_Status;
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_AC3L,&tmpreg_DXRB,1);
	if(SUCCESS == BMP180_Status) 
	{
		tmpreg[2] = tmpreg_DXRA;
		tmpreg[2] = (tmpreg[2]<<8) | tmpreg_DXRB;
		AC3 = tmpreg[2]; 
	}
	
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_AC4M,&tmpreg_DXRA,1);   //Read AC4
	if(ERROR == BMP180_Status) return BMP180_Status;
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_AC4L,&tmpreg_DXRB,1);
	if(ERROR == BMP180_Status) return BMP180_Status;
	if(SUCCESS == BMP180_Status) 
	{
		tmpreg[3] = tmpreg_DXRA;
		tmpreg[3] = (tmpreg[3]<<8) | tmpreg_DXRB;
		AC4 = tmpreg[3]; 
	}
	
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_AC5M,&tmpreg_DXRA,1);    //Read AC5
	if(ERROR == BMP180_Status) return BMP180_Status;
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_AC5L,&tmpreg_DXRB,1);
	if(ERROR == BMP180_Status) return BMP180_Status;
	if(SUCCESS == BMP180_Status) 
	{
		tmpreg[4] = tmpreg_DXRA;
		tmpreg[4] = (tmpreg[4]<<8) | tmpreg_DXRB;
		AC5 = tmpreg[4]; 
	}
	
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_AC6M,&tmpreg_DXRA,1);    //Read AC6
	if(ERROR == BMP180_Status) return BMP180_Status;
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_AC6L,&tmpreg_DXRB,1);
	if(SUCCESS == BMP180_Status) 
	{
		tmpreg[5] = tmpreg_DXRA;
		tmpreg[5] = (tmpreg[5]<<8) | tmpreg_DXRB;
		AC6 = tmpreg[5]; 
	}
	
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_B1M,&tmpreg_DXRA,1);    //Read B1
	if(ERROR == BMP180_Status) return BMP180_Status;
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_B1L,&tmpreg_DXRB,1);
	if(SUCCESS == BMP180_Status) 
	{
		tmpreg[6] = tmpreg_DXRA;
		tmpreg[6] = (tmpreg[6]<<8) | tmpreg_DXRB;
		B1 = tmpreg[6]; 
	}
	
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_B2M,&tmpreg_DXRA,1);    //Read B2
	if(ERROR == BMP180_Status) return BMP180_Status;
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_B2L,&tmpreg_DXRB,1);
	if(SUCCESS == BMP180_Status) 
	{
		tmpreg[7] = tmpreg_DXRA;
		tmpreg[7] = (tmpreg[7]<<8) | tmpreg_DXRB;
		B2 = tmpreg[7]; 
	}
	
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_MBM,&tmpreg_DXRA,1);    //Read MB
	if(ERROR == BMP180_Status) return BMP180_Status;
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_MBL,&tmpreg_DXRB,1);
	if(SUCCESS == BMP180_Status) 
	{
		tmpreg[8] = tmpreg_DXRA;
		tmpreg[8] = (tmpreg[8]<<8) | tmpreg_DXRB;
		MB = tmpreg[8]; 
	}
	
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_MCM,&tmpreg_DXRA,1);    //Read MC
	if(ERROR == BMP180_Status) return BMP180_Status;
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_MCL,&tmpreg_DXRB,1);
	if(SUCCESS == BMP180_Status) 
	{
		tmpreg[9] = tmpreg_DXRA;
		tmpreg[9] = (tmpreg[9]<<8) | tmpreg_DXRB;
		MC = tmpreg[9];
	}
	
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_MDM,&tmpreg_DXRA,1);    //Read MD
	if(ERROR == BMP180_Status) return BMP180_Status;
	BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_MDL,&tmpreg_DXRB,1);
	if(SUCCESS == BMP180_Status) 
	{
		tmpreg[10] = tmpreg_DXRA;
		tmpreg[10] = (tmpreg[10]<<8) | tmpreg_DXRB;
		MD = tmpreg[10];
	}
	return BMP180_Status;
}


//Read Pressure & Temperature Value
//retval:ERROR = 0,SUCCESS = 1;tmpreg = Pressure & Temperature
ErrorStatus Read_UT_UP_Value(long *tmpreg,unsigned char UT_UP)
{
	    unsigned char tmpreg_ADCM = 0,tmpreg_ADCL = 0,tmpreg_ADCXL = 0;
	    unsigned char Con_time = 5;
	    //Mode,oversampling_setting,Internal number of samples,Conversion time,
	    //Avg. current,RMS noise[hPa],RMS noise[m]
	    OSS = UT_UP & 0xc0;           //Hardware pressure sampling accuracy modes 
	
	    if(OSS == BMP180_OSS1) {OSS = 1;Con_time = 75;}
		else if(OSS == BMP180_OSS2) {OSS = 2;Con_time = 135;}
        else if(OSS == BMP180_OSS3) {OSS = 3;Con_time = 255;}
	    BMP180_Status = I2C2_Write(BMP180_Addr,BMP180_Control_R,&UT_UP,1);    //Wait 4.5ms
	    if(ERROR == BMP180_Status) return BMP180_Status;
	
	    delay_ms(Con_time);
	    BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_ADCM,&tmpreg_ADCM,1);
		if(ERROR == BMP180_Status) return BMP180_Status;
		BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_ADCL,&tmpreg_ADCL,1);
		if(SUCCESS == BMP180_Status) 
		{
			*tmpreg = tmpreg_ADCM;
			*tmpreg = (*tmpreg<<8) | tmpreg_ADCL;
			 if(UT_UP == 0x2e)UT = *tmpreg;
			 else UP = *tmpreg;
		}
        if(OSS != 0)
       	{
			BMP180_Status = I2C2_Read(BMP180_Addr,BMP180_ADCXL,&tmpreg_ADCXL,1);
			if(ERROR == BMP180_Status) return BMP180_Status;
			if(SUCCESS == BMP180_Status) 
			{
				*tmpreg = (*tmpreg<<8) | tmpreg_ADCXL;
				*tmpreg = (*tmpreg>>(8-OSS));
				 UP = *tmpreg;
			}
		}		
	    return BMP180_Status;
}

void Calculate_UT_Value(float *tempreg)
{
	long X1,X2;
	X1 = ((UT - AC6) * AC5)>>15;
	X2 = (MC << 11)/(X1 + MD);
	*tempreg = (X1+X2+8)>>4;
	Temperature = *tempreg/10;   // Resolution of output data  Temperature = 0.1
} 

void Calculate_UP_Value(float *tempreg)
{
	long X1,X2,X3,B3,B6,p;
	unsigned long B4,B7;
	X1 = ((UT - AC6) * AC5)>>15;
	X2 = (MC << 11)/(X1 + MD);
	
	B6 = X1 + X2 - 4000;
	
	X1 = (B2 * (B6 * B6>>12)) >> 11;
	X2 = AC2 * B6 >> 11;
	X3 = X1+ X2;
	
	B3 = (((AC1*4 + X3) << OSS) + 2)/4;
	
	X1 = AC3 * B6 >>13;
	X2 = (B1 *(B6 * B6 >> 12)) >> 16;
	X3 = (X1 + X2 + 2)/4;
	
	B4 = AC4 *(unsigned long)(X3 + 32768) >> 15;
	B7 = ((unsigned long)UP - B3) * (50000 >> OSS);
	
	if(B7<0x80000000) 
	{ 
		p = (B7 *2)/B4;
	}
	else 
      {
	     p = B7/B4 * 2;
	  }
	
	X1 = (p >> 8) * (p >> 8);
	X1 = (X1 *3038) >> 16;
	
	X2 = (-7357*p) >> 16;
	*tempreg = p + ((X1 + X2 + 3791) >> 4);
    Pressure = *tempreg/100;   // Resolution of output data  pressure = 0.01
}

void Calculate_Altitude_Value(float *tempreg)
{
	float p;
	p = (double)Pressure / 1013.25;
	p = pow(p,0.19);
	*tempreg = 44330 * (1 - p);
}

