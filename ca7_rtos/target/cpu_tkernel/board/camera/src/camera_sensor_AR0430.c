
/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "../include/camera_sensor_AR0430.h"

#include "camera_config.h"
#include "os_user_custom.h"
#include "im_pro.h"
#include <math.h>

#include "share.h" //debug
#include "dd_top.h" //GPIO

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define TEST_PATTERN_MODE
//#define TEST_440MBPS_MODE
//#define TEST_220MBPS_MODE
//#define MCLK_SLOW_DOWN E_CAMERA_SENSOR_INCLK_MAX
#define SET_VIRTUAL_CHANNEL

#ifdef TEST_PATTERN_MODE
	//#define SOLID_GREY
	#define PN9
#endif
#define	D_TBL_END		(0xFFFF)
#define	D_TBL_WAIT_MS	(0xFFFE)
#define LINE_LENGTH_PCK_10BIT		(0x1200) //4608. (x_addr_end - x_addr_start)/sub_sample
#define LINE_LENGTH_PCK_12BIT		(0x1608) //5640. (x_addr_end - x_addr_start)/sub_sample + min_line_blanking
#define	FRAME_LENGTH_LINES_10BIT	(0x10F4) //4340. REQ_FPS=30, 20000000/LINE_LENGTH_PCK_10BIT
#define	FRAME_LENGTH_LINES_12BIT	(0x0DDA) //3546. REQ_FPS=30, 20000000/LINE_LENGTH_PCK_10BIT

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static E_CAMERA_ERR_CODE access_tbl(E_CAMERA_ID sensor_id, const T_CAMERA_SENSOR_REG1* tbl);

static T_AR0430_GAIN G_gain_value = {0};
/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
const T_CAMERA_SENSOR_REG1		AR0430_MODE_FULL_2316x1746_RAW10_30FPS[] =
{
//corrections_recommended-start
	{ .address = 0x3042, .d_type.hword = 0x0004 },
	{ .address = 0x3220, .d_type.hword = 0x0000 },
	{ .address = 0x3222, .d_type.hword = 0x6060 },
	{ .address = 0x3C50, .d_type.hword = 0x0001 },
	{ .address = 0x3C66, .d_type.hword = 0x0FFF },
	{ .address = 0x3C68, .d_type.hword = 0x000A },
	{ .address = 0x3C6A, .d_type.hword = 0x0028 },
	{ .address = 0x3C6C, .d_type.hword = 0x0500 },
	{ .address = 0x3C6E, .d_type.hword = 0xECA0 },
	{ .address = 0x3C70, .d_type.hword = 0x003C },
	{ .address = 0x3C72, .d_type.hword = 0x00A8 },
	{ .address = 0x3C74, .d_type.hword = 0x000A },
	{ .address = 0x3C76, .d_type.hword = 0x0002 },
	{ .address = 0x3C78, .d_type.hword = 0x0003 },
	{ .address = 0x3C7A, .d_type.hword = 0x0004 },
	{ .address = 0x3C7C, .d_type.hword = 0x0007 },
	{ .address = 0x3C7E, .d_type.hword = 0x4020 },
	{ .address = 0x3C80, .d_type.hword = 0x007B },
	{ .address = 0x3C82, .d_type.hword = 0x0000 },
	{ .address = 0x3C84, .d_type.hword = 0x0210 },
	{ .address = 0x3C86, .d_type.hword = 0x000A },
	{ .address = 0x3C88, .d_type.hword = 0x0A0A },
	{ .address = 0x3C8A, .d_type.hword = 0x0A0A },
	{ .address = 0x3C8C, .d_type.hword = 0x1E1E },
	{ .address = 0x3C8E, .d_type.hword = 0x1E1E },
	{ .address = 0x3C90, .d_type.hword = 0x4444 },
	{ .address = 0x3C92, .d_type.hword = 0x0000 },
	{ .address = 0x3C94, .d_type.hword = 0x0000 },
	{ .address = 0x3C96, .d_type.hword = 0x0010 },
	{ .address = 0x3C98, .d_type.hword = 0x0FD7 },
	{ .address = 0x3C9A, .d_type.hword = 0x0810 },
	{ .address = 0x3C9E, .d_type.hword = 0x0000 },
	{ .address = 0x3C9C, .d_type.hword = 0x0000 },
	{ .address = 0x3CC2, .d_type.hword = 0x0000 },
	{ .address = 0x3CC4, .d_type.hword = 0x0000 },
	{ .address = 0x3CC6, .d_type.hword = 0x0000 },
	{ .address = 0x3CA0, .d_type.hword = 0x0000 },
	{ .address = 0x3172, .d_type.hword = 0x0601 },
////corrections_recommended-end
//Analog_Setup_Recommended-start
	{ .address = 0x3EB4, .d_type.hword = 0x676A },
	{ .address = 0x3EBC, .d_type.hword = 0x0010 },
	{ .address = 0x3ECE, .d_type.hword = 0x008B },
	{ .address = 0x3ED0, .d_type.hword = 0x0071 },
	{ .address = 0x3ED2, .d_type.hword = 0xB85C },
	{ .address = 0x3ED4, .d_type.hword = 0x120A },
	{ .address = 0x3EDC, .d_type.hword = 0x7D04 },
	{ .address = 0x30EE, .d_type.hword = 0x1140 },
	{ .address = 0x3120, .d_type.hword = 0x0001 },
	{ .address = 0x3044, .d_type.hword = 0x05E0 },
	{ .address = 0x30C0, .d_type.hword = 0x0004 },
	{ .address = 0x30C2, .d_type.hword = 0x0100 },
	{ .address = 0x316E, .d_type.hword = 0x8400 },
	{ .address = 0x3EF0, .d_type.hword = 0x3E3E },
	{ .address = 0x3EF2, .d_type.hword = 0x8082 },
	{ .address = 0x3EAE, .d_type.hword = 0x003F },
	{ .address = 0x3EC0, .d_type.hword = 0x8888 },
	{ .address = 0x3EC2, .d_type.hword = 0x008B },
	{ .address = 0x3EC4, .d_type.hword = 0x2763 },
	{ .address = 0x3EC6, .d_type.hword = 0x913D },
	{ .address = 0x3ECA, .d_type.hword = 0x90B7 },
	{ .address = 0x3EBA, .d_type.hword = 0x0C11 },
	{ .address = 0x3EBE, .d_type.hword = 0x5504 },
	{ .address = 0x3ED8, .d_type.hword = 0x9844 },
	{ .address = 0x3EDA, .d_type.hword = 0xB21D },
	{ .address = 0x3F3A, .d_type.hword = 0x0080 },
////Analog_Setup_Recommended-end
//Pixel_Timing_Recommended_10bit-start
	{ .address = 0x3D00, .d_type.hword = 0x046C },
	{ .address = 0x3D02, .d_type.hword = 0xFF72 },
	{ .address = 0x3D04, .d_type.hword = 0xFFFF },
	{ .address = 0x3D06, .d_type.hword = 0xFFFF },
	{ .address = 0x3D08, .d_type.hword = 0x8000 },
	{ .address = 0x3D0A, .d_type.hword = 0x3251 },
	{ .address = 0x3D0C, .d_type.hword = 0x2328 },
	{ .address = 0x3D0E, .d_type.hword = 0x080C },
	{ .address = 0x3D10, .d_type.hword = 0x6280 },
	{ .address = 0x3D12, .d_type.hword = 0x3105 },
	{ .address = 0x3D14, .d_type.hword = 0x4B73 },
	{ .address = 0x3D16, .d_type.hword = 0x11C0 },
	{ .address = 0x3D18, .d_type.hword = 0x1013 },
	{ .address = 0x3D1A, .d_type.hword = 0x3108 },
	{ .address = 0x3D1C, .d_type.hword = 0x2086 },
	{ .address = 0x3D1E, .d_type.hword = 0x8173 },
	{ .address = 0x3D20, .d_type.hword = 0x8073 },
	{ .address = 0x3D22, .d_type.hword = 0x821C },
	{ .address = 0x3D24, .d_type.hword = 0x0082 },
	{ .address = 0x3D26, .d_type.hword = 0x588A },
	{ .address = 0x3D28, .d_type.hword = 0x499E },
	{ .address = 0x3D2A, .d_type.hword = 0x4380 },
	{ .address = 0x3D2C, .d_type.hword = 0x1103 },
	{ .address = 0x3D2E, .d_type.hword = 0xC560 },
	{ .address = 0x3D30, .d_type.hword = 0x8F73 },
	{ .address = 0x3D32, .d_type.hword = 0x9447 },
	{ .address = 0x3D34, .d_type.hword = 0x8568 },
	{ .address = 0x3D36, .d_type.hword = 0x8061 },
	{ .address = 0x3D38, .d_type.hword = 0x8049 },
	{ .address = 0x3D3A, .d_type.hword = 0x4759 },
	{ .address = 0x3D3C, .d_type.hword = 0x6880 },
	{ .address = 0x3D3E, .d_type.hword = 0x5680 },
	{ .address = 0x3D40, .d_type.hword = 0x558E },
	{ .address = 0x3D42, .d_type.hword = 0x7384 },
	{ .address = 0x3D44, .d_type.hword = 0x4686 },
	{ .address = 0x3D46, .d_type.hword = 0x100C },
	{ .address = 0x3D48, .d_type.hword = 0x8B6A },
	{ .address = 0x3D4A, .d_type.hword = 0x8028 },
	{ .address = 0x3D4C, .d_type.hword = 0x4097 },
	{ .address = 0x3D4E, .d_type.hword = 0x100C },
	{ .address = 0x3D50, .d_type.hword = 0x847B },
	{ .address = 0x3D52, .d_type.hword = 0x8246 },
	{ .address = 0x3D54, .d_type.hword = 0x8273 },
	{ .address = 0x3D56, .d_type.hword = 0x9928 },
	{ .address = 0x3D58, .d_type.hword = 0x4055 },
	{ .address = 0x3D5A, .d_type.hword = 0x6A59 },
	{ .address = 0x3D5C, .d_type.hword = 0x8349 },
	{ .address = 0x3D5E, .d_type.hword = 0x6B82 },
	{ .address = 0x3D60, .d_type.hword = 0x6B82 },
	{ .address = 0x3D62, .d_type.hword = 0x6D82 },
	{ .address = 0x3D64, .d_type.hword = 0x7382 },
	{ .address = 0x3D66, .d_type.hword = 0x73AC },
	{ .address = 0x3D68, .d_type.hword = 0x7388 },
	{ .address = 0x3D6A, .d_type.hword = 0x4785 },
	{ .address = 0x3D6C, .d_type.hword = 0x6882 },
	{ .address = 0x3D6E, .d_type.hword = 0x4947 },
	{ .address = 0x3D70, .d_type.hword = 0x5968 },
	{ .address = 0x3D72, .d_type.hword = 0x8358 },
	{ .address = 0x3D74, .d_type.hword = 0x801A },
	{ .address = 0x3D76, .d_type.hword = 0x0081 },
	{ .address = 0x3D78, .d_type.hword = 0x738C },
	{ .address = 0x3D7A, .d_type.hword = 0x200C },
	{ .address = 0x3D7C, .d_type.hword = 0x8E10 },
	{ .address = 0x3D7E, .d_type.hword = 0xF080 },
	{ .address = 0x3D80, .d_type.hword = 0x4585 },
	{ .address = 0x3D82, .d_type.hword = 0x6A80 },
	{ .address = 0x3D84, .d_type.hword = 0x2840 },
	{ .address = 0x3D86, .d_type.hword = 0x4182 },
	{ .address = 0x3D88, .d_type.hword = 0x4181 },
	{ .address = 0x3D8A, .d_type.hword = 0x4354 },
	{ .address = 0x3D8C, .d_type.hword = 0x8110 },
	{ .address = 0x3D8E, .d_type.hword = 0x0381 },
	{ .address = 0x3D90, .d_type.hword = 0x1030 },
	{ .address = 0x3D92, .d_type.hword = 0x8446 },
	{ .address = 0x3D94, .d_type.hword = 0x8210 },
	{ .address = 0x3D96, .d_type.hword = 0x0CA4 },
	{ .address = 0x3D98, .d_type.hword = 0x4A85 },
	{ .address = 0x3D9A, .d_type.hword = 0x7B89 },
	{ .address = 0x3D9C, .d_type.hword = 0x4A80 },
	{ .address = 0x3D9E, .d_type.hword = 0x6783 },
	{ .address = 0x3DA0, .d_type.hword = 0x7393 },
	{ .address = 0x3DA2, .d_type.hword = 0x100C },
	{ .address = 0x3DA4, .d_type.hword = 0x8146 },
	{ .address = 0x3DA6, .d_type.hword = 0x8110 },
	{ .address = 0x3DA8, .d_type.hword = 0x3081 },
	{ .address = 0x3DAA, .d_type.hword = 0x4311 },
	{ .address = 0x3DAC, .d_type.hword = 0x0341 },
	{ .address = 0x3DAE, .d_type.hword = 0x8100 },
	{ .address = 0x3DB0, .d_type.hword = 0x0A86 },
	{ .address = 0x3DB2, .d_type.hword = 0x1133 },
	{ .address = 0x3DB4, .d_type.hword = 0x8046 },
	{ .address = 0x3DB6, .d_type.hword = 0x8210 },
	{ .address = 0x3DB8, .d_type.hword = 0x0CAE },
	{ .address = 0x3DBA, .d_type.hword = 0x4A88 },
	{ .address = 0x3DBC, .d_type.hword = 0x7373 },
	{ .address = 0x3DBE, .d_type.hword = 0x8273 },
	{ .address = 0x3DC0, .d_type.hword = 0x834A },
	{ .address = 0x3DC2, .d_type.hword = 0x9010 },
	{ .address = 0x3DC4, .d_type.hword = 0x0C46 },
	{ .address = 0x3DC6, .d_type.hword = 0x1133 },
	{ .address = 0x3DC8, .d_type.hword = 0x8100 },
	{ .address = 0x3DCA, .d_type.hword = 0x1800 },
	{ .address = 0x3DCC, .d_type.hword = 0x0681 },
	{ .address = 0x3DCE, .d_type.hword = 0x5581 },
	{ .address = 0x3DD0, .d_type.hword = 0x2CE0 },
	{ .address = 0x3DD2, .d_type.hword = 0x6E80 },
	{ .address = 0x3DD4, .d_type.hword = 0x3645 },
	{ .address = 0x3DD6, .d_type.hword = 0x7000 },
	{ .address = 0x3DD8, .d_type.hword = 0x8000 },
	{ .address = 0x3DDA, .d_type.hword = 0x0382 },
	{ .address = 0x3DDC, .d_type.hword = 0x4BC3 },
	{ .address = 0x3DDE, .d_type.hword = 0x4B82 },
	{ .address = 0x3DE0, .d_type.hword = 0x0003 },
	{ .address = 0x3DE2, .d_type.hword = 0x8070 },
	{ .address = 0x3DE4, .d_type.hword = 0xFFFF },
	{ .address = 0x3DE6, .d_type.hword = 0x937B },
	{ .address = 0x3DE8, .d_type.hword = 0x0000 },
	{ .address = 0x3DEA, .d_type.hword = 0x0000 },
	{ .address = 0x3DEC, .d_type.hword = 0x0000 },
	{ .address = 0x3DEE, .d_type.hword = 0x0000 },
	{ .address = 0x3DF0, .d_type.hword = 0x0000 },
	{ .address = 0x3DF2, .d_type.hword = 0x0000 },
	{ .address = 0x3DF4, .d_type.hword = 0x0000 },
	{ .address = 0x3DF6, .d_type.hword = 0x0000 },
	{ .address = 0x3DF8, .d_type.hword = 0x0000 },
	{ .address = 0x3DFA, .d_type.hword = 0x0000 },
	{ .address = 0x3DFC, .d_type.hword = 0x0000 },
	{ .address = 0x3DFE, .d_type.hword = 0x0000 },
	{ .address = 0x3E00, .d_type.hword = 0x0000 },
	{ .address = 0x3E02, .d_type.hword = 0x0000 },
	{ .address = 0x3E04, .d_type.hword = 0x0000 },
	{ .address = 0x3E06, .d_type.hword = 0x0000 },
	{ .address = 0x3E08, .d_type.hword = 0x0000 },
	{ .address = 0x3E0A, .d_type.hword = 0x0000 },
	{ .address = 0x3E0C, .d_type.hword = 0x0000 },
	{ .address = 0x3E0E, .d_type.hword = 0x0000 },
	{ .address = 0x3E10, .d_type.hword = 0x0000 },
	{ .address = 0x3E12, .d_type.hword = 0x0000 },
	{ .address = 0x3E14, .d_type.hword = 0x0000 },
	{ .address = 0x3E16, .d_type.hword = 0x0000 },
	{ .address = 0x3E18, .d_type.hword = 0x0000 },
	{ .address = 0x3E1A, .d_type.hword = 0x0000 },
	{ .address = 0x3E1C, .d_type.hword = 0x0000 },
	{ .address = 0x3E1E, .d_type.hword = 0x0000 },
	{ .address = 0x3E20, .d_type.hword = 0x0000 },
	{ .address = 0x3E22, .d_type.hword = 0x0000 },
	{ .address = 0x3E24, .d_type.hword = 0x0000 },
	{ .address = 0x3E26, .d_type.hword = 0x0000 },
	{ .address = 0x3E28, .d_type.hword = 0x0000 },
	{ .address = 0x3E2A, .d_type.hword = 0x0000 },
	{ .address = 0x3E2C, .d_type.hword = 0x0000 },
	{ .address = 0x3E2E, .d_type.hword = 0x0000 },
	{ .address = 0x3E30, .d_type.hword = 0x0000 },
	{ .address = 0x3E32, .d_type.hword = 0x0000 },
	{ .address = 0x3E34, .d_type.hword = 0x0000 },
	{ .address = 0x3E36, .d_type.hword = 0x0000 },
	{ .address = 0x3E38, .d_type.hword = 0x0000 },
	{ .address = 0x3E3A, .d_type.hword = 0x0000 },
	{ .address = 0x3E3C, .d_type.hword = 0x0000 },
	{ .address = 0x3E3E, .d_type.hword = 0x0000 },
	{ .address = 0x3E40, .d_type.hword = 0x0000 },
	{ .address = 0x3E42, .d_type.hword = 0x0000 },
	{ .address = 0x3E44, .d_type.hword = 0x0000 },
	{ .address = 0x3E46, .d_type.hword = 0x0000 },
	{ .address = 0x3E48, .d_type.hword = 0x0000 },
	{ .address = 0x3E4A, .d_type.hword = 0x0000 },
	{ .address = 0x3E4C, .d_type.hword = 0x0000 },
	{ .address = 0x3E4E, .d_type.hword = 0x0000 },
	{ .address = 0x3E50, .d_type.hword = 0x0000 },
	{ .address = 0x3E52, .d_type.hword = 0x0000 },
	{ .address = 0x3E54, .d_type.hword = 0x0000 },
	{ .address = 0x3E56, .d_type.hword = 0x0000 },
	{ .address = 0x3E58, .d_type.hword = 0x0000 },
	{ .address = 0x3E5A, .d_type.hword = 0x0000 },
	{ .address = 0x3E5C, .d_type.hword = 0x0000 },
	{ .address = 0x3E5E, .d_type.hword = 0x0000 },
	{ .address = 0x3E60, .d_type.hword = 0x0000 },
	{ .address = 0x3E62, .d_type.hword = 0x0000 },
	{ .address = 0x3E64, .d_type.hword = 0x0000 },
	{ .address = 0x3E66, .d_type.hword = 0x0000 },
	{ .address = 0x3E68, .d_type.hword = 0x0000 },
	{ .address = 0x3E6A, .d_type.hword = 0x0000 },
	{ .address = 0x3E6C, .d_type.hword = 0x0000 },
	{ .address = 0x3E6E, .d_type.hword = 0x0000 },
	{ .address = 0x3E70, .d_type.hword = 0x0000 },
	{ .address = 0x3E72, .d_type.hword = 0x0000 },
	{ .address = 0x3E74, .d_type.hword = 0x0000 },
	{ .address = 0x3E76, .d_type.hword = 0x0000 },
	{ .address = 0x3E78, .d_type.hword = 0x0000 },
	{ .address = 0x3E7A, .d_type.hword = 0x0000 },
	{ .address = 0x3E7C, .d_type.hword = 0x0000 },
	{ .address = 0x3E7E, .d_type.hword = 0x0000 },
	{ .address = 0x3E80, .d_type.hword = 0x0000 },
	{ .address = 0x3E82, .d_type.hword = 0x0000 },
	{ .address = 0x3E84, .d_type.hword = 0x0000 },
	{ .address = 0x3E86, .d_type.hword = 0x0000 },
	{ .address = 0x3E88, .d_type.hword = 0x0000 },
	{ .address = 0x3E8A, .d_type.hword = 0x0000 },
	{ .address = 0x3E8C, .d_type.hword = 0x0000 },
	{ .address = 0x3E8E, .d_type.hword = 0x0000 },
	{ .address = 0x3E90, .d_type.hword = 0x0000 },
	{ .address = 0x3E92, .d_type.hword = 0x0000 },
	{ .address = 0x3E94, .d_type.hword = 0x0000 },
	{ .address = 0x3E96, .d_type.hword = 0x0000 },
	{ .address = 0x3E98, .d_type.hword = 0x0000 },
	{ .address = 0x3E9A, .d_type.hword = 0x0000 },
	{ .address = 0x3E9C, .d_type.hword = 0x0000 },
	{ .address = 0x3E9E, .d_type.hword = 0x0000 },
	{ .address = 0x3EA0, .d_type.hword = 0x0000 },
	{ .address = 0x3EA2, .d_type.hword = 0x0000 },
	{ .address = 0x3EA4, .d_type.hword = 0x0000 },
	{ .address = 0x3EA6, .d_type.hword = 0x0000 },
	{ .address = 0x3EA8, .d_type.hword = 0x0000 },
	{ .address = 0x3EAA, .d_type.hword = 0x0000 },
	{ .address = 0x3EAC, .d_type.hword = 0x0000 },
////Pixel_Timing_Recommended_10bit-end
#ifdef TEST_PATTERN_MODE
	#ifdef SOLID_GREY
	{ .address = 0x0600, .d_type.hword = 0x0001 }, // test_pattern_mode, Solid gry
	#elif defined PN9
	{ .address = 0x0600, .d_type.hword = 0x0004 }, // test_pattern_mode, PN9
	#endif
#endif
#if defined(TEST_440MBPS_MODE) || defined(TEST_PATTERN_MODE)
//PLL_440by150_24MHz_10bit-start
	{ .address = 0x0300, .d_type.hword = 0x0006 }, //VT_PIX_CLK_DIV=6
	{ .address = 0x0302, .d_type.hword = 0x0C01 }, //VT_SYS_CLK_DIV2=12, VT_SYS_CLK_DIV=1
	{ .address = 0x0304, .d_type.hword = 0x0A02 }, //PRE_PLL_CLK_DIV2=12, PRE_PLL_CLK_DIV1=2
	{ .address = 0x0306, .d_type.hword = 0x404B }, //PLL_MULTIPLIER2=64, PLL_MULTIPLIER1=75
	{ .address = 0x0308, .d_type.hword = 0x0008 }, //OP_PIX_CLK_DIV=8
	{ .address = 0x030A, .d_type.hword = 0x0001 }, //OP_SYS_CLK_DIV=1
	{ .address = 0x030C, .d_type.hword = 0x50AE }, //MIPI_INT_PHY_PLL, PHY_PLL_CO=1->2 ,PD_TX_PLL=1, PHY_PLL_CN=16->3, PHY_PLL_CM=174->110
	{ .address = 0x030E, .d_type.hword = 0x0002 }, //PLL_CTRL=2
////PLL_440by150_24MHz_10bit-end
//MIPI_Timing_440x150_24MHz-start
	{ .address = 0x31B0, .d_type.hword = 0x006D }, //Frame preamble
	{ .address = 0x31B2, .d_type.hword = 0x003B }, //Line preamble 
	{ .address = 0x31B4, .d_type.hword = 0x1307 }, //MIPI timing0
	{ .address = 0x31B6, .d_type.hword = 0x16C7 }, //MIPI timing1
	{ .address = 0x31B8, .d_type.hword = 0x1251 }, //MIPI timing2
	{ .address = 0x31BA, .d_type.hword = 0xB808 }, //MIPI timing3
	{ .address = 0x31BC, .d_type.hword = 0x4D3D }, //MIPI timing4
////MIPI_Timing_440x150_24MHz-end
#elif defined TEST_220MBPS_MODE
//PLL_220by150_24MHz_10bit-start
	{ .address = 0x0300, .d_type.hword = 0x0006 }, //VT_PIX_CLK_DIV=6
	{ .address = 0x0302, .d_type.hword = 0x0C01 }, //VT_SYS_CLK_DIV2=12, VT_SYS_CLK_DIV=1
	{ .address = 0x0304, .d_type.hword = 0x0A02 }, //PRE_PLL_CLK_DIV2=12, PRE_PLL_CLK_DIV1=2
	{ .address = 0x0306, .d_type.hword = 0x804B }, //PLL_MULTIPLIER2=128, PLL_MULTIPLIER1=75
	{ .address = 0x0308, .d_type.hword = 0x0010 }, //OP_PIX_CLK_DIV=16
	{ .address = 0x030A, .d_type.hword = 0x0001 }, //OP_SYS_CLK_DIV=1
	{ .address = 0x030C, .d_type.hword = 0x90AE }, //MIPI_INT_PHY_PLL, PHY_PLL_CO=2->4 ,PD_TX_PLL=1, PHY_PLL_CN=16->3, PHY_PLL_CM=174->110
	{ .address = 0x030E, .d_type.hword = 0x0002 }, //PLL_CTRL=2
////PLL_220by150_24MHz_10bit-end
//MIPI_Timing_220x150_24MHz-start
	{ .address = 0x31B0, .d_type.hword = 0x006D }, //Frame preamble
	{ .address = 0x31B2, .d_type.hword = 0x003B }, //Line preamble 
	{ .address = 0x31B4, .d_type.hword = 0x1307 }, //MIPI timing0
	{ .address = 0x31B6, .d_type.hword = 0x16C7 }, //MIPI timing1
	{ .address = 0x31B8, .d_type.hword = 0x1251 }, //MIPI timing2
	{ .address = 0x31BA, .d_type.hword = 0xB808 }, //MIPI timing3
	{ .address = 0x31BC, .d_type.hword = 0x4D3D }, //MIPI timing4
////MIPI_Timing_220x150_24MHz-end
#else
//PLL_880by150_24MHz-start
	{ .address = 0x0300, .d_type.hword = 0x0006 }, //VT_PIX_CLK_DIV=6
	{ .address = 0x0302, .d_type.hword = 0x0C01 }, //VT_SYS_CLK_DIV2=12, VT_SYS_CLK_DIV=1
	{ .address = 0x0304, .d_type.hword = 0x0A02 }, //PRE_PLL_CLK_DIV2=12, PRE_PLL_CLK_DIV1=2
	{ .address = 0x0306, .d_type.hword = 0x204B }, //PLL_MULTIPLIER2=32, PLL_MULTIPLIER1=75
	{ .address = 0x0308, .d_type.hword = 0x0004 }, //OP_PIX_CLK_DIV=4
	{ .address = 0x030A, .d_type.hword = 0x0001 }, //OP_SYS_CLK_DIV=1
	{ .address = 0x030C, .d_type.hword = 0x10AE }, //MIPI_INT_PHY_PLL, PHY_PLL_CO=0->1 ,PD_TX_PLL=1, PHY_PLL_CN=16->3, PHY_PLL_CM=174->110
	{ .address = 0x030E, .d_type.hword = 0x0002 }, //PLL_CTRL=2
////PLL_880by150_24MHz-end
//MIPI_Timing_880x150_24MHz-start
	{ .address = 0x31B0, .d_type.hword = 0x003B }, //Frame preamble
	{ .address = 0x31B2, .d_type.hword = 0x003B }, //Line preamble 
	{ .address = 0x31B4, .d_type.hword = 0x130A }, //MIPI timing0
	{ .address = 0x31B6, .d_type.hword = 0x16CA }, //MIPI timing1
	{ .address = 0x31B8, .d_type.hword = 0x1251 }, //MIPI timing2
	{ .address = 0x31BA, .d_type.hword = 0xB808 }, //MIPI timing3
	{ .address = 0x31BC, .d_type.hword = 0x4D3D }, //MIPI timing4
////MIPI_Timing_880x150_24MHz-end
#endif
//Stream_4MP_2316x1746-start
	{ .address = 0x0344, .d_type.hword = 0x0020 }, //x_addr_start, 32
	{ .address = 0x0346, .d_type.hword = 0x0010 }, //y_addr_start, 16
	{ .address = 0x0348, .d_type.hword = 0x1220 }, //x_addr_end, 4640
	{ .address = 0x034A, .d_type.hword = 0x0DA8 }, //y_addr_end, 3496
	{ .address = 0x034C, .d_type.hword = 0x0900 }, //x_output_size, 2304
	{ .address = 0x034E, .d_type.hword = 0x06CC }, //y_output_size, 1740
	{ .address = 0x3040, .d_type.hword = 0x00C3 }, //read_mode
	//{ .address = 0x3040, .d_type.hword = 0xC0C3 }, //read_mode.mirror & flip
	{ .address = 0x0340, .d_type.hword = FRAME_LENGTH_LINES_10BIT }, //FRAME_LENGTH_LINES
////Stream_4MP_2316x1746_10bit-end

#if defined(TEST_440MBPS_MODE) || defined(TEST_PATTERN_MODE)
//4MP_2316x1746_15fps_440
	{ .address = 0x0342, .d_type.hword = (LINE_LENGTH_PCK_10BIT<<1) }, //LINE_LENGTH_PCK, 9216
#elif defined TEST_220MBPS_MODE
//4MP_2316x1746_7.5fps_10bit_220
	{ .address = 0x0342, .d_type.hword = (LINE_LENGTH_PCK_10BIT<<2) }, //LINE_LENGTH_PCK, 18432
#else
//4MP_2316x1746_30fps_10bit_880
	{ .address = 0x0342, .d_type.hword = LINE_LENGTH_PCK_10BIT }, //LINE_LENGTH_PCK, 4608.
#endif
	{ .address = D_TBL_END, .d_type.hword = 0xFFFF } // END
};

const T_CAMERA_SENSOR_REG1		AR0430_MODE_FULL_2316x1746_RAW12_30FPS[] =
{
//corrections_recommended-start
	{ .address = 0x3042, .d_type.hword = 0x0004 },
	{ .address = 0x3220, .d_type.hword = 0x0000 },
	{ .address = 0x3222, .d_type.hword = 0x6060 },
	{ .address = 0x3C50, .d_type.hword = 0x0001 },
	{ .address = 0x3C66, .d_type.hword = 0x0FFF },
	{ .address = 0x3C68, .d_type.hword = 0x000A },
	{ .address = 0x3C6A, .d_type.hword = 0x0028 },
	{ .address = 0x3C6C, .d_type.hword = 0x0500 },
	{ .address = 0x3C6E, .d_type.hword = 0xECA0 },
	{ .address = 0x3C70, .d_type.hword = 0x003C },
	{ .address = 0x3C72, .d_type.hword = 0x00A8 },
	{ .address = 0x3C74, .d_type.hword = 0x000A },
	{ .address = 0x3C76, .d_type.hword = 0x0002 },
	{ .address = 0x3C78, .d_type.hword = 0x0003 },
	{ .address = 0x3C7A, .d_type.hword = 0x0004 },
	{ .address = 0x3C7C, .d_type.hword = 0x0007 },
	{ .address = 0x3C7E, .d_type.hword = 0x4020 },
	{ .address = 0x3C80, .d_type.hword = 0x007B },
	{ .address = 0x3C82, .d_type.hword = 0x0000 },
	{ .address = 0x3C84, .d_type.hword = 0x0210 },
	{ .address = 0x3C86, .d_type.hword = 0x000A },
	{ .address = 0x3C88, .d_type.hword = 0x0A0A },
	{ .address = 0x3C8A, .d_type.hword = 0x0A0A },
	{ .address = 0x3C8C, .d_type.hword = 0x1E1E },
	{ .address = 0x3C8E, .d_type.hword = 0x1E1E },
	{ .address = 0x3C90, .d_type.hword = 0x4444 },
	{ .address = 0x3C92, .d_type.hword = 0x0000 },
	{ .address = 0x3C94, .d_type.hword = 0x0000 },
	{ .address = 0x3C96, .d_type.hword = 0x0010 },
	{ .address = 0x3C98, .d_type.hword = 0x0FD7 },
	{ .address = 0x3C9A, .d_type.hword = 0x0810 },
	{ .address = 0x3C9E, .d_type.hword = 0x0000 },
	{ .address = 0x3C9C, .d_type.hword = 0x0000 },
	{ .address = 0x3CC2, .d_type.hword = 0x0000 },
	{ .address = 0x3CC4, .d_type.hword = 0x0000 },
	{ .address = 0x3CC6, .d_type.hword = 0x0000 },
	{ .address = 0x3CA0, .d_type.hword = 0x0000 },
	{ .address = 0x3172, .d_type.hword = 0x0601 },
////corrections_recommended-end
//Analog_Setup_Recommended-start
	{ .address = 0x3EB4, .d_type.hword = 0x676A },
	{ .address = 0x3EBC, .d_type.hword = 0x0010 },
	{ .address = 0x3ECE, .d_type.hword = 0x008B },
	{ .address = 0x3ED0, .d_type.hword = 0x0071 },
	{ .address = 0x3ED2, .d_type.hword = 0x8040 },
	{ .address = 0x3ED4, .d_type.hword = 0x6030 },
	{ .address = 0x3ED6, .d_type.hword = 0x0A06 },
	{ .address = 0x3EDC, .d_type.hword = 0x4D04 },
	{ .address = 0x30EE, .d_type.hword = 0x1140 },
	{ .address = 0x3120, .d_type.hword = 0x0001 },
	{ .address = 0x3044, .d_type.hword = 0x05E0 },
	{ .address = 0x30C0, .d_type.hword = 0x0004 },
	{ .address = 0x30C2, .d_type.hword = 0x0100 },
	{ .address = 0x316E, .d_type.hword = 0x8400 },
	{ .address = 0x3EF0, .d_type.hword = 0x3E3E },
	{ .address = 0x3EF2, .d_type.hword = 0x8082 },
	{ .address = 0x3EAE, .d_type.hword = 0x003F },
	{ .address = 0x3EC0, .d_type.hword = 0x8888 },
	{ .address = 0x3EC2, .d_type.hword = 0x008B },
	{ .address = 0x3EC4, .d_type.hword = 0x2763 },
	{ .address = 0x3EC6, .d_type.hword = 0x913D },
	{ .address = 0x3ECA, .d_type.hword = 0x90B7 },
	{ .address = 0x3EBA, .d_type.hword = 0x0C11 },
	{ .address = 0x3EBE, .d_type.hword = 0x5504 },
	{ .address = 0x3ED8, .d_type.hword = 0x9844 },
	{ .address = 0x3EDA, .d_type.hword = 0xB21D },
	{ .address = 0x3F3A, .d_type.hword = 0x0080 },
////Analog_Setup_Recommended-end
//Pixel_Timing_Recommended_10bit-start
	{ .address = 0x3D00, .d_type.hword = 0x045C },
	{ .address = 0x3D02, .d_type.hword = 0xFF62 },
	{ .address = 0x3D04, .d_type.hword = 0xCDCF },
	{ .address = 0x3D06, .d_type.hword = 0xD1D3 },
	{ .address = 0x3D08, .d_type.hword = 0x8041 },
	{ .address = 0x3D0A, .d_type.hword = 0x1C00 },
	{ .address = 0x3D0C, .d_type.hword = 0x2321 },
	{ .address = 0x3D0E, .d_type.hword = 0x083C },
	{ .address = 0x3D10, .d_type.hword = 0x625B },
	{ .address = 0x3D12, .d_type.hword = 0x3105 },
	{ .address = 0x3D14, .d_type.hword = 0x4B73 },
	{ .address = 0x3D16, .d_type.hword = 0x10F0 },
	{ .address = 0x3D18, .d_type.hword = 0x1103 },
	{ .address = 0x3D1A, .d_type.hword = 0x3108 },
	{ .address = 0x3D1C, .d_type.hword = 0x2086 },
	{ .address = 0x3D1E, .d_type.hword = 0x8173 },
	{ .address = 0x3D20, .d_type.hword = 0x8073 },
	{ .address = 0x3D22, .d_type.hword = 0x9249 },
	{ .address = 0x3D24, .d_type.hword = 0x9E43 },
	{ .address = 0x3D26, .d_type.hword = 0x8011 },
	{ .address = 0x3D28, .d_type.hword = 0x03C5 },
	{ .address = 0x3D2A, .d_type.hword = 0x608F },
	{ .address = 0x3D2C, .d_type.hword = 0x7394 },
	{ .address = 0x3D2E, .d_type.hword = 0x4785 },
	{ .address = 0x3D30, .d_type.hword = 0x6880 },
	{ .address = 0x3D32, .d_type.hword = 0x6180 },
	{ .address = 0x3D34, .d_type.hword = 0x4947 },
	{ .address = 0x3D36, .d_type.hword = 0x5968 },
	{ .address = 0x3D38, .d_type.hword = 0x8056 },
	{ .address = 0x3D3A, .d_type.hword = 0x8055 },
	{ .address = 0x3D3C, .d_type.hword = 0x8E73 },
	{ .address = 0x3D3E, .d_type.hword = 0x996A },
	{ .address = 0x3D40, .d_type.hword = 0x8028 },
	{ .address = 0x3D42, .d_type.hword = 0x4094 },
	{ .address = 0x3D44, .d_type.hword = 0x4687 },
	{ .address = 0x3D46, .d_type.hword = 0x100C },
	{ .address = 0x3D48, .d_type.hword = 0x8673 },
	{ .address = 0x3D4A, .d_type.hword = 0xAE73 },
	{ .address = 0x3D4C, .d_type.hword = 0x8273 },
	{ .address = 0x3D4E, .d_type.hword = 0x8810 },
	{ .address = 0x3D50, .d_type.hword = 0x0C88 },
	{ .address = 0x3D52, .d_type.hword = 0x4698 },
	{ .address = 0x3D54, .d_type.hword = 0x7397 },
	{ .address = 0x3D56, .d_type.hword = 0x5580 },
	{ .address = 0x3D58, .d_type.hword = 0x2842 },
	{ .address = 0x3D5A, .d_type.hword = 0x496A },
	{ .address = 0x3D5C, .d_type.hword = 0x6B82 },
	{ .address = 0x3D5E, .d_type.hword = 0x736B },
	{ .address = 0x3D60, .d_type.hword = 0x826D },
	{ .address = 0x3D62, .d_type.hword = 0xA17B },
	{ .address = 0x3D64, .d_type.hword = 0x9E47 },
	{ .address = 0x3D66, .d_type.hword = 0x8568 },
	{ .address = 0x3D68, .d_type.hword = 0x8249 },
	{ .address = 0x3D6A, .d_type.hword = 0x4759 },
	{ .address = 0x3D6C, .d_type.hword = 0x6880 },
	{ .address = 0x3D6E, .d_type.hword = 0x1A00 },
	{ .address = 0x3D70, .d_type.hword = 0x5885 },
	{ .address = 0x3D72, .d_type.hword = 0x200C },
	{ .address = 0x3D74, .d_type.hword = 0x8210 },
	{ .address = 0x3D76, .d_type.hword = 0xF082 },
	{ .address = 0x3D78, .d_type.hword = 0x4593 },
	{ .address = 0x3D7A, .d_type.hword = 0x6A28 },
	{ .address = 0x3D7C, .d_type.hword = 0x4081 },
	{ .address = 0x3D7E, .d_type.hword = 0x73E9 },
	{ .address = 0x3D80, .d_type.hword = 0x7373 },
	{ .address = 0x3D82, .d_type.hword = 0x8273 },
	{ .address = 0x3D84, .d_type.hword = 0xFFFF },
	{ .address = 0x3D86, .d_type.hword = 0xFFD9 },
	{ .address = 0x3D88, .d_type.hword = 0x4184 },
	{ .address = 0x3D8A, .d_type.hword = 0x004A },
	{ .address = 0x3D8C, .d_type.hword = 0x9210 },
	{ .address = 0x3D8E, .d_type.hword = 0x3FA0 },
	{ .address = 0x3D90, .d_type.hword = 0x4A89 },
	{ .address = 0x3D92, .d_type.hword = 0x4A87 },
	{ .address = 0x3D94, .d_type.hword = 0x679A },
	{ .address = 0x3D96, .d_type.hword = 0x103F },
	{ .address = 0x3D98, .d_type.hword = 0x8D41 },
	{ .address = 0x3D9A, .d_type.hword = 0x8441 },
	{ .address = 0x3D9C, .d_type.hword = 0x8B10 },
	{ .address = 0x3D9E, .d_type.hword = 0x0F82 },
	{ .address = 0x3DA0, .d_type.hword = 0x1030 },
	{ .address = 0x3DA2, .d_type.hword = 0xA14A },
	{ .address = 0x3DA4, .d_type.hword = 0x894A },
	{ .address = 0x3DA6, .d_type.hword = 0xA410 },
	{ .address = 0x3DA8, .d_type.hword = 0x3F8E },
	{ .address = 0x3DAA, .d_type.hword = 0x005E },
	{ .address = 0x3DAC, .d_type.hword = 0x7B86 },
	{ .address = 0x3DAE, .d_type.hword = 0x5528 },
	{ .address = 0x3DB0, .d_type.hword = 0x4034 },
	{ .address = 0x3DB2, .d_type.hword = 0x4524 },
	{ .address = 0x3DB4, .d_type.hword = 0xA036 },
	{ .address = 0x3DB6, .d_type.hword = 0x0070 },
	{ .address = 0x3DB8, .d_type.hword = 0x8000 },
	{ .address = 0x3DBA, .d_type.hword = 0x0382 },
	{ .address = 0x3DBC, .d_type.hword = 0x4BC3 },
	{ .address = 0x3DBE, .d_type.hword = 0x4B82 },
	{ .address = 0x3DC0, .d_type.hword = 0x0003 },
	{ .address = 0x3DC2, .d_type.hword = 0x8070 },
	{ .address = 0x3DC4, .d_type.hword = 0x8001 },
	{ .address = 0x3DC6, .d_type.hword = 0x221C },
	{ .address = 0x3DC8, .d_type.hword = 0x0020 },
	{ .address = 0x3DCA, .d_type.hword = 0x2842 },
	{ .address = 0x3DCC, .d_type.hword = 0x6220 },
	{ .address = 0x3DCE, .d_type.hword = 0x0530 },
	{ .address = 0x3DD0, .d_type.hword = 0x0543 },
	{ .address = 0x3DD2, .d_type.hword = 0x8010 },
	{ .address = 0x3DD4, .d_type.hword = 0xC010 },
	{ .address = 0x3DD6, .d_type.hword = 0x0367 },
	{ .address = 0x3DD8, .d_type.hword = 0x5F80 },
	{ .address = 0x3DDA, .d_type.hword = 0x4482 },
	{ .address = 0x3DDC, .d_type.hword = 0x6C5E },
	{ .address = 0x3DDE, .d_type.hword = 0x8110 },
	{ .address = 0x3DE0, .d_type.hword = 0x308F },
	{ .address = 0x3DE2, .d_type.hword = 0x1003 },
	{ .address = 0x3DE4, .d_type.hword = 0x9210 },
	{ .address = 0x3DE6, .d_type.hword = 0x3084 },
	{ .address = 0x3DE8, .d_type.hword = 0x2300 },
	{ .address = 0x3DEA, .d_type.hword = 0x8049 },
	{ .address = 0x3DEC, .d_type.hword = 0x8059 },
	{ .address = 0x3DEE, .d_type.hword = 0x4483 },
	{ .address = 0x3DF0, .d_type.hword = 0x43A3 },
	{ .address = 0x3DF2, .d_type.hword = 0x60A0 },
	{ .address = 0x3DF4, .d_type.hword = 0x6189 },
	{ .address = 0x3DF6, .d_type.hword = 0x4782 },
	{ .address = 0x3DF8, .d_type.hword = 0x6C8C },
	{ .address = 0x3DFA, .d_type.hword = 0x5980 },
	{ .address = 0x3DFC, .d_type.hword = 0x0280 },
	{ .address = 0x3DFE, .d_type.hword = 0x1600 },
	{ .address = 0x3E00, .d_type.hword = 0xA16A },
	{ .address = 0x3E02, .d_type.hword = 0x5E84 },
	{ .address = 0x3E04, .d_type.hword = 0x63E8 },
	{ .address = 0x3E06, .d_type.hword = 0x4B88 },
	{ .address = 0x3E08, .d_type.hword = 0x4BAB },
	{ .address = 0x3E0A, .d_type.hword = 0x4686 },
	{ .address = 0x3E0C, .d_type.hword = 0x100C },
	{ .address = 0x3E0E, .d_type.hword = 0xA610 },
	{ .address = 0x3E10, .d_type.hword = 0x0C88 },
	{ .address = 0x3E12, .d_type.hword = 0x468E },
	{ .address = 0x3E14, .d_type.hword = 0x7B82 },
	{ .address = 0x3E16, .d_type.hword = 0x4963 },
	{ .address = 0x3E18, .d_type.hword = 0x5955 },
	{ .address = 0x3E1A, .d_type.hword = 0x806A },
	{ .address = 0x3E1C, .d_type.hword = 0x5EAD },
	{ .address = 0x3E1E, .d_type.hword = 0x6B80 },
	{ .address = 0x3E20, .d_type.hword = 0x4781 },
	{ .address = 0x3E22, .d_type.hword = 0x6B8C },
	{ .address = 0x3E24, .d_type.hword = 0x5958 },
	{ .address = 0x3E26, .d_type.hword = 0x5702 },
	{ .address = 0x3E28, .d_type.hword = 0x809A },
	{ .address = 0x3E2A, .d_type.hword = 0x6D81 },
	{ .address = 0x3E2C, .d_type.hword = 0x5582 },
	{ .address = 0x3E2E, .d_type.hword = 0x10C0 },
	{ .address = 0x3E30, .d_type.hword = 0x8C45 },
	{ .address = 0x3E32, .d_type.hword = 0x896A },
	{ .address = 0x3E34, .d_type.hword = 0x835E },
	{ .address = 0x3E36, .d_type.hword = 0x8263 },
	{ .address = 0x3E38, .d_type.hword = 0xF667 },
	{ .address = 0x3E3A, .d_type.hword = 0xEC30 },
	{ .address = 0x3E3C, .d_type.hword = 0x0580 },
	{ .address = 0x3E3E, .d_type.hword = 0x5FFA },
	{ .address = 0x3E40, .d_type.hword = 0x5A80 },
	{ .address = 0x3E42, .d_type.hword = 0x5BFF },
	{ .address = 0x3E44, .d_type.hword = 0xFF95 },
	{ .address = 0x3E46, .d_type.hword = 0x438E },
	{ .address = 0x3E48, .d_type.hword = 0x4383 },
	{ .address = 0x3E4A, .d_type.hword = 0x4286 },
	{ .address = 0x3E4C, .d_type.hword = 0x0102 },
	{ .address = 0x3E4E, .d_type.hword = 0x8100 },
	{ .address = 0x3E50, .d_type.hword = 0x1E82 },
	{ .address = 0x3E52, .d_type.hword = 0x4688 },
	{ .address = 0x3E54, .d_type.hword = 0x4A96 },
	{ .address = 0x3E56, .d_type.hword = 0x4A88 },
	{ .address = 0x3E58, .d_type.hword = 0x4686 },
	{ .address = 0x3E5A, .d_type.hword = 0x000C },
	{ .address = 0x3E5C, .d_type.hword = 0x4181 },
	{ .address = 0x3E5E, .d_type.hword = 0x4180 },
	{ .address = 0x3E60, .d_type.hword = 0x000C },
	{ .address = 0x3E62, .d_type.hword = 0x8B46 },
	{ .address = 0x3E64, .d_type.hword = 0x4A94 },
	{ .address = 0x3E66, .d_type.hword = 0x4A8E },
	{ .address = 0x3E68, .d_type.hword = 0x7B80 },
	{ .address = 0x3E6A, .d_type.hword = 0x004C },
	{ .address = 0x3E6C, .d_type.hword = 0x4181 },
	{ .address = 0x3E6E, .d_type.hword = 0x4100 },
	{ .address = 0x3E70, .d_type.hword = 0x0C89 },
	{ .address = 0x3E72, .d_type.hword = 0x4682 },
	{ .address = 0x3E74, .d_type.hword = 0x4A95 },
	{ .address = 0x3E76, .d_type.hword = 0x4A87 },
	{ .address = 0x3E78, .d_type.hword = 0x4686 },
	{ .address = 0x3E7A, .d_type.hword = 0x000C },
	{ .address = 0x3E7C, .d_type.hword = 0x4181 },
	{ .address = 0x3E7E, .d_type.hword = 0x4180 },
	{ .address = 0x3E80, .d_type.hword = 0x000C },
	{ .address = 0x3E82, .d_type.hword = 0x8246 },
	{ .address = 0x3E84, .d_type.hword = 0x884A },
	{ .address = 0x3E86, .d_type.hword = 0x984A },
	{ .address = 0x3E88, .d_type.hword = 0x8446 },
	{ .address = 0x3E8A, .d_type.hword = 0x4400 },
	{ .address = 0x3E8C, .d_type.hword = 0x0C41 },
	{ .address = 0x3E8E, .d_type.hword = 0x8663 },
	{ .address = 0x3E90, .d_type.hword = 0x8134 },
	{ .address = 0x3E92, .d_type.hword = 0x4055 },
	{ .address = 0x3E94, .d_type.hword = 0x2420 },
	{ .address = 0x3E96, .d_type.hword = 0x8036 },
	{ .address = 0x3E98, .d_type.hword = 0x0070 },
	{ .address = 0x3E9A, .d_type.hword = 0x046A },
	{ .address = 0x3E9C, .d_type.hword = 0xFF70 },
	{ .address = 0x3E9E, .d_type.hword = 0x046A },
	{ .address = 0x3EA0, .d_type.hword = 0xFF70 },
	{ .address = 0x3EA2, .d_type.hword = 0x046A },
	{ .address = 0x3EA4, .d_type.hword = 0xFF70 },
	{ .address = 0x3EA6, .d_type.hword = 0x046A },
	{ .address = 0x3EA8, .d_type.hword = 0x0000 },
	{ .address = 0x3EAA, .d_type.hword = 0x0000 },
	{ .address = 0x3EAC, .d_type.hword = 0x0000 },
////Pixel_Timing_Recommended_10bit-end
//PLL_876_150_24MHz_12bit-start
	{ .address = 0x0300, .d_type.hword = 0x0006 }, //VT_PIX_CLK_DIV=6
	{ .address = 0x0302, .d_type.hword = 0x0C01 }, //VT_SYS_CLK_DIV2=12 //VT_SYS_CLK_DIV=1
	{ .address = 0x0304, .d_type.hword = 0x0C02 }, //PRE_PLL_CLK_DIV2=12// PRE_PLL_CLK_DIV1=2
	{ .address = 0x0306, .d_type.hword = 0x284B }, //PLL_MULTIPLIER2=40// PLL_MULTIPLIER1=75
	{ .address = 0x0308, .d_type.hword = 0x0005 }, //OP_PIX_CLK_DIV=5
	{ .address = 0x030A, .d_type.hword = 0x0001 }, //OP_SYS_CLK_DIV=1
	{ .address = 0x030C, .d_type.hword = 0x0089 }, //MIPI_INT_PHY_PLL=0x0089
	{ .address = 0x030E, .d_type.hword = 0x0001 }, //PLL_CTRL=1
////PLL_876_150_24MHz_12bit-end
//MIPI_Timing_876Mbps_24MHz_12bit-start
	{ .address = 0x31B0, .d_type.hword = 0x003B }, //Frame preamble
	{ .address = 0x31B2, .d_type.hword = 0x003B }, //Line preamble 
	{ .address = 0x31B4, .d_type.hword = 0x130A }, //MIPI timing0
	{ .address = 0x31B6, .d_type.hword = 0x16CA }, //MIPI timing1
	{ .address = 0x31B8, .d_type.hword = 0x1251 }, //MIPI timing2
	{ .address = 0x31BA, .d_type.hword = 0xB808 }, //MIPI timing3
	{ .address = 0x31BC, .d_type.hword = 0x4D3D }, //MIPI timing4
////MIPI_Timing_876Mbps_24MHz_12bit-end
//Stream_4MP_2316x1746_30fps_12bit_876-start
	{ .address = 0x3172, .d_type.hword = 0x4601 }, //Analog_control2
	{ .address = 0x0344, .d_type.hword = 0x0020 }, //x_addr_start, 32
	{ .address = 0x0346, .d_type.hword = 0x0010 }, //y_addr_start, 16
	{ .address = 0x0348, .d_type.hword = 0x1220 }, //x_addr_end, 4640
	{ .address = 0x034A, .d_type.hword = 0x0DA8 }, //y_addr_end, 3496
	{ .address = 0x034C, .d_type.hword = 0x0900 }, //x_output_size, 2304
	{ .address = 0x034E, .d_type.hword = 0x06CC }, //y_output_size, 1740
	{ .address = 0x3040, .d_type.hword = 0x00C3 }, //read_mode
	{ .address = 0x0340, .d_type.hword = FRAME_LENGTH_LINES_12BIT }, //FRAME_LENGTH_LINES, 3546
	{ .address = 0x0342, .d_type.hword = LINE_LENGTH_PCK_12BIT }, //LINE_LENGTH_PCK, 5640
	{ .address = 0x0112, .d_type.hword = 0x0C0C }, //ccp_data_format
////Stream_4MP_2316x1746_30fps_12bit_876-end
	{ .address = D_TBL_END, .d_type.hword = 0xFFFF }		// END
};

const T_CAMERA_SENSOR_BASIC		AR0430_BASIC =
{
#ifdef MCLK_SLOW_DOWN
	.input_clock	= MCLK_SLOW_DOWN,
#else
	.input_clock	= E_CAMERA_SENSOR_INCLK_24MHz,
#endif
	.access			= E_CAMERA_ACCESS_I2C,
	.sensor_if		= E_CAMERA_SENSOR_IF_DMIPI,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_VD, //E_CAMERA_SENSOR_UPDATE_TIMING_V_BLANKING
};

const T_CAMERA_SENSOR_MODE		AR0430_MODE[E_SENSOR_AR0430_MODE_MAX] =
{
	[E_SENSOR_AR0430_MODE_FUll_2316x1746_RAW10_30FPS] = {
		.mode_reg				= (T_CAMERA_SENSOR_REG *)AR0430_MODE_FULL_2316x1746_RAW10_30FPS,
		
		.embedded_lines 		= 0,
		.total_pix_width		= 2304,
		.total_pix_lines		= 1740,
		//.total_pix_width		= 1152,
		//.total_pix_lines		= 868,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 0, //inside IPS PIPE cap
		.active_pix_y_pos		= 0,
		//.active_pix_x_pos		= 1152, //inside IPS PIPE cap
		//.active_pix_y_pos		= 872,
		.active_pix_width		= 2304, //Magnification = 4
		.active_pix_lines		= 1740,
		//.active_pix_lines		= 1728,
		.active_pix_x_rsz_ratio = 1.0,
		.active_pix_y_rsz_ratio = 1.0,
		.hdocyc 				= LINE_LENGTH_PCK_10BIT, //HTS: total horizontal timing size=LINE_LENGTH_PCK
	#if defined(TEST_440MBPS_MODE) || defined(TEST_PATTERN_MODE) //increase the exposure time.
		.vdocyc 				= (FRAME_LENGTH_LINES_10BIT<<1), //VTS: total vertical timing size=FRAME_LENGTH_LINES
	#elif defined TEST_220MBPS_MODE
		.vdocyc 				= (FRAME_LENGTH_LINES_10BIT<<2), //VTS: total vertical timing size=FRAME_LENGTH_LINES
	#else
		.vdocyc 				= FRAME_LENGTH_LINES_10BIT, //VTS: total vertical timing size=FRAME_LENGTH_LINES
	#endif
		.exp_start_line_min 	= 8, //min short exposure time
		.exp_end_line_offset	= 52815, //max long exposure time, lock the minimum FPS = 2. 600 x 10^6 / 2 /hdocyc
		.exp_clk_offset 		= 1, //coarse_intefration_time_max_margin
		.input_bit_width		= 10,
		.first_active_pix_color = FJ_PRO_CTRL_FIRST_PIX_COLOR_GR,
		.base_SV				= 4.0,
	#if defined(TEST_440MBPS_MODE) || defined(TEST_PATTERN_MODE)
		.fps					= 15.0, // FPS = (PCLK * 4 * 10^6) / (HTS * VTS) DG p.7
		.Mpps_HDclk 			= 150, // clk_pixel = ExtCLK / pre_pll_clk_div1 * pll_multiplier1 / vt_pix_clk_div DG p.6
		.one_lane_Mbps          = 440,
	#elif defined TEST_220MBPS_MODE
		.fps					= 7.5, // FPS = (PCLK * 4 * 10^6) / (HTS * VTS) DG p.7
		.Mpps_HDclk 			= 150, // clk_pixel = ExtCLK / pre_pll_clk_div1 * pll_multiplier1 / vt_pix_clk_div DG p.6
		.one_lane_Mbps          = 220,
	#else
		.fps					= 30.0, // FPS = (PCLK * 4 * 10^6) / (HTS * VTS) DG p.7
		.Mpps_HDclk 			= 150, // clk_pixel = ExtCLK / pre_pll_clk_div1 * pll_multiplier1 / vt_pix_clk_div DG p.6
		.one_lane_Mbps          = 880,
	#endif
		.lane_num				= 4,
	},
	[E_SENSOR_AR0430_MODE_FUll_2316x1746_RAW12_30FPS] = {
		.mode_reg				= (T_CAMERA_SENSOR_REG *)AR0430_MODE_FULL_2316x1746_RAW12_30FPS,

		.embedded_lines 		= 0,
		.total_pix_width		= 2304,
		.total_pix_lines		= 1740,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 0, //inside IPS PIPE cap
		.active_pix_y_pos		= 0,
		.active_pix_width		= 2304, //Magnification = 4
		.active_pix_lines		= 1740,
		.active_pix_x_rsz_ratio = 1.0,
		.active_pix_y_rsz_ratio = 1.0,
		
		.hdocyc 				= LINE_LENGTH_PCK_12BIT, // HTS: total horizontal timing size=LINE_LENGTH_PCK
		.vdocyc 				= FRAME_LENGTH_LINES_12BIT, // VTS: total vertical timing size=FRAME_LENGTH_LINES
		.exp_start_line_min 	= 8, //min short exposure time
		.exp_end_line_offset	= 52815, //max long exposure time, the minimum FPS = 2. 600 x 10^6 / 2 /hdocyc
		.exp_clk_offset 		= 1, //coarse_intefration_time_max_margin
		.input_bit_width		= 12,
		.first_active_pix_color = FJ_PRO_CTRL_FIRST_PIX_COLOR_GR,
		.base_SV				= 4.0,
		
		.fps					= 30.0, // FPS = (PCLK * 4 * 10^6) / (HTS * VTS) DG p.7
		.Mpps_HDclk 			= 150, // clk_pixel = ExtCLK / pre_pll_clk_div1 * pll_multiplier1 / vt_pix_clk_div DG p.6
		.one_lane_Mbps          = 876,
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
E_CAMERA_ERR_CODE Sensor_AR0430_Init(E_CAMERA_ID sensor_id)
{
	BF_Debug_Print_Information(("%s(%d):sensor_id=%d \n", __func__, __LINE__, sensor_id));

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_AR0430_Clock_Gpio_Init(VOID)
{
	INT32 sensor_id;
	
	// Clock‘S
	switch( CAMERA.sensor[0].basic->input_clock )
	{
#ifdef MCLK_SLOW_DOWN
	case MCLK_SLOW_DOWN: //for test only
		Dd_Top_Set_PLLCNT3_P00PREDIV(1);	// 40MHz / (val+1) = 40M / (1+1) = 20M
		Dd_Top_Set_PLLCNT4_P00FNUM(0);		//
		Dd_Top_Set_PLLCNT5_P00FDEN(0);		//
		Dd_Top_Set_PLLCNT3_P00PLLDIV(30);	// * val = 20 x 30 = 600M
		Dd_Top_Set_PLLCNT3_P00POSTDIV(49);	// / (val+1) = 600M / (49+1) = 12M
		Dd_Top_Start_Pll00();				// PLL00 Start
		break;
#endif
	case E_CAMERA_SENSOR_INCLK_24MHz:
		Dd_Top_Set_PLLCNT3_P00PREDIV(1);	// 40MHz / (val+1) = 40M / (1+1) = 20M
		Dd_Top_Set_PLLCNT4_P00FNUM(0);		//
		Dd_Top_Set_PLLCNT5_P00FDEN(0);		//
		Dd_Top_Set_PLLCNT3_P00PLLDIV(60);	// * val = 20 x 60 = 1200M
		Dd_Top_Set_PLLCNT3_P00POSTDIV(49);	// / (val+1) = 1200M / (49+1) = 24M
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
		break;
	}
	
	// Sensor Initialization‚Sen_Unit APCK
	for( sensor_id = 0; sensor_id < CAMERA.sensor_num; sensor_id++)
	{
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


		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_inck, 1 );	// CLK to Sensor
		//reset pin
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xclr, 0 );	// use as GPIO
		Dd_Top_Set_Gpio_Direction( CAMERA.sensor[sensor_id].port_xclr, 1 );	// output
		Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 0 );	// XCLR to low
		OS_User_Dly_Tsk(10); //DS p.23. AR0430_Socionext.ini
		Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 1 ); // XCLR low -> high
		OS_User_Dly_Tsk(10); //DS p.23. AR0430_Socionext.ini
		
		// XVS XHS
		if(E_DD_TOP_GPIO_PORT_MAX != CAMERA.sensor[sensor_id].port_xhs &&
			E_DD_TOP_GPIO_PORT_MAX != CAMERA.sensor[sensor_id].port_xvs) {
			Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xhs, 1 );	// XHS from Sensor
			Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xvs, 1 );	// XVS from Sensor
		}
		else {
			UPRINTF("%s(%d):MIPI without XVS & XHS\n", __func__, __LINE__);
		}
		
		// init setting
		if(CAMERA.sensor[sensor_id].Func_Init) {
			CAMERA.sensor[sensor_id].Func_Init(sensor_id);
		}
	}

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_AR0430_Standby(E_CAMERA_ID sensor_id)
{
	USHORT data = 0x0018;
	UCHAR reg_buf[2] = {'\0'};

	//Disable stream
	reg_buf[0] = (UCHAR)(data >> 8);
	reg_buf[1] = (UCHAR)(data & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
		CAMERA.sensor[sensor_id].i2c_slave7bit, 0x301A, &reg_buf[0], 2);
	UPRINTF("%s(%d):0x301A, reg_buf=0x%02X%02X\n", __func__, __LINE__, reg_buf[0], reg_buf[1]);
	OS_User_Dly_Tsk(1); //1ms

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_AR0430_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol)
{
#ifdef TEST_PATTERN_MODE
	UPRINTF("%s(%d):mode=%d - TestPatternMode_4MP_2316x1746_15fps_10bit_440\n", __func__, __LINE__, mode);
#elif defined TEST_440MBPS_MODE
	UPRINTF("%s(%d):mode=%d - Stream_4MP_2316x1746_15fps_10bit_440\n", __func__, __LINE__, mode);
#elif defined TEST_220MBPS_MODE
	UPRINTF("%s(%d):mode=%d - Stream_4MP_2316x1746_7.5fps_10bit_220\n", __func__, __LINE__, mode);
#else
	UPRINTF("%s(%d):mode=%d\n", __func__, __LINE__, mode);
#endif
	{
		UCHAR reg_buf[2] = {'\0'};
		
		//read CUSTOMER_REV
		Camera_Common_I2C_Read(CAMERA.sensor[sensor_id].i2c_ch,
			CAMERA.sensor[sensor_id].i2c_slave7bit, 0x31FE, &reg_buf[0], 2);
		UPRINTF("Read Customer verson:0x%02X%02X\n", reg_buf[0], reg_buf[1]); //0x0092
		
		//Read ChIP version
		Camera_Common_I2C_Read(CAMERA.sensor[sensor_id].i2c_ch,
			CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3000, &reg_buf[0], 2);
		UPRINTF("Read CHIP verson:0x%02X%02X\n", reg_buf[0], reg_buf[1]); //0x1550

		//XSHUTDOWN(hw reset test)
		Dd_Top_Set_Gpio_Status(CAMERA.sensor[sensor_id].port_xclr, 0);	// XCLR to low
		OS_User_Dly_Tsk(5); //DS p.23
		Dd_Top_Set_Gpio_Status(CAMERA.sensor[sensor_id].port_xclr, 1); // XCLR low -> high
		OS_User_Dly_Tsk(10);
		// Set LP_CTX_CTRL(LOW_POWER_CTX) register
		reg_buf[1] = reg_buf[0] = 0x00;
		Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
			CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3340, &reg_buf[0], 2);

		// Set Reset register, 0x0001;
		reg_buf[0] = 0x00; reg_buf[1] = 0x01;
		Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
			CAMERA.sensor[sensor_id].i2c_slave7bit, 0x0103, &reg_buf[0], 2);
		UPRINTF("0x0103, Reset_buf=0x%02X%02X\n", reg_buf[0], reg_buf[1]);
		OS_User_Dly_Tsk(5); //5ms
	}
	access_tbl(sensor_id, (T_CAMERA_SENSOR_REG1 *)AR0430_MODE[mode].mode_reg);
#ifdef SET_VIRTUAL_CHANNEL
	{ //set virtual channel, 0x2B2B
		UCHAR reg_buf[2];

		reg_buf[0] = 0x2B; reg_buf[1] = 0x2B;
		Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
			CAMERA.sensor[sensor_id].i2c_slave7bit, 0x31D6, &reg_buf[0], 2);
		UPRINTF("0x31D6(%d), VC_buf=0x%02X%02X\n", __LINE__, reg_buf[0], reg_buf[1]);
	}
#endif
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_AR0430_Stream(E_CAMERA_ID sensor_id)
{
#ifdef TEST_PATTERN_MODE
	USHORT data = 0x0014;
#else
	USHORT data = 0x001C;
#endif
	UCHAR reg_buf[2] = {'\0'};

	//Enable Stream
	reg_buf[0] = (UCHAR)(data >> 8);
	reg_buf[1] = (UCHAR)(data & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
		CAMERA.sensor[sensor_id].i2c_slave7bit, 0x301A, &reg_buf[0], 2);
	UPRINTF("%s(%d):0x301A, reg_buf=0x%02X%02X\n", __func__, __LINE__, reg_buf[0], reg_buf[1]);
	OS_User_Dly_Tsk(1); //1ms

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_AR0430_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
{
	UINT32 c_i_t; // sensor Coarse Integration Time, for each line exposure time
	UINT32 f_l_l; // sensor Frame Length Line

	// integration_time(second) * pix_clk(Mhz) * 4 * 10^6 / Line_lenght_pck(HTS). only 10^3, because the "in->exp_msec" unit=msec. DG p.8
	c_i_t = (UINT32)(((in->exp_msec * AR0430_MODE[mode].Mpps_HDclk * 4 * pow(10, 3)) / AR0430_MODE[mode].hdocyc) + 1); //+1 for Unconditional carry
	if(c_i_t < AR0430_MODE[mode].exp_start_line_min) { //confirm the short exposure time first
		c_i_t = AR0430_MODE[mode].exp_start_line_min;
	} //AE maximum "in->exp_msec" time = 100.001282ms, and AE will lock.

	//not Clip
	if(in->max_fps == D_CAMERA_FPS_AUTO && in->min_fps == D_CAMERA_FPS_AUTO)
	{
		const UINT32 exp_line_max = AR0430_MODE[mode].vdocyc - AR0430_MODE[mode].exp_clk_offset;

		//UPRINTF("%s(%d):===>\n", __func__, __LINE__);
		if(c_i_t > exp_line_max) {
			c_i_t = exp_line_max;
		}
		// Frame Length Line(VTS)
		out->vdocyc = f_l_l = AR0430_MODE[mode].vdocyc;
	}
	else // Clip by FPS
	{
		UINT32 clip_exp_line_max;
		FLOAT req_fps;
		FLOAT min_fps = in->min_fps;
		FLOAT max_fps = in->max_fps;
		const FLOAT limit_fps = 7.5;
		const UINT32 exp_line_max = AR0430_MODE[mode].exp_end_line_offset - AR0430_MODE[mode].exp_clk_offset;
		
		//UPRINTF("%s(%d):--->\n", __func__, __LINE__);
		//check min_fps, 
		if(min_fps < limit_fps) {
			min_fps = limit_fps;
		}
		else if(min_fps > AR0430_MODE[mode].fps) {
			min_fps = AR0430_MODE[mode].fps;
		}
 
		//check max_fps, lock the minimum FPS = 2
		if(max_fps < limit_fps) {
			max_fps = limit_fps;
		}
		else if(max_fps > AR0430_MODE[mode].fps) {
			max_fps = AR0430_MODE[mode].fps;
		}

		//check min_fps vs max_fps
		if(max_fps <= min_fps) {
			req_fps = min_fps;
		}
		else if(max_fps == min_fps + 1) {
			req_fps = max_fps;
		}
		else {
			req_fps = (max_fps - min_fps) / 2 + min_fps;
		}

		//Set the VTS(f_l_l), FPS = (PCLK * 4 * 10^6) / (HTS(Line_lenght_pck) * VTS(Frame Length Line))
		f_l_l = (AR0430_MODE[mode].Mpps_HDclk * 4 * pow(10, 6)) / (AR0430_MODE[mode].hdocyc * req_fps);
		if(f_l_l > AR0430_MODE[mode].exp_end_line_offset) {
			f_l_l = AR0430_MODE[mode].exp_end_line_offset;
		}
		// Frame Length Line(VTS)
		out->vdocyc = f_l_l;

		//clip the exposure line fixed on the correct line and map the right FPS also
		clip_exp_line_max = f_l_l - AR0430_MODE[mode].exp_clk_offset;
		if(clip_exp_line_max > exp_line_max) {
			clip_exp_line_max = exp_line_max;
		}

		if(c_i_t > clip_exp_line_max) {
			c_i_t = clip_exp_line_max;
		}
	}

	////Gain calculations start, 
	Sensor_AR0430_Calc_Gain(in, out);
	//Gain calculations end

	// Revert
	out->exp_msec = (FLOAT)c_i_t * AR0430_MODE[mode].hdocyc / (AR0430_MODE[mode].Mpps_HDclk * 4 * pow(10, 3)); //only 10^3, because the "out->exp_msec" unit=msec.
	out->hdocyc = AR0430_MODE[mode].hdocyc;
	out->fps = (AR0430_MODE[mode].Mpps_HDclk * 4 * pow(10, 6)) / AR0430_MODE[mode].hdocyc / f_l_l;
	#if 0
		if(in->gain_times) {
			UPRINTF("\nIn:\r\n\texp_msec=%f, gain=%f, f_l_l=%d\n"
				"Out:\r\n\texp_msec=%f, c_i_t=%d, Again=%f, Dgain=%f, fps=%f\r\n\t"
				"Global_Coarse=%u, global_fine=%u, Digital=%u, conversion=%u\n",
				in->exp_msec, in->gain_times, f_l_l,
				out->exp_msec, c_i_t, out->again_times, out->dgain_times, out->fps,
				G_gain_value.Global_Coarse_A, G_gain_value.global_fine_A, G_gain_value.Global_Digital, G_gain_value.conversion);
		}
	#endif
	// Reg
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] = c_i_t;
	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = AR0430_MODE[mode].hdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = f_l_l;
	out_reg->reg[E_CAMERA_SENSOR_REG_AGAIN] = out->again_times;
	out_reg->reg[E_CAMERA_SENSOR_REG_DGAIN] = out->dgain_times;

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_AR0430_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in)
{
	UCHAR reg_buf[2] = {'\0'};

	// EXPO
	reg_buf[0] = (UCHAR)(in->reg[E_CAMERA_SENSOR_REG_EXP] >> 8);
	reg_buf[1] = (UCHAR)(in->reg[E_CAMERA_SENSOR_REG_EXP] & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
		CAMERA.sensor[sensor_id].i2c_slave7bit, 0x0202, &reg_buf[0], 2);
	//UPRINTF("EXPO_buf=0x%02X%02X\n", reg_buf[0], reg_buf[1]);
	
	// VTS
	reg_buf[0] = (UCHAR)(in->reg[E_CAMERA_SENSOR_REG_VDOCYC] >> 8);
	reg_buf[1] = (UCHAR)(in->reg[E_CAMERA_SENSOR_REG_VDOCYC] & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
		CAMERA.sensor[sensor_id].i2c_slave7bit, 0x0340, &reg_buf[0], 2);
	//UPRINTF("VTS_buf=0x%02X%02X\n", reg_buf[0], reg_buf[1]);

	// Conversion Gain
	reg_buf[0] = (UCHAR)((G_gain_value.conversion << 4) & 0x10) | 0x41;
	reg_buf[1] = 0x6E;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
		CAMERA.sensor[sensor_id].i2c_slave7bit, 0x317A, &reg_buf[0], 2);
	//UPRINTF("Conversion_buf=0x%02X%02X\n", reg_buf[0], reg_buf[1]);

	// Global Gain
	reg_buf[0] = (UCHAR)((G_gain_value.Global_Digital >> 1) & 0xFF);
	reg_buf[1] = (UCHAR)((G_gain_value.Global_Digital << 7) & 0x80) |
			((G_gain_value.Global_Coarse_A << 4) & 0x30) |
			(G_gain_value.global_fine_A & 0x0F);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
		CAMERA.sensor[sensor_id].i2c_slave7bit, 0x305E, &reg_buf[0], 2);
	//UPRINTF("Global_GAIN=0x%02X%02X\n", reg_buf[0], reg_buf[1]);

	return E_CAMERA_ERR_OK;
}
/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static E_CAMERA_ERR_CODE access_tbl(E_CAMERA_ID sensor_id, const T_CAMERA_SENSOR_REG1* tbl)
{
	INT32 cnt = 0;
	UCHAR reg_buf[2] = {'\0'};

	UPRINTF("%s(%d)\n", __func__, __LINE__);
	while(1)
	{
		// end check
		if( D_TBL_END == tbl[cnt].address ) {
			break;
		}
		
		// access
		if( D_TBL_WAIT_MS == tbl[cnt].address ){
			OS_User_Dly_Tsk( tbl[cnt].d_type.byte[0] );
		}
		else {
			// address is MSB first
			//reg_buf[0] = (UCHAR)(tbl[cnt].d_type.hword >> 8); //MSB
			//reg_buf[1] = (UCHAR)(tbl[cnt].d_type.hword & 0xFF); //LSB
			reg_buf[0] = tbl[cnt].d_type.byte[1];
			reg_buf[1] = tbl[cnt].d_type.byte[0];
			Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
									CAMERA.sensor[sensor_id].i2c_slave7bit,
									tbl[cnt].address, &reg_buf[0], 2);
		}
		//UPRINTF("%d)[address=0x%04X]:reg_buf=0x%02X%02X\n", cnt, tbl[cnt].address, reg_buf[0], reg_buf[1]);
		cnt++;
	}
	
	return E_CAMERA_ERR_OK;
}

const T_SENSOR_AR0430_GAIN_INFO		AR0430_gain_info =
{
	.A_level1_step		=0.0625,
	.A_level2_step		=0.125,
	.A_level3_step	 	=0.25,
	.A_level4_step	 	=0.3825,
	.D_level_step		=0.18527,
	.tolerance 			=0.59,
	.A_gain_range		=31,
	.D_gain_range		=0x1FF,
};
void Sensor_AR0430_Calc_Gain(const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out)
{
	if(in->gain_times > 94.675) { //The a_gains and d_gains fomula is from DG p.7
		out->again_times = 11.8575;
		out->dgain_times = 7.984375;
		G_gain_value.conversion = 0; //0x317A, x 1.53
		G_gain_value.Global_Coarse_A = 2; //0x305E[5:4], x 4
		G_gain_value.global_fine_A = 15; //0x305E[3:0], x 1.9375
		G_gain_value.Global_Digital = 0x1FF; //0x305E[15:7], x 7.984375  //MAXgain
	}
	else if(in->gain_times > 11.8575 && in->gain_times <= 94.675) { //Dgain_level
		USHORT root;
		FLOAT sen_gain_setting = in->gain_times / AR0430_gain_info.D_level_step;

		if((root = sen_gain_setting + AR0430_gain_info.tolerance) > AR0430_gain_info.D_gain_range) {
			root = AR0430_gain_info.D_gain_range;
		}
		out->again_times = 11.8575;
		out->dgain_times = (FLOAT)root / 64;
		if(root != AR0430_gain_info.D_gain_range && in->gain_times && (out->again_times * out->dgain_times) < in->gain_times) {
			out->dgain_times += AR0430_gain_info.D_level_step; //out gain should great then in gain. prevent the small gain value then cause the gain hunting
			root += 1;
		}
		
		G_gain_value.conversion = 0; //0x317A, x1.53
		G_gain_value.Global_Coarse_A = 2; //0x305E[5:4], x 4
		G_gain_value.global_fine_A = 15; //0x305E[3:0], x 1.9375
		G_gain_value.Global_Digital = root; //0x305E[15:7], x 0.18527(4*1.9375*1.53*0.015625), watch out the root < 64
	}
	else if(in->gain_times > 7.75 && in->gain_times <= 11.8575) { //Again_level-4
		UCHAR root;
		FLOAT sen_gain_setting = in->gain_times / AR0430_gain_info.A_level4_step;

		if((root = sen_gain_setting + AR0430_gain_info.tolerance) > AR0430_gain_info.A_gain_range) {
			root = AR0430_gain_info.A_gain_range;
		}
		out->again_times = ((FLOAT)root) / 16.0 * 6.12;
		if(root != AR0430_gain_info.A_gain_range && in->gain_times && out->again_times < in->gain_times) {
			out->again_times += AR0430_gain_info.A_level4_step; //out gain should great then in gain. prevent the small gain value then cause the gain hunting
			root += 1;
		}
		out->dgain_times = 1.0;
		
		G_gain_value.conversion = 0; //0x317A, x1.53(0.3825=0.0625*4*1.53)
		G_gain_value.Global_Coarse_A = 2; //0x305E[5:4], x 4
		G_gain_value.global_fine_A = root - 16; //0x305E[3:0], x 0.0625
		G_gain_value.Global_Digital = 0x40; //0x305E[15:7], x 1
	}
	else if(in->gain_times > 3.875 && in->gain_times <= 7.75) { //Again_level-3
		UCHAR root;
		FLOAT sen_gain_setting = in->gain_times / AR0430_gain_info.A_level3_step;
		
		if((root = sen_gain_setting + AR0430_gain_info.tolerance) > AR0430_gain_info.A_gain_range) {
			root = AR0430_gain_info.A_gain_range;
		}
		out->again_times = ((FLOAT)root) / 4.0; //16.0 * 4.0;
		if(root != AR0430_gain_info.A_gain_range && in->gain_times && out->again_times < in->gain_times) {
			out->again_times += AR0430_gain_info.A_level3_step; //out gain should great then in gain. prevent the small gain value then cause the gain hunting
			root += 1;
		}
		out->dgain_times = 1.0;
		
		G_gain_value.conversion = 1; //0x317A, x1
		G_gain_value.Global_Coarse_A = 2; //0x305E[5:4], x 4(0.25=0.0625*4)
		G_gain_value.global_fine_A = root - 16; //0x305E[3:0], x 0.0625, watch out the root <= 15
		G_gain_value.Global_Digital = 0x40; //0x305E[15:7], x 1
	}
	else if(in->gain_times > 1.9375 && in->gain_times <= 3.875) //Again_level-2
	{	
		UCHAR root; //Set EXp will come each time wihout any AE target range or torences.
		FLOAT sen_gain_setting = in->gain_times / AR0430_gain_info.A_level2_step; //And in-gian was Fluttering
	
		if((root = sen_gain_setting + AR0430_gain_info.tolerance) > AR0430_gain_info.A_gain_range) { //+0.59. prevent the remain value than cause the gain hunting
			root = AR0430_gain_info.A_gain_range;
		}

		out->again_times = ((FLOAT)root) / 8.0; //16.0 * 2.0;
		if(root != AR0430_gain_info.A_gain_range && in->gain_times && out->again_times < in->gain_times) {
			out->again_times += AR0430_gain_info.A_level2_step; //out gain should great then in gain. prevent the small gain value then cause the gain hunting
			root += 1;
		}
		out->dgain_times = 1.0;
		
		G_gain_value.conversion = 1; //0x317A, x1
		G_gain_value.Global_Coarse_A = 1; //0x305E[5:4], x 2(0.125=0.0625*2)
		G_gain_value.global_fine_A = root - 16; //0x305E[3:0], x 0.0625, watch out the root <= 15
		G_gain_value.Global_Digital = 0x40; //0x305E[15:7], x 1
	}
	else if(in->gain_times > 1 && in->gain_times <= 1.9375) { //Again_level-1
		UCHAR root; //Set EXp will come each time wihout any AE target range or torences.
		FLOAT sen_gain_setting = in->gain_times / AR0430_gain_info.A_level1_step; //And in-gian was Fluttering

		if((root = sen_gain_setting + AR0430_gain_info.tolerance) > AR0430_gain_info.A_gain_range) { //+0.59. prevent the small gain value than cause the gain hunting
			root = AR0430_gain_info.A_gain_range;
		}

		out->again_times = ((FLOAT)root) / 16.0;
		if(root != AR0430_gain_info.A_gain_range && in->gain_times && out->again_times < in->gain_times) {
			out->again_times += AR0430_gain_info.A_level1_step; //out gain should great then in gain. prevent the small gain value then cause the gain hunting
			root += 1;
		}
		out->dgain_times = 1.0;
		
		G_gain_value.conversion = 1; //0x317A, x 1
		G_gain_value.Global_Coarse_A = 0; //0x305E[5:4], x1
		G_gain_value.global_fine_A = root - 16; //0x305E[3:0], x 0.0625, watch out the root <= 15
		G_gain_value.Global_Digital = 0x40; //0x305E[15:7], x 1
	}
	else {
		out->again_times = 1;
		out->dgain_times = 1;
		G_gain_value.conversion = 1; //0x317A, x 1
		G_gain_value.Global_Coarse_A = 0; //0x305E[5:4], x 1
		G_gain_value.global_fine_A = 0; //0x305E[3:0], x 1
		G_gain_value.Global_Digital = 0x40; //0x305E[15:7], x 1//exsection
	}
}

