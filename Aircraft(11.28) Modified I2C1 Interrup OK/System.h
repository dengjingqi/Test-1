#define SYSCLK  48000000
#define SYSTICK (SYSCLK/8) // 6MHz

#define FACTORY_CONFIG_ADDRESS 128

#define UsToSysTick(us) (us*(SYSCLK*32/1000000))

#define BOOL  unsigned char
#define TRUE  1
#define FALSE 0
#define NULL  0

#define IRQ_PRI_LOW      3
#define IRQ_PRI_MIDDLE   2
#define IRQ_PRI_HIGH     1
#define IRQ_PRI_REALTIME 0

typedef struct
{
	unsigned long PowerVoltageCorrection; // Reference is 10000
	unsigned short Checksum16;
} sSYS_FactoryConfig;

typedef union
{
	unsigned long GlobalFlags;
	struct
	{
#if defined(FS_IA4)
		unsigned char Timer17IRQ;
#elif defined(FS_IA6B) || defined(FS_CEI_IA6) || defined(FS_IA10) || defined(CSJ_IA10)
		unsigned char Timer14IRQ;
#endif
		unsigned char Timer16IRQ;
	} Flags;
} sSYS_WatchdogFlags;

extern volatile unsigned long SYS_SysTickMs;
extern sSYS_WatchdogFlags SYS_WatchdogFlags;
extern sSYS_FactoryConfig SYS_FactoryConfig;

void TIM16_IRQHandler(void);
void SystemInit(void);
void SYS_DelaySysTick(long Delay_SysTick);
void SYS_DelayUs(long Delay_us);
void SYS_DisableIRQs(void);
void SYS_EnableIRQs(void);
unsigned long SYS_GetSysTick(void);
void SYS_Init(void);
void SYS_LoadFactoryConfig(void);
void SYS_StartWatchdog(void);
void SYS_ResetToBootloader(void);
void SYS_ResetWatchdog(void);
