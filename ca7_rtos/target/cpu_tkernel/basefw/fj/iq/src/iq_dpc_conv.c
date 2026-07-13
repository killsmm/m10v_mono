/**
 * @file		iq_dpc_conv.c
 * @brief		Badpix data converter for additional pixel by image sensor.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// FJ header
#include "fj_iq.h"

// IQ header
#include "iq_dpc_conv.h"
#include "iq_dpc_tbl.h"
#include "iq_dpc_custom.h"

#include "iq_calib.h"

#include "iq_common.h"
#include "iq_dcache.h"

// Driver header
#include "ddim_typedef.h"

// STD-C header
#include <stdio.h>


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

//#define CO_BF_IQ_DPC_CONV_DEBUG_PRINT

#define BF_IQ_DPC_CONV_ADD_POS_Y_MAX		(20)

#define BF_IQ_DPC_CONV_POS_INVALID			(0xFFFF)


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

// T_BF_IQ_DPC_CONV_SRC_X
typedef struct {
	const U_BF_IQ_DPC_TBL_DATA*			src_cur_addr;					// Dst address of current X pixel.
	USHORT								src_pos_x_plus_trim;			// Include Htrim
	USHORT								dst_pos_x;						// Exclude Htrim
} T_BF_IQ_DPC_CONV_SRC_X;

// T_BF_IQ_DPC_CONV_SRC_Y
typedef struct {
	const U_BF_IQ_DPC_TBL_DATA*			src_cur_addr;					// Src address of current Y pixel.
	USHORT								src_pos_y_plus_trim;			// Include Vtrim
	USHORT								dst_pos_y;						// Exclude Vtrim
} T_BF_IQ_DPC_CONV_SRC_Y;

// T_BF_IQ_DPC_CONV_BY_COL
typedef struct {
	FJ_IQ_BADPIX_COLUM					even_odd_x;
	T_BF_IQ_DPC_CONV_SRC_X				src_x[BF_IQ_DPC_CONV_ADD_POS_Y_MAX];
	UINT32								src_x_valid_num;
	T_BF_IQ_DPC_CONV_SRC_Y				src_y;
	USHORT								src_cur_start_pos_y_plus_trim;	// Include Vtrim
	USHORT								src_cur_end_pos_y_plus_trim;	// Include Vtrim
	FJ_IQ_BADPIX_POS					src_crop_start_pos_plus_trim;	// Include Htrim/Vtrim
	FJ_IQ_BADPIX_POS					src_crop_end_pos_plus_trim;		// Include Htrim/Vtrim
	FJ_IQ_BADPIX_ADD_PAT				add_pat_intr;
} T_BF_IQ_DPC_CONV_BY_COL;

// T_BF_IQ_DPC_CALC_SUCCESSION_INFO
typedef struct {
	USHORT								x_pos_last;						// Exclude Htrim
	USHORT								y_pos_last;						// Exclude Vtrim
	UINT32								succession_count;
} T_BF_IQ_DPC_CALC_SUCCESSION_INFO;

// T_BF_IQ_DPC_CONV_DST_TBL
typedef struct {
	U_BF_IQ_DPC_TBL_DATA*				addr;
	ULLONG								maxbytelen;
	ULLONG								bytelen;
	UINT32								pixel_num;
	UINT32								pos_x;							// Exclude Htrim
	UINT32								pos_y;							// Exclude Vtrim
	U_BF_IQ_DPC_TBL_DATA*				addr_before[2];					// Even and odd.
	T_BF_IQ_DPC_CALC_SUCCESSION_INFO	succession_info[2];				// Even and odd.
} T_BF_IQ_DPC_CONV_DST_TBL;

// T_BF_IQ_DPC_CONV_PARAM - Convert internal parameter.
typedef struct {
	T_BF_IQ_DPC_CONV_BY_COL				conv_by_col[FJ_IQ_BADPIX_COLUM_MAX];
	const FJ_IQ_BADPIX_CONV_INFO*		conv_info;
	T_BF_IQ_DPC_TBL_INFO*				dst_tbl_info;
	T_BF_IQ_DPC_CONV_DST_TBL			dst_tbl;						// Destination table (Even or Odd)
	const T_BF_IQ_DPC_TBL_INFO*			src_tbl_info;
	FJ_IQ_BADPIX_POS					sen_window_pos_src_intr;
	FJ_IQ_BADPIX_POS					sen_window_pos_dst_intr;
} T_BF_IQ_DPC_CONV_PARAM;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/

static BOOL bf_iq_dpc_conv_forward_dst_x( T_BF_IQ_DPC_CONV_SRC_X* const src_x, const FJ_IQ_BADPIX_ADD_POS* const add_pos, const T_BF_IQ_DPC_CONV_BY_COL* const conv_by_col )
{
	const U_BF_IQ_DPC_TBL_DATA*	tbl_addr;
	USHORT						src_pos_x_in_cycle;
	USHORT						dst_pos_x;
	UINT32						loopcnt;

	if( src_x->src_cur_addr == NULL ) {
		src_x->src_pos_x_plus_trim = BF_IQ_DPC_CONV_POS_INVALID;
		src_x->dst_pos_x = BF_IQ_DPC_CONV_POS_INVALID;
		return FALSE;
	}

	while( 1 ) {
		// Fetch next src_pos_x
#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
		printf( "Src X %u pos plus trim before = %u\n", conv_by_col->even_odd_x, src_x->src_pos_x_plus_trim );
#endif
		for( tbl_addr = src_x->src_cur_addr; tbl_addr != NULL; tbl_addr++ ) {	/* pgr0689 */
			if( tbl_addr->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_ABS ) {
				tbl_addr++;
				src_x->src_pos_x_plus_trim = tbl_addr->abs.ui_addr;
				tbl_addr++;
				src_x->src_cur_addr = tbl_addr;
				break;
			}
			if( tbl_addr->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_REL ) {
				src_x->src_pos_x_plus_trim += tbl_addr->rel.ui_addr;
				tbl_addr++;
				src_x->src_cur_addr = tbl_addr;
				break;
			}
			if( (tbl_addr->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_ABS)
			 && (tbl_addr->rel.ui_data_type_id == D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE) ) {
				// End of Data reached.
				src_x->src_cur_addr = NULL;
				src_x->src_pos_x_plus_trim = BF_IQ_DPC_CONV_POS_INVALID;
				src_x->dst_pos_x = BF_IQ_DPC_CONV_POS_INVALID;
				return FALSE;
			}
			if( (tbl_addr->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_REL)
			 && (tbl_addr->rel.ui_data_type_id == D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE) ) {
				// End of Data reached.
				src_x->src_cur_addr = NULL;
				src_x->src_pos_x_plus_trim = BF_IQ_DPC_CONV_POS_INVALID;
				src_x->dst_pos_x = BF_IQ_DPC_CONV_POS_INVALID;
				return FALSE;
			}
			if( (tbl_addr->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_OTHER_DATA)
			 && (tbl_addr->rel.ui_data_type_id == D_BF_IQ_DPC_TBL_DTYPE_END_OF_DATA) ) {
				// End of Data reached.
				src_x->src_cur_addr = NULL;
				src_x->src_pos_x_plus_trim = BF_IQ_DPC_CONV_POS_INVALID;
				src_x->dst_pos_x = BF_IQ_DPC_CONV_POS_INVALID;
				return FALSE;
			}
		}

#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
		printf( "Src X %u pos = %u crop start = %u crop end = %u\n", conv_by_col->even_odd_x, src_x->src_pos_x_plus_trim, conv_by_col->src_crop_start_pos_plus_trim.x_pos, conv_by_col->src_crop_end_pos_plus_trim.x_pos );
#endif

		// Skip this when less then cropping area.
		if( src_x->src_pos_x_plus_trim < conv_by_col->src_crop_start_pos_plus_trim.x_pos ) {
			continue;
		}
		// Skip this when grather equal cropping area.
		if( src_x->src_pos_x_plus_trim >= conv_by_col->src_crop_end_pos_plus_trim.x_pos ) {
			src_x->src_cur_addr = NULL;
			src_x->src_pos_x_plus_trim = BF_IQ_DPC_CONV_POS_INVALID;
			src_x->dst_pos_x = BF_IQ_DPC_CONV_POS_INVALID;
			return FALSE;
		}


		// Calculate pos_x by additional pattern cycle.
		src_pos_x_in_cycle = (src_x->src_pos_x_plus_trim - conv_by_col->src_crop_start_pos_plus_trim.x_pos) % conv_by_col->add_pat_intr.cycle_rect.width;

#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
		printf( "Src X %u in_cycle = %u add_pos->x_num = %u\n", conv_by_col->even_odd_x, src_pos_x_in_cycle, add_pos->x_num );
#endif
		// Judge additional pixel.
		for( loopcnt = 0; loopcnt < add_pos->x_num; loopcnt++ ) {
#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
			printf( "Src X %u in_cycle = %u %u\n", conv_by_col->even_odd_x, src_pos_x_in_cycle, add_pos->x[loopcnt] );
#endif
			if( src_pos_x_in_cycle == add_pos->x[loopcnt] ) {
				break;
			}
		}
		if( loopcnt >= add_pos->x_num ) {
			// Not additional pixel(This is Skip pixel). Goto next pixel.
			continue;
		}

		// A src_pos_x_plus_trim is additional pixel.
		// Calculate dst_pos_x and return.
		dst_pos_x = (src_x->src_pos_x_plus_trim - conv_by_col->src_crop_start_pos_plus_trim.x_pos) / conv_by_col->add_pat_intr.cycle_rect.width;
		dst_pos_x *= 2;	// X direction interlace to progressive.
		if( dst_pos_x == src_x->dst_pos_x ) {
			continue;
		}

		if( conv_by_col->even_odd_x == FJ_IQ_BADPIX_COLUM_ODD ) {
			dst_pos_x++;
		}
		src_x->dst_pos_x = dst_pos_x;

#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
		printf( "Calc X %u, %u, %u -> %u\n", src_x->src_pos_x_plus_trim, conv_by_col->src_crop_start_pos_plus_trim.x_pos, conv_by_col->add_pat_intr.cycle_rect.width, dst_pos_x );
#endif
		break;
	}

	return TRUE;
}


static BOOL bf_iq_dpc_conv_forward_next_src_line( T_BF_IQ_DPC_CONV_BY_COL* const conv_by_col )
{
	const U_BF_IQ_DPC_TBL_DATA*	tbl_addr;

	for( tbl_addr = conv_by_col->src_y.src_cur_addr; tbl_addr != NULL; tbl_addr++ ) {	/* pgr0689 */
		if( (tbl_addr->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_ABS)
		 && (tbl_addr->rel.ui_data_type_id == D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE) ) {
			tbl_addr++;
			conv_by_col->src_y.src_pos_y_plus_trim = tbl_addr->abs.ui_addr;
			tbl_addr++;
			conv_by_col->src_y.src_cur_addr = tbl_addr;
#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
			printf( "Src Y pos plus trim = %u\n", conv_by_col->src_y.src_pos_y_plus_trim );
#endif
			return TRUE;
		}
		if( (tbl_addr->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_REL)
		 && (tbl_addr->rel.ui_data_type_id == D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE) ) {
			conv_by_col->src_y.src_pos_y_plus_trim += tbl_addr->rel.ui_addr;
			tbl_addr++;
			conv_by_col->src_y.src_cur_addr = tbl_addr;
#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
			printf( "Src Y pos plus trim = %u\n", conv_by_col->src_y.src_pos_y_plus_trim );
#endif
			return TRUE;
		}
		if( (tbl_addr->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_OTHER_DATA)
		 && (tbl_addr->rel.ui_data_type_id == D_BF_IQ_DPC_TBL_DTYPE_END_OF_DATA) ) {
			// End of Data reached.
			conv_by_col->src_y.src_cur_addr = NULL;
			conv_by_col->src_y.src_pos_y_plus_trim = BF_IQ_DPC_CONV_POS_INVALID;
			return FALSE;
		}
	}

	return FALSE;
}




static VOID bf_iq_dpc_conv_validate_dst_tbl_info( T_BF_IQ_DPC_TBL_INFO* const dst_tbl_info, const T_BF_IQ_DPC_CONV_PARAM* const param )
{
	dst_tbl_info->sta_x = param->conv_info->sen_window_pos_dst.x_pos;
	dst_tbl_info->sta_y = param->conv_info->sen_window_pos_dst.y_pos;
	dst_tbl_info->width = 0;
	dst_tbl_info->lines = 0;
	dst_tbl_info->field_num = D_BF_IQ_DPC_MAX_FIELDS;
	dst_tbl_info->valid = TRUE;
}


static VOID bf_iq_dpc_conv_update_dst_tbl_info( const USHORT odd_even, T_BF_IQ_DPC_TBL_INFO* const dst_tbl_info, const T_BF_IQ_DPC_CONV_DST_TBL* const dst_tbl )
{
	dst_tbl_info->addr_by_fields[odd_even] = dst_tbl_info->top_addr + (dst_tbl_info->bytelen / sizeof(U_BF_IQ_DPC_TBL_DATA));
	dst_tbl_info->bytelen_by_fields[odd_even] = M_IQ_ROUNDUP_8( dst_tbl->bytelen );
	dst_tbl_info->bytelen += M_IQ_ROUNDUP_8( dst_tbl->bytelen );
	dst_tbl_info->pixel_num += dst_tbl->pixel_num;
}


static VOID bf_iq_dpc_conv_init_dst_tbl_info( T_BF_IQ_DPC_TBL_INFO* const dst_tbl_info )
{
	UINT32	loopcnt;

	for( loopcnt = 0; loopcnt < D_BF_IQ_DPC_MAX_FIELDS; loopcnt++ ) {
		dst_tbl_info->addr_by_fields[loopcnt] = NULL;
		dst_tbl_info->bytelen_by_fields[loopcnt] = 0;
	}
	dst_tbl_info->bytelen = 0;
	dst_tbl_info->pixel_num = 0;
	dst_tbl_info->sta_x = 0;
	dst_tbl_info->sta_y = 0;
	dst_tbl_info->width = 0;
	dst_tbl_info->lines = 0;
	dst_tbl_info->field_num = 0;
	dst_tbl_info->valid = FALSE;
}




static VOID bf_iq_dpc_conv_init_dst_tbl( const USHORT odd_even, T_BF_IQ_DPC_CONV_DST_TBL* const dst_tbl, const T_BF_IQ_DPC_TBL_INFO* const dst_tbl_info )
{
	dst_tbl->addr = dst_tbl_info->top_addr + (dst_tbl_info->bytelen / sizeof(U_BF_IQ_DPC_TBL_DATA));
	dst_tbl->maxbytelen = dst_tbl_info->maxbytelen - dst_tbl_info->bytelen;
	dst_tbl->bytelen = 0;
	dst_tbl->pixel_num = 0;
	dst_tbl->pos_x = 0;
	dst_tbl->pos_y = 0;
	dst_tbl->succession_info[0].x_pos_last = BF_IQ_DPC_CONV_POS_INVALID;
	dst_tbl->succession_info[0].y_pos_last = BF_IQ_DPC_CONV_POS_INVALID;
	dst_tbl->succession_info[0].succession_count = 0;
	dst_tbl->succession_info[1].x_pos_last = BF_IQ_DPC_CONV_POS_INVALID;
	dst_tbl->succession_info[1].y_pos_last = BF_IQ_DPC_CONV_POS_INVALID;
	dst_tbl->succession_info[1].succession_count = 0;
}


static UINT32 bf_iq_dpc_dbg_conv_calc_succession_count( T_BF_IQ_DPC_CONV_DST_TBL* const dst_tbl, const UINT32 even_or_odd, const UCHAR x_pos, const USHORT y_pos )
{
	T_BF_IQ_DPC_CALC_SUCCESSION_INFO*	succession_info;

	succession_info = &dst_tbl->succession_info[even_or_odd];
	

	if( succession_info->y_pos_last == y_pos ) {
		if( (succession_info->x_pos_last +2) == x_pos ) {
			succession_info->succession_count++;
		}
		else {
			succession_info->succession_count = 1;
		}
	}
	else {
		succession_info->succession_count = 1;
	}

	succession_info->x_pos_last = x_pos;
	succession_info->y_pos_last = y_pos;

	return succession_info->succession_count;
}


static FJ_ERR_CODE bf_iq_dpc_conv_set_dst_pixel( T_BF_IQ_DPC_CONV_DST_TBL* const dst_tbl, const FJ_IQ_BADPIX_POS* const sen_window_pos_dst_intr, const USHORT pos_x, const USHORT pos_y )
{
	UINT32					ui_data_type_id;
	UINT32					ui_before_data_type_id;
	USHORT					pos_x_and_htrim;
	USHORT					pos_y_and_vtrim;
	U_BF_IQ_DPC_TBL_DATA*	addr_before = NULL;
	UINT32					even_or_odd;

	pos_x_and_htrim = pos_x + sen_window_pos_dst_intr->x_pos;
	pos_y_and_vtrim = pos_y + sen_window_pos_dst_intr->y_pos;
	even_or_odd = pos_x_and_htrim %2;

#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
	printf( "Add dst %u(%u), %u(%u)\n", pos_x, pos_x_and_htrim, pos_y, pos_y_and_vtrim );
#endif

	//
	// Set Vertical address infomation if needed.
	//
	if( pos_y_and_vtrim != dst_tbl->pos_y ) {
		if( (pos_y_and_vtrim - dst_tbl->pos_y) > D_BF_IQ_DPC_TBL_REL_ADDR_MAX ) {
			// ABS addressing.
			if( (dst_tbl->maxbytelen - dst_tbl->bytelen) < (3 * sizeof(U_BF_IQ_DPC_TBL_DATA)) ) {
				return FJ_ERR_IQ_TBL_FULL;
			}
			dst_tbl->addr->rel.ui_addr = 0;	// dummy
			dst_tbl->addr->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE;
			dst_tbl->addr->rel.ui_format_id = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_ABS;
			dst_tbl->addr++;
			dst_tbl->addr->abs.ui_addr = pos_y_and_vtrim;
#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
			printf( "ui_addr abs = %u\n", dst_tbl->addr->abs.ui_addr );
#endif
			dst_tbl->addr++;
			dst_tbl->bytelen += sizeof(U_BF_IQ_DPC_TBL_DATA) *2;
		}
		else {
			// REL addressing
			if( (dst_tbl->maxbytelen - dst_tbl->bytelen) < (2 * sizeof(U_BF_IQ_DPC_TBL_DATA)) ) {
				return FJ_ERR_IQ_TBL_FULL;
			}
			dst_tbl->addr->rel.ui_addr = pos_y_and_vtrim - dst_tbl->pos_y;
			dst_tbl->addr->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE;
			dst_tbl->addr->rel.ui_format_id = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_REL;
#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
			printf( "ui_addr rel = %u\n", dst_tbl->addr->rel.ui_addr );
#endif
			dst_tbl->addr++;
			dst_tbl->bytelen += sizeof(U_BF_IQ_DPC_TBL_DATA);
		}
		dst_tbl->pos_x = 0;
		dst_tbl->pos_y = pos_y_and_vtrim;
	}



	//
	// Set Horizontal address infomation if needed.
	//
	switch( bf_iq_dpc_dbg_conv_calc_succession_count( dst_tbl, even_or_odd, pos_x, pos_y ) ) {
		case 0:
			return FJ_ERR_NG;	// Internal error.
		case 1:
			// Badpix indivisual(1st pixel)
			addr_before = NULL;
			ui_data_type_id        = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_SINGLE;
			ui_before_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_BEFORE;	// dummy
			break;
		case 2:
			// Badpix two continuously(2nd pixel)
			addr_before = dst_tbl->addr_before[even_or_odd];
			ui_data_type_id        = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER;
			ui_before_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_BEFORE;
			break;
		case 3:
			// Badpix tree continuously(3rd pixel)
			addr_before = dst_tbl->addr_before[even_or_odd];
			ui_data_type_id        = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER;
			ui_before_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_CENTER;
			break;
		case 4:
			// Badpix four continuously(4th pixel)
			addr_before = dst_tbl->addr_before[even_or_odd];
			ui_data_type_id        = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER;
			ui_before_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_CENTER;
			break;
		default:
			// Badpix over five continuously(4th pixel)
			addr_before = dst_tbl->addr_before[even_or_odd];
			ui_data_type_id        = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER;
			ui_before_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_CENTER;
			break;
	}

	if( pos_x_and_htrim != dst_tbl->pos_x ) {
		dst_tbl->addr_before[even_or_odd] = dst_tbl->addr;

		if( (pos_x_and_htrim - dst_tbl->pos_x) > D_BF_IQ_DPC_TBL_REL_ADDR_MAX ) {
			// ABS addressing.
			if( (dst_tbl->maxbytelen - dst_tbl->bytelen) < (3 * sizeof(U_BF_IQ_DPC_TBL_DATA)) ) {
				return FJ_ERR_IQ_TBL_FULL;
			}
			dst_tbl->addr->rel.ui_addr = 0;	// dummy
			dst_tbl->addr->rel.ui_data_type_id = ui_data_type_id;
			dst_tbl->addr->rel.ui_format_id = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_ABS;
			dst_tbl->addr++;
			dst_tbl->addr->abs.ui_addr = pos_x_and_htrim;
			dst_tbl->addr++;
			dst_tbl->pos_x = pos_x_and_htrim;
			dst_tbl->bytelen += sizeof(U_BF_IQ_DPC_TBL_DATA) *2;
		}
		else {
			// REL addressing
			if( (dst_tbl->maxbytelen - dst_tbl->bytelen) < (2 * sizeof(U_BF_IQ_DPC_TBL_DATA)) ) {
				return FJ_ERR_IQ_TBL_FULL;
			}
			dst_tbl->addr->rel.ui_addr = pos_x_and_htrim - dst_tbl->pos_x;
			dst_tbl->addr->rel.ui_data_type_id = ui_data_type_id;
			dst_tbl->addr->rel.ui_format_id = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_REL;
			dst_tbl->addr++;
			dst_tbl->pos_x = pos_x_and_htrim;
			dst_tbl->bytelen += sizeof(U_BF_IQ_DPC_TBL_DATA);
		}

		dst_tbl->pixel_num++;

		// Replace before data type id.
		if( addr_before != NULL ) {
			addr_before->rel.ui_data_type_id = ui_before_data_type_id;
		}
	}

	return FJ_ERR_OK;
}


static VOID bf_iq_dpc_conv_set_dst_eod( T_BF_IQ_DPC_CONV_DST_TBL* const dst_tbl )
{
	dst_tbl->addr->rel.ui_addr = 0;	// dummy
	dst_tbl->addr->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_END_OF_DATA;
	dst_tbl->addr->rel.ui_format_id = D_BF_IQ_DPC_TBL_FORMAT_OTHER_DATA;
	dst_tbl->addr++;
	dst_tbl->bytelen += sizeof(U_BF_IQ_DPC_TBL_DATA);
}




static VOID bf_iq_dpc_conv_init_dst_pos_x( T_BF_IQ_DPC_CONV_BY_COL* const conv_by_col, const T_BF_IQ_DPC_CONV_PARAM* const param )
{
	UINT32	loopcnt;

	for( loopcnt = 0; loopcnt < conv_by_col->add_pat_intr.add_pos_num; loopcnt++ ) {
		(VOID)bf_iq_dpc_conv_forward_dst_x(
				&conv_by_col->src_x[loopcnt],
				&conv_by_col->add_pat_intr.add_pos[loopcnt],
				conv_by_col );
	}
}


static USHORT bf_iq_dpc_conv_calc_dst_pos_x_min( T_BF_IQ_DPC_CONV_BY_COL* const conv_by_col, const T_BF_IQ_DPC_CONV_PARAM* const param )
{
	UINT32	loopcnt;
	USHORT	dst_pos_x_min = BF_IQ_DPC_CONV_POS_INVALID;

	//
	// Get minimum X pixel position.
	//
	for( loopcnt = 0; loopcnt < conv_by_col->add_pat_intr.add_pos_num; loopcnt++ ) {
		dst_pos_x_min = M_IQ_MIN( dst_pos_x_min, conv_by_col->src_x[loopcnt].dst_pos_x );
	}
	if( dst_pos_x_min == BF_IQ_DPC_CONV_POS_INVALID ) {
		return dst_pos_x_min;
	}

	//
	// Forward "dst_pos_x" when current X pixel position equal "dst_pos_x_min".
	//
	for( loopcnt = 0; loopcnt < conv_by_col->add_pat_intr.add_pos_num; loopcnt++ ) {
		if( dst_pos_x_min == conv_by_col->src_x[loopcnt].dst_pos_x ) {
			(VOID)bf_iq_dpc_conv_forward_dst_x(
					&conv_by_col->src_x[loopcnt],
					&conv_by_col->add_pat_intr.add_pos[loopcnt],
					conv_by_col );
		}
	}

	return dst_pos_x_min;
}


static FJ_ERR_CODE bf_iq_dpc_conv_col( T_BF_IQ_DPC_CONV_PARAM* const param )
{
	FJ_IQ_BADPIX_COLUM	min_even_odd_x;
	FJ_ERR_CODE			fj_ercd = FJ_ERR_OK;
	USHORT				dst_pos_x[FJ_IQ_BADPIX_COLUM_MAX];

	bf_iq_dpc_conv_init_dst_pos_x( &param->conv_by_col[FJ_IQ_BADPIX_COLUM_EVEN], param );
	bf_iq_dpc_conv_init_dst_pos_x( &param->conv_by_col[FJ_IQ_BADPIX_COLUM_ODD ], param );

	dst_pos_x[FJ_IQ_BADPIX_COLUM_EVEN] = bf_iq_dpc_conv_calc_dst_pos_x_min( &param->conv_by_col[FJ_IQ_BADPIX_COLUM_EVEN], param );
	dst_pos_x[FJ_IQ_BADPIX_COLUM_ODD ] = bf_iq_dpc_conv_calc_dst_pos_x_min( &param->conv_by_col[FJ_IQ_BADPIX_COLUM_ODD ], param );

	while( (dst_pos_x[FJ_IQ_BADPIX_COLUM_EVEN] != BF_IQ_DPC_CONV_POS_INVALID)
	    || (dst_pos_x[FJ_IQ_BADPIX_COLUM_ODD ] != BF_IQ_DPC_CONV_POS_INVALID) ) {
		min_even_odd_x = FJ_IQ_BADPIX_COLUM_MAX;
		if( dst_pos_x[FJ_IQ_BADPIX_COLUM_EVEN] == BF_IQ_DPC_CONV_POS_INVALID ) {
			min_even_odd_x = FJ_IQ_BADPIX_COLUM_ODD;
		}
		if( dst_pos_x[FJ_IQ_BADPIX_COLUM_ODD ] == BF_IQ_DPC_CONV_POS_INVALID ) {
			min_even_odd_x = FJ_IQ_BADPIX_COLUM_EVEN;
		}
		if( dst_pos_x[FJ_IQ_BADPIX_COLUM_EVEN] < dst_pos_x[FJ_IQ_BADPIX_COLUM_ODD ] ) {
			min_even_odd_x = FJ_IQ_BADPIX_COLUM_EVEN;
		}
		else {
			min_even_odd_x = FJ_IQ_BADPIX_COLUM_ODD;
		}

		if( min_even_odd_x == FJ_IQ_BADPIX_COLUM_MAX ) {
			continue;
		}

		fj_ercd = bf_iq_dpc_conv_set_dst_pixel(
				&param->dst_tbl,
				&param->sen_window_pos_dst_intr,
				dst_pos_x[min_even_odd_x],
				param->conv_by_col[min_even_odd_x].src_y.dst_pos_y );
		if( fj_ercd != FJ_ERR_OK ) {
			break;
		}

		dst_pos_x[min_even_odd_x] = bf_iq_dpc_conv_calc_dst_pos_x_min( &param->conv_by_col[min_even_odd_x], param );
	}

	return fj_ercd;
}




static VOID bf_iq_dpc_conv_set_src_x( T_BF_IQ_DPC_CONV_SRC_X* const src_x, const U_BF_IQ_DPC_TBL_DATA* const addr )
{
	src_x->src_cur_addr = addr;
	src_x->src_pos_x_plus_trim = 0;
	src_x->dst_pos_x = BF_IQ_DPC_CONV_POS_INVALID;
}


static VOID bf_iq_dpc_conv_incr_line_of_src_x( T_BF_IQ_DPC_CONV_BY_COL* const conv_by_col )
{
	UINT32						loopcnt;

	conv_by_col->src_x_valid_num = 0;
	for( loopcnt = 0; loopcnt < BF_IQ_DPC_CONV_ADD_POS_Y_MAX; loopcnt++ ) {
		bf_iq_dpc_conv_set_src_x( &conv_by_col->src_x[loopcnt], NULL );
	}

	if( conv_by_col->src_y.src_cur_addr == NULL ) {
		return;
	}



	if( conv_by_col->src_y.src_pos_y_plus_trim < conv_by_col->src_cur_start_pos_y_plus_trim ) {
		if( bf_iq_dpc_conv_forward_next_src_line( conv_by_col ) == FALSE ) {
			return;
		}
	}

	while( 1 ) {
#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
		printf( "src_y.src_pos_y_plus_trim=%u cur_end_pos_y_plus_trim=%u\n", conv_by_col->src_y.src_pos_y_plus_trim, conv_by_col->src_cur_end_pos_y_plus_trim );
#endif
		//
		// Skip when table line No is out of area.
		//
		if( conv_by_col->src_y.src_pos_y_plus_trim >= conv_by_col->src_cur_end_pos_y_plus_trim ) {
			return;
		}


		for( loopcnt = 0; loopcnt < conv_by_col->add_pat_intr.add_pos_num; loopcnt++ ) {
			if( (conv_by_col->src_cur_start_pos_y_plus_trim + conv_by_col->add_pat_intr.add_pos[loopcnt].y) == conv_by_col->src_y.src_pos_y_plus_trim ) {
#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
				printf( "add_pos[%u].y = %u\n", loopcnt, conv_by_col->src_y.src_pos_y_plus_trim );
#endif
				bf_iq_dpc_conv_set_src_x( &conv_by_col->src_x[loopcnt], conv_by_col->src_y.src_cur_addr );
				conv_by_col->src_x_valid_num++;
				break;
			}
		}

		if( bf_iq_dpc_conv_forward_next_src_line( conv_by_col ) == FALSE ) {
			return;
		}
	}
}


static VOID bf_iq_dpc_conv_incr_dst_pos_y( T_BF_IQ_DPC_CONV_BY_COL* const conv_by_col )
{
	conv_by_col->src_cur_start_pos_y_plus_trim += conv_by_col->add_pat_intr.cycle_rect.lines;
	conv_by_col->src_cur_end_pos_y_plus_trim   += conv_by_col->add_pat_intr.cycle_rect.lines;
	conv_by_col->src_y.dst_pos_y++;
#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
	printf( "%u Incr Y %u\n", conv_by_col->even_odd_x, conv_by_col->src_y.dst_pos_y );
#endif
}


static VOID bf_iq_dpc_conv_set_crop_start_pos( T_BF_IQ_DPC_CONV_BY_COL* const conv_by_col, const T_BF_IQ_DPC_CONV_PARAM* const param )
{
	conv_by_col->src_crop_start_pos_plus_trim.x_pos = param->sen_window_pos_src_intr.x_pos
													+ conv_by_col->add_pat_intr.cycle_start_pos.x_pos
													+ param->conv_info->crop_pos_src.x_pos;
	conv_by_col->src_crop_start_pos_plus_trim.y_pos = param->sen_window_pos_src_intr.y_pos
													+ conv_by_col->add_pat_intr.cycle_start_pos.y_pos
													+ param->conv_info->crop_pos_src.y_pos /2;
	conv_by_col->src_crop_end_pos_plus_trim.x_pos	= conv_by_col->src_crop_start_pos_plus_trim.x_pos
													+ param->conv_info->crop_rect_src.width;
	conv_by_col->src_crop_end_pos_plus_trim.y_pos	= conv_by_col->src_crop_start_pos_plus_trim.y_pos
													+ param->conv_info->crop_rect_src.lines /2;
#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
	printf( "conv_by_col->src_crop_start_pos_plus_trim.y_pos=%u param->conv_info->crop_pos_src.y_pos=%u\n", conv_by_col->src_crop_start_pos_plus_trim.y_pos, param->conv_info->crop_pos_src.y_pos );
#endif
}


static FJ_ERR_CODE bf_iq_dpc_conv_line( T_BF_IQ_DPC_CONV_PARAM* const param )
{
	FJ_ERR_CODE	fj_ercd = FJ_ERR_OK;

	while( (param->conv_by_col[FJ_IQ_BADPIX_COLUM_EVEN].src_cur_start_pos_y_plus_trim < param->conv_by_col[FJ_IQ_BADPIX_COLUM_EVEN].src_crop_end_pos_plus_trim.y_pos)
	    || (param->conv_by_col[FJ_IQ_BADPIX_COLUM_ODD ].src_cur_start_pos_y_plus_trim < param->conv_by_col[FJ_IQ_BADPIX_COLUM_ODD ].src_crop_end_pos_plus_trim.y_pos) ) {
		bf_iq_dpc_conv_incr_line_of_src_x( &param->conv_by_col[FJ_IQ_BADPIX_COLUM_EVEN] );
		bf_iq_dpc_conv_incr_line_of_src_x( &param->conv_by_col[FJ_IQ_BADPIX_COLUM_ODD] );

#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
		printf( "Y valid = %u, %u\n", param->conv_by_col[FJ_IQ_BADPIX_COLUM_EVEN].src_x_valid_num, param->conv_by_col[FJ_IQ_BADPIX_COLUM_ODD ].src_x_valid_num );
#endif

		if( (param->conv_by_col[FJ_IQ_BADPIX_COLUM_EVEN].src_x_valid_num != 0)
		 || (param->conv_by_col[FJ_IQ_BADPIX_COLUM_ODD ].src_x_valid_num != 0) ) {
			fj_ercd = bf_iq_dpc_conv_col( param );
			if( fj_ercd != FJ_ERR_OK ) {
				break;
			}
		}

		if( (param->conv_by_col[FJ_IQ_BADPIX_COLUM_EVEN].src_y.src_cur_addr == NULL)
		 && (param->conv_by_col[FJ_IQ_BADPIX_COLUM_ODD ].src_y.src_cur_addr == NULL) ) {
			break;
		}

		bf_iq_dpc_conv_incr_dst_pos_y( &param->conv_by_col[FJ_IQ_BADPIX_COLUM_EVEN] );
		bf_iq_dpc_conv_incr_dst_pos_y( &param->conv_by_col[FJ_IQ_BADPIX_COLUM_ODD] );
	}

	return fj_ercd;
}




static VOID bf_iq_dpc_conv_set_add_pat_intr( FJ_IQ_BADPIX_ADD_PAT* const dst_intr, const FJ_IQ_BADPIX_ADD_PAT* const src_prog )
{
	UINT32	loopcnt;

	*dst_intr = *src_prog;

	dst_intr->cycle_start_pos.y_pos /= 2;
	dst_intr->cycle_rect.lines /= 2;

	for( loopcnt = 0; loopcnt < (sizeof(dst_intr->add_pos) / sizeof(dst_intr->add_pos[0])); loopcnt++ ) {
		dst_intr->add_pos[loopcnt].y /= 2;
	}
}


static FJ_ERR_CODE bf_iq_dpc_conv_interlace( T_BF_IQ_DPC_CONV_PARAM* const param )
{
	FJ_IQ_BADPIX_LINE	loopcnt_y;
	FJ_IQ_BADPIX_COLUM	loopcnt_x;
	FJ_ERR_CODE			fj_ercd;
	UCHAR				crop_start_line_is_odd;
	UCHAR				src_start_line_is_odd;
	UCHAR				field_no;
	static const UCHAR	odd_even_to_field_no_tbl[2][2] = {	//	[src_start_line_is_odd][crop_start_is_odd]
		[0][0] = 0,
		[0][1] = 1,
		[1][0] = 1,
		[1][1] = 0,
	};

	src_start_line_is_odd = param->src_tbl_info->sta_y % 2;
	param->sen_window_pos_src_intr = param->conv_info->sen_window_pos_src;
	param->sen_window_pos_src_intr.y_pos /= 2;	// Progressive to interlaced.
	param->sen_window_pos_dst_intr = param->conv_info->sen_window_pos_dst;
	param->sen_window_pos_dst_intr.y_pos /= 2;	// Progressive to interlaced.

	for( loopcnt_y = FJ_IQ_BADPIX_LINE_EVEN; loopcnt_y < FJ_IQ_BADPIX_LINE_MAX; loopcnt_y++ ) {
		for( loopcnt_x = FJ_IQ_BADPIX_COLUM_EVEN; loopcnt_x < FJ_IQ_BADPIX_COLUM_MAX; loopcnt_x++ ) {
			bf_iq_dpc_conv_set_add_pat_intr( &param->conv_by_col[loopcnt_x].add_pat_intr, &param->conv_info->add_pat[loopcnt_y][loopcnt_x] );
			bf_iq_dpc_conv_set_crop_start_pos( &param->conv_by_col[loopcnt_x], param );

			// Calc initial interlaced position.
			param->conv_by_col[loopcnt_x].src_cur_start_pos_y_plus_trim = param->conv_by_col[loopcnt_x].src_crop_start_pos_plus_trim.y_pos;
			param->conv_by_col[loopcnt_x].src_cur_end_pos_y_plus_trim = param->conv_by_col[loopcnt_x].src_cur_start_pos_y_plus_trim
																	  + param->conv_by_col[loopcnt_x].add_pat_intr.cycle_rect.lines;

#ifdef CO_BF_IQ_DPC_CONV_DEBUG_PRINT
			printf( "src_cur_start_pos_y_plus_trim=%u src_cur_end_pos_y_plus_trim=%u\n", param->conv_by_col[loopcnt_x].src_cur_start_pos_y_plus_trim, param->conv_by_col[loopcnt_x].src_cur_end_pos_y_plus_trim );
#endif

			// Calc field No. (Odd/Even line.)
			crop_start_line_is_odd = (param->conv_info->sen_window_pos_src.y_pos + param->conv_info->add_pat[loopcnt_y][loopcnt_x].add_pos[0].y) % 2;
			field_no = odd_even_to_field_no_tbl[src_start_line_is_odd][crop_start_line_is_odd];

			// Set "src_y".
			param->conv_by_col[loopcnt_x].src_y.src_cur_addr = param->src_tbl_info->addr_by_fields[field_no];
			param->conv_by_col[loopcnt_x].src_y.src_pos_y_plus_trim = 0;
			param->conv_by_col[loopcnt_x].src_y.dst_pos_y = 0;
			param->conv_by_col[loopcnt_x].even_odd_x = loopcnt_x;
		}

		bf_iq_dpc_conv_init_dst_tbl( loopcnt_y, &param->dst_tbl, param->dst_tbl_info );

		fj_ercd = bf_iq_dpc_conv_line( param );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}

		bf_iq_dpc_conv_set_dst_eod( &param->dst_tbl );

		bf_iq_dpc_conv_update_dst_tbl_info( loopcnt_y, param->dst_tbl_info, &param->dst_tbl );
	}


	return FJ_ERR_OK;
}


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

FJ_ERR_CODE BF_IQ_Dpc_Conv_Run( const E_IQ_CALIB_TGT dst_target, const E_IQ_CALIB_TGT src_target, const FJ_IQ_BADPIX_CONV_INFO* const conv_info )
{
	T_BF_IQ_DPC_CONV_PARAM	param;
	FJ_ERR_CODE				fj_ercd = FJ_ERR_OK;
	UINT32					tbl_idx;

	for( tbl_idx = 0; tbl_idx < D_BF_IQ_DPC_TBL_IDX_MAX; tbl_idx++ ) {
		param.src_tbl_info = BF_IQ_Dpc_Tbl_Get_Info( src_target, tbl_idx );
		if( param.src_tbl_info == NULL ) {
			return FJ_ERR_IQ_TBL_INVALID;
		}
		if( param.src_tbl_info->valid == 0 ) {
			continue;
		}

		param.dst_tbl_info = BF_IQ_Dpc_Tbl_Get_Info( dst_target, tbl_idx );
		if( param.dst_tbl_info == NULL ) {
			return FJ_ERR_IQ_TBL_INVALID;
		}

		bf_iq_dpc_conv_init_dst_tbl_info( param.dst_tbl_info );

		param.conv_info = conv_info;

		fj_ercd = bf_iq_dpc_conv_interlace( &param );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}

		bf_iq_dpc_conv_validate_dst_tbl_info( param.dst_tbl_info, &param );

		BF_IQ_Dcache_Clear( (ULONG)param.dst_tbl_info->top_addr, param.dst_tbl_info->bytelen );
	}

	return fj_ercd;
}

