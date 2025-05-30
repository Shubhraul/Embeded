/**************************************************************************************************
		Platform: LPC2148 Development Board.
		
		Written by: Rohit Chauhan, NEX Robotics Pvt. Ltd.
		Edited By: Sachitanand Malewar, NEX Robotics Pvt. Ltd.
		Last Modification: 2010-30-08

		This application code demonstrates SD/MMC interface on SPI bus. 
		Compiled with: RealView MDK-ARM Version:4.12 

		Hardware Setup:-
        Insert SPI jumpers SCK,SDO,SDI and /CS.
		Insert SD/MMC card into the socket
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
		PCLK	>>  30MHz 
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

#include <LPC214X.H> /* LPC214x definitions */
#include "type.h"
#include "spi_mmc.h"
#include "mmcmain.h"

#define Fosc            12000000                    //10MHz~25MHz
#define Fcclk           (Fosc * 5)                  //Fosc(1~32)<=60MHZ
#define Fcco            (Fcclk * 4)                 //CCO Fcclk 2񅦰�16�156MHz~320MHz
#define Fpclk           (Fcclk / 2) * 1             //VPB(Fcclk / 4) 1񄿔

#define  UART_BPS	9600	 //Set Baud Rate here

extern BYTE MMCWRData[MMC_DATA_SIZE];
extern BYTE MMCRDData[MMC_DATA_SIZE];
extern BYTE MMCStatus;



/* Main Program */

void  Delay_Ticks(unsigned int Delay)
{  
   unsigned int i;
   for(; Delay>0; Delay--) 
   for(i=0; i<50000; i++);
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
				

void  UART0_SendByte(unsigned char data)
{  
   U0THR = data;				    
   while( (U0LSR&0x40)==0 );	    
}


void  UART0_SendStr(const unsigned char *str)
{  
   while(1)
   {  
      if( *str == '\0' ) break;
      UART0_SendByte(*str++);	    
   }
}


int main (void)
{
 DWORD i, BlockNum = 0;
 PINSEL0 = 0x00000005;		    // UART0
 PINSEL1 = 0x40004000;
 PINSEL2 = 0x00000000;

 IODIR1 = LED_MSK; /* LED's defined as Outputs */
 SPI_Init(); /* initialize SPI for MMC card */
 Init_UART0();
 
 IOSET1 = LED_CFG;
 if ( mmc_init() != 0 )
 {
  IOSET0 = SPI_SEL; /* set SSEL to high */
  IOCLR1 = LED_MSK;
  UART0_SendStr("Init Failed");
  while ( 1 ); /* Very bad happened */
 }
 
 IOCLR1 = LED_MSK;
 IOSET1 = LED_WR;
 if ( mmc_write_block(0) == 0 )
 {
  //UART0_SendStr("Write Complete");
 }
 else
 {
  UART0_SendStr("Write Failed");
  while(1);
 }

 IOCLR1 = LED_MSK;
 IOSET1 = LED_RD;
 if(mmc_read_block(0)==0)
 {
  UART0_SendStr(MMCRDData);
  IOCLR1 = LED_MSK;
  IOSET1 = LED_DONE;
 }
 else
 {
  UART0_SendStr("Read Failed");
  while(1);
 }
 
 while (1); /* Loop forever */
}


