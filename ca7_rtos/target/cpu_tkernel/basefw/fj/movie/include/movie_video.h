/**
 * @file		movie_video.h
 * @brief		SocioNext Video Controller Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_VIDEO_H
#define _FJ_VIDEO_H

#include "fj_encoder.h"
#include "fj_decoder.h"

/******************************************************************************
 Fixed Define
******************************************************************************/

//----------------------
// Event Flag
//----------------------
// FID_ENCODER
#define D_BF_FLG_SETUP_COMPLETE_H264		(0x00000001)
#define D_BF_FLG_SETUP_COMPLETE_H265		(0x00000002)
#define D_BF_FLG_START_COMPLETE_H264		(0x00000004)
#define D_BF_FLG_START_COMPLETE_H265		(0x00000008)

#define D_BF_FLG_START_SYNC_H264			(0x10000000)
#define D_BF_FLG_START_SYNC_H265			(0x20000000)

// FID_DECODER
#define D_BF_FLG_DECODER_STATE_CHANGE		(0x00000001)

/******************************************************************************
 Structure
******************************************************************************/
typedef struct{
	ULONG	y_addr;
	ULONG	c_addr;
	ULONG	y_cmp_addr;
	ULONG	c_cmp_addr;
	USHORT	g_width;
	BOOL	dummy_flg;
} T_BF_ENCODER_GET_IMAGE_ADDR_PARAM;

/******************************************************************************
 Enumeration
******************************************************************************/
typedef enum{
	E_VIDEO_ENOCDE_ID_H264_CH0 = 0,		/**< H264 CH0 */
	E_VIDEO_ENOCDE_ID_H264_CH1,			/**< H264 CH1 */
	E_VIDEO_ENOCDE_ID_H264_CH2,			/**< H264 CH2 */
	E_VIDEO_ENOCDE_ID_H265_CH0,			/**< H265 CH0 */
	E_VIDEO_ENOCDE_ID_H265_CH1,			/**< H265 CH1 */
	E_VIDEO_ENOCDE_ID_MAX,
}E_VIDEO_ENOCDE_ID;

/******************************************************************************
 Function
******************************************************************************/
extern	FJ_ERR_CODE		fj_videoencoder_open( E_FJ_VIDEO_ENC_ENUM video_enc, UCHAR* video_enc_id );
extern	FJ_ERR_CODE		fj_videoencoder_close( UCHAR video_enc_id );
extern	FJ_ERR_CODE		fj_videoencoder_setinstanceenabled( UCHAR video_enc_id, BOOL enabled );
extern	FJ_ERR_CODE		fj_videoencoder_config( UCHAR video_enc_id, UINT32 wConfig, UINT32 wOperation );
extern	FJ_ERR_CODE		fj_videoencoder_configdynamic( UCHAR video_enc_id, UINT32 wConfig, UINT32 wOperation );
extern	FJ_ERR_CODE		fj_videoencoder_insert_idr( UCHAR video_enc_id, UINT32 wConfig );
extern	FJ_ERR_CODE		fj_videoencoder_setup( UCHAR video_enc_id );
extern	FJ_ERR_CODE		fj_videoencoder_start( UCHAR video_enc_id );
extern	FJ_ERR_CODE		fj_videoencoder_stop( UCHAR video_enc_id );
extern	FJ_ERR_CODE		fj_videoencoder_freeindex( UCHAR video_enc_id, UINT32 free_vstream_index, BOOL last_free_flg );
extern	FJ_ERR_CODE 	fj_videoencoder_compenable( UCHAR video_enc_id, BOOL comp_enable );

extern	FJ_ERR_CODE		fj_videodecoder_open( E_FJ_VIDEO_DEC_ENUM video_dec,  UCHAR* video_decoder_id );
extern	FJ_ERR_CODE		fj_videodecoder_close( UCHAR video_decoder_id );
extern	FJ_ERR_CODE		fj_videodecoder_connect( UCHAR video_decoder_id, UCHAR display_id, BOOL sub_inst );
extern	FJ_ERR_CODE		fj_videodecoder_disconnect( UCHAR video_decoder_id, UCHAR display_id, BOOL sub_inst );
extern	FJ_ERR_CODE		fj_videoplayback_videoinit( VOID );
extern	FJ_ERR_CODE		fj_videoplayback_setvideoconfig( T_FJ_MOVIE_PLAY_VIDEO_CONFIG_PARAM* video_config );
extern	FJ_ERR_CODE		fj_videoplayback_setstartpts( ULLONG start_pts );
extern	FJ_ERR_CODE		fj_videoplayback_setspeeddirection( E_FJ_MOVIE_PLAY_SPEED speed, E_FJ_MOVIE_PLAY_DIRECTION direction, BOOL pause );
extern	FJ_ERR_CODE		fj_videoplayback_setvideostream( E_FJ_VIDEO_DEC_ENUM codec, UINT32 store_num, T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM* video_stream_param );
extern	FJ_ERR_CODE		fj_videoplayback_startvideodecode( E_FJ_VIDEO_DEC_ENUM codec );
extern	FJ_ERR_CODE		fj_videoplayback_stopvideodecode( E_FJ_VIDEO_DEC_ENUM codec, E_FJ_MOVIE_PLAY_LAST_PICTURE last_picture );
extern	FJ_ERR_CODE		fj_videoplayback_pausevideo( VOID );
extern	FJ_ERR_CODE		fj_videoplayback_resumevideo( VOID );
extern	FJ_ERR_CODE		fj_videoplayback_getdisplayframe( T_FJ_MOVIE_PLAY_VIDEO_YUV_INFO* display_yuv_info );
extern	FJ_ERR_CODE		fj_videoplayback_startiframedecoding( BOOL display, ULLONG req_pts, UINT32 store_num, T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM* video_stream_param, T_FJ_MOVIE_PLAY_VIDEO_YUV_INFO* decode_yuv_info);
extern	FJ_ERR_CODE		fj_videoplayback_memalloc( E_FJ_MOVIE_PLAY_BUFFER_TYPE buffer_type, UINT32* size, UINT32* address );

extern	VOID			FJ_Record_Timer_Handler( VOID );
extern	VOID			FJ_Record_Timer_Enable( BOOL enable );

extern	FJ_ERR_CODE		BF_Encoder_Start( E_VIDEO_ENOCDE_ID video_enc_id, BOOL sync );
extern	FJ_ERR_CODE		BF_Encoder_Valid_Image_Address( E_VIDEO_ENOCDE_ID video_enc_id );
extern	FJ_ERR_CODE		BF_Encoder_Get_Image_Address( E_VIDEO_ENOCDE_ID video_enc_id, E_FJ_MOVIE_VIDEO_SIZE size, ULLONG frame_no, T_BF_ENCODER_GET_IMAGE_ADDR_PARAM* addr_param );
extern	FJ_ERR_CODE		BF_Encoder_Decide_Image_Address( E_VIDEO_ENOCDE_ID video_enc_id, ULLONG frame_no, BOOL dummy_flg );

extern	VOID			BF_Instance_Print_VideoDecode( VOID );

/******************************************************************************
 Global data
******************************************************************************/
#endif
