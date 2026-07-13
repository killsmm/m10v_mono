/**
 * @file		l1l2cache.h
 * @brief		L1 Cache and L2 Cache Clean,Invalidate Control
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _L1L2CACHE_H_
#define _L1L2CACHE_H_

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_BF_L1L2CACHE_ALL	=  0,			// ALL Cache
	E_BF_L1L2CACHE_ADDR					// Address and Size Cache
} E_BF_L1L2CACHE_TYPE;

typedef enum {
	E_BF_L1L2CACHE_FLUSH	=  0,		// Cache Flush
	E_BF_L1L2CACHE_CLEAN,				// Cache Clean
	E_BF_L1L2CACHE_CLEAN_FLUSH			// Cache Clean and Flush
} E_BF_L1L2CACHE_METHOD;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Cache Information */
typedef struct {
	E_BF_L1L2CACHE_TYPE		l1l2cache_type;
	E_BF_L1L2CACHE_METHOD	l1l2cache_method;
	UINT32					l1l2cache_address;
	UINT32					l1l2cache_size;
} T_L1L2CACHE_INFORMATION;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* FID_L1L2CACHE */
#define FLG_L1L2CACHE_RUN	(0x00000001UL)

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

VOID BF_L1l2cache_CallBack( UINT32 mask_status );
VOID BF_L1l2cache_Flush_All( VOID );
VOID BF_L1l2cache_Clean_All( VOID );
VOID BF_L1l2cache_Clean_Flush_All( VOID );
VOID BF_L1l2cache_Flush_Addr( UINT32 address, UINT32 size );
VOID BF_L1l2cache_Clean_Addr( UINT32 address, UINT32 size );
VOID BF_L1l2cache_Clean_Flush_Addr( UINT32 address, UINT32 size );
VOID BF_L1l2cache_Run( T_L1L2CACHE_INFORMATION l1l2cache_info );
VOID BF_L1l2cache_All_Main( E_BF_L1L2CACHE_METHOD method );

#ifdef __cplusplus
}
#endif

#endif	// _MEMORY_H_
