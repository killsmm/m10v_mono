/**
 * @file		iq_dpc_multipipe_tbl_mng.h
 * @brief		Badpix Detection/Compensation control API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef __IQ_DPC_MULTIPIPE_TBL_MNG_H__
#define __IQ_DPC_MULTIPIPE_TBL_MNG_H__

#include "fj_std.h"

#include "iq_dpc_multipipe_tbl.h"
#include "iq_dpc_tbl.h"
#include "iq_dpc_custom.h"
#include "iq_multipipe.h"

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
// nothing special.


/*==============================================================================*/
/* Function																		*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


extern FJ_ERR_CODE BF_IQ_Dpc_MultiPipeTblMng_Init( VOID );
extern T_BF_IQ_DPC_MULTIPIPE_TBL* BF_IQ_Dpc_MultiPipeTblMng_Get_Info( const UCHAR demosaic_id, const BOOL is_frame_1st );
extern T_BF_IQ_DPC_TBL_INFO* BF_IQ_Dpc_MultiPipeTblMng_Get_TblByAxisInfo( const UCHAR demosaic_id, const T_BF_IQ_SPLIT_SEL_INFO* const split_sel_info );
extern T_BF_IQ_DPC_AREA_INFO* BF_IQ_Dpc_MultiPipeTblMng_Get_CropWinByAxisInfo( const UCHAR demosaic_id, const T_BF_IQ_SPLIT_SEL_INFO* const split_sel_info );
extern FJ_ERR_CODE BF_IQ_Dpc_MultiPipeTblMng_Set_Info( const UCHAR demosaic_id, T_BF_IQ_DPC_MULTIPIPE_TBL* const multipipe_tbl );
extern VOID BF_IQ_Dpc_MultiPipeTblMng_Stat( VOID );
extern VOID BF_IQ_Dpc_MultiPipeTblMng_Dump( const UCHAR demosaic_id );


#ifdef __cplusplus
}
#endif

#endif // __IQ_DPC_MULTIPIPE_TBL_MNG_H__

