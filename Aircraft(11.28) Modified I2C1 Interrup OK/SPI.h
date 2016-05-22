void SPI_Deselect(void);
void SPI_FlushRXBuffer(void);
void SPI_Init(void);
void SPI_ReadRegister(unsigned char RegisterNb,void *pData,unsigned long DataLength);
void SPI_Select(void);
void SPI_TransmitOnly(const void *pData,unsigned long DataLength);
