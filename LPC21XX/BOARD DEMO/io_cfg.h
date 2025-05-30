
#include<LPC214x.h>

 /********************LED Init**********************************************/


//IO1DIR = (1<<19) | (1<<18) | (1<<17) | (1<<16);		// Set P1.16, P1.17, P1.18, P1.19 as Output

#define LED1_ON() IO1SET=(1<<16)	//Macro Functions to turn ON LED
#define LED2_ON() IO1SET=(1<<17)
#define LED3_ON() IO1SET=(1<<18)
#define LED4_ON() IO1SET=(1<<19)

#define LED1_OFF() IO1CLR=(1<<16)	//Macro Functions to turn OFF LED
#define LED2_OFF() IO1CLR=(1<<17)
#define LED3_OFF() IO1CLR=(1<<18)
#define LED4_OFF() IO1CLR=(1<<19)

/***************************************************************************/


/********************Buzzer*************************************************/
#define BUZZER_ON() IO0CLR=(1<<11)		 //Macro Functions to turn ON Buzzer
#define BUZZER_OFF() IO0SET=(1<<11)		 //Macro Functions to turn OFF Buzzer
/***************************************************************************/


/********************Switch*************************************************/
#define SW1 (IO0PIN & 0x00008000)	 //Macro Functions to read Push button switches on LPC2148 dev. board
#define SW2	(IO0PIN & 0x00002000)
#define SW3	(IO0PIN & 0x00001000)
#define SW4	(IO0PIN & 0x40000000)


/********************UART1************************************************/
#define  UART1_BPS	9600	 //Set Baud Rate here


















