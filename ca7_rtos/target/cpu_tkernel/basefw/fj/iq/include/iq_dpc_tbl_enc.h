/**
 * @file		iq_dpc_tbl_enc.h
 * @brief		Badpix table encorder.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef __IQ_DPC_TBL_ENC_H__
#define __IQ_DPC_TBL_ENC_H__

#include "fj_iq.h"

#include "iq_dpc_tbl.h"

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

// T_BF_IQ_DPC_TBL_ENC_PARAM
typedef struct {
	// Dpc table data
	FJ_IQ_BADPIX_POS*				pos_tbl_top;
	UINT32							pos_maxnum;
	FJ_IQ_BADPIX_POS*				pos_tbl[2];
	UINT32							pos_num[2];
	UINT32							pos_num_all;
	BOOL							flg_declease_sta;
} T_BF_IQ_DPC_TBL_ENC_PARAM;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Function																		*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


extern FJ_ERR_CODE BF_IQ_Dpc_Tbl_Enc( T_BF_IQ_DPC_TBL_ENC_PARAM* const param, T_BF_IQ_DPC_TBL_INFO* const tbl_info );


#ifdef __cplusplus
}
#endif

#endif // __IQ_DPC_TBL_ENC_H__

