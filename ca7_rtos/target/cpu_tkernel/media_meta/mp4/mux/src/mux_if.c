/**
 * @file		mux_if.c
 * @brief		Mux Interface.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*----------------------------------------------------------------------*/
/* Include Files														*/
/*----------------------------------------------------------------------*/
#include "mux_if.h"
#include "mux.h"
#include "mp4_common.h"
#include "debug.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
FJ_ERR_CODE fj_mux_initialize( T_FJ_MUX_CONFIG* config )
{
	INT32 ret;
	
	ret = BF_Mux_Initialize( config );
	if( ret != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("fj_mux_initialize : BF_Mux_Initialize error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}


FJ_ERR_CODE fj_mux_fixedatomsetting( UINT32 stream_id, T_FJ_MUX_H264_SPS_PPS_INFO* sps_pps_info, T_FJ_MUX_H265_PS_INFO* ps_info )
{
	INT32 ret;
	
	ret = BF_Mux_FixedAtomSetting( stream_id, sps_pps_info, ps_info );
	if( ret != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("fj_mux_fixedatomsetting : BF_Mux_FixedAtomSetting error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}


INT32 fj_mux_videoaddsample(
		UINT32		stream_id,
		UCHAR*		sample_address,
		ULONG		sample_size,
		ULONG		ct_offset,
		UINT32		frame_type,
		UCHAR		end_chunk_flg,
		UCHAR		end_flg
		)
{
	INT32 ret;
	
	ret = BF_Mux_VideoAddSample( stream_id, sample_address, sample_size, ct_offset, frame_type, end_chunk_flg, end_flg );
	if( ret != MP4_COMMON_OK ){
		if( ret == MP4_COMMON_OK_MAX_SIZE_END ){
//			BF_Debug_Print_Warning(("fj_mux_videoaddsample :  MaxSize.\n"));
			ret = FJ_OK_MAX_SIZE_END;
		}
		else if( ret == MP4_COMMON_OK_MAX_TIME_END ){
			BF_Debug_Print_Warning(("fj_mux_videoaddsample :  MaxTime.\n"));
			ret = FJ_OK_MAX_TIME_END;
		}
		else if(ret == MP4_COMMON_OK_MEDIAFULL_END){
			BF_Debug_Print_Warning(("fj_mux_videoaddsample :  Media Full.\n"));
			ret = FJ_OK_MEDIAFULL_END;
		}
		else{
			BF_Debug_Print_Error(("fj_mux_videoaddsample : BF_Mux_VideoAddSample error.\n"));
			ret = FJ_ERROR_MP4_COMMON_NG;
		}
	}
	else{
		ret = FJ_OK_MP4_COMMON_OK;
	}
	
	return ret;
}


FJ_ERR_CODE fj_mux_audioaddsample( UCHAR stream_id, ULONG area_index, BYTE* sample_address, ULONG sample_size, ULONG duration )
{
	INT32 ret;
	
	ret = BF_Mux_AudioAddSample( stream_id, area_index, sample_address, sample_size, duration );
	if( ret != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("fj_mux_audioaddsample : BF_Mux_AudioAddSample error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}


FJ_ERR_CODE fj_mux_addfreeatom( UINT32 stream_id, UCHAR* data_address, ULONG data_size )
{
	INT32 ret;
	
	ret = BF_Mux_AddFreeAtom( stream_id, data_address, data_size );
	if( ret != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("fj_mux_addfreeatom : BF_Mux_AddFreeAtom error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}


FJ_ERR_CODE fj_mux_finalize( UINT32 stream_id )
{
	INT32 ret;
	
	ret = BF_Mux_Finalize( stream_id );
	if( ret != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("fj_mux_finalize : BF_Mux_Finalize error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}


// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#ifdef CO_WIFI_EMMC_SAVE
FJ_ERR_CODE fj_mux_wifi_out_emmc( BOOL enable )
{
	BF_Mux_WIFI_Out_EMMC( enable );
	
	return FJ_ERR_OK;
}
#endif
// --- REMOVE_WIFI_EMMC_SAVE END ---

FJ_ERR_CODE fj_mux_atom_creat(UINT32 stream_id, INT32 file_id)
{
	INT32 ret;

	ret = BF_Mux_Atom_Creat( stream_id, file_id );
	if( ret != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("fj_mux_atom_creat : BF_Mux_Atom_Creat error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_mux_movie_file_old_set(UINT32 stream_id)
{
	if (BF_Mux_Movie_File_Old_Set(stream_id) == MP4_COMMON_OK) {
		return FJ_ERR_OK;
	}
	else {
		return FJ_ERR_NG;
	}
}

FJ_ERR_CODE fj_mux_close_file( UINT32 stream_id )
{
	INT32 ret;
	
	ret = BF_Mux_Close_File( stream_id );
	if( ret != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("fj_mux_close_file : BF_Mux_Close_File error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}
