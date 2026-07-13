/**
 * @file		demux.h
 * @brief		Demux Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _DEMUX_H
#define _DEMUX_H

#include "demux_if.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure														    */
/*----------------------------------------------------------------------*/
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
extern	BOOL	BF_Demux_Initialize( UINT32 file_id, T_FJ_DEMUX_HEADER_INFO* header_info );
extern	BOOL	BF_Demux_VideoSetup( UINT32 file_id, ULLONG start_pts, E_FJ_DEMUX_DIRECTION direction, E_FJ_DEMUX_SPEED speed );
extern	BOOL	BF_Demux_AudioSetup( UINT32 file_id, ULLONG start_pts );
extern	BOOL	BF_Demux_FreeAtomSizeGet( UINT32 file_id, UINT32* data_size );
extern	BOOL	BF_Demux_FreeAtomGet( UINT32 file_id, UINT32 data_address );
extern	BOOL	BF_Demux_StreamRead( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32 read_stream_num );
extern	BOOL	BF_Demux_StreamReadSync( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32 read_stream_num, T_FJ_DEMUX_STREAM_PARAM* stream_param );
extern	BOOL	BF_Demux_Finalize( UINT32 file_id );
extern	VOID	BF_Demux_Controller( INT32 stacd );

#endif	// _DEMUX_H
