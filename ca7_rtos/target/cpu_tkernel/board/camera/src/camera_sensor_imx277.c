/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "camera_sensor_imx277.h"
#include "camera_config.h"
#include "os_user_custom.h"
#include "im_pro.h"
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
typedef union {
	USHORT	hword;
	UCHAR	byte[2];
} U_REG;

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static E_CAMERA_ERR_CODE access_tbl(E_CAMERA_ID sensor_id, const T_CAMERA_SENSOR_REG* tbl);

static USHORT g_backup_analog_gain_value[4];
/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
const T_CAMERA_SENSOR_REG		IMX277_STANDBY_RELEASE[] = 
{
	// Standby Release

	// 1. 
	{ .address = 0x0000, .data[0] = 0x12 },

	// 1. PLSTMG
	{ .address = 0x0000, .data[0] = 0x1A },
	{ .address = 0x003D, .data[0] = 0x00 },
	{ .address = 0x004E, .data[0] = 0x02 },
	{ .address = 0x0057, .data[0] = 0x4A },
	{ .address = 0x0058, .data[0] = 0xF6 },
	{ .address = 0x0059, .data[0] = 0x00 },
	{ .address = 0x006B, .data[0] = 0x04 },
	{ .address = 0x0202, .data[0] = 0x63 },
	{ .address = 0x0203, .data[0] = 0x00 },
	{ .address = 0x0236, .data[0] = 0x64 },
	{ .address = 0x0237, .data[0] = 0x00 },
	{ .address = 0x0304, .data[0] = 0x0B },
	{ .address = 0x0305, .data[0] = 0x00 },
	{ .address = 0x0306, .data[0] = 0x0B },
	{ .address = 0x0307, .data[0] = 0x00 },
	{ .address = 0x037F, .data[0] = 0x64 },
	{ .address = 0x0380, .data[0] = 0x00 },
	{ .address = 0x038D, .data[0] = 0x64 },
	{ .address = 0x038E, .data[0] = 0x00 },
	{ .address = 0x0399, .data[0] = 0x01 },
	{ .address = 0x0510, .data[0] = 0x72 },
	{ .address = 0x0511, .data[0] = 0x00 },
	{ .address = 0x0528, .data[0] = 0x0F },
	{ .address = 0x0529, .data[0] = 0x0F },
	{ .address = 0x052A, .data[0] = 0x0F },
	{ .address = 0x052B, .data[0] = 0x0F },
	{ .address = 0x0538, .data[0] = 0x0F },
	{ .address = 0x0539, .data[0] = 0x13 },
	{ .address = 0x053C, .data[0] = 0x01 },
	{ .address = 0x0553, .data[0] = 0x00 },
	{ .address = 0x0554, .data[0] = 0x00 },
	{ .address = 0x0555, .data[0] = 0x00 },
	{ .address = 0x0556, .data[0] = 0x00 },
	{ .address = 0x0557, .data[0] = 0x00 },
	{ .address = 0x0558, .data[0] = 0x00 },
	{ .address = 0x0559, .data[0] = 0x00 },
	{ .address = 0x055A, .data[0] = 0x00 },
	{ .address = 0x057D, .data[0] = 0x07 },
	{ .address = 0x057F, .data[0] = 0x07 },
	{ .address = 0x0580, .data[0] = 0x04 },
	{ .address = 0x0583, .data[0] = 0x60 },
	{ .address = 0x0587, .data[0] = 0x01 },
	{ .address = 0x0590, .data[0] = 0x0B },
	{ .address = 0x0591, .data[0] = 0x00 },
	{ .address = 0x05BA, .data[0] = 0x0F },
	{ .address = 0x0606, .data[0] = 0xA3 },
	{ .address = 0x0607, .data[0] = 0x00 },
	{ .address = 0x060E, .data[0] = 0x4E },
	{ .address = 0x060F, .data[0] = 0x00 },
	{ .address = 0x0610, .data[0] = 0x50 },
	{ .address = 0x0611, .data[0] = 0x00 },
	{ .address = 0x0614, .data[0] = 0x20 },
	{ .address = 0x0615, .data[0] = 0x00 },
	{ .address = 0x0618, .data[0] = 0x4B },
	{ .address = 0x0619, .data[0] = 0x00 },
	{ .address = 0x061A, .data[0] = 0x55 },
	{ .address = 0x061B, .data[0] = 0x00 },
	{ .address = 0x061E, .data[0] = 0x90 },
	{ .address = 0x061F, .data[0] = 0x00 },
	{ .address = 0x0620, .data[0] = 0x4D },
	{ .address = 0x0621, .data[0] = 0x00 },
	{ .address = 0x0636, .data[0] = 0x89 },
	{ .address = 0x0637, .data[0] = 0x00 },
	{ .address = 0x0638, .data[0] = 0x8F },
	{ .address = 0x0639, .data[0] = 0x00 },
	{ .address = 0x063A, .data[0] = 0x22 },
	{ .address = 0x063B, .data[0] = 0x00 },
	{ .address = 0x063C, .data[0] = 0x24 },
	{ .address = 0x063D, .data[0] = 0x00 },
	{ .address = 0x063E, .data[0] = 0x4D },
	{ .address = 0x063F, .data[0] = 0x00 },
	{ .address = 0x0640, .data[0] = 0x4F },
	{ .address = 0x0641, .data[0] = 0x00 },
	{ .address = 0x0642, .data[0] = 0x48 },
	{ .address = 0x0643, .data[0] = 0x00 },
	{ .address = 0x0646, .data[0] = 0x87 },
	{ .address = 0x0647, .data[0] = 0x00 },
	{ .address = 0x0648, .data[0] = 0x92 },
	{ .address = 0x0649, .data[0] = 0x00 },
	{ .address = 0x064C, .data[0] = 0x91 },
	{ .address = 0x064D, .data[0] = 0x00 },
	{ .address = 0x064E, .data[0] = 0x22 },
	{ .address = 0x064F, .data[0] = 0x00 },
	{ .address = 0x0650, .data[0] = 0x24 },
	{ .address = 0x0651, .data[0] = 0x00 },
	{ .address = 0x0652, .data[0] = 0x4D },
	{ .address = 0x0653, .data[0] = 0x00 },
	{ .address = 0x0654, .data[0] = 0x4F },
	{ .address = 0x0655, .data[0] = 0x00 },
	{ .address = 0x0656, .data[0] = 0x25 },
	{ .address = 0x0657, .data[0] = 0x00 },
	{ .address = 0x0658, .data[0] = 0x50 },
	{ .address = 0x0659, .data[0] = 0x00 },
	{ .address = 0x066A, .data[0] = 0x0C },
	{ .address = 0x066B, .data[0] = 0x0B },
	{ .address = 0x066C, .data[0] = 0x07 },
	{ .address = 0x066D, .data[0] = 0x00 },
	{ .address = 0x066E, .data[0] = 0x00 },
	{ .address = 0x066F, .data[0] = 0x00 },
	{ .address = 0x0670, .data[0] = 0x00 },
	{ .address = 0x0671, .data[0] = 0x00 },
	{ .address = 0x0672, .data[0] = 0x00 },
	{ .address = 0x0673, .data[0] = 0x00 },
	{ .address = 0x0674, .data[0] = 0xDF },
	{ .address = 0x0675, .data[0] = 0x00 },
	{ .address = 0x0676, .data[0] = 0xA7 },
	{ .address = 0x0677, .data[0] = 0x01 },
	{ .address = 0x0687, .data[0] = 0x00 },
	{ .address = 0x075C, .data[0] = 0x02 },
	{ .address = 0x080A, .data[0] = 0x0A },
	{ .address = 0x082B, .data[0] = 0x16 },

	// 2. wait 10ms
	{ .address = D_TBL_WAIT_MS, .data[0] = 0x0B },
	{ .address = 0x010B, .data[0] = 0x00 },
	{ .address = 0x0127, .data[0] = 0x06 },
	{ .address = 0x0129, .data[0] = 0x0C },


	// 3. wait 0.3ms
	{ .address = D_TBL_WAIT_MS, .data[0] = 0x01 },
	{ .address = 0x0000, .data[0] = 0x08 },
	
	// 4. wait 6.7ms
	{ .address = D_TBL_WAIT_MS, .data[0] = 0x07 },
	{ .address = 0x0001, .data[0] = 0x10 },

	{ .address = 0x0112, .data[0] = 0x01 },
	{ .address = 0x0118, .data[0] = 0x14 },
	{ .address = 0x0119, .data[0] = 0x20 },

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
	
};

const T_CAMERA_SENSOR_REG		IMX277_MODE_H4_V3_NO_1[] = 
{
	{ .address = 0x0003, .data[0] = 0x00 },
	{ .address = 0x0004, .data[0] = 0x00 },
	{ .address = 0x0005, .data[0] = 0x01 },
	{ .address = 0x0006, .data[0] = 0x00 },
	{ .address = 0x0007, .data[0] = 0xA0 },
	{ .address = 0x0008, .data[0] = 0x00 },
	{ .address = 0x000B, .data[0] = 0x08 },
	{ .address = 0x000C, .data[0] = 0x00 },
	{ .address = 0x000D, .data[0] = 0x00 },
	{ .address = 0x000E, .data[0] = 0x00 },
	{ .address = 0x001A, .data[0] = 0x00 },
	{ .address = 0x0036, .data[0] = 0x00 },
	{ .address = 0x0068, .data[0] = 0x00 },
	{ .address = 0x00D5, .data[0] = 0x00 },
	{ .address = 0x00D6, .data[0] = 0x00 },
	{ .address = 0x00D7, .data[0] = 0x00 },
	{ .address = 0x00D8, .data[0] = 0x00 },
	{ .address = 0x00D9, .data[0] = 0x00 },
	{ .address = 0x00DA, .data[0] = 0x00 },
	{ .address = 0x00EE, .data[0] = 0x01 },
	
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};

const T_CAMERA_SENSOR_BASIC		IMX277_BASIC =
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_72MHz,
	.access			= E_CAMERA_ACCESS_SPI,
	.sensor_if		= E_CAMERA_SENSOR_IF_SLVS,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_V_BLANKING,
};

const T_CAMERA_SENSOR_MODE		IMX277_MODE[E_SENSOR_IMX277_MODE_MAX] =
{
	[E_SENSOR_IMX277_MODE_H4_V3_NO_1] = {
		.mode_reg				= IMX277_MODE_H4_V3_NO_1,
		
		.embedded_lines			= 0,
		.total_pix_width		= 4104,
		.total_pix_lines		= 3062,
		.ob_pix_x_pos			= (48+14),
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 4024,
		.ob_pix_lines			= 16,
		.active_pix_x_pos		= (48+14+12),
		.active_pix_y_pos		= (16+6+18),
		.active_pix_width		= 4000,
		.active_pix_lines		= 3000,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.hdocyc					= 380, // 260:87.91fps, 380:60fps
		.vdocyc					= 3150,
		.exp_start_line_min		= 8,
		.exp_end_line_offset	= 4,
		.exp_clk_offset			= 117,
		.input_bit_width		= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.base_SV				= 4.0,
		.fps					= 60.15, // 72.0 / 3150 / 380
		.Mpps_HDclk				= 72.0,
		.lane_num				= 8,
	}
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX277_Clock_Gpio_Init(VOID)
{
	INT32 sensor_id;

	// Clock ?平気？ sensor 全て同じにすべき？
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

	// SP0SCK Pull Up ?SP1は？
	Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PD2, 1 );
	Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PD2, 1 );
	
	// Sensor Initialization ?電源立上げシーケ?スは平気？？？
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
		
		if( CAMERA.sensor[sensor_id].is_spics_gpio ){
			// SP0CS0X is used as GPIO because it is pulled down
			Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PD3, D_DD_TOP_GPIO_FUNC_GPIO);
			Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_PD3, D_DD_TOP_GPIO_STATUS_HIGH);
			Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PD3, D_DD_TOP_GPIO_DIR_OUT);
		}

		/* Setting drivability 8mA (SNAPCLKOO/SNAPCLKO2) */
		Dd_Top_Set_DBCNT1_T11DC( 3 );
		Dd_Top_Set_DBCNT1_T15DC( 3 );

		// XVS XHS
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xhs, 1 );	// XHS from Sensor
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xvs, 1 );	// XVS from Sensor
		
		// init setting
		if( NULL != CAMERA.sensor[sensor_id].Func_Init ){
			CAMERA.sensor[sensor_id].Func_Init(sensor_id);
		}
	}

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX277_Standby(E_CAMERA_ID sensor_id)
{
	CAMERA_PRINT_ERROR( Im_PRO_SG_Stop( sensor_id, 1 ));

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX277_CancelStandby(E_CAMERA_ID sensor_id)
{
	access_tbl(sensor_id, IMX277_STANDBY_RELEASE);
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX277_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol)
{
	U_REG		reg;

	g_backup_analog_gain_value[sensor_id] = 0xFFFF;
	// MODE
	access_tbl(sensor_id, IMX277_MODE[mode].mode_reg);
	// PSLVDS1
	reg.hword = IMX277_MODE[mode].vdocyc - 17;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x032B, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	// PSLVDS2
	reg.hword = IMX277_MODE[mode].vdocyc - 17;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x034B, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	// PSLVDS3
	reg.hword = IMX277_MODE[mode].vdocyc - 17;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x05B6, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	// PSLVDS4
	reg.hword = IMX277_MODE[mode].vdocyc - 22;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x05B8, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX277_SgVHD(E_CAMERA_ID sensor_id, ULONG vdocyc, ULONG hdocyc)
{
	FJ_SG_CTRL	sg_tbl;
	
	sg_tbl.vhdo		= 1;
	sg_tbl.vdot		= 1;
	sg_tbl.vdinv	= 0;
	sg_tbl.hdinv	= 0;
	sg_tbl.hdchg	= 1;
	sg_tbl.vddly	= 0;
	sg_tbl.vdocyc	= vdocyc;
	sg_tbl.vdow		= 1;
	sg_tbl.hdocyc	= hdocyc;
	sg_tbl.hdow		= 4;
	sg_tbl.hdocyc2	= sg_tbl.hdocyc;
	sg_tbl.hdow2	= sg_tbl.hdow;
	CAMERA_PRINT_ERROR( FJ_SetSg( sensor_id, &sg_tbl ) );
	CAMERA_PRINT_ERROR( FJ_SgStart( sensor_id ) ); // restart after current frame
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX277_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
{
	FLOAT	reqFps;
	FLOAT	minFps, maxFps, defaultFps;
	FLOAT	dif_vdocyc = 0.0;
	FLOAT	req_vdocyc;
	FLOAT	req_SHR;
	USHORT	real_vdocyc;
	USHORT	real_SHR;
	FLOAT	realExpTime;
	FLOAT	realFps;
	
	FLOAT	again_dB;
	FLOAT	work;
	USHORT	PGC;
	
	// --------------------
	// Exposur Time
	// --------------------
	// Time = [ { vdocyc * (SVR+1) - SHR } * hdocyc + offset ] / INCK
	// -> (Time * INCK - offset ) / hdocyc = vdocyc - SHR (SVR=0 fixed)
	// -> (Time * INCK - offset ) / hdocyc ... (a)
	// -> vdocyc - SHR                     ... (b)
	
	// req_vdocyc = (a)
	req_vdocyc = ( (in->exp_msec / 1000.0) * (IMX277_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) - (FLOAT)IMX277_MODE[mode].exp_clk_offset) / (FLOAT)IMX277_MODE[mode].hdocyc;
	if( req_vdocyc < 0.0 ){
		req_vdocyc = 0.0;
	}
	
	// (a) = (b)
	// -> req_vdocyc = vdocyc - SHR
	// -> SHR = vdocyc - req_vdocyc
	req_SHR = (FLOAT)IMX277_MODE[mode].vdocyc - req_vdocyc;
	
	// find vdocyc and SHR combination
	if( req_SHR < (FLOAT)IMX277_MODE[mode].exp_start_line_min ){
		// extend vdocyc
		real_SHR = IMX277_MODE[mode].exp_start_line_min;
		real_vdocyc = (USHORT)( req_vdocyc + (FLOAT)IMX277_MODE[mode].exp_start_line_min );
	}
	else if( (IMX277_MODE[mode].vdocyc - IMX277_MODE[mode].exp_end_line_offset) < req_SHR ){
		// clip
		real_SHR = (IMX277_MODE[mode].vdocyc - IMX277_MODE[mode].exp_end_line_offset);
		real_vdocyc = IMX277_MODE[mode].vdocyc;
	}
	else{
		real_SHR = (USHORT)req_SHR;
		real_vdocyc = IMX277_MODE[mode].vdocyc;
	}
	
	// FPS
	if( D_CAMERA_FPS_AUTO == in->min_fps && D_CAMERA_FPS_AUTO == in->max_fps ){
		// nothing to do
	}
	else{
		// default
		defaultFps = (IMX277_MODE[mode].Mpps_HDclk  * 1000.0 * 1000.0) / IMX277_MODE[mode].hdocyc / IMX277_MODE[mode].vdocyc;
		
		// calc min_fps
		if( D_CAMERA_FPS_AUTO != in->min_fps ){
			if( in->min_fps > defaultFps ){
				minFps = defaultFps;
				CAMERA_PRINT_ERROR( TRUE );
			}else{
				minFps = in->min_fps;
			}
		}else{
			minFps = D_CAMERA_FPS_AUTO;
		}
		
		// calc max_fps
		if( D_CAMERA_FPS_AUTO != in->max_fps ){
			if( in->max_fps > defaultFps ){
				maxFps = defaultFps;
				CAMERA_PRINT_ERROR( TRUE );
			}else{
				maxFps = in->max_fps;
			}
		}else{
			maxFps = defaultFps;
		}
		
		// check max_fps and min_fps
		if( maxFps < minFps ){
			minFps = maxFps;
			CAMERA_PRINT_ERROR( TRUE );
		}
		
		// clip real_vdocyc
		reqFps = (IMX277_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX277_MODE[mode].hdocyc / (FLOAT)real_vdocyc;
		if( reqFps < minFps ){
			req_vdocyc   = (IMX277_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX277_MODE[mode].hdocyc / minFps;
			dif_vdocyc   = req_vdocyc - (FLOAT)real_vdocyc;
			real_vdocyc  = (USHORT)req_vdocyc;
		}
		if( reqFps > maxFps ){
			req_vdocyc   = (IMX277_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX277_MODE[mode].hdocyc / maxFps;
			dif_vdocyc   = req_vdocyc - (FLOAT)real_vdocyc;
			real_vdocyc  = (USHORT)req_vdocyc;
		}
		
		// SHR
		if( dif_vdocyc != 0.0 ){
			req_SHR = (FLOAT)real_SHR + dif_vdocyc;
			
			if( req_SHR < (FLOAT)IMX277_MODE[mode].exp_start_line_min ){
				req_SHR = (FLOAT)IMX277_MODE[mode].exp_start_line_min;
			}
			if( real_vdocyc - (FLOAT)IMX277_MODE[mode].exp_end_line_offset < req_SHR ){
				req_SHR = real_vdocyc - (FLOAT)IMX277_MODE[mode].exp_end_line_offset;
			}
			
			real_SHR = (USHORT)req_SHR;
		}
	}
	
	// real
	realExpTime = ( ((FLOAT)real_vdocyc - (FLOAT)real_SHR) * (FLOAT)IMX277_MODE[mode].hdocyc + IMX277_MODE[mode].exp_clk_offset) / (IMX277_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	realFps = (IMX277_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / real_vdocyc / IMX277_MODE[mode].hdocyc;
	
	// Out
	out->fps = realFps;
	out->exp_msec = realExpTime * 1000.0;
	out->hdocyc = IMX277_MODE[mode].hdocyc;
	out->vdocyc = real_vdocyc;
	
	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = out->hdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = out->vdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] = real_SHR;
	
	// --------------------
	// Analog Gain
	// --------------------
	again_dB = 20.0 * log10f(in->gain_times);
	
	work = 2048.0 * ( 1.0 - powf(10, (again_dB/(-20)) ) );
	
	if( work > 1957.0 ){
		PGC = 1957.0;
	}
	else if( work < 0.0 ){
		PGC = 0.0;
	}
	else{
		PGC = (USHORT)work;
	}
	// recalc
	again_dB = -20.0 * log10f( (2048.0 - PGC) / 2048.0 );
	out->again_times = powf( 10, (again_dB / 20.0) );
	out_reg->reg[E_CAMERA_SENSOR_REG_AGAIN] = PGC;
	
	// --------------------
	// Digital Gain
	// --------------------
	out->dgain_times = 1.0;
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX277_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in)
{
	U_REG	reg;

	// SHR
	reg.hword = (USHORT)in->reg[E_CAMERA_SENSOR_REG_EXP];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0b, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);

	// Analog Gain: PGC
	if( g_backup_analog_gain_value[sensor_id] == 0xFFFF ){
		g_backup_analog_gain_value[sensor_id] = in->reg[E_CAMERA_SENSOR_REG_AGAIN];
	}
	reg.hword = (USHORT)g_backup_analog_gain_value[sensor_id];
	g_backup_analog_gain_value[sensor_id] = in->reg[E_CAMERA_SENSOR_REG_AGAIN];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x09, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	
	return E_CAMERA_ERR_OK;
}
/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static E_CAMERA_ERR_CODE access_tbl(E_CAMERA_ID sensor_id, const T_CAMERA_SENSOR_REG* tbl)
{
	INT32	cnt = 0;
	
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
			Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, tbl[cnt].address, &(tbl[cnt].data[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
			
		}
		cnt++;
	}
	
	return E_CAMERA_ERR_OK;
}

