#include<LPC214x.h>
#include "rtc.h"

unsigned int hour=0;
unsigned int min=0;
unsigned int sec=0;

unsigned char RTC[2];

void Init_RTC()
{
 //VPBDIV=0x0;
 CCR=0x2; /* Reset the clock */
 ILR=0x3; /* Clear the Interrupt Location Register */
 //HOUR=18;	   //Enter 
 //SEC=0x0;
 //MIN=58;
 CIIR=0x07;			   /* Initialize Vectored Interrupt Controller (VIC) */
 VICIntSelect=0x000;
 VICIntEnable =0x2000;
 VICVectCntl1=0x2d;
 VICVectAddr1=(unsigned long )read_rtc;

 CCR=0x11; 	/* Start RTC */
 PCONP=0x1815BE;	/* Disconnect pclk to RTC */
}


/* ----------------- RTC Interrupt Service Routine ----------------- */
void read_rtc() __irq
{
 ILR=0x1;			/* Clearing Interrupt */
 hour=(CTIME0 & MASKHR)>>16;		/* Reading Consolidated Time registers */
 min= (CTIME0 & MASKMIN)>>8;
 sec=CTIME0 & MASKSEC;
 
 /* Updating VIC */
 VICVectAddr=0xff;
}


void RTC_DECtoASCII(unsigned char Data)		 //This function converts decimal data into ASCII
{
 RTC[1]=(Data % 10) + 48;
 RTC[0]=(Data / 10) + 48;
}

