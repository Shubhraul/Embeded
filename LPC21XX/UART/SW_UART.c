#include <lpc21xx.h>
int transmit(char ch){
	while(!(U0LSR & 0x20));
	U0THR = ch;
	return 0;
}
int main(){
	char str[2] = {'A','B'};
	int SW1 = (1<<2), SW2 = (1<<3),flag=1,status1,status2,prev1,prev2;
	PINSEL0 = 0x00000005;
	IODIR0 &= ~(SW1 | SW2);
	U0LCR = 0x83;
	U0DLL = 97;
	U0LCR = 0x03;
	prev1 = 0;
	prev2 = 0;
	while(1){
		status1 = (IOPIN0 & SW1 )==SW1;
		status2 = (IOPIN0 & SW2 )==SW2;
		if(status1 != prev1){
			if(status1==1){
				transmit(str[0]);
			}
			prev1 = status1;
		}
		else if(status2 != prev2){
			if(status2==1){
				transmit(str[1]);
				}
			prev2 = status2;  
		}
	}
}