/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * @author		KGDICFFQP
 * @time		2020.01.20
 * <B><I>Copyright 2020 XAG Inc.</I></B>
 */

#include "camera_sensor_imx571.h"
#include "camera_config.h"
#include "os_user_custom.h"
#include "im_pro.h"
#include <math.h>
#include "share_data.h"
#include "dd_tmr32.h"
#include "camera_use.h"
#include "category_parameter.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	D_TBL_END		(0xFFFF)
#define	D_TBL_WAIT_MS	(0xFFFE)



/*global shutter.add by liuxin 20210816*/
#ifdef IMX571_GLOBAL_SHUTTER
#define D_IMX571_MSHUTTER_CLOSE_BY_TIMER
#define D_IMX571_MSHUTTER_TIMER_ID0_CH		(7)
#define D_IMX571_MSHUTTER_STANDBY_ID0_CH		(6)
#define D_IMX571_CALLBACK_TIMER_ID0_CH      (2)

/**************************************************
*imx571 M-shutter
*when exptime=8.33ms,M-shutter set off 12.3ms in advance
* M-shutter will set 4.7ms in addvance
**************************************************/
#define D_IMX571_MSHUTTER_OFS_US			(15000)//16300
#define D_IMX571_MSHUTTER_SFT_US			10300//(10600) //fixed ,do not modify!!! 4.7ms before
#define D_IMX571_MSHUTTER_DELAY_US			(D_IMX571_MSHUTTER_OFS_US - D_IMX571_MSHUTTER_SFT_US)
// #define D_IMX571_GPS_EVENT_PIN				FJ_GPIO_P77

/***********************************************
PY0 for m-shutter voltage select pin
(1):PY0=0,Vol=3.0V A=180mA for shutter open
(2):PY0=1,Vol=3.4V A=255mA for shutter close
************************************************/
#define D_IMX571_MSHUUTER_VOLSET_PIN		FJ_GPIO_PY0 
#endif

static float g_exp_time_571 = 0;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef union {
	USHORT	hword;
	UCHAR	byte[2];
} U_REG;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
const T_CAMERA_SENSOR_REG	IMX571_STANDBY_RELEASE[] = 
{
	//standby release /normal operation
	{ .address = 0x0051, .data[0] = 0x08 },
	{ .address = 0x01EE, .data[0] = 0x01 },
	{ .address = 0x0000, .data[0] = 0x04 },

	//{ .address = D_TBL_WAIT_MS, .data[0] = 0x10 },//Wait>16ms  
	{ .address = D_TBL_END, .data[0] = 0xFF }// END
};

//readout mode0-c //6244x4168 fps=37.54
const T_CAMERA_SENSOR_REG		IMX571_MODE0C_V1H1[] = 
{

	{ .address = 0x0000, .data[0] =	0x04 },
	{ .address = 0x0001, .data[0] =	0x00 },
	{ .address = 0x0002, .data[0] =	0x54 },
	{ .address = 0x0003, .data[0] =	0x10 },
	{ .address = 0x0007, .data[0] =	0x00 },
	{ .address = 0x0008, .data[0] =	0x19 },
	{ .address = 0x0009, .data[0] =	0x00 },
	{ .address = 0x000A, .data[0] =	0x50 },
	{ .address = 0x000B, .data[0] =	0x10 },
	{ .address = 0x000E, .data[0] =	0x18 },
	{ .address = 0x0018, .data[0] =	0x01 },
	{ .address = 0x0027, .data[0] =	0x06 },
	{ .address = 0x002A, .data[0] =	0x0A },
	{ .address = 0x0051, .data[0] =	0x08 },
	{ .address = 0x0054, .data[0] =	0xBD },
	{ .address = 0x0055, .data[0] =	0x00 },
	{ .address = 0x005A, .data[0] =	0x96 },
	{ .address = 0x005B, .data[0] =	0x00 },
	{ .address = 0x005C, .data[0] =	0xC7 },
	{ .address = 0x005D, .data[0] =	0x00 },
	{ .address = 0x0062, .data[0] =	0xEC },
	{ .address = 0x0063, .data[0] =	0x01 },
	{ .address = 0x0065, .data[0] =	0x00 },
	{ .address = 0x0069, .data[0] =	0x00 },
	{ .address = 0x006C, .data[0] =	0x22 },
	{ .address = 0x006D, .data[0] =	0x01 },
	{ .address = 0x00A1, .data[0] =	0x00 },
	{ .address = 0x00A7, .data[0] =	0x00 },
	{ .address = 0x00A8, .data[0] =	0x00 },
	{ .address = 0x00D3, .data[0] =	0x08 },
	{ .address = 0x00D6, .data[0] =	0x53 },
	{ .address = 0x00D8, .data[0] =	0x53 },
	{ .address = 0x00DB, .data[0] =	0x5A },
	{ .address = 0x00DD, .data[0] =	0x5A },
	{ .address = 0x00DF, .data[0] =	0x00 },
	{ .address = 0x00E2, .data[0] =	0x88 },

	//add by XAG for SLVS-EC
	{ .address = 0x0113 , .data[0] = 0x01 },//SLVS-EC:CRC Enable
	{ .address = 0x0119,  .data[0] = 0x20 },//SLVS-EC:Deskew Length
	{ .address = 0x011A , .data[0] = 0x14 },//SLVS-EC:Deakew Interval
	{ .address = 0x011B , .data[0] = 0x20 },//SLVS-EC:Standby length
	{ .address = 0x011E , .data[0] = 0x60 },//SLVS-EC:Deakew symbol

	
	{ .address = 0x01D8, .data[0] =	0x00 },
	{ .address = 0x01D9, .data[0] =	0x00 },
	{ .address = 0x01DA, .data[0] =	0x00 },
	{ .address = 0x01DD, .data[0] =	0x00 },
	{ .address = 0x01DE, .data[0] =	0x00 },
	{ .address = 0x01EE, .data[0] =	0x01 },
	{ .address = 0x02D3, .data[0] =	0x00 },
	{ .address = 0x0400, .data[0] =	0x0E },
	{ .address = 0x040E, .data[0] =	0x06 },
	{ .address = 0x0410, .data[0] =	0x06 },
	{ .address = 0x041F, .data[0] =	0x66 },
	{ .address = 0x0422, .data[0] =	0x06 },
	{ .address = 0x0425, .data[0] =	0x7B },
	{ .address = 0x0454, .data[0] =	0x22 },
	{ .address = 0x0456, .data[0] =	0x22 },
	{ .address = 0x050E, .data[0] =	0x32 },
	{ .address = 0x050F, .data[0] =	0x59 },
	{ .address = 0x0512, .data[0] =	0xBF },
	{ .address = 0x0513, .data[0] =	0xBF },
	{ .address = 0x0514, .data[0] =	0xBF },
	{ .address = 0x0515, .data[0] =	0x00 },
	{ .address = 0x0516, .data[0] =	0x32 },
	{ .address = 0x0517, .data[0] =	0x50 },
	{ .address = 0x051E, .data[0] =	0x23 },
	{ .address = 0x051F, .data[0] =	0x5F },
	{ .address = 0x0553, .data[0] =	0x7B },
	{ .address = 0x0559, .data[0] =	0x19 },
	{ .address = 0x055A, .data[0] =	0x17 },
	{ .address = 0x055B, .data[0] =	0x02 },
	{ .address = 0x055C, .data[0] =	0x19 },
	{ .address = 0x055D, .data[0] =	0x17 },
	{ .address = 0x055E, .data[0] =	0x02 },
	{ .address = 0x055F, .data[0] =	0x20 },
	{ .address = 0x0560, .data[0] =	0x1E },
	{ .address = 0x0561, .data[0] =	0x02 },
	{ .address = 0x0562, .data[0] =	0x20 },
	{ .address = 0x0563, .data[0] =	0x1E },
	{ .address = 0x056B, .data[0] =	0x27 },
	{ .address = 0x056C, .data[0] =	0x25 },
	{ .address = 0x056E, .data[0] =	0x20 },
	{ .address = 0x056F, .data[0] =	0x1E },
	{ .address = 0x0573, .data[0] =	0x00 },
	{ .address = 0x0574, .data[0] =	0x0F },
	{ .address = 0x0575, .data[0] =	0x0F },
	{ .address = 0x0576, .data[0] =	0x0F },
	{ .address = 0x0577, .data[0] =	0x0F },
	{ .address = 0x0581, .data[0] =	0x04 },
	{ .address = 0x0582, .data[0] =	0x24 },
	{ .address = 0x0583, .data[0] =	0x24 },
	{ .address = 0x0584, .data[0] =	0x24 },
	{ .address = 0x0585, .data[0] =	0x24 },
	{ .address = 0x0590, .data[0] =	0x01 },
	{ .address = 0x0596, .data[0] =	0x19 },
	{ .address = 0x0597, .data[0] =	0x14 },
	{ .address = 0x0598, .data[0] =	0x20 },
	{ .address = 0x0599, .data[0] =	0x1B },
	{ .address = 0x059A, .data[0] =	0x04 },
	{ .address = 0x0600, .data[0] =	0x1C },
	{ .address = 0x0603, .data[0] =	0x4B },
	{ .address = 0x0605, .data[0] =	0x4B },
	{ .address = 0x062A, .data[0] =	0x81 },
	{ .address = 0x062C, .data[0] =	0x52 },
	{ .address = 0x0630, .data[0] =	0x7F },
	{ .address = 0x0632, .data[0] =	0x52 },
	{ .address = 0x0635, .data[0] =	0x19 },
	{ .address = 0x0636, .data[0] =	0x15 },
	{ .address = 0x0637, .data[0] =	0x20 },
	{ .address = 0x0638, .data[0] =	0x15 },
	{ .address = 0x063A, .data[0] =	0x19 },
	{ .address = 0x063B, .data[0] =	0x15 },
	{ .address = 0x063C, .data[0] =	0x20 },
	{ .address = 0x063D, .data[0] =	0x15 },
	{ .address = 0x063F, .data[0] =	0x19 },
	{ .address = 0x0640, .data[0] =	0x15 },
	{ .address = 0x0641, .data[0] =	0x20 },
	{ .address = 0x0642, .data[0] =	0x15 },
	{ .address = 0x0646, .data[0] =	0x7B },
	{ .address = 0x064A, .data[0] =	0x7B },
	{ .address = 0x066D, .data[0] =	0x00 },
	{ .address = 0x066E, .data[0] =	0x11 },
	{ .address = 0x0670, .data[0] =	0x00 },
	{ .address = 0x0671, .data[0] =	0x11 },
	{ .address = 0x0673, .data[0] =	0x00 },
	{ .address = 0x0674, .data[0] =	0x11 },
	{ .address = 0x0676, .data[0] =	0x00 },
	{ .address = 0x0677, .data[0] =	0x11 },
	{ .address = 0x067E, .data[0] =	0x04 },
	{ .address = 0x068A, .data[0] =	0x88 },
	{ .address = 0x07CC, .data[0] =	0x0A },
	{ .address = 0x0A2F, .data[0] =	0x95 },
	{ .address = 0x0A30, .data[0] =	0x00 },
	{ .address = 0x0A31, .data[0] =	0x95 },
	{ .address = 0x0A32, .data[0] =	0x00 },
	{ .address = 0x0A36, .data[0] =	0x95 },
	{ .address = 0x0A37, .data[0] =	0x00 },
	{ .address = 0x0A66, .data[0] =	0x00 },
	{ .address = 0x0A75, .data[0] =	0x00 },
	{ .address = 0x0A76, .data[0] =	0x00 },
	{ .address = 0x0A77, .data[0] =	0x00 },
	{ .address = 0x0A78, .data[0] =	0x00 },
	{ .address = 0x0A79, .data[0] =	0x00 },
	{ .address = 0x0A7A, .data[0] =	0x00 },
	{ .address = 0x0A7B, .data[0] =	0x00 },
                                     
	{ .address = D_TBL_END, .data[0] = 0xFF}// END

};

//readout mode0-e//10bit //6244x4168 fps=37.54
const T_CAMERA_SENSOR_REG IMX571_MODE0E_V1H1[] = 
{

	{ .address = 0x0000 , .data[0] = 0x04 },
	{ .address = 0x0001 , .data[0] = 0x00 },
	{ .address = 0x0002 , .data[0] = 0x3D },
	{ .address = 0x0003 , .data[0] = 0x10 },
	{ .address = 0x0007 , .data[0] = 0x00 },
	{ .address = 0x0008 , .data[0] = 0x19 },
	{ .address = 0x0009 , .data[0] = 0x00 },
	{ .address = 0x000A , .data[0] = 0x50 },
	{ .address = 0x000B , .data[0] = 0x10 },
	{ .address = 0x000E , .data[0] = 0x18 },
	{ .address = 0x0018 , .data[0] = 0x01 },
	{ .address = 0x0027 , .data[0] = 0x06 },
	{ .address = 0x002A , .data[0] = 0x0A },
	{ .address = 0x0051 , .data[0] = 0x08 },
	{ .address = 0x0054 , .data[0] = 0xBD },
	{ .address = 0x0055 , .data[0] = 0x00 },
	{ .address = 0x005A , .data[0] = 0x96 },
	{ .address = 0x005B , .data[0] = 0x00 },
	{ .address = 0x005C , .data[0] = 0xC7 },
	{ .address = 0x005D , .data[0] = 0x00 },
	{ .address = 0x0062 , .data[0] = 0xEC },
	{ .address = 0x0063 , .data[0] = 0x01 },
	{ .address = 0x0065 , .data[0] = 0x00 },
	{ .address = 0x0069 , .data[0] = 0x00 },
	{ .address = 0x006C , .data[0] = 0x22 },
	{ .address = 0x006D , .data[0] = 0x01 },
	{ .address = 0x00A1 , .data[0] = 0x00 },
	{ .address = 0x00A7 , .data[0] = 0x00 },
	{ .address = 0x00A8 , .data[0] = 0x00 },
	{ .address = 0x00D3 , .data[0] = 0x08 },
	{ .address = 0x00D6 , .data[0] = 0x44 },
	{ .address = 0x00D8 , .data[0] = 0x44 },
	{ .address = 0x00DB , .data[0] = 0x4B },
	{ .address = 0x00DD , .data[0] = 0x4B },
	{ .address = 0x00DF , .data[0] = 0x00 },
	{ .address = 0x00E2 , .data[0] = 0x88 },

	//add by XAG for SLVS-EC
	{ .address = 0x0113 , .data[0] = 0x01 },//SLVS-EC:CRC Enable
	{ .address = 0x0119,  .data[0] = 0x10 },//SLVS-EC:Deskew Length
	{ .address = 0x011A , .data[0] = 0x14 },//SLVS-EC:Deakew Interval
	{ .address = 0x011B , .data[0] = 0x20 },//SLVS-EC:Standby length
	{ .address = 0x011E , .data[0] = 0x60 },//SLVS-EC:Deakew symbol
	
	{ .address = 0x01D8 , .data[0] = 0x00 },
	{ .address = 0x01D9 , .data[0] = 0x00 },
	{ .address = 0x01DA , .data[0] = 0x00 },
	{ .address = 0x01DD , .data[0] = 0x00 },
	{ .address = 0x01DE , .data[0] = 0x00 },
	{ .address = 0x01EE , .data[0] = 0x01 },
	{ .address = 0x02D3 , .data[0] = 0x00 },
	{ .address = 0x0400 , .data[0] = 0x0E },
	{ .address = 0x040E , .data[0] = 0x08 },
	{ .address = 0x0410 , .data[0] = 0x08 },
	{ .address = 0x041F , .data[0] = 0x86 },
	{ .address = 0x0422 , .data[0] = 0x08 },
	{ .address = 0x0425 , .data[0] = 0x6C },
	{ .address = 0x0454 , .data[0] = 0x22 },
	{ .address = 0x0456 , .data[0] = 0x22 },
	{ .address = 0x050E , .data[0] = 0x28 },
	{ .address = 0x050F , .data[0] = 0x4A },
	{ .address = 0x0512 , .data[0] = 0xD2 },
	{ .address = 0x0513 , .data[0] = 0xD2 },
	{ .address = 0x0514 , .data[0] = 0xD2 },
	{ .address = 0x0515 , .data[0] = 0x00 },
	{ .address = 0x0516 , .data[0] = 0x28 },
	{ .address = 0x0517 , .data[0] = 0x41 },
	{ .address = 0x051E , .data[0] = 0x19 },
	{ .address = 0x051F , .data[0] = 0x50 },
	{ .address = 0x0553 , .data[0] = 0x6C },
	{ .address = 0x0559 , .data[0] = 0x19 },
	{ .address = 0x055A , .data[0] = 0x17 },
	{ .address = 0x055B , .data[0] = 0x02 },
	{ .address = 0x055C , .data[0] = 0x19 },
	{ .address = 0x055D , .data[0] = 0x17 },
	{ .address = 0x055E , .data[0] = 0x02 },
	{ .address = 0x055F , .data[0] = 0x20 },
	{ .address = 0x0560 , .data[0] = 0x1E },
	{ .address = 0x0561 , .data[0] = 0x02 },
	{ .address = 0x0562 , .data[0] = 0x20 },
	{ .address = 0x0563 , .data[0] = 0x1E },
	{ .address = 0x056B , .data[0] = 0x27 },
	{ .address = 0x056C , .data[0] = 0x25 },
	{ .address = 0x056E , .data[0] = 0x20 },
	{ .address = 0x056F , .data[0] = 0x1E },
	{ .address = 0x0573 , .data[0] = 0x00 },
	{ .address = 0x0574 , .data[0] = 0x0A },
	{ .address = 0x0575 , .data[0] = 0x0A },
	{ .address = 0x0576 , .data[0] = 0x0A },
	{ .address = 0x0577 , .data[0] = 0x0A },
	{ .address = 0x0581 , .data[0] = 0x05 },
	{ .address = 0x0582 , .data[0] = 0x22 },
	{ .address = 0x0583 , .data[0] = 0x22 },
	{ .address = 0x0584 , .data[0] = 0x22 },
	{ .address = 0x0585 , .data[0] = 0x22 },
	{ .address = 0x0590 , .data[0] = 0x01 },
	{ .address = 0x0596 , .data[0] = 0x19 },
	{ .address = 0x0597 , .data[0] = 0x14 },
	{ .address = 0x0598 , .data[0] = 0x20 },
	{ .address = 0x0599 , .data[0] = 0x1B },
	{ .address = 0x059A , .data[0] = 0x04 },
	{ .address = 0x0600 , .data[0] = 0x1C },
	{ .address = 0x0603 , .data[0] = 0x3C },
	{ .address = 0x0605 , .data[0] = 0x3C },
	{ .address = 0x062A , .data[0] = 0x72 },
	{ .address = 0x062C , .data[0] = 0x34 },
	{ .address = 0x0630 , .data[0] = 0x70 },
	{ .address = 0x0632 , .data[0] = 0x34 },
	{ .address = 0x0635 , .data[0] = 0x19 },
	{ .address = 0x0636 , .data[0] = 0x15 },
	{ .address = 0x0637 , .data[0] = 0x20 },
	{ .address = 0x0638 , .data[0] = 0x15 },
	{ .address = 0x063A , .data[0] = 0x19 },
	{ .address = 0x063B , .data[0] = 0x15 },
	{ .address = 0x063C , .data[0] = 0x20 },
	{ .address = 0x063D , .data[0] = 0x15 },
	{ .address = 0x063F , .data[0] = 0x19 },
	{ .address = 0x0640 , .data[0] = 0x15 },
	{ .address = 0x0641 , .data[0] = 0x20 },
	{ .address = 0x0642 , .data[0] = 0x15 },
	{ .address = 0x0646 , .data[0] = 0x6C },
	{ .address = 0x064A , .data[0] = 0x6C },
	{ .address = 0x066D , .data[0] = 0x00 },
	{ .address = 0x066E , .data[0] = 0x00 },
	{ .address = 0x0670 , .data[0] = 0x00 },
	{ .address = 0x0671 , .data[0] = 0x00 },
	{ .address = 0x0673 , .data[0] = 0x00 },
	{ .address = 0x0674 , .data[0] = 0x00 },
	{ .address = 0x0676 , .data[0] = 0x00 },
	{ .address = 0x0677 , .data[0] = 0x00 },
	{ .address = 0x067E , .data[0] = 0x04 },
	{ .address = 0x068A , .data[0] = 0x88 },
	{ .address = 0x07CC , .data[0] = 0x0A },
	{ .address = 0x0A2F , .data[0] = 0x95 },
	{ .address = 0x0A30 , .data[0] = 0x00 },
	{ .address = 0x0A31 , .data[0] = 0x95 },
	{ .address = 0x0A32 , .data[0] = 0x00 },
	{ .address = 0x0A36 , .data[0] = 0x95 },
	{ .address = 0x0A37 , .data[0] = 0x00 },
	{ .address = 0x0A66 , .data[0] = 0x00 },
	{ .address = 0x0A75 , .data[0] = 0x00 },
	{ .address = 0x0A76 , .data[0] = 0x00 },
	{ .address = 0x0A77 , .data[0] = 0x00 },
	{ .address = 0x0A78 , .data[0] = 0x00 },
	{ .address = 0x0A79 , .data[0] = 0x00 },
	{ .address = 0x0A7A , .data[0] = 0x00 },
	{ .address = 0x0A7B , .data[0] = 0x00 },
																																																																					                                        
	{ .address = D_TBL_END, .data[0] = 0xFF}// END

};


//readout mode5-c//3122x2084 fps=37.55
const T_CAMERA_SENSOR_REG		IMX571_MODE5C_V2H2[] = 
{
	{ .address = 0x0000, .data[0] = 0x04 },
	{ .address = 0x0001, .data[0] = 0x05 },
	{ .address = 0x0002, .data[0] = 0x54 },
	{ .address = 0x0003, .data[0] = 0x10 },
	{ .address = 0x0018, .data[0] = 0x01 },
	{ .address = 0x0027, .data[0] = 0x06 },
	{ .address = 0x0051, .data[0] = 0x08 },
	{ .address = 0x00D3, .data[0] = 0x08 },
	{ .address = 0x00D6, .data[0] = 0x53 },
	{ .address = 0x00D8, .data[0] = 0x53 },
	{ .address = 0x00DB, .data[0] = 0x5A },
	{ .address = 0x00DD, .data[0] = 0x5A },
	{ .address = 0x00DF, .data[0] = 0x00 },
	{ .address = 0x00E2, .data[0] = 0x88 },
	{ .address = 0x01EE, .data[0] = 0x01 },
	{ .address = 0x0400, .data[0] = 0x0E },
	{ .address = 0x0454, .data[0] = 0x22 },
	{ .address = 0x0456, .data[0] = 0x22 },
	{ .address = 0x050F, .data[0] = 0x59 },
	{ .address = 0x0512, .data[0] = 0xBF },
	{ .address = 0x0513, .data[0] = 0xBF },
	{ .address = 0x0514, .data[0] = 0xBF },
	{ .address = 0x0515, .data[0] = 0x00 },
	{ .address = 0x0517, .data[0] = 0x50 },
	{ .address = 0x051F, .data[0] = 0x5F },
	{ .address = 0x0553, .data[0] = 0x7B },
	{ .address = 0x0559, .data[0] = 0x19 },
	{ .address = 0x055A, .data[0] = 0x17 },
	{ .address = 0x055C, .data[0] = 0x19 },
	{ .address = 0x055D, .data[0] = 0x17 },
	{ .address = 0x055F, .data[0] = 0x20 },
	{ .address = 0x0560, .data[0] = 0x1E },
	{ .address = 0x0562, .data[0] = 0x20 },
	{ .address = 0x0563, .data[0] = 0x1E },
	{ .address = 0x056B, .data[0] = 0x27 },
	{ .address = 0x056C, .data[0] = 0x25 },
	{ .address = 0x056E, .data[0] = 0x20 },
	{ .address = 0x056F, .data[0] = 0x1E },
	{ .address = 0x0573, .data[0] = 0x00 },
	{ .address = 0x0574, .data[0] = 0x0F },
	{ .address = 0x0575, .data[0] = 0x0F },
	{ .address = 0x0576, .data[0] = 0x0F },
	{ .address = 0x0577, .data[0] = 0x0F },
	{ .address = 0x0581, .data[0] = 0x04 },
	{ .address = 0x0582, .data[0] = 0x24 },
	{ .address = 0x0583, .data[0] = 0x24 },
	{ .address = 0x0584, .data[0] = 0x24 },
	{ .address = 0x0585, .data[0] = 0x24 },
	{ .address = 0x0590, .data[0] = 0x01 },
	{ .address = 0x0596, .data[0] = 0x19 },
	{ .address = 0x0597, .data[0] = 0x14 },
	{ .address = 0x0598, .data[0] = 0x20 },
	{ .address = 0x0599, .data[0] = 0x1B },
	{ .address = 0x0600, .data[0] = 0x1C },
	{ .address = 0x0603, .data[0] = 0x4B },
	{ .address = 0x0605, .data[0] = 0x4B },
	{ .address = 0x062A, .data[0] = 0x81 },
	{ .address = 0x062C, .data[0] = 0x52 },
	{ .address = 0x0630, .data[0] = 0x7F },
	{ .address = 0x0632, .data[0] = 0x52 },
	{ .address = 0x0635, .data[0] = 0x19 },
	{ .address = 0x0636, .data[0] = 0x15 },
	{ .address = 0x0637, .data[0] = 0x20 },
	{ .address = 0x0638, .data[0] = 0x15 },
	{ .address = 0x063A, .data[0] = 0x19 },
	{ .address = 0x063B, .data[0] = 0x15 },
	{ .address = 0x063C, .data[0] = 0x20 },
	{ .address = 0x063D, .data[0] = 0x15 },
	{ .address = 0x063F, .data[0] = 0x19 },
	{ .address = 0x0640, .data[0] = 0x15 },
	{ .address = 0x0641, .data[0] = 0x20 },
	{ .address = 0x0642, .data[0] = 0x15 },
	{ .address = 0x0646, .data[0] = 0x7B },
	{ .address = 0x064A, .data[0] = 0x7B },
	{ .address = 0x066D, .data[0] = 0x00 },
	{ .address = 0x066E, .data[0] = 0x11 },
	{ .address = 0x0670, .data[0] = 0x00 },
	{ .address = 0x0671, .data[0] = 0x11 },
	{ .address = 0x0673, .data[0] = 0x00 },
	{ .address = 0x0674, .data[0] = 0x11 },
	{ .address = 0x0676, .data[0] = 0x00 },
	{ .address = 0x0677, .data[0] = 0x11 },
	{ .address = 0x067E, .data[0] = 0x04 },
	{ .address = 0x068A, .data[0] = 0x88 },
	{ .address = 0x07CC, .data[0] = 0x0A },

	
	{ .address = 0x0113, .data[0] = 0x01 },//SLVS-EC:CRC Enable
	{ .address = 0x0119, .data[0] = 0x10 },//SLVS-EC:Deskew Length
	{ .address = 0x011A, .data[0] = 0x14 },//SLVS-EC:Deskew Interval
	{ .address = 0x011B, .data[0] = 0x20 },//SLVS-EC:Standby Length 
	{ .address = 0x011E, .data[0] = 0x60 },//SLVS-EC:Deskew Symbol
	
	{ .address = D_TBL_END, .data[0] = 0xFF }// END

};

const T_CAMERA_SENSOR_BASIC		IMX571_BASIC =
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_72MHz,
	.access			= E_CAMERA_ACCESS_SPI,
	.sensor_if		= E_CAMERA_SENSOR_IF_SLVS,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_VD,
};


const T_CAMERA_SENSOR_MODE		IMX571_MODE[E_SENSOR_IMX571_MODE_MAX] =
{
	[E_SENSOR_IMX571_MODE0C_V1H1] = {
		.mode_reg				= IMX571_MODE0C_V1H1,
		
		.embedded_lines			= 0,
		.total_pix_width		= 6276,//page 77 of 167
		.total_pix_lines		= 4224,//page 77 of 167
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 20,//10 front vblank + 10 OB area without sensor
		.ob_pix_width			= 16,
		.ob_pix_lines			= 22,//refer to page 76 of 167
		//16HOB area + 6ignored HOB + 2overflow drain + 4offset
		.active_pix_x_pos		= (16+6+2+4) ,
		//10 front vblank + 10 OB area without sensor + 22 VOB area with sensor + 2ignore VOB + 4offset
		.active_pix_y_pos		= (10+10+22+2+4),
		.active_pix_width		= 6244 ,//effective pixels      6240
		.active_pix_lines		= 4168 ,//effective pixels-20   4160
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.hdocyc					= 681,//681,//25fps //711,//23.97fps //568,//30fps  //454, //refer to page 68 of 167 // 1XHS period(number of INCK) not sure !!
		.vdocyc					= 4224,                                       //5286,
		.exp_start_line_min		= 2,//refer ot page 127 of 167// SHR min =1 //2*SHR=2
		.exp_end_line_offset	= 4,
		.exp_clk_offset			= 0,//no need to set with imx571
		.base_SV				= 4.0,
		.input_bit_width		= 12,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.fps					= 25.0, //30.0
		.Mpps_HDclk				= 72.0,	// set INCLK for SLVS
		.one_lane_Mbps			= 0.0,	// no need for SLVS-EC
		.lane_num				= 8,
	},
	
	//refer to imx571 datasheet at page 76 of 167 
	[E_SENSOR_IMX571_MODE0E_V1H1] = {
		.mode_reg				= IMX571_MODE0E_V1H1,
		
		.embedded_lines 		= 0,
		.total_pix_width		= 6276,//page 77 of 167
		.total_pix_lines		= 4224,//page 77 of 167
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 20,//10 front vblank + 10 OB area without sensor
		.ob_pix_width			= 16,
		.ob_pix_lines			= 22,//refer to page 76 of 167
		//16HOB area + 6ignored HOB + 2overflow drain + 4offset
		.active_pix_x_pos		= (16+6+2+4),
		//10 front vblank + 10 OB area without sensor + 22 VOB area with sensor + 2ignore VOB + 4offset
		.active_pix_y_pos		= (10+10+22+2+4),
		.active_pix_width		= 6244,//effective pixels
		.active_pix_lines		= 4168,//effective pixels-20
		.active_pix_x_rsz_ratio = 1.0,
		.active_pix_y_rsz_ratio = 1.0,
		.hdocyc 				= 352,//refer to page 68 of 167 // 1XHS period(number of INCK) not sure !!
		.vdocyc 				= 6818,//5286,
		.exp_start_line_min 	= 2,//refer ot page 127 of 167// SHR min =1 //2*SHR=2
		.exp_end_line_offset	= 4,
		.exp_clk_offset 		= 0,//no need to set with imx571
		.base_SV				= 4.0,
		.input_bit_width		= 10,
		.first_active_pix_color = FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.fps					= 25.0, //
		.Mpps_HDclk 			= 72.0, // set INCLK for SLVS
		.one_lane_Mbps			= 0.0,	// no need for SLVS-EC
		.lane_num				= 8,
	},
	
	[E_SENSOR_IMX571_MODE5C_V2H2] = {
		.mode_reg				= IMX571_MODE5C_V2H2,
	
		.embedded_lines 		= 0,
		.total_pix_width		= 3138,
		.total_pix_lines		= 2116,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 8,
		.ob_pix_lines			= 10,
		.active_pix_x_pos		= (8+4+2),
		.active_pix_y_pos		= (8+4+10+2+2),
		.active_pix_width		= 3122,
		.active_pix_lines		= (2084-18),
		.active_pix_x_rsz_ratio = 1.0,
		.active_pix_y_rsz_ratio = 1.0,
		.hdocyc 				= 1134, 
		.vdocyc 				= 2116,
		.exp_start_line_min 	= 2,
		.exp_end_line_offset	= 4,
		.exp_clk_offset 		= 0,
		.base_SV				= 4.0,
		.input_bit_width		= 12,
		.first_active_pix_color = FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.fps					= 30.0, 
		.Mpps_HDclk 			= 72.0, // set INCLK for SLVS
		.one_lane_Mbps			= 0.0,	// no need for SLVS
		.lane_num				= 8,
	}

};

T_CAMERA_EXP *cur_exp_params;

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static E_CAMERA_ERR_CODE access_tbl(E_CAMERA_ID sensor_id, const T_CAMERA_SENSOR_REG* tbl);
VOID Sensor_IMX571_Power_GpioInit(VOID);
VOID Sensor_IMX571_PowerOn(VOID);
VOID Sensor_IMX571_PowerOff(VOID);




/*global shutter.add by liuxin 20210816*/
#ifdef IMX571_GLOBAL_SHUTTER
E_CAMERA_ERR_CODE Sensor_IMX571_GlobalShutter(E_CAMERA_ID sensor_id, E_CAMERA_SENSOR_SHUTTER shutter_mode);
static VOID mshutter_close_timer_handler0( VOID );
static void mshutter_close_by_timer_start(E_CAMERA_ID sensor_id, ULONG usec);
static void mshutter_standby_timer_handler( VOID );
static void mshutter_standby_later(int usec);
static void Sensor_IMX571_Check_Shutter_StandbyMode(void);
static UCHAR g_sensor_shut_stat = 0xFF; // 0xX0: G OFF, 0xX1: G ON, 0x0X: M OPEN, 0x1X: M CLOSE, others: init stat
static UINT8 g_isFirstView=0x00;
static UINT8 g_isCaptureFinish=0x00;
static ULONG g_sensor_cur_readout_us = 33333;//1sec/30fps=0.033333
static UINT8 g_isCapMode=0x00;//0x0 view mode //other :capture mode
#endif

static USHORT g_backup_exposure_time_value[4];
static USHORT g_backup_analog_gain_value[4];
static USHORT g_sensor_status = 0;



/*global shutter.add by liuxin 20210816*/
/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

#define D_MSHUTTER_NSLEEP E_DD_TOP_GPIO_P71
#define D_MSHUTTER_P	E_DD_TOP_GPIO_P73
#define D_MSHUTTER_N	E_DD_TOP_GPIO_P72
#define D_CALLBACK		E_DD_TOP_GPIO_CM0_P03

#define M_SHUTTER_INIT()\
	Dd_Top_Set_Gpio_Function(D_MSHUTTER_P, D_DD_TOP_GPIO_FUNC_GPIO);\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_P, D_DD_TOP_GPIO_STATUS_LOW);\
	Dd_Top_Set_Gpio_Direction(D_MSHUTTER_P, D_DD_TOP_GPIO_DIR_OUT);\
	Dd_Top_Set_Gpio_Function(D_MSHUTTER_N, D_DD_TOP_GPIO_FUNC_GPIO);\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_N, D_DD_TOP_GPIO_STATUS_LOW);\
	Dd_Top_Set_Gpio_Direction(D_MSHUTTER_N, D_DD_TOP_GPIO_DIR_OUT);\
	Dd_Top_Set_Gpio_Function(D_MSHUTTER_NSLEEP, D_DD_TOP_GPIO_FUNC_GPIO);\
	Dd_Top_Set_Gpio_Status(D_MSHUTTER_NSLEEP, D_DD_TOP_GPIO_STATUS_HIGH);\
	Dd_Top_Set_Gpio_Direction(D_MSHUTTER_NSLEEP, D_DD_TOP_GPIO_DIR_OUT);\
	Dd_Top_Set_Gpio_CM0_Function(D_CALLBACK, D_DD_TOP_GPIO_FUNC_GPIO);\
	Dd_Top_Set_Gpio_CM0_Status(D_CALLBACK, D_DD_TOP_GPIO_STATUS_LOW);\
	Dd_Top_Set_Gpio_CM0_Direction(D_CALLBACK, D_DD_TOP_GPIO_DIR_OUT);\

#define M_SHUTTER_USEC_TO_STANDBY 1

#define M_SHUTTER_STANDBY()\
		Dd_Top_Set_Gpio_Status(D_MSHUTTER_P, D_DD_TOP_GPIO_STATUS_LOW);\
		Dd_Top_Set_Gpio_Status(D_MSHUTTER_N, D_DD_TOP_GPIO_STATUS_LOW);\
		printf("(%ld)STANNDDDDDDDDDDDDDDDDDBY\n", BF_Measure_Time_Get_Long_usec(0));\

#define M_SHUTTER_OPEN(x)\
		printf("(%ld) OPPPPPPPPPPPPPPEEEEEEEEENNNNNN\n",BF_Measure_Time_Get_Long_usec(0) );\
		Dd_Top_Set_Gpio_Status(D_MSHUTTER_P, D_DD_TOP_GPIO_STATUS_HIGH);\
		Dd_Top_Set_Gpio_Status(D_MSHUTTER_N, D_DD_TOP_GPIO_STATUS_LOW);\
		// OS_User_Dly_Tsk(0);\
		// M_SHUTTER_STANDBY();\

		
#define M_SHUTTER_CLOSE(x)\
		printf("(%ld)CLOOOOOOOOOOOOOOOOOOOOOSSEEE\n", BF_Measure_Time_Get_Long_usec(0));\
		Dd_Top_Set_Gpio_Status(D_MSHUTTER_P, D_DD_TOP_GPIO_STATUS_LOW);\
		Dd_Top_Set_Gpio_Status(D_MSHUTTER_N, D_DD_TOP_GPIO_STATUS_HIGH);\
		// OS_User_Dly_Tsk(0);\
		// M_SHUTTER_STANDBY();

	



/************************************************************
** -
** -Function    : Sensor_imx571_power_gpioInit
** -Description : 
** -Owner       :  LiuXin
** -Date        :  20210608
** -
**************************************************************/
VOID Sensor_IMX571_Power_GpioInit(VOID)
{
	printf("[liuxin]-fun:%s..line:%d..\n",__func__,__LINE__);
#ifdef IMX571_GLOBAL_SHUTTER
	M_SHUTTER_INIT();
#endif
}	


/************************************************************
** -
** -Function    : Sensor_IMX571_PowerOn
** -Description : 
** -Owner       :  LiuXin
** -Date        :  20210608
** -
**************************************************************/
VOID Sensor_IMX571_PowerOn(VOID)
{
	printf("[liuxin]-fun:%s..line:%d..\n",__func__,__LINE__);
	// Dd_Top_Set_Gpio_Status(D_MSHUTTER_NSLEEP, D_DD_TOP_GPIO_STATUS_HIGH);
	// OS_User_Dly_Tsk(30);		// Wait Sensor Stability (Needs Over 100us)
	
}


/************************************************************
** -
** -Function    : Sensor_IMX571_PowerOff
** -Description : 
** -Owner       :  LiuXin
** -Date        :  20210608
** -
**************************************************************/
VOID Sensor_IMX571_PowerOff(VOID)
{
	printf("[liuxin]-fun:%s..line:%d..\n",__func__,__LINE__);
	// Dd_Top_Set_Gpio_Status(D_MSHUTTER_NSLEEP, D_DD_TOP_GPIO_STATUS_LOW);
}


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX571_Clock_Gpio_Init(VOID)
{
	INT32 sensor_id;
	
	printf("[liuxin]-fun:%s..line:%d..\n",__func__,__LINE__);
	
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

	// SP0SCK Pull Up 
	Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PD2, 1 );/*add by liuxin 20210608*/
	Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PD2, 1 );
	Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PD2, 1 );
	
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

		/* add by LiuXin on 20210608*/
		Sensor_IMX571_Power_GpioInit();
		
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

	return E_CAMERA_ERR_OK;
}



E_CAMERA_ERR_CODE Sensor_IMX571_Standby(E_CAMERA_ID sensor_id)
{
	U_REG	reg;
	g_sensor_status = 0;

	printf("[liuxin]-fun:%s..line:%d..\n",__func__,__LINE__);

/*change by LiuXin on 20210608*/
#if 1
	// SLP_CTRL = 1
	Camera_Common_SPI_Read(CAMERA.sensor[sensor_id].spi_ch, 0x01EE, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	reg.byte[0] = reg.byte[0] | 0x04;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x01EE, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);

	// STANDBY = 1
	Camera_Common_SPI_Read(CAMERA.sensor[sensor_id].spi_ch, 0x0000, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	reg.byte[0] = reg.byte[0] | 0x01;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0000, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);

#else
	reg.byte[0] = 0x01;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0000 , &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);

#endif
	CAMERA_PRINT_ERROR( Im_PRO_SG_Stop( sensor_id, 1 ));
	Dd_Top_Set_Gpio_Status( CAMERA.sensor[sensor_id].port_xclr, 0 ); // XCLR
	Sensor_IMX571_PowerOff();//power off when XCLR change to low lever
	return E_CAMERA_ERR_OK;
}


E_CAMERA_ERR_CODE Sensor_IMX571_CancelStandby(E_CAMERA_ID sensor_id)
{
		
	printf("[liuxin]-fun:%s..line:%d..\n",__func__,__LINE__);
	g_sensor_status = 1;

	/*add by LiuXin on 20210608*/
	Sensor_IMX571_PowerOn();//Power On when XCLR change to high lever

	//sensor reset 
	Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 0 ); // XCLR set low
	OS_User_Dly_Tsk(1); 	// Wait Sensor Stability (Needs Over 100ns)
	Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 1 ); // XCLR set high
	OS_User_Dly_Tsk(1); 	// Wait Sensor Stability (Needs Over 100ns)
	
	access_tbl(sensor_id, IMX571_STANDBY_RELEASE);
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX571_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol)
{

	printf("[liuxin]-fun:%s..line:%d..\n",__func__,__LINE__);
	Sensor_IMX571_SgVHD(sensor_id, CAMERA.sensor[sensor_id].mode[mode].vdocyc, CAMERA.sensor[sensor_id].mode[mode].hdocyc);

	g_backup_exposure_time_value[sensor_id] = 0xFFFF;
	g_backup_analog_gain_value[sensor_id] = 0xFFFF;
	// MODE
	access_tbl(sensor_id, IMX571_MODE[mode].mode_reg);
	
	return E_CAMERA_ERR_OK;
}


E_CAMERA_ERR_CODE Sensor_IMX571_SgVHD(E_CAMERA_ID sensor_id, ULONG vdocyc, ULONG hdocyc)
{
	FJ_SG_CTRL	sg_tbl;
	static ULONG vdocyc_bak = 0; 
	static ULONG hdocyc_bak = 0; 

	//printf("[liuxin]-fun:%s..line:%d..\n",__func__,__LINE__);
	
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
	CAMERA_PRINT_ERROR( FJ_SetSg( sensor_id, &sg_tbl ) );
	CAMERA_PRINT_ERROR( FJ_SgStart( sensor_id ) ); // restart after current frame
	
	return E_CAMERA_ERR_OK;
}


E_CAMERA_ERR_CODE Sensor_IMX571_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
{
#if 1
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
	USHORT	PGC = 0.0;
	FLOAT	shr_multi = 1;

	FLOAT req_exp_msec = 16.666;
	FLOAT req_gain_times = 1.0;

	shr_multi = 2.0;

	// --------------------
	// Exposur Time
	// --------------------
	// Time = [ { vdocyc * (SVR+1) - shr_multi * SHR } * hdocyc + offset ] / INCK
	// -> (Time * INCK - offset ) / hdocyc = vdocyc - shr_multi * SHR (SVR=0 fixed)
	// -> (Time * INCK - offset ) / hdocyc ... (a)
	// -> vdocyc - shr_multi * SHR         ... (b)

	//printf("[liuxin]-fun:%s..line:%d..\n",__func__,__LINE__);

	/*global shutter.add by liuxin 20210816*/
	#ifdef IMX571_GLOBAL_SHUTTER
	Sensor_IMX571_Check_Shutter_StandbyMode();
	#endif

	
	req_exp_msec = in->exp_msec;  //1; test 1/1000. liuxin 20210817
	req_gain_times = in->gain_times;
	
	// req_vdocyc = (a)
	req_vdocyc = ( (req_exp_msec / 1000.0) * (IMX571_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) - (FLOAT)IMX571_MODE[mode].exp_clk_offset) / (FLOAT)IMX571_MODE[mode].hdocyc;
	if( req_vdocyc < 0.0 ){
		req_vdocyc = 0.0;
	}
	
	// (a) = (b)
	// -> req_vdocyc = vdocyc - shr_multi * SHR
	// -> SHR = (vdocyc - req_vdocyc) / shr_multi
	req_SHR = ((FLOAT)IMX571_MODE[mode].vdocyc - req_vdocyc) / shr_multi;
	
	// find vdocyc and SHR combination
	if( req_SHR < ((FLOAT)IMX571_MODE[mode].exp_start_line_min ) ){
		// extend vdocyc
		real_SHR = IMX571_MODE[mode].exp_start_line_min;
		real_vdocyc = (USHORT)( req_vdocyc + (FLOAT)IMX571_MODE[mode].exp_start_line_min );
	}
	else if( (IMX571_MODE[mode].vdocyc / 2 - IMX571_MODE[mode].exp_end_line_offset) < req_SHR ){
		// clip
		real_SHR = IMX571_MODE[mode].vdocyc / 2 - IMX571_MODE[mode].exp_end_line_offset;
		real_vdocyc = IMX571_MODE[mode].vdocyc;
	}
	else{
		real_SHR = (USHORT)req_SHR;
		real_vdocyc = IMX571_MODE[mode].vdocyc;
	}
	// FPS
	if( D_CAMERA_FPS_AUTO == in->min_fps && D_CAMERA_FPS_AUTO == in->max_fps ){
		// nothing to do
	}
	else{
		// default
		defaultFps = (IMX571_MODE[mode].Mpps_HDclk  * 1000.0 * 1000.0) / IMX571_MODE[mode].hdocyc / IMX571_MODE[mode].vdocyc;
		
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
		reqFps = (IMX571_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX571_MODE[mode].hdocyc / (FLOAT)real_vdocyc;

		if( reqFps < minFps ){
			req_vdocyc   = (IMX571_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX571_MODE[mode].hdocyc / minFps;
			dif_vdocyc   = req_vdocyc - (FLOAT)real_vdocyc;
			real_vdocyc  = (USHORT)req_vdocyc;
		}
		if( reqFps > maxFps ){
			req_vdocyc   = (IMX571_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX571_MODE[mode].hdocyc / maxFps;
			dif_vdocyc   = req_vdocyc - (FLOAT)real_vdocyc;
			real_vdocyc  = (USHORT)req_vdocyc;
		}
		
		// SHR
		if( dif_vdocyc != 0.0 ){
			req_SHR = (FLOAT)real_SHR + dif_vdocyc;
			
			if( req_SHR < (FLOAT)IMX571_MODE[mode].exp_start_line_min ){
				req_SHR = (FLOAT)IMX571_MODE[mode].exp_start_line_min;
			}
			if( (real_vdocyc / 2 - (FLOAT)IMX571_MODE[mode].exp_end_line_offset) < req_SHR ){
				req_SHR = real_vdocyc / 2 - (FLOAT)IMX571_MODE[mode].exp_end_line_offset;
			}
			if(req_SHR < 2)
				req_SHR = 2;
			real_SHR = (USHORT)req_SHR;
		}
	}

	// real
	realExpTime = ( ((FLOAT)real_vdocyc - (FLOAT)real_SHR * shr_multi) * (FLOAT)IMX571_MODE[mode].hdocyc + IMX571_MODE[mode].exp_clk_offset) / (IMX571_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	realFps = (IMX571_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / real_vdocyc / IMX571_MODE[mode].hdocyc;
#if IMX571_DEBUG
	printf("req_vdocyc = %f req_SHR = %f real_SHR = %d\n", req_vdocyc, req_SHR, real_SHR);
	printf("exposure_time + shr = %f, exposure_time = %f\n", 
			((FLOAT)real_vdocyc * (FLOAT)IMX571_MODE[mode].hdocyc + IMX571_MODE[mode].exp_clk_offset)/(IMX571_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0),
			realExpTime);
#endif
	// Out
	out->fps = realFps;
	out->exp_msec = realExpTime * 1000.0;
	g_exp_time_571 = out->exp_msec;
	out->hdocyc = IMX571_MODE[mode].hdocyc;
	out->vdocyc = real_vdocyc;
	
	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = out->hdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = out->vdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] = real_SHR;

	// --------------------
	// Analog Gain
	// --------------------
	again_dB = 20.0 * log10f(req_gain_times);

	work = 4095.0 * ( 1.0 - powf(10, (again_dB/(-20)) ) );
	
	if( work < 0.0 )
		PGC = 0.0;
	else
	{
	switch(CAMERA.sensor[sensor_id].mode->input_bit_width)
		{
		case 12:
		case 14:
			if( work > 3996.0 ){
				PGC = 3996.0;
			}
			else{
				PGC = (USHORT)work;
			}
			break;

		case 11:
			if( work > 3837.0 ){
				PGC = 3837.0;
			}
			else{
				PGC = (USHORT)work;
			}
			break;

		default:
			break;
		}
	}
	// recalc
	again_dB = -20.0 * log10f( (4095.0 - PGC) / 4095.0 );
	out->again_times = powf( 10, (again_dB / 20.0) );
	out_reg->reg[E_CAMERA_SENSOR_REG_AGAIN] = PGC;

	// --------------------
	// Digital Gain
	//---0h: OFF	1h: 6[dB]	2h: 12[dB]	3h: 18[dB]---
	// --------------------
	dgain_dB = 0; // fix to 1.0 times (temporary)

	out->dgain_times = powf( 10, (dgain_dB / 20.0) );
	out_reg->reg[E_CAMERA_SENSOR_REG_DGAIN] = dgain_dB / 6;

	cur_exp_params = out;

	//printf("Exp:%f SHR:%04x, Gain:%f PGC:%04x\n", in->exp_msec, real_SHR, in->gain_times, PGC);
	//MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SYSTEM, FJ_MBALOG_FUNC_ID_AE, FJ_MBALOG_TYPE_POINT, "req_gain(%f), PGC(%04x)", req_gain_times, PGC));
	return E_CAMERA_ERR_OK;

#else
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
	
	// ----------------------------
	// Exposur Time IMX571 Mode5C
	// ---------------------------
	// Time = [ { vdocyc * (SVR+1) - SHR } * hdocyc + offset ] / INCK
	// -> (Time * INCK - offset ) / hdocyc = vdocyc - SHR (SVR=0 fixed)
	// -> (Time * INCK - offset ) / hdocyc ... (a)
	// -> vdocyc - SHR                     ... (b)
	//////////////////////////////////////////////////////////////////////
	// ----------------------------
	// Exposur Time IMX571 Mode0C
	// ---------------------------
	// Time = [ { vdocyc * (SVR+1) - 2*SHR } * hdocyc + offset ] / INCK
	// -> (Time * INCK - offset ) / hdocyc = vdocyc - 2*SHR (SVR=0 fixed)

	
	// req_vdocyc = (a)
	//sensor exp_ms =====> sensor exp_line
	req_vdocyc = ( (in->exp_msec / 1000.0) * (IMX571_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) - (FLOAT)IMX571_MODE[mode].exp_clk_offset) / (FLOAT)IMX571_MODE[mode].hdocyc;
	if( req_vdocyc < 0.0 ){
		req_vdocyc = 0.0;
	}
	
	// (a) = (b)
	// -> req_vdocyc = vdocyc - SHR
	// -> SHR = vdocyc - req_vdocyc
	req_SHR = (FLOAT)IMX571_MODE[mode].vdocyc - req_vdocyc;
	
	// find vdocyc and SHR combination
	if( req_SHR < (FLOAT)IMX571_MODE[mode].exp_start_line_min ){
		// extend vdocyc
		real_SHR = IMX571_MODE[mode].exp_start_line_min;
		real_vdocyc = (USHORT)( req_vdocyc + (FLOAT)IMX571_MODE[mode].exp_start_line_min );
	}
	else if( (IMX571_MODE[mode].vdocyc - IMX571_MODE[mode].exp_end_line_offset) < req_SHR ){
		// clip
		real_SHR = (IMX571_MODE[mode].vdocyc - IMX571_MODE[mode].exp_end_line_offset);
		real_vdocyc = IMX571_MODE[mode].vdocyc;
	}
	else{
		real_SHR = (USHORT)req_SHR;
		real_vdocyc = IMX571_MODE[mode].vdocyc;
	}
	
	// FPS
	if( D_CAMERA_FPS_AUTO == in->min_fps && D_CAMERA_FPS_AUTO == in->max_fps ){
		// nothing to do
	}
	else{
		// default
		defaultFps = (IMX571_MODE[mode].Mpps_HDclk  * 1000.0 * 1000.0) / IMX571_MODE[mode].hdocyc / IMX571_MODE[mode].vdocyc;
		
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
		reqFps = (IMX571_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX571_MODE[mode].hdocyc / (FLOAT)real_vdocyc;
		if( reqFps < minFps ){
			req_vdocyc   = (IMX571_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX571_MODE[mode].hdocyc / minFps;
			dif_vdocyc   = req_vdocyc - (FLOAT)real_vdocyc;
			real_vdocyc  = (USHORT)req_vdocyc;
		}
		if( reqFps > maxFps ){
			req_vdocyc   = (IMX571_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX571_MODE[mode].hdocyc / maxFps;
			dif_vdocyc   = req_vdocyc - (FLOAT)real_vdocyc;
			real_vdocyc  = (USHORT)req_vdocyc;
		}
		
		// SHR
		if( dif_vdocyc != 0.0 ){
			req_SHR = (FLOAT)real_SHR + dif_vdocyc;
			
			if( req_SHR < (FLOAT)IMX571_MODE[mode].exp_start_line_min ){
				req_SHR = (FLOAT)IMX571_MODE[mode].exp_start_line_min;
			}
			if( real_vdocyc - (FLOAT)IMX571_MODE[mode].exp_end_line_offset < req_SHR ){
				req_SHR = real_vdocyc - (FLOAT)IMX571_MODE[mode].exp_end_line_offset;
			}
			
			real_SHR = (USHORT)req_SHR;
		}
	}
	
	// real
	realExpTime = ( ((FLOAT)real_vdocyc - (FLOAT)real_SHR) * (FLOAT)IMX571_MODE[mode].hdocyc + IMX571_MODE[mode].exp_clk_offset) / (IMX571_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	realFps = (IMX571_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / real_vdocyc / IMX571_MODE[mode].hdocyc;
	
	// Out
	out->fps = realFps;
	out->exp_msec = realExpTime * 1000.0;
	out->hdocyc = IMX571_MODE[mode].hdocyc;
	out->vdocyc = real_vdocyc;
	
	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = out->hdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = out->vdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] = real_SHR;
	
	// --------------------
	// Analog Gain
	// --------------------
	again_dB = 20.0 * log10f(in->gain_times);
	
	work = 4095.0 * ( 1.0 - powf(10, (again_dB/(-20)) ) );
	
	if( work > 3914.0 ){
		PGC = 3914.0;
	}
	else if( work < 0.0 ){
		PGC = 0.0;
	}
	else{
		PGC = (USHORT)work;
	}
	// recalc
	again_dB = -20.0 * log10f( (4095.0 - PGC) / 4095.0 );
	out->again_times = powf( 10, (again_dB / 20.0) );
	out_reg->reg[E_CAMERA_SENSOR_REG_AGAIN] = PGC;
	
	// --------------------
	// Digital Gain
	// --------------------
	out->dgain_times = 1.0;
	
	return E_CAMERA_ERR_OK;

#endif
}


#if 0
FLOAT getSnapShootExp(VOID){
	return snapSootExp;
}

VOID getSnapShootExp(FLOAT exp){
	snapSootExp=exp;
}
#endif


E_CAMERA_ERR_CODE Sensor_IMX571_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in)
{
/*for test  change by LiuXin on 20210602*/
#if 1
#if 1
	U_REG	reg;
	// Exposure Time: SHR
	
	g_backup_exposure_time_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_EXP];
	// Analog Gain: PGC
	g_backup_analog_gain_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_AGAIN];

	// SHR
	reg.hword = (USHORT)g_backup_exposure_time_value[sensor_id];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0018, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	g_backup_exposure_time_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_EXP];

	//Additional Register setting for AGAIN
	//APGC_N
	reg.hword = (USHORT)g_backup_analog_gain_value[sensor_id];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0030, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	g_backup_analog_gain_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_AGAIN];

	//APGC_S
	reg.hword = (USHORT)g_backup_analog_gain_value[sensor_id];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0032, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	g_backup_analog_gain_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_AGAIN];


	if((g_backup_analog_gain_value[sensor_id] >= 0x0000) &&  (g_backup_analog_gain_value[sensor_id] <= 0x07FA))
	{
		//APGC_ADD
		reg.byte[0] = 0x00;
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x002F, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
		//CLPAPGC
		reg.byte[0] = 0x00;
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x004F, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	}else if((g_backup_analog_gain_value[sensor_id] >= 0x07FB) &&  (g_backup_analog_gain_value[sensor_id] <= 0x0DFB))
	{
		//APGC_ADD
		reg.byte[0] = 0x00;
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x002F, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
		//CLPAPGC
		reg.byte[0] = 0x02;
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x004F, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);

	}
	else
	{
		//APGC_ADD
		reg.byte[0] = 0x0C;
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x002F, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
		//CLPAPGC
		reg.byte[0] = 0x04;
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x004F, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	}

	// DGAIN  ---0h: OFF	1h: 6[dB]	2h: 12[dB]	3h: 18[dB]---
	reg.byte[0] = (UCHAR)in->reg[E_CAMERA_SENSOR_REG_DGAIN];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0040, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);

	return E_CAMERA_ERR_OK;


#else

	U_REG	reg;

	// Exposure Time: SHR
	g_backup_exposure_time_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_EXP];
	// Analog Gain: PGC
	g_backup_analog_gain_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_AGAIN];

	// SHR
	reg.hword = (USHORT)g_backup_exposure_time_value[sensor_id];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x18, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	g_backup_exposure_time_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_EXP];

	// Additional Register setting for AGAIN
	if((g_backup_analog_gain_value[sensor_id] >= 0x0000) &&  (g_backup_analog_gain_value[sensor_id] <= 0x07FA))
	{
		reg.byte[0] = 0x00;
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x002D, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
		reg.byte[0] = 0x08;
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x004D, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	}
	else if((g_backup_analog_gain_value[sensor_id] >= 0x07FB) &&  (g_backup_analog_gain_value[sensor_id] <= 0x0DFB))
	{
		reg.byte[0] = 0x00;
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x002D, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
		reg.byte[0] = 0x0A;
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x004D, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	}
	else
	{
		reg.byte[0] = 0x04;
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x002D, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
		reg.byte[0] = 0x0C;
		Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x004D, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	}
	reg.byte[0] = 0x12;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x01C3, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	reg.byte[0] = 0x12;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x01C4, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	reg.byte[0] = 0x12;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x01C5, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	reg.byte[0] = 0x12;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x01C6, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	
	// AGAIN
	reg.hword = (USHORT)g_backup_analog_gain_value[sensor_id];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x30, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	g_backup_analog_gain_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_AGAIN];

	// DGAIN
	reg.byte[0] = (UCHAR)in->reg[E_CAMERA_SENSOR_REG_DGAIN];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x40, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	
	return E_CAMERA_ERR_OK;
#endif
#endif
return E_CAMERA_ERR_OK;

}

E_CAMERA_ERR_CODE Sensor_IMX571_Test_shutter(int open, int last_usec){
	if(open){
		M_SHUTTER_OPEN(last_usec);
	}else{
		M_SHUTTER_CLOSE(last_usec);
	}
	return 0;
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



/*global shutter.add by liuxin 20210816*/
#ifdef IMX571_GLOBAL_SHUTTER
E_CAMERA_ERR_CODE Sensor_IMX571_GlobalShutter(E_CAMERA_ID sensor_id, E_CAMERA_SENSOR_SHUTTER shutter_mode)
{
	ULONG 	mshut_close_us = D_IMX571_MSHUTTER_DELAY_US;
	U_REG	reg;

	if(shutter_mode == 2){
		BF_Measure_Time_Start_usec(0);
	}
	printf("(%ld),Sensor_IMX571_GlobalShutter[%d,%02x]\n", 
		BF_Measure_Time_Get_Long_usec(0),shutter_mode, g_sensor_shut_stat);
	
	// if(((g_sensor_shut_stat & 0x11) == 0x00) && (shutter_mode == E_CAMERA_SENSOR_SHUTTER_G_ON_M_CLOSE)){
	// 	return E_CAMERA_ERR_OK;
	// }

	if(((g_sensor_shut_stat & 0x11) == 0x00) && (shutter_mode == E_CAMERA_SENSOR_SHUTTER_G_ON_M_CLOSE)){
		return E_CAMERA_ERR_OK;
	}
	
	if(((g_sensor_shut_stat & 0x11) == 0x11) && (shutter_mode == E_CAMERA_SENSOR_SHUTTER_G_ON_M_CLOSE)){
		return E_CAMERA_ERR_OK;
	}
   // BF_Debug_Print_Info("shutter_mode = %d, g_sensor_shut_stat = %#x--------\n", shutter_mode, g_sensor_shut_stat);
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SENSOR, FJ_MBALOG_FUNC_ID_SHUTTER, FJ_MBALOG_TYPE_POINT, "GlobalShutter(%d)", shutter_mode));

	switch(shutter_mode)
	{
		case E_CAMERA_SENSOR_SHUTTER_G_OFF_M_OPEN:
			M_SHUTTER_STANDBY();
			if(g_sensor_shut_stat == 0x01){ 
				//M_OPEN且G_ON的状态
				g_isCaptureFinish=0x01;
				BF_Debug_Print_Info("###G_OFF_M_OPEN,Capture finish...[%d]-[%s]\n",__LINE__,__FUNCTION__);
			}
			if((g_sensor_shut_stat & 0x0F) != 0x00)
			{
				//初始化或者G_ON的状态
				reg.byte[0] = 0x30; // SMD = 0
				Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, E_IMX571_REG_SMD, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
				g_sensor_shut_stat = g_sensor_shut_stat & 0xF0;
			}
			if((g_sensor_shut_stat & 0xF0) != 0x00)
			{
				//初始化或者M_CLOSE的状态
				// Dd_Top_Set_Gpio_Status( D_IMX571_MSHUUTER_VOLSET_PIN, 0 );//change Current to 180mA for shutter open
				// M shutter Open
				M_SHUTTER_OPEN(M_SHUTTER_USEC_TO_STANDBY);
				g_sensor_shut_stat = g_sensor_shut_stat & 0x0F;
				g_isFirstView=0x01;
				BF_Debug_Print_Info("###shutter firstView open...[line:%d]-[%s]\n",__LINE__,__FUNCTION__);
			}
		break;
		case E_CAMERA_SENSOR_SHUTTER_G_OFF_M_CLOSE:

			if((g_sensor_shut_stat & 0x0F) != 0x00)
			{
				// G是打开状态
				reg.byte[0] = 0x30; // SMD = 0
				Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, E_IMX571_REG_SMD, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
				g_sensor_shut_stat = g_sensor_shut_stat & 0xF0;
			}
			if((g_sensor_shut_stat & 0xF0) != 0x10)
			{
				// M shutter Close
				M_SHUTTER_CLOSE(M_SHUTTER_USEC_TO_STANDBY);
				g_sensor_shut_stat = (g_sensor_shut_stat & 0x0F) | 0x10;
			}
		break;
		case E_CAMERA_SENSOR_SHUTTER_G_ON_M_OPEN:

			if(((g_sensor_shut_stat & 0x11) == 0x00) || (g_sensor_shut_stat == 0xFF)){
				//初始化状态，或G关M开状态
				
				// mshut_close_us = g_exp_time_571 * 1000;
				ULONG shutter_calibration = 0;
				Cate_Get(0x0b, 0x0d, 1, &shutter_calibration);
				printf("shutter calibration = %ld seconds\n", shutter_calibration);
				mshut_close_us = 76000 - shutter_calibration;
				printf("(%ld) mshutter_close_by_timer_start mshut_close_us=%d\n", 
							BF_Measure_Time_Get_Long_usec(0),
							mshut_close_us);
				mshutter_close_by_timer_start(sensor_id, mshut_close_us);
				
				//mshut_close_us = g_sensor_cur_readout_us * 2  - D_MSHUTTER_DELAY_US  ;
			}else if((g_sensor_shut_stat & 0x11) == 0x11){
				//G开，M关状态
				mshut_close_us = g_sensor_cur_readout_us   - D_IMX571_MSHUTTER_DELAY_US + 40 * 1000;
			}
			//mshutter_close_by_timer_start(sensor_id, mshut_close_us);

			if((g_sensor_shut_stat & 0x0F) != 0x01)
			{
				//
				reg.byte[0] = 0x31; // SMD = 1
				Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, E_IMX571_REG_SMD, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
				g_sensor_shut_stat = (g_sensor_shut_stat & 0xF0) | 0x01;
			}
			if((g_sensor_shut_stat & 0xF0) != 0x00)
			{
				// M shutter Open
				Dd_Top_Set_Gpio_Status( D_IMX571_MSHUUTER_VOLSET_PIN, 0 );//change Current to 180mA for shutter open
				M_SHUTTER_OPEN(M_SHUTTER_USEC_TO_STANDBY);
				// Dd_Top_Set_Gpio_Status( D_IMX571_GPS_EVENT_PIN, 0);	// low lever to reset GPS sync event
				g_sensor_shut_stat = g_sensor_shut_stat & 0x0F;
			}
		break;
		case E_CAMERA_SENSOR_SHUTTER_G_ON_M_CLOSE:
			if((g_sensor_shut_stat & 0x0F) != 0x01)
			{
				reg.byte[0] = 0x31; // SMD = 1
				Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, E_IMX571_REG_SMD, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
				g_sensor_shut_stat = (g_sensor_shut_stat & 0xF0) | 0x01;
			}
			if((g_sensor_shut_stat & 0xF0) != 0x10)
			{
				// M shutter Close
				// mshut_close_us = g_exp_time_571 * 1000;
				// printf("(%ld) mshutter_close_by_timer_start mshut_close_us=%d\n", 
				// 			BF_Measure_Time_Get_Long_usec(0),
				// 			mshut_close_us);
				// mshutter_close_by_timer_start(sensor_id, mshut_close_us/2);
				// M_SHUTTER_CLOSE();
				// Dd_Top_Set_Gpio_Status( D_IMX571_GPS_EVENT_PIN, 1 );	// Rising pulse to trigger GPS sync event
				g_sensor_shut_stat = (g_sensor_shut_stat & 0x0F) | 0x10;
			}
		break;
		default:
		break;
	}
	
	return E_CAMERA_ERR_OK;
}


#ifdef D_IMX571_MSHUTTER_CLOSE_BY_TIMER
static VOID mshutter_close_timer_handler0( VOID )
{
	printf("(%ld) mshutter_close_timer_handler0\n", BF_Measure_Time_Get_Long_usec(0));
	// Sensor_IMX571_GlobalShutter(E_CAMERA_ID_0, E_CAMERA_SENSOR_SHUTTER_G_ON_M_CLOSE);
	M_SHUTTER_CLOSE(M_SHUTTER_USEC_TO_STANDBY);
	Dd_TMR32_Stop(D_IMX571_MSHUTTER_TIMER_ID0_CH);
	Dd_TMR32_Close(D_IMX571_MSHUTTER_TIMER_ID0_CH);
}
#endif


static void mshutter_close_by_timer_start(E_CAMERA_ID sensor_id, ULONG usec)
{

#ifdef D_IMX571_MSHUTTER_CLOSE_BY_TIMER
	INT32		ret;
	UINT32 		ch;
	VP_CALLBACK pcallback;

	switch(sensor_id)
	{
		case E_CAMERA_ID_0:
			ch = D_IMX571_MSHUTTER_TIMER_ID0_CH;
			pcallback = (VP_CALLBACK)mshutter_close_timer_handler0;
		break;
		case E_CAMERA_ID_1:
		case E_CAMERA_ID_2:
		case E_CAMERA_ID_3:
		default:
			ch = D_IMX571_MSHUTTER_TIMER_ID0_CH;
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

#if 0
static void callback_handler(){
	Dd_Top_Set_Gpio_CM0_Status(D_CALLBACK, D_DD_TOP_GPIO_STATUS_LOW);
	Dd_TMR32_Stop(D_IMX571_CALLBACK_TIMER_ID0_CH);
	Dd_TMR32_Close(D_IMX571_CALLBACK_TIMER_ID0_CH);
}

static void callback_timer_start(ULONG usec){
	VP_CALLBACK pcallback = callback_handler;
	int ch = D_IMX571_CALLBACK_TIMER_ID0_CH;

	Dd_Top_Set_Gpio_CM0_Status(D_CALLBACK, D_DD_TOP_GPIO_STATUS_HIGH);
	int ret = Dd_TMR32_Open( ch, D_DDIM_USER_SEM_WAIT_POL );
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
}
#endif
E_CAMERA_ERR_CODE Sensor_IMX571_CALLBACK	(E_CAMERA_ID sensor_id, UCHAR enable){
	// callback_timer_start(5000);
	Dd_Top_Set_Gpio_CM0_Status(D_CALLBACK, enable ? D_DD_TOP_GPIO_STATUS_HIGH : D_DD_TOP_GPIO_STATUS_LOW);
}


static void Sensor_IMX571_Check_Shutter_StandbyMode(void)
{
	static UINT32 times=0; 
	static UINT8 frameCounts=0;
	//only checking in view mode
	if((g_sensor_shut_stat & 0x01) == 0x00){
		times++;
		if(times>30*3 && g_isFirstView){//wait for 3 sec to run this code
			g_isFirstView=0x00;
			M_SHUTTER_STANDBY();
			Dd_Top_Set_Gpio_Status( D_IMX571_MSHUUTER_VOLSET_PIN, 1 );//change Current to 255mA for next shutter close
			BF_Debug_Print_Info("###FirstView start,setting shutter to standby mdoe  [line:%d][%s]\n",__LINE__,__FUNCTION__);
		}

		if(g_isCaptureFinish){
			frameCounts++;
			if(frameCounts>2){
				g_isCaptureFinish=0x00;
				frameCounts=0x00;
				M_SHUTTER_STANDBY();				
				Dd_Top_Set_Gpio_Status( D_IMX571_MSHUUTER_VOLSET_PIN, 1 );//change Current to 255mA for next shutter close
				BF_Debug_Print_Info("###setting shutter to standby mdoe...[line:%d][%s]\n",__LINE__,__FUNCTION__);
			}
		}
	}
	return;
}

static void mshutter_standby_timer_handler( VOID ){
	M_SHUTTER_STANDBY();
	Dd_TMR32_Stop(D_IMX571_MSHUTTER_STANDBY_ID0_CH);
	Dd_TMR32_Close(D_IMX571_MSHUTTER_STANDBY_ID0_CH);
}

static void mshutter_standby_later(int usec){
	INT32		ret;
	UINT32 		ch = D_IMX571_MSHUTTER_STANDBY_ID0_CH;
	VP_CALLBACK pcallback;
	pcallback = mshutter_standby_timer_handler;

	ret = Dd_TMR32_Open( ch, D_DDIM_USER_SEM_WAIT_POL );
	if (ret != D_DDIM_OK) {
		printf("\ntimer(%d) open error : ret=%08x\n", ch, ret);
		return;
	}
	ret = Dd_TMR32_SetTimer( ch, usec, pcallback );
	if(ret != D_DDIM_OK) {
		printf("\ntimer(%d) set error : ret=%08x\n", ch, ret);
		Dd_TMR32_Stop(ch);
		Dd_TMR32_Close(ch);
		return;
	}
	ret = Dd_TMR32_Start(ch);
	if(ret != D_DDIM_OK) {
		printf("\ntimer(%d) start error : ret=%08x\n", ch, ret);
		return;
	}
	printf("standby later %d\n", usec);
}


#endif



