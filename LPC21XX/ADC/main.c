/**************************************************************************************************
		Platform: LPC2148 Development Board.
		
		Written by: Rohit Chauhan, NEX Robotics Pvt. Ltd.
		Edited By: Sachitanand Malewar, NEX Robotics Pvt. Ltd.
		Last Modification: 2010-30-08

		This application code demonstrates ADC peripheral on LPC2148.
		Trimpots are used as source for analog signal
		Digital data is tranmitted on UART0(9600)
		 
		Compiled with: RealView MDK-ARM Version:4.12 

		Hardware Setup:-
        Insert trimpot jumpers AN1 & AN2
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

#include  <lpc214x.h>	 //Includes LPC2148 register definitions

#define Fosc            12000000                    
#define Fcclk           (Fosc * 5)                 
#define Fcco            (Fcclk * 4)                 
#define Fpclk           (Fcclk / 4) * 1              

#define  UART_BPS	9600			 //Set buadrate here

void DECtoASCII(unsigned int Data);
void Delay(unsigned char Ticks);
void Init_UART0(void);
void UART0_SendByte(unsigned char data);
void ADC_Init(void);
void  UART0_SendByte(unsigned char data);

unsigned int ADC_Data1=0;
unsigned int ADC_Data2=0;
unsigned char Temp[4];
double Data;


void DECtoASCII(unsigned int Data)		 //This function converts decimal data into ASCII
{
 Temp[3]='V';
 Temp[2]=(Data % 10) + 48; 
 Temp[1]='.';
 Temp[0]=(Data / 10) + 48;
}


void  Delay(unsigned char Ticks)	  //Function to generate small delay
{  
 unsigned int i=0;
 if(Ticks==0)
 {
  Ticks=1;
 }

 for(;Ticks>0;Ticks--)
 for(i=0; i<60000; i++);
}

void  Init_UART0(void)				 //This function setups UART0. Refer datasheet for more details
{  
   unsigned int Baud16;
   U0LCR = 0x83;		            // DLAB = 1
   Baud16 = (Fpclk / 16) / UART_BPS;  
   U0DLM = Baud16 / 256;							
   U0DLL = Baud16 % 256;						
   U0LCR = 0x03;
}

void  UART0_SendByte(unsigned char data)   //Function to send a byte on UART0
{  
   U0THR = data;				    
   while( (U0LSR&0x40)==0 );	    
}

void  UART0_SendStr(unsigned char *str)	 //A function to send a string on UART0
{  
   while(1)
   {  
      if( *str == '\0' ) break;
      UART0_SendByte(*str++);	    
   }
}


void ADC_Init()			//This function inits ADC peripheral
{
 AD0CR=0x00200E00;		// SEL = 1 	ADC0 channel 1	Channel 1
						// CLKDIV = Fpclk / 1000000 - 1 ;1MHz
						// BURST = 0   // CLKS = 0  // PDN = 1 
 						// START = 1   // EDGE = 0 (CAP/MAT)
} 
                           

unsigned char ADC_Conversion(unsigned char Channel)
{
 unsigned int Temp=0;
 AD0CR = (AD0CR&0xFFFFFF00) | Channel;			   //Select AD0.1 for conversion
 AD0CR|=(1 << 24);							   //Trigger conversion
 while((AD0DR1&0x80000000)==0);			   //Wait for the conversion to be completed
 Temp = AD0DR1;						   //Store converted data
 Temp = (Temp>>8) & 0x00FF;
 return(Temp);
}


int main(void)
{  
 PINSEL0 = 0x00000005;		// Enable UART0 Rx and Tx pins
 PINSEL1 = 0x05000000;		// Enable AD0.1 and AD0.2
 PINSEL2=  0x00000000;
 //dac
PINSEL1=PINSEL1|0x80000;  //Enable Aout
 Init_UART0();
 ADC_Init();
  
 while(1)	
 { 
  AD0CR = (AD0CR&0xFFFFFF00)|0x02;			   //Select AD0.1 for conversion
  AD0CR|=(1 << 24);							   //Trigger conversion
  while((AD0DR1&0x80000000)==0);			   //Wait for the conversion to be completed
  DACR=	AD0DR1;
  ADC_Data1 = AD0DR1;						   //Store converted data
  ADC_Data1 = (ADC_Data1>>8) & 0x00FF;
  
  Delay(1);

  AD0CR = (AD0CR & 0xFFFFFF00) | 0x04 ;		   //Select AD0.2 for conversion
  AD0CR|=(1 << 24);							   //Trigger conversion
  while((AD0DR2&0x80000000)==0);			   //Wait for the conversion to be completed
  ADC_Data2 = AD0DR2;						   //Store converted data
  ADC_Data2 = (ADC_Data2>>8) & 0x00FF;
  
  //UART0_SendByte('\r');						   //Send carriage return to display data on new line
  
  Data=(double)ADC_Data1 * 0.0129 * 10.0;	   //Convert Digital data to represent Analog value
  											   //(x/255)*3.3=0.0129x 
  DECtoASCII((unsigned int)Data);
  UART0_SendStr("AN1=");
  UART0_SendStr(Temp);
  UART0_SendByte(' ');

  Data=(double)ADC_Data2 * 0.0129 * 10.0;
  DECtoASCII((unsigned int)Data);
  UART0_SendStr("AN2=");
  UART0_SendStr(Temp);
  UART0_SendByte('\n');
  
  
 }
 
}
