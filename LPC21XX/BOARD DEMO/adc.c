#include<LPC214x.h>
#include "adc.h"


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

void ADC_Init()			//This function inits ADC peripheral
{
 PINSEL1&= 0xFAFFFFFF;
 PINSEL1|= 0x05000000;		// Enable AD0.1 and AD0.2
 AD0CR=0x00200E00;		// SEL = 1 	ADC0 channel 1	Channel 1
						// CLKDIV = Fpclk / 1000000 - 1 ;1MHz
						// BURST = 0   // CLKS = 0  // PDN = 1 
 						// START = 1   // EDGE = 0 (CAP/MAT)
} 
