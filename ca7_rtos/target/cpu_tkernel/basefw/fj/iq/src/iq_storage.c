/**
 * @file		iq_storage.c
 * @brief		Image Quality storage access API (from file or memory).
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_filesystem.h"

#include "iq_storage.h"

#include "debug.h"

#include "ddim_typedef.h"

#include <string.h>


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
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/

static FJ_ERR_CODE iqsub_read_file( const ULONG fileId, BYTE* const buf_addr, ULONG* const buf_len )
{
	FJ_ERR_CODE fj_ercd;
	ULONG read_bytes;

	fj_ercd = FJ_Fs_Read( fileId, (UINT32*)buf_addr, *buf_len, &read_bytes );
	*buf_len = read_bytes;
	if(( fj_ercd != FJ_ERR_OK ) && ( fj_ercd != FJ_ERR_READ_EOF)) {
#ifdef CO_BF_IQ_ENABLE_ERRMSG
		BF_Debug_Print_Error(( "iqsub_read_file(): file read NG\n" ));
#endif
		return FJ_ERR_IQ_FILE_FMT_NG;
	}

	return FJ_ERR_OK;
}


static FJ_ERR_CODE iqsub_read_sdram( T_BF_STORAGE_INFO_SDRAM* const src_sdram, BYTE* const buf_addr, ULONG* const buf_len )
{
	// Check remain bytes.
	if( (src_sdram->bytes - src_sdram->cur_pos) < (*buf_len) ) {
		*buf_len = 0;
		return FJ_ERR_IQ_FILE_FMT_NG;
	}

	memcpy( buf_addr, &src_sdram->top_addr[src_sdram->cur_pos], *buf_len );
	src_sdram->cur_pos += *buf_len;

	return FJ_ERR_OK;
}


static FJ_ERR_CODE iqsub_write_file( const ULONG fileId, const BYTE* const buf_addr, ULONG const buf_len )
{
	FJ_ERR_CODE fj_ercd;
	ULONG write_byte;

	fj_ercd = FJ_Fs_Write( fileId, (UINT32*)buf_addr, buf_len, &write_byte );
	if( fj_ercd != FJ_ERR_OK ) {
#ifdef CO_BF_IQ_ENABLE_ERRMSG
		BF_Debug_Print_Error(( "iqsub_read_file(): file read NG\n" ));
#endif
		return FJ_ERR_IQ_FILE_FMT_NG;
	}

	return FJ_ERR_OK;
}


static FJ_ERR_CODE iqsub_write_sdram( T_BF_STORAGE_INFO_SDRAM* const src_sdram, const BYTE* const buf_addr, const ULONG buf_len )
{
	// Check remain bytes.
	if( (src_sdram->bytes - src_sdram->cur_pos) < buf_len ) {
		return FJ_ERR_IQ_FILE_FMT_NG;
	}

	memcpy( &src_sdram->top_addr[src_sdram->cur_pos], buf_addr, buf_len );
	src_sdram->cur_pos += buf_len;

	return FJ_ERR_OK;
}


static FJ_ERR_CODE iqsub_lseek_file( const ULONG fileId, const INT64 iOffset, const USHORT whence, INT64* const piLength )
{
	FJ_ERR_CODE fj_ercd;

	fj_ercd = FJ_Fs_Lseek64( fileId, iOffset, whence, piLength );
	if( fj_ercd != FJ_ERR_OK ) {
#ifdef CO_BF_IQ_ENABLE_ERRMSG
		BF_Debug_Print_Error(( "%s(): file lseek NG\n", __func__ ));
#endif
		return FJ_ERR_IQ_FILE_FMT_NG;
	}

	return FJ_ERR_OK;
}


static FJ_ERR_CODE iqsub_lseek_sdram( T_BF_STORAGE_INFO_SDRAM* const src_sdram, const INT64 iOffset, const USHORT whence, INT64* const piLength )
{
	INT64	cur_pos;

	switch( whence ) {
		case 0:	// from file top
			src_sdram->cur_pos = 0;
			break;

		case 1: // from current
			// do nothing.
			break;

		case 2: // from file end
			src_sdram->cur_pos = src_sdram->bytes;
			break;

		default:
#ifdef CO_BF_IQ_ENABLE_ERRMSG
			BF_Debug_Print_Error(( "%s(): sdram lseek NG\n", __func__ ));
#endif
			return FJ_ERR_IQ_FILE_FMT_NG;
	}


	cur_pos = ((INT64)src_sdram->cur_pos) + iOffset;
	if( cur_pos < 0ull ) {
		return FJ_ERR_IQ_FILE_FMT_NG;
	}
	if( src_sdram->bytes < cur_pos ) {
		return FJ_ERR_IQ_FILE_FMT_NG;
	}
	src_sdram->cur_pos = cur_pos;

	return FJ_ERR_OK;
}


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

FJ_ERR_CODE BF_IQ_Storage_ReadOpen_File( T_BF_STORAGE_INFO* const strg_info, const CHAR* const file_path )
{
	memset( strg_info, '\0', sizeof(*strg_info) );

	strg_info->src.file.file_id = FJ_Fs_Open( (CHAR*)file_path, FJ_FS_OPEN_RDONLY );
	if( strg_info->src.file.file_id == 0 ) {
#ifdef CO_BF_IQ_ENABLE_ERRMSG
		BF_Debug_Print_Error(( "BF_IQ_Storage_Open_File(): file open NG\n" ));
#endif
		return FJ_ERR_IQ_FILE_OPEN_NG;
	}

	strg_info->type = E_BF_IQ_STORAGE_TYPE_FILE;
	strg_info->src.file.file_path = file_path;

	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_IQ_Storage_ReadOpen_Sdram( T_BF_STORAGE_INFO* const strg_info, const BYTE* const top_addr, const ULONG bytes )
{
	memset( strg_info, '\0', sizeof(*strg_info) );

	strg_info->type = E_BF_IQ_STORAGE_TYPE_SDRAM;
	strg_info->src.sdram.top_addr = (BYTE*)top_addr;
	strg_info->src.sdram.bytes = bytes;
	strg_info->src.sdram.cur_pos = 0;

	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_IQ_Storage_WriteOpen_File( T_BF_STORAGE_INFO* const strg_info, const CHAR* const file_path )
{
	memset( strg_info, '\0', sizeof(*strg_info) );

	strg_info->src.file.file_id = FJ_Fs_Open( (CHAR*)file_path, FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC );
	if( strg_info->src.file.file_id == 0 ) {
#ifdef CO_BF_IQ_ENABLE_ERRMSG
		BF_Debug_Print_Error(( "BF_IQ_Storage_Open_File(): file open NG\n" ));
#endif
		return FJ_ERR_IQ_FILE_OPEN_NG;
	}

	strg_info->type = E_BF_IQ_STORAGE_TYPE_FILE;
	strg_info->src.file.file_path = file_path;

	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_IQ_Storage_WriteOpen_Sdram( T_BF_STORAGE_INFO* const strg_info, BYTE* const top_addr, const ULONG maxbytes )
{
	memset( strg_info, '\0', sizeof(*strg_info) );

	strg_info->type = E_BF_IQ_STORAGE_TYPE_SDRAM;
	strg_info->src.sdram.top_addr = top_addr;
	strg_info->src.sdram.bytes = maxbytes;
	strg_info->src.sdram.cur_pos = 0;

	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_IQ_Storage_Read( T_BF_STORAGE_INFO* const strg_info, BYTE* const buf_addr, ULONG* const read_bytes )
{
	switch( strg_info->type ) {
		case E_BF_IQ_STORAGE_TYPE_FILE:
			return iqsub_read_file( strg_info->src.file.file_id, buf_addr, read_bytes );
		case E_BF_IQ_STORAGE_TYPE_SDRAM:
			return iqsub_read_sdram( &strg_info->src.sdram, buf_addr, read_bytes );
		default:
#ifdef CO_BF_IQ_ENABLE_ERRMSG
			BF_Debug_Print_Error(( "iqsub_read_src(): src read NG\n" ));
#endif
			return FJ_ERR_IQ_FILE_FMT_NG;
	}
}


FJ_ERR_CODE BF_IQ_Storage_Write( T_BF_STORAGE_INFO* const strg_info, const BYTE* const buf_addr, const ULONG write_bytes )
{
	switch( strg_info->type ) {
		case E_BF_IQ_STORAGE_TYPE_FILE:
			return iqsub_write_file( strg_info->src.file.file_id, buf_addr, write_bytes );
		case E_BF_IQ_STORAGE_TYPE_SDRAM:
			return iqsub_write_sdram( &strg_info->src.sdram, buf_addr, write_bytes );
		default:
#ifdef CO_BF_IQ_ENABLE_ERRMSG
			BF_Debug_Print_Error(( "iqsub_read_src(): src read NG\n" ));
#endif
			return FJ_ERR_IQ_FILE_FMT_NG;
	}
}


FJ_ERR_CODE BF_IQ_Storage_Lseek( T_BF_STORAGE_INFO* const strg_info, const INT64 iOffset, const USHORT whence, INT64* const piLength )
{
	switch( strg_info->type ) {
		case E_BF_IQ_STORAGE_TYPE_FILE:
			return iqsub_lseek_file( strg_info->src.file.file_id, iOffset, whence, piLength );
		case E_BF_IQ_STORAGE_TYPE_SDRAM:
			return iqsub_lseek_sdram( &strg_info->src.sdram, iOffset, whence, piLength );
		default:
#ifdef CO_BF_IQ_ENABLE_ERRMSG
			BF_Debug_Print_Error(( "iqsub_read_src(): src read NG\n" ));
#endif
			return FJ_ERR_IQ_FILE_FMT_NG;
	}
}


FJ_ERR_CODE BF_IQ_Storage_Get_WriteBytesSdram( T_BF_STORAGE_INFO* const strg_info, ULONG* const write_bytes )
{
	if( strg_info->type != E_BF_IQ_STORAGE_TYPE_SDRAM ) {
		return FJ_ERR_NG_INPUT_PARAM;
	}

	*write_bytes = strg_info->src.sdram.cur_pos;

	return FJ_ERR_OK;
}


VOID BF_IQ_Storage_Close( T_BF_STORAGE_INFO* const strg_info )
{
	switch( strg_info->type ) {
		case E_BF_IQ_STORAGE_TYPE_FILE:
			FJ_Fs_Close( strg_info->src.file.file_id );
			strg_info->type = E_BF_IQ_STORAGE_TYPE_NONE;
			return;
		case E_BF_IQ_STORAGE_TYPE_SDRAM:
			strg_info->type = E_BF_IQ_STORAGE_TYPE_NONE;
			return;
		default:
#ifdef CO_BF_IQ_ENABLE_ERRMSG
			BF_Debug_Print_Error(( "iqsub_close_src(): src close NG\n" ));
#endif
			return;
	}
}


FJ_ERR_CODE BF_IQ_Storage_Rmove_File( const CHAR* const pc_from_file_path )
{
	return FJ_Fs_Remove( (CHAR*)pc_from_file_path );
}


FJ_ERR_CODE BF_IQ_Storage_Get_FileSize( ULLONG* const file_size, const CHAR* const file_path )
{
	FJ_ERR_CODE		fj_ercd;
	FJ_FS_STAT_EX	stat_ex;

	fj_ercd = FJ_Fs_StatEx( (CHAR*)file_path, &stat_ex );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	*file_size = ((ULLONG)stat_ex.size_ms) * 0x100000000ull + ((ULLONG)stat_ex.size_ls);

	return FJ_ERR_OK;
}

