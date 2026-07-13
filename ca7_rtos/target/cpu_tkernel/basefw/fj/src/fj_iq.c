/**
 * @file		fj_iq.c
 * @brief		Algorithm for Image Quality API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "os_user_custom.h"
// delete_basefw\fj\iq
//#include "fj_iq.h"
//#include "iq_tbl.h"
//#include "iq_r2y.h"
//#include "iq_ela.h"
//#include "iq_cnr.h"
//#include "iq_cc0.h"
//#include "iq_mode.h"
//#include "iq_dpc.h"
//#include "iq_dpc_detect_sw_if.h"
//#include "iq_dpc_file.h"
//#include "iq_dpc_tbl_sel.h"
//#include "iq_geom_dist.h"
//#include "iq_fshd.h"
//#include "iq_fshdl0.h"
//#include "iq_pro.h"
//#include "iq_cag.h"

#include "mode.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define MODE_CHECK_ALL		(D_BF_MODE_VIEW_CODE|D_BF_MODE_OTHER_CODE|D_BF_MODE_CATEGORY_ALL)
#define MODE_CHECK_CALIB	(D_BF_MODE_VIEW_CODE|D_BF_MODE_OTHER_CODE|D_BF_MODE_CATEGORY_CALIBRATION)


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/


// delete_basefw\fj\iq
//FJ_ERR_CODE	FJ_IQ_Set_Next_Still_Mode_Sen( UCHAR sen_id, E_IQ_STILL_MODE mode )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	 delete_basefw\fj\iq
//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_set_next_still_mode_sen( sen_id, mode );
//	}

//	return ret;
//}

// delete_basefw\fj\iq
//FJ_ERR_CODE	FJ_IQ_Get_Still_Mode_Sen( UCHAR sen_id, E_IQ_STILL_MODE* curr_mode, E_IQ_STILL_MODE* next_mode )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_get_still_mode_sen( sen_id, curr_mode, next_mode );
//	}

//	return ret;
//}

FJ_ERR_CODE	FJ_IQ_Set_Next_IsoSpeed_Idx_Sen( UCHAR sen_id, FLOAT iso_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_set_next_isospeed_idx_sen( sen_id, iso_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Get_IsoSpeed_Idx_Sen( UCHAR sen_id, FLOAT* curr_iso_idx, FLOAT* next_iso_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_get_isospeed_idx_sen( sen_id, curr_iso_idx, next_iso_idx );
	//}

	return ret;
}

// delete_basefw\fj\iq
//FJ_ERR_CODE	FJ_IQ_Set_Next_Scene_No_Sen( UCHAR sen_id, E_IQ_SCENE scene )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	 delete_basefw\fj\iq
//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_set_next_scene_no_sen( sen_id, scene );
//	}

//	return ret;
//}

// delete_basefw\fj\iq
//FJ_ERR_CODE	FJ_IQ_Set_Next_Scene_No_For_Sharpness_Sen( UCHAR sen_id, E_IQ_SCENE scene )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_set_next_scene_no_for_sharpness_sen( sen_id, scene );
//	}

//	return ret;
//}

// delete_basefw\fj\iq
//FJ_ERR_CODE	FJ_IQ_Set_Next_Scene_No_For_Edge_Sen( UCHAR sen_id, E_IQ_SCENE scene )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_set_next_scene_no_for_edge_sen( sen_id, scene );
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE	FJ_IQ_Get_Scene_No_Sen( UCHAR sen_id, E_IQ_SCENE* curr_scene, E_IQ_SCENE* next_scene )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_get_scene_no_sen( sen_id, curr_scene, next_scene );
//	}

//	return ret;
//}

#if defined(CO_IQBIN_GENERAL)	// --- REMOVE_IQBIN_GENERAL BEGIN ---
FJ_ERR_CODE	FJ_IQ_Set_Next_ColorTemp_Idx_Sen( UCHAR sen_id, FLOAT color_temp_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_set_next_colortemp_idx_sen( sen_id, color_temp_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Get_ColorTemp_Idx_Sen( UCHAR sen_id, FLOAT* curr_color_temp_idx, FLOAT* next_color_temp_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_get_colortemp_idx_sen( sen_id, curr_color_temp_idx, next_color_temp_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Set_Next_ExposureValue_Idx_Sen( UCHAR sen_id, FLOAT exp_val_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_set_next_exposurevalue_idx_sen( sen_id, exp_val_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Get_ExposureValue_Idx_Sen( UCHAR sen_id, FLOAT* curr_exp_val_idx, FLOAT* next_exp_val_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_get_exposurevalue_idx_sen( sen_id, curr_exp_val_idx, next_exp_val_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Set_Next_TimeValue_Idx_Sen( UCHAR sen_id, FLOAT time_val_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_set_next_timevalue_idx_sen( sen_id, time_val_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Get_TimeValue_Idx_Sen( UCHAR sen_id, FLOAT* curr_time_val_idx, FLOAT* next_time_val_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_get_timevalue_idx_sen( sen_id, curr_time_val_idx, next_time_val_idx );
	//}

	return ret;
}

#endif // CO_IQBIN_GENERAL  --- REMOVE_IQBIN_GENERAL END ---

// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Get_SelectInfo_Sen( UCHAR sen_id, T_FJ_IQ_SEL_INFO* curr_sel_info, T_FJ_IQ_SEL_INFO* next_sel_info )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_get_selectinfo_sen( sen_id, curr_sel_info, next_sel_info );
//	}

//	return ret;
//}

FJ_ERR_CODE	FJ_IQ_Update_Current_Sen( UCHAR sen_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_update_current_sen( sen_id );
	//}

	return ret;
}


// delete_basefw\fj\iq
//FJ_ERR_CODE	FJ_IQ_Set_Next_Still_Mode( UCHAR demosaic_id, E_IQ_STILL_MODE mode )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_set_next_still_mode( demosaic_id, mode );
//	}

//	return ret;
//}

// delete_basefw\fj\iq
//FJ_ERR_CODE	FJ_IQ_Get_Still_Mode( UCHAR demosaic_id, E_IQ_STILL_MODE* curr_mode, E_IQ_STILL_MODE* next_mode )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_get_still_mode( demosaic_id, curr_mode, next_mode );
//	}

//	return ret;
//}

FJ_ERR_CODE	FJ_IQ_Set_Next_IsoSpeed_Idx( UCHAR demosaic_id, FLOAT iso_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_set_next_isospeed_idx( demosaic_id, iso_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Get_IsoSpeed_Idx( UCHAR demosaic_id, FLOAT* curr_iso_idx, FLOAT* next_iso_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_get_isospeed_idx( demosaic_id, curr_iso_idx, next_iso_idx );
	//}

	return ret;
}

// delete_basefw\fj\iq
//FJ_ERR_CODE	FJ_IQ_Set_Next_Scene_No( UCHAR demosaic_id, E_IQ_SCENE scene )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_set_next_scene_no( demosaic_id, scene );
//	}

//	return ret;
//}

// delete_basefw\fj\iq
//FJ_ERR_CODE	FJ_IQ_Set_Next_Scene_No_For_Sharpness( UCHAR demosaic_id, E_IQ_SCENE scene )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_set_next_scene_no_for_sharpness( demosaic_id, scene );
//	}

//	return ret;
//}

// delete_basefw\fj\iq
//FJ_ERR_CODE	FJ_IQ_Set_Next_Scene_No_For_Edge( UCHAR demosaic_id, E_IQ_SCENE scene )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_set_next_scene_no_for_edge( demosaic_id, scene );
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE	FJ_IQ_Get_Scene_No( UCHAR demosaic_id, E_IQ_SCENE* curr_scene, E_IQ_SCENE* next_scene )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_get_scene_no( demosaic_id, curr_scene, next_scene );
//	}

//	return ret;
//}

#if defined(CO_IQBIN_GENERAL)	// --- REMOVE_IQBIN_GENERAL BEGIN ---
FJ_ERR_CODE	FJ_IQ_Set_Next_OptZoom_Idx( UCHAR demosaic_id, UINT32 opt_zoom_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_set_next_optzoom_idx( demosaic_id, opt_zoom_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Get_OptZoom_Idx( UCHAR demosaic_id, UINT32* curr_opt_zoom_idx, UINT32* next_opt_zoom_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_get_optzoom_idx( demosaic_id, curr_opt_zoom_idx, next_opt_zoom_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Set_Next_LensType_Idx( UCHAR demosaic_id, UINT32 lens_type_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_set_next_lenstype_idx( demosaic_id, lens_type_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Get_LensType_Idx( UCHAR demosaic_id, UINT32* curr_lens_type_idx, UINT32* next_lens_type_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_get_lenstype_idx( demosaic_id, curr_lens_type_idx, next_lens_type_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Set_Next_ColorTemp_Idx( UCHAR demosaic_id, FLOAT color_temp_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_set_next_colortemp_idx( demosaic_id, color_temp_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Get_ColorTemp_Idx( UCHAR demosaic_id, FLOAT* curr_color_temp_idx, FLOAT* next_color_temp_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_get_colortemp_idx( demosaic_id, curr_color_temp_idx, next_color_temp_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Set_Next_ExposureValue_Idx( UCHAR demosaic_id, FLOAT exp_val_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_set_next_exposurevalue_idx( demosaic_id, exp_val_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Get_ExposureValue_Idx( UCHAR demosaic_id, FLOAT* curr_exp_val_idx, FLOAT* next_exp_val_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_get_exposurevalue_idx( demosaic_id, curr_exp_val_idx, next_exp_val_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Set_Next_TimeValue_Idx( UCHAR demosaic_id, FLOAT time_val_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_set_next_timevalue_idx( demosaic_id, time_val_idx );
	//}

	return ret;
}

FJ_ERR_CODE	FJ_IQ_Get_TimeValue_Idx( UCHAR demosaic_id, FLOAT* curr_time_val_idx, FLOAT* next_time_val_idx )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_get_timevalue_idx( demosaic_id, curr_time_val_idx, next_time_val_idx );
	//}

	return ret;
}

#endif // CO_IQBIN_xxxx  --- REMOVE_IQBIN_GENERAL END ---

// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Get_SelectInfo( UCHAR demosaic_id, T_FJ_IQ_SEL_INFO* curr_sel_info, T_FJ_IQ_SEL_INFO* next_sel_info )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_get_selectinfo( demosaic_id, curr_sel_info, next_sel_info );
//	}

//	return ret;
//}

FJ_ERR_CODE	FJ_IQ_Update_Current( UCHAR demosaic_id )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_update_current( demosaic_id );
	//}

	return ret;
}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Update_PreCnrCC0( UCHAR demosaic_id, T_IQ_PRE_CNR_CC0_CTRL* pre_cnr_cc_ctrl )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_update_precnrcc( demosaic_id, pre_cnr_cc_ctrl );
//	}

//	return ret;
//}

// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Get_PreCnrCC0( UCHAR demosaic_id, T_IQ_PRE_CNR_CC0_CTRL* pre_cnr_cc_ctrl )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_get_precnrcc( demosaic_id, pre_cnr_cc_ctrl );
//	}

//	return ret;
//}

// delete_basefw\fj\iq
//FJ_ERR_CODE	FJ_IQ_Get_GammaTbl( T_FJ_IQ_GAMMA* gamma_info, T_FJ_IQ_SEL_INFO* sel_info )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_get_gammatbl( gamma_info, sel_info );
//	}

//	return ret;
//}

// delete_basefw\fj\iq
//FJ_ERR_CODE	FJ_IQ_Get_YccTbl( T_FJ_IQ_YCC* ycc_info, T_FJ_IQ_SEL_INFO* sel_info )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_get_ycctbl( ycc_info, sel_info );
//	}

//	return ret;
//}

// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Get_Ela_OnOff( BOOL* on_off, T_FJ_IQ_SEL_INFO* sel_info )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_get_ela_onoff( on_off, sel_info );
//	}

//	return ret;
//}

// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Get_Cnr_OnOff( BOOL* on_off, T_FJ_IQ_SEL_INFO* sel_info )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_get_cnr_onoff( on_off, sel_info );
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Notify_Loading_Complete( E_FJ_IQ_BIN_TYPE type, BYTE* addr, ULONG bytes )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_notify_loading_complete( type, addr, bytes );
//	}

//	return ret;
//}

// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Set_IQ_FilePath( E_IQ_STILL_MODE mode, CHAR* param_file_path )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_set_iq_filepath( mode, param_file_path );
//	}

//	return ret;
//}

FJ_ERR_CODE FJ_IQ_Set_IQ_FilePath_common( CHAR* param_file_path )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	return fj_iq_set_iq_filepath_common( param_file_path );
	//}

	return ret;
}



/* Function declaration(Dead Pixel Compensation)								*/

// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_CalibrationCreateBadpixTable( UCHAR sen_id, 
//											E_IQ_CALIB_TGT target,
//											UINT32 tbl_idx,
//											E_IQ_DETECT_ALGORITHM e_detect_mode,
//											USHORT us_low_thresh,
//											USHORT us_high_thresh,
//											UINT32 ui_max_badpix_thresh,
//											FJ_IQ_CORNER_THRESHOLD* pt_corner_thresh,
//											UINT32* ui_over4succession_count )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_CALIB );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_calibrationcreatebadpixtable( sen_id,
//												  target,
//												  tbl_idx,
//												  e_detect_mode,
//												  us_low_thresh,
//												  us_high_thresh,
//												  ui_max_badpix_thresh,
//												  pt_corner_thresh,
//												  ui_over4succession_count );
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//UINT32 FJ_IQ_CalibrationGetNumOfBadpix( E_IQ_CALIB_TGT target, UINT32 tbl_idx )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		return fj_iq_calibrationgetnumofbadpix( target, tbl_idx );
//	} else {
//		return 0;
//	}
//}


// delete_basefw\fj\iq
//BOOL FJ_IQ_CalibrationClearBadpixTable( E_IQ_CALIB_TGT target, UINT32 tbl_idx )
//{
//	BOOL ret = FALSE;

//	if( BF_Mode_Check( MODE_CHECK_ALL ) ==  FJ_ERR_OK ) {
//		ret = fj_iq_calibrationclearbadpixtable( target, tbl_idx );
//	} else {
//		ret = FALSE;
//	}

//	return ret;
//}


VOID FJ_IQ_CalibrationOnOffBadpixCompensation( UCHAR demosaic_id, BOOL b_onoff )
{
	// delete_basefw\fj\iq
	//FJ_ERR_CODE ret;

	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	fj_iq_calibrationonoffbadpixcompensation( demosaic_id, b_onoff );
	//}
}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_CalibrationConvertBadpixTable( E_IQ_CALIB_TGT dst_target, E_IQ_CALIB_TGT src_target, FJ_IQ_BADPIX_CONV_INFO* conv_info )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret != FJ_ERR_OK ) {
//		return ret;
//	}

//	ret = fj_iq_calibrationconvertbadpixtable( dst_target, src_target, conv_info );

//	return ret;
//}


// delete_basefw\fj\iq
//BOOL FJ_IQ_CalibrationSaveBadpixTable( E_IQ_CALIB_TGT target, UINT32 tbl_idx, CHAR* to_file_path )
//{
//	BOOL ret = FALSE;

//	if( BF_Mode_Check( MODE_CHECK_ALL ) ==  FJ_ERR_OK ) {
//		ret = fj_iq_calibrationsavebadpixtable( target, tbl_idx, to_file_path );
//	} else {
//		ret = FALSE;
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//BOOL FJ_IQ_CalibrationLoadBadpixTable( E_IQ_CALIB_TGT target, UINT32 tbl_idx, CHAR* from_file_path )
//{
//	BOOL ret = FALSE;

//	if( BF_Mode_Check( MODE_CHECK_ALL ) ==  FJ_ERR_OK ) {
//		ret = fj_iq_calibrationloadbadpixtable( target, tbl_idx, from_file_path );
//	} else {
//		ret = FALSE;
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//BOOL FJ_IQ_CalibrationDeleteBadpixTable( E_IQ_CALIB_TGT target, UINT32 tbl_idx, CHAR* from_file_path )
//{
//	BOOL ret = FALSE;

//	if( BF_Mode_Check( MODE_CHECK_ALL ) ==  FJ_ERR_OK ) {
//		ret = fj_iq_calibrationdeletebadpixtable( target, tbl_idx, from_file_path );
//	} else {
//		ret = FALSE;
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//BOOL FJ_IQ_Notify_Loading_BadpixTable_Complete( E_IQ_CALIB_TGT target, UINT32 tbl_idx, BYTE* addr, ULONG bytes )
//{
//	BOOL ret = FALSE;

//	if( BF_Mode_Check( MODE_CHECK_ALL ) ==  FJ_ERR_OK ) {
//		ret = fj_iq_notify_loading_badpixtable_complete( target, tbl_idx, addr, bytes );
//	} else {
//		ret = FALSE;
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Get_BadpixTableAddr( E_IQ_CALIB_TGT target, UINT32 tbl_idx, BYTE** addr, UINT32* bytes )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret != FJ_ERR_OK ) {
//		return ret;
//	}

//	ret = fj_iq_get_badpixtableaddr( target, tbl_idx, addr, bytes );

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Select_BadpixTable( UCHAR demosaic_id, E_IQ_CALIB_TGT target, UINT32 tbl_idx )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret != FJ_ERR_OK ) {
//		return ret;
//	}

//	//ret = fj_iq_select_badpixtable( demosaic_id, target, tbl_idx );

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Flip_BadpixTable( E_IQ_CALIB_TGT target, FJ_IQ_BADPIX_FLIP_INFO* flip_info )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret != FJ_ERR_OK ) {
//		return ret;
//	}

//	//ret = fj_iq_flip_badpixtable( target, flip_info );

//	return ret;
//}




/* Function declaration(Frame shading Compensation)					*/
FJ_ERR_CODE FJ_IQ_OnOff_FrameShading( UCHAR demosaic_id, BOOL onoff )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_onoff_frameshading( demosaic_id, onoff );
	//}

	return ret;
}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Select_FrameShadingTable( UCHAR demosaic_id, T_FJ_IQ_FSHD_SEL_INFO* tbl_sel_info )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret != FJ_ERR_OK ) {
//		return ret;
//	}

//	ret = fj_iq_select_frameshadingtable( demosaic_id, tbl_sel_info );

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Calibration_FrameShading( UCHAR sen_id, T_FJ_IQ_FSHD_SEL_INFO* tbl_sel_info, DOUBLE corner_ratio )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_calibration_frameshading( sen_id, tbl_sel_info, corner_ratio );
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Link_FrameShadingTable( T_FJ_IQ_FSHD_SEL_INFO* to_tbl_sel_info, T_FJ_IQ_FSHD_SEL_INFO* from_tbl_sel_info )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_link_frameshadingtable( to_tbl_sel_info, from_tbl_sel_info );
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Conv_FrameShadingTable( E_IQ_CALIB_TGT calib_tgt_dst, E_IQ_CALIB_TGT calib_tgt_src, T_FJ_IQ_FSHD_CONVINFO* conv_info )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_conv_frameshadingtable( calib_tgt_dst, calib_tgt_src, conv_info );
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Notify_Loading_FrameShadingTable_Complete( E_IQ_CALIB_TGT calib_tgt, BYTE* addr, ULONG bytes )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_notify_loading_frameshadingtable_complete( calib_tgt, addr, bytes );
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Get_FrameShadingTableAddr( T_FJ_IQ_FSHD_SEL_INFO* tbl_sel_info, BYTE** addr, UINT32* bytes )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_get_frameshadingtableaddr( tbl_sel_info, addr, bytes );
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_LoadFile_FrameShadingTable( E_IQ_CALIB_TGT calib_tgt, CHAR* filepath )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_loadfile_frameshadingtable( calib_tgt, filepath );
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_SaveFile_FrameShadingTable( T_FJ_IQ_FSHD_SEL_INFO* tbl_sel_info, CHAR* filepath )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_savefile_frameshadingtable( tbl_sel_info, filepath );
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_LoadFile_FrameShadingTable_Sdram( E_IQ_CALIB_TGT calib_tgt, CHAR* tbl_addr )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_loadfile_frameshadingtable_sdram( calib_tgt, tbl_addr );
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_SaveFile_FrameShadingTable_Sdram(T_FJ_IQ_FSHD_SEL_INFO* tbl_sel_info, CHAR* tbl_addr, ULONG* tbl_size )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_savefile_frameshadingtable_sdram( tbl_sel_info, tbl_addr, tbl_size );
//	}

//	return ret;
//}


FJ_ERR_CODE FJ_IQ_Flip_FrameShadingTable( BOOL vflip_on_off )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_flip_frameshadingtable( vflip_on_off );
	//}

	return ret;
}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Set_FrameShadingSlopeGain( UCHAR demosaic_id, const T_FJ_IQ_FSHD_SLP_GAIN* slp_gain )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_set_frameshadingslopegain( demosaic_id, slp_gain );
//	}

//	return ret;
//}


/* Function declaration(Frame shading L0 Compensation)					*/

// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Conv_FSHDL0_Table( T_FJ_IQ_FSHDL_TBL* dst, const T_FJ_IQ_FSHDL_TBL* src )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_conv_fshdl0_table( dst, src );
//	}

//	return ret;
//}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Set_FSHDL0_Table( UCHAR demosaic_id, const T_FJ_IQ_FSHDL_TBL* tbl, const T_FJ_IQ_FSHDL_PARAM* param )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_set_fshdl0_table( demosaic_id, tbl, param );
//	}

//	return ret;
//}


FJ_ERR_CODE FJ_IQ_OnOff_FSHDL0( UCHAR demosaic_id, BOOL onoff )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_onoff_fshdl0( demosaic_id, onoff );
	//}

	return ret;
}


// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Set_FSHDL0_SlopeGain( UCHAR demosaic_id, const T_FJ_IQ_FSHDL_SLP_GAIN* slp_gain )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_set_fshdl0_slopegain( demosaic_id, slp_gain );
//	}

//	return ret;
//}


/* Function declaration(CAG-FFC Compensation)					*/

// delete_basefw\fj\iq
//FJ_ERR_CODE FJ_IQ_Conv_CagTable( E_IQ_STILL_MODE dst_mode, E_IQ_STILL_MODE src_mode, const T_FJ_IQ_CAG_CONVINFO* conv_info )
//{
//	FJ_ERR_CODE ret = FJ_ERR_NG;

//	ret = BF_Mode_Check( MODE_CHECK_ALL );
//	if( ret == FJ_ERR_OK ) {
//		ret = fj_iq_conv_cagtable( dst_mode, src_mode, conv_info );
//	}

//	return ret;
//}


FJ_ERR_CODE FJ_IQ_Flip_CagTable( BOOL vflip_on_off )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_flip_cagtable( vflip_on_off );
	//}

	return ret;
}


FJ_ERR_CODE FJ_IQ_Tbl_Set_Force_Reload( VOID )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	// delete_basefw\fj\iq
	//ret = BF_Mode_Check( MODE_CHECK_ALL );
	//if( ret == FJ_ERR_OK ) {
	//	ret = fj_iq_tbl_set_force_reload();
	//}

	return ret;
}
