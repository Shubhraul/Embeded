
#include <string.h>
#include <LPC214x.h>                        /* LPC214x definitions */

#include "type.h"
#include "usb.h"
#include "usbhw.h"
#include "usbcfg.h"
#include "usbcore.h"
#include "vcomuser.h"
#include "demo.h"

#define CR     0x0D

static BYTE KeyPressed = 0;

BYTE ReportStatus0 = 0, ReportStatus1 = 0;
BYTE Data2Host0 = 0, Data2Host1 = 0;

#if USB_VCOM
BYTE RxLength0, RxLength1;

BYTE  USB2UARTBuf0[USB_MAX_PACKET0];
BYTE  USB2UARTBuf1[USB_MAX_PACKET0];

BYTE  UART2USBBuf0[USB_MAX_PACKET0];
BYTE  UART2USBBuf1[USB_MAX_PACKET0];

/* if the portNum is 0, EP1 is the endpoints for 
STATUS IN, EP2 EP is BULK IN, BULK OUT and UART0 Virtual COM port.
   if the portNum is 1, EP4 is the endpoints for 
STATUS IN, EP5 is BULK IN, BULK OUT and UART1 Virtual COM port */

void DeviceData2UART( BYTE portNum )
{
 DWORD cnt, i;
 BYTE *pData;
 BYTE status=0;

  if ( portNum == 0 ) 					//if Data is being send from COMx
  {
	pData = &USB2UARTBuf0[0];
	cnt = USB_ReadEP(0x02, pData);
	switch(*pData)
	{
	 case '1': 
	 {
	 	if(IOPIN1 & 0x00010000)
			{IOCLR1 = 0x00010000;}
		else
			{IOSET1 = 0x00010000;}
		break;
	 }
	 case '2': 
	 {
	 	if(IOPIN1 & 0x00020000)
			{IOCLR1 = 0x00020000;}
		else
			{IOSET1 = 0x00020000;}
		break;
	 }
	 case '3': 
	 {
	 	if(IOPIN1 & 0x00040000)
			{IOCLR1 = 0x00040000;}
		else
			{IOSET1 = 0x00040000;}
		break;
	 }
	 case '4': 
	 {
	 	if(IOPIN1 & 0x00080000)																			 
			{IOCLR1 = 0x00080000;}
		else
			{IOSET1 = 0x00080000;}
		break;
	 }
	 
	 default:
	 break;	 
	 
	}
  }
  else if ( portNum == 1 ) 				 //if Data is being send from COMx+1
  {
	pData = &USB2UARTBuf1[0];
	cnt = USB_ReadEP(0x05, pData);
  }
  return;
}

void DeviceData2Host( BYTE portNum )
{
 const BYTE msg1[] = "SW1 pressed\r\n";
 const BYTE msg2[] = "SW2 pressed\r\n";
 const BYTE msg3[] = "SW3 pressed\r\n";
 const BYTE msg4[] = "SW4 pressed\r\n";
 BYTE length;
 
 
  if (!SW1 && !KeyPressed)				/* Check if S2 is pressed */
  {
   KeyPressed = 1;
   length = strlen(msg1);
   memcpy( UART2USBBuf0, msg1, length );
   USB_WriteEP(0x80 | 0x02, &UART2USBBuf0[0], length );
   KeyPressed = 0;
  }
  if (!SW2 && !KeyPressed)				/* Check if S2 is pressed */
  {
   KeyPressed = 1;
   length = strlen(msg2);
   memcpy( UART2USBBuf0, msg2, length );
   USB_WriteEP(0x80 | 0x02, &UART2USBBuf0[0], length );
   KeyPressed = 0;
  }
  if (!SW3 && !KeyPressed)				/* Check if S2 is pressed */
  {
   KeyPressed = 1;
   length = strlen(msg3);
   memcpy( UART2USBBuf0, msg3, length );
   USB_WriteEP(0x80 | 0x02, &UART2USBBuf0[0], length );
   KeyPressed = 0;
  }
  if (!SW4 && !KeyPressed)				/* Check if S2 is pressed */
  {
   KeyPressed = 1;
   length = strlen(msg4);
   memcpy( UART2USBBuf0, msg4, length );
   USB_WriteEP(0x80 | 0x02, &UART2USBBuf0[0], length );
   KeyPressed = 0;
  }

 
 return;
}
#endif

/*
 *  USB Power Event Callback
 *    Parameter:       power: On(TRUE)/Off(FALSE)
 */

#if USB_POWER_EVENT
void USB_Power_Event (BOOL  power) {
  power;
}
#endif


/*
 *  USB Reset Event Callback
 */

#if USB_RESET_EVENT
void USB_Reset_Event (void) {
  USB_ResetCore();
}
#endif


/*
 *  USB Suspend Event Callback
 */

#if USB_SUSPEND_EVENT
void USB_Suspend_Event (void) {
}
#endif


/*
 *  USB Resume Event Callback
 */

#if USB_RESUME_EVENT
void USB_Resume_Event (void) {
}
#endif


/*
 *  USB Remote Wakeup Event Callback
 */

#if USB_WAKEUP_EVENT
void USB_WakeUp_Event (void) {
}
#endif


/*
 *  USB Start of frame Event Callback
 *    Parameter:       frame: 11-bit Frame Number
 */

#if USB_SOF_EVENT
void USB_SOF_Event (DWORD frame) {
  frame;
}
#endif


/*
 *  USB Error Event Callback
 *    Parameter:       error: Error Code
 */

#if USB_ERROR_EVENT
void USB_Error_Event (DWORD error) {
  error;
}
#endif

/*
 *  USB Set Configuration Event Callback
 */
#if USB_CONFIGURE_EVENT
void USB_Configure_Event (void) {
  if (USB_Configuration) {             /* Check if USB is configured */
    ReportStatus0 = ReportStatus1 = 1;
	Data2Host0 = Data2Host1 = 1;
  }
}
#endif

/*
 *  USB Set Interface Event Callback
 */

#if USB_INTERFACE_EVENT
void USB_Interface_Event (void) {
}
#endif

/*
 *  USB Set/Clear Feature Event Callback
 */

#if USB_FEATURE_EVENT
void USB_Feature_Event (void) {
}
#endif


#define P_EP(n) ((USB_EP_EVENT & (1 << (n))) ? USB_EndPoint##n : NULL)

/* USB Endpoint Events Callback Pointers */
const void (* USB_P_EP[16]) (DWORD event) = {
  P_EP(0),
  P_EP(1),
  P_EP(2),
  P_EP(3),
  P_EP(4),
  P_EP(5),
  P_EP(6),
  P_EP(7),
  P_EP(8),
  P_EP(9),
  P_EP(10),
  P_EP(11),
  P_EP(12),
  P_EP(13),
  P_EP(14),
  P_EP(15),
};

/*
 *  USB Endpoint 1 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint1 (DWORD event) {
  event;
}

/*
 *  USB Endpoint 2 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint2 (DWORD event) {
  switch (event) {
    case USB_EVT_IN:
	  Data2Host0 = 1;
      break;
	case USB_EVT_OUT:
	  DeviceData2UART( 0 );
	  break;
  }
  event;
}


/*
 *  USB Endpoint 3 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint3 (DWORD event) {
  event;
}

/*
 *  USB Endpoint 4 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint4 (DWORD event) {
  event;
}

/*
 *  USB Endpoint 5 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint5 (DWORD event) {
  switch (event) {
    case USB_EVT_IN:
	  Data2Host1 = 1;
      break;
	case USB_EVT_OUT:
	  DeviceData2UART( 1 );
	  break;
  }
  event;
}

/*
 *  USB Endpoint 6 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint6 (DWORD event) {
  event;
}


/*
 *  USB Endpoint 7 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint7 (DWORD event) {
  event;
}


/*
 *  USB Endpoint 8 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint8 (DWORD event) {
  event;
}


/*
 *  USB Endpoint 9 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint9 (DWORD event) {
  event;
}


/*
 *  USB Endpoint 10 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint10 (DWORD event) {
  event;
}


/*
 *  USB Endpoint 11 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint11 (DWORD event) {
  event;
}

/*
 *  USB Endpoint 12 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint12 (DWORD event) {
  event;
}


/*
 *  USB Endpoint 13 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint13 (DWORD event) {
  event;
}


/*
 *  USB Endpoint 14 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint14 (DWORD event) {
  event;
}


/*
 *  USB Endpoint 15 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint15 (DWORD event) {
  event;
}



/* UART setup and simple putc() and getc() routine */
/* Default setting of CCLK is 60Mhz, VPBCLK is 1/4 = 15Mhz */

void init_serial (void)  {     /* Initialize Serial Interface       */

#if NO_UART_CABLE
#else
  PINSEL0 = 0x00050005;        /* Enable RxD1 and TxD1, RxD0 and TxD0 */
#endif
  
  U0LCR = 0x83;                /* 8 bits, no Parity, 1 Stop bit     */
  U0DLL = 97;                  /* 9600 Baud Rate @ 15MHz VPB Clock  */
  U0LCR = 0x03;                /* DLAB = 0                          */  
  U0FCR = 0x07;				   /* Enable and reset TX and RX FIFO. */

  U1LCR = 0x83;                /* 8 bits, no Parity, 1 Stop bit     */
  U1DLL = 97;                  /* 9600 Baud Rate @ 15MHz VPB Clock  */
  U1LCR = 0x03;                /* DLAB = 0                          */
  U1FCR = 0x07;				   /* Enable and reset TX and RX FIFO. */
}

int putchar(BYTE portNum, int ch)  {     /* Write character to Serial Port    */

  if ( portNum == 0 ) {
	if (ch == '\n')  {
		while (!(U0LSR & 0x20));
		U0THR = CR;                          /* output CR */
	}
	while (!(U0LSR & 0x20));
	return (U0THR = ch);
  }
  else {
	if (ch == '\n')  {
		while (!(U1LSR & 0x20));
		U1THR = CR;                          /* output CR */
	}
	while (!(U1LSR & 0x20));
	return (U1THR = ch);
  }
}

int getchar ( BYTE portNum )  {                    /* Read character from Serial Port   */

  if ( portNum == 0 ) {
	while (!(U0LSR & 0x01));
  	return (U0RBR);
  }
  else {
	while (!(U1LSR & 0x01));
  	return (U1RBR);
  }
}

/*  Setup SIO configuration based on the channel number */
void SetSIOBaudrate( BYTE channelNum, BYTE Data )
{
    /* Data = 0x1		Baudrate = 115,200
	   Data = 0x2		Baudrate = 57,600
	   Data = 0x3		Baudrate = 38,400
	   Data = 0x6		Baudrate = 19,200
	   Data	= 0x0C		Baudrate = 9,600 */
  
  /* PCLK is set the same as CCLK at 60Mhz */ 
  if ( channelNum == 0 ) {
	U0FCR = 0x07;				   /* Enable and reset TX and RX FIFO. */
    U0LCR |= 0x80;	
	if ( Data == 0x01 ) {
		U0DLL = 0x20;
		U0DLM = 0x00;
	}
	else if ( Data == 0x02 ) {
		U0DLL = 0x41;
		U0DLM = 0x00;
	}
	else if ( Data == 0x03 ) {
		U0DLL = 0x61;
		U0DLM = 0x00;
	}
	else if ( Data == 0x06 ) {
		U0DLL = 0xC3;
		U0DLM = 0x00;
	}
	else if ( Data == 0x0C ) {
		U0DLL = 0x86;
		U0DLM = 0x01;
	} 	
	U0LCR &= ~0x80;
  }
  else if ( channelNum == 1 ) {
	U1FCR = 0x07;				   /* Enable and reset TX and RX FIFO. */
	U1LCR |= 0x80;
	if ( Data == 0x01 ) {
		U1DLL = 0x20;
		U1DLM = 0x00;
	}
	else if ( Data == 0x02 ) {
		U1DLL = 0x41;
		U1DLM = 0x00;
	}
	else if ( Data == 0x03 ) {
		U1DLL = 0x61;
		U1DLM = 0x00;
	}
	else if ( Data == 0x06 ) {
		U1DLL = 0xC3;
		U1DLM = 0x00;
	}
	else if ( Data == 0x0C ) {
		U1DLL = 0x86;
		U1DLM = 0x01;
	}
	U1LCR &= ~0x80;
  }
}

void SetSIOStopBit( BYTE channelNum, BYTE ConfigValue )
{
  BYTE lcr;

  /* 0 is 1 stop bit, 1 is 2 stop bits, bit2 on LCR is stop bit setting */
  if ( channelNum == 0 ) {
	lcr = U0LCR & 0xFB;
	U0LCR = lcr | (ConfigValue << 2);		  	
  }
  else if ( channelNum == 1 ) {
  	lcr = U1LCR & 0xFB;
	U1LCR = lcr | (ConfigValue << 2);
  }
}

void SetSIODataBit( BYTE channelNum, BYTE ConfigValue )
{
  BYTE lcr;

  if ( channelNum == 0 ) {
	lcr = U0LCR & 0xFC;
	U0LCR = lcr | ConfigValue;		  	
  }
  else if ( channelNum == 1 ) {
  	lcr = U1LCR & 0xFC;
	U1LCR = lcr | ConfigValue;
  }
}

void SetSIOParity( BYTE channelNum, BYTE ConfigValue )
{
  BYTE lcr;

  if ( channelNum == 0 ) {
		lcr = U0LCR & 0xCF;
		U0LCR = lcr | (ConfigValue << 4);
  }
  else if ( channelNum == 1 ) {
  		lcr = U1LCR & 0xCF;
		U1LCR = lcr | (ConfigValue << 4);
  }
}

void SetSIOFlowControl( BYTE channelNum, BYTE ConfigValue )
{
  /* NO flow control setting is neceaasry, unlike the UART on x51 */ 
  if ( channelNum == 0 ) {
	ConfigValue = ConfigValue;		  	
  }
  else if ( channelNum == 1 ) {
	ConfigValue = ConfigValue;
  }
}

void SetSIODTR( BYTE channelNum, BYTE ConfigValue )
{
  BYTE mcr;
  /* only apply to channel 1 */
  if ( channelNum == 1 ) {
  	mcr = U1MCR & 0xFE;
	U1MCR = mcr | ConfigValue;
  }
}

void SetSIORTS( BYTE channelNum, BYTE ConfigValue )
{
  BYTE mcr;

  /* only apply to channel 1 */
  if ( channelNum == 1 ) {
  	mcr = U1MCR & 0xFD;
	U1MCR = mcr | (ConfigValue << 1);
  }
}

void GetSIOModemStatus( BYTE channelNum )
{
  channelNum = channelNum;
}

/*
 *  VCOM Get SIO Setup Request Callback
 *    Parameters:      None (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL VCOM_GetSIOSetup (void) {
  /* Get SIO setup from the device, not supported */
  return (TRUE);  
}


/*
 *  VCOM Set SIO Setup Request Callback
 *    Parameters:      None (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL VCOM_SetSIOSetup( BYTE Cmd, BYTE Data ) {
  
  BYTE ChannelNum;

  /* Set SIO configuration, baudrate, data bits, stop bits, parity, flow control, 
  based on the info. from the host */
  if ( !(Cmd & 0x20) )
	ChannelNum = 0;
  else
	ChannelNum = 1;

  Cmd &= ~0x20;			/* clear bit 5, use all cmd for channel 0 */
  switch ( Cmd ) {
    case BAUDRATE_SETUP:
      SetSIOBaudrate( ChannelNum, Data );
      break;
    case STOPBIT_SETUP:
		SetSIOStopBit( ChannelNum, Data );
      break;          
    case DATABIT_SETUP:
		SetSIODataBit( ChannelNum, Data );
	  break;
	case PARITY_SETUP:
		SetSIOParity( ChannelNum, Data );
	  break;
	case FLOWCTRL_SETUP:
		SetSIOFlowControl( ChannelNum, Data );
	  break;
	case DTR_SETUP:
		SetSIODTR( ChannelNum, Data );
	  break;
	case RTS_SETUP:
		SetSIORTS( ChannelNum, Data );
	  break;
	case HARDCODE_SETUP:
	  break;
	case MODEM_SETUP:
		GetSIOModemStatus( ChannelNum );
	  break;
	default:
      // EP0Buf[] = ...;
      // break;
      return (FALSE);
  }
  return (TRUE);
}

