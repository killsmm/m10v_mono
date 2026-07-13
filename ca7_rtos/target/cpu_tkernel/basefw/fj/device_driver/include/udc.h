/**
 * @file		: udc.h
 * @brief 		: Header of Up Down Counter
 * @note		: None
 * @attention	: None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _UDC_H
#define _UDC_H

#include "fj_peripheral.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Grobal Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
extern FJ_ERR_CODE		fj_udc_init( FJ_UDC_ZIN_CTRL zin_ctrl );
extern FJ_ERR_CODE		fj_udc_open( FJ_PERI_CH ch );
extern FJ_ERR_CODE		fj_udc_set_timer_mode( FJ_PERI_CH ch, ULONG usec, UDC_CALLBACK pCallback );
extern FJ_ERR_CODE		fj_udc_start( FJ_PERI_CH ch );
extern FJ_ERR_CODE		fj_udc_stop( FJ_PERI_CH ch );
extern FJ_ERR_CODE		fj_udc_set_up_down_count_mode( FJ_PERI_CH ch, FJ_UDC_COUNT_EDGE edge, FJ_UDC_CTRL const* const udc_crl );
extern FJ_ERR_CODE		fj_udc_set_phase_lag_mode( FJ_PERI_CH ch, FJ_UDC_PHASE_COUNT count_mode, FJ_UDC_CTRL const* const udc_crl );
extern FJ_ERR_CODE		fj_udc_set_up_down_counter( FJ_PERI_CH ch, USHORT up_down_counter );
extern FJ_ERR_CODE		fj_udc_set_comp_counter( FJ_PERI_CH ch, USHORT comp_counter );
extern FJ_ERR_CODE		fj_udc_set_callback( FJ_PERI_CH ch, UDC_CALLBACK pCallBack );
extern USHORT			fj_udc_get_up_down_counter( FJ_PERI_CH ch );
extern FJ_UDC_COUNT_DIR	fj_udc_get_up_down_flag( FJ_PERI_CH ch );
extern FJ_ERR_CODE		fj_udc_clear_up_down_counter( FJ_PERI_CH ch );
extern FJ_ERR_CODE		fj_udc_close( FJ_PERI_CH ch );

#endif
