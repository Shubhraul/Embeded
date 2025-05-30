#include<lpc214x.h>
int main(){
	PINSEL1 = 0x00000400;
	PWMPCR = 0x00;
	PWMPR = 60000-1;
	PWMMR0 = 20000;
	PWMMCR = (1<<1);
	PWMLER = (1<<0)|(1<<5);
	PWMPCR = (1<<13);
	PWMTCR = (1<<1);
	PWMTCR = (1<<0)|(1<<3);
	PWMMR5 = 650;
	PWMLER = (1<<5);
	while(1);
}