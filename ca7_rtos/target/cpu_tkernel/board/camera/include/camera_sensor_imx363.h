/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CAMERA_SENSOR_IMX363_H
#define _CAMERA_SENSOR_IMX363_H

#include "ddim_typedef.h"
#include "camera_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_SENSOR_IMX363_MODE4_4kp60_4032x2272_pd_4032x568 = 0,	//M4-02
	E_SENSOR_IMX363_MODE4_Fhdp90_2016x1136_pd_2016x284,		//M4-03
	E_SENSOR_IMX363_MODE4_Fullp30_4032x3024_pd_4032x756,	//M4-01
	E_SENSOR_IMX363_MODE_MAX
} E_SENSOR_IMX363_MODE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern const T_CAMERA_SENSOR_BASIC		IMX363_BASIC;
extern const T_CAMERA_SENSOR_MODE		IMX363_MODE[E_SENSOR_IMX363_MODE_MAX];

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX363_Clock_Gpio_Init(VOID);
E_CAMERA_ERR_CODE Sensor_IMX363_Init(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX363_CancelStandby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX363_Standby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX363_Stream(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX363_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol);
E_CAMERA_ERR_CODE Sensor_IMX363_Pdaf_Mode(E_CAMERA_ID sensor_id, UINT32 mode);

E_CAMERA_ERR_CODE Sensor_IMX363_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);
E_CAMERA_ERR_CODE Sensor_IMX363_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in);

#endif
