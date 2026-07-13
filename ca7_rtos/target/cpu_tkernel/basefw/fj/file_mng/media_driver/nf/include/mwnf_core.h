/**
 * @file		mwnf_core.h
 * @brief		Nand Flash driver core header file
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef __MWNF_CORE_H
#define __MWNF_CORE_H


/*----------------------------------------------------------------------*/
/* Include Files														*/
/*----------------------------------------------------------------------*/

#include "ddim_typedef.h"
#include "mwnf.h"
#include "nf.h"


//local optional flag
#define CO_SUPPORT_FAT32

#define CO_USE_XDMAC

#define CO_NF_DEBUG_PRINT_ON
#define CO_NF_ERR_PRINT_ON

// ===============================================
//   Debug Print
// ===============================================
#ifdef CO_NF_DEBUG_PRINT_ON
#define	M_NF_DEBUG_PRINT(arg)	(Media_Custom_Info_Printer arg)
#else	// CO_NF_DEBUG_PRINT_ON
#define M_NF_DEBUG_PRINT(arg)
#endif	// CO_NF_DEBUG_PRINT_ON

extern volatile UCHAR gFlgRdyWaitPoll;

#ifdef CO_NF_ERR_PRINT_ON
#define M_NF_ERR_PRINT(arg)				\
{										\
	if (gFlgRdyWaitPoll == 0) {			\
		(Media_Custom_Err_Printer arg);	\
	}									\
}
#else	// CO_NF_ERR_PRINT_ON
#define M_NF_ERR_PRINT(arg)
#endif	// CO_NF_ERR_PRINT_ON

#define D_MWNF_XDMAC_CH (0)

#define D_MWNF_ASSIGNED (0)
#define D_MWNF_BLK_NO_ASSIGN_MARKER (0xFFFF)
#define D_MWNF_BB_MARKER (0xFFFF)

#define D_MWNF_378MHz (2)
#define D_MWNF_756MHz (3)

#define D_MWNF_MAIN_BUFFER_SIZE (4*1024)

#define D_MWNF_SPARE_BUFFER_SIZE (512) 
#define D_MWNF_SECTOR_BUFFER_SIZE (512*3)

#define D_MWNF_MAX_BLOCK_NUM (8192)
#define D_MWNF_MAX_LOG_BLOCK_NUM (8000)
#define D_MWNF_MAX_BAD_BLOCK_NUM (200)
#define D_MWNF_MAX_CE_NUM (2)
#define D_MWNF_MAX_BBT_NUM (2)

#define D_MWNF_LOOKUP_TBL_SIZE		(2)

#define D_MWNF_INITIAL_BB_FLAG		(0x8000)

#define D_MWNF_BLK_TYPE_DATA		(0)
#define D_MWNF_BLK_TYPE_BBT_MAIN	(1)
#define D_MWNF_BLK_TYPE_BBT_MIRR	(2)

#define D_MWNF_MAX_WRITE_CNT		(254)

#define D_MWNF_WRITE_OK				(0)
#define D_MWNF_WRITE_NG				(1)

#define D_MWNF_FREE_BLK				(0x2000)
#define D_MWNF_BAD_BLK_TBL			(0x4000)
#define D_MWNF_INIT_BAD_BLK			(0x6000)
#define D_MWNF_USER_BAD_BLK			(0x8000)
#define D_MWNF_ETC_BLK				(0xA000)
#define D_MWNF_BLK_TYPE_MASK		(0xE000)
#define D_MWNF_LBA_MASK				(0x1FFF)

#define D_MWNF_BB_ALREADY_EXIST		(0)
#define D_MWNF_BB_ADD				(1)
#define D_MWNF_BB_NONE				(2)

#define D_MWNF_BBT_MAIN				(0)
#define D_MWNF_BBT_MIRR				(1)

#define D_MWNF_SKIP_BYTES_MAX		(63)

#define D_MWNF_INI_BB_OFST_LRG		(0)
#define D_MWNF_INI_BB_OFST_SML		(5)

//
//Common-used
//
#define D_MWNF_DISABLE	(0)
#define D_MWNF_ENABLE	(1)

#define D_MWNF_HW_SECTOR_SIZE_FOR_META	(1024+4)
#define D_MWNF_HW_SECTOR_SIZE	(1024)
#define D_MWNF_SECTOR_SIZE	(512)
#define FS_SECTOR_SIZE		(512)

#define D_MWNF_SECTOR_COUNT	(2)
#define D_MWNF_SPARE_COUNT	(1)

#define D_MWNF_SKIPBYTE0	(0)
#define D_MWNF_SKIPBYTE2	(2)


#define D_MWNF_PAGE_SIZE_512	(512)
#define D_MWNF_PAGE_SIZE_2048	(2048)
#define D_MWNF_PAGE_SIZE_4096	(4096)
#define D_MWNF_PAGE_SIZE_8192	(8192)

#define D_MWNF_WRITE_RETRY_CNT	(1)
#define D_MWNF_READ_RETRY_CNT	(1)

//
//device-related
//
//for device compliance
#define D_MWNF_ONFI (0)
#define D_MWNF_LEGACY (1)

//onfi features
#define D_MWNF_16BIT_DATA_BUS_SUPPORT (1<<0)
#define D_MWNF_MULTI_LUN_SUPPORT (1<<1)
#define D_MWNF_NON_SEQ_PAGE_PGM_SUPPORT (1<<2)
#define D_MWNF_INTERLEAVED_PGM_ERASE_SUPPORT (1<<3)
#define D_MWNF_ODD_TO_EVEN_PAGE_COPY_BACK_SUPPORT (1<<4)
#define D_MWNF_SOURCE_SYNCHRONOUS_SUPPORT (1<<5)
#define D_MWNF_INTERLEAVED_READ_SUPPORT (1<<6)
#define D_MWNF_EXTENDED_PARAM_SUPPORT (1<<7)
#define D_MWNF_PGM_PAGE_REG_CLEAR_ENHANCEMENT (1<<8)

//optional commands
#define D_MWNF_PAGE_CACHE_PGM_SUPPORT (1<<0)
#define D_MWNF_PAGE_CACHE_READ_SUPPORT (1<<1)
#define D_MWNF_GET_SET_FEATURES_SUPPORT (1<<2)
#define D_MWNF_READ_STATUS_ENHANCED_CMD_SUPPORT (1<<3)
#define D_MWNF_COPY_BACK_SUPPORT (1<<4)
#define D_MWNF_READ_UNIQUE_ID_SUPPORT (1<<5)
#define D_MWNF_CHANGE_READ_COLUMN_ENHANCED_SUPPORT (1<<6)
#define D_MWNF_CHANGE_ROW_ADDRESS_SUPPORT (1<<7)
#define D_MWNF_CHANGE_SMALL_DATA_MOVE (1<<8)
#define D_MWNF_RESET_LUN_SUPPORT (1<<9)

//timing mode
#define D_MWNF_TIMING_MODE0_SUPPORT (1<<0)
#define D_MWNF_TIMING_MODE1_SUPPORT (1<<1)
#define D_MWNF_TIMING_MODE2_SUPPORT (1<<2)
#define D_MWNF_TIMING_MODE3_SUPPORT (1<<3)
#define D_MWNF_TIMING_MODE4_SUPPORT (1<<4)
#define D_MWNF_TIMING_MODE5_SUPPORT (1<<5)

//ONFI-compliant
#define D_MWNF_DEVICE_TYPE			 (3<<30)
#define D_MWNF_ONFI_COMPLIANT_DEVICE (1<<30)

//
//Control Register related
//
//Field of Control Register
#define D_MWNF_CTRL_BACK_GND	(0x00000000)
#define D_MWNF_CMD_MAP_BIT	(26)
//#define D_MWNF_BANK_BIT		(24)
//#define D_BUFF_ADDR	(2)

//MAP Command
#define D_MWNF_MAP00	(0x00)
#define D_MWNF_MAP01	(0x01)
#define D_MWNF_MAP10	(0x02)
#define D_MWNF_MAP11	(0x03)

//definition of bank
#define D_MWNF_BANK0	(0)
#define D_MWNF_BANK1	(1)

//definition related to MAP11
#define D_MWNF_COMMAND_CYCLE	(0)
#define D_MWNF_ADDRESS_CYCLE	(1)
#define D_MWNF_DATA_CYCLE	(2)

//
//Data Register related
//
//MAP10 Command
#define D_MWNF_MAP10_ERASE									(0x01)
#define D_MWNF_MAP10_UNLOCK_START_ADDR						(0x10)
#define D_MWNF_MAP10_UNLOCK_END_ADDR_AND_INITIATE_UNLOCK	(0x11)
#define D_MWNF_MAP10_INITIATE_A_LOCK_OF_ALL_BLOCKS			(0x21)
#define D_MWNF_MAP10_INITIATE_A_LOCK_TIGHT_OF_ALL_BLOCKS	(0x31)
#define D_MWNF_MAP10_SPARE_MODE								(0x41)
#define D_MWNF_MAP10_DEFAULT_MODE							(0x42)
#define D_MWNF_MAP10_MAIN_SPARE_MODE						(0x43)
#define D_MWNF_MAP10_LOAD_PAGE_TO_BUFFER					(0x60)
#define D_MWNF_MAP10_SET_DEST_ADDR_FOR_PAGE_BUFFER			(0x61)
#define D_MWNF_MAP10_WRITE_PAGE_BUFFER						(0x62)
#define D_MWNF_MAP10_CUSTOM_MAIN_EEC_DISABLE_MODE			(0x77776666) // cutomised mode not cadense one
#define D_MWNF_MAP10_SET_COPY_SRC_ADDR						(0x1000)
#define D_MWNF_MAP10_SET_COPY_DST_ADDR						(0x1100)
#define D_MWNF_MAP10_SET_PIPELINE_READ_WRITE				(0x2000)

#define D_MWNF_COPY_BACK_ENABLE	(0)
#define D_MWNF_COPY_BACK_DISABLE	(1)

// CMD_DMA_Support
#define D_MWNF_TIMEOUT_DISABLE	(1<<4)


//DataDMA
#define D_MWNF_DATA_DMA_BEAT0_BACK_GND	(0x00002000)
#define D_MWNF_PP_BIT					(0)
#define D_MWNF_RW_BIT					(8)
#define D_MWNF_BURST_LENGTH_BIT		(16)
//#define D_MWNF_INT_BIT					(24)

#define D_DUMMY_BLK_NO (0)
#define D_DUMMY_PAGE_NO (0)

#define D_MWNF_SINGLE (8)
#define D_MWNF_INCR2 (16)
#define D_MWNF_INCR4 (32)
#define D_MWNF_INCR8 (64)
#define D_MWNF_INCR16 (128)

#define D_MWNF_READ (0)
#define D_MWNF_WRITE (1)

//Command DMA
#define D_MWNF_CMD_DMA_ADDR_ENC		(0x08000000)
#define D_MWNF_CMD_DMA_BEAT0_BACK_GND	(0x00000080)

// PIO work Mode Command
#define D_MWNF_PAGE_NUM_MAX			(256)
#define D_MWNF_CMD_REG0_ER			(0x40001000)	// erase
#define D_MWNF_CMD_REG0_RT			(0x40001100)	// reset
#define D_MWNF_CMD_REG0_RD			(0x40202200)	// read
#define D_MWNF_CMD_REG0_WR			(0x40202100)	// write
#define D_MWNF_CMD_REG0_SF			(0x40000100)	// Set Features
#define D_MWNF_CMD_REG1_SF			(0x00000001)	// Set Features

#define D_MWNF_CMD_INT_ON			(0x00100000)	// INT=ON
#define D_MWNF_CMD_DMA_MASTER		(0x00200000)	// DMA_SEL=Master
#define D_MWNF_INT_BIT				(20)

// DMA setting
#define D_MWNF_DMA_SETTING_OTE		(0x00010000)
#define D_MWNF_DMA_BURST_SEL_8		(0x00000007)
#define D_MWNF_DMA_BURST_SEL_16		(0x0000000F)
#define D_MWNF_DMA_BURST_SEL_32		(0x0000001F)





//
//command status related
//
#define D_MWNF_CMD_STATUS_ERROR_ALL				(0x00017803)
#define D_MWNF_CMD_STATUS_ERROR_INDEX			(0xFF<<24)
#define D_MWNF_CMD_STATUS_BUS_ERROR				(1<<16)
#define D_MWNF_CMD_STATUS_COMPLETE				(1<<15)
#define D_MWNF_CMD_STATUS_FAIL					(1<<14)
#define D_MWNF_CMD_STATUS_DQS_ERROR				(1<<13)
#define D_MWNF_CMD_STATUS_DEVICE_ERROR			(1<<12)
#define D_MWNF_CMD_STATUS_ERASED_PAGE			(1<<11)
#define D_MWNF_CMD_STATUS_MAX_ERROR				(0xFF<<2)
#define D_MWNF_CMD_STATUS_ECC_ERR				(1<<1)
#define D_MWNF_CMD_STATUS_CMD_ERROR				(1<<0)

//
//control status related
//
#define D_MWNF_CTRL_STATUS_INIT_COMP			(1<<9)
#define D_MWNF_CTRL_STATUS_CTRL_BUSY			(1<<8)
#define D_MWNF_CTRL_STATUS_MC_BUSY				(1<<3)
#define D_MWNF_CTRL_STATUS_CMD_ENG_BUSY			(1<<2)
#define D_MWNF_CTRL_STATUS_MDMA_BUSY			(1<<1)
#define D_MWNF_CTRL_STATUS_SDMA_BUSY			(1<<0)

//
//interrupt status related
//
#define D_MWNF_INT_STATUS_ALL					(0x00770000)
#define D_MWNF_INT_STATUS_SDMA_ERR				(1<<22)
#define D_MWNF_INT_STATUS_SDMA_TRIGG			(1<<21)
#define D_MWNF_INT_STATUS_CMD_IGNORED			(1<<20)
#define D_MWNF_INT_STATUS_DDMA_TERR				(1<<18)
#define D_MWNF_INT_STATUS_CDMA_TERR				(1<<17)
#define D_MWNF_INT_STATUS_CDMA_IDLE				(1<<16)

//
//trd interrupt status related
//
#define D_MWNF_TRDINT_STATUS_ALL				(0x0000000F)
#define D_MWNF_TRDINT_STATUS_TRD7				(1<<7)
#define D_MWNF_TRDINT_STATUS_TRD6				(1<<6)
#define D_MWNF_TRDINT_STATUS_TRD5				(1<<5)
#define D_MWNF_TRDINT_STATUS_TRD4				(1<<4)
#define D_MWNF_TRDINT_STATUS_TRD3				(1<<3)
#define D_MWNF_TRDINT_STATUS_TRD2				(1<<2)
#define D_MWNF_TRDINT_STATUS_TRD1				(1<<1)
#define D_MWNF_TRDINT_STATUS_TRD0				(1<<0)





#define D_MWNF_INT_CMDDMA_ALL			(0x7F)
#define D_MWNF_INT_CMDDMA_IDEL			(1<<6)
#define D_MWNF_INT_CMDDMA_MEMCPY_COMP	(1<<5)
#define D_MWNF_INT_CMDDMA_DESC_COMP3	(1<<4)
#define D_MWNF_INT_CMDDMA_DESC_COMP2	(1<<3)
#define D_MWNF_INT_CMDDMA_DESC_COMP1	(1<<2)
#define D_MWNF_INT_CMDDMA_DESC_COMP0	(1<<1)
#define D_MWNF_INT_CMDDMA_TARGET_ERROR	(1<<0)

#define D_MWNF_INT_CMDDMA_ERROR_CH_ALL	(0xF)
#define D_MWNF_INT_CMDDMA_ERROR_CH3	(1<<3)
#define D_MWNF_INT_CMDDMA_ERROR_CH2	(1<<2)
#define D_MWNF_INT_CMDDMA_ERROR_CH1	(1<<1)
#define D_MWNF_INT_CMDDMA_ERROR_CH0	(1<<0)

//
//Optimization Sequence related
//
#define D_MWNF_POLLING_MODE	(0x00)

// CMD_DMA_Support
#define D_MWNF_INTERRUPT_MODE	(0x1)

//
//ECC related
//
#define D_MWNF_ECC_CAP4		(1)
#define D_MWNF_ECC_CAP8		(2)
#define D_MWNF_ECC_CAP12	(3)
#define D_MWNF_ECC_CAP16	(4)
#define D_MWNF_ECC_AUTO		(0xFF)

//
//Timeout
//
#define D_MWNF_WAIT_TIME_RESET			(5000)
#define D_MWNF_WAIT_TIME_DMA_W			(5000)
#define D_MWNF_WAIT_TIME_DMA_R			(5000)
#define D_MWNF_WAIT_TIME_ERASE			(5000)
#define D_MWNF_WAIT_TIME_COPYBACK		(5000)		// no use
#define D_MWNF_WAIT_TIME_XDMAC			(5000)
#define D_MWNF_WAIT_TIME_NF_LP			(3000)
#define D_MWNF_WAIT_TIME_XDMAC_LP		(3000)
#define	D_MWNF_WAIT_TIME_CMD_DESC		(30)

#define D_MWNF_WAIT_TIME_NF_MACRO		(5000)

//
//Event Flag
//
#define D_MWNF_FLG_CDMA_IDLE				(0x00000001)
#define D_MWNF_FLG_CDMA_TERR				(0x00000002)
#define D_MWNF_FLG_DDMA_TERR				(0x00000004)
#define D_MWNF_FLG_CMD_IGNORED				(0x00000008)
#define D_MWNF_FLG_SDMA_TRIGG				(0x00000010)
#define D_MWNF_FLG_SDMA_ERR					(0x00000020)
#define D_MWNF_FLG_ERR_ALL					(D_MWNF_FLG_CDMA_TERR | D_MWNF_FLG_DDMA_TERR | D_MWNF_FLG_CMD_IGNORED | D_MWNF_FLG_SDMA_ERR)

#define D_MWNF_FLG_TRDCP_TRD0				(0x00000040)
#define D_MWNF_FLG_TRDCP_TRD1				(0x00000080)
#define D_MWNF_FLG_TRDCP_TRD2				(0x00000100)
#define D_MWNF_FLG_TRDCP_TRD3				(0x00000200)
#define D_MWNF_FLG_TRDCP_TRD4				(0x00000400)
#define D_MWNF_FLG_TRDCP_TRD5				(0x00000800)
#define D_MWNF_FLG_TRDCP_TRD6				(0x00001000)
#define D_MWNF_FLG_TRDCP_TRD7				(0x00002000)

#define D_MWNF_FLG_TRDER_TRD0				(0x00004000)
#define D_MWNF_FLG_TRDER_TRD1				(0x00008000)
#define D_MWNF_FLG_TRDER_TRD2				(0x00010000)
#define D_MWNF_FLG_TRDER_TRD3				(0x00020000)
#define D_MWNF_FLG_TRDER_TRD4				(0x00040000)
#define D_MWNF_FLG_TRDER_TRD5				(0x00080000)
#define D_MWNF_FLG_TRDER_TRD6				(0x00100000)
#define D_MWNF_FLG_TRDER_TRD7				(0x00200000)

#define D_MWNF_FLG_TRDTM_TRD0				(0x00400000)
#define D_MWNF_FLG_TRDTM_TRD1				(0x00800000)
#define D_MWNF_FLG_TRDTM_TRD2				(0x01000000)
#define D_MWNF_FLG_TRDTM_TRD3				(0x02000000)
#define D_MWNF_FLG_TRDTM_TRD4				(0x04000000)
#define D_MWNF_FLG_TRDTM_TRD5				(0x08000000)
#define D_MWNF_FLG_TRDTM_TRD6				(0x10000000)
#define D_MWNF_FLG_TRDTM_TRD7				(0x20000000)

#define D_MWNF_FLG_TRD0_ALL					(D_MWNF_FLG_TRDCP_TRD0 | D_MWNF_FLG_TRDER_TRD0 | D_MWNF_FLG_TRDTM_TRD0)
#define D_MWNF_FLG_TRD1_ALL					(D_MWNF_FLG_TRDCP_TRD1 | D_MWNF_FLG_TRDER_TRD1 | D_MWNF_FLG_TRDTM_TRD1)
#define D_MWNF_FLG_TRD2_ALL					(D_MWNF_FLG_TRDCP_TRD2 | D_MWNF_FLG_TRDER_TRD2 | D_MWNF_FLG_TRDTM_TRD2)
#define D_MWNF_FLG_TRD3_ALL					(D_MWNF_FLG_TRDCP_TRD3 | D_MWNF_FLG_TRDER_TRD3 | D_MWNF_FLG_TRDTM_TRD3)
#define D_MWNF_FLG_TRD4_ALL					(D_MWNF_FLG_TRDCP_TRD4 | D_MWNF_FLG_TRDER_TRD4 | D_MWNF_FLG_TRDTM_TRD4)
#define D_MWNF_FLG_TRD5_ALL					(D_MWNF_FLG_TRDCP_TRD5 | D_MWNF_FLG_TRDER_TRD5 | D_MWNF_FLG_TRDTM_TRD5)
#define D_MWNF_FLG_TRD6_ALL					(D_MWNF_FLG_TRDCP_TRD6 | D_MWNF_FLG_TRDER_TRD6 | D_MWNF_FLG_TRDTM_TRD6)
#define D_MWNF_FLG_TRD7_ALL					(D_MWNF_FLG_TRDCP_TRD7 | D_MWNF_FLG_TRDER_TRD7 | D_MWNF_FLG_TRDTM_TRD7)


#define D_MWNF_FLG_CMDDMA_IDEL				(0x00400000)
#define D_MWNF_FLG_CMDDMA_MEMCPY_COMP		(0x00200000)
#define D_MWNF_FLG_CMDDMA_DESC_COMP3		(0x00100000)
#define D_MWNF_FLG_CMDDMA_DESC_COMP2		(0x00080000)
#define D_MWNF_FLG_CMDDMA_DESC_COMP1		(0x00040000)
#define D_MWNF_FLG_CMDDMA_DESC_COMP0		(0x00020000)
#define D_MWNF_FLG_CMDDMA_TARGET_ERROR		(0x00010000)

#define D_MWNF_FLG_CMDDMA_ERROR_CH3			(0x08000000)
#define D_MWNF_FLG_CMDDMA_ERROR_CH2			(0x04000000)
#define D_MWNF_FLG_CMDDMA_ERROR_CH1			(0x02000000)
#define D_MWNF_FLG_CMDDMA_ERROR_CH0			(0x01000000)

#define D_MWNF_FLG_XDMAC_OK					(0x10000000)
#define D_MWNF_FLG_XDMAC_ERR				(0x20000000)


// CMD DAMA
// CMD_DMA_Support
#define D_NF_CMD_DMA_DESCRIPTOR_NUM	(8)
#define D_NF_CMD_DMA_SYNC_FLAG_NUM	(64)

// CMD_DMA_Support
#define D_NF_ALL_ZERO	(0x00000000)


// Cmd type
// CMD_DMA_Support
#define D_NF_2WPP	(0x2000)
#define D_NF_PP_RD	(0x0000)
#define D_NF_PP_WR	(0x0100)


// Cmd Flags
// CMD_DMA_Support
#define D_NF_DEFAULT_ACCESS		(0)
#define D_NF_MAIN_SPARE			(1)
#define D_NF_ONLY_SPARE			(2)
#define D_NF_MAIN_META			(3)

#define D_NF_WFM_ENABLE			(1)
#define D_NF_WFM_DISABLE		(0)

#define D_NF_MEMCOPY_ENABLE		(1)
#define D_NF_MEMCOPY_DISABLE	(0)

#define D_NF_COUNT_ENABLE		(1)
#define D_NF_COUNT_DISABLE		(0)

#define D_NF_INT_ENABLE			(1)
#define D_NF_INT_DISABLE		(0)

#define D_NF_INT_BURST_LEN_8	(8)
#define D_NF_INT_BURST_LEN_32	(32)
#define D_NF_INT_BURST_LEN_64	(64)
#define D_NF_INT_BURST_LEN_128	(128)

#define D_MWNF_CMD_FLAFS_TRANS_MODE_BIT		(12)
#define D_MWNF_CMD_FLAFS_WFM_BIT			(11)
#define D_MWNF_CMD_FLAFS_MEMCOPY_BIT		(10)
#define D_MWNF_CMD_FLAFS_COUNT_BIT			(9)
#define D_MWNF_CMD_FLAFS_INT_BIT			(8)

#define D_MWNF_CMDDMA_CH0		(0)
#define D_MWNF_CMDDMA_CH1		(1)
#define D_MWNF_CMDDMA_CH2		(2)
#define D_MWNF_CMDDMA_CH3		(3)

// CMD_DMA_Support
#define	D_MWNF_META_SIZE	(8)

// timing mode
#define D_MWNF_DLL_PHY_CTRL_RST			(0x00030707)
#define D_MWNF_DLL_PHY_CTRL_UPDATE		(0x03030300)
#define D_MWNF_DLL_PHY_CTRL_UPDATE_REQ	(0x02000000)
#define D_MWNF_DLL_PHY_UPDATE_CNT		(0x00000400)

/*----------------------------------------------------------------------*/
/* Macro    															*/
/*----------------------------------------------------------------------*/
									
/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UINT32 block_addr;
	UINT32 page_addr;
} T_MWNF_FLASH_POINTER;

// block address
typedef struct {
	UINT32 log_block;
	UINT32 page;
	UINT32 sector;
} T_MWNF_BLOCK_ADDR;

typedef struct {
	UINT32	INT;
	UINT32	INCR;
	UINT32	RW;
	UINT32	PP;
	UINT32  bank;
	UINT32  block_addr;
	UINT32  page_addr;
	UINT32	mem_addr;
	UINT32	mode;
} T_MWNF_DATA_DMA_CTRL;

// CMD_DMA_Support
typedef struct {
	UINT32	INT;
	UINT32	INCR;
	UINT32	RW;
	UINT32	PP;
	UINT32  bank;
	UINT32  block_addr;
	UINT32  page_addr;
	UINT32	mem_addr;
	UINT32	meta_addr;
	UINT32	mode;
} T_MWNF_CMD_DMA_CTRL;

typedef struct {
	UINT32 RW;
	UINT32 PP;
	UINT32 block_addr;
	UINT32 page_addr;
	volatile UINT32 mem_addr;
} T_CPU_XFER;

//for Command DMA
typedef union {
	USHORT hword;
	struct {
		USHORT burst_length :8;
		USHORT Int          :1;
		USHORT cont         :1;
		USHORT memcpy       :1;
		USHORT WFM          :1;//Wait for Memcopy
		USHORT xfer_mode    :3;//transfer mode 000:default 001:Main 010 Spare Only 011 Main+Meta
		USHORT reserved     :1;
	} bit;
} U_MWNF_CMD_DMA_FLAGS;

typedef union {
	USHORT hword;
	struct {
		USHORT access_err     :1;
		USHORT unsup_cmd      :1;
		USHORT locked_blk     :1;
		USHORT descriptor_err :1;
		USHORT max_err        :6;
		USHORT time_out       :1;
		USHORT                :3;
		USHORT fail           :1;
		USHORT comp           :1;
	} bit;
} U_MWNF_CMD_DMA_STATUS;

typedef union {
	UINT32 word;
	struct {
		UINT32 increment   :1;
		UINT32 condition   :1;
		UINT32 valid       :1;
		UINT32             :1;
		UINT32 sync_int    :1;
		UINT32             :11;
		UINT32 end_value   :8;
		UINT32 start_value :8;
	} bit;
} U_MWNF_CMD_DMA_SYNC_ARG;

typedef struct {
	UINT32 next_ptr_l;
	UINT32 next_ptr_u;
	UINT32 flash_ptr;

// CMD_DMA_Support
	UINT32 RSVD1;

	USHORT cmd_type;
	U_MWNF_CMD_DMA_FLAGS cmd_flags;
	U_MWNF_CMD_DMA_SYNC_ARG sync_arg;
	UINT32 mem_ptr_l;
	UINT32 mem_ptr_u;
	U_MWNF_CMD_DMA_STATUS status;

// CMD_DMA_Support
	USHORT RSVD2;
	UINT32 RSVD3;

	UINT32 sync_flag_ptr_l;
	UINT32 sync_flag_ptr_u;
	UINT32 memcpy_addr_l;
	UINT32 memcpy_addr_u;
	UINT32 meta_addr_l;
	UINT32 meta_addr_u;
} T_MWNF_CMD_DMA_DESCRIPTOR;

// CMD_DMA_Support
typedef struct {
	BYTE sync_data[8];
} T_MWNF_CMD_DMA_SYNC_FLAG;

// CMD_DMA_Support
typedef struct {
	BYTE meta_data[8];
} T_MWNF_CMD_DMA_META_DATA;

typedef struct {
	UCHAR block_type;
	UINT32 lba_bbcnt;
	UCHAR write_counter;
} T_MWNF_SPARE_DATA;

typedef struct {
	INT32 (*init_NF_controller)(VOID);
	INT32 (*data_DMA)(T_MWNF_DATA_DMA_CTRL*, UINT32*);
	INT32 (*erase)(UINT32);
	INT32 (*cerase)(UINT32);
	INT32 (*copy_bk)(T_MWNF_FLASH_POINTER*,T_MWNF_FLASH_POINTER*,UINT32);	
	INT32 (*copy_dma)(T_MWNF_FLASH_POINTER*,T_MWNF_FLASH_POINTER*,UINT32,UINT32*);
	UINT32 (*scan0)(UINT32,USHORT*,UINT32*,USHORT*);
#ifdef CO_USE_XDMAC
	INT32 (*xdmac_copy)(VOID*,VOID*,UINT32);
#endif
	UINT32 (*get_FB)(UINT32);
#ifdef CO_USE_XDMAC
	INT32 (*fill)(VOID* dst, UINT32 size);
#endif
	INT32 (*rd_spare)(ULONG, BYTE, BYTE*);
	INT32 (*wr_spare)(ULONG, BYTE, BYTE*);
	INT32 (*nf_bread)(VOID*, ULONG, ULONG, CHAR*);
	INT32 (*nf_bwrite)(VOID*, ULONG, ULONG, CHAR*);
	INT32 (*set_bb)(UINT32, UINT32*);
	INT32 (*wr_page_meta)(T_MWNF_FLASH_POINTER*, UINT32, UCHAR*, T_MWNF_SPARE_DATA*);
	INT32 (*wr_page)(UINT32, UINT32, UINT32, UCHAR*);
	INT32 (*wr_lba)(UINT32, UCHAR*, T_MWNF_SPARE_DATA*);
	INT32 (*rd_main_spare)(UCHAR*, UINT32, UINT32);
	INT32 (*wr_main_spare)(UCHAR*, UINT32, UINT32);
	INT32 (*start_clk)(VOID);
	VOID (*stop_clk)(VOID);
} T_MWNF_TEST_LFUNC;

typedef struct {
	UCHAR *page;
	UCHAR *oob;
	UCHAR *work1;
	UCHAR *work2;
}  T_MWNF_TEST_BUFFERS;

typedef struct {
	T_MWNF_DEVICE_INFORMATION *dev_info;
	T_MWNF_TEST_LFUNC *lfunc;
	T_MWNF_TEST_BUFFERS *buffers;
	USHORT *L2P;
	USHORT *FBT;
	USHORT *BBT;
	USHORT *BBT_PBA;
	UCHAR *Init_Flg;
	USHORT *bad_blk_num;
	NF_Device_Info *NFDEV;
	FS_Nformat_Info *NFFMT;
	USHORT *ce_num;
	ULONG *param_blk_top;
	ULONG *param_blk_size;
	ULONG *exception_log_top;
	USHORT *meta_mode_flg;
	UCHAR *ecc_cor_info;
	UINT32 *ecc_move_bits;
} T_MWNF_TEST_PF;

typedef struct{
	union io_nf_cmd_reg0 cmd_reg0; //!cmd_reg0
	union io_nf_cmd_reg1 cmd_reg1; //!cmd_reg1
	union io_nf_cmd_reg2 cmd_reg2; //!cmd_reg2
	union io_nf_cmd_reg3 cmd_reg3; //!cmd_reg3
	union io_nf_cmd_status cmd_status; //!cmd_status
	union io_nf_trd_status trd_status; //!trd_status
	union io_nf_ctrl_status ctrl_status; //!ctrl_status
	union io_nf_transfer_cfg_0 transfer_cfg_0;//!transfer_cfg_0
	union io_nf_transfer_cfg_1 transfer_cfg_1;//!transfer_cfg_1
	unsigned long nf_btsq_state;//!NF_BTSQ_STATE
} T_MWNF_COMMAND_INF;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

// CMD_DMA_Support
extern volatile T_MWNF_CMD_DMA_DESCRIPTOR	gNF_CMD_DMA_Descriptor_Tbl1[D_NF_CMD_DMA_DESCRIPTOR_NUM];
extern volatile T_MWNF_CMD_DMA_DESCRIPTOR	gNF_CMD_DMA_Descriptor_Tbl2[D_NF_CMD_DMA_DESCRIPTOR_NUM];
// CMD_DMA_Support
extern volatile T_MWNF_CMD_DMA_SYNC_FLAG 	gNF_CMD_DMA_SyncFlag_Pointer_Tbl[D_NF_CMD_DMA_SYNC_FLAG_NUM];



#endif	// __MWNF_CORE_H


