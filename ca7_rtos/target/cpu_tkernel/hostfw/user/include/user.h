/**
 * @file		user.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _USER_H_
#define _USER_H_

#include "share.h"
#include "share_data.h"
#include "fj_disp.h"
#include "fj_demux.h"
#include "LT6911UX.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_USER_NF0_DRIVE	"A"
#define D_USER_NF1_DRIVE	"B"
#define D_USER_NF2_DRIVE	"C"
#define D_USER_NF3_DRIVE	"D"
#define D_USER_NF4_DRIVE	"E"
#define D_USER_NF5_DRIVE	"F"
#define D_USER_NF6_DRIVE	"G"
#define D_USER_SD1_DRIVE	"I"
#define D_USER_EM0_DRIVE	"A"
#define D_USER_EM1_DRIVE	"B"
#define D_USER_EM2_DRIVE	"C"
#define D_USER_EM3_DRIVE	"D"
#define D_USER_EM4_DRIVE	"E"
#define D_USER_EM5_DRIVE	"F"
#define D_USER_EM6_DRIVE	"G"

#define D_ADDR_UNLOCK		(0)

#define D_JPEG_RING_AREA_SIZE 0x1400000
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_USER_EVENT_NONE = 0,
	E_USER_EVENT_STANDBY,
	E_USER_EVENT_VIEW_START,
	E_USER_EVENT_VIEW_RECONFIG,
	E_USER_EVENT_SIMPLE_VIEW_START,
	E_USER_EVENT_YUV_IN_VIEW_START,
	E_USER_EVENT_CAPTURE_START,
	E_USER_EVENT_CAPTURE_START_JPG_RAW_YUV,
	E_USER_EVENT_PLAY_START,
	E_USER_EVENT_STREAMING,
	E_USER_EVENT_STREAM_DEC_START,
	E_USER_EVENT_STREAM_DEC_STOP,
	E_USER_EVENT_SYSTEM_MODE_END,
	E_USER_EVENT_VD,
	E_USER_EVENT_QVIEW,
	E_USER_EVENT_YUV,
	E_USER_EVENT_AEAWB_STAT,
	E_USER_EVENT_AF_STAT,
	E_USER_EVENT_AF_MODE_CHG,
	E_USER_EVENT_AF_CTRL_CHG,
	E_USER_EVENT_AF_RANGE_CHG,
	E_USER_EVENT_AF_RELEASE,
	E_USER_EVENT_SAVE_RAW,
	E_USER_EVENT_SAVE_RAW_END,
	E_USER_EVENT_SAVE_JPG,
	E_USER_EVENT_SAVE_YUV,
	E_USER_EVENT_SAVE_YUV_END,
	E_USER_EVENT_JPG_ENC_END,
	E_USER_EVENT_JPG_SAVE_END,
	E_USER_EVENT_JPG_REQ_NUM_END,
	E_USER_EVENT_RAWYUV_SAVE_END,
	E_USER_EVENT_RECORD_START,
	E_USER_EVENT_RECORD_START_AGAIN,
	E_USER_EVENT_RECORD_START_COMPLETE,
	E_USER_EVENT_RECORD_CACHE_START,
	E_USER_EVENT_RECORD_CACHE_START_AGAIN,
	E_USER_EVENT_RECORD_CACHE_H264_STREAM_CB,
	E_USER_EVENT_RECORD_CACHE_H265_STREAM_CB,
	E_USER_EVENT_RECORD_STOP,
	E_USER_EVENT_RECORD_H265_END,
	E_USER_EVENT_RECORD_H264_0_END,
	E_USER_EVENT_RECORD_H264_1_END,
	E_USER_EVENT_RECORD_AUDIO_0_END,
	E_USER_EVENT_RECORD_AUDIO_1_END,
	E_USER_EVENT_FD_START,
	E_USER_EVENT_FD_STOP,
	E_USER_EVENT_PD_START,
	E_USER_EVENT_PD_STOP,
	E_USER_EVENT_PLAY_IMAGE_NO,
	E_USER_EVENT_PLAY_VIDEO,
	E_USER_EVENT_PLAY_PAUSE,
	E_USER_EVENT_PLAY_STOP,
	E_USER_EVENT_PLAY_FAST_FORWARD,
	E_USER_EVENT_PLAY_FAST_REWIND,
	E_USER_EVENT_PLAY_FRAME_BY_FRAME,
	E_USER_EVENT_PLAY_SPEC_TIME,
	E_USER_EVENT_PLAY_THUMBNAIL_START,
	E_USER_EVENT_PLAY_THUMBNAIL_KEY_UP,
	E_USER_EVENT_PLAY_THUMBNAIL_KEY_DOWN,
	E_USER_EVENT_PLAY_THUMBNAIL_KEY_LEFT,
	E_USER_EVENT_PLAY_THUMBNAIL_KEY_RIGHT,
	E_USER_EVENT_PLAY_THUMBNAIL_KEY_ENTER,
	E_USER_EVENT_PLAY_THUMBNAIL_END,
	E_USER_EVENT_PLAY_VIDEO_DECODE,
	E_USER_EVENT_PLAY_AUDIO_DECODE,
	E_USER_EVENT_IPCU_STREAMING_START_STOP,
	E_USER_EVENT_CAP_WITHOUT_RESET,
	E_USER_EVENT_MAX,
} E_USER_EVENT;

typedef enum {
	E_USER_SYNC_EVENT_NONE = 0,
	E_USER_SYNC_EVENT_VIEW_START,
	E_USER_SYNC_EVENT_CAP_START,
	E_USER_SYNC_EVENT_SAVE_RAW,
	E_USER_SYNC_EVENT_ENC_THUMB,
	E_USER_SYNC_EVENT_SAVE_JPG,
	E_USER_SYNC_EVENT_SAVE_YUV,
	E_USER_SYNC_EVENT_SAVE_RAW_YUV,
	E_USER_SYNC_EVENT_JPG_ENC_END,
	E_USER_SYNC_EVENT_JPG_SAVE_END,
	E_USER_SYNC_EVENT_LINUX_END_RAW,
	E_USER_SYNC_EVENT_LINUX_END_JPG,
	E_USER_SYNC_EVENT_LINUX_END_YUV,
	E_USER_SYNC_EVENT_RECORD_START,
	E_USER_SYNC_EVENT_RECORD_START_AGAIN,
	E_USER_SYNC_EVENT_RECORD_CACHE_START,
	E_USER_SYNC_EVENT_RECORD_CACHE_START_AGAIN,
	E_USER_SYNC_EVENT_RECORD_CACHE_H264_STREAM_CB,
	E_USER_SYNC_EVENT_RECORD_CACHE_H265_STREAM_CB,
	E_USER_SYNC_EVENT_RECORD_STOP,
	E_USER_SYNC_EVENT_FACE_DETECTION,
	E_USER_SYNC_EVENT_PERSON_DETECTION,
	E_USER_SYNC_EVENT_SAVE_FILE,
	E_USER_SYNC_EVENT_FEED_STREAM,
	E_USER_SYNC_EVENT_MAX,
} E_USER_SYNC_EVENT;

typedef enum {
	E_USER_RECORD_EVENT_NONE = 0,
	E_USER_RECORD_EVENT_RECORD_H264_FREE_INDEX_0,
	E_USER_RECORD_EVENT_RECORD_H264_FREE_INDEX_1,
	E_USER_RECORD_EVENT_RECORD_H265_FREE_INDEX_0,
	E_USER_RECORD_EVENT_MAX,
} E_USER_RECORD_EVENT;

typedef enum {
	E_USER_STATE_IDLE = 0,
	E_USER_STATE_BUSY,
	E_USER_STATE_RUN,
	E_USER_STATE_MAX,
} E_USER_STATE;

typedef enum {
	E_USER_OP_NONE = 0,
	E_USER_OP_CAP_JPEG_ONE_FRAME,
	E_USER_OP_CAP_JPEG_DONE,
	E_USER_OP_CAP_JPEG_RAW_YUV_ONE_FRAME,
	E_USER_OP_CAP_JPEG_RAW_YUV_DONE,
	E_USER_OP_CAP_CALIB_DONE,
	E_USER_OP_REC_RUN,
	E_USER_OP_REC_CACHE_RUN,
	E_USER_OP_REC_STOP,
	E_USER_OP_SAVE_YUV_DONE,
	E_USER_OP_SAVE_RAW_DONE,
	E_USER_OP_FD_START_DONE,
	E_USER_OP_FD_STOP_DONE,
	E_USER_OP_PD_START_DONE,
	E_USER_OP_PD_STOP_DONE,
	E_USER_OP_PLAY_START_DONE,
	E_USER_OP_PLAY_IMAGE_NO_DONE,
	E_USER_OP_PLAY_VIDEO_DONE,
	E_USER_OP_PLAY_PAUSE_DONE,
	E_USER_OP_PLAY_STOP_DONE,
	E_USER_OP_PLAY_FAST_FORWARD_DONE,
	E_USER_OP_PLAY_FAST_REWIND_DONE,
	E_USER_OP_PLAY_FRAME_BY_FRAME_DONE,
	E_USER_OP_PLAY_SPEC_TIME_DONE,
	E_USER_OP_PLAY_THUMBNAIL_START_DONE,
	E_USER_OP_PLAY_THUMBNAIL_KEY_UP_DONE,
	E_USER_OP_PLAY_THUMBNAIL_KEY_DOWN_DONE,
	E_USER_OP_PLAY_THUMBNAIL_KEY_LEFT_DONE,
	E_USER_OP_PLAY_THUMBNAIL_KEY_RIGHT_DONE,
	E_USER_OP_PLAY_THUMBNAIL_KEY_ENTER_DONE,
	E_USER_OP_PLAY_THUMBNAIL_END_DONE,
	E_USER_OP_STR_SETUP_DONE,
	E_USER_OP_STR_DEC_RUN,
	E_USER_OP_STR_DEC_IDLE,
	E_USER_OP_MAX,
} E_USER_OP_STATE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	ULONG*	BOOTUP_HDMI_addr;
	ULONG	BOOTUP_HDMI_size;
	ULONG*	BOOTUP_addr;
	ULONG	BOOTUP_size;
	ULONG*	IQ_BIN_addr[9];
	ULONG	IQ_BIN_size[9];
	ULONG*	CCM1_addr;
	ULONG	CCM1_size;
	ULONG*	EVP_MAP_addr;
	ULONG	EVP_MAP_size;
	ULONG*	FSHD_BIN_addr[9];
	ULONG	FSHD_BIN_size[9];
	ULONG*	RGB_BIN_addr;
	ULONG	RGB_BIN_size;
	ULONG*	LDC_BIN_addr[8];
	ULONG	LDC_BIN_size[8];
	ULONG*	WARP_L_BIN_addr;
	ULONG	WARP_L_BIN_size;
	ULONG*	WARP_R_BIN_addr;
	ULONG	WARP_R_BIN_size;
	ULONG*	CAM_CAL_BIN_addr[2];
	ULONG	CAM_CAL_BIN_size[2];
	ULONG*	BADPIX_addr[4];
	ULONG	BADPIX_size[4];
	ULONG*	GAMFL_addr[8];
	ULONG	GAMFL_size[8];
	ULONG*	GAMDF_addr[8];
	ULONG	GAMDF_size[8];
} T_RESOURCE;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

INT32 Snd_Event_to_User( E_USER_EVENT event, ULLONG frame_no );

INT32 User_Utility_Record_H264_Start( T_SHARE_VIDEO_REC_CONFIG* cfg, ULONG base_fps );
INT32 User_Utility_Record_H264_Stop( UCHAR stream_idx );
INT32 User_Utility_Record_H264_End( UCHAR stream_idx );
INT32 User_Utility_Record_H264_Get_Addr( T_BF_ENCODER_GET_IMAGE_ADDR_PARAM* addr_param, UCHAR stream_idx, ULLONG frame_no );
//INT32 User_Utility_Record_H264_Rel_Addr( UCHAR stream_idx, ULLONG frame_no );
INT32 User_Utility_Record_H264_Get_Addr2( T_BF_ENCODER_GET_IMAGE_ADDR_PARAM* addr_param, UCHAR stream_idx, ULLONG frame_no );
INT32 User_Utility_Record_H264_Rel_Addr2( BOOL dummy_flg, ULLONG frame_no );
INT32 User_Utility_Record_H264_SequenceStartCB( UCHAR video_enc_id );
INT32 User_Utility_Record_H264_StreamCB( UCHAR video_enc_id, T_FJ_MOVIE_H264_STREAM_PARAM* stream_info);
INT32 User_Utility_Record_H264_StreamCBSync( UCHAR video_enc_id, T_FJ_MOVIE_H264_STREAM_PARAM *stream_info );
INT32 User_Utility_Record_H264_Free_Index( UCHAR stream_idx, UINT32 index, BOOL last );
BOOL User_Utility_Record_H264_Is_Running( VOID );
INT32 User_Utility_Record_H264_Cache_Start( T_SHARE_VIDEO_REC_CONFIG* cfg, ULONG base_fps );
INT32 User_Utility_Record_H264_Cache_StreamCB( UCHAR video_enc_id );
INT32 User_Utility_Record_H264_Cache_End( UCHAR stream_idx );
BOOL User_Utility_Record_H264_Is_Cache_Running( VOID );
ULONG User_Utility_Record_H264_Get_Cache_Size( UINT32 bitrate );

INT32 User_Utility_Record_H265_Start( T_SHARE_VIDEO_REC_CONFIG* cfg, ULONG base_fps, E_SHARE_YUV_OUT stream_idx );
INT32 User_Utility_Record_H265_Stop( UCHAR stream_idx );
INT32 User_Utility_Record_H265_End( UCHAR stream_idx );
INT32 User_Utility_Record_H265_Get_Addr( T_BF_ENCODER_GET_IMAGE_ADDR_PARAM* addr_param, UCHAR stream_idx, ULLONG frame_no );
//INT32 User_Utility_Record_H265_Rel_Addr( ULLONG frame_no );
INT32 User_Utility_Record_H265_Get_Addr2( T_BF_ENCODER_GET_IMAGE_ADDR_PARAM* addr_param, UCHAR stream_idx, ULLONG frame_no );
INT32 User_Utility_Record_H265_Rel_Addr2( BOOL dummy_flg, ULLONG frame_no );
INT32 User_Utility_Record_H265_SequenceStartCB( UCHAR video_enc_id, INT32 *vop_cnt);
INT32 User_Utility_Record_H265_StreamCB( UCHAR video_enc_id, T_FJ_MOVIE_H265_STREAM_PARAM* stream_info);
INT32 User_Utility_Record_H265_StreamCB_Sync( UCHAR stream_idx, T_FJ_MOVIE_H265_STREAM_PARAM * stream_info );
INT32 User_Utility_Record_H265_Free_Index( UCHAR stream_idx, UINT32 index, UCHAR last);
BOOL User_Utility_Record_H265_Is_Running( VOID );
INT32 User_Utility_Record_H265_Cache_Start( T_SHARE_VIDEO_REC_CONFIG* cfg, ULONG base_fps, E_SHARE_YUV_OUT stream_idx );
INT32 User_Utility_Record_H265_Cache_StreamCB( UCHAR video_enc_id );
INT32 User_Utility_Record_H265_Cache_End( UCHAR stream_idx );
BOOL User_Utility_Record_H265_Is_Cache_Running( VOID );
ULONG User_Utility_Record_H265_Get_Cache_Size( UINT32 bitrate );

INT32 User_Utility_Record_Audio_PCMStreamCB( UCHAR audio_enc_id, T_FJ_AUDIO_PCM_STREAM_PARAM* cb_info );
INT32 User_Utility_Record_Audio_AACStreamCB( UCHAR audio_enc_id, T_FJ_AUDIO_AAC_STREAM_PARAM* cb_info );
INT32 User_Utility_Record_Audio_Free_Index( UCHAR aud_id, UINT32 free_index, BOOL last_free_flg );
INT32 User_Utility_Record_Audio_CapturePlugIn0CB( UCHAR audio_capture_id, T_FJ_AUDIO_CAP_PLUGIN0_CB_PARAM* param );
INT32 User_Utility_Record_Audio_CapturePlugIn1CB( UCHAR audio_capture_id, T_FJ_AUDIO_CAP_PLUGIN1_CB_PARAM* param );
INT32 User_Utility_Record_Audio_Start( UCHAR aud_id, T_SHARE_AUDIO_REC_CONFIG* cfg );
INT32 User_Utility_Record_Audio_Stop( UCHAR aud_id );
INT32 User_Utility_Record_Audio_End( UCHAR aud_id );

VOID User_Utility_Record_Mux_Initial( VOID );
INT32 User_Utility_Record_Mux_Start( T_SHARE_AUDIO_REC_CONFIG*	audio );
INT32 User_Utility_Record_Mux_Cache_Start( VOID );
T_FJ_MUX_VIDEO_CONFIG* User_Utility_Record_Mux_Get_Config( VOID );
VOID User_Utility_Record_Mux_Audio_Force_End( UINT32 stream_id );
VOID User_Utility_Record_Mux_Mem_Finalize( VOID );
BOOL User_Utility_Record_Mux_Check_Media_Size( VOID );
VOID User_Utility_Record_Mux_VideoAddSampleCB( UINT32 code );
VOID User_Utility_Aud_Mux_Config( UCHAR aud_id, T_SHARE_AUDIO_REC_CONFIG *audio );

INT32 User_Utility_Playback_Jpeg_Play( ULLONG file_index );
INT32 User_Utility_Playback_Video_Play( UINT32 second );
INT32 User_Utility_Playback_Video_Pause( VOID );
INT32 User_Utility_Playback_Video_Stop( VOID );
INT32 User_Utility_Playback_Video_Fast_Forward( E_FJ_MOVIE_PLAY_SPEED speed );
INT32 User_Utility_Playback_Video_Fast_Rewind( E_FJ_MOVIE_PLAY_SPEED speed );
INT32 User_Utility_Playback_Video_Frame_By_Frame( VOID );
INT32 User_Utility_Playback_Video_Play_Spec_Time( UINT32 second );
INT32 User_Utility_Playback_Thumbnail_Play( UINT32 thumbnail_style );
INT32 User_Utility_Playback_Thumbnail_Key_Up( VOID );
INT32 User_Utility_Playback_Thumbnail_Key_Down( VOID );
INT32 User_Utility_Playback_Thumbnail_Key_Left( VOID );
INT32 User_Utility_Playback_Thumbnail_Key_Right( VOID );
INT32 User_Utility_Playback_Thumbnail_Key_Enter( VOID );
INT32 User_Utility_Playback_Thumbnail_End( VOID );
INT32 User_Utility_Playback_Print_File_List( VOID );
INT32 User_Utility_Playback_Set_Disp_If( E_SHARE_DISP_IF disp_if );
INT32 User_Utility_Playback_AudioOutPlugInCB( UCHAR audio_out_id, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM * param );
INT32 User_Utility_Playback_StopWavCB( UCHAR audio_out_id, UCHAR stop_factor );
INT32 User_Utility_Playback_GetVideoStreamCB( VOID );
INT32 User_Utility_Playback_GetAudioStreamCB( VOID );
INT32 User_Utility_Playback_FirstFrameDisplayCB( E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG display_pts, BOOL error );
INT32 User_Utility_Playback_VideoDecodeCB( E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG display_pts, BOOL error );
INT32 User_Utility_Playback_AudioDecodeCB( E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG decoded_pts, BOOL error);
BOOL User_Utility_Playback_StreamReadCompleteCB( UINT32 file_id, E_FJ_DEMUX_CODEC codec, T_FJ_DEMUX_STREAM_PARAM* stream_param );
BOOL User_Utility_Playback_GetStreamAreaCB( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32* stream_size, UINT32* stream_address );
INT32 User_Utility_Playback_VideoDecode( VOID );
INT32 User_Utility_Playback_AudioDecode( VOID );

void User_Boot( void );

E_USER_STATE User_View( E_USER_EVENT event, ULLONG frame_no );
VOID User_View_Clear_Stat(VOID);
E_USER_STATE User_VIew_Get_Stat(VOID);
VOID User_View_Clear_OP_Stat(VOID);
E_USER_OP_STATE User_View_Get_OP_Stat(VOID);
BOOL User_View_Is_IPCU_Streaming(VOID);

E_USER_STATE User_YUVin_View( E_USER_EVENT event, ULLONG frame_no );
E_USER_STATE User_YUVin_VIew_Get_Stat(VOID);
VOID User_YUVin_View_Clear_OP_Stat(VOID);
E_USER_OP_STATE User_YUVin_View_Get_OP_Stat(VOID);

E_USER_STATE User_Simple_View( E_USER_EVENT event, ULLONG frame_no );
E_USER_STATE User_Simple_VIew_Get_Stat(VOID);

E_USER_STATE User_Capture( E_USER_EVENT event, ULLONG frame_no );
E_USER_STATE User_Capture_Get_Stat(VOID);
VOID User_Capture_Clear_OP_Stat(VOID);
E_USER_OP_STATE User_Capture_Get_OP_Stat(VOID);

E_USER_STATE User_Play( E_USER_EVENT event, ULLONG frame_no );
BOOL User_Play_Is_Busy( VOID );
E_USER_OP_STATE User_Play_Get_OP_Stat( VOID );
VOID User_Play_Clear_OP_Stat( VOID );

const T_RESOURCE* User_Get_Resource( void );
void Bootup_Image_Display_LCD(void);
void Bootup_Image_Display(void);
void Bootup_Music_Play(BOOL hdmi);

BOOL User_Is_Standby_Mode(VOID);
BOOL User_Is_EmmcBoot_Mode(VOID);

void 	User_Utility_MM_Init(void);
void*	User_Utility_MM_alloc(size_t size, char ch);
void	User_Utility_MM_free(void *ptr, char ch);
void	User_Utility_MM_addblock(void *addr, size_t size, char ch);

#ifdef __cplusplus
}
#endif

#endif	// _USER_H_

