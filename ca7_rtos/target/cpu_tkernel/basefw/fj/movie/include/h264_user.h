/**
 * @file		h264_user.h
 * @brief 		User Custom Part for H.264 Function Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _H264_USER_H
#define _H264_USER_H

#include "os_user_custom.h"
#include "ddim_typedef.h"		// "typedef" VOID,INT32,UCHAR...
#include <string.h>			// memcpy, memset...

#include "debug.h"
#include "fj_custom.h"
#include "h264_if.h"
// --- REMOVE_DCF_EXIF BEGIN ---
#include "dcf_if.h"
// --- REMOVE_DCF_EXIF END ---
#include "fj_playback.h"
#include "movie_common.h"

/*--------------------------------------------------------------*/
/* Definition													*/
/*--------------------------------------------------------------*/
#define D_BF_H264_USER_OK							0
#define D_BF_H264_USER_NG							-1
#define D_BF_H264_USER_NG_ALREADY_STOP_PLAY			-2

#define D_BF_H264_USER_ON							1
#define D_BF_H264_USER_OFF							0

/* Exclusive operation of Movie Record */
#define D_BF_H264_USER_RECORD_EXC_OFF				0
#define D_BF_H264_USER_RECORD_EXC_READY				1
#define D_BF_H264_USER_RECORD_EXC_RECORDING			2
#define D_BF_H264_USER_RECORD_EXC_MOVIE_STOP		3
#define D_BF_H264_USER_RECORD_EXC_VIEW_STOP			4

/* Exclusive operation of Movie Play */
#define D_BF_H264_USER_PLAY_EXC_OFF					0
#define D_BF_H264_USER_PLAY_EXC_READY				1
#define D_BF_H264_USER_PLAY_EXC_PLAYING				2

#define D_BF_H264_USER_AUTO_END_OFF					0
#define D_BF_H264_USER_AUTO_END_NORMAL				1
#define D_BF_H264_USER_AUTO_END_TIME				2
#define D_BF_H264_USER_AUTO_END_MEDIA				3
#define D_BF_H264_USER_AUTO_END_STAP_OVER			4
#define D_BF_H264_USER_AUTO_END_BUFF_OVER			5
#define D_BF_H264_USER_AUTO_END_FATAL_ERR			6

// [Host Implementation API]
#define	BF_H264_User_Notify_Stop_Trigger			Host_H264_Notify_Stop_Trigger
#define	BF_H264_User_Notify_Stop_Complete			Host_H264_Notify_Stop_Complete
#define	BF_H264_User_Notify_State_Change_Complete	Host_H264_Notify_State_Change_Complete
#define BF_H264_User_Notify_Video_Stream_Request	Host_H264_Notify_Video_Stream_Request
#define	BF_H264_User_Notify_FirstFrameDisplayCB		Host_H264_Notify_FirstFrameDisplayCB
#define	BF_H264_User_Notify_VideoDecodeCB			Host_H264_Notify_VideoDecodeCB
#define	BF_H264_User_Notify_Lcd_Update				Host_H264_Notify_Lcd_Update
#define	BF_H264_User_Notify_Hdmi_Update				Host_H264_Notify_Hdmi_Update
#define	BF_H264_User_Notify_Video_Stream			Host_H264_Notify_Video_Stream
#define	BF_H264_User_Notify_Vector_Info				Host_H264_Notify_Vector_Info
#define	BF_H264_User_Notify_PreEncode				Host_H264_Notify_PreEncode
#define	BF_H264_User_Notify_RepeatFrame				Host_H264_Notify_RepeatFrame

/*--------------------------------------------------------------*/
/* Enumeration													*/
/*--------------------------------------------------------------*/
/* Image Format */
typedef enum {
	E_BF_H264_USER_IMAGE_FORMAT_420 = 1,
	E_BF_H264_USER_IMAGE_FORMAT_422
} E_BF_H264_USER_IMAGE_FORMAT;

/* Scan Mode */
typedef enum {
	E_BF_H264_USER_SCAN_MODE_PROGRESSIVE = 1,
	E_BF_H264_USER_SCAN_MODE_INTERLACE
} E_BF_H264_USER_SCAN_MODE;

/*--------------------------------------------------------------*/
/* Structure													*/
/*--------------------------------------------------------------*/
typedef struct {
	ULONG						addr_y;
	ULONG						addr_c;
	USHORT						width;
	USHORT						lines;
	USHORT						display_width;
	USHORT						display_lines;
	E_BF_H264_USER_IMAGE_FORMAT	format;
	E_BF_H264_USER_SCAN_MODE	scan_mode;
} T_BF_H264_USER_TOP_IMAGE;

/*--------------------------------------------------------------*/
/* Macro														*/
/*--------------------------------------------------------------*/
// Nothing Special

/*--------------------------------------------------------------*/
/* Global Function												*/
/*--------------------------------------------------------------*/
// ***************************************************************************
// H.264 middleware "Host API" part
// ***************************************************************************
// [Host Implementation API]
extern	VOID	BF_H264_User_Notify_Stop_Trigger( E_H264_IF_STOP_NOTIFY param );
extern	VOID	BF_H264_User_Notify_Stop_Complete( VOID );
extern	VOID	BF_H264_User_Notify_Vstrm_Free_Timeout( VOID );
extern	VOID	BF_H264_User_Notify_State_Change_Complete( INT32 result );
extern	VOID	BF_H264_User_Notify_Lcd_Update( UINT32* y_addr, UINT32* c_addr, BOOL update );
extern	VOID	BF_H264_User_Notify_Hdmi_Update( UINT32* y_addr, UINT32* c_addr, BOOL update );
extern	INT32	BF_H264_User_Notify_Video_Stream(T_H264_IF_VIDEO_STREAM_PARAM* vstream_info);
extern	INT32	BF_H264_User_Notify_Vector_Info(T_H264_IF_VIDEO_VECTOR_PARAM* vector_info);
extern	INT32	BF_H264_User_Notify_PreEncode( E_H264_IF_STREAM_CH stream_ch );
extern	INT32	BF_H264_User_Notify_RepeatFrame( E_H264_IF_STREAM_CH stream_ch );

// ***************************************************************************
// Common Process
// ***************************************************************************
extern	VOID	BF_H264_User_Set_Codec_CoreMode( UINT32 core_mode );

// ***************************************************************************
// Encode Process
// ***************************************************************************
extern	INT32	BF_H264_User_Record_Ready_Movie( E_VIDEO_ENOCDE_ID video_enc_id );
extern	INT32	BF_H264_User_Record_Start_Video( E_VIDEO_ENOCDE_ID video_enc_id, BOOL sync );
extern	INT32	BF_H264_User_Record_Stop_Video( E_VIDEO_ENOCDE_ID video_enc_id );
extern	INT32	BF_H264_User_Record_Set_Dynamic_Parameter( E_VIDEO_ENOCDE_ID video_enc_id, T_FJ_MOVIE_H264_DYNAMIC_CONFIG_PARAM* dynamic_config );
extern	INT32	BF_H264_User_Record_Set_Insert_IDR_Picture( E_VIDEO_ENOCDE_ID video_enc_id );
extern	INT32	BF_H264_User_Record_Get_Sts(VOID);
extern	INT32	BF_H264_User_Record_Get_Image_Address( E_VIDEO_ENOCDE_ID video_enc_id, E_FJ_MOVIE_VIDEO_SIZE size, ULLONG frame_no, T_BF_ENCODER_GET_IMAGE_ADDR_PARAM* addr_param );
extern	VOID	BF_H264_User_Record_Decide_Image_Address( E_VIDEO_ENOCDE_ID video_enc_id, ULLONG frame_no, BOOL dummy_flg );
extern	VOID	BF_H264_User_Record_Frame_Enabled( E_VIDEO_ENOCDE_ID video_enc_id, ULLONG frame_no, BOOL dummy_flg, BOOL enabled );
extern	INT32	BF_H264_User_Record_Free_Video_Index( E_VIDEO_ENOCDE_ID video_enc_id, UINT32 free_vstream_index, BOOL last_free_flg );
extern	VOID	BF_H264_User_Record_Valid_Image_Address( E_VIDEO_ENOCDE_ID video_enc_id );
extern	VOID	BF_H264_User_Record_Set_Gop_Size( E_H264_IF_STREAM_CH stream_ch, UCHAR gop_size );
extern	UCHAR	BF_H264_User_Record_Get_Gop_Size( E_H264_IF_STREAM_CH stream_ch );

// debug
// ***************************************************************************
// Decode Process
// ***************************************************************************
extern	INT32	BF_H264_User_Play_Init( VOID );
extern	INT32	BF_H264_User_Play_Set_Video_Stream( UINT32 store_num, T_H264_IF_PARAM_SET_STREAM* video_stream_param );
extern	INT32	BF_H264_User_Play_Start( VOID );
extern	INT32	BF_H264_User_Play_Stop( BOOL last_picture_keep );
extern	INT32	BF_H264_User_Play_Flush( VOID );
extern	INT32	BF_H264_User_Play_Top( BOOL display, ULLONG req_pts, UINT32 store_num, T_H264_IF_PARAM_SET_STREAM* video_stream_param, T_H264_IF_VIDEO_YUV_INFO* decode_yuv_info );
extern	INT32	BF_H264_User_Play_Get_Display_Frame( T_H264_IF_VIDEO_YUV_INFO* display_yuv_info );
extern	INT32	BF_H264_User_Play_Pause( VOID );
extern	INT32	BF_H264_User_Play_Resume( VOID );
extern	BOOL	BF_H264_User_Is_Play_Stop( VOID );
#endif
