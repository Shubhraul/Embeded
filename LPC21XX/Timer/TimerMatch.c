#include<lpc214x.h>
void initExtTimer(void);
__irq void ISR(void);
void initExtTimer(){
	T0CTCR = 0;
	T0PR = 60000-1;
	T0MR0 = 500-1;
	T0MCR = 0x03;
	VICVectAddr4 = (unsigned int)ISR;
	VICVectCntl4 = (1<<5)|4;
	VICIntEnable = (1<<4);
	T0TCR = 0x02; 
}

__irq void ISR(){
	long int reg;
	reg = T0IR;
	IOPIN0 = ~IOPIN0;
	T0IR = reg;
	VICVectAddr = 0;
}

int main(){
	PINSEL0 = 0;
	initExtTimer();
	IODIR0 = 0xFFFFFFFF;
	IOPIN0 = 0;
	T0TCR = 0x01;
	while(1);
}