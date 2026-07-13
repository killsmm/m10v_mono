/**
 * @file		ct_im_shdr.c
 * @brief		This is ct code for im_shdr.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ct_im_shdr.h"

#include "jdsshdr.h"
#include "im_shdr.h"
#include <string.h>
#include <stdlib.h>

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
#endif //CO_ACT_CLOCK | CO_ACT_HCLOCK | CO_ACT_PCLOCK

#include "ddim_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define PC_DEBUG

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// nothing special.

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	T_IM_SHDR_CTRL		ctrl;
	T_IM_SHDR_PRE		pre;
	T_IM_SHDR_FMD		fmd;
	T_IM_SHDR_PMSK		pmsk;
	T_IM_SHDR_SPNR		spnr;
	T_IM_SHDR_MSK		msk;
	T_IM_SHDR_BLD		bld;
	T_IM_SHDR_POST		post;
	T_IM_SHDR_RESIZE	resize;
	T_IM_SHDR_SMC		smc;
	T_IM_SHDR_AXI		axi;
} T_IM_SHDR_PARAM;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
ULONG gct_im_shdr_in_addr[ 5 ] = {
	0x48000000,
	0x49000000,
	0x4A000000,
	0x4B000000,
	0x4C000000,
};

ULONG gct_im_shdr_out_addr[ 6 ]	= {
	0x4D000000,
	0x4E000000,
	0x4F000000,
	0x50000000,
	0x51000000,
	0x52000000,
};

/*----------------------------------------------------------------------*/
/* Local Method															*/
/*----------------------------------------------------------------------*/
static VOID ct_im_shdr_callback( VOID )
{
	Ddim_Print(("ct_im_shdr_callback\n"));
}

static VOID ct_im_shdr_lineint_callback( VOID )
{
	Ddim_Print(("ct_im_shdr_lineint_callback\n"));
}

static VOID ct_im_shdr_set_ctrl( VOID )
{
	UINT32			loopcnt;
	INT32			ret;
	T_IM_SHDR_CTRL	ctrl[] = {
		[ 0 ] = {	// Max.
			.image_set.in_bit_depth		= D_IM_SHDR_INBITDEPTH_16,
			.image_set.in_c_fmt			= D_IM_SHDR_INCFMT_422SP,
			.image_set.in_edge_cpy		= D_IM_SHDR_ON,
			.image_set.in_edge_cut		= D_IM_SHDR_ON,
			.image_set.out_ctrl			= D_IM_SHDR_OUTCTRL_RGB,
			.image_set.out_sel			= D_IM_SHDR_OUTSEL_RGB_16,
			.image_set.out_sel_en		= D_IM_SHDR_ENABLE,
			.image_mode.exp_mag			= 16383,
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			.image_mode.fil_margin		= D_IM_SHDR_FLITER_MARGIN_96,
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
			.image_mode.fil_margin		= D_IM_SHDR_FLITER_MARGIN_32,
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
			.image_mode.rexp_mag_shift	= 5,
			.image_mode.rexp_mag		= 65536,
			.image_mode.shift_x			= 4094,
			.image_mode.shift_y			= 4094,
			.image_mode.fmd_en			= D_IM_SHDR_ENABLE,
			.image_mode.bypass_en		= D_IM_SHDR_ENABLE,
			.image_mode.smc_in			= D_IM_SHDR_SMCIN_EXT_REF,
			.image_mode.shdr_in			= D_IM_SHDR_SHDRIN_SMCOUT_REF,
			.image_mode.tile_size		= D_IM_SHDR_TILESIZE_1024x512,
			.sdram.r_addr.a_y			= 0xFFFFFFFF,
			.sdram.r_addr.a_c			= 0xFFFFFFFF,
			.sdram.r_addr.b_y			= 0xFFFFFFFF,
			.sdram.r_addr.b_c			= 0xFFFFFFFF,
			.sdram.r_addr.c_y			= 0xFFFFFFFF,
			.sdram.r_addr.c_c			= 0xFFFFFFFF,
			.sdram.r_addr.d_y			= 0xFFFFFFFF,
			.sdram.r_addr.d_c			= 0xFFFFFFFF,
			.sdram.r_addr.e_y			= 0xFFFFFFFF,
			.sdram.r_addr.e_c			= 0xFFFFFFFF,
			.sdram.r_addr.f				= 0xFFFFFFFF,
			.sdram.r_ghsize.a_y			= 0xFFF8,
			.sdram.r_ghsize.a_c			= 0xFFF8,
			.sdram.r_ghsize.b_y			= 0xFFF8,
			.sdram.r_ghsize.b_c			= 0xFFF8,
			.sdram.r_ghsize.c_y			= 0xFFF8,
			.sdram.r_ghsize.c_c			= 0xFFF8,
			.sdram.r_ghsize.d_y			= 0xFFF8,
			.sdram.r_ghsize.d_c			= 0xFFF8,
			.sdram.r_ghsize.e_y			= 0xFFF8,
			.sdram.r_ghsize.e_c			= 0xFFF8,
			.sdram.r_ghsize.f			= 0xFFF8,
			.sdram.r_hsize.a			= 11992,
			.sdram.r_hsize.b			= 11992,
			.sdram.r_hsize.c			= 11992,
			.sdram.r_hsize.d			= 11992,
			.sdram.r_hsize.e			= 11992,
			.sdram.r_vsize.a			= 8992,
			.sdram.r_vsize.b			= 8992,
			.sdram.r_vsize.c			= 8992,
			.sdram.r_vsize.d			= 8992,
			.sdram.r_vsize.e			= 8992,
			.sdram.r_vfm.a				= D_IM_SHDR_VFMT_VIDEO,
			.sdram.w_addr.b_y			= 0xFFFFFFF8,
			.sdram.w_addr.b_c			= 0xFFFFFFF8,
			.sdram.w_addr.e_y			= 0xFFFFFFF8,
			.sdram.w_addr.e_c			= 0xFFFFFFF8,
			.sdram.w_addr.f_y			= 0xFFFFFFF8,
			.sdram.w_addr.f_c			= 0xFFFFFFF8,
			.sdram.w_addr.g_r_y			= 0xFFFFFFF8,
			.sdram.w_addr.g_g_c			= 0xFFFFFFF8,
			.sdram.w_addr.g_b			= 0xFFFFFFF8,
			.sdram.w_addr.h_x			= 0xFFFFFFF8,
			.sdram.w_addr.h_y			= 0xFFFFFFF8,
			.sdram.w_addr.i				= 0xFFFFFFF8,
			.sdram.w_ghsize.b_y			= 0xFFF8,
			.sdram.w_ghsize.b_c			= 0xFFF8,
			.sdram.w_ghsize.e_y			= 0xFFF8,
			.sdram.w_ghsize.e_c			= 0xFFF8,
			.sdram.w_ghsize.f_y			= 0xFFF8,
			.sdram.w_ghsize.f_c			= 0xFFF8,
			.sdram.w_ghsize.g_r_y		= 0xFFF8,
			.sdram.w_ghsize.g_g_c		= 0xFFF8,
			.sdram.w_ghsize.g_b			= 0xFFF8,
			.sdram.w_ghsize.h_x			= 0xFFF8,
			.sdram.w_ghsize.h_y			= 0xFFF8,
			.sdram.w_ghsize.i			= 0xFFF8,
			.sdram.w_vfm.a				= D_IM_SHDR_VFMT_COMPIP_VIDEO,
			.sdram.w_vfm.b				= D_IM_SHDR_VFMT_VIDEO,
			.sdram.w_baddr.a_y			= 0xFFFFC000,
			.sdram.w_baddr.a_c			= 0xFFFFC000,
			.sdram.w_baddr.c_y			= 0xFFFFC000,
			.sdram.w_baddr.c_c			= 0xFFFFC000,
			.sdram.w_baddr.d_y			= 0xFFFFC000,
			.sdram.w_baddr.d_c			= 0xFFFFC000,
			.sdram.w_hsize.a			= 0xFF00,
			.sdram.w_hsize.c			= 0xFF00,
			.sdram.w_hsize.d			= 0xFF00,
			.compip.mode				= D_IM_SHDR_COMP_MODE_LOSSY,
			.compip.mc_mode				= D_IM_SHDR_COMP_MC_UNUSED,
			.compip.comp_incfmt			= D_IM_SHDR_COMP_INCFMT_YUV420,
			.compip.r_bypass_mode		= D_IM_SHDR_ENABLE,
			.compip.w_bypass_mode		= D_IM_SHDR_ENABLE,
			.compip.r_img_axi_size		= D_IM_SHDR_COMP_AXI_512,
			.compip.r_comp_axi_size		= D_IM_SHDR_COMP_AXI_512,
			.compip.w_img_axi_size		= D_IM_SHDR_COMP_AXI_512,
			.compip.lint_no				= 0,
			.compip.lint_mask			= D_IM_SHDR_ENABLE,
			.compip.lint_sts			= D_IM_SHDR_DISABLE,
			.compip.w_addr_y			= 0,
			.compip.w_addr_c			= 0,
			.compip.r_addr_y			= 0,
			.compip.r_addr_c			= 0,
			.pCallBack					= (T_IM_SHDR_CALLBACK)ct_im_shdr_callback,
			.pLineInt_CallBack			= (T_IM_SHDR_LINEINT_CALLBACK)ct_im_shdr_lineint_callback,
		},
#if 0
		[ 1 ] = {	// Min.
			.image_set.in_bit_depth		= D_IM_SHDR_INBITDEPTH_8,
			.image_set.in_c_fmt			= D_IM_SHDR_INCFMT_420SP,
			.image_set.in_edge_cpy		= D_IM_SHDR_OFF,
			.image_set.in_edge_cut		= D_IM_SHDR_OFF,
			.image_set.out_ctrl			= D_IM_SHDR_OUTCTRL_YCC,
			.image_set.out_sel			= D_IM_SHDR_OUTSEL_YCC_16,
			.image_set.out_sel_en		= D_IM_SHDR_DISABLE,
			.image_mode.exp_mag			= 1,
			.image_mode.fil_margin		= D_IM_SHDR_FLITER_MARGIN_16,
			.image_mode.rexp_mag_shift	= -8,
			.image_mode.rexp_mag		= 0,
			.image_mode.shift_x			= -4096,
			.image_mode.shift_y			= -4096,
			.image_mode.fmd_en			= D_IM_SHDR_DISABLE,
			.image_mode.bypass_en		= D_IM_SHDR_DISABLE,
			.image_mode.smc_in			= D_IM_SHDR_SMCIN_STN_REF,
			.image_mode.shdr_in			= D_IM_SHDR_SHDRIN_STN_REF,
			.image_mode.tile_size		= D_IM_SHDR_TILESIZE_1024x1024,
			.sdram.r_addr.a_y			= 0x00000000,
			.sdram.r_addr.a_c			= 0x00000000,
			.sdram.r_addr.b_y			= 0x00000000,
			.sdram.r_addr.b_c			= 0x00000000,
			.sdram.r_addr.c_y			= 0x00000000,
			.sdram.r_addr.c_c			= 0x00000000,
			.sdram.r_addr.d_y			= 0x00000000,
			.sdram.r_addr.d_c			= 0x00000000,
			.sdram.r_addr.e_y			= 0x00000000,
			.sdram.r_addr.e_c			= 0x00000000,
			.sdram.r_addr.f				= 0x00000000,
			.sdram.r_ghsize.a_y			= 0x0008,
			.sdram.r_ghsize.a_c			= 0x0008,
			.sdram.r_ghsize.b_y			= 0x0008,
			.sdram.r_ghsize.b_c			= 0x0008,
			.sdram.r_ghsize.c_y			= 0x0008,
			.sdram.r_ghsize.c_c			= 0x0008,
			.sdram.r_ghsize.d_y			= 0x0008,
			.sdram.r_ghsize.d_c			= 0x0008,
			.sdram.r_ghsize.e_y			= 0x0008,
			.sdram.r_ghsize.e_c			= 0x0008,
			.sdram.r_ghsize.f			= 0x0008,
			.sdram.r_hsize.a			= 4,
			.sdram.r_hsize.b			= 4,
			.sdram.r_hsize.c			= 4,
			.sdram.r_hsize.d			= 4,
			.sdram.r_hsize.e			= 4,
			.sdram.r_vsize.a			= 4,
			.sdram.r_vsize.b			= 4,
			.sdram.r_vsize.c			= 4,
			.sdram.r_vsize.d			= 4,
			.sdram.r_vsize.e			= 4,
			.sdram.r_vfm.a				= D_IM_SHDR_VFMT_RASTER,
			.sdram.w_addr.b_y			= 0x00000000,
			.sdram.w_addr.b_c			= 0x00000000,
			.sdram.w_addr.e_y			= 0x00000000,
			.sdram.w_addr.e_c			= 0x00000000,
			.sdram.w_addr.f_y			= 0x00000000,
			.sdram.w_addr.f_c			= 0x00000000,
			.sdram.w_addr.g_r_y			= 0x00000000,
			.sdram.w_addr.g_g_c			= 0x00000000,
			.sdram.w_addr.g_b			= 0x00000000,
			.sdram.w_addr.h_x			= 0x00000000,
			.sdram.w_addr.h_y			= 0x00000000,
			.sdram.w_addr.i				= 0x00000000,
			.sdram.w_ghsize.b_y			= 0x0001,
			.sdram.w_ghsize.b_c			= 0x0001,
			.sdram.w_ghsize.e_y			= 0x0001,
			.sdram.w_ghsize.e_c			= 0x0001,
			.sdram.w_ghsize.f_y			= 0x0001,
			.sdram.w_ghsize.f_c			= 0x0001,
			.sdram.w_ghsize.g_r_y		= 0x0001,
			.sdram.w_ghsize.g_g_c		= 0x0001,
			.sdram.w_ghsize.g_b			= 0x0001,
			.sdram.w_ghsize.h_x			= 0x0001,
			.sdram.w_ghsize.h_y			= 0x0001,
			.sdram.w_ghsize.i			= 0x0001,
			.sdram.w_vfm.a				= D_IM_SHDR_VFMT_COMPIP_VIDEO,
			.sdram.w_vfm.b				= D_IM_SHDR_VFMT_RASTER,
			.sdram.w_baddr.a_y			= 0x00000000,
			.sdram.w_baddr.a_c			= 0x00000000,
			.sdram.w_baddr.c_y			= 0x00000000,
			.sdram.w_baddr.c_c			= 0x00000000,
			.sdram.w_baddr.d_y			= 0x00000000,
			.sdram.w_baddr.d_c			= 0x00000000,
			.sdram.w_hsize.a			= 0,
			.sdram.w_hsize.c			= 0,
			.sdram.w_hsize.d			= 0,
			.pCallBack					= (T_IM_SHDR_CALLBACK)ct_im_shdr_callback,
			.pLineInt_CallBack			= (T_IM_SHDR_LINEINT_CALLBACK)ct_im_shdr_lineint_callback,
		},
#endif
	};

	for( loopcnt = 0; loopcnt < (sizeof(ctrl) / sizeof(ctrl[0])); loopcnt++ ) {
		ret = Im_SHDR_Ctrl_Common( &ctrl[ loopcnt ] );
		Ddim_Print(("ct_im_shdr_set_ctrl(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_set_pre( VOID )
{
	UINT32			loopcnt;
	INT32			ret;
	T_IM_SHDR_PRE	pre[] = {
		[ 0 ] = {	// Max.
			.pre_mode.cus_mode			= D_IM_SHDR_CUS_YCC420_PADDING,
			.pre_mode.degamma_mode		= D_IM_SHDR_DEGAMMA_BITEXP,
			.y2r_coef[ 0 ]				= 0x3FFF,
			.y2r_coef[ 1 ]				= 0x3FFF,
			.y2r_coef[ 2 ]				= 0x3FFF,
			.y2r_coef[ 3 ]				= 0x3FFF,
			.y2r_coef[ 4 ]				= 0x3FFF,
			.y2r_coef[ 5 ]				= 0x3FFF,
			.y2r_coef[ 6 ]				= 0x3FFF,
			.y2r_coef[ 7 ]				= 0x3FFF,
			.y2r_coef[ 8 ]				= 0x3FFF,
			.y2r_clp.upper_long0[ 0 ]	= 0xFFFF,
			.y2r_clp.upper_long0[ 1 ]	= 0xFFFF,
			.y2r_clp.upper_long0[ 2 ]	= 0xFFFF,
			.y2r_clp.lower_long0[ 0 ]	= 0xFFFF,
			.y2r_clp.lower_long0[ 1 ]	= 0xFFFF,
			.y2r_clp.lower_long0[ 2 ]	= 0xFFFF,
			.y2r_clp.upper_short[ 0 ]	= 0xFF,
			.y2r_clp.upper_short[ 1 ]	= 0xFF,
			.y2r_clp.upper_short[ 2 ]	= 0xFF,
			.y2r_clp.lower_short[ 0 ]	= 0xFF,
			.y2r_clp.lower_short[ 1 ]	= 0xFF,
			.y2r_clp.lower_short[ 2 ]	= 0xFF,
			.r2y_coef[ 0 ]				= 0x3FFF,
			.r2y_coef[ 1 ]				= 0x3FFF,
			.r2y_coef[ 2 ]				= 0x3FFF,
			.r2y_coef[ 3 ]				= 0x3FFF,
			.r2y_coef[ 4 ]				= 0x3FFF,
			.r2y_coef[ 5 ]				= 0x3FFF,
			.r2y_coef[ 6 ]				= 0x3FFF,
			.r2y_coef[ 7 ]				= 0x3FFF,
			.r2y_coef[ 8 ]				= 0x3FFF,
			.r2y_clp.upper_long_y		= 0xFFFF,
			.r2y_clp.upper_long_c[ 0 ]	= 0x7FFF,
			.r2y_clp.upper_long_c[ 1 ]	= 0x7FFF,
			.r2y_clp.lower_long_y		= 0xFFFF,
			.r2y_clp.lower_long_c[ 0 ]	= 0x7FFF,
			.r2y_clp.lower_long_c[ 1 ]	= 0x7FFF,
			.r2y_clp.upper_short_y		= 0xFFF,
			.r2y_clp.upper_short_c[ 0 ]	= 0x7FF,
			.r2y_clp.upper_short_c[ 1 ]	= 0x7FF,
			.r2y_clp.lower_short_y		= 0xFFF,
			.r2y_clp.lower_short_c[ 0 ]	= 0x7FF,
			.r2y_clp.lower_short_c[ 1 ]	= 0x7FF,
			.short_knee.offset[ 0 ]		= 0xFF,
			.short_knee.offset[ 1 ]		= 0xFF,
			.short_knee.offset[ 2 ]		= 0xFF,
			.short_knee.slope[ 0 ]		= 0xFF,
			.short_knee.slope[ 1 ]		= 0xFF,
			.short_knee.slope[ 2 ]		= 0xFF,
			.short_knee.boundary[ 0 ]	= 0x3FF,
			.short_knee.boundary[ 1 ]	= 0x3FF,
			.long_knee.offset[ 0 ]		= 0xFF,
			.long_knee.offset[ 1 ]		= 0xFF,
			.long_knee.offset[ 2 ]		= 0xFF,
			.long_knee.slope[ 0 ]		= 0xFF,
			.long_knee.slope[ 1 ]		= 0xFF,
			.long_knee.slope[ 2 ]		= 0xFF,
			.long_knee.boundary[ 0 ]	= 0x3FF,
			.long_knee.boundary[ 1 ]	= 0x3FF,
		},
#if 0
		[ 1 ] = {	// Min.
			.pre_mode.cus_mode			= D_IM_SHDR_CUS_YCC422_BILINEAR_A,
			.pre_mode.degamma_mode		= D_IM_SHDR_DEGAMMA_LUT,
			.y2r_coef[ 0 ]				= -0x4000,
			.y2r_coef[ 1 ]				= -0x4000,
			.y2r_coef[ 2 ]				= -0x4000,
			.y2r_coef[ 3 ]				= -0x4000,
			.y2r_coef[ 4 ]				= -0x4000,
			.y2r_coef[ 5 ]				= -0x4000,
			.y2r_coef[ 6 ]				= -0x4000,
			.y2r_coef[ 7 ]				= -0x4000,
			.y2r_coef[ 8 ]				= -0x4000,
			.y2r_clp.upper_long0[ 0 ]	= 0x0000,
			.y2r_clp.upper_long0[ 1 ]	= 0x0000,
			.y2r_clp.upper_long0[ 2 ]	= 0x0000,
			.y2r_clp.lower_long0[ 0 ]	= 0x0000,
			.y2r_clp.lower_long0[ 1 ]	= 0x0000,
			.y2r_clp.lower_long0[ 2 ]	= 0x0000,
			.y2r_clp.upper_short[ 0 ]	= 0x00,
			.y2r_clp.upper_short[ 1 ]	= 0x00,
			.y2r_clp.upper_short[ 2 ]	= 0x00,
			.y2r_clp.lower_short[ 0 ]	= 0x00,
			.y2r_clp.lower_short[ 1 ]	= 0x00,
			.y2r_clp.lower_short[ 2 ]	= 0x00,
			.r2y_coef[ 0 ]				= -0x4000,
			.r2y_coef[ 1 ]				= -0x4000,
			.r2y_coef[ 2 ]				= -0x4000,
			.r2y_coef[ 3 ]				= -0x4000,
			.r2y_coef[ 4 ]				= -0x4000,
			.r2y_coef[ 5 ]				= -0x4000,
			.r2y_coef[ 6 ]				= -0x4000,
			.r2y_coef[ 7 ]				= -0x4000,
			.r2y_coef[ 8 ]				= -0x4000,
			.r2y_clp.upper_long_y		= 0x0000,
			.r2y_clp.upper_long_c[ 0 ]	= -0x8000,
			.r2y_clp.upper_long_c[ 1 ]	= -0x8000,
			.r2y_clp.lower_long_y		= 0x0000,
			.r2y_clp.lower_long_c[ 0 ]	= -0x8000,
			.r2y_clp.lower_long_c[ 1 ]	= -0x8000,
			.r2y_clp.upper_short_y		= 0x000,
			.r2y_clp.upper_short_c[ 0 ]	= -0x800,
			.r2y_clp.upper_short_c[ 1 ]	= -0x800,
			.r2y_clp.lower_short_y		= 0x000,
			.r2y_clp.lower_short_c[ 0 ]	= -0x800,
			.r2y_clp.lower_short_c[ 1 ]	= -0x800,
			.short_knee.offset[ 0 ]		= 0x00,
			.short_knee.offset[ 1 ]		= 0x00,
			.short_knee.offset[ 2 ]		= 0x00,
			.short_knee.slope[ 0 ]		= 0x00,
			.short_knee.slope[ 1 ]		= 0x00,
			.short_knee.slope[ 2 ]		= 0x00,
			.short_knee.boundary[ 0 ]	= 0x000,
			.short_knee.boundary[ 1 ]	= 0x000,
			.long_knee.offset[ 0 ]		= 0x00,
			.long_knee.offset[ 1 ]		= 0x00,
			.long_knee.offset[ 2 ]		= 0x00,
			.long_knee.slope[ 0 ]		= 0x00,
			.long_knee.slope[ 1 ]		= 0x00,
			.long_knee.slope[ 2 ]		= 0x00,
			.long_knee.boundary[ 0 ]	= 0x000,
			.long_knee.boundary[ 1 ]	= 0x000,
		},
#endif
	};

	for( loopcnt = 0; loopcnt < (sizeof(pre) / sizeof(pre[0])); loopcnt++ ) {
		ret = Im_SHDR_Set_Pre( &pre[ loopcnt ] );
		Ddim_Print(("ct_im_shdr_set_pre(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_set_fmd( VOID )
{
	UINT32			loopcnt;
	INT32			ret;
	T_IM_SHDR_FMD	fmd[] = {
		[ 0 ] = {	// Max.
			.ep_boundary[ 0 ]			= 0xFF,
			.ep_boundary[ 1 ]			= 0xFF,
			.ep_boundary[ 2 ]			= 0xFF,
			.ep_boundary[ 3 ]			= 0xFF,
			.ep_offset[ 0 ]				= 0xFF,
			.ep_offset[ 1 ]				= 0xFF,
			.ep_offset[ 2 ]				= 0xFF,
			.ep_offset[ 3 ]				= 0xFF,
			.ep_blend					= 0x7,
			.fd_weight					= 0x1F,
			.mvcnt.bounds				= 0xFF,
			.mvcnt.threshold			= 0xFF,
			.mvcnt.gr					= 0xFFF,
			.bldr_offset[ 0 ]			= 0xFF,
			.bldr_offset[ 1 ]			= 0xFF,
			.bldr_offset[ 2 ]			= 0xFF,
			.bldr_offset[ 3 ]			= 0xFF,
			.bldr_offset[ 4 ]			= 0xFF,
			.bldr_slope[ 0 ]			= 0xFFF,
			.bldr_slope[ 1 ]			= 0xFFF,
			.bldr_slope[ 2 ]			= 0xFFF,
			.bldr_slope[ 3 ]			= 0xFFF,
			.bldr_slope[ 4 ]			= 0xFFF,
			.bldr_boundary[ 0 ]			= 0xFF,
			.bldr_boundary[ 1 ]			= 0xFF,
			.bldr_boundary[ 2 ]			= 0xFF,
			.bldr_boundary[ 3 ]			= 0xFF,
			.bldr_boundary[ 4 ]			= 0xFF,
			.weight_r2y_coef[ 0 ]		= 0x3FFF,
			.weight_r2y_coef[ 1 ]		= 0x3FFF,
			.weight_r2y_coef[ 2 ]		= 0x3FFF,
			.weight_r2y_clp.upper		= 0xFF,
			.weight_r2y_clp.lower		= 0xFF,
			.weight_base				= D_IM_SHDR_WGHTBASE_RGB,
			.weight_offset[ 0 ][ 0 ]	= 0xFFF,
			.weight_offset[ 0 ][ 1 ]	= 0xFFF,
			.weight_offset[ 0 ][ 2 ]	= 0xFFF,
			.weight_offset[ 0 ][ 3 ]	= 0xFFF,
			.weight_offset[ 0 ][ 4 ]	= 0xFFF,
			.weight_offset[ 1 ][ 0 ]	= 0xFFF,
			.weight_offset[ 1 ][ 1 ]	= 0xFFF,
			.weight_offset[ 1 ][ 2 ]	= 0xFFF,
			.weight_offset[ 1 ][ 3 ]	= 0xFFF,
			.weight_offset[ 1 ][ 4 ]	= 0xFFF,
			.weight_offset[ 2 ][ 0 ]	= 0xFFF,
			.weight_offset[ 2 ][ 1 ]	= 0xFFF,
			.weight_offset[ 2 ][ 2 ]	= 0xFFF,
			.weight_offset[ 2 ][ 3 ]	= 0xFFF,
			.weight_offset[ 2 ][ 4 ]	= 0xFFF,
			.weight_slope[ 0 ][ 0 ]		= 0xFFF,
			.weight_slope[ 0 ][ 1 ]		= 0xFFF,
			.weight_slope[ 0 ][ 2 ]		= 0xFFF,
			.weight_slope[ 0 ][ 3 ]		= 0xFFF,
			.weight_slope[ 0 ][ 4 ]		= 0xFFF,
			.weight_slope[ 1 ][ 0 ]		= 0xFFF,
			.weight_slope[ 1 ][ 1 ]		= 0xFFF,
			.weight_slope[ 1 ][ 2 ]		= 0xFFF,
			.weight_slope[ 1 ][ 3 ]		= 0xFFF,
			.weight_slope[ 1 ][ 4 ]		= 0xFFF,
			.weight_slope[ 2 ][ 0 ]		= 0xFFF,
			.weight_slope[ 2 ][ 1 ]		= 0xFFF,
			.weight_slope[ 2 ][ 2 ]		= 0xFFF,
			.weight_slope[ 2 ][ 3 ]		= 0xFFF,
			.weight_slope[ 2 ][ 4 ]		= 0xFFF,
//			.weight_boundary[ 0 ][ 0 ]	= 0xFF,
			.weight_boundary[ 0 ][ 1 ]	= 0xFF,
			.weight_boundary[ 0 ][ 2 ]	= 0xFF,
			.weight_boundary[ 0 ][ 3 ]	= 0xFF,
			.weight_boundary[ 0 ][ 4 ]	= 0xFF,
//			.weight_boundary[ 1 ][ 0 ]	= 0xFF,
			.weight_boundary[ 1 ][ 1 ]	= 0xFF,
			.weight_boundary[ 1 ][ 2 ]	= 0xFF,
			.weight_boundary[ 1 ][ 3 ]	= 0xFF,
			.weight_boundary[ 1 ][ 4 ]	= 0xFF,
//			.weight_boundary[ 2 ][ 0 ]	= 0xFF,
			.weight_boundary[ 2 ][ 1 ]	= 0xFF,
			.weight_boundary[ 2 ][ 2 ]	= 0xFF,
			.weight_boundary[ 2 ][ 3 ]	= 0xFF,
			.weight_boundary[ 2 ][ 4 ]	= 0xFF,
			.iso_threshold				= 0x1FF,
			.iso_coef[ 0 ]				= 0x3,
			.iso_coef[ 1 ]				= 0x3,
			.iso_coef[ 2 ]				= 0x3,
			.iso_coef[ 3 ]				= 0x1F,
			.iso_coef[ 4 ]				= 0x1F,
			.iso_coef[ 5 ]				= 0xFF,
			.sat_threshold				= 0xFF,
			.exp77_mode					= D_IM_SHDR_EXP77MD_PERIMAXVAL,
			.fmd_sel					= D_IM_SHDR_FMDSEL_REDUCTION,
		},
#if 0
		[ 1 ] = {	// Min.
			.ep_boundary[ 0 ]			= 0x00,
			.ep_boundary[ 1 ]			= 0x00,
			.ep_boundary[ 2 ]			= 0x00,
			.ep_boundary[ 3 ]			= 0x00,
			.ep_offset[ 0 ]				= 0x00,
			.ep_offset[ 1 ]				= 0x00,
			.ep_offset[ 2 ]				= 0x00,
			.ep_offset[ 3 ]				= 0x00,
			.ep_blend					= 0x0,
			.fd_weight					= 0x00,
			.mvcnt.bounds				= 0x00,
			.mvcnt.threshold			= 0x00,
			.mvcnt.gr					= -0x1000,
			.bldr_offset[ 0 ]			= 0x00,
			.bldr_offset[ 1 ]			= 0x00,
			.bldr_offset[ 2 ]			= 0x00,
			.bldr_offset[ 3 ]			= 0x00,
			.bldr_offset[ 4 ]			= 0x00,
			.bldr_slope[ 0 ]			= 0x000,
			.bldr_slope[ 1 ]			= 0x000,
			.bldr_slope[ 2 ]			= 0x000,
			.bldr_slope[ 3 ]			= 0x000,
			.bldr_slope[ 4 ]			= 0x000,
			.bldr_boundary[ 0 ]			= 0x00,
			.bldr_boundary[ 1 ]			= 0x00,
			.bldr_boundary[ 2 ]			= 0x00,
			.bldr_boundary[ 3 ]			= 0x00,
			.bldr_boundary[ 4 ]			= 0x00,
			.weight_r2y_coef[ 0 ]		= -0x4000,
			.weight_r2y_coef[ 1 ]		= -0x4000,
			.weight_r2y_coef[ 2 ]		= -0x4000,
			.weight_r2y_clp.upper		= 0x00,
			.weight_r2y_clp.lower		= 0x00,
			.weight_base				= D_IM_SHDR_WGHTBASE_Y,
			.weight_offset[ 0 ][ 0 ]	= 0x000,
			.weight_offset[ 0 ][ 1 ]	= 0x000,
			.weight_offset[ 0 ][ 2 ]	= 0x000,
			.weight_offset[ 0 ][ 3 ]	= 0x000,
			.weight_offset[ 0 ][ 4 ]	= 0x000,
			.weight_offset[ 1 ][ 0 ]	= 0x000,
			.weight_offset[ 1 ][ 1 ]	= 0x000,
			.weight_offset[ 1 ][ 2 ]	= 0x000,
			.weight_offset[ 1 ][ 3 ]	= 0x000,
			.weight_offset[ 1 ][ 4 ]	= 0x000,
			.weight_offset[ 2 ][ 0 ]	= 0x000,
			.weight_offset[ 2 ][ 1 ]	= 0x000,
			.weight_offset[ 2 ][ 2 ]	= 0x000,
			.weight_offset[ 2 ][ 3 ]	= 0x000,
			.weight_offset[ 2 ][ 4 ]	= 0x000,
			.weight_slope[ 0 ][ 0 ]		= 0x000,
			.weight_slope[ 0 ][ 1 ]		= 0x000,
			.weight_slope[ 0 ][ 2 ]		= 0x000,
			.weight_slope[ 0 ][ 3 ]		= 0x000,
			.weight_slope[ 0 ][ 4 ]		= 0x000,
			.weight_slope[ 1 ][ 0 ]		= 0x000,
			.weight_slope[ 1 ][ 1 ]		= 0x000,
			.weight_slope[ 1 ][ 2 ]		= 0x000,
			.weight_slope[ 1 ][ 3 ]		= 0x000,
			.weight_slope[ 1 ][ 4 ]		= 0x000,
			.weight_slope[ 2 ][ 0 ]		= 0x000,
			.weight_slope[ 2 ][ 1 ]		= 0x000,
			.weight_slope[ 2 ][ 2 ]		= 0x000,
			.weight_slope[ 2 ][ 3 ]		= 0x000,
			.weight_slope[ 2 ][ 4 ]		= 0x000,
//			.weight_boundary[ 0 ][ 0 ]	= 0x00,
			.weight_boundary[ 0 ][ 1 ]	= 0x00,
			.weight_boundary[ 0 ][ 2 ]	= 0x00,
			.weight_boundary[ 0 ][ 3 ]	= 0x00,
			.weight_boundary[ 0 ][ 4 ]	= 0x00,
//			.weight_boundary[ 1 ][ 0 ]	= 0x00,
			.weight_boundary[ 1 ][ 1 ]	= 0x00,
			.weight_boundary[ 1 ][ 2 ]	= 0x00,
			.weight_boundary[ 1 ][ 3 ]	= 0x00,
			.weight_boundary[ 1 ][ 4 ]	= 0x00,
//			.weight_boundary[ 2 ][ 0 ]	= 0x00,
			.weight_boundary[ 2 ][ 1 ]	= 0x00,
			.weight_boundary[ 2 ][ 2 ]	= 0x00,
			.weight_boundary[ 2 ][ 3 ]	= 0x00,
			.weight_boundary[ 2 ][ 4 ]	= 0x00,
			.iso_threshold				= 0x000,
			.iso_coef[ 0 ]				= 0x0,
			.iso_coef[ 1 ]				= 0x0,
			.iso_coef[ 2 ]				= 0x0,
			.iso_coef[ 3 ]				= 0x00,
			.iso_coef[ 4 ]				= 0x00,
			.iso_coef[ 5 ]				= 0x00,
			.sat_threshold				= 0x00,
			.exp77_mode					= D_IM_SHDR_EXP77MD_COEFSUMDIV,
			.fmd_sel					= D_IM_SHDR_FMDSEL_EXPANSION,
		},
#endif
	};

	for( loopcnt = 0; loopcnt < (sizeof(fmd) / sizeof(fmd[0])); loopcnt++ ) {
		ret = Im_SHDR_Set_Fmd( &fmd[ loopcnt ] );
		Ddim_Print(("ct_im_shdr_set_fmd(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_set_pmsk( VOID )
{
	UINT32			loopcnt;
	INT32			ret;
	T_IM_SHDR_PMSK	pmsk[] = {
		[ 0 ] = {	// Max.
			.tlut[ 0 ]	= 0xFF,
			.tlut[ 1 ]	= 0xFF,
			.tlut[ 2 ]	= 0xFF,
			.tlut[ 3 ]	= 0xFF,
			.tlut[ 4 ]	= 0xFF,
			.tlut[ 5 ]	= 0xFF,
			.tlut[ 6 ]	= 0xFF,
			.tlut[ 7 ]	= 0xFF,
			.tlut[ 8 ]	= 0xFF,
			.tlut[ 9 ]	= 0xFF,
			.tlut[ 10 ]	= 0xFF,
			.tlut[ 11 ]	= 0xFF,
			.tlut[ 12 ]	= 0xFF,
			.tlut[ 13 ]	= 0xFF,
			.tlut[ 14 ]	= 0xFF,
			.tlut[ 15 ]	= 0xFF,
			.tlut[ 16 ]	= 0xFF,
			.pmsk_mode	= D_IM_SHDR_PMSKMD_AVE,
		},
#if 0
		[ 1 ] = {	// Min.
			.tlut[ 0 ]	= 0x00,
			.tlut[ 1 ]	= 0x00,
			.tlut[ 2 ]	= 0x00,
			.tlut[ 3 ]	= 0x00,
			.tlut[ 4 ]	= 0x00,
			.tlut[ 5 ]	= 0x00,
			.tlut[ 6 ]	= 0x00,
			.tlut[ 7 ]	= 0x00,
			.tlut[ 8 ]	= 0x00,
			.tlut[ 9 ]	= 0x00,
			.tlut[ 10 ]	= 0x00,
			.tlut[ 11 ]	= 0x00,
			.tlut[ 12 ]	= 0x00,
			.tlut[ 13 ]	= 0x00,
			.tlut[ 14 ]	= 0x00,
			.tlut[ 15 ]	= 0x00,
			.tlut[ 16 ]	= 0x00,
			.pmsk_mode	= D_IM_SHDR_PMSKMD_MAX,
		},
#endif
	};

	for( loopcnt = 0; loopcnt < (sizeof(pmsk) / sizeof(pmsk[0])); loopcnt++ ) {
		ret = Im_SHDR_Set_PreMask( &pmsk[ loopcnt ] );
		Ddim_Print(("ct_im_shdr_set_pmsk(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_set_spnr( VOID )
{
	UINT32			loopcnt;
	INT32			ret;
	T_IM_SHDR_SPNR	spnr[] = {
		[ 0 ] = {	// Max.
			.spnr_en			= D_IM_SHDR_ENABLE,
			.spe_blend			= 0x7,
//			.spe_boundary[ 0 ]	= 0xFFF,
			.spe_boundary[ 1 ]	= 0xFFF,
			.spe_boundary[ 2 ]	= 0xFFF,
			.spe_boundary[ 3 ]	= 0xFFF,
			.spe_offset[ 0 ]	= 0xFFF,
			.spe_offset[ 1 ]	= 0xFFF,
			.spe_offset[ 2 ]	= 0xFFF,
			.spe_offset[ 3 ]	= 0xFFF,
		},
#if 0
		[ 1 ] = {	// Min.
			.spnr_en			= D_IM_SHDR_DISABLE,
			.spe_blend			= 0x0,
//			.spe_boundary[ 0 ]	= 0x000,
			.spe_boundary[ 1 ]	= 0x000,
			.spe_boundary[ 2 ]	= 0x000,
			.spe_boundary[ 3 ]	= 0x000,
			.spe_offset[ 0 ]	= 0x000,
			.spe_offset[ 1 ]	= 0x000,
			.spe_offset[ 2 ]	= 0x000,
			.spe_offset[ 3 ]	= 0x000,
		},
#endif
	};

	for( loopcnt = 0; loopcnt < (sizeof(spnr) / sizeof(spnr[0])); loopcnt++ ) {
		ret = Im_SHDR_Set_Spnr( &spnr[ loopcnt ] );
		Ddim_Print(("ct_im_shdr_set_spnr(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_set_mask( VOID )
{
	UINT32			loopcnt;
	INT32			ret;
	T_IM_SHDR_MSK	msk[] = {
		[ 0 ] = {	// Max.
			.y2r_clp_upper_long[ 0 ]	= 0xFFF,
			.y2r_clp_upper_long[ 1 ]	= 0xFFF,
			.y2r_clp_upper_long[ 2 ]	= 0xFFF,
			.y2r_clp_lower_long[ 0 ]	= 0xFFF,
			.y2r_clp_lower_long[ 1 ]	= 0xFFF,
			.y2r_clp_lower_long[ 2 ]	= 0xFFF,
			.y2r_clp_upper_short[ 0 ]	= 0xFFF,
			.y2r_clp_upper_short[ 1 ]	= 0xFFF,
			.y2r_clp_upper_short[ 2 ]	= 0xFFF,
			.y2r_clp_lower_short[ 0 ]	= 0xFFF,
			.y2r_clp_lower_short[ 1 ]	= 0xFFF,
			.y2r_clp_lower_short[ 2 ]	= 0xFFF,
			.alpha						= 0x3F,
			.lpfc[ 0 ][ 0 ]				= 0xFF,
			.lpfc[ 0 ][ 1 ]				= 0xFF,
			.lpfc[ 0 ][ 2 ]				= 0xFF,
			.lpfc[ 1 ][ 0 ]				= 0xFF,
			.lpfc[ 1 ][ 1 ]				= 0xFF,
			.lpfc[ 1 ][ 2 ]				= 0xFF,
			.lpfc[ 2 ][ 0 ]				= 0xFF,
			.lpfc[ 2 ][ 1 ]				= 0xFF,
			.lpfc[ 2 ][ 2 ]				= 0xFF,
			.blend_offset[ 0 ]			= 0xFF,
			.blend_offset[ 1 ]			= 0xFF,
			.blend_offset[ 2 ]			= 0xFF,
			.blend_offset[ 3 ]			= 0xFF,
			.blend_offset[ 4 ]			= 0xFF,
			.blend_offset[ 5 ]			= 0xFF,
			.blend_offset[ 6 ]			= 0xFF,
			.blend_slope[ 0 ]			= 0xFFFF,
			.blend_slope[ 1 ]			= 0xFFFF,
			.blend_slope[ 2 ]			= 0xFFFF,
			.blend_slope[ 3 ]			= 0xFFFF,
			.blend_slope[ 4 ]			= 0xFFFF,
			.blend_slope[ 5 ]			= 0xFFFF,
			.blend_slope[ 6 ]			= 0xFFFF,
//			.blend_boundary[ 0 ]		= 0xFFF,
			.blend_boundary[ 1 ]		= 0xFFF,
			.blend_boundary[ 2 ]		= 0xFFF,
			.blend_boundary[ 3 ]		= 0xFFF,
			.blend_boundary[ 4 ]		= 0xFFF,
			.blend_boundary[ 5 ]		= 0xFFF,
			.blend_boundary[ 6 ]		= 0xFFF,
			.norm_gain[ 0 ]				= 0xFF,
			.norm_gain[ 1 ]				= 0xFF,
			.norm_gain[ 2 ]				= 0xFF,
			.norm_offset[ 0 ]			= 0xFF,
			.norm_offset[ 1 ]			= 0xFF,
			.norm_offset[ 2 ]			= 0xFF,
			.msk_mode					= D_IM_SHDR_MSKMD_MSKONLY,
		},
#if 0
		[ 1 ] = {	// Min.
			.y2r_clp_upper_long[ 0 ]	= 0x000,
			.y2r_clp_upper_long[ 1 ]	= 0x000,
			.y2r_clp_upper_long[ 2 ]	= 0x000,
			.y2r_clp_lower_long[ 0 ]	= 0x000,
			.y2r_clp_lower_long[ 1 ]	= 0x000,
			.y2r_clp_lower_long[ 2 ]	= 0x000,
			.y2r_clp_upper_short[ 0 ]	= 0x000,
			.y2r_clp_upper_short[ 1 ]	= 0x000,
			.y2r_clp_upper_short[ 2 ]	= 0x000,
			.y2r_clp_lower_short[ 0 ]	= 0x000,
			.y2r_clp_lower_short[ 1 ]	= 0x000,
			.y2r_clp_lower_short[ 2 ]	= 0x000,
			.alpha						= 0x00,
			.lpfc[ 0 ][ 0 ]				= 0x00,
			.lpfc[ 0 ][ 1 ]				= 0x00,
			.lpfc[ 0 ][ 2 ]				= 0x00,
			.lpfc[ 1 ][ 0 ]				= 0x00,
			.lpfc[ 1 ][ 1 ]				= 0x00,
			.lpfc[ 1 ][ 2 ]				= 0x00,
			.lpfc[ 2 ][ 0 ]				= 0x00,
			.lpfc[ 2 ][ 1 ]				= 0x00,
			.lpfc[ 2 ][ 2 ]				= 0x00,
			.blend_offset[ 0 ]			= 0x00,
			.blend_offset[ 1 ]			= 0x00,
			.blend_offset[ 2 ]			= 0x00,
			.blend_offset[ 3 ]			= 0x00,
			.blend_offset[ 4 ]			= 0x00,
			.blend_offset[ 5 ]			= 0x00,
			.blend_offset[ 6 ]			= 0x00,
			.blend_slope[ 0 ]			= 0x0000,
			.blend_slope[ 1 ]			= 0x0000,
			.blend_slope[ 2 ]			= 0x0000,
			.blend_slope[ 3 ]			= 0x0000,
			.blend_slope[ 4 ]			= 0x0000,
			.blend_slope[ 5 ]			= 0x0000,
			.blend_slope[ 6 ]			= 0x0000,
//			.blend_boundary[ 0 ]		= 0x000,
			.blend_boundary[ 1 ]		= 0x000,
			.blend_boundary[ 2 ]		= 0x000,
			.blend_boundary[ 3 ]		= 0x000,
			.blend_boundary[ 4 ]		= 0x000,
			.blend_boundary[ 5 ]		= 0x000,
			.blend_boundary[ 6 ]		= 0x000,
			.norm_gain[ 0 ]				= 0xFF,
			.norm_gain[ 1 ]				= 0xFF,
			.norm_gain[ 2 ]				= 0xFF,
			.norm_offset[ 0 ]			= 0xFF,
			.norm_offset[ 1 ]			= 0xFF,
			.norm_offset[ 2 ]			= 0xFF,
			.msk_mode					= D_IM_SHDR_MSKMD_SHORT,
		},
#endif
	};

	for( loopcnt = 0; loopcnt < (sizeof(msk) / sizeof(msk[0])); loopcnt++ ) {
		ret = Im_SHDR_Set_Mask( &msk[ loopcnt ] );
		Ddim_Print(("ct_im_shdr_set_msk(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_set_bld( VOID )
{
	UINT32			loopcnt;
	INT32			ret;
	T_IM_SHDR_BLD	bld[] = {
		[ 0 ] = {	// Max.
			.blend_sel		= D_IM_SHDR_BLDSEL_LONG_SHORT,
			.alpha_sel		= D_IM_SHDR_ALPHASEL_FIXED,
			.alpha_val[ 0 ]	= 0xFF,
			.alpha_val[ 1 ]	= 0xFF,
			.alpha_val[ 2 ]	= 0xFF,
		},
#if 0
		[ 1 ] = {	// Min.
			.blend_sel		= D_IM_SHDR_BLDSEL_SHORT_LONG,
			.alpha_sel		= D_IM_SHDR_ALPHASEL_MERGEMASK,
			.alpha_val[ 0 ]	= 0x00,
			.alpha_val[ 1 ]	= 0x00,
			.alpha_val[ 2 ]	= 0x00,
		},
#endif
	};

	for( loopcnt = 0; loopcnt < (sizeof(bld) / sizeof(bld[0])); loopcnt++ ) {
		ret = Im_SHDR_Set_Blend( &bld[ loopcnt ] );
		Ddim_Print(("ct_im_shdr_set_bld(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_set_post( VOID )
{
	UINT32			loopcnt;
	INT32			ret;
	T_IM_SHDR_POST	post[] = {
		[ 0 ] = {	// Max.
			.sft1rnd.sft1rnd_en		= D_IM_SHDR_ENABLE,
			.y2r_coef[ 0 ]			= 0x3FFF,
			.y2r_coef[ 1 ]			= 0x3FFF,
			.y2r_coef[ 2 ]			= 0x3FFF,
			.y2r_coef[ 3 ]			= 0x3FFF,
			.y2r_coef[ 4 ]			= 0x3FFF,
			.y2r_coef[ 5 ]			= 0x3FFF,
			.y2r_coef[ 6 ]			= 0x3FFF,
			.y2r_coef[ 7 ]			= 0x3FFF,
			.y2r_coef[ 8 ]			= 0x3FFF,
			.y2r_clp_th_upper[ 0 ]	= 0xFFFF,
			.y2r_clp_th_upper[ 1 ]	= 0xFFFF,
			.y2r_clp_th_upper[ 2 ]	= 0xFFFF,
			.y2r_clp_th_lower[ 0 ]	= 0xFFFF,
			.y2r_clp_th_lower[ 1 ]	= 0xFFFF,
			.y2r_clp_th_lower[ 2 ]	= 0xFFFF,
			.ev_sel					= D_IM_SHDR_POSTSEL1_RGB,
			.mulqpos				= D_IM_SHDR_POSTMULQPOS_Q2,
//			.ev_kn_boundary[ 0 ]	= 0xFFF,
			.ev_kn_boundary[ 1 ]	= 0xFFF,
			.ev_kn_boundary[ 2 ]	= 0xFFF,
			.ev_kn_boundary[ 3 ]	= 0xFFF,
			.ev_kn_boundary[ 4 ]	= 0xFFF,
			.ev_kn_boundary[ 5 ]	= 0xFFF,
			.ev_kn_boundary[ 6 ]	= 0xFFF,
			.ev_kn_boundary[ 7 ]	= 0xFFF,
			.ev_kn_boundary[ 8 ]	= 0xFFF,
			.ev_kn_boundary[ 9 ]	= 0xFFF,
			.ev_kn_offset[ 0 ]		= 0x3FFF,
			.ev_kn_offset[ 1 ]		= 0x3FFF,
			.ev_kn_offset[ 2 ]		= 0x3FFF,
			.ev_kn_offset[ 3 ]		= 0x3FFF,
			.ev_kn_offset[ 4 ]		= 0x3FFF,
			.ev_kn_offset[ 5 ]		= 0x3FFF,
			.ev_kn_offset[ 6 ]		= 0x3FFF,
			.ev_kn_offset[ 7 ]		= 0x3FFF,
			.ev_kn_offset[ 8 ]		= 0x3FFF,
			.ev_kn_offset[ 9 ]		= 0x3FFF,
			.ev_kn_slope[ 0 ]		= 0x3FFF,
			.ev_kn_slope[ 1 ]		= 0x3FFF,
			.ev_kn_slope[ 2 ]		= 0x3FFF,
			.ev_kn_slope[ 3 ]		= 0x3FFF,
			.ev_kn_slope[ 4 ]		= 0x3FFF,
			.ev_kn_slope[ 5 ]		= 0x3FFF,
			.ev_kn_slope[ 6 ]		= 0x3FFF,
			.ev_kn_slope[ 7 ]		= 0x3FFF,
			.ev_kn_slope[ 8 ]		= 0x3FFF,
			.ev_kn_slope[ 9 ]		= 0x3FFF,
			.css_sel				= D_IM_SHDR_POSTSEL0_BYPASS_YCC8,
			.hdrcss_mode			= D_IM_SHDR_HDRCSSMD_YCC422_CENTER,
		},
#if 0
		[ 1 ] = {	// Min.
			.sft1rnd.sft1rnd_en		= D_IM_SHDR_DISABLE,
			.y2r_coef[ 0 ]			= -0x4000,
			.y2r_coef[ 1 ]			= -0x4000,
			.y2r_coef[ 2 ]			= -0x4000,
			.y2r_coef[ 3 ]			= -0x4000,
			.y2r_coef[ 4 ]			= -0x4000,
			.y2r_coef[ 5 ]			= -0x4000,
			.y2r_coef[ 6 ]			= -0x4000,
			.y2r_coef[ 7 ]			= -0x4000,
			.y2r_coef[ 8 ]			= -0x4000,
			.y2r_clp_th_upper[ 0 ]	= 0x0000,
			.y2r_clp_th_upper[ 1 ]	= 0x0000,
			.y2r_clp_th_upper[ 2 ]	= 0x0000,
			.y2r_clp_th_lower[ 0 ]	= 0x0000,
			.y2r_clp_th_lower[ 1 ]	= 0x0000,
			.y2r_clp_th_lower[ 2 ]	= 0x0000,
			.ev_sel					= D_IM_SHDR_POSTSEL1_Y,
			.mulqpos				= D_IM_SHDR_POSTMULQPOS_Q5,
//			.ev_kn_boundary[ 0 ]	= 0x000,
			.ev_kn_boundary[ 1 ]	= 0x000,
			.ev_kn_boundary[ 2 ]	= 0x000,
			.ev_kn_boundary[ 3 ]	= 0x000,
			.ev_kn_boundary[ 4 ]	= 0x000,
			.ev_kn_boundary[ 5 ]	= 0x000,
			.ev_kn_boundary[ 6 ]	= 0x000,
			.ev_kn_boundary[ 7 ]	= 0x000,
			.ev_kn_boundary[ 8 ]	= 0x000,
			.ev_kn_boundary[ 9 ]	= 0x000,
			.ev_kn_offset[ 0 ]		= 0x0000,
			.ev_kn_offset[ 1 ]		= 0x0000,
			.ev_kn_offset[ 2 ]		= 0x0000,
			.ev_kn_offset[ 3 ]		= 0x0000,
			.ev_kn_offset[ 4 ]		= 0x0000,
			.ev_kn_offset[ 5 ]		= 0x0000,
			.ev_kn_offset[ 6 ]		= 0x0000,
			.ev_kn_offset[ 7 ]		= 0x0000,
			.ev_kn_offset[ 8 ]		= 0x0000,
			.ev_kn_offset[ 9 ]		= 0x0000,
			.ev_kn_slope[ 0 ]		= -0x4000,
			.ev_kn_slope[ 1 ]		= -0x4000,
			.ev_kn_slope[ 2 ]		= -0x4000,
			.ev_kn_slope[ 3 ]		= -0x4000,
			.ev_kn_slope[ 4 ]		= -0x4000,
			.ev_kn_slope[ 5 ]		= -0x4000,
			.ev_kn_slope[ 6 ]		= -0x4000,
			.ev_kn_slope[ 7 ]		= -0x4000,
			.ev_kn_slope[ 8 ]		= -0x4000,
			.ev_kn_slope[ 9 ]		= -0x4000,
			.css_sel				= D_IM_SHDR_POSTSEL0_BLEND_YCC16,
			.hdrcss_mode			= D_IM_SHDR_HDRCSSMD_YCC422_ODD,
		},
#endif
	};

	for( loopcnt = 0; loopcnt < (sizeof(post) / sizeof(post[0])); loopcnt++ ) {
		ret = Im_SHDR_Set_Post( &post[ loopcnt ] );
		Ddim_Print(("ct_im_shdr_set_post(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_set_resize( VOID )
{
	UINT32				loopcnt;
	INT32				ret;
	T_IM_SHDR_RESIZE	resize[] = {
		[ 0 ] = {	// Max.
			.chmode.a_w_en		= D_IM_SHDR_ENABLE,
			.chmode.a_drc_en	= D_IM_SHDR_ENABLE,
			.chmode.a_css_md	= D_IM_SHDR_CSSMD_YCC420_CENTER,
			.chmode.b_w_en		= D_IM_SHDR_ENABLE,
			.chmode.b_rdc_md	= D_IM_RDCMD1_1_32,
			.chmode.b_css_md	= D_IM_SHDR_CSSMD_YCC420_CENTER,
			.chmode.c_w_en		= D_IM_SHDR_ENABLE,
			.chmode.c_rsz_md	= D_IM_RSZMD_CENTER1,
			.chmode.c_rdc_en	= D_IM_SHDR_ENABLE,
			.chmode.c_rdc_md	= D_IM_RDCMD_1_8,
			.chmode.c_trm_en	= D_IM_SHDR_ENABLE,
			.chmode.c_drc_en	= D_IM_SHDR_ENABLE,
			.chmode.c_css_md	= D_IM_SHDR_CSSMD_YCC420_CENTER,
			.chmode.d_w_en		= D_IM_SHDR_ENABLE,
			.chmode.d_rsz_md	= D_IM_RSZMD_CENTER1,
			.chmode.d_rdc_en	= D_IM_SHDR_ENABLE,
			.chmode.d_rdc_md	= D_IM_RDCMD_1_8,
			.chmode.d_trm_en	= D_IM_SHDR_ENABLE,
			.chmode.d_drc_en	= D_IM_SHDR_ENABLE,
			.chmode.d_css_md	= D_IM_SHDR_CSSMD_YCC420_CENTER,
			.chmode.e_w_en		= D_IM_SHDR_ENABLE,
			.chmode.e_rsz_md	= D_IM_RSZMD_CENTER1,
			.chmode.e_rdc_en	= D_IM_SHDR_ENABLE,
			.chmode.e_rdc_md	= D_IM_RDCMD_1_8,
			.chmode.e_trm_en	= D_IM_SHDR_ENABLE,
			.chmode.e_css_md	= D_IM_SHDR_CSSMD_YCC420_CENTER,
			.chmode.f_w_en		= D_IM_SHDR_ENABLE,
			.chmode.f_rsz_md	= D_IM_RSZMD_CENTER1,
			.chmode.f_rdc_en	= D_IM_SHDR_ENABLE,
			.chmode.f_rdc_md	= D_IM_RDCMD_1_8,
			.chmode.f_trm_en	= D_IM_SHDR_ENABLE,
			.chmode.f_css_md	= D_IM_SHDR_CSSMD_YCC420_CENTER,
			.woutsz.a_hsize		= 11880,
			.woutsz.a_vsize		= 8800,
			.woutsz.b_hsize		= 2970,
			.woutsz.b_vsize		= 2200,
			.woutsz.c_hsize		= 11880,
			.woutsz.c_vsize		= 8800,
			.woutsz.d_hsize		= 11880,
			.woutsz.d_vsize		= 8800,
			.woutsz.e_hsize		= 11880,
			.woutsz.e_vsize		= 8800,
			.woutsz.f_hsize		= 11880,
			.woutsz.f_vsize		= 8800,
			.rszhcnt.c_hsta		= 0x3FFF,
			.rszhcnt.c_hpit		= 0x7FFF,
			.rszhcnt.d_hsta		= 0x3FFF,
			.rszhcnt.d_hpit		= 0x7FFF,
			.rszhcnt.e_hsta		= 0x3FFF,
			.rszhcnt.e_hpit		= 0x7FFF,
			.rszhcnt.f_hsta		= 0x3FFF,
			.rszhcnt.f_hpit		= 0x7FFF,
			.rszvcnt.c_vsta		= 0x3FFF,
			.rszvcnt.c_vpit		= 0x7FFF,
			.rszvcnt.d_vsta		= 0x3FFF,
			.rszvcnt.d_vpit		= 0x7FFF,
			.rszvcnt.e_vsta		= 0x3FFF,
			.rszvcnt.e_vpit		= 0x7FFF,
			.rszvcnt.f_vsta		= 0x3FFF,
			.rszvcnt.f_vpit		= 0x7FFF,
			.trmsta.c_hsta		= 11846,
			.trmsta.c_vsta		= 8774,
			.trmsta.d_hsta		= 11846,
			.trmsta.d_vsta		= 8774,
			.trmsta.e_hsta		= 11846,
			.trmsta.e_vsta		= 8774,
			.trmsta.f_hsta		= 11846,
			.trmsta.f_vsta		= 8774,
			.drcofst.a_y		= 0x1F,
			.drcofst.a_cb		= 0xF,
			.drcofst.a_cr		= 0xF,
			.drcofst.c_y		= 0x1F,
			.drcofst.c_cb		= 0xF,
			.drcofst.c_cr		= 0xF,
			.drcofst.d_y		= 0x1F,
			.drcofst.d_cb		= 0xF,
			.drcofst.d_cr		= 0xF,
			.drcgain.a_y		= 0x1FF,
			.drcgain.a_cb		= 0xFF,
			.drcgain.a_cr		= 0xFF,
			.drcgain.c_y		= 0x1FF,
			.drcgain.c_cb		= 0xFF,
			.drcgain.c_cr		= 0xFF,
			.drcgain.d_y		= 0x1FF,
			.drcgain.d_cb		= 0xFF,
			.drcgain.d_cr		= 0xFF,
			.drcclph.a_y		= 0xFF,
			.drcclph.a_cb		= 0x7F,
			.drcclph.a_cr		= 0x7F,
			.drcclph.c_y		= 0xFF,
			.drcclph.c_cb		= 0x7F,
			.drcclph.c_cr		= 0x7F,
			.drcclph.d_y		= 0xFF,
			.drcclph.d_cb		= 0x7F,
			.drcclph.d_cr		= 0x7F,
			.drcclpl.a_y		= 0xFF,
			.drcclpl.a_cb		= 0x7F,
			.drcclpl.a_cr		= 0x7F,
			.drcclpl.c_y		= 0xFF,
			.drcclpl.c_cb		= 0x7F,
			.drcclpl.c_cr		= 0x7F,
			.drcclpl.d_y		= 0xFF,
			.drcclpl.d_cb		= 0x7F,
			.drcclpl.d_cr		= 0x7F,
		},
#if 0
		[ 1 ] = {	// Min.
			.chmode.a_w_en		= D_IM_SHDR_DISABLE,
			.chmode.a_drc_en	= D_IM_SHDR_DISABLE,
			.chmode.a_css_md	= D_IM_SHDR_CSSMD_YCC422,
			.chmode.b_w_en		= D_IM_SHDR_DISABLE,
			.chmode.b_rdc_md	= D_IM_RDCMD1_1_4,
			.chmode.b_css_md	= D_IM_SHDR_CSSMD_YCC422,
			.chmode.c_w_en		= D_IM_SHDR_DISABLE,
			.chmode.c_rsz_md	= D_IM_RSZMD_ODD0,
			.chmode.c_rdc_en	= D_IM_SHDR_DISABLE,
			.chmode.c_rdc_md	= D_IM_RDCMD_1_2,
			.chmode.c_trm_en	= D_IM_SHDR_DISABLE,
			.chmode.c_drc_en	= D_IM_SHDR_DISABLE,
			.chmode.c_css_md	= D_IM_SHDR_CSSMD_YCC422,
			.chmode.d_w_en		= D_IM_SHDR_DISABLE,
			.chmode.d_rsz_md	= D_IM_RSZMD_ODD0,
			.chmode.d_rdc_en	= D_IM_SHDR_DISABLE,
			.chmode.d_rdc_md	= D_IM_RDCMD_1_2,
			.chmode.d_trm_en	= D_IM_SHDR_DISABLE,
			.chmode.d_drc_en	= D_IM_SHDR_DISABLE,
			.chmode.d_css_md	= D_IM_SHDR_CSSMD_YCC422,
			.chmode.e_w_en		= D_IM_SHDR_DISABLE,
			.chmode.e_rsz_md	= D_IM_RSZMD_ODD0,
			.chmode.e_rdc_en	= D_IM_SHDR_DISABLE,
			.chmode.e_rdc_md	= D_IM_RDCMD_1_2,
			.chmode.e_trm_en	= D_IM_SHDR_DISABLE,
			.chmode.e_css_md	= D_IM_SHDR_CSSMD_YCC422,
			.chmode.f_w_en		= D_IM_SHDR_DISABLE,
			.chmode.f_rsz_md	= D_IM_RSZMD_ODD0,
			.chmode.f_rdc_en	= D_IM_SHDR_DISABLE,
			.chmode.f_rdc_md	= D_IM_RDCMD_1_2,
			.chmode.f_trm_en	= D_IM_SHDR_DISABLE,
			.chmode.f_css_md	= D_IM_SHDR_CSSMD_YCC422,
			.woutsz.a_hsize		= 512,
			.woutsz.a_vsize		= 384,
			.woutsz.b_hsize		= 16,
			.woutsz.b_vsize		= 12,
			.woutsz.c_hsize		= 32,
			.woutsz.c_vsize		= 24,
			.woutsz.d_hsize		= 32,
			.woutsz.d_vsize		= 24,
			.woutsz.e_hsize		= 32,
			.woutsz.e_vsize		= 24,
			.woutsz.f_hsize		= 32,
			.woutsz.f_vsize		= 24,
			.rszhcnt.c_hsta		= 0x0000,
			.rszhcnt.c_hpit		= 0x4000,
			.rszhcnt.d_hsta		= 0x0000,
			.rszhcnt.d_hpit		= 0x4000,
			.rszhcnt.e_hsta		= 0x0000,
			.rszhcnt.e_hpit		= 0x4000,
			.rszhcnt.f_hsta		= 0x0000,
			.rszhcnt.f_hpit		= 0x4000,
			.rszvcnt.c_vsta		= 0x0000,
			.rszvcnt.c_vpit		= 0x4000,
			.rszvcnt.d_vsta		= 0x0000,
			.rszvcnt.d_vpit		= 0x4000,
			.rszvcnt.e_vsta		= 0x0000,
			.rszvcnt.e_vpit		= 0x4000,
			.rszvcnt.f_vsta		= 0x0000,
			.rszvcnt.f_vpit		= 0x4000,
			.trmsta.c_hsta		= 0,
			.trmsta.c_vsta		= 0,
			.trmsta.d_hsta		= 0,
			.trmsta.d_vsta		= 0,
			.trmsta.e_hsta		= 0,
			.trmsta.e_vsta		= 0,
			.trmsta.f_hsta		= 0,
			.trmsta.f_vsta		= 0,
			.drcofst.a_y		= 0x00,
			.drcofst.a_cb		= -0x10,
			.drcofst.a_cr		= -0x10,
			.drcofst.c_y		= 0x00,
			.drcofst.c_cb		= -0x10,
			.drcofst.c_cr		= -0x10,
			.drcofst.d_y		= 0x00,
			.drcofst.d_cb		= -0x10,
			.drcofst.d_cr		= -0x10,
			.drcgain.a_y		= 0x000,
			.drcgain.a_cb		= 0x00,
			.drcgain.a_cr		= 0x00,
			.drcgain.c_y		= 0x000,
			.drcgain.c_cb		= 0x00,
			.drcgain.c_cr		= 0x00,
			.drcgain.d_y		= 0x000,
			.drcgain.d_cb		= 0x00,
			.drcgain.d_cr		= 0x00,
			.drcclph.a_y		= 0x00,
			.drcclph.a_cb		= -0x80,
			.drcclph.a_cr		= -0x80,
			.drcclph.c_y		= 0x00,
			.drcclph.c_cb		= -0x80,
			.drcclph.c_cr		= -0x80,
			.drcclph.d_y		= 0x00,
			.drcclph.d_cb		= -0x80,
			.drcclph.d_cr		= -0x80,
			.drcclpl.a_y		= 0x00,
			.drcclpl.a_cb		= -0x80,
			.drcclpl.a_cr		= -0x80,
			.drcclpl.c_y		= 0x00,
			.drcclpl.c_cb		= -0x80,
			.drcclpl.c_cr		= -0x80,
			.drcclpl.d_y		= 0x00,
			.drcclpl.d_cb		= -0x80,
			.drcclpl.d_cr		= -0x80,
		},
#endif
	};

	for( loopcnt = 0; loopcnt < (sizeof(resize) / sizeof(resize[0])); loopcnt++ ) {
		ret = Im_SHDR_Set_Resize( &resize[ loopcnt ] );
		Ddim_Print(("ct_im_shdr_set_resize(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_set_smc( VOID )
{
	UINT32			loopcnt;
	INT32			ret;
	T_IM_SHDR_SMC	smc[] = {
		[ 0 ] = {	// Max.
			.mcarmd.mca_mode			= D_IM_MCAMD_MAX_MIN_VECTOR,
			.mcarmd.mcr_mode			= D_IM_MCRMD_MC,
			.mcmpmd.mcmp_mode			= D_IM_SHDR_ON,
			.mcmpmd.mvw_en				= D_IM_SHDR_ENABLE,
			.mmvval.mmv_x				= 64,
			.mmvval.mmv_y				= 48,
			.mmvrcnt.mmvro_x			= 16,
			.mmvrcnt.mmvro_y			= 16,
			.mmvrcnt.mmvrk_x			= 6,
			.mmvrcnt.mmvrk_y			= 6,
			.mdbcnt.mdb_x				= 5,
			.mdbcnt.mdb_y				= 3,
			.mdbrocnt.mdbro_x			= 16,
			.mdbrocnt.mdbro_y			= 16,
			.mdbrocnt.mdbrk_x			= 4,
			.mdbrocnt.mdbrk_y			= 3,
			.molrcnt.molr_sm			= 0xF,
			.molrcnt.molr_ss			= 0xF,
			.molrcnt.molr_o				= 16,
			.molrcnt.molr_k				= 2,
			.mcpcnt.mcpx_s				= 128,
			.mcpcnt.mcpx_o				= 16,
			.mcpcnt.mcpx_k				= 6,
			.mmcrcnt.mmcr_s				= 765,
			.mmcrcnt.mmcr_k				= 6,
			.molecnt.mole_s				= 765,
			.molecnt.mole_o				= 16,
			.molecnt.mole_k				= 6,
			.mmvecnt.mmve_s				= 765,
			.mmvecnt.mmve_o				= 16,
			.mmvecnt.mmve_k				= 6,
			.mcpxecnt.mcpxe_s			= 765,
			.mcpxecnt.mcpxe_o			= 16,
			.mcpxecnt.mcpxe_k			= 6,
			.mvlcnt.mvl_en				= D_IM_SHDR_ENABLE,
			.mvlcnt.mvl_threshold		= 64,
			.mvlcnt.mvl_count_threshold	= 127,
		},
#if 0
		[ 1 ] = {	// Min.
			.mcarmd.mca_mode			= D_IM_MCAMD_MAX,
			.mcarmd.mcr_mode			= D_IM_MCRMD_STANDARD,
			.mcmpmd.mcmp_mode			= D_IM_SHDR_OFF,
			.mcmpmd.mvw_en				= D_IM_SHDR_DISABLE,
			.mmvval.mmv_x				= 0,
			.mmvval.mmv_y				= 0,
			.mmvrcnt.mmvro_x			= 0,
			.mmvrcnt.mmvro_y			= 0,
			.mmvrcnt.mmvrk_x			= 0,
			.mmvrcnt.mmvrk_y			= 0,
			.mdbcnt.mdb_x				= 0,
			.mdbcnt.mdb_y				= 0,
			.mdbrocnt.mdbro_x			= 0,
			.mdbrocnt.mdbro_y			= 0,
			.mdbrocnt.mdbrk_x			= 0,
			.mdbrocnt.mdbrk_y			= 0,
			.molrcnt.molr_sm			= 0x0,
			.molrcnt.molr_ss			= 0x0,
			.molrcnt.molr_o				= 0,
			.molrcnt.molr_k				= 0,
			.mcpcnt.mcpx_s				= 0,
			.mcpcnt.mcpx_o				= 0,
			.mcpcnt.mcpx_k				= 0,
			.mmcrcnt.mmcr_s				= 0,
			.mmcrcnt.mmcr_k				= 0,
			.molecnt.mole_s				= 0,
			.molecnt.mole_o				= 0,
			.molecnt.mole_k				= 0,
			.mmvecnt.mmve_s				= 0,
			.mmvecnt.mmve_o				= 0,
			.mmvecnt.mmve_k				= 0,
			.mcpxecnt.mcpxe_s			= 0,
			.mcpxecnt.mcpxe_o			= 0,
			.mcpxecnt.mcpxe_k			= 0,
			.mvlcnt.mvl_en				= D_IM_SHDR_DISABLE,
			.mvlcnt.mvl_threshold		= 0,
			.mvlcnt.mvl_count_threshold	= 0,
		},
#endif
	};

	for( loopcnt = 0; loopcnt < (sizeof(smc) / sizeof(smc[0])); loopcnt++ ) {
		ret = Im_SHDR_Set_Smc( &smc[ loopcnt ] );
		Ddim_Print(("ct_im_shdr_set_smc(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_set_dgr( VOID )
{
	UINT32			loopcnt;
	INT32			ret;
	USHORT			src_tbl[][D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE] = {
			{	// Max.
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
			},
#if 0
			{	// Min.
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			},
#endif
		};

	for( loopcnt = 0; loopcnt < (sizeof(src_tbl) / sizeof(src_tbl[0])); loopcnt++ ) {
		ret = Im_SHDR_Set_Degamma_R( &src_tbl[loopcnt][0], 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
		Ddim_Print(("ct_im_shdr_set_dgr(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_set_dgg( VOID )
{
	UINT32			loopcnt;
	INT32			ret;
	USHORT			src_tbl[][ D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ] = {
			{	// Max.
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
			},
#if 0
			{	// Min.
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			},
#endif
		};

	for( loopcnt = 0; loopcnt < (sizeof(src_tbl) / sizeof(src_tbl[0])); loopcnt++ ) {
		ret = Im_SHDR_Set_Degamma_G( &src_tbl[loopcnt][0], 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
		Ddim_Print(("ct_im_shdr_set_dgg(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_set_dgb( VOID )
{
	UINT32			loopcnt;
	INT32			ret;
	USHORT			src_tbl[][ D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ] = {
			{	// Max.
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
			},
#if 0
			{	// Min.
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			},
#endif
		};

	for( loopcnt = 0; loopcnt < (sizeof(src_tbl) / sizeof(src_tbl[0])); loopcnt++ ) {
		ret = Im_SHDR_Set_Degamma_B( &src_tbl[loopcnt][0], 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
		Ddim_Print(("ct_im_shdr_set_dgb(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_set_dg( VOID )
{
	UINT32	loopcnt;
	INT32	ret;
	USHORT	src_tbl_r[][ D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ] = {
			{	// Max.
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
			},
#if 0
			{	// Min.
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			},
#endif
		};
	USHORT	src_tbl_g[][ D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ] = {
			{	// Max.
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
			},
#if 0
			{	// Min.
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			},
#endif
		};
	USHORT	src_tbl_b[][ D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ] = {
			{	// Max.
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
				0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
			},
#if 0
			{	// Min.
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
				0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			},
#endif
		};

	for( loopcnt = 0; loopcnt < (sizeof(src_tbl_r) / sizeof(src_tbl_r[0])); loopcnt++ ) {
		ret = Im_SHDR_Set_Degamma( &src_tbl_r[loopcnt][0], &src_tbl_g[loopcnt][0], &src_tbl_b[loopcnt][0], 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
		Ddim_Print(("ct_im_shdr_set_dg(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_set_axi( VOID )
{
	UINT32			loopcnt;
	INT32			ret;
	T_IM_SHDR_AXI	axi[] = {
		[ 0 ] = {	// Max.
			.ax_mode.r_burst		= D_IM_SHDR_BURSTLEN_256,
			.ax_mode.w_burst		= D_IM_SHDR_BURSTLEN_256,
			.ax_mode.w_w_mask		= D_IM_SHDR_WWMASK_OFF,
			.r_ctl.a_cahche[0]		= 0xF,
			.r_ctl.a_cahche[1]		= 0xF,
			.r_ctl.a_protect[0]		= 0x7,
			.r_ctl.a_protect[1]		= 0x7,
			.r_ctl.b_cahche[0]		= 0xF,
			.r_ctl.b_cahche[1]		= 0xF,
			.r_ctl.b_protect[0]		= 0x7,
			.r_ctl.b_protect[1]		= 0x7,
			.r_ctl.c_cahche[0]		= 0xF,
			.r_ctl.c_cahche[1]		= 0xF,
			.r_ctl.c_cahche[2]		= 0xF,
			.r_ctl.c_cahche[3]		= 0xF,
			.r_ctl.c_protect[0]		= 0x7,
			.r_ctl.c_protect[1]		= 0x7,
			.r_ctl.c_protect[2]		= 0x7,
			.r_ctl.c_protect[3]		= 0x7,
			.r_ctl.d_cahche[0]		= 0xF,
			.r_ctl.d_cahche[1]		= 0xF,
			.r_ctl.d_protect[0]		= 0x7,
			.r_ctl.d_protect[1]		= 0x7,
			.w_ctl.a_cahche[0]		= 0xF,
			.w_ctl.a_cahche[1]		= 0xF,
			.w_ctl.a_cahche[2]		= 0xF,
			.w_ctl.a_cahche[3]		= 0xF,
			.w_ctl.a_protect[0]		= 0x7,
			.w_ctl.a_protect[1]		= 0x7,
			.w_ctl.a_protect[2]		= 0x7,
			.w_ctl.a_protect[3]		= 0x7,
			.w_ctl.b_cahche[0]		= 0xF,
			.w_ctl.b_cahche[1]		= 0xF,
			.w_ctl.b_cahche[2]		= 0xF,
			.w_ctl.b_cahche[3]		= 0xF,
			.w_ctl.b_cahche[4]		= 0xF,
			.w_ctl.b_cahche[5]		= 0xF,
			.w_ctl.b_protect[0]		= 0x7,
			.w_ctl.b_protect[1]		= 0x7,
			.w_ctl.b_protect[2]		= 0x7,
			.w_ctl.b_protect[3]		= 0x7,
			.w_ctl.b_protect[4]		= 0x7,
			.w_ctl.b_protect[5]		= 0x7,
			.w_ctl.c_cahche[0]		= 0xF,
			.w_ctl.c_cahche[1]		= 0xF,
			.w_ctl.c_cahche[2]		= 0xF,
			.w_ctl.c_cahche[3]		= 0xF,
			.w_ctl.c_cahche[4]		= 0xF,
			.w_ctl.c_cahche[5]		= 0xF,
			.w_ctl.c_cahche[6]		= 0xF,
			.w_ctl.c_protect[0]		= 0x7,
			.w_ctl.c_protect[1]		= 0x7,
			.w_ctl.c_protect[2]		= 0x7,
			.w_ctl.c_protect[3]		= 0x7,
			.w_ctl.c_protect[4]		= 0x7,
			.w_ctl.c_protect[5]		= 0x7,
			.w_ctl.c_protect[6]		= 0x7,
		},
#if 0
		[ 1 ] = {	// Min.
			.ax_mode.r_burst		= D_IM_SHDR_BURSTLEN_512,
			.ax_mode.w_burst		= D_IM_SHDR_BURSTLEN_512,
			.ax_mode.w_w_mask		= D_IM_SHDR_WWMASK_ON,
			.r_ctl.a_cahche[0]		= 0x0,
			.r_ctl.a_cahche[1]		= 0x0,
			.r_ctl.a_protect[0]		= 0x0,
			.r_ctl.a_protect[1]		= 0x0,
			.r_ctl.b_cahche[0]		= 0x0,
			.r_ctl.b_cahche[1]		= 0x0,
			.r_ctl.b_protect[0]		= 0x0,
			.r_ctl.b_protect[1]		= 0x0,
			.r_ctl.c_cahche[0]		= 0x0,
			.r_ctl.c_cahche[1]		= 0x0,
			.r_ctl.c_cahche[2]		= 0x0,
			.r_ctl.c_cahche[3]		= 0x0,
			.r_ctl.c_protect[0]		= 0x0,
			.r_ctl.c_protect[1]		= 0x0,
			.r_ctl.c_protect[2]		= 0x0,
			.r_ctl.c_protect[3]		= 0x0,
			.r_ctl.d_cahche[0]		= 0x0,
			.r_ctl.d_cahche[1]		= 0x0,
			.r_ctl.d_protect[0]		= 0x0,
			.r_ctl.d_protect[1]		= 0x0,
			.w_ctl.a_cahche[0]		= 0x0,
			.w_ctl.a_cahche[1]		= 0x0,
			.w_ctl.a_cahche[2]		= 0x0,
			.w_ctl.a_cahche[3]		= 0x0,
			.w_ctl.a_protect[0]		= 0x0,
			.w_ctl.a_protect[1]		= 0x0,
			.w_ctl.a_protect[2]		= 0x0,
			.w_ctl.a_protect[3]		= 0x0,
			.w_ctl.b_cahche[0]		= 0x0,
			.w_ctl.b_cahche[1]		= 0x0,
			.w_ctl.b_cahche[2]		= 0x0,
			.w_ctl.b_cahche[3]		= 0x0,
			.w_ctl.b_cahche[4]		= 0x0,
			.w_ctl.b_cahche[5]		= 0x0,
			.w_ctl.b_protect[0]		= 0x0,
			.w_ctl.b_protect[1]		= 0x0,
			.w_ctl.b_protect[2]		= 0x0,
			.w_ctl.b_protect[3]		= 0x0,
			.w_ctl.b_protect[4]		= 0x0,
			.w_ctl.b_protect[5]		= 0x0,
			.w_ctl.c_cahche[0]		= 0x0,
			.w_ctl.c_cahche[1]		= 0x0,
			.w_ctl.c_cahche[2]		= 0x0,
			.w_ctl.c_cahche[3]		= 0x0,
			.w_ctl.c_cahche[4]		= 0x0,
			.w_ctl.c_cahche[5]		= 0x0,
			.w_ctl.c_cahche[6]		= 0x0,
			.w_ctl.c_protect[0]		= 0x0,
			.w_ctl.c_protect[1]		= 0x0,
			.w_ctl.c_protect[2]		= 0x0,
			.w_ctl.c_protect[3]		= 0x0,
			.w_ctl.c_protect[4]		= 0x0,
			.w_ctl.c_protect[5]		= 0x0,
			.w_ctl.c_protect[6]		= 0x0,
		},
#endif
	};

	for( loopcnt = 0; loopcnt < (sizeof(axi) / sizeof(axi[0])); loopcnt++ ) {
		ret = Im_SHDR_Set_Axi( &axi[ loopcnt ] );
		Ddim_Print(("ct_im_shdr_set_axi(%d)[ret=0x%08x]\n", loopcnt, ret));
	}
}

static VOID ct_im_shdr_get_ctrl( VOID )
{
	INT32			ret;
	T_IM_SHDR_CTRL	shdr_ctrl;

	ret = Im_SHDR_Get_Ctrl_Common( &shdr_ctrl );

	Ddim_Print(("ct_im_shdr_get_ctrl[ret=0x%08x]\n", ret));
	Ddim_Print(("shdr_ctrl.image_set.in_bit_depth = 0x%08x\n",		shdr_ctrl.image_set.in_bit_depth	));
	Ddim_Print(("shdr_ctrl.image_set.in_c_fmt     = 0x%08x\n",		shdr_ctrl.image_set.in_c_fmt		));
	Ddim_Print(("shdr_ctrl.image_set.in_edge_cpy  = 0x%08x\n",		shdr_ctrl.image_set.in_edge_cpy		));
	Ddim_Print(("shdr_ctrl.image_set.in_edge_cut  = 0x%08x\n",		shdr_ctrl.image_set.in_edge_cut		));
	Ddim_Print(("shdr_ctrl.image_set.out_ctrl     = 0x%08x\n",		shdr_ctrl.image_set.out_ctrl 		));
	Ddim_Print(("shdr_ctrl.image_set.out_sel      = 0x%08x\n",		shdr_ctrl.image_set.out_sel			));
	Ddim_Print(("shdr_ctrl.image_set.out_sel_en   = 0x%08x\n",		shdr_ctrl.image_set.out_sel_en		));
	Ddim_Print(("shdr_ctrl.sdram.r_addr.a_y = 0x%08lx\n",			shdr_ctrl.sdram.r_addr.a_y			));
	Ddim_Print(("shdr_ctrl.sdram.r_addr.a_c = 0x%08lx\n",			shdr_ctrl.sdram.r_addr.a_c			));
	Ddim_Print(("shdr_ctrl.sdram.r_addr.b_y = 0x%08lx\n",			shdr_ctrl.sdram.r_addr.b_y			));
	Ddim_Print(("shdr_ctrl.sdram.r_addr.b_c = 0x%08lx\n",			shdr_ctrl.sdram.r_addr.b_c			));
	Ddim_Print(("shdr_ctrl.sdram.r_addr.c_y = 0x%08lx\n",			shdr_ctrl.sdram.r_addr.c_y			));
	Ddim_Print(("shdr_ctrl.sdram.r_addr.c_c = 0x%08lx\n",			shdr_ctrl.sdram.r_addr.c_c			));
	Ddim_Print(("shdr_ctrl.sdram.r_addr.d_y = 0x%08lx\n",			shdr_ctrl.sdram.r_addr.d_y			));
	Ddim_Print(("shdr_ctrl.sdram.r_addr.d_c = 0x%08lx\n",			shdr_ctrl.sdram.r_addr.d_c			));
	Ddim_Print(("shdr_ctrl.sdram.r_addr.e_y = 0x%08lx\n",			shdr_ctrl.sdram.r_addr.e_y			));
	Ddim_Print(("shdr_ctrl.sdram.r_addr.e_c = 0x%08lx\n",			shdr_ctrl.sdram.r_addr.e_c			));
	Ddim_Print(("shdr_ctrl.sdram.r_addr.f   = 0x%08lx\n",			shdr_ctrl.sdram.r_addr.f			));
	Ddim_Print(("shdr_ctrl.sdram.r_ghsize.a_y = 0x%08x\n",			shdr_ctrl.sdram.r_ghsize.a_y		));
	Ddim_Print(("shdr_ctrl.sdram.r_ghsize.a_c = 0x%08x\n",			shdr_ctrl.sdram.r_ghsize.a_c		));
	Ddim_Print(("shdr_ctrl.sdram.r_ghsize.b_y = 0x%08x\n",			shdr_ctrl.sdram.r_ghsize.b_y		));
	Ddim_Print(("shdr_ctrl.sdram.r_ghsize.b_c = 0x%08x\n",			shdr_ctrl.sdram.r_ghsize.b_c		));
	Ddim_Print(("shdr_ctrl.sdram.r_ghsize.c_y = 0x%08x\n",			shdr_ctrl.sdram.r_ghsize.c_y		));
	Ddim_Print(("shdr_ctrl.sdram.r_ghsize.c_c = 0x%08x\n",			shdr_ctrl.sdram.r_ghsize.c_c		));
	Ddim_Print(("shdr_ctrl.sdram.r_ghsize.d_y = 0x%08x\n",			shdr_ctrl.sdram.r_ghsize.d_y		));
	Ddim_Print(("shdr_ctrl.sdram.r_ghsize.d_c = 0x%08x\n",			shdr_ctrl.sdram.r_ghsize.d_c		));
	Ddim_Print(("shdr_ctrl.sdram.r_ghsize.e_y = 0x%08x\n",			shdr_ctrl.sdram.r_ghsize.e_y		));
	Ddim_Print(("shdr_ctrl.sdram.r_ghsize.e_c = 0x%08x\n",			shdr_ctrl.sdram.r_ghsize.e_c		));
	Ddim_Print(("shdr_ctrl.sdram.r_ghsize.f   = 0x%08x\n",			shdr_ctrl.sdram.r_ghsize.f			));
	Ddim_Print(("shdr_ctrl.sdram.r_vfm.a = 0x%08x\n",				shdr_ctrl.sdram.r_vfm.a				));
	Ddim_Print(("shdr_ctrl.sdram.r_hsize.a = 0x%08x\n",				shdr_ctrl.sdram.r_hsize.a			));
	Ddim_Print(("shdr_ctrl.sdram.r_hsize.b = 0x%08x\n",				shdr_ctrl.sdram.r_hsize.b			));
	Ddim_Print(("shdr_ctrl.sdram.r_hsize.c = 0x%08x\n",				shdr_ctrl.sdram.r_hsize.c			));
	Ddim_Print(("shdr_ctrl.sdram.r_hsize.d = 0x%08x\n",				shdr_ctrl.sdram.r_hsize.d			));
	Ddim_Print(("shdr_ctrl.sdram.r_hsize.e = 0x%08x\n",				shdr_ctrl.sdram.r_hsize.e			));
	Ddim_Print(("shdr_ctrl.sdram.r_vsize.a = 0x%08x\n",				shdr_ctrl.sdram.r_vsize.a			));
	Ddim_Print(("shdr_ctrl.sdram.r_vsize.b = 0x%08x\n",				shdr_ctrl.sdram.r_vsize.b			));
	Ddim_Print(("shdr_ctrl.sdram.r_vsize.c = 0x%08x\n",				shdr_ctrl.sdram.r_vsize.c			));
	Ddim_Print(("shdr_ctrl.sdram.r_vsize.d = 0x%08x\n",				shdr_ctrl.sdram.r_vsize.d			));
	Ddim_Print(("shdr_ctrl.sdram.r_vsize.e = 0x%08x\n",				shdr_ctrl.sdram.r_vsize.e			));
	Ddim_Print(("shdr_ctrl.sdram.w_addr.b_y   = 0x%08lx\n",			shdr_ctrl.sdram.w_addr.b_y			));
	Ddim_Print(("shdr_ctrl.sdram.w_addr.b_c   = 0x%08lx\n",			shdr_ctrl.sdram.w_addr.b_c			));
	Ddim_Print(("shdr_ctrl.sdram.w_addr.e_y   = 0x%08lx\n",			shdr_ctrl.sdram.w_addr.e_y			));
	Ddim_Print(("shdr_ctrl.sdram.w_addr.e_c   = 0x%08lx\n",			shdr_ctrl.sdram.w_addr.e_c			));
	Ddim_Print(("shdr_ctrl.sdram.w_addr.f_y   = 0x%08lx\n",			shdr_ctrl.sdram.w_addr.f_y			));
	Ddim_Print(("shdr_ctrl.sdram.w_addr.f_c   = 0x%08lx\n",			shdr_ctrl.sdram.w_addr.f_c			));
	Ddim_Print(("shdr_ctrl.sdram.w_addr.g_r_y = 0x%08lx\n",			shdr_ctrl.sdram.w_addr.g_r_y		));
	Ddim_Print(("shdr_ctrl.sdram.w_addr.g_g_c = 0x%08lx\n",			shdr_ctrl.sdram.w_addr.g_g_c		));
	Ddim_Print(("shdr_ctrl.sdram.w_addr.g_b   = 0x%08lx\n",			shdr_ctrl.sdram.w_addr.g_b			));
	Ddim_Print(("shdr_ctrl.sdram.w_addr.h_x   = 0x%08lx\n",			shdr_ctrl.sdram.w_addr.h_x			));
	Ddim_Print(("shdr_ctrl.sdram.w_addr.h_y   = 0x%08lx\n",			shdr_ctrl.sdram.w_addr.h_y			));
	Ddim_Print(("shdr_ctrl.sdram.w_addr.i     = 0x%08lx\n",			shdr_ctrl.sdram.w_addr.i			));
	Ddim_Print(("shdr_ctrl.sdram.w_ghsize.b_y   = 0x%08x\n",		shdr_ctrl.sdram.w_ghsize.b_y		));
	Ddim_Print(("shdr_ctrl.sdram.w_ghsize.b_c   = 0x%08x\n",		shdr_ctrl.sdram.w_ghsize.b_c		));
	Ddim_Print(("shdr_ctrl.sdram.w_ghsize.e_y   = 0x%08x\n",		shdr_ctrl.sdram.w_ghsize.e_y		));
	Ddim_Print(("shdr_ctrl.sdram.w_ghsize.e_c   = 0x%08x\n",		shdr_ctrl.sdram.w_ghsize.e_c		));
	Ddim_Print(("shdr_ctrl.sdram.w_ghsize.f_y   = 0x%08x\n",		shdr_ctrl.sdram.w_ghsize.f_y		));
	Ddim_Print(("shdr_ctrl.sdram.w_ghsize.f_c   = 0x%08x\n",		shdr_ctrl.sdram.w_ghsize.f_c		));
	Ddim_Print(("shdr_ctrl.sdram.w_ghsize.g_r_y = 0x%08x\n",		shdr_ctrl.sdram.w_ghsize.g_r_y		));
	Ddim_Print(("shdr_ctrl.sdram.w_ghsize.g_g_c = 0x%08x\n",		shdr_ctrl.sdram.w_ghsize.g_g_c		));
	Ddim_Print(("shdr_ctrl.sdram.w_ghsize.g_b   = 0x%08x\n",		shdr_ctrl.sdram.w_ghsize.g_b		));
	Ddim_Print(("shdr_ctrl.sdram.w_ghsize.h_x   = 0x%08x\n",		shdr_ctrl.sdram.w_ghsize.h_x		));
	Ddim_Print(("shdr_ctrl.sdram.w_ghsize.h_y   = 0x%08x\n",		shdr_ctrl.sdram.w_ghsize.h_y		));
	Ddim_Print(("shdr_ctrl.sdram.w_ghsize.i     = 0x%08x\n",		shdr_ctrl.sdram.w_ghsize.i			));
	Ddim_Print(("shdr_ctrl.sdram.w_vfm.a = 0x%08x\n",				shdr_ctrl.sdram.w_vfm.a				));
	Ddim_Print(("shdr_ctrl.sdram.w_vfm.b = 0x%08x\n",				shdr_ctrl.sdram.w_vfm.b				));
	Ddim_Print(("shdr_ctrl.sdram.w_baddr.a_y = 0x%08lx\n",			shdr_ctrl.sdram.w_baddr.a_y			));
	Ddim_Print(("shdr_ctrl.sdram.w_baddr.a_c = 0x%08lx\n",			shdr_ctrl.sdram.w_baddr.a_c			));
	Ddim_Print(("shdr_ctrl.sdram.w_baddr.c_y = 0x%08lx\n",			shdr_ctrl.sdram.w_baddr.c_y			));
	Ddim_Print(("shdr_ctrl.sdram.w_baddr.c_c = 0x%08lx\n",			shdr_ctrl.sdram.w_baddr.c_c			));
	Ddim_Print(("shdr_ctrl.sdram.w_baddr.d_y = 0x%08lx\n",			shdr_ctrl.sdram.w_baddr.d_y			));
	Ddim_Print(("shdr_ctrl.sdram.w_baddr.d_c = 0x%08lx\n",			shdr_ctrl.sdram.w_baddr.d_c			));
	Ddim_Print(("shdr_ctrl.sdram.w_hsize.a = 0x%08x\n",				shdr_ctrl.sdram.w_hsize.a			));
	Ddim_Print(("shdr_ctrl.sdram.w_hsize.c = 0x%08x\n",				shdr_ctrl.sdram.w_hsize.c			));
	Ddim_Print(("shdr_ctrl.sdram.w_hsize.d = 0x%08x\n",				shdr_ctrl.sdram.w_hsize.d			));
	Ddim_Print(("shdr_ctrl.compip.mode = 0x%08x\n",					shdr_ctrl.compip.mode				));
	Ddim_Print(("shdr_ctrl.compip.mc_mode = 0x%08x\n",				shdr_ctrl.compip.mc_mode			));
	Ddim_Print(("shdr_ctrl.compip.comp_incfmt = 0x%08x\n",			shdr_ctrl.compip.comp_incfmt		));
	Ddim_Print(("shdr_ctrl.compip.r_bypass_mode = 0x%08x\n",		shdr_ctrl.compip.r_bypass_mode		));
	Ddim_Print(("shdr_ctrl.compip.w_bypass_mode = 0x%08x\n",		shdr_ctrl.compip.w_bypass_mode		));
	Ddim_Print(("shdr_ctrl.compip.r_img_axi_size = 0x%08x\n",		shdr_ctrl.compip.r_img_axi_size		));
	Ddim_Print(("shdr_ctrl.compip.r_comp_axi_size = 0x%08x\n",		shdr_ctrl.compip.r_comp_axi_size	));
	Ddim_Print(("shdr_ctrl.compip.w_img_axi_size = 0x%08x\n",		shdr_ctrl.compip.w_img_axi_size		));
	Ddim_Print(("shdr_ctrl.compip.lint_no = 0x%08x\n",				shdr_ctrl.compip.lint_no			));
	Ddim_Print(("shdr_ctrl.compip.lint_mask = 0x%08x\n",			shdr_ctrl.compip.lint_mask			));
	Ddim_Print(("shdr_ctrl.compip.lint_sts = 0x%08x\n",				shdr_ctrl.compip.lint_sts			));
	Ddim_Print(("shdr_ctrl.compip.w_addr_y = 0x%08lx\n",			shdr_ctrl.compip.w_addr_y			));
	Ddim_Print(("shdr_ctrl.compip.w_addr_c = 0x%08lx\n",			shdr_ctrl.compip.w_addr_c			));
	Ddim_Print(("shdr_ctrl.compip.r_addr_y = 0x%08lx\n",			shdr_ctrl.compip.r_addr_y			));
	Ddim_Print(("shdr_ctrl.compip.r_addr_c = 0x%08lx\n",			shdr_ctrl.compip.r_addr_c			));
	Ddim_Print(("shdr_ctrl.image_mode.exp_mag = 0x%08x\n",			shdr_ctrl.image_mode.exp_mag		));
	Ddim_Print(("shdr_ctrl.image_mode.rexp_mag = 0x%08lx\n",		shdr_ctrl.image_mode.rexp_mag		));
	Ddim_Print(("shdr_ctrl.image_mode.rexp_mag_shift = 0x%08x\n",	shdr_ctrl.image_mode.rexp_mag_shift	));
	Ddim_Print(("shdr_ctrl.image_mode.shift_x = 0x%08x\n",			shdr_ctrl.image_mode.shift_x		));
	Ddim_Print(("shdr_ctrl.image_mode.shift_y = 0x%08x\n",			shdr_ctrl.image_mode.shift_y		));
	Ddim_Print(("shdr_ctrl.image_mode.fil_margin = 0x%08x\n",		shdr_ctrl.image_mode.fil_margin		));
	Ddim_Print(("shdr_ctrl.image_mode.fmd_en = 0x%08x\n",			shdr_ctrl.image_mode.fmd_en			));
	Ddim_Print(("shdr_ctrl.image_mode.bypass_en = 0x%08x\n",		shdr_ctrl.image_mode.bypass_en		));
	Ddim_Print(("shdr_ctrl.image_mode.smc_in = 0x%08x\n",			shdr_ctrl.image_mode.smc_in			));
	Ddim_Print(("shdr_ctrl.image_mode.shdr_in = 0x%08x\n",			shdr_ctrl.image_mode.shdr_in		));
	Ddim_Print(("shdr_ctrl.image_mode.tile_size = 0x%08x\n",		shdr_ctrl.image_mode.tile_size		));
}

static VOID ct_im_shdr_get_pre( VOID )
{
	INT32			ret;
	T_IM_SHDR_PRE	pre;

	ret = Im_SHDR_Get_Pre( &pre );

	Ddim_Print(("ct_im_shdr_get_pre[ret=0x%08x]\n", ret));
	Ddim_Print(("pre_mode.cus_mode = 0x%08x\n",				pre.pre_mode.cus_mode			));
	Ddim_Print(("pre_mode.degamma_mode = 0x%08x\n",			pre.pre_mode.degamma_mode		));
	Ddim_Print(("y2r_coef[ 0 ] = 0x%08x\n",					pre.y2r_coef[ 0 ]				));
	Ddim_Print(("y2r_coef[ 1 ] = 0x%08x\n",					pre.y2r_coef[ 1 ]				));
	Ddim_Print(("y2r_coef[ 2 ] = 0x%08x\n",					pre.y2r_coef[ 2 ]				));
	Ddim_Print(("y2r_coef[ 3 ] = 0x%08x\n",					pre.y2r_coef[ 3 ]				));
	Ddim_Print(("y2r_coef[ 4 ] = 0x%08x\n",					pre.y2r_coef[ 4 ]				));
	Ddim_Print(("y2r_coef[ 5 ] = 0x%08x\n",					pre.y2r_coef[ 5 ]				));
	Ddim_Print(("y2r_coef[ 6 ] = 0x%08x\n",					pre.y2r_coef[ 6 ]				));
	Ddim_Print(("y2r_coef[ 7 ] = 0x%08x\n",					pre.y2r_coef[ 7 ]				));
	Ddim_Print(("y2r_coef[ 8 ] = 0x%08x\n",					pre.y2r_coef[ 8 ]				));
	Ddim_Print(("y2r_clp.upper_long0[ 0 ] = 0x%08x\n",		pre.y2r_clp.upper_long0[ 0 ]	));
	Ddim_Print(("y2r_clp.upper_long0[ 1 ] = 0x%08x\n",		pre.y2r_clp.upper_long0[ 1 ]	));
	Ddim_Print(("y2r_clp.upper_long0[ 2 ] = 0x%08x\n",		pre.y2r_clp.upper_long0[ 2 ]	));
	Ddim_Print(("y2r_clp.lower_long0[ 0 ] = 0x%08x\n",		pre.y2r_clp.lower_long0[ 0 ]	));
	Ddim_Print(("y2r_clp.lower_long0[ 1 ] = 0x%08x\n",		pre.y2r_clp.lower_long0[ 1 ]	));
	Ddim_Print(("y2r_clp.lower_long0[ 2 ] = 0x%08x\n",		pre.y2r_clp.lower_long0[ 2 ]	));
	Ddim_Print(("y2r_clp.upper_short[ 0 ] = 0x%08x\n",		pre.y2r_clp.upper_short[ 0 ]	));
	Ddim_Print(("y2r_clp.upper_short[ 1 ] = 0x%08x\n",		pre.y2r_clp.upper_short[ 1 ]	));
	Ddim_Print(("y2r_clp.upper_short[ 2 ] = 0x%08x\n",		pre.y2r_clp.upper_short[ 2 ]	));
	Ddim_Print(("y2r_clp.lower_short[ 0 ] = 0x%08x\n",		pre.y2r_clp.lower_short[ 0 ]	));
	Ddim_Print(("y2r_clp.lower_short[ 1 ] = 0x%08x\n",		pre.y2r_clp.lower_short[ 1 ]	));
	Ddim_Print(("y2r_clp.lower_short[ 2 ] = 0x%08x\n",		pre.y2r_clp.lower_short[ 2 ]	));
	Ddim_Print(("r2y_coef[ 0 ] = 0x%08x\n",					pre.r2y_coef[ 0 ]				));
	Ddim_Print(("r2y_coef[ 1 ] = 0x%08x\n",					pre.r2y_coef[ 1 ]				));
	Ddim_Print(("r2y_coef[ 2 ] = 0x%08x\n",					pre.r2y_coef[ 2 ]				));
	Ddim_Print(("r2y_coef[ 3 ] = 0x%08x\n",					pre.r2y_coef[ 3 ]				));
	Ddim_Print(("r2y_coef[ 4 ] = 0x%08x\n",					pre.r2y_coef[ 4 ]				));
	Ddim_Print(("r2y_coef[ 5 ] = 0x%08x\n",					pre.r2y_coef[ 5 ]				));
	Ddim_Print(("r2y_coef[ 6 ] = 0x%08x\n",					pre.r2y_coef[ 6 ]				));
	Ddim_Print(("r2y_coef[ 7 ] = 0x%08x\n",					pre.r2y_coef[ 7 ]				));
	Ddim_Print(("r2y_coef[ 8 ] = 0x%08x\n",					pre.r2y_coef[ 8 ]				));
	Ddim_Print(("r2y_clp.upper_long_y  = 0x%08x\n",			pre.r2y_clp.upper_long_y		));
	Ddim_Print(("r2y_clp.upper_long_c[ 0 ] = 0x%08x\n",		pre.r2y_clp.upper_long_c[ 0 ]	));
	Ddim_Print(("r2y_clp.upper_long_c[ 1 ] = 0x%08x\n",		pre.r2y_clp.upper_long_c[ 1 ]	));
	Ddim_Print(("r2y_clp.lower_long_y = 0x%08x\n",			pre.r2y_clp.lower_long_y		));
	Ddim_Print(("r2y_clp.lower_long_c[ 0 ] = 0x%08x\n",		pre.r2y_clp.lower_long_c[ 0 ]	));
	Ddim_Print(("r2y_clp.lower_long_c[ 1 ] = 0x%08x\n",		pre.r2y_clp.lower_long_c[ 1 ]	));
	Ddim_Print(("r2y_clp.upper_short_y = 0x%08x\n",			pre.r2y_clp.upper_short_y		));
	Ddim_Print(("r2y_clp.upper_short_c[ 0 ] = 0x%08x\n",	pre.r2y_clp.upper_short_c[ 0 ]	));
	Ddim_Print(("r2y_clp.upper_short_c[ 1 ] = 0x%08x\n",	pre.r2y_clp.upper_short_c[ 1 ]	));
	Ddim_Print(("r2y_clp.lower_short_y = 0x%08x\n",			pre.r2y_clp.lower_short_y		));
	Ddim_Print(("r2y_clp.lower_short_c[ 0 ] = 0x%08x\n",	pre.r2y_clp.lower_short_c[ 0 ]	));
	Ddim_Print(("r2y_clp.lower_short_c[ 1 ] = 0x%08x\n",	pre.r2y_clp.lower_short_c[ 1 ]	));
	Ddim_Print(("short_knee.offset[ 0 ] = 0x%08x\n",		pre.short_knee.offset[ 0 ]		));
	Ddim_Print(("short_knee.offset[ 1 ] = 0x%08x\n",		pre.short_knee.offset[ 1 ]		));
	Ddim_Print(("short_knee.offset[ 2 ] = 0x%08x\n",		pre.short_knee.offset[ 2 ]		));
	Ddim_Print(("short_knee.slope[ 0 ] = 0x%08x\n",			pre.short_knee.slope[ 0 ]		));
	Ddim_Print(("short_knee.slope[ 1 ] = 0x%08x\n",			pre.short_knee.slope[ 1 ]		));
	Ddim_Print(("short_knee.slope[ 2 ] = 0x%08x\n",			pre.short_knee.slope[ 2 ]		));
	Ddim_Print(("short_knee.boundary[ 0 ] = 0x%08x\n",		pre.short_knee.boundary[ 0 ]	));
	Ddim_Print(("short_knee.boundary[ 1 ] = 0x%08x\n",		pre.short_knee.boundary[ 1 ]	));
	Ddim_Print(("long_knee.offset[ 0 ] = 0x%08x\n",			pre.long_knee.offset[ 0 ]		));
	Ddim_Print(("long_knee.offset[ 1 ] = 0x%08x\n",			pre.long_knee.offset[ 1 ]		));
	Ddim_Print(("long_knee.offset[ 2 ] = 0x%08x\n",			pre.long_knee.offset[ 2 ]		));
	Ddim_Print(("long_knee.slope[ 0 ] = 0x%08x\n",			pre.long_knee.slope[ 0 ]		));
	Ddim_Print(("long_knee.slope[ 1 ] = 0x%08x\n",			pre.long_knee.slope[ 1 ]		));
	Ddim_Print(("long_knee.slope[ 2 ] = 0x%08x\n",			pre.long_knee.slope[ 2 ]		));
	Ddim_Print(("long_knee.boundary[ 0 ] = 0x%08x\n",		pre.long_knee.boundary[ 0 ]		));
	Ddim_Print(("long_knee.boundary[ 1 ] = 0x%08x\n",		pre.long_knee.boundary[ 1 ]		));
}

static VOID ct_im_shdr_get_fmd( VOID )
{
	INT32			ret;
	T_IM_SHDR_FMD	fmd;

	ret = Im_SHDR_Get_Fmd( &fmd );

	Ddim_Print(("ct_im_shdr_get_fmd[ret=0x%08x]\n", ret));
	Ddim_Print(("ep_boundary[ 0 ]			= 0x%08x\n",	fmd.ep_boundary[ 0 ]			));
	Ddim_Print(("ep_boundary[ 1 ]			= 0x%08x\n",	fmd.ep_boundary[ 1 ]			));
	Ddim_Print(("ep_boundary[ 2 ]			= 0x%08x\n",	fmd.ep_boundary[ 2 ]			));
	Ddim_Print(("ep_boundary[ 3 ]			= 0x%08x\n",	fmd.ep_boundary[ 3 ]			));
	Ddim_Print(("ep_offset[ 0 ]				= 0x%08x\n",	fmd.ep_offset[ 0 ]				));
	Ddim_Print(("ep_offset[ 1 ]				= 0x%08x\n",	fmd.ep_offset[ 1 ]				));
	Ddim_Print(("ep_offset[ 2 ]				= 0x%08x\n",	fmd.ep_offset[ 2 ]				));
	Ddim_Print(("ep_offset[ 3 ]				= 0x%08x\n",	fmd.ep_offset[ 3 ]				));
	Ddim_Print(("ep_blend					= 0x%08x\n",	fmd.ep_blend					));
	Ddim_Print(("fd_weight					= 0x%08x\n",	fmd.fd_weight					));
	Ddim_Print(("mvcnt.bounds				= 0x%08x\n",	fmd.mvcnt.bounds				));
	Ddim_Print(("mvcnt.threshold			= 0x%08x\n",	fmd.mvcnt.threshold				));
	Ddim_Print(("mvcnt.gr					= 0x%08x\n",	fmd.mvcnt.gr					));
	Ddim_Print(("bldr_offset[ 0 ]			= 0x%08x\n",	fmd.bldr_offset[ 0 ]			));
	Ddim_Print(("bldr_offset[ 1 ]			= 0x%08x\n",	fmd.bldr_offset[ 1 ]			));
	Ddim_Print(("bldr_offset[ 2 ]			= 0x%08x\n",	fmd.bldr_offset[ 2 ]			));
	Ddim_Print(("bldr_offset[ 3 ]			= 0x%08x\n",	fmd.bldr_offset[ 3 ]			));
	Ddim_Print(("bldr_offset[ 4 ]			= 0x%08x\n",	fmd.bldr_offset[ 4 ]			));
	Ddim_Print(("bldr_slope[ 0 ]			= 0x%08x\n",	fmd.bldr_slope[ 0 ]				));
	Ddim_Print(("bldr_slope[ 1 ]			= 0x%08x\n",	fmd.bldr_slope[ 1 ]				));
	Ddim_Print(("bldr_slope[ 2 ]			= 0x%08x\n",	fmd.bldr_slope[ 2 ]				));
	Ddim_Print(("bldr_slope[ 3 ]			= 0x%08x\n",	fmd.bldr_slope[ 3 ]				));
	Ddim_Print(("bldr_slope[ 4 ]			= 0x%08x\n",	fmd.bldr_slope[ 4 ]				));
	Ddim_Print(("bldr_boundary[ 0 ]			= 0x%08x\n",	fmd.bldr_boundary[ 0 ]			));
	Ddim_Print(("bldr_boundary[ 1 ]			= 0x%08x\n",	fmd.bldr_boundary[ 1 ]			));
	Ddim_Print(("bldr_boundary[ 2 ]			= 0x%08x\n",	fmd.bldr_boundary[ 2 ]			));
	Ddim_Print(("bldr_boundary[ 3 ]			= 0x%08x\n",	fmd.bldr_boundary[ 3 ]			));
	Ddim_Print(("bldr_boundary[ 4 ]			= 0x%08x\n",	fmd.bldr_boundary[ 4 ]			));
	Ddim_Print(("weight_r2y_coef[ 0 ]		= 0x%08x\n",	fmd.weight_r2y_coef[ 0 ]		));
	Ddim_Print(("weight_r2y_coef[ 1 ]		= 0x%08x\n",	fmd.weight_r2y_coef[ 1 ]		));
	Ddim_Print(("weight_r2y_coef[ 2 ]		= 0x%08x\n",	fmd.weight_r2y_coef[ 2 ]		));
	Ddim_Print(("weight_r2y_clp.upper		= 0x%08x\n",	fmd.weight_r2y_clp.upper		));
	Ddim_Print(("weight_r2y_clp.lower		= 0x%08x\n",	fmd.weight_r2y_clp.lower		));
	Ddim_Print(("weight_base				= 0x%08x\n",	fmd.weight_base					));
	Ddim_Print(("weight_offset[ 0 ][ 0 ]	= 0x%08x\n",	fmd.weight_offset[ 0 ][ 0 ]		));
	Ddim_Print(("weight_offset[ 0 ][ 1 ]	= 0x%08x\n",	fmd.weight_offset[ 0 ][ 1 ]		));
	Ddim_Print(("weight_offset[ 0 ][ 2 ]	= 0x%08x\n",	fmd.weight_offset[ 0 ][ 2 ]		));
	Ddim_Print(("weight_offset[ 0 ][ 3 ]	= 0x%08x\n",	fmd.weight_offset[ 0 ][ 3 ]		));
	Ddim_Print(("weight_offset[ 0 ][ 4 ]	= 0x%08x\n",	fmd.weight_offset[ 0 ][ 4 ]		));
	Ddim_Print(("weight_offset[ 1 ][ 0 ]	= 0x%08x\n",	fmd.weight_offset[ 1 ][ 0 ]		));
	Ddim_Print(("weight_offset[ 1 ][ 1 ]	= 0x%08x\n",	fmd.weight_offset[ 1 ][ 1 ]		));
	Ddim_Print(("weight_offset[ 1 ][ 2 ]	= 0x%08x\n",	fmd.weight_offset[ 1 ][ 2 ]		));
	Ddim_Print(("weight_offset[ 1 ][ 3 ]	= 0x%08x\n",	fmd.weight_offset[ 1 ][ 3 ]		));
	Ddim_Print(("weight_offset[ 1 ][ 4 ]	= 0x%08x\n",	fmd.weight_offset[ 1 ][ 4 ]		));
	Ddim_Print(("weight_offset[ 2 ][ 0 ]	= 0x%08x\n",	fmd.weight_offset[ 2 ][ 0 ]		));
	Ddim_Print(("weight_offset[ 2 ][ 1 ]	= 0x%08x\n",	fmd.weight_offset[ 2 ][ 1 ]		));
	Ddim_Print(("weight_offset[ 2 ][ 2 ]	= 0x%08x\n",	fmd.weight_offset[ 2 ][ 2 ]		));
	Ddim_Print(("weight_offset[ 2 ][ 3 ]	= 0x%08x\n",	fmd.weight_offset[ 2 ][ 3 ]		));
	Ddim_Print(("weight_offset[ 2 ][ 4 ]	= 0x%08x\n",	fmd.weight_offset[ 2 ][ 4 ]		));
	Ddim_Print(("weight_slope[ 0 ][ 0 ]		= 0x%08x\n",	fmd.weight_slope[ 0 ][ 0 ]		));
	Ddim_Print(("weight_slope[ 0 ][ 1 ]		= 0x%08x\n",	fmd.weight_slope[ 0 ][ 1 ]		));
	Ddim_Print(("weight_slope[ 0 ][ 2 ]		= 0x%08x\n",	fmd.weight_slope[ 0 ][ 2 ]		));
	Ddim_Print(("weight_slope[ 0 ][ 3 ]		= 0x%08x\n",	fmd.weight_slope[ 0 ][ 3 ]		));
	Ddim_Print(("weight_slope[ 0 ][ 4 ]		= 0x%08x\n",	fmd.weight_slope[ 0 ][ 4 ]		));
	Ddim_Print(("weight_slope[ 1 ][ 0 ]		= 0x%08x\n",	fmd.weight_slope[ 1 ][ 0 ]		));
	Ddim_Print(("weight_slope[ 1 ][ 1 ]		= 0x%08x\n",	fmd.weight_slope[ 1 ][ 1 ]		));
	Ddim_Print(("weight_slope[ 1 ][ 2 ]		= 0x%08x\n",	fmd.weight_slope[ 1 ][ 2 ]		));
	Ddim_Print(("weight_slope[ 1 ][ 3 ]		= 0x%08x\n",	fmd.weight_slope[ 1 ][ 3 ]		));
	Ddim_Print(("weight_slope[ 1 ][ 4 ]		= 0x%08x\n",	fmd.weight_slope[ 1 ][ 4 ]		));
	Ddim_Print(("weight_slope[ 2 ][ 0 ]		= 0x%08x\n",	fmd.weight_slope[ 2 ][ 0 ]		));
	Ddim_Print(("weight_slope[ 2 ][ 1 ]		= 0x%08x\n",	fmd.weight_slope[ 2 ][ 1 ]		));
	Ddim_Print(("weight_slope[ 2 ][ 2 ]		= 0x%08x\n",	fmd.weight_slope[ 2 ][ 2 ]		));
	Ddim_Print(("weight_slope[ 2 ][ 3 ]		= 0x%08x\n",	fmd.weight_slope[ 2 ][ 3 ]		));
	Ddim_Print(("weight_slope[ 2 ][ 4 ]		= 0x%08x\n",	fmd.weight_slope[ 2 ][ 4 ]		));
//	Ddim_Print(("weight_boundary[ 0 ][ 0 ]	= 0x%08x\n",	fmd.weight_boundary[ 0 ][ 0 ]	));
	Ddim_Print(("weight_boundary[ 0 ][ 1 ]	= 0x%08x\n",	fmd.weight_boundary[ 0 ][ 1 ]	));
	Ddim_Print(("weight_boundary[ 0 ][ 2 ]	= 0x%08x\n",	fmd.weight_boundary[ 0 ][ 2 ]	));
	Ddim_Print(("weight_boundary[ 0 ][ 3 ]	= 0x%08x\n",	fmd.weight_boundary[ 0 ][ 3 ]	));
	Ddim_Print(("weight_boundary[ 0 ][ 4 ]	= 0x%08x\n",	fmd.weight_boundary[ 0 ][ 4 ]	));
//	Ddim_Print(("weight_boundary[ 1 ][ 0 ]	= 0x%08x\n",	fmd.weight_boundary[ 1 ][ 0 ]	));
	Ddim_Print(("weight_boundary[ 1 ][ 1 ]	= 0x%08x\n",	fmd.weight_boundary[ 1 ][ 1 ]	));
	Ddim_Print(("weight_boundary[ 1 ][ 2 ]	= 0x%08x\n",	fmd.weight_boundary[ 1 ][ 2 ]	));
	Ddim_Print(("weight_boundary[ 1 ][ 3 ]	= 0x%08x\n",	fmd.weight_boundary[ 1 ][ 3 ]	));
	Ddim_Print(("weight_boundary[ 1 ][ 4 ]	= 0x%08x\n",	fmd.weight_boundary[ 1 ][ 4 ]	));
//	Ddim_Print(("weight_boundary[ 2 ][ 0 ]	= 0x%08x\n",	fmd.weight_boundary[ 2 ][ 0 ]	));
	Ddim_Print(("weight_boundary[ 2 ][ 1 ]	= 0x%08x\n",	fmd.weight_boundary[ 2 ][ 1 ]	));
	Ddim_Print(("weight_boundary[ 2 ][ 2 ]	= 0x%08x\n",	fmd.weight_boundary[ 2 ][ 2 ]	));
	Ddim_Print(("weight_boundary[ 2 ][ 3 ]	= 0x%08x\n",	fmd.weight_boundary[ 2 ][ 3 ]	));
	Ddim_Print(("weight_boundary[ 2 ][ 4 ]	= 0x%08x\n",	fmd.weight_boundary[ 2 ][ 4 ]	));
	Ddim_Print(("iso_threshold				= 0x%08x\n",	fmd.iso_threshold				));
	Ddim_Print(("iso_coef[ 0 ]				= 0x%08x\n",	fmd.iso_coef[ 0 ]				));
	Ddim_Print(("iso_coef[ 1 ]				= 0x%08x\n",	fmd.iso_coef[ 1 ]				));
	Ddim_Print(("iso_coef[ 2 ]				= 0x%08x\n",	fmd.iso_coef[ 2 ]				));
	Ddim_Print(("iso_coef[ 3 ]				= 0x%08x\n",	fmd.iso_coef[ 3 ]				));
	Ddim_Print(("iso_coef[ 4 ]				= 0x%08x\n",	fmd.iso_coef[ 4 ]				));
	Ddim_Print(("iso_coef[ 5 ]				= 0x%08x\n",	fmd.iso_coef[ 5 ]				));
	Ddim_Print(("sat_threshold				= 0x%08x\n",	fmd.sat_threshold				));
	Ddim_Print(("exp77_mode					= 0x%08x\n",	fmd.exp77_mode					));
	Ddim_Print(("fmd_sel					= 0x%08x\n",	fmd.fmd_sel						));
}

static VOID ct_im_shdr_get_pmsk( VOID )
{
	INT32			ret;
	T_IM_SHDR_PMSK	pmsk;

	ret = Im_SHDR_Get_PreMask( &pmsk );

	Ddim_Print(("ct_im_shdr_get_pmsk[ret=0x%08x]\n", ret));
	Ddim_Print(("tlut[ 0 ]	= 0x%08x\n",	pmsk.tlut[ 0 ]	));
	Ddim_Print(("tlut[ 1 ]	= 0x%08x\n",	pmsk.tlut[ 1 ]	));
	Ddim_Print(("tlut[ 2 ]	= 0x%08x\n",	pmsk.tlut[ 2 ]	));
	Ddim_Print(("tlut[ 3 ]	= 0x%08x\n",	pmsk.tlut[ 3 ]	));
	Ddim_Print(("tlut[ 4 ]	= 0x%08x\n",	pmsk.tlut[ 4 ]	));
	Ddim_Print(("tlut[ 5 ]	= 0x%08x\n",	pmsk.tlut[ 5 ]	));
	Ddim_Print(("tlut[ 6 ]	= 0x%08x\n",	pmsk.tlut[ 6 ]	));
	Ddim_Print(("tlut[ 7 ]	= 0x%08x\n",	pmsk.tlut[ 7 ]	));
	Ddim_Print(("tlut[ 8 ]	= 0x%08x\n",	pmsk.tlut[ 8 ]	));
	Ddim_Print(("tlut[ 9 ]	= 0x%08x\n",	pmsk.tlut[ 9 ]	));
	Ddim_Print(("tlut[ 10 ]	= 0x%08x\n",	pmsk.tlut[ 10 ]	));
	Ddim_Print(("tlut[ 11 ]	= 0x%08x\n",	pmsk.tlut[ 11 ]	));
	Ddim_Print(("tlut[ 12 ]	= 0x%08x\n",	pmsk.tlut[ 12 ]	));
	Ddim_Print(("tlut[ 13 ]	= 0x%08x\n",	pmsk.tlut[ 13 ]	));
	Ddim_Print(("tlut[ 14 ]	= 0x%08x\n",	pmsk.tlut[ 14 ]	));
	Ddim_Print(("tlut[ 15 ]	= 0x%08x\n",	pmsk.tlut[ 15 ]	));
	Ddim_Print(("tlut[ 16 ]	= 0x%08x\n",	pmsk.tlut[ 16 ]	));
	Ddim_Print(("pmsk_mode	= 0x%08x\n",	pmsk.pmsk_mode	));
}

static VOID ct_im_shdr_get_spnr( VOID )
{
	INT32			ret;
	T_IM_SHDR_SPNR	spnr;

	ret = Im_SHDR_Get_Spnr( &spnr );

	Ddim_Print(("ct_im_shdr_get_spnr[ret=0x%08x]\n", ret));
	Ddim_Print(("spnr_en			= 0x%08x\n",	spnr.spnr_en			));
	Ddim_Print(("spe_blend			= 0x%08x\n",	spnr.spe_blend			));
//	Ddim_Print(("spe_boundary[ 0 ]	= 0x%08x\n",	spnr.spe_boundary[ 0 ]	));
	Ddim_Print(("spe_boundary[ 1 ]	= 0x%08x\n",	spnr.spe_boundary[ 1 ]	));
	Ddim_Print(("spe_boundary[ 2 ]	= 0x%08x\n",	spnr.spe_boundary[ 2 ]	));
	Ddim_Print(("spe_boundary[ 3 ]	= 0x%08x\n",	spnr.spe_boundary[ 3 ]	));
	Ddim_Print(("spe_offset[ 0 ]	= 0x%08x\n",	spnr.spe_offset[ 0 ]	));
	Ddim_Print(("spe_offset[ 1 ]	= 0x%08x\n",	spnr.spe_offset[ 1 ]	));
	Ddim_Print(("spe_offset[ 2 ]	= 0x%08x\n",	spnr.spe_offset[ 2 ]	));
	Ddim_Print(("spe_offset[ 3 ]	= 0x%08x\n",	spnr.spe_offset[ 3 ]	));
}

static VOID ct_im_shdr_get_mask( VOID )
{
	INT32			ret;
	T_IM_SHDR_MSK	msk;

	ret = Im_SHDR_Get_Mask( &msk );

	Ddim_Print(("ct_im_shdr_get_msk[ret=0x%08x]\n", ret));
	Ddim_Print(("y2r_clp_upper_long[ 0 ]	= 0x%08x\n",	msk.y2r_clp_upper_long[ 0 ]	));
	Ddim_Print(("y2r_clp_upper_long[ 1 ]	= 0x%08x\n",	msk.y2r_clp_upper_long[ 1 ]	));
	Ddim_Print(("y2r_clp_upper_long[ 2 ]	= 0x%08x\n",	msk.y2r_clp_upper_long[ 2 ]	));
	Ddim_Print(("y2r_clp_lower_long[ 0 ]	= 0x%08x\n",	msk.y2r_clp_lower_long[ 0 ]	));
	Ddim_Print(("y2r_clp_lower_long[ 1 ]	= 0x%08x\n",	msk.y2r_clp_lower_long[ 1 ]	));
	Ddim_Print(("y2r_clp_lower_long[ 2 ]	= 0x%08x\n",	msk.y2r_clp_lower_long[ 2 ]	));
	Ddim_Print(("y2r_clp_upper_short[ 0 ]	= 0x%08x\n",	msk.y2r_clp_upper_short[ 0 ]));
	Ddim_Print(("y2r_clp_upper_short[ 1 ]	= 0x%08x\n",	msk.y2r_clp_upper_short[ 1 ]));
	Ddim_Print(("y2r_clp_upper_short[ 2 ]	= 0x%08x\n",	msk.y2r_clp_upper_short[ 2 ]));
	Ddim_Print(("y2r_clp_lower_short[ 0 ]	= 0x%08x\n",	msk.y2r_clp_lower_short[ 0 ]));
	Ddim_Print(("y2r_clp_lower_short[ 1 ]	= 0x%08x\n",	msk.y2r_clp_lower_short[ 1 ]));
	Ddim_Print(("y2r_clp_lower_short[ 2 ]	= 0x%08x\n",	msk.y2r_clp_lower_short[ 2 ]));
	Ddim_Print(("alpha						= 0x%08x\n",	msk.alpha					));
	Ddim_Print(("lpfc[ 0 ][ 0 ]				= 0x%08x\n",	msk.lpfc[ 0 ][ 0 ]			));
	Ddim_Print(("lpfc[ 0 ][ 1 ]				= 0x%08x\n",	msk.lpfc[ 0 ][ 1 ]			));
	Ddim_Print(("lpfc[ 0 ][ 2 ]				= 0x%08x\n",	msk.lpfc[ 0 ][ 2 ]			));
	Ddim_Print(("lpfc[ 1 ][ 0 ]				= 0x%08x\n",	msk.lpfc[ 1 ][ 0 ]			));
	Ddim_Print(("lpfc[ 1 ][ 1 ]				= 0x%08x\n",	msk.lpfc[ 1 ][ 1 ]			));
	Ddim_Print(("lpfc[ 1 ][ 2 ]				= 0x%08x\n",	msk.lpfc[ 1 ][ 2 ]			));
	Ddim_Print(("lpfc[ 2 ][ 0 ]				= 0x%08x\n",	msk.lpfc[ 2 ][ 0 ]			));
	Ddim_Print(("lpfc[ 2 ][ 1 ]				= 0x%08x\n",	msk.lpfc[ 2 ][ 1 ]			));
	Ddim_Print(("lpfc[ 2 ][ 2 ]				= 0x%08x\n",	msk.lpfc[ 2 ][ 2 ]			));
	Ddim_Print(("blend_offset[ 0 ]			= 0x%08x\n",	msk.blend_offset[ 0 ]		));
	Ddim_Print(("blend_offset[ 1 ]			= 0x%08x\n",	msk.blend_offset[ 1 ]		));
	Ddim_Print(("blend_offset[ 2 ]			= 0x%08x\n",	msk.blend_offset[ 2 ]		));
	Ddim_Print(("blend_offset[ 3 ]			= 0x%08x\n",	msk.blend_offset[ 3 ]		));
	Ddim_Print(("blend_offset[ 4 ]			= 0x%08x\n",	msk.blend_offset[ 4 ]		));
	Ddim_Print(("blend_offset[ 5 ]			= 0x%08x\n",	msk.blend_offset[ 5 ]		));
	Ddim_Print(("blend_offset[ 6 ]			= 0x%08x\n",	msk.blend_offset[ 6 ]		));
	Ddim_Print(("blend_slope[ 0 ]			= 0x%08x\n",	msk.blend_slope[ 0 ]		));
	Ddim_Print(("blend_slope[ 1 ]			= 0x%08x\n",	msk.blend_slope[ 1 ]		));
	Ddim_Print(("blend_slope[ 2 ]			= 0x%08x\n",	msk.blend_slope[ 2 ]		));
	Ddim_Print(("blend_slope[ 3 ]			= 0x%08x\n",	msk.blend_slope[ 3 ]		));
	Ddim_Print(("blend_slope[ 4 ]			= 0x%08x\n",	msk.blend_slope[ 4 ]		));
	Ddim_Print(("blend_slope[ 5 ]			= 0x%08x\n",	msk.blend_slope[ 5 ]		));
	Ddim_Print(("blend_slope[ 6 ]			= 0x%08x\n",	msk.blend_slope[ 6 ]		));
//	Ddim_Print(("blend_boundary[ 0 ]		= 0x%08x\n",	msk.blend_boundary[ 0 ]		));
	Ddim_Print(("blend_boundary[ 1 ]		= 0x%08x\n",	msk.blend_boundary[ 1 ]		));
	Ddim_Print(("blend_boundary[ 2 ]		= 0x%08x\n",	msk.blend_boundary[ 2 ]		));
	Ddim_Print(("blend_boundary[ 3 ]		= 0x%08x\n",	msk.blend_boundary[ 3 ]		));
	Ddim_Print(("blend_boundary[ 4 ]		= 0x%08x\n",	msk.blend_boundary[ 4 ]		));
	Ddim_Print(("blend_boundary[ 5 ]		= 0x%08x\n",	msk.blend_boundary[ 5 ]		));
	Ddim_Print(("blend_boundary[ 6 ]		= 0x%08x\n",	msk.blend_boundary[ 6 ]		));
	Ddim_Print(("norm_gain[ 0 ]				= 0x%08x\n",	msk.norm_gain[ 0 ]			));
	Ddim_Print(("norm_gain[ 1 ]				= 0x%08x\n",	msk.norm_gain[ 1 ]			));
	Ddim_Print(("norm_gain[ 2 ]				= 0x%08x\n",	msk.norm_gain[ 2 ]			));
	Ddim_Print(("norm_offset[ 0 ]			= 0x%08x\n",	msk.norm_offset[ 0 ]		));
	Ddim_Print(("norm_offset[ 1 ]			= 0x%08x\n",	msk.norm_offset[ 1 ]		));
	Ddim_Print(("norm_offset[ 2 ]			= 0x%08x\n",	msk.norm_offset[ 2 ]		));
	Ddim_Print(("msk_mode					= 0x%08x\n",	msk.msk_mode				));
}

static VOID ct_im_shdr_get_bld( VOID )
{
	INT32			ret;
	T_IM_SHDR_BLD	bld;

	ret = Im_SHDR_Get_Blend( &bld );

	Ddim_Print(("ct_im_shdr_get_bld[ret=0x%08x]\n", ret));
	Ddim_Print(("blend_sel		= 0x%08x\n",	bld.blend_sel		));
	Ddim_Print(("alpha_sel		= 0x%08x\n",	bld.alpha_sel		));
	Ddim_Print(("alpha_val[ 0 ]	= 0x%08x\n",	bld.alpha_val[ 0 ]	));
	Ddim_Print(("alpha_val[ 1 ]	= 0x%08x\n",	bld.alpha_val[ 1 ]	));
	Ddim_Print(("alpha_val[ 2 ]	= 0x%08x\n",	bld.alpha_val[ 2 ]	));
}

static VOID ct_im_shdr_get_post( VOID )
{
	INT32			ret;
	T_IM_SHDR_POST	post;

	ret = Im_SHDR_Get_Post( &post );

	Ddim_Print(("ct_im_shdr_get_post[ret=0x%08x]\n", ret));
	Ddim_Print(("sft1rnd.sft1rnd_en		= 0x%08x\n",	post.sft1rnd.sft1rnd_en		));
	Ddim_Print(("y2r_coef[ 0 ]			= 0x%08x\n",	post.y2r_coef[ 0 ]			));
	Ddim_Print(("y2r_coef[ 1 ]			= 0x%08x\n",	post.y2r_coef[ 1 ]			));
	Ddim_Print(("y2r_coef[ 2 ]			= 0x%08x\n",	post.y2r_coef[ 2 ]			));
	Ddim_Print(("y2r_coef[ 3 ]			= 0x%08x\n",	post.y2r_coef[ 3 ]			));
	Ddim_Print(("y2r_coef[ 4 ]			= 0x%08x\n",	post.y2r_coef[ 4 ]			));
	Ddim_Print(("y2r_coef[ 5 ]			= 0x%08x\n",	post.y2r_coef[ 5 ]			));
	Ddim_Print(("y2r_coef[ 6 ]			= 0x%08x\n",	post.y2r_coef[ 6 ]			));
	Ddim_Print(("y2r_coef[ 7 ]			= 0x%08x\n",	post.y2r_coef[ 7 ]			));
	Ddim_Print(("y2r_coef[ 8 ]			= 0x%08x\n",	post.y2r_coef[ 8 ]			));
	Ddim_Print(("y2r_clp_th_upper[ 0 ]	= 0x%08x\n",	post.y2r_clp_th_upper[ 0 ]	));
	Ddim_Print(("y2r_clp_th_upper[ 1 ]	= 0x%08x\n",	post.y2r_clp_th_upper[ 1 ]	));
	Ddim_Print(("y2r_clp_th_upper[ 2 ]	= 0x%08x\n",	post.y2r_clp_th_upper[ 2 ]	));
	Ddim_Print(("y2r_clp_th_lower[ 0 ]	= 0x%08x\n",	post.y2r_clp_th_lower[ 0 ]	));
	Ddim_Print(("y2r_clp_th_lower[ 1 ]	= 0x%08x\n",	post.y2r_clp_th_lower[ 1 ]	));
	Ddim_Print(("y2r_clp_th_lower[ 2 ]	= 0x%08x\n",	post.y2r_clp_th_lower[ 2 ]	));
	Ddim_Print(("ev_sel					= 0x%08x\n",	post.ev_sel					));
	Ddim_Print(("mulqpos				= 0x%08x\n",	post.mulqpos				));
	Ddim_Print(("ev_kn_boundary[ 0 ]	= 0x%08x\n",	post.ev_kn_boundary[ 0 ]	));
	Ddim_Print(("ev_kn_boundary[ 1 ]	= 0x%08x\n",	post.ev_kn_boundary[ 1 ]	));
	Ddim_Print(("ev_kn_boundary[ 2 ]	= 0x%08x\n",	post.ev_kn_boundary[ 2 ]	));
	Ddim_Print(("ev_kn_boundary[ 3 ]	= 0x%08x\n",	post.ev_kn_boundary[ 3 ]	));
	Ddim_Print(("ev_kn_boundary[ 4 ]	= 0x%08x\n",	post.ev_kn_boundary[ 4 ]	));
	Ddim_Print(("ev_kn_boundary[ 5 ]	= 0x%08x\n",	post.ev_kn_boundary[ 5 ]	));
	Ddim_Print(("ev_kn_boundary[ 6 ]	= 0x%08x\n",	post.ev_kn_boundary[ 6 ]	));
	Ddim_Print(("ev_kn_boundary[ 7 ]	= 0x%08x\n",	post.ev_kn_boundary[ 7 ]	));
	Ddim_Print(("ev_kn_boundary[ 8 ]	= 0x%08x\n",	post.ev_kn_boundary[ 8 ]	));
	Ddim_Print(("ev_kn_boundary[ 9 ]	= 0x%08x\n",	post.ev_kn_boundary[ 9 ]	));
	Ddim_Print(("ev_kn_offset[ 0 ]		= 0x%08x\n",	post.ev_kn_offset[ 0 ]		));
	Ddim_Print(("ev_kn_offset[ 1 ]		= 0x%08x\n",	post.ev_kn_offset[ 1 ]		));
	Ddim_Print(("ev_kn_offset[ 2 ]		= 0x%08x\n",	post.ev_kn_offset[ 2 ]		));
	Ddim_Print(("ev_kn_offset[ 3 ]		= 0x%08x\n",	post.ev_kn_offset[ 3 ]		));
	Ddim_Print(("ev_kn_offset[ 4 ]		= 0x%08x\n",	post.ev_kn_offset[ 4 ]		));
	Ddim_Print(("ev_kn_offset[ 5 ]		= 0x%08x\n",	post.ev_kn_offset[ 5 ]		));
	Ddim_Print(("ev_kn_offset[ 6 ]		= 0x%08x\n",	post.ev_kn_offset[ 6 ]		));
	Ddim_Print(("ev_kn_offset[ 7 ]		= 0x%08x\n",	post.ev_kn_offset[ 7 ]		));
	Ddim_Print(("ev_kn_offset[ 8 ]		= 0x%08x\n",	post.ev_kn_offset[ 8 ]		));
	Ddim_Print(("ev_kn_offset[ 9 ]		= 0x%08x\n",	post.ev_kn_offset[ 9 ]		));
	Ddim_Print(("ev_kn_slope[ 0 ]		= 0x%08x\n",	post.ev_kn_slope[ 0 ]		));
	Ddim_Print(("ev_kn_slope[ 1 ]		= 0x%08x\n",	post.ev_kn_slope[ 1 ]		));
	Ddim_Print(("ev_kn_slope[ 2 ]		= 0x%08x\n",	post.ev_kn_slope[ 2 ]		));
	Ddim_Print(("ev_kn_slope[ 3 ]		= 0x%08x\n",	post.ev_kn_slope[ 3 ]		));
	Ddim_Print(("ev_kn_slope[ 4 ]		= 0x%08x\n",	post.ev_kn_slope[ 4 ]		));
	Ddim_Print(("ev_kn_slope[ 5 ]		= 0x%08x\n",	post.ev_kn_slope[ 5 ]		));
	Ddim_Print(("ev_kn_slope[ 6 ]		= 0x%08x\n",	post.ev_kn_slope[ 6 ]		));
	Ddim_Print(("ev_kn_slope[ 7 ]		= 0x%08x\n",	post.ev_kn_slope[ 7 ]		));
	Ddim_Print(("ev_kn_slope[ 8 ]		= 0x%08x\n",	post.ev_kn_slope[ 8 ]		));
	Ddim_Print(("ev_kn_slope[ 9 ]		= 0x%08x\n",	post.ev_kn_slope[ 9 ]		));
	Ddim_Print(("css_sel				= 0x%08x\n",	post.css_sel				));
	Ddim_Print(("hdrcss_mode			= 0x%08x\n",	post.hdrcss_mode			));
}

static VOID ct_im_shdr_get_resize( VOID )
{
	INT32				ret;
	T_IM_SHDR_RESIZE	resize;

	ret = Im_SHDR_Get_Resize( &resize );

	Ddim_Print(("ct_im_shdr_get_resize[ret=0x%08x]\n", ret));
	Ddim_Print(("chmode.a_w_en		= 0x%08x\n",	resize.chmode.a_w_en	));
	Ddim_Print(("chmode.a_drc_en	= 0x%08x\n",	resize.chmode.a_drc_en	));
	Ddim_Print(("chmode.a_css_md	= 0x%08x\n",	resize.chmode.a_css_md	));
	Ddim_Print(("chmode.b_w_en		= 0x%08x\n",	resize.chmode.b_w_en	));
	Ddim_Print(("chmode.b_rdc_md	= 0x%08x\n",	resize.chmode.b_rdc_md	));
	Ddim_Print(("chmode.b_css_md	= 0x%08x\n",	resize.chmode.b_css_md	));
	Ddim_Print(("chmode.c_w_en		= 0x%08x\n",	resize.chmode.c_w_en	));
	Ddim_Print(("chmode.c_rsz_md	= 0x%08x\n",	resize.chmode.c_rsz_md	));
	Ddim_Print(("chmode.c_rdc_en	= 0x%08x\n",	resize.chmode.c_rdc_en	));
	Ddim_Print(("chmode.c_rdc_md	= 0x%08x\n",	resize.chmode.c_rdc_md	));
	Ddim_Print(("chmode.c_trm_en	= 0x%08x\n",	resize.chmode.c_trm_en	));
	Ddim_Print(("chmode.c_drc_en	= 0x%08x\n",	resize.chmode.c_drc_en	));
	Ddim_Print(("chmode.c_css_md	= 0x%08x\n",	resize.chmode.c_css_md	));
	Ddim_Print(("chmode.d_w_en		= 0x%08x\n",	resize.chmode.d_w_en	));
	Ddim_Print(("chmode.d_rsz_md	= 0x%08x\n",	resize.chmode.d_rsz_md	));
	Ddim_Print(("chmode.d_rdc_en	= 0x%08x\n",	resize.chmode.d_rdc_en	));
	Ddim_Print(("chmode.d_rdc_md	= 0x%08x\n",	resize.chmode.d_rdc_md	));
	Ddim_Print(("chmode.d_trm_en	= 0x%08x\n",	resize.chmode.d_trm_en	));
	Ddim_Print(("chmode.d_drc_en	= 0x%08x\n",	resize.chmode.d_drc_en	));
	Ddim_Print(("chmode.d_css_md	= 0x%08x\n",	resize.chmode.d_css_md	));
	Ddim_Print(("chmode.e_w_en		= 0x%08x\n",	resize.chmode.e_w_en	));
	Ddim_Print(("chmode.e_rsz_md	= 0x%08x\n",	resize.chmode.e_rsz_md	));
	Ddim_Print(("chmode.e_rdc_en	= 0x%08x\n",	resize.chmode.e_rdc_en	));
	Ddim_Print(("chmode.e_rdc_md	= 0x%08x\n",	resize.chmode.e_rdc_md	));
	Ddim_Print(("chmode.e_trm_en	= 0x%08x\n",	resize.chmode.e_trm_en	));
	Ddim_Print(("chmode.e_css_md	= 0x%08x\n",	resize.chmode.e_css_md	));
	Ddim_Print(("chmode.f_w_en		= 0x%08x\n",	resize.chmode.f_w_en	));
	Ddim_Print(("chmode.f_rsz_md	= 0x%08x\n",	resize.chmode.f_rsz_md	));
	Ddim_Print(("chmode.f_rdc_en	= 0x%08x\n",	resize.chmode.f_rdc_en	));
	Ddim_Print(("chmode.f_rdc_md	= 0x%08x\n",	resize.chmode.f_rdc_md	));
	Ddim_Print(("chmode.f_trm_en	= 0x%08x\n",	resize.chmode.f_trm_en	));
	Ddim_Print(("chmode.f_css_md	= 0x%08x\n",	resize.chmode.f_css_md	));
	Ddim_Print(("woutsz.a_hsize		= 0x%08x\n",	resize.woutsz.a_hsize	));
	Ddim_Print(("woutsz.a_vsize		= 0x%08x\n",	resize.woutsz.a_vsize	));
	Ddim_Print(("woutsz.b_hsize		= 0x%08x\n",	resize.woutsz.b_hsize	));
	Ddim_Print(("woutsz.b_vsize		= 0x%08x\n",	resize.woutsz.b_vsize	));
	Ddim_Print(("woutsz.c_hsize		= 0x%08x\n",	resize.woutsz.c_hsize	));
	Ddim_Print(("woutsz.c_vsize		= 0x%08x\n",	resize.woutsz.c_vsize	));
	Ddim_Print(("woutsz.d_hsize		= 0x%08x\n",	resize.woutsz.d_hsize	));
	Ddim_Print(("woutsz.d_vsize		= 0x%08x\n",	resize.woutsz.d_vsize	));
	Ddim_Print(("woutsz.e_hsize		= 0x%08x\n",	resize.woutsz.e_hsize	));
	Ddim_Print(("woutsz.e_vsize		= 0x%08x\n",	resize.woutsz.e_vsize	));
	Ddim_Print(("woutsz.f_hsize		= 0x%08x\n",	resize.woutsz.f_hsize	));
	Ddim_Print(("woutsz.f_vsize		= 0x%08x\n",	resize.woutsz.f_vsize	));
	Ddim_Print(("rszhcnt.c_hsta		= 0x%08x\n",	resize.rszhcnt.c_hsta	));
	Ddim_Print(("rszhcnt.c_hpit		= 0x%08x\n",	resize.rszhcnt.c_hpit	));
	Ddim_Print(("rszhcnt.d_hsta		= 0x%08x\n",	resize.rszhcnt.d_hsta	));
	Ddim_Print(("rszhcnt.d_hpit		= 0x%08x\n",	resize.rszhcnt.d_hpit	));
	Ddim_Print(("rszhcnt.e_hsta		= 0x%08x\n",	resize.rszhcnt.e_hsta	));
	Ddim_Print(("rszhcnt.e_hpit		= 0x%08x\n",	resize.rszhcnt.e_hpit	));
	Ddim_Print(("rszhcnt.f_hsta		= 0x%08x\n",	resize.rszhcnt.f_hsta	));
	Ddim_Print(("rszhcnt.f_hpit		= 0x%08x\n",	resize.rszhcnt.f_hpit	));
	Ddim_Print(("rszvcnt.c_vsta		= 0x%08x\n",	resize.rszvcnt.c_vsta	));
	Ddim_Print(("rszvcnt.c_vpit		= 0x%08x\n",	resize.rszvcnt.c_vpit	));
	Ddim_Print(("rszvcnt.d_vsta		= 0x%08x\n",	resize.rszvcnt.d_vsta	));
	Ddim_Print(("rszvcnt.d_vpit		= 0x%08x\n",	resize.rszvcnt.d_vpit	));
	Ddim_Print(("rszvcnt.e_vsta		= 0x%08x\n",	resize.rszvcnt.e_vsta	));
	Ddim_Print(("rszvcnt.e_vpit		= 0x%08x\n",	resize.rszvcnt.e_vpit	));
	Ddim_Print(("rszvcnt.f_vsta		= 0x%08x\n",	resize.rszvcnt.f_vsta	));
	Ddim_Print(("rszvcnt.f_vpit		= 0x%08x\n",	resize.rszvcnt.f_vpit	));
	Ddim_Print(("trmsta.c_hsta		= 0x%08x\n",	resize.trmsta.c_hsta	));
	Ddim_Print(("trmsta.c_vsta		= 0x%08x\n",	resize.trmsta.c_vsta	));
	Ddim_Print(("trmsta.d_hsta		= 0x%08x\n",	resize.trmsta.d_hsta	));
	Ddim_Print(("trmsta.d_vsta		= 0x%08x\n",	resize.trmsta.d_vsta	));
	Ddim_Print(("trmsta.e_hsta		= 0x%08x\n",	resize.trmsta.e_hsta	));
	Ddim_Print(("trmsta.e_vsta		= 0x%08x\n",	resize.trmsta.e_vsta	));
	Ddim_Print(("trmsta.f_hsta		= 0x%08x\n",	resize.trmsta.f_hsta	));
	Ddim_Print(("trmsta.f_vsta		= 0x%08x\n",	resize.trmsta.f_vsta	));
	Ddim_Print(("drcofst.a_y		= 0x%08x\n",	resize.drcofst.a_y		));
	Ddim_Print(("drcofst.a_cb		= 0x%08x\n",	resize.drcofst.a_cb		));
	Ddim_Print(("drcofst.a_cr		= 0x%08x\n",	resize.drcofst.a_cr		));
	Ddim_Print(("drcofst.c_y		= 0x%08x\n",	resize.drcofst.c_y		));
	Ddim_Print(("drcofst.c_cb		= 0x%08x\n",	resize.drcofst.c_cb		));
	Ddim_Print(("drcofst.c_cr		= 0x%08x\n",	resize.drcofst.c_cr		));
	Ddim_Print(("drcofst.d_y		= 0x%08x\n",	resize.drcofst.d_y		));
	Ddim_Print(("drcofst.d_cb		= 0x%08x\n",	resize.drcofst.d_cb		));
	Ddim_Print(("drcofst.d_cr		= 0x%08x\n",	resize.drcofst.d_cr		));
	Ddim_Print(("drcgain.a_y		= 0x%08x\n",	resize.drcgain.a_y		));
	Ddim_Print(("drcgain.a_cb		= 0x%08x\n",	resize.drcgain.a_cb		));
	Ddim_Print(("drcgain.a_cr		= 0x%08x\n",	resize.drcgain.a_cr		));
	Ddim_Print(("drcgain.c_y		= 0x%08x\n",	resize.drcgain.c_y		));
	Ddim_Print(("drcgain.c_cb		= 0x%08x\n",	resize.drcgain.c_cb		));
	Ddim_Print(("drcgain.c_cr		= 0x%08x\n",	resize.drcgain.c_cr		));
	Ddim_Print(("drcgain.d_y		= 0x%08x\n",	resize.drcgain.d_y		));
	Ddim_Print(("drcgain.d_cb		= 0x%08x\n",	resize.drcgain.d_cb		));
	Ddim_Print(("drcgain.d_cr		= 0x%08x\n",	resize.drcgain.d_cr		));
	Ddim_Print(("drcclph.a_y		= 0x%08x\n",	resize.drcclph.a_y		));
	Ddim_Print(("drcclph.a_cb		= 0x%08x\n",	resize.drcclph.a_cb		));
	Ddim_Print(("drcclph.a_cr		= 0x%08x\n",	resize.drcclph.a_cr		));
	Ddim_Print(("drcclph.c_y		= 0x%08x\n",	resize.drcclph.c_y		));
	Ddim_Print(("drcclph.c_cb		= 0x%08x\n",	resize.drcclph.c_cb		));
	Ddim_Print(("drcclph.c_cr		= 0x%08x\n",	resize.drcclph.c_cr		));
	Ddim_Print(("drcclph.d_y		= 0x%08x\n",	resize.drcclph.d_y		));
	Ddim_Print(("drcclph.d_cb		= 0x%08x\n",	resize.drcclph.d_cb		));
	Ddim_Print(("drcclph.d_cr		= 0x%08x\n",	resize.drcclph.d_cr		));
	Ddim_Print(("drcclpl.a_y		= 0x%08x\n",	resize.drcclpl.a_y		));
	Ddim_Print(("drcclpl.a_cb		= 0x%08x\n",	resize.drcclpl.a_cb		));
	Ddim_Print(("drcclpl.a_cr		= 0x%08x\n",	resize.drcclpl.a_cr		));
	Ddim_Print(("drcclpl.c_y		= 0x%08x\n",	resize.drcclpl.c_y		));
	Ddim_Print(("drcclpl.c_cb		= 0x%08x\n",	resize.drcclpl.c_cb		));
	Ddim_Print(("drcclpl.c_cr		= 0x%08x\n",	resize.drcclpl.c_cr		));
	Ddim_Print(("drcclpl.d_y		= 0x%08x\n",	resize.drcclpl.d_y		));
	Ddim_Print(("drcclpl.d_cb		= 0x%08x\n",	resize.drcclpl.d_cb		));
	Ddim_Print(("drcclpl.d_cr		= 0x%08x\n",	resize.drcclpl.d_cr		));
}

static VOID ct_im_shdr_get_smc( VOID )
{
	INT32			ret;
	T_IM_SHDR_SMC	smc;

	ret = Im_SHDR_Get_Smc( &smc );

	Ddim_Print(("ct_im_shdr_get_smc[ret=0x%08x]\n", ret));
	Ddim_Print(("mcarmd.mca_mode			= 0x%08x\n",	smc.mcarmd.mca_mode				));
	Ddim_Print(("mcarmd.mcr_mode			= 0x%08x\n",	smc.mcarmd.mcr_mode				));
	Ddim_Print(("mcmpmd.mcmp_mode			= 0x%08x\n",	smc.mcmpmd.mcmp_mode			));
	Ddim_Print(("mcmpmd.mvw_en				= 0x%08x\n",	smc.mcmpmd.mvw_en				));
	Ddim_Print(("mmvval.mmv_x				= 0x%08x\n",	smc.mmvval.mmv_x				));
	Ddim_Print(("mmvval.mmv_y				= 0x%08x\n",	smc.mmvval.mmv_y				));
	Ddim_Print(("mmvrcnt.mmvro_x			= 0x%08x\n",	smc.mmvrcnt.mmvro_x				));
	Ddim_Print(("mmvrcnt.mmvro_y			= 0x%08x\n",	smc.mmvrcnt.mmvro_y				));
	Ddim_Print(("mmvrcnt.mmvrk_x			= 0x%08x\n",	smc.mmvrcnt.mmvrk_x				));
	Ddim_Print(("mmvrcnt.mmvrk_y			= 0x%08x\n",	smc.mmvrcnt.mmvrk_y				));
	Ddim_Print(("mdbcnt.mdb_x				= 0x%08x\n",	smc.mdbcnt.mdb_x				));
	Ddim_Print(("mdbcnt.mdb_y				= 0x%08x\n",	smc.mdbcnt.mdb_y				));
	Ddim_Print(("mdbrocnt.mdbro_x			= 0x%08x\n",	smc.mdbrocnt.mdbro_x			));
	Ddim_Print(("mdbrocnt.mdbro_y			= 0x%08x\n",	smc.mdbrocnt.mdbro_y			));
	Ddim_Print(("mdbrocnt.mdbrk_x			= 0x%08x\n",	smc.mdbrocnt.mdbrk_x			));
	Ddim_Print(("mdbrocnt.mdbrk_y			= 0x%08x\n",	smc.mdbrocnt.mdbrk_y			));
	Ddim_Print(("molrcnt.molr_sm			= 0x%08x\n",	smc.molrcnt.molr_sm				));
	Ddim_Print(("molrcnt.molr_ss			= 0x%08x\n",	smc.molrcnt.molr_ss				));
	Ddim_Print(("molrcnt.molr_o				= 0x%08x\n",	smc.molrcnt.molr_o				));
	Ddim_Print(("molrcnt.molr_k				= 0x%08x\n",	smc.molrcnt.molr_k				));
	Ddim_Print(("mcpcnt.mcpx_s				= 0x%08x\n",	smc.mcpcnt.mcpx_s				));
	Ddim_Print(("mcpcnt.mcpx_o				= 0x%08x\n",	smc.mcpcnt.mcpx_o				));
	Ddim_Print(("mcpcnt.mcpx_k				= 0x%08x\n",	smc.mcpcnt.mcpx_k				));
	Ddim_Print(("mmcrcnt.mmcr_s				= 0x%08x\n",	smc.mmcrcnt.mmcr_s				));
	Ddim_Print(("mmcrcnt.mmcr_k				= 0x%08x\n",	smc.mmcrcnt.mmcr_k				));
	Ddim_Print(("molecnt.mole_s				= 0x%08x\n",	smc.molecnt.mole_s				));
	Ddim_Print(("molecnt.mole_o				= 0x%08x\n",	smc.molecnt.mole_o				));
	Ddim_Print(("molecnt.mole_k				= 0x%08x\n",	smc.molecnt.mole_k				));
	Ddim_Print(("mmvecnt.mmve_s				= 0x%08x\n",	smc.mmvecnt.mmve_s				));
	Ddim_Print(("mmvecnt.mmve_o				= 0x%08x\n",	smc.mmvecnt.mmve_o				));
	Ddim_Print(("mmvecnt.mmve_k				= 0x%08x\n",	smc.mmvecnt.mmve_k				));
	Ddim_Print(("mcpxecnt.mcpxe_s			= 0x%08x\n",	smc.mcpxecnt.mcpxe_s			));
	Ddim_Print(("mcpxecnt.mcpxe_o			= 0x%08x\n",	smc.mcpxecnt.mcpxe_o			));
	Ddim_Print(("mcpxecnt.mcpxe_k			= 0x%08x\n",	smc.mcpxecnt.mcpxe_k			));
	Ddim_Print(("mvlcnt.mvl_en				= 0x%08x\n",	smc.mvlcnt.mvl_en				));
	Ddim_Print(("mvlcnt.mvl_threshold		= 0x%08x\n",	smc.mvlcnt.mvl_threshold		));
	Ddim_Print(("mvlcnt.mvl_count_threshold	= 0x%08x\n",	smc.mvlcnt.mvl_count_threshold	));
}

static VOID ct_im_shdr_get_dgr( VOID )
{
	INT32			ret;
	INT32			i;
	USHORT			dst_tbl[D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE];

	ret = Im_SHDR_Get_Degamma_R( &dst_tbl[0], 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE, D_IM_SHDR_DGTRSEL_PREFL );

	Ddim_Print(("ct_im_shdr_get_dgr[ret=0x%08x]\n", ret));
	for ( i = 0; i < D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE; i++ ) {
		Ddim_Print(("dst_tbl[%d] = 0x%08x\n", i, dst_tbl[i]));
	}
}

static VOID ct_im_shdr_get_dgg( VOID )
{
	INT32			ret;
	INT32			i;
	USHORT			dst_tbl[D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE];

	ret = Im_SHDR_Get_Degamma_G( &dst_tbl[0], 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE, D_IM_SHDR_DGTRSEL_PREFL );

	Ddim_Print(("ct_im_shdr_get_dgg[ret=0x%08x]\n", ret));
	for ( i = 0; i < D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE; i++ ) {
		Ddim_Print(("dst_tbl[%d] = 0x%08x\n", i, dst_tbl[i]));
	}
}

static VOID ct_im_shdr_get_dgb( VOID )
{
	INT32			ret;
	INT32			i;
	USHORT			dst_tbl[D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE];

	ret = Im_SHDR_Get_Degamma_B( &dst_tbl[0], 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE, D_IM_SHDR_DGTRSEL_PREFL );

	Ddim_Print(("ct_im_shdr_get_dgb[ret=0x%08x]\n", ret));
	for ( i = 0; i < D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE; i++ ) {
		Ddim_Print(("dst_tbl[%d] = 0x%08x\n", i, dst_tbl[i]));
	}
}

static VOID ct_im_shdr_get_dg( VOID )
{
	INT32			ret;
	INT32			i, j;
	USHORT			dst_tbl[3][D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE];

	ret = Im_SHDR_Get_Degamma( &dst_tbl[0][0], &dst_tbl[1][0], &dst_tbl[2][0], 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE, D_IM_SHDR_DGTRSEL_PREFL );

	Ddim_Print(("ct_im_shdr_get_dg[ret=0x%08x]\n", ret));
	for ( i = 0; i < 3; i++ ) {
		for ( j = 0; j < D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE; j++ ) {
			Ddim_Print(("dst_tbl[%d][%d] = 0x%08x\n", i, j, dst_tbl[i][j]));
		}
	}
}

static VOID ct_im_shdr_get_axi( VOID )
{
	INT32			ret;
	T_IM_SHDR_AXI	axi;

	ret = Im_SHDR_Get_Axi( &axi );

	Ddim_Print(("ct_im_shdr_get_axi[ret=0x%08x]\n", ret));
	Ddim_Print(("ax_mode.r_burst	= 0x%08x\n",	axi.ax_mode.r_burst		));
	Ddim_Print(("ax_mode.w_burst	= 0x%08x\n",	axi.ax_mode.w_burst		));
	Ddim_Print(("ax_mode.w_w_mask	= 0x%08x\n",	axi.ax_mode.w_w_mask	));
	Ddim_Print(("r_ctl.a_cahche[0]	= 0x%08x\n",	axi.r_ctl.a_cahche[0]	));
	Ddim_Print(("r_ctl.a_cahche[1]	= 0x%08x\n",	axi.r_ctl.a_cahche[1]	));
	Ddim_Print(("r_ctl.a_protect[0]	= 0x%08x\n",	axi.r_ctl.a_protect[0]	));
	Ddim_Print(("r_ctl.a_protect[1]	= 0x%08x\n",	axi.r_ctl.a_protect[1]	));
	Ddim_Print(("r_ctl.b_cahche[0]	= 0x%08x\n",	axi.r_ctl.b_cahche[0]	));
	Ddim_Print(("r_ctl.b_cahche[1]	= 0x%08x\n",	axi.r_ctl.b_cahche[1]	));
	Ddim_Print(("r_ctl.b_protect[0]	= 0x%08x\n",	axi.r_ctl.b_protect[0]	));
	Ddim_Print(("r_ctl.b_protect[1]	= 0x%08x\n",	axi.r_ctl.b_protect[1]	));
	Ddim_Print(("r_ctl.c_cahche[0]	= 0x%08x\n",	axi.r_ctl.c_cahche[0]	));
	Ddim_Print(("r_ctl.c_cahche[1]	= 0x%08x\n",	axi.r_ctl.c_cahche[1]	));
	Ddim_Print(("r_ctl.c_cahche[2]	= 0x%08x\n",	axi.r_ctl.c_cahche[2]	));
	Ddim_Print(("r_ctl.c_cahche[3]	= 0x%08x\n",	axi.r_ctl.c_cahche[3]	));
	Ddim_Print(("r_ctl.c_protect[0]	= 0x%08x\n",	axi.r_ctl.c_protect[0]	));
	Ddim_Print(("r_ctl.c_protect[1]	= 0x%08x\n",	axi.r_ctl.c_protect[1]	));
	Ddim_Print(("r_ctl.c_protect[2]	= 0x%08x\n",	axi.r_ctl.c_protect[2]	));
	Ddim_Print(("r_ctl.c_protect[3]	= 0x%08x\n",	axi.r_ctl.c_protect[3]	));
	Ddim_Print(("r_ctl.d_cahche[0]	= 0x%08x\n",	axi.r_ctl.d_cahche[0]	));
	Ddim_Print(("r_ctl.d_cahche[1]	= 0x%08x\n",	axi.r_ctl.d_cahche[1]	));
	Ddim_Print(("r_ctl.d_protect[0]	= 0x%08x\n",	axi.r_ctl.d_protect[0]	));
	Ddim_Print(("r_ctl.d_protect[1]	= 0x%08x\n",	axi.r_ctl.d_protect[1]	));
	Ddim_Print(("w_ctl.a_cahche[0]	= 0x%08x\n",	axi.w_ctl.a_cahche[0]	));
	Ddim_Print(("w_ctl.a_cahche[1]	= 0x%08x\n",	axi.w_ctl.a_cahche[1]	));
	Ddim_Print(("w_ctl.a_cahche[2]	= 0x%08x\n",	axi.w_ctl.a_cahche[2]	));
	Ddim_Print(("w_ctl.a_cahche[3]	= 0x%08x\n",	axi.w_ctl.a_cahche[3]	));
	Ddim_Print(("w_ctl.a_protect[0]	= 0x%08x\n",	axi.w_ctl.a_protect[0]	));
	Ddim_Print(("w_ctl.a_protect[1]	= 0x%08x\n",	axi.w_ctl.a_protect[1]	));
	Ddim_Print(("w_ctl.a_protect[2]	= 0x%08x\n",	axi.w_ctl.a_protect[2]	));
	Ddim_Print(("w_ctl.a_protect[3]	= 0x%08x\n",	axi.w_ctl.a_protect[3]	));
	Ddim_Print(("w_ctl.b_cahche[0]	= 0x%08x\n",	axi.w_ctl.b_cahche[0]	));
	Ddim_Print(("w_ctl.b_cahche[1]	= 0x%08x\n",	axi.w_ctl.b_cahche[1]	));
	Ddim_Print(("w_ctl.b_cahche[2]	= 0x%08x\n",	axi.w_ctl.b_cahche[2]	));
	Ddim_Print(("w_ctl.b_cahche[3]	= 0x%08x\n",	axi.w_ctl.b_cahche[3]	));
	Ddim_Print(("w_ctl.b_cahche[4]	= 0x%08x\n",	axi.w_ctl.b_cahche[4]	));
	Ddim_Print(("w_ctl.b_cahche[5]	= 0x%08x\n",	axi.w_ctl.b_cahche[5]	));
	Ddim_Print(("w_ctl.b_protect[0]	= 0x%08x\n",	axi.w_ctl.b_protect[0]	));
	Ddim_Print(("w_ctl.b_protect[1]	= 0x%08x\n",	axi.w_ctl.b_protect[1]	));
	Ddim_Print(("w_ctl.b_protect[2]	= 0x%08x\n",	axi.w_ctl.b_protect[2]	));
	Ddim_Print(("w_ctl.b_protect[3]	= 0x%08x\n",	axi.w_ctl.b_protect[3]	));
	Ddim_Print(("w_ctl.b_protect[4]	= 0x%08x\n",	axi.w_ctl.b_protect[4]	));
	Ddim_Print(("w_ctl.b_protect[5]	= 0x%08x\n",	axi.w_ctl.b_protect[5]	));
	Ddim_Print(("w_ctl.c_cahche[0]	= 0x%08x\n",	axi.w_ctl.c_cahche[0]	));
	Ddim_Print(("w_ctl.c_cahche[1]	= 0x%08x\n",	axi.w_ctl.c_cahche[1]	));
	Ddim_Print(("w_ctl.c_cahche[2]	= 0x%08x\n",	axi.w_ctl.c_cahche[2]	));
	Ddim_Print(("w_ctl.c_cahche[3]	= 0x%08x\n",	axi.w_ctl.c_cahche[3]	));
	Ddim_Print(("w_ctl.c_cahche[4]	= 0x%08x\n",	axi.w_ctl.c_cahche[4]	));
	Ddim_Print(("w_ctl.c_cahche[5]	= 0x%08x\n",	axi.w_ctl.c_cahche[5]	));
	Ddim_Print(("w_ctl.c_cahche[6]	= 0x%08x\n",	axi.w_ctl.c_cahche[6]	));
	Ddim_Print(("w_ctl.c_protect[0]	= 0x%08x\n",	axi.w_ctl.c_protect[0]	));
	Ddim_Print(("w_ctl.c_protect[1]	= 0x%08x\n",	axi.w_ctl.c_protect[1]	));
	Ddim_Print(("w_ctl.c_protect[2]	= 0x%08x\n",	axi.w_ctl.c_protect[2]	));
	Ddim_Print(("w_ctl.c_protect[3]	= 0x%08x\n",	axi.w_ctl.c_protect[3]	));
	Ddim_Print(("w_ctl.c_protect[4]	= 0x%08x\n",	axi.w_ctl.c_protect[4]	));
	Ddim_Print(("w_ctl.c_protect[5]	= 0x%08x\n",	axi.w_ctl.c_protect[5]	));
	Ddim_Print(("w_ctl.c_protect[6]	= 0x%08x\n",	axi.w_ctl.c_protect[6]	));
}

static VOID ct_im_shdr_get_axires( VOID )
{
	INT32				ret;
	T_IM_SHDR_AXI_RESP	axires;

	ret = Im_SHDR_Get_Axi_Response( &axires );

	Ddim_Print(("ct_im_shdr_get_axires[ret=0x%08x]\n", ret));
	Ddim_Print(("r_resp_a[0]= 0x%08x\n", axires.r_resp_a[0]));
	Ddim_Print(("r_resp_a[1]= 0x%08x\n", axires.r_resp_a[1]));
	Ddim_Print(("r_resp_a[2]= 0x%08x\n", axires.r_resp_a[2]));
	Ddim_Print(("r_resp_a[3]= 0x%08x\n", axires.r_resp_a[3]));
	Ddim_Print(("r_resp_b[0]= 0x%08x\n", axires.r_resp_b[0]));
	Ddim_Print(("r_resp_b[1]= 0x%08x\n", axires.r_resp_b[1]));
	Ddim_Print(("r_resp_b[2]= 0x%08x\n", axires.r_resp_b[2]));
	Ddim_Print(("r_resp_b[3]= 0x%08x\n", axires.r_resp_b[3]));
	Ddim_Print(("r_resp_c[0]= 0x%08x\n", axires.r_resp_c[0]));
	Ddim_Print(("r_resp_c[1]= 0x%08x\n", axires.r_resp_c[1]));
	Ddim_Print(("r_resp_c[2]= 0x%08x\n", axires.r_resp_c[2]));
	Ddim_Print(("r_resp_c[3]= 0x%08x\n", axires.r_resp_c[3]));
	Ddim_Print(("r_resp_d[0]= 0x%08x\n", axires.r_resp_d[0]));
	Ddim_Print(("r_resp_d[1]= 0x%08x\n", axires.r_resp_d[1]));
	Ddim_Print(("r_resp_d[2]= 0x%08x\n", axires.r_resp_d[2]));
	Ddim_Print(("r_resp_d[3]= 0x%08x\n", axires.r_resp_d[3]));
	Ddim_Print(("w_resp_a[0]= 0x%08x\n", axires.w_resp_a[0]));
	Ddim_Print(("w_resp_a[1]= 0x%08x\n", axires.w_resp_a[1]));
	Ddim_Print(("w_resp_a[2]= 0x%08x\n", axires.w_resp_a[2]));
	Ddim_Print(("w_resp_a[3]= 0x%08x\n", axires.w_resp_a[3]));
	Ddim_Print(("w_resp_a[4]= 0x%08x\n", axires.w_resp_a[4]));
	Ddim_Print(("w_resp_a[5]= 0x%08x\n", axires.w_resp_a[5]));
	Ddim_Print(("w_resp_a[6]= 0x%08x\n", axires.w_resp_a[6]));
	Ddim_Print(("w_resp_b[0]= 0x%08x\n", axires.w_resp_b[0]));
	Ddim_Print(("w_resp_b[1]= 0x%08x\n", axires.w_resp_b[1]));
	Ddim_Print(("w_resp_b[2]= 0x%08x\n", axires.w_resp_b[2]));
	Ddim_Print(("w_resp_b[3]= 0x%08x\n", axires.w_resp_b[3]));
	Ddim_Print(("w_resp_b[4]= 0x%08x\n", axires.w_resp_b[4]));
	Ddim_Print(("w_resp_b[5]= 0x%08x\n", axires.w_resp_b[5]));
	Ddim_Print(("w_resp_b[6]= 0x%08x\n", axires.w_resp_b[6]));
	Ddim_Print(("w_resp_c[0]= 0x%08x\n", axires.w_resp_c[0]));
	Ddim_Print(("w_resp_c[1]= 0x%08x\n", axires.w_resp_c[1]));
	Ddim_Print(("w_resp_c[2]= 0x%08x\n", axires.w_resp_c[2]));
	Ddim_Print(("w_resp_c[3]= 0x%08x\n", axires.w_resp_c[3]));
	Ddim_Print(("w_resp_c[4]= 0x%08x\n", axires.w_resp_c[4]));
	Ddim_Print(("w_resp_c[5]= 0x%08x\n", axires.w_resp_c[5]));
	Ddim_Print(("w_resp_c[6]= 0x%08x\n", axires.w_resp_c[6]));
}

static VOID ct_im_shdr_err_ctrl( VOID )
{
	INT32			ret;
	T_IM_SHDR_CTRL	ctrl;

	ret = Im_SHDR_Ctrl_Common( NULL );
	Ddim_Print(("Im_SHDR_Ctrl_Common[ret=0x%08x]\n", ret));

	ctrl.image_set.in_bit_depth = D_IM_SHDR_INBITDEPTH_16 + 1;

	ret = Im_SHDR_Ctrl_Common( &ctrl );
	Ddim_Print(("Im_SHDR_Ctrl_Common[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Ctrl_Common( NULL );
	Ddim_Print(("Im_SHDR_Get_Ctrl_Common[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_err_pre( VOID )
{
	INT32			ret;
	T_IM_SHDR_PRE	pre;

	ret = Im_SHDR_Set_Pre( NULL );
	Ddim_Print(("Im_SHDR_Set_Pre[ret=0x%08x]\n", ret));

	pre.pre_mode.cus_mode = D_IM_SHDR_CUS_YCC420_PADDING + 1;
	ret = Im_SHDR_Set_Pre( &pre );
	Ddim_Print(("Im_SHDR_Set_Pre[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Pre( NULL );
	Ddim_Print(("Im_SHDR_Get_Pre[ret=0x%08x]\n", ret));
}


static VOID ct_im_shdr_err_fmd( VOID )
{
	INT32			ret;
	T_IM_SHDR_FMD	fmd;

	ret = Im_SHDR_Set_Fmd( NULL );
	Ddim_Print(("Im_SHDR_Set_Fmd[ret=0x%08x]\n", ret));

	fmd.weight_base = D_IM_SHDR_WGHTBASE_RGB + 1;
	ret = Im_SHDR_Set_Fmd( &fmd );
	Ddim_Print(("Im_SHDR_Set_Fmd[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Fmd( NULL );
	Ddim_Print(("Im_SHDR_Get_Fmd[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_err_pmsk( VOID )
{
	INT32			ret;
	T_IM_SHDR_PMSK	pmsk;

	ret = Im_SHDR_Set_PreMask( NULL );
	Ddim_Print(("Im_SHDR_Set_PreMask[ret=0x%08x]\n", ret));

	pmsk.pmsk_mode = D_IM_SHDR_PMSKMD_AVE + 1;
	ret = Im_SHDR_Set_PreMask( &pmsk );
	Ddim_Print(("Im_SHDR_Set_PreMask[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_PreMask( NULL );
	Ddim_Print(("Im_SHDR_Get_PreMask[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_err_spnr( VOID )
{
	INT32			ret;
	T_IM_SHDR_SPNR	spnr;

	ret = Im_SHDR_Set_Spnr( NULL );
	Ddim_Print(("Im_SHDR_Set_Spnr[ret=0x%08x]\n", ret));

	spnr.spnr_en = D_IM_SHDR_ENABLE + 1;
	ret = Im_SHDR_Set_Spnr( &spnr );
	Ddim_Print(("Im_SHDR_Set_Spnr[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Spnr( NULL );
	Ddim_Print(("Im_SHDR_Get_Spnr[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_err_mask( VOID )
{
	INT32			ret;

	ret = Im_SHDR_Set_Mask( NULL );
	Ddim_Print(("Im_SHDR_Set_Mask[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Mask( NULL );
	Ddim_Print(("Im_SHDR_Get_Mask[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_err_bld( VOID )
{
	INT32			ret;
	T_IM_SHDR_BLD	bld;

	ret = Im_SHDR_Set_Blend( NULL );
	Ddim_Print(("Im_SHDR_Set_Blend[ret=0x%08x]\n", ret));

	bld.blend_sel = D_IM_SHDR_BLDSEL_LONG_SHORT + 1;
	ret = Im_SHDR_Set_Blend( &bld );
	Ddim_Print(("Im_SHDR_Set_Blend[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Blend( NULL );
	Ddim_Print(("Im_SHDR_Get_Blend[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_err_post( VOID )
{
	INT32			ret;
	T_IM_SHDR_POST	post;

	ret = Im_SHDR_Set_Post( NULL );
	Ddim_Print(("Im_SHDR_Set_Post[ret=0x%08x]\n", ret));

	post.ev_sel = D_IM_SHDR_POSTSEL1_RGB + 1;
	ret = Im_SHDR_Set_Post( &post );
	Ddim_Print(("Im_SHDR_Set_Post[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Post( NULL );
	Ddim_Print(("Im_SHDR_Get_Post[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_err_resize( VOID )
{
	INT32				ret;
	T_IM_SHDR_RESIZE	resize;

	ret = Im_SHDR_Set_Resize( NULL );
	Ddim_Print(("Im_SHDR_Set_Resize[ret=0x%08x]\n", ret));

	resize.chmode.a_w_en = D_IM_SHDR_ENABLE + 1;
	ret = Im_SHDR_Set_Resize( &resize );
	Ddim_Print(("Im_SHDR_Set_Resize[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Resize( NULL );
	Ddim_Print(("Im_SHDR_Get_Resize[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_err_smc( VOID )
{
	INT32			ret;
	T_IM_SHDR_SMC	smc;

	ret = Im_SHDR_Set_Smc( NULL );
	Ddim_Print(("Im_SHDR_Set_Smc[ret=0x%08x]\n", ret));

	smc.mcarmd.mca_mode = D_IM_MCAMD_MAX_MIN_VECTOR + 1;
	ret = Im_SHDR_Set_Smc( &smc );
	Ddim_Print(("Im_SHDR_Set_Smc[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Smc( NULL );
	Ddim_Print(("Im_SHDR_Get_Smc[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_err_dgr( VOID )
{
	INT32	ret;
	USHORT	src_tbl[ D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ];

	ret = Im_SHDR_Set_Degamma_R( NULL, 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
	Ddim_Print(("Im_SHDR_Set_Degamma_R[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Set_Degamma_R( &src_tbl[0], 1, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
	Ddim_Print(("Im_SHDR_Set_Degamma_R[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Degamma_R( NULL, 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE, D_IM_SHDR_DGTRSEL_PREFL );
	Ddim_Print(("Im_SHDR_Get_Degamma_R[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Degamma_R( &src_tbl[0], 1, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE, D_IM_SHDR_DGTRSEL_PREFL );
	Ddim_Print(("Im_SHDR_Get_Degamma_R[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_err_dgg( VOID )
{
	INT32	ret;
	USHORT	src_tbl[ D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ];

	ret = Im_SHDR_Set_Degamma_G( NULL, 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
	Ddim_Print(("Im_SHDR_Set_Degamma_G[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Set_Degamma_G( &src_tbl[0], 1, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
	Ddim_Print(("Im_SHDR_Set_Degamma_G[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Degamma_G( NULL, 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE, D_IM_SHDR_DGTRSEL_PREFL );
	Ddim_Print(("Im_SHDR_Get_Degamma_G[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Degamma_G( &src_tbl[0], 1, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE, D_IM_SHDR_DGTRSEL_PREFL );
	Ddim_Print(("Im_SHDR_Get_Degamma_G[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_err_dgb( VOID )
{
	INT32	ret;
	USHORT	src_tbl[ D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ];

	ret = Im_SHDR_Set_Degamma_B( NULL, 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
	Ddim_Print(("Im_SHDR_Set_Degamma_B[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Set_Degamma_B( &src_tbl[0], 1, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
	Ddim_Print(("Im_SHDR_Set_Degamma_B[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Degamma_B( NULL, 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE, D_IM_SHDR_DGTRSEL_PREFL );
	Ddim_Print(("Im_SHDR_Get_Degamma_B[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Degamma_B( &src_tbl[0], 1, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE, D_IM_SHDR_DGTRSEL_PREFL );
	Ddim_Print(("Im_SHDR_Get_Degamma_B[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_err_dg( VOID )
{
	INT32	ret;
	USHORT	src_tbl[ D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ];

	ret = Im_SHDR_Set_Degamma( NULL, &src_tbl[0], &src_tbl[0], 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
	Ddim_Print(("Im_SHDR_Set_Degamma[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Set_Degamma( &src_tbl[0], &src_tbl[0], &src_tbl[0], 1, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
	Ddim_Print(("Im_SHDR_Set_Degamma[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Degamma( NULL, &src_tbl[0], &src_tbl[0], 0, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE, D_IM_SHDR_DGTRSEL_PREFL );
	Ddim_Print(("Im_SHDR_Get_Degamma[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Degamma( &src_tbl[0], &src_tbl[0],&src_tbl[0], 1, D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE, D_IM_SHDR_DGTRSEL_PREFL );
	Ddim_Print(("Im_SHDR_Get_Degamma[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_err_axi( VOID )
{
	INT32			ret;
	T_IM_SHDR_AXI	axi;

	ret = Im_SHDR_Set_Axi( NULL );
	Ddim_Print(("Im_SHDR_Set_Axi[ret=0x%08x]\n", ret));

	axi.ax_mode.r_burst = D_IM_SHDR_BURSTLEN_256 + 1;
	ret = Im_SHDR_Set_Axi( &axi );
	Ddim_Print(("Im_SHDR_Set_Axi[ret=0x%08x]\n", ret));

	ret = Im_SHDR_Get_Axi( NULL );
	Ddim_Print(("Im_SHDR_Get_Axi[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_err_axires( VOID )
{
	INT32			ret;

	ret = Im_SHDR_Get_Axi_Response( NULL );
	Ddim_Print(("Im_SHDR_Get_Axi_Response[ret=0x%08x]\n", ret));
}

static VOID ct_im_shdr_test( T_IM_SHDR_PARAM* shdr_param )
{
	INT32	ret;
	UINT32	wait_factor;

	Ddim_Print(("porocess 1\n"));
	ret = Im_SHDR_Open( 0 );
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Open[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 2\n"));
	ret = Im_SHDR_Init( FALSE );
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Init[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 3\n"));
	ret = Im_SHDR_Ctrl_Common( &shdr_param->ctrl );
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Ctrl_Common[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 4\n"));
	ret = Im_SHDR_Set_Pre( &shdr_param->pre );
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Set_Pre[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 5\n"));
	ret = Im_SHDR_Set_Fmd( &shdr_param->fmd );
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Set_Fmd[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 6\n"));
	ret = Im_SHDR_Set_PreMask( &shdr_param->pmsk );
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Set_PreMask[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 7\n"));
	ret = Im_SHDR_Set_Spnr( &shdr_param->spnr );
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Set_Spnr[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 8\n"));
	ret = Im_SHDR_Set_Mask( &shdr_param->msk );
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Set_Mask[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 9\n"));
	ret = Im_SHDR_Set_Blend( &shdr_param->bld );
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Set_Blend[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 10\n"));
	ret = Im_SHDR_Set_Post( &shdr_param->post );
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Set_Post[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 11\n"));
	ret = Im_SHDR_Set_Resize( &shdr_param->resize );
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Set_Resize[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 12\n"));
	ret = Im_SHDR_Set_Smc( &shdr_param->smc );
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Set_Smc[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 13\n"));
	ret = Im_SHDR_Set_Axi( &shdr_param->axi );
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Set_Axi[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 14\n"));
	ret = Im_SHDR_Start();
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Start[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 15\n"));
	ret = Im_SHDR_Wait_End( &wait_factor, 1000 );
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Wait_End[ret=0x%08x]\n", ret));
	}

	Ddim_Print(("porocess 16\n"));
	ret = Im_SHDR_Close();
	if ( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_SHDR_Close[ret=0x%08x]\n", ret));
	}
}

static VOID ct_im_shdr_test2_1( VOID )
{
	T_IM_SHDR_PARAM	shdr_param = {
		.ctrl = {
			.image_set.in_bit_depth		= D_IM_SHDR_INBITDEPTH_8,
			.image_set.in_c_fmt			= D_IM_SHDR_INCFMT_420SP,		// 420sp in.
			.image_set.in_edge_cpy		= D_IM_SHDR_ON,
			.image_set.in_edge_cut		= D_IM_SHDR_ON,
			.image_set.out_ctrl			= D_IM_SHDR_OUTCTRL_YCC,
			.image_set.out_sel			= D_IM_SHDR_OUTSEL_YCC_16,
			.image_set.out_sel_en		= D_IM_SHDR_DISABLE,
			.image_mode.exp_mag			= 0x0100,
			.image_mode.fil_margin		= D_IM_SHDR_FLITER_MARGIN_32,	// 32pixel.
			.image_mode.rexp_mag_shift	= 0x0,
			.image_mode.rexp_mag		= 0x10000,
			.image_mode.shift_x			= 0x0000,
			.image_mode.shift_y			= 0x0000,
			.image_mode.fmd_en			= D_IM_SHDR_DISABLE,
			.image_mode.bypass_en		= D_IM_SHDR_DISABLE,
			.image_mode.smc_in			= D_IM_SHDR_SMCIN_STN_REF,
			.image_mode.shdr_in			= D_IM_SHDR_SHDRIN_STN_REF,
			.image_mode.tile_size		= D_IM_SHDR_TILESIZE_1024x1024,
			.sdram.r_addr.a_y			= D_IM_SHDR_IMG_MEM_IN_YCC_REF_ADDR_Y,
			.sdram.r_addr.a_c			= D_IM_SHDR_IMG_MEM_IN_YCC_REF_ADDR_C,
			.sdram.r_addr.b_y			= D_IM_SHDR_IMG_MEM_IN_YCC_STD_ADDR_Y,
			.sdram.r_addr.b_c			= D_IM_SHDR_IMG_MEM_IN_YCC_STD_ADDR_C,
			.sdram.r_addr.c_y			= 0x00000000,
			.sdram.r_addr.c_c			= 0x00000000,
			.sdram.r_addr.d_y			= 0x00000000,
			.sdram.r_addr.d_c			= 0x00000000,
			.sdram.r_addr.e_y			= 0x00000000,
			.sdram.r_addr.e_c			= 0x00000000,
			.sdram.r_addr.f				= 0x00000000,
			.sdram.r_ghsize.a_y			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_REF,
			.sdram.r_ghsize.a_c			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_REF,
			.sdram.r_ghsize.b_y			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_STD,
			.sdram.r_ghsize.b_c			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_STD,
			.sdram.r_ghsize.c_y			= 0x0000,
			.sdram.r_ghsize.c_c			= 0x0000,
			.sdram.r_ghsize.d_y			= 0x0000,
			.sdram.r_ghsize.d_c			= 0x0000,
			.sdram.r_ghsize.e_y			= 0x0000,
			.sdram.r_ghsize.e_c			= 0x0000,
			.sdram.r_ghsize.f			= 0x0000,
			.sdram.r_hsize.a			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_REF,
			.sdram.r_hsize.b			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_STD,
			.sdram.r_hsize.c			= 0x0000,
			.sdram.r_hsize.d			= 0x0000,
			.sdram.r_hsize.e			= 0x0000,
			.sdram.r_vsize.a			= D_IM_SHDR_SRC_IMG_PIXS_LINES_YCC_4K2K_REF,
			.sdram.r_vsize.b			= D_IM_SHDR_SRC_IMG_PIXS_LINES_YCC_4K2K_STD,
			.sdram.r_vsize.c			= 0x0000,
			.sdram.r_vsize.d			= 0x0000,
			.sdram.r_vsize.e			= 0x0000,
			.sdram.r_vfm.a				= D_IM_SHDR_VFMT_RASTER,
			.sdram.w_addr.b_y			= D_IM_SHDR_IMG_MEM_OUT_MEIN_ADDR_Y,
			.sdram.w_addr.b_c			= D_IM_SHDR_IMG_MEM_OUT_MEIN_ADDR_C,
			.sdram.w_addr.e_y			= D_IM_SHDR_IMG_MEM_OUT_DISP_HDMI_ADDR_Y,
			.sdram.w_addr.e_c			= D_IM_SHDR_IMG_MEM_OUT_DISP_HDMI_ADDR_C,
			.sdram.w_addr.f_y			= D_IM_SHDR_IMG_MEM_OUT_DISP_LCD_ADDR_Y,
			.sdram.w_addr.f_c			= D_IM_SHDR_IMG_MEM_OUT_DISP_LCD_ADDR_C,
			.sdram.w_addr.g_r_y			= 0x00000000,
			.sdram.w_addr.g_g_c			= 0x00000000,
			.sdram.w_addr.g_b			= 0x00000000,
			.sdram.w_addr.h_x			= 0x00000000,
			.sdram.w_addr.h_y			= 0x00000000,
			.sdram.w_addr.i				= 0x00000000,
			.sdram.w_ghsize.b_y			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_MEIN,
			.sdram.w_ghsize.b_c			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_MEIN,
			.sdram.w_ghsize.e_y			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_HDMI,
			.sdram.w_ghsize.e_c			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_HDMI,
			.sdram.w_ghsize.f_y			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_LCD,
			.sdram.w_ghsize.f_c			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_LCD,
			.sdram.w_ghsize.g_r_y		= 0x0000,
			.sdram.w_ghsize.g_g_c		= 0x0000,
			.sdram.w_ghsize.g_b			= 0x0000,
			.sdram.w_ghsize.h_x			= 0x0000,
			.sdram.w_ghsize.h_y			= 0x0000,
			.sdram.w_ghsize.i			= 0x0000,
			.sdram.w_vfm.a				= D_IM_SHDR_VFMT_COMPIP_VIDEO,
			.sdram.w_vfm.b				= D_IM_SHDR_VFMT_RASTER,
			.sdram.w_baddr.a_y			= D_IM_SHDR_IMG_MEM_OUT_HEVC_ADDR_Y,
			.sdram.w_baddr.a_c			= D_IM_SHDR_IMG_MEM_OUT_HEVC_ADDR_C,
			.sdram.w_baddr.c_y			= D_IM_SHDR_IMG_MEM_OUT_RIB_FHD_ADDR_Y,
			.sdram.w_baddr.c_c			= D_IM_SHDR_IMG_MEM_OUT_RIB_FHD_ADDR_C,
			.sdram.w_baddr.d_y			= D_IM_SHDR_IMG_MEM_OUT_RIB_HD_ADDR_Y,
			.sdram.w_baddr.d_c			= D_IM_SHDR_IMG_MEM_OUT_RIB_HD_ADDR_C,
			.sdram.w_hsize.a			= D_IM_SHDR_IMG_MEM_HEVC_U8_Y_GLOBAL_WIDTH,
			.sdram.w_hsize.c			= D_IM_SHDR_IMG_MEM_RIB_FHD_U8_Y_GLOBAL_WIDTH,
			.sdram.w_hsize.d			= D_IM_SHDR_IMG_MEM_RIB_HD_U8_Y_GLOBAL_WIDTH,
			.compip.mode				= D_IM_SHDR_COMP_MODE_LOSSY,
			.compip.mc_mode				= D_IM_SHDR_COMP_MC_UNUSED,
			.compip.comp_incfmt			= D_IM_SHDR_COMP_INCFMT_YUV420,
			.compip.r_bypass_mode		= D_IM_SHDR_ENABLE,
			.compip.w_bypass_mode		= D_IM_SHDR_ENABLE,
			.compip.r_img_axi_size		= D_IM_SHDR_COMP_AXI_512,
			.compip.r_comp_axi_size		= D_IM_SHDR_COMP_AXI_512,
			.compip.w_img_axi_size		= D_IM_SHDR_COMP_AXI_512,
			.compip.lint_no				= 0,
			.compip.lint_mask			= D_IM_SHDR_ENABLE,
			.compip.lint_sts			= D_IM_SHDR_DISABLE,
			.compip.w_addr_y			= 0,
			.compip.w_addr_c			= 0,
			.compip.r_addr_y			= 0,
			.compip.r_addr_c			= 0,
			.pCallBack					= (T_IM_SHDR_CALLBACK)ct_im_shdr_callback,
			.pLineInt_CallBack			= (T_IM_SHDR_LINEINT_CALLBACK)ct_im_shdr_lineint_callback,
		},
		.pre = {
			.pre_mode.cus_mode			= D_IM_SHDR_CUS_YCC420_PADDING,		// 420 padding.
			.pre_mode.degamma_mode		= D_IM_SHDR_DEGAMMA_BITEXP,
			.y2r_coef[ 0 ] 				= 0x0100,
			.y2r_coef[ 1 ] 				= 0x0001,
			.y2r_coef[ 2 ] 				= 0x0166,
			.y2r_coef[ 3 ] 				= 0x0100,
			.y2r_coef[ 4 ] 				= 0x7FA8,
			.y2r_coef[ 5 ] 				= 0x7F48,
			.y2r_coef[ 6 ] 				= 0x0100,
			.y2r_coef[ 7 ] 				= 0x01C6,
			.y2r_coef[ 8 ] 				= 0x7FFE,
			.y2r_clp.upper_long0[ 0 ]	= 0x00FF,
			.y2r_clp.upper_long0[ 1 ]	= 0x00FF,
			.y2r_clp.upper_long0[ 2 ]	= 0x00FF,
			.y2r_clp.lower_long0[ 0 ]	= 0x0000,
			.y2r_clp.lower_long0[ 1 ]	= 0x0000,
			.y2r_clp.lower_long0[ 2 ]	= 0x0000,
			.y2r_clp.upper_short[ 0 ]	= 0xFF,
			.y2r_clp.upper_short[ 1 ]	= 0xFF,
			.y2r_clp.upper_short[ 2 ]	= 0xFF,
			.y2r_clp.lower_short[ 0 ]	= 0x00,
			.y2r_clp.lower_short[ 1 ]	= 0x00,
			.y2r_clp.lower_short[ 2 ]	= 0x00,
			.r2y_coef[ 0 ] 				= 0x004D,
			.r2y_coef[ 1 ] 				= 0x0096,
			.r2y_coef[ 2 ] 				= 0x001D,
			.r2y_coef[ 3 ] 				= 0x7FD5,
			.r2y_coef[ 4 ] 				= 0x7FAB,
			.r2y_coef[ 5 ] 				= 0x0080,
			.r2y_coef[ 6 ] 				= 0x0080,
			.r2y_coef[ 7 ] 				= 0x7F95,
			.r2y_coef[ 8 ] 				= 0x7FEB,
			.r2y_clp.upper_long_y		= 0x0FFF,
			.r2y_clp.upper_long_c[ 0 ]	= 0x07FF,
			.r2y_clp.upper_long_c[ 1 ]	= 0x07FF,
			.r2y_clp.lower_long_y		= 0x0000,
			.r2y_clp.lower_long_c[ 0 ]	= 0xF800,
			.r2y_clp.lower_long_c[ 1 ]	= 0xF800,
			.r2y_clp.upper_short_y		= 0xFFF,
			.r2y_clp.upper_short_c[ 0 ]	= 0x7FF,
			.r2y_clp.upper_short_c[ 1 ]	= 0x7FF,
			.r2y_clp.lower_short_y		= 0x000,
			.r2y_clp.lower_short_c[ 0 ]	= 0x800,
			.r2y_clp.lower_short_c[ 1 ]	= 0x800,
			.short_knee.offset[ 0 ]		= 0x00,
			.short_knee.offset[ 1 ]		= 0x00,
			.short_knee.offset[ 2 ]		= 0x00,
			.short_knee.slope[ 0 ]		= 0x00,
			.short_knee.slope[ 1 ]		= 0x00,
			.short_knee.slope[ 2 ]		= 0x20,
			.short_knee.boundary[ 0 ]	= 0x000,
			.short_knee.boundary[ 0 ]	= 0x000,
			.long_knee.offset[ 0 ]		= 0x00,
			.long_knee.offset[ 1 ]		= 0x00,
			.long_knee.offset[ 2 ]		= 0x00,
			.long_knee.slope[ 0 ]		= 0x00,
			.long_knee.slope[ 1 ]		= 0x00,
			.long_knee.slope[ 2 ]		= 0x20,
			.long_knee.boundary[ 0 ]	= 0x000,
			.long_knee.boundary[ 0 ]	= 0x000,
		},
		.fmd = {
			.ep_boundary[ 0 ]			= 0x00,		// D.C.
			.ep_boundary[ 1 ]			= 0x40,
			.ep_boundary[ 2 ]			= 0x80,
			.ep_boundary[ 3 ]			= 0xC0,
			.ep_offset[ 0 ]				= 0x0A,
			.ep_offset[ 1 ]				= 0x0A,
			.ep_offset[ 2 ]				= 0x0A,
			.ep_offset[ 3 ]				= 0x0A,
			.ep_blend					= 0x0,
			.fd_weight					= 0x10,
			.mvcnt.bounds				= 0x20,
			.mvcnt.threshold			= 0x80,
			.mvcnt.gr					= 0x1F6E,
			.bldr_offset[ 0 ]			= 0x00,
			.bldr_offset[ 1 ]			= 0x00,
			.bldr_offset[ 2 ]			= 0x00,
			.bldr_offset[ 3 ]			= 0x00,
			.bldr_offset[ 4 ]			= 0xFF,
			.bldr_slope[ 0 ]			= 0x000,
			.bldr_slope[ 1 ]			= 0x000,
			.bldr_slope[ 2 ]			= 0x000,
			.bldr_slope[ 3 ]			= 0x000,
			.bldr_slope[ 4 ]			= 0x000,
			.bldr_boundary[ 0 ]			= 0x00,		// D.C.
			.bldr_boundary[ 1 ]			= 0x00,
			.bldr_boundary[ 2 ]			= 0x00,
			.bldr_boundary[ 3 ]			= 0x00,
			.bldr_boundary[ 4 ]			= 0x06,
			.weight_r2y_coef[ 0 ]		= 0x004D,
			.weight_r2y_coef[ 1 ]		= 0x0096,
			.weight_r2y_coef[ 2 ]		= 0x001D,
			.weight_r2y_clp.upper		= 0xFF,
			.weight_r2y_clp.lower		= 0x00,
			.weight_base				= 0x0,
			.weight_offset[ 0 ][ 0 ]	= 0x60E,
			.weight_offset[ 0 ][ 1 ]	= 0x708,
			.weight_offset[ 0 ][ 2 ]	= 0x898,
			.weight_offset[ 0 ][ 3 ]	= 0xD48,
			.weight_offset[ 0 ][ 4 ]	= 0xFFF,
			.weight_offset[ 1 ][ 0 ]	= 0x60E,
			.weight_offset[ 1 ][ 1 ]	= 0x708,
			.weight_offset[ 1 ][ 2 ]	= 0x898,
			.weight_offset[ 1 ][ 3 ]	= 0xD48,
			.weight_offset[ 1 ][ 4 ]	= 0xFFF,
			.weight_offset[ 2 ][ 0 ]	= 0x60E,
			.weight_offset[ 2 ][ 1 ]	= 0x708,
			.weight_offset[ 2 ][ 2 ]	= 0x898,
			.weight_offset[ 2 ][ 3 ]	= 0xD48,
			.weight_offset[ 2 ][ 4 ]	= 0xFFF,
			.weight_slope[ 0 ][ 0 ]		= 0x274,
			.weight_slope[ 0 ][ 1 ]		= 0x3EC,
			.weight_slope[ 0 ][ 2 ]		= 0xBC4,
			.weight_slope[ 0 ][ 3 ]		= 0x6D1,
			.weight_slope[ 0 ][ 4 ]		= 0x000,
			.weight_slope[ 1 ][ 0 ]		= 0x274,
			.weight_slope[ 1 ][ 1 ]		= 0x3EC,
			.weight_slope[ 1 ][ 2 ]		= 0xBC4,
			.weight_slope[ 1 ][ 3 ]		= 0x6D1,
			.weight_slope[ 1 ][ 4 ]		= 0x000,
			.weight_slope[ 2 ][ 0 ]		= 0x274,
			.weight_slope[ 2 ][ 1 ]		= 0x3EC,
			.weight_slope[ 2 ][ 2 ]		= 0xBC4,
			.weight_slope[ 2 ][ 3 ]		= 0x6D1,
			.weight_slope[ 2 ][ 4 ]		= 0x000,
			.weight_boundary[ 0 ][ 0 ]	= 0x00,		// D.C.
			.weight_boundary[ 0 ][ 1 ]	= 0x33,
			.weight_boundary[ 0 ][ 2 ]	= 0x66,
			.weight_boundary[ 0 ][ 3 ]	= 0x99,
			.weight_boundary[ 0 ][ 4 ]	= 0xCC,
			.weight_boundary[ 1 ][ 0 ]	= 0x00,		// D.C.
			.weight_boundary[ 1 ][ 1 ]	= 0x33,
			.weight_boundary[ 1 ][ 2 ]	= 0x66,
			.weight_boundary[ 1 ][ 3 ]	= 0x99,
			.weight_boundary[ 1 ][ 4 ]	= 0xCC,
			.weight_boundary[ 2 ][ 0 ]	= 0x00,		// D.C.
			.weight_boundary[ 2 ][ 1 ]	= 0x33,
			.weight_boundary[ 2 ][ 2 ]	= 0x66,
			.weight_boundary[ 2 ][ 3 ]	= 0x99,
			.weight_boundary[ 2 ][ 4 ]	= 0xCC,
			.iso_threshold				= 0x010,
			.iso_coef[ 0 ]				= 0x1,
			.iso_coef[ 1 ]				= 0x1,
			.iso_coef[ 2 ]				= 0x1,
			.iso_coef[ 3 ]				= 0x01,
			.iso_coef[ 4 ]				= 0x01,
			.iso_coef[ 5 ]				= 0x01,
			.sat_threshold				= 0xFF,
			.exp77_mode					= D_IM_SHDR_EXP77MD_COEFSUMDIV,
			.fmd_sel					= D_IM_SHDR_FMDSEL_REDUCTION,
		},
		.pmsk = {
			.tlut[ 0 ]					= 0xFF,
			.tlut[ 1 ]					= 0xEF,
			.tlut[ 2 ]					= 0xDF,
			.tlut[ 3 ]					= 0xCF,
			.tlut[ 4 ]					= 0xBF,
			.tlut[ 5 ]					= 0xAF,
			.tlut[ 6 ]					= 0x9F,
			.tlut[ 7 ]					= 0x8F,
			.tlut[ 8 ]					= 0x7F,
			.tlut[ 9 ]					= 0x6F,
			.tlut[ 10 ]					= 0x5F,
			.tlut[ 11 ]					= 0x4F,
			.tlut[ 12 ]					= 0x3F,
			.tlut[ 13 ]					= 0x2F,
			.tlut[ 14 ]					= 0x1F,
			.tlut[ 15 ]					= 0x0F,
			.tlut[ 16 ]					= 0x00,
			.pmsk_mode					= D_IM_SHDR_PMSKMD_MAX,
		},
		.spnr = {
			.spnr_en					= D_IM_SHDR_ENABLE,
			.spe_blend					= 0x7,
			.spe_boundary[ 0 ]			= 0x000,		// D.C.
			.spe_boundary[ 1 ]			= 0x400,
			.spe_boundary[ 2 ]			= 0x800,
			.spe_boundary[ 3 ]			= 0xC00,
			.spe_offset[ 0 ]			= 0x0A0,
			.spe_offset[ 1 ]			= 0x0A0,
			.spe_offset[ 2 ]			= 0x0A0,
			.spe_offset[ 3 ]			= 0x0A0,
		},
		.msk = {
			.y2r_clp_upper_long[ 0 ]	= 0xFFF,
			.y2r_clp_upper_long[ 1 ]	= 0xFFF,
			.y2r_clp_upper_long[ 2 ]	= 0xFFF,
			.y2r_clp_lower_long[ 0 ]	= 0x000,
			.y2r_clp_lower_long[ 1 ]	= 0x000,
			.y2r_clp_lower_long[ 2 ]	= 0x000,
			.y2r_clp_upper_short[ 0 ]	= 0xFFF,
			.y2r_clp_upper_short[ 1 ]	= 0xFFF,
			.y2r_clp_upper_short[ 2 ]	= 0xFFF,
			.y2r_clp_lower_short[ 0 ]	= 0x000,
			.y2r_clp_lower_short[ 1 ]	= 0x000,
			.y2r_clp_lower_short[ 2 ]	= 0x000,
			.alpha						= 0x20,
			.lpfc[ 0 ][ 0 ]				= 0x00,
			.lpfc[ 0 ][ 1 ]				= 0x00,
			.lpfc[ 0 ][ 2 ]				= 0x00,
			.lpfc[ 1 ][ 0 ]				= 0x00,
			.lpfc[ 1 ][ 1 ]				= 0x80,
			.lpfc[ 1 ][ 2 ]				= 0x00,
			.lpfc[ 2 ][ 0 ]				= 0x00,
			.lpfc[ 2 ][ 1 ]				= 0x00,
			.lpfc[ 2 ][ 2 ]				= 0x00,
			.blend_offset[ 0 ]			= 0x00,
			.blend_offset[ 1 ]			= 0x00,
			.blend_offset[ 2 ]			= 0x00,
			.blend_offset[ 3 ]			= 0x00,
			.blend_offset[ 4 ]			= 0x00,
			.blend_offset[ 5 ]			= 0x00,
			.blend_offset[ 6 ]			= 0xFF,
			.blend_slope[ 0 ]			= 0x0000,
			.blend_slope[ 1 ]			= 0x0000,
			.blend_slope[ 2 ]			= 0x0000,
			.blend_slope[ 3 ]			= 0x0000,
			.blend_slope[ 4 ]			= 0x0000,
			.blend_slope[ 5 ]			= 0x00FF,
			.blend_slope[ 6 ]			= 0x0000,
			.blend_boundary[ 0 ]		= 0x000,		// D.C.
			.blend_boundary[ 1 ]		= 0x000,
			.blend_boundary[ 2 ]		= 0x000,
			.blend_boundary[ 3 ]		= 0x000,
			.blend_boundary[ 4 ]		= 0x000,
			.blend_boundary[ 5 ]		= 0x400,
			.blend_boundary[ 6 ]		= 0xC00,
			.norm_gain[ 0 ]				= 0x80,
			.norm_gain[ 1 ]				= 0x80,
			.norm_gain[ 2 ]				= 0x80,
			.norm_offset[ 0 ]			= 0x80,
			.norm_offset[ 1 ]			= 0x80,
			.norm_offset[ 2 ]			= 0x80,
			.msk_mode					= D_IM_SHDR_MSKMD_MSKONLY,
		},
		.bld = {
			.blend_sel					= D_IM_SHDR_BLDSEL_SHORT_LONG,
			.alpha_sel					= D_IM_SHDR_ALPHASEL_MERGEMASK,
			.alpha_val[ 0 ]				= 0xFF,
			.alpha_val[ 1 ]				= 0xFF,
			.alpha_val[ 2 ]				= 0xFF,
		},
		.post = {
			.sft1rnd.sft1rnd_en			= D_IM_SHDR_DISABLE,
			.y2r_coef[ 0 ]				= 0x0100,
			.y2r_coef[ 1 ]				= 0x0001,
			.y2r_coef[ 2 ]				= 0x0166,
			.y2r_coef[ 3 ]				= 0x0100,
			.y2r_coef[ 4 ]				= 0x7FA8,
			.y2r_coef[ 5 ]				= 0x7F48,
			.y2r_coef[ 6 ]				= 0x0100,
			.y2r_coef[ 7 ]				= 0x01C6,
			.y2r_coef[ 8 ]				= 0x7FFE,
			.y2r_clp_th_upper[ 0 ]		= 0xFFFF,
			.y2r_clp_th_upper[ 1 ]		= 0xFFFF,
			.y2r_clp_th_upper[ 2 ]		= 0xFFFF,
			.y2r_clp_th_lower[ 0 ]		= 0x0000,
			.y2r_clp_th_lower[ 1 ]		= 0x0000,
			.y2r_clp_th_lower[ 2 ]		= 0x0000,
			.ev_sel						= D_IM_SHDR_POSTSEL1_Y,
			.mulqpos					= D_IM_SHDR_POSTMULQPOS_Q5,
			.ev_kn_boundary[ 0 ]		= 0x000,		// D.C.
			.ev_kn_boundary[ 1 ]		= 0x000,
			.ev_kn_boundary[ 2 ]		= 0x000,
			.ev_kn_boundary[ 3 ]		= 0x000,
			.ev_kn_boundary[ 4 ]		= 0x000,
			.ev_kn_boundary[ 5 ]		= 0x000,
			.ev_kn_boundary[ 6 ]		= 0x000,
			.ev_kn_boundary[ 7 ]		= 0x000,
			.ev_kn_boundary[ 8 ]		= 0x000,
			.ev_kn_boundary[ 9 ]		= 0x000,
			.ev_kn_offset[ 0 ]			= 0x000,
			.ev_kn_offset[ 1 ]			= 0x0000,
			.ev_kn_offset[ 2 ]			= 0x0000,
			.ev_kn_offset[ 3 ]			= 0x0000,
			.ev_kn_offset[ 4 ]			= 0x0000,
			.ev_kn_offset[ 5 ]			= 0x0000,
			.ev_kn_offset[ 6 ]			= 0x0000,
			.ev_kn_offset[ 7 ]			= 0x0000,
			.ev_kn_offset[ 8 ]			= 0x0000,
			.ev_kn_offset[ 9 ]			= 0x0200,
			.ev_kn_slope[ 0 ]			= 0x0000,
			.ev_kn_slope[ 1 ]			= 0x0000,
			.ev_kn_slope[ 2 ]			= 0x0000,
			.ev_kn_slope[ 3 ]			= 0x0000,
			.ev_kn_slope[ 4 ]			= 0x0000,
			.ev_kn_slope[ 5 ]			= 0x0000,
			.ev_kn_slope[ 6 ]			= 0x0000,
			.ev_kn_slope[ 7 ]			= 0x0000,
			.ev_kn_slope[ 8 ]			= 0x0000,
			.ev_kn_slope[ 9 ]			= 0x0000,
			.css_sel					= D_IM_SHDR_POSTSEL0_BLEND_YCC16,
			.hdrcss_mode				= D_IM_SHDR_HDRCSSMD_YCC422_ODD,
		},
		.resize = {
			.chmode.a_w_en				= D_IM_SHDR_ENABLE,
			.chmode.a_drc_en			= D_IM_SHDR_DISABLE,
			.chmode.a_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.b_w_en				= D_IM_SHDR_DISABLE,
			.chmode.b_rdc_md			= D_IM_RDCMD1_1_4,
			.chmode.b_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.c_w_en				= D_IM_SHDR_ENABLE,
			.chmode.c_rsz_md			= D_IM_RSZMD_ODD1,
			.chmode.c_rdc_en			= D_IM_SHDR_DISABLE,
			.chmode.c_rdc_md			= D_IM_RDCMD_1_2,
			.chmode.c_trm_en			= D_IM_SHDR_DISABLE,
			.chmode.c_drc_en			= D_IM_SHDR_DISABLE,
			.chmode.c_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.d_w_en				= D_IM_SHDR_ENABLE,
			.chmode.d_rsz_md			= D_IM_RSZMD_ODD1,
			.chmode.d_rdc_en			= D_IM_SHDR_ENABLE,
			.chmode.d_rdc_md			= D_IM_RDCMD_1_2,
			.chmode.d_trm_en			= D_IM_SHDR_DISABLE,
			.chmode.d_drc_en			= D_IM_SHDR_DISABLE,
			.chmode.d_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.e_w_en				= D_IM_SHDR_ENABLE,
			.chmode.e_rsz_md			= D_IM_RSZMD_ODD1,
			.chmode.e_rdc_en			= D_IM_SHDR_ENABLE,
			.chmode.e_rdc_md			= D_IM_RDCMD_1_2,
			.chmode.e_trm_en			= D_IM_SHDR_DISABLE,
			.chmode.e_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.f_w_en				= D_IM_SHDR_ENABLE,
			.chmode.f_rsz_md			= D_IM_RDCMD_1_4,
			.chmode.f_rdc_en			= D_IM_SHDR_ENABLE,
			.chmode.f_rdc_md			= D_IM_RDCMD_1_4,
			.chmode.f_trm_en			= D_IM_SHDR_DISABLE,
			.chmode.f_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.woutsz.a_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_HEVC,		// 3840
			.woutsz.a_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_HEVC,		// 2160
			.woutsz.b_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_MEIN,		// 960
			.woutsz.b_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_MEIN,		// 540
			.woutsz.c_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_RIB_FHD,		// 1920
			.woutsz.c_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_RIB_FHD,		// 1080
			.woutsz.d_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_RIB_HD,		// 1280
			.woutsz.d_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_RIB_HD,		// 720
			.woutsz.e_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_HDMI,	// 1280
			.woutsz.e_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_DISP_HDMI,	// 720
			.woutsz.f_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_LCD,	// 640
			.woutsz.f_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_DISP_LCD,	// 360
			.rszhcnt.c_hsta				= 0x0000,
			.rszhcnt.c_hpit				= 0x7FFF,	// 50%
			.rszhcnt.d_hsta				= 0x0000,
			.rszhcnt.d_hpit				= 0x5FFF,	// 66.7%
			.rszhcnt.e_hsta				= 0x0000,
			.rszhcnt.e_hpit				= 0x5FFF,	// 66.7%
			.rszhcnt.f_hsta				= 0x0000,
			.rszhcnt.f_hpit				= 0x5FFF,	// 66.7%
			.rszvcnt.c_vsta				= 0x0000,
			.rszvcnt.c_vpit				= 0x7FFF,	// 50%
			.rszvcnt.d_vsta				= 0x0000,
			.rszvcnt.d_vpit				= 0x5FFF,	// 66.7%
			.rszvcnt.e_vsta				= 0x0000,
			.rszvcnt.e_vpit				= 0x5FFF,	// 66.7%
			.rszvcnt.f_vsta				= 0x0000,
			.rszvcnt.f_vpit				= 0x5FFF,	// 66.7%
			.trmsta.c_hsta				= 0x0000,
			.trmsta.c_vsta				= 0x0000,
			.trmsta.d_hsta				= 0x0000,
			.trmsta.d_vsta				= 0x0000,
			.trmsta.e_hsta				= 0x0000,
			.trmsta.e_vsta				= 0x0000,
			.trmsta.f_hsta				= 0x0000,
			.trmsta.f_vsta				= 0x0000,
			.drcofst.a_y				= 0x00,
			.drcofst.a_cb				= 0x00,
			.drcofst.a_cr				= 0x00,
			.drcofst.c_y				= 0x00,
			.drcofst.c_cb				= 0x00,
			.drcofst.c_cr				= 0x00,
			.drcofst.d_y				= 0x00,
			.drcofst.d_cb				= 0x00,
			.drcofst.d_cr				= 0x00,
			.drcgain.a_y				= 0x100,
			.drcgain.a_cb				= 0x80,
			.drcgain.a_cr				= 0x80,
			.drcgain.c_y				= 0x100,
			.drcgain.c_cb				= 0x80,
			.drcgain.c_cr				= 0x80,
			.drcgain.d_y				= 0x100,
			.drcgain.d_cb				= 0x80,
			.drcgain.d_cr				= 0x80,
			.drcclph.a_y				= 0xFF,
			.drcclph.a_cb				= 0x7F,
			.drcclph.a_cr				= 0x7F,
			.drcclph.c_y				= 0xFF,
			.drcclph.c_cb				= 0x7F,
			.drcclph.c_cr				= 0x7F,
			.drcclph.d_y				= 0xFF,
			.drcclph.d_cb				= 0x7F,
			.drcclph.d_cr				= 0x7F,
			.drcclpl.a_y				= 0x00,
			.drcclpl.a_cb				= 0x80,
			.drcclpl.a_cr				= 0x80,
			.drcclpl.c_y				= 0x00,
			.drcclpl.c_cb				= 0x80,
			.drcclpl.c_cr				= 0x80,
			.drcclpl.d_y				= 0x00,
			.drcclpl.d_cb				= 0x80,
			.drcclpl.d_cr				= 0x80,
		},
		.smc = {
			.mcarmd.mca_mode			= D_IM_MCAMD_NR_EFFECT,
			.mcarmd.mcr_mode			= D_IM_MCRMD_MC,
			.mcmpmd.mcmp_mode			= D_IM_SHDR_ON,
			.mcmpmd.mvw_en				= D_IM_SHDR_DISABLE,
			.mmvval.mmv_x				= 0x40,
			.mmvval.mmv_y				= 0x27,
			.mmvrcnt.mmvro_x			= 0x00,
			.mmvrcnt.mmvro_y			= 0x0E,
			.mmvrcnt.mmvrk_x			= 0x6,
			.mmvrcnt.mmvrk_y			= 0x4,
			.mdbcnt.mdb_x				= 0x5,
			.mdbcnt.mdb_y				= 0x3,
			.mdbrocnt.mdbro_x			= 0x04,
			.mdbrocnt.mdbro_y			= 0x08,
			.mdbrocnt.mdbrk_x			= 0x0,
			.mdbrocnt.mdbrk_y			= 0x0,
			.molrcnt.molr_sm			= 0x0,
			.molrcnt.molr_ss			= 0x4,
			.molrcnt.molr_o				= 0x0E,
			.molrcnt.molr_k				= 0x2,
			.mcpcnt.mcpx_s				= 0x00,
			.mcpcnt.mcpx_o				= 0x10,
			.mcpcnt.mcpx_k				= 0x3,
			.mmcrcnt.mmcr_s				= 0x00A,
			.mmcrcnt.mmcr_k				= 0x3,
			.molecnt.mole_s				= 0x010,
			.molecnt.mole_o				= 0x00,
			.molecnt.mole_k				= 0x2,
			.mmvecnt.mmve_s				= 0x002,
			.mmvecnt.mmve_o				= 0x00,
			.mmvecnt.mmve_k				= 0x3,
			.mcpxecnt.mcpxe_s			= 0x010,
			.mcpxecnt.mcpxe_o			= 0x00,
			.mcpxecnt.mcpxe_k			= 0x2,
			.mvlcnt.mvl_en				= D_IM_SHDR_ENABLE,
			.mvlcnt.mvl_threshold		= 0x20,
			.mvlcnt.mvl_count_threshold	= 0x05,
		},
		.axi = {
			.ax_mode.r_burst			= D_IM_SHDR_BURSTLEN_512,
			.ax_mode.w_burst			= D_IM_SHDR_BURSTLEN_512,
			.ax_mode.w_w_mask			= D_IM_SHDR_WWMASK_ON,
			.r_ctl.a_cahche[ 0 ]		= 0x0,
			.r_ctl.a_cahche[ 1 ]		= 0x0,
			.r_ctl.a_protect[ 0 ]		= 0x2,
			.r_ctl.a_protect[ 1 ]		= 0x2,
			.r_ctl.b_cahche[ 0 ]		= 0x0,
			.r_ctl.b_cahche[ 1 ]		= 0x0,
			.r_ctl.b_protect[ 0 ]		= 0x2,
			.r_ctl.b_protect[ 1 ]		= 0x2,
			.r_ctl.c_cahche[ 0 ]		= 0x0,
			.r_ctl.c_cahche[ 1 ]		= 0x0,
			.r_ctl.c_cahche[ 2 ]		= 0x0,
			.r_ctl.c_cahche[ 3 ]		= 0x0,
			.r_ctl.c_protect[ 0 ]		= 0x2,
			.r_ctl.c_protect[ 1 ]		= 0x2,
			.r_ctl.c_protect[ 2 ]		= 0x2,
			.r_ctl.c_protect[ 3 ]		= 0x2,
			.r_ctl.d_cahche[ 0 ]		= 0x0,
			.r_ctl.d_cahche[ 1 ]		= 0x0,
			.r_ctl.d_protect[ 0 ]		= 0x2,
			.r_ctl.d_protect[ 1 ]		= 0x2,
			.w_ctl.a_cahche[ 0 ]		= 0x0,
			.w_ctl.a_cahche[ 1 ]		= 0x0,
			.w_ctl.a_cahche[ 2 ]		= 0x0,
			.w_ctl.a_cahche[ 3 ]		= 0x0,
			.w_ctl.a_protect[ 0 ]		= 0x2,
			.w_ctl.a_protect[ 1 ]		= 0x2,
			.w_ctl.a_protect[ 2 ]		= 0x2,
			.w_ctl.a_protect[ 3 ]		= 0x2,
			.w_ctl.b_cahche[ 0 ]		= 0x0,
			.w_ctl.b_cahche[ 1 ]		= 0x0,
			.w_ctl.b_cahche[ 2 ]		= 0x0,
			.w_ctl.b_cahche[ 3 ]		= 0x0,
			.w_ctl.b_cahche[ 4 ]		= 0x0,
			.w_ctl.b_cahche[ 5 ]		= 0x0,
			.w_ctl.b_protect[ 0 ]		= 0x2,
			.w_ctl.b_protect[ 1 ]		= 0x2,
			.w_ctl.b_protect[ 2 ]		= 0x2,
			.w_ctl.b_protect[ 3 ]		= 0x2,
			.w_ctl.b_protect[ 4 ]		= 0x2,
			.w_ctl.b_protect[ 5 ]		= 0x2,
			.w_ctl.c_cahche[ 0 ]		= 0x0,
			.w_ctl.c_cahche[ 1 ]		= 0x0,
			.w_ctl.c_cahche[ 2 ]		= 0x0,
			.w_ctl.c_cahche[ 3 ]		= 0x0,
			.w_ctl.c_cahche[ 4 ]		= 0x0,
			.w_ctl.c_cahche[ 5 ]		= 0x0,
			.w_ctl.c_cahche[ 6 ]		= 0x0,
			.w_ctl.c_protect[ 0 ]		= 0x2,
			.w_ctl.c_protect[ 1 ]		= 0x2,
			.w_ctl.c_protect[ 2 ]		= 0x2,
			.w_ctl.c_protect[ 3 ]		= 0x2,
			.w_ctl.c_protect[ 4 ]		= 0x2,
			.w_ctl.c_protect[ 5 ]		= 0x2,
			.w_ctl.c_protect[ 6 ]		= 0x2,
		},
	};

	ct_im_shdr_test( &shdr_param );
}

static VOID ct_im_shdr_test2_3( VOID )
{
	T_IM_SHDR_PARAM	shdr_param = {
		.ctrl = {
			.image_set.in_bit_depth		= D_IM_SHDR_INBITDEPTH_8,
			.image_set.in_c_fmt			= D_IM_SHDR_INCFMT_420SP,		// 420sp in.
			.image_set.in_edge_cpy		= D_IM_SHDR_ON,
			.image_set.in_edge_cut		= D_IM_SHDR_ON,
			.image_set.out_ctrl			= D_IM_SHDR_OUTCTRL_YCC,
			.image_set.out_sel			= D_IM_SHDR_OUTSEL_YCC_16,
			.image_set.out_sel_en		= D_IM_SHDR_DISABLE,
			.image_mode.exp_mag			= 0x0100,
			.image_mode.fil_margin		= D_IM_SHDR_FLITER_MARGIN_32,	// 32pixel.
			.image_mode.rexp_mag_shift	= 0,
			.image_mode.rexp_mag		= 0x00000,
			.image_mode.shift_x			= 0x0000,
			.image_mode.shift_y			= 0x0000,
			.image_mode.fmd_en			= D_IM_SHDR_DISABLE,
			.image_mode.bypass_en		= D_IM_SHDR_DISABLE,
			.image_mode.smc_in			= D_IM_SHDR_SMCIN_STN_REF,
			.image_mode.shdr_in			= D_IM_SHDR_SHDRIN_STN_REF,
			.image_mode.tile_size		= D_IM_SHDR_TILESIZE_1024x1024,
			.sdram.r_addr.a_y			= D_IM_SHDR_IMG_MEM_IN_YCC_REF_ADDR_Y,
			.sdram.r_addr.a_c			= D_IM_SHDR_IMG_MEM_IN_YCC_REF_ADDR_C,
			.sdram.r_addr.b_y			= D_IM_SHDR_IMG_MEM_IN_YCC_STD_ADDR_Y,
			.sdram.r_addr.b_c			= D_IM_SHDR_IMG_MEM_IN_YCC_STD_ADDR_C,
			.sdram.r_addr.c_y			= 0x00000000,
			.sdram.r_addr.c_c			= 0x00000000,
			.sdram.r_addr.d_y			= 0x00000000,
			.sdram.r_addr.d_c			= 0x00000000,
			.sdram.r_addr.e_y			= 0x00000000,
			.sdram.r_addr.e_c			= 0x00000000,
			.sdram.r_addr.f				= 0x00000000,
			.sdram.r_ghsize.a_y			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_REF,
			.sdram.r_ghsize.a_c			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_REF,
			.sdram.r_ghsize.b_y			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_STD,
			.sdram.r_ghsize.b_c			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_STD,
			.sdram.r_ghsize.c_y			= 0x0000,
			.sdram.r_ghsize.c_c			= 0x0000,
			.sdram.r_ghsize.d_y			= 0x0000,
			.sdram.r_ghsize.d_c			= 0x0000,
			.sdram.r_ghsize.e_y			= 0x0000,
			.sdram.r_ghsize.e_c			= 0x0000,
			.sdram.r_ghsize.f			= 0x0000,
			.sdram.r_hsize.a			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_REF,
			.sdram.r_hsize.b			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_STD,
			.sdram.r_hsize.c			= 0x0000,
			.sdram.r_hsize.d			= 0x0000,
			.sdram.r_hsize.e			= 0x0000,
			.sdram.r_vsize.a			= D_IM_SHDR_SRC_IMG_PIXS_LINES_YCC_4K2K_REF,
			.sdram.r_vsize.b			= D_IM_SHDR_SRC_IMG_PIXS_LINES_YCC_4K2K_STD,
			.sdram.r_vsize.c			= 0x0000,
			.sdram.r_vsize.d			= 0x0000,
			.sdram.r_vsize.e			= 0x0000,
			.sdram.r_vfm.a				= D_IM_SHDR_VFMT_RASTER,
			.sdram.w_addr.b_y			= D_IM_SHDR_IMG_MEM_OUT_MEIN_ADDR_Y,
			.sdram.w_addr.b_c			= D_IM_SHDR_IMG_MEM_OUT_MEIN_ADDR_C,
			.sdram.w_addr.e_y			= D_IM_SHDR_IMG_MEM_OUT_DISP_HDMI_ADDR_Y,
			.sdram.w_addr.e_c			= D_IM_SHDR_IMG_MEM_OUT_DISP_HDMI_ADDR_C,
			.sdram.w_addr.f_y			= D_IM_SHDR_IMG_MEM_OUT_DISP_LCD_ADDR_Y,
			.sdram.w_addr.f_c			= D_IM_SHDR_IMG_MEM_OUT_DISP_LCD_ADDR_C,
			.sdram.w_addr.g_r_y			= 0x00000000,
			.sdram.w_addr.g_g_c			= 0x00000000,
			.sdram.w_addr.g_b			= 0x00000000,
			.sdram.w_addr.h_x			= 0x00000000,
			.sdram.w_addr.h_y			= 0x00000000,
			.sdram.w_addr.i				= 0x00000000,
			.sdram.w_ghsize.b_y			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_MEIN,
			.sdram.w_ghsize.b_c			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_MEIN,
			.sdram.w_ghsize.e_y			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_HDMI,
			.sdram.w_ghsize.e_c			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_HDMI,
			.sdram.w_ghsize.f_y			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_LCD,
			.sdram.w_ghsize.f_c			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_LCD,
			.sdram.w_ghsize.g_r_y		= 0x0000,
			.sdram.w_ghsize.g_g_c		= 0x0000,
			.sdram.w_ghsize.g_b			= 0x0000,
			.sdram.w_ghsize.h_x			= 0x0000,
			.sdram.w_ghsize.h_y			= 0x0000,
			.sdram.w_ghsize.i			= 0x0000,
			.sdram.w_vfm.a				= D_IM_SHDR_VFMT_COMPIP_VIDEO,
			.sdram.w_vfm.b				= D_IM_SHDR_VFMT_RASTER,
			.sdram.w_baddr.a_y			= D_IM_SHDR_IMG_MEM_OUT_HEVC_ADDR_Y,
			.sdram.w_baddr.a_c			= D_IM_SHDR_IMG_MEM_OUT_HEVC_ADDR_C,
			.sdram.w_baddr.c_y			= D_IM_SHDR_IMG_MEM_OUT_RIB_FHD_ADDR_Y,
			.sdram.w_baddr.c_c			= D_IM_SHDR_IMG_MEM_OUT_RIB_FHD_ADDR_C,
			.sdram.w_baddr.d_y			= D_IM_SHDR_IMG_MEM_OUT_RIB_HD_ADDR_Y,
			.sdram.w_baddr.d_c			= D_IM_SHDR_IMG_MEM_OUT_RIB_HD_ADDR_C,
			.sdram.w_hsize.a			= D_IM_SHDR_IMG_MEM_HEVC_U8_Y_GLOBAL_WIDTH,
			.sdram.w_hsize.c			= D_IM_SHDR_IMG_MEM_RIB_FHD_U8_Y_GLOBAL_WIDTH,
			.sdram.w_hsize.d			= D_IM_SHDR_IMG_MEM_RIB_HD_U8_Y_GLOBAL_WIDTH,
			.compip.mode				= D_IM_SHDR_COMP_MODE_LOSSY,
			.compip.mc_mode				= D_IM_SHDR_COMP_MC_UNUSED,
			.compip.comp_incfmt			= D_IM_SHDR_COMP_INCFMT_YUV420,
			.compip.r_bypass_mode		= D_IM_SHDR_ENABLE,
			.compip.w_bypass_mode		= D_IM_SHDR_ENABLE,
			.compip.r_img_axi_size		= D_IM_SHDR_COMP_AXI_512,
			.compip.r_comp_axi_size		= D_IM_SHDR_COMP_AXI_512,
			.compip.w_img_axi_size		= D_IM_SHDR_COMP_AXI_512,
			.compip.lint_no				= 0,
			.compip.lint_mask			= D_IM_SHDR_ENABLE,
			.compip.lint_sts			= D_IM_SHDR_DISABLE,
			.compip.w_addr_y			= 0,
			.compip.w_addr_c			= 0,
			.compip.r_addr_y			= 0,
			.compip.r_addr_c			= 0,
			.pCallBack					= (T_IM_SHDR_CALLBACK)ct_im_shdr_callback,
			.pLineInt_CallBack			= (T_IM_SHDR_LINEINT_CALLBACK)ct_im_shdr_lineint_callback,
		},
		.pre = {
			.pre_mode.cus_mode			= D_IM_SHDR_CUS_YCC420_PADDING,		// 420 padding.
			.pre_mode.degamma_mode		= D_IM_SHDR_DEGAMMA_BITEXP,
			.y2r_coef[ 0 ] 				= 0x0100,
			.y2r_coef[ 1 ] 				= 0x0001,
			.y2r_coef[ 2 ] 				= 0x0166,
			.y2r_coef[ 3 ] 				= 0x0100,
			.y2r_coef[ 4 ] 				= 0x7FA8,
			.y2r_coef[ 5 ] 				= 0x7F48,
			.y2r_coef[ 6 ] 				= 0x0100,
			.y2r_coef[ 7 ] 				= 0x01C6,
			.y2r_coef[ 8 ] 				= 0x7FFE,
			.y2r_clp.upper_long0[ 0 ]	= 0x00FF,
			.y2r_clp.upper_long0[ 1 ]	= 0x00FF,
			.y2r_clp.upper_long0[ 2 ]	= 0x00FF,
			.y2r_clp.lower_long0[ 0 ]	= 0x0000,
			.y2r_clp.lower_long0[ 1 ]	= 0x0000,
			.y2r_clp.lower_long0[ 2 ]	= 0x0000,
			.y2r_clp.upper_short[ 0 ]	= 0xFF,
			.y2r_clp.upper_short[ 1 ]	= 0xFF,
			.y2r_clp.upper_short[ 2 ]	= 0xFF,
			.y2r_clp.lower_short[ 0 ]	= 0x00,
			.y2r_clp.lower_short[ 1 ]	= 0x00,
			.y2r_clp.lower_short[ 2 ]	= 0x00,
			.r2y_coef[ 0 ] 				= 0x004D,
			.r2y_coef[ 1 ] 				= 0x0096,
			.r2y_coef[ 2 ] 				= 0x001D,
			.r2y_coef[ 3 ] 				= 0x7FD5,
			.r2y_coef[ 4 ] 				= 0x7FAB,
			.r2y_coef[ 5 ] 				= 0x0080,
			.r2y_coef[ 6 ] 				= 0x0080,
			.r2y_coef[ 7 ] 				= 0x7F95,
			.r2y_coef[ 8 ] 				= 0x7FEB,
			.r2y_clp.upper_long_y		= 0x0FFF,
			.r2y_clp.upper_long_c[ 0 ]	= 0x07FF,
			.r2y_clp.upper_long_c[ 1 ]	= 0x07FF,
			.r2y_clp.lower_long_y		= 0x0000,
			.r2y_clp.lower_long_c[ 0 ]	= 0xF800,
			.r2y_clp.lower_long_c[ 1 ]	= 0xF800,
			.r2y_clp.upper_short_y		= 0xFFF,
			.r2y_clp.upper_short_c[ 0 ]	= 0x7FF,
			.r2y_clp.upper_short_c[ 1 ]	= 0x7FF,
			.r2y_clp.lower_short_y		= 0x000,
			.r2y_clp.lower_short_c[ 0 ]	= 0x800,
			.r2y_clp.lower_short_c[ 1 ]	= 0x800,
			.short_knee.offset[ 0 ]		= 0x00,
			.short_knee.offset[ 1 ]		= 0x00,
			.short_knee.offset[ 2 ]		= 0x00,
			.short_knee.slope[ 0 ]		= 0x00,
			.short_knee.slope[ 1 ]		= 0x00,
			.short_knee.slope[ 2 ]		= 0x20,
			.short_knee.boundary[ 0 ]	= 0x000,
			.short_knee.boundary[ 0 ]	= 0x000,
			.long_knee.offset[ 0 ]		= 0x00,
			.long_knee.offset[ 1 ]		= 0x00,
			.long_knee.offset[ 2 ]		= 0x00,
			.long_knee.slope[ 0 ]		= 0x00,
			.long_knee.slope[ 1 ]		= 0x00,
			.long_knee.slope[ 2 ]		= 0x20,
			.long_knee.boundary[ 0 ]	= 0x000,
			.long_knee.boundary[ 0 ]	= 0x000,
		},
		.fmd = {
			.ep_boundary[ 0 ]			= 0x00,		// D.C.
			.ep_boundary[ 1 ]			= 0x40,
			.ep_boundary[ 2 ]			= 0x80,
			.ep_boundary[ 3 ]			= 0xC0,
			.ep_offset[ 0 ]				= 0x0A,
			.ep_offset[ 1 ]				= 0x0A,
			.ep_offset[ 2 ]				= 0x0A,
			.ep_offset[ 3 ]				= 0x0A,
			.ep_blend					= 0x0,
			.fd_weight					= 0x10,
			.mvcnt.bounds				= 0x20,
			.mvcnt.threshold			= 0x80,
			.mvcnt.gr					= 0x1F6E,
			.bldr_offset[ 0 ]			= 0x00,
			.bldr_offset[ 1 ]			= 0x00,
			.bldr_offset[ 2 ]			= 0x00,
			.bldr_offset[ 3 ]			= 0x00,
			.bldr_offset[ 4 ]			= 0xFF,
			.bldr_slope[ 0 ]			= 0x000,
			.bldr_slope[ 1 ]			= 0x000,
			.bldr_slope[ 2 ]			= 0x000,
			.bldr_slope[ 3 ]			= 0x000,
			.bldr_slope[ 4 ]			= 0x000,
			.bldr_boundary[ 0 ]			= 0x00,		// D.C.
			.bldr_boundary[ 1 ]			= 0x00,
			.bldr_boundary[ 2 ]			= 0x00,
			.bldr_boundary[ 3 ]			= 0x00,
			.bldr_boundary[ 4 ]			= 0x06,
			.weight_r2y_coef[ 0 ]		= 0x004D,
			.weight_r2y_coef[ 1 ]		= 0x0096,
			.weight_r2y_coef[ 2 ]		= 0x001D,
			.weight_r2y_clp.upper		= 0xFF,
			.weight_r2y_clp.lower		= 0x00,
			.weight_base				= 0x0,
			.weight_offset[ 0 ][ 0 ]	= 0x60E,
			.weight_offset[ 0 ][ 1 ]	= 0x708,
			.weight_offset[ 0 ][ 2 ]	= 0x898,
			.weight_offset[ 0 ][ 3 ]	= 0xD48,
			.weight_offset[ 0 ][ 4 ]	= 0xFFF,
			.weight_offset[ 1 ][ 0 ]	= 0x60E,
			.weight_offset[ 1 ][ 1 ]	= 0x708,
			.weight_offset[ 1 ][ 2 ]	= 0x898,
			.weight_offset[ 1 ][ 3 ]	= 0xD48,
			.weight_offset[ 1 ][ 4 ]	= 0xFFF,
			.weight_offset[ 2 ][ 0 ]	= 0x60E,
			.weight_offset[ 2 ][ 1 ]	= 0x708,
			.weight_offset[ 2 ][ 2 ]	= 0x898,
			.weight_offset[ 2 ][ 3 ]	= 0xD48,
			.weight_offset[ 2 ][ 4 ]	= 0xFFF,
			.weight_slope[ 0 ][ 0 ]		= 0x274,
			.weight_slope[ 0 ][ 1 ]		= 0x3EC,
			.weight_slope[ 0 ][ 2 ]		= 0xBC4,
			.weight_slope[ 0 ][ 3 ]		= 0x6D1,
			.weight_slope[ 0 ][ 4 ]		= 0x000,
			.weight_slope[ 1 ][ 0 ]		= 0x274,
			.weight_slope[ 1 ][ 1 ]		= 0x3EC,
			.weight_slope[ 1 ][ 2 ]		= 0xBC4,
			.weight_slope[ 1 ][ 3 ]		= 0x6D1,
			.weight_slope[ 1 ][ 4 ]		= 0x000,
			.weight_slope[ 2 ][ 0 ]		= 0x274,
			.weight_slope[ 2 ][ 1 ]		= 0x3EC,
			.weight_slope[ 2 ][ 2 ]		= 0xBC4,
			.weight_slope[ 2 ][ 3 ]		= 0x6D1,
			.weight_slope[ 2 ][ 4 ]		= 0x000,
			.weight_boundary[ 0 ][ 0 ]	= 0x00,		// D.C.
			.weight_boundary[ 0 ][ 1 ]	= 0x33,
			.weight_boundary[ 0 ][ 2 ]	= 0x66,
			.weight_boundary[ 0 ][ 3 ]	= 0x99,
			.weight_boundary[ 0 ][ 4 ]	= 0xCC,
			.weight_boundary[ 1 ][ 0 ]	= 0x00,		// D.C.
			.weight_boundary[ 1 ][ 1 ]	= 0x33,
			.weight_boundary[ 1 ][ 2 ]	= 0x66,
			.weight_boundary[ 1 ][ 3 ]	= 0x99,
			.weight_boundary[ 1 ][ 4 ]	= 0xCC,
			.weight_boundary[ 2 ][ 0 ]	= 0x00,		// D.C.
			.weight_boundary[ 2 ][ 1 ]	= 0x33,
			.weight_boundary[ 2 ][ 2 ]	= 0x66,
			.weight_boundary[ 2 ][ 3 ]	= 0x99,
			.weight_boundary[ 2 ][ 4 ]	= 0xCC,
			.iso_threshold				= 0x010,
			.iso_coef[ 0 ]				= 0x1,
			.iso_coef[ 1 ]				= 0x1,
			.iso_coef[ 2 ]				= 0x1,
			.iso_coef[ 3 ]				= 0x01,
			.iso_coef[ 4 ]				= 0x01,
			.iso_coef[ 5 ]				= 0x01,
			.sat_threshold				= 0xFF,
			.exp77_mode					= D_IM_SHDR_EXP77MD_COEFSUMDIV,
			.fmd_sel					= D_IM_SHDR_FMDSEL_REDUCTION,
		},
		.pmsk = {
			.tlut[ 0 ]					= 0xFF,
			.tlut[ 1 ]					= 0xEF,
			.tlut[ 2 ]					= 0xDF,
			.tlut[ 3 ]					= 0xCF,
			.tlut[ 4 ]					= 0xBF,
			.tlut[ 5 ]					= 0xAF,
			.tlut[ 6 ]					= 0x9F,
			.tlut[ 7 ]					= 0x8F,
			.tlut[ 8 ]					= 0x7F,
			.tlut[ 9 ]					= 0x6F,
			.tlut[ 10 ]					= 0x5F,
			.tlut[ 11 ]					= 0x4F,
			.tlut[ 12 ]					= 0x3F,
			.tlut[ 13 ]					= 0x2F,
			.tlut[ 14 ]					= 0x1F,
			.tlut[ 15 ]					= 0x0F,
			.tlut[ 16 ]					= 0x00,
			.pmsk_mode					= D_IM_SHDR_PMSKMD_MAX,
		},
		.spnr = {
			.spnr_en					= D_IM_SHDR_ENABLE,
			.spe_blend					= 0x7,
			.spe_boundary[ 0 ]			= 0x000,		// D.C.
			.spe_boundary[ 1 ]			= 0x400,
			.spe_boundary[ 2 ]			= 0x800,
			.spe_boundary[ 3 ]			= 0xC00,
			.spe_offset[ 0 ]			= 0x0A0,
			.spe_offset[ 1 ]			= 0x0A0,
			.spe_offset[ 2 ]			= 0x0A0,
			.spe_offset[ 3 ]			= 0x0A0,
		},
		.msk = {
			.y2r_clp_upper_long[ 0 ]	= 0xFFF,
			.y2r_clp_upper_long[ 1 ]	= 0xFFF,
			.y2r_clp_upper_long[ 2 ]	= 0xFFF,
			.y2r_clp_lower_long[ 0 ]	= 0x000,
			.y2r_clp_lower_long[ 1 ]	= 0x000,
			.y2r_clp_lower_long[ 2 ]	= 0x000,
			.y2r_clp_upper_short[ 0 ]	= 0xFFF,
			.y2r_clp_upper_short[ 1 ]	= 0xFFF,
			.y2r_clp_upper_short[ 2 ]	= 0xFFF,
			.y2r_clp_lower_short[ 0 ]	= 0x000,
			.y2r_clp_lower_short[ 1 ]	= 0x000,
			.y2r_clp_lower_short[ 2 ]	= 0x000,
			.alpha						= 0x20,
			.lpfc[ 0 ][ 0 ]				= 0x00,
			.lpfc[ 0 ][ 1 ]				= 0x00,
			.lpfc[ 0 ][ 2 ]				= 0x00,
			.lpfc[ 1 ][ 0 ]				= 0x00,
			.lpfc[ 1 ][ 1 ]				= 0x80,
			.lpfc[ 1 ][ 2 ]				= 0x00,
			.lpfc[ 2 ][ 0 ]				= 0x00,
			.lpfc[ 2 ][ 1 ]				= 0x00,
			.lpfc[ 2 ][ 2 ]				= 0x00,
			.blend_offset[ 0 ]			= 0x00,
			.blend_offset[ 1 ]			= 0x00,
			.blend_offset[ 2 ]			= 0x00,
			.blend_offset[ 3 ]			= 0x00,
			.blend_offset[ 4 ]			= 0x00,
			.blend_offset[ 5 ]			= 0x00,
			.blend_offset[ 6 ]			= 0xFF,
			.blend_slope[ 0 ]			= 0x0000,
			.blend_slope[ 1 ]			= 0x0000,
			.blend_slope[ 2 ]			= 0x0000,
			.blend_slope[ 3 ]			= 0x0000,
			.blend_slope[ 4 ]			= 0x0000,
			.blend_slope[ 5 ]			= 0x00FF,
			.blend_slope[ 6 ]			= 0x0000,
			.blend_boundary[ 0 ]		= 0x000,		// D.C.
			.blend_boundary[ 1 ]		= 0x000,
			.blend_boundary[ 2 ]		= 0x000,
			.blend_boundary[ 3 ]		= 0x000,
			.blend_boundary[ 4 ]		= 0x000,
			.blend_boundary[ 5 ]		= 0x400,
			.blend_boundary[ 6 ]		= 0xC00,
			.norm_gain[ 0 ]				= 0x80,
			.norm_gain[ 1 ]				= 0x80,
			.norm_gain[ 2 ]				= 0x80,
			.norm_offset[ 0 ]			= 0x80,
			.norm_offset[ 1 ]			= 0x80,
			.norm_offset[ 2 ]			= 0x80,
			.msk_mode					= D_IM_SHDR_MSKMD_MSKONLY,
		},
		.bld = {
			.blend_sel					= D_IM_SHDR_BLDSEL_SHORT_LONG,
			.alpha_sel					= D_IM_SHDR_ALPHASEL_MERGEMASK,
			.alpha_val[ 0 ]				= 0xFF,
			.alpha_val[ 1 ]				= 0xFF,
			.alpha_val[ 2 ]				= 0xFF,
		},
		.post = {
			.sft1rnd.sft1rnd_en			= D_IM_SHDR_DISABLE,
			.y2r_coef[ 0 ]				= 0x0100,
			.y2r_coef[ 1 ]				= 0x0001,
			.y2r_coef[ 2 ]				= 0x0166,
			.y2r_coef[ 3 ]				= 0x0100,
			.y2r_coef[ 4 ]				= 0x7FA8,
			.y2r_coef[ 5 ]				= 0x7F48,
			.y2r_coef[ 6 ]				= 0x0100,
			.y2r_coef[ 7 ]				= 0x01C6,
			.y2r_coef[ 8 ]				= 0x7FFE,
			.y2r_clp_th_upper[ 0 ]		= 0xFFFF,
			.y2r_clp_th_upper[ 1 ]		= 0xFFFF,
			.y2r_clp_th_upper[ 2 ]		= 0xFFFF,
			.y2r_clp_th_lower[ 0 ]		= 0x0000,
			.y2r_clp_th_lower[ 1 ]		= 0x0000,
			.y2r_clp_th_lower[ 2 ]		= 0x0000,
			.ev_sel						= D_IM_SHDR_POSTSEL1_Y,
			.mulqpos					= D_IM_SHDR_POSTMULQPOS_Q5,
			.ev_kn_boundary[ 0 ]		= 0x000,		// D.C.
			.ev_kn_boundary[ 1 ]		= 0x000,
			.ev_kn_boundary[ 2 ]		= 0x000,
			.ev_kn_boundary[ 3 ]		= 0x000,
			.ev_kn_boundary[ 4 ]		= 0x000,
			.ev_kn_boundary[ 5 ]		= 0x000,
			.ev_kn_boundary[ 6 ]		= 0x000,
			.ev_kn_boundary[ 7 ]		= 0x000,
			.ev_kn_boundary[ 8 ]		= 0x000,
			.ev_kn_boundary[ 9 ]		= 0x000,
			.ev_kn_offset[ 0 ]			= 0x000,
			.ev_kn_offset[ 1 ]			= 0x0000,
			.ev_kn_offset[ 2 ]			= 0x0000,
			.ev_kn_offset[ 3 ]			= 0x0000,
			.ev_kn_offset[ 4 ]			= 0x0000,
			.ev_kn_offset[ 5 ]			= 0x0000,
			.ev_kn_offset[ 6 ]			= 0x0000,
			.ev_kn_offset[ 7 ]			= 0x0000,
			.ev_kn_offset[ 8 ]			= 0x0000,
			.ev_kn_offset[ 9 ]			= 0x0200,
			.ev_kn_slope[ 0 ]			= 0x0000,
			.ev_kn_slope[ 1 ]			= 0x0000,
			.ev_kn_slope[ 2 ]			= 0x0000,
			.ev_kn_slope[ 3 ]			= 0x0000,
			.ev_kn_slope[ 4 ]			= 0x0000,
			.ev_kn_slope[ 5 ]			= 0x0000,
			.ev_kn_slope[ 6 ]			= 0x0000,
			.ev_kn_slope[ 7 ]			= 0x0000,
			.ev_kn_slope[ 8 ]			= 0x0000,
			.ev_kn_slope[ 9 ]			= 0x0000,
			.css_sel					= D_IM_SHDR_POSTSEL0_BLEND_YCC16,
			.hdrcss_mode				= D_IM_SHDR_HDRCSSMD_YCC422_ODD,
		},
		.resize = {
			.chmode.a_w_en				= D_IM_SHDR_ENABLE,
			.chmode.a_drc_en			= D_IM_SHDR_DISABLE,
			.chmode.a_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.b_w_en				= D_IM_SHDR_DISABLE,
			.chmode.b_rdc_md			= D_IM_RDCMD1_1_4,
			.chmode.b_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.c_w_en				= D_IM_SHDR_ENABLE,
			.chmode.c_rsz_md			= D_IM_RSZMD_ODD1,
			.chmode.c_rdc_en			= D_IM_SHDR_DISABLE,
			.chmode.c_rdc_md			= D_IM_RDCMD_1_2,
			.chmode.c_trm_en			= D_IM_SHDR_DISABLE,
			.chmode.c_drc_en			= D_IM_SHDR_DISABLE,
			.chmode.c_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.d_w_en				= D_IM_SHDR_ENABLE,
			.chmode.d_rsz_md			= D_IM_RSZMD_ODD1,
			.chmode.d_rdc_en			= D_IM_SHDR_ENABLE,
			.chmode.d_rdc_md			= D_IM_RDCMD_1_2,
			.chmode.d_trm_en			= D_IM_SHDR_DISABLE,
			.chmode.d_drc_en			= D_IM_SHDR_DISABLE,
			.chmode.d_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.e_w_en				= D_IM_SHDR_ENABLE,
			.chmode.e_rsz_md			= D_IM_RSZMD_ODD1,
			.chmode.e_rdc_en			= D_IM_SHDR_ENABLE,
			.chmode.e_rdc_md			= D_IM_RDCMD_1_2,
			.chmode.e_trm_en			= D_IM_SHDR_DISABLE,
			.chmode.e_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.f_w_en				= D_IM_SHDR_ENABLE,
			.chmode.f_rsz_md			= D_IM_RDCMD_1_4,
			.chmode.f_rdc_en			= D_IM_SHDR_ENABLE,
			.chmode.f_rdc_md			= D_IM_RDCMD_1_4,
			.chmode.f_trm_en			= D_IM_SHDR_DISABLE,
			.chmode.f_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.woutsz.a_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_HEVC,		// 3840
			.woutsz.a_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_HEVC,		// 2160
			.woutsz.b_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_MEIN,		// 960
			.woutsz.b_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_MEIN,		// 540
			.woutsz.c_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_RIB_FHD,		// 1920
			.woutsz.c_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_RIB_FHD,		// 1080
			.woutsz.d_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_RIB_HD,		// 1280
			.woutsz.d_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_RIB_HD,		// 720
			.woutsz.e_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_HDMI,	// 1280
			.woutsz.e_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_DISP_HDMI,	// 720
			.woutsz.f_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_LCD,	// 640
			.woutsz.f_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_DISP_LCD,	// 360
			.rszhcnt.c_hsta				= 0x0000,
			.rszhcnt.c_hpit				= 0x7FFF,	// 50%
			.rszhcnt.d_hsta				= 0x0000,
			.rszhcnt.d_hpit				= 0x5FFF,	// 66.7%
			.rszhcnt.e_hsta				= 0x0000,
			.rszhcnt.e_hpit				= 0x5FFF,	// 66.7%
			.rszhcnt.f_hsta				= 0x0000,
			.rszhcnt.f_hpit				= 0x5FFF,	// 66.7%
			.rszvcnt.c_vsta				= 0x0000,
			.rszvcnt.c_vpit				= 0x7FFF,	// 50%
			.rszvcnt.d_vsta				= 0x0000,
			.rszvcnt.d_vpit				= 0x5FFF,	// 66.7%
			.rszvcnt.e_vsta				= 0x0000,
			.rszvcnt.e_vpit				= 0x5FFF,	// 66.7%
			.rszvcnt.f_vsta				= 0x0000,
			.rszvcnt.f_vpit				= 0x5FFF,	// 66.7%
			.trmsta.c_hsta				= 0x0000,
			.trmsta.c_vsta				= 0x0000,
			.trmsta.d_hsta				= 0x0000,
			.trmsta.d_vsta				= 0x0000,
			.trmsta.e_hsta				= 0x0000,
			.trmsta.e_vsta				= 0x0000,
			.trmsta.f_hsta				= 0x0000,
			.trmsta.f_vsta				= 0x0000,
			.drcofst.a_y				= 0x00,
			.drcofst.a_cb				= 0x00,
			.drcofst.a_cr				= 0x00,
			.drcofst.c_y				= 0x00,
			.drcofst.c_cb				= 0x00,
			.drcofst.c_cr				= 0x00,
			.drcofst.d_y				= 0x00,
			.drcofst.d_cb				= 0x00,
			.drcofst.d_cr				= 0x00,
			.drcgain.a_y				= 0x100,
			.drcgain.a_cb				= 0x80,
			.drcgain.a_cr				= 0x80,
			.drcgain.c_y				= 0x100,
			.drcgain.c_cb				= 0x80,
			.drcgain.c_cr				= 0x80,
			.drcgain.d_y				= 0x100,
			.drcgain.d_cb				= 0x80,
			.drcgain.d_cr				= 0x80,
			.drcclph.a_y				= 0xFF,
			.drcclph.a_cb				= 0x7F,
			.drcclph.a_cr				= 0x7F,
			.drcclph.c_y				= 0xFF,
			.drcclph.c_cb				= 0x7F,
			.drcclph.c_cr				= 0x7F,
			.drcclph.d_y				= 0xFF,
			.drcclph.d_cb				= 0x7F,
			.drcclph.d_cr				= 0x7F,
			.drcclpl.a_y				= 0x00,
			.drcclpl.a_cb				= 0x80,
			.drcclpl.a_cr				= 0x80,
			.drcclpl.c_y				= 0x00,
			.drcclpl.c_cb				= 0x80,
			.drcclpl.c_cr				= 0x80,
			.drcclpl.d_y				= 0x00,
			.drcclpl.d_cb				= 0x80,
			.drcclpl.d_cr				= 0x80,
		},
		.smc = {
			.mcarmd.mca_mode			= D_IM_MCAMD_NR_EFFECT,
			.mcarmd.mcr_mode			= D_IM_MCRMD_MC,
			.mcmpmd.mcmp_mode			= D_IM_SHDR_ON,
			.mcmpmd.mvw_en				= D_IM_SHDR_DISABLE,
			.mmvval.mmv_x				= 0x40,
			.mmvval.mmv_y				= 0x27,
			.mmvrcnt.mmvro_x			= 0x00,
			.mmvrcnt.mmvro_y			= 0x0E,
			.mmvrcnt.mmvrk_x			= 0x6,
			.mmvrcnt.mmvrk_y			= 0x4,
			.mdbcnt.mdb_x				= 0x5,
			.mdbcnt.mdb_y				= 0x3,
			.mdbrocnt.mdbro_x			= 0x04,
			.mdbrocnt.mdbro_y			= 0x08,
			.mdbrocnt.mdbrk_x			= 0x0,
			.mdbrocnt.mdbrk_y			= 0x0,
			.molrcnt.molr_sm			= 0x0,
			.molrcnt.molr_ss			= 0x4,
			.molrcnt.molr_o				= 0x0E,
			.molrcnt.molr_k				= 0x2,
			.mcpcnt.mcpx_s				= 0x00,
			.mcpcnt.mcpx_o				= 0x10,
			.mcpcnt.mcpx_k				= 0x3,
			.mmcrcnt.mmcr_s				= 0x00A,
			.mmcrcnt.mmcr_k				= 0x3,
			.molecnt.mole_s				= 0x010,
			.molecnt.mole_o				= 0x00,
			.molecnt.mole_k				= 0x2,
			.mmvecnt.mmve_s				= 0x002,
			.mmvecnt.mmve_o				= 0x00,
			.mmvecnt.mmve_k				= 0x3,
			.mcpxecnt.mcpxe_s			= 0x010,
			.mcpxecnt.mcpxe_o			= 0x00,
			.mcpxecnt.mcpxe_k			= 0x2,
			.mvlcnt.mvl_en				= D_IM_SHDR_ENABLE,
			.mvlcnt.mvl_threshold		= 0x20,
			.mvlcnt.mvl_count_threshold	= 0x05,
		},
		.axi = {
			.ax_mode.r_burst			= D_IM_SHDR_BURSTLEN_512,
			.ax_mode.w_burst			= D_IM_SHDR_BURSTLEN_512,
			.ax_mode.w_w_mask			= D_IM_SHDR_WWMASK_ON,
			.r_ctl.a_cahche[ 0 ]		= 0x0,
			.r_ctl.a_cahche[ 1 ]		= 0x0,
			.r_ctl.a_protect[ 0 ]		= 0x2,
			.r_ctl.a_protect[ 1 ]		= 0x2,
			.r_ctl.b_cahche[ 0 ]		= 0x0,
			.r_ctl.b_cahche[ 1 ]		= 0x0,
			.r_ctl.b_protect[ 0 ]		= 0x2,
			.r_ctl.b_protect[ 1 ]		= 0x2,
			.r_ctl.c_cahche[ 0 ]		= 0x0,
			.r_ctl.c_cahche[ 1 ]		= 0x0,
			.r_ctl.c_cahche[ 2 ]		= 0x0,
			.r_ctl.c_cahche[ 3 ]		= 0x0,
			.r_ctl.c_protect[ 0 ]		= 0x2,
			.r_ctl.c_protect[ 1 ]		= 0x2,
			.r_ctl.c_protect[ 2 ]		= 0x2,
			.r_ctl.c_protect[ 3 ]		= 0x2,
			.r_ctl.d_cahche[ 0 ]		= 0x0,
			.r_ctl.d_cahche[ 1 ]		= 0x0,
			.r_ctl.d_protect[ 0 ]		= 0x2,
			.r_ctl.d_protect[ 1 ]		= 0x2,
			.w_ctl.a_cahche[ 0 ]		= 0x0,
			.w_ctl.a_cahche[ 1 ]		= 0x0,
			.w_ctl.a_cahche[ 2 ]		= 0x0,
			.w_ctl.a_cahche[ 3 ]		= 0x0,
			.w_ctl.a_protect[ 0 ]		= 0x2,
			.w_ctl.a_protect[ 1 ]		= 0x2,
			.w_ctl.a_protect[ 2 ]		= 0x2,
			.w_ctl.a_protect[ 3 ]		= 0x2,
			.w_ctl.b_cahche[ 0 ]		= 0x0,
			.w_ctl.b_cahche[ 1 ]		= 0x0,
			.w_ctl.b_cahche[ 2 ]		= 0x0,
			.w_ctl.b_cahche[ 3 ]		= 0x0,
			.w_ctl.b_cahche[ 4 ]		= 0x0,
			.w_ctl.b_cahche[ 5 ]		= 0x0,
			.w_ctl.b_protect[ 0 ]		= 0x2,
			.w_ctl.b_protect[ 1 ]		= 0x2,
			.w_ctl.b_protect[ 2 ]		= 0x2,
			.w_ctl.b_protect[ 3 ]		= 0x2,
			.w_ctl.b_protect[ 4 ]		= 0x2,
			.w_ctl.b_protect[ 5 ]		= 0x2,
			.w_ctl.c_cahche[ 0 ]		= 0x0,
			.w_ctl.c_cahche[ 1 ]		= 0x0,
			.w_ctl.c_cahche[ 2 ]		= 0x0,
			.w_ctl.c_cahche[ 3 ]		= 0x0,
			.w_ctl.c_cahche[ 4 ]		= 0x0,
			.w_ctl.c_cahche[ 5 ]		= 0x0,
			.w_ctl.c_cahche[ 6 ]		= 0x0,
			.w_ctl.c_protect[ 0 ]		= 0x2,
			.w_ctl.c_protect[ 1 ]		= 0x2,
			.w_ctl.c_protect[ 2 ]		= 0x2,
			.w_ctl.c_protect[ 3 ]		= 0x2,
			.w_ctl.c_protect[ 4 ]		= 0x2,
			.w_ctl.c_protect[ 5 ]		= 0x2,
			.w_ctl.c_protect[ 6 ]		= 0x2,
		},
	};

	ct_im_shdr_test( &shdr_param );
}

static VOID ct_im_shdr_test2_5( VOID )
{
	T_IM_SHDR_PARAM	shdr_param = {
		.ctrl = {
			.image_set.in_bit_depth		= D_IM_SHDR_INBITDEPTH_8,
			.image_set.in_c_fmt			= D_IM_SHDR_INCFMT_420SP,		// 420sp in.
			.image_set.in_edge_cpy		= D_IM_SHDR_OFF,
			.image_set.in_edge_cut		= D_IM_SHDR_OFF,
			.image_set.out_ctrl			= D_IM_SHDR_OUTCTRL_YCC,
			.image_set.out_sel			= D_IM_SHDR_OUTSEL_YCC_16,
			.image_set.out_sel_en		= D_IM_SHDR_DISABLE,
			.image_mode.exp_mag			= 0x0100,
			.image_mode.fil_margin		= D_IM_SHDR_FLITER_MARGIN_32,	// 32pixel.
			.image_mode.rexp_mag_shift	= 0,
			.image_mode.rexp_mag		= 0x00000,
			.image_mode.shift_x			= 0x0000,
			.image_mode.shift_y			= 0x0000,
			.image_mode.fmd_en			= D_IM_SHDR_DISABLE,
			.image_mode.bypass_en		= D_IM_SHDR_ENABLE,
			.image_mode.smc_in			= D_IM_SHDR_SMCIN_STN_REF,
			.image_mode.shdr_in			= D_IM_SHDR_SHDRIN_STN_REF,
			.image_mode.tile_size		= D_IM_SHDR_TILESIZE_1024x1024,
			.sdram.r_addr.a_y			= 0x00000000,
			.sdram.r_addr.a_c			= 0x00000000,
			.sdram.r_addr.b_y			= D_IM_SHDR_IMG_MEM_IN_YCC_REF_ADDR_Y,
			.sdram.r_addr.b_c			= D_IM_SHDR_IMG_MEM_IN_YCC_REF_ADDR_C,
			.sdram.r_addr.c_y			= 0x00000000,
			.sdram.r_addr.c_c			= 0x00000000,
			.sdram.r_addr.d_y			= 0x00000000,
			.sdram.r_addr.d_c			= 0x00000000,
			.sdram.r_addr.e_y			= 0x00000000,
			.sdram.r_addr.e_c			= 0x00000000,
			.sdram.r_addr.f				= 0x00000000,
			.sdram.r_ghsize.a_y			= 0x0000,
			.sdram.r_ghsize.a_c			= 0x0000,
			.sdram.r_ghsize.b_y			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_REF,
			.sdram.r_ghsize.b_c			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_REF,
			.sdram.r_ghsize.c_y			= 0x0000,
			.sdram.r_ghsize.c_c			= 0x0000,
			.sdram.r_ghsize.d_y			= 0x0000,
			.sdram.r_ghsize.d_c			= 0x0000,
			.sdram.r_ghsize.e_y			= 0x0000,
			.sdram.r_ghsize.e_c			= 0x0000,
			.sdram.r_ghsize.f			= 0x0000,
			.sdram.r_hsize.a			= 0x0000,
			.sdram.r_hsize.b			= D_IM_SHDR_SRC_IMG_PIXS_WIDTH_YCC_4K2K_REF,
			.sdram.r_hsize.c			= 0x0000,
			.sdram.r_hsize.d			= 0x0000,
			.sdram.r_hsize.e			= 0x0000,
			.sdram.r_vsize.a			= 0x0000,
			.sdram.r_vsize.b			= D_IM_SHDR_SRC_IMG_PIXS_LINES_YCC_4K2K_REF,
			.sdram.r_vsize.c			= 0x0000,
			.sdram.r_vsize.d			= 0x0000,
			.sdram.r_vsize.e			= 0x0000,
			.sdram.r_vfm.a				= D_IM_SHDR_VFMT_RASTER,
			.sdram.w_addr.b_y			= D_IM_SHDR_IMG_MEM_OUT_MEIN_ADDR_Y,
			.sdram.w_addr.b_c			= D_IM_SHDR_IMG_MEM_OUT_MEIN_ADDR_C,
			.sdram.w_addr.e_y			= D_IM_SHDR_IMG_MEM_OUT_DISP_HDMI_ADDR_Y,
			.sdram.w_addr.e_c			= D_IM_SHDR_IMG_MEM_OUT_DISP_HDMI_ADDR_C,
			.sdram.w_addr.f_y			= D_IM_SHDR_IMG_MEM_OUT_DISP_LCD_ADDR_Y,
			.sdram.w_addr.f_c			= D_IM_SHDR_IMG_MEM_OUT_DISP_LCD_ADDR_C,
			.sdram.w_addr.g_r_y			= 0x00000000,
			.sdram.w_addr.g_g_c			= 0x00000000,
			.sdram.w_addr.g_b			= 0x00000000,
			.sdram.w_addr.h_x			= 0x00000000,
			.sdram.w_addr.h_y			= 0x00000000,
			.sdram.w_addr.i				= 0x00000000,
			.sdram.w_ghsize.b_y			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_MEIN,
			.sdram.w_ghsize.b_c			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_MEIN,
			.sdram.w_ghsize.e_y			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_HDMI,
			.sdram.w_ghsize.e_c			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_HDMI,
			.sdram.w_ghsize.f_y			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_LCD,
			.sdram.w_ghsize.f_c			= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_LCD,
			.sdram.w_ghsize.g_r_y		= 0x0000,
			.sdram.w_ghsize.g_g_c		= 0x0000,
			.sdram.w_ghsize.g_b			= 0x0000,
			.sdram.w_ghsize.h_x			= 0x0000,
			.sdram.w_ghsize.h_y			= 0x0000,
			.sdram.w_ghsize.i			= 0x0000,
			.sdram.w_vfm.a				= D_IM_SHDR_VFMT_COMPIP_VIDEO,
			.sdram.w_vfm.b				= D_IM_SHDR_VFMT_RASTER,
			.sdram.w_baddr.a_y			= D_IM_SHDR_IMG_MEM_OUT_HEVC_ADDR_Y,
			.sdram.w_baddr.a_c			= D_IM_SHDR_IMG_MEM_OUT_HEVC_ADDR_C,
			.sdram.w_baddr.c_y			= D_IM_SHDR_IMG_MEM_OUT_RIB_FHD_ADDR_Y,
			.sdram.w_baddr.c_c			= D_IM_SHDR_IMG_MEM_OUT_RIB_FHD_ADDR_C,
			.sdram.w_baddr.d_y			= D_IM_SHDR_IMG_MEM_OUT_RIB_HD_ADDR_Y,
			.sdram.w_baddr.d_c			= D_IM_SHDR_IMG_MEM_OUT_RIB_HD_ADDR_C,
			.sdram.w_hsize.a			= D_IM_SHDR_IMG_MEM_HEVC_U8_Y_GLOBAL_WIDTH,
			.sdram.w_hsize.c			= D_IM_SHDR_IMG_MEM_RIB_FHD_U8_Y_GLOBAL_WIDTH,
			.sdram.w_hsize.d			= D_IM_SHDR_IMG_MEM_RIB_HD_U8_Y_GLOBAL_WIDTH,
			.compip.mode				= D_IM_SHDR_COMP_MODE_LOSSY,
			.compip.mc_mode				= D_IM_SHDR_COMP_MC_UNUSED,
			.compip.comp_incfmt			= D_IM_SHDR_COMP_INCFMT_YUV420,
			.compip.r_bypass_mode		= D_IM_SHDR_ENABLE,
			.compip.w_bypass_mode		= D_IM_SHDR_ENABLE,
			.compip.r_img_axi_size		= D_IM_SHDR_COMP_AXI_512,
			.compip.r_comp_axi_size		= D_IM_SHDR_COMP_AXI_512,
			.compip.w_img_axi_size		= D_IM_SHDR_COMP_AXI_512,
			.compip.lint_no				= 0,
			.compip.lint_mask			= D_IM_SHDR_ENABLE,
			.compip.lint_sts			= D_IM_SHDR_DISABLE,
			.compip.w_addr_y			= 0,
			.compip.w_addr_c			= 0,
			.compip.r_addr_y			= 0,
			.compip.r_addr_c			= 0,
			.pCallBack					= (T_IM_SHDR_CALLBACK)ct_im_shdr_callback,
			.pLineInt_CallBack			= (T_IM_SHDR_LINEINT_CALLBACK)ct_im_shdr_lineint_callback,
		},
		.pre = {
			.pre_mode.cus_mode			= D_IM_SHDR_CUS_YCC420_PADDING,
			.pre_mode.degamma_mode		= D_IM_SHDR_DEGAMMA_LUT,
			.y2r_coef[ 0 ] 				= 0x0000,
			.y2r_coef[ 1 ] 				= 0x0000,
			.y2r_coef[ 2 ] 				= 0x0000,
			.y2r_coef[ 3 ] 				= 0x0000,
			.y2r_coef[ 4 ] 				= 0x0000,
			.y2r_coef[ 5 ] 				= 0x0000,
			.y2r_coef[ 6 ] 				= 0x0000,
			.y2r_coef[ 7 ] 				= 0x0000,
			.y2r_coef[ 8 ] 				= 0x0000,
			.y2r_clp.upper_long0[ 0 ]	= 0x0000,
			.y2r_clp.upper_long0[ 1 ]	= 0x0000,
			.y2r_clp.upper_long0[ 2 ]	= 0x0000,
			.y2r_clp.lower_long0[ 0 ]	= 0x0000,
			.y2r_clp.lower_long0[ 1 ]	= 0x0000,
			.y2r_clp.lower_long0[ 2 ]	= 0x0000,
			.y2r_clp.upper_short[ 0 ]	= 0x00,
			.y2r_clp.upper_short[ 1 ]	= 0x00,
			.y2r_clp.upper_short[ 2 ]	= 0x00,
			.y2r_clp.lower_short[ 0 ]	= 0x00,
			.y2r_clp.lower_short[ 1 ]	= 0x00,
			.y2r_clp.lower_short[ 2 ]	= 0x00,
			.r2y_coef[ 0 ] 				= 0x0000,
			.r2y_coef[ 1 ] 				= 0x0000,
			.r2y_coef[ 2 ] 				= 0x0000,
			.r2y_coef[ 3 ] 				= 0x0000,
			.r2y_coef[ 4 ] 				= 0x0000,
			.r2y_coef[ 5 ] 				= 0x0000,
			.r2y_coef[ 6 ] 				= 0x0000,
			.r2y_coef[ 7 ] 				= 0x0000,
			.r2y_coef[ 8 ] 				= 0x0000,
			.r2y_clp.upper_long_y		= 0x0000,
			.r2y_clp.upper_long_c[ 0 ]	= 0x0000,
			.r2y_clp.upper_long_c[ 1 ]	= 0x0000,
			.r2y_clp.lower_long_y		= 0x0000,
			.r2y_clp.lower_long_c[ 0 ]	= 0x0000,
			.r2y_clp.lower_long_c[ 1 ]	= 0x0000,
			.r2y_clp.upper_short_y		= 0x000,
			.r2y_clp.upper_short_c[ 0 ]	= 0x000,
			.r2y_clp.upper_short_c[ 1 ]	= 0x000,
			.r2y_clp.lower_short_y		= 0x000,
			.r2y_clp.lower_short_c[ 0 ]	= 0x000,
			.r2y_clp.lower_short_c[ 1 ]	= 0x000,
			.short_knee.offset[ 0 ]		= 0x00,
			.short_knee.offset[ 1 ]		= 0x00,
			.short_knee.offset[ 2 ]		= 0x00,
			.short_knee.slope[ 0 ]		= 0x00,
			.short_knee.slope[ 1 ]		= 0x00,
			.short_knee.slope[ 2 ]		= 0x00,
			.short_knee.boundary[ 0 ]	= 0x000,
			.short_knee.boundary[ 0 ]	= 0x000,
			.long_knee.offset[ 0 ]		= 0x00,
			.long_knee.offset[ 1 ]		= 0x00,
			.long_knee.offset[ 2 ]		= 0x00,
			.long_knee.slope[ 0 ]		= 0x00,
			.long_knee.slope[ 1 ]		= 0x00,
			.long_knee.slope[ 2 ]		= 0x00,
			.long_knee.boundary[ 0 ]	= 0x000,
			.long_knee.boundary[ 0 ]	= 0x000,
		},
		.fmd = {
			.ep_boundary[ 0 ]			= 0x00,		// D.C.
			.ep_boundary[ 1 ]			= 0x00,
			.ep_boundary[ 2 ]			= 0x00,
			.ep_boundary[ 3 ]			= 0x00,
			.ep_offset[ 0 ]				= 0x00,
			.ep_offset[ 1 ]				= 0x00,
			.ep_offset[ 2 ]				= 0x00,
			.ep_offset[ 3 ]				= 0x00,
			.ep_blend					= 0x0,
			.fd_weight					= 0x00,
			.mvcnt.bounds				= 0x00,
			.mvcnt.threshold			= 0x00,
			.mvcnt.gr					= 0x0000,
			.bldr_offset[ 0 ]			= 0x00,
			.bldr_offset[ 1 ]			= 0x00,
			.bldr_offset[ 2 ]			= 0x00,
			.bldr_offset[ 3 ]			= 0x00,
			.bldr_offset[ 4 ]			= 0x00,
			.bldr_slope[ 0 ]			= 0x000,
			.bldr_slope[ 1 ]			= 0x000,
			.bldr_slope[ 2 ]			= 0x000,
			.bldr_slope[ 3 ]			= 0x000,
			.bldr_slope[ 4 ]			= 0x000,
			.bldr_boundary[ 0 ]			= 0x00,		// D.C.
			.bldr_boundary[ 1 ]			= 0x00,
			.bldr_boundary[ 2 ]			= 0x00,
			.bldr_boundary[ 3 ]			= 0x00,
			.bldr_boundary[ 4 ]			= 0x00,
			.weight_r2y_coef[ 0 ]		= 0x0000,
			.weight_r2y_coef[ 1 ]		= 0x0000,
			.weight_r2y_coef[ 2 ]		= 0x0000,
			.weight_r2y_clp.upper		= 0x00,
			.weight_r2y_clp.lower		= 0x00,
			.weight_base				= 0x0,
			.weight_offset[ 0 ][ 0 ]	= 0x000,
			.weight_offset[ 0 ][ 1 ]	= 0x000,
			.weight_offset[ 0 ][ 2 ]	= 0x000,
			.weight_offset[ 0 ][ 3 ]	= 0x000,
			.weight_offset[ 0 ][ 4 ]	= 0x000,
			.weight_offset[ 1 ][ 0 ]	= 0x000,
			.weight_offset[ 1 ][ 1 ]	= 0x000,
			.weight_offset[ 1 ][ 2 ]	= 0x000,
			.weight_offset[ 1 ][ 3 ]	= 0x000,
			.weight_offset[ 1 ][ 4 ]	= 0x000,
			.weight_offset[ 2 ][ 0 ]	= 0x000,
			.weight_offset[ 2 ][ 1 ]	= 0x000,
			.weight_offset[ 2 ][ 2 ]	= 0x000,
			.weight_offset[ 2 ][ 3 ]	= 0x000,
			.weight_offset[ 2 ][ 4 ]	= 0x000,
			.weight_slope[ 0 ][ 0 ]		= 0x000,
			.weight_slope[ 0 ][ 1 ]		= 0x000,
			.weight_slope[ 0 ][ 2 ]		= 0x000,
			.weight_slope[ 0 ][ 3 ]		= 0x000,
			.weight_slope[ 0 ][ 4 ]		= 0x000,
			.weight_slope[ 1 ][ 0 ]		= 0x000,
			.weight_slope[ 1 ][ 1 ]		= 0x000,
			.weight_slope[ 1 ][ 2 ]		= 0x000,
			.weight_slope[ 1 ][ 3 ]		= 0x000,
			.weight_slope[ 1 ][ 4 ]		= 0x000,
			.weight_slope[ 2 ][ 0 ]		= 0x000,
			.weight_slope[ 2 ][ 1 ]		= 0x000,
			.weight_slope[ 2 ][ 2 ]		= 0x000,
			.weight_slope[ 2 ][ 3 ]		= 0x000,
			.weight_slope[ 2 ][ 4 ]		= 0x000,
			.weight_boundary[ 0 ][ 0 ]	= 0x00,		// D.C.
			.weight_boundary[ 0 ][ 1 ]	= 0x00,
			.weight_boundary[ 0 ][ 2 ]	= 0x00,
			.weight_boundary[ 0 ][ 3 ]	= 0x00,
			.weight_boundary[ 0 ][ 4 ]	= 0x00,
			.weight_boundary[ 1 ][ 0 ]	= 0x00,		// D.C.
			.weight_boundary[ 1 ][ 1 ]	= 0x00,
			.weight_boundary[ 1 ][ 2 ]	= 0x00,
			.weight_boundary[ 1 ][ 3 ]	= 0x00,
			.weight_boundary[ 1 ][ 4 ]	= 0x00,
			.weight_boundary[ 2 ][ 0 ]	= 0x00,		// D.C.
			.weight_boundary[ 2 ][ 1 ]	= 0x00,
			.weight_boundary[ 2 ][ 2 ]	= 0x00,
			.weight_boundary[ 2 ][ 3 ]	= 0x00,
			.weight_boundary[ 2 ][ 4 ]	= 0x00,
			.iso_threshold				= 0x000,
			.iso_coef[ 0 ]				= 0x1,
			.iso_coef[ 1 ]				= 0x1,
			.iso_coef[ 2 ]				= 0x1,
			.iso_coef[ 3 ]				= 0x01,
			.iso_coef[ 4 ]				= 0x01,
			.iso_coef[ 5 ]				= 0x01,
			.sat_threshold				= 0x00,
			.exp77_mode					= D_IM_SHDR_EXP77MD_COEFSUMDIV,
			.fmd_sel					= D_IM_SHDR_FMDSEL_EXPANSION,
		},
		.pmsk = {
			.tlut[ 0 ]					= 0x00,
			.tlut[ 1 ]					= 0x00,
			.tlut[ 2 ]					= 0x00,
			.tlut[ 3 ]					= 0x00,
			.tlut[ 4 ]					= 0x00,
			.tlut[ 5 ]					= 0x00,
			.tlut[ 6 ]					= 0x00,
			.tlut[ 7 ]					= 0x00,
			.tlut[ 8 ]					= 0x00,
			.tlut[ 9 ]					= 0x00,
			.tlut[ 10 ]					= 0x00,
			.tlut[ 11 ]					= 0x00,
			.tlut[ 12 ]					= 0x00,
			.tlut[ 13 ]					= 0x00,
			.tlut[ 14 ]					= 0x00,
			.tlut[ 15 ]					= 0x00,
			.tlut[ 16 ]					= 0x00,
			.pmsk_mode					= D_IM_SHDR_PMSKMD_MAX,
		},
		.spnr = {
			.spnr_en					= D_IM_SHDR_DISABLE,
			.spe_blend					= 0x0,
			.spe_boundary[ 0 ]			= 0x000,		// D.C.
			.spe_boundary[ 1 ]			= 0x000,
			.spe_boundary[ 2 ]			= 0x000,
			.spe_boundary[ 3 ]			= 0x000,
			.spe_offset[ 0 ]			= 0x000,
			.spe_offset[ 1 ]			= 0x000,
			.spe_offset[ 2 ]			= 0x000,
			.spe_offset[ 3 ]			= 0x000,
		},
		.msk = {
			.y2r_clp_upper_long[ 0 ]	= 0x000,
			.y2r_clp_upper_long[ 1 ]	= 0x000,
			.y2r_clp_upper_long[ 2 ]	= 0x000,
			.y2r_clp_lower_long[ 0 ]	= 0x000,
			.y2r_clp_lower_long[ 1 ]	= 0x000,
			.y2r_clp_lower_long[ 2 ]	= 0x000,
			.y2r_clp_upper_short[ 0 ]	= 0x000,
			.y2r_clp_upper_short[ 1 ]	= 0x000,
			.y2r_clp_upper_short[ 2 ]	= 0x000,
			.y2r_clp_lower_short[ 0 ]	= 0x000,
			.y2r_clp_lower_short[ 1 ]	= 0x000,
			.y2r_clp_lower_short[ 2 ]	= 0x000,
			.alpha						= 0x20,
			.lpfc[ 0 ][ 0 ]				= 0x00,
			.lpfc[ 0 ][ 1 ]				= 0x00,
			.lpfc[ 0 ][ 2 ]				= 0x00,
			.lpfc[ 1 ][ 0 ]				= 0x00,
			.lpfc[ 1 ][ 1 ]				= 0x00,
			.lpfc[ 1 ][ 2 ]				= 0x00,
			.lpfc[ 2 ][ 0 ]				= 0x00,
			.lpfc[ 2 ][ 1 ]				= 0x00,
			.lpfc[ 2 ][ 2 ]				= 0x00,
			.blend_offset[ 0 ]			= 0x00,
			.blend_offset[ 1 ]			= 0x00,
			.blend_offset[ 2 ]			= 0x00,
			.blend_offset[ 3 ]			= 0x00,
			.blend_offset[ 4 ]			= 0x00,
			.blend_offset[ 5 ]			= 0x00,
			.blend_offset[ 6 ]			= 0x00,
			.blend_slope[ 0 ]			= 0x0000,
			.blend_slope[ 1 ]			= 0x0000,
			.blend_slope[ 2 ]			= 0x0000,
			.blend_slope[ 3 ]			= 0x0000,
			.blend_slope[ 4 ]			= 0x0000,
			.blend_slope[ 5 ]			= 0x0000,
			.blend_slope[ 6 ]			= 0x0000,
			.blend_boundary[ 0 ]		= 0x000,		// D.C.
			.blend_boundary[ 1 ]		= 0x400,
			.blend_boundary[ 2 ]		= 0x700,
			.blend_boundary[ 3 ]		= 0x7C0,
			.blend_boundary[ 4 ]		= 0x940,
			.blend_boundary[ 5 ]		= 0xA00,
			.blend_boundary[ 6 ]		= 0xC00,
			.norm_gain[ 0 ]				= 0x00,
			.norm_gain[ 1 ]				= 0x00,
			.norm_gain[ 2 ]				= 0x00,
			.norm_offset[ 0 ]			= 0x00,
			.norm_offset[ 1 ]			= 0x00,
			.norm_offset[ 2 ]			= 0x00,
			.msk_mode					= D_IM_SHDR_MSKMD_SHORT,
		},
		.bld = {
			.blend_sel					= D_IM_SHDR_BLDSEL_SHORT_LONG,
			.alpha_sel					= D_IM_SHDR_ALPHASEL_MERGEMASK,
			.alpha_val[ 0 ]				= 0x00,
			.alpha_val[ 1 ]				= 0x00,
			.alpha_val[ 2 ]				= 0x00,
		},
		.post = {
			.sft1rnd.sft1rnd_en			= D_IM_SHDR_DISABLE,
			.y2r_coef[ 0 ]				= 0x0000,
			.y2r_coef[ 1 ]				= 0x0000,
			.y2r_coef[ 2 ]				= 0x0000,
			.y2r_coef[ 3 ]				= 0x0000,
			.y2r_coef[ 4 ]				= 0x0000,
			.y2r_coef[ 5 ]				= 0x0000,
			.y2r_coef[ 6 ]				= 0x0000,
			.y2r_coef[ 7 ]				= 0x0000,
			.y2r_coef[ 8 ]				= 0x0000,
			.y2r_clp_th_upper[ 0 ]		= 0x0000,
			.y2r_clp_th_upper[ 1 ]		= 0x0000,
			.y2r_clp_th_upper[ 2 ]		= 0x0000,
			.y2r_clp_th_lower[ 0 ]		= 0x0000,
			.y2r_clp_th_lower[ 1 ]		= 0x0000,
			.y2r_clp_th_lower[ 2 ]		= 0x0000,
			.ev_sel						= D_IM_SHDR_POSTSEL1_Y,
			.mulqpos					= D_IM_SHDR_POSTMULQPOS_Q5,
			.ev_kn_boundary[ 0 ]		= 0x000,		// D.C.
			.ev_kn_boundary[ 1 ]		= 0x000,
			.ev_kn_boundary[ 2 ]		= 0x000,
			.ev_kn_boundary[ 3 ]		= 0x000,
			.ev_kn_boundary[ 4 ]		= 0x000,
			.ev_kn_boundary[ 5 ]		= 0x000,
			.ev_kn_boundary[ 6 ]		= 0x000,
			.ev_kn_boundary[ 7 ]		= 0x000,
			.ev_kn_boundary[ 8 ]		= 0x000,
			.ev_kn_boundary[ 9 ]		= 0x000,
			.ev_kn_offset[ 0 ]			= 0x000,
			.ev_kn_offset[ 1 ]			= 0x0000,
			.ev_kn_offset[ 2 ]			= 0x0000,
			.ev_kn_offset[ 3 ]			= 0x0000,
			.ev_kn_offset[ 4 ]			= 0x0000,
			.ev_kn_offset[ 5 ]			= 0x0000,
			.ev_kn_offset[ 6 ]			= 0x0000,
			.ev_kn_offset[ 7 ]			= 0x0000,
			.ev_kn_offset[ 8 ]			= 0x0000,
			.ev_kn_offset[ 9 ]			= 0x0200,
			.ev_kn_slope[ 0 ]			= 0x0000,
			.ev_kn_slope[ 1 ]			= 0x0000,
			.ev_kn_slope[ 2 ]			= 0x0000,
			.ev_kn_slope[ 3 ]			= 0x0000,
			.ev_kn_slope[ 4 ]			= 0x0000,
			.ev_kn_slope[ 5 ]			= 0x0000,
			.ev_kn_slope[ 6 ]			= 0x0000,
			.ev_kn_slope[ 7 ]			= 0x0000,
			.ev_kn_slope[ 8 ]			= 0x0000,
			.ev_kn_slope[ 9 ]			= 0x0000,
			.css_sel					= D_IM_SHDR_POSTSEL0_BYPASS_YCC8,	// Bypass.
			.hdrcss_mode				= D_IM_SHDR_HDRCSSMD_YCC422_ODD,
		},
		.resize = {
			.chmode.a_w_en				= D_IM_SHDR_ENABLE,
			.chmode.a_drc_en			= D_IM_SHDR_DISABLE,
			.chmode.a_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.b_w_en				= D_IM_SHDR_DISABLE,
			.chmode.b_rdc_md			= D_IM_RDCMD1_1_4,
			.chmode.b_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.c_w_en				= D_IM_SHDR_ENABLE,
			.chmode.c_rsz_md			= D_IM_RSZMD_ODD1,
			.chmode.c_rdc_en			= D_IM_SHDR_DISABLE,
			.chmode.c_rdc_md			= D_IM_RDCMD_1_2,
			.chmode.c_trm_en			= D_IM_SHDR_DISABLE,
			.chmode.c_drc_en			= D_IM_SHDR_DISABLE,
			.chmode.c_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.d_w_en				= D_IM_SHDR_ENABLE,
			.chmode.d_rsz_md			= D_IM_RSZMD_ODD1,
			.chmode.d_rdc_en			= D_IM_SHDR_ENABLE,
			.chmode.d_rdc_md			= D_IM_RDCMD_1_2,
			.chmode.d_trm_en			= D_IM_SHDR_DISABLE,
			.chmode.d_drc_en			= D_IM_SHDR_DISABLE,
			.chmode.d_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.e_w_en				= D_IM_SHDR_ENABLE,
			.chmode.e_rsz_md			= D_IM_RSZMD_ODD1,
			.chmode.e_rdc_en			= D_IM_SHDR_ENABLE,
			.chmode.e_rdc_md			= D_IM_RDCMD_1_2,
			.chmode.e_trm_en			= D_IM_SHDR_DISABLE,
			.chmode.e_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.chmode.f_w_en				= D_IM_SHDR_ENABLE,
			.chmode.f_rsz_md			= D_IM_RDCMD_1_4,
			.chmode.f_rdc_en			= D_IM_SHDR_ENABLE,
			.chmode.f_rdc_md			= D_IM_RDCMD_1_4,
			.chmode.f_trm_en			= D_IM_SHDR_DISABLE,
			.chmode.f_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
			.woutsz.a_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_HEVC,		// 3840
			.woutsz.a_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_HEVC,		// 2160
			.woutsz.b_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_MEIN,		// 960
			.woutsz.b_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_MEIN,		// 540
			.woutsz.c_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_RIB_FHD,		// 1920
			.woutsz.c_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_RIB_FHD,		// 1080
			.woutsz.d_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_RIB_HD,		// 1280
			.woutsz.d_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_RIB_HD,		// 720
			.woutsz.e_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_HDMI,	// 1280
			.woutsz.e_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_DISP_HDMI,	// 720
			.woutsz.f_hsize				= D_IM_SHDR_DST_IMG_PIXS_WIDTH_DISP_LCD,	// 640
			.woutsz.f_vsize				= D_IM_SHDR_DST_IMG_PIXS_LINES_DISP_LCD,	// 360
			.rszhcnt.c_hsta				= 0x0000,
			.rszhcnt.c_hpit				= 0x7FFF,	// 50%
			.rszhcnt.d_hsta				= 0x0000,
			.rszhcnt.d_hpit				= 0x5FFF,	// 66.7%
			.rszhcnt.e_hsta				= 0x0000,
			.rszhcnt.e_hpit				= 0x5FFF,	// 66.7%
			.rszhcnt.f_hsta				= 0x0000,
			.rszhcnt.f_hpit				= 0x5FFF,	// 66.7%
			.rszvcnt.c_vsta				= 0x0000,
			.rszvcnt.c_vpit				= 0x7FFF,	// 50%
			.rszvcnt.d_vsta				= 0x0000,
			.rszvcnt.d_vpit				= 0x5FFF,	// 66.7%
			.rszvcnt.e_vsta				= 0x0000,
			.rszvcnt.e_vpit				= 0x5FFF,	// 66.7%
			.rszvcnt.f_vsta				= 0x0000,
			.rszvcnt.f_vpit				= 0x5FFF,	// 66.7%
			.trmsta.c_hsta				= 0x0000,
			.trmsta.c_vsta				= 0x0000,
			.trmsta.d_hsta				= 0x0000,
			.trmsta.d_vsta				= 0x0000,
			.trmsta.e_hsta				= 0x0000,
			.trmsta.e_vsta				= 0x0000,
			.trmsta.f_hsta				= 0x0000,
			.trmsta.f_vsta				= 0x0000,
			.drcofst.a_y				= 0x00,
			.drcofst.a_cb				= 0x00,
			.drcofst.a_cr				= 0x00,
			.drcofst.c_y				= 0x00,
			.drcofst.c_cb				= 0x00,
			.drcofst.c_cr				= 0x00,
			.drcofst.d_y				= 0x00,
			.drcofst.d_cb				= 0x00,
			.drcofst.d_cr				= 0x00,
			.drcgain.a_y				= 0x100,
			.drcgain.a_cb				= 0x80,
			.drcgain.a_cr				= 0x80,
			.drcgain.c_y				= 0x100,
			.drcgain.c_cb				= 0x80,
			.drcgain.c_cr				= 0x80,
			.drcgain.d_y				= 0x100,
			.drcgain.d_cb				= 0x80,
			.drcgain.d_cr				= 0x80,
			.drcclph.a_y				= 0xFF,
			.drcclph.a_cb				= 0x7F,
			.drcclph.a_cr				= 0x7F,
			.drcclph.c_y				= 0xFF,
			.drcclph.c_cb				= 0x7F,
			.drcclph.c_cr				= 0x7F,
			.drcclph.d_y				= 0xFF,
			.drcclph.d_cb				= 0x7F,
			.drcclph.d_cr				= 0x7F,
			.drcclpl.a_y				= 0x00,
			.drcclpl.a_cb				= 0x80,
			.drcclpl.a_cr				= 0x80,
			.drcclpl.c_y				= 0x00,
			.drcclpl.c_cb				= 0x80,
			.drcclpl.c_cr				= 0x80,
			.drcclpl.d_y				= 0x00,
			.drcclpl.d_cb				= 0x80,
			.drcclpl.d_cr				= 0x80,
		},
		.smc = {
			.mcarmd.mca_mode			= D_IM_MCAMD_MAX,
			.mcarmd.mcr_mode			= D_IM_MCRMD_STANDARD,
			.mcmpmd.mcmp_mode			= D_IM_SHDR_OFF,
			.mcmpmd.mvw_en				= D_IM_SHDR_DISABLE,
			.mmvval.mmv_x				= 0x00,
			.mmvval.mmv_y				= 0x00,
			.mmvrcnt.mmvro_x			= 0x00,
			.mmvrcnt.mmvro_y			= 0x00,
			.mmvrcnt.mmvrk_x			= 0x0,
			.mmvrcnt.mmvrk_y			= 0x0,
			.mdbcnt.mdb_x				= 0x0,
			.mdbcnt.mdb_y				= 0x0,
			.mdbrocnt.mdbro_x			= 0x00,
			.mdbrocnt.mdbro_y			= 0x00,
			.mdbrocnt.mdbrk_x			= 0x0,
			.mdbrocnt.mdbrk_y			= 0x0,
			.molrcnt.molr_sm			= 0x0,
			.molrcnt.molr_ss			= 0x0,
			.molrcnt.molr_o				= 0x0,
			.molrcnt.molr_k				= 0x0,
			.mcpcnt.mcpx_s				= 0x00,
			.mcpcnt.mcpx_o				= 0x00,
			.mcpcnt.mcpx_k				= 0x0,
			.mmcrcnt.mmcr_s				= 0x000,
			.mmcrcnt.mmcr_k				= 0x0,
			.molecnt.mole_s				= 0x000,
			.molecnt.mole_o				= 0x00,
			.molecnt.mole_k				= 0x0,
			.mmvecnt.mmve_s				= 0x000,
			.mmvecnt.mmve_o				= 0x00,
			.mmvecnt.mmve_k				= 0x0,
			.mcpxecnt.mcpxe_s			= 0x000,
			.mcpxecnt.mcpxe_o			= 0x00,
			.mcpxecnt.mcpxe_k			= 0x0,
			.mvlcnt.mvl_en				= D_IM_SHDR_DISABLE,
			.mvlcnt.mvl_threshold		= 0x00,
			.mvlcnt.mvl_count_threshold	= 0x00,
		},
		.axi = {
			.ax_mode.r_burst			= D_IM_SHDR_BURSTLEN_512,
			.ax_mode.w_burst			= D_IM_SHDR_BURSTLEN_512,
			.ax_mode.w_w_mask			= D_IM_SHDR_WWMASK_ON,
			.r_ctl.a_cahche[ 0 ]		= 0x0,
			.r_ctl.a_cahche[ 1 ]		= 0x0,
			.r_ctl.a_protect[ 0 ]		= 0x2,
			.r_ctl.a_protect[ 1 ]		= 0x2,
			.r_ctl.b_cahche[ 0 ]		= 0x0,
			.r_ctl.b_cahche[ 1 ]		= 0x0,
			.r_ctl.b_protect[ 0 ]		= 0x2,
			.r_ctl.b_protect[ 1 ]		= 0x2,
			.r_ctl.c_cahche[ 0 ]		= 0x0,
			.r_ctl.c_cahche[ 1 ]		= 0x0,
			.r_ctl.c_cahche[ 2 ]		= 0x0,
			.r_ctl.c_cahche[ 3 ]		= 0x0,
			.r_ctl.c_protect[ 0 ]		= 0x2,
			.r_ctl.c_protect[ 1 ]		= 0x2,
			.r_ctl.c_protect[ 2 ]		= 0x2,
			.r_ctl.c_protect[ 3 ]		= 0x2,
			.r_ctl.d_cahche[ 0 ]		= 0x0,
			.r_ctl.d_cahche[ 1 ]		= 0x0,
			.r_ctl.d_protect[ 0 ]		= 0x2,
			.r_ctl.d_protect[ 1 ]		= 0x2,
			.w_ctl.a_cahche[ 0 ]		= 0x0,
			.w_ctl.a_cahche[ 1 ]		= 0x0,
			.w_ctl.a_cahche[ 2 ]		= 0x0,
			.w_ctl.a_cahche[ 3 ]		= 0x0,
			.w_ctl.a_protect[ 0 ]		= 0x2,
			.w_ctl.a_protect[ 1 ]		= 0x2,
			.w_ctl.a_protect[ 2 ]		= 0x2,
			.w_ctl.a_protect[ 3 ]		= 0x2,
			.w_ctl.b_cahche[ 0 ]		= 0x0,
			.w_ctl.b_cahche[ 1 ]		= 0x0,
			.w_ctl.b_cahche[ 2 ]		= 0x0,
			.w_ctl.b_cahche[ 3 ]		= 0x0,
			.w_ctl.b_cahche[ 4 ]		= 0x0,
			.w_ctl.b_cahche[ 5 ]		= 0x0,
			.w_ctl.b_protect[ 0 ]		= 0x2,
			.w_ctl.b_protect[ 1 ]		= 0x2,
			.w_ctl.b_protect[ 2 ]		= 0x2,
			.w_ctl.b_protect[ 3 ]		= 0x2,
			.w_ctl.b_protect[ 4 ]		= 0x2,
			.w_ctl.b_protect[ 5 ]		= 0x2,
			.w_ctl.c_cahche[ 0 ]		= 0x0,
			.w_ctl.c_cahche[ 1 ]		= 0x0,
			.w_ctl.c_cahche[ 2 ]		= 0x0,
			.w_ctl.c_cahche[ 3 ]		= 0x0,
			.w_ctl.c_cahche[ 4 ]		= 0x0,
			.w_ctl.c_cahche[ 5 ]		= 0x0,
			.w_ctl.c_cahche[ 6 ]		= 0x0,
			.w_ctl.c_protect[ 0 ]		= 0x2,
			.w_ctl.c_protect[ 1 ]		= 0x2,
			.w_ctl.c_protect[ 2 ]		= 0x2,
			.w_ctl.c_protect[ 3 ]		= 0x2,
			.w_ctl.c_protect[ 4 ]		= 0x2,
			.w_ctl.c_protect[ 5 ]		= 0x2,
			.w_ctl.c_protect[ 6 ]		= 0x2,
		},
	};

	ct_im_shdr_test( &shdr_param );
}

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
 * @brief       Command processing of SHDR.
 * @param[in]   INT32 argc
 * @param[in]   CHAR** argv
 *
 *	The meaning of parameters
 *	+----------+---------+--------+-------+-------+-----------------------------------------------------+
 *	| P1       | P2      | P4     | P5    | P6    | Meaning                                             |
 *	+----------+---------+--------+-------+-------+-----------------------------------------------------+
 *	| init     |         |        |       |       | SHDR initialize                                     |
 *	|          |         |        |       |       | Im_SHDR_Init()                                      |
 *	|----------+---------+--------+-------+-------+-----------------------------------------------------+
 *	| open     |         |        |       |       | The semaphore of SHDR is acquired                   |
 *	|          |         |        |       |       | Im_SHDR_Open()                                      |
 *	|----------+---------+--------+-------+-------+-----------------------------------------------------+
 *	| close    |         |        |       |       | The semaphore of SHDR is returned                   |
 *	|          |         |        |       |       | Im_SHDR_Close()                                     |
 *	|----------+---------+--------+-------+-------+-----------------------------------------------------+
 *	| ctrl     |         |        |       |       | SHDR parameters set                                 |
 *	|          |         |        |       |       | Im_SHDR_Ctrl_Common()                               |
 *	|----------+---------+--------+-------+-------+-----------------------------------------------------+
 *	| start    |         |        |       |       | SHDR start                                          |
 *	|          |         |        |       |       | Im_SHDR_Start()                                     |
 *	|----------+---------+--------+-------+-------+-----------------------------------------------------+
 *	| wait     |         |        |       |       | SHDR wait end                                       |
 *	|          |         |        |       |       | Im_SHDR_Wait_End()                                  |
 *	|----------+---------+--------+-------+-------+-----------------------------------------------------+
 *	| stop     |         |        |       |       | SHDR stop                                           |
 *	|          |         |        |       |       | Im_SHDR_Stop()                                      |
 *	|----------+---------+--------+-------+-------+-----------------------------------------------------+
 *	| set      | pre     |        |       |       | SHDR pre parameter set                              |
 *	|          |         |        |       |       | Im_SHDR_Set_Pre()                                   |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | fmd     |        |       |       | SHDR FMD parameter set                              |
 *	|          |         |        |       |       | Im_SHDR_Set_Fmd()                                   |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | pmsk    |        |       |       | SHDR PMSK parameter set                             |
 *	|          |         |        |       |       | Im_SHDR_Set_PreMask()                               |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | spnr    |        |       |       | SHDR SPNR parameter set                             |
 *	|          |         |        |       |       | Im_SHDR_Set_Spnr()                                  |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | mask    |        |       |       | SHDR MSK parameter set                              |
 *	|          |         |        |       |       | Im_SHDR_Set_Mask()                                  |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | blend   |        |       |       | SHDR BLD parameter set                              |
 *	|          |         |        |       |       | Im_SHDR_Set_Blend()                                 |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | post    |        |       |       | SHDR POST parameter set                             |
 *	|          |         |        |       |       | Im_SHDR_Set_Post()                                  |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | resize  |        |       |       | SHDR resize parameter set                           |
 *	|          |         |        |       |       | Im_SHDR_Set_Resize()                                |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | smc     |        |       |       | SHDR smc parameter set                              |
 *	|          |         |        |       |       | Im_SHDR_Set_Smc()                                   |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | dgr     |        |       |       | SHDR degamma R table set                            |
 *	|          |         |        |       |       | Im_SHDR_Set_Degamma_R()                             |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | dgg     |        |       |       | SHDR degamma G table set                            |
 *	|          |         |        |       |       | Im_SHDR_Set_Degamma_G()                             |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | dgb     |        |       |       | SHDR degamma B table set                            |
 *	|          |         |        |       |       | Im_SHDR_Set_Degamma_B()                             |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | dg      |        |       |       | SHDR degamma table set                              |
 *	|          |         |        |       |       | Im_SHDR_Set_Degamma()                               |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | axi     |        |       |       | SHDR AXI parameter set                              |
 *	|          |         |        |       |       | Im_SHDR_Set_Axi()                                   |
 *	|----------+---------+--------+-------+-------+-----------------------------------------------------+
 *	| get      | ctrl    |        |       |       | SHDR common parameter get                           |
 *	|          |         |        |       |       | Im_SHDR_Get_Ctrl_Common()                           |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | pre     |        |       |       | SHDR pre parameter get                              |
 *	|          |         |        |       |       | Im_SHDR_Get_Pre()                                   |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | fmd     |        |       |       | SHDR FMD parameter get                              |
 *	|          |         |        |       |       | Im_SHDR_Get_Fmd()                                   |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | pmsk    |        |       |       | SHDR PMSK parameter get                             |
 *	|          |         |        |       |       | Im_SHDR_Get_PreMask()                               |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | spnr    |        |       |       | SHDR SPNR parameter get                             |
 *	|          |         |        |       |       | Im_SHDR_Get_Spnr()                                  |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | mask    |        |       |       | SHDR MSK parameter get                              |
 *	|          |         |        |       |       | Im_SHDR_Get_Mask()                                  |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | blend   |        |       |       | SHDR BLD parameter get                              |
 *	|          |         |        |       |       | Im_SHDR_Get_Blend()                                 |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | post    |        |       |       | SHDR POST parameter get                             |
 *	|          |         |        |       |       | Im_SHDR_Get_Post()                                  |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | resize  |        |       |       | SHDR resize parameter get                           |
 *	|          |         |        |       |       | Im_SHDR_Get_Resize()                                |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | smc     |        |       |       | SHDR smc parameter get                              |
 *	|          |         |        |       |       | Im_SHDR_Get_Smc()                                   |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | dgr     |        |       |       | SHDR degamma R table get                            |
 *	|          |         |        |       |       | Im_SHDR_Get_Degamma_R()                             |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | dgg     |        |       |       | SHDR degamma G table get                            |
 *	|          |         |        |       |       | Im_SHDR_Get_Degamma_G()                             |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | dgb     |        |       |       | SHDR degamma B table get                            |
 *	|          |         |        |       |       | Im_SHDR_Get_Degamma_B()                             |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | dg      |        |       |       | SHDR degamma table get                              |
 *	|          |         |        |       |       | Im_SHDR_Get_Degamma()                               |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | axi     |        |       |       | SHDR AXI parameter get                              |
 *	|          |         |        |       |       | Im_SHDR_Get_Axi()                                   |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | axires  |        |       |       | SHDR AXI response get                               |
 *	|          |         |        |       |       | Im_SHDR_Get_Axi_Response()                          |
 *	+----------+---------+--------+-------+-------+-----------------------------------------------------+
 *	| err      | init                             | Im_SHDR_Init()                                      |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | open                             | Im_SHDR_Open()                                      |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | close                            | Im_SHDR_Close()                                     |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | ctrl                             | Im_SHDR_Ctrl_Common()                               |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_Ctrl_Common()                           |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | start                            | Im_SHDR_Start()                                     |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | wait                             | Im_SHDR_Wait_End()                                  |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | stop                             | Im_SHDR_Stop()                                      |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | pre                              | Im_SHDR_Set_Pre()                                   |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_Pre()                                   |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | fmd                              | Im_SHDR_Set_Fmd()                                   |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_Fmd()                                   |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | pmsk                             | Im_SHDR_Set_PreMask()                               |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_PreMask()                               |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | spnr                             | Im_SHDR_Set_Spnr()                                  |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_Spnr()                                  |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | mask                             | Im_SHDR_Set_Mask()                                  |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_Mask()                                  |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | blend                            | Im_SHDR_Set_Blend()                                 |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_Blend()                                 |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | post                             | Im_SHDR_Set_Post()                                  |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_Post()                                  |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | resize                           | Im_SHDR_Set_Resize()                                |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_Resizet()                               |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | smc                              | Im_SHDR_Set_Smc()                                   |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_Smc()                                   |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | dgr                              | Im_SHDR_Set_Degamma_R()                             |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_Degamma_R()                             |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | dgg                              | Im_SHDR_Set_Degamma_G()                             |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_Degamma_G()                             |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | dgb                              | Im_SHDR_Set_Degamma_B()                             |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_Degamma_B()                             |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | dg                               | Im_SHDR_Set_Degamma()                               |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_Degamma()                               |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | axi                              | Im_SHDR_Set_Axi()                                   |
 *	|          |                                  +-----------------------------------------------------+
 *	|          |                                  | Im_SHDR_Get_Axi()                                   |
 *	|          +---------+--------+-------+-------+-----------------------------------------------------+
 *	|          | axires                           | Im_SHDR_Get_Axi_Response()                          |
 *	|----------+---------+--------+-------+-------+-----------------------------------------------------+
 * 
 * @return      VOID
 */
VOID Ct_Im_SHDR_Main( INT32 argc, char** argv )
{
	INT32	ret;
	UINT32	wait_factor;

	if ( strcmp(argv[1], "init") == 0 ) {
#ifdef PC_DEBUG
		IO_SHDR.SHDRACTL1.bit.TRG = 2;
#endif
		ret = Im_SHDR_Init( FALSE );
		Ddim_Print( ("Im_SHDR_Init() ret = %d\n", ret) );
	}
	else if ( strcmp(argv[1], "open") == 0 ) {
		ret = Im_SHDR_Open( D_DDIM_USER_SEM_WAIT_FEVR );
		Ddim_Print( ("Im_SHDR_Open() ret = %d\n", ret) );
	}
	else if ( strcmp(argv[1], "close") == 0 ) {
		ret = Im_SHDR_Close();
		Ddim_Print( ("Im_SHDR_Close() ret = %d\n", ret) );
	}
	else if ( strcmp(argv[1], "ctrl") == 0 ) {
		ct_im_shdr_set_ctrl();
	}
	else if ( strcmp(argv[1], "start") == 0 ) {
#ifdef PC_DEBUG
		IO_SHDR.SHDRACTL1.bit.TRG = 2;
#endif
		ret = Im_SHDR_Start();
		Ddim_Print( ("Im_SHDR_Start() ret = %d\n", ret) );
	}
	else if ( strcmp(argv[1], "wait") == 0 ) {
#ifdef PC_DEBUG
		(VOID)DDIM_User_Set_Flg( FID_IM_SHDR, 1 );
#endif
		ret = Im_SHDR_Wait_End( &wait_factor, 0 );
		Ddim_Print( ("Im_SHDR_Wait_End() End ret = %d\n", ret) );

#ifdef PC_DEBUG
		(VOID)DDIM_User_Set_Flg( FID_IM_SHDR, 2 );
#endif
		ret = Im_SHDR_Wait_End( &wait_factor, 0 );
		Ddim_Print( ("Im_SHDR_Wait_End() Stop ret = %d\n", ret) );
	}
	else if ( strcmp(argv[1], "stop") == 0 ) {
#ifdef PC_DEBUG
		IO_SHDR.SHDRACTL1.bit.TRG = 3;
#endif
		ret = Im_SHDR_Stop();
		Ddim_Print( ("Im_SHDR_Stop() ret = %d\n", ret) );
	}
	else if ( strcmp(argv[1], "set") == 0 ) {
		if ( strcmp(argv[2], "pre") == 0 ) {
			ct_im_shdr_set_pre();
		}
		else if ( strcmp(argv[2], "fmd") == 0 ) {
			ct_im_shdr_set_fmd();
		}
		else if ( strcmp(argv[2], "pmsk") == 0 ) {
			ct_im_shdr_set_pmsk();
		}
		else if ( strcmp(argv[2], "spnr") == 0 ) {
			ct_im_shdr_set_spnr();
		}
		else if ( strcmp(argv[2], "mask") == 0 ) {
			ct_im_shdr_set_mask();
		}
		else if ( strcmp(argv[2], "blend") == 0 ) {
			ct_im_shdr_set_bld();
		}
		else if ( strcmp(argv[2], "post") == 0 ) {
			ct_im_shdr_set_post();
		}
		else if ( strcmp(argv[2], "resize") == 0 ) {
			ct_im_shdr_set_resize();
		}
		else if ( strcmp(argv[2], "smc") == 0 ) {
			ct_im_shdr_set_smc();
		}
		else if ( strcmp(argv[2], "dgr") == 0 ) {
#ifdef PC_DEBUG
			IO_SHDR.SHDRACTL1.bit.TRG = 2;
#endif
			ct_im_shdr_set_dgr();
		}
		else if ( strcmp(argv[2], "dgg") == 0 ) {
#ifdef PC_DEBUG
			IO_SHDR.SHDRACTL1.bit.TRG = 2;
#endif
			ct_im_shdr_set_dgg();
		}
		else if ( strcmp(argv[2], "dgb") == 0 ) {
#ifdef PC_DEBUG
			IO_SHDR.SHDRACTL1.bit.TRG = 2;
#endif
			ct_im_shdr_set_dgb();
		}
		else if ( strcmp(argv[2], "dg") == 0 ) {
#ifdef PC_DEBUG
			IO_SHDR.SHDRACTL1.bit.TRG = 2;
#endif
			ct_im_shdr_set_dg();
		}
		else if ( strcmp(argv[2], "axi") == 0 ) {
			ct_im_shdr_set_axi();
		}
		else {
			Ddim_Print( ("please check 2nd parameter!\n") );
			return;
		}
	}
	else if ( strcmp(argv[1], "get") == 0 ) {
		if ( strcmp(argv[2], "ctrl") == 0 ) {
			ct_im_shdr_get_ctrl();
		}
		else if ( strcmp(argv[2], "pre") == 0 ) {
			ct_im_shdr_get_pre();
		}
		else if ( strcmp(argv[2], "fmd") == 0 ) {
			ct_im_shdr_get_fmd();
		}
		else if ( strcmp(argv[2], "pmsk") == 0 ) {
			ct_im_shdr_get_pmsk();
		}
		else if ( strcmp(argv[2], "spnr") == 0 ) {
			ct_im_shdr_get_spnr();
		}
		else if ( strcmp(argv[2], "mask") == 0 ) {
			ct_im_shdr_get_mask();
		}
		else if ( strcmp(argv[2], "blend") == 0 ) {
			ct_im_shdr_get_bld();
		}
		else if ( strcmp(argv[2], "post") == 0 ) {
			ct_im_shdr_get_post();
		}
		else if ( strcmp(argv[2], "resize") == 0 ) {
			ct_im_shdr_get_resize();
		}
		else if ( strcmp(argv[2], "smc") == 0 ) {
			ct_im_shdr_get_smc();
		}
		else if ( strcmp(argv[2], "dgr") == 0 ) {
			ct_im_shdr_get_dgr();
		}
		else if ( strcmp(argv[2], "dgg") == 0 ) {
			ct_im_shdr_get_dgg();
		}
		else if ( strcmp(argv[2], "dgb") == 0 ) {
			ct_im_shdr_get_dgb();
		}
		else if ( strcmp(argv[2], "dg") == 0 ) {
			ct_im_shdr_get_dg();
		}
		else if ( strcmp(argv[2], "axi") == 0 ) {
			ct_im_shdr_get_axi();
		}
		else if ( strcmp(argv[2], "axires") == 0 ) {
			ct_im_shdr_get_axires();
		}
		else {
			Ddim_Print( ("please check 2nd parameter!\n") );
			return;
		}
	}
	else if ( strcmp(argv[1], "err") == 0 ) {
		if ( strcmp(argv[2], "init") == 0 ) {
#ifdef PC_DEBUG
			IO_SHDR.SHDRACTL1.bit.TRG = 0;
#endif
			ret = Im_SHDR_Init( FALSE );
			Ddim_Print( ("Im_SHDR_Init() ret = %d\n", ret) );
		}
		else if ( strcmp(argv[2], "open") == 0 ) {
			ret = Im_SHDR_Open( -2 );
			Ddim_Print( ("Im_SHDR_Open() ret = %d\n", ret) );
		}
		else if ( strcmp(argv[2], "close") == 0 ) {
			ret = Im_SHDR_Close();
			Ddim_Print( ("Im_SHDR_Close() ret = %d\n", ret) );
		}
		else if ( strcmp(argv[2], "ctrl") == 0 ) {
			ct_im_shdr_err_ctrl();
		}
		else if ( strcmp(argv[2], "start") == 0 ) {
#ifdef PC_DEBUG
			IO_SHDR.SHDRACTL1.bit.TRG = 0;
#endif
			ret = Im_SHDR_Start();
			Ddim_Print( ("Im_SHDR_Start() ret = %d\n", ret) );
		}
		else if ( strcmp(argv[2], "wait") == 0 ) {
#ifdef PC_DEBUG
			(VOID)DDIM_User_Clr_Flg( FID_IM_SHDR, ~3 );
#endif
			ret = Im_SHDR_Wait_End( &wait_factor, 100 );
			Ddim_Print( ("Im_SHDR_Wait_End() ret = %d\n", ret) );

#ifdef PC_DEBUG
			(VOID)DDIM_User_Set_Flg( FID_IM_SHDR, 1 );
#endif
			ret = Im_SHDR_Wait_End( NULL, 0 );
			Ddim_Print( ("Im_SHDR_Wait_End() ret = %d\n", ret) );

			ret = Im_SHDR_Wait_End( &wait_factor, -2 );
			Ddim_Print( ("Im_SHDR_Wait_End() ret = %d\n", ret) );
		}
		else if ( strcmp(argv[2], "stop") == 0 ) {
#ifdef PC_DEBUG
			IO_SHDR.SHDRACTL1.bit.TRG = 2;
#endif
			ret = Im_SHDR_Stop();
			Ddim_Print( ("Im_SHDR_Stop() ret = %d\n", ret) );
		}
		else if ( strcmp(argv[2], "pre") == 0 ) {
			ct_im_shdr_err_pre();
		}
		else if ( strcmp(argv[2], "fmd") == 0 ) {
			ct_im_shdr_err_fmd();
		}
		else if ( strcmp(argv[2], "pmsk") == 0 ) {
			ct_im_shdr_err_pmsk();
		}
		else if ( strcmp(argv[2], "spnr") == 0 ) {
			ct_im_shdr_err_spnr();
		}
		else if ( strcmp(argv[2], "mask") == 0 ) {
			ct_im_shdr_err_mask();
		}
		else if ( strcmp(argv[2], "blend") == 0 ) {
			ct_im_shdr_err_bld();
		}
		else if ( strcmp(argv[2], "post") == 0 ) {
			ct_im_shdr_err_post();
		}
		else if ( strcmp(argv[2], "resize") == 0 ) {
			ct_im_shdr_err_resize();
		}
		else if ( strcmp(argv[2], "smc") == 0 ) {
			ct_im_shdr_err_smc();
		}
		else if ( strcmp(argv[2], "dgr") == 0 ) {
			ct_im_shdr_err_dgr();
		}
		else if ( strcmp(argv[2], "dgg") == 0 ) {
			ct_im_shdr_err_dgg();
		}
		else if ( strcmp(argv[2], "dgb") == 0 ) {
			ct_im_shdr_err_dgb();
		}
		else if ( strcmp(argv[2], "dg") == 0 ) {
			ct_im_shdr_err_dg();
		}
		else if ( strcmp(argv[2], "axi") == 0 ) {
			ct_im_shdr_err_axi();
		}
		else if ( strcmp(argv[2], "axires") == 0 ) {
			ct_im_shdr_err_axires();
		}
		else {
			Ddim_Print( ("please check 2nd parameter!\n") );
			return;
		}
	}
	// Palladium.
	else if ( strcmp(argv[1], "2") == 0 ) {

#ifdef PC_DEBUG
		IO_SHDR.SHDRACTL1.bit.TRG = 2;
		(VOID)DDIM_User_Set_Flg( FID_IM_SHDR, 1 );
#endif

		if ( strcmp(argv[2], "1") == 0 ) {
			Ddim_Print(( "shdr test2_1 Start [3DNR w/o MC]\n" ));
			ct_im_shdr_test2_1();
			Ddim_Print(( "shdr test2_1 End\n" ));
		}
		else if ( strcmp(argv[2], "2") == 0 ) {
			Ddim_Print(( "shdr test2_2 Start [3DNR with MC]\n" ));
//			ct_im_shdr_test2_2();
			Ddim_Print(( "shdr test2_2 End\n" ));
		}
		else if ( strcmp(argv[2], "3") == 0 ) {
			Ddim_Print(( "shdr test2_3 Start [Video-HDR w/o MC]\n" ));
			ct_im_shdr_test2_3();
			Ddim_Print(( "shdr test2_3 End\n" ));
		}
		else if ( strcmp(argv[2], "4") == 0 ) {
			Ddim_Print(( "shdr test2_4 Start [Video-HDR with MC]\n" ));
//			ct_im_shdr_test2_4();
			Ddim_Print(( "shdr test2_4 End\n" ));
		}
		else if ( strcmp(argv[2], "5") == 0 ) {
			Ddim_Print(( "shdr test2_5 Start [Bypass-YCC]\n" ));
			ct_im_shdr_test2_5();
			Ddim_Print(( "shdr test2_5 End\n" ));
		}
		else if ( strcmp(argv[2], "6") == 0 ) {
			Ddim_Print(( "shdr test2_6 Start [SHDR w/o MC]\n" ));
//			ct_im_shdr_test2_6();
			Ddim_Print(( "shdr test2_6 End\n" ));
		}
		else if ( strcmp(argv[2], "7") == 0 ) {
			Ddim_Print(( "shdr test2_7 Start [SHDR with MC]\n" ));
//			ct_im_shdr_test2_7();
			Ddim_Print(( "shdr test2_7 End\n" ));
		}
		else if ( strcmp(argv[2], "8") == 0 ) {
			Ddim_Print(( "shdr test2_8 Start [MFNR w/o MC]\n" ));
//			ct_im_shdr_test2_8();
			Ddim_Print(( "shdr test2_8 End\n" ));
		}
		else if ( strcmp(argv[2], "9") == 0 ) {
			Ddim_Print(( "shdr test2_9 Start [MFNR with MC]\n" ));
//			ct_im_shdr_test2_9();
			Ddim_Print(( "shdr test2_9 End\n" ));
		}
		else if ( strcmp(argv[2], "10") == 0 ) {
			Ddim_Print(( "shdr test2_10 Start [Force stop]\n" ));
//			ct_im_shdr_test2_10();
			Ddim_Print(( "shdr test2_10 End\n" ));
		}
		else {
			Ddim_Print( ("please check 2nd parameter!\n") );
			return;
		}
	}
	else {
		Ddim_Print( ("please check 1st parameter!\n") );
	}

	return ;
}
