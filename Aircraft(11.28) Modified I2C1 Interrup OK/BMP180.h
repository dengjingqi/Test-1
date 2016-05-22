#include "stm32f30x.h"
/*=====================================================
The 176 bit E2PROM is partitioned in 11 words of 16 bit each. These contain 11 calibration 
coefficients. Every sensor module has individual coefficients. Before the first calculation of 
temperature and pressure, the master reads out the E2PROM data.
The data communication can be checked by checking that none of the words has the value 0 or 0xFFFF.
==========================================================*/
#define   BMP180_AC1M    0xaa
#define   BMP180_AC1L    0xab
#define   BMP180_AC2M    0xac
#define   BMP180_AC2L    0xad
#define   BMP180_AC3M    0xae
#define   BMP180_AC3L    0xaf

#define   BMP180_AC4M    0xB0
#define   BMP180_AC4L    0xB1
#define   BMP180_AC5M    0xB2
#define   BMP180_AC5L    0xB3
#define   BMP180_AC6M    0xB4
#define   BMP180_AC6L    0xB5

#define BMP180_B1M  0xB6
#define BMP180_B1L  0xB7
#define BMP180_B2M  0xB8
#define BMP180_B2L  0xB9

#define BMP180_MBM  0xBA
#define BMP180_MBL  0xBB
#define BMP180_MCM  0xBC
#define BMP180_MCL  0xBD
#define BMP180_MDM  0xBE
#define BMP180_MDL  0xBF

#define BMP180_ADCM  0xF6
#define BMP180_ADCL  0xF7
#define BMP180_ADCXL  0xF8
#define BMP180_Control_R  0xF4

#define BMP180_Reset  0xE0
#define BMP180_ID  0xD0

#define BMP180_Addr  0xee
#define BMP180_UT    0x2e
#define BMP180_UP    0x34
#define BMP180_OSS0  0x00
#define BMP180_OSS1  0x40
#define BMP180_OSS2  0x80
#define BMP180_OSS3  0xc0

ErrorStatus BMP180_Init(unsigned int *tmpreg);
ErrorStatus Read_UT_UP_Value(long *tmpreg,unsigned char UT_UP);
void Calculate_UT_Value(float *tempreg);
void Calculate_UP_Value(float *tempreg);
void Calculate_Altitude_Value(float *tempreg);


