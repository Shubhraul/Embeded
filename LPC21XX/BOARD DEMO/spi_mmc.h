#ifndef __SPI_MMC_H__
#define __SPI_MMC_H__

#define SPI_SEL 0x00000080			/* SPI select pin */
#define MMC_CMD_SIZE 6				/* The SPI data is 8 bit long, the MMC use 48 bits, 6 bytes */

#define MMC_DATA_SIZE 512 			/* 512 bytes */
#define MAX_TIMEOUT 0xFF
#define IDLE_STATE_TIMEOUT 1
#define OP_COND_TIMEOUT 2
#define SET_BLOCKLEN_TIMEOUT 3
#define WRITE_BLOCK_TIMEOUT 4
#define WRITE_BLOCK_FAIL 5
#define READ_BLOCK_TIMEOUT 6
#define READ_BLOCK_DATA_TOKEN_MISSING 7
#define DATA_TOKEN_TIMEOUT 8
#define SELECT_CARD_TIMEOUT 9
#define SET_RELATIVE_ADDR_TIMEOUT 10
void SPI_Init( void );
void SPI_Send( BYTE *Buf, DWORD Length );
void SPI_Receive( BYTE *Buf, DWORD Length );
BYTE SPI_ReceiveByte( void );
int mmc_init(void);
int mmc_response(BYTE response);
int mmc_read_block(WORD block_number);
int mmc_write_block(WORD block_number);
int mmc_wait_for_write_finish(void);

#endif /* __SPI_MMC_H__ */

