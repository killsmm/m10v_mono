/**
 * @file		still_dcache.h
 * @brief		CPU Data-Cache and memory barrier Control functions
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _STILL_DCACHE_H_
#define _STILL_DCACHE_H_

// FJ Header
#include "fj_std.h"

// Map Header
#include "sdram_map_common.h"

// Driver Header
#include "ddim_typedef.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

// MDF request Non-Cached address. (Tentative. MDF will support Cached address.)
#define M_STILL_DCACHE_ADR_TO_FS(addr)	(addr)


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


extern FJ_ERR_CODE BF_Still_Dcache_Clean( const VOID* const addr, const ULONG bytes );
extern FJ_ERR_CODE BF_Still_Dcache_CleanFlush( const VOID* const addr, const ULONG bytes );
extern FJ_ERR_CODE BF_Still_Dcache_CleanFlush_FjImg( const FJ_IMG_INFO* const img_info );
extern FJ_ERR_CODE BF_Still_Dcache_CleanFlush_FjInfoConv( const FJ_IMG_INFO_CONV* const info_conv );


#ifdef __cplusplus
}
#endif

#endif	// _STILL_DCACHE_H_

