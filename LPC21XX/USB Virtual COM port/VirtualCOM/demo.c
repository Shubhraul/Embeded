/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    DEMO.C
 *      Purpose: Main C entry file for Philips LPC214x
 *      Version: V1.04
 *----------------------------------------------------------------------------*/
#include <LPC214x.h>                        /* LPC214x definitions */

#include "type.h"
#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "vcomuser.h"
#include "demo.h"

extern BYTE ReportStatus0, ReportStatus1;
extern BYTE Data2Host0, Data2Host1;

BYTE Modem0Status, Modem1Status;

/* Main Program */
int main (void) {

  BYTE i = 0;

  IODIR1 = LED1 | LED2 | LED3 |             /* LED1..4 are Outputs */
           LED4;
  
  init_serial();
  USB_Init();                               /* USB Initialization */
  USB_Connect(TRUE);                        /* USB Connect */

  while (1)                                /* Loop forever */
  {
#if USB_VCOM
	if ( Data2Host0 || Data2Host1 ) {
		DeviceData2Host( 0 );
		DeviceData2Host( 1 );
	}
#if 0
	/* EP3 is used to report UART0 status, and EP9
	is used to report UART1 status respectively */
	if ( ReportStatus0 ) {
		ReportStatus0 = 0;
		/* For Interrupt IN on EP3 */
		USB_WriteEP( 3 | 0x80, &Modem0Status, sizeof(Modem0Status) );
	}
	if ( ReportStatus1 ) {
		ReportStatus1 = 0;
		/* For Interrupt IN on EP9 */
		USB_WriteEP( 9 | 0x80, &Modem1Status, sizeof(Modem1Status) );
	}
#endif
#endif
  }											   
}
