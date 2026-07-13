/**
 * @file		color_bar.c
 * @brief		Draw Color Bar
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#pragma arm section rwdata ="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "ddim_typedef.h"
#include "color_bar.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* White, Yellow, Cyan, Green, Magenta, Red, Blue, Black */
#define D_BF_COLOR_BAR_NUMBER_OF_GRAY	(16)
#define D_BF_COLOR_BAR_VALUE_MAX		(255)
#define D_BF_COLOR_BAR_VALUE_MID		(128)
#define D_BF_COLOR_BAR_VALUE_MIN		(0)

#define D_BF_COLOR_BAR_Y_OFFSET			(0)
#define D_BF_COLOR_BAR_CB_OFFSET		(128)
#define D_BF_COLOR_BAR_CR_OFFSET		(128)

#define D_BF_COLOR_BAR_DISP_SIZE_X		D_WIDTH_VGA
#if 0	/* HVGA */
#define D_BF_COLOR_BAR_DISP_SIZE_Y		D_LINES_HVGA
#else	/* VGA */
#define D_BF_COLOR_BAR_DISP_SIZE_Y		D_LINES_VGA
#endif

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
typedef struct {
	UCHAR R;	/* Red Value   */
	UCHAR G;	/* Green Value */
	UCHAR B;	/* Blue Value  */
} T_BF_COLOR_BAR_RGB_VALUE;

typedef struct {
	UCHAR Y;
	UCHAR Cb;
	UCHAR Cr;
} T_BF_COLOR_BAR_YUV_VALUE;

typedef enum {
	E_BF_COLOR_BAR_COLOR_INDEX_WHITE	= 0,
	E_BF_COLOR_BAR_COLOR_INDEX_YERLLOW,
	E_BF_COLOR_BAR_COLOR_INDEX_SYAN,
	E_BF_COLOR_BAR_COLOR_INDEX_GREEN,
	E_BF_COLOR_BAR_COLOR_INDEX_MAGENDA,
	E_BF_COLOR_BAR_COLOR_INDEX_RED,
	E_BF_COLOR_BAR_COLOR_INDEX_BLUE,
	E_BF_COLOR_BAR_COLOR_INDEX_BLACK,
	E_BF_COLOR_BAR_COLOR_INDEX_MAX
} E_BF_COLOR_BAR_COLOR_INDEX; /* for Color Bar */

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
/* YC conversion coefficient Table (Y) Type */
/***
R601 coefficient
static const float gBF_Color_Bar_Rgb2Yuv_Coeff_Y[3]  = { 0.257, 0.504, 0.098 };
static const float gBF_Color_Bar_Rgb2Yuv_Coeff_Cb[3] = { 0.148, 0.291, 0.439 };
static const float gBF_Color_Bar_Rgb2Yuv_Coeff_Cr[3] = { 0.439, 0.368, 0.071 };
**/

/* Full scale Type */
static const FLOAT gBF_Color_Bar_Rgb2Yuv_Coeff_Y[3]  = { 0.299, 0.587, 0.114 };
static const FLOAT gBF_Color_Bar_Rgb2Yuv_Coeff_Cb[3] = { 0.169, 0.331, 0.500 };
static const FLOAT gBF_Color_Bar_Rgb2Yuv_Coeff_Cr[3] = { 0.500, 0.419, 0.081 };

const T_BF_COLOR_BAR_RGB_VALUE gRGB_ColorTbl[ E_BF_COLOR_BAR_COLOR_INDEX_MAX ] = {
	{ D_BF_COLOR_BAR_VALUE_MAX, D_BF_COLOR_BAR_VALUE_MAX, D_BF_COLOR_BAR_VALUE_MAX }, /* white */
	{ D_BF_COLOR_BAR_VALUE_MAX, D_BF_COLOR_BAR_VALUE_MAX, D_BF_COLOR_BAR_VALUE_MIN }, /* yellow */
	{ D_BF_COLOR_BAR_VALUE_MIN, D_BF_COLOR_BAR_VALUE_MAX, D_BF_COLOR_BAR_VALUE_MAX }, /* syan */
	{ D_BF_COLOR_BAR_VALUE_MIN, D_BF_COLOR_BAR_VALUE_MAX, D_BF_COLOR_BAR_VALUE_MIN }, /* green */
	{ D_BF_COLOR_BAR_VALUE_MAX, D_BF_COLOR_BAR_VALUE_MIN, D_BF_COLOR_BAR_VALUE_MAX }, /* magenda */
	{ D_BF_COLOR_BAR_VALUE_MAX, D_BF_COLOR_BAR_VALUE_MIN, D_BF_COLOR_BAR_VALUE_MIN }, /* red */
	{ D_BF_COLOR_BAR_VALUE_MIN, D_BF_COLOR_BAR_VALUE_MIN, D_BF_COLOR_BAR_VALUE_MAX }, /* blue */
	{ D_BF_COLOR_BAR_VALUE_MIN, D_BF_COLOR_BAR_VALUE_MIN, D_BF_COLOR_BAR_VALUE_MIN }  /* black */
};

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/**
 * Drawing Color Bar
 * @param y_addr: Absolute Y data address
 *		  Cr_addr: Absolute Cbdata address
 *		  Cb_addr: Absolute Cr data address
**/
VOID BF_Color_Bar_Draw(T_IMAGE_ADDR_YCC *destination)
{
	T_BF_COLOR_BAR_YUV_VALUE	rgb_data[ E_BF_COLOR_BAR_COLOR_INDEX_MAX ];
	T_BF_COLOR_BAR_YUV_VALUE	gray_data[ D_BF_COLOR_BAR_NUMBER_OF_GRAY ];
	UINT32 i, x, y;
	BYTE gray_rgb_data;
	BYTE* y_addr;
	BYTE* cb_addr;
	BYTE* cr_addr;

	y_addr  = (BYTE*)(destination->y);
	cb_addr = (BYTE*)(destination->cb);
	cr_addr = (BYTE*)(destination->cr);
	/* RGB to YCbCr convert method									*/
	/*	Y =   0.257R + 0.504G + 0.098B + 16							*/
	/*	Cb = -0.148R - 0.291G + 0.439B + 128						*/
	/*	Cr =  0.439R - 0.368G - 0.071B + 128						*/
	/*																*/
	/********** Color Bar Generation **********/
	/* White, Yellow, Cyan, Green, Magenta, Red, Blue, Black */

	/********** YUV Convert Execute   at ITU-R BT601 ***********/
	/* convert RGB -> YUV %601 */
	for( i = 0; i < E_BF_COLOR_BAR_COLOR_INDEX_MAX; i++ ) {
		rgb_data[i].Y  = (unsigned char)( gBF_Color_Bar_Rgb2Yuv_Coeff_Y[0]  * gRGB_ColorTbl[i].R + gBF_Color_Bar_Rgb2Yuv_Coeff_Y[1]  * gRGB_ColorTbl[i].G + gBF_Color_Bar_Rgb2Yuv_Coeff_Y[2]  * gRGB_ColorTbl[i].B + D_BF_COLOR_BAR_Y_OFFSET);
		rgb_data[i].Cb = (unsigned char)(-gBF_Color_Bar_Rgb2Yuv_Coeff_Cb[0] * gRGB_ColorTbl[i].R - gBF_Color_Bar_Rgb2Yuv_Coeff_Cb[1] * gRGB_ColorTbl[i].G + gBF_Color_Bar_Rgb2Yuv_Coeff_Cb[2] * gRGB_ColorTbl[i].B + D_BF_COLOR_BAR_CB_OFFSET);
		rgb_data[i].Cr = (unsigned char)( gBF_Color_Bar_Rgb2Yuv_Coeff_Cr[0] * gRGB_ColorTbl[i].R - gBF_Color_Bar_Rgb2Yuv_Coeff_Cr[1] * gRGB_ColorTbl[i].G - gBF_Color_Bar_Rgb2Yuv_Coeff_Cr[2] * gRGB_ColorTbl[i].B + D_BF_COLOR_BAR_CR_OFFSET);
	}

	for( i = 0, gray_rgb_data = 0; i < D_BF_COLOR_BAR_NUMBER_OF_GRAY; i++, gray_rgb_data += (256 / ( D_BF_COLOR_BAR_NUMBER_OF_GRAY - 1))) {
		gray_data[i].Y	= (unsigned char)( gBF_Color_Bar_Rgb2Yuv_Coeff_Y[0]  * gray_rgb_data + gBF_Color_Bar_Rgb2Yuv_Coeff_Y[1]  * gray_rgb_data + gBF_Color_Bar_Rgb2Yuv_Coeff_Y[2]  * gray_rgb_data + D_BF_COLOR_BAR_Y_OFFSET);
		gray_data[i].Cb = (unsigned char)(-gBF_Color_Bar_Rgb2Yuv_Coeff_Cb[0] * gray_rgb_data - gBF_Color_Bar_Rgb2Yuv_Coeff_Cb[1] * gray_rgb_data + gBF_Color_Bar_Rgb2Yuv_Coeff_Cb[2] * gray_rgb_data + D_BF_COLOR_BAR_CB_OFFSET);
		gray_data[i].Cr = (unsigned char)( gBF_Color_Bar_Rgb2Yuv_Coeff_Cr[0] * gray_rgb_data - gBF_Color_Bar_Rgb2Yuv_Coeff_Cr[1] * gray_rgb_data - gBF_Color_Bar_Rgb2Yuv_Coeff_Cr[2] * gray_rgb_data + D_BF_COLOR_BAR_CR_OFFSET);
	}
	/* RGB Data Write */
	for(y = 0; y < (D_BF_COLOR_BAR_DISP_SIZE_Y / 2); y++){
		for(x = 0; x < D_BF_COLOR_BAR_DISP_SIZE_X; x++){
			*y_addr++ = rgb_data[ (E_BF_COLOR_BAR_COLOR_INDEX_MAX * x) / D_BF_COLOR_BAR_DISP_SIZE_X ].Y;	/* pgr0000 */
		}
	}
	for(y = 0; y < (D_BF_COLOR_BAR_DISP_SIZE_Y / 2 / 2); y++){
		for(x = 0; x < (D_BF_COLOR_BAR_DISP_SIZE_X / 2); x++){
			*cb_addr++ = rgb_data[ (2 * E_BF_COLOR_BAR_COLOR_INDEX_MAX * x) / D_BF_COLOR_BAR_DISP_SIZE_X ].Cb;
			*cr_addr++ = rgb_data[ (2 * E_BF_COLOR_BAR_COLOR_INDEX_MAX * x) / D_BF_COLOR_BAR_DISP_SIZE_X ].Cr;
		}
	}
	/* Gray Data Write */
	for(y = 0; y < (D_BF_COLOR_BAR_DISP_SIZE_Y / 2); y++){
		for(x = 0; x < D_BF_COLOR_BAR_DISP_SIZE_X; x++){
			*y_addr++ = gray_data[ (D_BF_COLOR_BAR_NUMBER_OF_GRAY * x) / D_BF_COLOR_BAR_DISP_SIZE_X ].Y;
		}
	}
	for(y = 0; y < (D_BF_COLOR_BAR_DISP_SIZE_Y / 2 / 2); y++){
		for(x = 0; x < (D_BF_COLOR_BAR_DISP_SIZE_X / 2); x++){
			*cb_addr++ = gray_data[ (2 * D_BF_COLOR_BAR_NUMBER_OF_GRAY * x) / D_BF_COLOR_BAR_DISP_SIZE_X ].Cb;
			*cr_addr++ = gray_data[ (2 * D_BF_COLOR_BAR_NUMBER_OF_GRAY * x) / D_BF_COLOR_BAR_DISP_SIZE_X ].Cr;
		}
	}
}
