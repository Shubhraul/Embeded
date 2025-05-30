/**************************************************************************************************
		Platform: LPC2148 Development Board.
		
		Written by: Rohit Chauhan, NEX Robotics Pvt. Ltd.
		Edited By: Sachitanand Malewar, NEX Robotics Pvt. Ltd.
		Last Modification: 2010-30-06

		This application code demonstrates RTC peripheral on LPC2148.
		It transmits current time in 24h format on UART0
	    To update current time set HOUR, MIN, SEC registers	
		in the Initialize function and load the hex file.
		After updating recomment this registers to retain the current time. 
 
		Compiled with: RealView MDK-ARM Version:4.12 

		Hardware Setup:-
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

#include <lpc214x.h>	 //Includes LPC2148 register definitions

/* Mask values used while reading from Consolidated Time Registers */
#define MASKSEC 0x3f
#define MASKMIN 0x3f00
#define MASKHR 0x1f0000

#define Fosc            12000000                    
#define Fcclk           (Fosc * 5)                  
#define Fcco            (Fcclk * 4)                 
#define Fpclk           (Fcclk / 4) * 1             

#define  UART_BPS	9600	 //Set Baud Rate here


void read_rtc(void)__irq;
void Initialize(void);
void Init_UART0(void);
void UART0_SendByte(unsigned char data);
void UART0_SendStr(const unsigned char *str);


int hour=0;
int min=0;
int sec=0;
unsigned char Temp[2];

void DECtoASCII(unsigned char Data)		 //This function converts decimal data into ASCII
{
 Temp[1]=(Data % 10) + 48;
 Temp[0]=(Data / 10) + 48;
}



int main (void) 
{
 int i;
 PINSEL0=0x00000005;
 PINSEL1=0x00000000;
 PINSEL2=0x00000000;

 Initialize();	/* Initialize System */
 Init_UART0();	/* Initialize Serial port */
 UART0_SendStr("Nex Robotics PVT LTD\nARM7 LPC214x Development Board\nRTC TEST\n");
 CCR=0x11; 	/* Start RTC */
 PCONP=0x1815BE;	/* Disconnect pclk to RTC */
 
 while (1)
 {
 }
}


/* --------------------- Initialize --------------------- */
void Initialize()
{
 /* VPBDIV setting */
 VPBDIV=0x0;
 /* Intialize RTC */
 CCR=0x2; /* Reset the clock */
 ILR=0x3; /* Clear the Interrupt Location Register */
 HOUR=1;	   //Enter current time here 
 SEC=50;
 MIN=59;
 /* Initialize Vectored Interrupt Controller (VIC) */
 CIIR=0x07;
 VICIntSelect=0x000;
 VICIntEnable =0x2000;
 VICVectCntl0=0x2d;
 VICVectAddr0=(unsigned long )read_rtc;
}


/* ----------------- RTC Interrupt Service Routine ----------------- */
void read_rtc() __irq
{
 ILR=0x1;			/* Clearing Interrupt */
 hour=(CTIME0 & MASKHR)>>16;		/* Reading Consolidated Time registers */
 min= (CTIME0 & MASKMIN)>>8;
 sec=CTIME0 & MASKSEC;

 /* Displaying time on Serial port */
 UART0_SendStr("Current Time is "); 
 DECtoASCII(hour);
 UART0_SendStr(Temp);
 UART0_SendStr(":");
 DECtoASCII(min);
 UART0_SendStr(Temp);
 UART0_SendStr(":");
 DECtoASCII(sec);
 UART0_SendStr(Temp);
 UART0_SendStr("\n");
 
 /* Updating VIC */
 VICVectAddr=0xff;
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

