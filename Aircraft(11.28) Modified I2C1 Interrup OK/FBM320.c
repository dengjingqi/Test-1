#include <stm32f0xx.h>
#include "System.h"
#include "FBM320.h"
#include "I2CInterrup.h"

void FBM320_Init(void)
{
	unsigned char tmpreg_DXRA = 0xB6;
	I2C_Read(FBM320_ADDR,FBM320_PARTID,&tmpreg_DXRA,1);
	I2C_Write(FBM320_ADDR,FBM320_SOFTRST,&tmpreg_DXRA,1);
	SYS_DelayUs(5000);
}

BOOL Read_FBM320_24BitAD(unsigned long *Pressuretmpreg,unsigned long *Temptmpreg)
{
	unsigned char FBM320_Status;
	unsigned char CalibrateBuffer[20];
	unsigned char tmpreg_DXRA = 0xB6,tmpreg_DXRB = 0,tmpreg_DXRC = 0;
	unsigned char Type_Pressure=0x2E;
	I2C_Read(FBM320_ADDR,FBM320_CONFIG,&tmpreg_DXRA,1);
	I2C_Write(FBM320_ADDR,FBM320_CONFIG,&Type_Pressure,1);
	FBM320_Status = I2C_Read(FBM320_ADDR,FBM320_CALLCOEFF1,&CalibrateBuffer[0],1);
	FBM320_Status = I2C_Read(FBM320_ADDR,FBM320_CALLCOEFF2,&CalibrateBuffer[1],1);
	FBM320_Status = I2C_Read(FBM320_ADDR,FBM320_CALLCOEFF3,&CalibrateBuffer[2],18);
	SYS_DelayUs(3000);
	FBM320_Status = I2C_Read(FBM320_ADDR,FBM320_DATA_LSB,&tmpreg_DXRA,1);
	FBM320_Status = I2C_Read(FBM320_ADDR,FBM320_DATA_CSB,&tmpreg_DXRB,1);    
	FBM320_Status = I2C_Read(FBM320_ADDR,FBM320_DATA_MSB,&tmpreg_DXRC,1); 
	*Temptmpreg=tmpreg_DXRC<<16 | tmpreg_DXRB<<8 | tmpreg_DXRA;
	Type_Pressure=0xF4;
	I2C_Write(FBM320_ADDR,FBM320_CONFIG,&Type_Pressure,1);
	SYS_DelayUs(10000);
	FBM320_Status = I2C_Read(FBM320_ADDR,FBM320_DATA_LSB,&tmpreg_DXRA,1);
	FBM320_Status = I2C_Read(FBM320_ADDR,FBM320_DATA_CSB,&tmpreg_DXRB,1);    
	FBM320_Status = I2C_Read(FBM320_ADDR,FBM320_DATA_MSB,&tmpreg_DXRC,1); 
	*Pressuretmpreg=tmpreg_DXRC<<16 | tmpreg_DXRB<<8 | tmpreg_DXRA;
	return FBM320_Status;
}



