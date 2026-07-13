/**
 * @file		fj_muxr.c
 * @brief		Mux API.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_mux.h"
#include "mux_if.h"

/******************************************************************************
 Program
******************************************************************************/
FJ_ERR_CODE FJ_Mux_Initialize( T_FJ_MUX_CONFIG* config )
{
	return fj_mux_initialize( config );
}

FJ_ERR_CODE FJ_Mux_Fixed_Atom_Setting( UINT32 stream_id, T_FJ_MUX_H264_SPS_PPS_INFO* sps_pps_info, T_FJ_MUX_H265_PS_INFO* ps_info )
{
	return fj_mux_fixedatomsetting( stream_id, sps_pps_info, ps_info );
}

INT32 FJ_Mux_Video_Add_Sample( UINT32 stream_id, UCHAR* sample_address, ULONG sample_size, ULONG ct_offset, UINT32 frame_type, UCHAR end_chunk_flg, UCHAR end_flg )
{
	return fj_mux_videoaddsample( stream_id, sample_address, sample_size, ct_offset, frame_type, end_chunk_flg, end_flg );
}

FJ_ERR_CODE FJ_Mux_Audio_Add_Sample( UCHAR stream_id, ULONG area_index, BYTE* sample_address, ULONG sample_size, ULONG duration )
{
	return fj_mux_audioaddsample( stream_id, area_index, sample_address, sample_size, duration );
}

FJ_ERR_CODE FJ_Mux_Add_FreeAtom( UINT32 stream_id, UCHAR* data_address, ULONG data_size )
{
	return fj_mux_addfreeatom( stream_id, data_address, data_size );
}

FJ_ERR_CODE FJ_Mux_Finalize( UINT32 stream_id )
{
	return fj_mux_finalize( stream_id );
}

// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#ifdef CO_WIFI_EMMC_SAVE
FJ_ERR_CODE FJ_Mux_WIFI_Out_EMMC( BOOL enable )
{
	return fj_mux_wifi_out_emmc( enable );
}
#endif
// --- REMOVE_WIFI_EMMC_SAVE END ---

FJ_ERR_CODE FJ_Mux_Atom_Creat(UINT32 stream_id , INT32 file_id)
{
	return fj_mux_atom_creat(stream_id , file_id);
}

FJ_ERR_CODE FJ_Mux_Movie_File_Old_Set(UINT32 stream_id)
{
	return fj_mux_movie_file_old_set(stream_id);
}

FJ_ERR_CODE FJ_Mux_Close_File( UINT32 stream_id )
{
	return fj_mux_close_file( stream_id );
}
