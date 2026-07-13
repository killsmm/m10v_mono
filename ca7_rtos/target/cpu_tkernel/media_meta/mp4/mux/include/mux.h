/**
 * @file		mux.h
 * @brief		Mux Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _MUX_H_
#define _MUX_H_

#include "fj_mux.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_BF_MUX_OFF					(0)
#define D_BF_MUX_ON						(1)

//Audio
/* Audio Sampling Size */
#define D_BF_MUX_AUDIO_SR_8				( 8000)
#define D_BF_MUX_AUDIO_SR_11025			(11025)
#define D_BF_MUX_AUDIO_SR_12			(12000)
#define D_BF_MUX_AUDIO_SR_16			(16000)
#define D_BF_MUX_AUDIO_SR_225			(22050)
#define D_BF_MUX_AUDIO_SR_24			(24000)
#define D_BF_MUX_AUDIO_SR_32			(32000)
#define D_BF_MUX_AUDIO_SR_441			(44100)
#define D_BF_MUX_AUDIO_SR_48			(48000)

/* Audio Mono/Streao */
#define D_BF_MUX_AUDIO_CH_MONO			(1)
#define D_BF_MUX_AUDIO_CH_STEREO		(2)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
extern	INT32	BF_Mux_Initialize( T_FJ_MUX_CONFIG* config );
extern	INT32	BF_Mux_FixedAtomSetting( UINT32 stream_id, T_FJ_MUX_H264_SPS_PPS_INFO* sps_pps_info, T_FJ_MUX_H265_PS_INFO* ps_info );
extern	INT32	BF_Mux_VideoAddSample( UINT32 stream_id, UCHAR* sample_address, ULONG sample_size, ULONG ct_offset, UINT32 frame_type, UCHAR end_chunk_flg, UCHAR end_flg );
extern	INT32	BF_Mux_AudioAddSample( UCHAR stream_id, ULONG area_index, BYTE* sample_address, ULONG sample_size, ULONG duration );
extern	INT32	BF_Mux_AddFreeAtom( UINT32 stream_id, UCHAR* data_address, ULONG data_size );
extern	INT32	BF_Mux_Finalize( UINT32 stream_id );
extern	INT32	BF_Mux_Atom_Creat(UINT32 stream_id, INT32 file_id);
extern	INT32	BF_Mux_Movie_File_Old_Set(UINT32 stream_id);
extern	INT32	BF_Mux_Close_File(INT32 stream_id);
// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#ifdef CO_WIFI_EMMC_SAVE
extern	VOID BF_Mux_WIFI_Out_EMMC( BOOL enable );
#endif
// --- REMOVE_WIFI_EMMC_SAVE END ---

#endif	// _MUX_H_
