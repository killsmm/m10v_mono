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
#include "os_if.h"
#include <stdlib.h>

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

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
static BOOL g_is_audio_record[2] = {FALSE, FALSE};   // need exclusive control !!!!
static UCHAR g_audio_cap_id[2];
static UCHAR g_audio_out_id[2];
static UCHAR g_audio_enc_id[2];	//#define FJ_AUDIO_ENC_ID_0			(0x00000001)		/**< AudioEncoder Instance ID 0 */
								//#define FJ_AUDIO_ENC_ID_1			(0x00000002)		/**< AudioEncoder Instance ID 1 */
static BOOL g_es_header[2] = {FALSE, FALSE};
static BOOL g_uac_en = FALSE;

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/

static INT32 user_utility_record_audio_start( UCHAR aud_id, T_SHARE_AUDIO_REC_CONFIG* cfg );
static INT32 user_utility_record_audio_stop( UCHAR aud_id );
static INT32 user_utility_record_audio_end( UCHAR aud_id );

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

INT32 User_Utility_Record_Audio_PCMStreamCB( UCHAR audio_enc_id, T_FJ_AUDIO_PCM_STREAM_PARAM* cb_info )
{
	INT32 ret = NG;
	ULLONG temp_pts;

	if( g_is_audio_record[audio_enc_id - 1] ){
		temp_pts = cb_info->time_stamp * cb_info->sampling_freq / 90000;
		if( g_es_header[audio_enc_id - 1]) {
			OS_IF_Rtos_Audio_Stream_Send_PCM(g_audio_enc_id[audio_enc_id - 1], cb_info, g_uac_en);
		} else {
			ret = FJ_Mux_Audio_Add_Sample(audio_enc_id - 1, cb_info->area_index, (BYTE *)cb_info->pcm_data, cb_info->pcm_size, temp_pts);
			UPRINTF_ERR(ret);
		}
	}

	return ret;
}

INT32 User_Utility_Record_Audio_AACStreamCB( UCHAR audio_enc_id, T_FJ_AUDIO_AAC_STREAM_PARAM* cb_info )
{
	INT32 ret = NG;
	ULLONG temp_pts;

	if( g_is_audio_record[audio_enc_id - 1] ){
		temp_pts = cb_info->time_stamp * cb_info->sampling_freq / 90000;
		if( g_es_header[audio_enc_id - 1]) {
			OS_IF_Rtos_Audio_Stream_Send_AAC(g_audio_enc_id[audio_enc_id - 1], cb_info, g_uac_en);
		} else {
			ret = FJ_Mux_Audio_Add_Sample( audio_enc_id - 1, cb_info->area_index, (BYTE*)cb_info->es_data, cb_info->es_size, temp_pts );
			UPRINTF_ERR(ret);
		}
	}

	return ret;
}

INT32 User_Utility_Record_Audio_Free_Index( UCHAR aud_id, UINT32 free_index, BOOL last_free_flg )
{
	INT32 ret = NG;

	if( g_is_audio_record[aud_id] ){
        ret = FJ_AudioEncoder_FreeIndex( g_audio_enc_id[aud_id], free_index, last_free_flg );
		UPRINTF_ERR(ret);
		
		if( last_free_flg ) {
			User_Utility_Record_Audio_End(aud_id);
		}
	}

	return ret;
}

INT32 User_Utility_Record_Audio_CapturePlugIn0CB( UCHAR audio_capture_id, T_FJ_AUDIO_CAP_PLUGIN0_CB_PARAM* param )
{
	UINT32 loop;
	UINT8* out_data;
	UINT8* in_data;

	if ((param->in_ch_type == 4) && (param->out_ch_type == 2)){
		// 4ch -> 2ch
		out_data = (UINT8*)param->out_addr;
		in_data = (UINT8*)param->in_addr;
		for (loop = 0; loop < 1024; loop++){
			// L1
			*out_data = *in_data;
			out_data++;	
			in_data++;
			*out_data = *in_data;
			out_data++;
			in_data++;
			// data is 24bits case
			if (param->in_size == 1024 * 3) {
				*out_data = *in_data;
				out_data++;
				in_data++;
			}
			
			// R1
			*out_data = *in_data;
			out_data++;
			in_data++;
			*out_data = *in_data;
			out_data++;
			in_data++;
			if (param->in_size == 1024 * 3) {
				*out_data = *in_data;
				out_data++;
				in_data++;
			}
			
			//L2
			in_data++;
			in_data++;
			if (param->in_size == 1024 * 3) in_data++;
			
			//R2
			in_data++;
			in_data++;
			if (param->in_size == 1024 * 3) in_data++;
		}
		
		param->out_size = param->in_size / 2;
		param->out_ch_type = param->in_ch_type / 2;
		param->out_sampling_rate = param->in_sampling_rate;
	}
	else if ((param->in_ch_type == 3) && (param->out_ch_type == 2)){
		// 3ch -> 2ch
		out_data = (UINT8*)param->out_addr;
		in_data = (UINT8*)param->in_addr;
		for (loop = 0; loop < 1024; loop++){
			// L1
			*out_data = *in_data;
			out_data++;	
			in_data++;
			*out_data = *in_data;
			out_data++;
			in_data++;
			if (param->in_size == 1024 * 3) {
				*out_data = *in_data;
				out_data++;
				in_data++;
			}
			
			// R1
			*out_data = *in_data;
			out_data++;
			in_data++;
			*out_data = *in_data;
			out_data++;
			in_data++;
			if (param->in_size == 1024 * 3) {
				*out_data = *in_data;
				out_data++;
				in_data++;
			}
			
			//L2
			in_data++;
			in_data++;
			if (param->in_size == 1024 * 3) in_data++;
		}
		
		param->out_size = (param->in_size / 3) * 2;
		param->out_ch_type = 2;
		param->out_sampling_rate = param->in_sampling_rate;
	}
	else if ((param->in_ch_type == 3) && (param->out_ch_type == 1)){
		// 3ch -> 1ch
		out_data = (UINT8*)param->out_addr;
		in_data = (UINT8*)param->in_addr;
		for (loop = 0; loop < 1024; loop++){
			// L1
			in_data++;
			in_data++;
			if (param->in_size == 1024 * 3) in_data++;
			
			// R1
			in_data++;
			in_data++;
			if (param->in_size == 1024 * 3) in_data++;
			
			// L2
			*out_data = *in_data;
			out_data++;	
			in_data++;
			*out_data = *in_data;
			out_data++;
			in_data++;
			if (param->in_size == 1024 * 3) {
				*out_data = *in_data;
				out_data++;
				in_data++;
			}
		}
		
		param->out_size = (param->in_size / 3);
		param->out_ch_type = 1;
		param->out_sampling_rate = param->in_sampling_rate;
	}
	else if ((param->in_ch_type == 1) && (param->out_ch_type == 2)){
		// 1ch -> 2ch
		out_data = (UINT8*)param->out_addr;
		in_data = (UINT8*)param->in_addr;
		for (loop = 0; loop < 1024; loop++){
			// L1
			*out_data = *in_data;
			out_data++;	
			in_data++;
			*out_data = *in_data;
			out_data++;
			in_data++;
			if (param->in_size == 1024 * 3) {
				*out_data = *in_data;
				out_data++;
				in_data++;
			}

			// R1
			in_data--;
			in_data--;
			if (param->in_size == 1024 * 3) in_data--;

			*out_data = *in_data;
			out_data++;	
			in_data++;
			*out_data = *in_data;
			out_data++;
			in_data++;
			if (param->in_size == 1024 * 3) {
				*out_data = *in_data;
				out_data++;
				in_data++;
			}
		}
		
		param->out_size = (param->in_size * 2);
		param->out_ch_type = 2;
		param->out_sampling_rate = param->in_sampling_rate;
	}
	else {
		memcpy((VOID*)param->out_addr,
			   (VOID*)param->in_addr,
			   param->in_size);
		
		param->out_size = param->in_size;
		param->out_ch_type = param->in_ch_type;
		param->out_sampling_rate = param->in_sampling_rate;
	}

	return OK;
}

INT32 User_Utility_Record_Audio_CapturePlugIn1CB( UCHAR audio_capture_id, T_FJ_AUDIO_CAP_PLUGIN1_CB_PARAM* param )
{
	FJ_AudioCapture_CompletePlugIn1(audio_capture_id, param->area_index);
	return OK;
}

INT32 User_Utility_Record_Audio_Start( UCHAR aud_id, T_SHARE_AUDIO_REC_CONFIG* cfg)
{
	INT32 ret;

	ret = user_utility_record_audio_start(aud_id, cfg);
	UPRINTF_ERR(ret);

	g_is_audio_record[aud_id] = TRUE;
	g_es_header[aud_id] = cfg->stream_format == FJ_MOVIE_STREAM_FORMAT_ES_HEADER ? TRUE : FALSE;
	g_uac_en = cfg->uac_en;
	
	return OK;
}

INT32 User_Utility_Record_Audio_Stop( UCHAR aud_id )
{
	INT32 ret;
	
	if( g_is_audio_record[aud_id] ){
		ret = user_utility_record_audio_stop(aud_id);
		UPRINTF_ERR(ret);
	}
	
	return OK;
}

INT32 User_Utility_Record_Audio_End( UCHAR aud_id )
{
	INT32 ret;
	
	if( g_is_audio_record[aud_id] ){
		ret = user_utility_record_audio_end(aud_id);
		UPRINTF_ERR(ret);

		g_is_audio_record[aud_id] = FALSE;
		Snd_Event_to_User( aud_id == 0 ? E_USER_EVENT_RECORD_AUDIO_0_END : E_USER_EVENT_RECORD_AUDIO_1_END, 0 );
	}
	
	return OK;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/

static INT32 user_utility_record_audio_start( UCHAR aud_id, T_SHARE_AUDIO_REC_CONFIG* cfg )
{
	INT32 ret;
	T_FJ_AUDIO_CAPTURE_CFG cap_cfg;
	T_FJ_AUDIO_CAP_SMPL_CFG smpl_cfg;
	T_FJ_AUDIO_AAC_CONFIG_PARAM	aac_config;
	T_FJ_AUDIO_OUT_CFG out_cfg;

	ret = FJ_AudioCapture_Open( aud_id, &g_audio_cap_id[aud_id]);
	UPRINTF_ERR(ret);

	if( cfg->hdmi_out_en == TRUE){
		ret = FJ_AudioOut_Open((E_FJ_AUDIO_OUT_ENUM)aud_id, &g_audio_out_id[aud_id]);
		UPRINTF_ERR(ret);
		ret = FJ_AudioCapture_ConnectOut(g_audio_cap_id[aud_id], g_audio_out_id[aud_id]);
		UPRINTF_ERR(ret);
	}
	else{
		g_audio_out_id[aud_id] = 0;
	}
	
	if( cfg->codec_type == FJ_COMPRESS_TYPE_AAC) {
		ret = FJ_AudioEncoder_Open(aud_id == 0 ? E_FJ_AUDIO_ENC_AAC_0 : E_FJ_AUDIO_ENC_AAC_1, (UCHAR *)&g_audio_enc_id[aud_id]);
		UPRINTF_ERR(ret);
	} else {
		ret = FJ_AudioEncoder_Open(E_FJ_AUDIO_ENC_NONE_0, (UCHAR *)&g_audio_enc_id[aud_id]);
		UPRINTF_ERR(ret);
	}

	ret = FJ_AudioCapture_ConnectEnc(g_audio_cap_id[aud_id], g_audio_enc_id[aud_id]);
	UPRINTF_ERR(ret);

	cap_cfg.audio_in_port = cfg->audio_in_port;
	cap_cfg.i2s_cfg.master_slave = cfg->slave_mode;
	cap_cfg.i2s_cfg.lr_clock = cfg->samplingrate;
	cap_cfg.i2s_cfg.data_cycle = 64;
	cap_cfg.i2s_cfg.i2s_format = E_FJ_AUDIO_I2S_Data_FORMAT_I2S;
	cap_cfg.i2s_cfg.bit_count = cfg->bitdepth;
	ret = FJ_AudioCapture_Config(g_audio_cap_id[aud_id], &cap_cfg);
	UPRINTF_ERR(ret);

	// Only master mode, audio codec applied. slave mode I2S is not controlled by M10v
	if (cfg->slave_mode == FALSE) {
		AudioCodecWrapper_PowerUp();
		AudioCodecWrapper_Record_Start();
	}

	if( cfg->hdmi_out_en == TRUE){
		out_cfg.audio_out_port = E_FJ_AUDIO_OUT_PORT_I2S3;
		out_cfg.i2s_cfg.master_slave = cfg->slave_mode;
		out_cfg.i2s_cfg.lr_clock = cfg->samplingrate;
		out_cfg.i2s_cfg.data_cycle = 64;
		out_cfg.i2s_cfg.i2s_format = E_FJ_AUDIO_I2S_Data_FORMAT_LEFT;
		out_cfg.i2s_cfg.bit_count = cfg->bitdepth;
		ret = FJ_AudioOut_Config(g_audio_out_id[aud_id], &out_cfg);
		UPRINTF_ERR(ret);
	}
	
	smpl_cfg.in_sampling_rate = cfg->samplingrate;
	smpl_cfg.in_ch_type = cfg->in_ch_type;
	smpl_cfg.out_sampling_rate = cfg->samplingrate;
	smpl_cfg.out_ch_type = cfg->out_ch_type;
	ret = FJ_AudioCapture_SampleConfig(g_audio_cap_id[aud_id], &smpl_cfg);
	UPRINTF_ERR(ret);

	ret = FJ_AudioCapture_EnablePlugIn1(g_audio_cap_id[aud_id], FJ_AUDIO_ENABLE);
	UPRINTF_ERR(ret);

	ret = FJ_AudioCapture_Start(g_audio_cap_id[aud_id]);
	UPRINTF_ERR(ret);

	aac_config.enable_flag = FJ_AUDIO_ENABLE;
	aac_config.sampling_rate = cfg->samplingrate;
	aac_config.bit_rate	= cfg->bitrate;
	aac_config.out_ch_type = cfg->out_ch_type;
	ret = FJ_AudioEncoder_Config( (UCHAR)g_audio_enc_id[aud_id], FJ_AUDIO_AAC_CFG, (UINT32 *)&aac_config );
	UPRINTF_ERR(ret);

	ret = FJ_AudioEncoder_Setup( (UCHAR)g_audio_enc_id[aud_id] );
	UPRINTF_ERR(ret);

	OS_User_Dly_Tsk(5);

	ret = FJ_AudioEncoder_Start( (UCHAR)g_audio_enc_id[aud_id] );
	UPRINTF_ERR(ret);

	return OK;
}

static INT32 user_utility_record_audio_stop( UCHAR aud_id )
{
	INT32 ret;

	ret = FJ_AudioEncoder_Stop( (UCHAR)g_audio_enc_id[aud_id] );
	UPRINTF_ERR(ret);

	return OK;
}

static INT32 user_utility_record_audio_end( UCHAR aud_id )
{
	INT32 ret;

	ret = FJ_AudioCapture_Stop(g_audio_cap_id[aud_id]);
	UPRINTF_ERR(ret);

	ret = FJ_AudioCapture_DisconnectEnc(g_audio_cap_id[aud_id], g_audio_enc_id[aud_id]);
	UPRINTF_ERR(ret);

	if(g_audio_out_id[aud_id] != 0){
		FJ_AudioCapture_DisconnectOut(g_audio_cap_id[aud_id], g_audio_out_id[aud_id]);
	}

	ret = FJ_AudioEncoder_Close( (UCHAR)g_audio_enc_id[aud_id] );
	UPRINTF_ERR(ret);

	ret = FJ_AudioCapture_Close(g_audio_cap_id[aud_id]);
	UPRINTF_ERR(ret);

	if(g_audio_out_id[aud_id] != 0){
		FJ_AudioOut_Close(g_audio_out_id[aud_id]);
		g_audio_out_id[aud_id] = 0;
	}

	return OK;
}


