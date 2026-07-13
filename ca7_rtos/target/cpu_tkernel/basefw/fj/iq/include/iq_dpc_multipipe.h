/**
 * @file		iq_dpc_multipipe.h
 * @brief		Badpix Detection/Compensation control API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef __IQ_DPC_MULTIPIPE_H__
#define __IQ_DPC_MULTIPIPE_H__

#include "fj_std.h"

#include "iq_dpc_multipipe_tbl.h"
#include "iq_dpc_custom.h"

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

// T_BF_IQ_DPC_MULTIPIPE_CALC_PARAM
typedef struct {
	UINT32					x_num;
	UINT32					y_num;
	T_BF_IQ_DPC_AREA_INFO	crop_info[D_BF_IQ_DPC_MULTIPIPE_TBL_X_NUM][D_BF_IQ_DPC_MULTIPIPE_TBL_Y_NUM];
	BOOL					x_decrease_crop_offset;
	BOOL					y_decrease_crop_offset;
} T_BF_IQ_DPC_MULTIPIPE_CALC_PARAM;


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


extern VOID BF_IQ_Dpc_MultiPipe_Init( VOID );
extern FJ_ERR_CODE BF_IQ_Dpc_MultiPipe_Calc( const UCHAR demosaic_id, const T_BF_IQ_DPC_MULTIPIPE_CALC_PARAM* const calc_param );
extern VOID BF_IQ_Dpc_MultiPipe_Print_SizeDbg( VOID );


#ifdef __cplusplus
}
#endif

#endif // __IQ_DPC_MULTIPIPE_H__

