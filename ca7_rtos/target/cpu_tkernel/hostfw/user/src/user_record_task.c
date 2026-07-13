/**
 * @file		user_record_task.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")

#include "os_user_custom.h"
#include "user_record_task.h"
#include "fj_encoder.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static INT32 User_Record_Rcv_Msg( T_USER_RECORD_MSG* msg );

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void User_Record_Task( INT32 stacd )
{
	T_USER_RECORD_MSG msg;
	OS_USER_ER ercd = 0;

	while (1) {
		ercd = User_Record_Rcv_Msg( &msg );
		UPRINTF_ERR(ercd);

		switch( msg.event ) {
		case E_USER_RECORD_EVENT_RECORD_H264_FREE_INDEX_0:
			if(msg.param == E_FJ_VIDEO_ENC_ENUM_H265_0) {
				User_Utility_Record_H265_StreamCB_Sync(E_SHARE_YUV_OUT_H264_0, &msg.stream.h265);
				MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H264_0, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "H265_1 free"));
				User_Utility_Record_H265_Free_Index(E_SHARE_YUV_OUT_H264_0, msg.stream.h265.vstream_index, msg.stream.h265.stream_end_flg);
				MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H264_0, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_1 free"));
			}
			else if(msg.param == E_FJ_VIDEO_ENC_ENUM_H265_1) {
				User_Utility_Record_H265_StreamCB_Sync(E_SHARE_YUV_OUT_H264_0, &msg.stream.h265);
				MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H264_0, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "H265_1 free"));
				User_Utility_Record_H265_Free_Index(E_SHARE_YUV_OUT_H264_0, msg.stream.h265.vstream_index, msg.stream.h265.stream_end_flg);
				MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H264_0, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_1 free"));
			}
			else {
				User_Utility_Record_H264_StreamCBSync(E_SHARE_YUV_OUT_H264_0, &msg.stream.h264);
				MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H264_0, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "H264_0 free"));
				User_Utility_Record_H264_Free_Index(E_SHARE_YUV_OUT_H264_0, msg.stream.h264.vstream_index, (BOOL)msg.stream.h264.stream_end_flg);
				MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H264_0, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H264_0 free"));
			}
			break;
		case E_USER_RECORD_EVENT_RECORD_H264_FREE_INDEX_1:
			User_Utility_Record_H264_StreamCBSync(E_SHARE_YUV_OUT_H264_1, &msg.stream.h264);
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H264_1, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "H264_1 free"));
			User_Utility_Record_H264_Free_Index(E_SHARE_YUV_OUT_H264_1, msg.stream.h264.vstream_index, (BOOL)msg.stream.h264.stream_end_flg);
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H264_1, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H264_1 free"));
			break;
		case E_USER_RECORD_EVENT_RECORD_H265_FREE_INDEX_0:
			if(msg.param == E_FJ_VIDEO_ENC_ENUM_H265_0) {
				User_Utility_Record_H265_StreamCB_Sync(E_SHARE_YUV_OUT_H265, &msg.stream.h265);
			} else {
				User_Utility_Record_H264_StreamCBSync(E_SHARE_YUV_OUT_H265, &msg.stream.h264);
			}
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H265, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_START, "H265_0 free"));
			if(msg.param == E_FJ_VIDEO_ENC_ENUM_H265_0) {
				User_Utility_Record_H265_Free_Index(E_SHARE_YUV_OUT_H265, msg.stream.h265.vstream_index, msg.stream.h265.stream_end_flg);
			} else {
				User_Utility_Record_H264_Free_Index(E_SHARE_YUV_OUT_H265, msg.stream.h264.vstream_index, msg.stream.h264.stream_end_flg);
			}
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_H265, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_END, "H265_0 free"));
			break;
		default:
			break;
		}
	}
}

INT32 Snd_Msg_To_User_Record(const T_USER_RECORD_MSG* msg)
{
	OS_USER_ER	ercd = 0;
	T_USER_RECORD_MBOX *mbox = NULL;

	ercd = OS_User_Get_Mpf( MPFID_USER_RECORD, (OS_USER_VP*)&mbox );
	UPRINTF_ERR(ercd);

	mbox->blf_addr = mbox;

	memcpy( mbox->msg, msg, sizeof(T_USER_RECORD_MSG) );

	ercd = OS_User_Snd_Mbx( MID_USER_RECORD, (T_OS_USER_MSG*)mbox );
	UPRINTF_ERR(ercd);

	return (ercd);
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static INT32 User_Record_Rcv_Msg( T_USER_RECORD_MSG* msg )
{
	OS_USER_ER	ercd = 0;
	T_USER_RECORD_MBOX *mbox = NULL;
	
	/* [mbox] */
	ercd = OS_User_Rcv_Mbx( MID_USER_RECORD, (T_OS_USER_MSG**)&mbox );
	if( ercd != D_OS_USER_E_OK ){
		UPRINTF_ERR(ercd);
		return ( ercd );
	}

	memcpy( msg, mbox->msg, sizeof(T_USER_RECORD_MSG) );

	ercd = OS_User_Rel_Mpf( MPFID_USER_RECORD, (OS_USER_VP)mbox->blf_addr );
	UPRINTF_ERR(ercd);
	return (ercd);
}

