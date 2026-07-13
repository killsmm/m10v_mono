/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CAMERA_SENSOR_OV12895_H
#define _CAMERA_SENSOR_OV12895_H

#include "ddim_typedef.h"
#include "camera_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
//	E_SENSOR_OV12895_MODE_FUll_3840x2160_RAW12_30FPS,
	E_SENSOR_OV12895_MODE_FUll_3840x2160_RAW10_60FPS,
	E_SENSOR_OV12895_MODE_MAX
} E_SENSOR_OV12895_MODE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern const T_CAMERA_SENSOR_BASIC		OV12895_BASIC;
extern const T_CAMERA_SENSOR_MODE		OV12895_MODE[E_SENSOR_OV12895_MODE_MAX];
/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_OV12895_Clock_Gpio_Init(VOID);
E_CAMERA_ERR_CODE Sensor_OV12895_Standby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_OV12895_CancelStandby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_OV12895_Init(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_OV12895_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol);
E_CAMERA_ERR_CODE Sensor_OV12895_Stream(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_OV12895_SgVHD(E_CAMERA_ID sensor_id, ULONG vdocyc, ULONG hdocyc);
E_CAMERA_ERR_CODE Sensor_OV12895_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);
E_CAMERA_ERR_CODE Sensor_OV12895_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in);

#endif
