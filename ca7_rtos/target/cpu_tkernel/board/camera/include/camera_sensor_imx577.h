/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CAMERA_SENSOR_IMX577_H
#define _CAMERA_SENSOR_IMX577_H

#include "ddim_typedef.h"
#include "camera_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Socionext_IMX577-AACK_Standard-DPHY_RegisterSetting_INCK24MHz_ver0.4-0.0....xlsx
typedef enum {
	E_SENSOR_IMX577_MODE_Full_4056x2288_RAW10_60fps = 0,//
    E_SENSOR_IMX577_MODE_Full_4056x3040_RAW10_60fps,
	E_SENSOR_IMX577_MODE_Full_4056x3040_RAW12_40fps,
	E_SENSOR_IMX577_MODE_BINNING_2x2_2028x1128_RAW10_240fps,
	E_SENSOR_IMX577_MODE_MAX
} E_SENSOR_IMX577_MODE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern const T_CAMERA_SENSOR_BASIC		IMX577_BASIC;
extern const T_CAMERA_SENSOR_MODE		IMX577_MODE[E_SENSOR_IMX577_MODE_MAX];

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX577_Clock_Gpio_Init(VOID);
E_CAMERA_ERR_CODE Sensor_IMX577_Init(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX577_CancelStandby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX577_Standby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX577_Stream(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX577_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol);
E_CAMERA_ERR_CODE Sensor_IMX577_Pdaf_Mode(E_CAMERA_ID sensor_id, UINT32 mode);
E_CAMERA_ERR_CODE Sensor_IMX577_Multi(E_CAMERA_ID sensor_id, BOOL is_multi, BOOL is_master);

E_CAMERA_ERR_CODE Sensor_IMX577_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);
E_CAMERA_ERR_CODE Sensor_IMX577_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in);

E_CAMERA_ERR_CODE Sensor_IMX577_DOL_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);
E_CAMERA_ERR_CODE Sensor_IMX577_DOL_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in );
E_CAMERA_ERR_CODE Sensor_IMX577_Get_PD_Info(E_CAMERA_ID sensor_id, UINT32 mode, ULONG sta_addr, USHORT pd_num, T_CAMERA_PD_DATA* pd_info);

#endif
