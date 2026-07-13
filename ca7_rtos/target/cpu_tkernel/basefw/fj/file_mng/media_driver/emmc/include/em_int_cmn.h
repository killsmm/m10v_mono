// ************************************************************* //
//                                                               //
//        Copyright (C) 2015 by Socionext Inc.                   //
//        All rights reserved.                                   //
//        Socionext Inc. Proprietary and Confidential.           //
//                                                               //
// ************************************************************* //
/*****************************************************************************/
/**
 * @file em_int_cmn.h
 * @brief eMMC internel common definition
 * @note None
 * @attention None
 *
 */
/*****************************************************************************/

#ifndef EM_INT_COM_H
#define EM_INT_COM_H

// ===============================================
//   Compile Option
// ===============================================
#define CO_EM_DEBUG_PRINT_ON
#define CO_EM_ERR_PRINT_ON

// include -------------------------------------

/// NULL
#ifndef NULL
#define NULL ((void*)0)
#endif

/// wait usec
#define EM_WAIT_USEC( Value ) Media_Custom_Wait_ns( (Value) * 1000 )

/// Debug Print
#ifdef CO_EM_DEBUG_PRINT_ON
#define	M_EM_DEBUG_PRINT(arg)	(Media_Custom_Info_Printer arg)
#else	// CO_SD1_DEBUG_PRINT_ON
#define M_EM_DEBUG_PRINT(arg)
#endif	// CO_SD1_DEBUG_PRINT_ON

#ifdef CO_EM_ERR_PRINT_ON
#define M_EM_ERR_PRINT(arg)		(Media_Custom_Err_Printer arg)
#else	// CO_SD1_ERR_PRINT_ON
#define M_EM_ERR_PRINT(arg)
#endif	// CO_SD1_ERR_PRINT_ON

//---------------------------------------------------------------------------
/// @name Enable/Disable
//---------------------------------------------------------------------------
//@{
/// Disable
#define EM_DISABLE				0
/// Enable
#define EM_ENABLE				1
//@}

//---------------------------------------------------------------------------
/// @name OCR Register Accessmode
//---------------------------------------------------------------------------
//@{
/// byte mode
#define EM_CAPACITY_NORMAL		1
/// sector mode
#define EM_CAPACITY_HIGH		2
//@}

//---------------------------------------------------------------------------
/// @name Training result
//---------------------------------------------------------------------------
//@{
/// unknown
#define EM_TRAINING_UNKNOWN		0
/// pass
#define EM_TRAINING_PASS		1
/// fail
#define EM_TRAINING_FAIL		2
//@}


/// MHz->KHZ
#define EM_MULTIPLE_MHZ_TO_KHZ			1000
/// SRS11.DTCV(ms)
#define EM_DATA_TIMEOUT_COUNTER_VALUE	2000
/// SRS11.DTCV TimeOut Interval(2^(12+2))
#define EM_DTCV_TIMEOUT_INTERVAL_START	16384

/// 4byte full mask bits
#define EM_ALL_F_4_BYTE					0xFFFFFFFFuL

/// int max value(Please use if want to intentionally 4byte the enumeration.)
#define EM_MAX_SIGNED_INT				0x7FFFFFFF

/// SDCLK Delay Value Maximum
#define EM_MAX_PHY_SDCLK_DELAY_HS200	0x27uL
#define EM_MAX_PHY_SDCLK_DELAY_LEGACY	0x7FuL

/// SDCLK Delay init Value
#define EM_INIT_VAL_PHY_SDCLK_DELAY_HS200	0x1FuL
#define EM_INIT_VAL_PHY_SDCLK_DELAY_LEGACY	0x00uL


//---------------------------------------------------------------------------
/// @name Event flg
//---------------------------------------------------------------------------
//@{
//! Transfer complete
#define D_EM_FLG_TC						0x00000001
//! Error interrupt
#define D_EM_FLG_ERR					0x00000002

//---------------------------------------------------------------------------
/// @name Ret value
//---------------------------------------------------------------------------
//@{
//! Valid
#define EM_OK							0x00u
//! Invalid
#define EM_INVAL						0x01u
//! SRS16/Timeout Clock Frequency is 0
#define EM_ERR_BASE_CLOCK_IS_ZERO		0x02u
//! Timeout
#define EM_ERR_TIMEOUT					0x03u
//! I/F error
#define EM_ERR_PARAM_ERR				0x04u
//! NULL pointer
#define EM_ERR_DEV_NULL_POINTER			0x05u
//! Invalid parameter
#define EM_ERR_INVALID_PARAMETER		0x06u
//! Unsupported operation
#define EM_ERR_UNSUPPORTED_OPERATION	0x07u
//! Unusable card
#define EM_ERR_UNUSABLE_CARD			0x08u
//! Undefined error
#define EM_ERR_UNDEFINED				0x09u
//! Response error
#define EM_ERR_RESP_ERROR				0x20u
//! ADMA error
#define EM_ERR_ADMA						0x21u
//! Error during configuration - current limit error
#define EM_ERR_CURRENT_LIMIT_ERROR		0x22u
//! Error during transfer data - end bit error
#define EM_ERR_DATA_END_BIT_ERROR		0x23u
//! Error during transfer data - crc bit error
#define EM_ERR_DATA_CRC_ERROR			0x24u
//! Error during transfer data - timeout error
#define EM_ERR_DATA_TIMEOUT_ERROR		0x25u
//! Error during execute command - command index error
#define EM_ERR_COMMAND_INDEX_ERROR		0x26u
//! Error during execute command - command line conflict
#define EM_ERR_CMD_LINE_CONFLICT		0x27u
//! Error during execute command - timeout error
#define EM_ERR_COMMAND_TIMEOUT_ERROR	0x28u
//! Error during execute command - CRC error
#define EM_ERR_COMMAND_CRC_ERROR		0x29u
//! Error during execute command - end bit error
#define EM_ERR_COMMAND_END_BIT_ERROR	0x2Au
//! Auto CMD error
#define EM_ERR_AUTO_CMD					0x2Bu
//! The command's argument was out of the allowed range for this card
#define EM_ERR_DEVICE_STATUS_ERROR		0x40u
//! SLOT is busy can't execute a command
#define EM_ERR_SLOT_IS_BUSY				0x80u
//! Error DAT line is busy, can't execute command
#define EM_ERR_DAT_LINE_BUSY			0x81u
//! Error CMD line is busy, can't execute command
#define EM_ERR_CMD_LINE_BUSY			0x82u
//! error function is not supported (CMD6 command)
#define EM_ERR_FUNCTION_UNSUPP			0x83u
//! ADMA few memory
#define EM_ERR_TO_FEW_MEM				0x84u
//! card not inserted
#define EM_ERR_CARD_IS_NOT_INSERTED		0x85u
//! card not attached
#define EM_ERR_CARD_IS_NOT_ATTACHED		0x86u
//! Extends CSD setting failed
#define EM_ERR_SETTING_EXT_CSD_FAILED	0x87u
//! HW problem
#define EM_ERR_HARDWARE_PROBLEM			0x88u
//! Indicated address is not aligned by 512byte
#define EM_ERR_ADDRESS_ALIGN_NG			0x89u
//! Format error
#define EM_ERR_FORMAT					0x90u
//! Command status pending
#define EM_STATUS_PENDING				0xFFu

//@}


/// DMA mode
typedef enum
{
	/// Standard DMA mode
	EM_DMA_SDMA_MODE = 0,
	/// Advanced DMA Mode Version 1
	EM_DMA_ADMA1_MODE = 1,
	/// Advanced DMA Mode Version 2
	EM_DMA_ADMA2_MODE = 2,
	/// DMA mode is selected automatically
	EM_DMA_AUTO_MODE = 3,
	/// DMA is disabled
	EM_DMA_NONEDMA_MODE = 255,
	// 4byte size indicated
	EM_DMA_MAXINT = EM_MAX_SIGNED_INT,
} EM_DMAMODE;

/// Bus width
typedef enum
{
	/// Bus width1
	EM_BUS_WIDTH_1 = 1,
	/// Bus width4
	EM_BUS_WIDTH_4 = 4,
	/// Bus width8
	EM_BUS_WIDTH_8 = 8,
	/// Bus width4(DDR)
	EM_BUS_WIDTH_4_DDR = 5,
	/// Bus width8(DDR)
	EM_BUS_WIDTH_8_DDR = 6,
} EM_BUS_WIDTH;

/// @brief Transfer direction
typedef enum
{
	/// Unspecified
	EM_TRANSFER_DEFAULT = 0,
	/// Write
	EM_TRANSFER_WRITE = 1,
	/// Read
	EM_TRANSFER_READ = 2,
} EM_TRANSFER_DIRECTION;

/// @brief Command Type
/// @note Set value to the SRS03-Command Type
typedef enum
{
	/// normal command
	EM_CMD_TYPE_NORMAL = 0,
	/// suspend command
	EM_CMD_TYPE_SUSPEND = 1,
	/// resume command
	EM_CMD_TYPE_RESUME = 2,
	/// abort command
	EM_CMD_TYPE_ABORT = 3,
} EM_COMMAND_TYPE;

/// Response Type
typedef enum
{
	// No Response
	EM_RESPONSE_NO_RESP = 0,
	// Response R1
	EM_RESPONSE_R1 = 1,
	// Response R1B
	EM_RESPONSE_R1B = 2,
	// Response R2
	EM_RESPONSE_R2 = 3,
	// Response R3
	EM_RESPONSE_R3 = 4,
} EM_RESPONSE_TYPE;

/// @brief Access mode
/// @note 0-5 is not described because for the SD Interface.
typedef enum
{
	/// eMMC legacy          (CLK: max 26MHz, DT: max 26MB/s) 
	EM_ACCESS_MODE_MMC_LEGACY = 6,
	/// eMMC high speed SDR  (CLK: max 52MHz, DT: max 52MB/s) 
	EM_ACCESS_MODE_HS_SDR = 7,
	/// eMMC high speed DDR  (CLK: max 52MHz, DT: max 104MB/s) 
	EM_ACCESS_MODE_HS_DDR = 8,
	/// eMMC HS200           (CLK: max 200MHz, DT: max 200MB/s) 
	EM_ACCESS_MODE_HS_200 = 9,
	/// eMMC HS400           (CLK: max 200MHz, DT: max 400MB/s) 
	EM_ACCESS_MODE_HS_400 = 10,
	/// Invalid
	EM_ACCESS_MODE_INVALID = 0xFFFF,
	// 4byte size indication
	EM_ACCESS_MODE_MAXINT = EM_MAX_SIGNED_INT,
} EM_ACCESS_MODE;

/// Access patition
typedef enum
{
	/// none
	EM_ACCCESS_BOOT_NONE = 0,
	/// boot partition 1 
	EM_ACCCESS_BOOT_1 = 1,
	/// boot partition 2 
	EM_ACCCESS_BOOT_2 = 2,
	/// general purpose partition 1 
	EM_ACCCESS_BOOT_GENERAL_PURP_1 = 3,
	/// general purpose partition 2 
	EM_ACCCESS_BOOT_GENERAL_PURP_2 = 4,
	/// general purpose partition 3 
	EM_ACCCESS_BOOT_GENERAL_PURP_3 = 5,
	/// general purpose partition 4 
	EM_ACCCESS_BOOT_GENERAL_PURP_4 = 6,
	/// Replay-protected memory-block partition 
	EM_ACCCESS_BOOT_RPMB = 7,
} EM_PARTITION_ACCESS;

#endif
