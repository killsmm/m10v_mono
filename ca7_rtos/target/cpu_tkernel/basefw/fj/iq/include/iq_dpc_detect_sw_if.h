/**
 * @file		iq_dpc_detect_sw_if.h
 * @brief		Badpix software detection API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __IQ_DPC_DETECT_SW_IF_H__
#define __IQ_DPC_DETECT_SW_IF_H__

#include "fj_iq.h"

#include "fj_std.h"

#include "ddim_typedef.h"
#include "iq_dpc_detect_info.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Macro declaration															*/
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
/* Function declaration															*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

FJ_ERR_CODE  BF_IQ_Dpc_Create_Bad_Pix_Table( const E_IQ_CALIB_TGT e_calib_tgt,
											const UINT32 tbl_idx, 
											const BOOL is_merge_last_table,
											T_BF_IQ_DPC_DETECT_INFO* const detect_info );

FJ_ERR_CODE fj_iq_calibrationcreatebadpixtable( const UCHAR ch,
											const E_IQ_CALIB_TGT e_calib_tgt,
											const UINT32 tbl_idx, 
											const E_IQ_DETECT_ALGORITHM e_detect_mode,
											const USHORT us_low_thresh,
											const USHORT us_high_thresh,
											const UINT32 ui_max_badpix_thresh,
											const FJ_IQ_CORNER_THRESHOLD* const pt_corner_thresh,
											UINT32* const ui_over4succession_count );

#ifdef __cplusplus
}
#endif

#endif /* __IQ_DPC_DETECT_SW_IF_H__ */

