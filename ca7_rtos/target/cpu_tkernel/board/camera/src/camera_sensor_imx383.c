/**
 * @file		
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "camera_sensor_imx383.h"
#include "camera_config.h"
#include "os_user_custom.h"
#include <math.h>
#include "ae_task.h"
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

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
const T_CAMERA_SENSOR_REG		IMX383_PLSTMG[] = 
{
	{ .address = 0x0019, .data[0] = 0x01 },
	{ .address = 0x00a7, .data[0] = 0xa5 },
	{ .address = 0x00a8, .data[0] = 0x07 },
	{ .address = 0x00B2, .data[0] = 0x01 },
	{ .address = 0x0300, .data[0] = 0x02 },
	{ .address = 0x0301, .data[0] = 0x00 },
	{ .address = 0x0302, .data[0] = 0x00 },
	{ .address = 0x0303, .data[0] = 0x00 },
	{ .address = 0x0304, .data[0] = 0x30 },
	{ .address = 0x0305, .data[0] = 0x22 },
	{ .address = 0x0306, .data[0] = 0x40 },
	{ .address = 0x0307, .data[0] = 0x41 },
	{ .address = 0x0308, .data[0] = 0xf8 },
	{ .address = 0x0309, .data[0] = 0x02 },
	{ .address = 0x030A, .data[0] = 0x01 },
	{ .address = 0x030B, .data[0] = 0x90 },
	{ .address = 0x030C, .data[0] = 0x24 },
	{ .address = 0x030D, .data[0] = 0x03 },
	{ .address = 0x030E, .data[0] = 0x01 },
	{ .address = 0x030F, .data[0] = 0x90 },
	{ .address = 0x0310, .data[0] = 0xa8 },
	{ .address = 0x0311, .data[0] = 0x03 },
	{ .address = 0x0312, .data[0] = 0x01 },
	{ .address = 0x0313, .data[0] = 0x90 },
	{ .address = 0x0314, .data[0] = 0xec },
	{ .address = 0x0315, .data[0] = 0x03 },
	{ .address = 0x0316, .data[0] = 0x01 },
	{ .address = 0x0317, .data[0] = 0x90 },
	{ .address = 0x0318, .data[0] = 0x06 },
	{ .address = 0x0319, .data[0] = 0x00 },
	{ .address = 0x031A, .data[0] = 0x00 },
	{ .address = 0x031B, .data[0] = 0x20 },
	{ .address = 0x031C, .data[0] = 0x90 },
	{ .address = 0x031D, .data[0] = 0x38 },
	{ .address = 0x031E, .data[0] = 0x80 },
	{ .address = 0x031F, .data[0] = 0x50 },
	{ .address = 0x0320, .data[0] = 0x04 },
	{ .address = 0x0321, .data[0] = 0x00 },
	{ .address = 0x0322, .data[0] = 0x00 },
	{ .address = 0x0323, .data[0] = 0x34 },
	{ .address = 0x0324, .data[0] = 0x04 },
	{ .address = 0x0325, .data[0] = 0x00 },
	{ .address = 0x0326, .data[0] = 0x00 },
	{ .address = 0x0327, .data[0] = 0x18 },
	{ .address = 0x0328, .data[0] = 0x90 },
	{ .address = 0x0329, .data[0] = 0x38 },
	{ .address = 0x032A, .data[0] = 0xc0 },
	{ .address = 0x032B, .data[0] = 0x50 },
	{ .address = 0x032C, .data[0] = 0x90 },
	{ .address = 0x032D, .data[0] = 0x04 },
	{ .address = 0x032E, .data[0] = 0x01 },
	{ .address = 0x032F, .data[0] = 0x90 },
	{ .address = 0x0330, .data[0] = 0x3c },
	{ .address = 0x0331, .data[0] = 0x05 },
	{ .address = 0x0332, .data[0] = 0x01 },
	{ .address = 0x0333, .data[0] = 0x90 },
	{ .address = 0x0334, .data[0] = 0x03 },
	{ .address = 0x0335, .data[0] = 0x04 },
	{ .address = 0x0336, .data[0] = 0x01 },
	{ .address = 0x0337, .data[0] = 0x20 },
	{ .address = 0x0338, .data[0] = 0x1c },
	{ .address = 0x0339, .data[0] = 0x30 },
	{ .address = 0x033A, .data[0] = 0x80 },
	{ .address = 0x033B, .data[0] = 0x50 },
	{ .address = 0x033C, .data[0] = 0x04 },
	{ .address = 0x033D, .data[0] = 0x00 },
	{ .address = 0x033E, .data[0] = 0x00 },
	{ .address = 0x033F, .data[0] = 0x34 },
	{ .address = 0x0340, .data[0] = 0x04 },
	{ .address = 0x0341, .data[0] = 0x00 },
	{ .address = 0x0342, .data[0] = 0x00 },
	{ .address = 0x0343, .data[0] = 0x18 },
	{ .address = 0x0344, .data[0] = 0x1c },
	{ .address = 0x0345, .data[0] = 0x30 },
	{ .address = 0x0346, .data[0] = 0xc0 },
	{ .address = 0x0347, .data[0] = 0x50 },
	{ .address = 0x0348, .data[0] = 0x04 },
	{ .address = 0x0349, .data[0] = 0x00 },
	{ .address = 0x034A, .data[0] = 0x00 },
	{ .address = 0x034B, .data[0] = 0x20 },
	{ .address = 0x034C, .data[0] = 0x84 },
	{ .address = 0x034D, .data[0] = 0x24 },
	{ .address = 0x034E, .data[0] = 0x80 },
	{ .address = 0x034F, .data[0] = 0x50 },
	{ .address = 0x0350, .data[0] = 0x04 },
	{ .address = 0x0351, .data[0] = 0x00 },
	{ .address = 0x0352, .data[0] = 0x00 },
	{ .address = 0x0353, .data[0] = 0x34 },
	{ .address = 0x0354, .data[0] = 0x04 },
	{ .address = 0x0355, .data[0] = 0x00 },
	{ .address = 0x0356, .data[0] = 0x00 },
	{ .address = 0x0357, .data[0] = 0x18 },
	{ .address = 0x0358, .data[0] = 0x84 },
	{ .address = 0x0359, .data[0] = 0x24 },
	{ .address = 0x035A, .data[0] = 0xc0 },
	{ .address = 0x035B, .data[0] = 0x50 },
	{ .address = 0x035C, .data[0] = 0x01 },
	{ .address = 0x035D, .data[0] = 0x00 },
	{ .address = 0x035E, .data[0] = 0x00 },
	{ .address = 0x035F, .data[0] = 0x20 },
	{ .address = 0x0360, .data[0] = 0x88 },
	{ .address = 0x0361, .data[0] = 0x24 },
	{ .address = 0x0362, .data[0] = 0x80 },
	{ .address = 0x0363, .data[0] = 0x50 },
	{ .address = 0x0364, .data[0] = 0x04 },
	{ .address = 0x0365, .data[0] = 0x00 },
	{ .address = 0x0366, .data[0] = 0x00 },
	{ .address = 0x0367, .data[0] = 0x34 },
	{ .address = 0x0368, .data[0] = 0x04 },
	{ .address = 0x0369, .data[0] = 0x00 },
	{ .address = 0x036A, .data[0] = 0x00 },
	{ .address = 0x036B, .data[0] = 0x18 },
	{ .address = 0x036C, .data[0] = 0x88 },
	{ .address = 0x036D, .data[0] = 0x24 },
	{ .address = 0x036E, .data[0] = 0xc0 },
	{ .address = 0x036F, .data[0] = 0x50 },
	{ .address = 0x0370, .data[0] = 0xa8 },
	{ .address = 0x0371, .data[0] = 0x05 },
	{ .address = 0x0372, .data[0] = 0x01 },
	{ .address = 0x0373, .data[0] = 0x90 },
	{ .address = 0x0374, .data[0] = 0x5c },
	{ .address = 0x0375, .data[0] = 0x06 },
	{ .address = 0x0376, .data[0] = 0x01 },
	{ .address = 0x0377, .data[0] = 0x90 },
	{ .address = 0x0378, .data[0] = 0x80 },
	{ .address = 0x0379, .data[0] = 0x01 },
	{ .address = 0x037A, .data[0] = 0x01 },
	{ .address = 0x037B, .data[0] = 0x10 },

	{ .address = 0x0739, .data[0] = 0x00 },
	{ .address = 0x073c, .data[0] = 0x0a },
	{ .address = 0x073d, .data[0] = 0x0a },
	{ .address = 0x073e, .data[0] = 0x0a },
	{ .address = 0x073f, .data[0] = 0x0a },
	{ .address = 0x0740, .data[0] = 0x0a },
	{ .address = 0x0741, .data[0] = 0x0a },
	{ .address = 0x0742, .data[0] = 0x0a },
	{ .address = 0x0743, .data[0] = 0x0a },
	{ .address = 0x0744, .data[0] = 0x0a },
	{ .address = 0x0745, .data[0] = 0x0a },
	{ .address = 0x0747, .data[0] = 0x0a },
	{ .address = 0x0755, .data[0] = 0xa4 },
	{ .address = 0x0756, .data[0] = 0xd6 },
	{ .address = 0x075b, .data[0] = 0x1c },
	{ .address = 0x075c, .data[0] = 0x1c },
	{ .address = 0x075d, .data[0] = 0x1c },
	{ .address = 0x075e, .data[0] = 0x1c },
	{ .address = 0x075f, .data[0] = 0x1c },
	{ .address = 0x07a3, .data[0] = 0x00 },
	{ .address = 0x07ee, .data[0] = 0xdf },
	{ .address = 0x07ef, .data[0] = 0x00 },
	{ .address = 0x07f0, .data[0] = 0xdd },
	{ .address = 0x07f1, .data[0] = 0x00 },

	{ .address = 0x0b8c, .data[0] = 0x00 },
	{ .address = 0x0bad, .data[0] = 0x00 },
	{ .address = 0x0baf, .data[0] = 0x0a },
	{ .address = 0x0bb0, .data[0] = 0x0a },
	{ .address = 0x0bb1, .data[0] = 0x0a },
	{ .address = 0x0bb2, .data[0] = 0x0a },
	{ .address = 0x0bb3, .data[0] = 0x0a },
	{ .address = 0x0bb4, .data[0] = 0x0a },
	{ .address = 0x0bb5, .data[0] = 0x0a },
	{ .address = 0x0bb6, .data[0] = 0x0a },
	{ .address = 0x0bb7, .data[0] = 0x0a },
	{ .address = 0x0bb8, .data[0] = 0x0a },
	{ .address = 0x0bb9, .data[0] = 0x0a },
	{ .address = 0x0bc0, .data[0] = 0xa4 },
	{ .address = 0x0bc1, .data[0] = 0xd6 },
	{ .address = 0x0bd8, .data[0] = 0x1c },
	{ .address = 0x0bd9, .data[0] = 0x1c },
	{ .address = 0x0bda, .data[0] = 0x1c },
	{ .address = 0x0bdb, .data[0] = 0x1c },
	{ .address = 0x0bdc, .data[0] = 0x1c },
	{ .address = 0x0bf2, .data[0] = 0xdf },
	{ .address = 0x0bf3, .data[0] = 0x00 },
	{ .address = 0x0bf4, .data[0] = 0xdd },
	{ .address = 0x0bf5, .data[0] = 0x00 },
	{ .address = 0x1000, .data[0] = 0x1e },
	{ .address = 0x1001, .data[0] = 0x1b },
	{ .address = 0x1002, .data[0] = 0x18 },
	{ .address = 0x1003, .data[0] = 0x10 },
	{ .address = 0x1008, .data[0] = 0x1a },
	{ .address = 0x1009, .data[0] = 0x18 },
	{ .address = 0x100a, .data[0] = 0x14 },
	{ .address = 0x1010, .data[0] = 0x18 },
	{ .address = 0x1011, .data[0] = 0x14 },
	{ .address = 0x1012, .data[0] = 0x10 },
	{ .address = 0x102b, .data[0] = 0x02 },
	{ .address = 0x1048, .data[0] = 0x1e },
	{ .address = 0x1049, .data[0] = 0x1b },
	{ .address = 0x104a, .data[0] = 0x18 },
	{ .address = 0x104b, .data[0] = 0x10 },
	{ .address = 0x1050, .data[0] = 0x14 },
	{ .address = 0x1051, .data[0] = 0x12 },
	{ .address = 0x1052, .data[0] = 0x0e },
	{ .address = 0x1058, .data[0] = 0x12 },
	{ .address = 0x1059, .data[0] = 0x0e },
	{ .address = 0x105a, .data[0] = 0x0a },
	{ .address = 0x11b5, .data[0] = 0x06 },
	{ .address = 0x11b6, .data[0] = 0x00 },
	{ .address = 0x11b9, .data[0] = 0x07 },
	{ .address = 0x11ba, .data[0] = 0x00 },
	{ .address = 0x11bd, .data[0] = 0x0a },
	{ .address = 0x11be, .data[0] = 0x00 },

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
	
};

const T_CAMERA_SENSOR_REG		IMX383_MODE_H4_V3_NO_1[] = 
{

	{ .address = 0x0003, .data[0] = 0x00 },
	{ .address = 0x0004, .data[0] = 0x07 },
	{ .address = 0x0005, .data[0] = 0x41 },
	{ .address = 0x0006, .data[0] = 0x10 },
	{ .address = 0x0007, .data[0] = 0x05 },

	{ .address = 0x0009, .data[0] = 0x00 },
	{ .address = 0x000a, .data[0] = 0x00 },
	{ .address = 0x000c, .data[0] = 0x00 },
	{ .address = 0x000e, .data[0] = 0x00 },
	{ .address = 0x0010, .data[0] = 0x00 },
	{ .address = 0x0011, .data[0] = 0x00 },
	{ .address = 0x0012, .data[0] = 0x49 },
	{ .address = 0x0013, .data[0] = 0x00 },
	{ .address = 0x0014, .data[0] = 0x0b },
	{ .address = 0x0015, .data[0] = 0x0f },
	{ .address = 0x006d, .data[0] = 0x0e },
	{ .address = 0x006e, .data[0] = 0x08 },
	{ .address = 0x0073, .data[0] = 0x12 },
	{ .address = 0x0075, .data[0] = 0x15 },
	{ .address = 0x0103, .data[0] = 0x00 },
	{ .address = 0x0104, .data[0] = 0x00 },
	{ .address = 0x0105, .data[0] = 0x00 },
	{ .address = 0x0106, .data[0] = 0x00 },
	{ .address = 0x0107, .data[0] = 0x00 },
	{ .address = 0x0108, .data[0] = 0x00 },
	{ .address = 0x010a, .data[0] = 0x00 },
	{ .address = 0x0132, .data[0] = 0x02 },
	{ .address = 0x0133, .data[0] = 0x02 },
	{ .address = 0x014e, .data[0] = 0x00 },
	{ .address = 0x014f, .data[0] = 0x00 },
	{ .address = 0x0150, .data[0] = 0x00 },
	{ .address = 0x0151, .data[0] = 0x00 },
	{ .address = 0x0203, .data[0] = 0x02 },
	{ .address = 0x0204, .data[0] = 0x02 },
	{ .address = 0x0205, .data[0] = 0x00 },
	{ .address = 0x0206, .data[0] = 0x00 },
	{ .address = 0x0207, .data[0] = 0x00 },
	{ .address = 0x0208, .data[0] = 0x00 },
	{ .address = 0x020a, .data[0] = 0x00 },
	{ .address = 0x0592, .data[0] = 0xaf },
	{ .address = 0x0593, .data[0] = 0x00 },
	{ .address = 0x05ab, .data[0] = 0x00 },
	{ .address = 0x05b9, .data[0] = 0x00 },
	{ .address = 0x05da, .data[0] = 0x00 },
	{ .address = 0x062a, .data[0] = 0x33 },
	{ .address = 0x062b, .data[0] = 0x00 },
	{ .address = 0x0674, .data[0] = 0x33 },
	{ .address = 0x0675, .data[0] = 0x00 },
	{ .address = 0x0676, .data[0] = 0x38 },
	{ .address = 0x0677, .data[0] = 0x00 },

	{ .address = 0x0746, .data[0] = 0x3f },
	{ .address = 0x075a, .data[0] = 0x1f },
	{ .address = 0x0798, .data[0] = 0x01 },
	{ .address = 0x08f6, .data[0] = 0x2a },
	{ .address = 0x0bba, .data[0] = 0x3f },
	{ .address = 0x0bd7, .data[0] = 0x1f },
	{ .address = 0x1500, .data[0] = 0x01 },



	{ .address = 0x1502, .data[0] = 0x00 },
	{ .address = 0x1503, .data[0] = 0x11 },
	{ .address = 0x1504, .data[0] = 0x00 },

	{ .address = 0x1506, .data[0] = 0x00 },
	{ .address = 0x1507, .data[0] = 0x00 },
	{ .address = 0x1508, .data[0] = 0x00 },
	{ .address = 0x1509, .data[0] = 0x00 },
	{ .address = 0x150a, .data[0] = 0x00 },
	{ .address = 0x1528, .data[0] = 0x00 },
	{ .address = 0x1529, .data[0] = 0x00 },
	{ .address = 0x152a, .data[0] = 0x00 },
	{ .address = 0x1538, .data[0] = 0x20 },
	{ .address = 0x1539, .data[0] = 0x20 },
	{ .address = 0x153a, .data[0] = 0x20 },
	{ .address = 0x153b, .data[0] = 0x20 },
	{ .address = 0x153c, .data[0] = 0xff },
	{ .address = 0x153d, .data[0] = 0xff },
	{ .address = 0x153e, .data[0] = 0xff },
	{ .address = 0x153f, .data[0] = 0xff },
	{ .address = 0x1540, .data[0] = 0x00 },
	{ .address = 0x1541, .data[0] = 0x0a },
	{ .address = 0x1542, .data[0] = 0x14 },
	{ .address = 0x1543, .data[0] = 0x20 },
	{ .address = 0x1544, .data[0] = 0x00 },
	{ .address = 0x1545, .data[0] = 0x0a },
	{ .address = 0x1546, .data[0] = 0x14 },
	{ .address = 0x1547, .data[0] = 0x20 },
	{ .address = 0x1548, .data[0] = 0x00 },
	{ .address = 0x1549, .data[0] = 0x0a },
	{ .address = 0x154a, .data[0] = 0x14 },
	{ .address = 0x154b, .data[0] = 0x20 },
	{ .address = 0x154c, .data[0] = 0x00 },
	{ .address = 0x154d, .data[0] = 0x0a },
	{ .address = 0x154e, .data[0] = 0x14 },
	{ .address = 0x154f, .data[0] = 0x20 },
	{ .address = 0x1550, .data[0] = 0x00 },
	{ .address = 0x1551, .data[0] = 0x0a },
	{ .address = 0x1552, .data[0] = 0x14 },
	{ .address = 0x1553, .data[0] = 0x20 },
	{ .address = 0x1554, .data[0] = 0x00 },
	{ .address = 0x1555, .data[0] = 0x0a },
	{ .address = 0x1556, .data[0] = 0x14 },
	{ .address = 0x1557, .data[0] = 0x20 },
	{ .address = 0x1558, .data[0] = 0x00 },
	{ .address = 0x1559, .data[0] = 0x50 },
	{ .address = 0x155a, .data[0] = 0xa0 },
	{ .address = 0x155b, .data[0] = 0xff },

	{ .address = 0x155d, .data[0] = 0x50 },
	{ .address = 0x155e, .data[0] = 0xa0 },
	{ .address = 0x155f, .data[0] = 0xff },

	{ .address = 0x1561, .data[0] = 0x50 },
	{ .address = 0x1562, .data[0] = 0xa0 },
	{ .address = 0x1563, .data[0] = 0xff },
	{ .address = 0x1564, .data[0] = 0x00 },
	{ .address = 0x1565, .data[0] = 0x50 },
	{ .address = 0x1566, .data[0] = 0xa0 },
	{ .address = 0x1567, .data[0] = 0xff },
	{ .address = 0x1569, .data[0] = 0x50 },
	{ .address = 0x156a, .data[0] = 0xa0 },
	{ .address = 0x156b, .data[0] = 0xff },
	{ .address = 0x156d, .data[0] = 0x50 },
	{ .address = 0x156e, .data[0] = 0xa0 },
	{ .address = 0x156f, .data[0] = 0xff },
	{ .address = 0x1570, .data[0] = 0x20 },
	{ .address = 0x1571, .data[0] = 0x20 },
	{ .address = 0x1572, .data[0] = 0x20 },
	{ .address = 0x1573, .data[0] = 0x20 },
	{ .address = 0x1574, .data[0] = 0x20 },
	{ .address = 0x1575, .data[0] = 0x20 },
	{ .address = 0x1576, .data[0] = 0x20 },
	{ .address = 0x1577, .data[0] = 0x20 },
	{ .address = 0x1578, .data[0] = 0x20 },
	{ .address = 0x1579, .data[0] = 0x20 },
	{ .address = 0x157a, .data[0] = 0x20 },
	{ .address = 0x157b, .data[0] = 0x20 },
	{ .address = 0x157c, .data[0] = 0x04 },
	{ .address = 0x157d, .data[0] = 0x04 },
	{ .address = 0x157e, .data[0] = 0x04 },
	{ .address = 0x157f, .data[0] = 0x04 },
	{ .address = 0x1580, .data[0] = 0x04 },
	{ .address = 0x1581, .data[0] = 0x04 },
	{ .address = 0x1582, .data[0] = 0x04 },
	{ .address = 0x1583, .data[0] = 0x04 },
	{ .address = 0x1584, .data[0] = 0x04 },
	{ .address = 0x1585, .data[0] = 0x04 },
	{ .address = 0x1586, .data[0] = 0x04 },
	{ .address = 0x1587, .data[0] = 0x04 },
	{ .address = 0x158e, .data[0] = 0x00 },
	{ .address = 0x1591, .data[0] = 0x00 },
	{ .address = 0x1594, .data[0] = 0x16 },
	{ .address = 0x1595, .data[0] = 0x16 },
	{ .address = 0x1596, .data[0] = 0xcc },
	{ .address = 0x1597, .data[0] = 0x0c },
	{ .address = 0x1598, .data[0] = 0x50 },
	{ .address = 0x1599, .data[0] = 0x00 },
	{ .address = 0x159a, .data[0] = 0xa0 },
	{ .address = 0x159b, .data[0] = 0x00 },
	{ .address = 0x159c, .data[0] = 0xff },
	{ .address = 0x159d, .data[0] = 0x00 },
	{ .address = 0x159e, .data[0] = 0x50 },
	{ .address = 0x159f, .data[0] = 0x00 },
	{ .address = 0x15a0, .data[0] = 0xa0 },
	{ .address = 0x15a1, .data[0] = 0x00 },
	{ .address = 0x15a2, .data[0] = 0xff },
	{ .address = 0x15a3, .data[0] = 0x00 },
	{ .address = 0x15a4, .data[0] = 0x50 },
	{ .address = 0x15a5, .data[0] = 0x00 },
	{ .address = 0x15a6, .data[0] = 0xa0 },
	{ .address = 0x15a7, .data[0] = 0x00 },
	{ .address = 0x15a8, .data[0] = 0xff },
	{ .address = 0x15a9, .data[0] = 0x00 },
	{ .address = 0x15aa, .data[0] = 0x50 },
	{ .address = 0x15ab, .data[0] = 0x00 },
	{ .address = 0x15ac, .data[0] = 0xa0 },
	{ .address = 0x15ad, .data[0] = 0x00 },
	{ .address = 0x15ae, .data[0] = 0xff },
	{ .address = 0x15af, .data[0] = 0x00 },
	{ .address = 0x15b0, .data[0] = 0x50 },
	{ .address = 0x15b1, .data[0] = 0x00 },
	{ .address = 0x15b2, .data[0] = 0xa0 },
	{ .address = 0x15b3, .data[0] = 0x00 },
	{ .address = 0x15b4, .data[0] = 0xff },
	{ .address = 0x15b5, .data[0] = 0x00 },
	{ .address = 0x15b6, .data[0] = 0x50 },
	{ .address = 0x15b7, .data[0] = 0x00 },
	{ .address = 0x15b8, .data[0] = 0xa0 },
	{ .address = 0x15b9, .data[0] = 0x00 },
	{ .address = 0x15ba, .data[0] = 0xff },
	{ .address = 0x15bb, .data[0] = 0x00 },
	{ .address = 0x15bc, .data[0] = 0x50 },
	{ .address = 0x15bd, .data[0] = 0x00 },
	{ .address = 0x15be, .data[0] = 0xa0 },
	{ .address = 0x15bf, .data[0] = 0x00 },
	{ .address = 0x15c0, .data[0] = 0xff },
	{ .address = 0x15c1, .data[0] = 0x00 },
	{ .address = 0x15c2, .data[0] = 0x50 },
	{ .address = 0x15c3, .data[0] = 0x00 },
	{ .address = 0x15c4, .data[0] = 0xa0 },
	{ .address = 0x15c5, .data[0] = 0x00 },
	{ .address = 0x15c6, .data[0] = 0xff },
	{ .address = 0x15c7, .data[0] = 0x00 },
	{ .address = 0x15c8, .data[0] = 0x50 },
	{ .address = 0x15c9, .data[0] = 0x00 },
	{ .address = 0x15ca, .data[0] = 0xa0 },
	{ .address = 0x15cb, .data[0] = 0x00 },
	{ .address = 0x15cc, .data[0] = 0xff },
	{ .address = 0x15cd, .data[0] = 0x00 },
	{ .address = 0x15ce, .data[0] = 0x50 },
	{ .address = 0x15cf, .data[0] = 0x00 },
	{ .address = 0x15d0, .data[0] = 0xa0 },
	{ .address = 0x15d1, .data[0] = 0x00 },
	{ .address = 0x15d2, .data[0] = 0xff },
	{ .address = 0x15d3, .data[0] = 0x00 },
	{ .address = 0x15d4, .data[0] = 0x50 },
	{ .address = 0x15d5, .data[0] = 0x00 },
	{ .address = 0x15d6, .data[0] = 0xa0 },
	{ .address = 0x15d7, .data[0] = 0x00 },
	{ .address = 0x15d8, .data[0] = 0xff },
	{ .address = 0x15d9, .data[0] = 0x00 },
	{ .address = 0x15da, .data[0] = 0x50 },
	{ .address = 0x15db, .data[0] = 0x00 },
	{ .address = 0x15dc, .data[0] = 0xa0 },
	{ .address = 0x15dd, .data[0] = 0x00 },
	{ .address = 0x15de, .data[0] = 0xff },
	{ .address = 0x15df, .data[0] = 0x00 },
	{ .address = 0x15e0, .data[0] = 0x0c },
	{ .address = 0x15e1, .data[0] = 0x0c },
	{ .address = 0x15e2, .data[0] = 0x0c },
	{ .address = 0x15e3, .data[0] = 0x08 },
	{ .address = 0x15e4, .data[0] = 0x08 },
	{ .address = 0x15e5, .data[0] = 0x08 },
	{ .address = 0x15e6, .data[0] = 0x0c },
	{ .address = 0x15e7, .data[0] = 0x0c },
	{ .address = 0x15e8, .data[0] = 0x0c },
	{ .address = 0x15e9, .data[0] = 0x08 },
	{ .address = 0x15ea, .data[0] = 0x08 },
	{ .address = 0x15eb, .data[0] = 0x08 },
	{ .address = 0x15ec, .data[0] = 0x0c },
	{ .address = 0x15ed, .data[0] = 0x0c },
	{ .address = 0x15ee, .data[0] = 0x0c },
	{ .address = 0x15ef, .data[0] = 0x08 },
	{ .address = 0x15f0, .data[0] = 0x08 },
	{ .address = 0x15f1, .data[0] = 0x08 },
	{ .address = 0x15f2, .data[0] = 0x0c },
	{ .address = 0x15f3, .data[0] = 0x0c },
	{ .address = 0x15f4, .data[0] = 0x0c },
	{ .address = 0x15f5, .data[0] = 0x08 },
	{ .address = 0x15f6, .data[0] = 0x08 },
	{ .address = 0x15f7, .data[0] = 0x08 },
	{ .address = 0x15f8, .data[0] = 0x00 },

	
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};

const T_CAMERA_SENSOR_REG		IMX383_MODE_H17_V9_NO_1[] = 
{
	{ .address = 0x0003, .data[0] = 0x00 },
	{ .address = 0x0004, .data[0] = 0x1f },
	{ .address = 0x0005, .data[0] = 0x59 },
	{ .address = 0x0006, .data[0] = 0x10 },
	{ .address = 0x0007, .data[0] = 0x05 },

	{ .address = 0x0009, .data[0] = 0x00 },
	{ .address = 0x000a, .data[0] = 0x00 },
	{ .address = 0x000c, .data[0] = 0x00 },
	{ .address = 0x000e, .data[0] = 0x00 },
	{ .address = 0x0010, .data[0] = 0x00 },
	{ .address = 0x0011, .data[0] = 0x00 },
	{ .address = 0x0012, .data[0] = 0x49 },
	{ .address = 0x0013, .data[0] = 0x00 },
	{ .address = 0x0014, .data[0] = 0xe4 },
	{ .address = 0x0015, .data[0] = 0x08 },
	{ .address = 0x006d, .data[0] = 0x0e },
	{ .address = 0x006e, .data[0] = 0x08 },
	{ .address = 0x0073, .data[0] = 0x12 },
	{ .address = 0x0075, .data[0] = 0x15 },
	{ .address = 0x0103, .data[0] = 0x02 },
	{ .address = 0x0104, .data[0] = 0x00 },
	{ .address = 0x0105, .data[0] = 0x00 },
	{ .address = 0x0106, .data[0] = 0x00 },
	{ .address = 0x0107, .data[0] = 0x00 },
	{ .address = 0x0108, .data[0] = 0x00 },
	{ .address = 0x010a, .data[0] = 0x00 },
	{ .address = 0x0132, .data[0] = 0x02 },
	{ .address = 0x0133, .data[0] = 0x02 },
	{ .address = 0x014e, .data[0] = 0x00 },
	{ .address = 0x014f, .data[0] = 0x00 },
	{ .address = 0x0150, .data[0] = 0x00 },
	{ .address = 0x0151, .data[0] = 0x00 },
	{ .address = 0x0203, .data[0] = 0x02 },
	{ .address = 0x0204, .data[0] = 0x02 },
	{ .address = 0x0205, .data[0] = 0x00 },
	{ .address = 0x0206, .data[0] = 0x00 },
	{ .address = 0x0207, .data[0] = 0x00 },
	{ .address = 0x0208, .data[0] = 0x00 },
	{ .address = 0x020a, .data[0] = 0x00 },
	{ .address = 0x0592, .data[0] = 0xaf },
	{ .address = 0x0593, .data[0] = 0x00 },
	{ .address = 0x05ab, .data[0] = 0x00 },
	{ .address = 0x05b9, .data[0] = 0x00 },
	{ .address = 0x05da, .data[0] = 0x00 },
	{ .address = 0x062a, .data[0] = 0x33 },
	{ .address = 0x062b, .data[0] = 0x00 },
	{ .address = 0x0674, .data[0] = 0x33 },
	{ .address = 0x0675, .data[0] = 0x00 },
	{ .address = 0x0676, .data[0] = 0x38 },
	{ .address = 0x0677, .data[0] = 0x00 },

	{ .address = 0x0746, .data[0] = 0x3c },
	{ .address = 0x075a, .data[0] = 0x1c },
	{ .address = 0x0798, .data[0] = 0x04 },
	{ .address = 0x08f6, .data[0] = 0x2a },
	{ .address = 0x0bba, .data[0] = 0x3c },
	{ .address = 0x0bd7, .data[0] = 0x1c },
	{ .address = 0x1500, .data[0] = 0x01 },
	

	{ .address = 0x1502, .data[0] = 0x00 },
	{ .address = 0x1503, .data[0] = 0x11 },
	{ .address = 0x1504, .data[0] = 0x00 },

	{ .address = 0x1506, .data[0] = 0x00 },
	{ .address = 0x1507, .data[0] = 0x00 },
	{ .address = 0x1508, .data[0] = 0x00 },
	{ .address = 0x1509, .data[0] = 0x00 },
	{ .address = 0x150a, .data[0] = 0x00 },
	{ .address = 0x1528, .data[0] = 0x00 },
	{ .address = 0x1529, .data[0] = 0x00 },
	{ .address = 0x152a, .data[0] = 0x00 },
	{ .address = 0x1538, .data[0] = 0x20 },
	{ .address = 0x1539, .data[0] = 0x20 },
	{ .address = 0x153a, .data[0] = 0x20 },
	{ .address = 0x153b, .data[0] = 0x20 },
	{ .address = 0x153c, .data[0] = 0xff },
	{ .address = 0x153d, .data[0] = 0xff },
	{ .address = 0x153e, .data[0] = 0xff },
	{ .address = 0x153f, .data[0] = 0xff },
	{ .address = 0x1540, .data[0] = 0x00 },
	{ .address = 0x1541, .data[0] = 0x0a },
	{ .address = 0x1542, .data[0] = 0x14 },
	{ .address = 0x1543, .data[0] = 0x20 },
	{ .address = 0x1544, .data[0] = 0x00 },
	{ .address = 0x1545, .data[0] = 0x0a },
	{ .address = 0x1546, .data[0] = 0x14 },
	{ .address = 0x1547, .data[0] = 0x20 },
	{ .address = 0x1548, .data[0] = 0x00 },
	{ .address = 0x1549, .data[0] = 0x0a },
	{ .address = 0x154a, .data[0] = 0x14 },
	{ .address = 0x154b, .data[0] = 0x20 },
	{ .address = 0x154c, .data[0] = 0x00 },
	{ .address = 0x154d, .data[0] = 0x0a },
	{ .address = 0x154e, .data[0] = 0x14 },
	{ .address = 0x154f, .data[0] = 0x20 },
	{ .address = 0x1550, .data[0] = 0x00 },
	{ .address = 0x1551, .data[0] = 0x0a },
	{ .address = 0x1552, .data[0] = 0x14 },
	{ .address = 0x1553, .data[0] = 0x20 },
	{ .address = 0x1554, .data[0] = 0x00 },
	{ .address = 0x1555, .data[0] = 0x0a },
	{ .address = 0x1556, .data[0] = 0x14 },
	{ .address = 0x1557, .data[0] = 0x20 },
	{ .address = 0x1558, .data[0] = 0x00 },
	{ .address = 0x1559, .data[0] = 0x50 },
	{ .address = 0x155a, .data[0] = 0xa0 },
	{ .address = 0x155b, .data[0] = 0xff },

	{ .address = 0x155d, .data[0] = 0x50 },
	{ .address = 0x155e, .data[0] = 0xa0 },
	{ .address = 0x155f, .data[0] = 0xff },

	{ .address = 0x1561, .data[0] = 0x50 },
	{ .address = 0x1562, .data[0] = 0xa0 },
	{ .address = 0x1563, .data[0] = 0xff },
	{ .address = 0x1564, .data[0] = 0x00 },
	{ .address = 0x1565, .data[0] = 0x50 },
	{ .address = 0x1566, .data[0] = 0xa0 },
	{ .address = 0x1567, .data[0] = 0xff },
	{ .address = 0x1569, .data[0] = 0x50 },
	{ .address = 0x156a, .data[0] = 0xa0 },
	{ .address = 0x156b, .data[0] = 0xff },
	{ .address = 0x156d, .data[0] = 0x50 },
	{ .address = 0x156e, .data[0] = 0xa0 },
	{ .address = 0x156f, .data[0] = 0xff },
	{ .address = 0x1570, .data[0] = 0x20 },
	{ .address = 0x1571, .data[0] = 0x20 },
	{ .address = 0x1572, .data[0] = 0x20 },
	{ .address = 0x1573, .data[0] = 0x20 },
	{ .address = 0x1574, .data[0] = 0x20 },
	{ .address = 0x1575, .data[0] = 0x20 },
	{ .address = 0x1576, .data[0] = 0x20 },
	{ .address = 0x1577, .data[0] = 0x20 },
	{ .address = 0x1578, .data[0] = 0x20 },
	{ .address = 0x1579, .data[0] = 0x20 },
	{ .address = 0x157a, .data[0] = 0x20 },
	{ .address = 0x157b, .data[0] = 0x20 },
	{ .address = 0x157c, .data[0] = 0x04 },
	{ .address = 0x157d, .data[0] = 0x04 },
	{ .address = 0x157e, .data[0] = 0x04 },
	{ .address = 0x157f, .data[0] = 0x04 },
	{ .address = 0x1580, .data[0] = 0x04 },
	{ .address = 0x1581, .data[0] = 0x04 },
	{ .address = 0x1582, .data[0] = 0x04 },
	{ .address = 0x1583, .data[0] = 0x04 },
	{ .address = 0x1584, .data[0] = 0x04 },
	{ .address = 0x1585, .data[0] = 0x04 },
	{ .address = 0x1586, .data[0] = 0x04 },
	{ .address = 0x1587, .data[0] = 0x04 },
	{ .address = 0x158e, .data[0] = 0x00 },
	{ .address = 0x1591, .data[0] = 0x00 },
	{ .address = 0x1594, .data[0] = 0x16 },
	{ .address = 0x1595, .data[0] = 0x16 },
	{ .address = 0x1596, .data[0] = 0xcc },
	{ .address = 0x1597, .data[0] = 0x0c },
	{ .address = 0x1598, .data[0] = 0x50 },
	{ .address = 0x1599, .data[0] = 0x00 },
	{ .address = 0x159a, .data[0] = 0xa0 },
	{ .address = 0x159b, .data[0] = 0x00 },
	{ .address = 0x159c, .data[0] = 0xff },
	{ .address = 0x159d, .data[0] = 0x00 },
	{ .address = 0x159e, .data[0] = 0x50 },
	{ .address = 0x159f, .data[0] = 0x00 },
	{ .address = 0x15a0, .data[0] = 0xa0 },
	{ .address = 0x15a1, .data[0] = 0x00 },
	{ .address = 0x15a2, .data[0] = 0xff },
	{ .address = 0x15a3, .data[0] = 0x00 },
	{ .address = 0x15a4, .data[0] = 0x50 },
	{ .address = 0x15a5, .data[0] = 0x00 },
	{ .address = 0x15a6, .data[0] = 0xa0 },
	{ .address = 0x15a7, .data[0] = 0x00 },
	{ .address = 0x15a8, .data[0] = 0xff },
	{ .address = 0x15a9, .data[0] = 0x00 },
	{ .address = 0x15aa, .data[0] = 0x50 },
	{ .address = 0x15ab, .data[0] = 0x00 },
	{ .address = 0x15ac, .data[0] = 0xa0 },
	{ .address = 0x15ad, .data[0] = 0x00 },
	{ .address = 0x15ae, .data[0] = 0xff },
	{ .address = 0x15af, .data[0] = 0x00 },
	{ .address = 0x15b0, .data[0] = 0x50 },
	{ .address = 0x15b1, .data[0] = 0x00 },
	{ .address = 0x15b2, .data[0] = 0xa0 },
	{ .address = 0x15b3, .data[0] = 0x00 },
	{ .address = 0x15b4, .data[0] = 0xff },
	{ .address = 0x15b5, .data[0] = 0x00 },
	{ .address = 0x15b6, .data[0] = 0x50 },
	{ .address = 0x15b7, .data[0] = 0x00 },
	{ .address = 0x15b8, .data[0] = 0xa0 },
	{ .address = 0x15b9, .data[0] = 0x00 },
	{ .address = 0x15ba, .data[0] = 0xff },
	{ .address = 0x15bb, .data[0] = 0x00 },
	{ .address = 0x15bc, .data[0] = 0x50 },
	{ .address = 0x15bd, .data[0] = 0x00 },
	{ .address = 0x15be, .data[0] = 0xa0 },
	{ .address = 0x15bf, .data[0] = 0x00 },
	{ .address = 0x15c0, .data[0] = 0xff },
	{ .address = 0x15c1, .data[0] = 0x00 },
	{ .address = 0x15c2, .data[0] = 0x50 },
	{ .address = 0x15c3, .data[0] = 0x00 },
	{ .address = 0x15c4, .data[0] = 0xa0 },
	{ .address = 0x15c5, .data[0] = 0x00 },
	{ .address = 0x15c6, .data[0] = 0xff },
	{ .address = 0x15c7, .data[0] = 0x00 },
	{ .address = 0x15c8, .data[0] = 0x50 },
	{ .address = 0x15c9, .data[0] = 0x00 },
	{ .address = 0x15ca, .data[0] = 0xa0 },
	{ .address = 0x15cb, .data[0] = 0x00 },
	{ .address = 0x15cc, .data[0] = 0xff },
	{ .address = 0x15cd, .data[0] = 0x00 },
	{ .address = 0x15ce, .data[0] = 0x50 },
	{ .address = 0x15cf, .data[0] = 0x00 },
	{ .address = 0x15d0, .data[0] = 0xa0 },
	{ .address = 0x15d1, .data[0] = 0x00 },
	{ .address = 0x15d2, .data[0] = 0xff },
	{ .address = 0x15d3, .data[0] = 0x00 },
	{ .address = 0x15d4, .data[0] = 0x50 },
	{ .address = 0x15d5, .data[0] = 0x00 },
	{ .address = 0x15d6, .data[0] = 0xa0 },
	{ .address = 0x15d7, .data[0] = 0x00 },
	{ .address = 0x15d8, .data[0] = 0xff },
	{ .address = 0x15d9, .data[0] = 0x00 },
	{ .address = 0x15da, .data[0] = 0x50 },
	{ .address = 0x15db, .data[0] = 0x00 },
	{ .address = 0x15dc, .data[0] = 0xa0 },
	{ .address = 0x15dd, .data[0] = 0x00 },
	{ .address = 0x15de, .data[0] = 0xff },
	{ .address = 0x15df, .data[0] = 0x00 },
	{ .address = 0x15e0, .data[0] = 0x0c },
	{ .address = 0x15e1, .data[0] = 0x0c },
	{ .address = 0x15e2, .data[0] = 0x0c },
	{ .address = 0x15e3, .data[0] = 0x08 },
	{ .address = 0x15e4, .data[0] = 0x08 },
	{ .address = 0x15e5, .data[0] = 0x08 },
	{ .address = 0x15e6, .data[0] = 0x0c },
	{ .address = 0x15e7, .data[0] = 0x0c },
	{ .address = 0x15e8, .data[0] = 0x0c },
	{ .address = 0x15e9, .data[0] = 0x08 },
	{ .address = 0x15ea, .data[0] = 0x08 },
	{ .address = 0x15eb, .data[0] = 0x08 },
	{ .address = 0x15ec, .data[0] = 0x0c },
	{ .address = 0x15ed, .data[0] = 0x0c },
	{ .address = 0x15ee, .data[0] = 0x0c },
	{ .address = 0x15ef, .data[0] = 0x08 },
	{ .address = 0x15f0, .data[0] = 0x08 },
	{ .address = 0x15f1, .data[0] = 0x08 },
	{ .address = 0x15f2, .data[0] = 0x0c },
	{ .address = 0x15f3, .data[0] = 0x0c },
	{ .address = 0x15f4, .data[0] = 0x0c },
	{ .address = 0x15f5, .data[0] = 0x08 },
	{ .address = 0x15f6, .data[0] = 0x08 },
	{ .address = 0x15f7, .data[0] = 0x08 },
	{ .address = 0x15f8, .data[0] = 0x00 },
	
	
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
};

const T_CAMERA_SENSOR_BASIC		IMX383_BASIC =
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_72MHz,
	.access			= E_CAMERA_ACCESS_SPI,
	.sensor_if		= E_CAMERA_SENSOR_IF_SLVS,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_V_BLANKING,
};

const T_CAMERA_SENSOR_MODE		IMX383_MODE[E_SENSOR_IMX383_MODE_MAX] =
{
	[E_SENSOR_IMX383_MODE_H17_V9_NO_1] = {
		.mode_reg				= IMX383_MODE_H17_V9_NO_1,
		
		.embedded_lines			= 0,
		.total_pix_width		= 4264,
		.total_pix_lines		= (2182+18),
		.ob_pix_x_pos			= (96+12),
		.ob_pix_y_pos			= (0+18),
		.ob_pix_width			= 4128,
		.ob_pix_lines			= 8,
		.active_pix_x_pos		= (96+12),
		.active_pix_y_pos		= (0+8+18),
		.active_pix_width		= 4128,
		.active_pix_lines		= (2174+6),// need +6 for 4k2k, +6 area is v blank with HD
		.active_pix_x_rsz_ratio	= 1.0/1.4,
		.active_pix_y_rsz_ratio	= 1.0/1.4,
		.hdocyc					= 458,
		.vdocyc					= 2200,
		.exp_start_line_min		= 8,
		.exp_end_line_offset	= 4,
		.exp_clk_offset			= 132,
		.base_SV				= 4.0,
		.input_bit_width		= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.fps					= 71.45, // 72.0 / 2200 / 458
		.Mpps_HDclk				= 72.0,	// set INCLK for SLVS
		.one_lane_Mbps			= 0.0,	// no need for SLVS
		.lane_num				= 4,
	},
	[E_SENSOR_IMX383_MODE_H4_V3_NO_1] = {
		.mode_reg				= IMX383_MODE_H4_V3_NO_1,

		.embedded_lines			= 0,
		.total_pix_width		= 5640,
		.total_pix_lines		= 3710,
		.ob_pix_x_pos			= (96+24),
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 5496,
		.ob_pix_lines			= 16,
		.active_pix_x_pos		= (96+24+12),
		.active_pix_y_pos		= (16+12+12),
		.active_pix_width		= 5472,
		.active_pix_lines		= 3648,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.hdocyc					= 311 * 2, // *2 to slow 
		.vdocyc					= 3728 * 2,// *2 to slow
		.exp_start_line_min		= 8,
		.exp_end_line_offset	= 4,
		.exp_clk_offset			= 132,
		.base_SV				= 4.0,
		.input_bit_width		= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.fps					= 15.52, // 72 / (3728 * 2) / (311 * 2)
		.Mpps_HDclk				= 72.0,	// set INCLK for SLVS
		.one_lane_Mbps			= 0.0,	// no need for SLVS
		.lane_num				= 8,
	}
};

static USHORT g_backup_analog_gain_value[4];//★Sensor数

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define ROUNDUP2(a) (((a) + 1) & 0xFFFFFFFE)

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX383_Clock_Gpio_Init(VOID)
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

	// SP0SCK Pull Up ★SP1は？
	Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PD2, 1 );
	Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PD2, 1 );
	
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

E_CAMERA_ERR_CODE Sensor_IMX383_Standby(E_CAMERA_ID sensor_id)
{
	// SG Force Stop
	CAMERA_PRINT_ERROR( FJ_SgStop( sensor_id, 1 ) );
	
	// ★
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX383_CancelStandby(E_CAMERA_ID sensor_id)
{
	U_REG		reg;
	
	// CRCEN=1(With CRC), ECCEN=0(without ECC)
	reg.hword = 0x82;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0127, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	
	// set until V1
	access_tbl(sensor_id, IMX383_PLSTMG);
	
	// 1.
	reg.hword = 0x0F;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x04EB, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	
	reg.hword = 0x82;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0000, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	
	// 2.
	OS_User_Dly_Tsk(10);
	
	reg.hword = 0x00;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x04EB, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	
	// 3.
	OS_User_Dly_Tsk(1);
	
	reg.hword = 0x80;
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x0000, &(reg.byte[0]), 1, CAMERA.sensor[sensor_id].is_spics_gpio);
	
	// 4.
	OS_User_Dly_Tsk(8);
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX383_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol)
{
	g_backup_analog_gain_value[sensor_id] = 0xFFFF;
	
	// MODE
	access_tbl(sensor_id, IMX383_MODE[mode].mode_reg);
	
	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX383_SgVHD(E_CAMERA_ID sensor_id, ULONG vdocyc, ULONG hdocyc)
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

E_CAMERA_ERR_CODE Sensor_IMX383_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
{
	FLOAT	reqFps;
	FLOAT	minFps, maxFps, defaultFps;
	FLOAT	req_vdocyc;
	FLOAT	req_SHR;
	USHORT	real_vdocyc;
	USHORT	real_SHR;
	FLOAT	realExpTime;
	FLOAT	realFps;
	
	FLOAT	again_dB;
	FLOAT	work;
	USHORT	PGC;
	FLOAT req_exp_msec = 16.666;
	DOUBLE	realExpClk, _realExpTime;
	
	// --------------------
	// Exposur Time
	// --------------------
	// Time = [ { vdocyc * (SVR+1) - SHR } * hdocyc + offset ] / INCK
	// -> (Time * INCK - offset ) / hdocyc = vdocyc - SHR (SVR=0 fixed)
	// -> (Time * INCK - offset ) / hdocyc ... (a)
	// -> vdocyc - SHR         ... (b)

	req_exp_msec = in->exp_msec;
	
	// req_vdocyc = (a)
	req_vdocyc = ( (req_exp_msec / 1000.0) * (IMX383_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) - (FLOAT)IMX383_MODE[mode].exp_clk_offset) / (FLOAT)IMX383_MODE[mode].hdocyc;
	if( req_vdocyc < 0.0 ){
		req_vdocyc = 0.0;
	}
	
	// (a) = (b)
	// -> req_vdocyc = vdocyc - SHR
	// -> SHR = vdocyc - req_vdocyc
	req_SHR = (FLOAT)IMX383_MODE[mode].vdocyc - req_vdocyc;
	
	// find vdocyc and SHR combination
	if( req_SHR < (FLOAT)IMX383_MODE[mode].exp_start_line_min ){
		// extend vdocyc
		real_SHR = IMX383_MODE[mode].exp_start_line_min;
		real_vdocyc = (USHORT)( req_vdocyc + (FLOAT)IMX383_MODE[mode].exp_start_line_min );
	}
	else if( (IMX383_MODE[mode].vdocyc - IMX383_MODE[mode].exp_end_line_offset) < req_SHR ){
		// clip
		real_SHR = (IMX383_MODE[mode].vdocyc - IMX383_MODE[mode].exp_end_line_offset);
		real_vdocyc = IMX383_MODE[mode].vdocyc;
	}
	else{
		real_SHR = (USHORT)req_SHR;
		real_vdocyc = IMX383_MODE[mode].vdocyc;
	}
	
	// FPS
	if( D_CAMERA_FPS_AUTO == in->min_fps && D_CAMERA_FPS_AUTO == in->max_fps ){
		// nothing to do
	}
	else{
		// default
		defaultFps = (IMX383_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / IMX383_MODE[mode].hdocyc / IMX383_MODE[mode].vdocyc;
		
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
		reqFps = (IMX383_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX383_MODE[mode].hdocyc / (FLOAT)real_vdocyc;
		if( reqFps < minFps ){
			req_vdocyc   = (IMX383_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX383_MODE[mode].hdocyc / minFps;
			real_vdocyc  = (USHORT)req_vdocyc;
		}
		if( reqFps > maxFps ){
			req_vdocyc   = (IMX383_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX383_MODE[mode].hdocyc / maxFps;
			real_vdocyc  = (USHORT)req_vdocyc;
		}
		
		if (real_SHR > (real_vdocyc - IMX383_MODE[mode].exp_end_line_offset)) {
            req_SHR     = real_vdocyc - IMX383_MODE[mode].exp_end_line_offset;
            realExpClk  = (DOUBLE)(real_vdocyc - req_SHR) * IMX383_MODE[mode].hdocyc;
			_realExpTime = realExpClk / (DOUBLE)(IMX383_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
 			_realExpTime = (DOUBLE)expTimeQuantization(_realExpTime * 1000, in->flicker_mode, QUANTIZ_ROUND_OFF);
			req_SHR	    = (ULONG)( (_realExpTime / 1000.0) * (DOUBLE)(IMX383_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (DOUBLE)IMX383_MODE[mode].hdocyc);
            real_SHR    = real_vdocyc - req_SHR;

            if (real_SHR < (FLOAT)IMX383_MODE[mode].exp_start_line_min)
                real_SHR = IMX383_MODE[mode].exp_start_line_min;

            if (real_SHR > (real_vdocyc - IMX383_MODE[mode].exp_end_line_offset))
                real_SHR = real_vdocyc - IMX383_MODE[mode].exp_end_line_offset;
        }
	}

	real_vdocyc = ROUNDUP2(real_vdocyc);
	
	// real
	realExpTime = ( ((FLOAT)real_vdocyc - (FLOAT)real_SHR) * (FLOAT)IMX383_MODE[mode].hdocyc + IMX383_MODE[mode].exp_clk_offset) / (IMX383_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) ;
	realFps = (IMX383_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / real_vdocyc / IMX383_MODE[mode].hdocyc;
	
	// Out
	out->fps = realFps;
	out->exp_msec = realExpTime * 1000.0;
	out->hdocyc = IMX383_MODE[mode].hdocyc;
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


E_CAMERA_ERR_CODE Sensor_IMX383_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in)
{
	U_REG	reg;
	
	// SHR
	reg.hword = (USHORT)in->reg[E_CAMERA_SENSOR_REG_EXP];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x120, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	
	// Analog Gain: PGC
	if( g_backup_analog_gain_value[sensor_id] == 0xFFFF ){
		g_backup_analog_gain_value[sensor_id] = in->reg[E_CAMERA_SENSOR_REG_AGAIN];
	}
	reg.hword = (USHORT)g_backup_analog_gain_value[sensor_id];
	g_backup_analog_gain_value[sensor_id] = in->reg[E_CAMERA_SENSOR_REG_AGAIN];
	Camera_Common_SPI_Write(CAMERA.sensor[sensor_id].spi_ch, 0x17e, &(reg.byte[0]), 2, CAMERA.sensor[sensor_id].is_spics_gpio);
	
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
