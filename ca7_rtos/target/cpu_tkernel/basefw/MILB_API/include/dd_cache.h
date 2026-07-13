/**
 * @file		dd_cache.h
 * @brief		Cache Operation driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_cache
@{
	- @ref dd_cache_sample_section1
	- @ref dd_cache_sample_section2
*/

#ifndef _DD_CACHE_H_
#define _DD_CACHE_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_DD_CACHE_ALIGN_64BIT_MASK			((1 << 6) - 1)	/**< local mask 64bit for D-cache address */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/** Level 1 Cache Flush D-cache for CPU, by address with align */
#define Dd_CACHE_L1_Flush_Dcache_Addr(adrs,size)		Dd_CACHE_L1_Flush_Dcache_Addr_Align((adrs)&(~D_DD_CACHE_ALIGN_64BIT_MASK),((adrs)&D_DD_CACHE_ALIGN_64BIT_MASK)+(size))
/** Level 1 Cache Clean D-cache for CPU, by address with align */
#define Dd_CACHE_L1_Clean_Dcache_Addr(adrs,size)		Dd_CACHE_L1_Clean_Dcache_Addr_Align((adrs)&(~D_DD_CACHE_ALIGN_64BIT_MASK),((adrs)&D_DD_CACHE_ALIGN_64BIT_MASK)+(size))
/** Level 1 Cache Clean & Flush D-cache for CPU, by address with align */
#define Dd_CACHE_L1_Clean_Flush_Dcache_Addr(adrs,size)	Dd_CACHE_L1_Clean_Flush_Dcache_Addr_Align((adrs)&(~D_DD_CACHE_ALIGN_64BIT_MASK),((adrs)&D_DD_CACHE_ALIGN_64BIT_MASK)+(size))
/** Level 2 Cache Flush D-cache for CPU, by address with align */
#define Dd_CACHE_L2_Flush_Dcache_Addr(adrs,size)		Dd_CACHE_L2_Flush_Dcache_Addr_Align((adrs)&(~D_DD_CACHE_ALIGN_64BIT_MASK),((adrs)&D_DD_CACHE_ALIGN_64BIT_MASK)+(size))
/** Level 2 Cache Clean D-cache for CPU, by address with align */
#define Dd_CACHE_L2_Clean_Dcache_Addr(adrs,size)		Dd_CACHE_L2_Clean_Dcache_Addr_Align((adrs)&(~D_DD_CACHE_ALIGN_64BIT_MASK),((adrs)&D_DD_CACHE_ALIGN_64BIT_MASK)+(size))
/** Level 2 Cache Clean & Flush D-cache for CPU, by address with align */
#define Dd_CACHE_L2_Clean_Flush_Dcache_Addr(adrs,size)	Dd_CACHE_L2_Clean_Flush_Dcache_Addr_Align((adrs)&(~D_DD_CACHE_ALIGN_64BIT_MASK),((adrs)&D_DD_CACHE_ALIGN_64BIT_MASK)+(size))

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


/**
Level 1 D-Cache Flush by Addressing & size function
@param[in]		address		: target start address
@param[in]		size		: target size
@remarks	This function flush is Level 1 Cache Only.
*/
VOID Dd_CACHE_L1_Flush_Dcache_Addr_Align(UINT32 address, UINT32 size);

/**
Level 1 D-Cache Clean by Addressing & size function
@param[in]		address		: target start address
@param[in]		size		: target size
@remarks	This function clean is Level 1 Cache Only.
*/
VOID Dd_CACHE_L1_Clean_Dcache_Addr_Align(UINT32 address, UINT32 size);

/**
Level 1 D-Cache Clean & Flush by Addressing & size function
@param[in]		address		: target start address
@param[in]		size		: target size
@remarks	This function clean & flush is Level 1 Cache Only.
*/
VOID Dd_CACHE_L1_Clean_Flush_Dcache_Addr_Align(UINT32 address, UINT32 size);

/**
Level 2 D-Cache Flush by Addressing & size function
@param[in]		address		: target start address
@param[in]		size		: target size
@remarks	This function flush is Level 2 Cache Only.
*/
VOID Dd_CACHE_L2_Flush_Dcache_Addr_Align(UINT32 address, UINT32 size);

/**
Level 2 D-Cache Clean by Addressing & size function
@param[in]		address		: target start address
@param[in]		size		: target size
@remarks	This function clean is Level 2 Cache Only.
*/
VOID Dd_CACHE_L2_Clean_Dcache_Addr_Align(UINT32 address, UINT32 size);

/**
Level 2 D-Cache Clean & Flush by Addressing & size function
@param[in]		address		: target start address
@param[in]		size		: target size
@remarks	This function clean & flush is Level 2 Cache Only.
*/
VOID Dd_CACHE_L2_Clean_Flush_Dcache_Addr_Align(UINT32 address, UINT32 size);

/**
Level 1 D-Cache ALL Flush function
@remarks	This function ALL flush is Level 1 Cache Only.
*/
VOID Dd_CACHE_L1_Flush_Dcache_All(VOID);

/**
Level 1 D-Cache ALL Clean function
@remarks	This function ALL Clean is Level 1 Cache Only.
*/
VOID Dd_CACHE_L1_Clean_Dcache_All(VOID);

/**
Level 1 D-Cache ALL Clean & Flush function
@remarks	This function ALL Clean is Level 1 Cache Only.
*/
VOID Dd_CACHE_L1_Clean_Flush_Dcache_All(VOID);

/**
Level 2 D-Cache ALL Flush function
@remarks	This function ALL flush is Level 2 Cache Only.
*/
VOID Dd_CACHE_L2_Flush_Dcache_All(VOID);

/**
Level 2 D-Cache ALL Clean function
@remarks	This function ALL Clean is Level 2 Cache Only.
*/
VOID Dd_CACHE_L2_Clean_Dcache_All(VOID);

/**
Level 2 D-Cache ALL Clean & Flush function
@remarks	This function ALL Clean is Level 2 Cache Only.
*/
VOID Dd_CACHE_L2_Clean_Flush_Dcache_All(VOID);


#ifdef __cplusplus
}
#endif

#endif	// _DD_CACHE_H_

/*@}*/

/**
@weakgroup dd_arm
@{
	@section dd_cache_sample_section1		Sample of Clean & Flush by Addressing & size
	@code
	//
	// CPU accessing in same memory area.
	//
	Dd_CACHE_L1_Clean_Flush_Dcache_Addr( adrs, size );
	Dd_ARM_Dsb_Pou();
	Dd_CACHE_L2_Clean_Flush_Dcache_Addr( adrs, size );
	//
	// Macro and/or DMA accessing in same memory area.
	//
	@endcode
	<br>
	@section dd_cache_sample_section2		Sample of Clean & Flush by All Address
	@code
	//
	// CPU accessing in same memory area.
	//
	Dd_CACHE_L1_Clean_Flush_Dcache_All();
	Dd_ARM_Dsb_Pou();
	Dd_CACHE_L2_Clean_Flush_Dcache_All();
	//
	// Macro and/or DMA accessing in same memory area.
	//
	@endcode
@}*/
