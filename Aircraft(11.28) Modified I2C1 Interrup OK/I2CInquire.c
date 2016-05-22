#include "Config.h"
#include <stm32f0xx.h>
#include <String.h>
#include "System.h"
#include "I2CInquire.h"

BOOL I2C_Initialized;
void I2C_Init(void)
{
	// Set PB6 and 7 to their respective I2C1 output
	GPIOB->AFR[0]=(GPIOB->AFR[0]&0x00FFFFFF) | 0x11000000;
	// Set PB6 and 7 to alternate function
	GPIOB->MODER=(GPIOB->MODER&0xFFFF0FFF) | 0x0000A000;
	
	GPIOB->PUPDR   &= 0xFFFF0FFF;
	GPIOB->PUPDR   |= 0x00005000;
	GPIOB->OSPEEDR &= 0xFFFF0FFF;
	GPIOB->OSPEEDR |= 0x0000F000;
	// Enable the I2C module clock
	RCC->APB1ENR|=RCC_APB1ENR_I2C1EN;
	// Reset I2C1
	RCC->APB1RSTR|=RCC_APB1RSTR_I2C1RST;
	RCC->APB1RSTR&=~RCC_APB1RSTR_I2C1RST;
	// Set I2C clock to SYSCLK
	RCC->CFGR3|=RCC_CFGR3_I2C1SW;
	// Disable the I2C peripheral
	I2C1->CR1&=~I2C_CR1_PE;
	// PE=0 Peripheral disabled
	// TXIE=0 TX interrupt disabled
	// RXIE=0 RX interrupt disabled
	// ADDRIE=0 Address match interrupt disabled
	// NACKIE=0 Not acknowledge received interrupt disabled
	// STOPIE=0 STOP detection interrupt disabled
	// TCIE=0 Transfer complete interrupt disabled
	// ERRIE=0 Error interrupt disabled
	// DNF=4 Digital filter set to 2*tI2CCLK
	// ANFOFF=1 Analog noise filter OFF
	// TXDMAEN=0 TX DMA disabled
	// RXDMAEN=0 RX DMA disabled
	// SBC=0 Slave byte control disabled
	// NOSTRETCH=0 Clock stretching enabled
	// WUPEN=0 Wakeup from stop disabled
	// GCEN=0 General call disabled
	// SMBHEN=0 SMBus host address disabled
	// SMBDEN=0 SMBus device default address disabled
	// ALERTEN=0 SMBus alert disabled
	// PECEN=0 PEC calculation disabled
	I2C1->CR1=(2<<8) | I2C_CR1_ANFOFF;
	// SCLL=I2C_SCK_LOW_TSCK-1
	// SCLH=I2C_SCK_HIGH_TSCK-1
	// SDADEL=I2C_SCL_DEL_TSCK
	// SCLDEL=I2C_SDA_DEL_TSCK-1
	// PRESC=0 Prescaler disabled
	I2C1->TIMINGR=((I2C_SCK_LOW_TSCK-1)<<0) | ((I2C_SCK_HIGH_TSCK-1)<<8) | 
		(I2C_SCL_DEL_TSCK<<16) | ((I2C_SDA_DEL_TSCK-1)<<20) | (0<<28);
	// Enable the I2C peripheral
	I2C1->CR1|=I2C_CR1_PE;
	I2C_Initialized=TRUE;
	return;
}

BOOL I2C_IsNack(void)
{
	if (!(I2C1->ISR&I2C_ISR_NACKF))
		return FALSE;
	I2C1->ICR=I2C_ICR_NACKCF;
	while (!(I2C1->ISR&I2C_ISR_STOPF));
	I2C1->ICR=I2C_ICR_STOPCF;
	return TRUE;
}

BOOL I2C_Read(unsigned char Address,unsigned char RegAddress,const void *pData,unsigned long DataLength)
{
	if(!I2C_Transfer(TRUE,Address,&RegAddress,1,FALSE))
		return FALSE;
	return I2C_Transfer(FALSE,Address,pData,DataLength,TRUE);
}

BOOL I2C_Transfer(BOOL IsWrite,unsigned char Address,const void *pData,unsigned long DataLength,BOOL SendStop)
{
	BOOL FirstBlock;
	
	FirstBlock=TRUE;
	do
	{
		unsigned long BlockLength;
		unsigned long BlockLength2;
		
		BlockLength=DataLength;
		if (BlockLength>0xFF)
			BlockLength=0xFF;
		// SADDR=0 Not used
		// SADD=Address slave address
		// RD_WRN=0 write transfer
		// ADD10=0 7-bit addressing mode
		// HEAD10R=0 Not used
		// START=1 Generate a start
		// STOP=0 Do not gerenate a stop
		// NACK=0 Not used
		// NBYTES=DataLength Data length
		// RELOAD=0 DataLength<256
		// AUTOEND=SendStop
		// PECBYTE=0 No PEC transfer
		if (DataLength<=0xFF)
			I2C1->CR2=(Address<<1) | ((IsWrite)?0:I2C_CR2_RD_WRN) | I2C_CR2_START | (BlockLength<<16) | ((SendStop)?I2C_CR2_AUTOEND:0);
		else
			I2C1->CR2=(Address<<1) | ((IsWrite)?0:I2C_CR2_RD_WRN) | ((FirstBlock)?I2C_CR2_START:0) | (BlockLength<<16) | I2C_CR2_RELOAD;
		BlockLength2=BlockLength;
		while (BlockLength2)
		{
			// Wait for space in transmit register and detect a NACK condition
			do
			{
				if (I2C_IsNack())
					return FALSE;
			} while ((IsWrite && !(I2C1->ISR&I2C_ISR_TXIS)) ||
				(!IsWrite && !(I2C1->ISR&I2C_ISR_RXNE)));
			if (IsWrite)
				I2C1->TXDR=*(unsigned char *)pData;
			else
				*(unsigned char *)pData=I2C1->RXDR;
			pData=(unsigned char *)pData+1;
			BlockLength2--;
		}
		// Wait for the end of the transmission and detect a NACK condition
		if (DataLength<=0xFF)
		{
			do
			{
				if (I2C_IsNack())
					return FALSE;
			} while ((SendStop)?!(I2C1->ISR&I2C_ISR_STOPF):!(I2C1->ISR&I2C_ISR_TC));
		}
		else
		{
			do
			{
				if (I2C_IsNack())
					return FALSE;
			} while (!(I2C1->ISR&I2C_ISR_TCR));
		}
		FirstBlock=FALSE;
		DataLength-=BlockLength;
	} while (DataLength);
	if (I2C_IsNack())
		return FALSE;
	if (SendStop)
		I2C1->ICR=I2C_ICR_STOPCF;
	return TRUE;
}

BOOL I2C_Write(unsigned char Address,unsigned char RegAddress,const void *pData,unsigned long DataLength)
{
	unsigned char Buffer[2];
	Buffer[0]=RegAddress;
	memcpy(Buffer+1,pData,DataLength);
	return I2C_Transfer(TRUE,Address,Buffer,DataLength+1,TRUE);
}
