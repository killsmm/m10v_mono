/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CAMERA_SENSOR_IMX533_H
#define _CAMERA_SENSOR_IMX533_H

#include "ddim_typedef.h"
#include "camera_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define TEST_SENSOR_MODE_0
//#define TEST_SENSOR_MODE_1
//#define TEST_SENSOR_MODE_1N
//#define TEST_SENSOR_MODE_2F

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
#if 1
	E_SENSOR_IMX533_MODE_H1_V1_NO_1 = 0,//
	E_SENSOR_IMX533_MODE_H1_V1_NO_0 = 1,//
#elif defined(TEST_SENSOR_MODE_1N)
	E_SENSOR_IMX533_MODE_H1_V1_NO_1N = 0,//
#elif defined(TEST_SENSOR_MODE_2F)
	E_SENSOR_IMX533_MODE_H1_V1_NO_2F = 0,//
#endif
	E_SENSOR_IMX533_MODE_MAX
} E_SENSOR_IMX533_MODE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern const T_CAMERA_SENSOR_BASIC		IMX533_BASIC;
extern const T_CAMERA_SENSOR_MODE		IMX533_MODE[E_SENSOR_IMX533_MODE_MAX];
/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX533_Clock_Gpio_Init(VOID);
E_CAMERA_ERR_CODE Sensor_IMX533_Standby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX533_CancelStandby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX533_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol);
E_CAMERA_ERR_CODE Sensor_IMX533_SgVHD(E_CAMERA_ID sensor_id, ULONG vdocyc, ULONG hdocyc);
E_CAMERA_ERR_CODE Sensor_IMX533_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);
E_CAMERA_ERR_CODE Sensor_IMX533_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in);

#endif
