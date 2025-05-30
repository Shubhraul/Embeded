#include <LPC214X.H> /* LPC214x definitions */
#include "type.h"
#include "spi_mmc.h"


BYTE MMCWRData[MMC_DATA_SIZE]={"\nNEX ROBOTICS PVT LTD\nARM7 LPC2148 DEVELOPMENT BOARD\nSD/MMC SPI INTERFACE TEST"};
BYTE MMCRDData[MMC_DATA_SIZE];
BYTE MMCCmd[MMC_CMD_SIZE];
BYTE MMCStatus = 0;


extern const unsigned char Test[5120];
extern unsigned char Demo[5120];
extern unsigned char Name[];
/*
* SPI and MMC commands related modules.
*
*/
void SPI_Init( void )
{
 DWORD portConfig;
 BYTE i, Dummy;
 
 portConfig = PINSEL0;			  /* Configure PIN connect block */
 PINSEL0 = portConfig | 0x1500;   /* P0.4, P0.5, P0.6, P0.7 are set as SCK, MISO, MOSI and GPIO */
 
 IODIR0 = SPI_SEL; 				  /* SSEL is output */
 IOSET0 = SPI_SEL; 				  /* set SSEL to high */
 VPBDIV = 0x02;					  /* Set PCLK 1/2 of CCLK. CCLK=60MHz, PCLK=30MHz*/ 	
 S0SPCCR=8;						  /* SPI clock prescale register minimum value is 8. */
 S0SPCR=0x0030;					  /* Device select as master, Set data to 8-bit, CPOL = 0, CPHA = 0*/ 
 for ( i = 0; i < 8; i++ )		  
 {
 Dummy = S0SPDR; 				  /* clear the RxFIFO */
 }
 return;
}


 /*
 * SPI Send a block of data based on the length
 */
void SPI_Send( BYTE *buf, DWORD Length )
{
 BYTE Dummy;
 if ( Length == 0 )
 return;
 while ( Length != 0 )
 {
  S0SPDR = *buf;
  while ( !(S0SPSR & 0x80) );	/* Wait until the SPIF bit is set to indicate trabsfer complete */
  Dummy = S0SPDR; 				/* Flush the RxFIFO */
  Length--;
  buf++;
 }
 return;
}


/*
* SPI receives a block of data based on the length
*/
void SPI_Receive( BYTE *buf, DWORD Length )
{
 DWORD i;
 for ( i = 0; i < Length; i++ )
 {
  *buf = SPI_ReceiveByte();
  buf++;
 }
 return;
}



/*
* SPI Receive Byte, receive one byte only, return Data byte
* used a lot to check the status.
*/
BYTE SPI_ReceiveByte( void )
{
 BYTE data;
 S0SPDR = 0xFF;					/* wrtie dummy byte out to generate clock, then read data from  MISO */
 while (!(S0SPSR & 0x80) );		/* Wait until the SPIF bit is set to indicate trabsfer complete */
 data = S0SPDR;
 return ( data );
}



/************************** MMC Init *********************************/
/*
* Initialises the MMC into SPI mode and sets block size(512), returns
* 0 on success
*
*/

int mmc_init()
{
 DWORD i;
 MMCStatus = 0;
 IOSET0 = SPI_SEL; 		/* set SPI SSEL */
 
 for(i=0; i<10; i++)	/* initialise the MMC card into SPI mode by sending 80 clks and keep MOSI line high */
 {
  MMCRDData[i] = 0xFF;	 /* Use MMCRDData as a temporary buffer for SPI_Send() */
 }
 SPI_Send( MMCRDData, 10 );
 IOCLR0 = SPI_SEL; 		 /* clear SPI SSEL */
 MMCCmd[0] = 0x40;		 /* send CMD0(RESET or GO_IDLE_STATE) command, all the arguments are 0x00 for the reset command, precalculated checksum */
 MMCCmd[1] = 0x00;
 MMCCmd[2] = 0x00;
 MMCCmd[3] = 0x00;
 MMCCmd[4] = 0x00;
 MMCCmd[5] = 0x95;
 SPI_Send( MMCCmd, MMC_CMD_SIZE );
 if( mmc_response(0x01) == 1 )	/* if = 1 then there was a timeout waiting for 0x01 from the MMC */
 {
  MMCStatus = IDLE_STATE_TIMEOUT;
  IOSET0 = SPI_SEL; /* set SPI SSEL */
  return MMCStatus;
 }
 
 IOSET0 = SPI_SEL; /* set SPI SSEL */	   /* Send some dummy clocks after GO_IDLE_STATE */
 SPI_ReceiveByte();
 IOCLR0 = SPI_SEL; /* clear SPI SSEL */
 
 i = MAX_TIMEOUT;			   /* must keep sending command until zero response ia back. */
 do
 {
  MMCCmd[0] = 0x41;		/* send mmc CMD1(SEND_OP_COND) to bring out of idle state */ /* all the arguments are 0x00 for command one */
  MMCCmd[1] = 0x00;
  MMCCmd[2] = 0x00;
  MMCCmd[3] = 0x00;
  MMCCmd[4] = 0x00;
  MMCCmd[5] = 0xFF;		/* checksum is no longer required but we always send 0xFF */
  SPI_Send( MMCCmd, MMC_CMD_SIZE );
  i--;
 } while ( (mmc_response(0x00) != 0) && (i>0) );
 
 if ( i == 0 )			/* timeout waiting for 0x00 from the MMC */
 {
  MMCStatus = OP_COND_TIMEOUT;
  IOSET0 = SPI_SEL; /* set SPI SSEL */
  return MMCStatus;
 }
 
 IOSET0 = SPI_SEL; /* set SPI SSEL */		  /* Send some dummy clocks after SEND_OP_COND */
 SPI_ReceiveByte();
 IOCLR0 = SPI_SEL; /* clear SPI SSEL */
 
 MMCCmd[0] = 0x50;	   /* send MMC CMD16(SET_BLOCKLEN) to set the block length */
 MMCCmd[1] = 0x00; 	   /* 4 bytes from here is the block length */
 MMCCmd[2] = 0x00;	   /* LSB is first */
 MMCCmd[3] = 0x02;     /* high block length bits - 512 bytes */
 MMCCmd[4] = 0x00;	   /* low block length bits */
 MMCCmd[5] = 0xFF;     /* checksum is no longer required but we always send 0xFF */
 SPI_Send( MMCCmd, MMC_CMD_SIZE );
 if( (mmc_response(0x00))==1 )
 {
  MMCStatus = SET_BLOCKLEN_TIMEOUT;
  IOSET0 = SPI_SEL; /* set SPI SSEL */
  return MMCStatus;
 }
 IOSET0 = SPI_SEL; /* set SPI SSEL */
 SPI_ReceiveByte();
 return 0;
}


/************************** MMC Write Block ***************************/
/* write a block of data based on the length that has been set
* in the SET_BLOCKLEN command.
* Send the WRITE_SINGLE_BLOCK command out first, check the
* R1 response, then send the data start token(bit 0 to 0) followed by
* the block of data. The test program sets the block length to 512
* bytes. When the data write finishs, the response should come back
* as 0xX5 bit 3 to 0 as 0101B, then another non-zero value indicating
* that MMC card is in idle state again.
*
*/
int mmc_write_block(WORD block_number)
{
 DWORD var;
 BYTE Status;
 IOCLR0 = SPI_SEL; /* clear SPI SSEL */

 var=(block_number)<<9;		/* Aligning block address with linear address */
  
 MMCCmd[0] = 0x58;						/* send mmc CMD24(WRITE_SINGLE_BLOCK) to write the data to MMC card */
 MMCCmd[1] = (var)>>24;				/* high block address bits, varh HIGH and LOW */
 MMCCmd[2] = (var)>>16;
 MMCCmd[3] = (var)>>8;				/* low block address bits, varl HIGH and LOW */
 MMCCmd[4] = var;
 MMCCmd[5] = 0xFF;
 						/* checksum is no longer required but we always send 0xFF */
 SPI_Send(MMCCmd, MMC_CMD_SIZE );
 if((mmc_response(0x00))==1)			/* if mmc_response returns 1 then we failed to get a 0x00 response */
 {
  MMCStatus = WRITE_BLOCK_TIMEOUT;
  IOSET0 = SPI_SEL; /* set SPI SSEL */
  return MMCStatus;
 } 
 
 MMCCmd[0] = 0xFE;						/* Set bit 0 to 0 which indicates the beginning of the data block */
 SPI_Send( MMCCmd, 1 );
 SPI_Send( MMCWRData, MMC_DATA_SIZE );  /*send data*/
 /* Send dummy checksum */
 /* when the last check sum is sent, the response should come back
 immediately. So, check the SPI FIFO MISO and make sure the status
 return 0xX5, the bit 3 through 0 should be 0x05 */
 MMCCmd[0] = 0xFF;
 MMCCmd[1] = 0xFF;
 SPI_Send( MMCCmd, 2 );
 Status = SPI_ReceiveByte();
 if ( (Status & 0x0F) != 0x05 )
 {
  MMCStatus = WRITE_BLOCK_FAIL;
  IOSET0 = SPI_SEL; /* set SPI SSEL */
  return MMCStatus;
 } 
 /* if the status is already zero, the write hasn't finished
 yet and card is busy */
 if(mmc_wait_for_write_finish()==1)
 {
  MMCStatus = WRITE_BLOCK_FAIL;
  IOSET0 = SPI_SEL; /* set SPI SSEL */
  return MMCStatus;
 } 
 IOSET0 = SPI_SEL; /* set SPI SSEL */
 SPI_ReceiveByte();
 return 0;
}


/************************** MMC Read Block ****************************/
/*
* Reads a 512 Byte block from the MMC
* Send READ_SINGLE_BLOCK command first, wait for response come back
* 0x00 followed by 0xFE. The call SPI_Receive() to read the data
* block back followed by the checksum.
*
*/
int mmc_read_block(WORD block_number)
{
 WORD Checksum;
 DWORD var;
 IOCLR0 = SPI_SEL; /* clear SPI SSEL */
 var=(block_number)<<9;
 
 MMCCmd[0] = 0x51;						/* send MMC CMD17(READ_SINGLE_BLOCK) to read the data from MMC card */
 MMCCmd[1] = (var)>>24;				/* high block address bits, varh HIGH and LOW */
 MMCCmd[2] = (var)>>6;
 MMCCmd[3] = (var)>>8;				/* low block address bits, varl HIGH and LOW */
 MMCCmd[4] = var;
 MMCCmd[5] = 0xFF;						/* checksum is no longer required but we always send 0xFF */
 SPI_Send(MMCCmd, MMC_CMD_SIZE );
 
 if((mmc_response(0x00))==1)			/* if mmc_response returns 1 then we failed to get a 0x00 response */
 {
  MMCStatus = READ_BLOCK_TIMEOUT;
  IOSET0 = SPI_SEL; /* set SPI SSEL */
  return MMCStatus;
 }

 if((mmc_response(0xFE))==1)			/* wait for data token */
 {
 MMCStatus = READ_BLOCK_DATA_TOKEN_MISSING;
 IOSET0 = SPI_SEL;
 return MMCStatus;
 }
 
 SPI_Receive( MMCRDData, MMC_DATA_SIZE );/* Get the block of data based on the length */
 /* CRC bytes that are not needed */
 Checksum = SPI_ReceiveByte();
 Checksum = Checksum << 0x08 | SPI_ReceiveByte();
 IOSET0 = SPI_SEL; /* set SPI SSEL */
 SPI_ReceiveByte();
 return 0;
}


/***************** MMC get response *******************/
/*
* Repeatedly reads the MMC until we get the
* response we want or timeout
*/
int mmc_response(BYTE response)
{
 DWORD count = 0xFFF;
 BYTE result;
 while( count > 0 )
 {
  result = SPI_ReceiveByte();
  if ( result == response )
  {
   break;
  }
  count--;
 }
 if ( count == 0 )
 return 1; /* Failure, loop was exited due to timeout */
 else
 return 0; /* Normal, loop was exited before timeout */
}

/***************** MMC wait for write finish *******************/
/*
* Repeatedly reads the MMC until we get a non-zero value (after
* a zero value) indicating the write has finished and card is no
* longer busy.
*
*/
int mmc_wait_for_write_finish( void )
{
 DWORD count = 0xFFFF; /* The delay is set to maximum considering
 the longest data block length to handle */
 BYTE result = 0;
 while( (result == 0) && count )
 {
 result = SPI_ReceiveByte();
 count--;
 }
 if ( count == 0 )
 return 1; /* Failure, loop was exited due to timeout */
 else
 return 0; /* Normal, loop was exited before timeout */
}


