/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    usbcore.h
 *      Purpose: USB Core definiton file for Philips LPC214x Family 
 *		Microprocessors
 *      Version: V1.04
 *----------------------------------------------------------------------------*/
#ifndef __USBCORE_H__
#define __USBCORE_H__

/* USB Endpoint Data Structure */
typedef struct _USB_EP_DATA {
  BYTE  *pData;
  WORD   Count;
} USB_EP_DATA;

/* USB Core Global Variables */
extern WORD  USB_DeviceStatus;
extern BYTE  USB_DeviceAddress;
extern BYTE  USB_Configuration;
extern DWORD USB_EndPointMask;
extern DWORD USB_EndPointHalt;
extern BYTE  USB_AltSetting[USB_IF_NUM];

/* USB Endpoint 0 Buffer */
extern BYTE  EP0Buf[USB_MAX_PACKET0];

/* USB Setup Packet */
extern USB_SETUP_PACKET SetupPacket;

/* USB Core Functions */
extern void  USB_ResetCore (void);


#endif  /* __USBCORE_H__ */
