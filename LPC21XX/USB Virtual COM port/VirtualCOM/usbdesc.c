/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    usbdesc.c
 *      Purpose: USB Descriptors file for Philips LPC214x Family 
 *		Microprocessors
 *      Version: V1.04
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express, 
 *      implied or statutory, including but not limited to the implied 
 *      warranties of fitness for purpose, satisfactory quality and 
 *      noninfringement. Keil extends you a royalty-free right to reproduce and
 *      distribute executable files created using this software for use on 
 *      Philips LPC2xxx microcontroller devices only. Nothing else gives you the 
 *      right to use this software. 
 *
 *      Copyright (c) 2005 Keil Software.
 *		Modified by Philips Semiconductor
 *---------------------------------------------------------------------------*/
#include "type.h"
#include "usb.h"
#include "usbcfg.h"
#include "usbdesc.h"

/* Two interface descriptor for 2 Virtual COM port,, each VCOM uses 3 endpoints */
/* The total number of endpoints are 6, 2 interrupt IN for modem status reading,
2 BULK OUT for COM TX, and 2 BULK IN for COM RX */
 
#define NUM_ENDPOINTS	3 

/* USB Standard Device Descriptor */
const BYTE USB_DeviceDescriptor[] = {
  USB_DEVICE_DESC_SIZE,              /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0100), /* 1.00 */          /* bcdUSB */
  0x00,                              /* bDeviceClass */
  0x00,                              /* bDeviceSubClass */
  0x00,                              /* bDeviceProtocol */
  USB_MAX_PACKET0,                   /* bMaxPacketSize0 */
  WBVAL(0xC251),                     /* idVendor */
  WBVAL(0x1305),                     /* idProduct */
  WBVAL(0x0110), /* 1.10 */          /* bcdDevice */
  0x04,                              /* iManufacturer */
  0x20,                              /* iProduct */
  0x4A,                              /* iSerialNumber */
  0x01                               /* bNumConfigurations */
};

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const BYTE USB_ConfigDescriptor[] = {
/* Configuration 1 */
  USB_CONFIGUARTION_DESC_SIZE,       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(                             /* wTotalLength */
    USB_CONFIGUARTION_DESC_SIZE +
    USB_INTERFACE_DESC_SIZE     +
	NUM_ENDPOINTS * USB_ENDPOINT_DESC_SIZE +
    USB_INTERFACE_DESC_SIZE     +
    NUM_ENDPOINTS * USB_ENDPOINT_DESC_SIZE
  ),
  0x02,                              /* bNumInterfaces */
  0x01,                              /* bConfigurationValue */
  0x00,                              /* iConfiguration */
  USB_CONFIG_BUS_POWERED |           /* bmAttributes */
  USB_CONFIG_REMOTE_WAKEUP,
  USB_CONFIG_POWER_MA(100),          /* bMaxPower */
/* Interface 0, Alternate Setting 0, Class Code Unknown */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x00,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  NUM_ENDPOINTS,                     /* bNumEndpoints */
  USB_DEVICE_CLASS_VENDOR_SPECIFIC,  /* bInterfaceClass, USB_CLASS_VENDOR_SPECIFIC */
  0xFF,                              /* bInterfaceSubClass, USB_SUBCLASS_CODE_UNKNOWN */
  0xFF,                              /* bInterfaceProtocol, USB_PROTOCOL_CODE_UNKNOWN */
  0x00,                              /* iInterface, STR_INDEX_INTERFACE = no_string */
/* Endpoint, EP1 Interrupt In */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_IN(1),                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_INTERRUPT,       /* bmAttributes */
  WBVAL(0x0004),                     /* wMaxPacketSize */
  0x20,          /* 32ms */          /* bInterval */
/* Endpoint, EP2 Bulk Out */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_OUT(2),               /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(0x0040),                     /* wMaxPacketSize */
  0x20,          /* 32ms */
/* Endpoint, EP2 Bulk In */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_IN(2),                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(0x0040),                     /* wMaxPacketSize */
  0x20,          /* 32ms */
/* Interface 1, Alternate Setting 0, Class Code Unknown */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x01,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  NUM_ENDPOINTS,                     /* bNumEndpoints */
  USB_DEVICE_CLASS_VENDOR_SPECIFIC,  /* bInterfaceClass, USB_CLASS_CODE_UNKNOWN */
  0xFF,                              /* bInterfaceSubClass, USB_SUBCLASS_CODE_UNKNOWN */
  0xFF,                              /* bInterfaceProtocol, USB_PROTOCOL_CODE_UNKNOWN */
  0x00,                              /* iInterface, STR_INDEX_INTERFACE = no_string */
/* Endpoint, EP4 Interrupt In */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_IN(4),                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_INTERRUPT,       /* bmAttributes */
  WBVAL(0x0004),                     /* wMaxPacketSize */
  0x20,          /* 32ms */          /* bInterval */
/* Endpoint, EP5 Bulk Out */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_OUT(5),               /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(0x0040),                     /* wMaxPacketSize */
  0x20,          /* 32ms */
/* Endpoint, EP5 Bulk In */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_IN(5),                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(0x0040),                     /* wMaxPacketSize */
  0x20,          /* 32ms */
/* Terminator */
  0                                  /* bLength */
};

/* USB String Descriptor (optional) */
const BYTE USB_StringDescriptor[] = {
/* Index 0x00: LANGID Codes */
  0x04,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0409), /* US English */    /* wLANGID */
/* Index 0x04: Manufacturer */
  0x1C,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'N',0,
  'e',0,
  'x',0,
  ' ',0,
  'R',0,
  'o',0,
  'b',0,
  'o',0,
  't',0,
  'i',0,
  'c',0,
  's',0,
  ' ',0,
/* Index 0x20: Product */
  0x2A,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'L',0,
  'P',0,
  'C',0,
  '2',0,
  '1',0,
  '4',0,
  '8',0,
  ' ',0,
  'V',0,
  'i',0,
  'r',0,
  't',0,
  'u',0,
  'a',0,
  'l',0,
  ' ',0,
  'C',0,
  'O',0,
  'M',0,
  ' ',0,
/* Index 0x4A: Serial Number */
  0x1C,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '-',0,
  'D',0,
  'E',0,
  'M',0,
  'O',0,
/* Index 0x66: Interface 0, Alternate Setting 0 */
  0x0A,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'V',0,
  'C',0,
  'O',0,
  'M',0,
};
