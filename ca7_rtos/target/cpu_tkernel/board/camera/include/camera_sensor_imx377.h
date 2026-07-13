/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CAMERA_SENSOR_IMX377_H
#define _CAMERA_SENSOR_IMX377_H

#include "ddim_typedef.h"
#include "camera_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {

	E_SENSOR_IMX377_12M_MODE0_4000x3000 = 0,
	E_SENSOR_IMX377_9M_MODE1_3840x2160,
	E_SENSOR_IMX377_9M_MODE2_2048x1080,
	E_SENSOR_IMX377_MODE_MAX
} E_SENSOR_IMX377_MODE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern const T_CAMERA_SENSOR_BASIC		IMX377_BASIC;
extern const T_CAMERA_SENSOR_MODE		IMX377_MODE[E_SENSOR_IMX377_MODE_MAX];

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX377_Clock_Gpio_Init(VOID);
E_CAMERA_ERR_CODE Sensor_IMX377_CancelStandby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX377_Standby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX377_Stream(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX377_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol);

E_CAMERA_ERR_CODE Sensor_IMX377_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);
E_CAMERA_ERR_CODE Sensor_IMX377_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in);

#endif
