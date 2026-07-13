/**
 * @file		h264_if_play.c
 * @brief		This file is a movie play controller function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "h264_if.h"
#include "h264_if_common.h"
#include "h264_movie.h"

#ifdef CO_H264_SECTION_ON
//#pragma arm section code="H264_CODE"
//#pragma arm section rodata="H264_CONST"
//#pragma arm section rwdata="H264_INIT"
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static	void (*volatile gCallBack)(int, int) = NULL;

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static	VOID	h264_if_play_task_pri_set( VOID );
static	VOID	h264_if_play_task_pri_reset( VOID );

static	INT32	h264_if_play_type_initialize( T_H264_IF_MSG* msg );
static	INT32	h264_if_play_type_finalize( T_H264_IF_MSG* msg );
static	INT32	h264_if_play_type_start( T_H264_IF_MSG* msg );
static	INT32	h264_if_play_type_resume( T_H264_IF_MSG* msg );
static	INT32	h264_if_play_type_stop( T_H264_IF_MSG* msg );
static	INT32	h264_if_play_type_pause( T_H264_IF_MSG* msg );
static	INT32	h264_if_play_type_flush( T_H264_IF_MSG* msg );
static	INT32	h264_if_play_type_topimage( T_H264_IF_MSG* msg );

static	VOID	h264_if_play_msg_handler( T_H264_IF_MSG* msg );
static	VOID	h264_if_play_event_msg_handler( T_H264_IF_MSG* msg );


/*----------------------------------------------------------------------*/
/* Movie -> Host function call/callback IF								*/
/*----------------------------------------------------------------------*/
/**
 * Movie Play CallBack Process
 * @param	type	I/F type
 * @param	param	I/F result
 * @return	nothing
 */
VOID H264_If_Play_Callback( E_H264_IF_TYPE type, INT32 param )
{
	H264_Info_Print(("<MOVIE DEC TASK> CALLBACK::%x,result=%d\n", type, param));
	
	if (gCallBack != NULL){ gCallBack( type, param ); }
	return ;
}

/**
 * Stop Complete Message.
 * @param	nothing
 * @return	nothing
 */
VOID H264_If_Play_Notify_STOP_COMPLETE( VOID )
{
	H264_Info_Print(("<MOVIE DEC TASK> STOP COMPLETE\n"));
	Host_H264_Notify_Stop_Complete();
	return ;
}

/**
 * State Change Complete Message.
 * @param	result  : Processing result
 *                    H264_OK  : Normal
 *                    other    : Abnormal
 * @return	nothing
 */
VOID H264_If_Play_Notify_STATE_CHANGE_COMPLETE( INT32 result )
{
	H264_Info_Print(("<MOVIE DEC TASK> STATE CHANGE COMPLETE\n"));
	Host_H264_Notify_State_Change_Complete( result );
	return ;
}

/**
 * LCD Address Get Message.
 * @param	nothing
 * @return	nothing
 */
VOID H264_If_Play_Notify_LCD_UPDATE( UINT32* y_addr, UINT32* c_addr, BOOL update )
{
//	H264_Info_Print(("<MOVIE DEC TASK> LCD_ADDRESS_GET\n"));
	Host_H264_Notify_Lcd_Update( y_addr, c_addr, update );
	return ;
}

/**
 * TV Address Get Message.
 * @param	nothing
 * @return	nothing
 */
VOID H264_If_Play_Notify_TV_UPDATE( ULONG* y_addr, ULONG* c_addr )
{
#if 0	// Currently unused.
//	H264_Info_Print(("<MOVIE DEC TASK> TV_ADDRESS_GET\n"));
	Host_H264_Notify_Tv_Update( y_addr, c_addr );
#endif	// Currently unused.
	return ;
}

/**
 * HDMI Address Get Message.
 * @param	nothing
 * @return	nothing
 */
VOID H264_If_Play_Notify_HDMI_UPDATE( UINT32* y_addr, UINT32* c_addr, BOOL update )
{
//	H264_Info_Print(("<MOVIE DEC TASK> HDMI_ADDRESS_GET\n"));
	Host_H264_Notify_Hdmi_Update( y_addr, c_addr, update );
	return ;
}


/*----------------------------------------------------------------------*/
/* Host -> Movie function call IF										*/
/*----------------------------------------------------------------------*/
/**
 * Topimage get API (function call version).
 * @param	nothing
 * @return	nothing
 */
INT32 H264_If_Get_Play_Topimage( T_H264_IF_MSG* msg )
{
	return h264_if_play_type_topimage(msg);
}

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 * Task priority changing process during Movie Playing.
 * @param	nothing
 * @return	nothing
 */
static VOID h264_if_play_task_pri_set( VOID )
{
	OS_User_Chg_Pri(TID_H264_CONTROLLER, H264_TASK_PRI_DEC_MAIN);
	return ;
}

/**
 * Task priority recovery process after Movie Play
 * @param	nothing
 * @return	nothing
 */
static VOID h264_if_play_task_pri_reset( VOID )
{
	OS_User_Chg_Pri(TID_H264_CONTROLLER, D_OS_USER_TPRI_INI);
	return ;
}


/*----------------------------------------------------------------------*/
/* Main Interface Function												*/
/*----------------------------------------------------------------------*/
/**
 * Initialization Process.
 * @param	msg		no means
 * @return	result
 */
static INT32 h264_if_play_type_initialize( T_H264_IF_MSG* msg )
{
	INT32	ret;
	
	/* It checks current status */
	switch( H264_If_Play_Api_Get_Status() ){
		case E_H264_IF_PLAY_STATUS_END:
			break;
		default:
			return H264_PARAM_ERR;
	}
	/* It executes Initialization process */
	H264_If_Play_Api_Initialize();
	
	/* It checks input parameters */
	ret = H264_If_Play_Api_Param_Check_Init( msg );
	if( ret != H264_OK ){
		return ret;
	}
	
	/* It changes Task priority for Movie Playing */
	h264_if_play_task_pri_set();
	
	/* It executes movie decode play start process. */
	/* Loop has been done while Movie Play is being done in the following functions.  */
	ret = H264_If_Play_Api_Start_Init( msg );
	if( ret != H264_OK ){
		return ret;
	}
	
	return H264_OK;
}

/**
 * Finalization Process.
 * @param	msg		no means
 * @return	result
 */
static INT32 h264_if_play_type_finalize( T_H264_IF_MSG* msg )
{
	/* It checks current status */
	switch( H264_If_Play_Api_Get_Status() ){
		case E_H264_IF_PLAY_STATUS_END:
		case E_H264_IF_PLAY_STATUS_INIT:
		case E_H264_IF_PLAY_STATUS_PLAY_STOP:
		case E_H264_IF_PLAY_STATUS_PLAY_DISPTOP:
			break;
		default:
			return H264_PARAM_ERR;
	}
	/* It executes Finalize process */
	H264_If_Play_Api_Finalize();
	/* It recovers Task priority for Normal PlayBack */
	h264_if_play_task_pri_reset();
	/* It set "End Status" to current status */
	H264_If_Play_Api_Set_Status( E_H264_IF_PLAY_STATUS_END );
	
	return H264_OK;
}

/**
 * Movie Play Start Process.
 * @param	msg		please refer to T_H264_IF_PARAM_PLAY.
 * @return	result
 */
static INT32 h264_if_play_type_start( T_H264_IF_MSG* msg )
{
	INT32	ret;
	
	/* It checks current status */
	switch( H264_If_Play_Api_Get_Status() ){
		case E_H264_IF_PLAY_STATUS_INIT:
		case E_H264_IF_PLAY_STATUS_PLAY_FLUSH:
			/* It checks input parameters */
			ret = H264_If_Play_Api_Param_Check_Play( msg );
			if( ret != H264_OK ){
				return H264_PARAM_ERR;
			}
			
			ret = H264_If_Play_Api_Start( msg );
			if( ret != H264_OK ){
				return H264_PARAM_ERR;
			}
			
			if( msg->data.pPlayParam->pause == FALSE ){
				H264_If_Play_Api_Set_Status( E_H264_IF_PLAY_STATUS_PLAY_NOR );
			}
			else{
				H264_If_Play_Api_Set_Status( E_H264_IF_PLAY_STATUS_PLAY_PAUSE );
			}
			break;
		default:
			return H264_PARAM_ERR;
	}
	return H264_OK;
}

/**
 * Movie Replay Process.
 * @param	msg		no means
 * @return	result
 */
static INT32 h264_if_play_type_resume( T_H264_IF_MSG* msg )
{
	/* It checks current status */
	switch( H264_If_Play_Api_Get_Status() ){
		case E_H264_IF_PLAY_STATUS_PLAY_PAUSE:
			break;
		default:
			return H264_PARAM_ERR;
	}
	/* It set "Normal Play" to current status */
	H264_If_Play_Api_Set_Status( E_H264_IF_PLAY_STATUS_PLAY_NOR );
	
	return H264_OK;
}

/**
 * Movie Play Stop Process.
 * @param	msg		no means
 * @return	result
 */
static INT32 h264_if_play_type_stop( T_H264_IF_MSG* msg )
{
	/* It checks current status */
	switch( H264_If_Play_Api_Get_Status() ){
		case E_H264_IF_PLAY_STATUS_INIT:
		case E_H264_IF_PLAY_STATUS_PLAY_NOR:
		case E_H264_IF_PLAY_STATUS_PLAY_PAUSE:
		case E_H264_IF_PLAY_STATUS_PLAY_FLUSH:
			break;
		default:
			return H264_PARAM_ERR;
	}
	
	// Set last picture keep setting.
	H264_If_Play_Api_Set_Last_Picture_Keep( msg->data.pPlayStopParam->last_picture_keep );
	
	/* It set "Play Stop" to current status */
	H264_If_Play_Api_Set_Status( E_H264_IF_PLAY_STATUS_PLAY_STOP );
	
	return H264_OK;
}

/**
 * Movie Play Pause Process.
 * @param	msg		no means
 * @return	result
 */
static INT32 h264_if_play_type_pause( T_H264_IF_MSG* msg )
{
	/* It checks current status */
	switch( H264_If_Play_Api_Get_Status() ){
		case E_H264_IF_PLAY_STATUS_PLAY_NOR:
			break;
		default:
			return H264_PARAM_ERR;
	}
	/* It set "Play Pause" to current status */
	H264_If_Play_Api_Set_Status( E_H264_IF_PLAY_STATUS_PLAY_PAUSE );
	
	return H264_OK;
}

/**
 * Movie Play Pause Process.
 * @param	msg		no means
 * @return	result
 */
static INT32 h264_if_play_type_flush( T_H264_IF_MSG* msg )
{
	/* It checks current status */
	switch( H264_If_Play_Api_Get_Status() ){
		case E_H264_IF_PLAY_STATUS_PLAY_NOR:
		case E_H264_IF_PLAY_STATUS_PLAY_PAUSE:
			break;
		default:
			return H264_PARAM_ERR;
	}
	/* It set "Play Flush" to current status */
	H264_If_Play_Api_Set_Status( E_H264_IF_PLAY_STATUS_PLAY_FLUSH );
	
	return H264_OK;
}


/**
 * Topimage Get Process.
 * @param	msg		please refer to T_H264_IF_PARAM_TOPIMAGE.
 * @return	result
 */
static INT32 h264_if_play_type_topimage( T_H264_IF_MSG* msg )
{
	INT32	ret;
	
	/* It checks current status */
	switch( H264_If_Play_Api_Get_Status() ){
		case E_H264_IF_PLAY_STATUS_INIT:
			break;
		default:
			return H264_PARAM_ERR;
	}
	/* It checks input parameters */
	ret = H264_If_Play_Api_Param_Check_Topimage( msg );
	if( ret != H264_OK ){
		return ret;
	}
	
	/* It executes Topimage get process. */
	ret = H264_If_Play_Api_TopImage_Get( msg );
	
	// Stop Decoder
	OS_User_Set_Flg( FID_H264, FLG_H264_STOP );
	OS_User_Set_Flg( FID_H264_SYNC, FLG_H264_SYNC_DEC );	// Put the dummy sync in order to stop processing the DISP task.
	
	return ret;
}

/******************************************************************************/
/**
 * Movie decode message handler
 * @param stacd task start code
 * @return VOID
 * @attention 
*/
/******************************************************************************/
const T_H264_IF_HANDLER gPlay_HandlerTable[] = {
	{	E_H264_IF_TYPE_PLAY_INITIALIZE,			h264_if_play_type_initialize,		"E_H264_IF_TYPE_PLAY_INITIALIZE",		1	},
	{	E_H264_IF_TYPE_PLAY_FINALIZE,			h264_if_play_type_finalize,			"E_H264_IF_TYPE_PLAY_FINALIZE",			1	},
	{	E_H264_IF_TYPE_PLAY_START,				h264_if_play_type_start,			"E_H264_IF_TYPE_PLAY_START",			1	},
	{	E_H264_IF_TYPE_PLAY_RESUME,				h264_if_play_type_resume,			"E_H264_IF_TYPE_PLAY_RESUME",			1	},
	{	E_H264_IF_TYPE_PLAY_STOP,				h264_if_play_type_stop,				"E_H264_IF_TYPE_PLAY_STOP",				1	},
	{	E_H264_IF_TYPE_PLAY_PAUSE,				h264_if_play_type_pause,			"E_H264_IF_TYPE_PLAY_PAUSE",			1	},
	{	E_H264_IF_TYPE_PLAY_FLUSH,				h264_if_play_type_flush,			"E_H264_IF_TYPE_PLAY_FLUSH",			1	},
	{	E_H264_IF_TYPE_PLAY_TOPIMAGE, 			h264_if_play_type_topimage,			"E_H264_IF_TYPE_PLAY_TOPIMAGE",			1	},
};

/**
 * Movie Play Message Handller during "not play" Movie.
 * @param	msg		no means
 * @return	result
 */
static VOID h264_if_play_msg_handler( T_H264_IF_MSG* msg )
{
	int message_loop, message_max, ret;
	
	message_max = E_H264_IF_TYPE_PLAY_LAST - E_H264_IF_TYPE_PLAY_INITIALIZE;
	
	for( message_loop=0; message_loop<message_max; message_loop++ ){
		if(gPlay_HandlerTable[message_loop].type == msg->type){ break; }
	}
	if( message_loop == message_max ){ return; }
	
	// callback set
#ifndef DEBUG_ON_PC
//	gCallBack = (volatile void (*)(int, int))msg->cbfunc;

//	gCallBack = msg->cbfunc;

	gCallBack = (void (*volatile)(int, int))msg->cbfunc;

#else
	gCallBack = (void (*volatile)(int, int))msg->cbfunc;
#endif	
	H264_Info_Print(("\n <MOVIE DEC TASK> Received : %s\n", gPlay_HandlerTable[message_loop].sztype));
	
	if(( ret = gPlay_HandlerTable[message_loop].handler(msg) ) < 0 ){
		/* Abnormal end */
		H264_If_Play_Api_Set_Status( E_H264_IF_PLAY_STATUS_END );
		switch( ret ){
			case H264_PARAM_ERR:
				H264_If_Play_Callback( msg->type, H264_PARAM_ERR );
				break;
			default:
				H264_If_Play_Callback( msg->type, H264_FATAL_ERR );
		}
		return;
	}
	else if( gPlay_HandlerTable[message_loop].bcallback ){
		/* Normal End */
		if( msg->type != E_H264_IF_TYPE_PLAY_START ){
			H264_If_Play_Callback( msg->type, H264_OK );
		}
	}
	return;
}

/**
 * Movie Play Message Handller during Movie "playing".
 * @param	msg		no means
 * @return	result
 */
static VOID h264_if_play_event_msg_handler( T_H264_IF_MSG* msg )
{
	int message_loop, message_max, ret;
	
	message_max = E_H264_IF_TYPE_PLAY_LAST - E_H264_IF_TYPE_PLAY_INITIALIZE;
	
	for( message_loop=0; message_loop<message_max; message_loop++ ){
		if( gPlay_HandlerTable[message_loop].type == msg->type ){ break; }
	}
	if( message_loop == message_max ){
		msg->type = E_H264_IF_TYPE_NO_REQUEST;
		return;
	}
	
	// callback set
	gCallBack = (void (*volatile)(int, int))msg->cbfunc;
	
	/* Set User Event */
	H264_If_Play_Api_Set_User_Event( msg );
	
	H264_Info_Print(("\n <MOVIE DEC TASK> Received : %s\n", gPlay_HandlerTable[message_loop].sztype));
	if(( ret = gPlay_HandlerTable[message_loop].handler(msg) ) < 0 ){
		switch( ret ){
			case H264_PARAM_ERR:
				H264_If_Play_Callback( msg->type, H264_PARAM_ERR );
				break;
			default:
				H264_If_Play_Callback( msg->type, H264_FATAL_ERR );
		}
		msg->type = E_H264_IF_TYPE_NO_REQUEST;
		return;
	}
	return;
}

/**
 * Movie Play Event Check Process on Playing.
 * @param	msg		Movie Play Message
 * @return	none
 */
VOID H264_If_Play_Event_Check( T_H264_IF_MSG* msg )
{
	H264_Movie_Memset( msg, 0, sizeof( T_H264_IF_MSG ));
	
	/* Get User Request */
	H264_If_Get_Msg( msg ,E_H264_IF_MSG_WAIT_NONBLOCK );
	
	if( msg->type == E_H264_IF_TYPE_NO_REQUEST ){
		/* No Event */
		return;
	}
	
	h264_if_play_event_msg_handler( msg );
	return;
}


/*----------------------------------------------------------------------*/
/* Movie Play controller [TASK]											*/
/*----------------------------------------------------------------------*/
VOID H264_Play_Controller( T_H264_IF_MSG* in_msg )
{
	T_H264_IF_MSG msg;
	
	H264_Movie_Memcpy( &msg, in_msg, sizeof(T_H264_IF_MSG) );
	
	H264_Info_Print(( "+++ Movie Decode controller task is start.\n" ));
	
	while(1){
		// polling designation
		H264_If_Get_Msg( &msg ,E_H264_IF_MSG_WAIT_NONBLOCK );
		
		/* It checks request. if any message is received, it executes according request. */
		if( msg.type != E_H264_IF_TYPE_NO_REQUEST ) {
			h264_if_play_msg_handler( &msg );
			
			// finalize/TopImage -> break&return
			if( (msg.type == E_H264_IF_TYPE_PLAY_FINALIZE	) || 
				(msg.type == E_H264_IF_TYPE_PLAY_TOPIMAGE	) ){
				H264_Movie_Memset( &msg, 0, sizeof( T_H264_IF_MSG ));
				break;
			}
			
			H264_Movie_Memset( &msg, 0, sizeof( T_H264_IF_MSG ));
		}
		
		// wait for dispatch
		OS_User_Dly_Tsk(1);
	}
	
	H264_Info_Print(( "+++ Movie Decode controller task is stop.\n" ));
	
	return;
}
