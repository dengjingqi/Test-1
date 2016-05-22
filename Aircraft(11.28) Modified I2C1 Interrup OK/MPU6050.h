#include <stm32f0xx.h>
#define MPU6050_ADDR    0x68
#define SELF_TEST_X     0x0d
#define SELF_TEST_Y     0x0E
#define SELF_TEST_Z     0x0F
#define SELF_TEST_A     0x10

#define SMPLRT_DIV      0x19
#define CONFIG          0x1a
#define GYRO_CONFIG     0x1b
#define ACCEL_CONFIG    0x1c
#define MOT_THR         0x1f
#define FIFO_EN         0x23

#define I2C_MST_CTRL    0x24
#define I2C_SLV0_ADDR   0x25
#define I2C_SLV0_REG    0x26
#define I2C_SLV0_CTRL   0x27
#define I2C_SLV1_ADDR   0x28
#define I2C_SLV1_REG    0x29
#define I2C_SLV1_CTRL   0x2A

#define I2C_SLV2_ADDR   0x2B
#define I2C_SLV2_REG    0x2C
#define I2C_SLV2_CTRL   0x2D
#define I2C_SLV3_ADDR   0x2E
#define I2C_SLV3_REG    0x2F
#define I2C_SLV3_CTRL   0x30

#define I2C_SLV4_ADDR   0x31
#define I2C_SLV4_REG    0x32
#define I2C_SLV4_DO     0x33
#define I2C_SLV4_CTRL   0x34
#define I2C_SLV4_DI     0x35

#define I2C_MST_STATUS  0x36
#define INT_PIN_CFG     0x37
#define INT_ENABLE      0x38
#define INT_STATUS      0x3a

#define ACCEL_XOUT_H    0x3b
#define ACCEL_XOUT_L    0x3C
#define ACCEL_YOUT_H    0x3D
#define ACCEL_YOUT_L    0x3E
#define ACCEL_ZOUT_H    0x3F
#define ACCEL_ZOUT_L    0x40

#define TEMP_OUT_H      0x41
#define TEMP_OUT_L      0x42

#define GYRO_XOUT_H     0x43
#define GYRO_XOUT_L     0x44
#define GYRO_YOUT_H     0x45
#define GYRO_YOUT_L     0x46
#define GYRO_ZOUT_H     0x47
#define GYRO_ZOUT_L     0x48

#define EXT_SENS_DATA_00 0x49
#define EXT_SENS_DATA_01 0x4a
#define EXT_SENS_DATA_02 0x4b
#define EXT_SENS_DATA_03 0x4c
#define EXT_SENS_DATA_04 0x4d
#define EXT_SENS_DATA_05 0x4e
#define EXT_SENS_DATA_06 0x4f
#define EXT_SENS_DATA_07 0x50
#define EXT_SENS_DATA_08 0x51
#define EXT_SENS_DATA_09 0x52
#define EXT_SENS_DATA_10 0x53
#define EXT_SENS_DATA_11 0x54
#define EXT_SENS_DATA_12 0x55
#define EXT_SENS_DATA_13 0x56
#define EXT_SENS_DATA_14 0x57
#define EXT_SENS_DATA_15 0x58
#define EXT_SENS_DATA_16 0x59
#define EXT_SENS_DATA_17 0x5a
#define EXT_SENS_DATA_18 0x5b
#define EXT_SENS_DATA_19 0x5c
#define EXT_SENS_DATA_20 0x5d
#define EXT_SENS_DATA_21 0x5e
#define EXT_SENS_DATA_22 0x5f
#define EXT_SENS_DATA_23 0x60

#define I2C_SLV0_DO     0x63
#define I2C_SLV1_DO     0x64
#define I2C_SLV2_DO     0x65
#define I2C_SLV3_DO     0x66

#define I2C_MST_DELAY_CTRL   0x67
#define SIGNAL_PATH_RESET    0x68
#define MOT_DETECT_CTRL      0x69
#define USER_CTRL            0x6a
#define PWR_MGMT1            0x6b
#define PWR_MGMT2            0x6c

#define FIFO_COUNTH         0x72
#define FIFO_COUNTL         0x73
#define FIOF_R_W            0x74
#define WHO_AM_I            0x75

//================define bit===================
#define XG_ST     0x80
#define YG_ST     0x40
#define ZG_ST     0x20
#define FS_SEL0   0x00
#define FS_SEL1   0x08
#define FS_SEL2   0x10
#define FS_SEL3   0x18

#define XA_ST     0x80
#define YA_ST     0x40
#define ZA_ST     0x20
#define AFS_SEL0   0x00
#define AFS_SEL1   0x08
#define AFS_SEL2   0x10
#define AFS_SEL3   0x18

#define FSYNC_DISABLE   0x00
#define TEMP_OUT_L0     0x08   //001
#define GYRO_XOUT_L0    0x10   //010
#define GYRO_YOUT_L0    0x18   //011
#define GYRO_ZOUT_L0    0x20   //100

#define ACCEL_XOUT_L0    0x28  //101
#define ACCEL_YOUT_L0    0x30  //110
#define ACCEL_ZOUT_L0    0x38  //111

#define DLPF_CFG0   00
#define DLPF_CFG1   01
#define DLPF_CFG2   02
#define DLPF_CFG3   03
#define DLPF_CFG4   04
#define DLPF_CFG5   05
#define DLPF_CFG6   06
#define DLPF_CFG7   07

#define TEMP_FIFO_EN    0x80
#define XG_FIFO_EN      0x40
#define YG_FIFO_EN      0x20
#define ZG_FIFO_EN      0x10

#define ACCEL_FIFO_EN     0x08
#define SLV2_FIFO_EN      0x04
#define SLV1_FIFO_EN      0x02
#define SLV0_FIFO_EN      0x01

#define MULT_MST_EN     0x80
#define WAIT_FOR_ES     0x40
#define I2C_MST_P_NSR   0x10
#define I2C_MST_CLK400K 0x0d  
#define SLV3_FIFO_EN      0x20

//=============I2C_SLV0_ADDR==============
#define I2C_SLAVE_ADDR   0x01
#define I2C_SLAVE_W      0x80
#define I2C_SLAVE_R      0x00

//===========I2C_SLV0_CTRL================
#define I2C_SLAVE_REG    0x00
#define I2C_SLAVE_EN     0x80 
#define I2C_SLAVE_BYTE_SW  0x40
#define I2C_SLAVE_REG_DIS  0x20
#define I2C_SLAVE_GRP      0x10
#define I2C_SLAVE_LEN      0x0f

//===========I2C_MST_STATUS=============
#define PASS_THROUGH    0x80
#define I2C_SLV4_DONE   0x40
#define I2C_LOST_ARB    0x20
#define I2C_SLV4_NACK   0x10
#define I2C_SLV3_NACK   0x08
#define I2C_SLV2_NACK   0x04
#define I2C_SLV1_NACK   0x02
#define I2C_SLV0_NACK   0x01

//===========INT_PIN_CFG============
#define  INT_LEVEL    0x80
#define  INT_OPEN     0x40
#define  LATCH_INT_EN 0x20
#define  INT_RD_CLEAR 0x10
#define  FSYNC_INT_LEVEL    0x08
#define  FSYNC_INT_EN       0x04
#define  I2C_BYPASS_EN      0x02

//===========INT_ENABLE===============
#define MOT_EN    0x40
#define FIFO_OFLOW_EN   0x10
#define I2C_MST_INT_EN  0x08
#define DATA_RDY_EN     0x01

//============INT_STATUS=============
#define MOT_INT    0x40
#define FIFO_OFLOW_INT   0x10
#define I2C_MST_INT_INT  0x08
#define DATA_RDY_INT     0x01

//===============I2C_MST_DELAY_CTRL=============
#define DELAY_ES_SHADOW    0x80
#define I2_SLV4_DLY_EN     0x10
#define I2_SLV3_DLY_EN     0x08
#define I2_SLV2_DLY_EN     0x04
#define I2_SLV1_DLY_EN     0x02
#define I2_SLV0_DLY_EN     0x01

//================SIGNAL_PATH_RESET==============
#define GYRO_RESET  0x04
#define ACCEL_RESET 0x02
#define TEMP_RESET  0x01

//===============USER_CTRL===============
#define USER_CTRL_FIFO_EN        0x40
#define USER_CTRL_I2C_MST_EN     0x20
#define USER_CTRL_I2C_IF_DIS     0x10
#define FIFO_RESET               0x04
#define I2C_MST_RESET            0x02
#define SIG_COND_RESET           0x01

//=================PWR_MGMT1====================
#define  DEVIVE_RESET  0x80
#define  SLEEP         0x40
#define  CYCLE         0x20
#define  TEMP_DIS      0x08
#define  CLKSEL0       0x00
#define  CLKSEL1       0x01
#define  CLKSEL2       0x02
#define  CLKSEL3       0x03
#define  CLKSEL4       0x04
#define  CLKSEL5       0x05
#define  CLKSEL6       0x06
#define  CLKSEL7       0x07

//=================PWR_MGMT1====================
#define  LP_WAKE_CTRL0   0x00
#define  LP_WAKE_CTRL1   0x40
#define  LP_WAKE_CTRL2   0x80
#define  LP_WAKE_CTRL3   0xc0
#define  STBY_XA         0x20
#define  STBY_YA         0x10
#define  STBY_ZA         0x08
#define  STBY_XG         0x04
#define  STBY_YG         0x02
#define  STBY_ZG         0x01

BOOL Read_SELF_TEST(float *tempregX);
float Calculate_FTG(float  *FTG);
float Calculate_FTYG(float *FTYG);
float Calculate_FTA(float *FTA);
BOOL	 Read_Sample_Rate(unsigned char *tmpreg);
BOOL Read_Config(unsigned char *tmpreg);
BOOL Write_Config(unsigned char *tmpreg);
BOOL Write_GYRO_CONFIG(unsigned char *tmpreg);
BOOL Write_ACCEL_CONFIG(unsigned char *tmpreg);
BOOL Write_MOT_THR(unsigned char *tmpreg);
BOOL Write_FIFO_EN(unsigned char *tmpreg);
BOOL Write_MPU6050_MST_CTRL(unsigned char *tmpreg);
BOOL Write_MPU6050_SLAVE_ADDR(unsigned char *I2C,unsigned char *tmpreg);
BOOL Write_MPU6050_SLAVE_REG(unsigned char *I2C,unsigned char *tmpreg);
BOOL Write_MPU6050_SLAVE_CTRL(unsigned char *I2C,unsigned char *tmpreg);
BOOL MPU6050_Select_SLAVE(unsigned char *I2C,unsigned char *SLAVE_ADDR,unsigned char *SLAVE_REG,unsigned char *SLAVE_CTRL);
BOOL Write_MPU6050_SLAVE_DO(unsigned char *I2C,unsigned char *tmpreg);
BOOL Read_MPU6050_SLAVE_DI(unsigned char *I2C,unsigned char *tmpreg);
BOOL Read_MPU6050_MST_STATUS(unsigned char *tmpreg);
BOOL Write_MPU6050_INT_PIN_CFG(unsigned char *tmpreg);
BOOL Write_MPU6050_INT_ENABLE(unsigned char *tmpreg);
BOOL Read_MPU6050_INT_STATUS(unsigned char *tmpreg);
BOOL Read_MPU6050_ACCEL(unsigned short *tmpreg);
BOOL Read_MPU6050_TEMP(unsigned short *tmpreg);
BOOL Read_MPU6050_GYRO(unsigned short *tmpreg);
BOOL Write_MPU6050_MST_DELAY_CTRL(unsigned char *tmpreg);
BOOL Write_SIGNAL_PATH_RESET(unsigned char *tmpreg);
BOOL Write_MOT_DETECT_CTRL(unsigned char *tmpreg);
BOOL Write_USER_CTRL(unsigned char *tmpreg);
BOOL Write_PWR_MGMT1(unsigned char *tmpreg);
BOOL Write_PWR_MGMT1(unsigned char *tmpreg);
BOOL Read_MPU6050_FIFO_COUNT(unsigned short *tmpreg);
BOOL Read_MPU6050_FIFO(unsigned char *tmpreg);
BOOL Write_MPU6050_FIFO(unsigned char *tmpreg);
BOOL Read_MPU6050_ADDR(unsigned char *tmpreg);

BOOL Read_MPU6050_Reg(unsigned char Reg_Addr,unsigned char *tmpreg);
BOOL Write_MPU6050_Reg(unsigned char Reg_Addr,unsigned char tmpreg);
void MPU6050_Init(void);
void MPU6050_Config(void);
unsigned char MPU6050_GetSensorsData(short *pSensorsDataBuff);

