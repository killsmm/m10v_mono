/**
 * @file
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "camera_sensor_imx363.h"
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
static E_CAMERA_ERR_CODE access_tbl(E_CAMERA_ID sensor_id, const T_CAMERA_SENSOR_REG* tbl);

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
const T_CAMERA_SENSOR_REG IMX363_INIT[] =
{
	// External Clock Setting
	{ .address = 0x0136, .data[0] =	0x18 },
	{ .address = 0x0137, .data[0] =	0x00 },
	// Global Setting
	{ .address = 0x31A3, .data[0] =	0x00 },
	{ .address = 0x64D4, .data[0] =	0x01 },
	{ .address = 0x64D5, .data[0] =	0xAA },
	{ .address = 0x64D6, .data[0] =	0x01 },
	{ .address = 0x64D7, .data[0] =	0xA9 },
	{ .address = 0x64D8, .data[0] =	0x01 },
	{ .address = 0x64D9, .data[0] =	0xA5 },
	{ .address = 0x64DA, .data[0] =	0x01 },
	{ .address = 0x64DB, .data[0] =	0xA1 },
	{ .address = 0x720A, .data[0] =	0x24 },
	{ .address = 0x720B, .data[0] =	0x89 },
	{ .address = 0x720C, .data[0] =	0x85 },
	{ .address = 0x720D, .data[0] =	0xA1 },
	{ .address = 0x720E, .data[0] =	0x6E },
	{ .address = 0x729C, .data[0] =	0x59 },
	{ .address = 0x817C, .data[0] =	0xFF },
	{ .address = 0x817D, .data[0] =	0x80 },
	{ .address = 0x9348, .data[0] =	0x96 },
	{ .address = 0x934B, .data[0] =	0x8C },
	{ .address = 0x934C, .data[0] =	0x82 },
	{ .address = 0x9353, .data[0] =	0xAA },
	{ .address = 0x9354, .data[0] =	0xAA },

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};

const T_CAMERA_SENSOR_REG		IMX363_MODE4_Fullp30_4032x3024_pd_4032x756[] =
{
	//Mode Setting		
	{ .address = 0x0112, .data[0] = 0x0A },
	{ .address = 0x0113, .data[0] = 0x0A },
	{ .address = 0x0114, .data[0] = 0x03 },
	{ .address = 0x0220, .data[0] = 0x00 },
	{ .address = 0x0221, .data[0] = 0x11 },
	{ .address = 0x0340, .data[0] = 0x0C },
	{ .address = 0x0341, .data[0] = 0x62 },
	{ .address = 0x0342, .data[0] = 0x22 },
	{ .address = 0x0343, .data[0] = 0x80 },
	{ .address = 0x0381, .data[0] = 0x01 },
	{ .address = 0x0383, .data[0] = 0x01 },
	{ .address = 0x0385, .data[0] = 0x01 },
	{ .address = 0x0387, .data[0] = 0x01 },
	{ .address = 0x0900, .data[0] = 0x00 },
	{ .address = 0x0901, .data[0] = 0x11 },
	{ .address = 0x30F4, .data[0] = 0x02 },
	{ .address = 0x30F5, .data[0] = 0x80 },
	{ .address = 0x30F6, .data[0] = 0x00 },
	{ .address = 0x30F7, .data[0] = 0xC8 },
	{ .address = 0x31A0, .data[0] = 0x00 },
	{ .address = 0x31A5, .data[0] = 0x00 },
	{ .address = 0x31A6, .data[0] = 0x00 },
	{ .address = 0x560F, .data[0] = 0xBE },
	{ .address = 0x5856, .data[0] = 0x08 },
	{ .address = 0x58D0, .data[0] = 0x10 },
	{ .address = 0x734A, .data[0] = 0x01 },
	{ .address = 0x734F, .data[0] = 0x2B },
	{ .address = 0x7441, .data[0] = 0x55 },
	{ .address = 0x7914, .data[0] = 0x03 },
	{ .address = 0x7928, .data[0] = 0x04 },
	{ .address = 0x7929, .data[0] = 0x04 },
	{ .address = 0x793F, .data[0] = 0x03 },
	{ .address = 0xBC7B, .data[0] = 0x18 },
	//Output Size Setting		
	{ .address = 0x0344, .data[0] =	0x00 },            
	{ .address = 0x0345, .data[0] =	0x00 },            
	{ .address = 0x0346, .data[0] =	0x00 },            
	{ .address = 0x0347, .data[0] =	0x00 },            
	{ .address = 0x0348, .data[0] =	0x0F },            
	{ .address = 0x0349, .data[0] =	0xBF },            
	{ .address = 0x034A, .data[0] =	0x0B },            
	{ .address = 0x034B, .data[0] =	0xCF },            
	{ .address = 0x034C, .data[0] =	0x0F },            
	{ .address = 0x034D, .data[0] =	0xC0 },            
	{ .address = 0x034E, .data[0] =	0x0B },            
	{ .address = 0x034F, .data[0] =	0xD0 },            
	{ .address = 0x0408, .data[0] =	0x00 },            
	{ .address = 0x0409, .data[0] =	0x00 },            
	{ .address = 0x040A, .data[0] =	0x00 },            
	{ .address = 0x040B, .data[0] =	0x00 },            
	{ .address = 0x040C, .data[0] =	0x0F },            
	{ .address = 0x040D, .data[0] =	0xC0 },            
	{ .address = 0x040E, .data[0] =	0x0B },            
	{ .address = 0x040F, .data[0] =	0xD0 },            
	//Clock Setting		
	{ .address = 0x0301, .data[0] =	0x03 },
	{ .address = 0x0303, .data[0] =	0x02 },
	{ .address = 0x0305, .data[0] =	0x04 },
	{ .address = 0x0306, .data[0] =	0x00 },
	{ .address = 0x0307, .data[0] =	0xD2 },
	{ .address = 0x0309, .data[0] =	0x0A },
	{ .address = 0x030B, .data[0] =	0x01 },
	{ .address = 0x030D, .data[0] =	0x04 },
	{ .address = 0x030E, .data[0] =	0x00 },
	{ .address = 0x030F, .data[0] =	0xDF },
	{ .address = 0x0310, .data[0] =	0x01 },
	//Integration Time Setting		
	{ .address = 0x0202, .data[0] =	0x0C },
	{ .address = 0x0203, .data[0] =	0x52 },
	{ .address = 0x0224, .data[0] =	0x01 },
	{ .address = 0x0225, .data[0] =	0xF4 },
	//Gain Setting		
	{ .address = 0x0204, .data[0] =	0x00 },
	{ .address = 0x0205, .data[0] =	0x00 },
	{ .address = 0x0216, .data[0] =	0x00 },
	{ .address = 0x0217, .data[0] =	0x00 },
	{ .address = 0x020E, .data[0] =	0x01 },
	{ .address = 0x020F, .data[0] =	0x00 },
	{ .address = 0x0226, .data[0] =	0x01 },
	{ .address = 0x0227, .data[0] =	0x00 },

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};

const T_CAMERA_SENSOR_REG IMX363_MODE4_4kp60_4032x2272_pd_4032x568[] =
{
	//Mode Setting		
	{ .address = 0x0112, .data[0] = 0x0A },
	{ .address = 0x0113, .data[0] = 0x0A },
	{ .address = 0x0114, .data[0] = 0x03 },
	{ .address = 0x0220, .data[0] = 0x00 },
	{ .address = 0x0221, .data[0] = 0x11 },
	{ .address = 0x0340, .data[0] = 0x09 },
	{ .address = 0x0341, .data[0] = 0x4C },
	{ .address = 0x0342, .data[0] = 0x16 },
	{ .address = 0x0343, .data[0] = 0xF8 },
	{ .address = 0x0381, .data[0] = 0x01 },
	{ .address = 0x0383, .data[0] = 0x01 },
	{ .address = 0x0385, .data[0] = 0x01 },
	{ .address = 0x0387, .data[0] = 0x01 },
	{ .address = 0x0900, .data[0] = 0x00 },
	{ .address = 0x0901, .data[0] = 0x11 },
	{ .address = 0x30F4, .data[0] = 0x01 },
	{ .address = 0x30F5, .data[0] = 0xF4 },
	{ .address = 0x30F6, .data[0] = 0x00 },
	{ .address = 0x30F7, .data[0] = 0x14 },
	{ .address = 0x31A0, .data[0] = 0x00 },
	{ .address = 0x31A5, .data[0] = 0x00 },
	{ .address = 0x31A6, .data[0] = 0x00 },
	{ .address = 0x560F, .data[0] = 0x64 },
	{ .address = 0x5856, .data[0] = 0x08 },
	{ .address = 0x58D0, .data[0] = 0x10 },
	{ .address = 0x734A, .data[0] = 0x01 },
	{ .address = 0x734F, .data[0] = 0x2B },
	{ .address = 0x7441, .data[0] = 0x55 },
	{ .address = 0x7914, .data[0] = 0x03 },
	{ .address = 0x7928, .data[0] = 0x04 },
	{ .address = 0x7929, .data[0] = 0x04 },
	{ .address = 0x793F, .data[0] = 0x03 },
	{ .address = 0xBC7B, .data[0] = 0x18 },
	//Output Size Setting		
	{ .address = 0x0344, .data[0] =	0x00 },            
	{ .address = 0x0345, .data[0] =	0x00 },            
	{ .address = 0x0346, .data[0] =	0x01 },            
	{ .address = 0x0347, .data[0] =	0x78 },            
	{ .address = 0x0348, .data[0] =	0x0F },            
	{ .address = 0x0349, .data[0] =	0xBF },            
	{ .address = 0x034A, .data[0] =	0x0A },            
	{ .address = 0x034B, .data[0] =	0x57 },            
	{ .address = 0x034C, .data[0] =	0x0F },            
	{ .address = 0x034D, .data[0] =	0xC0 },            
	{ .address = 0x034E, .data[0] =	0x08 },            
	{ .address = 0x034F, .data[0] =	0xE0 },            
	{ .address = 0x0408, .data[0] =	0x00 },            
	{ .address = 0x0409, .data[0] =	0x00 },            
	{ .address = 0x040A, .data[0] =	0x00 },            
	{ .address = 0x040B, .data[0] =	0x00 },            
	{ .address = 0x040C, .data[0] =	0x0F },            
	{ .address = 0x040D, .data[0] =	0xC0 },            
	{ .address = 0x040E, .data[0] =	0x08 },            
	{ .address = 0x040F, .data[0] =	0xE0 },            
	//Clock Setting		
	{ .address = 0x0301, .data[0] =	0x03 },
	{ .address = 0x0303, .data[0] =	0x02 },
	{ .address = 0x0305, .data[0] =	0x04 },
	{ .address = 0x0306, .data[0] =	0x00 },
	{ .address = 0x0307, .data[0] =	0xD2 },
	{ .address = 0x0309, .data[0] =	0x0A },
	{ .address = 0x030B, .data[0] =	0x01 },
	{ .address = 0x030D, .data[0] =	0x04 },
	{ .address = 0x030E, .data[0] =	0x01 },
	{ .address = 0x030F, .data[0] =	0x57 },
	{ .address = 0x0310, .data[0] =	0x01 },
	//Integration Time Setting	
	{ .address = 0x0202, .data[0] =	0x09 },
	{ .address = 0x0203, .data[0] =	0x3C },
	{ .address = 0x0224, .data[0] =	0x01 },
	{ .address = 0x0225, .data[0] =	0xF4 },
	//Gain Setting		
	{ .address = 0x0204, .data[0] =	0x00 },
	{ .address = 0x0205, .data[0] =	0x00 },
	{ .address = 0x0216, .data[0] =	0x00 },
	{ .address = 0x0217, .data[0] =	0x00 },
	{ .address = 0x020E, .data[0] =	0x01 },
	{ .address = 0x020F, .data[0] =	0x00 },
	{ .address = 0x0226, .data[0] =	0x01 },
	{ .address = 0x0227, .data[0] =	0x00 },

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};

const T_CAMERA_SENSOR_REG IMX363_MODE4_Fhdp90_2016x1136_pd_2016x284[] =
{
	//Mode Setting		
	{ .address = 0x0112, .data[0] = 0x0A },
	{ .address = 0x0113, .data[0] = 0x0A },
	{ .address = 0x0114, .data[0] = 0x03 },
	{ .address = 0x0220, .data[0] = 0x00 },
	{ .address = 0x0221, .data[0] = 0x11 },
	{ .address = 0x0340, .data[0] = 0x06 },
	{ .address = 0x0341, .data[0] = 0x9A },
	{ .address = 0x0342, .data[0] = 0x15 },
	{ .address = 0x0343, .data[0] = 0x90 },
	{ .address = 0x0381, .data[0] = 0x01 },
	{ .address = 0x0383, .data[0] = 0x01 },
	{ .address = 0x0385, .data[0] = 0x01 },
	{ .address = 0x0387, .data[0] = 0x01 },
	{ .address = 0x0900, .data[0] = 0x01 },
	{ .address = 0x0901, .data[0] = 0x22 },
	{ .address = 0x30F4, .data[0] = 0x01 },
	{ .address = 0x30F5, .data[0] = 0xF4 },
	{ .address = 0x30F6, .data[0] = 0x00 },
	{ .address = 0x30F7, .data[0] = 0x14 },
	{ .address = 0x31A0, .data[0] = 0x00 },
	{ .address = 0x31A5, .data[0] = 0x00 },
	{ .address = 0x31A6, .data[0] = 0x01 },
	{ .address = 0x560F, .data[0] = 0x2A },
	{ .address = 0x5856, .data[0] = 0x08 },
	{ .address = 0x58D0, .data[0] = 0x10 },
	{ .address = 0x734A, .data[0] = 0x01 },
	{ .address = 0x734F, .data[0] = 0x2B },
	{ .address = 0x7441, .data[0] = 0x55 },
	{ .address = 0x7914, .data[0] = 0x03 },
	{ .address = 0x7928, .data[0] = 0x04 },
	{ .address = 0x7929, .data[0] = 0x04 },
	{ .address = 0x793F, .data[0] = 0x03 },
	{ .address = 0xBC7B, .data[0] = 0x18 },
	//Output Size Setting		
	{ .address = 0x0344, .data[0] =	0x00 },            
	{ .address = 0x0345, .data[0] =	0x00 },            
	{ .address = 0x0346, .data[0] =	0x01 },            
	{ .address = 0x0347, .data[0] =	0x78 },            
	{ .address = 0x0348, .data[0] =	0x0F },            
	{ .address = 0x0349, .data[0] =	0xBF },            
	{ .address = 0x034A, .data[0] =	0x0A },            
	{ .address = 0x034B, .data[0] =	0x57 },            
	{ .address = 0x034C, .data[0] =	0x07 },            
	{ .address = 0x034D, .data[0] =	0xE0 },            
	{ .address = 0x034E, .data[0] =	0x04 },            
	{ .address = 0x034F, .data[0] =	0x70 },            
	{ .address = 0x0408, .data[0] =	0x00 },            
	{ .address = 0x0409, .data[0] =	0x00 },            
	{ .address = 0x040A, .data[0] =	0x00 },            
	{ .address = 0x040B, .data[0] =	0x00 },            
	{ .address = 0x040C, .data[0] =	0x07 },            
	{ .address = 0x040D, .data[0] =	0xE0 },            
	{ .address = 0x040E, .data[0] =	0x04 },            
	{ .address = 0x040F, .data[0] =	0x70 },            
	//Clock Setting		
	{ .address = 0x0301, .data[0] =	0x03 },
	{ .address = 0x0303, .data[0] =	0x02 },
	{ .address = 0x0305, .data[0] =	0x04 },
	{ .address = 0x0306, .data[0] =	0x00 },
	{ .address = 0x0307, .data[0] =	0xD2 },
	{ .address = 0x0309, .data[0] =	0x0A },
	{ .address = 0x030B, .data[0] =	0x01 },
	{ .address = 0x030D, .data[0] =	0x04 },
	{ .address = 0x030E, .data[0] =	0x00 },
	{ .address = 0x030F, .data[0] =	0xCC },
	{ .address = 0x0310, .data[0] =	0x01 },
	//Integration Time Setting	
	{ .address = 0x0202, .data[0] =	0x06 },
	{ .address = 0x0203, .data[0] =	0x8A },
	{ .address = 0x0224, .data[0] =	0x01 },
	{ .address = 0x0225, .data[0] =	0xF4 },
	//Gain Setting		
	{ .address = 0x0204, .data[0] =	0x00 },
	{ .address = 0x0205, .data[0] =	0x00 },
	{ .address = 0x0216, .data[0] =	0x00 },
	{ .address = 0x0217, .data[0] =	0x00 },
	{ .address = 0x020E, .data[0] =	0x01 },
	{ .address = 0x020F, .data[0] =	0x00 },
	{ .address = 0x0226, .data[0] =	0x01 },
	{ .address = 0x0227, .data[0] =	0x00 },

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};

const T_CAMERA_SENSOR_BASIC		IMX363_BASIC =
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_24MHz,
	.access			= E_CAMERA_ACCESS_I2C,
	.sensor_if		= E_CAMERA_SENSOR_IF_DMIPI,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_VD,
};
const T_CAMERA_SENSOR_MODE		IMX363_MODE[E_SENSOR_IMX363_MODE_MAX] =
{
	[E_SENSOR_IMX363_MODE4_4kp60_4032x2272_pd_4032x568] = {
		.mode_reg				= IMX363_MODE4_4kp60_4032x2272_pd_4032x568,
		.pdaf_mode_reg			= IMX363_MODE4_4kp60_4032x2272_pd_4032x568,

		.embedded_lines			= 2,
		.total_pix_width		= 4032,
		.total_pix_lines		= 2272 + 2, // +2 embedded lines
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 0,
		.active_pix_y_pos		= 2,
		.active_pix_width		= 4032,
		.active_pix_lines		= 2272,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.dualpd_line_rsz_rate	= 0x0104, // 1/4
		.input_bit_width		= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,

		.hdocyc					= 5880,
		.vdocyc					= 2380,
		.exp_end_line_offset	= 16,
		.exp_fine				= 0.0,
		.base_SV				= 4.0,

		.Mpps_HDclk				= 840.0,
		.one_lane_Mbps			= 1260.0,
		.lane_num				= 4,

		.fps					= 60.02,
	},
	[E_SENSOR_IMX363_MODE4_Fhdp90_2016x1136_pd_2016x284] = {
		.mode_reg				= IMX363_MODE4_Fhdp90_2016x1136_pd_2016x284,
		.pdaf_mode_reg			= IMX363_MODE4_Fhdp90_2016x1136_pd_2016x284,

		.embedded_lines			= 2,
		.total_pix_width		= 2016,
		.total_pix_lines		= 1136 + 2, // +2 embedded lines
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 0,
		.active_pix_y_pos		= 2,
		.active_pix_width		= 2016,
		.active_pix_lines		= 1136,
		.active_pix_x_rsz_ratio	= 1.0/2.0,
		.active_pix_y_rsz_ratio	= 1.0/2.0,
		.dualpd_line_rsz_rate	= 0x0104, // 1/4
		.input_bit_width		= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,

		.hdocyc					= 5520,
		.vdocyc					= 1690,
		.exp_end_line_offset	= 16,
		.exp_fine				= 0.0,
		.base_SV				= 4.0,

		.Mpps_HDclk				= 840.0,
		.one_lane_Mbps			= 1260.0,
		.lane_num				= 4,

		.fps					= 90.04,
	},
	[E_SENSOR_IMX363_MODE4_Fullp30_4032x3024_pd_4032x756] = {
		.mode_reg				= IMX363_MODE4_Fullp30_4032x3024_pd_4032x756,
		.pdaf_mode_reg			= IMX363_MODE4_Fullp30_4032x3024_pd_4032x756,
		
		.embedded_lines 		= 2,
		.total_pix_width		= 4032,
		.total_pix_lines		= 3024 + 2, // +2 embedded lines
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 0,
		.active_pix_y_pos		= 2,
		.active_pix_width		= 4032,
		.active_pix_lines		= 3024,
		.active_pix_x_rsz_ratio = 1.0,
		.active_pix_y_rsz_ratio = 1.0,
		.dualpd_line_rsz_rate	= 0x0104, // 1/4
		.input_bit_width		= 10,
		.first_active_pix_color = FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		
		.hdocyc 				= 8832,
		.vdocyc 				= 3170,
		.exp_end_line_offset	= 16,
		.exp_fine				= 0.0,
		.base_SV				= 4.0,
		
		.Mpps_HDclk 			= 840.0,
		.one_lane_Mbps			= 1260.0,
		.lane_num				= 4,
		
		.fps					= 30.00,
	},
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX363_Clock_Gpio_Init(VOID)
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

E_CAMERA_ERR_CODE Sensor_IMX363_Standby(E_CAMERA_ID sensor_id)
{
	UCHAR					data;

	data = 0;

	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x0100,
							&data,
							1);
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX363_Stream(E_CAMERA_ID sensor_id)
{
	UCHAR					data;

	data = 1;

	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x0100,
							&data,
							1);
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX363_Init(E_CAMERA_ID sensor_id)
{
	return access_tbl(sensor_id, IMX363_INIT);
}

E_CAMERA_ERR_CODE Sensor_IMX363_CancelStandby(E_CAMERA_ID sensor_id)
{
	return access_tbl(sensor_id, IMX363_INIT);
}

E_CAMERA_ERR_CODE Sensor_IMX363_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol)
{
	return dol ? access_tbl(sensor_id, IMX363_MODE[mode].dol_mode_reg) : access_tbl(sensor_id, IMX363_MODE[mode].mode_reg);
}

E_CAMERA_ERR_CODE Sensor_IMX363_Pdaf_Mode(E_CAMERA_ID sensor_id, UINT32 mode)
{
	return access_tbl(sensor_id, IMX363_MODE[mode].pdaf_mode_reg);
}

E_CAMERA_ERR_CODE Sensor_IMX363_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
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
	CIT = (UINT32) ( exp_sec * (IMX363_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / IMX363_MODE[mode].hdocyc - IMX363_MODE[mode].exp_fine);

	if( CIT > ( 65534 - IMX363_MODE[mode].exp_end_line_offset ) ){ // 65534 is max of FLL
		CIT = ( 65534 - IMX363_MODE[mode].exp_end_line_offset );
	}

	if( CIT == 0 ){
		CIT = 1;
	}

	// Frame Length Line
	if( CIT > IMX363_MODE[mode].vdocyc - IMX363_MODE[mode].exp_end_line_offset ){
		FLL = CIT + IMX363_MODE[mode].exp_end_line_offset;
	}
	else{
		FLL = IMX363_MODE[mode].vdocyc;
	}

	// Clip by FPS
	if( in->max_fps == D_CAMERA_FPS_AUTO && in->min_fps == D_CAMERA_FPS_AUTO ){
		// auto, nothing to do
	}else{
		default_fps = (IMX363_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX363_MODE[mode].hdocyc / (FLOAT)IMX363_MODE[mode].vdocyc;
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
		req_fps = (IMX363_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX363_MODE[mode].hdocyc / (FLOAT)FLL;
		if( req_fps < min_fps ){
			FLL = (IMX363_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX363_MODE[mode].hdocyc / min_fps;
		}
		if( req_fps > max_fps ){
			FLL = (IMX363_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX363_MODE[mode].hdocyc / max_fps;
		}
		// clip CIT
		if( CIT > ( FLL - IMX363_MODE[mode].exp_end_line_offset ) ){
			CIT = ( FLL - IMX363_MODE[mode].exp_end_line_offset );
		}
	}

	// Binning mode x 2, CIT : 2/4/6 are prohibit
	if( mode == E_SENSOR_IMX363_MODE4_Fhdp90_2016x1136_pd_2016x284 && (CIT == 2 || CIT == 4 || CIT == 6)) {
		CIT += 1;
	}

	// Analog Gain
	again = in->gain_times;
	if( again < 1.0 ){
		again = 1.0;
	}

	AGG = (UINT32)( 512.0 - (512.0 / again) );
	if( AGG > 448 ){
		AGG = 448;
	}
	again = ( 512.0 / (512.0 - (FLOAT)AGG) );

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
	exp_sec = (FLOAT)( CIT + IMX363_MODE[mode].exp_fine ) * (FLOAT)IMX363_MODE[mode].hdocyc / (IMX363_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	out->exp_msec = exp_sec * 1000.0;
	out->fps = (IMX363_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / IMX363_MODE[mode].hdocyc / FLL;
	out->hdocyc = IMX363_MODE[mode].hdocyc;
	out->vdocyc = FLL;
	out->again_times = again;
	out->dgain_times = dgain;

	// Reg
	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = IMX363_MODE[mode].hdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = FLL;
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] = CIT;
	out_reg->reg[E_CAMERA_SENSOR_REG_AGAIN] = AGG;
	out_reg->reg[E_CAMERA_SENSOR_REG_DGAIN] = DG;

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX363_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in)
{
	UCHAR data[8];

	// group param hold ON
	data[0] = 0x01;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x0104,
							data,
							1);

	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// coarse integration time & analog_gain_global
	data[0] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_EXP] >> 8) & 0xFF);
	data[1] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_EXP]       & 0xFF);
	data[2] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_AGAIN] >> 8) & 0xFF);
	data[3] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_AGAIN]       & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x0202,
							data,
							4);

	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// frame length line
	data[0] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_VDOCYC] >> 8) & 0xFF);
	data[1] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_VDOCYC]       & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x0340,
							data,
							2);

	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// digital gain ( DPGA_USE_GLOBAL_GAIN = 1 all color )
	data[0] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_DGAIN] >> 8) & 0xFF);
	data[1] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_DGAIN]       & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x020E,
							data,
							2);

	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// group param hold OFF
	data[0] = 0;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x0104,
							data,
							1);

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
