/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    DEMO.h
 *      Purpose: USB Virtual COM demo definiton file for Philips LPC214x
 *      Version: V1.04
 *----------------------------------------------------------------------------*/
/* Push Button Definitions */
  #define SW1   (IO0PIN & 0x00008000)	 //Macro Functions to read Push button switches on LPC2148 dev. board
  #define SW2	(IO0PIN & 0x00002000)
  #define SW3	(IO0PIN & 0x00001000)
  #define SW4	(IO0PIN & 0x40000000)


/* LED Definitions */
#define LED1   0x00010000  /* P1.16 */
#define LED2   0x00020000  /* P1.17 */
#define LED3   0x00040000  /* P1.18 */
#define LED4   0x00080000  /* P1.19 */



