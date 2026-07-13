/**
 * @file		fj_common.h
 * @brief		FJ_API common header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_COMMON_H
#define _FJ_COMMON_H

// --- REMOVE_CAMERAIF BEGIN ---
#include "ddim_typedef.h"
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
// --- REMOVE_CAMERAIF BEGIN ---
// set date time
typedef struct {
	ULONG year		: 16;
	ULONG month		: 8;
	ULONG day		: 8;
	
	ULONG hour		: 5;
	ULONG min		: 6;
	ULONG sec		: 6;
	ULONG milsec	: 15;
} T_FJ_COMMON_DATE_TIME;
// --- REMOVE_CAMERAIF END ---

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
// --- REMOVE_CAMERAIF BEGIN ---
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set base date&time.
@param[in] date & time
@retval None
*/
VOID FJ_Common_Set_Base_DateTime( T_FJ_COMMON_DATE_TIME* date_time );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get base date&time.
@param[out] date & time
@retval None
*/
VOID FJ_Common_Get_Base_DateTime( T_FJ_COMMON_DATE_TIME* date_time );
// --- REMOVE_CAMERAIF END ---

#endif	// _FJ_COMMON_H
