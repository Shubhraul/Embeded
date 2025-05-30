#ifndef __LCD_H
#define __LCD_H


#define DATA_PORT() IO0SET=(1<<16)		 //Function to select data port on LCD
#define READ_DATA() IO0SET=(1<<17)		 //Function to select read operation on LCD
#define EN_HI() IO0SET=(1<<18)			 //Function to Enable LCD


#define COMMAND_PORT() IO0CLR=(1<<16)	  //Function to select command port on LCD
#define WRITE_DATA() IO0CLR=(1<<17)		  //Function to select write operation on LCD
#define EN_LOW() IO0CLR=(1<<18)			  //Function to disable LCD

void LCD_Delay(unsigned char j);
unsigned char Busy_Wait(void);
void LCD_Command(unsigned int data);
void LCD_Data(unsigned int data);
void LCD_Init(void);
void LCD_String(unsigned char *data);
extern void Delay(unsigned char j);

#endif
