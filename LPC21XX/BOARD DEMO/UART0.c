#include<LPC214x.h>
#include "uart0.h"

void  Init_UART0(void)					//This function setups UART0
{  
   unsigned int Baud16;
   PINSEL0&=0xFFFFFFF0;
   PINSEL0|=0x00000005;
   U0LCR = 0x83;		            // DLAB = 1
   Baud16 = (Fpclk / 16) / UART0_BPS;  
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
