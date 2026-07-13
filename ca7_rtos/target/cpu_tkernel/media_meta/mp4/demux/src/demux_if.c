/**
 * @file		demux_if.c
 * @brief		Demux Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include <stdio.h>

#include "demux_if.h"
#include "demux.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/**
 * @brief		Demuxer initialize
 * @param		UINT32	file_id
 *				T_FJ_DEMUX_HEADER_INFO*  header_info
 * @return		Success/Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_demux_initialize( UINT32 file_id, T_FJ_DEMUX_HEADER_INFO* header_info )
{
	BOOL ret;
	
	ret = BF_Demux_Initialize( file_id, header_info );
	if( ret != TRUE ){
		printf("fj_demux_initialize : BF_Demux_Initialize error.\n");
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_demux_videosetup( UINT32 file_id, ULLONG start_pts, E_FJ_DEMUX_DIRECTION direction, E_FJ_DEMUX_SPEED speed )
{
	BOOL ret;
	
	ret = BF_Demux_VideoSetup( file_id, start_pts, direction, speed );
	if( ret != TRUE ){
		printf("fj_demux_videosetup : BF_Demux_VideoSetup error.\n");
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_demux_audiosetup( UINT32 file_id, ULLONG start_pts )
{
	BOOL ret;
	
	ret = BF_Demux_AudioSetup( file_id, start_pts );
	if( ret != TRUE ){
		printf("fj_demux_audiosetup : BF_Demux_AudioSetup error.\n");
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_demux_freeatomsizeget( UINT32 file_id, UINT32* data_size )
{
	BOOL ret;
	
	ret = BF_Demux_FreeAtomSizeGet( file_id, data_size );
	if( ret != TRUE ){
		printf("fj_demux_freeatomsizeget : BF_Demux_FreeAtomSizeGet error.\n");
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_demux_freeatomget( UINT32 file_id, UINT32 data_address )
{
	BOOL ret;
	
	ret = BF_Demux_FreeAtomGet( file_id, data_address );
	if( ret != TRUE ){
		printf("fj_demux_freeatomget : BF_Demux_FreeAtomGet error.\n");
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_demux_streamread( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32 read_stream_num )
{
	BOOL ret;
	
	ret = BF_Demux_StreamRead( file_id, codec, read_stream_num );
	if( ret != TRUE ){
//		printf("fj_demux_streamread : BF_Demux_StreamRead error.\n");
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_demux_streamreadsync( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32 read_stream_num, T_FJ_DEMUX_STREAM_PARAM* stream_param )
{
	BOOL ret;
	
	ret = BF_Demux_StreamReadSync( file_id, codec, read_stream_num, stream_param );
	if( ret != TRUE ){
		printf("fj_demux_streamreadsync : BF_Demux_StreamReadSync error.\n");
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_demux_finalize( UINT32 file_id )
{
	BOOL ret;
	
	ret = BF_Demux_Finalize( file_id );
	if( ret != TRUE ){
		printf("fj_demux_finalize : BF_Demux_Finalize error.\n");
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}
