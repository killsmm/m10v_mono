/**
 * @file		still_utility.c
 * @brief		Utility APIs for Still.
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include <stdlib.h>

#include "dd_top.h"
#include "im_xch.h"
#include "sdram_map_manager.h"
#include "sdram_map_cache_define.h"
#include "still.h"
#include "still_utility.h"
#include "still_common.h"
#include "still_proc_subr.h"
#include "still_debug.h"
#include "still_dcache.h"
#include "still_img_info.h"
#include "still_attr.h"
#include "instance.h"
#include "debug.h"
#include "l1l2cache.h"
#include "mode.h"
#include "mba_if.h"
#include "jpgepc.h"
// --- REMOVE_EIS BEGIN ---
#include "ip_eis.h"
// --- REMOVE_EIS END ---

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_JPEG_THUMB_QUALITY				(3)
#define QUAT_VAL							(3)
#define D_STILL_UTILITY_JPEG_QTBL_CRRENT	(0)
// --- REMOVE_AHA BEGIN ---
#define D_STILL_UTILITY_GYRO_DATA_CNT		(0xFUL)
// --- REMOVE_AHA END ---

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// nothing special.

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// nothing special.

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Zigzag scan order for Y.
static volatile T_IM_JPEG_QUAT_TBL gStill_Utility_JPEG_ENC_Qtbl_Base_Y = {
	{	0x10,	0x0B,	0x0C,	0x0E,	0x0C,	0x0A,	0x10,	0x0E,
		0x0D,	0x0E,	0x12,	0x11,	0x10,	0x13,	0x18,	0x28,
		0x1A,	0x18,	0x16,	0x16,	0x18,	0x31,	0x23,	0x25,
		0x1D,	0x28,	0x3A,	0x33,	0x3D,	0x3C,	0x39,	0x33,
		0x38,	0x37,	0x40,	0x48,	0x5C,	0x4E,	0x40,	0x44,
		0x57,	0x45,	0x37,	0x38,	0x50,	0x6D,	0x51,	0x57,
		0x5F,	0x62,	0x67,	0x68,	0x67,	0x3E,	0x4D,	0x71,
		0x79,	0x70,	0x64,	0x78,	0x5C,	0x65,	0x67,	0x63 }
};

static volatile const T_IM_JPEG_QUAT_TBL gStill_Utility_JPEG_ENC_Qtbl_Base_Y_Const = {
	{	0x10,	0x0B,	0x0C,	0x0E,	0x0C,	0x0A,	0x10,	0x0E,
		0x0D,	0x0E,	0x12,	0x11,	0x10,	0x13,	0x18,	0x28,
		0x1A,	0x18,	0x16,	0x16,	0x18,	0x31,	0x23,	0x25,
		0x1D,	0x28,	0x3A,	0x33,	0x3D,	0x3C,	0x39,	0x33,
		0x38,	0x37,	0x40,	0x48,	0x5C,	0x4E,	0x40,	0x44,
		0x57,	0x45,	0x37,	0x38,	0x50,	0x6D,	0x51,	0x57,
		0x5F,	0x62,	0x67,	0x68,	0x67,	0x3E,	0x4D,	0x71,
		0x79,	0x70,	0x64,	0x78,	0x5C,	0x65,	0x67,	0x63 }
};

// Zigzag scan order for Cb/Cr.
static volatile T_IM_JPEG_QUAT_TBL gStill_Utility_JPEG_ENC_Qtbl_Base_C = {
	{	0x11,	0x12,	0x12,	0x18,	0x15,	0x18,	0x2F,	0x1A,
		0x1A,	0x2F,	0x63,	0x42,	0x38,	0x42,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63 }
};

static volatile const T_IM_JPEG_QUAT_TBL gStill_Utility_JPEG_ENC_Qtbl_Base_C_Const = {
	{	0x11,	0x12,	0x12,	0x18,	0x15,	0x18,	0x2F,	0x1A,
		0x1A,	0x2F,	0x63,	0x42,	0x38,	0x42,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,
		0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63,	0x63 }
};

static const T_IM_JPEG_QUAT_TBL gStill_Utility_JPEG_ENC_Qtbl_Debug_Y = {
	{	0x10>>QUAT_VAL,	0x0B>>QUAT_VAL,	0x0C>>QUAT_VAL,	0x0E>>QUAT_VAL,	0x0C>>QUAT_VAL,	0x0A>>QUAT_VAL,	0x10>>QUAT_VAL,	0x0E>>QUAT_VAL,
		0x0D>>QUAT_VAL,	0x0E>>QUAT_VAL,	0x12>>QUAT_VAL,	0x11>>QUAT_VAL,	0x10>>QUAT_VAL,	0x13>>QUAT_VAL,	0x18>>QUAT_VAL,	0x28>>QUAT_VAL,
		0x1A>>QUAT_VAL,	0x18>>QUAT_VAL,	0x16>>QUAT_VAL,	0x16>>QUAT_VAL,	0x18>>QUAT_VAL,	0x31>>QUAT_VAL,	0x23>>QUAT_VAL,	0x25>>QUAT_VAL,
		0x1D>>QUAT_VAL,	0x28>>QUAT_VAL,	0x3A>>QUAT_VAL,	0x33>>QUAT_VAL,	0x3D>>QUAT_VAL,	0x3C>>QUAT_VAL,	0x39>>QUAT_VAL,	0x33>>QUAT_VAL,
		0x38>>QUAT_VAL,	0x37>>QUAT_VAL,	0x40>>QUAT_VAL,	0x48>>QUAT_VAL,	0x5C>>QUAT_VAL,	0x4E>>QUAT_VAL,	0x40>>QUAT_VAL,	0x44>>QUAT_VAL,
		0x57>>QUAT_VAL,	0x45>>QUAT_VAL,	0x37>>QUAT_VAL,	0x38>>QUAT_VAL,	0x50>>QUAT_VAL,	0x6D>>QUAT_VAL,	0x51>>QUAT_VAL,	0x57>>QUAT_VAL,
		0x5F>>QUAT_VAL,	0x62>>QUAT_VAL,	0x67>>QUAT_VAL,	0x68>>QUAT_VAL,	0x67>>QUAT_VAL,	0x3E>>QUAT_VAL,	0x4D>>QUAT_VAL,	0x71>>QUAT_VAL,
		0x79>>QUAT_VAL,	0x70>>QUAT_VAL,	0x64>>QUAT_VAL,	0x78>>QUAT_VAL,	0x5C>>QUAT_VAL,	0x65>>QUAT_VAL,	0x67>>QUAT_VAL,	0x63>>QUAT_VAL }
};

// Zigzag scan order for Cb/Cr
static const T_IM_JPEG_QUAT_TBL gStill_Utility_JPEG_ENC_Qtbl_Debug_C = {
	{	0x11>>QUAT_VAL,	0x12>>QUAT_VAL,	0x12>>QUAT_VAL,	0x18>>QUAT_VAL,	0x15>>QUAT_VAL,	0x18>>QUAT_VAL,	0x2F>>QUAT_VAL,	0x1A>>QUAT_VAL,
		0x1A>>QUAT_VAL,	0x2F>>QUAT_VAL,	0x63>>QUAT_VAL,	0x42>>QUAT_VAL,	0x38>>QUAT_VAL,	0x42>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,
		0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,
		0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,
		0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,
		0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,
		0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,
		0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL,	0x63>>QUAT_VAL }
};

static FJ_GYRO_CONFIG	gStill_Utility_Gyro_Config				= { { 384, 216, 3072, 1728 }, 0.0f, 1.0f, 16600, 1000, 0, 0 };
// --- REMOVE_AHA BEGIN ---
static BF_GYRO_DATA_AHA	gStill_Utility_Gyro_Data[ D_STILL_UTILITY_GYRO_DATA_CNT + 1 ];
// --- REMOVE_AHA END ---

// for Linker
extern VOID Jpgepc_Wrap_Err_Printer( VOID );
VOID* test_grlobal = (VOID*)Jpgepc_Wrap_Err_Printer;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// nothing special.

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

static VOID bf_still_capture_convert_img_info_rgb( const FJ_IMG_INFO_CONV* const org_img, T_BF_STILL_IMG_CONV* conv_img )
{
	E_BF_STILL_IMG_TYPE		type;
	E_BF_STILL_PLANE_TYPE	plane;

	switch ( org_img->pfmt ) {
		case FJ_IMG_PFMT_YCC420:
			type	= E_BF_STILL_IMG_TYPE_YCC420;
			plane	= E_BF_STILL_PLANE_TYPE_C_INTL;
			break;

		case FJ_IMG_PFMT_YCC422:
			type	= E_BF_STILL_IMG_TYPE_YCC422;
			plane	= E_BF_STILL_PLANE_TYPE_C_INTL;
			break;

		case FJ_IMG_PFMT_YCC444:
			type	= E_BF_STILL_IMG_TYPE_YCC444;
			plane	= E_BF_STILL_PLANE_TYPE_C_INTL;
			break;

		case FJ_IMG_PFMT_VIDEO:
			type	= E_BF_STILL_IMG_TYPE_YCC420;
			plane	= E_BF_STILL_PLANE_TYPE_VIDEO;
			break;

		case FJ_IMG_PFMT_RGBA8888:
			type	= E_BF_STILL_IMG_TYPE_RGB888;
			plane	= E_BF_STILL_PLANE_TYPE_CHUNKY;
			break;

		case FJ_IMG_PFMT_RGBA4444:
			type	= E_BF_STILL_IMG_TYPE_RGB444;
			plane	= E_BF_STILL_PLANE_TYPE_CHUNKY;
			break;

		default:
			return;
	}

	conv_img->type				= type;
	conv_img->plane				= plane;
	conv_img->depth				= E_BF_STILL_IMG_DEPTH_U8;
	conv_img->addr.y_g			= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( org_img->addr.Y_G );
	conv_img->addr.cb_b			= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( org_img->addr.Cb_B );
	conv_img->addr.cr_r			= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( org_img->addr.Cr_R );
	conv_img->addr.alpha		= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( org_img->addr.Alpha );
	conv_img->g_width.y_g		= org_img->g_y_g_width;
	conv_img->g_width.cb_b		= org_img->g_cb_b_width;
	conv_img->g_width.cr_r		= org_img->g_cr_r_width;
	conv_img->g_width.alpha		= org_img->g_alpha_width;
	conv_img->pixs.width		= org_img->width;
	conv_img->pixs.lines		= org_img->lines;
	conv_img->line_bytes.y_g	= org_img->line_bytes.Y_G;
	conv_img->line_bytes.cb_b	= org_img->line_bytes.Cb_B;
	conv_img->line_bytes.cr_r	= org_img->line_bytes.Cr_R;
	conv_img->line_bytes.alpha	= org_img->line_bytes.Alpha;

	return;
}

static VOID bf_still_capture_convert_img_info_crop( const FJ_IMG_INFO* const org_img, T_BF_STILL_IMG_CONV* conv_img )
{
	E_BF_STILL_IMG_TYPE		type;
	E_BF_STILL_PLANE_TYPE	plane;

	switch ( org_img->pfmt ) {
		case FJ_IMG_PFMT_YCC420:
			type	= E_BF_STILL_IMG_TYPE_YCC420;
			plane	= E_BF_STILL_PLANE_TYPE_C_INTL;
			break;

		case FJ_IMG_PFMT_YCC422:
			type	= E_BF_STILL_IMG_TYPE_YCC422;
			plane	= E_BF_STILL_PLANE_TYPE_C_INTL;
			break;

		case FJ_IMG_PFMT_YCC444:
			type	= E_BF_STILL_IMG_TYPE_YCC444;
			plane	= E_BF_STILL_PLANE_TYPE_C_INTL;
			break;

		case FJ_IMG_PFMT_VIDEO:
			type	= E_BF_STILL_IMG_TYPE_YCC420;
			plane	= E_BF_STILL_PLANE_TYPE_VIDEO;
			break;

		case FJ_IMG_PFMT_RGBA8888:
			type	= E_BF_STILL_IMG_TYPE_RGB888;
			plane	= E_BF_STILL_PLANE_TYPE_PLANAR;
			break;

		case FJ_IMG_PFMT_RGBA4444:
			type	= E_BF_STILL_IMG_TYPE_RGB444;
			plane	= E_BF_STILL_PLANE_TYPE_CHUNKY;
			break;

		default:
			return;
	}

	conv_img->type				= type;
	conv_img->plane				= plane;
	conv_img->depth				= E_BF_STILL_IMG_DEPTH_U8;
	conv_img->addr.y_g			= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( org_img->addr.AddressY );
	conv_img->addr.cb_b			= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( org_img->addr.AddressCb );
	conv_img->addr.cr_r			= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( org_img->addr.AddressCr );
	conv_img->addr.alpha		= 0;
	conv_img->g_width.y_g		= org_img->g_y_width;
	conv_img->g_width.cb_b		= org_img->g_c_width;
	conv_img->g_width.cr_r		= org_img->g_c_width;
	conv_img->g_width.alpha		= 0;
	conv_img->pixs.width		= org_img->width;
	conv_img->pixs.lines		= org_img->lines;
	conv_img->line_bytes.y_g	= 0;
	conv_img->line_bytes.cb_b	= 0;
	conv_img->line_bytes.cr_r	= 0;
	conv_img->line_bytes.alpha	= 0;

	return;
}

static VOID bf_still_capture_calc_effective_image_size( const T_IMAGE_SIZE* const src_pixs, const T_IMAGE_SIZE* const dst_pixs, T_IMAGE_SIZE* const effective_img )
{
	USHORT	tmp_width;
	USHORT	tmp_lines;

	// Calculate the vertical.(As the basis to the horizontal size).
	tmp_lines = ( (UINT32)src_pixs->lines * dst_pixs->width ) / src_pixs->width;

	// Fill up and down or Fill nothing
	if ( tmp_lines <= dst_pixs->lines ) {
		effective_img->width	= dst_pixs->width;
		effective_img->lines	= tmp_lines;
	}
	// Fill left and right
	else {
		// Calculate the horizontal.(As the basis to the vertical size).
		tmp_width = ( (UINT32)src_pixs->width * dst_pixs->lines ) / src_pixs->lines;

		effective_img->width	= tmp_width;
		effective_img->lines	= dst_pixs->lines;
	}

	return;
}

static VOID bf_still_capture_calc_crop_rgb(T_BF_STILL_IMG_CROP *crop, T_BF_STILL_IMG_CONV *src, const T_IMAGE_SIZE* src_correct_aspect_ratio, T_BF_STILL_IMG_CONV *dst)
{
	T_IMAGE_SIZE	effective_img;
	UINT32			out_fill_size;
	UINT32			in_fill_size;

	if( src_correct_aspect_ratio == NULL ) {
		src_correct_aspect_ratio = &src->pixs;
	}

	// Calculation of the effective image area.
	bf_still_capture_calc_effective_image_size( src_correct_aspect_ratio, &dst->pixs, &effective_img );

	// Fill up and down.
	if ( effective_img.lines < dst->pixs.lines ) {

		out_fill_size = M_BF_STILL_ROUNDUP_4(dst->pixs.lines - effective_img.lines);

		in_fill_size = ((out_fill_size * crop->lines) / effective_img.lines);

		crop->lines += in_fill_size;

		crop->sta_y -= (((DOUBLE)in_fill_size) / 2);
		crop->sta_x = 0.0;

	}
	// Fill left and right.
	else if ( effective_img.width < dst->pixs.width ) {

		out_fill_size = M_BF_STILL_ROUNDUP_4(dst->pixs.width - effective_img.width);

		in_fill_size = ((out_fill_size * crop->width) / effective_img.width);

		crop->width += in_fill_size;

		crop->sta_x -= (((DOUBLE)in_fill_size) / 2);
		crop->sta_y = 0;
	}
	else {
	}
}

static ULONG bf_still_utility_encode_jpeg_for_CR( FJ_IMG_INFO* jpeg_info, ULONG dst_addr, INT32 wait_time, USHORT cr )
{
	T_IM_JPEG_ENC_MNG			enc_mng = {
		.mem_format					= E_IM_JPEG_MEM_FORM_PLANE_DOT,
		.component[0].quant_tbl_no	= 0,	// Y
		.component[0].huf_dc_tbl_no	= 0,	// Y
		.component[0].huf_ac_tbl_no	= 0,	// Y
		.component[1].quant_tbl_no	= 1,	// CB
		.component[1].huf_dc_tbl_no	= 1,	// CB
		.component[1].huf_ac_tbl_no	= 1,	// CB
		.component[2].quant_tbl_no	= 1,	// CR
		.component[2].huf_dc_tbl_no	= 1,	// CR
		.component[2].huf_ac_tbl_no	= 1,	// CR
		.skip_mk_flg				= D_IM_JPEG_ENABLE_OFF,
		.dri_mk_num					= 0,
		.exif_fmt_flg				= D_IM_JPEG_ENABLE_OFF,
		.pint_line					= 0,
		.pint_sect					= 0,
		.pbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE,
		.pbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_4,
		.pbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF,
		.pbuf_ctrl.prot_type		= D_IM_JPEG_D_SEC_NORMAL_ACCESS,
		.pbuf_ctrl.err_state		= D_IM_JPEG_AXI_OKAY,
		.jbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE,
		.jbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_4,
		.jbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF,
		.jbuf_ctrl.prot_type		= D_IM_JPEG_D_SEC_NORMAL_ACCESS,
		.jbuf_ctrl.err_state		= D_IM_JPEG_AXI_OKAY,	// output parameter.
		.pburst_length				= E_IM_JPEG_BURST_INCR_16,
		.jburst_length				= E_IM_JPEG_BURST_INCR_16,
		.bit_depth					= E_IM_JPEG_BIT_DEPTH_8BIT,
		.burst_alignment			= E_IM_JPEG_BURST_AL_ON,
		.port_mode					= E_IM_JPEG_IIP_PORT_MODE_OFF,
		.ring_on					= D_IM_JPEG_ENABLE_OFF,
		.block_size					= E_IM_JPEG_IIP_BLOCK_64x8,
		.ring_size					= 1,
		.code_size					= 0,	// output parameter.
		.result						= 0,	// output parameter.
		.pcallback					= NULL,
	};
	T_IM_JPEG_ENC_FRAME_MNG		enc_frame_mng = {
		.code_count_flg				= D_IM_JPEG_ENABLE_OFF,
		.code_over_count_flg		= D_IM_JPEG_ENABLE_OFF,
		.limit_size					= 0,
	};
	E_IM_JPEG_SMPL_TYPE			ycmode;

	T_BF_JPGENC_PARAM			jpgenc_param;
	
	switch ( jpeg_info->pfmt ) {
		case FJ_IMG_PFMT_YCC420:
			ycmode = E_IM_JPEG_SMPL_TYPE_YCC420;
			break;

		case FJ_IMG_PFMT_YCC422:
			ycmode = E_IM_JPEG_SMPL_TYPE_YCC422;
			break;

		case FJ_IMG_PFMT_YCC444:
			ycmode = E_IM_JPEG_SMPL_TYPE_YCC444;
			break;

		default:
			BF_Debug_Print_Error(( "%s(): unknown pfmt %u\n", __func__, jpeg_info->pfmt ));
			ycmode = E_IM_JPEG_SMPL_TYPE_YCC422;	// failsafe
			break;
	}
	
	enc_mng.ycc_smpl				= ycmode;
	enc_mng.width					= jpeg_info->width;
	enc_mng.lines					= jpeg_info->lines;
	
	enc_frame_mng.global_y_width	= jpeg_info->g_y_width;
	enc_frame_mng.global_c_width	= jpeg_info->g_c_width;
	enc_frame_mng.ycc_addr.y		= jpeg_info->addr.AddressY;
	enc_frame_mng.ycc_addr.c		= jpeg_info->addr.AddressCb;
	enc_frame_mng.code_addr			= dst_addr;



	jpgenc_param.jpg_enc_mng		= enc_mng;
	jpgenc_param.jpg_enc_frame_mng	= enc_frame_mng;
	
	jpgenc_param.jpg_enc_quant_tbl.quantization_tbl_0	= (T_IM_JPEG_QUAT_TBL*)&gStill_Utility_JPEG_ENC_Qtbl_Base_Y;
	jpgenc_param.jpg_enc_quant_tbl.quantization_tbl_1	= (T_IM_JPEG_QUAT_TBL*)&gStill_Utility_JPEG_ENC_Qtbl_Base_C;
	jpgenc_param.jpg_enc_quant_tbl.quantization_tbl_2	= (T_IM_JPEG_QUAT_TBL*)&gStill_Utility_JPEG_ENC_Qtbl_Base_Y;
	jpgenc_param.jpg_enc_quant_tbl.quantization_tbl_3	= (T_IM_JPEG_QUAT_TBL*)&gStill_Utility_JPEG_ENC_Qtbl_Base_C;

	jpgenc_param.mode				= E_BF_JPGEPC_MODE_CAPTURE;
	jpgenc_param.limit_size			= jpeg_info->width * jpeg_info->lines * 3;
	jpgenc_param.wait_time			= wait_time;
	jpgenc_param.quality			= 0;
	jpgenc_param.cr					= cr;

	Jpgepc_Encode_Jpeg( &jpgenc_param );

	BF_L1l2cache_Clean_Flush_Addr( dst_addr, (((UINT32)jpeg_info->width) * jpeg_info->lines) );
	
	return jpgenc_param.jpg_enc_mng.code_size;
}

static ULONG bf_still_utility_encode_jpeg( FJ_IMG_INFO* jpeg_info, ULONG dst_addr, INT32 wait_time )
{
	T_IM_JPEG_ENC_MNG			enc_mng = {
		.mem_format					= E_IM_JPEG_MEM_FORM_PLANE_DOT,
		.component[0].quant_tbl_no	= 0,	// Y
		.component[0].huf_dc_tbl_no	= 0,	// Y
		.component[0].huf_ac_tbl_no	= 0,	// Y
		.component[1].quant_tbl_no	= 1,	// CB
		.component[1].huf_dc_tbl_no	= 1,	// CB
		.component[1].huf_ac_tbl_no	= 1,	// CB
		.component[2].quant_tbl_no	= 1,	// CR
		.component[2].huf_dc_tbl_no	= 1,	// CR
		.component[2].huf_ac_tbl_no	= 1,	// CR
		.skip_mk_flg				= D_IM_JPEG_ENABLE_OFF,
		.dri_mk_num					= 0,
		.exif_fmt_flg				= D_IM_JPEG_ENABLE_OFF,
		.pint_line					= 0,
		.pint_sect					= 0,
		.pbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE,
		.pbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_4,
		.pbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF,
		.pbuf_ctrl.prot_type		= D_IM_JPEG_D_SEC_NORMAL_ACCESS,
		.pbuf_ctrl.err_state		= D_IM_JPEG_AXI_OKAY,
		.jbuf_ctrl.endian			= E_IM_JPEG_ENDIAN_LITTLE,
		.jbuf_ctrl.issue_tran_num	= E_IM_JPEG_ISSUE_TRAN_4,
		.jbuf_ctrl.cache_type		= D_IM_JPEG_NON_CACHE_NON_BUF,
		.jbuf_ctrl.prot_type		= D_IM_JPEG_D_SEC_NORMAL_ACCESS,
		.jbuf_ctrl.err_state		= D_IM_JPEG_AXI_OKAY,	// output parameter.
		.pburst_length				= E_IM_JPEG_BURST_INCR_16,
		.jburst_length				= E_IM_JPEG_BURST_INCR_16,
		.bit_depth					= E_IM_JPEG_BIT_DEPTH_8BIT,
		.burst_alignment			= E_IM_JPEG_BURST_AL_ON,
		.port_mode					= E_IM_JPEG_IIP_PORT_MODE_OFF,
		.ring_on					= D_IM_JPEG_ENABLE_OFF,
		.block_size					= E_IM_JPEG_IIP_BLOCK_64x8,
		.ring_size					= 1,
		.code_size					= 0,	// output parameter.
		.result						= 0,	// output parameter.
		.pcallback					= NULL,
	};
	T_IM_JPEG_ENC_FRAME_MNG		enc_frame_mng = {
		.code_count_flg				= D_IM_JPEG_ENABLE_OFF,
		.code_over_count_flg		= D_IM_JPEG_ENABLE_OFF,
		.limit_size					= 0,
	};
	E_IM_JPEG_SMPL_TYPE			ycmode;
	INT32						ret;

	switch ( jpeg_info->pfmt ) {
		case FJ_IMG_PFMT_YCC420:
			ycmode = E_IM_JPEG_SMPL_TYPE_YCC420;
			break;

		case FJ_IMG_PFMT_YCC422:
			ycmode = E_IM_JPEG_SMPL_TYPE_YCC422;
			break;

		case FJ_IMG_PFMT_YCC444:
			ycmode = E_IM_JPEG_SMPL_TYPE_YCC444;
			break;

		default:
			BF_Debug_Print_Error(( "%s(): unknown pfmt %u\n", __func__, jpeg_info->pfmt ));
			ycmode = E_IM_JPEG_SMPL_TYPE_YCC422;	// failsafe
			break;
	}

	enc_mng.ycc_smpl				= ycmode;
	enc_mng.width					= jpeg_info->width;
	enc_mng.lines					= jpeg_info->lines;

	ret = Im_JPEG_Ctrl_Enc( &enc_mng );
	if ( ret != D_IM_JPEG_OK ) {
		BF_Debug_Print_Information(("I:Im_JPEG_Ctrl_Enc() Error : ercd = %x\n", ret));
	}

	enc_frame_mng.global_y_width	= jpeg_info->g_y_width;
	enc_frame_mng.global_c_width	= jpeg_info->g_c_width;
	enc_frame_mng.ycc_addr.y		= jpeg_info->addr.AddressY;
	enc_frame_mng.ycc_addr.c		= jpeg_info->addr.AddressCb;
	enc_frame_mng.code_addr			= dst_addr;

	BF_L1l2cache_Clean_Flush_Addr( dst_addr, (((UINT32)jpeg_info->width) * jpeg_info->lines) );

	ret = Im_JPEG_Ctrl_Enc_Frame( &enc_frame_mng );
	if ( ret != D_IM_JPEG_OK ) {
		BF_Debug_Print_Information(("I:Im_JPEG_Ctrl_Frame() Error : ercd = %x\n", ret));
	}

	ret = Im_JPEG_Start_Enc();
	if ( ret != D_IM_JPEG_OK ) {
		BF_Debug_Print_Information(("I:Im_JPEG_Start_Enc() Error : ercd = %x\n", ret));
	}

	ret = Im_JPEG_Wait_End_Enc( &enc_mng, wait_time );
	if ( ret != D_IM_JPEG_OK ) {
		BF_Debug_Print_Information(("I:Im_JPEG_Wait_End_Enc() Error : ercd = %x\n", ret));
	}

	return enc_mng.code_size;
}

static VOID bf_still_utility_update_jpeg_qtbl( UCHAR still_encode_id, T_IM_JPEG_QUAT_TBL_PACK* const quant_tbl )
{
	T_BF_INSTANCE_STILL_ENCODE*	still_encode;
	INT32						cnt;
	USHORT						kind;
	USHORT						quality;
	ULONG						scale;
	ULONG						temp_y;
	ULONG						temp_c;
	T_IM_JPEG_QUAT_TBL*			quantization_tbl_0;
	T_IM_JPEG_QUAT_TBL*			quantization_tbl_1;

	still_encode = BF_Instance_Get_StillEncode( still_encode_id );

	if ( still_encode != NULL ) {
		kind	= still_encode->quality.kind;
		quality = still_encode->quality.operand;
	}
	else {
		kind	= FJ_JPEG_QUALITY_KIND_Q;
		quality	= 50;
	}

	// CR->Quality.
	if ( kind == FJ_JPEG_QUALITY_KIND_CR ) {
		quality = BF_Still_Utility_Get_Jpeg_Encode_Quality( quality, 0 );
	}

	if ( quality == 0 ) {
		scale = 5000;
	}
	else if ( quality >= 100 ) {
		scale = 0;
	}
	else if ( quality < 50 ) {
		scale = ( 5000 / quality );
	}
	else {
		scale = ( 200 - quality * 2 );
	}

	quantization_tbl_0 = quant_tbl->quantization_tbl_0;
	quantization_tbl_1 = quant_tbl->quantization_tbl_1;

	for ( cnt = 0; cnt < 64; cnt ++ ) {

		temp_y = ( (ULONG)gStill_Utility_JPEG_ENC_Qtbl_Base_Y_Const.quant_value[ cnt ] * scale + 50 ) / 100;
		temp_c = ( (ULONG)gStill_Utility_JPEG_ENC_Qtbl_Base_C_Const.quant_value[ cnt ] * scale + 50 ) / 100;

		// limit the values to the valid range.
		if ( temp_y == 0 ) {
			temp_y = 1;
		}

		if ( temp_c == 0 ) {
			temp_c = 1;
		}

		// max quantizer needed for baseline.
		if ( temp_y > 255 ) {
			temp_y = 255;
		}

		if ( temp_c > 255 ) {
			temp_c = 255;
		}

		quantization_tbl_0->quant_value[ cnt ] = (UCHAR)temp_y;
		quantization_tbl_1->quant_value[ cnt ] = (UCHAR)temp_c;
	}
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

//=========================================================================================

FJ_ERR_CODE fj_imageconvert( FJ_IMG_CONV_CTRL* conv_info )
{
	FJ_ERR_CODE	fj_ercd;

	fj_ercd = BF_Still_Dcache_CleanFlush_FjInfoConv( &conv_info->src_img );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	fj_ercd = BF_Still_Dcache_CleanFlush_FjInfoConv( &conv_info->dst_img );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	return BF_Still_FJ_ImageConvert( conv_info, NULL );
}

FJ_ERR_CODE fj_imagecrop( FJ_IMG_CROP_CTRL* crop_info )
{
	T_BF_STILL_IIP_IMG_CONVERT		img_info;
	UCHAR							fill_en = 0;

	// Convert the param argument.
	bf_still_capture_convert_img_info_crop( &crop_info->src_img, &img_info.src );
	bf_still_capture_convert_img_info_crop( &crop_info->dst_img, &img_info.dst );

	img_info.crop_rect.sta_x	= crop_info->src_rect.left;
	img_info.crop_rect.sta_y	= crop_info->src_rect.top;
	img_info.crop_rect.width	= crop_info->src_rect.width;
	img_info.crop_rect.lines	= crop_info->src_rect.lines;

	if ( crop_info->resize_mode == FJ_IMG_RESIZE_MODE_BILINEAR ) {
		img_info.itmd	= E_BF_STILL_IIP_ITMD_BILINEAR;
	}
	else {
		img_info.itmd	= E_BF_STILL_IIP_ITMD_BICUBIC;
	}

	fill_en = crop_info->fill_enable;

	// Fill Enable.
	if ( fill_en ) {
		bf_still_capture_calc_crop_rgb( &img_info.crop_rect, &img_info.src, NULL, &img_info.dst );

		if ( img_info.src.type == E_BF_STILL_IMG_TYPE_YCC422 ) {
			img_info.src.addr.cb_b	+= crop_info->src_rect.top * img_info.src.g_width.cb_b;
		}
		else {
			img_info.src.addr.cb_b	+= ( crop_info->src_rect.top / 2 ) * img_info.src.g_width.cb_b;
		}

		img_info.src.addr.y_g	+= crop_info->src_rect.top * img_info.src.g_width.y_g;
		img_info.src.addr.cr_r	= img_info.src.addr.cb_b;
		img_info.src.pixs.lines = crop_info->src_rect.lines;
	}
	// Fill Disable.
	else {
		// Do Nothing.
	}

	img_info.src_mif					= 0;
	img_info.dst_mif					= 0;
	img_info.fill_videofmt_alignment	= TRUE;
	img_info.rotate_deg					= FJ_IMG_ROTATE_DEGREE_0;
	img_info.mirror						= FALSE;

	// delete\basefw\fj\imgproc\still
	//return BF_Still_Proc_Execute_AfnConvertImage( &img_info, fill_en, crop_info->timeout );
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_imagejpegencode( FJ_IMG_JPEG_CTRL* enc_info )
{
	FJ_IMG_INFO	jpeg_info;
	ULONG		dst_addr;

	jpeg_info					= enc_info->src_img;
	dst_addr					= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)enc_info->dst_addr );
	jpeg_info.addr.AddressY		= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)enc_info->src_img.addr.AddressY );
	jpeg_info.addr.AddressCb	= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)enc_info->src_img.addr.AddressCb );
	jpeg_info.addr.AddressCr	= M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)enc_info->src_img.addr.AddressCr );

	enc_info->enc_size = BF_Still_Encode_Jpeg_Debug( &jpeg_info, dst_addr );
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_configgyro( FJ_GYRO_CONFIG* gyro_config )
{
	FJ_ERR_CODE fj_ret = FJ_ERR_OK;

	memcpy( (VOID*)&gStill_Utility_Gyro_Config, (VOID*)gyro_config, sizeof(FJ_GYRO_CONFIG));

	return fj_ret;
}

//extern INT32 gExposureCounter;
FJ_ERR_CODE fj_addgyrodata( ULLONG frame_no, FJ_GYRO_DATA* gyro_data )
{
	FJ_ERR_CODE					fj_ret = FJ_ERR_OK;
	INT32						ret;
// --- REMOVE_EIS BEGIN ---
	T_IP_EIS_GYRO_DATA			eis_gyro;
// --- REMOVE_EIS END ---

	if ( gyro_data == NULL ) {
		return FJ_ERR_NG_INPUT_PARAM;
	}

#if 0
	if( ( gyro_data->gyro->x < -5.0 ) ||
		( gyro_data->gyro->x >  5.0 ) ||
		( gyro_data->gyro->y < -5.0 ) ||
		( gyro_data->gyro->y >  5.0 ) ||
		( gyro_data->gyro->z < -5.0 ) ||
		( gyro_data->gyro->z >  5.0 ) ) {
		BF_Debug_Print_Information(( "x=%+02.6f,y=%+02.6f,z=%+02.6f, time=0x%08x\n", gyro_data->gyro->x, gyro_data->gyro->y, gyro_data->gyro->z, gyro_data->time_stamp ));
	}
#endif

// --- REMOVE_EIS BEGIN ---
	eis_gyro.timeStamp		= (UINT32)gyro_data->time_stamp;
	eis_gyro.exposureTime	= gyro_data->exposure_time;
	eis_gyro.gyro			= (T_IP_EIS_GYRO_SAMPLE*)gyro_data->gyro;
	eis_gyro.accel			= (T_IP_EIS_GYRO_SAMPLE*)gyro_data->accel;
	eis_gyro.magn			= (T_IP_EIS_GYRO_SAMPLE*)gyro_data->magnet;
// --- REMOVE_EIS END ---

	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_START, " AddGyro %04lX", (ULONG)frame_no ));
// --- REMOVE_AHA BEGIN ---
	FJ_STILL_STITCH_TYPE stitch_type;
	FJ_Get_Capture_Stitch_Status( &stitch_type );
	if ( stitch_type == FJ_STILL_STITCH_TYPE_VIDEO_AHA ) {
		if ( gStill_Utility_Gyro_Data[ frame_no & D_STILL_UTILITY_GYRO_DATA_CNT ].cnt < D_STILL_UTILITY_GYRO_DATA_AHA_CNT ) {
			gStill_Utility_Gyro_Data[ frame_no & D_STILL_UTILITY_GYRO_DATA_CNT ].data[ gStill_Utility_Gyro_Data[ frame_no & D_STILL_UTILITY_GYRO_DATA_CNT ].cnt++ ] = *gyro_data;
		}
	}
	else {
// --- REMOVE_AHA END ---
// --- REMOVE_EIS BEGIN ---
		ret = IP_EIS_AddGyroData( 0 , &eis_gyro );
		if ( ret != D_IP_EIS_OK ) {
			BF_Debug_Print_Error(( "IP_EIS_AddGyroData() error = 0x%08x\n", ret ));
			fj_ret = FJ_ERR_NG;
		}
// --- REMOVE_EIS END ---
// --- REMOVE_AHA BEGIN ---
	}
// --- REMOVE_AHA END ---
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_END, " AddGyro %04lX", (ULONG)frame_no ));

	return fj_ret;
}

VOID BF_Still_Get_Gyro_Config( FJ_GYRO_CONFIG* const gyro_config )
{
	*gyro_config = gStill_Utility_Gyro_Config;
}

ULONG BF_Still_Encode_Jpeg_Debug( FJ_IMG_INFO* jpeg_info, ULONG dst_addr )
{
	T_IM_JPEG_QUAT_TBL_PACK		quant_tbl;
	INT32						ret;
	ULONG						code_size;
	INT32						wait_time = 3000;

	Dd_Top_Set_CLKSTOP3_JPGAH( 0 );
	Dd_Top_Set_CLKSTOP3_JPGAX( 0 );
	Dd_Top_Set_CLKSTOP3_JPGCK( 0 );

	ret = Im_JPEG_Open( wait_time );

	if ( ret != D_IM_JPEG_OK ) {
		BF_Debug_Print_Information(("I:Im_JPEG_Open() Error : ercd = %x\n", ret));
	}

	quant_tbl.quantization_tbl_0 = (T_IM_JPEG_QUAT_TBL*)&gStill_Utility_JPEG_ENC_Qtbl_Debug_Y;
	quant_tbl.quantization_tbl_1 = (T_IM_JPEG_QUAT_TBL*)&gStill_Utility_JPEG_ENC_Qtbl_Debug_C;
	quant_tbl.quantization_tbl_2 = (T_IM_JPEG_QUAT_TBL*)&gStill_Utility_JPEG_ENC_Qtbl_Debug_Y;
	quant_tbl.quantization_tbl_3 = (T_IM_JPEG_QUAT_TBL*)&gStill_Utility_JPEG_ENC_Qtbl_Debug_C;

	Im_JPEG_Set_QTbl( &quant_tbl, D_STILL_UTILITY_JPEG_QTBL_CRRENT );

	code_size = bf_still_utility_encode_jpeg( jpeg_info, dst_addr, wait_time );

	ret = Im_JPEG_Close();
	if ( ret != D_IM_JPEG_OK ) {
		BF_Debug_Print_Information(("I:Im_JPEG_Close() Error : ercd = %x\n", ret));
	}

	BF_Debug_Print_Information(( "I:Jpeg Encode completed. src:0x%x dst:0x%lx encsize:%lu\n",
				jpeg_info->addr.AddressY,
				dst_addr,
				code_size ));

	return code_size;
}

ULONG BF_Still_Encode_Jpeg( UCHAR still_encode_id, FJ_IMG_INFO* jpeg_info, ULONG dst_addr, INT32 wait_time )
{
	T_IM_JPEG_QUAT_TBL_PACK		quant_tbl;
	INT32						ret;
	ULONG						code_size;
	T_BF_INSTANCE_STILL_ENCODE*	still_encode;
	USHORT						kind;
	USHORT						cr;

	Dd_Top_Set_CLKSTOP3_JPGAH( 0 );
	Dd_Top_Set_CLKSTOP3_JPGAX( 0 );
	Dd_Top_Set_CLKSTOP3_JPGCK( 0 );
	
	still_encode = BF_Instance_Get_StillEncode( still_encode_id );
	if ( still_encode != NULL ) {
		kind	= still_encode->quality.kind;
	}
	else {
		kind	= FJ_JPEG_QUALITY_KIND_Q;
	}
	
	if( kind == FJ_JPEG_QUALITY_KIND_Q ){
		ret = Im_JPEG_Open( wait_time );

		if ( ret != D_IM_JPEG_OK ) {
			BF_Debug_Print_Information(("I:Im_JPEG_Open() Error : ercd = %x\n", ret));
		}

		quant_tbl.quantization_tbl_0 = (T_IM_JPEG_QUAT_TBL*)&gStill_Utility_JPEG_ENC_Qtbl_Base_Y;
		quant_tbl.quantization_tbl_1 = (T_IM_JPEG_QUAT_TBL*)&gStill_Utility_JPEG_ENC_Qtbl_Base_C;
		quant_tbl.quantization_tbl_2 = (T_IM_JPEG_QUAT_TBL*)&gStill_Utility_JPEG_ENC_Qtbl_Base_Y;
		quant_tbl.quantization_tbl_3 = (T_IM_JPEG_QUAT_TBL*)&gStill_Utility_JPEG_ENC_Qtbl_Base_C;

		bf_still_utility_update_jpeg_qtbl( still_encode_id, &quant_tbl );

		Im_JPEG_Set_QTbl( &quant_tbl, D_STILL_UTILITY_JPEG_QTBL_CRRENT );

		code_size = bf_still_utility_encode_jpeg( jpeg_info, dst_addr, wait_time );

		ret = Im_JPEG_Close();
		if ( ret != D_IM_JPEG_OK ) {
			BF_Debug_Print_Information(("I:Im_JPEG_Close() Error : ercd = %x\n", ret));
		}
	}
	else{	// FJ_JPEG_QUALITY_KIND_CR
		cr = still_encode->quality.operand;		// pgr0689
		code_size = bf_still_utility_encode_jpeg_for_CR( jpeg_info, dst_addr, wait_time, cr );
	}
	
	BF_Debug_Print_Information(( "I:Jpeg Encode completed. src:0x%x dst:0x%lx encsize:%lu\n",
				jpeg_info->addr.AddressY,
				dst_addr,
				code_size ));

	return code_size;
}

/**
 * Get Quality of JPEG Encode
 * @param  USHORT Compression Ratio
 * @return SHORT  quality
 */
SHORT BF_Still_Utility_Get_Jpeg_Encode_Quality( const USHORT cr, const UCHAR thumb_flg )
{
	SHORT quality = 1;

	if( thumb_flg == 1 ){
		return D_JPEG_THUMB_QUALITY;
	}

	switch ( cr ) {
		case FJ_CR_2_0:
		case FJ_CR_2_5:
		case FJ_CR_2_7:
		case FJ_CR_3_0:
		case FJ_CR_3_5:
		case FJ_CR_4_0:
		case FJ_CR_4_5:
			quality = 40;
			break;
		case FJ_CR_5_0:
		case FJ_CR_5_5:
		case FJ_CR_6_0:
		case FJ_CR_6_5:
		case FJ_CR_7_0:
			quality = 8;
			break;
		case FJ_CR_7_5:
		case FJ_CR_8_0:
			quality = 7;
			break;
		case FJ_CR_8_5:
		case FJ_CR_9_0:
		case FJ_CR_9_5:
		case FJ_CR_10_0:
			quality = 3;
			break;
		case FJ_CR_12_0:
		case FJ_CR_14_0:
			quality = 2;
			break;
		case FJ_CR_15_0:
		case FJ_CR_16_0:
		case FJ_CR_17_0:
		case FJ_CR_18_0:
		case FJ_CR_19_0:
		case FJ_CR_20_0:
		case FJ_CR_21_0:
		case FJ_CR_22_0:
		case FJ_CR_23_0:
		case FJ_CR_24_0:
		case FJ_CR_25_0:
		case FJ_CR_26_0:
		case FJ_CR_27_0:
		case FJ_CR_28_0:
		case FJ_CR_29_0:
		case FJ_CR_30_0:
		case FJ_CR_31_0:
		case FJ_CR_32_0:
		case FJ_CR_33_0:
		case FJ_CR_34_0:
		case FJ_CR_35_0:
		case FJ_CR_36_0:
		case FJ_CR_37_0:
		case FJ_CR_38_0:
		case FJ_CR_39_0:
		case FJ_CR_40_0:
			break;
		default :
			break;
	}
	return quality;
}

static T_BF_STILL_IIP_IMG_CONVERT	gImg_Info;
static BOOL								gImg_Conv_2_Param_Status = FALSE;
static BOOL								gImg_Conv_2_Act_Check_Flag = FALSE;

FJ_ERR_CODE BF_Still_Image_Convert_2_Init( FJ_IMG_CONV_CTRL* conv_info, const T_IMAGE_SIZE* src_correct_aspect_ratio )
{
	T_BF_STILL_IIP_IMG_CONVERT		img_info;
	T_BF_STILL_IMG_CONV				src =	{
												.type	=	E_BF_STILL_IMG_TYPE_YCC420,
												.plane	=	E_BF_STILL_PLANE_TYPE_C_INTL,
												.depth	=	E_BF_STILL_IMG_DEPTH_U8,
												.addr	=	{
																.y_g	= 0,
																.cb_b	= 0,
																.cr_r	= 0,
																.alpha	= 0,
															},
												.pixs	=	{
																.width	= 0,
																.lines	= 0,
															},
												.g_width=	{
																.y_g	= 0,
																.cb_b	= 0,
																.cr_r	= 0,
																.alpha	= 0,
															},
												.line_bytes={
																.y_g	= 0,
																.cb_b	= 0,
																.cr_r	= 0,
																.alpha	= 0,
															},
											};
	T_BF_STILL_IMG_CONV				dst;
	T_BF_STILL_IMG_CROP				crop;
	UCHAR							fill_en;

	if( gImg_Conv_2_Param_Status == TRUE ){
		// already parameter set
		return FJ_ERR_OK;
	}

	// Convert the param argument
	bf_still_capture_convert_img_info_rgb( &conv_info->src_img, &src );
	bf_still_capture_convert_img_info_rgb( &conv_info->dst_img, &dst );

	crop.sta_x	= 0.0;
	crop.sta_y	= 0.0;
	crop.width	= (DOUBLE)src.pixs.width;
	crop.lines	= (DOUBLE)src.pixs.lines;

	fill_en = conv_info->fill_enable;

	// Fill Enable.
	if ( fill_en ) {
		bf_still_capture_calc_crop_rgb( &crop, &src, src_correct_aspect_ratio, &dst );
	}
	// Fill Disable.
	else {
		// Do Nothing.
	}

	img_info.src						= src;
	img_info.dst						= dst;
	img_info.crop_rect					= crop;
	img_info.fill_videofmt_alignment	= TRUE;

	if ( conv_info->resize_mode == FJ_IMG_RESIZE_MODE_BILINEAR ) {
		img_info.itmd	= E_BF_STILL_IIP_ITMD_BILINEAR;
	}
	else {
		img_info.itmd	= E_BF_STILL_IIP_ITMD_BICUBIC;
	}

	img_info.src_mif	= 0;
	img_info.dst_mif	= 0;
	img_info.rotate_deg	= conv_info->rotate_deg;
	img_info.mirror		= FALSE;

	// update static information
	gImg_Info = img_info;
	gImg_Conv_2_Param_Status = TRUE;

	return BF_Still_Proc_Execute_AfnConvertImage_2_Open_SetData( &img_info, fill_en, conv_info->timeout );
}

FJ_ERR_CODE BF_Still_Image_Convert_2_Execute( FJ_IMG_CONV_CTRL* conv_info )
{
	FJ_ERR_CODE		fj_ercd;

	if( gImg_Conv_2_Param_Status == FALSE ){
		// not yet parameter set
		return FJ_ERR_NG;
	}

	// update (src/dst) address
	gImg_Info.src.addr.y_g			= conv_info->src_img.addr.Y_G;
	gImg_Info.src.addr.cb_b			= conv_info->src_img.addr.Cb_B;
	gImg_Info.src.addr.cr_r			= conv_info->src_img.addr.Cr_R;
	gImg_Info.src.addr.alpha		= conv_info->src_img.addr.Alpha;
	gImg_Info.dst.addr.y_g			= conv_info->dst_img.addr.Y_G;
	gImg_Info.dst.addr.cb_b			= conv_info->dst_img.addr.Cb_B;
	gImg_Info.dst.addr.cr_r			= conv_info->dst_img.addr.Cr_R;
	gImg_Info.dst.addr.alpha		= conv_info->dst_img.addr.Alpha;
	gImg_Info.rotate_deg			= conv_info->rotate_deg;

	gImg_Conv_2_Act_Check_Flag = TRUE;

	fj_ercd = BF_Still_Proc_Execute_AfnConvertImage_2_Execute( &gImg_Info );
	if( fj_ercd != FJ_ERR_OK ){
		BF_Debug_Print_Error(( "BF_Still_Image_Convert_2_Execute: no=%u ercd=%d\n", fj_ercd, 1 ));
	}

	gImg_Conv_2_Act_Check_Flag = FALSE;

	return fj_ercd;
}

FJ_ERR_CODE BF_Still_Image_Convert_2_Close( VOID )
{
	FJ_ERR_CODE		fj_ercd;

	if( gImg_Conv_2_Param_Status == FALSE ){
		// not necessary close process
		return FJ_ERR_OK;
	}

	// check IIP action
	while( gImg_Conv_2_Act_Check_Flag == TRUE ){
		OS_User_Dly_Tsk(1);
	}

	// close process
	fj_ercd = BF_Still_Proc_Execute_AfnConvertImage_2_Close();
	if( fj_ercd != FJ_ERR_OK ){
		BF_Debug_Print_Error(( "BF_Still_Image_Convert_2_Close: no=%u ercd=%d\n", fj_ercd, 1 ));
	}

	gImg_Conv_2_Param_Status = FALSE;

	return fj_ercd;
}

FJ_ERR_CODE BF_Still_FJ_ImageConvert( FJ_IMG_CONV_CTRL* conv_info, const T_IMAGE_SIZE* src_correct_aspect_ratio )
{
	T_BF_STILL_IIP_IMG_CONVERT		img_info;
	T_BF_STILL_IMG_CONV				src =	{
												.type	=	E_BF_STILL_IMG_TYPE_YCC420,
												.plane	=	E_BF_STILL_PLANE_TYPE_C_INTL,
												.depth	=	E_BF_STILL_IMG_DEPTH_U8,
												.addr	=	{
																.y_g	= 0,
																.cb_b	= 0,
																.cr_r	= 0,
																.alpha	= 0,
															},
												.pixs	=	{
																.width	= 0,
																.lines	= 0,
															},
												.g_width=	{
																.y_g	= 0,
																.cb_b	= 0,
																.cr_r	= 0,
																.alpha	= 0,
															},
												.line_bytes={
																.y_g	= 0,
																.cb_b	= 0,
																.cr_r	= 0,
																.alpha	= 0,
															},
											};
	T_BF_STILL_IMG_CONV				dst;
	T_BF_STILL_IMG_CROP				crop;
	UCHAR							fill_en = 0;
	// for Rotation
	T_RESIZE_IMG					rotate_src;
	T_RESIZE_IMG					rotate_dst;
	E_IM_IIP_UTIL_ROTATE			rotation;
	USHORT							tmp_width, tmp_lines;

	if( (conv_info->dst_img.pfmt > FJ_IMG_PFMT_YCC400) ||
	    (conv_info->rotate_deg == FJ_IMG_ROTATE_DEGREE_0) ){
	
		// Convert the param argument
		bf_still_capture_convert_img_info_rgb( &conv_info->src_img, &src );
		bf_still_capture_convert_img_info_rgb( &conv_info->dst_img, &dst );

		crop.sta_x	= 0.0;
		crop.sta_y	= 0.0;
		crop.width	= (DOUBLE)src.pixs.width;
		crop.lines	= (DOUBLE)src.pixs.lines;

		fill_en = conv_info->fill_enable;

		// Fill Enable.
		if ( fill_en ) {
			bf_still_capture_calc_crop_rgb( &crop, &src, src_correct_aspect_ratio, &dst );
		}
		// Fill Disable.
		else {
			// Do Nothing.
		}

		img_info.src						= src;
		img_info.dst						= dst;
		img_info.crop_rect					= crop;
		img_info.fill_videofmt_alignment	= TRUE;
        
		if ( conv_info->resize_mode == FJ_IMG_RESIZE_MODE_BILINEAR ) {
			img_info.itmd	= E_BF_STILL_IIP_ITMD_BILINEAR;
		}
		else {
			img_info.itmd	= E_BF_STILL_IIP_ITMD_BICUBIC;
		}
        
		img_info.src_mif	= 0;
		img_info.dst_mif	= 0;
		img_info.rotate_deg = conv_info->rotate_deg;
		img_info.mirror		= FALSE;

		return BF_Still_Proc_Execute_AfnConvertImage( &img_info, fill_en, conv_info->timeout );
	}
	else {
		
		// Check File Format (YUV only)
		if( conv_info->dst_img.pfmt > FJ_IMG_PFMT_YCC400 ){
			return FJ_ERR_NG;
		}
		
		// Convert the param argument
		bf_still_capture_convert_img_info_rgb( &conv_info->src_img, &src );
		bf_still_capture_convert_img_info_rgb( &conv_info->dst_img, &dst );
		
		switch( conv_info->rotate_deg ){
			case  FJ_IMG_ROTATE_DEGREE_90:
				rotation = E_IM_IIP_UTIL_ROTATE_090;
				break;
			
			case  FJ_IMG_ROTATE_DEGREE_180:
				rotation = E_IM_IIP_UTIL_ROTATE_180;
				break;

			case  FJ_IMG_ROTATE_DEGREE_270:
				rotation = E_IM_IIP_UTIL_ROTATE_270;
				break;

			case  FJ_IMG_ROTATE_DEGREE_0:
			default:
				// impossible route
				rotation = E_IM_IIP_UTIL_ROTATE_000;	// for safety
				break;
		}
		
		// Src Settings
		rotate_src.rect.top			= 0.0;
		rotate_src.rect.left		= 0.0;
		rotate_src.rect.width		= src.pixs.width;
		rotate_src.rect.lines		= src.pixs.lines;
		rotate_src.img.type			= src.type;
		rotate_src.img.plane		= src.plane;
		rotate_src.img.depth		= src.depth;
		rotate_src.img.addr.y		= src.addr.y_g;
		rotate_src.img.addr.cb		= src.addr.cb_b;
		rotate_src.img.addr.cr		= src.addr.cr_r;
		rotate_src.img.pixs			= src.pixs;
		rotate_src.img.g_width.y	= src.g_width.y_g;
		rotate_src.img.g_width.cb	= src.g_width.cb_b;
		rotate_src.img.g_width.cr	= src.g_width.cr_r;
		
		// Dst Settings
		if( (rotation != E_IM_IIP_UTIL_ROTATE_090) && (rotation != E_IM_IIP_UTIL_ROTATE_270) ){
			tmp_width = dst.pixs.width;
			tmp_lines = dst.pixs.lines;
		}
		else {
			tmp_width = dst.pixs.lines;
			tmp_lines = dst.pixs.width;
		}
		rotate_dst.rect.top			= 0.0;
		rotate_dst.rect.left		= 0.0;
		rotate_dst.rect.width		= tmp_width;
		rotate_dst.rect.lines		= tmp_lines;
		rotate_dst.img.type			= dst.type;
		rotate_dst.img.plane		= dst.plane;
		rotate_dst.img.depth		= dst.depth;
		rotate_dst.img.addr.y		= dst.addr.y_g;
		rotate_dst.img.addr.cb		= dst.addr.cb_b;
		rotate_dst.img.addr.cr		= dst.addr.cr_r;
		rotate_dst.img.pixs.width	= tmp_width;
		rotate_dst.img.pixs.lines	= tmp_lines;
		rotate_dst.img.g_width.y	= M_BF_STILL_ROUNDUP_16(tmp_width);
		rotate_dst.img.g_width.cb	= M_BF_STILL_ROUNDUP_16(tmp_width);
		rotate_dst.img.g_width.cr	= M_BF_STILL_ROUNDUP_16(tmp_width);

#if 0	// for Debug
	BF_Debug_Print_Information(("I:[IMAGE Convert(Rotate ON) INFORMATION]\n"));
	BF_Debug_Print_Information(("---- src info ----\n"));
	BF_Debug_Print_Information((" rect.top		= %f\n", rotate_src.rect.top));
	BF_Debug_Print_Information((" rect.left		= %f\n", rotate_src.rect.left));
	BF_Debug_Print_Information((" rect.width		= %d\n", rotate_src.rect.width));
	BF_Debug_Print_Information((" rect.lines		= %d\n", rotate_src.rect.lines));
	BF_Debug_Print_Information((" img.type		= %d\n", rotate_src.img.type));
	BF_Debug_Print_Information((" img.plane		= %d\n", rotate_src.img.plane));
	BF_Debug_Print_Information((" img.depth		= %d\n", rotate_src.img.depth));
	BF_Debug_Print_Information((" img.addr.y		= 0x%X\n", rotate_src.img.addr.y));
	BF_Debug_Print_Information((" img.addr.cb		= 0x%X\n", rotate_src.img.addr.cb));
	BF_Debug_Print_Information((" img.addr.cr		= 0x%X\n", rotate_src.img.addr.cr));
	BF_Debug_Print_Information((" img.pixs.width	= %d\n", rotate_src.img.pixs.width));
	BF_Debug_Print_Information((" img.pixs.lines	= %d\n", rotate_src.img.pixs.lines));
	BF_Debug_Print_Information((" img.g_width.y	= %d\n", rotate_src.img.g_width.y));
	BF_Debug_Print_Information((" img.g_width.cb	= %d\n", rotate_src.img.g_width.cb));
	BF_Debug_Print_Information((" img.g_width.cr	= %d\n", rotate_src.img.g_width.cr));
	BF_Debug_Print_Information(("---- dst info ----\n"));
	BF_Debug_Print_Information((" rect.top		= %f\n", rotate_dst.rect.top));
	BF_Debug_Print_Information((" rect.left		= %f\n", rotate_dst.rect.left));
	BF_Debug_Print_Information((" rect.width		= %d\n", rotate_dst.rect.width));
	BF_Debug_Print_Information((" rect.lines		= %d\n", rotate_dst.rect.lines));
	BF_Debug_Print_Information((" img.type		= %d\n", rotate_dst.img.type));
	BF_Debug_Print_Information((" img.plane		= %d\n", rotate_dst.img.plane));
	BF_Debug_Print_Information((" img.depth		= %d\n", rotate_dst.img.depth));
	BF_Debug_Print_Information((" img.addr.y		= 0x%X\n", rotate_dst.img.addr.y));
	BF_Debug_Print_Information((" img.addr.cb		= 0x%X\n", rotate_dst.img.addr.cb));
	BF_Debug_Print_Information((" img.addr.cr		= 0x%X\n", rotate_dst.img.addr.cr));
	BF_Debug_Print_Information((" img.pixs.width	= %d\n", rotate_dst.img.pixs.width));
	BF_Debug_Print_Information((" img.pixs.lines	= %d\n", rotate_dst.img.pixs.lines));
	BF_Debug_Print_Information((" img.g_width.y	= %d\n", rotate_dst.img.g_width.y));
	BF_Debug_Print_Information((" img.g_width.cb	= %d\n", rotate_dst.img.g_width.cb));
	BF_Debug_Print_Information((" img.g_width.cr	= %d\n", rotate_dst.img.g_width.cr));
#endif
		
		return BF_Still_Resize_Image( &rotate_src, &rotate_dst, E_RESIZE_MODE_NEAREST, rotation);
	}
}

