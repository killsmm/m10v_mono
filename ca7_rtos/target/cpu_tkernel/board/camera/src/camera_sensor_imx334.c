/**
 * @file
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2021 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "camera_sensor_imx334.h"
#include "camera_config.h"
#include "os_user_custom.h"
#include <math.h>
#include "ae_task.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	D_TBL_END		(0xFFFF)
#define	D_TBL_WAIT_MS	(0xFFFE)

#define D_MAX_SENSOR_GAIN 72.0 // 72dB
#define D_MAX_SENSOR_AGAIN 30.0 // 30dB

#define ROUNDUP4(a)		(((a) + 3) & 0xFFFFFFFC)
#define ROUNDDOWN4(a)	(((a) / 4) * 4)

#define ROUNDUP2(a)		(((a) + 1) & 0xFFFFFFFE)
#define ROUNDDOWN2(a)	(((a) / 2) * 2)
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static E_CAMERA_ERR_CODE access_tbl(E_CAMERA_ID sensor_id, const T_CAMERA_SENSOR_REG* tbl);

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
const T_CAMERA_SENSOR_REG IMX334_MODE_ALL_PIX_SCAN_RAW12_60fps[] =
{
	{ .address = 0x300c, .data[0] = 0x3b },
    { .address = 0x300d, .data[0] = 0x2a },
	{ .address = 0x3048, .data[0] = 0x00 },
	{ .address = 0x3049, .data[0] = 0x00 },
	{ .address = 0x304a, .data[0] = 0x00 },
	{ .address = 0x304b, .data[0] = 0x01 },
	{ .address = 0x304c, .data[0] = 0x14 },
	{ .address = 0x3058, .data[0] = 0x05 },
	{ .address = 0x3059, .data[0] = 0x00 },
	{ .address = 0x3068, .data[0] = 0x8b },
    { .address = 0x314c, .data[0] = 0x29 },
    { .address = 0x314d, .data[0] = 0x01 },
    { .address = 0x315a, .data[0] = 0x02 },
    { .address = 0x3168, .data[0] = 0xa0 },
    { .address = 0x316a, .data[0] = 0x7e },
	{ .address = 0x319f, .data[0] = 0x03 },
    { .address = 0x31a1, .data[0] = 0x00 },
	{ .address = 0x31d7, .data[0] = 0x00 },
	{ .address = 0x3200, .data[0] = 0x11 },
    { .address = 0x3288, .data[0] = 0x21 },
    { .address = 0x328a, .data[0] = 0x02 },
    { .address = 0x3414, .data[0] = 0x05 },
    { .address = 0x3416, .data[0] = 0x18 },
    { .address = 0x35AC, .data[0] = 0x0E },
    { .address = 0x3648, .data[0] = 0x01 },
    { .address = 0x364A, .data[0] = 0x04 },
    { .address = 0x364C, .data[0] = 0x04 },
    { .address = 0x3678, .data[0] = 0x01 },
    { .address = 0x367C, .data[0] = 0x31 },
    { .address = 0x367E, .data[0] = 0x31 },
    { .address = 0x3708, .data[0] = 0x02 },
    { .address = 0x3714, .data[0] = 0x01 },
    { .address = 0x3715, .data[0] = 0x02 },
    { .address = 0x3716, .data[0] = 0x02 },
    { .address = 0x3717, .data[0] = 0x02 },
    { .address = 0x371C, .data[0] = 0x3D },
    { .address = 0x371D, .data[0] = 0x3F },
    { .address = 0x372C, .data[0] = 0x00 },
    { .address = 0x372D, .data[0] = 0x00 },
    { .address = 0x372E, .data[0] = 0x46 },
    { .address = 0x372F, .data[0] = 0x00 },
    { .address = 0x3730, .data[0] = 0x89 },
    { .address = 0x3731, .data[0] = 0x00 },
    { .address = 0x3732, .data[0] = 0x08 },
    { .address = 0x3733, .data[0] = 0x01 },
    { .address = 0x3734, .data[0] = 0xFE },
    { .address = 0x3735, .data[0] = 0x05 },
    { .address = 0x375D, .data[0] = 0x00 },
    { .address = 0x375E, .data[0] = 0x00 },
    { .address = 0x375F, .data[0] = 0x61 },
    { .address = 0x3760, .data[0] = 0x06 },
    { .address = 0x3768, .data[0] = 0x1B },
    { .address = 0x3769, .data[0] = 0x1B },
    { .address = 0x376A, .data[0] = 0x1A },
    { .address = 0x376B, .data[0] = 0x19 },
    { .address = 0x376C, .data[0] = 0x18 },
    { .address = 0x376D, .data[0] = 0x14 },
    { .address = 0x376E, .data[0] = 0x0F },
    { .address = 0x3776, .data[0] = 0x00 },
    { .address = 0x3777, .data[0] = 0x00 },
    { .address = 0x3778, .data[0] = 0x46 },
    { .address = 0x3779, .data[0] = 0x00 },
    { .address = 0x377A, .data[0] = 0x08 },
    { .address = 0x377B, .data[0] = 0x01 },
    { .address = 0x377C, .data[0] = 0x45 },
    { .address = 0x377D, .data[0] = 0x01 },
    { .address = 0x377E, .data[0] = 0x23 },
    { .address = 0x377F, .data[0] = 0x02 },
    { .address = 0x3780, .data[0] = 0xD9 },
    { .address = 0x3781, .data[0] = 0x03 },
    { .address = 0x3782, .data[0] = 0xF5 },
    { .address = 0x3783, .data[0] = 0x06 },
    { .address = 0x3784, .data[0] = 0xA5 },
    { .address = 0x3788, .data[0] = 0x0F },
    { .address = 0x378A, .data[0] = 0xD9 },
    { .address = 0x378B, .data[0] = 0x03 },
    { .address = 0x378C, .data[0] = 0xEB },
    { .address = 0x378D, .data[0] = 0x05 },
    { .address = 0x378E, .data[0] = 0x87 },
    { .address = 0x378F, .data[0] = 0x06 },
    { .address = 0x3790, .data[0] = 0xF5 },
    { .address = 0x3792, .data[0] = 0x43 },
    { .address = 0x3794, .data[0] = 0x7A },
    { .address = 0x3796, .data[0] = 0xA1 },
    { .address = 0x3e04, .data[0] = 0x0e },
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
};

const T_CAMERA_SENSOR_REG IMX334_MODE_ALL_PIX_SCAN_RAW12_60fps_DOL2[] =
{
	{ .address = 0x300c, .data[0] = 0x3b },
    { .address = 0x300d, .data[0] = 0x2a },
	{ .address = 0x3048, .data[0] = 0x01 },
	{ .address = 0x3049, .data[0] = 0x01 },
	{ .address = 0x304a, .data[0] = 0x01 },
	{ .address = 0x304b, .data[0] = 0x02 },
	{ .address = 0x304c, .data[0] = 0x13 },
	{ .address = 0x3068, .data[0] = 0x19 },
    { .address = 0x314c, .data[0] = 0x29 },
    { .address = 0x314d, .data[0] = 0x01 },
    { .address = 0x315a, .data[0] = 0x02 },
    { .address = 0x3168, .data[0] = 0xa0 },
    { .address = 0x316a, .data[0] = 0x7e },
	{ .address = 0x319f, .data[0] = 0x03 },
    { .address = 0x31a1, .data[0] = 0x00 },
	{ .address = 0x31d7, .data[0] = 0x01 },
	{ .address = 0x3200, .data[0] = 0x10 },
    { .address = 0x3288, .data[0] = 0x21 },
    { .address = 0x328a, .data[0] = 0x02 },
    { .address = 0x3414, .data[0] = 0x05 },
    { .address = 0x3416, .data[0] = 0x18 },
    { .address = 0x35AC, .data[0] = 0x0E },
    { .address = 0x3648, .data[0] = 0x01 },
    { .address = 0x364A, .data[0] = 0x04 },
    { .address = 0x364C, .data[0] = 0x04 },
    { .address = 0x3678, .data[0] = 0x01 },
    { .address = 0x367C, .data[0] = 0x31 },
    { .address = 0x367E, .data[0] = 0x31 },
    { .address = 0x3708, .data[0] = 0x02 },
    { .address = 0x3714, .data[0] = 0x01 },
    { .address = 0x3715, .data[0] = 0x02 },
    { .address = 0x3716, .data[0] = 0x02 },
    { .address = 0x3717, .data[0] = 0x02 },
    { .address = 0x371C, .data[0] = 0x3D },
    { .address = 0x371D, .data[0] = 0x3F },
    { .address = 0x372C, .data[0] = 0x00 },
    { .address = 0x372D, .data[0] = 0x00 },
    { .address = 0x372E, .data[0] = 0x46 },
    { .address = 0x372F, .data[0] = 0x00 },
    { .address = 0x3730, .data[0] = 0x89 },
    { .address = 0x3731, .data[0] = 0x00 },
    { .address = 0x3732, .data[0] = 0x08 },
    { .address = 0x3733, .data[0] = 0x01 },
    { .address = 0x3734, .data[0] = 0xFE },
    { .address = 0x3735, .data[0] = 0x05 },
    { .address = 0x375D, .data[0] = 0x00 },
    { .address = 0x375E, .data[0] = 0x00 },
    { .address = 0x375F, .data[0] = 0x61 },
    { .address = 0x3760, .data[0] = 0x06 },
    { .address = 0x3768, .data[0] = 0x1B },
    { .address = 0x3769, .data[0] = 0x1B },
    { .address = 0x376A, .data[0] = 0x1A },
    { .address = 0x376B, .data[0] = 0x19 },
    { .address = 0x376C, .data[0] = 0x18 },
    { .address = 0x376D, .data[0] = 0x14 },
    { .address = 0x376E, .data[0] = 0x0F },
    { .address = 0x3776, .data[0] = 0x00 },
    { .address = 0x3777, .data[0] = 0x00 },
    { .address = 0x3778, .data[0] = 0x46 },
    { .address = 0x3779, .data[0] = 0x00 },
    { .address = 0x377A, .data[0] = 0x08 },
    { .address = 0x377B, .data[0] = 0x01 },
    { .address = 0x377C, .data[0] = 0x45 },
    { .address = 0x377D, .data[0] = 0x01 },
    { .address = 0x377E, .data[0] = 0x23 },
    { .address = 0x377F, .data[0] = 0x02 },
    { .address = 0x3780, .data[0] = 0xD9 },
    { .address = 0x3781, .data[0] = 0x03 },
    { .address = 0x3782, .data[0] = 0xF5 },
    { .address = 0x3783, .data[0] = 0x06 },
    { .address = 0x3784, .data[0] = 0xA5 },
    { .address = 0x3788, .data[0] = 0x0F },
    { .address = 0x378A, .data[0] = 0xD9 },
    { .address = 0x378B, .data[0] = 0x03 },
    { .address = 0x378C, .data[0] = 0xEB },
    { .address = 0x378D, .data[0] = 0x05 },
    { .address = 0x378E, .data[0] = 0x87 },
    { .address = 0x378F, .data[0] = 0x06 },
    { .address = 0x3790, .data[0] = 0xF5 },
    { .address = 0x3792, .data[0] = 0x43 },
    { .address = 0x3794, .data[0] = 0x7A },
    { .address = 0x3796, .data[0] = 0xA1 },
    { .address = 0x3e04, .data[0] = 0x0e },
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
};

const T_CAMERA_SENSOR_BASIC		IMX334_BASIC =
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_24MHz,
	.access			= E_CAMERA_ACCESS_I2C,
	.sensor_if		= E_CAMERA_SENSOR_IF_DMIPI,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_VD,
};
const T_CAMERA_SENSOR_MODE		IMX334_MODE[E_SENSOR_IMX334_MODE_MAX] =
{
	[E_SENSOR_IMX334_MODE_ALL_PIX_SCAN_RAW12_60fps] = {
		.mode_reg			    = IMX334_MODE_ALL_PIX_SCAN_RAW12_60fps,
		.dol_mode_reg		    = IMX334_MODE_ALL_PIX_SCAN_RAW12_60fps_DOL2,
		.pdaf_mode_reg		    = NULL,

		.embedded_lines			= 1, // for stitch calc only, not useful
		.total_pix_width		= 3864,
		// sensor spec is 2200
		// DOL case: EBD (1) + Ignored OB 1 and 2 (3+3) + effective OB (13), active_pix_y_pos = 20
		// lines input to B2B is 2180: Ignored area (4) + Effect margin (8) + Effect (2160) + Effect margin (8) = 2180
		// since 4K 3840x2160, we need to input 2180 lines, 2180 - 20 (PRO_LOST_LINES) = 2160
		// DOL, VHDR is after S2Y, if line is shortage not 2160, EX:2158 -> vHDR process -> 2160, vHDR cannot work well

		// Normal case: EBD (1) + Ignored OB (7) + effective OB (13), active_pix_y_pos = 21 <-- [but odd 21 running in LTM mode, calc round up, the first pixel is changed]
		// so active_pix_y_pos we set 20 for both normal and DOL modes, so normal mode, 1st line will be black OB line
		// Normal, SGE is after S2Y, SGDE can support scale-up, EX:2158 -> SGDE -> 2160 is OK
		.total_pix_lines		= 2200,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 9,
		.ob_pix_width			= 3864,
		.ob_pix_lines			= 13,
		.active_pix_x_pos		= 0,
		.active_pix_y_pos		= 20,
		.active_pix_width		= 3864,
		.active_pix_lines		= 2180,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,

		.input_bit_width		= 12,
		.dol_input_bit_width	= 12,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_GB,
		.dol_first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,

		.hdocyc					= 550,
		.vdocyc					= 2250,
		.dol_hdocyc				= 550,
		.dol_vdocyc				= 2250,
        // Integration time = frame length - SHR0, so start line and end offset is reversed
        // SHR0 : 5 ~ (VMAX - 1) [1: start line, 5: end offset]
        // start line min = 1
        // exp_end_line_offset = 5
        .exp_start_line_min     = 1,
		.exp_end_line_offset	= 5,
		.exp_clk_offset			= 171, // 2.303us (2.303 / (1 / 74.25M * 1000000))
		.base_SV				= 4.0,

		.Mpps_HDclk				= 74.25,
		.one_lane_Mbps			= 1782.0,
		.lane_num				= 4,

		.fps					= 60.00,
		.dol_fps				= 30.0,
	},
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX334_Clock_Gpio_Init(VOID)
{
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

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX334_Standby(E_CAMERA_ID sensor_id)
{
	UCHAR					data;

	data = 1;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3000,
							&data,
							1);
    data = 1;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3002,
							&data,
							1);
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX334_Stream(E_CAMERA_ID sensor_id)
{
	UCHAR					data;

	data = 0;

	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3002,
							&data,
							1);
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX334_CancelStandby(E_CAMERA_ID sensor_id)
{
	UCHAR					data;

	data = 0;

	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3000,
							&data,
							1);

	OS_User_Dly_Tsk(18);	// Wait Sensor Stability (Needs Over 18ms)
    // 8 abnormal frames
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX334_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol)
{
	return dol ? access_tbl(sensor_id, IMX334_MODE[mode].dol_mode_reg) : access_tbl(sensor_id, IMX334_MODE[mode].mode_reg);
}

E_CAMERA_ERR_CODE Sensor_IMX334_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
{
	UINT32	CIT;	// Coarse Integration Time
	UINT32	FLL;	// Frame Length Line
	UINT32	AGG;	// Analog Gain Global
	UINT32	DG = 0;		// Digital Gain
	FLOAT	exp_sec;
	DOUBLE	again;
	DOUBLE	dgain;
	FLOAT	default_fps, min_fps, max_fps, req_fps;
	DOUBLE	realExpClk, realExpTime;

	// Coarse Integration Time
	exp_sec = in->exp_msec / 1000.0;
    CIT = (exp_sec * (IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) - (FLOAT)IMX334_MODE[mode].exp_clk_offset) / (FLOAT)IMX334_MODE[mode].hdocyc;

    // 1048575 (bit0~bit19) is max of FLL
	if (CIT > (1048575 - IMX334_MODE[mode].exp_end_line_offset))
		CIT = (1048575 - IMX334_MODE[mode].exp_end_line_offset);

	if (CIT < IMX334_MODE[mode].exp_start_line_min)
		CIT = IMX334_MODE[mode].exp_start_line_min;

	// Frame Length Line
	if (CIT > IMX334_MODE[mode].vdocyc - IMX334_MODE[mode].exp_end_line_offset)
		FLL = CIT + IMX334_MODE[mode].exp_end_line_offset;
    else
		FLL = IMX334_MODE[mode].vdocyc;

	// Clip by FPS
	if (in->max_fps != D_CAMERA_FPS_AUTO || in->min_fps != D_CAMERA_FPS_AUTO) {
		default_fps = (IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX334_MODE[mode].hdocyc / (FLOAT)IMX334_MODE[mode].vdocyc;
		// check min_fps
		if (in->min_fps == D_CAMERA_FPS_AUTO)
			min_fps = D_CAMERA_FPS_AUTO;
		else if (in->min_fps > default_fps)
			min_fps = default_fps;
		else
			min_fps = in->min_fps;

		// check max_fps
		if (in->max_fps == D_CAMERA_FPS_AUTO)
			max_fps = D_CAMERA_FPS_AUTO;
		else if (in->max_fps > default_fps)
			max_fps = default_fps;
		else
			max_fps = in->max_fps;

		// check min_fps vs max_fps
		if (max_fps < min_fps)
			min_fps = max_fps;

		// clip FLL
		req_fps = (IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX334_MODE[mode].hdocyc / (FLOAT)FLL;
		if (req_fps < min_fps)
			FLL = (IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX334_MODE[mode].hdocyc / min_fps;

		if (req_fps > max_fps)
			FLL = (IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX334_MODE[mode].hdocyc / max_fps;

		if (FLL < IMX334_MODE[mode].vdocyc)
			FLL = IMX334_MODE[mode].vdocyc;

		// clip CIT
		if (CIT > (FLL - IMX334_MODE[mode].exp_end_line_offset)) {
			CIT = (FLL - IMX334_MODE[mode].exp_end_line_offset);
			realExpClk  = (DOUBLE)CIT * IMX334_MODE[mode].hdocyc + IMX334_MODE[mode].exp_clk_offset;
			realExpTime = realExpClk / (DOUBLE)(IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
 			realExpTime = (DOUBLE)expTimeQuantization(realExpTime * 1000, in->flicker_mode, QUANTIZ_ROUND_OFF);
			CIT	= (ULONG)(((realExpTime / 1000.0) * (DOUBLE)(IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) - (DOUBLE)IMX334_MODE[mode].exp_clk_offset) / (DOUBLE)IMX334_MODE[mode].hdocyc);
		}

        if (CIT > (FLL - IMX334_MODE[mode].exp_end_line_offset))
			CIT = (FLL - IMX334_MODE[mode].exp_end_line_offset);

        if (CIT < IMX334_MODE[mode].exp_start_line_min)
		    CIT = IMX334_MODE[mode].exp_start_line_min;		
	}

	// Analog Gain in times
	again = in->gain_times;
	if( again < 1.0 ){
		again = 1.0;
	}

    // gain times -> dB
    again = (DOUBLE)20.0 * log10(again);
    if (again > D_MAX_SENSOR_GAIN)
        again = D_MAX_SENSOR_GAIN;

	AGG = (UINT32)(again * 10.0 / 3.0);
	again = (DOUBLE)AGG * 3.0 / 10.0;

    if (again >= D_MAX_SENSOR_AGAIN) {
        dgain = again - D_MAX_SENSOR_AGAIN;
        again = D_MAX_SENSOR_AGAIN;
    } else {
        dgain = 0.0; // dB
    }

    // gain dB -> gain times
    again =  pow(10.0, again / 20.0);
    dgain =  pow(10.0, dgain / 20.0);

	// Revert
	exp_sec = ((FLOAT) CIT * (FLOAT)IMX334_MODE[mode].hdocyc + IMX334_MODE[mode].exp_clk_offset) / (IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	out->exp_msec = exp_sec * 1000.0;
	out->fps = (IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / IMX334_MODE[mode].hdocyc / FLL;
	out->hdocyc = IMX334_MODE[mode].hdocyc;
	out->vdocyc = FLL;
	out->again_times = (FLOAT)again;
	out->dgain_times = (FLOAT)dgain;

	// Reg
	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = IMX334_MODE[mode].hdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = FLL;
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] = FLL - CIT;
	out_reg->reg[E_CAMERA_SENSOR_REG_AGAIN] = AGG;
	out_reg->reg[E_CAMERA_SENSOR_REG_DGAIN] = DG;

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX334_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in)
{
	UCHAR data[8];

	// group param hold ON
	data[0] = 0x01;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3001,
							data,
							1);
	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// coarse integration time & analog_gain_global
    data[0] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_EXP]       & 0xFF);
	data[1] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_EXP] >> 8) & 0xFF);
    data[2] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_EXP] >> 16) & 0x0F);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3058,
							data,
							3);
	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();
	data[0] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_AGAIN]       & 0xFF);
    data[1] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_AGAIN] >> 8) & 0x07);
    Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x30e8,
							data,
							2);

    Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// frame length line
	data[0] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_VDOCYC]       & 0xFF);
    data[1] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_VDOCYC] >> 8) & 0xFF);
    data[2] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_VDOCYC] >> 16) & 0x0F);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3030,
							data,
							3);
	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// group param hold OFF
	data[0] = 0;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3001,
							data,
							1);

#if defined(D_DEBUG_SENSOR_GAIN_REFLECT_TIME)
	MPRINTF((D_MBA_KIND_PREVIEW_DEBUG, FJ_MBALOG_LINE_ID_SENSOR, FJ_MBALOG_FUNC_ID_CCD, FJ_MBALOG_TYPE_POINT, "ag:%x", in->reg[E_CAMERA_SENSOR_REG_AGAIN]));
	//MPRINTF((D_MBA_KIND_PREVIEW_DEBUG, FJ_MBALOG_LINE_ID_SENSOR, FJ_MBALOG_FUNC_ID_CCD, FJ_MBALOG_TYPE_POINT, "dg:%x", in->reg[E_CAMERA_SENSOR_REG_DGAIN]));
#endif
#if defined(D_DEBUG_SENSOR_SHUTTER_REFLECT_TIME)
	MPRINTF((D_MBA_KIND_PREVIEW_DEBUG, FJ_MBALOG_LINE_ID_SENSOR, FJ_MBALOG_FUNC_ID_CCD, FJ_MBALOG_TYPE_POINT, "exp:%x", in->reg[E_CAMERA_SENSOR_REG_EXP]));
#endif
	return E_CAMERA_ERR_OK;
}

#define D_DOL_L		0
#define D_DOL_S		1

E_CAMERA_ERR_CODE Sensor_IMX334_DOL_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
{
	UINT32	BRL;			// Base Read Line
	UINT32	CIT[2];			// Coarse Integration Time
	UINT32	SHR0, SHR1;
	UINT32	FSC[2];			// Frame Set Count
	UINT32	RHS1;
	UINT32	AGG[2];			// Analog Gain Global
	UINT32	DG[2];			// Digital Gain
	UINT32	mgn[2];
	UINT32	minmgn[2];
	FLOAT	exp_sec[2];
	FLOAT	gain_times[2];
	FLOAT	again[2];
	FLOAT	dgain[2];
	FLOAT	default_fps, min_fps, max_fps, req_fps;
	DOUBLE	realExpClk, realExpTime;
	INT32	i;
	
	BRL = IMX334_MODE[mode].total_pix_lines; // from spec
	RHS1 = in->DOL_short_max_gap_lines;
	// RHS1 = 4n + 1
	RHS1 = ROUNDUP4(RHS1);
	RHS1++;

	// 9 <= SHR1 <= (RHS1-2), RHS1(min) at least 11
	if (RHS1 < 11) {
		RHS1 = 11;
		// RHS1 = 4n + 1
		RHS1 = ROUNDUP4(RHS1);
		RHS1++;
	}

	// RHS1 < (BRL x 2), RHS1(max) BRL x 2
	if (RHS1 >= (BRL * 2)) {
		RHS1 = BRL * 2 - 1;
		// RHS1 = 4n + 1
		RHS1 = ROUNDDOWN4(RHS1);
		RHS1++;
	}
#if defined(D_WITHIN_FSC_MODE)
	// within FSC mode, (VMAX x 2 - BRL x 2) / 2 - 2 > (RHS1 - 3) / 2
	if (((RHS1 - 3) / 2) >= ((IMX334_MODE[mode].dol_vdocyc * 2 - BRL * 2) / 2 - 2)) {
		RHS1 = ((IMX334_MODE[mode].dol_vdocyc * 2 - BRL * 2) / 2 - 2) * 2 + 3 - 1;
		// RHS1 = 4n + 1
		RHS1 = ROUNDDOWN4(RHS1);
		RHS1++;
	}
#endif
	exp_sec[D_DOL_L] = in->exp_msec / 1000.0;
	exp_sec[D_DOL_S] = in->DOL_short_exp_msec / 1000.0;
	
	// clip short exposure
	if (exp_sec[D_DOL_S] > exp_sec[D_DOL_L])
		exp_sec[D_DOL_S] = exp_sec[D_DOL_L];

	// (RHS1 + 9) <= SHR0 <= (FSC - 2), exposure time = FSC - SHR0 + 2.303us, so the max margin is reversed
	mgn[D_DOL_L] = RHS1 + 9;
	minmgn[D_DOL_L] = 2;
	// 9 <= SHR1 <= (RHS1 - 2)
	mgn[D_DOL_S] = 9;
	minmgn[D_DOL_S] = 2;

	for (i = 0; i < 2; i ++) {
		// Coarse Integration Time
		CIT[i] = (UINT32) ( exp_sec[i] * (IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) -
			(FLOAT)IMX334_MODE[mode].exp_clk_offset) / IMX334_MODE[mode].dol_hdocyc;

		if (i == D_DOL_L) {
			// SHR0: 2n, FSC: 4n, SHR0(2n) = FSC(4n) - CIT(2n)
			CIT[i] = ROUNDDOWN2(CIT[i]);

			if (CIT[i] > (1048575 * 2 - mgn[i])) { // VMAX:12bits = 1048575
				CIT[i] = 1048575 * 2 - mgn[i];
				CIT[i] = ROUNDDOWN2(CIT[i]);
			}

			if (CIT[i] < minmgn[i]) {
				CIT[i] = minmgn[i];
				CIT[i] = ROUNDUP2(CIT[i]);
			}

			// Frame Length Line
			if (CIT[i] > IMX334_MODE[mode].dol_vdocyc * 2 - mgn[i]) { // FSC: VMAX * 2
				FSC[i] = CIT[i] + mgn[i];
				FSC[i] = ROUNDUP4(FSC[i]);
			} else {
				FSC[i] = IMX334_MODE[mode].dol_vdocyc * 2; // FSC: VMAX * 2
				FSC[i] = ROUNDUP4(FSC[i]);
			}
		} else {
			// make sure CIT[i] is 2n, RHS1 always 4n + 1 (odd), so RHS1 (odd) - 2n (even) => SHR1 = 2n + 1 (odd)
			CIT[i] = ROUNDDOWN2(CIT[i]);

			if (CIT[i] < minmgn[i]) {
				CIT[i] = minmgn[i];
				CIT[i] = ROUNDUP2(CIT[i]);
			}

			if (CIT[i] > RHS1 - mgn[i]) {
				CIT[i] = RHS1 - mgn[i];
				CIT[i] = ROUNDDOWN2(CIT[i]);
			}
		}
	}

	// Clip by FPS, only for long exposure which affects FPS
	if( in->max_fps == D_CAMERA_FPS_AUTO && in->min_fps == D_CAMERA_FPS_AUTO ){
		// auto, nothing to do
	}else{
		// FSC = VMAX * 2
		default_fps = (IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX334_MODE[mode].dol_hdocyc /
			((FLOAT)IMX334_MODE[mode].dol_vdocyc * 2);		// check min_fps
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
		req_fps = (IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX334_MODE[mode].dol_hdocyc / (FLOAT)FSC[D_DOL_L];
		if( req_fps < min_fps )
			FSC[D_DOL_L] = (IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX334_MODE[mode].dol_hdocyc / min_fps;
		if( req_fps > max_fps )
			FSC[D_DOL_L] = (IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX334_MODE[mode].dol_hdocyc / max_fps;

		// 4n
		FSC[D_DOL_L] = ROUNDUP4(FSC[D_DOL_L]);

		// clip CIT
		if (CIT[D_DOL_L] > (FSC[D_DOL_L] - mgn[D_DOL_L])) {
			CIT[D_DOL_L] = (FSC[D_DOL_L] - mgn[D_DOL_L]);
			CIT[D_DOL_L] = ROUNDDOWN2(CIT[D_DOL_L]); //2n
			realExpClk  = (DOUBLE)(CIT[D_DOL_L] * IMX334_MODE[mode].dol_hdocyc + IMX334_MODE[mode].exp_clk_offset);
			realExpTime = realExpClk / (DOUBLE)(IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
			realExpTime = (DOUBLE)expTimeQuantization(realExpTime * 1000, in->flicker_mode, QUANTIZ_ROUND_OFF);
			CIT[D_DOL_L] = (ULONG) (((realExpTime / 1000.0) * (DOUBLE)(IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) -
				(DOUBLE)IMX334_MODE[mode].exp_clk_offset) / (DOUBLE)IMX334_MODE[mode].dol_hdocyc);
			CIT[D_DOL_L] = ROUNDDOWN2(CIT[D_DOL_L]); //2n
		}

		if (CIT[D_DOL_L] > (FSC[D_DOL_L] - mgn[D_DOL_L])) {
			CIT[D_DOL_L] = (FSC[D_DOL_L] - mgn[D_DOL_L]);
			CIT[D_DOL_L] = ROUNDDOWN2(CIT[D_DOL_L]); //2n
		}

		if (CIT[D_DOL_L] < minmgn[D_DOL_L]) {
			CIT[D_DOL_L] = minmgn[D_DOL_L];
			CIT[D_DOL_L] = ROUNDUP2(CIT[D_DOL_L]);
		}
	}
	
	// Check Error
	if (((RHS1 - 1) % 4) ||
		(RHS1 < (CIT[D_DOL_S] + 2)) ||
		(RHS1 > (CIT[D_DOL_L] - 9)) ||
		(RHS1 >= (BRL * 2))) {
		CAMERA_PRINT_ERROR(1);
		printf("RHS1:%d\n", RHS1);
	}

	if (FSC[D_DOL_L] % 4) {
		CAMERA_PRINT_ERROR(1);
		printf("FSC:%d\n", FSC[D_DOL_L]);
	}

	SHR0 = FSC[D_DOL_L] - CIT[D_DOL_L];
	if ((SHR0 % 2) ||
		(SHR0 < (RHS1 + 9)) ||
		(SHR0 > (FSC[D_DOL_L] - 2))) {
		CAMERA_PRINT_ERROR(1);
		printf("SHR0:%d\n", SHR0);
	}

	SHR1 = RHS1 - CIT[D_DOL_S];
	if (((SHR1 - 1) % 2) ||
		(SHR1 < 9) ||
		(SHR1 > (RHS1 - 2))) {
		CAMERA_PRINT_ERROR(1);
		printf("SHR1:%d\n", SHR1);
	}

	// Gain
	gain_times[D_DOL_L] = in->gain_times;
	gain_times[D_DOL_S] = in->DOL_short_gain_times;

	for( i = 0; i < 2; i ++ ){
		// Analog Gain
		again[i] = gain_times[i];
		if( again[i] < 1.0 ){
			again[i] = 1.0;
		}

		 // gain times -> dB
		again[i] = (DOUBLE)20.0 * log10(again[i]);
		if (again[i] > D_MAX_SENSOR_GAIN)
			again[i] = D_MAX_SENSOR_GAIN;

		AGG[i] = (UINT32)(again[i] * 10.0 / 3.0);
		again[i] = (DOUBLE)AGG[i] * 3.0 / 10.0;

		if (again[i] >= D_MAX_SENSOR_AGAIN) {
			dgain[i] = again[i] - D_MAX_SENSOR_AGAIN;
			again[i] = D_MAX_SENSOR_AGAIN;
		} else {
			dgain[i] = 0.0; // dB
		}

		// gain dB -> gain times
		again[i] =  pow(10.0, again[i] / 20.0);
		dgain[i] =  pow(10.0, dgain[i] / 20.0);	
	}

	// Revert
	exp_sec[D_DOL_L] = ((FLOAT)CIT[D_DOL_L] * (FLOAT)IMX334_MODE[mode].dol_hdocyc + (FLOAT)IMX334_MODE[mode].exp_clk_offset) /
		((FLOAT)IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	out->exp_msec = exp_sec[D_DOL_L] * 1000.0;
	out->fps = (IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / IMX334_MODE[mode].dol_hdocyc / FSC[D_DOL_L];
	out->hdocyc = IMX334_MODE[mode].dol_hdocyc;
	out->vdocyc = FSC[D_DOL_L] >> 1;
	out->again_times = again[D_DOL_L];
	out->dgain_times = dgain[D_DOL_L];
	exp_sec[D_DOL_S] = ((FLOAT)CIT[D_DOL_S] * (FLOAT)IMX334_MODE[mode].dol_hdocyc + (FLOAT)IMX334_MODE[mode].exp_clk_offset) /
		((FLOAT)IMX334_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	out->DOL_short_exp_msec = exp_sec[D_DOL_S] * 1000.0;
	out->DOL_short_again_times = again[D_DOL_S];
	out->DOL_short_dgain_times = dgain[D_DOL_S];
	
	// Reg
	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = IMX334_MODE[mode].dol_hdocyc;
	// FSC: VMAX * 2, it should be FSC[D_DOL_L] >> 1
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = FSC[D_DOL_L] >> 1;
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] = SHR0;
	out_reg->reg[E_CAMERA_SENSOR_REG_AGAIN] = AGG[D_DOL_L];
	out_reg->reg[E_CAMERA_SENSOR_REG_DGAIN] = DG[D_DOL_L];
	out_reg->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_OFST] = RHS1;
	out_reg->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_EXP] = SHR1;
	out_reg->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_AGAIN] = AGG[D_DOL_S];
	out_reg->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_DGAIN] = DG[D_DOL_S];

	return E_CAMERA_ERR_OK;
}


E_CAMERA_ERR_CODE Sensor_IMX334_DOL_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in )
{
	UCHAR data[8];

	// group param hold ON
	data[0] = 0x01;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3001,
							data,
							1);

	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// Exposure
	data[0] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_EXP]       & 0xFF);
	data[1] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_EXP] >> 8) & 0xFF);
	data[2] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_EXP] >> 16) & 0x0F);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3058,
							data,
							3);

	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	data[0] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_EXP]       & 0xFF);
	data[1] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_EXP] >> 8) & 0xFF);
	data[2] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_EXP] >> 16) & 0x0F);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x305C,
							data,
							3);

	// Frame length line
	data[0] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_VDOCYC]       & 0xFF);
    data[1] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_VDOCYC] >> 8) & 0xFF);
    data[2] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_VDOCYC] >> 16) & 0x0F);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3030,
							data,
							3);

	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// Short Offset
	data[0] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_OFST]       & 0xFF);
	data[1] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_OFST] >> 8) & 0xFF);
	data[2] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_OFST] >> 16) & 0x0F);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3068,
							data,
							3);

	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// Digital Gain Long and Short
	data[0] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_AGAIN]       & 0xFF);
	data[1] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_AGAIN] >> 8) & 0x07);
	data[2] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_AGAIN]       & 0xFF);
	data[3] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_AGAIN] >> 8) & 0x07);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x30E8,
							data,
							4);

	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// group param hold OFF
	data[0] = 0;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3001,
							data,
							1);

	//MPRINTF((D_MBA_KIND_PREVIEW_DEBUG, FJ_MBALOG_LINE_ID_10, FJ_MBALOG_FUNC_ID_CCD, FJ_MBALOG_TYPE_POINT,
		//"vmax:%d", in->reg[E_CAMERA_SENSOR_REG_VDOCYC]));
	//MPRINTF((D_MBA_KIND_PREVIEW_DEBUG, FJ_MBALOG_LINE_ID_11, FJ_MBALOG_FUNC_ID_CCD, FJ_MBALOG_TYPE_POINT,
		//"rhs1:%d", in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_OFST]));
	//MPRINTF((D_MBA_KIND_PREVIEW_DEBUG, FJ_MBALOG_LINE_ID_12, FJ_MBALOG_FUNC_ID_CCD, FJ_MBALOG_TYPE_POINT,
		//"shr0:%d", in->reg[E_CAMERA_SENSOR_REG_EXP]));
	//MPRINTF((D_MBA_KIND_PREVIEW_DEBUG, FJ_MBALOG_LINE_ID_13, FJ_MBALOG_FUNC_ID_CCD, FJ_MBALOG_TYPE_POINT,
		//"shr1:%d", in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_EXP]));

	return E_CAMERA_ERR_OK;
}

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static E_CAMERA_ERR_CODE access_tbl(E_CAMERA_ID sensor_id, const T_CAMERA_SENSOR_REG* tbl)
{
	INT32	cnt;
	UCHAR	data;
	E_CAMERA_ERR_CODE	ret = E_CAMERA_ERR_OK;

	cnt = 0;
	while(1)
	{
		// end check
		if( D_TBL_END == tbl[cnt].address ){
			break;
		}

		// access
		if( D_TBL_WAIT_MS == tbl[cnt].address ){
			OS_User_Dly_Tsk( tbl[cnt].data[0] );
		}
		else{
			data = tbl[cnt].data[0];
			ret = Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
									CAMERA.sensor[sensor_id].i2c_slave7bit,
									tbl[cnt].address,
									&data,
									1);

			if( ret == E_CAMERA_ERR_NG ){
				break;
			}
		}
		cnt++;
	}

	return ret;
}
