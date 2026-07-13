/**
 * @file		iq_dpc_tbl.c
 * @brief		Badpix data table management
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_iq.h"

#include "iq_dpc_tbl.h"
#include "iq_dpc_detect_info.h"
#include "iq_dpc_custom.h"

#include "iq_calib.h"

#include "iq_common.h"
#include "iq_dcache.h"

#include "still.h"
//#include "still_view.h"
#include "still_attr.h"

#include "ddim_typedef.h"
#include "debug.h"

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
// nothing special.


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/

/*
 * Bad pixel table (buffer)
 */
/** Dead Pixel Compensation table - body */
static volatile UCHAR	gbf_iq_dpc_defect_info_tbl[E_IQ_CALIB_TGT_MAX][D_BF_IQ_DPC_TBL_IDX_MAX][D_BF_IQ_DPC_TBL_BYTES] __attribute__((aligned(8)));


/*
 * Bad pixel table metadata
 */
static volatile T_BF_IQ_DPC_TBL_INFO gBF_IQ_DpcTblInfo[E_IQ_CALIB_TGT_MAX][D_BF_IQ_DPC_TBL_IDX_MAX];


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
#ifdef CO_BF_IQ_DPC_DEBUG
static VOID iqsub_dpc_tbl_dump( VOID* pv_defect_info_tbl, UINT32 ui_defect_info_tbl_byte, VOID* pv_bayer_addr, UINT32 ui_columns );
#endif


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

FJ_ERR_CODE BF_IQ_Dpc_Tbl_Init( VOID )
{
	E_IQ_CALIB_TGT	e_target;
	UINT32			tbl_idx;
	UINT32			loopcnt;

	for( e_target = E_IQ_CALIB_TGT_INIT; e_target < E_IQ_CALIB_TGT_MAX; e_target++ ) {
		for( tbl_idx = 0; tbl_idx < D_BF_IQ_DPC_TBL_IDX_MAX; tbl_idx++ ) {
			for( loopcnt = 0; loopcnt < D_BF_IQ_DPC_MAX_FIELDS; loopcnt++ ) {
				gBF_IQ_DpcTblInfo[e_target][tbl_idx].addr_by_fields[loopcnt]	= NULL;
				gBF_IQ_DpcTblInfo[e_target][tbl_idx].bytelen_by_fields[loopcnt]	= 0;
			}
			gBF_IQ_DpcTblInfo[e_target][tbl_idx].top_addr	= (U_BF_IQ_DPC_TBL_DATA*)&gbf_iq_dpc_defect_info_tbl[e_target][tbl_idx][0];
			gBF_IQ_DpcTblInfo[e_target][tbl_idx].maxbytelen	= sizeof(gbf_iq_dpc_defect_info_tbl[e_target][tbl_idx]);
			gBF_IQ_DpcTblInfo[e_target][tbl_idx].bytelen	= 0;
			gBF_IQ_DpcTblInfo[e_target][tbl_idx].pixel_num	= 0;
			gBF_IQ_DpcTblInfo[e_target][tbl_idx].valid		= FALSE;
		}
	}

	return FJ_ERR_OK;
}


T_BF_IQ_DPC_TBL_INFO* BF_IQ_Dpc_Tbl_Get_Info( const E_IQ_CALIB_TGT e_calib_tgt, const UINT32 tbl_idx )
{
	if( e_calib_tgt >= E_IQ_CALIB_TGT_MAX ) {
		return NULL;
	}
	if( gBF_IQ_DpcTblInfo[e_calib_tgt][tbl_idx].top_addr == NULL ) {
		return NULL;
	}

	return (T_BF_IQ_DPC_TBL_INFO*)&gBF_IQ_DpcTblInfo[e_calib_tgt][tbl_idx];
}


#ifdef FJ_DEBUG_PRINT_ON
VOID BF_IQ_Dpc_Tbl_Stat( VOID )
{
	E_IQ_CALIB_TGT	e_calib_tgt;
	UINT32			tbl_idx;
	const T_BF_IQ_DPC_TBL_INFO* tbl;

	for( e_calib_tgt = (E_IQ_CALIB_TGT)(E_IQ_CALIB_TGT_INIT +1); e_calib_tgt < E_IQ_CALIB_TGT_MAX; e_calib_tgt++ ) {
		for( tbl_idx = 0; tbl_idx < D_BF_IQ_DPC_TBL_IDX_MAX; tbl_idx++ ) {
			tbl = BF_IQ_Dpc_Tbl_Get_Info( e_calib_tgt, tbl_idx );
			if( tbl == NULL ) {
				BF_Debug_Print_Information(( "[calib_tgt %s(%u) idx %u]\n",
							BF_IQ_Calib_Get_CalibTgtName( e_calib_tgt ),
							e_calib_tgt,
							tbl_idx ));
				BF_Debug_Print_Information(( "buffer not initialized.\n" ));
				continue;
			}

			BF_Debug_Print_Information(( "[calib_tgt %s(%u) idx %u] valid=%u\n",
						BF_IQ_Calib_Get_CalibTgtName( e_calib_tgt ),
						e_calib_tgt,
						tbl_idx,
						tbl->valid ));
			if( tbl->valid != FALSE ) {
				BF_Debug_Print_Information(( "raw_rect=%ux%u\n", tbl->width, tbl->lines ));
				BF_Debug_Print_Information(( "ofs=%u, %u\n", tbl->sta_x, tbl->sta_y ));
//				BF_Debug_Print_Information(( "sensor_fields=%u\n", tbl->field_num ));
				BF_Debug_Print_Information(( "pixel_num=%u (%lu bytes)\n", tbl->pixel_num, (ULONG)tbl->bytelen ));
				BF_Debug_Print_Information(( "addr=%p(%lu) %p(%lu)\n", tbl->addr_by_fields[0], (ULONG)tbl->bytelen_by_fields[0], tbl->addr_by_fields[1], (ULONG)tbl->bytelen_by_fields[1] ));
				BF_Debug_Print_Information(( "\n" ));
			}
		}
	}
}
#else
VOID BF_IQ_Dpc_Tbl_Stat( VOID )
{
}
#endif


#ifdef CO_BF_IQ_DBG_TABLE_BYTES
VOID BF_IQ_Dpc_Tbl_Print_SizeDbg( VOID )
{
	USHORT	tbl_type;
	UINT32	tbl_idx;

	printf( "[Badpix tbl memory]\n" );
	for( tbl_type = E_IQ_CALIB_TGT_INIT +1; tbl_type < E_IQ_CALIB_TGT_MAX; tbl_type++ ) {
		for( tbl_idx = 0; tbl_idx < D_BF_IQ_DPC_TBL_IDX_MAX; tbl_idx++ ) {
			printf( "gbf_iq_dpc_defect_info_tbl %u-%u_memory = %llu bytes\n",
					tbl_type,
					tbl_idx,
					gBF_IQ_DpcTblInfo[tbl_type][tbl_idx].maxbytelen );
		}
	}
}
#endif //CO_BF_IQ_DBG_TABLE_BYTES


#ifdef CO_BF_IQ_DPC_DEBUG
/** @brief			BF_IQ_Dpc_Tbl_Dump
 *	@param[in]		e_target	Calibration target(view/movie/capture)
 *	@return			None
 *	@note			None
 *	@attention		for FJ Debug Only
 */
VOID BF_IQ_Dpc_Tbl_Dump( const E_IQ_CALIB_TGT e_calib_tgt, const UINT32 tbl_idx )
{
	UINT32 ui_field_idx;
	T_BF_IQ_DPC_TBL_INFO* tbl_info = BF_IQ_Dpc_Tbl_Get_Info( e_calib_tgt, tbl_idx );
	
	if( tbl_info == NULL ) {
		return;
	}

	if( !tbl_info->valid ) {
		BF_Debug_Print_Information(( "*** DPC table is not valid.\n" ));
		return;
	}

	for( ui_field_idx = 0; ui_field_idx < D_BF_IQ_DPC_MAX_FIELDS; ui_field_idx++ ) {
		BF_Debug_Print_Information(( "*** field: %u\n", ui_field_idx ));
		if( tbl_info->addr_by_fields[ui_field_idx] == NULL ) {
			BF_Debug_Print_Information(( "   null\n" ));
			continue;
		}
		iqsub_dpc_tbl_dump(
				tbl_info->addr_by_fields[ui_field_idx],
				tbl_info->bytelen_by_fields[ui_field_idx],
				NULL,
				0 );
	}
}


/** @brief			iqsub_dpc_tbl_dump
 *	@param[in]		pv_defect_info_tbl			defect tbl top address
 *	@param[in]		ui_defect_info_tbl_byte		defect tbl size(byte)
 *	@param[in]		pv_bayer_addr				bayer top address when created defect tbl
 *	@param[in]		ui_columns					columns(horizontal pixels) of bayer data
 *	@return			None
 *	@note			None
 *	@attention		for FJ Debug Only
 */
VOID iqsub_dpc_tbl_dump( VOID* pv_defect_info_tbl, UINT32 ui_defect_info_tbl_byte, VOID* pv_bayer_addr, UINT32 ui_columns )
{
	UINT32 i_loop;
	USHORT* pus_defect_info_tbl = (USHORT *)pv_defect_info_tbl;
	U_BF_IQ_DPC_TBL_DATA* pt_defect_info_tbl;
	UINT32 ui_cur_pixel_val = 0;
	UINT32 ui_cur_row = 0;
	UINT32 ui_cur_pixel_count;
	USHORT* pus_bayer16_addr = pv_bayer_addr;


	BF_Debug_Print_Information(( "tbl_idx, format_id, data_type_id, ui_addr, pixel_val, tbl_dump\n" ));
	for( i_loop = 0; i_loop < (ui_defect_info_tbl_byte / D_BF_IQ_DPC_TBL_SIZEOF_DEFECT_INFO_DATA); i_loop++ ) {
		pt_defect_info_tbl = (U_BF_IQ_DPC_TBL_DATA *)&(pus_defect_info_tbl[i_loop]);

		if( (pt_defect_info_tbl->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_OTHER_DATA)
		 && (pt_defect_info_tbl->rel.ui_data_type_id == D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE)
		 ) {
			ui_cur_row = pt_defect_info_tbl->rel.ui_addr;
			ui_cur_pixel_val = 0;
			BF_Debug_Print_Information(( "\n" ));
		}

		if( pt_defect_info_tbl->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_REL ) {
			ui_cur_pixel_count = ui_cur_row * ui_columns + pt_defect_info_tbl->rel.ui_addr;
			if( pus_bayer16_addr != NULL ) {
				ui_cur_pixel_val = pus_bayer16_addr[ui_cur_pixel_count];
			}
		}

		BF_Debug_Print_Information(( "%5u, %u, %u, %4u, %4u, 0x%04x\n", 
				i_loop,
				pt_defect_info_tbl->rel.ui_format_id,
				pt_defect_info_tbl->rel.ui_data_type_id,
				pt_defect_info_tbl->rel.ui_addr,
				ui_cur_pixel_val,
				pus_defect_info_tbl[i_loop]
				));
	}
}
#endif



