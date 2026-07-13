/**
 * @file
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "camera_sensor_imx577.h"
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

static BOOL	gIs_Mode_Transition = FALSE; // If TRUE, no need to do standby cancellation process. It should depends on sensor spec
/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
const T_CAMERA_SENSOR_REG IMX577_INIT[] =
{
    { .address = 0x0136	, .data[0] = 0x18 },
    { .address = 0x0137	, .data[0] = 0x00 },
                                      
    { .address = 0x3C7E	, .data[0] = 0x01 },
    { .address = 0x3C7F	, .data[0] = 0x02 },
                                      
    { .address = 0x38A8	, .data[0] = 0x1F },
    { .address = 0x38A9	, .data[0] = 0xFF },
    { .address = 0x38AA	, .data[0] = 0x1F },
    { .address = 0x38AB	, .data[0] = 0xFF },
    { .address = 0x55D4	, .data[0] = 0x00 },
    { .address = 0x55D5	, .data[0] = 0x00 },
    { .address = 0x55D6	, .data[0] = 0x07 },
    { .address = 0x55D7	, .data[0] = 0xFF },
    { .address = 0x55E8	, .data[0] = 0x07 },
    { .address = 0x55E9	, .data[0] = 0xFF },
    { .address = 0x55EA	, .data[0] = 0x00 },
    { .address = 0x55EB	, .data[0] = 0x00 },
    { .address = 0x575C	, .data[0] = 0x07 },
    { .address = 0x575D	, .data[0] = 0xFF },
    { .address = 0x575E	, .data[0] = 0x00 },
    { .address = 0x575F	, .data[0] = 0x00 },
    { .address = 0x5764	, .data[0] = 0x00 },
    { .address = 0x5765	, .data[0] = 0x00 },
    { .address = 0x5766	, .data[0] = 0x07 },
    { .address = 0x5767	, .data[0] = 0xFF },
    { .address = 0x5974	, .data[0] = 0x04 },
    { .address = 0x5975	, .data[0] = 0x01 },
    { .address = 0x5F10	, .data[0] = 0x09 },
    { .address = 0x5F11	, .data[0] = 0x92 },
    { .address = 0x5F12	, .data[0] = 0x32 },
    { .address = 0x5F13	, .data[0] = 0x72 },
    { .address = 0x5F14	, .data[0] = 0x16 },
    { .address = 0x5F15	, .data[0] = 0xBA },
    { .address = 0x5F17	, .data[0] = 0x13 },
    { .address = 0x5F18	, .data[0] = 0x24 },
    { .address = 0x5F19	, .data[0] = 0x60 },
    { .address = 0x5F1A	, .data[0] = 0xE3 },
    { .address = 0x5F1B	, .data[0] = 0xAD },
    { .address = 0x5F1C	, .data[0] = 0x74 },
    { .address = 0x5F2D	, .data[0] = 0x25 },
    { .address = 0x5F5C	, .data[0] = 0xD0 },
    { .address = 0x6A22	, .data[0] = 0x00 },
    { .address = 0x6A23	, .data[0] = 0x1D },
    { .address = 0x7BA8	, .data[0] = 0x00 },
    { .address = 0x7BA9	, .data[0] = 0x00 },
    { .address = 0x886B	, .data[0] = 0x00 },
    { .address = 0x9002	, .data[0] = 0x0A },
    { .address = 0x9004	, .data[0] = 0x1A },
    { .address = 0x9214	, .data[0] = 0x93 },
    { .address = 0x9215	, .data[0] = 0x69 },
    { .address = 0x9216	, .data[0] = 0x93 },
    { .address = 0x9217	, .data[0] = 0x6B },
    { .address = 0x9218	, .data[0] = 0x93 },
    { .address = 0x9219	, .data[0] = 0x6D },
    { .address = 0x921A	, .data[0] = 0x57 },
    { .address = 0x921B	, .data[0] = 0x58 },
    { .address = 0x921C	, .data[0] = 0x57 },
    { .address = 0x921D	, .data[0] = 0x59 },
    { .address = 0x921E	, .data[0] = 0x57 },
    { .address = 0x921F	, .data[0] = 0x5A },
    { .address = 0x9220	, .data[0] = 0x57 },
    { .address = 0x9221	, .data[0] = 0x5B },
    { .address = 0x9222	, .data[0] = 0x93 },
    { .address = 0x9223	, .data[0] = 0x02 },
    { .address = 0x9224	, .data[0] = 0x93 },
    { .address = 0x9225	, .data[0] = 0x03 },
    { .address = 0x9226	, .data[0] = 0x93 },
    { .address = 0x9227	, .data[0] = 0x04 },
    { .address = 0x9228	, .data[0] = 0x93 },
    { .address = 0x9229	, .data[0] = 0x05 },
    { .address = 0x922A	, .data[0] = 0x98 },
    { .address = 0x922B	, .data[0] = 0x21 },
    { .address = 0x922C	, .data[0] = 0xB2 },
    { .address = 0x922D	, .data[0] = 0xDB },
    { .address = 0x922E	, .data[0] = 0xB2 },
    { .address = 0x922F	, .data[0] = 0xDC },
    { .address = 0x9230	, .data[0] = 0xB2 },
    { .address = 0x9231	, .data[0] = 0xDD },
    { .address = 0x9232	, .data[0] = 0xB2 },
    { .address = 0x9233	, .data[0] = 0xE1 },
    { .address = 0x9234	, .data[0] = 0xB2 },
    { .address = 0x9235	, .data[0] = 0xE2 },
    { .address = 0x9236	, .data[0] = 0xB2 },
    { .address = 0x9237	, .data[0] = 0xE3 },
    { .address = 0x9238	, .data[0] = 0xB7 },
    { .address = 0x9239	, .data[0] = 0xB9 },  
    { .address = 0x923A	, .data[0] = 0xB7 },  
    { .address = 0x923B	, .data[0] = 0xBB },  
    { .address = 0x923C	, .data[0] = 0xB7 },  
    { .address = 0x923D	, .data[0] = 0xBC },  
    { .address = 0x923E	, .data[0] = 0xB7 },  
    { .address = 0x923F	, .data[0] = 0xC5 },  
    { .address = 0x9240	, .data[0] = 0xB7 },  
    { .address = 0x9241	, .data[0] = 0xC7 },  
    { .address = 0x9242	, .data[0] = 0xB7 },  
    { .address = 0x9243	, .data[0] = 0xC9 },  
    { .address = 0x9244	, .data[0] = 0x98 },  
    { .address = 0x9245	, .data[0] = 0x56 },  
    { .address = 0x9246	, .data[0] = 0x98 },  
    { .address = 0x9247	, .data[0] = 0x55 },  
    { .address = 0x9380	, .data[0] = 0x00 },  
    { .address = 0x9381	, .data[0] = 0x62 },  
    { .address = 0x9382	, .data[0] = 0x00 },  
    { .address = 0x9383	, .data[0] = 0x56 },  
    { .address = 0x9384	, .data[0] = 0x00 },  
    { .address = 0x9385	, .data[0] = 0x52 },  
    { .address = 0x9388	, .data[0] = 0x00 },  
    { .address = 0x9389	, .data[0] = 0x55 },  
    { .address = 0x938A	, .data[0] = 0x00 },  
    { .address = 0x938B	, .data[0] = 0x55 },  
    { .address = 0x938C	, .data[0] = 0x00 },
    { .address = 0x938D	, .data[0] = 0x41 },
    { .address = 0x5078	, .data[0] = 0x01 },


	{ .address = D_TBL_END, .data[0] = 0xFF }		// END

};

const T_CAMERA_SENSOR_REG		IMX577_MODE_Full_4056x2288_RAW10_60fps[] =
{
    { .address = 0x0112, .data[0] =	0x0A },
    { .address = 0x0113, .data[0] =	0x0A },
    { .address = 0x0114, .data[0] =	0x03 },

    { .address = 0x0342, .data[0] =	0x11 },
    { .address = 0x0343, .data[0] =	0xA0 },

    { .address = 0x0340, .data[0] =	0x0C },
    { .address = 0x0341, .data[0] =	0x1E },
    { .address = 0x3210, .data[0] =	0x00 },

    { .address = 0x0344, .data[0] =	0x00 },
    { .address = 0x0345, .data[0] =	0x00 },
    { .address = 0x0346, .data[0] =	0x01 },
    { .address = 0x0347, .data[0] =	0x78 },
    { .address = 0x0348, .data[0] =	0x0F },
    { .address = 0x0349, .data[0] =	0xD7 },
    { .address = 0x034A, .data[0] =	0x0A },
    { .address = 0x034B, .data[0] =	0x67 },

    { .address = 0x00E3, .data[0] =	0x00 },
    { .address = 0x00E4, .data[0] =	0x00 },
    { .address = 0x00E5, .data[0] =	0x01 },
    { .address = 0x00FC, .data[0] =	0x0A },
    { .address = 0x00FD, .data[0] =	0x0A },
    { .address = 0x00FE, .data[0] =	0x0A },
    { .address = 0x00FF, .data[0] =	0x0A },
    { .address = 0xE013, .data[0] =	0x00 },
    { .address = 0x0220, .data[0] =	0x00 },
    { .address = 0x0221, .data[0] =	0x11 },
    { .address = 0x0381, .data[0] =	0x01 },
    { .address = 0x0383, .data[0] =	0x01 },
    { .address = 0x0385, .data[0] =	0x01 },
    { .address = 0x0387, .data[0] =	0x01 },
    { .address = 0x0900, .data[0] =	0x00 },
    { .address = 0x0901, .data[0] =	0x11 },
    { .address = 0x0902, .data[0] =	0x00 },
    { .address = 0x3140, .data[0] =	0x02 },
    { .address = 0x3241, .data[0] =	0x11 },
    { .address = 0x3250, .data[0] =	0x03 },
    { .address = 0x3E10, .data[0] =	0x00 },
    { .address = 0x3E11, .data[0] =	0x00 },
    { .address = 0x3F0D, .data[0] =	0x00 },
    { .address = 0x3F42, .data[0] =	0x00 },
    { .address = 0x3F43, .data[0] =	0x00 },

    { .address = 0x0401, .data[0] =	0x00 },
    { .address = 0x0404, .data[0] =	0x00 },
    { .address = 0x0405, .data[0] =	0x10 },
    { .address = 0x0408, .data[0] =	0x00 },
    { .address = 0x0409, .data[0] =	0x00 },
    { .address = 0x040A, .data[0] =	0x00 },
    { .address = 0x040B, .data[0] =	0x00 },
    { .address = 0x040C, .data[0] =	0x0F },
    { .address = 0x040D, .data[0] =	0xD8 },
    { .address = 0x040E, .data[0] =	0x08 },
    { .address = 0x040F, .data[0] =	0xF0 },

    { .address = 0x034C, .data[0] =	0x0F },
    { .address = 0x034D, .data[0] =	0xD8 },
    { .address = 0x034E, .data[0] =	0x08 },
    { .address = 0x034F, .data[0] =	0xF0 },

    { .address = 0x0301, .data[0] =	0x05 },
    { .address = 0x0303, .data[0] =	0x02 },
    { .address = 0x0305, .data[0] =	0x04 },
    { .address = 0x0306, .data[0] =	0x01 },
    { .address = 0x0307, .data[0] =	0x5E },
    { .address = 0x0309, .data[0] =	0x0A },
    { .address = 0x030B, .data[0] =	0x01 },
    { .address = 0x030D, .data[0] =	0x02 },
    { .address = 0x030E, .data[0] =	0x01 },
    { .address = 0x030F, .data[0] =	0x5E },
    { .address = 0x0310, .data[0] =	0x00 },
    { .address = 0x0820, .data[0] =	0x20 },
    { .address = 0x0821, .data[0] =	0xD0 },
    { .address = 0x0822, .data[0] =	0x00 },
    { .address = 0x0823, .data[0] =	0x00 },

    { .address = 0x3E20, .data[0] =	0x01 },
    { .address = 0x3E37, .data[0] =	0x00 },

    { .address = 0x3F50, .data[0] =	0x00 },
    { .address = 0x3F56, .data[0] =	0x00 },
    { .address = 0x3F57, .data[0] =	0x82 },

    { .address = 0x3C0A, .data[0] =	0x5A },
    { .address = 0x3C0B, .data[0] =	0x55 },
    { .address = 0x3C0C, .data[0] =	0x28 },
    { .address = 0x3C0D, .data[0] =	0x07 },
    { .address = 0x3C0E, .data[0] =	0xFF },
    { .address = 0x3C0F, .data[0] =	0x00 },
    { .address = 0x3C10, .data[0] =	0x00 },
    { .address = 0x3C11, .data[0] =	0x02 },
    { .address = 0x3C12, .data[0] =	0x00 },
    { .address = 0x3C13, .data[0] =	0x03 },
    { .address = 0x3C14, .data[0] =	0x00 },
    { .address = 0x3C15, .data[0] =	0x00 },
    { .address = 0x3C16, .data[0] =	0x0C },
    { .address = 0x3C17, .data[0] =	0x0C },
    { .address = 0x3C18, .data[0] =	0x0C },
    { .address = 0x3C19, .data[0] =	0x0A },
    { .address = 0x3C1A, .data[0] =	0x0A },
    { .address = 0x3C1B, .data[0] =	0x0A },
    { .address = 0x3C1C, .data[0] =	0x00 },
    { .address = 0x3C1D, .data[0] =	0x00 },
    { .address = 0x3C1E, .data[0] =	0x00 },
    { .address = 0x3C1F, .data[0] =	0x00 },
    { .address = 0x3C20, .data[0] =	0x00 },
    { .address = 0x3C21, .data[0] =	0x00 },
    { .address = 0x3C22, .data[0] =	0x3F },
    { .address = 0x3C23, .data[0] =	0x0A },
    { .address = 0x3E35, .data[0] =	0x01 },
    { .address = 0x3F4A, .data[0] =	0x03 },
    { .address = 0x3F4B, .data[0] =	0xBF },
    { .address = 0x3F26, .data[0] =	0x00 },

    { .address = 0x0202, .data[0] =	0x0C },
    { .address = 0x0203, .data[0] =	0x08 },

    { .address = 0x0204, .data[0] =	0x00 },
    { .address = 0x0205, .data[0] =	0x00 },
    { .address = 0x020E, .data[0] =	0x01 },
    { .address = 0x020F, .data[0] =	0x00 },
    { .address = 0x0210, .data[0] =	0x01 },
    { .address = 0x0211, .data[0] =	0x00 },
    { .address = 0x0212, .data[0] =	0x01 },
    { .address = 0x0213, .data[0] =	0x00 },
    { .address = 0x0214, .data[0] =	0x01 },
    { .address = 0x0215, .data[0] =	0x00 },


	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
};

const T_CAMERA_SENSOR_REG		IMX577_MODE_Full_4056x3040_RAW10_60fps[] =
{
    { .address = 0x0112, .data[0] = 0x0A   },
    { .address = 0x0113, .data[0] = 0x0A   },
    { .address = 0x0114, .data[0] = 0x03   },

    { .address = 0x0342, .data[0] = 0x11   },
    { .address = 0x0343, .data[0] = 0xA0   },

    { .address = 0x0340, .data[0] = 0x0C   },
    { .address = 0x0341, .data[0] = 0x1E   },
    { .address = 0x3210, .data[0] = 0x00   },

    { .address = 0x0344, .data[0] = 0x00   },
    { .address = 0x0345, .data[0] = 0x00   },
    { .address = 0x0346, .data[0] = 0x00   },
    { .address = 0x0347, .data[0] = 0x00   },
    { .address = 0x0348, .data[0] = 0x0F   },
    { .address = 0x0349, .data[0] = 0xD7   },
    { .address = 0x034A, .data[0] = 0x0B   },
    { .address = 0x034B, .data[0] = 0xDF   },

    { .address = 0x00E3, .data[0] = 0x00   },
    { .address = 0x00E4, .data[0] = 0x00   },
    { .address = 0x00E5, .data[0] = 0x01   },
    { .address = 0x00FC, .data[0] = 0x0A   },
    { .address = 0x00FD, .data[0] = 0x0A   },
    { .address = 0x00FE, .data[0] = 0x0A   },
    { .address = 0x00FF, .data[0] = 0x0A   },
    { .address = 0xE013, .data[0] = 0x00   },
    { .address = 0x0220, .data[0] = 0x00   },
    { .address = 0x0221, .data[0] = 0x11   },
    { .address = 0x0381, .data[0] = 0x01   },
    { .address = 0x0383, .data[0] = 0x01   },
    { .address = 0x0385, .data[0] = 0x01   },
    { .address = 0x0387, .data[0] = 0x01   },
    { .address = 0x0900, .data[0] = 0x00   },
    { .address = 0x0901, .data[0] = 0x11   },
    { .address = 0x0902, .data[0] = 0x00   },
    { .address = 0x3140, .data[0] = 0x02   },
    { .address = 0x3241, .data[0] = 0x11   },
    { .address = 0x3250, .data[0] = 0x03   },
    { .address = 0x3E10, .data[0] = 0x00   },
    { .address = 0x3E11, .data[0] = 0x00   },
    { .address = 0x3F0D, .data[0] = 0x00   },
    { .address = 0x3F42, .data[0] = 0x00   },
    { .address = 0x3F43, .data[0] = 0x00   },

    { .address = 0x0401, .data[0] = 0x00   },
    { .address = 0x0404, .data[0] = 0x00   },
    { .address = 0x0405, .data[0] = 0x10   },
    { .address = 0x0408, .data[0] = 0x00   },
    { .address = 0x0409, .data[0] = 0x00   },
    { .address = 0x040A, .data[0] = 0x00   },
    { .address = 0x040B, .data[0] = 0x00   },
    { .address = 0x040C, .data[0] = 0x0F   },
    { .address = 0x040D, .data[0] = 0xD8   },
    { .address = 0x040E, .data[0] = 0x0B   },
    { .address = 0x040F, .data[0] = 0xE0   },

    { .address = 0x034C, .data[0] = 0x0F   },
    { .address = 0x034D, .data[0] = 0xD8   },
    { .address = 0x034E, .data[0] = 0x0B   },
    { .address = 0x034F, .data[0] = 0xE0   },

    { .address = 0x0301, .data[0] = 0x05   },
    { .address = 0x0303, .data[0] = 0x02   },
    { .address = 0x0305, .data[0] = 0x04   },
    { .address = 0x0306, .data[0] = 0x01   },
    { .address = 0x0307, .data[0] = 0x5E   },
    { .address = 0x0309, .data[0] = 0x0A   },
    { .address = 0x030B, .data[0] = 0x01   },
    { .address = 0x030D, .data[0] = 0x02   },
    { .address = 0x030E, .data[0] = 0x01   },
    { .address = 0x030F, .data[0] = 0x5E   },
    { .address = 0x0310, .data[0] = 0x00   },
    { .address = 0x0820, .data[0] = 0x20   },
    { .address = 0x0821, .data[0] = 0xD0   },
    { .address = 0x0822, .data[0] = 0x00   },
    { .address = 0x0823, .data[0] = 0x00   },

    { .address = 0x3E20, .data[0] = 0x01   },
    { .address = 0x3E37, .data[0] = 0x00   },

    { .address = 0x3F50, .data[0] = 0x00   },
    { .address = 0x3F56, .data[0] = 0x00   },
    { .address = 0x3F57, .data[0] = 0x82   },

    { .address = 0x3C0A, .data[0] = 0x5A   },
    { .address = 0x3C0B, .data[0] = 0x55   },
    { .address = 0x3C0C, .data[0] = 0x28   },
    { .address = 0x3C0D, .data[0] = 0x07   },
    { .address = 0x3C0E, .data[0] = 0xFF   },
    { .address = 0x3C0F, .data[0] = 0x00   },
    { .address = 0x3C10, .data[0] = 0x00   },
    { .address = 0x3C11, .data[0] = 0x02   },
    { .address = 0x3C12, .data[0] = 0x00   },
    { .address = 0x3C13, .data[0] = 0x03   },
    { .address = 0x3C14, .data[0] = 0x00   },
    { .address = 0x3C15, .data[0] = 0x00   },
    { .address = 0x3C16, .data[0] = 0x0C   },
    { .address = 0x3C17, .data[0] = 0x0C   },
    { .address = 0x3C18, .data[0] = 0x0C   },
    { .address = 0x3C19, .data[0] = 0x0A   },
    { .address = 0x3C1A, .data[0] = 0x0A   },
    { .address = 0x3C1B, .data[0] = 0x0A   },
    { .address = 0x3C1C, .data[0] = 0x00   },
    { .address = 0x3C1D, .data[0] = 0x00   },
    { .address = 0x3C1E, .data[0] = 0x00   },
    { .address = 0x3C1F, .data[0] = 0x00   },
    { .address = 0x3C20, .data[0] = 0x00   },
    { .address = 0x3C21, .data[0] = 0x00   },
    { .address = 0x3C22, .data[0] = 0x3F   },
    { .address = 0x3C23, .data[0] = 0x0A   },
    { .address = 0x3E35, .data[0] = 0x01   },
    { .address = 0x3F4A, .data[0] = 0x03   },
    { .address = 0x3F4B, .data[0] = 0xBF   },
    { .address = 0x3F26, .data[0] = 0x00   },

    { .address = 0x0202, .data[0] = 0x0C   },
    { .address = 0x0203, .data[0] = 0x08   },

    { .address = 0x0204, .data[0] = 0x00   },
    { .address = 0x0205, .data[0] = 0x00   },
    { .address = 0x020E, .data[0] = 0x01   },
    { .address = 0x020F, .data[0] = 0x00   },
    { .address = 0x0210, .data[0] = 0x01   },
    { .address = 0x0211, .data[0] = 0x00   },
    { .address = 0x0212, .data[0] = 0x01   },
    { .address = 0x0213, .data[0] = 0x00   },
    { .address = 0x0214, .data[0] = 0x01   },
    { .address = 0x0215, .data[0] = 0x00   },


	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
};

const T_CAMERA_SENSOR_REG IMX577_MODE_Full_4056x3040_RAW12_40fps[] =
{
    { .address = 0x0112, .data[0] =	0x0C  },
    { .address = 0x0113, .data[0] =	0x0C  },
    { .address = 0x0114, .data[0] =	0x03  },

    { .address = 0x0342, .data[0] =	0x18  },
    { .address = 0x0343, .data[0] =	0x50  },

    { .address = 0x0340, .data[0] =	0x0D  },
    { .address = 0x0341, .data[0] =	0x2E  },
    { .address = 0x3210, .data[0] =	0x00  },

    { .address = 0x0344, .data[0] =	0x00  },
    { .address = 0x0345, .data[0] =	0x00  },
    { .address = 0x0346, .data[0] =	0x00  },
    { .address = 0x0347, .data[0] =	0x00  },
    { .address = 0x0348, .data[0] =	0x0F  },
    { .address = 0x0349, .data[0] =	0xD7  },
    { .address = 0x034A, .data[0] =	0x0B  },
    { .address = 0x034B, .data[0] =	0xDF  },

    { .address = 0x00E3, .data[0] =	0x00  },
    { .address = 0x00E4, .data[0] =	0x00  },
    { .address = 0x00E5, .data[0] =	0x01  },
    { .address = 0x00FC, .data[0] =	0x0A  },
    { .address = 0x00FD, .data[0] =	0x0A  },
    { .address = 0x00FE, .data[0] =	0x0A  },
    { .address = 0x00FF, .data[0] =	0x0A  },
    { .address = 0xE013, .data[0] =	0x00  },
    { .address = 0x0220, .data[0] =	0x00  },
    { .address = 0x0221, .data[0] =	0x11  },
    { .address = 0x0381, .data[0] =	0x01  },
    { .address = 0x0383, .data[0] =	0x01  },
    { .address = 0x0385, .data[0] =	0x01  },
    { .address = 0x0387, .data[0] =	0x01  },
    { .address = 0x0900, .data[0] =	0x00  },
    { .address = 0x0901, .data[0] =	0x11  },
    { .address = 0x0902, .data[0] =	0x00  },
    { .address = 0x3140, .data[0] =	0x02  },
    { .address = 0x3241, .data[0] =	0x11  },
    { .address = 0x3250, .data[0] =	0x03  },
    { .address = 0x3E10, .data[0] =	0x00  },
    { .address = 0x3E11, .data[0] =	0x00  },
    { .address = 0x3F0D, .data[0] =	0x01  },
    { .address = 0x3F42, .data[0] =	0x00  },
    { .address = 0x3F43, .data[0] =	0x00  },

    { .address = 0x0401, .data[0] =	0x00  },
    { .address = 0x0404, .data[0] =	0x00  },
    { .address = 0x0405, .data[0] =	0x10  },
    { .address = 0x0408, .data[0] =	0x00  },
    { .address = 0x0409, .data[0] =	0x00  },
    { .address = 0x040A, .data[0] =	0x00  },
    { .address = 0x040B, .data[0] =	0x00  },
    { .address = 0x040C, .data[0] =	0x0F  },
    { .address = 0x040D, .data[0] =	0xD8  },
    { .address = 0x040E, .data[0] =	0x0B  },
    { .address = 0x040F, .data[0] =	0xE0  },

    { .address = 0x034C, .data[0] =	0x0F  },
    { .address = 0x034D, .data[0] =	0xD8  },
    { .address = 0x034E, .data[0] =	0x0B  },
    { .address = 0x034F, .data[0] =	0xE0  },

    { .address = 0x0301, .data[0] =	0x05  },
    { .address = 0x0303, .data[0] =	0x02  },
    { .address = 0x0305, .data[0] =	0x04  },
    { .address = 0x0306, .data[0] =	0x01  },
    { .address = 0x0307, .data[0] =	0x5E  },
    { .address = 0x0309, .data[0] =	0x0C  },
    { .address = 0x030B, .data[0] =	0x01  },
    { .address = 0x030D, .data[0] =	0x02  },
    { .address = 0x030E, .data[0] =	0x01  },
    { .address = 0x030F, .data[0] =	0x5E  },
    { .address = 0x0310, .data[0] =	0x00  },
    { .address = 0x0820, .data[0] =	0x20  },
    { .address = 0x0821, .data[0] =	0xD0  },
    { .address = 0x0822, .data[0] =	0x00  },
    { .address = 0x0823, .data[0] =	0x00  },

    { .address = 0x3E20, .data[0] =	0x01  },
    { .address = 0x3E37, .data[0] =	0x00  },

    { .address = 0x3F50, .data[0] =	0x00  },
    { .address = 0x3F56, .data[0] =	0x00  },
    { .address = 0x3F57, .data[0] =	0xB2  },

    { .address = 0x3C0A, .data[0] =	0x5A  },
    { .address = 0x3C0B, .data[0] =	0x55  },
    { .address = 0x3C0C, .data[0] =	0x28  },
    { .address = 0x3C0D, .data[0] =	0x07  },
    { .address = 0x3C0E, .data[0] =	0xFF  },
    { .address = 0x3C0F, .data[0] =	0x00  },
    { .address = 0x3C10, .data[0] =	0x00  },
    { .address = 0x3C11, .data[0] =	0x02  },
    { .address = 0x3C12, .data[0] =	0x00  },
    { .address = 0x3C13, .data[0] =	0x03  },
    { .address = 0x3C14, .data[0] =	0x00  },
    { .address = 0x3C15, .data[0] =	0x00  },
    { .address = 0x3C16, .data[0] =	0x0C  },
    { .address = 0x3C17, .data[0] =	0x0C  },
    { .address = 0x3C18, .data[0] =	0x0C  },
    { .address = 0x3C19, .data[0] =	0x0A  },
    { .address = 0x3C1A, .data[0] =	0x0A  },
    { .address = 0x3C1B, .data[0] =	0x0A  },
    { .address = 0x3C1C, .data[0] =	0x00  },
    { .address = 0x3C1D, .data[0] =	0x00  },
    { .address = 0x3C1E, .data[0] =	0x00  },
    { .address = 0x3C1F, .data[0] =	0x00  },
    { .address = 0x3C20, .data[0] =	0x00  },
    { .address = 0x3C21, .data[0] =	0x00  },
    { .address = 0x3C22, .data[0] =	0x3F  },
    { .address = 0x3C23, .data[0] =	0x0A  },
    { .address = 0x3E35, .data[0] =	0x01  },
    { .address = 0x3F4A, .data[0] =	0x03  },
    { .address = 0x3F4B, .data[0] =	0x85  },
    { .address = 0x3F26, .data[0] =	0x00  },

    { .address = 0x0202, .data[0] =	0x0D  },
    { .address = 0x0203, .data[0] =	0x18  },

    { .address = 0x0204, .data[0] =	0x00  },
    { .address = 0x0205, .data[0] =	0x00  },
    { .address = 0x020E, .data[0] =	0x01  },
    { .address = 0x020F, .data[0] =	0x00  },
    { .address = 0x0210, .data[0] =	0x01  },
    { .address = 0x0211, .data[0] =	0x00  },
    { .address = 0x0212, .data[0] =	0x01  },
    { .address = 0x0213, .data[0] =	0x00  },
    { .address = 0x0214, .data[0] =	0x01  },
    { .address = 0x0215, .data[0] =	0x00  },

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
};

const T_CAMERA_SENSOR_REG IMX577_DOL_2frame_4056x2288_RAW10_30fps[] =
{
    { .address = 0x0112, .data[0] = 0x0A   },
    { .address = 0x0113, .data[0] = 0x0A   },
    { .address = 0x0114, .data[0] = 0x03   },
         
    { .address = 0x0342, .data[0] = 0x11   },
    { .address = 0x0343, .data[0] = 0xA0   },
         
    { .address = 0x0340, .data[0] = 0x0C   },
    { .address = 0x0341, .data[0] = 0x1E   },
    { .address = 0x3210, .data[0] = 0x00   },
         
    { .address = 0x0344, .data[0] = 0x00   },
    { .address = 0x0345, .data[0] = 0x00   },
    { .address = 0x0346, .data[0] = 0x01   },
    { .address = 0x0347, .data[0] = 0x78   },
    { .address = 0x0348, .data[0] = 0x0F   },
    { .address = 0x0349, .data[0] = 0xD7   },
    { .address = 0x034A, .data[0] = 0x0A   },
    { .address = 0x034B, .data[0] = 0x67   },
         
    { .address = 0x00E3, .data[0] = 0x01   },
    { .address = 0x00E4, .data[0] = 0x01   },
    { .address = 0x00E5, .data[0] = 0x01   },
    { .address = 0x00FC, .data[0] = 0x0A   },
    { .address = 0x00FD, .data[0] = 0x0A   },
    { .address = 0x00FE, .data[0] = 0x0A   },
    { .address = 0x00FF, .data[0] = 0x0A   },
    { .address = 0xE013, .data[0] = 0x00   },
    { .address = 0x0220, .data[0] = 0x00   },
    { .address = 0x0221, .data[0] = 0x11   },
    { .address = 0x0381, .data[0] = 0x01   },
    { .address = 0x0383, .data[0] = 0x01   },
    { .address = 0x0385, .data[0] = 0x01   },
    { .address = 0x0387, .data[0] = 0x01   },
    { .address = 0x0900, .data[0] = 0x00   },
    { .address = 0x0901, .data[0] = 0x11   },
    { .address = 0x0902, .data[0] = 0x00   },
    { .address = 0x3140, .data[0] = 0x02   },
    { .address = 0x3241, .data[0] = 0x11   },
    { .address = 0x3250, .data[0] = 0x03   },
    { .address = 0x3E10, .data[0] = 0x00   },
    { .address = 0x3E11, .data[0] = 0x00   },
    { .address = 0x3F0D, .data[0] = 0x00   },
    { .address = 0x3F42, .data[0] = 0x00   },
    { .address = 0x3F43, .data[0] = 0x00   },
         
    { .address = 0x0401, .data[0] = 0x00   },
    { .address = 0x0404, .data[0] = 0x00   },
    { .address = 0x0405, .data[0] = 0x10   },
    { .address = 0x0408, .data[0] = 0x00   },
    { .address = 0x0409, .data[0] = 0x00   },
    { .address = 0x040A, .data[0] = 0x00   },
    { .address = 0x040B, .data[0] = 0x00   },
    { .address = 0x040C, .data[0] = 0x0F   },
    { .address = 0x040D, .data[0] = 0xD8   },
    { .address = 0x040E, .data[0] = 0x08   },
    { .address = 0x040F, .data[0] = 0xF0   },
         
    { .address = 0x034C, .data[0] = 0x0F   },
    { .address = 0x034D, .data[0] = 0xDC   },
    { .address = 0x034E, .data[0] = 0x08   },
    { .address = 0x034F, .data[0] = 0xF0   },
         
    { .address = 0x0301, .data[0] = 0x05   },
    { .address = 0x0303, .data[0] = 0x02   },
    { .address = 0x0305, .data[0] = 0x04   },
    { .address = 0x0306, .data[0] = 0x01   },
    { .address = 0x0307, .data[0] = 0x5E   },
    { .address = 0x0309, .data[0] = 0x0A   },
    { .address = 0x030B, .data[0] = 0x01   },
    { .address = 0x030D, .data[0] = 0x02   },
    { .address = 0x030E, .data[0] = 0x01   },
    { .address = 0x030F, .data[0] = 0x5E   },
    { .address = 0x0310, .data[0] = 0x00   },
    { .address = 0x0820, .data[0] = 0x20   },
    { .address = 0x0821, .data[0] = 0xD0   },
    { .address = 0x0822, .data[0] = 0x00   },
    { .address = 0x0823, .data[0] = 0x00   },
         
    { .address = 0x3E20, .data[0] = 0x01   },
    { .address = 0x3E37, .data[0] = 0x00   },
         
    { .address = 0x3F50, .data[0] = 0x00   },
    { .address = 0x3F56, .data[0] = 0x00   },
    { .address = 0x3F57, .data[0] = 0x82   },
         
    { .address = 0x3C0A, .data[0] = 0x5A   },
    { .address = 0x3C0B, .data[0] = 0x55   },
    { .address = 0x3C0C, .data[0] = 0x28   },
    { .address = 0x3C0D, .data[0] = 0x07   },
    { .address = 0x3C0E, .data[0] = 0xFF   },
    { .address = 0x3C0F, .data[0] = 0x00   },
    { .address = 0x3C10, .data[0] = 0x00   },
    { .address = 0x3C11, .data[0] = 0x02   },
    { .address = 0x3C12, .data[0] = 0x00   },
    { .address = 0x3C13, .data[0] = 0x03   },
    { .address = 0x3C14, .data[0] = 0x00   },
    { .address = 0x3C15, .data[0] = 0x00   },
    { .address = 0x3C16, .data[0] = 0x0C   },
    { .address = 0x3C17, .data[0] = 0x0C   },
    { .address = 0x3C18, .data[0] = 0x0C   },
    { .address = 0x3C19, .data[0] = 0x0A   },
    { .address = 0x3C1A, .data[0] = 0x0A   },
    { .address = 0x3C1B, .data[0] = 0x0A   },
    { .address = 0x3C1C, .data[0] = 0x00   },
    { .address = 0x3C1D, .data[0] = 0x00   },
    { .address = 0x3C1E, .data[0] = 0x00   },
    { .address = 0x3C1F, .data[0] = 0x00   },
    { .address = 0x3C20, .data[0] = 0x00   },
    { .address = 0x3C21, .data[0] = 0x00   },
    { .address = 0x3C22, .data[0] = 0x3F   },
    { .address = 0x3C23, .data[0] = 0x0A   },
    { .address = 0x3E35, .data[0] = 0x01   },
    { .address = 0x3F4A, .data[0] = 0x03   },
    { .address = 0x3F4B, .data[0] = 0xBF   },
    { .address = 0x3F26, .data[0] = 0x00   },
         
    { .address = 0x0202, .data[0] = 0x0C   },
    { .address = 0x0203, .data[0] = 0x08   },
         
    { .address = 0x0204, .data[0] = 0x00   },
    { .address = 0x0205, .data[0] = 0x00   },
    { .address = 0x020E, .data[0] = 0x01   },
    { .address = 0x020F, .data[0] = 0x00   },
    { .address = 0x0210, .data[0] = 0x01   },
    { .address = 0x0211, .data[0] = 0x00   },
    { .address = 0x0212, .data[0] = 0x01   },
    { .address = 0x0213, .data[0] = 0x00   },
    { .address = 0x0214, .data[0] = 0x01   },
    { .address = 0x0215, .data[0] = 0x00   },

	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
};

const T_CAMERA_SENSOR_REG IMX577_DOL_2frame_4056x3040_RAW10_15fps[] =
{
    { .address = 0x0112, .data[0] =	0x0A  },
    { .address = 0x0113, .data[0] =	0x0A  },
    { .address = 0x0114, .data[0] =	0x03  },

    { .address = 0x0342, .data[0] =	0x11  },
    { .address = 0x0343, .data[0] =	0xA0  },

    { .address = 0x0340, .data[0] =	0x18  },
    { .address = 0x0341, .data[0] =	0x3D  },
    { .address = 0x3210, .data[0] =	0x00  },

    { .address = 0x0344, .data[0] =	0x00  },
    { .address = 0x0345, .data[0] =	0x00  },
    { .address = 0x0346, .data[0] =	0x00  },
    { .address = 0x0347, .data[0] =	0x00  },
    { .address = 0x0348, .data[0] =	0x0F  },
    { .address = 0x0349, .data[0] =	0xD7  },
    { .address = 0x034A, .data[0] =	0x0B  },
    { .address = 0x034B, .data[0] =	0xDF  },

    { .address = 0x00E3, .data[0] =	0x01  },
    { .address = 0x00E4, .data[0] =	0x01  },
    { .address = 0x00E5, .data[0] =	0x01  },
    { .address = 0x00FC, .data[0] =	0x0A  },
    { .address = 0x00FD, .data[0] =	0x0A  },
    { .address = 0x00FE, .data[0] =	0x0A  },
    { .address = 0x00FF, .data[0] =	0x0A  },
    { .address = 0xE013, .data[0] =	0x00  },
    { .address = 0x0220, .data[0] =	0x00  },
    { .address = 0x0221, .data[0] =	0x11  },
    { .address = 0x0381, .data[0] =	0x01  },
    { .address = 0x0383, .data[0] =	0x01  },
    { .address = 0x0385, .data[0] =	0x01  },
    { .address = 0x0387, .data[0] =	0x01  },
    { .address = 0x0900, .data[0] =	0x00  },
    { .address = 0x0901, .data[0] =	0x11  },
    { .address = 0x0902, .data[0] =	0x00  },
    { .address = 0x3140, .data[0] =	0x02  },
    { .address = 0x3241, .data[0] =	0x11  },
    { .address = 0x3250, .data[0] =	0x03  },
    { .address = 0x3E10, .data[0] =	0x00  },
    { .address = 0x3E11, .data[0] =	0x00  },
    { .address = 0x3F0D, .data[0] =	0x00  },
    { .address = 0x3F42, .data[0] =	0x00  },
    { .address = 0x3F43, .data[0] =	0x00  },

    { .address = 0x0401, .data[0] =	0x00  },
    { .address = 0x0404, .data[0] =	0x00  },
    { .address = 0x0405, .data[0] =	0x10  },
    { .address = 0x0408, .data[0] =	0x00  },
    { .address = 0x0409, .data[0] =	0x00  },
    { .address = 0x040A, .data[0] =	0x00  },
    { .address = 0x040B, .data[0] =	0x00  },
    { .address = 0x040C, .data[0] =	0x0F  },
    { .address = 0x040D, .data[0] =	0xD8  },
    { .address = 0x040E, .data[0] =	0x0B  },
    { .address = 0x040F, .data[0] =	0xE0  },

    { .address = 0x034C, .data[0] =	0x0F  },
    { .address = 0x034D, .data[0] =	0xDC  },
    { .address = 0x034E, .data[0] =	0x0B  },
    { .address = 0x034F, .data[0] =	0xE0  },

    { .address = 0x0301, .data[0] =	0x05  },
    { .address = 0x0303, .data[0] =	0x02  },
    { .address = 0x0305, .data[0] =	0x04  },
    { .address = 0x0306, .data[0] =	0x01  },
    { .address = 0x0307, .data[0] =	0x5E  },
    { .address = 0x0309, .data[0] =	0x0A  },
    { .address = 0x030B, .data[0] =	0x01  },
    { .address = 0x030D, .data[0] =	0x02  },
    { .address = 0x030E, .data[0] =	0x01  },
    { .address = 0x030F, .data[0] =	0x5E  },
    { .address = 0x0310, .data[0] =	0x00  },
    { .address = 0x0820, .data[0] =	0x20  },
    { .address = 0x0821, .data[0] =	0xD0  },
    { .address = 0x0822, .data[0] =	0x00  },
    { .address = 0x0823, .data[0] =	0x00  },

    { .address = 0x3E20, .data[0] =	0x01  },
    { .address = 0x3E37, .data[0] =	0x00  },

    { .address = 0x3F50, .data[0] =	0x00  },
    { .address = 0x3F56, .data[0] =	0x00  },
    { .address = 0x3F57, .data[0] =	0x82  },

    { .address = 0x3C0A, .data[0] =	0x5A  },
    { .address = 0x3C0B, .data[0] =	0x55  },
    { .address = 0x3C0C, .data[0] =	0x28  },
    { .address = 0x3C0D, .data[0] =	0x07  },
    { .address = 0x3C0E, .data[0] =	0xFF  },
    { .address = 0x3C0F, .data[0] =	0x00  },
    { .address = 0x3C10, .data[0] =	0x00  },
    { .address = 0x3C11, .data[0] =	0x02  },
    { .address = 0x3C12, .data[0] =	0x00  },
    { .address = 0x3C13, .data[0] =	0x03  },
    { .address = 0x3C14, .data[0] =	0x00  },
    { .address = 0x3C15, .data[0] =	0x00  },
    { .address = 0x3C16, .data[0] =	0x0C  },
    { .address = 0x3C17, .data[0] =	0x0C  },
    { .address = 0x3C18, .data[0] =	0x0C  },
    { .address = 0x3C19, .data[0] =	0x0A  },
    { .address = 0x3C1A, .data[0] =	0x0A  },
    { .address = 0x3C1B, .data[0] =	0x0A  },
    { .address = 0x3C1C, .data[0] =	0x00  },
    { .address = 0x3C1D, .data[0] =	0x00  },
    { .address = 0x3C1E, .data[0] =	0x00  },
    { .address = 0x3C1F, .data[0] =	0x00  },
    { .address = 0x3C20, .data[0] =	0x00  },
    { .address = 0x3C21, .data[0] =	0x00  },
    { .address = 0x3C22, .data[0] =	0x3F  },
    { .address = 0x3C23, .data[0] =	0x0A  },
    { .address = 0x3E35, .data[0] =	0x01  },
    { .address = 0x3F4A, .data[0] =	0x03  },
    { .address = 0x3F4B, .data[0] =	0xBF  },
    { .address = 0x3F26, .data[0] =	0x00  },

    { .address = 0x0202, .data[0] =	0x18  },
    { .address = 0x0203, .data[0] =	0x27  },

    { .address = 0x0204, .data[0] =	0x00  },
    { .address = 0x0205, .data[0] =	0x00  },
    { .address = 0x020E, .data[0] =	0x01  },
    { .address = 0x020F, .data[0] =	0x00  },
    { .address = 0x0210, .data[0] =	0x01  },
    { .address = 0x0211, .data[0] =	0x00  },
    { .address = 0x0212, .data[0] =	0x01  },
    { .address = 0x0213, .data[0] =	0x00  },
    { .address = 0x0214, .data[0] =	0x01  },
    { .address = 0x0215, .data[0] =	0x00  },


	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
};

// Not support in SDK
const T_CAMERA_SENSOR_REG		IMX577_MODE_Full_4056x2288_RAW10_PD_60fps[] =
{
	{ .address = 0x0112, .data[0] = 0x0A },
	{ .address = 0x0113, .data[0] = 0x0A },
	{ .address = 0x0114, .data[0] = 0x03 },
	
	{ .address = 0x0342, .data[0] = 0x11 },
	{ .address = 0x0343, .data[0] = 0xA0 },
	
#if 1 // 60fps
	{ .address = 0x0340, .data[0] = 0x0C },
	{ .address = 0x0341, .data[0] = 0x14 },
#else // 30fps
	{ .address = 0x0340, .data[0] = 0x18 },
	{ .address = 0x0341, .data[0] = 0x28 },
#endif
	
	{ .address = 0x0344, .data[0] = 0x00 },
	{ .address = 0x0345, .data[0] = 0x00 },
	{ .address = 0x0346, .data[0] = 0x01 },
	{ .address = 0x0347, .data[0] = 0x78 },
	{ .address = 0x0348, .data[0] = 0x0F },
	{ .address = 0x0349, .data[0] = 0xD7 },
	{ .address = 0x034A, .data[0] = 0x0A },
	{ .address = 0x034B, .data[0] = 0x67 },
	
	{ .address = 0x00E3, .data[0] = 0x00 },
	{ .address = 0x00E4, .data[0] = 0x00 },
	{ .address = 0x00FC, .data[0] = 0x0A },
	{ .address = 0x00FD, .data[0] = 0x0A },
	{ .address = 0x00FE, .data[0] = 0x0A },
	{ .address = 0x00FF, .data[0] = 0x0A },
	{ .address = 0x0220, .data[0] = 0x00 },
	{ .address = 0x0221, .data[0] = 0x11 },
	{ .address = 0x0381, .data[0] = 0x01 },
	{ .address = 0x0383, .data[0] = 0x01 },
	{ .address = 0x0385, .data[0] = 0x01 },
	{ .address = 0x0387, .data[0] = 0x01 },
	{ .address = 0x0900, .data[0] = 0x00 },
	{ .address = 0x0901, .data[0] = 0x11 },
	{ .address = 0x0902, .data[0] = 0x02 },
	{ .address = 0x3140, .data[0] = 0x02 },
	{ .address = 0x3C00, .data[0] = 0x00 },
	{ .address = 0x3C01, .data[0] = 0x03 },
	{ .address = 0x3C02, .data[0] = 0xDC },
	{ .address = 0x3F0D, .data[0] = 0x00 },
	{ .address = 0x5748, .data[0] = 0x07 },
	{ .address = 0x5749, .data[0] = 0xFF },
	{ .address = 0x574A, .data[0] = 0x00 },
	{ .address = 0x574B, .data[0] = 0x00 },
	{ .address = 0x7B75, .data[0] = 0x0E },
	{ .address = 0x7B76, .data[0] = 0x09 },
	{ .address = 0x7B77, .data[0] = 0x0C },
	{ .address = 0x7B78, .data[0] = 0x06 },
	{ .address = 0x7B79, .data[0] = 0x3B },
	{ .address = 0x7B53, .data[0] = 0x01 },
	{ .address = 0x9369, .data[0] = 0x5A },
	{ .address = 0x936B, .data[0] = 0x55 },
	{ .address = 0x936D, .data[0] = 0x28 },
	{ .address = 0x9304, .data[0] = 0x03 },
	{ .address = 0x9305, .data[0] = 0x00 },
	{ .address = 0x9E9A, .data[0] = 0x2F },
	{ .address = 0x9E9B, .data[0] = 0x2F },
	{ .address = 0x9E9C, .data[0] = 0x2F },
	{ .address = 0x9E9D, .data[0] = 0x00 },
	{ .address = 0x9E9E, .data[0] = 0x00 },
	{ .address = 0x9E9F, .data[0] = 0x00 },
	{ .address = 0xA2A9, .data[0] = 0x60 },
	{ .address = 0xA2B7, .data[0] = 0x00 },
	
	{ .address = 0x0401, .data[0] = 0x00 },
	{ .address = 0x0404, .data[0] = 0x00 },
	{ .address = 0x0405, .data[0] = 0x10 },
	{ .address = 0x0408, .data[0] = 0x00 },
	{ .address = 0x0409, .data[0] = 0x00 },
	{ .address = 0x040A, .data[0] = 0x00 },
	{ .address = 0x040B, .data[0] = 0x00 },
	{ .address = 0x040C, .data[0] = 0x0F },
	{ .address = 0x040D, .data[0] = 0xD8 },
	{ .address = 0x040E, .data[0] = 0x08 },
	{ .address = 0x040F, .data[0] = 0xF0 },
	
	{ .address = 0x034C, .data[0] = 0x0F },
	{ .address = 0x034D, .data[0] = 0xD8 },
	{ .address = 0x034E, .data[0] = 0x08 },
	{ .address = 0x034F, .data[0] = 0xF0 },
	
	{ .address = 0x0301, .data[0] = 0x05 },
	{ .address = 0x0303, .data[0] = 0x02 },
	{ .address = 0x0305, .data[0] = 0x02 },
	{ .address = 0x0306, .data[0] = 0x00 },
	{ .address = 0x0307, .data[0] = 0xAF },
	{ .address = 0x0309, .data[0] = 0x0A },
	{ .address = 0x030B, .data[0] = 0x01 },
	{ .address = 0x030D, .data[0] = 0x02 },
	{ .address = 0x030E, .data[0] = 0x01 },
	{ .address = 0x030F, .data[0] = 0x5E },
	{ .address = 0x0310, .data[0] = 0x00 },
	{ .address = 0x0820, .data[0] = 0x20 },
	{ .address = 0x0821, .data[0] = 0xD0 },
	{ .address = 0x0822, .data[0] = 0x00 },
	{ .address = 0x0823, .data[0] = 0x00 },
	
	{ .address = 0x080A, .data[0] = 0x00 },
	{ .address = 0x080B, .data[0] = 0xC7 },
	{ .address = 0x080C, .data[0] = 0x00 },
	{ .address = 0x080D, .data[0] = 0x87 },
	{ .address = 0x080E, .data[0] = 0x00 },
	{ .address = 0x080F, .data[0] = 0xDF },
	{ .address = 0x0810, .data[0] = 0x00 },
	{ .address = 0x0811, .data[0] = 0x97 },
	{ .address = 0x0812, .data[0] = 0x00 },
	{ .address = 0x0813, .data[0] = 0x8F },
	{ .address = 0x0814, .data[0] = 0x00 },
	{ .address = 0x0815, .data[0] = 0x7F },
	{ .address = 0x0816, .data[0] = 0x02 },
	{ .address = 0x0817, .data[0] = 0x27 },
	{ .address = 0x0818, .data[0] = 0x00 },
	{ .address = 0x0819, .data[0] = 0x6F },
	{ .address = 0xE04C, .data[0] = 0x00 },
	{ .address = 0xE04D, .data[0] = 0xDF },
	{ .address = 0xE04E, .data[0] = 0x00 },
	{ .address = 0xE04F, .data[0] = 0x1F },
	
	{ .address = 0x3E20, .data[0] = 0x01 },

	{ .address = 0x38A3, .data[0] = 0x00 }, // Fixed window mode (16x12)
	{ .address = 0x38A4, .data[0] = 0x00 }, // PD_AREA_X
	{ .address = 0x38A5, .data[0] = 0x00 },
	{ .address = 0x38A6, .data[0] = 0x00 }, // PD_AREA_Y
	{ .address = 0x38A7, .data[0] = 0x00 },
	{ .address = 0x38A8, .data[0] = 0x00 }, // PD_AREA_WIDTH : even(4056 / 16) = 254
	{ .address = 0x38A9, .data[0] = 0xFE },
	{ .address = 0x38AA, .data[0] = 0x00 }, // PD_AREA_HEIGHT : even(2288 / 12) = 190
	{ .address = 0x38AB, .data[0] = 0xBE },
	{ .address = 0x3E35, .data[0] = 0x01 }, // Enable SPC
	{ .address = 0x3E36, .data[0] = 0x01 }, // Enable Additional defect correction control
	{ .address = 0x3E37, .data[0] = 0x01 }, // PD data output

	{ .address = 0x3F50, .data[0] = 0x00 },
	{ .address = 0x3F56, .data[0] = 0x00 },
	{ .address = 0x3F57, .data[0] = 0x81 },
	
	// make image brighter
	{ .address = 0x0202, .data[0] = 0x08 },
	{ .address = 0x0203, .data[0] = 0x00 },
	{ .address = 0x0204, .data[0] = 0x03 },
	{ .address = 0x0205, .data[0] = 0x00 },
	
	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
};

const T_CAMERA_SENSOR_REG IMX577_MODE_BINNING_2x2_2028x1128_RAW10_240fps[] =
{
	{ .address = 0x0112, .data[0] = 0x0A },
	{ .address = 0x0113, .data[0] = 0x0A },
	{ .address = 0x0114, .data[0] = 0x03 },

	{ .address = 0x0342, .data[0] = 0x0B },
	{ .address = 0x0343, .data[0] = 0xA0 },

	{ .address = 0x0340, .data[0] = 0x04 },
	{ .address = 0x0341, .data[0] = 0x98 },

	{ .address = 0x0344, .data[0] = 0x00 },
	{ .address = 0x0345, .data[0] = 0x00 },
	{ .address = 0x0346, .data[0] = 0x01 },
	{ .address = 0x0347, .data[0] = 0x88 },
	{ .address = 0x0348, .data[0] = 0x0F },
	{ .address = 0x0349, .data[0] = 0xD7 },
	{ .address = 0x034A, .data[0] = 0x0A },
	{ .address = 0x034B, .data[0] = 0x57 },

	{ .address = 0x00E3, .data[0] = 0x00 },
	{ .address = 0x00E4, .data[0] = 0x00 },
	{ .address = 0x00FC, .data[0] = 0x0A },
	{ .address = 0x00FD, .data[0] = 0x0A },
	{ .address = 0x00FE, .data[0] = 0x0A },
	{ .address = 0x00FF, .data[0] = 0x0A },
	{ .address = 0x0220, .data[0] = 0x00 },
	{ .address = 0x0221, .data[0] = 0x11 },
	{ .address = 0x0381, .data[0] = 0x01 },
	{ .address = 0x0383, .data[0] = 0x01 },
	{ .address = 0x0385, .data[0] = 0x01 },
	{ .address = 0x0387, .data[0] = 0x01 },
	{ .address = 0x0900, .data[0] = 0x01 },
	{ .address = 0x0901, .data[0] = 0x22 },
	{ .address = 0x0902, .data[0] = 0x02 },
	{ .address = 0x3140, .data[0] = 0x02 },
	{ .address = 0x3C00, .data[0] = 0x00 },
	{ .address = 0x3C01, .data[0] = 0x01 },
	{ .address = 0x3C02, .data[0] = 0x9C },
	{ .address = 0x3F0D, .data[0] = 0x00 },
	{ .address = 0x5748, .data[0] = 0x00 },
	{ .address = 0x5749, .data[0] = 0x00 },
	{ .address = 0x574A, .data[0] = 0x00 },
	{ .address = 0x574B, .data[0] = 0xA4 },
	{ .address = 0x7B75, .data[0] = 0x0E },
	{ .address = 0x7B76, .data[0] = 0x09 },
	{ .address = 0x7B77, .data[0] = 0x08 },
	{ .address = 0x7B78, .data[0] = 0x06 },
	{ .address = 0x7B79, .data[0] = 0x34 },
	{ .address = 0x7B53, .data[0] = 0x00 },
	{ .address = 0x9369, .data[0] = 0x73 },
	{ .address = 0x936B, .data[0] = 0x64 },
	{ .address = 0x936D, .data[0] = 0x5F },
	{ .address = 0x9304, .data[0] = 0x03 },
	{ .address = 0x9305, .data[0] = 0x80 },
	{ .address = 0x9E9A, .data[0] = 0x2F },
	{ .address = 0x9E9B, .data[0] = 0x2F },
	{ .address = 0x9E9C, .data[0] = 0x2F },
	{ .address = 0x9E9D, .data[0] = 0x00 },
	{ .address = 0x9E9E, .data[0] = 0x00 },
	{ .address = 0x9E9F, .data[0] = 0x00 },
	{ .address = 0xA2A9, .data[0] = 0x27 },
	{ .address = 0xA2B7, .data[0] = 0x03 },

	{ .address = 0x0401, .data[0] = 0x00 },
	{ .address = 0x0404, .data[0] = 0x00 },
	{ .address = 0x0405, .data[0] = 0x10 },
	{ .address = 0x0408, .data[0] = 0x00 },
	{ .address = 0x0409, .data[0] = 0x00 },
	{ .address = 0x040A, .data[0] = 0x00 },
	{ .address = 0x040B, .data[0] = 0x00 },
	{ .address = 0x040C, .data[0] = 0x07 },
	{ .address = 0x040D, .data[0] = 0xEC },
	{ .address = 0x040E, .data[0] = 0x04 },
	{ .address = 0x040F, .data[0] = 0x68 },

	{ .address = 0x034C, .data[0] = 0x07 },
	{ .address = 0x034D, .data[0] = 0xEC },
	{ .address = 0x034E, .data[0] = 0x04 },
	{ .address = 0x034F, .data[0] = 0x68 },

	{ .address = 0x0301, .data[0] = 0x05 },
	{ .address = 0x0303, .data[0] = 0x02 },
	{ .address = 0x0305, .data[0] = 0x02 },
	{ .address = 0x0306, .data[0] = 0x00 },
	{ .address = 0x0307, .data[0] = 0xAF },
	{ .address = 0x0309, .data[0] = 0x0A },
	{ .address = 0x030B, .data[0] = 0x01 },
	{ .address = 0x030D, .data[0] = 0x02 },
	{ .address = 0x030E, .data[0] = 0x01 },
	{ .address = 0x030F, .data[0] = 0x5E },
	{ .address = 0x0310, .data[0] = 0x00 },
	{ .address = 0x0820, .data[0] = 0x20 },
	{ .address = 0x0821, .data[0] = 0xD0 },
	{ .address = 0x0822, .data[0] = 0x00 },
	{ .address = 0x0823, .data[0] = 0x00 },

	{ .address = 0x080A, .data[0] = 0x00 },
	{ .address = 0x080B, .data[0] = 0xC7 },
	{ .address = 0x080C, .data[0] = 0x00 },
	{ .address = 0x080D, .data[0] = 0x87 },
	{ .address = 0x080E, .data[0] = 0x00 },
	{ .address = 0x080F, .data[0] = 0xDF },
	{ .address = 0x0810, .data[0] = 0x00 },
	{ .address = 0x0811, .data[0] = 0x97 },
	{ .address = 0x0812, .data[0] = 0x00 },
	{ .address = 0x0813, .data[0] = 0x8F },
	{ .address = 0x0814, .data[0] = 0x00 },
	{ .address = 0x0815, .data[0] = 0x7F },
	{ .address = 0x0816, .data[0] = 0x02 },
	{ .address = 0x0817, .data[0] = 0x27 },
	{ .address = 0x0818, .data[0] = 0x00 },
	{ .address = 0x0819, .data[0] = 0x6F },
	{ .address = 0xE04C, .data[0] = 0x00 },
	{ .address = 0xE04D, .data[0] = 0xDF },
	{ .address = 0xE04E, .data[0] = 0x00 },
	{ .address = 0xE04F, .data[0] = 0x1F },

	{ .address = 0x3E20, .data[0] = 0x01 },
	{ .address = 0x3E37, .data[0] = 0x00 },

	{ .address = 0x3F50, .data[0] = 0x00 },
	{ .address = 0x3F56, .data[0] = 0x00 },
	{ .address = 0x3F57, .data[0] = 0x56 },

	// make image brighter
	{ .address = 0x0202, .data[0] = 0x04 },
	{ .address = 0x0203, .data[0] = 0x50 },
	{ .address = 0x0204, .data[0] = 0x03 },
	{ .address = 0x0205, .data[0] = 0xC0 },


	{ .address = D_TBL_END, .data[0] = 0xFF }		// END
};

const T_CAMERA_SENSOR_BASIC		IMX577_BASIC =
{
	.input_clock	= E_CAMERA_SENSOR_INCLK_24MHz,
	.access			= E_CAMERA_ACCESS_I2C,
	.sensor_if		= E_CAMERA_SENSOR_IF_DMIPI,
	.update_timing	= E_CAMERA_SENSOR_UPDATE_TIMING_VD,
};
const T_CAMERA_SENSOR_MODE		IMX577_MODE[E_SENSOR_IMX577_MODE_MAX] =
{
	[E_SENSOR_IMX577_MODE_Full_4056x2288_RAW10_60fps] = {
		.mode_reg			= IMX577_MODE_Full_4056x2288_RAW10_60fps,
		.dol_mode_reg		= IMX577_DOL_2frame_4056x2288_RAW10_30fps,
		.pdaf_mode_reg		= IMX577_MODE_Full_4056x2288_RAW10_PD_60fps,	// Not support in SDK

		.embedded_lines			= 2,
		.total_pix_width		= 4056,
		.total_pix_lines		= 2288 + 2,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 0,
		.active_pix_y_pos		= 2,
		.active_pix_width		= 4056,
		.active_pix_lines		= 2288,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
			
		.pd_data_extend_lines	= 1, // PD(1 line)	// Not support in SDK
		.pd_g_cnt				= 16,				// Not support in SDK
		.pd_win_h_cnt			= 16,				// Not support in SDK
		.pd_win_v_cnt			= 12,				// Not support in SDK
			
		.input_bit_width		= 10,
		.dol_input_bit_width	= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.dol_first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,

		.hdocyc					= 4512,
		.vdocyc					= 3092,
		.dol_hdocyc				= 4512,
		.dol_vdocyc				= 3092,// *2, // *2 is for M10V performance
		.exp_end_line_offset	= 20,
		.exp_fine				= 0.429,
		.base_SV				= 4.0,

		.Mpps_HDclk				= 840.0,
		.one_lane_Mbps			= 2100.0,
		.lane_num				= 4,

		.fps					= 60.21,   // 840000000 / 4512 / 3092
		.dol_fps				= 30.0,// /2, // /2 is for M10V performance
	},
	[E_SENSOR_IMX577_MODE_Full_4056x3040_RAW12_40fps] = {
		.mode_reg			= IMX577_MODE_Full_4056x3040_RAW12_40fps,
		.dol_mode_reg		= IMX577_DOL_2frame_4056x3040_RAW10_15fps,

		.embedded_lines			= 2,
		.total_pix_width		= 4056,
		.total_pix_lines		= 3040 + 2, // +2 embedded lines
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 0,
		.active_pix_y_pos		= 2,
		.active_pix_width		= 4056,
		.active_pix_lines		= 3040,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
		.input_bit_width		= 12,
		.dol_input_bit_width	= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.dol_first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,

		.hdocyc					= 6224,
		.vdocyc					= 3364,
		.dol_hdocyc				= 4512,
		.dol_vdocyc				= 6202,// *2, // *2 is for M10V performance,
		.exp_end_line_offset	= 20,
		.exp_fine				= 0.429,
		.base_SV				= 4.0,

		.Mpps_HDclk				= 840.0,
		.one_lane_Mbps			= 2100.0,
		.lane_num				= 4,

		.fps					= 40.119, // 840000000 / 6224 / 3364
		.dol_fps				= 15.0,// /2,// /2 is for M10V performance
	},
	[E_SENSOR_IMX577_MODE_BINNING_2x2_2028x1128_RAW10_240fps] = {
		.mode_reg			= IMX577_MODE_BINNING_2x2_2028x1128_RAW10_240fps,

		.embedded_lines			= 2,
		.total_pix_width		= 2028,
		.total_pix_lines		= 1128,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 0,
		.active_pix_y_pos		= 2,
		.active_pix_width		= 2028,
		.active_pix_lines		= 1128,
		.active_pix_x_rsz_ratio	= 1.0/2.0,
		.active_pix_y_rsz_ratio	= 1.0/2.0,
		.input_bit_width		= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,

		.hdocyc					= 2976,
		.vdocyc					= 1176,
		.exp_end_line_offset	= 20,
		.exp_fine				= 0.429,
		.base_SV				= 4.0,

		.Mpps_HDclk				= 840.0,
		.one_lane_Mbps			= 2100.0,
		.lane_num				= 4,

		.fps					= 240.01, // 840000000 / 2976 / 1176
	},
    	[E_SENSOR_IMX577_MODE_Full_4056x3040_RAW10_60fps] = {
		.mode_reg			= IMX577_MODE_Full_4056x3040_RAW10_60fps,
		.dol_mode_reg		= IMX577_DOL_2frame_4056x3040_RAW10_15fps,
		.pdaf_mode_reg		= IMX577_MODE_Full_4056x3040_RAW10_60fps,	// Not support in SDK

		.embedded_lines			= 2,
		.total_pix_width		= 4056,
		.total_pix_lines		= 3040 + 2,
		.ob_pix_x_pos			= 0,
		.ob_pix_y_pos			= 0,
		.ob_pix_width			= 0,
		.ob_pix_lines			= 0,
		.active_pix_x_pos		= 0,
		.active_pix_y_pos		= 2,
		.active_pix_width		= 4056,
		.active_pix_lines		= 3040,
		.active_pix_x_rsz_ratio	= 1.0,
		.active_pix_y_rsz_ratio	= 1.0,
			
		.input_bit_width		= 10,
		.dol_input_bit_width	= 10,
		.first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		.dol_first_active_pix_color	= FJ_PRO_CTRL_FIRST_PIX_COLOR_R,
		
		.hdocyc					= 4512,
		.vdocyc					= 3092,
		.dol_hdocyc				= 4512,
		.dol_vdocyc				= 3092,// *2, // *2 is for M10V performance
		.exp_end_line_offset	= 20,
		.exp_fine				= 0.429,
		.base_SV				= 4.0,

		.Mpps_HDclk				= 840.0,
		.one_lane_Mbps			= 2100.0,
		.lane_num				= 4,

		.fps					= 60.21,   // 840000000 / 4512 / 3092
		.dol_fps				= 30.0,// /2, // /2 is for M10V performance
	}
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
E_CAMERA_ERR_CODE Sensor_IMX577_Clock_Gpio_Init(VOID)
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

	gIs_Mode_Transition = FALSE;
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

E_CAMERA_ERR_CODE Sensor_IMX577_Standby(E_CAMERA_ID sensor_id)
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

E_CAMERA_ERR_CODE Sensor_IMX577_Stream(E_CAMERA_ID sensor_id)
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

E_CAMERA_ERR_CODE Sensor_IMX577_Init(E_CAMERA_ID sensor_id)
{
	if(gIs_Mode_Transition == TRUE){
		return E_CAMERA_ERR_OK;
	}else{
		return access_tbl(sensor_id, IMX577_INIT);
	}
}

E_CAMERA_ERR_CODE Sensor_IMX577_CancelStandby(E_CAMERA_ID sensor_id)
{
	if(gIs_Mode_Transition == TRUE){
		return E_CAMERA_ERR_OK;
	}else{
		return access_tbl(sensor_id, IMX577_INIT);
	}
}

E_CAMERA_ERR_CODE Sensor_IMX577_Mode(E_CAMERA_ID sensor_id, UINT32 mode, BOOL dol)
{
	gIs_Mode_Transition = TRUE;
	return dol ? access_tbl(sensor_id, IMX577_MODE[mode].dol_mode_reg) : access_tbl(sensor_id, IMX577_MODE[mode].mode_reg);
}

E_CAMERA_ERR_CODE Sensor_IMX577_Pdaf_Mode(E_CAMERA_ID sensor_id, UINT32 mode)
{
	return access_tbl(sensor_id, IMX577_MODE[mode].pdaf_mode_reg);
}

E_CAMERA_ERR_CODE Sensor_IMX577_Multi(E_CAMERA_ID sensor_id, BOOL is_multi, BOOL is_master)
{
	UCHAR mc_mode;
	UCHAR master_slave_sel;
	UCHAR xvs_io_ctrl;
	UCHAR extout_en;

	if( is_multi ){
		mc_mode = 1;
		if( is_master ){
			master_slave_sel = 1;
			xvs_io_ctrl = 1;
			extout_en = 1;
		}else{
			master_slave_sel = 0;
			xvs_io_ctrl = 0;
			extout_en = 0;
		}
	}else{
		mc_mode = 0;
		master_slave_sel = 1;
		xvs_io_ctrl = 0;
		extout_en = 0;
	}
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3F0B,
							&mc_mode,
							1);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3041,
							&master_slave_sel,
							1);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x3040,
							&xvs_io_ctrl,
							1);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x4B81,
							&extout_en,
							1);

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX577_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
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
	CIT = (UINT32) ( exp_sec * (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / IMX577_MODE[mode].hdocyc - IMX577_MODE[mode].exp_fine);

	if( CIT > ( 65525 - IMX577_MODE[mode].exp_end_line_offset ) ){ // 65525 is max of FLL
		CIT = ( 65525 - IMX577_MODE[mode].exp_end_line_offset );
	}

	if( CIT == 0 ){
		CIT = 1;
	}

	// Frame Length Line
	if( CIT > IMX577_MODE[mode].vdocyc - IMX577_MODE[mode].exp_end_line_offset ){
		FLL = CIT + IMX577_MODE[mode].exp_end_line_offset;
	}
	else{
		FLL = IMX577_MODE[mode].vdocyc;
	}

	// Clip by FPS
	if( in->max_fps == D_CAMERA_FPS_AUTO && in->min_fps == D_CAMERA_FPS_AUTO ){
		// auto, nothing to do
	}else{
		default_fps = (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX577_MODE[mode].hdocyc / (FLOAT)IMX577_MODE[mode].vdocyc;
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
		req_fps = (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX577_MODE[mode].hdocyc / (FLOAT)FLL;
		if( req_fps < min_fps ){
			FLL = (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX577_MODE[mode].hdocyc / min_fps;
		}
		if( req_fps > max_fps ){
			FLL = (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX577_MODE[mode].hdocyc / max_fps;
		}
		// clip CIT
		if( CIT > ( FLL - IMX577_MODE[mode].exp_end_line_offset ) ){
			CIT = ( FLL - IMX577_MODE[mode].exp_end_line_offset );
		}
	}

	// Binning mode x 2, CIT : 2/4/6 are prohibit
	if( mode == E_SENSOR_IMX577_MODE_BINNING_2x2_2028x1128_RAW10_240fps && (CIT == 2 || CIT == 4 || CIT == 6)) {
		CIT += 1;
	}

	// Analog Gain
	again = in->gain_times;
	if( again < 1.0 ){
		again = 1.0;
	}

	AGG = (UINT32)( 1024.0 - (1024.0 / again) );
	if( AGG > 978 ){
		AGG = 978;
	}
	again = ( 1024.0 / (1024.0 - (FLOAT)AGG) );

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
	exp_sec = (FLOAT)( CIT + IMX577_MODE[mode].exp_fine ) * (FLOAT)IMX577_MODE[mode].hdocyc / (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0);
	out->exp_msec = exp_sec * 1000.0;
	out->fps = (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / IMX577_MODE[mode].hdocyc / FLL;
	out->hdocyc = IMX577_MODE[mode].hdocyc;
	out->vdocyc = FLL;
	out->again_times = again;
	out->dgain_times = dgain;

	// Reg
	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = IMX577_MODE[mode].hdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = FLL;
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] = CIT;
	out_reg->reg[E_CAMERA_SENSOR_REG_AGAIN] = AGG;
	out_reg->reg[E_CAMERA_SENSOR_REG_DGAIN] = DG;

	return E_CAMERA_ERR_OK;
}

E_CAMERA_ERR_CODE Sensor_IMX577_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in)
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

#define D_DOL_L		0
#define D_DOL_S		1
#define D_DOL_C		2

E_CAMERA_ERR_CODE Sensor_IMX577_DOL_Calc_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_CALC_EXP_IN* in, T_CAMERA_EXP* out, T_CAMERA_EXP_REG* out_reg)
{
	UINT32	MGN = 20;
	UINT32	BRL;			// Base Read Line
	UINT32	CIT[3];			// Coarse Integration Time
	UINT32	FLL[3];			// Frame Length Line
	UINT32	GAP;
	UINT32	AGG[2];			// Analog Gain Global
	UINT32	DG[2];			// Digital Gain
	UINT32	mgn[3];
	FLOAT	exp_sec[3];
	FLOAT	gain_times[2];
	FLOAT	again[2];
	FLOAT	dgain[2];
	FLOAT	default_fps, min_fps, max_fps, req_fps;
	INT32	i;
	
	BRL = IMX577_MODE[mode].active_pix_lines + 37; // from spec
	
	exp_sec[D_DOL_L] = in->exp_msec / 1000.0;
	exp_sec[D_DOL_S] = in->DOL_short_exp_msec / 1000.0;
	
	// clip short exposure
	if( exp_sec[D_DOL_S] > exp_sec[D_DOL_L] ){
		exp_sec[D_DOL_S] = exp_sec[D_DOL_L];
	}
	
	// Exposure Long, Short, Combine
	exp_sec[D_DOL_C] = exp_sec[D_DOL_L] + exp_sec[D_DOL_S];
	mgn[D_DOL_L] = MGN*2 + 2;
	mgn[D_DOL_S] = BRL + MGN;
	mgn[D_DOL_C] = MGN*2;
	for( i = 0; i < 3; i ++ ){
		// Coarse Integration Time
		CIT[i] = (UINT32) ( exp_sec[i] * (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / IMX577_MODE[mode].dol_hdocyc / 2.0 );
		
		if( CIT[i] > 65525 - mgn[i] ){ // 65525 is max of FLL
			CIT[i] = 65525 - mgn[i];
		}
		if( CIT[i] < 8 ){
			CIT[i] = 8; // recomend min
		}
		// Frame Length Line
		if( CIT[i] > IMX577_MODE[mode].dol_vdocyc - mgn[i] ){
			FLL[i] = CIT[i] + mgn[i];
		}
		else{
			FLL[i] = IMX577_MODE[mode].dol_vdocyc;
		}
		
		// Clip by FPS
		if( in->max_fps == D_CAMERA_FPS_AUTO && in->min_fps == D_CAMERA_FPS_AUTO ){
			// auto, nothing to do
		}else{
			default_fps = (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX577_MODE[mode].dol_hdocyc / (FLOAT)IMX577_MODE[mode].dol_vdocyc;
			default_fps /= 2.0;
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
			req_fps = (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX577_MODE[mode].dol_hdocyc / (FLOAT)FLL[i];
			req_fps /= 2.0;
			if( req_fps < min_fps ){
				FLL[i] = (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX577_MODE[mode].dol_hdocyc / min_fps;
				FLL[i] /= 2;
			}
			if( req_fps > max_fps ){
				FLL[i] = (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / (FLOAT)IMX577_MODE[mode].dol_hdocyc / max_fps;
				FLL[i] /= 2;
			}
			// clip CIT
			if( CIT[i] > ( FLL[i] - mgn[i] ) ){
				CIT[i] = ( FLL[i] - mgn[i] );
			}
		}
	}
	
	// Clip CIT by 2
	CIT[D_DOL_L] &= 0xFFFE;
	CIT[D_DOL_S] &= 0xFFFE;
	CIT[D_DOL_C] &= 0xFFFE;
	
	FLL[D_DOL_L] &= 0xFFFE;
	FLL[D_DOL_S] &= 0xFFFE;
	FLL[D_DOL_C] &= 0xFFFE;
	
	// Check Error
	if( CIT[D_DOL_L] > CIT[D_DOL_C] ){
		CAMERA_PRINT_ERROR(1);
	}
	if( CIT[D_DOL_S] > CIT[D_DOL_C] ){
		CAMERA_PRINT_ERROR(2);
	}
	
	// Clip GAP
	GAP = in->DOL_short_max_gap_lines & 0xFFFE;
	if( FLL[D_DOL_C] - BRL < GAP ){
		GAP = FLL[D_DOL_C] - BRL;
		GAP &= 0xFFFE;
	}
	
	// Clip by GAP
	if( CIT[D_DOL_S] + MGN > GAP ){
		CIT[D_DOL_S] = GAP - MGN;
	}
	
	// Clip by FLL Combine
	if( CIT[D_DOL_L] + CIT[D_DOL_S] + MGN*2 > FLL[D_DOL_C] ){
		CIT[D_DOL_L] = FLL[D_DOL_C] - CIT[D_DOL_S] - MGN*2;
	}
	
	if( CIT[D_DOL_L] < 2 || CIT[D_DOL_L] > (FLL[D_DOL_C] - (2 * MGN) - 2)) {
		CAMERA_PRINT_ERROR(3);
		printf("CIT[D_DOL_L:%d, FLL[D_DOL_C]:%d\n", CIT[D_DOL_L], FLL[D_DOL_C]);
	}

	if( CIT[D_DOL_S] < 2 || CIT[D_DOL_S] > (FLL[D_DOL_C] - BRL - MGN)) {
		CAMERA_PRINT_ERROR(4);
		printf("CIT[D_DOL_S:%d, FLL[D_DOL_C]:%d\n", CIT[D_DOL_S], FLL[D_DOL_C]);
	}
	// Gain
	gain_times[D_DOL_L] = in->gain_times;
	gain_times[D_DOL_S] = in->DOL_short_gain_times;
	for( i = 0; i < 2; i ++ ){
		// Analog Gain
		again[i] = gain_times[i];
		if( again[i] < 1.0 ){
			again[i] = 1.0;
		}

		AGG[i] = (UINT32)( 1024.0 - (1024.0 / again[i]) );
		if( AGG[i] > 978 ){
			AGG[i] = 978;
		}
		again[i] = ( 1024.0 / (1024.0 - (FLOAT)AGG[i]) );

		// Digital Gain if again cannot equal to in->gain_times
		dgain[i] = gain_times[i] / again[i];
		DG[i] = (UINT32)( dgain[i] * 256.0 );
		if( DG[i] < 0x100 ){
			DG[i] = 0x100;
		}
		if( DG[i] > 0xFFF ){
			DG[i] = 0xFFF;
		}
		dgain[i] = (FLOAT)DG[i]/256.0;
	}

	// Revert
	exp_sec[D_DOL_L] = (FLOAT)( CIT[D_DOL_L] ) * (FLOAT)IMX577_MODE[mode].dol_hdocyc / (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) * 2.0;
	out->exp_msec = exp_sec[D_DOL_L] * 1000.0;
	out->fps = (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) / IMX577_MODE[mode].dol_hdocyc / FLL[D_DOL_C] / 2.0;
	out->hdocyc = IMX577_MODE[mode].dol_hdocyc;
	out->vdocyc = FLL[D_DOL_C];
	out->again_times = again[D_DOL_L];
	out->dgain_times = dgain[D_DOL_L];
	exp_sec[D_DOL_S] = (FLOAT)( CIT[D_DOL_S] ) * (FLOAT)IMX577_MODE[mode].dol_hdocyc / (IMX577_MODE[mode].Mpps_HDclk * 1000.0 * 1000.0) * 2.0;
	out->DOL_short_exp_msec = exp_sec[D_DOL_S] * 1000.0;
	out->DOL_short_again_times = again[D_DOL_S];
	out->DOL_short_dgain_times = dgain[D_DOL_S];
	
	// Reg
	out_reg->reg[E_CAMERA_SENSOR_REG_HDOCYC] = IMX577_MODE[mode].dol_hdocyc;
	out_reg->reg[E_CAMERA_SENSOR_REG_VDOCYC] = FLL[D_DOL_C];
	out_reg->reg[E_CAMERA_SENSOR_REG_EXP] = CIT[D_DOL_L];
	out_reg->reg[E_CAMERA_SENSOR_REG_AGAIN] = AGG[D_DOL_L];
	out_reg->reg[E_CAMERA_SENSOR_REG_DGAIN] = DG[D_DOL_L];
	out_reg->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_OFST] = GAP;
	//out_reg->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_OFST] = CIT[D_DOL_S] + MGN;
	out_reg->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_EXP] = CIT[D_DOL_S];
	out_reg->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_AGAIN] = AGG[D_DOL_S];
	out_reg->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_DGAIN] = DG[D_DOL_S];

	return E_CAMERA_ERR_OK;
}


E_CAMERA_ERR_CODE Sensor_IMX577_DOL_Set_Exp(E_CAMERA_ID sensor_id, UINT32 mode, const T_CAMERA_EXP_REG* in )
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

	// Exposure
	data[0] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_EXP] >> 8) & 0xFF);
	data[1] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_EXP]       & 0xFF);
	data[2] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_EXP] >> 8) & 0xFF);
	data[3] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_EXP]       & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x00EA,
							data,
							4);

	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// Frame length line
	data[0] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_VDOCYC] >> 8) & 0xFF);
	data[1] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_VDOCYC]       & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x0340,
							data,
							2);

	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// Short Offset
	data[0] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_OFST] >> 8) & 0xFF);
	data[1] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_OFST]       & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x00E6,
							data,
							2);

	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// Analog Gain Long and Short
	data[0] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_AGAIN] >> 8) & 0xFF);
	data[1] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_AGAIN]       & 0xFF);
	data[2] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_AGAIN] >> 8) & 0xFF);
	data[3] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_AGAIN]       & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x00F0,
							data,
							4);

	Dd_ARM_Dsb_Pou();
	Dd_ARM_Isb_Pou();

	// Digital Gain Long and Short
	data[0] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_DGAIN] >> 8) & 0xFF);
	data[1] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_DGAIN]       & 0xFF);
	data[2] = (UCHAR)((in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_DGAIN] >> 8) & 0xFF);
	data[3] = (UCHAR)( in->reg[E_CAMERA_SENSOR_REG_DOL_SHORT_DGAIN]       & 0xFF);
	Camera_Common_I2C_Write(CAMERA.sensor[sensor_id].i2c_ch,
							CAMERA.sensor[sensor_id].i2c_slave7bit,
							0x00F6,
							data,
							4);

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

E_CAMERA_ERR_CODE Sensor_IMX577_Get_PD_Info(E_CAMERA_ID sensor_id, UINT32 mode, ULONG sta_addr, USHORT pd_num, T_CAMERA_PD_DATA* pd_info)
{
	USHORT* pdxxx_addr = (USHORT*)sta_addr;
	USHORT  confidence = 0;
	USHORT	pd_data = 0;
	USHORT  temp0 = 0, temp1 = 0, temp2 = 0;
	
	//Header: 8 pixels
	pdxxx_addr = pdxxx_addr + 8;
	//PD Info: each 4 pixels
	pdxxx_addr = pdxxx_addr + pd_num * 4;

	if(IMX577_MODE[mode].input_bit_width == 10){
		temp0 = (*pdxxx_addr & 0x03FC);
		pdxxx_addr++;
		temp1 = (*pdxxx_addr & 0x03FC);
		pdxxx_addr++;
		temp2 = (*pdxxx_addr & 0x03F0);

		confidence = (temp0 << 1) | (temp1 >> 7);
		pd_data = ((temp1 & 0x007C) << 4) | (temp2 >> 4);
	}else if(IMX577_MODE[mode].input_bit_width == 12){
		temp0 = (*pdxxx_addr & 0x0FF0);
		pdxxx_addr++;
		temp1 = (*pdxxx_addr & 0x0FF0);
		pdxxx_addr++;
		temp2 = (*pdxxx_addr & 0x0FC0);

		confidence = (temp0 >> 1) | (temp1 >> 9);
		pd_data = ((temp1 & 0x001F0) << 2) | (temp2 >> 6);
	}

	pd_info->level = (FLOAT)confidence;
	if(pd_data >= 0x0400){
		pd_data = pd_data - 0x0400;
		pd_info->pd = 0.0 - (FLOAT)pd_data / 16.0;
	}else{
		pd_info->pd = (FLOAT)pd_data / 16.0;
	}
	
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
