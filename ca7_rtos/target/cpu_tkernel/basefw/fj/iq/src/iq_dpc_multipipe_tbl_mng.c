/**
 * @file		iq_dpc_multipipe_tbl_mng.c
 * @brief		Badpix Detection/Compensation control API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

// IQ header
#include "iq_dpc_multipipe_tbl_mng.h"
#include "iq_dpc_multipipe_tbl.h"
#include "iq_dpc_tbl.h"
#include "iq_dpc_custom.h"
#include "iq_common.h"

// System header
#include "instance.h"

// Utility header
#include "debug.h"

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

// T_BF_IQ_DPC_MULTIPLE_TBL_MNG
typedef struct {
	BOOL						valid;
	T_BF_IQ_DPC_MULTIPIPE_TBL*	curr;
	T_BF_IQ_DPC_MULTIPIPE_TBL*	next;
} T_BF_IQ_DPC_MULTIPLE_TBL_MNG;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/

/*
 * Bad pixel table manager
 */
static volatile T_BF_IQ_DPC_MULTIPLE_TBL_MNG g_bf_iq_dpc_multiple_tbl_mng[D_BF_INSTANCE_ID_MAX_DEMOSAIC];


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
#ifdef CO_BF_IQ_DPC_DEBUG
static VOID iqsub_dpc_tbl_dump( VOID* pv_defect_info_tbl, UINT32 ui_defect_info_tbl_byte, VOID* pv_bayer_addr, UINT32 ui_columns );
#endif

FJ_ERR_CODE BF_IQ_Dpc_MultiPipeTblMng_Init( VOID )
{
	UINT32							loopcnt1;

	for( loopcnt1 = 0; loopcnt1 < D_BF_INSTANCE_ID_MAX_DEMOSAIC; loopcnt1++ ) {
		g_bf_iq_dpc_multiple_tbl_mng[loopcnt1].valid	= FALSE;
		g_bf_iq_dpc_multiple_tbl_mng[loopcnt1].curr		= NULL;
		g_bf_iq_dpc_multiple_tbl_mng[loopcnt1].next		= NULL;
	}

	return FJ_ERR_OK;
}


T_BF_IQ_DPC_MULTIPIPE_TBL* BF_IQ_Dpc_MultiPipeTblMng_Get_Info( const UCHAR demosaic_id, const BOOL is_frame_1st )
{
	T_BF_IQ_DPC_MULTIPLE_TBL_MNG*	tbl_mng;

	if( demosaic_id >= D_BF_INSTANCE_ID_MAX_DEMOSAIC ) {
		return NULL;
	}

	tbl_mng = (T_BF_IQ_DPC_MULTIPLE_TBL_MNG*)&g_bf_iq_dpc_multiple_tbl_mng[demosaic_id];

	if( tbl_mng->valid == FALSE ) {
		return NULL;
	}

	if( is_frame_1st != FALSE ) {
		if( tbl_mng->curr != tbl_mng->next ) {
			if( tbl_mng->curr != NULL ) {
				BF_IQ_Dpc_MultiPipeTbl_Free( tbl_mng->curr );
			}
			tbl_mng->curr = tbl_mng->next;
		}
	}

	if( tbl_mng->curr == NULL ) {
		return NULL;
	}

	return tbl_mng->curr;
}


T_BF_IQ_DPC_TBL_INFO* BF_IQ_Dpc_MultiPipeTblMng_Get_TblByAxisInfo( const UCHAR demosaic_id, const T_BF_IQ_SPLIT_SEL_INFO* const split_sel_info )
{
	T_BF_IQ_DPC_MULTIPIPE_TBL* multipipe_tbl;
	
	multipipe_tbl = BF_IQ_Dpc_MultiPipeTblMng_Get_Info( demosaic_id, split_sel_info->is_frame_1st );
	
	if( multipipe_tbl == NULL ) {
		return NULL;
	}

	if( multipipe_tbl->x_num <= split_sel_info->x_idx ) {
		return NULL;
	}
	if( multipipe_tbl->y_num <= split_sel_info->y_idx ) {
		return NULL;
	}

	return &multipipe_tbl->tbl_info[split_sel_info->x_idx][split_sel_info->y_idx];
}


T_BF_IQ_DPC_AREA_INFO* BF_IQ_Dpc_MultiPipeTblMng_Get_CropWinByAxisInfo( const UCHAR demosaic_id, const T_BF_IQ_SPLIT_SEL_INFO* const split_sel_info )
{
	T_BF_IQ_DPC_MULTIPIPE_TBL* multipipe_tbl;
	
	multipipe_tbl = BF_IQ_Dpc_MultiPipeTblMng_Get_Info( demosaic_id, split_sel_info->is_frame_1st );
	
	if( multipipe_tbl == NULL ) {
		return NULL;
	}

	if( multipipe_tbl->x_num <= split_sel_info->x_idx ) {
		return NULL;
	}
	if( multipipe_tbl->y_num <= split_sel_info->y_idx ) {
		return NULL;
	}

	return &multipipe_tbl->crop_win[split_sel_info->x_idx][split_sel_info->y_idx];
}


FJ_ERR_CODE BF_IQ_Dpc_MultiPipeTblMng_Set_Info( const UCHAR demosaic_id, T_BF_IQ_DPC_MULTIPIPE_TBL* const multipipe_tbl )
{
	T_BF_IQ_DPC_MULTIPLE_TBL_MNG*	tbl_mng;

	if( demosaic_id >= D_BF_INSTANCE_ID_MAX_DEMOSAIC ) {
		return FJ_ERR_NG;
	}

	tbl_mng = (T_BF_IQ_DPC_MULTIPLE_TBL_MNG*)&g_bf_iq_dpc_multiple_tbl_mng[demosaic_id];

	if( tbl_mng->curr != tbl_mng->next ) {
		if( tbl_mng->next != NULL ) {
			BF_IQ_Dpc_MultiPipeTbl_Free( tbl_mng->next );
		}
	}
	tbl_mng->next = multipipe_tbl;
	tbl_mng->valid = TRUE;

	return FJ_ERR_OK;
}


#ifdef FJ_DEBUG_PRINT_ON
VOID BF_IQ_Dpc_MultiPipeTblMng_Stat( VOID )
{
	UINT32							loopcnt;
	UINT32							loopcnt_x;
	UINT32							loopcnt_y;
	const T_BF_IQ_DPC_TBL_INFO*		tbl;
	const T_BF_IQ_DPC_AREA_INFO*	crop_win;
	T_BF_IQ_SPLIT_SEL_INFO			split_sel_info = {
		.x_idx = 0, .y_idx = 0, .is_frame_1st = FALSE,
	};

	BF_Debug_Print_Information(( "[multipipe_info]\n" ));
	for( loopcnt = 0; loopcnt < D_BF_INSTANCE_ID_MAX_DEMOSAIC; loopcnt++ ) {
		for( loopcnt_y = 0; loopcnt_y < D_BF_INSTANCE_ID_MAX_DEMOSAIC; loopcnt_y++ ) {
			for( loopcnt_x = 0; loopcnt_x < D_BF_INSTANCE_ID_MAX_DEMOSAIC; loopcnt_x++ ) {
				split_sel_info.x_idx = loopcnt_x;
				split_sel_info.y_idx = loopcnt_y;
				tbl = BF_IQ_Dpc_MultiPipeTblMng_Get_TblByAxisInfo( loopcnt, &split_sel_info );
				if( tbl == NULL ) {
//					BF_Debug_Print_Information(( "[demosaic_id %u  axis %u, %u]\n", loopcnt, loopcnt_x, loopcnt_y ));
//					BF_Debug_Print_Information(( "buffer not initialized.\n" ));
					continue;
				}
				crop_win = BF_IQ_Dpc_MultiPipeTblMng_Get_CropWinByAxisInfo( loopcnt, &split_sel_info );
				if( crop_win == NULL ) {
					continue;
				}

				if( tbl->valid != FALSE ) {
					BF_Debug_Print_Information(( "[demosaic_id %u  axis %u, %u] valid=%u\n",
								loopcnt, loopcnt_x, loopcnt_y, tbl->valid ));
					BF_Debug_Print_Information(( "raw_sta=%u, %u raw_rect=%ux%u\n", crop_win->pos_x, crop_win->pos_y, crop_win->width, crop_win->lines ));
					BF_Debug_Print_Information(( "ofs=%u, %u\n", tbl->sta_x, tbl->sta_y ));
					BF_Debug_Print_Information(( "pixel_num=%u (%lu bytes)\n", tbl->pixel_num, (ULONG)tbl->bytelen ));
					BF_Debug_Print_Information(( "addr=%p(%lu) %p(%lu)\n", tbl->addr_by_fields[0], (ULONG)tbl->bytelen_by_fields[0], tbl->addr_by_fields[1], (ULONG)tbl->bytelen_by_fields[1] ));
					BF_Debug_Print_Information(( "\n" ));
				}
			}
		}
	}
	BF_Debug_Print_Information(( "[multipipe_info end]\n" ));
}
#else
VOID BF_IQ_Dpc_MultiPipeTblMng_Stat( VOID )
{
}
#endif


#ifdef CO_BF_IQ_DPC_DEBUG
VOID BF_IQ_Dpc_MultiPipeTblMng_Dump( const UCHAR demosaic_id )
{
	UINT32						loopcnt_x;
	UINT32						loopcnt_y;
	const T_BF_IQ_DPC_TBL_INFO*	tbl;
	T_BF_IQ_SPLIT_SEL_INFO		split_sel_info = {
		.x_idx = 0, .y_idx = 0, .is_frame_1st = FALSE,
	};

	BF_Debug_Print_Information(( "[multipipe_info]\n" ));
	for( loopcnt_y = 0; loopcnt_y < D_BF_INSTANCE_ID_MAX_DEMOSAIC; loopcnt_y++ ) {
		for( loopcnt_x = 0; loopcnt_x < D_BF_INSTANCE_ID_MAX_DEMOSAIC; loopcnt_x++ ) {
			split_sel_info.x_idx = loopcnt_x;
			split_sel_info.y_idx = loopcnt_y;
			tbl = BF_IQ_Dpc_MultiPipeTblMng_Get_TblByAxisInfo( demosaic_id, &split_sel_info );
			if( tbl == NULL ) {
//					BF_Debug_Print_Information(( "[demosaic_id %u  axis %u, %u]\n", demosaic_id, loopcnt_x, loopcnt_y ));
//					BF_Debug_Print_Information(( "buffer not initialized.\n" ));
				continue;
			}

			if( tbl->valid != FALSE ) {
				BF_Debug_Print_Information(( "[demosaic_id %u  axis %u, %u] valid=%u\n",
							demosaic_id, loopcnt_x, loopcnt_y, tbl->valid ));
				iqsub_dpc_tbl_dump( tbl->addr_by_fields[0], tbl->bytelen_by_fields[0], NULL, 0 );
				iqsub_dpc_tbl_dump( tbl->addr_by_fields[1], tbl->bytelen_by_fields[1], NULL, 0 );
				BF_Debug_Print_Information(( "\n" ));
			}
		}
	}
	BF_Debug_Print_Information(( "[multipipe_info end]\n" ));
}
#else
VOID BF_IQ_Dpc_MultiPipeTblMng_Dump( const UCHAR demosaic_id )
{
}
#endif

