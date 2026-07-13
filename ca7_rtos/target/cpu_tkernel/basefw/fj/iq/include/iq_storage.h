/**
 * @file		iq_storage.h
 * @brief		Image Quality storage access API (from file or memory).
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __IQ_STORAGE_H__
#define __IQ_STORAGE_H__

#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Macro declaration															*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

typedef enum {
	E_BF_IQ_STORAGE_TYPE_NONE = 0,
	E_BF_IQ_STORAGE_TYPE_FILE,
	E_BF_IQ_STORAGE_TYPE_SDRAM,
	E_BF_IQ_STORAGE_TYPE_MAX,
} E_BF_IQ_STORAGE_TYPE;


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

typedef struct {
	const CHAR*					file_path;
	ULONG						file_id;		// FJ_Fs_Open()'s return value.
} T_BF_STORAGE_INFO_FILE;

typedef struct {
	BYTE*						top_addr;
	ULONG						bytes;
	ULONG						cur_pos;
} T_BF_STORAGE_INFO_SDRAM;

typedef struct {
	E_BF_IQ_STORAGE_TYPE			type;
	union {
		T_BF_STORAGE_INFO_FILE		file;
		T_BF_STORAGE_INFO_SDRAM		sdram;
	} src;
} T_BF_STORAGE_INFO;


/*==============================================================================*/
/* Global variable declaration													*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Function declaration															*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


extern FJ_ERR_CODE BF_IQ_Storage_ReadOpen_File( T_BF_STORAGE_INFO* const strg_info, const CHAR* const file_path );
extern FJ_ERR_CODE BF_IQ_Storage_ReadOpen_Sdram( T_BF_STORAGE_INFO* const strg_info, const UCHAR* const top_addr, const ULONG bytes );
extern FJ_ERR_CODE BF_IQ_Storage_WriteOpen_File( T_BF_STORAGE_INFO* const strg_info, const CHAR* const file_path );
extern FJ_ERR_CODE BF_IQ_Storage_WriteOpen_Sdram( T_BF_STORAGE_INFO* const strg_info, BYTE* const top_addr, const ULONG maxbytes );
extern FJ_ERR_CODE BF_IQ_Storage_Read( T_BF_STORAGE_INFO* const strg_info, BYTE* const buf_addr, ULONG* const load_bytes );
extern FJ_ERR_CODE BF_IQ_Storage_Write( T_BF_STORAGE_INFO* const strg_info, const BYTE* const buf_addr, const ULONG write_bytes );
extern FJ_ERR_CODE BF_IQ_Storage_Lseek( T_BF_STORAGE_INFO* const strg_info, const INT64 iOffset, const USHORT whence, INT64* const piLength );
extern FJ_ERR_CODE BF_IQ_Storage_Get_WriteBytesSdram( T_BF_STORAGE_INFO* const strg_info, ULONG* const write_bytes );
extern VOID BF_IQ_Storage_Close( T_BF_STORAGE_INFO* const strg_info );
extern FJ_ERR_CODE BF_IQ_Storage_Rmove_File( const CHAR* const pc_from_file_path );
extern FJ_ERR_CODE BF_IQ_Storage_Get_FileSize( ULLONG* const file_size, const CHAR* const file_path );


#ifdef __cplusplus
}
#endif

#endif /* __IQ_LOAD_DATA_H__ */

