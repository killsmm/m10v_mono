/**
 * @file		h264_user.c
 * @brief		User Custom Part for H.264 Function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_LAST_CODE"

#include "h264_user.h"
#include "movie_video.h"
#include "movie_video_attr.h"
#include "movie_common.h"
#include "sdram_map_movie_record.h"
#include "sdram_map_movie_play.h"
#include "sdram_map_play.h"
#include "sdram_map_cache_define.h"
#include "mba_if.h"

#include "im_iip.h"
#include "im_iip_param.h"
#include "bf_iip_resource.h"
#include "instance.h"
#include "disp.h"
#include "l1l2cache.h"
#include "user_streaming.h"

/*--------------------------------------------------------------*/
/* Definition													*/
/*--------------------------------------------------------------*/
// Image SRAM ring buffer address.
// IIP
#define D_BF_H264_USER_IIP_TBL_ALIGN			(8)
#define D_BF_H264_USER_IIP_PIXID_MAX			(D_IM_IIP_PIXID_MAXNUM)


/*--------------------------------------------------------------*/
/* Enumeration													*/
/*--------------------------------------------------------------*/
typedef enum{
	E_BF_H264_USER_RESIZE_MODE_NEAREST		= 0,
	E_BF_H264_USER_RESIZE_MODE_BILINEAR		= 1,
	E_BF_H264_USER_RESIZE_MODE_BICUBIC		= 2,
} E_BF_H264_USER_RESIZE_MODE;

/* Rotation (clockwise) */
typedef enum {
	E_BF_H264_USER_ROTATE_0 = 0,	/**< not rotate						*/
	E_BF_H264_USER_ROTATE_90,		/**< rotate 90 degree (clockwise)	*/
	E_BF_H264_USER_ROTATE_180,		/**< rotate 180 degree (clockwise)	*/
	E_BF_H264_USER_ROTATE_270,		/**< rotate 270 degree (clockwise)	*/
} E_BF_H264_USER_ROTATE;

/*--------------------------------------------------------------*/
/* Structure													*/
/*--------------------------------------------------------------*/

#if 0	// for Debug Dump
typedef struct {
	UCHAR*	addr;
	ULONG	copy_bytes;
	ULONG	last_bytes;
} T_BF_RIBERY_RECOVERY_AREA;
#endif

typedef struct {
	ULONG						src_addr[2];	// src address
	ULONG 						dst_addr[2];	// dst address
	USHORT						g_src_width;	// global src width
	USHORT						g_src_lines;	// global src lines
	USHORT						src_width;		// src width
	USHORT						src_lines;		// src lines
	USHORT						g_dst_width;	// global dst width
	USHORT						g_dst_lines;	// global dst lines
	USHORT						dst_width;		// dst width
	USHORT						dst_lines;		// dst lines
	USHORT						ribery_mode;	// ribery Mode
	E_BF_H264_USER_RESIZE_MODE	resize_mode;	// resize mode
	E_BF_H264_USER_ROTATE		rotate;			// rotate(clockwise)
} T_BF_H264_USER_RESIZE_TBL;

/*--------------------------------------------------------------*/
/* Global Data													*/
/*--------------------------------------------------------------*/
static	volatile	UCHAR					gH264_User_Play_Exc		= D_BF_H264_USER_PLAY_EXC_OFF;
static	volatile	UCHAR					gH264_User_Record_Exc	= D_BF_H264_USER_RECORD_EXC_OFF;
static	volatile	UCHAR					gH264_User_Auto_End		= D_BF_H264_USER_AUTO_END_OFF;
static	volatile	T_H264_IF_PARAM_RECORD	gH264_User_Record_Param[E_H264_IF_STREAM_CH_MAX];

static	volatile	UCHAR					gH264_User_Record_Gop_Size[E_H264_IF_STREAM_CH_MAX];

/*--------------------------------------------------------------*/
/* Macro														*/
/*--------------------------------------------------------------*/
#define M_BF_H264_USER_IIP_ROUNDUP_ALIGN(val)	(((((val) +D_BF_H264_USER_IIP_TBL_ALIGN -1)) /D_BF_H264_USER_IIP_TBL_ALIGN) *D_BF_H264_USER_IIP_TBL_ALIGN)

/*--------------------------------------------------------------*/
/* Local Function												*/
/*--------------------------------------------------------------*/
static	VOID	h264_user_record_start_err_notify( VOID );
static	VOID	h264_user_record_initialize_callback(int type, int param);
static	VOID	h264_user_record_start_video_callback(int type, int param);
static	VOID	h264_user_record_start_video_callback_sync(int type, int param);
static	VOID	h264_user_record_stop_video_callback(int type, int param);

static	INT32	h264_user_play_get_display_setting( T_H264_IF_DISPLAY_PARAM* lcd_info, T_H264_IF_DISPLAY_PARAM* hdmi_info, T_H264_IF_DECODER_PARAM* decoder_param );
static	VOID	h264_user_play_finalize_callback(int type, int param);

static	INT32	h264_user_resize_rotate( T_BF_H264_USER_RESIZE_TBL* resize_tbl );

// ***************************************************************************
//                                Record part
// ***************************************************************************
/**
 *	@brief		Report "Movie Record Start Error" to U.I.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h264_user_record_start_err_notify( VOID )
{
#ifdef CO_MOVIE_DSC_MODE
	OS_USER_ER ercd;
	
	FJ_HostPostMessage( FJ_HM_MOVIE_SETUP_COMPLETE, 1 );
	
#endif
	
	gH264_User_Record_Exc = D_BF_H264_USER_RECORD_EXC_OFF;
	gH264_User_Auto_End = D_BF_H264_USER_AUTO_END_OFF;
	OS_User_Set_Flg( FID_ENCODER, D_BF_FLG_SETUP_COMPLETE_H264 );
	
	OS_User_Ext_Tsk();
	
	return ;
}

/**
 *	@brief		H264 Movie Record Ready Process for BF.\n
 *				Ready Movie Record on encord controller task.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_H264_User_Record_Ready_Movie( E_VIDEO_ENOCDE_ID video_enc_id )
{
	INT32			ret;
	T_H264_IF_MSG	msg;
	E_H264_IF_STREAM_CH stream_ch;
	OS_USER_FLGPTN		flgptn;
	
	if( gH264_User_Record_Exc == D_BF_H264_USER_RECORD_EXC_OFF ){
		OS_User_Clr_Flg( FID_ENCODER, ~D_BF_FLG_START_SYNC_H264 );
		// Reset Msg Box
		ret = H264_If_Clear_Msg();
		if( ret != D_OS_USER_E_TMOUT ){
			BF_Debug_Print_Error(("--- H264_If_Clear_Msg ERR. ret=%d\n", ret));
		}
		
		// initialize gH264_User_Record_Param
		memset( (VOID *)&gH264_User_Record_Param, 0, sizeof(gH264_User_Record_Param) );
		
		// start Encode controller
		OS_User_Sta_Tsk( TID_H264_CONTROLLER, 0 );
	}
	
	BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, &stream_ch );
	
	BF_Movie_Video_Attr_Get_H264_Config( video_enc_id, (T_H264_IF_PARAM_RECORD*)&gH264_User_Record_Param[stream_ch] );
	
	/* Encode Sync Setting */
	BF_Movie_Common_Sync_Setting( video_enc_id );
	
	// ==== Movie Initialize ====
	msg.data.pRecordParam = (T_H264_IF_PARAM_RECORD*)&gH264_User_Record_Param[stream_ch];
	msg.type = E_H264_IF_TYPE_RECORD_INITIALIZE;
	msg.cbfunc = h264_user_record_initialize_callback;
	
	// Send Msg (for Movie Initialize)
	ret = H264_If_Set_Msg( &msg, E_H264_IF_MSG_WAIT_NONBLOCK );
	if( ret != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("--- Movie Initialize Msg ERR. ret=%x\n", ret ));
		gH264_User_Record_Exc = D_BF_H264_USER_RECORD_EXC_OFF;
		gH264_User_Auto_End = D_BF_H264_USER_AUTO_END_OFF;
		return D_BF_H264_USER_NG;
	}
		
	// Wait complete flag for synchronization API.
	OS_User_Wai_Flg( FID_ENCODER, D_BF_FLG_SETUP_COMPLETE_H264, D_OS_USER_TWF_ORW, &flgptn );
	OS_User_Clr_Flg( FID_ENCODER, ~D_BF_FLG_SETUP_COMPLETE_H264 );
	
	if( gH264_User_Record_Exc == D_BF_H264_USER_RECORD_EXC_OFF ){
		return D_BF_H264_USER_NG;
	}
	
	return D_BF_H264_USER_OK;
}

/**
 *	@brief		H264 Movie Record Process for BF.\n
 *				Start Movie Record on encord controller task.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_H264_User_Record_Start_Video( E_VIDEO_ENOCDE_ID video_enc_id, BOOL sync )
{
	INT32			ret;
	T_H264_IF_MSG	msg;
	E_H264_IF_STREAM_CH stream_ch;
	OS_USER_FLGPTN		flgptn;
	
	if( gH264_User_Record_Exc == D_BF_H264_USER_RECORD_EXC_READY ){
		BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, &stream_ch );
		
		BF_Movie_Common_Timer_Start_View(video_enc_id);
		
		// ==== Movie Start ====
		msg.type = E_H264_IF_TYPE_RECORD_START_VIDEO;
		msg.cbfunc = h264_user_record_start_video_callback;
		
		// wait if sync
		if( sync ){
			msg.cbfunc = h264_user_record_start_video_callback_sync;
		}
		
#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
		BF_H264_User_Record_Valid_Image_Address( video_enc_id );
		BF_Movie_Common_Set_Dummy_Sync( video_enc_id, TRUE );
#endif
		
		// Send Msg (for Movie Start)
		ret = H264_If_Set_Msg( &msg, E_H264_IF_MSG_WAIT_NONBLOCK );
		if( ret != D_OS_USER_E_OK ){
			BF_Debug_Print_Error(("--- BF_H264_User_Record_Start_Video Msg ERR. ret=%x\n", ret ));
			gH264_User_Record_Exc = D_BF_H264_USER_RECORD_EXC_OFF;
			gH264_User_Auto_End = D_BF_H264_USER_AUTO_END_OFF;
			return D_BF_H264_USER_NG;
		}
		
		// wait if sync
		if( sync ){
			OS_User_Wai_Flg( FID_ENCODER, D_BF_FLG_START_COMPLETE_H264, D_OS_USER_TWF_ORW, &flgptn );
			OS_User_Clr_Flg( FID_ENCODER, ~D_BF_FLG_START_COMPLETE_H264 );
		}
		
		gH264_User_Record_Exc = D_BF_H264_USER_RECORD_EXC_RECORDING;
	}
	else if( gH264_User_Record_Exc == D_BF_H264_USER_RECORD_EXC_RECORDING ){
#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
		BF_H264_User_Record_Valid_Image_Address( video_enc_id );
		BF_Movie_Common_Set_Dummy_Sync( video_enc_id, TRUE );
#endif
		// Nothing
	}
	else{
		BF_Debug_Print_Error(("--- H264 Status ERR. Record_Exc=0x%x\n", gH264_User_Record_Exc));
		gH264_User_Record_Exc = D_BF_H264_USER_RECORD_EXC_OFF;
		gH264_User_Auto_End = D_BF_H264_USER_AUTO_END_OFF;
		return D_BF_H264_USER_NG;
	}
	
	return D_BF_H264_USER_OK ;
}

/**
 *	@brief		H264 Movie Record Stop Process for BF.\n
 *				Stop Movie Record on encord controller task.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_H264_User_Record_Stop_Video( E_VIDEO_ENOCDE_ID video_enc_id )
{
	INT32		ret;
	E_H264_IF_STREAM_CH stream_ch;
	T_H264_IF_MSG	msg;
	
	// ==== Movie Stop ====
	BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, &stream_ch );
	
	msg.type = E_H264_IF_TYPE_RECORD_STOP;
	msg.cbfunc = h264_user_record_stop_video_callback;
	msg.data.info = (ULONG)stream_ch;
	
	// Send Msg (for Movie Stop)
	ret = H264_If_Set_Msg( &msg, E_H264_IF_MSG_WAIT_NONBLOCK );
	if( ret != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("--- BF_H264_User_Record_Stop_Video Msg ERR.(Stop) ret=%x\n", ret ));
		return D_BF_H264_USER_NG;
	}
	
//	gH264_User_Record_Exc = D_BF_H264_USER_RECORD_EXC_MOVIE_STOP;
	
	return D_BF_H264_USER_OK;
}



/**
 *	@brief		H264 Movie Record Set Dynamic Parameter.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_H264_User_Record_Set_Dynamic_Parameter( E_VIDEO_ENOCDE_ID video_enc_id, T_FJ_MOVIE_H264_DYNAMIC_CONFIG_PARAM* dynamic_config )
{
	T_H264_IF_DYNAMIC_PARAM_RECORD param;
	E_H264_IF_STREAM_CH stream_ch;
	
	BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, &stream_ch );
	
	if(dynamic_config->frame_num_of_gop != BF_H264_User_Record_Get_Gop_Size(stream_ch)){
		if(dynamic_config->frame_num_of_gop > 254){
			BF_H264_User_Record_Set_Gop_Size(stream_ch, 254);
		}
		else{
			BF_H264_User_Record_Set_Gop_Size(stream_ch, dynamic_config->frame_num_of_gop);
		}
	}
	
	param.bitrate				= dynamic_config->bitrate;
	param.frame_num_of_gop		= dynamic_config->frame_num_of_gop;
	param.closed_gop			= dynamic_config->closed_gop_flag;
	param.gop_num_idr_to_idr	= dynamic_config->gop_num_idr_to_idr;

	switch( dynamic_config->gop_struct ){
		case FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP :
			param.gop_struct		= E_H264_IF_GOP_STR_IPPP;
			break;
		case FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP :
			param.gop_struct		= E_H264_IF_GOP_STR_IBBP;
			break;
		case FJ_MOVIE_VIDEO_GOP_STRUCT_IBP :
			param.gop_struct		= E_H264_IF_GOP_STR_IBP;
			break;
		case FJ_MOVIE_VIDEO_GOP_STRUCT_IIII :
			param.gop_struct		= E_H264_IF_GOP_STR_IIII;
			break;
		default :
			BF_Debug_Print_Error(("--- BF_H264_User_Record_Set_Dynamic_Parameter GOP ERR.\n" ));
			return D_BF_H264_USER_NG;
	}
	
	H264_If_Record_Dynamic_Param_Set( stream_ch, &param );
	
	return D_BF_H264_USER_OK;
}

/**
 *	@brief		H264 Set Insert IDR trigger.\n
 *	@param		video_enc_id	encodeID
 *	@return		D_BF_H264_USER_OK(0)
 *	@note		None
 *	@attention	None
 */
INT32 BF_H264_User_Record_Set_Insert_IDR_Picture( E_VIDEO_ENOCDE_ID video_enc_id )
{
	FJ_ERR_CODE ercd;
	E_H264_IF_STREAM_CH stream_ch;
	
	ercd = BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, &stream_ch );
	if( ercd != D_BF_H264_USER_OK ){
		return D_BF_H264_USER_NG;
	}
	
	H264_If_Record_Insert_IDR_Trigger_Set( stream_ch );
	
	return D_BF_H264_USER_OK;
}

/**
 *	@brief		H264 Movie Record Get Record Status for BF.
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_H264_User_Record_Get_Sts(VOID)
{
	return gH264_User_Record_Exc;
}

/**
 *	@brief		H264 Movie Record Get Image Address for BF.
 *	@param		video_enc_id	encodeID
 *	@param		size			stream Size
 *	@param		frame_no		frame number
 *	@param		addr_y output	y address
 *	@param		addr_c output	c address
 *	@param		g_width			global width
 *	@param		dummy_flg		dummy bank use flag
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_H264_User_Record_Get_Image_Address( E_VIDEO_ENOCDE_ID video_enc_id, E_FJ_MOVIE_VIDEO_SIZE size, ULLONG frame_no, T_BF_ENCODER_GET_IMAGE_ADDR_PARAM* addr_param )
{
	INT32 ret;
	E_H264_IF_STREAM_CH stream_ch;
	E_H264_IF_IMAGE_SIZE stream_size;
	T_H264_IF_GET_IMAGE_ADDR_PARAM h264_addr_param;
	
	switch( size ){
		case E_FJ_MOVIE_VIDEO_SIZE_4096_2304 :
			stream_size			= E_H264_IF_IMAGE_SIZE_4096_2304;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_4096_2160 :
			stream_size			= E_H264_IF_IMAGE_SIZE_4096_2160;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_4096_2048 :
			stream_size			= E_H264_IF_IMAGE_SIZE_4096_2048;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_3840_2160 :
			stream_size			= E_H264_IF_IMAGE_SIZE_3840_2160;
			break;
        case E_FJ_MOVIE_VIDEO_SIZE_3840_1920 :
			stream_size			= E_H264_IF_IMAGE_SIZE_3840_1920;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_3000_3000 :
			stream_size			= E_H264_IF_IMAGE_SIZE_3000_3000;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2704_2028 :
			stream_size			= E_H264_IF_IMAGE_SIZE_2704_2028;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2704_1520 :
			stream_size			= E_H264_IF_IMAGE_SIZE_2704_1520;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2560_1440 :
			stream_size			= E_H264_IF_IMAGE_SIZE_2560_1440;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1440 :
			stream_size			= E_H264_IF_IMAGE_SIZE_1920_1440;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1080 :
			stream_size			= E_H264_IF_IMAGE_SIZE_1920_1080;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_960 :
			stream_size			= E_H264_IF_IMAGE_SIZE_1920_960;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1440_1080 :
			stream_size			= E_H264_IF_IMAGE_SIZE_1440_1080;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1440_720 :
			stream_size			= E_H264_IF_IMAGE_SIZE_1440_720;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_1024 :
			stream_size			= E_H264_IF_IMAGE_SIZE_1280_1024;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_960 :
			stream_size			= E_H264_IF_IMAGE_SIZE_1280_960;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_768 :
			stream_size			= E_H264_IF_IMAGE_SIZE_1280_768;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_720 :
			stream_size			= E_H264_IF_IMAGE_SIZE_1280_720;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1024_768 :
			stream_size			= E_H264_IF_IMAGE_SIZE_1024_768;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_864_480 :
			stream_size			= E_H264_IF_IMAGE_SIZE_864_480;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_960_540 :
			stream_size 		= E_H264_IF_IMAGE_SIZE_960_540;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_960_480 :
			stream_size			= E_H264_IF_IMAGE_SIZE_960_480;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_800_600 :
			stream_size			= E_H264_IF_IMAGE_SIZE_800_600;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_848_480 :
			stream_size			= E_H264_IF_IMAGE_SIZE_848_480;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_720_576 :
			stream_size			= E_H264_IF_IMAGE_SIZE_720_576;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_720_480 :
			stream_size			= E_H264_IF_IMAGE_SIZE_720_480;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_480 :
			stream_size			= E_H264_IF_IMAGE_SIZE_640_480;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_360 :
			stream_size			= E_H264_IF_IMAGE_SIZE_640_360;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_480_270 :
			stream_size 		= E_H264_IF_IMAGE_SIZE_480_270;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_432_240 :
			stream_size			= E_H264_IF_IMAGE_SIZE_432_240;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_320_240 :
			stream_size			= E_H264_IF_IMAGE_SIZE_320_240;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1600_896:
			stream_size 		= E_H264_IF_IMAGE_SIZE_1600_896;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1024_576:
			stream_size 		= E_H264_IF_IMAGE_SIZE_1024_576;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_800_448:
			stream_size 		= E_H264_IF_IMAGE_SIZE_800_448;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_352_288:
			stream_size 		= E_H264_IF_IMAGE_SIZE_352_288;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2592_1944:
		    stream_size		  = E_H264_IF_IMAGE_SIZE_2592_1944;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2304_1296:
		  	stream_size		  = E_H264_IF_IMAGE_SIZE_2304_1296;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1280:
		  	stream_size		  = E_H264_IF_IMAGE_SIZE_1920_1280;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_512:
		  	stream_size		  = E_H264_IF_IMAGE_SIZE_640_512;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_384_288:
		  	stream_size		  = E_H264_IF_IMAGE_SIZE_384_288;
			break;
		default :
			return D_BF_H264_USER_NG;
	}
	
	BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, &stream_ch );
	
	ret = H264_If_Get_Record_Image_Address( stream_ch, stream_size, frame_no, &h264_addr_param );
	
	addr_param->y_addr		= h264_addr_param.y_addr;
	addr_param->c_addr		= h264_addr_param.c_addr;
	addr_param->y_cmp_addr	= 0;
	addr_param->c_cmp_addr	= 0;
	addr_param->g_width		= h264_addr_param.g_width;
	addr_param->dummy_flg	= h264_addr_param.dummy_flg;
	
	return ret;
}

/**
 *	@brief		H264 Movie Record Decide Image Address for BF.
 *	@param		video_enc_id	encodeID
 *	@param		frame_no		frame number
 *	@param		dummy_flg		dummy bank use flag
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_H264_User_Record_Decide_Image_Address( E_VIDEO_ENOCDE_ID video_enc_id, ULLONG frame_no, BOOL dummy_flg )
{
	E_H264_IF_STREAM_CH stream_ch;
	
	BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, &stream_ch );
	
	H264_If_Decide_Record_Image_Address( stream_ch, frame_no, dummy_flg );
	
	return;
}

/**
 *	@brief		H264 Movie Record Enabled Frame for TimeLapse.
 *	@param		video_enc_id	encodeID
 *	@param		enabled			enabled flag
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_H264_User_Record_Frame_Enabled( E_VIDEO_ENOCDE_ID video_enc_id, ULLONG frame_no, BOOL dummy_flg, BOOL enabled )
{
	E_H264_IF_STREAM_CH stream_ch;
	
	BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, &stream_ch );
	
	H264_If_Record_Frame_Enabled( stream_ch, frame_no, dummy_flg, enabled );
	
	return;
}

/**
 *	@brief		Free video stream.
 *	@param		i:vstream_free_info		Free video stream
 *	@return		D_BF_H264_USER_OK	(0)Normal End
 *	@return		D_BF_H264_USER_NG	(1)Error
 *	@note		None
 *	@attention	None
*/
INT32 BF_H264_User_Record_Free_Video_Index( E_VIDEO_ENOCDE_ID video_enc_id, UINT32 free_vstream_index, BOOL last_free_flg )
{
	E_H264_IF_STREAM_CH stream_ch;
	
	BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, &stream_ch );
	
//	printf("free: 0x%X, 0x%X\n", stream_ch, free_vstream_index);
	return H264_If_Record_Free_Video_Index( stream_ch, free_vstream_index, last_free_flg );
}

/**
 *	@brief		Free video stream.
 *	@param		i:vstream_free_info		Free video stream
 *	@return		D_BF_H264_USER_OK	(0)Normal End
 *	@return		D_BF_H264_USER_NG	(1)Error
 *	@note		None
 *	@attention	None
*/
VOID BF_H264_User_Record_Valid_Image_Address( E_VIDEO_ENOCDE_ID video_enc_id )
{
	E_H264_IF_STREAM_CH stream_ch;
	
	BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, &stream_ch );
	
	H264_If_Record_Valid_Image_Address( stream_ch );
	
	return;
}

VOID BF_H264_User_Record_Set_Gop_Size( E_H264_IF_STREAM_CH stream_ch, UCHAR gop_size )
{
	gH264_User_Record_Gop_Size[stream_ch] = gop_size;
		
	return;
}

UCHAR BF_H264_User_Record_Get_Gop_Size( E_H264_IF_STREAM_CH stream_ch )
{
	return gH264_User_Record_Gop_Size[stream_ch];
}

VOID BF_H264_User_Set_Codec_CoreMode( UINT32 core_mode )
{
	H264_If_Set_Codec_CoreMode( core_mode );
	return;
}


#if 0	// for Debug Dump
extern volatile	USHORT	gErrorInfo[];

ULONG BF_H264_User_Recovery_Copy( const ULONG error_factor, UCHAR* const top_addr, const ULONG area_bytes )
{
	T_BF_RIBERY_RECOVERY_AREA	recovery_area = {
		.addr = top_addr,	/* pgr0339 */
		.copy_bytes = 0ul,
		.last_bytes = area_bytes,	/* pgr0339 */
	};
	ULONG	work0_addr;
	ULONG	debug_area_addr;
	
	work0_addr      = Sdram_Map_Movie_Record_Get_H264_Stream_Work0();
	debug_area_addr = work0_addr + 0x88000;
	
	memcpy( recovery_area.addr, (CHAR*)debug_area_addr, ((32*1024) - (sizeof(USHORT)*40)) );
	recovery_area.addr += ((32*1024) - (sizeof(USHORT)*40));
	recovery_area.copy_bytes += ((32*1024) - (sizeof(USHORT)*40));
	
	memcpy( recovery_area.addr, (CHAR*)gErrorInfo, (sizeof(USHORT)*40) );
	recovery_area.addr += (sizeof(USHORT)*40);
	recovery_area.copy_bytes += (sizeof(USHORT)*40);
	
	return recovery_area.copy_bytes;
}
#endif

// ***************************************************************************
//                              Play part
// ***************************************************************************
/**
 *	@brief		Send Init Message to H.264 Middleware\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_H264_User_Play_Init( VOID )
{
	INT32							ret;
	OS_USER_FLGPTN					flgptn;
	static T_H264_IF_MSG			msg;
	static T_H264_IF_PARAM_INIT		init_param;
	static T_H264_IF_DISPLAY_PARAM	lcd_info;	// ** connectivity wo hukumete you kenntou
	static T_H264_IF_DISPLAY_PARAM	hdmi_info;
	
	BF_Debug_Print_Information(("BF_H264_User_Play_Init() Start\n"));
	
	if(gH264_User_Play_Exc != D_BF_H264_USER_PLAY_EXC_OFF){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Init : Play_Exc ERR. gH264_User_Play_Exc=%d\n", gH264_User_Play_Exc));
		return D_BF_H264_USER_NG;
	}
	
	// Reset Msg Box
	ret = H264_If_Clear_Msg();
	if( ret != D_OS_USER_E_TMOUT ){
		BF_Debug_Print_Error(("--- H264_If_Clear_Msg ERR. ret=%d\n", ret));
	}
	
	gH264_User_Play_Exc  = D_BF_H264_USER_PLAY_EXC_READY;
	
	/* Create Control Task */
	OS_User_Sta_Tsk(TID_H264_CONTROLLER, 0 );
	
	/* Send Message "Play Initialize" */
	memset( &lcd_info, 0, sizeof( T_H264_IF_DISPLAY_PARAM ) );
	memset( &hdmi_info, 0, sizeof( T_H264_IF_DISPLAY_PARAM ) );
	
	BF_Movie_Video_Attr_Get_H264_Decoder_Config( &init_param.decoder_param );
	
	/* Get Display Setting */
	ret = h264_user_play_get_display_setting( &lcd_info, &hdmi_info, &init_param.decoder_param );
	if( ret != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Init display setting ERR. ret=%d\n", ret));
		return D_BF_H264_USER_NG;
	}
	
	init_param.display_info.lcd_info	= lcd_info;
	init_param.display_info.hdmi_info	= hdmi_info;
	
	msg.type			= E_H264_IF_TYPE_PLAY_INITIALIZE;
	msg.data.pInitParam	= &init_param;
	msg.cbfunc			= NULL;
	
	OS_User_Clr_Flg( FID_DECODER, 0x0000 );
	
	ret = H264_If_Set_Msg(&msg, E_H264_IF_MSG_WAIT_NONBLOCK);
	if( ret != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Init Set Msg ERR. ret=%d\n", ret));
		return D_BF_H264_USER_NG;
	}
	
	// Wait complete flag for synchronization API.
	OS_User_Wai_Flg( FID_DECODER, D_BF_FLG_DECODER_STATE_CHANGE, D_OS_USER_TWF_ORW, &flgptn );
	OS_User_Clr_Flg( FID_DECODER, ~D_BF_FLG_DECODER_STATE_CHANGE );
	
	BF_Debug_Print_Information(("BF_H264_User_Play_Init() End\n"));
	
	return D_BF_H264_USER_OK;
}

/**
 *	@brief		Send Play Message to H.264 Middleware\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_H264_User_Play_Start( VOID )
{
	INT32							ret;
	OS_USER_FLGPTN					flgptn;
	static T_H264_IF_MSG			msg;
	static T_H264_IF_PARAM_PLAY		pb_param;
	
	E_FJ_MOVIE_PLAY_SPEED			speed;
	E_FJ_MOVIE_PLAY_DIRECTION		direction;
	BOOL							pause;
	
	BF_Debug_Print_Information(("BF_H264_User_Play_Start() Start\n"));
	BF_Debug_Print_Information(("\n"));
	
	if(gH264_User_Play_Exc != D_BF_H264_USER_PLAY_EXC_READY){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Start : Play_Exc ERR. gH264_User_Play_Exc=%d\n", gH264_User_Play_Exc));
		return D_BF_H264_USER_NG;
	}
	gH264_User_Play_Exc  = D_BF_H264_USER_PLAY_EXC_PLAYING;
	
	// Play Start Frame Setting
	BF_Movie_Video_Attr_Get_Decoder_Play_Start_Pts( &pb_param.start_pts );
	
	// Play Speed Setting
	BF_Movie_Video_Attr_Get_Decoder_Play_Speed( &speed );
	switch( speed ){
		case E_FJ_MOVIE_PLAY_SPEED_1X:
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
			BF_Debug_Print_Information(("Play Start PTS = %lu\n", (ULONG)pb_param.start_pts));
#else
			BF_Debug_Print_Information(("Play Start PTS = %llu\n", pb_param.start_pts));
#endif
			pb_param.speed = E_H264_IF_PLAY_SPEED_1X;
			BF_Debug_Print_Information(("Play Start Speed = 1x\n"));
			break;
		case E_FJ_MOVIE_PLAY_SPEED_2X:
			pb_param.speed = E_H264_IF_PLAY_SPEED_2X;
			BF_Debug_Print_Information(("Play Start Speed = 2x\n"));
			break;
		case E_FJ_MOVIE_PLAY_SPEED_4X:
			pb_param.speed = E_H264_IF_PLAY_SPEED_4X;
			BF_Debug_Print_Information(("Play Start Speed = 4x\n"));
			break;
		case E_FJ_MOVIE_PLAY_SPEED_8X:
			pb_param.speed = E_H264_IF_PLAY_SPEED_8X;
			BF_Debug_Print_Information(("Play Start Speed = 8x\n"));
			break;
		case E_FJ_MOVIE_PLAY_SPEED_1_2X:
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
			BF_Debug_Print_Information(("Play Start PTS = %lu\n", (ULONG)pb_param.start_pts));
#else
			BF_Debug_Print_Information(("Play Start PTS = %llu\n", pb_param.start_pts));
#endif
			pb_param.speed = E_H264_IF_PLAY_SPEED_1_2X;
			BF_Debug_Print_Information(("Play Start Speed = 1/2x\n"));
			break;
		case E_FJ_MOVIE_PLAY_SPEED_1_4X:
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
			BF_Debug_Print_Information(("Play Start PTS = %lu\n", (ULONG)pb_param.start_pts));
#else
			BF_Debug_Print_Information(("Play Start PTS = %llu\n", pb_param.start_pts));
#endif
			pb_param.speed = E_H264_IF_PLAY_SPEED_1_4X;
			BF_Debug_Print_Information(("Play Start Speed = 1/4x\n"));
			break;
		case E_FJ_MOVIE_PLAY_SPEED_1_8X:
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
			BF_Debug_Print_Information(("Play Start PTS = %lu\n", (ULONG)pb_param.start_pts));
#else
			BF_Debug_Print_Information(("Play Start PTS = %llu\n", pb_param.start_pts));
#endif
			pb_param.speed = E_H264_IF_PLAY_SPEED_1_8X;
			BF_Debug_Print_Information(("Play Start Speed = 1/8x\n"));
			break;
		default:
			BF_Debug_Print_Error(("BF_H264_User_Play_Start : speed param error.\n"));
			return D_BF_H264_USER_NG;
	}
	// Play Direction Setting
	BF_Movie_Video_Attr_Get_Decoder_Play_Direction( &direction );
	switch( direction ){
		case E_FJ_MOVIE_PLAY_DIRECTION_FWD:
			pb_param.direction = E_H264_IF_PLAY_DIRECTION_FWD;
			BF_Debug_Print_Information(("Play Start Direction = FWD\n"));
			break;
		case E_FJ_MOVIE_PLAY_DIRECTION_RWD:
			pb_param.direction = E_H264_IF_PLAY_DIRECTION_RWD;
			BF_Debug_Print_Information(("Play Start Direction = RWD\n"));
			break;
		default:
			BF_Debug_Print_Error(("BF_H264_User_Play_Start : direction param error.\n"));
			return D_BF_H264_USER_NG;
	}
	
	// Play Pause Setting
	BF_Movie_Video_Attr_Get_Decoder_Play_Pause( &pause );
	pb_param.pause = pause;
	
	msg.type			= E_H264_IF_TYPE_PLAY_START;
	msg.data.pPlayParam	= &pb_param;
	msg.cbfunc			= NULL;
	
	ret = H264_If_Set_Msg(&msg, E_H264_IF_MSG_WAIT_NONBLOCK);
	if( ret != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Start Set Msg ERR. ret=%d\n", ret));
		return D_BF_H264_USER_NG;
	}
	
	if( pause == TRUE ){
		// Wait complete flag for synchronization API.
		OS_User_Wai_Flg( FID_DECODER, D_BF_FLG_DECODER_STATE_CHANGE, D_OS_USER_TWF_ORW, &flgptn );
		OS_User_Clr_Flg( FID_DECODER, ~D_BF_FLG_DECODER_STATE_CHANGE );
	}
	
	BF_Debug_Print_Information(("\n"));
	BF_Debug_Print_Information(("BF_H264_User_Play_Start() End\n"));
	
	return D_BF_H264_USER_OK;
}

/**
 *	@brief		Send Pause Message to H.264 Middleware\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_H264_User_Play_Pause( VOID )
{
	INT32					ret = D_BF_H264_USER_OK;
	OS_USER_FLGPTN			flgptn;
	static T_H264_IF_MSG	msg;
	
	BF_Debug_Print_Information(("BF_H264_User_Play_Pause() Start\n"));
	
	if(gH264_User_Play_Exc != D_BF_H264_USER_PLAY_EXC_PLAYING){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Pause : Play_Exc ERR. gH264_User_Play_Exc=%d\n", gH264_User_Play_Exc));
		return D_BF_H264_USER_NG;
	}
	
	msg.type = E_H264_IF_TYPE_PLAY_PAUSE;
	msg.data.info = 0;
	msg.cbfunc = NULL;
	
	ret = H264_If_Set_Msg(&msg, E_H264_IF_MSG_WAIT_NONBLOCK); 
	if( ret != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Pause Send Msg ERR. ret=%d\n", ret));
		return D_BF_H264_USER_NG;
	}
	
	// Wait complete flag for synchronization API.
	OS_User_Wai_Flg( FID_DECODER, D_BF_FLG_DECODER_STATE_CHANGE, D_OS_USER_TWF_ORW, &flgptn );
	OS_User_Clr_Flg( FID_DECODER, ~D_BF_FLG_DECODER_STATE_CHANGE );
	
	BF_Debug_Print_Information(("BF_H264_User_Play_Pause() End\n"));
	
	return D_BF_H264_USER_OK;
}


/**
 *	@brief		Send Resume Message to H.264 Middleware\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_H264_User_Play_Resume( VOID )
{
	INT32					ret = D_BF_H264_USER_OK;
	static T_H264_IF_MSG	msg;
	
	BF_Debug_Print_Information(("BF_H264_User_Play_Resume() Start\n"));
	
	if(gH264_User_Play_Exc != D_BF_H264_USER_PLAY_EXC_PLAYING){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Resume : Play_Exc ERR. gH264_User_Play_Exc=%d\n", gH264_User_Play_Exc));
		return D_BF_H264_USER_NG;
	}
	
	msg.type = E_H264_IF_TYPE_PLAY_RESUME;
	msg.data.info = 0;
	msg.cbfunc = NULL;
	
	ret = H264_If_Set_Msg(&msg, E_H264_IF_MSG_WAIT_NONBLOCK);
	if( ret != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Resume Send Msg ERR. ret=%d\n", ret));
		return D_BF_H264_USER_NG;
	}
	
	BF_Debug_Print_Information(("BF_H264_User_Play_Resume() End\n"));
	
	return D_BF_H264_USER_OK;
}


/**
 *	@brief		Send Stop Message to H.264 Middleware\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_H264_User_Play_Stop( BOOL last_picture_keep )
{
	INT32								ret;
	OS_USER_FLGPTN						flgptn;
	static T_H264_IF_MSG				msg;
	static T_H264_IF_PARAM_PLAY_STOP	pPlayStopParam;
	
	if( (gH264_User_Play_Exc != D_BF_H264_USER_PLAY_EXC_READY) && (gH264_User_Play_Exc != D_BF_H264_USER_PLAY_EXC_PLAYING) ){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Stop : Play_Exc ERR. gH264_User_Play_Exc=%d\n", gH264_User_Play_Exc));
		return D_BF_H264_USER_NG_ALREADY_STOP_PLAY;
	}
	
	pPlayStopParam.last_picture_keep = last_picture_keep;
	
	msg.type				= E_H264_IF_TYPE_PLAY_STOP;
	msg.data.pPlayStopParam	= &pPlayStopParam;
	msg.cbfunc				= NULL;
	
	OS_User_Clr_Flg( FID_DECODER, ~D_BF_FLG_DECODER_STATE_CHANGE );
	ret = H264_If_Set_Msg(&msg, E_H264_IF_MSG_WAIT_NONBLOCK);
	if( ret != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Stop Set Msg ERR. ret=%d\n", ret));
	}
	
	// Wait complete flag for synchronization API.
	OS_User_Wai_Flg( FID_DECODER, D_BF_FLG_DECODER_STATE_CHANGE, D_OS_USER_TWF_ORW, &flgptn );
	OS_User_Clr_Flg( FID_DECODER, ~D_BF_FLG_DECODER_STATE_CHANGE );
	
	/* Send Message "Play Finalize" */
	msg.type		= E_H264_IF_TYPE_PLAY_FINALIZE;
	msg.data.info	= 0;
	msg.cbfunc		= h264_user_play_finalize_callback;
	
	OS_User_Clr_Flg( FID_DECODER, ~D_BF_FLG_DECODER_STATE_CHANGE );
	ret = H264_If_Set_Msg(&msg, E_H264_IF_MSG_WAIT_NONBLOCK);
	if( ret != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Stop Set Msg ERR. ret=%d\n", ret));
	}
	
	// Wait complete flag for synchronization API.
	OS_User_Wai_Flg( FID_DECODER, D_BF_FLG_DECODER_STATE_CHANGE, D_OS_USER_TWF_ORW, &flgptn );
	OS_User_Clr_Flg( FID_DECODER, ~D_BF_FLG_DECODER_STATE_CHANGE );
	
	gH264_User_Play_Exc	= D_BF_H264_USER_PLAY_EXC_OFF;
	
	BF_H264_User_Notify_VideoDecodeCB( E_H264_IF_PLAY_DECODER_STATUS_IDLE , 0, FALSE );
	
	return D_BF_H264_USER_OK;
}

BOOL BF_H264_User_Is_Play_Stop( VOID )
{
	return gH264_User_Play_Exc == D_BF_H264_USER_PLAY_EXC_OFF ? TRUE : FALSE;
}

/**
 *	@brief		Send Flush Message to H.264 Middleware\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_H264_User_Play_Flush( VOID )
{
	INT32					ret = D_BF_H264_USER_OK;
	OS_USER_FLGPTN			flgptn;
	static T_H264_IF_MSG	msg;
	
	BF_Debug_Print_Information(("BF_H264_User_Play_Flush() Start\n"));
	
	if(gH264_User_Play_Exc != D_BF_H264_USER_PLAY_EXC_PLAYING){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Flush : Play_Exc ERR. gH264_User_Play_Exc=%d\n", gH264_User_Play_Exc));
		return D_BF_H264_USER_NG;
	}
	
	gH264_User_Play_Exc  = D_BF_H264_USER_PLAY_EXC_READY;
	
	OS_User_Clr_Flg( FID_DECODER, ~D_BF_FLG_DECODER_STATE_CHANGE );
	
	msg.type = E_H264_IF_TYPE_PLAY_FLUSH;
	msg.data.info = 0;
	msg.cbfunc = NULL;
	
	ret = H264_If_Set_Msg(&msg, E_H264_IF_MSG_WAIT_NONBLOCK); 
	if( ret != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Flush Send Msg ERR. ret=%d\n", ret));
		return D_BF_H264_USER_NG;
	}
	
	// Wait complete flag for synchronization API.
	OS_User_Wai_Flg( FID_DECODER, D_BF_FLG_DECODER_STATE_CHANGE, D_OS_USER_TWF_ORW, &flgptn );
	OS_User_Clr_Flg( FID_DECODER, ~D_BF_FLG_DECODER_STATE_CHANGE );
	
	BF_H264_User_Notify_VideoDecodeCB( E_H264_IF_PLAY_DECODER_STATUS_IDLE , 0, FALSE );
	
	BF_Debug_Print_Information(("BF_H264_User_Play_Flush() End\n"));
	
	return D_BF_H264_USER_OK;
}

/**
 *	@brief		Set Video Stream to H.264 Middleware\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_H264_User_Play_Set_Video_Stream( UINT32 store_num, T_H264_IF_PARAM_SET_STREAM* video_stream_param )
{
	INT32 ret;
	
	ret = H264_If_Play_Set_Video_Stream( store_num, video_stream_param );
	if( ret != H264_OK ){
//		BF_Debug_Print_Error(("--- BF_H264_User_Play_Set_Video_Stream Error [%d]\n", ret));
		return D_BF_H264_USER_NG;
	}
	
	return D_BF_H264_USER_OK;
}


//#pragma arm section code="BASEFW_PB_CODE"

/**
 *	@brief		Get the first frame of h264 file.\n
 *	@param		None
 *	@return		D_BF_H264_USER_OK				(0)Normal End
 *	@note		None
 *	@attention	If user change to Play Mode, it have to call this method.
*/
INT32 BF_H264_User_Play_Top( BOOL display, ULLONG req_pts, UINT32 store_num, T_H264_IF_PARAM_SET_STREAM* video_stream_param, T_H264_IF_VIDEO_YUV_INFO* decode_yuv_info )
{
	INT32						top_get_ret = D_BF_H264_USER_NG;
	INT32						ret = D_BF_H264_USER_NG;
	OS_USER_FLGPTN				flgptn;
	T_H264_IF_MSG				msg;
	T_H264_IF_PARAM_TOPIMAGE	top_param;
	T_H264_IF_DISPLAY_PARAM		lcd_info;
	T_H264_IF_DISPLAY_PARAM		hdmi_info;
	FJ_DISP_YCC_ADDR			display_addr;
	FJ_DISP_ROTATE_DEGREE		rotate_degree = FJ_DISP_ROTATE_DEGREE_0;
	T_BF_H264_USER_RESIZE_TBL	resize_tbl;
	
	if( gH264_User_Play_Exc != D_BF_H264_USER_PLAY_EXC_READY ){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Top : Play_Exc ERR. gH264_User_Play_Exc=%d\n", gH264_User_Play_Exc));
		return D_BF_H264_USER_NG;
	}
	
	gH264_User_Play_Exc = D_BF_H264_USER_PLAY_EXC_PLAYING;
	
	top_param.req_pts				= req_pts;
	top_param.store_num				= store_num;
	top_param.video_stream_param	= video_stream_param;
	top_param.decode_yuv_info		= decode_yuv_info;
	
	msg.type = E_H264_IF_TYPE_PLAY_TOPIMAGE;
	msg.data.pTopimageParam = &top_param;
	msg.cbfunc = NULL;
	
	OS_User_Clr_Flg( FID_DECODER, ~D_BF_FLG_DECODER_STATE_CHANGE );
	top_get_ret = H264_If_Set_Msg(&msg, E_H264_IF_MSG_WAIT_NONBLOCK); 
	if( top_get_ret != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Top: Topimage Send Msg ERR.\n"));
		top_get_ret = D_BF_H264_USER_NG;
	}
	
	// Wait complete flag for synchronization API.
	OS_User_Wai_Flg( FID_DECODER, D_BF_FLG_DECODER_STATE_CHANGE, D_OS_USER_TWF_ORW, &flgptn );
	OS_User_Clr_Flg( FID_DECODER, ~D_BF_FLG_DECODER_STATE_CHANGE );
	
	/* Send Message "Play Finalize" */
	msg.type		= E_H264_IF_TYPE_PLAY_FINALIZE;
	msg.data.info	= 0;
	msg.cbfunc		= h264_user_play_finalize_callback;
	
	OS_User_Clr_Flg( FID_DECODER, ~D_BF_FLG_DECODER_STATE_CHANGE );
	ret = H264_If_Set_Msg(&msg, E_H264_IF_MSG_WAIT_NONBLOCK);
	if( ret != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Top Set Msg ERR. ret=%d\n", ret));
	}
	
	// Wait complete flag for synchronization API.
	OS_User_Wai_Flg( FID_DECODER, D_BF_FLG_DECODER_STATE_CHANGE, D_OS_USER_TWF_ORW, &flgptn );
	OS_User_Clr_Flg( FID_DECODER, ~D_BF_FLG_DECODER_STATE_CHANGE );
	
	if( top_get_ret != D_BF_H264_USER_OK ){
		ret = D_BF_H264_USER_NG;
	}
	
	gH264_User_Play_Exc = D_BF_H264_USER_PLAY_EXC_OFF;
	
	if( display == TRUE ){
		
		// get display device setting
		h264_user_play_get_display_setting( &lcd_info, &hdmi_info, NULL );
		
		if( lcd_info.valid_flag == TRUE ){
			
			BF_H264_User_Notify_Lcd_Update( &display_addr.y_addr, &display_addr.cbcr_addr, TRUE );
			
			// Resize settings
			resize_tbl.src_width	= decode_yuv_info->width;
			resize_tbl.src_lines	= decode_yuv_info->lines;
			resize_tbl.g_src_width	= decode_yuv_info->g_width;
			resize_tbl.g_src_lines	= decode_yuv_info->g_lines;
			resize_tbl.src_addr[0]	= decode_yuv_info->y_addr;
			resize_tbl.src_addr[1]	= decode_yuv_info->cbcr_addr;
			
			resize_tbl.dst_width	= lcd_info.image_width;
			resize_tbl.dst_lines	= lcd_info.image_lines;
			resize_tbl.g_dst_width	= lcd_info.global_width;
			resize_tbl.g_dst_lines	= lcd_info.global_lines;
			resize_tbl.dst_addr[0]	= display_addr.y_addr;
			resize_tbl.dst_addr[1]	= display_addr.cbcr_addr;
			
			resize_tbl.ribery_mode	= TRUE;
			resize_tbl.resize_mode	= E_BF_H264_USER_RESIZE_MODE_BILINEAR;
			
			fj_disp_video_get_rotate( lcd_info.disp_id, &rotate_degree );
			switch( rotate_degree ){
				case FJ_DISP_ROTATE_DEGREE_0:
					resize_tbl.rotate		= E_BF_H264_USER_ROTATE_0;
					break;
				case FJ_DISP_ROTATE_DEGREE_90:
					resize_tbl.rotate		= E_BF_H264_USER_ROTATE_90;
					break;
				case FJ_DISP_ROTATE_DEGREE_180:
					resize_tbl.rotate		= E_BF_H264_USER_ROTATE_180;
					break;
				case FJ_DISP_ROTATE_DEGREE_270:
					resize_tbl.rotate		= E_BF_H264_USER_ROTATE_270;
					break;
				default:
					; // DO NOTHING
					break;
			}
			// resize
			h264_user_resize_rotate( &resize_tbl );
			
			BF_Disp_Set_Main_Addr( lcd_info.disp_id, &display_addr, E_BF_DISP_IMAGE_FORMAT_420, E_BF_DISP_YCC_COLOR_SPACE_601, lcd_info.global_width );
		}
		
		if( hdmi_info.valid_flag == TRUE ){
			
			BF_H264_User_Notify_Hdmi_Update( &display_addr.y_addr, &display_addr.cbcr_addr, TRUE );
			
			// Resize settings
			resize_tbl.src_width	= decode_yuv_info->width;
			resize_tbl.src_lines	= decode_yuv_info->lines;
			resize_tbl.g_src_width	= decode_yuv_info->g_width;
			resize_tbl.g_src_lines	= decode_yuv_info->g_lines;
			resize_tbl.src_addr[0]	= decode_yuv_info->y_addr;
			resize_tbl.src_addr[1]	= decode_yuv_info->cbcr_addr;
			
			resize_tbl.dst_width	= hdmi_info.image_width;
			resize_tbl.dst_lines	= hdmi_info.image_lines;
			resize_tbl.g_dst_width	= hdmi_info.global_width;
			resize_tbl.g_dst_lines	= hdmi_info.global_lines;
			resize_tbl.dst_addr[0]	= display_addr.y_addr;
			resize_tbl.dst_addr[1]	= display_addr.cbcr_addr;
			
			resize_tbl.ribery_mode	= TRUE;
			resize_tbl.resize_mode	= E_BF_H264_USER_RESIZE_MODE_BILINEAR;
			
			fj_disp_video_get_rotate( hdmi_info.disp_id, &rotate_degree );
			switch( rotate_degree ){
				case FJ_DISP_ROTATE_DEGREE_0:
					resize_tbl.rotate		= E_BF_H264_USER_ROTATE_0;
					break;
				case FJ_DISP_ROTATE_DEGREE_90:
					resize_tbl.rotate		= E_BF_H264_USER_ROTATE_90;
					break;
				case FJ_DISP_ROTATE_DEGREE_180:
					resize_tbl.rotate		= E_BF_H264_USER_ROTATE_180;
					break;
				case FJ_DISP_ROTATE_DEGREE_270:
					resize_tbl.rotate		= E_BF_H264_USER_ROTATE_270;
					break;
				default:
					; // DO NOTHING
					break;
			}
			// resize
			h264_user_resize_rotate( &resize_tbl );
			
			BF_Disp_Set_Main_Addr( hdmi_info.disp_id, &display_addr, E_BF_DISP_IMAGE_FORMAT_420, E_BF_DISP_YCC_COLOR_SPACE_601, hdmi_info.global_width );
		}
	}
	
	return ret;
}


/**
 *	@brief		Get the display frame.\n
 *	@param		None
 *	@return		D_BF_H264_USER_OK				(0)Normal End
 *	@note		None.
 *	@attention	None.
*/
INT32 BF_H264_User_Play_Get_Display_Frame( T_H264_IF_VIDEO_YUV_INFO* display_yuv_info )
{
	INT32	ret = D_BF_H264_USER_NG;
	
	if( gH264_User_Play_Exc != D_BF_H264_USER_PLAY_EXC_PLAYING ){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Get_Display_Frame : Play_Exc ERR. gH264_User_Play_Exc=%d\n", gH264_User_Play_Exc));
		return D_BF_H264_USER_NG;
	}
	
	ret = H264_If_Play_Get_Display_Frame( display_yuv_info );
	if( ret != D_OS_USER_E_OK ){
		BF_Debug_Print_Error(("--- BF_H264_User_Play_Get_Display_Frame: Get Display Frame Error. ret=%d\n", ret));
		return D_BF_H264_USER_NG;
	}
	
	return D_BF_H264_USER_OK;
}


INT32 h264_user_play_get_display_setting( T_H264_IF_DISPLAY_PARAM* lcd_info, T_H264_IF_DISPLAY_PARAM* hdmi_info, T_H264_IF_DECODER_PARAM* decoder_param )
{
	T_BF_INSTANCE_VIDEO_DECODE*		decode_instance;
	FJ_DISP_ROTATE_DEGREE			rotate_degree = FJ_DISP_ROTATE_DEGREE_0;
	UINT32							output_loop;
	
	decode_instance = BF_Instance_Get_VideoDecode( (UCHAR)E_FJ_VIDEO_DEC_H264 );
	
	if( decode_instance->flag == E_BF_INSTANCE_FLAG_UNUSE ){
		BF_Debug_Print_Error(("h264_user_play_get_display_setting : Not connected.\n" ));
		return D_BF_H264_USER_NG;
	}
	
	for(output_loop=0; output_loop<D_BF_INSTANCE_OUTPUT_MAX_VIDEO_DEC; output_loop++){
		
		if ( decode_instance->connect.display[output_loop].instance_p != NULL ) {
			
			if( ((T_BF_INSTANCE_DISPLAY *)(decode_instance->connect.display[output_loop].instance_p))->flag == E_BF_INSTANCE_FLAG_USE ){
				
				switch( ((T_BF_INSTANCE_DISPLAY *)(decode_instance->connect.display[output_loop].instance_p))->type ){
					case FJ_DISP_TYPE_LCD:
						lcd_info->valid_flag	= TRUE;				// LCD
						lcd_info->disp_id		= decode_instance->connect.display[output_loop].instance_id;
						lcd_info->image_width	= ((T_BF_INSTANCE_DISPLAY *)(decode_instance->connect.display[output_loop].instance_p))->window.input.width;
						lcd_info->image_lines	= ((T_BF_INSTANCE_DISPLAY *)(decode_instance->connect.display[output_loop].instance_p))->window.input.lines;

						if (User_IsDualStream()) {
							hdmi_info->global_width	= 640;
							hdmi_info->global_lines	= 480;
						} else {
							if( (decoder_param != NULL) &&
								(lcd_info->image_width == decoder_param->image_width) &&
								(lcd_info->image_lines == decoder_param->image_lines) ){

								lcd_info->global_width	= decoder_param->image_bank_y_width;	// use Ribery Area (DISP Resize)
								lcd_info->global_lines	= decoder_param->image_bank_y_lines;
							}
							else{
								lcd_info->global_width	= 640;					// use Output Area (IIP Resize)
								lcd_info->global_lines	= 480;
							}
						}
						
						fj_disp_video_get_rotate( lcd_info->disp_id, &rotate_degree );
						switch( rotate_degree ){
							case FJ_DISP_ROTATE_DEGREE_0:
								lcd_info->rotate = E_H264_IF_DISP_ROTATE_0;
								break;
							case FJ_DISP_ROTATE_DEGREE_90:
								lcd_info->rotate = E_H264_IF_DISP_ROTATE_90;
								break;
							case FJ_DISP_ROTATE_DEGREE_180:
								lcd_info->rotate = E_H264_IF_DISP_ROTATE_180;
								break;
							case FJ_DISP_ROTATE_DEGREE_270:
								lcd_info->rotate = E_H264_IF_DISP_ROTATE_270;
								break;
							default:
								; // DO NOTHING
								break;
						}
						
						break;
					case FJ_DISP_TYPE_HDMI:
						hdmi_info->valid_flag	= TRUE;				// HDMI
						hdmi_info->disp_id		= decode_instance->connect.display[output_loop].instance_id;
						hdmi_info->image_width	= ((T_BF_INSTANCE_DISPLAY *)(decode_instance->connect.display[output_loop].instance_p))->window.input.width;
						hdmi_info->image_lines	= ((T_BF_INSTANCE_DISPLAY *)(decode_instance->connect.display[output_loop].instance_p))->window.input.lines;

						if (User_IsDualStream()) {
							hdmi_info->global_width	= SDRAM_WIDTH_PLAY_HDMI_MAIN;
							hdmi_info->global_lines	= SDRAM_LINES_PLAY_HDMI_MAIN;
						} else {
							if( (decoder_param != NULL) &&
								(hdmi_info->image_width == decoder_param->image_width) &&
								(hdmi_info->image_lines == decoder_param->image_lines) ){

								hdmi_info->global_width	= decoder_param->image_bank_y_width;	// use Ribery Area (DISP Resize)
								hdmi_info->global_lines	= decoder_param->image_bank_y_lines;
							}
							else{
								hdmi_info->global_width	= SDRAM_WIDTH_PLAY_HDMI_MAIN;			// use Output Area (IIP Resize)
								hdmi_info->global_lines	= SDRAM_LINES_PLAY_HDMI_MAIN;
							}
						}
						
						fj_disp_video_get_rotate( hdmi_info->disp_id, &rotate_degree );
						switch( rotate_degree ){
							case FJ_DISP_ROTATE_DEGREE_0:
								hdmi_info->rotate = E_H264_IF_DISP_ROTATE_0;
								break;
							case FJ_DISP_ROTATE_DEGREE_90:
								hdmi_info->rotate = E_H264_IF_DISP_ROTATE_90;
								break;
							case FJ_DISP_ROTATE_DEGREE_180:
								hdmi_info->rotate = E_H264_IF_DISP_ROTATE_180;
								break;
							case FJ_DISP_ROTATE_DEGREE_270:
								hdmi_info->rotate = E_H264_IF_DISP_ROTATE_270;
								break;
							default:
								; // DO NOTHING
								break;
						}
						
						break;
					default:
						; // DO NOTHING
						break;
				}
			}
		}
	}
	return D_BF_H264_USER_OK;
}


// ***************************************************************************
//                    H.264 middleware "call back" part
// ***************************************************************************
static VOID h264_user_record_initialize_callback(int type, int param)
{
	if( param != H264_OK ){
		BF_Debug_Print_Error(("Record Initialize ERR type=%d, param=%d\n", type, param ));
		h264_user_record_start_err_notify();
		return;
	}
	gH264_User_Record_Exc = D_BF_H264_USER_RECORD_EXC_READY;
	OS_User_Set_Flg( FID_ENCODER, D_BF_FLG_SETUP_COMPLETE_H264 );
}

static VOID h264_user_record_start_video_callback(int type, int param)
{
	if( param != H264_OK ){
		BF_Debug_Print_Error(("Record Start ERR type=%d, param=%d\n", type, param ));
		h264_user_record_start_err_notify();
		return;
	}
	
	BF_Debug_Print_Info(("H264 Record Start OK\n"));
}

static VOID h264_user_record_start_video_callback_sync(int type, int param)
{
	if( param != H264_OK ){
		BF_Debug_Print_Error(("Record Start ERR type=%d, param=%d\n", type, param ));
		h264_user_record_start_err_notify();
		return;
	}

	{
		OS_USER_FLGPTN flgptn;
		INT32 ercd;
		
		BF_Debug_Print_Info(("H264 Wait Sync Start\n"));
		ercd = OS_User_Twai_Flg( FID_ENCODER, D_BF_FLG_START_SYNC_H264, D_OS_USER_TWF_ORW, &flgptn, 300 );
		if( ercd != D_OS_USER_E_OK ){
			BF_Debug_Print_Error(("+++ H264 Wait Sync Start: Time out ercd=%x\n", ercd ));
		}
	}
	
	BF_Debug_Print_Info(("H264 Record Start OK\n"));
	OS_User_Set_Flg( FID_ENCODER, D_BF_FLG_START_COMPLETE_H264 );
}

static VOID h264_user_record_stop_video_callback(int type, int param)
{
	if( param != H264_OK ){
		BF_Debug_Print_Error(("Record Stop ERR type=%d, param=%d\n", type, param ));
		return;
	}
	return;
}

static VOID h264_user_play_finalize_callback(int type, int param)
{
	if( param != H264_OK ){
		BF_Debug_Print_Error(("Record Stop ERR type=%d, param=%d\n", type, param ));
		return;
	}
	OS_User_Set_Flg( FID_DECODER, D_BF_FLG_DECODER_STATE_CHANGE );
	return;
}


INT32 h264_user_resize_rotate( T_BF_H264_USER_RESIZE_TBL* resize_tbl )
{
	INT32						result = D_BF_H264_USER_OK;
#ifndef CO_H264_PORTING_BLOCK
	INT32						ret = D_BF_H264_USER_OK;
	UCHAR						param_buffer[(D_IM_IIP_UTIL_RR_BUF_BYTES*2) + D_BF_H264_USER_IIP_TBL_ALIGN];
	ULONG						param_buffer_addr = M_BF_H264_USER_IIP_ROUNDUP_ALIGN( (ULONG)param_buffer );	// Convet 8bytes align
	INT32						master_IF = D_IM_IIP_MASTER_IF0;
	INT32						wait_time = 200;
	INT32						set_loop;
	INT32						src_width, src_lines;
	INT32						g_src_width;
	INT32						dst_width, dst_lines;
	INT32						g_dst_width;
	ULONG						dst_y_offset;
	ULONG						dst_c_offset;
	
	T_FJ_IIP_RES_CONFIG_ARG		iip_res_config_arg;
	T_FJ_IIP_RES_CONFIG_OUT		iip_res_config_out;
	T_FJ_IIP_RES_CONFIG_OUT		iip_res_get_resource;
	static T_IM_IIP_UTIL_RR		cfg;
	
	if( resize_tbl == NULL ){
		BF_Debug_Print_Error(("--- h264_user_resize_rotate : resize_tbl is NULL.\n"));
		return D_BF_H264_USER_NG;
	}
	
	while(1){
		
		dst_y_offset = (resize_tbl->dst_addr[0] & 0x0000000F);
		dst_c_offset = (resize_tbl->dst_addr[1] & 0x0000000F);
		
		memset( &iip_res_config_arg,   0, sizeof( T_FJ_IIP_RES_CONFIG_ARG ));
		memset( &iip_res_config_out, 0, sizeof( T_FJ_IIP_RES_CONFIG_OUT ));
		
		iip_res_config_arg.unit.afn				= 1;
		iip_res_config_arg.unit.sl_master_if_0	= 1;
		iip_res_config_arg.pixid				= 2;
		
		// IIP Resource Get
		ret = BF_IIP_Res_Config_Lock_BaseFW( &iip_res_config_out, &iip_res_config_arg, 1, wait_time );
		if( ret != D_FJ_IIP_ERR_OK ){
			BF_Debug_Print_Error(("--- h264_user_resize_rotate : BF_IIP_Res_Config_Lock_BaseFW Error=%d\n", ret));
			result = D_BF_H264_USER_NG;
			break;
		}
		
		// saves local
		iip_res_get_resource = iip_res_config_out;
		
		// afn unit id get
		if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_AFN0 ){
			cfg.afn_unitid = E_IM_IIP_UNIT_ID_AFN0;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_AFN0;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_AFN1 ){
			cfg.afn_unitid = E_IM_IIP_UNIT_ID_AFN1;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_AFN1;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_AFN2 ){
			cfg.afn_unitid = E_IM_IIP_UNIT_ID_AFN2;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_AFN2;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_AFN3 ){
			cfg.afn_unitid = E_IM_IIP_UNIT_ID_AFN3;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_AFN3;
		}
		else{
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
			BF_Debug_Print_Error(("--- h264_user_resize_rotate : IIP Resource(afn) Get Error=%lu\n", (ULONG)iip_res_get_resource.unitid_bitmask64));
#else
			BF_Debug_Print_Error(("--- h264_user_resize_rotate : IIP Resource(afn) Get Error=%llu\n", iip_res_get_resource.unitid_bitmask64));
#endif
			result = D_BF_H264_USER_NG;
			break;
		}
		
		// sl unit(memory) id get
		if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL3 ){
			cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL3;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL3;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL4 ){
			cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL4;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL4;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL5 ){
			cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL5;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL5;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL6 ){
			cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL6;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL6;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL7 ){
			cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL7;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL7;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL8 ){
			cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL8;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL8;
		}
		else{
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
			BF_Debug_Print_Error(("--- h264_user_resize_rotate : IIP Resource(sl) Get Error=%lu\n", (ULONG)iip_res_get_resource.unitid_bitmask64));
#else
			BF_Debug_Print_Error(("--- h264_user_resize_rotate : IIP Resource(sl) Get Error=%llu\n", iip_res_get_resource.unitid_bitmask64));
#endif
			result = D_BF_H264_USER_NG;
			break;
		}
		
		// pixel id get
		set_loop = 0;
		for( ; set_loop < D_BF_H264_USER_IIP_PIXID_MAX; set_loop++ ){
			if( iip_res_get_resource.pixid_bitmask & (1 << set_loop) ){
				cfg.src_pixid = (E_IM_IIP_PIXID)(1 << set_loop);
				iip_res_get_resource.pixid_bitmask &= ~(1 << set_loop);
				break;
			}
		}
		for( ; set_loop < D_BF_H264_USER_IIP_PIXID_MAX; set_loop++ ){
			if( iip_res_get_resource.pixid_bitmask & (1 << set_loop) ){
				cfg.dst_pixid = (E_IM_IIP_PIXID)(1 << set_loop);
				iip_res_get_resource.pixid_bitmask &= ~(1 << set_loop);
				break;
			}
		}
		if( set_loop == D_BF_H264_USER_IIP_PIXID_MAX ){
			BF_Debug_Print_Error(("--- h264_user_resize_rotate : IIP Resource PIXID Get NG!!\n"));
			result = D_BF_H264_USER_NG;
			break;
		}
		if( (iip_res_get_resource.unitid_bitmask64 != 0) || (iip_res_get_resource.pixid_bitmask != 0) ){
			BF_Debug_Print_Error(("--- h264_user_resize_rotate : IIP Resource Abnormal!! \n"));
			result = D_BF_H264_USER_NG;
			break;
		}
		
		cfg.bi_cubic_unitid						= E_IM_IIP_PARAM_SCUB_0;
		cfg.afn_cache_select					= E_IM_IIP_PARAM_CSEL_0;
		
		src_width								= resize_tbl->src_width;
		src_lines								= resize_tbl->src_lines;
		g_src_width								= resize_tbl->g_src_width;
		
		cfg.src.rect.left						= 0.0;
		cfg.src.rect.top						= 0.0;
		cfg.src.rect.width						= src_width;
		cfg.src.rect.lines						= src_lines;
		
		cfg.src.gbl.alpha						= D_IM_IIP_ALPHA_FALSE;
		cfg.src.gbl.frame_type					= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
		if(resize_tbl->ribery_mode == TRUE){
			cfg.src.gbl.pix_format				= E_IM_IIP_PFMT_VIDEO;
		}
		else{
			cfg.src.gbl.pix_format				= E_IM_IIP_PFMT_YCC420;
		}
		cfg.src.gbl.chunky_color.component0		= D_IM_IIP_CHUNKY_COLOR_Y0_G;
		cfg.src.gbl.chunky_color.component1		= D_IM_IIP_CHUNKY_COLOR_CB_B;
		cfg.src.gbl.chunky_color.component2		= D_IM_IIP_CHUNKY_COLOR_CR_R;
		cfg.src.gbl.chunky_color.component3		= D_IM_IIP_CHUNKY_COLOR_Y1_A;
		cfg.src.gbl.sign_Y_G					= D_IM_IIP_UNSIGNED_DATA;
		cfg.src.gbl.sign_Cb_B					= D_IM_IIP_UNSIGNED_DATA;
		cfg.src.gbl.sign_Cr_R					= D_IM_IIP_UNSIGNED_DATA;
		cfg.src.gbl.sign_D3						= D_IM_IIP_UNSIGNED_DATA;
		cfg.src.gbl.width						= src_width;
		cfg.src.gbl.lines						= src_lines;
		cfg.src.gbl.line_bytes.Y_G				= g_src_width;
		cfg.src.gbl.line_bytes.Cb_B				= g_src_width;
		cfg.src.gbl.line_bytes.Cr_R				= g_src_width;
		cfg.src.gbl.line_bytes.Alpha			= 0;
		cfg.src.gbl.addr.Y_G					= resize_tbl->src_addr[0];
		cfg.src.gbl.addr.Cb_B					= resize_tbl->src_addr[1];
		cfg.src.gbl.addr.Cr_R					= resize_tbl->src_addr[1];
		cfg.src.gbl.addr.Alpha					= 0;
		
		if( (cfg.sl_unitid == E_IM_IIP_UNIT_ID_SL3) || (cfg.sl_unitid == E_IM_IIP_UNIT_ID_SL6) ){
			cfg.src.gbl.masterIF_Y_G			= D_IM_IIP_MASTER_IF1;
			cfg.src.gbl.masterIF_Cb_B			= D_IM_IIP_MASTER_IF1;
			cfg.src.gbl.masterIF_Cr_R			= D_IM_IIP_MASTER_IF1;
			cfg.src.gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF1;
		}
		else{
			cfg.src.gbl.masterIF_Y_G			= D_IM_IIP_MASTER_IF0;
			cfg.src.gbl.masterIF_Cb_B			= D_IM_IIP_MASTER_IF0;
			cfg.src.gbl.masterIF_Cr_R			= D_IM_IIP_MASTER_IF0;
			cfg.src.gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF0;
		}
		
		dst_width								= resize_tbl->dst_width;
		dst_lines								= resize_tbl->dst_lines;
		g_dst_width								= resize_tbl->g_dst_width;
		
		if( g_dst_width % 8 ){
			BF_Debug_Print_Warning(("h264_user_resize_rotate: the value of the global width(%d) is not divisible by 8.\n", g_dst_width ));
		}
		
		cfg.dst.rect.left						= 0.0;
		cfg.dst.rect.top						= 0.0;
		cfg.dst.rect.width						= dst_width;
		cfg.dst.rect.lines						= dst_lines;
		
		cfg.dst.gbl.alpha						= D_IM_IIP_ALPHA_FALSE;
		cfg.dst.gbl.frame_type					= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
		cfg.dst.gbl.pix_format					= E_IM_IIP_PFMT_YCC420;
		cfg.dst.gbl.chunky_color.component0		= D_IM_IIP_CHUNKY_COLOR_Y0_G;
		cfg.dst.gbl.chunky_color.component1		= D_IM_IIP_CHUNKY_COLOR_CB_B;
		cfg.dst.gbl.chunky_color.component2		= D_IM_IIP_CHUNKY_COLOR_CR_R;
		cfg.dst.gbl.chunky_color.component3		= D_IM_IIP_CHUNKY_COLOR_Y1_A;
		cfg.dst.gbl.sign_Y_G					= D_IM_IIP_UNSIGNED_DATA;
		cfg.dst.gbl.sign_Cb_B					= D_IM_IIP_UNSIGNED_DATA;
		cfg.dst.gbl.sign_Cr_R					= D_IM_IIP_UNSIGNED_DATA;
		cfg.dst.gbl.sign_D3						= D_IM_IIP_UNSIGNED_DATA;
		cfg.dst.gbl.width						= dst_width;
		cfg.dst.gbl.lines						= dst_lines;
		cfg.dst.gbl.line_bytes.Y_G				= g_dst_width;
		cfg.dst.gbl.line_bytes.Cb_B				= g_dst_width;
		cfg.dst.gbl.line_bytes.Cr_R				= g_dst_width;
		cfg.dst.gbl.line_bytes.Alpha			= 0;
		cfg.dst.gbl.addr.Y_G					= resize_tbl->dst_addr[0] - dst_y_offset;
		cfg.dst.gbl.addr.Cb_B					= resize_tbl->dst_addr[1] - dst_c_offset;
		cfg.dst.gbl.addr.Cr_R					= resize_tbl->dst_addr[1] - dst_c_offset;
		cfg.dst.gbl.addr.Alpha					= 0;
		
		if( (cfg.sl_unitid == E_IM_IIP_UNIT_ID_SL3) || (cfg.sl_unitid == E_IM_IIP_UNIT_ID_SL6) ){
			cfg.dst.gbl.masterIF_Y_G			= D_IM_IIP_MASTER_IF0;
			cfg.dst.gbl.masterIF_Cb_B			= D_IM_IIP_MASTER_IF0;
			cfg.dst.gbl.masterIF_Cr_R			= D_IM_IIP_MASTER_IF0;
			cfg.dst.gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF0;
		}
		if( (cfg.sl_unitid == E_IM_IIP_UNIT_ID_SL5) || (cfg.sl_unitid == E_IM_IIP_UNIT_ID_SL8) ){
			cfg.dst.gbl.masterIF_Y_G			= D_IM_IIP_MASTER_IF2;
			cfg.dst.gbl.masterIF_Cb_B			= D_IM_IIP_MASTER_IF2;
			cfg.dst.gbl.masterIF_Cr_R			= D_IM_IIP_MASTER_IF2;
			cfg.dst.gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF2;
		}
		else{
			cfg.dst.gbl.masterIF_Y_G			= D_IM_IIP_MASTER_IF1;
			cfg.dst.gbl.masterIF_Cb_B			= D_IM_IIP_MASTER_IF1;
			cfg.dst.gbl.masterIF_Cr_R			= D_IM_IIP_MASTER_IF1;
			cfg.dst.gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF1;
		}
		
		cfg.pix_depth							= D_IM_IIP_PDEPTH_8BITS;
		
		switch( resize_tbl->rotate ){
			case E_BF_H264_USER_ROTATE_0:
				cfg.rotation		= E_IM_IIP_UTIL_ROTATE_000;
				break;
			case E_BF_H264_USER_ROTATE_90:
				cfg.rotation		= E_IM_IIP_UTIL_ROTATE_090;
				break;
			case E_BF_H264_USER_ROTATE_180:
				cfg.rotation		= E_IM_IIP_UTIL_ROTATE_180;
				break;
			case E_BF_H264_USER_ROTATE_270:
				cfg.rotation		= E_IM_IIP_UTIL_ROTATE_270;
				break;
			default:
				BF_Debug_Print_Error(("--- h264_user_resize_rotate : rotate param error.\n"));
				cfg.rotation		= E_IM_IIP_UTIL_ROTATE_000;
				break;
		}
		
		switch( resize_tbl->resize_mode ){
			case E_BF_H264_USER_RESIZE_MODE_NEAREST:
				cfg.resize_mode	= E_IM_IIP_UTIL_RESIZE_MODE_NEAREST;
				break;
			case E_BF_H264_USER_RESIZE_MODE_BILINEAR:
				cfg.resize_mode	= E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;
				break;
			case E_BF_H264_USER_RESIZE_MODE_BICUBIC:
				cfg.resize_mode	= E_IM_IIP_UTIL_RESIZE_MODE_BICUBIC;
				break;
			default:
				BF_Debug_Print_Error(("--- h264_user_resize_mode : resize param error.\n"));
				cfg.resize_mode	= E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;
				break;
		}
		
		ret = IM_IIP_Utility_ResizeRotate( &cfg, NULL, master_IF, M_SDRAM_MAP_COMMON_ADR_LOG2PHY((UINT32)param_buffer_addr), wait_time );
		
		if (ret == D_IM_IIP_OK) {
			result = D_BF_H264_USER_OK;
		}
		else{
			BF_Debug_Print_Error(("[Movie Play]IM_IIP_Utility_ResizeRotate ERR : ret=0x%x\n", ret));
			result = D_BF_H264_USER_NG;
		}
		
		break;
	}
	
	// IIP Resource Release
	ret = BF_IIP_Res_Config_Free_BaseFW( &iip_res_config_out, 1, wait_time );
	if( ret != D_FJ_IIP_ERR_OK ){
		BF_Debug_Print_Error(("--- h264_user_resize_rotate : BF_IIP_Res_Config_Free_BaseFW Error=%d\n", ret));
		result = D_BF_H264_USER_NG;
	}
	
#endif	//CO_H264_PORTING_BLOCK
	
	return result;
}

// ***************************************************************************
//                    H.264 middleware "Host API" part
// ***************************************************************************

VOID BF_H264_User_Notify_Stop_Trigger( E_H264_IF_STOP_NOTIFY param )
{
	BF_Debug_Print_Information(("BF_H264_User_Notify_Stop_Trigger param= %d\n",param));
	
	if( ((gH264_User_Play_Exc != D_BF_H264_USER_PLAY_EXC_OFF) && (gH264_User_Record_Exc != D_BF_H264_USER_RECORD_EXC_OFF)) || 
		((gH264_User_Play_Exc == D_BF_H264_USER_PLAY_EXC_OFF) && (gH264_User_Record_Exc == D_BF_H264_USER_RECORD_EXC_OFF)) ){ 
		BF_Debug_Print_Error(("BF_H264_User_Notify_Stop_Trigger ExcMode Error(Record:%d Play:%d)\n", gH264_User_Record_Exc, gH264_User_Play_Exc));
		return ;
	}
	
	if( gH264_User_Record_Exc != D_BF_H264_USER_RECORD_EXC_OFF ){
		switch(param){
			case E_H264_IF_STOP_NOTIFY_NORMAL_END:
				break;
			case E_H264_IF_STOP_NOTIFY_VRAW_MULTI_ERR:
				gH264_User_Auto_End = D_BF_H264_USER_AUTO_END_FATAL_ERR;
				break;
			case E_H264_IF_STOP_NOTIFY_VSTRM_EMPTY_ERR:
				gH264_User_Auto_End = D_BF_H264_USER_AUTO_END_FATAL_ERR;
				break;
			case E_H264_IF_STOP_NOTIFY_FATAL_ERR:
				gH264_User_Auto_End = D_BF_H264_USER_AUTO_END_FATAL_ERR;
				break;
			default:
				BF_Debug_Print_Error(("BF_H264_User_Notify_Stop_Trigger Error param=%d\n", param ));
				gH264_User_Record_Exc = D_BF_H264_USER_RECORD_EXC_OFF;
				return;
		}
		gH264_User_Record_Exc = D_BF_H264_USER_RECORD_EXC_VIEW_STOP;
	}
	else{
		;  // DO NOTHING
	}
	
	return;
}

VOID BF_H264_User_Notify_Stop_Complete( VOID )
{
	BF_Debug_Print_Information(("BF_H264_User_Notify_Stop_Trigger Enter\n"));
	
	if( ((gH264_User_Play_Exc != D_BF_H264_USER_PLAY_EXC_OFF) && (gH264_User_Record_Exc != D_BF_H264_USER_RECORD_EXC_OFF)) || 
		((gH264_User_Play_Exc == D_BF_H264_USER_PLAY_EXC_OFF) && (gH264_User_Record_Exc == D_BF_H264_USER_RECORD_EXC_OFF)) ){ 
		BF_Debug_Print_Error(("BF_H264_User_Notify_Stop_Complete ExcMode Error(Record:%d Play:%d)\n", gH264_User_Record_Exc, gH264_User_Play_Exc));
		return ;
	}
	if( gH264_User_Record_Exc != D_BF_H264_USER_RECORD_EXC_OFF ){
		/* Encode Stop Request (Reload Timer Stop) */
		BF_Movie_Common_Sync_Stop_H264();
		BF_Movie_Common_Timer_Counter_Reset();
		gH264_User_Record_Exc = D_BF_H264_USER_RECORD_EXC_OFF;
		if( gH264_User_Auto_End != D_BF_H264_USER_AUTO_END_OFF ){
			gH264_User_Auto_End = D_BF_H264_USER_AUTO_END_OFF;
			FJ_HostPostMessage( FJ_HM_OPERATION_COMPLETE, 1 );
		}
		else{
			FJ_HostPostMessage( FJ_HM_OPERATION_COMPLETE, 0 );
		}
	}
	else if( gH264_User_Play_Exc != D_BF_H264_USER_PLAY_EXC_OFF ){
		OS_User_Set_Flg( FID_DECODER, D_BF_FLG_DECODER_STATE_CHANGE );
	}
	else{
		;  // DO NOTHING
	}
}

VOID BF_H264_User_Notify_State_Change_Complete( INT32 result )
{
	if( ((gH264_User_Play_Exc != D_BF_H264_USER_PLAY_EXC_OFF) && (gH264_User_Record_Exc != D_BF_H264_USER_RECORD_EXC_OFF)) || 
		((gH264_User_Play_Exc == D_BF_H264_USER_PLAY_EXC_OFF) && (gH264_User_Record_Exc == D_BF_H264_USER_RECORD_EXC_OFF)) ){ 
		BF_Debug_Print_Error(("BF_H264_User_Notify_State_Change_Complete ExcMode Error(Record:%d Play:%d)\n", gH264_User_Record_Exc, gH264_User_Play_Exc));
		return ; 
	}
	
	if( gH264_User_Record_Exc != D_BF_H264_USER_RECORD_EXC_OFF ){
		// Record
	}
	else if( gH264_User_Play_Exc != D_BF_H264_USER_PLAY_EXC_OFF ){ 
		// Play
		OS_User_Set_Flg( FID_DECODER, D_BF_FLG_DECODER_STATE_CHANGE );
	}
	else{
		; // DO NOTHING
	}
	
	return;
}


BOOL BF_H264_User_Notify_Video_Stream_Request( VOID )
{
	return FJ_Host_VideoPlayback_GetVideoStreamCB(E_FJ_VIDEO_DEC_H264);
}


BOOL BF_H264_User_Notify_FirstFrameDisplayCB( E_H264_IF_PLAY_DECODER_STATUS decoder_status, ULLONG display_pts, BOOL error )
{
	BOOL							ret;
	E_FJ_MOVIE_PLAY_DECODER_STATUS	notify_status;
	
	switch( decoder_status ){
		case E_H264_IF_PLAY_DECODER_STATUS_IDLE:
			notify_status = E_FJ_MOVIE_PLAY_DECODER_STATUS_IDLE;
			break;
		case E_H264_IF_PLAY_DECODER_STATUS_NORMAL:
			notify_status = E_FJ_MOVIE_PLAY_DECODER_STATUS_NORMAL;
			break;
		case E_H264_IF_PLAY_DECODER_STATUS_LAST:
			notify_status = E_FJ_MOVIE_PLAY_DECODER_STATUS_LAST;
			break;
		default:
			BF_Debug_Print_Error(("BF_H264_User_Notify_FirstFrameDisplayCB : Error param(status) =%d\n", decoder_status ));
			return FALSE;
	}
	
	ret = FJ_Host_VideoPlayback_FirstFrameDisplayCB( notify_status, display_pts, error );
	
	return ret;
}


BOOL BF_H264_User_Notify_VideoDecodeCB( E_H264_IF_PLAY_DECODER_STATUS decoder_status, ULLONG display_pts, BOOL error )
{
	BOOL							ret;
	E_FJ_MOVIE_PLAY_DECODER_STATUS	notify_status;
	
	switch( decoder_status ){
		case E_H264_IF_PLAY_DECODER_STATUS_IDLE:
			notify_status = E_FJ_MOVIE_PLAY_DECODER_STATUS_IDLE;
			break;
		case E_H264_IF_PLAY_DECODER_STATUS_NORMAL:
			notify_status = E_FJ_MOVIE_PLAY_DECODER_STATUS_NORMAL;
			break;
		case E_H264_IF_PLAY_DECODER_STATUS_LAST:
			notify_status = E_FJ_MOVIE_PLAY_DECODER_STATUS_LAST;
			break;
		default:
			BF_Debug_Print_Error(("BF_H264_User_Notify_FirstFrameDisplayCB : Error param(status) =%d\n", decoder_status ));
			return FALSE;
	}
	
	ret = FJ_Host_VideoPlayback_VideoDecodeCB( E_FJ_VIDEO_DEC_H264, notify_status, display_pts, error );
	
	return ret;
}


VOID BF_H264_User_Notify_Lcd_Update( UINT32* y_addr, UINT32* c_addr, BOOL update )
{
	*y_addr = (UINT32)Sdram_Map_Common_Change_Vde(update);
	*c_addr = *y_addr + (640 * 480);
	
	return;
}

VOID BF_H264_User_Notify_Hdmi_Update( UINT32* y_addr, UINT32* c_addr, BOOL update )
{
	*y_addr = (UINT32)Sdram_Map_Play_Change_HDMI_HD(update);
	*c_addr = (UINT32)Sdram_Map_Play_Get_HDMI_HD_Cb();
	
	return;
}

/**
 *	@brief		Repeat timming Send Message.
 *	@param		i:E_H264_IF_STREAM_CH
 *	@return		TRUE	(1)Normal End
 *	@return		FALSE	(0)Error
 *	@note		None
 *	@attention	None
*/
BOOL BF_H264_User_Notify_RepeatFrame( E_H264_IF_STREAM_CH stream_ch )
{
	E_VIDEO_ENOCDE_ID video_enc_id;
	
	BF_Movie_Video_Attr_Change_StreamCH_to_EncodeID_H264( stream_ch, &video_enc_id );
	
	return FJ_Host_EncoderRepeatFrameCB ( video_enc_id );
}

/**
 *	@brief		PreEncode timming Send Message.
 *	@param		i:E_H264_IF_STREAM_CH
 *	@return		TRUE	(1)Normal End
 *	@return		FALSE	(0)Error
 *	@note		None
 *	@attention	None
*/
BOOL BF_H264_User_Notify_PreEncode( E_H264_IF_STREAM_CH stream_ch )
{
	E_VIDEO_ENOCDE_ID video_enc_id;
	INT32 vop_cnt = 0;
	
	BF_Movie_Video_Attr_Change_StreamCH_to_EncodeID_H264( stream_ch, &video_enc_id );
	
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Pre:%d", video_enc_id ));
	
	return FJ_Host_EncoderSequenceStartCB ( video_enc_id, &vop_cnt );
}

/**
 *	@brief		Vector info Send Message.
 *	@param		i:T_H264_IF_VIDEO_VECTOR_PARAM
 *	@return		TRUE	(1)Normal End
 *	@return		FALSE	(0)Error
 *	@note		None
 *	@attention	None
*/
BOOL BF_H264_User_Notify_Vector_Info( T_H264_IF_VIDEO_VECTOR_PARAM* vector_info )
{
	T_FJ_MOVIE_H264_VECTOR_PARAM h264_vector_info;
	E_VIDEO_ENOCDE_ID video_enc_id;
	
	h264_vector_info.frame_no	= vector_info->frame_no;
	h264_vector_info.width		= vector_info->width;
	h264_vector_info.lines		= vector_info->lines;
	
	if( vector_info->gop_struct == E_H264_IF_GOP_STR_IIII ){
		h264_vector_info.gop_struct = FJ_MOVIE_VIDEO_GOP_STRUCT_IIII;
	}
	else if( vector_info->gop_struct == E_H264_IF_GOP_STR_IPPP ){
		h264_vector_info.gop_struct = FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP;
	}
	else{
		h264_vector_info.gop_struct = FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP;
	}
	h264_vector_info.frame_num_of_gop	= vector_info->gop_num;
	h264_vector_info.frame_type			= vector_info->sample_type;
	
	h264_vector_info.stream_pts = vector_info->sample_pts;
	h264_vector_info.stream_dts = vector_info->sample_dts;
	
	h264_vector_info.bitrate			= vector_info->bitrate;
	h264_vector_info.ave_bitrate		= vector_info->ave_bitrate;
	h264_vector_info.vector_addr		= (UINT32)vector_info->vector_addr;
	h264_vector_info.vector_size		= vector_info->vector_size;
	h264_vector_info.brightness_addr	= (UINT32)vector_info->brightness_addr;
	h264_vector_info.brightness_size	= vector_info->brightness_size;
	
	BF_Movie_Video_Attr_Change_StreamCH_to_EncodeID_H264( (E_H264_IF_STREAM_CH)vector_info->stream_ch, &video_enc_id );
	
	return FJ_Host_H264VectorCB( video_enc_id, &h264_vector_info );
}

/**
 *	@brief		VStream info Send Message.
 *	@param		i:T_H264_IF_VIDEO_STREAM_PARAM
 *	@return		TRUE	(1)Normal End
 *	@return		FALSE	(0)Error
 *	@note		None
 *	@attention	None
*/
BOOL BF_H264_User_Notify_Video_Stream( T_H264_IF_VIDEO_STREAM_PARAM* vstream_info )
{
	E_VIDEO_ENOCDE_ID video_enc_id;
	T_FJ_MOVIE_H264_STREAM_PARAM h264_stream_info;
	
	h264_stream_info.frame_no = vstream_info->frame_no;
	h264_stream_info.width = vstream_info->width;
	h264_stream_info.lines = vstream_info->lines;
	h264_stream_info.stream_serial_num = vstream_info->stream_serial_num;
	h264_stream_info.stream_end_flg = vstream_info->stream_end_flg;
	h264_stream_info.vstream_index = vstream_info->vstream_index;
	h264_stream_info.stream_address = (UINT32)vstream_info->sample_address;
	h264_stream_info.stream_size = vstream_info->sample_size;
	h264_stream_info.frame_type = vstream_info->sample_type;
	h264_stream_info.stream_poc = vstream_info->sample_poc;
	h264_stream_info.counter_in_gop = vstream_info->frame_of_GOP;
	h264_stream_info.VBV_buffer = vstream_info->VBV_buffer;
	h264_stream_info.nal_unit_num = vstream_info->nal_unit_num;
	memcpy( h264_stream_info.nal_unit_size, vstream_info->nal_unit_size, 8*sizeof(ULONG) );
	
	h264_stream_info.stream_pts = vstream_info->sample_pts;
	h264_stream_info.stream_dts = vstream_info->sample_dts;
	
	h264_stream_info.bitrate = vstream_info->bitrate;
	h264_stream_info.ave_bitrate = vstream_info->ave_bitrate;
	h264_stream_info.error_flag = vstream_info->error_flg;
	
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "%u-0x%lX ch", vstream_info->stream_ch, (ULONG)h264_stream_info.frame_no ));
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "0x%lX pts", (ULONG)h264_stream_info.stream_pts ));
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "0x%lX dts", (ULONG)h264_stream_info.stream_dts ));
	
	BF_Movie_Video_Attr_Change_StreamCH_to_EncodeID_H264( (E_H264_IF_STREAM_CH)vstream_info->stream_ch, &video_enc_id );
	
#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
	if( (h264_stream_info.stream_end_flg == 1) ||
		(h264_stream_info.error_flag == TRUE) ){
		BF_Movie_Common_Set_Dummy_Sync( video_enc_id, FALSE );
	}
#endif
	
	return FJ_Host_H264StreamCB( video_enc_id, &h264_stream_info );
}

