/**
 * @file		h264_if_record.c
 * @brief		This file is a movie record controller api function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "h264_if.h"
#include "h264_if_common.h"
#include "h264_movie.h"

#include "os_user_custom.h"

#ifdef CO_H264_SECTION_ON
//#pragma arm section code="H264_CODE"
//#pragma arm section rodata="H264_CONST"
//#pragma arm section rwdata="H264_INIT"
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
static	void (*volatile gH264_CallBack)(int, int) = NULL;

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static	VOID	h264_if_record_task_pri_set( VOID );
static	VOID	h264_if_record_task_pri_reset( VOID );

static	INT32	h264_if_record_type_initialize( T_H264_IF_MSG* msg );
static	INT32	h264_if_record_type_finalize( T_H264_IF_MSG* msg );
static	INT32	h264_if_record_type_start_video( T_H264_IF_MSG* msg );
static	INT32	h264_if_record_type_stop( T_H264_IF_MSG* msg );
static	VOID	h264_if_record_msg_handler(T_H264_IF_MSG* msg);


/*----------------------------------------------------------------------*/
/* Movie -> Host function call/callback IF								*/
/*----------------------------------------------------------------------*/
/**
 * Movie Record CallBack Process
 * @param	type	I/F type
 * @param	param	I/F result
 * @return	nothing
 */
VOID H264_If_Record_Callback( E_H264_IF_TYPE type, INT32 param )
{
	H264_Info_Print(("<H264 Ctrl> CALLBACK::%x,result=%d\n", type, param));
	
	if( gH264_CallBack != NULL ){ gH264_CallBack(type, param); }
	return ;
}

/**
 * Normal End Message.
 * @param	nothing
 * @return	nothing
 */
VOID H264_If_Record_Notify_NORMAL_END( VOID )
{
	H264_Info_Print(("<H264 Ctrl> NORMAL END\n"));
	Host_H264_Notify_Stop_Trigger( E_H264_IF_STOP_NOTIFY_NORMAL_END );
	return ;
}

/**
 * VRAW MULTI Error Message.
 * @param	nothing
 * @return	nothing
 */
VOID H264_If_Record_Notify_VRAW_MULTI_ERR( VOID )
{
	H264_Info_Print(("<H264 Ctrl> VRAW MULTI ERROR\n"));
	Host_H264_Notify_Stop_Trigger( E_H264_IF_STOP_NOTIFY_VRAW_MULTI_ERR );
	return ;
}

/**
 * VSTRM EMPTY Error Message.
 * @param	nothing
 * @return	nothing
 */
VOID H264_If_Record_Notify_VSTRM_EMPTY_ERR( VOID )
{
	H264_Info_Print(("<H264 Ctrl> VSTRM EMPTY ERROR\n"));
	Host_H264_Notify_Stop_Trigger( E_H264_IF_STOP_NOTIFY_VSTRM_EMPTY_ERR );
	return ;
}

/**
 * Fatal Error Message.
 * @param	nothing
 * @return	nothing
 */
VOID H264_If_Record_Notify_FATAL_ERR( VOID )
{
	H264_Info_Print(("<H264 Ctrl> FATAL ERROR\n"));
	Host_H264_Notify_Stop_Trigger( E_H264_IF_STOP_NOTIFY_FATAL_ERR );
	return ;
}

/**
 * Stop Complete Message.
 * @param	nothing
 * @return	nothing
 */
VOID H264_If_Record_Notify_STOP_COMPLETE( VOID )
{
	H264_Info_Print(("<H264 Ctrl> STOP COMPLETE\n"));
	Host_H264_Notify_Stop_Complete();
	return ;
}

/**
 * State Change Complete Message.
 * @param	result  : Processing result
 *                    H264_OK  : Normal
 *                    !H264_OK : Abnormal
 * @return	nothing
 */
VOID H264_If_Record_Notify_STATE_CHANGE_COMPLETE( INT32 result )
{
	H264_Info_Print(("<H264 Ctrl> STATE CHANGE COMPLETE\n"));
	Host_H264_Notify_State_Change_Complete( result );
	return ;
}

/**
 * Video Stream Information Send Message.
 * @param	vstream_info         : pointer of video stream information
 * @return	TRUE  : Normal
 *			FALSE : Abnormal
 */
BOOL H264_If_Record_Notify_Video_Stream( T_H264_IF_VIDEO_STREAM_PARAM* vstream_info )
{
	//H264_Info_Print(("<H264 Ctrl> VIDEO STREAM SEND\n"));
	return Host_H264_Notify_Video_Stream( vstream_info );
}

/**
 * Video Vector Information Send Message.
 * @param	vectore_info         : pointer of video vector information
 * @return	TRUE  : Normal
 *			FALSE : Abnormal
 */
BOOL H264_If_Record_Notify_Vector_Info( T_H264_IF_VIDEO_VECTOR_PARAM* vector_info )
{
	return Host_H264_Notify_Vector_Info( vector_info );
}

/**
 * Video PreEncode Timming Send Message.
 * @param	stream_ch         : pointer of video stream channel
 * @return	TRUE  : Normal
 *			FALSE : Abnormal
 */
BOOL H264_If_Record_Notify_PreEncode( E_H264_IF_STREAM_CH stream_ch )
{
	return Host_H264_Notify_PreEncode( stream_ch );
}

/**
 * Video RepeatFrame occurred Timming Send Message.
 * @param	stream_ch         : pointer of video stream channel
 * @return	TRUE  : Normal
 *			FALSE : Abnormal
 */
BOOL H264_If_Record_Notify_RepeatFrame( E_H264_IF_STREAM_CH stream_ch )
{
	return Host_H264_Notify_RepeatFrame( stream_ch );
}


/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 * Task priority changing process during Movie Recording.
 * @param	nothing
 * @return	nothing
 */
static VOID h264_if_record_task_pri_set( VOID )
{
	OS_User_Chg_Pri( TID_H264_CONTROLLER, H264_TASK_PRI_ENC_MAIN );
	return ;
}

/**
 * Task priority recovery process after Movie Record
 * @param	nothing
 * @return	nothing
 */
static VOID h264_if_record_task_pri_reset( VOID )
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
static INT32 h264_if_record_type_initialize( T_H264_IF_MSG* msg )
{
	INT32 ret;
	
	if( H264_If_Record_Api_Get_Status() == E_H264_IF_RECORD_STATUS_END ){
		/* It executes Initialization process */
		H264_If_Record_Api_Initialize();
		/* It set "Initialization" to current status */
		H264_If_Record_Api_Set_Status( E_H264_IF_RECORD_STATUS_INIT );
	}
	
	/* It keeps input parameters to control data in movie_encode_api.c */
	ret = H264_If_Record_Api_Param_Set( msg );
	if( ret != H264_OK ) {
		if( H264_If_Record_Api_Get_Status() == E_H264_IF_RECORD_STATUS_INIT ){
			H264_If_Record_Api_Set_Status( E_H264_IF_RECORD_STATUS_END );
		}
		return ret;
	}
	/* It executes Ready process */
	ret = H264_If_Record_Api_Ready( msg );
	if( ret != H264_OK ) {
		if( H264_If_Record_Api_Get_Status() == E_H264_IF_RECORD_STATUS_INIT ){
			H264_If_Record_Api_Set_Status( E_H264_IF_RECORD_STATUS_END );
		}
		return ret;
	}
	/* It changes Task priority for Movie Recording */
	h264_if_record_task_pri_set();
	
	return H264_OK;
}

/**
 * Finalization Process.
 * @param	msg		no means
 * @return	result
 */
static INT32 h264_if_record_type_finalize( T_H264_IF_MSG* msg )
{
	/* It executes Finalization process */
	H264_If_Record_Api_Finalize();
	/* It set "End Status" to current status */
	H264_If_Record_Api_Set_Status( E_H264_IF_RECORD_STATUS_END );
	return H264_OK;
}


/**
 * Movie Video Record Start Process
 * @param	msg		no means
 * @return	result
 */
static INT32 h264_if_record_type_start_video( T_H264_IF_MSG* msg )
{
	/* It checks current status */
	if( H264_If_Record_Api_Get_Status() == E_H264_IF_RECORD_STATUS_INIT ){
		/* Record Start (Stop->Record) */
		OS_User_Set_Flg( FID_H264, FLG_H264_START );
		/* It set "Running Status" to current status */
		H264_If_Record_Api_Set_Status( E_H264_IF_RECORD_STATUS_RUNNING );
	}
	else if( H264_If_Record_Api_Get_Status() == E_H264_IF_RECORD_STATUS_RUNNING ){
		// **** entertain ****
	}
	else{
		return H264_PARAM_ERR;
	}
	
	return H264_OK;
}

/**
 * Movie Record Stop Process
 * @param	msg		no means
 * @return	result
 */
static INT32 h264_if_record_type_stop( T_H264_IF_MSG* msg )
{
	INT32	ret;
	
	/* It checks current status */
	switch( H264_If_Record_Api_Get_Status() ){
		case E_H264_IF_RECORD_STATUS_INIT:
		case E_H264_IF_RECORD_STATUS_RUNNING:
			break ;
		default :
			return H264_OK;
	}
	/* It executes STOP process */
	ret = H264_If_Record_Api_Stop( msg );
	
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
const T_H264_IF_HANDLER gH264_Record_HandlerTable[] = {
	{	E_H264_IF_TYPE_RECORD_INITIALIZE,	h264_if_record_type_initialize,		"E_H264_IF_TYPE_RECORD_INITIALIZE",		1	},
	{	E_H264_IF_TYPE_RECORD_FINALIZE,		h264_if_record_type_finalize,		"E_H264_IF_TYPE_RECORD_FINALIZE",		1	},
	{	E_H264_IF_TYPE_RECORD_START_VIDEO,	h264_if_record_type_start_video,	"E_H264_IF_TYPE_RECORD_START_VIDEO",	1	},
	{	E_H264_IF_TYPE_RECORD_STOP,			h264_if_record_type_stop,			"E_H264_IF_TYPE_RECORD_STOP",			1	},
};

/**
 * Movie Record Message Handller
 * @param	msg		no means
 * @return	result
 */
static VOID h264_if_record_msg_handler( T_H264_IF_MSG* msg )
{
	int i, max, ret;
	
	max = E_H264_IF_TYPE_RECORD_LAST - E_H264_IF_TYPE_RECORD_INITIALIZE;
	
	for( i=0; i<max; i++ ){
		if(gH264_Record_HandlerTable[i].type == msg->type){
			break;
		}
	}
	if( i == max ){ return; }
	
	// callback set
	gH264_CallBack = (void (*volatile)(int, int))msg->cbfunc;
	H264_Info_Print(("<H264 Ctrl> Received : %s\n", gH264_Record_HandlerTable[i].sztype));
	if(( ret = gH264_Record_HandlerTable[i].handler(msg) ) < 0 ){
		/* Abnormal end */
		H264_If_Record_Api_Set_Status( E_H264_IF_RECORD_STATUS_END );
		/* Response to demand */
		switch( ret ){
			case H264_PARAM_ERR:
				H264_If_Record_Callback( msg->type, H264_PARAM_ERR );
				break;
			default:
				H264_If_Record_Callback( msg->type, H264_FATAL_ERR );
				break;
		}
		return;
	}
	else if( gH264_Record_HandlerTable[i].bcallback ){
		/* Normal end */
		H264_If_Record_Callback( msg->type, H264_OK );
	}
	return;
}


/*----------------------------------------------------------------------*/
/* Movie Record controller [TASK]										*/
/*----------------------------------------------------------------------*/
VOID H264_Record_Controller( T_H264_IF_MSG* in_msg )
{
	INT32	init_flg = H264_ON;
	T_H264_IF_MSG msg;
	
	H264_Movie_Memcpy( &msg, in_msg, sizeof(T_H264_IF_MSG) );
	
	H264_Info_Print(("+++ H264 Encode controller task is started.\n\r"));
	
	while(1){
		if( init_flg == H264_OFF ){
			// wait designation
			H264_If_Get_Msg( &msg, E_H264_IF_MSG_WAIT_BLOCK );
		}
		init_flg = H264_OFF;
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "Rctr" ));
		/* It checks request. if any message is received, it executes according request. */
		if( msg.type != E_H264_IF_TYPE_NO_REQUEST ) {
			h264_if_record_msg_handler( &msg );
			
			// finalize -> break&return
			if( msg.type == E_H264_IF_TYPE_RECORD_FINALIZE ) {
				H264_Movie_Memset( &msg, 0, sizeof(T_H264_IF_MSG) );
				break;
			}
			
			H264_Movie_Memset( &msg, 0, sizeof(T_H264_IF_MSG) );
		}
		switch( H264_If_Record_Api_Get_Status() ){
			case E_H264_IF_RECORD_STATUS_INIT:
			case E_H264_IF_RECORD_STATUS_RUNNING:
				/* Event notification executing Movie Record */
				switch( H264_If_Record_Api_Check_End() ){
					case H264_OK:
						break;
					case H264_ERR_SYSTEM:
					case H264_OK_NORMAL_END:
					default:
						h264_if_record_task_pri_reset();
						H264_If_Record_Api_Set_Status( E_H264_IF_RECORD_STATUS_END );
						h264_if_record_type_finalize( &msg );
						M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "Rctr_End" ));
						OS_User_Set_Flg( FID_H264, FLG_H264_CONTROLLER_STOP );
						OS_User_Ext_Tsk();
						break;
				}
				M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "Rctr" ));
				OS_User_Tslp_Tsk(15);
				break;
				
			case E_H264_IF_RECORD_STATUS_END:
				/* Nothing Special */
				break;
				
			default:
				OS_User_Tslp_Tsk(100);
				break;
		}
	}
	
	return;
}
