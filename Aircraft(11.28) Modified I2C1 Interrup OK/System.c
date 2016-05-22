#include <stm32f0xx.h>
#include "System.h"
#include "ADC.h"
#include "SPI.h"
#include "RF_XN297.h"

sSYS_FactoryConfig SYS_FactoryConfig;
volatile unsigned long SYS_SysTickMs;
sSYS_WatchdogFlags SYS_WatchdogFlags;
unsigned long DisableIRQLevel;
unsigned long SYS_LastIRQIntMask;

void TIM16_IRQHandler(void)
{
	if (TIM16->SR&TIM_SR_UIF)
	{
		TIM16->SR&=~TIM_SR_UIF;
		SYS_WatchdogFlags.Flags.Timer16IRQ^=1;
		SYS_SysTickMs++;
		ADC_MsIRQHandler();
	}
	return;
}


void SystemInit(void)
{
	return;
}

void SYS_DelaySysTick(long Delay_SysTick)
{
	long DelaySysTick;
	long StartSysTick;

	StartSysTick=SysTick->VAL<<8;
	DelaySysTick=Delay_SysTick<<8;
	while (StartSysTick-(SysTick->VAL<<8)<DelaySysTick);
	return;
}

void SYS_DelayUs(long Delay_us)
{
	long DelaySysTick;
	long StartSysTick;

	StartSysTick=SysTick->VAL<<8;
	DelaySysTick=UsToSysTick(Delay_us);
	while (StartSysTick-(SysTick->VAL<<8)<DelaySysTick);
	return;
}

void  SYS_DisableIRQs(void)
{
	if (DisableIRQLevel==0)
	{
		SYS_LastIRQIntMask=NVIC->ISER[0];
		NVIC->ICER[0]=0xFFFFFFFF;
	}
	DisableIRQLevel++;
	return;
}

void  SYS_EnableIRQs(void)
{
	if (DisableIRQLevel==0)
		return;
	DisableIRQLevel--;
	if (DisableIRQLevel==0)
		NVIC->ISER[0]=SYS_LastIRQIntMask;
	return;
}

unsigned long SYS_GetSysTick(void)
{
	return SysTick->VAL;
}

void SYS_Init(void)
{
	SYS_SysTickMs=0;
	DisableIRQLevel=0;

	// Enable the SysTick counter and set the maximum reload value
	SysTick->LOAD=0xFFFFFF;
	// SysTick SYSCLK/8=6M
	SysTick->CTRL|=1<<SysTick_CTRL_ENABLE_Pos;

	// Enable the SYSCFG clock
	RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN;

	// Enable HSI clock
	RCC->CR|=RCC_CR_HSION;
	// Wait for the HSI clock to be stable
	while (!(RCC->CR&RCC_CR_HSIRDY));
	// Set PLL Clock (8(HSI)*6 MHz) to System Clock
	// Disable the PLL
	RCC->CR&=~RCC_CR_PLLON;
	// Wait for the PLL to be disabled
	while (RCC->CR&RCC_CR_PLLRDY);
	// SW=2 PLL selected as system clock
	// SWI=0 read only, set by hardware
	// HPRE=0 HAB clock = SYSCLK = HCLK
	// PPRE=0 PCLK not divided, APB
	// PLLSRC=0 PLL input source = HSI/2
	// PLLXTPRE=0 Disable the PLL prescaler
	// PLLMUL=10 PLL multiplication factor = 12*4
	// MCO=0 MCU clock output disabled
	// MCOPRE=0 ignored
	// PLLNODIV ignored
	RCC->CFGR=RCC_CFGR_PLLSRC_HSI_DIV2 | RCC_CFGR_PLLMUL12;	
	// Turn on the PLL
	RCC->CR|=RCC_CR_PLLON;
	// Wait for the PLL to be ready (locked)
	while (!RCC->CR&RCC_CR_PLLRDY);
	// Set the PLL output as system clock source
	RCC->CFGR=(RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
	// Wait for the PLL to have been selected as system clock source
	while ((RCC->CFGR & RCC_CFGR_SWS)!=RCC_CFGR_SWS_1);
	// Enable IO ports A, B and F and DMA clocks
	RCC->AHBENR|=RCC_AHBENR_DMAEN | RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOFEN;
#if defined(DEBUG)
	// Enable the DBGMCU module clock	
	RCC->APB2ENR|=RCC_APB2ENR_DBGMCUEN;
#endif

	// Set TIM16 to be a 1ms periodic interrupt
	// Enable TIM16 clock
	RCC->APB2ENR|=RCC_APB2ENR_TIM16EN;
	// Reset TIM16
	RCC->APB2RSTR|=RCC_APB2RSTR_TIM16RST;
	RCC->APB2RSTR&=~RCC_APB2RSTR_TIM16RST;
	// CEN=0 counter disabled
	// UDIS=0 Update event enabled
	// URS=0 Update event is any event
	// OPM=0 Counter is not stopped at update event
	// ARPE=0 ARR register is not buffered
	// CKD=0 not used
	TIM16->CR1=0;
	// UIE=1 interrupt enabled
	// Other interrupts and DMA disabled
	TIM16->DIER=TIM_DIER_UIE;
	// Prescaler=1MHz timer counter clock
	TIM16->PSC=SYSCLK/1000000-1;
	// Generate an update event to update the prescaler
	TIM16->EGR=TIM_EGR_UG;
	// Auto reload register set to 999 (counter overflows every 1ms)
	TIM16->ARR=1000-1;
	// Enable the timer counter
	TIM16->CR1|=TIM_CR1_CEN;
	// Set the timer 16 IRQ to low priority and enable it
	NVIC_SetPriority(TIM16_IRQn,IRQ_PRI_LOW);
	NVIC->ISER[0]|=1UL<<TIM16_IRQn;
	// Trigger an event
	TIM16->EGR|=TIM_EGR_UG;
	
#if !defined(DEBUG)
	// Enable the watchdog
	// LSI RC oscillator frequency varies from 30 to 50KHz, set the watchdog on a 50KHz calculation for safety
	// The desired watchdog minimum trigger time is 10ms
	// Enable register access
	IWDG->KR=0x5555;
	// Set the prescaler to 4 (50KHz/4=12500Hz)
	IWDG->PR=0;
	// Set the reload register to 2500 (200ms timeout)
	IWDG->RLR=2500;
#endif

#if defined(DEBUG)
	// Freeze everything when debug stop
	DBGMCU->APB1FZ|=DBGMCU_APB1_FZ_DBG_TIM2_STOP | DBGMCU_APB1_FZ_DBG_TIM3_STOP |
		DBGMCU_APB1_FZ_DBG_TIM14_STOP | DBGMCU_APB1_FZ_DBG_IWDG_STOP;
	DBGMCU->APB2FZ|=DBGMCU_APB2_FZ_DBG_TIM1_STOP | DBGMCU_APB2_FZ_DBG_TIM16_STOP |
		DBGMCU_APB2_FZ_DBG_TIM17_STOP;
#endif
	return;
}


void SYS_ResetWatchdog(void)
{
#if !defined(DEBUG)
	IWDG->KR=0xAAAA;
#endif
	return;
}
void SYS_StartWatchdog(void)
{
#if !defined(DEBUG)
	IWDG->KR=0xCCCC;
#endif
	return;
}
