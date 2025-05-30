/**************************************************************************************************
		Platform: LPC2148 Development Board.
		
		Written by: Rohit Chauhan, NEX Robotics Pvt. Ltd.
		Edited By: Sachitanand Malewar, NEX Robotics Pvt. Ltd.
		Last Modification: 2010-30-09

		This application code explains reading and writing on I2C EEPROM 24LC04.
		It will write 16 butes of DATA on the EEPROM and read back the same data.
		The read data is then put on UART0.

		Compiled with: RealView MDK-ARM Version:4.12 

		Hardware Setup:-
        Insert LED jumpers D1-D4.
		Insert I2C jumpers SDA & SCL
		Connect a DB9 cable between PC and UART0.
		COMPORT Settings
		Baudrate:-9600
		Databits:-8
		Parity:-None
		Stopbits:1
		Setup terminal software to receive data in string format
        
		Clock Settings:
		FOSC	>>	12MHz (onboard)
		PLL		>>	M=5, P=2
		CCLK	>>  60MHz
		PCLK	>>  15MHz 
**************************************************************************************************/

/********************************************************************************

   Copyright (c) 2010, NEX Robotics Pvt. Ltd.                       -*- c -*-
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

   * Source code can be used for academic purpose. 
	 For commercial use permission form the author needs to be taken.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. 

  Software released under Creative Commence cc by-nc-sa licence.
  For legal information refer to: 
  http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode

********************************************************************************/ 


#include  <lpc214x.h>		   //Includes LPC2148 register definitions

#define MAX_BUFFER_SIZE 16
#define DEVICE_ADDR 0xA0
#define BLK_0 0x00
#define BLK_1 0x02
#define MAX_BLOCK_SIZE 256

#define LED1_ON() IO1SET=(1<<16)		//I2C write indicator
#define LED2_ON() IO1SET=(1<<17)		//I2C read indicator
#define LED3_ON() IO1SET=(1<<18)		//Comm failure indicator
#define LED4_ON() IO1SET=(1<<19)		//Comm success indicator

#define LED1_OFF() IO1CLR=(1<<16)	
#define LED2_OFF() IO1CLR=(1<<17)
#define LED3_OFF() IO1CLR=(1<<18)
#define LED4_OFF() IO1CLR=(1<<19)

#define Fosc            12000000                    
#define Fcclk           (Fosc * 5)                  
#define Fcco            (Fcclk * 4)                 
#define Fpclk           (Fcclk / 4) * 1            

#define  UART_BPS	9600	 //Set Baud Rate here



void Delay(unsigned char j);
void Init_UART0(void);
void UART0_SendByte(unsigned char data);
void UART0_SendStr(const unsigned char *str);
void Send_Start(void);
void Send_Stop(void);
unsigned char Send_I2C(unsigned char *Data,unsigned char Len);
unsigned char Read_I2C(unsigned char *Data,unsigned char Len);
unsigned char Page_Write(unsigned char BLOCK_NUMBER,unsigned char BLOCK_ADDR);
unsigned char Page_Read(unsigned int BLOCK_NUMBER,unsigned char BLOCK_ADDR);
unsigned char I2C_Status(unsigned char status_code);


unsigned char I2C_WR_Buf[MAX_BUFFER_SIZE]={"  Nex Robotics  "};
unsigned char I2C_RD_Buf[MAX_BUFFER_SIZE];
unsigned char Status=0;
unsigned char Status_Flag=0;

void  Delay(unsigned char j)
{  
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<60000; i++);
 } 
}



void  Init_UART0(void)
{  
   unsigned int Baud16;
   U0LCR = 0x83;		            // DLAB = 1
   Baud16 = (Fpclk / 16) / UART_BPS;  
   U0DLM = Baud16 / 256;							
   U0DLL = Baud16 % 256;						
   U0LCR = 0x03;
}
				

void UART0_SendByte(unsigned char data)
{  
   U0THR = data;				    
   while( (U0LSR&0x40)==0 );
   return;	    
}
                               

void  UART0_SendStr(const unsigned char *str)	 //A function to send a string on UART0
{  
   while(1)
   {  
      if( *str == '\0' ) break;
      UART0_SendByte(*str++);	    
   }
}


void Send_Start()
{
 I2C0CONSET=0x20; 
}


void Send_Stop()
{
 I2C0CONSET=0x10;
}



// This function sends sequential data to the EEPROM 24LC04
// The buffer size for EEPROM 24LC04 is 16 bytes
// The Len parameter should not exceed this value
unsigned char Send_I2C(unsigned char *Data,unsigned char Len)
{
 while(Len)
 {
  I2C0DAT=*Data;
  if(I2C_Status(0x28))
  {
   return 1;
  }
  Len--;
  Data++;
 }
 return 0;
}


// This function reads random data from the EEPROM 24LC04

unsigned char Read_I2C(unsigned char *Data,unsigned char Len)
{
 while(Len)
 {
  if(Len==1)  //Last byte
  {
   I2C0CONCLR=0x04;		 //set hardware to send nack
   if(I2C_Status(0x58))	//last byte has been received and NACK has been returned
   {
    return 1;
   }
   *Data=I2C0DAT;
  }
  else
  {
   I2C0CONSET=0x04;	 		//set hardware to send  ack
   if(I2C_Status(0x50))	//Byte has been received ACK has been returned
   {
    return 1;
   }
   *Data=I2C0DAT;	
  } 
  Data++;
  Len--;
 }
 return 0;
}


unsigned char I2C_Status(unsigned char status_code)
{
 while(Status_Flag==0);
 Status_Flag=0;
 if(Status!=status_code)
 {
  return 1;
 }
 else
 {
  return 0;
 }
}


unsigned char Page_Write(unsigned char BLOCK_NUMBER,unsigned char BLOCK_ADDR)
{
 Send_Start();
 if(I2C_Status(0x08))	//Start has been transmitted
 {
  return 1;
 }

 I2C0DAT=DEVICE_ADDR | BLOCK_NUMBER;	// Send Address
 if(I2C_Status(0x18))					//Device address, block num and write has been transmitted
 {
  return 1;
 }

 I2C0DAT=BLOCK_ADDR;	// Send block address
 if(I2C_Status(0x28))	//Block address has been transmitted
 {
  return 1;
 }

 if(Send_I2C(I2C_WR_Buf,MAX_BUFFER_SIZE))			//Send Data
 {
  Send_Stop();
  return 1;
 }
 Send_Stop();
 return 0;
}


unsigned char Page_Read(unsigned int BLOCK_NUMBER,unsigned char BLOCK_ADDR)
{
 Send_Start();
 if(I2C_Status(0x08))	//Start has been transmitted
 {
  return 1;
 }

 I2C0DAT=DEVICE_ADDR | BLOCK_NUMBER;	// Send Address
 if(I2C_Status(0x18))	//Device address, block num and write has been transmitted
 {
  return 1;
 }

 I2C0DAT=BLOCK_ADDR;
 if(I2C_Status(0x28))	//Block address has been transmitted
 {
  return 1;
 }

 Send_Start();		     // Repeat Start
 if(I2C_Status(0x10))	//Repeated Start has been transmitted
 {
  return 1;
 }

 I2C0DAT=DEVICE_ADDR | BLOCK_NUMBER | 0x01;			//Device address, block num and read has been transmitted
 if(I2C_Status(0x40))	//
 {
  return 1;
 }
 if(Read_I2C(I2C_RD_Buf,MAX_BUFFER_SIZE))			//Receive 16bytes of Data from EEPROM
 {
  Send_Stop();
  return 1;
 }
 Send_Stop();
 return 0;
}


void  __irq I2C0_Status(void)
{ 
  Status_Flag=0xFF; 			//update status flag
  Status=I2C0STAT;				//Read Status byte
  I2C0CONCLR=0x28;				
  VICVectAddr = 0x00;   		//Acknowledge Interrupt
}	


void I2C_Init()
{
 PINSEL0&=0xFFFFFF0F;
 PINSEL0|=0x00000050;

 I2C0CONCLR=0x6C;
 I2C0CONSET=0x40;
 I2C0SCLH=80;
 I2C0SCLL=70;

 /*  Init VIC for I2C0	*/
 VICIntSelect = 0x00000000;		// Setting all interrupts as IRQ(Vectored)
 VICVectCntl0 = 0x20 | 9;		// Assigning Highest Priority Slot to I2C0 and enabling this slot
 VICVectAddr0 = (unsigned long)I2C0_Status; // Storing vector address of I2C0
 VICIntEnable = (1<<9);	

}

int  main(void)
{  
 PINSEL0 = 0x00000005;		// Enable GPIO on all pins
 PINSEL1 = 0x00000000;
 PINSEL2 = 0x00000000;
 IO1DIR = (1<<19) | (1<<18) | (1<<17) | (1<<16);		// Set P1.16, P1.17, P1.18, P1.19 as Output
 LED1_OFF();LED2_OFF();LED3_OFF();LED4_OFF();
 Init_UART0();
 I2C_Init();
 
 LED1_ON();  //Write Indicator
 if(Page_Write(BLK_1,0x00))
 {
  UART0_SendStr("Write Failed");
  LED3_ON();
 }
 LED1_OFF();

 Delay(1);

 LED2_ON();	//Read indicator
 if(Page_Read(BLK_1,0x00))
 {
  UART0_SendStr("Read Failed");
  LED3_ON();
 }
 else
 {
  UART0_SendStr(I2C_RD_Buf);
  LED4_ON();
 }
 LED2_OFF();
 while(1)
 {
  
 }	
 
 
}

