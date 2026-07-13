/**
 * @file		iq_dpc_tbl_sel.h
 * @brief		Badpix data table management
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __IQ_DPC_TBL_SEL_H__
#define __IQ_DPC_TBL_SEL_H__

#include "fj_iq.h"

#include "fj_std.h"

#include "iq_common.h"

#include "ddim_typedef.h"


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


extern VOID BF_IQ_Dpc_Tbl_Sel_Init( VOID );
extern E_IQ_CALIB_TGT BF_IQ_Dpc_Tbl_Sel_Get( const UCHAR demosaic_id, E_IQ_CALIB_TGT* const calib_tgt, UINT32* const tbl_idx );
extern FJ_ERR_CODE BF_IQ_Dpc_Tbl_Sel_Set( const UCHAR demosaic_id, const E_IQ_CALIB_TGT target, UINT32 tbl_idx );


#ifdef __cplusplus
}
#endif

#endif /* __IQ_DPC_TBL_SEL_H__ */

