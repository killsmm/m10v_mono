/**
 * @file		iq_dpc_tbl.h
 * @brief		Badpix data table management
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __IQ_DPC_CONV_H__
#define __IQ_DPC_CONV_H__

#include "fj_iq.h"

#include "fj_std.h"

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


extern FJ_ERR_CODE BF_IQ_Dpc_Conv_Run( const E_IQ_CALIB_TGT dst_target, const E_IQ_CALIB_TGT src_target, const FJ_IQ_BADPIX_CONV_INFO* const conv_info );


#ifdef __cplusplus
}
#endif

#endif /* __IQ_DPC_CONV_H__ */

