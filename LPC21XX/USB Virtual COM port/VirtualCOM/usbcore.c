/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    usbcore.c
 *      Purpose: USB Core Module file for Philips LPC214x Family 
 *		Microprocessors
 *      Version: V1.04
 *----------------------------------------------------------------------------*/
#include "type.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbdesc.h"
#include "vcomuser.h"


WORD  USB_DeviceStatus;
BYTE  USB_DeviceAddress;
BYTE  USB_Configuration;
DWORD USB_EndPointMask;
DWORD USB_EndPointHalt;
BYTE  USB_NumInterfaces;
BYTE  USB_AltSetting[USB_IF_NUM];

BYTE  EP0Buf[USB_MAX_PACKET0];

USB_EP_DATA EP0Data;

USB_SETUP_PACKET SetupPacket;


/*
 *  Reset USB Core
 *    Parameters:      None
 *    Return Value:    None
 */

void USB_ResetCore (void) {

  USB_DeviceStatus  = USB_POWER;
  USB_DeviceAddress = 0;
  USB_Configuration = 0;
  USB_EndPointMask  = 0x00010001;
  USB_EndPointHalt  = 0x00000000;
}


/*
 *  USB Request - Setup Stage
 *    (global SetupPacket)
 *    Parameters:      None
 *    Return Value:    None
 */

void USB_SetupStage (void) {
  USB_ReadEP(0x00, (BYTE *)&SetupPacket);
}


/*
 *  USB Request - Data In Stage
 *    Parameters:      None (global EP0Data)
 *    Return Value:    None
 */

void USB_DataInStage (void) {
  DWORD cnt;

  if (EP0Data.Count > USB_MAX_PACKET0) {
    cnt = USB_MAX_PACKET0;
  } else {
    cnt = EP0Data.Count;
  }
  cnt = USB_WriteEP(0x80, EP0Data.pData, cnt);
  EP0Data.pData += cnt;
  EP0Data.Count -= cnt;
}


/*
 *  USB Request - Data Out Stage
 *    Parameters:      None (global EP0Data)
 *    Return Value:    None
 */

void USB_DataOutStage (void) {
  DWORD cnt;

  cnt = USB_ReadEP(0x00, EP0Data.pData);
  EP0Data.pData += cnt;
  EP0Data.Count -= cnt;
}


/*
 *  USB Request - Status In Stage
 *    Parameters:      None
 *    Return Value:    None
 */

void USB_StatusInStage (void) {
  USB_WriteEP(0x80, NULL, 0);
}


/*
 *  USB Request - Status Out Stage
 *    Parameters:      None
 *    Return Value:    None
 */

void USB_StatusOutStage (void) {
  USB_ReadEP(0x00, EP0Buf);
}


/*
 *  Get Status USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

__inline BOOL USB_GetStatus (void) {
  DWORD n, m;

  switch (SetupPacket.bmRequestType.BM.Recipient) {
    case REQUEST_TO_DEVICE:
      EP0Data.pData = (BYTE *)&USB_DeviceStatus;
      USB_DataInStage();
      break;
    case REQUEST_TO_INTERFACE:
      if ((USB_Configuration != 0) && (SetupPacket.wIndex.WB.L < USB_NumInterfaces)) {
        *((WORD *)EP0Buf) = 0;
        EP0Data.pData = EP0Buf;
        USB_DataInStage();
      } else {
        return (FALSE);
      }
      break;
    case REQUEST_TO_ENDPOINT:
      n = SetupPacket.wIndex.WB.L & 0x8F;
      m = (n & 0x80) ? ((1 << 16) << n) : (1 << n);
      if (((USB_Configuration != 0) || ((n & 0x0F) == 0)) && (USB_EndPointMask & m)) {
        *((WORD *)EP0Buf) = (USB_EndPointHalt & m) ? 1 : 0;
        EP0Data.pData = EP0Buf;
        USB_DataInStage();
      } else {
        return (FALSE);
      }
      break;
    default:
      return (FALSE);
  }
  return (TRUE);
}


/*
 *  Set/Clear Feature USB Request
 *    Parameters:      sc:    0 - Clear, 1 - Set
 *                            None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

__inline BOOL USB_SetClrFeature (DWORD sc) {
  DWORD n, m;

  switch (SetupPacket.bmRequestType.BM.Recipient) {
    case REQUEST_TO_DEVICE:
      if (SetupPacket.wValue.W == USB_FEATURE_REMOTE_WAKEUP) {
        if (sc) {
          USB_WakeUpCfg(1);
          USB_DeviceStatus |=  USB_GETSTATUS_REMOTE_WAKEUP;
        } else {
          USB_WakeUpCfg(0);
          USB_DeviceStatus &= ~USB_GETSTATUS_REMOTE_WAKEUP;
        }
      } else {
        return (FALSE);
      }
      break;
    case REQUEST_TO_INTERFACE:
      return (FALSE);
    case REQUEST_TO_ENDPOINT:
      n = SetupPacket.wIndex.WB.L & 0x8F;
      m = (n & 0x80) ? ((1 << 16) << n) : (1 << n);
      if ((USB_Configuration != 0) && ((n & 0x0F) != 0) && (USB_EndPointMask & m)) {
        if (SetupPacket.wValue.W == USB_FEATURE_ENDPOINT_STALL) {
          if (sc) {
            USB_SetStallEP(n);
            USB_EndPointHalt |=  m;
          } else {
            USB_ClrStallEP(n);
            USB_EndPointHalt &= ~m;
          }
        } else {
          return (FALSE);
        }
      } else {
        return (FALSE);
      }
      break;
    default:
      return (FALSE);
  }
  return (TRUE);
}


/*
 *  Get Descriptor USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

__inline BOOL USB_GetDescriptor (void) {
  BYTE  *pD;
  DWORD len, n;

  switch (SetupPacket.bmRequestType.BM.Recipient) {
    case REQUEST_TO_DEVICE:
      switch (SetupPacket.wValue.WB.H) {
        case USB_DEVICE_DESCRIPTOR_TYPE:
          EP0Data.pData = (BYTE *)USB_DeviceDescriptor;
          len = USB_DEVICE_DESC_SIZE;
          break;
        case USB_CONFIGURATION_DESCRIPTOR_TYPE:
          pD = (BYTE *)USB_ConfigDescriptor;
          for (n = 0; n != SetupPacket.wValue.WB.L; n++) {
            if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bLength != 0) {
              pD += ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
            }
          }
          if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bLength == 0) {
            return (FALSE);
          }
          EP0Data.pData = pD;
          len = ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
          break;
        case USB_STRING_DESCRIPTOR_TYPE:
          EP0Data.pData = (BYTE *)USB_StringDescriptor + SetupPacket.wValue.WB.L;
          len = ((USB_STRING_DESCRIPTOR *)EP0Data.pData)->bLength;
          break;
        default:
          return (FALSE);
      }
      break;
    case REQUEST_TO_INTERFACE:
      switch (SetupPacket.wValue.WB.H) {
        default:
          return (FALSE);
      }
      break;
    default:
      return (FALSE);
  }

  if (EP0Data.Count > len) {
    EP0Data.Count = len;
  }
  USB_DataInStage();

  return (TRUE);
}


/*
 *  Set Configuration USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

__inline BOOL USB_SetConfiguration (void) {
  USB_COMMON_DESCRIPTOR *pD;
  DWORD                  alt, n, m;

  if (SetupPacket.wValue.WB.L) {
    pD = (USB_COMMON_DESCRIPTOR *)USB_ConfigDescriptor;
    while (pD->bLength) {
      switch (pD->bDescriptorType) {
        case USB_CONFIGURATION_DESCRIPTOR_TYPE:
          if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bConfigurationValue == SetupPacket.wValue.WB.L) {
            USB_Configuration = SetupPacket.wValue.WB.L;
            USB_NumInterfaces = ((USB_CONFIGURATION_DESCRIPTOR *)pD)->bNumInterfaces;
            for (n = 0; n < USB_IF_NUM; n++) {
              USB_AltSetting[n] = 0;
            }
            for (n = 1; n < 16; n++) {
              if (USB_EndPointMask & (1 << n)) {
                USB_DisableEP(n);
              }
              if (USB_EndPointMask & ((1 << 16) << n)) {
                USB_DisableEP(n | 0x80);
              }
            }
            USB_EndPointMask = 0x00010001;
            USB_EndPointHalt = 0x00000000;
            USB_Configure(TRUE);
            if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bmAttributes & USB_CONFIG_SELF_POWERED) {
              USB_DeviceStatus |=  USB_GETSTATUS_SELF_POWERED;
            } else {
              USB_DeviceStatus &= ~USB_GETSTATUS_SELF_POWERED;
            }
          } else {
            (BYTE *)pD += ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
            continue;
          }
          break;
        case USB_INTERFACE_DESCRIPTOR_TYPE:
          alt = ((USB_INTERFACE_DESCRIPTOR *)pD)->bAlternateSetting;
          break;
        case USB_ENDPOINT_DESCRIPTOR_TYPE:
          if (alt == 0) {
            n = ((USB_ENDPOINT_DESCRIPTOR *)pD)->bEndpointAddress & 0x8F;
            m = (n & 0x80) ? ((1 << 16) << n) : (1 << n);
            USB_EndPointMask |= m;
            USB_ConfigEP((USB_ENDPOINT_DESCRIPTOR *)pD);
            USB_EnableEP(n);
            USB_ResetEP(n);
          }
          break;
      }
      (BYTE *)pD += pD->bLength;
    }
  }
  else {
    USB_Configuration = 0;
    for (n = 1; n < 16; n++) {
      if (USB_EndPointMask & (1 << n)) {
        USB_DisableEP(n);
      }
      if (USB_EndPointMask & ((1 << 16) << n)) {
        USB_DisableEP(n | 0x80);
      }
    }
    USB_EndPointMask  = 0x00010001;
    USB_EndPointHalt  = 0x00000000;
    USB_Configure(FALSE);
  }

  if (USB_Configuration == SetupPacket.wValue.WB.L) {
    return (TRUE);
  } else {
    return (FALSE);
  }
}


/*
 *  Set Interface USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

__inline BOOL USB_SetInterface (void) {
  USB_COMMON_DESCRIPTOR *pD;
  DWORD                  ifn, alt, old, msk, n, m;
  BOOL                   set;

  if (USB_Configuration == 0) return (FALSE);

  set = FALSE;
  pD  = (USB_COMMON_DESCRIPTOR *)USB_ConfigDescriptor;
  while (pD->bLength) {
    switch (pD->bDescriptorType) {
      case USB_CONFIGURATION_DESCRIPTOR_TYPE:
        if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bConfigurationValue != USB_Configuration) {
          (BYTE *)pD += ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
          continue;
        }
        break;
      case USB_INTERFACE_DESCRIPTOR_TYPE:
        ifn = ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber;
        alt = ((USB_INTERFACE_DESCRIPTOR *)pD)->bAlternateSetting;
        msk = 0;
        if ((ifn == SetupPacket.wIndex.WB.L) && (alt == SetupPacket.wValue.WB.L)) {
          set = TRUE;
          old = USB_AltSetting[ifn];
          USB_AltSetting[ifn] = (BYTE)alt;
        }
        break;
      case USB_ENDPOINT_DESCRIPTOR_TYPE:
        if (ifn == SetupPacket.wIndex.WB.L) {
          n = ((USB_ENDPOINT_DESCRIPTOR *)pD)->bEndpointAddress & 0x8F;
          m = (n & 0x80) ? ((1 << 16) << n) : (1 << n);
          if (alt == SetupPacket.wValue.WB.L) {
            USB_EndPointMask |=  m;
            USB_EndPointHalt &= ~m;
            USB_ConfigEP((USB_ENDPOINT_DESCRIPTOR *)pD);
            USB_EnableEP(n);
            USB_ResetEP(n);
            msk |= m;
          }
          else if ((alt == old) && ((msk & m) == 0)) {
            USB_EndPointMask &= ~m;
            USB_EndPointHalt &= ~m;
            USB_DisableEP(n);
          }
        }
        break;
    }
    (BYTE *)pD += pD->bLength;
  }
  return (set);
}


/*
 *  USB Endpoint 0 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint0 (DWORD event) {

  BYTE UARTSettingCmd, UARTSettingData;

  switch (event) {
    case USB_EVT_SETUP:
      USB_SetupStage();
      USB_DirCtrlEP(SetupPacket.bmRequestType.BM.Dir);
      EP0Data.Count = SetupPacket.wLength;
      switch (SetupPacket.bmRequestType.BM.Type) {

        case REQUEST_STANDARD:
          switch (SetupPacket.bRequest) {

            case USB_REQUEST_GET_STATUS:
              if (!USB_GetStatus()) {
                goto stall_i;
              }
              break;

            case USB_REQUEST_CLEAR_FEATURE:
              if (!USB_SetClrFeature(0)) {
                goto stall_i;
              }
              USB_StatusInStage();
#if USB_FEATURE_EVENT
              USB_Feature_Event();
#endif
              break;              

            case USB_REQUEST_SET_FEATURE:
              if (!USB_SetClrFeature(1)) {
                goto stall_i;
              }
              USB_StatusInStage();
#if USB_FEATURE_EVENT
              USB_Feature_Event();
#endif
              break;              

            case USB_REQUEST_SET_ADDRESS:
              switch (SetupPacket.bmRequestType.BM.Recipient) {
                case REQUEST_TO_DEVICE:
                  USB_DeviceAddress = 0x80 | SetupPacket.wValue.WB.L;
                  USB_StatusInStage();
                  break;
                default:
                  goto stall_i;
              }
              break;

            case USB_REQUEST_GET_DESCRIPTOR:
              if (!USB_GetDescriptor()) {
                goto stall_i;
              }                  
              break;              

            case USB_REQUEST_SET_DESCRIPTOR:
/*stall_o:*/  USB_SetStallEP(0x00);
              EP0Data.Count = 0;
              break;

            case USB_REQUEST_GET_CONFIGURATION:
              switch (SetupPacket.bmRequestType.BM.Recipient) {
                case REQUEST_TO_DEVICE:
                  EP0Data.pData = &USB_Configuration;
                  USB_DataInStage();
                  break;
                default:
                  goto stall_i;
              }
              break;

            case USB_REQUEST_SET_CONFIGURATION:
              switch (SetupPacket.bmRequestType.BM.Recipient) {
                case REQUEST_TO_DEVICE:
                  if (!USB_SetConfiguration()) {
                    goto stall_i;
                  }
                  USB_StatusInStage();
#if USB_CONFIGURE_EVENT
                  USB_Configure_Event();
#endif
                  break;
                default:
                  goto stall_i;
              }
              break;              

            case USB_REQUEST_GET_INTERFACE:
              switch (SetupPacket.bmRequestType.BM.Recipient) {
                case REQUEST_TO_INTERFACE:
                  if ((USB_Configuration != 0) && 
                      (SetupPacket.wIndex.WB.L < USB_NumInterfaces)) {
                    EP0Data.pData = USB_AltSetting + SetupPacket.wIndex.WB.L;
                    USB_DataInStage();
                  } else {
                    goto stall_i;
                  }
                  break;
                default:
                  goto stall_i;
              }
              break;              

            case USB_REQUEST_SET_INTERFACE:
              switch (SetupPacket.bmRequestType.BM.Recipient) {
                case REQUEST_TO_INTERFACE:
                  if (!USB_SetInterface()) {
                    goto stall_i;
                  }
                  USB_StatusInStage();
#if USB_INTERFACE_EVENT
                  USB_Interface_Event();
#endif
                  break;
                default:
                  goto stall_i;
              }
              break;              

            default:
              goto stall_i;

          }
          break;

        case REQUEST_CLASS:
#if USB_CLASS
          switch (SetupPacket.bmRequestType.BM.Recipient) {
            case REQUEST_TO_INTERFACE:
//            break;              
            default:
              goto stall_i;
          }              
#else
          goto stall_i;
#endif  /* USB_CLASS */

        case REQUEST_VENDOR:
#if USB_VCOM
		  UARTSettingCmd = SetupPacket.bRequest & 0xFF;
		  UARTSettingData = SetupPacket.wValue.WB.L;
		  if (!VCOM_SetSIOSetup( UARTSettingCmd, UARTSettingData )) {
			goto stall_i;
		  }
		  USB_DataInStage();
#endif      
        break;

        default:
stall_i:  USB_SetStallEP(0x80);
          EP0Data.Count = 0;
          break;

      }
      break;

    case USB_EVT_OUT:
      if (SetupPacket.bmRequestType.BM.Dir == 0) {
        if (EP0Data.Count) {
          USB_DataOutStage();
          if (EP0Data.Count == 0) {
            switch (*((WORD *)&SetupPacket)) {
              default:
                goto stall_i;
            }
            USB_StatusInStage();
          }
        }
      } else {
        USB_StatusOutStage();
      }
      break;

    case USB_EVT_IN:
      if (SetupPacket.bmRequestType.BM.Dir == 1) {
        if (EP0Data.Count) {
          USB_DataInStage();
        }
      } else {
        if (USB_DeviceAddress & 0x80) {
          USB_DeviceAddress &= 0x7F;
          USB_SetAddress(USB_DeviceAddress);
        }
      }
      break;

    case USB_EVT_IN_STALL:
      USB_ClrStallEP(0x80);
      break;

    case USB_EVT_OUT_STALL:
      USB_ClrStallEP(0x00);
      break;

  }
}
