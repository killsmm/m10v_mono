/**
 * @file		still_mov_ctrl.c
 * @brief		Movie codec control functions
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// Still header
#include "still_mov_ctrl.h"
// delete\basefw\fj\imgproc\still
//#include "still_sync_ctrl.h"

// Movie header
#include "still_attr.h"
#include "h264_user.h"
#include "h265_user.h"

// Utility Header
#include "debug.h"
#include "measure_time.h"
#include "mba_if.h"

// OS header
#include "os_user_custom.h"
#include "querystatus.h"
#include "mode.h"
#include "instance.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

#define D_BF_STILL_MOV_CTRL_FRAMENO_PRINTF_CONV( frame_no_64 )		( (UINT32)(frame_no_64 & 0xFFFFull) )
#define D_BF_STILL_MOV_CTRL_FRAMENO_PRINTF_FMT						"%04X"


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

typedef enum {
	E_STILL_MOVIE_READY_IDLE = 0,
	E_STILL_MOVIE_READY_EXPO,
	E_STILL_MOVIE_READY_READ,
	E_STILL_MOVIE_READY_GETADDR,
	E_STILL_MOVIE_READY_GETADDR_ALL_STRM,
	E_STILL_MOVIE_READY_IMGPROC,
	E_STILL_MOVIE_READY_REC,
	E_STILL_MOVIE_READY_MAX
} E_STILL_MOVIE_READY;

typedef enum {
	E_STILL_STREAM_TYPE_AUDIO = 0,
	E_STILL_STREAM_TYPE_H264,
	E_STILL_STREAM_TYPE_MAX,
} E_STILL_STREAM_TYPE;


// Sequence of state change to record:
//
// E_STILL_STREAM_TYPE_AUDIO:
//   1. E_STILL_MOVIE_READY_IDLE
//   2. E_STILL_MOVIE_READY_READ ( by FJ_SetMode( mode ); ... (mode & FJ_MODE_CAPTURE_AUDIO) == TRUE )
//   3. E_STILL_MOVIE_READY_REC  ( by next VD of BaseFW task. )
//
// E_STILL_STREAM_TYPE_H264 (Current FJ_MODE is FJ_MODE_MOVIE_VIEW):
//   1. E_STILL_MOVIE_READY_IDLE
//   2. E_STILL_MOVIE_READY_READ
//      by FJ_SetMode( mode ) ... (mode & FJ_MODE_CAPTURE_H264) == TRUE
//   3. E_STILL_MOVIE_READY_GETADDR_ALL_STRM
//      by BF_StillMovCtrl_Set_ImgProcStat( frame_no_begin, FJ_STREAM_TYPE_H264, E_FJ_IMGPROC_STAT_BEGIN )
//   4. E_STILL_MOVIE_READY_IMGPROC
//      by BF_StillMovCtrl_Set_ImgProcStat( frame_no_getaddr, FJ_STREAM_TYPE_H264, E_FJ_IMGPROC_STAT_GETADDR_ALL_STRM )
//      and frame_no_getaddr >= frame_no_begin.
//   5. E_STILL_MOVIE_READY_REC
//      by BF_StillMovCtrl_Set_ImgProcStat( frame_no_finish, FJ_STREAM_TYPE_H264, E_FJ_IMGPROC_STAT_FINISH )
//      and frame_no_finish >= frame_no_begin.
//
// E_STILL_STREAM_TYPE_H264 (Current FJ_MODE is FJ_MODE_CAPTURE_H264, and/or FJ_MODE_CAPTURE_AUDIO):
//   1. E_STILL_MOVIE_READY_IDLE
//   2. E_STILL_MOVIE_READY_READ
//      by FJ_SetMode( mode ) ... (mode & FJ_MODE_CAPTURE_H264) == TRUE
//   3. E_STILL_MOVIE_READY_GETADDR
//      by BF_StillMovCtrl_Set_ImgProcStat( frame_no_begin, FJ_STREAM_TYPE_H264, E_FJ_IMGPROC_STAT_BEGIN )
//   4. E_STILL_MOVIE_READY_IMGPROC
//      by BF_StillMovCtrl_Set_ImgProcStat( frame_no_getaddr, FJ_STREAM_TYPE_H264, E_FJ_IMGPROC_STAT_GETADDR )
//      and frame_no_getaddr >= frame_no_begin.
//   5. E_STILL_MOVIE_READY_REC
//      by BF_StillMovCtrl_Set_ImgProcStat( frame_no_finish, FJ_STREAM_TYPE_H264, E_FJ_IMGPROC_STAT_FINISH )
//      and frame_no_finish >= frame_no_begin.
//
// E_STILL_STREAM_TYPE_MJPEG:
//   Same as E_STILL_STREAM_TYPE_MJPEG.
//   Replace from FJ_STREAM_TYPE_H264 to FJ_STREAM_TYPE_JPEG when read sequence.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

typedef struct {
	E_STILL_MOVIE_READY	stat;
	ULLONG				frame_no;
	BOOL				prev_mode_is_view;
	const CHAR*			name;
	ULLONG				start_time;
} T_STILL_MOV_READY_STAT;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/

static volatile T_STILL_MOV_READY_STAT	g_movie_ready_status[E_STILL_STREAM_TYPE_MAX][D_BF_INSTANCE_ID_MAX_VIDEO_ENCODE] = {
	[E_STILL_STREAM_TYPE_AUDIO][0].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_AUDIO][0].name = "Audio0",
	[E_STILL_STREAM_TYPE_H264][0].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_H264][0].name = "H2640",
	[E_STILL_STREAM_TYPE_AUDIO][1].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_AUDIO][1].name = "Audio1",
	[E_STILL_STREAM_TYPE_H264][1].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_H264][1].name = "H2641",
	[E_STILL_STREAM_TYPE_AUDIO][2].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_AUDIO][2].name = "Audio2",
	[E_STILL_STREAM_TYPE_H264][2].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_H264][2].name = "H2642",
	[E_STILL_STREAM_TYPE_AUDIO][3].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_AUDIO][3].name = "Audio0",
	[E_STILL_STREAM_TYPE_H264][3].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_H264][3].name = "H2650",
	[E_STILL_STREAM_TYPE_AUDIO][4].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_AUDIO][4].name = "Audio1",
	[E_STILL_STREAM_TYPE_H264][4].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_H264][4].name = "H2651",
};


static volatile T_STILL_MOV_READY_STAT	g_movie_ready_status_temp[E_STILL_STREAM_TYPE_MAX][D_BF_INSTANCE_ID_MAX_VIDEO_ENCODE] = {
	[E_STILL_STREAM_TYPE_AUDIO][0].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_AUDIO][0].name = "Audio0",
	[E_STILL_STREAM_TYPE_H264][0].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_H264][0].name = "H2640",
	[E_STILL_STREAM_TYPE_AUDIO][1].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_AUDIO][1].name = "Audio1",
	[E_STILL_STREAM_TYPE_H264][1].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_H264][1].name = "H2641",
	[E_STILL_STREAM_TYPE_AUDIO][2].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_AUDIO][2].name = "Audio2",
	[E_STILL_STREAM_TYPE_H264][2].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_H264][2].name = "H2642",
	[E_STILL_STREAM_TYPE_AUDIO][3].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_AUDIO][3].name = "Audio0",
	[E_STILL_STREAM_TYPE_H264][3].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_H264][3].name = "H2650",
	[E_STILL_STREAM_TYPE_AUDIO][4].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_AUDIO][4].name = "Audio1",
	[E_STILL_STREAM_TYPE_H264][4].stat = E_STILL_MOVIE_READY_IDLE,
	[E_STILL_STREAM_TYPE_H264][4].name = "H2651",
};

static volatile UCHAR g_still_mov_ctrl_mov_common_notify_callback_set_idle_flag = 0;
static volatile UCHAR g_still_mov_ctrl_mov_common_notify_callback_set_idle_flag_need_lock = 1;

/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/

#if 0
//
// Common function.
//
static VOID still_mov_ctrl_set_prev_mode_is_view( const UCHAR instance_id, const E_STILL_STREAM_TYPE stream_type, const BOOL prev_mode_is_view )
{
	g_movie_ready_status[stream_type][instance_id].prev_mode_is_view = prev_mode_is_view;
}
#endif


//
// Movie codec control.
//

#if 0	// Not Use
/**
 * Mode change callback.
 */
static VOID still_mov_ctrl_mode_change_notify_callback(FJ_MODE mode, E_BF_MOVIE_COMMON_NOTIFIER_TYPE type)
{
	if(type >= E_BF_MOVIE_COMMON_NOTIFIER_TYPE_MAX){
		BF_Debug_Print_Information(( "Still_View_Mode_Change_Notify_CallBack: Param ERR\n" ));
	}

	switch (type){
		case E_BF_MOVIE_COMMON_NOTIFIER_START:
		case E_BF_MOVIE_COMMON_NOTIFIER_STOP:
		case E_BF_MOVIE_COMMON_NOTIFIER_ERR:
		default:										//fall through
			break;
	}
	//Set next mode
	BF_Mode_Set_Next_Sysmode(mode);
	if(mode != FJ_MODE_IDLE){
		BF_Mode_Update_Sysmode_Change();
		BF_Mode_Set_Sr1( FJ_SR1_SUCCESS );
		FJ_HostPostMessage( FJ_HM_NEW_MODE, (ULONG)mode);
	}
	else {
		if (g_still_mov_ctrl_mov_common_notify_callback_set_idle_flag_need_lock == 1){
			OS_User_Wai_Sem( SID_HOST_IMG_PROC );
		}
		g_still_mov_ctrl_mov_common_notify_callback_set_idle_flag = 1;
		if (g_still_mov_ctrl_mov_common_notify_callback_set_idle_flag_need_lock == 1){
			OS_User_Sig_Sem( SID_HOST_IMG_PROC );
		}
	}
}
#endif

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

// FJ mode change to Movie/Audio start inside View mode.
// Call from BaseFW task when detect mode change to FJ_MODE_CAPTURE_AUDIO and FJ_MODE_CAPTURE_H264_xxxx.
VOID BF_StillMovCtrl_Start( const UCHAR instance_id, const BOOL current_is_view )
{
//	ULLONG audio_start_time;

//	BF_Movie_Common_Timer_Start_View();
//	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "RTstart" ));

//	printf("MovCtrl counter[%llu]\n", BF_Movie_Common_Timer_Counter_Get());

	OS_User_Wai_Sem( SID_HOST_IMG_PROC );

#if 0
	// Start Audio
	if( (next_mode & FJ_MODE_CAPTURE_AUDIO) == FJ_MODE_CAPTURE_AUDIO ) {
//		audio_start_time = BF_Get_Time_us();
		audio_start_time = BF_Movie_Common_Timer_Counter_Get();
//		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "RTCac:%llu", audio_start_time ));
		
		still_mov_ctrl_set_prev_mode_is_view( instance_id, E_STILL_STREAM_TYPE_AUDIO, current_is_view );
		still_mov_ctrl_set_movie_ready_status( instance_id, E_STILL_STREAM_TYPE_AUDIO, E_STILL_MOVIE_READY_READ );
		still_mov_ctrl_set_start_time( instance_id, E_STILL_STREAM_TYPE_AUDIO, audio_start_time );
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "Audio read" ));
//		printf( "Audio read %lu\n", audio_start_time );
		g_still_mov_ctrl_mov_common_notify_callback_set_idle_flag_need_lock = 0;
//		BF_Audio_If_Stream_Start(D_AUDIO_STREAM_REC);
		g_still_mov_ctrl_mov_common_notify_callback_set_idle_flag_need_lock = 1;
	}
#endif
	//Start H264
	g_movie_ready_status_temp[E_STILL_STREAM_TYPE_H264][instance_id].stat = E_STILL_MOVIE_READY_GETADDR;

	OS_User_Sig_Sem( SID_HOST_IMG_PROC );
}

VOID BF_StillMovCtrl_Stop_OnIdleMode( VOID )
{
	for( UCHAR i = 0; i < D_BF_INSTANCE_ID_MAX_VIDEO_ENCODE; i++ ) {
		g_movie_ready_status_temp[E_STILL_STREAM_TYPE_H264][i].stat = E_STILL_MOVIE_READY_IDLE;
	}
}
