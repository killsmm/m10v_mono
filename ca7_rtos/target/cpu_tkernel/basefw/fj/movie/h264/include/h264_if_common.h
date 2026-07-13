/**
 * @file		h264_if_common.h
 * @brief		H.264 movie function common Host I/F header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _H264_IF_COMMON_H_
#define _H264_IF_COMMON_H_


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Movie Record Status */
typedef enum{	
	E_H264_IF_RECORD_STATUS_END = 0,
	E_H264_IF_RECORD_STATUS_INIT,
	E_H264_IF_RECORD_STATUS_RUNNING,
} E_H264_IF_RECORD_STATUS;

/* Movie PLAY Status */
typedef enum{	
	E_H264_IF_PLAY_STATUS_END = 0,
	E_H264_IF_PLAY_STATUS_INIT,
	E_H264_IF_PLAY_STATUS_PLAY_NOR,
	E_H264_IF_PLAY_STATUS_PLAY_PAUSE,
	E_H264_IF_PLAY_STATUS_PLAY_FLUSH,
	E_H264_IF_PLAY_STATUS_PLAY_STOP,
	E_H264_IF_PLAY_STATUS_PLAY_DISPTOP,
} E_H264_IF_PLAY_STATUS;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
// [h264_if_record.c]
extern	VOID					H264_If_Record_Callback( E_H264_IF_TYPE type, INT32 param );
extern	VOID					H264_If_Record_Notify_NORMAL_END( VOID );
extern	VOID					H264_If_Record_Notify_VRAW_MULTI_ERR( VOID );
extern	VOID					H264_If_Record_Notify_VSTRM_EMPTY_ERR( VOID );
extern	VOID					H264_If_Record_Notify_FATAL_ERR( VOID );
extern	VOID					H264_If_Record_Notify_STOP_COMPLETE( VOID );
extern	VOID					H264_If_Record_Notify_STATE_CHANGE_COMPLETE( INT32 result );
extern	VOID					H264_Record_Controller( T_H264_IF_MSG* in_msg );
extern	BOOL					H264_If_Record_Notify_Video_Stream(T_H264_IF_VIDEO_STREAM_PARAM* vstream_info);
extern	BOOL					H264_If_Record_Notify_Vector_Info(T_H264_IF_VIDEO_VECTOR_PARAM* vector_info);
extern	BOOL					H264_If_Record_Notify_PreEncode( E_H264_IF_STREAM_CH stream_ch );
extern	BOOL					H264_If_Record_Notify_RepeatFrame( E_H264_IF_STREAM_CH stream_ch );


// [h264_if_record_api.c]
extern	VOID					H264_If_Record_Api_Set_Status( E_H264_IF_RECORD_STATUS status );
extern	E_H264_IF_RECORD_STATUS	H264_If_Record_Api_Get_Status( VOID );
extern	VOID					H264_If_Record_Api_Initialize( VOID );
extern	VOID					H264_If_Record_Api_Finalize( VOID );
extern	INT32					H264_If_Record_Api_Param_Set( T_H264_IF_MSG* msg );
extern	INT32					H264_If_Record_Api_Ready( T_H264_IF_MSG* msg );
extern	INT32					H264_If_Record_Api_Stop( T_H264_IF_MSG* msg );
extern	INT32					H264_If_Record_Api_Check_End( VOID );

// [h264_if_play.c]
extern	VOID					H264_If_Play_Callback( E_H264_IF_TYPE type, INT32 param );
extern	VOID					H264_If_Play_Notify_STOP_COMPLETE( VOID );
extern	VOID					H264_If_Play_Notify_STATE_CHANGE_COMPLETE( INT32 result );
extern	VOID					H264_If_Play_Notify_LCD_UPDATE( UINT32* y_addr, UINT32* c_addr, BOOL update );
extern	VOID					H264_If_Play_Notify_TV_UPDATE( ULONG* y_addr, ULONG* c_addr );
extern	VOID					H264_If_Play_Notify_HDMI_UPDATE( UINT32* y_addr, UINT32* c_addr, BOOL update );
extern	VOID					H264_If_Play_Event_Check( T_H264_IF_MSG* msg );
extern	VOID					H264_Play_Controller( T_H264_IF_MSG* in_msg );

// [h264_if_play_api.c]
extern	INT32					H264_If_Play_Api_Start_Init( T_H264_IF_MSG* msg );
extern	VOID					H264_If_Play_Api_Set_Status( E_H264_IF_PLAY_STATUS status );
extern	E_H264_IF_PLAY_STATUS	H264_If_Play_Api_Get_Status( VOID );
extern	VOID					H264_If_Play_Api_Set_Last_Picture_Keep( BOOL last_picture_keep );
extern	BOOL					H264_If_Play_Api_Get_Last_Picture_Keep( VOID );
extern	VOID					H264_If_Play_Api_Set_User_Event( T_H264_IF_MSG* msg );
extern	INT32					H264_If_Play_Api_Check_Event( VOID );
extern	INT32					H264_If_Play_Api_Param_Check_Init( T_H264_IF_MSG* msg );
extern	INT32					H264_If_Play_Api_Param_Check_Topimage( T_H264_IF_MSG* msg );
extern	INT32					H264_If_Play_Api_Param_Check_Play( T_H264_IF_MSG* msg );
extern	VOID					H264_If_Play_Api_Initialize( VOID );
extern	VOID					H264_If_Play_Api_Finalize( VOID );
extern	INT32					H264_If_Play_Api_TopImage_Get( T_H264_IF_MSG* msg );
extern	INT32					H264_If_Play_Api_Start( T_H264_IF_MSG* msg );

#endif
