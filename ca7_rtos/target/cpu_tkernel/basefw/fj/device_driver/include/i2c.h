/**
 * @file		i2c.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */
#ifndef _I2C_H_
#define _I2C_H_
#include "fj_peripheral.h"

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

FJ_ERR_CODE fj_i2c_open( FJ_PERI_CH ch );
FJ_ERR_CODE fj_i2c_close( FJ_PERI_CH ch );
FJ_ERR_CODE fj_i2c_ctrl( FJ_PERI_CH ch, FJ_I2C_CTRL i2c_ctrl, I2C_CALLBACK pcallback );
FJ_ERR_CODE fj_i2c_transfer( FJ_PERI_CH ch, FJ_I2C_TRANSFER_DATA trans_ctrl[], UINT32 length );
FJ_ERR_CODE fj_i2c_set_approxclock( FJ_PERI_CH ch, UINT32 req_clock, UINT32* act_clock, BOOL under_flag );

#ifdef __cplusplus
}
#endif

#endif	/* _I2C_H_ */
