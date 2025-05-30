/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    usbcfg.h
 *      Purpose: USB custom configuration file for Philips LPC214x Family 
 *		Microprocessors
 *      Version: V1.04
 *----------------------------------------------------------------------------*/
#ifndef __USBCFG_H__
#define __USBCFG_H__


/*
//*** <<< Use Configuration Wizard in Context Menu >>> ***
*/


/*
// <o> USB Power
//   <i> Default Power Setting
//   <0=> Bus-powered
//   <1=> Self-powered
*/

#define USB_POWER           0


/*
// <h> USB Interface and Endpoint Configuration
//   <o0> Max Number of Interfaces <1-256>
//   <o1> Max Number of Endpoints  <1-32>
//   <o2> Max Endpoint 0 Packet Size
//        <8=> 8 Bytes <16=> 16 Bytes <32=> 32 Bytes <64=> 64 Bytes
// </h>
*/

#define USB_IF_NUM          4
#define USB_EP_NUM          32
#define USB_MAX_PACKET0     64
#define USB_VCOM_BUFSIZE	4		/* should be smaller than USB_MAX_PACKET0 */


/*
// <h> USB Event Callback Functions
//   <o0.0> Power Event
//   <o1.0> Reset Event
//   <o2.0> Suspend Event
//   <o3.0> Resume Event
//   <o4.0> Remote Wakeup Event
//   <o5.0> Start of Frame Event
//   <o6.0> Error Event
//   <h>  Endpoint Event Mask
//     <o7.0>  Endpoint 0 Event
//     <o7.1>  Endpoint 1 Event
//     <o7.2>  Endpoint 2 Event
//     <o7.3>  Endpoint 3 Event
//     <o7.4>  Endpoint 4 Event
//     <o7.5>  Endpoint 5 Event
//     <o7.6>  Endpoint 6 Event
//     <o7.7>  Endpoint 7 Event
//     <o7.8>  Endpoint 8 Event
//     <o7.9>  Endpoint 9 Event
//     <o7.10> Endpoint 10 Event
//     <o7.11> Endpoint 11 Event
//     <o7.12> Endpoint 12 Event
//     <o7.13> Endpoint 13 Event
//     <o7.14> Endpoint 14 Event
//     <o7.15> Endpoint 15 Event
//   </h>
// </h>
*/

#define USB_POWER_EVENT     0
#define USB_RESET_EVENT     1
#define USB_SUSPEND_EVENT   0
#define USB_RESUME_EVENT    0
#define USB_WAKEUP_EVENT    0
#define USB_SOF_EVENT       0
#define USB_ERROR_EVENT     0
/* Enable EPs from 0 to 11, the minimum setting should be 0x03,
that EP0 IN and OUT are enabled */ 
#define USB_EP_EVENT        0x003F


/*
// <h> USB Device Request Callbacks
//   <o0.0> Set Configuration Event
//   <o1.0> Set Interface Event
//   <o2.0> Set/Clear Feature Event
// </h>
*/

#define USB_CONFIGURE_EVENT 1
#define USB_INTERFACE_EVENT 0
#define USB_FEATURE_EVENT   0


/*
// <e0> USB Class Support
//   <e1> Human Interface Device (HID)
//     <o2> Interface Number <0-255>
//   </e>
// </e>
*/

#define USB_CLASS           1
#define USB_HID             0
#define USB_VCOM			1
#define USB_HID_IF_NUM      0


#endif  /* __USBCFG_H__ */
