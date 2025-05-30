#include <LPC21xx.h>
void delay(unsigned int t)
{
	unsigned int i,j;
	for(i=0;i<=t;i++)
		for(j=0;j<=200;j++);
}
int main()
{
	unsigned int i;
	PINSEL2 = 0;
	IODIR1 = 0xFFFFFFFF;
	while(1)
	{
		IOSET1 = i++;
		delay(2500);
		IOCLR1 = 0xFFFFFFFF;
	} 
}