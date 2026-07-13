/**
 * @file		mux_if.h
 * @brief		Mux Interface.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _MUX_IF_H_
#define _MUX_IF_H_

#include "fj_mux.h"

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

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
extern	FJ_ERR_CODE	fj_mux_initialize( T_FJ_MUX_CONFIG* config );
extern	FJ_ERR_CODE	fj_mux_fixedatomsetting( UINT32 stream_id, T_FJ_MUX_H264_SPS_PPS_INFO* sps_pps_info, T_FJ_MUX_H265_PS_INFO* ps_info );
extern	INT32	fj_mux_videoaddsample( UINT32 stream_id, UCHAR* sample_address, ULONG sample_size, ULONG ct_offset, UINT32 frame_type, UCHAR end_chunk_flg, UCHAR end_flg );
extern	FJ_ERR_CODE	fj_mux_audioaddsample( UCHAR stream_id, ULONG area_index, BYTE* sample_address, ULONG sample_size, ULONG duration );
extern	FJ_ERR_CODE	fj_mux_addfreeatom( UINT32 stream_id, UCHAR* data_address, ULONG data_size );
extern	FJ_ERR_CODE	fj_mux_finalize( UINT32 stream_id );
extern	FJ_ERR_CODE fj_mux_atom_creat(UINT32 stream_id, INT32 file_id);
extern	FJ_ERR_CODE fj_mux_movie_file_old_set(UINT32 stream_id);
extern	FJ_ERR_CODE fj_mux_close_file( UINT32 stream_id );
// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#ifdef CO_WIFI_EMMC_SAVE
extern	FJ_ERR_CODE fj_mux_wifi_out_emmc( BOOL enable );
#endif
// --- REMOVE_WIFI_EMMC_SAVE END ---
#endif	// _MUX_IF_H_
