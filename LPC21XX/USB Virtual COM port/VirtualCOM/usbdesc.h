/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    usbdesc.h
 *      Purpose: USB Descriptors header file for Philips LPC214x Family 
 *		Microprocessors
 *      Version: V1.04
 *----------------------------------------------------------------------------*/
#ifndef __USBDESC_H__
#define __USBDESC_H__


#define WBVAL(x) (x & 0xFF),((x >> 8) & 0xFF)

#define USB_DEVICE_DESC_SIZE        (sizeof(USB_DEVICE_DESCRIPTOR))
#define USB_CONFIGUARTION_DESC_SIZE (sizeof(USB_CONFIGURATION_DESCRIPTOR))
#define USB_INTERFACE_DESC_SIZE     (sizeof(USB_INTERFACE_DESCRIPTOR))
#define USB_ENDPOINT_DESC_SIZE      (sizeof(USB_ENDPOINT_DESCRIPTOR))

extern const BYTE USB_DeviceDescriptor[];
extern const BYTE USB_ConfigDescriptor[];
extern const BYTE USB_StringDescriptor[];

#endif  /* __USBDESC_H__ */
