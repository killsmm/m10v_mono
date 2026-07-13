/**
 * @file		osd_general.h
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _OSD_GENERAL_H_
#define _OSD_GENERAL_H_

#include "os_user_custom.h"
#include "debug.h"
#include "dd_arm.h"
#include "fs_if.h"
#include "sdram_map_common.h"
#include "play_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define CO_BF_OSD_GENERAL_DEBUG_HISTOGRAM			// Debug for histogram display
//#define CO_BF_OSD_GENERAL_LCD_POWER_CTRL_ON

#define D_BF_OSD_GENERAL_DMA_CH_OSD				(0)	/**< DAM transfer */

#define D_BF_OSD_GENERAL_OSD_WIDTH				(WIDTH_VGA)	/**< LCD width */
#define D_BF_OSD_GENERAL_OSD_LINES				(LINES_VGA)	/**< LCD lines */

#define D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_FHD	(1920)	/**< FHD MAIN width */
#define D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_LINES_FHD	(1080)	/**< FHD MAIN lines */

#define D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_FHD_4_3	(1440)	/**< FHD MAIN width */
#define D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_LINES_FHD_4_3 	(1080)	/**< FHD MAIN lines */

#define D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_HD		(1280)	/**< HD MAIN width */
#define D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_LINES_HD		(720)	/**< HD MAIN lines */

#define D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_SXVGA		(1280)	/**< SXVGA MAIN width */
#define D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_LINES_SXVGA		(960)	/**< SXVGA MAIN lines */

#define D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_HVGAW		(640)	/**< HVGAW MAIN width */
#define D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_LINES_HVGAW		(368)	/**< HVGAW MAIN lines */

#define D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_WIDTH_QVGA		(320)	/**< QVGA MAIN width */
#define D_BF_OSD_GENERAL_RESOLUTION_MAIN_INPUT_LINES_QVGA		(240)	/**< QVGA MAIN lines */

#define D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_WIDTH_FHD		(1440)	/**< FHD OSD width */
#define D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_LINES_FHD		(1080)	/**< FHD OSD lines */

#define D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_WIDTH_HD		(960)	/**< HD OSD width */
#define D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_LINES_HD		(720)	/**< HD OSD lines */

#define D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_SD60_WIDTH	(720)	/**< SD60 OSD width */
#define D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_SD60_LINES	(480)	/**< SD60 OSD lines */

#define D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_SD50_WIDTH	(720)	/**< SD50 OSD width */
#define D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_SD50_LINES	(576)	/**< SD50 OSD lines */

#define D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_WIDTH_SD		(640)	/**< SD OSD width */
#define D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_LINES_SD		(480)	/**< SD OSD lines */

#define D_BF_OSD_GENERAL_ALPHA_OPAQUE 		(255)	/**< Alpha opaque */
#define D_BF_OSD_GENERAL_BIT_NUM_PER_PIXEL	(8)		/**< Bit per pixel */
#define D_BF_OSD_GENERAL_ALPHA_TRANS		(0)		/**< Alpha trans */

#if (FJ_OSD_COMM_RGBAXXXX == 2)
typedef unsigned short OSD_USHORT_INT;
#else
typedef unsigned int OSD_USHORT_INT;
#endif


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** YCC format */
typedef enum {
	D_BF_OSD_GENERAL_YCC_Y = 0,		/**< Y */
	D_BF_OSD_GENERAL_YCC_CB,		/**< Cb */
	D_BF_OSD_GENERAL_YCC_CR,		/**< Cr */
	D_BF_OSD_GENERAL_YCC_END		/**< Number of enums */
} D_BF_OSD_GENERAL_YCC;

/** RGBA format */
typedef enum {
	D_BF_OSD_GENERAL_RGBA_R = 0,	/**< R */
	D_BF_OSD_GENERAL_RGBA_G,		/**< G */
	D_BF_OSD_GENERAL_RGBA_B,		/**< B */
	D_BF_OSD_GENERAL_RGBA_A,		/**< A */
	D_BF_OSD_GENERAL_RGBA_END		/**< Number of enums */
} D_BF_OSD_GENERAL_RGBA;

/** Bitmask of RGBA format */
typedef enum {
	E_BF_OSD_GENERAL_RGBA_MASK_A = 0x000000ff,			/**< A */
	E_BF_OSD_GENERAL_RGBA_MASK_B = 0x0000ff00,			/**< B */
	E_BF_OSD_GENERAL_RGBA_MASK_G = 0x00ff0000,			/**< G */
	E_BF_OSD_GENERAL_RGBA_MASK_R = (INT32)0xff000000	/**< R */
}E_BF_OSD_GENERAL_RGBA_MASK;

/** Bitshift of RGBA format */
typedef enum {
	E_BF_OSD_GENERAL_RGBA_RIGHT_SHIFT_B = 8,	/**< B */
	E_BF_OSD_GENERAL_RGBA_RIGHT_SHIFT_G = 16,	/**< G */
	E_BF_OSD_GENERAL_RGBA_RIGHT_SHIFT_R = 24	/**< R */
}E_BF_OSD_GENERAL_RGBA_RIGHT_SHIFT;

/** Clock setting */
typedef enum {
	E_BF_OSD_GENERAL_CLK_ON = 0,		/**< Clock ON */
	E_BF_OSD_GENERAL_CLK_OFF,			/**< Clock OFF */
} E_BF_OSD_GENERAL_CLK;

/** Bit pattern of HIFCLK. */
typedef enum {
	E_BF_OSD_GENERAL_CLK_SEL_594		= 0x08,	/**< 594MHZ */
	E_BF_OSD_GENERAL_CLK_SEL_297		= 0x09,	/**< 297MHZ */
	E_BF_OSD_GENERAL_CLK_SEL_148_5		= 0x0A,	/**< 148.5MHZ */
	E_BF_OSD_GENERAL_CLK_SEL_74_25		= 0x0B,	/**< 74.25MHZ */
	E_BF_OSD_GENERAL_CLK_SEL_27			= 0x0C,	/**< 27MHZ */
} E_BF_OSD_GENERAL_CLK_SEL;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define BF_Osd_General_Get_Max(a, b)  (((a) > (b)) ? (a) : (b))
#define BF_Osd_General_Get_Min(a, b)  (((a) < (b)) ? (a) : (b))

#define BF_Osd_General_RGBA_To_R(word) (((word) & (UINT32)E_BF_OSD_GENERAL_RGBA_MASK_R) >> E_BF_OSD_GENERAL_RGBA_RIGHT_SHIFT_R)
#define BF_Osd_General_RGBA_To_G(word) (((word) & E_BF_OSD_GENERAL_RGBA_MASK_G) >> E_BF_OSD_GENERAL_RGBA_RIGHT_SHIFT_G)
#define BF_Osd_General_RGBA_To_B(word) (((word) & E_BF_OSD_GENERAL_RGBA_MASK_B) >> E_BF_OSD_GENERAL_RGBA_RIGHT_SHIFT_B)
#define BF_Osd_General_RGBA_To_A(word) ((word) & E_BF_OSD_GENERAL_RGBA_MASK_A)
#define BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(a)    (((0xF0000000 & a)>>16)|((0x00F00000 & a)>>12)|((0x0000F000 & a)>>8)|((0x000000F0 & a)>>4))
#define BF_Osd_General_TO_RGBA8888_FROM_RGBA4444(a)    (((0xF000 & a)<<16)|((0xF000 & a)<<12)|((0x0F00 & a)<<12)|((0x0F00 & a)<<8) | ((0x00F0 & a)<<8) | ((0x00F0 & a)<<4) | ((0x000F & a)<<4)| (0x000F & a))
#define BF_Osd_General_GET_ARRAY_ELEMENT_NUMS(a)	(sizeof(a)/sizeof(a[0])) //Get a array's element nums
/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
Compare 2 string no case
@param[in]	string1	string 1
@param[in]	string2	string 2
@param[in]	count	string size
@retval 0		string1 = string2.
@retval plus	string1 > string2.
@retval minus	string1 < string2.
*/
extern INT32 BF_Osd_General_Strnicmp(const CHAR* string1, const CHAR* string2, size_t count);

/**
Fill a box area with data
@param[in]	fill_data	Fill data
@param[in]	gl_width	global width
@param[in]	dst_addr	Address filled
@param[in]	fill_width	box width
@param[in]	fill_lines	box lines
*/
extern VOID BF_Osd_General_Fill_Box(BYTE fill_data, ULONG gl_width, ULONG dst_addr, ULONG fill_width, ULONG fill_lines);

/**
Fill a box area with data for GR
@param[in]	fill_data	Fill data
@param[in]	gl_width	global width
@param[in]	dst_addr	Address filled
@param[in]	fill_width	box width
@param[in]	fill_lines	box lines
*/
extern VOID BF_Osd_General_Fill_Box_For_GR(OSD_USHORT_INT fill_data, ULONG gl_width, OSD_USHORT_INT* dst_addr, ULONG fill_width, ULONG fill_lines);

/**
Fill a box area with data part for GR
@param[in]	set_data	set data
@param[in]	gl_width	global width
@param[in]	dst_addr	Address filled
@param[in]	bit_patten	set bit pattern
@param[in]	fill_width	box width
@param[in]	fill_lines	box lines
*/
extern VOID BF_Osd_General_Fill_Box_Part_For_GR(OSD_USHORT_INT set_data, ULONG gl_width, OSD_USHORT_INT* dst_addr, E_BF_OSD_GENERAL_RGBA_MASK bit_patten, ULONG fill_width, ULONG fill_lines);

/**
Copy a box area the another
@param[in]	src_addr	Copy source address
@param[in]	dst_gl_width	global width
@param[in]	dst_addr	copy destination Address
@param[in]	width	box width
@param[in]	lines	box lines
*/
extern VOID BF_Osd_General_Copy_To_Box(ULONG src_addr, USHORT dst_gl_width, ULONG dst_addr, USHORT width, USHORT lines );

/**
Copy a box area the another
@param[in]	src_addr	Copy source address
@param[in]	src_gl_width	global width of source
@param[in]	dst_gl_width	global width of destination
@param[in]	dst_addr	copy destination Address
@param[in]	width	box width
@param[in]	lines	box lines
*/
extern VOID BF_Osd_General_Copy_Box_To_Box(ULONG src_addr, USHORT src_gl_width, USHORT dst_gl_width, ULONG dst_addr, USHORT width, USHORT lines);

/**
Get greatest common divisor
@param[in]	a	number
@param[in]	b	number
@return		greatest common divisor
*/
extern UINT32 BF_Osd_General_greatest_common_divisor(UINT32 a, UINT32 b);

/**
Get fraction
@param[in]	a	number
@param[in]	b	number
@return		fraction
*/
extern VOID BF_Osd_General_Fraction(UINT32* a,  UINT32* b);

/**
Convert RGBA8888 to RGBA4444
@param[in/out] vgr_address	RGBA8888 data address
*/
extern VOID BF_Osd_General_Conv_RGBA8888_To_RGBA4444(UINT32* vgr_address);

/**
Get bit that is 1 position which from 0
@param[in]	bitmask		bit map
@param[in]	bit_pos		the buffer that bit is 1 position
@param[in]	bit_pos_num	bit_pos' ssize
@return		The size of bit_pos gotten
@code
		UINT32 bitmask= 0x00005012;
		UINT32 bit_pos[3];
		UINT32 bit_pos_num = BF_Osd_General_Get_Bit_Position(bitmask,bit_pos,3);
		//bit_pos_num= 3
		//bit_pos[0] = 1
		//bit_pos[1] = 4
		//bit_pos[2] = 8
		bitmask= 0x00005000;
		bit_pos_num = BF_Osd_General_Get_Bit_Position(bitmask,bit_pos,3);
		//bit_pos_num= 2
		//bit_pos[0] = 8
		//bit_pos[1] = 10
@endcode
*/
extern UINT32 BF_Osd_General_Get_Bit_Position(UINT32  bitmask, UINT32* bit_pos, UINT32 bit_pos_num);

/**
Convert YCbCr420 to RGBA8888
@param[in]	jpgmng	jpeg information
@param[in]	screen_rect_data	screen rect data
@param[in]	dst_size	[0]: width [1]:height
@param[in]	rgb_address	RGBA8888/4444 output data address (I)
@retval D_DDIM_OK	OK.
@retval other		REC macro error.
*/
extern INT32 BF_Osd_General_Conv_YCC420_To_RGBA( const T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG *jpgmng 
													, const T_Rect *screen_rect_data
													, USHORT dst_size[2]
													, ULONG rgb_address);

/**
Convert Layer to Define
@param[in]	layer_id	layer ID
@retval D_IM_DISP_BLD_OSD0	OSD-0 layer.
@retval D_IM_DISP_BLD_OSD1	OSD-1 layer.
@retval D_IM_DISP_BLD_GRID	Grid layer.
@retval D_IM_DISP_BLD_FACE	Face frame layer.
@retval -1					Parameter error.
*/
extern INT8 BF_Osd_General_Conv_Layer_To_Define(E_OSDLayer layer_id);

/**
Convert Define to Layer
@param[in] disp_type	Display H/W type.
						@ref FJ_DISP_TYPE
@param[in]	layer_id	layer ID
@retval E_OSDLayer_HDMI_OSD0	OSD-0 layer.
@retval E_OSDLayer_HDMI_OSD1	OSD-1 layer.
@retval E_OSDLayer_HDMI_GRID	Grid layer.
@retval E_OSDLayer_HDMI_FACE	Face frame layer.
*/
extern E_OSDLayer BF_Osd_General_Conv_Define_To_Layer(FJ_DISP_TYPE disp_type, INT8 layer_id);


#endif // _OSD_GENERAL_H_
