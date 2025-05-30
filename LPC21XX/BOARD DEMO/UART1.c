#include<LPC214x.h>
#include "uart1.h"


void  Init_UART1(void)				   //This function setups UART1
{  
   unsigned int Baud16;
   PINSEL0&=0xFFF0FFFF;
   PINSEL0|=0x00050000;
   U1LCR = 0x83;		            // DLAB = 1
   Baud16 = (Fpclk / 16) / UART1_BPS;  
   U1DLM = Baud16 / 256;							
   U1DLL = Baud16 % 256;						
   U1LCR = 0x03;
}
				

void  UART1_SendByte(unsigned char data)		  //A function to send a byte on UART1
{  
   U1THR = data;				    
   while( (U1LSR&0x40)==0 );	    
}


void  UART1_SendStr(const unsigned char *str)		   //A function to send a string on UART1
{  
   while(1)
   {  
      if( *str == '\0' ) break;
      UART1_SendByte(*str++);	    
   }
}

