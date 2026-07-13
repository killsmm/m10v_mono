/**
 * @file		movie_video.c
 * @brief		SocioNext Video Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_LAST_CODE"

#include "fj_host.h"
#include "fj_filesystem.h"
// delete_basefw\custom
//#include "host_view.h"
#include "instance.h"
#include "still_mov_ctrl.h"

#include "movie_common.h"
#include "movie_video.h"
#include "h264_user.h"
#include "h265_user.h"
#include "play_attr.h"

#include "sdram_map_movie_play.h"

#include "audio.h"
#include "instance.h"

// ***************************************************************************
//                           Definition
// ***************************************************************************
#define D_BF_MOVIE_VIDEO_SET_STREAM_MAX_NUM			(240)	// prepared 240 side.
#define D_BF_MOVIE_VIDEO_SET_STREAM_MAX_NUM_SCREEN	(120)	// prepared 120 side.

// ***************************************************************************
//                           Global Data
// ***************************************************************************

static	volatile	T_BF_INSTANCE_VIDEO_ENCODE	gBF_Instance_Video_Encode[D_BF_INSTANCE_ID_MAX_VIDEO_ENCODE] = {
	{ .flag	= E_BF_INSTANCE_FLAG_UNUSE },
	{ .flag	= E_BF_INSTANCE_FLAG_UNUSE },
	{ .flag	= E_BF_INSTANCE_FLAG_UNUSE },
	{ .flag	= E_BF_INSTANCE_FLAG_UNUSE },
	{ .flag	= E_BF_INSTANCE_FLAG_UNUSE }	// flag init num = D_BF_INSTANCE_ID_MAX_VIDEO_ENCODE(5)
};

static	volatile	T_BF_INSTANCE_VIDEO_DECODE	gBF_Instance_Video_Decode[D_BF_INSTANCE_ID_MAX_VIDEO_DECODE] = {
	{ .flag	= E_BF_INSTANCE_FLAG_UNUSE },
	{ .flag	= E_BF_INSTANCE_FLAG_UNUSE }	// flag init num = D_BF_INSTANCE_ID_MAX_VIDEO_DECODE(2)
};

static T_BF_INSTANCE_DISPLAY g_sub_disp_inst;

//#pragma arm section code="BASEFW_VIEW_CODE"
/**
 * @brief		Opne video encoder instance.
 * @param		E_FJ_VIDEO_ENC_ENUM video_enc
 * @param		UCHAR* video_enc_id
 * @return		FJ_ERR_OK:ok\n
 *              FJ_ERR_NG:error
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_videoencoder_open( E_FJ_VIDEO_ENC_ENUM video_enc, UCHAR* video_enc_id )
{
	switch( video_enc ){
		case E_FJ_VIDEO_ENC_ENUM_H264_0 :
			if( gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH0].flag != E_BF_INSTANCE_FLAG_USE ){
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH0].flag = E_BF_INSTANCE_FLAG_USE;
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH0].type = E_FJ_VIDEO_ENC_ENUM_H264_0;
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH0].state = E_BF_INSTANCE_STATE_NORMAL;
				*video_enc_id = (UCHAR)E_VIDEO_ENOCDE_ID_H264_CH0;
			}
			else{
				BF_Debug_Print_Error(("This ID is already use\n"));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_VIDEO_ENC_ENUM_H264_1 :
			if( gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH1].flag != E_BF_INSTANCE_FLAG_USE ){
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH1].flag = E_BF_INSTANCE_FLAG_USE;
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH1].type = E_FJ_VIDEO_ENC_ENUM_H264_1;
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH1].state = E_BF_INSTANCE_STATE_NORMAL;
				*video_enc_id = (UCHAR)E_VIDEO_ENOCDE_ID_H264_CH1;
			}
			else{
				BF_Debug_Print_Error(("This ID is already use\n"));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_VIDEO_ENC_ENUM_H264_2 :
			if( gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH2].flag != E_BF_INSTANCE_FLAG_USE ){
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH2].flag = E_BF_INSTANCE_FLAG_USE;
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH2].type = E_FJ_VIDEO_ENC_ENUM_H264_2;
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH2].state = E_BF_INSTANCE_STATE_NORMAL;
				*video_enc_id = (UCHAR)E_VIDEO_ENOCDE_ID_H264_CH2;
			}
			else{
				BF_Debug_Print_Error(("This ID is already use\n"));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_VIDEO_ENC_ENUM_H265_0 :
			if( gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H265_CH0].flag != E_BF_INSTANCE_FLAG_USE ){
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H265_CH0].flag = E_BF_INSTANCE_FLAG_USE;
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H265_CH0].type = E_FJ_VIDEO_ENC_ENUM_H265_0;
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H265_CH0].state = E_BF_INSTANCE_STATE_NORMAL;
				*video_enc_id = (UCHAR)E_VIDEO_ENOCDE_ID_H265_CH0;
			}
			else{
				BF_Debug_Print_Error(("This ID is already use\n"));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_VIDEO_ENC_ENUM_H265_1 :
			if( gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H265_CH1].flag != E_BF_INSTANCE_FLAG_USE ){
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H265_CH1].flag = E_BF_INSTANCE_FLAG_USE;
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H265_CH1].type = E_FJ_VIDEO_ENC_ENUM_H265_1;
				gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H265_CH1].state = E_BF_INSTANCE_STATE_NORMAL;
				*video_enc_id = (UCHAR)E_VIDEO_ENOCDE_ID_H265_CH1;
			}
			else{
				BF_Debug_Print_Error(("This ID is already use\n"));
				return FJ_ERR_NG;
			}
			break;
		default :
			BF_Debug_Print_Error(("not exist this ID\n"));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

//#pragma arm section code="BASEFW_VIEW_CODE"
/**
 * @brief		Close video encoder instance.
 * @param		E_FJ_VIDEO_ENC_ENUM video_enc
 * @param		UCHAR* video_enc_id
 * @return		FJ_ERR_OK:ok\n
 *              FJ_ERR_NG:error
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_videoencoder_close( UCHAR video_enc_id )
{
	switch( (E_VIDEO_ENOCDE_ID)video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
			gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH0].flag = E_BF_INSTANCE_FLAG_UNUSE;
			break;
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
			gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH1].flag = E_BF_INSTANCE_FLAG_UNUSE;
			break;
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
			gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H264_CH2].flag = E_BF_INSTANCE_FLAG_UNUSE;
			break;
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
			gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H265_CH0].flag = E_BF_INSTANCE_FLAG_UNUSE;
			break;
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			gBF_Instance_Video_Encode[E_VIDEO_ENOCDE_ID_H265_CH1].flag = E_BF_INSTANCE_FLAG_UNUSE;
			break;
		default :
			BF_Debug_Print_Error(("not exist this ID\n"));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

//#pragma arm section code="BASEFW_VIEW_CODE"
/**
 * @brief		The video encoder frame enable ( for video time lapse).
 * @param		UCHAR video_enc_id
 * @param		BOOL enabled
 * @return		FJ_ERR_OK:ok\n
 *              FJ_ERR_NG:error
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_videoencoder_setinstanceenabled( UCHAR video_enc_id, BOOL enabled )
{
// delete_basefw\custom
//	T_COMMON_HOST_VIEW_ENCODER_INFO latest_encoder_info;
//	
//	switch( video_enc_id ){
//		case E_VIDEO_ENOCDE_ID_H264_CH0 :
//		case E_VIDEO_ENOCDE_ID_H264_CH1 :
//		case E_VIDEO_ENOCDE_ID_H264_CH2 :
//			Common_HostView_GetLatestEncoderInfo( video_enc_id, &latest_encoder_info );
//			BF_H264_User_Record_Frame_Enabled( (E_VIDEO_ENOCDE_ID)video_enc_id, latest_encoder_info.frame_no, latest_encoder_info.encoder_addr_get_dummy_flg, enabled );
//			break;
//		case E_VIDEO_ENOCDE_ID_H265_CH0 :
//		case E_VIDEO_ENOCDE_ID_H265_CH1 :
//			Common_HostView_GetLatestEncoderInfo( video_enc_id, &latest_encoder_info );
//			BF_H265_User_Record_Frame_Enabled( (E_VIDEO_ENOCDE_ID)video_enc_id, latest_encoder_info.frame_no, latest_encoder_info.encoder_addr_get_dummy_flg, enabled );
//			break;
//		default :
//			BF_Debug_Print_Error(("fj_videoencoder_setinstanceenabled Encode ID err.\n"));
//			return FJ_ERR_NG;
//	}
	
	return FJ_ERR_OK;

}

////#pragma arm section code="BASEFW_VIEW_CODE"
/**
 * @brief		The video encoder config to be set.
 * @param		UCHAR video_enc_id
 * @param		INT32 wConfig
 * @param		INT32 wOperation
 * @return		FJ_ERR_OK:ok\n
 *              FJ_ERR_NG:error
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_videoencoder_config( UCHAR video_enc_id, UINT32 wConfig, UINT32 wOperation )
{
	INT32	ret;
	
	if( wConfig == FJ_MOVIE_H264_CFG ){
		ret = BF_Movie_Video_Attr_Set_H264_Config( (E_VIDEO_ENOCDE_ID)video_enc_id, (T_FJ_MOVIE_H264_CONFIG_PARAM*)wOperation );
		if( ret != FJ_ERR_OK ){
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H264_Config err. ret=%d\n", ret));
			return FJ_ERR_NG;
		}
	}
	else if( wConfig == FJ_MOVIE_H265_CFG ){
		ret = BF_Movie_Video_Attr_Set_H265_Config( (E_VIDEO_ENOCDE_ID)video_enc_id, (T_FJ_MOVIE_H265_CONFIG_PARAM*)wOperation );
		if( ret != FJ_ERR_OK ){
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H265_Config err. ret=%d\n", ret));
			return FJ_ERR_NG;
		}
	}
	else{
		BF_Debug_Print_Error(("not support codec\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

//#pragma arm section code="BASEFW_VIEW_CODE"
/**
 * @brief		The video encoder config to be set.
 * @param		INT32 uConfig
 * @param		INT32 wOperation
 * @return		FJ_ERR_OK:ok\n
 *              FJ_ERR_NG:error
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_videoencoder_configdynamic( UCHAR video_enc_id, UINT32 wConfig, UINT32 wOperation )
{
	INT32	ret;
	
	if( wConfig == FJ_MOVIE_H264_CFG ){
		ret = BF_H264_User_Record_Set_Dynamic_Parameter( (E_VIDEO_ENOCDE_ID)video_enc_id, (T_FJ_MOVIE_H264_DYNAMIC_CONFIG_PARAM*)wOperation );
		if( ret != D_BF_H264_USER_OK ){
			BF_Debug_Print_Error(("BF_H264_User_Record_Set_Dynamic_Parameter err. ret=%d\n", ret));
			return FJ_ERR_NG;
		}
	}
	else if( wConfig == FJ_MOVIE_H265_CFG ){
		ret = BF_H265_User_Record_Set_Dynamic_Parameter( (E_VIDEO_ENOCDE_ID)video_enc_id, (T_FJ_MOVIE_H265_DYNAMIC_CONFIG_PARAM*)wOperation );
		if( ret != D_BF_H265_USER_OK ){
			BF_Debug_Print_Error(("BF_H265_User_Record_Set_Dynamic_Parameter err. ret=%d\n", ret));
			return FJ_ERR_NG;
		}
	}
	else{
		BF_Debug_Print_Error(("not support codec\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_videoencoder_insert_idr( UCHAR video_enc_id, UINT32 wConfig )
{
	INT32	ret;
	
	if( wConfig == FJ_MOVIE_H264_CFG ){
		ret = BF_H264_User_Record_Set_Insert_IDR_Picture( (E_VIDEO_ENOCDE_ID)video_enc_id);
		if( ret != D_BF_H264_USER_OK ){
			BF_Debug_Print_Error(("BF_H264_User_Record_Set_Insert_IDR_Picture err. ret=%d\n", ret));
			return FJ_ERR_NG;
		}
	}
	else if( wConfig == FJ_MOVIE_H265_CFG ){
		ret = BF_H265_User_Record_Set_Insert_IDR_Picture( (E_VIDEO_ENOCDE_ID)video_enc_id );
		if( ret != D_BF_H265_USER_OK ){
			BF_Debug_Print_Error(("BF_H265_User_Record_Set_Insert_IDR_Picture err. ret=%d\n", ret));
			return FJ_ERR_NG;
		}
	}
	else{
		BF_Debug_Print_Error(("not support codec\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

//#pragma arm section code="BASEFW_VIEW_CODE"
/**
 * @brief		Movie Record Setup
 * @param		i:video_enc_id	Encode ID
 * @return		FJ_ERR_OK:Normal end
 * @return		FJ_ERR_NG:Parameter Error
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_videoencoder_setup( UCHAR video_enc_id )
{
	INT32	ret;
	
	// Reload Timer Setting.
	ret = BF_Movie_Common_Timer_Setting( (E_VIDEO_ENOCDE_ID)video_enc_id );
	if( ret != D_BF_MOVIE_COMMON_OK ){
		BF_Debug_Print_Error(("fj_movierecordsetup timer setting err.\n"));
		return FJ_ERR_NG;
	}
	
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
			ret = BF_H264_User_Record_Ready_Movie( (E_VIDEO_ENOCDE_ID)video_enc_id );
			if( ret != D_BF_H264_USER_OK ){
				BF_Debug_Print_Error(("BF_H264_User_Record_Ready_Movie err. ret=%d\n", ret));
				return FJ_ERR_NG;
			}
			break;
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			ret = BF_H265_User_Record_Ready_Movie( (E_VIDEO_ENOCDE_ID)video_enc_id );
			if( ret != D_BF_H265_USER_OK ){
				BF_Debug_Print_Error(("BF_H265_User_Record_Ready_Movie err. ret=%d\n", ret));
				return FJ_ERR_NG;
			}
			break;
		default :
			BF_Debug_Print_Error(("fj_movierecordsetup Encode ID err.\n"));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

//#pragma arm section code="BASEFW_VIEW_CODE"
/**
 * @brief		Movie Record Start
 * @param		i:video_enc_id	Encode ID
 * @return		FJ_ERR_OK:Normal end
 * @return		FJ_ERR_NG:Parameter Error
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_videoencoder_start( UCHAR video_enc_id )
{
	INT32	ret;
	
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
			ret = BF_H264_User_Record_Start_Video( (E_VIDEO_ENOCDE_ID)video_enc_id, 0 );
			if( ret != D_BF_H264_USER_OK ){
				BF_Debug_Print_Error(("BF_H264_User_Record_Start_Video err. ret=%d\n", ret));
				return FJ_ERR_NG;
			}
#else
			ret = BF_Encoder_Valid_Image_Address( (E_VIDEO_ENOCDE_ID)video_enc_id );
			if( ret != FJ_ERR_OK ){
				BF_Debug_Print_Error(("BF_Encoder_Valid_Image_Address err. ret=%d\n", ret));
				return FJ_ERR_NG;
			}
			BF_StillMovCtrl_Start( (E_VIDEO_ENOCDE_ID)video_enc_id, TRUE );
#endif
			break;
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
#ifdef CO_H265_ENC_DEBUG_VIEW_DUMMY
			ret = BF_H265_User_Record_Start_Video( (E_VIDEO_ENOCDE_ID)video_enc_id, 0 );
			if( ret != D_BF_H265_USER_OK ){
				BF_Debug_Print_Error(("BF_H265_User_Record_Start_Video err. ret=%d\n", ret));
				return FJ_ERR_NG;
			}
#else
			ret = BF_Encoder_Valid_Image_Address( (E_VIDEO_ENOCDE_ID)video_enc_id );
			if( ret != FJ_ERR_OK ){
				BF_Debug_Print_Error(("BF_Encoder_Valid_Image_Address err. ret=%d\n", ret));
				return FJ_ERR_NG;
			}
			BF_StillMovCtrl_Start( (E_VIDEO_ENOCDE_ID)video_enc_id, TRUE );
#endif
		break;
		default :
			BF_Debug_Print_Error(("fj_videoencoder_start Encode ID err.\n"));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

//#pragma arm section code="BASEFW_VIEW_CODE"
/**
 * @brief		Movie Record Stop
 * @param		i:video_enc_id	Encode ID
 * @return		FJ_ERR_OK:Normal end
 * @return		FJ_ERR_NG:Parameter Error
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_videoencoder_stop( UCHAR video_enc_id )
{
	INT32	ret;
	
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
			ret = BF_H264_User_Record_Stop_Video( (E_VIDEO_ENOCDE_ID)video_enc_id );
			if( ret != D_BF_H264_USER_OK ){
				BF_Debug_Print_Error(("BF_H264_User_Record_Stop_Video err. ret=%d\n", ret));
				return FJ_ERR_NG;
			}
			break;
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			ret = BF_H265_User_Record_Stop_Video( (E_VIDEO_ENOCDE_ID)video_enc_id );
			if( ret != D_BF_H265_USER_OK ){
				BF_Debug_Print_Error(("BF_H265_User_Record_Stop_Video err. ret=%d\n", ret));
				return FJ_ERR_NG;
			}
			break;
		default :
			BF_Debug_Print_Error(("fj_videoencoder_stop Encode ID err.\n"));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

//#pragma arm section code="BASEFW_COMM_CODE"
/**
 * @brief		Free H.264 video stream. 
 * @param		i:vstream_free_info		Free video stream information
 * @return		FJ_ERR_OK:Normal end
 * @return		FJ_ERR_NG:Error
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_videoencoder_freeindex( UCHAR video_enc_id, UINT32 free_vstream_index, BOOL last_free_flg )
{
	INT32 ret;
	
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
			ret = BF_H264_User_Record_Free_Video_Index( (E_VIDEO_ENOCDE_ID)video_enc_id, free_vstream_index, last_free_flg );
			if( ret != H264_OK ){
				BF_Debug_Print_Error(("BF_H264_User_Record_Free_Video_Index err. ret=%d\n", ret));
				return FJ_ERR_NG;
			}
			break;
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			ret = BF_H265_User_Record_Free_Video_Index( (E_VIDEO_ENOCDE_ID)video_enc_id, free_vstream_index, last_free_flg );
			if( ret != H264_OK ){
				BF_Debug_Print_Error(("BF_H265_User_Record_Free_Video_Index err. ret=%d\n", ret));
				return FJ_ERR_NG;
			}
			break;
		default :
			BF_Debug_Print_Error(("fj_moviefreevideoindex_h264 Encode ID err.\n"));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_videoencoder_compenable( UCHAR video_enc_id, BOOL comp_enable )
{
	return BF_Movie_Video_Attr_Set_Compression((E_VIDEO_ENOCDE_ID)video_enc_id, comp_enable);
}

/***************/
/* for Decoder */
/***************/
FJ_ERR_CODE fj_videodecoder_open( E_FJ_VIDEO_DEC_ENUM video_dec,  UCHAR* video_decoder_id )
{
	switch( video_dec ){
		case E_FJ_VIDEO_DEC_H264 :
		case E_FJ_VIDEO_DEC_H265 :
			*video_decoder_id = (UCHAR)video_dec;
			if( gBF_Instance_Video_Decode[*video_decoder_id].flag == E_BF_INSTANCE_FLAG_UNUSE ){
				
				gBF_Instance_Video_Decode[*video_decoder_id].flag	= E_BF_INSTANCE_FLAG_USE;
				gBF_Instance_Video_Decode[*video_decoder_id].type	= video_dec;
				gBF_Instance_Video_Decode[*video_decoder_id].state	= E_BF_INSTANCE_STATE_NORMAL;
			}
			else{
				BF_Debug_Print_Error(("fj_videodecoder_open : This ID is already use\n"));
				return FJ_ERR_NG;
			}
			break;
		default :
			BF_Debug_Print_Error(("fj_videodecoder_open : not exist this ID\n"));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_videodecoder_close( UCHAR video_decoder_id )
{
	if( video_decoder_id >= D_BF_INSTANCE_ID_MAX_VIDEO_DECODE ){
		BF_Debug_Print_Error(("fj_videodecoder_close : not exist this ID\n"));
		return FJ_ERR_NG;
	}
	
	memset( (void*)&gBF_Instance_Video_Decode[video_decoder_id], 0, sizeof( gBF_Instance_Video_Decode[video_decoder_id] ) );
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_videodecoder_connect( UCHAR video_decoder_id, UCHAR display_id, BOOL sub_inst )
{
	T_BF_INSTANCE_DISPLAY*	instance_disp;
	FJ_ERR_CODE				err_code = FJ_ERR_OK;
	UINT32					output_loop;
	BOOL					excute_flag = FALSE;
	
	if ( ( video_decoder_id < D_BF_INSTANCE_ID_MAX_VIDEO_DECODE ) &&
		 ( display_id       < D_BF_INSTANCE_ID_MAX_DISPLAY ) ) {
		
		// Get display instance.
		instance_disp = BF_Instance_Get_Display( display_id );
		if (sub_inst) {
			memcpy(&g_sub_disp_inst, instance_disp, sizeof(T_BF_INSTANCE_DISPLAY));
			g_sub_disp_inst.window.input.width >>= 1;
			g_sub_disp_inst.window.input.lines >>= 1;
		}

#ifdef CO_DEBUG_ON_PC
		gBF_Instance_Video_Decode[video_decoder_id].flag	= E_BF_INSTANCE_FLAG_USE;
		instance_disp->flag									= E_BF_INSTANCE_FLAG_USE;
#endif
		if ( ( gBF_Instance_Video_Decode[video_decoder_id].flag	== E_BF_INSTANCE_FLAG_USE ) &&
			 ( instance_disp->flag								== E_BF_INSTANCE_FLAG_USE ) ) {
			
			for(output_loop=0; output_loop<D_BF_INSTANCE_OUTPUT_MAX_VIDEO_DEC; output_loop++){
				
				if ( gBF_Instance_Video_Decode[video_decoder_id].connect.display[output_loop].instance_p == NULL ) {
					gBF_Instance_Video_Decode[video_decoder_id].connect.display[output_loop].instance_id	= display_id;
					gBF_Instance_Video_Decode[video_decoder_id].connect.display[output_loop].instance_p		= sub_inst ? (UCHAR*)&g_sub_disp_inst : (UCHAR*)instance_disp;
					excute_flag = TRUE;
					break;
				}
			}
			
			if( excute_flag == FALSE ){
				err_code = FJ_ERR_NG;			// full.
			}
		}
		else {
			err_code = FJ_ERR_NG;				// not use.
		}
	}
	else {
		err_code = FJ_ERR_NG_INPUT_PARAM;		// param err.
	}
	
	return err_code;
}

FJ_ERR_CODE fj_videodecoder_disconnect( UCHAR video_decoder_id, UCHAR display_id, BOOL sub_inst )
{
	T_BF_INSTANCE_DISPLAY*	instance_disp;
	FJ_ERR_CODE				err_code = FJ_ERR_OK;
	UINT32					output_loop;
	BOOL					excute_flag = FALSE;
	
	if ( ( video_decoder_id < D_BF_INSTANCE_ID_MAX_VIDEO_DECODE ) &&
		 ( display_id       < D_BF_INSTANCE_ID_MAX_DISPLAY ) ) {
		
		// Get display instance.
		instance_disp = BF_Instance_Get_Display( display_id );
		if (sub_inst) {
			instance_disp = &g_sub_disp_inst;
		}
		
		if ( ( gBF_Instance_Video_Decode[video_decoder_id].flag	== E_BF_INSTANCE_FLAG_USE ) &&
			 ( instance_disp->flag								== E_BF_INSTANCE_FLAG_USE ) ) {
			
			for(output_loop=0; output_loop<D_BF_INSTANCE_OUTPUT_MAX_VIDEO_DEC; output_loop++){
				
				if ( gBF_Instance_Video_Decode[video_decoder_id].connect.display[output_loop].instance_p == (UCHAR*)instance_disp ) {
					
					gBF_Instance_Video_Decode[video_decoder_id].connect.display[output_loop].instance_p = NULL;
					excute_flag = TRUE;
				}
			}
			
			if( excute_flag == FALSE ){
				err_code = FJ_ERR_NG_CONDITION;	// connection unmatch.
			}
		}
		else {
			err_code = FJ_ERR_NG;				// not use.
		}
	}
	else {
		err_code = FJ_ERR_NG_INPUT_PARAM;		// param err.
	}
	
	return err_code;
}


FJ_ERR_CODE fj_videoplayback_videoinit( VOID )
{
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_videoplayback_setvideoconfig( T_FJ_MOVIE_PLAY_VIDEO_CONFIG_PARAM* video_config )
{
	FJ_ERR_CODE	fj_ret;
	INT32		user_ret;
	
	fj_ret = BF_Movie_Video_Attr_Set_Decoder_Codec_Type( video_config->codec_type );
	if( fj_ret != FJ_ERR_OK ){
		return FJ_ERR_NG;
	}
	
	if( video_config->codec_type == FJ_MOVIE_PLAY_CODEC_H264 ){
		
		fj_ret = BF_Movie_Video_Attr_Set_H264_Decoder_Config( video_config );
		if( fj_ret != FJ_ERR_OK ){
			return FJ_ERR_NG;
		}
		
		user_ret = BF_H264_User_Play_Init();
		if( user_ret != D_BF_H264_USER_OK ){
			BF_Debug_Print_Error(("fj_videoplayback_setvideoconfig : BF_H264_User_Play_Init error.\n"));
			return FJ_ERR_NG;
		}
	}
	else if( video_config->codec_type == FJ_MOVIE_PLAY_CODEC_H265 ){
		
		fj_ret = BF_Movie_Video_Attr_Set_H265_Decoder_Config( video_config );
		if( fj_ret != FJ_ERR_OK ){
			return FJ_ERR_NG;
		}
		
		user_ret = BF_H265_User_Play_Init();
		if( user_ret != D_BF_H264_USER_OK ){
			BF_Debug_Print_Error(("fj_videoplayback_setvideoconfig : BF_H265_User_Play_Init error.\n"));
			return FJ_ERR_NG;
		}
	}
	else{
		BF_Debug_Print_Error(("fj_videoplayback_setvideoconfig : codec_type error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_videoplayback_setstartpts( ULLONG start_pts )
{
	return BF_Movie_Video_Attr_Set_Decoder_Play_Start_Pts( start_pts );
}

FJ_ERR_CODE fj_videoplayback_setspeeddirection( E_FJ_MOVIE_PLAY_SPEED speed, E_FJ_MOVIE_PLAY_DIRECTION direction, BOOL pause )
{
	BF_Movie_Video_Attr_Set_Decoder_Play_Speed( speed );
	BF_Movie_Video_Attr_Set_Decoder_Play_Direction( direction );
	BF_Movie_Video_Attr_Set_Decoder_Play_Pause( pause );
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_videoplayback_setvideostream( E_FJ_VIDEO_DEC_ENUM codec, UINT32 store_num, T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM* video_stream_param )
{
	INT32	user_ret;
	UINT32	stream_loop;
	//UINT8	codec_type;
	
	T_H264_IF_PARAM_SET_STREAM	set_h264_stream_param[D_BF_MOVIE_VIDEO_SET_STREAM_MAX_NUM];
	T_H265_IF_PARAM_SET_STREAM	set_h265_stream_param[D_BF_MOVIE_VIDEO_SET_STREAM_MAX_NUM];
	
	if( store_num > D_BF_MOVIE_VIDEO_SET_STREAM_MAX_NUM ){
		BF_Debug_Print_Error(("fj_videoplayback_setvideostream : Setting number of streams exceeds the maximum value.\n"));
		return FJ_ERR_NG;
	}
	if( store_num == 0 ){
		BF_Debug_Print_Error(("fj_videoplayback_setvideostream : value of store_num is 0.\n"));
		return FJ_ERR_NG;
	}
	
	// Get Codec Type.
	//BF_Movie_Video_Attr_Get_Decoder_Codec_Type( &codec_type );
	
	if( codec == FJ_MOVIE_PLAY_CODEC_H264 ){
		
		for(stream_loop=0; stream_loop<store_num; stream_loop++){
			set_h264_stream_param[stream_loop].stream_end_flag	= video_stream_param[stream_loop].stream_end_flag;
			set_h264_stream_param[stream_loop].stss				= video_stream_param[stream_loop].stss;
			set_h264_stream_param[stream_loop].stream_size		= video_stream_param[stream_loop].stream_size;
			set_h264_stream_param[stream_loop].stream_address	= video_stream_param[stream_loop].stream_address;
			
			if( FALSE == Sdram_Map_Movie_Play_H264_Check_Stream_Area( video_stream_param[stream_loop].stream_size, video_stream_param[stream_loop].stream_address ) ){
				BF_Debug_Print_Error(("fj_videoplayback_setvideostream : Sdram_Map_Movie_Play_H264_Check_Stream_Area error.\n"));
				return FJ_ERR_NG;
			}
			
			set_h264_stream_param[stream_loop].pts				= video_stream_param[stream_loop].pts;
			set_h264_stream_param[stream_loop].dts				= video_stream_param[stream_loop].dts;
		}
		
		user_ret = BF_H264_User_Play_Set_Video_Stream( store_num, set_h264_stream_param );
		if( user_ret != D_BF_H264_USER_OK ){
//			BF_Debug_Print_Error(("fj_videoplayback_setvideostream : set video stream error.\n"));
			// In this case, stream management table is full.
			return FJ_ERR_OVERFLOW;
		}
	}
	else if( codec == FJ_MOVIE_PLAY_CODEC_H265 ){
		
		for(stream_loop=0; stream_loop<store_num; stream_loop++){
			set_h265_stream_param[stream_loop].stream_end_flag	= video_stream_param[stream_loop].stream_end_flag;
			set_h265_stream_param[stream_loop].stss				= video_stream_param[stream_loop].stss;
			set_h265_stream_param[stream_loop].stream_size		= video_stream_param[stream_loop].stream_size;
			set_h265_stream_param[stream_loop].stream_address	= video_stream_param[stream_loop].stream_address;
			
			if( FALSE == Sdram_Map_Movie_Play_H265_Check_Stream_Area( video_stream_param[stream_loop].stream_size, video_stream_param[stream_loop].stream_address ) ){
				BF_Debug_Print_Error(("fj_videoplayback_setvideostream : Sdram_Map_Movie_Play_H265_Check_Stream_Area error.\n"));
				return FJ_ERR_NG;
			}
			
			set_h265_stream_param[stream_loop].pts				= video_stream_param[stream_loop].pts;
			set_h265_stream_param[stream_loop].dts				= video_stream_param[stream_loop].dts;
		}
		
		user_ret = BF_H265_User_Play_Set_Video_Stream( store_num, set_h265_stream_param );
		if( user_ret != D_BF_H265_USER_OK ){
//			BF_Debug_Print_Error(("fj_videoplayback_setvideostream : set video stream error.\n"));
			// In this case, stream management table is full.
			return FJ_ERR_OVERFLOW;
		}
	}
	else{
		BF_Debug_Print_Error(("fj_videoplayback_setvideostream: codec type error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_videoplayback_startvideodecode( E_FJ_VIDEO_DEC_ENUM codec )
{
	INT32	user_ret;
//	UINT8	codec_type;
	
	// Get Codec Type.
	//BF_Movie_Video_Attr_Get_Decoder_Codec_Type( &codec_type );
	
	if( codec == FJ_MOVIE_PLAY_CODEC_H264 ){
		
		user_ret = BF_H264_User_Play_Start();
		if( user_ret != D_BF_H264_USER_OK ){
			BF_Debug_Print_Error(("fj_videoplayback_startvideodecode: BF_H264_User_Play_Start error.\n"));
			return FJ_ERR_NG;
		}
	}
	else if( codec == FJ_MOVIE_PLAY_CODEC_H265 ){
		user_ret = BF_H265_User_Play_Start();
		if( user_ret != D_BF_H265_USER_OK ){
			BF_Debug_Print_Error(("fj_videoplayback_startvideodecode: BF_H265_User_Play_Start error.\n"));
			return FJ_ERR_NG;
		}
	}
	else{
		BF_Debug_Print_Error(("fj_videoplayback_startvideodecode: codec type error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_videoplayback_stopvideodecode( E_FJ_VIDEO_DEC_ENUM codec, E_FJ_MOVIE_PLAY_LAST_PICTURE last_picture )
{
	INT32	ret;
	//UINT8	codec_type;
	
	// Get Codec Type.
	//BF_Movie_Video_Attr_Get_Decoder_Codec_Type( &codec_type );
	
	if( codec == FJ_MOVIE_PLAY_CODEC_H264 ){
		
		switch( last_picture ){
			case E_FJ_MOVIE_PLAY_LAST_PICTURE_KEEP :
				// case of trick play(FF,REW,etc..)
				ret = BF_H264_User_Play_Flush();
				if( ret != D_BF_H264_USER_OK ){
					BF_Debug_Print_Error(("fj_videoplayback_stopvideodecode : BF_H264_User_Play_Flush error.\n"));
					return FJ_ERR_NG;
				}
				
				break;
			case E_FJ_MOVIE_PLAY_LAST_PICTURE_DONOT_KEEP :
				// case of play stop. (last picture is don't keep)
				ret = BF_H264_User_Play_Stop( FALSE );
				if( ret != D_BF_H264_USER_OK ){
					if( ret == D_BF_H264_USER_NG_ALREADY_STOP_PLAY ){
						BF_Debug_Print_Warning(("fj_videoplayback_stopvideodecode : VideoPlayback is already stopped.\n"));
						return FJ_ERR_MOVIE_ALREADY_STOP_PLAY;
					}
					else{
						BF_Debug_Print_Error(("fj_videoplayback_stopvideodecode : BF_H264_User_Play_Stop error.\n"));
						return FJ_ERR_NG;
					}
				}
				break;
			case E_FJ_MOVIE_PLAY_LAST_PICTURE_KEEP_STOP :
				// case of play stop(file end) (last picture is keep).
				ret = BF_H264_User_Play_Stop( TRUE );
				if( ret != D_BF_H264_USER_OK ){
					if( ret == D_BF_H264_USER_NG_ALREADY_STOP_PLAY ){
						BF_Debug_Print_Warning(("fj_videoplayback_stopvideodecode : VideoPlayback is already stopped.\n"));
						return FJ_ERR_MOVIE_ALREADY_STOP_PLAY;
					}
					else{
						BF_Debug_Print_Error(("fj_videoplayback_stopvideodecode : BF_H264_User_Play_Stop error.\n"));
						return FJ_ERR_NG;
					}
				}
				break;
			default :
				BF_Debug_Print_Error(("fj_videoplayback_stopvideodecode : last_picture setting error (last_picture=%d).\n", last_picture));
				return FJ_ERR_NG;
		}
	}
	else if( codec == FJ_MOVIE_PLAY_CODEC_H265 ){
		
		switch( last_picture ){
			case E_FJ_MOVIE_PLAY_LAST_PICTURE_KEEP :
				// case of trick play(FF,REW,etc..)
				ret = BF_H265_User_Play_Flush();
				if( ret != D_BF_H265_USER_OK ){
					BF_Debug_Print_Error(("fj_videoplayback_stopvideodecode : BF_H265_User_Play_Flush error.\n"));
					return FJ_ERR_NG;
				}
				
				break;
			case E_FJ_MOVIE_PLAY_LAST_PICTURE_DONOT_KEEP :
				// case of play stop. (last picture is don't keep)
				ret = BF_H265_User_Play_Stop( FALSE );
				if( ret != D_BF_H265_USER_OK ){
					if( ret == D_BF_H265_USER_NG_ALREADY_STOP_PLAY ){
						BF_Debug_Print_Warning(("fj_videoplayback_stopvideodecode : VideoPlayback is already stopped.\n"));
						return FJ_ERR_MOVIE_ALREADY_STOP_PLAY;
					}
					else{
						BF_Debug_Print_Error(("fj_videoplayback_stopvideodecode : BF_H265_User_Play_Stop error.\n"));
						return FJ_ERR_NG;
					}
				}
				break;
			case E_FJ_MOVIE_PLAY_LAST_PICTURE_KEEP_STOP :
				// case of play stop(file end) (last picture is keep).
				ret = BF_H265_User_Play_Stop( TRUE );
				if( ret != D_BF_H265_USER_OK ){
					if( ret == D_BF_H265_USER_NG_ALREADY_STOP_PLAY ){
						BF_Debug_Print_Warning(("fj_videoplayback_stopvideodecode : VideoPlayback is already stopped.\n"));
						return FJ_ERR_MOVIE_ALREADY_STOP_PLAY;
					}
					else{
						BF_Debug_Print_Error(("fj_videoplayback_stopvideodecode : BF_H265_User_Play_Stop error.\n"));
						return FJ_ERR_NG;
					}
				}
				break;
			default :
				BF_Debug_Print_Error(("fj_videoplayback_stopvideodecode : last_picture setting error (last_picture=%d).\n", last_picture));
				return FJ_ERR_NG;
		}
	}
	else{
		BF_Debug_Print_Error(("fj_videoplayback_stopvideodecode: codec type error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_videoplayback_pausevideo( VOID )
{
	INT32	user_ret;
	UINT8	codec_type;
	
	// Get Codec Type.
	BF_Movie_Video_Attr_Get_Decoder_Codec_Type( &codec_type );
	
	if( codec_type == FJ_MOVIE_PLAY_CODEC_H264 ){
		
		user_ret = BF_H264_User_Play_Pause();
		if( user_ret != D_BF_H264_USER_OK ){
			return FJ_ERR_NG;
		}
	}
	else if( codec_type == FJ_MOVIE_PLAY_CODEC_H265 ){
		
		user_ret = BF_H265_User_Play_Pause();
		if( user_ret != D_BF_H265_USER_OK ){
			return FJ_ERR_NG;
		}
	}
	else{
		BF_Debug_Print_Error(("fj_videoplayback_pausevideo: codec type error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_videoplayback_resumevideo( VOID )
{
	INT32	user_ret;
	UINT8	codec_type;
	
	// Get Codec Type.
	BF_Movie_Video_Attr_Get_Decoder_Codec_Type( &codec_type );
	
	if( codec_type == FJ_MOVIE_PLAY_CODEC_H264 ){
		
		user_ret = BF_H264_User_Play_Resume();
		if( user_ret == D_BF_H264_USER_OK ){
			return FJ_ERR_NG;
		}
	}
	else if( codec_type == FJ_MOVIE_PLAY_CODEC_H265 ){
		
		user_ret = BF_H265_User_Play_Resume();
		if( user_ret == D_BF_H265_USER_OK ){
			return FJ_ERR_NG;
		}
	}
	else{
		BF_Debug_Print_Error(("fj_videoplayback_resumevideo: codec type error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_videoplayback_getdisplayframe( T_FJ_MOVIE_PLAY_VIDEO_YUV_INFO* display_yuv_info )
{
	INT32						user_ret;
	UINT8						codec_type;
	T_H264_IF_VIDEO_YUV_INFO	get_h264_display_yuv_info;
	T_H265_IF_VIDEO_YUV_INFO	get_h265_display_yuv_info;
	
	// Get Codec Type.
	BF_Movie_Video_Attr_Get_Decoder_Codec_Type( &codec_type );
	
	if( codec_type == FJ_MOVIE_PLAY_CODEC_H264 ){
		
		user_ret = BF_H264_User_Play_Get_Display_Frame( &get_h264_display_yuv_info );
		if( user_ret != D_BF_H264_USER_OK ){
			BF_Debug_Print_Error(("fj_videoplayback_getdisplayframe: get display frame error.\n"));
			return FJ_ERR_NG;
		}
		
		display_yuv_info->y_addr	= get_h264_display_yuv_info.y_addr;
		display_yuv_info->cbcr_addr	= get_h264_display_yuv_info.cbcr_addr;
		display_yuv_info->g_width	= get_h264_display_yuv_info.g_width;
		display_yuv_info->g_lines	= get_h264_display_yuv_info.g_lines;
		display_yuv_info->width		= get_h264_display_yuv_info.width;
		display_yuv_info->lines		= get_h264_display_yuv_info.lines;
		
		switch( get_h264_display_yuv_info.yuv_format ){
			case E_H264_IF_YUV_FORMAT_VIDEO :
				display_yuv_info->yuv_format	= E_FJ_MOVIE_PLAY_YUV_FORMAT_VIDEO;
				break;
				
			case E_H264_IF_YUV_FORMAT_YUV420 :
				display_yuv_info->yuv_format	= E_FJ_MOVIE_PLAY_YUV_FORMAT_YUV420;
				break;
				
			default:
				BF_Debug_Print_Error(("fj_videoplayback_getdisplayframe : yuv format error (%d).\n", get_h264_display_yuv_info.yuv_format));
				break;
		}
	}
	else if( codec_type == FJ_MOVIE_PLAY_CODEC_H265 ){
		
		user_ret = BF_H265_User_Play_Get_Display_Frame( &get_h265_display_yuv_info );
		if( user_ret != D_BF_H265_USER_OK ){
			BF_Debug_Print_Error(("fj_videoplayback_getdisplayframe: get display frame error.\n"));
			return FJ_ERR_NG;
		}
		
		display_yuv_info->y_addr	= get_h265_display_yuv_info.y_addr;
		display_yuv_info->cbcr_addr	= get_h265_display_yuv_info.cbcr_addr;
		display_yuv_info->g_width	= get_h265_display_yuv_info.g_width;
		display_yuv_info->g_lines	= get_h265_display_yuv_info.g_lines;
		display_yuv_info->width		= get_h265_display_yuv_info.width;
		display_yuv_info->lines		= get_h265_display_yuv_info.lines;
		
		switch( get_h265_display_yuv_info.yuv_format ){
			case E_H265_IF_YUV_FORMAT_VIDEO :
				display_yuv_info->yuv_format	= E_FJ_MOVIE_PLAY_YUV_FORMAT_VIDEO;	// ** For the name of the video image format, it needs to be considered. **
				break;
				
			case E_H265_IF_YUV_FORMAT_YUV420 :
				display_yuv_info->yuv_format	= E_FJ_MOVIE_PLAY_YUV_FORMAT_YUV420;
				break;
				
			default:
				BF_Debug_Print_Error(("fj_videoplayback_getdisplayframe : yuv format error (%d).\n", get_h265_display_yuv_info.yuv_format));
				break;
		}
	}
	else{
		BF_Debug_Print_Error(("fj_videoplayback_getdisplayframe: codec type error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_videoplayback_startiframedecoding( BOOL display, ULLONG req_pts, UINT32 store_num, T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM* video_stream_param, T_FJ_MOVIE_PLAY_VIDEO_YUV_INFO* decode_yuv_info)
{
	INT32						user_ret;
	UINT8						codec_type;
	UINT32						stream_loop;
	T_H264_IF_PARAM_SET_STREAM	set_h264_video_stream_param[D_BF_MOVIE_VIDEO_SET_STREAM_MAX_NUM_SCREEN];
	T_H264_IF_VIDEO_YUV_INFO	get_h264_decode_yuv_info;
	T_H265_IF_PARAM_SET_STREAM	set_h265_video_stream_param[D_BF_MOVIE_VIDEO_SET_STREAM_MAX_NUM_SCREEN];
	T_H265_IF_VIDEO_YUV_INFO	get_h265_decode_yuv_info;
	
	// Get Codec Type.
	BF_Movie_Video_Attr_Get_Decoder_Codec_Type( &codec_type );
	
	if( codec_type == FJ_MOVIE_PLAY_CODEC_H264 ){
		
		for(stream_loop=0; stream_loop<store_num; stream_loop++){
			set_h264_video_stream_param[stream_loop].stream_end_flag	= video_stream_param[stream_loop].stream_end_flag;
			set_h264_video_stream_param[stream_loop].stss				= video_stream_param[stream_loop].stss;
			set_h264_video_stream_param[stream_loop].stream_size		= video_stream_param[stream_loop].stream_size;
			set_h264_video_stream_param[stream_loop].stream_address		= video_stream_param[stream_loop].stream_address;
			
			if( FALSE == Sdram_Map_Movie_Play_H264_Check_Stream_Area( video_stream_param[stream_loop].stream_size, video_stream_param[stream_loop].stream_address ) ){
				BF_Debug_Print_Error(("fj_videoplayback_startiframedecoding : stream size/address error.\n"));
				return FJ_ERR_NG;
			}
			
			set_h264_video_stream_param[stream_loop].pts				= video_stream_param[stream_loop].pts;
			set_h264_video_stream_param[stream_loop].dts				= video_stream_param[stream_loop].dts;
		}		
		user_ret = BF_H264_User_Play_Top( display, req_pts, store_num, set_h264_video_stream_param, &get_h264_decode_yuv_info );
		if( user_ret != D_BF_H264_USER_OK ){
			BF_Debug_Print_Error(("fj_videoplayback_startiframedecoding: get top frame error.\n"));
			return FJ_ERR_NG;
		}
		
		decode_yuv_info->y_addr		= get_h264_decode_yuv_info.y_addr;
		decode_yuv_info->cbcr_addr	= get_h264_decode_yuv_info.cbcr_addr;
		decode_yuv_info->g_width	= get_h264_decode_yuv_info.g_width;
		decode_yuv_info->g_lines	= get_h264_decode_yuv_info.g_lines;
		decode_yuv_info->width		= get_h264_decode_yuv_info.width;
		decode_yuv_info->lines		= get_h264_decode_yuv_info.lines;
		
		switch( get_h264_decode_yuv_info.yuv_format ){
			case E_H264_IF_YUV_FORMAT_VIDEO :
				decode_yuv_info->yuv_format	= E_FJ_MOVIE_PLAY_YUV_FORMAT_VIDEO;
				break;
				
			case E_H264_IF_YUV_FORMAT_YUV420 :
				decode_yuv_info->yuv_format	= E_FJ_MOVIE_PLAY_YUV_FORMAT_YUV420;
				break;
				
			default:
				BF_Debug_Print_Error(("fj_videoplayback_startiframedecoding : yuv format error (%d).\n", get_h264_decode_yuv_info.yuv_format));
				break;
		}
	}
	else if( codec_type == FJ_MOVIE_PLAY_CODEC_H265 ){
		
		for(stream_loop=0; stream_loop<store_num; stream_loop++){
			set_h265_video_stream_param[stream_loop].stream_end_flag	= video_stream_param[stream_loop].stream_end_flag;
			set_h265_video_stream_param[stream_loop].stss				= video_stream_param[stream_loop].stss;
			set_h265_video_stream_param[stream_loop].stream_size		= video_stream_param[stream_loop].stream_size;
			set_h265_video_stream_param[stream_loop].stream_address		= video_stream_param[stream_loop].stream_address;
			
			if( FALSE == Sdram_Map_Movie_Play_H265_Check_Stream_Area( video_stream_param[stream_loop].stream_size, video_stream_param[stream_loop].stream_address ) ){
				BF_Debug_Print_Error(("fj_videoplayback_startiframedecoding : stream size/address error.\n"));
				return FJ_ERR_NG;
			}
			
			set_h265_video_stream_param[stream_loop].pts				= video_stream_param[stream_loop].pts;
			set_h265_video_stream_param[stream_loop].dts				= video_stream_param[stream_loop].dts;
		}		
		user_ret = BF_H265_User_Play_Top( display, req_pts, store_num, set_h265_video_stream_param, &get_h265_decode_yuv_info );
		if( user_ret != D_BF_H265_USER_OK ){
			BF_Debug_Print_Error(("fj_videoplayback_startiframedecoding: get top frame error.\n"));
			return FJ_ERR_NG;
		}
		
		decode_yuv_info->y_addr		= get_h265_decode_yuv_info.y_addr;
		decode_yuv_info->cbcr_addr	= get_h265_decode_yuv_info.cbcr_addr;
		decode_yuv_info->g_width	= get_h265_decode_yuv_info.g_width;
		decode_yuv_info->g_lines	= get_h265_decode_yuv_info.g_lines;
		decode_yuv_info->width		= get_h265_decode_yuv_info.width;
		decode_yuv_info->lines		= get_h265_decode_yuv_info.lines;
		
		switch( get_h265_decode_yuv_info.yuv_format ){
			case E_H265_IF_YUV_FORMAT_VIDEO :
				decode_yuv_info->yuv_format	= E_FJ_MOVIE_PLAY_YUV_FORMAT_VIDEO;	// ** For the name of the video image format, it needs to be considered. **
				break;
				
			case E_H265_IF_YUV_FORMAT_YUV420 :
				decode_yuv_info->yuv_format	= E_FJ_MOVIE_PLAY_YUV_FORMAT_YUV420;
				break;
				
			default:
				BF_Debug_Print_Error(("fj_videoplayback_startiframedecoding : yuv format error (%d).\n", get_h265_decode_yuv_info.yuv_format));
				break;
		}
	}
	else{
		BF_Debug_Print_Error(("fj_videoplayback_startiframedecoding: codec type error.\n"));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_videoplayback_memalloc( E_FJ_MOVIE_PLAY_BUFFER_TYPE buffer_type, UINT32* size, UINT32* address )
{
	INT32	ret;
	
	switch( buffer_type ){
		case E_FJ_MOVIE_PLAY_BUFFER_TYPE_H264_STREAM :
			ret = Sdram_Map_Movie_Play_H264_Get_Stream_Area( size, address );
			if( ret != 0 ){
				// There is no free space in the buffer.
				// It is subnormal route, so the error log is not output.
				return FJ_ERR_NG;
			}
			break;
			
		case E_FJ_MOVIE_PLAY_BUFFER_TYPE_H265_STREAM :
			ret = Sdram_Map_Movie_Play_H265_Get_Stream_Area( size, address );
			if( ret != 0 ){
				// There is no free space in the buffer.
				// It is subnormal route, so the error log is not output.
				return FJ_ERR_NG;
			}
			break;
			
		case E_FJ_MOVIE_PLAY_BUFFER_TYPE_AAC_STREAM :
		case E_FJ_MOVIE_PLAY_BUFFER_TYPE_PCM_STREAM :
			return fj_audio_decoder_mem_allocate(FJ_AUDIO_DEC_ID_0, size, address);
			
		default :
			BF_Debug_Print_Error(("fj_videoplayback_memalloc : buffer type error (buffer_type=%d).\n", buffer_type));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

VOID FJ_Record_Timer_Handler( VOID )
{
	BF_Movie_Common_Reload_Enc_Handler();
}

VOID FJ_Record_Timer_Enable( BOOL enable )
{
	BF_Movie_Common_RecTimer_Enable(enable);
}

/********************************************************************************/
/*		Base F/W Local API														*/
/********************************************************************************/

FJ_ERR_CODE BF_Encoder_Start( E_VIDEO_ENOCDE_ID video_enc_id, BOOL sync )
{
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
			BF_H264_User_Record_Start_Video( video_enc_id, sync );
			break;
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			BF_H265_User_Record_Start_Video( video_enc_id, sync );
			break;
		default :
			BF_Debug_Print_Error(("BF_Encoder_Start Encode ID err.\n"));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}



//#pragma arm section code="BASEFW_COMM_CODE"
FJ_ERR_CODE BF_Encoder_Valid_Image_Address( E_VIDEO_ENOCDE_ID video_enc_id )
{
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
			BF_H264_User_Record_Valid_Image_Address( video_enc_id );
			break;
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			BF_H265_User_Record_Valid_Image_Address( video_enc_id );
			break;
		default :
			BF_Debug_Print_Error(("BF_Encoder_Valid_Image_Address Encode ID err.\n"));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

//#pragma arm section code="BASEFW_COMM_CODE"
/**
 * @brief		Get H.264/H.265 Image Address. 
 * @param		steam_no	stream number
 * @param		size		stream size
 * @param		frame_no	frame number
 * @param		addr_y		output y address
 * @param		addr_c		output c address
 * @param		dummy_flg	dummy banck use flag
 * @return		FJ_ERR_OK:Normal end
 * @return		FJ_ERR_NG:Error
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Encoder_Get_Image_Address( E_VIDEO_ENOCDE_ID video_enc_id, E_FJ_MOVIE_VIDEO_SIZE size, ULLONG frame_no, T_BF_ENCODER_GET_IMAGE_ADDR_PARAM* addr_param )
{
	INT32 ret;
	
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
			ret = BF_H264_User_Record_Get_Image_Address( video_enc_id, size, frame_no, addr_param );
			if( ret != D_BF_H264_USER_OK ){
				BF_Debug_Print_Error(("BF_Encoder_Get_Image_Address err. ret=%d\n", ret));
				return FJ_ERR_NG;
			}
			break;
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			ret = BF_H265_User_Record_Get_Image_Address( video_enc_id, size, frame_no, addr_param );
			if( ret != D_BF_H264_USER_OK ){
//				BF_Debug_Print_Error(("BF_Encoder_Get_Image_Address err. ret=%d\n", ret));
				return FJ_ERR_NG;
			}			break;
		default :
			BF_Debug_Print_Error(("BF_Encoder_Get_Image_Address Encode ID err.\n"));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

//#pragma arm section code="BASEFW_COMM_CODE"
/**
 * @brief		Decide H.264/H.265 Image Address. 
 * @param		steam_no	stream number
 * @param		frame_no	frame number
 * @param		dummy_flg	dummy banck use flag
 * @return		FJ_ERR_OK:Normal end
 * @return		FJ_ERR_NG:Error
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Encoder_Decide_Image_Address( E_VIDEO_ENOCDE_ID video_enc_id, ULLONG frame_no, BOOL dummy_flg )
{
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
			BF_H264_User_Record_Decide_Image_Address( video_enc_id, frame_no, dummy_flg );
			break;
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			BF_H265_User_Record_Decide_Image_Address( video_enc_id, frame_no, dummy_flg );
			break;
		default :
			BF_Debug_Print_Error(("BF_Encoder_Decide_Image_Address Encode ID err.\n"));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}


/**
 * Get video encode instance.
 */
T_BF_INSTANCE_VIDEO_ENCODE* BF_Instance_Get_VideoEncode( UCHAR video_encode_id )
{
	return (T_BF_INSTANCE_VIDEO_ENCODE*)&gBF_Instance_Video_Encode[video_encode_id];
}

/**
 * Get video decode instance.
 */
T_BF_INSTANCE_VIDEO_DECODE* BF_Instance_Get_VideoDecode( UCHAR video_decoder_id )
{
	return (T_BF_INSTANCE_VIDEO_DECODE*)&gBF_Instance_Video_Decode[video_decoder_id];
}


VOID BF_Instance_Print_VideoDecode( VOID )
{
	T_BF_INSTANCE_VIDEO_DECODE*		decoder_instance;
	UCHAR							decoder_loop;
	UCHAR							display_loop;
	
	BF_Debug_Print_Information(( "<< VideoDecoder Instance Information >>\n" ));
	
	for( decoder_loop = 0; decoder_loop < D_BF_INSTANCE_ID_MAX_VIDEO_DECODE; decoder_loop++ ){
		
		decoder_instance = BF_Instance_Get_VideoDecode( decoder_loop );
		
		BF_Debug_Print_Information(( "\n" ));
		BF_Debug_Print_Information(( " [VideoDecoder id : %d]\n", decoder_loop ));
		
		if( decoder_instance->flag == E_BF_INSTANCE_FLAG_UNUSE ){
			BF_Debug_Print_Information(( "\n" ));
			BF_Debug_Print_Information(( "Non Active.\n" ));
			BF_Debug_Print_Information(( "\n" ));
			continue;
		}
		
		BF_Debug_Print_Information(( "  |  VideoDecoder type                    : %4d\n", decoder_instance->type ));
		BF_Debug_Print_Information(( "  |  VideoDecoder state                   : %4d\n", decoder_instance->type ));
		BF_Debug_Print_Information(( "  |\n"));
		
		for( display_loop = 0; display_loop < D_BF_INSTANCE_OUTPUT_MAX_VIDEO_DEC; display_loop++ ) {
			BF_Debug_Print_Information(( "  +--[VideoDecoderOut id : %d]", display_loop ));
			if( decoder_instance->connect.display[display_loop].instance_p == NULL ){
				BF_Debug_Print_Information(( "--[Non Connect.]\n" ));
				BF_Debug_Print_Information(( "  |\n"));
			}
			else{
				BF_Debug_Print_Information(( "--[Display id  : %d]\n", decoder_instance->connect.display[display_loop].instance_id ));
				BF_Debug_Print_Information(( "  |      Disp type                  : %4d\n", ((T_BF_INSTANCE_DISPLAY *)(decoder_instance->connect.display[display_loop].instance_p))->type ));
				BF_Debug_Print_Information(( "  |      Disp state                 : %4d\n", ((T_BF_INSTANCE_DISPLAY *)(decoder_instance->connect.display[display_loop].instance_p))->state ));
				BF_Debug_Print_Information(( "  |      Disp rotate_degree         : %4d\n", ((T_BF_INSTANCE_DISPLAY *)(decoder_instance->connect.display[display_loop].instance_p))->rotate_degree ));
				BF_Debug_Print_Information(( "  |      DispIn x_pos               : %4d\n", ((T_BF_INSTANCE_DISPLAY *)(decoder_instance->connect.display[display_loop].instance_p))->window.in_x_pos ));
				BF_Debug_Print_Information(( "  |      DispIn x_pos               : %4d\n", ((T_BF_INSTANCE_DISPLAY *)(decoder_instance->connect.display[display_loop].instance_p))->window.in_y_pos ));
				BF_Debug_Print_Information(( "  |      DispIn width               : %4d\n", ((T_BF_INSTANCE_DISPLAY *)(decoder_instance->connect.display[display_loop].instance_p))->window.input.width ));
				BF_Debug_Print_Information(( "  |      DispIn lines               : %4d\n", ((T_BF_INSTANCE_DISPLAY *)(decoder_instance->connect.display[display_loop].instance_p))->window.input.lines ));
				BF_Debug_Print_Information(( "  |      DispOut x_pos              : %4d\n", ((T_BF_INSTANCE_DISPLAY *)(decoder_instance->connect.display[display_loop].instance_p))->window.out_x_pos ));
				BF_Debug_Print_Information(( "  |      DispOut y_pos              : %4d\n", ((T_BF_INSTANCE_DISPLAY *)(decoder_instance->connect.display[display_loop].instance_p))->window.out_y_pos ));
				BF_Debug_Print_Information(( "  |      DispOut width              : %4d\n", ((T_BF_INSTANCE_DISPLAY *)(decoder_instance->connect.display[display_loop].instance_p))->window.output.width ));
				BF_Debug_Print_Information(( "  |      DispOut lines              : %4d\n", ((T_BF_INSTANCE_DISPLAY *)(decoder_instance->connect.display[display_loop].instance_p))->window.output.lines ));
				BF_Debug_Print_Information(( "  |\n" ));
			}
		}
	}

}


/********************************************************************************/
/*		(for Movie Package)														*/
/********************************************************************************/

