#define FBM320_ADDR          0x6D
#define FBM320_DATA_LSB      0xF8
#define FBM320_DATA_CSB      0xF7
#define FBM320_DATA_MSB      0xF6
#define FBM320_CONFIG        0xF4
#define FBM320_CALLCOEFF1    0xF1
#define FBM320_CALLCOEFF2    0xD0
#define FBM320_CALLCOEFF3    0xAA
#define FBM320_PARTID        0x6B
#define FBM320_SOFTRST       0xE0

BOOL Read_FBM320_24BitAD(unsigned long *Pressuretmpreg,unsigned long *Temptmpreg);
void FBM320_Init(void);

