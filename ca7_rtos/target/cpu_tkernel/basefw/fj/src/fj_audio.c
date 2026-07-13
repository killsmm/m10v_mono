/**
 * @file		fj_audio.c
 * @brief		Audio function API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/******************************************************************************
 Include
******************************************************************************/
#include "fj_audio.h"
#include "fj_encoder.h"
#include "fj_decoder.h"
#include "audio.h"

/******************************************************************************
 Program
******************************************************************************/

FJ_ERR_CODE FJ_AudioSRC_Open( E_FJ_AUDIO_SRC_ENUM audio_src, UCHAR * audio_src_id )
{
	return fj_audio_src_open(audio_src, audio_src_id);
}

FJ_ERR_CODE FJ_AudioSRC_Close( UCHAR audio_src_id )
{
	return fj_audio_src_close(audio_src_id);
}

FJ_ERR_CODE FJ_AudioSRC_Connect( UCHAR audio_src_id, UCHAR audio_capture_id )
{
	return fj_audio_src_connect(audio_src_id, audio_capture_id);
}

FJ_ERR_CODE FJ_AudioSRC_Disconnect( UCHAR audio_src_id, UCHAR audio_capture_id )
{
	return fj_audio_src_disconnect(audio_src_id, audio_capture_id);
}

FJ_ERR_CODE FJ_AudioCapture_Open( E_FJ_AUDIO_CAPTURE_ENUM audio_cap,  UCHAR * audio_capture_id )
{
	return fj_audio_capture_open(audio_cap, audio_capture_id);
}

FJ_ERR_CODE FJ_AudioCapture_Close( UCHAR audio_capture_id )
{
	return fj_audio_capture_close(audio_capture_id);
}

FJ_ERR_CODE FJ_AudioCapture_ConnectOut( UCHAR audio_capture_id, UCHAR audio_out_id )
{
	return fj_audio_capture_connect_out(audio_capture_id, audio_out_id);
}

FJ_ERR_CODE FJ_AudioCapture_DisconnectOut( UCHAR audio_capture_id, UCHAR audio_out_id )
{
	return fj_audio_capture_disconnect_out(audio_capture_id, audio_out_id);
}

FJ_ERR_CODE FJ_AudioCapture_ConnectEnc( UCHAR audio_capture_id, UCHAR audio_enc_id )
{
	return fj_audio_capture_connect_enc(audio_capture_id, audio_enc_id);
}

FJ_ERR_CODE FJ_AudioCapture_DisconnectEnc( UCHAR audio_capture_id, UCHAR audio_enc_id )
{
	return fj_audio_capture_disconnect_enc(audio_capture_id, audio_enc_id);
}

FJ_ERR_CODE FJ_AudioCapture_Config( UCHAR audio_capture_id, T_FJ_AUDIO_CAPTURE_CFG * audio_capture_config )
{
	return fj_audio_capture_config(audio_capture_id, audio_capture_config);
}

FJ_ERR_CODE FJ_AudioCapture_SampleConfig( UCHAR audio_capture_id, T_FJ_AUDIO_CAP_SMPL_CFG * audio_cap_smpl_config)
{
	return fj_audio_capture_sample_config(audio_capture_id, audio_cap_smpl_config);
}

FJ_ERR_CODE FJ_AudioCapture_Start( UCHAR audio_capture_id )
{
	return fj_audio_capture_start(audio_capture_id);
}

FJ_ERR_CODE FJ_AudioCapture_Stop( UCHAR audio_capture_id )
{
	return fj_audio_capture_stop(audio_capture_id);
}

FJ_ERR_CODE FJ_AudioCapture_EnablePlugIn1( UCHAR audio_capture_id, UCHAR enable )
{
	return fj_audio_capture_enable_plugin1(audio_capture_id, enable);
}

FJ_ERR_CODE FJ_AudioCapture_CompletePlugIn1( UCHAR audio_capture_id, UCHAR free_area_index )
{
	return fj_audio_capture_complete_plugin1(audio_capture_id, free_area_index);
}

FJ_ERR_CODE FJ_AudioOut_Open( E_FJ_AUDIO_OUT_ENUM audio_out,  UCHAR * audio_out_id )
{
	return fj_audio_out_open(audio_out, audio_out_id);
}

FJ_ERR_CODE FJ_AudioOut_Close( UCHAR audio_out_id )
{
	return fj_audio_out_close(audio_out_id);
}

FJ_ERR_CODE FJ_AudioOut_ForceClose( UCHAR audio_out_id )
{
	return fj_audio_out_forceclose(audio_out_id);
}

FJ_ERR_CODE FJ_AudioOut_Config( UCHAR audio_out_id, T_FJ_AUDIO_OUT_CFG * audio_out_config )
{
	return fj_audio_out_config(audio_out_id, audio_out_config);
}

FJ_ERR_CODE FJ_AudioOut_DirectLoadWav( UINT32 addr, ULONG size, UINT32 * wav_id )
{
	return fj_audio_out_direct_load_wav(addr, size, wav_id);
}

FJ_ERR_CODE FJ_AudioOut_DirectUnloadWav( UINT32 wav_id )
{
	return fj_audio_out_direct_unload_wav(wav_id);
}

FJ_ERR_CODE FJ_AudioOut_DirectPlayWav( UCHAR audio_out_id, UINT32 wav_id )
{
	return fj_audio_out_direct_play_wav(audio_out_id, wav_id);
}

FJ_ERR_CODE FJ_AudioOut_DirectStopWav( UCHAR audio_out_id, UINT32 wav_id )
{
	return fj_audio_out_direct_stop_wav(audio_out_id, wav_id);
}

FJ_ERR_CODE FJ_AudioOut_ControlWav( UCHAR audio_out_id, E_FJ_AUDIO_WAV_CTL control )
{
	return fj_audio_out_direct_control_wav(audio_out_id, control);
}
