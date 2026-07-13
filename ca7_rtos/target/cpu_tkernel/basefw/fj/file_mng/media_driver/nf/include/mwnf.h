/**
 * @file		mwnf.h
 * @brief		Nand Flash driver header file
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef __MWNF_H
#define __MWNF_H

#include "ddim_typedef.h"
#include "mw_user_custom.h"
/**
@addtogroup mwnf
@{
*/
/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UINT32 features;
	UINT32 optional_cmd;
	UINT32 timing_mode;
	UINT32 pgm_cache_timing_mode;
	UINT32 no_of_luns;
	UINT32 no_of_blocks_per_lun;
	UINT32 reserved;
} T_MD_NF_ONFI_INFORMATION;

typedef struct {
	UINT32 MID;
	UINT32 DID;
	UINT32 compliance;
	UINT32 main_area_size;		//size of main area in NF device.
	UINT32 spare_area_size;		//size of spare area in NF device.
	UINT32 meta_area_size;		//size of meta_area_in spare area.
	UINT32 PP;					//pages per block
	UINT32 device_size;
	UINT32 M_bit;				//Block Address Mapping bit.
	UINT32 sectors_per_blk;		
	UINT32 sectors_per_page;
	UINT32 ecc_capability;
	UINT32 ecc_check_bits;
	UINT32 blk_type_offset;
	UINT32 log_addr_offset1;
	UINT32 log_addr_offset2;
	T_MD_NF_ONFI_INFORMATION onfi;
} T_MWNF_DEVICE_INFORMATION;

typedef struct {
	UINT32 phy_blk_num;
	UINT32 page_per_blk;
	UINT32 sec_per_page;
	UINT32 page_size;
	UCHAR  device_kind;
	UINT32 bad_blk_num;
	UINT32 physical_free_size;
	UINT32 logical_free_size;
} T_NF_DEV_INFO;

typedef struct {
	UINT32 totalSector;
	UINT32 bootSector;
	UINT32 rootSector;
	UINT32 clusPerSec;
	UINT32 fatBits;
} T_NF_Fatsector_Info;

typedef struct
{
	UINT32 acc_clks;
	UINT32 rdwr_en_lo_cnt;
	UINT32 rdwr_en_hi_cnt;
	UINT32 max_rd_delay;
	UINT32 cs_setup_cnt;
	UINT32 twhr2_and_we_2_re;
    UINT32 re_2_re;
	UINT32 tcwaw_and_addr_2_data;
	UINT32 re_2_we;
	UINT32 ecc_enable;
	UINT32 cache_read_enable;
	UINT32 ecc_correction;
} T_NF_REG_INIT_INFO;

typedef struct {
	UINT32 partType;
	UINT32 startLBA;
	UINT32 partSize;
} T_NF_MBR_INFO;

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	CO_META_ON

//return value
#define	D_MWNF_OK						(0)		//Success
#define D_MWNF_ARG_ERR					(-1)	//argument error.
#define D_MWNF_SYSTEMCALL_ERR			(-2)	//system call error.
#define D_MWNF_UNSUP_CMD_ERR			(-3)	//unsuported command has issue.
#define D_MWNF_ERASE_FAIL				(-4)	//erase fail.
#define D_MWNF_PROGRAM_FAIL				(-5)	//program fail.
#define D_MWNF_TIMEOUT_ERR				(-6)	//watchdog time out.
#define D_MWNF_ECC_UNCORRECTABLE_ERR	(-7)	//uncorrectable ecc error.
#define D_MWNF_UNSUPPORTED_DEVICE		(-8)	//non-support device is connected.
#define D_MWNF_INITIALIZE_ERR			(-9)	//error has occur while initializing NF Controller.
#define D_MWNF_UNINITIALIZE_ERR			(-10)	//NF Controller has not initialized yet.
#define D_MWNF_FORMAT_ERR				(-11)	//error has occur in Format process.
#define D_MWNF_FREE_BLK_ERR				(-12)	//there's no free block.
#define D_MWNF_BAD_BLK_ERR				(-13)	//bad block full error.
#define D_MWNF_XDMAC_ERR				(-14)	//XDMAC error.
#define D_MWNF_MBR_ERR					(-15)	//MBR error.
#define D_MWNF_ALLOCATE_ERR				(-16)	//block is not allocated.
#define D_MWNF_COMMAND_ERR				(-17)	//command status error.

#define D_MWNF_NO_INI_BAD_BLOCK			(0)		//no initial bad block
#define D_MWNF_INI_BAD_BLOCK			(1)		//initial bad block

#define D_MWNF_CODE_AREA_DISABLE		(0)
#define D_MWNF_CODE_AREA_ENABLE			(1)

//for device kind
#define D_MWNF_LARGE_DEVICE (1)
#define D_MWNF_DEVICE_KIND_BIT (1<<1)
#define D_MWNF_DEVICE_SIZE_BIT (1<<2)

// The value of MD[2:0]
#define	D_MWNF_NAND_BOOT_LARGE_3ROW	(0)
#define	D_MWNF_NAND_BOOT_LARGE_2ROW	(1)

//User Custom 
//#define	D_MWNF_FID_NF	(FID_NF)	//Event flag ID
//#define D_MWNF_SID_NF	(SID_NF)	//Semafore ID
extern volatile UINT32	gMWNF_FID_NF;		//Event flag ID
extern volatile UINT32	gMWNF_SID_NF;		//Semafore ID

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern volatile T_MWNF_DEVICE_INFORMATION gNF_dev_inf;

extern volatile NF_Device_Info NFdevInfo[];
extern volatile FS_Nformat_Info NFformatInfo[];

extern volatile unsigned long gNF_Code_Size;
extern volatile unsigned int  gNF_Drive_Num;
extern volatile  unsigned char gMW_Exs_IRAM_NF[];

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_MWNF_I_DRIVE = 0, //A Drive
	E_MWNF_A_DRIVE = 1, //B Drive
	E_MWNF_B_DRIVE = 2, //C Drive
	E_MWNF_C_DRIVE = 3, //D Drive
	E_MWNF_D_DRIVE = 4, //E Drive
	E_MWNF_E_DRIVE = 5, //F Drive
	E_MWNF_F_DRIVE = 6  //G Drive
} E_MWNF_DRIVE;

typedef enum {
	E_MWNF_TIMEOUT_RESET	= 0,
	E_MWNF_TIMEOUT_DMA_W	= 1,
	E_MWNF_TIMEOUT_DMA_R	= 2,
	E_MWNF_TIMEOUT_ERASE	= 3,
	E_MWNF_TIMEOUT_COPYBACK	= 4,
	E_MWNF_TIMEOUT_XDMAC	= 5,
	E_MWNF_TIMEOUT_NF_LP	= 6,
	E_MWNF_TIMEOUT_XDMAC_LP	= 7,
	E_MWNF_TIMEOUT_CMD_DESC	= 8,
	E_MWNF_TIMEOUT_MAX		= 9
} E_MWNF_TIMEOUT;

/*----------------------------------------------------------------------*/
/* Function Prototype Declaration										*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

extern INT32 mwnf_XDAMC_copy(VOID* dst, VOID* src, UINT32 size);


//initializes NandFlash macro.
extern INT32 MWNF_Init(VOID);

//initializes NandFlash connected.
extern INT32 MWNF_BasicInit(UINT32 w_addr, UCHAR bits);

//eraces block in NandFlash.
extern INT32 MWNF_EraseBlk(ULONG phy_addr);

//formats NandFlash
extern INT32 MWNF_Format(VOID);

//formats the Drive specified. 
extern INT32  MWNF_Format_Drive(E_MWNF_DRIVE drive_no);

//Interrupt Handler
extern VOID MWNF_Int_Handler(VOID);

//issues RESET command to NandFlash.
extern INT32 MWNF_CardReset(VOID);

//reads data from NandFlash connected.
extern INT32 MWNF_BRead(VOID *devinfo, ULONG start_sec, ULONG read_size, char *buf_data);

//writes data to NandFlash connected.
extern INT32 MWNF_BWrite(VOID *devinfo, ULONG start_sec, ULONG write_size, char *buf_data);

//get Device Information to NandFlash
extern INT32 MWNF_Get_Dev_Info(T_NF_DEV_INFO* devinfo);

//get parameter block information
extern INT32 MWNF_Get_ParamBlk_Info(ULONG* param_blk_top, ULONG* param_blk_size);

//get exception log information
extern INT32 MWNF_Get_ExceptionLog_Info(ULONG* exception_log_top);

//reads MBR to NandFlash connected.
extern INT32 MWNF_Read_MBR(unsigned char* d_buf);

//reads MBR Information to NandFlash connected.
extern INT32 MWNF_Read_MBR_Info(T_NF_MBR_INFO* mbrinfo, ULONG* par_num);

//writes MBR to NandFlash connected.
extern INT32 MWNF_Write_MBR(VOID);

//check MBR
extern INT32 MWNF_Check_MBR(VOID);

// NF 7PARTITION
//get Number of Drive
extern INT32 MWNF_Get_Drive_Num(VOID);

//check Initial BadBlock
extern INT32 MWNF_Check_Initial_BadBlock(ULONG phy_blk_addr, BYTE* result);

//set register init information
extern INT32 MWNF_Set_Reg_Init(T_NF_REG_INIT_INFO init_info);

//set spare_area_skip_bytes
extern INT32 MWNF_Set_Skip_Bytes(ULONG skip_bytes);

//get spare_area_skip_bytes
extern INT32 MWNF_Get_Skip_Bytes(ULONG* skip_bytes);

//set ecc parameter
extern INT32 MWNF_Set_Ecc_Param(ULONG ecc_enable, ULONG ecc_correction);

//get ecc parameter
extern INT32 MWNF_Get_Ecc_Param(ULONG* ecc_enable, ULONG* ecc_correction);

//set retry count
extern VOID MWNF_Set_Retry_Cnt(UCHAR retry_cnt_read, UCHAR retry_cnt_write);

//get retry count
extern VOID MWNF_Get_Retry_Cnt(UCHAR* retry_cnt_read, UCHAR* retry_cnt_write);

//set timeout
extern VOID MWNF_Set_Timeout(E_MWNF_TIMEOUT type, USHORT timeout);

//get timeout
extern VOID MWNF_Get_Timeout(E_MWNF_TIMEOUT type, USHORT* timeout);

//get mid did
extern VOID MWNF_Get_Get_MID_DID(UINT32* mid, UINT32* did);

//writes data to NandFlash connected for format
extern INT32 MWNF_BWrite_format(VOID *devinfo, UINT32 start_sec, UINT32 write_size, UCHAR *buf_data);

//
//MDF related
//
//reads data from NandFlash connected.
extern INT32 NF_Bread(VOID *devinfo, ULONG start_sec, ULONG read_size, char *buf_data);

//writes data to NandFlash connected.
extern INT32 NF_Bwrite(VOID *devinfo, ULONG start_sec, ULONG write_size, char *buf_data);

//reads data from NandFlash connected.(physical address)
extern INT32 MWNF_Direct_BRead(ULONG start_sec, LONG read_size, char* buf_data);

//reads data from NandFlash connected.(physical address by DataDMA)
extern INT32 MWNF_Direct_BRead_DataDMA(ULONG start_sec, LONG read_size, char* buf_data);

//writes data from NandFlash connected.(physical address)
extern INT32 MWNF_Direct_BWrite(ULONG start_sec, ULONG write_size, char* buf_data);

//reads data from NandFlash connected.(page basis)
extern INT32 MWNF_ReadPages(ULONG phy_blk_addr, ULONG start_page, ULONG page_num, BYTE* d_buf);

//writes data to NandFlash connected.(1 block basis to physical address)
extern INT32 MWNF_WritePages_NoRetry(ULONG phy_blk_addr, ULONG start_page, ULONG page_num, BYTE* d_buf);

//eraces all block in NandFlash.
extern INT32 MWNF_Erase_All_Phy_Block(VOID);

//writes data to NandFlash connected.(physical address)
extern INT32 MWNF_Direct_Write(ULONG start_sec, ULONG write_size, ULONG data_addr);

//
//for debug
//
//get BadBlock information
extern INT32 MWNF_Get_BadBlock_Num(ULONG limit, UCHAR mode, ULONG* num, USHORT* buf);

//break data (logical block address)
extern INT32 MWNF_Break_Data_Log(ULONG log_blk_addr, ULONG break_page, ULONG break_byte, UCHAR bits);

//break data (phygical block address)
extern INT32 MWNF_Break_Data_Phy(ULONG phy_blk_addr, ULONG break_page, ULONG break_byte, UCHAR bits);

//set DMA mode
extern INT32 MWNF_Set_DMA_Mode(UCHAR mode);

//read raw pages
extern INT32 MWNF_Read_Raw_Pages(ULONG phy_blk_addr, ULONG start_page, ULONG page_num, BYTE* d_buf);

//erase bad block table
extern INT32 MWNF_Erase_Bad_Block_Table(VOID);

extern INT32   mwnf_read_spare(ULONG phy_blk_addr, BYTE page, BYTE* d_buf);
#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// __MWNF_H

/*@}*/
