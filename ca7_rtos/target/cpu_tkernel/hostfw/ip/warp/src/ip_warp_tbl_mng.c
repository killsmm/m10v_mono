/**
 * @file		ip_warp_tbl.c
 * @brief		Warp/DZoom's APIs wrapper layer: table manager.
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

// IP header
#include "ip_warp_tbl_mng.h"
#include "ip_warp_tbl.h"
#include "ip_warp_custom.h"

// FJ header
#include "fj_host_debug.h"
#include "fj_debug.h"

// Driver header
#include "ddim_typedef.h"

// STD-C header
#include <stdio.h>


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

// T_IP_WARP_TBL_MNG
typedef struct {
	T_IP_WARP_GRID_INFO*	grid_info_curr;
	T_IP_WARP_GRID_INFO*	grid_info_next;
} T_IP_WARP_TBL_MNG;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/

static T_IP_WARP_TBL_MNG g_ip_warp_grid_info_mng[D_IP_WARP_DEMOSAIC_INSTANCE_NUM] = {
	{ .grid_info_curr = FALSE },
};


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

VOID IP_Warp_Tbl_Mng_Disable( const UCHAR demosaic_id )
{
	T_IP_WARP_TBL_MNG*	tbl_mng;

	if( demosaic_id >= D_IP_WARP_DEMOSAIC_INSTANCE_NUM ) {
		printf( "%s(): error: invalid demosaic_id.\n", __func__ );	/* pgr0822 */
		return;
	}

	tbl_mng = &g_ip_warp_grid_info_mng[demosaic_id];

	if( tbl_mng->grid_info_next != NULL ) {
		IP_Warp_Tbl_Free( tbl_mng->grid_info_next, demosaic_id );
		tbl_mng->grid_info_next = NULL;
	}

	if( tbl_mng->grid_info_curr != NULL ) {
		IP_Warp_Tbl_Free( tbl_mng->grid_info_curr, demosaic_id );
		tbl_mng->grid_info_curr = NULL;
	}
}

VOID IP_Warp_Tbl_Mng_Change_Next( const UCHAR demosaic_id )
{
	T_IP_WARP_TBL_MNG*	tbl_mng;

	if( demosaic_id >= D_IP_WARP_DEMOSAIC_INSTANCE_NUM ) {
		printf( "%s(): error: invalid demosaic_id.\n", __func__ );	/* pgr0822 */
		return;
	}

	tbl_mng = &g_ip_warp_grid_info_mng[demosaic_id];

	if( tbl_mng->grid_info_next == NULL ) {
		printf( "%s(): error: grid_info_next is NULL.\n", __func__ );	/* pgr0822 */
		return;
	}

	if( tbl_mng->grid_info_curr != NULL ) {
		IP_Warp_Tbl_Free( tbl_mng->grid_info_curr, demosaic_id );
	}

	tbl_mng->grid_info_curr = tbl_mng->grid_info_next;
	tbl_mng->grid_info_next = NULL;
}

T_IP_WARP_GRID_INFO* IP_Warp_Tbl_Mng_Get_Next( const UCHAR demosaic_id )
{
	T_IP_WARP_TBL_MNG*	tbl_mng;

	if( demosaic_id >= D_IP_WARP_DEMOSAIC_INSTANCE_NUM ) {
		printf( "%s(): error: invalid demosaic_id.\n", __func__ );	/* pgr0822 */
		return NULL;
	}

	tbl_mng = &g_ip_warp_grid_info_mng[demosaic_id];

	if( tbl_mng->grid_info_next != NULL ) {
		return tbl_mng->grid_info_next;
	}

	tbl_mng->grid_info_next = IP_Warp_Tbl_Alloc( demosaic_id );
	
	return tbl_mng->grid_info_next;
}

T_IP_WARP_GRID_INFO* IP_Warp_Tbl_Mng_Get_Current( const UCHAR demosaic_id )
{
	T_IP_WARP_TBL_MNG*	tbl_mng;

	if( demosaic_id >= D_IP_WARP_DEMOSAIC_INSTANCE_NUM ) {
		printf( "%s(): error: invalid demosaic_id.\n", __func__ );	/* pgr0822 */
		return NULL;
	}

	tbl_mng = &g_ip_warp_grid_info_mng[demosaic_id];

	return tbl_mng->grid_info_curr;
}

