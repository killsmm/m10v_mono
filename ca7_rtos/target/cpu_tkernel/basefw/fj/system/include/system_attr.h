/**
 * @file		system_attr.h
 * @brief		system attribute process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _SYSTEM_ATTR_H_
#define _SYSTEM_ATTR_H_

#include "fj_api.h"
// --- REMOVE_CAMERAIF BEGIN ---
#include "fj_common.h"
// --- REMOVE_CAMERAIF END ---

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

/*----------------------------------------------------------------------------
 * System Attribute Data
----------------------------------------------------------------------------*/
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

FJ_ERR_CODE fj_getparam( USHORT wParamID, USHORT* pwValue );
FJ_ERR_CODE fj_setparam( USHORT wParamID, USHORT wValue );
FJ_ERR_CODE	fj_getparamlong(USHORT wParamID, LONG* pwValue);

// --- REMOVE_CAMERAIF BEGIN ---
#if 1
VOID fj_setbasedatetime( T_FJ_COMMON_DATE_TIME* date_time );
VOID fj_getbasedatetime( T_FJ_COMMON_DATE_TIME* date_time );
#endif
// --- REMOVE_CAMERAIF END ---

#ifdef __cplusplus
}
#endif

#endif	// _SYSTEM_ATTR_H_
