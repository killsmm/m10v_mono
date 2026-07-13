/**
 * @file		iq_dcache.c
 * @brief		ARM D-cache and SDIU buffer control.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// FJ Header
#include "fj_std.h"

// IQ Header
#include "iq_dcache.h"

// DeviceDriver Header
#include "l1l2cache.h"

// Driver Header
#include "ddim_typedef.h"


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
/* Local Method Definition														*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

FJ_ERR_CODE BF_IQ_Dcache_Clear( ULONG addr, ULONG bytes )	/* pgr0339 */
{
	BF_L1l2cache_Run( (T_L1L2CACHE_INFORMATION) {
			.l1l2cache_type = E_BF_L1L2CACHE_ADDR,
			.l1l2cache_method = E_BF_L1L2CACHE_CLEAN,
			.l1l2cache_address = addr,
			.l1l2cache_size = bytes,
		} );

	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_IQ_Dcache_Flush( ULONG addr, ULONG bytes )	/* pgr0339 */
{
	BF_L1l2cache_Run( (T_L1L2CACHE_INFORMATION) {
			.l1l2cache_type = E_BF_L1L2CACHE_ADDR,
			.l1l2cache_method = E_BF_L1L2CACHE_CLEAN_FLUSH,
			.l1l2cache_address = addr,
			.l1l2cache_size = bytes,
		} );

	return FJ_ERR_OK;
}


