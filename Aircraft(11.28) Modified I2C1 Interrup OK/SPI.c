#include "Config.h"
#include <stm32f0xx.h>
#include "System.h"
#include "SPI.h"

void SPI_Deselect(void)
{
	GPIOA->BSRR=1<<15;
	return;
}

void SPI_FlushRXBuffer(void)
{
	while (SPI1->SR&SPI_SR_RXNE)
		SPI1->DR;
	// Clear the overrun flag
	SPI1->SR;
	return;
}

void SPI_Init(void)
{
	// Set PA15 to high speed GPIO output (SPI CS)
	GPIOA->MODER=(GPIOA->MODER&~(3UL<<(15*2))) | (1<<(15*2));
	GPIOA->OSPEEDR=(GPIOA->OSPEEDR&~(3UL<<(15*2))) | (3UL<<(15*2));
	// Set PB3 to alternate SPI SCK mode
	GPIOB->MODER=(GPIOB->MODER&~(3<<(3*2))) | (2<<(3*2));
	GPIOB->AFR[0]=(GPIOB->AFR[0]&~(0xF<<(3*4))) | (0<<(3*4));
	// Set PB5 to alternate SPI MOSI mode
	GPIOB->MODER=(GPIOB->MODER&~(3<<(5*2))) | (2<<(5*2));
	GPIOB->AFR[0]=(GPIOB->AFR[0]&~(0xF<<(5*4))) | (0<<(5*4));
	// Set PB4 to alternate SPI MISO mode
	GPIOB->MODER=(GPIOB->MODER&~(3<<(4*2))) | (2<<(4*2));
	GPIOB->AFR[0]=(GPIOB->AFR[0]&~(0xF<<(4*4))) | (0<<(4*4));
	// Enable the SPI module clock
	RCC->APB2ENR|=RCC_APB2ENR_SPI1EN;
	// Reset SPI1
	RCC->APB2RSTR|=RCC_APB2RSTR_SPI1RST;
	RCC->APB2RSTR&=~RCC_APB2RSTR_SPI1RST;
	// CPHA=0 Clock phase
	// CPOL=0 Clock polarity
	// MSTR=1 Master mode
	// BR=2 SPI clock=SYSCLK/8=6MHz
	// SPE=0 SPI disabled
	// LSBFIRST=0 MSB first
	// SSI=0 Not used
	// SSM=0 Software slave management disabled
	// RXONLY=0 Full duplex
	// CRCL=0 Not used
	// CRCNEXT=0 Not used
	// CRCEN=0 CRC calculation disabled
	// BIDIOE=0 Not used
	// BIDIMODE=0 2-line unidirectional data mode selected
	SPI1->CR1=SPI_CR1_MSTR | SPI_CR1_BR_1;
	// RXDMAEN=0 RX DMA disabled
	// TXDMAEN=0 TX DMA disabled
	// SSOE=1 SS output enabled
	// NNSP=0 No NSS pulse
	// FRF=0 SPI Motorola mode
	// ERRIE=0 Error interrupt masked
	// RXNEIE=0 RXNE interrupt masked
	// TXEIE=0 TXE interrupt masked
	// DS=7 Data length is 8 bits
	// FRXTH=1 RXNE event on a 8 bits data
	// LDMA_RX=0 Not used
	// LDMA_TX=0 Not used
	SPI1->CR2=SPI_CR2_SSOE | SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_FRXTH;
	// Enable SPI
	SPI1->CR1|=SPI_CR1_SPE;
	SPI_Deselect();
	return;
}

void SPI_ReadRegister(unsigned char RegisterNb,void *pData,unsigned long DataLength)
{
	// CS=0
	SPI_Select();
	// Transmit the register number
	*(unsigned char *)&SPI1->DR=RegisterNb;
	// Wait for the end of the transmittion
	while (SPI1->SR&SPI_SR_BSY);
	// Receive the register data
	SPI_FlushRXBuffer();
	// Set up the TX DMA
	// Disable DMA
	DMA1_Channel3->CCR&=~DMA_CCR_EN;
	DMA1_Channel3->CPAR=(unsigned long)&SPI1->DR;
	DMA1_Channel3->CMAR=(unsigned long)pData;
	DMA1_Channel3->CNDTR=DataLength;
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
	DMA1_Channel3->CCR=DMA_CCR_EN | DMA_CCR_DIR | DMA_CCR_MINC;
	// Set up the RX DMA
	// Disable DMA
	DMA1_Channel2->CCR&=~DMA_CCR_EN;
	DMA1_Channel2->CPAR=(unsigned long)&SPI1->DR;
	DMA1_Channel2->CMAR=(unsigned long)pData;
	DMA1_Channel2->CNDTR=DataLength;
	// EN=1 DMA channel enabled
	// TCIE=0 Transfer complete interrupt disabled
	// HTIE=0 Half transfer interrupt disabled
	// TEIE=0 Transfer error interrupt disabled
	// DIR=0 Read from peripheral
	// CIRC=0 Circular mode disabled
	// PINC=0 Peripheral increment mode disabled
	// MINC=1 Memory increment mode enabled
	// PSIZE=0 8-bit peripheral size
	// MSIZE=0 8-bit memory size
	// PL=0 Low priority
	// MEM2MEM=0 Memory to memory mode disabled
	DMA1_Channel2->CCR=DMA_CCR_EN | DMA_CCR_MINC;
	// Enable SPI RX and TX DMA
	SPI1->CR2|=SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN;
	// Wait for the end of the transfer
	while (!(DMA1->ISR&DMA_ISR_TCIF2));
	DMA1->IFCR|=DMA_IFCR_CTCIF2;
	while (SPI1->SR&SPI_SR_BSY);
	// Disable the RX DMA
	DMA1_Channel2->CCR=0;
	// Disable the TX DMA
	DMA1_Channel3->CCR=0;
	// Disable SPI RX and TX DMA
	SPI1->CR2&=~(SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);
	// CS=1
	SPI_Deselect();
	return;
}

void SPI_Select(void)
{
	GPIOA->BSRR=1UL<<(16+15);
	return;
}

// Set CS=0, send a data packet, wait for the end of the packet and Set CS=1
// The previous SPI access must be finished and CS must be high
void SPI_TransmitOnly(const void *pData,unsigned long DataLength)
{
	// CS=0
	SPI_Select();
	// Set up the TX DMA
	DMA1_Channel3->CPAR=(unsigned long)&SPI1->DR;
	DMA1_Channel3->CMAR=(unsigned long)pData;
	DMA1_Channel3->CNDTR=DataLength;
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
	DMA1_Channel3->CCR=DMA_CCR_EN | DMA_CCR_DIR | DMA_CCR_MINC;
	// Enable SPI TX DMA
	SPI1->CR2|=SPI_CR2_TXDMAEN;
	// Wait for the end of the transfer
	while (!(DMA1->ISR&DMA_ISR_TCIF3));
	DMA1->IFCR|=DMA_IFCR_CTCIF3;
	while (SPI1->SR&SPI_SR_BSY);
	// Disable the DMA
	DMA1_Channel3->CCR=0;
	// Disable SPI TX DMA
	SPI1->CR2&=~SPI_CR2_TXDMAEN;
	// CS=1
	SPI_Deselect();
	return;
}
