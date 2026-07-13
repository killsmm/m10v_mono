/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2019 Ability Inc.</I></B>
 */
#ifndef _CAMERA_SENSOR_IMX269_H
#define _CAMERA_SENSOR_IMX269_H

#include "ddim_typedef.h"
#include "camera_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Socionext_IMX269CJK-C-Data-Sheet
typedef enum {
	E_SENSOR_IMX269_MODE7_4128x2264_RAW10_60_46FPS = 0,
	E_SENSOR_IMX269_MODE5_5280x3044_RAW10_36_35FPS,
	E_SENSOR_IMX269_MODE1_5280x3956_RAW12_18_92FPS,
	E_SENSOR_IMX269_MODE_MAX,
} E_SENSOR_IMX269_MODE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern const T_CAMERA_SENSOR_BASIC		IMX269_BASIC;
extern const T_CAMERA_SENSOR_MODE		IMX269_MODE[];

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX269_Clock_Gpio_Init(VOID);
E_CAMERA_ERR_CODE Sensor_IMX269_CancelStandby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX269_Standby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX269_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol);
E_CAMERA_ERR_CODE Sensor_IMX269_SgVHD(E_CAMERA_ID sensor_id, ULONG vdocyc, ULONG hdocyc);
E_CAMERA_ERR_CODE Sensor_IMX269_Multi(E_CAMERA_ID sensor_id, BOOL is_multi, BOOL is_master);
E_CAMERA_ERR_CODE Sensor_IMX269_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);
E_CAMERA_ERR_CODE Sensor_IMX269_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in);
#endif
