/**
 * @file		ip_warp_dzoom.c
 * @brief		Warp/DZoom's APIs wrapper layer: calc D-Zoom only table.
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

// IP header
#include "ip_warp_dzoom.h"
#include "ip_warp.h"
#include "ip_warp_tbl.h"

// FJ header
#include "fj_host_debug.h"
#include "fj_debug.h"

// Driver header
#include "ddim_typedef.h"

// STD-C header
#include <stdio.h>
#include <string.h>


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/

// for integer only.
#define ip_warp_dzoom_calc_tbl_num( pixs, pitch )	( (((pixs) + pitch -1) / pitch) +1 )


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
/* Local Function																*/
/*==============================================================================*/

static VOID ip_warp_dzoom_calc_nodes( const FJ_WARP_CALC_PARAM* const calc_param, FJ_GE_TBL_INFO* const fj_ge_tbl_info, AHIP_GDC_LNODE* const nodes )
{
	FLOAT	in_sta_x;
	FLOAT	in_sta_y;
	FLOAT	in_pitch_width;
	FLOAT	in_pitch_lines;
	FLOAT	tbl_width;
	FLOAT	tbl_lines;

	tbl_width = ((FLOAT)calc_param->out_img_width) / D_IP_WARP_DZOOM_PITCH_X;
	tbl_lines = ((FLOAT)calc_param->out_img_lines) / D_IP_WARP_DZOOM_PITCH_Y;

	in_sta_x       = calc_param->crop_sta_x;
	in_sta_y       = calc_param->crop_sta_y;
	in_pitch_width = ((FLOAT)calc_param->crop_width) / tbl_width;
	in_pitch_lines = ((FLOAT)calc_param->crop_lines) / tbl_lines;

	for( UINT32 y_idx = 0; y_idx < fj_ge_tbl_info->height; y_idx++ ) {
		for( UINT32 x_idx = 0; x_idx < fj_ge_tbl_info->width; x_idx++ ) {
			nodes[y_idx * fj_ge_tbl_info->width + x_idx].u = in_pitch_width * x_idx + in_sta_x;
			nodes[y_idx * fj_ge_tbl_info->width + x_idx].v = in_pitch_lines * y_idx + in_sta_y;
		}
	}
}


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

INT32 IP_Warp_DZoom_Calc_Table( const FJ_WARP_CALC_PARAM* const calc_param, T_IP_WARP_GRID_INFO* const grid_info )
{
	grid_info->fj_ge_tbl_info.width  = ip_warp_dzoom_calc_tbl_num( calc_param->out_img_width, D_IP_WARP_DZOOM_PITCH_X );
	grid_info->fj_ge_tbl_info.height = ip_warp_dzoom_calc_tbl_num( calc_param->out_img_lines, D_IP_WARP_DZOOM_PITCH_Y );
	grid_info->fj_ge_tbl_info.pitchX = D_IP_WARP_DZOOM_PITCH_X;
	grid_info->fj_ge_tbl_info.pitchY = D_IP_WARP_DZOOM_PITCH_Y;

	ip_warp_dzoom_calc_nodes( calc_param, &grid_info->fj_ge_tbl_info, &grid_info->gdc_node[0].ahip_gdc_node );

	grid_info->fj_ge_tbl_info.ge_tbl_bytes = sizeof(grid_info->gdc_node[0].ahip_gdc_node)
										   * grid_info->fj_ge_tbl_info.width
										   * grid_info->fj_ge_tbl_info.height;

	return D_IP_WARP_ERR_OK;
}

