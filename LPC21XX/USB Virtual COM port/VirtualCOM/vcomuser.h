/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    vcomuser.h
 *      Purpose: Virtual COM custom user definition file for Philips LPC214x Family 
 *		Microprocessors
 *      Version: V1.04
 *----------------------------------------------------------------------------*/
#ifndef __VCOMUSER_H__
#define __VCOMUSER_H__

#define NO_UART_CABLE		0


/* USB Events Callback Functions */
extern void USB_Power_Event     (BOOL  power);
extern void USB_Reset_Event     (void);
extern void USB_Suspend_Event   (void);
extern void USB_Resume_Event    (void);
extern void USB_WakeUp_Event    (void);
extern void USB_SOF_Event       (DWORD frame);
extern void USB_Error_Event     (DWORD error);

/* USB Error Codes */
#define USB_ERR_PID         1  /* PID Encoding Error */
#define USB_ERR_CRC         2  /* Packet CRC Error */
#define USB_ERR_BIT_STUFF   3  /* Bit Stuff Error */
#define USB_ERR_TIMEOUT     4  /* Bus Time-out Error */
#define USB_ERR_EOP         5  /* End of Packet Error */
#define USB_ERR_DATA_TOGGLE 6  /* Data Toggle Error */
#define USB_ERR_BABBLE      7  /* Babble Error */
#define USB_ERR_LOA         8  /* Loss of Activity Error */

/* USB Enpoint Callback Events */
#define USB_EVT_SETUP       1
#define USB_EVT_OUT         2
#define USB_EVT_IN          3
#define USB_EVT_OUT_NAK     4
#define USB_EVT_IN_NAK      5
#define USB_EVT_OUT_STALL   6
#define USB_EVT_IN_STALL    7

extern void DeviceData2UART( BYTE portNum );
extern void DeviceData2Host( BYTE portNum );

/* USB Endpoint Events Callback Pointers */
extern const void (*USB_P_EP[16])(DWORD event);

/* USB Endpoint Events Callback Functions */
extern void USB_EndPoint0  (DWORD event);
extern void USB_EndPoint1  (DWORD event);
extern void USB_EndPoint2  (DWORD event);
extern void USB_EndPoint3  (DWORD event);
extern void USB_EndPoint4  (DWORD event);
extern void USB_EndPoint5  (DWORD event);
extern void USB_EndPoint6  (DWORD event);
extern void USB_EndPoint7  (DWORD event);
extern void USB_EndPoint8  (DWORD event);
extern void USB_EndPoint9  (DWORD event);
extern void USB_EndPoint10 (DWORD event);
extern void USB_EndPoint11 (DWORD event);
extern void USB_EndPoint12 (DWORD event);
extern void USB_EndPoint13 (DWORD event);
extern void USB_EndPoint14 (DWORD event);
extern void USB_EndPoint15 (DWORD event);

/* USB Device Request Events Callback Functions */
extern void USB_Configure_Event (void);
extern void USB_Interface_Event (void);
extern void USB_Feature_Event   (void);

/* UART related setting */
#define BAUDRATE_SETUP		0x03
#define STOPBIT_SETUP		0x04
#define DATABIT_SETUP		0x05
#define PARITY_SETUP		0x06
#define FLOWCTRL_SETUP		0x07
#define DTR_SETUP			0x08
#define RTS_SETUP			0x09
#define HARDCODE_SETUP		0x0B
#define MODEM_SETUP			0x0C

#define DIVMSB_9600			0x00
#define DIVLSB_9600			0x0C
#define DIVMSB_19200		0x00
#define DIVLSB_91200		0x06
#define DIVMSB_38400		0x00
#define DIVLSB_38400		0x03

/* Serial port related prototyping */
extern void init_serial( void );
extern int putchar( BYTE, int );
extern int getchar( BYTE );

extern BOOL VCOM_SetSIOSetup(BYTE Cmd, BYTE Data);
extern BOOL VCOM_GetSIOSetup(void);
extern BOOL VCOM_GetIdle    (void);
extern BOOL VCOM_SetIdle    (void);
extern BOOL VCOM_GetProtocol(void);
extern BOOL VCOM_SetProtocol(void);

#endif  /* __VCOMUSER_H__ */
