/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "camera_sensor_imx183.h"
#include "camera_config.h"
#include "os_user_custom.h"
#include "im_pro.h"
#include <math.h>
#include "share_data.h"
#include "dd_tmr32.h"
#include "ae_task.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	D_TBL_END		(0xFFFF)
#define	D_TBL_WAIT_MS	(0xFFFE)

#define D_MSHUTTER_P	(E_DD_TOP_GPIO_PG7)
#define D_MSHUTTER_N	(E_DD_TOP_GPIO_PG6)

#define D_MSHUTTER_CLOSE_BY_TIMER
#define D_MSHUTTER_TIMER_ID0_CH		(8)

#define D_MSHUTTER_OFS_US			(15000)
#define D_MSHUTTER_SFT_US			(12000) // Tuned by calibration??
#define D_MSHUTTER_DELAY_US			(D_MSHUTTER_OFS_US - D_MSHUTTER_SFT_US)

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
static VOID mshutter_close_timer_handler0( VOID );
static void mshutter_close_by_timer_start(E_CAMERA_ID sensor_id, ULONG usec);

static USHORT g_backup_exposure_time_value[4];
static USHORT g_backup_analog_gain_value[4];
static USHORT g_sensor_status = 0;
static UCHAR g_sensor_shut_stat = 0xFF; // 0xX0: G OFF, 0xX1: G ON, 0x0X: M OPEN, 0x1X: M CLOSE, others: init stat
static ULONG g_sensor_cur_readout_us = 66666;
/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

const T_CAMERA_SENSOR_SYNC_CODE	IMX183_SYNC_CODE[E_SENSOR_IMX183_MODE_MAX] = {
#ifdef D_LANE_10_BOARD
	[E_SENSOR_IMX183_MODE_TYPE_1_NO_1_1].sav = { 0x03FF, 0x0000, 0x0000, 0x0200 },
	[E_SENSOR_IMX183_MODE_TYPE_1_NO_1_1].eav = { 0x03FF, 0x0000, 0x0000, 0x0274 },
	[E_SENSOR_IMX183_MODE_TYPE_1_NO_1_1].sab = { 0x03FF, 0x0000, 0x0000, 0x02AC },
	[E_SENSOR_IMX183_MODE_TYPE_1_NO_1_1].eab = { 0x03FF, 0x0000, 0x0000, 0x02D8 },
#else
	[E_SENSOR_IMX183_MODE_TYPE_1P4_NO_1].sav = { 0x03FF, 0x0000, 0x0000, 0x0200 },
	[E_SENSOR_IMX183_MODE_TYPE_1P4_NO_1].eav = { 0x03FF, 0x0000, 0x0000, 0x0274 },
	[E_SENSOR_IMX183_MODE_TYPE_1P4_NO_1].sab = { 0x03FF, 0x0000, 0x0000, 0x02AC },
	[E_SENSOR_IMX183_MODE_TYPE_1P4_NO_1].eab = { 0x03FF, 0x0000, 0x0000, 0x02D8 },
#endif
	
	[E_SENSOR_IMX183_MODE_TYPE_1_NO_1].sav = { 0x03FF, 0x0000, 0x0000, 0x0200 },
	[E_SENSOR_IMX183_MODE_TYPE_1_NO_1].eav = { 0x03FF, 0x0000, 0x0000, 0x0274 },
	[E_SENSOR_IMX183_MODE_TYPE_1_NO_1].sab = { 0x03FF, 0x0000, 0x0000, 0x02AC },
	[E_SENSOR_IMX183_MODE_TYPE_1_NO_1].eab = { 0x03FF, 0x0000, 0x0000, 0x02D8 },
};

const T_CAMERA_SENSOR_SYNC_MASK	IMX183_MASK = {
	.msav = { 0x0000, 0x0000, 0x0000, 0x0000 },
	.meav = { 0x0000, 0x0000, 0x0000, 0x0000 },
	.msab = { 0x0000, 0x0000, 0x0000, 0x0000 },
	.meab = { 0x0000, 0x0000, 0x0000, 0x0000 },
};

const E_CAMERA_LVDS_SNCLK_SEL	IMX183_CLOCK_SELECT[8] = {
	[0] = D_SENSOR_CLK0_SEL,
	[1] = D_SENSOR_CLK0_SEL,
	[2] = D_SENSOR_CLK1_SEL,
	[3] = D_SENSOR_CLK1_SEL,
	[4] = D_SENSOR_CLK2_SEL,
	[5] = D_SENSOR_CLK2_SEL,
	[6] = 0,
	[7] = 0,
};

const E_CAMERA_LVDS_SNDATA_SEL 	IMX183_DATA_SELECT[16] = {
	[0] = D_SENSOR_DATA0_SEL,
	[1] = D_SENSOR_DATA1_SEL,
	[2] = D_SENSOR_DATA2_SEL,
	[3] = D_SENSOR_DATA3_SEL,
	[4] = D_SENSOR_DATA4_SEL,
	[5] = D_SENSOR_DATA5_SEL,
	[6] = D_SENSOR_DATA6_SEL,
	[7] = D_SENSOR_DATA7_SEL,
	[8] = D_SENSOR_DATA8_SEL,
	[9] = D_SENSOR_DATA9_SEL,
	[10] = D_SENSOR_DATA10_SEL,
	[11] = D_SENSOR_DATA11_SEL,
	[12] = 0,
	[13] = 0,
	[14] = 0,
	[15] = 0,
};

const T_CAMERA_SENSOR_REG		IMX183_STANDBY_RELEASE[] = 
{
	// Standby Release

	// 1. 
	{ .address = 0x0000, .data[0] = 0x02 },
	{ .address = 0x0045, .data[0] = 0x40 },
	{ .address = 0x0058, .data[0] = 0x37 },
	{ .address = 0x005A, .data[0] = 0x2B },
	{ .address = 0x0312, .data[0] = 0x11 },
	{ .address = 0x0355, .data[0] = 0x00 },
	{ .address = 0x0381, .data[0] = 0x00 },
	{ .address = 0x052E, .data[0] = 0x02 },
	{ .address = 0x0530, .data[0] = 0x0B },
	{ .address = 0x0531, .data[0] = 0x0B },
	{ .address = 0x0532, .data[0] = 0x0B },
	{ .address = 0x0533, .data[0] = 0x0B },
	{ .address = 0x0534, .data[0] = 0x0B },
	{ .address = 0x0535, .data[0] = 0x0B },
	{ .address = 0x053F, .data[0] = 0x1D },
	{ .address = 0x0541, .data[0] = 0x1D },
	{ .address = 0x0545, .data[0] = 0x00 },
	{ .address = 0x0549, .data[0] = 0x02 },
	{ .address = 0x054B, .data[0] = 0x00 },
	{ .address = 0x0555, .data[0] = 0x02 },
	{ .address = 0x0563, .data[0] = 0x05 },
	{ .address = 0x05A4, .data[0] = 0x00 },
	{ .address = 0x05A5, .data[0] = 0x07 },
	{ .address = 0x05AA, .data[0] = 0x00 },
	{ .address = 0x05D1, .data[0] = 0x16 },
	{ .address = 0x05D2, .data[0] = 0x15 },
	{ .address = 0x05D3, .data[0] = 0x14 },
	{ .address = 0x065C, .data[0] = 0x01 },
	{ .address = 0x065E, .data[0] = 0x01 },
	
	//Wait>1ms	
	{ .address = D_TBL_WAIT_MS, .data[0] = 0x01 },
	
	{ .address = 0x0000, .data[0] = 0x00 },
			
	//Wait>16ms  
	{ .address = D_TBL_WAIT_MS, .data[0] = 0x10 },
	
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
	
};

#ifdef D_LANE_10_BOARD
const T_CAMERA_SENSOR_REG		IMX183_MODE_TYPE_1_NO_1_1[] = 
{
	{ .address = 0x0001, .data[0] = 0x11 },
	{ .address = 0x0003, .data[0] = 0x00 },
	{ .address = 0x0004, .data[0] = 0x00 },
	{ .address = 0x0005, .data[0] = 0x01 },
	{ .address = 0x0006, .data[0] = 0x20 },
	{ .address = 0x0007, .data[0] = 0x50 },
	{ .address = 0x0008, .data[0] = 0x00 },
	{ .address = 0x0009, .data[0] = 0x00 }, // PGC Low
	{ .address = 0x000A, .data[0] = 0x00 }, // PGC High
	{ .address = 0x000B, .data[0] = 0x10 }, // SHR Low
	{ .address = 0x000C, .data[0] = 0x00 }, // SHR High
	{ .address = 0x000D, .data[0] = 0x00 }, // SVR Low
	{ .address = 0x000E, .data[0] = 0x00 }, // SVR High
	{ .address = 0x000F, .data[0] = 0x00 }, // SPL Low
	{ .address = 0x0010, .data[0] = 0x00 }, // SPL High
	{ .address = 0x0011, .data[0] = 0x00 }, // DGAIN
	{ .address = 0x001A, .data[0] = 0x00 },
	{ .address = 0x006F, .data[0] = 0x1C },
	{ .address = 0x0070, .data[0] = 0x01 },
	{ .address = 0x0071, .data[0] = 0x1D },
	{ .address = 0x0072, .data[0] = 0x01 },
		
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

	};
#else
const T_CAMERA_SENSOR_REG		IMX183_MODE_TYPE_1P4_NO_1[] = 
{
	{ .address = 0x0001, .data[0] = 0x11 },
	{ .address = 0x0003, .data[0] = 0x11 },
	{ .address = 0x0004, .data[0] = 0x54 },
	{ .address = 0x0005, .data[0] = 0x41 },
	{ .address = 0x0006, .data[0] = 0x00 },
	{ .address = 0x0007, .data[0] = 0x00 },
	{ .address = 0x0008, .data[0] = 0x00 },
	{ .address = 0x0009, .data[0] = 0x00 }, // PGC Low
	{ .address = 0x000A, .data[0] = 0x00 }, // PGC High
	{ .address = 0x000B, .data[0] = 0x10 }, // SHR Low
	{ .address = 0x000C, .data[0] = 0x00 }, // SHR High
	{ .address = 0x000D, .data[0] = 0x00 }, // SVR Low
	{ .address = 0x000E, .data[0] = 0x00 }, // SVR High
	{ .address = 0x000F, .data[0] = 0x00 }, // SPL Low
	{ .address = 0x0010, .data[0] = 0x00 }, // SPL High
	{ .address = 0x0011, .data[0] = 0x00 }, // DGAIN
	{ .address = 0x001A, .data[0] = 0x00 },
	
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};
#endif

const T_CAMERA_SENSOR_REG		IMX183_MODE_TYPE_1_NO_1[] = 
{
	{ .address = 0x0001, .data[0] = 0x11 },
#ifdef D_LANE_10_BOARD
	{ .address = 0x0003, .data[0] = 0x00 },
#else
	{ .address = 0x0003, .data[0] = 0x11 },
#endif
	{ .address = 0x0004, .data[0] = 0x00 },
	{ .address = 0x0005, .data[0] = 0x01 },
	{ .address = 0x0006, .data[0] = 0x00 },
	{ .address = 0x0007, .data[0] = 0x00 },
	{ .address = 0x0008, .data[0] = 0x00 },
	{ .address = 0x0009, .data[0] = 0x00 }, // PGC Low
	{ .address = 0x000A, .data[0] = 0x00 }, // PGC High
	{ .address = 0x000B, .data[0] = 0x10 }, // SHR Low
	{ .address = 0x000C, .data[0] = 0x00 }, // SHR High
	{ .address = 0x000D, .data[0] = 0x00 }, // SVR Low
	{ .address = 0x000E, .data[0] = 0x00 }, // SVR High
	{ .address = 0x000F, .data[0] = 0x00 }, // SPL Low
	{ .address = 0x0010, .data[0] = 0x00 }, // SPL High
	{ .address = 0x0011, .data[0] = 0x00 }, // DGAIN
	{ .address = 0x001A, .data[0] = 0x00 },
	
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};

const T_CAMERA_SENSOR_BASIC		IMX183_BASIC =
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_72MHz,
	.access			= E_CAMERA_ACCESS_SPI,
	.sensor_if		= E_CAMERA_SENSOR_IF_LVDS,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_VD,
};

const T_CAMERA_SENSOR_MODE		IMX183_MODE[E_SENSOR_IMX183_MODE_MAX] =
{
#ifdef D_LANE_10_BOARD
	[E_SENSOR_IMX183_MODE_TYPE_1_NO_1_1] = {
		.mode_reg				= IMX183_MODE_TYPE_1_NO_1_1,
		
		.embedded_lines			= 0,
		.total_pix_width		= 5640,
		.total_pix_lines		= 3124,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 74+24,
		.active_pix_y_pos		= 16+12,
		.active_pix_width		= 5496,
		.active_pix_lines		= 2508,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.hdocyc					= 728,
		.vdocyc					= 3296,
		.exp_start_line_min		= 8,
		.exp_end_line_offset	= 4,
		.exp_clk_offset			= 157,
		.input_bit_width		= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.base_SV				= 4.0,
		.fps					= 30.00,
		.Mpps_HDclk				= 72.0,
		.lane_num				= 10,
		.sync_code				= &IMX183_SYNC_CODE[E_SENSOR_IMX183_MODE_TYPE_1_NO_1_1],
		.sync_mask				= &IMX183_MASK,
		.clock_select			= IMX183_CLOCK_SELECT,
		.data_select			= IMX183_DATA_SELECT,
	},
#else
	[E_SENSOR_IMX183_MODE_TYPE_1P4_NO_1] = {
		.mode_reg				= IMX183_MODE_TYPE_1P4_NO_1,
		
		.embedded_lines			= 0,
		.total_pix_width		= 4128,
		.total_pix_lines		= 2180,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 82,
		.active_pix_y_pos		= 12,
		.active_pix_width		= 4128,
		.active_pix_lines		= 2180,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.hdocyc					= 700,
		.vdocyc					= 3428,
		.exp_start_line_min		= 8,
		.exp_end_line_offset	= 4,
		.exp_clk_offset			= 157,
		.input_bit_width		= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.base_SV				= 4.0,
		.fps					= 30.00,
		.Mpps_HDclk				= 72.0,
		.lane_num				= 8,
		.sync_code				= &IMX183_SYNC_CODE[E_SENSOR_IMX183_MODE_TYPE_1P4_NO_1],
		.sync_mask				= &IMX183_MASK,
		.clock_select			= IMX183_CLOCK_SELECT,
		.data_select			= IMX183_DATA_SELECT,
	},
#endif

	[E_SENSOR_IMX183_MODE_TYPE_1_NO_1] = {
		.mode_reg				= IMX183_MODE_TYPE_1_NO_1,
		
		.embedded_lines			= 0,
		.total_pix_width		= 5640,
		.total_pix_lines		= 3710,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 74+24,
		.active_pix_y_pos		= 16+12,
		.active_pix_width		= 5496, // effective capture width: 5472
		.active_pix_lines		= 3672, // effective capture lines: 3648
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.hdocyc					= 900,
		.vdocyc					= 5333,
		.exp_start_line_min		= 0,
		.exp_end_line_offset	= 120,
		.exp_clk_offset			= 157,
		.input_bit_width		= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.base_SV				= 4.0,
		.fps					= 15.00,
		.Mpps_HDclk				= 72.0,
#ifdef D_LANE_10_BOARD
		.lane_num				= 10,
#else
		.lane_num				= 8,
#endif
		.sync_code				= &IMX183_SYNC_CODE[E_SENSOR_IMX183_MODE_TYPE_1_NO_1],
		.sync_mask				= &IMX183_MASK,
		.clock_select			= IMX183_CLOCK_SELECT,
		.data_select			= IMX183_DATA_SELECT,
	}
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define M_SHUTTER_INIT()\
	Dd_Top_Set_Gpio_Function(D_MSHUTTER_P, D_DD_TOP_GPIO_FUNC_GPIO);\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_P, D_DD_TOP_GPIO_STATUS_HIGH);\
	Dd_Top_Set_Gpio_Direction(D_MSHUTTER_P, D_DD_TOP_GPIO_DIR_OUT);\
	Dd_Top_Set_Gpio_Function(D_MSHUTTER_N, D_DD_TOP_GPIO_FUNC_GPIO);\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_N, D_DD_TOP_GPIO_STATUS_HIGH);\
	Dd_Top_Set_Gpio_Direction(D_MSHUTTER_N, D_DD_TOP_GPIO_DIR_OUT);\

#define M_SHUTTER_OPEN()\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_P, D_DD_TOP_GPIO_STATUS_HIGH);\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_N, D_DD_TOP_GPIO_STATUS_LOW);\
	__asm__ volatile ("nop");\
	__asm__ volatile ("nop");\
	__asm__ volatile ("nop");\
	__asm__ volatile ("nop");\
	__asm__ volatile ("nop");\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_P, D_DD_TOP_GPIO_STATUS_HIGH);\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_N, D_DD_TOP_GPIO_STATUS_HIGH);\
	
#define M_SHUTTER_CLOSE()\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_P, D_DD_TOP_GPIO_STATUS_LOW);\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_N, D_DD_TOP_GPIO_STATUS_HIGH);\
	__asm__ volatile ("nop");\
	__asm__ volatile ("nop");\
	__asm__ volatile ("nop");\
	__asm__ volatile ("nop");\
	__asm__ volatile ("nop");\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_P, D_DD_TOP_GPIO_STATUS_HIGH);\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_N, D_DD_TOP_GPIO_STATUS_HIGH);\

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX183_Clock_Gpio_Init(VOID)
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
		Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 0 );	// XCLR
		OS_User_Dly_Tsk(1);		// Wait Sensor Stability (Needs Over 100ns)
		
		if( CAMERA.sensor[sensor_id].is_spics_gpio ){
			// SP0CS0X is used as GPIO because it is pulled down
			Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PD3, D_DD_TOP_GPIO_FUNC_GPIO);
			Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_PD3, D_DD_TOP_GPIO_STATUS_HIGH);
			Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PD3, D_DD_TOP_GPIO_DIR_OUT);
		}

		// XVS XHS
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xhs, 1 );	// XHS from Sensor
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xvs, 1 );	// XVS from Sensor
		
		// init setting
		if( NULL != CAMERA.sensor[sensor_id].Func_Init ){
			CAMERA.sensor[sensor_id].Func_Init(sensor_id);
		}
	}
	M_SHUTTER_INIT();

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX183_Standby(E_CAMERA_ID sensor_id)
{
	U_REG		reg;
	
	g_sensor_status = 0;
	
	reg.byte[0] = 0x03; // default
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0000, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);

	CAMERA_PRINT_ERROR( Im_PRO_SG_Stop( sensor_id, 1 ));

	Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 0 ); // XCLR
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX183_CancelStandby(E_CAMERA_ID sensor_id)
{
	g_sensor_status = 1;
	
	Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 1 ); // XCLR
	OS_User_Dly_Tsk(1); 	// Wait Sensor Stability (Needs Over 100ns)
	
	access_tbl(sensor_id, IMX183_STANDBY_RELEASE);
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX183_GlobalShutter(E_CAMERA_ID sensor_id, E_CAMERA_SENSOR_SHUTTER shutter_mode)
{
	ULONG 	mshut_close_us = D_MSHUTTER_DELAY_US;
	U_REG	reg;
	
	if(((g_sensor_shut_stat & 0x11) == 0x00) && (shutter_mode == E_CAMERA_SENSOR_SHUTTER_G_ON_M_CLOSE)){
		return E_CAMERA_ERR_OK;
	}
	
	if(((g_sensor_shut_stat & 0x11) == 0x11) && (shutter_mode == E_CAMERA_SENSOR_SHUTTER_G_ON_M_CLOSE)){
		return E_CAMERA_ERR_OK;
	}
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SENSOR, FJ_MBALOG_FUNC_ID_SHUTTER, FJ_MBALOG_TYPE_POINT, "GlobalShutter(%d)", shutter_mode));

	switch(shutter_mode)
	{
		case E_CAMERA_SENSOR_SHUTTER_G_OFF_M_OPEN:
			if((g_sensor_shut_stat & 0x0F) != 0x00)
			{
				reg.byte[0] = 0x00; // SMD = 0
				Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0008, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);

				g_sensor_shut_stat = g_sensor_shut_stat & 0xF0;
			}
			if((g_sensor_shut_stat & 0xF0) != 0x00)
			{
				// M shutter Open
				M_SHUTTER_OPEN();

				g_sensor_shut_stat = g_sensor_shut_stat & 0x0F;
			}
		break;
		case E_CAMERA_SENSOR_SHUTTER_G_OFF_M_CLOSE:
			if((g_sensor_shut_stat & 0x0F) != 0x00)
			{
				reg.byte[0] = 0x00; // SMD = 0
				Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0008, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);

				g_sensor_shut_stat = g_sensor_shut_stat & 0xF0;
			}
			if((g_sensor_shut_stat & 0xF0) != 0x10)
			{
				// M shutter Close
				M_SHUTTER_CLOSE();

				g_sensor_shut_stat = (g_sensor_shut_stat & 0x0F) | 0x10;
			}
		break;
		case E_CAMERA_SENSOR_SHUTTER_G_ON_M_OPEN:

			if(((g_sensor_shut_stat & 0x11) == 0x00) || (g_sensor_shut_stat == 0xFF)){
				mshut_close_us = g_sensor_cur_readout_us * (2 + CAMERA.start_frame_num) - D_MSHUTTER_DELAY_US; // exp + Read
			}else if((g_sensor_shut_stat & 0x11) == 0x11){
				mshut_close_us = g_sensor_cur_readout_us - D_MSHUTTER_DELAY_US;
			}
			mshutter_close_by_timer_start(sensor_id, mshut_close_us);
			
			if((g_sensor_shut_stat & 0x0F) != 0x01)
			{
				reg.byte[0] = 0x01; // SMD = 1
				Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0008, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);

				g_sensor_shut_stat = (g_sensor_shut_stat & 0xF0) | 0x01;
			}
			if((g_sensor_shut_stat & 0xF0) != 0x00)
			{
				// M shutter Open
				M_SHUTTER_OPEN();

				g_sensor_shut_stat = g_sensor_shut_stat & 0x0F;
			}
		break;
		case E_CAMERA_SENSOR_SHUTTER_G_ON_M_CLOSE:
			if((g_sensor_shut_stat & 0x0F) != 0x01)
			{
				reg.byte[0] = 0x01; // SMD = 1
				Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0008, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);

				g_sensor_shut_stat = (g_sensor_shut_stat & 0xF0) | 0x01;
			}
			if((g_sensor_shut_stat & 0xF0) != 0x10)
			{
				// M shutter Close
				M_SHUTTER_CLOSE();

				g_sensor_shut_stat = (g_sensor_shut_stat & 0x0F) | 0x10;
			}
		break;
		default:
		break;
	}
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX183_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol)
{

	g_backup_exposure_time_value[sensor_id] = 0xFFFF;
	g_backup_analog_gain_value[sensor_id] = 0xFFFF;
	// MODE
	access_tbl(sensor_id, IMX183_MODE[mode].mode_reg);
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX183_SgVHD(E_CAMERA_ID sensor_id, ULONG vdocyc, ULONG hdocyc)
{
	E_IM_PRO_SG_CH sg_ch = E_IM_PRO_SG_CH_0;
	FJ_SG_CTRL	sg_tbl;
	static ULONG vdocyc_bak = 0; 
	static ULONG hdocyc_bak = 0; 

	if(g_sensor_status == 0)
	{
		vdocyc_bak = 0; 
		hdocyc_bak = 0; 
		return E_CAMERA_ERR_OK;
	}

	if((vdocyc_bak == vdocyc) && (hdocyc_bak == hdocyc))
	{
		return E_CAMERA_ERR_OK;
	}
	vdocyc_bak = vdocyc; 
	hdocyc_bak = hdocyc; 
	
	sg_tbl.vhdo		= 1;
	sg_tbl.vdot		= 0;
	sg_tbl.vdinv	= 0;
	sg_tbl.hdinv	= 0;
	sg_tbl.hdchg	= 0;
	sg_tbl.vddly	= 0;
	sg_tbl.vdocyc	= vdocyc;
	sg_tbl.vdow		= 1;
	sg_tbl.hdocyc	= hdocyc;
	sg_tbl.hdow		= 14;
	sg_tbl.hdocyc2	= sg_tbl.hdocyc;
	sg_tbl.hdow2	= sg_tbl.hdow;

	switch(CAMERA.sensor[sensor_id].port_inck)
	{
		case E_DD_TOP_GPIO_PG2:
			sg_ch = E_IM_PRO_SG_CH_0;
		break;
		case E_DD_TOP_GPIO_PG5:
			sg_ch = E_IM_PRO_SG_CH_1;
		break;
		case E_DD_TOP_GPIO_PH0:
			sg_ch = E_IM_PRO_SG_CH_2;
		break;
		case E_DD_TOP_GPIO_PH3:
			sg_ch = E_IM_PRO_SG_CH_3;
		break;
		default:
			sg_ch = E_IM_PRO_SG_CH_0;
		break;
	}
	
	CAMERA_PRINT_ERROR( FJ_SetSg( sg_ch, &sg_tbl ) );
	CAMERA_PRINT_ERROR( FJ_SgStart( sg_ch ) ); // restart after current frame
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX183_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
{
	FLOAT	reqFps;
	FLOAT	minFps, maxFps, defaultFps;
	FLOAT	req_vdocyc;
	FLOAT	req_SHR;
	USHORT	real_vdocyc;
	USHORT	real_SHR;
	FLOAT	realExpTime;
	FLOAT	realFps;
	
	FLOAT	again_dB, dgain_dB;
	FLOAT	work;
	USHORT	PGC;

	FLOAT req_exp_msec = 16.666;
	FLOAT req_gain_times = 1.0;
	DOUBLE	realExpClk, _realExpTime;

	// --------------------
	// Exposur Time
	// --------------------
	// Time = [ { vdocyc * (SVR+1) - SHR } * hdocyc + offset ] / INCK
	// -> (Time * INCK - offset ) / hdocyc = vdocyc - SHR (SVR=0 fixed)
	// -> (Time * INCK - offset ) / hdocyc ... (a)
	// -> vdocyc - SHR         ... (b)

	req_exp_msec = in->exp_msec;
	req_gain_times = in->gain_times;
	
	// req_vdocyc = (a)
	req_vdocyc = ( (req_exp_msec / 1000.0) * (IMX183_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) - (FLOAT)IMX183_MODE[mode].exp_clk_offset) / (FLOAT)IMX183_MODE[mode].hdocyc;
	if( req_vdocyc < 0.0 ){
		req_vdocyc = 0.0;
	}
	
	// (a) = (b)
	// -> req_vdocyc = vdocyc - SHR
	// -> SHR = vdocyc - req_vdocyc
	req_SHR = (FLOAT)IMX183_MODE[mode].vdocyc - req_vdocyc;

	// find vdocyc and SHR combination
	if( req_SHR < ((FLOAT)IMX183_MODE[mode].exp_start_line_min ) ){
		// extend vdocyc
		real_SHR = IMX183_MODE[mode].exp_start_line_min;
		real_vdocyc = (USHORT)( req_vdocyc + (FLOAT)IMX183_MODE[mode].exp_start_line_min );
	}
	else if( (IMX183_MODE[mode].vdocyc - IMX183_MODE[mode].exp_end_line_offset) < req_SHR ){
		// clip
		real_SHR = IMX183_MODE[mode].vdocyc - IMX183_MODE[mode].exp_end_line_offset;
		real_vdocyc = IMX183_MODE[mode].vdocyc;
	}
	else{
		real_SHR = (USHORT)req_SHR;
		real_vdocyc = IMX183_MODE[mode].vdocyc;
	}

	// FPS
	if( D_CAMERA_FPS_AUTO == in->min_fps && D_CAMERA_FPS_AUTO == in->max_fps ){
		// nothing to do
	}
	else{
		// default
		defaultFps = (IMX183_MODE[mode].Mpps_HDclk  * 1000.0 * 1000.0) / IMX183_MODE[mode].hdocyc / IMX183_MODE[mode].vdocyc;
		
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
		reqFps = (IMX183_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX183_MODE[mode].hdocyc / (FLOAT)real_vdocyc;
		if( reqFps < minFps ){
			req_vdocyc   = (IMX183_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX183_MODE[mode].hdocyc / minFps;
			real_vdocyc  = (USHORT)req_vdocyc;
		}
		if( reqFps > maxFps ){
			req_vdocyc   = (IMX183_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX183_MODE[mode].hdocyc / maxFps;
			real_vdocyc  = (USHORT)req_vdocyc;
		}
		
		if (real_SHR > (real_vdocyc - IMX183_MODE[mode].exp_end_line_offset)) {
            req_SHR     = real_vdocyc - IMX183_MODE[mode].exp_end_line_offset;
            realExpClk  = (DOUBLE)(real_vdocyc - req_SHR) * IMX183_MODE[mode].hdocyc;
			_realExpTime = realExpClk / (DOUBLE)(IMX183_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
 			_realExpTime = (DOUBLE)expTimeQuantization(_realExpTime * 1000, in->flicker_mode, QUANTIZ_ROUND_OFF);
			req_SHR	    = (ULONG)( (_realExpTime / 1000.0) * (DOUBLE)(IMX183_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (DOUBLE)IMX183_MODE[mode].hdocyc);
            real_SHR    = real_vdocyc - req_SHR;

            if (real_SHR < (FLOAT)IMX183_MODE[mode].exp_start_line_min)
                real_SHR = IMX183_MODE[mode].exp_start_line_min;

            if (real_SHR > (real_vdocyc - IMX183_MODE[mode].exp_end_line_offset))
                real_SHR = real_vdocyc - IMX183_MODE[mode].exp_end_line_offset;
        }
	}

	// real
	realExpTime = ( ((FLOAT)real_vdocyc - (FLOAT)real_SHR) * (FLOAT)IMX183_MODE[mode].hdocyc + IMX183_MODE[mode].exp_clk_offset) / (IMX183_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	realFps = (IMX183_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / real_vdocyc / IMX183_MODE[mode].hdocyc;
	g_sensor_cur_readout_us = (ULONG)((FLOAT)1000000.0 / realFps);

	// Out
	out->fps = realFps;
	out->exp_msec = realExpTime * 1000.0;
	out->hdocyc = IMX183_MODE[mode].hdocyc;
	out->vdocyc = real_vdocyc;
	
	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = out->hdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = out->vdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] = real_SHR;
	
	// --------------------
	// Analog Gain
	// --------------------
	again_dB = 20.0 * log10f(req_gain_times);
	
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
	dgain_dB = 0; // fix to 1.0 times (temporary)

	out->dgain_times = powf( 10, (dgain_dB / 20.0) );
	out_reg->reg[E_CAMERA_SENSOR_REG_DGAIN] = dgain_dB / 6;
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX183_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in)
{
	U_REG	reg;

	// Exposure Time: SHR
	g_backup_exposure_time_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_EXP];
	// Analog Gain: PGC
	g_backup_analog_gain_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_AGAIN];

	// SHR
	reg.hword = (USHORT)g_backup_exposure_time_value[sensor_id];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x000B, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	g_backup_exposure_time_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_EXP];

	// AGAIN
	reg.hword = (USHORT)g_backup_analog_gain_value[sensor_id];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0009, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	g_backup_analog_gain_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_AGAIN];

	// DGAIN
	reg.byte[0] = (UCHAR)in->reg[E_CAMERA_SENSOR_REG_DGAIN];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0011, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	
	if((g_sensor_shut_stat & 0x01) == 0x01){
		reg.byte[0] = 0x01; // CLPMDCHG = 1
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0014, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
		reg.byte[0] = 0x01; // CLPMODE = 1
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0015, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
		reg.byte[0] = 0x10; // CLPSQRST = 1
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0001, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	}
	
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

#ifdef D_MSHUTTER_CLOSE_BY_TIMER
static VOID mshutter_close_timer_handler0( VOID )
{
	Sensor_IMX183_GlobalShutter(E_CAMERA_ID_0, E_CAMERA_SENSOR_SHUTTER_G_ON_M_CLOSE);

	Dd_TMR32_Stop(D_MSHUTTER_TIMER_ID0_CH);
	Dd_TMR32_Close(D_MSHUTTER_TIMER_ID0_CH);
}
#endif

static void mshutter_close_by_timer_start(E_CAMERA_ID sensor_id, ULONG usec)
{

#ifdef D_MSHUTTER_CLOSE_BY_TIMER
	INT32		ret;
	UINT32 		ch;
	VP_CALLBACK pcallback;

	switch(sensor_id)
	{
		case E_CAMERA_ID_0:
			ch = D_MSHUTTER_TIMER_ID0_CH;
			pcallback = (VP_CALLBACK)mshutter_close_timer_handler0;
		break;
		case E_CAMERA_ID_1:
		case E_CAMERA_ID_2:
		case E_CAMERA_ID_3:
		default:
			ch = D_MSHUTTER_TIMER_ID0_CH;
			pcallback = (VP_CALLBACK)mshutter_close_timer_handler0;
		break;
	}

	ret = Dd_TMR32_Open( ch, D_DDIM_USER_SEM_WAIT_POL );
	if (ret != D_DDIM_OK) {
		printf("\ntimer(%d) open error : ret=%d", ch, ret);
		return;
	}
	
	ret = Dd_TMR32_SetTimer( ch, usec, pcallback );
	if(ret != D_DDIM_OK) {
		Dd_TMR32_Stop(ch);
		Dd_TMR32_Close(ch);
		printf("\ntimer(%d) set error : ret=%d", ch, ret);
		return;
	}
	
	ret = Dd_TMR32_Start(ch);
	if(ret != D_DDIM_OK) {
		printf("\ntimer(%d) start error : ret=%d", ch, ret);
		return;
	}
#endif

	return;

}

