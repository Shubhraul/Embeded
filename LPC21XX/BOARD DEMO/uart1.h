#ifndef __UART1_h
#define __UART1_h

#define Fosc            12000000                    
#define Fcclk           (Fosc * 5)                  
#define Fcco            (Fcclk * 4)                 
#define Fpclk           (Fcclk / 2) * 1  
#define  UART1_BPS	9600	 //Set Baud Rate here

void  Init_UART1(void);
void  UART1_SendByte(unsigned char data);
void  UART1_SendStr(const unsigned char *str);

#endif

