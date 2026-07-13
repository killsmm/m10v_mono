/**
 * @file		ip_warp_tbl.c
 * @brief		Warp/DZoom's APIs wrapper layer: table manager.
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

// IP header
#include "ip_warp_tbl.h"
#include "ip_warp_custom.h"

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
#define M_PITCH2INT( pitch )		(1 << ((UINT32)(pitch)))



/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

typedef struct {
	BOOL				allocated;
	T_IP_WARP_GRID_INFO	grid_info;
} T_IP_WARP_TBL;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/

static T_IP_WARP_TBL g_ip_warp_tbl[D_IP_WARP_BANK_NUM] = {
	{ .allocated = FALSE },
};
static volatile UINT32 g_ip_warp_tbl_info_last_idx = D_IP_WARP_BANK_NUM -1;	// 1st index is "0".

static T_IP_WARP_TBL_DIVIDED g_ip_warp_tbl_divided[D_IP_WARP_TBL_DIVIDED_PAGENUM] = {
	{ .allocated = FALSE },
};
static volatile UINT32 g_ip_warp_tbl_divided_info_last_idx = 0;  // 1st index is "0".


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
T_IP_WARP_TBL_DIVIDED* ip_warp_div_tbl_alloc( VOID )
{
	T_IP_WARP_TBL_DIVIDED* tbl_info;
	UINT32 tbl_idx;
	UINT32 loopcnt;
	
	tbl_idx = g_ip_warp_tbl_divided_info_last_idx;
	for( loopcnt = 0; loopcnt < D_IP_WARP_TBL_DIVIDED_PAGENUM; loopcnt++ ) {
		
		tbl_idx  = tbl_idx % D_IP_WARP_TBL_DIVIDED_PAGENUM;
		g_ip_warp_tbl_divided_info_last_idx = tbl_idx + 1;
		
		tbl_info = &g_ip_warp_tbl_divided[tbl_idx];
		
		if( tbl_info->allocated == FALSE ) {
			
			tbl_info->allocated = TRUE;
			
			break;
		}
		
		tbl_idx++;
	}
	
	if( loopcnt >= D_IP_WARP_TBL_DIVIDED_PAGENUM ) {
		tbl_info = NULL;
		printf( "%s(): Fail to allocate table.\n", (char*)__func__ );
	}
	
	return tbl_info;
}

VOID ip_warp_div_tbl_free( const T_IP_WARP_TBL_DIVIDED* tbl_info_to_free )
{
	T_IP_WARP_TBL_DIVIDED* tbl_info;
	UINT32 tbl_idx;
	UINT32 loopcnt;
	
	tbl_idx = g_ip_warp_tbl_divided_info_last_idx;
	for( loopcnt = 0; loopcnt < D_IP_WARP_TBL_DIVIDED_PAGENUM; loopcnt++ ) {
		
		tbl_idx  = tbl_idx % D_IP_WARP_TBL_DIVIDED_PAGENUM;
		g_ip_warp_tbl_divided_info_last_idx = tbl_idx + 1;
		
		tbl_info = &g_ip_warp_tbl_divided[tbl_idx];
		
		if( tbl_info == tbl_info_to_free ) {
			
			tbl_info->allocated = FALSE;
			
			break;
		}
		
		tbl_idx++;
	}
	
	if( loopcnt >= D_IP_WARP_TBL_DIVIDED_PAGENUM ) {
		printf( "%s(): Unknown address. 0x%lx\n", __func__, (ULONG)tbl_info_to_free );	/* pgr0822 */
	}
	
	return;
}


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

UINT8 IP_Warp_Tbl_Conv_Logarithm( const UINT32 num )
{
	switch( num ) {
	case 4:
		return 2;
	case 8:
		return 3;
	case 16:
		return 4;
	case 32:
		return 5;
	case 64:
		return 6;
	default:
		return D_IP_WARP_TBL_PITCH_ERR;
	}
}

UINT32 IP_Warp_Tbl_Conv_Power( const UINT8 num )
{
	switch( num ) {
	case 2:
		return 4;
	case 3:
		return 8;
	case 4:
		return 16;
	case 5:
		return 32;
	case 6:
		return 64;
	default:
		return D_IP_WARP_TBL_PITCH_ERR;
	}
}

T_IP_WARP_GRID_INFO* IP_Warp_Tbl_Alloc( const UCHAR demosaic_id )
{
	T_IP_WARP_TBL*	tbl_info;
	UINT32			tbl_idx;
	UINT32			loopcnt;
	
	T_IP_WARP_GRID_INFO* p_grid_info = NULL;

	for( loopcnt = 0, tbl_idx = g_ip_warp_tbl_info_last_idx +1; loopcnt < D_IP_WARP_BANK_NUM; loopcnt++, tbl_idx++ ) {
		tbl_idx = tbl_idx % D_IP_WARP_BANK_NUM;
		tbl_info = &g_ip_warp_tbl[tbl_idx];

		if( tbl_info->allocated == FALSE ) {
			
			tbl_info->allocated = TRUE;
			p_grid_info = &tbl_info->grid_info;
			
			break;
		}
	}
	
	if( p_grid_info != NULL ) {
		
		if ( IP_Warp_Need_To_Divide( demosaic_id ) ) {
			p_grid_info->tbl_divided = ip_warp_div_tbl_alloc();
			if( p_grid_info->tbl_divided == NULL ) {
				
				tbl_info->allocated = FALSE;
				p_grid_info = NULL;
			}
		}
	}
	
	return p_grid_info;
}

VOID IP_Warp_Tbl_Free( const T_IP_WARP_GRID_INFO* grid_info, const UCHAR demosaic_id )
{
	T_IP_WARP_TBL*	tbl_info;
	UINT32			tbl_idx;
	UINT32			loopcnt;

	for( loopcnt = 0, tbl_idx = g_ip_warp_tbl_info_last_idx +1; loopcnt < D_IP_WARP_BANK_NUM; loopcnt++, tbl_idx++ ) {
		tbl_idx = tbl_idx % D_IP_WARP_BANK_NUM;
		tbl_info = &g_ip_warp_tbl[tbl_idx];

		if( grid_info == &tbl_info->grid_info ) {
			tbl_info->allocated = FALSE;
			tbl_info->grid_info.valid = FALSE;

			// Free successful.
			break;
		}
	}
	
	if( loopcnt < D_IP_WARP_BANK_NUM ) {
		
		if ( IP_Warp_Need_To_Divide( demosaic_id ) ) {
			ip_warp_div_tbl_free( grid_info->tbl_divided );
		}
	}
	else {
		printf( "%s(): Unknown address. 0x%lx\n", __func__, (ULONG)grid_info );	/* pgr0822 */
	}
	
	return;
}

// Dump script source array format.
VOID IP_Warp_Tbl_Dump_LgridTable( const AHIP_GDC_LGRID* const lgrid_tbl )
{
	UINT32					x_idx;
	UINT32					y_idx;
	const AHIP_GDC_LNODE*	nodes;

	nodes = lgrid_tbl->nodes;

	for( y_idx = 0; y_idx < lgrid_tbl->height; y_idx++ ) {
		for( x_idx = 0; x_idx < lgrid_tbl->width; x_idx++ ) {
			printf( "[ %4u, %4u, %10.4f, %10.4f ],\n",
					x_idx,
					y_idx,
					nodes->u,
					nodes->v
				  );
			nodes++;
		}
	}

	printf( "width   = %u\n", lgrid_tbl->width );
	printf( "height  = %u\n", lgrid_tbl->height );
	printf( "pitchX  = %u(%u)\n", M_PITCH2INT(lgrid_tbl->pitchX), lgrid_tbl->pitchX );
	printf( "pitchY  = %u(%u)\n", M_PITCH2INT(lgrid_tbl->pitchY), lgrid_tbl->pitchY );
	printf( "outputX = %u\n", lgrid_tbl->outputX );
	printf( "outputY = %u\n", lgrid_tbl->outputY );
	printf( "nodes   = 0x%lx (%u bytes)\n", (ULONG)lgrid_tbl->nodes, sizeof(lgrid_tbl->nodes[0]) * lgrid_tbl->width * lgrid_tbl->height );
//	printf( "affine  = 0x%lx\n", (ULONG)lgrid_tbl->affine );
}

// Dump C source array format.
VOID IP_Warp_Tbl_Dump2_LgridTable( const AHIP_GDC_LGRID* const lgrid_tbl )
{
	UINT32					x_idx;
	UINT32					y_idx;
	const AHIP_GDC_LNODE*	nodes;

	printf( "#include \"ahip.h\"\n\n" );
	printf( "AHIP_GDC_LNODE g_ahip_gdc_node[] = {\n" );

	nodes = lgrid_tbl->nodes;

	for( y_idx = 0; y_idx < lgrid_tbl->height; y_idx++ ) {
		for( x_idx = 0; x_idx < lgrid_tbl->width; x_idx++ ) {
			printf( "[ %4u, %4u, %10.4f, %10.4f ],\n",
					x_idx,
					y_idx,
					nodes->u,
					nodes->v
				  );
			nodes++;
		}
	}
	printf( "};\n" );
	printf( "\n" );
	printf( "AHIP_GDC_LGRID g_ahip_gdc_lgrid = {\n" );
	printf( "\t.width   = %u,\n", lgrid_tbl->width );
	printf( "\t.height  = %u,\n", lgrid_tbl->height );
	printf( "\t.pitchX  = %u,	// %u\n", lgrid_tbl->pitchX, M_PITCH2INT(lgrid_tbl->pitchX) );
	printf( "\t.pitchY  = %u,	// %u\n", lgrid_tbl->pitchY, M_PITCH2INT(lgrid_tbl->pitchY) );
	printf( "\t.outputX = %u,\n", lgrid_tbl->outputX );
	printf( "\t.outputY = %u,\n", lgrid_tbl->outputY );
//	printf( "\t.affine  = NULL\n" );
	printf( "\t.nodes   = &g_ahip_gdc_node[0]\n" );
	printf( "};\n" );
}

