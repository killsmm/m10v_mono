/**
 * @file		iq_dpc_tbl_enc.c
 * @brief		Badpix table encoder.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// IQ header
#include "iq_dpc_tbl_enc.h"
#include "iq_common.h"

// FJ header
#include "fj_iq.h"

// Utility header
#include "debug.h"

// Driver header
#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

//#define D_BF_IQ_DPC_TBL_ENC_DBG_PRINT


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
/* Local Method Definition														*/
/*==============================================================================*/

static FJ_ERR_CODE bf_iq_dpc_tbl_enc_1field( T_BF_IQ_DPC_TBL_ENC_PARAM* const param, T_BF_IQ_DPC_TBL_INFO* const tbl_info, const UINT32 field_idx )
{
	FJ_IQ_BADPIX_POS*			pos_tbl;
	const U_BF_IQ_DPC_TBL_DATA*	dpc_tbl_data;
	const U_BF_IQ_DPC_TBL_DATA*	dpc_tbl_data_last;
	UINT32						x_pos = 0;
	UINT32						y_pos = 0;
	UINT32						x_sta = 0;
	UINT32						y_sta = 0;
	UINT32						pos_num;

	if( param->flg_declease_sta != FALSE ) {
		x_sta = tbl_info->sta_x;
		y_sta = tbl_info->sta_y /2;	// Interlaced.
	}

	pos_num = 0;

	pos_tbl = &(param->pos_tbl_top[param->pos_num_all]);
	param->pos_tbl[field_idx] = pos_tbl;

	dpc_tbl_data = tbl_info->addr_by_fields[field_idx];
	dpc_tbl_data_last = dpc_tbl_data
					  + tbl_info->bytelen_by_fields[field_idx] / sizeof(dpc_tbl_data[0]);

	while( dpc_tbl_data < dpc_tbl_data_last ) {
		if( pos_num >= (param->pos_maxnum - param->pos_num_all) ) {
			break;
		}
		if( dpc_tbl_data->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_ABS ) {
			dpc_tbl_data++;
			x_pos = dpc_tbl_data->abs.ui_addr;
			dpc_tbl_data++;
			pos_tbl[pos_num].x_pos = x_pos - x_sta;
			pos_tbl[pos_num].y_pos = y_pos - y_sta;
			pos_num++;
			continue;
		}
		if( dpc_tbl_data->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_REL ) {
			x_pos += dpc_tbl_data->rel.ui_addr;
			dpc_tbl_data++;
			pos_tbl[pos_num].x_pos = x_pos - x_sta;
			pos_tbl[pos_num].y_pos = y_pos - y_sta;
			pos_num++;
			continue;
		}
		if( (dpc_tbl_data->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_ABS)
		 && (dpc_tbl_data->rel.ui_data_type_id == D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE) ) {
			dpc_tbl_data++;
			y_pos = dpc_tbl_data->abs.ui_addr;
			x_pos = 0;
			dpc_tbl_data++;
			continue;
		}
		if( (dpc_tbl_data->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_REL)
		 && (dpc_tbl_data->rel.ui_data_type_id == D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE) ) {
			y_pos += dpc_tbl_data->rel.ui_addr;
			x_pos = 0;
			dpc_tbl_data++;
			continue;
		}
		if( (dpc_tbl_data->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_OTHER_DATA)
		 && (dpc_tbl_data->rel.ui_data_type_id == D_BF_IQ_DPC_TBL_DTYPE_END_OF_DATA) ) {
			param->pos_num[field_idx] = pos_num;
			param->pos_num_all += pos_num;

			// End of Data reached.
			return FJ_ERR_OK;
		}
	}

#ifdef D_BF_IQ_DPC_TBL_ENC_DBG_PRINT
	printf( "%s(): too short\n", __func__ );
#endif

	// End of data not found.
	return FJ_ERR_IQ_FILE_TOO_SHORT;
}

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

FJ_ERR_CODE BF_IQ_Dpc_Tbl_Enc( T_BF_IQ_DPC_TBL_ENC_PARAM* const param, T_BF_IQ_DPC_TBL_INFO* const tbl_info )
{
	FJ_ERR_CODE	fj_ercd[2];

	param->pos_num_all = 0;

	fj_ercd[0] = bf_iq_dpc_tbl_enc_1field( param, tbl_info, 0 );
	fj_ercd[1] = bf_iq_dpc_tbl_enc_1field( param, tbl_info, 1 );

	if( fj_ercd[0] != FJ_ERR_OK ) {
		return fj_ercd[0];
	}
	if( fj_ercd[1] != FJ_ERR_OK ) {
		return fj_ercd[1];
	}

	return FJ_ERR_OK;
}

