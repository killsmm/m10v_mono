/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "camera_sensor_imx533.h"
#include "camera_config.h"
#include "os_user_custom.h"
#include "im_pro.h"
#include <math.h>

#include "share.h"

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

static USHORT g_backup_exposure_time_value[4];
static USHORT g_backup_analog_gain_value[4];
static USHORT g_sensor_status = 0;
/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
const T_CAMERA_SENSOR_REG		IMX533_STANDBY_RELEASE[] = 
{
	// Standby Release

	// 1. 
	{ .address = 0x019E, .data[0] = 0x01 },
	{ .address = 0x0000, .data[0] = 0x04 },
	{ .address = 0x0028, .data[0] = 0x04 }, // OPBN_WIDTH: 4 (OB without sensor)
	{ .address = 0x0029, .data[0] = 0x00 },
	{ .address = 0x01C0, .data[0] = 0x0A },
	{ .address = 0x01C5, .data[0] = 0x12 },
	{ .address = 0x01C6, .data[0] = 0x12 },
	{ .address = 0x01C9, .data[0] = 0xDF },
	{ .address = 0x04AA, .data[0] = 0x03 },
	{ .address = 0x04AB, .data[0] = 0x28 },
	{ .address = 0x04CF, .data[0] = 0x02 },
	{ .address = 0x067A, .data[0] = 0x33 },

	// wait 12ms
	{ .address = D_TBL_WAIT_MS, .data[0] = 0x0D },
	
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
	
};

const T_CAMERA_SENSOR_REG		IMX533_MODE_H1_V1_NO_0[] = 
{
	{ .address = 0x0001, .data[0] = 0x40 },
	{ .address = 0x0025, .data[0] = 0x0A/*0x10*/ }, // VBLK_WIDTH
	{ .address = 0x0012, .data[0] = 0x00 }, // AE: SVR(L)
	{ .address = 0x0013, .data[0] = 0x00 }, // AE: SVR(H)
	{ .address = 0x0014, .data[0] = 0x00 }, // AE: SPL(L)
	{ .address = 0x0015, .data[0] = 0x00 }, // AE: SPL(H)
	{ .address = 0x0016, .data[0] = 0x5E }, // AE: SHR(L)
	{ .address = 0x0017, .data[0] = 0x02 }, // AE: SHR(H)
	{ .address = 0x0030, .data[0] = 0x00 }, // AE: APGC(L)
	{ .address = 0x0031, .data[0] = 0x00 }, // AE: APGC(H)
	{ .address = 0x003E, .data[0] = 0x00 }, // AE: DGAIN
	{ .address = 0x0052, .data[0] = 0x9B },
	{ .address = 0x0053, .data[0] = 0x04 },
	{ .address = 0x0058, .data[0] = 0x8A },
	{ .address = 0x0059, .data[0] = 0x04 },
	{ .address = 0x005A, .data[0] = 0x5F },
	{ .address = 0x005B, .data[0] = 0x00 },
	{ .address = 0x0060, .data[0] = 0xE0 },
	{ .address = 0x0061, .data[0] = 0x05 },
	{ .address = 0x00AB, .data[0] = 0x01 },
	{ .address = 0x00AE, .data[0] = 0x01 }, // SPI: SDO Output Enable
	{ .address = 0x00CC, .data[0] = 0x3C },
	{ .address = 0x00D1, .data[0] = 0x44 },
	{ .address = 0x00D5, .data[0] = 0x01 },
	{ .address = 0x00D7, .data[0] = 0x20 },
	{ .address = 0x010D, .data[0] = 0x00 }, // Embedded line
	{ .address = 0x0111, .data[0] = 0x00 },
	{ .address = 0x0112, .data[0] = 0x03 },
	{ .address = 0x0113, .data[0] = 0x01 }, // SLVS-EC: CRC Enable
	{ .address = 0x011A, .data[0] = 0x14 }, // SLVS-EC: Deakew Interval
	{ .address = 0x011B, .data[0] = 0x20 }, // SLVS-EC: Stadnby Length
	{ .address = 0x02F0, .data[0] = 0x89 },
	{ .address = 0x02F4, .data[0] = 0x57 },
	{ .address = 0x02F6, .data[0] = 0x78 },
	{ .address = 0x040E, .data[0] = 0x06 },
	{ .address = 0x041F, .data[0] = 0x66 },
	{ .address = 0x048F, .data[0] = 0x5A },
	{ .address = 0x049A, .data[0] = 0x5A },
	{ .address = 0x04C3, .data[0] = 0x00 },
	{ .address = 0x04C7, .data[0] = 0x00 },
	{ .address = 0x0509, .data[0] = 0x55 },
	{ .address = 0x050F, .data[0] = 0x34 },
	{ .address = 0x0512, .data[0] = 0x70 },
	{ .address = 0x0513, .data[0] = 0x70 },
	{ .address = 0x0514, .data[0] = 0x70 },
	{ .address = 0x051F, .data[0] = 0x43 },
	{ .address = 0x0553, .data[0] = 0x5A },
	{ .address = 0x055C, .data[0] = 0x37 },
	{ .address = 0x055D, .data[0] = 0x35 },
	{ .address = 0x0562, .data[0] = 0x37 },
	{ .address = 0x0563, .data[0] = 0x35 },
	{ .address = 0x056B, .data[0] = 0x3E },
	{ .address = 0x056C, .data[0] = 0x3C },
	{ .address = 0x056E, .data[0] = 0x3E },
	{ .address = 0x056F, .data[0] = 0x3C },
	{ .address = 0x0575, .data[0] = 0x02 },
	{ .address = 0x0581, .data[0] = 0x02 },
	{ .address = 0x0583, .data[0] = 0x21 },
	{ .address = 0x0596, .data[0] = 0x37 },
	{ .address = 0x0597, .data[0] = 0x32 },
	{ .address = 0x0598, .data[0] = 0x37 },
	{ .address = 0x0599, .data[0] = 0x32 },
	{ .address = 0x059A, .data[0] = 0x00 },
	{ .address = 0x0603, .data[0] = 0x34 },
	{ .address = 0x062A, .data[0] = 0x87 },
	{ .address = 0x0630, .data[0] = 0x85 },
	{ .address = 0x0635, .data[0] = 0x37 },
	{ .address = 0x0636, .data[0] = 0x33 },
	{ .address = 0x0637, .data[0] = 0x37 },
	{ .address = 0x0638, .data[0] = 0x33 },
	{ .address = 0x063A, .data[0] = 0x37 },
	{ .address = 0x063B, .data[0] = 0x33 },
	{ .address = 0x063C, .data[0] = 0x37 },
	{ .address = 0x063D, .data[0] = 0x33 },
	{ .address = 0x063F, .data[0] = 0x37 },
	{ .address = 0x0640, .data[0] = 0x33 },
	{ .address = 0x0641, .data[0] = 0x37 },
	{ .address = 0x0642, .data[0] = 0x33 },
	{ .address = 0x0646, .data[0] = 0x5E },
	{ .address = 0x064A, .data[0] = 0x5E },
	{ .address = 0x066D, .data[0] = 0x77 },
	{ .address = 0x066E, .data[0] = 0x77 },
	{ .address = 0x0673, .data[0] = 0x77 },
	{ .address = 0x0674, .data[0] = 0x77 },
	{ .address = 0x0676, .data[0] = 0x77 },
	{ .address = 0x0677, .data[0] = 0x77 },
	{ .address = 0x0679, .data[0] = 0x05 },
	{ .address = 0x068A, .data[0] = 0x20 },
	{ .address = 0x0A2F, .data[0] = 0x73 },
	{ .address = 0x0A36, .data[0] = 0x73 },
	{ .address = 0x004E, .data[0] = 0x00 }, // VREAD_DIR
	{ .address = 0x0005, .data[0] = 0x00 }, // WND
	{ .address = 0x0006, .data[0] = 0x00 }, // WIN_START [7:0]
	{ .address = 0x0007, .data[0] = 0x00 }, // WIN_START [13:8]
	{ .address = 0x0008, .data[0] = 0x00 }, // WIN_WIDTH [7:0]
	{ .address = 0x0009, .data[0] = 0x00 }, // WIN_WIDTH [13:8]
	{ .address = 0x00A4, .data[0] = 0x00 },
	{ .address = 0x00D4, .data[0] = 0x00 },
	{ .address = 0x0501, .data[0] = 0x08 },
	{ .address = 0x0502, .data[0] = 0x05 },
	{ .address = 0x0503, .data[0] = 0x00 },
	{ .address = 0x00A5, .data[0] = 0x00 }, // HCROP_START_EN
	{ .address = 0x00A6, .data[0] = 0x00 }, // HCROP_START[7:0]
	{ .address = 0x00A7, .data[0] = 0x00 }, // HCROP_START[8]
	{ .address = 0x0187, .data[0] = 0x00 }, // HCROP_WIDTH_EN
	{ .address = 0x018C, .data[0] = 0x00 }, // HCROP_WIDTH[7:0]
	{ .address = 0x018D, .data[0] = 0x00 }, // HCROP_WIDTH[14:8]
	
	// wait 12ms
	{ .address = D_TBL_WAIT_MS, .data[0] = 0x0D },
	
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};

const T_CAMERA_SENSOR_REG		IMX533_MODE_H1_V1_NO_1[] = 
{
	{ .address = 0x0001, .data[0] = 0x80 },
	{ .address = 0x0025, .data[0] = 0x04 },
	{ .address = 0x0012, .data[0] = 0x00 }, // AE: SVR(L)
	{ .address = 0x0013, .data[0] = 0x00 }, // AE: SVR(H)
	{ .address = 0x0014, .data[0] = 0x00 }, // AE: SPL(L)
	{ .address = 0x0015, .data[0] = 0x00 }, // AE: SPL(H)
	{ .address = 0x0052, .data[0] = 0x78 },
	{ .address = 0x0053, .data[0] = 0x04 },
	{ .address = 0x0058, .data[0] = 0x20 },
	{ .address = 0x0059, .data[0] = 0x00 },
	{ .address = 0x005A, .data[0] = 0x5D },
	{ .address = 0x005B, .data[0] = 0x00 },
	{ .address = 0x0060, .data[0] = 0x76 },
	{ .address = 0x0061, .data[0] = 0x00 },
	{ .address = 0x00AB, .data[0] = 0x02 },
	{ .address = 0x00AE, .data[0] = 0x01 }, // SPI: SDO Output Enable
	{ .address = 0x00CC, .data[0] = 0x35 },
	{ .address = 0x00D1, .data[0] = 0x3D },
	{ .address = 0x00D5, .data[0] = 0x00 },
	{ .address = 0x00D7, .data[0] = 0x80 },
	{ .address = 0x0111, .data[0] = 0x00 },
	{ .address = 0x0112, .data[0] = 0x02 },
	{ .address = 0x0113, .data[0] = 0x01 }, // SLVS-EC: CRC Enable
	{ .address = 0x011A, .data[0] = 0x14 }, // SLVS-EC: Deakew Interval
	{ .address = 0x011B, .data[0] = 0x20 }, // SLVS-EC: Stadnby Length
	{ .address = 0x02F0, .data[0] = 0x64 },
	{ .address = 0x02F4, .data[0] = 0x55 },
	{ .address = 0x02F6, .data[0] = 0x22 },
	{ .address = 0x040E, .data[0] = 0x06 },
	{ .address = 0x041F, .data[0] = 0x66 },
	{ .address = 0x048F, .data[0] = 0x53 },
	{ .address = 0x049A, .data[0] = 0x53 },
	{ .address = 0x04C3, .data[0] = 0x00 },
	{ .address = 0x04C7, .data[0] = 0x00 },
	{ .address = 0x0509, .data[0] = 0x53 },
	{ .address = 0x050F, .data[0] = 0x39 },
	{ .address = 0x0512, .data[0] = 0xB8 },
	{ .address = 0x0513, .data[0] = 0xB8 },
	{ .address = 0x0514, .data[0] = 0xB8 },
	{ .address = 0x051F, .data[0] = 0x3C },
	{ .address = 0x0553, .data[0] = 0x53 },
	{ .address = 0x055C, .data[0] = 0x19 },
	{ .address = 0x055D, .data[0] = 0x17 },
	{ .address = 0x0562, .data[0] = 0x19 },
	{ .address = 0x0563, .data[0] = 0x17 },
	{ .address = 0x056B, .data[0] = 0x20 },
	{ .address = 0x056C, .data[0] = 0x1E },
	{ .address = 0x056E, .data[0] = 0x20 },
	{ .address = 0x056F, .data[0] = 0x1E },
	{ .address = 0x0575, .data[0] = 0x02 },
	{ .address = 0x0581, .data[0] = 0x04 },
	{ .address = 0x0583, .data[0] = 0x20 },
	{ .address = 0x0596, .data[0] = 0x19 },
	{ .address = 0x0597, .data[0] = 0x14 },
	{ .address = 0x0598, .data[0] = 0x19 },
	{ .address = 0x0599, .data[0] = 0x14 },
	{ .address = 0x059A, .data[0] = 0x04 },
	{ .address = 0x0603, .data[0] = 0x2D },
	{ .address = 0x062A, .data[0] = 0x62 },
	{ .address = 0x0630, .data[0] = 0x60 },
	{ .address = 0x0635, .data[0] = 0x19 },
	{ .address = 0x0636, .data[0] = 0x15 },
	{ .address = 0x0637, .data[0] = 0x19 },
	{ .address = 0x0638, .data[0] = 0x15 },
	{ .address = 0x063A, .data[0] = 0x19 },
	{ .address = 0x063B, .data[0] = 0x15 },
	{ .address = 0x063C, .data[0] = 0x19 },
	{ .address = 0x063D, .data[0] = 0x15 },
	{ .address = 0x063F, .data[0] = 0x19 },
	{ .address = 0x0640, .data[0] = 0x15 },
	{ .address = 0x0641, .data[0] = 0x19 },
	{ .address = 0x0642, .data[0] = 0x15 },
	{ .address = 0x0646, .data[0] = 0x57 },
	{ .address = 0x064A, .data[0] = 0x57 },
	{ .address = 0x066D, .data[0] = 0x00 },
	{ .address = 0x066E, .data[0] = 0x00 },
	{ .address = 0x0673, .data[0] = 0x00 },
	{ .address = 0x0674, .data[0] = 0x00 },
	{ .address = 0x0676, .data[0] = 0x00 },
	{ .address = 0x0677, .data[0] = 0x00 },
	{ .address = 0x0679, .data[0] = 0x07 },
	{ .address = 0x068A, .data[0] = 0x80 },
	{ .address = 0x0A2F, .data[0] = 0xAF },
	{ .address = 0x0A36, .data[0] = 0xAF },
	{ .address = 0x004E, .data[0] = 0x00 },
	{ .address = 0x0005, .data[0] = 0x00 },
	{ .address = 0x0006, .data[0] = 0x00 },
	{ .address = 0x0007, .data[0] = 0x00 },
	{ .address = 0x0008, .data[0] = 0x00 },
	{ .address = 0x0009, .data[0] = 0x00 },
	{ .address = 0x00A4, .data[0] = 0x00 },
	{ .address = 0x00D4, .data[0] = 0x00 },
	{ .address = 0x0501, .data[0] = 0x08 },
	{ .address = 0x0502, .data[0] = 0x05 },
	{ .address = 0x0503, .data[0] = 0x00 },
	{ .address = 0x00A5, .data[0] = 0x00 },
	{ .address = 0x00A6, .data[0] = 0x00 },
	{ .address = 0x00A7, .data[0] = 0x00 },
	{ .address = 0x0187, .data[0] = 0x00 },
	{ .address = 0x018C, .data[0] = 0x00 },
	{ .address = 0x018D, .data[0] = 0x00 },

	// wait 12ms
	{ .address = D_TBL_WAIT_MS, .data[0] = 0x0D },
	
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};

const T_CAMERA_SENSOR_REG		IMX533_MODE_H1_V1_NO_1N[] = 
{
	{ .address = 0x0001, .data[0] = 0x81 },
	{ .address = 0x0025, .data[0] = 0x04 },
	{ .address = 0x0012, .data[0] = 0x00 }, // AE: SVR(L)
	{ .address = 0x0013, .data[0] = 0x00 }, // AE: SVR(H)
	{ .address = 0x0014, .data[0] = 0x00 }, // AE: SPL(L)
	{ .address = 0x0015, .data[0] = 0x00 }, // AE: SPL(H)
	{ .address = 0x0016, .data[0] = 0x5E }, // AE: SHR(L)
	{ .address = 0x0017, .data[0] = 0x02 }, // AE: SHR(H)
	{ .address = 0x0030, .data[0] = 0x00 }, // AE: APGC(L)
	{ .address = 0x0031, .data[0] = 0x00 }, // AE: APGC(H)
	{ .address = 0x003E, .data[0] = 0x00 }, // AE: DGAIN
	{ .address = 0x0052, .data[0] = 0x80 },
	{ .address = 0x0053, .data[0] = 0x04 },
	{ .address = 0x0058, .data[0] = 0x20 },
	{ .address = 0x0059, .data[0] = 0x00 },
	{ .address = 0x005A, .data[0] = 0x8D },
	{ .address = 0x005B, .data[0] = 0x04 },
	{ .address = 0x0060, .data[0] = 0x76 },
	{ .address = 0x0061, .data[0] = 0x00 },
	{ .address = 0x00AB, .data[0] = 0x02 },
	{ .address = 0x00AE, .data[0] = 0x01 }, // SPI: SDO Output Enable
	{ .address = 0x00CC, .data[0] = 0x3D },
	{ .address = 0x00D1, .data[0] = 0x45 },
	{ .address = 0x00D5, .data[0] = 0x00 },
	{ .address = 0x00D7, .data[0] = 0x80 },
	{ .address = 0x0111, .data[0] = 0x00 },
	{ .address = 0x0112, .data[0] = 0x02 },
	{ .address = 0x0113, .data[0] = 0x01 }, // SLVS-EC: CRC Enable
	{ .address = 0x011A, .data[0] = 0x14 }, // SLVS-EC: Deakew Interval
	{ .address = 0x011B, .data[0] = 0x20 }, // SLVS-EC: Stadnby Length
	{ .address = 0x02F0, .data[0] = 0x6C },
	{ .address = 0x02F4, .data[0] = 0x85 },
	{ .address = 0x02F6, .data[0] = 0x22 },
	{ .address = 0x040E, .data[0] = 0x06 },
	{ .address = 0x041F, .data[0] = 0x66 },
	{ .address = 0x048F, .data[0] = 0x5B },
	{ .address = 0x049A, .data[0] = 0x5B },
	{ .address = 0x04C3, .data[0] = 0x00 },
	{ .address = 0x04C7, .data[0] = 0x00 },
	{ .address = 0x0509, .data[0] = 0x53 },
	{ .address = 0x050F, .data[0] = 0x3F },
	{ .address = 0x0512, .data[0] = 0xC0 },
	{ .address = 0x0513, .data[0] = 0xC0 },
	{ .address = 0x0514, .data[0] = 0xC0 },
	{ .address = 0x051F, .data[0] = 0x44 },
	{ .address = 0x0553, .data[0] = 0x5B },
	{ .address = 0x055C, .data[0] = 0x19 },
	{ .address = 0x055D, .data[0] = 0x17 },
	{ .address = 0x0562, .data[0] = 0x19 },
	{ .address = 0x0563, .data[0] = 0x17 },
	{ .address = 0x056B, .data[0] = 0x20 },
	{ .address = 0x056C, .data[0] = 0x1E },
	{ .address = 0x056E, .data[0] = 0x20 },
	{ .address = 0x056F, .data[0] = 0x1E },
	{ .address = 0x0575, .data[0] = 0x02 },
	{ .address = 0x0581, .data[0] = 0x04 },
	{ .address = 0x0583, .data[0] = 0x20 },
	{ .address = 0x0596, .data[0] = 0x19 },
	{ .address = 0x0597, .data[0] = 0x14 },
	{ .address = 0x0598, .data[0] = 0x19 },
	{ .address = 0x0599, .data[0] = 0x14 },
	{ .address = 0x059A, .data[0] = 0x04 },
	{ .address = 0x0603, .data[0] = 0x35 },
	{ .address = 0x062A, .data[0] = 0x6A },
	{ .address = 0x0630, .data[0] = 0x68 },
	{ .address = 0x0635, .data[0] = 0x19 },
	{ .address = 0x0636, .data[0] = 0x15 },
	{ .address = 0x0637, .data[0] = 0x19 },
	{ .address = 0x0638, .data[0] = 0x15 },
	{ .address = 0x063A, .data[0] = 0x19 },
	{ .address = 0x063B, .data[0] = 0x15 },
	{ .address = 0x063C, .data[0] = 0x19 },
	{ .address = 0x063D, .data[0] = 0x15 },
	{ .address = 0x063F, .data[0] = 0x19 },
	{ .address = 0x0640, .data[0] = 0x15 },
	{ .address = 0x0641, .data[0] = 0x19 },
	{ .address = 0x0642, .data[0] = 0x15 },
	{ .address = 0x0646, .data[0] = 0x5F },
	{ .address = 0x064A, .data[0] = 0x5F },
	{ .address = 0x066D, .data[0] = 0x00 },
	{ .address = 0x066E, .data[0] = 0x00 },
	{ .address = 0x0673, .data[0] = 0x00 },
	{ .address = 0x0674, .data[0] = 0x00 },
	{ .address = 0x0676, .data[0] = 0x00 },
	{ .address = 0x0677, .data[0] = 0x00 },
	{ .address = 0x0679, .data[0] = 0x07 },
	{ .address = 0x068A, .data[0] = 0x80 },
	{ .address = 0x0A2F, .data[0] = 0xAF },
	{ .address = 0x0A36, .data[0] = 0xAF },
	{ .address = 0x004E, .data[0] = 0x00 },
	{ .address = 0x0005, .data[0] = 0x00 },
	{ .address = 0x0006, .data[0] = 0x00 },
	{ .address = 0x0007, .data[0] = 0x00 },
	{ .address = 0x0008, .data[0] = 0x00 },
	{ .address = 0x0009, .data[0] = 0x00 },
	{ .address = 0x00A4, .data[0] = 0x00 },
	{ .address = 0x00D4, .data[0] = 0x00 },
	{ .address = 0x0501, .data[0] = 0x08 },
	{ .address = 0x0502, .data[0] = 0x05 },
	{ .address = 0x0503, .data[0] = 0x00 },
	{ .address = 0x00A5, .data[0] = 0x00 },
	{ .address = 0x00A6, .data[0] = 0x00 },
	{ .address = 0x00A7, .data[0] = 0x00 },
	{ .address = 0x0187, .data[0] = 0x00 },
	{ .address = 0x018C, .data[0] = 0x00 },
	{ .address = 0x018D, .data[0] = 0x00 },
		
	// wait 12ms
	{ .address = D_TBL_WAIT_MS, .data[0] = 0x0D },
		
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
		
};

const T_CAMERA_SENSOR_REG		IMX533_MODE_H1_V1_NO_2F[] = 
{
	{ .address = 0x0001, .data[0] = 0xA0 },
	{ .address = 0x0025, .data[0] = 0x04 },
	{ .address = 0x0012, .data[0] = 0x00 }, // AE: SVR(L)
	{ .address = 0x0013, .data[0] = 0x00 }, // AE: SVR(H)
	{ .address = 0x0014, .data[0] = 0x00 }, // AE: SPL(L)
	{ .address = 0x0015, .data[0] = 0x00 }, // AE: SPL(H)
	{ .address = 0x0016, .data[0] = 0x5E }, // AE: SHR(L)
	{ .address = 0x0017, .data[0] = 0x02 }, // AE: SHR(H)
	{ .address = 0x0030, .data[0] = 0x00 }, // AE: APGC(L)
	{ .address = 0x0031, .data[0] = 0x00 }, // AE: APGC(H)
	{ .address = 0x003E, .data[0] = 0x00 }, // AE: DGAIN
	{ .address = 0x0052, .data[0] = 0x66 },
	{ .address = 0x0053, .data[0] = 0x00 },
	{ .address = 0x0058, .data[0] = 0x15 },
	{ .address = 0x0059, .data[0] = 0x00 },
	{ .address = 0x005A, .data[0] = 0x59 },
	{ .address = 0x005B, .data[0] = 0x04 },
	{ .address = 0x0060, .data[0] = 0x40 },
	{ .address = 0x0061, .data[0] = 0x00 },
	{ .address = 0x00AB, .data[0] = 0x03 },
	{ .address = 0x00AE, .data[0] = 0x01 }, // SPI: SDO Output Enable
	{ .address = 0x00CC, .data[0] = 0x32 },
	{ .address = 0x00D1, .data[0] = 0x3A },
	{ .address = 0x00D5, .data[0] = 0x00 },
	{ .address = 0x00D7, .data[0] = 0x40 },
	{ .address = 0x0111, .data[0] = 0x00 },
	{ .address = 0x0112, .data[0] = 0x01 },
	{ .address = 0x0113, .data[0] = 0x01 }, // SLVS-EC: CRC Enable
	{ .address = 0x011A, .data[0] = 0x14 }, // SLVS-EC: Deakew Interval
	{ .address = 0x011B, .data[0] = 0x20 }, // SLVS-EC: Stadnby Length
	{ .address = 0x02F0, .data[0] = 0x5E },
	{ .address = 0x02F4, .data[0] = 0x65 },
	{ .address = 0x02F6, .data[0] = 0x17 },
	{ .address = 0x040E, .data[0] = 0x08 },
	{ .address = 0x041F, .data[0] = 0x86 },
	{ .address = 0x048F, .data[0] = 0x50 },
	{ .address = 0x049A, .data[0] = 0x50 },
	{ .address = 0x04C3, .data[0] = 0x00 },
	{ .address = 0x04C7, .data[0] = 0x00 },
	{ .address = 0x0509, .data[0] = 0x63 },
	{ .address = 0x050F, .data[0] = 0x39 },
	{ .address = 0x0512, .data[0] = 0xDC },
	{ .address = 0x0513, .data[0] = 0xDC },
	{ .address = 0x0514, .data[0] = 0xDC },
	{ .address = 0x051F, .data[0] = 0x39 },
	{ .address = 0x0553, .data[0] = 0x50 },
	{ .address = 0x055C, .data[0] = 0x16 },
	{ .address = 0x055D, .data[0] = 0x14 },
	{ .address = 0x0562, .data[0] = 0x16 },
	{ .address = 0x0563, .data[0] = 0x14 },
	{ .address = 0x056B, .data[0] = 0x1D },
	{ .address = 0x056C, .data[0] = 0x1B },
	{ .address = 0x056E, .data[0] = 0x1D },
	{ .address = 0x056F, .data[0] = 0x1B },
	{ .address = 0x0575, .data[0] = 0x0A },
	{ .address = 0x0581, .data[0] = 0x05 },
	{ .address = 0x0583, .data[0] = 0x1E },
	{ .address = 0x0596, .data[0] = 0x16 },
	{ .address = 0x0597, .data[0] = 0x11 },
	{ .address = 0x0598, .data[0] = 0x16 },
	{ .address = 0x0599, .data[0] = 0x11 },
	{ .address = 0x059A, .data[0] = 0x04 },
	{ .address = 0x0603, .data[0] = 0x2A },
	{ .address = 0x062A, .data[0] = 0x5C },
	{ .address = 0x0630, .data[0] = 0x5A },
	{ .address = 0x0635, .data[0] = 0x16 },
	{ .address = 0x0636, .data[0] = 0x0B },
	{ .address = 0x0637, .data[0] = 0x16 },
	{ .address = 0x0638, .data[0] = 0x0B },
	{ .address = 0x063A, .data[0] = 0x16 },
	{ .address = 0x063B, .data[0] = 0x0B },
	{ .address = 0x063C, .data[0] = 0x16 },
	{ .address = 0x063D, .data[0] = 0x0B },
	{ .address = 0x063F, .data[0] = 0x16 },
	{ .address = 0x0640, .data[0] = 0x0B },
	{ .address = 0x0641, .data[0] = 0x16 },
	{ .address = 0x0642, .data[0] = 0x0B },
	{ .address = 0x0646, .data[0] = 0x5B },
	{ .address = 0x064A, .data[0] = 0x5B },
	{ .address = 0x066D, .data[0] = 0x00 },
	{ .address = 0x066E, .data[0] = 0x00 },
	{ .address = 0x0673, .data[0] = 0x00 },
	{ .address = 0x0674, .data[0] = 0x00 },
	{ .address = 0x0676, .data[0] = 0x00 },
	{ .address = 0x0677, .data[0] = 0x00 },
	{ .address = 0x0679, .data[0] = 0x07 },
	{ .address = 0x068A, .data[0] = 0x40 },
	{ .address = 0x0A2F, .data[0] = 0x73 },
	{ .address = 0x0A36, .data[0] = 0x73 },
	{ .address = 0x004E, .data[0] = 0x00 },
	{ .address = 0x0005, .data[0] = 0x00 },
	{ .address = 0x0006, .data[0] = 0x00 },
	{ .address = 0x0007, .data[0] = 0x00 },
	{ .address = 0x0008, .data[0] = 0x00 },
	{ .address = 0x0009, .data[0] = 0x00 },
	{ .address = 0x00A4, .data[0] = 0x00 },
	{ .address = 0x00D4, .data[0] = 0x00 },
	{ .address = 0x0501, .data[0] = 0x08 },
	{ .address = 0x0502, .data[0] = 0x05 },
	{ .address = 0x0503, .data[0] = 0x00 },
	{ .address = 0x00A5, .data[0] = 0x00 },
	{ .address = 0x00A6, .data[0] = 0x00 },
	{ .address = 0x00A7, .data[0] = 0x00 },
	{ .address = 0x0187, .data[0] = 0x00 },
	{ .address = 0x018C, .data[0] = 0x00 },
	{ .address = 0x018D, .data[0] = 0x00 },

	// wait 12ms
	{ .address = D_TBL_WAIT_MS, .data[0] = 0x0D },
	
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};

const T_CAMERA_SENSOR_BASIC		IMX533_BASIC =
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_72MHz,
	.access			= E_CAMERA_ACCESS_SPI,
	.sensor_if		= E_CAMERA_SENSOR_IF_SLVS,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_V_BLANKING,
};

const T_CAMERA_SENSOR_MODE		IMX533_MODE[E_SENSOR_IMX533_MODE_MAX] =
{
#if 1 // enable mode 0 & mode 1
	[E_SENSOR_IMX533_MODE_H1_V1_NO_0] = {
		.mode_reg				= IMX533_MODE_H1_V1_NO_0,
		
		.embedded_lines 		= 0,
		.total_pix_width		= 3000,
		.total_pix_lines		= 3020,
		.ob_pix_x_pos			= 4,
		.ob_pix_y_pos			= 4,
		.ob_pix_width			= 3000,
		.ob_pix_lines			= 12,
		.active_pix_x_pos		= 4,
		.active_pix_y_pos		= (4+12+2+4),
		.active_pix_width		= 3000,
		.active_pix_lines		= 3020,
		.active_pix_x_rsz_ratio = 1.0,
		.active_pix_y_rsz_ratio = 1.0,
		.hdocyc 				= 878,
		.vdocyc 				= 3280,
		.exp_start_line_min 	= 2,
		.exp_end_line_offset	= 2,
		.exp_clk_offset 		= 0,
		.input_bit_width		= 14,
		.first_active_pix_color = FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.base_SV				= 4.0,
		.fps					= 25.00,
		.Mpps_HDclk 			= 72.0,
		.lane_num				= 8,
	},
	[E_SENSOR_IMX533_MODE_H1_V1_NO_1] = {
		.mode_reg				= IMX533_MODE_H1_V1_NO_1,
		
		.embedded_lines			= 0,
		.total_pix_width		= 3000,
		.total_pix_lines		= 3020,
		.ob_pix_x_pos			= 4,
		.ob_pix_y_pos			= (4+4),
		.ob_pix_width			= 3000,
		.ob_pix_lines			= 12,
		.active_pix_x_pos		= 4,
		.active_pix_y_pos		= (4+4+12+2+4),
		.active_pix_width		= 3000,
		.active_pix_lines		= 3020,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.hdocyc					= 372,
		.vdocyc					= 3225,
		.exp_start_line_min		= 2,
		.exp_end_line_offset	= 2,
		.exp_clk_offset			= 0,
		.input_bit_width		= 12,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.base_SV				= 4.0,
		.fps					= 60.01,
		.Mpps_HDclk				= 72.0,
		.lane_num				= 8,
	}
#elif defined(TEST_SENSOR_MODE_1N)
	[E_SENSOR_IMX533_MODE_H1_V1_NO_1N] = {
		.mode_reg				= IMX533_MODE_H1_V1_NO_1N,
		
		.embedded_lines			= 0,
		.total_pix_width		= 3000,
		.total_pix_lines		= 3000,
		.ob_pix_x_pos			= 4,
		.ob_pix_y_pos			= 4,
		.ob_pix_width			= 3000,
		.ob_pix_lines			= 12,
		.active_pix_x_pos		= 4,
		.active_pix_y_pos		= (4+12+2+4),
		.active_pix_width		= 3000,
		.active_pix_lines		= 3000,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.hdocyc					= 856,
		.vdocyc					= 3364,
		.exp_start_line_min		= 2,
		.exp_end_line_offset	= 2,
		.exp_clk_offset			= 0,
		.input_bit_width		= 12,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.base_SV				= 4.0,
		.fps					= 25.00,
		.Mpps_HDclk				= 72.0,
		.lane_num				= 8,
	}
#elif defined(TEST_SENSOR_MODE_2F)
	[E_SENSOR_IMX533_MODE_H1_V1_NO_2F] = {
		.mode_reg				= IMX533_MODE_H1_V1_NO_2F,
		
		.embedded_lines			= 0,
		.total_pix_width		= 3000,
		.total_pix_lines		= 3000,
		.ob_pix_x_pos			= 4,
		.ob_pix_y_pos			= 4,
		.ob_pix_width			= 3000,
		.ob_pix_lines			= 12,
		.active_pix_x_pos		= 4,
		.active_pix_y_pos		= (4+12+2+4),
		.active_pix_width		= 3000,
		.active_pix_lines		= 3000,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.hdocyc					= 310,
		.vdocyc					= 3870,
		.exp_start_line_min		= 2,
		.exp_end_line_offset	= 2,
		.exp_clk_offset			= 0,
		.input_bit_width		= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.base_SV				= 4.0,
		.fps					= 60.01,
		.Mpps_HDclk				= 72.0,
		.lane_num				= 8,
	}
#endif
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX533_Clock_Gpio_Init(VOID)
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

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX533_Standby(E_CAMERA_ID sensor_id)
{
	U_REG		reg;
	
	g_sensor_status = 0;
	
	// SLP_CTRL = 1
	Camera_Common_SPI_Read(CAMERA.sensor[sensor_id].spi_ch, 0x019E, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	reg.byte[0] = reg.byte[0] | 0x04;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x019E, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);

	// STANDBY = 1
	Camera_Common_SPI_Read(CAMERA.sensor[sensor_id].spi_ch, 0x0000, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	reg.byte[0] = reg.byte[0] | 0x01;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0000, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);

	CAMERA_PRINT_ERROR( Im_PRO_SG_Stop( sensor_id, 1 ));

	Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 0 ); // XCLR
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX533_CancelStandby(E_CAMERA_ID sensor_id)
{
	g_sensor_status = 1;
	
	Dd_Top_Set_Gpio_Status(    CAMERA.sensor[sensor_id].port_xclr, 1 ); // XCLR
	OS_User_Dly_Tsk(1); 	// Wait Sensor Stability (Needs Over 100ns)
	
	access_tbl(sensor_id, IMX533_STANDBY_RELEASE);
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX533_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol)
{

	g_backup_exposure_time_value[sensor_id] = 0xFFFF;
	g_backup_analog_gain_value[sensor_id] = 0xFFFF;
	// MODE
	access_tbl(sensor_id, IMX533_MODE[mode].mode_reg);
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX533_SgVHD(E_CAMERA_ID sensor_id, ULONG vdocyc, ULONG hdocyc)
{
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
	CAMERA_PRINT_ERROR( FJ_SetSg( sensor_id, &sg_tbl ) );
	CAMERA_PRINT_ERROR( FJ_SgStart( sensor_id ) ); // restart after current frame
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX533_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
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
	FLOAT	shr_multi = 1;

	FLOAT req_exp_msec = 16.666;
	FLOAT req_gain_times = 1.0;
	
#if 1
	if(mode == E_SENSOR_IMX533_MODE_H1_V1_NO_0 || E_SENSOR_IMX533_MODE_H1_V1_NO_1) 
		shr_multi = 2.0;
	else 
		shr_multi = 1.0;
#elif defined(TEST_SENSOR_MODE_1N)
	if(mode == E_SENSOR_IMX533_MODE_H1_V1_NO_1N) shr_multi = 1.0;
	else shr_multi= 1.0;
#elif defined(TEST_SENSOR_MODE_2F)
	if(mode == E_SENSOR_IMX533_MODE_H1_V1_NO_2F) shr_multi = 2.0;
	else shr_multi= 1.0;
#endif
	// --------------------
	// Exposur Time
	// --------------------
	// Time = [ { vdocyc * (SVR+1) - shr_multi * SHR } * hdocyc + offset ] / INCK
	// -> (Time * INCK - offset ) / hdocyc = vdocyc - shr_multi * SHR (SVR=0 fixed)
	// -> (Time * INCK - offset ) / hdocyc ... (a)
	// -> vdocyc - shr_multi * SHR         ... (b)

	req_exp_msec = in->exp_msec;
	req_gain_times = in->gain_times;
	
	// req_vdocyc = (a)
	req_vdocyc = ( (req_exp_msec / 1000.0) * (IMX533_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) - (FLOAT)IMX533_MODE[mode].exp_clk_offset) / (FLOAT)IMX533_MODE[mode].hdocyc;
	if( req_vdocyc < 0.0 ){
		req_vdocyc = 0.0;
	}
	
	// (a) = (b)
	// -> req_vdocyc = vdocyc - shr_multi * SHR
	// -> SHR = (vdocyc - req_vdocyc) / shr_multi
	req_SHR = ((FLOAT)IMX533_MODE[mode].vdocyc - req_vdocyc) / shr_multi;

	// find vdocyc and SHR combination
	if( req_SHR < ((FLOAT)IMX533_MODE[mode].exp_start_line_min ) ){
		// extend vdocyc
		real_SHR = IMX533_MODE[mode].exp_start_line_min;
		real_vdocyc = (USHORT)( req_vdocyc + (FLOAT)IMX533_MODE[mode].exp_start_line_min );
	}
	else if( (IMX533_MODE[mode].vdocyc / 2 - IMX533_MODE[mode].exp_end_line_offset) < req_SHR ){
		// clip
		real_SHR = IMX533_MODE[mode].vdocyc / 2 - IMX533_MODE[mode].exp_end_line_offset;
		real_vdocyc = IMX533_MODE[mode].vdocyc;
	}
	else{
		real_SHR = (USHORT)req_SHR;
		real_vdocyc = IMX533_MODE[mode].vdocyc;
	}

	// FPS
	if( D_CAMERA_FPS_AUTO == in->min_fps && D_CAMERA_FPS_AUTO == in->max_fps ){
		// nothing to do
	}
	else{
		// default
		defaultFps = (IMX533_MODE[mode].Mpps_HDclk  * 1000.0 * 1000.0) / IMX533_MODE[mode].hdocyc / IMX533_MODE[mode].vdocyc;
		
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
		reqFps = (IMX533_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX533_MODE[mode].hdocyc / (FLOAT)real_vdocyc;
		if( reqFps < minFps ){
			req_vdocyc   = (IMX533_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX533_MODE[mode].hdocyc / minFps;
			dif_vdocyc   = req_vdocyc - (FLOAT)real_vdocyc;
			real_vdocyc  = (USHORT)req_vdocyc;
		}
		if( reqFps > maxFps ){
			req_vdocyc   = (IMX533_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX533_MODE[mode].hdocyc / maxFps;
			dif_vdocyc   = req_vdocyc - (FLOAT)real_vdocyc;
			real_vdocyc  = (USHORT)req_vdocyc;
		}
		
		// SHR
		if( dif_vdocyc != 0.0 ){
			req_SHR = (FLOAT)real_SHR + dif_vdocyc;
			
			if( req_SHR < (FLOAT)IMX533_MODE[mode].exp_start_line_min ){
				req_SHR = (FLOAT)IMX533_MODE[mode].exp_start_line_min;
			}
			if( (real_vdocyc / 2 - (FLOAT)IMX533_MODE[mode].exp_end_line_offset) < req_SHR ){
				req_SHR = real_vdocyc / 2 - (FLOAT)IMX533_MODE[mode].exp_end_line_offset;
			}
			
			real_SHR = (USHORT)req_SHR;
		}
	}

	// real
	realExpTime = ( ((FLOAT)real_vdocyc - (FLOAT)real_SHR * shr_multi) * (FLOAT)IMX533_MODE[mode].hdocyc + IMX533_MODE[mode].exp_clk_offset) / (IMX533_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	realFps = (IMX533_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / real_vdocyc / IMX533_MODE[mode].hdocyc;

	// Out
	out->fps = realFps;
	out->exp_msec = realExpTime * 1000.0;
	out->hdocyc = IMX533_MODE[mode].hdocyc;
	out->vdocyc = real_vdocyc;
	
	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = out->hdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = out->vdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] = real_SHR;
	
	// --------------------
	// Analog Gain
	// --------------------
	again_dB = 20.0 * log10f(req_gain_times);
	
	work = 4095.0 * ( 1.0 - powf(10, (again_dB/(-20)) ) );
	
	if( work > 4030.0 ){
		PGC = 4030.0;
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
	dgain_dB = 0; // fix to 1.0 times (temporary)

	out->dgain_times = powf( 10, (dgain_dB / 20.0) );
	out_reg->reg[E_CAMERA_SENSOR_REG_DGAIN] = dgain_dB / 6;
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX533_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in)
{
	U_REG	reg;

	// Exposure Time: SHR
	g_backup_exposure_time_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_EXP];
	// Analog Gain: PGC
	g_backup_analog_gain_value[sensor_id] = (USHORT)in->reg[E_CAMERA_SENSOR_REG_AGAIN];

	// SHR
	reg.hword = (USHORT)g_backup_exposure_time_value[sensor_id];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x16, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
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
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x3E, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	
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

