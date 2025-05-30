#include <LPC21xx.h>
void delay(unsigned int t)
{
	unsigned int i,j;
	for(i=0;i<=t;i++)
		for(j=0;j<=200;j++);
}
int main()
{
unsigned int i=0,status=1;
PINSEL0 = 0x00000000;
PINSEL2 = 0;
IODIR0 &= 0xFFFFFFFE;
IODIR1 = 0xFFFFFFFF;
while(1){
if((IOPIN0 & 0x00000001)==1){
	if(status){
	IOCLR1 = 0xFFFFFFFF;
	IOSET1 = i++;
	delay(200);
	}
	status=0;
}
else{
status=1 ;
}
}
}
