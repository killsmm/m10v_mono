/**
 * @file		ip_warp.c
 * @brief		Warp's APIs wrapper layer
 * @note		None
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

// IP header
#include "ip_warp.h"
#include "ip_warp_dzoom.h"
#include "ip_warp_mirror.h"
#include "ip_warp_tbl.h"
#include "ip_warp_tbl_mng.h"

// FJ header
#include "fj_host_debug.h"
#include "fj_debug.h"

// Host header
//#include "rs_system_attr.h"

// OS header
#include "os_user_custom.h"

// Driver header
#include "ddim_typedef.h"

// STD-C header
#include <stdio.h>
#include <string.h>
//#include "rs_system_factor.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define D_IP_WARP_DEMOSAIC_ID_MAIN   (0)
#define D_IP_WARP_DEMOSAIC_ID_SUB    (1)

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

static T_IP_WARP_TBL_INFO g_ip_warp_tbl_info[E_IP_WARP_TBLIDX_MAX] = {
	[E_IP_WARP_TBLIDX_WARPING] = {
		.is_loaded     = FALSE,
		.mirror_enable = FALSE,
		.tbl_idx = E_IP_WARP_TBLIDX_WARPING,
	},
	[E_IP_WARP_TBLIDX_LDC0] = {
		.is_loaded     = FALSE,
		.mirror_enable = FALSE,
		.tbl_idx = E_IP_WARP_TBLIDX_LDC0,
	},
	[E_IP_WARP_TBLIDX_LDC1] = {
		.is_loaded     = FALSE,
		.mirror_enable = FALSE,
		.tbl_idx = E_IP_WARP_TBLIDX_LDC1,
	},
	[E_IP_WARP_TBLIDX_LDC2] = {
		.is_loaded     = FALSE,
		.mirror_enable = FALSE,
		.tbl_idx = E_IP_WARP_TBLIDX_LDC2,
	},
	[E_IP_WARP_TBLIDX_LDC3] = {
		.is_loaded     = FALSE,
		.mirror_enable = FALSE,
		.tbl_idx = E_IP_WARP_TBLIDX_LDC3,
	},
	[E_IP_WARP_TBLIDX_DZOOM_MIRROR] = {
		.is_loaded     = TRUE,
		.mirror_enable = FALSE,
		.tbl_idx = E_IP_WARP_TBLIDX_DZOOM_MIRROR,
	},
};
static volatile BOOL g_ip_warp_skip_get_tbl = FALSE;


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/

static INT32 ip_warp_conv_err_code( const FJ_ERR_CODE fj_ercd )
{
	switch( fj_ercd ) {
		case FJ_ERR_OK:
			return D_IP_WARP_ERR_OK;
		case FJ_ERR_NG_FORMAT:
			return D_IP_WARP_ERR_NG_FORMAT;
		case FJ_ERR_OVERFLOW:
			return D_IP_WARP_ERR_OVERFLOW;
		case FJ_ERR_NG_MEMORY:
			return D_IP_WARP_ERR_NG_TABLE;
		case FJ_ERR_NG_INPUT_PARAM:
			return D_IP_WARP_ERR_NOTFOUND;
		default:
			return D_IP_WARP_ERR_NG;	// failsafe
	}
}

static INT32 ip_warp_conv_ge_tbl_info( T_IP_WARP_GRID_INFO* const grid_info )
{
	AHIP_GDC_LGRID* ahip_ge_tbl_info;
	FJ_GE_TBL_INFO* fj_ge_tbl_info;

	ahip_ge_tbl_info = &grid_info->ahip_ge_tbl_info;
	fj_ge_tbl_info = &grid_info->fj_ge_tbl_info;

	ahip_ge_tbl_info->width   = fj_ge_tbl_info->width;
	ahip_ge_tbl_info->height  = fj_ge_tbl_info->height;
	ahip_ge_tbl_info->pitchX  = IP_Warp_Tbl_Conv_Logarithm( fj_ge_tbl_info->pitchX );
	ahip_ge_tbl_info->pitchY  = IP_Warp_Tbl_Conv_Logarithm( fj_ge_tbl_info->pitchY );
	ahip_ge_tbl_info->outputX = 0;	// left-top position.
	ahip_ge_tbl_info->outputY = 0;	// left-top position.
	ahip_ge_tbl_info->nodes   = &grid_info->gdc_node[0].ahip_gdc_node;
//	ahip_ge_tbl_info->affine  = NULL;

	if( ahip_ge_tbl_info->pitchX == D_IP_WARP_TBL_PITCH_ERR ) {
		return D_IP_WARP_ERR_OVERFLOW;
	}
	if( ahip_ge_tbl_info->pitchY == D_IP_WARP_TBL_PITCH_ERR ) {
		return D_IP_WARP_ERR_OVERFLOW;
	}

	return D_IP_WARP_ERR_OK;
}

static VOID ip_warp_copy_node_to_left_table(
	const AHIP_GDC_LNODE* const src_node, T_IP_WARP_TBL_DIVIDED* const dst_tbl_info, UINT32 dst_node_ind )
{
	dst_tbl_info->grid_info[D_IP_WARP_DIVIDED_ID_LEFT].gdc_node[dst_node_ind].ahip_gdc_node = *src_node;
	return;
}

static VOID ip_warp_copy_node_to_right_table(
	const AHIP_GDC_LNODE* const src_node, T_IP_WARP_TBL_DIVIDED* const dst_tbl_info, UINT32 dst_node_ind )
{
	dst_tbl_info->grid_info[D_IP_WARP_DIVIDED_ID_RIGHT].gdc_node[dst_node_ind].ahip_gdc_node = *src_node;
	return;
}


static VOID ip_warp_divide_ge_tbl( T_IP_WARP_GRID_INFO* const src_grid_info )
{
	AHIP_GDC_LGRID* src_ge_tbl_info = &src_grid_info->ahip_ge_tbl_info;
	
	/* Horizontal width of the image after dewarp processing. */
	UINT32 out_img_width = src_grid_info->calc_param.out_img_width;
	
	/* Horizontal width of the image after dewarp processing with left half table.
	   It must be a multiple of 8. */
	UINT32 out_img_width_left = ( out_img_width / 2 ) & (~0x00000007);
	
	/* Horizontal width between coordinates after dewarp processing. */
	UINT32 box_width = IP_Warp_Tbl_Conv_Power(src_ge_tbl_info->pitchX);
	
	/* Horizontal index of the first left coordinate from the center line. */
	UINT32 x0 = out_img_width_left / box_width;
	
	/* The center line divides the line segment connecting 
	   the coordinates [x0, 0] and the coordinates [x0 + 1, 0] into m: n. */
	UINT32 m = out_img_width_left - x0 * box_width;
	UINT32 n = box_width - m;
	
	UINT32 i, j;
	
	UINT32 node_ind_left = 0;
	UINT32 node_ind_right = 0;
	
	T_IP_WARP_TBL_DIVIDED* dst_tbl_info = src_grid_info->tbl_divided;
	
	AHIP_GDC_LGRID* dst_ge_tbl_info_left  = &dst_tbl_info->grid_info[D_IP_WARP_DIVIDED_ID_LEFT].ahip_ge_tbl_info;
	AHIP_GDC_LGRID* dst_ge_tbl_info_right = &dst_tbl_info->grid_info[D_IP_WARP_DIVIDED_ID_RIGHT].ahip_ge_tbl_info;
	
	
	/* Set the horizontal width of the image after dewarp processing with left and right half table. */
	dst_tbl_info->grid_info[D_IP_WARP_DIVIDED_ID_LEFT].out_img_width = out_img_width_left;
	dst_tbl_info->grid_info[D_IP_WARP_DIVIDED_ID_RIGHT].out_img_width = out_img_width - out_img_width_left;
	printf( "%s(): out_img_width_left=%u, out_img_width_right=%u\n", (char*)__func__, 
		dst_tbl_info->grid_info[D_IP_WARP_DIVIDED_ID_LEFT].out_img_width, 
		dst_tbl_info->grid_info[D_IP_WARP_DIVIDED_ID_RIGHT].out_img_width );
	
	if( out_img_width % 0x10 ) {
		printf( "%s(): Warnning: out_img_width(%u) is not a multiple of 16.\n", (char*)__func__, out_img_width );
	}
	
	/* AHIP_GDC_LGRID structure setting. */
	*dst_ge_tbl_info_left  = *src_ge_tbl_info;
	dst_ge_tbl_info_left->width = m == 0 ? x0 + 1 : x0 + 2;
	dst_ge_tbl_info_left->nodes = &dst_tbl_info->grid_info[D_IP_WARP_DIVIDED_ID_LEFT].gdc_node[0].ahip_gdc_node;
	
	*dst_ge_tbl_info_right = *src_ge_tbl_info;
	dst_ge_tbl_info_right->width = src_ge_tbl_info->width - x0;
	dst_ge_tbl_info_right->nodes = &dst_tbl_info->grid_info[D_IP_WARP_DIVIDED_ID_RIGHT].gdc_node[0].ahip_gdc_node;
	
	/* copy or calculate the coordinate positions of the left and right tables. */
	for ( j = 0; j < src_ge_tbl_info->height; j++) {
		for ( i = 0; i < src_ge_tbl_info->width; i++) {
			
			UINT32 node_ind = j * src_ge_tbl_info->width + i;
			const AHIP_GDC_LNODE* this_node = &src_ge_tbl_info->nodes[node_ind];
			const AHIP_GDC_LNODE* prev_node = &src_ge_tbl_info->nodes[node_ind - 1];
			
			
			if ( i <= x0 ) {
				ip_warp_copy_node_to_left_table(this_node, dst_tbl_info, node_ind_left++);
			}
			
			if ( i == x0 + 1 && m != 0 ) {
				ip_warp_copy_node_to_left_table(this_node, dst_tbl_info, node_ind_left++);
			}
			
			if ( x0 <= i ) {
				AHIP_GDC_LNODE new_node;
				AHIP_GDC_LNODE next_node;
				
				if ( i == src_ge_tbl_info->width - 1 ) {
					/* Since there is no next node, the virtual node position is calculated.  */
					next_node.u = 2 * this_node->u - prev_node->u;
					next_node.v = 2 * this_node->v - prev_node->v;
				}
				else {
					next_node = src_ge_tbl_info->nodes[node_ind + 1];
				}
				
				new_node.u = ( n * this_node->u + m * next_node.u ) / ( m + n );	/* pgr0872 */
				new_node.v = ( n * this_node->v + m * next_node.v ) / ( m + n );	/* pgr0872 */
				
				ip_warp_copy_node_to_right_table(&new_node, dst_tbl_info, node_ind_right++);
			}
		}
	}
	
	return;
}

static T_IP_WARP_GRID_INFO_DIVIDED* ip_warp_get_divided_grid_info( const UCHAR demosaic_id, const UCHAR divided_id )
{
	T_IP_WARP_GRID_INFO* grid_info;

	if( g_ip_warp_skip_get_tbl != FALSE ) {
		return NULL;
	}
	
	grid_info = IP_Warp_Tbl_Mng_Get_Current( demosaic_id );
	if( grid_info == NULL || grid_info->valid == FALSE ) {
		
		printf( "%s(): Error: Current warp table can not be used.\n", (char*)__func__ );
		return NULL;
	}
	
	if( divided_id != D_IP_WARP_DIVIDED_ID_LEFT && divided_id != D_IP_WARP_DIVIDED_ID_RIGHT ) {
		
		printf( "%s(): Error: Invalid divided_id: %u.\n", (char*)__func__, divided_id );
		return NULL;
	}
	
	if( grid_info->tbl_divided == NULL || 
		grid_info->tbl_divided->allocated == FALSE ) {
		
		printf( "%s(): Error: Divided warp table can not be used.\n", (char*)__func__ );
		return NULL;
	}
	
	return &grid_info->tbl_divided->grid_info[divided_id];
}


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

INT32 IP_Warp_Notify_LoadingComplete( const E_IP_WARP_TBLIDX tbl_idx, const BYTE* const addr, const ULONG bytes )
{
	FJ_ERR_CODE			fj_ercd;
	INT32				retcd;
	T_IP_WARP_TBL_INFO*	tbl_info;

	tbl_info = &g_ip_warp_tbl_info[tbl_idx];

	fj_ercd = FJ_Notify_Loading_WarpTable_Complete( tbl_info->tbl_idx, addr, bytes );
	retcd = ip_warp_conv_err_code( fj_ercd );
	if( retcd != D_IP_WARP_ERR_OK ) {
		return retcd;
	}

	tbl_info->is_loaded = TRUE;

	return retcd;
}

INT32 IP_Warp_Calc_Table( const UCHAR demosaic_id, const E_IP_WARP_TBLIDX tbl_idx, const FJ_WARP_CALC_PARAM* const calc_param, const BOOL mirror_enable )
{
	FJ_ERR_CODE				fj_ercd;
	INT32					retcd;
	T_IP_WARP_TBL_INFO*		tbl_info;
	T_IP_WARP_GRID_INFO*	grid_info;
//	INT32 					table_length;

	tbl_info  = &g_ip_warp_tbl_info[tbl_idx];

	if( tbl_info->is_loaded == FALSE ) {
		return D_IP_WARP_ERR_NOTFOUND;
	}

	grid_info = IP_Warp_Tbl_Mng_Get_Next( demosaic_id );
	if( grid_info == NULL ) {
		return D_IP_WARP_ERR_TBL_ALLOC;
	}

	grid_info->calc_param = *calc_param;
	grid_info->fj_ge_tbl_info.ge_tbl_addr		= &grid_info->gdc_node[0].fj_ge_cell;
	grid_info->fj_ge_tbl_info.ge_tbl_max_bytes	= sizeof(grid_info->gdc_node);
	grid_info->fj_ge_tbl_info.ge_tbl_bytes		= 0;
	tbl_info->mirror_enable						= mirror_enable;

	switch( tbl_idx ) {
		case E_IP_WARP_TBLIDX_DZOOM_MIRROR:
			retcd = IP_Warp_DZoom_Calc_Table( calc_param, grid_info );
			if( retcd != D_IP_WARP_ERR_OK ) {
				return retcd;
			}

			if( tbl_info->mirror_enable != FALSE ) {
				retcd = IP_Warp_Mirror_Calc_Table( grid_info );
				if( retcd != D_IP_WARP_ERR_OK ) {
					return retcd;
				}
			}
			
			retcd = ip_warp_conv_ge_tbl_info( grid_info );
			if( retcd != D_IP_WARP_ERR_OK ) {
				return retcd;
			}

			grid_info->tbl_info = NULL;
			grid_info->valid = TRUE;
			break;
		default:
			fj_ercd = FJ_Warp_Calc_GeTable( tbl_info->tbl_idx, &grid_info->fj_ge_tbl_info, calc_param );
			retcd = ip_warp_conv_err_code( fj_ercd );
			if( retcd != D_IP_WARP_ERR_OK ) {
				return retcd;
			}

			if( tbl_info->mirror_enable != FALSE ) {
				retcd = IP_Warp_Mirror_Calc_Table( grid_info );
				if( retcd != D_IP_WARP_ERR_OK ) {
					return retcd;
				}
			}
/*//šgoodby_RS_System
			if (RS_System_Factor_Draft_Warping_Parameter_Get(E_RS_SYS_DRAFT_WARP_PARAM_GRID_NODE_WIDTH) != 0) {
				grid_info->fj_ge_tbl_info.width = RS_System_Factor_Draft_Warping_Parameter_Get(E_RS_SYS_DRAFT_WARP_PARAM_GRID_NODE_WIDTH);
			}
			if (RS_System_Factor_Draft_Warping_Parameter_Get(E_RS_SYS_DRAFT_WARP_PARAM_GRID_NODE_HEIGHT) != 0) {
				grid_info->fj_ge_tbl_info.height= RS_System_Factor_Draft_Warping_Parameter_Get(E_RS_SYS_DRAFT_WARP_PARAM_GRID_NODE_HEIGHT);
			}
			if (RS_System_Factor_Draft_Warping_Parameter_Get(E_RS_SYS_DRAFT_WARP_PARAM_GRID_CELL_WIDTH) != 0) {
				grid_info->fj_ge_tbl_info.pitchX= RS_System_Factor_Draft_Warping_Parameter_Get(E_RS_SYS_DRAFT_WARP_PARAM_GRID_CELL_WIDTH);
			}
			if (RS_System_Factor_Draft_Warping_Parameter_Get(E_RS_SYS_DRAFT_WARP_PARAM_GRID_CELL_HEIGHT) != 0) {
				grid_info->fj_ge_tbl_info.pitchY= RS_System_Factor_Draft_Warping_Parameter_Get(E_RS_SYS_DRAFT_WARP_PARAM_GRID_CELL_HEIGHT);
			}

			if (RS_System_Factor_Draft_Warping_Parameter_Get(E_RS_SYS_DRAFT_WARP_PARAM_OUTPUT_IMG_WIDTH) != 0 &&
				RS_System_Factor_Draft_Warping_Parameter_Get(E_RS_SYS_DRAFT_WARP_PARAM_OUTPUT_IMG_HEIGHT) != 0) {
				printf("grid_info->fj_ge_tbl_info.width = %d,grid_info->fj_ge_tbl_info.height= %d,grid_info->fj_ge_tbl_info.pitchX= %d,grid_info->fj_ge_tbl_info.pitchY= %d\n",grid_info->fj_ge_tbl_info.width,grid_info->fj_ge_tbl_info.height,grid_info->fj_ge_tbl_info.pitchX,grid_info->fj_ge_tbl_info.pitchY);
				table_length = (RS_System_Factor_Draft_Warping_Parameter_Get(E_RS_SYS_DRAFT_WARP_PARAM_OUTPUT_IMG_WIDTH)/grid_info->fj_ge_tbl_info.width + 1)*
					(RS_System_Factor_Draft_Warping_Parameter_Get(E_RS_SYS_DRAFT_WARP_PARAM_OUTPUT_IMG_HEIGHT)/grid_info->fj_ge_tbl_info.height + 1);
				for(int i = 0; i < table_length; i++) {
					if (RS_System_Factor_Draft_Warping_Tbl_Node_Get(i).v != 0 &&
						RS_System_Factor_Draft_Warping_Tbl_Node_Get(i).u != 0 ){
						grid_info->gdc_node[i].ahip_gdc_node = RS_System_Factor_Draft_Warping_Tbl_Node_Get(i); 
					}
				}
				grid_info->fj_ge_tbl_info.ge_tbl_bytes= sizeof(grid_info->gdc_node[0].ahip_gdc_node)
																   *table_length;
			}
*/		   
			
			retcd = ip_warp_conv_ge_tbl_info( grid_info );
			if( retcd != D_IP_WARP_ERR_OK ) {
				return retcd;
			}

			grid_info->tbl_info = tbl_info;
			
			/* Divide warp table */
			if ( IP_Warp_Need_To_Divide( demosaic_id ) ) {
				
				printf( "%s(): Warp table need to be divided.\n", (char*)__func__ );
				ip_warp_divide_ge_tbl( grid_info );
			}
			
			grid_info->valid = TRUE;
			break;
	}

	IP_Warp_Tbl_Mng_Change_Next( demosaic_id );

	return retcd;
}

INT32 IP_Warp_Disable_Table( const UCHAR demosaic_id )
{
	IP_Warp_Tbl_Mng_Disable( demosaic_id );

	return D_IP_WARP_ERR_OK;
}

INT32 IP_Warp_Disable_Table_All( VOID )
{
	UCHAR	demosaic_id;

	for( demosaic_id = 0; demosaic_id < D_IP_WARP_DEMOSAIC_INSTANCE_NUM; demosaic_id++ ) {
		IP_Warp_Tbl_Mng_Disable( demosaic_id );
	}

	return D_IP_WARP_ERR_OK;
}

INT32 IP_Warp_Get_Table( const UCHAR demosaic_id, AHIP_GDC_LGRID* const ge_tbl_info )
{
	T_IP_WARP_GRID_INFO*	grid_info;

	if( g_ip_warp_skip_get_tbl != FALSE ) {
		return D_IP_WARP_ERR_NOTFOUND;
	}

	grid_info = IP_Warp_Tbl_Mng_Get_Current( demosaic_id );

	if( grid_info == NULL ) {
		return D_IP_WARP_ERR_NOTFOUND;
	}

	if( grid_info->valid == FALSE ) {
		return D_IP_WARP_ERR_NOTFOUND;
	}

	*ge_tbl_info = grid_info->ahip_ge_tbl_info;

	return D_IP_WARP_ERR_OK;
}

VOID IP_Warp_Disable_GetTable( const BOOL onoff )
{
	g_ip_warp_skip_get_tbl = onoff;
}

BOOL IP_Warp_Table_Available( VOID )
{
	return !g_ip_warp_skip_get_tbl;
}

INT32 IP_Warp_Get_Table_Divided( const UCHAR demosaic_id, AHIP_GDC_LGRID* const ge_tbl_info, const UCHAR divided_id )
{
	T_IP_WARP_GRID_INFO* grid_info;

	if( g_ip_warp_skip_get_tbl != FALSE ) {
		return D_IP_WARP_ERR_NOTFOUND;
	}
	
	grid_info = IP_Warp_Tbl_Mng_Get_Current( demosaic_id );
	if( grid_info == NULL || grid_info->valid == FALSE ) {
		return D_IP_WARP_ERR_NOTFOUND;
	}
	
	if( divided_id == D_IP_WARP_DIVIDED_ID_LEFT || divided_id == D_IP_WARP_DIVIDED_ID_RIGHT ) {
		if( grid_info->tbl_divided == NULL || 
			grid_info->tbl_divided->allocated == FALSE ) {
			
			return D_IP_WARP_ERR_NOTFOUND;
		}
		
		*ge_tbl_info = grid_info->tbl_divided->grid_info[divided_id].ahip_ge_tbl_info;
	}
	else {
		
		*ge_tbl_info = grid_info->ahip_ge_tbl_info;
	}
	
	return D_IP_WARP_ERR_OK;
}

BOOL IP_Warp_Need_To_Divide( const UCHAR demosaic_id )
{
	BOOL need_to_divide =
////šgoodby_RS_System		RS_System_Attr_Get_MainImgPos() == E_RS_SYSTEM_ATTR_MAIN_IMG_POS_CENTER &&
		demosaic_id == D_IP_WARP_DEMOSAIC_ID_SUB;
	
	return need_to_divide;
}

INT32 IP_Warp_Get_Divided_Img_Width( const UCHAR demosaic_id, UINT32* const div_img_width, const UCHAR divided_id )
{
	T_IP_WARP_GRID_INFO_DIVIDED* grid_info = ip_warp_get_divided_grid_info( demosaic_id, divided_id );
	if( grid_info == NULL ) {
		
		return D_IP_WARP_ERR_NOTFOUND;
	}
	
	*div_img_width = grid_info->out_img_width;
	
	return D_IP_WARP_ERR_OK;
}
