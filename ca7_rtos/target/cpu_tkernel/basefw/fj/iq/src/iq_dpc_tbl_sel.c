/**
 * @file		iq_dpc_tbl_sel.c
 * @brief		Badpix data table management
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// FJ header
#include "fj_iq.h"

// IQ header
#include "iq_dpc_tbl.h"

// System header
#include "instance.h"

// Driver header
#include "ddim_typedef.h"
#include "im_pro.h"


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

// T_BF_IQ_DPC_TBL_SEL_INFO
typedef struct {
	E_IQ_CALIB_TGT	target;
	UINT32			tbl_idx;
} T_BF_IQ_DPC_TBL_SEL_INFO;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/

static volatile T_BF_IQ_DPC_TBL_SEL_INFO gbf_iq_dpc_tbl_sel[D_BF_INSTANCE_ID_MAX_DEMOSAIC];


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

VOID BF_IQ_Dpc_Tbl_Sel_Init( VOID )
{
	UINT32	loopcnt;

	for( loopcnt = 0; loopcnt < D_BF_INSTANCE_ID_MAX_SEN; loopcnt++ ) {
		gbf_iq_dpc_tbl_sel[loopcnt].target = E_IQ_CALIB_TGT_INIT;
		gbf_iq_dpc_tbl_sel[loopcnt].tbl_idx = D_BF_IQ_DPC_TBL_IDX_ERR;
	}
}


VOID BF_IQ_Dpc_Tbl_Sel_Get( const UCHAR demosaic_id, E_IQ_CALIB_TGT* const calib_tgt, UINT32* const tbl_idx )
{
	if( demosaic_id >= D_BF_INSTANCE_ID_MAX_DEMOSAIC ) {
		*calib_tgt = E_IQ_CALIB_TGT_ERR;
	}

	*calib_tgt = gbf_iq_dpc_tbl_sel[demosaic_id].target;
	*tbl_idx = gbf_iq_dpc_tbl_sel[demosaic_id].tbl_idx;

}


FJ_ERR_CODE BF_IQ_Dpc_Tbl_Sel_Set( const UCHAR demosaic_id, const E_IQ_CALIB_TGT target, UINT32 tbl_idx )
{
	if( demosaic_id >= D_BF_INSTANCE_ID_MAX_DEMOSAIC ) {
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( tbl_idx >= D_BF_IQ_DPC_TBL_IDX_MAX ) {
		return FJ_ERR_NG_INPUT_PARAM;
	}

	gbf_iq_dpc_tbl_sel[demosaic_id].target = target;
	gbf_iq_dpc_tbl_sel[demosaic_id].tbl_idx = tbl_idx;

	return FJ_ERR_OK;
}

