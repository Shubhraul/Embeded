#include<lpc214x.h>
#define row1 (1<<20)
#define row2 (1<<21)
#define row3 (1<<22)
#define row4 (1<<23)
#define col1 (1<<16)
#define col2 (1<<17)
#define col3 (1<<18)
#define col4 (1<<19)

void delay(int N){
	int i,j;
	for(i=0;i<N;i++){
		for(j=0;j<6000;j++);
	}
}

void delay_key(){
	int i,j;
	for(i=0;i<300;i++){
		for(j=0;j<1275;j++);
	}
}

char keypad(){
	char key='o';
	IOCLR1 |= (row1|row2|row3|row4|col1|col2|col3|col4);
	while(1){
		IOCLR1 |= row1;			 //row1
		IOSET1 |= (row2|row3|row4);
		if((IOPIN1&col1)==0){
			key='7';
			delay_key();
			return key;
		}
		else if((IOPIN1&col2)==0){
			key='8';
			delay_key();
			return key;
		}
		else if((IOPIN1&col3)==0){
			key='9';
			delay_key();
			return key;
		}
		else if((IOPIN1&col4)==0){
			key='/';
			delay_key();
			return key;
		}
		IOCLR1 |= row2;				  //row2
		IOSET1 |= (row1|row3|row4);
		if((IOPIN1&col1)==0){
			key='4';
			delay_key();
			return key;
		}
		else if((IOPIN1&col2)==0){
			key='5';
			delay_key();
			return key;
		}
		else if((IOPIN1&col3)==0){
			key='6';
			delay_key();
			return key;
		}
		else if((IOPIN1&col4)==0){
			key='*';
			delay_key();
			return key;
		}
		IOCLR1 |= row3;				  //row3
		IOSET1 |= (row1|row2|row4);
		if((IOPIN1&col1)==0){
			key='1';
			delay_key();
			return key;
		}
		else if((IOPIN1&col2)==0){
			key='2';
			delay_key();
			return key;
		}
		else if((IOPIN1&col3)==0){
			key='3';
			delay_key();
			return key;
		}
		else if((IOPIN1&col4)==0){
			key='-';
			delay_key();
			return key;
		}
		IOCLR1 |= row4;				  //row4
		IOSET1 |= (row1|row2|row3);
		if((IOPIN1&col1)==0){
			key='C';
			delay_key();
			return key;
		}
		else if((IOPIN1&col2)==0){
			key='0';
			delay_key();
			return key;
		}
		else if((IOPIN1&col3)==0){
			key='=';
			delay_key();
			return key;
		}
		else if((IOPIN1&col4)==0){
			key='+';
			delay_key();
			return key;
		}
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
	delay(2);
	IOCLR0 = 0x00000040; //EN:0
	delay(5);
	IOPIN0 = ((IOPIN0 & 0xFFFF00FF)| (cmd & 0x0F)<<12);	  //LSB	8
	if(RS){ 
		IOSET0 = 0x00000050; // RS:1, EN:1
		IOCLR0 = 0x00000020; // RW:0
	}else{
		IOSET0 = 0x00000040; // RS:0, EN:1
		IOCLR0 = 0x00000030; // RW:0
	}
	delay(2);
	IOCLR0 = 0x00000040; //EN:0
	delay(5);
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

void Drive(int val,int Rotation){
	int Pin,i,j;
	LCD_CMD(0x01,0);
	LCD_CMD(0x80,0);
	LCD_String("Processing ");
	if(val==0){
		LCD_String("Coke ");
		Pin = 16;
	}else if(val==1){
		LCD_String("Lime ");
		Pin = 20;
	}else if(val==2){
		LCD_String("Fanta");
		Pin = 28;
	}else if(val==3){
		LCD_String("Pepsi");
		Pin = 0;
	}
	for(i=0;i<Rotation;i++){
		LCD_CMD(0xC0,0);
		for(j=0;j<4;j++){
			IOPIN0 = 1<<(Pin+j);
			LCD_String("**");
			delay(150);
		}
	}
}

void Welcome(){
	int i;
	LCD_CMD(0x01,0);
	LCD_CMD(0x80,0);
	LCD_String("-*-*-*-*-*-*-*-*");
	LCD_CMD(0xC0,0);
	LCD_String("   Just Juicy   ");
	LCD_CMD(0x90,0);
	LCD_String("-*-*-*-*-*-*-*-*");
	for(i=0;i<3;i++){
		LCD_CMD(0xD0,0);
		LCD_String("                ");
		LCD_CMD(0xD0,0);
		LCD_String(" Have Some Cool ");
		delay_key();
	}
}

void Product(){
	LCD_CMD(0x01,0);
	LCD_CMD(0x80,0);
	LCD_String("Available Juices : ");
	LCD_CMD(0xC0,0);
	LCD_String("1:Coke    2:Lime");
	LCD_CMD(0x90,0);
	LCD_String("3:Fanta  4:Pepsi");
	LCD_CMD(0xD0,0);
	LCD_String("C:Cancel  =:Next");
}

void Prod_Count(){
	LCD_CMD(0x01,0);
	LCD_CMD(0x80,0);
	LCD_String("Select Quantity ");
	LCD_CMD(0xC0,0);
}

int Process(){
	int cnt=0,Status=1,i,k[6],rot[5];
	char temp;
	for(i=0;i<5;i++){
		temp = keypad();
		if(temp=='C'){
			Status=1;
			break;
		}else if(temp=='='){
			Status=0;
			break;
		}else if(temp=='1'){
			k[i] = 0;
			cnt++;
		}else if(temp=='2'){
			k[i] = 1;
			cnt++;
		}else if(temp=='3'){
			k[i] = 2;
			cnt++;
		}else if(temp=='4'){
			k[i] = 3;
			cnt++;
		}		
	delay(50);
	}
	if(Status){return 0;}
	for(i=0;i<cnt;i++){
		Prod_Count();
		if(k[i]==0){
			LCD_String("for Coke");
		}if(k[i]==1){
			LCD_String("for Lime");
		}if(k[i]==2){
			LCD_String("for Fanta");
		}if(k[i]==3){
			LCD_String("for Pepsi");
		}
		LCD_CMD(0x90,0);
		LCD_String("Max Upto 9");
		rot[i] = keypad()-'0';
	}
	for(i=0;i<cnt;i++){
		Drive(k[i],rot[i]);
	}
	LCD_CMD(0x01,0);
	LCD_CMD(0xC0,0);
	LCD_String("   Thank You!   ");
	delay(400);
}

int main(){
	int i,status;
	LCD_Init();
	PINSEL1 = 0x00000000;
	IODIR1 = 0x00F00000;
	IODIR0 |= 0xF0FF000F;
	while(1){
		Welcome();
		while(keypad()!='o'){
			Product();
			status = Process();
			break;
		}
	}
}