/**
 * @file
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "camera_sensor_imx377.h"
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
const T_CAMERA_SENSOR_REG IMX377_INIT[] =
{

	// Standby Cancel Sequence
	{ .address = 0x3123, .data[0] =	0x00 },	// PLRD10 = 00h
	{ .address = 0x3124, .data[0] =	0x00 },	// PLRD11 = 00h
	{ .address = 0x3127, .data[0] =	0x02 },	// PLRD13 = 02h
	{ .address = 0x312D, .data[0] =	0x02 },	// PLRD14 = 02h
	{ .address = 0x3125, .data[0] =	0x01 },	// PLRD12 = 01h

	// according to INCK's frequency=24MHz
	{ .address = 0x3120, .data[0] =	0xF0 },	// PLRD1 = 00F0h
	{ .address = 0x3121, .data[0] =	0x00 },
	{ .address = 0x3129, .data[0] =	0x90 },	// PLRD2 = 90h
	{ .address = 0x3122, .data[0] =	0x02 },	// PLRD3 = 02h
	{ .address = 0x312A, .data[0] =	0x02 },	// PLRD4 = 02h

	// PLSTMG setting = Readout Drive Pulse Timing
	{ .address = 0x3003, .data[0] =	0x20 },	// PLSTMG02 = 20h
	{ .address = 0x304E, .data[0] =	0x02 },	// PLSTMG31 = 02h
	{ .address = 0x3057, .data[0] =	0x4A },	// PLSTMG22 = 4Ah
	{ .address = 0x3058, .data[0] =	0xF6 },	// PLSTMG23 = 0F6h
	{ .address = 0x3059, .data[0] =	0x00 },
	{ .address = 0x306B, .data[0] =	0x04 },	// PLSTMG24 = 04h
	{ .address = 0x3145, .data[0] =	0x00 },	// PLSTMG04 = 00h
	{ .address = 0x3202, .data[0] =	0x63 },	// PLSTMG25 = 063h
	{ .address = 0x3203, .data[0] =	0x00 },
	{ .address = 0x3236, .data[0] =	0x64 },	// PLSTMG26 = 064h
	{ .address = 0x3237, .data[0] =	0x00 },
	{ .address = 0x3304, .data[0] =	0x0B },	// PLSTMG32 = 00Bh
	{ .address = 0x3305, .data[0] =	0x00 },
	{ .address = 0x3306, .data[0] =	0x0B },	// PLSTMG33 = 00Bh
	{ .address = 0x3307, .data[0] =	0x00 },
	{ .address = 0x337F, .data[0] =	0x64 },	// PLSTMG27 = 064h
	{ .address = 0x3380, .data[0] =	0x00 },
	{ .address = 0x338D, .data[0] =	0x64 },	// PLSTMG28 = 064h
	{ .address = 0x338E, .data[0] =	0x00 },
	{ .address = 0x3510, .data[0] =	0x72 },	// PLSTMG05 = 072h
	{ .address = 0x3511, .data[0] =	0x00 },
	{ .address = 0x3528, .data[0] =	0x0F },	// PLSTMG06 = 0Fh
	{ .address = 0x3529, .data[0] =	0x0F },	// PLSTMG07 = 0Fh
	{ .address = 0x352A, .data[0] =	0x0F },	// PLSTMG08 = 0Fh
	{ .address = 0x352B, .data[0] =	0x0F },	// PLSTMG09 = 0Fh
	{ .address = 0x3538, .data[0] =	0x0F },	// PLSTMG10 = 0Fh
	{ .address = 0x3539, .data[0] =	0x13 },	// PLSTMG11 = 13h
	{ .address = 0x353C, .data[0] =	0x01 },	// PLSTMG13 = 01h
	{ .address = 0x3553, .data[0] =	0x00 },	// PLSTMG34 = 00h
	{ .address = 0x3554, .data[0] =	0x00 },	// PLSTMG35 = 00h
	{ .address = 0x3555, .data[0] =	0x00 },	// PLSTMG36 = 00h
	{ .address = 0x3556, .data[0] =	0x00 },	// PLSTMG37 = 00h
	{ .address = 0x3557, .data[0] =	0x00 },	// PLSTMG38 = 00h
	{ .address = 0x3558, .data[0] =	0x00 },	// PLSTMG39 = 00h
	{ .address = 0x3559, .data[0] =	0x00 },	// PLSTMG40 = 00h
	{ .address = 0x355A, .data[0] =	0x00 },	// PLSTMG41 = 00h
	{ .address = 0x357D, .data[0] =	0x07 },	// PLSTMG14 = 07h
	{ .address = 0x357F, .data[0] =	0x07 },	// PLSTMG16 = 07h
	{ .address = 0x3580, .data[0] =	0x04 },	// PLSTMG17 = 04h
	{ .address = 0x3583, .data[0] =	0x60 },	// PLSTMG19 = 0h, PLSTMG20 = 6h
	{ .address = 0x3587, .data[0] =	0x01 },	// PLSTMG21 = 01h
	{ .address = 0x3590, .data[0] =	0x0B },	// PLSTMG42 = 00Bh
	{ .address = 0x3591, .data[0] =	0x00 },
	{ .address = 0x35BA, .data[0] =	0x0F },	// PLSTMG43 = 0Fh
	{ .address = 0x366A, .data[0] =	0x0C },	// PLSTMG44 = 0Ch
	{ .address = 0x366B, .data[0] =	0x0B },	// PLSTMG45 = 0Bh
	{ .address = 0x366C, .data[0] =	0x07 },	// PLSTMG46 = 07h
	{ .address = 0x366D, .data[0] =	0x00 },	// PLSTMG47 = 00h
	{ .address = 0x366E, .data[0] =	0x00 },	// PLSTMG48 = 00h
	{ .address = 0x366F, .data[0] =	0x00 },	// PLSTMG49 = 00h
	{ .address = 0x3670, .data[0] =	0x00 },	// PLSTMG50 = 00h
	{ .address = 0x3671, .data[0] =	0x00 },	// PLSTMG51 = 00h
	{ .address = 0x3672, .data[0] =	0x00 },	// PLSTMG52 = 000h
	{ .address = 0x3673, .data[0] =	0x00 },
	{ .address = 0x3674, .data[0] =	0xDF },	// PLSTMG53 = 0DFh
	{ .address = 0x3675, .data[0] =	0x00 },
	{ .address = 0x3676, .data[0] =	0xA7 },	// PLSTMG54 = 1A7h
	{ .address = 0x3677, .data[0] =	0x01 },
	{ .address = 0x3687, .data[0] =	0x00 },	// PLSTMG55 = 00h
	{ .address = 0x375C, .data[0] =	0x02 },	// PLSTMG56 = 02h
	{ .address = 0x380A, .data[0] =	0x0A },	// PLSTMG29 = 0Ah
	{ .address = 0x382B, .data[0] =	0x16 },	// PLSTMG30 = 16h
	
	{ .address = 0x3000, .data[0] =	0x16 },	// STANDBY = 0h, STBLOGIC = 1h, PLSTMG01 = 1h, STBMIPI = 0h, STBDV = 1h
	{ .address = 0x3018, .data[0] =	0xA2 },	// SYNCDRV = 2h		attention!!
	{ .address = 0x3399, .data[0] =	0x01 },	// LMRSVRG = 1h
	{ .address = 0x310B, .data[0] =	0x11 },	// STBPL_F = 1h, STBPL_SD = 1h
	{ .address = 0x3A56, .data[0] =	0x00 },	// MIPIEBD_TAGEN = 0h
	
	/* Wait Sensor Stability (Needs Over 10ms) */
	{ .address = D_TBL_WAIT_MS, .data[0] = 0x0A },

	{ .address = 0x310B, .data[0] =	0x00 },	// STBPL_F = 0h, STBPL_SD = 0h
	
	/*Wait Sensor Stability (Needs Over 0.3ms) */
	{ .address = D_TBL_WAIT_MS, .data[0] = 0x01 },

	{ .address = 0x3000, .data[0] =	0x04 },	// STANDBY = 0h, STBLOGIC = 0h, PLSTMG01 = 1h, STBMIPI = 0h, STBDV = 0h

	/*Wait Sensor Stability (Needs Over 6.7ms) */
	{ .address = D_TBL_WAIT_MS, .data[0] = 0x07 },

	{ .address = 0x3001, .data[0] =	0x10 },	// CLPSQRST = 1
	{ .address = 0x30F4, .data[0] =	0x00 },	// XMSTA = 0

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
};


/*
1. mode0 All-pixel scan mode (12-bit)
*/
const T_CAMERA_SENSOR_REG IMX377_12M_MODE0_4000x3000[] =
{
	/* Mode Setting */		
	{ .address = 0x3004, .data[0] = 0x00 },	// MDSEL1[7~0]=00h
	{ .address = 0x3005, .data[0] = 0x07 },	// MDSEL2[7~0]=07h
	{ .address = 0x3006, .data[0] = 0x00 },	// MDSEL3[7~0]=00h
	{ .address = 0x3007, .data[0] = 0xA0 },	// MDSEL4[7~0]=A0h
	{ .address = 0x3039, .data[0] = 0x00 },	// HTRIMMING_END[12~0]=0000h
	{ .address = 0x303A, .data[0] = 0x00 },
	{ .address = 0x303E, .data[0] = 0x00 },	// HTRIMMING_START[12~0]=0000h
	{ .address = 0x303F, .data[0] = 0x00 },
	{ .address = 0x3040, .data[0] = 0x00 },	// HTRIMMING_EN[0]=0h
	{ .address = 0x3045, .data[0] = 0x40 },	// BLKLEVEL[7~0]=40h
	{ .address = 0x3068, .data[0] = 0x00 },	// MDSEL7[7~0]=00h
	{ .address = 0x307E, .data[0] = 0x00 },	// MDSEL5[15~0]=0000h
	{ .address = 0x307F, .data[0] = 0x00 },
	{ .address = 0x3080, .data[0] = 0x00 },	// MDPLS01[15~0]=0000h
	{ .address = 0x3081, .data[0] = 0x00 },
	{ .address = 0x3082, .data[0] = 0x00 },	// MDPLS02[15~0]=0000h
	{ .address = 0x3083, .data[0] = 0x00 },
	{ .address = 0x3084, .data[0] = 0x00 },	// MDPLS03[15~0]=0000h
	{ .address = 0x3085, .data[0] = 0x00 },
	{ .address = 0x3086, .data[0] = 0x00 },	// MDPLS04[15~0]=0000h
	{ .address = 0x3087, .data[0] = 0x00 },
	{ .address = 0x3095, .data[0] = 0x00 },	// MDPLS05[15~0]=0000h
	{ .address = 0x3096, .data[0] = 0x00 },
	{ .address = 0x3097, .data[0] = 0x00 },	// MDPLS06[15~0]=0000h
	{ .address = 0x3098, .data[0] = 0x00 },
	{ .address = 0x3099, .data[0] = 0x00 },	// MDPLS07[15~0]=0000h
	{ .address = 0x309A, .data[0] = 0x00 },
	{ .address = 0x309B, .data[0] = 0x00 },	// MDPLS08[15~0]=0000h
	{ .address = 0x309C, .data[0] = 0x00 },
	{ .address = 0x30BC, .data[0] = 0x00 },	// MDPLS40[15~0]=0000h
	{ .address = 0x30BD, .data[0] = 0x00 },
	{ .address = 0x30BE, .data[0] = 0x00 },	// MDPLS41[15~0]=0000h
	{ .address = 0x30BF, .data[0] = 0x00 },
	{ .address = 0x30C0, .data[0] = 0x00 },	// MDPLS42[15~0]=0000h
	{ .address = 0x30C1, .data[0] = 0x00 },
	{ .address = 0x30C2, .data[0] = 0x00 },	// MDPLS43[15~0]=0000h
	{ .address = 0x30C3, .data[0] = 0x00 },
	{ .address = 0x30C4, .data[0] = 0x00 },	// MDPLS44[15~0]=0000h
	{ .address = 0x30C5, .data[0] = 0x00 },
	{ .address = 0x30C6, .data[0] = 0x00 },	// MDPLS45[15~0]=0000h
	{ .address = 0x30C7, .data[0] = 0x00 },
	{ .address = 0x30C8, .data[0] = 0x00 },	// MDPLS46[15~0]=0000h
	{ .address = 0x30C9, .data[0] = 0x00 },
	{ .address = 0x30CA, .data[0] = 0x00 },	// MDPLS47[15~0]=0000h
	{ .address = 0x30CB, .data[0] = 0x00 },
	{ .address = 0x30CC, .data[0] = 0x00 },	// MDPLS48[7~0]=00h
	{ .address = 0x30D0, .data[0] = 0x00 },	// MDPLS52[15~0]=0000h
	{ .address = 0x30D1, .data[0] = 0x00 },
	{ .address = 0x30D5, .data[0] = 0x00 },	// VWIDCUTEN[0]=0h
	{ .address = 0x30D6, .data[0] = 0x00 },	// VWIDCUT[11~0]=000h
	{ .address = 0x30D7, .data[0] = 0x00 },
	{ .address = 0x30D8, .data[0] = 0x00 },	// VWINPOS[11~0]=000h
	{ .address = 0x30D9, .data[0] = 0x00 },
	{ .address = 0x30DA, .data[0] = 0x00 },	// MDREG01[7~0]=00h
	{ .address = 0x30EE, .data[0] = 0x01 },	// PSMOVEN[7~0]=01h
	{ .address = 0x30F5, .data[0] = 0x33 },	// HMAX[15~0]=0333h
	{ .address = 0x30F6, .data[0] = 0x03 },
	{ .address = 0x30F7, .data[0] = 0xBF },	// VMAX[19~0]=00DBFh
	{ .address = 0x30F8, .data[0] = 0x0D },
	{ .address = 0x30F9, .data[0] = 0x00 },
	{ .address = 0x312F, .data[0] = 0xF6 },	// MDSEL9[15~0]=0BF6h
	{ .address = 0x3130, .data[0] = 0x0B },
	{ .address = 0x3131, .data[0] = 0xE6 },	// MDSEL10[15~0]=08E6h
	{ .address = 0x3132, .data[0] = 0x08 },
	{ .address = 0x3A41, .data[0] = 0x10 },	// MDSEL11[7~0]=10h

	{ .address = 0x300B, .data[0] =	0x08 },	// Set SUB (SHR) clk number = 8.
	{ .address = 0x300C, .data[0] = 0x00 },
	{ .address = 0x300D, .data[0] =	0x00 },	// Set SVR number = 0.
	{ .address = 0x300E, .data[0] = 0x00 },

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
};

/*
11. mode1 1/2.5 All-pixel scan mode (10-bit)
*/
const T_CAMERA_SENSOR_REG IMX377_9M_MODE1_3840x2160[] =
{	
	/* Mode Setting */		
	{ .address = 0x3004, .data[0] = 0x19 },	// MDSEL1[7~0]=19h
	{ .address = 0x3005, .data[0] = 0x01 },	// MDSEL2[7~0]=01h
	{ .address = 0x3006, .data[0] = 0x04 },	// MDSEL3[7~0]=04h
	{ .address = 0x3007, .data[0] = 0xA0 },	// MDSEL4[7~0]=A0h
	{ .address = 0x3039, .data[0] = 0x00 },	// HTRIMMING_END[12~0]=0000h
	{ .address = 0x303A, .data[0] = 0x00 },
	{ .address = 0x303E, .data[0] = 0x00 },	// HTRIMMING_START[12~0]=0000h
	{ .address = 0x303F, .data[0] = 0x00 },
	{ .address = 0x3040, .data[0] = 0x00 },	// HTRIMMING_EN[0]=0h
	{ .address = 0x3045, .data[0] = 0x40 },	// BLKLEVEL[7~0]=40h
	{ .address = 0x3068, .data[0] = 0x00 },	// MDSEL7[7~0]=00h
	{ .address = 0x307E, .data[0] = 0x00 },	// MDSEL5[15~0]=0800h
	{ .address = 0x307F, .data[0] = 0x08 },
	{ .address = 0x3080, .data[0] = 0x00 },	// MDPLS01[15~0]=0000h
	{ .address = 0x3081, .data[0] = 0x00 },
	{ .address = 0x3082, .data[0] = 0x00 },	// MDPLS02[15~0]=0000h
	{ .address = 0x3083, .data[0] = 0x00 },
	{ .address = 0x3084, .data[0] = 0x00 },	// MDPLS03[15~0]=0000h
	{ .address = 0x3085, .data[0] = 0x00 },
	{ .address = 0x3086, .data[0] = 0x00 },	// MDPLS04[15~0]=0000h
	{ .address = 0x3087, .data[0] = 0x00 },
	{ .address = 0x3095, .data[0] = 0x00 },	// MDPLS05[15~0]=0000h
	{ .address = 0x3096, .data[0] = 0x00 },
	{ .address = 0x3097, .data[0] = 0x00 },	// MDPLS06[15~0]=0000h
	{ .address = 0x3098, .data[0] = 0x00 },
	{ .address = 0x3099, .data[0] = 0x00 },	// MDPLS07[15~0]=0000h
	{ .address = 0x309A, .data[0] = 0x00 },
	{ .address = 0x309B, .data[0] = 0x00 },	// MDPLS08[15~0]=0000h
	{ .address = 0x309C, .data[0] = 0x00 },
	{ .address = 0x30BC, .data[0] = 0x00 },	// MDPLS40[15~0]=0000h
	{ .address = 0x30BD, .data[0] = 0x00 },
	{ .address = 0x30BE, .data[0] = 0x00 },	// MDPLS41[15~0]=0000h
	{ .address = 0x30BF, .data[0] = 0x00 },
	{ .address = 0x30C0, .data[0] = 0x00 },	// MDPLS42[15~0]=0000h
	{ .address = 0x30C1, .data[0] = 0x00 },
	{ .address = 0x30C2, .data[0] = 0x00 },	// MDPLS43[15~0]=0000h
	{ .address = 0x30C3, .data[0] = 0x00 },
	{ .address = 0x30C4, .data[0] = 0x00 },	// MDPLS44[15~0]=0000h
	{ .address = 0x30C5, .data[0] = 0x00 },
	{ .address = 0x30C6, .data[0] = 0x00 },	// MDPLS45[15~0]=0000h
	{ .address = 0x30C7, .data[0] = 0x00 },
	{ .address = 0x30C8, .data[0] = 0x00 },	// MDPLS46[15~0]=0000h
	{ .address = 0x30C9, .data[0] = 0x00 },
	{ .address = 0x30CA, .data[0] = 0x00 },	// MDPLS47[15~0]=0000h
	{ .address = 0x30CB, .data[0] = 0x00 },
	{ .address = 0x30CC, .data[0] = 0x00 },	// MDPLS48[7~0]=00h
	{ .address = 0x30D0, .data[0] = 0x48 },	// MDPLS52[15~0]=0F48h
	{ .address = 0x30D1, .data[0] = 0x0F },
	{ .address = 0x30D5, .data[0] = 0x00 },	// VWIDCUTEN[0]=0h
	{ .address = 0x30D6, .data[0] = 0x00 },	// VWIDCUT[11~0]=000h
	{ .address = 0x30D7, .data[0] = 0x00 },
	{ .address = 0x30D8, .data[0] = 0x00 },	// VWINPOS[11~0]=000h
	{ .address = 0x30D9, .data[0] = 0x00 },
	{ .address = 0x30DA, .data[0] = 0x00 },	// MDREG01[7~0]=00h
	{ .address = 0x30EE, .data[0] = 0x01 },	// PSMOVEN[7~0]=01h
	{ .address = 0x30F5, .data[0] = 0x8A },	// HMAX[15~0]=028Ah
	{ .address = 0x30F6, .data[0] = 0x02 },
	{ .address = 0x30F7, .data[0] = 0x6F },	// VMAX[19~0]=00E6Fh
	{ .address = 0x30F8, .data[0] = 0x0E },
	{ .address = 0x30F9, .data[0] = 0x00 },
	{ .address = 0x312F, .data[0] = 0x86 },	// MDSEL9[15~0]=0886h
	{ .address = 0x3130, .data[0] = 0x08 },
	{ .address = 0x3131, .data[0] = 0x7E },	// MDSEL10[15~0]=087Eh
	{ .address = 0x3132, .data[0] = 0x08 },
	{ .address = 0x3A41, .data[0] = 0x08 },	// MDSEL11[7~0]=08h

	{ .address = 0x300B, .data[0] =	0x08 },	// Set SUB (SHR) clk number = 8.
	{ .address = 0x300C, .data[0] = 0x00 },
	{ .address = 0x300D, .data[0] =	0x00 },	// Set SVR number = 0.
	{ .address = 0x300E, .data[0] = 0x00 },

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
};

/*
12. mode2 1/2.5 Horizontal/Vertical 2/2-line binning
*/
const T_CAMERA_SENSOR_REG IMX377_9M_MODE2_2048x1080[] =
{	
	/* Mode Setting */		
	{ .address = 0x3004, .data[0] = 0xB1 },	// MDSEL1[7~0]=B1h
	{ .address = 0x3005, .data[0] = 0x25 },	// MDSEL2[7~0]=25h
	{ .address = 0x3006, .data[0] = 0x01 },	// MDSEL3[7~0]=01h
	{ .address = 0x3007, .data[0] = 0xA0 },	// MDSEL4[7~0]=A0h
	{ .address = 0x3039, .data[0] = 0x00 },	// HTRIMMING_END[12~0]=0000h
	{ .address = 0x303A, .data[0] = 0x00 },
	{ .address = 0x303E, .data[0] = 0x00 },	// HTRIMMING_START[12~0]=0000h
	{ .address = 0x303F, .data[0] = 0x00 },
	{ .address = 0x3040, .data[0] = 0x00 },	// HTRIMMING_EN[0]=0h
	{ .address = 0x3045, .data[0] = 0x40 },	// BLKLEVEL[7~0]=40h
	{ .address = 0x3068, .data[0] = 0x00 },	// MDSEL7[7~0]=00h
	{ .address = 0x307E, .data[0] = 0x00 },	// MDSEL5[15~0]=0000h
	{ .address = 0x307F, .data[0] = 0x00 },
	{ .address = 0x3080, .data[0] = 0x00 },	// MDPLS01[15~0]=0000h
	{ .address = 0x3081, .data[0] = 0x00 },
	{ .address = 0x3082, .data[0] = 0x00 },	// MDPLS02[15~0]=0000h
	{ .address = 0x3083, .data[0] = 0x00 },
	{ .address = 0x3084, .data[0] = 0x00 },	// MDPLS03[15~0]=0000h
	{ .address = 0x3085, .data[0] = 0x00 },
	{ .address = 0x3086, .data[0] = 0x00 },	// MDPLS04[15~0]=0000h
	{ .address = 0x3087, .data[0] = 0x00 },
	{ .address = 0x3095, .data[0] = 0x00 },	// MDPLS05[15~0]=0000h
	{ .address = 0x3096, .data[0] = 0x00 },
	{ .address = 0x3097, .data[0] = 0x00 },	// MDPLS06[15~0]=0000h
	{ .address = 0x3098, .data[0] = 0x00 },
	{ .address = 0x3099, .data[0] = 0x00 },	// MDPLS07[15~0]=0000h
	{ .address = 0x309A, .data[0] = 0x00 },
	{ .address = 0x309B, .data[0] = 0x00 },	// MDPLS08[15~0]=0000h
	{ .address = 0x309C, .data[0] = 0x00 },
	{ .address = 0x30BC, .data[0] = 0x00 },	// MDPLS40[15~0]=0000h
	{ .address = 0x30BD, .data[0] = 0x00 },
	{ .address = 0x30BE, .data[0] = 0x00 },	// MDPLS41[15~0]=0000h
	{ .address = 0x30BF, .data[0] = 0x00 },
	{ .address = 0x30C0, .data[0] = 0x00 },	// MDPLS42[15~0]=0000h
	{ .address = 0x30C1, .data[0] = 0x00 },
	{ .address = 0x30C2, .data[0] = 0x00 },	// MDPLS43[15~0]=0000h
	{ .address = 0x30C3, .data[0] = 0x00 },
	{ .address = 0x30C4, .data[0] = 0x00 },	// MDPLS44[15~0]=0000h
	{ .address = 0x30C5, .data[0] = 0x00 },
	{ .address = 0x30C6, .data[0] = 0x00 },	// MDPLS45[15~0]=0000h
	{ .address = 0x30C7, .data[0] = 0x00 },
	{ .address = 0x30C8, .data[0] = 0x00 },	// MDPLS46[15~0]=0000h
	{ .address = 0x30C9, .data[0] = 0x00 },
	{ .address = 0x30CA, .data[0] = 0x00 },	// MDPLS47[15~0]=0000h
	{ .address = 0x30CB, .data[0] = 0x00 },
	{ .address = 0x30CC, .data[0] = 0x00 },	// MDPLS48[7~0]=00h
	{ .address = 0x30D0, .data[0] = 0x00 },	// MDPLS52[15~0]=0000h
	{ .address = 0x30D1, .data[0] = 0x00 },
	{ .address = 0x30D5, .data[0] = 0x00 },	// VWIDCUTEN[0]=0h
	{ .address = 0x30D6, .data[0] = 0x00 },	// VWIDCUT[11~0]=000h
	{ .address = 0x30D7, .data[0] = 0x00 },
	{ .address = 0x30D8, .data[0] = 0x00 },	// VWINPOS[11~0]=000h
	{ .address = 0x30D9, .data[0] = 0x00 },
	{ .address = 0x30DA, .data[0] = 0x02 },	// MDREG01[7~0]=02h
	{ .address = 0x30EE, .data[0] = 0x01 },	// PSMOVEN[7~0]=01h
	{ .address = 0x30F5, .data[0] = 0x04 },	// HMAX[15~0]=0104h
	{ .address = 0x30F6, .data[0] = 0x01 },
	{ .address = 0x30F7, .data[0] = 0x05 },	// VMAX[19~0]=00905h
	{ .address = 0x30F8, .data[0] = 0x09 },
	{ .address = 0x30F9, .data[0] = 0x00 },
	{ .address = 0x312F, .data[0] = 0x4A },	// MDSEL9[15~0]=044Ah
	{ .address = 0x3130, .data[0] = 0x04 },
	{ .address = 0x3131, .data[0] = 0x46 },	// MDSEL10[15~0]=0446h
	{ .address = 0x3132, .data[0] = 0x04 },
	{ .address = 0x3A41, .data[0] = 0x04 },	// MDSEL11[7~0]=04h

	{ .address = 0x300B, .data[0] =	0x08 },	// Set SUB (SHR) clk number = 8.
	{ .address = 0x300C, .data[0] = 0x00 },
	{ .address = 0x300D, .data[0] =	0x00 },	// Set SVR number = 0.
	{ .address = 0x300E, .data[0] = 0x00 },

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
};


const T_CAMERA_SENSOR_BASIC		IMX377_BASIC =
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_24MHz,
	.access			= E_CAMERA_ACCESS_I2C,
	.sensor_if		= E_CAMERA_SENSOR_IF_DMIPI,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_VD,
};
const T_CAMERA_SENSOR_MODE		IMX377_MODE[E_SENSOR_IMX377_MODE_MAX] =
{

	[E_SENSOR_IMX377_12M_MODE0_4000x3000] = {
		.mode_reg				= IMX377_12M_MODE0_4000x3000,

		.embedded_lines			= 2,
		.total_pix_width		= 4104,		// 48+14+12+4000+12+18
		.total_pix_lines		= 3064,		// 2+16+6+18+3000+18+4
		.ob_pix_x_pos			= 48+14,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 4104,
		.ob_pix_lines			= 16,
		
		.active_pix_x_pos		= 48+14+12,
		.active_pix_y_pos		= 2+16+6+18,
		.active_pix_width		= 4000,
		.active_pix_lines		= 3000,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.input_bit_width		= 12,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		
		.hdocyc					= 809,		// minV=809, recommened HMAX=819
		.vdocyc					= 3100,		// minV=3100, recommened VMAX=3520
		.exp_clk_offset			= 174,
		.exp_start_line_min		= 8,
		.exp_end_line_offset	= 4,
		.base_SV				= 4.0,

		.Mpps_HDclk				= 72,
		.one_lane_Mbps			= 1152,
		.lane_num				= 4,

		.fps					= 28.71,	// max fps=28.71, target fps=24.98
	},
	[E_SENSOR_IMX377_9M_MODE1_3840x2160] = {
		.mode_reg				= IMX377_9M_MODE1_3840x2160,

		.embedded_lines			= 2,
		.total_pix_width		= 3912,		// 48+0+12+3840+12+0
		.total_pix_lines		= 2184,		// 2+8+6+4+2160+4+0
		.ob_pix_x_pos			= 48+0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 3864,
		.ob_pix_lines			= 8,
		
		.active_pix_x_pos		= 48+0+12,
		.active_pix_y_pos		= 2+8+6+4,
		.active_pix_width		= 3840,
		.active_pix_lines		= 2160,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.input_bit_width		= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		
		.hdocyc					= 650,
		.vdocyc					= 2220,		// minV=2220, recommened VMAX=3696
		.exp_clk_offset			= 117,
		.exp_start_line_min		= 8,
		.exp_end_line_offset	= 4,
		.base_SV				= 4.0,

		.Mpps_HDclk				= 72,
		.one_lane_Mbps			= 1152,
		.lane_num				= 4,

		.fps					= 49.9,	// max fps=49.9, target fps=29.97
	},

	[E_SENSOR_IMX377_9M_MODE2_2048x1080] = {
		.mode_reg				= IMX377_9M_MODE2_2048x1080,

		.embedded_lines			= 2,
		.total_pix_width		= 2100,		// 24+8+6+2048+6+8
		.total_pix_lines		= 1100,		// 2+4+6+4+1080+4+0
		.ob_pix_x_pos			= 24+8,
		.ob_pix_y_pos			= 2+0,
		.ob_pix_width			= 2060,
		.ob_pix_lines			= 4,
		
		.active_pix_x_pos		= 24+8+6,
		.active_pix_y_pos		= 0,//2+4+6+4,	// add extra 20 lines for PIPE
		.active_pix_width		= 2048,
		.active_pix_lines		= 1100,//1080,	// add extra 20 lines for PIPE
		.active_pix_x_rsz_ratio	= 1.0/2.0,
		.active_pix_y_rsz_ratio	= 1.0/2.0,
		.input_bit_width		= 12,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		
		.hdocyc					= 260,
		.vdocyc					= 2228,		// minV=2228, recommened VMAX=2310
		.exp_clk_offset			= 117,
		.exp_start_line_min		= 8,
		.exp_end_line_offset	= 4,
		.base_SV				= 4.0,

		.Mpps_HDclk				= 72,
		.one_lane_Mbps			= 1152,
		.lane_num				= 4,

		.fps					= 124.29,	// max fps=124.29, target fps=119.88
	}

};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX377_Clock_Gpio_Init(VOID)
{
	INT32 sensor_id;

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

		// Release Reset
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xclr, 0 );	// use as GPIO
		Dd_Top_Set_Gpio_Direction( CAMERA.sensor[sensor_id].port_xclr, 1 );	// output
		Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 1 );	// XCLR
		OS_User_Dly_Tsk(1);		// Wait Sensor Stability (Needs Over 100ns)

		// XVS XHS
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xhs, 1 );	// XHS from Sensor
		Dd_Top_Set_Gpio_Direction( CAMERA.sensor[sensor_id].port_xhs, 0 );	// input (for test)
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xvs, 1 );	// XVS from Sensor
		Dd_Top_Set_Gpio_Direction( CAMERA.sensor[sensor_id].port_xvs, 0 );	// input (for test)
		
		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_inck, 1 );	// CLK from Sensor

		// init setting
		if( NULL != CAMERA.sensor[sensor_id].Func_Init ){
			if( CAMERA.sensor[sensor_id].Func_Init(sensor_id) == E_CAMERA_ERR_NG ){
				CAMERA_PRINT_ERROR(1);
				printf( "Sensor %d NOT connected\n", sensor_id );
			}
		}
	}
	
	/* Sensor Master Clock Start */
	// CAMERA.sensor[0].basic->input_clock = E_CAMERA_SENSOR_INCLK_24MHz
	// x = 40 / A * (N + P / Q) / B [MHz]
	Dd_Top_Set_PLLCNT3_P00PREDIV(1);	// A (1-4(reg val=0-3))
	Dd_Top_Set_PLLCNT4_P00FNUM(0);		// P
	Dd_Top_Set_PLLCNT5_P00FDEN(0);		// Q (P <= Q.if P>Q, P/Q become 0)
	Dd_Top_Set_PLLCNT3_P00PLLDIV(30);	// * val
	Dd_Top_Set_PLLCNT3_P00POSTDIV(24);	// / (val+1)
	Dd_Top_Start_Pll00();				// PLL00 Start


	/* Setting drivability 8mA (SNAPCLKOO) */
	Dd_Top_Set_DBCNT1_T11DC( 3 );

	/* set APCLK */
	Dd_Top_Set_CLKSEL8_APCLK( 4 );				// 72MHz (is it needed?)

	Dd_Top_Set_CLKSEL8_AP0SEL( 0 );				// SNAPCLKO0 use PLL00(fractional)
	Dd_Top_Set_CLKSTOP4_APCK0( 0 );				// APCK0 Start
			
	return E_CAMERA_ERR_OK;
}



E_CAMERA_ERR_CODE Sensor_IMX377_Standby(E_CAMERA_ID sensor_id)
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

E_CAMERA_ERR_CODE Sensor_IMX377_Stream(E_CAMERA_ID sensor_id)
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

E_CAMERA_ERR_CODE Sensor_IMX377_CancelStandby(E_CAMERA_ID sensor_id)
{
	return access_tbl(sensor_id, IMX377_INIT);
}

E_CAMERA_ERR_CODE Sensor_IMX377_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol)
{
	return dol ? access_tbl(sensor_id, IMX377_MODE[mode].dol_mode_reg) : access_tbl(sensor_id, IMX377_MODE[mode].mode_reg);
}

E_CAMERA_ERR_CODE Sensor_IMX377_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
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
	
	FLOAT	again_dB, dgain_dB;
	FLOAT	work;
	USHORT	PGC;

	FLOAT req_exp_msec = 16.666;
	FLOAT req_gain_times = 1.0;
	
	// --------------------
	// Exposur Time
	// --------------------
	// Time = [ { vdocyc * (SVR+1) - SHR } * hdocyc + offset ] / INCK
	// -> (Time * INCK - offset ) / hdocyc = vdocyc - SHR (SVR=0 fixed)
	// -> (Time * INCK - offset ) / hdocyc ... (a)
	// -> vdocyc - SHR                     ... (b)
	
	req_exp_msec = in->exp_msec;
	req_gain_times = in->gain_times;
	
	// req_vdocyc = (a)
	req_vdocyc = ( (req_exp_msec / 1000.0) * (IMX377_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) - (FLOAT)IMX377_MODE[mode].exp_clk_offset) / (FLOAT)IMX377_MODE[mode].hdocyc;
	if( req_vdocyc < 0.0 ){
		req_vdocyc = 0.0;
	}
	
	// (a) = (b)
	// -> req_vdocyc = vdocyc - SHR
	// -> SHR = vdocyc - req_vdocyc
	req_SHR = (FLOAT)IMX377_MODE[mode].vdocyc - req_vdocyc;
	
	// find vdocyc and SHR combination
	if( req_SHR < (FLOAT)IMX377_MODE[mode].exp_start_line_min ){
		// extend vdocyc
		real_SHR = IMX377_MODE[mode].exp_start_line_min;
		real_vdocyc = (USHORT)( req_vdocyc + (FLOAT)IMX377_MODE[mode].exp_start_line_min );
	}
	else if( (IMX377_MODE[mode].vdocyc - IMX377_MODE[mode].exp_end_line_offset) < req_SHR ){
		// clip
		real_SHR = (IMX377_MODE[mode].vdocyc - IMX377_MODE[mode].exp_end_line_offset);
		real_vdocyc = IMX377_MODE[mode].vdocyc;
	}
	else{
		real_SHR = (USHORT)req_SHR;
		real_vdocyc = IMX377_MODE[mode].vdocyc;
	}
	
	// FPS
	if( D_CAMERA_FPS_AUTO == in->min_fps && D_CAMERA_FPS_AUTO == in->max_fps ){
		// nothing to do
	}
	else{
		// default
		defaultFps = (IMX377_MODE[mode].Mpps_HDclk  * 1000.0 * 1000.0) / IMX377_MODE[mode].hdocyc / IMX377_MODE[mode].vdocyc;
		
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
		reqFps = (IMX377_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX377_MODE[mode].hdocyc / (FLOAT)real_vdocyc;
		if( reqFps < minFps ){
			req_vdocyc   = (IMX377_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX377_MODE[mode].hdocyc / minFps;
			dif_vdocyc   = req_vdocyc - (FLOAT)real_vdocyc;
			real_vdocyc  = (USHORT)req_vdocyc;
		}
		if( reqFps > maxFps ){
			req_vdocyc   = (IMX377_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX377_MODE[mode].hdocyc / maxFps;
			dif_vdocyc   = req_vdocyc - (FLOAT)real_vdocyc;
			real_vdocyc  = (USHORT)req_vdocyc;
		}
		
		// SHR
		if( dif_vdocyc != 0.0 ){
			req_SHR = (FLOAT)real_SHR + dif_vdocyc;
			
			if( req_SHR < (FLOAT)IMX377_MODE[mode].exp_start_line_min ){
				req_SHR = (FLOAT)IMX377_MODE[mode].exp_start_line_min;
			}
			if( real_vdocyc - (FLOAT)IMX377_MODE[mode].exp_end_line_offset < req_SHR ){
				req_SHR = real_vdocyc - (FLOAT)IMX377_MODE[mode].exp_end_line_offset;
			}
			
			real_SHR = (USHORT)req_SHR;
		}
	}
	
	if (real_SHR > (USHORT)(real_vdocyc-4))
		real_SHR = (USHORT)(real_vdocyc-4);

	// real
	realExpTime = ( ((FLOAT)real_vdocyc - (FLOAT)real_SHR) * (FLOAT)IMX377_MODE[mode].hdocyc + IMX377_MODE[mode].exp_clk_offset) / (IMX377_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	realFps = (IMX377_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / real_vdocyc / IMX377_MODE[mode].hdocyc;
	
	// Out
	out->fps = realFps;
	out->exp_msec = realExpTime * 1000.0;
	out->hdocyc = IMX377_MODE[mode].hdocyc;
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



E_CAMERA_ERR_CODE Sensor_IMX377_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in)
{
	UCHAR data[8];

	// group param hold ON
	// set REGHOLD = 1h
	
	data[0] = 0x01;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x302D,
							data,
							1);

	// SHR
	data[1] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_EXP] >> 8) & 0xFF);
	data[0] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_EXP]       & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x300B,
							data,
							2);
	
	// set PGC
	data[1] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_AGAIN] >> 8) & 0xFF);
	data[0] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_AGAIN]       & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3009,
							data,
							2);

	// digital gain
	data[0] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_DGAIN]       & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3011,
							data,
							1);

	// group param hold OFF
	data[0] = 0;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x302D,
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
