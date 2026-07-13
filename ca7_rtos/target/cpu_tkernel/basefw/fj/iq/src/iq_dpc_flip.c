/**
 * @file		iq_dpc_flip.c
 * @brief		Badpix table flip.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// IQ header
#include "iq_dpc_flip.h"
#include "iq_dpc_tbl.h"
#include "iq_dpc_tbl_dec.h"
#include "iq_dpc_custom.h"

// FJ header
#include "fj_iq.h"

// Utility header
#include "debug.h"

// Driver header
#include "ddim_typedef.h"

// STD-C header
#include <stdlib.h>


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

//#define CO_IQ_DPC_FLIP_DEBUG_PRINT


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

// T_BF_IQ_DPC_FLIP_POS
typedef struct {
	FJ_IQ_BADPIX_POS				(*pos_tbl)[D_BF_IQ_DPC_MAX_PIXELS + D_BF_IQ_DPC_MAX_LINES];
	UINT32							pos_maxnum;
	UINT32							pos_num;
} T_BF_IQ_DPC_FLIP_POS;

// T_BF_IQ_DPC_FLIP_PARAM
typedef struct {
	T_BF_IQ_DPC_TBL_INFO*			tbl_info;
	const FJ_IQ_BADPIX_FLIP_INFO*	flip_info;
	T_BF_IQ_DPC_FLIP_POS			pos[D_BF_IQ_DPC_MAX_FIELDS];
	UINT32							field_idx;	// PRO intelace field index. 0:Even 1:Odd
} T_BF_IQ_DPC_FLIP_PARAM;


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/

static FJ_ERR_CODE bf_iq_dpc_flip_dpc_tbl_to_pos_tbl( T_BF_IQ_DPC_FLIP_PARAM* const flip_param )
{
	T_BF_IQ_DPC_FLIP_POS*		pos;
	const U_BF_IQ_DPC_TBL_DATA*	dpc_tbl_data;
	const U_BF_IQ_DPC_TBL_DATA*	dpc_tbl_data_last;
	UINT32						x_pos = 0;
	UINT32						y_pos = 0;
	UINT32						x_sta = flip_param->tbl_info->sta_x;
	UINT32						y_sta = flip_param->tbl_info->sta_y /2;	// Interlaced.

	pos = &flip_param->pos[flip_param->field_idx];

	pos->pos_num = 0;
	dpc_tbl_data = flip_param->tbl_info->addr_by_fields[flip_param->field_idx];
	dpc_tbl_data_last = dpc_tbl_data
					  + flip_param->tbl_info->bytelen_by_fields[flip_param->field_idx] / sizeof(dpc_tbl_data[0]);

	while( dpc_tbl_data < dpc_tbl_data_last ) {
		if( pos->pos_num >= pos->pos_maxnum ) {
			break;
		}
		if( dpc_tbl_data->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_ABS ) {
			dpc_tbl_data++;
			x_pos = dpc_tbl_data->abs.ui_addr;
			dpc_tbl_data++;
			(*pos->pos_tbl)[pos->pos_num].x_pos = x_pos - x_sta;
			(*pos->pos_tbl)[pos->pos_num].y_pos = y_pos - y_sta;
			pos->pos_num++;
			continue;
		}
		if( dpc_tbl_data->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_REL ) {
			x_pos += dpc_tbl_data->rel.ui_addr;
			dpc_tbl_data++;
			(*pos->pos_tbl)[pos->pos_num].x_pos = x_pos - x_sta;
			(*pos->pos_tbl)[pos->pos_num].y_pos = y_pos - y_sta;
			pos->pos_num++;
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
			// End of Data reached.
			return FJ_ERR_OK;
		}
	}

#ifdef CO_IQ_DPC_FLIP_DEBUG_PRINT
	printf( "%s(): too short\n", __func__ );
#endif

	// End of data not found.
	return FJ_ERR_IQ_FILE_TOO_SHORT;
}

static INT32 bf_iq_dpc_flip_qsort_cb( const VOID *a, const VOID *b )
{
	const FJ_IQ_BADPIX_POS*	pix_pos_a = (const FJ_IQ_BADPIX_POS*)a;
	const FJ_IQ_BADPIX_POS*	pix_pos_b = (const FJ_IQ_BADPIX_POS*)b;

	if( pix_pos_a->y_pos < pix_pos_b->y_pos ) {
		return -1;
	}
	if( pix_pos_a->y_pos > pix_pos_b->y_pos ) {
		return 1;
	}

	if( pix_pos_a->x_pos < pix_pos_b->x_pos ) {
		return -1;
	}
	if( pix_pos_a->x_pos > pix_pos_b->x_pos ) {
		return 1;
	}

	return 0;
}

static FJ_ERR_CODE bf_iq_dpc_flip_vflip_pos_tbl( T_BF_IQ_DPC_FLIP_PARAM* const flip_param )
{
	UINT32						loopcnt;
	UINT32						lines;
	const T_BF_IQ_DPC_FLIP_POS*	pos;

	pos = &flip_param->pos[flip_param->field_idx];
	lines = flip_param->tbl_info->lines /2;		// Set interlaced lines.
	lines -= 1;									// Start number is 0.

	for( loopcnt = 0; loopcnt < pos->pos_num; loopcnt++ ) {
#ifdef CO_IQ_DPC_FLIP_DEBUG_PRINT
		printf( "Y y_pos=%u lines=%u", (*pos->pos_tbl)[loopcnt].y_pos, lines );
#endif
		(*pos->pos_tbl)[loopcnt].y_pos = lines - (*pos->pos_tbl)[loopcnt].y_pos;
#ifdef CO_IQ_DPC_FLIP_DEBUG_PRINT
		printf( " => %u\n", (*pos->pos_tbl)[loopcnt].y_pos );
#endif
	}

	qsort( pos->pos_tbl, pos->pos_num, sizeof((*pos->pos_tbl)[0]), bf_iq_dpc_flip_qsort_cb );

	return FJ_ERR_OK;
}

static FJ_ERR_CODE bf_iq_dpc_flip_pos_tbl_to_dpc_tbl( T_BF_IQ_DPC_FLIP_PARAM* const flip_param )
{
	UINT32							loopcnt;
	UINT32							sta_x;
	UINT32							sta_y;
	const T_BF_IQ_DPC_FLIP_POS*		pos;
	T_BF_IQ_DPC_TBL_DEC_PARAM		decorder;
	FJ_ERR_CODE						fj_ercd;

	flip_param->tbl_info->addr_by_fields[flip_param->field_idx] =
		&flip_param->tbl_info->top_addr[flip_param->tbl_info->bytelen / sizeof(flip_param->tbl_info->top_addr[0])];
	flip_param->tbl_info->bytelen_by_fields[flip_param->field_idx] =
		0ULL;

	fj_ercd = BF_IQ_Dpc_Tbl_Dec_Init( &decorder,
									  flip_param->tbl_info->addr_by_fields[flip_param->field_idx],
									  (flip_param->tbl_info->maxbytelen - flip_param->tbl_info->bytelen)
									  );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}


	pos = &flip_param->pos[D_BF_IQ_DPC_MAX_FIELDS -1 - flip_param->field_idx];	// Reverse Even and odd.
	sta_x = flip_param->tbl_info->sta_x;
	sta_y = flip_param->tbl_info->sta_y /2;		// Set interlaced sta_y.

	for( loopcnt = 0; loopcnt < pos->pos_num; loopcnt++ ) {
		fj_ercd = BF_IQ_Dpc_Tbl_Dec_Add_Pixel( &decorder,
											   (*pos->pos_tbl)[loopcnt].x_pos + sta_x,
											   (*pos->pos_tbl)[loopcnt].y_pos + sta_y
											   );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}
	}

	fj_ercd = BF_IQ_Dpc_Tbl_Dec_Finish( &decorder );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	flip_param->tbl_info->bytelen_by_fields[flip_param->field_idx] = BF_IQ_Dpc_Tbl_Dec_Get_Bytelen( &decorder );
	flip_param->tbl_info->bytelen += BF_IQ_Dpc_Tbl_Dec_Get_BytelenAlign( &decorder );
	flip_param->tbl_info->pixel_num += BF_IQ_Dpc_Tbl_Dec_Get_Pixnum( &decorder );

	return FJ_ERR_OK;
}

static FJ_ERR_CODE bf_iq_dpc_flip_conv_one( T_BF_IQ_DPC_FLIP_PARAM* const flip_param )
{
	static FJ_IQ_BADPIX_POS	pos_tbl_even[D_BF_IQ_DPC_MAX_PIXELS + D_BF_IQ_DPC_MAX_LINES];
	static FJ_IQ_BADPIX_POS	pos_tbl_odd[D_BF_IQ_DPC_MAX_PIXELS + D_BF_IQ_DPC_MAX_LINES];
	FJ_ERR_CODE				fj_ercd;

	flip_param->pos[0].pos_tbl = &pos_tbl_even;
	flip_param->pos[0].pos_maxnum = sizeof(pos_tbl_even) / sizeof(pos_tbl_even[0]);
	flip_param->pos[1].pos_tbl = &pos_tbl_odd;
	flip_param->pos[1].pos_maxnum = sizeof(pos_tbl_odd) / sizeof(pos_tbl_odd[0]);

	//
	// Convert dpc table -> pos_tbl and flip vertical.
	//
	for( flip_param->field_idx = 0; flip_param->field_idx < flip_param->tbl_info->field_num; flip_param->field_idx++ ) {
		fj_ercd = bf_iq_dpc_flip_dpc_tbl_to_pos_tbl( flip_param );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}
		fj_ercd = bf_iq_dpc_flip_vflip_pos_tbl( flip_param );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}
	}

	//
	// Convert pos_tbl -> dpc table.
	//
	flip_param->tbl_info->valid = 0;
	flip_param->tbl_info->bytelen = 0ULL;
	flip_param->tbl_info->pixel_num = 0;
	flip_param->tbl_info->sta_x = flip_param->flip_info->flip_pos_dst.x_pos;
	flip_param->tbl_info->sta_y = flip_param->flip_info->flip_pos_dst.y_pos;
	for( flip_param->field_idx = 0; flip_param->field_idx < flip_param->tbl_info->field_num; flip_param->field_idx++ ) {
		fj_ercd = bf_iq_dpc_flip_pos_tbl_to_dpc_tbl( flip_param );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}
	}

	flip_param->tbl_info->valid = 1;

	return FJ_ERR_OK;
}


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

FJ_ERR_CODE BF_IQ_Dpc_Flip_Run_Vertical( const E_IQ_CALIB_TGT target, const FJ_IQ_BADPIX_FLIP_INFO* const flip_info )
{
	UINT32					tbl_idx;
	T_BF_IQ_DPC_FLIP_PARAM	flip_param;
	FJ_ERR_CODE				fj_ercd;

	flip_param.flip_info = flip_info;

	for( tbl_idx = 0; tbl_idx < D_BF_IQ_DPC_TBL_IDX_MAX; tbl_idx++ ) {

#ifdef CO_IQ_DPC_FLIP_DEBUG_PRINT
	printf( "%s(): tbl_idx = %u\n", __func__, tbl_idx );
#endif
		flip_param.tbl_info = BF_IQ_Dpc_Tbl_Get_Info( target, tbl_idx );
		if( flip_param.tbl_info == NULL ) {
			return FJ_ERR_IQ_TBL_INVALID;
		}
		if( flip_param.tbl_info->valid == 0 ) {
			continue;
		}

		fj_ercd = bf_iq_dpc_flip_conv_one( &flip_param );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}
	}

	return FJ_ERR_OK;
}

