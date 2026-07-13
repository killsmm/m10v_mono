/**
 * @file		h265_if_play.c
 * @brief		This file is a movie play controller function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "h265_if.h"
#include "h265_if_common.h"
#include "h265_movie.h"

#ifdef CO_H265_SECTION_ON
//#pragma arm section code="H265_CODE"
//#pragma arm section rodata="H265_CONST"
//#pragma arm section rwdata="H265_INIT"
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
static	void (*volatile gH265_If_Play_CallBack)(int, int) = NULL;


/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static	VOID	h265_if_play_task_pri_set( VOID );
static	VOID	h265_if_play_task_pri_reset( VOID );

static	INT32	h265_if_play_type_initialize( T_H265_IF_MSG* msg );
static	INT32	h265_if_play_type_finalize( T_H265_IF_MSG* msg );
static	INT32	h265_if_play_type_start( T_H265_IF_MSG* msg );
static	INT32	h265_if_play_type_resume( T_H265_IF_MSG* msg );
static	INT32	h265_if_play_type_stop( T_H265_IF_MSG* msg );
static	INT32	h265_if_play_type_pause( T_H265_IF_MSG* msg );
static	INT32	h265_if_play_type_flush( T_H265_IF_MSG* msg );
static	INT32	h265_if_play_type_topimage( T_H265_IF_MSG* msg );

static	VOID	h265_if_play_msg_handler( T_H265_IF_MSG* msg );
static	VOID	h265_if_play_event_msg_handler( T_H265_IF_MSG* msg );


/*----------------------------------------------------------------------*/
/* Movie -> Host function call/callback IF								*/
/*----------------------------------------------------------------------*/
/**
 * Movie Play CallBack Process
 * @param	type	I/F type
 * @param	param	I/F result
 * @return	nothing
 */
VOID H265_If_Play_Callback( E_H265_IF_TYPE type, INT32 param )
{
	H265_Info_Print(("<MOVIE DEC TASK> CALLBACK::%x,result=%d\n", type, param));
	
	if (gH265_If_Play_CallBack != NULL){ gH265_If_Play_CallBack( type, param ); }
	
	return;
}

/**
 * Stop Complete Message.
 * @param	nothing
 * @return	nothing
 */
VOID H265_If_Play_Notify_STOP_COMPLETE( VOID )
{
	H265_Info_Print(("<MOVIE DEC TASK> STOP COMPLETE\n"));
	Host_H265_Notify_Stop_Complete();
	
	return;
}

/**
 * State Change Complete Message.
 * @param	result  : Processing result
 *                    H265_OK  : Normal
 *                    other    : Abnormal
 * @return	nothing
 */
VOID H265_If_Play_Notify_STATE_CHANGE_COMPLETE( INT32 result )
{
	H265_Info_Print(("<MOVIE DEC TASK> STATE CHANGE COMPLETE\n"));
	Host_H265_Notify_State_Change_Complete( result );
	
	return;
}

/**
 * LCD Address Get Message.
 * @param	nothing
 * @return	nothing
 */
VOID H265_If_Play_Notify_LCD_UPDATE( UINT32* y_addr, UINT32* c_addr, BOOL update )
{
//	H265_Info_Print(("<MOVIE DEC TASK> LCD_ADDRESS_GET\n"));
	Host_H265_Notify_Lcd_Update( y_addr, c_addr, update );
	
	return;
}

/**
 * TV Address Get Message.
 * @param	nothing
 * @return	nothing
 */
VOID H265_If_Play_Notify_TV_UPDATE( ULONG* y_addr, ULONG* c_addr )
{
#if 0	// Currently unused.
//	H265_Info_Print(("<MOVIE DEC TASK> TV_ADDRESS_GET\n"));
	Host_H265_Notify_Tv_Update( y_addr, c_addr );
#endif	// Currently unused.
	
	return;
}

/**
 * HDMI Address Get Message.
 * @param	nothing
 * @return	nothing
 */
VOID H265_If_Play_Notify_HDMI_UPDATE( UINT32* y_addr, UINT32* c_addr, BOOL update )
{
//	H265_Info_Print(("<MOVIE DEC TASK> HDMI_ADDRESS_GET\n"));
	Host_H265_Notify_Hdmi_Update( y_addr, c_addr, update);
	
	return;
}


/*----------------------------------------------------------------------*/
/* Host -> Movie function call IF										*/
/*----------------------------------------------------------------------*/
/**
 * Topimage get API (function call version).
 * @param	nothing
 * @return	nothing
 */
INT32 H265_If_Get_Play_Topimage( T_H265_IF_MSG* msg )
{
	return h265_if_play_type_topimage(msg);
}

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 * Task priority changing process during Movie Playing.
 * @param	nothing
 * @return	nothing
 */
static VOID h265_if_play_task_pri_set( VOID )
{
	OS_User_Chg_Pri(TID_H265_CONTROLLER, H265_TASK_PRI_DEC_MAIN);
	
	return;
}

/**
 * Task priority recovery process after Movie Play
 * @param	nothing
 * @return	nothing
 */
static VOID h265_if_play_task_pri_reset( VOID )
{
	OS_User_Chg_Pri(TID_H265_CONTROLLER, D_OS_USER_TPRI_INI);
	
	return;
}


/*----------------------------------------------------------------------*/
/* Main Interface Function												*/
/*----------------------------------------------------------------------*/
/**
 * Initialization Process.
 * @param	msg		no means
 * @return	result
 */
static INT32 h265_if_play_type_initialize( T_H265_IF_MSG* msg )
{
	INT32	ret;
	
	/* It checks current status */
	switch( H265_If_Play_Api_Get_Status() ){
		case E_H265_IF_PLAY_STATUS_END:
			break;
		default:
			return H265_PARAM_ERR;
	}
	/* It executes Initialization process */
	H265_If_Play_Api_Initialize();
	
	/* It checks input parameters */
	ret = H265_If_Play_Api_Param_Check_Init( msg );
	if( ret != H265_OK ){
		return ret;
	}
	
	/* It changes Task priority for Movie Playing */
	h265_if_play_task_pri_set();
	
	/* It executes movie decode play start process. */
	/* Loop has been done while Movie Play is being done in the following functions.  */
	ret = H265_If_Play_Api_Start_Init( msg );
	if( ret != H265_OK ){
		return ret;
	}
	
	return H265_OK;
}

/**
 * Finalization Process.
 * @param	msg		no means
 * @return	result
 */
static INT32 h265_if_play_type_finalize( T_H265_IF_MSG* msg )
{
	/* It checks current status */
	switch( H265_If_Play_Api_Get_Status() ){
		case E_H265_IF_PLAY_STATUS_END:
		case E_H265_IF_PLAY_STATUS_INIT:
		case E_H265_IF_PLAY_STATUS_PLAY_STOP:
		case E_H265_IF_PLAY_STATUS_PLAY_DISPTOP:
			break;
		default:
			return H265_PARAM_ERR;
	}
	/* It executes Finalize process */
	H265_If_Play_Api_Finalize();
	/* It recovers Task priority for Normal PlayBack */
	h265_if_play_task_pri_reset();
	/* It set "End Status" to current status */
	H265_If_Play_Api_Set_Status( E_H265_IF_PLAY_STATUS_END );
	
	return H265_OK;
}

/**
 * Movie Play Start Process.
 * @param	msg		please refer to T_H265_IF_PARAM_PLAY.
 * @return	result
 */
static INT32 h265_if_play_type_start( T_H265_IF_MSG* msg )
{
	INT32	ret;
	
	/* It checks current status */
	switch( H265_If_Play_Api_Get_Status() ){
		case E_H265_IF_PLAY_STATUS_INIT:
		case E_H265_IF_PLAY_STATUS_PLAY_FLUSH:
			/* It checks input parameters */
			ret = H265_If_Play_Api_Param_Check_Play( msg );
			if( ret != H265_OK ){
				return H265_PARAM_ERR;
			}
			
			ret = H265_If_Play_Api_Start( msg );
			if( ret != H265_OK ){
				return H265_PARAM_ERR;
			}
			
			if( msg->data.pPlayParam->pause == FALSE ){
				H265_If_Play_Api_Set_Status( E_H265_IF_PLAY_STATUS_PLAY_NOR );
			}
			else{
				H265_If_Play_Api_Set_Status( E_H265_IF_PLAY_STATUS_PLAY_PAUSE );
			}
			break;
		default:
			return H265_PARAM_ERR;
	}
	
	return H265_OK;
}

/**
 * Movie Replay Process.
 * @param	msg		no means
 * @return	result
 */
static INT32 h265_if_play_type_resume( T_H265_IF_MSG* msg )
{
	/* It checks current status */
	switch( H265_If_Play_Api_Get_Status() ){
		case E_H265_IF_PLAY_STATUS_PLAY_PAUSE:
			break;
		default:
			return H265_PARAM_ERR;
	}
	/* It set "Normal Play" to current status */
	H265_If_Play_Api_Set_Status( E_H265_IF_PLAY_STATUS_PLAY_NOR );
	
	return H265_OK;
}

/**
 * Movie Play Stop Process.
 * @param	msg		no means
 * @return	result
 */
static INT32 h265_if_play_type_stop( T_H265_IF_MSG* msg )
{
	/* It checks current status */
	switch( H265_If_Play_Api_Get_Status() ){
		case E_H265_IF_PLAY_STATUS_INIT:
		case E_H265_IF_PLAY_STATUS_PLAY_NOR:
		case E_H265_IF_PLAY_STATUS_PLAY_PAUSE:
		case E_H265_IF_PLAY_STATUS_PLAY_FLUSH:
			break;
		default:
			return H265_PARAM_ERR;
	}
	
	// Set last picture keep setting.
	H265_If_Play_Api_Set_Last_Picture_Keep( msg->data.pPlayStopParam->last_picture_keep );
	
	/* It set "Play Stop" to current status */
	H265_If_Play_Api_Set_Status( E_H265_IF_PLAY_STATUS_PLAY_STOP );
	
	return H265_OK;
}

/**
 * Movie Play Pause Process.
 * @param	msg		no means
 * @return	result
 */
static INT32 h265_if_play_type_pause( T_H265_IF_MSG* msg )
{
	/* It checks current status */
	switch( H265_If_Play_Api_Get_Status() ){
		case E_H265_IF_PLAY_STATUS_PLAY_NOR:
			break;
		default:
			return H265_PARAM_ERR;
	}
	/* It set "Play Pause" to current status */
	H265_If_Play_Api_Set_Status( E_H265_IF_PLAY_STATUS_PLAY_PAUSE );
	
	return H265_OK;
}

/**
 * Movie Play Pause Process.
 * @param	msg		no means
 * @return	result
 */
static INT32 h265_if_play_type_flush( T_H265_IF_MSG* msg )
{
	/* It checks current status */
	switch( H265_If_Play_Api_Get_Status() ){
		case E_H265_IF_PLAY_STATUS_PLAY_NOR:
		case E_H265_IF_PLAY_STATUS_PLAY_PAUSE:
			break;
		default:
			return H265_PARAM_ERR;
	}
	/* It set "Play Flush" to current status */
	H265_If_Play_Api_Set_Status( E_H265_IF_PLAY_STATUS_PLAY_FLUSH );
	
	return H265_OK;
}


/**
 * Topimage Get Process.
 * @param	msg		please refer to T_H265_IF_PARAM_TOPIMAGE.
 * @return	result
 */
static INT32 h265_if_play_type_topimage( T_H265_IF_MSG* msg )
{
	INT32	ret;
	
	/* It checks current status */
	switch( H265_If_Play_Api_Get_Status() ){
		case E_H265_IF_PLAY_STATUS_INIT:
			break;
		default:
			return H265_PARAM_ERR;
	}
	/* It checks input parameters */
	ret = H265_If_Play_Api_Param_Check_Topimage( msg );
	if( ret != H265_OK ){
		return ret;
	}
	
	/* It executes Topimage get process. */
	ret = H265_If_Play_Api_TopImage_Get( msg );
	
	// Stop Decoder
	OS_User_Set_Flg( FID_H265, FLG_H265_STOP );
	OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_DEC );	// Put the dummy sync in order to stop processing the DISP task.
	
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
const T_H265_IF_HANDLER gH265_If_Play_HandlerTable[] = {
	{	E_H265_IF_TYPE_PLAY_INITIALIZE,			h265_if_play_type_initialize,		"E_H265_IF_TYPE_PLAY_INITIALIZE",		1	},
	{	E_H265_IF_TYPE_PLAY_FINALIZE,			h265_if_play_type_finalize,			"E_H265_IF_TYPE_PLAY_FINALIZE",			1	},
	{	E_H265_IF_TYPE_PLAY_START,				h265_if_play_type_start,			"E_H265_IF_TYPE_PLAY_START",			1	},
	{	E_H265_IF_TYPE_PLAY_RESUME,				h265_if_play_type_resume,			"E_H265_IF_TYPE_PLAY_RESUME",			1	},
	{	E_H265_IF_TYPE_PLAY_STOP,				h265_if_play_type_stop,				"E_H265_IF_TYPE_PLAY_STOP",				1	},
	{	E_H265_IF_TYPE_PLAY_PAUSE,				h265_if_play_type_pause,			"E_H265_IF_TYPE_PLAY_PAUSE",			1	},
	{	E_H265_IF_TYPE_PLAY_FLUSH,				h265_if_play_type_flush,			"E_H265_IF_TYPE_PLAY_FLUSH",			1	},
	{	E_H265_IF_TYPE_PLAY_TOPIMAGE, 			h265_if_play_type_topimage,			"E_H265_IF_TYPE_PLAY_TOPIMAGE",			1	},
};

/**
 * Movie Play Message Handller during "not play" Movie.
 * @param	msg		no means
 * @return	result
 */
static VOID h265_if_play_msg_handler( T_H265_IF_MSG* msg )
{
	int message_loop, message_max, ret;
	
	message_max = E_H265_IF_TYPE_PLAY_LAST - E_H265_IF_TYPE_PLAY_INITIALIZE;
	
	for( message_loop=0; message_loop<message_max; message_loop++ ){
		if(gH265_If_Play_HandlerTable[message_loop].type == msg->type){ break; }
	}
	if( message_loop == message_max ){ return; }
	
	// callback set
	gH265_If_Play_CallBack = (void (*volatile)(int, int))msg->cbfunc;
	
	H265_Info_Print(("\n <MOVIE DEC TASK> Received : %s\n", gH265_If_Play_HandlerTable[message_loop].sztype));
	
	if(( ret = gH265_If_Play_HandlerTable[message_loop].handler(msg) ) < 0 ){
		/* Abnormal end */
		H265_If_Play_Api_Set_Status( E_H265_IF_PLAY_STATUS_END );
		switch( ret ){
			case H265_PARAM_ERR:
				H265_If_Play_Callback( msg->type, H265_PARAM_ERR );
				break;
			default:
				H265_If_Play_Callback( msg->type, H265_FATAL_ERR );
		}
		return;
	}
	else if( gH265_If_Play_HandlerTable[message_loop].bcallback ){
		/* Normal End */
		if( msg->type != E_H265_IF_TYPE_PLAY_START ){
			H265_If_Play_Callback( msg->type, H265_OK );
		}
	}
	
	return;
}

/**
 * Movie Play Message Handller during Movie "playing".
 * @param	msg		no means
 * @return	result
 */
static VOID h265_if_play_event_msg_handler( T_H265_IF_MSG* msg )
{
	int message_loop, message_max, ret;
	
	message_max = E_H265_IF_TYPE_PLAY_LAST - E_H265_IF_TYPE_PLAY_INITIALIZE;
	
	for( message_loop=0; message_loop<message_max; message_loop++ ){
		if( gH265_If_Play_HandlerTable[message_loop].type == msg->type ){ break; }
	}
	if( message_loop == message_max ){
		msg->type = E_H265_IF_TYPE_NO_REQUEST;
		return;
	}
	
	// callback set
	gH265_If_Play_CallBack = (void (*volatile)(int, int))msg->cbfunc;
	
	H265_Info_Print(("\n <MOVIE DEC TASK> Received : %s\n", gH265_If_Play_HandlerTable[message_loop].sztype));
	if(( ret = gH265_If_Play_HandlerTable[message_loop].handler(msg) ) < 0 ){
		switch( ret ){
			case H265_PARAM_ERR:
				H265_If_Play_Callback( msg->type, H265_PARAM_ERR );
				break;
			default:
				H265_If_Play_Callback( msg->type, H265_FATAL_ERR );
		}
		msg->type = E_H265_IF_TYPE_NO_REQUEST;
		return;
	}
	
	return;
}

/**
 * Movie Play Event Check Process on Playing.
 * @param	msg		Movie Play Message
 * @return	none
 */
VOID H265_If_Play_Event_Check( T_H265_IF_MSG* msg )
{
	H265_Movie_Memset( msg, 0, sizeof( T_H265_IF_MSG ));
	
	/* Get User Request */
	H265_If_Get_Msg( msg ,E_H265_IF_MSG_WAIT_NONBLOCK );
	
	if( msg->type == E_H265_IF_TYPE_NO_REQUEST ){
		/* No Event */
		return;
	}
	
	h265_if_play_event_msg_handler( msg );
	
	return;
}


/*----------------------------------------------------------------------*/
/* Movie Play controller [TASK]											*/
/*----------------------------------------------------------------------*/
VOID H265_Play_Controller( T_H265_IF_MSG* in_msg )
{
	T_H265_IF_MSG msg;
	
	H265_Movie_Memcpy( &msg, in_msg, sizeof(T_H265_IF_MSG) );
	
	H265_Info_Print(( "+++ Movie Decode controller task is start.\n" ));
	
	while(1){
		// polling designation
		H265_If_Get_Msg( &msg ,E_H265_IF_MSG_WAIT_NONBLOCK );
		
		/* It checks request. if any message is received, it executes according request. */
		if( msg.type != E_H265_IF_TYPE_NO_REQUEST ) {
			h265_if_play_msg_handler( &msg );
			
			// finalize/TopImage -> break&return
			if( (msg.type == E_H265_IF_TYPE_PLAY_FINALIZE	) || 
				(msg.type == E_H265_IF_TYPE_PLAY_TOPIMAGE	) ){
				H265_Movie_Memset( &msg, 0, sizeof( T_H265_IF_MSG ));
				break;
			}
			
			H265_Movie_Memset( &msg, 0, sizeof( T_H265_IF_MSG ));
		}
		
		// wait for dispatch
		OS_User_Dly_Tsk(1);
	}
	
	H265_Info_Print(( "+++ Movie Decode controller task is stop.\n" ));
	
	return;
}
