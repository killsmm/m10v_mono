/**
 * @file		iq_dpc_multipipe.c
 * @brief		Badpix Detection/Compensation control API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

// IQ header
#include "iq_dpc_multipipe.h"
#include "iq_dpc_multipipe_tbl.h"
#include "iq_dpc_multipipe_tbl_mng.h"
#include "iq_dpc_tbl.h"
#include "iq_dpc_tbl_sel.h"
#include "iq_dpc_tbl_enc.h"
#include "iq_dpc_tbl_dec.h"
#include "iq_dcache.h"
#include "iq_common.h"

// System header
#include "instance.h"

// Utility header
#include "debug.h"
#include "mba_if.h"

// Driver header
#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

//#define D_BF_IQ_DPC_MULTIPIPE_DEBUG

#define D_BF_IQ_DPC_MULTIPIPE_COMP_MERGINE_X	(2)		// for Compensation reference pixel.
#define D_BF_IQ_DPC_MULTIPIPE_PARAM_MAX_NUM		(3)


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

// T_BF_IQ_DPC_MULTIPIPE_PARAM
typedef struct {
	UCHAR									allocated;
	// API arguments.
	UCHAR									demosaic_id;
	const T_BF_IQ_DPC_MULTIPIPE_CALC_PARAM*	calc_param;
	// Input table.
	E_IQ_CALIB_TGT							e_calib_tgt;
	UINT32									tbl_idx;
	T_BF_IQ_DPC_TBL_INFO*					tbl_info;
	// Output table.
	T_BF_IQ_DPC_MULTIPIPE_TBL*				multipipe_tbl;
	// Work
	FJ_IQ_BADPIX_POS						pos_tbl_buf[D_BF_IQ_DPC_MAX_PIXELS + D_BF_IQ_DPC_MAX_LINES];
	T_BF_IQ_DPC_TBL_ENC_PARAM				enc_param;
	UINT32									x_idx;
	UINT32									y_idx;
	const T_BF_IQ_DPC_AREA_INFO*			crop_info;
	T_BF_IQ_DPC_TBL_INFO*					dst_dpc_tbl_info;
	U_BF_IQ_DPC_TBL_DATA*					top_addr;
	ULLONG									maxbytelen;
	T_BF_IQ_DPC_AREA_INFO*					dst_crop_win;
} T_BF_IQ_DPC_MULTIPIPE_PARAM;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/

static volatile T_BF_IQ_DPC_MULTIPIPE_PARAM gbf_iq_dpc_multipipe_param[D_BF_IQ_DPC_MULTIPIPE_PARAM_MAX_NUM];
static ULONG gbf_iq_dpc_multipipe_spin_lock __attribute__((section(".LOCK_SECTION"), aligned(64)));


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/

static T_BF_IQ_DPC_MULTIPIPE_PARAM* bf_iq_dpc_multipipe_calc_alloc_param_buf( VOID )
{
	UINT32							loopcnt;
	T_BF_IQ_DPC_MULTIPIPE_PARAM*	ret = NULL;

	Dd_ARM_Critical_Section_Start(gbf_iq_dpc_multipipe_spin_lock);

	for( loopcnt = 0; loopcnt < D_BF_IQ_DPC_MULTIPIPE_PARAM_MAX_NUM; loopcnt++ ) {
		if( gbf_iq_dpc_multipipe_param[loopcnt].allocated == 0 ) {
			gbf_iq_dpc_multipipe_param[loopcnt].allocated = 1;

			ret = (T_BF_IQ_DPC_MULTIPIPE_PARAM*)&gbf_iq_dpc_multipipe_param[loopcnt];
			break;
		}
	}

	Dd_ARM_Critical_Section_End(gbf_iq_dpc_multipipe_spin_lock);

	return ret;
}


static VOID bf_iq_dpc_multipipe_calc_free_param_buf( T_BF_IQ_DPC_MULTIPIPE_PARAM* param )
{
	param->allocated = 0;
}


static FJ_ERR_CODE bf_iq_dpc_multipipe_calc_ind( T_BF_IQ_DPC_MULTIPIPE_PARAM* const param )
{
	FJ_IQ_BADPIX_POS			pos;
	T_BF_IQ_DPC_TBL_DEC_PARAM	dec_param;
	UINT32						odd_even;
	UINT32						loopcnt;
	FJ_ERR_CODE					fj_ercd;
	USHORT						sta_x;
	USHORT						end_x;
	USHORT						dec_x = 0;
	USHORT						sta_y;
	USHORT						end_y;
	USHORT						dec_y = 0;


	param->dst_dpc_tbl_info->bytelen	= 0ull;
	param->dst_dpc_tbl_info->pixel_num	= 0;
	param->dst_dpc_tbl_info->valid		= FALSE;

	if( param->calc_param->x_decrease_crop_offset != FALSE ) {
		dec_x =  param->crop_info->pos_x;
	}
	if( param->calc_param->y_decrease_crop_offset != FALSE ) {
		dec_y =  param->crop_info->pos_y /2;	// Interlaced
	}
	sta_x =  dec_x +D_BF_IQ_DPC_MULTIPIPE_COMP_MERGINE_X;
	sta_y =  param->crop_info->pos_y /2;	// Interlaced
	end_x =  param->crop_info->pos_x + param->crop_info->width -D_BF_IQ_DPC_MULTIPIPE_COMP_MERGINE_X -1;
	end_y = (param->crop_info->pos_y + param->crop_info->lines) /2 -1;	// Interlaced


#ifdef D_BF_IQ_DPC_MULTIPIPE_DEBUG
	BF_Debug_Print_Debug(( "%s() [%u][%u] x=%u-%u y=%u-%u\n",
				__func__,
				param->x_idx, param->y_idx,
				sta_x, end_x,
				sta_y *2, end_y *2 +1	// Interlace -> Progressive
				));
#endif


	for( odd_even = 0; odd_even < 2; odd_even++ ) {
		if( param->maxbytelen == 0 ) {
			return FJ_ERR_IQ_FILE_TOO_SHORT;
		}

		// Create cropped table.
		fj_ercd = BF_IQ_Dpc_Tbl_Dec_Init( &dec_param, param->top_addr, param->maxbytelen );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}

		for( loopcnt = 0; loopcnt < param->enc_param.pos_num[odd_even]; loopcnt++ ) {
			pos = param->enc_param.pos_tbl[odd_even][loopcnt];

			if( pos.x_pos < sta_x ) {
#ifdef D_BF_IQ_DPC_MULTIPIPE_DEBUG
				BF_Debug_Print_Debug(( "skip1: %u, %u\n", pos.x_pos, pos.y_pos ));
#endif
				continue;
			}
			if( pos.y_pos < sta_y ) {
#ifdef D_BF_IQ_DPC_MULTIPIPE_DEBUG
				BF_Debug_Print_Debug(( "skip2: %u, %u\n", pos.x_pos, pos.y_pos ));
#endif
				continue;
			}
			if( pos.x_pos > end_x ) {
#ifdef D_BF_IQ_DPC_MULTIPIPE_DEBUG
				BF_Debug_Print_Debug(( "skip3: %u, %u\n", pos.x_pos, pos.y_pos ));
#endif
				continue;
			}
			if( pos.y_pos > end_y ) {
#ifdef D_BF_IQ_DPC_MULTIPIPE_DEBUG
				BF_Debug_Print_Debug(( "skip4: %u, %u\n", pos.x_pos, pos.y_pos ));
#endif
				continue;
			}

#ifdef D_BF_IQ_DPC_MULTIPIPE_DEBUG
			BF_Debug_Print_Debug(( "use: %u, %u\n", pos.x_pos, pos.y_pos ));
#endif

			fj_ercd = BF_IQ_Dpc_Tbl_Dec_Add_Pixel( &dec_param, pos.x_pos - dec_x, pos.y_pos - dec_y );
			if( fj_ercd != FJ_ERR_OK ) {
				return fj_ercd;
			}
		}

		fj_ercd = BF_IQ_Dpc_Tbl_Dec_Finish( &dec_param );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}

		param->dst_dpc_tbl_info->top_addr						 = param->multipipe_tbl->top_addr;
		param->dst_dpc_tbl_info->maxbytelen						 = 0;
		param->dst_dpc_tbl_info->addr_by_fields[odd_even]		 = BF_IQ_Dpc_Tbl_Dec_Get_TblAddr( &dec_param );
		param->dst_dpc_tbl_info->bytelen_by_fields[odd_even]	 = BF_IQ_Dpc_Tbl_Dec_Get_Bytelen( &dec_param );
		param->dst_dpc_tbl_info->bytelen						+= BF_IQ_Dpc_Tbl_Dec_Get_BytelenHardwareAlign( &dec_param );
		param->dst_dpc_tbl_info->pixel_num						+= BF_IQ_Dpc_Tbl_Dec_Get_Pixnum( &dec_param );

#ifdef D_BF_IQ_DPC_MULTIPIPE_DEBUG
		BF_Debug_Print_Debug(( "%s() [%u][%u] addr=%p\n",
				__func__,
				param->x_idx, param->y_idx,
				param->dst_dpc_tbl_info->top_addr
				));
#endif

		if( param->maxbytelen < param->dst_dpc_tbl_info->bytelen_by_fields[odd_even] ) {
			param->maxbytelen = 0;
		}
		else {
			param->maxbytelen -= BF_IQ_Dpc_Tbl_Dec_Get_BytelenAlign( &dec_param );
			param->top_addr += BF_IQ_Dpc_Tbl_Dec_Get_BytelenAlign( &dec_param ) / sizeof(U_BF_IQ_DPC_TBL_DATA);
		}

		BF_IQ_Dcache_Clear( (ULONG)param->dst_dpc_tbl_info->addr_by_fields[odd_even], (ULONG)param->dst_dpc_tbl_info->bytelen_by_fields[odd_even] );
	}

	*(param->dst_crop_win)				= *(param->crop_info);
	param->dst_dpc_tbl_info->sta_x		= param->crop_info->pos_x - dec_x;
	param->dst_dpc_tbl_info->sta_y		= param->crop_info->pos_y - (dec_y *2);	// dec_y is interlaced value.
	param->dst_dpc_tbl_info->width		= param->crop_info->width;
	param->dst_dpc_tbl_info->lines		= param->crop_info->lines;
	param->dst_dpc_tbl_info->field_num	= 2;

	param->dst_dpc_tbl_info->valid		= TRUE;

	return FJ_ERR_OK;
}


static FJ_ERR_CODE bf_iq_dpc_multipipe_calc_all( T_BF_IQ_DPC_MULTIPIPE_PARAM* const param )
{
	UINT32		x_idx;
	UINT32		y_idx;
	FJ_ERR_CODE	fj_ercd;

	param->multipipe_tbl->valid = FALSE;
	for( y_idx = 0; y_idx < param->calc_param->y_num; y_idx++ ) {
		for( x_idx = 0; x_idx < param->calc_param->x_num; x_idx++ ) {
			param->multipipe_tbl->tbl_info[x_idx][y_idx].valid = FALSE;
		}
	}


	param->top_addr		= param->multipipe_tbl->top_addr;
	param->maxbytelen	= param->multipipe_tbl->maxbytelen;

	param->enc_param.pos_tbl_top		= param->pos_tbl_buf;
	param->enc_param.pos_maxnum			= sizeof(param->pos_tbl_buf) / sizeof(param->pos_tbl_buf[0]);
	param->enc_param.pos_num[0]			= 0;
	param->enc_param.pos_num[1]			= 0;
	param->enc_param.flg_declease_sta	= FALSE;

	// Create(convert to) pos_tbl.
	fj_ercd = BF_IQ_Dpc_Tbl_Enc( &param->enc_param, param->tbl_info );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	for( y_idx = 0; y_idx < param->calc_param->y_num; y_idx++ ) {
		for( x_idx = 0; x_idx < param->calc_param->x_num; x_idx++ ) {
			param->x_idx			= x_idx;
			param->y_idx			= y_idx;
			param->crop_info		= &param->calc_param->crop_info[x_idx][y_idx];
			param->dst_dpc_tbl_info	= &param->multipipe_tbl->tbl_info[param->x_idx][param->y_idx];
			param->dst_crop_win		= &param->multipipe_tbl->crop_win[param->x_idx][param->y_idx];
			fj_ercd = bf_iq_dpc_multipipe_calc_ind( param );
			if( fj_ercd != FJ_ERR_OK ) {
				return fj_ercd;
			}
		}
	}

	param->multipipe_tbl->x_num = param->calc_param->x_num;
	param->multipipe_tbl->y_num = param->calc_param->y_num;

	param->multipipe_tbl->valid = TRUE;

	return FJ_ERR_OK;
}


static FJ_ERR_CODE bf_iq_dpc_multipipe_calc_main( T_BF_IQ_DPC_MULTIPIPE_PARAM* param )
{
	FJ_ERR_CODE	fj_ercd;

	//
	// Get original Badpix table. (Input table)
	//
	BF_IQ_Dpc_Tbl_Sel_Get( param->demosaic_id, &param->e_calib_tgt, &param->tbl_idx );
	switch( param->e_calib_tgt ) {
		case E_IQ_CALIB_TGT_ERR:	// FALLTHROUGH
		case E_IQ_CALIB_TGT_INIT:
			return FJ_ERR_IQ_BP_CALIB_TGT;
		default:
			break;
	}
	if( param->tbl_idx == D_BF_IQ_DPC_TBL_IDX_ERR ) {
		return FJ_ERR_IQ_TBL_INVALID;
	}

	param->tbl_info = BF_IQ_Dpc_Tbl_Get_Info( param->e_calib_tgt, param->tbl_idx );
	if( param->tbl_info == NULL ) {
		return FJ_ERR_IQ_TBL_INVALID;
	}


	//
	// Allocate Badpix table memory for multi-pipe. (Output table)
	//
	param->multipipe_tbl = BF_IQ_Dpc_MultiPipeTbl_Alloc();
	if( param->multipipe_tbl == NULL ) {
		return FJ_ERR_IQ_TBL_INVALID;
	}



	//
	// Process cropping.
	//
	fj_ercd = bf_iq_dpc_multipipe_calc_all( param );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}



	//
	// Set multi-pipe table to table manager.
	//
	fj_ercd = BF_IQ_Dpc_MultiPipeTblMng_Set_Info( param->demosaic_id, param->multipipe_tbl );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}


	return FJ_ERR_OK;
}


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

VOID BF_IQ_Dpc_MultiPipe_Init( VOID )
{
	UINT32	loopcnt;

	for( loopcnt = 0; loopcnt < D_BF_IQ_DPC_MULTIPIPE_PARAM_MAX_NUM; loopcnt++ ) {
		gbf_iq_dpc_multipipe_param[loopcnt].allocated = 0;
	}
}


FJ_ERR_CODE BF_IQ_Dpc_MultiPipe_Calc( const UCHAR demosaic_id, const T_BF_IQ_DPC_MULTIPIPE_CALC_PARAM* const calc_param )
{
	T_BF_IQ_DPC_MULTIPIPE_PARAM*	param;
	FJ_ERR_CODE						fj_ercd;

	param = bf_iq_dpc_multipipe_calc_alloc_param_buf();
	if( param == NULL ) {
		return FJ_ERR_IQ_TBL_FULL;
	}

	param->demosaic_id = demosaic_id;
	param->calc_param = calc_param;

	fj_ercd = bf_iq_dpc_multipipe_calc_main( param );

	bf_iq_dpc_multipipe_calc_free_param_buf( param );

	return fj_ercd;
}


#ifdef CO_BF_IQ_DBG_TABLE_BYTES
VOID BF_IQ_Dpc_MultiPipe_Print_SizeDbg( VOID )
{
	printf( "[Badpix multi pipe convert work memory]\n" );
	printf( "gbf_iq_dpc_multipipe_param memory = %u bytes\n",
			sizeof(gbf_iq_dpc_multipipe_param) );
}
#endif //CO_BF_IQ_DBG_TABLE_BYTES

