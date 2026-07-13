/**
 * @file		h264_if_play_api.c
 * @brief		This file is a movie play api function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#include "image_info.h"
#include "h264_if.h"
#include "h264_if_common.h"
#include "h264_movie.h"
#include "h264_decode.h"

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
// Play Information
typedef struct{
		E_H264_IF_PLAY_STATUS			status;
		BOOL							last_picture_keep;
} T_H264_IF_PLAY_API_INFO;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Movie Play Message
static	volatile	T_H264_IF_MSG				gPlay_Msg;

// Movie Play Infromation(status, condition, etc)
static	volatile	T_H264_IF_PLAY_API_INFO		gPlay_Info;

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */


/*----------------------------------------------------------------------*/
/* Sub Function															*/
/*----------------------------------------------------------------------*/
/**
 * set current status of movie record function
 * @param	status
 * @return	nothing
 */
VOID H264_If_Play_Api_Set_Status( E_H264_IF_PLAY_STATUS status )
{
	gPlay_Info.status = status;
}

/**
 * get current status of movie play function
 * @param	nothing
 * @return	current status. please refer to E_H264_IF_PLAY_STATUS.
 */
E_H264_IF_PLAY_STATUS H264_If_Play_Api_Get_Status( VOID )
{
	return gPlay_Info.status;
}


/**
 * set last picture keep
 * @param	last_picture_keep
 * @return	nothing
 */
VOID H264_If_Play_Api_Set_Last_Picture_Keep( BOOL last_picture_keep )
{
	gPlay_Info.last_picture_keep = last_picture_keep;
}

/**
 * get last picture keep
 * @param	nothing
 * @return	last_picture_keep.
 */
BOOL H264_If_Play_Api_Get_Last_Picture_Keep( VOID )
{
	return gPlay_Info.last_picture_keep;
}

/**
 * It set User Event to global data..
 * @param	msg
 * @return	none
 */
VOID H264_If_Play_Api_Set_User_Event( T_H264_IF_MSG* msg )
{
	if( msg == NULL ){
		return;
	}
	H264_Movie_Memcpy( (VOID*)&gPlay_Msg, msg, sizeof(T_H264_IF_MSG) );
	return;
}

/**
 * Event Check Process on Movie Playing.
 * @param	nothing
 * @return	Event
 */
INT32 H264_If_Play_Api_Check_Event( VOID )
{
	INT32			mp_event;
	T_H264_IF_MSG	msg;
	
	/* Get User Event */
	H264_If_Play_Event_Check( &msg );
	switch( msg.type ){
		case E_H264_IF_TYPE_NO_REQUEST:
			mp_event = (INT32)E_H264_EVENT_NONE;
			break;
		case E_H264_IF_TYPE_PLAY_START:
			if( msg.data.pPlayParam->pause == FALSE ){
				mp_event = (INT32)E_H264_EVENT_PLAY;
			}
			else{
				mp_event = (INT32)E_H264_EVENT_PAUSE;
			}
			break;
		case E_H264_IF_TYPE_PLAY_PAUSE:
			mp_event = (INT32)E_H264_EVENT_PAUSE;
			break;
		case E_H264_IF_TYPE_PLAY_RESUME:
			mp_event = (INT32)E_H264_EVENT_REPLAY;
			break;
		case E_H264_IF_TYPE_PLAY_FLUSH:
			mp_event = (INT32)E_H264_EVENT_FLUSH;
			break;
		case E_H264_IF_TYPE_PLAY_STOP:
			mp_event = (INT32)E_H264_EVENT_STOP;
			break;
		default:
			mp_event = (INT32)E_H264_EVENT_NONE;
			break;
	}
	return mp_event;
}

/*----------------------------------------------------------------------*/
/* Param Check function													*/
/*----------------------------------------------------------------------*/

/**
 * Param Check (Init).
 * @param	nothing
 * @return	nothing
 */
INT32 H264_If_Play_Api_Param_Check_Init( T_H264_IF_MSG* msg )
{
	if( msg == NULL ){
		return H264_PARAM_ERR;
	}
	return H264_OK;
}

/**
 * Param Check (Topimage).
 * @param	nothing
 * @return	nothing
 */
INT32 H264_If_Play_Api_Param_Check_Topimage( T_H264_IF_MSG* msg )
{
	if( msg == NULL ){
		return H264_PARAM_ERR;
	}
	// Stream Store num Check
	if( msg->data.pTopimageParam->store_num == 0 ){
		return H264_PARAM_ERR;
	}
	
	// Null Check
	if( msg->data.pTopimageParam->video_stream_param == NULL ){
		return H264_PARAM_ERR;
	}
	if( msg->data.pTopimageParam->decode_yuv_info == NULL ){
		return H264_PARAM_ERR;
	}
	
	return H264_OK;
}

/**
 * Param Check (Play).
 * @param	nothing
 * @return	nothing
 */
INT32 H264_If_Play_Api_Param_Check_Play( T_H264_IF_MSG* msg )
{
	if( msg == NULL ){
		return H264_PARAM_ERR;
	}
	return H264_OK;
}

/*----------------------------------------------------------------------*/
/* Main API Function													*/
/*----------------------------------------------------------------------*/
/**
 * Initialize.
 * @param	nothing
 * @return	nothing
 */
VOID H264_If_Play_Api_Initialize( VOID )
{
	/* User Interface Area */
	H264_Movie_Memset( (VOID*)&gPlay_Msg, 0, sizeof(gPlay_Msg) );
	
	/* Play Information */
	H264_Movie_Memset( (VOID*)&gPlay_Info, 0, sizeof(gPlay_Info) );
	
	return;
}

/**
 * Finalize.
 * @param	nothing
 * @return	nothing
 */
VOID H264_If_Play_Api_Finalize( VOID )
{
	// Nothing Special.
	
	return;
}

/**
 * TopImage get.
 * @param	nothing
 * @return	nothing
 */
INT32 H264_If_Play_Api_TopImage_Get( T_H264_IF_MSG* msg )
{
	INT32	ret;
	
	ret = H264_Movie_Play_Top_Get(
			msg->data.pTopimageParam->decoder_param,
			msg->data.pTopimageParam->req_pts,
			msg->data.pTopimageParam->store_num,
			msg->data.pTopimageParam->video_stream_param,
			msg->data.pTopimageParam->decode_yuv_info
			);
	if( ret != H264_OK ){
		return H264_FATAL_ERR;
	}
	
	return H264_OK;
}

/**
 * Play Start Init.
 * @param	nothing
 * @return	nothing
 */
INT32 H264_If_Play_Api_Start_Init( T_H264_IF_MSG* msg )
{
	INT32	ret;
	
	if( msg == NULL ){
		return H264_PARAM_ERR;
	}
	/* It keeps user request message to global data */
	H264_If_Play_Api_Set_User_Event( msg );
	
	/* Set Play Index Information */
	ret = H264_Movie_Play_Top_Set( &msg->data.pInitParam->decoder_param, &msg->data.pInitParam->display_info );
	if( ret != H264_OK ){
		return H264_FATAL_ERR;
	}
	
	ret = H264_Movie_Play_Start_Init();
	if( ret != H264_OK ){
		H264_Info_Print(("----- Movie Play Init Result : %d\n", ret ));
	}
	
	return H264_OK;
}

/**
 * Play Start.
 * @param	nothing
 * @return	nothing
 */
INT32 H264_If_Play_Api_Start( T_H264_IF_MSG* msg )
{
	INT32	ret;
	
	/* It executes Movie Play */
	ret = H264_Movie_Play_Start(
			msg->data.pPlayParam->start_pts,
			msg->data.pPlayParam->direction,
			msg->data.pPlayParam->speed,
			msg->data.pPlayParam->pause
			);
	if( ret != H264_OK ){
		H264_Info_Print(("----- Movie Play NG : %d\n", ret ));
		return H264_NG;
	}
	
	return H264_OK;
}

