/**
 * @file		fj_encoder.c
 * @brief		Encoder API.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_encoder.h"
#include "fj_audio.h"
#include "fj_decoder.h"
#include "audio.h"

#include "movie_video.h"
#include "still_basic.h"
#include "mba_if.h"
/******************************************************************************
 Program
******************************************************************************/

FJ_ERR_CODE FJ_VideoEncoder_Open( E_FJ_VIDEO_ENC_ENUM video_enc, UCHAR* video_enc_id )
{
	return fj_videoencoder_open( video_enc, video_enc_id );
}

FJ_ERR_CODE FJ_VideoEncoder_Close( UCHAR video_enc_id )
{
	return fj_videoencoder_close( video_enc_id );
}

FJ_ERR_CODE FJ_VideoEncoder_SetInstanceEnabled( UCHAR video_enc_id, BOOL enabled )
{
	return fj_videoencoder_setinstanceenabled( video_enc_id, enabled );
}

FJ_ERR_CODE FJ_VideoEncoder_Config( UCHAR video_enc_id, UINT32 wConfig, UINT32 wOperation )
{
	return fj_videoencoder_config( video_enc_id, wConfig, wOperation );
}

FJ_ERR_CODE FJ_VideoEncoder_ConfigDynamic( UCHAR video_enc_id, UINT32 wConfig, UINT32 wOperation )
{
	return fj_videoencoder_configdynamic( video_enc_id, wConfig, wOperation );
}

FJ_ERR_CODE FJ_VideoEncoder_InsertIDR( UCHAR video_enc_id, UINT32 wConfig )
{
	return fj_videoencoder_insert_idr( video_enc_id, wConfig );
}

FJ_ERR_CODE FJ_VideoEncoder_Setup( UCHAR video_enc_id )
{
	return fj_videoencoder_setup( video_enc_id );
}

FJ_ERR_CODE FJ_VideoEncoder_Start( UCHAR video_enc_id )
{
	return fj_videoencoder_start( video_enc_id );
}

FJ_ERR_CODE FJ_VideoEncoder_Stop( UCHAR video_enc_id )
{
	return fj_videoencoder_stop( video_enc_id );
}

FJ_ERR_CODE FJ_VideoEncoder_FreeIndex( UCHAR video_enc_id, UINT32 free_vstream_index, BOOL last_free_flg )
{
	return fj_videoencoder_freeindex( video_enc_id, free_vstream_index, last_free_flg );
}

FJ_ERR_CODE FJ_VideoEncoder_CompEnable( UCHAR video_enc_id, BOOL comp_enable )
{
	return fj_videoencoder_compenable( video_enc_id, comp_enable );
}


FJ_ERR_CODE FJ_StillEncoder_Open( E_FJ_STILL_ENC_ENUM still_enc, UCHAR* still_enc_id )
{
	// delete basefw\fj\imgproc\still
	//return fj_stillencoder_open( still_enc, still_enc_id );
	return FJ_ERR_OK;
}

FJ_ERR_CODE FJ_StillEncoder_Close( UCHAR still_enc_id )
{
	// delete basefw\fj\imgproc\still
	//return fj_stillencoder_close( still_enc_id );
	return FJ_ERR_OK;
}

FJ_ERR_CODE	FJ_StillEncoder_SetInstanceEnabled( UCHAR still_enc_id, BOOL enabled )
{
	// delete basefw\fj\imgproc\still
	//return fj_stillencoder_setinstanceenabled( still_enc_id, enabled );
	return FJ_ERR_OK;
}

FJ_ERR_CODE	FJ_FreeJpegBuffer( UCHAR still_enc_id, UINT32 jpg_idx )
{
	// delete basefw\fj\imgproc\still
	//M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_API_CHECK, FJ_MBALOG_TYPE_POINT, "FJ_FreeJpegBuffer" ));
	//return  fj_freejpegbuffer( still_enc_id, jpg_idx );
	return FJ_ERR_OK;
}

FJ_ERR_CODE FJ_AudioEncoder_Open( E_FJ_AUDIO_ENC_ENUM audio_enc,  UCHAR * audio_enc_id )
{
	return fj_audio_encoder_open(audio_enc, audio_enc_id);
}

FJ_ERR_CODE FJ_AudioEncoder_Close( UCHAR audio_enc_id )
{
	return fj_audio_encoder_close(audio_enc_id);
}

FJ_ERR_CODE FJ_AudioEncoder_FreeIndex( UCHAR audio_enc_id, UINT32 free_astream_index, BOOL last_free_flg )
{
	return fj_audio_encoder_free_index(audio_enc_id, free_astream_index, last_free_flg);
}

FJ_ERR_CODE FJ_AudioEncoder_Config( UCHAR audio_enc_id, UINT32 wConfig, UINT32 * wOperation )
{
	return fj_audio_encoder_config(audio_enc_id, wConfig, wOperation);
}

FJ_ERR_CODE FJ_AudioEncoder_Setup( UCHAR audio_enc_id )
{
	return fj_audio_encoder_setup(audio_enc_id);
}

FJ_ERR_CODE FJ_AudioEncoder_Start( UCHAR audio_enc_id )
{
	return fj_audio_encoder_start(audio_enc_id);
}

FJ_ERR_CODE FJ_AudioEncoder_Stop( UCHAR audio_enc_id )
{
	return fj_audio_encoder_stop(audio_enc_id);
}

FJ_ERR_CODE FJ_AudioEncoder_StartMute( UCHAR audio_enc_id )
{
	return fj_audio_encoder_startmute(audio_enc_id);
}

FJ_ERR_CODE FJ_AudioEncoder_StopMute( UCHAR audio_enc_id )
{
	return fj_audio_encoder_stopmute(audio_enc_id);
}
