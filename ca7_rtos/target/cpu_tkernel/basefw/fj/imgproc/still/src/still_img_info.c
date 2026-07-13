/**
 * @file		still_img_info.c
 * @brief		Image data information structure
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// Still
#include "still_img_info.h"
#include "still_common.h"
#include "still_dcache.h"

// FileSystem
#include "fs_if.h"

// Map
#include "sdram_map_common.h"
#include "sdram_map_cache_define.h"

// Debug
#include "debug.h"

// DDIM
#include "ddim_typedef.h"
#include "image_info.h"
#include "im_iip.h"
#include "im_iip_param.h"

// OS
#include "os_user_custom.h"

// STD-C
#include <string.h>


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Macro declaration															*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

USHORT BF_Still_FjImgInfo_Calc_C_Lines( const FJ_IMG_INFO* const fj_img_info )
{
	switch( fj_img_info->pfmt ) {
		case FJ_IMG_PFMT_YCC420:
			return fj_img_info->lines /2;
		case FJ_IMG_PFMT_YCC422:			// FALLTHROUGH
		case FJ_IMG_PFMT_YCC444:
			return fj_img_info->lines;
		case FJ_IMG_PFMT_VIDEO:
			return M_BF_STILL_ROUNDUP_VIDEOFMT_LINES( fj_img_info->lines /2 );
		default:
			return 0;
	}
}


USHORT BF_Still_ImgInfo_Calc_Cb_Lines( const T_BF_STILL_IMG* const img_info )
{
	switch( img_info->type ) {
		case E_BF_STILL_IMG_TYPE_YCC420:			// FALLTHROUGH
			return img_info->pixs.lines /2;
		case E_BF_STILL_IMG_TYPE_YCC422:			// FALLTHROUGH
		case E_BF_STILL_IMG_TYPE_YCC444:			// FALLTHROUGH
			return img_info->pixs.lines;
		default:
			return 0;
	}
}


USHORT BF_Still_ImgInfo_Calc_Cb_Width( const T_BF_STILL_IMG* const img_info )
{
	switch( img_info->type ) {
		case E_BF_STILL_IMG_TYPE_YCC420:			// FALLTHROUGH
		case E_BF_STILL_IMG_TYPE_YCC422:			// FALLTHROUGH
			return img_info->pixs.width /2;
		case E_BF_STILL_IMG_TYPE_YCC444:
			return img_info->pixs.width;
		default:
			return 0;
	}
}


INT32 BF_Still_ImgInfo_Set_IipPixFmtTbl( T_IM_IIP_PIXFMTTBL* const pixfmt_tbl, const T_BF_STILL_IMG* const img_info, const ULONG master_if )
{
	switch( img_info->depth ) {
		case E_BF_STILL_IMG_DEPTH_U8:
			pixfmt_tbl->pix_depth		= D_IM_IIP_PDEPTH_8BITS;
			pixfmt_tbl->alpha_depth		= E_IM_IIP_ADEPTH_8BITS;	// dummy
			break;
		case E_BF_STILL_IMG_DEPTH_U12:
			pixfmt_tbl->pix_depth		= D_IM_IIP_PDEPTH_12BITS;
			pixfmt_tbl->alpha_depth		= E_IM_IIP_ADEPTH_8BITS;	// dummy
			break;
		case E_BF_STILL_IMG_DEPTH_U16:
			pixfmt_tbl->pix_depth		= D_IM_IIP_PDEPTH_16BITS;
			pixfmt_tbl->alpha_depth		= E_IM_IIP_ADEPTH_8BITS;	// dummy
			break;
		default:
			BF_Debug_Print_Error(( "BF_Still_ImgInfo_Set_IipPixFmtTbl() err 1 (depth=%u)\n", img_info->depth ));
			return D_OS_USER_E_PAR;
	}
	pixfmt_tbl->alpha					= D_IM_IIP_ALPHA_FALSE;
	switch( img_info->type ) {
		case E_BF_STILL_IMG_TYPE_YCC420:
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_YCC420;
			break;
		case E_BF_STILL_IMG_TYPE_YCC422:
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_YCC422;
			break;
		case E_BF_STILL_IMG_TYPE_YCC400:
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_PLANE;
			break;
		case E_BF_STILL_IMG_TYPE_RGB888:			// FALLTHROUGH
		case E_BF_STILL_IMG_TYPE_YCC444:
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_YCC444;
			break;
		case E_BF_STILL_IMG_TYPE_VIDEO:
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_VIDEO;
			break;
		default:
			BF_Debug_Print_Error(( "BF_Still_ImgInfo_Set_IipPixFmtTbl() err 2 (type=%u)\n", img_info->type ));
			return D_OS_USER_E_PAR;
	}
	switch( img_info->plane ) {
		case E_BF_STILL_PLANE_TYPE_C_INTL:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_LINEAR_C_INTL:
			pixfmt_tbl->frame_type		= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
			break;
		case E_BF_STILL_PLANE_TYPE_PLANAR:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_LINEAR_PLANAR:	// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_C_LINEAR:
			pixfmt_tbl->frame_type		= E_IM_IIP_FTYPE_PLANE;
			break;
		case E_BF_STILL_PLANE_TYPE_1PLANE:
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_PLANE;
			pixfmt_tbl->frame_type		= E_IM_IIP_FTYPE_PLANE;
			break;
		case E_BF_STILL_PLANE_TYPE_VIDEO:
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_VIDEO;
			pixfmt_tbl->frame_type		= E_IM_IIP_FTYPE_PLANE;		// dummy
			break;
		case E_BF_STILL_PLANE_TYPE_CHUNKY:
			pixfmt_tbl->frame_type		= E_IM_IIP_FTYPE_CHUNKY;
			break;
//		case E_BF_STILL_PLANE_TYPE_TILE:
		default:
			BF_Debug_Print_Error(( "BF_Still_ImgInfo_Set_IipPixFmtTbl() err 3 (plane=%u)\n", img_info->plane ));
			return D_OS_USER_E_PAR;
	}
	pixfmt_tbl->chunky_color.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G;
	pixfmt_tbl->chunky_color.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B;
	pixfmt_tbl->chunky_color.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R;
	pixfmt_tbl->chunky_color.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A;
	pixfmt_tbl->sign_Y_G				= D_IM_IIP_UNSIGNED_DATA;
	pixfmt_tbl->sign_Cb_B				= D_IM_IIP_UNSIGNED_DATA;
	pixfmt_tbl->sign_Cr_R				= D_IM_IIP_UNSIGNED_DATA;
	pixfmt_tbl->sign_D3					= D_IM_IIP_UNSIGNED_DATA;
	pixfmt_tbl->masterIF_Y_G			= master_if;
	pixfmt_tbl->masterIF_Cb_B			= master_if;
	pixfmt_tbl->masterIF_Cr_R			= master_if;
	pixfmt_tbl->masterIF_Alpha			= master_if;
	pixfmt_tbl->alpha_subsampling		= D_IM_IIP_ALPHA_NO_SUBSAMP;
	pixfmt_tbl->pix_outside_img			= D_IM_IIP_OUTSIDE_IMG_EDGE;
	pixfmt_tbl->Y_G_val_outside_img		= 0;						// dummy
	pixfmt_tbl->Cb_B_val_outside_img	= 0;						// dummy
	pixfmt_tbl->Cr_R_val_outside_img	= 0;						// dummy
	pixfmt_tbl->Alpha_val_outside_img	= 0;						// dummy
	pixfmt_tbl->width					= img_info->pixs.width;
	pixfmt_tbl->lines					= img_info->pixs.lines;
	pixfmt_tbl->line_bytes.Y_G			= img_info->g_width.y;
	pixfmt_tbl->line_bytes.Cb_B			= img_info->g_width.cb;
	pixfmt_tbl->line_bytes.Cr_R			= img_info->g_width.cr;
	pixfmt_tbl->line_bytes.Alpha		= img_info->g_width.y;		// dummy
	pixfmt_tbl->addr.Y_G				= M_SDRAM_MAP_COMMON_ADR_LOG2PHY(img_info->addr.y);
	pixfmt_tbl->addr.Cb_B				= M_SDRAM_MAP_COMMON_ADR_LOG2PHY(img_info->addr.cb);
	pixfmt_tbl->addr.Cr_R				= M_SDRAM_MAP_COMMON_ADR_LOG2PHY(img_info->addr.cr);
	pixfmt_tbl->addr.Alpha				= M_SDRAM_MAP_COMMON_ADR_LOG2PHY(img_info->addr.y);			// dummy

	return D_OS_USER_E_OK;
}

INT32 BF_Still_ImgInfo_Set_IipPixFmtTblRgb( T_IM_IIP_PIXFMTTBL* const pixfmt_tbl, const T_BF_STILL_IMG_CONV* const img_info, const ULONG master_if )
{
	switch ( img_info->depth ) {
		case E_BF_STILL_IMG_DEPTH_U8:
			pixfmt_tbl->pix_depth		= D_IM_IIP_PDEPTH_8BITS;
			pixfmt_tbl->alpha_depth		= E_IM_IIP_ADEPTH_8BITS;	// dummy
			break;
		case E_BF_STILL_IMG_DEPTH_U12:
			pixfmt_tbl->pix_depth		= D_IM_IIP_PDEPTH_12BITS;
			pixfmt_tbl->alpha_depth		= E_IM_IIP_ADEPTH_8BITS;	// dummy
			break;
		case E_BF_STILL_IMG_DEPTH_U16:
			pixfmt_tbl->pix_depth		= D_IM_IIP_PDEPTH_16BITS;
			pixfmt_tbl->alpha_depth		= E_IM_IIP_ADEPTH_8BITS;	// dummy
			break;
		default:
			BF_Debug_Print_Error(( "BF_Still_ImgInfo_Set_IipPixFmtTblRgb() err 1 (depth=%u)\n", img_info->depth ));
			return D_OS_USER_E_PAR;
	}

	pixfmt_tbl->alpha = D_IM_IIP_ALPHA_FALSE;

	switch ( img_info->type ) {
		case E_BF_STILL_IMG_TYPE_YCC420:
			if ( img_info->addr.alpha != 0 ) {
				pixfmt_tbl->alpha		= D_IM_IIP_ALPHA_TRUE;
			}
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_YCC420;
			break;
		case E_BF_STILL_IMG_TYPE_YCC422:
			if ( img_info->addr.alpha != 0 ) {
				pixfmt_tbl->alpha		= D_IM_IIP_ALPHA_TRUE;
			}
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_YCC422;
			break;
		case E_BF_STILL_IMG_TYPE_YCC444:
			if ( img_info->addr.alpha != 0 ) {
				pixfmt_tbl->alpha		= D_IM_IIP_ALPHA_TRUE;
			}
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_YCC444;
			break;
		case E_BF_STILL_IMG_TYPE_VIDEO:
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_VIDEO;
			break;
		case E_BF_STILL_IMG_TYPE_RGB888:
			pixfmt_tbl->alpha			= D_IM_IIP_ALPHA_TRUE;
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_YCC444;
			break;
		case E_BF_STILL_IMG_TYPE_RGB444:
			pixfmt_tbl->alpha			= D_IM_IIP_ALPHA_TRUE;
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_RGBA4444;
			break;
		default:
			BF_Debug_Print_Error(( "BF_Still_ImgInfo_Set_IipPixFmtTblRgb() err 2 (type=%u)\n", img_info->type ));
			return D_OS_USER_E_PAR;
	}

	switch ( img_info->plane ) {
		case E_BF_STILL_PLANE_TYPE_C_INTL:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_LINEAR_C_INTL:
			pixfmt_tbl->frame_type		= E_IM_IIP_FTYPE_Y_PACKED_CBCR;
			break;
		case E_BF_STILL_PLANE_TYPE_PLANAR:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_LINEAR_PLANAR:	// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_C_LINEAR:
			pixfmt_tbl->frame_type		= E_IM_IIP_FTYPE_PLANE;
			break;
		case E_BF_STILL_PLANE_TYPE_1PLANE:
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_PLANE;
			pixfmt_tbl->frame_type		= E_IM_IIP_FTYPE_PLANE;
			break;
		case E_BF_STILL_PLANE_TYPE_VIDEO:
			pixfmt_tbl->pix_format		= E_IM_IIP_PFMT_VIDEO;
			pixfmt_tbl->frame_type		= E_IM_IIP_FTYPE_PLANE;		// dummy
			break;
		case E_BF_STILL_PLANE_TYPE_CHUNKY:
			pixfmt_tbl->frame_type		= E_IM_IIP_FTYPE_CHUNKY;
			break;
		default:
			BF_Debug_Print_Error(( "BF_Still_ImgInfo_Set_IipPixFmtTblRgb() err 3 (plane=%u)\n", img_info->plane ));
			return D_OS_USER_E_PAR;
	}

	pixfmt_tbl->chunky_color.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G;
	pixfmt_tbl->chunky_color.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B;
	pixfmt_tbl->chunky_color.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R;
	pixfmt_tbl->chunky_color.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A;
	pixfmt_tbl->sign_Y_G				= D_IM_IIP_UNSIGNED_DATA;
	pixfmt_tbl->sign_Cb_B				= D_IM_IIP_UNSIGNED_DATA;
	pixfmt_tbl->sign_Cr_R				= D_IM_IIP_UNSIGNED_DATA;
	pixfmt_tbl->sign_D3					= D_IM_IIP_UNSIGNED_DATA;
	pixfmt_tbl->masterIF_Y_G			= master_if;
	pixfmt_tbl->masterIF_Cb_B			= master_if;
	pixfmt_tbl->masterIF_Cr_R			= master_if;
	pixfmt_tbl->masterIF_Alpha			= master_if;
	pixfmt_tbl->alpha_subsampling		= D_IM_IIP_ALPHA_NO_SUBSAMP;
	pixfmt_tbl->pix_outside_img			= D_IM_IIP_OUTSIDE_IMG_EDGE;
	pixfmt_tbl->Y_G_val_outside_img		= 0;						// dummy
	pixfmt_tbl->Cb_B_val_outside_img	= 0;						// dummy
	pixfmt_tbl->Cr_R_val_outside_img	= 0;						// dummy
	pixfmt_tbl->Alpha_val_outside_img	= 0;						// dummy
	pixfmt_tbl->width					= img_info->pixs.width;
	pixfmt_tbl->lines					= img_info->pixs.lines;
	pixfmt_tbl->line_bytes.Y_G			= img_info->g_width.y_g;
	pixfmt_tbl->line_bytes.Cb_B			= img_info->g_width.cb_b;
	pixfmt_tbl->line_bytes.Cr_R			= img_info->g_width.cr_r;
	pixfmt_tbl->line_bytes.Alpha		= img_info->g_width.alpha;
	pixfmt_tbl->addr.Y_G				= M_SDRAM_MAP_COMMON_ADR_LOG2PHY(img_info->addr.y_g);
	pixfmt_tbl->addr.Cb_B				= M_SDRAM_MAP_COMMON_ADR_LOG2PHY(img_info->addr.cb_b);
	pixfmt_tbl->addr.Cr_R				= M_SDRAM_MAP_COMMON_ADR_LOG2PHY(img_info->addr.cr_r);
	pixfmt_tbl->addr.Alpha				= M_SDRAM_MAP_COMMON_ADR_LOG2PHY(img_info->addr.alpha);

	return D_OS_USER_E_OK;
}

INT32 BF_Still_ImgInfo_Set_IipPixFmtTblCbCr( T_IM_IIP_PIXFMTTBL* const pixfmt_tbl, const T_BF_STILL_IMG* const img_info, const ULONG master_if )
{
	switch( img_info->depth ) {
		case E_BF_STILL_IMG_DEPTH_U8:
			pixfmt_tbl->pix_depth		= D_IM_IIP_PDEPTH_8BITS;
			pixfmt_tbl->alpha_depth		= E_IM_IIP_ADEPTH_8BITS;	// dummy
			break;
		case E_BF_STILL_IMG_DEPTH_U12:
			pixfmt_tbl->pix_depth		= D_IM_IIP_PDEPTH_12BITS;
			pixfmt_tbl->alpha_depth		= E_IM_IIP_ADEPTH_8BITS;	// dummy
			break;
		case E_BF_STILL_IMG_DEPTH_U16:
			pixfmt_tbl->pix_depth		= D_IM_IIP_PDEPTH_16BITS;
			pixfmt_tbl->alpha_depth		= E_IM_IIP_ADEPTH_8BITS;	// dummy
			break;
		default:
			BF_Debug_Print_Error(( "BF_Still_ImgInfo_Set_IipPixFmtTblCbCr() err 1 (depth=%u)\n", img_info->depth ));
			return D_OS_USER_E_PAR;
	}
	pixfmt_tbl->alpha					= D_IM_IIP_ALPHA_FALSE;
	switch( img_info->plane ) {
		case E_BF_STILL_PLANE_TYPE_C_INTL:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_LINEAR_C_INTL:
			pixfmt_tbl->frame_type		= E_IM_IIP_FTYPE_CHUNKY;
			break;
		case E_BF_STILL_PLANE_TYPE_PLANAR:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_LINEAR_PLANAR:	// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_C_LINEAR:
			pixfmt_tbl->frame_type		= E_IM_IIP_FTYPE_PLANE;
			break;
		case E_BF_STILL_PLANE_TYPE_1PLANE:
			pixfmt_tbl->frame_type		= E_IM_IIP_FTYPE_PLANE;
			break;
		case E_BF_STILL_PLANE_TYPE_VIDEO:
			pixfmt_tbl->frame_type		= E_IM_IIP_FTYPE_PLANE;		// dummy
			break;
//		case E_BF_STILL_PLANE_TYPE_TILE:
		default:
			BF_Debug_Print_Error(( "BF_Still_ImgInfo_Set_IipPixFmtTblCbCr() err 2 (plane=%u)\n", img_info->plane ));
			return D_OS_USER_E_PAR;
	}
	pixfmt_tbl->pix_format				= E_IM_IIP_PFMT_CBCR;
	pixfmt_tbl->chunky_color.component0 = D_IM_IIP_CHUNKY_COLOR_CB_B;	// Valid when frame_type=E_IM_IIP_FTYPE_CHUNKY.
	pixfmt_tbl->chunky_color.component1 = D_IM_IIP_CHUNKY_COLOR_CR_R;	// Valid when frame_type=E_IM_IIP_FTYPE_CHUNKY.
	pixfmt_tbl->chunky_color.component2 = 0;						// dummy
	pixfmt_tbl->chunky_color.component3 = 0;						// dummy
	pixfmt_tbl->sign_Y_G				= D_IM_IIP_UNSIGNED_DATA;
	pixfmt_tbl->sign_Cb_B				= D_IM_IIP_UNSIGNED_DATA;
	pixfmt_tbl->sign_Cr_R				= D_IM_IIP_UNSIGNED_DATA;
	pixfmt_tbl->sign_D3					= D_IM_IIP_UNSIGNED_DATA;
	pixfmt_tbl->masterIF_Y_G			= master_if;
	pixfmt_tbl->masterIF_Cb_B			= master_if;
	pixfmt_tbl->masterIF_Cr_R			= master_if;
	pixfmt_tbl->masterIF_Alpha			= master_if;
	pixfmt_tbl->alpha_subsampling		= D_IM_IIP_ALPHA_NO_SUBSAMP;
	pixfmt_tbl->pix_outside_img			= D_IM_IIP_OUTSIDE_IMG_EDGE;
	pixfmt_tbl->Y_G_val_outside_img		= 0;						// dummy
	pixfmt_tbl->Cb_B_val_outside_img	= 0;						// dummy
	pixfmt_tbl->Cr_R_val_outside_img	= 0;						// dummy
	pixfmt_tbl->Alpha_val_outside_img	= 0;						// dummy
	pixfmt_tbl->width					= BF_Still_ImgInfo_Calc_Cb_Width( img_info );
	pixfmt_tbl->lines					= BF_Still_ImgInfo_Calc_Cb_Lines( img_info );
	pixfmt_tbl->line_bytes.Y_G			= 0;						// dummy
	pixfmt_tbl->line_bytes.Cb_B			= img_info->g_width.cb;
	pixfmt_tbl->line_bytes.Cr_R			= img_info->g_width.cr;
	pixfmt_tbl->line_bytes.Alpha		= 0;						// dummy
	pixfmt_tbl->addr.Y_G				= 0;						// dummy
	pixfmt_tbl->addr.Cb_B				= M_SDRAM_MAP_COMMON_ADR_LOG2PHY(img_info->addr.cb);
	pixfmt_tbl->addr.Cr_R				= M_SDRAM_MAP_COMMON_ADR_LOG2PHY(img_info->addr.cr);
	pixfmt_tbl->addr.Alpha				= 0;						// dummy

	return D_OS_USER_E_OK;
}


// Resize and change aspect ratio
INT32 BF_Still_ImgInfo_Calc_ResizeRectAfnTbl( T_IM_IIP_PARAM_AFN_OPCOL_0* const afn_tbl, const T_BF_STILL_IMG_WINDOW_L* const crop_rect, const T_BF_STILL_IMG_CONV* const dst_img, const T_BF_STILL_IMG_CONV* const src_img, const BOOL nearest_neighbor )
{
	LONG		pit_x;
	LONG		pit_y;
	LONG		ref_x;
	LONG		ref_y;
	LONG		sta_x;
	LONG		sta_y;
	const CHAR	errmsg[] = "%s: errno=%u\n";

	if( (((UINT32)afn_tbl) % D_BF_STILL_AFNTBL_ALIGN) != 0 ) {
		BF_Debug_Print_Error(( errmsg, __func__, 1 ));
		return D_OS_USER_E_PAR;
	}

	if ( crop_rect->sta_x >= 0 ) {
		if( (crop_rect->sta_x + crop_rect->width) > M_BF_STILL_AFN_CONV_TBL_VAL( src_img->pixs.width ) ) {
			BF_Debug_Print_Error(( errmsg, __func__, 2 ));
			return D_OS_USER_E_PAR;
		}
	}

	if ( crop_rect->sta_y >= 0 ) {
		if( (crop_rect->sta_y + crop_rect->lines) > M_BF_STILL_AFN_CONV_TBL_VAL( src_img->pixs.lines ) ) {
			BF_Debug_Print_Error(( errmsg, __func__, 3 ));
			return D_OS_USER_E_PAR;
		}
	}

	if( nearest_neighbor == FALSE ) {
		// BI-Linear
		pit_x = M_BF_STILL_AFN_CALC_PIT_WINL( dst_img->pixs.width, crop_rect->width );
		pit_y = M_BF_STILL_AFN_CALC_PIT_WINL( dst_img->pixs.lines, crop_rect->lines );
	}
	else {
		// Nearest Neighbor
		pit_x = M_BF_STILL_AFN_CALC_PIT_WINL_NEAREST( dst_img->pixs.width, crop_rect->width );
		pit_y = M_BF_STILL_AFN_CALC_PIT_WINL_NEAREST( dst_img->pixs.lines, crop_rect->lines );
	}
	ref_x = pit_x * (dst_img->pixs.width -1);
	ref_y = pit_y * (dst_img->pixs.lines -1);
	sta_x = ((crop_rect->width -D_IM_IIP_PARAM_AFN_VAL_1_0) - ref_x) /2 + crop_rect->sta_x;
	sta_y = ((crop_rect->lines -D_IM_IIP_PARAM_AFN_VAL_1_0) - ref_y) /2 + crop_rect->sta_y;

	memset( afn_tbl, '\0', sizeof(*afn_tbl) );
	afn_tbl->STAXY0.bit.STAX = sta_x;
	afn_tbl->STAXY0.bit.STAY = sta_y;
	afn_tbl->D_OUT_IN_0.bit.DXX = pit_x;	// X increment X
	afn_tbl->D_OUT_IN_0.bit.DXY = 0x0000;	// X increment Y=0.0
	afn_tbl->D_OUT_IN_0.bit.DYX = 0x0000;	// Y increment X=0.0
	afn_tbl->D_OUT_IN_0.bit.DYY = pit_y;	// Y increment Y
	afn_tbl->SZ.bit.AFNDVSZ = dst_img->pixs.lines;
	afn_tbl->SZ.bit.AFNDHSZ = dst_img->pixs.width;

	return D_OS_USER_E_OK;
}

// Resize and change aspect ratio
INT32 BF_Still_ImgInfo_Calc_ResizeRectAfnTbl_CbCrOnly( T_IM_IIP_PARAM_AFN_OPCOL_0* const afn_tbl, const T_BF_STILL_IMG_WINDOW_L* const crop_rect, const T_BF_STILL_IMG* const dst_img, const T_BF_STILL_IMG* const src_img )
{
	INT32 ercd;
	UINT32 dst_reduce_x_ratio;
	UINT32 dst_reduce_y_ratio;
	T_BF_STILL_IMG_CONV src_img_conv;
	T_BF_STILL_IMG_CONV dst_img_conv;
	static const CHAR errmsg[] = "%s: format not supported. %u.\n";

	BF_Still_ImgInfo_To_ImgConv( &src_img_conv, src_img );
	BF_Still_ImgInfo_To_ImgConv( &dst_img_conv, dst_img );

	// Set YCC's parameter
	ercd = BF_Still_ImgInfo_Calc_ResizeRectAfnTbl( afn_tbl, crop_rect, &dst_img_conv, &src_img_conv, FALSE );
	if( ercd != D_OS_USER_E_OK ) {
		return ercd;
	}

	// Calculate ractangle ratio from Y to CbCr.
	switch( dst_img->type ) {
		case E_BF_STILL_IMG_TYPE_YCC422:
			dst_reduce_x_ratio = 2;
			dst_reduce_y_ratio = 1;
			break;
		case E_BF_STILL_IMG_TYPE_YCC420:
			dst_reduce_x_ratio = 2;
			dst_reduce_y_ratio = 2;
			break;
		case E_BF_STILL_IMG_TYPE_YCC444:
			dst_reduce_x_ratio = 1;
			dst_reduce_y_ratio = 1;
			break;
		default:
			BF_Debug_Print_Error(( errmsg, __func__, 1 ));
			return D_OS_USER_E_PAR;
	}

	// Tuning YCC's parameters to CbCr Only parameters.
	afn_tbl->STAXY0.bit.STAX /= dst_reduce_x_ratio;
	afn_tbl->STAXY0.bit.STAY /= dst_reduce_y_ratio;
	afn_tbl->SZ.bit.AFNDHSZ = BF_Still_ImgInfo_Calc_Cb_Width( dst_img );
	afn_tbl->SZ.bit.AFNDVSZ = BF_Still_ImgInfo_Calc_Cb_Lines( dst_img );

	return ercd;
}


VOID BF_Still_ImgInfo_Dump( const char* const header_str, const T_BF_STILL_IMG* const img )
{
	UINT32				g_width_pixs_y;
	UINT32				width_pixs_sum;
	UINT32				lines_pixs_sum;
	UINT32				lines_pixs_sum_planar;
	const CHAR*			depth_str;
	const CHAR*			type_str;
	const CHAR*			plane_str;
	const CHAR			unknown_str[] = "UNKNOWN";
	const CHAR* const	errmsg = "BF_Still_ImgInfo_Dump() error %u\n";

	switch( img->depth ) {
		case E_BF_STILL_IMG_DEPTH_U8:
			g_width_pixs_y = img->g_width.y;
			depth_str = "U8";
			break;
		case E_BF_STILL_IMG_DEPTH_U12:
			g_width_pixs_y = img->g_width.y /3 *2;
			depth_str = "U12";
			break;
		case E_BF_STILL_IMG_DEPTH_U16:
			g_width_pixs_y = img->g_width.y /2;
			depth_str = "U16";
			break;
		default:
			BF_Debug_Print_Warning(( errmsg, 1 ));
			g_width_pixs_y = img->g_width.y;	// fail safe
			depth_str = unknown_str;
			break;
	}

	switch( img->type ) {
		case E_BF_STILL_IMG_TYPE_YCC420:
			lines_pixs_sum_planar = img->pixs.lines *3 /2;	// x1.5
			type_str = "YCC420";
			break;
		case E_BF_STILL_IMG_TYPE_YCC422:
			lines_pixs_sum_planar = img->pixs.lines *2;	// x2.0
			type_str = "YCC422";
			break;
		case E_BF_STILL_IMG_TYPE_YCC444:
			lines_pixs_sum_planar = img->pixs.lines *3;	// x3.0
			type_str = "YCC444";
			break;
		case E_BF_STILL_IMG_TYPE_VIDEO:
			lines_pixs_sum_planar = img->pixs.lines *3 /2;	// x1.5
			type_str = "VIDEO";
			break;
		default:
			BF_Debug_Print_Warning(( errmsg, 2 ));
			lines_pixs_sum_planar = img->pixs.lines *2;	// fail safe
			type_str = unknown_str;
			break;
	}

	switch( img->plane ) {
		case E_BF_STILL_PLANE_TYPE_TILE:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_LINEAR_C_INTL:	// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_LINEAR_PLANAR:
			width_pixs_sum = g_width_pixs_y;
			lines_pixs_sum = img->pixs.lines;
			break;
		case E_BF_STILL_PLANE_TYPE_1PLANE:
			width_pixs_sum = g_width_pixs_y;
			lines_pixs_sum = img->pixs.lines;
			break;
		case E_BF_STILL_PLANE_TYPE_PLANAR:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_C_LINEAR:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_VIDEO:			// FALLTHROUGH
		case E_BF_STILL_PLANE_TYPE_C_INTL:
			width_pixs_sum = g_width_pixs_y;
			lines_pixs_sum = lines_pixs_sum_planar;
			break;
		default:
			BF_Debug_Print_Warning(( errmsg, 3 ));
			width_pixs_sum = g_width_pixs_y;		// fail safe
			lines_pixs_sum = img->pixs.lines;		// fail safe
			break;
	}

	switch( img->plane ) {
		case E_BF_STILL_PLANE_TYPE_TILE:
			plane_str = "TILE";
			break;
		case E_BF_STILL_PLANE_TYPE_LINEAR_C_INTL:
			plane_str = "LINEAR_PACKED_CbCr";
			break;
		case E_BF_STILL_PLANE_TYPE_LINEAR_PLANAR:
			plane_str = "LINEAR";
			break;
		case E_BF_STILL_PLANE_TYPE_1PLANE:
			plane_str = "1_PLANE";
			break;
		case E_BF_STILL_PLANE_TYPE_PLANAR:
			plane_str = "PLANAR";
			break;
		case E_BF_STILL_PLANE_TYPE_C_LINEAR:
			plane_str = "LINEAR CbCr";
			break;
		case E_BF_STILL_PLANE_TYPE_VIDEO:
			plane_str = "VIDEO";
			break;
		case E_BF_STILL_PLANE_TYPE_C_INTL:
			plane_str = "PACKED_CbCr";
			break;
		default:
			BF_Debug_Print_Warning(( errmsg, 4 ));
			plane_str = unknown_str;
			break;
	}

	BF_Debug_Print_Information(( "%s: %ux%u(%ux%u) gw_y=%u gw_cb=%u gw_cr=%u type=%s(%u) plane=%s(%u) depth=%s(%u) y=0x%lx cb=0x%lx cr=0x%lx\n",
			header_str,
			img->pixs.width,
			img->pixs.lines,
			width_pixs_sum,
			lines_pixs_sum,
			img->g_width.y,
			img->g_width.cb,
			img->g_width.cr,
			type_str,
			img->type,
			plane_str,
			img->plane,
			depth_str,
			img->depth,
			img->addr.y,
			img->addr.cb,
			img->addr.cr
			));
}


VOID BF_Still_ImgInfo_To_ImgConv( T_BF_STILL_IMG_CONV* const img_conv, const T_BF_STILL_IMG* const img )
{
	img_conv->type				= img->type;
	img_conv->plane				= img->plane;
	img_conv->depth				= img->depth;
	img_conv->addr.y_g			= img->addr.y;
	img_conv->addr.cb_b			= img->addr.cb;
	img_conv->addr.cr_r			= img->addr.cr;
	img_conv->addr.alpha		= 0;				// dummy
	img_conv->pixs				= img->pixs;
	img_conv->g_width.y_g		= img->g_width.y;
	img_conv->g_width.cb_b		= img->g_width.cb;
	img_conv->g_width.cr_r		= img->g_width.cr;
	img_conv->g_width.alpha		= img->g_width.y;	// dummy
	img_conv->line_bytes.y_g	= img->g_width.y;
	img_conv->line_bytes.cb_b	= img->g_width.cb;
	img_conv->line_bytes.cr_r	= img->g_width.cr;
	img_conv->line_bytes.alpha	= img->g_width.y;	// dummy
}

