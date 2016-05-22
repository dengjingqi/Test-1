#define XN297_ENABLE()		GPIOB->ODR |=  (1<<8)
#define XN297_DISABLE()		GPIOB->ODR &= ~(1<<8)

#define XN297_CMD_RD_REG					0x00
#define XN297_CMD_WR_REG					0x20
#define XN297_CMD_RD_RX_PLOAD			0x61
#define XN297_CMD_WR_TX_PLOAD			0xA0
#define XN297_CMD_FLUSH_TX				0xE1
#define XN297_CMD_FLUSH_RX				0xE2
#define XN297_CMD_REUSE_TX_PL			0xE3
#define XN297_CMD_ACTIVATE				0x50
#define XN297_CMD_RD_RX_PL_WID		0x60
#define XN297_CMD_WR_ACK_PLOAD		0xA8
#define XN297_CMD_WR_NO_ACK_PLOAD	0xB0
#define XN297_CMD_NOPOP						0xFF

#define XN297_REG_CONFIG			0x00
#define XN297_REG_EN_AA				0x01
#define XN297_REG_EN_RXADDR		0x02
#define XN297_REG_SETUP_AW		0x03
#define XN297_REG_SETUP_RETR	0x04
#define XN297_REG_RF_CH				0x05
#define XN297_REG_RF_SETUP		0x06
#define XN297_REG_STATUS			0x07
#define XN297_REG_OBSERVE_TX	0x08
#define XN297_REG_DATAOUT			0x09
#define XN297_REG_RX_ADDR_P0	0x0A
#define XN297_REG_RX_ADDR_P1	0x0B
#define XN297_REG_RX_ADDR_P2	0x0C
#define XN297_REG_RX_ADDR_P3	0x0D
#define XN297_REG_RX_ADDR_P4	0x0E
#define XN297_REG_RX_ADDR_P5	0x0F
#define XN297_REG_TX_ADDR			0x10
#define XN297_REG_RX_PW_P0		0x11
#define XN297_REG_RX_PW_P1		0x12
#define XN297_REG_RX_PW_P2		0x13
#define XN297_REG_RX_PW_P3		0x14
#define XN297_REG_RX_PW_P4		0x15
#define XN297_REG_RX_PW_P5		0x16
#define XN297_REG_FIFO_STATUS	0x17
#define XN297_REG_DM_CAL			0x19
#define XN297_REG_DYNPD				0x1C
#define XN297_REG_FEATURE			0x1D
#define XN297_REG_RF_CAL			0x1E
#define XN297_REG_BB_CAL			0x1F

#define BIND_CH		2

extern unsigned char BindOk;
extern unsigned char A7105_ReceiveOk;
extern unsigned char XN297_IsReceiveOk(void);

extern void XN297_RegInit(void);
extern void XN297_ReadReg(unsigned char ReadAddr, unsigned char *pData, unsigned long Length);
extern void XN297_ReceivedPacket(unsigned char *pData, unsigned long Length);
extern void XN297_SendPacket(unsigned char *pData, unsigned long Length);
extern void XN297_WriteRegMore(unsigned char *pData, unsigned long Length);
extern void XN297_WriteRegOne(unsigned char RegAddr, unsigned char RegValue);

