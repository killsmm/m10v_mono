/**
 * @file
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2019 Ability Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "camera_sensor_imx269.h"
#include "camera_config.h"
#include "os_user_custom.h"
#include "camera_common.h"
#include "im_pro.h"
#include <math.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	D_TBL_END		(0xFF)
#define	D_TBL_WAIT_MS	(0xFE)

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
static E_CAMERA_ERR_CODE access_tbl(E_CAMERA_ID sensor_id, const T_CAMERA_SENSOR_REG_EX* tbl);

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
const T_CAMERA_SENSOR_REG_EX afe_Standby[] ={
    {.chipid = 0x02, .address = 0x00, .data = 0x01}, 			/* [0]: standby ,[1]: sleep */
    {.chipid = 0x00, .address = D_TBL_END, .data = 0xFF}		// END
};

const T_CAMERA_SENSOR_REG_EX afe_Wakeup[]={
    {.chipid = 0x02, .address = 0x00, .data = 0x04},
    {.chipid = 0x00, .address = D_TBL_END, .data = 0xFF}		// END
};


const T_CAMERA_SENSOR_REG_EX afe_sExp_code[] ={
    {.chipid = 0x02, .address = 0x07, .data = 0x40}, 			/*SHR start pos(temp exp), unit:line(low-byte)*/
    {.chipid = 0x02, .address = 0x08, .data = 0x00}, 			/*SHR start pos(temp exp), unit:line(high-byte)*/
    {.chipid = 0x02, .address = 0x03, .data = 0x00}, 			/*SVR (can reduce frame rate)*/
    {.chipid = 0x02, .address = 0x04, .data = 0x00},			/*SVR*/
    {.chipid = 0x02, .address = 0x05, .data = 0x00}, 			/*SPL, low*/
    {.chipid = 0x02, .address = 0x06, .data = 0x00}, 			/*SPL, high*/
    {.chipid = 0x00, .address = D_TBL_END, .data = 0xFF}		// END
};

#define N_1_1V_USE_EXT_POWER 1

const T_CAMERA_SENSOR_REG_EX IMX269_MODE7_4128x2264_RAW10_60_46FPS[] ={
	{.chipid = 0x02, .address = 0x00, .data = 0x04},
	{.chipid = 0x02, .address = 0x01, .data = 0x07},
	{.chipid = 0x02, .address = 0x0D, .data = 0x02},
	{.chipid = 0x02, .address = 0x34, .data = 0xF9},
	{.chipid = 0x02, .address = 0x3B, .data = 0x00},
	{.chipid = 0x02, .address = 0x3C, .data = 0x00},
	{.chipid = 0x02, .address = 0x56, .data = 0x00},
	{.chipid = 0x02, .address = 0x57, .data = 0x00},
	{.chipid = 0x02, .address = 0x58, .data = 0x00},
	{.chipid = 0x02, .address = 0x5A, .data = 0x32},
	{.chipid = 0x02, .address = 0x60, .data = 0x04},
	{.chipid = 0x02, .address = 0x62, .data = 0x00},
	{.chipid = 0x02, .address = 0x63, .data = 0x00},
	{.chipid = 0x02, .address = 0x66, .data = 0x00},
	{.chipid = 0x02, .address = 0x8E, .data = 0x10},
	{.chipid = 0x02, .address = 0x93, .data = 0x70},
	{.chipid = 0x02, .address = 0xA2, .data = 0x02},
#if defined(N_1_1V_USE_EXT_POWER)
	{.chipid = 0x02, .address = 0xA4, .data = 0x24},
	{.chipid = 0x02, .address = 0xA6, .data = 0x18},
#endif
	{.chipid = 0x02, .address = 0xAC, .data = 0x0F},
	{.chipid = 0x02, .address = 0xB0, .data = 0x00},
	{.chipid = 0x02, .address = 0xEC, .data = 0x91},
	{.chipid = 0x02, .address = 0xEF, .data = 0x73},
	{.chipid = 0x02, .address = 0xF0, .data = 0x09},
	{.chipid = 0x02, .address = 0xF1, .data = 0x09},
	{.chipid = 0x02, .address = 0xF3, .data = 0x44},
	{.chipid = 0x03, .address = 0x3D, .data = 0x74},
	{.chipid = 0x03, .address = 0xC2, .data = 0x10},
	{.chipid = 0x03, .address = 0xC6, .data = 0x10},
	{.chipid = 0x03, .address = 0xC8, .data = 0x3F},
	{.chipid = 0x03, .address = 0xCA, .data = 0x1E},
	{.chipid = 0x03, .address = 0xCE, .data = 0x19},
	{.chipid = 0x03, .address = 0xD2, .data = 0x55},
	{.chipid = 0x03, .address = 0xD6, .data = 0x1E},
	{.chipid = 0x03, .address = 0xDA, .data = 0x19},
	{.chipid = 0x03, .address = 0xDE, .data = 0x81},
	{.chipid = 0x03, .address = 0xDF, .data = 0x01},
	{.chipid = 0x03, .address = 0xF4, .data = 0x63},
	{.chipid = 0x03, .address = 0xF8, .data = 0xC2},
	{.chipid = 0x04, .address = 0x35, .data = 0x64},
	{.chipid = 0x04, .address = 0x39, .data = 0xC3},
	{.chipid = 0x04, .address = 0x58, .data = 0x10},
	{.chipid = 0x04, .address = 0x70, .data = 0x3F},
	{.chipid = 0x04, .address = 0x72, .data = 0x34},
	{.chipid = 0x04, .address = 0x76, .data = 0x0A},
	{.chipid = 0x04, .address = 0x78, .data = 0x69},
	{.chipid = 0x04, .address = 0x7A, .data = 0x10},
	{.chipid = 0x04, .address = 0x7E, .data = 0x11},
	{.chipid = 0x04, .address = 0x82, .data = 0x4E},
	{.chipid = 0x04, .address = 0x84, .data = 0xC9},
	{.chipid = 0x04, .address = 0x86, .data = 0x3B},
	{.chipid = 0x04, .address = 0xA0, .data = 0x01},
	{.chipid = 0x04, .address = 0xA1, .data = 0x0E},
	{.chipid = 0x04, .address = 0xA2, .data = 0xFF},
	{.chipid = 0x04, .address = 0xA3, .data = 0x07},
	{.chipid = 0x04, .address = 0xA9, .data = 0x0C},
	{.chipid = 0x04, .address = 0xAB, .data = 0x11},
	{.chipid = 0x05, .address = 0x00, .data = 0x00},
	{.chipid = 0x05, .address = 0x02, .data = 0x00},
	{.chipid = 0x05, .address = 0x0A, .data = 0x00},
	{.chipid = 0x05, .address = 0x0B, .data = 0x00},
	{.chipid = 0x05, .address = 0x0C, .data = 0x00},
	{.chipid = 0x05, .address = 0x0D, .data = 0x00},
	{.chipid = 0x00, .address = D_TBL_WAIT_MS, .data = 0x01},
	{.chipid = 0x00, .address = D_TBL_END, .data = 0xFF}
};

const T_CAMERA_SENSOR_REG_EX IMX269_MODE5_5280x3044_RAW10_36_35FPS[] ={
	{.chipid = 0x02, .address = 0x00, .data = 0x04},
	{.chipid = 0x02, .address = 0x01, .data = 0x05},
	{.chipid = 0x02, .address = 0x0D, .data = 0x02},	//When using to slow down the frame rate, set to appropriately
	{.chipid = 0x02, .address = 0x34, .data = 0xF9},
	{.chipid = 0x02, .address = 0x3B, .data = 0x00},	//When using in LVDS 8 ch: 10h, 12ch :00h
	{.chipid = 0x02, .address = 0x3C, .data = 0x03},
	{.chipid = 0x02, .address = 0x56, .data = 0x00},
	{.chipid = 0x02, .address = 0x57, .data = 0x00},
	{.chipid = 0x02, .address = 0x58, .data = 0x00},
	{.chipid = 0x02, .address = 0x5A, .data = 0x42},
	{.chipid = 0x02, .address = 0x60, .data = 0x0A},
	{.chipid = 0x02, .address = 0x62, .data = 0x00},	//Set according to the analog gain setting
	{.chipid = 0x02, .address = 0x63, .data = 0x04},
	{.chipid = 0x02, .address = 0x66, .data = 0x00},	//When using movie clamp in LVDS 8 ch : 02h, 12h: 00h
	{.chipid = 0x02, .address = 0x8E, .data = 0x10},	//12h : 648 Mbps 10h : 576 Mbps
	{.chipid = 0x02, .address = 0x93, .data = 0x70},	//70h : INCK 36 MHz
	{.chipid = 0x02, .address = 0xA2, .data = 0x02},
#if defined(N_1_1V_USE_EXT_POWER)
	{.chipid = 0x02, .address = 0xA4, .data = 0x24},	//Standby setting for the charge pump VCPTRG : 24h
	{.chipid = 0x02, .address = 0xA6, .data = 0x18},	//Standby setting for the charge pump VCPRST : 18h
#endif
	{.chipid = 0x02, .address = 0xAC, .data = 0x0F},
	{.chipid = 0x02, .address = 0xB0, .data = 0x40},	//Register for 4k movie crop setting
	{.chipid = 0x02, .address = 0xEC, .data = 0x95},	//Register for 4k movie crop setting
	{.chipid = 0x02, .address = 0xEF, .data = 0x73},
	{.chipid = 0x02, .address = 0xF0, .data = 0x09},
	{.chipid = 0x02, .address = 0xF1, .data = 0x09},
	{.chipid = 0x02, .address = 0xF3, .data = 0x44},
	{.chipid = 0x03, .address = 0x3D, .data = 0x74},
	{.chipid = 0x03, .address = 0xC2, .data = 0x10},
	{.chipid = 0x03, .address = 0xC6, .data = 0x10},
	{.chipid = 0x03, .address = 0xC8, .data = 0x3F},
	{.chipid = 0x03, .address = 0xCA, .data = 0x1E},	//Timing setting for AD 12 bits
	{.chipid = 0x03, .address = 0xCE, .data = 0x19},	//Timing setting for AD 12 bits
	{.chipid = 0x03, .address = 0xD2, .data = 0x55},	//Timing setting for AD 12 bits
	{.chipid = 0x03, .address = 0xD6, .data = 0x1E},	//Timing setting for AD 12 bits
	{.chipid = 0x03, .address = 0xDA, .data = 0x19},	//Timing setting for AD 12 bits
	{.chipid = 0x03, .address = 0xDE, .data = 0x81},	//Timing setting for AD 12 bits
	{.chipid = 0x03, .address = 0xDF, .data = 0x01},	//Timing setting for AD 12 bits
	{.chipid = 0x03, .address = 0xF4, .data = 0x57},
	{.chipid = 0x03, .address = 0xF8, .data = 0xB6},
	{.chipid = 0x04, .address = 0x35, .data = 0x58},
	{.chipid = 0x04, .address = 0x39, .data = 0xB7},
	{.chipid = 0x04, .address = 0x58, .data = 0x10},	//12h : 648 Mbps, 10h : 576 Mbps
	{.chipid = 0x04, .address = 0x70, .data = 0x3F},
	{.chipid = 0x04, .address = 0x72, .data = 0x34},	//Timing register for AD 10 bits
	{.chipid = 0x04, .address = 0x76, .data = 0x0A},	//Timing register for AD 10 bits
	{.chipid = 0x04, .address = 0x78, .data = 0x69},	//Timing register for AD 10 bits
	{.chipid = 0x04, .address = 0x7A, .data = 0x10},	//Timing register for AD 10 bits
	{.chipid = 0x04, .address = 0x7E, .data = 0x11},	//Timing register for AD 10 bits
	{.chipid = 0x04, .address = 0x82, .data = 0x4E},	//Timing register for AD 10 bits
	{.chipid = 0x04, .address = 0x84, .data = 0xC9},	//Timing register for AD 10 bits
	{.chipid = 0x04, .address = 0x86, .data = 0x11},	//Timing register for AD 10 bits
	{.chipid = 0x04, .address = 0xA0, .data = 0x01},
	{.chipid = 0x04, .address = 0xA1, .data = 0x0E},
	{.chipid = 0x04, .address = 0xA2, .data = 0xFF},
	{.chipid = 0x04, .address = 0xA3, .data = 0x07},
	{.chipid = 0x04, .address = 0xA9, .data = 0x00},
	{.chipid = 0x04, .address = 0xAB, .data = 0x11},
	{.chipid = 0x05, .address = 0x00, .data = 0x4D},	//Register for low noise mode
	{.chipid = 0x05, .address = 0x02, .data = 0xAD},	//Register for low noise mode
	{.chipid = 0x05, .address = 0x0A, .data = 0x86},	//Register for low noise mode
	{.chipid = 0x05, .address = 0x0B, .data = 0x10},	//Register for low noise mode
	{.chipid = 0x05, .address = 0x0C, .data = 0x0E},	//Register for low noise mode
	{.chipid = 0x05, .address = 0x0D, .data = 0x11},	//Register for low noise mode
	{.chipid = 0x00, .address = D_TBL_WAIT_MS, .data = 0x01},
	{.chipid = 0x00, .address = D_TBL_END, .data = 0xFF}
};

const T_CAMERA_SENSOR_REG_EX IMX269_MODE1_5280x3956_RAW12_18_92FPS[] ={
	{.chipid = 0x02, .address = 0x00, .data = 0x04},
	{.chipid = 0x02, .address = 0x01, .data = 0x01},
	{.chipid = 0x02, .address = 0x0D, .data = 0x02},	//When using to slow down the frame rate, set to appropriately
	{.chipid = 0x02, .address = 0x34, .data = 0xF9},
	{.chipid = 0x02, .address = 0x3B, .data = 0x00},	//When using in LVDS 8ch: 10h, 12ch: 00h
	{.chipid = 0x02, .address = 0x3C, .data = 0x03},
	{.chipid = 0x02, .address = 0x56, .data = 0x00},
	{.chipid = 0x02, .address = 0x57, .data = 0x00},
	{.chipid = 0x02, .address = 0x58, .data = 0x00},
	{.chipid = 0x02, .address = 0x5A, .data = 0x42},
	{.chipid = 0x02, .address = 0x60, .data = 0x0A},
	{.chipid = 0x02, .address = 0x62, .data = 0x00},	//Set according to the analog gain setting
	{.chipid = 0x02, .address = 0x63, .data = 0x04},
	{.chipid = 0x02, .address = 0x66, .data = 0x00},	//When using movie clamp in LVDS 8 ch : 02h, 12ch: 00h
	{.chipid = 0x02, .address = 0x8E, .data = 0x10}, 	//12h: 648 Mbps, 10h : 576 Mbps
	{.chipid = 0x02, .address = 0x93, .data = 0x70}, 	//: INCK 36 MHz
	{.chipid = 0x02, .address = 0xA2, .data = 0x02},
#if defined(N_1_1V_USE_EXT_POWER)
	{.chipid = 0x02, .address = 0xA4, .data = 0x24},	//standby setting for the charge pump VCPTRG : 24h
	{.chipid = 0x02, .address = 0xA6, .data = 0x18},	//standby setting for the charge pump VCPRST : 18h
#endif
	{.chipid = 0x02, .address = 0xAC, .data = 0x0F},
	{.chipid = 0x02, .address = 0xB0, .data = 0x40},	//Register for 4k movie crop setting
	{.chipid = 0x02, .address = 0xEC, .data = 0x95},	//Register for 4k movie crop setting
	{.chipid = 0x02, .address = 0xEF, .data = 0x72},
	{.chipid = 0x02, .address = 0xF0, .data = 0x0B},
	{.chipid = 0x02, .address = 0xF1, .data = 0x0B},
	{.chipid = 0x02, .address = 0xF3, .data = 0x44},
	{.chipid = 0x03, .address = 0x3D, .data = 0x74},
	{.chipid = 0x03, .address = 0xC2, .data = 0x10},
	{.chipid = 0x03, .address = 0xC6, .data = 0x10},
	{.chipid = 0x03, .address = 0xC8, .data = 0x3F},
	{.chipid = 0x03, .address = 0xCA, .data = 0x3F},	//Timing setting for AD 12 bits
	{.chipid = 0x03, .address = 0xCE, .data = 0x0A},	//Timing setting for AD 12 bits
	{.chipid = 0x03, .address = 0xD2, .data = 0x59},	//Timing setting for AD 12 bits
	{.chipid = 0x03, .address = 0xD6, .data = 0x11},	//Timing setting for AD 12 bits
	{.chipid = 0x03, .address = 0xDA, .data = 0x35},	//Timing setting for AD 12 bits
	{.chipid = 0x03, .address = 0xDE, .data = 0xDA},	//Timing setting for AD 12 bits
	{.chipid = 0x03, .address = 0xDF, .data = 0x00},	//Timing setting for AD 12 bits
	{.chipid = 0x03, .address = 0xF4, .data = 0x5C},
	{.chipid = 0x03, .address = 0xF8, .data = 0xBB},
	{.chipid = 0x04, .address = 0x35, .data = 0x5D},
	{.chipid = 0x04, .address = 0x39, .data = 0xBC},
	{.chipid = 0x04, .address = 0x58, .data = 0x10}, 	//: 648 Mbps: 12h, 576 Mbps 10h
	{.chipid = 0x04, .address = 0x70, .data = 0x3F},
	{.chipid = 0x04, .address = 0x72, .data = 0x1E},	//timing register for AD 10 bits
	{.chipid = 0x04, .address = 0x76, .data = 0x0E},	//timing register for AD 10 bits
	{.chipid = 0x04, .address = 0x78, .data = 0x5F},	//timing register for AD 10 bits
	{.chipid = 0x04, .address = 0x7A, .data = 0x05},	//timing register for AD 10 bits
	{.chipid = 0x04, .address = 0x7E, .data = 0x1E},	//timing register for AD 10 bits
	{.chipid = 0x04, .address = 0x82, .data = 0x0E},	//timing register for AD 10 bits
	{.chipid = 0x04, .address = 0x84, .data = 0xBF},	//timing register for AD 10 bits
	{.chipid = 0x04, .address = 0x86, .data = 0x31},	//timing register for AD 10 bits
	{.chipid = 0x04, .address = 0xA0, .data = 0x01},
	{.chipid = 0x04, .address = 0xA1, .data = 0x0F},
	{.chipid = 0x04, .address = 0xA2, .data = 0xFF},
	{.chipid = 0x04, .address = 0xA3, .data = 0x07},
	{.chipid = 0x04, .address = 0xA9, .data = 0x05},
	{.chipid = 0x04, .address = 0xAB, .data = 0x11},
	{.chipid = 0x05, .address = 0x00, .data = 0x4D},	//register for low noise mode
	{.chipid = 0x05, .address = 0x02, .data = 0xAD},	//register for low noise mode
	{.chipid = 0x05, .address = 0x0A, .data = 0x86},	//register for low noise mode
	{.chipid = 0x05, .address = 0x0B, .data = 0x10},	//register for low noise mode
	{.chipid = 0x05, .address = 0x0C, .data = 0x0E},	//register for low noise mode
	{.chipid = 0x05, .address = 0x0D, .data = 0x11},	//register for low noise mode
	{.chipid = 0x00, .address = D_TBL_WAIT_MS, .data = 0x01},
	{.chipid = 0x00, .address = D_TBL_END, .data = 0xFF}		// END
};

const T_CAMERA_SENSOR_SYNC_CODE	IMX269_10BIT_SYNC_CODE = 
{
	.sav = { 0x03FF, 0x0000, 0x0000, 0x0200 },
	.eav = { 0x03FF, 0x0000, 0x0000, 0x0274 },
	.sab = { 0x03FF, 0x0000, 0x0000, 0x02AC },
	.eab = { 0x03FF, 0x0000, 0x0000, 0x02D8 },
};

const T_CAMERA_SENSOR_SYNC_CODE	IMX269_10BIT_ALL_PIXEL_SYNC_CODE = 
{
	.sav = { 0x03FF, 0x0000, 0x0000, 0x0208 },
	.eav = { 0x03FF, 0x0000, 0x0000, 0x0274 },
	.sab = { 0x03FF, 0x0000, 0x0000, 0x02AC },
	.eab = { 0x03FF, 0x0000, 0x0000, 0x02D8 },
};


const T_CAMERA_SENSOR_SYNC_CODE	IMX269_12BIT_SYNC_CODE = 
{
	.sav = { 0x0FFF, 0x0000, 0x0000, 0x0800 },
	.eav = { 0x0FFF, 0x0000, 0x0000, 0x09D0 },
	.sab = { 0x0FFF, 0x0000, 0x0000, 0x0AB0 },
	.eab = { 0x0FFF, 0x0000, 0x0000, 0x0B60 },
};

const T_CAMERA_SENSOR_SYNC_CODE	IMX269_12BIT_ALL_PIXEL_SYNC_CODE = 
{
	.sav = { 0x0FFF, 0x0000, 0x0000, 0x0820 },
	.eav = { 0x0FFF, 0x0000, 0x0000, 0x09D0 },
	.sab = { 0x0FFF, 0x0000, 0x0000, 0x0AB0 },
	.eab = { 0x0FFF, 0x0000, 0x0000, 0x0B60 },
};

const T_CAMERA_SENSOR_SYNC_MASK	IMX269_MASK = {
	.msav = { 0x0000, 0x0000, 0x0000, 0x0000 },
	.meav = { 0x0000, 0x0000, 0x0000, 0x0000 },
	.msab = { 0x0000, 0x0000, 0x0000, 0x0000 },
	.meab = { 0x0000, 0x0000, 0x0000, 0x0000 },
};

const E_CAMERA_LVDS_SNCLK_SEL	IMX269_CLOCK_SELECT[8] = {
	[0] = E_CAMERA_LVDS_SNCLK_SEL_SNCLK0,
	[1] = E_CAMERA_LVDS_SNCLK_SEL_SNCLK0,
	[2] = E_CAMERA_LVDS_SNCLK_SEL_SNCLK1,
	[3] = E_CAMERA_LVDS_SNCLK_SEL_SNCLK1,
	[4] = E_CAMERA_LVDS_SNCLK_SEL_SNCLK2,
	[5] = E_CAMERA_LVDS_SNCLK_SEL_SNCLK2,
	[6] = E_IM_PRO_LVDS_SNCLK_SEL_STOP,
	[7] = E_IM_PRO_LVDS_SNCLK_SEL_STOP,
};


const E_CAMERA_LVDS_SNDATA_SEL 	IMX269_DATA_SELECT[16] = {
	[0] = E_CAMERA_LVDS_SNDATA_SEL_SNDATA0,
	[1] = E_CAMERA_LVDS_SNDATA_SEL_SNDATA1,
	[2] = E_CAMERA_LVDS_SNDATA_SEL_SNDATA2,
	[3] = E_CAMERA_LVDS_SNDATA_SEL_SNDATA3,
	[4] = E_CAMERA_LVDS_SNDATA_SEL_SNDATA4,
	[5] = E_CAMERA_LVDS_SNDATA_SEL_SNDATA5,
	[6] = E_CAMERA_LVDS_SNDATA_SEL_SNDATA6,
	[7] = E_CAMERA_LVDS_SNDATA_SEL_SNDATA7,
	[8] = E_CAMERA_LVDS_SNDATA_SEL_SNDATA8,
	[9] = E_CAMERA_LVDS_SNDATA_SEL_SNDATA9,
	[10] = E_CAMERA_LVDS_SNDATA_SEL_SNDATA10,
	[11] = E_CAMERA_LVDS_SNDATA_SEL_SNDATA11,
	[12] = 0,
	[13] = 0,
	[14] = 0,
	[15] = 0,
};

const T_CAMERA_SENSOR_BASIC		IMX269_BASIC =
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_36MHz,
	.access			= E_CAMERA_ACCESS_SPI_EX,
	.sensor_if		= E_CAMERA_SENSOR_IF_LVDS,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_VD,
};

const T_CAMERA_SENSOR_MODE	IMX269_MODE[] =
{
	[E_SENSOR_IMX269_MODE7_4128x2264_RAW10_60_46FPS] = {
		.mode_reg_ex			= IMX269_MODE7_4128x2264_RAW10_60_46FPS,
		.embedded_lines			= 0,
		.total_pix_width		= 4176,
		.total_pix_lines		= 2280,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= (12+36),
		.active_pix_y_pos		= (6+10),
		.active_pix_width		= 4128,
		.active_pix_lines		= 2264,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.input_bit_width		= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.hdocyc					= 260,
		.vdocyc					= 2290,
		.exp_start_line_min		= 0,
		.exp_end_line_offset	= 2,
		.exp_clk_offset			= 362,
		.base_SV				= 4.0,
		.Mpps_HDclk				= 36.0,
		.one_lane_Mbps			= 36,
		.lane_num				= 12,
		.fps					= 60.46,
		.sync_code				= &IMX269_10BIT_SYNC_CODE,
		.sync_mask				= &IMX269_MASK,
		.clock_select			= IMX269_CLOCK_SELECT,
		.data_select			= IMX269_DATA_SELECT,
	},
	[E_SENSOR_IMX269_MODE5_5280x3044_RAW10_36_35FPS] = {
		.mode_reg_ex			= IMX269_MODE5_5280x3044_RAW10_36_35FPS,
		.embedded_lines			= 0,
		.total_pix_width		= 5376,
		.total_pix_lines		= 3066,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= (12+36+2+2+20),
		.active_pix_y_pos		= (12+10),
		.active_pix_width		= 5280,
		.active_pix_lines		= 3044,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.input_bit_width		= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.hdocyc					= 322,
		.vdocyc					= 3076,
		.exp_start_line_min		= 0,
		.exp_end_line_offset	= 2,
		.exp_clk_offset			= 362,
		.base_SV				= 4.0,
		.Mpps_HDclk				= 36.0,
		.one_lane_Mbps			= 36,
		.lane_num				= 12,
		.fps					= 36.35,
		.sync_code				= &IMX269_10BIT_SYNC_CODE,
		.sync_mask				= &IMX269_MASK,
		.clock_select			= IMX269_CLOCK_SELECT,
		.data_select			= IMX269_DATA_SELECT,
	},
	[E_SENSOR_IMX269_MODE1_5280x3956_RAW12_18_92FPS] = {
		.mode_reg_ex			= IMX269_MODE1_5280x3956_RAW12_18_92FPS,
		.embedded_lines			= 0,
		.total_pix_width		= 5376,
		.total_pix_lines		= 3978,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= (12+36+2+2+20),
		.active_pix_y_pos		= (12+10),
		.active_pix_width		= 5280,
		.active_pix_lines		= 3956,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.input_bit_width		= 12,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.hdocyc					= 477,
		.vdocyc					= 3988,
		.exp_start_line_min		= 0,
		.exp_end_line_offset	= 2,
		.exp_clk_offset			= 626,
		.base_SV				= 4.0,
		.Mpps_HDclk				= 36.0,
		.lane_num				= 12,
		.fps					= 18.92,
		.sync_code				= &IMX269_12BIT_SYNC_CODE,
		.sync_mask				= &IMX269_MASK,
		.clock_select			= IMX269_CLOCK_SELECT,
		.data_select			= IMX269_DATA_SELECT,
	}
};

//static USHORT g_backup_analog_gain_value[4] = {0, 0, 0, 0};
static USHORT g_sensor_status = 0;
static FLOAT g_again_db = 0.0;
/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define ROUNDUP2(a) (((a) + 1) & 0xFFFFFFFE)

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

E_CAMERA_ERR_CODE Sensor_IMX269_Clock_Gpio_Init(VOID)
{
	INT32 sensor_id;

	printf("@@@@@@@@@@@@@@@@@@CAMERA.sensor[0].basic->input_clock:%d@@@@@@@@@@@@@@@@@@@@\n\r",CAMERA.sensor[0].basic->input_clock);
	// Clock sensor
	switch( CAMERA.sensor[0].basic->input_clock ) {
		case E_CAMERA_SENSOR_INCLK_12MHz:
			// 40 / (1+3)  *  (60*1+0)/1  / (49 + 1) = 12
			Dd_Top_Set_PLLCNT3_P00PREDIV(3);	// 40MHz / (val+1)
			Dd_Top_Set_PLLCNT4_P00FNUM(0);		//
			Dd_Top_Set_PLLCNT5_P00FDEN(0);		//
			Dd_Top_Set_PLLCNT3_P00PLLDIV(60);	// * val
			Dd_Top_Set_PLLCNT3_P00POSTDIV(49);	// / (val+1)
			Dd_Top_Start_Pll00();				// PLL00 Start
		break;
		
		case E_CAMERA_SENSOR_INCLK_24MHz:
			Dd_Top_Set_PLLCNT3_P00PREDIV(1);	// 40MHz / (val+1)
			Dd_Top_Set_PLLCNT4_P00FNUM(0);		//
			Dd_Top_Set_PLLCNT5_P00FDEN(0);		//
			Dd_Top_Set_PLLCNT3_P00PLLDIV(60);	// * val
			Dd_Top_Set_PLLCNT3_P00POSTDIV(49);	// / (val+1)
			Dd_Top_Start_Pll00();				// PLL00 Start
		break;

		case E_CAMERA_SENSOR_INCLK_36MHz:
			// 40 / (1+1)  *  (90*1+0)/1  / (49 + 1) = 20*1.8 = 36
			Dd_Top_Set_PLLCNT3_P00PREDIV(1);	// 40MHz / (val+1)
			Dd_Top_Set_PLLCNT4_P00FNUM(0);		//
			Dd_Top_Set_PLLCNT5_P00FDEN(0);		//
			Dd_Top_Set_PLLCNT3_P00PLLDIV(90);	// * val
			Dd_Top_Set_PLLCNT3_P00POSTDIV(49);	// / (val+1)
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

	// Sensor Initialization
	for (sensor_id = 0; sensor_id < CAMERA.sensor_num; sensor_id++)	{
		// XCLR
		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(CAMERA.sensor[sensor_id].port_xclr, 0 ); //1 0 : Pull-Down
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(CAMERA.sensor[sensor_id].port_xclr, 1 );
		Dd_Top_Set_Gpio_Function(CAMERA.sensor[sensor_id].port_xclr, D_DD_TOP_GPIO_FUNC_GPIO );	// use as GPIO
		Dd_Top_Set_Gpio_Direction(CAMERA.sensor[sensor_id].port_xclr, D_DD_TOP_GPIO_DIR_OUT );	// output
		Dd_Top_Set_Gpio_Status(CAMERA.sensor[sensor_id].port_xclr, D_DD_TOP_GPIO_STATUS_LOW );	// XCLR
		// pull down xvs/xhs
		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(CAMERA.sensor[sensor_id].port_xhs, 0 );
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(CAMERA.sensor[sensor_id].port_xhs, 1 );
		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(CAMERA.sensor[sensor_id].port_xvs, 0 );
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(CAMERA.sensor[sensor_id].port_xvs, 1 );

		// Sensor Power
		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PG5, 0 );	// Pull-Down
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PG5, 1 );
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PG5, FJ_GPIO_FUNC_GPIO );// use as GPIO
		Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PG5, FJ_GPIO_DIR_OUT );	// output
		Dd_Top_Set_Gpio_Status(  E_DD_TOP_GPIO_PG5, D_DD_TOP_GPIO_STATUS_LOW ); //CMOS_D1V2_EN

		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PG4, 0 );	// Pull-Down
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PG4, 1 );
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PG4, FJ_GPIO_FUNC_GPIO );// use as GPIO
		Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PG4, FJ_GPIO_DIR_OUT );	// output
		Dd_Top_Set_Gpio_Status(  E_DD_TOP_GPIO_PG4, D_DD_TOP_GPIO_STATUS_LOW ); //CMOS_D1V8_EN

		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PG3, 0 );	// Pull-Down
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PG3, 1 );
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PG3, FJ_GPIO_FUNC_GPIO );// use as GPIO
		Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PG3, FJ_GPIO_DIR_OUT );	// output
		Dd_Top_Set_Gpio_Status(  E_DD_TOP_GPIO_PG3, D_DD_TOP_GPIO_STATUS_LOW ); //VR2V9_EN
		OS_User_Dly_Tsk(10);

		Dd_Top_Set_Gpio_Status(  E_DD_TOP_GPIO_PG5, D_DD_TOP_GPIO_STATUS_HIGH );
		Dd_ARM_Wait_ns( 1 );
		Dd_Top_Set_Gpio_Status(  E_DD_TOP_GPIO_PG4, D_DD_TOP_GPIO_STATUS_HIGH );
		Dd_ARM_Wait_ns( 1 );
		Dd_Top_Set_Gpio_Status(  E_DD_TOP_GPIO_PG3, D_DD_TOP_GPIO_STATUS_HIGH );
		Dd_ARM_Wait_ns( 1 );

		// INCK
		switch (sensor_id) {
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
		Dd_Top_Set_Gpio_Function(CAMERA.sensor[sensor_id].port_inck, FJ_GPIO_FUNC_HWMODE);
		OS_User_Dly_Tsk(6);
		
		// XCLR
		Dd_Top_Set_Gpio_Status(CAMERA.sensor[sensor_id].port_xclr, D_DD_TOP_GPIO_STATUS_HIGH);	// XCLR
		OS_User_Dly_Tsk(1);

		// XCE
		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PD3, 1 );	//1 1: Pull-Up
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PD3, 1 );
		// SCK
		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PD2, 1 );	//1 1: Pull-Up
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PD2, 1 );	
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PD2, FJ_GPIO_FUNC_HWMODE );
		// SDO
		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PD0, 1 );	//1 1: Pull-Up
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PD0, 1 );
		// SDI
		Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PD1, 1 );	//1 1: Pull-Up
		Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PD1, 1 );
		// SDI/SDI/XCE HW function
		Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PD0, FJ_GPIO_FUNC_HWMODE );	// SDO
		Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PD1, FJ_GPIO_FUNC_HWMODE );	// SDI
		Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PD3, FJ_GPIO_FUNC_HWMODE );	// XCE

		// XVS XHS
		Dd_Top_Set_Gpio_Function(CAMERA.sensor[sensor_id].port_xhs, FJ_GPIO_FUNC_HWMODE );
		Dd_Top_Set_Gpio_Function(CAMERA.sensor[sensor_id].port_xvs, FJ_GPIO_FUNC_HWMODE );
		OS_User_Dly_Tsk(1);	
		
		// initial setting
		if( NULL != CAMERA.sensor[sensor_id].Func_Init ){
			if( CAMERA.sensor[sensor_id].Func_Init(sensor_id) == E_CAMERA_ERR_NG ){
				CAMERA_PRINT_ERROR(1);
			}
		}
	}
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX269_Standby(E_CAMERA_ID sensor_id)
{
	printf("@@@@@@@@@@@@@@@@@@Sensor_IMX269_Standby sensor_id:%d@@@@@@@@@@@@@@@@@@@@\n\r",(UINT32)sensor_id);
	//Sensor_IMX269_SgVHD(sensor_id, CAMERA.sensor[sensor_id].mode[0].vdocyc, CAMERA.sensor[sensor_id].mode[0].hdocyc);
	//OS_User_Dly_Tsk(1);
	g_sensor_status = 0;
	access_tbl(sensor_id, afe_Standby);
	CAMERA_PRINT_ERROR( Im_PRO_SG_Stop( sensor_id, 1 ));
	//Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 0 ); // XCLR
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX269_CancelStandby(E_CAMERA_ID sensor_id)
{
	printf("@@@@@@@@@@@@@@@@@@Sensor_IMX269_CancelStandby sensor_id:%d@@@@@@@@@@@@@@@@@@@@\n\r",(UINT32)sensor_id);
	g_sensor_status = 1;
	//Dd_Top_Set_Gpio_Status(CAMERA.sensor[sensor_id].port_xclr, 1); // XCLR
	//OS_User_Dly_Tsk(1); 	// Wait Sensor Stability (Needs Over 100ns)
	//Sensor_IMX269_SgVHD(sensor_id, CAMERA.sensor[sensor_id].mode[0].vdocyc, CAMERA.sensor[sensor_id].mode[0].hdocyc);
	//OS_User_Dly_Tsk(1);
	access_tbl(sensor_id, afe_Wakeup);

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX269_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol)
{
	//printf("@@@@@@@@@@@@@@@@@@Sensor_IMX269_Mode sensor_id:%d mode:%d dol:%d@@@@@@@@@@@@@@@@@@@@\n\r",(UINT32)sensor_id,(UINT32)mode,dol);
	//printf("@@@@@@@@@@@@@@@@@@CAMERA.sensor[sensor_id].mode[mode].vdocyc:%d@@@@@@@@@@@@@@@@@@@@\n\r",(UINT32)CAMERA.sensor[sensor_id].mode[mode].vdocyc);
	//printf("@@@@@@@@@@@@@@@@@@CAMERA.sensor[sensor_id].mode[mode].hdocyc:%d@@@@@@@@@@@@@@@@@@@@\n\r",(UINT32)CAMERA.sensor[sensor_id].mode[mode].hdocyc);
	// Training Sequence for V1 period
	//Sensor_IMX269_SgVHD(sensor_id, CAMERA.sensor[sensor_id].mode[mode].vdocyc, CAMERA.sensor[sensor_id].mode[mode].hdocyc);
	//OS_User_Dly_Tsk(1);
	// mode setting
	return access_tbl(sensor_id, IMX269_MODE[mode].mode_reg_ex);
}

E_CAMERA_ERR_CODE Sensor_IMX269_SgVHD(E_CAMERA_ID sensor_id, ULONG vdocyc, ULONG hdocyc)
{
	//printf("@@@@@@@@@@@@@@@@@@Sensor_IMX269_SgVHD sensor_id:%d vdocyc:%d hdocyc:%d@@@@@@@@@@@@@@@@@@@@\n\r",(UINT32)sensor_id,(UINT32)vdocyc,(UINT32)hdocyc);
	FJ_SG_CTRL	sg_tbl;
	static ULONG vdocyc_bak = 0; 
	static ULONG hdocyc_bak = 0; 
	
	if (g_sensor_status == 0) {
		vdocyc_bak = 0; 
		hdocyc_bak = 0; 
		return E_CAMERA_ERR_OK;
	}

	if ((vdocyc_bak == vdocyc) && (hdocyc_bak == hdocyc)) {
		return E_CAMERA_ERR_OK;
	}
	
	vdocyc_bak = vdocyc; 
	hdocyc_bak = hdocyc; 
	
	sg_tbl.vhdo		= 1;
	sg_tbl.vdot		= 1;
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

	CAMERA_PRINT_ERROR( FJ_SetSg( (UCHAR)sensor_id, &sg_tbl ) );
	CAMERA_PRINT_ERROR( FJ_SgStart( (UCHAR)sensor_id ) ); // restart after current frame

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX269_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
{
	UINT32	CIT;	// Coarse Integration Time
	UINT32	FLL;	// Frame Length Line
	UINT32	req_SHR = 0;
	UINT32	real_SHR = 0;
	FLOAT	req_fps;
	FLOAT	min_fps, max_fps, default_fps;
	FLOAT	realExpTime;

	FLOAT	again_dB, dgain_dB;
	FLOAT	work;
	USHORT	PGC;

	FLOAT req_exp_msec = 16.666;
	FLOAT req_gain_times = 1.0;

	req_exp_msec 	= in->exp_msec;
	req_gain_times 	= in->gain_times;

	CIT = ( (req_exp_msec / 1000.0) * (IMX269_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) + (FLOAT)IMX269_MODE[mode].exp_clk_offset) / (FLOAT)IMX269_MODE[mode].hdocyc;

	if (CIT == 0)
		CIT = 1;

	// Frame Length Line
	if (CIT > IMX269_MODE[mode].vdocyc)
		FLL = CIT;
	else
		FLL = IMX269_MODE[mode].vdocyc;

	// FPS, to confirm vdocyc
	if (D_CAMERA_FPS_AUTO != in->min_fps || D_CAMERA_FPS_AUTO != in->max_fps) {
		// default
		default_fps = (IMX269_MODE[mode].Mpps_HDclk  * 1000.0 * 1000.0) / IMX269_MODE[mode].hdocyc / IMX269_MODE[mode].vdocyc;
		// check min_fps
		if (in->min_fps == D_CAMERA_FPS_AUTO)
			min_fps = D_CAMERA_FPS_AUTO;
		else if (in->min_fps > default_fps) {
			min_fps = default_fps;
			CAMERA_PRINT_ERROR(TRUE);
		} else
			min_fps = in->min_fps;

		// check max_fps
		if (in->max_fps == D_CAMERA_FPS_AUTO)
			max_fps = D_CAMERA_FPS_AUTO;
		else if( in->max_fps > default_fps ) {
			max_fps = default_fps;
			CAMERA_PRINT_ERROR(TRUE);
		} else
			max_fps = in->max_fps;

		// check min_fps vs max_fps
		if (max_fps < min_fps)
			min_fps = max_fps;

		// clip real_vdocyc
		req_fps = (IMX269_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX269_MODE[mode].hdocyc / (FLOAT)FLL;
		if (req_fps < min_fps)
			FLL = (IMX269_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX269_MODE[mode].hdocyc / min_fps;

		if (req_fps > max_fps)
			FLL = (IMX269_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX269_MODE[mode].hdocyc / max_fps;

		// clip CIT
		if( CIT > FLL)
			CIT = FLL;
	}

	req_SHR = FLL - CIT;

	// SHR
	if ((FLL - IMX269_MODE[mode].exp_end_line_offset) < req_SHR)
		real_SHR = FLL - IMX269_MODE[mode].exp_end_line_offset;
	else
		real_SHR = (USHORT)req_SHR;
	
	// --------------------
	// Analog Gain
	// --------------------
	if (req_gain_times < 1.0)
		req_gain_times = 1.0;

	again_dB = 20.0 * log10f(req_gain_times);
	if(again_dB > 24.0)
		again_dB = 24.0;

	g_again_db = again_dB;

	work = 4095.0 * (1.0 - powf(10, (again_dB / (-20))));
	
	if (work > 4095.0)
		PGC = 4095.0;
	else if (work < 0.0)
		PGC = 0.0;
	else
		PGC = (USHORT)work;

	// recalc
	again_dB = -20.0 * log10f( (4095.0 - PGC) / 4095.0 );
	// --------------------
	// Digital Gain
	// --------------------
	dgain_dB = 0; // fix to 1.0 times (temporary)

	// Revert
	realExpTime = ( ((FLOAT)FLL - (FLOAT)real_SHR) * (FLOAT)IMX269_MODE[mode].hdocyc - IMX269_MODE[mode].exp_clk_offset) / (IMX269_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	out->exp_msec = realExpTime * 1000.0;
	out->fps = (IMX269_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / IMX269_MODE[mode].hdocyc / FLL;
	out->hdocyc = IMX269_MODE[mode].hdocyc;
	out->vdocyc = FLL;
	out->again_times = powf(10, (again_dB / 20.0));
	out->dgain_times = powf(10, (dgain_dB / 20.0));

	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = out->hdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = out->vdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] 	 = real_SHR;
	out_reg->reg[E_CAMERA_SENSOR_REG_AGAIN]	= PGC;
	out_reg->reg[E_CAMERA_SENSOR_REG_DGAIN]	= 0;

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX269_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in)
{
	U_REG reg;
	UCHAR data;

	// SHR
	reg.hword = (USHORT)in->reg[E_CAMERA_SENSOR_REG_EXP];
	Camera_Common_SPI_Write_Ex(0x02 , CAMERA.sensor[sensor_id].spi_ch, 0x07, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);

	if (mode == E_SENSOR_IMX269_MODE7_4128x2264_RAW10_60_46FPS) {
		// Clamp reset
		Camera_Common_SPI_Read_Ex(0x02 | 0x80, CAMERA.sensor[sensor_id].spi_ch, 0x5a, &data, 1, CAMERA.sensor[sensor_id].is_spics_gpio);
		data |= 0x40;
		Camera_Common_SPI_Write_Ex(0x02 , CAMERA.sensor[sensor_id].spi_ch, 0x5a, &data, 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	}

	if (0.0 < g_again_db && g_again_db <= 6.0)
		reg.byte[0] = 1;
	else if (6.0 < g_again_db && g_again_db <= 12.0)
		reg.byte[0] = 2;
	else if (12.0 < g_again_db && g_again_db <= 18.0)
		reg.byte[0] = 3;
	else if (18.0 < g_again_db && g_again_db <= 24.0)
		reg.byte[0] = 4;
	else
		reg.byte[0] = 0;

	Camera_Common_SPI_Write_Ex(0x02 ,CAMERA.sensor[sensor_id].spi_ch, 0x62, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);

	// AGAIN
	//reg.hword = (USHORT)g_backup_analog_gain_value[sensor_id];
	reg.hword = (USHORT)in->reg[E_CAMERA_SENSOR_REG_AGAIN];
	Camera_Common_SPI_Write_Ex(0x02 ,CAMERA.sensor[sensor_id].spi_ch, 0x75, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	Camera_Common_SPI_Write_Ex(0x02 ,CAMERA.sensor[sensor_id].spi_ch, 0x77, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	//g_backup_analog_gain_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_AGAIN];

	if (mode == E_SENSOR_IMX269_MODE7_4128x2264_RAW10_60_46FPS) {
		// Clamp reset
		data &= ~0x40;
		Camera_Common_SPI_Write_Ex(0x02 , CAMERA.sensor[sensor_id].spi_ch, 0x5a, &data, 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	}

	return E_CAMERA_ERR_OK;
}

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static E_CAMERA_ERR_CODE access_tbl(E_CAMERA_ID sensor_id, const T_CAMERA_SENSOR_REG_EX* tbl)
{
	INT32	cnt = 0;
	
	while(1) {
		// end check
		if( D_TBL_END == tbl[cnt].address )
			break;
		
		// access
		if (D_TBL_WAIT_MS == tbl[cnt].address) {
			OS_User_Dly_Tsk( tbl[cnt].data );
		} else {
			Camera_Common_SPI_Write_Ex( tbl[cnt].chipid , CAMERA.sensor[sensor_id].spi_ch, tbl[cnt].address, (UCHAR*) &(tbl[cnt].data), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
		}
		cnt++;
	}

	return E_CAMERA_ERR_OK;
}
