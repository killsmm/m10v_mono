/**
 * @file		cmddef.h
 * @brief		Command define
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

/* Uart Communication Commands & Reply */
#define CMD_FLASH_WRITE  0x08
#define CMD_FLASH_CHECK  0x30
#define CMD_FLASH_ERASE  0x10
#define CMD_SECTOR_ERASE 0x11
#define CMD_FLASH_AUTO   0x12
#define CMD_BLANK_CHECK  0x20
#define DOWNLOAD_RAMCODE 0x14
#define FLASH_WRITE_MODE 0x15
#define CMD_RAM_WRITE    0x00

#define REPLY_RW_CHK_W   0x02
#define REPLY_CMD_W      0x03
#define REPLY_W_COMPLETE  0x04
/* Flash Control Command */
/*
#define WriteFlashW      0
#define ReadResetFlashW  4
#define ChipEraseFlash   8
*/

/* Flash Memory Start Address */
#define flash_address    0x00000000
/*#define flash_address    0x00040000*/


#define SEND_DDR 	0x0010  // Send DDR parameters to M3MMA
#define SEND_RAM 	0x0011  // Send "ramcode.bin" to M3MMA for code switching

#define SEND_NOR 	0x0012  // Send NOR flash paramters to M3MMA
#define SEND_NAND 	0x0013  // Send NAND flash parameters to M3MMA
#define SEND_DATA 	0x0014  // Send program code to M3MMA
#define SEND_KEY	0x0015	// Send Security Key
#define POLL_STATUS 0x0016  // Poll Programming Status
#define AUTO 		0x0020  // Start flash write in M3MMA
#define ASK_STATUS 	0x0030  // Ask M3MMA for programming status

#define ERASE		0x0040  // Erase NAND or NOR flash
#define BLANK_CHECK	0x0041  // Blank check NAND or NOR flash
#define WRITE		0x0042  // Write "data.bin" to NOR or NAND flash
#define VERIFY		0x0043  // Verify "data.bin" in NOR or NAND flash

