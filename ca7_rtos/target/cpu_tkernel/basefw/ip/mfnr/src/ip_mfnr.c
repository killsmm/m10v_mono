 /**
 * @file		ip_mfnr.c
 * @brief		Source code of MFNR wrapper layer
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */
 

/*----------------------------------------------------------------------*/
/* Include Files														*/
/*----------------------------------------------------------------------*/

#include <string.h>
#include "ip_mfnr.h"
#include "ipc_if.h"
#include "ipc_gmd.h"
#include "ip_ge.h"
#include "im_me.h"
#include "im_shdr.h"
#include "dd_arm.h"
#include "ddim_typedef.h"
#include "os_user_custom.h"
#include "fj_debug.h"
// delete basefw\fj\imgproc\still
//#include "still_proc_subr.h"
#include "sdram_map_manager.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_IP_MFNR_WAIT_TIME				(100)
#define D_IP_MFNR_AFFIN_MAX				(6)
#define D_IP_MFNR_HEAP_SIZE				(14336)


//#define CO_IP_MFNR_CROP_DBG_PRINT
//#define CO_IP_MFNR_CROP_POS_DUMP

// Xform array index number definitions.
#define D_IP_MFNR_CROP_XFORM_IDX_PIT_XX	(0)
#define D_IP_MFNR_CROP_XFORM_IDX_PIT_XY	(1)
#define D_IP_MFNR_CROP_XFORM_IDX_STA_X	(2)
#define D_IP_MFNR_CROP_XFORM_IDX_PIT_YX	(3)
#define D_IP_MFNR_CROP_XFORM_IDX_PIT_YY	(4)
#define D_IP_MFNR_CROP_XFORM_IDX_STA_Y	(5)

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
#define ip_mfnr_roundup_4( val )		((((val) +    3) /     4) *     4)
#define ip_mfnr_roundup_16( val )		((((val) +   15) /    16) *    16)
#define ip_mfnr_roundup_32( val )		((((val) +   31) /    32) *    32)
#define ip_mfnr_roundup_256( val )		((((val) +  255) /   256) *   256)
#define ip_mfnr_roundup_1k( val )		((((val) + 1023) /  1024) *  1024)
#define ip_mfnr_roundup_16k( val )		((((val) +16383) / 16384) * 16384)
#define ip_mfnr_calc_pit( src, dst )	((UINT32)((UINT64)0x4000 * ((src) -1) / ((dst) -1)))
#define ip_mfnr_min( a, b )				(((a) < (b))?(a):(b))
#define ip_mfnr_max( a, b )				(((a) > (b))?(a):(b))

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static volatile BOOL				gIp_Mfnr_Open			= FALSE;
static volatile UCHAR				gIp_Mfnr_MaxCount;
static volatile IP_MFNR_START_PRM	gIp_Mfnr_Start_Prm;
static volatile INT32				gIp_Mfnr_Count;
static volatile FJ_STILL_IP_IMAGE	gIp_Mfnr_Input_Prev_Frame_Info;
static volatile FJ_STILL_IP_IMAGE	gIp_Mfnr_Input_Prev_Frame_Info_Red;

static volatile T_IM_ME_CTRL	gIp_Mfnr_Me_Ctrl_Prm = {
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

static volatile T_IM_ME_START	gIp_Mfnr_Me_Start_Prm = {
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
		.pre_shift_x			= 0,		// Fixed.
		.pre_shift_y			= 0,		// Fixed.
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
		.cur_margin_width		= D_IM_ME_CUR_MARGIN_0,		// Variable.
		.cur_margin_height		= D_IM_ME_CUR_MARGIN_0,		// Variable.
		.prefetch_width			= 0,		// Variable.
		.prefetch_height		= 0,		// Variable.
		.search_width			= 0,		// Variable.
		.search_height			= 0,		// Variable.
	},
};

static volatile T_IM_SHDR_CTRL	gIp_Mfnr_Shdr_Ctrl_Prm = {
	.image_set.in_bit_depth		= D_IM_SHDR_INBITDEPTH_8,
	.image_set.in_c_fmt			= D_IM_SHDR_INCFMT_422SP,
	.image_set.in_edge_cpy		= D_IM_SHDR_OFF,
	.image_set.in_edge_cut		= D_IM_SHDR_OFF,
	.image_set.out_ctrl			= D_IM_SHDR_OUTCTRL_YCC,
	.image_set.out_sel			= D_IM_SHDR_OUTSEL_YCC_16,
	.image_set.out_sel_en		= D_IM_SHDR_DISABLE,
	.image_mode.exp_mag			= 0x0100,
	.image_mode.fil_margin		= D_IM_SHDR_FLITER_MARGIN_16,
	.image_mode.rexp_mag_shift	= 0,
	.image_mode.rexp_mag		= 0x10000,
	.image_mode.shift_x			= 0,
	.image_mode.shift_y			= 0,
	.image_mode.fmd_en			= D_IM_SHDR_ENABLE,
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

static volatile T_IM_SHDR_PRE	gIp_Mfnr_Pre_Prm = {
	.pre_mode.cus_mode			= D_IM_SHDR_CUS_YCC422_BILINEAR_A,
	.pre_mode.degamma_mode		= D_IM_SHDR_DEGAMMA_BITEXP,
	.y2r_coef[ 0 ] 				= 0x0100,							// IQ.bin
	.y2r_coef[ 1 ] 				= 0x0001,							// IQ.bin
	.y2r_coef[ 2 ] 				= 0x0166,							// IQ.bin
	.y2r_coef[ 3 ] 				= 0x0100,							// IQ.bin
	.y2r_coef[ 4 ] 				= 0x7FA8,							// IQ.bin
	.y2r_coef[ 5 ] 				= 0x7F48,							// IQ.bin
	.y2r_coef[ 6 ] 				= 0x0100,							// IQ.bin
	.y2r_coef[ 7 ] 				= 0x01C6,							// IQ.bin
	.y2r_coef[ 8 ] 				= 0x7FFE,							// IQ.bin
	.y2r_clp.upper_long0[ 0 ]	= 0x00FF,							// IQ.bin
	.y2r_clp.upper_long0[ 1 ]	= 0x00FF,							// IQ.bin
	.y2r_clp.upper_long0[ 2 ]	= 0x00FF,							// IQ.bin
	.y2r_clp.lower_long0[ 0 ]	= 0x0000,							// IQ.bin
	.y2r_clp.lower_long0[ 1 ]	= 0x0000,							// IQ.bin
	.y2r_clp.lower_long0[ 2 ]	= 0x0000,							// IQ.bin
	.y2r_clp.upper_short[ 0 ]	= 0xFF,								// IQ.bin
	.y2r_clp.upper_short[ 1 ]	= 0xFF,								// IQ.bin
	.y2r_clp.upper_short[ 2 ]	= 0xFF,								// IQ.bin
	.y2r_clp.lower_short[ 0 ]	= 0x00,								// IQ.bin
	.y2r_clp.lower_short[ 1 ]	= 0x00,								// IQ.bin
	.y2r_clp.lower_short[ 2 ]	= 0x00,								// IQ.bin
	.r2y_coef[ 0 ] 				= 0x004D,							// IQ.bin
	.r2y_coef[ 1 ] 				= 0x0096,							// IQ.bin
	.r2y_coef[ 2 ] 				= 0x001D,							// IQ.bin
	.r2y_coef[ 3 ] 				= 0x7FD5,							// IQ.bin
	.r2y_coef[ 4 ] 				= 0x7FAB,							// IQ.bin
	.r2y_coef[ 5 ] 				= 0x0080,							// IQ.bin
	.r2y_coef[ 6 ] 				= 0x0080,							// IQ.bin
	.r2y_coef[ 7 ] 				= 0x7F95,							// IQ.bin
	.r2y_coef[ 8 ] 				= 0x7FEB,							// IQ.bin
	.r2y_clp.upper_long_y		= 0x0FFF,							// IQ.bin
	.r2y_clp.upper_long_c[ 0 ]	= 0x07FF,							// IQ.bin
	.r2y_clp.upper_long_c[ 1 ]	= 0x07FF,							// IQ.bin
	.r2y_clp.lower_long_y		= 0x0000,							// IQ.bin
	.r2y_clp.lower_long_c[ 0 ]	= 0xF800,							// IQ.bin
	.r2y_clp.lower_long_c[ 1 ]	= 0xF800,							// IQ.bin
	.r2y_clp.upper_short_y		= 0xFFF,							// IQ.bin
	.r2y_clp.upper_short_c[ 0 ]	= 0x7FF,							// IQ.bin
	.r2y_clp.upper_short_c[ 1 ]	= 0x7FF,							// IQ.bin
	.r2y_clp.lower_short_y		= 0x000,							// IQ.bin
	.r2y_clp.lower_short_c[ 0 ]	= 0x800,							// IQ.bin
	.r2y_clp.lower_short_c[ 1 ]	= 0x800,							// IQ.bin
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

static volatile T_IM_SHDR_FMD	gIp_Mfnr_Fmd_Prm = {
	.ep_boundary[ 0 ]			= 0x00,		// D.C.
	.ep_boundary[ 1 ]			= 0x40,								// IQ.bin
	.ep_boundary[ 2 ]			= 0x80,								// IQ.bin
	.ep_boundary[ 3 ]			= 0xC0,								// IQ.bin
	.ep_offset[ 0 ]				= 0x0A,								// IQ.bin
	.ep_offset[ 1 ]				= 0x0A,								// IQ.bin
	.ep_offset[ 2 ]				= 0x0A,								// IQ.bin
	.ep_offset[ 3 ]				= 0x0A,								// IQ.bin
	.ep_blend					= 0x0,								// IQ.bin
	.fd_weight					= 0x08,								// IQ.bin
	.mvcnt.bounds				= 0x00,
	.mvcnt.threshold			= 0x00,
	.mvcnt.gr					= 0x0000,
	.bldr_offset[ 0 ]			= 0x00,								// IQ.bin
	.bldr_offset[ 1 ]			= 0x08,								// IQ.bin
	.bldr_offset[ 2 ]			= 0x18,								// IQ.bin
	.bldr_offset[ 3 ]			= 0xE8,								// IQ.bin
	.bldr_offset[ 4 ]			= 0xFF,								// IQ.bin
	.bldr_slope[ 0 ]			= 0x020,							// IQ.bin
	.bldr_slope[ 1 ]			= 0x100,							// IQ.bin
	.bldr_slope[ 2 ]			= 0x680,							// IQ.bin
	.bldr_slope[ 3 ]			= 0x170,							// IQ.bin
	.bldr_slope[ 4 ]			= 0x000,							// IQ.bin
	.bldr_boundary[ 0 ]			= 0x00,		// D.C.
	.bldr_boundary[ 1 ]			= 0x04,								// IQ.bin
	.bldr_boundary[ 2 ]			= 0x05,								// IQ.bin
	.bldr_boundary[ 3 ]			= 0x07,								// IQ.bin
	.bldr_boundary[ 4 ]			= 0x08,								// IQ.bin
	.weight_r2y_coef[ 0 ]		= 0x004D,							// IQ.bin
	.weight_r2y_coef[ 1 ]		= 0x0096,							// IQ.bin
	.weight_r2y_coef[ 2 ]		= 0x001D,							// IQ.bin
	.weight_r2y_clp.upper		= 0xFF,								// IQ.bin
	.weight_r2y_clp.lower		= 0x00,								// IQ.bin
	.weight_base				= 0x0,								// IQ.bin
	.weight_offset[ 0 ][ 0 ]	= 0x100,							// IQ.bin
	.weight_offset[ 0 ][ 1 ]	= 0x100,							// IQ.bin
	.weight_offset[ 0 ][ 2 ]	= 0x100,							// IQ.bin
	.weight_offset[ 0 ][ 3 ]	= 0x100,							// IQ.bin
	.weight_offset[ 0 ][ 4 ]	= 0x100,							// IQ.bin
	.weight_offset[ 1 ][ 0 ]	= 0x100,							// IQ.bin
	.weight_offset[ 1 ][ 1 ]	= 0x100,							// IQ.bin
	.weight_offset[ 1 ][ 2 ]	= 0x100,							// IQ.bin
	.weight_offset[ 1 ][ 3 ]	= 0x100,
	.weight_offset[ 1 ][ 4 ]	= 0x100,
	.weight_offset[ 2 ][ 0 ]	= 0x100,							// IQ.bin
	.weight_offset[ 2 ][ 1 ]	= 0x100,							// IQ.bin
	.weight_offset[ 2 ][ 2 ]	= 0x100,							// IQ.bin
	.weight_offset[ 2 ][ 3 ]	= 0x100,							// IQ.bin
	.weight_offset[ 2 ][ 4 ]	= 0x100,							// IQ.bin
	.weight_slope[ 0 ][ 0 ]		= 0x000,							// IQ.bin
	.weight_slope[ 0 ][ 1 ]		= 0x000,							// IQ.bin
	.weight_slope[ 0 ][ 2 ]		= 0x000,							// IQ.bin
	.weight_slope[ 0 ][ 3 ]		= 0x000,							// IQ.bin
	.weight_slope[ 0 ][ 4 ]		= 0x000,							// IQ.bin
	.weight_slope[ 1 ][ 0 ]		= 0x000,							// IQ.bin
	.weight_slope[ 1 ][ 1 ]		= 0x000,							// IQ.bin
	.weight_slope[ 1 ][ 2 ]		= 0x000,							// IQ.bin
	.weight_slope[ 1 ][ 3 ]		= 0x000,							// IQ.bin
	.weight_slope[ 1 ][ 4 ]		= 0x000,							// IQ.bin
	.weight_slope[ 2 ][ 0 ]		= 0x000,							// IQ.bin
	.weight_slope[ 2 ][ 1 ]		= 0x000,							// IQ.bin
	.weight_slope[ 2 ][ 2 ]		= 0x000,							// IQ.bin
	.weight_slope[ 2 ][ 3 ]		= 0x000,							// IQ.bin
	.weight_slope[ 2 ][ 4 ]		= 0x000,							// IQ.bin
	.weight_boundary[ 0 ][ 0 ]	= 0x00,		// D.C.
	.weight_boundary[ 0 ][ 1 ]	= 0x00,								// IQ.bin
	.weight_boundary[ 0 ][ 2 ]	= 0xFF,								// IQ.bin
	.weight_boundary[ 0 ][ 3 ]	= 0xFF,								// IQ.bin
	.weight_boundary[ 0 ][ 4 ]	= 0xFF,								// IQ.bin
	.weight_boundary[ 1 ][ 0 ]	= 0x00,		// D.C.
	.weight_boundary[ 1 ][ 1 ]	= 0x00,								// IQ.bin
	.weight_boundary[ 1 ][ 2 ]	= 0xFF,								// IQ.bin
	.weight_boundary[ 1 ][ 3 ]	= 0xFF,								// IQ.bin
	.weight_boundary[ 1 ][ 4 ]	= 0xFF,								// IQ.bin
	.weight_boundary[ 2 ][ 0 ]	= 0x00,		// D.C.
	.weight_boundary[ 2 ][ 1 ]	= 0x00,								// IQ.bin
	.weight_boundary[ 2 ][ 2 ]	= 0xFF,								// IQ.bin
	.weight_boundary[ 2 ][ 3 ]	= 0xFF,								// IQ.bin
	.weight_boundary[ 2 ][ 4 ]	= 0xFF,								// IQ.bin
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

static volatile T_IM_SHDR_PMSK	gIp_Mfnr_Pmsk_Prm = {
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
};

static volatile T_IM_SHDR_SPNR	gIp_Mfnr_Spnr_Prm = {
	.spnr_en					= D_IM_SHDR_ENABLE,					// IQ.bin
	.spe_blend					= 0x0,								// IQ.bin
	.spe_boundary[ 0 ]			= 0x000,		// D.C.
	.spe_boundary[ 1 ]			= 0x400,							// IQ.bin
	.spe_boundary[ 2 ]			= 0x800,							// IQ.bin
	.spe_boundary[ 3 ]			= 0xC00,							// IQ.bin
	.spe_offset[ 0 ]			= 0x030,							// IQ.bin
	.spe_offset[ 1 ]			= 0x030,							// IQ.bin
	.spe_offset[ 2 ]			= 0x030,							// IQ.bin
	.spe_offset[ 3 ]			= 0x030,							// IQ.bin
};

static volatile T_IM_SHDR_MSK	gIp_Mfnr_Msk_Prm = {
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
	.blend_boundary[ 1 ]		= 0x000,
	.blend_boundary[ 2 ]		= 0x000,
	.blend_boundary[ 3 ]		= 0x000,
	.blend_boundary[ 4 ]		= 0x000,
	.blend_boundary[ 5 ]		= 0x000,
	.blend_boundary[ 6 ]		= 0x000,
	.norm_gain[ 0 ]				= 0x80,								// IQ.bin
	.norm_gain[ 1 ]				= 0x80,								// IQ.bin
	.norm_gain[ 2 ]				= 0x80,								// IQ.bin
	.norm_offset[ 0 ]			= 0x80,								// IQ.bin
	.norm_offset[ 1 ]			= 0x80,								// IQ.bin
	.norm_offset[ 2 ]			= 0x80,								// IQ.bin
	.msk_mode					= D_IM_SHDR_MSKMD_MSKONLY,
};

static volatile T_IM_SHDR_BLD	gIp_Mfnr_Bld_Prm = {
	.blend_sel					= D_IM_SHDR_BLDSEL_SHORT_LONG,
	.alpha_sel					= D_IM_SHDR_MSKMD_MSKONLY,
	.alpha_val[ 0 ]				= 0xFF,
	.alpha_val[ 1 ]				= 0xFF,
	.alpha_val[ 2 ]				= 0xFF,
};

static volatile T_IM_SHDR_POST	gIp_Mfnr_Post_Prm = {
	.sft1rnd.sft1rnd_en			= D_IM_SHDR_DISABLE,
	.y2r_coef[ 0 ]				= 0x0100,
	.y2r_coef[ 1 ]				= 0x0001,
	.y2r_coef[ 2 ]				= 0x0166,
	.y2r_coef[ 3 ]				= 0x0100,
	.y2r_coef[ 4 ]				= 0x7fa8,
	.y2r_coef[ 5 ]				= 0x7f48,
	.y2r_coef[ 6 ]				= 0x0100,
	.y2r_coef[ 7 ]				= 0x01c6,
	.y2r_coef[ 8 ]				= 0x7ffe,
	.y2r_clp_th_upper[ 0 ]		= 0xffff,
	.y2r_clp_th_upper[ 1 ]		= 0xffff,
	.y2r_clp_th_upper[ 2 ]		= 0xffff,
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
};

static volatile T_IM_SHDR_RESIZE	gIp_Mfnr_Resize_Prm = {
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
	.woutsz.f_hsize				= 0x0280,	// 640
	.woutsz.f_vsize				= 0x0168,	// 360
	.rszhcnt.c_hsta				= 0x0000,
	.rszhcnt.c_hpit				= 0x7FFF,	// 50%
	.rszhcnt.d_hsta				= 0x0000,
	.rszhcnt.d_hpit				= 0x5FFF,	// 66.7%
	.rszhcnt.e_hsta				= 0x0000,
	.rszhcnt.e_hpit				= 0x4000,	// 100%
	.rszhcnt.f_hsta				= 0x0000,
	.rszhcnt.f_hpit				= 0x4000,	// 100%
	.rszvcnt.c_vsta				= 0x0000,
	.rszvcnt.c_vpit				= 0x7FFF,	// 50%
	.rszvcnt.d_vsta				= 0x0000,
	.rszvcnt.d_vpit				= 0x5FFF,	// 66.7%
	.rszvcnt.e_vsta				= 0x0000,
	.rszvcnt.e_vpit				= 0x4000,	// 100%
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

static volatile T_IM_SHDR_SMC	gIp_Mfnr_Smc_Prm = {
	.mcarmd.mca_mode			= D_IM_MCAMD_NR_EFFECT,
	.mcarmd.mcr_mode			= D_IM_MCAMD_MAX_VECTOR,
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
	.molrcnt.molr_o				= 0xE,
	.molrcnt.molr_k				= 0x2,
	.mcpcnt.mcpx_s				= 0x00,
	.mcpcnt.mcpx_o				= 0x10,
	.mcpcnt.mcpx_k				= 0x3,
	.mmcrcnt.mmcr_s				= 0x00a,
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
};

static volatile T_IM_SHDR_AXI	gIp_Mfnr_Axi_Prm = {
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

static volatile USHORT	gIp_Mfnr_Degamma_Prm[ 3 ][ D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ] = {
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

IPC_UCHAR	gIp_Mfnr_Gainup_Tbl0[ 255 ];
IPC_UCHAR	gIp_Mfnr_Gainup_Tbl1[ 255 ];

IP_MFNR_CROP_CORNER_POS	gIp_Mfnr_Base_Corner_Pos;

#if defined(CO_IQBIN_GENERAL)	// --- REMOVE_IQBIN_GENERAL BEGIN ---
static volatile UINT32 gIp_Mfnr_ShdrIn_Prm = D_IM_SHDR_SHDRIN_STN_SMCOUT;

#endif // CO_IQBIN_GENERAL  --- REMOVE_IQBIN_GENERAL END ---
/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

static VOID ip_mfnr_print_xform( const CHAR* const str, const FLOAT xform[6] ) __attribute__((unused));
static VOID ip_mfnr_print_crop_corner_pos( const CHAR* const str, const IP_MFNR_CROP_CORNER_POS* const crop_corner_pos ) __attribute__((unused));

static VOID ip_mfnr_print_xform( const CHAR* const str, const FLOAT xform[6] )
{
	printf( "%s: X: %.2f %.2f %.2f Y: %.2f %.2f, %.2f\n", str, xform[0], xform[1], xform[2], xform[3], xform[4], xform[5] );
}

static VOID ip_mfnr_print_crop_corner_pos( const CHAR* const str, const IP_MFNR_CROP_CORNER_POS* const crop_corner_pos )
{
	printf( "%s: left-top : %.2f, %.2f\n", str, crop_corner_pos->left_top.pos_x, crop_corner_pos->left_top.pos_y );
	printf( "%s: right-top: %.2f, %.2f\n", str, crop_corner_pos->right_top.pos_x, crop_corner_pos->right_top.pos_y );
	printf( "%s: left-btm : %.2f, %.2f\n", str, crop_corner_pos->left_bottom.pos_x, crop_corner_pos->left_bottom.pos_y );
	printf( "%s: right-btm: %.2f, %.2f\n", str, crop_corner_pos->right_bottom.pos_x, crop_corner_pos->right_bottom.pos_y );
}

#ifdef CO_IP_MFNR_CROP_POS_DUMP
static VOID ip_mfnr_dump_crop_corner_pos( const CHAR* const str, const IP_MFNR_CROP_CORNER_POS* const crop_corner_pos )
{
	printf( "%s [ %7.2f, %7.2f, %7.2f, %7.2f, %7.2f, %7.2f, %7.2f, %7.2f, ],\n",
			str,
			crop_corner_pos->left_top.pos_x, crop_corner_pos->left_top.pos_y,
			crop_corner_pos->right_top.pos_x, crop_corner_pos->right_top.pos_y,
			crop_corner_pos->left_bottom.pos_x, crop_corner_pos->left_bottom.pos_y,
			crop_corner_pos->right_bottom.pos_x, crop_corner_pos->right_bottom.pos_y );
}
#endif

static VOID ip_mfnr_int_handler_me( ULONG interrupt_flag_mask )
{
	if ( interrupt_flag_mask != 0 ) {
		FJ_Debug_Print_Err( "ip_mfnr_int_handler_me() : AXI Error = 0x%08lx\n", interrupt_flag_mask );
	}
}

static VOID ip_mfnr_int_handler_shdr( ULONG interrupt_flag_mask )
{
	if ( interrupt_flag_mask != 0 ) {
		FJ_Debug_Print_Err( "ip_mfnr_int_handler_shdr() : AXI Error = 0x%08lx\n", interrupt_flag_mask );
	}
}

static VOID ip_mfnr_crop_conv_afn2corner_pos( IP_MFNR_CROP_CORNER_POS* const dst_corner_pos, const FLOAT xform[6], const IP_MFNR_CROP_IMG_SIZE* const dst_rect )
{
	IP_MFNR_CROP_XY_POS right_offset;
	IP_MFNR_CROP_XY_POS bottom_offset;

	right_offset.pos_x = xform[D_IP_MFNR_CROP_XFORM_IDX_PIT_XX] * (dst_rect->width -1);
	right_offset.pos_y = xform[D_IP_MFNR_CROP_XFORM_IDX_PIT_XY] * (dst_rect->width -1);

	bottom_offset.pos_x = xform[D_IP_MFNR_CROP_XFORM_IDX_PIT_YX] * (dst_rect->lines -1);
	bottom_offset.pos_y = xform[D_IP_MFNR_CROP_XFORM_IDX_PIT_YY] * (dst_rect->lines -1);

#ifdef CO_IP_MFNR_CROP_DBG_PRINT
	ip_mfnr_print_xform          ( "target_xform     ", xform );
	printf( "%.2f %.2f %.2f %.2f %ux%u\n", right_offset.pos_x, right_offset.pos_y, bottom_offset.pos_x, bottom_offset.pos_y, dst_rect->width, dst_rect->lines );
#endif

	dst_corner_pos->left_top.pos_x = xform[D_IP_MFNR_CROP_XFORM_IDX_STA_X];
	dst_corner_pos->left_top.pos_y = xform[D_IP_MFNR_CROP_XFORM_IDX_STA_Y];

	dst_corner_pos->right_top.pos_x = dst_corner_pos->left_top.pos_x + right_offset.pos_x;
	dst_corner_pos->right_top.pos_y = dst_corner_pos->left_top.pos_y + right_offset.pos_y;

	dst_corner_pos->left_bottom.pos_x = dst_corner_pos->left_top.pos_x + bottom_offset.pos_x;
	dst_corner_pos->left_bottom.pos_y = dst_corner_pos->left_top.pos_y + bottom_offset.pos_y;

	dst_corner_pos->right_bottom.pos_x = dst_corner_pos->right_top.pos_x + bottom_offset.pos_x;
	dst_corner_pos->right_bottom.pos_y = dst_corner_pos->right_top.pos_y + bottom_offset.pos_y;
}

static FLOAT ip_mfnr_crop_calc_left_x( const IP_MFNR_CROP_CORNER_POS* const base_corner_pos, const IP_MFNR_CROP_CORNER_POS* const target_corner_pos )
{
	FLOAT	left_x;

	if( target_corner_pos->left_top.pos_x > target_corner_pos->left_bottom.pos_x ) {
		// Role zero or plus degree.
		if( base_corner_pos->left_top.pos_y <= target_corner_pos->left_top.pos_y ) {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route a(5) %.2f %.2f\n", base_corner_pos->left_top.pos_y, target_corner_pos->left_top.pos_y );
#endif
			return ip_mfnr_max( base_corner_pos->left_top.pos_x, target_corner_pos->left_top.pos_x );
		}

		left_x = (target_corner_pos->left_bottom.pos_x - target_corner_pos->left_top.pos_x)
			   * (base_corner_pos->left_top.pos_y - target_corner_pos->left_top.pos_y)
			   / (target_corner_pos->left_bottom.pos_y - target_corner_pos->left_top.pos_y)
			   + target_corner_pos->left_top.pos_x;

		if( left_x > base_corner_pos->left_top.pos_x ) {
			// Cross base top line and target left line.
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route b(4) %.2f %.2f\n", left_x, base_corner_pos->left_top.pos_x );
#endif
			return left_x;
		}
		else {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route c(6) %.2f %.2f\n", left_x, base_corner_pos->left_top.pos_x );
#endif
			return ip_mfnr_max( base_corner_pos->left_bottom.pos_x, target_corner_pos->left_bottom.pos_x );
		}
	}
	else {
		// Role minus degree.
		if( base_corner_pos->left_bottom.pos_y >= target_corner_pos->left_bottom.pos_y ) {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route d(2) %.2f %.2f\n", base_corner_pos->left_bottom.pos_y, target_corner_pos->left_bottom.pos_y );
#endif
			return ip_mfnr_max( base_corner_pos->left_bottom.pos_x, target_corner_pos->left_bottom.pos_x );
		}

		left_x = (target_corner_pos->left_bottom.pos_x - target_corner_pos->left_top.pos_x)
			   * (base_corner_pos->left_bottom.pos_y - target_corner_pos->left_top.pos_y)
			   / (target_corner_pos->left_bottom.pos_y - target_corner_pos->left_top.pos_y)
			   + target_corner_pos->left_top.pos_x;

		if( left_x > base_corner_pos->left_bottom.pos_x ) {
			// Cross base bottom line and target left line.
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route e(1) %.2f %.2f\n", left_x, base_corner_pos->left_bottom.pos_x );
#endif
			return left_x;
		}
		else {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route f(3) %.2f %.2f\n", left_x, base_corner_pos->left_bottom.pos_x );
#endif
			return ip_mfnr_max( base_corner_pos->left_bottom.pos_x, target_corner_pos->left_bottom.pos_x );
		}
	}
}

static FLOAT ip_mfnr_crop_calc_right_x( const IP_MFNR_CROP_CORNER_POS* const base_corner_pos, const IP_MFNR_CROP_CORNER_POS* const target_corner_pos )
{
	FLOAT	right_x;

	if( target_corner_pos->right_top.pos_x > target_corner_pos->right_bottom.pos_x ) {
		// Role zero or plus degree.
		if( base_corner_pos->right_bottom.pos_y >= target_corner_pos->right_bottom.pos_y ) {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route a(6) %.2f %.2f\n", base_corner_pos->right_bottom.pos_y, target_corner_pos->right_bottom.pos_y );
#endif
			return ip_mfnr_min( base_corner_pos->right_bottom.pos_x, target_corner_pos->right_bottom.pos_x );
		}

		right_x = (target_corner_pos->right_bottom.pos_x - target_corner_pos->right_top.pos_x)
				* (base_corner_pos->right_bottom.pos_y - target_corner_pos->right_top.pos_y)
				/ (target_corner_pos->right_bottom.pos_y - target_corner_pos->right_top.pos_y)
				+ target_corner_pos->right_top.pos_x;

		if( right_x < base_corner_pos->right_bottom.pos_x ) {
			// Cross base top line and target right line.
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route b(4) %.2f %.2f\n", right_x, base_corner_pos->right_bottom.pos_x );
#endif
			return right_x;
		}
		else {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route c(5) %.2f %.2f\n", right_x, base_corner_pos->right_bottom.pos_x );
#endif
			return ip_mfnr_min( base_corner_pos->right_bottom.pos_x, target_corner_pos->right_bottom.pos_x );
		}
	}
	else {
		// Role minus degree.
		if( base_corner_pos->right_top.pos_y <= target_corner_pos->right_top.pos_y ) {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route d(3) %.2f %.2f\n", base_corner_pos->right_top.pos_y, target_corner_pos->right_top.pos_y );
#endif
			return ip_mfnr_min( base_corner_pos->right_top.pos_x, target_corner_pos->right_top.pos_x );
		}

		right_x = (target_corner_pos->right_bottom.pos_x - target_corner_pos->right_top.pos_x)
				* (base_corner_pos->right_top.pos_y - target_corner_pos->right_top.pos_y)
				/ (target_corner_pos->right_bottom.pos_y - target_corner_pos->right_top.pos_y)
				+ target_corner_pos->right_top.pos_x;

		if( right_x < base_corner_pos->right_top.pos_x ) {
			// Cross base bottom line and target right line.
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route e(1) %.2f %.2f\n", right_x, base_corner_pos->right_top.pos_x );
#endif
			return right_x;
		}
		else {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route f(2) %.2f %.2f\n", right_x, base_corner_pos->right_top.pos_x );
#endif
			return ip_mfnr_min( base_corner_pos->right_top.pos_x, target_corner_pos->right_top.pos_x );
		}
	}
}

static FLOAT ip_mfnr_crop_calc_top_y( const IP_MFNR_CROP_CORNER_POS* const base_corner_pos, const IP_MFNR_CROP_CORNER_POS* const target_corner_pos )
{
	FLOAT	top_y;

	if( target_corner_pos->left_top.pos_y <= target_corner_pos->right_top.pos_y ) {
		// Role zero or plus degree.
		if( base_corner_pos->right_top.pos_x >= target_corner_pos->right_top.pos_x ) {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route a(2) %.2f %.2f\n", base_corner_pos->right_top.pos_x, target_corner_pos->right_top.pos_x );
#endif
			return ip_mfnr_max( base_corner_pos->right_top.pos_y, target_corner_pos->right_top.pos_y );
		}

		top_y = (target_corner_pos->right_top.pos_y - target_corner_pos->left_top.pos_y)
			  * (base_corner_pos->right_top.pos_x - target_corner_pos->left_top.pos_x)
			  / (target_corner_pos->right_top.pos_x - target_corner_pos->left_top.pos_x)
			  + target_corner_pos->left_top.pos_y;

		if( top_y > base_corner_pos->right_top.pos_y ) {
			// Cross base top line and target left line.
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route b(1) %.2f %.2f\n", top_y, base_corner_pos->right_top.pos_y );
#endif
			return top_y;
		}
		else {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route c(3) %.2f %.2f\n", top_y, base_corner_pos->right_top.pos_y );
#endif
			return ip_mfnr_max( base_corner_pos->right_top.pos_y, target_corner_pos->right_top.pos_y );
		}
	}
	else {
		// Role minus degree.
		if( base_corner_pos->left_top.pos_x <= target_corner_pos->left_top.pos_x ) {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route d(5) %.2f %.2f\n", base_corner_pos->left_top.pos_x, target_corner_pos->left_top.pos_x );
#endif
			return ip_mfnr_max( base_corner_pos->left_top.pos_y, target_corner_pos->left_top.pos_y );
		}

		top_y = (target_corner_pos->right_top.pos_y - target_corner_pos->left_top.pos_y)
			  * (base_corner_pos->left_top.pos_x - target_corner_pos->left_top.pos_x)
			  / (target_corner_pos->right_top.pos_x - target_corner_pos->left_top.pos_x)
			  + target_corner_pos->left_top.pos_y;

		if( top_y > base_corner_pos->left_top.pos_y ) {
			// Cross base bottom line and target left line.
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route e(4) %.2f %.2f\n", top_y, base_corner_pos->left_top.pos_y );
#endif
			return top_y;
		}
		else {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route f(6) %.2f %.2f\n", top_y, base_corner_pos->left_top.pos_y );
#endif
			return ip_mfnr_max( base_corner_pos->left_top.pos_y, target_corner_pos->left_top.pos_y );
		}
	}
}

static FLOAT ip_mfnr_crop_calc_bottom_y( const IP_MFNR_CROP_CORNER_POS* const base_corner_pos, const IP_MFNR_CROP_CORNER_POS* const target_corner_pos )
{
	FLOAT	bottom_y;

	if( target_corner_pos->left_bottom.pos_y <= target_corner_pos->right_bottom.pos_y ) {
		// Role zero or plus degree.
		if( base_corner_pos->left_bottom.pos_x <= target_corner_pos->left_bottom.pos_x ) {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route d(3) %.2f %.2f\n", base_corner_pos->left_bottom.pos_x, target_corner_pos->left_bottom.pos_x );
#endif
			return ip_mfnr_min( base_corner_pos->left_bottom.pos_y, target_corner_pos->left_bottom.pos_y );
		}

		bottom_y = (target_corner_pos->right_bottom.pos_y - target_corner_pos->left_bottom.pos_y)
				 * (base_corner_pos->left_bottom.pos_x - target_corner_pos->left_bottom.pos_x)
				 / (target_corner_pos->right_bottom.pos_x - target_corner_pos->left_bottom.pos_x)
				 + target_corner_pos->left_bottom.pos_y;

		if( bottom_y < base_corner_pos->left_bottom.pos_y ) {
			// Cross base bottom line and target left line.
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route e(1) %.2f %.2f\n", bottom_y, base_corner_pos->left_bottom.pos_y );
#endif
			return bottom_y;
		}
		else {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route f(2) %.2f %.2f\n", bottom_y, base_corner_pos->left_bottom.pos_y );
#endif
			return ip_mfnr_min( base_corner_pos->left_bottom.pos_y, target_corner_pos->left_bottom.pos_y );
		}
	}
	else {
		// Role minus degree.
		if( base_corner_pos->right_bottom.pos_x >= target_corner_pos->right_bottom.pos_x ) {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route a(6) %.2f %.2f\n", base_corner_pos->right_bottom.pos_x, target_corner_pos->right_bottom.pos_x );
#endif
			return ip_mfnr_min( base_corner_pos->right_bottom.pos_y, target_corner_pos->right_bottom.pos_y );
		}

		bottom_y = (target_corner_pos->right_bottom.pos_y - target_corner_pos->left_bottom.pos_y)
				 * (base_corner_pos->right_bottom.pos_x - target_corner_pos->left_bottom.pos_x)
				 / (target_corner_pos->right_bottom.pos_x - target_corner_pos->left_bottom.pos_x)
				 + target_corner_pos->left_bottom.pos_y;

		if( bottom_y < base_corner_pos->right_bottom.pos_y ) {
			// Cross base bottom line and target left line.
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route b(4) %.2f %.2f\n", bottom_y, base_corner_pos->right_bottom.pos_y );
#endif
			return bottom_y;
		}
		else {
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
			FJ_Debug_Print_Info( "route c(5) %.2f %.2f\n", bottom_y, base_corner_pos->right_bottom.pos_y );
#endif
			return ip_mfnr_min( base_corner_pos->right_bottom.pos_y, target_corner_pos->right_bottom.pos_y );
		}
	}
}

static UCHAR ip_mfnr_get_pixel_avg_reduction_info( UINT32 srcSize, UINT32 dstSize, USHORT* pitch, UCHAR* rdc_md )
{
	FLOAT	ratio;
	UCHAR	rdc_en;

	ratio = (FLOAT)srcSize / dstSize;

	if ( ratio == 1.0f ) {
		rdc_en	= D_IM_SHDR_DISABLE;
		*rdc_md	= D_IM_RDCMD_1_2;	// dummy
		*pitch	= 0x4000;
	}
	else if ( ratio < 2.0f ) {
		rdc_en	= D_IM_SHDR_DISABLE;
		*rdc_md	= D_IM_RDCMD_1_2;	// dummy
		*pitch	= (USHORT)ip_mfnr_calc_pit( srcSize, dstSize );
	}
	else if ( ratio < 4.0f ) {
		rdc_en	= D_IM_SHDR_ENABLE;
		*rdc_md	= D_IM_RDCMD_1_2;
		*pitch	= (USHORT)ip_mfnr_calc_pit( srcSize, dstSize * 2 );
	}
	else if ( ( ratio >= 4.0f ) && ( ratio <= 8.0f ) ) {
		rdc_en	= D_IM_SHDR_ENABLE;
		*rdc_md	= D_IM_RDCMD_1_4;
		*pitch	= (USHORT)ip_mfnr_calc_pit( srcSize, dstSize * 4 );
	}
	else {
		rdc_en	= D_IM_SHDR_ENABLE;
		*rdc_md	= D_IM_RDCMD_1_8;
		*pitch	= (USHORT)ip_mfnr_calc_pit( srcSize, dstSize * 8 );
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

	return rdc_en;
}

static INT32 ip_mfnr_ctrl_gmd( IP_MFNR_START_PRM* startPrm, const ULONG heapAddr, const ULONG heapSize )
{
#ifndef CO_DEBUG_ON_PC
	T_IPC_GMD_CTRL_PRM	ctrlPrm;
	IPC_ULONG			heapSize_gmd;

	heapSize_gmd		= (IPC_ULONG)heapSize;

	ctrlPrm.heap_addr	= heapAddr;
	ctrlPrm.width		= startPrm->imgSrc_2nd[ 1 ].imgInfo[ 0 ].wImgSize;
	ctrlPrm.height		= startPrm->imgSrc_2nd[ 1 ].imgInfo[ 0 ].hImgSize;
	ctrlPrm.full_width	= startPrm->imgSrc_2nd[ 0 ].imgInfo[ 0 ].wImgSize;
	ctrlPrm.full_height	= startPrm->imgSrc_2nd[ 0 ].imgInfo[ 0 ].hImgSize;

	IPC_INT ret = IPC_GMD_Ctrl( &heapSize_gmd, &ctrlPrm );
	if ( ret != D_IPC_GMD_OK ) {
		FJ_Debug_Print_Err( "IPC_GMD_Ctrl() error ret=0x%08x heap=%lu.\n", ret, heapSize_gmd );
		return D_IP_MFNR_NG;
	}
#else
	OS_User_Dly_Tsk( 10 );
#endif

	return D_IP_MFNR_OK;
}

static INT32 ip_mfnr_start_gmd( IP_MFNR_START_PRM* startPrm, IPC_FLOAT* affin )
{
#ifndef CO_DEBUG_ON_PC
	T_IPC_GMD_START_PRM		startPrm_gmd;
	T_IPC_GMD_EXP_PRM		expPrm;

	expPrm.brc_en						= 0;													// Brightness correct enable.
	expPrm.brc_mod						= 0;													// Brightness correct mode(for Base).
	expPrm.lut_sel						= 0;													// LUT table select(LUT0).
	expPrm.gainup_tbl0					= &gIp_Mfnr_Gainup_Tbl0[ 0 ];							// LUT0 table start address.
	expPrm.gainup_tbl1					= &gIp_Mfnr_Gainup_Tbl1[ 0 ];							// LUT1 table start address.

	startPrm_gmd.sel_affin					= 1;												// affin calc.
	startPrm_gmd.shift_permissible_range	= 4;												// 4%.
	startPrm_gmd.affin_permissible_degree	= 1;												// 1 degree.
	startPrm_gmd.affin_permissible_expand	= 1;												// 1%.
	memcpy( &startPrm_gmd.base_img,        &startPrm->imgSrc_2nd[ 1 ], sizeof(T_IPC_IMAGE) );	// base   reduced image.
	memcpy( &startPrm_gmd.target_img,      &startPrm->imgSrc_1st[ 1 ], sizeof(T_IPC_IMAGE) );	// target reduced image.
	memcpy( &startPrm_gmd.base_full_img,   &startPrm->imgSrc_2nd[ 0 ], sizeof(T_IPC_IMAGE) );	// base   full    image.
	memcpy( &startPrm_gmd.target_full_img, &startPrm->imgSrc_1st[ 0 ], sizeof(T_IPC_IMAGE) );	// target full    image.

	// GMD start.
	IPC_INT ret = IPC_GMD_Start( &startPrm_gmd, &expPrm );
	if ( ret != D_IPC_GMD_OK ) {
		FJ_Debug_Print_Wrn( "IPC_GMD_Start() error ret=0x%08x.\n", ret );
	}

	affin[ 0 ] = startPrm_gmd.affin_a;
	affin[ 1 ] = startPrm_gmd.affin_b;
	affin[ 2 ] = startPrm_gmd.affin_c;
	affin[ 3 ] = startPrm_gmd.affin_d;
	affin[ 4 ] = startPrm_gmd.affin_e;
	affin[ 5 ] = startPrm_gmd.affin_f;

#else
	OS_User_Dly_Tsk( 10 );

	for ( int i = 0; i < D_IP_MFNR_AFFIN_MAX ; i++ ) {
		affin[ i ] = 0.0f;
	}
#endif

	return D_IP_MFNR_OK;
}

static INT32 ip_mfnr_start_sgde( IP_MFNR_START_PRM* startPrm, FJ_STILL_IP_IMAGE* sgdeHeap, IPC_FLOAT* affin )
{
#ifndef CO_DEBUG_ON_PC
	T_IP_GE_LGRID	local		= {
									.width		= 2,
									.height		= 2,
									.pitchX		= 6,
									.pitchY		= 5,	// 6 is not used.
									.outputX	= 0,
									.outputY	= 0,
									.nodes		= NULL,
									.xform		= NULL,
								};
	FLOAT						xform[ 9 ]	= { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	T_IP_GE_IMAGE	in_frame;
	T_IP_GE_IMAGE	out_frame[ 2 ];

	for ( int i = 0; i < D_IP_MFNR_AFFIN_MAX ; i++ ) {
		xform[ i ] = (FLOAT)affin[ i ];
	}

	local.width		= ( ( startPrm->imgSrc_2nd[ 0 ].imgInfo[ 0 ].wImgSize + 63) / 64 ) + 1;	// pitchX = 6.
	local.height	= ( ( startPrm->imgSrc_2nd[ 0 ].imgInfo[ 0 ].hImgSize + 31) / 32 ) + 1;	// pitchY = 5.

	INT32 ret = IP_GE_SetCorrectParameter( NULL, 1, &local, xform );
	if ( ret != D_IP_GE_OK ) {
		FJ_Debug_Print_Err( "IP_GE_SetCorrectParameter() error ret=0x%08x.\n", ret );
		return D_IP_MFNR_NG;
	}

	in_frame.format						= E_IP_GE_FMT_YUV_422_P2;
	in_frame.width						= (IPC_UINT)startPrm->imgSrc_2nd[ 0 ].imgInfo[ 0 ].wImgSize;
	in_frame.height						= (IPC_UINT)startPrm->imgSrc_2nd[ 0 ].imgInfo[ 0 ].hImgSize;
	in_frame.planes[ 0 ].stride			= (IPC_INT)startPrm->imgSrc_2nd[ 0 ].imgInfo[ 0 ].gImgSize;
	in_frame.planes[ 0 ].data			= (IPC_VOID*)startPrm->imgSrc_2nd[ 0 ].imgInfo[ 0 ].pImgAdrs;
	in_frame.planes[ 1 ].stride			= (IPC_INT)startPrm->imgSrc_2nd[ 0 ].imgInfo[ 1 ].gImgSize;
	in_frame.planes[ 1 ].data			= (IPC_VOID*)startPrm->imgSrc_2nd[ 0 ].imgInfo[ 1 ].pImgAdrs;

	out_frame[ 0 ].format				= E_IP_GE_FMT_YUV_422_P2;
	out_frame[ 0 ].width				= (IPC_UINT)sgdeHeap[ 0 ].imgInfo[ 0 ].wImgSize;
	out_frame[ 0 ].height				= (IPC_UINT)sgdeHeap[ 0 ].imgInfo[ 0 ].hImgSize;
	out_frame[ 0 ].planes[ 0 ].stride	= (IPC_INT)sgdeHeap[ 0 ].imgInfo[ 0 ].gImgSize;
	out_frame[ 0 ].planes[ 0 ].data		= (IPC_VOID*)sgdeHeap[ 0 ].imgInfo[ 0 ].pImgAdrs;
	out_frame[ 0 ].planes[ 1 ].stride	= (IPC_INT)sgdeHeap[ 0 ].imgInfo[ 1 ].gImgSize;
	out_frame[ 0 ].planes[ 1 ].data		= (IPC_VOID*)sgdeHeap[ 0 ].imgInfo[ 1 ].pImgAdrs;

	out_frame[ 1 ].format				= E_IP_GE_FMT_YUV_422_P2;
	out_frame[ 1 ].width				= (IPC_UINT)in_frame.width  >> 2;
	out_frame[ 1 ].height				= (IPC_UINT)in_frame.height >> 2;
	out_frame[ 1 ].planes[ 0 ].stride	= (IPC_INT)ip_mfnr_roundup_256( in_frame.planes[ 0 ].stride >> 2 );
	out_frame[ 1 ].planes[ 0 ].data		= (IPC_VOID*)sgdeHeap[ 1 ].imgInfo[ 0 ].pImgAdrs;
	out_frame[ 1 ].planes[ 1 ].stride	= (IPC_INT)out_frame[ 1 ].planes[ 0 ].stride;
	out_frame[ 1 ].planes[ 1 ].data		= (IPC_VOID*)sgdeHeap[ 1 ].imgInfo[ 1 ].pImgAdrs;

	ret = IP_GE_Process( &in_frame, &out_frame[ 0 ], NULL, &out_frame[ 1 ], (IPC_INT)D_IP_MFNR_WAIT_TIME );
	if ( ret != D_IP_GE_OK ) {
		FJ_Debug_Print_Err( "IP_GE_Process() error ret=0x%08x.\n", ret );
		return D_IP_MFNR_NG;
	}


// delete\basefw\fj\imgproc\still
#if 0
	FJ_ERR_CODE			fj_ercd;
	FJ_IMG_INFO			src, dst;
	T_IMAGE_ADDR_YCC	image_addr_ycc;

	image_addr_ycc.y	= Sdram_Map_Mgr_Change_CNR_Work();

	src.addr.AddressY	= (ULONG)out_frame[ 1 ].planes[ 0 ].data;
	src.addr.AddressCb	= (ULONG)out_frame[ 1 ].planes[ 1 ].data;
	src.addr.AddressCr	= 0;
	src.g_y_width		= out_frame[ 1 ].planes[ 0 ].stride;
	src.g_c_width		= out_frame[ 1 ].planes[ 1 ].stride;
	src.width			= out_frame[ 1 ].width;
	src.lines			= out_frame[ 1 ].height;
	src.pfmt			= FJ_IMG_PFMT_YCC422;

	dst.g_y_width		= src.g_y_width;
	dst.g_c_width		= src.g_c_width;
	dst.width			= src.width;
	dst.lines			= ip_mfnr_roundup_32( src.lines );
	dst.pfmt			= FJ_IMG_PFMT_VIDEO;
	dst.addr.AddressY	= image_addr_ycc.y;
	dst.addr.AddressCb	= dst.addr.AddressY + ( dst.g_y_width * dst.lines );
	dst.addr.AddressCr	= 0;

	fj_ercd = BF_Still_Proc_Subr_Format_Convert( &src, &dst, 0, 0 );
	if ( fj_ercd != FJ_ERR_OK ) {
		FJ_Debug_Print_Err( "BF_Still_Proc_Subr_Format_Convert() ercd=0x%08x.\n", fj_ercd );
	}

	sgdeHeap[ 1 ].imgInfo[ 0 ].pImgAdrs = (VOID*)dst.addr.AddressY;
	sgdeHeap[ 1 ].imgInfo[ 1 ].pImgAdrs = (VOID*)dst.addr.AddressCb;
	sgdeHeap[ 1 ].imgInfo[ 0 ].hImgSize = dst.lines;

#endif

#else
	OS_User_Dly_Tsk( 5 );
#endif

	return D_IP_MFNR_OK;
}

static INT32 ip_mfnr_start_sgde_edgecut( FLOAT* crop_xform, FJ_STILL_IP_IMAGE* imgSrc, FJ_STILL_IP_IMAGE* imgDst )
{
#ifndef CO_DEBUG_ON_PC
	T_IP_GE_LGRID	local	= {
								.width		= 2,
								.height		= 2,
								.pitchX		= 6,
								.pitchY		= 5,	// 6 is not used.
								.outputX	= 0,
								.outputY	= 0,
								.nodes		= NULL,
								.xform		= NULL,
							};
	FLOAT			xform[ 9 ]	= { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	T_IP_GE_IMAGE	in_frame;
	T_IP_GE_IMAGE	out_frame;

	local.width		= ( ( imgSrc->imgInfo[ 0 ].wImgSize + 63) / 64 ) + 1;	// pitchX = 6.
	local.height	= ( ( imgSrc->imgInfo[ 0 ].hImgSize + 31) / 32 ) + 1;	// pitchY = 5.

#if 1
#ifdef CO_IP_MFNR_CROP_DBG_PRINT
		ip_mfnr_print_xform( "last_xform", crop_xform );
#endif
	for ( int i = 0; i < D_IP_MFNR_AFFIN_MAX ; i++ ) {
		xform[ i ] = (FLOAT)crop_xform[ i ];
	}
#else
	xform[ 0 ] = 1.0f;
	xform[ 4 ] = 1.0f;
	xform[ 2 ] = 32.0f;
	xform[ 5 ] = 32.0f;
#endif

	INT32 ret = IP_GE_SetCorrectParameter( NULL, 1, &local, xform );
	if ( ret != D_IP_GE_OK ) {
		FJ_Debug_Print_Err( "IP_GE_SetCorrectParameter() error ret=0x%08x.\n", ret );
		return D_IP_MFNR_NG;
	}

	in_frame.format					= E_IP_GE_FMT_YUV_422_P2;
	in_frame.width					= (IPC_UINT)imgSrc->imgInfo[ 0 ].wImgSize;
	in_frame.height					= (IPC_UINT)imgSrc->imgInfo[ 0 ].hImgSize;
	in_frame.planes[ 0 ].stride		= (IPC_INT)imgSrc->imgInfo[ 0 ].gImgSize;
	in_frame.planes[ 0 ].data		= (IPC_VOID*)imgSrc->imgInfo[ 0 ].pImgAdrs;
	in_frame.planes[ 1 ].stride		= (IPC_INT)imgSrc->imgInfo[ 1 ].gImgSize;
	in_frame.planes[ 1 ].data		= (IPC_VOID*)imgSrc->imgInfo[ 1 ].pImgAdrs;

	out_frame.format				= E_IP_GE_FMT_YUV_422_P2;
	out_frame.width					= (IPC_UINT)imgDst->imgInfo[ 0 ].wImgSize;
	out_frame.height				= (IPC_UINT)imgDst->imgInfo[ 0 ].hImgSize;
	out_frame.planes[ 0 ].stride	= (IPC_INT)imgDst->imgInfo[ 0 ].gImgSize;
	out_frame.planes[ 0 ].data		= (IPC_VOID*)imgDst->imgInfo[ 0 ].pImgAdrs;
	out_frame.planes[ 1 ].stride	= (IPC_INT)imgDst->imgInfo[ 1 ].gImgSize;
	out_frame.planes[ 1 ].data		= (IPC_VOID*)imgDst->imgInfo[ 1 ].pImgAdrs;

	ret = IP_GE_Process( &in_frame, &out_frame, NULL, NULL, (IPC_INT)D_IP_MFNR_WAIT_TIME );
	if ( ret != D_IP_GE_OK ) {
		FJ_Debug_Print_Err( "IP_GE_Process() error ret=0x%08x.\n", ret );
		return D_IP_MFNR_NG;
	}
#else
	OS_User_Dly_Tsk( 5 );
#endif

	return D_IP_MFNR_OK;
}

static INT32 ip_mfnr_ctrl_me( VOID )
{
	// Callback set.
	gIp_Mfnr_Me_Ctrl_Prm.pCallBack = (T_IM_ME_CALLBACK)ip_mfnr_int_handler_me;

#ifndef CO_DEBUG_ON_PC
	// ME macro driver set.
	INT32 ercd = Im_ME_Ctrl( (T_IM_ME_CTRL*)&gIp_Mfnr_Me_Ctrl_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_ME_Ctrl() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}
#else
	OS_User_Dly_Tsk( 5 );
#endif

	return D_IP_MFNR_OK;
}

static INT32 ip_mfnr_start_me( const FJ_STILL_IP_IMAGE* const imgSrc, const FJ_STILL_IP_IMAGE* const imgRef, FJ_STILL_IP_IMAGE* const imgMe )
{
	USHORT	vec_hsize;

	if ( imgSrc->imgInfo[ 0 ].wImgSize <= D_IM_ME_PRE_VEC_HSIZE_1024 ) {
		vec_hsize = D_IM_ME_PRE_VEC_HSIZE_512;
	}
	else {
		vec_hsize = D_IM_ME_PRE_VEC_HSIZE_1024;
	}

	// Vector output foramt horizontal size set.
	for ( int i = 0; i < E_IM_ME_PPNUM_MAX; i++ ) {
		gIp_Mfnr_Me_Start_Prm.pme[ i ].vec_hsize = vec_hsize;
	}

	// Previous image base address(16K byte align).
	gIp_Mfnr_Me_Start_Prm.rmcif.pre_r_addr_hi		= (USHORT)((ULONG)imgRef->imgInfo[ 0 ].pImgAdrs >> 16);
	gIp_Mfnr_Me_Start_Prm.rmcif.pre_r_addr_lo		= (USHORT)((ULONG)imgRef->imgInfo[ 0 ].pImgAdrs & 0x0000FFFF);

	// Current image base address(16K byte align).
	gIp_Mfnr_Me_Start_Prm.rmcif.cur_r_addr_hi		= (USHORT)((ULONG)imgSrc->imgInfo[ 0 ].pImgAdrs >> 16);
	gIp_Mfnr_Me_Start_Prm.rmcif.cur_r_addr_lo		= (USHORT)((ULONG)imgSrc->imgInfo[ 0 ].pImgAdrs & 0x0000FFFF);

	// Vector information base address(1K byte align).
	gIp_Mfnr_Me_Start_Prm.rmcif.vec_w_addr_hi		= (USHORT)((ULONG)imgMe->imgInfo[ 0 ].pImgAdrs >> 16);
	gIp_Mfnr_Me_Start_Prm.rmcif.vec_w_addr_lo		= (USHORT)((ULONG)imgMe->imgInfo[ 0 ].pImgAdrs & 0x0000FFFF);

	// Current image size.
	gIp_Mfnr_Me_Start_Prm.rmcif.cur_gwidth			= imgSrc->imgInfo[ 0 ].gImgSize;
	gIp_Mfnr_Me_Start_Prm.rmcif.cur_margin_width	= D_IM_ME_CUR_MARGIN_8;
	gIp_Mfnr_Me_Start_Prm.rmcif.cur_margin_height	= D_IM_ME_CUR_MARGIN_8;

	// Previous image size.
	gIp_Mfnr_Me_Start_Prm.rmcif.pre_gwidth			= imgRef->imgInfo[ 0 ].gImgSize;
	gIp_Mfnr_Me_Start_Prm.rmcif.pre_width			= ip_mfnr_roundup_4( imgRef->imgInfo[ 0 ].wImgSize - ( gIp_Mfnr_Me_Start_Prm.rmcif.cur_margin_width  * 2 ) );
	gIp_Mfnr_Me_Start_Prm.rmcif.pre_height			= ip_mfnr_roundup_4( imgRef->imgInfo[ 0 ].hImgSize - ( gIp_Mfnr_Me_Start_Prm.rmcif.cur_margin_height * 2 ) );

	// Prefetch image size.
	gIp_Mfnr_Me_Start_Prm.rmcif.prefetch_width		= ( gIp_Mfnr_Me_Start_Prm.rmcif.pre_width  + ( gIp_Mfnr_Me_Start_Prm.rmcif.cur_margin_width  * 2 ) ) * 4;
	gIp_Mfnr_Me_Start_Prm.rmcif.prefetch_height		= ( gIp_Mfnr_Me_Start_Prm.rmcif.pre_height + ( gIp_Mfnr_Me_Start_Prm.rmcif.cur_margin_height * 2 ) ) * 4;

	// ME1 initialize search size.
	if ( gIp_Mfnr_Me_Start_Prm.rmcif.cur_margin_width >= D_IM_ME_CUR_MARGIN_4 ) {
		if ( gIp_Mfnr_Me_Start_Prm.rmcif.pre_width <= D_IM_ME_SRCH_WIDTH_MAX ) {
			gIp_Mfnr_Me_Start_Prm.rmcif.search_width = gIp_Mfnr_Me_Start_Prm.rmcif.pre_width;
		}
		else {
			gIp_Mfnr_Me_Start_Prm.rmcif.search_width = D_IM_ME_SRCH_WIDTH_MAX;
		}
	}
	else {
		if ( ( gIp_Mfnr_Me_Start_Prm.rmcif.pre_width - 8 ) <= D_IM_ME_SRCH_WIDTH_MAX ) {
			gIp_Mfnr_Me_Start_Prm.rmcif.search_width = gIp_Mfnr_Me_Start_Prm.rmcif.pre_width - 8;
		}
		else {
			gIp_Mfnr_Me_Start_Prm.rmcif.search_width = D_IM_ME_SRCH_WIDTH_MAX;
		}
	}

	if ( gIp_Mfnr_Me_Ctrl_Prm.rmcif.pme_num == 2 ) {
		gIp_Mfnr_Me_Start_Prm.rmcif.search_height = 40;
	}
	else if ( gIp_Mfnr_Me_Ctrl_Prm.rmcif.pme_num == 1 ) {
		gIp_Mfnr_Me_Start_Prm.rmcif.search_height = 32;
	}
	else {
		gIp_Mfnr_Me_Start_Prm.rmcif.search_height = 24;
	}

	// MB number set.
	for ( int i = 0; i < E_IM_ME_PPNUM_MAX; i++ ) {

		gIp_Mfnr_Me_Start_Prm.pme[ i ].mbnum_h = gIp_Mfnr_Me_Start_Prm.rmcif.prefetch_width  / 16;

		if ( gIp_Mfnr_Me_Start_Prm.pme[ i ].mbnum_h > D_IM_ME_MBNUM_H_MAX ) {
			gIp_Mfnr_Me_Start_Prm.pme[ i ].mbnum_h = D_IM_ME_MBNUM_H_MAX;
		}

		gIp_Mfnr_Me_Start_Prm.pme[ i ].mbnum_v = gIp_Mfnr_Me_Start_Prm.rmcif.prefetch_height / 16;

		if ( gIp_Mfnr_Me_Start_Prm.pme[ i ].mbnum_v > D_IM_ME_MBNUM_V_MAX ) {
			gIp_Mfnr_Me_Start_Prm.pme[ i ].mbnum_v = D_IM_ME_MBNUM_V_MAX;
		}
	}

#ifndef CO_DEBUG_ON_PC
	UINT32	wait_factor_result = 0;

	INT32 ercd = Im_ME_Start( (T_IM_ME_START*)&gIp_Mfnr_Me_Start_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_ME_Start() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}

	ercd = Im_ME_Wait_End( &wait_factor_result, D_IP_MFNR_WAIT_TIME );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_ME_Wait_End() error. ercd[0x%08x],  wait_factor_result = 0x%x\n", ercd, wait_factor_result );
		return D_IP_MFNR_NG;
	}
#else
	OS_User_Dly_Tsk( 1 );
#endif

	return D_IP_MFNR_OK;
}

static INT32 ip_mfnr_ctrl_shdr( VOID )
{
#ifndef CO_DEBUG_ON_PC
#if defined(CO_IQBIN_GENERAL)	// --- REMOVE_IQBIN_GENERAL BEGIN ---
	gIp_Mfnr_Shdr_Ctrl_Prm.image_mode.shdr_in = gIp_Mfnr_ShdrIn_Prm;

#endif // CO_IQBIN_GENERAL  --- REMOVE_IQBIN_GENERAL END ---
	INT32 ercd = Im_SHDR_Set_Pre( (T_IM_SHDR_PRE*)&gIp_Mfnr_Pre_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Pre() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}

	ercd = Im_SHDR_Set_Fmd( (T_IM_SHDR_FMD*)&gIp_Mfnr_Fmd_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Fmd() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}

	ercd = Im_SHDR_Set_PreMask( (T_IM_SHDR_PMSK*)&gIp_Mfnr_Pmsk_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_PreMask() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}

	ercd = Im_SHDR_Set_Spnr( (T_IM_SHDR_SPNR*)&gIp_Mfnr_Spnr_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Spnr() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}

	// SHDR macro driver set/start/end.
	ercd = Im_SHDR_Set_Blend( (T_IM_SHDR_BLD*)&gIp_Mfnr_Bld_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Blend() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}

	ercd = Im_SHDR_Set_Post( (T_IM_SHDR_POST*)&gIp_Mfnr_Post_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Post() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}

	ercd = Im_SHDR_Set_Smc( (T_IM_SHDR_SMC*)&gIp_Mfnr_Smc_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Smc() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}

	ercd = Im_SHDR_Set_Axi( (T_IM_SHDR_AXI*)&gIp_Mfnr_Axi_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Axi() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}

	ercd = Im_SHDR_Set_Degamma( (USHORT*)&gIp_Mfnr_Degamma_Prm[ 0 ][ 0 ],
								(USHORT*)&gIp_Mfnr_Degamma_Prm[ 1 ][ 0 ],
								(USHORT*)&gIp_Mfnr_Degamma_Prm[ 2 ][ 0 ],
								0,
								D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Degamma() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}
#else
	OS_User_Dly_Tsk( 10 );
#endif

	return D_IP_MFNR_OK;
}

static INT32 ip_mfnr_start_shdr( const	UCHAR					 count,
								 E_IP_MFNR_FIL_MARGIN			 filMargin,
								 const	FJ_STILL_IP_IMAGE* const imgSrc,
								 const	FJ_STILL_IP_IMAGE* const imgRef,
								 const	FJ_STILL_IP_IMAGE* const imgMe,
										FJ_STILL_IP_IMAGE* const imgDst_Full,
										FJ_STILL_IP_IMAGE* const imgDst_Red )
{
	INT32 ercd;

	// Set SHDR register include IQ value.
	ercd = ip_mfnr_ctrl_shdr();
	if ( ercd != D_IP_MFNR_OK ) {
		FJ_Debug_Print_Err( "ip_mfnr_ctrl_shdr error. ercd=0x%08x.\n", ercd );
		return D_IP_MFNR_NG;
	}

	switch ( count ) {
		case 1:
			gIp_Mfnr_Msk_Prm.norm_gain[ 0 ]		= 128;
			gIp_Mfnr_Msk_Prm.norm_gain[ 1 ]		= 128;
			gIp_Mfnr_Msk_Prm.norm_gain[ 2 ]		= 128;
			gIp_Mfnr_Msk_Prm.norm_offset[ 0 ]	= 128;
			gIp_Mfnr_Msk_Prm.norm_offset[ 1 ]	= 128;
			gIp_Mfnr_Msk_Prm.norm_offset[ 2 ]	= 128;
			break;
		case 2:
			gIp_Mfnr_Msk_Prm.norm_gain[ 0 ]		= 171;
			gIp_Mfnr_Msk_Prm.norm_gain[ 1 ]		= 171;
			gIp_Mfnr_Msk_Prm.norm_gain[ 2 ]		= 171;
			gIp_Mfnr_Msk_Prm.norm_offset[ 0 ]	=  85;
			gIp_Mfnr_Msk_Prm.norm_offset[ 1 ]	=  85;
			gIp_Mfnr_Msk_Prm.norm_offset[ 2 ]	=  85;
			break;
		case 3:
			gIp_Mfnr_Msk_Prm.norm_gain[ 0 ]		= 192;
			gIp_Mfnr_Msk_Prm.norm_gain[ 1 ]		= 192;
			gIp_Mfnr_Msk_Prm.norm_gain[ 2 ]		= 192;
			gIp_Mfnr_Msk_Prm.norm_offset[ 0 ]	=  64;
			gIp_Mfnr_Msk_Prm.norm_offset[ 1 ]	=  64;
			gIp_Mfnr_Msk_Prm.norm_offset[ 2 ]	=  64;
			break;
		case 4:
			gIp_Mfnr_Msk_Prm.norm_gain[ 0 ]		= 205;
			gIp_Mfnr_Msk_Prm.norm_gain[ 1 ]		= 205;
			gIp_Mfnr_Msk_Prm.norm_gain[ 2 ]		= 205;
			gIp_Mfnr_Msk_Prm.norm_offset[ 0 ]	=  51;
			gIp_Mfnr_Msk_Prm.norm_offset[ 1 ]	=  51;
			gIp_Mfnr_Msk_Prm.norm_offset[ 2 ]	=  51;
			break;
		case 5:
			gIp_Mfnr_Msk_Prm.norm_gain[ 0 ]		= 213;
			gIp_Mfnr_Msk_Prm.norm_gain[ 1 ]		= 213;
			gIp_Mfnr_Msk_Prm.norm_gain[ 2 ]		= 213;
			gIp_Mfnr_Msk_Prm.norm_offset[ 0 ]	=  43;
			gIp_Mfnr_Msk_Prm.norm_offset[ 1 ]	=  43;
			gIp_Mfnr_Msk_Prm.norm_offset[ 2 ]	=  43;
			break;
		default :
			break;
	}

	// MRGN.
	if ( filMargin == E_IP_MFNR_FIL_MARGIN_16 ) {
		gIp_Mfnr_Shdr_Ctrl_Prm.image_mode.fil_margin = D_IM_SHDR_FLITER_MARGIN_16;
	}
	else {
		gIp_Mfnr_Shdr_Ctrl_Prm.image_mode.fil_margin = D_IM_SHDR_FLITER_MARGIN_32;
	}

	// SDRAM for read (MC input standard).
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_addr.c_y		= (ULONG)imgSrc->imgInfo[ 0 ].pImgAdrs;			// Source image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_addr.c_c		= (ULONG)imgSrc->imgInfo[ 1 ].pImgAdrs;			// Source image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_ghsize.c_y	= imgSrc->imgInfo[ 0 ].gImgSize;				// Source image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_ghsize.c_c	= imgSrc->imgInfo[ 1 ].gImgSize;				// Source image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_hsize.c		= imgSrc->imgInfo[ 0 ].wImgSize;				// Source image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_vsize.c		= imgSrc->imgInfo[ 0 ].hImgSize;				// Source image.
	// SDRAM for read (MC input reference).
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_addr.a_y		= (ULONG)imgRef->imgInfo[ 0 ].pImgAdrs;			// Reference image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_addr.a_c		= (ULONG)imgRef->imgInfo[ 1 ].pImgAdrs;			// Reference image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_ghsize.a_y	= imgRef->imgInfo[ 0 ].gImgSize;				// Reference image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_ghsize.a_c	= imgRef->imgInfo[ 1 ].gImgSize;				// Reference image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_hsize.a		= imgRef->imgInfo[ 0 ].wImgSize;				// Reference image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_vsize.a		= imgRef->imgInfo[ 0 ].hImgSize;				// Reference image.
	// SDRAM for read (Motion vector).
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_addr.d_y		= (ULONG)imgMe->imgInfo[ 0 ].pImgAdrs;			// Motion vector.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_addr.d_c		= (ULONG)imgMe->imgInfo[ 0 ].pImgAdrs;			// Motion vector.
	if ( ( imgSrc->imgInfo[ 0 ].wImgSize >> 2 ) <= D_IM_ME_PRE_VEC_HSIZE_1024 ) {
		gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_ghsize.d_y	= D_IM_ME_PRE_VEC_HSIZE_512;				// Motion vector.
		gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_ghsize.d_c	= D_IM_ME_PRE_VEC_HSIZE_512;				// Motion vector.
	}
	else {
		gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_ghsize.d_y	= D_IM_ME_PRE_VEC_HSIZE_1024;				// Motion vector.
		gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_ghsize.d_c	= D_IM_ME_PRE_VEC_HSIZE_1024;				// Motion vector.
	}
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_hsize.d		= imgSrc->imgInfo[ 0 ].wImgSize;				// Motion vector.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_vsize.d		= imgSrc->imgInfo[ 0 ].hImgSize;				// Motion vector.
	// SDRAM for read (Standard).
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_addr.b_y		= (ULONG)imgSrc->imgInfo[ 0 ].pImgAdrs;			// Source image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_addr.b_c		= (ULONG)imgSrc->imgInfo[ 1 ].pImgAdrs;			// Source image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_ghsize.b_y	= imgSrc->imgInfo[ 0 ].gImgSize;				// Source image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_ghsize.b_c	= imgSrc->imgInfo[ 1 ].gImgSize;				// Source image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_hsize.b		= imgSrc->imgInfo[ 0 ].wImgSize;				// Source image.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.r_vsize.b		= imgSrc->imgInfo[ 0 ].hImgSize;				// Source image.
	// SDRAM for write.
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.w_addr.e_y		= (ULONG)imgDst_Full->imgInfo[ 0 ].pImgAdrs;	// Dst image (Full).
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.w_addr.e_c		= (ULONG)imgDst_Full->imgInfo[ 1 ].pImgAdrs;	// Dst image (Full).
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.w_ghsize.e_y	= imgDst_Full->imgInfo[ 0 ].gImgSize;			// Dst image (Full).
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.w_ghsize.e_c	= imgDst_Full->imgInfo[ 1 ].gImgSize;			// Dst image (Full).

	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.w_addr.f_y		= (ULONG)imgDst_Red->imgInfo[ 0 ].pImgAdrs;		// Dst image (Reduction).
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.w_addr.f_c		= (ULONG)imgDst_Red->imgInfo[ 1 ].pImgAdrs;		// Dst image (Reduction).
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.w_ghsize.f_y	= imgDst_Red->imgInfo[ 0 ].gImgSize;			// Dst image (Reduction).
	gIp_Mfnr_Shdr_Ctrl_Prm.sdram.w_ghsize.f_c	= imgDst_Red->imgInfo[ 1 ].gImgSize;			// Dst image (Reduction).

	// Resize.
	gIp_Mfnr_Resize_Prm.chmode.a_w_en			= D_IM_SHDR_DISABLE;
	gIp_Mfnr_Resize_Prm.chmode.b_w_en			= D_IM_SHDR_DISABLE;
	gIp_Mfnr_Resize_Prm.chmode.c_w_en			= D_IM_SHDR_DISABLE;
	gIp_Mfnr_Resize_Prm.chmode.d_w_en			= D_IM_SHDR_DISABLE;

	gIp_Mfnr_Resize_Prm.woutsz.a_hsize			= imgDst_Full->imgInfo[ 0 ].wImgSize;			// Dst image (Full).
	gIp_Mfnr_Resize_Prm.woutsz.a_vsize			= imgDst_Full->imgInfo[ 0 ].hImgSize;			// Dst image (Full).

	if ( imgDst_Full->imgInfo[ 0 ].pImgAdrs == NULL ) {
		gIp_Mfnr_Resize_Prm.chmode.e_w_en		= D_IM_SHDR_DISABLE;							// Dst image (Full).
	}
	else {
		gIp_Mfnr_Resize_Prm.chmode.e_w_en		= D_IM_SHDR_ENABLE;								// Dst image (Full).
	}
	gIp_Mfnr_Resize_Prm.chmode.e_rsz_md			= D_IM_RSZMD_ODD0;								// Dst image (Full).
	gIp_Mfnr_Resize_Prm.chmode.e_rdc_en			= D_IM_SHDR_DISABLE;							// Dst image (Full).
	gIp_Mfnr_Resize_Prm.chmode.e_rdc_md			= D_IM_RDCMD_1_2;								// Dst image (Full).
	gIp_Mfnr_Resize_Prm.chmode.e_trm_en			= D_IM_SHDR_DISABLE;							// Dst image (Full).
	gIp_Mfnr_Resize_Prm.chmode.e_css_md			= D_IM_SHDR_CSSMD_YCC422;						// Dst image (Full).
	gIp_Mfnr_Resize_Prm.woutsz.e_hsize			= imgDst_Full->imgInfo[ 0 ].wImgSize;			// Dst image (Full).
	gIp_Mfnr_Resize_Prm.woutsz.e_vsize			= imgDst_Full->imgInfo[ 0 ].hImgSize;			// Dst image (Full).

	if ( imgDst_Red->imgInfo[ 0 ].pImgAdrs == NULL ) {
		gIp_Mfnr_Resize_Prm.chmode.f_w_en		= D_IM_SHDR_DISABLE;							// Dst image (Reduction).
	}
	else {
		gIp_Mfnr_Resize_Prm.chmode.f_w_en		= D_IM_SHDR_ENABLE;								// Dst image (Reduction).
	}
	gIp_Mfnr_Resize_Prm.chmode.f_rsz_md			= D_IM_RSZMD_ODD0;								// Dst image (Reduction).
	gIp_Mfnr_Resize_Prm.chmode.f_trm_en			= D_IM_SHDR_DISABLE;							// Dst image (Reduction).
	gIp_Mfnr_Resize_Prm.chmode.f_css_md			= D_IM_SHDR_CSSMD_YCC422;						// Dst image (Reduction).
	gIp_Mfnr_Resize_Prm.woutsz.f_hsize			= imgDst_Red->imgInfo[ 0 ].wImgSize;			// Dst image (Reduction).
	gIp_Mfnr_Resize_Prm.woutsz.f_vsize			= imgDst_Red->imgInfo[ 0 ].hImgSize;			// Dst image (Reduction).

	UCHAR	rdc_md;
	USHORT	pitch[ 2 ];
	(VOID)ip_mfnr_get_pixel_avg_reduction_info(	imgDst_Full->imgInfo[ 0 ].wImgSize,
												imgDst_Red->imgInfo[ 0 ].wImgSize,
												&pitch[ 0 ],
												&rdc_md );

	gIp_Mfnr_Resize_Prm.chmode.f_rdc_en			= ip_mfnr_get_pixel_avg_reduction_info(	imgDst_Full->imgInfo[ 0 ].hImgSize,
																						imgDst_Red->imgInfo[ 0 ].hImgSize,
																						&pitch[ 1 ],
																						&rdc_md );

	gIp_Mfnr_Resize_Prm.chmode.f_rdc_md			= rdc_md;

	gIp_Mfnr_Resize_Prm.rszhcnt.f_hpit			= pitch[ 0 ];									// Dst image (Reduction).
	gIp_Mfnr_Resize_Prm.rszvcnt.f_vpit			= pitch[ 1 ];									// Dst image (Reduction).

	gIp_Mfnr_Shdr_Ctrl_Prm.pCallBack = (T_IM_SHDR_CALLBACK)ip_mfnr_int_handler_shdr;

#ifndef CO_DEBUG_ON_PC
	UINT32	wait_factor_result = 0;

	ercd = Im_SHDR_Ctrl_Common( (T_IM_SHDR_CTRL*)&gIp_Mfnr_Shdr_Ctrl_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Ctrl_Common() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}

	ercd = Im_SHDR_Set_Mask( (T_IM_SHDR_MSK*)&gIp_Mfnr_Msk_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Mask() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}

	ercd = Im_SHDR_Set_Resize( (T_IM_SHDR_RESIZE*)&gIp_Mfnr_Resize_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Resize() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}

	ercd = Im_SHDR_Start();
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Start() error. ercd[0x%08x]\n", ercd );
		return D_IP_MFNR_NG;
	}

	ercd = Im_SHDR_Wait_End( &wait_factor_result, D_IP_MFNR_WAIT_TIME );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Wait_End() error. ercd[0x%08x],  wait_factor_result = 0x%x\n", ercd, wait_factor_result );
		return D_IP_MFNR_NG;
	}

#if 1	// Workaround.
	ercd = Im_SHDR_Init( FALSE );
	if( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Init error. ercd=0x%08x.\n", ercd );
		return D_IP_MFNR_NG;
	}
#endif

#else
	OS_User_Dly_Tsk( 14 );
#endif

	return D_IP_MFNR_OK;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
Open MFNR.
@return		If the function succeeds, the return value is D_IP_MFNR_OK.
*/
INT32 IP_MFNR_Open( VOID )
{
	INT32 ercd;

	OS_User_Wai_Sem( SID_IP_MFNR );

#ifndef CO_DEBUG_ON_PC
	// GMD library open.
	IPC_INT ret = IPC_GMD_Open();
	if ( ret != D_IPC_GMD_OK ) {
		FJ_Debug_Print_Err( "IP_MFNR_Open() IPC_GMD_Open error ret=0x%08x.\n", ret );
		OS_User_Sig_Sem( SID_IP_MFNR );
		return D_IP_MFNR_NG;
	}

	// ME macro driver open.
	ercd = Im_ME_Open( D_IP_MFNR_WAIT_TIME );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "IP_MFNR_Open() Im_ME_Open error. ercd=0x%08x.\n", ercd );
		OS_User_Sig_Sem( SID_IP_MFNR );
		return D_IP_MFNR_NG;
	}

	// SHDR macro driver open.
	ercd = Im_SHDR_Open( D_IP_MFNR_WAIT_TIME );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "IP_MFNR_Open() Im_SHDR_Open error. ercd=0x%08x.\n", ercd );
		OS_User_Sig_Sem( SID_IP_MFNR );
		return D_IP_MFNR_NG;
	}
#else
	OS_User_Dly_Tsk( 5 );
#endif

	gIp_Mfnr_Open = TRUE;

	OS_User_Sig_Sem( SID_IP_MFNR );

	return D_IP_MFNR_OK;
}

/**
Initialize MFNR.
@param[in]	maxCount	: Max number of input images (2~6).
@return		If the function succeeds, the return value is D_IP_MFNR_OK.
*/
INT32 IP_MFNR_Init( const UCHAR maxCount )
{
	INT32 ercd;

	if ( !gIp_Mfnr_Open ) {
		FJ_Debug_Print_Err( "IP_MFNR_Init() sequece error.\n" );
		return D_IP_MFNR_NG;
	}

	if ( ( maxCount < 2) || ( maxCount > D_IP_MFNR_MAX_COUNT ) ) {
		FJ_Debug_Print_Err( "IP_MFNR_Init() maxCount(%d) is invalid.\n", maxCount );
		return D_IP_MFNR_NG;
	}

	OS_User_Wai_Sem( SID_IP_MFNR );

	// Initialization.
	gIp_Mfnr_MaxCount	= maxCount;
	gIp_Mfnr_Count		= 0;
	memset( (void*)&gIp_Mfnr_Start_Prm,                  0, sizeof( gIp_Mfnr_Start_Prm ) );
	memset( (void*)&gIp_Mfnr_Input_Prev_Frame_Info,      0, sizeof( gIp_Mfnr_Input_Prev_Frame_Info ) );
	memset( (void*)&gIp_Mfnr_Input_Prev_Frame_Info_Red,  0, sizeof( gIp_Mfnr_Input_Prev_Frame_Info_Red ) );

#ifndef CO_DEBUG_ON_PC
	// GMD library init.
	IPC_INT ret = IPC_GMD_Init();
	if ( ret != D_IPC_GMD_OK ) {
		FJ_Debug_Print_Err( "IP_MFNR_Init() IPC_GMD_Init error ret=0x%08x.\n", ret );
		OS_User_Sig_Sem( SID_IP_MFNR );
		return D_IP_MFNR_NG;
	}

	// ME macro driver init.
	ercd = Im_ME_Init();
	if( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "IP_MFNR_Init() Im_ME_Init error. ercd=0x%08x.\n", ercd );
		OS_User_Sig_Sem( SID_IP_MFNR );
		return D_IP_MFNR_NG;
	}

	// SHDR macro driver init.
	ercd = Im_SHDR_Init( FALSE );
	if( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "IP_MFNR_Init() Im_SHDR_Init error. ercd=0x%08x.\n", ercd );
		OS_User_Sig_Sem( SID_IP_MFNR );
		return D_IP_MFNR_NG;
	}
#else
	OS_User_Dly_Tsk( 10 );
#endif

	ercd = ip_mfnr_ctrl_me();
	if ( ercd != D_IP_MFNR_OK ) {
		OS_User_Sig_Sem( SID_IP_MFNR );
		return D_IP_MFNR_NG;
	}

	ercd = ip_mfnr_ctrl_shdr();
	if ( ercd != D_IP_MFNR_OK ) {
		OS_User_Sig_Sem( SID_IP_MFNR );
		return D_IP_MFNR_NG;
	}

	OS_User_Sig_Sem( SID_IP_MFNR );

	return D_IP_MFNR_OK;
}

/**
Start MFNR.
@param[in]	startPrm	: MFNR start parameter.
@param[in]	heapAddr	: MFNR Heap address.
@param[in]	sgdeHeap	: SGDE heap.
@param[in]	shdrOut		: SHDR out.
@param[in]	count		: Processing count. (1~maxCount-1).
@return		If the function succeeds, the return value is D_IP_MFNR_OK.
*/
INT32 IP_MFNR_Start( IP_MFNR_START_PRM* startPrm, const ULONG heapAddr, FJ_STILL_IP_IMAGE* sgdeHeap, FJ_STILL_IP_IMAGE* shdrOut, const UCHAR count )
{
	IPC_FLOAT	affin[ D_IP_MFNR_AFFIN_MAX ];

	if ( !gIp_Mfnr_Open ) {
		FJ_Debug_Print_Err( "IP_MFNR_Start() sequece error.\n" );
		return D_IP_MFNR_NG;
	}

	if ( heapAddr == 0 ) {
		FJ_Debug_Print_Err( "IP_MFNR_Start() heapAddr is null.\n" );
		return D_IP_MFNR_NG;
	}

	if ( sgdeHeap == NULL ) {
		FJ_Debug_Print_Err( "IP_MFNR_Start() sgdeHeap is invalid.\n");
		return D_IP_MFNR_NG;
	}

	if ( shdrOut == NULL ) {
		FJ_Debug_Print_Err( "IP_MFNR_Start() shdrOut is invalid.\n");
		return D_IP_MFNR_NG;
	}

	if ( count >= gIp_Mfnr_MaxCount ) {
		FJ_Debug_Print_Err( "IP_MFNR_Start() count(%d/%d) is invalid.\n", count, gIp_Mfnr_MaxCount );
		return D_IP_MFNR_NG;
	}

	OS_User_Wai_Sem( SID_IP_MFNR );

	gIp_Mfnr_Start_Prm	= *startPrm;
	gIp_Mfnr_Count		= count;

	if ( count != 0 ) {

		INT32 ercd = ip_mfnr_ctrl_gmd( startPrm, heapAddr, D_IP_MFNR_HEAP_SIZE );
		if ( ercd != D_IP_MFNR_OK ) {
			OS_User_Sig_Sem( SID_IP_MFNR );
			return D_IP_MFNR_NG;
		}

		ercd = ip_mfnr_start_gmd( startPrm, &affin[ 0 ] );
		if ( ercd != D_IP_MFNR_OK ) {
			OS_User_Sig_Sem( SID_IP_MFNR );
			return D_IP_MFNR_NG;
		}

		IP_MFNR_CROP_CORNER_POS	target_corner_pos;
		IP_MFNR_CROP_IMG_SIZE	target_rect;

		target_rect.width = startPrm->imgSrc_2nd[ 0 ].imgInfo[ 0 ].wImgSize;
		target_rect.lines = startPrm->imgSrc_2nd[ 0 ].imgInfo[ 0 ].hImgSize;
		IP_MFNR_Calc_CropCornerPos( &target_corner_pos, &gIp_Mfnr_Base_Corner_Pos, (FLOAT*)&affin[ 0 ], &target_rect );

		FJ_Debug_Print_Info( "Base_Corner_Pos l_top_x=%.2f y=%.2f, r_top_x=%.2f y=%.2f, l_btm_x=%.2f y=%.2f, r_btm_x=%.2f y=%.2f\n",
							gIp_Mfnr_Base_Corner_Pos.left_top.pos_x,		gIp_Mfnr_Base_Corner_Pos.left_top.pos_y,
							gIp_Mfnr_Base_Corner_Pos.right_top.pos_x,		gIp_Mfnr_Base_Corner_Pos.right_top.pos_y,
							gIp_Mfnr_Base_Corner_Pos.left_bottom.pos_x,		gIp_Mfnr_Base_Corner_Pos.left_bottom.pos_y,
							gIp_Mfnr_Base_Corner_Pos.right_bottom.pos_x,	gIp_Mfnr_Base_Corner_Pos.right_bottom.pos_y );

		FJ_Debug_Print_Info( "Target_Corner_Pos l_top_x=%.2f y=%.2f, r_top_x=%.2f y=%.2f, l_btm_x=%.2f y=%.2f, r_btm_x=%.2f y=%.2f\n",
							target_corner_pos.left_top.pos_x,		target_corner_pos.left_top.pos_y,
							target_corner_pos.right_top.pos_x,		target_corner_pos.right_top.pos_y,
							target_corner_pos.left_bottom.pos_x,	target_corner_pos.left_bottom.pos_y,
							target_corner_pos.right_bottom.pos_x,	target_corner_pos.right_bottom.pos_y );

		ercd = ip_mfnr_start_sgde( startPrm, sgdeHeap, &affin[ 0 ] );
		if ( ercd != D_IP_MFNR_OK ) {
			OS_User_Sig_Sem( SID_IP_MFNR );
			return D_IP_MFNR_NG;
		}

		ercd = ip_mfnr_start_me(	&startPrm->imgSrc_1st[ 2 ],		// [In]Source image 1st (Y-only 8bit 1/4).
									&sgdeHeap[ 1 ],					// [In]Source image 2nd (Y-only 8bit 1/4).
									&startPrm->imgMe );				// [Out]Motion vector.
		if ( ercd != D_IP_MFNR_OK ) {
			OS_User_Sig_Sem( SID_IP_MFNR );
			return D_IP_MFNR_NG;
		}

		ercd = ip_mfnr_start_shdr(	count,							// [In]count.
									startPrm->filMargin,			// [In]filter margin.
									&startPrm->imgSrc_1st[ 0 ],		// [In]Source image 1st (YUV422 8bit Full).
									&sgdeHeap[ 0 ],					// [In]Source image 2nd (YUV422 8bit Full).
									&startPrm->imgMe,				// [In]Motion vector.
									&shdrOut[ 0 ],					// [Out]Destination image (YUV422 8bit Full).
									&shdrOut[ 1 ] );				// [Out]Destination image (YUV422 8bit Reduction).
		if ( ercd != D_IP_MFNR_OK ) {
			OS_User_Sig_Sem( SID_IP_MFNR );
			return D_IP_MFNR_NG;
		}

		if ( count == ( gIp_Mfnr_MaxCount - 1 ) ) {
			FLOAT					crop_xform[ 6 ];
			IP_MFNR_CROP_IMG_SIZE	dst_rect;

			dst_rect.width = startPrm->imgDst.imgInfo[ 0 ].wImgSize;
			dst_rect.lines = startPrm->imgDst.imgInfo[ 0 ].hImgSize;
			IP_MFNR_Calc_AspectCropPos( crop_xform, &dst_rect, &target_corner_pos );
#ifdef CO_IP_MFNR_CROP_POS_DUMP
			ip_mfnr_dump_crop_corner_pos( "MFNR_CROP_DUMP_LAST", &target_corner_pos );
#endif
			ercd = ip_mfnr_start_sgde_edgecut( crop_xform, &shdrOut[ 0 ], &startPrm->imgDst );
			if ( ercd != D_IP_MFNR_OK ) {
				OS_User_Sig_Sem( SID_IP_MFNR );
				return D_IP_MFNR_NG;
			}
		}

		// Previous image saved.
		gIp_Mfnr_Input_Prev_Frame_Info		= shdrOut[ 0 ];
		gIp_Mfnr_Input_Prev_Frame_Info_Red	= shdrOut[ 1 ];

		gIp_Mfnr_Base_Corner_Pos = target_corner_pos;
	}
	else {
		// Previous image saved.
		gIp_Mfnr_Input_Prev_Frame_Info		= startPrm->imgSrc_1st[ 0 ];
		gIp_Mfnr_Input_Prev_Frame_Info_Red	= startPrm->imgSrc_1st[ 1 ];

		IP_MFNR_CROP_IMG_SIZE	base_rect;
		base_rect.width = startPrm->imgSrc_1st[ 0 ].imgInfo[ 0 ].wImgSize;
		base_rect.lines = startPrm->imgSrc_1st[ 0 ].imgInfo[ 0 ].hImgSize;
		IP_MFNR_Conv_Rect2CornerPos( &gIp_Mfnr_Base_Corner_Pos, &base_rect );
#ifdef CO_IP_MFNR_CROP_POS_DUMP
		ip_mfnr_dump_crop_corner_pos( "MFNR_CROP_DUMP", &gIp_Mfnr_Base_Corner_Pos );
#endif

		FJ_Debug_Print_Info( "Base_Corner_Pos l_top_x=%.2f y=%.2f, r_top_x=%.2f y=%.2f, l_btm_x=%.2f y=%.2f, r_btm_x=%.2f y=%.2f\n",
							gIp_Mfnr_Base_Corner_Pos.left_top.pos_x,		gIp_Mfnr_Base_Corner_Pos.left_top.pos_y,
							gIp_Mfnr_Base_Corner_Pos.right_top.pos_x,		gIp_Mfnr_Base_Corner_Pos.right_top.pos_y,
							gIp_Mfnr_Base_Corner_Pos.left_bottom.pos_x,		gIp_Mfnr_Base_Corner_Pos.left_bottom.pos_y,
							gIp_Mfnr_Base_Corner_Pos.right_bottom.pos_x,	gIp_Mfnr_Base_Corner_Pos.right_bottom.pos_y );
	}

	OS_User_Sig_Sem( SID_IP_MFNR );

	return D_IP_MFNR_OK;
}

/**
Close MFNR.
@return		If the function succeeds, the return value is D_IP_MFNR_OK.
*/
INT32 IP_MFNR_Close( VOID )
{
	if ( !gIp_Mfnr_Open ) {
		return D_IP_MFNR_OK;
	}

#ifndef CO_DEBUG_ON_PC
	// GMD library close.
	IPC_INT ret = IPC_GMD_Close();
	if ( ret != D_IPC_GMD_OK ) {
		FJ_Debug_Print_Err( "IP_MFNR_Close() IPC_GMD_Close error ret=0x%08x.\n", ret );
		return D_IP_MFNR_NG;
	}

	// ME macro driver close.
	INT32 ercd = Im_ME_Close();
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "IP_MFNR_Close() Im_ME_Close error ercd=0x%08x.\n", ercd );
		return D_IP_MFNR_NG;
	}

	// SHDR macro driver close.
	ercd = Im_SHDR_Close();
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "IP_MFNR_Close() Im_SHDR_Close error ercd=0x%08x.\n", ercd );
		return D_IP_MFNR_NG;
	}
#else
	OS_User_Dly_Tsk( 10 );
#endif

	gIp_Mfnr_Open = FALSE;

	return D_IP_MFNR_OK;
}

/**
Get MFNR version.
@param[out]		str	: version
*/
VOID IP_MFNR_Get_Version( CHAR* str )
{
	const CHAR ver[ 3 ] = { '0', '.', '1' };

	strcpy( (char*)str, (const char*)ver );

	return;
}

/**
Get MFNR input previous frame information.
@param[out]	prev_info		: previous image infomation.
@param[out]	prev_info_red	: previous image infomation (reduction).
*/
VOID IP_MFNR_Get_Input_Prev_Frame_Info( FJ_STILL_IP_IMAGE* prev_info, FJ_STILL_IP_IMAGE* prev_info_red )
{
	*prev_info		= gIp_Mfnr_Input_Prev_Frame_Info;
	*prev_info_red	= gIp_Mfnr_Input_Prev_Frame_Info_Red;
}

/**
Get MFNR count.
@return		Processing count. (1~maxCount-1).
*/
INT32 IP_MFNR_Get_Count()
{
	return gIp_Mfnr_Count;
}

#if defined(CO_IQBIN_GENERAL)	// --- REMOVE_IQBIN_GENERAL BEGIN ---
VOID IP_MFNR_Get_RegTbl( T_IM_SHDR_CTRL** shdr_ctrl, T_IM_SHDR_PRE** shdr_pre, T_IM_SHDR_FMD** shdr_fmd, T_IM_SHDR_SPNR** shdr_spnr, T_IM_SHDR_MSK** shdr_msk, T_IM_SHDR_PMSK** shdr_pmsk, UINT32** shdr_in )
{
	*shdr_ctrl = (T_IM_SHDR_CTRL*)&gIp_Mfnr_Shdr_Ctrl_Prm;
	*shdr_pre  = (T_IM_SHDR_PRE*)&gIp_Mfnr_Pre_Prm;
	*shdr_fmd  = (T_IM_SHDR_FMD*)&gIp_Mfnr_Fmd_Prm;
	*shdr_spnr = (T_IM_SHDR_SPNR*)&gIp_Mfnr_Spnr_Prm;
	*shdr_msk  = (T_IM_SHDR_MSK*)&gIp_Mfnr_Msk_Prm;
	*shdr_pmsk = (T_IM_SHDR_PMSK*)&gIp_Mfnr_Pmsk_Prm;
	*shdr_in   = (UINT32*)&gIp_Mfnr_ShdrIn_Prm;
}
#endif // CO_IQBIN_xxxx  --- REMOVE_IQBIN_GENERAL END ---

VOID IP_MFNR_Conv_Rect2CornerPos( IP_MFNR_CROP_CORNER_POS* const corner_pos, const IP_MFNR_CROP_IMG_SIZE* const rect )
{
	corner_pos->left_top.pos_x = 0.0f;
	corner_pos->left_top.pos_y = 0.0f;

	corner_pos->right_top.pos_x = (FLOAT)(rect->width -1);
	corner_pos->right_top.pos_y = 0.0f;

	corner_pos->left_bottom.pos_x = 0.0f;
	corner_pos->left_bottom.pos_y = (FLOAT)(rect->lines -1);

	corner_pos->right_bottom.pos_x = (FLOAT)(rect->width -1);
	corner_pos->right_bottom.pos_y = (FLOAT)(rect->lines -1);
}

VOID IP_MFNR_Calc_CropCornerPos( IP_MFNR_CROP_CORNER_POS* const crop_corner_pos, const IP_MFNR_CROP_CORNER_POS* const base_corner_pos, const FLOAT target_rotate_xform[6], const IP_MFNR_CROP_IMG_SIZE* const target_rect )
{
	IP_MFNR_CROP_CORNER_POS	target_corner_pos;

	ip_mfnr_crop_conv_afn2corner_pos( &target_corner_pos, target_rotate_xform, target_rect );
#ifdef CO_IP_MFNR_CROP_POS_DUMP
		ip_mfnr_dump_crop_corner_pos( "MFNR_CROP_DUMP", &target_corner_pos );
#endif
	crop_corner_pos->left_top.pos_x = crop_corner_pos->left_bottom.pos_x
								= ip_mfnr_crop_calc_left_x( base_corner_pos, &target_corner_pos );
	crop_corner_pos->right_top.pos_x = crop_corner_pos->right_bottom.pos_x
								= ip_mfnr_crop_calc_right_x( base_corner_pos, &target_corner_pos );
	crop_corner_pos->left_top.pos_y = crop_corner_pos->right_top.pos_y
								= ip_mfnr_crop_calc_top_y( base_corner_pos, &target_corner_pos );
	crop_corner_pos->left_bottom.pos_y = crop_corner_pos->right_bottom.pos_y
								= ip_mfnr_crop_calc_bottom_y( base_corner_pos, &target_corner_pos );

#ifdef CO_IP_MFNR_CROP_DBG_PRINT
	ip_mfnr_print_crop_corner_pos( "base_corner_pos  ", base_corner_pos );
	ip_mfnr_print_xform          ( "target_xform     ", target_rotate_xform );
	ip_mfnr_print_crop_corner_pos( "target_corner_pos", &target_corner_pos );
	ip_mfnr_print_crop_corner_pos( "out_corner_pos   ", crop_corner_pos );
#endif
}

VOID IP_MFNR_Calc_AspectCropPos( FLOAT crop_xform[6], const IP_MFNR_CROP_IMG_SIZE* const dst_rect, const IP_MFNR_CROP_CORNER_POS* const crop_corner_pos )
{
	FLOAT	crop_width;
	FLOAT	crop_lines;
	FLOAT	crop_aspect_width;
	FLOAT	crop_aspect_lines;
	FLOAT	crop_aspect_offset_x;
	FLOAT	crop_aspect_offset_y;

	// Normalize cropping window's aspect ratio
	// and Cauculate cropping window offset.
	crop_width = crop_corner_pos->right_top.pos_x   - crop_corner_pos->left_top.pos_x;
	crop_lines = crop_corner_pos->left_bottom.pos_y - crop_corner_pos->left_top.pos_y;

	crop_aspect_lines = crop_width * dst_rect->lines / dst_rect->width;
	if( crop_aspect_lines < crop_lines ) {
		// Crop area is tall.
		// Cut top and bottom area. (Normalize aspect ratio.)
		crop_aspect_width = crop_width;
		crop_aspect_offset_x = 0.0f;
		crop_aspect_offset_y = (crop_lines - crop_aspect_lines) / 2.0f;
	}
	else {
		// Crop area is fat.
		// Cut left and right area. (Normalize aspect ratio.)
		crop_aspect_width = crop_lines * dst_rect->width / dst_rect->lines;
		crop_aspect_lines = crop_lines;
		crop_aspect_offset_x = (crop_width - crop_aspect_width) / 2.0f;
		crop_aspect_offset_y = 0.0f;
	}


	// Calculate xform.
	crop_xform[D_IP_MFNR_CROP_XFORM_IDX_STA_X] = crop_corner_pos->left_top.pos_x + crop_aspect_offset_x;
	crop_xform[D_IP_MFNR_CROP_XFORM_IDX_STA_Y] = crop_corner_pos->left_top.pos_y + crop_aspect_offset_y;
	crop_xform[D_IP_MFNR_CROP_XFORM_IDX_PIT_XX] = crop_aspect_width / (dst_rect->width -1);
	crop_xform[D_IP_MFNR_CROP_XFORM_IDX_PIT_XY] = 0.0f;
	crop_xform[D_IP_MFNR_CROP_XFORM_IDX_PIT_YX] = 0.0f;
	crop_xform[D_IP_MFNR_CROP_XFORM_IDX_PIT_YY] = crop_aspect_lines / (dst_rect->lines -1);
}
