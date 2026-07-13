 /**
 * @file		ip_shdr.c
 * @brief		Source code of SHDR wrapper layer
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */
 

/*----------------------------------------------------------------------*/
/* Include Files														*/
/*----------------------------------------------------------------------*/

#include <string.h>
#include "ip_shdr.h"
#include "im_shdr.h"
#include "dd_arm.h"
#include "ddim_typedef.h"
#include "os_user_custom.h"
#include "fj_debug.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_IP_SHDR_WAIT_TIME				(1000)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static volatile BOOL				gIp_Shdr_Open		= FALSE;
static volatile IP_SHDR_INIT_PRM	gIp_Shdr_Init_Prm;
static volatile IP_SHDR_SET_PRM		gIp_Shdr_Set_Prm;
static volatile IP_SHDR_START_PRM	gIp_Shdr_Start_Prm;

static volatile T_IM_SHDR_CTRL	gIp_Shdr_Ctrl_Prm = {
	.image_set.in_bit_depth		= D_IM_SHDR_INBITDEPTH_8,
	.image_set.in_c_fmt			= D_IM_SHDR_INCFMT_420SP,
	.image_set.in_edge_cpy		= D_IM_SHDR_OFF,
	.image_set.in_edge_cut		= D_IM_SHDR_OFF,
	.image_set.out_ctrl			= D_IM_SHDR_OUTCTRL_YCC,
	.image_set.out_sel			= D_IM_SHDR_OUTSEL_YCC_16,
	.image_set.out_sel_en		= D_IM_SHDR_ENABLE,
	.image_mode.exp_mag			= 0x0100,
	.image_mode.fil_margin		= D_IM_SHDR_FLITER_MARGIN_32,
	.image_mode.rexp_mag_shift	= 0,
	.image_mode.rexp_mag		= 0x10000,
	.image_mode.shift_x			= 0,
	.image_mode.shift_y			= 0,
	.image_mode.fmd_en			= D_IM_SHDR_DISABLE,
	.image_mode.bypass_en		= D_IM_SHDR_DISABLE,
	.image_mode.smc_in			= D_IM_SHDR_SMCIN_EXT_REF,
	.image_mode.shdr_in			= D_IM_SHDR_SHDRIN_STN_SMCOUT,
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
	.sdram.r_ghsize.a_y			= 0x0000,
	.sdram.r_ghsize.a_c			= 0x0000,
	.sdram.r_ghsize.b_y			= 0x0000,
	.sdram.r_ghsize.b_c			= 0x0000,
	.sdram.r_ghsize.c_y			= 0x0000,
	.sdram.r_ghsize.c_c			= 0x0000,
	.sdram.r_ghsize.d_y			= 0x0000,
	.sdram.r_ghsize.d_c			= 0x0000,
	.sdram.r_ghsize.e_y			= 0x0000,
	.sdram.r_ghsize.e_c			= 0x0000,
	.sdram.r_ghsize.f			= 0x0000,
	.sdram.r_hsize.a			= 0x0000,
	.sdram.r_hsize.b			= 0x0000,
	.sdram.r_hsize.c			= 0x0000,
	.sdram.r_hsize.d			= 0x0000,
	.sdram.r_hsize.e			= 0x0000,
	.sdram.r_vsize.a			= 0x0000,
	.sdram.r_vsize.b			= 0x0000,
	.sdram.r_vsize.c			= 0x0000,
	.sdram.r_vsize.d			= 0x0000,
	.sdram.r_vsize.e			= 0x0000,
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
	.sdram.w_ghsize.b_y			= 0x0000,
	.sdram.w_ghsize.b_c			= 0x0000,
	.sdram.w_ghsize.e_y			= 0x0000,
	.sdram.w_ghsize.e_c			= 0x0000,
	.sdram.w_ghsize.f_y			= 0x0000,
	.sdram.w_ghsize.f_c			= 0x0000,
	.sdram.w_ghsize.g_r_y		= 0x0000,
	.sdram.w_ghsize.g_g_c		= 0x0000,
	.sdram.w_ghsize.g_b			= 0x0000,
	.sdram.w_ghsize.h_x			= 0x0000,
	.sdram.w_ghsize.h_y			= 0x0000,
	.sdram.w_ghsize.i			= 0x0000,
	.sdram.w_vfm.a				= D_IM_SHDR_VFMT_RASTER,
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
	.compip.mode				= D_IM_SHDR_COMP_MODE_LOSSY,
	.compip.mc_mode				= D_IM_SHDR_COMP_MC_USED,
	.compip.comp_incfmt			= D_IM_SHDR_COMP_INCFMT_YUV420,
	.compip.r_bypass_mode		= D_IM_SHDR_ENABLE,
	.compip.w_bypass_mode		= D_IM_SHDR_ENABLE,
	.compip.r_img_axi_size		= D_IM_SHDR_COMP_AXI_512,
	.compip.r_comp_axi_size		= D_IM_SHDR_COMP_AXI_512,
	.compip.w_img_axi_size		= D_IM_SHDR_COMP_AXI_512,
	.compip.lint_no				= 0,
	.compip.lint_mask			= D_IM_SHDR_ENABLE,
	.compip.lint_sts			= D_IM_SHDR_DISABLE,
	.compip.w_addr_y			= 0x00000000,
	.compip.w_addr_c			= 0x00000000,
	.compip.r_addr_y			= 0x00000000,
	.compip.r_addr_c			= 0x00000000,
	.pCallBack					= NULL,
	.pLineInt_CallBack			= NULL,
};

static volatile T_IM_SHDR_PRE	gIp_Shdr_Pre_Prm = {
	.pre_mode.cus_mode			= D_IM_SHDR_CUS_YCC420_BILINEAR_A,
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
	.short_knee.boundary[ 1 ]	= 0x000,
	.long_knee.offset[ 0 ]		= 0x00,
	.long_knee.offset[ 1 ]		= 0x00,
	.long_knee.offset[ 2 ]		= 0x00,
	.long_knee.slope[ 0 ]		= 0x00,
	.long_knee.slope[ 1 ]		= 0x00,
	.long_knee.slope[ 2 ]		= 0x20,
	.long_knee.boundary[ 0 ]	= 0x000,
	.long_knee.boundary[ 1 ]	= 0x000,
};

static volatile T_IM_SHDR_FMD	gIp_Shdr_Fmd_Prm = {
	.ep_boundary[ 0 ]			= 0x00,		// D.C.
	.ep_boundary[ 1 ]			= 0x40,
	.ep_boundary[ 2 ]			= 0x80,
	.ep_boundary[ 3 ]			= 0xC0,
	.ep_offset[ 0 ]				= 0x0A,
	.ep_offset[ 1 ]				= 0x0A,
	.ep_offset[ 2 ]				= 0x0A,
	.ep_offset[ 3 ]				= 0x0A,
	.ep_blend					= 0x0,
	.fd_weight					= 0x08,
	.mvcnt.bounds				= 0x00,
	.mvcnt.threshold			= 0x00,
	.mvcnt.gr					= 0x0000,
	.bldr_offset[ 0 ]			= 0x00,
	.bldr_offset[ 1 ]			= 0x08,
	.bldr_offset[ 2 ]			= 0x18,
	.bldr_offset[ 3 ]			= 0xE8,
	.bldr_offset[ 4 ]			= 0xFF,
	.bldr_slope[ 0 ]			= 0x016,
	.bldr_slope[ 1 ]			= 0x040,
	.bldr_slope[ 2 ]			= 0x340,
	.bldr_slope[ 3 ]			= 0x05C,
	.bldr_slope[ 4 ]			= 0x000,
	.bldr_boundary[ 0 ]			= 0x00,		// D.C.
	.bldr_boundary[ 1 ]			= 0x06,
	.bldr_boundary[ 2 ]			= 0x0A,
	.bldr_boundary[ 3 ]			= 0x0E,
	.bldr_boundary[ 4 ]			= 0x12,
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
	.weight_offset[ 0 ][ 4 ]	= 0x100,
	.weight_offset[ 1 ][ 0 ]	= 0x000,
	.weight_offset[ 1 ][ 1 ]	= 0x000,
	.weight_offset[ 1 ][ 2 ]	= 0x000,
	.weight_offset[ 1 ][ 3 ]	= 0x000,
	.weight_offset[ 1 ][ 4 ]	= 0x100,
	.weight_offset[ 2 ][ 0 ]	= 0x000,
	.weight_offset[ 2 ][ 1 ]	= 0x000,
	.weight_offset[ 2 ][ 2 ]	= 0x000,
	.weight_offset[ 2 ][ 3 ]	= 0x000,
	.weight_offset[ 2 ][ 4 ]	= 0x100,
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
	.iso_threshold				= 0x010,
	.iso_coef[ 0 ]				= 0x1,
	.iso_coef[ 1 ]				= 0x1,
	.iso_coef[ 2 ]				= 0x1,
	.iso_coef[ 3 ]				= 0x11,
	.iso_coef[ 4 ]				= 0x01,
	.iso_coef[ 5 ]				= 0x00,
	.sat_threshold				= 0xFE,
	.exp77_mode					= D_IM_SHDR_EXP77MD_COEFSUMDIV,
	.fmd_sel					= D_IM_SHDR_FMDSEL_REDUCTION,
};

static volatile T_IM_SHDR_PMSK	gIp_Shdr_Pmsk_Prm = {
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
};

static volatile T_IM_SHDR_SPNR	gIp_Shdr_Spnr_Prm = {
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
};

static volatile T_IM_SHDR_MSK	gIp_Shdr_Msk_Prm = {
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
	.blend_slope[ 5 ]			= 0x0000,
	.blend_slope[ 6 ]			= 0x00FF,
	.blend_boundary[ 0 ]		= 0x000,		// D.C.
	.blend_boundary[ 1 ]		= 0x000,
	.blend_boundary[ 2 ]		= 0x000,
	.blend_boundary[ 3 ]		= 0x000,
	.blend_boundary[ 4 ]		= 0x000,
	.blend_boundary[ 5 ]		= 0x400,
	.blend_boundary[ 6 ]		= 0xC00,
	.norm_gain[ 0 ]				= 0xFF,
	.norm_gain[ 1 ]				= 0xFF,
	.norm_gain[ 2 ]				= 0xFF,
	.norm_offset[ 0 ]			= 0xFF,
	.norm_offset[ 1 ]			= 0xFF,
	.norm_offset[ 2 ]			= 0xFF,
	.msk_mode					= D_IM_SHDR_MSKMD_MSKONLY,
};

static volatile T_IM_SHDR_BLD	gIp_Shdr_Bld_Prm = {
	.blend_sel					= D_IM_SHDR_BLDSEL_SHORT_LONG,
	.alpha_sel					= D_IM_SHDR_ALPHASEL_MERGEMASK,
	.alpha_val[ 0 ]				= 0xFF,
	.alpha_val[ 1 ]				= 0xFF,
	.alpha_val[ 2 ]				= 0xFF,
};

static volatile T_IM_SHDR_POST	gIp_Shdr_Post_Prm = {
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
	.css_sel					= D_IM_SHDR_POSTSEL0_BYPASS_YCC8,	// Bypass.
	.hdrcss_mode				= D_IM_SHDR_HDRCSSMD_YCC422_ODD,
};

static volatile T_IM_SHDR_RESIZE	gIp_Shdr_Resize_Prm = {
	.chmode.a_w_en				= D_IM_SHDR_DISABLE,
	.chmode.a_drc_en			= D_IM_SHDR_DISABLE,
	.chmode.a_css_md			= D_IM_SHDR_CSSMD_YCC420_CENTER,
	.chmode.b_w_en				= D_IM_SHDR_ENABLE,
	.chmode.b_rdc_md			= D_IM_RDCMD1_1_4,
	.chmode.b_css_md			= D_IM_SHDR_CSSMD_YCC422,
	.chmode.c_w_en				= D_IM_SHDR_DISABLE,
	.chmode.c_rsz_md			= D_IM_RSZMD_ODD1,
	.chmode.c_rdc_en			= D_IM_SHDR_DISABLE,
	.chmode.c_rdc_md			= D_IM_RDCMD_1_2,
	.chmode.c_trm_en			= D_IM_SHDR_DISABLE,
	.chmode.c_drc_en			= D_IM_SHDR_DISABLE,
	.chmode.c_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
	.chmode.d_w_en				= D_IM_SHDR_DISABLE,
	.chmode.d_rsz_md			= D_IM_RSZMD_ODD0,
	.chmode.d_rdc_en			= D_IM_SHDR_DISABLE,
	.chmode.d_rdc_md			= D_IM_RDCMD_1_2,
	.chmode.d_trm_en			= D_IM_SHDR_DISABLE,
	.chmode.d_drc_en			= D_IM_SHDR_DISABLE,
	.chmode.d_css_md			= D_IM_SHDR_CSSMD_YCC422,
	.chmode.e_w_en				= D_IM_SHDR_DISABLE,
	.chmode.e_rsz_md			= D_IM_RSZMD_ODD1,
	.chmode.e_rdc_en			= D_IM_SHDR_ENABLE,
	.chmode.e_rdc_md			= D_IM_RDCMD_1_2,
	.chmode.e_trm_en			= D_IM_SHDR_DISABLE,
	.chmode.e_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
	.chmode.f_w_en				= D_IM_SHDR_DISABLE,
	.chmode.f_rsz_md			= D_IM_RDCMD_1_4,
	.chmode.f_rdc_en			= D_IM_SHDR_ENABLE,
	.chmode.f_rdc_md			= D_IM_RDCMD_1_4,
	.chmode.f_trm_en			= D_IM_SHDR_DISABLE,
	.chmode.f_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
	.woutsz.a_hsize				= 0x0F00,	// 3840
	.woutsz.a_vsize				= 0x0870,	// 2160
	.woutsz.b_hsize				= 0x03C0,	// 960
	.woutsz.b_vsize				= 0x021C,	// 540
	.woutsz.c_hsize				= 0x0780,	// 1920
	.woutsz.c_vsize				= 0x0438,	// 1080
	.woutsz.d_hsize				= 0x0000,
	.woutsz.d_vsize				= 0x0000,
	.woutsz.e_hsize				= 0x0500,	// 1280
	.woutsz.e_vsize				= 0x02D0,	// 720
	.woutsz.f_hsize				= 0x0280,	// 640
	.woutsz.f_vsize				= 0x01E0,	// 480
	.rszhcnt.c_hsta				= 0x0000,
	.rszhcnt.c_hpit				= 0x7FFF,	// 50%
	.rszhcnt.d_hsta				= 0x0000,
	.rszhcnt.d_hpit				= 0x4000,	// 100%
	.rszhcnt.e_hsta				= 0x0000,
	.rszhcnt.e_hpit				= 0x5FFF,	// 66.6%
	.rszhcnt.f_hsta				= 0x0000,
	.rszhcnt.f_hpit				= 0x5FFF,	// 66.6%
	.rszvcnt.c_vsta				= 0x0000,
	.rszvcnt.c_vpit				= 0x7FFF,	// 50%
	.rszvcnt.d_vsta				= 0x0000,
	.rszvcnt.d_vpit				= 0x4000,	// 100%
	.rszvcnt.e_vsta				= 0x0000,
	.rszvcnt.e_vpit				= 0x5FFF,	// 66.6%
	.rszvcnt.f_vsta				= 0x0000,
	.rszvcnt.f_vpit				= 0x4000,	// 100%
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
};

static volatile T_IM_SHDR_SMC	gIp_Shdr_Smc_Prm = {
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
};

static volatile T_IM_SHDR_AXI	gIp_Shdr_Axi_Prm = {
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
};

static volatile USHORT	gIp_Shdr_Degamma_Prm[ 3 ][ D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ] = {
	{	// R.
		0x000, 0x010, 0x020, 0x030, 0x040, 0x050, 0x060, 0x070, 0x080, 0x090, 0x0A0, 0x0B0, 0x0C0, 0x0D0, 0x0E0, 0x0F0,
		0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170, 0x180, 0x190, 0x1A0, 0x1B0, 0x1C0, 0x1D0, 0x1E0, 0x1F0,
		0x200, 0x210, 0x220, 0x230, 0x240, 0x250, 0x260, 0x270, 0x280, 0x290, 0x2A0, 0x2B0, 0x2C0, 0x2D0, 0x2E0, 0x2F0,
		0x300, 0x310, 0x320, 0x330, 0x340, 0x350, 0x360, 0x370, 0x380, 0x390, 0x3A0, 0x3B0, 0x3C0, 0x3D0, 0x3E0, 0x3F0,
		0x400, 0x410, 0x420, 0x430, 0x440, 0x450, 0x460, 0x470, 0x480, 0x490, 0x4A0, 0x4B0, 0x4C0, 0x4D0, 0x4E0, 0x4F0,
		0x500, 0x510, 0x520, 0x530, 0x540, 0x550, 0x560, 0x570, 0x580, 0x590, 0x5A0, 0x5B0, 0x5C0, 0x5D0, 0x5E0, 0x5F0,
		0x600, 0x610, 0x620, 0x630, 0x640, 0x650, 0x660, 0x670, 0x680, 0x690, 0x6A0, 0x6B0, 0x6C0, 0x6D0, 0x6E0, 0x6F0,
		0x700, 0x710, 0x720, 0x730, 0x740, 0x750, 0x760, 0x770, 0x780, 0x790, 0x7A0, 0x7B0, 0x7C0, 0x7D0, 0x7E0, 0x7F0,
		0x800, 0x810, 0x820, 0x830, 0x840, 0x850, 0x860, 0x870, 0x880, 0x890, 0x8A0, 0x8B0, 0x8C0, 0x8D0, 0x8E0, 0x8F0,
		0x900, 0x910, 0x920, 0x930, 0x940, 0x950, 0x960, 0x970, 0x980, 0x990, 0x9A0, 0x9B0, 0x9C0, 0x9D0, 0x9E0, 0x9F0,
		0xA00, 0xA10, 0xA20, 0xA30, 0xA40, 0xA50, 0xA60, 0xA70, 0xA80, 0xA90, 0xAA0, 0xAB0, 0xAC0, 0xAD0, 0xAE0, 0xAF0,
		0xB00, 0xB10, 0xB20, 0xB30, 0xB40, 0xB50, 0xB60, 0xB70, 0xB80, 0xB90, 0xBA0, 0xBB0, 0xBC0, 0xBD0, 0xBE0, 0xBF0,
		0xC00, 0xC10, 0xC20, 0xC30, 0xC40, 0xC50, 0xC60, 0xC70, 0xC80, 0xC90, 0xCA0, 0xCB0, 0xCC0, 0xCD0, 0xCE0, 0xCF0,
		0xD00, 0xD10, 0xD20, 0xD30, 0xD40, 0xD50, 0xD60, 0xD70, 0xD80, 0xD90, 0xDA0, 0xDB0, 0xDC0, 0xDD0, 0xDE0, 0xDF0,
		0xE00, 0xE10, 0xE20, 0xE30, 0xE40, 0xE50, 0xE60, 0xE70, 0xE80, 0xE90, 0xEA0, 0xEB0, 0xEC0, 0xED0, 0xEE0, 0xEF0,
		0xF00, 0xF10, 0xF20, 0xF30, 0xF40, 0xF50, 0xF60, 0xF70, 0xF80, 0xF90, 0xFA0, 0xFB0, 0xFC0, 0xFD0, 0xFE0, 0xFF0,
	},
	{	// G.
		0x000, 0x010, 0x020, 0x030, 0x040, 0x050, 0x060, 0x070, 0x080, 0x090, 0x0A0, 0x0B0, 0x0C0, 0x0D0, 0x0E0, 0x0F0,
		0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170, 0x180, 0x190, 0x1A0, 0x1B0, 0x1C0, 0x1D0, 0x1E0, 0x1F0,
		0x200, 0x210, 0x220, 0x230, 0x240, 0x250, 0x260, 0x270, 0x280, 0x290, 0x2A0, 0x2B0, 0x2C0, 0x2D0, 0x2E0, 0x2F0,
		0x300, 0x310, 0x320, 0x330, 0x340, 0x350, 0x360, 0x370, 0x380, 0x390, 0x3A0, 0x3B0, 0x3C0, 0x3D0, 0x3E0, 0x3F0,
		0x400, 0x410, 0x420, 0x430, 0x440, 0x450, 0x460, 0x470, 0x480, 0x490, 0x4A0, 0x4B0, 0x4C0, 0x4D0, 0x4E0, 0x4F0,
		0x500, 0x510, 0x520, 0x530, 0x540, 0x550, 0x560, 0x570, 0x580, 0x590, 0x5A0, 0x5B0, 0x5C0, 0x5D0, 0x5E0, 0x5F0,
		0x600, 0x610, 0x620, 0x630, 0x640, 0x650, 0x660, 0x670, 0x680, 0x690, 0x6A0, 0x6B0, 0x6C0, 0x6D0, 0x6E0, 0x6F0,
		0x700, 0x710, 0x720, 0x730, 0x740, 0x750, 0x760, 0x770, 0x780, 0x790, 0x7A0, 0x7B0, 0x7C0, 0x7D0, 0x7E0, 0x7F0,
		0x800, 0x810, 0x820, 0x830, 0x840, 0x850, 0x860, 0x870, 0x880, 0x890, 0x8A0, 0x8B0, 0x8C0, 0x8D0, 0x8E0, 0x8F0,
		0x900, 0x910, 0x920, 0x930, 0x940, 0x950, 0x960, 0x970, 0x980, 0x990, 0x9A0, 0x9B0, 0x9C0, 0x9D0, 0x9E0, 0x9F0,
		0xA00, 0xA10, 0xA20, 0xA30, 0xA40, 0xA50, 0xA60, 0xA70, 0xA80, 0xA90, 0xAA0, 0xAB0, 0xAC0, 0xAD0, 0xAE0, 0xAF0,
		0xB00, 0xB10, 0xB20, 0xB30, 0xB40, 0xB50, 0xB60, 0xB70, 0xB80, 0xB90, 0xBA0, 0xBB0, 0xBC0, 0xBD0, 0xBE0, 0xBF0,
		0xC00, 0xC10, 0xC20, 0xC30, 0xC40, 0xC50, 0xC60, 0xC70, 0xC80, 0xC90, 0xCA0, 0xCB0, 0xCC0, 0xCD0, 0xCE0, 0xCF0,
		0xD00, 0xD10, 0xD20, 0xD30, 0xD40, 0xD50, 0xD60, 0xD70, 0xD80, 0xD90, 0xDA0, 0xDB0, 0xDC0, 0xDD0, 0xDE0, 0xDF0,
		0xE00, 0xE10, 0xE20, 0xE30, 0xE40, 0xE50, 0xE60, 0xE70, 0xE80, 0xE90, 0xEA0, 0xEB0, 0xEC0, 0xED0, 0xEE0, 0xEF0,
		0xF00, 0xF10, 0xF20, 0xF30, 0xF40, 0xF50, 0xF60, 0xF70, 0xF80, 0xF90, 0xFA0, 0xFB0, 0xFC0, 0xFD0, 0xFE0, 0xFF0,
	},
	{	// B.
		0x000, 0x010, 0x020, 0x030, 0x040, 0x050, 0x060, 0x070, 0x080, 0x090, 0x0A0, 0x0B0, 0x0C0, 0x0D0, 0x0E0, 0x0F0,
		0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170, 0x180, 0x190, 0x1A0, 0x1B0, 0x1C0, 0x1D0, 0x1E0, 0x1F0,
		0x200, 0x210, 0x220, 0x230, 0x240, 0x250, 0x260, 0x270, 0x280, 0x290, 0x2A0, 0x2B0, 0x2C0, 0x2D0, 0x2E0, 0x2F0,
		0x300, 0x310, 0x320, 0x330, 0x340, 0x350, 0x360, 0x370, 0x380, 0x390, 0x3A0, 0x3B0, 0x3C0, 0x3D0, 0x3E0, 0x3F0,
		0x400, 0x410, 0x420, 0x430, 0x440, 0x450, 0x460, 0x470, 0x480, 0x490, 0x4A0, 0x4B0, 0x4C0, 0x4D0, 0x4E0, 0x4F0,
		0x500, 0x510, 0x520, 0x530, 0x540, 0x550, 0x560, 0x570, 0x580, 0x590, 0x5A0, 0x5B0, 0x5C0, 0x5D0, 0x5E0, 0x5F0,
		0x600, 0x610, 0x620, 0x630, 0x640, 0x650, 0x660, 0x670, 0x680, 0x690, 0x6A0, 0x6B0, 0x6C0, 0x6D0, 0x6E0, 0x6F0,
		0x700, 0x710, 0x720, 0x730, 0x740, 0x750, 0x760, 0x770, 0x780, 0x790, 0x7A0, 0x7B0, 0x7C0, 0x7D0, 0x7E0, 0x7F0,
		0x800, 0x810, 0x820, 0x830, 0x840, 0x850, 0x860, 0x870, 0x880, 0x890, 0x8A0, 0x8B0, 0x8C0, 0x8D0, 0x8E0, 0x8F0,
		0x900, 0x910, 0x920, 0x930, 0x940, 0x950, 0x960, 0x970, 0x980, 0x990, 0x9A0, 0x9B0, 0x9C0, 0x9D0, 0x9E0, 0x9F0,
		0xA00, 0xA10, 0xA20, 0xA30, 0xA40, 0xA50, 0xA60, 0xA70, 0xA80, 0xA90, 0xAA0, 0xAB0, 0xAC0, 0xAD0, 0xAE0, 0xAF0,
		0xB00, 0xB10, 0xB20, 0xB30, 0xB40, 0xB50, 0xB60, 0xB70, 0xB80, 0xB90, 0xBA0, 0xBB0, 0xBC0, 0xBD0, 0xBE0, 0xBF0,
		0xC00, 0xC10, 0xC20, 0xC30, 0xC40, 0xC50, 0xC60, 0xC70, 0xC80, 0xC90, 0xCA0, 0xCB0, 0xCC0, 0xCD0, 0xCE0, 0xCF0,
		0xD00, 0xD10, 0xD20, 0xD30, 0xD40, 0xD50, 0xD60, 0xD70, 0xD80, 0xD90, 0xDA0, 0xDB0, 0xDC0, 0xDD0, 0xDE0, 0xDF0,
		0xE00, 0xE10, 0xE20, 0xE30, 0xE40, 0xE50, 0xE60, 0xE70, 0xE80, 0xE90, 0xEA0, 0xEB0, 0xEC0, 0xED0, 0xEE0, 0xEF0,
		0xF00, 0xF10, 0xF20, 0xF30, 0xF40, 0xF50, 0xF60, 0xF70, 0xF80, 0xF90, 0xFA0, 0xFB0, 0xFC0, 0xFD0, 0xFE0, 0xFF0,
	}
};

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static VOID ip_shdr_int_handler( ULONG interrupt_flag_mask )
{
	T_IM_SHDR_AXI_RESP	res;

	if ( ( interrupt_flag_mask & D_IM_SHDR_AXI_ERR_CH_RW ) != 0 ) {

		Im_SHDR_Get_Axi_Response( &res );

		FJ_Debug_Print_Err( "ip_shdr_int_handler: AXI Error(Read channel)\n" );
		FJ_Debug_Print_Err( "RRESPA_0=0x%x RRESPA_1=0x%x\n",
								res.r_resp_a[ 0 ],
								res.r_resp_a[ 1 ]
							);
		FJ_Debug_Print_Err( "RRESPB_0=0x%x RRESPB_1=0x%x\n",
								res.r_resp_b[ 0 ],
								res.r_resp_b[ 1 ]
							);
		FJ_Debug_Print_Err( "RRESPC_0=0x%x RRESPC_1=0x%x RRESPC_2=0x%x RRESPC_3=0x%x\n",
								res.r_resp_c[ 0 ],
								res.r_resp_c[ 1 ],
								res.r_resp_c[ 2 ],
								res.r_resp_c[ 3 ]
							);
		FJ_Debug_Print_Err( "RRESPD_0=0x%x RRESPD_1=0x%x\n\n",
								res.r_resp_d[ 0 ],
								res.r_resp_d[ 1 ]
							);
		FJ_Debug_Print_Err( "ip_shdr_int_handler: AXI Error(Write channel)\n" );
		FJ_Debug_Print_Err( "BRESPA_0=0x%x BRESPA_1=0x%x BRESPA_2=0x%x BRESPA_3=0x%x\n",
								res.w_resp_a[ 0 ],
								res.w_resp_a[ 1 ],
								res.w_resp_a[ 2 ],
								res.w_resp_a[ 3 ]
							);
		FJ_Debug_Print_Err( "BRESPB_0=0x%x BRESPB_1=0x%x BRESPB_2=0x%x BRESPB_3=0x%x BRESPB_4=0x%x BRESPB_5=0x%x\n",
								res.w_resp_b[ 0 ],
								res.w_resp_b[ 1 ],
								res.w_resp_b[ 2 ],
								res.w_resp_b[ 3 ],
								res.w_resp_b[ 4 ],
								res.w_resp_b[ 5 ]
							);
		FJ_Debug_Print_Err( "BRESPC_0=0x%x BRESPC_1=0x%x BRESPC_2=0x%x BRESPC_3=0x%x BRESPC_4=0x%x BRESPC_5=0x%x BRESPC_6=0x%x\n",
								res.w_resp_c[ 0 ],
								res.w_resp_c[ 1 ],
								res.w_resp_c[ 2 ],
								res.w_resp_c[ 3 ],
								res.w_resp_c[ 4 ],
								res.w_resp_c[ 5 ],
								res.w_resp_c[ 6 ]
							);
	}
}

static INT32 ip_shdr_set_iip( IP_SHDR_SET_PRM* setPrm )
{
	return D_IP_SHDR_OK;
}

static INT32 ip_shdr_set_me( IP_SHDR_SET_PRM* setPrm )
{
	return D_IP_SHDR_OK;
}

static INT32 ip_shdr_set_shdr( IP_SHDR_SET_PRM* setPrm )
{
	INT32	ercd;

	ercd = Im_SHDR_Set_Pre( (T_IM_SHDR_PRE*)&gIp_Shdr_Pre_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Pre() error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Set_Fmd( (T_IM_SHDR_FMD*)&gIp_Shdr_Fmd_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Fmd() error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Set_PreMask( (T_IM_SHDR_PMSK*)&gIp_Shdr_Pmsk_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_PreMask() error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Set_Spnr( (T_IM_SHDR_SPNR*)&gIp_Shdr_Spnr_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Spnr() error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Set_Mask( (T_IM_SHDR_MSK*)&gIp_Shdr_Msk_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Mask() error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Set_Blend( (T_IM_SHDR_BLD*)&gIp_Shdr_Bld_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Blend() error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Set_Post( (T_IM_SHDR_POST*)&gIp_Shdr_Post_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Post() error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Set_Smc( (T_IM_SHDR_SMC*)&gIp_Shdr_Smc_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Smc() error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Set_Axi( (T_IM_SHDR_AXI*)&gIp_Shdr_Axi_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Axi() error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Set_Degamma( (USHORT*)&gIp_Shdr_Degamma_Prm[ 0 ][ 0 ],
								(USHORT*)&gIp_Shdr_Degamma_Prm[ 1 ][ 0 ],
								(USHORT*)&gIp_Shdr_Degamma_Prm[ 2 ][ 0 ],
								0,
								D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Degamma() error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	return D_IP_SHDR_OK;
}

static INT32 ip_shdr_start_iip( FJ_STILL_IP_IMAGE* imgSrc, FJ_STILL_IP_IMAGE* imgSrcHalf, FJ_STILL_IP_IMAGE* imgDst, FJ_STILL_IP_IMAGE* imgDstHalf )
{
	return D_IP_SHDR_OK;
}

static INT32 ip_shdr_start_me( FJ_STILL_IP_IMAGE* imgSrcHalf0, FJ_STILL_IP_IMAGE* imgSrcHalf1, FJ_STILL_IP_IMAGE* imgMe )
{
	return D_IP_SHDR_OK;
}

static INT32 ip_shdr_start_shdr_1st( FJ_STILL_IP_IMAGE* imgSrc0, FJ_STILL_IP_IMAGE* imgSrc1, FJ_STILL_IP_IMAGE* imgSrc2, FJ_STILL_IP_IMAGE* imgMe, FJ_STILL_IP_IMAGE* imgDstYuv16 )
{
	INT32	ercd;
	UINT32	wait_factor_result = 0;

	// SHDROFRM.
	gIp_Shdr_Ctrl_Prm.image_set.out_ctrl	= D_IM_SHDR_OUTCTRL_YCC;
	gIp_Shdr_Ctrl_Prm.image_set.out_sel		= D_IM_SHDR_OUTSEL_YCC_16;
	gIp_Shdr_Ctrl_Prm.image_set.out_sel_en	= D_IM_SHDR_ENABLE;
	// SDRAM for read (Standard).
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.b_y		= (ULONG)imgSrc2->imgInfo[ 0 ].pImgAdrs;		// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.b_c		= (ULONG)imgSrc2->imgInfo[ 1 ].pImgAdrs;		// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.b_y	= imgSrc2->imgInfo[ 0 ].gImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.b_c	= imgSrc2->imgInfo[ 1 ].gImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_hsize.b		= imgSrc2->imgInfo[ 0 ].wImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_vsize.b		= imgSrc2->imgInfo[ 0 ].hImgSize;				// Src image.
	// SDRAM for read (SMC_in standard).
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.c_y		= (ULONG)imgSrc0->imgInfo[ 0 ].pImgAdrs;		// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.c_c		= (ULONG)imgSrc0->imgInfo[ 1 ].pImgAdrs;		// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.c_y	= imgSrc0->imgInfo[ 0 ].gImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.c_c	= imgSrc0->imgInfo[ 1 ].gImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_hsize.c		= imgSrc0->imgInfo[ 0 ].wImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_vsize.c		= imgSrc0->imgInfo[ 0 ].hImgSize;				// Src image.
	// SDRAM for read(SMC_in reference).
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.a_y		= (ULONG)imgSrc1->imgInfo[ 0 ].pImgAdrs;		// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.a_c		= (ULONG)imgSrc1->imgInfo[ 1 ].pImgAdrs;		// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.a_y	= imgSrc1->imgInfo[ 0 ].gImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.a_c	= imgSrc1->imgInfo[ 1 ].gImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_hsize.a		= imgSrc1->imgInfo[ 0 ].wImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_vsize.a		= imgSrc1->imgInfo[ 0 ].hImgSize;				// Src image.
	// SDRAM for read (Motion vector).
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.d_y		= (ULONG)imgMe->imgInfo[ 0 ].pImgAdrs;			// Motion vector.
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.d_c		= (ULONG)imgMe->imgInfo[ 1 ].pImgAdrs;			// Motion vector.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.d_y	= imgMe->imgInfo[ 0 ].gImgSize;					// Motion vector.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.d_c	= imgMe->imgInfo[ 1 ].gImgSize;					// Motion vector.
	gIp_Shdr_Ctrl_Prm.sdram.r_hsize.d		= imgMe->imgInfo[ 0 ].wImgSize;					// Motion vector.
	gIp_Shdr_Ctrl_Prm.sdram.r_vsize.d		= imgMe->imgInfo[ 0 ].hImgSize;					// Motion vector.
	// SDRAM for write.
	gIp_Shdr_Ctrl_Prm.sdram.w_addr.g_r_y	= (ULONG)imgDstYuv16->imgInfo[ 0 ].pImgAdrs;	// YUV16.
	gIp_Shdr_Ctrl_Prm.sdram.w_addr.g_g_c	= (ULONG)imgDstYuv16->imgInfo[ 1 ].pImgAdrs;;	// YUV16.
	gIp_Shdr_Ctrl_Prm.sdram.w_ghsize.g_r_y	= imgDstYuv16->imgInfo[ 0 ].gImgSize;			// YUV16.
	gIp_Shdr_Ctrl_Prm.sdram.w_ghsize.g_g_c	= imgDstYuv16->imgInfo[ 1 ].gImgSize;			// YUV16.

	gIp_Shdr_Ctrl_Prm.pCallBack = (T_IM_SHDR_CALLBACK)ip_shdr_int_handler;

	// SHDR macro driver set/start/end.
	ercd = Im_SHDR_Ctrl_Common( (T_IM_SHDR_CTRL*)&gIp_Shdr_Ctrl_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Ctrl_Common(1st) error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Set_Resize( (T_IM_SHDR_RESIZE*)&gIp_Shdr_Resize_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Resize(1st) error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Start();
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Start(1st) error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Wait_End( &wait_factor_result, D_IP_SHDR_WAIT_TIME );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Wait_End(1st) error. ercd[0x%08x],  wait_factor_result = 0x%x\n", ercd, wait_factor_result );
		return D_IP_SHDR_NG;
	}

	return D_IP_SHDR_OK;
}

static INT32 ip_shdr_start_shdr_2nd( FJ_STILL_IP_IMAGE* imgSrc0, FJ_STILL_IP_IMAGE* imgSrc1, FJ_STILL_IP_IMAGE* imgSrc2, FJ_STILL_IP_IMAGE* imgMe, FJ_STILL_IP_IMAGE* imgDstRgb16 )
{
	INT32	ercd;
	UINT32	wait_factor_result = 0;

	// SHDROFRM.
	gIp_Shdr_Ctrl_Prm.image_set.out_ctrl	= D_IM_SHDR_OUTCTRL_RGB;
	gIp_Shdr_Ctrl_Prm.image_set.out_sel		= D_IM_SHDR_OUTSEL_RGB_16;
	gIp_Shdr_Ctrl_Prm.image_set.out_sel_en	= D_IM_SHDR_ENABLE;
	// SDRAM for read (Standard).
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.b_y		= (ULONG)imgSrc2->imgInfo[ 0 ].pImgAdrs;		// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.b_c		= (ULONG)imgSrc2->imgInfo[ 1 ].pImgAdrs;		// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.b_y	= imgSrc2->imgInfo[ 0 ].gImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.b_c	= imgSrc2->imgInfo[ 1 ].gImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_hsize.b		= imgSrc2->imgInfo[ 0 ].wImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_vsize.b		= imgSrc2->imgInfo[ 0 ].hImgSize;				// Src image.
	// SDRAM for read (SMC_in standard).
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.c_y		= (ULONG)imgSrc0->imgInfo[ 0 ].pImgAdrs;		// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.c_c		= (ULONG)imgSrc0->imgInfo[ 1 ].pImgAdrs;		// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.c_y	= imgSrc0->imgInfo[ 0 ].gImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.c_c	= imgSrc0->imgInfo[ 1 ].gImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_hsize.c		= imgSrc0->imgInfo[ 0 ].wImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_vsize.c		= imgSrc0->imgInfo[ 0 ].hImgSize;				// Src image.
	// SDRAM for read(SMC_in reference).
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.a_y		= (ULONG)imgSrc1->imgInfo[ 0 ].pImgAdrs;		// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.a_c		= (ULONG)imgSrc1->imgInfo[ 1 ].pImgAdrs;		// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.a_y	= imgSrc1->imgInfo[ 0 ].gImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.a_c	= imgSrc1->imgInfo[ 1 ].gImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_hsize.a		= imgSrc1->imgInfo[ 0 ].wImgSize;				// Src image.
	gIp_Shdr_Ctrl_Prm.sdram.r_vsize.a		= imgSrc1->imgInfo[ 0 ].hImgSize;				// Src image.
	// SDRAM for read (Motion vector).
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.d_y		= (ULONG)imgMe->imgInfo[ 0 ].pImgAdrs;			// Motion vector.
	gIp_Shdr_Ctrl_Prm.sdram.r_addr.d_c		= (ULONG)imgMe->imgInfo[ 1 ].pImgAdrs;			// Motion vector.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.d_y	= imgMe->imgInfo[ 0 ].gImgSize;					// Motion vector.
	gIp_Shdr_Ctrl_Prm.sdram.r_ghsize.d_c	= imgMe->imgInfo[ 1 ].gImgSize;					// Motion vector.
	gIp_Shdr_Ctrl_Prm.sdram.r_hsize.d		= imgMe->imgInfo[ 0 ].wImgSize;					// Motion vector.
	gIp_Shdr_Ctrl_Prm.sdram.r_vsize.d		= imgMe->imgInfo[ 0 ].hImgSize;					// Motion vector.
	// SDRAM for write.
	gIp_Shdr_Ctrl_Prm.sdram.w_addr.g_r_y	= (ULONG)imgDstRgb16->imgInfo[ 0 ].pImgAdrs;	// RGB16.
	gIp_Shdr_Ctrl_Prm.sdram.w_addr.g_g_c	= (ULONG)imgDstRgb16->imgInfo[ 1 ].pImgAdrs;	// RGB16.
	gIp_Shdr_Ctrl_Prm.sdram.w_addr.g_b		= (ULONG)imgDstRgb16->imgInfo[ 2 ].pImgAdrs;	// RGB16.
	gIp_Shdr_Ctrl_Prm.sdram.w_ghsize.g_r_y	= imgDstRgb16->imgInfo[ 0 ].gImgSize;			// RGB16.
	gIp_Shdr_Ctrl_Prm.sdram.w_ghsize.g_g_c	= imgDstRgb16->imgInfo[ 1 ].gImgSize;			// RGB16.
	gIp_Shdr_Ctrl_Prm.sdram.w_ghsize.g_b	= imgDstRgb16->imgInfo[ 2 ].gImgSize;			// RGB16.

	gIp_Shdr_Ctrl_Prm.pCallBack = (T_IM_SHDR_CALLBACK)ip_shdr_int_handler;

	// SHDR macro driver set/start/end.
	ercd = Im_SHDR_Ctrl_Common( (T_IM_SHDR_CTRL*)&gIp_Shdr_Ctrl_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Ctrl_Common(2nd) error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Set_Resize( (T_IM_SHDR_RESIZE*)&gIp_Shdr_Resize_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Resize(2nd) error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Start();
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Start(2nd) error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}

	ercd = Im_SHDR_Wait_End( &wait_factor_result, D_IP_SHDR_WAIT_TIME );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Wait_End(2nd) error. ercd[0x%08x],  wait_factor_result = 0x%x\n", ercd, wait_factor_result );
		return D_IP_SHDR_NG;
	}

	return D_IP_SHDR_OK;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
Open SHDR.
@return		If the function succeeds, the return value is D_IP_SHDR_OK.
*/
INT32 IP_SHDR_Open( VOID )
{
	INT32	ercd;

	OS_User_Wai_Sem( SID_IP_SHDR );

	// IIP macro driver open.
#ifndef DEBUG_ON_PC
#endif

	// ME macro driver open.
#ifndef DEBUG_ON_PC
#endif

	// SHDR macro driver open.
#ifndef DEBUG_ON_PC
	ercd = Im_SHDR_Open( D_IP_SHDR_WAIT_TIME );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "IP_SHDR_Open() shdr open error. ercd[0x%08x]\n", ercd );
		OS_User_Sig_Sem( SID_IP_SHDR );
		return D_IP_SHDR_NG;
	}
#endif

	gIp_Shdr_Open = TRUE;

	OS_User_Sig_Sem( SID_IP_SHDR );

	return D_IP_SHDR_OK;
}

/**
Initialize SHDR.
@param[in]	iniPrm	: SHDR initialize parameter.
@return		If the function succeeds, the return value is D_IP_SHDR_OK.
*/
INT32 IP_SHDR_Init( IP_SHDR_INIT_PRM* iniPrm  )
{
	INT32	ercd;

	if ( !gIp_Shdr_Open ) {
		return D_IP_SHDR_OK;
	}

	OS_User_Wai_Sem( SID_IP_SHDR );

	gIp_Shdr_Init_Prm = *iniPrm;

	// Initialization.
	memset( (void*)&gIp_Shdr_Set_Prm,   0, sizeof( gIp_Shdr_Set_Prm ) );
	memset( (void*)&gIp_Shdr_Start_Prm, 0, sizeof( gIp_Shdr_Start_Prm ) );

	// IIP macro driver init.
#ifndef DEBUG_ON_PC
#endif

	// ME macro driver init.
#ifndef DEBUG_ON_PC
#endif

	// SHDR macro driver init.
#ifndef DEBUG_ON_PC
	ercd = Im_SHDR_Init( FALSE );
	if( ercd != D_DDIM_OK ) {
		OS_User_Sig_Sem( SID_IP_SHDR );
		FJ_Debug_Print_Err( "IP_SHDR_Init() shdr init error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}
#endif

	OS_User_Sig_Sem( SID_IP_SHDR );

	return D_IP_SHDR_OK;
}

/**
Set SHDR.
@param[in]	setPrm	: SHDR setting parameter.
@return		If the function succeeds, the return value is D_IP_SHDR_OK.
*/
INT32 IP_SHDR_Set( IP_SHDR_SET_PRM* setPrm )
{
	INT32	ercd;

	if ( !gIp_Shdr_Open ) {
		return D_IP_SHDR_OK;
	}

	OS_User_Wai_Sem( SID_IP_SHDR );

	gIp_Shdr_Set_Prm = *setPrm;

#ifndef DEBUG_ON_PC
	ercd = ip_shdr_set_iip( setPrm );
	if ( ercd != D_IP_SHDR_OK ) {
		OS_User_Sig_Sem( SID_IP_SHDR );
		return D_IP_SHDR_NG;
	}
#else
	OS_User_Dly_Tsk(1);
#endif

#ifndef DEBUG_ON_PC
	ercd = ip_shdr_set_me( setPrm );
	if ( ercd != D_IP_SHDR_OK ) {
		OS_User_Sig_Sem( SID_IP_SHDR );
		return D_IP_SHDR_NG;
	}
#else
	OS_User_Dly_Tsk(1);
#endif

#ifndef DEBUG_ON_PC
	ercd = ip_shdr_set_shdr( setPrm );
	if ( ercd != D_IP_SHDR_OK ) {
		OS_User_Sig_Sem( SID_IP_SHDR );
		return D_IP_SHDR_NG;
	}
#else
	OS_User_Dly_Tsk(1);
#endif

	OS_User_Sig_Sem( SID_IP_SHDR );

	return D_IP_SHDR_OK;
}

/**
Start SHDR.
@param[in]	startPrm	: SHDR start parameter.
@return		If the function succeeds, the return value is D_IP_SHDR_OK.
*/
INT32 IP_SHDR_Start( IP_SHDR_START_PRM* startPrm )
{
	INT32				ercd;
	FJ_STILL_IP_IMAGE	imgMe;
	FJ_STILL_IP_IMAGE	imgDst[ 2 ];
	FJ_STILL_IP_IMAGE	imgYuv16;

	if ( !gIp_Shdr_Open ) {
		return D_IP_SHDR_OK;
	}

	memset( (void*)&imgMe,       0, sizeof( FJ_STILL_IP_IMAGE ) );
	memset( (void*)&imgDst[ 0 ], 0, sizeof( FJ_STILL_IP_IMAGE ) * 2 );
	memset( (void*)&imgYuv16,    0, sizeof( FJ_STILL_IP_IMAGE ) );

	if ( gIp_Shdr_Init_Prm.heapAddr != 0 ) {
		imgDst[ 0 ].imgInfo[ 0 ].pImgAdrs	= (VOID*)gIp_Shdr_Init_Prm.heapAddr;
		imgDst[ 0 ].imgInfo[ 1 ].pImgAdrs	= (VOID*)( (UINT32)imgDst[ 0 ].imgInfo[ 0 ].pImgAdrs
												+ ( startPrm->imgSrcL[ 0 ].imgInfo[ 0 ].gImgSize * startPrm->imgSrcL[ 0 ].imgInfo[ 0 ].hImgSize ) );
		imgDst[ 1 ].imgInfo[ 0 ].pImgAdrs	= (VOID*)( (UINT32)imgDst[ 0 ].imgInfo[ 1 ].pImgAdrs
												+ ( startPrm->imgSrcL[ 0 ].imgInfo[ 1 ].gImgSize * startPrm->imgSrcL[ 0 ].imgInfo[ 1 ].hImgSize ) * 3 / 2 );
		imgDst[ 1 ].imgInfo[ 1 ].pImgAdrs	= (VOID*)( (UINT32)imgDst[ 1 ].imgInfo[ 0 ].pImgAdrs
												+ ( startPrm->imgSrcL[ 1 ].imgInfo[ 0 ].gImgSize * startPrm->imgSrcL[ 1 ].imgInfo[ 0 ].hImgSize ) );
		imgYuv16.imgInfo[ 0 ].pImgAdrs		= (VOID*)( (UINT32)imgDst[ 1 ].imgInfo[ 1 ].pImgAdrs
												+ ( startPrm->imgSrcL[ 1 ].imgInfo[ 1 ].gImgSize * startPrm->imgSrcL[ 1 ].imgInfo[ 1 ].hImgSize ) * 3 / 2 );
		imgYuv16.imgInfo[ 1 ].pImgAdrs		= (VOID*)( (UINT32)imgYuv16.imgInfo[ 0 ].pImgAdrs
												+ ( startPrm->imgSrcL[ 0 ].imgInfo[ 0 ].gImgSize * startPrm->imgSrcL[ 0 ].imgInfo[ 0 ].hImgSize ) * 2 );
		imgMe.imgInfo[ 0 ].pImgAdrs			= (VOID*)( (UINT32)imgYuv16.imgInfo[ 1 ].pImgAdrs
												+ ( startPrm->imgSrcL[ 0 ].imgInfo[ 1 ].gImgSize * startPrm->imgSrcL[ 0 ].imgInfo[ 1 ].hImgSize ) * 3 );
	}
	else {
		FJ_Debug_Print_Err( "IP_SHDR_Start() heapAddr is null.\n" );
		return D_IP_SHDR_OK;
	}

	OS_User_Wai_Sem( SID_IP_SHDR );

	gIp_Shdr_Start_Prm = *startPrm;

#ifndef DEBUG_ON_PC
	ercd = ip_shdr_start_iip(	&startPrm->imgSrcL[ 0 ],		// [In]  YUV8     (Long exposure).
								&startPrm->imgSrcL[ 1 ],		// [In]  YUV8 1/4 (Long exposure).
								&imgDst[ 0 ],					// [Out] YUV8     (Long exposure : Brightness compensation).
								&imgDst[ 1 ] );					// [Out] Y8   1/4 (Long exposure : Brightness compensation).
	if ( ercd != D_IP_SHDR_OK ) {
		OS_User_Sig_Sem( SID_IP_SHDR );
		return D_IP_SHDR_NG;
	}
#else
	OS_User_Dly_Tsk(10);
#endif

#ifndef DEBUG_ON_PC
	ercd = ip_shdr_start_me(	&startPrm->imgSrcM[ 1 ],		// [In] Y8 1/4 (Middle exposure).
								&imgDst[ 1 ],					// [In] Y8 1/4 (Long   exposure : Brightness compensation).
								&imgMe );						// [Out] Motion vector.
	if ( ercd != D_IP_SHDR_OK ) {
		OS_User_Sig_Sem( SID_IP_SHDR );
		return D_IP_SHDR_NG;
	}
#else
	OS_User_Dly_Tsk(10);
#endif

#ifndef DEBUG_ON_PC
	ercd = ip_shdr_start_shdr_1st(	&startPrm->imgSrcM[ 0 ],	// [In] Standard		:YUV8 (Middle exposure).
									&imgDst[ 0 ],				// [In] SMC in standard	:YUV8 (Middle exposure : Brightness compensation).
									&startPrm->imgSrcL[ 0 ],	// [In] SMC in reference:YUV8 (Long exposure).
									&imgMe,						// [In] Motion vector.
									&imgYuv16 );				// [Out] YUV16 (1st blending image).
	if ( ercd != D_IP_SHDR_OK ) {
		OS_User_Sig_Sem( SID_IP_SHDR );
		return D_IP_SHDR_NG;
	}
#else
	OS_User_Dly_Tsk(10);
#endif

#ifndef DEBUG_ON_PC
	ercd = ip_shdr_start_iip(	&startPrm->imgSrcM[ 0 ],		// [In]  YUV8     (Middle exposure).
								&startPrm->imgSrcS[ 1 ],		// [In]  YUV8 1/4 (Short  exposure).
								&imgDst[ 0 ],					// [Out] YUV8     (Middle exposure : Brightness compensation).
								&imgDst[ 1 ] );					// [Out] Y8   1/4 (Short  exposure : Brightness compensation).
	if ( ercd != D_IP_SHDR_OK ) {
		OS_User_Sig_Sem( SID_IP_SHDR );
		return D_IP_SHDR_NG;
	}
#else
	OS_User_Dly_Tsk(10);
#endif

#ifndef DEBUG_ON_PC
	ercd = ip_shdr_start_me(	&startPrm->imgSrcM[ 1 ],		// [In] Y8 1/4 (Middle exposure).
								&imgDst[ 1 ],					// [In] Y8 1/4 (Short  exposure : Brightness compensation).
								&imgMe );						// [Out] Motion vector.
	if ( ercd != D_IP_SHDR_OK ) {
		OS_User_Sig_Sem( SID_IP_SHDR );
		return D_IP_SHDR_NG;
	}
#else
	OS_User_Dly_Tsk(10);
#endif

#ifndef DEBUG_ON_PC
	ercd = ip_shdr_start_shdr_2nd(	&imgYuv16,					// [In] YUV16 (1st blending image).
									&imgDst[ 0 ],				// [In] YUV8  (Middle exposure : Brightness compensation).
									&startPrm->imgSrcS[ 0 ],	// [In] YUV8  (Shprt  exposure).
									&imgMe,						// [In] Motion vector.
									&startPrm->imgDst );		// [Out] RGB16 (SHDR output image).
	if ( ercd != D_IP_SHDR_OK ) {
		OS_User_Sig_Sem( SID_IP_SHDR );
		return D_IP_SHDR_NG;
	}
#else
	OS_User_Dly_Tsk(10);
#endif

	OS_User_Sig_Sem( SID_IP_SHDR );

	return D_IP_SHDR_OK;
}

/**
Close SHDR.
@return		If the function succeeds, the return value is D_IP_SHDR_OK.
*/
INT32 IP_SHDR_Close( VOID )
{
	INT32	ercd;

	if ( !gIp_Shdr_Open ) {
		return D_IP_SHDR_OK;
	}

	// IIP macro driver close.
#ifndef DEBUG_ON_PC
#endif

	// ME macro driver close.
#ifndef DEBUG_ON_PC
#endif

	// SHDR macro driver close.
#ifndef DEBUG_ON_PC
	ercd = Im_SHDR_Close();
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "IP_SHDR_Close() Im_SHDR_Close error. ercd[0x%08x]\n", ercd );
		return D_IP_SHDR_NG;
	}
#endif

	gIp_Shdr_Open		= FALSE;

	return D_IP_SHDR_OK;
}

/**
Get SHDR version.
@param[out]		str	: version
*/
VOID IP_SHDR_Get_Version( CHAR* str )
{
	const CHAR ver[ 3 ] = { '0', '.', '1' };

	strcpy( (char*)str, (const char*)ver );

	return;
}

