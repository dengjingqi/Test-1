#if defined(CSJ_IA10)
	#define TWO_ANTENNAS
	#define UPDATE_SWITCH
	#define I_BUS
	#define I_SETUP
#endif

#if defined(FS_IA4)
	#define UPDATE_SWITCH
	#define PWM
	#define I_BUS
	#define I_BUS_FULL
	#define I_SETUP
#endif

#if defined(FS_IA6B)
	#define TWO_ANTENNAS
	#define UPDATE_SWITCH
	#define PWM
	#define PPM
	#define PPM_TimerNb         3
	#define PPM_TIMn            TIM3
	#define PPM_TIMn_IRQHandler TIM3_IRQHandler
	#define PPM_TIMn_IRQn       TIM3_IRQn
	#define PPM_TIMn_CHn        1 // Timer output channel number from 1 to 4
	#define PPM_NB_CHANNELS     6
	#define I_BUS
	#define I_BUS_FULL
	#define I_SETUP
#endif

#if defined(FS_CEI_IA6)
	#define TWO_ANTENNAS
	#define UPDATE_SWITCH
	#define PPM
	#define PPM_TimerNb         2
	#define PPM_TIMn            TIM2
	#define PPM_TIMn_IRQHandler TIM2_IRQHandler
	#define PPM_TIMn_IRQn       TIM2_IRQn
	#define PPM_TIMn_CHn        4 // Timer output channel number from 1 to 4
	#define PPM_NB_CHANNELS     6
	#define I_BUS
#endif

#if defined(FS_IA10)
	#define TWO_ANTENNAS
	#define UPDATE_SWITCH
	#define PWM
	#define PPM
	#define PPM_TimerNb         1
	#define PPM_TIMn            TIM1
	#define PPM_TIMn_IRQHandler TIM1_CC_IRQHandler
	#define PPM_TIMn_IRQn       TIM1_CC_IRQn
	#define PPM_TIMn_CHn        2 // Timer output channel number from 1 to 4
	#define PPM_NB_CHANNELS     8
	#define I_BUS
	#define I_BUS_FULL
	#define I_SETUP
#endif

#if defined(FS_FY_IA6)
#endif

#if defined(FS_FY_IA10)
	#define TWO_ANTENNAS
	#define UPDATE_SWITCH
#endif

#if defined(FS_SZL_IA10)
	#define I_BUS
	#define I_BUS_LIGHT
	#define TWO_ANTENNAS
	#define UPDATE_SWITCH
	#define PWM_NB_CHANNELS 6
#endif
