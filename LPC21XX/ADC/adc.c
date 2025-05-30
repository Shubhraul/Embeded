#include<lpc214x.h>
void initADC(){
	PINSEL1 &= ~(1<<29);
	PINSEL1 |= (1<<28);
	PINSEL2 = 0;
	IODIR1 = 0XFFFFFFFF;
	PCONP |= (unsigned long)(1<<12);   //Enable Power
	AD0CR &= ~((1<<16)|(7<<17));  //Disable Brust, 10/11 cycle
	AD0CR |= ((1<<3) | (7<<8) | (1<<21)); //Select ADC 3 , PCLK 8 , activate ADC
	AD0CR &= ~(1<<0);
	IOPIN1=0x0;
}
int main(){
	unsigned int val;
	initADC();
	while(1){
		AD0CR |= (1<<24);
		while(!(AD0GDR & (1<<31)));
		val = (AD0GDR >> 6 ) & 0x000003FF;
		IOPIN1 = val<<16;
		AD0CR &= ~(1<<24);
	}
}