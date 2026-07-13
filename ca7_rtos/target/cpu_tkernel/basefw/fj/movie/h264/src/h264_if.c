/**
 * @file		h264_if.c
 * @brief		H.264 movie function Host I/F.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "h264_if.h"
#include "h264_if_common.h"
#include "h264_movie.h"
#include "h264_encode.h"
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
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */


/*----------------------------------------------------------------------*/
/* H264 controller [main TASK]											*/
/*----------------------------------------------------------------------*/
VOID H264_Controller( INT32 stacd )
{
	T_H264_IF_MSG	msg;
	E_H264_IF_ID	id;
	
	H264_Info_Print(("+++ H264 controller task is started.\n\r"));
	
	// block -> Record or Play "INITIALIZE" waiting...
	H264_If_Get_Msg(&msg, E_H264_IF_MSG_WAIT_BLOCK);
	
	// Record or Play main route.
	id = (E_H264_IF_ID)( msg.type & 0xFFFF0000 );
	
	if( id == E_H264_IF_ID_RECORD ){
		H264_Record_Controller( &msg );
	}
	else{
		H264_Play_Controller( &msg );
	}
	
	OS_User_Ext_Tsk();
}


/*----------------------------------------------------------------------*/
/* Message IF															*/
/*----------------------------------------------------------------------*/
/**
 * set movie task interface
 */
INT32 H264_If_Set_Msg( T_H264_IF_MSG* msg, E_H264_IF_MSG_WAIT wait )
{
	OS_USER_ER		ercd;
	T_H264_IF_MBOX	*mbox_buff;
	
	/* [blf] */
	if( wait == E_H264_IF_MSG_WAIT_BLOCK ){	ercd = OS_User_Get_Mpf( MPFID_H264_IF_MSG, (OS_USER_VP*)&mbox_buff);	}
	else{									ercd = OS_User_Pget_Mpf(MPFID_H264_IF_MSG, (OS_USER_VP*)&mbox_buff);	}
	
	if( ercd != D_OS_USER_E_OK ){
		H264_Err_Print(("H264 OS_User_Get_Mpf/OS_User_Pget_Mpf(): ercd=%x, mboxid=%d, mpfid=%d\n", ercd, MID_H264_IF, MPFID_H264_IF_MSG));
		return ( ercd );
	}
	
	/* [copy] */
	mbox_buff->blf_addr = mbox_buff;
	
#if 0	// for Debug
	H264_Info_Print(("H264_If_Set_Msg : get blf_addr = %#x\n", mbox_buff->blf_addr));
#endif	// for Debug
	
	H264_Movie_Memcpy( mbox_buff->msg, msg, sizeof(T_H264_IF_MSG) );
	
	/* [mbox] */
//	H264_Info_Print(("[MBOX] message send! ID: %d (%x[byte])\n", MID_H264_IF, sizeof(T_H264_IF_MBOX)));
	ercd = OS_User_Snd_Mbx( MID_H264_IF, (T_OS_USER_MSG*)mbox_buff );
	if( ercd != D_OS_USER_E_OK ){
		H264_Err_Print(("H264 OS_User_Snd_Mbx(): ercd=%x, mboxid=%d, mpfid=%d\n", ercd, MID_H264_IF, MPFID_H264_IF_MSG));
	}
	return ( ercd );
}


/**
 * get movie task interface
 */
INT32 H264_If_Get_Msg( T_H264_IF_MSG* msg, E_H264_IF_MSG_WAIT wait )
{
	OS_USER_ER		ercd;
	T_H264_IF_MBOX	*mbox_buff;
	
	/* [mbox] */
	if( wait == E_H264_IF_MSG_WAIT_BLOCK ){	ercd = OS_User_Rcv_Mbx( MID_H264_IF, (T_OS_USER_MSG**)&mbox_buff);	}
	else{									ercd = OS_User_Prcv_Mbx(MID_H264_IF, (T_OS_USER_MSG**)&mbox_buff);	}
	
	if( ercd != D_OS_USER_E_OK ){
		if( ercd != D_OS_USER_E_TMOUT ){
			H264_Err_Print(("H264 OS_User_Rcv_Mbx/OS_User_Prcv_Mbx(): ercd=%x, mboxid=%d, mpfid=%d\n", ercd, MID_H264_IF, MPFID_H264_IF_MSG));
		}
		return ( ercd );
	}
//	H264_Info_Print(("[MBOX] message received! ID: %d (%x[byte])\n", MID_H264_IF, sizeof(T_H264_IF_MBOX)));
	
	/* [copy] */
	H264_Movie_Memcpy( msg, mbox_buff->msg, sizeof(T_H264_IF_MSG) );
	
#if 0	// for Debug
	H264_Info_Print(("H264_If_Get_Msg : free blf_addr = %#x\n", mbox_buff->blf_addr));
#endif	// for Debug
	
	/* [blf] */
	ercd = OS_User_Rel_Mpf( MPFID_H264_IF_MSG, (OS_USER_VP)mbox_buff->blf_addr );
	if( ercd != D_OS_USER_E_OK ){
		H264_Err_Print(("H264 OS_User_Rel_Mpf(): ercd=%x, mboxid=%d, mpfid=%d\n", ercd, MID_H264_IF, MPFID_H264_IF_MSG));
	}
	return ( ercd );
}


/**
 * clear movie task interface
 */
INT32 H264_If_Clear_Msg( VOID )
{
	OS_USER_ER		ercd;
	T_H264_IF_MBOX	*mbox_buff;
	
	while(1){
		ercd = OS_User_Prcv_Mbx(MID_H264_IF, (T_OS_USER_MSG**)&mbox_buff);
		if( ercd != D_OS_USER_E_OK ){
			if( ercd != D_OS_USER_E_TMOUT ){
				H264_Err_Print(("H264 OS_User_Prcv_Mbx(): ercd=%x, mboxid=%d, mpfid=%d\n", ercd, MID_H264_IF, MPFID_H264_IF_MSG));
			}
			break;
		}
	}
	
	return ( ercd );
}


/*----------------------------------------------------------------------*/
/* Function call IF (Record)											*/
/*----------------------------------------------------------------------*/
/**
 * Get Image Area Address on Movie Mode
 * @param		stream_no		stream number
 * @param		stream_no		stream size
 * @param		frame_no		frame number
 * @param		addr_param		Address parameter
 * @return		result
 * @attention	It gets image area address.
 */
INT32 H264_If_Get_Record_Image_Address(
			E_H264_IF_STREAM_CH		stream_ch,
			E_H264_IF_IMAGE_SIZE	size,
			ULLONG					frame_no, 
			T_H264_IF_GET_IMAGE_ADDR_PARAM* addr_param
			)
{
	INT32 ret;
	
	if( addr_param == NULL ){
		return H264_PARAM_ERR;
	}
	ret = H264_Encode_Image_Address_Get( stream_ch, size, frame_no, addr_param );
	
	return ret;
}

/**
 * Decide Image Area Address on Movie Mode
 * @param		stream_ch		stream channel
 * @param		frame_no		frame number
 * @param		dummy_flg		dummy bunk use flag
 * @return		None
 * @attention	It decide image area address.
 */
VOID H264_If_Decide_Record_Image_Address( E_H264_IF_STREAM_CH stream_ch, ULLONG frame_no, BOOL dummy_flg )
{
	H264_Encode_Image_Address_Decide( stream_ch, frame_no, dummy_flg );
	return;
}

/**
 * Encode Frame Enabled
 * @param		stream_ch		stream channel
 * @param		enabled			enabled flag
 * @return		None
 * @attention	None
 */
VOID H264_If_Record_Frame_Enabled( E_H264_IF_STREAM_CH stream_ch, ULLONG frame_no, BOOL dummy_flg, BOOL enabled )
{
	H264_Encode_Frame_Enabled( stream_ch, frame_no, dummy_flg, enabled );
	return;
}

/**
 * Valid Image Area Address on Movie Mode
 * @param		stream_ch		stream channel
 * @return		None
 * @attention	None
 */
VOID H264_If_Record_Valid_Image_Address( E_H264_IF_STREAM_CH stream_ch )
{
	H264_Encode_Vin_Address_Valid( stream_ch );
	return;
}

/**
 * Set Dynamic Parameter  on Movie Mode
 * @param		stream_ch				stream channel
 * @param		dynamic_param			dynamic parameter
 * @return		None
 * @attention	None
 */
VOID H264_If_Record_Dynamic_Param_Set( E_H264_IF_STREAM_CH stream_ch, T_H264_IF_DYNAMIC_PARAM_RECORD* dynamic_param )
{
	H264_Encode_Dynamic_Parameter_Set( stream_ch, dynamic_param );
	return;
}

VOID H264_If_Record_Insert_IDR_Trigger_Set( E_H264_IF_STREAM_CH stream_ch )
{
	H264_Encode_Insert_IDR_Trriger_Set( stream_ch );
	return;
}

/**
 * Free video stream
 * @param		i:vstream_free_info		Free video stream
 * @return		result
 * @attention	None.
 */
INT32 H264_If_Record_Free_Video_Index( E_H264_IF_STREAM_CH stream_ch, UINT32 free_vstream_index, BOOL last_free_flg )
{
	return H264_Encode_Free_Video_Index( stream_ch, free_vstream_index, last_free_flg );
}

/*----------------------------------------------------------------------*/
/* Function call IF (Play)												*/
/*----------------------------------------------------------------------*/

/**
 * Set Video Stream
 * @param		Stream Information
 * @return		OK/NG
 * @attention	None
 */
INT32 H264_If_Play_Set_Video_Stream( UINT32 store_num, T_H264_IF_PARAM_SET_STREAM* video_stream_param )
{
	INT32 ret;
	
	ret = H264_Decode_Video_Stream_Set( store_num, video_stream_param );
	
	return ret;
}

/**
 * Get Display Frame
 * @param		YUV Information
 * @return		OK/NG
 * @attention	None
 */
INT32 H264_If_Play_Get_Display_Frame( T_H264_IF_VIDEO_YUV_INFO* display_yuv_info )
{
	INT32 ret;
	
	ret = H264_Decode_Video_Display_Frame_Get( display_yuv_info );
	
	return ret;
}

/*----------------------------------------------------------------------*/
/* Function call IF (Common)											*/
/*----------------------------------------------------------------------*/
VOID H264_If_Set_Codec_CoreMode( UINT32 core_mode )
{
	H264_Movie_Ribery_Core_Mode_Set( core_mode );
	return;
}
