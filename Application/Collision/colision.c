#include<lpc214x.h>
void delay(int N){
	int i,j;
	for(i=0;i<N;i++){
		for(j=0;j<3000;j++);
	}
}


void LCD_CMD(char cmd, int RS){
	IOPIN0 = ((IOPIN0 & 0xFFFF00FF)| (cmd & 0xF0)<<8);	 //MSB 8
	if(RS){ 
		IOSET0 = 0x00000050; // RS:1, EN:1
		IOCLR0 = 0x00000020; // RW:0
	}else{
		IOSET0 = 0x00000040; // RS:0, EN:1
		IOCLR0 = 0x00000030; // RW:0
	}
	delay(1);
	IOCLR0 = 0x00000040; //EN:0
	delay(2);
	IOPIN0 = ((IOPIN0 & 0xFFFF00FF)| (cmd & 0x0F)<<12);	  //LSB	8
	if(RS){ 
		IOSET0 = 0x00000050; // RS:1, EN:1
		IOCLR0 = 0x00000020; // RW:0
	}else{
		IOSET0 = 0x00000040; // RS:0, EN:1
		IOCLR0 = 0x00000030; // RW:0
	}
	delay(1);
	IOCLR0 = 0x00000040; //EN:0
	delay(2);
}

void LCD_Init(){
	IODIR0 = 0x0000FFF0; //p0.12 to p0.15 LCD Data p0.4,5,6 as RS, RW, En
	delay(20);
	LCD_CMD(0x02,0); //LCD $bit data mode
	LCD_CMD(0x28,0); // 2 line display
	LCD_CMD(0x0C,0); //Display ON Cursor OFF Blink OFF
	LCD_CMD(0x06,0); // Cursor Auto Increament
	LCD_CMD(0x01,0); //Display Clear
	LCD_CMD(0x80,0); //Cursor on First row first column
}

void LCD_String(char* str){
	int i =0;
	while(str[i]!=0){
		LCD_CMD(str[i],1);
		i++;
	}
}

void initUART(){
	PINSEL0 |= 0x00000005; 
	U0LCR = 0x83;
	U0DLL = 97;
	U0LCR = 0x03; 
}

int transmit(char *ch,int N){
	int i;
	for(i=0;i<N;i++){
		while(!(U0LSR & 0x20));
		U0THR = ch[i];
	}
	return 0;
}

void trasmitADC(int num,char* ch){
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
	LCD_String(read);
	transmit(ch,4);
	transmit(read,N);
	transmit("  ",2);								 	
}

void initADC(){
	PINSEL1 &= ~(1<<29);
	PINSEL1 |= (1<<28);
	PINSEL2 = 0;
	IODIR1 = 0XFFFFFFFF;
	PCONP |= (unsigned long)(1<<12);   //Enable Power
	AD0CR &= ~((1<<16)|(7<<17));  //Disable Brust, 10/11 cycle
	AD0CR |= ((1<<3) | (7<<8) | (1<<21)); //Select ADC 3 , PCLK 8 , activate ADC
	AD0CR &= ~(1<<0);
	IOPIN1=0x0;
}
int adcRead(){
	int val;
	AD0CR |= (1<<24);
	while(!(AD0GDR & (1<<31)));
	val = (AD0GDR >> 6 ) & 0x000003FF;
	AD0CR &= ~(1<<24);
	return val;
}

void Drive(int rotation,int speed){
	int i,j;
	for(i=0;i<rotation;i++){
		for(j=0;j<4;j++){
			IOPIN0 = 1<<(16+j);
			delay(speed);
		}
	}
}

int main(){
	unsigned int val;
	PINSEL0 = 0;
	PINSEL1 = 0;
	IODIR1 |= 0x000000FF;
	initADC();
	initUART();
	LCD_Init();
	IODIR0 |= 0x00FF0000;
	LCD_CMD(0x01,0);
	LCD_CMD(0xC0,0);
	LCD_String(" Anti-Colision ");
	while(1){
		val = adcRead();
		IOCLR1 = ((1<<16)|(1<<17)|(1<<18));
		if(val<150){
			IOSET1 |= (1<<16);
			LCD_CMD(0x80,0);
			LCD_String("-----Aleart-----");
			LCD_CMD(0x90,0);
			LCD_String(" -Object Ahead- ");
			LCD_CMD(0xD0,0);
			LCD_String("Distance: ");
			trasmitADC(val,"Val ");
			LCD_String("cm");
			delay(1000);
		}else if(val<350){
			LCD_CMD(0x80,0);
			LCD_String("-----Warning----");
			LCD_CMD(0x90,0);
			LCD_String(" -Object Ahead- ");
			LCD_CMD(0xD0,0);
			LCD_String("Distance: ");
			trasmitADC(val,"Val ");
			LCD_String("cm");
			IOSET1 |=(1<<17);
			Drive(1,700);
		}else{
			LCD_CMD(0x01,0);
			LCD_CMD(0xC0,0);
			LCD_String(" Anti-Colision ");
			LCD_CMD(0x90,0);
			LCD_String("   -ALL GOOD-   ");
			IOSET1 = (1<<18);
			Drive(1,300);
		}
	}
}