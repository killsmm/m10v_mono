/**
 * @file		fj_decoder.c
 * @brief		Decoder API.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_decoder.h"
#include "fj_audio.h"
#include "fj_encoder.h"
#include "audio.h"

#include "movie_video.h"
#include "instance.h"
/******************************************************************************
 Program
******************************************************************************/
FJ_ERR_CODE FJ_VideoDecoder_Open( E_FJ_VIDEO_DEC_ENUM video_dec,  UCHAR* video_decoder_id )
{
	return fj_videodecoder_open( video_dec, video_decoder_id );
}

FJ_ERR_CODE FJ_VideoDecoder_Close( UCHAR video_decoder_id )
{
	return fj_videodecoder_close( video_decoder_id );
}

FJ_ERR_CODE FJ_VideoDecoder_Connect( UCHAR video_decoder_id, UCHAR display_id, BOOL sub_inst)
{
	return fj_videodecoder_connect( video_decoder_id, display_id, sub_inst );
}

FJ_ERR_CODE FJ_VideoDecoder_Disconnect( UCHAR video_decoder_id, UCHAR display_id, BOOL sub_inst )
{
	return fj_videodecoder_disconnect( video_decoder_id, display_id, sub_inst );
}

FJ_ERR_CODE FJ_VideoPlayback_VideoInit( VOID )
{
	return fj_videoplayback_videoinit();
}

FJ_ERR_CODE FJ_VideoPlayback_AudioInit( VOID )
{
	return fj_videoplayback_audioinit();
}

FJ_ERR_CODE FJ_VideoPlayback_SetVideoConfig( T_FJ_MOVIE_PLAY_VIDEO_CONFIG_PARAM* video_config )
{
	return fj_videoplayback_setvideoconfig( video_config );
}

FJ_ERR_CODE FJ_VideoPlayback_SetStartPTS( ULLONG start_pts )
{
	return fj_videoplayback_setstartpts( start_pts );
}

FJ_ERR_CODE FJ_VideoPlayback_SetSpeedDirection( E_FJ_MOVIE_PLAY_SPEED speed, E_FJ_MOVIE_PLAY_DIRECTION direction, BOOL pause )
{
	return fj_videoplayback_setspeeddirection( speed, direction, pause );
}

FJ_ERR_CODE FJ_VideoPlayback_SetVideoStream( E_FJ_VIDEO_DEC_ENUM codec, UINT32 store_num, T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM* video_stream_param )
{
	return fj_videoplayback_setvideostream( codec, store_num, video_stream_param );
}

FJ_ERR_CODE FJ_VideoPlayback_SetAudioStream( UINT32 store_num, T_FJ_MOVIE_PLAY_AUDIO_STREAM_PARAM* audio_stream_param )
{
	return fj_videoplayback_setaudiostream(store_num, audio_stream_param);
}

FJ_ERR_CODE FJ_VideoPlayback_StartVideoDecode( E_FJ_VIDEO_DEC_ENUM codec )
{
	return fj_videoplayback_startvideodecode(codec);
}

FJ_ERR_CODE FJ_VideoPlayback_StartAudioDecode( VOID )
{
	return fj_videoplayback_startaudiodecode();
}

FJ_ERR_CODE FJ_VideoPlayback_StartAudioOutput( VOID )
{
	return fj_videoplayback_startaudiooutput();
}

FJ_ERR_CODE FJ_VideoPlayback_StopAudioOutput( VOID )
{
	return fj_videoplayback_stopaudiooutput();
}

FJ_ERR_CODE FJ_VideoPlayback_StopVideoDecode( E_FJ_VIDEO_DEC_ENUM codec, E_FJ_MOVIE_PLAY_LAST_PICTURE last_picture )
{
	return fj_videoplayback_stopvideodecode( codec, last_picture );
}

FJ_ERR_CODE FJ_VideoPlayback_StopAudioDecode( VOID )
{
	return fj_videoplayback_stopaudiodecode();
}

FJ_ERR_CODE FJ_VideoPlayback_PauseVideo( VOID )
{
	return fj_videoplayback_pausevideo();
}

FJ_ERR_CODE FJ_VideoPlayback_ResumeVideo( VOID )
{
	return fj_videoplayback_resumevideo();
}

FJ_ERR_CODE FJ_VideoPlayback_GetDisplayFrame( T_FJ_MOVIE_PLAY_VIDEO_YUV_INFO* display_yuv_info )
{
	return fj_videoplayback_getdisplayframe( display_yuv_info );
}

FJ_ERR_CODE FJ_VideoPlayback_StartIFrameDecoding( BOOL display, ULLONG req_pts, UINT32 store_num, T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM* video_stream_param, T_FJ_MOVIE_PLAY_VIDEO_YUV_INFO* decode_yuv_info)
{
	return fj_videoplayback_startiframedecoding( display, req_pts, store_num, video_stream_param, decode_yuv_info);
}

FJ_ERR_CODE FJ_VideoPlayback_MemAlloc( E_FJ_MOVIE_PLAY_BUFFER_TYPE buffer_type, UINT32* size, UINT32* address )
{
	return fj_videoplayback_memalloc( buffer_type, size, address );
}

FJ_ERR_CODE FJ_AudioDecoder_Open( E_FJ_AUDIO_DEC_ENUM audio_dec, UCHAR * audio_dec_id )
{
	return fj_audio_decoder_open(audio_dec, audio_dec_id);
}

FJ_ERR_CODE FJ_AudioDecoder_Close( UCHAR audio_dec_id )
{
	return fj_audio_decoder_close(audio_dec_id);
}

FJ_ERR_CODE FJ_AudioDecoder_Connect( UCHAR audio_dec_id, UCHAR audio_out_id )
{
	return fj_audio_decoder_connect(audio_dec_id, audio_out_id);
}

FJ_ERR_CODE FJ_AudioDecoder_Disconnect( UCHAR audio_dec_id, UCHAR audio_out_id )
{
	return fj_audio_decoder_disconnect(audio_dec_id, audio_out_id);
}

FJ_ERR_CODE FJ_AudioDecoder_Config( UCHAR audio_dec_id, T_FJ_AUDIO_DEC_CONFIG_PARAM * audio_dec_config )
{
	return fj_audio_decoder_config(audio_dec_id, audio_dec_config);
}


