/**
 * @file		h265_if_common.h
 * @brief		H.265 movie function common Host I/F header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _H265_IF_COMMON_H_
#define _H265_IF_COMMON_H_


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Movie Record Status */
typedef enum{	
	E_H265_IF_RECORD_STATUS_END = 0,
	E_H265_IF_RECORD_STATUS_INIT,
	E_H265_IF_RECORD_STATUS_RUNNING,
} E_H265_IF_RECORD_STATUS;

/* Movie PLAY Status */
typedef enum{	
	E_H265_IF_PLAY_STATUS_END = 0,
	E_H265_IF_PLAY_STATUS_INIT,
	E_H265_IF_PLAY_STATUS_PLAY_NOR,
	E_H265_IF_PLAY_STATUS_PLAY_PAUSE,
	E_H265_IF_PLAY_STATUS_PLAY_FLUSH,
	E_H265_IF_PLAY_STATUS_PLAY_STOP,
	E_H265_IF_PLAY_STATUS_PLAY_DISPTOP,
} E_H265_IF_PLAY_STATUS;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
// [h265_if_record.c]
extern	VOID					H265_If_Record_Callback( E_H265_IF_TYPE type, INT32 param );
extern	VOID					H265_If_Record_Notify_NORMAL_END( VOID );
extern	VOID					H265_If_Record_Notify_FATAL_ERR( VOID );
extern	VOID					H265_If_Record_Notify_STOP_COMPLETE( VOID );
extern	VOID					H265_If_Record_Notify_STATE_CHANGE_COMPLETE( INT32 result );
extern	VOID					H265_Record_Controller( T_H265_IF_MSG* in_msg );
extern	BOOL					H265_If_Record_Notify_Video_Stream( E_H265_IF_STREAM_CH stream_ch, T_H265_IF_VIDEO_STREAM_PARAM* vstream_info );
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
extern	BOOL					H265_If_Record_Notify_Vector_Info( T_H265_IF_VIDEO_VECTOR_PARAM* vector_info );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
extern	BOOL					H265_If_Record_Notify_PreEncode( E_H265_IF_STREAM_CH stream_ch, INT32 *vop_cnt);
extern	BOOL					H265_If_Record_Notify_RepeatFrame( E_H265_IF_STREAM_CH stream_ch );


// [h265_if_record_api.c]
extern	VOID					H265_If_Record_Api_Set_Status( E_H265_IF_RECORD_STATUS status );
extern	E_H265_IF_RECORD_STATUS	H265_If_Record_Api_Get_Status( VOID );
extern	VOID					H265_If_Record_Api_Initialize( VOID );
extern	VOID					H265_If_Record_Api_Finalize( VOID );
extern	INT32					H265_If_Record_Api_Param_Set( T_H265_IF_MSG* msg );
extern	INT32					H265_If_Record_Api_Ready( T_H265_IF_MSG* msg );
extern	INT32					H265_If_Record_Api_Stop( T_H265_IF_MSG* msg );
extern	INT32					H265_If_Record_Api_Check_End( VOID );

// [h265_if_play.c]
extern	VOID					H265_If_Play_Callback( E_H265_IF_TYPE type, INT32 param );
extern	VOID					H265_If_Play_Notify_STOP_COMPLETE( VOID );
extern	VOID					H265_If_Play_Notify_STATE_CHANGE_COMPLETE( INT32 result );
extern	VOID					H265_If_Play_Notify_LCD_UPDATE( UINT32* y_addr, UINT32* c_addr, BOOL update );
extern	VOID					H265_If_Play_Notify_TV_UPDATE( ULONG* y_addr, ULONG* c_addr );
extern	VOID					H265_If_Play_Notify_HDMI_UPDATE( UINT32* y_addr, UINT32* c_addr, BOOL update );
extern	VOID					H265_If_Play_Event_Check( T_H265_IF_MSG* msg );
extern	VOID					H265_Play_Controller( T_H265_IF_MSG* in_msg );

// [h265_if_play_api.c]
extern	INT32					H265_If_Play_Api_Start_Init( T_H265_IF_MSG* msg );
extern	VOID					H265_If_Play_Api_Set_Status( E_H265_IF_PLAY_STATUS status );
extern	E_H265_IF_PLAY_STATUS	H265_If_Play_Api_Get_Status( VOID );
extern	VOID					H265_If_Play_Api_Set_Last_Picture_Keep( BOOL last_picture_keep );
extern	BOOL					H265_If_Play_Api_Get_Last_Picture_Keep( VOID );
extern	INT32					H265_If_Play_Api_Check_Event( VOID );
extern	INT32					H265_If_Play_Api_Param_Check_Init( T_H265_IF_MSG* msg );
extern	INT32					H265_If_Play_Api_Param_Check_Topimage( T_H265_IF_MSG* msg );
extern	INT32					H265_If_Play_Api_Param_Check_Play( T_H265_IF_MSG* msg );
extern	VOID					H265_If_Play_Api_Initialize( VOID );
extern	VOID					H265_If_Play_Api_Finalize( VOID );
extern	INT32					H265_If_Play_Api_TopImage_Get( T_H265_IF_MSG* msg );
extern	INT32					H265_If_Play_Api_Start( T_H265_IF_MSG* msg );

#endif
