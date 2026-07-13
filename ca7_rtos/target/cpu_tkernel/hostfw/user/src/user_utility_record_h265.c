/**
 * @file		user_utility.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "user.h"
#include "user_sync_task.h"
#include "user_record_task.h"
#include "category_parameter.h"
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
#include "h265_user.h"
#include "os_if.h"
#include <stdlib.h>

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
//#define DBG_H265_SEQUENCE_MBA

#define D_RS_MUX_STARTCODE_SIZE			( 4 )
#define D_RS_MUX_STARTCODE				( 0x00000001 )
#define D_RS_MUX_NAL_TYPE_SPS			( 7 )
#define D_RS_MUX_NAL_TYPE_PPS			( 8 )
#define D_RS_MOVIE_H265_CACHE_FPS_MAX	( 60 )		// fps
#define D_RS_MOVIE_H265_CACHE_TIME_MAX	( 5 )		// second
#define D_RS_MOVIE_H265_CACHE_NUM		( D_RS_MOVIE_H265_CACHE_FPS_MAX * D_RS_MOVIE_H265_CACHE_TIME_MAX )		// 60fps * 5s = 300

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

typedef struct{
	ULONG	nal_address;
	UINT32	nal_size;
} T_RS_MOVIE_MUX_NAL_CONTROL_TBL;

typedef struct{
	DOUBLE init_ms;
	DOUBLE frame_ms;
	ULONG frame_cnt;
} T_RS_MOVIE_TIMELAPSE_INFO;

typedef struct{
	T_FJ_MOVIE_H265_STREAM_PARAM stream_param[D_RS_MOVIE_H265_CACHE_NUM];
	ULONG sample_addr[D_RS_MOVIE_H265_CACHE_NUM];
	ULONG sample_size[D_RS_MOVIE_H265_CACHE_NUM];
	ULONG cache_addr;
	ULONG cache_size;
	ULONG cache_sample_num;
	ULONG addr_start;
	ULONG addr_end;
	ULONG write_point;
	ULONG read_point;
	ULONG sample_num;
} T_RS_MOVIE_H265_CACHE_INFO;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static BOOL g_is_record[E_SHARE_YUV_OUT_H264_0 + 1] = {FALSE, FALSE}; // need exclusive control !!!!
//static BOOL g_is_get_addr[E_SHARE_YUV_OUT_H264_1 + 1] = {FALSE, FALSE}; // need exclusive control !!!!
static BOOL g_decide_first[E_SHARE_YUV_OUT_H264_0 + 1] = {FALSE, FALSE};

static E_VIDEO_ENOCDE_ID g_enc_id[E_SHARE_YUV_OUT_H264_0 + 1];
static E_FJ_VIDEO_ENC_ENUM g_enc_enum[E_SHARE_YUV_OUT_H264_0 + 1];

static CHAR g_file_path[256];
static ULONG g_file_id[E_SHARE_YUV_OUT_H264_0 + 1];

T_FJ_MUX_H265_PS_INFO g_ps_info[E_SHARE_YUV_OUT_H264_0 + 1];
static BOOL g_is_set_ps[E_SHARE_YUV_OUT_H264_0 + 1] = {FALSE, FALSE};
static BOOL g_dummy_flg[E_SHARE_YUV_OUT_H264_0 + 1] = {FALSE, FALSE};
static ULONG g_TimeDelta[E_SHARE_YUV_OUT_H264_0 + 1] = {0, 0};
static BOOL g_comp[E_SHARE_YUV_OUT_H264_0 + 1] = {FALSE, FALSE};

static UINT8 g_frame_div[E_SHARE_YUV_OUT_H264_0 + 1] = {1, 1};

static T_RS_MOVIE_TIMELAPSE_INFO gRec_TimeLapse[E_SHARE_YUV_OUT_H264_0 + 1] = {{0, 0, 0}, {0, 0, 0}};

static ULLONG g_max_frame_cnt[E_SHARE_YUV_OUT_H264_0 + 1];
static ULLONG g_cur_frame_cnt[E_SHARE_YUV_OUT_H264_0 + 1];
static INT32 g_mux_status[E_SHARE_YUV_OUT_H264_0 + 1];
static UCHAR g_gop_num[E_SHARE_YUV_OUT_H264_0 + 1];
static UINT8 g_stream_format[E_SHARE_YUV_OUT_H264_0 + 1];
static ULONG g_fps_ti[E_SHARE_YUV_OUT_H264_0 + 1];
static ULONG g_fps_ts[E_SHARE_YUV_OUT_H264_0 + 1];
static UINT8 g_gop_struct[E_SHARE_YUV_OUT_H264_0 + 1];

static BOOL g_is_cache_record[E_SHARE_YUV_OUT_H264_0 + 1] = {FALSE, FALSE};
static BOOL g_is_cache_first_frame[E_SHARE_YUV_OUT_H264_0 + 1] = {FALSE, FALSE};
static T_RS_MOVIE_H265_CACHE_INFO g_rec_cache_info[E_SHARE_YUV_OUT_H264_0 + 1];

static T_FJ_MOVIE_H265_DYNAMIC_CONFIG_PARAM gRec_DynamicConfig[E_SHARE_YUV_OUT_H264_0 + 1] ={{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/

static INT32 user_utility_record_h265_start( T_SHARE_VIDEO_REC_CONFIG * cfg, ULONG base_fps, E_SHARE_YUV_OUT stream_idx );
static INT32 user_utility_record_h265_stream_cb( UCHAR video_enc_id, T_FJ_MOVIE_H265_STREAM_PARAM * stream_info, E_SHARE_YUV_OUT	stream_idx );
static INT32 user_utility_record_h265_end( UCHAR stream_idx );

static INT32 user_utility_record_h265_cache_start( T_SHARE_VIDEO_REC_CONFIG * cfg, ULONG base_fps, E_SHARE_YUV_OUT	stream_idx );
static INT32 user_utility_record_h265_cache_stream_cb( UCHAR video_enc_id, T_FJ_MOVIE_H265_STREAM_PARAM * stream_info, E_SHARE_YUV_OUT	stream_idx );
static INT32 user_utility_record_h265_cache_end( UCHAR stream_idx );

static INT32 user_utility_record_h265_cache_malloc( UCHAR stream_idx, ULONG * sample_addr, ULONG sample_size );
static INT32 user_utility_record_h265_cache_free( UCHAR stream_idx, ULONG sample_addr, ULONG sample_size );

static VOID user_utility_record_h265_stream_edit( T_FJ_MOVIE_H265_STREAM_PARAM * stream_param, T_FJ_MUX_H265_PS_INFO * ps_info );
static VOID user_utility_record_h265_psinfo_save( T_FJ_MUX_H265_PS_INFO * ps_info_src, T_FJ_MUX_H265_PS_INFO * ps_info_dst );
static VOID user_utility_record_h265_psinfo_free( T_FJ_MUX_H265_PS_INFO * ps_info );
static INT32 user_utility_record_h265_change_file( UCHAR stream_idx, UCHAR video_enc_id );
static int round( float x );

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

INT32 User_Utility_Record_H265_Start( T_SHARE_VIDEO_REC_CONFIG* cfg, ULONG base_fps, E_SHARE_YUV_OUT stream_idx )
{
	INT32 ret;
	T_FJ_MUX_VIDEO_CONFIG * mux_config = NULL;

	if( cfg->stream_format == FJ_MOVIE_STREAM_FORMAT_ES && FALSE == User_Utility_Record_Mux_Check_Media_Size() ) {
		return NG;
	}

	if( g_is_cache_record[stream_idx] == FALSE ) {
		ret = user_utility_record_h265_start( cfg, base_fps, stream_idx );
		UPRINTF_ERR(ret);
	}

	if( cfg->stream_format == FJ_MOVIE_STREAM_FORMAT_ES ) {
		// File
		ret = FJ_Dcf_Make_New_File_Path_By_Ext( "MP4", g_file_path );
		UPRINTF_ERR(ret);

		g_file_id[stream_idx] = FJ_Fs_Open( g_file_path, ( FJ_FS_OPEN_RDWR|FJ_FS_OPEN_CREAT|FJ_FS_OPEN_TRUNC ) );

		ret = FJ_Dcf_Update_DcfDb_Add_File( g_file_path );
		UPRINTF_ERR(ret);

		// MUX
		// video
		mux_config = User_Utility_Record_Mux_Get_Config();
		mux_config->enable = TRUE;
		mux_config->file_id = g_file_id[stream_idx];
		mux_config->pre_file_id = mux_config->file_id;
		mux_config->stream_id = g_enc_id[stream_idx];
		mux_config->codec_type = FJ_MOVIE_CODEC_H265;
		mux_config->image_size = cfg->image_size;
		mux_config->display_size = cfg->image_size;
		mux_config->bitrate = cfg->bitrate;
		mux_config->time_increment = g_fps_ti[stream_idx];
		mux_config->time_scale = g_fps_ts[stream_idx];
		mux_config->gop_struct = g_gop_struct[stream_idx];
		mux_config->frame_num_of_gop = g_gop_num[stream_idx];
		mux_config->max_time = 7200;
		mux_config->max_size = 0xFFFFFFFF;
		mux_config->compatible_brand_count = 2;
		memcpy( &mux_config->compatible_brand[0], ( VOID * )"mp42", 4 );
		memcpy( &mux_config->compatible_brand[1], ( VOID * )"hvc1", 4 );
	}
	else if( cfg->stream_format == FJ_MOVIE_STREAM_FORMAT_ES_HEADER ) {
	}

	g_mux_status[stream_idx] = FJ_OK_MP4_COMMON_OK;
	g_is_record[stream_idx] = TRUE;
	g_decide_first[stream_idx] = TRUE;

	if( cfg->stream_format == FJ_MOVIE_STREAM_FORMAT_ES_HEADER ) {
		ret = FJ_VideoEncoder_Start( ( UCHAR )g_enc_id[stream_idx] );
		UPRINTF_ERR(ret);

		MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H265, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "BF_Enc" ) );
		//ret = BF_Encoder_Start( ( E_VIDEO_ENOCDE_ID )g_enc_id[stream_idx], cfg->low_latency);
		ret = BF_Encoder_Start( ( E_VIDEO_ENOCDE_ID )g_enc_id[stream_idx], TRUE);
		UPRINTF_ERR(ret);
		MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H265, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "" ) );
	}

	return OK;
}

INT32 User_Utility_Record_H265_Get_Addr( T_BF_ENCODER_GET_IMAGE_ADDR_PARAM * addr_param, UCHAR stream_idx, ULLONG frame_no )
{
	INT32 ret;
	DOUBLE lapse_time = 0;

	#ifdef DBG_H265_SEQUENCE_MBA
	if(stream_idx == E_SHARE_YUV_OUT_H265)
		MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H265, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "H265_Get([%d]: %ld )", stream_idx, ( ULONG )frame_no ) );
	else
		MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H264_0, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "H265_Get([%d]: %ld )", stream_idx, ( ULONG )frame_no ) );
	#endif

	if( g_is_record[stream_idx] == FALSE && g_is_cache_record[stream_idx] == FALSE ) return NG;

	if( ( frame_no % g_frame_div[stream_idx] ) != 0 ) return NG;

	if( gRec_TimeLapse[stream_idx].frame_ms != 0 ) {
		lapse_time = gRec_TimeLapse[stream_idx].frame_ms * gRec_TimeLapse[stream_idx].frame_cnt;

		if( ( lapse_time >= gRec_TimeLapse[stream_idx].init_ms ) || ( lapse_time == 0 ) ) {
			BF_H265_User_Record_Frame_Enabled( g_enc_id[stream_idx], 0, FALSE, TRUE ); // frame no meaningless?
			gRec_TimeLapse[stream_idx].frame_cnt = 0;
		}
		else {
			BF_H265_User_Record_Frame_Enabled( g_enc_id[stream_idx], 0, FALSE, FALSE ); // frame no meaningless?
		}

		gRec_TimeLapse[stream_idx].frame_cnt++;
	}
	else {
		BF_H265_User_Record_Frame_Enabled( g_enc_id[stream_idx], 0, FALSE, TRUE ); // frame no meaningless?
	}
#if 0
	if( !g_is_get_addr ) {
		g_is_get_addr = TRUE;

		ret = BF_Encoder_Get_Image_Address( g_enc_id, E_FJ_MOVIE_VIDEO_SIZE_4096_2160, frame_no, addr_param );
		UPRINTF_ERR(ret);

		g_dummy_flg = addr_param->dummy_flg;

		if( !g_comp ) {
			addr_param->y_cmp_addr = 0;
			addr_param->c_cmp_addr = 0;
		}
	}
	else {
		ret = NG;
	}
#else
	ret = BF_Encoder_Get_Image_Address( g_enc_id[stream_idx], E_FJ_MOVIE_VIDEO_SIZE_4096_2160, frame_no, addr_param );
	UPRINTF_ERR(ret);

	g_dummy_flg[stream_idx] = addr_param->dummy_flg;
	BF_Encoder_Decide_Image_Address( g_enc_id[stream_idx], frame_no, g_dummy_flg[stream_idx] );

	if( !g_comp[stream_idx] ) {
		addr_param->y_cmp_addr = 0;
		addr_param->c_cmp_addr = 0;
	}
#endif
	return ret;
}
#if 0
INT32 User_Utility_Record_H265_Rel_Addr( ULLONG frame_no )
{
	INT32 ret;

#ifdef DBG_H265_SEQUENCE_MBA
	MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H265, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "H265_Rel( %ld )", ( ULONG )frame_no ) );
#endif

	if( g_is_get_addr ) {
		g_is_get_addr = FALSE;
		BF_Encoder_Decide_Image_Address( g_enc_id, frame_no, g_dummy_flg );
	}
	else {
		ret = NG;
	}

	return ret;
}
#endif


INT32 User_Utility_Record_H265_Get_Addr2( T_BF_ENCODER_GET_IMAGE_ADDR_PARAM * addr_param, UCHAR stream_idx, ULLONG frame_no )
{
	INT32 ret;
	stream_idx = E_SHARE_YUV_OUT_H265; // fix;
	
	if( g_is_record[stream_idx] == FALSE && g_is_cache_record[stream_idx] == FALSE ) return NG;
	if( ( frame_no % g_frame_div[stream_idx] ) != 0 ) return NG;
	
	BF_H265_User_Record_Frame_Enabled( g_enc_id[stream_idx], 0, FALSE, TRUE ); // frame no meaningless?
	ret = BF_Encoder_Get_Image_Address( g_enc_id[stream_idx], E_FJ_MOVIE_VIDEO_SIZE_4096_2160, frame_no, addr_param );
	UPRINTF_ERR(ret);
	
	#ifdef DBG_H265_SEQUENCE_MBA
	if(stream_idx == E_SHARE_YUV_OUT_H265)
		MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H265, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "H265_Get([%d]: %lx, %lx )", stream_idx, ( ULONG )frame_no, addr_param->y_addr ));
	else
		MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H264_0, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "H265_Get([%d]: %lx, %lx )", stream_idx, ( ULONG )frame_no, addr_param->y_addr ));
	#endif

	if( !g_comp[stream_idx] ) {
		addr_param->y_cmp_addr = 0;
		addr_param->c_cmp_addr = 0;
	}
	
	return ret;
}

INT32 User_Utility_Record_H265_Rel_Addr2( BOOL dummy_flg, ULLONG frame_no )
{
	INT32 ret;
	UCHAR stream_idx = E_SHARE_YUV_OUT_H265; // fix;

	if( g_is_record[stream_idx] == FALSE && g_is_cache_record[stream_idx] == FALSE ) return NG;
	if( ( frame_no % g_frame_div[stream_idx] ) != 0 ) return NG;
	
	#ifdef DBG_H265_SEQUENCE_MBA
	if( !dummy_flg ){
		if(stream_idx == E_SHARE_YUV_OUT_H265)
			MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H265, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "H265_Rel( %lx )", ( ULONG )frame_no ) );
		else
			MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H264_0, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "H265_Rel( %lx )", ( ULONG )frame_no ) );
	}
	#endif

	if( !dummy_flg && g_decide_first[stream_idx] ){
		BF_Movie_Common_Sync_Start_H265( 0 ); // skip "first_flag" and "first_delay_count" in H265_Encode_CC_Main()
		g_decide_first[stream_idx] = FALSE;
		UPRINTF( "Sync Start Forcely\n" );
	}
	ret = BF_Encoder_Decide_Image_Address( g_enc_id[stream_idx], frame_no, dummy_flg );
	
	return ret;
}


INT32 User_Utility_Record_H265_SequenceStartCB( UCHAR video_enc_id, INT32 *vop_cnt)
{
	INT32 ret = 0;
	E_SHARE_YUV_OUT	stream_idx = E_SHARE_YUV_OUT_H265;

	if( video_enc_id == E_VIDEO_ENOCDE_ID_H265_CH0 ) {
		if( g_is_record[E_SHARE_YUV_OUT_H265] ) {
			stream_idx = E_SHARE_YUV_OUT_H265;
		}
		else{
			stream_idx = E_SHARE_YUV_OUT_H264_0;
		}
	}
	else if( video_enc_id == E_VIDEO_ENOCDE_ID_H265_CH1 ) {
		stream_idx = E_SHARE_YUV_OUT_H264_0;
	}

	if(Cate_Get_MovieRec_H265_DynamicConfig(stream_idx, &gRec_DynamicConfig[stream_idx]) == TRUE){
		UPRINTF( "[H265 %d] Dynamic Update [bitrate:%d]\n", stream_idx, gRec_DynamicConfig[stream_idx].bitrate );
		UPRINTF( "[H265 %d] Dynamic Update [frame_num_of_gop:%d]\n", stream_idx, gRec_DynamicConfig[stream_idx].frame_num_of_gop );
		FJ_VideoEncoder_ConfigDynamic(video_enc_id, FJ_MOVIE_H265_CFG, (UINT32)&gRec_DynamicConfig[stream_idx]);
		*vop_cnt = 1;
	}

	return ret;
}

INT32 User_Utility_Record_H265_StreamCB( UCHAR video_enc_id, T_FJ_MOVIE_H265_STREAM_PARAM * stream_info )
{
	INT32 ret = 0;
	T_USER_RECORD_MSG user_record_msg;
	E_SHARE_YUV_OUT	stream_idx = E_SHARE_YUV_OUT_H265;
	E_USER_EVENT event_free_index = E_USER_RECORD_EVENT_RECORD_H265_FREE_INDEX_0;

	if( video_enc_id == E_VIDEO_ENOCDE_ID_H265_CH0 ) {
		if( g_is_record[E_SHARE_YUV_OUT_H265] ) {
			event_free_index = E_USER_RECORD_EVENT_RECORD_H265_FREE_INDEX_0;
			stream_idx = E_SHARE_YUV_OUT_H265;
		}
		else{
			event_free_index = E_USER_RECORD_EVENT_RECORD_H264_FREE_INDEX_0;
			stream_idx = E_SHARE_YUV_OUT_H264_0;
		}
	}
	else if( video_enc_id == E_VIDEO_ENOCDE_ID_H265_CH1 ) {
		event_free_index = E_USER_RECORD_EVENT_RECORD_H264_FREE_INDEX_0;
		stream_idx = E_SHARE_YUV_OUT_H264_0;
	}

	user_record_msg.event = event_free_index;
	user_record_msg.param = g_enc_enum[stream_idx];
	user_record_msg.stream.h265 = *stream_info;

	ret = Snd_Msg_To_User_Record( &user_record_msg );
	UPRINTF_ERR(ret);

	return ret;
}

INT32 User_Utility_Record_H265_StreamCB_Sync( UCHAR stream_idx, T_FJ_MOVIE_H265_STREAM_PARAM * stream_info )
{
	INT32 ret = 0;

	if( g_is_cache_record[stream_idx] == FALSE ) {
		ret = user_utility_record_h265_stream_cb( g_enc_id[stream_idx], stream_info, stream_idx );
		UPRINTF_ERR(ret);
	} else {
		ret = user_utility_record_h265_cache_stream_cb( g_enc_id[stream_idx], stream_info, stream_idx );
		UPRINTF_ERR(ret);
	}

	return ret;
}

INT32 User_Utility_Record_H265_Free_Index( UCHAR stream_idx, UINT32 index, UCHAR last)
{
	INT32 ret = NG;

	#ifdef DBG_H265_SEQUENCE_MBA
	if(stream_idx == E_SHARE_YUV_OUT_H265)
		MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H265, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "H265_Free([%d]: %d, %d )", stream_idx, index, last ));
	else
		MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H264_0, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "H265_Free([%d]: %d, %d )", stream_idx, index, last ));
	#endif

	if( g_is_record[stream_idx] || g_is_cache_record[stream_idx] ) {
		ret = FJ_VideoEncoder_FreeIndex( g_enc_id[stream_idx], index, last );
		UPRINTF_ERR(ret);

		if( last ) {
			User_Utility_Record_H265_End(stream_idx);
		}
	}

	if( last ) {
		g_is_record[stream_idx] = FALSE;
	}

	return ret;
}

INT32 User_Utility_Record_H265_Stop( UCHAR stream_idx )
{
	INT32 ret;

	if( g_is_record[stream_idx] || g_is_cache_record[stream_idx] ) {
		ret = FJ_VideoEncoder_Stop( ( UCHAR )g_enc_id[stream_idx] );
		UPRINTF_ERR(ret);
	}

	return OK;
}

INT32 User_Utility_Record_H265_End( UCHAR stream_idx )
{
	INT32 ret;

	ret = FJ_VideoEncoder_Close( ( UCHAR )g_enc_id[stream_idx] );
	UPRINTF_ERR(ret);

	

	if( g_is_record[stream_idx] && g_is_cache_record[stream_idx] ) {
		ret = user_utility_record_h265_cache_end(stream_idx);
		UPRINTF_ERR(ret);
	}
	else {
		if( g_is_set_ps[stream_idx] == TRUE ) {
			g_is_set_ps[stream_idx] = FALSE;
			user_utility_record_h265_psinfo_free( &g_ps_info[stream_idx] );
		}

		g_is_cache_record[stream_idx] = FALSE;

		ret = user_utility_record_h265_end(stream_idx);
		UPRINTF_ERR(ret);
	}

	return OK;
}

BOOL User_Utility_Record_H265_Is_Running( VOID )
{
	return ( ( g_is_record[E_SHARE_YUV_OUT_H265] == TRUE )
			|| ( g_is_record[E_SHARE_YUV_OUT_H264_0] == TRUE ) );
}

INT32 User_Utility_Record_H265_Cache_Start( T_SHARE_VIDEO_REC_CONFIG * cfg, ULONG base_fps, E_SHARE_YUV_OUT stream_idx )
{
	INT32 ret;

	if( cfg->stream_format == FJ_MOVIE_STREAM_FORMAT_ES_HEADER ) {
		return NG;
	}

	if( cfg->stream_format == FJ_MOVIE_STREAM_FORMAT_ES && FALSE == User_Utility_Record_Mux_Check_Media_Size() ) {
		return NG;
	}

	ret = user_utility_record_h265_cache_start( cfg, base_fps, stream_idx );
	UPRINTF_ERR(ret);

	ret = user_utility_record_h265_start( cfg, base_fps, stream_idx );
	UPRINTF_ERR(ret);

	g_is_cache_record[stream_idx] = TRUE;
	g_is_cache_first_frame[stream_idx] = FALSE;

	ret = FJ_VideoEncoder_Start( ( UCHAR )g_enc_id[stream_idx] );
	UPRINTF_ERR(ret);

	ret = BF_Encoder_Start( ( E_VIDEO_ENOCDE_ID )g_enc_id[stream_idx], 0 );
	UPRINTF_ERR(ret);

	return OK;
}

INT32 User_Utility_Record_H265_Cache_StreamCB( UCHAR video_enc_id )
{
	INT32 ret = 0;
	T_FJ_MOVIE_H265_STREAM_PARAM * stream_info;
	E_SHARE_YUV_OUT	stream_idx = E_SHARE_YUV_OUT_H265;

	if( video_enc_id == E_VIDEO_ENOCDE_ID_H265_CH0 ) {
		if( g_is_record[E_SHARE_YUV_OUT_H265] ) {
			stream_idx = E_SHARE_YUV_OUT_H265;
		}
		else{
			stream_idx = E_SHARE_YUV_OUT_H264_0;
		}
	}
	else if( video_enc_id == E_VIDEO_ENOCDE_ID_H265_CH1 ) {
		stream_idx = E_SHARE_YUV_OUT_H264_0;
	}

	if( g_is_record[stream_idx] ) {
		if( g_is_cache_first_frame[stream_idx] == TRUE ) {
			g_is_cache_first_frame[stream_idx] = FALSE;

			ret = FJ_Mux_Fixed_Atom_Setting( video_enc_id, NULL, &g_ps_info[stream_idx] );
			UPRINTF_ERR(ret);
		}

		if( g_rec_cache_info[stream_idx].sample_num ) {
			stream_info = &g_rec_cache_info[stream_idx].stream_param[g_rec_cache_info[stream_idx].read_point];

			ret = user_utility_record_h265_stream_cb( video_enc_id, stream_info, stream_idx );
			UPRINTF_ERR(ret);

			ret = user_utility_record_h265_cache_free( stream_idx, g_rec_cache_info[stream_idx].sample_addr[g_rec_cache_info[stream_idx].read_point], g_rec_cache_info[stream_idx].sample_size[g_rec_cache_info[stream_idx].read_point] );
			UPRINTF_ERR(ret);

			g_rec_cache_info[stream_idx].read_point = ( ( g_rec_cache_info[stream_idx].read_point + 1 ) % g_rec_cache_info[stream_idx].cache_sample_num );
			g_rec_cache_info[stream_idx].sample_num -= 1;

			if( stream_info->stream_end_flg ) {
				ret = User_Utility_Record_H265_Cache_End(stream_idx);
				UPRINTF_ERR(ret);
			}
		}
	}

	return OK;
}

INT32 User_Utility_Record_H265_Cache_End( UCHAR stream_idx )
{
	INT32 ret;

	ret = user_utility_record_h265_end(stream_idx);
	UPRINTF_ERR(ret);

	g_is_cache_record[stream_idx] = FALSE;

	return OK;
}

BOOL User_Utility_Record_H265_Is_Cache_Running( VOID )
{
	return ( ( g_is_cache_record[E_SHARE_YUV_OUT_H265] == TRUE )
			|| ( g_is_cache_record[E_SHARE_YUV_OUT_H264_0] == TRUE ) );
}

ULONG User_Utility_Record_H265_Get_Cache_Size( UINT32 bitrate )
{
	return ( ULONG )( ( bitrate / 8.0 * D_RS_MOVIE_H265_CACHE_TIME_MAX ) * ( 1.1 ) );
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/

static INT32 user_utility_record_h265_start( T_SHARE_VIDEO_REC_CONFIG * cfg, ULONG base_fps, E_SHARE_YUV_OUT stream_idx )
{
	INT32 ret;
	T_FJ_MOVIE_H265_CONFIG_PARAM	config_param_h265;
	T_FJ_MOVIE_H265_VUI_PARAM		vui_param_h265;
	T_SHARE_SYSTEM_CONFIG			syscfg;
	UINT32	fps_rec, fps_mux;
	UINT8	lapse_enable = FJ_MOVIE_OFF;
	ULONG	actual_fps = 0;

	memset( &config_param_h265, 0, sizeof( config_param_h265 ) );
	memset( &vui_param_h265, 0, sizeof( vui_param_h265 ) );

	actual_fps = cfg->frame_rate / 100;
	if (cfg->gop_num == 0xFF) {
		if( ( ( UCHAR )actual_fps % 2 ) == 0 )
			g_gop_num[stream_idx] = ( UCHAR )( actual_fps / 2 );
		else
			g_gop_num[stream_idx] = ( UCHAR )round( ( ( float )cfg->frame_rate / 100.0 ) );
	} else {
		g_gop_num[stream_idx] = cfg->gop_num;
	}

	UPRINTF( "\n [H265 %d] Idx:%d Rec fps = %f Base fps = %f GOP number = %d\n", cfg->codec_id - 10, stream_idx, ( float )cfg->frame_rate / 100.0, ( float )base_fps/100.0, g_gop_num[stream_idx] );

	fps_rec = cfg->frame_rate;

	if( ( cfg->slow_rate <= 0 ) || ( cfg->slow_rate >= 1 ) ) {
		fps_mux = fps_rec;
	}
	else {
		fps_mux = ( UINT32 )( cfg->frame_rate * cfg->slow_rate );
	}

	if( ( fps_mux % 100 ) == 0 ) {
		g_fps_ti[stream_idx] = 1000;
		g_fps_ts[stream_idx] = fps_mux * 10;
	}
	else {
		g_fps_ti[stream_idx] = 1001;
		g_fps_ts[stream_idx] = ( fps_mux * g_fps_ti[stream_idx] ) / 1000 + 1;
		g_fps_ts[stream_idx] = g_fps_ts[stream_idx] * 10;
	}

	g_frame_div[stream_idx] = ( UCHAR )( base_fps / cfg->frame_rate );

	if( cfg->stream_format == FJ_MOVIE_STREAM_FORMAT_ES ) {
		gRec_TimeLapse[stream_idx].init_ms = cfg->lapse_period_ms;
	}
	else if( cfg->stream_format == FJ_MOVIE_STREAM_FORMAT_ES_HEADER ) {
		gRec_TimeLapse[stream_idx].init_ms = 0;
	}

	if( gRec_TimeLapse[stream_idx].init_ms == 0 ) {
		gRec_TimeLapse[stream_idx].frame_ms = 0;
		lapse_enable = FJ_MOVIE_OFF;
	}
	else {
		gRec_TimeLapse[stream_idx].frame_ms = ( DOUBLE )1000 / ( ( float )cfg->frame_rate / 100.0 );
		lapse_enable = FJ_MOVIE_ON;
	}

	Get_Share_System_Config( &syscfg );
	// Encoder
#if 1
	vui_param_h265.video_signal_type_present_flag		= 1;
	vui_param_h265.video_format							= 2;	//( NTSC )
	vui_param_h265.video_full_range_flag				= syscfg.yc_range == E_YC_RANGE_FULL ? 1 : 0;
	vui_param_h265.colour_description_present_flag		= 1;
	vui_param_h265.aspect_ratio_info_present_flag		= 1;
	vui_param_h265.aspect_ratio_idc						= 1;
	vui_param_h265.sar_width							= 0;	// ???
	vui_param_h265.sar_height							= 0;	// ???
	vui_param_h265.chroma_loc_info_present_flag			= 0;
	vui_param_h265.chroma_sample_loc_type_top_field		= 0;
	vui_param_h265.chroma_sample_loc_type_bottom_field	= 0;

	if ( syscfg.yc_bt == E_YC_BT_601 || syscfg.yc_bt == E_YC_BT_IQ ) {
		vui_param_h265.colour_primaries					= 6;	//( BT.601 )
		vui_param_h265.transfer_characteristics			= 6;	//( BT.601 )
		vui_param_h265.matrix_coefficients				= 6;	//( BT.601 )
	}
	else if ( syscfg.yc_bt == E_YC_BT_709 ) {
		vui_param_h265.colour_primaries					= 1;	//( BT.709 )
		vui_param_h265.transfer_characteristics			= 1;	//( BT.709 )
		vui_param_h265.matrix_coefficients				= 1;	//( BT.709 )
	}
#endif

	g_enc_enum[stream_idx] = cfg->codec_id;

	if( cfg->stream_format == FJ_MOVIE_STREAM_FORMAT_ES ) {
		config_param_h265.enable_flag			= FJ_MOVIE_ON;
		config_param_h265.image_size			= cfg->image_size;
		config_param_h265.stream_format			= cfg->stream_format;
		config_param_h265.bitrate_algorithm		= cfg->bitrate_algorithm;
		config_param_h265.bitrate				= cfg->bitrate;
		config_param_h265.average_bitrate		= cfg->average_bitrate;
		config_param_h265.frame_rate			= fps_rec;
		config_param_h265.gop_struct			= cfg->gop_struct;
		config_param_h265.frame_num_of_gop		= g_gop_num[stream_idx];
		config_param_h265.closed_gop_flag		= FJ_MOVIE_VIDEO_CLOSED_GOP_OFF;
		config_param_h265.gop_num_idr_to_idr	= 2;
		config_param_h265.level					= FJ_MOVIE_VIDEO_LEVEL_5_1;
		config_param_h265.buffer_ctrl_flag		= FJ_MOVIE_OFF;
		config_param_h265.pictstruct_flag		= FJ_MOVIE_OFF;
		config_param_h265.filler_flag			= FJ_MOVIE_OFF;
		config_param_h265.time_lapse_flag		= lapse_enable;
		config_param_h265.vui_param				= vui_param_h265;
		config_param_h265.slice_encode_flag		= cfg->slice;
		config_param_h265.slice_int_line		= cfg->slice_int_line;
		config_param_h265.oneshot_encode_flag	= FJ_MOVIE_OFF;
		config_param_h265.delay_notify_flag		= !cfg->low_latency;
		
		ret = FJ_VideoEncoder_Open( cfg->codec_id , ( UCHAR * )&g_enc_id[stream_idx] );
		UPRINTF_ERR(ret);
	}
	else if( cfg->stream_format == FJ_MOVIE_STREAM_FORMAT_ES_HEADER ) {
		config_param_h265.enable_flag			= FJ_MOVIE_ON;
		config_param_h265.image_size			= cfg->image_size;
		config_param_h265.stream_format			= cfg->stream_format;
		config_param_h265.bitrate_algorithm		= cfg->bitrate_algorithm;
		config_param_h265.bitrate				= cfg->bitrate;
		config_param_h265.average_bitrate		= cfg->average_bitrate;
		config_param_h265.frame_rate			= fps_rec;

		config_param_h265.frame_num_of_gop		= g_gop_num[stream_idx];
		config_param_h265.closed_gop_flag		= FJ_MOVIE_VIDEO_CLOSED_GOP_OFF;
		config_param_h265.gop_num_idr_to_idr	= 2;
		config_param_h265.gop_struct			= cfg->gop_struct;
		config_param_h265.level					= FJ_MOVIE_VIDEO_LEVEL_5_1;
		config_param_h265.buffer_ctrl_flag		= FJ_MOVIE_OFF;
		config_param_h265.pictstruct_flag		= FJ_MOVIE_OFF;
		config_param_h265.filler_flag			= FJ_MOVIE_OFF;
		config_param_h265.time_lapse_flag		= lapse_enable;
		config_param_h265.vui_param				= vui_param_h265;
		config_param_h265.slice_encode_flag		= cfg->slice;
		config_param_h265.slice_int_line		= cfg->slice_int_line;
		config_param_h265.oneshot_encode_flag	= FJ_MOVIE_OFF;
		config_param_h265.delay_notify_flag		= !cfg->low_latency;

		ret = FJ_VideoEncoder_Open( cfg->codec_id , ( UCHAR * )&g_enc_id[stream_idx] );
		UPRINTF_ERR(ret);
	}

	ret = FJ_VideoEncoder_Config( ( UCHAR )g_enc_id[stream_idx], FJ_MOVIE_H265_CFG, ( UINT32 )&config_param_h265 );
	UPRINTF_ERR(ret);

	gRec_DynamicConfig[stream_idx].bitrate = config_param_h265.bitrate;
	gRec_DynamicConfig[stream_idx].gop_struct = config_param_h265.gop_struct;
	gRec_DynamicConfig[stream_idx].frame_num_of_gop = config_param_h265.frame_num_of_gop;
	gRec_DynamicConfig[stream_idx].closed_gop_flag = config_param_h265.closed_gop_flag;
	gRec_DynamicConfig[stream_idx].gop_num_idr_to_idr = config_param_h265.gop_num_idr_to_idr;

	g_stream_format[stream_idx] = config_param_h265.stream_format;
	g_gop_struct[stream_idx] = config_param_h265.gop_struct;

#ifdef CO_ES3_HARDWARE
	if( config_param_h265.gop_struct == FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP && cfg->compress ) {
		// not support H265 + ES3 + IBBP + COMP
		g_comp[stream_idx] = FALSE;
		FJ_VideoEncoder_CompEnable( ( UCHAR )g_enc_id[stream_idx], FALSE );
		UPRINTF( " H265 + ES3 + IBBP + COMP does not support, so force disable COMP\n" );

		// at IBBP those flag OFF makes better image.
		config_param_h265.buffer_ctrl_flag		= FJ_MOVIE_OFF;
		config_param_h265.pictstruct_flag		= FJ_MOVIE_OFF;

	}
	else {
		g_comp[stream_idx] = cfg->compress;
		FJ_VideoEncoder_CompEnable( ( UCHAR )g_enc_id[stream_idx], cfg->compress );
	}
#else
	g_comp[stream_idx] = cfg->compress;
	FJ_VideoEncoder_CompEnable( ( UCHAR )g_enc_id[stream_idx], cfg->compress );
#endif

	ret = FJ_VideoEncoder_Setup( ( UCHAR )g_enc_id[stream_idx] );
	UPRINTF_ERR(ret);

	g_is_set_ps[stream_idx] = FALSE;
	gRec_TimeLapse[stream_idx].frame_cnt = 0;

	if( config_param_h265.gop_struct == FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP ) {
		g_TimeDelta[stream_idx] = g_fps_ti[stream_idx];
	}
	else {
		g_TimeDelta[stream_idx] = 0;
	}

	g_max_frame_cnt[stream_idx] = ( cfg->max_time * 60 ) * fps_mux / 100;
	g_cur_frame_cnt[stream_idx] = 0;

	return OK;
}

static INT32 user_utility_record_h265_stream_cb( UCHAR video_enc_id, T_FJ_MOVIE_H265_STREAM_PARAM * stream_info, E_SHARE_YUV_OUT	stream_idx )
{
	INT32 ret = 0;
	T_FJ_MUX_H265_PS_INFO ps_info = {0};
	ULONG ct_offset;
	UCHAR end_chunk_flg = FJ_MUX_OFF;

	#ifdef DBG_H265_SEQUENCE_MBA
	if( video_enc_id == E_VIDEO_ENOCDE_ID_H265_CH0 )
		MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H265, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "H265_CB([%d]: %ld )", stream_idx, ( ULONG )stream_info->frame_no ) );
	else
		MPRINTF( ( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H264_0, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "H265_CB([%d]: %ld )", stream_idx, ( ULONG )stream_info->frame_no ) );
	#endif

	if( g_stream_format[stream_idx] == FJ_MOVIE_STREAM_FORMAT_ES ) {
		if (stream_info->stream_size > 0) user_utility_record_h265_stream_edit( stream_info, &ps_info );

		if( g_is_set_ps[stream_idx] == FALSE ) {
			g_is_set_ps[stream_idx] = TRUE;

			user_utility_record_h265_psinfo_save( &ps_info, &g_ps_info[stream_idx] );

			ret = FJ_Mux_Fixed_Atom_Setting( video_enc_id, NULL, &g_ps_info[stream_idx] ); // only 1st
			UPRINTF_ERR(ret);
		}

		if( stream_info->frame_type == FJ_MOVIE_VIDEO_FRAME_TYPE_B ) {
			ct_offset = 0;
		}
		else {
			ct_offset = g_TimeDelta[stream_idx] * 3;
		}

		//For FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP
		if( g_gop_num[stream_idx] == stream_info->counter_in_gop + 1 ) {
			end_chunk_flg = FJ_MUX_ON;
		} else if (g_gop_num[stream_idx] == 0) {
			end_chunk_flg = FJ_MUX_ON;
		}

		if( ( stream_info->frame_type == FJ_MOVIE_VIDEO_FRAME_TYPE_IDR )
				&& ( ( g_cur_frame_cnt[stream_idx] >= g_max_frame_cnt[stream_idx] ) || ( g_mux_status[stream_idx] == FJ_OK_MAX_SIZE_END ) ) ) {
			ret = user_utility_record_h265_change_file( stream_idx, video_enc_id );
			UPRINTF_ERR(ret);

			ret = FJ_Mux_Atom_Creat( video_enc_id , g_file_id[stream_idx] );
			UPRINTF_ERR(ret);

			ret = FJ_Mux_Fixed_Atom_Setting( video_enc_id, NULL, &g_ps_info[stream_idx] );
			UPRINTF_ERR(ret);

			g_cur_frame_cnt[stream_idx] = 0;
			g_mux_status[stream_idx] = FJ_OK_MP4_COMMON_OK;
		}

		if (stream_info->stream_size > 0) {
			ret = FJ_Mux_Video_Add_Sample( video_enc_id, ( UCHAR * )stream_info->stream_address, stream_info->stream_size, ct_offset, stream_info->frame_type, end_chunk_flg, stream_info->stream_end_flg );
			UPRINTF_ERR(ret);
		}else if(stream_info->stream_end_flg == FJ_MUX_ON){
			User_Utility_Record_Mux_Audio_Force_End(video_enc_id);
		}

		g_cur_frame_cnt[stream_idx] += 1;

		if( ( ret != FJ_OK_MP4_COMMON_OK ) && ( g_mux_status[stream_idx] == FJ_OK_MP4_COMMON_OK ) ) {
			g_mux_status[stream_idx] = ret;
		}

		if( ret == FJ_OK_MEDIAFULL_END ) {
			Snd_Event_to_User( E_USER_EVENT_RECORD_STOP, 0LL );
		}

		if( stream_info->stream_end_flg ) {
			g_is_set_ps[stream_idx] = FALSE;
			user_utility_record_h265_psinfo_free( &g_ps_info[stream_idx] );
		}
	}
	else if( g_stream_format[stream_idx] == FJ_MOVIE_STREAM_FORMAT_ES_HEADER ) {
		FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, ( UINT32 )stream_info->stream_address, stream_info->stream_size );
		OS_IF_Rtos_H265_Stream_Send( stream_info, video_enc_id, stream_info->frame_no );
	}

	return ret;
}

static INT32 user_utility_record_h265_end( UCHAR stream_idx )
{
	INT32 ret;

	if( g_is_record[stream_idx] ) {
		if ( g_stream_format[stream_idx] == FJ_MOVIE_STREAM_FORMAT_ES ) {
			ret = FJ_Mux_Finalize( ( UCHAR )g_enc_id[stream_idx] );
			UPRINTF_ERR(ret);

			FJ_Fs_Close( g_file_id[stream_idx] );

			User_Utility_Record_Mux_Mem_Finalize();
		}

		g_is_record[stream_idx] = FALSE;
	}

	if( stream_idx == E_SHARE_YUV_OUT_H265 ) {
		Snd_Event_to_User( E_USER_EVENT_RECORD_H265_END, 0 );
	}
	else if( stream_idx == E_SHARE_YUV_OUT_H264_0 ) {
		Snd_Event_to_User( E_USER_EVENT_RECORD_H264_0_END, 0 );
	}

	return OK;
}

static INT32 user_utility_record_h265_cache_start( T_SHARE_VIDEO_REC_CONFIG * cfg, ULONG base_fps, E_SHARE_YUV_OUT	stream_idx )
{
	ULONG actual_fps = 0;

	actual_fps = ( ( cfg->frame_rate % 100 ) > 0 ) ? ( ( cfg->frame_rate / 100 ) + 1 ) : ( cfg->frame_rate / 100 );

	memset( &g_rec_cache_info[stream_idx], 0, sizeof( T_RS_MOVIE_H265_CACHE_INFO ) );

	g_rec_cache_info[stream_idx].cache_addr = cfg->mov_cache_addr;
	g_rec_cache_info[stream_idx].cache_size = cfg->mov_cache_size;
	g_rec_cache_info[stream_idx].cache_sample_num = actual_fps * D_RS_MOVIE_H265_CACHE_TIME_MAX;

	g_rec_cache_info[stream_idx].addr_start = g_rec_cache_info[stream_idx].cache_addr;
	g_rec_cache_info[stream_idx].addr_end = g_rec_cache_info[stream_idx].cache_addr;
	g_rec_cache_info[stream_idx].write_point = 0;
	g_rec_cache_info[stream_idx].read_point = 0;
	g_rec_cache_info[stream_idx].sample_num = 0;

	return OK;
}

static INT32 user_utility_record_h265_cache_stream_cb( UCHAR video_enc_id, T_FJ_MOVIE_H265_STREAM_PARAM * stream_info, E_SHARE_YUV_OUT	stream_idx )
{
	INT32 ret;
	T_FJ_MUX_H265_PS_INFO ps_info = {0};
	T_USER_SYNC_MSG user_sync_msg;
	INT32 i = 0;

	if( g_rec_cache_info[stream_idx].sample_num >= g_rec_cache_info[stream_idx].cache_sample_num ) {
		for( i = 0; i < g_rec_cache_info[stream_idx].sample_num; i++ ) {
			if( g_is_set_ps[stream_idx] == FALSE ) {
				g_is_set_ps[stream_idx] = TRUE;
				g_is_cache_first_frame[stream_idx] = TRUE;
				user_utility_record_h265_stream_edit( &g_rec_cache_info[stream_idx].stream_param[g_rec_cache_info[stream_idx].read_point], &ps_info );
				user_utility_record_h265_psinfo_save( &ps_info, &g_ps_info[stream_idx] );
			}

			ret = user_utility_record_h265_cache_free( stream_idx, g_rec_cache_info[stream_idx].sample_addr[g_rec_cache_info[stream_idx].read_point], g_rec_cache_info[stream_idx].sample_size[g_rec_cache_info[stream_idx].read_point] );
			UPRINTF_ERR(ret);

			g_rec_cache_info[stream_idx].read_point = ( ( g_rec_cache_info[stream_idx].read_point + 1 ) % g_rec_cache_info[stream_idx].cache_sample_num );
			g_rec_cache_info[stream_idx].sample_num -= 1;

			if( g_rec_cache_info[stream_idx].stream_param[g_rec_cache_info[stream_idx].read_point].frame_type == FJ_MOVIE_VIDEO_FRAME_TYPE_IDR ) {
				break;
			}
		}
	}

	ret = user_utility_record_h265_cache_malloc( stream_idx, &g_rec_cache_info[stream_idx].sample_addr[g_rec_cache_info[stream_idx].write_point], stream_info->stream_size );
	UPRINTF_ERR(ret);

	memcpy( &g_rec_cache_info[stream_idx].stream_param[g_rec_cache_info[stream_idx].write_point], stream_info, sizeof( T_FJ_MOVIE_H265_STREAM_PARAM ) );
	g_rec_cache_info[stream_idx].stream_param[g_rec_cache_info[stream_idx].write_point].stream_address = g_rec_cache_info[stream_idx].sample_addr[g_rec_cache_info[stream_idx].write_point];

	memcpy( ( UINT8 * )g_rec_cache_info[stream_idx].sample_addr[g_rec_cache_info[stream_idx].write_point], ( UINT8 * )stream_info->stream_address, stream_info->stream_size );
	g_rec_cache_info[stream_idx].sample_size[g_rec_cache_info[stream_idx].write_point] = stream_info->stream_size;
	g_rec_cache_info[stream_idx].write_point = ( ( g_rec_cache_info[stream_idx].write_point + 1 ) % g_rec_cache_info[stream_idx].cache_sample_num );
	g_rec_cache_info[stream_idx].sample_num += 1;

	user_sync_msg.event = E_USER_SYNC_EVENT_RECORD_CACHE_H265_STREAM_CB;
	user_sync_msg.param = g_enc_id[stream_idx];
	user_sync_msg.param1 = g_enc_enum[stream_idx];

	ret = Snd_Msg_To_User_Sync( &user_sync_msg );
	UPRINTF_ERR(ret);

	return OK;
}

static INT32 user_utility_record_h265_cache_end( UCHAR stream_idx )
{
	INT32 ret;
	T_USER_SYNC_MSG user_sync_msg;
	INT32 i;

	for( i = 0; i < g_rec_cache_info[stream_idx].cache_sample_num; i++ ) {
		user_sync_msg.event = E_USER_SYNC_EVENT_RECORD_CACHE_H265_STREAM_CB;
		user_sync_msg.param = g_enc_id[stream_idx];
		user_sync_msg.param1 = g_enc_enum[stream_idx];

		ret = Snd_Msg_To_User_Sync( &user_sync_msg );
		UPRINTF_ERR(ret);
	}

	return OK;
}

static INT32 user_utility_record_h265_cache_malloc( UCHAR stream_idx, ULONG * sample_addr, ULONG sample_size )
{
	if( g_rec_cache_info[stream_idx].addr_start == g_rec_cache_info[stream_idx].addr_end ) {
		if( g_rec_cache_info[stream_idx].sample_num == 0 ) {
			 * sample_addr = g_rec_cache_info[stream_idx].addr_end;
			g_rec_cache_info[stream_idx].addr_end += sample_size;
		}
		else {
			UPRINTF( "Run out of memory!!!\n" );

			return NG;
		}
	}
	else if( g_rec_cache_info[stream_idx].addr_start < g_rec_cache_info[stream_idx].addr_end ) {
		if( ( g_rec_cache_info[stream_idx].addr_end + sample_size ) <= ( g_rec_cache_info[stream_idx].cache_addr + g_rec_cache_info[stream_idx].cache_size ) ) {
			 * sample_addr = g_rec_cache_info[stream_idx].addr_end;
			g_rec_cache_info[stream_idx].addr_end += sample_size;
		}
		else {
			if( ( g_rec_cache_info[stream_idx].cache_addr + sample_size ) <= g_rec_cache_info[stream_idx].addr_start ) {
				 * sample_addr = g_rec_cache_info[stream_idx].cache_addr;
				g_rec_cache_info[stream_idx].addr_end = g_rec_cache_info[stream_idx].cache_addr + sample_size;
			}
			else {
				UPRINTF( "Run out of memory!!!\n" );

				return NG;
			}
		}
	}
	else if( g_rec_cache_info[stream_idx].addr_start > g_rec_cache_info[stream_idx].addr_end ) {
		if( ( g_rec_cache_info[stream_idx].addr_end + sample_size ) <= g_rec_cache_info[stream_idx].addr_start ) {
			 * sample_addr = g_rec_cache_info[stream_idx].addr_end;
			g_rec_cache_info[stream_idx].addr_end += sample_size;
		}
		else {
			UPRINTF( "Run out of memory!!!\n" );

			return NG;
		}
	}

	return OK;
}

static INT32 user_utility_record_h265_cache_free( UCHAR stream_idx, ULONG sample_addr, ULONG sample_size )
{
	if( g_rec_cache_info[stream_idx].addr_start == g_rec_cache_info[stream_idx].addr_end ) {
		if( g_rec_cache_info[stream_idx].sample_num == 0 ) {
			UPRINTF( "Free memory ERROR!!!\n" );

			return NG;
		}
		else {
			if( ( g_rec_cache_info[stream_idx].addr_start + sample_size ) <= ( g_rec_cache_info[stream_idx].cache_addr + g_rec_cache_info[stream_idx].cache_size ) ) {
				g_rec_cache_info[stream_idx].addr_start += sample_size;
			}
			else {
				g_rec_cache_info[stream_idx].addr_start = g_rec_cache_info[stream_idx].cache_addr + sample_size;
			}
		}
	}
	else if( g_rec_cache_info[stream_idx].addr_start < g_rec_cache_info[stream_idx].addr_end ) {
		if( ( g_rec_cache_info[stream_idx].cache_addr + sample_size ) <= g_rec_cache_info[stream_idx].addr_end ) {
			g_rec_cache_info[stream_idx].addr_start += sample_size;
		}
		else {
			UPRINTF( "Free memory ERROR!!!\n" );

			return NG;
		}
	}
	else if( g_rec_cache_info[stream_idx].addr_start > g_rec_cache_info[stream_idx].addr_end ) {
		if( ( g_rec_cache_info[stream_idx].addr_start + sample_size ) <= ( g_rec_cache_info[stream_idx].cache_addr + g_rec_cache_info[stream_idx].cache_size ) ) {
			g_rec_cache_info[stream_idx].addr_start += sample_size;
		}
		else {
			if( ( g_rec_cache_info[stream_idx].cache_addr + sample_size ) <= g_rec_cache_info[stream_idx].addr_end ) {
				g_rec_cache_info[stream_idx].addr_start = g_rec_cache_info[stream_idx].cache_addr + sample_size;
			}
			else {
				UPRINTF( "Free memory ERROR!!!\n" );

				return NG;
			}
		}
	}

	return OK;
}

static VOID user_utility_record_h265_stream_edit( T_FJ_MOVIE_H265_STREAM_PARAM * stream_param, T_FJ_MUX_H265_PS_INFO * ps_info )
{
	UINT32							i;
	ULONG							stream_address;
	ULONG							addsample_addr=0;
	UINT32							addsample_size=0;
	BYTE							nal_nuit_type;
	BYTE							nal_nuit_type_previous=0xFF;
	T_RS_MOVIE_MUX_NAL_CONTROL_TBL	nal_unit[32];
	T_FJ_MUX_H265_PS_INFO_ARRAY * 	array_elements;
	UINT32							read_offset;
	UINT8							 * base_address;
	UINT32							pps_count;

	memset( ( void * )ps_info, 0, sizeof( T_FJ_MUX_H265_PS_INFO ) );

	stream_address = ( ULONG )stream_param->stream_address;

	if( stream_param->nal_unit_num <= 0 ) {
		printf( "user_utility_record_h265_stream_edit : nal_unit_num NULL ERROR=%x\n", stream_param->nal_unit_num );
	}

	if( stream_param->nal_unit_num > 32 ) {
		stream_param->nal_unit_num = 32;
	}

	nal_unit[0].nal_address	= stream_address;
	nal_unit[0].nal_size	= stream_param->nal_unit_size[0];

	for( i = 1; i < stream_param->nal_unit_num; i++ ) {
		nal_unit[i].nal_address	= nal_unit[i-1].nal_address + nal_unit[i-1].nal_size;
		nal_unit[i].nal_size	= stream_param->nal_unit_size[i];
	}

	for( i = 0; i < stream_param->nal_unit_num; i++ ) {

		nal_nuit_type = ( ( * ( UCHAR * )( nal_unit[i].nal_address + D_RS_MUX_STARTCODE_SIZE ) )>>1 )&0x3F;

		if( ( nal_nuit_type == D_BF_MUX_H265_NALTYPE_VPS ) || ( nal_nuit_type == D_BF_MUX_H265_NALTYPE_SPS ) ) {

			if( nal_nuit_type != nal_nuit_type_previous ) {
				ps_info->array_num++;
			}

			if( ps_info->array_num == 0 ) {
				printf( "user_utility_record_h265_stream_edit : Error! array_num is abnormal.( set 1 )\n" );
				ps_info->array_num = 1;
			}

			array_elements = &ps_info->array_elements[ps_info->array_num - 1];

			array_elements->array_completeness						= 1;
			array_elements->nal_type								= nal_nuit_type;
			array_elements->nal_address[array_elements->nal_num]	= nal_unit[i].nal_address;
			array_elements->nal_size[array_elements->nal_num]		= nal_unit[i].nal_size;
			array_elements->nal_num++;

			{
				ULONG	value;
				ULONG	result;
				value =	nal_unit[i].nal_size - D_RS_MUX_STARTCODE_SIZE;
				result =	( ( ( value ) & 0x000000ff ) << 24 ) | ( ( ( value ) & 0x0000ff00 ) << 8 ) | 
							( ( ( value ) & 0x00ff0000 ) >> 8 ) | ( ( ( value ) & 0xff000000 ) >> 24 );
				memcpy( ( UCHAR * )( nal_unit[i].nal_address ), ( UCHAR * )&result, D_RS_MUX_STARTCODE_SIZE );
				FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, ( ULONG )nal_unit[i].nal_address, D_RS_MUX_STARTCODE_SIZE );
			}
		}
		else if( nal_nuit_type == D_BF_MUX_H265_NALTYPE_PPS ) {
			ps_info->array_num++;

			array_elements = &ps_info->array_elements[ps_info->array_num - 1];

			array_elements->array_completeness						= 1;
			array_elements->nal_type								= nal_nuit_type;

			base_address	= ( UINT8 * )nal_unit[i].nal_address;
			read_offset		= 0;

			for( read_offset=0; read_offset<( nal_unit[i].nal_size-3 ); read_offset++ ) {
				if( ( base_address[read_offset] == 0 ) && 
					( base_address[read_offset+1] == 0 ) &&
					( base_address[read_offset+2] == 0 ) &&
					( base_address[read_offset+3] == 1 ) ) {

					array_elements->nal_address[array_elements->nal_num] = ( UINT32 )( base_address + read_offset );
					array_elements->nal_num++;
				}
			}
			array_elements->nal_address[array_elements->nal_num] = nal_unit[i].nal_address + nal_unit[i].nal_size;

			for( pps_count=0; pps_count<array_elements->nal_num; pps_count++ ) {
				array_elements->nal_size[pps_count] = array_elements->nal_address[pps_count+1] - array_elements->nal_address[pps_count];

				{
					ULONG	value;
					ULONG	result;
					value =	array_elements->nal_size[pps_count] - D_RS_MUX_STARTCODE_SIZE;
					result =	( ( ( value ) & 0x000000ff ) << 24 ) | ( ( ( value ) & 0x0000ff00 ) << 8 ) | 
								( ( ( value ) & 0x00ff0000 ) >> 8 ) | ( ( ( value ) & 0xff000000 ) >> 24 );
					memcpy( ( UCHAR * )( array_elements->nal_address[pps_count] ), ( UCHAR * )&result, D_RS_MUX_STARTCODE_SIZE );
					FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, ( ULONG )nal_unit[i].nal_address, D_RS_MUX_STARTCODE_SIZE );
				}
			}
		}
		else {
			if( addsample_addr == 0 ) {
				addsample_addr = nal_unit[i].nal_address;
			}

			addsample_size += nal_unit[i].nal_size;

			{
				ULONG	value;
				ULONG	result;
				value =	nal_unit[i].nal_size - D_RS_MUX_STARTCODE_SIZE;
				result =	( ( ( value ) & 0x000000ff ) << 24 ) | ( ( ( value ) & 0x0000ff00 ) << 8 ) | 
							( ( ( value ) & 0x00ff0000 ) >> 8 ) | ( ( ( value ) & 0xff000000 ) >> 24 );
				memcpy( ( UCHAR * )( nal_unit[i].nal_address ), ( UCHAR * )&result, D_RS_MUX_STARTCODE_SIZE );
				FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, ( ULONG )nal_unit[i].nal_address, D_RS_MUX_STARTCODE_SIZE );
			}
		}

		nal_nuit_type_previous = nal_nuit_type;		// To hold the nal_unit_type.
	}

	stream_param->stream_address	= addsample_addr;
	stream_param->stream_size		= addsample_size;

	return;
}

static VOID user_utility_record_h265_psinfo_save( T_FJ_MUX_H265_PS_INFO * ps_info_src, T_FJ_MUX_H265_PS_INFO * ps_info_dst )
{
	UINT32 i, j;

	memset( ps_info_dst, 0 , sizeof( T_FJ_MUX_H265_PS_INFO ) );

	ps_info_dst->array_num = ps_info_src->array_num;

	for( i = 0; i < ps_info_src->array_num; i++ ) {
		ps_info_dst->array_elements[i].array_completeness = ps_info_src->array_elements[i].array_completeness;
		ps_info_dst->array_elements[i].nal_type = ps_info_src->array_elements[i].nal_type;
		ps_info_dst->array_elements[i].nal_num = ps_info_src->array_elements[i].nal_num;

		for( j = 0; j < ps_info_dst->array_elements[i].nal_num; j++ ) {
			ps_info_dst->array_elements[i].nal_size[j] = ps_info_src->array_elements[i].nal_size[j];
			ps_info_dst->array_elements[i].nal_address[j] = ( UINT32 )malloc( ps_info_dst->array_elements[i].nal_size[j] );
			memcpy( ( UCHAR * )ps_info_dst->array_elements[i].nal_address[j], ( UCHAR * )ps_info_src->array_elements[i].nal_address[j], ps_info_dst->array_elements[i].nal_size[j] );
			FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, ps_info_dst->array_elements[i].nal_address[j], ps_info_dst->array_elements[i].nal_size[j] );
		}
	}

	return;
}

static VOID user_utility_record_h265_psinfo_free( T_FJ_MUX_H265_PS_INFO * ps_info )
{
	UINT32 i, j;

	for( i = 0; i < ps_info->array_num; i++ ) {
		for( j = 0; j < ps_info->array_elements[i].nal_num; j++ ) {
			if( ps_info->array_elements[i].nal_address[j] != 0 ) {
				free( ( void * )ps_info->array_elements[i].nal_address[j] );
			}
		}
	}

	return;
}

static INT32 user_utility_record_h265_change_file( UCHAR stream_idx, UCHAR video_enc_id )
{
	INT32 ret;
	ULONG prev_file_id;

	ret = FJ_Dcf_Make_New_File_Path_By_Ext( "MP4", g_file_path );
	UPRINTF_ERR(ret);

	prev_file_id = g_file_id[stream_idx];

	g_file_id[stream_idx] = FJ_Fs_Open( g_file_path, ( FJ_FS_OPEN_RDWR|FJ_FS_OPEN_CREAT|FJ_FS_OPEN_TRUNC ) );

	ret = FJ_Dcf_Update_DcfDb_Add_File( g_file_path );
	UPRINTF_ERR(ret);

	ret = FJ_Mux_Movie_File_Old_Set( video_enc_id );
	UPRINTF_ERR(ret);

	ret = FJ_Mux_Close_File( video_enc_id );
	UPRINTF_ERR(ret);

	FJ_Fs_Close( prev_file_id );

	return OK;
}

static int round( float x )
{
	return ( int )( x + 0.5 );
}

