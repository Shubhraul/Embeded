#ifndef __EEPROM_H
#define __EEPROM_H

#define MAX_BUFFER_SIZE 16
#define DEVICE_ADDR 0xA0
#define BLK_0 0x00
#define BLK_1 0x02
#define MAX_BLOCK_SIZE 256

void Send_Start(void);
void Send_Stop(void);
unsigned char Send_I2C(unsigned char *Data,unsigned char Len);
unsigned char Read_I2C(unsigned char *Data,unsigned char Len);
unsigned char I2C_Status(unsigned char status_code);
unsigned char Page_Write(unsigned char BLOCK_NUMBER,unsigned char BLOCK_ADDR);
unsigned char Page_Read(unsigned int BLOCK_NUMBER,unsigned char BLOCK_ADDR);
void  __irq I2C0_Status(void);
void I2C_Init(void);

#endif
