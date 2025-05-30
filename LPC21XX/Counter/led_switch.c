#include<LPC21xx.h>
void delay(int cnt){
int i,j;
for(i=1;i<cnt;i++){
	for(j=1;j<2000;j++){}
	}
}
int main(){
int status,input = (1<<16)| (1<<17), output = (1<<15);
PINSEL2 = 0x00000000;
PINSEL0 = 0x00000000;
IODIR0 &= output;
IODIR1 |= ~input;
while(1){
	status = (IOPIN1 & input)==input;
	if(status){
	IOSET0 = output;
	}
	else{
	IOCLR0 = output;
	}
}
}