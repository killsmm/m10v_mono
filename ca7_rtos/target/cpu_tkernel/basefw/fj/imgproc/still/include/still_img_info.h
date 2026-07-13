/**
 * @file		still_img_info.h
 * @brief		Image data information structure
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef __STILL_IMG_INFO_H__
#define __STILL_IMG_INFO_H__

// Still header
#include "image_info.h"

// FJ header
#include "fj_still.h"

// DDIM header
#include "ddim_typedef.h"
#include "im_iip.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

#define D_BF_STILL_AFNTBL_ALIGN	(8)


/*==============================================================================*/
/* Macro declaration															*/
/*==============================================================================*/
#define BF_Still_ImgInfo_Calc_Y_Lines( img_info )	((img_info)->pixs.lines)
#define BF_Still_ImgInfo_Calc_Cr_Lines( img_info )	(BF_Still_ImgInfo_Calc_Cb_Lines( img_info ))
#define BF_Still_ImgInfo_Calc_Y_Bytes( img_info )	(BF_Still_ImgInfo_Calc_Y_Lines( img_info ) * (img_info)->g_width.y)
#define BF_Still_ImgInfo_Calc_Cb_Bytes( img_info )	(BF_Still_ImgInfo_Calc_Cb_Lines( img_info ) * (img_info)->g_width.cb)
#define BF_Still_ImgInfo_Calc_Cr_Bytes( img_info )	(BF_Still_ImgInfo_Calc_Cr_Lines( img_info ) * (img_info)->g_width.cr)

#define M_BF_STILL_AFN_CALC_PIT( dst, src )					((UINT32)(((UINT64)D_IM_IIP_PARAM_AFN_VAL_1_0 * ((src) -1) -1) / ((dst) -1)))
#define M_BF_STILL_AFN_CALC_PIT_WINL( dst, src )			((LONG)(((INT64)((src) -D_IM_IIP_PARAM_AFN_VAL_1_0) -1) / ((dst) -1)))
#define M_BF_STILL_AFN_CALC_PIT_WINL_NEAREST( dst, src )	((LONG)(((INT64)(src) -D_IM_IIP_PARAM_AFN_VAL_1_0) / ((dst) -1)))
#define M_BF_STILL_AFN_CONV_TBL_VAL( pixs )					((LONG)(((INT64)D_IM_IIP_PARAM_AFN_VAL_1_0) * (pixs)))
#define M_BF_STILL_AFN_CONV_FLORT_VAL( pixs )				(((DOUBLE)(pixs)) / D_IM_IIP_PARAM_AFN_VAL_1_0)


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

/*
 E_BF_STILL_PLANE_TYPE_PLANAR	E_BF_STILL_PLANE_TYPE_LINEAR_PLANAR		E_BF_STILL_PLANE_TYPE_C_LINEAR
 --------------                 --------------------                    ---------------
 |     Y      |                 |   Y    | Cb | Cr |                    |      Y      |
 --------------                 --------------------                    ---------------
 -------                                                                ---------------
 | Cb  |                                                                |  Cb   |  Cr |
 -------                                                                ---------------
 -------
 | Cr  |
 -------
*/

/*
 E_BF_STILL_PLANE_TYPE_C_INTL
 ---------------
 |      Y      |
 ---------------
 ---------------
 |    CbCr     |
 ---------------
*/

/*
 E_BF_STILL_PLANE_TYPE_LINEAR_C_INTL
 -----------------------------
 |      Y      |     CbCr    |
 -----------------------------
*/

typedef enum {
	E_BF_STILL_PLANE_TYPE_PLANAR = 0,
	E_BF_STILL_PLANE_TYPE_LINEAR_PLANAR,
	E_BF_STILL_PLANE_TYPE_VIDEO,
	E_BF_STILL_PLANE_TYPE_TILE,				// Raster format for GPU
	E_BF_STILL_PLANE_TYPE_C_LINEAR,
	E_BF_STILL_PLANE_TYPE_C_INTL,			// Y Planar and CbCr Interleaved
	E_BF_STILL_PLANE_TYPE_LINEAR_C_INTL,	// Y Planar and CbCr Interleaved
	E_BF_STILL_PLANE_TYPE_1PLANE,			// Y or Cb or Cr (Using "Y"'s parameter)
	E_BF_STILL_PLANE_TYPE_CHUNKY,			// Chunkey format.
	E_BF_STILL_PLANE_TYPE_MAX,
} E_BF_STILL_PLANE_TYPE;

typedef enum {
	E_BF_STILL_IMG_TYPE_YCC420 = 0,
	E_BF_STILL_IMG_TYPE_YCC422,
	E_BF_STILL_IMG_TYPE_YCC444,
	E_BF_STILL_IMG_TYPE_RGB888,
	E_BF_STILL_IMG_TYPE_RGB444,
	E_BF_STILL_IMG_TYPE_VIDEO,
	E_BF_STILL_IMG_TYPE_YCC400,
	E_BF_STILL_IMG_TYPE_MAX,
} E_BF_STILL_IMG_TYPE;

typedef enum {
	E_BF_STILL_IMG_DEPTH_U8 = 0,
	E_BF_STILL_IMG_DEPTH_P10,
	E_BF_STILL_IMG_DEPTH_U12,
	E_BF_STILL_IMG_DEPTH_U16,
	E_BF_STILL_IMG_DEPTH_MAX,
} E_BF_STILL_IMG_DEPTH;


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

typedef struct {
	LONG	sta_x;
	LONG	sta_y;
	LONG	width;
	LONG	lines;
} T_BF_STILL_IMG_WINDOW_L;

typedef struct {
	DOUBLE	sta_x;
	DOUBLE	sta_y;
	DOUBLE	width;
	DOUBLE	lines;
} T_BF_STILL_IMG_WINDOW_F;

typedef struct {
	UINT32	y;
	UINT32	cb;
	UINT32	cr;
} T_BF_STILL_IMG_GWIDTH;

typedef struct {
	UINT32	y_g;
	UINT32	cb_b;
	UINT32	cr_r;
	UINT32	alpha;
} T_BF_STILL_IMG_GWIDTH_CONV;

typedef struct {
	UINT32	y_g;
	UINT32	cb_b;
	UINT32	cr_r;
	UINT32	alpha;
} T_BF_STILL_IMG_LINE_BYTE;

typedef struct {
	E_BF_STILL_IMG_TYPE		type;
	E_BF_STILL_PLANE_TYPE	plane;
	E_BF_STILL_IMG_DEPTH	depth;
	T_IMAGE_ADDR_YCC		addr;
	T_IMAGE_SIZE			pixs;
	T_BF_STILL_IMG_GWIDTH	g_width;
} T_BF_STILL_IMG;

typedef struct {
	E_BF_STILL_IMG_DEPTH	depth;
	T_IMAGE_ADDR_RGB		addr;
	T_IMAGE_SIZE			pixs;
	UINT32					g_width;
} T_BF_STILL_IMG_RGB;

typedef struct {
	E_BF_STILL_IMG_TYPE			type;
	E_BF_STILL_PLANE_TYPE		plane;
	E_BF_STILL_IMG_DEPTH		depth;
	T_IMAGE_ADDR_RGBA			addr;
	T_IMAGE_SIZE				pixs;
	T_BF_STILL_IMG_GWIDTH_CONV	g_width;
	T_BF_STILL_IMG_LINE_BYTE	line_bytes;
} T_BF_STILL_IMG_CONV;

typedef struct {
	DOUBLE	sta_x;
	DOUBLE	sta_y;
	DOUBLE	width;
	DOUBLE	lines;
} T_BF_STILL_IMG_CROP;

/*==============================================================================*/
/* Global variable declaration													*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Function declaration															*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


extern USHORT	BF_Still_FjImgInfo_Calc_C_Lines( const FJ_IMG_INFO* const fj_img_info );

extern USHORT	BF_Still_ImgInfo_Calc_Cb_Lines( const T_BF_STILL_IMG* const img_info );
extern USHORT	BF_Still_ImgInfo_Calc_Cb_Width( const T_BF_STILL_IMG* const img_info );
extern INT32	BF_Still_ImgInfo_Set_IipPixFmtTbl( T_IM_IIP_PIXFMTTBL* const pixfmt_tbl, const T_BF_STILL_IMG* const img_info, const ULONG master_if );
extern INT32	BF_Still_ImgInfo_Set_IipPixFmtTblRgb( T_IM_IIP_PIXFMTTBL* const pixfmt_tbl, const T_BF_STILL_IMG_CONV* const img_info, const ULONG master_if );
extern INT32	BF_Still_ImgInfo_Set_IipPixFmtTblCbCr( T_IM_IIP_PIXFMTTBL* const pixfmt_tbl, const T_BF_STILL_IMG* const img_info, const ULONG master_if );
extern INT32	BF_Still_ImgInfo_Calc_ResizeRectAfnTbl( T_IM_IIP_PARAM_AFN_OPCOL_0* const afn_tbl, const T_BF_STILL_IMG_WINDOW_L* const crop_rect, const T_BF_STILL_IMG_CONV* const dst_img, const T_BF_STILL_IMG_CONV* const src_img, const BOOL nearest_neighbor );
extern INT32	BF_Still_ImgInfo_Calc_ResizeRectAfnTbl_CbCrOnly( T_IM_IIP_PARAM_AFN_OPCOL_0* const afn_tbl, const T_BF_STILL_IMG_WINDOW_L* const crop_rect, const T_BF_STILL_IMG* const dst_img, const T_BF_STILL_IMG* const src_img );

extern VOID		BF_Still_ImgInfo_Dump( const char* const header_str, const T_BF_STILL_IMG* const img );

extern VOID		BF_Still_ImgInfo_To_ImgConv( T_BF_STILL_IMG_CONV* const img_conv, const T_BF_STILL_IMG* const img );


#ifdef __cplusplus
}
#endif

#endif // __STILL_IMG_INFO_H__

