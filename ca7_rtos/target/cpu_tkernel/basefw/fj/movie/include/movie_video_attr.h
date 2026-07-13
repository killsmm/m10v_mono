/**
 * @file		movie_video_attr.h
 * @brief		Movie Attribute Control Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_VIDEO_ATTR_H
#define _FJ_VIDEO_ATTR_H

#include "movie_video.h"
#include "h264_if.h"
#include "h265_if.h"

/******************************************************************************
 Fixed Define
******************************************************************************/
/******************************************************************************
 Structure
******************************************************************************/
/******************************************************************************
 Enumeration
******************************************************************************/
/******************************************************************************
 Global data
******************************************************************************/

/******************************************************************************
 Function
******************************************************************************/
FJ_ERR_CODE BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H264( E_VIDEO_ENOCDE_ID video_enc_id, E_H264_IF_STREAM_CH* stream_ch );
FJ_ERR_CODE BF_Movie_Video_Attr_Change_StreamCH_to_EncodeID_H264( E_H264_IF_STREAM_CH stream_ch, E_VIDEO_ENOCDE_ID* video_enc_id );
FJ_ERR_CODE BF_Movie_Video_Attr_Change_EncodeID_to_StreamCH_H265( E_VIDEO_ENOCDE_ID video_enc_id, E_H265_IF_STREAM_CH* stream_ch );
FJ_ERR_CODE BF_Movie_Video_Attr_Change_StreamCH_to_EncodeID_H265( E_H265_IF_STREAM_CH stream_ch, E_VIDEO_ENOCDE_ID* video_enc_id );
FJ_ERR_CODE	BF_Movie_Video_Attr_Set_H264_Config( E_VIDEO_ENOCDE_ID video_enc_id, T_FJ_MOVIE_H264_CONFIG_PARAM* cfg_param );
FJ_ERR_CODE	BF_Movie_Video_Attr_Get_H264_Config( E_VIDEO_ENOCDE_ID video_enc_id, T_H264_IF_PARAM_RECORD* cfg_param );
FJ_ERR_CODE	BF_Movie_Video_Attr_Set_H265_Config( E_VIDEO_ENOCDE_ID video_enc_id, T_FJ_MOVIE_H265_CONFIG_PARAM* cfg_param );
FJ_ERR_CODE	BF_Movie_Video_Attr_Get_H265_Config( E_VIDEO_ENOCDE_ID video_enc_id, T_H265_IF_PARAM_RECORD* cfg_param );
FJ_ERR_CODE BF_Movie_Video_Attr_Set_Enable_Flg( E_VIDEO_ENOCDE_ID video_enc_id, UCHAR enable_flag );
UCHAR	BF_Movie_Video_Attr_Get_Enable_Flg( E_VIDEO_ENOCDE_ID video_enc_id );
INT32	BF_Movie_Video_Attr_Get_Frame_Rate( E_VIDEO_ENOCDE_ID video_enc_id );
INT32	BF_Movie_Video_Attr_Get_Frame_Rate_Value( E_VIDEO_ENOCDE_ID video_enc_id );
UCHAR	BF_Movie_Video_Attr_Get_Timelapse_Flag( E_VIDEO_ENOCDE_ID video_enc_id );
FJ_ERR_CODE	BF_Movie_Video_Attr_Set_Compression( E_VIDEO_ENOCDE_ID video_enc_id, BOOL compression );

FJ_ERR_CODE	BF_Movie_Video_Attr_Set_Decoder_Codec_Type( UINT8 codec_type );
FJ_ERR_CODE	BF_Movie_Video_Attr_Get_Decoder_Codec_Type( UINT8* codec_type );
FJ_ERR_CODE	BF_Movie_Video_Attr_Set_H264_Decoder_Config( T_FJ_MOVIE_PLAY_VIDEO_CONFIG_PARAM* video_config );
FJ_ERR_CODE BF_Movie_Video_Attr_Get_H264_Decoder_Config( T_H264_IF_DECODER_PARAM* video_config );
FJ_ERR_CODE BF_Movie_Video_Attr_Set_H265_Decoder_Config( T_FJ_MOVIE_PLAY_VIDEO_CONFIG_PARAM* video_config );
FJ_ERR_CODE BF_Movie_Video_Attr_Get_H265_Decoder_Config( T_H265_IF_DECODER_PARAM* video_config );
FJ_ERR_CODE BF_Movie_Video_Attr_Set_Decoder_Play_Start_Pts( ULLONG start_pts );
FJ_ERR_CODE BF_Movie_Video_Attr_Get_Decoder_Play_Start_Pts( ULLONG* start_pts );
FJ_ERR_CODE BF_Movie_Video_Attr_Set_Decoder_Play_Speed( E_FJ_MOVIE_PLAY_SPEED speed );
FJ_ERR_CODE BF_Movie_Video_Attr_Get_Decoder_Play_Speed( E_FJ_MOVIE_PLAY_SPEED* speed );
FJ_ERR_CODE BF_Movie_Video_Attr_Set_Decoder_Play_Direction( E_FJ_MOVIE_PLAY_DIRECTION direction );
FJ_ERR_CODE BF_Movie_Video_Attr_Get_Decoder_Play_Direction( E_FJ_MOVIE_PLAY_DIRECTION* direction );
FJ_ERR_CODE BF_Movie_Video_Attr_Set_Decoder_Play_Pause( BOOL pause );
FJ_ERR_CODE BF_Movie_Video_Attr_Get_Decoder_Play_Pause( BOOL* pause );

#endif
