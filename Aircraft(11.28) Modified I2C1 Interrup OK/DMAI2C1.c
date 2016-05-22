#include <stm32f0xx.h>
#include "System.h"
#include "DMAI2C.h"

#define SPEED_10K   0xB042C3C7
#define SPEED_100K  0xB0420F13
#define SPEED_400K  0x50330309
//#define I2C_SPEED   SPEED_400K

#define I2C_Reload_Mode                I2C_CR2_RELOAD
#define I2C_AutoEnd_Mode               I2C_CR2_AUTOEND
#define I2C_SoftEnd_Mode               ((uint32_t)0x00000000)

#define I2C_No_StartStop                 ((uint32_t)0x00000000)
#define I2C_Generate_Stop                I2C_CR2_STOP
#define I2C_Generate_Start_Read          (uint32_t)(I2C_CR2_START | I2C_CR2_RD_WRN)
#define I2C_Generate_Start_Write         I2C_CR2_START


void (*IIC_Handle)(void);

void IIC_Init(void)//(void (*Tranfer_Handle)(void))
{
	// Set PB6 and 7 to their respective I2C1 output
	GPIOB->AFR[0]=(GPIOB->AFR[0]&0x00FFFFFF) | 0x11000000;
	// Set PB6 and 7 to alternate function
	GPIOB->MODER=(GPIOB->MODER&0xFFFF0FFF) | 0x0000A000;
	GPIOB->PUPDR   &= 0xFFFF0FFF;
	GPIOB->PUPDR   |= 0x00005000;
	// Enable the I2C module clock
	RCC->APB1ENR|=RCC_APB1ENR_I2C1EN;
	// Reset I2C1
	RCC->APB1RSTR|=RCC_APB1RSTR_I2C1RST;
	RCC->APB1RSTR&=~RCC_APB1RSTR_I2C1RST;
	// Set I2C clock to SYSCLK
	RCC->CFGR3|=RCC_CFGR3_I2C1SW;
	//IIC_Handle=Tranfer_Handle;
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
	return;
}

BOOL I2C_WaitISRFlag(unsigned long ISRBit, unsigned char Status)
{
	unsigned long I2C1_ISR_FLAG;
	unsigned long TimeOut;
	TimeOut = 50000;
	do
	{
		if (Status)
			I2C1_ISR_FLAG = I2C1->ISR & ISRBit;			/* Wait until I2C1_ISR_FLAG is set */
		else
			I2C1_ISR_FLAG = ~(I2C1->ISR & ISRBit);	/* Wait until I2C1_ISR_FLAG is cleared */		
		if (!(TimeOut--))
			return FALSE;
	} while(!I2C1_ISR_FLAG);
	return TRUE;
}	

void I2C_TransferHandling(unsigned short Address, unsigned char Number_Bytes, unsigned long ReloadEndMode, unsigned long StartStopMode)
{
	uint32_t tmpreg = 0;
  /* Get the CR2 register value */
  tmpreg = I2C1->CR2; 
  /* clear tmpreg specific bits */
  tmpreg &= (unsigned long)~((unsigned long)(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP)); 
  /* update tmpreg */
  tmpreg |= (unsigned long)(((unsigned long)Address & I2C_CR2_SADD) | (((uint32_t)Number_Bytes << 16 ) & I2C_CR2_NBYTES) | \
            (unsigned long)ReloadEndMode | (uint32_t)StartStopMode); 
  /* update CR2 register */
  I2C1->CR2 = tmpreg;  	
	return;
}

BOOL I2C_Write(unsigned char ChipID, unsigned char WriteAddr, unsigned char *WriteBuffer, unsigned long NumByteToWrite)
{
	unsigned long DataNum=0;
	/* Configure slave address, nbytes, reload and generate start */
  I2C_TransferHandling(ChipID, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);
	/* Wait until TXIS flag is set */
	if (!I2C_WaitISRFlag(I2C_ISR_TXE, 1))
		return FALSE;
	/* Send memory address */
	I2C1->TXDR = WriteAddr;
	/* Wait until TCR flag is set */
	if (!I2C_WaitISRFlag(I2C_ISR_TCR, 1))
		return FALSE;
	I2C_TransferHandling(ChipID, NumByteToWrite, I2C_AutoEnd_Mode, I2C_No_StartStop);
	while (DataNum != NumByteToWrite)
  {      
    /* Wait until TXIS flag is set */
    if (!I2C_WaitISRFlag(I2C_ISR_TXE, 1))
			return FALSE;   
    /* Write data to TXDR */
    I2C1->TXDR = WriteBuffer[DataNum];  
    /* Update number of transmitted data */
    DataNum++;   
  } 
	/* Wait until STOPF flag is set */
  if (!I2C_WaitISRFlag(I2C_ISR_STOPF, 1))
		return FALSE;
  /* Clear STOPF flag */
  I2C1->ISR |= I2C_ISR_STOPF;
	return TRUE;
}

BOOL I2C_Read(unsigned char ChipID, unsigned char ReadAddr, unsigned char *ReadBuffer, unsigned long NumByteToRead)
{
  /* Configure slave address, nbytes, reload and generate start */
  I2C_TransferHandling(ChipID, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	/* Wait until TXIS flag is set */
  if (!I2C_WaitISRFlag(I2C_ISR_TXE, 1))
		return FALSE;
	/* Send memory address */
  *(unsigned char *)&I2C1->TXDR = ReadAddr;
	/* Wait until TC flag is set */
  if (!I2C_WaitISRFlag(I2C_ISR_TC, 1))
		return FALSE;
	I2C_TransferHandling(ChipID, NumByteToRead, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	// Set up the TX DMA
	// Disable DMA
	DMA1_Channel5->CCR&=~DMA_CCR_EN;
	DMA1_Channel5->CPAR=(unsigned long)&I2C1->TXDR;
	DMA1_Channel5->CMAR=(unsigned long)ReadBuffer;
	DMA1_Channel5->CNDTR=NumByteToRead;
	// EN=1 DMA channel enabled
	// TCIE=0 Transfer complete interrupt disabled
	// HTIE=0 Half transfer interrupt disabled
	// TEIE=0 Transfer error interrupt disabled
	// DIR=1 Read from memory
	// CIRC=0 Circular mode disabled
	// PINC=0 Peripheral increment mode disabled
	// MINC=1 Memory increment mode enabled
	// PSIZE=0 8-bit peripheral size
	// MSIZE=0 8-bit memory size
	// PL=0 Low priority
	// MEM2MEM=0 Memory to memory mode disabled
	DMA1_Channel5->CCR=DMA_CCR_EN | DMA_CCR_DIR | DMA_CCR_MINC;
	// Disable the DMA, Set up the RX DMA
	DMA1_Channel4->CCR&=~DMA_CCR_EN;
	DMA1_Channel4->CNDTR = NumByteToRead;
	DMA1_Channel4->CPAR=(unsigned long)&I2C1->RXDR;
	DMA1_Channel4->CMAR=(unsigned long)ReadBuffer;
	// EN=1 DMA channel enabled
	// TCIE=0 Transfer complete interrupt disabled
	// HTIE=0 Half transfer interrupt disabled
	// TEIE=0 Transfer error interrupt disabled
	// DIR=1 Read from memory
	// CIRC=0 Circular mode disabled
	// PINC=0 Peripheral increment mode disabled
	// MINC=1 Memory increment mode enabled
	// PSIZE=0 8-bit peripheral size
	// MSIZE=0 8-bit memory size
	// PL=1 Midium priority
	// MEM2MEM=0 Memory to memory mode disabled
	DMA1_Channel4->CCR = DMA_CCR_EN | DMA_CCR_MINC | DMA_CCR_PL_0|DMA_CCR_TCIE;
	//Clean interrupt
//	NVIC_SetPriority(DMA1_Channel2_3_IRQn,IRQ_PRI_HIGH);
  I2C1->CR1 |= I2C_CR1_RXDMAEN;
	// Wait for the end of the transfer
	while (!(DMA1->ISR&DMA_ISR_TCIF4));
	DMA1->IFCR|=DMA_IFCR_CTCIF4;
	// Disable the TX DMA
	DMA1_Channel4->CCR=0;
	// Disable SPI RX and TX DMA
	I2C1->CR1 &=~ I2C_CR1_RXDMAEN;
	return TRUE;
}

void DMA1_Channel2_3_IRQHandler(void )
{
	//Transfer complete
	if((DMA1->ISR&DMA_ISR_TCIF3))     
	{
		//IIC_Handle();
		//Clean interrupt
		DMA1->IFCR |=DMA_IFCR_CTCIF3;
	}
}

