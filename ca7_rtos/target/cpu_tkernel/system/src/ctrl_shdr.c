/**
 * @file		ctrl_shdr.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "ctrl_shdr.h"

#include "im_shdr.h"
#include "im_me.h"

#include "system.h"
#include "fj_still.h"
#include "still_utility.h"
// delete basefw\fj\imgproc\still
//#include "still_proc_subr.h"

#include "ipc_if.h"
#include "ipc_gmd.h"

#include "ip_ahip.h"
#include "ip_ge.h"
#include "ip_eis.h"

#include "iqpara_api.h"
#include "l1l2cache.h"

#include <string.h>


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
//#define DEBUG_DUMP_SHDR_REG
//#define DEBUG_SAVE_IMG
//#define DEBUG_SRC_REF_MBA
//#define DEBUG_DUMP_ME_RESULT
//#define DEBUG_3DNR_EFFECT
//#define DEBUG_3DNR_GMD

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define ipu_3dnr_rounddown_2( val )		(((UINT32)(val))   & 0xFFFFFFFE)
#define ipu_3dnr_calc_pit( src, dst )	((UINT32)((UINT64)0x4000 * ((src) -1) / ((dst) -1)))

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	UCHAR	en;
	UCHAR	force_fmd_off;
	UCHAR	force_mc_off;
	UCHAR	force_chg_blend;
	UCHAR	force_chg_me_shift;
	UCHAR	force_chg_shdr_shift;
	UCHAR	force_chg_me_out;
	
	UCHAR	force_blend_ratio; // 256 = 100% cur, 0 = 100% prv
	CHAR	force_me_out_x;
	CHAR	force_me_out_y;
	CHAR	force_shift_me_x;
	CHAR	force_shift_me_y;
	CHAR	force_shift_shdr_x;
	CHAR	force_shift_shdr_y;
} T_DEBUG_3DNR;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_DEBUG_3DNR		g_debug_3dnr = {0};

static volatile T_IM_ME_CTRL	gIpu_3dnr_Me_Ctrl_Prm = {
	.apbbrg = {
		.pp_enable[ 0 ]			= D_IM_ME_ENABLE,
		.pp_enable[ 1 ]			= D_IM_ME_ENABLE,
		.pp_enable[ 2 ]			= D_IM_ME_ENABLE,
		.int_mode				= D_IM_ME_INTMODE_AFTER_CYCLE,
		.interval				= 0x100,
	},
	.pme[ 0 ] = {
		.input_mode				= D_IM_ME_INPUTMODE_PROGRESSIVE,
		.ref_txb				= D_IM_ME_REFTXB_TOP,
		.out_limit				= D_IM_ME_OUTLIMIT_NOLIMIT0,
		.bank_num				= D_IM_ME_BANKNUM_1,
		.m1org_mode[ 0 ] = {
			.direction			= D_IM_ME_M1ORG_DIR_FWD,
			.parity				= D_IM_ME_M1ORG_PARI_TOP,
			.read_mode			= D_IM_ME_M1ORG_READ_FIXED,
			.sad0_mode			= D_IM_ME_M1ORG_SAD_WRITE,
			.sad1_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.vec_mode			= D_IM_ME_M1ORG_VECM_WRITE0,
			.vec_select			= D_IM_ME_M1ORG_VECS_SAVE,
			.pic_type			= D_IM_ME_M1ORG_PICT_P,
		},
		.m1org_mode[ 1 ] = {
			.direction			= D_IM_ME_M1ORG_DIR_FWD,
			.parity				= D_IM_ME_M1ORG_PARI_TOP,
			.read_mode			= D_IM_ME_M1ORG_READ_FIXED,
			.sad0_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.sad1_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.vec_mode			= D_IM_ME_M1ORG_VECM_WRITE0,
			.vec_select			= D_IM_ME_M1ORG_VECS_SAVE,
			.pic_type			= D_IM_ME_M1ORG_PICT_P,
		},
		.m1org_mode[ 2 ] = {
			.direction			= D_IM_ME_M1ORG_DIR_FWD,
			.parity				= D_IM_ME_M1ORG_PARI_TOP,
			.read_mode			= D_IM_ME_M1ORG_READ_FIXED,
			.sad0_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.sad1_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.vec_mode			= D_IM_ME_M1ORG_VECM_WRITE0,
			.vec_select			= D_IM_ME_M1ORG_VECS_SAVE,
			.pic_type			= D_IM_ME_M1ORG_PICT_P,
		},
		.m1org_mode[ 3 ] = {
			.direction			= D_IM_ME_M1ORG_DIR_FWD,
			.parity				= D_IM_ME_M1ORG_PARI_TOP,
			.read_mode			= D_IM_ME_M1ORG_READ_FIXED,
			.sad0_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.sad1_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.vec_mode			= D_IM_ME_M1ORG_VECM_WRITE0,
			.vec_select			= D_IM_ME_M1ORG_VECS_SAVE,
			.pic_type			= D_IM_ME_M1ORG_PICT_P,
		},
		.m1correct = {
			.front_ky			= 3,
			.back_ky			= 0,
			.front_kx			= 3,
			.back_kx			= 0,
			.mode				= 1,
			.front_h			= 0,
			.front_v			= 0,
			.back_h				= 0,
			.back_v				= 0,
		},
		.m1mc = {
			.p_pic				= 1,
			.b_pic				= 0,
		},
		.m1splt = {
			.mode				= D_IM_ME_SPLITMODE_NORMAL,
			.th_h				= 8,
			.th_v				= 8,
		},
		.m1skip					= D_IM_ME_OFF,
		.m1sad = {
			.upper				= 0xFFF,
			.lower				= 0,
		},
		.m1adjust = {
			.wait_cycle			= 0,
			.wait_onoff			= D_IM_ME_OFF,
		},
		.m1pause = {
			.pause_line			= 0,
			.pause_onoff		= D_IM_ME_OFF,
		},
		.m1limit = {
			.v_plus				= 12,
			.v_minus			= 12,
			.h_plus				= 16,
			.h_minus			= 16,
		},
		.core_mode = {
			.line_interval		= D_IM_ME_LINEINTV_2QMB,
			.init_pos_y			= 0,
		},
	},
	.pme[ 1 ] = {
		.input_mode				= D_IM_ME_INPUTMODE_PROGRESSIVE,
		.ref_txb				= D_IM_ME_REFTXB_TOP,
		.out_limit				= D_IM_ME_OUTLIMIT_NOLIMIT0,
		.bank_num				= D_IM_ME_BANKNUM_1,
		.m1org_mode[ 0 ] = {
			.direction			= D_IM_ME_M1ORG_DIR_FWD,
			.parity				= D_IM_ME_M1ORG_PARI_TOP,
			.read_mode			= D_IM_ME_M1ORG_READ_FIXED,
			.sad0_mode			= D_IM_ME_M1ORG_SAD_WRITE,
			.sad1_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.vec_mode			= D_IM_ME_M1ORG_VECM_WRITE0,
			.vec_select			= D_IM_ME_M1ORG_VECS_SAVE,
			.pic_type			= D_IM_ME_M1ORG_PICT_P,
		},
		.m1org_mode[ 1 ] = {
			.direction			= D_IM_ME_M1ORG_DIR_FWD,
			.parity				= D_IM_ME_M1ORG_PARI_TOP,
			.read_mode			= D_IM_ME_M1ORG_READ_FIXED,
			.sad0_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.sad1_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.vec_mode			= D_IM_ME_M1ORG_VECM_WRITE0,
			.vec_select			= D_IM_ME_M1ORG_VECS_SAVE,
			.pic_type			= D_IM_ME_M1ORG_PICT_P,
		},
		.m1org_mode[ 2 ] = {
			.direction			= D_IM_ME_M1ORG_DIR_FWD,
			.parity				= D_IM_ME_M1ORG_PARI_TOP,
			.read_mode			= D_IM_ME_M1ORG_READ_FIXED,
			.sad0_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.sad1_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.vec_mode			= D_IM_ME_M1ORG_VECM_WRITE0,
			.vec_select			= D_IM_ME_M1ORG_VECS_SAVE,
			.pic_type			= D_IM_ME_M1ORG_PICT_P,
		},
		.m1org_mode[ 3 ] = {
			.direction			= D_IM_ME_M1ORG_DIR_FWD,
			.parity				= D_IM_ME_M1ORG_PARI_TOP,
			.read_mode			= D_IM_ME_M1ORG_READ_FIXED,
			.sad0_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.sad1_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.vec_mode			= D_IM_ME_M1ORG_VECM_WRITE0,
			.vec_select			= D_IM_ME_M1ORG_VECS_SAVE,
			.pic_type			= D_IM_ME_M1ORG_PICT_P,
		},
		.m1correct = {
			.front_ky			= 3,
			.back_ky			= 0,
			.front_kx			= 3,
			.back_kx			= 0,
			.mode				= 1,
			.front_h			= 0,
			.front_v			= 0,
			.back_h				= 0,
			.back_v				= 0,
		},
		.m1mc = {
			.p_pic				= 1,
			.b_pic				= 0,
		},
		.m1splt = {
			.mode				= D_IM_ME_SPLITMODE_NORMAL,
			.th_h				= 8,
			.th_v				= 8,
		},
		.m1skip					= D_IM_ME_OFF,
		.m1sad = {
			.upper				= 0xFFF,
			.lower				= 0,
		},
		.m1adjust = {
			.wait_cycle			= 0,
			.wait_onoff			= D_IM_ME_OFF,
		},
		.m1pause = {
			.pause_line			= 0,
			.pause_onoff		= D_IM_ME_OFF,
		},
		.m1limit = {
			.v_plus				= 12,
			.v_minus			= 12,
			.h_plus				= 16,
			.h_minus			= 16,
		},
		.core_mode = {
			.line_interval		= D_IM_ME_LINEINTV_2QMB,
			.init_pos_y			= 1,
		},
	},
	.pme[ 2 ] = {
		.input_mode				= D_IM_ME_INPUTMODE_PROGRESSIVE,
		.ref_txb				= D_IM_ME_REFTXB_TOP,
		.out_limit				= D_IM_ME_OUTLIMIT_NOLIMIT0,
		.bank_num				= D_IM_ME_BANKNUM_1,
		.m1org_mode[ 0 ] = {
			.direction			= D_IM_ME_M1ORG_DIR_FWD,
			.parity				= D_IM_ME_M1ORG_PARI_TOP,
			.read_mode			= D_IM_ME_M1ORG_READ_FIXED,
			.sad0_mode			= D_IM_ME_M1ORG_SAD_WRITE,
			.sad1_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.vec_mode			= D_IM_ME_M1ORG_VECM_WRITE0,
			.vec_select			= D_IM_ME_M1ORG_VECS_SAVE,
			.pic_type			= D_IM_ME_M1ORG_PICT_P,
		},
		.m1org_mode[ 1 ] = {
			.direction			= D_IM_ME_M1ORG_DIR_FWD,
			.parity				= D_IM_ME_M1ORG_PARI_TOP,
			.read_mode			= D_IM_ME_M1ORG_READ_FIXED,
			.sad0_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.sad1_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.vec_mode			= D_IM_ME_M1ORG_VECM_WRITE0,
			.vec_select			= D_IM_ME_M1ORG_VECS_SAVE,
			.pic_type			= D_IM_ME_M1ORG_PICT_P,
		},
		.m1org_mode[ 2 ] = {
			.direction			= D_IM_ME_M1ORG_DIR_FWD,
			.parity				= D_IM_ME_M1ORG_PARI_TOP,
			.read_mode			= D_IM_ME_M1ORG_READ_FIXED,
			.sad0_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.sad1_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.vec_mode			= D_IM_ME_M1ORG_VECM_WRITE0,
			.vec_select			= D_IM_ME_M1ORG_VECS_SAVE,
			.pic_type			= D_IM_ME_M1ORG_PICT_P,
		},
		.m1org_mode[ 3 ] = {
			.direction			= D_IM_ME_M1ORG_DIR_FWD,
			.parity				= D_IM_ME_M1ORG_PARI_TOP,
			.read_mode			= D_IM_ME_M1ORG_READ_FIXED,
			.sad0_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.sad1_mode			= D_IM_ME_M1ORG_SAD_SAVE,
			.vec_mode			= D_IM_ME_M1ORG_VECM_WRITE0,
			.vec_select			= D_IM_ME_M1ORG_VECS_SAVE,
			.pic_type			= D_IM_ME_M1ORG_PICT_P,
		},
		.m1correct = {
			.front_ky			= 3,
			.back_ky			= 0,
			.front_kx			= 3,
			.back_kx			= 0,
			.mode				= 1,
			.front_h			= 0,
			.front_v			= 0,
			.back_h				= 0,
			.back_v				= 0,
		},
		.m1mc = {
			.p_pic				= 1,
			.b_pic				= 0,
		},
		.m1splt = {
			.mode				= D_IM_ME_SPLITMODE_NORMAL,
			.th_h				= 8,
			.th_v				= 8,
		},
		.m1skip					= D_IM_ME_OFF,
		.m1sad = {
			.upper				= 0xFFF,
			.lower				= 0,
		},
		.m1adjust = {
			.wait_cycle			= 0,
			.wait_onoff			= D_IM_ME_OFF,
		},
		.m1pause = {
			.pause_line			= 0,
			.pause_onoff		= D_IM_ME_OFF,
		},
		.m1limit = {
			.v_plus				= 12,
			.v_minus			= 12,
			.h_plus				= 16,
			.h_minus			= 16,
		},
		.core_mode = {
			.line_interval		= D_IM_ME_LINEINTV_2QMB,
			.init_pos_y			= 2,
		},
	},
	.rmcif = {
		.pme_num				= 2,
		.abt_type				= D_IM_ME_ABT_FIXED,
		.bank_tbl = {
			.fwd_pvec			= 0,
			.top				= 0,
			.fwd_8pel			= 0,
		},
		.axi_if = {
			.r_cache_type		= 0,
			.r_protect_type		= 0,
			.w_cache_type		= 0,
			.w_protect_type		= 0,
		},
	},
	.pCallBack					= NULL,
};

static volatile T_IM_ME_START	gIpu_3dnr_Me_Start_Prm = {
	.pme[ 0 ] = {
		.mbnum_h				= 0,		// Variable.
		.mbnum_v				= 0,		// Variable.
		.cancel_mode			= D_IM_ME_VEC_CANCEL_MODE_OFF,
		.cancel_threshold_hi	= 0xFF,
		.cancel_threshold_lo	= 0x00,
		.vec_hsize				= 0,		// Variable.
	},
	.pme[ 1 ] = {
		.mbnum_h				= 0,		// Variable.
		.mbnum_v				= 0,		// Variable.
		.cancel_mode			= D_IM_ME_VEC_CANCEL_MODE_OFF,
		.cancel_threshold_hi	= 0xFF,
		.cancel_threshold_lo	= 0x00,
		.vec_hsize				= 0,		// Variable.
	},
	.pme[ 2 ] = {
		.mbnum_h				= 0,		// Variable.
		.mbnum_v				= 0,		// Variable.
		.cancel_mode			= D_IM_ME_VEC_CANCEL_MODE_OFF,
		.cancel_threshold_hi	= 0xFF,
		.cancel_threshold_lo	= 0x00,
		.vec_hsize				= 0,		// Variable.
	},
	.rmcif = {
		.pre_shift_x			= 0,		// Variable.
		.pre_shift_y			= 0,		// Variable.
		.pre_r_addr_hi			= 0,		// Variable.
		.pre_r_addr_lo			= 0,		// Variable.
		.cur_r_addr_hi			= 0,		// Variable.
		.cur_r_addr_lo			= 0,		// Variable.
		.vec_w_addr_hi			= 0,		// Variable.
		.vec_w_addr_hi			= 0,		// Variable.
		.pre_gwidth				= 0,		// Variable.
		.pre_width				= 0,		// Variable.
		.pre_height				= 0,		// Variable.
		.cur_gwidth				= 0,		// Variable.
		.cur_margin_width		= D_IM_ME_CUR_MARGIN_0,		// Fixed.
		.cur_margin_height		= D_IM_ME_CUR_MARGIN_0,		// Fixed.
		.prefetch_width			= 0,		// Variable.
		.prefetch_height		= 0,		// Variable.
		.search_width			= 0,		// Variable.
		.search_height			= 0,		// Variable.
	},
};

static volatile T_IM_SHDR_CTRL	gIpu_3dnr_shdr_Ctrl_Prm = {
	.image_set.in_bit_depth		= D_IM_SHDR_INBITDEPTH_8,
	.image_set.in_c_fmt			= D_IM_SHDR_INCFMT_420SP,
	.image_set.in_edge_cpy		= D_IM_SHDR_ON,
	.image_set.in_edge_cut		= D_IM_SHDR_ON,
	.image_set.out_ctrl			= D_IM_SHDR_OUTCTRL_YCC,
	.image_set.out_sel			= D_IM_SHDR_OUTSEL_YCC_16,
	.image_set.out_sel_en		= D_IM_SHDR_DISABLE,
	.image_mode.exp_mag			= 0x0100,
	.image_mode.fil_margin		= D_IM_SHDR_FLITER_MARGIN_16,
	.image_mode.rexp_mag_shift	= 0,
	.image_mode.rexp_mag		= 0x10000,
	.image_mode.shift_x			= 0,
	.image_mode.shift_y			= 0,
	.image_mode.fmd_en			= D_IM_SHDR_ENABLE,					// IQ.bin
	.image_mode.bypass_en		= D_IM_SHDR_DISABLE,
	.image_mode.smc_in			= D_IM_SHDR_SMCIN_STN_REF,
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
	.sdram.r_vfm.a				= D_IM_SHDR_VFMT_VIDEO,
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
	.compip.mode				= D_IM_SHDR_COMP_MODE_LOSSLESS,
//	.compip.mode				= D_IM_SHDR_COMP_MODE_LOSSY,
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

static volatile T_IM_SHDR_PRE	gIpu_3dnr_shdr_Pre_Prm = {
	.pre_mode.cus_mode			= D_IM_SHDR_CUS_YCC420_BILINEAR_B,
	.pre_mode.degamma_mode		= D_IM_SHDR_DEGAMMA_LUT,
	.y2r_coef[ 0 ] = 0x00000100,
	.y2r_coef[ 1 ] = 0x00000000,
	.y2r_coef[ 2 ] = 0x00000194,
	.y2r_coef[ 3 ] = 0x00000100,
	.y2r_coef[ 4 ] = 0xffd0,
	.y2r_coef[ 5 ] = 0xff88,
	.y2r_coef[ 6 ] = 0x00000100,
	.y2r_coef[ 7 ] = 0x000001db,
	.y2r_coef[ 8 ] = 0x00000000,
	.y2r_clp.upper_long0[ 0 ] = 0x000000ff,
	.y2r_clp.upper_long0[ 1 ] = 0x000000ff,
	.y2r_clp.upper_long0[ 2 ] = 0x000000ff,
	.y2r_clp.lower_long0[ 0 ] = 0x00000000,
	.y2r_clp.lower_long0[ 1 ] = 0x00000000,
	.y2r_clp.lower_long0[ 2 ] = 0x00000000,
	.y2r_clp.upper_short[ 0 ] = 0x000000ff,
	.y2r_clp.upper_short[ 1 ] = 0x000000ff,
	.y2r_clp.upper_short[ 2 ] = 0x000000ff,
	.y2r_clp.lower_short[ 0 ] = 0x00000000,
	.y2r_clp.lower_short[ 1 ] = 0x00000000,
	.y2r_clp.lower_short[ 2 ] = 0x00000000,
	.r2y_coef[ 0 ] = 0x00000036,
	.r2y_coef[ 1 ] = 0x000000b7,
	.r2y_coef[ 2 ] = 0x00000013,
	.r2y_coef[ 3 ] = 0xffe3,
	.r2y_coef[ 4 ] = 0xff9d,
	.r2y_coef[ 5 ] = 0x00000080,
	.r2y_coef[ 6 ] = 0x00000080,
	.r2y_coef[ 7 ] = 0xff8c,
	.r2y_coef[ 8 ] = 0xfff4,
	.r2y_clp.upper_long_y  = 0x00000fff,
	.r2y_clp.upper_long_c[ 0 ] = 0x000007ff,
	.r2y_clp.upper_long_c[ 1 ] = 0x000007ff,
	.r2y_clp.lower_long_y = 0x00000000,
	.r2y_clp.lower_long_c[ 0 ] = 0xf800,
	.r2y_clp.lower_long_c[ 1 ] = 0xf800,
	.r2y_clp.upper_short_y = 0x00000fff,
	.r2y_clp.upper_short_c[ 0 ] = 0x000007ff,
	.r2y_clp.upper_short_c[ 1 ] = 0x000007ff,
	.r2y_clp.lower_short_y = 0x00000000,
	.r2y_clp.lower_short_c[ 0 ] = 0xf800,
	.r2y_clp.lower_short_c[ 1 ] = 0xf800,
	.short_knee.offset[ 0 ] = 0x00000000,
	.short_knee.offset[ 1 ] = 0x00000000,
	.short_knee.offset[ 2 ] = 0x00000000,
	.short_knee.slope[ 0 ] = 0x00000000,
	.short_knee.slope[ 1 ] = 0x00000000,
	.short_knee.slope[ 2 ] = 0x00000020,
	.short_knee.boundary[ 0 ] = 0x00000000,
	.short_knee.boundary[ 1 ] = 0x00000000,
	.long_knee.offset[ 0 ] = 0x00000000,
	.long_knee.offset[ 1 ] = 0x00000000,
	.long_knee.offset[ 2 ] = 0x00000000,
	.long_knee.slope[ 0 ] = 0x00000000,
	.long_knee.slope[ 1 ] = 0x00000000,
	.long_knee.slope[ 2 ] = 0x00000020,
	.long_knee.boundary[ 0 ] = 0x00000000,
	.long_knee.boundary[ 1 ] = 0x00000000,
};

static volatile T_IM_SHDR_FMD	gIpu_3dnr_shdr_Fmd_Prm = {
	.ep_boundary[ 0 ]			= 0x00,		// D.C.
	.ep_boundary[ 1 ]			= 0x40,									// IQ.bin
	.ep_boundary[ 2 ]			= 0x80,									// IQ.bin
	.ep_boundary[ 3 ]			= 0xC0,									// IQ.bin
	.ep_offset[ 0 ]				= 0x0A,									// IQ.bin
	.ep_offset[ 1 ]				= 0x0A,									// IQ.bin
	.ep_offset[ 2 ]				= 0x0A,									// IQ.bin
	.ep_offset[ 3 ]				= 0x0A,									// IQ.bin
	.ep_blend					= 0x0,									// IQ.bin
	.fd_weight					= 0x08,									// IQ.bin
	.mvcnt.bounds				= 0x00,
	.mvcnt.threshold			= 0x00,
	.mvcnt.gr					= 0x0000,
	.bldr_offset[ 0 ]			= 0x00,									// IQ.bin
	.bldr_offset[ 1 ]			= 0x08,									// IQ.bin
	.bldr_offset[ 2 ]			= 0x18,									// IQ.bin
	.bldr_offset[ 3 ]			= 0xe8,									// IQ.bin
	.bldr_offset[ 4 ]			= 0xFF,									// IQ.bin
	.bldr_slope[ 0 ]			= 0x20,									// IQ.bin
	.bldr_slope[ 1 ]			= 0x100,									// IQ.bin
	.bldr_slope[ 2 ]			= 0x680,									// IQ.bin
	.bldr_slope[ 3 ]			= 0x170,									// IQ.bin
	.bldr_slope[ 4 ]			= 0x000,								// IQ.bin
	.bldr_boundary[ 0 ]			= 0x00,		// D.C.
	.bldr_boundary[ 1 ]			= 0x04,									// IQ.bin
	.bldr_boundary[ 2 ]			= 0x05,									// IQ.bin
	.bldr_boundary[ 3 ]			= 0x06,									// IQ.bin
	.bldr_boundary[ 4 ]			= 0x07,									// IQ.bin
	.weight_r2y_coef[ 0 ]		= 0x004D,								// IQ.bin
	.weight_r2y_coef[ 1 ]		= 0x0096,								// IQ.bin
	.weight_r2y_coef[ 2 ]		= 0x001D,								// IQ.bin
	.weight_r2y_clp.upper		= 0xFF,									// IQ.bin
	.weight_r2y_clp.lower		= 0x00,									// IQ.bin
	.weight_base				= 0x0,									// IQ.bin
	.weight_offset[ 0 ][ 0 ]	= 0x000,								// IQ.bin
	.weight_offset[ 0 ][ 1 ]	= 0x000,								// IQ.bin
	.weight_offset[ 0 ][ 2 ]	= 0x000,								// IQ.bin
	.weight_offset[ 0 ][ 3 ]	= 0x000,								// IQ.bin
	.weight_offset[ 0 ][ 4 ]	= 0x100,								// IQ.bin
	.weight_offset[ 1 ][ 0 ]	= 0x000,								// IQ.bin
	.weight_offset[ 1 ][ 1 ]	= 0x000,								// IQ.bin
	.weight_offset[ 1 ][ 2 ]	= 0x000,								// IQ.bin
	.weight_offset[ 1 ][ 3 ]	= 0x000,								// IQ.bin
	.weight_offset[ 1 ][ 4 ]	= 0x100,								// IQ.bin
	.weight_offset[ 2 ][ 0 ]	= 0x000,								// IQ.bin
	.weight_offset[ 2 ][ 1 ]	= 0x000,								// IQ.bin
	.weight_offset[ 2 ][ 2 ]	= 0x000,								// IQ.bin
	.weight_offset[ 2 ][ 3 ]	= 0x000,								// IQ.bin
	.weight_offset[ 2 ][ 4 ]	= 0x100,								// IQ.bin
	.weight_slope[ 0 ][ 0 ]		= 0x000,								// IQ.bin
	.weight_slope[ 0 ][ 1 ]		= 0x000,								// IQ.bin
	.weight_slope[ 0 ][ 2 ]		= 0x000,								// IQ.bin
	.weight_slope[ 0 ][ 3 ]		= 0x000,								// IQ.bin
	.weight_slope[ 0 ][ 4 ]		= 0x000,								// IQ.bin
	.weight_slope[ 1 ][ 0 ]		= 0x000,								// IQ.bin
	.weight_slope[ 1 ][ 1 ]		= 0x000,								// IQ.bin
	.weight_slope[ 1 ][ 2 ]		= 0x000,								// IQ.bin
	.weight_slope[ 1 ][ 3 ]		= 0x000,								// IQ.bin
	.weight_slope[ 1 ][ 4 ]		= 0x000,								// IQ.bin
	.weight_slope[ 2 ][ 0 ]		= 0x000,								// IQ.bin
	.weight_slope[ 2 ][ 1 ]		= 0x000,								// IQ.bin
	.weight_slope[ 2 ][ 2 ]		= 0x000,								// IQ.bin
	.weight_slope[ 2 ][ 3 ]		= 0x000,								// IQ.bin
	.weight_slope[ 2 ][ 4 ]		= 0x000,								// IQ.bin
	.weight_boundary[ 0 ][ 0 ]	= 0x00,		// D.C.
	.weight_boundary[ 0 ][ 1 ]	= 0x00,									// IQ.bin
	.weight_boundary[ 0 ][ 2 ]	= 0x00,									// IQ.bin
	.weight_boundary[ 0 ][ 3 ]	= 0x00,									// IQ.bin
	.weight_boundary[ 0 ][ 4 ]	= 0x00,									// IQ.bin
	.weight_boundary[ 1 ][ 0 ]	= 0x00,		// D.C.
	.weight_boundary[ 1 ][ 1 ]	= 0x00,									// IQ.bin
	.weight_boundary[ 1 ][ 2 ]	= 0x00,									// IQ.bin
	.weight_boundary[ 1 ][ 3 ]	= 0x00,									// IQ.bin
	.weight_boundary[ 1 ][ 4 ]	= 0x00,									// IQ.bin
	.weight_boundary[ 2 ][ 0 ]	= 0x00,		// D.C.
	.weight_boundary[ 2 ][ 1 ]	= 0x00,									// IQ.bin
	.weight_boundary[ 2 ][ 2 ]	= 0x00,									// IQ.bin
	.weight_boundary[ 2 ][ 3 ]	= 0x00,									// IQ.bin
	.weight_boundary[ 2 ][ 4 ]	= 0x00,									// IQ.bin
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
};

static volatile T_IM_SHDR_PMSK	gIpu_3dnr_shdr_Pmsk_Prm = {
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

static volatile T_IM_SHDR_SPNR	gIpu_3dnr_shdr_Spnr_Prm = {
	.spnr_en					= D_IM_SHDR_ENABLE,						// IQ.bin
	.spe_blend					= 0x0,									// IQ.bin
	.spe_boundary[ 0 ]			= 0x000,		// D.C.
	.spe_boundary[ 1 ]			= 0x400,								// IQ.bin
	.spe_boundary[ 2 ]			= 0x800,								// IQ.bin
	.spe_boundary[ 3 ]			= 0xC00,								// IQ.bin
	.spe_offset[ 0 ]			= 0x000,								// IQ.bin
	.spe_offset[ 1 ]			= 0x000,								// IQ.bin
	.spe_offset[ 2 ]			= 0x000,								// IQ.bin
	.spe_offset[ 3 ]			= 0x000,								// IQ.bin
};

static volatile T_IM_SHDR_MSK	gIpu_3dnr_shdr_Msk_Prm = {
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
	.norm_gain[ 0 ]				= 0x80,									// IQ.bin
	.norm_gain[ 1 ]				= 0x80,									// IQ.bin
	.norm_gain[ 2 ]				= 0x80,									// IQ.bin
	.norm_offset[ 0 ]			= 0x80,									// IQ.bin
	.norm_offset[ 1 ]			= 0x80,									// IQ.bin
	.norm_offset[ 2 ]			= 0x80,									// IQ.bin
	.msk_mode					= D_IM_SHDR_MSKMD_MSKONLY,
};

static volatile T_IM_SHDR_BLD	gIpu_3dnr_shdr_Bld_Prm = {
	.blend_sel					= D_IM_SHDR_BLDSEL_SHORT_LONG,
	.alpha_sel					= D_IM_SHDR_ALPHASEL_MERGEMASK,
	.alpha_val[ 0 ]				= 0xFF,
	.alpha_val[ 1 ]				= 0xFF,
	.alpha_val[ 2 ]				= 0xFF,
};

static volatile T_IM_SHDR_POST	gIpu_3dnr_shdr_Post_Prm = {
	.sft1rnd.sft1rnd_en			= D_IM_SHDR_ENABLE,
//	.sft1rnd.sft1rnd_en			= D_IM_SHDR_DISABLE,
	.y2r_coef[ 0 ]				= 0x0100,
	.y2r_coef[ 1 ]				= 0x0001,
	.y2r_coef[ 2 ]				= 0x0166,
	.y2r_coef[ 3 ]				= 0x0100,
	.y2r_coef[ 4 ]				= 0xffa8,
	.y2r_coef[ 5 ]				= 0xff48,
	.y2r_coef[ 6 ]				= 0x0100,
	.y2r_coef[ 7 ]				= 0x01c6,
	.y2r_coef[ 8 ]				= 0xfffe,
	.y2r_clp_th_upper[ 0 ]		= 0xffff,
	.y2r_clp_th_upper[ 1 ]		= 0xffff,
	.y2r_clp_th_upper[ 2 ]		= 0xffff,
	.y2r_clp_th_lower[ 0 ]		= 0x0000,
	.y2r_clp_th_lower[ 1 ]		= 0x0000,
	.y2r_clp_th_lower[ 2 ]		= 0x0000,
	.ev_sel						= D_IM_SHDR_POSTSEL1_Y,
	.mulqpos					= D_IM_SHDR_POSTMULQPOS_Q5,
	.ev_kn_boundary[ 0 ]		= 0xa75c,		// D.C.
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
	.hdrcss_mode				= D_IM_SHDR_HDRCSSMD_YCC422_CENTER,
};

static volatile T_IM_SHDR_RESIZE	gIpu_3dnr_shdr_Resize_Prm = {
	.chmode.a_w_en				= D_IM_SHDR_ENABLE,
	.chmode.a_drc_en			= D_IM_SHDR_DISABLE,
	.chmode.a_css_md			= D_IM_SHDR_CSSMD_YCC420_CENTER,
	.chmode.b_w_en				= D_IM_SHDR_DISABLE,
	.chmode.b_rdc_md			= D_IM_RDCMD1_1_4,
	.chmode.b_css_md			= D_IM_SHDR_CSSMD_YCC422,
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
	.chmode.d_trm_en			= D_IM_SHDR_ENABLE,
	.chmode.d_drc_en			= D_IM_SHDR_DISABLE,
	.chmode.d_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
	.chmode.e_w_en				= D_IM_SHDR_ENABLE,
	.chmode.e_rsz_md			= D_IM_RSZMD_ODD1,
	.chmode.e_rdc_en			= D_IM_SHDR_ENABLE,
	.chmode.e_rdc_md			= D_IM_RDCMD_1_2,
	.chmode.e_trm_en			= D_IM_SHDR_DISABLE,
	.chmode.e_css_md			= D_IM_SHDR_CSSMD_YCC420_ODD,
	.chmode.f_w_en				= D_IM_SHDR_ENABLE,
	.chmode.f_rsz_md			= D_IM_RSZMD_ODD1,
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
	.woutsz.d_hsize				= 0x0500,	// 1280
	.woutsz.d_vsize				= 0x02D0,	// 720
	.woutsz.e_hsize				= 0x0500,	// 1280
	.woutsz.e_vsize				= 0x02D0,	// 720
	.woutsz.f_hsize				= 0x01E0,	// 480
	.woutsz.f_vsize				= 0x010E,	// 270
	.rszhcnt.c_hsta				= 0x0000,
	.rszhcnt.c_hpit				= 0x7FFF,	// 50%
	.rszhcnt.d_hsta				= 0x0000,
	.rszhcnt.d_hpit				= 0x5FFF,	// 66.7%
	.rszhcnt.e_hsta				= 0x0000,
	.rszhcnt.e_hpit				= 0x5FFF,	// 66.7%
	.rszhcnt.f_hsta				= 0x0000,
	.rszhcnt.f_hpit				= 0x7FFF,	// 50%
	.rszvcnt.c_vsta				= 0x0000,
	.rszvcnt.c_vpit				= 0x7FFF,	// 50%
	.rszvcnt.d_vsta				= 0x0000,
	.rszvcnt.d_vpit				= 0x5FFF,	// 66.7%
	.rszvcnt.e_vsta				= 0x0000,
	.rszvcnt.e_vpit				= 0x5FFF,	// 66.7%
	.rszvcnt.f_vsta				= 0x0000,
	.rszvcnt.f_vpit				= 0x7FFF,	// 50%
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

static volatile T_IM_SHDR_SMC	gIpu_3dnr_shdr_Smc_Prm = {
	.mcarmd.mca_mode			= D_IM_MCAMD_NR_EFFECT,
	.mcarmd.mcr_mode			= D_IM_MCAMD_MAX_VECTOR,
	.mcmpmd.mcmp_mode			= D_IM_SHDR_ON,
	.mcmpmd.mvw_en				= D_IM_SHDR_DISABLE,
	.mmvval.mmv_x				= 0x40,
	.mmvval.mmv_y				= 0x30,
	.mmvrcnt.mmvro_x			= 0x08,
	.mmvrcnt.mmvro_y			= 0x08,
	.mmvrcnt.mmvrk_x			= 0x2,
	.mmvrcnt.mmvrk_y			= 0x2,
	.mdbcnt.mdb_x				= 0x5,
	.mdbcnt.mdb_y				= 0x3,
	.mdbrocnt.mdbro_x			= 0x04,
	.mdbrocnt.mdbro_y			= 0x08,
	.mdbrocnt.mdbrk_x			= 0x0,
	.mdbrocnt.mdbrk_y			= 0x0,
	.molrcnt.molr_sm			= 0x4,
	.molrcnt.molr_ss			= 0x8,
	.molrcnt.molr_o				= 0x8,
	.molrcnt.molr_k				= 0x2,
	.mcpcnt.mcpx_s				= 0x00,
	.mcpcnt.mcpx_o				= 0x10,
	.mcpcnt.mcpx_k				= 0x3,
	.mmcrcnt.mmcr_s				= 0x008,
	.mmcrcnt.mmcr_k				= 0x1,
	.molecnt.mole_s				= 0x004,
	.molecnt.mole_o				= 0x00,
	.molecnt.mole_k				= 0x3,
	.mmvecnt.mmve_s				= 0x002,
	.mmvecnt.mmve_o				= 0x00,
	.mmvecnt.mmve_k				= 0x3,
	.mcpxecnt.mcpxe_s			= 0x010,
	.mcpxecnt.mcpxe_o			= 0x00,
	.mcpxecnt.mcpxe_k			= 0x2,
	.mvlcnt.mvl_en				= D_IM_SHDR_ENABLE,
	.mvlcnt.mvl_threshold		= 0x04,
	.mvlcnt.mvl_count_threshold	= 0x00,
};

static volatile T_IM_SHDR_AXI	gIpu_3dnr_shdr_Axi_Prm = {
	.ax_mode.r_burst			= D_IM_SHDR_BURSTLEN_512,
	.ax_mode.w_burst			= D_IM_SHDR_BURSTLEN_512,
	.ax_mode.w_w_mask			= D_IM_SHDR_WWMASK_ON,
	.r_ctl.a_cahche[ 0 ]		= 0x0,
	.r_ctl.a_cahche[ 1 ]		= 0x0,
	.r_ctl.a_protect[ 0 ]		= 0x0,
	.r_ctl.a_protect[ 1 ]		= 0x0,
	.r_ctl.b_cahche[ 0 ]		= 0x0,
	.r_ctl.b_cahche[ 1 ]		= 0x0,
	.r_ctl.b_protect[ 0 ]		= 0x0,
	.r_ctl.b_protect[ 1 ]		= 0x0,
	.r_ctl.c_cahche[ 0 ]		= 0x0,
	.r_ctl.c_cahche[ 1 ]		= 0x0,
	.r_ctl.c_cahche[ 2 ]		= 0x0,
	.r_ctl.c_cahche[ 3 ]		= 0x0,
	.r_ctl.c_protect[ 0 ]		= 0x0,
	.r_ctl.c_protect[ 1 ]		= 0x0,
	.r_ctl.c_protect[ 2 ]		= 0x0,
	.r_ctl.c_protect[ 3 ]		= 0x0,
	.r_ctl.d_cahche[ 0 ]		= 0x0,
	.r_ctl.d_cahche[ 1 ]		= 0x0,
	.r_ctl.d_protect[ 0 ]		= 0x0,
	.r_ctl.d_protect[ 1 ]		= 0x0,
	.w_ctl.a_cahche[ 0 ]		= 0x0,
	.w_ctl.a_cahche[ 1 ]		= 0x0,
	.w_ctl.a_cahche[ 2 ]		= 0x0,
	.w_ctl.a_cahche[ 3 ]		= 0x0,
	.w_ctl.a_protect[ 0 ]		= 0x0,
	.w_ctl.a_protect[ 1 ]		= 0x0,
	.w_ctl.a_protect[ 2 ]		= 0x0,
	.w_ctl.a_protect[ 3 ]		= 0x0,
	.w_ctl.b_cahche[ 0 ]		= 0x0,
	.w_ctl.b_cahche[ 1 ]		= 0x0,
	.w_ctl.b_cahche[ 2 ]		= 0x0,
	.w_ctl.b_cahche[ 3 ]		= 0x0,
	.w_ctl.b_cahche[ 4 ]		= 0x0,
	.w_ctl.b_cahche[ 5 ]		= 0x0,
	.w_ctl.b_protect[ 0 ]		= 0x0,
	.w_ctl.b_protect[ 1 ]		= 0x0,
	.w_ctl.b_protect[ 2 ]		= 0x0,
	.w_ctl.b_protect[ 3 ]		= 0x0,
	.w_ctl.b_protect[ 4 ]		= 0x0,
	.w_ctl.b_protect[ 5 ]		= 0x0,
	.w_ctl.c_cahche[ 0 ]		= 0x0,
	.w_ctl.c_cahche[ 1 ]		= 0x0,
	.w_ctl.c_cahche[ 2 ]		= 0x0,
	.w_ctl.c_cahche[ 3 ]		= 0x0,
	.w_ctl.c_cahche[ 4 ]		= 0x0,
	.w_ctl.c_cahche[ 5 ]		= 0x0,
	.w_ctl.c_cahche[ 6 ]		= 0x0,
	.w_ctl.c_protect[ 0 ]		= 0x0,
	.w_ctl.c_protect[ 1 ]		= 0x0,
	.w_ctl.c_protect[ 2 ]		= 0x0,
	.w_ctl.c_protect[ 3 ]		= 0x0,
	.w_ctl.c_protect[ 4 ]		= 0x0,
	.w_ctl.c_protect[ 5 ]		= 0x0,
	.w_ctl.c_protect[ 6 ]		= 0x0,
};

static volatile USHORT	gIpu_3dnr_shdr_Degamma_Prm[ 3 ][ D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ] = {
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

static UCHAR	gIpu_3dnr_shdr_Gainup_Tbl0[ 256 ] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};
static UCHAR	gIpu_3dnr_shdr_Gainup_Tbl1[ 256 ] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

static volatile UINT32 gIpu_3dnr_shdr_InPrm = D_IM_SHDR_SHDRIN_STN_SMCOUT;

static ULLONG gFrame_No_Shdr = 0;

static ULONG   start_time[10],end_time[10];

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static INT32 ipu_3dnr_set_gmd( ULONG heap_addr, const T_SHDR_MNG_OUT* img_red, const T_SHDR_MNG_OUT* img_full  );
static INT32 ipu_3dnr_set_me( void );
static INT32 ipu_3dnr_set_shdr( BOOL set_degamma );
static INT32 ipu_3dnr_start_iip(	T_SHDR_MNG_OUT* const imgSrc, T_SHDR_MNG_OUT* const imgDst );
static INT32 ipu_3dnr_start_gmd(E_SHARE_OP_3DNR_MODE mode,
										BOOL bypassMode,
										BOOL brcEnable,
										BOOL brcMode,
										const T_SHDR_MNG_OUT* const imgtSrcFull,
										const T_SHDR_MNG_OUT* const imgtSrc,
										const T_SHDR_MNG_OUT* const imgtRefFull,
										const T_SHDR_MNG_OUT* const imgtRef,
										T_SHDR_MOTION* const motion );
static INT32 ipu_3dnr_start_me(	BOOL bypassMode,
										const T_SHDR_MOTION* const motion,
										const T_SHDR_MNG_OUT* const imgSrc,
										const T_SHDR_MNG_IN* const imgRef,
										T_SHDR_MNG_IN* const imgMe );
static INT32 ipu_3dnr_start_shdr(	BOOL	bypassMode,
								 			BOOL	compMode,
								 			BOOL	mcMode,
								 			BOOL	edgeCpy,
								 			BOOL	edgeCut,
								 			const	T_SHDR_MNG_IN* const imgSrc,
								 			const	T_SHDR_MNG_IN* const imgRef,
								 			const	T_SHDR_MNG_IN* const imgMe,
											T_SHDR_MNG_OUT* const imgHevc,
											T_SHDR_MNG_OUT* const imgMeIn,
											T_SHDR_MNG_OUT* const imgH264_0,
											T_SHDR_MNG_OUT* const imgH264_1,
											T_SHDR_MNG_OUT* const imgYUV_0,
											T_SHDR_MNG_OUT* const imgYUV_1,
											T_SHDR_MNG_OUT* const comp_w,
											T_SHDR_MNG_IN* const comp_r,
											UCHAR cbcr_global_x2);
static VOID ipu_3dnr_get_pixel_avg_reduction_info_rdc( FLOAT ratio, UCHAR *rdc_en, UCHAR* rdc_md );
static VOID ipu_3dnr_get_pixel_avg_reduction_info_pitch( UINT32 srcSize, UINT32 dstSize, USHORT* pitch, UCHAR rdc_en );
static UCHAR ipu_3dnr_get_pixel_avg_reduction_info( UINT32 src_width_Size, UINT32 src_height_Size,
																	UINT32  dst_width_Size, UINT32 dst_height_Size,
																	USHORT* pitch0, USHORT* pitch1, 
																	USHORT* trmsta0, USHORT* trmsta1,
																	UCHAR* rdc_md );
static VOID ipu_3dnr_get_regtbl( T_IM_SHDR_CTRL** shdr_ctrl, T_IM_SHDR_PRE** shdr_pre,
										T_IM_SHDR_FMD** shdr_fmd, T_IM_SHDR_SPNR** shdr_spnr, 
										T_IM_SHDR_MSK** shdr_msk, T_IM_SHDR_PMSK** shdr_pmsk, 
										UINT32** shdr_in );
static VOID ipu_3dnr_me_irq_callback( ULONG interrupt_flag_mask );
static VOID ipu_3dnr_shdr_irq_callback( ULONG interrupt_flag_mask );
static FJ_ERR_CODE ipu_iq_default_set_3dnr_reg_tbl( T_IQ_3DNR_IM_ARG* const im_arg );
static VOID ipu_3dnr_me_shdr_calc_shift( const T_SHDR_MNG_OUT* const imgSrcFull, 
										 const T_SHDR_MNG_OUT* const imgRefFull,
										 BOOL is_me,
										 T_SHDR_MOTION* motion );

/*==============================================================================*/
/* Debug Function																*/
/*==============================================================================*/
#ifdef DEBUG_DUMP_SHDR_REG
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
	Ddim_Print(("\n"));
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
	Ddim_Print(("\n"));
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
	Ddim_Print(("\n"));
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
	Ddim_Print(("\n"));
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
	Ddim_Print(("\n"));
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
	Ddim_Print(("\n"));
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
	Ddim_Print(("\n"));
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
	Ddim_Print(("\n"));
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
	Ddim_Print(("\n"));
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
	Ddim_Print(("\n"));
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
	Ddim_Print(("\n"));
}

#endif

#ifdef DEBUG_SAVE_IMG
#include "fj_filesystem.h"
static void debug_save_file( CHAR* fname, ULONG addr, ULONG size )
{
	ULONG fid;
	ULONG write_size = 0;

	fid = FJ_Fs_Open ( fname, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	FJ_Fs_Write ( fid, (VOID*)addr, size, &write_size );
	FJ_Fs_Close( fid );
}

#endif

#ifdef DEBUG_DUMP_ME_RESULT
static void debug_dump_me_result( ULONG addr, USHORT gwidth, USHORT width, USHORT height )
{
	static INT32 print_interval = 0;
	INT32	print_interval_num = 10;
	INT32	x, y;
	INT32	x_ofs, y_ofs;
	INT32	print_num = 8;
	
	USHORT*	p_data;
	USHORT	data;
	CHAR	dx, dy;
	
	if( print_interval-- > 0 ){
		return;
	}else{
		print_interval = print_interval_num;
	}
	
	BF_L1l2cache_Flush_Addr(addr, gwidth * height);
	
	if( print_num == 0 ){
		x_ofs = 1;
		y_ofs = 1;
	}else{
		x_ofs = width / print_num;
		y_ofs = height / print_num;
	}
	
	p_data = (USHORT*)addr;
	for( y = 0; y < height; y += y_ofs ){
		for( x = 0; x < width; x += x_ofs ){
			data = p_data[ y * gwidth/2 + x ];
			
			dy = (data>>8) & 0x1F;
			dx = (data   ) & 0x1F;
			
			if( dy & 0x10 ){
				dy |= 0xF0;
			}
			if( dx & 0x10 ){
				dx |= 0xF0;
			}
			
			//printf( "(%d, %d),", dx, dy );
			//printf( "%d,", dx );
			printf( "%d,", dy );
		}
		printf("\n");
	}
	printf("\n");
}
#endif

#ifdef DEBUG_3DNR_EFFECT
static void debug_3dnr_effect( T_IQ_3DNR_IM_ARG* const im_arg )
{
	im_arg->shdr_msk->norm_gain[0]		= 0;
	im_arg->shdr_msk->norm_gain[1]		= im_arg->shdr_msk->norm_gain[0];
	im_arg->shdr_msk->norm_gain[2]		= im_arg->shdr_msk->norm_gain[0];
	im_arg->shdr_msk->norm_offset[0]	= 128;
	im_arg->shdr_msk->norm_offset[1]	= im_arg->shdr_msk->norm_offset[0];
	im_arg->shdr_msk->norm_offset[2]	= im_arg->shdr_msk->norm_offset[0];
}
#endif

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
VOID IPU_3DNR_IQ_SetParam( VOID )
{
	INT32	ercd;
	T_IQ_3DNR_IM_ARG	im_arg;

	ipu_3dnr_get_regtbl( &im_arg.shdr_ctrl, &im_arg.shdr_pre, &im_arg.shdr_fmd, &im_arg.shdr_spnr, &im_arg.shdr_msk,
						&im_arg.shdr_pmsk, &im_arg.shdr_in );
	
	ercd = IQPara_3DNR( 0, &im_arg );
	if( ercd != IQ_NoErr ) {
		UPRINTF_ERR( (INT32)ercd );
		ipu_iq_default_set_3dnr_reg_tbl( &im_arg );
	}

#ifdef DEBUG_3DNR_EFFECT
	debug_3dnr_effect( &im_arg );
#endif
}

void Init_GMD_Proc( ULONG heap_addr, const T_SHDR_MNG_OUT* img_red, const T_SHDR_MNG_OUT* img_full  )
{
	INT32 ercd;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_GMD, FJ_MBALOG_TYPE_POINT, "GMD Init"));
	
	// Open GMD
	ercd = IPC_GMD_Open();
	if ( ercd != D_IPC_GMD_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return;
	}
			
	// Initial GMD
	ercd = IPC_GMD_Init();
	if( ercd != D_IPC_GMD_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return;
	}
			
	// Set GMD
	ercd = ipu_3dnr_set_gmd( heap_addr, img_red, img_full  );
	if( ercd != D_IPC_GMD_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return;
	}

	start_time[2] = 0;
	end_time[2] = 0;
}

void Start_GMD_Proc( T_IPU2SHDR_MNG* in, E_SHARE_OP_3DNR_MODE mode, ULLONG frame_no )
{
	INT32 ercd;
	T_SHDR_MNG_OUT ref_in_bak[2];
	ULONG	diff_time;
	T_SHDR_MOTION			motion = {0};
	T_SHARE_MOTION_VECTOR	share_motion = {0};;
	
	end_time[2]= BF_Get_Time_us();
	if(start_time[2] == 0) diff_time = 0;
	else diff_time = end_time[2] - start_time[2];
	start_time[2]= BF_Get_Time_us();
		
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_GMD, FJ_MBALOG_TYPE_START, "GMD Start %ld", diff_time));

#ifdef DEBUG_SRC_REF_MBA
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_GMD, FJ_MBALOG_TYPE_POINT, "GMD_Src0(%08lx)", in->sgde_out[E_SGDE_OUT_0].mem_addr_y));
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_GMD, FJ_MBALOG_TYPE_POINT, "GMD_Ref0(%08lx)", in->gmd_ref[0].mem_addr_y));
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_GMD, FJ_MBALOG_TYPE_POINT, "GMD_Src1(%08lx)", in->sgde_out[E_SGDE_OUT_1].mem_addr_y));
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_GMD, FJ_MBALOG_TYPE_POINT, "GMD_Ref1(%08lx)", in->gmd_ref[1].mem_addr_y));
#endif
		
	//save gmd reference image
	ref_in_bak[0] = in->gmd_in[0];
	ref_in_bak[1] = in->gmd_in[1];

	ercd = ipu_3dnr_start_gmd(	mode,					// [In]3dnr mode.
								FALSE,					// [In]Bypass mode.
								FALSE,					// [In]Brightness correct enable.
								FALSE,					// [In]Brightness correct mode.
								(const T_SHDR_MNG_OUT* const)&in->gmd_in[0],	// [In]Source image(YUV8).
								(const T_SHDR_MNG_OUT* const)&in->gmd_in[1],	// [In]Source image(Y8 reduction).
								(const T_SHDR_MNG_OUT* const)&in->gmd_ref[0],	// [In]Reference image(YUV8).
								(const T_SHDR_MNG_OUT* const)&in->gmd_ref[1],	// [In]Reference image(Y8 reduction).
								(T_SHDR_MOTION* const)&motion);					// [out]global motion.
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
	}
	
	// Set Share
	share_motion.x = motion.shift_x;
	share_motion.y = motion.shift_y;
	Set_Frame_Share_Motion_Vector( &share_motion, frame_no );
	
#ifdef DEBUG_SRC_REF_MBA
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_GMD, FJ_MBALOG_TYPE_POINT, "GMD_shift_x(%d)", gIpu_3dnr_shdr_Ctrl_Prm.image_mode.shift_x));
#endif
		
	//save gmd reference image
	in->gmd_ref[0] = ref_in_bak[0];
	in->gmd_ref[1] = ref_in_bak[1];
		
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_GMD, FJ_MBALOG_TYPE_END, "GMD End %ld", diff_time));

#ifdef DEBUG_SAVE_IMG
	{
		static UCHAR d_frame_cnt = 0;
		ULONG c_offs = 0;
		ULONG size = 0;
		
		if(d_frame_cnt == 30)
		{
			c_offs = in->gmd_in[0].mem_addr_c - in->gmd_in[0].mem_addr_y;
			size = c_offs - in->gmd_in[0].yuv_h_size * in->gmd_in[0].yuv_v_size + 16;
			size = size + in->gmd_in[0].yuv_h_size * in->gmd_in[0].yuv_v_size * 2;
			debug_save_file((CHAR *)"I:\\GMD_0.YUV", in->gmd_in[0].mem_addr_y, size);
			UPRINTF("Size: %d x %d, CbCr Addr Offset:0x%08lx\n", in->gmd_in[0].yuv_h_size, in->gmd_in[0].yuv_v_size, c_offs);
		}
		else if(d_frame_cnt == 60)
		{
			c_offs = in->gmd_ref[1].mem_addr_c - in->gmd_ref[1].mem_addr_y;
			size = c_offs - in->gmd_ref[1].yuv_h_size * in->gmd_ref[1].yuv_v_size + 16;
			size = size + in->gmd_ref[1].yuv_h_size * in->gmd_ref[1].yuv_v_size * 2;
			debug_save_file((CHAR *)"I:\\GMD_1.YUV", in->gmd_ref[1].mem_addr_y, size);
			UPRINTF("Size: %d x %d, CbCr Addr Offset:0x%08lx\n", in->gmd_ref[1].yuv_h_size, in->gmd_ref[1].yuv_v_size, c_offs);
		}
		else if(d_frame_cnt >= 90)
		{
			d_frame_cnt = 90;
		}
		
		d_frame_cnt++;
	}
#endif

}

void Close_GMD_Proc( void )
{
	INT32 ercd;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_GMD, FJ_MBALOG_TYPE_POINT, "GMD Close"));
	
	ercd = IPC_GMD_Close();
	if( ercd != D_IPC_GMD_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return;
	}
	
}

void Init_ME_Proc( void )
{
	INT32 ercd;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_ME, FJ_MBALOG_FUNC_ID_ME, FJ_MBALOG_TYPE_POINT, "ME Init"));
	
	// Open ME
	ercd = Im_ME_Open( 100 );
	if ( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return;
	}
			
	// Initial ME
	ercd = Im_ME_Init();
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return;
	}
			
	// Set ME
	ercd = ipu_3dnr_set_me();
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return;
	}

	start_time[3] = 0;
	end_time[3] = 0;
	start_time[4] = 0;
	end_time[4] = 0;
}

void Start_ME_Proc( T_IPU2SHDR_MNG* in, ULLONG frame_no )
{
	INT32 ercd;
	T_SHDR_MOTION motion;
	T_SHDR_MNG_IN ref_in_bak;
	ULONG	diff_time;
	T_SHARE_MOTION_VECTOR share_motion;
	
	end_time[3]= BF_Get_Time_us();
	if(start_time[3] == 0) diff_time = 0;
	else diff_time = end_time[3] - start_time[3];
	start_time[3]= BF_Get_Time_us();
		
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_ME, FJ_MBALOG_FUNC_ID_ME, FJ_MBALOG_TYPE_START, "IIP Start %d %ld", (INT32)frame_no, diff_time));
	
#ifdef DEBUG_SRC_REF_MBA
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_ME, FJ_MBALOG_FUNC_ID_ME, FJ_MBALOG_TYPE_POINT, "IIP_Src(%08lx)", in->iip_in.mem_addr_y));
#endif
	
	ipu_3dnr_start_iip((T_SHDR_MNG_OUT* const)&in->iip_in, (T_SHDR_MNG_OUT* const)&in->iip_out);
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_ME, FJ_MBALOG_FUNC_ID_ME, FJ_MBALOG_TYPE_END, "IIP Start %ld", diff_time));

	end_time[4]= BF_Get_Time_us();
	if(start_time[4] == 0) diff_time = 0;
	else diff_time = end_time[4] - start_time[4];
	start_time[4]= BF_Get_Time_us();

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_ME, FJ_MBALOG_FUNC_ID_ME, FJ_MBALOG_TYPE_START, "ME Start %d %ld", (INT32)frame_no, diff_time));

	// Share
	Get_Frame_Share_Motion_Vector( &share_motion, frame_no );
	
#ifdef DEBUG_3DNR_GMD
	if( share_motion.x || share_motion.y ){
		printf( "GMD = (%d, %d) \n", share_motion.x, share_motion.y );
	}
#endif
	
#ifdef DEBUG_SRC_REF_MBA
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_ME, FJ_MBALOG_FUNC_ID_ME, FJ_MBALOG_TYPE_POINT, "ME_Out(%08lx)", in->me_out.mem_addr_y));
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_ME, FJ_MBALOG_FUNC_ID_ME, FJ_MBALOG_TYPE_POINT, "ME_Src(%08lx)", in->iip_out.mem_addr_y));
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_ME, FJ_MBALOG_FUNC_ID_ME, FJ_MBALOG_TYPE_POINT, "ME_Ref(%08lx)", in->ref_in[1].mem_addr_y));
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_ME, FJ_MBALOG_FUNC_ID_ME, FJ_MBALOG_TYPE_POINT, "ME_shift_x(%d)", share_motion.x));
#endif
	
	// Calc shift for ME
	motion.shift_x = share_motion.x;
	motion.shift_y = share_motion.y;
	ipu_3dnr_me_shdr_calc_shift( (const T_SHDR_MNG_OUT* const)&in->shdr_in,
								 (const T_SHDR_MNG_OUT* const)&in->shdr_out[0], TRUE, &motion );
#ifdef DEBUG_3DNR_GMD
	if( share_motion.x || share_motion.y ){ //if( motion.shift_x || motion.shift_y ){
		printf( "MEin = (%d, %d) \n", motion.shift_x, motion.shift_y );
	}
#endif
	

	//backup me reference image
	ref_in_bak.mem_addr_y = in->iip_out.mem_addr_y;
	ref_in_bak.mem_addr_c = in->iip_out.mem_addr_c;
	ref_in_bak.yuv_h_byte = in->iip_out.yuv_h_byte;
	ref_in_bak.yuv_h_size = in->iip_out.yuv_h_size;
	ref_in_bak.yuv_v_size = in->iip_out.yuv_v_size;
	ref_in_bak.yuv_format = in->iip_out.yuv_format;
	
	ercd = ipu_3dnr_start_me(	FALSE,											// [In]Bypass mode.
								(const	T_SHDR_MOTION* const)&motion,			// [In]global motion.
								(const	T_SHDR_MNG_OUT* const)&in->iip_out, 	// [In]Source image(Y8 1/4).
								(const	T_SHDR_MNG_IN* const)&in->ref_in[1], 	// [In]Reference image(Y8 1/4).
								(T_SHDR_MNG_IN* const)&in->me_out); 			// [Out]Motion vector.
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
	}
	
	// for debug
	if( g_debug_3dnr.en && g_debug_3dnr.force_chg_me_out ){
		INT32 tmp_value		=( (g_debug_3dnr.force_me_out_x<<24) & 0xFF000000) |
							 ( (g_debug_3dnr.force_me_out_y<<16) & 0x00FF0000) |
							 ( (g_debug_3dnr.force_me_out_x<< 8) & 0x0000FF00) |
							 ( (g_debug_3dnr.force_me_out_y)     & 0x000000FF);
		BF_L1l2cache_Flush_Addr(in->me_out.mem_addr_y, (in->me_out.yuv_h_byte * in->me_out.yuv_v_size));
		memset( (VOID*)in->me_out.mem_addr_y, tmp_value, (in->me_out.yuv_h_byte * in->me_out.yuv_v_size) );
		BF_L1l2cache_Flush_Addr(in->me_out.mem_addr_y, (in->me_out.yuv_h_byte * in->me_out.yuv_v_size));
	}
	
#ifdef DEBUG_DUMP_ME_RESULT
	debug_dump_me_result( in->me_out.mem_addr_y, in->me_out.yuv_h_byte, in->me_out.yuv_h_size, in->me_out.yuv_v_size );
#endif

	//save me reference image
	in->ref_in[1] = ref_in_bak;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_ME, FJ_MBALOG_FUNC_ID_ME, FJ_MBALOG_TYPE_END, "ME Start %ld", diff_time));
}

void Close_ME_Proc( void )
{
	INT32 ercd;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_ME, FJ_MBALOG_FUNC_ID_ME, FJ_MBALOG_TYPE_POINT, "ME Close"));
	
	ercd = Im_ME_Close();
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return;
	}
	
}

void Init_SHDR_Proc( void )
{
	INT32 ercd;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_3DNR, FJ_MBALOG_TYPE_POINT, "SHDR Init"));
	
	// Open SHDR
	ercd = Im_SHDR_Open( 100 );
	if ( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return;
	}
			
	// Initial SHDR
	ercd = Im_SHDR_Init( FALSE );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return;
	}
			
	// Set SHDR
	ercd = ipu_3dnr_set_shdr(TRUE);
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return;
	}

	start_time[5] = 0;
	end_time[5] = 0;
}

void Start_SHDR_Proc( T_IPU2SHDR_MNG* in, E_SHARE_OP_3DNR_MODE mode, BOOL compMode, ULLONG frame_no, UCHAR cbcr_global_x2 )
{
	INT32	ercd;
	BOOL	bypassMode, mcMode, edgeCpy, edgeCut;
	T_SHDR_MNG_IN ref_in_bak, cmp_in_back;
	ULONG	diff_time;
	
	end_time[5]= BF_Get_Time_us();
	if(start_time[5] == 0) diff_time = 0;
	else diff_time = end_time[5] - start_time[5];
	start_time[5]= BF_Get_Time_us();

	gFrame_No_Shdr = frame_no;

	bypassMode = TRUE;
	mcMode = FALSE;
	edgeCpy = FALSE;
	edgeCut = FALSE;
	
#ifdef D_IPU_3DNR_PROCESS_BY_TASK
	switch(mode)
	{
		case E_SHARE_OP_3DNR_MODE_OFF:
		break;
		case E_SHARE_OP_3DNR_MODE_NO_ME:
			bypassMode = FALSE;
			mcMode = FALSE;
			edgeCpy = TRUE;
			edgeCut = TRUE;
		break;
		case E_SHARE_OP_3DNR_MODE_ME:
		case E_SHARE_OP_3DNR_MODE_FPT:
		case E_SHARE_OP_3DNR_MODE_GYRO:
		case E_SHARE_OP_3DNR_MODE_PREHRE:
			bypassMode = FALSE;
			mcMode = TRUE;
			edgeCpy = TRUE;
			edgeCut = TRUE;
		break;
		default:
			return;
		break;
	}
#else
	switch(mode)
	{
		case E_SHARE_OP_3DNR_MODE_OFF:
		break;
		case E_SHARE_OP_3DNR_MODE_NO_ME:
			bypassMode = FALSE;
			mcMode = FALSE;
			edgeCpy = TRUE;
			edgeCut = TRUE;
		break;
		case E_SHARE_OP_3DNR_MODE_FPT:
			Start_GMD_Proc(in, mode, frame_no);
		case E_SHARE_OP_3DNR_MODE_ME:
		case E_SHARE_OP_3DNR_MODE_GYRO:
		case E_SHARE_OP_3DNR_MODE_PREHRE:
			Start_ME_Proc(in, frame_no);
			
			bypassMode = FALSE;
			mcMode = TRUE;
			edgeCpy = TRUE;
			edgeCut = TRUE;
		break;
		default:
			return;
		break;
	}
#endif

	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_3DNR, FJ_MBALOG_TYPE_START, "SHDR Start %d %ld", (INT32)frame_no, diff_time));

#ifdef DEBUG_SRC_REF_MBA
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_3DNR, FJ_MBALOG_TYPE_POINT, "Me_In(%08lx)", in->me_in.mem_addr_y));
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_3DNR, FJ_MBALOG_TYPE_POINT, "SHDR_Src(%08lx)", in->shdr_in.mem_addr_y));
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_3DNR, FJ_MBALOG_TYPE_POINT, "SHDR_Ref(%08lx)", in->ref_in[0].mem_addr_y));
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_3DNR, FJ_MBALOG_TYPE_POINT, "SHDR_Out0(%08lx)", in->shdr_out[ E_SHDR_OUT_RBK0_H265 ].mem_addr_y));
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_3DNR, FJ_MBALOG_TYPE_POINT, "SHDR_In_Cmp(%08lx)", in->cmptbl_in.mem_addr_y));
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_3DNR, FJ_MBALOG_TYPE_POINT, "SHDR_Out_Cmp(%08lx)", in->cmptbl_out.mem_addr_y));
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_3DNR, FJ_MBALOG_TYPE_POINT, "SHDR_shift_x(%d)", gIpu_3dnr_shdr_Ctrl_Prm.image_mode.shift_x));
#endif

	// ReUse SHDR
	if(in->vhdr_on == TRUE)
	{
		// SHDR macro driver open.
		OS_User_Wai_Sem( SID_IP_SHDR );
		
		// Open SHDR
		ercd = Im_SHDR_Open( 100 );
		if ( ercd != D_DDIM_OK ) {
			UPRINTF_ERR((INT32)ercd);
		}
				
		// Initial SHDR
		ercd = Im_SHDR_Init( FALSE );
		if( ercd != D_DDIM_OK ) {
			UPRINTF_ERR((INT32)ercd);
		}
				
		// Set SHDR
		ercd = ipu_3dnr_set_shdr(TRUE);
		if( ercd != D_DDIM_OK ) {
			UPRINTF_ERR((INT32)ercd);
		}
	}
			
	//save shdr reference image
	ref_in_bak.mem_addr_y = in->shdr_out[ E_SHDR_OUT_RBK0_H265 ].mem_addr_y;
	ref_in_bak.mem_addr_c = in->shdr_out[ E_SHDR_OUT_RBK0_H265 ].mem_addr_c;
	ref_in_bak.yuv_h_byte = in->shdr_out[ E_SHDR_OUT_RBK0_H265 ].yuv_h_byte;
	ref_in_bak.yuv_h_size = in->shdr_out[ E_SHDR_OUT_RBK0_H265 ].yuv_h_size;
	ref_in_bak.yuv_v_size = in->shdr_out[ E_SHDR_OUT_RBK0_H265 ].yuv_v_size;
	ref_in_bak.yuv_format = in->shdr_out[ E_SHDR_OUT_RBK0_H265 ].yuv_format;
	cmp_in_back.mem_addr_y = in->cmptbl_out.mem_addr_y;
	cmp_in_back.mem_addr_c = in->cmptbl_out.mem_addr_c;
	cmp_in_back.yuv_h_byte = in->cmptbl_out.yuv_h_byte;
	cmp_in_back.yuv_h_size = in->cmptbl_out.yuv_h_size;
	cmp_in_back.yuv_v_size = in->cmptbl_out.yuv_v_size;
	cmp_in_back.yuv_format = in->cmptbl_out.yuv_format;
	
	// for debug
	if( g_debug_3dnr.en ){
		if( g_debug_3dnr.force_fmd_off ){
			gIpu_3dnr_shdr_Ctrl_Prm.image_mode.fmd_en = 0;
		}
		if( g_debug_3dnr.force_mc_off ){
			mcMode = FALSE;
		}
		if( g_debug_3dnr.force_chg_blend ){
			gIpu_3dnr_shdr_Msk_Prm.norm_gain[0] = 0;
			gIpu_3dnr_shdr_Msk_Prm.norm_gain[1] = 0;
			gIpu_3dnr_shdr_Msk_Prm.norm_gain[2] = 0;
			gIpu_3dnr_shdr_Msk_Prm.norm_offset[0] = g_debug_3dnr.force_blend_ratio;
			gIpu_3dnr_shdr_Msk_Prm.norm_offset[1] = g_debug_3dnr.force_blend_ratio;
			gIpu_3dnr_shdr_Msk_Prm.norm_offset[2] = g_debug_3dnr.force_blend_ratio;
		}
	}
	
	ercd = ipu_3dnr_start_shdr(	bypassMode,				// [In]Bypass mode.
								compMode,				// [In]Compression mode.
								mcMode,					// [In]Mc mode.
								edgeCpy,				// [In]Edge copy.
								edgeCut,				// [In]Edge cut.
								(const	T_SHDR_MNG_IN* const)&in->shdr_in,		// [In]Source image(YUV8).
								(const	T_SHDR_MNG_IN* const)&in->ref_in[0],	// [In]Reference image(YUV8).
								(const	T_SHDR_MNG_IN* const)&in->me_in,		// [In]Motion vector.
								(T_SHDR_MNG_OUT* const)&in->shdr_out[ E_SHDR_OUT_RBK0_H265 ],		// [Out]HEVC enc.
								(T_SHDR_MNG_OUT* const)&in->shdr_out[ E_SHDR_OUT_RBK1_ME ],			// [Out]Motion vector.
								(T_SHDR_MNG_OUT* const)&in->shdr_out[ E_SHDR_OUT_RBK2_H264_0 ],		// [Out]H264 enc 0.
								(T_SHDR_MNG_OUT* const)&in->shdr_out[ E_SHDR_OUT_RBK3_H264_1 ],		// [Out]H264 enc 1.
								(T_SHDR_MNG_OUT* const)&in->shdr_out[ E_SHDR_OUT_RBK4_YUV_0 ],		// [Out]YUV 0.
								(T_SHDR_MNG_OUT* const)&in->shdr_out[ E_SHDR_OUT_RBK5_YUV_1 ],		// [Out]YUV 1.
								(T_SHDR_MNG_OUT* const)&in->cmptbl_out,			// [Out]Comp table write.
								(T_SHDR_MNG_IN* const)&in->cmptbl_in,			// [In]Comp table read.
								cbcr_global_x2); // For HDMI in case, make YUV422 in image to YUV420 in image
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);

		// ReUse SHDR
		if(in->vhdr_on == TRUE)
		{
			ercd = Im_SHDR_Close();
			if ( ercd != D_DDIM_OK ) {
				UPRINTF_ERR((INT32)ercd);
			}
			OS_User_Sig_Sem( SID_IP_SHDR );
		}
				
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_3DNR, FJ_MBALOG_TYPE_END, "SHDR Start"));
		return;
	}

	//save shdr reference image
	in->ref_in[0] = ref_in_bak;
	in->cmptbl_in = cmp_in_back;
	
	// ReUse SHDR
	if(in->vhdr_on == TRUE)
	{
		ercd = Im_SHDR_Close();
		if ( ercd != D_DDIM_OK ) {
			UPRINTF_ERR((INT32)ercd);
		}
		OS_User_Sig_Sem( SID_IP_SHDR );
	}
			
	#ifdef D_IPU_3DNR_PROCESS_BY_TASK
		#ifdef DEBUG_SAVE_IMG
		{
			static UCHAR d_frame_cnt = 0;
			ULONG c_offs = 0;
			ULONG size = 0;
		
			if(d_frame_cnt == 30)
			{
				c_offs = in->shdr_out[E_SHDR_OUT_RBK1_ME].mem_addr_c - in->shdr_out[E_SHDR_OUT_RBK1_ME].mem_addr_y;
				size = c_offs - in->shdr_out[E_SHDR_OUT_RBK1_ME].yuv_h_size * in->shdr_out[E_SHDR_OUT_RBK1_ME].yuv_v_size + 16;
				size = size + in->shdr_out[E_SHDR_OUT_RBK1_ME].yuv_h_size * in->shdr_out[E_SHDR_OUT_RBK1_ME].yuv_v_size * 2;
				debug_save_file((CHAR *)"I:\\SHDR_B1.YUV", in->shdr_out[E_SHDR_OUT_RBK1_ME].mem_addr_y, size);
				UPRINTF("Size: %d x %d, CbCr Addr Offset:0x%08lx\n", in->shdr_out[E_SHDR_OUT_RBK1_ME].yuv_h_size, in->shdr_out[E_SHDR_OUT_RBK1_ME].yuv_v_size, c_offs);
			}
			else if(d_frame_cnt == 60)
			{
				c_offs = in->shdr_out[E_SHDR_OUT_RBK4_YUV_0].mem_addr_c - in->shdr_out[E_SHDR_OUT_RBK4_YUV_0].mem_addr_y;
				size = c_offs - in->shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_h_size * in->shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_v_size + 16;
				size = size + in->shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_h_size * in->shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_v_size * 2;
				debug_save_file((CHAR *)"I:\\SHDR_B4.YUV", in->shdr_out[E_SHDR_OUT_RBK4_YUV_0].mem_addr_y, size);
				UPRINTF("Size: %d x %d, CbCr Addr Offset:0x%08lx\n", in->shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_h_size, in->shdr_out[E_SHDR_OUT_RBK4_YUV_0].yuv_v_size, c_offs);
			}
			else if(d_frame_cnt >= 90)
			{
				d_frame_cnt = 90;
			}
		
			d_frame_cnt++;
		}
		#endif
	#endif
	
}

void Close_SHDR_Proc( void )
{
	INT32 ercd;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_3DNR, FJ_MBALOG_TYPE_POINT, "SHDR Close"));
	
	ercd = Im_SHDR_Close();
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return;
	}
	
}

void Debug_3DNR( ULONG prm0, ULONG prm1 )
{
	g_debug_3dnr.en						= (prm0 & 0x00000001);
	g_debug_3dnr.force_fmd_off			= (prm0 & 0x00000002) >> 1;
	g_debug_3dnr.force_mc_off			= (prm0 & 0x00000004) >> 2;
	g_debug_3dnr.force_chg_blend		= (prm0 & 0x00000008) >> 3;
	g_debug_3dnr.force_chg_me_shift		= (prm0 & 0x00000010) >> 4;
	g_debug_3dnr.force_chg_shdr_shift	= (prm0 & 0x00000020) >> 5;
	g_debug_3dnr.force_chg_me_out		= (prm0 & 0x00000040) >> 6;
	
	g_debug_3dnr.force_blend_ratio		= (prm0 & 0x0000FF00) >> 8;
	g_debug_3dnr.force_me_out_y			= (prm0 & 0x00FF0000) >> 16;
	g_debug_3dnr.force_me_out_x			= (prm0 & 0xFF000000) >> 24;
	
	g_debug_3dnr.force_shift_me_y		= (prm1 & 0x000000FF) >> 0;
	g_debug_3dnr.force_shift_me_x		= (prm1 & 0x0000FF00) >> 8;
	g_debug_3dnr.force_shift_shdr_y		= (prm1 & 0x00FF0000) >> 16;
	g_debug_3dnr.force_shift_shdr_x		= (prm1 & 0xFF000000) >> 24;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/

static INT32 ipu_3dnr_set_gmd( ULONG heap_addr, const T_SHDR_MNG_OUT* img_red, const T_SHDR_MNG_OUT* img_full  )
{
	INT32				ercd;
	IPC_ULONG			heapSize = D_IPU_GMD_HEAP_SIZE;
	T_IPC_GMD_CTRL_PRM	ctrlPrm;

	memset ( &ctrlPrm, 0x0, sizeof(T_IPC_GMD_CTRL_PRM) );

	ctrlPrm.heap_addr		= heap_addr;
	ctrlPrm.width			= img_red->yuv_h_size;
	ctrlPrm.height			= img_red->yuv_v_size;
	ctrlPrm.full_width		= img_full->yuv_h_size;
	ctrlPrm.full_height		= img_full->yuv_v_size;

	ercd = IPC_GMD_Ctrl( &heapSize, &ctrlPrm );
	if( ercd != D_IPC_GMD_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

	return D_IPC_GMD_OK;
}

static INT32 ipu_3dnr_set_me( void )
{
	INT32				ercd;

	gIpu_3dnr_Me_Ctrl_Prm.pCallBack = (T_IM_ME_CALLBACK)ipu_3dnr_me_irq_callback;
	
	ercd = Im_ME_Ctrl( (T_IM_ME_CTRL*)&gIpu_3dnr_Me_Ctrl_Prm );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
	}
	return ercd;
}

static INT32 ipu_3dnr_set_shdr( BOOL set_degamma )
{
	T_SHARE_SYSTEM_CONFIG cfg;
	INT32	ercd;
	
#if 1	// W/A for EPK-1754 (To avoid SHDR Resize Issue, HDR output is -127~127 ( instead of -128~127 ) )
	if( gIpu_3dnr_shdr_Pre_Prm.r2y_clp.lower_short_c[ 0 ] < (-127*16) ){
		gIpu_3dnr_shdr_Pre_Prm.r2y_clp.lower_short_c[ 0 ] = (-127*16);
	}
	if( gIpu_3dnr_shdr_Pre_Prm.r2y_clp.lower_short_c[ 1 ] < (-127*16) ){
		gIpu_3dnr_shdr_Pre_Prm.r2y_clp.lower_short_c[ 1 ] = (-127*16);
	}
	if( gIpu_3dnr_shdr_Pre_Prm.r2y_clp.lower_long_c [ 0 ] < (-127*16) ){
		gIpu_3dnr_shdr_Pre_Prm.r2y_clp.lower_long_c [ 0 ] = (-127*16);
	}
	if( gIpu_3dnr_shdr_Pre_Prm.r2y_clp.lower_long_c [ 1 ] < (-127*16) ){
		gIpu_3dnr_shdr_Pre_Prm.r2y_clp.lower_long_c [ 1 ] = (-127*16);
	}
#endif

	Get_Share_System_Config(&cfg);
	if (cfg.yc_bt == E_YC_BT_601 || cfg.yc_bt == E_YC_BT_IQ) {
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[0] = 256;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[1] = 0;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[2] = 358;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[3] = 256;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[4] = -88;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[5] = -184;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[6] = 256;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[7] = 464;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[8] = -2;

		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[0] = 77;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[1] = 150;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[2] = 29;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[3] = -43;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[4] = -85;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[5] = 128;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[6] = 128;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[7] = -107;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[8] = -21;
	} else if (cfg.yc_bt == E_YC_BT_709) {
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[0] = 256;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[1] = 0;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[2] = 404;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[3] = 256;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[4] = -48;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[5] = -120;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[6] = 256;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[7] = 475;
		gIpu_3dnr_shdr_Pre_Prm.y2r_coef[8] = 0;

		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[0] = 54;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[1] = 183;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[2] = 18;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[3] = -29;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[4] = -99;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[5] = 128;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[6] = 128;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[7] = -116;
		gIpu_3dnr_shdr_Pre_Prm.r2y_coef[8] = -12;
	}

	ercd = Im_SHDR_Set_Pre( (T_IM_SHDR_PRE*)&gIpu_3dnr_shdr_Pre_Prm );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

	ercd = Im_SHDR_Set_Fmd( (T_IM_SHDR_FMD*)&gIpu_3dnr_shdr_Fmd_Prm );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

	ercd = Im_SHDR_Set_PreMask( (T_IM_SHDR_PMSK*)&gIpu_3dnr_shdr_Pmsk_Prm );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

	ercd = Im_SHDR_Set_Spnr( (T_IM_SHDR_SPNR*)&gIpu_3dnr_shdr_Spnr_Prm );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

	ercd = Im_SHDR_Set_Mask( (T_IM_SHDR_MSK*)&gIpu_3dnr_shdr_Msk_Prm );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

	ercd = Im_SHDR_Set_Blend( (T_IM_SHDR_BLD*)&gIpu_3dnr_shdr_Bld_Prm );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

	ercd = Im_SHDR_Set_Smc( (T_IM_SHDR_SMC*)&gIpu_3dnr_shdr_Smc_Prm );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

	ercd = Im_SHDR_Set_Axi( (T_IM_SHDR_AXI*)&gIpu_3dnr_shdr_Axi_Prm );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

	if ( set_degamma ) {
		ercd = Im_SHDR_Set_Degamma( (USHORT*)&gIpu_3dnr_shdr_Degamma_Prm[ 0 ][ 0 ],
									(USHORT*)&gIpu_3dnr_shdr_Degamma_Prm[ 1 ][ 0 ],
									(USHORT*)&gIpu_3dnr_shdr_Degamma_Prm[ 2 ][ 0 ],
									0,
									D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
		if( ercd != D_DDIM_OK ) {
			UPRINTF_ERR((INT32)ercd);
			return ercd;
		}
	}

	return ercd;
}

static INT32 ipu_3dnr_start_iip(	T_SHDR_MNG_OUT* const imgSrc, T_SHDR_MNG_OUT* const imgDst )
{
	FJ_IMG_CONV_CTRL		img_conv_ctl;
	
	memset( &img_conv_ctl, 0, sizeof(img_conv_ctl) );
	
	img_conv_ctl.src_img.addr.Y_G			= imgSrc->mem_addr_y;
	img_conv_ctl.src_img.addr.Cb_B			= imgSrc->mem_addr_c;
	img_conv_ctl.src_img.addr.Cr_R			= 0;
	img_conv_ctl.src_img.addr.Alpha			= 0;
	img_conv_ctl.src_img.g_y_g_width		= imgSrc->yuv_h_byte;
	img_conv_ctl.src_img.g_cb_b_width		= imgSrc->yuv_h_byte;
	img_conv_ctl.src_img.g_cr_r_width		= 0;
	img_conv_ctl.src_img.g_alpha_width		= 0;
	img_conv_ctl.src_img.width				= imgSrc->yuv_h_size;
	img_conv_ctl.src_img.lines				= imgSrc->yuv_v_size;
	img_conv_ctl.src_img.line_bytes.Y_G		= imgSrc->yuv_h_byte;
	img_conv_ctl.src_img.line_bytes.Cb_B	= imgSrc->yuv_h_byte;
	img_conv_ctl.src_img.line_bytes.Cr_R	= 0;
	img_conv_ctl.src_img.line_bytes.Alpha	= 0;

	if(imgSrc->yuv_format == E_SHARE_YUV_FORMAT_NONE)
		img_conv_ctl.src_img.pfmt			= FJ_IMG_PFMT_VIDEO;
	else if(imgSrc->yuv_format == E_SHARE_YUV_FORMAT_422_SEMI)
		img_conv_ctl.src_img.pfmt			= FJ_IMG_PFMT_YCC422;
	else
		img_conv_ctl.src_img.pfmt			= FJ_IMG_PFMT_YCC420;

	img_conv_ctl.dst_img.addr.Y_G			= imgDst->mem_addr_y;
	img_conv_ctl.dst_img.addr.Cb_B			= imgDst->mem_addr_c;
	img_conv_ctl.dst_img.addr.Cr_R			= 0;
	img_conv_ctl.dst_img.addr.Alpha			= 0;
	img_conv_ctl.dst_img.g_y_g_width		= imgDst->yuv_h_byte;
	img_conv_ctl.dst_img.g_cb_b_width		= imgDst->yuv_h_byte;
	img_conv_ctl.dst_img.g_cr_r_width		= 0;
	img_conv_ctl.dst_img.g_alpha_width		= 0;
	img_conv_ctl.dst_img.width				= imgDst->yuv_h_size;
	img_conv_ctl.dst_img.lines				= imgDst->yuv_v_size;
	img_conv_ctl.dst_img.line_bytes.Y_G		= imgDst->yuv_h_byte;
	img_conv_ctl.dst_img.line_bytes.Cb_B	= imgDst->yuv_h_byte;
	img_conv_ctl.dst_img.line_bytes.Cr_R	= 0;
	img_conv_ctl.dst_img.line_bytes.Alpha	= 0;

	if(imgDst->yuv_format == E_SHARE_YUV_FORMAT_NONE)
		img_conv_ctl.dst_img.pfmt			= FJ_IMG_PFMT_VIDEO;
	else if(imgDst->yuv_format == E_SHARE_YUV_FORMAT_422_SEMI)
		img_conv_ctl.dst_img.pfmt			= FJ_IMG_PFMT_YCC422;
	else
		img_conv_ctl.dst_img.pfmt			= FJ_IMG_PFMT_YCC420;

	img_conv_ctl.fill_enable				= 0;
	img_conv_ctl.resize_mode				= FJ_IMG_RESIZE_MODE_BILINEAR;
	img_conv_ctl.rotate_deg 				= FJ_IMG_ROTATE_DEGREE_0;
	img_conv_ctl.timeout					= 100;
	
	BF_Still_Image_Convert_2_Init( &img_conv_ctl, NULL );
	BF_Still_Image_Convert_2_Execute( &img_conv_ctl );
	BF_Still_Image_Convert_2_Close();

	return D_DDIM_OK;
}

static INT32 ipu_3dnr_start_gmd(E_SHARE_OP_3DNR_MODE mode,
										BOOL bypassMode,
										BOOL brcEnable,
										BOOL brcMode,
										const T_SHDR_MNG_OUT* const imgtSrcFull,
										const T_SHDR_MNG_OUT* const imgtSrc,
										const T_SHDR_MNG_OUT* const imgtRefFull,
										const T_SHDR_MNG_OUT* const imgtRef,
										T_SHDR_MOTION* const motion )
{
	INT32 ercd;
	T_IPC_GMD_START_PRM		start_prm;
	T_IPC_GMD_EXP_PRM		exp_prm;
	INT32					shiftX = 0, shiftY = 0;

	memset ( &start_prm, 0x0, sizeof(T_IPC_GMD_START_PRM) );
	memset ( &exp_prm,   0x0, sizeof(T_IPC_GMD_EXP_PRM) );

	// Bypass mode check(or first start check).
	if ( ( bypassMode ) || ( imgtRefFull->mem_addr_y == 0 ) ) {
		// skip.
		shiftX = 0;
		shiftY = 0;
	}
	else {
		if(mode == E_SHARE_OP_3DNR_MODE_FPT) {
			exp_prm.brc_en						= (IPC_UCHAR)brcEnable;				// Brightness correct enable.
			exp_prm.brc_mod						= (IPC_UCHAR)brcMode;				// Brightness correct mode.
			exp_prm.lut_sel						= 0;								// LUT table select(LUT0).
			exp_prm.gainup_tbl0					= &gIpu_3dnr_shdr_Gainup_Tbl0[ 0 ];		// LUT0 table start address.
			exp_prm.gainup_tbl1					= &gIpu_3dnr_shdr_Gainup_Tbl1[ 0 ];		// LUT1 table start address.

			start_prm.sel_affin					= 0;								// shift calc.
			start_prm.shift_permissible_range	= 4;								// 4%.
			start_prm.affin_permissible_degree	= 2;								// 2 degree.
			start_prm.affin_permissible_expand	= 2;								// 2%.

			// red base image
			if(imgtSrc->yuv_format == E_SHARE_YUV_FORMAT_422_SEMI)
				start_prm.base_img.imgForm	= E_IPC_IMAGE_FORM_422SP;
			else
				start_prm.base_img.imgForm	= E_IPC_IMAGE_FORM_420SP;
			start_prm.base_img.imgInfo[0].pImgAdrs = (IPC_VOID *)imgtSrc->mem_addr_y;
			start_prm.base_img.imgInfo[1].pImgAdrs = (IPC_VOID *)imgtSrc->mem_addr_c;
			start_prm.base_img.imgInfo[0].gImgSize = imgtSrc->yuv_h_byte;
			start_prm.base_img.imgInfo[1].gImgSize = imgtSrc->yuv_h_byte;
			start_prm.base_img.imgInfo[0].wImgSize = imgtSrc->yuv_h_size;
			start_prm.base_img.imgInfo[1].wImgSize = imgtSrc->yuv_h_size;
			start_prm.base_img.imgInfo[0].hImgSize = imgtSrc->yuv_v_size;
			start_prm.base_img.imgInfo[1].hImgSize = imgtSrc->yuv_v_size;
			start_prm.base_img.bitDepth = 8;
			start_prm.base_img.dataForm = E_IPC_IMAGE_DATA_FORM_PACK;
			// red ref image
			if(imgtRef->yuv_format == E_SHARE_YUV_FORMAT_422_SEMI)
				start_prm.target_img.imgForm	= E_IPC_IMAGE_FORM_422SP;
			else
				start_prm.target_img.imgForm	= E_IPC_IMAGE_FORM_420SP;
			start_prm.target_img.imgInfo[0].pImgAdrs = (IPC_VOID *)imgtRef->mem_addr_y;
			start_prm.target_img.imgInfo[1].pImgAdrs = (IPC_VOID *)imgtRef->mem_addr_c;
			start_prm.target_img.imgInfo[0].gImgSize = imgtRef->yuv_h_byte;
			start_prm.target_img.imgInfo[1].gImgSize = imgtRef->yuv_h_byte;
			start_prm.target_img.imgInfo[0].wImgSize = imgtRef->yuv_h_size;
			start_prm.target_img.imgInfo[1].wImgSize = imgtRef->yuv_h_size;
			start_prm.target_img.imgInfo[0].hImgSize = imgtRef->yuv_v_size;
			start_prm.target_img.imgInfo[1].hImgSize = imgtRef->yuv_v_size;
			start_prm.target_img.bitDepth = 8;
			start_prm.target_img.dataForm = E_IPC_IMAGE_DATA_FORM_PACK;
			// full base image
			if(imgtSrcFull->yuv_format == E_SHARE_YUV_FORMAT_422_SEMI)
				start_prm.base_full_img.imgForm	= E_IPC_IMAGE_FORM_422SP;
			else
				start_prm.base_full_img.imgForm	= E_IPC_IMAGE_FORM_420SP;
			start_prm.base_full_img.imgInfo[0].pImgAdrs = (IPC_VOID *)imgtSrcFull->mem_addr_y;
			start_prm.base_full_img.imgInfo[1].pImgAdrs = (IPC_VOID *)imgtSrcFull->mem_addr_c;
			start_prm.base_full_img.imgInfo[0].gImgSize = imgtSrcFull->yuv_h_byte;
			start_prm.base_full_img.imgInfo[1].gImgSize = imgtSrcFull->yuv_h_byte;
			start_prm.base_full_img.imgInfo[0].wImgSize = imgtSrcFull->yuv_h_size;
			start_prm.base_full_img.imgInfo[1].wImgSize = imgtSrcFull->yuv_h_size;
			start_prm.base_full_img.imgInfo[0].hImgSize = imgtSrcFull->yuv_v_size;
			start_prm.base_full_img.imgInfo[1].hImgSize = imgtSrcFull->yuv_v_size;
			start_prm.base_full_img.bitDepth = 8;
			start_prm.base_full_img.dataForm = E_IPC_IMAGE_DATA_FORM_PACK;
			// full ref image
			if(imgtRefFull->yuv_format == E_SHARE_YUV_FORMAT_422_SEMI)
				start_prm.target_full_img.imgForm	= E_IPC_IMAGE_FORM_422SP;
			else
				start_prm.target_full_img.imgForm	= E_IPC_IMAGE_FORM_420SP;
			start_prm.target_full_img.imgInfo[0].pImgAdrs = (IPC_VOID *)imgtRefFull->mem_addr_y;
			start_prm.target_full_img.imgInfo[1].pImgAdrs = (IPC_VOID *)imgtRefFull->mem_addr_c;
			start_prm.target_full_img.imgInfo[0].gImgSize = imgtRefFull->yuv_h_byte;
			start_prm.target_full_img.imgInfo[1].gImgSize = imgtRefFull->yuv_h_byte;
			start_prm.target_full_img.imgInfo[0].wImgSize = imgtRefFull->yuv_h_size;
			start_prm.target_full_img.imgInfo[1].wImgSize = imgtRefFull->yuv_h_size;
			start_prm.target_full_img.imgInfo[0].hImgSize = imgtRefFull->yuv_v_size;
			start_prm.target_full_img.imgInfo[1].hImgSize = imgtRefFull->yuv_v_size;
			start_prm.target_full_img.bitDepth = 8;
			start_prm.target_full_img.dataForm = E_IPC_IMAGE_DATA_FORM_PACK;

			// GMD start.
			ercd = IPC_GMD_Start( &start_prm, &exp_prm );
			if ( ercd != D_IPC_GMD_OK ) {
				motion->shift_x = 0;
				motion->shift_y = 0;
				//UPRINTF_ERR((INT32)ercd);
				if ( ercd != D_IPC_GMD_NG_POINT_SHORTAGE ) {
					UPRINTF_ERR((INT32)ercd);
					return ercd;
				}
			}
			shiftX = start_prm.shift_x / 65536;
			shiftY = start_prm.shift_y / 65536;
		}else{
			shiftX = 0;
			shiftY = 0;
		}
	}
	motion->shift_x = shiftX;
	motion->shift_y = shiftY;
	return D_DDIM_OK;
}

static INT32 ipu_3dnr_start_me(	BOOL bypassMode,
										const T_SHDR_MOTION* const motion,
										const T_SHDR_MNG_OUT* const imgSrc,
										const T_SHDR_MNG_IN* const imgRef,
										T_SHDR_MNG_IN* const imgMe )
{
	INT32 ercd;
	UINT32	wait_factor_result = 0;
	USHORT	vec_hsize;
	
	// Bypass mode check(or first start check).
	if ( ( bypassMode ) || ( imgRef->mem_addr_y == 0 ) ) {
		// skip.
	}
	else {
		if ( imgSrc->yuv_h_size <= D_IM_ME_PRE_VEC_HSIZE_1024 ) {
			vec_hsize = D_IM_ME_PRE_VEC_HSIZE_512;
		}
		else {
			vec_hsize = D_IM_ME_PRE_VEC_HSIZE_1024;
		}

		// Vector output foramt horizontal size set.
		for ( int i = 0; i < E_IM_ME_PPNUM_MAX; i++ ) {
			gIpu_3dnr_Me_Start_Prm.pme[ i ].vec_hsize = vec_hsize;
		}

		// Previous image shift. // clip
		gIpu_3dnr_Me_Start_Prm.rmcif.pre_shift_x		= motion->shift_x;
		gIpu_3dnr_Me_Start_Prm.rmcif.pre_shift_y		= motion->shift_y;

		// Previous image base address(16K byte align).
		gIpu_3dnr_Me_Start_Prm.rmcif.pre_r_addr_hi		= (USHORT)((ULONG)imgRef->mem_addr_y >> 16);
		gIpu_3dnr_Me_Start_Prm.rmcif.pre_r_addr_lo		= (USHORT)((ULONG)imgRef->mem_addr_y & 0x0000FFFF);

		// Current image base address(16K byte align).
		gIpu_3dnr_Me_Start_Prm.rmcif.cur_r_addr_hi		= (USHORT)((ULONG)imgSrc->mem_addr_y >> 16);
		gIpu_3dnr_Me_Start_Prm.rmcif.cur_r_addr_lo		= (USHORT)((ULONG)imgSrc->mem_addr_y & 0x0000FFFF);

		// Vector information base address(1K byte align).
		gIpu_3dnr_Me_Start_Prm.rmcif.vec_w_addr_hi		= (USHORT)((ULONG)imgMe->mem_addr_y >> 16);
		gIpu_3dnr_Me_Start_Prm.rmcif.vec_w_addr_lo		= (USHORT)((ULONG)imgMe->mem_addr_y & 0x0000FFFF);

		// Previous image size.
		gIpu_3dnr_Me_Start_Prm.rmcif.pre_gwidth			= imgRef->yuv_h_byte;
		gIpu_3dnr_Me_Start_Prm.rmcif.pre_width			= imgRef->yuv_h_size;
		gIpu_3dnr_Me_Start_Prm.rmcif.pre_height			= imgRef->yuv_v_size;

		// Current image size.
		gIpu_3dnr_Me_Start_Prm.rmcif.cur_gwidth			= imgSrc->yuv_h_byte;
		gIpu_3dnr_Me_Start_Prm.rmcif.cur_margin_width	= D_IM_ME_CUR_MARGIN_0;
		gIpu_3dnr_Me_Start_Prm.rmcif.cur_margin_height	= D_IM_ME_CUR_MARGIN_0;

		// Prefetch image size.
		gIpu_3dnr_Me_Start_Prm.rmcif.prefetch_width		= gIpu_3dnr_Me_Start_Prm.rmcif.pre_width  * 4;
		gIpu_3dnr_Me_Start_Prm.rmcif.prefetch_height	= gIpu_3dnr_Me_Start_Prm.rmcif.pre_height * 4;

		// ME1 initialize search size.
		if ( gIpu_3dnr_Me_Start_Prm.rmcif.cur_margin_width >= D_IM_ME_CUR_MARGIN_4 ) {
			if ( gIpu_3dnr_Me_Start_Prm.rmcif.pre_width <= D_IM_ME_SRCH_WIDTH_MAX ) {
				gIpu_3dnr_Me_Start_Prm.rmcif.search_width = gIpu_3dnr_Me_Start_Prm.rmcif.pre_width;
			}
			else {
				gIpu_3dnr_Me_Start_Prm.rmcif.search_width = D_IM_ME_SRCH_WIDTH_MAX;
			}
		}
		else {
			if ( ( gIpu_3dnr_Me_Start_Prm.rmcif.pre_width - 8 ) <= D_IM_ME_SRCH_WIDTH_MAX ) {
				gIpu_3dnr_Me_Start_Prm.rmcif.search_width = gIpu_3dnr_Me_Start_Prm.rmcif.pre_width - 8;
			}
			else {
				gIpu_3dnr_Me_Start_Prm.rmcif.search_width = D_IM_ME_SRCH_WIDTH_MAX;
			}
		}

		if ( gIpu_3dnr_Me_Ctrl_Prm.rmcif.pme_num == 2 ) {
			gIpu_3dnr_Me_Start_Prm.rmcif.search_height = 40;
		}
		else if ( gIpu_3dnr_Me_Ctrl_Prm.rmcif.pme_num == 1 ) {
			gIpu_3dnr_Me_Start_Prm.rmcif.search_height = 32;
		}
		else {
			gIpu_3dnr_Me_Start_Prm.rmcif.search_height = 24;
		}

		// MB number set.
		for ( int i = 0; i < E_IM_ME_PPNUM_MAX; i++ ) {
			gIpu_3dnr_Me_Start_Prm.pme[ i ].mbnum_h = gIpu_3dnr_Me_Start_Prm.rmcif.prefetch_width  / 16;
			gIpu_3dnr_Me_Start_Prm.pme[ i ].mbnum_v = gIpu_3dnr_Me_Start_Prm.rmcif.prefetch_height / 16;
		}

		ercd = Im_ME_Start( (T_IM_ME_START*)&gIpu_3dnr_Me_Start_Prm );
		if ( ercd != D_DDIM_OK ) {
			UPRINTF_ERR((INT32)ercd);
			return ercd;
		}

		ercd = Im_ME_Wait_End( &wait_factor_result, 100 );
		if ( ercd != D_DDIM_OK ) {
			UPRINTF_ERR((INT32)ercd);
			return ercd;
		}
	}

	return D_DDIM_OK;
}

static INT32 ipu_3dnr_start_shdr(	BOOL	bypassMode,
								 			BOOL	compMode,
								 			BOOL	mcMode,
								 			BOOL	edgeCpy,
								 			BOOL	edgeCut,
								 			const	T_SHDR_MNG_IN* const imgSrc,
								 			const	T_SHDR_MNG_IN* const imgRef,
								 			const	T_SHDR_MNG_IN* const imgMe,
											T_SHDR_MNG_OUT* const imgHevc,
											T_SHDR_MNG_OUT* const imgMeIn,
											T_SHDR_MNG_OUT* const imgH264_0,
											T_SHDR_MNG_OUT* const imgH264_1,
											T_SHDR_MNG_OUT* const imgYUV_0,
											T_SHDR_MNG_OUT* const imgYUV_1,
											T_SHDR_MNG_OUT* const comp_w,
											T_SHDR_MNG_IN* const comp_r,
											UCHAR cbcr_global_x2)
{
	UINT32	wait_factor_result = 0;
	UCHAR tlut_mc[ 17 ] = { 0xFF, 0xEF, 0xDF, 0xCF, 0xBF, 0xAF, 0x9F, 0x8F, 0x7F, 0x6F, 0x5F, 0x4F, 0x3F, 0x2F, 0x1F, 0x0F, 0x00 };
	USHORT pitch[ 2 ];
	USHORT trmsta[ 2 ] = {0, 0};
	UCHAR rdc_md;
	UINT32 std_width;
	UINT32 std_height;
	INT32 ercd;
	T_SHARE_MOTION_VECTOR	share_motion = {0};
	T_SHDR_MOTION			motion = {0};
	T_SHARE_SYSTEM_CONFIG	cfg;

	Get_Share_System_Config(&cfg);
	// Bypass mode check(or first start check).
	if ( ( bypassMode ) || ( imgRef->mem_addr_y == 0 ) ) {
		// FLWCNT.
		gIpu_3dnr_shdr_Ctrl_Prm.image_mode.bypass_en		= D_IM_SHDR_ENABLE;
		gIpu_3dnr_shdr_Ctrl_Prm.image_mode.shdr_in			= D_IM_SHDR_SHDRIN_STN_REF;
		// SHDRIFRM.
		gIpu_3dnr_shdr_Ctrl_Prm.image_set.in_edge_cpy		= FALSE;
		if (imgHevc->mem_addr_y != 0) {
			if ( imgSrc->yuv_h_size != imgHevc->yuv_h_size ) {
				gIpu_3dnr_shdr_Ctrl_Prm.image_set.in_edge_cut	= TRUE;
			}
			else {
				gIpu_3dnr_shdr_Ctrl_Prm.image_set.in_edge_cut	= FALSE;
			}
		}
		else {
			gIpu_3dnr_shdr_Ctrl_Prm.image_set.in_edge_cut	= FALSE;
		}
		// POSTSEL0.
		gIpu_3dnr_shdr_Post_Prm.css_sel						= D_IM_SHDR_POSTSEL0_BYPASS_YCC8;
		// COMPIP.
		if ( compMode && comp_w->mem_addr_y && comp_w->mem_addr_c) {
			gIpu_3dnr_shdr_Ctrl_Prm.compip.r_bypass_mode	= D_IM_SHDR_ENABLE;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.w_bypass_mode	= D_IM_SHDR_DISABLE;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.w_addr_y			= comp_w->mem_addr_y;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.w_addr_c			= comp_w->mem_addr_c;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.r_addr_y			= 0x00000000;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.r_addr_c			= 0x00000000;
		}
		else {
			gIpu_3dnr_shdr_Ctrl_Prm.compip.r_bypass_mode	= D_IM_SHDR_ENABLE;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.w_bypass_mode	= D_IM_SHDR_ENABLE;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.w_addr_y			= 0x00000000;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.w_addr_c			= 0x00000000;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.r_addr_y			= 0x00000000;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.r_addr_c			= 0x00000000;
		}
		gIpu_3dnr_shdr_Ctrl_Prm.compip.mc_mode				= D_IM_SHDR_COMP_MC_UNUSED;
		gIpu_3dnr_shdr_Ctrl_Prm.image_mode.shift_x			= 0;
		gIpu_3dnr_shdr_Ctrl_Prm.image_mode.shift_y			= 0;

		memset( (void*)&gIpu_3dnr_shdr_Pmsk_Prm.tlut, 0x00, sizeof(tlut_mc) );
	}
	else {
		// FLWCNT.
		gIpu_3dnr_shdr_Ctrl_Prm.image_mode.bypass_en		= D_IM_SHDR_DISABLE;
		// SHDRIFRM.
		gIpu_3dnr_shdr_Ctrl_Prm.image_set.in_edge_cpy		= edgeCpy;
		gIpu_3dnr_shdr_Ctrl_Prm.image_set.in_edge_cut		= edgeCut;

		if ( mcMode == FALSE ) {
			gIpu_3dnr_shdr_Ctrl_Prm.compip.mc_mode			= D_IM_SHDR_COMP_MC_UNUSED;
		}
		else {
			gIpu_3dnr_shdr_Ctrl_Prm.compip.mc_mode			= D_IM_SHDR_COMP_MC_USED;
		}
		
		// POSTSEL0.
		gIpu_3dnr_shdr_Post_Prm.css_sel						= D_IM_SHDR_POSTSEL0_BLEND_YCC16;
		// COMPIP.
		if ( compMode && comp_w->mem_addr_y && comp_w->mem_addr_c) {
			gIpu_3dnr_shdr_Ctrl_Prm.compip.r_bypass_mode	= D_IM_SHDR_DISABLE;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.w_bypass_mode	= D_IM_SHDR_DISABLE;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.w_addr_y			= comp_w->mem_addr_y;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.w_addr_c			= comp_w->mem_addr_c;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.r_addr_y			= comp_r->mem_addr_y;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.r_addr_c			= comp_r->mem_addr_c;
		}
		else {
			gIpu_3dnr_shdr_Ctrl_Prm.compip.r_bypass_mode	= D_IM_SHDR_ENABLE;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.w_bypass_mode	= D_IM_SHDR_ENABLE;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.w_addr_y			= 0x00000000;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.w_addr_c			= 0x00000000;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.r_addr_y			= 0x00000000;
			gIpu_3dnr_shdr_Ctrl_Prm.compip.r_addr_c			= 0x00000000;
		}

		// ME/MC On.
		if ( mcMode == FALSE ) {
			gIpu_3dnr_shdr_Ctrl_Prm.image_mode.shdr_in	= D_IM_SHDR_SHDRIN_STN_REF;
			
			memset( (void*)&gIpu_3dnr_shdr_Pmsk_Prm.tlut, 0x00, sizeof(tlut_mc) );
		}
		else {
			gIpu_3dnr_shdr_Ctrl_Prm.image_mode.shdr_in	= D_IM_SHDR_SHDRIN_STN_SMCOUT;
			
			memcpy( &gIpu_3dnr_shdr_Pmsk_Prm.tlut, tlut_mc, sizeof(tlut_mc) );
		}
		// SDRAM for read (MC input standard).
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_addr.c_y		= imgSrc->mem_addr_y;		// Src image.
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_addr.c_c		= imgSrc->mem_addr_c;		// Src image.
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_ghsize.c_y		= imgSrc->yuv_h_byte;		// Src image.
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_ghsize.c_c		= imgSrc->yuv_h_byte << cbcr_global_x2;		// Src image.
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_hsize.c			= imgSrc->yuv_h_size;		// Src image.
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_vsize.c			= imgSrc->yuv_v_size;		// Src image.
		// SDRAM for read (MC input reference).
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_addr.a_y		= imgRef->mem_addr_y;		// Ref image.
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_addr.a_c		= imgRef->mem_addr_c;		// Ref image.
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_ghsize.a_y		= imgRef->yuv_h_byte;		// Ref image.
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_ghsize.a_c		= imgRef->yuv_h_byte << cbcr_global_x2;		// Ref image.
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_hsize.a			= imgRef->yuv_h_size;		// Ref image.
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_vsize.a			= imgRef->yuv_v_size;		// Ref image.
		// SDRAM for read (Motion vector).
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_addr.d_y		= imgMe->mem_addr_y;			// Motion vector.
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_addr.d_c		= imgMe->mem_addr_y;			// Motion vector.
		if ( ( imgSrc->yuv_h_size >> 2 ) <= D_IM_ME_PRE_VEC_HSIZE_1024 ) {
			gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_ghsize.d_y	= D_IM_ME_PRE_VEC_HSIZE_512;	// Motion vector.
			gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_ghsize.d_c	= D_IM_ME_PRE_VEC_HSIZE_512;	// Motion vector.
		}
		else {
			gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_ghsize.d_y	= D_IM_ME_PRE_VEC_HSIZE_1024;	// Motion vector.
			gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_ghsize.d_c	= D_IM_ME_PRE_VEC_HSIZE_1024;	// Motion vector.
		}
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_hsize.d			= imgSrc->yuv_h_size;			// Motion vector.
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_vsize.d			= imgSrc->yuv_v_size;			// Motion vector.
		
		// Share
		Get_Frame_Share_Motion_Vector( &share_motion, gFrame_No_Shdr );
		motion.shift_x = share_motion.x;
		motion.shift_y = share_motion.y;
		ipu_3dnr_me_shdr_calc_shift( (const T_SHDR_MNG_OUT* const)imgSrc,
									 (const T_SHDR_MNG_OUT* const)imgRef, FALSE, &motion );
		gIpu_3dnr_shdr_Ctrl_Prm.image_mode.shift_x = motion.shift_x;
		gIpu_3dnr_shdr_Ctrl_Prm.image_mode.shift_y = motion.shift_y;
	}
	
	if( imgHevc->yuv_video == E_SHARE_YUV_VIDEO_NONE) {
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_vfm.a		= D_IM_SHDR_VFMT_RASTER;
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_vfm.a		= D_IM_SHDR_VFMT_COMPIP_RASTER;
	}
	else{
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_vfm.a		= D_IM_SHDR_VFMT_VIDEO;
		gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_vfm.a		= D_IM_SHDR_VFMT_COMPIP_VIDEO;
	}

	// Change Param for 422/420
	if ( imgSrc->yuv_format== E_SHARE_YUV_FORMAT_422_SEMI ) {
		gIpu_3dnr_shdr_Pre_Prm.pre_mode.cus_mode = D_IM_SHDR_CUS_YCC422_BILINEAR_B;
		
		if( imgYUV_0->yuv_format == E_SHARE_YUV_FORMAT_422_SEMI || imgYUV_1->yuv_format == E_SHARE_YUV_FORMAT_422_SEMI ){
			// if input = output = 422, strange line appears. r burst = 256 can avoid.
			gIpu_3dnr_shdr_Axi_Prm.ax_mode.r_burst = D_IM_SHDR_BURSTLEN_256;
		}else{
			gIpu_3dnr_shdr_Axi_Prm.ax_mode.r_burst = D_IM_SHDR_BURSTLEN_512;
		}
		
	}else{
		gIpu_3dnr_shdr_Pre_Prm.pre_mode.cus_mode = D_IM_SHDR_CUS_YCC420_BILINEAR_B;
		gIpu_3dnr_shdr_Axi_Prm.ax_mode.r_burst = D_IM_SHDR_BURSTLEN_512;
	}
	ercd = ipu_3dnr_set_shdr( FALSE );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

	if ( imgSrc->yuv_format== E_SHARE_YUV_FORMAT_420_SEMI ) {
		gIpu_3dnr_shdr_Ctrl_Prm.image_set.in_c_fmt	= D_IM_SHDR_INCFMT_420SP;
	}
	else {
		gIpu_3dnr_shdr_Ctrl_Prm.image_set.in_c_fmt	= D_IM_SHDR_INCFMT_422SP;
	}

	gIpu_3dnr_shdr_Ctrl_Prm.image_mode.fil_margin	= D_IM_SHDR_FLITER_MARGIN_16;
	
	if ( gIpu_3dnr_shdr_Ctrl_Prm.image_set.in_edge_cut == FALSE ) {
		std_width	= imgSrc->yuv_h_size;
		std_height	= imgSrc->yuv_v_size;
	} else {
		std_width	= imgSrc->yuv_h_size - gIpu_3dnr_shdr_Ctrl_Prm.image_mode.fil_margin * 2;
		std_height	= imgSrc->yuv_v_size - gIpu_3dnr_shdr_Ctrl_Prm.image_mode.fil_margin * 2;
	}

	// SDRAM for read(Standard).
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_addr.b_y	= imgSrc->mem_addr_y;		// Src image.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_addr.b_c	= imgSrc->mem_addr_c;		// Src image.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_ghsize.b_y	= imgSrc->yuv_h_byte;		// Src image.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_ghsize.b_c	= imgSrc->yuv_h_byte << cbcr_global_x2;		// Src image.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_hsize.b		= imgSrc->yuv_h_size;		// Src image.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.r_vsize.b		= imgSrc->yuv_v_size;		// Src image.
	// SDRAM for write.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_baddr.a_y	= imgHevc->mem_addr_y;		// HEVC enc.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_baddr.a_c	= imgHevc->mem_addr_c;		// HEVC enc.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_hsize.a		= imgHevc->yuv_h_byte;		// HEVC enc.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_addr.b_y	= imgMeIn->mem_addr_y;		// Me input.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_addr.b_c	= imgMeIn->mem_addr_c;		// Me input.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_ghsize.b_y	= imgMeIn->yuv_h_byte;		// Me input.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_ghsize.b_c	= imgMeIn->yuv_h_byte;		// Me input.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_baddr.c_y	= imgH264_0->mem_addr_y;	// H264 enc 0.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_baddr.c_c	= imgH264_0->mem_addr_c;	// H264 enc 0.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_hsize.c		= imgH264_0->yuv_h_byte;	// H264 enc 0.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_baddr.d_y	= imgH264_1->mem_addr_y;	// H264 enc 1.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_baddr.d_c	= imgH264_1->mem_addr_c;	// H264 enc 1.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_hsize.d		= imgH264_1->yuv_h_byte;	// H264 enc 1.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_addr.e_y	= imgYUV_0->mem_addr_y;		// YUV 0.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_addr.e_c	= imgYUV_0->mem_addr_c;		// YUV 0.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_ghsize.e_y	= imgYUV_0->yuv_h_byte;		// YUV 0.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_ghsize.e_c	= imgYUV_0->yuv_h_byte;		// YUV 0.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_addr.f_y	= imgYUV_1->mem_addr_y;		// YUV 1.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_addr.f_c	= imgYUV_1->mem_addr_c;		// YUV 1.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_ghsize.f_y	= imgYUV_1->yuv_h_byte;		// YUV 1.
	gIpu_3dnr_shdr_Ctrl_Prm.sdram.w_ghsize.f_c	= imgYUV_1->yuv_h_byte;		// YUV 1.

	// Resize.
	// HEVC enc.
	if (imgHevc->mem_addr_y == 0) {
		gIpu_3dnr_shdr_Resize_Prm.chmode.a_w_en	= D_IM_SHDR_DISABLE;				// HEVC enc.
	}
	else {
		gIpu_3dnr_shdr_Resize_Prm.chmode.a_w_en	= D_IM_SHDR_ENABLE;					// HEVC enc.
	}
	gIpu_3dnr_shdr_Resize_Prm.chmode.a_drc_en	= cfg.yc_range == E_YC_RANGE_FULL ? D_IM_SHDR_DISABLE : D_IM_SHDR_ENABLE; // HEVC enc.
	if( gIpu_3dnr_shdr_Resize_Prm.chmode.a_drc_en ) {
		gIpu_3dnr_shdr_Resize_Prm.drcofst.a_y = 16;
		gIpu_3dnr_shdr_Resize_Prm.drcgain.a_y = 0x1B8;
		gIpu_3dnr_shdr_Resize_Prm.drcclph.a_y = 235;
		gIpu_3dnr_shdr_Resize_Prm.drcclpl.a_y = 16;

		gIpu_3dnr_shdr_Resize_Prm.drcofst.a_cb = 0;
		gIpu_3dnr_shdr_Resize_Prm.drcgain.a_cb = 113;
		gIpu_3dnr_shdr_Resize_Prm.drcclph.a_cb = 112;
		gIpu_3dnr_shdr_Resize_Prm.drcclpl.a_cb = -112;

		gIpu_3dnr_shdr_Resize_Prm.drcofst.a_cr = 0;
		gIpu_3dnr_shdr_Resize_Prm.drcgain.a_cr = 113;
		gIpu_3dnr_shdr_Resize_Prm.drcclph.a_cr = 112;
		gIpu_3dnr_shdr_Resize_Prm.drcclpl.a_cr = -112;
	}

	gIpu_3dnr_shdr_Resize_Prm.chmode.a_css_md	= D_IM_SHDR_CSSMD_YCC420_CENTER;	// HEVC enc.
	gIpu_3dnr_shdr_Resize_Prm.woutsz.a_hsize	= imgHevc->yuv_h_size;				// HEVC enc.
	gIpu_3dnr_shdr_Resize_Prm.woutsz.a_vsize	= imgHevc->yuv_v_size;				// HEVC enc.

	// Me input.
	if (imgMeIn->mem_addr_y == 0) {
		gIpu_3dnr_shdr_Resize_Prm.chmode.b_w_en	= D_IM_SHDR_DISABLE;				// Me input.
	}
	else {
		gIpu_3dnr_shdr_Resize_Prm.chmode.b_w_en	= D_IM_SHDR_ENABLE;					// Me input.
		if( imgMeIn->yuv_format == E_SHARE_YUV_FORMAT_420_SEMI ){
			gIpu_3dnr_shdr_Resize_Prm.chmode.b_css_md	= D_IM_SHDR_CSSMD_YCC420_ODD;
		}
		else{
			gIpu_3dnr_shdr_Resize_Prm.chmode.b_css_md	= D_IM_SHDR_CSSMD_YCC422;
		}
	}
	gIpu_3dnr_shdr_Resize_Prm.chmode.b_rdc_md	= D_IM_RDCMD1_1_4;					// Me input.
	gIpu_3dnr_shdr_Resize_Prm.woutsz.b_hsize	= imgMeIn->yuv_h_size;				// Me input.
	gIpu_3dnr_shdr_Resize_Prm.woutsz.b_vsize	= imgMeIn->yuv_v_size;				// Me input.

	// H264 enc 0.
	if (imgH264_0->mem_addr_y == 0) {
		gIpu_3dnr_shdr_Resize_Prm.chmode.c_w_en	= D_IM_SHDR_DISABLE;				// H264 enc 0.
	}
	else {
		gIpu_3dnr_shdr_Resize_Prm.chmode.c_w_en	= D_IM_SHDR_ENABLE;					// H264 enc 0.
	}
	gIpu_3dnr_shdr_Resize_Prm.chmode.c_rsz_md	= D_IM_RSZMD_ODD0;					// H264 enc 0.
	gIpu_3dnr_shdr_Resize_Prm.chmode.c_trm_en	= D_IM_SHDR_ENABLE;					// H264 enc 0.
	gIpu_3dnr_shdr_Resize_Prm.chmode.c_drc_en	= cfg.yc_range == E_YC_RANGE_FULL ? D_IM_SHDR_DISABLE : D_IM_SHDR_ENABLE; // H264 enc 0.

	if( gIpu_3dnr_shdr_Resize_Prm.chmode.c_drc_en ) {
		gIpu_3dnr_shdr_Resize_Prm.drcofst.c_y = 16;
		gIpu_3dnr_shdr_Resize_Prm.drcgain.c_y = 0x1B8;
		gIpu_3dnr_shdr_Resize_Prm.drcclph.c_y = 235;
		gIpu_3dnr_shdr_Resize_Prm.drcclpl.c_y = 16;

		gIpu_3dnr_shdr_Resize_Prm.drcofst.c_cb = 0;
		gIpu_3dnr_shdr_Resize_Prm.drcgain.c_cb = 113;
		gIpu_3dnr_shdr_Resize_Prm.drcclph.c_cb = 112;
		gIpu_3dnr_shdr_Resize_Prm.drcclpl.c_cb = -112;

		gIpu_3dnr_shdr_Resize_Prm.drcofst.c_cr = 0;
		gIpu_3dnr_shdr_Resize_Prm.drcgain.c_cr = 113;
		gIpu_3dnr_shdr_Resize_Prm.drcclph.c_cr = 112;
		gIpu_3dnr_shdr_Resize_Prm.drcclpl.c_cr = -112;
	}

	gIpu_3dnr_shdr_Resize_Prm.chmode.c_css_md	= D_IM_SHDR_CSSMD_YCC420_ODD;		// H264 enc 0.
	gIpu_3dnr_shdr_Resize_Prm.woutsz.c_hsize	= imgH264_0->yuv_h_size;			// H264 enc 0.
	gIpu_3dnr_shdr_Resize_Prm.woutsz.c_vsize	= imgH264_0->yuv_v_size;			// H264 enc 0.
	gIpu_3dnr_shdr_Resize_Prm.chmode.c_rdc_en 	=ipu_3dnr_get_pixel_avg_reduction_info( std_width, std_height,
												imgH264_0->yuv_h_size, imgH264_0->yuv_v_size,
												&pitch[ 0 ], &pitch[ 1 ],
												&trmsta[ 0 ], &trmsta[ 1 ],
												&rdc_md );
	gIpu_3dnr_shdr_Resize_Prm.chmode.c_rdc_md	= rdc_md;	
	gIpu_3dnr_shdr_Resize_Prm.rszhcnt.c_hpit	= pitch[ 0 ];						// H264 enc 0.
	gIpu_3dnr_shdr_Resize_Prm.rszvcnt.c_vpit	= pitch[ 1 ];						// H264 enc 0.
	gIpu_3dnr_shdr_Resize_Prm.trmsta.c_hsta		= trmsta[ 0 ];
	gIpu_3dnr_shdr_Resize_Prm.trmsta.c_vsta		= trmsta[ 1 ];
	
	// H264 enc 1.
	if (imgH264_1->mem_addr_y == 0) {
		gIpu_3dnr_shdr_Resize_Prm.chmode.d_w_en	= D_IM_SHDR_DISABLE;				// H264 enc 1.
	}
	else {
		gIpu_3dnr_shdr_Resize_Prm.chmode.d_w_en	= D_IM_SHDR_ENABLE;					// H264 enc 1
	}
	gIpu_3dnr_shdr_Resize_Prm.chmode.d_rsz_md	= D_IM_RSZMD_ODD0;					// H264 enc 1.
	gIpu_3dnr_shdr_Resize_Prm.chmode.d_trm_en	= D_IM_SHDR_ENABLE;					// H264 enc 1.
	gIpu_3dnr_shdr_Resize_Prm.chmode.d_drc_en	= cfg.yc_range == E_YC_RANGE_FULL ? D_IM_SHDR_DISABLE : D_IM_SHDR_ENABLE; // H264 enc 1.
	if( gIpu_3dnr_shdr_Resize_Prm.chmode.d_drc_en ) {
		gIpu_3dnr_shdr_Resize_Prm.drcofst.d_y = 16;
		gIpu_3dnr_shdr_Resize_Prm.drcgain.d_y = 0x1B8;
		gIpu_3dnr_shdr_Resize_Prm.drcclph.d_y = 235;
		gIpu_3dnr_shdr_Resize_Prm.drcclpl.d_y = 16;

		gIpu_3dnr_shdr_Resize_Prm.drcofst.d_cb = 0;
		gIpu_3dnr_shdr_Resize_Prm.drcgain.d_cb = 113;
		gIpu_3dnr_shdr_Resize_Prm.drcclph.d_cb = 112;
		gIpu_3dnr_shdr_Resize_Prm.drcclpl.d_cb = -112;

		gIpu_3dnr_shdr_Resize_Prm.drcofst.d_cr = 0;
		gIpu_3dnr_shdr_Resize_Prm.drcgain.d_cr = 113;
		gIpu_3dnr_shdr_Resize_Prm.drcclph.d_cr = 112;
		gIpu_3dnr_shdr_Resize_Prm.drcclpl.d_cr = -112;
	}

	gIpu_3dnr_shdr_Resize_Prm.chmode.d_css_md	= D_IM_SHDR_CSSMD_YCC420_ODD;		// H264 enc 1.
	gIpu_3dnr_shdr_Resize_Prm.woutsz.d_hsize	= imgH264_1->yuv_h_size;			// H264 enc 1.
	gIpu_3dnr_shdr_Resize_Prm.woutsz.d_vsize	= imgH264_1->yuv_v_size;			// H264 enc 1.
	gIpu_3dnr_shdr_Resize_Prm.chmode.d_rdc_en	= ipu_3dnr_get_pixel_avg_reduction_info( std_width, std_height,
												imgH264_1->yuv_h_size, imgH264_1->yuv_v_size,
												&pitch[ 0 ], &pitch[ 1 ],
												&trmsta[ 0 ], &trmsta[ 1 ],
												&rdc_md );
	gIpu_3dnr_shdr_Resize_Prm.chmode.d_rdc_md	= rdc_md;
	gIpu_3dnr_shdr_Resize_Prm.rszhcnt.d_hpit	= pitch[ 0 ];						// H264 enc 1.
	gIpu_3dnr_shdr_Resize_Prm.rszvcnt.d_vpit	= pitch[ 1 ];						// H264 enc 1.
	gIpu_3dnr_shdr_Resize_Prm.trmsta.d_hsta		= trmsta[ 0 ];
	gIpu_3dnr_shdr_Resize_Prm.trmsta.d_vsta		= trmsta[ 1 ];
	

	// YUV 0.
	if (imgYUV_0->mem_addr_y == 0) {
		gIpu_3dnr_shdr_Resize_Prm.chmode.e_w_en			= D_IM_SHDR_DISABLE;			// YUV 0.
	}
	else {
		gIpu_3dnr_shdr_Resize_Prm.chmode.e_w_en			= D_IM_SHDR_ENABLE;				// YUV 0.
		gIpu_3dnr_shdr_Resize_Prm.chmode.e_rdc_en		= ipu_3dnr_get_pixel_avg_reduction_info( std_width, std_height,
														imgYUV_0->yuv_h_size, imgYUV_0->yuv_v_size,
														&pitch[ 0 ], &pitch[ 1 ],
														&trmsta[ 0 ], &trmsta[ 1 ],
														&rdc_md );
		gIpu_3dnr_shdr_Resize_Prm.chmode.e_rdc_md		= rdc_md;
		if( imgYUV_0->yuv_format == E_SHARE_YUV_FORMAT_420_SEMI ){
			gIpu_3dnr_shdr_Resize_Prm.chmode.e_css_md	= D_IM_SHDR_CSSMD_YCC420_ODD;
		}
		else{
			gIpu_3dnr_shdr_Resize_Prm.chmode.e_css_md	= D_IM_SHDR_CSSMD_YCC422;
		}
	}
	gIpu_3dnr_shdr_Resize_Prm.chmode.e_rsz_md			= D_IM_RSZMD_ODD0;				// YUV 0.
	gIpu_3dnr_shdr_Resize_Prm.chmode.e_trm_en			= D_IM_SHDR_ENABLE;				// YUV 0.
	gIpu_3dnr_shdr_Resize_Prm.woutsz.e_hsize			= imgYUV_0->yuv_h_size;			// YUV 0.
	gIpu_3dnr_shdr_Resize_Prm.woutsz.e_vsize			= imgYUV_0->yuv_v_size;			// YUV 0.
	gIpu_3dnr_shdr_Resize_Prm.rszhcnt.e_hpit			= pitch[ 0 ];					// YUV 0.
	gIpu_3dnr_shdr_Resize_Prm.rszvcnt.e_vpit			= pitch[ 1 ];					// YUV 0.
	gIpu_3dnr_shdr_Resize_Prm.trmsta.e_hsta				= trmsta[ 0 ];
	gIpu_3dnr_shdr_Resize_Prm.trmsta.e_vsta				= trmsta[ 1 ];

	// YUV 1.
	if (imgYUV_1->mem_addr_y == 0) {
		gIpu_3dnr_shdr_Resize_Prm.chmode.f_w_en			= D_IM_SHDR_DISABLE;			// YUV 1.
	}
	else {
		gIpu_3dnr_shdr_Resize_Prm.chmode.f_w_en			= D_IM_SHDR_ENABLE;				// YUV 1.
		gIpu_3dnr_shdr_Resize_Prm.chmode.f_rdc_en		= ipu_3dnr_get_pixel_avg_reduction_info(  std_width, std_height,
																						imgYUV_1->yuv_h_size, imgYUV_1->yuv_v_size,
																						&pitch[ 0 ], &pitch[ 1 ],
																						&trmsta[ 0 ], &trmsta[ 1 ],
																						&rdc_md );
		gIpu_3dnr_shdr_Resize_Prm.chmode.f_rdc_md		= rdc_md;
		if( imgYUV_1->yuv_format == E_SHARE_YUV_FORMAT_420_SEMI ){
			gIpu_3dnr_shdr_Resize_Prm.chmode.f_css_md	= D_IM_SHDR_CSSMD_YCC420_ODD;
		}
		else{
			gIpu_3dnr_shdr_Resize_Prm.chmode.f_css_md	= D_IM_SHDR_CSSMD_YCC422;
		}
	}
	gIpu_3dnr_shdr_Resize_Prm.chmode.f_rsz_md			= D_IM_RSZMD_ODD0;				// YUV 1.
	gIpu_3dnr_shdr_Resize_Prm.chmode.f_trm_en			= D_IM_SHDR_ENABLE;				// YUV 1.
	gIpu_3dnr_shdr_Resize_Prm.woutsz.f_hsize			= imgYUV_1->yuv_h_size;			// YUV 1.
	gIpu_3dnr_shdr_Resize_Prm.woutsz.f_vsize			= imgYUV_1->yuv_v_size;			// YUV 1.
	gIpu_3dnr_shdr_Resize_Prm.rszhcnt.f_hpit			= pitch[ 0 ];					// YUV 1.
	gIpu_3dnr_shdr_Resize_Prm.rszvcnt.f_vpit			= pitch[ 1 ];					// YUV 1.
	gIpu_3dnr_shdr_Resize_Prm.trmsta.f_hsta				= trmsta[ 0 ];
	gIpu_3dnr_shdr_Resize_Prm.trmsta.f_vsta				= trmsta[ 1 ];

	// Interrupt callback
	gIpu_3dnr_shdr_Ctrl_Prm.pCallBack = (T_IM_SHDR_CALLBACK)ipu_3dnr_shdr_irq_callback;
	
	// SHDR macro driver set/start/end.
	ercd = Im_SHDR_Ctrl_Common( (T_IM_SHDR_CTRL*)&gIpu_3dnr_shdr_Ctrl_Prm );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

	if (cfg.yc_bt == E_YC_BT_601 || cfg.yc_bt == E_YC_BT_IQ) {
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = 256;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = 0;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = 358;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = 256;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = -88;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = -184;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = 256;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = 464;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = -2;
	} else if (cfg.yc_bt == E_YC_BT_709) {
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = 256;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = 0;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = 404;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = 256;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = -48;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = -120;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = 256;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = 475;
		gIpu_3dnr_shdr_Post_Prm.y2r_coef[0] = 0;
	}
	ercd = Im_SHDR_Set_Post( (T_IM_SHDR_POST*)&gIpu_3dnr_shdr_Post_Prm );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

	ercd = Im_SHDR_Set_Resize( (T_IM_SHDR_RESIZE*)&gIpu_3dnr_shdr_Resize_Prm );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

#ifdef DEBUG_3DNR_GMD
	if( share_motion.x || share_motion.y ){ //if( motion.shift_x || motion.shift_y ){
		printf( "SHDR = (%d, %d) \n", motion.shift_x, motion.shift_y );
	}
#endif

#ifdef DEBUG_DUMP_SHDR_REG
	ct_im_shdr_get_ctrl();
	ct_im_shdr_get_pre();
	ct_im_shdr_get_fmd();
	ct_im_shdr_get_pmsk();
	ct_im_shdr_get_spnr();
	ct_im_shdr_get_mask();
	ct_im_shdr_get_bld();
	ct_im_shdr_get_post();
	ct_im_shdr_get_resize();
	ct_im_shdr_get_smc();
	ct_im_shdr_get_axi();
#endif

	ercd = Im_SHDR_Start();
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

	ercd = Im_SHDR_Wait_End( &wait_factor_result, 100 );
	if ( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		ercd = Im_SHDR_Init( TRUE );
		if( ercd != D_DDIM_OK ) {
			UPRINTF_ERR((INT32)ercd);
			return ercd;
		}
		return D_DDIM_SYSTEM_ERROR;
	}

	ercd = Im_SHDR_Init( FALSE );
	if( ercd != D_DDIM_OK ) {
		UPRINTF_ERR((INT32)ercd);
		return ercd;
	}

	return D_DDIM_OK;
}

static VOID ipu_3dnr_get_pixel_avg_reduction_info_rdc( FLOAT ratio, UCHAR *rdc_en, UCHAR* rdc_md )
{
	if ( ratio < 2.0f ) {
		*rdc_en = D_IM_SHDR_DISABLE;
		*rdc_md = D_IM_RDCMD_1_2;	// dummy
	}
	else if ( ratio < 4.0f ) {
		*rdc_en = D_IM_SHDR_ENABLE;
		*rdc_md = D_IM_RDCMD_1_2;
	}
	else if ( ( ratio >= 4.0f ) && ( ratio <= 8.0f ) ) {
		*rdc_en = D_IM_SHDR_ENABLE;
		*rdc_md = D_IM_RDCMD_1_4;
	}
	else {
		*rdc_en = D_IM_SHDR_ENABLE;
		*rdc_md = D_IM_RDCMD_1_8;
	}

}

static VOID ipu_3dnr_get_pixel_avg_reduction_info_pitch( UINT32 srcSize, UINT32 dstSize, USHORT* pitch, UCHAR rdc_en )
{
	FLOAT	ratio;

	ratio = (FLOAT)srcSize / dstSize;

	if ( rdc_en == D_IM_SHDR_DISABLE ) {
		if ( ratio == 1.0f ) {
			*pitch	= 0x4000;
		}
		else if ( ratio < 2.0f ) {
			*pitch	= (USHORT)ipu_3dnr_calc_pit( srcSize, dstSize );
		}
		else{
			*pitch	= 0x7FFF;
		}
	}
	else {
		if ( ratio < 4.0f ) {
			*pitch	= (USHORT)ipu_3dnr_calc_pit( srcSize, dstSize * 2 );
		}
		else if ( ( ratio >= 4.0f ) && ( ratio <= 8.0f ) ) {
			*pitch	= (USHORT)ipu_3dnr_calc_pit( srcSize, dstSize * 4 );
		}
		else {
			*pitch	= (USHORT)ipu_3dnr_calc_pit( srcSize, dstSize * 8 );
		}
	}

	if ( *pitch < 0x4000 ) {
		*pitch = 0x4000;
	}
	// WA 1.
	else if ( *pitch == 0x5a25 ) {
		if ( ( srcSize == 2704 ) && ( dstSize == 480 ) ) {
			*pitch = 0x5a06;
		}
	}
	else if ( *pitch > 0x7FFF ) {
		*pitch = 0x7FFF;
	}
	else {
	}
}

static UCHAR ipu_3dnr_get_pixel_avg_reduction_info( UINT32 src_width_Size, UINT32 src_height_Size,
																	UINT32  dst_width_Size, UINT32 dst_height_Size,
																	USHORT* pitch0, USHORT* pitch1, 
																	USHORT* trmsta0, USHORT* trmsta1,
																	UCHAR* rdc_md )
{
	FLOAT	ratio_h, ratio_v;
	UCHAR	rdc_en_h, rdc_en_v;
	UCHAR	rdc_md_h, rdc_md_v;
	UINT32	pre_trm_width, pre_trm_height;

	if( dst_width_Size == 0 || dst_height_Size == 0 ){
		*pitch0 = 0x4000;
		*pitch1 = 0x4000;
		*trmsta0 = 0;
		*trmsta1 = 0;
		*rdc_md = 0;
		return D_IM_SHDR_DISABLE;
	}
	
	ratio_h = (FLOAT)src_width_Size / dst_width_Size;
	ratio_v = (FLOAT)src_height_Size / dst_height_Size;
	
	// make same ratio
	if( ratio_h >= ratio_v ){
		ratio_h = ratio_v;
		pre_trm_width  = src_width_Size / ratio_v;
		pre_trm_height = dst_height_Size;
	}else{
		ratio_v = ratio_h;
		pre_trm_width  = dst_width_Size;
		pre_trm_height = src_height_Size / ratio_h;
	}
#if 0 // no trim
	pre_trm_width  = dst_width_Size;
	pre_trm_height = dst_height_Size;
#endif
	*trmsta0 = (pre_trm_width - dst_width_Size) / 2;
	*trmsta1 = (pre_trm_height - dst_height_Size) / 2;
	
	ipu_3dnr_get_pixel_avg_reduction_info_rdc( ratio_v, &rdc_en_v, &rdc_md_v );
	ipu_3dnr_get_pixel_avg_reduction_info_rdc( ratio_h, &rdc_en_h, &rdc_md_h );

	if( ( rdc_en_v & rdc_en_h ) == D_IM_SHDR_DISABLE ) {
		*rdc_md = D_IM_RDCMD_1_2;

		ipu_3dnr_get_pixel_avg_reduction_info_pitch( src_height_Size, pre_trm_height, pitch1, D_IM_SHDR_DISABLE );	
		ipu_3dnr_get_pixel_avg_reduction_info_pitch( src_width_Size, pre_trm_width, pitch0, D_IM_SHDR_DISABLE );

		return D_IM_SHDR_DISABLE;
	}
	else{
		*rdc_md = rdc_md_v > rdc_md_h ? rdc_md_h:rdc_md_v;

		ipu_3dnr_get_pixel_avg_reduction_info_pitch( src_height_Size, pre_trm_height, pitch1, D_IM_SHDR_ENABLE );

#if 0		
		if ( src_width_Size == 4096 && dst_width_Size == 480 &&
			src_height_Size == 2160 && dst_height_Size == 270 ){
			// The ratio_h is 8.5f and the ratio_v is 8.0f, so make the pitch calculation in the same case.
			*pitch0 = 0x7FFF;//(USHORT)ip_3dnr_calc_pit( src_width_Size, dst_width_Size * 4 );
		}
		else
#endif
		{
			ipu_3dnr_get_pixel_avg_reduction_info_pitch( src_width_Size, pre_trm_width, pitch0, D_IM_SHDR_ENABLE );
		}

		return D_IM_SHDR_ENABLE;
	}
}

static VOID ipu_3dnr_get_regtbl( T_IM_SHDR_CTRL** shdr_ctrl, T_IM_SHDR_PRE** shdr_pre,
										T_IM_SHDR_FMD** shdr_fmd, T_IM_SHDR_SPNR** shdr_spnr, 
										T_IM_SHDR_MSK** shdr_msk, T_IM_SHDR_PMSK** shdr_pmsk, 
										UINT32** shdr_in )
{
	*shdr_ctrl = (T_IM_SHDR_CTRL*)&gIpu_3dnr_shdr_Ctrl_Prm;
	*shdr_pre  = (T_IM_SHDR_PRE*)&gIpu_3dnr_shdr_Pre_Prm;
	*shdr_fmd  = (T_IM_SHDR_FMD*)&gIpu_3dnr_shdr_Fmd_Prm;
	*shdr_spnr = (T_IM_SHDR_SPNR*)&gIpu_3dnr_shdr_Spnr_Prm;
	*shdr_msk  = (T_IM_SHDR_MSK*)&gIpu_3dnr_shdr_Msk_Prm;
	*shdr_pmsk = (T_IM_SHDR_PMSK*)&gIpu_3dnr_shdr_Pmsk_Prm;
	*shdr_in   = (UINT32*)&gIpu_3dnr_shdr_InPrm;
}

static VOID ipu_3dnr_me_irq_callback( ULONG interrupt_flag_mask )
{
	
	if ( interrupt_flag_mask != 0 ) {
		UPRINTF_ERR((INT32)interrupt_flag_mask);
		return;
	}
}

static VOID ipu_3dnr_shdr_irq_callback( ULONG interrupt_flag_mask )
{
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHDR, FJ_MBALOG_FUNC_ID_3DNR, FJ_MBALOG_TYPE_END, "SHDR Start"));
	
	if ( interrupt_flag_mask != 0 ) {
		UPRINTF_ERR((INT32)interrupt_flag_mask);
	}

	//Snd_Event_to_System( E_SYSTEM_EVENT_SHDR_YUV, gFrame_No_Shdr );
}

static FJ_ERR_CODE ipu_iq_default_set_3dnr_reg_tbl( T_IQ_3DNR_IM_ARG* const im_arg )
{
	im_arg->shdr_ctrl->image_mode.fmd_en		= 1;

	im_arg->shdr_pre->y2r_coef[0]				=  256;
	im_arg->shdr_pre->y2r_coef[1]				=    1;
	im_arg->shdr_pre->y2r_coef[2]				=  358;
	im_arg->shdr_pre->y2r_coef[3]				=  256;
	im_arg->shdr_pre->y2r_coef[4]				=  -88;
	im_arg->shdr_pre->y2r_coef[5]				= -184;
	im_arg->shdr_pre->y2r_coef[6]				=  256;
	im_arg->shdr_pre->y2r_coef[7]				=  454;
	im_arg->shdr_pre->y2r_coef[8]				=   -2;

	im_arg->shdr_pre->y2r_clp.upper_long0[0]	= 255;
	im_arg->shdr_pre->y2r_clp.upper_long0[1]	= 255;
	im_arg->shdr_pre->y2r_clp.upper_long0[2]	= 255;

	im_arg->shdr_pre->y2r_clp.lower_long0[0]	=   0;
	im_arg->shdr_pre->y2r_clp.lower_long0[1]	=   0;
	im_arg->shdr_pre->y2r_clp.lower_long0[2]	=   0;

	im_arg->shdr_pre->y2r_clp.upper_short[0]	= 255;
	im_arg->shdr_pre->y2r_clp.upper_short[1]	= 255;
	im_arg->shdr_pre->y2r_clp.upper_short[2]	= 255;

	im_arg->shdr_pre->y2r_clp.lower_short[0]	=   0;
	im_arg->shdr_pre->y2r_clp.lower_short[1]	=   0;
	im_arg->shdr_pre->y2r_clp.lower_short[2]	=   0;

	im_arg->shdr_pre->r2y_coef[0]				=   77;
	im_arg->shdr_pre->r2y_coef[1]				=  150;
	im_arg->shdr_pre->r2y_coef[2]				=   29;
	im_arg->shdr_pre->r2y_coef[3]				=  -43;
	im_arg->shdr_pre->r2y_coef[4]				=  -85;
	im_arg->shdr_pre->r2y_coef[5]				=  128;
	im_arg->shdr_pre->r2y_coef[6]				=  128;
	im_arg->shdr_pre->r2y_coef[7]				= -107;
	im_arg->shdr_pre->r2y_coef[8]				=  -21;

	im_arg->shdr_pre->r2y_clp.upper_long_y		=  4095;
	im_arg->shdr_pre->r2y_clp.upper_long_c[0]	=  2047;
	im_arg->shdr_pre->r2y_clp.upper_long_c[1]	=  2047;

	im_arg->shdr_pre->r2y_clp.lower_long_y		=     0;
	im_arg->shdr_pre->r2y_clp.lower_long_c[0]	= -2048;
	im_arg->shdr_pre->r2y_clp.lower_long_c[1]	= -2048;

	im_arg->shdr_pre->r2y_clp.upper_short_y		=  4095;
	im_arg->shdr_pre->r2y_clp.upper_short_c[0]	=  2047;
	im_arg->shdr_pre->r2y_clp.upper_short_c[1]	=  2047;

	im_arg->shdr_pre->r2y_clp.lower_short_y		=     0;
	im_arg->shdr_pre->r2y_clp.lower_short_c[0]	= -2048;
	im_arg->shdr_pre->r2y_clp.lower_short_c[1]	= -2048;

	im_arg->shdr_fmd->ep_boundary[1]			=  64;
	im_arg->shdr_fmd->ep_boundary[2]			= 128;
	im_arg->shdr_fmd->ep_boundary[3]			= 192;

	im_arg->shdr_fmd->ep_offset[0]				=  10;
	im_arg->shdr_fmd->ep_offset[1]				=  10;
	im_arg->shdr_fmd->ep_offset[2]				=  10;
	im_arg->shdr_fmd->ep_offset[3]				=  10;

	im_arg->shdr_fmd->ep_blend					= 0;
	im_arg->shdr_fmd->fd_weight					= 8;

	im_arg->shdr_fmd->bldr_offset[0]			=   0;
	im_arg->shdr_fmd->bldr_offset[1]			=   8;
	im_arg->shdr_fmd->bldr_offset[2]			=  24;
	im_arg->shdr_fmd->bldr_offset[3]			= 232;
	im_arg->shdr_fmd->bldr_offset[4]			= 255;

	im_arg->shdr_fmd->bldr_slope[0]				=   32;
	im_arg->shdr_fmd->bldr_slope[1]				=  256;
	im_arg->shdr_fmd->bldr_slope[2]				= 1664;
	im_arg->shdr_fmd->bldr_slope[3]				=  368;
	im_arg->shdr_fmd->bldr_slope[4]				=    0;

	im_arg->shdr_fmd->bldr_boundary[1]			= 4;
	im_arg->shdr_fmd->bldr_boundary[2]			= 5;
	im_arg->shdr_fmd->bldr_boundary[3]			= 7;
	im_arg->shdr_fmd->bldr_boundary[4]			= 8;

	im_arg->shdr_fmd->weight_r2y_coef[0]		=  77;
	im_arg->shdr_fmd->weight_r2y_coef[1]		= 150;
	im_arg->shdr_fmd->weight_r2y_coef[2]		=  29;

	im_arg->shdr_fmd->weight_r2y_clp.upper		= 255;
	im_arg->shdr_fmd->weight_r2y_clp.lower		=   0;
	im_arg->shdr_fmd->weight_base				=   0;

	// R
	im_arg->shdr_fmd->weight_offset[0][0]		= 256;
	im_arg->shdr_fmd->weight_offset[0][1]		= 256;
	im_arg->shdr_fmd->weight_offset[0][2]		= 256;
	im_arg->shdr_fmd->weight_offset[0][3]		= 256;
	im_arg->shdr_fmd->weight_offset[0][4]		= 256;

	// G
	im_arg->shdr_fmd->weight_offset[1][0]		= 256;
	im_arg->shdr_fmd->weight_offset[1][1]		= 256;
	im_arg->shdr_fmd->weight_offset[1][2]		= 256;
	im_arg->shdr_fmd->weight_offset[1][3]		= 256;
	im_arg->shdr_fmd->weight_offset[1][4]		= 256;

	// B
	im_arg->shdr_fmd->weight_offset[2][0]		= 256;
	im_arg->shdr_fmd->weight_offset[2][1]		= 256;
	im_arg->shdr_fmd->weight_offset[2][2]		= 256;
	im_arg->shdr_fmd->weight_offset[2][3]		= 256;
	im_arg->shdr_fmd->weight_offset[2][4]		= 256;

	// R
	im_arg->shdr_fmd->weight_slope[0][0]		= 0;
	im_arg->shdr_fmd->weight_slope[0][1]		= 0;
	im_arg->shdr_fmd->weight_slope[0][2]		= 0;
	im_arg->shdr_fmd->weight_slope[0][3]		= 0;
	im_arg->shdr_fmd->weight_slope[0][4]		= 0;

	// G
	im_arg->shdr_fmd->weight_slope[1][0]		= 0;
	im_arg->shdr_fmd->weight_slope[1][1]		= 0;
	im_arg->shdr_fmd->weight_slope[1][2]		= 0;
	im_arg->shdr_fmd->weight_slope[1][3]		= 0;
	im_arg->shdr_fmd->weight_slope[1][4]		= 0;

	// B
	im_arg->shdr_fmd->weight_slope[2][0]		= 0;
	im_arg->shdr_fmd->weight_slope[2][1]		= 0;
	im_arg->shdr_fmd->weight_slope[2][2]		= 0;
	im_arg->shdr_fmd->weight_slope[2][3]		= 0;
	im_arg->shdr_fmd->weight_slope[2][4]		= 0;

	// R
	im_arg->shdr_fmd->weight_boundary[0][1]		=   0;
	im_arg->shdr_fmd->weight_boundary[0][2]		= 255;
	im_arg->shdr_fmd->weight_boundary[0][3]		= 255;
	im_arg->shdr_fmd->weight_boundary[0][4]		= 255;

	// G
	im_arg->shdr_fmd->weight_boundary[1][1]		=   0;
	im_arg->shdr_fmd->weight_boundary[1][2]		= 255;
	im_arg->shdr_fmd->weight_boundary[1][3]		= 255;
	im_arg->shdr_fmd->weight_boundary[1][4]		= 255;

	// B
	im_arg->shdr_fmd->weight_boundary[2][1]		=   0;
	im_arg->shdr_fmd->weight_boundary[2][2]		= 255;
	im_arg->shdr_fmd->weight_boundary[2][3]		= 255;
	im_arg->shdr_fmd->weight_boundary[2][4]		= 255;

	im_arg->shdr_spnr->spnr_en					= 1;

	im_arg->shdr_spnr->spe_boundary[1]			= 1024;
	im_arg->shdr_spnr->spe_boundary[2]			= 2048;
	im_arg->shdr_spnr->spe_boundary[3]			= 3072;

	im_arg->shdr_spnr->spe_offset[0]			= 48;
	im_arg->shdr_spnr->spe_offset[1]			= 48;
	im_arg->shdr_spnr->spe_offset[2]			= 48;
	im_arg->shdr_spnr->spe_offset[3]			= 48;

	im_arg->shdr_spnr->spe_blend				= 0;

	im_arg->shdr_msk->norm_gain[0]				= 128;
	im_arg->shdr_msk->norm_gain[1]				= 128;
	im_arg->shdr_msk->norm_gain[2]				= 128;

	im_arg->shdr_msk->norm_offset[0]			= 128;
	im_arg->shdr_msk->norm_offset[1]			= 128;
	im_arg->shdr_msk->norm_offset[2]			= 128;

	return FJ_ERR_OK;
}


static VOID ipu_3dnr_me_shdr_calc_shift( const T_SHDR_MNG_OUT* const imgSrcFull, 
										 const T_SHDR_MNG_OUT* const imgRefFull,
										 BOOL is_me,
										 T_SHDR_MOTION* motion )
{
	SHORT	shift_x, shift_y;
	SHORT	max;
	SHORT	mod;
	USHORT	mgn;
	
	// Divide by 4 for ME ( ME input is Full/4 )
	shift_x = motion->shift_x /4;
	shift_y = motion->shift_y /4;
	
	// Clip by -64 ~ 63 for ME Limit
	shift_x = (shift_x >  63) ?  63 : shift_x;
	shift_x = (shift_x < -64) ? -63 : shift_x;
	shift_y = (shift_y >  63) ?  63 : shift_y;
	shift_y = (shift_y < -64) ? -63 : shift_y;
	
	// Multi by 4 for SHDR
	shift_x *= 4;
	shift_y *= 4;
	
	// Clip by SHDR Limit X
	mod = imgRefFull->yuv_h_size % 1024;
	if( mod ){
		max = mod - 1;
	}else{
		max = 1023;
	}
	if( shift_x > max ){
		UPRINTF( "Attention shift_x:%d > max:%d\n", shift_x, max );
		shift_x = max;
	}
	shift_x &= 0xFFFE;
	// Clip by SHDR Limit Y
	mod = imgRefFull->yuv_v_size % 1023; // depends on TMODE
	if( mod ){
		max = mod - 1;
	}else{
		max = 1023;
	}
	if( shift_y > max ){
		UPRINTF( "Attention shift_y:%d > max:%d\n", shift_y, max );
		shift_y = max;
	}
	shift_y &= 0xFFFE;
	
	// Clip by MGN ( optional )
	mgn = D_IPU_3DNR_MARGIN/2;
	shift_x = (shift_x >  mgn) ?  mgn : shift_x;
	shift_x = (shift_x < -mgn) ? -mgn : shift_x;
	shift_y = (shift_y >  mgn) ?  mgn : shift_y;
	shift_y = (shift_y < -mgn) ? -mgn : shift_y;
	
	// Set result
	if( is_me ){
		motion->shift_x = shift_x / 4;
		motion->shift_y = shift_y / 4;
	}else{
		motion->shift_x = shift_x;
		motion->shift_y = shift_y;
	}
	
	// for debug
	if( g_debug_3dnr.en && g_debug_3dnr.force_chg_me_shift && is_me ){
		motion->shift_x = g_debug_3dnr.force_shift_me_x;
		motion->shift_y = g_debug_3dnr.force_shift_me_y;
	}
	if( g_debug_3dnr.en && g_debug_3dnr.force_chg_shdr_shift && !is_me ){
		motion->shift_x = g_debug_3dnr.force_shift_shdr_x;
		motion->shift_y = g_debug_3dnr.force_shift_shdr_y;
	}
}
