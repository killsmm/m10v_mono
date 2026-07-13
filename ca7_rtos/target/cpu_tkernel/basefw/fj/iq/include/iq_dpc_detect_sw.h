/**
 * @file		iq_dpc_detect_sw.h
 * @brief		Algorithm for Badpix software detection
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __IQ_DPC_DETECT_SW_H__
#define __IQ_DPC_DETECT_SW_H__

#include "fj_iq.h"
#include "fj_std.h"

#include "iq_dpc_detect_info.h"

#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

#define D_BF_IQ_DPC_DETECT_SW_REF_PIXS				(2)


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

// T_BF_IQ_DPC_TBL_MERGE
typedef struct {
	VOID*   pv_defect_info_tbl1_addr;		/**< [in] pickuped defect information table addr */
	UINT32	ui_defect_info_tbl1_byte;		/**< [in] defect information table size(bytes) */
	VOID*   pv_defect_info_tbl2_addr;		/**< [in] pickuped defect information table addr */
	UINT32	ui_defect_info_tbl2_byte;		/**< [in] defect information table size(bytes) */
	VOID*   pv_defect_info_tbl_out_addr;	/**< [out] pickuped defect information table addr */
	UINT32	ui_defect_info_tbl_out_byte;	/**< [inout] defect information table size(bytes) */
	UINT32	ui_detect_info_tbl_out_overflow;/**< [out] ui_defect_info_tbl is overflow */
	UINT32	ui_h_pixels;					/**< [in] bayer hight(pixcels) */
	UINT32	ui_w_pixels;					/**< [in] bayer width(pixcels) */
	UINT32	ui_detect_count;				/**< [out] detected defect pixcels counter */
	UINT32	ui_over4succession_count;		/**< [out] over 4 succession defect pixcels count */
} T_BF_IQ_DPC_TBL_MERGE;


/*==============================================================================*/
/* Function declaration															*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

extern FJ_ERR_CODE BF_IQ_Dpc_Detect_Sw_Run( T_BF_IQ_DPC_DETECT_INFO* const pt_detect_info_arg );
extern FJ_ERR_CODE BF_IQ_Dpc_Detect_Sw_Merge_Tbl( T_BF_IQ_DPC_TBL_MERGE* const pt_dpc_tbl_merge );

#ifdef __cplusplus
}
#endif

#endif /* __IQ_DPC_DETECT_SW_H__ */

