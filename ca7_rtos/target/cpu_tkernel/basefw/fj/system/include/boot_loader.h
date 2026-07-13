/**
 * @file		boot_loader.h
 * @brief		boot_loader process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _BOOT_LOADER_H_
#define _BOOT_LOADER_H_

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_BF_BOOT_LOADER_SECOND_CODE		(0x80000000)
//#define D_BF_BOOT_LOADER_THIRD_CODE		(0x40000000)
//#define D_BF_BOOT_LOADER_FOURTH_CODE		(0x20000000)
//#define D_BF_BOOT_LOADER_ALL_CODE			(0xE0000000)
#define D_BF_BOOT_LOADER_ALL_CODE			(0x00000000)

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
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

FJ_ERR_CODE fj_bootload( UCHAR areaNo, char* dstStartAdr, ULONG loadSize );

INT32 fj_bootloadstate( VOID );

extern UCHAR BF_Boot_Loader_Check(UINT32 chk_area);

#ifdef __cplusplus
}
#endif

#endif	// _BOOT_LOADER_H_
