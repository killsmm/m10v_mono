/**
 * @file		user_streaming.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2019 Socionext Inc.</I></B>
 */

#ifndef _USER_STREAMING_H_
#define _USER_STREAMING_H_

#include "share.h"
#include "fj_decoder.h"
#include "os_if.h"
#include "user.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_DECODE_REQUEST_FRAME_NUM 1
//#define D_STREAMING_FROM_FILE
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/


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

E_USER_STATE User_Streaming( E_USER_EVENT event, ULLONG frame_no );
E_USER_OP_STATE User_Streaming_Get_OP_Stat( VOID );
VOID User_Streaming_Clear_OP_Stat( VOID );
int User_StreamingStop(E_FJ_VIDEO_DEC_ENUM codec);

#if defined(D_STREAMING_FROM_FILE)
int User_StreamingGetFrameASync(E_FJ_VIDEO_DEC_ENUM codec, unsigned char num_frame);
int User_StreamingGetFrameSync(E_FJ_VIDEO_DEC_ENUM codec, unsigned char num_frame);
#endif
BOOL User_IsStreamingProcess(void);
INT32 User_StreamingDecodeOneFrameDoneCB( E_FJ_VIDEO_DEC_ENUM codec, E_FJ_MOVIE_PLAY_DECODER_STATUS status, ULLONG pts, BOOL err );
INT32 User_StreamingFirstFrameDisplayCB( E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG display_pts, BOOL error );
INT32 User_SendStreamToReceiver(T_CPUCMD_VIDEO_DEC_STREAM str_info);
const T_SHARE_STREAM_INFO* User_GetStreamType(VOID);
BOOL User_IsH265MainStream(void);
BOOL User_IsDualStream(void);

#ifdef __cplusplus
}
#endif

#endif	// _USER_STREAMING_H_

