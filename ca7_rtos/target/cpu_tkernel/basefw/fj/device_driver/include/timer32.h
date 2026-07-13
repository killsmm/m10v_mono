/**
 * @file	  : timer32.h
 * @brief	  : Peripheral 32Bit reload timer wrapper
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>\n
 */

#ifndef _TIMER32_H_
#define _TIMER32_H_

#include "ddim_typedef.h"
#include "fj_std.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

extern FJ_ERR_CODE fj_tmr32_open( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_tmr32_set( FJ_PERI_CH ch, ULONG usec, VP_CALLBACK pcallback );
extern FJ_ERR_CODE fj_tmr32_set_pwm( FJ_PERI_CH reload_ch, FJ_PERI_CH pwm_ch, UCHAR mode, VP_CALLBACK pcallback );
extern FJ_ERR_CODE fj_tmr32_ctrl( FJ_PERI_CH ch, FJ_TMR32_CTRL const* const ctrl_param, VP_CALLBACK pcallback );
extern FJ_ERR_CODE fj_tmr32_tout_ctrl( FJ_PERI_CH ch, BOOL enable, UINT32 out_level );
extern FJ_ERR_CODE fj_tmr32_start( FJ_PERI_CH ch, BOOL reload );
extern FJ_ERR_CODE fj_tmr32_stop( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_tmr32_close( FJ_PERI_CH ch );

#endif
