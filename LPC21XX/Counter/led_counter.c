#include<lpc21xx.h>
void delay(int cnt){
	int i,j;
	for(i=0;i<cnt;i++){
		for(j=0;j<200;j++){}
	}
}
int main(){
	int status,prev_status,input=(1<<0),output=0xFFFFFFFF,count=0;
	PINSEL0 = 0x00000000;
	PINSEL2 = 0x00000000;
	IODIR0 = ~input;
	IODIR1 = output	;
	prev_status = status ;
	while(1){
		IOSET1 = count;
		status = (IOPIN0 & input)==input;
		if(status != prev_status){
			if(status==1){
				count++;
				IOCLR1 = 0xFFFFFFFF;
			}
			prev_status = status;
		}
	}
}