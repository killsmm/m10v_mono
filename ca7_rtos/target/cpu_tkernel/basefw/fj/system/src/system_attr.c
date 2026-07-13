/**
 * @file		system_attr.c
 * @brief		system attribute process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_mode.h"
#include "fj_param.h"
// --- REMOVE_DCF_EXIF BEGIN ---
#include "fj_dcf.h"
#include "exif_attr.h"
#include "dcf_attr.h"
#include "dcf.h"
// --- REMOVE_DCF_EXIF END ---
#include "system_attr.h"
#include "sdram_map_common.h"
#include "media_attr.h"
#include "debug.h"
#include "movie_video.h"
#include "osd_draw_comm.h"
#include "mdf_wrapper.h"
#include "play_attr.h"
#include "movie_video_attr.h"
#include "still_attr.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// --- REMOVE_CAMERAIF BEGIN ---
typedef struct {
	T_FJ_COMMON_DATE_TIME date_time;
	OS_USER_SYSTIM system_time;
} T_SYSTEM_ATTR_DATE_TIME_MNG;
// --- REMOVE_CAMERAIF END ---

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
// --- REMOVE_CAMERAIF BEGIN ---
static T_SYSTEM_ATTR_DATE_TIME_MNG g_system_atrr_base_date_time_mng = {
	{
		.year	= 2016,
		.month	= 1,
		.day	= 1,
		.hour	= 0,
		.min	= 0,
		.sec	= 0,
		.milsec	= 0,
	},
	0,
};
// --- REMOVE_CAMERAIF END ---

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/**
 * @brief		Get Parameter
 * @param		i:wParamID		Parameter ID
 * @param		o:pwValue		Pointer of Parameter Value
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_getparam( USHORT wParamID, USHORT* pwValue )
{
	switch( wParamID ){
		/* DCF */
		case FJ_PARA_DCF_DUPLICATE_OPERATION:
// --- REMOVE_DCF_EXIF BEGIN ---
			*pwValue = BF_Dcf_Attr_Get_Attr_Dupli_Ope();
// --- REMOVE_DCF_EXIF END ---
			break;
		default:
			return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Set Parameter
 *	@param		i:wParamID		Parameter ID
 *	@param		i:wValue		Parameter Value
 *	@return		None
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_setparam( USHORT wParamID, USHORT wValue )
{
	FJ_ERR_CODE ret = FJ_ERR_OK;

	switch( wParamID ){
		/* Interrupt */
		case FJ_PARA_HOST_MSG_BLOCK:
		case FJ_PARA_HOST_MSG_UNBLOCK:
			BF_Debug_Print_Information(("please use fj_confighostmessage()."));
//			ret = FJ_setparam_interrupt( wParamID, wValue );
			break;

		/* Memory Manupulate */
		case FJ_PARA_TL_TOO_MANY_BAD_BLOCKS_CALLBACK_FUNC:
		case FJ_PARA_TL_SYSTEM_DATA_ERROR_CALLBACK_FUNC:
		case FJ_PARA_TL_ERROR_STATUS:
//			ret = FJ_setparam_memory( wParamID, wValue );
			break;

		/* General */
		case FJ_PARA_MEM_FILE_NAME:
//			ret = FJ_setparam_general( wParamID, wValue );
			break;

		/* MEDIA */
		case FJ_PARA_ASYNC_MOUNT:
			if( (wValue ==	1) || (wValue == 0) ){
				BF_Media_Attr_Set_Attr_Sync(wValue);
				ret = FJ_ERR_OK;
			}
			else{
				ret = FJ_ERR_NG;
			}
			break;

		case FJ_PARA_SET_VOLUME_LABEL:
			if( (wValue ==	1) || (wValue == 0) ){
				BF_Media_Attr_Set_Attr_Volume_Label(wValue);
				ret = FJ_ERR_OK;
			}
			else{
				ret = FJ_ERR_NG;
			}
			break;

		case FJ_PARA_FCU_OPERATION_TIME_OUT:
			BF_Media_Attr_Set_Attr_Time_Out(wValue);
			break;

		case FJ_PARA_EXPECTED_DPOF_FILE_SIZE:
			BF_Media_Attr_Set_Attr_Dpof_Size(wValue * 1024);
			break;

		case FJ_PARA_EXTERNAL_MEDIA:
			if (((E_BF_MEDIA_ATTR_EXT)wValue == E_BF_MEDIA_ATTR_EXT_SD) || ((E_BF_MEDIA_ATTR_EXT)wValue == E_BF_MEDIA_ATTR_EXT_SD1)) {
				if (BF_Media_Attr_Set_Attr_Ext_Media((E_BF_MEDIA_ATTR_EXT)wValue) == 0) {
					ret = FJ_ERR_OK;
				}
				else {
					ret = FJ_ERR_NG;
				}
			}
			else {
				ret = FJ_ERR_NG;
			}
			break;

// --- REMOVE_DCF_EXIF BEGIN ---
		/* Exif */
		case FJ_PARA_MAKER_NOTE_SIZE:
			if( wValue > FJ_CUSTOM_EXIF_MAKERNOTE_S_MAX ){
				ret = FJ_ERR_NG;
			}
			BF_Exif_Attr_Set_Attr_Maker_Note_Size(wValue);
			break;

//		case FJ_PARA_EXIF_TAG_AUTO :
//		case FJ_PARA_MAX_CUSTOM_MARKER_SIZE :
		case FJ_PARA_APPN_SIZE:
			BF_Exif_Attr_Set_Attr_Appn_Size(wValue);
			break;
// --- REMOVE_DCF_EXIF END ---

		case FJ_PARA_SCR_CLASS1_HSIZE:
			if(wValue > 640){
				return FJ_ERR_NG;
			}
			BF_Still_Attr_Set_Size_Screen_Vga_Width(wValue);
			break;

		case FJ_PARA_SCR_CLASS1_VSIZE:
			if(wValue > 480){
				return FJ_ERR_NG;
			}
			BF_Still_Attr_Set_Size_Screen_Vga_Lines(wValue);
			break;
//			ret =FJ_setparam_exif( wParamID, wValue );

		case FJ_PARA_SCR_CLASS2_HSIZE:
			if(wValue > 1920){
				return FJ_ERR_NG;
			}
			BF_Still_Attr_Set_Size_Screen_Hd_Width(wValue);
			break;

		case FJ_PARA_SCR_CLASS2_VSIZE:
			if(wValue > 1080){
				return FJ_ERR_NG;
			}
			BF_Still_Attr_Set_Size_Screen_Hd_Lines(wValue);
			break;

// --- REMOVE_DCF_EXIF BEGIN ---
		/* DCF */
		case FJ_PARA_DCF_LAST_KEY:
//			ret = FJ_setparam_dcf( wParamID, wValue );
			break;

		case FJ_PARA_DCF_DUPLICATE_OPERATION:
			if((wValue == 0) || (wValue == 1)){
				BF_Dcf_Attr_Set_Attr_Dupli_Ope(wValue);
			}
			else{
				ret = FJ_ERR_NG;
			}
			break;

		case FJ_PARA_DCF_EDIT_FILE_NOT_ADD_DATE_DB_:
			BF_Dcf_Attr_Set_Not_Add_Date_DB(wValue);
			break;
// --- REMOVE_DCF_EXIF END ---

			/* Still Capture */
#if 0
		case FJ_PARA_QVGA_CR:
			if((wValue > 44) && (wValue != 0xFFFF)){
				return FJ_ERR_NG;
			}
			BF_Still_Attr_Set_Comp_Screen_Vga((FJ_COMPRESSION_RATIO)wValue);
			break;
#endif

		case FJ_PARA_JPEG_CUT_THRESHOLD:
			if(wValue > 100){
				return FJ_ERR_NG;
			}
			BF_Still_Attr_Set_Jpeg_Threshold(wValue);
			break;

		case FJ_PARA_VIEW_YUV_1_SET_OUTPUT_SIZE:
			BF_Still_Attr_Set_View_YUV_Manipulation_1st_Size(wValue);
			break;

		case FJ_PARA_VIEW_YUV_2_SET_OUTPUT_SIZE:
			BF_Still_Attr_Set_View_YUV_Manipulation_2nd_Size(wValue);
			break;

		case FJ_PARA_VIEW_YUV_1_SET_OUTPUT_AREA:
			BF_Still_Attr_Set_View_YUV_Manipulation_1st_Area(wValue);
			break;

		case FJ_PARA_VIEW_YUV_2_SET_OUTPUT_AREA:
			BF_Still_Attr_Set_Capture_YUV_Manipulation_2nd_Area(wValue);
			break;

		case FJ_PARA_CAPTURE_YUV_1_SET_OUTPUT_SIZE:
			BF_Still_Attr_Set_Capture_YUV_Manipulation_1st_Size(wValue);
			break;

		case FJ_PARA_CAPTURE_YUV_2_SET_OUTPUT_SIZE:
			BF_Still_Attr_Set_Capture_YUV_Manipulation_2nd_Size(wValue);
			break;

		case FJ_PARA_CAPTURE_YUV_3_SET_OUTPUT_SIZE:
			BF_Still_Attr_Set_Capture_YUV_Manipulation_3rd_Size(wValue);
			break;

		case FJ_PARA_CAPTURE_YUV_4_SET_OUTPUT_SIZE:
			BF_Still_Attr_Set_Capture_YUV_Manipulation_4th_Size(wValue);
			break;

		case FJ_PARA_CAPTURE_YUV_1_SET_OUTPUT_AREA:
			BF_Still_Attr_Set_Capture_YUV_Manipulation_1st_Area(wValue);
			break;

		case FJ_PARA_CAPTURE_YUV_2_SET_OUTPUT_AREA:
			BF_Still_Attr_Set_Capture_YUV_Manipulation_2nd_Area(wValue);
			break;

		case FJ_PARA_CAPTURE_YUV_3_SET_OUTPUT_AREA:
			BF_Still_Attr_Set_Capture_YUV_Manipulation_3rd_Area(wValue);
			break;

		case FJ_PARA_CAPTURE_YUV_4_SET_OUTPUT_AREA:
			BF_Still_Attr_Set_Capture_YUV_Manipulation_4th_Area(wValue);
			break;

		/* Thumbnail */
		case FJ_PARA_THUMB_DRAW_WIDTH_LCD_TV:
			BF_Play_Attr_Set_Thumb_Draw_Width_Lcd( wValue );
			break;

		case FJ_PARA_THUMB_DRAW_LINES_LCD_TV:
			BF_Play_Attr_Set_Thumb_Draw_Lines_Lcd( wValue );
			break;

		case FJ_PARA_THUMB_DRAW_WIDTH_HD:
			BF_Play_Attr_Set_Thumb_Draw_Width_Hdmi( wValue );
			break;

		case FJ_PARA_THUMB_DRAW_LINES_HD:
			BF_Play_Attr_Set_Thumb_Draw_Lines_Hdmi( wValue );
			break;

		/* --- Movie --- */
		case FJ_PARA_MOVIE_REC_SYNC:
			break;
			
		case FJ_PARA_MOVIE_PLAY_SYNC:
#if 0	// not used.
#if 0	// *H.264 only*
			BF_Movie_Video_Attr_Set_PB_Clip_Cmd_Sync(wValue);
#else
			BF_Movie_Video_Attr_Set_PB_Clip_Cmd_Sync(FJ_MOVIE_H264_CH0, wValue);
#endif
#endif
			break;

		case FJ_PARA_MOVIE_SCR_CLASS1_HSIZE:
#if 0	// not used.
			if(wValue > WIDTH_VGA){
				return FJ_ERR_NG;
			}
#if 0	// *H.264 only*
			BF_Movie_Video_Attr_Set_Size_Screen_Vga_Width(wValue);
#else
			BF_Movie_Video_Attr_Set_Size_Screen_Vga_Width(FJ_MOVIE_H264_CH0, wValue);
#endif
#endif
			break;

		case FJ_PARA_MOVIE_SCR_CLASS1_VSIZE:
#if 0	// not used.
			if(wValue > LINES_VGA){
				return FJ_ERR_NG;
			}
#if 0	// *H.264 only*
			BF_Movie_Video_Attr_Set_Size_Screen_Vga_Lines(wValue);
#else
			BF_Movie_Video_Attr_Set_Size_Screen_Vga_Lines(FJ_MOVIE_H264_CH0, wValue);
#endif
#endif
			break;

		case FJ_PARA_MOVIE_SCR_CLASS2_HSIZE:
#if 0	// not used.
			if(wValue > WIDTH_3M_16_9){
				return FJ_ERR_NG;
			}
#if 0	// *H.264 only*
			BF_Movie_Video_Attr_Set_Size_Screen_Hd_Width(wValue);
#else
			BF_Movie_Video_Attr_Set_Size_Screen_Hd_Width(FJ_MOVIE_H264_CH0, wValue);
#endif
#endif
			break;

		case FJ_PARA_MOVIE_SCR_CLASS2_VSIZE:
#if 0	// not used.
			if(wValue > LINES_3M_16_9){
				return FJ_ERR_NG;
			}
#if 0	// *H.264 only*
			BF_Movie_Video_Attr_Set_Size_Screen_Hd_Lines(wValue);
#else
			BF_Movie_Video_Attr_Set_Size_Screen_Hd_Lines(FJ_MOVIE_H264_CH0, wValue);
#endif
#endif
			break;

		default:
			BF_Debug_Print_Error(("DEBUG: SetParam Error.PRMID=%x\n", wParamID ));
			return FJ_ERR_NG;
	}
	return ret;
}

/**
 * @brief		Get Parameter (long type)
 * @param		I : INT32 wParamID
 *				O : LONG* pwValue
 * @return		FJ_ERR_OK : Success
 *				FJ_ERR_NG : Fail (Not Supprt)
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE	fj_getparamlong(USHORT wParamID, LONG* pwValue)
{
	FJ_ERR_CODE	ret = FJ_ERR_NG;

// --- REMOVE_DCF_EXIF BEGIN ---
	switch( wParamID ){
		case FJ_PARA_DCF_LAST_KEY:
		case FJ_PARA_DCF_DUPLICATE:
		case FJ_PARA_DCF_HIDDEN:
		case FJ_PARA_DCF_ASSIGN_LAST_KEY:
		case FJ_PARA_DCF_ASSIGN_DUPLICATE:
		case FJ_PARA_DCF_ASSIGN_HIDDEN:
		case FJ_PARA_CHIPTEST_ADDR:
			ret = BF_Dcf_Get_Param_Long( wParamID, pwValue );
			break;
		default:
			ret = FJ_ERR_NG;
			break;
	}
// --- REMOVE_DCF_EXIF END ---
	return ret;
}

// --- REMOVE_CAMERAIF BEGIN ---
#if 1	// set Date&Time from Linux
/**
 * @brief		Set Base DateTime
 * @param		I : T_FJ_COMMON_DATE_TIME* date_time
 * @return		None
 * @note		None
 * @attention	None
 */
VOID fj_setbasedatetime( T_FJ_COMMON_DATE_TIME* date_time )
{
	ULONG sr;
	if (date_time) {
		sr = Dd_ARM_DI();
		OS_User_Get_Tim(&g_system_atrr_base_date_time_mng.system_time);
		g_system_atrr_base_date_time_mng.date_time = *date_time;
		Dd_ARM_EI(sr);
	}
}

/**
 * @brief		Get Base DateTime
 * @param		O : T_FJ_COMMON_DATE_TIME* date_time
 * @return		None
 * @note		None
 * @attention	None
 */
VOID fj_getbasedatetime( T_FJ_COMMON_DATE_TIME* date_time )
{
	ULONG sr;
	if (date_time) {
		sr = Dd_ARM_DI();
		*date_time = g_system_atrr_base_date_time_mng.date_time;
		Dd_ARM_EI(sr);
	}
}
#endif
// --- REMOVE_CAMERAIF END ---
