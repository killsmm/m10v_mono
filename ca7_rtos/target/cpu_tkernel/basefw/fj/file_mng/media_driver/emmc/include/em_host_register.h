// ************************************************************* //
//                                                               //
//        Copyright (C) 2015 by Socionext Inc.                   //
//        All rights reserved.                                   //
//        Socionext Inc. Proprietary and Confidential.           //
//                                                               //
// ************************************************************* //
/*****************************************************************************/
/**
 * @file em_host_register.h
 * @brief Host registers related definition
 * @note None
 * @attention None
 *
 */
/*****************************************************************************/
#ifndef EM_HOST_REGISTER_H
#define EM_HOST_REGISTER_H

// include -------------------------------------
#include "em_config.h"

// define -------------------------------------
//! Register access(read)
#define EM_READ_REG( adr ) (adr)
//! Register access(write)
#define EM_WRITE_REG( adr, var ) (adr) = (var)


//! Special Function Registers -------------------------------------
//-----------------------------------------------------------------------------
/// @name General information register (HRS0) 
//-----------------------------------------------------------------------------
//@{
//! Get Slot Available on indicated bit
#define HRS0_ACCESABLE_SLOT(X)		(1uL << (16 + (X)))
//! Software Reset.
#define HRS0_SOFTWARE_RESET			(1uL << 0)
//@}

//-----------------------------------------------------------------------------
/// @name UHS-I PHY Setting (HRS4) 
//-----------------------------------------------------------------------------
//@{
/// UIS_ACK - UHS-I Setting Acknowredge
#define HRS4_UIS_SET_ACKNOWREDGE					(0x1 << 26)
/// UIS_RD - UHS-I Setting Read Request
#define HRS4_UIS_SET_READ_REQUEST					(0x1 << 25)
/// UIS_WR - UHS-I Setting Write Request
#define HRS4_UIS_SET_WRITE_REQUEST					(0x1 << 24)
/// UIS_RDATA - UHS-I Setting Read Data Mask
#define HRS4_UIS_SET_READ_DATA_MASK					(0xFF << 16)

/// UIS_ADDR value - eMMC Legacy input path delay
#define HRS4_UIS_ADDR_LEGACY_IN_DELAY				(0x06)
/// UIS_ADDR value - eMMC SDR input path delay
#define HRS4_UIS_ADDR_SDR_IN_DELAY					(0x07)
/// UIS_ADDR value - eMMC DDR input path delay
#define HRS4_UIS_ADDR_DDR_IN_DELAY					(0x08)
/// UIS_ADDR value - DLL Lock Value
#define HRS4_UIS_ADDR_DLL_LOCK_VALUE				(0x09)
/// UIS_ADDR value - DLL SDCLK Delay except HS200 and HS400
#define HRS4_UIS_ADDR_DLL_SDCLK_DELAY				(0x0B)
/// UIS_ADDR value - DLL SDCLK Delay
#define HRS4_UIS_ADDR_DLL_SDCLK_DELAY_HS			(0x0C)

//@}

//-----------------------------------------------------------------------------
/// @name eMMC control registers (HRS6) 
//-----------------------------------------------------------------------------
//@{
/// EMM - eMMC Mode select
#define HRS6_EMMC_MODE_MASK				(0x7 << 0)
/// SD Card in use
#define HRS6_EMMC_MODE_SDCARD			(0x0 << 0)
/// SDR
#define HRS6_EMMC_MODE_SDR				(0x2 << 0)
/// DDR
#define HRS6_EMMC_MODE_DDR				(0x3 << 0)
/// HS200
#define HRS6_EMMC_MODE_HS200			(0x4 << 0)
/// HS400
#define HRS6_EMMC_MODE_HS400			(0x5 << 0)
/// Legacy
#define HRS6_EMMC_MODE_LEGACY			(0x6 << 0)
/// ETV - eMMC Tune Value mask
#define HRS6_EMMC_TUNE_VALUE_MASK		(0x3FuL << 8)
/// ETV - eMMC Tune Value
#define HRS6_EMMC_TUNE_SET_VALUE(val)	((unsigned long)(val) << 8)
/// ETR - eMMC Tune Request
#define HRS6_EMMC_TUNE_REQUEST			(1uL << 15)
/// Writeable Bit
#define HRS6_WRITEABLE_BITS				(0x000003F3)
//@}

//-----------------------------------------------------------------------------
/// @name General information register (HRS41) 
//-----------------------------------------------------------------------------
//@{
//! RST_n Control.
#define HRS41_RSTNC						(1uL << 0)
//@}


//-----------------------------------------------------------------------------
/// @name Block Size / Block Count (SRS1) - masks
//-----------------------------------------------------------------------------
/// DMA buffer size 512kB
#define SRS1_DMA_BUFF_SIZE_512KB	0x00007000uL

//-----------------------------------------------------------------------------
/// @name Command/Transfer Mode (SRS3) - masks
//-----------------------------------------------------------------------------
//@{
/// DPS - Data Present Select
#define SRS3_DATA_PRESENT			(0x1uL << 21)
/// CICE - Command Index Check Enable
#define SRS3_INDEX_CHECK_EN			(0x1uL << 20)
/// CRCCE - Command CRC Check Enable
#define SRS3_CRC_CHECK_EN			(0x1uL << 19)
/// no response
#define SRS3_NO_RESPONSE			(0x0uL << 16)
/// 136-bit response
#define SRS3_RESP_LENGTH_136		(0x1uL << 16)
/// 48-bit response
#define SRS3_RESP_LENGTH_48			(0x2uL << 16)
/// 48-bit response with BUSY
#define SRS3_RESP_LENGTH_48B		(0x3uL << 16)
/// RID - Response Interrupt Disable
#define SRS3_RESP_INTER_DISABLE		(0x1uL << 8)
/// RECE - Response Error Check Enable.
#define SRS3_RESP_ERR_CHECK_EN		(0x1uL << 7)
/// RECT - Response Type R1/R5 : R1 (SD Memory)
#define SRS3_RESPONSE_CHECK_TYPE_R1 (0x0uL << 6)
/// MSBS - Multi/Single Block Select
#define SRS3_MULTI_BLOCK_SEL		(0x1uL << 5)
/// DTDS - Data Transfer Direction Select
#define SRS3_TRANS_DIRECT_READ		(0x1uL << 4)
/// Auto CMD12 enable
#define SRS3_AUTOCMD12_ENABLE       (0x1uL << 2)
/// BCE - Block Count Enable
#define SRS3_BLOCK_COUNT_ENABLE		(0x1uL << 1)
/// DMAE - DMA Enable
#define SRS3_DMA_ENABLE				(0x1uL << 0)
//@}

//-----------------------------------------------------------------------------
/// @name Present State Register (SFR9) 
//-----------------------------------------------------------------------------
//@{
/// Card state stable
#define SRS9_CARD_STATE_STABLE		(0x1uL << 17)
/// Card inserted
#define SRS9_CARD_INSERTED			(0x1uL << 16)
/// Command Inhibit (DAT)
#define SRS9_CMD_INHIBIT_DAT		(0x1uL << 1)
/// Command Inhibit (CMD)
#define SRS9_CMD_INHIBIT_CMD		(0x1uL << 0)
//@}


//-----------------------------------------------------------------------------
/// @name Host Control 1 (General / Power / Block-Gap / Wake-Up) (SRS10) 
//-----------------------------------------------------------------------------
//@{
/// BVS - SD Bus Voltage Select mask
#define SRS10_BUS_VOLTAGE_MASK				(0x7uL << 9)
/// 3.3V (typical)
#define SRS10_SET_3_3V_BUS_VOLTAGE			(0x7uL << 9)
/// 3.0V (typical)
#define SRS10_SET_3_0V_BUS_VOLTAGE			(0x6uL << 9)
/// 1.8V (typical) for embedded
#define SRS10_SET_1_8V_BUS_VOLTAGE			(0x5uL << 9)
/// BP - SD Bus Power for VDD1
#define SRS10_SD_BUS_POWER					(0x1uL << 8)
/// EDTW - Extended Data Transfer Width
#define SRS10_EXTENDED_DATA_TRANSFER_WIDTH	(0x1uL << 5)
/// DMASEL - DMA Select mask
#define SRS10_DMA_SELECT_MASK				(0x3uL << 3)
/// ADMA2 
#define SRS10_DMA_SELECT_ADMA2				(0x2uL << 3)
/// DTW - Data Transfer Width (SD mode only)
#define SRS10_DATA_WIDTH_4BIT				(0x1uL << 1)
/// Writeable Bit
#define SRS10_WRITEABLE_BITS				(0x0701FFFF)
//@}

//-----------------------------------------------------------------------------
/// @name Host Control 2 (Clock, Timeout, Reset) (SRS11) 
//-----------------------------------------------------------------------------
//@{
/// SRDAT - Software Reset For DAT Line (SD Mode only)
#define SRS11_SOFT_RESET_DAT_LINE			(0x1uL << 26)
/// SRCMD - Software Reset For CMD Line (SD Mode only)
#define SRS11_SOFT_RESET_CMD_LINE			(0x1uL << 25)
/// SRFA - Software Reset For All
#define SRS11_SOFT_RESET_FOR_ALL			(0x1uL << 24)
/// DTCV - Data Timeout Counter Value mask
#define SRS11_TIMEOUT_MASK					(0xFuL << 16)
/// SDCFSL - SDCLK Frequency Select (lower part)
#define SRS11_SDCLK_FREQ_LOW				(0xFFuL << 8)
/// SDCFSH - SDCLK Frequency Select (higher part)
#define SRS11_SDCLK_FREQ_HIGH				(0x3uL << 6)
/// SDCFSL - SDCLK Frequency Select mask
#define SRS11_SEL_FREQ_BASE_MASK			(0x000FFC0uL)
/// SDCE - SD Clock Enable
#define SRS11_SD_CLOCK_ENABLE				(0x1uL << 2)
/// ICS - Internal Clock Stable
#define SRS11_INT_CLOCK_STABLE				(0x1uL << 1)
/// ICE - Internal Clock Enable
#define SRS11_INT_CLOCK_ENABLE				(0x1uL << 0)
/// Writeable Bit
#define SRS11_WRITEABLE_BITS				(0x000FFFC7uL)
// SDCFS
#define SRS11_GET_SDCFS( srs11 ) (((srs11) & SRS11_SDCLK_FREQ_LOW) >> 8) | (((srs11) & SRS11_SDCLK_FREQ_HIGH) << 2)
//@}

//-----------------------------------------------------------------------------
/// @name Error/Normal Interrupt Status (SFR12)
//-----------------------------------------------------------------------------
//@{
/// ERSP - Response Error (SD Mode only)
#define SRS12_RESPONSE_ERROR		(0x1uL << 27)
/// EADMA - ADMA error
#define SRS12_ADMA_ERROR			(0x1uL << 25)
/// ECL - Current Limit Error
#define SRS12_CURRENT_LIMIT_ERROR	(0x1uL << 23)
/// Auto CMD (CMD12 or CMD23) error
#define SRS12_AUTO_CMD_ERROR        (0x1uL << 24)
/// EDEB - Data End Bit Error (SD mode only)
#define SRS12_DATA_END_BIT_ERROR	(0x1uL << 22)
/// EDCRC - Data CRC Error (SD mode only)
#define SRS12_DATA_CRC_ERROR		(0x1uL << 21)
/// EDT - Data Timeout Error (SD mode only)
#define SRS12_DATA_TIMEOUT_ERROR	(0x1uL << 20)
/// ECI - Command Index Error (SD mode only)
#define SRS12_COMMAND_INDEX_ERROR	(0x1uL << 19)
/// ECEB - Command End Bit Error (SD mode only)
#define SRS12_COMMAND_END_BIT_ERROR (0x1uL << 18)
/// ECCRC - Command CRC Error (SD mode only)
#define SRS12_COMMAND_CRC_ERROR		(0x1uL << 17)
/// ECT - Command Timeout Error (SD mode only)
#define SRS12_COMMAND_TIMEOUT_ERROR (0x1uL << 16)
/// EINT - Error Interrupt
#define SRS12_ERROR_INTERRUPT		(0x1uL << 15)
/// CINT - Card Interrupt
#define SRS12_CARD_INTERRUPT		(0x1uL << 8)
/// CR - Card Removal
#define SRS12_CARD_REMOVAL			(0x1uL << 7)
/// CIN - Card Insertion
#define SRS12_CARD_INSERTION		(0x1uL << 6)
/// BRR - Buffer Read Ready
#define SRS12_BUFFER_READ_READY		(0x1uL << 5)
/// BWR - Buffer Write Ready
#define SRS12_BUFFER_WRITE_READY	(0x1uL << 4)
/// TC - Transfer Complete
#define SRS12_TRANSFER_COMPLETE		(0x1uL << 1)
/// CC - Command Complete
#define SRS12_COMMAND_COMPLETE		(0x1uL << 0)
/// normal interrupt status mask
#define SRS12_NORMAL_STAUS_MASK		(0xFFFFuL << 0)
/// error interrupt status mask
#define SRS12_ERROR_STAUS_MASK		(0xFFFFuL << 16)
/// error interrupt status(DATA)
#define SRS12_ERROR_STAUS_DATA		(0x03F0uL << 16)
/// error interrupt status
#define SRS12_ERROR_STAUS_ALL		(0x03FFuL << 16)
//@}

//-----------------------------------------------------------------------------
/// @name Error/Normal Status Enable (SRS13)
//-----------------------------------------------------------------------------
//@{
/// ERSP_SE - Response Error Status Enable
#define SRS13_RESPONSE_ERROR_STAT_EN		(0x1uL << 27)
/// Tuning error status enable
#define SRS13_TUNING_ERROR_STAT_EN			(0x1uL << 26)
/// EADMA_SE - ADMA Error status enable
#define SRS13_ADMA_ERROR_STAT_EN			(0x1uL << 25)
/// EAC_SE - Auto CMD12 Error status enable (SD mode only)
#define SRS13_AUTO_CMD12_ERR_STAT_EN		(0x1uL << 24)
/// ECL_SE - Current Limit Error status enable
#define SRS13_CURRENT_LIMIT_ERR_STAT_EN		(0x1uL << 23)
/// EDEB_SE - Data End Bit Error status enable (SD mode only)
#define SRS13_DATA_END_BIT_ERR_STAT_EN		(0x1uL << 22)
/// EDCRC_SE - Data CRC Error status enable (SD mode only)
#define SRS13_DATA_CRC_ERR_STAT_EN			(0x1uL << 21)
/// EDT_SE - Data Timeout Error status enable (SD mode only)
#define SRS13_DATA_TIMEOUT_ERR_STAT_EN		(0x1uL << 20)
/// ECI_SE - Command Index Error status enable (SD mode only)
#define SRS13_COMMAND_INDEX_ERR_STAT_EN		(0x1uL << 19)
/// ECEB_SE - Command End Bit Error status enable (SD mode only)
#define SRS13_COMMAND_END_BIT_ERR_STAT_EN	(0x1uL << 18)
/// ECCRC_SE - Command CRC Error status enable (SD mode only)
#define SRS13_COMMAND_CRC_ERR_STAT_EN		(0x1uL << 17)
/// ECT_SE - Command Timeout Error status enable (SD mode only)
#define SRS13_COMMAND_TIMEOUT_ERR_STAT_EN	(0x1uL << 16)
/// Re-Tuning Event status enable
#define SRS13_RETUNING_EVENT_STAT_EN		(0x1uL << 12)
///Interrupt on line C status enable
#define SRS13_INTERRUPT_ON_LINE_C_STAT_EN	(0x1uL << 11)
/// Interrupt on line B status enable
#define SRS13_INTERRUPT_ON_LINE_B_STAT_EN	(0x1uL << 10)
/// Interrupt on line A status enable
#define SRS13_INTERRUPT_ON_LINE_A_STAT_EN	(0x1uL << 9)
/// CINT_SE - CINT_SE - Card Interrupt status enable
#define SRS13_CARD_INTERRUPT_STAT_EN		(0x1uL << 8)
/// CR_SE - Card Removal status enable
#define SRS13_CARD_REMOVAL_STAT_EN			(0x1uL << 7)
/// CIN_SE -Card Insertion status enable
#define SRS13_CARD_INSERTION_STAT_EN		(0x1uL << 6)
/// BRR_SE - Buffer Read Ready status enable
#define SRS13_BUFFER_READ_READY_STAT_EN		(0x1uL << 5)
/// BWR_SE - Buffer Write Ready status enable
#define SRS13_BUFFER_WRITE_READY_STAT_EN	(0x1uL << 4)
/// DMAINT_SE - DMA Interrupt status enable
#define SRS13_DMA_INTERRUPT_STAT_EN			(0x1uL << 3)
/// BGE_SE - Block Gap Event status enable
#define SRS13_BLOCK_GAP_EVENT_STAT_EN		(0x1uL << 2)
/// TC_SE - Transfer Complete status enable
#define SRS13_TRANSFER_COMPLETE_STAT_EN		(0x1uL << 1)
/// CC_SE - Command Complete status enable
#define SRS13_COMMAND_COMPLETE_STAT_EN		(0x1uL << 0)
/// Writeable Bit
#define SRS13_WRITEABLE_BITS				(0x0BFF01FF)
//@}


//-----------------------------------------------------------------------------
/// @name Error/Normal Signal Enable (SRS14) 
//-----------------------------------------------------------------------------
//@{
/// ERSP_IE - Response Error Interrupt Enable
#define SRS14_RESPONSE_ERROR_SIG_EN			(0x1uL << 27)
/// Tuning error signal enable
#define SRS14_TUNING_ERROR_SIG_EN			(0x1uL << 26)
/// EADMA_IE - ADMA Error interrupt enable
#define SRS14_ADMA_ERROR_SIG_EN				(0x1uL << 25)
/// EAC12_IE - Auto CMD12 Error interrupt enable (SD mode only)
#define SRS14_AUTO_CMD12_ERR_SIG_EN			(0x1uL << 24)
/// ECL_IE - Current Limit Error interrupt enable
#define SRS14_CURRENT_LIMIT_ERR_SIG_EN		(0x1uL << 23)
/// EDEB_IE - Data End Bit Error interrupt enable (SD mode only)
#define SRS14_DATA_END_BIT_ERR_SIG_EN		(0x1uL << 22)
/// EDCRC_IE - Data CRC Error interrupt enable (SD mode only)
#define SRS14_DATA_CRC_ERR_SIG_EN			(0x1uL << 21)
/// EDT_IE - ata Timeout Error interrupt enable (SD mode only)
#define SRS14_DATA_TIMEOUT_ERR_SIG_EN		(0x1uL << 20)
/// ECI_IE-Command Index Error interrupt enable (SD mode only)
#define SRS14_COMMAND_INDEX_ERR_SIG_EN		(0x1uL << 19)
/// ECEB_IE - Command End Bit Error interrupt enable (SD mode only)
#define SRS14_COMMAND_END_BIT_ERR_SIG_EN	(0x1uL << 18)
/// ECCRC_IE - Command CRC Error interrupt enable (SD mode only)
#define SRS14_COMMAND_CRC_ERR_SIG_EN		(0x1uL << 17)
/// ECT_IE - Command Timeout Error interrupt enable (SD mode only)
#define SRS14_COMMAND_TIMEOUT_ERR_SIG_EN	(0x1uL << 16)
/// Re-Tuning Event signal enable
#define SRS14_RETUNING_EVENT_SIG_EN			(0x1uL << 12)
///Interrupt on line C signal enable
#define SRS14_INTERRUPT_ON_LINE_C_SIG_EN	(0x1uL << 11)
/// Interrupt on line B signal enable
#define SRS14_INTERRUPT_ON_LINE_B_SIG_EN	(0x1uL << 10)
/// Interrupt on line A signal enable
#define SRS14_INTERRUPT_ON_LINE_A_SIG_EN	(0x1uL << 9)
/// CINT_IE - Card Interrupt - interrupt enable
#define SRS14_CARD_INTERRUPT_SIG_EN			(0x1uL << 8)
/// CR_IE - Card Removal interrupt enable
#define SRS14_CARD_REMOVAL_SIG_EN			(0x1uL << 7)
/// CIN_IE - Card Insertion interrupt enable
#define SRS14_CARD_INSERTION_SIG_EN			(0x1uL << 6)
/// BRR_IE - Buffer Read Ready interrupt enable
#define SRS14_BUFFER_READ_READY_SIG_EN		(0x1uL << 5)
/// BWR_IE - Buffer Write Ready interrupt enable
#define SRS14_BUFFER_WRITE_READY_SIG_EN		(0x1uL << 4)
/// DMAINT_IE - DMA Interrupt enable
#define SRS14_DMA_INTERRUPT_SIG_EN			(0x1uL << 3)
/// BGE_IE - Block Gap Event interrupt enable
#define SRS14_BLOCK_GAP_EVENT_SIG_EN		(0x1uL << 2)
/// TC_IE - Transfer Complete interrupt enable
#define SRS14_TRANSFER_COMPLETE_SIG_EN		(0x1uL << 1)
/// CC_IE - Command Complete interrupt enable
#define SRS14_COMMAND_COMPLETE_SIG_EN		(0x1uL << 0)
/// Writeable Bit
#define SRS14_WRITEABLE_BITS				(0x0BFF01FF)
//@}

//-----------------------------------------------------------------------------
/// @name AutoCMD12 Error Status Register/Host Control Register (SRS15)
//-----------------------------------------------------------------------------
//@{
/// CNIACE - Command not issued bu auto CMD12 error
#define SRS15_CMD_NOT_ISSUED_ERR        (0x1uL << 7)
/// ACIE - Auto CMD12 index error
#define SRS15_AUTO_CMD12_INDEX_ERR      (0x1uL << 4)
/// ACEBE - Auto CMD12 end bit error
#define SRS15_AUTO_CMD12_END_BIT_ERR    (0x1uL << 3)
/// ACCE - Auto CMD12 CRC error
#define SRS15_AUTO_CMD12_CRC_ERR        (0x1uL << 2)
/// ACTE - Auto CMD12 timeout error
#define SRS15_AUTO_CMD12_TIMEOUT_ERR    (0x1uL << 1)
/// ACNE - Autp CMD12 not executed
#define SRS15_AUTO_CMD12_NOT_EXECUTED   (0x1uL << 0)
//@}

//-----------------------------------------------------------------------------
/// @name Capabilities #1 (SRS16) 
//-----------------------------------------------------------------------------
//@{
/// VS18 - Voltage Support 1.8V
#define SRS16_VOLTAGE_1_8V_SUPPORT			(0x1uL << 26)
/// VS30 - Voltage Support 3.0V
#define SRS16_VOLTAGE_3_0V_SUPPORT			(0x1uL << 25)
/// VS33 - Voltage Support 3.3V
#define SRS16_VOLTAGE_3_3V_SUPPORT			(0x1uL << 24)
/// ADMA2S - ADMA2 Support
#define SRS16_ADMA2_SUPPORT					(0x1uL << 19)
/// BCSDCLK - Base Clock Frequency For SD Clock
#define SRS16_GET_BASE_CLK_FREQ_MHZ(VALUE)	((VALUE >> 8) & 0xFF)
/// TCU - Timeout Clock Unit
#define SRS16_TIMEOUT_CLOCK_UNIT_MHZ		(0x1uL << 7)
/// TCF - Timeout Clock Frequency
#define SRS16_GET_TIMEOUT_CLK_FREQ(VALUE)	(VALUE & 0x3F)
//@}

//-----------------------------------------------------------------------------
/// @name ADMA Error Status (SRS21)
//-----------------------------------------------------------------------------
//@{
/// EADMAL - ADMA Length Mismatch Error
#define SRS21_ADMA_ERROR_LENGTH_MISMATCH				(0x1uL << 2)
/// EADMAS - ADMA Error State
#define SRS21_ADMA_ERROR_STATE_TRF						(0x3uL << 0)
/// ST_STOP - ADMA Stopped
#define SRS21_ADMA_ERROR_STATE_MASK						(0x3uL << 0)
/// ST_STOP - ADMA Stopped
#define SRS21_ADMA_ERROR_STATE_STOP						(0x0uL << 0)
/// ST_FDS - Fetching descriptor
#define SRS21_ADMA_ERROR_STATE_FDS						(0x1uL << 0)
//@}

//-----------------------------------------------------------------------------
/// @name Host Controller Version/Slot Interrupt Status (CRS63) 
//-----------------------------------------------------------------------------
//@{
/// SVN - Specification Version Number
#define CRS63_GET_SPEC_VER_NUM( val )	  ((((val) >> 16) & 0xFF) + 1)

#define CRS63_HOST_SPEC_VER_1_00			(1)
#define CRS63_HOST_SPEC_VER_2_00			(2)
#define CRS63_HOST_SPEC_VER_3_00			(3)
//@}



// Structure  -------------------------------------

/**
 * @struct EM_HRS00
 * @brief General Information Register Fields
 */
typedef volatile union EM_HRS00 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long SWR:1;				 //!<  Software Reset
		unsigned long rsv1:15;				 //!< Reserved
		unsigned long SAV:8;				 //!<  Slot Available
		unsigned long rsv2:8;				 //!< Reserved
	} bit;
} EM_HRS00 ;

/**
 * @struct EM_HRS01
 * @brief Debounce Setting Register Fields
 */
typedef volatile union EM_HRS01 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long DP:24;				 //!<  Debounce Period
		unsigned long rsv:8;				 //!< Reserved
	} bit;
} EM_HRS01 ;

/**
 * @struct EM_HRS02
 * @brief Bus Setting Register Fields
 */
typedef volatile union EM_HRS02 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long PBL:4;				 //!<  Programmable Burst Length
		unsigned long rsv1:12;				 //!< Reserved
		unsigned long OTN:2;				 //!<  Number of Outstanding Transfers
		unsigned long rsv2:14;				 //!< Reserved
	} bit;
} EM_HRS02 ;

/**
 * @struct EM_HRS03
 * @brief AXI ERROR Responses Register
 */
typedef volatile union EM_HRS03 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long AER_RD:1;				 //!<  AXI ERROR Response R channel: DECERR
		unsigned long AER_RS:1;				 //!<  AXI ERROR Response R channel: SLVERR
		unsigned long AER_BD:1;				 //!<  AXI ERROR Response B channel: DECERR
		unsigned long AER_BS:1;				 //!<  AXI ERROR Response B channel: SLVERR
		unsigned long rsv1:4;				 //!< Reserved
		unsigned long AER_SERD:1;			 //!<  Status Enable for AXI ERROR Response R channel:
		unsigned long AER_SENRS:1;			 //!< Status Enable for AXI ERROR Response R channel:
		unsigned long AER_SENBD:1;			 //!<  Status Enable for AXI ERROR Response B chan-
		unsigned long AER_SENBS:1;			 //!< Status Enable for AXI ERROR Response B channel:
		unsigned long rsv2:4;				 //!< Reserved
		unsigned long AER_IERD:1;			 //!<  Signal Enable for AXI ERROR Response R channel:
		unsigned long AER_IERS:1;			 //!<  Signal Enable for AXI ERROR Response R channel:
		unsigned long AER_IEBD:1;			 //!<  Signal Enable for AXI ERROR Response B channel:
		unsigned long AER_IEBS:1;			 //!<  Signal Enable for AXI ERROR Response B channel:
		unsigned long rsv3:12;				 //!< Reserved
	} bit;
} EM_HRS03 ;

/**
 * @struct EM_HRS04
 * @brief UHS-I PHY Settings
 */
typedef volatile union EM_HRS04 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long UIS_ADDR:6;			 //!<  UHS-I Delay Address Pointer
		unsigned long rsv1:2;				 //!< Reserved
		unsigned long UIS_WDATA:8;			 //!<  UHS-I Settings Write Data
		unsigned long UIS_RDATA:8;			 //!<  UHS-I Settings Read Data
		unsigned long UIS_WR:1;				 //!<  UHS-I Settings Write Request
		unsigned long UIS_RD:1;				 //!<  UHS-I Settings Read Request
		unsigned long UIS_ACK:1;			 //!<  UHS-I Settings Acknowledge
		unsigned long rsv2:5;				 //!< Reserved
	} bit;
} EM_HRS04 ;

/**
 * @struct EM_HRS05
 * @brief UHS-I PHY BIST Control/Status
 */
typedef volatile union EM_HRS05 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long BSTE:1;				 //!<  UHS-I BIST Enable
		unsigned long BSTD:1;				 //!<  UHS-I BIST Done
		unsigned long BSTS:14;				 //!<  UHS-I BIST Status
		unsigned long rsv:16;				 //!< Reserved
	} bit;
} EM_HRS05 ;

/**
 * @struct EM_HRS06
 * @brief eMMC Control
 */
typedef volatile union EM_HRS06 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long EMM:3;				 //!<  eMMC Mode select
		unsigned long rsv1:5;				 //!< Reserved
		unsigned long ETV:6;				 //!<  eMMC Tune Value
		unsigned long rsv2:1;				 //!< Reserved
		unsigned long ETR:1;				 //!<  eMMC Tune Request
		unsigned long rsv3:16;				 //!< Reserved
	} bit;
} EM_HRS06 ;

/**
 * @struct EM_HRS32
 * @brief FSM Monitor Register
 */
typedef volatile union EM_HRS32 {
	unsigned long word;						 //!< 4byte word
	struct {

		unsigned long DATA:16;				 //!<  FSM status
		unsigned long ADDR:15;				 //!<  FSM address
		unsigned long LOAD:1;				 //!<  FSM monitor update request
	} bit;
} EM_HRS32 ;

/**
 * @struct EM_HRS33
 * @brief Tune Status 0 Register
 */
typedef volatile union EM_HRS33 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long STAT0:32;				 //!<  Tune status 0
	} bit;
} EM_HRS33 ;

/**
 * @struct EM_HRS34
 * @brief Tune Status 1 Register
 */
typedef volatile union EM_HRS34 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long STAT1:8;				 //!<  Tune status 1
		unsigned long rsv:24;				 //!< Reserved
	} bit;
} EM_HRS34 ;

/**
 * @struct EM_HRS35
 * @brief Tune Debug Register
 */
typedef volatile union EM_HRS35 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long TVAL:6;				 //!<  Tune Current Value
		unsigned long rsv1:10;				 //!< Reserved
		unsigned long TFV:6;				 //!<  Tune Force Value
		unsigned long rsv2:9;				 //!< Reserved
		unsigned long TFR:1;				 //!<  Tune Force Request
	} bit;
} EM_HRS35 ;

/**
 * @struct EM_HRS36
 * @brief Boot Status Register
 */
typedef volatile union EM_HRS36 {
	unsigned long word;						//!< 4byte word
	struct {
		unsigned long BOO_ACT:1;			//!<  Boot Active
		unsigned long BOO_EAT:1;			//!<  Boot Error(Acknowredge Timeout Error)
		unsigned long BOO_EAI:1;			//!<  Boot Error(Invalid Acknowredge Error)
		unsigned long BOO_EDT:1;			//!<  Boot Error(Data Timeout Error)
		unsigned long BOO_EDC:1;			//!<  Boot Error(Data CRC Error)
		unsigned long BOO_EDE:1;			//!<  Boot Error(End Bit Error)
		unsigned long rsv:26;				//!< Reserved
	} bit;
} EM_HRS36 ;

/**
 * @struct EM_HRS39
 * @brief AXI Master Output Control Register
 */
typedef volatile union EM_HRS39 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long AWCACHE:4;			 //!< AM_AWCACHE_O Signal
		unsigned long rsv1:4;				 //!< Reserved
		unsigned long AWPROT:3;				 //!< AM_AWCACHE_O Signal
		unsigned long rsv2:5;				 //!< Reserved
		unsigned long ARCACHE:4;			 //!< AM_ARCACHE_O Signal
		unsigned long rsv3:4;				 //!< Reserved
		unsigned long ARPROT:3;				 //!< AM_ARCACHE_O Signal
		unsigned long rsv4:5;				 //!< Reserved
	} bit;
} EM_HRS39 ;


/**
 * @struct EM_HRS40
 * @brief I/O Control Register
 */
typedef volatile union EM_HRS40 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long XDATOEC:8;			 //!< MMC_XDATOE_O Control
		unsigned long DATC:8;				 //!< MMC_DAT_O Control
		unsigned long XCMDOEC:1;			 //!< MMC_XCMDOE_O Control
		unsigned long CMDC:1;				 //!< MMC_CMD_O Control
		unsigned long MSELC:1;				 //!< MMC_MSEL_O Control
		unsigned long CRESC:1;				 //!< MMC_CRES_O Control
		unsigned long rsv:12;				 //!< Reserved
	} bit;
} EM_HRS40 ;

/**
 * @struct EM_HRS41
 * @brief MMC Control Register
 */
typedef volatile union EM_HRS41 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long RSTNC:1;				 //!< RST_n Control
		unsigned long VCCC:1;				 //!< VCC Control
		unsigned long VCCQC:1;				 //!< VCCQ Control
		unsigned long rsv:29;				 //!< Reserved
	} bit;
} EM_HRS41 ;

/**
 * @struct EM_REGISTER_HRS
 * @brief General Information Register
 */
typedef volatile struct EM_REGISTER_HRS {
	EM_HRS00 HRS0;							 //!< General Information Register Fields
	EM_HRS01 HRS1;							 //!< Debounce Setting Register Fields
	EM_HRS02 HRS2;							 //!< Bus Setting Register Fields
	EM_HRS03 HRS3;							 //!< AXI ERROR Responses Register
	EM_HRS04 HRS4;							 //!< UHS-I PHY Settings
	EM_HRS05 HRS5;							 //!< UHS-I PHY BIST Control/Status
	EM_HRS06 HRS6;							 //!< eMMC Control
	unsigned long reserved0[ 32 - 7 ];		 //!< reserved
	EM_HRS32 HRS32;							 //!< FSM Monitor Register
	EM_HRS33 HRS33;							 //!< Tune Status 0 Register
	EM_HRS34 HRS34;							 //!< Tune Status 1 Register
	EM_HRS35 HRS35;							 //!< Tune Debug Register
	EM_HRS36 HRS36;							 //!< Boot Status Register
	unsigned long reserved1[ 39 - 37 ];		 //!< reserved0
	EM_HRS39 HRS39;							 //!< AXI Master Output Control Register
	EM_HRS40 HRS40;							 //!< I/O Control Register
	EM_HRS41 HRS41;							 //!< MMC Control Register
	unsigned long reserved2[ 63 - 42 ];		 //!< reserved0
} EM_REGISTER_HRS;

/**
 * @struct EM_REGISTER_SRS4_7
 * @brief Slot Register Set 4-7
 */
typedef volatile struct EM_REGISTER_SRS4_7 {
	unsigned long SRS4;						 //!< Response Register#1
	unsigned long SRS5;						 //!< Response Register#2
	unsigned long SRS6;						 //!< Response Register#3
	unsigned long SRS7;						 //!< Response Register#4
} EM_REGISTER_SRS4_7 ;

/**
 * @union EM_REGISTER_SRS4_7
 * @brief Slot Register Set 4-7
 */
typedef volatile union EM_REGISTER_UNION_SRS4_7 {
	EM_REGISTER_SRS4_7	  dedicate;			  //!< Response Register#1-#4
	unsigned long Response[ 4 ];			  //!< Response Register#1-#4
} EM_REGISTER_UNION_SRS4_7 ;

/**
 * @struct EM_SRS0
 * @brief SDMA System Address / Argument 2
 */
typedef volatile union EM_SRS0 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long SAAR:32;				 //!< System Address / Argument 2
	} bit;
} EM_SRS0 ;

/**
 * @struct EM_SRS1
 * @brief Block Size / Block Count 
 */
typedef volatile union EM_SRS1 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long TBS:12;				 //!< Transfer Block Size
		unsigned long SDMABB:3;				 //!< SDMA Buffer Boundary
		unsigned long rsv:1;				 //!< reserved
		unsigned long BCCT:16;				 //!< Block Count For Current Transfer
	} bit;
} EM_SRS1 ;

/**
 * @struct EM_SRS2
 * @brief Argument 1
 */
typedef volatile union EM_SRS2 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long ARG1:32;				 //!< Command Argument 1
	} bit;
} EM_SRS2 ;

/**
 * @struct EM_SRS3
 * @brief Command/Transfer Mode 
 */
typedef volatile union EM_SRS3 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long DMAE:1;				 //!< DMA Enable
		unsigned long BCE:1;				 //!< Block Count Enable
		unsigned long ACE:2;				 //!< Auto CMD Enable
		unsigned long DTDS:1;				 //!< Data Transfer Direction Select
		unsigned long MSBS:1;				 //!< Multi/Single Block Select
		unsigned long RECT:1;				 //!< Response Type R1/R5
		unsigned long RECE:1;				 //!< Response Error Check Enable
		unsigned long RID:1;				 //!< Response Interrupt Disable
		unsigned long rsv1:7;				 //!< reserved
		unsigned long RTS:2;				 //!< Response Type Select
		unsigned long rsv2:1;				 //!< reserved
		unsigned long CRCCE:1;				 //!< Command CRC Check Enable
		unsigned long CICE:1;				 //!< Command Index Check Enable
		unsigned long DPS:1;				 //!< Data Present Select
		unsigned long CT:2;					 //!< Command Type
		unsigned long CI:6;					 //!< Command Index
		unsigned long rsv3:2;				 //!< reserved
	} bit;
} EM_SRS3 ;

/**
 * @struct EM_SRS8
 * @brief Data Buffer 
 */
typedef volatile union EM_SRS8 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long BDP:32;				  //!< 
	} bit;
} EM_SRS8 ;

/**
 * @struct EM_SRS9
 * @brief Present State Register
 */
typedef volatile union EM_SRS9 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long CICMD:1;				 //!<  Command Inhibit CMD
		unsigned long CIDAT:1;				 //!<  Command Inhibit DAT (SD Mode only)
		unsigned long DLA:1;				 //!<  DAT Line Active (SD Mode only)
		unsigned long rsv1:1;				 //!< Reserved
		unsigned long DATSL2:4;				 //!<  DAT[7:4] Line Signal Level (SD Mode only)
		unsigned long WTA:1;				 //!<  Write Transfer Active (SD Mode only)
		unsigned long RTA:1;				 //!<  Read Transfer Active (SD Mode only)
		unsigned long BWE:1;				 //!<  Buffer Write Enable
		unsigned long BRE:1;				 //!<  Buffer Read Enable
		unsigned long rsv2:4;				 //!< Reserved
		unsigned long CI:1;					 //!<  Card Inserted
		unsigned long CSS:1;				 //!<  Card State Stable
		unsigned long CDSL:1;				 //!<  Card Detect Pin Level
		unsigned long WPSL:1;				 //!<  Write Protect Switch Pin Level
		unsigned long DATSL1:4;				 //!<  DAT[3:0] Line Signal Level (SD Mode only)
		unsigned long CMDSL:1;				 //!<  CMD Line Signal Level (SD Mode only)
		unsigned long rsv3:7;				 //!< Reserved
	} bit;
} EM_SRS9 ;

/**
 * @struct EM_SRS10
 * @brief Host Control 1 
 */
typedef volatile union EM_SRS10 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long LEDC:1;				 //!<  LED Control
		unsigned long DTW:1;				 //!<  Data Transfer Width (SD mode only)
		unsigned long HSE:1;				 //!<  High Speed Enable (SD mode only)
		unsigned long DMASEL:2;				 //!<  DMA Select
		unsigned long EDTW:1;				 //!<  Extended Data Transfer Width
		unsigned long CDTL:1;				 //!<  Card Detect Test Level
		unsigned long CDSS:1;				 //!<  Card Detect Signal Selection
		unsigned long BP:1;					 //!<  SD Bus Power for VDD1
		unsigned long BVS:3;				 //!<  SD Bus Voltage Select
		unsigned long BP2:1;				 //!<  SD Bus Power for VDD2 (unused in SD mode)
		unsigned long BVS2:3;				 //!<  SD Bus Voltage Select for VDD2 (unused in SD mode)
		unsigned long SBGR:1;				 //!<  Stop At Block Gap Request
		unsigned long CR:1;					 //!<  Continue Request
		unsigned long rsv1:6;				 //!< Reserved
		unsigned long WOIQ:1;				 //!<  Wakeup Event Enable On Card Interrupt
		unsigned long WOIS:1;				 //!<  Wake-Up Event Enable On Card Inserted
		unsigned long WORM:1;				 //!<  Wakeup Event Enable On SD Card Removal
		unsigned long rsv2:5;				 //!< Reserved
	} bit;
} EM_SRS10 ;

/**
 * @struct EM_SRS11
 * @brief Host Control 2 (Clock, Timeout, Reset)
 */
typedef volatile union EM_SRS11 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long ICE:1;				 //!<  Internal Clock Enable
		unsigned long ICS:1;				 //!<  Internal Clock Stable
		unsigned long SDCE:1;				 //!<  SD Clock Enable
		unsigned long rsv1:3;				 //!< Reserved
		unsigned long SDCFSH:2;				 //!<  SDCLK Frequency Select (higher part)
		unsigned long SDCFSL:8;				 //!<  SDCLK Frequency Select (lower part)
		unsigned long DTCV:4;				 //!<  Data Timeout Counter Value
		unsigned long rsv2:4;				 //!< Reserved
		unsigned long SRFA:1;				 //!<  Software Reset For All
		unsigned long SRCMD:1;				 //!<  Software Reset For CMD Line (SD Mode only)
		unsigned long SRDAT:1;				 //!<  Software Reset For DAT Line (SD Mode only)
		unsigned long rsv3:5;				 //!< Reserved
	} bit;
} EM_SRS11 ;

/**
 * @struct EM_SRS12
 * @brief Error/Normal Interrupt Status
 */
typedef volatile union EM_SRS12 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long CC:1;					 //!<  Command Complete
		unsigned long TC:1;					 //!<  Transfer Complete
		unsigned long BGE:1;				 //!<  Block Gap Event
		unsigned long DMAINT:1;				 //!<  DMA Interrupt
		unsigned long BWR:1;				 //!<  Buffer Write Ready
		unsigned long BRR:1;				 //!<  Buffer Read Ready
		unsigned long CIN:1;				 //!<  Card Insertion
		unsigned long CR:1;					 //!<  Card Removal
		unsigned long CINT:1;				 //!<  Card Interrupt
		unsigned long rsv1:6;				 //!< Reserved
		unsigned long EINT:1;				 //!<  Error Interrupt
		unsigned long ECT:1;				 //!<  Command Timeout Error (SD mode only)
		unsigned long ECCRC:1;				 //!<  Command CRC Error (SD mode only)
		unsigned long ECEB:1;				 //!<  Command End Bit Error (SD mode only)
		unsigned long ECI:1;				 //!<  Command Index Error (SD mode only)
		unsigned long EDT:1;				 //!<  Data Timeout Error (SD mode only)
		unsigned long EDCRC:1;				 //!<  Data CRC Error (SD mode only)
		unsigned long EDEB:1;				 //!<  Data End Bit Error (SD mode only)
		unsigned long ECL:1;				 //!<  Current Limit Error
		unsigned long EAC:1;				 //!<  Auto CMD Error (SD mode only)
		unsigned long EADMA:1;				 //!<  ADMA Error
		unsigned long rsv2:1;				 //!< Reserved
		unsigned long ERSP:1;				 //!<  Response Error (SD Mode only)
		unsigned long rsv3:4;				 //!< Reserved
	} bit;
} EM_SRS12 ;

/**
 * @struct EM_SRS13
 * @brief Error/Normal Status Enable
 */
typedef volatile union EM_SRS13 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long CC_SE:1;				 //!<  Command Complete status enable
		unsigned long TC_SE:1;				 //!<  Transfer Complete status enable
		unsigned long BGE_SE:1;				 //!<  Block Gap Event status enable
		unsigned long DMAINT_SE:1;			 //!<  DMA Interrupt status enable
		unsigned long BWR_SE:1;				 //!<  Buffer Write Ready status enable
		unsigned long BRR_SE:1;				 //!<  Buffer Read Ready status enable
		unsigned long CIN_SE:1;				 //!< Card Insertion status enable
		unsigned long CR_SE:1;				 //!<  Card Removal status enable
		unsigned long CINT_SE:1;			 //!<  CINT_SE - Card Interrupt status enable
		unsigned long rsv1:7;				 //!< Reserved
		unsigned long ECT_SE:1;				 //!<  Command Timeout Error status enable (SD mode
		unsigned long ECCRC_SE:1;			 //!<  Command CRC Error status enable (SD mode only)
		unsigned long ECEB_SE:1;			 //!<  Command End Bit Error status enable (SD mode
		unsigned long ECI_SE:1;				 //!<  Command Index Error status enable (SD mode only)
		unsigned long EDT_SE:1;				 //!<  Data Timeout Error status enable (SD mode only)
		unsigned long EDCRC_SE:1;			 //!<  Data CRC Error status enable (SD mode only)
		unsigned long EDEB_SE:1;			 //!<  Data End Bit Error status enable (SD mode only)
		unsigned long ECL_SE:1;				 //!<  Current Limit Error status enable
		unsigned long EAC_SE:1;				 //!<  Auto CMD12 Error status enable (SD mode only)
		unsigned long EADMA_SE:1;			 //!<  ADMA Error status enable
		unsigned long rsv2:1;				 //!< Reserved
		unsigned long ERSP_SE:1;			 //!<  Response Error Status Enable
		unsigned long rsv3:4;				 //!< Reserved
	} bit;
} EM_SRS13 ;

/**
 * @struct EM_SRS14
 * @brief Error/Normal Signal Enable
 */
typedef volatile union EM_SRS14 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long CC_IE:1;				 //!<  Command Complete interrupt enable
		unsigned long TC_IE:1;				 //!<  Transfer Complete interrupt enable
		unsigned long BGE_IE:1;				 //!<  Block Gap Event interrupt enable
		unsigned long DMAINT_IE:1;			 //!<  DMA Interrupt enable
		unsigned long BWR_IE:1;				 //!<  Buffer Write Ready interrupt enable
		unsigned long BRR_IE:1;				 //!<  Buffer Read Ready interrupt enable
		unsigned long CIN_IE:1;				 //!<  Card Insertion interrupt enable
		unsigned long CR_IE:1;				 //!<  Card Removal interrupt enable
		unsigned long CINT_IE:1;			 //!<  Card Interrupt - interrupt enable
		unsigned long rsv1:7;				 //!< Reserved
		unsigned long ECT_IE:1;				 //!<  Command Timeout Error interrupt enable (SD mode
		unsigned long ECCRC_IE:1;			 //!<  Command CRC Error interrupt enable (SD mode
		unsigned long ECEB_IE:1;			 //!<  Command End Bit Error interrupt enable (SD mode
		unsigned long ECI_IE:1;				 //!< Command Index Error interrupt enable (SD mode only)
		unsigned long EDT_IE:1;				 //!<  ata Timeout Error interrupt enable (SD mode only)
		unsigned long EDCRC_IE:1;			 //!<  Data CRC Error interrupt enable (SD mode only)
		unsigned long EDEB_IE:1;			 //!<  Data End Bit Error interrupt enable (SD mode only)
		unsigned long ECL_IE:1;				 //!<  Current Limit Error interrupt enable
		unsigned long EAC12_IE:1;			 //!<  Auto CMD12 Error interrupt enable (SD mode only)
		unsigned long EADMA_IE:1;			 //!<  ADMA Error interrupt enable
		unsigned long rsv2:1;				 //!< Reserved
		unsigned long ERSP_IE:1;			 //!<  Response Error Interrupt Enable
		unsigned long rsv3:4;				 //!< Reserved
	} bit;
} EM_SRS14 ;

/**
 * @struct EM_SRS15
 * @brief Host Control #2 / Auto CMD Error Status
 */
typedef volatile union EM_SRS15 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long ACNE:1;				 //!< Auto CMD12 Not Executed
		unsigned long ACTE:1;				 //!<  Auto CMD Timeout Error
		unsigned long ACCE:1;				 //!<  Auto CMD CRC Error
		unsigned long ACEBE:1;				 //!<  Auto CMD End Bit Error
		unsigned long ACIE:1;				 //!<  Auto CMD Index Error
		unsigned long ACRE:1;				 //!<  Auto CMD Response Error
		unsigned long rsv1:1;				 //!< Reserved
		unsigned long CNIACE:1;				 //!<  Command Not Issued By Auto CMD12 Error
		unsigned long rsv2:8;				 //!< Reserved
		unsigned long UMS:3;				 //!<  UHS Mode Select
		unsigned long V18SE:1;				 //!<  1.8V Signaling Enable (UHS-I only)
		unsigned long DSS:2;				 //!<  Driver Strength Select (UHS-I only)
		unsigned long EXTNG:1;				 //!<  Execute Tuning (UHS-I only)
		unsigned long SCS:1;				 //!<  Sampling Clock Select (UHS-I only)
		unsigned long rsv3:4;				 //!< Reserved
		unsigned long HV4E:1;				 //!<  Host Version 4.00 Enable
		unsigned long A64B:1;				 //!<  64-bit Addressing
		unsigned long rsv4:1;				 //!< Reserved
		unsigned long PVE:1;				 //!<  Preset Value Enable
	} bit;
} EM_SRS15 ;

/**
 * @struct EM_SRS16
 * @brief Capabilities #1
 */
typedef volatile union EM_SRS16 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long TCF:6;				 //!<  Timeout Clock Frequency
		unsigned long rsv1:1;				 //!< Reserved
		unsigned long TCU:1;				 //!<  Timeout Clock Unit
		unsigned long BCSDCLK:8;			 //!<  Base Clock Frequency For SD Clock
		unsigned long MBL:2;				 //!< Max Block Length
		unsigned long _8EDS:1;				 //!<  8-bit Embedded Device Support
		unsigned long ADMA2S:1;				 //!<  ADMA2 Support
		unsigned long ADMA1S:1;				 //!<  ADMA1 Support
		unsigned long HSS:1;				 //!<  High Speed Support
		unsigned long DMAS:1;				 //!<  SDMA Support
		unsigned long SRS:1;				 //!<  Suspend / Resume Support
		unsigned long VS33:1;				 //!<  Voltage Support 3.3V
		unsigned long VS30:1;				 //!<  Voltage Support 3.0V
		unsigned long VS18:1;				 //!< - Voltage Support 1.8V
		unsigned long rsv2:1;				 //!< Reserved
		unsigned long A64S:1;				 //!<  64-bit System Addressing Support
		unsigned long AIS:1;				 //!<  Asynchronous Interrupt Support
		unsigned long SLT:2;				 //!<  Slot Type
	} bit;
} EM_SRS16 ;

/**
 * @struct EM_SRS17
 * @brief Capabilities #2
 */
typedef volatile union EM_SRS17 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long SDR50:1;				 //!<  SDR50 Supported
		unsigned long SDR104:1;				 //!<  SDR104 Supported
		unsigned long DDR50:1;				 //!<  DDR50 Supported
		unsigned long UHSII:1;				 //!<  UHS-II Supported
		unsigned long DRVA:1;				 //!<  1.8V Line Driver Type A Supported
		unsigned long DRVC:1;				 //!< 1.8V Line Driver Type C Supported
		unsigned long DRVD:1;				 //!<  1.8V Line Driver Type D Supported
		unsigned long rsv1:1;				 //!< Reserved
		unsigned long RTNGCNT:4;			 //!<  Timer Count for Re-Tuning
		unsigned long rsv2:1;				 //!< Reserved
		unsigned long UTSM50:1;				 //!<  Use Tuning for SDR50
		unsigned long RTNGM:2;				 //!<  Re-Tuning Modes
		unsigned long CLKMPR:8;				 //!<  Clock Multiplier
		unsigned long rsv3:4;				 //!< Reserved
		unsigned long VDD2S:1;				 //!<  VDD2 Supported
		unsigned long rsv4:3;				 //!< Reserved
	} bit;
} EM_SRS17 ;

/**
 * @struct EM_SRS18
 * @brief Capabilities #3
 */
typedef volatile union EM_SRS18 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long MC33:8;				 //!<  Maximum Current for 3.3V
		unsigned long MC30:8;				 //!<  Maximum Current for 3.0V
		unsigned long MC18:8;				 //!<  Maximum Current for 1.8V
		unsigned long rsv:8;				 //!< Reserved
	} bit;
} EM_SRS18 ;

/**
 * @struct EM_SRS19
 * @brief Capabilities #4
 */
typedef volatile union EM_SRS19 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long MC18V2:8;				 //!<  Maximum Current for 1.8V VDD2
		unsigned long rsv:24;				 //!< Reserved
	} bit;
} EM_SRS19 ;

/**
 * @struct EM_SRS20
 * @brief Force Event
 */
typedef volatile union EM_SRS20 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long ACNE_FE:1;			 //!<  Force Auto CMD12 Not Executed Event
		unsigned long ACTE_FE:1;			 //!<  Force Auto CMD Timeout Error Event
		unsigned long ACCE_FE:1;			 //!<  Force Auto CMD CRC Error Event
		unsigned long ACEBE_FE:1;			 //!<  Force Auto CMD End Bit Error Event
		unsigned long ACIE_FE:1;			 //!<  Force Auto CMD Index Error Event
		unsigned long rsv1:2;				 //!< Reserved
		unsigned long CNIACE_FE:1;			 //!<  Force Command Not Issued By Auto CMD12 Error
		unsigned long rsv2:8;				 //!< Reserved
		unsigned long ECT_FE:1;				 //!<  Force Command Timeout Error Event
		unsigned long ECCRC_FE:1;			 //!<  Force Command CRC Error Event
		unsigned long ECEB_FE:1;			 //!<  Force Command End Bit Error Event
		unsigned long ECI_FE:1;				 //!<  Force Command Index Error Event
		unsigned long EDT_FE:1;				 //!<  Force Data Timeout Error Event
		unsigned long EDCRC_FE:1;			 //!<  Force Data CRC Error Event
		unsigned long EDEB_FE:1;			 //!<  Force Data End Bit Error Event
		unsigned long ECL_FE:1;				 //!<  Force Current Limit Error Event
		unsigned long EAC12_FE:1;			 //!<  Force Auto CMD12 Error Event
		unsigned long EADMA_FE:1;			 //!<  Force ADMA Error Event
		unsigned long rsv3:1;				 //!< Reserved
		unsigned long ERESP_FE:1;			 //!<  Force Response Error Event
		unsigned long rsv4:4;				 //!< Reserved
	} bit;
} EM_SRS20 ;

/**
 * @struct EM_SRS21
 * @brief ADMA Error Status
 */
typedef volatile union EM_SRS21 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long EADMAS:2;				 //!<  ADMA Error State
		unsigned long EADMAL:1;				 //!<  ADMA Length Mismatch Error
		unsigned long rsv:29;				 //!< Reserved
	} bit;
} EM_SRS21 ;

/**
 * @struct EM_SRS22
 * @brief ADMA/SDMA System Address#1
 */
typedef volatile union EM_SRS22 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long DMASA1:32;			 //!<  ADMA System Address
	} bit;
} EM_SRS22 ;

/**
 * @struct EM_SRS23
 * @brief ADMA/SDMA System Address#2
 */
typedef volatile union EM_SRS23 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long DMASA2:32;			 //!< ADMA System Address #2
	} bit;
} EM_SRS23 ;

/**
 * @struct EM_SRS24
 * @brief Preset Value
 */
typedef volatile union EM_SRS24 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long rsv1:16;				 //!< Reserved
		unsigned long SDCFSPV_25_16:10;		 //!< SDCLK Clock Frequency Select Preset Value
		unsigned long rsv2:4;				 //!< Reserved
		unsigned long DSSPV_31_30:2;		 //!< Driver Strngth Select Preset Value
	} bit;
} EM_SRS24 ;

/**
 * @struct EM_SRS25
 * @brief Preset Value
 */
typedef volatile union EM_SRS25 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long SDCFSPV_09_00:10;		 //!< SDCLK Clock Frequency Select Preset Value
		unsigned long rsv1:4;				 //!< Reserved
		unsigned long DSSPV_15_14:2;		 //!< Driver Strngth Select Preset Value
		unsigned long SDCFSPV_25_16:10;		 //!< SDCLK Clock Frequency Select Preset Value
		unsigned long rsv2:4;				 //!< Reserved
		unsigned long DSSPV_31_30:2;		 //!< Driver Strngth Select Preset Value
	} bit;
} EM_SRS25 ;

/**
 * @struct EM_SRS26
 * @brief Preset Value
 */
typedef volatile union EM_SRS26 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long SDCFSPV_09_00:10;		 //!< SDCLK Clock Frequency Select Preset Value
		unsigned long rsv1:4;				 //!< Reserved
		unsigned long DSSPV_15_14:2;		 //!< Driver Strngth Select Preset Value
		unsigned long SDCFSPV_25_16:10;		 //!< SDCLK Clock Frequency Select Preset Value
		unsigned long rsv2:4;				 //!< Reserved
		unsigned long DSSPV_31_30:2;		 //!< Driver Strngth Select Preset Value
	} bit;
} EM_SRS26 ;

/**
 * @struct EM_SRS27
 * @brief Preset Value
 */
typedef volatile union EM_SRS27 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long SDCFSPV_09_00:10;		 //!< SDCLK Clock Frequency Select Preset Value
		unsigned long rsv1:4;				 //!< Reserved
		unsigned long DSSPV_15_14:2;		 //!< Driver Strngth Select Preset Value
		unsigned long SDCFSPV_25_16:10;		 //!< SDCLK Clock Frequency Select Preset Value
		unsigned long rsv2:4;				 //!< Reserved
		unsigned long DSSPV_31_30:2;		 //!< Driver Strngth Select Preset Value
	} bit;
} EM_SRS27 ;

/**
 * @struct EM_REGISTER_SRS
 * @brief Slot Register Set
 */
typedef volatile struct EM_REGISTER_SRS {
	EM_SRS0 SRS0;							 //!< SDMA System Address / Argument 2 
	EM_SRS1 SRS1;							 //!< Block Size / Block Count 
	EM_SRS2 SRS2;							 //!< Argument 1
	EM_SRS3 SRS3;							 //!< Command/Transfer Mode 
	EM_REGISTER_UNION_SRS4_7 u_4_7;			 //!< Response Register#1-#4
	EM_SRS8 SRS8;							 //!< Data Buffer 
	EM_SRS9 SRS9;							 //!< Present State Register
	EM_SRS10 SRS10;							 //!< Host Control 1 
	EM_SRS11 SRS11;							 //!< Host Control 2 (Clock, Timeout, Reset)
	EM_SRS12 SRS12;							 //!< Error/Normal Interrupt Status
	EM_SRS13 SRS13;							 //!< Error/Normal Status Enable
	EM_SRS14 SRS14;							 //!< Error/Normal Signal Enable
	EM_SRS15 SRS15;							 //!< Host Control #2 / Auto CMD Error Status
	EM_SRS16 SRS16;							 //!< Capabilities #1
	EM_SRS17 SRS17;							 //!< Capabilities #2
	EM_SRS18 SRS18;							 //!< Capabilities #3
	EM_SRS19 SRS19;							 //!< Capabilities #4
	EM_SRS20 SRS20;							 //!< Force Event
	EM_SRS21 SRS21;							 //!< ADMA Error Status
	EM_SRS22 SRS22;							 //!< ADMA/SDMA System Address#1
	EM_SRS23 SRS23;							 //!< ADMA/SDMA System Address#2
	EM_SRS24 SRS24;							 //!< Preset Value
	EM_SRS25 SRS25;							 //!< Preset Value
	EM_SRS26 SRS26;							 //!< Preset Value
	EM_SRS27 SRS27;							 //!< Preset Value
	unsigned long reserved[ 63 - 28 ];
} EM_REGISTER_SRS;

/**
 * @struct EM_CRS63
 * @brief Host Controller Version/Slot Interrupt Status
 */
typedef volatile union EM_CRS63 {
	unsigned long word;						 //!< 4byte word
	struct {
		unsigned long ISES:8;				 //!< Interrupt Signal For Each Slot
		unsigned long rsv1:8;				 //!< reserved
		unsigned long SVN:8;				 //!< Specification Version Number
		unsigned long rsv2:8;				 //!< reserved
	} bit;
} EM_CRS63 ;

/**
 * @struct EM_REGISTER_CRS
 * @brief Common Register Set
 */
typedef volatile struct EM_REGISTER_CRS {
	EM_CRS63 CRS63;							 //!< Host Controller Version/Slot Interrupt Status
} EM_REGISTER_CRS ;

/**
 * @struct EM_REGISTER_SRS_SLOT
 * @brief SRS Slot
 */
typedef volatile struct EM_REGISTER_SRS_SLOT {
	EM_REGISTER_SRS			 SRS ;					   //!< 0x200 - 0x2FC
	EM_REGISTER_CRS			 unused_CRS ;			   //!< 0x2FC - 0x2FF
	unsigned char			 reserved_srs[0x100] ;	   //!< 0x300 - 0x3FF
} EM_REGISTER_SRS_SLOT ;

/**
 * @struct EM_REGISTER_SFR
 * @brief Special Function Registers
 */
typedef volatile struct EM_REGISTER_SFR {
	EM_REGISTER_HRS			 HRS ;						   //!< 0x000 - 0x0FC
	EM_REGISTER_CRS			 CRS ;						   //!< 0x0FC - 0x0FF
	unsigned char			 reserved_hrs[0x100] ;		   //!< 0x100 - 0x1FF
	EM_REGISTER_SRS_SLOT	 sSRS_Slot[ EM_CONFIG_SLOT_COUNT ] ;  //!< SRS
} EM_REGISTER_SFR ;


extern volatile EM_REGISTER_SFR IO_EMMC;


#endif
