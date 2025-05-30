#include<lpc214x.h>
void delay(unsigned int val){
	T0CTCR = 0x00;
	T0PR = (60000000*4)-1;
	T0TC=0;
	T0TCR = 0x01;
	while(T0TC < val);
	T0TCR = 0x00;
}
int main(){
	PINSEL0 &= 0xFFFFFFF0;
	IODIR0 |= 0x01;
	while(1){
		IOSET0 |= 0x01;
		delay(1);
		IOCLR0 |= 0x01; 
		delay(1);
	}  
}