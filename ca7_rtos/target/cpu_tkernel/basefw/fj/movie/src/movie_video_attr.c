/**
 * @file		movie_video_attr.c
 * @brief		Movie Attribute Control.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_LAST_CODE"

#include <string.h>			// memcpy, memset...
#include "debug.h"
#include "movie_video_attr.h"
#include "play_attr.h"
#include "h264_user.h"

/******************************************************************************
 Enumeration
******************************************************************************/
/******************************************************************************
 Structure
******************************************************************************/
/******************************************************************************
 Global Data
******************************************************************************/
static	volatile	T_H264_IF_PARAM_RECORD		gBF_Encoder_H264_If_Param[E_H264_IF_STREAM_CH_MAX];
static	volatile	INT32						gBF_Encoder_H264_Framerate_Value[E_H264_IF_STREAM_CH_MAX];

static	volatile	T_H265_IF_PARAM_RECORD		gBF_Encoder_H265_If_Param[E_H265_IF_STREAM_CH_MAX];
static	volatile	INT32						gBF_Encoder_H265_Framerate_Value[E_H265_IF_STREAM_CH_MAX];

static	volatile	T_H264_IF_DECODER_PARAM		gBF_Decoder_H264_If_Param;
static	volatile	T_H265_IF_DECODER_PARAM		gBF_Decoder_H265_If_Param;
static	volatile	UINT8						gBF_Decoder_Valid_Codec_Type;
static	volatile	ULLONG						gBF_Decoder_Play_Start_Pts	= 0;
static	volatile	E_FJ_MOVIE_PLAY_SPEED		gBF_Decoder_Play_Speed		= E_FJ_MOVIE_PLAY_SPEED_1X;
static	volatile	E_FJ_MOVIE_PLAY_DIRECTION	gBF_Decoder_Play_Direction	= E_FJ_MOVIE_PLAY_DIRECTION_FWD;
static	volatile	BOOL						gBF_Decoder_Play_Pause		= FALSE;

/******************************************************************************
 Function
******************************************************************************/
/*--------------*/
/* Movie Video  */
/*--------------*/
FJ_ERR_CODE BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( E_VIDEO_ENOCDE_ID video_enc_id, E_H264_IF_STREAM_CH* stream_ch )
{
	// CH setting
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
			*stream_ch = E_H264_IF_STREAM_CH_0;
			break;
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
			*stream_ch = E_H264_IF_STREAM_CH_1;
			break;
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
			*stream_ch = E_H264_IF_STREAM_CH_2;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Encoder Instance ID=%d\n", video_enc_id ));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Change_StreamCH_to_EncodeID_H264( E_H264_IF_STREAM_CH stream_ch, E_VIDEO_ENOCDE_ID* video_enc_id )
{
	// CH setting
	switch( stream_ch ){
		case E_H264_IF_STREAM_CH_0 :
			*video_enc_id = E_VIDEO_ENOCDE_ID_H264_CH0;
			break;
		case E_H264_IF_STREAM_CH_1 :
			*video_enc_id = E_VIDEO_ENOCDE_ID_H264_CH1;
			break;
		case E_H264_IF_STREAM_CH_2 :
			*video_enc_id = E_VIDEO_ENOCDE_ID_H264_CH2;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Encoder Stream CH=%d\n", stream_ch ));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H265( E_VIDEO_ENOCDE_ID video_enc_id, E_H265_IF_STREAM_CH* stream_ch )
{
	// CH setting
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
			*stream_ch = E_H265_IF_STREAM_CH_0;
			break;
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			*stream_ch = E_H265_IF_STREAM_CH_1;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Encoder Instance ID=%d\n", video_enc_id ));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Change_StreamCH_to_EncodeID_H265( E_H265_IF_STREAM_CH stream_ch, E_VIDEO_ENOCDE_ID* video_enc_id )
{
	// CH setting
	switch( stream_ch ){
		case E_H265_IF_STREAM_CH_0 :
			*video_enc_id = E_VIDEO_ENOCDE_ID_H265_CH0;
			break;
		case E_H265_IF_STREAM_CH_1 :
			*video_enc_id = E_VIDEO_ENOCDE_ID_H265_CH1;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Encoder Stream CH=%d\n", stream_ch ));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Set_H264_Config( E_VIDEO_ENOCDE_ID video_enc_id, T_FJ_MOVIE_H264_CONFIG_PARAM* cfg_param )
{
	FJ_ERR_CODE ercd;
	E_H264_IF_STREAM_CH stream_ch;
	
	ercd = BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, &stream_ch );
	if( ercd != FJ_ERR_OK ){
		return FJ_ERR_NG;
	}
	
	memset( (void *)&gBF_Encoder_H264_If_Param[stream_ch], 0, sizeof(T_H264_IF_PARAM_RECORD) );
	
	gBF_Encoder_H264_If_Param[stream_ch].stream_ch = stream_ch;
	
	// Enable flag check
	switch( cfg_param->enable_flag ){
		case FJ_MOVIE_ON :
			gBF_Encoder_H264_If_Param[stream_ch].enable_flg = H264_ON;
			break;
		case FJ_MOVIE_OFF :
			gBF_Encoder_H264_If_Param[stream_ch].enable_flg = H264_OFF;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Enable Flag=%d\n", cfg_param->enable_flag ));
			return FJ_ERR_NG;
	}
	
	// Image Size
	switch( cfg_param->image_size ){
		case E_FJ_MOVIE_VIDEO_SIZE_4096_2304 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_4096_2304;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_4096_2304;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 4096;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 2304;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 2400 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_4096_2160 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_4096_2160;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_4096_2160;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 4096;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 2160;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_4096_2048 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_4096_2048;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_4096_2048;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 4096;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 2048;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_3840_2160 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_3840_2160;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_3840_2160;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 3840;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 2160;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_3840_1920 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_3840_1920;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_3840_1920;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 3840;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 1920;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_3000_3000 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_3000_3000;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_3000_3000;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 3008;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 3008;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 8;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;//8;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 8;//0;			
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2704_2028 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_2704_2028;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_2704_2028;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 2704;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 2028;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 4;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2704_1520 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_2704_1520;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_2704_1520;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 2704;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 1520;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2560_1440 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_2560_1440;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_2560_1440;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 2560;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 1440;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1440 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_1920_1440;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_1920_1440;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 1920;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 1440;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1080 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_1920_1080;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_1920_1080;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 1920;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 1088;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 8;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 12000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_960 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_1920_960;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_1920_960;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 1920;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 960;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 12000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1440_1080 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_1440_1080;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_1440_1080;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 1440;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 1088;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 8;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 12000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1440_720 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_1440_720;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_1440_720;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 1440;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 720;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 12000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_1024 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_1280_1024;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_1280_1024;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 1280;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 1024;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 12000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_960 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_1280_960;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_1280_960;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 1280;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 960;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			if( cfg_param->frame_rate > 12000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_768 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_1280_768;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_1280_768;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 1280;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 768;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_720 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_1280_720;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_1280_720;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 1280;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 720;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1024_768 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_1024_768;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_1024_768;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 1024;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 768;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_864_480 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_864_480;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_864_480;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 864;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 480;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_960_540 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_960_540;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_960_540;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 960;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 544;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 4;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset 	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_960_480 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_960_480;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_960_480;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 960;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 480;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_848_480 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_848_480;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_848_480;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 848;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 480;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_800_600 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_800_600;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_800_600;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 800;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 608;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 8;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_720_576 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_720_576;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_720_576;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 720;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 576;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_720_480 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_720_480;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_720_480;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 720;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 480;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_480 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_640_480;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_640_480;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 640;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 480;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_360 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_640_360;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_640_360;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 640;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 368;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 8;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_480_270 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_480_270;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_480_270;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 480;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 272;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 2;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset 	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_432_240 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_432_240;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_432_240;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 432;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 240;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_320_240 :
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_320_240;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_320_240;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 320;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 240;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1600_896:
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_1600_896;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_1600_896;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 1600;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 896;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1024_576:
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_1024_576;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_1024_576;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 1024;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 576;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_800_448:
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_800_448;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_800_448;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 800;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 448;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_352_288:
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_352_288;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_352_288;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 352;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 288;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2592_1944:
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_2592_1944;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_2592_1944;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 2592;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 1944;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2304_1296:
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_2304_1296;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_2304_1296;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 2304;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 1296;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1280:
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_1920_1280;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_1920_1280;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 1920;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 1280;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_512:
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_640_512;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_640_512;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 640;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 512;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_384_288:
			gBF_Encoder_H264_If_Param[stream_ch].size						= E_H264_IF_IMAGE_SIZE_384_288;
			gBF_Encoder_H264_If_Param[stream_ch].display_size				= E_H264_IF_IMAGE_SIZE_384_288;
			gBF_Encoder_H264_If_Param[stream_ch].width						= 384;
			gBF_Encoder_H264_If_Param[stream_ch].lines						= 288;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_top_offset		= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H264_If_Param[stream_ch].frame_crop_left_offset		= 0;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Image Size=%d\n", cfg_param->image_size ));
			return FJ_ERR_NG;
	}
	
	// Stream Format
	switch( cfg_param->stream_format ){
		case FJ_MOVIE_STREAM_FORMAT_ES :
			gBF_Encoder_H264_If_Param[stream_ch].stream_format	= E_H264_IF_STREAM_FORMAT_ES;
			break;
		case FJ_MOVIE_STREAM_FORMAT_PES :
			gBF_Encoder_H264_If_Param[stream_ch].stream_format	= E_H264_IF_STREAM_FORMAT_PES;
			break;
		case FJ_MOVIE_STREAM_FORMAT_ES_HEADER :
			gBF_Encoder_H264_If_Param[stream_ch].stream_format	= E_H264_IF_STREAM_FORMAT_ES_HEADER;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Stream Format=%d\n", cfg_param->stream_format ));
			return FJ_ERR_NG;
	}
	
	// BitRate Algorithm
	switch( cfg_param->bitrate_algorithm ){
		case FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_CBR :
			gBF_Encoder_H264_If_Param[stream_ch].rate_mode = E_H264_IF_RATE_MODE_CBR;
			break;
		case FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_VBR :
			gBF_Encoder_H264_If_Param[stream_ch].rate_mode = E_H264_IF_RATE_MODE_VBR;
			break;
		case FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_FIXED_QP :
			gBF_Encoder_H264_If_Param[stream_ch].rate_mode = E_H264_IF_RATE_MODE_FIXED_QP;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. BitRate Algorithm=%d\n", cfg_param->bitrate_algorithm ));
			return FJ_ERR_NG;
	}
	
	// Bitrate
	if( (cfg_param->bitrate > 0) && (cfg_param->bitrate <= 100000000) ){
		gBF_Encoder_H264_If_Param[stream_ch].bitrate = cfg_param->bitrate;
	}
	else{
		BF_Debug_Print_Error(("Param Error. BitRate=%d\n", cfg_param->bitrate ));
		return FJ_ERR_NG;
	}
	
	// Average Bitrate
	if( (cfg_param->average_bitrate > 0) && (cfg_param->bitrate <= 100000000) ){
		gBF_Encoder_H264_If_Param[stream_ch].average_bitrate = cfg_param->average_bitrate;
	}
	else{
		BF_Debug_Print_Error(("Param Error. Average BitRate=%d\n", cfg_param->average_bitrate ));
		return FJ_ERR_NG;
	}
	
	// FrameRate
	switch( cfg_param->frame_rate ){
		case 23976:	// 239.76fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_239_76;
			break;
		case 11988:	// 119.88fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_119_88;
			break;
		case 5994:	// 59.94fps (NTSC)
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_59_94;
			break;
		case 4795:	// 47.95fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_47_95;
			break;
		case 2997:	// 29.97fps (NTSC)
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_29_97;
			break;
		case 2397:	// 23.97fps (NTSC)
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_23_97;
			break;
		case 1498:	// 14.98fps (NTSC)
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_14_98;
			break;
		case 999:	// 9.99fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_9_99;
			break;
		case 499:	// 4.99fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_4_99;
			break;
		case 299:	// 2.99fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_2_99;
			break;
		case 199:	// 1.99fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_1_99;
			break;
		case 99:	// 0.99fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_0_99;
			break;
		
		case 24000:	// 240fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_240_0;
			break;
		case 12000:	// 120fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_120_0;
			break;
		case 6000:	// 60fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_60_0;
			break;
		case 5000:	// 50fps (PAL)
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_50_0;
			break;
		case 4800:	// 48fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_48_0;
			break;
		case 3000:	// 30fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_30_0;
			break;
		case 2500:	// 25fps (PAL)
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_25_0;
			break;
		case 2400:	// 24fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_24_0;
			break;
		case 1500:	// 15fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_15_0;
			break;
		case 1250:	// 12.5fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_12_5;
			break;
		case 1000:	// 10fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_10_0;
			break;
		case 500:	// 5fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_5_0;
			break;
		case 300:	// 3fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_3_0;
			break;
		case 200:	// 2fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_2_0;
			break;
		case 100:	// 1fps
			gBF_Encoder_H264_If_Param[stream_ch].frame_rate = E_H264_IF_FRAME_RATE_1_0;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. FrameRate=%d\n", cfg_param->frame_rate ));
			return FJ_ERR_NG;
	}
	gBF_Encoder_H264_Framerate_Value[stream_ch] = cfg_param->frame_rate;
	
	// GOP struct
	switch( cfg_param->gop_struct ){
		case FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP :
			gBF_Encoder_H264_If_Param[stream_ch].gop_struct = E_H264_IF_GOP_STR_IPPP;
			break;
		case FJ_MOVIE_VIDEO_GOP_STRUCT_IBP :
			gBF_Encoder_H264_If_Param[stream_ch].gop_struct = E_H264_IF_GOP_STR_IBP;
			break;
		case FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP :
			gBF_Encoder_H264_If_Param[stream_ch].gop_struct = E_H264_IF_GOP_STR_IBBP;
			break;
		case FJ_MOVIE_VIDEO_GOP_STRUCT_IIII :
			gBF_Encoder_H264_If_Param[stream_ch].gop_struct = E_H264_IF_GOP_STR_IIII;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. GOP Struct=%d\n", cfg_param->gop_struct ));
			return FJ_ERR_NG;
	}
	
	// Frame Number of GOP
	BF_H264_User_Record_Set_Gop_Size(stream_ch, cfg_param->frame_num_of_gop);
	if(cfg_param->extend_gop_num == 0xFFFF){
		gBF_Encoder_H264_If_Param[stream_ch].gop_num = BF_H264_User_Record_Get_Gop_Size(stream_ch);
	}
	else{
		gBF_Encoder_H264_If_Param[stream_ch].gop_num = cfg_param->extend_gop_num;
	}
	
	if( cfg_param->gop_struct != FJ_MOVIE_VIDEO_GOP_STRUCT_IIII){
		#if 0
		UINT32 onesec_frame = (cfg_param->frame_rate + 50) / 100;
		
		if( (cfg_param->frame_num_of_gop == 0) ||
			(cfg_param->frame_num_of_gop > onesec_frame) ){
			BF_Debug_Print_Error(("Param Error. frame num of GOP=%d\n", cfg_param->frame_num_of_gop ));
			return FJ_ERR_NG;
		}
		if( (cfg_param->frame_rate != 2500) &&
			(cfg_param->frame_rate != 1250) ){
			if( (onesec_frame % cfg_param->frame_num_of_gop) != 0){
				BF_Debug_Print_Error(("Param Error. frame num of GOP=%d (not multiple framerate)\n", cfg_param->frame_num_of_gop ));
				return FJ_ERR_NG;
			}
			if( ((onesec_frame / cfg_param->frame_num_of_gop) != 1) &&
				((onesec_frame / cfg_param->frame_num_of_gop) != 2) &&
				((onesec_frame / cfg_param->frame_num_of_gop) != 3) &&
				((onesec_frame / cfg_param->frame_num_of_gop) != 4) ){
				BF_Debug_Print_Error(("Param Error. frame num of GOP=%d (not allowed range)\n", cfg_param->frame_num_of_gop ));
				return FJ_ERR_NG;
			}
		}
		else if( cfg_param->frame_rate == 2500 ){
			// Special 25fps
			if( (cfg_param->frame_num_of_gop != 25) &&
				(cfg_param->frame_num_of_gop != 13) &&
				(cfg_param->frame_num_of_gop != 12) ){
				BF_Debug_Print_Error(("Param Error. frame num of GOP=%d (not allowed range)\n", cfg_param->frame_num_of_gop ));
				return FJ_ERR_NG;
			}
		}
		else{
			// Special 12.5fps
			if( cfg_param->frame_num_of_gop != 12 ){
				BF_Debug_Print_Error(("Param Error. frame num of GOP=%d (not allowed range)\n", cfg_param->frame_num_of_gop ));
				return FJ_ERR_NG;
			}
		}
		#endif
	}
	else{
		switch( cfg_param->image_size ){
			case E_FJ_MOVIE_VIDEO_SIZE_4096_2304 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_4096_2160 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_4096_2048 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_3840_2160 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_3840_1920 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_3000_3000 :
			case E_FJ_MOVIE_VIDEO_SIZE_2704_2028 :
				if( cfg_param->frame_rate > 1500 ){
					BF_Debug_Print_Error(("Param Error. Over spec in All Iframe \n" ));
					return FJ_ERR_NG;
				}
				break;
			case E_FJ_MOVIE_VIDEO_SIZE_2704_1520 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_1920_1440 :
				if( cfg_param->frame_rate > 3000 ){
					BF_Debug_Print_Error(("Param Error. Over spec in All Iframe \n" ));
					return FJ_ERR_NG;
				}
				break;
			case E_FJ_MOVIE_VIDEO_SIZE_1920_1080 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_1920_960 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_1440_1080 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_1440_720 :
				if( cfg_param->frame_rate > 6000 ){
					BF_Debug_Print_Error(("Param Error. Over spec in All Iframe \n" ));
					return FJ_ERR_NG;
				}
				break;
			case E_FJ_MOVIE_VIDEO_SIZE_1280_1024 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_1280_960 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_1280_768 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_1280_720 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_1024_768 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_864_480 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_960_540 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_960_480 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_848_480 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_800_600 :
			case E_FJ_MOVIE_VIDEO_SIZE_720_576 :
			case E_FJ_MOVIE_VIDEO_SIZE_720_480 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_640_480 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_640_360 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_480_270 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_432_240 :	// FALL THROUGH
			case E_FJ_MOVIE_VIDEO_SIZE_320_240 :
			case E_FJ_MOVIE_VIDEO_SIZE_1600_896:
			case E_FJ_MOVIE_VIDEO_SIZE_1024_576:
			case E_FJ_MOVIE_VIDEO_SIZE_800_448:
			case E_FJ_MOVIE_VIDEO_SIZE_352_288:
			case E_FJ_MOVIE_VIDEO_SIZE_2592_1944:
			case E_FJ_MOVIE_VIDEO_SIZE_2304_1296:
			case E_FJ_MOVIE_VIDEO_SIZE_1920_1280:
			case E_FJ_MOVIE_VIDEO_SIZE_640_512:
			case E_FJ_MOVIE_VIDEO_SIZE_384_288:
				if( cfg_param->frame_rate > 12000 ){
					BF_Debug_Print_Error(("Param Error. Over spec in All Iframe \n" ));
					return FJ_ERR_NG;
				}
				break;
			default :
				BF_Debug_Print_Error(("Param Error. Image Size=%d\n", cfg_param->image_size ));
				return FJ_ERR_NG;
		}
	}
	
	if( cfg_param->gop_struct == FJ_MOVIE_VIDEO_GOP_STRUCT_IBP ){
		if( (cfg_param->frame_num_of_gop % 2) != 0 ){
			BF_Debug_Print_Error(("Param Error. frame num of GOP & GOP struct unmatch\n" ));
			return FJ_ERR_NG;
		}
	}
	else if( cfg_param->gop_struct == FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP ){
		if( (cfg_param->frame_num_of_gop % 3) != 0 ){
			BF_Debug_Print_Error(("Param Error. frame num of GOP & GOP struct unmatch\n" ));
			return FJ_ERR_NG;
		}
	}
	else{
		// IPPP is nothing
	}
	
	// Closed GOP
	switch( cfg_param->closed_gop_flag ){
		case FJ_MOVIE_VIDEO_CLOSED_GOP_ON:
			gBF_Encoder_H264_If_Param[stream_ch].closed_gop = H264_ON;
			break;
		case FJ_MOVIE_VIDEO_CLOSED_GOP_OFF :
			gBF_Encoder_H264_If_Param[stream_ch].closed_gop = H264_OFF;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Closed GOP flag=%d\n", cfg_param->closed_gop_flag ));
			return FJ_ERR_NG;
	}
	
	// GOP Num IDR to IDR
	if( cfg_param->gop_num_idr_to_idr > 0 ){
		gBF_Encoder_H264_If_Param[stream_ch].gop_num_idr_to_idr = cfg_param->gop_num_idr_to_idr;	// new
	}
	else{
		BF_Debug_Print_Error(("Param Error. GOP num IDR to IDR=%d\n", cfg_param->gop_num_idr_to_idr ));
		return FJ_ERR_NG;
	}
	
	// Profile
	switch( cfg_param->profile ){
		case FJ_MOVIE_VIDEO_PROFILE_HIGH :
			gBF_Encoder_H264_If_Param[stream_ch].profile = E_H264_IF_PROFILE_HIGH;
			break;
		case FJ_MOVIE_VIDEO_PROFILE_MAIN :
			gBF_Encoder_H264_If_Param[stream_ch].profile = E_H264_IF_PROFILE_MAIN;
			break;
		case FJ_MOVIE_VIDEO_PROFILE_CONSTRAINED_BASELINE :
			gBF_Encoder_H264_If_Param[stream_ch].profile = E_H264_IF_PROFILE_CONSTRAINED_BASELINE;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Profile=%d\n", cfg_param->profile ));
			return FJ_ERR_NG;
	}
	
	// Level
	switch( cfg_param->level ){
		case FJ_MOVIE_VIDEO_LEVEL_1_3 :
			gBF_Encoder_H264_If_Param[stream_ch].level = E_H264_IF_LEVEL_1_3;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_2 :
			gBF_Encoder_H264_If_Param[stream_ch].level = E_H264_IF_LEVEL_2;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_2_1 :
			gBF_Encoder_H264_If_Param[stream_ch].level = E_H264_IF_LEVEL_2_1;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_2_2 :
			gBF_Encoder_H264_If_Param[stream_ch].level = E_H264_IF_LEVEL_2_2;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_3 :
			gBF_Encoder_H264_If_Param[stream_ch].level = E_H264_IF_LEVEL_3;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_3_1 :
			gBF_Encoder_H264_If_Param[stream_ch].level = E_H264_IF_LEVEL_3_1;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_3_2 :
			gBF_Encoder_H264_If_Param[stream_ch].level = E_H264_IF_LEVEL_3_2;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_4 :
			gBF_Encoder_H264_If_Param[stream_ch].level = E_H264_IF_LEVEL_4;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_4_1 :
			gBF_Encoder_H264_If_Param[stream_ch].level = E_H264_IF_LEVEL_4_1;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_4_2 :
			gBF_Encoder_H264_If_Param[stream_ch].level = E_H264_IF_LEVEL_4_2;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_5 :
			gBF_Encoder_H264_If_Param[stream_ch].level = E_H264_IF_LEVEL_5;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_5_1 :
			gBF_Encoder_H264_If_Param[stream_ch].level = E_H264_IF_LEVEL_5_1;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_5_2 :
			gBF_Encoder_H264_If_Param[stream_ch].level = E_H264_IF_LEVEL_5_2;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Level=%d\n", cfg_param->level ));
			return FJ_ERR_NG;
	}
	
	// Entropy coding Mode
	switch( cfg_param->entropy_coding ){
		case FJ_MOVIE_VIDEO_VLC_MODE_CABAC:
			gBF_Encoder_H264_If_Param[stream_ch].vlc_mode = E_H264_IF_VLC_MODE_CABAC;
			break;
		case FJ_MOVIE_VIDEO_VLC_MODE_CAVLC :
			gBF_Encoder_H264_If_Param[stream_ch].vlc_mode = E_H264_IF_VLC_MODE_CAVLC;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Entropy Coding=%d\n", cfg_param->entropy_coding ));
			return FJ_ERR_NG;
	}
	
	// Scan Mode
	gBF_Encoder_H264_If_Param[stream_ch].scan_mode = E_H264_IF_SCAN_MODE_PROGRESSIVE;	// Fixed
	
	// Flag to Insert hrd_parameters in VUI/BuffeingPeriodSEI/removal_delay in PictureTimingSEI.
	if( (cfg_param->buffer_ctrl_flag == FJ_MOVIE_ON) || (cfg_param->buffer_ctrl_flag == FJ_MOVIE_OFF) ){
		gBF_Encoder_H264_If_Param[stream_ch].buffer_ctrl_flag = cfg_param->buffer_ctrl_flag;
	}
	else{
		BF_Debug_Print_Error(("Param Error. Buffer Control flag=%d\n", cfg_param->buffer_ctrl_flag ));
		return FJ_ERR_NG;
	}
	
	// Flag to Pictstruct in PictureTiming SEI.
	if( (cfg_param->pictstruct_flag == FJ_MOVIE_ON) || (cfg_param->pictstruct_flag == FJ_MOVIE_OFF) ){
		gBF_Encoder_H264_If_Param[stream_ch].pictstruct_flag = cfg_param->pictstruct_flag;
	}
	else{
		BF_Debug_Print_Error(("Param Error. PictStruct flag=%d\n", cfg_param->pictstruct_flag ));
		return FJ_ERR_NG;
	}
	
	// Flag to Recovery Point SEI
	if( (cfg_param->recovery_point_sei_flag == FJ_MOVIE_ON) || (cfg_param->recovery_point_sei_flag == FJ_MOVIE_OFF) ){
		gBF_Encoder_H264_If_Param[stream_ch].recovery_point_sei_flag = cfg_param->recovery_point_sei_flag;
	}
	else{
		BF_Debug_Print_Error(("Param Error. RecoveryPointSEI flag=%d\n", cfg_param->recovery_point_sei_flag ));
		return FJ_ERR_NG;
	}
	
	// Flag to End Of Stream
	if( (cfg_param->end_of_stream_flag == FJ_MOVIE_ON) || (cfg_param->end_of_stream_flag == FJ_MOVIE_OFF) ){
		gBF_Encoder_H264_If_Param[stream_ch].end_of_stream_flag = cfg_param->end_of_stream_flag;
	}
	else{
		BF_Debug_Print_Error(("Param Error. End Of Stream flag=%d\n", cfg_param->end_of_stream_flag ));
		return FJ_ERR_NG;
	}
	
	// Flag to End Of Sequence
	if( (cfg_param->end_of_sequence_flag == FJ_MOVIE_ON) || (cfg_param->end_of_sequence_flag == FJ_MOVIE_OFF) ){
		gBF_Encoder_H264_If_Param[stream_ch].end_of_sequence_flag = cfg_param->end_of_sequence_flag;
	}
	else{
		BF_Debug_Print_Error(("Param Error. End Of Sequence flag=%d\n", cfg_param->end_of_sequence_flag ));
		return FJ_ERR_NG;
	}
	
	// Flag to Insert Filler Data
	if( (cfg_param->filler_flag == FJ_MOVIE_ON) || (cfg_param->filler_flag == FJ_MOVIE_OFF) ){
		gBF_Encoder_H264_If_Param[stream_ch].filler_flag = cfg_param->filler_flag;
	}
	else{
		BF_Debug_Print_Error(("Param Error. Filler flag=%d\n", cfg_param->filler_flag ));
		return FJ_ERR_NG;
	}
	
	// Flag to VECTOR info notify
	if( (cfg_param->vector_info_flag == FJ_MOVIE_ON) || (cfg_param->vector_info_flag == FJ_MOVIE_OFF) ){
		gBF_Encoder_H264_If_Param[stream_ch].vector_info_flag			= cfg_param->vector_info_flag;
	}
	else{
		BF_Debug_Print_Error(("Param Error. VectorInfo flag=%d\n", cfg_param->vector_info_flag ));
		return FJ_ERR_NG;
	}
	
	// Flag to TimeLapse
	if( (cfg_param->time_lapse_flag == FJ_MOVIE_ON) || (cfg_param->time_lapse_flag == FJ_MOVIE_OFF) ){
		gBF_Encoder_H264_If_Param[stream_ch].time_lapse_flag			= cfg_param->time_lapse_flag;
	}
	else{
		BF_Debug_Print_Error(("Param Error. TimeLapse flag=%d\n", cfg_param->time_lapse_flag ));
		return FJ_ERR_NG;
	}
	
	// VUI
	gBF_Encoder_H264_If_Param[stream_ch].vui.video_signal_type_present_flag			= cfg_param->vui_param.video_signal_type_present_flag;
	gBF_Encoder_H264_If_Param[stream_ch].vui.video_format							= cfg_param->vui_param.video_format;
	gBF_Encoder_H264_If_Param[stream_ch].vui.video_full_range_flag					= cfg_param->vui_param.video_full_range_flag;
	gBF_Encoder_H264_If_Param[stream_ch].vui.colour_description_present_flag		= cfg_param->vui_param.colour_description_present_flag;	
	gBF_Encoder_H264_If_Param[stream_ch].vui.colour_primaries						= cfg_param->vui_param.colour_primaries;
	gBF_Encoder_H264_If_Param[stream_ch].vui.transfer_characteristics				= cfg_param->vui_param.transfer_characteristics;
	gBF_Encoder_H264_If_Param[stream_ch].vui.matrix_coefficients					= cfg_param->vui_param.matrix_coefficients;
	gBF_Encoder_H264_If_Param[stream_ch].vui.aspect_ratio_info_present_flag			= cfg_param->vui_param.aspect_ratio_info_present_flag;
	gBF_Encoder_H264_If_Param[stream_ch].vui.aspect_ratio_idc						= cfg_param->vui_param.aspect_ratio_idc;
	gBF_Encoder_H264_If_Param[stream_ch].vui.sar_width								= cfg_param->vui_param.sar_width;
	gBF_Encoder_H264_If_Param[stream_ch].vui.sar_height								= cfg_param->vui_param.sar_height;
	gBF_Encoder_H264_If_Param[stream_ch].vui.chroma_loc_info_present_flag			= cfg_param->vui_param.chroma_loc_info_present_flag;
	gBF_Encoder_H264_If_Param[stream_ch].vui.chroma_sample_loc_top_field			= cfg_param->vui_param.chroma_sample_loc_type_top_field;
	gBF_Encoder_H264_If_Param[stream_ch].vui.chroma_sample_loc_bottom_field			= cfg_param->vui_param.chroma_sample_loc_type_bottom_field;
	
	// Scaling List
	if(( cfg_param->scaling_list.scaling_list_flg >= H264_ON ) && ( cfg_param->scaling_list.scaling_list_flg <= 2 )){
		gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_flg = cfg_param->scaling_list.scaling_list_flg;
		memcpy((VOID*)(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_4x4_Y1),  cfg_param->scaling_list.scaling_list_4x4_Y1,  sizeof(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_4x4_Y1));
		memcpy((VOID*)(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_4x4_Cb1), cfg_param->scaling_list.scaling_list_4x4_Cb1, sizeof(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_4x4_Cb1));
		memcpy((VOID*)(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_4x4_Cr1), cfg_param->scaling_list.scaling_list_4x4_Cr1, sizeof(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_4x4_Cr1));
		memcpy((VOID*)(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_4x4_Y2),  cfg_param->scaling_list.scaling_list_4x4_Y2,  sizeof(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_4x4_Y2));
		memcpy((VOID*)(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_4x4_Cb2), cfg_param->scaling_list.scaling_list_4x4_Cb2, sizeof(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_4x4_Cb2));
		memcpy((VOID*)(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_4x4_Cr2), cfg_param->scaling_list.scaling_list_4x4_Cr2, sizeof(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_4x4_Cr2));
		memcpy((VOID*)(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_8x8_Y1),  cfg_param->scaling_list.scaling_list_8x8_Y1,  sizeof(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_8x8_Y1));
		memcpy((VOID*)(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_8x8_Y2),  cfg_param->scaling_list.scaling_list_8x8_Y2,  sizeof(gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_8x8_Y2));
	}
	else if( cfg_param->scaling_list.scaling_list_flg == H264_OFF ){
		gBF_Encoder_H264_If_Param[stream_ch].initial_scalinglist_info.scaling_list_flg = H264_OFF;
	}
	else{
		BF_Debug_Print_Error(("Param Error. ScalingList\n"));
		return FJ_ERR_NG;
	}
	
	// Flag to Slice Encode Mode
	if( cfg_param->slice_encode_flag == FJ_MOVIE_ON ){
		if( (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_1280_1024) ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_1280_768) ||
		    (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_1280_720) ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_1024_768) ||
		    (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_960_540) ||
		    (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_960_480) ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_800_600)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_864_480)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_848_480)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_720_576)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_720_480)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_640_480)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_640_360)  ||
		    (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_480_270)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_432_240)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_320_240)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_1600_896)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_1024_576)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_800_448)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_352_288)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_2592_1944)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_2304_1296)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_1920_1280)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_640_512)  ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_384_288)
			){
			
			BF_Debug_Print_Error(("Param Error. Slice Encode Flag ON. But can not supported size=%d\n", cfg_param->image_size ));
			return FJ_ERR_NG;
		}
		gBF_Encoder_H264_If_Param[stream_ch].slice_encode_flag = H264_ON;
	}
	else if( cfg_param->slice_encode_flag == FJ_MOVIE_OFF ){
		gBF_Encoder_H264_If_Param[stream_ch].slice_encode_flag = H264_OFF;
	}
	else{
		BF_Debug_Print_Error(("Param Error. Slice Encode Flag=%d\n", cfg_param->slice_encode_flag ));
		return FJ_ERR_NG;
	}
	
// --- REMOVE_H264_IQ_ENABLE BEGIN ---
	if( cfg_param->prefetch_bypath_flag == FJ_MOVIE_ON ){
		gBF_Encoder_H264_If_Param[stream_ch].prefetch_bypath = H264_ON;
	}
	else if( cfg_param->prefetch_bypath_flag == FJ_MOVIE_OFF ){
		gBF_Encoder_H264_If_Param[stream_ch].prefetch_bypath = H264_OFF;
	}
	else{
		BF_Debug_Print_Error(("Param Error. Prefetch Bypath Encode Flag=%d\n", cfg_param->prefetch_bypath_flag ));
		return FJ_ERR_NG;
	}
// --- REMOVE_H264_IQ_ENABLE END ---
	
	gBF_Encoder_H264_If_Param[stream_ch].delay_notify_flag = cfg_param->delay_notify_flag;
	
	// Fixed Value
	// --- Flag to Allow CPB Buffer Over ---
	gBF_Encoder_H264_If_Param[stream_ch].CPB_allow_flg = 1;
	// --- Maximum QP ---
	gBF_Encoder_H264_If_Param[stream_ch].max_qp = 51;
	// --- Minimum QP ---
	gBF_Encoder_H264_If_Param[stream_ch].min_qp = 0;
	// --- Value for initial_cpb_removal_delay(msec) ---
	gBF_Encoder_H264_If_Param[stream_ch].CPB_removal_delay = 400;
// --- REMOVE_H264_IQ_ENABLE BEGIN ---
	// --- Next Frame Compare Mode ---
	gBF_Encoder_H264_If_Param[stream_ch].next_frame_comp_flg = 0;
// --- REMOVE_H264_IQ_ENABLE END ---
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Get_H264_Config( E_VIDEO_ENOCDE_ID video_enc_id, T_H264_IF_PARAM_RECORD* cfg_param )
{
	FJ_ERR_CODE ercd;
	E_H264_IF_STREAM_CH stream_ch;
	
	ercd = BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, &stream_ch );
	if( ercd != FJ_ERR_OK ){
		return FJ_ERR_NG;
	}
	
//	*cfg_param = gBF_Encoder_H264_If_Param[stream_ch];
	memcpy( (void*)cfg_param, (const void*)&gBF_Encoder_H264_If_Param[stream_ch], sizeof(T_H264_IF_PARAM_RECORD) );
	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_Movie_Video_Attr_Set_H265_Config( E_VIDEO_ENOCDE_ID video_enc_id, T_FJ_MOVIE_H265_CONFIG_PARAM* cfg_param )
{
	FJ_ERR_CODE ercd;
	E_H265_IF_STREAM_CH stream_ch;
	UINT32 onesec_frame;
	
	ercd = BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H265( video_enc_id, &stream_ch );
	if( ercd != FJ_ERR_OK ){
		return FJ_ERR_NG;
	}
	
	gBF_Encoder_H265_If_Param[stream_ch].stream_ch = stream_ch;
	
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	// Check Performance
	INT32 i;
	INT32 stream_cnt = 0;
	E_H265_IF_STREAM_CH use_stream_ch = E_H265_IF_STREAM_CH_0;
	
	for( i=0; i<E_H265_IF_STREAM_CH_MAX; i++ ){
		if( gBF_Encoder_H265_If_Param[i].enable_flg == H265_ON ){
			stream_cnt++;
			use_stream_ch = i;
		}
	}
	if( stream_cnt > 0 ){
		// Already Stting
		if( (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_4000_3000) ||
			(gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_4096_2304) ||
			(gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_3000_3000) ){
			if( gBF_Encoder_H265_If_Param[use_stream_ch].frame_rate > E_H265_IF_FRAME_RATE_30_0 ){
				BF_Debug_Print_Error(("Param Error. Secondary Over Spec\n"));
				return FJ_ERR_NG;
			}
		}
		else if( (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_4096_2160) ||
				 (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_4096_2048) ||
				 (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_3840_2160) ||
                 (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_3840_1920)){
			if( gBF_Encoder_H265_If_Param[use_stream_ch].frame_rate > E_H265_IF_FRAME_RATE_30_0 ){
				BF_Debug_Print_Error(("Param Error. Secondary Over Spec\n"));
				return FJ_ERR_NG;
			}
		}
		else if( (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_4096_2160) ||
				 (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_4096_2048) ||
				 (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_3840_2160) ||
                 (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_3840_1920)){
			if( gBF_Encoder_H265_If_Param[use_stream_ch].frame_rate > E_H265_IF_FRAME_RATE_30_0 ){
				BF_Debug_Print_Error(("Param Error. Secondary Over Spec\n"));
				return FJ_ERR_NG;
			}
		}
		else if( gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_2704_2028 ){
			if( gBF_Encoder_H265_If_Param[use_stream_ch].frame_rate > E_H265_IF_FRAME_RATE_48_0 ){
				BF_Debug_Print_Error(("Param Error. Secondary Over Spec\n"));
				return FJ_ERR_NG;
			}
		}
		else if( (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_2704_1520) ||
				 (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_1920_1440) ){
			if( gBF_Encoder_H265_If_Param[use_stream_ch].frame_rate > E_H265_IF_FRAME_RATE_60_0 ){
				BF_Debug_Print_Error(("Param Error. Secondary Over Spec\n"));
				return FJ_ERR_NG;
			}
		}
		else if( (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_1920_1080) ||
				 (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_1440_1080) ||
				 (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_1440_720) ||
				 (gBF_Encoder_H265_If_Param[use_stream_ch].size == E_H265_IF_IMAGE_SIZE_1280_960) ){
			if( gBF_Encoder_H265_If_Param[use_stream_ch].frame_rate > E_H265_IF_FRAME_RATE_120_0 ){
				BF_Debug_Print_Error(("Param Error. Secondary Over Spec\n"));
				return FJ_ERR_NG;
			}
		}
		
		// Current Setting
		if( (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_4000_3000) ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_4096_2304) ||
			(cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_3000_3000) ){
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Secondary Over Spec\n"));
				return FJ_ERR_NG;
			}
		}
		else if( (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_4096_2160) ||
				 (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_4096_2048) ||
				 (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_3840_2160) ||
                 (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_3840_1920)){
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Secondary Over Spec\n"));
				return FJ_ERR_NG;
			}
		}
		else if( (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_4096_2160) ||
				 (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_4096_2048) ||
				 (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_3840_2160) ||
                 (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_3840_1920)){
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Secondary Over Spec\n"));
				return FJ_ERR_NG;
			}
		}
		else if( cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_2704_2028 ){
			if( cfg_param->frame_rate > 4800 ){
				BF_Debug_Print_Error(("Param Error. Secondary Over Spec\n"));
				return FJ_ERR_NG;
			}
		}
		else if( (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_2704_1520) ||
				 (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_1920_1440) ){
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Secondary Over Spec\n"));
				return FJ_ERR_NG;
			}
		}
		else if( (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_1920_1080) ||
				 (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_1440_1080) ||
				 (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_1440_720) ||
				 (cfg_param->image_size == E_FJ_MOVIE_VIDEO_SIZE_1280_960) ){
			if( cfg_param->frame_rate > 12000 ){
				BF_Debug_Print_Error(("Param Error. Secondary Over Spec\n"));
				return FJ_ERR_NG;
			}
		}
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---

	// Enable flag check
	switch( cfg_param->enable_flag ){
		case FJ_MOVIE_ON :
			gBF_Encoder_H265_If_Param[stream_ch].enable_flg = H265_ON;
			break;
		case FJ_MOVIE_OFF :
			gBF_Encoder_H265_If_Param[stream_ch].enable_flg = H265_OFF;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Enable Flag=%d\n", cfg_param->enable_flag ));
			return FJ_ERR_NG;
	}
	
	// Image Size
	switch( cfg_param->image_size ){
// --- REMOVE_COMPILE_OPT BEGIN ---
#ifdef CO_SUPPORT_60FPS
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_60FPS BEGIN ---
		case E_FJ_MOVIE_VIDEO_SIZE_4000_3000 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_4000_3000;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 4032;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 3008;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 16;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 4;
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
// --- REMOVE_60FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#endif // CO_SUPPORT_60FPS
// --- REMOVE_COMPILE_OPT END ---
		case E_FJ_MOVIE_VIDEO_SIZE_4096_2160 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_4096_2160;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 4096;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 2160;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
// --- REMOVE_COMPILE_OPT BEGIN ---
#if defined(CO_SUPPORT_60FPS) && defined(CO_SUPPORT_30FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_60_30FPS BEGIN ---
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_60_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#elif defined(CO_SUPPORT_60FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_60FPS BEGIN ---
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_60FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#else  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_30FPS BEGIN ---
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#endif  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_3840_2160 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_3840_2160;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 3840;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 2160;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
// --- REMOVE_COMPILE_OPT BEGIN ---
#if defined(CO_SUPPORT_60FPS) && defined(CO_SUPPORT_30FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_60_30FPS BEGIN ---
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_60_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#elif defined(CO_SUPPORT_60FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_60FPS BEGIN ---
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_60FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#else  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_30FPS BEGIN ---
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#endif  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_3840_1920 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_3840_1920;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 3840;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 1920;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
// --- REMOVE_COMPILE_OPT BEGIN ---
#if defined(CO_SUPPORT_60FPS) && defined(CO_SUPPORT_30FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_60_30FPS BEGIN ---
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_60_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#elif defined(CO_SUPPORT_60FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_60FPS BEGIN ---
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_60FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#else  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_30FPS BEGIN ---
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#endif  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_3000_3000 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_3000_3000;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 3008;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 3000;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 4;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
// --- REMOVE_COMPILE_OPT BEGIN ---
#if defined(CO_SUPPORT_60FPS) && defined(CO_SUPPORT_30FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_60_30FPS BEGIN ---
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_60_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#elif defined(CO_SUPPORT_60FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_60FPS BEGIN ---
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_60FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#else  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_30FPS BEGIN ---
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#endif  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2704_2028 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_2704_2028;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 2752;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 2048;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 24;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 10;
// --- REMOVE_COMPILE_OPT BEGIN ---
#if defined(CO_SUPPORT_60FPS) && defined(CO_SUPPORT_30FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_60_30FPS BEGIN ---
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_60_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#elif defined(CO_SUPPORT_60FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_60FPS BEGIN ---
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_60FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#else  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_30FPS BEGIN ---
			if( cfg_param->frame_rate > 3000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#endif  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2704_1520 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_2704_1520;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 2752;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 1520;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 24;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
// --- REMOVE_COMPILE_OPT BEGIN ---
#if defined(CO_SUPPORT_60FPS) && defined(CO_SUPPORT_30FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_60_30FPS BEGIN ---
			if( cfg_param->frame_rate > 12000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_60_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#elif defined(CO_SUPPORT_60FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_60FPS BEGIN ---
			if( cfg_param->frame_rate > 12000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_60FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#else  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_30FPS BEGIN ---
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#endif  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2560_1920 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_2560_1920;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 2560;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 1920;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2560_1440 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_2560_1440;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 2560;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 1440;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1440 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1920_1440;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1920;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 1440;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1080 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1920_1080;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1920;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 1080;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
// --- REMOVE_COMPILE_OPT BEGIN ---
#if defined(CO_SUPPORT_60FPS) && defined(CO_SUPPORT_30FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_60_30FPS BEGIN ---
			if( cfg_param->frame_rate > 24000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_60_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#elif defined(CO_SUPPORT_60FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_60FPS BEGIN ---
			if( cfg_param->frame_rate > 24000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_60FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#else  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_30FPS BEGIN ---
			if( cfg_param->frame_rate > 12000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#endif  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_960 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1920_960;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1920;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 960;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1504_1504 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1504_1504;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1536;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 1504;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 16;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
// --- REMOVE_COMPILE_OPT BEGIN ---
#if defined(CO_SUPPORT_60FPS) && defined(CO_SUPPORT_30FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_60_30FPS BEGIN ---
			if( cfg_param->frame_rate > 24000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_60_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#elif defined(CO_SUPPORT_60FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_60FPS BEGIN ---
			if( cfg_param->frame_rate > 24000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_60FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#else  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_30FPS BEGIN ---
			if( cfg_param->frame_rate > 12000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#endif  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1440_1080 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1440_1080;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1472;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 1080;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 16;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
// --- REMOVE_COMPILE_OPT BEGIN ---
#if defined(CO_SUPPORT_60FPS) && defined(CO_SUPPORT_30FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_60_30FPS BEGIN ---
			if( cfg_param->frame_rate > 24000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_60_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#elif defined(CO_SUPPORT_60FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_60FPS BEGIN ---
			if( cfg_param->frame_rate > 24000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_60FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#else  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_30FPS BEGIN ---
			if( cfg_param->frame_rate > 12000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#endif  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
			break;
/*
		case E_FJ_MOVIE_VIDEO_SIZE_1440_720 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1440_720;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1472;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 720;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 16;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
// --- REMOVE_COMPILE_OPT BEGIN ---
#if defined(CO_SUPPORT_60FPS) && defined(CO_SUPPORT_30FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_60_30FPS BEGIN ---
			if( cfg_param->frame_rate > 24000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_60_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#elif defined(CO_SUPPORT_60FPS)
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_60FPS BEGIN ---
			if( cfg_param->frame_rate > 24000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_60FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#else  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_ONLY_30FPS BEGIN ---
			if( cfg_param->frame_rate > 12000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
// --- REMOVE_ONLY_30FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#endif  // CO_SUPPORT_30FPS
// --- REMOVE_COMPILE_OPT END ---
			break;
*/
		case E_FJ_MOVIE_VIDEO_SIZE_1440_720 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1440_720;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1440;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 720;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			if( cfg_param->frame_rate > 6000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_960 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1280_960;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1280;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 960;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			if( cfg_param->frame_rate > 12000 ){
				BF_Debug_Print_Error(("Param Error. Size&FrameRate Combination. %d, %d\n", cfg_param->image_size, cfg_param->frame_rate ));
				return FJ_ERR_NG;
			}
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_720 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1280_720;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1280;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 720;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_960_540 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_960_540;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 960;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 544;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 2;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_800_600 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_800_600;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 800;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 600;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_960_480 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_960_480;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 960;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 480;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_864_480 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_864_480;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 896;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 480;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 16;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_848_480 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_848_480;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 896;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 480;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 24;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_480 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_640_480;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 640;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 480;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_360 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_640_360;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 640;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 384;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 12;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_480_270 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_480_270;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 512;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 288;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 16;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 9;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_432_240 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_432_240;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 448;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 256;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 8;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 8;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_320_240 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_320_240;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 320;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 256;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 8;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_720_576 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_720_576;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 720;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 576;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_720_480 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_720_480;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 768;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 480;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 24;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1024_768 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1024_768;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1024;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 768;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_768 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1280_768;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1280;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 768;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_1024 :
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1280_1024;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1280;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 1024;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1600_896:
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1600_896;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1600;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 896;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1024_576:
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1024_576;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1024;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 576;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_800_448:
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_800_448;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 800;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 448;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_352_288:
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_352_288;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 352;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 288;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2592_1944:
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_2592_1944;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 2592;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 1952;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 4;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2304_1296:
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_2304_1296;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 2304;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 1312;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 8;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1280:
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_1920_1280;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 1920;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 1280;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_512:
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_640_512;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 640;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 512;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_384_288:
			gBF_Encoder_H265_If_Param[stream_ch].size						= E_H265_IF_IMAGE_SIZE_384_288;
			gBF_Encoder_H265_If_Param[stream_ch].width						= 384;
			gBF_Encoder_H265_If_Param[stream_ch].lines						= 288;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_right_offset	= 0;
			gBF_Encoder_H265_If_Param[stream_ch].frame_crop_bottom_offset	= 0;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Image Size=%d\n", cfg_param->image_size ));
			return FJ_ERR_NG;
	}
	
	// Stream Format
	switch( cfg_param->stream_format ){
		case FJ_MOVIE_STREAM_FORMAT_ES :
			gBF_Encoder_H265_If_Param[stream_ch].stream_format	= E_H265_IF_STREAM_FORMAT_ES;
			break;
		case FJ_MOVIE_STREAM_FORMAT_PES :
			gBF_Encoder_H265_If_Param[stream_ch].stream_format	= E_H265_IF_STREAM_FORMAT_PES;
			break;
		case FJ_MOVIE_STREAM_FORMAT_ES_HEADER :
			gBF_Encoder_H265_If_Param[stream_ch].stream_format	= E_H265_IF_STREAM_FORMAT_ES_HEADER;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Stream Format=%d\n", cfg_param->stream_format ));
			return FJ_ERR_NG;
	}
	
	// BitRate Algorithm
	switch( cfg_param->bitrate_algorithm ){
		case FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_CBR :
			gBF_Encoder_H265_If_Param[stream_ch].rate_mode = E_H265_IF_RATE_MODE_CBR;
			break;
		case FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_VBR :
			gBF_Encoder_H265_If_Param[stream_ch].rate_mode = E_H265_IF_RATE_MODE_VBR;
			break;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		case FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_CVBR :
			gBF_Encoder_H265_If_Param[stream_ch].rate_mode = E_H265_IF_RATE_MODE_CVBR;
			break;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		case FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_ABR :
			gBF_Encoder_H265_If_Param[stream_ch].rate_mode = E_H265_IF_RATE_MODE_ABR;
			break;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
		default :
			BF_Debug_Print_Error(("Param Error. BitRate Algorithm=%d\n", cfg_param->bitrate_algorithm ));
			return FJ_ERR_NG;
	}
	
	// Bitrate
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	if( (cfg_param->bitrate > 0) && (cfg_param->bitrate <= 120000000) ){
		gBF_Encoder_H265_If_Param[stream_ch].bitrate = cfg_param->bitrate;
	}
	else{
		BF_Debug_Print_Error(("Param Error. BitRate=%d\n", cfg_param->bitrate ));
		return FJ_ERR_NG;
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	if( cfg_param->bitrate_algorithm != FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_ABR ){
		if( (cfg_param->bitrate > 0) && (cfg_param->bitrate <= 120000000) ){
			gBF_Encoder_H265_If_Param[stream_ch].bitrate = cfg_param->bitrate;
		}
		else{
			BF_Debug_Print_Error(("Param Error. BitRate=%d\n", cfg_param->bitrate ));
			return FJ_ERR_NG;
		}
	}
	else{
		if( cfg_param->bitrate == 0 ){
			gBF_Encoder_H265_If_Param[stream_ch].bitrate = cfg_param->bitrate;
		}
		else{
			BF_Debug_Print_Error(("Param Error. BitRate=%d\n", cfg_param->bitrate ));
			return FJ_ERR_NG;
		}
	}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
	
	// Average Bitrate
	if( (cfg_param->average_bitrate > 0) && (cfg_param->bitrate <= 120000000) ){
		gBF_Encoder_H265_If_Param[stream_ch].average_bitrate = cfg_param->average_bitrate;
	}
	else{
		BF_Debug_Print_Error(("Param Error. Average BitRate=%d\n", cfg_param->average_bitrate ));
		return FJ_ERR_NG;
	}
		
	switch( cfg_param->frame_rate ){
		case 7300:
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_73_0;
			onesec_frame = 73;
			break;
		case 7200:
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_72_0;
			onesec_frame = 72;
			break;
		case 7000:
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_70_0;
			onesec_frame = 70;
			break;
		case 2500:
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_25_0;
			onesec_frame = 25;
			break;
		case 2900:
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_29_0;
			onesec_frame = 29;
			break;
		case 23976:	// 239.76fps
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_239_76;
			onesec_frame = 240;
			break;
		case 11988:	// 119.88fps
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_119_88;
			onesec_frame = 120;
			break;
		case 5994:	// 59.94fps (NTSC)
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_59_94;
			onesec_frame = 60;
			break;
		case 2997:	// 29.97fps (NTSC)
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_29_97;
			onesec_frame = 30;
			break;
		case 2397:	// 23.97fps (NTSC)
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_23_97;
			onesec_frame = 24;
			break;
		case 1498:	// 14.98fps (NTSC)
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_14_98;
			onesec_frame = 15;
			break;
			
		case 24000:	// 240fps
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_240_0;
			onesec_frame = 240;
			break;
		case 12000:	// 120fps
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_120_0;
			onesec_frame = 120;
			break;
		case 6000:	// 60fps
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_60_0;
			onesec_frame = 60;
			break;
		case 5000:
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_50_0;
			onesec_frame = 50;
			break;
		case 3000:	// 30fps
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_30_0;
			onesec_frame = 30;
			break;
		case 2400:	// 24fps
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_24_0;
			onesec_frame = 12;
			break;
		case 1500:	// 15fps
			gBF_Encoder_H265_If_Param[stream_ch].frame_rate = E_H265_IF_FRAME_RATE_15_0;
			onesec_frame = 15;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. FrameRate=%d\n", cfg_param->frame_rate ));
			return FJ_ERR_NG;
	}
	gBF_Encoder_H265_Framerate_Value[stream_ch] = cfg_param->frame_rate;
	
	// GOP struct
	switch( cfg_param->gop_struct ){
		case FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP :
			gBF_Encoder_H265_If_Param[stream_ch].gop_struct = E_H265_IF_GOP_STR_IPPP;
			break;
		case FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP :
			gBF_Encoder_H265_If_Param[stream_ch].gop_struct = E_H265_IF_GOP_STR_IBBP;
			break;
		case FJ_MOVIE_VIDEO_GOP_STRUCT_IIII :
			gBF_Encoder_H265_If_Param[stream_ch].gop_struct = E_H265_IF_GOP_STR_IIII;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. GOP Struct=%d\n", cfg_param->gop_struct ));
			return FJ_ERR_NG;
	}
	
	// Frame Number of GOP
	gBF_Encoder_H265_If_Param[stream_ch].gop_num = cfg_param->frame_num_of_gop;
	
	if( cfg_param->gop_struct != FJ_MOVIE_VIDEO_GOP_STRUCT_IIII && cfg_param->gop_struct != FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP){
		if( (cfg_param->frame_num_of_gop == 0) ||
			(cfg_param->frame_num_of_gop > onesec_frame) ){
			BF_Debug_Print_Error(("Param Error. frame num of GOP=%d\n", cfg_param->frame_num_of_gop ));
			return FJ_ERR_NG;
		}
		if( (onesec_frame % cfg_param->frame_num_of_gop) != 0){
			BF_Debug_Print_Error(("Param Error. frame num of GOP=%d (not multiple framerate)\n", cfg_param->frame_num_of_gop ));
			return FJ_ERR_NG;
		}
		if( ((onesec_frame / cfg_param->frame_num_of_gop) != 1) &&
			((onesec_frame / cfg_param->frame_num_of_gop) != 2) &&
			((onesec_frame / cfg_param->frame_num_of_gop) != 3) &&
			((onesec_frame / cfg_param->frame_num_of_gop) != 4) ){
			BF_Debug_Print_Error(("Param Error. frame num of GOP=%d (not allowed range)\n", cfg_param->frame_num_of_gop ));
			return FJ_ERR_NG;
		}
	}
	
	if( cfg_param->gop_struct == FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP ){
		if( (cfg_param->frame_num_of_gop % 3) != 0 ){
			BF_Debug_Print_Error(("Param Error. frame num of GOP & GOP struct unmatch\n" ));
			return FJ_ERR_NG;
		}
	}
	else{
		// IPPP is nothing
	}
	
	// Closed GOP
	switch( cfg_param->closed_gop_flag ){
		case FJ_MOVIE_VIDEO_CLOSED_GOP_ON:
			gBF_Encoder_H265_If_Param[stream_ch].closed_gop = H265_ON;
			break;
		case FJ_MOVIE_VIDEO_CLOSED_GOP_OFF :
			gBF_Encoder_H265_If_Param[stream_ch].closed_gop = H265_OFF;
			break;
		default :
			BF_Debug_Print_Error(("Param Error. Closed GOP flag=%d\n", cfg_param->closed_gop_flag ));
			return FJ_ERR_NG;
	}
	
	// GOP Num IDR to IDR
	if( cfg_param->gop_num_idr_to_idr > 0 ){
		gBF_Encoder_H265_If_Param[stream_ch].gop_num_idr_to_idr = cfg_param->gop_num_idr_to_idr;	// new
	}
	else{
		BF_Debug_Print_Error(("Param Error. GOP num IDR to IDR=%d\n", cfg_param->gop_num_idr_to_idr ));
		return FJ_ERR_NG;
	}
	
	// Level
	switch( cfg_param->level ){
		case FJ_MOVIE_VIDEO_LEVEL_1 :
			gBF_Encoder_H265_If_Param[stream_ch].level = E_H265_IF_LEVEL_1;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_2 :
			gBF_Encoder_H265_If_Param[stream_ch].level = E_H265_IF_LEVEL_2;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_2_1 :
			gBF_Encoder_H265_If_Param[stream_ch].level = E_H265_IF_LEVEL_2_1;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_3 :
			gBF_Encoder_H265_If_Param[stream_ch].level = E_H265_IF_LEVEL_3;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_3_1 :
			gBF_Encoder_H265_If_Param[stream_ch].level = E_H265_IF_LEVEL_3_1;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_4 :
			gBF_Encoder_H265_If_Param[stream_ch].level = E_H265_IF_LEVEL_4;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_4_1 :
			gBF_Encoder_H265_If_Param[stream_ch].level = E_H265_IF_LEVEL_4_1;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_5 :
			gBF_Encoder_H265_If_Param[stream_ch].level = E_H265_IF_LEVEL_5;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_5_1 :
			gBF_Encoder_H265_If_Param[stream_ch].level = E_H265_IF_LEVEL_5_1;
			break;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		case FJ_MOVIE_VIDEO_LEVEL_5_2 :
			gBF_Encoder_H265_If_Param[stream_ch].level = E_H265_IF_LEVEL_5_2;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_6 :
			gBF_Encoder_H265_If_Param[stream_ch].level = E_H265_IF_LEVEL_6;
			break;
		case FJ_MOVIE_VIDEO_LEVEL_6_1 :
			gBF_Encoder_H265_If_Param[stream_ch].level = E_H265_IF_LEVEL_6_1;
			break;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
		default :
			BF_Debug_Print_Error(("Param Error. Level=%d\n", cfg_param->level ));
			return FJ_ERR_NG;
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	// Tier
	if( cfg_param->tier == FJ_MOVIE_VIDEO_TIER_MAIN ){
		gBF_Encoder_H265_If_Param[stream_ch].tier = E_H265_IF_TIER_MAIN;
	}
	else{
		gBF_Encoder_H265_If_Param[stream_ch].tier = E_H265_IF_TIER_HIGH;
	}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
	
	// Scan Mode
	gBF_Encoder_H265_If_Param[stream_ch].scan_mode = E_H265_IF_SCAN_MODE_PROGRESSIVE;	// Fixed
	
	// Flag to Insert hrd_parameters in VUI/BuffeingPeriodSEI/removal_delay in PictureTimingSEI.
	if( (cfg_param->buffer_ctrl_flag == FJ_MOVIE_ON) || (cfg_param->buffer_ctrl_flag == FJ_MOVIE_OFF) ){
		gBF_Encoder_H265_If_Param[stream_ch].buffer_ctrl_flag = cfg_param->buffer_ctrl_flag;
	}
	else{
		BF_Debug_Print_Error(("Param Error. Buffer Control flag=%d\n", cfg_param->buffer_ctrl_flag ));
		return FJ_ERR_NG;
	}
	
	// Flag to Pictstruct in PictureTiming SEI.
	if( (cfg_param->pictstruct_flag == FJ_MOVIE_ON) || (cfg_param->pictstruct_flag == FJ_MOVIE_OFF) ){
		gBF_Encoder_H265_If_Param[stream_ch].pictstruct_flag = cfg_param->pictstruct_flag;
	}
	else{
		BF_Debug_Print_Error(("Param Error. PictStruct flag=%d\n", cfg_param->pictstruct_flag ));
		return FJ_ERR_NG;
	}
	
	// Flag to Insert Filler Data
	if( (cfg_param->filler_flag == FJ_MOVIE_ON) || (cfg_param->filler_flag == FJ_MOVIE_OFF) ){
		gBF_Encoder_H265_If_Param[stream_ch].filler_flag = cfg_param->filler_flag;
	}
	else{
		BF_Debug_Print_Error(("Param Error. Filler flag=%d\n", cfg_param->filler_flag ));
		return FJ_ERR_NG;
	}
	
	// Flag to TimeLapse
	if( (cfg_param->time_lapse_flag == FJ_MOVIE_ON) || (cfg_param->time_lapse_flag == FJ_MOVIE_OFF) ){
		gBF_Encoder_H265_If_Param[stream_ch].time_lapse_flag			= cfg_param->time_lapse_flag;
	}
	else{
		BF_Debug_Print_Error(("Param Error. TimeLapse flag=%d\n", cfg_param->time_lapse_flag ));
		return FJ_ERR_NG;
	}
	
	// VUI
	gBF_Encoder_H265_If_Param[stream_ch].vui.video_signal_type_present_flag			= cfg_param->vui_param.video_signal_type_present_flag;
	gBF_Encoder_H265_If_Param[stream_ch].vui.video_format							= cfg_param->vui_param.video_format;
	gBF_Encoder_H265_If_Param[stream_ch].vui.video_full_range_flag					= cfg_param->vui_param.video_full_range_flag;
	gBF_Encoder_H265_If_Param[stream_ch].vui.colour_description_present_flag		= cfg_param->vui_param.colour_description_present_flag;	
	gBF_Encoder_H265_If_Param[stream_ch].vui.colour_primaries						= cfg_param->vui_param.colour_primaries;
	gBF_Encoder_H265_If_Param[stream_ch].vui.transfer_characteristics				= cfg_param->vui_param.transfer_characteristics;
	gBF_Encoder_H265_If_Param[stream_ch].vui.matrix_coefficients					= cfg_param->vui_param.matrix_coefficients;
	gBF_Encoder_H265_If_Param[stream_ch].vui.aspect_ratio_info_present_flag			= cfg_param->vui_param.aspect_ratio_info_present_flag;
	gBF_Encoder_H265_If_Param[stream_ch].vui.aspect_ratio_idc						= cfg_param->vui_param.aspect_ratio_idc;
	gBF_Encoder_H265_If_Param[stream_ch].vui.sar_width								= cfg_param->vui_param.sar_width;
	gBF_Encoder_H265_If_Param[stream_ch].vui.sar_height								= cfg_param->vui_param.sar_height;
	gBF_Encoder_H265_If_Param[stream_ch].vui.chroma_loc_info_present_flag			= cfg_param->vui_param.chroma_loc_info_present_flag;
	gBF_Encoder_H265_If_Param[stream_ch].vui.chroma_sample_loc_type_top_field		= cfg_param->vui_param.chroma_sample_loc_type_top_field;
	gBF_Encoder_H265_If_Param[stream_ch].vui.chroma_sample_loc_type_bottom_field	= cfg_param->vui_param.chroma_sample_loc_type_bottom_field;
	
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	if( cfg_param->vector_info_flag == FJ_MOVIE_ON ){
		gBF_Encoder_H265_If_Param[stream_ch].vector_info_flag = H265_ON;
	}
	else if( cfg_param->vector_info_flag == FJ_MOVIE_OFF ){
		gBF_Encoder_H265_If_Param[stream_ch].vector_info_flag = H265_OFF;
	}
	else{
		BF_Debug_Print_Error(("Param Error. Vector Flag=%d\n", cfg_param->vector_info_flag ));
		return FJ_ERR_NG;
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
	
	// Flag to Slice Encode Mode
	if( cfg_param->slice_encode_flag == FJ_MOVIE_ON ){
		gBF_Encoder_H265_If_Param[stream_ch].slice_encode_flag = H265_ON;
		gBF_Encoder_H265_If_Param[stream_ch].slice_int_line = cfg_param->slice_int_line;
	}
	else if( cfg_param->slice_encode_flag == FJ_MOVIE_OFF ){
		gBF_Encoder_H265_If_Param[stream_ch].slice_encode_flag = H265_OFF;
	}
	else{
		BF_Debug_Print_Error(("Param Error. Slice Encode Flag=%d\n", cfg_param->slice_encode_flag ));
		return FJ_ERR_NG;
	}
	
	// Flag to 1shot Encode Mode
	if( cfg_param->oneshot_encode_flag == FJ_MOVIE_ON ){
		gBF_Encoder_H265_If_Param[stream_ch].oneshot_encode_flag = H265_ON;
	}
	else if( cfg_param->slice_encode_flag == FJ_MOVIE_OFF ){
		gBF_Encoder_H265_If_Param[stream_ch].oneshot_encode_flag = H265_OFF;
	}
	else{
//		BF_Debug_Print_Error(("Param Error. 1shot Encode Flag=%d\n", cfg_param->oneshot_encode_flag ));
//		return FJ_ERR_NG;
	}
	
	gBF_Encoder_H265_If_Param[stream_ch].delay_notify_flag = cfg_param->delay_notify_flag;
	
	// Conflict check between one shot mode and time lapse mode.
	if( (gBF_Encoder_H265_If_Param[stream_ch].time_lapse_flag == H265_ON) && (gBF_Encoder_H265_If_Param[stream_ch].oneshot_encode_flag == H265_ON) ){
		BF_Debug_Print_Error(("Param Error. One shot mode and time lapse mode are enabled.\n" ));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_Movie_Video_Attr_Get_H265_Config( E_VIDEO_ENOCDE_ID video_enc_id, T_H265_IF_PARAM_RECORD* cfg_param )
{
	FJ_ERR_CODE ercd;
	E_H265_IF_STREAM_CH stream_ch;
	
	ercd = BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H265( video_enc_id, &stream_ch );
	if( ercd != FJ_ERR_OK ){
		return FJ_ERR_NG;
	}
	
	memcpy( (void*)cfg_param, (const void*)&gBF_Encoder_H265_If_Param[stream_ch], sizeof(T_H265_IF_PARAM_RECORD) );
	return FJ_ERR_OK;
}


//#pragma arm section code="BASEFW_LAST_CODE"
UCHAR BF_Movie_Video_Attr_Get_Enable_Flg( E_VIDEO_ENOCDE_ID video_enc_id )
{
	UINT8 stream_ch;
	
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
			BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, (E_H264_IF_STREAM_CH *)&stream_ch );
			return gBF_Encoder_H264_If_Param[stream_ch].enable_flg;
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H265( video_enc_id, (E_H265_IF_STREAM_CH *)&stream_ch );
			return gBF_Encoder_H265_If_Param[stream_ch].enable_flg;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Get_Enable_Flg Error.\n"));
			break;
	}
	
	return FJ_MOVIE_OFF;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Set_Enable_Flg( E_VIDEO_ENOCDE_ID video_enc_id, UCHAR enable_flag )
{
	UINT8 stream_ch;
	
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
			BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, (E_H264_IF_STREAM_CH *)&stream_ch );
			gBF_Encoder_H264_If_Param[stream_ch].enable_flg = enable_flag;
			break;
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H265( video_enc_id, (E_H265_IF_STREAM_CH *)&stream_ch );
			gBF_Encoder_H265_If_Param[stream_ch].enable_flg = enable_flag;
			break;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Get_Enable_Flg Error.\n"));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

//#pragma arm section code="BASEFW_LAST_CODE"
INT32 BF_Movie_Video_Attr_Get_Frame_Rate( E_VIDEO_ENOCDE_ID video_enc_id )
{
	UINT8 stream_ch;
	
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
			BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, (E_H264_IF_STREAM_CH *)&stream_ch );
			return gBF_Encoder_H264_If_Param[stream_ch].frame_rate;
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H265( video_enc_id, (E_H265_IF_STREAM_CH *)&stream_ch );
			return gBF_Encoder_H265_If_Param[stream_ch].frame_rate;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Get_Frame_Rate Error.\n"));
			break;
	}
	
	return 0;
}

//#pragma arm section code="BASEFW_LAST_CODE"
INT32 BF_Movie_Video_Attr_Get_Frame_Rate_Value( E_VIDEO_ENOCDE_ID video_enc_id )
{
	UINT8 stream_ch;
	
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
			BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, (E_H264_IF_STREAM_CH *)&stream_ch );
			return gBF_Encoder_H264_Framerate_Value[stream_ch];
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H265( video_enc_id, (E_H265_IF_STREAM_CH *)&stream_ch );
			return gBF_Encoder_H265_Framerate_Value[stream_ch];
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Get_Frame_Rate_Value Error.\n"));
			break;
	}
	
	return 0;
}

UCHAR BF_Movie_Video_Attr_Get_Timelapse_Flag( E_VIDEO_ENOCDE_ID video_enc_id )
{
	UINT8 stream_ch;
	
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H264_CH0 :
		case E_VIDEO_ENOCDE_ID_H264_CH1 :
		case E_VIDEO_ENOCDE_ID_H264_CH2 :
			BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( video_enc_id, (E_H264_IF_STREAM_CH *)&stream_ch );
			return gBF_Encoder_H264_If_Param[stream_ch].time_lapse_flag;
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H265( video_enc_id, (E_H265_IF_STREAM_CH *)&stream_ch );
			return gBF_Encoder_H265_If_Param[stream_ch].time_lapse_flag;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Get_Frame_Rate_Value Error.\n"));
			break;
	}
	
	return 0;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Set_Compression( E_VIDEO_ENOCDE_ID video_enc_id, BOOL compression )
{
	UINT8 stream_ch;
	
	switch( video_enc_id ){
		case E_VIDEO_ENOCDE_ID_H265_CH0 :
		case E_VIDEO_ENOCDE_ID_H265_CH1 :
			BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H265( video_enc_id, (E_H265_IF_STREAM_CH *)&stream_ch );
			if( compression ){
				gBF_Encoder_H265_If_Param[stream_ch].compression_enable = H265_ON;
			}
			else{
				gBF_Encoder_H265_If_Param[stream_ch].compression_enable = H265_OFF;
			}
			break;
		default :
			break;
	}
	
	return FJ_ERR_OK;
}

/*---------------*/
/* Video Decoder */
/*---------------*/
FJ_ERR_CODE BF_Movie_Video_Attr_Set_Decoder_Codec_Type( UINT8 codec_type )
{
	switch( codec_type ){
		case FJ_MOVIE_PLAY_CODEC_H264 :	// Fallthrough
		case FJ_MOVIE_PLAY_CODEC_H265 :
			gBF_Decoder_Valid_Codec_Type = codec_type;
			break;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_Decoder_Codec_Type : Codec Type Error. Codec Type=%d\n", codec_type ));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Get_Decoder_Codec_Type( UINT8* codec_type )
{
	*codec_type = gBF_Decoder_Valid_Codec_Type;
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Set_H264_Decoder_Config( T_FJ_MOVIE_PLAY_VIDEO_CONFIG_PARAM* video_config )
{
	UINT32	frame_rate;
	
	// Image Size
	switch( video_config->image_size ){
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2304 :
			gBF_Decoder_H264_If_Param.image_width			= 4096;
			gBF_Decoder_H264_If_Param.image_lines			= 2304;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 4096;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 2304;
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 1152;
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x900000;	// 4096*2304 = 0x900000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x480000;	// 4096*1152 = 0x480000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_4096_2304;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2160 :
			gBF_Decoder_H264_If_Param.image_width			= 4096;
			gBF_Decoder_H264_If_Param.image_lines			= 2160;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 4096;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 2176;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 1088;
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x880000;	// 4096*2176 = 0x880000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x440000;	// 4096*1088 = 0x440000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_4096_2160;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2048 :
			gBF_Decoder_H264_If_Param.image_width			= 4096;
			gBF_Decoder_H264_If_Param.image_lines			= 2048;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 4096;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 2048;
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 1024;
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x800000;	// 4096*2048 = 0x800000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x400000;	// 4096*1024 = 0x400000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_4096_2048;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_3840_2160 :
			gBF_Decoder_H264_If_Param.image_width			= 3840;
			gBF_Decoder_H264_If_Param.image_lines			= 2160;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 3840;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 2176;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 1088;
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x7F8000;	// 3840*2176 = 0x7F8000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x3FC000;	// 3840*1088 = 0x3FC000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_3840_2160;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_3000_3000 :
			gBF_Decoder_H264_If_Param.image_width			= 3000;
			gBF_Decoder_H264_If_Param.image_lines			= 3000;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 3000;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 3008; 	// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 1504;
			gBF_Decoder_H264_If_Param.image_bank_y_size 	= 0x89B200; // 3000*3008 = 0x89B200
			gBF_Decoder_H264_If_Param.image_bank_c_size 	= 0x44D900; // 3000*1504 = 0x44D900
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_3000_3000;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_2704_2028 :
			gBF_Decoder_H264_If_Param.image_width			= 2704;
			gBF_Decoder_H264_If_Param.image_lines			= 2028;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 2816;		// align 128
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 2048;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 1024;
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x580000;	// 2816*2048 = 0x580000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x2C0000;	// 2816*1024 = 0x2C0000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_2704_2028;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_2704_1520 :
			gBF_Decoder_H264_If_Param.image_width			= 2704;
			gBF_Decoder_H264_If_Param.image_lines			= 1520;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 2816;		// align 128
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 1536;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 768;
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x420000;	// 2816*1536 = 0x420000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x210000;	// 2816*768  = 0x210000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_2704_1520;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_2560_1440 :
			gBF_Decoder_H264_If_Param.image_width			= 2560;
			gBF_Decoder_H264_If_Param.image_lines			= 1440;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 2560;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 1440;
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 736;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x384000;	// 2560*1440 = 0x384000 -> 16KB align -> 0x384000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x1CC000;	// 2560*736  = 0x1CC000 -> 16KB align -> 0x1CC000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_2560_1440;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1440 :
			gBF_Decoder_H264_If_Param.image_width			= 1920;
			gBF_Decoder_H264_If_Param.image_lines			= 1440;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 1920;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 1440;
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 736;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x2A4000;	// 1920*1440 = 0x2A3000 -> 16KB align -> 0x2A4000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x15C000;	// 1920*736  = 0x159000 -> 16KB align -> 0x15C000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_1920_1440;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1080 :
			gBF_Decoder_H264_If_Param.image_width			= 1920;
			gBF_Decoder_H264_If_Param.image_lines			= 1080;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 1920;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 1088;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 544;
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x200000;	// 1920*1088 = 0x1FE000 -> 16KB align -> 0x200000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x100000;	// 1920*544  = 0xFF000  -> 16KB align -> 0x100000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_1920_1080;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_960 :
			gBF_Decoder_H264_If_Param.image_width			= 1920;
			gBF_Decoder_H264_If_Param.image_lines			= 960;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 1920;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 960;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x1C4000;	// 1920*960 = 0x1C2000 -> 16KB align -> 0x1C4000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0xE4000;	// 1920*480  = 0xE1000  -> 16KB align -> 0xE4000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_1920_960;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1440_1080 :
			gBF_Decoder_H264_If_Param.image_width			= 1440;
			gBF_Decoder_H264_If_Param.image_lines			= 1080;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 1536;		// align 128
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 1088;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 544;
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x198000;	// 1536*1088 = 0x198000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0xCC000;	// 1536*544  = 0xCC000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_1440_1080;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1440_720 :
			gBF_Decoder_H264_If_Param.image_width			= 1440;
			gBF_Decoder_H264_If_Param.image_lines			= 720;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 1536;		// align 128
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 1088;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 544;
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x198000;	// 1536*1088 = 0x198000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0xCC000;	// 1536*544  = 0xCC000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_1440_720;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_960 :
			gBF_Decoder_H264_If_Param.image_width			= 1280;
			gBF_Decoder_H264_If_Param.image_lines			= 960;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x12C000;	// 1280*960 = 0x12C000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x98000;	// 1280*480 = 0x96000 -> 16KB align -> 0x98000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_1280_960;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_720 :
			gBF_Decoder_H264_If_Param.image_width			= 1280;
			gBF_Decoder_H264_If_Param.image_lines			= 720;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 736;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 384;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0xE8000;	// 1280*736 = 0xE6000 -> 16KB align -> 0xE8000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x78000;	// 1280*384 = 0x78000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_1280_720;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_960_540 :
			gBF_Decoder_H264_If_Param.image_width			= 960;
			gBF_Decoder_H264_If_Param.image_lines			= 540;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 960;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 544;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 288;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size 	= 0x80000;	// 960*544 = 0x7F800 -> 16KB align -> 0x80000
			gBF_Decoder_H264_If_Param.image_bank_c_size 	= 0x44000;	// 960*288 = 0x43800 -> 16KB align -> 0x44000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_960_540;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_960_480 :
			gBF_Decoder_H264_If_Param.image_width			= 960;
			gBF_Decoder_H264_If_Param.image_lines			= 480;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 960;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 480;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 256;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size 	= 0x80000;	// 960*480 = 0x70800 -> 16KB align -> 0x80000
			gBF_Decoder_H264_If_Param.image_bank_c_size 	= 0x3C000;	// 960*256 = 0x3C000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_960_540;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_1024 :
			gBF_Decoder_H264_If_Param.image_width			= 1280;
			gBF_Decoder_H264_If_Param.image_lines			= 1024;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 1280;		// align 128
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 1024;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 512;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x140000;	// 1280*1024 = 0x140000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0xA0000;	// 1280*512 = 0xA0000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_1280_1024;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_768 :
			gBF_Decoder_H264_If_Param.image_width			= 1280;
			gBF_Decoder_H264_If_Param.image_lines			= 768;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 1280;		// align 128
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 768;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 384;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0xF0000;	// 1280*768 = 0xF0000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x78000;	// 1280*384 = 0x78000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_1280_768;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1024_768 :
			gBF_Decoder_H264_If_Param.image_width			= 1024;
			gBF_Decoder_H264_If_Param.image_lines			= 768;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 1024;		// align 128
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 768;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 384;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0xC0000;	// 1024*768 = 0xC0000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x60000;	// 1024*384 = 0x60000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_1024_768;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_720_576 :
			gBF_Decoder_H264_If_Param.image_width			= 720;
			gBF_Decoder_H264_If_Param.image_lines			= 576;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 768;		// align 128
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 576;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 288;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x6C000;	// 768*576 = 0x6C000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x38000;	// 768*288 = 0x36000 > 16KB align -> 0x38000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_720_576;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_864_480 :
			gBF_Decoder_H264_If_Param.image_width			= 864;
			gBF_Decoder_H264_If_Param.image_lines			= 480;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 896;		// align 128
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 480;
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 256;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x6C000;	// 896*480 = 0x69000 -> 16KB align -> 0x6C000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x38000;	// 896*256 = 0x38000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_864_480;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_848_480 :
			gBF_Decoder_H264_If_Param.image_width			= 848;
			gBF_Decoder_H264_If_Param.image_lines			= 480;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 896;		// align 128
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 480;
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 256;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x6C000;	// 896*480 = 0x69000 -> 16KB align -> 0x6C000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x38000;	// 896*256 = 0x38000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_848_480;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_800_600 :
			gBF_Decoder_H264_If_Param.image_width			= 800;
			gBF_Decoder_H264_If_Param.image_lines			= 600;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 896;		// align 128
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 608;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 320;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x88000;	// 896*608 = 0x69000 -> 16KB align -> 0x88000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x46000;	// 896*320 = 0x38000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_800_600;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_720_480 :
			gBF_Decoder_H264_If_Param.image_width			= 720;
			gBF_Decoder_H264_If_Param.image_lines			= 480;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 768;		// align 128
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 480;
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 256;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x5C000;	// 768*480 = 0x5A000 -> 16KB align -> 0x5C000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x30000;	// 768*256 = 0x30000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_720_480;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_480 :
			gBF_Decoder_H264_If_Param.image_width			= 640;
			gBF_Decoder_H264_If_Param.image_lines			= 480;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 640;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 480;
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 256;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x4C000;	// 640*480 = 0x4B000 -> 16KB align -> 0x4C000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x28000;	// 640*256 = 0x28000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_640_480;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_360 :
			gBF_Decoder_H264_If_Param.image_width			= 640;
			gBF_Decoder_H264_If_Param.image_lines			= 360;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 640;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 384;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 192;
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x3C000;	// 640*384 = 0x3C000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x20000;	// 640*192 = 0x1E000 -> 16KB align -> 0x20000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_640_360;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_480_270 :
			gBF_Decoder_H264_If_Param.image_width			= 480;
			gBF_Decoder_H264_If_Param.image_lines			= 270;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 480;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 288;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 160;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size 	= 0x24000;	// 480*288 = 0x21C00 -> 16KB align -> 0x24000
			gBF_Decoder_H264_If_Param.image_bank_c_size 	= 0x14000;	// 480*160 = 0x12C00 -> 16KB align -> 0x14000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_480_270;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_432_240 :
			gBF_Decoder_H264_If_Param.image_width			= 432;
			gBF_Decoder_H264_If_Param.image_lines			= 240;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 512;		// align 128
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 256;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 128;
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x20000;	// 512*256 = 0x20000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0x10000;	// 512*128 = 0x10000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_432_240;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_320_240 :
			gBF_Decoder_H264_If_Param.image_width			= 320;
			gBF_Decoder_H264_If_Param.image_lines			= 240;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 384;		// align 128
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 256;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 128;
			gBF_Decoder_H264_If_Param.image_bank_y_size		= 0x18000;	// 384*256 = 0x18000
			gBF_Decoder_H264_If_Param.image_bank_c_size		= 0xC000;	// 384*128 = 0xC000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_320_240;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1600_896 :
			gBF_Decoder_H264_If_Param.image_width			= 1600;
			gBF_Decoder_H264_If_Param.image_lines			= 896;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 1600;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 896;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 448;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size 	= 0x160000;	// 1600*896 = 0x15E000 -> 16KB align -> 0x160000
			gBF_Decoder_H264_If_Param.image_bank_c_size 	= 0xB0000;	// 1600*448 = 0xAF000 -> 16KB align -> 0xB0000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_1600_896;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1024_576 :
			gBF_Decoder_H264_If_Param.image_width			= 1024;
			gBF_Decoder_H264_If_Param.image_lines			= 576;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 1024;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 576;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 288;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size 	= 0x90000;	// 1024*576 = 0x90000 -> 16KB align -> 0x90000
			gBF_Decoder_H264_If_Param.image_bank_c_size 	= 0x48000;	// 1024*288 = 0x48000 -> 16KB align -> 0x48000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_1024_576;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_800_448 :
			gBF_Decoder_H264_If_Param.image_width			= 800;
			gBF_Decoder_H264_If_Param.image_lines			= 448;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 800;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 448;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 224;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size 	= 0x58000;	// 800*448 = 0x57800 -> 16KB align -> 0x58000
			gBF_Decoder_H264_If_Param.image_bank_c_size 	= 0x2C000;	// 800*224 = 0x2BC00 -> 16KB align -> 0x2C000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_800_448;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_352_288 :
			gBF_Decoder_H264_If_Param.image_width			= 352;
			gBF_Decoder_H264_If_Param.image_lines			= 288;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 352;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 288;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 160;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size 	= 0x1C000;	// 352*288 = 0x18C00 -> 16KB align -> 0x1C000
			gBF_Decoder_H264_If_Param.image_bank_c_size 	= 0x10000;	// 352*160 = 0xDC00 -> 16KB align -> 0x10000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_352_288;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_2592_1944 :
			gBF_Decoder_H264_If_Param.image_width			= 2592;
			gBF_Decoder_H264_If_Param.image_lines			= 1944;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 2592;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 1952;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 992;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size 	= 0x4D4000;	// 2592*1952 = 0x4D3400 -> 16KB align -> 0x4D4000
			gBF_Decoder_H264_If_Param.image_bank_c_size 	= 0x274000;	// 2592*992 = 0x273C00 -> 16KB align -> 0x274000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_2592_1944;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_2304_1296 :
			gBF_Decoder_H264_If_Param.image_width			= 2304;
			gBF_Decoder_H264_If_Param.image_lines			= 1296;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 2304;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 1312;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 672;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size 	= 0x2E4000;	// 2304*1312 = 0x2E2000 -> 16KB align -> 0x2E4000
			gBF_Decoder_H264_If_Param.image_bank_c_size 	= 0x17C000;	// 2304*672 = 0x17A000 -> 16KB align -> 0x17C000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_2304_1296;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1280 :
			gBF_Decoder_H264_If_Param.image_width			= 1920;
			gBF_Decoder_H264_If_Param.image_lines			= 1280;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 1920;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 1280; 	// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 640;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size 	= 0x258000; // 1920*1280 = 0x258000 -> 16KB align
			gBF_Decoder_H264_If_Param.image_bank_c_size 	= 0x12C000; // 1920*640 = 0x12C000 -> 16KB align
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_1920_1280;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_512 :
			gBF_Decoder_H264_If_Param.image_width			= 640;
			gBF_Decoder_H264_If_Param.image_lines			= 512;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 640;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 512; 		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 256;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size 	= 0x50000;  // 640*512 = 0x50000 -> 16KB align
			gBF_Decoder_H264_If_Param.image_bank_c_size 	= 0x28000;  // 640*256 = 0x28000 -> 16KB align
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_640_512;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_384_288 :
			gBF_Decoder_H264_If_Param.image_width			= 384;
			gBF_Decoder_H264_If_Param.image_lines			= 288;
			gBF_Decoder_H264_If_Param.image_bank_y_width	= 384;
			gBF_Decoder_H264_If_Param.image_bank_y_lines	= 288; 		// align 32
			gBF_Decoder_H264_If_Param.image_bank_c_lines	= 160;		// align 32
			gBF_Decoder_H264_If_Param.image_bank_y_size 	= 0x1C000;  // 384*288 = 0x1B000 -> 16KB align -> 0x1C000
			gBF_Decoder_H264_If_Param.image_bank_c_size 	= 0x10000;  // 384*160 = 0xF000 -> 16KB align -> 0x10000
			gBF_Decoder_H264_If_Param.image_size			= E_H264_IF_IMAGE_SIZE_384_288;
			break;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H264_Decoder_Config : Param Error. Image Size=%d\n", video_config->image_size ));
			return FJ_ERR_NG;
	}
	
	// Scan Mode
	switch( video_config->scan_mode ){
		case FJ_MOVIE_PLAY_VIDEO_SCAN_MODE_PROGRESSIVE :
			gBF_Decoder_H264_If_Param.scan_mode		= E_H264_IF_SCAN_MODE_PROGRESSIVE;
			break;
		case FJ_MOVIE_PLAY_VIDEO_SCAN_MODE_INTERLACE :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H264_Decoder_Config : Interlace is not supported. Scan Mode=%d\n", video_config->scan_mode ));
			return FJ_ERR_NG;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H264_Decoder_Config : Param Error. Scan Mode=%d\n", video_config->scan_mode ));
			return FJ_ERR_NG;
	}
	
	// Conceal Error
	switch( video_config->conceal_error ){
		case FJ_MOVIE_PLAY_VIDEO_CONCEAL_ERROR_STOP :
			gBF_Decoder_H264_If_Param.conceal_error	= E_H264_IF_CONCEAL_ERROR_STOP;
			break;
		case FJ_MOVIE_PLAY_VIDEO_CONCEAL_ERROR_SKIP :
			gBF_Decoder_H264_If_Param.conceal_error	= E_H264_IF_CONCEAL_ERROR_SKIP;
			break;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H264_Decoder_Config : Param Error. Conceal Error=%d\n", video_config->conceal_error ));
			return FJ_ERR_NG;
	}
	
	// Time Scale
	if( video_config->time_scale != 0 ){
		gBF_Decoder_H264_If_Param.time_scale	= video_config->time_scale;
	}
	else{
		BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H264_Decoder_Config : Param Error. Time Scale=%d\n", video_config->time_scale ));
		return FJ_ERR_NG;
	}
	
	// Time Delta
	if( video_config->time_delta != 0 ){
		gBF_Decoder_H264_If_Param.time_delta	= video_config->time_delta;
	}
	else{
		BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H264_Decoder_Config : Param Error. Time Delta=%d\n", video_config->time_delta ));
		return FJ_ERR_NG;
	}
	
	// Frame Rate
	frame_rate = (video_config->time_scale*100)/video_config->time_delta;
	switch( frame_rate ){
		case 23976:	// 239.76fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_239_76;
			break;
		case 11988:	// 119.88fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_119_88;
			break;
		case 5994:	// 59.94fps (NTSC)
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_59_94;
			break;
		case 4795:	// 47.95fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_47_95;
			break;
		case 2997:	// 29.97fps (NTSC)
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_29_97;
			break;
		case 2397:	// 23.97fps (NTSC)
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_23_97;
			break;
		case 1498:	// 14.98fps (NTSC)
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_14_98;
			break;
		case 999:	// 9.99fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_9_99;
			break;
		case 499:	// 4.99fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_4_99;
			break;
		case 299:	// 2.99fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_2_99;
			break;
		case 199:	// 1.99fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_1_99;
			break;
		case 99:	// 0.99fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_0_99;
			break;
		
		case 24000:	// 240fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_240_0;
			break;
		case 12000:	// 120fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_120_0;
			break;
		case 6000:	// 60fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_60_0;
			break;
		case 5000:	// 50fps (PAL)
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_50_0;
			break;
		case 4800:	// 48fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_48_0;
			break;
		case 3000:	// 30fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_30_0;
			break;
		case 2500:	// 25fps (PAL)
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_25_0;
			break;
		case 2400:	// 24fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_24_0;
			break;
		case 1500:	// 15fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_15_0;
			break;
		case 1250:	// 12.5fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_12_5;
			break;
		case 1000:	// 10fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_10_0;
			break;
		case 500:	// 5fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_5_0;
			break;
		case 300:	// 3fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_3_0;
			break;
		case 200:	// 2fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_2_0;
			break;
		case 100:	// 1fps
			gBF_Decoder_H264_If_Param.frame_rate = E_H264_IF_FRAME_RATE_1_0;
			break;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H264_Decoder_Config : Param Error. FrameRate=time_scale(%d) time_delta(%d)\n", video_config->time_scale, video_config->time_delta ));
			return FJ_ERR_NG;
	}
	
	// Trick
	gBF_Decoder_H264_If_Param.trick	= video_config->trick;
	
#if 0	// Force Trick Limit
	BF_Debug_Print_Warning(("\n"));
	BF_Debug_Print_Warning(("BF_Movie_Video_Attr_Set_H264_Decoder_Config : Force trick limit.\n"));
	BF_Debug_Print_Warning(("\n"));
	gBF_Decoder_H264_If_Param.trick	= FALSE;
#endif
	
	// compatiblity_mode
	switch( video_config->compatibility_mode ){
		case FJ_MOVIE_PLAY_ON :
			// compatibility_mode = ON -> self_rec = OFF
			gBF_Decoder_H264_If_Param.self_rec	= H264_OFF;
			break;
		case FJ_MOVIE_PLAY_OFF :
			// compatibility_mode = OFF -> self_rec = ON
			gBF_Decoder_H264_If_Param.self_rec	= H264_ON;
			break;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H264_Decoder_Config : Param Error. compatibility_mode=%d\n", video_config->compatibility_mode ));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Get_H264_Decoder_Config( T_H264_IF_DECODER_PARAM* video_config )
{
	*video_config = gBF_Decoder_H264_If_Param;
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Set_H265_Decoder_Config( T_FJ_MOVIE_PLAY_VIDEO_CONFIG_PARAM* video_config )
{
	UINT32	frame_rate;
	
	// Image Size
	switch( video_config->image_size ){
// --- REMOVE_60FPS BEGIN ---
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_4000_3000 :
			gBF_Decoder_H265_If_Param.image_width			= 4000;
			gBF_Decoder_H265_If_Param.image_lines			= 3000;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 4096;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 3008;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 1504;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0xBC0000;	// SR_DECODE_MODE_8 (for 4K3K)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x5E0000;	// SR_DECODE_MODE_8 (for 4K3K)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_4000_3000;
			break;
// --- REMOVE_60FPS END ---
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2160 :
			gBF_Decoder_H265_If_Param.image_width			= 4096;
			gBF_Decoder_H265_If_Param.image_lines			= 2160;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 4096;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 2176;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 1088;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x880000;	// SR_DECODE_MODE_6 (for 4K2K Cinema)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x440000;	// SR_DECODE_MODE_6 (for 4K2K Cinema)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_4096_2160;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_3840_2160 :
			gBF_Decoder_H265_If_Param.image_width			= 3840;
			gBF_Decoder_H265_If_Param.image_lines			= 2160;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 3840;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 2176;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 1088;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x7F8000;	// SR_DECODE_MODE_5 (for 4K2K)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x3FC000;	// SR_DECODE_MODE_5 (for 4K2K)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_3840_2160;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_3840_1920 :
			gBF_Decoder_H265_If_Param.image_width			= 3840;
			gBF_Decoder_H265_If_Param.image_lines			= 1920;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 4096;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 2176;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 1088;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x880000;	// SR_DECODE_MODE_2 (for 4K2K)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x440000;	// SR_DECODE_MODE_2 (for 4K2K)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_3840_1920;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_3000_3000 :
			gBF_Decoder_H265_If_Param.image_width			= 3000;
			gBF_Decoder_H265_If_Param.image_lines			= 3000;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 3072;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 3008;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 1504;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x8D0000;	// SR_DECODE_MODE_7 (for 3K)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x468000;	// SR_DECODE_MODE_7 (for 3K)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_3000_3000;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_2704_2028 :
			gBF_Decoder_H265_If_Param.image_width			= 2704;
			gBF_Decoder_H265_If_Param.image_lines			= 2028;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 2816;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 2048;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 1024;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x580000;	// SR_DECODE_MODE_4 (for 2.7K(4:3))
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x2C0000;	// SR_DECODE_MODE_4 (for 2.7K(4:3))
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_2704_2028;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_2704_1520 :
			gBF_Decoder_H265_If_Param.image_width			= 2704;
			gBF_Decoder_H265_If_Param.image_lines			= 1520;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 2816;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 1536;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 768;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x420000;	// SR_DECODE_MODE_3 (for 2.7K(16:9))
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x210000;	// SR_DECODE_MODE_3 (for 2.7K(16:9))
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_2704_1520;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_2560_1920 :
			gBF_Decoder_H265_If_Param.image_width			= 2560;
			gBF_Decoder_H265_If_Param.image_lines			= 1920;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 2816;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 2048;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 1024;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x580000;	// SR_DECODE_MODE_4 (for 2.7K(4:3))
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x2C0000;	// SR_DECODE_MODE_4 (for 2.7K(4:3))
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_2560_1920;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_2560_1440 :
			gBF_Decoder_H265_If_Param.image_width			= 2560;
			gBF_Decoder_H265_If_Param.image_lines			= 1440;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 2560;		// align 128
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 1440;		// align 32
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 736;		// align 32
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x384000;	// 2560*1440 = 0x384000
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x1CC000;	// 2560*736  = 0x1CC000
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_2560_1440;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1440 :
			gBF_Decoder_H265_If_Param.image_width			= 1920;
			gBF_Decoder_H265_If_Param.image_lines			= 1440;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1920;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 1536;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 768;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x2D0000;	// SR_DECODE_MODE_2 (for HD)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x168000;	// SR_DECODE_MODE_2 (for HD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_1920_1440;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1080 :
			gBF_Decoder_H265_If_Param.image_width			= 1920;
			gBF_Decoder_H265_If_Param.image_lines			= 1080;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1920;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 1088;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 544;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x200000;	// SR_DECODE_MODE_1 (for 1080p)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x100000;	// SR_DECODE_MODE_1 (for 1080p)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_1920_1080;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_960 :
			gBF_Decoder_H265_If_Param.image_width			= 1920;
			gBF_Decoder_H265_If_Param.image_lines			= 960;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1920;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x1C4000;	// 1920*960 = 0x1C2000 -> 16KB align -> 0x1C4000
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0xE4000;	// 1920*480  = 0xE1000  -> 16KB align -> 0xE4000
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_1920_960;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1504_1504 :
			gBF_Decoder_H265_If_Param.image_width			= 1504;
			gBF_Decoder_H265_If_Param.image_lines			= 1504;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1920;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 1536;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 768;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x2D0000;	// SR_DECODE_MODE_2 (for HD)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x168000;	// SR_DECODE_MODE_2 (for HD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_1504_1504;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1440_1080 :
			gBF_Decoder_H265_If_Param.image_width			= 1440;
			gBF_Decoder_H265_If_Param.image_lines			= 1080;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1920;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 1088;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 544;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x200000;	// SR_DECODE_MODE_1 (for 1080p)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x100000;	// SR_DECODE_MODE_1 (for 1080p)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_1440_1080;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_1024 :
			gBF_Decoder_H265_If_Param.image_width			= 1280;
			gBF_Decoder_H265_If_Param.image_lines			= 1024;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1920;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 1088;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 544;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x200000;	// SR_DECODE_MODE_1 (for 1080p)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x100000;	// SR_DECODE_MODE_1 (for 1080p)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_1280_1024;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1440_720 :
			gBF_Decoder_H265_If_Param.image_width			= 1440;
			gBF_Decoder_H265_If_Param.image_lines			= 720;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1920;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 1088;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 544;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x200000;	// SR_DECODE_MODE_1 (for 1080p)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x100000;	// SR_DECODE_MODE_1 (for 1080p)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_1440_720;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_960 :
			gBF_Decoder_H265_If_Param.image_width			= 1280;
			gBF_Decoder_H265_If_Param.image_lines			= 960;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x12C000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_1280_960;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_720 :
			gBF_Decoder_H265_If_Param.image_width			= 1280;
			gBF_Decoder_H265_If_Param.image_lines			= 720;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x12C000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_1280_720;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_960_540 :
			gBF_Decoder_H265_If_Param.image_width			= 960;
			gBF_Decoder_H265_If_Param.image_lines			= 540;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size 	= 0x12C000; // SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size 	= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_960_540;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_960_480 :
			gBF_Decoder_H265_If_Param.image_width			= 960;
			gBF_Decoder_H265_If_Param.image_lines			= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size 	= 0x12C000; // SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size 	= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_960_480;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_864_480 :
			gBF_Decoder_H265_If_Param.image_width			= 864;
			gBF_Decoder_H265_If_Param.image_lines			= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x12C000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_864_480;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_848_480 :
			gBF_Decoder_H265_If_Param.image_width			= 848;
			gBF_Decoder_H265_If_Param.image_lines			= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x12C000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_848_480;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_800_600 :
			gBF_Decoder_H265_If_Param.image_width			= 800;
			gBF_Decoder_H265_If_Param.image_lines			= 600;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x12C000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_800_600;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_720_576 :
			gBF_Decoder_H265_If_Param.image_width			= 720;
			gBF_Decoder_H265_If_Param.image_lines			= 576;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x12C000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_720_576;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_720_480 :
			gBF_Decoder_H265_If_Param.image_width			= 720;
			gBF_Decoder_H265_If_Param.image_lines			= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x12C000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_720_480;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1024_768 :
			gBF_Decoder_H265_If_Param.image_width			= 1024;
			gBF_Decoder_H265_If_Param.image_lines			= 768;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x12C000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_1024_768;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_768 :
			gBF_Decoder_H265_If_Param.image_width			= 1280;
			gBF_Decoder_H265_If_Param.image_lines			= 768;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x12C000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_1280_768;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_480 :
			gBF_Decoder_H265_If_Param.image_width			= 640;
			gBF_Decoder_H265_If_Param.image_lines			= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x12C000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_640_480;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_360 :
			gBF_Decoder_H265_If_Param.image_width			= 640;
			gBF_Decoder_H265_If_Param.image_lines			= 360;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size 	= 0x12C000; // SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size 	= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_640_360;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_480_270 :
			gBF_Decoder_H265_If_Param.image_width			= 480;
			gBF_Decoder_H265_If_Param.image_lines			= 270;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size 	= 0x12C000; // SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size 	= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_480_270;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1600_896 :
			gBF_Decoder_H265_If_Param.image_width			= 1600;
			gBF_Decoder_H265_If_Param.image_lines			= 896;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1920;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 1088;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 544;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x200000;	// SR_DECODE_MODE_1 (for 1080p)
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x100000;	// SR_DECODE_MODE_1 (for 1080p)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_1600_896;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1024_576 :
			gBF_Decoder_H265_If_Param.image_width			= 1024;
			gBF_Decoder_H265_If_Param.image_lines			= 576;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size 	= 0x12C000; // SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size 	= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_1024_576;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_800_448 :
			gBF_Decoder_H265_If_Param.image_width			= 800;
			gBF_Decoder_H265_If_Param.image_lines			= 448;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size 	= 0x12C000; // SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size 	= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_800_448;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_352_288 :
			gBF_Decoder_H265_If_Param.image_width			= 352;
			gBF_Decoder_H265_If_Param.image_lines			= 288;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size 	= 0x12C000; // SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size 	= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_352_288;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_2592_1944 :
			gBF_Decoder_H265_If_Param.image_width			= 2592;
			gBF_Decoder_H265_If_Param.image_lines			= 1944;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 2816;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 2048;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 1024;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x580000;	// SR_DECODE_MODE_4 (for 2.7K(4:3))
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x2C0000;	// SR_DECODE_MODE_4 (for 2.7K(4:3))
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_2592_1944;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_2304_1296 :
			gBF_Decoder_H265_If_Param.image_width			= 2304;
			gBF_Decoder_H265_If_Param.image_lines			= 1296;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 2816;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 2048;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 1024;
			gBF_Decoder_H265_If_Param.image_bank_y_size		= 0x580000;	// SR_DECODE_MODE_4 (for 2.7K(4:3))
			gBF_Decoder_H265_If_Param.image_bank_c_size		= 0x2C0000;	// SR_DECODE_MODE_4 (for 2.7K(4:3))
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_2304_1296;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_432_240 :
			gBF_Decoder_H265_If_Param.image_width			= 432;
			gBF_Decoder_H265_If_Param.image_lines			= 240;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size 	= 0x12C000; // SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size 	= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_432_240;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_320_240 :
			gBF_Decoder_H265_If_Param.image_width			= 320;
			gBF_Decoder_H265_If_Param.image_lines			= 240;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size 	= 0x12C000; // SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size 	= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_320_240;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1280 :
			gBF_Decoder_H265_If_Param.image_width			= 1920;
			gBF_Decoder_H265_If_Param.image_lines			= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 2048; 	// align 128
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 1536; 	// align 32
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 768;		// align 32
			gBF_Decoder_H265_If_Param.image_bank_y_size 	= 0x300000; // 2048*1536 = 0x384000
			gBF_Decoder_H265_If_Param.image_bank_c_size 	= 0x180000; // 2048*768  = 0x1CC000
			gBF_Decoder_H265_If_Param.image_size			= E_FJ_MOVIE_VIDEO_SIZE_1920_1280;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_512 :
			gBF_Decoder_H265_If_Param.image_width			= 640;
			gBF_Decoder_H265_If_Param.image_lines			= 512;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size 	= 0x12C000; // SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size 	= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_640_512;
			break;
		case E_FJ_MOVIE_PLAY_VIDEO_SIZE_384_288 :
			gBF_Decoder_H265_If_Param.image_width			= 384;
			gBF_Decoder_H265_If_Param.image_lines			= 288;
			gBF_Decoder_H265_If_Param.image_bank_y_width	= 1280;
			gBF_Decoder_H265_If_Param.image_bank_y_lines	= 960;
			gBF_Decoder_H265_If_Param.image_bank_c_lines	= 480;
			gBF_Decoder_H265_If_Param.image_bank_y_size 	= 0x12C000; // SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_bank_c_size 	= 0x98000;	// SR_DECODE_MODE_0 (for NHD)
			gBF_Decoder_H265_If_Param.image_size			= E_H265_IF_IMAGE_SIZE_384_288;
			break;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H265_Decoder_Config : Param Error. Image Size=%d\n", video_config->image_size ));
			return FJ_ERR_NG;
	}
	
	// Scan Mode
	switch( video_config->scan_mode ){
		case FJ_MOVIE_PLAY_VIDEO_SCAN_MODE_PROGRESSIVE :
			gBF_Decoder_H265_If_Param.scan_mode		= E_H265_IF_SCAN_MODE_PROGRESSIVE;
			break;
		case FJ_MOVIE_PLAY_VIDEO_SCAN_MODE_INTERLACE :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H265_Decoder_Config : Interlace is not supported. Scan Mode=%d\n", video_config->scan_mode ));
			return FJ_ERR_NG;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H265_Decoder_Config : Param Error. Scan Mode=%d\n", video_config->scan_mode ));
			return FJ_ERR_NG;
	}
	
	// Conceal Error
	switch( video_config->conceal_error ){
		case FJ_MOVIE_PLAY_VIDEO_CONCEAL_ERROR_STOP :
			gBF_Decoder_H265_If_Param.conceal_error	= E_H265_IF_CONCEAL_ERROR_STOP;
			break;
		case FJ_MOVIE_PLAY_VIDEO_CONCEAL_ERROR_SKIP :
			gBF_Decoder_H265_If_Param.conceal_error	= E_H265_IF_CONCEAL_ERROR_SKIP;
			break;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H265_Decoder_Config : Param Error. Conceal Error=%d\n", video_config->conceal_error ));
			return FJ_ERR_NG;
	}
	
	// Time Scale
	if( video_config->time_scale != 0 ){
		gBF_Decoder_H265_If_Param.time_scale	= video_config->time_scale;
	}
	else{
		BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H265_Decoder_Config : Param Error. Time Scale=%d\n", video_config->time_scale ));
		return FJ_ERR_NG;
	}
	
	// Time Delta
	if( video_config->time_delta != 0 ){
		gBF_Decoder_H265_If_Param.time_delta	= video_config->time_delta;
	}
	else{
		BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H265_Decoder_Config : Param Error. Time Delta=%d\n", video_config->time_delta ));
		return FJ_ERR_NG;
	}
	
	// Frame Rate
	frame_rate = (video_config->time_scale*100)/video_config->time_delta;
	switch( frame_rate ){
		case 23976:	// 239.76fps
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_239_76;
			break;
		case 11988:	// 119.88fps
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_119_88;
			break;
		case 5994:	// 59.94fps (NTSC)
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_59_94;
			break;
		case 2997:	// 29.97fps (NTSC)
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_29_97;
			break;
		case 2397:
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_23_97;
			break;
		case 1498:	// 14.98fps (NTSC)
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_14_98;
			break;
		case 24000:	// 240fps
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_240_0;
			break;
		case 12000:	// 120fps
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_120_0;
			break;
		case 7300:
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_73_0;
			break;
		case 7200:
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_72_0;
			break;
		case 7000:
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_70_0;
			break;
		case 6000:	// 60fps
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_60_0;
			break;
		case 5000:
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_50_0;
			break;
		case 3000:	// 30fps
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_30_0;
			break;
		case 2900:
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_29_0;
			break;
		case 2500:
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_25_0;
			break;
		case 2400:
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_24_0;
			break;
		case 1500:	// 15fps
			gBF_Decoder_H265_If_Param.frame_rate = E_H265_IF_FRAME_RATE_15_0;
			break;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H265_Decoder_Config : Param Error. FrameRate=time_scale(%d) time_delta(%d)\n", video_config->time_scale, video_config->time_delta ));
			return FJ_ERR_NG;
	}
	
	// Trick
	gBF_Decoder_H265_If_Param.trick	= video_config->trick;
	
#if 0	// Force Trick Limit
	BF_Debug_Print_Warning(("\n"));
	BF_Debug_Print_Warning(("BF_Movie_Video_Attr_Set_H265_Decoder_Config : Force trick limit.\n"));
	BF_Debug_Print_Warning(("\n"));
	gBF_Decoder_H265_If_Param.trick	= FALSE;
#endif
	
	// compatiblity_mode
	switch( video_config->compatibility_mode ){
		case FJ_MOVIE_PLAY_ON :
			// compatibility_mode = ON -> self_rec = OFF
			gBF_Decoder_H265_If_Param.self_rec	= H265_OFF;
			break;
		case FJ_MOVIE_PLAY_OFF :
			// compatibility_mode = OFF -> self_rec = ON
			gBF_Decoder_H265_If_Param.self_rec	= H265_ON;
			break;
		default :
			BF_Debug_Print_Error(("BF_Movie_Video_Attr_Set_H265_Decoder_Config : Param Error. compatibility_mode=%d\n", video_config->compatibility_mode ));
			return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Get_H265_Decoder_Config( T_H265_IF_DECODER_PARAM* video_config )
{
	*video_config = gBF_Decoder_H265_If_Param;
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Set_Decoder_Play_Start_Pts( ULLONG start_pts )
{
	gBF_Decoder_Play_Start_Pts = start_pts;
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Get_Decoder_Play_Start_Pts( ULLONG* start_pts )
{
	*start_pts = gBF_Decoder_Play_Start_Pts;
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Set_Decoder_Play_Speed( E_FJ_MOVIE_PLAY_SPEED speed )
{
	gBF_Decoder_Play_Speed = speed;
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Get_Decoder_Play_Speed( E_FJ_MOVIE_PLAY_SPEED* speed )
{
	*speed = gBF_Decoder_Play_Speed;
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Set_Decoder_Play_Direction( E_FJ_MOVIE_PLAY_DIRECTION direction )
{
	gBF_Decoder_Play_Direction = direction;
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Get_Decoder_Play_Direction( E_FJ_MOVIE_PLAY_DIRECTION* direction )
{
	*direction = gBF_Decoder_Play_Direction;
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Set_Decoder_Play_Pause( BOOL pause )
{
	gBF_Decoder_Play_Pause = pause;
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Movie_Video_Attr_Get_Decoder_Play_Pause( BOOL* pause )
{
	*pause = gBF_Decoder_Play_Pause;
	
	return FJ_ERR_OK;
}

