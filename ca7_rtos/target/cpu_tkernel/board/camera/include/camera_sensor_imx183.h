/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CAMERA_SENSOR_IMX183_H
#define _CAMERA_SENSOR_IMX183_H

#include "ddim_typedef.h"
#include "camera_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define D_LANE_10_BOARD

//1C12D
#ifdef D_LANE_10_BOARD
#define D_SENSOR_DATA0_SEL	(E_CAMERA_LVDS_SNDATA_SEL)(E_CAMERA_LVDS_SNDATA_SEL_SNDATA0)
#else
#define D_SENSOR_DATA0_SEL	(E_CAMERA_LVDS_SNDATA_SEL)(E_CAMERA_LVDS_SNDATA_SEL_SNDATA4)
#endif
#define D_SENSOR_DATA1_SEL	(E_CAMERA_LVDS_SNDATA_SEL)(D_SENSOR_DATA0_SEL + 1)
#define D_SENSOR_DATA2_SEL	(E_CAMERA_LVDS_SNDATA_SEL)(D_SENSOR_DATA0_SEL + 2)
#define D_SENSOR_DATA3_SEL	(E_CAMERA_LVDS_SNDATA_SEL)(D_SENSOR_DATA0_SEL + 3)
#define D_SENSOR_DATA4_SEL	(E_CAMERA_LVDS_SNDATA_SEL)(D_SENSOR_DATA0_SEL + 4)
#define D_SENSOR_DATA5_SEL	(E_CAMERA_LVDS_SNDATA_SEL)(D_SENSOR_DATA0_SEL + 5)
#define D_SENSOR_DATA6_SEL	(E_CAMERA_LVDS_SNDATA_SEL)(D_SENSOR_DATA0_SEL + 6)
#define D_SENSOR_DATA7_SEL	(E_CAMERA_LVDS_SNDATA_SEL)(D_SENSOR_DATA0_SEL + 7)
#define D_SENSOR_DATA8_SEL	(E_CAMERA_LVDS_SNDATA_SEL)(D_SENSOR_DATA0_SEL + 8)
#define D_SENSOR_DATA9_SEL	(E_CAMERA_LVDS_SNDATA_SEL)(D_SENSOR_DATA0_SEL + 9)
#define D_SENSOR_DATA10_SEL	(E_CAMERA_LVDS_SNDATA_SEL)(D_SENSOR_DATA0_SEL + 10)
#define D_SENSOR_DATA11_SEL	(E_CAMERA_LVDS_SNDATA_SEL)(D_SENSOR_DATA0_SEL + 11)

#define D_SENSOR_CLK0_SEL	(E_CAMERA_LVDS_SNCLK_SEL)(D_SENSOR_DATA0_SEL / 4 + 1)
#define D_SENSOR_CLK1_SEL	(E_CAMERA_LVDS_SNCLK_SEL)(D_SENSOR_CLK0_SEL + 1)
#define D_SENSOR_CLK2_SEL	(E_CAMERA_LVDS_SNCLK_SEL)(D_SENSOR_CLK0_SEL + 2)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
#ifdef D_LANE_10_BOARD
	E_SENSOR_IMX183_MODE_TYPE_1_NO_1_1 = 0,
#else
	E_SENSOR_IMX183_MODE_TYPE_1P4_NO_1 = 0,
#endif
	E_SENSOR_IMX183_MODE_TYPE_1_NO_1,
	E_SENSOR_IMX183_MODE_MAX
} E_SENSOR_IMX183_MODE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern const T_CAMERA_SENSOR_BASIC		IMX183_BASIC;
extern const T_CAMERA_SENSOR_MODE		IMX183_MODE[E_SENSOR_IMX183_MODE_MAX];
/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX183_Clock_Gpio_Init(VOID);
E_CAMERA_ERR_CODE Sensor_IMX183_Standby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX183_CancelStandby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_IMX183_GlobalShutter(E_CAMERA_ID sensor_id, E_CAMERA_SENSOR_SHUTTER shutter_mode);
E_CAMERA_ERR_CODE Sensor_IMX183_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol);
E_CAMERA_ERR_CODE Sensor_IMX183_SgVHD(E_CAMERA_ID sensor_id, ULONG vdocyc, ULONG hdocyc);
E_CAMERA_ERR_CODE Sensor_IMX183_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);
E_CAMERA_ERR_CODE Sensor_IMX183_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in);

#endif
