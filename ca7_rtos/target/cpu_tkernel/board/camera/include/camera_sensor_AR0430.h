/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CAMERA_SENSOR_AR0430_H
#define _CAMERA_SENSOR_AR0430_H

#include "ddim_typedef.h"
#include "camera_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_SENSOR_AR0430_MODE_FUll_2316x1746_RAW10_30FPS,
	E_SENSOR_AR0430_MODE_FUll_2316x1746_RAW12_30FPS,
	E_SENSOR_AR0430_MODE_MAX
} E_SENSOR_AR0430_MODE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UCHAR conversion		: 1;
	UCHAR global_fine_A		: 4;
	UCHAR Global_Coarse_A	: 2;
	USHORT Global_Digital	: 9;
	//FLOAT previous_gain;
} T_AR0430_GAIN;

typedef union T_DATA T_DATA;
union T_DATA{
	USHORT	hword;
	UCHAR	byte[2];
};

typedef struct {
	USHORT		address;
	T_DATA 		d_type;
} T_CAMERA_SENSOR_REG1;

typedef struct {
	FLOAT		A_level1_step;
	FLOAT		A_level2_step;
	FLOAT		A_level3_step;
	FLOAT		A_level4_step;
	FLOAT		D_level_step;
	FLOAT		tolerance;
	UCHAR 		A_gain_range;
	USHORT	 	D_gain_range;
} T_SENSOR_AR0430_GAIN_INFO;
/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern const T_CAMERA_SENSOR_BASIC		AR0430_BASIC;
extern const T_CAMERA_SENSOR_MODE		AR0430_MODE[E_SENSOR_AR0430_MODE_MAX];
/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_AR0430_Init(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_AR0430_Clock_Gpio_Init(VOID);
E_CAMERA_ERR_CODE Sensor_AR0430_Standby(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_AR0430_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol);
E_CAMERA_ERR_CODE Sensor_AR0430_Stream(E_CAMERA_ID sensor_id);
E_CAMERA_ERR_CODE Sensor_AR0430_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg);
E_CAMERA_ERR_CODE Sensor_AR0430_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in);
void Sensor_AR0430_Calc_Gain(const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out);
#endif
