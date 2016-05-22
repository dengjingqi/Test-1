#define ADC_NB_SAMPLES 8

void ADC_MsIRQHandler(void);
unsigned short ADC_GetInternalVoltage(void);
void ADC_Init(void);
