/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "../include/camera_sensor_OS08A10.h"

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
static UINT32 g_backup_digital_gain_value[4];
static USHORT g_sensor_status = 0;
/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
const T_CAMERA_SENSOR_REG		OS08A10_MODE_FULL_3840x2160_RAW10_60FPS[] =
{
//	{ .address = 0x0100, .data[0] = 0x00 }, // Software standby

	{ .address = 0x0103, .data[0] = 0x01 },
	{ .address = 0x0303, .data[0] = 0x01 },
	{ .address = 0x0305, .data[0] = 0x5a },
	{ .address = 0x0306, .data[0] = 0x00 },
	{ .address = 0x0308, .data[0] = 0x03 },
	{ .address = 0x0309, .data[0] = 0x04 },
	{ .address = 0x032a, .data[0] = 0x00 },
	{ .address = 0x3002, .data[0] = 0xC0 }, // io_vsync_oen = 1, io_href_oen = 1
	{ .address = 0x300f, .data[0] = 0x11 },
	{ .address = 0x3010, .data[0] = 0x01 },
	{ .address = 0x3011, .data[0] = 0x04 },
	{ .address = 0x3012, .data[0] = 0x41 },
	{ .address = 0x3016, .data[0] = 0xf0 },
	{ .address = 0x301e, .data[0] = 0x98 },
	{ .address = 0x3031, .data[0] = 0xa9 },
	{ .address = 0x3103, .data[0] = 0x92 },
	{ .address = 0x3104, .data[0] = 0x01 },
	{ .address = 0x3106, .data[0] = 0x10 },
	{ .address = 0x3400, .data[0] = 0x04 },
	{ .address = 0x3025, .data[0] = 0x03 },
	{ .address = 0x3425, .data[0] = 0x01 },
	{ .address = 0x3428, .data[0] = 0x01 },
	{ .address = 0x3406, .data[0] = 0x08 },
	{ .address = 0x3408, .data[0] = 0x03 },
	{ .address = 0x340c, .data[0] = 0xff },
	{ .address = 0x340d, .data[0] = 0xff },
	{ .address = 0x031e, .data[0] = 0x09 },
//	{ .address = 0x3501, .data[0] = 0x08 }, // Long exposure[15:8]
//	{ .address = 0x3502, .data[0] = 0xe5 }, // Long exposure[7:0]
	{ .address = 0x3505, .data[0] = 0x83 },
	{ .address = 0x3508, .data[0] = 0x00 },
	{ .address = 0x3509, .data[0] = 0x80 },
	{ .address = 0x350a, .data[0] = 0x04 },
	{ .address = 0x350b, .data[0] = 0x00 },
	{ .address = 0x350c, .data[0] = 0x00 },
	{ .address = 0x350d, .data[0] = 0x80 },
	{ .address = 0x350e, .data[0] = 0x04 },
	{ .address = 0x350f, .data[0] = 0x00 },
	{ .address = 0x3600, .data[0] = 0x00 },
	{ .address = 0x3603, .data[0] = 0x2c },
	{ .address = 0x3605, .data[0] = 0x50 },
	{ .address = 0x3609, .data[0] = 0xb5 },
	{ .address = 0x3610, .data[0] = 0x39 },
	{ .address = 0x360c, .data[0] = 0x01 },
	{ .address = 0x3628, .data[0] = 0xa4 },
	{ .address = 0x362d, .data[0] = 0x10 },
	{ .address = 0x3660, .data[0] = 0x43 },
	{ .address = 0x3661, .data[0] = 0x06 },
	{ .address = 0x3662, .data[0] = 0x00 },
	{ .address = 0x3663, .data[0] = 0x28 },
	{ .address = 0x3664, .data[0] = 0x0d },
	{ .address = 0x366a, .data[0] = 0x38 },
	{ .address = 0x366b, .data[0] = 0xa0 },
	{ .address = 0x366d, .data[0] = 0x00 },
	{ .address = 0x366e, .data[0] = 0x00 },
	{ .address = 0x3680, .data[0] = 0x00 },
	{ .address = 0x36c0, .data[0] = 0x00 },
	{ .address = 0x3701, .data[0] = 0x02 },
	{ .address = 0x373b, .data[0] = 0x02 },
	{ .address = 0x373c, .data[0] = 0x02 },
	{ .address = 0x3736, .data[0] = 0x02 },
	{ .address = 0x3737, .data[0] = 0x02 },
	{ .address = 0x3705, .data[0] = 0x00 },
	{ .address = 0x3706, .data[0] = 0x39 },
	{ .address = 0x370a, .data[0] = 0x00 },
	{ .address = 0x370b, .data[0] = 0x98 },
	{ .address = 0x3709, .data[0] = 0x49 },
	{ .address = 0x3714, .data[0] = 0x21 },
	{ .address = 0x371c, .data[0] = 0x00 },
	{ .address = 0x371d, .data[0] = 0x08 },
	{ .address = 0x3740, .data[0] = 0x1b },
	{ .address = 0x3741, .data[0] = 0x04 },
	{ .address = 0x375e, .data[0] = 0x0b },
	{ .address = 0x3760, .data[0] = 0x10 },
	{ .address = 0x3776, .data[0] = 0x10 },
	{ .address = 0x3781, .data[0] = 0x02 },
	{ .address = 0x3782, .data[0] = 0x04 },
	{ .address = 0x3783, .data[0] = 0x02 },
	{ .address = 0x3784, .data[0] = 0x08 },
	{ .address = 0x3785, .data[0] = 0x08 },
	{ .address = 0x3788, .data[0] = 0x01 },
	{ .address = 0x3789, .data[0] = 0x01 },
	{ .address = 0x3797, .data[0] = 0x04 },
	{ .address = 0x3762, .data[0] = 0x11 },
	{ .address = 0x3800, .data[0] = 0x00 },
	{ .address = 0x3801, .data[0] = 0x00 }, // start X = 0
	{ .address = 0x3802, .data[0] = 0x00 },
	{ .address = 0x3803, .data[0] = 0x00 }, // start Y = 0
	{ .address = 0x3804, .data[0] = 0x0f },
	{ .address = 0x3805, .data[0] = 0x1f }, // end X = 3871
	{ .address = 0x3806, .data[0] = 0x08 },
	{ .address = 0x3807, .data[0] = 0x8f }, // end Y = 2191
	{ .address = 0x3808, .data[0] = 0x0f },
	{ .address = 0x3809, .data[0] = 0x20 }, // output width = 3872
	{ .address = 0x380a, .data[0] = 0x08 },
	{ .address = 0x380b, .data[0] = 0x90 }, // output width = 2192
	{ .address = 0x380c, .data[0] = 0x04 }, // HTS[15:8]
	{ .address = 0x380d, .data[0] = 0x0c }, // HTS[7:0]
	{ .address = 0x380e, .data[0] = 0x09 }, // VTS[15:8]
	{ .address = 0x380f, .data[0] = 0x0a }, // VTS[7:0]
	{ .address = 0x3810, .data[0] = 0x00 },
	{ .address = 0x3811, .data[0] = 0x00 },
	{ .address = 0x3812, .data[0] = 0x00 },
	{ .address = 0x3813, .data[0] = 0x00 },
	{ .address = 0x3814, .data[0] = 0x01 },
	{ .address = 0x3815, .data[0] = 0x01 },
	{ .address = 0x3816, .data[0] = 0x01 },
	{ .address = 0x3817, .data[0] = 0x01 },
	{ .address = 0x381c, .data[0] = 0x00 },
	{ .address = 0x3820, .data[0] = 0x00 },
	{ .address = 0x3821, .data[0] = 0x00 },
	{ .address = 0x3823, .data[0] = 0x08 },
	{ .address = 0x3826, .data[0] = 0x00 },
	{ .address = 0x3827, .data[0] = 0x08 },
	{ .address = 0x382d, .data[0] = 0x08 },
	{ .address = 0x3832, .data[0] = 0x02 },
	{ .address = 0x3833, .data[0] = 0x00 },
	{ .address = 0x383c, .data[0] = 0x48 },
	{ .address = 0x383d, .data[0] = 0xff },
	{ .address = 0x3d85, .data[0] = 0x0b },
	{ .address = 0x3d84, .data[0] = 0x40 },
	{ .address = 0x3d8c, .data[0] = 0x63 },
	{ .address = 0x3d8d, .data[0] = 0xd7 },
	{ .address = 0x4000, .data[0] = 0xf8 },
	{ .address = 0x4001, .data[0] = 0x2b },
	{ .address = 0x4004, .data[0] = 0x00 },
	{ .address = 0x4005, .data[0] = 0x40 },
	{ .address = 0x400a, .data[0] = 0x01 },
	{ .address = 0x400f, .data[0] = 0xa0 },
	{ .address = 0x4010, .data[0] = 0x12 },
	{ .address = 0x4018, .data[0] = 0x00 },
	{ .address = 0x4008, .data[0] = 0x02 },
	{ .address = 0x4009, .data[0] = 0x0d },
	{ .address = 0x401a, .data[0] = 0x58 },
	{ .address = 0x4050, .data[0] = 0x00 },
	{ .address = 0x4051, .data[0] = 0x01 },
	{ .address = 0x4028, .data[0] = 0x2f },
	{ .address = 0x4052, .data[0] = 0x00 },
	{ .address = 0x4053, .data[0] = 0x80 },
	{ .address = 0x4054, .data[0] = 0x00 },
	{ .address = 0x4055, .data[0] = 0x80 },
	{ .address = 0x4056, .data[0] = 0x00 },
	{ .address = 0x4057, .data[0] = 0x80 },
	{ .address = 0x4058, .data[0] = 0x00 },
	{ .address = 0x4059, .data[0] = 0x80 },
	{ .address = 0x430b, .data[0] = 0xff },
	{ .address = 0x430c, .data[0] = 0xff },
	{ .address = 0x430d, .data[0] = 0x00 },
	{ .address = 0x430e, .data[0] = 0x00 },
	{ .address = 0x4501, .data[0] = 0x18 },
	{ .address = 0x4502, .data[0] = 0x00 },
	{ .address = 0x4643, .data[0] = 0x00 },
	{ .address = 0x4640, .data[0] = 0x01 },
	{ .address = 0x4641, .data[0] = 0x04 },
	{ .address = 0x4800, .data[0] = 0x64 },
	{ .address = 0x4809, .data[0] = 0x2b },
	{ .address = 0x4813, .data[0] = 0x90 },
	{ .address = 0x4817, .data[0] = 0x04 },
	{ .address = 0x4833, .data[0] = 0x18 },
	{ .address = 0x4837, .data[0] = 0x0b },
	{ .address = 0x483b, .data[0] = 0x00 },
	{ .address = 0x484b, .data[0] = 0x03 },
	{ .address = 0x4850, .data[0] = 0x7c },
	{ .address = 0x4852, .data[0] = 0x06 },
	{ .address = 0x4856, .data[0] = 0x58 },
	{ .address = 0x4857, .data[0] = 0xaa },
	{ .address = 0x4862, .data[0] = 0x0a },
	{ .address = 0x4869, .data[0] = 0x18 },
	{ .address = 0x486a, .data[0] = 0xaa },
	{ .address = 0x486e, .data[0] = 0x03 },
	{ .address = 0x486f, .data[0] = 0x55 },
	{ .address = 0x4875, .data[0] = 0xf0 },
	{ .address = 0x5000, .data[0] = 0x89 },
	{ .address = 0x5001, .data[0] = 0x42 },
	{ .address = 0x5004, .data[0] = 0x40 },
	{ .address = 0x5005, .data[0] = 0x00 },
	{ .address = 0x5180, .data[0] = 0x00 },
	{ .address = 0x5181, .data[0] = 0x10 },
	{ .address = 0x580b, .data[0] = 0x03 },
	{ .address = 0x4d00, .data[0] = 0x03 },
	{ .address = 0x4d01, .data[0] = 0xc9 },
	{ .address = 0x4d02, .data[0] = 0xbc },
	{ .address = 0x4d03, .data[0] = 0xc6 },
	{ .address = 0x4d04, .data[0] = 0x4a },
	{ .address = 0x4d05, .data[0] = 0x25 },
	{ .address = 0x4700, .data[0] = 0x2b },
	{ .address = 0x4e00, .data[0] = 0x2b },
	{ .address = 0x3501, .data[0] = 0x09 }, // PIXEL_CLOCK = 1440*4/10 = 576, LINE_LENGTH_PCK = 576*1036/144 = 4144, ROW_TIME = 4144/576 = 7.1944 us
	{ .address = 0x3502, .data[0] = 0x01 }, // exposure time = 0x901 * 7.1944 = 16.583 ms

//	{ .address = 0x0100, .data[0] = 0x01 }, // Streaming

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};

const T_CAMERA_SENSOR_REG		OS08A10_MODE_FULL_3840x2160_RAW12_30FPS[] =
{
//	{ .address = 0x0100, .data[0] = 0x00 }, // Software standby

	{ .address = 0x0103, .data[0] = 0x01 },
	{ .address = 0x0303, .data[0] = 0x01 },
	{ .address = 0x0305, .data[0] = 0x5a },
	{ .address = 0x0306, .data[0] = 0x00 },
	{ .address = 0x0308, .data[0] = 0x03 },
	{ .address = 0x0309, .data[0] = 0x04 },
	{ .address = 0x032a, .data[0] = 0x00 },
	{ .address = 0x3002, .data[0] = 0xC0 }, // io_vsync_oen = 1, io_href_oen = 1
	{ .address = 0x300f, .data[0] = 0x11 },
	{ .address = 0x3010, .data[0] = 0x01 },
	{ .address = 0x3011, .data[0] = 0x04 },
	{ .address = 0x3012, .data[0] = 0x41 },
	{ .address = 0x3016, .data[0] = 0xf0 },
	{ .address = 0x301e, .data[0] = 0x98 },
	{ .address = 0x3031, .data[0] = 0xa9 },
	{ .address = 0x3103, .data[0] = 0x92 },
	{ .address = 0x3104, .data[0] = 0x01 },
	{ .address = 0x3106, .data[0] = 0x10 },
	{ .address = 0x3400, .data[0] = 0x04 },
	{ .address = 0x3025, .data[0] = 0x03 },
	{ .address = 0x3425, .data[0] = 0x01 },
	{ .address = 0x3428, .data[0] = 0x01 },
	{ .address = 0x3406, .data[0] = 0x08 },
	{ .address = 0x3408, .data[0] = 0x03 },
	{ .address = 0x340c, .data[0] = 0xff },
	{ .address = 0x340d, .data[0] = 0xff },
	{ .address = 0x031e, .data[0] = 0x0a },
//	{ .address = 0x3501, .data[0] = 0x08 },
//	{ .address = 0x3502, .data[0] = 0xe5 },
	{ .address = 0x3505, .data[0] = 0x83 },
	{ .address = 0x3508, .data[0] = 0x00 },
	{ .address = 0x3509, .data[0] = 0x80 },
	{ .address = 0x350a, .data[0] = 0x04 },
	{ .address = 0x350b, .data[0] = 0x00 },
	{ .address = 0x350c, .data[0] = 0x00 },
	{ .address = 0x350d, .data[0] = 0x80 },
	{ .address = 0x350e, .data[0] = 0x04 },
	{ .address = 0x350f, .data[0] = 0x00 },
	{ .address = 0x3600, .data[0] = 0x00 },
	{ .address = 0x3603, .data[0] = 0x2c },
	{ .address = 0x3605, .data[0] = 0x50 },
	{ .address = 0x3609, .data[0] = 0xdb },
	{ .address = 0x3610, .data[0] = 0x39 },
	{ .address = 0x360c, .data[0] = 0x01 },
	{ .address = 0x3628, .data[0] = 0xa4 },
	{ .address = 0x362d, .data[0] = 0x10 },
	{ .address = 0x3660, .data[0] = 0xd3 },
	{ .address = 0x3661, .data[0] = 0x06 },
	{ .address = 0x3662, .data[0] = 0x00 },
	{ .address = 0x3663, .data[0] = 0x28 },
	{ .address = 0x3664, .data[0] = 0x0d },
	{ .address = 0x366a, .data[0] = 0x38 },
	{ .address = 0x366b, .data[0] = 0xa0 },
	{ .address = 0x366d, .data[0] = 0x00 },
	{ .address = 0x366e, .data[0] = 0x00 },
	{ .address = 0x3680, .data[0] = 0x00 },
	{ .address = 0x36c0, .data[0] = 0x00 },
	{ .address = 0x3701, .data[0] = 0x02 },
	{ .address = 0x373b, .data[0] = 0x02 },
	{ .address = 0x373c, .data[0] = 0x02 },
	{ .address = 0x3736, .data[0] = 0x02 },
	{ .address = 0x3737, .data[0] = 0x02 },
	{ .address = 0x3705, .data[0] = 0x00 },
	{ .address = 0x3706, .data[0] = 0x72 },
	{ .address = 0x370a, .data[0] = 0x01 },
	{ .address = 0x370b, .data[0] = 0x30 },
	{ .address = 0x3709, .data[0] = 0x48 },
	{ .address = 0x3714, .data[0] = 0x21 },
	{ .address = 0x371c, .data[0] = 0x00 },
	{ .address = 0x371d, .data[0] = 0x08 },
	{ .address = 0x3740, .data[0] = 0x1b },
	{ .address = 0x3741, .data[0] = 0x04 },
	{ .address = 0x375e, .data[0] = 0x0b },
	{ .address = 0x3760, .data[0] = 0x10 },
	{ .address = 0x3776, .data[0] = 0x10 },
	{ .address = 0x3781, .data[0] = 0x02 },
	{ .address = 0x3782, .data[0] = 0x04 },
	{ .address = 0x3783, .data[0] = 0x02 },
	{ .address = 0x3784, .data[0] = 0x08 },
	{ .address = 0x3785, .data[0] = 0x08 },
	{ .address = 0x3788, .data[0] = 0x01 },
	{ .address = 0x3789, .data[0] = 0x01 },
	{ .address = 0x3797, .data[0] = 0x04 },
	{ .address = 0x3762, .data[0] = 0x11 },
	{ .address = 0x3800, .data[0] = 0x00 },
	{ .address = 0x3801, .data[0] = 0x00 }, // start X = 0
	{ .address = 0x3802, .data[0] = 0x00 },
	{ .address = 0x3803, .data[0] = 0x00 }, // start Y = 0
	{ .address = 0x3804, .data[0] = 0x0f },
	{ .address = 0x3805, .data[0] = 0x1f }, // end X = 3871
	{ .address = 0x3806, .data[0] = 0x08 },
	{ .address = 0x3807, .data[0] = 0x8f }, // end Y = 2191
	{ .address = 0x3808, .data[0] = 0x0f },
	{ .address = 0x3809, .data[0] = 0x20 }, // output width = 3872
	{ .address = 0x380a, .data[0] = 0x08 },
	{ .address = 0x380b, .data[0] = 0x90 }, // output width = 2192
	{ .address = 0x380c, .data[0] = 0x08 }, // HTS[15:8]
	{ .address = 0x380d, .data[0] = 0x18 }, // HTS[7:0]
	{ .address = 0x380e, .data[0] = 0x09 }, // VTS[15:8]
	{ .address = 0x380f, .data[0] = 0x0a }, // VTS[7:0]
	{ .address = 0x3810, .data[0] = 0x00 },
	{ .address = 0x3811, .data[0] = 0x00 },
	{ .address = 0x3812, .data[0] = 0x00 },
	{ .address = 0x3813, .data[0] = 0x00 },
	{ .address = 0x3814, .data[0] = 0x01 },
	{ .address = 0x3815, .data[0] = 0x01 },
	{ .address = 0x3816, .data[0] = 0x01 },
	{ .address = 0x3817, .data[0] = 0x01 },
	{ .address = 0x381c, .data[0] = 0x00 },
	{ .address = 0x3820, .data[0] = 0x00 },
	{ .address = 0x3821, .data[0] = 0x00 },
	{ .address = 0x3823, .data[0] = 0x08 },
	{ .address = 0x3826, .data[0] = 0x00 },
	{ .address = 0x3827, .data[0] = 0x08 },
	{ .address = 0x382d, .data[0] = 0x08 },
	{ .address = 0x3832, .data[0] = 0x02 },
	{ .address = 0x3833, .data[0] = 0x00 },
	{ .address = 0x383c, .data[0] = 0x48 },
	{ .address = 0x383d, .data[0] = 0xff },
	{ .address = 0x3d85, .data[0] = 0x0b },
	{ .address = 0x3d84, .data[0] = 0x40 },
	{ .address = 0x3d8c, .data[0] = 0x63 },
	{ .address = 0x3d8d, .data[0] = 0xd7 },
	{ .address = 0x4000, .data[0] = 0xf8 },
	{ .address = 0x4001, .data[0] = 0x2b },
	{ .address = 0x4004, .data[0] = 0x01 },
	{ .address = 0x4005, .data[0] = 0x00 },
	{ .address = 0x400a, .data[0] = 0x01 },
	{ .address = 0x400f, .data[0] = 0xa0 },
	{ .address = 0x4010, .data[0] = 0x12 },
	{ .address = 0x4018, .data[0] = 0x00 },
	{ .address = 0x4008, .data[0] = 0x02 },
	{ .address = 0x4009, .data[0] = 0x0d },
	{ .address = 0x401a, .data[0] = 0x58 },
	{ .address = 0x4050, .data[0] = 0x00 },
	{ .address = 0x4051, .data[0] = 0x01 },
	{ .address = 0x4028, .data[0] = 0x2f },
	{ .address = 0x4052, .data[0] = 0x00 },
	{ .address = 0x4053, .data[0] = 0x80 },
	{ .address = 0x4054, .data[0] = 0x00 },
	{ .address = 0x4055, .data[0] = 0x80 },
	{ .address = 0x4056, .data[0] = 0x00 },
	{ .address = 0x4057, .data[0] = 0x80 },
	{ .address = 0x4058, .data[0] = 0x00 },
	{ .address = 0x4059, .data[0] = 0x80 },
	{ .address = 0x430b, .data[0] = 0xff },
	{ .address = 0x430c, .data[0] = 0xff },
	{ .address = 0x430d, .data[0] = 0x00 },
	{ .address = 0x430e, .data[0] = 0x00 },
	{ .address = 0x4501, .data[0] = 0x18 },
	{ .address = 0x4502, .data[0] = 0x00 },
	{ .address = 0x4600, .data[0] = 0x00 },
	{ .address = 0x4601, .data[0] = 0x20 },
	{ .address = 0x4603, .data[0] = 0x01 },
	{ .address = 0x4643, .data[0] = 0x00 },
	{ .address = 0x4640, .data[0] = 0x01 },
	{ .address = 0x4641, .data[0] = 0x04 },
	{ .address = 0x4800, .data[0] = 0x64 },
	{ .address = 0x4809, .data[0] = 0x2b },
	{ .address = 0x4813, .data[0] = 0x90 },
	{ .address = 0x4817, .data[0] = 0x04 },
	{ .address = 0x4833, .data[0] = 0x18 },
	{ .address = 0x4837, .data[0] = 0x0b },
	{ .address = 0x483b, .data[0] = 0x00 },
	{ .address = 0x484b, .data[0] = 0x03 },
	{ .address = 0x4850, .data[0] = 0x7c },
	{ .address = 0x4852, .data[0] = 0x06 },
	{ .address = 0x4856, .data[0] = 0x58 },
	{ .address = 0x4857, .data[0] = 0xaa },
	{ .address = 0x4862, .data[0] = 0x0a },
	{ .address = 0x4869, .data[0] = 0x18 },
	{ .address = 0x486a, .data[0] = 0xaa },
	{ .address = 0x486e, .data[0] = 0x03 },
	{ .address = 0x486f, .data[0] = 0x55 },
	{ .address = 0x4875, .data[0] = 0xf0 },
	{ .address = 0x5000, .data[0] = 0x89 },
	{ .address = 0x5001, .data[0] = 0x42 },
	{ .address = 0x5004, .data[0] = 0x40 },
	{ .address = 0x5005, .data[0] = 0x00 },
	{ .address = 0x5180, .data[0] = 0x00 },
	{ .address = 0x5181, .data[0] = 0x10 },
	{ .address = 0x580b, .data[0] = 0x03 },
	{ .address = 0x4d00, .data[0] = 0x03 },
	{ .address = 0x4d01, .data[0] = 0xc9 },
	{ .address = 0x4d02, .data[0] = 0xbc },
	{ .address = 0x4d03, .data[0] = 0xc6 },
	{ .address = 0x4d04, .data[0] = 0x4a },
	{ .address = 0x4d05, .data[0] = 0x25 },
	{ .address = 0x4700, .data[0] = 0x2b },
	{ .address = 0x4e00, .data[0] = 0x2b },
	{ .address = 0x3501, .data[0] = 0x09 }, // PIXEL_CLOCK = 1440*4/12 = 480, LINE_LENGTH_PCK = 480*2072/144 = 6906, ROW_TIME = 6906/576 = 14.3888 us
	{ .address = 0x3502, .data[0] = 0x01 }, // exposure time = 0x901 * 14.3888 = 33.166 ms
          
//	{ .ddress = 0x0100, .data[0] = 0x01 }, // Streaming

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};

const T_CAMERA_SENSOR_BASIC		OS08A10_BASIC =
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_24MHz,
	.access			= E_CAMERA_ACCESS_I2C,
	.sensor_if		= E_CAMERA_SENSOR_IF_DMIPI,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_VD,
};

const T_CAMERA_SENSOR_MODE		OS08A10_MODE[E_SENSOR_OS08A10_MODE_MAX] =
{
	[E_SENSOR_OS08A10_MODE_FUll_3840x2160_RAW10_60FPS] = {
		.mode_reg				= OS08A10_MODE_FULL_3840x2160_RAW10_60FPS,
		
		.embedded_lines 		= 0,
		.total_pix_width		= 3872,
		.total_pix_lines		= 2192,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 16,
		.active_pix_y_pos		= 14,
		.active_pix_width		= 3840,
		.active_pix_lines		= 2160,
		.active_pix_x_rsz_ratio = 1.0,
		.active_pix_y_rsz_ratio = 1.0,
		.hdocyc 				= 1036, // HTS: total horizontal timing size
		.vdocyc 				= 2314, // VTS: total vertical timing size
		.exp_start_line_min 	= 4,
		.exp_end_line_offset	= 8,
		.exp_clk_offset 		= 0,
		.input_bit_width		= 10,
		.first_active_pix_color = FJ_PRO_CTRL_FIRST_PIX_COLOR_B,
		.base_SV				= 4.0,
		.fps					= 60.06,   // FPS = SCLK / HTS / VTS. 60.06 = 144000000 / 1036 / 2314
		.Mpps_HDclk 			= 140,	// This is SCLK from spec p.33.
		.one_lane_Mbps          = 1440,
		.lane_num				= 4,
	},
	[E_SENSOR_OS08A10_MODE_FUll_3840x2160_RAW12_30FPS] = {
		.mode_reg				= OS08A10_MODE_FULL_3840x2160_RAW12_30FPS,

		.embedded_lines 		= 0,
		.total_pix_width		= 3872,
		.total_pix_lines		= 2192,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 16,
		.active_pix_y_pos		= 14,
		.active_pix_width		= 3840,
		.active_pix_lines		= 2160,
		.active_pix_x_rsz_ratio = 1.0,
		.active_pix_y_rsz_ratio = 1.0,
		.hdocyc 				= 2072, // HTS: total horizontal timing size
		.vdocyc 				= 2314, // VTS: total vertical timing size
		.exp_start_line_min 	= 4,
		.exp_end_line_offset	= 8,
		.exp_clk_offset 		= 0,
		.input_bit_width		= 12,
		.first_active_pix_color = FJ_PRO_CTRL_FIRST_PIX_COLOR_B,
		.base_SV				= 4.0,
		.fps					= 30.03,   // FPS = SCLK / HTS / VTS. 30.03 = 144000000 / 2072 / 2314
		.Mpps_HDclk 			= 140,	// This is SCLK from spec p.33.
		.one_lane_Mbps          = 1440,
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
E_CAMERA_ERR_CODE Sensor_OS08A10_Clock_Gpio_Init(VOID)
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


		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_inck, 1 );	// CLK to Sensor

		Dd_Top_Set_Gpio_Function(  CAMERA.sensor[sensor_id].port_xclr, 0 );	// use as GPIO
		Dd_Top_Set_Gpio_Direction( CAMERA.sensor[sensor_id].port_xclr, 1 );	// output
		Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 0 );	// XCLR to low
		OS_User_Dly_Tsk(1);
		Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 1 ); // XCLR low -> high
		OS_User_Dly_Tsk(5); 	// // refer spec p.28, t1 = 5ms
		
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

E_CAMERA_ERR_CODE Sensor_OS08A10_Standby(E_CAMERA_ID sensor_id)
{
	UCHAR data;

	UPRINTF("%s(%d) \n", __func__, __LINE__);

	// Refer spec. p.32
	data = 0;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x0100, &data, 1);
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_OS08A10_CancelStandby(E_CAMERA_ID sensor_id)
{
	UPRINTF("%s(%d) \n", __func__, __LINE__);
	g_sensor_status = 1;
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_OS08A10_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol)
{
	UPRINTF("%s(%d) mode = %d \n", __func__, __LINE__, mode);

	g_backup_exposure_time_value[sensor_id] = 0xFFFF;
	g_backup_analog_gain_value[sensor_id] = 0xFFFF;

	{	// Read ChIP ID for check
		UCHAR data_LSB, data_MSB, data_HSB;
		data_LSB = data_MSB = data_HSB = 0;
		Camera_Common_I2C_Read(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x300A, &data_HSB, 1);
		Camera_Common_I2C_Read(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x300B, &data_MSB, 1);
		Camera_Common_I2C_Read(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x300C, &data_LSB, 1);
		UPRINTF("%s(%d) Read CHIP ID (0x530841) = 0x%x before initialization \n", __func__, __LINE__, (data_HSB << 16 | data_MSB << 8 | data_LSB));
	}

	// MODE
	access_tbl(sensor_id, OS08A10_MODE[mode].mode_reg);
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_OS08A10_Stream(E_CAMERA_ID sensor_id)
{
	UCHAR data;

	data = 1;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x0100, &data, 1);
	OS_User_Dly_Tsk(10); // refer spec p.28, t4 = 10ms

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_OS08A10_SgVHD(E_CAMERA_ID sensor_id, ULONG vdocyc, ULONG hdocyc)
{
	UPRINTF("%s(%d) \n", __func__, __LINE__);
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_OS08A10_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
{
	FLOAT AGAINS, DGAINS;
	UINT32	EXPR, VTS, BT1, BT2;
	FLOAT	exp_sec, default_fps, min_fps, max_fps, req_fps;

	// Coarse Integration Time
	exp_sec = in->exp_msec / 1000.0;

	/**
	 * PIXEL_CLOCK / LINE_LENGTH_PCK / FRAME_LENGTH_LINE = SCLK / HTS / VTS
	 *
	 * FPS = SCLK / HTS / VTS    // the fomula is from OVT
	 * FPS * VTS = SCLK / HTS
	 * EXPR = int(EXPT*FPS*VTS)  // the fomula is from OS08A10_AM14_EC_Venus_TPilot.txt
	 * EXPR = EXPT * SCLK / HTS
	 */
	EXPR = (UINT32) ( exp_sec * (OS08A10_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / OS08A10_MODE[mode].hdocyc);
	// 0xFFFF is max of LONG EXPO (0x3501[15:8], 0x3502[7:0])
	if( EXPR > ( 0xFFFF - OS08A10_MODE[mode].exp_end_line_offset ) ){
		EXPR = ( 0xFFFF - OS08A10_MODE[mode].exp_end_line_offset );
	}


	// The fomula is from OS08A10_AM14_EC_Venus_TPilot.txt
	VTS = OS08A10_MODE[mode].vdocyc;
	if( EXPR > VTS - OS08A10_MODE[mode].exp_end_line_offset ){
		VTS = EXPR + OS08A10_MODE[mode].exp_end_line_offset;
	} else if (EXPR < OS08A10_MODE[mode].exp_start_line_min) {
		EXPR = 4;
	}

	// Clip by FPS
	if( in->max_fps == D_CAMERA_FPS_AUTO && in->min_fps == D_CAMERA_FPS_AUTO ){
		// auto, nothing to do
	}else{
		default_fps = (OS08A10_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)OS08A10_MODE[mode].hdocyc / (FLOAT)OS08A10_MODE[mode].vdocyc;
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
		// clip VTS
		req_fps = (OS08A10_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)OS08A10_MODE[mode].hdocyc / (FLOAT)VTS;
		if( req_fps < min_fps ){
			VTS = (OS08A10_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)OS08A10_MODE[mode].hdocyc / min_fps;
		}
		if( req_fps > max_fps ){
			VTS = (OS08A10_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)OS08A10_MODE[mode].hdocyc / max_fps;
		}
		// clip EXPR
		if( EXPR > ( VTS - OS08A10_MODE[mode].exp_end_line_offset ) ){
			EXPR = ( VTS - OS08A10_MODE[mode].exp_end_line_offset );
		}
	}


	//UPRINTF("%s(%d) exp = %f, VTS = %d EXPR = %d\n", __func__, __LINE__, in->exp_msec, VTS, EXPR);

	// The AGAINS and DGAINS fomula is from OS08A10_AM14_EC_Venus_TPilot.txt
	if (in->gain_times > 15.5) {
		AGAINS = 15.5;
		DGAINS = in->gain_times / AGAINS;
	} else if (in->gain_times >= 1) {
		AGAINS = in->gain_times;
		DGAINS = 1;
	} else if (in->gain_times < 1) {
		AGAINS = 1;
		DGAINS = in->gain_times / AGAINS;
	}

	BT1 = AGAINS * 128;
	BT2 = DGAINS * 1024;

	// conflict: the maximum value 0x7c0 is from spec p.113, but FAE use 0x7FF in OS08A10_AM14_EC_Venus_TPilot.txt
	if (BT1 > 2047) BT1 = 2047;
	if (BT2 > 16383) BT2 = 16383;


	// Revert
	exp_sec = (FLOAT) EXPR  * (FLOAT)OS08A10_MODE[mode].hdocyc / (OS08A10_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	out->exp_msec = exp_sec * 1000.0;
	out->fps = (OS08A10_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / OS08A10_MODE[mode].hdocyc / VTS;
	out->hdocyc = OS08A10_MODE[mode].hdocyc;
	out->vdocyc = VTS;
	out->again_times = AGAINS;
	out->dgain_times = DGAINS;

	// Reg
	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = OS08A10_MODE[mode].hdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = VTS;
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] = EXPR;
	out_reg->reg[E_CAMERA_SENSOR_REG_AGAIN] = BT1;
	out_reg->reg[E_CAMERA_SENSOR_REG_DGAIN] = BT2;


	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_OS08A10_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in)
{
	UCHAR byte0, byte1;

	// Exposure Time: LONG EXPO
	g_backup_exposure_time_value[sensor_id] = in->reg[E_CAMERA_SENSOR_REG_EXP];
	// Analog Gain: LONG GAIN
	g_backup_analog_gain_value[sensor_id] = in->reg[E_CAMERA_SENSOR_REG_AGAIN];
	// Digital Gain: LONG DGAIN
	g_backup_digital_gain_value[sensor_id] = in->reg[E_CAMERA_SENSOR_REG_DGAIN];

	// LONG EXPO
	byte0 = (g_backup_exposure_time_value[sensor_id] & 0x000000FF);
	byte1 = (g_backup_exposure_time_value[sensor_id] & 0x0000FF00) >> 8;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3501, &(byte1), 1);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3502, &(byte0), 1);

	// LONG GAIN
	byte0 = (g_backup_analog_gain_value[sensor_id] & 0x000000FF);
	byte1 = (g_backup_analog_gain_value[sensor_id] & 0x00003F00) >> 8;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3508, &(byte1), 1);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x3509, &(byte0), 1);

	// LONG DGAIN
	byte0 = (g_backup_digital_gain_value[sensor_id] & 0x000000FF);
	byte1 = (g_backup_digital_gain_value[sensor_id] & 0x00003F00) >> 8;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x350A, &(byte1), 1);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x350B, &(byte0), 1);

	// VTS
	byte0 = (in->reg[E_CAMERA_SENSOR_REG_VDOCYC] & 0x000000FF);
	byte1 = (in->reg[E_CAMERA_SENSOR_REG_VDOCYC] & 0x0000FF00) >> 8;
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x380E, &(byte1), 1);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch, CAMERA.sensor[sensor_id].i2c_slave7bit, 0x380F, &(byte0), 1);

//	UPRINTF("%s(%d) LONG EXPO = %d, LONG GAIN = %d LONG DGAIN = %d\n", __func__, __LINE__,
//			g_backup_exposure_time_value[sensor_id], g_backup_analog_gain_value[sensor_id], g_backup_digital_gain_value[sensor_id]);

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
			// Refer spec. p35.
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

