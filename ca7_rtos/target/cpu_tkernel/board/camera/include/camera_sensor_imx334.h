/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2021 Socionext Inc.</I></B>
 */
#ifndef _CAMERA_SENSOR_IMX334_H
#define _CAMERA_SENSOR_IMX334_H

#include "ddim_typedef.h"
#include "camera_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_SENSOR_IMX334_MODE_ALL_PIX_SCAN_RAW12_60fps = 0,
	E_SENSOR_IMX334_MODE_MAX
} E_SENSOR_IMX334_MODE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern const T_CAMERA_SENSOR_BASIC		IMX334_BASIC;
extern const T_CAMERA_SENSOR_MODE		IMX334_MODE[E_SENSOR_IMX334_MODE_MAX];

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX334_Clock_Gpio_Init(VOID);
E_CAMERA_ERR_CODE Sensor_IMX334_CancelStandby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX334_Standby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX334_Stream(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX334_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol);

E_CAMERA_ERR_CODE Sensor_IMX334_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);
E_CAMERA_ERR_CODE Sensor_IMX334_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in);

E_CAMERA_ERR_CODE Sensor_IMX334_DOL_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);
E_CAMERA_ERR_CODE Sensor_IMX334_DOL_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in );

#endif
