/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    usbhw.c
 *      Purpose: USB Hardware layer module file for Philips LPC214x Family 
 *		Microprocessors
 *      Version: V1.04
 *----------------------------------------------------------------------------*/
#include <LPC214x.h>                        /* LPC214x definitions */

#include "type.h"
#include "usb.h"
#include "usbcfg.h"
#include "usbreg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "vcomuser.h"

/*
 *  Get Endpoint Address
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    Endpoint Physical Address
 */

DWORD EPAdr (DWORD EPNum) {
  DWORD val;

  val = (EPNum & 0x0F) << 1;
  if (EPNum & 0x80) {
    val += 1;
  }
  return (val);
}


/*
 *  Write Command
 *    Parameters:      cmd:   Command
 *    Return Value:    None
 */

void WrCmd (DWORD cmd) {
    
  CMD_CODE = cmd;
  while ((DEV_INT_STAT & CCEMTY_INT) == 0);    
  DEV_INT_CLR = CCEMTY_INT;
}


/*
 *  Write Command Data
 *    Parameters:      cmd:   Command
 *                     val:   Data
 *    Return Value:    None
 */

void WrCmdDat (DWORD cmd, DWORD val) {
    
  CMD_CODE = cmd;
  while ((DEV_INT_STAT & CCEMTY_INT) == 0);    
  DEV_INT_CLR = CCEMTY_INT;
  CMD_CODE = val;
  while ((DEV_INT_STAT & CCEMTY_INT) == 0);    
  DEV_INT_CLR = CCEMTY_INT;
}


/*
 *  Read Command Data
 *    Parameters:      cmd:   Command
 *    Return Value:    Data Value
 */

DWORD RdCmdDat (DWORD cmd) {
  DWORD val;

  DEV_INT_CLR = CDFULL_INT; 
  CMD_CODE = cmd;
  while ((DEV_INT_STAT & CCEMTY_INT) == 0);    
  DEV_INT_CLR = CCEMTY_INT; 
  while ((DEV_INT_STAT & CDFULL_INT) == 0);    
  val = CMD_DATA;
  DEV_INT_CLR = CDFULL_INT;
  return (val);
}


/*
 *  USB Initialize Function
 *    Return Value:    None
 */

void USB_Init (void) {

  PINSEL1 &= ~0xC000C000;
  PINSEL1 |=  0x80004000;                   /* Select USB Link, VBUS */
  PCONP |= 0x80000000;                      /* Turn On USB PCLK */

  // Configure 48MHz USB Clock;  FOsc = 12MHz, M = 4, P = 2
  PLL48CFG  = 0x23;                         /* M = 4, P = 2 */
  PLL48CON  = PLLCON_PLLE;                  /* PLL Enable */
  PLL48FEED = 0xAA;                         /* Feed Sequence 1 */
  PLL48FEED = 0x55;                         /* Feed Sequence 2 */
    
  while ((PLL48STAT & PLLSTAT_PLOCK) == 0); /* Wait for PLL Lock */

  PLL48CON  = PLLCON_PLLE | PLLCON_PLLC;    /* PLL Enable & Connect */
  PLL48FEED = 0xAA;                         /* Feed Sequence 1 */
  PLL48FEED = 0x55;                         /* Feed Sequence 2 */

  VICVectAddr0 = (unsigned long)USB_ISR;    /* USB Interrupt -> Vector 0 */
  VICVectCntl0 = 0x20 | 22;                 /* USB Interrupt -> IRQ Slot 0 */
  VICIntEnable = 1 << 22;                   /* Enable USB Interrupt */

  DEV_INT_EN = DEV_STAT_INT;                /* Enable Device Status Interrupt */

#if 1 /* Partial Manual Reset */
  USB_Reset();
  USB_SetAddress(0);
#endif
}


/*
 *  USB Connect Function
 *    Parameters:      con:   Connect/Disconnect
 *    Return Value:    None
 */

void USB_Connect (BOOL con) {
  WrCmdDat(CMD_SET_DEV_STAT, DAT_WR_BYTE(con ? DEV_CON : 0));
}


/*
 *  USB Reset Function
 *    Return Value:    None
 */

void USB_Reset (void) {

  EP_INDEX = 0;
  MAXPACKET_SIZE = USB_MAX_PACKET0;
  EP_INDEX = 1;
  MAXPACKET_SIZE = USB_MAX_PACKET0;
  while ((DEV_INT_STAT & EP_RLZED_INT) == 0);

  EP_INT_CLR  = 0xFFFFFFFF;
  EP_INT_EN   = 0xFFFFFFFF;
  DEV_INT_CLR = 0xFFFFFFFF;
  DEV_INT_EN  = DEV_STAT_INT    | EP_SLOW_INT    | 
               (USB_SOF_EVENT   ? FRAME_INT : 0) |
               (USB_ERROR_EVENT ? ERR_INT   : 0);
}


/*
 *  USB Suspend Function
 *    Return Value:    None
 */

void USB_Suspend (void) {
  // Performed by Hardware
}


/*
 *  USB Resume Function
 *    Return Value:    None
 */

void USB_Resume (void) {
  // Performed by Hardware
}


/*
 *  USB Remote Wakeup Function
 *    Return Value:    None
 */

void USB_WakeUp (void) {

  if (USB_DeviceStatus & USB_GETSTATUS_REMOTE_WAKEUP) {
    WrCmdDat(CMD_SET_DEV_STAT, DAT_WR_BYTE(DEV_CON));
  }
}


/*
 *  USB Remote Wakeup Configuration Function
 *    Parameters:      cfg:   Enable/Disable
 *    Return Value:    None
 */

void USB_WakeUpCfg (BOOL cfg) {
  cfg;  // Not needed
}


/*
 *  USB Set Address Function
 *    Parameters:      adr:   USB Address
 *    Return Value:    None
 */

void USB_SetAddress (BYTE adr) {
  WrCmdDat(CMD_SET_ADDR, DAT_WR_BYTE(DEV_EN | adr)); /* Don't wait for next */
  WrCmdDat(CMD_SET_ADDR, DAT_WR_BYTE(DEV_EN | adr)); /*  Setup Status Phase */
}


/*
 *  USB Configure Function
 *    Parameters:      cfg:   Configure/Deconfigure
 *    Return Value:    None
 */

void USB_Configure (BOOL cfg) {

  WrCmdDat(CMD_CFG_DEV, DAT_WR_BYTE(cfg ? CONF_DVICE : 0));

  REALIZE_EP = 0x00000003;
  while ((DEV_INT_STAT & EP_RLZED_INT) == 0);
  DEV_INT_CLR = EP_RLZED_INT;
}


/*
 *  Configure USB Endpoint
 *    Parameters:      pEPD:  Pointer to Endpoint Descriptor
 *    Return Value:    None
 */

void USB_ConfigEP (USB_ENDPOINT_DESCRIPTOR *pEPD) {
  DWORD num;

  num = EPAdr(pEPD->bEndpointAddress);
  REALIZE_EP |= (1 << num);
  EP_INDEX = num;
  MAXPACKET_SIZE = pEPD->wMaxPacketSize;
  while ((DEV_INT_STAT & EP_RLZED_INT) == 0);
  DEV_INT_CLR = EP_RLZED_INT;
}


/*
 *  Set Direction for USB Control Endpoint
 *    Parameters:      dir:   Out (dir == 0), In (dir <> 0)
 *    Return Value:    None
 */

void USB_DirCtrlEP (BYTE dir) {
  dir;  // Not needed
}


/*
 *  Enable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_EnableEP (BYTE EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(0));
}


/*
 *  Disable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_DisableEP (BYTE EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(EP_STAT_DA));
}


/*
 *  Reset USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_ResetEP (BYTE EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(0));
}


/*
 *  Set Stall for USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_SetStallEP (BYTE EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(EP_STAT_ST));
}


/*
 *  Clear Stall for USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_ClrStallEP (BYTE EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(0));
}


/*
 *  Read USB Endpoint Data
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *    Return Value:    Number of bytes read
 */

DWORD USB_ReadEP (BYTE EPNum, BYTE *pData) {
  DWORD cnt, n;

  USB_CTRL = ((EPNum & 0x0F) << 2) | CTRL_RD_EN;

  do {
    cnt = RX_PLENGTH;
  } while ((cnt & PKT_RDY) == 0);
  cnt &= PKT_LNGTH_MASK;

  for (n = 0; n < (cnt + 3) / 4; n++) {
    *((__packed DWORD *)pData) = RX_DATA;
    pData += 4;
  }

  USB_CTRL = 0;

  WrCmd(CMD_SEL_EP(EPAdr(EPNum)));
  WrCmd(CMD_CLR_BUF);

  return (cnt);
}


/*
 *  Write USB Endpoint Data
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *                     cnt:   Number of bytes to write
 *    Return Value:    Number of bytes written
 */

DWORD USB_WriteEP (BYTE EPNum, BYTE *pData, DWORD cnt) {
  DWORD n;

  USB_CTRL = ((EPNum & 0x0F) << 2) | CTRL_WR_EN;

  TX_PLENGTH = cnt;

  for (n = 0; n < (cnt + 3) / 4; n++) {
    TX_DATA = *((__packed DWORD *)pData);
    pData += 4;
  }

  USB_CTRL = 0;

  WrCmd(CMD_SEL_EP(EPAdr(EPNum)));
  WrCmd(CMD_VALID_BUF);

  return (cnt);
}


/*
 *  USB Interrupt Service Routine
 */

void USB_ISR (void) __irq {
  DWORD disr, eisr, val, n, m;

  disr = DEV_INT_STAT;                      /* Device Interrupt Status */
    
  // Device Status Interrupt (Reset, Suspend/Resume, Connect change)
  if (disr & DEV_STAT_INT) {
    WrCmd(CMD_GET_DEV_STAT);
    val = RdCmdDat(DAT_GET_DEV_STAT);       /* Device Status */
    if (val & DEV_RST) {                    /* Reset */
      USB_Reset();
#if   USB_RESET_EVENT
      USB_Reset_Event();
#endif
      goto isr_end;
    }
    if (val & DEV_SUS_CH) {                 /* Suspend/Resume */
      if (val & DEV_SUS) {                  /* Suspend */
        USB_Suspend();
#if     USB_SUSPEND_EVENT
        USB_Suspend_Event();
#endif
      } else {                              /* Resume */
        USB_Resume();
#if     USB_RESUME_EVENT
        USB_Resume_Event();
#endif
      }
      goto isr_end;
    }
    if (val & DEV_CON_CH) {                 /* Connect change */
#if   USB_POWER_EVENT
      USB_Power_Event(val & DEV_CON);
#endif
      goto isr_end;
    }
  }

#if USB_SOF_EVENT
  // Start of Frame Interrupt
  if (disr & FRAME_INT) {
    WrCmd(CMD_RD_FRAME);
    val = RdCmdDat(DAT_RD_FRAME);
    val = val | (RdCmdDat(DAT_RD_FRAME) << 8);
    USB_SOF_Event(val);
  }
#endif

#if USB_ERROR_EVENT
  // Error Interrupt
  if (disr & ERR_INT) {
    WrCmd(CMD_RD_ERR_STAT);
    val = RdCmdDat(DAT_RD_ERR_STAT);
    if        (val & 0x01) {
      val = USB_ERR_PID;
    } else if (val & 0x02) {
      val = 0x100;  // Unexpected Error
    } else if (val & 0x04) {
      val = USB_ERR_CRC;
    } else if (val & 0x08) {
      val = USB_ERR_TIMEOUT;
    } else if (val & 0x10) {
      val = USB_ERR_EOP;
    } else if (val & 0x20) {
      val = 0x101;  // Buffer Overrun
    } else if (val & 0x40) {
      val = USB_ERR_BIT_STUFF;
    } else if (val & 0x80) {
      val = USB_ERR_DATA_TOGGLE;
    }
    USB_Error_Event(val);
  }
#endif

  // Ednpoint's Slow Interrupt
  if (disr & EP_SLOW_INT) {
    
	while ( eisr = EP_INT_STAT ) {           /* Endpoint Interrupt Status */
      // Check All Endpoints
      for (n = 0; n < USB_EP_NUM; n++) {
        if (eisr & (1 << n)) {
          m = n >> 1;
		    
		  EP_INT_CLR = 1 << n;
          while ((DEV_INT_STAT & CDFULL_INT) == 0);
          val = CMD_DATA;
          DEV_INT_CLR = CDFULL_INT;

          if ((n & 1) == 0) {
            // OUT Endpoint
            if (n == 0) {                     /* Control OUT Endpoint */
              if (val & EP_SEL_STP) {         /* Setup Packet */
                if (USB_P_EP[0]) {
                  USB_P_EP[0](USB_EVT_SETUP);
				  continue;
                }
              }
            }
            if (USB_P_EP[m]) {
              USB_P_EP[m](USB_EVT_OUT);
            }
          } else {
            // IN Endpoint
            if (USB_P_EP[m]) {
              USB_P_EP[m](USB_EVT_IN);
            }
          }
		}
      }
    }
  }

isr_end:
  DEV_INT_CLR = disr;
  VICVectAddr = 0;                          /* Acknowledge Interrupt */
}
