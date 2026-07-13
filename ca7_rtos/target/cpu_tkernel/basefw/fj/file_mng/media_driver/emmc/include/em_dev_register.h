// ************************************************************* //
//                                                               //
//        Copyright (C) 2015 by Socionext Inc.                   //
//        All rights reserved.                                   //
//        Socionext Inc. Proprietary and Confidential.           //
//                                                               //
// ************************************************************* //
/*****************************************************************************/
/**
* @file em_dev_register.h
* @brief related information of device registers
* @note None
* @attention None
*
*/
/*****************************************************************************/

#ifndef EM_DEV_REGISTER_H_INCLUDE
#define EM_DEV_REGISTER_H_INCLUDE


// include -------------------------------------

/// Extends CSD Register size
#define EXTCSD_SIZE							512

// Response Tab size
#define EM_RESP_TAB_SIZE					4

//---------------------------------------------------------------------------
/// @name CSD registers
//---------------------------------------------------------------------------
//@{
/// CSD register : Device size
#define EM_GET_CSD_REG_C_SIZE( ui_buf ) ( ( ui_buf[2] & 3 ) << 10  ) | ( ui_buf[1] >> 22 )
/// CSD register : Device size multiplier
#define EM_GET_CSD_REG_C_SIZE_MULT( ui_buf ) ( ( ui_buf[1] >> 7 ) & 0x7 )
/// CSD register : Max. read data block length
#define EM_GET_CSD_REG_READ_BL_LEN( ui_buf ) ( ( ui_buf[2] >> 8 ) & 0xF )
/// CSD register : System specification version
#define EM_GET_CSD_REG_SPEC_VERS( ui_buf ) ( ( ui_buf[3] >> 18 ) & 0xF )
/// CSD register : Max. bus clock frequency
#define EM_GET_CSD_REG_TRAN_SPEED( ui_buf ) ( ( ui_buf[2] >> 24  ) & 0xFF )
/// CSD register : Max. bus clock frequency(Frequency unit)
#define EM_GET_CSD_REG_FREQ_UNIT( tran_speed ) ( (tran_speed) & 0x07 )
/// CSD register : Max. bus clock frequency(Multiplier factor)
#define EM_GET_CSD_REG_MULTI_FACT( tran_speed ) ( ((tran_speed) & 0x78) >> 3 )
//@}

//---------------------------------------------------------------------------
/// @name Device status 
//---------------------------------------------------------------------------
//@{
/// The card will expect ACMD, or an indication that the command has been interpreted as ACMD
#define CARD_SATUS_APP_CMD					(0x1uL << 5)
/// Card didn't switch to the expected mode as requested by the SWITCH command
#define DEVICE_STATUS_SWITCH_ERROR			(0x1uL << 7)
/// Corresponds to buffer empty signaling on the bus - buffer is ready
#define CARD_SATUS_READY_FOR_DATA			(0x1uL << 8)
/// The state of the card when receiving the command. Below are definded all 9 satuses.
#define CARD_SATUS_CURRENT_STATUS_MASK		(0xFuL << 9)
/// Current status card is in Idle State
#define DEVICE_STATUS_CS_IDLE				(0x0uL << 9)
/// Current status card is in Ready State
#define DEVICE_STATUS_CS_READY				(0x1uL << 9)
/// Current status card is Identification State
#define DEVICE_STATUS_CS_IDENT				(0x2uL << 9)
/// Current status card is in Stand-by State
#define DEVICE_STATUS_CS_STBY				(0x3uL << 9)
/// Current status card is in Transfer State
#define DEVICE_STATUS_CS_TRAN				(0x4uL << 9)
/// Current status card is in Sending-data State
#define DEVICE_STATUS_CS_DATA				(0x5uL << 9)
/// Current status card is in Receive-data State
#define DEVICE_STATUS_CS_RCV				(0x6uL << 9)
/// Current status card is in Programming State
#define DEVICE_STATUS_CS_PRG				(0x7uL << 9)
/// Current status card is in Disconnect State
#define DEVICE_STATUS_CS_DIS				(0x8uL << 9)
/// An erase sequence was cleared before executing because an out of erase sequence command was received
#define DEVICE_STATUS_ERASE_RESET			(0x1uL << 13)
/// The command has been executed without using the internal ECC.
#define DEVICE_STATUS_CARD_ECC_DISABLED		(0x1uL << 14)
/// Problem with erase part of memory because it is protected
#define DEVICE_STATUS_WP_ERASE_SKIP			(0x1uL << 15)
/// @brief Can be either one of the following errors:
/// - The read only section of the CSD does not match the card content.
/// - An attempt to reverse the copy (set as original) or permanent WP (unprotected) bits was made.
#define DEVICE_STATUS_CSD_OVERWRITE			(0x1uL << 16)
/// written and cannot be overwritten
#define DEVICE_STATUS_CIDCSD_OVERWRITE		(0x1uL << 18)
/// A general or an unknown error occurred during the operation.
#define DEVICE_STATUS_ERROR					(0x1uL << 19)
/// Internal card controller error
#define DEVICE_STATUS_CC_ERROR				(0x1uL << 20)
/// Card internal ECC was applied but failure failed to correct the data.
#define DEVICE_STATUS_DEVICE_ECC_FAILED		(0x1uL << 21)
/// Command not legal for the card state
#define DEVICE_STATUS_ILLEGAL_COMMAND		(0x1uL << 22)
/// The CRC check of the previous error command failed.
#define DEVICE_STATUS_COM_CRC_ERROR			(0x1uL << 23)
/// Set when a sequence or password error has been detected in lock/unlock card command.
#define DEVICE_STATUS_LOCK_UNLOCK_FAILED	(0x1uL << 24)
/// When set, signals that the card is card locked by the host
#define DEVICE_STATUS_DEVICE_IS_LOCKED		(0x1uL << 25)
/// Set when the host attempts to write to a protected block or to the temporary or permanent write protected card.
#define DEVICE_STATUS_WP_VIOLATION			(0x1uL << 26)
/// An invalid selection of write-blocks for erase occurred.
#define DEVICE_STATUS_ERASE_PARAM			(0x1uL << 27)
/// An error in the sequence of erase error commands occurred.
#define DEVICE_STATUS_ERASE_SEQ_ERROR		(0x1uL << 28)
/// @brief The transferred block length is not allowed for this card, or the number
/// of transferred bytes does not match the block length.
#define DEVICE_STATUS_BLOCK_LEN_ERROR		(0x1uL << 29)
/// A misaligned address which did not match the block length was used in the command.
#define DEVICE_STATUS_ADDRESS_MISALIGN		(0x1uL << 30)
/// The command's argument was out of the allowed range for this card.
#define DEVICE_STATUS_OUT_OF_RANGE			(0x1uL << 31)

/// All errors mask definition
#define DEVICE_STATUS_ALL_ERRORS_MASK (	  DEVICE_STATUS_OUT_OF_RANGE		\
									  | DEVICE_STATUS_ADDRESS_MISALIGN	  \
									  | DEVICE_STATUS_BLOCK_LEN_ERROR	  \
									  | DEVICE_STATUS_ERASE_SEQ_ERROR	  \
									  | DEVICE_STATUS_ERASE_PARAM		  \
									  | DEVICE_STATUS_WP_VIOLATION		  \
									  | DEVICE_STATUS_LOCK_UNLOCK_FAILED  \
									  | DEVICE_STATUS_COM_CRC_ERROR		  \
									  | DEVICE_STATUS_ILLEGAL_COMMAND	  \
									  | DEVICE_STATUS_DEVICE_ECC_FAILED	  \
									  | DEVICE_STATUS_CC_ERROR			  \
									  | DEVICE_STATUS_ERROR				  \
									  | DEVICE_STATUS_CIDCSD_OVERWRITE	  \
									  | DEVICE_STATUS_WP_ERASE_SKIP		  \
									  | DEVICE_STATUS_SWITCH_ERROR )
//@}
//---------------------------------------------------------------------------
/// @name R5 response 
//---------------------------------------------------------------------------
// Nothing Special

//---------------------------------------------------------------------------
/// @name R6 Response 
//---------------------------------------------------------------------------
// Nothing Special

//---------------------------------------------------------------------------
/// @name OCR register bits defnitions of SD memory cards
//---------------------------------------------------------------------------
//{@
/// 1.70 - 1.95V
#define EM_REG_OCR_1_7_1_95 (1uL << 7)
/// 2.70 - 2.80V
#define EM_REG_OCR_2_7_2_8	(1uL << 15)
/// 2.80 - 2.90V
#define EM_REG_OCR_2_8_2_9	(1uL << 16)
/// 2.90 - 3.00V
#define EM_REG_OCR_2_9_3_0	(1uL << 17)
/// 3.00 - 3.10V
#define EM_REG_OCR_3_0_3_1	(1uL << 18)
/// 3.10 - 3.20V
#define EM_REG_OCR_3_1_3_2	(1uL << 19)
/// 3.20 - 3.30V
#define EM_REG_OCR_3_2_3_3	(1uL << 20)
/// 3.30 - 3.40V
#define EM_REG_OCR_3_3_3_4	(1uL << 21)
/// 3.40 - 3.50V
#define EM_REG_OCR_3_4_3_5	(1uL << 22)
/// 3.50 - 3.60V
#define EM_REG_OCR_3_5_3_6	(1uL << 23)
/// Access Mode
#define EM_REG_OCR_ACSMODE	(3uL << 29)
/// card capacity status (this bit is not aviable in the SDIO cards)
#define EM_REG_OCR_CCS		(1uL << 30)
/// card power up busy status (this is not aviable in the SDIO cards)
#define EM_REG_OCR_READY	(1uL << 31)
/// MMC card access mode - sector mode
#define EM_REG_OCR_SECTOR_MODE (0x2uL << 29 )
/// MMC card access mode - byte mode
#define EM_REG_OCR_BYTE_MODE   (0x0uL << 29 )

//@}
//---------------------------------------------------------------------------
/// @name Extended CSD byte index
//---------------------------------------------------------------------------
//@{
/// Device type
#define EM_EXT_CSD_DEVICE_TYPE			196
/// High speed interface timing
#define EM_EXT_CSD_HS_TIMING			185
/// Bus width mode
#define EM_EXT_CSD_BUS_WIDTH			183
/// Partition configuration
#define EM_EXT_CSD_BOOT_PART_CONFIG		179
//@}

//-----------------------------------------------------------------------------
/// @name Extended CSD register/PARTITION_CONFIG (before BOOT_CONFIG) [179]
//-----------------------------------------------------------------------------
//@{
/// select partitions no access to boot partition (default)
#define EM_EXT_CSD_BOOT_PART_CONFIG_ACCESS_NONE	   (0 << 0)
/// select partitions to access to R/W boot partition 1
#define EM_EXT_CSD_BOOT_PART_CONFIG_ACCESS_BOOT_1  (1 << 0)
/// select partitions to access to R/W boot partition 2
#define EM_EXT_CSD_BOOT_PART_CONFIG_ACCESS_BOOT_2  (2 << 0)
///R/W Replay Protected Memory Block (RPMB)
#define EM_EXT_CSD_BOOT_PART_CONFIG_ACCESS_RPMB	   (3 << 0)
/// select partitions to access to to General Purpose partition 1
#define EM_EXT_CSD_BOOT_PART_CONFIG_ACCESS_GEN_1   (4 << 0)
/// select partitions to access to to General Purpose partition 2
#define EM_EXT_CSD_BOOT_PART_CONFIG_ACCESS_GEN_2   (5 << 0)
/// select partitions to access to to General Purpose partition 3
#define EM_EXT_CSD_BOOT_PART_CONFIG_ACCESS_GEN_3   (6 << 0)
/// select partitions to access to to General Purpose partition 4
#define EM_EXT_CSD_BOOT_PART_CONFIG_ACCESS_GEN_4   (7 << 0)
/// select partitions to access mask
#define EM_EXT_CSD_BOOT_PART_CONFIG_ACCESS_MASK	   (7 << 0)

/// Device not boot enabled (default)
#define EM_EXT_CSD_BOOT_PART_CONFIG_BOOT_DISABLE   (0 << 3)
/// Boot partition 1 enabled for boot
#define EM_EXT_CSD_BOOT_PART_CONFIG_BOOT_EN_BOOT_1 (1 << 3)
/// Boot partition 2 enabled for boot
#define EM_EXT_CSD_BOOT_PART_CONFIG_BOOT_EN_BOOT_2 (2 << 3)
/// User area enabled for boot
#define EM_EXT_CSD_BOOT_PART_CONFIG_BOOT_EN_USR	   (7 << 3)
/// boot partition selection mask
#define EM_EXT_CSD_BOOT_PART_CONFIG_BOOT_EN_MASK   (7 << 3)

///Boot acknowledge sent during boot operation Bit
#define EM_EXT_CSD_BOOT_PART_CONFIG_BOOT_ACK	   (1 << 6)
//@}

//---------------------------------------------------------------------------
/// @name Extended CSD register/BUS_WIDTH [183]
//---------------------------------------------------------------------------
//@{
/// dual data rate mask
#define EM_SWITCH_BUS_WIDTH_DDR_MASK	4
/// 1 bit data bus
#define EM_SWITCH_BUS_WIDTH_1		0
/// 4 bit data bus
#define EM_SWITCH_BUS_WIDTH_4		1
/// 8 bit data bus
#define EM_SWITCH_BUS_WIDTH_8		2
/// 4 bit data bus (dual data rate)
#define EM_SWITCH_BUS_WIDTH_4_DDR	(EM_SWITCH_BUS_WIDTH_4 \
	| EM_SWITCH_BUS_WIDTH_DDR_MASK)
/// 8 bit data bus (dual data rate)
#define EM_SWITCH_BUS_WIDTH_8_DDR	(EM_SWITCH_BUS_WIDTH_8 \
	| EM_SWITCH_BUS_WIDTH_DDR_MASK)
//@}

//---------------------------------------------------------------------------
/// @name Extended CSD register/HS_TIMING [185]
//---------------------------------------------------------------------------
//@{
/// Standard speed
#define EM_HIGH_SPEED_DISABLE		0
/// High speed enabled
#define EM_SWITCH_HIGH_SPEED		1
/// High speed 200
#define EM_SWITCH_HIGH_SPEED_200	2
/// High speed 400
#define EM_SWITCH_HIGH_SPEED_400	3
//@}

//---------------------------------------------------------------------------
/// @name Extended CSD register/DEVICE_TYPE [196]
//---------------------------------------------------------------------------
//@{
/// High-Speed at 26MHz
#define EM_EXT_CSD_DEVICE_TYPE_HS_26MHZ			(1 << 0)
/// High-Speed at 52MHz
#define EM_EXT_CSD_DEVICE_TYPE_HS_52MHZ			(1 << 1)
/// High-speed DDR at 52 MHz, 1.8V or 3V I/O
#define EM_EXT_CSD_DEVICE_TYPE_HS_DDR_52MHZ_1_8_V_3_3_V	   (1 << 2)
/// High-speed DDR at 52 MHz, 1.2V I/O
#define EM_EXT_CSD_DEVICE_TYPE_HS_DDR_52MHZ_1_2_V		(1 << 3)
/// High-speed HS200 SDR at 200 MHz, 1.8V I/O
#define EM_EXT_CSD_DEVICE_TYPE_HS_SDR_200MHZ_1_8_V		(1 << 4)
/// High-speed HS200 SDR at 200 MHz, 1.2V I/O
#define EM_EXT_CSD_DEVICE_TYPE_HS_SDR_200MHZ_1_2_V		(1 << 5)
/// High-speed HS400 DDR at 200 MHz, 1.8V I/O
#define EM_EXT_CSD_DEVICE_TYPE_HS_DDR_200MHZ_1_8_V		(1 << 6)
/// High-speed HS400 DDR at 200 MHz, 1.2V I/O
#define EM_EXT_CSD_DEVICE_TYPE_HS_DDR_200MHZ_1_2_V		(1 << 7)
//@}

//---------------------------------------------------------------------------
/// @name CMD6 command Argument
//---------------------------------------------------------------------------
//@{
/// CMD6 command argument : Access field
#define EM_CMD6_ARG_VALUE(VAL) ((unsigned long)(VAL) << 8)
/// CMD6 command argument : Index field
#define EM_CMD6_ARG_INDEX(VAL) ((unsigned long)(VAL) << 16)
//@}

/**
 * @brief 4byte->1byte conversion
 * @details Conversion to byte from dword
 */
#define GetByte(dword, byte_nr)		(((dword) >> ((byte_nr) * 8)) & 0xFF)
/**
 * @brief 4byte(array)->1byte conversion
 * @details Conversion to byte from array
 */
#define GET_BYTE_FROM_BUFFER(buffer, byte_nr) GetByte((*(unsigned long*)((unsigned long)(buffer) + ((byte_nr) & ~3))), ((byte_nr) & 3))


#endif

