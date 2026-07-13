/**
 * @file		h265_if_record.c
 * @brief		This file is a movie record controller api function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "h265_if.h"
#include "h265_if_common.h"
#include "h265_movie.h"

#include "os_user_custom.h"

#ifdef CO_H265_SECTION_ON
//#pragma arm section code="H265_CODE"
//#pragma arm section rodata="H265_CONST"
//#pragma arm section rwdata="H265_INIT"
#endif

#include "mba_if.h"

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
static	void (*volatile gH265_CallBack)(int, int) = NULL;

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static	VOID	h265_if_record_task_pri_set( VOID );
static	VOID	h265_if_record_task_pri_reset( VOID );

static	INT32	h265_if_record_type_initialize( T_H265_IF_MSG* msg );
static	INT32	h265_if_record_type_finalize( T_H265_IF_MSG* msg );
static	INT32	h265_if_record_type_start_video( T_H265_IF_MSG* msg );
static	INT32	h265_if_record_type_stop( T_H265_IF_MSG* msg );
static	VOID	h265_if_record_msg_handler(T_H265_IF_MSG* msg);


/*----------------------------------------------------------------------*/
/* Movie -> Host function call/callback IF								*/
/*----------------------------------------------------------------------*/
/**
 * Movie Record CallBack Process
 * @param	type	I/F type
 * @param	param	I/F result
 * @return	nothing
 */
VOID H265_If_Record_Callback( E_H265_IF_TYPE type, INT32 param )
{
	H265_Info_Print(("<H265 Ctrl TASK> CALLBACK::%x,result=%d\n", type, param));
	
	if( gH265_CallBack != NULL ){ gH265_CallBack(type, param); }
	return;
}

/**
 * Normal End Message.
 * @param	nothing
 * @return	nothing
 */
VOID H265_If_Record_Notify_NORMAL_END( VOID )
{
	H265_Info_Print(("<H265 Ctrl> NORMAL END\n"));
	Host_H265_Notify_Stop_Trigger( E_H265_IF_STOP_NOTIFY_NORMAL_END );
	return;
}

/**
 * Fatal Error Message.
 * @param	nothing
 * @return	nothing
 */
VOID H265_If_Record_Notify_FATAL_ERR( VOID )
{
	H265_Info_Print(("<H265 Ctrl> FATAL ERROR\n"));
	Host_H265_Notify_Stop_Trigger( E_H265_IF_STOP_NOTIFY_FATAL_ERR );
	return;
}

/**
 * Stop Complete Message.
 * @param	nothing
 * @return	nothing
 */
VOID H265_If_Record_Notify_STOP_COMPLETE( VOID )
{
	H265_Info_Print(("<H265 Ctrl> STOP COMPLETE\n"));
	Host_H265_Notify_Stop_Complete();
	return;
}

/**
 * State Change Complete Message.
 * @param	result  : Processing result
 *                    H265_OK  : Normal
 *                    !H265_OK : Abnormal
 * @return	nothing
 */
VOID H265_If_Record_Notify_STATE_CHANGE_COMPLETE( INT32 result )
{
	H265_Info_Print(("<H265 Ctrl> STATE CHANGE COMPLETE\n"));
	Host_H265_Notify_State_Change_Complete( result );
	return;
}

/**
 * Video Stream Information Send Message.
 * @param	vstream_info         : pointer of video stream information
 * @return	TRUE  : Normal
 *			FALSE : Abnormal
 */
BOOL H265_If_Record_Notify_Video_Stream( E_H265_IF_STREAM_CH stream_ch, T_H265_IF_VIDEO_STREAM_PARAM* vstream_info )
{
	//H265_Info_Print(("<H265 Ctrl> VIDEO STREAM SEND\n"));
	return Host_H265_Notify_Video_Stream( stream_ch, vstream_info );
}

// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
/**
 * Video Vector Information Send Message.
 * @param	vectore_info         : pointer of video vector information
 * @return	TRUE  : Normal
 *			FALSE : Abnormal
 */
BOOL H265_If_Record_Notify_Vector_Info( T_H265_IF_VIDEO_VECTOR_PARAM* vector_info )
{
	return Host_H265_Notify_Vector_Info( vector_info );
}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---

/**
 * Video PreEncode Timming Send Message.
 * @param	stream_ch         : pointer of video stream channel
 * @return	TRUE  : Normal
 *			FALSE : Abnormal
 */
BOOL H265_If_Record_Notify_PreEncode( E_H265_IF_STREAM_CH stream_ch, INT32 *vop_cnt )
{
	return Host_H265_Notify_PreEncode( stream_ch, vop_cnt );
}

/**
 * Video RepeatFrame occurred Timming Send Message.
 * @param	stream_ch         : pointer of video stream channel
 * @return	TRUE  : Normal
 *			FALSE : Abnormal
 */
BOOL H265_If_Record_Notify_RepeatFrame( E_H265_IF_STREAM_CH stream_ch )
{
	return Host_H265_Notify_RepeatFrame( stream_ch );
}


/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 * Task priority changing process during Movie Recording.
 * @param	nothing
 * @return	nothing
 */
static VOID h265_if_record_task_pri_set( VOID )
{
	OS_User_Chg_Pri( TID_H265_CONTROLLER, H265_TASK_PRI_ENC_MAIN );
	return;
}

/**
 * Task priority recovery process after Movie Record
 * @param	nothing
 * @return	nothing
 */
static VOID h265_if_record_task_pri_reset( VOID )
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
static INT32 h265_if_record_type_initialize( T_H265_IF_MSG* msg )
{
	INT32 ret;
	
	if( H265_If_Record_Api_Get_Status() == E_H265_IF_RECORD_STATUS_END ){
		/* It executes Initialization process */
		H265_If_Record_Api_Initialize();
		/* It set "Initialization" to current status */
		H265_If_Record_Api_Set_Status( E_H265_IF_RECORD_STATUS_INIT );
	}
	
	/* It keeps input parameters to control data in movie_encode_api.c */
	ret = H265_If_Record_Api_Param_Set( msg );
	if( ret != H265_OK ) {
		if( H265_If_Record_Api_Get_Status() == E_H265_IF_RECORD_STATUS_INIT ){
			H265_If_Record_Api_Set_Status( E_H265_IF_RECORD_STATUS_END );
		}
		return ret;
	}
	/* It executes Ready process */
	ret = H265_If_Record_Api_Ready( msg );
	if( ret != H265_OK ) {
		if( H265_If_Record_Api_Get_Status() == E_H265_IF_RECORD_STATUS_INIT ){
			H265_If_Record_Api_Set_Status( E_H265_IF_RECORD_STATUS_END );
		}
		return ret;
	}
	/* It changes Task priority for Movie Recording */
	h265_if_record_task_pri_set();
	
	return H265_OK;
}

/**
 * Finalization Process.
 * @param	msg		no means
 * @return	result
 */
static INT32 h265_if_record_type_finalize( T_H265_IF_MSG* msg )
{
	/* It executes Finalization process */
	H265_If_Record_Api_Finalize();
	/* It set "End Status" to current status */
	H265_If_Record_Api_Set_Status( E_H265_IF_RECORD_STATUS_END );
	return H265_OK;
}


/**
 * Movie Video Record Start Process
 * @param	msg		no means
 * @return	result
 */
static INT32 h265_if_record_type_start_video( T_H265_IF_MSG* msg )
{
	/* It checks current status */
	if( H265_If_Record_Api_Get_Status() == E_H265_IF_RECORD_STATUS_INIT ){
		/* Record Start (Stop->Record) */
		OS_User_Set_Flg( FID_H265, FLG_H265_START );
		/* It set "Running Status" to current status */
		H265_If_Record_Api_Set_Status( E_H265_IF_RECORD_STATUS_RUNNING );
	}
	else if( H265_If_Record_Api_Get_Status() == E_H265_IF_RECORD_STATUS_RUNNING ){
		// **** entertain ****
	}
	else{
		return H265_PARAM_ERR;
	}
	
	return H265_OK;
}

/**
 * Movie Record Stop Process
 * @param	msg		no means
 * @return	result
 */
static INT32 h265_if_record_type_stop( T_H265_IF_MSG* msg )
{
	INT32	ret;
	
	/* It checks current status */
	switch( H265_If_Record_Api_Get_Status() ){
		case E_H265_IF_RECORD_STATUS_INIT:
		case E_H265_IF_RECORD_STATUS_RUNNING:
			break ;
		default :
			return H265_OK;
	}
	/* It executes STOP process */
	ret = H265_If_Record_Api_Stop( msg );
	
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
const T_H265_IF_HANDLER gH265_Record_HandlerTable[] = {
	{	E_H265_IF_TYPE_RECORD_INITIALIZE,	h265_if_record_type_initialize,		"E_H265_IF_TYPE_RECORD_INITIALIZE",		1	},
	{	E_H265_IF_TYPE_RECORD_FINALIZE,		h265_if_record_type_finalize,		"E_H265_IF_TYPE_RECORD_FINALIZE",		1	},
	{	E_H265_IF_TYPE_RECORD_START_VIDEO,	h265_if_record_type_start_video,	"E_H265_IF_TYPE_RECORD_START_VIDEO",	1	},
	{	E_H265_IF_TYPE_RECORD_STOP,			h265_if_record_type_stop,			"E_H265_IF_TYPE_RECORD_STOP",			1	},
};

/**
 * Movie Record Message Handller
 * @param	msg		no means
 * @return	result
 */
static VOID h265_if_record_msg_handler( T_H265_IF_MSG* msg )
{
	int i, max, ret;
	
	max = E_H265_IF_TYPE_RECORD_LAST - E_H265_IF_TYPE_RECORD_INITIALIZE;
	
	for( i=0; i<max; i++ ){
		if(gH265_Record_HandlerTable[i].type == msg->type){
			break;
		}
	}
	if( i == max ){ return; }
	
	// callback set
	gH265_CallBack = (void (*volatile)(int, int))msg->cbfunc;
	
	H265_Info_Print(("<H265 Ctrl> Received : %s\n", gH265_Record_HandlerTable[i].sztype));
	if(( ret = gH265_Record_HandlerTable[i].handler(msg) ) < 0 ){
		/* Abnormal end */
		H265_If_Record_Api_Set_Status( E_H265_IF_RECORD_STATUS_END );
		/* Response to demand */
		switch( ret ){
			case H265_PARAM_ERR:
				H265_If_Record_Callback( msg->type, H265_PARAM_ERR );
				break;
			default:
				H265_If_Record_Callback( msg->type, H265_FATAL_ERR );
				break;
		}
		return;
	}
	else if( gH265_Record_HandlerTable[i].bcallback ){
		/* Normal end */
		H265_If_Record_Callback( msg->type, H265_OK );
	}
	return;
}


/*----------------------------------------------------------------------*/
/* Movie Record controller [TASK]										*/
/*----------------------------------------------------------------------*/
VOID H265_Record_Controller( T_H265_IF_MSG* in_msg )
{
	INT32			ret;
	INT32			init_flg = H265_ON;
	T_H265_IF_MSG	msg;
	
	H265_Movie_Memcpy( &msg, in_msg, sizeof(T_H265_IF_MSG) );
	
	H265_Info_Print(("+++ H265 Encode controller task is started.\n\r"));
	
	while(1){
		if( init_flg == H265_OFF ){
			// wait designation
			H265_If_Get_Msg( &msg, E_H265_IF_MSG_WAIT_BLOCK );
		}
		init_flg = H265_OFF;
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "H265Rctr" ));
		/* It checks request. if any message is received, it executes according request. */
		if( msg.type != E_H265_IF_TYPE_NO_REQUEST ) {
			h265_if_record_msg_handler( &msg );
			
			// finalize -> break&return
			if( msg.type == E_H265_IF_TYPE_RECORD_FINALIZE ) {
				H265_Movie_Memset( &msg, 0, sizeof(T_H265_IF_MSG) );
				break;
			}
			
			H265_Movie_Memset( &msg, 0, sizeof(T_H265_IF_MSG) );
		}
		switch( H265_If_Record_Api_Get_Status() ){
			case E_H265_IF_RECORD_STATUS_INIT:
			case E_H265_IF_RECORD_STATUS_RUNNING:
				/* Event notification executing Movie Record */
				switch( H265_If_Record_Api_Check_End() ){
					case H265_OK:
						break;
					case H265_ERR_SYSTEM:
					case H265_OK_NORMAL_END:
					default:
						h265_if_record_task_pri_reset();
						H265_If_Record_Api_Set_Status( E_H265_IF_RECORD_STATUS_END );
						h265_if_record_type_finalize( &msg );
						H265_Info_Print(("H265 Internal message full release start.\n"));
						while(1){
							ret = H265_If_Get_Msg( &msg, E_H265_IF_MSG_WAIT_NONBLOCK );
							if( ret != D_OS_USER_E_OK ){
								break;
							}
						}
						H265_Info_Print(("H265 Internal message full release end.\n"));
						M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "H265Rctr_End" ));
						OS_User_Set_Flg( FID_H265_TASK, FLG_H265_TASK_CONTROLLER_STOP );
						OS_User_Ext_Tsk();
						break;
				}
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265Rctr" ));
				OS_User_Tslp_Tsk(15);
				break;
				
			case E_H265_IF_RECORD_STATUS_END:
				/* Nothing Special */
				break;
				
			default:
				OS_User_Tslp_Tsk(100);
				break;
		}
	}
	
	H265_Info_Print(("+++ H265 Encode controller task is end.\n\r"));
	
	return;
}
