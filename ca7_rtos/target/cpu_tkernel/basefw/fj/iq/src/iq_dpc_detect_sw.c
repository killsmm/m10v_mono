/**
 * @file		iq_dpc_detect_sw.c
 * @brief		Algorithm for Badpix software detection
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "fj_iq.h"

#include "fj_std.h"

#include "iq_dpc_detect_sw.h"

#include "iq_dpc_detect_info.h"
#include "iq_dpc_tbl.h"
#include "iq_dpc_custom.h"

#include "iq_dcache.h"
#include "iq_common.h"


#include "debug.h"

#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
//#define CO_BF_IQ_DPC_PERCENT_DEBUGLOG	// enable debug log for percent threthold detection
//#define CO_BF_IQ_DPC_SW_DETECT_DEBUGLOG
//#define CO_BF_IQ_DPC_SW_MERGE_DEBUGLOG


#define D_BF_IQ_BAYER_ONE_PIXEL_BYTES		sizeof(USHORT)	/* 16bit unpack */

/* U_BF_IQ_DPC_TBL_DATA table array number  in 8 byte */
#define D_BF_IQ_DPC_TBL_IDX_8BYTE_ALIGN_NUM	(8 / D_BF_IQ_DPC_TBL_SIZEOF_DEFECT_INFO_DATA)


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
// U_BF_IQ_DPC_TBL_DATA table array index  round up to 8byte alignment index number
#define M_BF_IQ_DPC_TBL_IDX_8BYTE_ROUNDUP(idx) ( \
			( ((idx) + (D_BF_IQ_DPC_TBL_IDX_8BYTE_ALIGN_NUM -1)) \
			  / D_BF_IQ_DPC_TBL_IDX_8BYTE_ALIGN_NUM \
			) * D_BF_IQ_DPC_TBL_IDX_8BYTE_ALIGN_NUM \
		)


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

// E_EO
typedef enum {
	E_EO_EVEN = 0,
	E_EO_ODD,
	E_EO_MAX
} E_EO;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/

//
// For BF_IQ_Dpc_Detect_Sw_Run()
//
// "volatile" no needed because this global variables used on single task.
static const USHORT* pus_last_bad_pix_bay_addr[E_EO_MAX] = { NULL, NULL };
static UINT32 ui_cur_row_in_field;					// current row number(in PRO 1field).
static UINT32 ui_defect_info_tbl_idx;				// badpix table count(total of all PRO field).
static UINT32 ui_defect_info_tbl_idx_by_field;		// badpix table count(PRO 1field).
static const USHORT* pus_bayer16_addr;				// RAW address on D-Cache.
static const USHORT* pus_top_bay_addr;				// RAW top address of current line.
static UINT32 ui_last_writed_row_address;			// Last row number of writed badpix table.
static UINT32 ui_last_writed_column_address;		// Last column number of writed badpix table.
static UINT32 ui_defect_info_tbl_idx_max;
static USHORT* pus_defect_info_tbl;
static T_BF_IQ_DPC_DETECT_INFO* pt_detect_info;
static USHORT us_corner_column_top[3];
static USHORT us_corner_column_bottom[3];
static USHORT us_corner_threshold_high[3];
static USHORT us_corner_threshold_low[3];
static U_BF_IQ_DPC_TBL_DATA* pt_defect_info_tbl_last[E_EO_MAX] = { NULL, NULL };
static USHORT g_vd_ofs;								// VD offset of indivisually field.
#ifdef CO_BF_IQ_DPC_SW_DETECT_DEBUGLOG
static USHORT g_us_cur_row;							// current row number.(Progressive)
#endif


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
// nothing to do


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/

//
// For BF_IQ_Dpc_Detect_Sw_Run()
//

static VOID bf_iq_dpc_detect_sw_add_tbl_row( const UINT32 ui_addr )
{
	U_BF_IQ_DPC_TBL_DATA* pt_defect_info_tbl_cur = NULL;

	if( ui_last_writed_row_address == ui_addr ) {
		// Row address equal before write.
		return;
	}

	if( ui_last_writed_row_address == 0xffffffff ) {
		ui_last_writed_row_address = 0;
	}

	if( ui_defect_info_tbl_idx >= ui_defect_info_tbl_idx_max ) {
		// Table area is full.
		return;
	}

	pt_defect_info_tbl_cur = (U_BF_IQ_DPC_TBL_DATA *)&(pus_defect_info_tbl[ui_defect_info_tbl_idx]);
	if( pt_defect_info_tbl_cur == NULL ) {	// for pass to PG-Relief
		return;
	}


	if( (ui_addr - ui_last_writed_row_address) < 4096 ) {
		pt_defect_info_tbl_cur->rel.ui_format_id    = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_REL;
		pt_defect_info_tbl_cur->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE;
		pt_defect_info_tbl_cur->rel.ui_addr         = ui_addr - ui_last_writed_row_address;

		ui_defect_info_tbl_idx++;
		ui_defect_info_tbl_idx_by_field++;
	}
	else {
		if( (ui_defect_info_tbl_idx +1) >= ui_defect_info_tbl_idx_max ) {
			// Table area is full.
			return;
		}

		pt_defect_info_tbl_cur->rel.ui_format_id    = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_ABS;
		pt_defect_info_tbl_cur->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE;
		pt_defect_info_tbl_cur->rel.ui_addr         = 0;		// dummy

		ui_defect_info_tbl_idx++;
		ui_defect_info_tbl_idx_by_field++;
		pt_defect_info_tbl_cur++;

		pt_defect_info_tbl_cur->abs.ui_addr         = ui_addr;

		ui_defect_info_tbl_idx++;
		ui_defect_info_tbl_idx_by_field++;
	}

	ui_last_writed_row_address = ui_addr;

	ui_last_writed_column_address = 0;
}


static VOID bf_iq_dpc_detect_sw_add_tbl_column( const UINT32 ui_data_type_id, const UINT32 ui_addr, const E_EO even_odd, const UINT32 ui_before_data_type_id )
{
	U_BF_IQ_DPC_TBL_DATA* pt_defect_info_tbl_cur = NULL;

	if( ui_defect_info_tbl_idx >= ui_defect_info_tbl_idx_max ) {
		// Table area is full.
		return;
	}

	if( ui_before_data_type_id != D_BF_IQ_DPC_TBL_DTYPE_DEFECT_SINGLE ) {
		pt_defect_info_tbl_last[even_odd]->rel.ui_data_type_id = ui_before_data_type_id;
	}

	pt_defect_info_tbl_cur = (U_BF_IQ_DPC_TBL_DATA *)&(pus_defect_info_tbl[ui_defect_info_tbl_idx]);
	if( pt_defect_info_tbl_cur == NULL ) {	// for pass to PG-Relief
		return;
	}

	pt_defect_info_tbl_last[even_odd] = pt_defect_info_tbl_cur;	/* pgr0000 */


	if( (ui_addr - ui_last_writed_column_address) < 4096 ) {
		pt_defect_info_tbl_cur->rel.ui_format_id    = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_REL;
		pt_defect_info_tbl_cur->rel.ui_data_type_id = ui_data_type_id;
		pt_defect_info_tbl_cur->rel.ui_addr         = ui_addr - ui_last_writed_column_address;

		ui_defect_info_tbl_idx++;
		ui_defect_info_tbl_idx_by_field++;
	}
	else {
		if( (ui_defect_info_tbl_idx +1) >= ui_defect_info_tbl_idx_max ) {
			// Table area is full.
			return;
		}

		pt_defect_info_tbl_cur->rel.ui_format_id    = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_ABS;
		pt_defect_info_tbl_cur->rel.ui_data_type_id = ui_data_type_id;
		pt_defect_info_tbl_cur->rel.ui_addr         = 0;		// dummy

		ui_defect_info_tbl_idx++;
		ui_defect_info_tbl_idx_by_field++;
		pt_defect_info_tbl_cur++;

		pt_defect_info_tbl_cur->abs.ui_addr         = ui_addr;

		ui_defect_info_tbl_idx++;
		ui_defect_info_tbl_idx_by_field++;
	}

	ui_last_writed_column_address = ui_addr;
}


static UINT32 bf_iq_dpc_detect_sw_calc_succession_count( const E_EO even_odd, const USHORT* const pus_cur_bay_addr )
{
	static UINT32 ui_succession_count[E_EO_MAX];		/* dead pixel continuously count */

	if( (pus_last_bad_pix_bay_addr[even_odd] + 2/*pixel*/) == pus_cur_bay_addr ) {
		ui_succession_count[even_odd]++;
	} else {
		ui_succession_count[even_odd] = 1;
	}

	pus_last_bad_pix_bay_addr[even_odd] = pus_cur_bay_addr;

	return ui_succession_count[even_odd];
}


static VOID bf_iq_dpc_detect_sw_write_tbl_1pix( const USHORT* const pus_cur_bay_addr )
{
	UINT32 ui_cur_column;
	UINT32 ui_succession_count;
	E_EO even_odd;	// 0:even 1:odd

	ui_cur_column = ((UINT32)pus_cur_bay_addr - (UINT32)pus_top_bay_addr) / D_BF_IQ_BAYER_ONE_PIXEL_BYTES;
	even_odd = (E_EO)(ui_cur_column % 2);


	ui_succession_count = bf_iq_dpc_detect_sw_calc_succession_count( even_odd, pus_cur_bay_addr );

	/* writing Virtical address move request if line move detected (first of dead pixel per line) */
	bf_iq_dpc_detect_sw_add_tbl_row( g_vd_ofs + ui_cur_row_in_field );


#ifdef CO_BF_IQ_DPC_SW_DETECT_DEBUGLOG
	BF_Debug_Print_Information(( "X=%u Y_Intr=%u(%u) Y_Prog=%u bay=%u\n", ui_cur_column, ui_cur_row_in_field, ui_cur_row_in_field + g_vd_ofs, g_us_cur_row, *pus_cur_bay_addr ));
#endif

	/* writing Horizontal address */
	switch( ui_succession_count ) {
	case 0:
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "bf_iq_dpc_detect_sw_write_tbl_1pix(): error: internal variable invalid. ui_succession_count=0\n" ));
#endif
		return;
	case 1:
		// dead pixel indivisual(1st pixel)
		bf_iq_dpc_detect_sw_add_tbl_column(
				D_BF_IQ_DPC_TBL_DTYPE_DEFECT_SINGLE,
				pt_detect_info->us_hd_ofs + ui_cur_column,
				even_odd,
				D_BF_IQ_DPC_TBL_DTYPE_DEFECT_SINGLE
				);
		break;
	case 2:
		// dead pixel two continuously(2nd pixel)
		bf_iq_dpc_detect_sw_add_tbl_column(
				D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER,
				pt_detect_info->us_hd_ofs + ui_cur_column,
				even_odd,
				D_BF_IQ_DPC_TBL_DTYPE_DEFECT_BEFORE
				);
		break;
	case 3:
		/* dead pixel tree continuously(3rd pixel) */
		bf_iq_dpc_detect_sw_add_tbl_column(
				D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER,
				pt_detect_info->us_hd_ofs + ui_cur_column,
				even_odd,
				D_BF_IQ_DPC_TBL_DTYPE_DEFECT_CENTER
				);
		break;
	case 4:
		/* dead pixel four continuously(4th pixel) */
		pt_detect_info->ui_over4succession_count += 4;	/* add number of impossible pixel */

		bf_iq_dpc_detect_sw_add_tbl_column(
				D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER,
				pt_detect_info->us_hd_ofs + ui_cur_column,
				even_odd,
				D_BF_IQ_DPC_TBL_DTYPE_DEFECT_CENTER
				);
		break;
	default:
		/* dead pixel over five continuously(4th pixel) */
		pt_detect_info->ui_over4succession_count++;	/* add number of impossible pixel */

		bf_iq_dpc_detect_sw_add_tbl_column(
				D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER,
				pt_detect_info->us_hd_ofs + ui_cur_column,
				even_odd,
				D_BF_IQ_DPC_TBL_DTYPE_DEFECT_CENTER
				);
		break;
	}

	pt_detect_info->ui_detect_count++;
}


static VOID bf_iq_dpc_detect_sw_write_eod( VOID )
{
	U_BF_IQ_DPC_TBL_DATA* pt_defect_info_tbl_cur;

	if( ui_defect_info_tbl_idx >= ui_defect_info_tbl_idx_max ) {
		/* table is overflow */

		pt_detect_info->ui_detect_info_tbl_overflow = 1;	/* overflow marker is set */

		pt_defect_info_tbl_cur = (U_BF_IQ_DPC_TBL_DATA *)&(pus_defect_info_tbl[ui_defect_info_tbl_idx -1]);
		pt_defect_info_tbl_cur->rel.ui_format_id    = D_BF_IQ_DPC_TBL_FORMAT_OTHER_DATA;
		pt_defect_info_tbl_cur->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_END_OF_DATA;
		pt_defect_info_tbl_cur->rel.ui_addr         = 0;

	} else {
		pt_defect_info_tbl_cur = (U_BF_IQ_DPC_TBL_DATA *)&(pus_defect_info_tbl[ui_defect_info_tbl_idx]);
		pt_defect_info_tbl_cur->rel.ui_format_id    = D_BF_IQ_DPC_TBL_FORMAT_OTHER_DATA;
		pt_defect_info_tbl_cur->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_END_OF_DATA;
		pt_defect_info_tbl_cur->rel.ui_addr         = 0;
		ui_defect_info_tbl_idx++, ui_defect_info_tbl_idx_by_field++;
	}
}


/* algolithm pattern 0  ...  bad = (us_threshold_low > pixel_val) || (pixel_val < us_threshold_high) */
static VOID bf_iq_dpc_detect_sw_1line_algo0( const UINT32 ui_cur_row )
{
	register USHORT us_threshold_high;
	register USHORT us_threshold_low;
	register const USHORT* pus_cur_bay_addr;
	register const USHORT* pus_bottom_bay_addr;
	USHORT us_corner_column_idx;


	pus_top_bay_addr = pus_bayer16_addr
					 + (pt_detect_info->us_global_w_pixels * ui_cur_row);


	for( us_corner_column_idx = 0; us_corner_column_idx < 3; us_corner_column_idx++ ) {
		pus_bottom_bay_addr = pus_top_bay_addr
							+ us_corner_column_bottom[us_corner_column_idx];

		us_threshold_high = us_corner_threshold_high[us_corner_column_idx];
		us_threshold_low = us_corner_threshold_low[us_corner_column_idx];

		/* loop by image columns */
		for( pus_cur_bay_addr = pus_top_bay_addr + us_corner_column_top[us_corner_column_idx];
		  pus_cur_bay_addr < pus_bottom_bay_addr;
		  pus_cur_bay_addr++
		  ) {
			/** threshold check **/
			if( ((*pus_cur_bay_addr) <= us_threshold_high)
			 && ((*pus_cur_bay_addr) >= us_threshold_low)
			 ) {
				/* pixel is OK. */
				continue;
			}

			/* pixel is BAD! */
			bf_iq_dpc_detect_sw_write_tbl_1pix( pus_cur_bay_addr );
		}
	}
}


/* algolithm pattern 1 */
/*
    <BayerData>
       |   |   |   |
    ---+---+---+---+---
       |   |   |   |
    ---+---+---+---+---
     A |   | P |   |
    ---+---+---+---+---
       |   |   |   |
    ---+---+---+---+---
       |   |   |   |

    if (FJ_abs(A - P) > TH) { // ... Path1
        P is Badpix;
    }

    A is last OK pixel value.
*/
static VOID bf_iq_dpc_detect_sw_1line_algo1( const UINT32 ui_cur_row )
{
	register USHORT us_threshold_high;
	register const USHORT* pus_cur_pix_bay_addr;
	register const USHORT* pus_bottom_bay_addr;
	USHORT us_corner_column_idx;
	USHORT us_last_ok_pixval[2];	/* even & odd pixel */
	UINT32 ui_cur_pix_even_odd;

	pus_top_bay_addr = pus_bayer16_addr
					 + (pt_detect_info->us_global_w_pixels * ui_cur_row);

	us_last_ok_pixval[0] = *(pus_top_bay_addr + (us_corner_column_top[0] -2));
	us_last_ok_pixval[1] = *(pus_top_bay_addr + (us_corner_column_top[0] -1));
	ui_cur_pix_even_odd = 0;


	for( us_corner_column_idx = 0; us_corner_column_idx < 3; us_corner_column_idx++ ) {
		pus_bottom_bay_addr = pus_top_bay_addr
							+ us_corner_column_bottom[us_corner_column_idx];

		us_threshold_high = us_corner_threshold_high[us_corner_column_idx];

		/* loop by image columns */
		for( pus_cur_pix_bay_addr    = pus_top_bay_addr +  us_corner_column_top[us_corner_column_idx];
		  pus_cur_pix_bay_addr < pus_bottom_bay_addr;
		  pus_cur_pix_bay_addr++,
		  ui_cur_pix_even_odd ^= 1
		  ) {
			/** threshold check **/
			if( us_last_ok_pixval[ui_cur_pix_even_odd] > (*pus_cur_pix_bay_addr) ) {
				if( (us_last_ok_pixval[ui_cur_pix_even_odd] - (*pus_cur_pix_bay_addr)) <= us_threshold_high ) {
					/* pixel is OK. */
					us_last_ok_pixval[ui_cur_pix_even_odd] = *pus_cur_pix_bay_addr;
					continue;
				}
			} else {
				if( ((*pus_cur_pix_bay_addr) - us_last_ok_pixval[ui_cur_pix_even_odd]) <= us_threshold_high ) {
					/* pixel is OK. */
					us_last_ok_pixval[ui_cur_pix_even_odd] = *pus_cur_pix_bay_addr;
					continue;
				}
			}

			/* pixel is BAD! */
			bf_iq_dpc_detect_sw_write_tbl_1pix( pus_cur_pix_bay_addr );
		}
	}
}


/* algolithm pattern 2 */
/*
    <BayerData>
       |   | B |   |
    ---+---+---+---+---
       |   |   |   |
    ---+---+---+---+---
   A,A'|   | P |   | C
    ---+---+---+---+---
       |   |   |   |
    ---+---+---+---+---
       |   | D |   |


    if (FJ_abs(A - P) > TH) { // ... Path1
        if (FJ_abs(Average(A',B,C,D) - P) > TH) { // ... Path2
            P is Badpix;
        }
    }

    A is last OK pixel value.
    A' is P -2 position pixel value.
*/
static VOID bf_iq_dpc_detect_sw_1line_algo2( const UINT32 ui_cur_row )
{
	USHORT us_threshold_high;
	register USHORT us_threshold_low;
	register const USHORT* pus_cur_pix_bay_addr;
	register const USHORT* pus_bottom_bay_addr;
	UINT32	ui_nearby_pixs_val;
	UINT32	ui_cur_column;
	const USHORT* pus_prev_2line_top_bay_addr;
	const USHORT* pus_next_2line_top_bay_addr;
	USHORT us_corner_column_idx;
	USHORT us_last_ok_pixval[2];	/* even & odd pixel */
	UINT32 ui_cur_pix_even_odd;

	pus_top_bay_addr = pus_bayer16_addr
					 + (pt_detect_info->us_global_w_pixels * ui_cur_row);

	pus_prev_2line_top_bay_addr = pus_bayer16_addr
					 + (pt_detect_info->us_global_w_pixels * (ui_cur_row -2));
	pus_next_2line_top_bay_addr = pus_bayer16_addr
					 + (pt_detect_info->us_global_w_pixels * (ui_cur_row +2));

	us_last_ok_pixval[0] = *(pus_top_bay_addr + (us_corner_column_top[0] -2));
	us_last_ok_pixval[1] = *(pus_top_bay_addr + (us_corner_column_top[0] -1));
	ui_cur_pix_even_odd = 0;


	for( us_corner_column_idx = 0; us_corner_column_idx < 3; us_corner_column_idx++ ) {
		pus_bottom_bay_addr = pus_top_bay_addr
							+ us_corner_column_bottom[us_corner_column_idx];

		us_threshold_high = us_corner_threshold_high[us_corner_column_idx];
		us_threshold_low = us_corner_threshold_low[us_corner_column_idx];

		/* loop by image columns */
		for( pus_cur_pix_bay_addr    = pus_top_bay_addr + us_corner_column_top[us_corner_column_idx];
		  pus_cur_pix_bay_addr < pus_bottom_bay_addr;
		  pus_cur_pix_bay_addr++,
		  ui_cur_pix_even_odd ^= 1
		  ) {
			/** threshold check(path1) **/
			if( us_last_ok_pixval[ui_cur_pix_even_odd] > (*pus_cur_pix_bay_addr) ) {
				if( (us_last_ok_pixval[ui_cur_pix_even_odd] - (*pus_cur_pix_bay_addr)) <= us_threshold_low ) {
					/* pixel is OK. */
					us_last_ok_pixval[ui_cur_pix_even_odd] = *pus_cur_pix_bay_addr;
					continue;
				}
			} else {
				if( ((*pus_cur_pix_bay_addr) - us_last_ok_pixval[ui_cur_pix_even_odd]) <= us_threshold_low ) {
					/* pixel is OK. */
					us_last_ok_pixval[ui_cur_pix_even_odd] = *pus_cur_pix_bay_addr;
					continue;
				}
			}



			/** threshold check(path2) **/

			/* calc average of nearby pixs val */
			ui_cur_column = ((UINT32)pus_cur_pix_bay_addr - (UINT32)pus_top_bay_addr) / D_BF_IQ_BAYER_ONE_PIXEL_BYTES;

			ui_nearby_pixs_val = (UINT32)(*(pus_cur_pix_bay_addr -2));
			ui_nearby_pixs_val += (UINT32)(*(pus_cur_pix_bay_addr +2));
			ui_nearby_pixs_val += (UINT32)(*(pus_prev_2line_top_bay_addr + ui_cur_column));
			ui_nearby_pixs_val += (UINT32)(*(pus_next_2line_top_bay_addr + ui_cur_column));

			ui_nearby_pixs_val /= 4;

			if( ui_nearby_pixs_val > (*pus_cur_pix_bay_addr) ) {
				if( (ui_nearby_pixs_val - (*pus_cur_pix_bay_addr)) <= us_threshold_high ) {
					/* pixel is OK. */
					us_last_ok_pixval[ui_cur_pix_even_odd] = *pus_cur_pix_bay_addr;
					continue;
				}
			} else {
				if( ((*pus_cur_pix_bay_addr) - ui_nearby_pixs_val) <= us_threshold_high ) {
					/* pixel is OK. */
					us_last_ok_pixval[ui_cur_pix_even_odd] = *pus_cur_pix_bay_addr;
					continue;
				}
			}


			/* pixel is BAD! */
			bf_iq_dpc_detect_sw_write_tbl_1pix( pus_cur_pix_bay_addr );
		}
	}
}


/* algolithm pattern 3 */
/*
    <BayerData>
       |   | B |   |
    ---+---+---+---+---
       |   |   |   |
    ---+---+---+---+---
   A,A'|   | P |   | C
    ---+---+---+---+---
       |   |   |   |
    ---+---+---+---+---
       |   | D |   |

    TH_Luminance ... threshold_low(0-4095)
    TH_Ratio ....... threshold_high(0-100%)

    if (FJ_abs(A - P) > TH_Luminance) { // ... Path1
        if (FJ_abs((P *100 / Average(A',B,C,D)) -100) > TH_Ratio) {
            P is Badpix;
        }
    }

    A is last OK pixel value.
    A' is P -2 position pixel value.
*/
static VOID bf_iq_dpc_detect_sw_1line_algo3( const UINT32 ui_cur_row )
{
	register USHORT us_threshold_low;
	register USHORT us_threshold_high;
	register const USHORT* pus_cur_pix_bay_addr;
	register const USHORT* pus_bottom_bay_addr;
	UINT32	ui_nearby_pixs_val;
	UINT32	ui_cur_column;
	const USHORT* pus_prev_2line_top_bay_addr;
	const USHORT* pus_next_2line_top_bay_addr;
	USHORT us_corner_column_idx;
	USHORT us_last_ok_pixval[2];	/* even & odd pixel */
	UINT32 ui_cur_pix_even_odd;

	pus_top_bay_addr = pus_bayer16_addr
					 + (pt_detect_info->us_global_w_pixels * ui_cur_row);

	pus_prev_2line_top_bay_addr = pus_bayer16_addr
					 + (pt_detect_info->us_global_w_pixels * (ui_cur_row -2));
	pus_next_2line_top_bay_addr = pus_bayer16_addr
					 + (pt_detect_info->us_global_w_pixels * (ui_cur_row +2));

	us_last_ok_pixval[0] = *(pus_top_bay_addr + (us_corner_column_top[0] -2));
	us_last_ok_pixval[1] = *(pus_top_bay_addr + (us_corner_column_top[0] -1));
	ui_cur_pix_even_odd = 0;


	/* loop by 0:left_corner/1:center/2:right_corner */
	for( us_corner_column_idx = 0; us_corner_column_idx < 3; us_corner_column_idx++ ) {
		pus_bottom_bay_addr = pus_top_bay_addr
							+ us_corner_column_bottom[us_corner_column_idx];

		/* path1 threthold(luminance 0-4095) */
		us_threshold_low = us_corner_threshold_low[us_corner_column_idx];

		/* path2 threthold(ratio 0-100) */
		us_threshold_high = us_corner_threshold_high[us_corner_column_idx];


		/* loop by image columns */
		for( pus_cur_pix_bay_addr    = pus_top_bay_addr + us_corner_column_top[us_corner_column_idx];
		  pus_cur_pix_bay_addr < pus_bottom_bay_addr;
		  pus_cur_pix_bay_addr++,
		  ui_cur_pix_even_odd ^= 1
		  ) {
			/** path1: threshold(luminance) check **/
			if( us_last_ok_pixval[ui_cur_pix_even_odd] > (*pus_cur_pix_bay_addr) ) {
				if( (us_last_ok_pixval[ui_cur_pix_even_odd] - (*pus_cur_pix_bay_addr)) <= us_threshold_low ) {
					/* pixel is OK. */
					us_last_ok_pixval[ui_cur_pix_even_odd] = *pus_cur_pix_bay_addr;
					continue;
				}
#ifdef CO_BF_IQ_DPC_PERCENT_DEBUGLOG
				printf( "algo3 path1-1: row=%u col=%u befo=%u cur=%u thresh=%u\n",
						ui_cur_row,
						((UINT32)pus_cur_pix_bay_addr - (UINT32)pus_top_bay_addr) / D_BF_IQ_BAYER_ONE_PIXEL_BYTES,
						us_last_ok_pixval[ui_cur_pix_even_odd],
						*pus_cur_pix_bay_addr,
						us_threshold_low
					  );
#endif //CO_BF_IQ_DPC_PERCENT_DEBUGLOG
			} else {
				if( ((*pus_cur_pix_bay_addr) - us_last_ok_pixval[ui_cur_pix_even_odd]) <= us_threshold_low ) {
					/* pixel is OK. */
					us_last_ok_pixval[ui_cur_pix_even_odd] = *pus_cur_pix_bay_addr;
					continue;
				}
#ifdef CO_BF_IQ_DPC_PERCENT_DEBUGLOG
				printf( "algo3 path1-2: row=%u col=%u befo=%u cur=%u thresh=%u\n",
						ui_cur_row,
						((UINT32)pus_cur_pix_bay_addr - (UINT32)pus_top_bay_addr) / D_BF_IQ_BAYER_ONE_PIXEL_BYTES,
						us_last_ok_pixval[ui_cur_pix_even_odd],
						*pus_cur_pix_bay_addr,
						us_threshold_low
					  );
#endif //CO_BF_IQ_DPC_PERCENT_DEBUGLOG
			}



			/** path2: threshold(ratio) check **/

			/* calc average of nearby pixs val */
			ui_cur_column = ((UINT32)pus_cur_pix_bay_addr - (UINT32)pus_top_bay_addr) / D_BF_IQ_BAYER_ONE_PIXEL_BYTES;

			ui_nearby_pixs_val = (UINT32)(*(pus_cur_pix_bay_addr -2));
			ui_nearby_pixs_val += (UINT32)(*(pus_cur_pix_bay_addr +2));
			ui_nearby_pixs_val += (UINT32)(*(pus_prev_2line_top_bay_addr + ui_cur_column));
			ui_nearby_pixs_val += (UINT32)(*(pus_next_2line_top_bay_addr + ui_cur_column));

			ui_nearby_pixs_val /= 4;

			if( ui_nearby_pixs_val > (*pus_cur_pix_bay_addr) ) {
				if( (100 - (((*pus_cur_pix_bay_addr) *100) / ui_nearby_pixs_val)) <= us_threshold_high ) {
					/* pixel is OK. */
					us_last_ok_pixval[ui_cur_pix_even_odd] = *pus_cur_pix_bay_addr;
					continue;
				}
#ifdef CO_BF_IQ_DPC_PERCENT_DEBUGLOG
				printf( "algo3 path2-1: row=%u col=%u near=%u cur=%u calc=%u threth=%u\n",
						ui_cur_row,
						ui_cur_column,
						ui_nearby_pixs_val,
						*pus_cur_pix_bay_addr,
						(100 - (((*pus_cur_pix_bay_addr) *100) / ui_nearby_pixs_val)),
						us_threshold_high
					  );
#endif //CO_BF_IQ_DPC_PERCENT_DEBUGLOG
			} else {
				if( ((((*pus_cur_pix_bay_addr) *100) / ui_nearby_pixs_val) - 100) <= us_threshold_high ) {
					/* pixel is OK. */
					us_last_ok_pixval[ui_cur_pix_even_odd] = *pus_cur_pix_bay_addr;
					continue;
				}
#ifdef CO_BF_IQ_DPC_PERCENT_DEBUGLOG
				printf( "algo3 path2-2: row=%u col=%u near=%u cur=%u calc=%u threth=%u\n",
						ui_cur_row,
						ui_cur_column,
						ui_nearby_pixs_val,
						*pus_cur_pix_bay_addr,
						((((*pus_cur_pix_bay_addr) *100) / ui_nearby_pixs_val) - 100),
						us_threshold_high
					  );
#endif //CO_BF_IQ_DPC_PERCENT_DEBUGLOG
			}

			/* pixel is BAD! */
			bf_iq_dpc_detect_sw_write_tbl_1pix( pus_cur_pix_bay_addr );
		}
	}
}


static VOID bf_iq_dpc_detect_sw_calc_corner_threshold( const USHORT ui_cur_row, const FJ_IQ_CORNER_THRESHOLD* const pt_thresh_info )
{
	/* calc left corner absolute row and columns */
	if( ui_cur_row < (pt_detect_info->us_upper_trim + pt_thresh_info->t_left_top.us_v) ) {
		/* within left-top corner */
		us_corner_column_top[0]    = pt_detect_info->us_left_trim;
		us_corner_column_bottom[0] = pt_detect_info->us_left_trim + pt_thresh_info->t_left_top.us_h;
		us_corner_column_top[1]    = pt_detect_info->us_left_trim + pt_thresh_info->t_left_top.us_h;
		us_corner_threshold_high[0]= pt_thresh_info->t_left_top.us_threshold_high;
		us_corner_threshold_low[0] = pt_thresh_info->t_left_top.us_threshold_low;
		us_corner_threshold_high[1]= pt_detect_info->us_threshold_high;
		us_corner_threshold_low[1] = pt_detect_info->us_threshold_low;
	} else
	if( ui_cur_row > (pt_detect_info->us_upper_trim + pt_detect_info->us_h_pixels - pt_thresh_info->t_left_bottom.us_v) ) {
		/* within left-bottom corner */
		us_corner_column_top[0]    = pt_detect_info->us_left_trim;
		us_corner_column_bottom[0] = pt_detect_info->us_left_trim + pt_thresh_info->t_left_bottom.us_h;
		us_corner_column_top[1]    = pt_detect_info->us_left_trim + pt_thresh_info->t_left_bottom.us_h;
		us_corner_threshold_high[0]= pt_thresh_info->t_left_bottom.us_threshold_high;
		us_corner_threshold_low[0] = pt_thresh_info->t_left_bottom.us_threshold_low;
		us_corner_threshold_high[1]= pt_detect_info->us_threshold_high;
		us_corner_threshold_low[1] = pt_detect_info->us_threshold_low;
	} else {
		/* without left-top and left-bottom corners */
		us_corner_column_top[0]    = pt_detect_info->us_left_trim;
		us_corner_column_bottom[0] = pt_detect_info->us_left_trim;
		us_corner_column_top[1]    = pt_detect_info->us_left_trim;
		us_corner_threshold_high[0]= pt_detect_info->us_threshold_high;
		us_corner_threshold_low[0] = pt_detect_info->us_threshold_low;
		us_corner_threshold_high[1]= pt_detect_info->us_threshold_high;
		us_corner_threshold_low[1] = pt_detect_info->us_threshold_low;
	}

	/* calc right corner absolute row and columns */
	if( ui_cur_row < (pt_detect_info->us_upper_trim + pt_thresh_info->t_right_top.us_v) ) {
		/* within right-top corner */
		us_corner_column_bottom[1] = pt_detect_info->us_left_trim + pt_detect_info->us_w_pixels - pt_thresh_info->t_right_top.us_h;
		us_corner_column_top[2]    = pt_detect_info->us_left_trim + pt_detect_info->us_w_pixels - pt_thresh_info->t_right_top.us_h;
		us_corner_column_bottom[2] = pt_detect_info->us_left_trim + pt_detect_info->us_w_pixels;
		us_corner_threshold_high[2]= pt_thresh_info->t_right_top.us_threshold_high;
		us_corner_threshold_low[2] = pt_thresh_info->t_right_top.us_threshold_low;
	} else
	if( ui_cur_row > (pt_detect_info->us_upper_trim + pt_detect_info->us_h_pixels - pt_thresh_info->t_right_bottom.us_v) ) {
		/* within right-bottom corner */
		us_corner_column_bottom[1] = pt_detect_info->us_left_trim + pt_detect_info->us_w_pixels - pt_thresh_info->t_right_bottom.us_h;
		us_corner_column_top[2]    = pt_detect_info->us_left_trim + pt_detect_info->us_w_pixels - pt_thresh_info->t_right_bottom.us_h;
		us_corner_column_bottom[2] = pt_detect_info->us_left_trim + pt_detect_info->us_w_pixels;
		us_corner_threshold_high[2]= pt_thresh_info->t_right_bottom.us_threshold_high;
		us_corner_threshold_low[2] = pt_thresh_info->t_right_bottom.us_threshold_low;
	} else {
		us_corner_column_bottom[1] = pt_detect_info->us_left_trim + pt_detect_info->us_w_pixels;
		us_corner_column_top[2]    = pt_detect_info->us_left_trim + pt_detect_info->us_w_pixels;
		us_corner_column_bottom[2] = pt_detect_info->us_left_trim + pt_detect_info->us_w_pixels;
		us_corner_threshold_high[2]= pt_detect_info->us_threshold_high;
		us_corner_threshold_low[2] = pt_detect_info->us_threshold_low;
	}
}


//
// For BF_IQ_Dpc_Detect_Sw_Merge_Tbl()
//
static BOOL bf_iq_dpc_detect_sw_is_equal_addr( const U_BF_IQ_DPC_TBL_DATA* const pt_defect_tbl, UINT32 ui_last_writed_row_address, const UINT32 ui_addr )
{
	if( ui_last_writed_row_address == 0xffffffff ) {
		ui_last_writed_row_address = 0;
	}

	if( (pt_defect_tbl->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_REL)
	 && (pt_defect_tbl->rel.ui_data_type_id == D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE)
	 && ((pt_defect_tbl->rel.ui_addr + ui_last_writed_row_address) == ui_addr)
	 ) {
		return TRUE;
	}

	if( (pt_defect_tbl->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_ABS)
	 && (pt_defect_tbl->rel.ui_data_type_id == D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE)
	 && (pt_defect_tbl->rel.ui_addr == ui_addr)
	 ) {
		return TRUE;
	}

	return FALSE;
}


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

/** @brief			BF_IQ_Dpc_Detect_Sw_Run
 *	@param[inout]	pt_detect_info			detect parameters structure
 *	@retval			FJ_ERR_OK  success.
 *	@retval			FJ_ERR_NG  parameter errror.
 *	@note			None
 *	@attention		None
 */
FJ_ERR_CODE BF_IQ_Dpc_Detect_Sw_Run( T_BF_IQ_DPC_DETECT_INFO* const pt_detect_info_arg )
{
	UINT32 ui_all_pixels;
	UINT32 ui_cur_row = 0;				/* current row number(physical) */
	UINT32 ui_cur_row_by_cycle;			/* current row number by cycle(multiple cycle val) */
	UINT32 ui_cur_field;
	UINT32 ui_line_order_tbl_maxnum;
	UINT32 ui_line_order_tbl_idx;
	USHORT us_field_separate_row_cycle;
	const T_FJ_IQ_DPC_LINE_ORDER_IN_FIELDS* pt_line_order_in_fields_tbl;
	const USHORT* pus_line_order_tbl_addr;
	E_IQ_DETECT_ALGORITHM e_detect_algorithm;
	const FJ_IQ_CORNER_THRESHOLD* pt_corner_thresh_info;


	pt_detect_info = pt_detect_info_arg;

	if( pt_detect_info == NULL ) {
		return FJ_ERR_IQ_BP_DETECT_INTERNAL1;	// log3_1.txt
	}
	if( pt_detect_info->pus_bayer16_addr == NULL ) {
		return FJ_ERR_IQ_BP_DETECT_INTERNAL1;	// log3_2.txt
	}
	if( pt_detect_info->ui_bayer_byte == 0 ) {
		return FJ_ERR_IQ_BP_DETECT_INTERNAL1;	// log3_3.txt
	}
	if( pt_detect_info->ull_defect_info_tbl_byte == 0 ) {
		return FJ_ERR_IQ_BP_DETECT_INTERNAL1;	// log3_4.txt
	}
	if( pt_detect_info->us_global_h_pixels == 0 ) {
		return FJ_ERR_IQ_BP_DETECT_INTERNAL1;	// log3_5.txt
	}
	if( pt_detect_info->us_h_pixels == 0 ) {
		return FJ_ERR_IQ_BP_DETECT_INTERNAL1;	// log3_6.txt
	}
	if( pt_detect_info->pt_line_order_table->us_line_order_tbl_num != pt_detect_info->ui_field_num ) {
		return FJ_ERR_IQ_BP_DETECT_INTERNAL1;	// log3_7.txt
	}

	pus_bayer16_addr = (USHORT*)M_BF_IQ_DCACHE_ADDR((UINT32)pt_detect_info->pus_bayer16_addr);
	BF_IQ_Dcache_Flush( (ULONG)pus_bayer16_addr, pt_detect_info->ui_bayer_byte );

	ui_all_pixels = pt_detect_info->ui_bayer_byte / D_BF_IQ_BAYER_ONE_PIXEL_BYTES;
	if( ui_all_pixels != (pt_detect_info->us_global_h_pixels * pt_detect_info->us_global_w_pixels) ) {
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(( "error: unmatched bayer data size. hight=%u width=%u pixels=%u\n",
				pt_detect_info->us_global_h_pixels,
				pt_detect_info->us_global_w_pixels,
				ui_all_pixels ));
#endif

		return FJ_ERR_IQ_BP_DETECT_INTERNAL2;	// log3_8.txt
	}

	e_detect_algorithm = pt_detect_info->e_detect_mode;
	pt_corner_thresh_info = pt_detect_info->pt_corner_thresh;

	pus_defect_info_tbl = (USHORT*)pt_detect_info->pt_defect_info_tbl_addr;
	ui_defect_info_tbl_idx_max = M_IQ_ROUNDDOWN_8(pt_detect_info->ull_defect_info_tbl_byte) / D_BF_IQ_DPC_TBL_SIZEOF_DEFECT_INFO_DATA;
							/* round down for easy to ui_defect_info_tbl_idx round up 8byte */
	ui_defect_info_tbl_idx = 0;
	pt_detect_info->ui_detect_count = 0;
	pt_detect_info->ui_over4succession_count = 0;
	pt_detect_info->ui_detect_info_tbl_overflow = 0;
	us_field_separate_row_cycle = pt_detect_info->pt_line_order_table->us_field_separate_row_cycle;
	pt_line_order_in_fields_tbl = pt_detect_info->pt_line_order_table->pt_line_order_in_fields_tbl;


	/* loop by PRO fields */
	for( ui_cur_field = 0; ui_cur_field < pt_detect_info->ui_field_num; ui_cur_field++ ) {
		ui_last_writed_row_address = 0xffffffff;
		ui_defect_info_tbl_idx_by_field = 0;
		pus_last_bad_pix_bay_addr[E_EO_EVEN] = (USHORT*)0xffffffff;
		pus_last_bad_pix_bay_addr[E_EO_ODD] = (USHORT*)0xffffffff;

		ui_line_order_tbl_maxnum = pt_line_order_in_fields_tbl[ui_cur_field].ui_table_num;
		pus_line_order_tbl_addr = pt_line_order_in_fields_tbl[ui_cur_field].pus_line_order_in_field_tbl;

		if( (ui_cur_field %2) == 0 ) {
			g_vd_ofs = BF_IQ_Dpc_Calc_Sta_y_Progr2Intr( E_BF_IQ_FIELD_EVEN, pt_detect_info->us_vd_ofs );
		}
		else {
			g_vd_ofs = BF_IQ_Dpc_Calc_Sta_y_Progr2Intr( E_BF_IQ_FIELD_ODD, pt_detect_info->us_vd_ofs );
		}

		/* loop by image line(in PRO 1field) */
		for( ui_cur_row_by_cycle = 0, ui_cur_row_in_field = 0;
		  ;
		  ui_cur_row_by_cycle += us_field_separate_row_cycle
		  ) {


			for( ui_line_order_tbl_idx = 0;
			  ui_line_order_tbl_idx < ui_line_order_tbl_maxnum;
			  ui_line_order_tbl_idx++, ui_cur_row_in_field++
			  ) {
				ui_cur_row = ui_cur_row_by_cycle + pus_line_order_tbl_addr[ui_line_order_tbl_idx];

				/* skip lower trim(judge part 1 of 2) */
				if( ui_cur_row >= (pt_detect_info->us_upper_trim + pt_detect_info->us_h_pixels) ) {
					break;	/* break ui_cur_row_by_cycle loop */
				}

				/* skip upper trim */
				if( ui_cur_row < pt_detect_info->us_upper_trim ) {
					continue;
				}

				bf_iq_dpc_detect_sw_calc_corner_threshold( ui_cur_row, pt_corner_thresh_info );

#ifdef CO_BF_IQ_DPC_SW_DETECT_DEBUGLOG
				g_us_cur_row = ui_cur_row;
#endif

				if( e_detect_algorithm == E_IQ_DETECT_ALGO_0 ){
					bf_iq_dpc_detect_sw_1line_algo0( ui_cur_row );
				} else if( e_detect_algorithm == E_IQ_DETECT_ALGO_1 ){
					bf_iq_dpc_detect_sw_1line_algo1( ui_cur_row );
				} else if( e_detect_algorithm == E_IQ_DETECT_ALGO_2 ){
					bf_iq_dpc_detect_sw_1line_algo2( ui_cur_row );
				} else {
					bf_iq_dpc_detect_sw_1line_algo3( ui_cur_row );
				}
			}

			/* skip lower trim(judge part 2 of 2) */
			if( ui_cur_row >= (pt_detect_info->us_upper_trim + pt_detect_info->us_h_pixels) ) {
				break;
			}
		}


		/** check end by PRO 1field **/

		/* writing 'end of data' marker */
		bf_iq_dpc_detect_sw_write_eod();


		/* index number round up to 8byte alignment for next field(alignment is PRO specification) */
		ui_defect_info_tbl_idx = M_BF_IQ_DPC_TBL_IDX_8BYTE_ROUNDUP(ui_defect_info_tbl_idx);


		/* padding for 8byte alignment */
		ui_defect_info_tbl_idx_by_field = M_BF_IQ_DPC_TBL_IDX_8BYTE_ROUNDUP(ui_defect_info_tbl_idx_by_field);
		/* store table size(bytelen) per PRO 1field */
		pt_detect_info->ull_defect_info_tbl_bytelen[ui_cur_field] = ui_defect_info_tbl_idx_by_field * D_BF_IQ_DPC_TBL_SIZEOF_DEFECT_INFO_DATA;
	}

	/** all PRO fields is checked **/


	/* store total table size(all PRO fields + 8byte alignment padding) */
	pt_detect_info->ull_defect_info_tbl_byte = ui_defect_info_tbl_idx * D_BF_IQ_DPC_TBL_SIZEOF_DEFECT_INFO_DATA;

#ifdef CO_BF_IQ_DPC_SW_DETECT_DEBUGLOG
	BF_Debug_Print_Information(( "DPC SW Detect count = %u\n", pt_detect_info->ui_detect_count ));
#endif

	return FJ_ERR_OK;
}


/** @brief			BF_IQ_Dpc_Detect_Sw_Merge_Tbl
 *	@param[inout]	pt_dpc_tbl_merge		mergeing table parameters structure
 *	@retval			FJ_ERR_OK  success.
 *	@retval			FJ_ERR_NG  parameter errror.
 *	@note			None
 *	@attention		None
 */
FJ_ERR_CODE BF_IQ_Dpc_Detect_Sw_Merge_Tbl( T_BF_IQ_DPC_TBL_MERGE* const pt_dpc_tbl_merge )
{
	const U_BF_IQ_DPC_TBL_DATA* pt_defect_tbl1;
	UINT32 ui_defect_tbl1_idx;
	UINT32 ui_defect_tbl1_idx_max;
	UINT32 ui_last_row1 = 0;
	UINT32 ui_last_col1 = 0;
	const U_BF_IQ_DPC_TBL_DATA* pt_defect_tbl2;
	UINT32 ui_defect_tbl2_idx;
	UINT32 ui_defect_tbl2_idx_max;
	UINT32 ui_last_row2 = 0;
	UINT32 ui_last_col2 = 0;
	U_BF_IQ_DPC_TBL_DATA* pt_defect_info_data = NULL;
	UINT32 ui_defect_info_data_idx;
	UINT32 ui_defect_info_data_idx_max;
	UINT32 ui_cur_column;
	UINT32 ui_cur_row;
	UINT32 ui_last_writed_row_address = 0xffffffff;
	UINT32 ui_last_writed_column_address = 0;
	U_BF_IQ_DPC_TBL_DATA* pt_defect_info_tbl_last[E_EO_MAX];
	UINT32 ui_succession_count[E_EO_MAX] = { 0, 0 };
	E_EO   even_odd;
	UCHAR  uc_is_column_detected = 0;
	UCHAR  uc_is_row_detected = 0;
	USHORT* pus_defect_tbl1 = NULL;
	USHORT* pus_defect_tbl2 = NULL;
	USHORT* pus_defect_info_data = NULL;
	USHORT ui_data_type_id_before;
	USHORT ui_data_type_id;

	if( pt_dpc_tbl_merge == NULL ) {
		return FJ_ERR_IQ_BP_MERGE_INTERNAL1;		// log4_1.txt
	}
	if( pt_dpc_tbl_merge->pv_defect_info_tbl1_addr == NULL ) {
		return FJ_ERR_IQ_BP_MERGE_INTERNAL1;		// log4_2.txt
	}
	if( pt_dpc_tbl_merge->pv_defect_info_tbl2_addr == NULL ) {
		return FJ_ERR_IQ_BP_MERGE_INTERNAL1;		// log4_3.txt
	}
	if( pt_dpc_tbl_merge->pv_defect_info_tbl_out_addr == NULL ) {
		return FJ_ERR_IQ_BP_MERGE_INTERNAL1;		// log4_4.txt
	}
	if( pt_dpc_tbl_merge->ui_defect_info_tbl1_byte == 0 ) {
		return FJ_ERR_IQ_BP_MERGE_INTERNAL1;		// log4_5.txt
	}
	if( pt_dpc_tbl_merge->ui_defect_info_tbl2_byte == 0 ) {
		return FJ_ERR_IQ_BP_MERGE_INTERNAL1;		// log4_6.txt
	}
	if( pt_dpc_tbl_merge->ui_defect_info_tbl_out_byte == 0 ) {
		return FJ_ERR_IQ_BP_MERGE_INTERNAL1;		// log4_7.txt
	}
	if( pt_dpc_tbl_merge->ui_h_pixels == 0 ) {
		return FJ_ERR_IQ_BP_MERGE_INTERNAL1;		// log4_8.txt
	}


	pus_defect_tbl1 = (USHORT*)( M_BF_IQ_DCACHE_ADDR((unsigned long)pt_dpc_tbl_merge->pv_defect_info_tbl1_addr) );
	pus_defect_tbl2 = (USHORT*)( M_BF_IQ_DCACHE_ADDR((unsigned long)pt_dpc_tbl_merge->pv_defect_info_tbl2_addr) );
	pus_defect_info_data = (USHORT*)( M_BF_IQ_DCACHE_ADDR((unsigned long)pt_dpc_tbl_merge->pv_defect_info_tbl_out_addr) );

	ui_defect_tbl1_idx_max = pt_dpc_tbl_merge->ui_defect_info_tbl1_byte / D_BF_IQ_DPC_TBL_SIZEOF_DEFECT_INFO_DATA;
	ui_defect_tbl1_idx = 0;
	ui_defect_tbl2_idx_max = pt_dpc_tbl_merge->ui_defect_info_tbl2_byte / D_BF_IQ_DPC_TBL_SIZEOF_DEFECT_INFO_DATA;
	ui_defect_tbl2_idx = 0;
	ui_defect_info_data_idx_max = pt_dpc_tbl_merge->ui_defect_info_tbl_out_byte / D_BF_IQ_DPC_TBL_SIZEOF_DEFECT_INFO_DATA;
	ui_defect_info_data_idx = 0;

	pt_dpc_tbl_merge->ui_detect_count = 0;
	pt_dpc_tbl_merge->ui_over4succession_count = 0;
	pt_dpc_tbl_merge->ui_detect_info_tbl_out_overflow = 0;



	/* vertical line loop */
	for( ui_cur_row = 0; ui_cur_row < pt_dpc_tbl_merge->ui_h_pixels; ui_cur_row++ ) {
		pt_defect_tbl1 = (U_BF_IQ_DPC_TBL_DATA*)&pus_defect_tbl1[ui_defect_tbl1_idx];
		pt_defect_tbl2 = (U_BF_IQ_DPC_TBL_DATA*)&pus_defect_tbl2[ui_defect_tbl2_idx];
		/* Both of a merging former table reached END_OF_DATA. */
		if( (pt_defect_tbl1->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_OTHER_DATA)
		 && (pt_defect_tbl1->rel.ui_data_type_id == D_BF_IQ_DPC_TBL_DTYPE_END_OF_DATA)
		 && (pt_defect_tbl2->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_OTHER_DATA)
		 && (pt_defect_tbl2->rel.ui_data_type_id == D_BF_IQ_DPC_TBL_DTYPE_END_OF_DATA)
		 ) {
			break;
		}

		/* Both of a merging former table reached table byte terminal. (Though there is no END_OF_DATA. ) */
		if( (ui_defect_tbl1_idx >= ui_defect_tbl1_idx_max)
		 && (ui_defect_tbl2_idx >= ui_defect_tbl2_idx_max)
		 ) {
			break;
		}

		uc_is_row_detected = 0;
		if( bf_iq_dpc_detect_sw_is_equal_addr( pt_defect_tbl1, ui_last_row1, ui_cur_row ) ) {
			if( pt_defect_tbl1->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_ABS ) {
				ui_defect_tbl1_idx += 2;
			}
			else {
				ui_defect_tbl1_idx++;
			}
			ui_last_row1 = ui_cur_row;
			uc_is_row_detected = 1;
			pt_defect_tbl1 = (U_BF_IQ_DPC_TBL_DATA*)&pus_defect_tbl1[ui_defect_tbl1_idx];
		}
		if( bf_iq_dpc_detect_sw_is_equal_addr( pt_defect_tbl2, ui_last_row2, ui_cur_row ) ) {
			if( pt_defect_tbl2->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_ABS ) {
				ui_defect_tbl2_idx += 2;
			}
			else {
				ui_defect_tbl2_idx++;
			}
			ui_last_row2 = ui_cur_row;
			uc_is_row_detected = 1;
			pt_defect_tbl2 = (U_BF_IQ_DPC_TBL_DATA*)&pus_defect_tbl2[ui_defect_tbl2_idx];
		}

		if( uc_is_row_detected == 0 ) {
			continue;
		}

		ui_last_col1 = 0;
		ui_last_col2 = 0;
		ui_last_writed_column_address = 0;
		ui_succession_count[E_EO_EVEN] = 0;	/* Number of continuous defects */
		ui_succession_count[E_EO_ODD] = 0;	/* Number of continuous defects */
		pt_defect_info_tbl_last[E_EO_EVEN] = (U_BF_IQ_DPC_TBL_DATA*)0xffffffff;
		pt_defect_info_tbl_last[E_EO_ODD] = (U_BF_IQ_DPC_TBL_DATA*)0xffffffff;
		/* widthwise line loop */
		for( ui_cur_column = 0; ui_cur_column < pt_dpc_tbl_merge->ui_w_pixels; ui_cur_column++ ) {
			if( (pt_defect_tbl1->rel.ui_format_id != D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_REL)
			 && (pt_defect_tbl1->rel.ui_format_id != D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_ABS)
			 && (pt_defect_tbl2->rel.ui_format_id != D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_REL)
			 && (pt_defect_tbl2->rel.ui_format_id != D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_ABS)
			 ) {
				break;
			}

			if( (ui_defect_tbl1_idx >= ui_defect_tbl1_idx_max)
			 && (ui_defect_tbl2_idx >= ui_defect_tbl2_idx_max)
			 ) {
				break;
			}

			uc_is_column_detected = 0;
			if( (pt_defect_tbl1->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_REL)
			 && ((pt_defect_tbl1->rel.ui_addr + ui_last_col1) == ui_cur_column)
			 && (ui_defect_tbl1_idx < ui_defect_tbl1_idx_max)
			  ) {
				uc_is_column_detected = 1;
				ui_defect_tbl1_idx++;
				pt_defect_tbl1 = (U_BF_IQ_DPC_TBL_DATA*)&pus_defect_tbl1[ui_defect_tbl1_idx];
				ui_last_col1 = ui_cur_column;
			}
			else
			if( (pt_defect_tbl1->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_ABS)
			 && ((pt_defect_tbl1 +1)->abs.ui_addr == ui_cur_column)
			 && (ui_defect_tbl1_idx < ui_defect_tbl1_idx_max)
			  ) {
				uc_is_column_detected = 1;
				ui_defect_tbl1_idx += 2;
				pt_defect_tbl1 = (U_BF_IQ_DPC_TBL_DATA*)&pus_defect_tbl1[ui_defect_tbl1_idx];
				ui_last_col1 = ui_cur_column;
			}
			if( (pt_defect_tbl2->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_REL)
			 && ((pt_defect_tbl2->rel.ui_addr + ui_last_col2) == ui_cur_column)
			 && (ui_defect_tbl2_idx < ui_defect_tbl2_idx_max)
			  ) {
				uc_is_column_detected = 1;
				ui_defect_tbl2_idx++;
				pt_defect_tbl2 = (U_BF_IQ_DPC_TBL_DATA*)&pus_defect_tbl2[ui_defect_tbl2_idx];
				ui_last_col2 = ui_cur_column;
			}
			else
			if( (pt_defect_tbl2->rel.ui_format_id == D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_ABS)
			 && ((pt_defect_tbl2 +1)->abs.ui_addr == ui_cur_column)
			 && (ui_defect_tbl2_idx < ui_defect_tbl2_idx_max)
			  ) {
				uc_is_column_detected = 1;
				ui_defect_tbl2_idx += 2;
				pt_defect_tbl2 = (U_BF_IQ_DPC_TBL_DATA*)&pus_defect_tbl2[ui_defect_tbl2_idx];
				ui_last_col2 = ui_cur_column;
			}

			even_odd = (E_EO)(ui_cur_column % 2);

			if( uc_is_column_detected == 0 ) {
				ui_succession_count[even_odd] = 0;
				continue;
			}


			/* Detected! */
			ui_succession_count[even_odd]++;

			/* Check on continuous pixel */

			/* Vertical address writing */
			if( (ui_last_writed_row_address == 0xffffffff)
			 || (ui_last_writed_row_address != ui_cur_row)
			 ) {
				if( ui_last_writed_row_address == 0xffffffff ) {
					ui_last_writed_row_address = 0;
				}

				if( (ui_cur_row - ui_last_writed_row_address) >= 4096 ) {
					if( (ui_defect_info_data_idx +1) < ui_defect_info_data_idx_max ) {
						pt_defect_info_data = (U_BF_IQ_DPC_TBL_DATA*)&pus_defect_info_data[ui_defect_info_data_idx];
						pt_defect_info_data->rel.ui_format_id    = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_ABS;	/* pgr0689 */
						pt_defect_info_data->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE;
						pt_defect_info_data->rel.ui_addr         = 0;	// dummy
						pt_defect_info_data++;	/* pgr0689 */
						pt_defect_info_data->abs.ui_addr         = ui_cur_row;
						ui_defect_info_data_idx += 2;
						ui_last_writed_row_address = ui_cur_row;
					}
				}
				else {
					if( ui_defect_info_data_idx < ui_defect_info_data_idx_max ) {
						pt_defect_info_data = (U_BF_IQ_DPC_TBL_DATA*)&pus_defect_info_data[ui_defect_info_data_idx];
						pt_defect_info_data->rel.ui_format_id    = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_ROW_MOVE_REL;	/* pgr0689 */
						pt_defect_info_data->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_ROW_MOVE;
						pt_defect_info_data->rel.ui_addr         = ui_cur_row - ui_last_writed_row_address;
						ui_defect_info_data_idx++;
						ui_last_writed_row_address = ui_cur_row;
					}
				}
			}

			/* Horizontal address writing */
			switch( ui_succession_count[even_odd] ) {
			case 0:
#ifdef FJ_DEBUG_PRINT_ON
				BF_Debug_Print_Error(( "error: internal variable invalid. ui_succession_count=0\n" ));
#endif
				return FJ_ERR_IQ_BP_MERGE_INTERNAL2;
			case 1:
				/* Continuousness the first pixel */
				/* Single pixel defect information is stored. */
				ui_data_type_id_before = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_SINGLE;	// first pixel
				ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_SINGLE;
				break;
			case 2:
				/* Continuousness the second pixel */
				ui_data_type_id_before = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_BEFORE; // Addition information to the second pixel in this front side is changed.
				ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER;
				break;
			case 3:
				/* Continuousness the third pixel */
				ui_data_type_id_before = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_CENTER; // Addition information to the third pixel in this front side is changed.
				ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER;
				break;
			case 4:
				/* Continuousness the fourth pixel */
				/* The number of excess pixel of continuousness four pixels is counted. */
				pt_dpc_tbl_merge->ui_over4succession_count += 4;

				ui_data_type_id_before = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_CENTER; // Addition information to the third pixel in this front side is changed.
				ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER;
				break;
			default:
				/* Since continuousness the fifth pixel */
				/* The number of excess pixel of continuousness four pixels is counted. */
				pt_dpc_tbl_merge->ui_over4succession_count++;

				ui_data_type_id_before = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_CENTER; // Addition information to the third pixel in this front side is changed.
				ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_DEFECT_AFTER;
				break;
			}

			if( (ui_cur_column - ui_last_writed_column_address) >= 4096 ) {
				if( (ui_defect_info_data_idx +1) < ui_defect_info_data_idx_max ) {
					if( ui_data_type_id_before != D_BF_IQ_DPC_TBL_DTYPE_DEFECT_SINGLE ) {
						pt_defect_info_tbl_last[even_odd]->rel.ui_data_type_id = ui_data_type_id_before;
					}
					pt_defect_info_data = (U_BF_IQ_DPC_TBL_DATA*)&pus_defect_info_data[ui_defect_info_data_idx];
					if( pt_defect_info_data == NULL ) {	 // for pass to PG-Relief check
						return FJ_ERR_NG;
					}
					pt_defect_info_data->rel.ui_format_id    = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_ABS;
					pt_defect_info_data->rel.ui_data_type_id = ui_data_type_id;
					pt_defect_info_data->rel.ui_addr         = 0;	// dummy
					pt_defect_info_data++;	/* pgr0689 */
					pt_defect_info_data->abs.ui_addr         = ui_cur_column;
					ui_defect_info_data_idx += 2;
					ui_last_writed_column_address = ui_cur_column;
				}
			}
			else {
				if( ui_defect_info_data_idx < ui_defect_info_data_idx_max ) {
					if( ui_data_type_id_before != D_BF_IQ_DPC_TBL_DTYPE_DEFECT_SINGLE ) {
						pt_defect_info_tbl_last[even_odd]->rel.ui_data_type_id = ui_data_type_id_before;
					}
					pt_defect_info_data = (U_BF_IQ_DPC_TBL_DATA*)&pus_defect_info_data[ui_defect_info_data_idx];
					if( pt_defect_info_data == NULL ) {	 // for pass to PG-Relief check
						return FJ_ERR_NG;
					}
					pt_defect_info_data->rel.ui_format_id    = D_BF_IQ_DPC_TBL_FORMAT_DEFECT_INFO_REL;
					pt_defect_info_data->rel.ui_data_type_id = ui_data_type_id;
					pt_defect_info_data->rel.ui_addr         = ui_cur_column - ui_last_writed_column_address;
					ui_defect_info_data_idx++;
					ui_last_writed_column_address = ui_cur_column;
				}
			}


			if( pt_defect_info_data != NULL ) {
				pt_defect_info_tbl_last[even_odd] = pt_defect_info_data;
			}
			pt_dpc_tbl_merge->ui_detect_count++;

#ifdef CO_BF_IQ_DPC_SW_MERGE_DEBUGLOG
			BF_Debug_Print_Information(( "X=%u Y=%u\n", ui_last_writed_column_address, ui_last_writed_row_address ));
#endif
		}	// column loop
	}	// row loop



	if( ui_defect_info_data_idx >= ui_defect_info_data_idx_max ) {
		pt_defect_info_data = (U_BF_IQ_DPC_TBL_DATA*)&pus_defect_info_data[ui_defect_info_data_idx -1];
		pt_dpc_tbl_merge->ui_detect_info_tbl_out_overflow = 1;

		pt_defect_info_data->rel.ui_format_id    = D_BF_IQ_DPC_TBL_FORMAT_OTHER_DATA;
		pt_defect_info_data->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_END_OF_DATA;
		pt_defect_info_data->rel.ui_addr         = 0;

	} else {
		pt_defect_info_data = (U_BF_IQ_DPC_TBL_DATA*)&pus_defect_info_data[ui_defect_info_data_idx];
		pt_dpc_tbl_merge->ui_detect_info_tbl_out_overflow = 0;

		pt_defect_info_data->rel.ui_format_id    = D_BF_IQ_DPC_TBL_FORMAT_OTHER_DATA;
		pt_defect_info_data->rel.ui_data_type_id = D_BF_IQ_DPC_TBL_DTYPE_END_OF_DATA;
		pt_defect_info_data->rel.ui_addr         = 0;
		ui_defect_info_data_idx++;
	}


	pt_dpc_tbl_merge->ui_defect_info_tbl_out_byte = ui_defect_info_data_idx * D_BF_IQ_DPC_TBL_SIZEOF_DEFECT_INFO_DATA;

	/* Adjustment 8byte alignment */
	pt_dpc_tbl_merge->ui_defect_info_tbl_out_byte
		= M_IQ_ROUNDUP_8((UINT32)pt_dpc_tbl_merge->ui_defect_info_tbl_out_byte);

//	pt_dpc_tbl_merge->ui_defect_info_tbl_out_byte = 10046594;	// for debug log2_3.txt

	return FJ_ERR_OK;
}

