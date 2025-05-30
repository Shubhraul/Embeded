#include<lpc214x.h>
void initExtTimer(void);
__irq void ISR(void);
int Sensor[5];

void delay(int N){
	int i,j;
	for(i=0;i<N;i++){
		for(j=0;j<6000;j++);
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

void initADC(){
	PINSEL1 &= ~(1<<19);
	PINSEL1 |= (1<<18);
	PCONP |= (unsigned long)(1<<12);   //Enable Power
	AD0CR &= ~((1<<16)|(7<<17));  //Disable Brust, 10/11 cycle
	AD0CR |= ((7<<8) | (1<<21)); //Select ADC 4 , PCLK 8 , activate ADC
	AD0CR &= ~(1<<0);
}

int adcRead(int pin){
	int val;
	AD0CR &= 0xFFFFFF00; 
	AD0CR |= (1<<pin+1);
	AD0CR |= (1<<24);
	while(!(AD0GDR & (1<<31)));
	val = (AD0GDR >> 6 ) & 0x000003FF;
	AD0CR &= ~(1<<24);
	return val;
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
	transmit(ch,2);
	transmit(read,N);
	transmit("  ",2);								 	
}

void initExtTimer(){
	T0CTCR = 0;
	T0PR = 60000-1;
	T0MR0 = 50-1;
	T0MCR = 0x03;
	VICVectAddr4 = (unsigned int)ISR;
	VICVectCntl4 = (1<<5)|4;
	VICIntEnable = (1<<4);
	T0TCR = 0x02; 
}

void Drive(int Rotation,int Pin){
	int i,j;
	for(i=0;i<Rotation;i++){
		for(j=0;j<4;j++){
			IOPIN0 = 1<<(Pin+j);
			delay(50);
		}
	}
}

int CheckTemp(){
	IOCLR1 = ((1<<22)|(1<<23)|(1<<24));
	if(Sensor[0]>800){
		IOSET1 = (1<<22);
		Drive(1,16);
		return 2;
	}else if(Sensor[0]>450 & Sensor[0]<800){
		IOSET1 = (1<<23);
		return 1;	
	}else if(Sensor[0]<=450){
		IOSET1 = (1<<24);
	}
	return 0;
}

int CheckFuel(){
	IOCLR1 = ((1<<19)|(1<<20)|(1<<21));
	if(Sensor[1]<=210){
		IOSET1 = (1<<19);
		return 2;
	}else if(Sensor[1]>210 & Sensor[1]<500){
		IOSET1 = (1<<20);
		return 1;	
	}else if(Sensor[1]>=500){
		IOSET1 = (1<<21);
	}
	return 0;
}

int CheckSpeed(){
	IOCLR1 = ((1<<16)|(1<<17)|(1<<18));
	if(Sensor[2]>=850){
		IOSET1 = (1<<16);
		return 2;
	}else if(Sensor[2]>680 & Sensor[2]<850){
		IOSET1 = (1<<17);
		return 1;	
	}else if(Sensor[2]<=680){
		IOSET1 = (1<<18);
	}
	return 0;
}

int CheckBattery(){
	IOCLR1 = ((1<<25)|(1<<26)|(1<<27));
	if(Sensor[3]<=250){
		IOSET1 = (1<<25);
		return 2;
	}else if(Sensor[3]>250 & Sensor[3]<390){
		IOSET1 = (1<<26);
		return 1;	
	}else if(Sensor[3]>=390){
		IOSET1 = (1<<27);
	}
	return 0;
}

void UpdateLED(){
	int i,max=0,C=0;
	C=CheckTemp();
	if(C>max){max=C;}
	C=CheckFuel();
	if(C>max){max=C;}
	C=CheckSpeed();
	if(C>max){max=C;}
	C=CheckBattery();
	if(C>max){max=C;}
	LCD_CMD(0x80,0);
	LCD_String("                ");
	if(max==2){
		LCD_CMD(0x80,0);
		LCD_String("!!!!Critical!!!!");
	}else if(max==1){
		LCD_CMD(0x80,0);
		LCD_String("    Warning     ");
	}else if(max==0){
		LCD_CMD(0x80,0);
		LCD_String("    All Good!   ");
	}
}

void Update(){
	LCD_CMD(0x92,0);
	trasmitADC(1+(Sensor[3])*100/700,"B:"); 		   //Battery
	LCD_String("% ");
	LCD_CMD(0x9B,0);
	trasmitADC(2+((Sensor[0]*100)/1050),"T:");	  //Temprature
	LCD_String("'c");
	LCD_CMD(0xD2,0);
	trasmitADC(1+(Sensor[1]*100)/1030,"F:");		  //Fuel
	LCD_String("% ");
	LCD_CMD(0xDB,0);
	trasmitADC(1+(Sensor[2]*400)/1030,"S:");		  //Speed
	LCD_String(" ");
	UpdateLED();
}

void Dashboard(){
	LCD_CMD(0x01,0);
	LCD_CMD(0xC0,0);
	LCD_String("Health Dashboard");
	LCD_CMD(0x90,0);
	LCD_String("B:----");
	LCD_CMD(0x99,0);
	LCD_String("T:----");
	LCD_CMD(0xD0,0);
	LCD_String("F:----");
	LCD_CMD(0xD9,0);
	LCD_String("S:----");

}

__irq void ISR(){
	int i,temp;
	long int reg;
	reg = T0IR;
	for(i=0;i<4;i++){
		Sensor[i] = adcRead(i+1);
	}
	Update();
	T0IR = reg;
	VICVectAddr = 0;
}

void Engine(){
	LCD_CMD(0x01,0);
	LCD_CMD(0x80,0);
	LCD_String("----------------");
	LCD_CMD(0xC0,0);
	LCD_String("    MAX-Cars    ");
	LCD_CMD(0x90,0);
	LCD_String("----------------");
	LCD_CMD(0xD0,0);
	LCD_String("   Power-OFF    ");
}

int main(){
	int Status=1;
	PINSEL2 = 0;
	initADC();
	initUART();
	initExtTimer();
	LCD_Init();
	IODIR1 = 0x0FFFFFFF;
	IODIR0 |= 0x00FF0000;
	Engine();
	while(1){
		if(((IOPIN1&(1<<28))==(1<<28))){
			if(Status){
				LCD_CMD(0x01,0);	
				LCD_CMD(0xC0,0);
				LCD_String("    Power-ON    ");
				delay(2);
				Dashboard();
				T0TCR = 0x01;
				Status = 0;
				}
		}else{
			if(!Status){
				T0TCR = 0x00;
				LCD_CMD(0x01,0);	
				LCD_CMD(0xC0,0);
				LCD_String("    Power-OFF   ");
				delay(2);
				Engine();
				Status = 1;
				}
			}
		}
}