/**************************************************************************************************
		Platform: LPC2148 Development Board.
		
		Written by: Rohit Chauhan, NEX Robotics Pvt. Ltd.
		Edited By: Sachitanand Malewar, NEX Robotics Pvt. Ltd.
		Last Modification: 2010-30-08

		This application code controls Buzzer toggling speed using Switch SW4.
		Switch SW4 is used to trigger Ext interrupt EINT3
		Compiled with: RealView MDK-ARM Version:4.12 

		Hardware Setup:-
        Insert SW4 Jumper
		Insert Buzzer Jumper
        
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


#include  <lpc214x.h>					 //Includes LPC2148 register definitions


#define BUZZER_ON() IO0CLR=(1<<11)		 //Macro Functions to turn ON Buzzer
#define BUZZER_OFF() IO0SET=(1<<11)		 //Macro Functions to turn OFF Buzzer

void  __irq IRQ_Eint3(void);			 //Prototype function for EINT3 ISR
void Debounce_Delay(unsigned char j);
void Buzzer_Delay(unsigned char j);


unsigned char Count=40;

void Buzzer_Delay(unsigned char j)		 //This function is used to generate delay events between buzzer toggling
{  
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<60000; i++);
 } 
}

void Debounce_Delay(unsigned char j)	//This delay function is used to compensate switch debounci
{  
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<60000; i++);
 } 
}
                               
void INT3_Setup()		 //This function setups EINT3
{
 
 EXTMODE = (1<<3);	// EINT3 is edge sensitive
 EXTPOLAR = (0<<3);					// EINT3 in triggered on falling edge

 VICIntSelect = 0x00000000;		// Setting EINT3 as IRQ(Vectored)
 VICVectCntl0 = 0x20|17;		// Assigning Highest Priority Slot to EINT3 and enabling this slot
 VICVectAddr0 = (int)IRQ_Eint3; // Storing vector address of EINT3
 EXTINT = 0x08;	//Clearing EINT3 flag	
 VICIntEnable = (1<<17);	// Enable EINT3
}


int  main(void)
{  
 PINSEL0 = 0x00000000;		// Enable GPIO on all pins
 PINSEL1 = 0x20000000;
 PINSEL2 = 0x00000000;

 IO0DIR = (1<<11) ;  // Set P0.11 as Output	to control buzzer
 INT3_Setup();		
 while(1)	
 { 
  BUZZER_ON();
  Buzzer_Delay(Count);	 //The count value is updated in the EINT3 ISR
  BUZZER_OFF();
  Buzzer_Delay(Count);
 }
}



void  __irq IRQ_Eint3(void)		//EINT3 ISR
{  
   Count-=5;					//Count value is decremented by 5 every time a switch is pressed
   if(Count<5)					//if count value underflows update it
   { 
    Count=40;
   }
   Debounce_Delay(5);
   EXTINT = 0x08;				// Clear EINT3 flag
   VICVectAddr = 0x00;   		//Acknowledge Interrupt
}		