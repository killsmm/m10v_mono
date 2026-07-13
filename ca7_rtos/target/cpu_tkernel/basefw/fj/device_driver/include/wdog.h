/**
 * @file		wdog.h
 * @brief		watchdog timer process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _WDOG_H_
#define _WDOG_H_

#include "ddim_typedef.h"

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
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

FJ_ERR_CODE fj_wdog_timer_init( VOID );
FJ_ERR_CODE fj_wdog_timer_open( VOID );
FJ_ERR_CODE fj_wdog_timer_ctrl( FJ_WDOG_MODE wdog_mode, ULONG msec, VOID* user_handler );
FJ_ERR_CODE fj_wdog_timer_refresh( VOID );
FJ_ERR_CODE fj_wdog_timer_get_reset_status( FJ_WDOG_RESET_STATUS* raw_wdog_int, FJ_WDOG_RESET_MASK_STATUS* mask_wdog_int );
FJ_ERR_CODE fj_wdog_timer_get_load_value( ULONG* load_value );
FJ_ERR_CODE fj_wdog_timer_stop( VOID );
FJ_ERR_CODE fj_wdog_timer_close( VOID );

#ifdef __cplusplus
}
#endif


#endif	// _WDOG_H_
