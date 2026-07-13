/**
 * @file		demux_if.h
 * @brief		Demux Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _DEMUX_IF_H
#define _DEMUX_IF_H

#include "fj_demux.h"

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
/* Function														  */
/*----------------------------------------------------------------*/
extern	FJ_ERR_CODE	fj_demux_initialize( UINT32 file_id, T_FJ_DEMUX_HEADER_INFO* header_info );
extern	FJ_ERR_CODE	fj_demux_videosetup( UINT32 file_id, ULLONG start_pts, E_FJ_DEMUX_DIRECTION direction, E_FJ_DEMUX_SPEED speed );
extern	FJ_ERR_CODE	fj_demux_audiosetup( UINT32 file_id, ULLONG start_pts );
extern	FJ_ERR_CODE fj_demux_freeatomsizeget( UINT32 file_id, UINT32* data_size );
extern	FJ_ERR_CODE fj_demux_freeatomget( UINT32 file_id, UINT32 data_address );
extern	FJ_ERR_CODE	fj_demux_streamread( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32 read_stream_num );
extern	FJ_ERR_CODE	fj_demux_streamreadsync( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32 read_stream_num, T_FJ_DEMUX_STREAM_PARAM* stream_param );
extern	FJ_ERR_CODE	fj_demux_finalize( UINT32 file_id );

#endif	// _DEMUX_IF_H
