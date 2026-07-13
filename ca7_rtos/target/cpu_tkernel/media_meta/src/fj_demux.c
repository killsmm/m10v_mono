/**
 * @file		fj_demux.c
 * @brief		Demux Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include <stdio.h>

#include "fj_demux.h"
#include "demux_if.h"

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
FJ_ERR_CODE FJ_Demux_Initialize( UINT32 file_id, T_FJ_DEMUX_HEADER_INFO* header_info )
{
	if( file_id == 0 ){
		printf("FJ_Demux_Initialize : Error!! file_id is zero.\n");
		return FJ_ERR_NG;
	}
	if( header_info == NULL ){
		printf("FJ_Demux_Initialize : Error!! header_info is null.\n");
		return FJ_ERR_NG;
	}
	
	return fj_demux_initialize( file_id, header_info );
}


FJ_ERR_CODE FJ_Demux_VideoSetup( UINT32 file_id, ULLONG start_pts, E_FJ_DEMUX_DIRECTION direction, E_FJ_DEMUX_SPEED speed )
{
	return fj_demux_videosetup( file_id, start_pts, direction, speed );
}


FJ_ERR_CODE FJ_Demux_AudioSetup( UINT32 file_id, ULLONG start_pts )
{
	return fj_demux_audiosetup( file_id, start_pts );
}


FJ_ERR_CODE FJ_Demux_FreeAtomSizeGet( UINT32 file_id, UINT32* data_size )
{
	return fj_demux_freeatomsizeget( file_id, data_size );
}


FJ_ERR_CODE FJ_Demux_FreeAtomGet( UINT32 file_id, UINT32 data_address )
{
	return fj_demux_freeatomget( file_id, data_address );
}


FJ_ERR_CODE FJ_Demux_StreamRead( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32 read_stream_num )
{
	return fj_demux_streamread( file_id, codec, read_stream_num );
}


FJ_ERR_CODE FJ_Demux_StreamReadSync( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32 read_stream_num, T_FJ_DEMUX_STREAM_PARAM* stream_param )
{
	return fj_demux_streamreadsync( file_id, codec, read_stream_num, stream_param );
}


FJ_ERR_CODE FJ_Demux_Finalize( UINT32 file_id )
{
	return fj_demux_finalize( file_id );
}

