/**
 * @file		movie_common.h
 * @brief		Movie Common Function Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _MOVIE_COMMON_H_
#define _MOVIE_COMMON_H_

#include "ddim_typedef.h"
#include "sdram_map_movie_record.h"
#include "fj_message.h"
#include "fj_host.h"
#include "movie_video.h"
#include "movie_video_attr.h"
#include "h264_if.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_BF_MOVIE_COMMON_OK		(0)			// OK (normal end)
#define D_BF_MOVIE_COMMON_NG		(-1)		// NG (error)
#define D_BF_MOVIE_COMMON_ON		(1)			// ON
#define D_BF_MOVIE_COMMON_OFF		(0)			// OFF

/* Interlace/Progressive */
#define D_BF_MOVIE_COMMON_SCAN_MODE_PROGRESSIVE	(0)	// scan mode is PROGRESSIVE
#define D_BF_MOVIE_COMMON_SCAN_MODE_INTERLACE	(1)	// scan mode is interlace

/* VOP Time Increment Resolution */
#define D_BF_MOVIE_COMMON_TIMERESO_239_76	(240000)	// 239.76 fps
#define D_BF_MOVIE_COMMON_TIMERESO_240_0	(240000)	// 240 fps
#define D_BF_MOVIE_COMMON_TIMERESO_50_0		(50000)		// 50 fps

/* VOP Time Increment */
#define D_BF_MOVIE_COMMON_TIMEINC_239_76	(1001)		// 239.76 fps
#define D_BF_MOVIE_COMMON_TIMEINC_240_0		(1000)		// 240 fps
#define D_BF_MOVIE_COMMON_TIMEINC_50_0		(1000)		// 50 fps


#define D_BF_MOVIE_COMMON_PATTERN_NUM	(E_SDRAM_MAP_MOVIE_RECORD_PATTERN_MAX)

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* --- H264 Time Information */
/* Reload Timer */
typedef struct{
	USHORT	tmcsr_csl;					// select clock
	USHORT	soft_tmrlr;					// Soft Count
	UINT32	tmrlr;						// TMRLR value
}T_BF_MOVIE_COMMON_RTIME_CTL;

/* Timer Frame Rate */
typedef enum{
	E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_239_76 = 0,
	E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_240_0,
	E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_50_0,
	E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_INITIALIZE,
} E_BF_MOVIE_COMMON_TIMER_FRAME_RATE;

/* Reload Timer mode */
typedef enum{
	E_BF_MOVIE_COMMON_RTIMER_MODE_ENC = 0,
	E_BF_MOVIE_COMMON_RTIMER_MODE_DEC,
}E_BF_MOVIE_COMMON_RTIMER_MODE;

/* Stream  Status*/
typedef enum{
	E_BF_MOVIE_COMMON_STREAM_STATUS_STOP	= 0,
	E_BF_MOVIE_COMMON_STREAM_STATUS_START	= 1,
	E_BF_MOVIE_COMMON_STREAM_STATUS_CHANGETO_START = 2, 			//CAPTURE TYPE
	E_BF_MOVIE_COMMON_STREAM_STATUS_CHANGETO_STOP = 3,				//IDLE TYPE
}E_BF_MOVIE_COMMON_STREAM_STATUS;

/* Sender of notification */
typedef enum{
	E_BF_MOVIE_COMMON_MODULE_AUDIO = 0,
	E_BF_MOVIE_COMMON_MODULE_H264 ,			
	E_BF_MOVIE_COMMON_MODULE_MAX
}E_BF_MOVIE_COMMON_MODULE;

/* Movie Common stream Control */
typedef enum{
     E_BF_MOVIE_COMMON_MOVIE_STOP = 0,
     E_BF_MOVIE_COMMON_MOVIE_SETUP ,
     E_BF_MOVIE_COMMON_MOVIE_SETUP_COMPLETE ,
     E_BF_MOVIE_COMMON_MOVIE_SETMODE ,
     E_BF_MOVIE_COMMON_MOVIE_RUNNING ,
     E_BF_MOVIE_COMMON_MOVIE_STOPING
}E_BF_MOVIE_COMMON_MOVIE_CTL;

typedef enum{
	E_BF_MOVIE_COMMON_NOTIFIER_START = 0,
	E_BF_MOVIE_COMMON_NOTIFIER_STOP ,	
	E_BF_MOVIE_COMMON_NOTIFIER_ERR ,			
	E_BF_MOVIE_COMMON_NOTIFIER_TYPE_MAX
}E_BF_MOVIE_COMMON_NOTIFIER_TYPE;

typedef VOID (*T_Mode_Change_Callback)(FJ_MODE , E_BF_MOVIE_COMMON_NOTIFIER_TYPE);
/*--------------------------------------------------------------*/
/* Global Function												*/
/*--------------------------------------------------------------*/

//  ReloadTimer Wrapper
extern	VOID	BF_Movie_Common_Timer_Calculate( UINT32 time_inc, UINT32 time_inc_resol, UINT32 scan_mode );
extern	INT32	BF_Movie_Common_Timer_Start( E_BF_MOVIE_COMMON_RTIMER_MODE mode );
extern	VOID	BF_Movie_Common_Timer_Stop( E_BF_MOVIE_COMMON_RTIMER_MODE mode );

// Movie Timer Control
extern	INT32	BF_Movie_Common_Convert_Framerate( E_BF_MOVIE_COMMON_TIMER_FRAME_RATE framerate, ULONG *vop_time_increment, ULONG *vop_time_inc_resol );
extern	VOID	BF_Movie_Common_Sync_Setting( E_VIDEO_ENOCDE_ID video_enc_id );
extern	VOID	BF_Movie_Common_Timer_Start_View( E_VIDEO_ENOCDE_ID video_enc_id );
extern	VOID	BF_Movie_Common_Set_Timer_Start_Flag(E_VIDEO_ENOCDE_ID video_enc_id, BOOL start);
extern	BOOL	BF_Movie_Common_Get_Timer_Start_Flag( E_VIDEO_ENOCDE_ID video_enc_id );
extern	INT32	BF_Movie_Common_Timer_Counter_Reset(VOID);
extern	ULLONG	BF_Movie_Common_Timer_Counter_Get( VOID );
extern	INT32	BF_Movie_Common_Timer_Setting( E_VIDEO_ENOCDE_ID video_enc_id );
extern	VOID	BF_Movie_Common_Sync_Start_H264( ULONG stream_ch );
extern	VOID	BF_Movie_Common_Sync_Stop_H264( VOID );
extern	VOID	BF_Movie_Common_Sync_Start_H265( ULONG stream_ch );
extern	VOID	BF_Movie_Common_Sync_Stop_H265( VOID );

#if defined(CO_H264_ENC_DEBUG_VIEW_DUMMY) || defined(CO_H265_ENC_DEBUG_VIEW_DUMMY)
extern	VOID	BF_Movie_Common_Set_Dummy_Sync( E_VIDEO_ENOCDE_ID enc_id, BOOL flag );
#endif
VOID BF_Movie_Common_Reload_Enc_Handler( VOID );
VOID BF_Movie_Common_RecTimer_Enable( BOOL enable );

#endif

