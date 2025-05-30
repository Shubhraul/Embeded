/**************************************************************************************************
		Platform: LPC2148 Development Board.
		
		Written by: Rohit Chauhan, NEX Robotics Pvt. Ltd.
		Edited By: Sachitanand Malewar, NEX Robotics Pvt. Ltd.
		Last Modification: 2010-30-06

		This application code demostrates LCD interface on LPC2148 development board 

		Compiled with: RealView MDK-ARM Version:4.12 

		Hardware Setup:-
        Insert all LCD jumpers.
        
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

#include  <lpc214x.h>	  //Includes LPC2148 register definitions

#define DATA_PORT() IO0SET=(1<<16)		 //Function to select data port on LCD
#define READ_DATA() IO0SET=(1<<17)		 //Function to select read operation on LCD
#define EN_HI() IO0SET=(1<<18)			 //Function to Enable LCD


#define COMMAND_PORT() IO0CLR=(1<<16)	  //Function to select command port on LCD
#define WRITE_DATA() IO0CLR=(1<<17)		  //Function to select write operation on LCD
#define EN_LOW() IO0CLR=(1<<18)			  //Function to disable LCD

unsigned char String1[16]={"  Nex Robotics  "};
unsigned char String2[16]={"  ARM LPC2148   "};

void Delay(unsigned char j)
{  
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<60000; i++);
 } 
}

void Delay_Small(unsigned char j)
{
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<1000; i++);
 } 
}
                               

unsigned char Busy_Wait()			   //This function checks the busy status of LCD
{
 unsigned int temp=0;
 EN_LOW();
 COMMAND_PORT();
 READ_DATA();
 
 IO0PIN&=0xFF87FFFF;	  
 IO0DIR&=0xFF87FFFF;
 IO0PIN|=0x00400000;
 
 do{
 EN_HI();
 EN_LOW();
 EN_HI();
 EN_LOW();
 temp=IO0PIN;
 }
 while((temp & 0x00400000)==0x00400000);
 EN_LOW();
 WRITE_DATA();
 IO0DIR&=0xFF80FFFF;
 IO0DIR|=0x007F0000;
 return (0);
}    


void LCD_Command(unsigned int data)			  //This function is used to send LCD commands
{
 unsigned int temp=0;
 EN_LOW();
 COMMAND_PORT();
 WRITE_DATA();
 
 temp=data;
 IO0PIN&=0xFF87FFFF;
 IO0PIN|=(temp & 0xF0) << 15;

 EN_HI();
 EN_LOW();
 
 temp=data & 0x0F;
 IO0PIN&=0xFF87FFFF;
 IO0PIN|=(temp) << 19;

 EN_HI();
 EN_LOW();
 while(Busy_Wait());
 Delay(10);
} 


void LCD_Data(unsigned int data)		   //This function is used to send data to LCD
{
 unsigned int temp=0;
 EN_LOW();
 DATA_PORT();
 WRITE_DATA();
 
 temp=data;
 IO0PIN&=0xFF87FFFF;
 IO0PIN|=(temp & 0xF0) << 15;

 EN_HI();
 EN_LOW();
 
 temp=data & 0x0F;
 
 IO0PIN&=0xFF87FFFF;
 IO0PIN|=(temp) << 19;

 EN_HI();
 EN_LOW();
 Delay_Small(1);
}

void LCD_Init()
{
 LCD_Command(0x20);
 LCD_Command(0x28);
 LCD_Command(0x0C);
 LCD_Command(0x06);
}


void LCD_String(unsigned char *data)
{
 while(*data)
 {
  LCD_Data(*data);
  data++;
 } 
}  


int main(void)
{ 
 PINSEL0 = 0x00000000;		// Enable GPIO on all pins
 PINSEL1 = 0x00000000;
 PINSEL2 = 0x00000000;

 Delay(20);
 IO0DIR = (1<<22) | (1<<21) | (1<<20) | (1<<19) | (1<<18) | (1<<17) | (1<<16);		// Set P0.16, P0.17, P0.18, P0.19, P0.20, P0.21, P0.22 as Output
 IO1DIR = (1<<16);
 
 LCD_Init();
 LCD_Command(0x01);
 Delay(20);

 LCD_Command(0x80);
 LCD_String(&String1[0]);
 LCD_Command(0xC0);
 LCD_String(&String2[0]);
 while(1)	
 { 
  
   
 }
 
}


