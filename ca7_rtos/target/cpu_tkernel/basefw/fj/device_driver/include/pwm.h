/**
 * @file	  : pwm.h
 * @brief	  : PWM Timer wrapper
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>\n
 */

#ifndef _PWM_H_
#define _PWM_H_

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

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/
extern FJ_ERR_CODE fj_pwm_open( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_pwm_open_step( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_pwm_open_step_2ch( FJ_PERI_CH ch_1st, FJ_PERI_CH ch_2nd );

extern FJ_ERR_CODE fj_pwm_set_by_ratio( FJ_PWM_CTRL const* const ctrl, UINT32 cycle_hz, UINT32 duty_ratio );
extern FJ_ERR_CODE fj_pwm_set_by_time( FJ_PWM_CTRL const* const ctrl, ULONG time1, ULONG time2  );
extern FJ_ERR_CODE fj_pwm_set_2ch( FJ_PWM_CTRL_PARAM const* const ctrl_a, FJ_PWM_CTRL_PARAM const* const ctrl_b );
extern FJ_ERR_CODE fj_pwm_set_step( FJ_PWM_STEP_CTRL const* const ctrl  );
extern FJ_ERR_CODE fj_pwm_start( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_pwm_2ch_start( FJ_PERI_CH ch_a, FJ_PERI_CH ch_b );
extern FJ_ERR_CODE fj_pwm_calculate_clock_sel(ULONG usec, UCHAR* const clk, USHORT* const cycle);
extern FJ_ERR_CODE fj_pwm_calculate_array_clock_sel(ULONG usec_array[][2], UCHAR* const clk_sel, FJ_PWM_STEP_DATA step_data[], UCHAR step_data_num);
extern FJ_ERR_CODE fj_pwm_set_buffer_data_window(FJ_PERI_CH ch, ULONG usec_array[][2], FJ_PWM_STEP_DATA step_data[], UCHAR data_num, UCHAR pulse_end_flag);
extern FJ_ERR_CODE fj_pwm_set_buffer_data_window_by_step_data(FJ_PERI_CH ch, FJ_PWM_STEP_DATA step_data[], UCHAR data_num);
extern FJ_ERR_CODE fj_pwm_calculate_cycle(ULONG usec[][2], UCHAR const clk_sel, FJ_PWM_STEP_DATA step_data[], UCHAR cycle_num);




extern FJ_ERR_CODE fj_pwm_stop( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_pwm_close( FJ_PERI_CH ch );
#endif
