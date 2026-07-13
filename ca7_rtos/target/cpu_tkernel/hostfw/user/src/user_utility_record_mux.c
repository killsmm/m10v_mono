/**
 * @file		user_utility.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "user.h"
#include "fj_encoder.h"
#include "movie_video.h"
#include "sdram_map_movie_record.h"
#include "fj_still.h"
#include "fj_dcf.h"
#include "fj_filesystem.h"
#include "still_mov_ctrl.h"
#include "fj_mux.h"
#include "fj_mux_demux_common.h"
#include "audio_encoder.h"
#include "audio_codec_wrapper.h"
#include "mp4_common.h"
#include <stdlib.h>

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

#define D_RS_MUX_FRAME_SIZE_MAX	(PIX_SIZ_4K2K*1.5)

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_FJ_MUX_CONFIG 						g_mux_config;
static T_FJ_MUX_DEMUX_COMMON_MEMORY_CONFIG 	g_mem_config;
static E_FJ_MUX_CH g_mux_ch;

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static INT32 Mux_init( VOID );

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

VOID User_Utility_Record_Mux_Initial( VOID )
{
	g_mux_ch = E_FJ_MUX_CH_0;
	memset( &g_mux_config, 0, sizeof(g_mux_config) );
	memset( &g_mem_config, 0, sizeof(g_mem_config) );

	Sdram_Map_Movie_Record_Initialize(E_SDRAM_MAP_MOVIE_RECORD_PATTERN_0); // can be deleted later?
	
}

INT32 User_Utility_Record_Mux_Start( T_SHARE_AUDIO_REC_CONFIG *audio )
{
	INT32 ret;
	UINT8 mux_cnt;
	T_SHARE_AUDIO_REC_CONFIG *pAudio = NULL;
	BOOL sync = TRUE;
	
	// for low latency
	T_SHARE_MOVIE_REC_CONFIG	movie_cfg;
	BOOL						low_latency = FALSE;
	OS_USER_SYSTIM				start_time;
	OS_USER_SYSTIM				end_time;
	ULONG						period_ms;
	
	if (g_mux_ch == E_FJ_MUX_CH_0) return OK;

	// check low_latency_en
	Get_Share_Movie_Rec_Config(&movie_cfg);
	if( movie_cfg.video[E_SHARE_YUV_OUT_H265].low_latency ||
	    movie_cfg.video[E_SHARE_YUV_OUT_H264_0].low_latency || 
	    movie_cfg.video[E_SHARE_YUV_OUT_H264_1].low_latency
	  ){
		low_latency = TRUE;
		OS_User_Get_Tim(&start_time);
	}
	
	ret = Mux_init();

	// wait H264_Record_Controller/H265_Record_Controller waiting time ( = 15ms )
	if( low_latency ){
		OS_User_Get_Tim(&end_time);
		period_ms = (ULONG)( end_time - start_time );
		if( period_ms < 15 ){
			period_ms = 15 - period_ms;
			MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H265, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "wait %d", (INT32)period_ms ) );
			OS_User_Dly_Tsk( ( period_ms ) );
			MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H265, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "" ) );
		}
		
		sync = low_latency;
	}
	
	pAudio = audio;
	for(mux_cnt = 0 ; mux_cnt < E_FJ_MUX_CH_MAX; mux_cnt++) {
		if(g_mux_config.video[mux_cnt].enable == TRUE) {
			ret = FJ_VideoEncoder_Start( (UCHAR)g_mux_config.video[mux_cnt].stream_id );
			UPRINTF_ERR(ret);
		}
	}
	
	// Encoder Start
	for(mux_cnt = 0 ; mux_cnt < E_FJ_MUX_CH_MAX; mux_cnt++) {
		if(g_mux_config.video[mux_cnt].enable == TRUE) {
			ret = BF_Encoder_Start( (E_VIDEO_ENOCDE_ID)g_mux_config.video[mux_cnt].stream_id, sync );
			UPRINTF_ERR(ret);
		}
	}

	for(mux_cnt = 0 ; mux_cnt < 2; mux_cnt++) {
		if (g_mux_config.audio[mux_cnt].enable == TRUE) {
			User_Utility_Record_Audio_Start(mux_cnt, pAudio+mux_cnt);
		}
	}

	return OK;
}

INT32 User_Utility_Record_Mux_Cache_Start( VOID )
{
	INT32 ret;

	if (g_mux_ch == E_FJ_MUX_CH_0) return OK;

	ret = Mux_init();
	UPRINTF_ERR(ret);

	return OK;
}

T_FJ_MUX_VIDEO_CONFIG* User_Utility_Record_Mux_Get_Config( VOID )
{
	E_FJ_MUX_CH mux_ch = E_FJ_MUX_CH_0;

	mux_ch = g_mux_ch;

	g_mux_ch++;
	if(g_mux_ch >= E_FJ_MUX_CH_MAX)
		g_mux_ch = E_FJ_MUX_CH_0;
	
	return &g_mux_config.video[mux_ch];
}

VOID User_Utility_Record_Mux_Audio_Force_End( UINT32 stream_id )
{
	UINT32 mux_ch = E_FJ_MUX_CH_0;
	UCHAR aud_id = 0;

	for(mux_ch = 0; mux_ch < E_FJ_MUX_CH_MAX; mux_ch++ ){
		if(g_mux_config.video[mux_ch].stream_id == stream_id){
			break;
		}
	}

	if( g_mux_config.audio[0].enable == TRUE && g_mux_config.audio[1].enable == TRUE && mux_ch == E_FJ_MUX_CH_1) {
		// if 2 video streams, and 2 audio streams, we assign audio stream1 to video stream1
		aud_id = 1;
	}

	User_Utility_Record_Audio_Free_Index( aud_id, 0, TRUE );	
}

VOID User_Utility_Record_Mux_Mem_Finalize( VOID )
{
	if(g_mux_ch == E_FJ_MUX_CH_0) return;
	else g_mux_ch--;

	if(g_mux_ch == E_FJ_MUX_CH_0)
	{
		FJ_Mux_Demux_Common_Memory_Finalize();
	}
	
}

BOOL User_Utility_Record_Mux_Check_Media_Size( VOID )
{
	ULLONG media_size = BF_MP4_Common_Fs_Mediasize_Get();

	//2 frame, must bigger than bf_mux_video_sample_size_check
	if( media_size <= ( D_RS_MUX_FRAME_SIZE_MAX * 2 ) )
	{
		UPRINTF("Media size is running out, %u\n", ( UINT32 )media_size);
		return FALSE;
	}

	return TRUE;
}

VOID User_Utility_Record_Mux_VideoAddSampleCB( UINT32 code )
{
	INT32 ret;
	INT32 i;

	for(i = 0; i < 2; i++ ) {
		if(g_mux_config.audio[i].enable) {
			ret = User_Utility_Record_Audio_Stop(i);
			UPRINTF_ERR(ret);
		}
	}
}

VOID User_Utility_Aud_Mux_Config( UCHAR aud_id, T_SHARE_AUDIO_REC_CONFIG *audio )
{
	// audio
	g_mux_config.audio[aud_id].enable = audio->enable;
	g_mux_config.audio[aud_id].codec_type = audio->codec_type;
	g_mux_config.audio[aud_id].channel = audio->in_ch_type;
	g_mux_config.audio[aud_id].samplingrate = audio->samplingrate;
	g_mux_config.audio[aud_id].bitrate = audio->bitrate;
}
/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static INT32 Mux_init( VOID )
{
	INT32 ret;
	UINT32 addr, size = 0xC5CE80ul/*SDRAM_SIZ_XCODE_HOST_MUXER_INDEX_STREAM_AREA*/;

	// MUX
	// memory
	ret = FJ_GetMem( FJ_HOST_MUXER_INDEX_BUF_STREAM1_MEM_LOCATION, &addr, &size );
	UPRINTF_ERR(ret);

	g_mem_config.header_mem_addr = addr;
	g_mem_config.header_mem_size = size;
	g_mem_config.muxing_mem_addr = SDRAM_ADR_MOVIE_RECORD0_HOST_VIDEO_DFS_AREA;
	g_mem_config.muxing_mem_size = SDRAM_SIZ_MOVIE_RECORD0_HOST_VIDEO_DFS_AREA;
	g_mem_config.muxing_file_num = g_mux_ch;
	FJ_Mux_Demux_Common_Memory_Initialize( &g_mem_config );

	// string
	memcpy( &g_mux_config.string.major_brand, (VOID*)"mp42", 4 );
	g_mux_config.string.major_version = 0;
	g_mux_config.string.minor_version = 0;

	memcpy( g_mux_config.string.video_handler_name, (CHAR*)"Video Handler ", 14 );
	memcpy( g_mux_config.string.audio_handler_name, (CHAR*)"Audio Handler ", 14 );
	ret = FJ_Mux_Initialize( &g_mux_config );
	UPRINTF_ERR(ret);

	return ret;
}

