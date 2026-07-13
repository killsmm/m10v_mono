/**
 * @file		h265_if.c
 * @brief		H.265 movie function Host I/F.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "h265_if.h"
#include "h265_if_common.h"
#include "h265_movie.h"
#include "h265_encode.h"
#include "h265_decode.h"

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
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */


/*----------------------------------------------------------------------*/
/* H265 controller [main TASK]											*/
/*----------------------------------------------------------------------*/
VOID H265_Controller( INT32 stacd )
{
	T_H265_IF_MSG	msg;
	E_H265_IF_ID	id;
	
	H265_Info_Print(("+++ Movie controller task is started.\n\r"));
	
	// block -> Record or Play "INITIALIZE" waiting...
	H265_If_Get_Msg(&msg, E_H265_IF_MSG_WAIT_BLOCK);
	
	// Record or Play main route.
	id = (E_H265_IF_ID)( msg.type & 0xFFFF0000 );
	
	if( id == E_H265_IF_ID_RECORD ){
		H265_Record_Controller( &msg );
	}
	else{
		H265_Play_Controller( &msg );
	}
	
	OS_User_Ext_Tsk();
	
	return;
}


/*----------------------------------------------------------------------*/
/* Message IF															*/
/*----------------------------------------------------------------------*/
/**
 * set movie task interface
 */
INT32 H265_If_Set_Msg( T_H265_IF_MSG* msg, E_H265_IF_MSG_WAIT wait )
{
	OS_USER_ER		ercd;
	T_H265_IF_MBOX	*mbox_buff;
	
	/* [blf] */
	if( wait == E_H265_IF_MSG_WAIT_BLOCK ){	ercd = OS_User_Get_Mpf( MPFID_H265_IF_MSG, (OS_USER_VP*)&mbox_buff);	}
	else{									ercd = OS_User_Pget_Mpf(MPFID_H265_IF_MSG, (OS_USER_VP*)&mbox_buff);	}
	
	if( ercd != D_OS_USER_E_OK ){
		H265_Err_Print(("H265 OS_User_Get_Mpf/OS_User_Pget_Mpf(): ercd=%x, mboxid=%d, mpfid=%d\n", ercd, MID_H265_IF, MPFID_H265_IF_MSG));
		return ( ercd );
	}
	
	/* [copy] */
	mbox_buff->blf_addr = mbox_buff;
	H265_Movie_Memcpy( mbox_buff->msg, msg, sizeof(T_H265_IF_MSG) );
	
	/* [mbox] */
//	H265_Info_Print(("[MBOX] message send! ID: %d (%x[byte])\n", MID_H265_IF, sizeof(T_H265_IF_MBOX)));
	ercd = OS_User_Snd_Mbx( MID_H265_IF, (T_OS_USER_MSG*)mbox_buff );
	if( ercd != D_OS_USER_E_OK ){
		H265_Err_Print(("H265 OS_User_Snd_Mbx(): ercd=%x, mboxid=%d, mpfid=%d\n", ercd, MID_H265_IF, MPFID_H265_IF_MSG));
	}
	return ( ercd );
}


/**
 * get movie task interface
 */
INT32 H265_If_Get_Msg( T_H265_IF_MSG* msg, E_H265_IF_MSG_WAIT wait )
{
	OS_USER_ER		ercd;
	T_H265_IF_MBOX	*mbox_buff;
	
	/* [mbox] */
	if( wait == E_H265_IF_MSG_WAIT_BLOCK ){	ercd = OS_User_Rcv_Mbx( MID_H265_IF, (T_OS_USER_MSG**)&mbox_buff);	}
	else{									ercd = OS_User_Prcv_Mbx(MID_H265_IF, (T_OS_USER_MSG**)&mbox_buff);	}
	
	if( ercd != D_OS_USER_E_OK ){
		if( ercd != D_OS_USER_E_TMOUT ){
			H265_Err_Print(("OS_User_Rcv_Mbx/OS_User_Prcv_Mbx(): ercd=%x, mboxid=%d, mpfid=%d\n", ercd, MID_H265_IF, MPFID_H265_IF_MSG));
		}
		return ( ercd );
	}
//	H265_Info_Print(("[MBOX] message received! ID: %d (%x[byte])\n", MID_H265_IF, sizeof(T_H265_IF_MBOX)));
	
	/* [copy] */
	H265_Movie_Memcpy( msg, mbox_buff->msg, sizeof(T_H265_IF_MSG) );
	
	/* [blf] */
	ercd = OS_User_Rel_Mpf( MPFID_H265_IF_MSG, (OS_USER_VP)mbox_buff->blf_addr );
	if( ercd != D_OS_USER_E_OK ){
		H265_Err_Print(("OS_User_Rel_Mpf(): ercd=%x, mboxid=%d, mpfid=%d\n", ercd, MID_H265_IF, MPFID_H265_IF_MSG));
	}
	return ( ercd );
}


/**
 * clear movie task interface
 */
INT32 H265_If_Clear_Msg( VOID )
{
	OS_USER_ER		ercd;
	T_H265_IF_MBOX	*mbox_buff;
	
	while(1){
		ercd = OS_User_Prcv_Mbx(MID_H265_IF, (T_OS_USER_MSG**)&mbox_buff);
		if( ercd != D_OS_USER_E_OK ){
			if( ercd != D_OS_USER_E_TMOUT ){
				H265_Err_Print(("OS_User_Prcv_Mbx(): ercd=%x, mboxid=%d, mpfid=%d\n", ercd, MID_H265_IF, MPFID_H265_IF_MSG));
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
 * @param		y_addr			Y    Address for YCC420
 * @param		c_addr			CbCr Address for YCC420
 * @param		g_width			Global Width
 * @param		dummy_flg		Dummy Bank use flag
 * @return		result
 * @attention	It gets image area address.
 */
INT32 H265_If_Get_Record_Image_Address(
			E_H265_IF_STREAM_CH		stream_ch,
			E_H265_IF_IMAGE_SIZE	size,
			ULLONG					frame_no, 
			T_H265_IF_GET_IMAGE_ADDR_PARAM* addr_param
			)
{
	INT32 ret;
	
	if( addr_param == NULL ){
		return H265_PARAM_ERR;
	}
	ret = H265_Encode_Image_Address_Get( stream_ch, size, frame_no, addr_param );
	
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
VOID H265_If_Decide_Record_Image_Address( E_H265_IF_STREAM_CH stream_ch, ULLONG frame_no, BOOL dummy_flg )
{
	H265_Encode_Image_Address_Decide( stream_ch, frame_no, dummy_flg );
	return;
}

/**
 * Encode Frame Enabled
 * @param		stream_ch		stream channel
 * @param		enabled			enabled flag
 * @return		None
 * @attention	None
 */
VOID H265_If_Record_Frame_Enabled( E_H265_IF_STREAM_CH stream_ch, ULLONG frame_no, BOOL dummy_flg, BOOL enabled )
{
	H265_Encode_Frame_Enabled( stream_ch, frame_no, dummy_flg, enabled );
	return;
}

/**
 * Valid Image Area Address on Movie Mode
 * @param		stream_ch		stream channel
 * @return		None
 * @attention	None
 */
VOID H265_If_Record_Valid_Image_Address( E_H265_IF_STREAM_CH stream_ch )
{
	H265_Encode_Vin_Address_Valid( stream_ch );
	return;
}

/**
 * Set Dynamic Parameter  on Movie Mode
 * @param		stream_ch				stream channel
 * @param		dynamic_param			dynamic parameter
 * @return		None
 * @attention	None
 */
VOID H265_If_Record_Dynamic_Param_Set( E_H265_IF_STREAM_CH stream_ch, T_H265_IF_DYNAMIC_PARAM_RECORD* dynamic_param )
{
	H265_Encode_Dynamic_Parameter_Set( stream_ch, dynamic_param );
	return;
}

VOID H265_If_Record_Insert_IDR_Trigger_Set( E_H265_IF_STREAM_CH stream_ch )
{
	H265_Encode_Insert_IDR_Trriger_Set( stream_ch );
	return;
}

/**
 * Free video stream
 * @param		i:vstream_free_info		Free video stream
 * @return		result
 * @attention	None.
 */
INT32 H265_If_Record_Free_Video_Index( E_H265_IF_STREAM_CH stream_ch, UINT32 free_vstream_index, BOOL last_free_flg )
{
	return H265_Encode_Free_Video_Index( stream_ch, free_vstream_index, last_free_flg );
}

/**
 * Sync Interrupt Counter Increment
 * @param		i:stream_ch			stream channel
 * @return		None.
 * @attention	None.
 */
VOID H265_If_Record_Sync_Interrupt_Counter_Increment( E_H265_IF_STREAM_CH stream_ch )
{
	H265_Encode_Sync_Interrupt_Count_Increment( stream_ch );
	return;
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
INT32 H265_If_Play_Set_Video_Stream( UINT32 store_num, T_H265_IF_PARAM_SET_STREAM* video_stream_param )
{
	INT32 ret;
	
	ret = H265_Decode_Video_Stream_Set( store_num, video_stream_param );
	
	return ret;
}

/**
 * Get Display Frame
 * @param		YUV Information
 * @return		OK/NG
 * @attention	None
 */
INT32 H265_If_Play_Get_Display_Frame( T_H265_IF_VIDEO_YUV_INFO* display_yuv_info )
{
	INT32 ret;
	
	ret = H265_Decode_Video_Display_Frame_Get( display_yuv_info );
	
	return ret;
}

