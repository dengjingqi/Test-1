/*=======================================
*  Date    :           2014-05-27   
*  Author  :           longweiguo
*  FileName:           MPU6050.c
*  MDK Versions:   uVision4 V4.73.0.0
*  Brief   :        
I2C1 Hardware config
PB8 = I2C1_SCL =32 = AF1
PB7 = I2C1_SDA =30 = AF1
STM32F031x = Master mode
RCC_I2CCLK:  I2C clock source = System clock =4.
I2CCLK = 400K;
=========================================*/
#include <stm32f0xx.h>
#include "System.h"
#include "I2CInterrup.h"
#include <math.h>
#include "MPU6050.h"

BOOL MPU6050_Status = FALSE ;
unsigned char Sample_Rate = 0;
float ACCEL_XOUT,ACCEL_YOUT,ACCEL_ZOUT,TEMP_OUT;
float GYRO_XOUT,GYRO_YOUT,GYRO_ZOUT;
/*============================================
These registers are used for gyroscope and accelerometer self-tests that permit the user to test the 
mechanical and electrical portions of the gyroscope and the accelerometer. 
tempreg[0]  = XG_TEST;   tempreg[1]  = YG_TEST
tempreg[2]  = ZG_TEST;   tempreg[4]  = XA_TEST
tempreg[5]  = YA_TEST;   tempreg[6]  = ZA_TEST
==============================================*/
BOOL Read_SELF_TEST(float *tempreg)
{
	unsigned char tmpreg_DXRA = 0;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,SELF_TEST_X,&tmpreg_DXRA,1);     //Read SELF_TEST_X
	if(SUCCESS == MPU6050_Status) 
	{
		tempreg[0] = tmpreg_DXRA & 0x1f;	
		tempreg[0] = Calculate_FTG(&tempreg[0]);
		tempreg[4] = (tmpreg_DXRA & 0xe0)>>3 ;			
	}
	
	MPU6050_Status = I2C_Read(MPU6050_ADDR,SELF_TEST_Y,&tmpreg_DXRA,1);     //Read SELF_TEST_Y
	if(SUCCESS == MPU6050_Status) 
	{
		tempreg[1] = tmpreg_DXRA & 0x1f;	
		tempreg[1] = Calculate_FTYG(&tempreg[1]);
		tempreg[5] = (tmpreg_DXRA & 0xe0)>>3 ;			
	}
	
	MPU6050_Status = I2C_Read(MPU6050_ADDR,SELF_TEST_Z,&tmpreg_DXRA,1);     //Read SELF_TEST_Z
	if(SUCCESS == MPU6050_Status) 
	{
		tempreg[2] = tmpreg_DXRA & 0x1f;
		tempreg[2] = Calculate_FTG(&tempreg[2]);		
		tempreg[6] = (tmpreg_DXRA & 0xe0)>>3 ;			
	}
	
	MPU6050_Status = I2C_Read(MPU6050_ADDR,SELF_TEST_A,&tmpreg_DXRA,1);     //Read SELF_TEST_A
	if(SUCCESS == MPU6050_Status) 
	{
		tempreg[3] = tmpreg_DXRA;		
		tempreg[4] = (unsigned char )tempreg[4] | (tmpreg_DXRA >>4);	
		tempreg[4] = Calculate_FTA(&tempreg[4]);
		tempreg[5] = (unsigned char)tempreg[5] | ((tmpreg_DXRA & 0x0c) >>2);	
		tempreg[5] = Calculate_FTA(&tempreg[5]);
		tempreg[6] = (unsigned char)tempreg[6] | (tmpreg_DXRA & 0x03) ;	
		tempreg[6] = Calculate_FTA(&tempreg[6]);
	}	
	return 	MPU6050_Status;
}

float Calculate_FTG(float *FTG)
{
	float p;
	p = pow(1.046,(*FTG-1));
	*FTG = 25 *131 * p;
	return *FTG;
}

float Calculate_FTYG(float *FTYG)
{
	float p;
	p = pow(1.046,(*FTYG-1));
	*FTYG = -25 *131 * p;
	return *FTYG;
}

float Calculate_FTA(float *FTA)
{
	float p,x1,x2;
	x1 = (*FTA - 1)/30;
	x2 = 0.92/0.34;
	p = pow(x2,x1);
	*FTA = 4069 *0.34 *p;
	return *FTA;
}

/*==========================================
This register specifies the divider from the gyroscope output 
rate used to generate the Sample Rate for the MPU-60X0. 
============================================*/
BOOL Read_Sample_Rate(unsigned char *tmpreg)
{
	unsigned char tmpreg_DXRA = 0;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,SMPLRT_DIV,&tmpreg_DXRA,1);     //Read Sample_Rate
	if(SUCCESS == MPU6050_Status) 
	{
		*tmpreg = tmpreg_DXRA;	
		Sample_Rate = tmpreg_DXRA;	
	}
	return MPU6050_Status;
}


/*=============================================
This register configures the external Frame Synchronization (FSYNC) pin sampling and the Digital 
Low Pass Filter (DLPF) setting for both the gyroscopes and accelerometers.
An external signal connected to the FSYNC pin can be sampled by configuring EXT_SYNC_SET. 
Signal changes to the FSYNC pin are latched so that short strobes may be captured. The latched 
FSYNC signal will be sampled at the Sampling Rate, as defined in register 25. After sampling, the 
latch will reset to the current FSYNC signal state.
The sampled value will be reported in place of the least significant bit in a sensor data register
determined by the value of EXT_SYNC_SET according to the following table.
==============================================*/
BOOL Read_Config(unsigned char *tmpreg)
{
	unsigned char tmpreg_DXRA = 0;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,CONFIG,&tmpreg_DXRA,1);    
	if(SUCCESS == MPU6050_Status) 
	{
		*tmpreg = tmpreg_DXRA;	
		Sample_Rate = tmpreg_DXRA;	
	}
	return MPU6050_Status;
}


BOOL Write_Config(unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,CONFIG,tmpreg,1);   
	 return MPU6050_Status;
}

/*=========================================================
This register is used to trigger gyroscope self-test and configure the gyroscopes’ full scale range.
Parameters:
XG_ST  Setting this bit causes the X axis gyroscope to perform self test.
YG_ST  Setting this bit causes the Y axis gyroscope to perform self test.
ZG_ST  Setting this bit causes the Z axis gyroscope to perform self test.
FS_SEL  2-bit unsigned value.  Selects the full scale range of gyroscopes. 
==========================================================*/
BOOL Write_GYRO_CONFIG(unsigned char *tmpreg)
{	 
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,GYRO_CONFIG,tmpreg,1);   
	 return MPU6050_Status;
}


/*========================================================
This register is used to trigger accelerometer self test and configure the accelerometer full scale 
range. This register also configures the Digital High Pass Filter (DHPF).
Parameters:
XA_ST  When set to 1, the X- Axis accelerometer performs self test.
YA_ST  When set to 1, the Y- Axis accelerometer performs self test.
ZA_ST  When set to 1, the Z- Axis accelerometer performs self test.
AFS_SEL  2-bit unsigned value. Selects the full scale range of accelerometers.
=========================================================*/
BOOL Write_ACCEL_CONFIG(unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,ACCEL_CONFIG,tmpreg,1);   
	 return MPU6050_Status;
}

/*==========================================================
This register configures the detection threshold for Motion interrupt generation. 
MOT_THR  8-bit unsigned value. Specifies the Motion detection threshold.
============================================================*/
BOOL Write_MOT_THR(unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,MOT_THR,tmpreg,1);    
	 return MPU6050_Status;
}

/*===============================================================
//his register determines which sensor measurements are loaded into the FIFO buffer.
When a sensor is FIFO_EN bit is enabled in this register, data from the sensor data registers will be 
loaded into the FIFO buffer. The sensors are sampled at the Sample Rate as defined in Register 25. 
For further information regarding sensor data registers, please refer to Registers 59 to 96 
=================================================================*/
BOOL Write_FIFO_EN(unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,FIFO_EN,tmpreg,1);    
	 return MPU6050_Status;
}


/*====================================================================
This register configures the auxiliary I2C bus for single-master or multi-master control. In addition, the 
register is used to delay the Data Ready interrupt, and also enables the writing of Slave 3 data into 
the FIFO buffer. The register also configures the auxiliary I2C Master is transition from one slave read 
to the next, as well as the MPU-60X0’s 8MHz internal clock.
=====================================================================*/
BOOL Write_MPU6050_MST_CTRL(unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,I2C_MST_CTRL,tmpreg,1);    
	 return MPU6050_Status;
}

/*======================================================================
I2C slave data transactions between the MPU-60X0 and Slave 0 are set as either read or write 
operations by the I2C_SLV0_RW bit. When this bit is 1, the transfer is a read operation. When the bit 
is 0, the transfer is a write operation. 
I2C_SLV0_ADDR is used to specify the I2C slave address of Slave 0.
Data transfer starts at an internal register within Slave 0. This address of this register is specified by
I2C_SLV0_REG. 
The number of bytes transferred is specified by  I2C_SLV0_LEN.   When more than 1 byte is 
transferred (I2C_SLV0_LEN > 1), data is read from (written to) sequential addresses starting from 
I2C_SLV0_REG.
=======================================================================*/
BOOL Write_MPU6050_SLAVE_ADDR(unsigned char *I2C,unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,*I2C,tmpreg,1);    
	 return MPU6050_Status;
}


BOOL Write_MPU6050_SLAVE_REG(unsigned char *I2C,unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,*I2C,tmpreg,1);    
	 return MPU6050_Status;
}

BOOL Write_MPU6050_SLAVE_CTRL(unsigned char *I2C,unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,*I2C,tmpreg,1);    
	 return MPU6050_Status;
}

BOOL MPU6050_Select_SLAVE(unsigned char *I2C,unsigned char *SLAVE_ADDR,unsigned char *SLAVE_REG,unsigned char *SLAVE_CTRL)
{
	*SLAVE_ADDR = I2C_SLAVE_W | I2C_SLAVE_ADDR;
	MPU6050_Status = Write_MPU6050_SLAVE_ADDR(I2C,SLAVE_ADDR);
	if(ERROR == MPU6050_Status) return MPU6050_Status;

	*SLAVE_REG = I2C_SLAVE_REG;
	MPU6050_Status = Write_MPU6050_SLAVE_REG(I2C,SLAVE_REG);
	if(ERROR == MPU6050_Status) return MPU6050_Status;

	*SLAVE_CTRL = 0;
	MPU6050_Status = Write_MPU6050_SLAVE_CTRL(I2C,0);
	return MPU6050_Status;
}

/*===============================================================
In read mode, the result of the read will be available in I2C_SLV4_DI. In write mode, the contents of 
I2C_SLV4_DO will be written into the slave device. 
=================================================================*/
BOOL Write_MPU6050_SLAVE_DO(unsigned char *I2C,unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,*I2C,tmpreg,1);    
	 return MPU6050_Status;
}

BOOL Read_MPU6050_SLAVE_DI(unsigned char *I2C,unsigned char *tmpreg)
{
	unsigned char tmpreg_DXRA = 0;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,*I2C,&tmpreg_DXRA,1);    
	if(SUCCESS == MPU6050_Status) 
	{
		*tmpreg = tmpreg_DXRA;	
	}
	return MPU6050_Status;
}


/*=================================================================
This register shows the status of the interrupt generating signals in the I2C Master 
within the MPU-60X0.  This register also communicates the status of the FSYNC interrupt to the host processor. 

Reading this register will clear all the status bits in the register. 
==================================================================*/
BOOL Read_MPU6050_MST_STATUS(unsigned char *tmpreg)
{
	unsigned char tmpreg_DXRA = 0;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,I2C_MST_STATUS,&tmpreg_DXRA,1);    
	if(SUCCESS == MPU6050_Status) 
	{
		*tmpreg = tmpreg_DXRA;	
	}
	return MPU6050_Status;
}

/*========================================================
This register configures the behavior of the interrupt signals at the INT pins. 
This register is also used to enable the FSYNC Pin to be used as an interrupt to 
the host application processor, as well 
as to enable Bypass Mode on the I2C Master. This bit also enables the clock output.
=========================================================*/
BOOL Write_MPU6050_INT_PIN_CFG(unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,INT_PIN_CFG,tmpreg,1);    
	 return MPU6050_Status;
}


/*=================================================
This register enables interrupt generation by interrupt sources.
For information regarding the interrupt status for each interrupt generation source, 
please refer to Register 58. Further information regarding I2C Master interrupt generation 
can be found in Register 54.
====================================================*/
BOOL Write_MPU6050_INT_ENABLE(unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,INT_ENABLE,tmpreg,1);    
	 return MPU6050_Status;
}


/*=====================================================
This register shows the interrupt status of each interrupt generation source. 
Each bit will clear after the register is read.
For information regarding the corresponding interrupt enable bits,
please refer to Register 56.For a list of I2C Master interrupts, please refer to Register 54.
=======================================================*/
BOOL Read_MPU6050_INT_STATUS(unsigned char *tmpreg)
{
	unsigned char tmpreg_DXRA = 0;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,INT_STATUS,&tmpreg_DXRA,1);    
	if(SUCCESS == MPU6050_Status) 
	{
		*tmpreg = tmpreg_DXRA;	
	}
	return MPU6050_Status;
}


/*===============================================
These registers store the most recent accelerometer measurements. 
Accelerometer measurements are written to these registers at the Sample Rate as
defined in Register 25. 
=================================================*/
BOOL Read_MPU6050_ACCEL(unsigned short *tmpreg)
{
	unsigned char tmpreg_DXRA = 0,tmpreg_DXRB = 0;	
	MPU6050_Status = I2C_Read(MPU6050_ADDR,ACCEL_XOUT_H,&tmpreg_DXRA,1);     //Read AC1
	if(ERROR == MPU6050_Status) return MPU6050_Status;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,ACCEL_XOUT_L,&tmpreg_DXRB,1);
	if(ERROR == MPU6050_Status) return MPU6050_Status;
	if(SUCCESS == MPU6050_Status) 
	{
		tmpreg[0] = tmpreg_DXRA;
		tmpreg[0] = (tmpreg[0]<<8) | tmpreg_DXRB;
		ACCEL_XOUT = tmpreg[0]; 
	}	
	
	MPU6050_Status = I2C_Read(MPU6050_ADDR,ACCEL_YOUT_H,&tmpreg_DXRA,1);   //Read AC2
	if(ERROR == MPU6050_Status) return MPU6050_Status;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,ACCEL_YOUT_L,&tmpreg_DXRB,1);
	if(ERROR == MPU6050_Status) return MPU6050_Status;
	if(SUCCESS == MPU6050_Status) 
	{
		tmpreg[1] = tmpreg_DXRA;
		tmpreg[1] = (tmpreg[1]<<8) | tmpreg_DXRB;
		ACCEL_YOUT = tmpreg[1]; 
	}
	
	MPU6050_Status = I2C_Read(MPU6050_ADDR,ACCEL_ZOUT_H,&tmpreg_DXRA,1);   //Read AC3
	if(ERROR == MPU6050_Status) return MPU6050_Status;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,ACCEL_ZOUT_L,&tmpreg_DXRB,1);
	if(SUCCESS == MPU6050_Status) 
	{
		tmpreg[2] = tmpreg_DXRA;
		tmpreg[2] = (tmpreg[2]<<8) | tmpreg_DXRB;
		ACCEL_ZOUT = tmpreg[2]; 
	}
	return MPU6050_Status;
}


/*===============================================================
These registers store the most recent temperature sensor measurement. 
Temperature measurements are written to these registers at the Sample Rate as 
defined in Register 25. 
=================================================================*/
BOOL Read_MPU6050_TEMP(unsigned short *tmpreg)
{
	unsigned char tmpreg_DXRA = 0,tmpreg_DXRB = 0;	
	MPU6050_Status = I2C_Read(MPU6050_ADDR,TEMP_OUT_H,&tmpreg_DXRA,1);     //Read AC1
	if(ERROR == MPU6050_Status) return MPU6050_Status;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,TEMP_OUT_L,&tmpreg_DXRB,1);
	if(ERROR == MPU6050_Status) return MPU6050_Status;
	if(SUCCESS == MPU6050_Status) 
	{
		tmpreg[0] = tmpreg_DXRA;
		tmpreg[0] = (tmpreg[0]<<8) | tmpreg_DXRB;
		TEMP_OUT = (double)tmpreg[0]/340 + 36.53; 
	}	
	return MPU6050_Status;
}


//============Read_MPU6050_GYRO============
BOOL Read_MPU6050_GYRO(unsigned short *tmpreg)
{
	unsigned char tmpreg_DXRA = 0,tmpreg_DXRB = 0;	
	MPU6050_Status = I2C_Read(MPU6050_ADDR,GYRO_XOUT_H,&tmpreg_DXRA,1);     //Read AC1
	if(ERROR == MPU6050_Status) return MPU6050_Status;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,GYRO_XOUT_L,&tmpreg_DXRB,1);
	if(ERROR == MPU6050_Status) return MPU6050_Status;
	if(SUCCESS == MPU6050_Status) 
	{
		tmpreg[0] = tmpreg_DXRA;
		tmpreg[0] = (tmpreg[0]<<8) | tmpreg_DXRB;
		GYRO_XOUT = tmpreg[0]; 
	}	
	
	MPU6050_Status = I2C_Read(MPU6050_ADDR,GYRO_YOUT_H,&tmpreg_DXRA,1);   //Read AC2
	if(ERROR == MPU6050_Status) return MPU6050_Status;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,GYRO_YOUT_L,&tmpreg_DXRB,1);
	if(ERROR == MPU6050_Status) return MPU6050_Status;
	if(SUCCESS == MPU6050_Status) 
	{
		tmpreg[1] = tmpreg_DXRA;
		tmpreg[1] = (tmpreg[1]<<8) | tmpreg_DXRB;
		GYRO_YOUT = tmpreg[1]; 
	}
	
	MPU6050_Status = I2C_Read(MPU6050_ADDR,GYRO_ZOUT_H,&tmpreg_DXRA,1);   //Read AC3
	if(ERROR == MPU6050_Status) return MPU6050_Status;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,GYRO_ZOUT_L,&tmpreg_DXRB,1);
	if(SUCCESS == MPU6050_Status) 
	{
		tmpreg[2] = tmpreg_DXRA;
		tmpreg[2] = (tmpreg[2]<<8) | tmpreg_DXRB;
		GYRO_ZOUT = tmpreg[2]; 
	}
	return MPU6050_Status;
}


/*================================================
This register is used to specify the timing of external sensor data shadowing. The register is also 
used to decrease the access rate of slave devices relative to the Sample Rate.
When DELAY_ES_SHADOW is set to 1, shadowing of external sensor data is delayed until all data 
has been received.
=================================================*/
BOOL Write_MPU6050_MST_DELAY_CTRL(unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,I2C_MST_DELAY_CTRL,tmpreg,1);    
	 return MPU6050_Status;
}


/*===================================================
This register is used to reset the analog and digital signal paths of the gyroscope, 
accelerometer, and temperature sensors. 
The reset will revert the  signal path analog to digital converters and filters to their power up 
configurations.
=====================================================*/
BOOL Write_SIGNAL_PATH_RESET(unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,SIGNAL_PATH_RESET,tmpreg,1);    
	 return MPU6050_Status;
}


/*======================================================
This register is used to add delay to the accelerometer power on time. It is also used to configure the 
Motion detection decrement rate.
ACCEL_ON_DELAY  2-bit unsigned value.  Specifies the additional  power-on delay  applied  
to accelerometer data path modules. 
Unit of 1 LSB = 1 ms.
========================================================*/
BOOL Write_MOT_DETECT_CTRL(unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,MOT_DETECT_CTRL,tmpreg,1);    
	 return MPU6050_Status;
}


/*=======================================================
This register allows the user to enable and disable the FIFO buffer, I2C Master Mode, 
and primary I2C interface. The FIFO buffer, I2C Master, 
sensor signal paths and sensor registers can also be reset using this register.
=========================================================*/
BOOL Write_USER_CTRL(unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,USER_CTRL,tmpreg,1);    
	 return MPU6050_Status;
}

/*==============================================
This register allows the user to configure the power mode and clock source. It also provides a bit for 
resetting the entire device, and a bit for disabling the temperature sensor.
===============================================*/
BOOL Write_PWR_MGMT1(unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,PWR_MGMT1,tmpreg,1);    
	 return MPU6050_Status;
}


/*================================================
This register allows the user to configure the frequency of wake-ups in Accelerometer Only Low 
Power Mode. This register also allows the user to put individual axes of the accelerometer and 
gyroscope into standby mode. 
The MPU-60X0 can be put into Accelerometer Only Low Power Mode using the following steps: 
(i)  Set CYCLE bit to 1
(ii)  Set SLEEP bit to 0
(iii)  Set TEMP_DIS bit to 1
(iv)  Set STBY_XG, STBY_YG, STBY_ZG bits to 
=================================================*/
BOOL Write_PWR_MGMT2(unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,PWR_MGMT2,tmpreg,1);    
	 return MPU6050_Status;
}


/*=================================================
These registers keep track of the number of samples currently in the FIFO buffer. 
These registers shadow the FIFO Count value. Both registers are loaded with the current sample 
count when FIFO_COUNT_H (Register 72) is read.
==================================================*/
BOOL Read_MPU6050_FIFO_COUNT(unsigned short *tmpreg)
{
	unsigned char tmpreg_DXRA = 0,tmpreg_DXRB = 0;	
	MPU6050_Status = I2C_Read(MPU6050_ADDR,FIFO_COUNTH,&tmpreg_DXRA,1);     //Read AC1
	if(ERROR == MPU6050_Status) return MPU6050_Status;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,FIFO_COUNTL,&tmpreg_DXRB,1);
	if(ERROR == MPU6050_Status) return MPU6050_Status;
	if(SUCCESS == MPU6050_Status) 
	{
		tmpreg[0] = tmpreg_DXRA;
		tmpreg[0] = (tmpreg[0]<<8) | tmpreg_DXRB;
		TEMP_OUT = tmpreg[0]; 
	}	
	return MPU6050_Status;
}

/*=============================================
This register is used to read and write data from the FIFO buffer. 
Data is written to the FIFO in order of register number (from lowest to highest). If all the FIFO enable 
flags (see below) are enabled and all External Sensor Data registers (Registers 73 to 96) are 
associated with a Slave device, the contents of registers 59 through 96 will be written in order at the 
Sample Rate.
===============================================*/
BOOL Read_MPU6050_FIFO(unsigned char *tmpreg)
{
	unsigned char tmpreg_DXRA = 0;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,FIOF_R_W,&tmpreg_DXRA,1);    
	if(SUCCESS == MPU6050_Status) 
	{
		*tmpreg = tmpreg_DXRA;	
	}
	return MPU6050_Status;
}


BOOL Write_MPU6050_FIFO(unsigned char *tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,FIOF_R_W,tmpreg,1);    
	 return MPU6050_Status;
}


/*==================================================
This register is used to verify the identity of the device. The contents of WHO_AM_I are the upper 6 
bits of the MPU-60X0’s 7-bit I2C address. The least significant bit of the MPU-60X0 is I2C address is 
determined by the value of the AD0 pin. The value of the AD0 pin is not reflected in this register. 
The default value of the register is 0x68. 
===================================================*/
BOOL Read_MPU6050_ADDR(unsigned char *tmpreg)
{
	unsigned char tmpreg_DXRA = 0;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,WHO_AM_I,&tmpreg_DXRA,1);    
	if(SUCCESS == MPU6050_Status) 
	{
		*tmpreg = tmpreg_DXRA;	
	}
	return MPU6050_Status;
}



BOOL Read_MPU6050_Reg(unsigned char Reg_Addr,unsigned char *tmpreg)
{
	unsigned char tmpreg_DXRA = 0;
	MPU6050_Status = I2C_Read(MPU6050_ADDR,Reg_Addr,&tmpreg_DXRA,1);    
	if(SUCCESS == MPU6050_Status) 
	{
		*tmpreg = tmpreg_DXRA;	
	}
	return MPU6050_Status;
}


BOOL Write_MPU6050_Reg(unsigned char Reg_Addr,unsigned char tmpreg)
{
	 MPU6050_Status = I2C_Write(MPU6050_ADDR,Reg_Addr,&tmpreg,1);    
	 return MPU6050_Status;
}

void MPU6050_Init(void)
{
	unsigned char MPU6050_Address;
	Read_MPU6050_ADDR(&MPU6050_Address);
	Write_MPU6050_Reg(PWR_MGMT1,0x00); 
	Write_MPU6050_Reg(CONFIG,0x00);   //FSYNC Pin Input disable,Accel = 1kHZ Bandwidth = 260HZ,Delay(0),Gyro = 8KHZ,Bandwidth = 256HZ,Delay(0.98ms)
	Write_MPU6050_Reg(SMPLRT_DIV,0x00);   //Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV) = 1K / 1, accelerometer,FIFO,DMP,sensor registers,Gyro = 8KHZ, 
	Write_MPU6050_Reg(GYRO_CONFIG,0xE0);   // 250
	Write_MPU6050_Reg(ACCEL_CONFIG,0xE0);  //2g
	Write_MPU6050_Reg(FIFO_EN,0xf8);       //TEMP_FIFO_EN,XG_FIFO_EN,YG_FIFO_EN,ZG_FIFO_EN,ACCEL_FIFO_EN
	Write_MPU6050_Reg(USER_CTRL,USER_CTRL_FIFO_EN);  
}

void MPU6050_Config(void)
{
	Write_MPU6050_Reg(PWR_MGMT1,0x80); 
	SYS_DelayUs(5000);
	Write_MPU6050_Reg(PWR_MGMT1,0x03); 
	SYS_DelayUs(5000);
	Write_MPU6050_Reg(CONFIG,0x01);  //FSYNC Pin Input disable,Accel = 1kHZ Bandwidth = 184HZ,Delay(2ms),Gyro = 1KHZ,Bandwidth = 188HZ,Delay(1.9ms)
	SYS_DelayUs(5000);
	Write_MPU6050_Reg(SMPLRT_DIV,0x01);  //Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
	SYS_DelayUs(5000);
	Write_MPU6050_Reg(GYRO_CONFIG,0x18);  //1000
	SYS_DelayUs(5000);
	Write_MPU6050_Reg(ACCEL_CONFIG,0x18); //16g
	SYS_DelayUs(5000);
	Write_MPU6050_Reg(INT_PIN_CFG,0x30); //16g
	SYS_DelayUs(5000);
	Write_MPU6050_Reg(INT_ENABLE,0x01); //16g
	SYS_DelayUs(5000);
	Write_MPU6050_Reg(FIFO_EN,0xf8);   //TEMP_FIFO_EN,XG_FIFO_EN,YG_FIFO_EN,ZG_FIFO_EN,ACCEL_FIFO_EN	
	SYS_DelayUs(5000);
	Write_MPU6050_Reg(USER_CTRL,USER_CTRL_FIFO_EN);  
	SYS_DelayUs(5000);
}

