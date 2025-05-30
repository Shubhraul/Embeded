#include<LPC214x.h>
#include "type.h"
#include "io_cfg.h"
#include "LCD.h"
#include "eeprom.h"
#include "uart0.h"
#include "uart1.h"
#include "spi_mmc.h"
#include "adc.h"
#include "rtc.h"


#define M1_MASK 0x00090000
#define M2_MASK 0x00060000

extern unsigned char I2C_WR_Buf[MAX_BUFFER_SIZE];
extern unsigned char I2C_RD_Buf[MAX_BUFFER_SIZE];
extern BYTE MMCWRData[MMC_DATA_SIZE];
extern BYTE MMCRDData[MMC_DATA_SIZE];
extern BYTE MMCStatus;
extern unsigned int ADC_Data1;
extern unsigned int ADC_Data2;
extern unsigned char Temp[4];
extern double Data;

extern unsigned int hour;
extern unsigned int min;
extern unsigned int sec;
extern unsigned char RTC[2];

unsigned int Step_Dir=0;
const unsigned int Step_Sequence[4]={0x01,0x02,0x04,0x08};		//1Phase Full step sequence
unsigned char LCD_Count=0;
unsigned char UART_Data=0;

void Delay(unsigned char j)
{  
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<60000; i++);
 } 
}


void Init_IO()
{
 PINSEL0 = 0x00000000;		// Enable GPIO on all pins
 PINSEL1 = 0x00000000;
 PINSEL2 = 0x00000000;

 IO0PIN=0x00000800;
 IO0DIR=0x007F0800;
 
 IO1CLR=0x03FF0000;
 IO1DIR=0x03FF0000;		
}


void InitPeripherals()
{
 Init_IO();
 LCD_Init();
 I2C_Init();
 Init_UART0();
 Init_UART1();
 SPI_Init();
 ADC_Init();
 Init_RTC();
}


int main()
{
 InitPeripherals();
 LCD_Command(0x01);
 Delay(1);

 if(Page_Write(BLK_1,0x00))
 {
  LCD_String("EEPROM WR Fail");
 }
 Delay(1);
 if(Page_Read(BLK_1,0x00))
 {
  LCD_String("EEPROM RD Fail");
 }


 if ( mmc_init() != 0 )
 {
  LCD_String("SD/MMC Init Fail");
 }
 else if ( mmc_write_block(0) != 0 )
 {
  LCD_String("Card Write Fail");
 }
 else if(mmc_read_block(0)!=0)
 {
  LCD_String("Card Read Fail");
 }
 
 LCD_Command(0x80);
 LCD_String(&I2C_RD_Buf[0]);
 LCD_Command(0xC0);
 LCD_String(MMCRDData);
 
 Delay(100);
 LCD_Command(0x80);
 LCD_String("   Press SW1    ");
 LCD_Command(0xC0);
 LCD_String("  To Continue   ");
 while(SW1);

 LCD_Command(0x80);
 LCD_String("  Press SW2 to  ");
 LCD_Command(0xC0);
 LCD_String(" turn ON Buzzer ");
 while(SW2);

 BUZZER_ON();
 LCD_Command(0x80);
 LCD_String("  Press SW3 to  ");
 LCD_Command(0xC0);
 LCD_String("turn OFF Buzzer ");
 while(SW3);
 BUZZER_OFF();

 LCD_Command(0x80);
 LCD_String("  Press SW1 to  ");
 LCD_Command(0xC0);
 LCD_String("  turn ON LED1  ");
 while(SW1);
 LED1_ON();

 LCD_Command(0x80);
 LCD_String("  Press SW2 to  ");
 LCD_Command(0xC0);
 LCD_String("  turn ON LED2  ");
 while(SW2);
 LED2_ON();

 LCD_Command(0x80);
 LCD_String("  Press SW3 to  ");
 LCD_Command(0xC0);
 LCD_String("  turn ON LED3  ");
 while(SW3);
 LED3_ON();

 LCD_Command(0x80);
 LCD_String("  Press SW4 to  ");
 LCD_Command(0xC0);
 LCD_String("  turn ON LED4  ");
 while(SW4);
 LED4_ON();

 LCD_Command(0x80);
 LCD_String(" Adjust Trimpot ");
 LCD_Command(0xC0);
 LCD_String("   AN1 & AN2    ");
 Delay(200);

 LCD_Command(0x80);
 LCD_String("                ");
 LCD_Command(0xC0);
 LCD_String("To Skip use SW2 ");

 while(SW2)
 {
  AD0CR =(AD0CR&0xFFFFFF00)|0x02;			   //Select AD0.1 for conversion
  Delay(1);
  AD0CR|=(1 << 24);							   //Trigger conversion
  while((AD0DR1&0x80000000)==0);			   //Wait for the conversion to be completed
  ADC_Data1 = AD0DR1;						   //Store converted data
  ADC_Data1 = (ADC_Data1>>8) & 0x00FF;

  Data=(double)ADC_Data1 * 0.0129 * 10.0;	   //Convert Digital data to represent Analog value  (x/255)*3=0.0129x 
  DECtoASCII((unsigned int)Data);
  LCD_Command(0x80);
  LCD_String("A1=");
  LCD_String(Temp);

  Delay(5);
  AD0CR = (AD0CR & 0xFFFFFF00) | 0x04 ;		   //Select AD0.2 for conversion
  Delay(1);
  AD0CR|=(1 << 24);							   //Trigger conversion
  while((AD0DR2&0x80000000)==0);			   //Wait for the conversion to be completed
  ADC_Data2 = AD0DR2;						   //Store converted data
  ADC_Data2 = (ADC_Data2>>8) & 0x00FF;
    
  Data=(double)ADC_Data2 * 0.0129 * 10.0;
  DECtoASCII((unsigned int)Data);
  LCD_String("  A2=");
  LCD_String(Temp);
 }

 LCD_Command(0x80);
 LCD_String("    RTC TEST    ");
 LCD_Command(0xC0);
 LCD_String("To Skip use SW4 ");
 while(SW4)
 {
  LCD_Command(0x84);
  RTC_DECtoASCII(hour);
  LCD_String(RTC);
  LCD_String(":");
  RTC_DECtoASCII(min);
  LCD_String(RTC);
  LCD_String(":");
  RTC_DECtoASCII(sec);
  LCD_String(RTC);
 }
 IO1CLR=M1_MASK;
 IO1CLR=M2_MASK;
 LCD_Command(0x80);
 LCD_String("      L293D     ");
 LCD_Command(0xC0);
 LCD_String("  DC MOTOR TEST ");
 Delay(200);
 LCD_Command(0x80);
 LCD_String("MOTOR CONNECTION");
 LCD_Command(0xC0);
 LCD_String("M1->A&B  M2->C&D");
 Delay(200);
 LCD_Command(0x80);
 LCD_String("M1->A&B  M2->C&D");
 LCD_Command(0xC0);
 LCD_String(" SW2 TO PROCEED ");
 while(SW2);
 LCD_Command(0x80);
 LCD_String("M1:SW1    M2:SW4");
 LCD_Command(0xC0);
 LCD_String("To Skip use SW3 ");
 while(SW3)
 {
  if(!SW1)
  {
   IO1CLR=M1_MASK;
   IO1SET=0x00080000;
  }
  else
  {
   IO1CLR=M1_MASK;
   IO1SET=0x00010000;
  }
  if(!SW4)
  {
   IO1CLR=M2_MASK;
   IO1SET=0x00040000;
  }
  else
  {
   IO1CLR=M2_MASK;
   IO1SET=0x00020000;
  }
 }
 IO1CLR=M1_MASK;
 IO1CLR=M2_MASK;
 LCD_Command(0x80);
 LCD_String("Disconnect motor");
 LCD_Command(0xC0);
 LCD_String("  and Press SW1 ");
 while(SW1);
 LCD_Command(0x80);
 LCD_String("ULN2003 UNIPOLAR");
 LCD_Command(0xC0);
 LCD_String("  STEPPER MOTOR ");
 Delay(200);
 LCD_Command(0x80);
 LCD_String("MOTOR CONNECTION");
 LCD_Command(0xC0);
 LCD_String("Coil-1234  I1234");
 Delay(400);
 LCD_Command(0x80);
 LCD_String("      G-GND     ");
 LCD_Command(0xC0);
 LCD_String(" SW2 TO PROCEED ");
 while(SW2);
 LCD_Command(0x80);
 LCD_String(" SW1->Motor Dir ");
 LCD_Command(0xC0);
 LCD_String("    SW4->Skip   ");
 while(SW4)
 {
  if(!SW1)
  {
   Step_Dir=~Step_Dir;
  }
  IO1CLR=0x00F00000;
  if(Step_Dir){IO1PIN|=Step_Sequence[0] << 20;}
  else		  {IO1PIN|=Step_Sequence[3] << 20;}
  Delay(10);
  IO1CLR=0x00F00000;
  if(Step_Dir){IO1PIN|=Step_Sequence[1] << 20;}
  else		  {IO1PIN|=Step_Sequence[2] << 20;}
  Delay(10);
  IO1CLR=0x00F00000;
  if(Step_Dir){IO1PIN|=Step_Sequence[2] << 20;}
  else		  {IO1PIN|=Step_Sequence[1] << 20;}
  Delay(10);
  IO1CLR=0x00F00000;
  if(Step_Dir){IO1PIN|=Step_Sequence[3] << 20;}
  else		  {IO1PIN|=Step_Sequence[0] << 20;}
  Delay(10);
 }

 LCD_Command(0x80);
 LCD_String("   UART0 TEST   ");
 LCD_Command(0xC0);
 LCD_String(" Setup Terminal ");
 Delay(400);
 LCD_Command(0x80);
 LCD_String("Baud-9600  8-N-1");
 LCD_Command(0xC0);
 LCD_String("Type on KyBoard ");
 while((U0LSR&0x01)==0);
 UART_Data=U0RBR;
 LCD_Command(0x80);
 LCD_String("                ");
 LCD_Command(0xC0);
 LCD_String("    SW1->Skip   ");
 LCD_Command(0x80);
 LCD_Data(UART_Data);
 LCD_Count++;
 while(SW1)
 {
  if((U0LSR&0x01)==1)
  {
   UART_Data=U0RBR;
   LCD_Data(UART_Data);
   LCD_Count++;
   if(LCD_Count==16)
   {
    LCD_Count=0;
    LCD_Command(0x80);
    LCD_String("                ");
    LCD_Command(0x80);
   }
  } 
 }

 LCD_Command(0x80);
 LCD_String("   UART1 TEST   ");
 LCD_Command(0xC0);
 LCD_String(" Setup Terminal ");
 Delay(400);
 LCD_Command(0x80);
 LCD_String("Set UART1 Jumper");
 LCD_Command(0xC0);
 LCD_String("  SW2-> Proceed ");
 while(SW2);
 LCD_Command(0x80);
 LCD_String("Baud-9600  8-N-1");
 LCD_Command(0xC0);
 LCD_String("Type on KyBoard ");
 while((U1LSR&0x01)==0);
 UART_Data=U1RBR;
 LCD_Command(0x80);
 LCD_String("                ");
 LCD_Command(0xC0);
 LCD_String("    SW4->Skip   ");
 LCD_Command(0x80);
 LCD_Data(UART_Data);
 LCD_Count++;
 while(SW4)
 {
  if((U1LSR&0x01)==1)
  {
   UART_Data=U1RBR;
   LCD_Data(UART_Data);
   LCD_Count++;
   if(LCD_Count==16)
   {
    LCD_Count=0;
    LCD_Command(0x80);
    LCD_String("                ");
    LCD_Command(0x80);
   }
  }
 }

 LCD_Command(0x80);
 LCD_String("   XBEE TEST    ");
 LCD_Command(0xC0);
 LCD_String(" Setup Terminal ");
 Delay(400);
 LCD_Command(0x80);
 LCD_String("Set XBEE Jumper ");
 LCD_Command(0xC0);
 LCD_String("  SW2-> Proceed ");
 while(SW2);
 LCD_Command(0x80);
 LCD_String("Baud-9600  8-N-1");
 LCD_Command(0xC0);
 LCD_String("Type on KyBoard ");
 while((U1LSR&0x01)==0);
 UART_Data=U1RBR;
 LCD_Command(0x80);
 LCD_String("                ");
 LCD_Command(0xC0);
 LCD_String("    SW4->Skip   ");
 LCD_Command(0x80);
 LCD_Data(UART_Data);
 LCD_Count++;
 while(SW4)
 {
  if((U1LSR&0x01)==1)
  {
   UART_Data=U1RBR;
   LCD_Data(UART_Data);
   LCD_Count++;
   if(LCD_Count==16)
   {
    LCD_Count=0;
    LCD_Command(0x80);
    LCD_String("                ");
    LCD_Command(0x80);
   }
  }
 }
 
 LCD_Command(0x80);
 LCD_String("   BOARD TEST   ");
 LCD_Command(0xC0);
 LCD_String("    FINISHED    ");
 while(1)
 {
  
 }
}

