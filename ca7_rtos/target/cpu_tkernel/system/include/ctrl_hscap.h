/**
 * @file		ctrl_hscap.h
 * @brief		control hs( high speed ) capture
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2019 Socionext Inc.</I></B>
 */

#ifndef _HSCAP_H_
#define _HSCAP_H_

#include "share.h"
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

void HSCap_Init( UINT32 pipe_bank_num, UINT32 media_bank_num, FLOAT fps, BOOL is_slow_case );
BOOL HSCap_Is_Close_MShutter( ULLONG frame_no );
void HSCap_Judge_MShutter( ULLONG frame_no );
INT32 HSCap_Push_PipeQ_MediaQ( ULLONG frame_no );
void HSCap_Timer_Callback( void );
void HSCap_Start_Pipe_Dummy( void );
BOOL HSCap_Judge_Start_NextProcess( ULLONG* frame_no );
void HSCap_Pull_PipeQ( void );
void HSCap_Pull_MediaQ( void );
void HSCap_Measure_Pipe_Start( ULLONG frame_no );
void HSCap_Measure_Pipe_End( void );
void HSCap_Set_Media_Bank( UINT32 media_bank_num );

#ifdef __cplusplus
}
#endif

#endif	// _HSCAP_H_

