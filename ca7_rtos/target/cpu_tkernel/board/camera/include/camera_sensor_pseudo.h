/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CAMERA_SENSOR_PSEUDO_H
#define _CAMERA_SENSOR_PSEUDO_H

#include "ddim_typedef.h"
#include "camera_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Socionext_PSEUDO-AACK_Standard-DPHY_RegisterSetting_INCK24MHz_ver0.4-0.0....xlsx
typedef enum {
	E_SENSOR_PSEUDO_MODE_Full_3840x2160_RAW10_30fps = 0,//
	E_SENSOR_PSEUDO_MODE_MAX
} E_SENSOR_PSEUDO_MODE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern const T_CAMERA_SENSOR_BASIC		PSEUDO_BASIC;
extern const T_CAMERA_SENSOR_MODE		PSEUDO_MODE[E_SENSOR_PSEUDO_MODE_MAX];

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_PSEUDO_Clock_Gpio_Init(VOID);
E_CAMERA_ERR_CODE Sensor_PSEUDO_Standby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_PSEUDO_Stream(E_CAMERA_ID sensor_id);

E_CAMERA_ERR_CODE Sensor_PSEUDO_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);


#endif
