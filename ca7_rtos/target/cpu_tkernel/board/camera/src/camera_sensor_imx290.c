/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "camera_sensor_imx290.h"
#include "camera_config.h"
#include "os_user_custom.h"
#include "im_pro.h"
#include <math.h>

#include "share.h"
#include "dd_top.h"

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

static UINT32 g_backup_exposure_time_value[4];
static UINT32 g_backup_analog_gain_value[4];
static USHORT g_sensor_status = 0;
/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
const T_CAMERA_SENSOR_REG		IMX290_MODE_FULL_1920x1080_RAW12_60FPS[] =
{
//	{ .address = 0x3002, .data[0] = 0x00 }, // XMSTA
	{ .address = 0x3005, .data[0] = 0x01 }, // ADBIT => 12bit
	{ .address = 0x3405, .data[0] = 0x10 }, // REPETITION
	{ .address = 0x3007, .data[0] = 0x00 }, // VREVERSE, HREVERSE, WINMODE
	{ .address = 0x3407, .data[0] = 0x03 }, // PHYSICAL_LANE_NUM
	{ .address = 0x3009, .data[0] = 0x01 }, // FRSEL, FDG_SEL
	{ .address = 0x300A, .data[0] = 0xF0 }, // BLKLEVEL
	{ .address = 0x300F, .data[0] = 0x00 },
	{ .address = 0x3010, .data[0] = 0x21 },
	{ .address = 0x3012, .data[0] = 0x64 },
	{ .address = 0x3414, .data[0] = 0x0A }, // OPB_SIZE_V
	{ .address = 0x3016, .data[0] = 0x09 },
	{ .address = 0x3018, .data[0] = 0x65 }, // VMAX
	{ .address = 0x3019, .data[0] = 0x04 }, // VMAX
	{ .address = 0x3119, .data[0] = 0x9E }, // VMAX
	{ .address = 0x3418, .data[0] = 0x49 }, // Y_OUT_SIZE
	{ .address = 0x3419, .data[0] = 0x04 }, // Y_OUT_SIZE
	{ .address = 0x301C, .data[0] = 0x98 }, // HMAX
	{ .address = 0x301D, .data[0] = 0x08 }, // HMAX
	{ .address = 0x311C, .data[0] = 0x1E }, // HMAX
	{ .address = 0x311E, .data[0] = 0x08 },
	{ .address = 0x3128, .data[0] = 0x05 },
	{ .address = 0x3129, .data[0] = 0x00 }, // ADBIT1 => 12bit
	{ .address = 0x332C, .data[0] = 0xD3 },
	{ .address = 0x332D, .data[0] = 0x10 },
	{ .address = 0x332E, .data[0] = 0x0D },
	{ .address = 0x313D, .data[0] = 0x83 },
	{ .address = 0x3441, .data[0] = 0x0C }, // CSI_DT_FMT
	{ .address = 0x3442, .data[0] = 0x0C }, // CSI_DT_FMT
	{ .address = 0x3443, .data[0] = 0x03 }, // CSI_LANE_MODE => 4 lane
	{ .address = 0x3444, .data[0] = 0x20 }, // EXTCK_FREQ
	{ .address = 0x3445, .data[0] = 0x25 }, // EXTCK_FREQ => 0x2520 = 37.125Mhz
	{ .address = 0x3046, .data[0] = 0x01 }, // ODBIT, OPORTSEL
	{ .address = 0x3446, .data[0] = 0x57 }, // TCLKPOST
	{ .address = 0x3447, .data[0] = 0x00 },
	{ .address = 0x3448, .data[0] = 0x37 }, // THSZERO
	{ .address = 0x3449, .data[0] = 0x00 },
	{ .address = 0x344A, .data[0] = 0x1F },
	{ .address = 0x304B, .data[0] = 0x0A }, // XVSOUTSEL, XHSOUTSEL
	{ .address = 0x344C, .data[0] = 0x1F },
	{ .address = 0x344D, .data[0] = 0x00 },
	{ .address = 0x344E, .data[0] = 0x1F },
	{ .address = 0x344F, .data[0] = 0x00 },
	{ .address = 0x3150, .data[0] = 0x03 },
	{ .address = 0x3450, .data[0] = 0x77 }, // TCLKZERO
	{ .address = 0x3451, .data[0] = 0x00 },
	{ .address = 0x3452, .data[0] = 0x1F }, // TCLKPREPARE
	{ .address = 0x3453, .data[0] = 0x00 },
	{ .address = 0x3454, .data[0] = 0x17 }, // TLPX
	{ .address = 0x3455, .data[0] = 0x00 },
	{ .address = 0x3358, .data[0] = 0x06 },
	{ .address = 0x3359, .data[0] = 0xE1 },
	{ .address = 0x335A, .data[0] = 0x11 },
	{ .address = 0x305C, .data[0] = 0x18 }, // INCKSEL1
	{ .address = 0x305D, .data[0] = 0x03 }, // INCKSEL2
	{ .address = 0x305E, .data[0] = 0x20 }, // INCKSEL3
	{ .address = 0x315E, .data[0] = 0x1A }, // INCKSEL5
	{ .address = 0x305F, .data[0] = 0x01 }, // INCKSEL4
	{ .address = 0x3360, .data[0] = 0x1E },
	{ .address = 0x3361, .data[0] = 0x61 },
	{ .address = 0x3362, .data[0] = 0x10 },
	{ .address = 0x3164, .data[0] = 0x1A }, // INCKSEL6
	{ .address = 0x3070, .data[0] = 0x02 },
	{ .address = 0x3071, .data[0] = 0x11 },
	{ .address = 0x3472, .data[0] = 0x9C }, // X_OUT_SIZE
	{ .address = 0x3473, .data[0] = 0x07 }, // X_OUT_SIZE
	{ .address = 0x317C, .data[0] = 0x00 }, // ADBIT2
	{ .address = 0x317E, .data[0] = 0x00 },
	{ .address = 0x3480, .data[0] = 0x49 }, // INCKSEL7
	{ .address = 0x309B, .data[0] = 0x10 },
	{ .address = 0x309C, .data[0] = 0x22 },
	{ .address = 0x30A2, .data[0] = 0x02 },
	{ .address = 0x30A6, .data[0] = 0x20 },
	{ .address = 0x30A8, .data[0] = 0x20 },
	{ .address = 0x30AA, .data[0] = 0x20 },
	{ .address = 0x30AC, .data[0] = 0x20 },
	{ .address = 0x30B0, .data[0] = 0x43 },
	{ .address = 0x33B0, .data[0] = 0x50 },
	{ .address = 0x33B2, .data[0] = 0x1A },
	{ .address = 0x33B3, .data[0] = 0x04 },
	{ .address = 0x32B8, .data[0] = 0x50 },
	{ .address = 0x32B9, .data[0] = 0x10 },
	{ .address = 0x32BA, .data[0] = 0x00 },
	{ .address = 0x32BB, .data[0] = 0x04 },
	{ .address = 0x32C8, .data[0] = 0x50 },
	{ .address = 0x32C9, .data[0] = 0x10 },
	{ .address = 0x32CA, .data[0] = 0x00 },
	{ .address = 0x32CB, .data[0] = 0x04 },
	{ .address = 0x31EC, .data[0] = 0x0E }, // ADBIT3

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};

const T_CAMERA_SENSOR_BASIC		IMX290_BASIC =
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_37_125MHz,
	.access			= E_CAMERA_ACCESS_I2C,
	.sensor_if		= E_CAMERA_SENSOR_IF_DMIPI,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_VD, //E_CAMERA_SENSOR_UPDATE_TIMING_V_BLANKING,
};

const T_CAMERA_SENSOR_MODE		IMX290_MODE[E_SENSOR_IMX290_MODE_MAX] =
{
	[E_SENSOR_IMX290_MODE_FUll_1920x1080_RAW12_60FPS] = {
		.mode_reg				= IMX290_MODE_FULL_1920x1080_RAW12_60FPS,
		
		.embedded_lines 		= 1,
		.total_pix_width		= 1945,
		.total_pix_lines		= 1109+20,
		.ob_pix_x_pos			= 4,
		.ob_pix_y_pos			= 1+1+1,
		.ob_pix_width			= 8+1920+8,
		.ob_pix_lines			= 10,
		.active_pix_x_pos		= 4, //(4+8)
		.active_pix_y_pos		= (1+1+10), //(1+1+1+10)-4,
		.active_pix_width		= 1920,
		.active_pix_lines		= 1080 + 20,
		.active_pix_x_rsz_ratio = 1.0,
		.active_pix_y_rsz_ratio = 1.0,
		.hdocyc 				= 2200, // line length pck
		.vdocyc 				= 1125, // frame length line
		.exp_start_line_min 	= 1,
		.exp_end_line_offset	= 2,
		.exp_clk_offset 		= 0,
		.input_bit_width		= 12,
		.first_active_pix_color = FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.base_SV				= 4.0,
		.fps					= 60,		// frame rate = pixel clock / frame length line / line length pck = 148.5 * 1000000 / 2200 / 1125
		.Mpps_HDclk 			= 148.5,	// pixel clock = data rate * lane number / input_bit_width = 445.5 * 4 / 12. Refer spec. 50
		.one_lane_Mbps          = 445.5,
		.lane_num				= 4,
	}
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX290_Clock_Gpio_Init(VOID)
{
	INT32 sensor_id;

	UPRINTF("%s(%d) \n", __func__, __LINE__);
	
	// Clock全
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
	case E_CAMERA_SENSOR_INCLK_37_125MHz:
		// 40 / (3+1) * (29*10+7)/10 / (7 + 1) = 37.125
		Dd_Top_Set_PLLCNT3_P00PREDIV(3);	// 40MHz / (val+1)
		Dd_Top_Set_PLLCNT4_P00FNUM(7);		//
		Dd_Top_Set_PLLCNT5_P00FDEN(10);		//
		Dd_Top_Set_PLLCNT3_P00PLLDIV(29);	// * val
		Dd_Top_Set_PLLCNT3_P00POSTDIV(7);	// / (val+1)
		Dd_Top_Start_Pll00();				// PLL00 Start
		break;
	default:
		return E_CAMERA_ERR_NG;
		break;
	}
	
	// Sensor Initializationは平気？？？
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

		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PC0L, 1);
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PC1L, 1);

		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_inck, 1 );	// CLK from Sensor

		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xclr, 0 );	// use as GPIO
		Dd_Top_Set_Gpio_Direction( CAMERA.sensor[sensor_id].port_xclr, 1 );	// output
		Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 0 );	// XCLR to low
		OS_User_Dly_Tsk(1);		// Wait Sensor Stability (Needs Over 100ns)
		Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 1 ); // XCLR low -> high
		OS_User_Dly_Tsk(1); 	// Wait Sensor Stability (Needs Over 100ns)
		
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

E_CAMERA_ERR_CODE Sensor_IMX290_Standby(E_CAMERA_ID sensor_id)
{
	UCHAR data;

	UPRINTF("%s(%d) \n", __func__, __LINE__);

	// Refer spec. p.102
	data = 1;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3000, &data, 1);
	data = 1;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3002, &data, 1);

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX290_CancelStandby(E_CAMERA_ID sensor_id)
{
	UPRINTF("%s(%d) \n", __func__, __LINE__);
	g_sensor_status = 1;
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX290_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol)
{
	UPRINTF("%s(%d) mode = %d \n", __func__, __LINE__, mode);

	g_backup_exposure_time_value[sensor_id] = 0xFFFF;
	g_backup_analog_gain_value[sensor_id] = 0xFFFF;
	// MODE
	access_tbl(sensor_id, IMX290_MODE[mode].mode_reg);
	
	{	// Read VMAX for check
		UCHAR VMAX_data_LSB, VMAX_data_MSB;
		VMAX_data_LSB = VMAX_data_MSB = 0;
		Camera_Common_I2C_Read(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3019, &VMAX_data_MSB, 1);
		Camera_Common_I2C_Read(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3018, &VMAX_data_LSB, 1);
		UPRINTF("%s(%d) Read VMAX = 0x%x after initialization \n", __func__, __LINE__, (VMAX_data_MSB << 8 | VMAX_data_LSB));
	}

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX290_Stream(E_CAMERA_ID sensor_id)
{
	UCHAR					data;

	// STANDBY = 0
	data = 0;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3000, &data, 1);
	OS_User_Dly_Tsk(22);

	// XMSTA = 0
	data = 0;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3002, &data, 1);

	OS_User_Dly_Tsk(8*17); // Refer spec. 77, wait 8 vsync time

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX290_SgVHD(E_CAMERA_ID sensor_id, ULONG vdocyc, ULONG hdocyc)
{
	UPRINTF("%s(%d) \n", __func__, __LINE__);
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX290_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
{
	FLOAT again_dB, dgain_dB;
	USHORT	PGC;

#if 0 // In this case, exposure time can't over 1 frame
	FLOAT row_time;
	INT32 SHS1, MinExposureLines, MaxExposureLines, exposure_lines;
	UINT32 VMAX = IMX290_MODE[mode].dol_vdocyc;

	// Refer spec. p.50 ~ 51
	// row time = HMAX / (data_rate * data_lane / num_of_bits)
	// row time = 2200 / (445.5Mhz * 4 / 12) = 14.8us
	// exposure lines = exposure time / row time
	row_time = IMX290_MODE[mode].hdocyc / IMX290_MODE[mode].Mpps_HDclk; // the unit of row_time is us
	exposure_lines = (UINT32) (in->exp_msec * 1000 / row_time);

	// Refer spec. p.83~84
	// Integration time = 1 frame period - (SHS1 + 1) * row time
	// exposure lines = total lines per frame - (SHS1 + 1)
	// SHS1 = total lines per frame - exposure lines - 1

	MinExposureLines = IMX290_MODE[mode].exp_start_line_min;
	MaxExposureLines = VMAX - IMX290_MODE[mode].exp_end_line_offset;
	SHS1 = VMAX - exposure_lines - 1;
	if (SHS1 > MaxExposureLines)
		SHS1 = MaxExposureLines;
	else if (SHS1 < MinExposureLines)
		SHS1 = MinExposureLines;

#else // Follow IMX477 logic and then caculate SHS1 based on spec
	UINT32 SHS1, MinExposureLines, MaxExposureLines;
	UINT32	CIT;	// Coarse Integration Time
	UINT32	FLL;	// Frame Length Line
	FLOAT	exp_sec;
	FLOAT	default_fps, min_fps, max_fps, req_fps;

	// Coarse Integration Time
	exp_sec = in->exp_msec / 1000.0;
	CIT = (UINT32) ( exp_sec * (IMX290_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / IMX290_MODE[mode].hdocyc - IMX290_MODE[mode].exp_fine);
	if( CIT > ( 0x3FFFF - IMX290_MODE[mode].exp_end_line_offset ) ){ // 0x3FFFF is max of FLL
		CIT = ( 0x3FFFF - IMX290_MODE[mode].exp_end_line_offset );
	}

	if( CIT == 0 ){
		CIT = 1;
	}

	// Frame Length Line
	if( CIT > IMX290_MODE[mode].vdocyc - IMX290_MODE[mode].exp_end_line_offset ){
		FLL = CIT + IMX290_MODE[mode].exp_end_line_offset;
	}
	else{
		FLL = IMX290_MODE[mode].vdocyc;
	}

	// Clip by FPS
	if( in->max_fps == D_CAMERA_FPS_AUTO && in->min_fps == D_CAMERA_FPS_AUTO ){
		// auto, nothing to do
	}else{
		default_fps = (IMX290_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX290_MODE[mode].hdocyc / (FLOAT)IMX290_MODE[mode].vdocyc;
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
		req_fps = (IMX290_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX290_MODE[mode].hdocyc / (FLOAT)FLL;
		if( req_fps < min_fps ){
			FLL = (IMX290_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX290_MODE[mode].hdocyc / min_fps;
		}
		if( req_fps > max_fps ){
			FLL = (IMX290_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX290_MODE[mode].hdocyc / max_fps;
		}
		// clip CIT
		if( CIT > ( FLL - IMX290_MODE[mode].exp_end_line_offset ) ){
			CIT = ( FLL - IMX290_MODE[mode].exp_end_line_offset );
		}
	}

	// Refer spec. p.83~84
	// Integration time = 1 frame period - (SHS1 + 1) * row time
	// exposure lines = total lines per frame - (SHS1 + 1)
	// SHS1 = total lines per frame - exposure lines - 1
	MinExposureLines = IMX290_MODE[mode].exp_start_line_min;
	MaxExposureLines = FLL - IMX290_MODE[mode].exp_end_line_offset;

	SHS1 = FLL - CIT - 1;
	if (SHS1 > MaxExposureLines)
		SHS1 = MaxExposureLines;
	else if (SHS1 < MinExposureLines)
		SHS1 = 1;

//UPRINTF("%s(%d) exp = %f, FLL = %d CIT = %d, SHS1 = %d\n", __func__, __LINE__, in->exp_msec, FLL, CIT, SHS1);

#endif

	// Analog Gain
	// Refer spec. p.80
	// PGC = again_dB * 10 / 3
	again_dB = 20.0 * log10f(in->gain_times);
	PGC = (USHORT) (again_dB * 10 / 3);
	if (PGC > 240) PGC = 240;

	dgain_dB = 0;

	if(PGC > 100){
		again_dB = 30.0;
		dgain_dB = ((FLOAT)PGC - 100) / (10.0 / 3.0);
	}
	else{
		dgain_dB = 0.0;
		again_dB = ((FLOAT)PGC) / (10.0 / 3.0);
	}

	// Revert
	exp_sec = (FLOAT)( CIT + IMX290_MODE[mode].exp_fine ) * (FLOAT)IMX290_MODE[mode].hdocyc / (IMX290_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	out->exp_msec = exp_sec * 1000.0;
	out->fps = (IMX290_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / IMX290_MODE[mode].hdocyc / FLL;
	out->hdocyc = IMX290_MODE[mode].hdocyc;
	out->vdocyc = FLL;
	out->again_times = powf( 10, (again_dB / 20.0) );
	out->dgain_times = powf( 10, (dgain_dB / 20.0) );

	// Reg
	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = IMX290_MODE[mode].hdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = FLL;
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] = SHS1;
	out_reg->reg[E_CAMERA_SENSOR_REG_AGAIN] = PGC;
	out_reg->reg[E_CAMERA_SENSOR_REG_DGAIN] = dgain_dB;


	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX290_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in)
{

	UCHAR byte0, byte1, byte2;

	// Exposure Time: SHS1
	g_backup_exposure_time_value[sensor_id] = in->reg[E_CAMERA_SENSOR_REG_EXP];
	// Analog Gain: PGC
	g_backup_analog_gain_value[sensor_id] = in->reg[E_CAMERA_SENSOR_REG_AGAIN];

	// SHS1
	byte0 = (g_backup_exposure_time_value[sensor_id] & 0x000000FF);
	byte1 = (g_backup_exposure_time_value[sensor_id] & 0x0000FF00) >> 8;
	byte2 = (g_backup_exposure_time_value[sensor_id] & 0x00030000) >> 16;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3020, &(byte0), 1);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3021, &(byte1), 1);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3022, &(byte2), 1);

	// GAIN
	byte0 = g_backup_analog_gain_value[sensor_id];
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3014, &(byte0), 1);

	//UPRINTF("%s(%d) SHS1 = %d, PGC = %d \n", __func__, __LINE__, g_backup_exposure_time_value[sensor_id], g_backup_analog_gain_value[sensor_id]);

	// VMAX
	byte0 = (in->reg[E_CAMERA_SENSOR_REG_VDOCYC] & 0x000000FF);
	byte1 = (in->reg[E_CAMERA_SENSOR_REG_VDOCYC] & 0x0000FF00) >> 8;
	byte2 = (in->reg[E_CAMERA_SENSOR_REG_VDOCYC] & 0x00030000) >> 16;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3018, &(byte0), 1);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3019, &(byte1), 1);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x301A, &(byte2), 1);

	return E_CAMERA_ERR_OK;
}
/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static E_CAMERA_ERR_CODE access_tbl(E_CAMERA_ID sensor_id, const T_CAMERA_SENSOR_REG* tbl)
{
	INT32	cnt = 0;
	UCHAR	data;
	
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
			// Refer spec. p30.
			// address is MSB first
			data = tbl[cnt].data[0];
			Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
									CAMERA.sensor[sensor_id].i2c_slave7bit,
									tbl[cnt].address, &data, 1);
		}
		cnt++;
	}
	
	return E_CAMERA_ERR_OK;
}

