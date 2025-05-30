#include <lpc214x.h>
void initUART(){
	PINSEL0 |= 0x00000005; 
	U0LCR = 0x83;
	U0DLL = 97;
	U0LCR = 0x03; 
}
void initADC(){
	PINSEL1 &= ~(1<<19);
	PINSEL1 |= (1<<18);
	PCONP |= (unsigned long)(1<<12);   //Enable Power
	AD0CR &= ~((1<<16)|(7<<17));  //Disable Brust, 10/11 cycle
	AD0CR |= ((1<<4) | (7<<8) | (1<<21)); //Select ADC 4 , PCLK 8 , activate ADC
	AD0CR &= ~(1<<0);
}

int adcRead(){
	int val;
	AD0CR |= (1<<24);
	while(!(AD0GDR & (1<<31)));
	val = (AD0GDR >> 6 ) & 0x000003FF;
	AD0CR &= ~(1<<24);
	return val;
}

int transmit(char *ch,int N){
	int i;
	for(i=0;i<N;i++){
		while(!(U0LSR & 0x20));
		U0THR = ch[i];
	}
	return 0;
}

void trasmitADC(int num){
	int i,temp=num,N=0;
	char read[5];
	while(temp != 0 ){
	temp /= 10;
	N++;
	}
	for(i=N-1;i>=0;i--){
	read[i] = '0'+(num%10);
	num/= 10;			
	}
	transmit(read,N);								 	
}

int main(){
	int Status,flag=1,val, LED=(1<<17),Switch=(1<<6);
	PINSEL0 = 0;
	initUART();
	initADC();
	IODIR0 |= LED;
	IODIR0 &= ~Switch;
	while(1){
		Status = (IOPIN0 & Switch) == Switch;
		if(Status){
			if(flag){
			flag=0;
			val = adcRead();
			if(val>700){
				IOSET0 |= LED;
				transmit(" HIGH : ",8);
			}
			else if(val<400){
				IOCLR0 |= LED;
				transmit(" LOW : ",7);
			}else{
				transmit(" NORMAL : ",10);
			}
			trasmitADC(val);
			}
		}else{
			flag =1;
		}		
	} 
}