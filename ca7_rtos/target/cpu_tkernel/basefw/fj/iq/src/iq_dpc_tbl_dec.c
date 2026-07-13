/**
 * @file		iq_dpc_tbl_dec.c
 * @brief		Badpix table decoder.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// IQ header
#include "iq_dpc_tbl_dec.h"
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

//#define D_BF_IQ_DPC_TBL_DEC_DBG_PRINT

#define D_BF_IQ_DPC_TBL_DEC_POS_INVALID			(0xFFFF)


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

static UINT32 bf_iq_dpc_tbl_dec_calc_succession_count( T_BF_IQ_DPC_TBL_DEC_PARAM* const param, const UCHAR x_pos, const USHORT y_pos )
{
	UINT32										even_or_odd;
	T_BF_IQ_DPC_TBL_DEC_SUCCESSION_INFO*	succession_info;

	even_or_odd = x_pos %2;
	succession_info = &param->succession_info[even_or_odd];
	

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


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

FJ_ERR_CODE BF_IQ_Dpc_Tbl_Dec_Add_Pixel( T_BF_IQ_DPC_TBL_DEC_PARAM* const param, const USHORT x_pos, const USHORT y_pos )
{
	UINT32					ui_data_type_id;
	UINT32					ui_before_data_type_id;
	U_BF_IQ_DPC_TBL_DATA*	addr_before = NULL;


	//
	// Set Vertical address infomation if needed.
	//
	if( y_pos != param->y_pos_last ) {
		if( (y_pos - param->y_pos_last) > D_BF_IQ_DPC_TBL_REL_ADDR_MAX ) {
			// ABS addressing.
			if( (param->maxbytelen - param->bytelen) < (3 * sizeof(U_BF_IQ_DPC_TBL_DATA)) ) {
				return FJ_ERR_IQ_TBL_FULL;
			}
			param->tbl_addr->rel.ui_addr = 0;	// dummy
			param->tbl_addr->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE;
			param->tbl_addr->rel.ui_format_id = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_ABS;
#ifdef D_BF_IQ_DPC_TBL_DEC_DBG_PRINT
			printf( "Y=%u: %u %u %u", y_pos, param->tbl_addr->rel.ui_format_id, param->tbl_addr->rel.ui_data_type_id, param->tbl_addr->rel.ui_addr );
#endif
			param->tbl_addr++;
			param->tbl_addr->abs.ui_addr = y_pos;
#ifdef D_BF_IQ_DPC_TBL_DEC_DBG_PRINT
			printf( " %u\n", param->tbl_addr->abs.ui_addr );
#endif
			param->tbl_addr++;
			param->bytelen += sizeof(U_BF_IQ_DPC_TBL_DATA) *2;
		}
		else {
			// REL addressing
			if( (param->maxbytelen - param->bytelen) < (2 * sizeof(U_BF_IQ_DPC_TBL_DATA)) ) {
				return FJ_ERR_IQ_TBL_FULL;
			}
			param->tbl_addr->rel.ui_addr = y_pos - param->y_pos_last;
			param->tbl_addr->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE;
			param->tbl_addr->rel.ui_format_id = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_REL;
#ifdef D_BF_IQ_DPC_TBL_DEC_DBG_PRINT
			printf( "Y=%u: %u %u %u\n", y_pos, param->tbl_addr->rel.ui_format_id, param->tbl_addr->rel.ui_data_type_id, param->tbl_addr->rel.ui_addr );
#endif
			param->tbl_addr++;
			param->bytelen += sizeof(U_BF_IQ_DPC_TBL_DATA);
		}
		param->x_pos_last = 0;
		param->y_pos_last = y_pos;
	}



	//
	// Set Horizontal address infomation if needed.
	//
	switch( bf_iq_dpc_tbl_dec_calc_succession_count( param, x_pos, y_pos ) ) {
		case 0:
			return FJ_ERR_NG;	// Internal error.
		case 1:
			// Badpix indivisual(1st pixel)
			param->tbl_addr_before[x_pos %2] = NULL;
			addr_before = NULL;
			ui_data_type_id        = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_SINGLE;
			ui_before_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_BEFORE;	// dummy
			break;
		case 2:
			// Badpix two continuously(2nd pixel)
			addr_before = param->tbl_addr_before[x_pos %2] ;
			ui_data_type_id        = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER;
			ui_before_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_BEFORE;
			break;
		case 3:
			// Badpix tree continuously(3rd pixel)
			addr_before = param->tbl_addr_before[x_pos %2] ;
			ui_data_type_id        = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER;
			ui_before_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_CENTER;
			break;
		case 4:
			// Badpix four continuously(4th pixel)
			addr_before = param->tbl_addr_before[x_pos %2] ;
			ui_data_type_id        = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER;
			ui_before_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_CENTER;
			break;
		default:
			// Badpix over five continuously(4th pixel)
			addr_before = param->tbl_addr_before[x_pos %2] ;
			ui_data_type_id        = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER;
			ui_before_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_CENTER;
			break;
	}

	if( x_pos != param->x_pos_last ) {
		param->tbl_addr_before[x_pos %2] = param->tbl_addr;

		if( (x_pos - param->x_pos_last) > D_BF_IQ_DPC_TBL_REL_ADDR_MAX ) {
			// ABS addressing.
			if( (param->maxbytelen - param->bytelen) < (3 * sizeof(U_BF_IQ_DPC_TBL_DATA)) ) {
				return FJ_ERR_IQ_TBL_FULL;
			}
			param->tbl_addr->rel.ui_addr = 0;	// dummy
			param->tbl_addr->rel.ui_data_type_id = ui_data_type_id;
			param->tbl_addr->rel.ui_format_id = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_ABS;
#ifdef D_BF_IQ_DPC_TBL_DEC_DBG_PRINT
			printf( "X=%u: %u %u %u", x_pos, param->tbl_addr->rel.ui_format_id, param->tbl_addr->rel.ui_data_type_id, param->tbl_addr->rel.ui_addr );
#endif
			param->tbl_addr++;
			param->tbl_addr->abs.ui_addr = x_pos;
#ifdef D_BF_IQ_DPC_TBL_DEC_DBG_PRINT
			printf( " %u\n", param->tbl_addr->abs.ui_addr );
#endif
			param->tbl_addr++;
			param->x_pos_last = x_pos;
			param->bytelen += sizeof(U_BF_IQ_DPC_TBL_DATA) *2;
		}
		else {
			// REL addressing
			if( (param->maxbytelen - param->bytelen) < (2 * sizeof(U_BF_IQ_DPC_TBL_DATA)) ) {
				return FJ_ERR_IQ_TBL_FULL;
			}
			param->tbl_addr->rel.ui_addr = x_pos - param->x_pos_last;
			param->tbl_addr->rel.ui_data_type_id = ui_data_type_id;
			param->tbl_addr->rel.ui_format_id = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_REL;
#ifdef D_BF_IQ_DPC_TBL_DEC_DBG_PRINT
			printf( "X=%u: %u %u %u\n", x_pos, param->tbl_addr->rel.ui_format_id, param->tbl_addr->rel.ui_data_type_id, param->tbl_addr->rel.ui_addr );
#endif
			param->tbl_addr++;
			param->x_pos_last = x_pos;
			param->bytelen += sizeof(U_BF_IQ_DPC_TBL_DATA);
		}

		param->pix_num++;

		// Replace before data type id.
		if( addr_before != NULL ) {
			addr_before->rel.ui_data_type_id = ui_before_data_type_id;
		}
	}

	return FJ_ERR_OK;
}


ULLONG BF_IQ_Dpc_Tbl_Dec_Get_BytelenAlign( T_BF_IQ_DPC_TBL_DEC_PARAM* const param )
{
	return M_IQ_ROUNDUP_8( param->bytelen );
}


ULLONG BF_IQ_Dpc_Tbl_Dec_Get_BytelenHardwareAlign( T_BF_IQ_DPC_TBL_DEC_PARAM* const param )
{
	return M_IQ_ROUNDUP_PRCH_BURST_BYTES_MIN( param->bytelen );
}


ULLONG BF_IQ_Dpc_Tbl_Dec_Get_Bytelen( T_BF_IQ_DPC_TBL_DEC_PARAM* const param )
{
	return param->bytelen;
}


UINT32 BF_IQ_Dpc_Tbl_Dec_Get_Pixnum( T_BF_IQ_DPC_TBL_DEC_PARAM* const param )
{
	return param->pix_num;
}


U_BF_IQ_DPC_TBL_DATA* BF_IQ_Dpc_Tbl_Dec_Get_TblAddr( T_BF_IQ_DPC_TBL_DEC_PARAM* const param )
{
	return param->tbl_addr_top;
}


FJ_ERR_CODE BF_IQ_Dpc_Tbl_Dec_Init( T_BF_IQ_DPC_TBL_DEC_PARAM* const param, U_BF_IQ_DPC_TBL_DATA* const addr, const ULLONG maxbytelen )
{
	param->x_pos_last = 0;
	param->y_pos_last = 0;
	param->maxbytelen = maxbytelen;
	param->tbl_addr_top = addr;
	param->tbl_addr = addr;
	param->pix_num = 0;
	param->bytelen = 0ULL;
	param->succession_info[0].x_pos_last = D_BF_IQ_DPC_TBL_DEC_POS_INVALID;
	param->succession_info[0].y_pos_last = D_BF_IQ_DPC_TBL_DEC_POS_INVALID;
	param->succession_info[0].succession_count = 0;
	param->succession_info[1].x_pos_last = D_BF_IQ_DPC_TBL_DEC_POS_INVALID;
	param->succession_info[1].y_pos_last = D_BF_IQ_DPC_TBL_DEC_POS_INVALID;
	param->succession_info[1].succession_count = 0;

	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_IQ_Dpc_Tbl_Dec_Finish( T_BF_IQ_DPC_TBL_DEC_PARAM* const param )
{
	if( (param->maxbytelen - param->bytelen) < sizeof(U_BF_IQ_DPC_TBL_DATA) ) {
		return FJ_ERR_IQ_TBL_FULL;
	}

	param->tbl_addr->rel.ui_addr = 0;	// dummy
	param->tbl_addr->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_END_OF_DATA;
	param->tbl_addr->rel.ui_format_id = D_BF_IQ_DPC_TBL_FORMAT_OTHER_DATA;
	param->tbl_addr++;
	param->bytelen += sizeof(U_BF_IQ_DPC_TBL_DATA);

	return FJ_ERR_OK;
}

