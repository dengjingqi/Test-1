#include <stm32f0xx.h>
#include "System.h"
#include "SPI.h"
#include "RF_XN297.h"
#include  "main.h"

extern unsigned char RXOvertime;
BOOL XN297_Initialized;
unsigned char XN297Receive_DataBuffer[14];
const unsigned char XN297_RegAddrInitTable[13] = {
	XN297_CMD_FLUSH_TX,
	XN297_CMD_FLUSH_RX,
	XN297_REG_STATUS | XN297_CMD_WR_REG,
	XN297_REG_EN_AA | XN297_CMD_WR_REG,
	XN297_REG_EN_RXADDR | XN297_CMD_WR_REG,
	XN297_REG_SETUP_AW | XN297_CMD_WR_REG,
	XN297_REG_RF_CH | XN297_CMD_WR_REG,
	XN297_REG_SETUP_RETR | XN297_CMD_WR_REG,
	XN297_REG_RX_PW_P0 | XN297_CMD_WR_REG,
	XN297_REG_RF_SETUP | XN297_CMD_WR_REG,
	XN297_CMD_ACTIVATE,
	XN297_REG_DYNPD | XN297_CMD_WR_REG,
	XN297_REG_FEATURE | XN297_CMD_WR_REG
};

const unsigned char XN297_RegValueInitTable[13] = {
	0x00,
	0x00,
	0x70,
	0x00,
	0x01,
	0x01,
	0x02,
	0x00,
	0x0F,
	0x07,
	0x73,
	0x00,
	0x00	
};


void XN297_WriteRegMore(unsigned char *pData, unsigned long Length)
{
	SPI_TransmitOnly(pData, Length+1);
	return;
}

void XN297_WriteRegOne(unsigned char RegAddr, unsigned char RegValue)
{
	unsigned char pData[2];
	pData[0] = RegAddr;
	pData[1] = RegValue;
	SPI_TransmitOnly(pData, 2);
	return;
}

void XN297_WriteCmd(unsigned char *pData, unsigned long Length)
{
	SPI_TransmitOnly(pData, Length+1);
	return;	
}

void XN297_RegInit(void)
{
	unsigned char iCnt;
	unsigned char Data[10];
	
	XN297_DISABLE();
	SYS_DelayUs(10000);
	//	Set RX Address
	Data[0] = XN297_REG_RX_ADDR_P0 | XN297_CMD_WR_REG;
	Data[1] = 0xCC;Data[2] = 0xCC;Data[3] = 0xCC;Data[4] = 0xCC;Data[5] = 0xCC;
	XN297_WriteRegMore(Data, 5);
	for(iCnt=0; iCnt<10; iCnt++)
		Data[iCnt] = 0;
	SPI_ReadRegister(XN297_REG_RX_ADDR_P0, Data, 5);
	
	//	Set TX Address
	Data[0] = XN297_REG_TX_ADDR | XN297_CMD_WR_REG;
	Data[1] = 0xCC;Data[2] = 0xCC;Data[3] = 0xCC;Data[4] = 0xCC;Data[5] = 0xCC;
	XN297_WriteRegMore(Data, 5);
	for(iCnt=0; iCnt<10; iCnt++)
		Data[iCnt] = 0;
	SPI_ReadRegister(XN297_REG_TX_ADDR, Data, 5);
	
	//	Set DM_CAL
	Data[0] = XN297_REG_DM_CAL | XN297_CMD_WR_REG;
	Data[5] = 0x03;Data[4] = 0xA7;Data[3] = 0xC4;Data[2] = 0xDF;Data[1] = 0x0B;
	XN297_WriteRegMore(Data, 5);
	for(iCnt=0; iCnt<10; iCnt++)
		Data[iCnt] = 0;
	SPI_ReadRegister(XN297_REG_DM_CAL, Data, 5);
	
	//	Set RF_CAL
	Data[0] = XN297_REG_RF_CAL | XN297_CMD_WR_REG;
	Data[7] = 0x9C;Data[6] = 0xAB;Data[5] = 0xBB;Data[4] = 0x61;Data[3] = 0xB0;Data[2] = 0x9A;Data[1] = 0xC9;
	XN297_WriteRegMore(Data, 7);
	for(iCnt=0; iCnt<10; iCnt++)
		Data[iCnt] = 0;
	SPI_ReadRegister(XN297_REG_RF_CAL, Data, 7);
	
	//	Set BB_CAL
	Data[0] = XN297_REG_BB_CAL | XN297_CMD_WR_REG;
	Data[5] = 0x20;Data[4] = 0x9C;Data[3] = 0x67;Data[2] = 0x84;Data[1] = 0x4C;
	XN297_WriteRegMore(Data, 5);
	for(iCnt=0; iCnt<10; iCnt++)
		Data[iCnt] = 0;
	SPI_ReadRegister(XN297_REG_BB_CAL, Data, 5);
	
	
	for(iCnt=0; iCnt<13; iCnt++)
	{
		XN297_WriteRegOne(XN297_RegAddrInitTable[iCnt], XN297_RegValueInitTable[iCnt]);
	}	
	XN297_WriteRegOne(XN297_REG_CONFIG|XN297_CMD_WR_REG, 0x0F);
	XN297_WriteRegOne(XN297_REG_RF_CH|XN297_CMD_WR_REG, 50);	
	SYSCFG->EXTICR[2]|=SYSCFG_EXTICR3_EXTI8_PB;    //external PB8
	EXTI->IMR|=EXTI_IMR_MR8;
	EXTI->FTSR|=EXTI_FTSR_TR8;
	NVIC_SetPriority(EXTI4_15_IRQn,IRQ_PRI_HIGH);
	NVIC->ISER[0]|=1UL<<EXTI4_15_IRQn;
	XN297_ENABLE();
	XN297_Initialized=TRUE;
	return;
}

void EXTI4_15_IRQHandler(void)
{
	unsigned char Status;
	XN297_ENABLE();
	SPI_ReadRegister(XN297_REG_STATUS|XN297_CMD_RD_REG,&Status,1);
	if((Status&0x40)==0x40)
		SPI_ReadRegister(XN297_CMD_RD_RX_PLOAD, XN297Receive_DataBuffer, 14);
	return;
}

