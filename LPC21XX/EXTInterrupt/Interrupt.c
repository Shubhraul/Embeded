#include<lpc214x.h>
__irq void ISR(void);

void initEXTINT(){
	PINSEL0 = 0;
	PINSEL0 |= (1<<29);
	PINSEL0 &= ~(1<<28);
	EXTMODE |= (1<<1);
	EXTPOLAR |= (1<<1);
	EXTINT |= (1<<1);
	VICIntSelect &= ~(1<<15);
	VICIntEnable = (1<<15);
	VICVectCntl5 = (1<<5)|15;
	VICVectAddr5 = (unsigned int)ISR;
}

void delay(int N){
	int i,j;
	for(i=0;i<N;i++){									 
		for(j=0;j<=400;j++);
	}
}

void ISR()__irq{
	IOSET0 = 0x01;
	delay(500);
	IOCLR0 = 0x01;
	delay(500);
	EXTINT |= (1<<1);
	VICVectAddr = 0;
}

int main(){
	initEXTINT();
	IODIR0 |= (1<<0);
	while(1);
}