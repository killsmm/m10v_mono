/**
 * @file		vbd.h
 * @brief		Virtual Block Device function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */
#ifndef _VBD_H_
#define _VBD_H_


#include "fs_user_custom.h"
#include "sdram_map_define.h"
#include "nf_custom.h"
// --- REMOVE_ES3_HARDWARE BEGIN ---
#include "emmc_custom.h"
// --- REMOVE_ES3_HARDWARE END --

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* For Virtual Block Device */
#define D_BF_FS_VBD_NANDBLK_GET_SIZE	(0x3000)
#define D_BF_FS_VBD_NANDBLK_READ		(0x3001)
#define D_BF_FS_VBD_NANDBLK_WRITE		(0x3002)

/* Error Code */
#define D_BF_FS_VBD_ERR_OK				(0x0000)
#define D_BF_FS_VBD_ERR_NG				(0x0001)
#define D_BF_FS_VBD_ERR_LAST_ENTRY		(0x0002)
#define D_BF_FS_VBD_ERR_EOF				(0x0003)

/* sector size */
#define D_BF_FS_VBD_SECTOR_SIZE			(512)

/* page size */
#define D_BF_FS_VBD_PAGE_SIZE			(2048)

/* block size */
#define D_BF_FS_VBD_BLOCK_SIZE			(131072)

/* parameter */
#define D_BF_FS_VBD_PAGE_PAR_BLOCK		(D_BF_FS_VBD_BLOCK_SIZE / D_BF_FS_VBD_PAGE_SIZE)
#define D_BF_FS_VBD_SEC_PAR_PAGE		(D_BF_FS_VBD_PAGE_SIZE / D_BF_FS_VBD_SECTOR_SIZE)
#define D_BF_FS_VBD_SEC_PAR_BLOCK		(D_BF_FS_VBD_PAGE_PAR_BLOCK * D_BF_FS_VBD_SEC_PAR_PAGE)

/*---------------------------------------------------------------*/
/* Error Number													 */
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* VBD Info table */
typedef struct {
	FW_INT32 ope_cmd;
	FW_INT32 buffer_num;
	FW_INT32 plane_num;
	FW_INT32 reserved1;
	FW_INT32 reserved2;
	FW_INT32 reserved3;
	FW_INT32 reserved4;
	FW_INT32 reserved5;
} T_FS_VBD_INFO_TABLE_COMM;

/* VBD Table Get Size Info table  */
typedef struct {
	T_FS_VBD_INFO_TABLE_COMM	common_data;
	FW_INT32					sector_size;
	FW_INT32					error_code;
} T_FS_VBD_INFO_TABLE_GET_SIZE;

/* VBD Write Info table  */
typedef struct {
	T_FS_VBD_INFO_TABLE_COMM	common_data;
	FW_INT32					start_sec;
	FW_INT32					write_sec;
	FW_CHAR*					data_addr;
	FW_INT32					error_code;
} T_FS_VBD_INFO_TABLE_WRITE;

/* VND Read Info table  */
typedef struct {
	T_FS_VBD_INFO_TABLE_COMM	common_data;
	FW_INT32					start_sec;
	FW_INT32					read_sec;
	FW_CHAR*					data_addr;
	FW_INT32					error_code;
} T_FS_VBD_INFO_TABLE_READ;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Macro					     ?€?€?€?€						 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Grobal Function											     */
/*---------------------------------------------------------------*/
extern FW_INT32 BF_Vbd_Get_Sector_Size(T_FS_VBD_INFO_TABLE_GET_SIZE* info_table);
extern FW_INT32 BF_Vbd_Sector_Write(T_FS_VBD_INFO_TABLE_WRITE* info_table);
extern FW_INT32 BF_Vbd_Sector_Read(T_FS_VBD_INFO_TABLE_READ* info_table);

#endif	// _VBD_H_
