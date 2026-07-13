/**
 * @file		user_yuvin_view.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2019 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "user.h"
#include "user_utility_jpeg.h"
#include "user_utility_disp.h"
#include "user_utility_ip.h"
#include "user_utility_dump_rawyuv.h"
#include "os_user_custom.h"
#include "share_data.h"
#include "category_parameter.h"
#include "fj_filesystem.h"
#include "fj_disp.h"
#include "os_if.h"
#include "clk.h"
#include "arb.h"
#include "sdram_map_define.h"
#include "user_sync_task.h"
#include "detect_task.h"
#include "LT6911UX.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define USER_VIEW_SDCH1 (1)
#define D_AUDIO_REC_END_INDEX_0 3
#define D_AUDIO_REC_END_INDEX_1 4
/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
typedef enum {
	E_USER_SNAP_NONE = 0,
	E_USER_SNAP_YUV,
	E_USER_SNAP_JPG,
	E_USER_SNAP_MAX,
} E_USER_SNAP_TYPE;

typedef enum {
	E_USER_REC_STATE_IDLE = 0,
	E_USER_REC_STATE_WAIT_STOP,
	E_USER_REC_STATE_RUN,
	E_USER_REC_STATE_MAX,
} E_USER_REC_STATE;

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	E_USER_REC_STATE	rec_state;
	BOOL				rec_end[D_AUDIO_REC_END_INDEX_1 + 1]; // index0:H265, 1:H264_0, 2:H264_1, 3:AUDIO_0, 4:AUDIO_1
} T_USER_YUVIN_MNG;
/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_USER_YUVIN_MNG	g_user_yuvin_mng;
static E_USER_STATE		g_user_yuvin_view_state = E_USER_STATE_IDLE;
static E_USER_OP_STATE  g_user_yuvin_view_op_state = E_USER_OP_NONE;

static T_SHARE_SYSTEM_PARAM  g_system_param;
static T_SHARE_SYSTEM_CONFIG g_system_config = {
	.available_memory_addr_ch0 = SDRAM_ADR_CH0_RTOS_IMAGE_WORK,
	.end_available_memory_addr_ch0 = SDRAM_ADR_CH0_RTOS_IMAGE_WORK + SDRAM_SIZ_CH0_RTOS_IMAGE_WORK - 1,
	.available_memory_addr_ch1 = SDRAM_ADR_CH1_RTOS_IMAGE_WORK,
	.end_available_memory_addr_ch1 = SDRAM_ADR_CH1_RTOS_IMAGE_WORK + SDRAM_SIZ_CH1_RTOS_IMAGE_WORK - 1,
	.sensor_num = 1,
	.sensor_layout = E_SHARE_SENSOR_LAYOUT_H_LINEAR,
	.sensor_out[E_SHARE_SENSOR_OUT_A] = E_SHARE_SENSOR_ID_0,
	.sensor_out[E_SHARE_SENSOR_OUT_B] = E_SHARE_SENSOR_ID_1,
	.sensor_out[E_SHARE_SENSOR_OUT_C] = E_SHARE_SENSOR_ID_2,
	.sensor_out[E_SHARE_SENSOR_OUT_D] = E_SHARE_SENSOR_ID_3,
	.disp_if = E_SHARE_DISP_IF_HDMI,
	.sensor_yuv_use_flg = (1<<E_SHARE_YUV_OUT_USER_0),
	.sensor_exp_NO_update = FALSE,
	.sensor_div_forced = 0,
	.sensor_yuv_out_0_none_resize = FALSE,
	.frame_num = D_FRAME_NUM_INFINITY,
	.op_ipu_mode 	= E_SHARE_OP_IPU_MODE_NORMAL,
	.op_3dnr_mode	= E_SHARE_OP_3DNR_MODE_OFF,
	.op_eis_mode 	= E_SHARE_OP_EIS_MODE_OFF,
	.sensor_yuv[E_SHARE_YUV_OUT_USER_0].format = E_SHARE_YUV_FORMAT_420_SEMI,
	.sensor_yuv[E_SHARE_YUV_OUT_USER_0].video = E_SHARE_YUV_VIDEO_NONE,
	.sensor_yuv[E_SHARE_YUV_OUT_USER_0].ask_user_for_video_addr = NULL,
	.sensor_yuv[E_SHARE_YUV_OUT_USER_0].h_size = 3840,
	.sensor_yuv[E_SHARE_YUV_OUT_USER_0].v_size = 2160,

	.sensor_yuv[E_SHARE_YUV_OUT_H265].format = E_SHARE_YUV_FORMAT_420_SEMI,
	.sensor_yuv[E_SHARE_YUV_OUT_H265].video = E_SHARE_YUV_VIDEO_265,
	.sensor_yuv[E_SHARE_YUV_OUT_H265].ask_user_for_video_addr = User_Utility_Record_H265_Get_Addr,
	.sensor_yuv[E_SHARE_YUV_OUT_H265].h_size = 3840,
	.sensor_yuv[E_SHARE_YUV_OUT_H265].v_size = 2160,
		
	.sensor_yuv[E_SHARE_YUV_OUT_H264_0].format = E_SHARE_YUV_FORMAT_420_SEMI,
	.sensor_yuv[E_SHARE_YUV_OUT_H264_0].video = E_SHARE_YUV_VIDEO_264,
	.sensor_yuv[E_SHARE_YUV_OUT_H264_0].ask_user_for_video_addr = User_Utility_Record_H264_Get_Addr,
	.sensor_yuv[E_SHARE_YUV_OUT_H264_0].h_size = 1920,
	.sensor_yuv[E_SHARE_YUV_OUT_H264_0].v_size = 1080,
			
	.sensor_yuv[E_SHARE_YUV_OUT_H264_1].format = E_SHARE_YUV_FORMAT_420_SEMI,
	.sensor_yuv[E_SHARE_YUV_OUT_H264_1].video = E_SHARE_YUV_VIDEO_264,
	.sensor_yuv[E_SHARE_YUV_OUT_H264_1].ask_user_for_video_addr = User_Utility_Record_H264_Get_Addr,
	.sensor_yuv[E_SHARE_YUV_OUT_H264_1].h_size = 1280,
	.sensor_yuv[E_SHARE_YUV_OUT_H264_1].v_size = 720,
	
};
static T_SHARE_SYSTEM_CONFIG_RETURN g_system_config_ret;

static T_SHARE_YUV g_share_yuv;
static T_SHARE_YUV g_share_sgde_yuv;

static T_SHARE_MOVIE_REC_CONFIG	g_movie_rec_cfg = 
{
	.video[E_SHARE_YUV_OUT_H265].enable = TRUE,
	.video[E_SHARE_YUV_OUT_H265].compress = TRUE,
	.video[E_SHARE_YUV_OUT_H265].codec_id = E_FJ_VIDEO_ENC_ENUM_H265_0,
	.video[E_SHARE_YUV_OUT_H265].image_size = E_FJ_MOVIE_VIDEO_SIZE_3840_2160,
	.video[E_SHARE_YUV_OUT_H265].stream_format = FJ_MOVIE_STREAM_FORMAT_ES,
	.video[E_SHARE_YUV_OUT_H265].gop_struct = FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP,
	.video[E_SHARE_YUV_OUT_H265].bitrate_algorithm = FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_VBR,
	.video[E_SHARE_YUV_OUT_H265].bitrate = 30000000,
	.video[E_SHARE_YUV_OUT_H265].average_bitrate = 30000000,
	.video[E_SHARE_YUV_OUT_H265].frame_rate = 3000,
	.video[E_SHARE_YUV_OUT_H265].lapse_period_ms = 0,
	.video[E_SHARE_YUV_OUT_H265].slow_rate = 1,

	.video[E_SHARE_YUV_OUT_H264_0].enable = TRUE,
	.video[E_SHARE_YUV_OUT_H264_0].compress = FALSE,
	.video[E_SHARE_YUV_OUT_H264_0].codec_id = E_FJ_VIDEO_ENC_ENUM_H264_0,
	.video[E_SHARE_YUV_OUT_H264_0].image_size = E_FJ_MOVIE_VIDEO_SIZE_1920_1080,
	.video[E_SHARE_YUV_OUT_H264_0].stream_format = FJ_MOVIE_STREAM_FORMAT_ES,
	.video[E_SHARE_YUV_OUT_H264_0].gop_struct = FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP,
	.video[E_SHARE_YUV_OUT_H264_0].bitrate_algorithm = FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_CBR,
	.video[E_SHARE_YUV_OUT_H264_0].bitrate = 7000000,
	.video[E_SHARE_YUV_OUT_H264_0].average_bitrate = 7000000,
	.video[E_SHARE_YUV_OUT_H264_0].frame_rate = 3000,
	.video[E_SHARE_YUV_OUT_H264_0].lapse_period_ms = 0,
	.video[E_SHARE_YUV_OUT_H264_0].slow_rate = 1,
		
	.video[E_SHARE_YUV_OUT_H264_1].enable = FALSE,
	.video[E_SHARE_YUV_OUT_H264_1].compress = FALSE,
	.video[E_SHARE_YUV_OUT_H264_1].codec_id = E_FJ_VIDEO_ENC_ENUM_H264_1,
	.video[E_SHARE_YUV_OUT_H264_1].image_size = E_FJ_MOVIE_VIDEO_SIZE_1280_720,
	.video[E_SHARE_YUV_OUT_H264_1].stream_format = FJ_MOVIE_STREAM_FORMAT_ES,
	.video[E_SHARE_YUV_OUT_H264_1].gop_struct = FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP,
	.video[E_SHARE_YUV_OUT_H264_1].bitrate_algorithm = FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_CBR,
	.video[E_SHARE_YUV_OUT_H264_1].bitrate = 7000000,
	.video[E_SHARE_YUV_OUT_H264_1].average_bitrate = 7000000,
	.video[E_SHARE_YUV_OUT_H264_1].frame_rate = 3000,
	.video[E_SHARE_YUV_OUT_H264_1].lapse_period_ms = 0,
	.video[E_SHARE_YUV_OUT_H264_1].slow_rate = 1,

	.audio[E_SHARE_AUDIO_INSTANCE_0].enable = FALSE,
	.audio[E_SHARE_AUDIO_INSTANCE_0].codec_type = FJ_COMPRESS_TYPE_AAC,
	.audio[E_SHARE_AUDIO_INSTANCE_0].samplingrate = 48000,
	.audio[E_SHARE_AUDIO_INSTANCE_0].bitrate = 192000,
	.audio[E_SHARE_AUDIO_INSTANCE_0].audio_in_port = E_FJ_AUDIO_IN_PORT_I2S0,
	.audio[E_SHARE_AUDIO_INSTANCE_0].in_ch_type = FJ_AUDIO_NUM_CHANNEL_2,
	.audio[E_SHARE_AUDIO_INSTANCE_0].out_ch_type = FJ_AUDIO_NUM_CHANNEL_2,
	.audio[E_SHARE_AUDIO_INSTANCE_0].slave_mode = TRUE,
	.audio[E_SHARE_AUDIO_INSTANCE_0].bitdepth	= 16,

	.audio[E_SHARE_AUDIO_INSTANCE_1].enable = FALSE,
	.audio[E_SHARE_AUDIO_INSTANCE_1].codec_type = FJ_COMPRESS_TYPE_AAC,
	.audio[E_SHARE_AUDIO_INSTANCE_1].samplingrate = 48000,
	.audio[E_SHARE_AUDIO_INSTANCE_1].bitrate = 192000,
	.audio[E_SHARE_AUDIO_INSTANCE_1].audio_in_port = E_FJ_AUDIO_IN_PORT_I2S2,
	.audio[E_SHARE_AUDIO_INSTANCE_1].in_ch_type = FJ_AUDIO_NUM_CHANNEL_2,
	.audio[E_SHARE_AUDIO_INSTANCE_1].out_ch_type = FJ_AUDIO_NUM_CHANNEL_2,
	.audio[E_SHARE_AUDIO_INSTANCE_1].slave_mode = TRUE,
	.audio[E_SHARE_AUDIO_INSTANCE_1].bitdepth	= 16,
};

static ULONG g_movie_request_fps;


static E_USER_SNAP_TYPE g_snapshot_type = E_USER_SNAP_NONE;
static BOOL g_ipcu_streaming = FALSE;
static BOOL g_pre_op_idle_mode = FALSE;
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static INT32 system_config_callback( T_SHARE_SYSTEM_CONFIG_RETURN* ret );
static E_FJ_MOVIE_VIDEO_SIZE system_config_convert_video_size( ULONG width, ULONG lines );

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
E_USER_STATE User_YUVin_View( E_USER_EVENT event, ULLONG frame_no )
{
	static int snap_param = 0;
	T_USER_SYNC_MSG user_sync_msg;
	OS_USER_ER os_err = 0;
	FJ_ERR_CODE ercd = FJ_ERR_OK;
	UCHAR detect = 0;
	E_SHARE_YUV_OUT layout;
	int rval = 0;
	T_SHARE_LAYOUT *yuv_layout = NULL;
	FJ_PERI_CH i2c_ch = FJ_PERI_CH_0;
	int i, j;
	ULONG sen_use = 0;
	UINT32 yuv_size_byte;
	
	if ((event == E_USER_EVENT_SAVE_JPG && User_Utility_is_IPCU_JPEG_Enable() == E_DUMP_STORAGE) ||
		(event == E_USER_EVENT_SAVE_YUV && User_Utility_is_IPCU_YUV_Enable() == E_DUMP_STORAGE) ||
		(event == E_USER_EVENT_RECORD_START && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].stream_format == FJ_MOVIE_STREAM_FORMAT_ES) ||
		(event == E_USER_EVENT_RECORD_START && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].stream_format == FJ_MOVIE_STREAM_FORMAT_ES) ||
		(event == E_USER_EVENT_RECORD_START && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].stream_format == FJ_MOVIE_STREAM_FORMAT_ES)) {
		ercd = FJ_Get_MediaDetect(USER_VIEW_SDCH1, &detect);
		UPRINTF_ERR(ercd);

		if( detect == 0 ){
			ercd = FJ_ERR_NOT_MOUNT;
			UPRINTF_ERR(ercd);
			return g_user_yuvin_view_state;
		}
	}

	switch ( event )
	{
	case E_USER_EVENT_YUV_IN_VIEW_START:
		g_user_yuvin_view_op_state = E_USER_OP_NONE;
		g_user_yuvin_view_state = E_USER_STATE_BUSY;
		UPRINTF("[CLOCK]Change Image Clock Mode to View\n");
		FJ_Top_Set_Image_Clock(	IMAGE_CLOCK[E_IMG_CLOCK_MODE_NORMAL_VIEW] );
		Set_Arb_MXIC(0);
		//Set_Arb_SDRAMC(0);

		memset(&g_system_param, 0, sizeof(g_system_param));
		g_system_param.lock_address_y = D_ADDR_UNLOCK;
		g_system_param.lock_address_c = D_ADDR_UNLOCK;
		g_system_param.lock_address_raw = D_ADDR_UNLOCK;

		g_system_config.IQ_BIN_addr = User_Get_Resource()->IQ_BIN_addr[0];
		g_system_config.IQ_BIN_size = User_Get_Resource()->IQ_BIN_size[0];

		g_system_config.available_memory_addr_ch0 = SDRAM_ADR_CH0_RTOS_IMAGE_WORK;
		g_system_config.available_memory_addr_ch1 = SDRAM_ADR_CH1_RTOS_IMAGE_WORK;

		sen_use |= Cate_Get_HDMI_SEN_USED() & 0x0000000F ? 1 << 0 : 0;
		sen_use |= Cate_Get_HDMI_SEN_USED() & 0x000000F0 ? 1 << 1 : 0;
		sen_use |= Cate_Get_HDMI_SEN_USED() & 0x00000F00 ? 1 << 2 : 0;
		sen_use |= Cate_Get_HDMI_SEN_USED() & 0x0000F000 ? 1 << 3 : 0;

		for(i = 0, j = 0; i < Cate_Get_Bridge_Num(); i++) {
			for(; j < 4; j++) {
				if((1 << j) & sen_use) {
					i2c_ch = j;
					break;
				}
			}
			rval = LT6911UX_inqury_va_info(i2c_ch, &g_system_config.va_info[i]);
			if (rval) {
				UPRINTF_ERR(rval);
				return E_USER_STATE_IDLE;
			}
		}

		for(i = 0; i < Cate_Get_Bridge_Num(); i++) {
			switch (g_system_config.va_info[i].audio.sampling_rate) {
			case 8000:
			case 11025:
			case 12000:
			case 16000:
			case 22050:
			case 24000:
			case 32000:
			case 44100:
			case 48000:
				g_movie_rec_cfg.audio[i].samplingrate = g_system_config.va_info[i].audio.sampling_rate;
				break;
			default:
				UPRINTF("Audio sampling rate:%d is not supported\n", g_system_config.va_info[i].audio.sampling_rate);
				UPRINTF_ERR(-2);
				g_movie_rec_cfg.audio[i].samplingrate = 48000;
				break;
			}
		}

		Cate_Get_Config( &g_system_config, D_CONFIG_MODE_YUVIN_VIEW );
		Cate_Get_MovieRec_Config(&g_movie_rec_cfg);
		Cate_Get_System_Param( &g_system_param, D_CONFIG_MODE_VIEW );

		for(i = 0; i < 2; i++) {
			g_movie_rec_cfg.audio[i].codec_type = g_system_config.audio  == E_SHARE_AUDIO_AAC ? FJ_COMPRESS_TYPE_AAC : FJ_COMPRESS_TYPE_PCM;
		}
		// Update recording fps
		if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == TRUE) {
			g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].frame_rate = g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H265].video_fps;
			g_movie_request_fps = (FLOAT)g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].frame_rate;

			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE) {
				g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].frame_rate = g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_0].video_fps;
				if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE) {
					g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].frame_rate = g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].video_fps;
				}
			} else if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE) {
				g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].frame_rate = g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].video_fps;
			}
		} else if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE) {
			g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].frame_rate = g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_0].video_fps;
			g_movie_request_fps = (FLOAT)g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].frame_rate;

			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE) {
				g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].frame_rate = g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].video_fps;
			}
		} else if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE) {
			g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].frame_rate = g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].video_fps;
			g_movie_request_fps = (FLOAT)g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].frame_rate;
		} else {
			g_movie_request_fps = 0;
		}
		g_movie_rec_cfg.base_fps = g_movie_request_fps;
		
		Set_Share_System_Param( &g_system_param );

		// Update recording size
		if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == TRUE) {
			g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].image_size = 
				system_config_convert_video_size(g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H265].h_size, g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H265].v_size);
		}
		if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE) {
			g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].image_size = 
				system_config_convert_video_size(g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_0].h_size, g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_0].v_size);
		}
		if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE) {
			g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].image_size = 
				system_config_convert_video_size(g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].h_size, g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].v_size);
		}

		g_system_config.h265_compress = FALSE;
		g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].compress = g_system_config.h265_compress;

		g_user_yuvin_mng.rec_state = E_USER_REC_STATE_IDLE;
		g_user_yuvin_mng.rec_end[E_SHARE_YUV_OUT_H265] = FALSE;
		g_user_yuvin_mng.rec_end[E_SHARE_YUV_OUT_H264_0] = FALSE;
		g_user_yuvin_mng.rec_end[E_SHARE_YUV_OUT_H264_1] = FALSE;
		g_user_yuvin_mng.rec_end[D_AUDIO_REC_END_INDEX_0] = FALSE;
		g_user_yuvin_mng.rec_end[D_AUDIO_REC_END_INDEX_1] = FALSE;

		// inform user sync task, view is started
		user_sync_msg.event		= E_USER_SYNC_EVENT_VIEW_START;
		user_sync_msg.frame_no	= frame_no;
		os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
		UPRINTF_ERR(os_err);

		Set_Share_Available_Memory( g_system_config.available_memory_addr_ch0, g_system_config.end_available_memory_addr_ch0 ); 
		Set_Share_Available_Memory( g_system_config.available_memory_addr_ch1, g_system_config.end_available_memory_addr_ch1 ); 
		Set_Share_Movie_Rec_Config(&g_movie_rec_cfg);
		Set_Share_System_Config( &g_system_config, E_SHARE_SYSTEM_MODE_YUVIN_VIEW, (T_SHARE_ENTRY)system_config_callback );
		User_Utility_disp_switch( g_system_config.disp_if );
		Send_Share_System_Event(E_SHARE_SYSTEM_EVENT_START_MODE, 0);
		break;
	case E_USER_EVENT_STANDBY:
		if (g_user_yuvin_mng.rec_state == E_USER_REC_STATE_RUN) {
			Snd_Event_to_User( E_USER_EVENT_RECORD_STOP, 0 );
			g_pre_op_idle_mode = TRUE;
		} else if ( g_system_config.face_detection ) {
			Snd_Event_to_User( E_USER_EVENT_FD_STOP, 0 );
			g_pre_op_idle_mode = TRUE;
		} else if ( g_system_config.person_detection ) {
			Snd_Event_to_User( E_USER_EVENT_PD_STOP, 0 );
			g_pre_op_idle_mode = TRUE;
		} else {
			Send_Share_System_Event(E_SHARE_SYSTEM_EVENT_STOP_MODE, 0);
		}
		OS_IF_Rtos_UVC_Term();
		break;
	case E_USER_EVENT_SYSTEM_MODE_END:
		g_user_yuvin_view_state = E_USER_STATE_IDLE;
		break;
	case E_USER_EVENT_VD:
		Get_Share_System_Param(&g_system_param);
		Cate_Get_System_Param( &g_system_param, D_CONFIG_MODE_VIEW );
		Set_Share_System_Param( &g_system_param );
		break;
	case E_USER_EVENT_YUV:
		g_user_yuvin_view_state = E_USER_STATE_RUN;

		if ( g_system_config.face_detection ) {
			user_sync_msg.event		= E_USER_SYNC_EVENT_FACE_DETECTION;
			user_sync_msg.frame_no	= frame_no;
			os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
			UPRINTF_ERR(os_err);
		}
		if ( g_system_config.person_detection ) {
			user_sync_msg.event		= E_USER_SYNC_EVENT_PERSON_DETECTION;
			user_sync_msg.frame_no	= frame_no;
			os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
			UPRINTF_ERR(os_err);
		}

		Get_Frame_Share_YUV( &g_share_yuv, frame_no );
		if(g_system_config.op_ipu_mode == E_SHARE_OP_IPU_MODE_PIPE_ONLY ||
		   g_system_config.op_ipu_mode == E_SHARE_OP_IPU_MODE_IPU_ONLY) {
			layout = E_SHARE_YUV_OUT_USER_0;
		} else {
			layout = E_SHARE_YUV_OUT_USER_1;
		}

		switch ( g_system_config.disp_if ) {
			case E_SHARE_DISP_IF_LCD:
				User_Utility_LCD_Yuv( &g_share_yuv.layout[layout]);
			break;
			case E_SHARE_DISP_IF_HDMI:
				User_Utility_HDMI_Yuv( &g_share_yuv.layout[layout] );
			break;
			default:
				User_Utility_HDMI_Yuv( &g_share_yuv.layout[layout] );
			break;
		}
#if 0
		if (g_user_yuvin_mng.rec_state == E_USER_REC_STATE_RUN || g_user_yuvin_mng.rec_state == E_USER_REC_STATE_WAIT_STOP) {
			if( g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == TRUE ) {
				if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].codec_id == E_FJ_VIDEO_ENC_ENUM_H265_0)
					User_Utility_Record_H265_Rel_Addr( frame_no );
				else
					User_Utility_Record_H264_Rel_Addr( E_SHARE_YUV_OUT_H265, frame_no );
			}
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE) {
				User_Utility_Record_H264_Rel_Addr( E_SHARE_YUV_OUT_H264_0, frame_no );
			}
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE){
				User_Utility_Record_H264_Rel_Addr( E_SHARE_YUV_OUT_H264_1, frame_no );
			}
		}
#endif
		if ( g_ipcu_streaming ) {
			if ( User_Utility_is_IPCU_JPEG_Enable() ) {
				user_sync_msg.event		= E_USER_SYNC_EVENT_SAVE_JPG;
				user_sync_msg.frame_no	= frame_no;
				user_sync_msg.param		= E_NORMAL_JPEG;
				user_sync_msg.param1	= 0; // no used here
				os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
				UPRINTF_ERR(os_err);
			}
			
			if ( User_Utility_is_IPCU_YUV_Enable() ) {
				user_sync_msg.event		= E_USER_SYNC_EVENT_SAVE_YUV;
				user_sync_msg.frame_no	= frame_no;
				user_sync_msg.param		= E_SHARE_YUV_OUT_USER_0; // Normal view YUV
				os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
				UPRINTF_ERR(os_err);
			}
		}

		switch ( g_snapshot_type ) {
		case E_USER_SNAP_YUV:
			user_sync_msg.event 	= E_USER_SYNC_EVENT_SAVE_YUV;
			break;
		case E_USER_SNAP_JPG:
			yuv_size_byte = g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].h_size * g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].v_size;
			if (g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].format == E_SHARE_YUV_FORMAT_422_SEMI) {
				yuv_size_byte *= 2;
			} else {
				yuv_size_byte = yuv_size_byte * 3 / 2;
			}
			ercd = User_Sync_Create_Ring(D_JPEG_RING_AREA_SIZE, yuv_size_byte, g_system_config.yuv_jpeg_ratio);
			if (ercd) g_snapshot_type = E_USER_SNAP_NONE;
			user_sync_msg.event 	= E_USER_SYNC_EVENT_SAVE_JPG;
			snap_param				= E_NORMAL_JPEG;
			break;
		default:
			break;
		}
		if( g_snapshot_type ) {
			user_sync_msg.param		= snap_param;
			user_sync_msg.frame_no	= frame_no;
			os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
			UPRINTF_ERR(os_err);
			g_snapshot_type = E_USER_SNAP_NONE;
		}
		break;
	case E_USER_EVENT_SAVE_YUV:
		g_snapshot_type = E_USER_SNAP_YUV;
		snap_param = (int)frame_no;
		break;
	case E_USER_EVENT_SAVE_JPG:
		g_snapshot_type = E_USER_SNAP_JPG;
		snap_param = (int)frame_no;
		break;
	case E_USER_EVENT_JPG_ENC_END:
		user_sync_msg.event		= E_USER_SYNC_EVENT_JPG_ENC_END;
		user_sync_msg.frame_no	= frame_no;
		os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
		UPRINTF_ERR(os_err);
		break;
	case E_USER_EVENT_JPG_SAVE_END:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_SAVE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "JPEG Save"));
		if (User_Sync_Is_Ring_Created()) User_Sync_Destroy_Ring();
		g_user_yuvin_view_op_state = E_USER_OP_CAP_JPEG_DONE;
		break;
	case E_USER_EVENT_RECORD_START:
		if( g_user_yuvin_mng.rec_state == E_USER_REC_STATE_IDLE ){
			user_sync_msg.event		= E_USER_SYNC_EVENT_RECORD_START;
			user_sync_msg.frame_no	= frame_no;
			os_err = Snd_Msg_To_User_Sync(&user_sync_msg); UPRINTF_ERR(os_err);
			g_user_yuvin_mng.rec_state = E_USER_REC_STATE_RUN;
			g_user_yuvin_view_op_state = E_USER_OP_REC_RUN;
			UPRINTF("rec state is RUN\n" );
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "rec state is RUN"));
		}
		break;
	case E_USER_EVENT_RECORD_STOP:
		if( g_user_yuvin_mng.rec_state != E_USER_REC_STATE_RUN ){
			break;
		}
		g_user_yuvin_mng.rec_state = E_USER_REC_STATE_WAIT_STOP;
		UPRINTF("rec state is WAIT_STOP\n" );
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "rec state is WAIT_STOP"));
		user_sync_msg.event		= E_USER_SYNC_EVENT_RECORD_STOP;
		user_sync_msg.frame_no	= frame_no;
		os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
		UPRINTF_ERR(os_err)
		break;
	case E_USER_EVENT_RECORD_H265_END:
	case E_USER_EVENT_RECORD_H264_0_END:
	case E_USER_EVENT_RECORD_H264_1_END:
	case E_USER_EVENT_RECORD_AUDIO_0_END:
	case E_USER_EVENT_RECORD_AUDIO_1_END:
		if( event == E_USER_EVENT_RECORD_H265_END )		g_user_yuvin_mng.rec_end[E_SHARE_YUV_OUT_H265] = TRUE;
		if( event == E_USER_EVENT_RECORD_H264_0_END )	g_user_yuvin_mng.rec_end[E_SHARE_YUV_OUT_H264_0] = TRUE;
		if( event == E_USER_EVENT_RECORD_H264_1_END )	g_user_yuvin_mng.rec_end[E_SHARE_YUV_OUT_H264_1] = TRUE;
		if( event == E_USER_EVENT_RECORD_AUDIO_0_END )	g_user_yuvin_mng.rec_end[D_AUDIO_REC_END_INDEX_0] = TRUE;
		if( event == E_USER_EVENT_RECORD_AUDIO_1_END )	g_user_yuvin_mng.rec_end[D_AUDIO_REC_END_INDEX_1] = TRUE;
		
		if( (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable   == g_user_yuvin_mng.rec_end[E_SHARE_YUV_OUT_H265])   && 
		    (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == g_user_yuvin_mng.rec_end[E_SHARE_YUV_OUT_H264_0]) && 
		    (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == g_user_yuvin_mng.rec_end[E_SHARE_YUV_OUT_H264_1]) &&
		    (g_movie_rec_cfg.audio[0].enable == g_user_yuvin_mng.rec_end[D_AUDIO_REC_END_INDEX_0]) &&
			(g_movie_rec_cfg.audio[1].enable == g_user_yuvin_mng.rec_end[D_AUDIO_REC_END_INDEX_1]) &&
		    (g_user_yuvin_mng.rec_state == E_USER_REC_STATE_WAIT_STOP ) ) {
			g_user_yuvin_mng.rec_state = E_USER_REC_STATE_IDLE;
			g_user_yuvin_view_op_state = E_USER_OP_REC_STOP;
			UPRINTF("rec state is IDLE\n" );
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "rec state is IDLE"));
			if (g_pre_op_idle_mode) {
				Snd_Event_to_User(E_USER_EVENT_STANDBY, 0);
				g_pre_op_idle_mode = FALSE;
			}
		}
		break;
	case E_USER_EVENT_FD_START:
		Get_Frame_Share_SGDE_YUV( &g_share_sgde_yuv, frame_no );
		yuv_layout = &g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0];
		ercd = User_Utility_FD_Start(yuv_layout);
		if( ercd) {
			break;
		}
		g_user_yuvin_view_op_state = E_USER_OP_FD_START_DONE;
		Get_Share_System_Config(&g_system_config);
		g_system_config.face_detection = TRUE;
		Set_Share_System_Config( &g_system_config, E_SHARE_SYSTEM_MODE_VIEW, (T_SHARE_ENTRY)system_config_callback );
		break;
	case E_USER_EVENT_FD_STOP:
		ercd = User_Utility_FD_Stop(g_system_config.disp_if == E_SHARE_DISP_IF_HDMI ? E_SHARE_DISP_IF_HDMI : E_SHARE_DISP_IF_LCD);
		if( ercd) {
			break;
		}
		g_user_yuvin_view_op_state = E_USER_OP_FD_STOP_DONE;
		Get_Share_System_Config(&g_system_config);
		g_system_config.face_detection = FALSE;
		Set_Share_System_Config( &g_system_config, E_SHARE_SYSTEM_MODE_VIEW, (T_SHARE_ENTRY)system_config_callback );
		if (g_pre_op_idle_mode) {
			Snd_Event_to_User(E_USER_EVENT_STANDBY, 0);
			g_pre_op_idle_mode = FALSE;
		}
		break;
	case E_USER_EVENT_PD_START:
		Get_Frame_Share_SGDE_YUV( &g_share_sgde_yuv, frame_no );
		yuv_layout = &g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0];
		ercd = User_Utility_PD_Start(yuv_layout);
		if( ercd) {
			break;
		}
		g_user_yuvin_view_op_state = E_USER_OP_PD_START_DONE;
		Get_Share_System_Config(&g_system_config);
		g_system_config.person_detection = TRUE;
		Set_Share_System_Config( &g_system_config, E_SHARE_SYSTEM_MODE_VIEW, (T_SHARE_ENTRY)system_config_callback );
		break;
	case E_USER_EVENT_PD_STOP:
		ercd = User_Utility_PD_Stop(g_system_config.disp_if == E_SHARE_DISP_IF_HDMI ? E_SHARE_DISP_IF_HDMI : E_SHARE_DISP_IF_LCD);
		if( ercd) {
			break;
		}
		g_user_yuvin_view_op_state = E_USER_OP_PD_STOP_DONE;
		Get_Share_System_Config(&g_system_config);
		g_system_config.person_detection = FALSE;
		Set_Share_System_Config( &g_system_config, E_SHARE_SYSTEM_MODE_VIEW, (T_SHARE_ENTRY)system_config_callback );
		if (g_pre_op_idle_mode) {
			Snd_Event_to_User(E_USER_EVENT_STANDBY, 0);
			g_pre_op_idle_mode = FALSE;
		}
		break;
	case E_USER_EVENT_IPCU_STREAMING_START_STOP:
		g_ipcu_streaming = (BOOL)frame_no;
		if (g_ipcu_streaming && User_Utility_is_IPCU_JPEG_Enable()) {
			yuv_size_byte = g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].h_size * g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].v_size;
			if (g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].format == E_SHARE_YUV_FORMAT_422_SEMI) {
				yuv_size_byte *= 2;
			} else {
				yuv_size_byte = yuv_size_byte * 3 / 2;
			}

			ercd = User_Sync_Create_Ring(D_JPEG_RING_AREA_SIZE, yuv_size_byte, g_system_config.yuv_jpeg_ratio);
			if (ercd)
				g_ipcu_streaming = FALSE;
		} else if (g_ipcu_streaming == 0 && User_Sync_Is_Ring_Created()) {
			User_Sync_Destroy_Ring();
		}
		break;
	case E_USER_EVENT_SAVE_YUV_END:
		g_user_yuvin_view_op_state = E_USER_OP_SAVE_YUV_DONE;
		break;
	default:
		break;
	}

	return g_user_yuvin_view_state;
}

E_USER_STATE User_YUVin_VIew_Get_Stat(VOID)
{
	return g_user_yuvin_view_state;
}

E_USER_OP_STATE User_YUVin_View_Get_OP_Stat(VOID)
{
	return g_user_yuvin_view_op_state;
}

VOID User_YUVin_View_Clear_OP_Stat(VOID)
{
	g_user_yuvin_view_op_state = E_USER_OP_NONE;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static INT32 system_config_callback( T_SHARE_SYSTEM_CONFIG_RETURN* ret )
{
	FLOAT		view_fps, disp_fps;
	E_SDRAM_CH	disp_ch;
	INT32		rval = 0;

	g_system_config_ret = *ret;

	Set_Share_Available_Memory( g_system_config_ret.available_memory_addr_ch0, g_system_config.end_available_memory_addr_ch0 );
	Set_Share_Available_Memory( g_system_config_ret.available_memory_addr_ch1, g_system_config.end_available_memory_addr_ch1 );

	User_Utility_MM_Init();
	User_Utility_MM_addblock((void*)Get_Share_Available_Memory(0), g_system_config.end_available_memory_addr_ch0 - Get_Share_Available_Memory(0) + 1, 0);
	User_Utility_MM_addblock((void*)Get_Share_Available_Memory(1), g_system_config.end_available_memory_addr_ch1 - Get_Share_Available_Memory(1) + 1, 1);

	OS_IF_Rtos_UVC_Init();
	
	// Display FPS
	view_fps = (float)g_movie_rec_cfg.base_fps / 100.0;
	disp_fps = hdmi_get_video_id_fps();
	if( disp_fps == 0 ){
		disp_fps = 30.0; // fail safe
	}
	// Display Size
	switch( g_system_config.op_ipu_mode )
	{
	case E_SHARE_OP_IPU_MODE_PIPE_ONLY:
		disp_ch = g_system_config_ret.bw_yuv_ch[E_SHARE_YUV_OUT_USER_0];
		g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_DISP_IMAGE ]  = g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_PIPE0_R2Y_YYWCH0 ]
																		+ g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_PIPE0_CNR_WRITE ]
																		+ g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_PIPE1_R2Y_YYWCH0 ]
																		+ g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_PIPE1_CNR_WRITE ];
	break;
	case E_SHARE_OP_IPU_MODE_IPU_ONLY:
		disp_ch = g_system_config_ret.bw_yuv_ch[E_SHARE_YUV_OUT_USER_0];
		g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_DISP_IMAGE ] = g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_IPU_SGDE_W_MAIN ];
	break;
	default:
		disp_ch = g_system_config_ret.bw_yuv_ch[E_SHARE_YUV_OUT_USER_1];
		g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_DISP_IMAGE ] = g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_SHDR_W_RBK5 ];
	break;
	}
	
	if( g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_DISP_IMAGE ] == 0 ){
		g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_DISP_IMAGE ] = 1920 * 1080 * 1.5; // fail safe
	}
	
	// Arb
	Set_Arb_Auto( &(g_system_config_ret.bw_byte[E_SDRAM_CH0][0]),
				  &(g_system_config_ret.bw_byte[E_SDRAM_CH1][0]),
				  view_fps,
				  disp_fps,
				  FALSE,//g_system_config_ret.pipe_is_direct,
				  FALSE ); // print

	return rval;
}

E_FJ_MOVIE_VIDEO_SIZE system_config_convert_video_size( ULONG width, ULONG lines )
{
	E_FJ_MOVIE_VIDEO_SIZE video_size = E_FJ_MOVIE_VIDEO_SIZE_3840_2160;

	if( ( width == 4000 ) && ( lines == 3000 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_4000_3000;
	}
	else if( ( width == 4096 ) && ( lines == 2304 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_4096_2304;
	}
	else if( ( width == 4096 ) && ( lines == 2160 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_4096_2160;
	}
	else if( ( width == 4096 ) && ( lines == 2048 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_4096_2048;
	}
	else if( ( width == 3840 ) && ( lines == 2160 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_3840_2160;
	}
	else if( ( width == 3840 ) && ( lines == 1920 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_3840_1920;
	}
	else if( ( width == 3000 ) && ( lines == 3000 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_3000_3000;
	}
	else if( ( width == 2704 ) && ( lines == 2028 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_2704_2028;
	}
	else if( ( width == 2704 ) && ( lines == 1520 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_2704_1520;
	}
	else if( ( width == 2560 ) && ( lines == 1920 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_2560_1920;
	}
	else if( ( width == 2560 ) && ( lines == 1440 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_2560_1440;
	}
	else if( ( width == 1920 ) && ( lines == 1440 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_1920_1440;
	}
	else if( ( width == 1920 ) && ( lines == 1080 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_1920_1080;
	}
	else if( ( width == 1920 ) && ( lines == 960 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_1920_960;
	}
	else if( ( width == 1504 ) && ( lines == 1504 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_1504_1504;
	}
	else if( ( width == 1440 ) && ( lines == 1080 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_1440_1080;
	}
	else if( ( width == 1440 ) && ( lines == 720 ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_1440_720;
	}
	else if( ( width == 1280 ) && ( lines == 960  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_1280_960;
	}
	else if( ( width == 1280 ) && ( lines == 720  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_1280_720;
	}
	else if( ( width == 960  ) && ( lines == 540  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_960_540;
	}
	else if( ( width == 960  ) && ( lines == 480  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_960_480;
	}
	else if( ( width == 800  ) && ( lines == 600  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_800_600;
	}
	else if( ( width == 864  ) && ( lines == 480  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_864_480;
	}
	else if( ( width == 848  ) && ( lines == 480  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_848_480;
	}
	else if( ( width == 720  ) && ( lines == 480  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_720_480;
	}
	else if( ( width == 640  ) && ( lines == 480  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_640_480;
	}
	else if( ( width == 640  ) && ( lines == 360  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_640_360;
	}
	else if( ( width == 480  ) && ( lines == 270  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_480_270;
	}
	else if( ( width == 432  ) && ( lines == 240  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_432_240;
	}
	else if( ( width == 320  ) && ( lines == 240  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_320_240;
	}
	else if( ( width == 720  ) && ( lines == 576  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_720_576;
	}
	else if( ( width == 1024  ) && ( lines == 768  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_1024_768;
	}
	else if( ( width == 1280  ) && ( lines == 768  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_1280_768;
	}
	else if( ( width == 1280  ) && ( lines == 1024  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_1280_1024;
	}
	else if( ( width == 1600  ) && ( lines == 896  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_1600_896;
	}
	else if( ( width == 1024  ) && ( lines == 576  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_1024_576;
	}
	else if( ( width == 800  ) && ( lines == 448  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_800_448;
	}
	else if( ( width == 352  ) && ( lines == 288  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_352_288;
	}
	else if( ( width == 2592  ) && ( lines == 1944  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_2592_1944;
	}
	else if( ( width == 2304  ) && ( lines == 1296  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_2304_1296;
	}
	else if( ( width == 1920  ) && ( lines == 1280  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_1920_1280;
	}
	else if( ( width == 640  ) && ( lines == 512  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_640_512;
	}
	else if( ( width == 384  ) && ( lines == 288  ) ) {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_384_288;
	}
	else {
		video_size = E_FJ_MOVIE_VIDEO_SIZE_3840_2160;
	}

	return video_size;
}


