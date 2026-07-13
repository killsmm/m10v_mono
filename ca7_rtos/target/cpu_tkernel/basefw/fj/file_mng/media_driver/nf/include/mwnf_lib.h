/**
 * @file		mwnf_lib.h
 * @brief		Nand Flash driver lib header file
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef __MWNF_LIB_H
#define __MWNF_LIB_H

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Declaration										*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

// memory data copy
extern VOID* MWNF_lib_memcpy(VOID* dst_add, const VOID* src_add, UINT32 size);

// memory data compare
extern INT32 MWNF_lib_memcmp(BYTE* s_buf, BYTE* t_buf, ULONG size);

// memory data fill
extern VOID MWNF_lib_Filldata_By_Byte(BYTE* d_buf, BYTE fill_data, ULONG size);

// memory data set
extern VOID MWNF_lib_memset(VOID* dst,UCHAR data,ULONG size);

// make random number
extern INT32 MWNF_lib_Rand(VOID);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// __MWNF_LIB_H

