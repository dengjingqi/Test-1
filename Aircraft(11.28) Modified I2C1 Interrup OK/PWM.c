#include "Config.h"
#include <stm32f0xx.h>
#include "System.h"
#include "PWM.h"

BOOL PWM_Initialized;
BOOL PWM_ChannelsSet;
BOOL PWM_FrequencySet;

unsigned long PWM_Frequency;

void PWM_Init(void)
{
	if (PWM_Initialized)
		return;
	PWM_Frequency=0; // Not set yet
	PWM_FrequencySet=FALSE;
	PWM_ChannelsSet=FALSE;

#if defined(FS_IA4)
	// Set PA6 and 7 to their respective TIM3 CHX output
	GPIOA->AFR[0]=(GPIOA->AFR[0]&0x00FFFFFF) | 0x11000000;
	// Set PA6 and 7 to alternate function
	GPIOA->MODER=(GPIOA->MODER&0xFFFF0FFF) | 0x0000A000;
	// Set PB0 and 1 to their respective TIM3 CHX output
	GPIOB->AFR[0]=(GPIOB->AFR[0]&0xFFFFFF00) | 0x00000011;
	// Set PB0 and 1 to alternate function
	GPIOB->MODER=(GPIOB->MODER&0xFFFFFFF0) | 0x0000000A;
#elif defined(FS_IA6B)
	// Set PA6 and 7 to their respective TIM3 CHX output
	GPIOA->AFR[0]=(GPIOA->AFR[0]&0x00FFFFFF) | 0x11000000;
	// Set PA6 and 7 alternate function
	GPIOA->MODER=(GPIOA->MODER&0xFFFF0FFF) | 0x0000A000;
	// Set PB0, 1, 10 and 11 to their respective TIM2 or TIM3 CHX output
	GPIOB->AFR[0]=(GPIOB->AFR[0]&0xFFFFFF00) | 0x00000011;
	GPIOB->AFR[1]=(GPIOB->AFR[1]&0xFFFF00FF) | 0x00002200;
	// Set PB0, 1, 10 and 11 to alternate function
	GPIOB->MODER=(GPIOB->MODER&0xFF0FFFF0) | 0x00A0000A;
#elif defined(FS_IA10) || defined(CSJ_IA10)
	// Set PA6, 7, 8, 9, 10 and 11 to their respective TIM1 or TIM3 CHX output
	GPIOA->AFR[0]=(GPIOA->AFR[0]&0x00FFFFFF) | 0x11000000;
	GPIOA->AFR[1]=(GPIOA->AFR[1]&0xFFFF0000) | 0x00002222;
	// Set PA6, 7, 8, 9, 10 and 11 to alternate function
	GPIOA->MODER=(GPIOA->MODER&0xFF000FFF) | 0x00AAA000;
	// Set PB0, 1, 10 and 11 to their respective TIM2 or TIM3 CHX output
	GPIOB->AFR[0]=(GPIOB->AFR[0]&0xFFFFFF00) | 0x00000011;
	GPIOB->AFR[1]=(GPIOB->AFR[1]&0xFFFF00FF) | 0x00002200;
	// Set PB0, 1, 10 and 11 to alternate function
	GPIOB->MODER=(GPIOB->MODER&0xFF0FFFF0) | 0x00A0000A;
#endif
#if defined(FS_IA10) || defined(CSJ_IA10)
	// Set TIM1 to be an up counter with a 1us increment time and a PWM output on its 4 channels
	// Enable TIM1 clock
	RCC->APB2ENR|=RCC_APB2ENR_TIM1EN;
	// Reset TIM1
	RCC->APB2RSTR|=RCC_APB2RSTR_TIM1RST;
	RCC->APB2RSTR&=~RCC_APB2RSTR_TIM1RST;
	// CEN=0 counter disabled
	// UDIS=0 Update event enabled
	// URS=0 Update event is any event
	// OPM=0 Counter is not stopped at update event
	// DIR=0 Counter used as upcounter
	// CMS=0 The counter counts up
	// ARPE=1 ARR register is buffered
	// CKD=0 not used
	TIM1->CR1=TIM_CR1_ARPE;
	// CCPC=0 CCxE, CCxNE and OCxM bits are not preloaded
	// CCUS=0 Not used
	// CCDS=0 Not used
	// MMS=0 Not used
	// TI1S=0 Not used
	// OIS1=0 Not used
	// OIS1N=0 Not used
	// OIS2=0 Not used
	// OIS2N=0 Not used
	// OIS3=0 Not used
	// OIS3N=0 Not used
	// OIS4=0 Not used
	TIM1->CR2=0;
	// Disable all channels output
	TIM1->CCER=0;
	// CC1S=0 CC1 channel is configured as output
	// OC1FE=0 Not used
	// OC1PE=1 Output Compare 1 preload enabled
	// OC1M=6 PWM1 mode
	// OC1CE=0 OC1Ref is not affected by the ETRF Input
	// CC2S=0 CC2 channel is configured as output
	// OC2FE=0 Not used
	// OC2PE=1 Output Compare 2 preload enabled
	// OC2M=6 PWM1 mode
	// OC2CE=0 OC2Ref is not affected by the ETRF Input
	TIM1->CCMR1=TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
		TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
	// CC3S=0 CC3 channel is configured as output
	// OC3FE=0 Not used
	// OC3PE=1 Output Compare 3 preload enabled
	// OC3M=6 PWM1 mode
	// OC3CE=0 OC3Ref is not affected by the ETRF Input
	// CC4S=0 CC4 channel is configured as output
	// OC4FE=0 Not used
	// OC4PE=1 Output Compare 4 preload enabled
	// OC4M=6 PWM1 mode
	// OC4CE=0 OC4Ref is not affected by the ETRF Input
	TIM1->CCMR2=TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 |
		TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
	// Enable all channels non complementary output
	// CC1E=1 OC1 output active
	// CC1P=0 OC1 active high
	// CC1NE=0 OC1N output not active
	// CC1NP=0 Not used
	// CC2E=1 OC2 output active
	// CC2P=0 OC2 active high
	// CC2NE=0 OC2N output not active
	// CC2NP=0 Not used
	// CC3E=1 OC3 output active
	// CC3P=0 OC3 active high
	// CC3NE=0 OC3N output not active
	// CC3NP=0 Not used
	// CC4E=1 OC4 output active
	// CC4P=0 OC4 active high
	TIM1->CCER=TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
	// Set a 1MHz counter
	TIM1->PSC=SYSCLK/1000000-1;
	// Block the counter
	TIM1->ARR=0;
	// Generate an update event to update the prescaler and ARR
	TIM1->EGR=TIM_EGR_UG;
	// Enable outputs
	TIM1->BDTR|=TIM_BDTR_MOE;
	// Enable the timer counter
	TIM1->CR1|=TIM_CR1_CEN;
#endif

#if defined(FS_IA6B) || defined(FS_IA10) || defined(CSJ_IA10)
	// Set TIM2 to be an up counter with a 1us increment time and a PWM output on its channels 3 and 4
	// Enable TIM2 clock
	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;
	// Reset TIM2
	RCC->APB1RSTR|=RCC_APB1RSTR_TIM2RST;
	RCC->APB1RSTR&=~RCC_APB1RSTR_TIM2RST;
	// CEN=0 counter disabled
	// UDIS=0 Update event enabled
	// URS=0 Update event is any event
	// OPM=0 Counter is not stopped at update event
	// DIR=0 Counter used as upcounter
	// CMS=0 The counter counts up
	// ARPE=1 ARR register is buffered
	// CKD=0 not used
	TIM2->CR1=TIM_CR1_ARPE;
	// CCDS=0 Not used
	// MMS=0 Not used
	// TI1S=0 Not used
	TIM2->CR2=0;
	// Disable all channels output
	TIM2->CCER=0;
	// CC1S=0 CC1 channel is configured as output
	// OC1FE=0 Not used
	// OC1PE=1 Output Compare 1 preload enabled
	// OC1M=6 PWM1 mode
	// OC1CE=0 OC1Ref is not affected by the ETRF Input
	// CC2S=0 CC2 channel is configured as output
	// OC2FE=0 Not used
	// OC2PE=1 Output Compare 2 preload enabled
	// OC2M=6 PWM1 mode
	// OC2CE=0 OC2Ref is not affected by the ETRF Input
	TIM2->CCMR1=TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
		TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
	// CC3S=0 CC3 channel is configured as output
	// OC3FE=0 Not used
	// OC3PE=1 Output Compare 3 preload enabled
	// OC3M=6 PWM1 mode
	// OC3CE=0 OC3Ref is not affected by the ETRF Input
	// CC4S=0 CC4 channel is configured as output
	// OC4FE=0 Not used
	// OC4PE=1 Output Compare 4 preload enabled
	// OC4M=6 PWM1 mode
	// OC4CE=0 OC4Ref is not affected by the ETRF Input
	TIM2->CCMR2=TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 |
		TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
	// Enable all channels non complementary output
	// CC1E=0 OC1 is not active
	// CC1P=0 OC1 active high
	// CC1NE=0 OC1N output not active
	// CC1NP=0 Not used
	// CC2E=0 OC2 is not active
	// CC2P=0 OC2 active high
	// CC2NE=0 OC2N output not active
	// CC2NP=0 Not used
	// CC3E=1 OC3 output active
	// CC3P=0 OC3 active high
	// CC3NE=0 OC3N output not active
	// CC3NP=0 Not used
	// CC4E=1 OC4 output active
	// CC4P=0 OC4 active high
	TIM2->CCER=TIM_CCER_CC3E | TIM_CCER_CC4E;
	// Set a 1MHz counter
	TIM2->PSC=SYSCLK/1000000-1;
	// Block the counter
	TIM2->ARR=0;
	// Generate an update event to update the prescaler and ARR
	TIM2->EGR=TIM_EGR_UG;
	// Enable outputs
	TIM2->BDTR|=TIM_BDTR_MOE;
	// Enable the timer counter
	TIM2->CR1|=TIM_CR1_CEN;
#endif

	// Set TIM3 to be an up counter with a 1us increment time and a PWM output on its 4 channels
	// Enable TIM3 clock
	RCC->APB1ENR|=RCC_APB1ENR_TIM3EN;
	// Reset TIM3
	RCC->APB1RSTR|=RCC_APB1RSTR_TIM3RST;
	RCC->APB1RSTR&=~RCC_APB1RSTR_TIM3RST;
	// CEN=0 counter disabled
	// UDIS=0 Update event enabled
	// URS=0 Update event is any event
	// OPM=0 Counter is not stopped at update event
	// DIR=0 Counter used as upcounter
	// CMS=0 The counter counts up
	// ARPE=1 ARR register is buffered
	// CKD=0 not used
	TIM3->CR1=TIM_CR1_ARPE;
	// CCDS=0 Not used
	// MMS=0 Not used
	// TI1S=0 Not used
	TIM3->CR2=0;
	// Disable all channels output
	TIM3->CCER=0;
	// CC1S=0 CC1 channel is configured as output
	// OC1FE=0 Not used
	// OC1PE=1 Output Compare 1 preload enabled
	// OC1M=6 PWM1 mode
	// OC1CE=0 OC1Ref is not affected by the ETRF Input
	// CC2S=0 CC2 channel is configured as output
	// OC2FE=0 Not used
	// OC2PE=1 Output Compare 2 preload enabled
	// OC2M=6 PWM1 mode
	// OC2CE=0 OC2Ref is not affected by the ETRF Input
	TIM3->CCMR1=TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
		TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
	// CC3S=0 CC3 channel is configured as output
	// OC3FE=0 Not used
	// OC3PE=1 Output Compare 3 preload enabled
	// OC3M=6 PWM1 mode
	// OC3CE=0 OC3Ref is not affected by the ETRF Input
	// CC4S=0 CC4 channel is configured as output
	// OC4FE=0 Not used
	// OC4PE=1 Output Compare 4 preload enabled
	// OC4M=6 PWM1 mode
	// OC4CE=0 OC4Ref is not affected by the ETRF Input
	TIM3->CCMR2=TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 |
		TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
	// Enable all channels non complementary output
	// CC1E=1 OC1 output active
	// CC1P=0 OC1 active high
	// CC1NE=0 OC1N output not active
	// CC1NP=0 Not used
	// CC2E=1 OC2 output active
	// CC2P=0 OC2 active high
	// CC2NE=0 OC2N output not active
	// CC2NP=0 Not used
	// CC3E=1 OC3 output active
	// CC3P=0 OC3 active high
	// CC3NE=0 OC3N output not active
	// CC3NP=0 Not used
	// CC4E=1 OC4 output active
	// CC4P=0 OC4 active high
	TIM3->CCER=TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
	// Set a 1MHz counter
	TIM3->PSC=SYSCLK/1000000-1;
	// Block the counter
	TIM3->ARR=0;
	// Generate an update event to update the prescaler and ARR
	TIM3->EGR=TIM_EGR_UG;
	// Enable outputs
	TIM3->BDTR|=TIM_BDTR_MOE;
	// Enable the timer counter
	TIM3->CR1|=TIM_CR1_CEN;

	PWM_Initialized=TRUE;
	return;
}

#if defined(FS_SZL_IA10)
void PWM_SetChannels(const unsigned short *pChannelsValue)
{
	if (!PWM_Initialized)
		return;
	RFM_SZL_TransmitPWMChannels();
	RFM_LastTransmitFailSafeMs=SYS_SysTickMs;
}
#else
void PWM_SetChannels(const unsigned short *pChannelsValue)
{
	if (!PWM_Initialized)
		return;
#if defined(FS_IA4)
	TIM3->CCR4=pChannelsValue[0];
	TIM3->CCR3=pChannelsValue[1];
	TIM3->CCR2=pChannelsValue[2];
	TIM3->CCR1=pChannelsValue[3];
#elif defined(FS_IA6B)
	TIM3->CCR1=pChannelsValue[0];
	TIM3->CCR2=pChannelsValue[1];
	TIM3->CCR3=pChannelsValue[2];
	TIM3->CCR4=pChannelsValue[3];
	TIM2->CCR3=pChannelsValue[4];
	TIM2->CCR4=pChannelsValue[5];
#elif defined(FS_IA10) || defined(CSJ_IA10)
	TIM1->CCR2=pChannelsValue[0];
	TIM1->CCR1=pChannelsValue[1];
	TIM1->CCR4=pChannelsValue[2];
	TIM1->CCR3=pChannelsValue[3];
	TIM2->CCR4=pChannelsValue[4];
	TIM2->CCR3=pChannelsValue[5];
	TIM3->CCR4=pChannelsValue[6];
	TIM3->CCR3=pChannelsValue[7];
	TIM3->CCR2=pChannelsValue[8];
	TIM3->CCR1=pChannelsValue[9];
#endif
	PWM_ChannelsSet=TRUE;
	if (!PWM_FrequencySet &&
		PWM_Frequency!=0)
		PWM_SetFrequency(0);
	return;
}
#endif

void PWM_SetFrequency(unsigned long Frequency)
{
	unsigned long ARR;

	if (!PWM_Initialized)
		return;
	if (Frequency!=0)
	{
		if (Frequency==PWM_Frequency)
			return;
		PWM_Frequency=Frequency;
	}
	// Channels position must be set first, delay the frequency setting
	if (!PWM_ChannelsSet)
		return;
	// Set the cycle time
	ARR=(1000000+PWM_Frequency/2)/PWM_Frequency-1;
#if defined(FS_IA10) || defined(CSJ_IA10)
	TIM1->ARR=ARR;
#endif
#if defined(FS_IA6B) || defined(FS_IA10) || defined(CSJ_IA10)
	TIM2->ARR=ARR;
#endif
	TIM3->ARR=ARR;
	// Start the timers if first time frequency setting
	if (!PWM_FrequencySet)
	{
#if defined(FS_IA10) || defined(CSJ_IA10)
		TIM1->EGR|=TIM_EGR_UG;
#endif
#if defined(FS_IA6B) || defined(FS_IA10) || defined(CSJ_IA10)
		TIM2->EGR|=TIM_EGR_UG;
#endif
		TIM3->EGR|=TIM_EGR_UG;
		PWM_FrequencySet=TRUE;
	}
	return;
}
