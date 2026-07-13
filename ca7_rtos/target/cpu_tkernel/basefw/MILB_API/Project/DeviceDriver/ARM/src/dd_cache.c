/**
 * @file		dd_cache.c
 * @brief		Cache Operation driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dd_cache.h"

extern VOID Dd_CACHE_L1_Flush_Dcache_Addr_Asm(UINT32 address, UINT32 size);
extern VOID Dd_CACHE_L1_Clean_Dcache_Addr_Asm(UINT32 address, UINT32 size);
extern VOID Dd_CACHE_L1_Clean_Flush_Dcache_Addr_Asm(UINT32 address, UINT32 size);
extern VOID Dd_CACHE_L2_Flush_Dcache_Addr_Asm(UINT32 address, UINT32 size);
extern VOID Dd_CACHE_L2_Clean_Dcache_Addr_Asm(UINT32 address, UINT32 size);
extern VOID Dd_CACHE_L2_Clean_Flush_Dcache_Addr_Asm(UINT32 address, UINT32 size);

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

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
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
Level 1 D-Cache Flush by Addressing & size function
@param[in]		address		: target start address
@param[in]		size		: target size
@remarks	This function flush is Level 1 Cache Only.
*/
VOID Dd_CACHE_L1_Flush_Dcache_Addr_Align(UINT32 address, UINT32 size)
{
	if( 0 == size ) {
#ifdef CO_PARAM_CHECK
		Ddim_Print(("Dd_CACHE_L1_Flush_Dcache_Addr_Align : input size 0 error.\n"));
#endif
		return;
	}
	Dd_CACHE_L1_Flush_Dcache_Addr_Asm(address, size);
}

/**
Level 1 D-Cache Clean by Addressing & size function
@param[in]		address		: target start address
@param[in]		size		: target size
@remarks	This function clean is Level 1 Cache Only.
*/
VOID Dd_CACHE_L1_Clean_Dcache_Addr_Align(UINT32 address, UINT32 size)
{
	if( 0 == size ) {
#ifdef CO_PARAM_CHECK
		Ddim_Print(("Dd_CACHE_L1_Clean_Dcache_Addr_Align : input size 0 error.\n"));
#endif
		return;
	}
	Dd_CACHE_L1_Clean_Dcache_Addr_Asm(address, size);
}

/**
Level 1 D-Cache Clean & Flush by Addressing & size function
@param[in]		address		: target start address
@param[in]		size		: target size
@remarks	This function clean & flush is Level 1 Cache Only.
*/
VOID Dd_CACHE_L1_Clean_Flush_Dcache_Addr_Align(UINT32 address, UINT32 size)
{
	if( 0 == size ) {
#ifdef CO_PARAM_CHECK
		Ddim_Print(("Dd_CACHE_L1_Clean_Flush_Dcache_Addr_Align : input size 0 error.\n"));
#endif
		return;
	}
	Dd_CACHE_L1_Clean_Flush_Dcache_Addr_Asm(address, size);
}

/**
Level 2 D-Cache Flush by Addressing & size function
@param[in]		address		: target start address
@param[in]		size		: target size
@remarks	This function's flush is Level 2 Cache Only.
*/
VOID Dd_CACHE_L2_Flush_Dcache_Addr_Align(UINT32 address, UINT32 size)
{
	if( 0 == size ) {
#ifdef CO_PARAM_CHECK
		Ddim_Print(("Dd_CACHE_L2_Flush_Dcache_Addr_Align : input size 0 error.\n"));
#endif
		return;
	}
	Dd_CACHE_L2_Flush_Dcache_Addr_Asm(address, size);
}

/**
Level 2 D-Cache Clean by Addressing & size function
@param[in]		address		: target start address
@param[in]		size		: target size
@remarks	This function's clean is Level 2 Cache Only.
*/
VOID Dd_CACHE_L2_Clean_Dcache_Addr_Align(UINT32 address, UINT32 size)
{
	if( 0 == size ) {
#ifdef CO_PARAM_CHECK
		Ddim_Print(("Dd_CACHE_L2_Clean_Dcache_Addr_Align : input size 0 error.\n"));
#endif
		return;
	}
	Dd_CACHE_L2_Clean_Dcache_Addr_Asm(address, size);
}

/**
Level 2 D-Cache Clean & Flush by Addressing & size function
@param[in]		address		: target start address
@param[in]		size		: target size
@remarks	This function's clean & flush is Level 2 Cache Only.
*/
VOID Dd_CACHE_L2_Clean_Flush_Dcache_Addr_Align(UINT32 address, UINT32 size)
{
	if( 0 == size ) {
#ifdef CO_PARAM_CHECK
		Ddim_Print(("Dd_CACHE_L2_Clean_Flush_Dcache_Addr_Align : input size 0 error.\n"));
#endif
		return;
	}
	Dd_CACHE_L2_Clean_Flush_Dcache_Addr_Asm(address, size);
}

/* end */
