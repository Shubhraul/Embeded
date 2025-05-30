#ifndef __UART0_h
#define __UART0_h

#define Fosc            12000000                    
#define Fcclk           (Fosc * 5)                  
#define Fcco            (Fcclk * 4)                 
#define Fpclk           (Fcclk / 2) * 1  
#define  UART0_BPS	9600	 //Set Baud Rate here

void  Init_UART0(void);
void  UART0_SendByte(unsigned char data);
void  UART0_SendStr(const unsigned char *str);

#endif
