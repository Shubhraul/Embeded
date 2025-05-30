/**************************************************************************************************
		Platform: LPC2148 Development Board.
		
		Written by: Rohit Chauhan, NEX Robotics Pvt. Ltd.
		Edited By: Sachitanand Malewar, NEX Robotics Pvt. Ltd.
		Last Modification: 2010-30-08

		This application demonstrates how to use L293D for controlling DC motor.
		SW1 is used to Start/Stop Motor1 connected across terminals A & B on the L293D o/p header
		SW2 is used to control direction of Motor1

		SW3 is used to Start/Stop Motor2 connected across terminals C & D on the L293D o/p header
		SW4 is used to control direction of Motor2

		Compiled with: RealView MDK-ARM Version:4.12 

		Hardware Setup:-
        Insert LED jumpers D1-D4.
		Insert Switch jumpers SW1-SW4 
		Insert PWM jumpers for +5V 
		Connect Motor i/p supply(12V) to the V pin on L293D header     

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

#include  <lpc214x.h>

#define RUN 1
#define STOP 0

#define FORWARD 1
#define REVERSE 0

#define SW1 (IO0PIN & 0x00008000)	 //Macro Functions to read Push button switches on LPC2148 dev. board
#define SW2	(IO0PIN & 0x00002000)
#define SW3	(IO0PIN & 0x00001000)
#define SW4	(IO0PIN & 0x40000000)

#define M1_MASK 0x00090000
#define M2_MASK 0x00060000

void  Delay(unsigned char j)
{  
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<60000; i++);
 } 
}
                               
void M1_Forward()
{
 IO1CLR=M1_MASK;
 IO1SET=0x00080000;
}

void M1_Reverse()
{
 IO1CLR=M1_MASK;
 IO1SET=0x00010000;
}

void M1_Stop()
{
 IO1CLR=M1_MASK;
}

void M2_Forward()
{
 IO1CLR=M2_MASK;
 IO1SET=0x00040000;
}

void M2_Reverse()
{
 IO1CLR=M2_MASK;
 IO1SET=0x00020000;
}

void M2_Stop()
{
 IO1CLR=M2_MASK;
}

int main(void)
{
 unsigned char M1_Status=0;
 unsigned char M2_Status=0;
 unsigned char M1_DIR=0;
 unsigned char M2_DIR=0; 
  
 PINSEL0 = 0x00000000;		// Enable GPIO on all pins
 PINSEL1 = 0x00000000;
 PINSEL2 = 0x00000000;
 
 IO1DIR = (1<<19) | (1<<18) | (1<<17) | (1<<16);		// Set P1.16, P1.17, P1.18, P1.19 as Output
 IO0DIR = (0<<12) | (0<<13)	| (0<<15) | (0<<30);		// Set P0.12, P0.13, P0.15, P0.30 as Input to read switch status
 
 while(1)	
 { 
  if(!SW1)
  {
   if(M2_Status==STOP)	{M2_Status=RUN;}
   else					{M2_Status=STOP;}
  }

  if(!SW3)
  {
   if(M1_Status==STOP)	{M1_Status=RUN;}
   else					{M1_Status=STOP;}
  }

  if(!SW2)
  {
   if(M2_DIR==REVERSE){M2_DIR=FORWARD;}
   else{M2_DIR=REVERSE;}
  }

  if(!SW4)
  {
   if(M1_DIR==REVERSE){M1_DIR=FORWARD;}
   else{M1_DIR=REVERSE;}
  }

  	
  if(M1_Status==STOP){M1_Stop();}
  else if(M1_DIR==FORWARD){M1_Forward();}
  else if(M1_DIR==REVERSE){M1_Reverse();}

  if(M2_Status==STOP){M2_Stop();}
  else if(M2_DIR==FORWARD){M2_Forward();}
  else if(M2_DIR==REVERSE){M2_Reverse();}
  
  Delay(50);

 }
 return(0);
}
