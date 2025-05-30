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

void forward(){
	int j;
	for(j=0;j<4;j++){
		IOPIN0 = 1<<(j);
		delay(150);
	}
}

void reverse(){
	int j;
	for(j=4;j>0;j--){
		IOPIN0 = 1<<(j);
		delay(150);
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

int check(char *val){
	char key[6] = "123456";
	int i;
	for(i=0;i<6;i++){
		if(val[i] != key[i]){
			return 0;
		}	
	}
	return 1;
}

int main(){
	int i;
	char temp,pass[16];
	LCD_Init();
	PINSEL0 &= ~0xFF;
	IODIR0 |= 0x0F;
	PINSEL1 = 0x00000000;
	IODIR1 = 0x00F00000;
	while(1){
		LCD_CMD(0x80,0);
		LCD_String("*****LOCKED*****");
		LCD_CMD(0xC0,0);
		LCD_String("Press any Key...");
		delay_key();
		if(keypad()!='o'){
			LCD_CMD(0x01,0);
			LCD_CMD(0x80,0);
			LCD_String("Enter 6 Pin Key ");
			LCD_CMD(0xC0,0);
			temp = 'o';
			i=0;
			while(temp!='='){
				temp = keypad();
				delay_key();
				pass[i] = temp;
				LCD_CMD(temp,1);
				delay_key();
				LCD_CMD(0xC0+i,0);
				LCD_CMD('*',1);
				i++;
			}
			if(check(pass)){
				forward();
				LCD_CMD(0x01,0);
				LCD_String("****UNLOCKED****");
				while(keypad()!='C');
				reverse();
			}else{
				LCD_CMD(0x01,0);
				LCD_String("!!!Wrong Pin!!!");
			}
		}
	}
}