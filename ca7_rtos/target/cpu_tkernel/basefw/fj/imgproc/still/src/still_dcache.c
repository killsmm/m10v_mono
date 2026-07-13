/**
 * @file		still_dcache.c
 * @brief		CPU Data-Cache and memory barrier Control functions
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// FJ Header
#include "fj_std.h"
#include "fj_still.h"

// Still header
#include "still_dcache.h"
#include "still_common.h"

// Debug header
#include "debug.h"

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

static ULONG bf_still_dache_calc_fjimg_y_bytes( const FJ_IMG_INFO* const img_info )
{
	switch( img_info->pfmt ) {
		case FJ_IMG_PFMT_YCC422:	// FALLTHROUGH
		case FJ_IMG_PFMT_YCC444:	// FALLTHROUGH
		case FJ_IMG_PFMT_YCC420:	// FALLTHROUGH
		case FJ_IMG_PFMT_YCC400:
			return ((ULONG)img_info->g_y_width) * ((ULONG)img_info->lines);
		case FJ_IMG_PFMT_VIDEO:
			return ((ULONG)img_info->g_y_width) * (ULONG)M_BF_STILL_ROUNDUP_VIDEOFMT_LINES( img_info->lines );
		default:
			BF_Debug_Print_Error(( "bf_still_dache_calc_fjimg_y_bytes: unknown pfmt %u.\n", img_info->pfmt ));
			return 0ul;
	}
}


static ULONG bf_still_dache_calc_fjimg_c_bytes( const FJ_IMG_INFO* const img_info )
{
	switch( img_info->pfmt ) {
		case FJ_IMG_PFMT_YCC422:	// FALLTHROUGH
		case FJ_IMG_PFMT_YCC444:
			return ((ULONG)img_info->g_c_width) * ((ULONG)img_info->lines);
		case FJ_IMG_PFMT_YCC420:
			return ((ULONG)img_info->g_c_width) * ((ULONG)img_info->lines) /2;
		case FJ_IMG_PFMT_YCC400:
			return 0ul;
		case FJ_IMG_PFMT_VIDEO:
			return ((ULONG)img_info->g_c_width) * M_BF_STILL_ROUNDUP_VIDEOFMT_LINES( ((ULONG)img_info->lines) /2 );
		default:
			BF_Debug_Print_Error(( "bf_still_dache_calc_fjimg_c_bytes: unknown pfmt %u.\n", img_info->pfmt ));
			return 0ul;
	}
}


static ULONG bf_still_dache_calc_fjinfoconv_y_bytes( const FJ_IMG_INFO_CONV* const info_conv )
{
	switch( info_conv->pfmt ) {
		case FJ_IMG_PFMT_RGBA8888:	// FALLTHROUGH
		case FJ_IMG_PFMT_RGBA4444:	// FALLTHROUGH
		case FJ_IMG_PFMT_YCC422:	// FALLTHROUGH
		case FJ_IMG_PFMT_YCC444:	// FALLTHROUGH
		case FJ_IMG_PFMT_YCC420:	// FALLTHROUGH
		case FJ_IMG_PFMT_YCC400:
			return ((ULONG)info_conv->g_y_g_width) * ((ULONG)info_conv->lines);
		case FJ_IMG_PFMT_VIDEO:
			return ((ULONG)info_conv->g_y_g_width) * (ULONG)M_BF_STILL_ROUNDUP_VIDEOFMT_LINES( info_conv->lines );
		default:
			BF_Debug_Print_Error(( "%s: unknown pfmt %u.\n", __func__, info_conv->pfmt ));
			return 0ul;
	}
}


static ULONG bf_still_dache_calc_fjinfoconv_c_bytes( const FJ_IMG_INFO_CONV* const info_conv )
{
	switch( info_conv->pfmt ) {
		case FJ_IMG_PFMT_YCC422:	// FALLTHROUGH
		case FJ_IMG_PFMT_YCC444:
			return ((ULONG)info_conv->g_cb_b_width) * ((ULONG)info_conv->lines);
		case FJ_IMG_PFMT_YCC420:
			return ((ULONG)info_conv->g_cb_b_width) * ((ULONG)info_conv->lines) /2;
		case FJ_IMG_PFMT_RGBA8888:	// FALLTHROUGH
		case FJ_IMG_PFMT_RGBA4444:	// FALLTHROUGH
		case FJ_IMG_PFMT_YCC400:
			return 0ul;
		case FJ_IMG_PFMT_VIDEO:
			return ((ULONG)info_conv->g_cb_b_width) * M_BF_STILL_ROUNDUP_VIDEOFMT_LINES( ((ULONG)info_conv->lines) /2 );
		default:
			BF_Debug_Print_Error(( "%s: unknown pfmt %u.\n", __func__, info_conv->pfmt ));
			return 0ul;
	}
}


static ULONG bf_still_dache_calc_fjinfoconv_a_bytes( const FJ_IMG_INFO_CONV* const info_conv )
{
	switch( info_conv->pfmt ) {
		case FJ_IMG_PFMT_RGBA8888:	// FALLTHROUGH
		case FJ_IMG_PFMT_RGBA4444:	// FALLTHROUGH
		case FJ_IMG_PFMT_YCC422:	// FALLTHROUGH
		case FJ_IMG_PFMT_YCC444:	// FALLTHROUGH
		case FJ_IMG_PFMT_YCC420:	// FALLTHROUGH
		case FJ_IMG_PFMT_YCC400:	// FALLTHROUGH
		case FJ_IMG_PFMT_VIDEO:
			return 0ul;		// Unsupported Alpha plane.
		default:
			BF_Debug_Print_Error(( "%s: unknown pfmt %u.\n", __func__, info_conv->pfmt ));
			return 0ul;
	}
}


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

FJ_ERR_CODE BF_Still_Dcache_Clean( const VOID* const addr, const ULONG bytes )	/* pgr0339 */
{
	BF_L1l2cache_Run( (T_L1L2CACHE_INFORMATION) {
			.l1l2cache_type = E_BF_L1L2CACHE_ADDR,
			.l1l2cache_method = E_BF_L1L2CACHE_CLEAN,
			.l1l2cache_address = (UINT32)addr,
			.l1l2cache_size = bytes,
		} );

	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_Still_Dcache_CleanFlush( const VOID* const addr, const ULONG bytes )	/* pgr0339 */
{
	BF_L1l2cache_Run( (T_L1L2CACHE_INFORMATION) {
			.l1l2cache_type = E_BF_L1L2CACHE_ADDR,
			.l1l2cache_method = E_BF_L1L2CACHE_CLEAN_FLUSH,
			.l1l2cache_address = (UINT32)addr,
			.l1l2cache_size = bytes,
		} );

	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_Still_Dcache_CleanFlush_FjImg( const FJ_IMG_INFO* const img_info )
{
	ULONG bytes;

	// Y
	bytes = bf_still_dache_calc_fjimg_y_bytes( img_info );
	BF_Still_Dcache_CleanFlush( (VOID*)img_info->addr.AddressY, bytes );

	// C
	bytes = bf_still_dache_calc_fjimg_c_bytes( img_info );
	BF_Still_Dcache_CleanFlush( (VOID*)img_info->addr.AddressCb, bytes );

	return FJ_ERR_OK;
}


FJ_ERR_CODE BF_Still_Dcache_CleanFlush_FjInfoConv( const FJ_IMG_INFO_CONV* const info_conv )
{
	ULONG bytes;

	// Y
	bytes = bf_still_dache_calc_fjinfoconv_y_bytes( info_conv );
	if( bytes != 0ul ) {
#ifdef CO_STILL_DCACHE_DEBUG_LOG
		BF_Debug_Print_Debug(( "%s: Y %lx, %lu bytes.\n", __func__, info_conv->addr.Y_G, bytes ));
#endif
		BF_Still_Dcache_CleanFlush( (VOID*)info_conv->addr.Y_G, bytes );
	}

	// C
	bytes = bf_still_dache_calc_fjinfoconv_c_bytes( info_conv );
	if( bytes != 0ul ) {
#ifdef CO_STILL_DCACHE_DEBUG_LOG
		BF_Debug_Print_Debug(( "%s: C %lx, %lu bytes.\n", __func__, info_conv->addr.Cb_B, bytes ));
#endif
		BF_Still_Dcache_CleanFlush( (VOID*)info_conv->addr.Cb_B, bytes );
	}

	// A
	bytes = bf_still_dache_calc_fjinfoconv_a_bytes( info_conv );
	if( bytes != 0ul ) {
#ifdef CO_STILL_DCACHE_DEBUG_LOG
		BF_Debug_Print_Debug(( "%s: A %lx, %lu bytes.\n", __func__, info_conv->addr.Alpha, bytes ));
#endif
		BF_Still_Dcache_CleanFlush( (VOID*)info_conv->addr.Alpha, bytes );
	}

	return FJ_ERR_OK;
}

