/**
 * @file
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "camera_sensor_pseudo.h"
#include "LT6911UX.h"
#include "camera_config.h"
#include "os_user_custom.h"
#include <math.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	D_TBL_END		(0xFFFF)
#define	D_TBL_WAIT_MS	(0xFFFE)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
const T_CAMERA_SENSOR_BASIC		PSEUDO_BASIC =
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_24MHz,
	.access			= E_CAMERA_ACCESS_I2C,
	.sensor_if		= E_CAMERA_SENSOR_IF_DMIPI,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_VD,
};
const T_CAMERA_SENSOR_MODE		PSEUDO_MODE[E_SENSOR_PSEUDO_MODE_MAX] =
{
	[E_SENSOR_PSEUDO_MODE_Full_3840x2160_RAW10_30fps] = {
		.mode_reg			= NULL,
		.dol_mode_reg		= NULL,

		.embedded_lines			= 0,
		.total_pix_width		= 3840,
		.total_pix_lines		= 2160,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 0,
		.active_pix_y_pos		= 0,
		.active_pix_width		= 3840,
		.active_pix_lines		= 2160,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.input_bit_width		= 10,
		.dol_input_bit_width	= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,

		.hdocyc					= 4512,	//dummy
		.vdocyc					= 3092,	//dummy
		.exp_end_line_offset	= 20,	//dummy
		.exp_fine				= 0.429,//dummy
		.base_SV				= 4.0,

		.Mpps_HDclk				= 840.0,	//dummy
		.one_lane_Mbps			= 2100.0,	//dummy
		.lane_num				= 4,

		.fps					= 60.21,   // 840000000 / 4512 / 3092 // dummy
	},
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_PSEUDO_Clock_Gpio_Init(VOID)
{
#if 0
	INT32 sensor_id;

	// Clock ★平気？ sensor 全て同じにすべき？
	switch( CAMERA.sensor[0].basic->input_clock )
	{
	case E_CAMERA_SENSOR_INCLK_24MHz:
		Dd_Top_Set_PLLCNT3_P00PREDIV(1);	// 40MHz / (val+1)
		Dd_Top_Set_PLLCNT4_P00FNUM(0);		//
		Dd_Top_Set_PLLCNT5_P00FDEN(0);		//
		Dd_Top_Set_PLLCNT3_P00PLLDIV(60);	// * val
		Dd_Top_Set_PLLCNT3_P00POSTDIV(49);	// / (val+1)
		Dd_Top_Start_Pll00();				// PLL00 Start
		break;
	case E_CAMERA_SENSOR_INCLK_72MHz:
		Dd_Top_Set_PLLCNT3_P00PREDIV(1);	// 40MHz / (val+1)
		Dd_Top_Set_PLLCNT4_P00FNUM(0);		//
		Dd_Top_Set_PLLCNT5_P00FDEN(0);		//
		Dd_Top_Set_PLLCNT3_P00PLLDIV(90);	// * val
		Dd_Top_Set_PLLCNT3_P00POSTDIV(24);	// / (val+1)
		Dd_Top_Start_Pll00();				// PLL00 Start
		break;

	default:
		return E_CAMERA_ERR_NG;
		//break;
	}

	// Sensor Initialization ★電源立上げシーケンスは平気？？？
	for( sensor_id = 0; sensor_id < CAMERA.sensor_num; sensor_id++){

		// Clock
		switch(sensor_id){
		case 0:
			FJ_Top_Set_Clock_Select_AP0(FJ_TOP_SNAPCLKO_CLKSEL_PLL0);
			Dd_Top_Set_CLKSTOP4_APCK0(0);
			break;
		case 1:
			FJ_Top_Set_Clock_Select_AP1(FJ_TOP_SNAPCLKO_CLKSEL_PLL0);
			Dd_Top_Set_CLKSTOP4_APCK1(0);
			break;
		case 2:
			FJ_Top_Set_Clock_Select_AP2(FJ_TOP_SNAPCLKO_CLKSEL_PLL0);
			Dd_Top_Set_CLKSTOP4_APCK2(0);
			break;
		case 3:
			FJ_Top_Set_Clock_Select_AP3(FJ_TOP_SNAPCLKO_CLKSEL_PLL0);
			Dd_Top_Set_CLKSTOP4_APCK3(0);
			break;
		default:
			break;
		}

		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_inck, 1 );	// CLK from Sensor

		// Release Reset
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xclr, 0 );	// use as GPIO
		Dd_Top_Set_Gpio_Direction( CAMERA.sensor[sensor_id].port_xclr, 1 );	// output
		Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 1 );	// XCLR
		OS_User_Dly_Tsk(1);		// Wait Sensor Stability (Needs Over 100ns)

		// XVS XHS
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xhs, 1 );	// XHS from Sensor
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xvs, 1 );	// XVS from Sensor

		// init setting
		if( NULL != CAMERA.sensor[sensor_id].Func_Init ){
			if( CAMERA.sensor[sensor_id].Func_Init(sensor_id) == E_CAMERA_ERR_NG ){
				CAMERA_PRINT_ERROR(1);
				printf( "Sensor %d NOT connected\n", sensor_id );
			}
		}
	}
#else
	LT6911UX_GPIO_init();
#endif

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_PSEUDO_Standby(E_CAMERA_ID sensor_id)
{
	LT6911UX_StartStop_Straming( 0, FALSE );
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_PSEUDO_Stream(E_CAMERA_ID sensor_id)
{
	T_SHARE_HDMI_IN_VA_INFO	va_info;
	LT6911UX_inqury_va_info( 0, &va_info );
	LT6911UX_StartStop_Straming( 0, TRUE );
	return E_CAMERA_ERR_OK;
}


E_CAMERA_ERR_CODE Sensor_PSEUDO_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
{
	UINT32	CIT;	// Coarse Integration Time
	UINT32	FLL;	// Frame Length Line
	UINT32	AGG;	// Analog Gain Global
	UINT32	DG;		// Digital Gain
	FLOAT	exp_sec;
	FLOAT	again;
	FLOAT	dgain;
	FLOAT	default_fps, min_fps, max_fps, req_fps;

	// Coarse Integration Time
	exp_sec = in->exp_msec / 1000.0;
	CIT = (UINT32) ( exp_sec * (PSEUDO_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / PSEUDO_MODE[mode].hdocyc - PSEUDO_MODE[mode].exp_fine);

	if( CIT > ( 65525 - PSEUDO_MODE[mode].exp_end_line_offset ) ){ // 65525 is max of FLL
		CIT = ( 65525 - PSEUDO_MODE[mode].exp_end_line_offset );
	}

	if( CIT == 0 ){
		CIT = 1;
	}

	// Frame Length Line
	if( CIT > PSEUDO_MODE[mode].vdocyc - PSEUDO_MODE[mode].exp_end_line_offset ){
		FLL = CIT + PSEUDO_MODE[mode].exp_end_line_offset;
	}
	else{
		FLL = PSEUDO_MODE[mode].vdocyc;
	}

	// Clip by FPS
	if( in->max_fps == D_CAMERA_FPS_AUTO && in->min_fps == D_CAMERA_FPS_AUTO ){
		// auto, nothing to do
	}else{
		default_fps = (PSEUDO_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)PSEUDO_MODE[mode].hdocyc / (FLOAT)PSEUDO_MODE[mode].vdocyc;
		// check min_fps
		if( in->min_fps == D_CAMERA_FPS_AUTO ){
			min_fps = D_CAMERA_FPS_AUTO;
		}else if( in->min_fps > default_fps ){
			min_fps = default_fps;
		}else{
			min_fps = in->min_fps;
		}
		// check max_fps
		if( in->max_fps == D_CAMERA_FPS_AUTO ){
			max_fps = D_CAMERA_FPS_AUTO;
		}else if( in->max_fps > default_fps ){
			max_fps = default_fps;
		}else{
			max_fps = in->max_fps;
		}
		// check min_fps vs max_fps
		if( max_fps < min_fps ){
			min_fps = max_fps;
		}
		// clip FLL
		req_fps = (PSEUDO_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)PSEUDO_MODE[mode].hdocyc / (FLOAT)FLL;
		if( req_fps < min_fps ){
			FLL = (PSEUDO_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)PSEUDO_MODE[mode].hdocyc / min_fps;
		}
		if( req_fps > max_fps ){
			FLL = (PSEUDO_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)PSEUDO_MODE[mode].hdocyc / max_fps;
		}
		// clip CIT
		if( CIT > ( FLL - PSEUDO_MODE[mode].exp_end_line_offset ) ){
			CIT = ( FLL - PSEUDO_MODE[mode].exp_end_line_offset );
		}
	}

	// Analog Gain
	again = in->gain_times;
	if( again < 1.0 ){
		again = 1.0;
	}

	AGG = (UINT32)( 1024.0 - (1024.0 / again) );
	if( AGG > 978 ){
		AGG = 978;
	}
	again = ( 1024.0 / (1024.0 - (FLOAT)AGG) );

	// Digital Gain if again cannot equal to in->gain_times
	dgain = in->gain_times / again;
	DG = (UINT32)( dgain * 256.0 );
	if( DG < 0x100 ){
		DG = 0x100;
	}
	if( DG > 0xFD9 ){
		DG = 0xFD9;
	}
	dgain = (FLOAT)DG/256.0;

	// Revert
	exp_sec = (FLOAT)( CIT + PSEUDO_MODE[mode].exp_fine ) * (FLOAT)PSEUDO_MODE[mode].hdocyc / (PSEUDO_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	out->exp_msec = exp_sec * 1000.0;
	out->fps = (PSEUDO_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / PSEUDO_MODE[mode].hdocyc / FLL;
	out->hdocyc = PSEUDO_MODE[mode].hdocyc;
	out->vdocyc = FLL;
	out->again_times = again;
	out->dgain_times = dgain;

	// Reg
	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = PSEUDO_MODE[mode].hdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = FLL;
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] = CIT;
	out_reg->reg[E_CAMERA_SENSOR_REG_AGAIN] = AGG;
	out_reg->reg[E_CAMERA_SENSOR_REG_DGAIN] = DG;

	return E_CAMERA_ERR_OK;
}



/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
