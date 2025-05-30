#include <LPC21xx.h>
int main()
{
PINSEL0 &= 0xFFFFFFF0;
IODIR0 &= 0xFFFFFFFE;
IODIR0 |= 0x00000002;
while(1){
if((IOPIN0 & 0x00000001)==1){
	IOSET0 |= 0x00000002;
}
else{
	IOCLR0 |= 0x00000002;
}
}
}
