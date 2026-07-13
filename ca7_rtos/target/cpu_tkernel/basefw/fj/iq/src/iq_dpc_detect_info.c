/**
 * @file		iq_dpc_detect_info.c
 * @brief		Badpix software/hardware detection parameter management
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// FJ header
#include "fj_iq.h"
#include "fj_std.h"

// IQ header
#include "iq_dpc_detect_info.h"

#include "iq_calib.h"

// System header
#include "instance.h"

// Driver header
#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// nothing special.


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
/** Dead Pixel Compensation detection information(IQ internal use only) */
static volatile T_BF_IQ_DPC_DETECT_INFO		gbf_iq_dpc_detect_Info[D_BF_INSTANCE_ID_MAX_SEN] = { { 0 } };


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

/** @brief			BF_IQ_Dpc_Detect_Info_Set_CalibParam
 *					setting Badpix parameter
 *	@param[in]		pt_calib_param		detect parameters structure
 *	@note			None
 *	@attention		None
 */
VOID BF_IQ_Dpc_Detect_Info_Set_CalibParam( const UCHAR sen_id, const T_BF_IQ_CALIB_PARAM* const pt_calib_param )
{
	gbf_iq_dpc_detect_Info[sen_id].pus_bayer16_addr    = pt_calib_param->pus_bayer16_addr;
	gbf_iq_dpc_detect_Info[sen_id].ui_bayer_byte       = pt_calib_param->ui_bayer_bytelen;
	gbf_iq_dpc_detect_Info[sen_id].us_global_h_pixels  = pt_calib_param->us_global_rows;
	gbf_iq_dpc_detect_Info[sen_id].us_global_w_pixels  = pt_calib_param->us_global_columns;
	gbf_iq_dpc_detect_Info[sen_id].ui_field_num        = pt_calib_param->us_field_num;
	gbf_iq_dpc_detect_Info[sen_id].us_vd_ofs           = pt_calib_param->us_vd_ofs;
	gbf_iq_dpc_detect_Info[sen_id].us_hd_ofs           = pt_calib_param->us_hd_ofs;
	gbf_iq_dpc_detect_Info[sen_id].us_upper_trim       = pt_calib_param->us_upper_trim;
	gbf_iq_dpc_detect_Info[sen_id].us_left_trim        = pt_calib_param->us_left_trim;
	gbf_iq_dpc_detect_Info[sen_id].us_h_pixels         = pt_calib_param->us_rows;
	gbf_iq_dpc_detect_Info[sen_id].us_w_pixels         = pt_calib_param->us_columns;
	gbf_iq_dpc_detect_Info[sen_id].pt_line_order_table = pt_calib_param->pt_line_order_table;
}


VOID BF_IQ_Dpc_Detect_Info_Set_Bay( const UCHAR sen_id, const VOID* const bay_addr, const UINT32 bay_bytes, const UINT32 width, const UINT32 lines )
{
	gbf_iq_dpc_detect_Info[sen_id].pus_bayer16_addr    = bay_addr;
	gbf_iq_dpc_detect_Info[sen_id].ui_bayer_byte       = bay_bytes;
	gbf_iq_dpc_detect_Info[sen_id].us_global_w_pixels  = width;
	gbf_iq_dpc_detect_Info[sen_id].us_global_h_pixels  = lines;
}


VOID BF_IQ_Dpc_Detect_Info_Set_BayAddr( const UCHAR sen_id, const VOID* const bay_addr )
{
	gbf_iq_dpc_detect_Info[sen_id].pus_bayer16_addr    = bay_addr;
}


T_BF_IQ_DPC_DETECT_INFO* BF_IQ_Dpc_Detect_Info_Get_Info( const UCHAR sen_id )
{
	return (T_BF_IQ_DPC_DETECT_INFO*)&gbf_iq_dpc_detect_Info[sen_id];
}

