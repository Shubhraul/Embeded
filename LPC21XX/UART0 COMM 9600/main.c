/**************************************************************************************************
		Platform: LPC2148 Development Board.
		
		Written by: Rohit Chauhan, NEX Robotics Pvt. Ltd.
		Edited By: Sachitanand Malewar, NEX Robotics Pvt. Ltd.
		Last Modification: 2010-30-08

		This application code demonstrates UART0 peripheral on LPC2148.
		On reset it sends a string and waits for a character to be received.

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

#include  <lpc214x.h>		 //Includes LPC2148 register definitions

 
#define Fosc            12000000                    
#define Fcclk           (Fosc * 5)                  
#define Fcco            (Fcclk * 4)                 
#define Fpclk           (Fcclk / 4) * 1             

#define  UART_BPS	9600	 //Set Baud Rate here

const unsigned char SEND_STRING[] = "Nex Robotics PVT LTD\nARM7 LPC214x Development Board\nCommunication Test\nSend any character to continue\n";
const unsigned char SEND_STRING1[] = "Test Passed\n";


void  Delay_Ticks(unsigned int Delay)  //Function to generate finite delay
{  
   unsigned int i;
   for(; Delay>0; Delay--) 
   for(i=0; i<50000; i++);
}


void  Init_UART0(void)					//This function setups UART0
{  
   unsigned int Baud16;
   U0LCR = 0x83;		            // DLAB = 1
   Baud16 = (Fpclk / 16) / UART_BPS;  
   U0DLM = Baud16 / 256;							
   U0DLL = Baud16 % 256;						
   U0LCR = 0x03;
}
				

void  UART0_SendByte(unsigned char data)	   //A function to send a byte on UART0
{  
   U0THR = data;				    
   while( (U0LSR&0x40)==0 );	    
}


void  UART0_SendStr(const unsigned char *str)	 //A function to send a string on UART0
{  
   while(1)
   {  
      if( *str == '\0' ) break;
      UART0_SendByte(*str++);	    
   }
}


int  main(void)
{  
   PINSEL0 = 0x00000005;		    // Enable UART0 Rx and Tx pins
   PINSEL1 = 0x00000000;
   PINSEL2 = 0x00000000;

   Init_UART0();
   UART0_SendStr(SEND_STRING);
   while((U0LSR&0x01)==0);
   UART0_SendStr(SEND_STRING1);
   while(1)	
   {  
     
   }
   return(0);
}