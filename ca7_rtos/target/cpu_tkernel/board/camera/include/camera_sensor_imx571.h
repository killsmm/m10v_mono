/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CAMERA_SENSOR_IMX571_H
#define _CAMERA_SENSOR_IMX571_H

#include "ddim_typedef.h"
#include "camera_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define IMX571_GLOBAL_SHUTTER
#define E_IMX571_REG_SMD 0x0003
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// IMX571BQR-C_Initial_Register_Setting(1.1.0)_190306.xlsx
typedef enum {
	E_SENSOR_IMX571_MODE0C_V1H1 = 0,//
    E_SENSOR_IMX571_MODE0E_V1H1,
	E_SENSOR_IMX571_MODE5C_V2H2,
	E_SENSOR_IMX571_MODE_MAX
} E_SENSOR_IMX571_MODE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern const T_CAMERA_SENSOR_BASIC		IMX571_BASIC;
extern const T_CAMERA_SENSOR_MODE		IMX571_MODE[E_SENSOR_IMX571_MODE_MAX];

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX571_Clock_Gpio_Init(VOID);
E_CAMERA_ERR_CODE Sensor_IMX571_CancelStandby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX571_Standby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX571_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol);
E_CAMERA_ERR_CODE Sensor_IMX571_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);
E_CAMERA_ERR_CODE Sensor_IMX571_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in);
E_CAMERA_ERR_CODE Sensor_IMX571_GlobalShutter(E_CAMERA_ID sensor_id, E_CAMERA_SENSOR_SHUTTER shutter_mode);
E_CAMERA_ERR_CODE Sensor_IMX571_SgVHD(E_CAMERA_ID sensor_id, ULONG vdocyc, ULONG hdocyc);
E_CAMERA_ERR_CODE Sensor_IMX571_Test_shutter(int open, int last_usec);
E_CAMERA_ERR_CODE Sensor_IMX571_CALLBACK	(E_CAMERA_ID sensor_id, UCHAR enable);
#endif
