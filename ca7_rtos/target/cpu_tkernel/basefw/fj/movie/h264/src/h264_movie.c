/**
 * @file		h264_movie.c
 * @brief		H.264 Movie Function Control.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "h264_movie.h"
#include "h264_encode.h"
#include "h264_decode.h"
#include "h264_ribery.h"
#include "sdram_map_movie_record.h"
#include "sdram_map_movie_play.h"
#include "sdram_map_cache_define.h"

#include "im_iip.h"
#include "im_iip_param.h"
#include "im_sdramc.h"
#include "bf_arm.h"
#include "dd_pmu.h"
#include "dd_arm.h"

#include "bf_iip_resource.h"
#include "l1l2cache.h"
#include "disp.h"

#include "ip_ge.h"
#include "ip_ahip.h"

#ifdef CO_H264_SECTION_ON
//#pragma arm section code="H264_CODE"
//#pragma arm section rodata="H264_CONST"
//#pragma arm section rwdata="H264_INIT"
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// [IIP]
#define H264_IIP_PIXID_MAX			(D_IM_IIP_PIXID_MAXNUM)

// IIP Table Align
#define H264_IIP_TBL_ALIGN			(8)

#define D_LOCAL_GRID_H_SIZ	(64)
#define D_LOCAL_GRID_V_SIZ	(32)
#define D_LOCAL_GRID_H_PIT	(6)		// 2^6 = 64
#define D_LOCAL_GRID_V_PIT	(5)		// 2^5 = 32

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
static	volatile	UINT32					gH264_Ribery_Core_Mode;
static	volatile	E_H264_EVENT			gMovie_User_Evevt;					// User Request

static	UINT32		gH264_Ribery_CS			__attribute__((section(".LOCK_SECTION"), aligned(64)));

/*--------------------------------------------------------------*/
/* Macro														*/
/*--------------------------------------------------------------*/
#define H264_IIP_ROUNDUP_ALIGN(val)	(((((val) +H264_IIP_TBL_ALIGN -1)) /H264_IIP_TBL_ALIGN) *H264_IIP_TBL_ALIGN)

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* A. H264 Control														*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		H264 CC Process Task.
 *	@param		i:stacd		nothing
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_CC_Process( INT32 stacd )
{
	H264_Info_Print(("+++ H264_CC Process Start\n" ));
	
	if( stacd == H264_MODE_RECORD ){
		OS_User_Chg_Pri( D_OS_USER_TSK_SELF, H264_TASK_PRI_ENC_CC );
		H264_Encode_CC_Main();
	}
	else{
		OS_User_Chg_Pri( D_OS_USER_TSK_SELF, H264_TASK_PRI_DEC_CC );
		H264_Decode_CC_Main();
	}
	
	H264_Info_Print(("+++ H264_CC Process End\n" ));
	
	OS_User_Ext_Tsk();
	return;
}


/**
 *	@brief		H264 ENT Process Task.
 *	@param		i:stacd		nothing
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_ENT_Process( INT32 stacd )
{
	H264_Info_Print(("+++ H264_ENT Process Start\n" ));
	
	if( stacd == H264_MODE_RECORD ){
		OS_User_Chg_Pri( D_OS_USER_TSK_SELF, H264_TASK_PRI_ENC_ENT );
		H264_Encode_ENT_Main();
	}
	else{
		OS_User_Chg_Pri( D_OS_USER_TSK_SELF, H264_TASK_PRI_DEC_ENT );
		H264_Decode_ENT_Main();
	}
	
	H264_Info_Print(("+++ H264_ENT Process End\n" ));
	
	OS_User_Ext_Tsk();
	return;
}

/**
 *	@brief		H264 VECTOR Process Task.
 *	@param		i:stacd		nothing
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_VECTOR_Process( INT32 stacd )
{
	H264_Info_Print(("+++ H264_VECTOR Process Start\n" ));
	
	if( stacd == H264_MODE_RECORD ){
		OS_User_Chg_Pri( D_OS_USER_TSK_SELF, H264_TASK_PRI_ENC_VECTOR);
		H264_Encode_VECTOR_Main();
	}

	H264_Info_Print(("+++ H264_VECTOR Process End\n" ));
	
	OS_User_Ext_Tsk();
	return;
}

/**
 *	@brief		H264 Display Process Task.\n
 *	@param		i:stacd		nothing
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_DISP_Process( INT32 stacd )
{
	H264_Info_Print(("+++ H264_DISP Process Start\n" ));
	
	if( stacd == H264_MODE_RECORD ){
		OS_User_Chg_Pri( D_OS_USER_TSK_SELF, H264_TASK_PRI_ENC_DISP );
#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
		H264_Encode_DISP_Main();
#endif
	}
	else{
		OS_User_Chg_Pri( D_OS_USER_TSK_SELF, H264_TASK_PRI_DEC_DISP );
		H264_Decode_DISP_Main();
	}
	
	H264_Info_Print(("+++ H264_DISP Process End\n" ));
	
	OS_User_Ext_Tsk();
	return;
}


/**
 *	@brief		H264 Display LCD Process Task.\n
 *	@param		i:stacd		nothing
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_DISP_LCD_Process( INT32 stacd )
{
	H264_Info_Print(("+++ H264 DISP LCD Process Start\n" ));
	
	OS_User_Chg_Pri( D_OS_USER_TSK_SELF, H264_TASK_PRI_DEC_DISP_LCD );
	H264_Decode_DISP_LCD_Main();
	
	H264_Info_Print(("+++ H264 DISP LCD Process End\n" ));
	
	OS_User_Ext_Tsk();
	return;
}

/**
 *	@brief		H264 Display HDMI Process Task.\n
 *	@param		i:stacd		nothing
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_DISP_HDMI_Process( INT32 stacd )
{
	H264_Info_Print(("+++ H264 DISP HDMI Process Start\n" ));
	
	OS_User_Chg_Pri( D_OS_USER_TSK_SELF, H264_TASK_PRI_DEC_DISP_HDMI );
	H264_Decode_DISP_HDMI_Main();
	
	H264_Info_Print(("+++ H264 DISP HDMI Process End\n" ));
	
	OS_User_Ext_Tsk();
	return;
}

/*----------------------------------------------------------------------*/
/* C. H264 Decode														*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		Get TOP Image.\n
 *	@param		play_info		i:Play Information of UI
 *	@param		va_adr_y		i/o: Y Address Pointer
 *	@param		table_index		i:Table Index for Play Index.(0 to 7)
 *	@return		H264_OK						Normal End
 *	@return		H264_ERR_INPUT_PARAM		Input Parameter Error
 *	@return		H264_ERR_FILE_ACCESS		File Access Error
 *	@return		H264_ERR_FILE_NO_SUPPORT	Not Support File Error
 *	@note		None
 *	@attention	None
*/
INT32 H264_Movie_Play_Top_Get(
			T_H264_IF_DECODER_PARAM*	decoder_param,
			ULLONG						req_pts,
			UINT32						store_num,
			T_H264_IF_PARAM_SET_STREAM*	video_stream_param,
			T_H264_IF_VIDEO_YUV_INFO*	decode_yuv_info
			)
{
	INT32	ret;
	
	ret = H264_Decode_Video_Stream_Set( store_num, video_stream_param );
	if( ret != H264_OK ){
		return ret;
	}
	
	ret = H264_Decode_Top_Main( req_pts, decode_yuv_info );
	if( ret != H264_OK ){
		return ret;
	}
	
	return H264_OK;
}


/**
 *	@brief		Set TOP Image.\n
 *	@param		play_info		i:Play Information of UI
 *	@param		table_index		i:Table Index for Play Index.(0 to 7)
 *	@return		H264_OK						Normal End
 *	@return		H264_ERR_INPUT_PARAM		Input Parameter Error
 *	@return		H264_ERR_FILE_ACCESS		File Access Error
 *	@return		H264_ERR_FILE_NO_SUPPORT	Not Support File Error
 *	@note		None
 *	@attention	None
*/
INT32 H264_Movie_Play_Top_Set( T_H264_IF_DECODER_PARAM* decoder_param, T_H264_IF_DISPLAY_INFO* display_info )
{
	INT32	ret;
	
	ret = H264_Decode_Top_Init( decoder_param, display_info );
	if( ret != H264_OK ){
		return ret;
	}
	
	return H264_OK;
}


INT32 H264_Movie_Play_Start_Init( VOID )
{
	INT32	ret;
	
	/* --- 5. Executes Decode main process --- */
	ret = H264_Decode_Main();
	if( ret != H264_OK ){
		H264_Err_Print(("H264_Movie_Play_Start_Init : H264_Decode_Main Error.\n"));
		return H264_NG;
	}
	
	return H264_OK;
}


/**
 *	@brief		Movie PlayBack Function.\n
 *	@param		file_id			i:File Identifier
 *	@param		table_index		i:Table Index
 *	@param		volume			i:Sound Volume
 *	@param		ttr_set			i:Time Trimming ON/OFF of File TOP
 *	@return		Result_code
 *	@note		None
 *	@attention	None
*/
INT32 H264_Movie_Play_Start( ULLONG start_pts, E_H264_IF_PLAY_DIRECTION direction, E_H264_IF_PLAY_SPEED speed, BOOL pause )
{
	return H264_Decode_Play_Start( start_pts, direction, speed, pause );
}


/* ------------------------ U.I. Function for Decode ----------------------------------------------- */
/**
 *	@brief		Check User Event in PLAY Mode.\n
 *	@param		current_mode		Current Mode
 *	@return		Next Mode
 *	@note		None
 *	@attention	None
 */
E_H264_PLAY_MODE H264_Movie_Play_Event_Check( E_H264_PLAY_MODE current_mode )
{
	/* Check Event */
	gMovie_User_Evevt = (E_H264_EVENT)H264_If_Play_Api_Check_Event();
	
	if( gMovie_User_Evevt == E_H264_EVENT_NONE ){
		return current_mode;
	}
	
	switch( gMovie_User_Evevt ){
		case E_H264_EVENT_PLAY :
			return E_H264_PLAY_MODE_PLAY;
			
		case E_H264_EVENT_PAUSE :
			return E_H264_PLAY_MODE_PAUSE;
			
		case E_H264_EVENT_REPLAY :
			return E_H264_PLAY_MODE_PLAY;
			
		case E_H264_EVENT_FLUSH :
			H264_Decode_Flush_Request();
			return E_H264_PLAY_MODE_FLUSH;
			
		case E_H264_EVENT_STOP :
			H264_Decode_Stop_Request( H264_If_Play_Api_Get_Last_Picture_Keep() );
			return E_H264_PLAY_MODE_STOP;
			
		default :
			break;
	}
	/* Not Support Event */
	gMovie_User_Evevt = E_H264_EVENT_NONE;
	
	return current_mode;
}


/*----------------------------------------------------------------------*/
/* D. H264 Common Function												*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		frame rate to Time Increment & Time Increment Resolution convert.\n
 *	@param		i:framerate				frame rate(H264_FR_xx_xxFPS)
 *	@param		o:*vop_time_increment	Time Increment(H264_TIMEINC_xx_xx)
 *	@param		o:*vop_time_inc_resol	Time Increment Resolution(H264_TIMERESO_xx_xx)
 *	@return		None
 *	@note		When framerate is an illegal value, 0 is set. 
 *	@attention	None
*/
INT32 H264_Movie_Convert_Framerate( UINT32 framerate, ULONG *vop_time_increment, ULONG *vop_time_inc_resol )
{
	*vop_time_increment = 0;
	*vop_time_inc_resol = 0;
	
	switch( framerate ){
		case E_H264_IF_FRAME_RATE_239_76:
			*vop_time_increment = H264_TIMEINC_239_76;		// 239.76fps
			*vop_time_inc_resol = H264_TIMERESO_239_76;
			break;
		case E_H264_IF_FRAME_RATE_119_88:
			*vop_time_increment = H264_TIMEINC_119_88;		// 119.88fps
			*vop_time_inc_resol = H264_TIMERESO_119_88;
			break;
		case E_H264_IF_FRAME_RATE_59_94:
			*vop_time_increment = H264_TIMEINC_59_94;		// 59.94fps
			*vop_time_inc_resol = H264_TIMERESO_59_94;
			break;
		case E_H264_IF_FRAME_RATE_47_95:
			*vop_time_increment = H264_TIMEINC_47_95;		// 47.95fps
			*vop_time_inc_resol = H264_TIMERESO_47_95;
			break;
		case E_H264_IF_FRAME_RATE_29_97:
			*vop_time_increment = H264_TIMEINC_29_97;		// 29.97fps
			*vop_time_inc_resol = H264_TIMERESO_29_97;
			break;
		case E_H264_IF_FRAME_RATE_23_97:
			*vop_time_increment = H264_TIMEINC_23_97;		// 23.97fps
			*vop_time_inc_resol = H264_TIMERESO_23_97;
			break;
		case E_H264_IF_FRAME_RATE_14_98:
			*vop_time_increment = H264_TIMEINC_14_98;		// 14.98fps
			*vop_time_inc_resol = H264_TIMERESO_14_98;
			break;
		case E_H264_IF_FRAME_RATE_9_99:
			*vop_time_increment = H264_TIMEINC_9_99;		// 9.99fps
			*vop_time_inc_resol = H264_TIMERESO_9_99;
			break;
		case E_H264_IF_FRAME_RATE_4_99:
			*vop_time_increment = H264_TIMEINC_4_99;		// 4.99fps
			*vop_time_inc_resol = H264_TIMERESO_4_99;
			break;
		case E_H264_IF_FRAME_RATE_2_99:
			*vop_time_increment = H264_TIMEINC_2_99;		// 2.99fps
			*vop_time_inc_resol = H264_TIMERESO_2_99;
			break;
		case E_H264_IF_FRAME_RATE_1_99:
			*vop_time_increment = H264_TIMEINC_1_99;		// 1.99fps
			*vop_time_inc_resol = H264_TIMERESO_1_99;
			break;
		case E_H264_IF_FRAME_RATE_0_99:
			*vop_time_increment = H264_TIMEINC_0_99;		// 0.99fps
			*vop_time_inc_resol = H264_TIMERESO_0_99;
			break;
		
		case E_H264_IF_FRAME_RATE_240_0:
			*vop_time_increment = H264_TIMEINC_240_0;		// 240fps
			*vop_time_inc_resol = H264_TIMERESO_240_0;
			break;
		case E_H264_IF_FRAME_RATE_120_0:
			*vop_time_increment = H264_TIMEINC_120_0;		// 120fps
			*vop_time_inc_resol = H264_TIMERESO_120_0;
			break;
		case E_H264_IF_FRAME_RATE_60_0:
			*vop_time_increment = H264_TIMEINC_60_0;		// 60fps
			*vop_time_inc_resol = H264_TIMERESO_60_0;
			break;
		case E_H264_IF_FRAME_RATE_50_0:
			*vop_time_increment = H264_TIMEINC_50_0;		// 50fps
			*vop_time_inc_resol = H264_TIMERESO_50_0;
			break;
		case E_H264_IF_FRAME_RATE_48_0:
			*vop_time_increment = H264_TIMEINC_48_0;		// 48fps
			*vop_time_inc_resol = H264_TIMERESO_48_0;
			break;
		case E_H264_IF_FRAME_RATE_30_0:
			*vop_time_increment = H264_TIMEINC_30_0;		// 30fps
			*vop_time_inc_resol = H264_TIMERESO_30_0;
			break;
		case E_H264_IF_FRAME_RATE_25_0:
			*vop_time_increment = H264_TIMEINC_25_0;		// 25fps
			*vop_time_inc_resol = H264_TIMERESO_25_0;
			break;
		case E_H264_IF_FRAME_RATE_24_0:
			*vop_time_increment = H264_TIMEINC_24_0;		// 24fps
			*vop_time_inc_resol = H264_TIMERESO_24_0;
			break;
		case E_H264_IF_FRAME_RATE_15_0:
			*vop_time_increment = H264_TIMEINC_15_0;		// 15fps
			*vop_time_inc_resol = H264_TIMERESO_15_0;
			break;
		case E_H264_IF_FRAME_RATE_12_5:
			*vop_time_increment = H264_TIMEINC_12_5;		// 12.5fps
			*vop_time_inc_resol = H264_TIMERESO_12_5;
			break;
		case E_H264_IF_FRAME_RATE_10_0:
			*vop_time_increment = H264_TIMEINC_10_0;		// 10fps
			*vop_time_inc_resol = H264_TIMERESO_10_0;
			break;
		case E_H264_IF_FRAME_RATE_7_5:
			*vop_time_increment = H264_TIMEINC_7_5;		// 7.5fps
			*vop_time_inc_resol = H264_TIMERESO_7_5;
			break;
		case E_H264_IF_FRAME_RATE_5_0:
			*vop_time_increment = H264_TIMEINC_5_0;		// 5fps
			*vop_time_inc_resol = H264_TIMERESO_5_0;
			break;
		case E_H264_IF_FRAME_RATE_3_0:
			*vop_time_increment = H264_TIMEINC_3_0;		// 3fps
			*vop_time_inc_resol = H264_TIMERESO_3_0;
			break;
		case E_H264_IF_FRAME_RATE_2_0:
			*vop_time_increment = H264_TIMEINC_2_0;		// 2fps
			*vop_time_inc_resol = H264_TIMERESO_2_0;
			break;
		case E_H264_IF_FRAME_RATE_1_0:
			*vop_time_increment = H264_TIMEINC_1_0;		// 1fps
			*vop_time_inc_resol = H264_TIMERESO_1_0;
			break;
		default :
			H264_Err_Print(("H264_Movie_Convert_Framerate:framerate is not support(%d)\n", framerate));
			return H264_NG;
	}
	
	return H264_OK;
}


/**
 *	@brief		Convert Endian.\n
 *	@param		i/o:input data   :input data
 *	@param		i:convert_size   :convert size
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
INT32 H264_Movie_Convert_Endian( VOID *input_data, ULONG convert_size )
{
	ULONG	loop_count;
	CHAR	work_area[64]={0};
	
	if( (convert_size < 4) || (convert_size > 64) ){
		H264_Err_Print(("H264_Movie_Convert_Endian: Covert Size Error!! %lu\n", convert_size));
		return H264_NG;
	}
	
	for(loop_count=0; loop_count<convert_size; loop_count++){
		work_area[loop_count] = ((CHAR *)input_data)[loop_count];
	}
	
	for(loop_count=1; loop_count<=convert_size; loop_count++){
		((CHAR *)input_data)[loop_count - 1] = work_area[convert_size - loop_count];
	}
	
	return H264_OK;
}


/*----------------------------------------------------------------------*/
/* F. ImageMacro Wrapper												*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		REC Size Normal Convert Function Wrapper\n
 *	@param		i:T_H264_MOVIE_REC_TBL	rec_tbl		Rec table
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 H264_Movie_Rec_Resize( T_H264_MOVIE_REC_TBL* rec_tbl )
{
	INT32						result = H264_OK;
#ifndef CO_H264_PORTING_BLOCK
	INT32						ret = H264_OK;
	UCHAR						param_buffer[(D_IM_IIP_UTIL_RR_BUF_BYTES*2) + H264_IIP_TBL_ALIGN];
	ULONG						param_buffer_addr = H264_IIP_ROUNDUP_ALIGN( (ULONG)param_buffer );	// Convet 8bytes align
	INT32						master_IF = D_IM_IIP_MASTER_IF0;
	INT32						wait_time = 200;
	INT32						set_loop;
	INT32						src_width, src_lines;
	INT32						g_src_width;
	INT32						dst_width, dst_lines;
	INT32						g_dst_width;
	ULONG						dst_y_offset;
	ULONG						dst_c_offset;
	
	T_FJ_IIP_RES_CONFIG_ARG		iip_res_config_arg;
	T_FJ_IIP_RES_CONFIG_OUT		iip_res_config_out;
	T_FJ_IIP_RES_CONFIG_OUT		iip_res_get_resource;
	static T_IM_IIP_UTIL_RR		cfg;
	
	if( rec_tbl == NULL ){
		H264_Err_Print(("--- H264_Movie_Rec_Resize : rec_tbl is NULL.\n"));
		return H264_NG;
	}
	
	while(1){
		
		dst_y_offset = (rec_tbl->dst_addr[0] & 0x0000000F);
		dst_c_offset = (rec_tbl->dst_addr[1] & 0x0000000F);
		
		H264_Movie_Memset( &iip_res_config_arg,   0, sizeof( T_FJ_IIP_RES_CONFIG_ARG ));
		H264_Movie_Memset( &iip_res_config_out, 0, sizeof( T_FJ_IIP_RES_CONFIG_OUT ));
		
		iip_res_config_arg.unit.afn				= 1;
		iip_res_config_arg.unit.sl_master_if_0	= 1;
		iip_res_config_arg.pixid				= 2;
		
		// IIP Resource Get
		ret = BF_IIP_Res_Config_Lock_BaseFW( &iip_res_config_out, &iip_res_config_arg, 1, wait_time );
		if( ret != D_FJ_IIP_ERR_OK ){
			H264_Err_Print(("--- H264_Movie_Rec_Resize : BF_IIP_Res_Config_Lock_BaseFW Error=%d\n", ret));
			result = H264_NG;
			break;
		}
		
		// saves local
		iip_res_get_resource = iip_res_config_out;
		
		// afn unit id get
		if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_AFN0 ){
			cfg.afn_unitid = E_IM_IIP_UNIT_ID_AFN0;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_AFN0;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_AFN1 ){
			cfg.afn_unitid = E_IM_IIP_UNIT_ID_AFN1;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_AFN1;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_AFN2 ){
			cfg.afn_unitid = E_IM_IIP_UNIT_ID_AFN2;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_AFN2;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_AFN3 ){
			cfg.afn_unitid = E_IM_IIP_UNIT_ID_AFN3;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_AFN3;
		}
		else{
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
			H264_Err_Print(("--- H264_Movie_Rec_Resize : IIP Resource(afn) Get Error=%lu\n", (ULONG)iip_res_get_resource.unitid_bitmask64));
#else
			H264_Err_Print(("--- H264_Movie_Rec_Resize : IIP Resource(afn) Get Error=%llu\n", iip_res_get_resource.unitid_bitmask64));
#endif
			result = H264_NG;
			break;
		}
		
		// sl unit(memory) id get
		if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL3 ){
			cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL3;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL3;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL4 ){
			cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL4;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL4;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL5 ){
			cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL5;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL5;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL6 ){
			cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL6;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL6;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL7 ){
			cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL7;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL7;
		}
		else if( iip_res_get_resource.unitid_bitmask64 & D_IM_IIP_PARAM_PLDUNIT_SL8 ){
			cfg.sl_unitid = E_IM_IIP_UNIT_ID_SL8;
			iip_res_get_resource.unitid_bitmask64 &= ~D_IM_IIP_PARAM_PLDUNIT_SL8;
		}
		else{
#if 1	// MILB : Movie porting. printf 64bit Not Supported.
			H264_Err_Print(("--- H264_Movie_Rec_Resize : IIP Resource(sl) Get Error=%lu\n", (ULONG)iip_res_get_resource.unitid_bitmask64));
#else
			H264_Err_Print(("--- H264_Movie_Rec_Resize : IIP Resource(sl) Get Error=%llu\n", iip_res_get_resource.unitid_bitmask64));
#endif
			result = H264_NG;
			break;
		}
		
		// pixel id get
		set_loop = 0;
		for( ; set_loop < H264_IIP_PIXID_MAX; set_loop++ ){
			if( iip_res_get_resource.pixid_bitmask & (1 << set_loop) ){
				cfg.src_pixid = (E_IM_IIP_PIXID)(1 << set_loop);
				iip_res_get_resource.pixid_bitmask &= ~(1 << set_loop);
				break;
			}
		}
		for( ; set_loop < H264_IIP_PIXID_MAX; set_loop++ ){
			if( iip_res_get_resource.pixid_bitmask & (1 << set_loop) ){
				cfg.dst_pixid = (E_IM_IIP_PIXID)(1 << set_loop);
				iip_res_get_resource.pixid_bitmask &= ~(1 << set_loop);
				break;
			}
		}
		if( set_loop == H264_IIP_PIXID_MAX ){
			H264_Err_Print(("--- H264_Movie_Rec_Resize : IIP Resource PIXID Get NG!!\n"));
			result = H264_NG;
			break;
		}
		if( (iip_res_get_resource.unitid_bitmask64 != 0) || (iip_res_get_resource.pixid_bitmask != 0) ){
			H264_Err_Print(("--- H264_Movie_Rec_Resize : IIP Resource Abnormal!! \n"));
			result = H264_NG;
			break;
		}
		
		cfg.bi_cubic_unitid						= E_IM_IIP_PARAM_SCUB_0;
		cfg.afn_cache_select					= E_IM_IIP_PARAM_CSEL_0;
		
		src_width								= rec_tbl->src_width;
		src_lines								= rec_tbl->src_lines;
		g_src_width								= rec_tbl->g_src_width;
		
		cfg.src.rect.left						= 0.0;
		cfg.src.rect.top						= 0.0;
		cfg.src.rect.width						= src_width;
		cfg.src.rect.lines						= src_lines;
		
		cfg.src.gbl.alpha						= D_IM_IIP_ALPHA_FALSE;
		cfg.src.gbl.frame_type					= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
		if(rec_tbl->ribery_mode == TRUE){
			cfg.src.gbl.pix_format				= E_IM_IIP_PFMT_VIDEO;
		}
		else{
			cfg.src.gbl.pix_format				= E_IM_IIP_PFMT_YCC420;
		}
		cfg.src.gbl.chunky_color.component0		= D_IM_IIP_CHUNKY_COLOR_Y0_G;
		cfg.src.gbl.chunky_color.component1		= D_IM_IIP_CHUNKY_COLOR_CB_B;
		cfg.src.gbl.chunky_color.component2		= D_IM_IIP_CHUNKY_COLOR_CR_R;
		cfg.src.gbl.chunky_color.component3		= D_IM_IIP_CHUNKY_COLOR_Y1_A;
		cfg.src.gbl.sign_Y_G					= D_IM_IIP_UNSIGNED_DATA;
		cfg.src.gbl.sign_Cb_B					= D_IM_IIP_UNSIGNED_DATA;
		cfg.src.gbl.sign_Cr_R					= D_IM_IIP_UNSIGNED_DATA;
		cfg.src.gbl.sign_D3						= D_IM_IIP_UNSIGNED_DATA;
		cfg.src.gbl.width						= src_width;
		cfg.src.gbl.lines						= src_lines;
		cfg.src.gbl.line_bytes.Y_G				= g_src_width;
		cfg.src.gbl.line_bytes.Cb_B				= g_src_width;
		cfg.src.gbl.line_bytes.Cr_R				= g_src_width;
		cfg.src.gbl.line_bytes.Alpha			= 0;
		cfg.src.gbl.addr.Y_G					= rec_tbl->src_addr[0];
		cfg.src.gbl.addr.Cb_B					= rec_tbl->src_addr[1];
		cfg.src.gbl.addr.Cr_R					= rec_tbl->src_addr[1];
		cfg.src.gbl.addr.Alpha					= 0;
		// S/L[0,3,6] is MasterI/F0 (Write).
		// S/L[1,4,7] is MasterI/F1 (Write).
		// S/L[8] is MasterI/F2 (Write).
		// S/L[2] is MasterI/F1 or 2 (Write).
		// S/L[5] is MasterI/F0 or 2 (Write).
		if( (cfg.sl_unitid == E_IM_IIP_UNIT_ID_SL3) || (cfg.sl_unitid == E_IM_IIP_UNIT_ID_SL6) ){
			cfg.src.gbl.masterIF_Y_G			= D_IM_IIP_MASTER_IF1;
			cfg.src.gbl.masterIF_Cb_B			= D_IM_IIP_MASTER_IF1;
			cfg.src.gbl.masterIF_Cr_R			= D_IM_IIP_MASTER_IF1;
			cfg.src.gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF1;
		}
		else{
			cfg.src.gbl.masterIF_Y_G			= D_IM_IIP_MASTER_IF0;
			cfg.src.gbl.masterIF_Cb_B			= D_IM_IIP_MASTER_IF0;
			cfg.src.gbl.masterIF_Cr_R			= D_IM_IIP_MASTER_IF0;
			cfg.src.gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF0;
		}
		
		dst_width								= rec_tbl->dst_width;
		dst_lines								= rec_tbl->dst_lines;
		g_dst_width								= rec_tbl->g_dst_width;
		
		if( g_dst_width % 8 ){
			H264_Warning_Print(("H264_Movie_Rec_Resize: the value of the global width(%d) is not divisible by 8.\n", g_dst_width ));
		}
		
		cfg.dst.rect.left						= 0.0;
		cfg.dst.rect.top						= 0.0;
		cfg.dst.rect.width						= dst_width;
		cfg.dst.rect.lines						= dst_lines;
		
		cfg.dst.gbl.alpha						= D_IM_IIP_ALPHA_FALSE;
		cfg.dst.gbl.frame_type					= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
		cfg.dst.gbl.pix_format					= E_IM_IIP_PFMT_YCC420;
		cfg.dst.gbl.chunky_color.component0		= D_IM_IIP_CHUNKY_COLOR_Y0_G;
		cfg.dst.gbl.chunky_color.component1		= D_IM_IIP_CHUNKY_COLOR_CB_B;
		cfg.dst.gbl.chunky_color.component2		= D_IM_IIP_CHUNKY_COLOR_CR_R;
		cfg.dst.gbl.chunky_color.component3		= D_IM_IIP_CHUNKY_COLOR_Y1_A;
		cfg.dst.gbl.sign_Y_G					= D_IM_IIP_UNSIGNED_DATA;
		cfg.dst.gbl.sign_Cb_B					= D_IM_IIP_UNSIGNED_DATA;
		cfg.dst.gbl.sign_Cr_R					= D_IM_IIP_UNSIGNED_DATA;
		cfg.dst.gbl.sign_D3						= D_IM_IIP_UNSIGNED_DATA;
		cfg.dst.gbl.width						= dst_width;
		cfg.dst.gbl.lines						= dst_lines;
		cfg.dst.gbl.line_bytes.Y_G				= g_dst_width;
		cfg.dst.gbl.line_bytes.Cb_B				= g_dst_width;
		cfg.dst.gbl.line_bytes.Cr_R				= g_dst_width;
		cfg.dst.gbl.line_bytes.Alpha			= 0;
		cfg.dst.gbl.addr.Y_G					= rec_tbl->dst_addr[0] - dst_y_offset;
		cfg.dst.gbl.addr.Cb_B					= rec_tbl->dst_addr[1] - dst_c_offset;
		cfg.dst.gbl.addr.Cr_R					= rec_tbl->dst_addr[1] - dst_c_offset;
		cfg.dst.gbl.addr.Alpha					= 0;
		// S/L[0,3,6] is MasterI/F0 (Write).
		// S/L[1,4,7] is MasterI/F1 (Write).
		// S/L[8] is MasterI/F2 (Write).
		// S/L[2] is MasterI/F1 or 2 (Write).
		// S/L[5] is MasterI/F0 or 2 (Write).
		if( (cfg.sl_unitid == E_IM_IIP_UNIT_ID_SL3) || (cfg.sl_unitid == E_IM_IIP_UNIT_ID_SL6) ){
			cfg.dst.gbl.masterIF_Y_G			= D_IM_IIP_MASTER_IF0;
			cfg.dst.gbl.masterIF_Cb_B			= D_IM_IIP_MASTER_IF0;
			cfg.dst.gbl.masterIF_Cr_R			= D_IM_IIP_MASTER_IF0;
			cfg.dst.gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF0;
		}
		if( (cfg.sl_unitid == E_IM_IIP_UNIT_ID_SL5) || (cfg.sl_unitid == E_IM_IIP_UNIT_ID_SL8) ){
			cfg.dst.gbl.masterIF_Y_G			= D_IM_IIP_MASTER_IF2;
			cfg.dst.gbl.masterIF_Cb_B			= D_IM_IIP_MASTER_IF2;
			cfg.dst.gbl.masterIF_Cr_R			= D_IM_IIP_MASTER_IF2;
			cfg.dst.gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF2;
		}
		else{
			cfg.dst.gbl.masterIF_Y_G			= D_IM_IIP_MASTER_IF1;
			cfg.dst.gbl.masterIF_Cb_B			= D_IM_IIP_MASTER_IF1;
			cfg.dst.gbl.masterIF_Cr_R			= D_IM_IIP_MASTER_IF1;
			cfg.dst.gbl.masterIF_Alpha			= D_IM_IIP_MASTER_IF1;
		}
		
		cfg.pix_depth							= D_IM_IIP_PDEPTH_8BITS;
		
		switch( rec_tbl->rotate ){
			case E_H264_IF_DISP_ROTATE_0:
				cfg.rotation		= E_IM_IIP_UTIL_ROTATE_000;
				break;
			case E_H264_IF_DISP_ROTATE_90:
				cfg.rotation		= E_IM_IIP_UTIL_ROTATE_090;
				break;
			case E_H264_IF_DISP_ROTATE_180:
				cfg.rotation		= E_IM_IIP_UTIL_ROTATE_180;
				break;
			case E_H264_IF_DISP_ROTATE_270:
				cfg.rotation		= E_IM_IIP_UTIL_ROTATE_270;
				break;
			default:
				H264_Err_Print(("--- H264_Movie_Rec_Resize : rotate param error.\n"));
				cfg.rotation		= E_IM_IIP_UTIL_ROTATE_000;
				break;
		}
		
		switch( rec_tbl->resize_mode ){
			case E_H264_RESIZE_MODE_NEAREST:
				cfg.resize_mode	= E_IM_IIP_UTIL_RESIZE_MODE_NEAREST;
				break;
			case E_H264_RESIZE_MODE_BILINEAR:
				cfg.resize_mode	= E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;
				break;
			case E_H264_RESIZE_MODE_BICUBIC:
				cfg.resize_mode	= E_IM_IIP_UTIL_RESIZE_MODE_BICUBIC;
				break;
			default:
				H264_Err_Print(("--- H264_Movie_Rec_Resize : resize param error.\n"));
				cfg.resize_mode	= E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;
				break;
		}
		
		if( ((src_width==4096)&&(src_lines==2160)) && ( dst_width==1920 ) ){
			cfg.resize_mode	= E_IM_IIP_UTIL_RESIZE_MODE_NEAREST;
		}
		else{
			cfg.resize_mode	= E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR;
		}
		
		ret = IM_IIP_Utility_ResizeRotate( &cfg, NULL, master_IF, M_SDRAM_MAP_COMMON_ADR_LOG2PHY((UINT32)param_buffer_addr), wait_time );
		
		if (ret == D_IM_IIP_OK) {
			result = H264_OK;
		}
		else{
			H264_Err_Print(("[Movie Play]IM_IIP_Utility_ResizeRotate ERR : ret=0x%x\n", ret));
			result = H264_NG;
		}
		
		break;
	}
	
	// IIP Resource Release
	ret = BF_IIP_Res_Config_Free_BaseFW( &iip_res_config_out, 1, wait_time );
	if( ret != D_FJ_IIP_ERR_OK ){
		H264_Err_Print(("--- H264_Movie_Rec_Resize : BF_IIP_Res_Config_Free_BaseFW Error=%d\n", ret));
		result = H264_NG;
	}
	
#endif	//CO_H264_PORTING_BLOCK
	
	return result;
}

/**
 *	@brief		Lcd Address Update Function Wrapper\n
 *	@param		i:lcd_info			LCD information
 *	@param		i:disp_addr			Update address
 *	@param		i:first_disp_flg	first display flag
 *	@param		i:color_matrix		Color Matrix
 *	@param		i:ribery_format		ribery format
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Movie_Lcd_Update( T_H264_IF_DISPLAY_PARAM* lcd_info, T_H264_MOVIE_ADDR* disp_addr, BOOL ribery_format )
{
#ifndef CO_DEBUG_ON_PC
	FJ_ERR_CODE							ret;
	E_BF_DISP_IMAGE_FORMAT				image_format;
	UINT32								global_width;
	FJ_DISP_YCC_ADDR					ycc_addr;
	
	ycc_addr.y_addr		= disp_addr->y_address;
	ycc_addr.cbcr_addr	= disp_addr->c_address;
	
	if( ribery_format == TRUE ){
		image_format	= E_BF_DISP_IMAGE_FORMAT_VIDEO;
		global_width	= lcd_info->global_width;
	}
	else{
		image_format	= E_BF_DISP_IMAGE_FORMAT_420;
		global_width	= lcd_info->global_width;
	}
	
	ret = BF_Disp_Set_Main_Addr( lcd_info->disp_id, &ycc_addr, image_format, E_BF_DISP_YCC_COLOR_SPACE_601, global_width );
	if( ret != FJ_ERR_OK ){
		H264_Err_Print(("--- H264_Movie_Lcd_Update() : BF_Disp_Set_Main_Addr error.[ret=%d]\n", ret));
	}
	
#endif
	return;
}

/**
 *	@brief		Lcd Interrupt Wait Function Wrapper\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Movie_Lcd_Wait( VOID )
{
#ifndef CO_DEBUG_ON_PC
	BF_Disp_Wait_Vsync( FJ_DISP_TYPE_LCD );
#else
	OS_User_Dly_Tsk(100);
#endif
}

/**
 *	@brief		Tv Address Update Function Wrapper\n
 *	@param		i:disp_addr			Update address
 *	@param		i:first_disp_flg	first display flag
 *	@param		i:color_matrix		Color Matrix
 *	@param		i:ribery_format		ribery format
 *	@param		i:img_size			image size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Movie_Tv_Update( T_H264_MOVIE_ADDR disp_addr, UCHAR* first_disp_flg, UCHAR color_matrix, UCHAR ribery_format )
{
#if 0	// Currently unused.
#ifndef CO_H264_PORTING_BLOCK
	T_H264_IF_DISPLAY_PARAM*			tv = NULL;
	T_IM_DISP_IMAGE_ADDR				tv_addr;
	T_IM_DISP_CTRL_MAIN_LAYER_TRG_LIMIT	tv_ctrl_trg_limit;
	T_IM_DISP_CTRL_MAIN_LAYER			tv_ctrl;
	E_IM_DISP_BANK						switch_bank;
	UINT32								ret;
	
	/* Display Setting Get */
	H264_If_Play_Api_Get_Tv_Setting(&tv);

	tv_addr.y_addr = disp_addr.y_address;
	tv_addr.c_addr = disp_addr.c_address;

	if( *first_disp_flg == H264_ON ){
		// Set Default Value
		tv_ctrl_trg_limit	= gTv_ctrl_trg_limit;
		tv_ctrl				= gTv_ctrl;
		if( ribery_format == H264_ON ){
			tv_ctrl.y_hga		= gTv_ctrl.y_hga;
			tv_ctrl.c_hga		= gTv_ctrl.c_hga;
			tv_ctrl.lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_RIBERY;
		}
		else{
			tv_ctrl.y_hga		= gTv_ctrl.y_hga;
			tv_ctrl.c_hga		= gTv_ctrl.c_hga;
			tv_ctrl.lidt.bit.IFMT = D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT;
		}

		// Update Info
		tv_ctrl.lisize.size.width = tv->image_width;
		tv_ctrl.lisize.size.lines = tv->image_lines;
		tv_ctrl.ldsta.bit.DSH = disp_addr.pos_x;
		tv_ctrl.ldsta.bit.DSV = disp_addr.pos_y;

		/* Set Control */
		Im_DISP_Ctrl_Main_Layer( &tv_ctrl_trg_limit, &tv_ctrl );

		if( color_matrix == H264_COLOR_REC709 ){
			// TV REC601->REC709 convert setting
			Im_DISP_Set_Matrix( E_IM_DISP_MATRIX_KIND_Y2R, g_ycc709_to_rgb );
		}
		else{
			// TV non convert setting
			Im_DISP_Set_Matrix( E_IM_DISP_MATRIX_KIND_Y2R, g_ycc601_to_rgb );
		}
		*first_disp_flg = H264_OFF;
	}
	/* Set Image Data */
	ret = Im_DISP_Get_Main_Bank(&switch_bank);
	if( ret != D_DDIM_OK) {
		H264_Err_Print(("--- H264_Movie_Tv_Update() : Im_DISP_Get_Main_Bank error.[ret=%d]\n", ret));
	}
	switch(switch_bank) {
		case E_IM_DISP_BANK_00:
		case E_IM_DISP_BANK_01:
		case E_IM_DISP_BANK_10:
			switch_bank = (E_IM_DISP_BANK)(switch_bank +1);
			break;
		default://E_IM_DISP_BANK_11
			switch_bank = E_IM_DISP_BANK_00;
			break;
	}
	//Set bank address
	ret = Im_DISP_Set_Addr(switch_bank, &tv_addr);
	if( ret != D_DDIM_OK) {
		H264_Err_Print(("--- H264_Movie_Tv_Update() : Im_DISP_Set_Addr error.[ret=%d]\n", ret));
	}
	Im_DISP_Set_Main_Bank(switch_bank);
	
#endif	//CO_H264_PORTING_BLOCK
#endif	// Currently unused.
	return;
}

/**
 *	@brief		Tv Interrupt Wait Function Wrapper\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Movie_Tv_Wait( VOID )
{
#if 0	// Currently unused.
#ifndef CO_H264_PORTING_BLOCK 
	Im_DISP_Wait_Vsync( E_IM_DISP_INTERRUPTION_SELECT_VE, E_IM_DISP_WAIT_1_TIME );
#endif	//CO_H264_PORTING_BLOCK
#endif	// Currently unused.
	return;
}



/**
 *	@brief		Hdmi Address Update Function Wrapper\n
 *	@param		i:hdmi_info			HDMI information
 *	@param		i:disp_addr			Update address
 *	@param		i:first_disp_flg	first display flag
 *	@param		i:color_matrix		Color Matrix
 *	@param		i:ribery_format		ribery format
 *	@param		i:img_size			image size
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Movie_Hdmi_Update( T_H264_IF_DISPLAY_PARAM* hdmi_info, T_H264_MOVIE_ADDR* disp_addr, BOOL ribery_format )
{
#ifndef CO_DEBUG_ON_PC
	FJ_ERR_CODE							ret;
	E_BF_DISP_IMAGE_FORMAT				image_format;
	UINT32								global_width;
	FJ_DISP_YCC_ADDR					ycc_addr;
	
	ycc_addr.y_addr		= disp_addr->y_address;
	ycc_addr.cbcr_addr	= disp_addr->c_address;
	
	if( ribery_format == TRUE ){
		image_format	= E_BF_DISP_IMAGE_FORMAT_VIDEO;
		global_width	= hdmi_info->global_width;
	}
	else{
		image_format	= E_BF_DISP_IMAGE_FORMAT_420;
		global_width	= hdmi_info->global_width;
	}
	
	ret = BF_Disp_Set_Main_Addr( hdmi_info->disp_id, &ycc_addr, image_format, E_BF_DISP_YCC_COLOR_SPACE_601, global_width );
	if( ret != FJ_ERR_OK ){
		H264_Err_Print(("--- H264_Movie_Hdmi_Update() : BF_Disp_Set_Main_Addr error.[ret=%d]\n", ret));
	}
	
#endif
	return;
}

/**
 *	@brief		Hdmi Interrupt Wait Function Wrapper\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Movie_Hdmi_Wait( VOID )
{
#ifndef CO_DEBUG_ON_PC
	BF_Disp_Wait_Vsync( FJ_DISP_TYPE_HDMI );
#else
	OS_User_Dly_Tsk(100);
#endif
	return;
}

/*----------------------------------------------------------------------*/
/* H. DD Wrapper: Ribery												*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		ribery reset.\n
 *	@param		INT32 flag
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Ribery_Reset( E_H264_TOP_RIBERY_RESET reset )
{
#ifndef CO_H264_PORTING_BLOCK
	if( reset == E_H264_TOP_RIBERY_RESET_ON ){
		Dd_Top_Reset_CRRRS_RIBSR();						// RIBSR Cancellation
	}
	else{
		Dd_Top_Clear_CRRRS_RIBSR();						// RIBSR reset
	}
	Dd_ARM_Dsb_Poc();
#endif	//CO_H264_PORTING_BLOCK
}

/**
 *	@brief		set ribery core mode.\n
 *	@param		UINT32 core_mode
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Ribery_Core_Mode_Set( UINT32 core_mode )
{
	gH264_Ribery_Core_Mode = core_mode;
	return;
}

/**
 *	@brief		set ribery core mode.\n
 *	@param		UINT32 core_mode
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Ribery_Core_Mode_Get( UINT32* core_mode )
{
	*core_mode = gH264_Ribery_Core_Mode;
	return;
}

/**
 *	@brief		ribery pmu set.\n
 *	@param		INT32 pmu
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Ribery_PMU_Setting( INT32 pmu )
{
#ifndef CO_H264_PORTING_BLOCK
	ULONG status;
	if( pmu == H264_ON ){
		Dd_PMU_PD_On( E_DD_PMU_PD_TYPE_H264 );
	}
	else{
		Dd_PMU_PD_Off( E_DD_PMU_PD_TYPE_H264 );
	}
	
	Dd_PMU_Get_Status( E_DD_PMU_PD_TYPE_H264, &status );
	H264_Info_Print(("--- H264 PMU after status() [status=%ld]\n", status));
#endif	//CO_H264_PORTING_BLOCK
}


/**
 *	@brief		ribery clock set.\n
 *	@param		INT32 clock
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Ribery_Clock_Set( INT32 clock )
{
#ifndef CO_H264_PORTING_BLOCK
	Dd_Top_Set_CLKSEL12_RIBCLK( clock );
#endif	//CO_H264_PORTING_BLOCK
}

/**
 *	@brief		ribery clock stop.\n
 *	@param		INT32 flag
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Ribery_Clock_Stop( E_H264_TOP_RIBERY_CLOCK_STOP clk_stop )
{
#ifndef CO_H264_PORTING_BLOCK
	if( clk_stop == E_H264_TOP_RIBERY_CLOCK_STOP_ON ){
		Dd_Top_Set_CLKSTOP8_RIBCK( 1 );						// clock Stop
	}
	else{
		Dd_Top_Set_CLKSTOP8_RIBCK( 0 );						// clock Start
	}
	Dd_ARM_Dsb_Poc();
#endif	//CO_H264_PORTING_BLOCK
}

/**
 *	@brief		ribery AHB clock start.\n
 *	@param		INT32 flag
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Ribery_AHClock_Start( UCHAR* ahclk_count )
{
#ifndef CO_H264_PORTING_BLOCK
	Dd_Top_Start_Clock( ahclk_count, &Dd_Top_Get_CLKSTOP8(), ~D_DD_TOP_RIBAH_BIT );
#endif	//CO_H264_PORTING_BLOCK
}


/**
 *	@brief		ribery AHB clock start.\n
 *	@param		INT32 flag
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Ribery_AHClock_Stop( UCHAR* ahclk_count )
{
#ifndef CO_H264_PORTING_BLOCK
	Dd_Top_Stop_Clock( ahclk_count, &Dd_Top_Get_CLKSTOP8(), D_DD_TOP_RIBAH_BIT );
#endif	//CO_H264_PORTING_BLOCK
}

/**
 *	@brief		SDRAM ch check.\n
 *	@param		i:check_address			Check Address
 *	@return		sdram_channel			SDRAM channel
 *	@note		None
 *	@attention	None
 */
ULONG H264_Movie_Sdram_Ch_Check( ULONG check_address )
{
	ULONG	sdram_channel;
	
	if( check_address >= SDRAM_ADR_CH1_TOP  ){
		sdram_channel = SDRAM_ADR_CH1_TOP;
	}
	else{
		sdram_channel = SDRAM_ADR_CH0_TOP;
	}
	
	return sdram_channel;
}

/**
 *	@brief		sdramc ribery clock(bus interface) enable.\n
 *	@param		INT32 flag
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Sdramc_BusIf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH ch, E_H264_SDRAMC_RIBERY_CLOCK_PORT port, E_H264_SDRAMC_RIBERY_CLOCK_EN enable )
{
#ifndef CO_H264_PORTING_BLOCK
	E_IM_SDRAMC_CH		set_ch;
	E_IM_SDRAMC_PORT	set_port;
	E_IM_SDRAMC_EN		set_enable;
	E_IM_SDRAMC_ACCESS	set_access;
	
	// channel
	if( ch == E_H264_SDRAMC_RIBERY_CLOCK_CH_0 ){
		set_ch = E_IM_SDRAMC_CH_0;
	}
	else{
		set_ch = E_IM_SDRAMC_CH_1;
	}
	
	// port
	if( port == E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB0 ){
		set_port = E_IM_SDRAMC_PORT_RB0;
	}
	else if( port == E_H264_SDRAMC_RIBERY_CLOCK_PORT_RB1 ){
		set_port = E_IM_SDRAMC_PORT_RB1;
	}
	else{
		set_port = E_IM_SDRAMC_PORT_RB2;
	}
	
	// enable
	if( enable == E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE ){
		set_enable = E_IM_SDRAMC_EN_ENABLE;
	}
	else{
		set_enable = E_IM_SDRAMC_EN_DISABLE;
	}
	
	// access (Because this is ignored by the driver, a fixed value set.)
	set_access = E_IM_SDRAMC_ACCESS_READ;
	
	Im_SDRAMC_Set_Clock_Enable_BusIf( set_ch, set_port, set_access, set_enable );
	
	return;
#endif
}

/**
 *	@brief		sdramc ribery clock(data buf) enable.\n
 *	@param		INT32 flag
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Sdramc_DataBuf_Ribery_Clock_Enable( E_H264_SDRAMC_RIBERY_CLOCK_CH ch, E_H264_SDRAMC_RIBERY_CLOCK_DATABUF dbuf, E_H264_SDRAMC_RIBERY_CLOCK_EN enable )
{
#ifndef CO_H264_PORTING_BLOCK
	E_IM_SDRAMC_CH		set_ch;
	E_IM_SDRAMC_DATABUF	set_dbno;
	E_IM_SDRAMC_EN		set_enable;
	
	// channel
	if( ch == E_H264_SDRAMC_RIBERY_CLOCK_CH_0 ){
		set_ch = E_IM_SDRAMC_CH_0;
	}
	else{
		set_ch = E_IM_SDRAMC_CH_1;
	}
	
	// dbuf
	if( dbuf == E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB0 ){
		set_dbno = E_IM_SDRAMC_DATABUF_10;
	}
	else if( dbuf == E_H264_SDRAMC_RIBERY_CLOCK_DATABUF_RB1 ){
		set_dbno = E_IM_SDRAMC_DATABUF_11;
	}
	else{
		set_dbno = E_IM_SDRAMC_DATABUF_12;
	}
	
	// enable
	if( enable == E_H264_SDRAMC_RIBERY_CLOCK_EN_ENABLE ){
		set_enable = E_IM_SDRAMC_EN_ENABLE;
	}
	else{
		set_enable = E_IM_SDRAMC_EN_DISABLE;
	}
	
	Im_SDRAMC_Set_Clock_Enable_DataBuf( set_ch, set_dbno, set_enable );
	
	return;
#endif	//CO_H264_PORTING_BLOCK
}

/**
 *	@brief		ARM Data Synchronization Barrier and L2 Cache Sync.\n
 *	@param		INT32 flag
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Arm_Ribery_Dsb_Poc( VOID )
{
	Dd_ARM_Dsb_Poc();
}

/*----------------------------------------------------------------------*/
/* M. Standard Library Wrapper											*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		Set initialization data to memory area\n
 *	@param		i:ptr		Pointer
 *	@param		i:value		Setting Data
 *	@param		i:size		Size
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Memset( VOID* ptr, UINT32 value,  ULONG size )
{
	memset( ptr, value, size );
	return;
}

/**
 *	@brief		Copy Data to memory area.\n
 *	@param		i:ptr1		Pointer 1
 *	@param		i:ptr2		Pointer 2
 *	@param		i:size		Size
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Memcpy( VOID* ptr1, VOID* ptr2,  ULONG size )
{
	memcpy( ptr1, ptr2, size );
	return;
}


/**
 * malloc wrapper for mpl area
 * @param size_t size
 * @return VOID* allocated pointer
 */
static VOID* h264_movie_malloc_mpl( size_t size )
{
	OS_USER_ER		ercd;
	OS_USER_VP		blk = (OS_USER_VP)0;
	
	ercd = OS_User_Pget_Mpl( MPKID_H264, (INT32)size, &blk );
	if( ercd != D_OS_USER_E_OK ){
		H264_Info_Print(("------ malloc NG on File Format area ----\n"));
		return(0);
	}
	return (VOID*)blk;
}


/**
 * malloc wrapper for mpl area
 * @param size_t size
 * @return VOID* allocated pointer
 */
static VOID h264_movie_free_mpl( VOID* ptr )
{
	INT32	err;
	
	err = OS_User_Rel_Mpl( MPKID_H264, (OS_USER_VP)ptr );
	if( err != D_OS_USER_E_OK ){
		err = err;
	}
	return;
}


/**
 *	@brief		Hunt the memory area.\n
 *	@param		i:size				Memory Area Size
 *	@return		Not equall NULL		Address of huntted area
 *	@return		NULL				Hunt NG
 *	@note		None
 *	@attention	None
*/
VOID* H264_Movie_Malloc( ULONG size )
{
	INT32*	pointer;
	
	pointer = NULL;
	pointer = (INT32*)h264_movie_malloc_mpl( size );
	return pointer;
}


/**
 *	@brief		Free memory area.\n
 *	@param		i:ptr		Pointer
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Free( VOID* ptr )
{
	h264_movie_free_mpl( ptr );
	return;
}

/*----------------------------------------------------------------------*/
/* N. Bit Stream Control												*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		Set Bit Stream Pointer Address.\n
 *	@param		i:bit_stream       :Bit Stream Information
 *				i:pointer_address  :Pointer Address
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Movie_Bs_Set_Pointer_Address( T_H264_MOVIE_STREAM_CONTROL* bit_stream, UCHAR* pointer_address )
{
	bit_stream->bit_remain = 8;
	bit_stream->pointer = pointer_address;
	return;
}

/**
 *	@brief		Skip Bit Stream Pointer Address.\n
 *	@param		i:bit_stream    :Bit Stream Information
 *				i:skip_bytes    :Skip Bytes
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Movie_Bs_Skip_Pointer_Address( T_H264_MOVIE_STREAM_CONTROL* bit_stream, ULONG skip_bytes )
{
	bit_stream->bit_remain = 8;
	bit_stream->pointer += skip_bytes;
	return;
}

/**
 *	@brief		Skip Bit Stream Pointer Scope Bit.\n
 *	@param		i:bit_stream   :Bit Stream Information
 *				i:skip_bits    :Skip Bits
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H264_Movie_Bs_Skip_Pointer_Scope_Bit( T_H264_MOVIE_STREAM_CONTROL* bit_stream, ULONG skip_bits )
{
	ULONG  skip_loop  = 0;
	for( skip_loop=0; skip_loop < skip_bits; skip_loop++ ){
		bit_stream->bit_remain--;
		if( bit_stream->bit_remain == 0 ){
			bit_stream->pointer++;
			bit_stream->bit_remain = 8;
		}
	}
	return;
}


/**
 *	@brief		Get Bit Stream Pointer.\n
 *	@param		i:bit_stream   :Bit Stream Information
 *	@return		Pointer Address
 *	@note		None
 *	@attention	None
 */
UCHAR* H264_Movie_Bs_Get_Pointer( T_H264_MOVIE_STREAM_CONTROL* bit_stream )
{
	return bit_stream->pointer;
}

/**
 *	@brief		Get Bit Stream Remain bits.\n
 *	@param		i:bit_stream   :Bit Stream Information
 *	@return		Remain bits
 *	@note		None
 *	@attention	None
 */
UCHAR H264_Movie_Bs_Get_Remain_Bits( T_H264_MOVIE_STREAM_CONTROL* bit_stream )
{
	return bit_stream->bit_remain;
}


/**
 *	@brief		Bit Stream Read(unsigend int 1bit).\n
 *	@param		i:bit_stream   :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
ULONG H264_Movie_Bs_Read_U_1bit( T_H264_MOVIE_STREAM_CONTROL* bit_stream )
{
	UINT32 read_value = 0;
	
	bit_stream->bit_remain--;
	read_value = ( *(bit_stream->pointer) >> bit_stream->bit_remain ) & 0x1;
	if( bit_stream->bit_remain == 0 ){
		bit_stream->pointer++;
		bit_stream->bit_remain = 8;
	}
	
	return read_value;
}

/**
 *	@brief		Bit Stream Read(unsigned n bit).\n
 *	@param		i:bit_stream  :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
ULONG H264_Movie_Bs_Read_U_Nbit( T_H264_MOVIE_STREAM_CONTROL* bit_stream, UCHAR read_num )
{
	UINT32 read_value = 0;
	UINT32 target_bit = 0;
	UCHAR  read_loop  = 0;
	
	for( read_loop=0; read_loop < read_num; read_loop++ ){
		target_bit = read_num - read_loop - 1;
		read_value |= ( H264_Movie_Bs_Read_U_1bit(bit_stream) << target_bit );
	}
	
	return read_value;
}

/**
 *	@brief		Bit Stream Read(unsined int Exp-Golomb-coded).\n
 *	@param		i:bit_stream   :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
INT32 H264_Movie_Bs_Read_U_E( T_H264_MOVIE_STREAM_CONTROL* bit_stream )
{
	UINT32 read_value = 0;
	INT32  read_loop  = 0;
	
	for( read_loop = -1; read_value == 0; read_loop++ ){
		read_value = H264_Movie_Bs_Read_U_1bit(bit_stream);
	}
	
	read_value = ((1 << read_loop) - 1) + H264_Movie_Bs_Read_U_Nbit(bit_stream, read_loop);
	
    return read_value;
}

/**
 *	@brief		Bit Stream Read(sined int Exp-Golomb-coded).\n
 *	@param		i:bit_stream    :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
INT32 H264_Movie_Bs_Read_S_E( T_H264_MOVIE_STREAM_CONTROL* bit_stream )
{
	UINT32 read_value = 0;
	
	read_value = H264_Movie_Bs_Read_U_E(bit_stream);
	if( read_value & 0x1 ){
		read_value = (read_value+1)/2;
	}
	else{
		read_value = -(read_value/2);
	}
	
	return read_value;
}


/**
 *	@brief		Bit Stream Write(unsigend int 1bit).\n
 *	@param		i:bit_stream    :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
VOID H264_Movie_Bs_Write_U_1bit( T_H264_MOVIE_STREAM_CONTROL* bit_stream, INT32 write_value )
{
	bit_stream->bit_remain--;
	
	if( (write_value & 0x01) == 1 ){
		// set value is "1"
		(*(bit_stream->pointer)) |= (1 << bit_stream->bit_remain);
	}
	else{
		// set value is "0"
		(*(bit_stream->pointer)) &= ~(1 << bit_stream->bit_remain);
	}
	if( bit_stream->bit_remain == 0 ){
		bit_stream->pointer++;
		bit_stream->bit_remain = 8;
	}
	
	return;
}


/**
 *	@brief		Bit Stream Write(unsigned n bit).\n
 *	@param		i:bit_stream    :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
VOID H264_Movie_Bs_Write_U_Nbit( T_H264_MOVIE_STREAM_CONTROL* bit_stream, UCHAR write_num, INT32 write_value )
{
	INT32  write_loop = 0;
	UINT32 target_bit = 0;
	
	for( write_loop = 0; write_loop < write_num; write_loop++ ) {
		target_bit = write_num - write_loop - 1;
		H264_Movie_Bs_Write_U_1bit(bit_stream, (write_value >> target_bit) & 0x01 );
	}
	
	return;
}

/*----------------------------------------------------------------------*/
/* O. Clean & Flush														*/
/*----------------------------------------------------------------------*/

/**
 *	@brief		cache clean.\n
 *	@param		i:addr	Pointer
 *	@param		i:size	Size
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Cache_Clean( ULONG addr, ULONG size )
{
#ifndef CO_H264_PORTING_BLOCK
	Dd_ARM_Dmb_Pou();
	DDIM_User_L1l2cache_Clean_Addr( (UINT32)addr, (UINT32)size );
#endif	//CO_H264_PORTING_BLOCK
}

/**
 *	@brief		cache flush.\n
 *	@param		i:addr	Pointer
 *	@param		i:size	Size
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Cache_Flush( ULONG addr, ULONG size )
{
#ifndef CO_H264_PORTING_BLOCK
	Dd_ARM_Dmb_Pou();
	DDIM_User_L1l2cache_Flush_Addr( (UINT32)addr, (UINT32)size );
#endif	//CO_H264_PORTING_BLOCK
}

/**
 *	@brief		cache clean and flush.\n
 *	@param		i:addr	Pointer
 *	@param		i:size	Size
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Cache_Clean_Flush( ULONG addr, ULONG size )
{
#ifndef CO_H264_PORTING_BLOCK
	Dd_ARM_Dmb_Pou();
	DDIM_User_L1l2cache_Clean_Flush_Addr( (UINT32)addr, (UINT32)size );
#endif	//CO_H264_PORTING_BLOCK
}


/*----------------------------------------------------------------------*/
/* P. DI & EI															*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		DI.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
*/
INT32 H264_Movie_Ribery_DI( VOID )
{
#ifndef CO_H264_PORTING_BLOCK
	return BF_Arm_Lock( &gH264_Ribery_CS );
#endif	//CO_H264_PORTING_BLOCK
}

/**
 *	@brief		EI.\n
 *	@param		Status
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID H264_Movie_Ribery_EI( INT32 status )
{
#ifndef CO_H264_PORTING_BLOCK
	BF_Arm_Unlock( &gH264_Ribery_CS, (UINT32)status );
#endif	//CO_H264_PORTING_BLOCK
}

VOID H264_Movie_SGDE_Resize( T_H264_MOVIE_REC_TBL* rec_tbl )
{
	T_IP_GE_CONFIG geConfig;
	T_IP_GE_LGRID local;
	T_IP_GE_XFORM xform;
	INT32 ercd;
	VOID *heapBuffer = (void*)SDRAM_ADR_CH1_RTOS_IMAGE_WORK; // TODO
	T_IP_GE_IMAGE imgInput, imgResult;

	ercd = IP_AHIP_Open(0);
	if (ercd != 0) {
		H264_Err_Print(("Error : AhipOpen() : %x n", ercd ));
		goto __cleanup;
	}

	geConfig.flags = 0;
	geConfig.fill[0] = 0;
	geConfig.fill[1] = 0x80;
	geConfig.fill[2] = 0x80;
	geConfig.split_y = rec_tbl->src_lines >> 1;
	ercd = IP_GE_Open( &geConfig, (VOID*)heapBuffer, 6*1024*1024 );
	if (ercd != 0) {
		H264_Err_Print(("Error : IP_GE_Open() : %x n", ercd ));
		goto __cleanup;
	}
#if defined(CO_ES3_HARDWARE)
	imgInput.format = E_IP_GE_FMT_YUV_420_B;
#else
#warning Video format is not supported from ES1
#endif
	imgInput.width = rec_tbl->src_width;
	imgInput.height = rec_tbl->src_lines;
	imgInput.planes[0].stride = rec_tbl->g_src_width;
	imgInput.planes[0].data = (void*)rec_tbl->src_addr[0];
	imgInput.planes[1].stride = rec_tbl->g_src_width;
	imgInput.planes[1].data = (void*)rec_tbl->src_addr[1];
	imgResult.format = E_IP_GE_FMT_YUV_420_P2;
	imgResult.width = rec_tbl->dst_width;
	imgResult.height = rec_tbl->dst_lines;
	imgResult.planes[0].stride = rec_tbl->g_dst_width;
	imgResult.planes[0].data = (void*)rec_tbl->dst_addr[0];
	imgResult.planes[1].stride = rec_tbl->g_dst_width;
	imgResult.planes[1].data = (void*)rec_tbl->dst_addr[1];

	// Horizontal flip setting
	xform.a[0] = xform.a[4] = 1.0;
	xform.a[1]		= 0.0;
	xform.a[2]		= 0.0;
	xform.a[3]		= 0.0;
	xform.a[5]		= 0.0;
	xform.a[6]		= 0.0;
	xform.a[7]		= 0.0;
	xform.a[8]		= 1.0;
	xform.next		= 0;

	local.width		= ( ( rec_tbl->dst_width + D_LOCAL_GRID_H_SIZ - 1 ) / D_LOCAL_GRID_H_SIZ ) + 1;
	local.height	= ( ( rec_tbl->dst_lines + D_LOCAL_GRID_V_SIZ - 1 ) / D_LOCAL_GRID_V_SIZ ) + 1;
	local.pitchX	= D_LOCAL_GRID_H_PIT;
	local.pitchY	= D_LOCAL_GRID_V_PIT;
	local.outputX	= 0;
	local.outputY	= 0;
	local.nodes		= NULL;
	local.xform		= NULL;

	ercd = IP_GE_SetCorrectParameter( NULL, 1, &local, xform.a );
	if (ercd != 0) {
		H264_Err_Print(("Error : IP_GE_SetCorre ctParameter() : %x n", ercd ));
		goto __cleanup;
	}

	ercd = IP_GE_Process( &imgInput, &imgResult, NULL, NULL, 1 );
	if (ercd != D_IP_GE_OK) {
		H264_Err_Print(("Error : IP_GE_Process() : %x n", ercd ));
		goto __cleanup;
	}

__cleanup:
	IP_GE_Close();
	IP_AHIP_Close(0);
}
