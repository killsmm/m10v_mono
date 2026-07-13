/**
 * @file		iq_dcache.h
 * @brief		ARM D-cache and SDIU buffer control.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __IQ_DCACHE_H__
#define __IQ_DCACHE_H__

#include "fj_memory.h"

#include "sdram_map_common.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
/* D-cache address */
#if defined(__CYGWIN__) || defined(__MINGW32__)
#define M_BF_IQ_DCACHE_ADDR(x)		(x)
#else //!(__CYGWIN__ || __MINGW32__)
#define M_BF_IQ_DCACHE_ADDR(x)		(x)
#endif //!(__CYGWIN__ || __MINGW32__)

#define D_BF_IQ_DCACHE_ALIGN		32
#define M_BF_IQ_DCACHE_ROUNDUP(val)	((((val) + D_BF_IQ_DCACHE_ALIGN - 1) / D_BF_IQ_DCACHE_ALIGN) * D_BF_IQ_DCACHE_ALIGN)


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
/* Function																		*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

extern FJ_ERR_CODE BF_IQ_Dcache_Clear( ULONG addr, ULONG bytes );
extern FJ_ERR_CODE BF_IQ_Dcache_Flush( ULONG addr, ULONG bytes );



#ifdef __cplusplus
}
#endif

#endif // __IQ_DCACHE_H__

