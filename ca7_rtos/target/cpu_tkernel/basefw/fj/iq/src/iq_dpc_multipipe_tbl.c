/**
 * @file		iq_dpc_multipipe_tbl.c
 * @brief		Badpix Detection/Compensation control API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

// IQ header
#include "iq_dpc_multipipe_tbl.h"
#include "iq_dpc_tbl.h"
#include "iq_common.h"

// Utility header
#include "debug.h"

// Driver header
#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

#define D_BF_IQ_DPC_MULTIPIPE_TBL_BYTES	(D_BF_IQ_DPC_TBL_BYTES + (16 *8 *8 *2))	// Margin of 16 bytes alignment of one axis.
																				// 16 bytes  x  8 x-axis  x  8 y-axis  x  2 fields


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

/*
 * Bad pixel table.
 */
static volatile UCHAR	gbf_iq_dpc_multipipe_tbl_info_tbl[D_BF_IQ_DPC_MULTIPIPE_TBL_MAX_NUM][M_IQ_ROUNDUP_PRCH_BURST_BYTES_MIN(D_BF_IQ_DPC_MULTIPIPE_TBL_BYTES)] __attribute__((aligned(D_IQ_PRCH_BURST_BYTES_MIN)));


/*
 * Bad pixel table metadata
 */
static volatile T_BF_IQ_DPC_MULTIPIPE_TBL g_bf_iq_dpc_multipipe_tbl_info[D_BF_IQ_DPC_MULTIPIPE_TBL_MAX_NUM];


/*
 * Bad pixel table metadata allocation information.
 */
static volatile BOOL g_bf_iq_dpc_multipipe_tbl_info_alloc[D_BF_IQ_DPC_MULTIPIPE_TBL_MAX_NUM];
static volatile UINT32 g_bf_iq_dpc_multipipe_tbl_info_alloc_last_idx = D_BF_IQ_DPC_MULTIPIPE_TBL_MAX_NUM -1;


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

FJ_ERR_CODE BF_IQ_Dpc_MultiPipeTbl_Init( VOID )
{
	UINT32							loopcnt1;
	UINT32							loopcnt_x;
	UINT32							loopcnt_y;
	UINT32							loopcnt_field;
	volatile T_BF_IQ_DPC_TBL_INFO*	tbl_info;

	for( loopcnt1 = 0; loopcnt1 < D_BF_IQ_DPC_MULTIPIPE_TBL_MAX_NUM; loopcnt1++ ) {
		g_bf_iq_dpc_multipipe_tbl_info[loopcnt1].tbl_idx	= loopcnt1;
		g_bf_iq_dpc_multipipe_tbl_info[loopcnt1].valid		= FALSE;
		g_bf_iq_dpc_multipipe_tbl_info[loopcnt1].top_addr	= (U_BF_IQ_DPC_TBL_DATA*)&gbf_iq_dpc_multipipe_tbl_info_tbl[loopcnt1][0];
		g_bf_iq_dpc_multipipe_tbl_info[loopcnt1].maxbytelen	= sizeof(gbf_iq_dpc_multipipe_tbl_info_tbl[loopcnt1]);

		for( loopcnt_y = 0; loopcnt_y < D_BF_IQ_DPC_MULTIPIPE_TBL_Y_NUM; loopcnt_y++ ) {
			for( loopcnt_x = 0; loopcnt_x < D_BF_IQ_DPC_MULTIPIPE_TBL_X_NUM; loopcnt_x++ ) {
				tbl_info = &g_bf_iq_dpc_multipipe_tbl_info[loopcnt1].tbl_info[loopcnt_x][loopcnt_y];
				for( loopcnt_field = 0; loopcnt_field < D_BF_IQ_DPC_MAX_FIELDS; loopcnt_field++ ) {
					tbl_info->addr_by_fields[loopcnt_field]		= NULL;
					tbl_info->bytelen_by_fields[loopcnt_field]	= 0;
				}
				tbl_info->top_addr		= (U_BF_IQ_DPC_TBL_DATA*)&gbf_iq_dpc_multipipe_tbl_info_tbl[loopcnt1][0];
				tbl_info->maxbytelen	= sizeof(gbf_iq_dpc_multipipe_tbl_info_tbl[loopcnt1]);
				tbl_info->bytelen		= 0;
				tbl_info->pixel_num		= 0;
				tbl_info->valid			= FALSE;
			}
		}

		g_bf_iq_dpc_multipipe_tbl_info_alloc[loopcnt1] = FALSE;
	}

	return FJ_ERR_OK;
}


T_BF_IQ_DPC_MULTIPIPE_TBL* BF_IQ_Dpc_MultiPipeTbl_Alloc( VOID )
{
	UINT32	loopcnt;
	UINT32	last_idx;
	UINT32	idx;

	last_idx = g_bf_iq_dpc_multipipe_tbl_info_alloc_last_idx;

	for( loopcnt = 0; loopcnt < D_BF_IQ_DPC_MULTIPIPE_TBL_MAX_NUM; loopcnt++ ) {
		idx = (last_idx + loopcnt) % D_BF_IQ_DPC_MULTIPIPE_TBL_MAX_NUM;

		if( g_bf_iq_dpc_multipipe_tbl_info_alloc[idx] == FALSE ) {
			g_bf_iq_dpc_multipipe_tbl_info_alloc[idx] = TRUE;
			g_bf_iq_dpc_multipipe_tbl_info_alloc_last_idx = idx;
			return (T_BF_IQ_DPC_MULTIPIPE_TBL*)&g_bf_iq_dpc_multipipe_tbl_info[idx];
		}
	}

	return NULL;
}


VOID BF_IQ_Dpc_MultiPipeTbl_Free( T_BF_IQ_DPC_MULTIPIPE_TBL* const multipipe_tbl )
{
	UINT32 tbl_idx;
	
	tbl_idx = multipipe_tbl->tbl_idx;


	if( g_bf_iq_dpc_multipipe_tbl_info_alloc[tbl_idx] == FALSE ) {
		BF_Debug_Print_Warning(( "%s(): already free. %u 0x%lx\n", __func__, tbl_idx, (ULONG)multipipe_tbl ));
		return;
	}

	multipipe_tbl->valid = FALSE;
	g_bf_iq_dpc_multipipe_tbl_info_alloc[tbl_idx] = FALSE;
}


#ifdef FJ_DEBUG_PRINT_ON
VOID BF_IQ_Dpc_MultiPipeTbl_Stat( const T_BF_IQ_DPC_MULTIPIPE_TBL* const multipipe_tbl )
{
	UINT32						loopcnt_x;
	UINT32						loopcnt_y;
	const T_BF_IQ_DPC_TBL_INFO*	tbl;

	for( loopcnt_y = 0; loopcnt_y < D_BF_IQ_DPC_MULTIPIPE_TBL_Y_NUM; loopcnt_y++ ) {
		for( loopcnt_x = 0; loopcnt_x < D_BF_IQ_DPC_MULTIPIPE_TBL_X_NUM; loopcnt_x++ ) {
			tbl = &multipipe_tbl->tbl_info[loopcnt_x][loopcnt_y];
			if( tbl->valid != FALSE ) {
				BF_Debug_Print_Information(( "[idx %u  axis %u, %u] valid=%u\n",
							multipipe_tbl->tbl_idx, loopcnt_x, loopcnt_y, tbl->valid ));
				BF_Debug_Print_Information(( "raw_rect=%ux%u\n", tbl->width, tbl->lines ));
				BF_Debug_Print_Information(( "ofs=%u, %u\n", tbl->sta_x, tbl->sta_y ));
				BF_Debug_Print_Information(( "pixel_num=%u (%lu bytes)\n", tbl->pixel_num, (ULONG)tbl->bytelen ));
				BF_Debug_Print_Information(( "addr=%p(%lu) %p(%lu)\n", tbl->addr_by_fields[0], (ULONG)tbl->bytelen_by_fields[0], tbl->addr_by_fields[1], (ULONG)tbl->bytelen_by_fields[1] ));
				BF_Debug_Print_Information(( "\n" ));
			}
		}
	}
}
#else
VOID BF_IQ_Dpc_MultiPipeTbl_Stat( const T_BF_IQ_DPC_MULTIPIPE_TBL* const multipipe_tbl )
{
}
#endif


#ifdef CO_BF_IQ_DBG_TABLE_BYTES
VOID BF_IQ_Dpc_MultiPipeTbl_Print_SizeDbg( VOID )
{
	UINT32	loopcnt;

	printf( "[Badpix multi pipe tbl memory]\n" );
	for( loopcnt = 0; loopcnt < D_BF_IQ_DPC_MULTIPIPE_TBL_MAX_NUM; loopcnt++ ) {
		printf( "g_bf_iq_dpc_multipipe_tbl_info %u_memory = %lu bytes\n",
				loopcnt,
				(ULONG)g_bf_iq_dpc_multipipe_tbl_info[loopcnt].maxbytelen );
	}
}
#endif //CO_BF_IQ_DBG_TABLE_BYTES

