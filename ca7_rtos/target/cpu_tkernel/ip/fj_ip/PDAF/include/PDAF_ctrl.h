/**
 * @file		PDAF_ctrl.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2019 Socionext Inc.</I></B>
 */

#ifndef _PDAF_CTRL_H_
#define _PDAF_CTRL_H_

#include "ddim_typedef.h"
#include "share_data.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	// PDAF保持結果全出力
	FLOAT	PdafPos;			// レンズ推定位置
	ULONG	ConfidenceLevel;	// Lib:信頼度 1024 * ConfidenceLevel / Threshold
}T_PDAF_RESULT_DETAILS;

typedef struct {
	USHORT				g_width;
	USHORT				width;
	USHORT				height;
	ULONG				addr;
} DUAL_PD_DATA;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

INT32 PDAF_Ctrl_Test( VOID );
INT32 PDAF_Ctrl( T_SHARE_AF_DATA *in_data, FLOAT now_pos, T_PDAF_RESULT_DETAILS *result );


#ifdef __cplusplus
}
#endif

#endif	// _PDAF_CTRL_H_

