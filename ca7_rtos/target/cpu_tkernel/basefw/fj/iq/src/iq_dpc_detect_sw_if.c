/**
 * @file		iq_dpc_detect_sw_if.c
 * @brief		Badpix software detection API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_iq.h"
#include "fj_std.h"

#include "iq_dpc_detect_sw_if.h"

#include "iq_dpc_custom.h"
#include "iq_dpc_detect_sw.h"
#include "iq_dpc_detect_info.h"
#include "iq_dpc_tbl.h"

#include "iq_common.h"
#include "iq_dcache.h"

#include "debug.h"

#include "ddim_typedef.h"

#include <string.h>


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
//#define CO_BF_IQ_DPC_DETECT_DBG	for BaseFW internal used

#define D_BF_IQ_DPC_TBL_BYTE_TMP_DETECT	(M_IQ_ROUNDUP_8( M_BF_IQ_DPC_CALC_TBL_BYTE(D_BF_IQ_DPC_MAX_PIXELS) ))
#define D_BF_IQ_DPC_TBL_BYTE_TMP_MERGE	(M_IQ_ROUNDUP_8( M_BF_IQ_DPC_CALC_TBL_BYTE(D_BF_IQ_DPC_MAX_PIXELS) ))



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
/** Dead Pixel Compensation table - body(temporary detection) */
static volatile UINT32	gbf_iq_dpc_defect_info_tbl_tmp_detect[(D_BF_IQ_DPC_TBL_BYTE_TMP_DETECT + 3) / sizeof(UINT32)];
/** Dead Pixel Compensation table - body(detect data merge) */
static volatile UINT32	gbf_iq_dpc_defect_info_tbl_tmp_merge[(D_BF_IQ_DPC_TBL_BYTE_TMP_MERGE + 3) / sizeof(UINT32)];


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
static BOOL bf_iq_dpc_detect_sw_if_check_corner_thresh_param( const T_BF_IQ_DPC_DETECT_INFO* const detect_info, const FJ_IQ_CORNER_THRESHOLD* const pt_thresh_info )
{
	if( pt_thresh_info == NULL ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "iqsub_corner_thresh_param_check(): pt_threth_info is NULL\n" ));
#endif
		return FALSE;
	}

	if( (pt_thresh_info->t_left_top.us_h > detect_info->us_w_pixels) || 
		(pt_thresh_info->t_left_top.us_v > detect_info->us_h_pixels) ){
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "iqsub_corner_thresh_param_check(): corner threshold parameter error. left_top[%d:%d]\n",
					pt_thresh_info->t_left_top.us_h, pt_thresh_info->t_left_top.us_v ));
#endif
		return FALSE;
	}

	if( (pt_thresh_info->t_right_top.us_h > detect_info->us_w_pixels) || 
		(pt_thresh_info->t_right_top.us_v > detect_info->us_h_pixels) ){
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "iqsub_corner_thresh_param_check(): corner threshold parameter error. right_top[%d:%d]\n",
					pt_thresh_info->t_right_top.us_h, pt_thresh_info->t_right_top.us_v ));
#endif
		return FALSE;
	}

	if( (pt_thresh_info->t_left_bottom.us_h > detect_info->us_w_pixels) || 
		(pt_thresh_info->t_left_bottom.us_v > detect_info->us_h_pixels) ){
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "iqsub_corner_thresh_param_check(): corner threshold parameter error. left_bottom[%d:%d]\n",
					pt_thresh_info->t_left_bottom.us_h, pt_thresh_info->t_left_bottom.us_v ));
#endif
		return FALSE;
	}

	if( (pt_thresh_info->t_right_bottom.us_h > detect_info->us_w_pixels) || 
		(pt_thresh_info->t_right_bottom.us_v > detect_info->us_h_pixels) ){
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "iqsub_corner_thresh_param_check(): corner threshold parameter error. right_bottom[%d:%d]\n",
					pt_thresh_info->t_right_bottom.us_h, pt_thresh_info->t_right_bottom.us_v ));
#endif
		return FALSE;
	}

	return TRUE;
}


/** @brief			bf_iq_dpc_detected_sw_if_create_table<br>
 *	                main code of FJ_IQ_CalibrationCreateBadpixTable() function.
 *	@param[inout]	pt_detect_info			detect parameters structure
 *	@retval			FJ_ERR_OK					success.
 *	@retval			FJ_ERR_IQ_BP_TOO_MUCH		errror. (see also FJ_IQ_CalibrationCreateBadpixTable())
 *	@retval			FJ_ERR_IQ_BP_TOO_OVERFLOW	errror. (see also FJ_IQ_CalibrationCreateBadpixTable())
 *	@note			None
 * @attention	None
 */
static FJ_ERR_CODE bf_iq_dpc_detected_sw_if_create_table( T_BF_IQ_DPC_DETECT_INFO* const pt_detect_info, const E_IQ_CALIB_TGT e_calib_tgt, const UINT32 tbl_idx )
{
	FJ_ERR_CODE iq_ercd;
	UINT32 ui_field_idx;
	UINT32 ui_tbl_offset_byte;
	UINT32 ui_tbl2_offset_byte;
	UINT32 ui_merge_pixel_num = 0;
	UINT32 ui_merge_over4_count = 0;
	T_BF_IQ_DPC_TBL_MERGE t_table_merge_info = { 0 };
	BOOL b_before_tbl_valid;
	T_BF_IQ_DPC_TBL_INFO* tbl_info;
	U_BF_IQ_DPC_TBL_DATA* merged_tbl_addr_by_fields[D_BF_IQ_DPC_MAX_FIELDS];
	UINT32 merged_tbl_bytelen_by_fields[D_BF_IQ_DPC_MAX_FIELDS];


	tbl_info = BF_IQ_Dpc_Tbl_Get_Info( e_calib_tgt, tbl_idx );
	if( tbl_info == NULL ) {
		return FJ_ERR_IQ_TBL_INVALID;
	}

	b_before_tbl_valid = tbl_info->valid;

	tbl_info->valid = FALSE;

	iq_ercd = BF_IQ_Dpc_Detect_Sw_Run( pt_detect_info );
	if( iq_ercd != FJ_ERR_OK ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "FJ_IQ_CalibrationCreateBadpixTable(): error. ercd=%d.\n", iq_ercd ));
#endif
		return iq_ercd;
	}

	if( pt_detect_info->ui_detect_count > pt_detect_info->ui_max_badpix_thresh ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "FJ_IQ_CalibrationCreateBadpixTable(): too meny bad pixels. (%u)\n", pt_detect_info->ui_detect_count ));
#endif
		tbl_info->valid = b_before_tbl_valid;
		return FJ_ERR_IQ_BP_TOO_MUCH;
	}

	if( pt_detect_info->ui_detect_info_tbl_overflow ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "FJ_IQ_CalibrationCreateBadpixTable(): table overflow. (%u)\n", pt_detect_info->ui_detect_count ));
#endif
		return FJ_ERR_IQ_BP_TOO_OVERFLOW;
	}


	if( b_before_tbl_valid != FALSE ){
		for( ui_field_idx = 0, ui_tbl_offset_byte = 0, ui_tbl2_offset_byte = 0; 
			 ui_field_idx < pt_detect_info->ui_field_num; 
			 ui_field_idx++ ) {

			/* data setting for merge table information */
			t_table_merge_info.pv_defect_info_tbl1_addr = tbl_info->addr_by_fields[ui_field_idx];
			t_table_merge_info.ui_defect_info_tbl1_byte = tbl_info->bytelen_by_fields[ui_field_idx];
			t_table_merge_info.pv_defect_info_tbl2_addr = (U_BF_IQ_DPC_TBL_DATA*)(((UCHAR*)pt_detect_info->pt_defect_info_tbl_addr)
														  + ui_tbl2_offset_byte);
			t_table_merge_info.ui_defect_info_tbl2_byte = pt_detect_info->ull_defect_info_tbl_bytelen[ui_field_idx];
			t_table_merge_info.pv_defect_info_tbl_out_addr = (U_BF_IQ_DPC_TBL_DATA*)(((UCHAR*)M_IQ_ROUNDUP_8((UINT32)gbf_iq_dpc_defect_info_tbl_tmp_merge))
															 + ui_tbl_offset_byte);
			t_table_merge_info.ui_defect_info_tbl_out_byte = D_BF_IQ_DPC_TBL_BYTE_TMP_MERGE - ui_tbl_offset_byte;
			t_table_merge_info.ui_h_pixels = pt_detect_info->us_upper_trim
											 + (pt_detect_info->us_h_pixels / pt_detect_info->ui_field_num)
											 + pt_detect_info->us_vd_ofs;
			t_table_merge_info.ui_w_pixels = pt_detect_info->us_left_trim + pt_detect_info->us_w_pixels + pt_detect_info->us_hd_ofs;

			/* detect table merge */
			iq_ercd = BF_IQ_Dpc_Detect_Sw_Merge_Tbl( &t_table_merge_info );
			if( iq_ercd != FJ_ERR_OK ) {
#ifdef FJ_DEBUG_PRINT_ON
				BF_Debug_Print_Error(( "FJ_IQ_CalibrationCreateBadpixTable(): table merge error.\n" ));
#endif
				return iq_ercd;
			}

			merged_tbl_addr_by_fields[ui_field_idx]
				= (U_BF_IQ_DPC_TBL_DATA*)(((UCHAR*)tbl_info->top_addr) + ui_tbl_offset_byte);
			merged_tbl_bytelen_by_fields[ui_field_idx]
				= t_table_merge_info.ui_defect_info_tbl_out_byte;
			ui_tbl_offset_byte += t_table_merge_info.ui_defect_info_tbl_out_byte;

			ui_tbl2_offset_byte += pt_detect_info->ull_defect_info_tbl_bytelen[ui_field_idx];
			ui_merge_pixel_num += t_table_merge_info.ui_detect_count;
			ui_merge_over4_count += t_table_merge_info.ui_over4succession_count;

			if( ui_tbl_offset_byte > tbl_info->maxbytelen ){
#ifdef FJ_DEBUG_PRINT_ON
				BF_Debug_Print_Error(( "FJ_IQ_CalibrationCreateBadpixTable(): table merge size over error.\n" ));
#endif
				return FJ_ERR_IQ_BP_TOO_OVERFLOW;
			}

			if( ui_merge_pixel_num > pt_detect_info->ui_max_badpix_thresh ){
#ifdef FJ_DEBUG_PRINT_ON
				BF_Debug_Print_Error(( "FJ_IQ_CalibrationCreateBadpixTable(): table merge detect count over error.\n" ));
#endif
				tbl_info->valid = b_before_tbl_valid;
				return FJ_ERR_IQ_BP_TOO_MUCH;
			}

			if( t_table_merge_info.ui_detect_info_tbl_out_overflow ){
#ifdef FJ_DEBUG_PRINT_ON
				BF_Debug_Print_Error(( "FJ_IQ_CalibrationCreateBadpixTable():table merge overflow.\n" ));
#endif
				return FJ_ERR_IQ_BP_TOO_OVERFLOW;
			}

			pt_detect_info->ui_detect_count = ui_merge_pixel_num;
			pt_detect_info->ui_over4succession_count = ui_merge_over4_count;

		}

		// Merge succes. Store addr_by_fields and bytelen_by_fields
		for( ui_field_idx = 0; ui_field_idx < pt_detect_info->ui_field_num; ui_field_idx++ ) {
			tbl_info->addr_by_fields[ui_field_idx]
				= merged_tbl_addr_by_fields[ui_field_idx];	/* pgr0000 */
			tbl_info->bytelen_by_fields[ui_field_idx]
				= merged_tbl_bytelen_by_fields[ui_field_idx];	/* pgr0000 */
		}
	} else {
		/* calc DPC table addr. and bytelen by fields */
		for( ui_field_idx = 0, ui_tbl_offset_byte = 0; ui_field_idx < pt_detect_info->ui_field_num; ui_field_idx++ ) {
			tbl_info->addr_by_fields[ui_field_idx]
				= (U_BF_IQ_DPC_TBL_DATA*)(((UCHAR*)tbl_info->top_addr) + ui_tbl_offset_byte);

			tbl_info->bytelen_by_fields[ui_field_idx]
				= pt_detect_info->ull_defect_info_tbl_bytelen[ui_field_idx];
			ui_tbl_offset_byte += pt_detect_info->ull_defect_info_tbl_bytelen[ui_field_idx];
		}
	}

	for( ui_field_idx = pt_detect_info->ui_field_num; ui_field_idx < D_BF_IQ_DPC_MAX_FIELDS; ui_field_idx++ ) {
		tbl_info->addr_by_fields[ui_field_idx] = NULL;
		tbl_info->bytelen_by_fields[ui_field_idx] = 0;
	}



	//
	// Store detected (or merged) table to current table
	//
	/* store table bytelen */
	tbl_info->bytelen = ui_tbl_offset_byte;

	/* store detect count */
	tbl_info->pixel_num = pt_detect_info->ui_detect_count;

	tbl_info->sta_x = pt_detect_info->us_hd_ofs;
	tbl_info->sta_y = pt_detect_info->us_vd_ofs;
	tbl_info->width = pt_detect_info->us_global_w_pixels;
	tbl_info->lines = pt_detect_info->us_global_h_pixels;
	tbl_info->field_num = pt_detect_info->ui_field_num;

	tbl_info->valid = TRUE;

	/* data setting to original table */
	if( b_before_tbl_valid != FALSE ){
		memcpy( tbl_info->top_addr, 
				(VOID*)M_IQ_ROUNDUP_8((UINT32)gbf_iq_dpc_defect_info_tbl_tmp_merge), 
				ui_tbl_offset_byte );
	} else {
		memcpy( tbl_info->top_addr, 
				(VOID*)M_IQ_ROUNDUP_8((UINT32)gbf_iq_dpc_defect_info_tbl_tmp_detect),
				ui_tbl_offset_byte );
	}
	BF_IQ_Dcache_Clear( (ULONG)tbl_info->top_addr, ui_tbl_offset_byte );

	return FJ_ERR_OK;
}


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

FJ_ERR_CODE  BF_IQ_Dpc_Create_Bad_Pix_Table( const E_IQ_CALIB_TGT e_calib_tgt,
											const UINT32 tbl_idx, 
											const BOOL is_merge_last_table,
											T_BF_IQ_DPC_DETECT_INFO* const detect_info )
{
	FJ_ERR_CODE					iq_ercd;
	T_BF_IQ_DPC_TBL_INFO*		tbl_info;
	UINT32						i_field_idx;
	
	if( detect_info == NULL ){
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	if( !bf_iq_dpc_detect_sw_if_check_corner_thresh_param( detect_info, detect_info->pt_corner_thresh ) ) {
		return FJ_ERR_IQ_BP_THRESH;
	}
	
	if( detect_info->e_detect_mode >= E_IQ_DETECT_ALGO_MAX ) {
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	// clear last information
	if( !is_merge_last_table ){
		tbl_info = BF_IQ_Dpc_Tbl_Get_Info( e_calib_tgt, tbl_idx );
		
		if( tbl_info == NULL ){
			return FJ_ERR_IQ_TBL_INVALID;
		}
		
		tbl_info->valid = FALSE;	// if true, merge...
		tbl_info->top_addr->rel.ui_format_id = D_BF_IQ_DPC_TBL_FORMAT_OTHER_DATA;
		tbl_info->top_addr->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_END_OF_DATA;
		tbl_info->top_addr->rel.ui_addr = 0;
		
		for( i_field_idx = 0; i_field_idx < D_BF_IQ_DPC_MAX_FIELDS; i_field_idx++ ) {
			tbl_info->addr_by_fields[i_field_idx] = tbl_info->top_addr;
			tbl_info->bytelen_by_fields[i_field_idx] = 0;
		}
		tbl_info->bytelen = 0;
		tbl_info->pixel_num = 0;
		tbl_info->sta_x = 0;
		tbl_info->sta_y = 0;
		tbl_info->width = 0;
		tbl_info->lines = 0;
		tbl_info->field_num = 0;
	}
	
	// use temp buffer for output
	detect_info->pt_defect_info_tbl_addr = (U_BF_IQ_DPC_TBL_DATA*)(M_IQ_ROUNDUP_8((UINT32)gbf_iq_dpc_defect_info_tbl_tmp_detect));
	detect_info->ull_defect_info_tbl_byte = D_BF_IQ_DPC_TBL_BYTE_TMP_DETECT;

	iq_ercd = bf_iq_dpc_detected_sw_if_create_table( detect_info, e_calib_tgt, tbl_idx );

	if( iq_ercd != FJ_ERR_OK ) {
		return iq_ercd;
	}

	return FJ_ERR_OK;
}


/** @brief		FJ_IQ_CalibrationCreateBadpixTable<BR>
 * 				detecting Dead Pixel(DP) from bayer data  and create Dead Pixel Compensation(DPC) table.
 * 				if DPC table create successful, DPC table set to enabled.
 * 				before calling this API, setting calibrasion target and pixel format.
 * 				(using FJ_IQ_CalibrationSetTarget() and FJ_IQ_CalibrationSetFormatPix())
 * @param[in]	e_detect_mode			defect pixel detection algorithm
 * @param[in]	us_low_thresh			low threshold(if pixel value less then us_low_thresh, pixel was judged dead pixel)
 * @param[in]	us_high_thresh			high threshold(if pixel value greater then us_high_thresh, pixel was judged dead pixel)
 * @param[in]	ui_max_badpix_thresh	limit number of bad pixels. bad pixels are more then this value, FJ_ERR_IQ_BP_TOO_MUCH is returned.
 * @param[in]	pt_corner_thresh		threshold information on corner
 * @param[out]	ui_over4succession_count	number of impossible correct pixels(over 4 bad pixel continuously).
 * @retval		FJ_ERR_OK					table creating successful, and table set to valid.
 * @retval		FJ_ERR_IQ_BP_TOO_MUCH		table creating failure. number of detected pixel greater than ui_max_badpix_thresh.
 * @retval		FJ_ERR_IQ_BP_TOO_OVERFLOW	table creating failure. dead pixel compensation table is overflow.
 * @retval		FJ_ERR_IQ_BP_THRESH			table creating failure. pt_corner_thresh is invalid.
 * @retval		FJ_ERR_IQ_BP_FMT_PIX		table creating failure. pixel format is not equal E_IQ_CALIB_FMT_PIX_16NONPACK.
 * @retval		FJ_ERR_IQ_BP_CALIB_TGT		table creating failure. calibrasion target is not set.
 * @retval		FJ_ERR_IQ_BP_HW_DETECT_ON	table creating failure. don't disable Hardware detection.
 * @retval		FJ_ERR_NG_INPUT_PARAM		table creating failure. parameter error.
 * @note		only support pixel format is E_IQ_CALIB_FMT_PIX_16NONPACK
 * @attention	None
 */
FJ_ERR_CODE fj_iq_calibrationcreatebadpixtable(	const UCHAR sen_id,
											const E_IQ_CALIB_TGT e_calib_tgt,
											const UINT32 tbl_idx,
											const E_IQ_DETECT_ALGORITHM e_detect_mode,
											const USHORT us_low_thresh, 
											const USHORT us_high_thresh, 
											const UINT32 ui_max_badpix_thresh, 
											const FJ_IQ_CORNER_THRESHOLD* const pt_corner_thresh,
											UINT32* const ui_over4succession_count )
{
	FJ_ERR_CODE					iq_ercd;
	T_BF_IQ_DPC_DETECT_INFO*	detect_info;
#ifdef CO_BF_IQ_DPC_DETECT_DBG
	T_BF_IQ_DPC_TBL_INFO*		tbl_info;
#endif

	detect_info = BF_IQ_Dpc_Detect_Info_Get_Info( sen_id );

#if 0
	if( detect_info->b_hwDetectOnOff != FALSE ) {
		return FJ_ERR_IQ_BP_HW_DETECT_ON;
	}
#endif

	if( ui_over4succession_count == NULL ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "FJ_IQ_CalibrationCreateBadpixTable(): ui_over4succession_count is NULL.\n" ));
#endif
		return FJ_ERR_NG_INPUT_PARAM;
	}

	*ui_over4succession_count = 0;

	if( FJ_IQ_CalibrationGetFormatPix() != E_IQ_CALIB_FMT_PIX_16NONPACK ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "FJ_IQ_CalibrationCreateBadpixTable(): only E_IQ_CALIB_FMT_PIX_16NONPACK supported. (%u)\n", FJ_IQ_CalibrationGetFormatPix() ));
#endif
		return FJ_ERR_IQ_BP_FMT_PIX;
	}


	/* Corner Threshold parameter check */
	if( !bf_iq_dpc_detect_sw_if_check_corner_thresh_param( detect_info, pt_corner_thresh ) ) {
		return FJ_ERR_IQ_BP_THRESH;
	}


	if( e_detect_mode >= E_IQ_DETECT_ALGO_MAX ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "FJ_IQ_CalibrationCreateBadpixTable(): detect mode %u not supported.\n", e_detect_mode ));
#endif
		return FJ_ERR_NG_INPUT_PARAM;
	}


	detect_info->e_detect_mode = e_detect_mode;
	detect_info->us_threshold_high = us_high_thresh;
	detect_info->us_threshold_low = us_low_thresh;
	detect_info->ui_max_badpix_thresh = ui_max_badpix_thresh;
	detect_info->pt_corner_thresh = pt_corner_thresh;
	detect_info->pt_defect_info_tbl_addr = (U_BF_IQ_DPC_TBL_DATA*)(M_IQ_ROUNDUP_8((UINT32)gbf_iq_dpc_defect_info_tbl_tmp_detect));
	detect_info->ull_defect_info_tbl_byte = D_BF_IQ_DPC_TBL_BYTE_TMP_DETECT;

#ifdef CO_BF_IQ_DPC_DETECT_DBG
	tbl_info = BF_IQ_Dpc_Tbl_Get_Info( e_calib_tgt, tbl_idx );
	if( tbl_info->valid == FALSE ){
		BF_Debug_Print_Warning(( "Debug Bayer Loader Enable! 1st pass\n" ));
		BF_IQ_Calib_Load_BayerFile( "I:\\DPCTEST1.RAW" );
	} else {
		BF_Debug_Print_Warning(( "Debug Bayer Loader Enable! 2nd pass\n" ));
		BF_IQ_Calib_Load_BayerFile( "I:\\DPCTEST2.RAW" );
	}
#endif	// CO_BF_IQ_DPC_DETECT_DBG

	iq_ercd = bf_iq_dpc_detected_sw_if_create_table( detect_info, e_calib_tgt, tbl_idx );

	if( iq_ercd != FJ_ERR_OK ) {
		return iq_ercd;
	}


	/* store out param */
	*ui_over4succession_count = detect_info->ui_over4succession_count;

	return FJ_ERR_OK;
}


