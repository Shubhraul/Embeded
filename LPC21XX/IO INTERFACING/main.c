/**************************************************************************************************
		Platform: LPC2148 Development Board.
		
		Written by: Rohit Chauhan, NEX Robotics Pvt. Ltd.
		Edited By: Sachitanand Malewar, NEX Robotics Pvt. Ltd.
		Last Modification: 2010-30-08

		This application demonstrates simple IO interfacing on LPC2148 development board. 
		Compiled with: RealView MDK-ARM Version:4.12 

		Hardware Setup:-
        Insert LED jumpers D1-D4.
		Insert Switch jumpers SW1-SW4        

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


#include  <lpc214x.h>			   //Includes LPC2148 register definitions

#define LED1_ON() IO1SET=(1<<16)	 //Macro Functions to turn ON LED
#define LED2_ON() IO1SET=(1<<17)
#define LED3_ON() IO1SET=(1<<18)
#define LED4_ON() IO1SET=(1<<19)

#define LED1_OFF() IO1CLR=(1<<16)	 //Macro Functions to turn OFF LED
#define LED2_OFF() IO1CLR=(1<<17)
#define LED3_OFF() IO1CLR=(1<<18)
#define LED4_OFF() IO1CLR=(1<<19)

#define SW1 (IO0PIN & 0x00008000)	 //Macro Functions to read Push button switches on LPC2148 dev. board
#define SW2	(IO0PIN & 0x00002000)
#define SW3	(IO0PIN & 0x00001000)
#define SW4	(IO0PIN & 0x40000000)

                        

int  main(void)
{  
 PINSEL0 = 0x00000000;		// Enable GPIO on all pins
 PINSEL1 = 0x00000000;
 PINSEL2 = 0x00000000;

 IO0DIR = (0<<12) | (0<<13)	| (0<<15) | (0<<30);		// Set P0.12, P0.13, P0.15, P0.30 as Input to read switch status
 IO1DIR = (1<<19) | (1<<18) | (1<<17) | (1<<16);		// Set P1.16, P1.17, P1.18, P1.19 as Output	to control LEDs

 while(1)	
 { 
  if(!SW1) {LED1_ON();}		   //If switch 1 is pressed turn ON LED1
  else 	   {LED1_OFF();}	   //else turn it OFF	
  
  if(!SW2) {LED2_ON();}		   //If switch 1 is pressed turn ON LED1
  else 	   {LED2_OFF();}	   //else turn it OFF

  if(!SW3) {LED3_ON();}		   //If switch 1 is pressed turn ON LED1
  else 	   {LED3_OFF();}	   //else turn it OFF

  if(!SW4) {LED4_ON();}		   //If switch 1 is pressed turn ON LED1
  else 	   {LED4_OFF();}	   //else turn it OFF
 }
}
