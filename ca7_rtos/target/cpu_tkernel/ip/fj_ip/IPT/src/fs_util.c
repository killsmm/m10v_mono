/**
 * @file		fs_util.c
 * @brief		
 * @author		
 * @since		2016/6/9
 * @date		
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT Inc. 2016</I></B>
 */

#include "../include/fs_util.h"

#include "fj_filesystem.h"
#include "fj_debug.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Union	    														*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Private Function														*/
/*----------------------------------------------------------------------*/
static ULONG fs_util_get_filebytes( CHAR* const filepath )
{
	FJ_ERR_CODE ercd;
	FJ_FS_STAT	sta;

	ercd = FJ_Fs_Stat( filepath, &sta );
	if( ercd != FJ_ERR_OK ) {
		return 0UL;
	}

	return sta.size;
}

/*----------------------------------------------------------------------*/
/* Public Function														*/
/*----------------------------------------------------------------------*/

/**
 *  File Load
 */
INT32 FS_Util_Load_File( CHAR* filepath, ULONG dst_addr, ULONG bytes, ULONG* length )
{
	ULONG		fid;
	INT32		errcd = 0;
	ULONG		read_size;
	ULONG		request_bytes;

	if ( bytes == 0 ) {
		request_bytes = fs_util_get_filebytes( filepath );
	}
	else {
		request_bytes = bytes;
	}

	if( request_bytes == 0 ) {
		FJ_Debug_Print_Err( "FS_Util_Load_File() load bytes is zero \"%s\"\n", filepath );
		errcd = -1;
	}

	fid = FJ_Fs_Open( filepath, FJ_FS_OPEN_RDONLY );
	if( fid == 0 ) {
		FJ_Debug_Print_Err( "FS_Util_Load_File() open error. ercd=%lu, \"%s\"\n", fid, filepath );
		errcd = -1;
	}

	read_size = 0;
	FJ_Fs_Read( fid, (VOID*)dst_addr, request_bytes, &read_size );
	if( read_size <= 0 ) {
		FJ_Debug_Print_Err( "FS_Util_Load_File() read error. \"%s\"\n", filepath );
		errcd = -1;
	}

//	FJ_Debug_Print_Info( "FS_Util_Load_File() load addr=0x%x bytes=%u/%u/%u \"%s\"\n", dst_addr, read_size, request_bytes, bytes, filepath );

	FJ_Fs_Close ( fid );

	*length = read_size;

	return errcd;
}

/**
 *  File Save
 */
INT32 FS_Util_Save_File( CHAR* filepath, ULONG src_addr, ULONG bytes )
{
	ULONG		fid;
	ULONG		write_size;
	FJ_ERR_CODE	ercd;
	INT32		ret = 0;

	fid = FJ_Fs_Open ( filepath, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	if( fid == 0 ) {
		FJ_Debug_Print_Err( "FS_Util_Save_File() open error. ercd=%lu, \"%s\"\n", fid, filepath );
		ret  =-1;
	}

	write_size = 0;
	ercd = FJ_Fs_Write ( fid, (VOID*)src_addr, bytes, &write_size );
	if( ercd != FJ_ERR_OK ) {
		FJ_Debug_Print_Err( "FS_Util_Save_File() write error. ercd=%d, \"%s\"\n", ercd, filepath );
		ret  =-1;
	}

//	FJ_Debug_Print_Info( "FS_Util_Save_File() save addr=0x%x bytes=%u \"%s\"\n", src_addr, bytes, filepath );

	FJ_Fs_Close( fid );

	return ret;
}
