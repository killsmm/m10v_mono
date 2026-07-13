/**
 * @file		assertion.h
 * @brief		Assertion Log
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _ASSERTION_H
#define _ASSERTION_H

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define ASSERTION_LOG_TBL_SIZE		(100)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	ULONG	time_us;
	ULONG	ercd;
	ULONG	calling_func;
	ULONG	called_func;
	ULONG	tskid;
	UCHAR	oname[12];
} T_ASSERTION_DATA;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#ifdef CO_ASSERT_LOG_ON
#define Assertion_Log_Write_Macro(ercd,called_func)	Assertion_Log_Write(ercd,__return_address(),(VOID *)called_func)
#else
#define Assertion_Log_Write_Macro(ercd,called_func)
#endif

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

VOID Assertion_Log_Init(VOID);
VOID Assertion_Log_Write(ULONG ercd, ULONG calling_func, VOID *called_func);

#ifdef __cplusplus
}
#endif

#endif	// _ASSERTION_H
