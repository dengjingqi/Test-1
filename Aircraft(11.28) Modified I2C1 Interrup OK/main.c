#include <stm32f0xx.h>
#include "System.h"
#include "I2CInterrup.h"
#include "Main.h"
#include "ADC.h"
#include "PWM.h"
#include "MPU6050.h"
#include "RF_XN297.h"
#include "SPI.h"
#include "FBM320.h"

unsigned short GYRO[3],ACCEL[3];
unsigned char Status;
unsigned char MPU6050_Address;
unsigned long FBM320ADValue;
unsigned long FBMTemp;
int main(void)
{
	SYS_Init();
	SYS_DelayUs(5000); // Allow the power supply to stabilize
	LED_Config();
	ADC_Init();
	I2C_Init();
	Read_MPU6050_ADDR(&MPU6050_Address);  //MPU6050_Address=0x68 or 0x72
	MPU6050_Init();
	MPU6050_Config();
//	FBM320_Init();
	SPI_Init();
	XN297_RegInit();
	while(1)
	{
//		Status=Read_FBM320_24BitAD(&FBM320ADValue,&FBMTemp);
//		SYS_DelayUs(10000);
		Read_MPU6050_ACCEL(ACCEL);
		Read_MPU6050_GYRO(GYRO);
	}
}


void LED_Config(void)
{	
	//Set PB1 4BitLED
	GPIOB->MODER=(GPIOB->MODER&~(3<<(1*2))) | (1<<(1*2));    
	GPIOB->BSRR=1<<( 0+1);
	GPIOB->BSRR=1<<(16+1);
	//Set PB2 take photo
	GPIOB->MODER=(GPIOB->MODER&~(3<<(2*2))) | (1<<(2*2));
	GPIOB->BSRR=1<<( 0+2);
	GPIOB->BSRR=1<<(16+2);
	//Set PF0 LED Headless
	GPIOF->MODER |=  (1<<(0*2));    //GPIO Output
	GPIOF->OSPEEDR|= (3UL<<(0*2));  //Speed
	GPIOF->BSRR=1UL<<(16+0);        //Output 0
	GPIOF->BSRR=1UL<<(0+0);        //Output 0
	//PWM1-PWM4 is PA8-PA11
	//PB8
	GPIOB->MODER=(GPIOB->MODER&~(3<<(8*2))) | (1<<(8*2));    
	GPIOB->BSRR=1<<( 0+8);
	GPIOB->BSRR=1<<(16+8);
}
	
