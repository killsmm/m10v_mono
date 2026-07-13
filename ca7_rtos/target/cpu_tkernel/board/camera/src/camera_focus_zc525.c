/**
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 * @file		: camera_focus_zc525.c
 * @brief 		: focus driver for ZC525
 * @note		: None
 * @attention	: None
 */

#include "camera_config.h"
#include "camera_use.h"
#include "camera_focus_zc525.h"
#include "os_user_custom.h"
#include "share.h"
#include <math.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define REG_00	(0x00)
#define REG_01	(0x01)
#define REG_02	(0x02)
#define REG_03	(0x03)
#define REG_04	(0x04)
#define REG_05	(0x05)
#define REG_06	(0x06)
#define REG_07	(0x07)

#define D_DAC_POS_MIN			(0)				// 10bit resolution (temporary, need calibrarte)
#define D_DAC_POS_MAX			(1023)			// 10bit resolution (temporary, need calibrarte)

#define D_DAC_POS_INIT				(512)	// Default position

// If your module doesn't have Calibration Data, please tune D_DAC_POS_FW_INIT and D_DAC_POS_BW_INIT by using your module
#define D_DAC_POS_FW_INIT			(460)	// Start position for forward (temporary, need fine tuning)
#define D_DAC_POS_BW_INIT			(800)	// Start position for backward (temporary, need fine tuning)

#define D_DAC_POS_COARSE_FW_SETP	(50)	// coarse search forward moviing step (temporary, need fine tuning)
#define D_DAC_POS_COARSE_BW_SETP	(50)	// coarse search backward moviing step (temporary, need fine tuning)
#define D_DAC_POS_FINE_FW_SETP		(10)	// fine search forward moviing step (temporary, need fine tuning)
#define D_DAC_POS_FINE_BW_SETP		(10)	// fine search backward moviing step (temporary, need fine tuning)
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static E_CAMERA_ERR_CODE	init_T_CAMERA_FOCUS_PARAM( E_CAMERA_ID focus_id, T_CAMERA_FOCUS_PARAM* prm );

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

#define DoF_INF				10000
const T_CAMERA_FOCUS_DOF Z525_DOF[E_CAMERA_FOCUS_DOF_MAX] =		// dof table (temporary, need calibrarte)
{									// lens[um]	best[m]		far[m]		near[m]		adj_on	adj_lens_um
	[E_CAMERA_FOCUS_DOF_INF] 	=	{0.00,		DoF_INF,	DoF_INF,	DoF_INF,	OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_1000CM]	=	{2.00,		10,			DoF_INF,	2.74,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_500CM] 	=	{4.00,		5,			DoF_INF,	2.15,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_400CM] 	=	{5.00,		4,			DoF_INF,	1.95,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_300CM] 	=	{6.00,		3,			14.563,		1.67,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_200CM] 	=	{10.00,		2,			4.240,		1.31,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_190CM] 	=	{10.00,		1.9,		3.820,		1.27,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_180CM] 	=	{11.00,		1.8,		3.430,		1.22,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_170CM] 	=	{11.00,		1.7,		3.090,		1.17,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_160CM] 	=	{12.00,		1.6,		2.770,		1.13,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_150CM] 	=	{13.00,		1.5,		2.480,		1.08,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_140CM] 	=	{14.00,		1.4,		2.220,		1.02,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_130CM] 	=	{15.00,		1.3,		1.980,		0.97,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_120CM] 	=	{16.00,		1.2,		1.760,		0.91,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_110CM] 	=	{17.00,		1.1,		1.550,		0.85,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_100CM] 	=	{19.00,		1,			1.360,		0.79,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_90CM] 	=	{21.00,		0.9,		1.180,		0.73,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_80CM] 	=	{24.00,		0.8,		1.010,		0.66,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_70CM] 	=	{27.00,		0.7,		0.860,		0.59,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_60CM] 	=	{32.00,		0.6,		0.710,		0.52,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_50CM] 	=	{38.00,		0.5,		0.580,		0.44,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_45CM] 	=	{43.00,		0.45,		0.510,		0.403,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_40CM] 	=	{48.00,		0.4,		0.446,		0.362,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_35CM] 	=	{55.00,		0.35,		0.385,		0.321,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_30CM]	=	{64.00,		0.3,		0.325,		0.279,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_25CM] 	=	{77.00,		0.25,		0.267,		0.235,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_20CM] 	=	{97.00,		0.2,		0.211,		0.190,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_15CM] 	=	{131.00,	0.15,		0.156,		0.145,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_14CM] 	=	{140.00,	0.14,		0.145,		0.135,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_13CM] 	=	{151.00,	0.13,		0.134,		0.126,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_12CM] 	=	{164.00,	0.12,		0.124,		0.117,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_11CM] 	=	{180.00,	0.11,		0.113,		0.107,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_10CM] 	=	{199.00,	0.1,		0.102,		0.098,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_9CM] 	=	{222.00,	0.09,		0.092,		0.088,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_8CM] 	=	{251.00,	0.08,		0.082,		0.079,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_7CM] 	=	{290.00,	0.07,		0.071,		0.069,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_6CM] 	=	{342.00,	0.06,		0.061,		0.059,		OFF,	0.000000000},
	[E_CAMERA_FOCUS_DOF_5CM] 	=	{417.00,	0.05,		0.051,		0.049,		OFF,	0.000000000},
};

const T_CAMERA_FOCUS_BASIC	ZC525_BASIC =
{
	.access = E_CAMERA_ACCESS_I2C,
	.focus_if = E_CAMERA_FOCUS_IF_VCM,
	.i2c_ch = FJ_PERI_CH_0,
	.i2c_slave7bit = 0x0C,

	.mini_value = D_DAC_POS_MIN,
	.max_value = D_DAC_POS_MAX,
	.coarse_fw_step_num = (D_DAC_POS_MAX - D_DAC_POS_FW_INIT) / D_DAC_POS_COARSE_FW_SETP,
	.coarse_bw_step_num = (D_DAC_POS_BW_INIT - D_DAC_POS_MIN) / D_DAC_POS_COARSE_BW_SETP,
	.fine_fw_step_num = (D_DAC_POS_COARSE_FW_SETP * 2) / D_DAC_POS_FINE_FW_SETP,
	.fine_bw_step_num = (D_DAC_POS_COARSE_BW_SETP * 2) / D_DAC_POS_FINE_BW_SETP,
};

static USHORT gDacValue_Curr[E_CAMERA_ID_MAX] = {D_DAC_POS_INIT, D_DAC_POS_INIT, D_DAC_POS_INIT, D_DAC_POS_INIT};
/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

E_CAMERA_ERR_CODE	Focus_VCM_ZC525_Init(E_CAMERA_ID focus_id, T_CAMERA_FOCUS_PARAM* prm)
{
	UCHAR	maker_id, model_id;
	UCHAR	data;

	Camera_Common_I2C_Read_2(CAMERA.focus[focus_id].basic->i2c_ch,
							CAMERA.focus[focus_id].basic->i2c_slave7bit,
							REG_00,
							&data,
							1);
	maker_id = data >> 4;
	model_id = data & 0x0F;
	printf("  Focus Maker ID = 0x%02x, Device ID = 0x%02x\n", maker_id, model_id );


	data = 0x00; // Direct mode
	Camera_Common_I2C_Write_2(CAMERA.focus[focus_id].basic->i2c_ch,
							CAMERA.focus[focus_id].basic->i2c_slave7bit,
							REG_02,
							&data,
							1);

	data = 0x00; // Direct mode
	Camera_Common_I2C_Write_2(CAMERA.focus[focus_id].basic->i2c_ch,
							CAMERA.focus[focus_id].basic->i2c_slave7bit,
							REG_06,
							&data,
							1);

	data = 0x60; // default
	Camera_Common_I2C_Write_2(CAMERA.focus[focus_id].basic->i2c_ch,
							CAMERA.focus[focus_id].basic->i2c_slave7bit,
							REG_07,
							&data,
							1);
	
	init_T_CAMERA_FOCUS_PARAM( focus_id, prm );

	Focus_VCM_ZC525_SetDAC(focus_id, D_DAC_POS_INIT); // default
	
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE	Focus_VCM_ZC525_SetDAC(E_CAMERA_ID focus_id, USHORT dac)
{
	UCHAR	data[2];
	
	if(dac > D_DAC_POS_MAX) dac = D_DAC_POS_MAX; // 10bit resolution
	
	data[0] = (UCHAR)(dac >> 8);
	data[1] = (UCHAR)(dac & 0xFF);
	Camera_Common_I2C_Write_2(CAMERA.focus[focus_id].basic->i2c_ch,
							CAMERA.focus[focus_id].basic->i2c_slave7bit,
							REG_03,
							&data[0],
							2);
	gDacValue_Curr[focus_id] = dac;

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE	Focus_VCM_ZC525_GetDAC(E_CAMERA_ID focus_id, USHORT* value)
{
	*value = gDacValue_Curr[focus_id];

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE	Focus_VCM_ZC525_Move(E_CAMERA_ID focus_id, E_CAMERA_FOCUS_MOVE_DIR dir, BOOL is_coarse, USHORT* value)
{
	E_CAMERA_ERR_CODE	ret = E_CAMERA_ERR_OK;
	USHORT step_val;

	switch(dir)
	{
		case E_CAMERA_FOCUS_MOVE_DIR_FORWARD:
			if(is_coarse){
				step_val = D_DAC_POS_COARSE_FW_SETP;
			}else{
				step_val = D_DAC_POS_FINE_FW_SETP;
			}
			
			if((gDacValue_Curr[focus_id] + step_val) > D_DAC_POS_MAX){
				*value = gDacValue_Curr[focus_id];
				return E_CAMERA_ERR_NG;
			}else{
				gDacValue_Curr[focus_id] = gDacValue_Curr[focus_id] + step_val;
			}
		break;
		case E_CAMERA_FOCUS_MOVE_DIR_BACKWARD:
			if(is_coarse){
				step_val = D_DAC_POS_COARSE_BW_SETP;
			}else{
				step_val = D_DAC_POS_FINE_BW_SETP;
			}
			
			if(gDacValue_Curr[focus_id] < (step_val + D_DAC_POS_MIN)){
				*value = gDacValue_Curr[focus_id];
				return E_CAMERA_ERR_NG;
			}else{
				gDacValue_Curr[focus_id] = gDacValue_Curr[focus_id] - step_val;
			}
		break;
		case E_CAMERA_FOCUS_MOVE_DIR_FORWARD_INIT:
			gDacValue_Curr[focus_id] = D_DAC_POS_FW_INIT;
		break;
		case E_CAMERA_FOCUS_MOVE_DIR_BACKWARD_INIT:
			gDacValue_Curr[focus_id] = D_DAC_POS_BW_INIT;
		break;
		default:
			gDacValue_Curr[focus_id] = D_DAC_POS_INIT;
		break;
	}

	ret = Focus_VCM_ZC525_SetDAC(focus_id, gDacValue_Curr[focus_id]);
	*value = gDacValue_Curr[focus_id];

	return ret;
}
//float now_um;
//USHORT now_dac;
E_CAMERA_ERR_CODE	Focus_VCM_ZC525_CalcDAC(E_CAMERA_ID focus_id, T_CAMERA_FOCUS_PARAM* prm, UCHAR convert)
{
	FLOAT work;
	
	if( convert == D_FOCUS_DAC2LENS ){
		prm->now_um = prm->kando_slope * prm->now_dac + prm->kando_intercept;
	}else if( convert == D_FOCUS_LENS2DAC ){
//		now_um = prm->now_um;
		work = (prm->now_um - prm->kando_intercept ) / prm->kando_slope;
//		printf("now_um %f", prm->now_um);
//		printf("intercept %f", prm->kando_intercept);
//		printf("slope %f", prm->kando_slope);
//		printf("work %f", work);
//		printf("min %d", CAMERA.focus[focus_id].basic->mini_value);
//		printf("max %d\n", CAMERA.focus[focus_id].basic->max_value);
		if( work < CAMERA.focus[focus_id].basic->mini_value ){
			work = CAMERA.focus[focus_id].basic->mini_value;
		}
		if( work > CAMERA.focus[focus_id].basic->max_value ){
			work = CAMERA.focus[focus_id].basic->max_value;
		}
		prm->now_dac = (USHORT) work;
		// Recalculation for DAC clipping
		prm->now_um = prm->kando_slope * prm->now_dac + prm->kando_intercept;
//		now_dac = prm->now_dac;
	}

	return E_CAMERA_ERR_OK;
}

static E_CAMERA_ERR_CODE	init_T_CAMERA_FOCUS_PARAM( E_CAMERA_ID focus_id, T_CAMERA_FOCUS_PARAM* prm )
{
	float lens_10cm;
	float lens_1000cm;

	prm->inf_dac = D_DAC_POS_FW_INIT;	// Please set this parameter by using Calibration data.
	prm->mac_dac = D_DAC_POS_BW_INIT;	// Please set this parameter by using Calibration data.
	
	prm->sta_dac = prm->inf_dac;		// This parameter is not used..
	prm->end_dac = prm->mac_dac;		// This parameter is not used..
	prm->ini_dac = prm->sta_dac;		// This parameter is not used..
	prm->pos_dac = 0;
	
	lens_10cm   = CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_10CM].lens_um;
	lens_1000cm = CAMERA.focus[focus_id].dof[E_CAMERA_FOCUS_DOF_INF].lens_um;
	prm->kando_slope = (lens_10cm - lens_1000cm) / (prm->mac_dac - prm->inf_dac);
	prm->kando_intercept = lens_10cm - (prm->mac_dac *  prm->kando_slope);
/*	
	printf("***** Lens Paarameters\n ***");
	printf("  inf_dac:0x%x\n", prm->inf_dac);
	printf("  mac_dac:0x%x\n", prm->mac_dac);
	printf("  sta_dac:0x%x\n", prm->sta_dac);
	printf("  end_dac:0x%x\n", prm->end_dac);
	printf("  pos_dac:0x%x\n", prm->pos_dac);
	printf("  kando_slope:%f\n", prm->kando_slope);
	printf("  kando_intercept:%f\n", prm->kando_intercept);
	printf("  lens_10cm:%f\n", lens_10cm);
	printf("  lens_1000cm:%f\n", lens_1000cm);
*/	
	return E_CAMERA_ERR_OK;
}

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/


