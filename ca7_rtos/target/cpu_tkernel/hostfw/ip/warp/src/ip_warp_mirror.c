/**
 * @file		ip_warp_mirror.c
 * @brief		Warp/DZoom's APIs wrapper layer: convert mirror table.
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

// IP header
#include "ip_warp_mirror.h"
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
/* Local Function																*/
/*==============================================================================*/

static VOID ip_warp_mirror_procss_x_axis( const FJ_GE_TBL_INFO* const fj_ge_tbl_info, const UINT32 right_trim, const UINT32 width, AHIP_GDC_LNODE nodes[width] )
{
	UINT32			x_idx;
	UINT32			x_idx_orig;
	AHIP_GDC_LNODE	nodes_orig[width];

	//
	// Backup original table.
	//
	memcpy( nodes_orig, nodes, sizeof(nodes_orig[0]) * width );


	//
	//                       valid pixel area            mergin area(roundup to pitchX).
	//       |<------------------------------------------>|<>|
	//       0     1     2     3     4     5     6    N-1   N
	//       +-----+-----+-----+-----+-----+-----+-----+-----+ original table.
	//    N    N-1    6     5     4     3     2     1     0
	//    +-----+-----+-----+-----+-----+-----+-----+-----+ mirrored table.  Step1: Calculated No.0~N-1. Linear interplation.
	//    -->|--|<--                                                                No.0 started right side of valid pixel area. (exclude mergin area.)
	//        |
	//        | Step2: Calcuated No.N. Linear interpolated between mirrord N-1 and original 0 value. (Stretch)
	//        V
	//    |<--->|
	//    N    N-1

	//
	// Step1: Calculated No.0~N-1. Linear interplation.
	//
	for( x_idx = 0, x_idx_orig = width -1; x_idx < (width -1); x_idx++, x_idx_orig-- ) {
		nodes[x_idx].u = ((nodes_orig[x_idx_orig -1].u - nodes_orig[x_idx_orig].u) * right_trim / fj_ge_tbl_info->pitchX) + nodes_orig[x_idx_orig].u;
		nodes[x_idx].v = ((nodes_orig[x_idx_orig -1].v - nodes_orig[x_idx_orig].v) * right_trim / fj_ge_tbl_info->pitchX) + nodes_orig[x_idx_orig].v;
	}


	//
	// Step2: Calcuated No.N. Linear interpolated between mirrord N-1 and original 0 value. (Stretch)
	//
	if( right_trim == 0 ) {
		nodes[x_idx].u = nodes_orig[x_idx_orig].u;
		nodes[x_idx].v = nodes_orig[x_idx_orig].v;
	}
	else {
		nodes[x_idx].u = ((nodes_orig[x_idx_orig].u - nodes[x_idx -1].u) * fj_ge_tbl_info->pitchX / (fj_ge_tbl_info->pitchX - right_trim)) + nodes[x_idx -1].u;
		nodes[x_idx].v = ((nodes_orig[x_idx_orig].v - nodes[x_idx -1].v) * fj_ge_tbl_info->pitchX / (fj_ge_tbl_info->pitchX - right_trim)) + nodes[x_idx -1].v;
	}
}

static VOID ip_warp_mirror_procss_y_axis( const T_IP_WARP_GRID_INFO* const grid_info, AHIP_GDC_LNODE* const nodes )
{
	const FJ_GE_TBL_INFO*	fj_ge_tbl_info;
	UINT32					right_trim;

	fj_ge_tbl_info = &grid_info->fj_ge_tbl_info;

	right_trim = grid_info->calc_param.out_img_width % fj_ge_tbl_info->pitchX;
	if( right_trim != 0 ) {
		right_trim = fj_ge_tbl_info->pitchX - right_trim;
	}

	for( UINT32 y_idx = 0; y_idx < fj_ge_tbl_info->height; y_idx++ ) {
		ip_warp_mirror_procss_x_axis( fj_ge_tbl_info, right_trim, fj_ge_tbl_info->width, &nodes[y_idx * fj_ge_tbl_info->width] );
	}
}


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

INT32 IP_Warp_Mirror_Calc_Table( T_IP_WARP_GRID_INFO* const grid_info )
{
	ip_warp_mirror_procss_y_axis( grid_info, &grid_info->gdc_node[0].ahip_gdc_node );

	return D_IP_WARP_ERR_OK;
}

