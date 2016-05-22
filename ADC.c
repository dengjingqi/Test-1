#include "Config.h"
#include <stm32f0xx.h>
#include "System.h"
#include "ADC.h"

BOOL ADC_Initialized;
unsigned long ADC_Samples[ADC_NB_SAMPLES];
unsigned long ADC_NextSampleNb;
void ADC_MsIRQHandler(void)
{
	if (!ADC_Initialized)
		return;
	// Save the last conversion result
	ADC_Samples[ADC_NextSampleNb]=ADC1->DR;
	ADC_NextSampleNb++;
	if (ADC_NextSampleNb>=ADC_NB_SAMPLES)
		ADC_NextSampleNb=0;
	// Start the next conversion
	ADC1->CR|=ADC_CR_ADSTART;
	return;
}

// Return the ADC value converted to 0.01V unit
unsigned short ADC_GetInternalVoltage(void)
{
	unsigned long SampleNb;
	unsigned long Value=0;
	
	for (SampleNb=0;SampleNb<ADC_NB_SAMPLES;SampleNb++)
		Value+=ADC_Samples[SampleNb];
	Value=(Value+(ADC_NB_SAMPLES/2))/ADC_NB_SAMPLES;
	
	Value=Value*77/195;
	return Value=Value*SYS_FactoryConfig.PowerVoltageCorrection/10000;
}

void ADC_Init(void)
{
	ADC_NextSampleNb=0;
	// Set PA1 to analog mode
	GPIOA->MODER=(GPIOA->MODER&~(3<<(1*2))) | (3<<(1*2));
	// Enable the ADC module clock
	RCC->APB2ENR|=RCC_APB2ENR_ADC1EN;
	// Reset ADC1
	RCC->APB2RSTR|=RCC_APB2RSTR_ADC1RST;
	RCC->APB2RSTR&=~RCC_APB2RSTR_ADC1RST;
	// Disable the ADC if previously enabled
	if (ADC1->CR&ADC_CR_ADEN)
	{
		// Ongoing conversion?
		if (ADC1->CR&ADC_CR_ADSTART)
		{
			ADC1->CR|=ADC_CR_ADSTP;
			while (ADC1->CR&ADC_CR_ADSTP);
		}
		// Disable the ADC
		ADC1->CR|=ADC_CR_ADDIS;
		while (ADC1->CR&ADC_CR_ADEN);
	}
	// Calibrate the ADC
	ADC1->CR |= ADC_CR_ADCAL;
	while (ADC1->CR&ADC_CR_ADCAL);
		ADC1->CR|=ADC_CR_ADEN;
	// Enable the ADC
	do
	{
		ADC1->CR|=ADC_CR_ADEN;
		if (!(ADC1->CR&ADC_CR_ADEN))
			ADC1->CR;
	} while (!(ADC1->CR&ADC_CR_ADEN));
	while (!(ADC1->ISR&ADC_ISR_ADRDY));
	// Set PCLK/4 as ADC clock source (12MHz)
	ADC1->CFGR2=ADC_CFGR2_CKMODE_1;
	// Set the ADC channel 1 as source
	ADC1->CHSELR|=(1<<1);
	// Set the sample time to 28.5 cycles
	ADC1->SMPR|=ADC_SMPR1_SMPR_0 | ADC_SMPR1_SMPR_1; 
	// Set conversion mode
	// DMAEN=0 DMA disabled
	// DMACFG=0 Not used
	// SCANDIR=0 Upward scan
	// RES=0 12 bits resolution
	// ALIGN=0 Right alignment
	// EXTSEL=0 Not used
	// EXTEN=0 Hardware trigger detection disabled
	// OVRMOD=0 Old data is preserved in case of overrun
	// CONT=0 Single conversion mode
	// WAIT=0
	// AUTOFF=0
	// DISCEN=0 Discontinuous mode disabled
	// AWDSGL=0 Not used
	// AWDEN=0 Analog watchdog disabled
	// AWDCH=0 Not used
	ADC1->CFGR1=0;
	ADC_Initialized=TRUE;
	return;
}
