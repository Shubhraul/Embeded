#include<LPC214x.h>
#include "LCD.h"


void LCD_Delay(unsigned char j)
{
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<1000; i++);
 } 
}
                               

unsigned char Busy_Wait()			   //This function checks the busy status of LCD
{
 unsigned int temp=0;
 EN_LOW();
 COMMAND_PORT();
 READ_DATA();
 
 IO0PIN&=0xFF87FFFF;	  
 IO0DIR&=0xFF87FFFF;
 IO0PIN|=0x00400000;
 
 do{
 EN_HI();
 EN_LOW();
 EN_HI();
 EN_LOW();
 temp=IO0PIN;
 }
 while((temp & 0x00400000)==0x00400000);
 EN_LOW();
 WRITE_DATA();
 IO0DIR&=0xFF80FFFF;
 IO0DIR|=0x007F0000;
 return (0);
}    


void LCD_Command(unsigned int data)			  //This function is used to send LCD commands
{
 unsigned int temp=0;
 EN_LOW();
 COMMAND_PORT();
 WRITE_DATA();
 
 temp=data;
 IO0PIN&=0xFF87FFFF;
 IO0PIN|=(temp & 0xF0) << 15;

 EN_HI();
 EN_LOW();
 
 temp=data & 0x0F;
 IO0PIN&=0xFF87FFFF;
 IO0PIN|=(temp) << 19;

 EN_HI();
 EN_LOW();
 while(Busy_Wait());
 Delay(10);
} 


void LCD_Data(unsigned int data)		   //This function is used to send data to LCD
{
 unsigned int temp=0;
 EN_LOW();
 DATA_PORT();
 WRITE_DATA();
 
 temp=data;
 IO0PIN&=0xFF87FFFF;
 IO0PIN|=(temp & 0xF0) << 15;

 EN_HI();
 EN_LOW();
 
 temp=data & 0x0F;
 
 IO0PIN&=0xFF87FFFF;
 IO0PIN|=(temp) << 19;

 EN_HI();
 EN_LOW();
 LCD_Delay(1);
}

void LCD_Init()
{
 LCD_Command(0x20);
 LCD_Command(0x28);
 LCD_Command(0x0C);
 LCD_Command(0x06);
}


void LCD_String(unsigned char *data)
{
 while(*data)
 {
  LCD_Data(*data);
  data++;
 } 
}

