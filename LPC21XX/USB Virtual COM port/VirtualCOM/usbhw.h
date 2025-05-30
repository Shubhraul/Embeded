/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    usbhw.h
 *      Purpose: USB Hardware layer header file for Philips LPC214x Family 
 *		Microprocessors
 *      Version: V1.04
 *----------------------------------------------------------------------------*/
#ifndef __USBHW_H__
#define __USBHW_H__


/* USB Hardware Functions */
extern void  USB_Init       (void);
extern void  USB_Connect    (BOOL con);
extern void  USB_Reset      (void);
extern void  USB_Suspend    (void);
extern void  USB_Resume     (void);
extern void  USB_WakeUp     (void);
extern void  USB_WakeUpCfg  (BOOL cfg);
extern void  USB_SetAddress (BYTE adr);
extern void  USB_Configure  (BOOL cfg);
extern void  USB_ConfigEP   (USB_ENDPOINT_DESCRIPTOR *pEPD);
extern void  USB_DirCtrlEP  (BYTE dir);
extern void  USB_EnableEP   (BYTE EPNum);
extern void  USB_DisableEP  (BYTE EPNum);
extern void  USB_ResetEP    (BYTE EPNum);
extern void  USB_SetStallEP (BYTE EPNum);
extern void  USB_ClrStallEP (BYTE EPNum);
extern DWORD USB_ReadEP     (BYTE EPNum, BYTE *pData);
extern DWORD USB_WriteEP    (BYTE EPNum, BYTE *pData, DWORD cnt);
extern void  USB_ISR        (void) __irq;


#endif  /* __USBHW_H__ */
