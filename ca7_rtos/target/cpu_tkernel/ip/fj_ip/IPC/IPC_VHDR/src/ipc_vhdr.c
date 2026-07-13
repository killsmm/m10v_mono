/**
 * @file		ipc_vhdr.c
 * @brief		API for VHDR
 * @author		kajiki
 * @since		2017/03/16
 * @date		
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2017</I></B>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ipc_vhdr.h"
#include "fj_still.h"
#include "dd_arm.h"
#include "ddim_typedef.h"
#include "driver_common.h"
#include "os_user_custom.h"
#include "im_me.h"
#include "im_shdr.h"
#include "ipc_hdrprm.h"

#include "fj_debug.h"
#include "mba_if.h"

/*
#include "../include/ipc_vhdr_common.h"
#include "../include/ipc_vhdr_core.h"
#include "../include/ipc_vhdr_debug.h"
#include "../include/ipc_vhdr_bin_data.h"
#include "../include/ipc_vhdr_core_hw.h"
*/
#include "ipc_if_user_custom.h"

//#include "ipc_mcsd.h"

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/
#define D_IPC_VHDR_OPENBIT                     (0x00000001)
#define D_IPC_VHDR_INITBIT                     (0x00000002)
#define D_IPC_VHDR_CTRLBIT                     (0x00000004)
#define D_IPC_VHDR_GAIN_MAX                    (102300)

/**< MIFチェック時 Start_FMD/Start_HDR判定フラグ  */
#define D_IPC_VHDR_WAIT_TIME                   (3000)

#define D_IPC_VHDR_FMD_MIF_FLG                 (0)
#define D_IPC_VHDR_HDR_MIF_FLG                 (1)

#define D_IPC_VHDR_BASE_EXPO                   (16)
#define D_IPC_VHDR_BASE_TM_LV                  (0)
#define D_IPC_VHDR_BASE_TM_SMOOTH              (0)
#define D_IPC_VHDR_BASE_TM_BLACK               (0)
#define D_IPC_VHDR_BASE_TM_OFFSET              (0)
#define D_IPC_VHDR_BASE_TM_GRAD                (256)
#define D_IPC_VHDR_BASE_TM_INTRCPT             (128)

#define D_IPC_VHDR_TM_LV_MAX                   (7)
#define D_IPC_VHDR_BLD_TBL_INDEX_MAX           (7)
#define D_IPC_VHDR_BLD_TBL_NUM                 (8)
#define D_IPC_VHDR_NORMAL_COMP                 (0) // 通常合成
#define D_IPC_VHDR_LONG_COMP                   (1) // 合成無効(長露光のみ出力)
#define D_IPC_VHDR_SHORT_COMP                  (2) // 合成無効(短露光のみ出力)
#define D_IPC_VHDR_BOOST_MAX                   (4096)
#define D_IPC_VHDR_BOOST_MIN                   (1)

#define D_IPC_VHDR_LUT_FMD_GAIN_INDEX          (0) // Mask部明るさ合わせ
#define D_IPC_VHDR_LUT_FMD_BLEND_INDEX         (1) // Mask部Blendテーブル
#define D_IPC_VHDR_LUT_FMD_ABS_INDEX           (2) // Mask部絶対値化
#define D_IPC_VHDR_LUT_MRG_EXPAND_INDEX        (3) // Merge部明るさ合わせ
#define D_IPC_VHDR_LUT_MRG_GAIN_INDEX          (4) // Merge部ゲインテーブル
#define D_IPC_VHDR_LUT_NO_MRG_GAIN_INDEX       (5) // 長短露光出力時(非合成時)Merge部ゲインテーブル
#define D_IPC_VHDR_LUT_MCSD_GAIN_INDEX         (6) // MCSD部明るさ合わせ

#define D_IPC_VHDR_YUV_RGB_CSC_COEF_QBIT       (6) // YUV2RGB変換係数ビット

#define D_IPC_VHDR_Gam_NUM0					   (0x00000001)
#define D_IPC_VHDR_Gam_NUM1					   (0x00000002)
#define D_IPC_VHDR_Gam_NUM2					   (0x00000004)
#define D_IPC_VHDR_Gam_NUM3					   (0x00000008)
#define D_IPC_VHDR_Gam_NUM4					   (0x00000010)
#define D_IPC_VHDR_Gam_NUM5					   (0x00000020)
#define D_IPC_VHDR_Gam_NUM6					   (0x00000040)
#define D_IPC_VHDR_Gam_NUM7					   (0x00000080)

#define D_IPC_VHDR_SAT_LV_INDEX_MAX            (7)
#define D_IPC_VHDR_SAT_LV_NUM                  (8)

#define D_IPC_VHDR_TM_STLENGTH_INDEX_MAX       (7)

#define D_IPC_VHDR_TM_BLACK_MAX                (255)

#define D_IPC_VHDR_BLD_RATIO_DISABLE           (0)

#define D_IPC_VHDR_SHIFT_PERMISSION_CLP        (3) // 手振れ保証クリップ範囲

//#define D_DEBUG_VHDR_OUT_LONG			// output only long frame
//#define D_DEBUG_VHDR_OUT_SHORT		// output only short frame

#define D_DEBUG_SET_RECOMMENDED			// set recommended
#define D_IPC_VHDR_BLD_LPF_TAP				   (4)

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/
/** ライブラリ管理情報 */
/*typedef struct {

	//	共通パラメータ
	T_IPC_IMAGE					half_dst_img;			// ブレンドマスク(縦横1/2サイズ)
	T_IPC_IMAGE					srcImg[2];				// [0]:長露光入力画像(フルサイズ)
														// [1]:短露光入力画像(フルサイズ)
	T_IPC_IMAGE					half_short_srcImg;		// 短露光入力画像(1/2サイズ 単面)
	T_IPC_IMAGE					dstImg;					// 出力画像情報
	T_IPC_VHDR_INIT_PRM			init_prm;				// 初期化パラメータ情報
//	T_IPC_VHDR_BIN_MNG*			bin_load_info;			// バイナリロード情報
	T_IPC_VHDR_CTRL_PRM			ctrl_prm;				// 静的制御パラメータ情報
	T_IPC_RECT					rect_prm;				// 塗りつぶしパラメータ情報

	IPC_ULONG					fmdHeapAddr;			// 被写体ブレ検出・マスク部ヒープアドレス
	IPC_UINT					fmdHeapSize;			// 被写体ブレ検出・マスク部ヒープサイズ
	T_IPC_VHDR_FMD_PRM			fmd_prm;				// 被写体ブレ検出パラメータ情報
	T_IPC_VHDR_EXP_PRM			fmd_exp_prm;			// 被写体ブレ検出露光条件パラメータ情報
	T_IPC_VHDR_EXP_PRM			hdr_exp_prm;			// 合成処理露光条件パラメータ情報
	T_IPC_VHDR_EXP_PRM			mcsd_exp_prm;			// 位置合わせ露光条件パラメータ情報
	IPC_ULONG					MergeExMaskHeapAddr;	// Merge部(外部Mask)ヒープアドレス
	IPC_UINT					MergeExMaskHeapSize;	// Merge部(外部Mask)ヒープサイズ
	IPC_ULONG					MergeInMaskHeapAddr;	// Merge部(内部Mask)ヒープアドレス
	IPC_UINT					MergeInMaskHeapSize;	// Merge部(内部Mask)ヒープサイズ
//	T_IPC_IIP_LUTRAM			lutInfo[7];				// LUTRAM用バッファ先頭アドレス
														// [0]:Mask部明るさ合わせ
														// [1]:Mask部Blendテーブル
														// [2]:Mask部絶対値化
														// [3]:Merge部明るさ合わせ
														// [4]:Merge部ゲインテーブル
														// [5]:長短露光出力時(非合成時)Merge部ゲインテーブル
														// [6]:MCSD分明るさ合わせ
	T_IPC_VHDR_DYNAMIC_PRM		dynamic_prm;			// 動作制御動的パラメータ情報
	T_IPC_VHDR_MCSD_PRM			mcsd_prm;				// フレーム間位置ずれ検出パラメータ情報
	IPC_ULONG					McsdHeapAddr;			// MCSD用ヒープアドレス
	IPC_UINT					McsdHeapSize;			// MCSD用ヒープサイズ
	IPC_ULONG					gam_tbl_addr;			// ガンマテーブルアドレス
	IPC_ULONG					exp_gam_tbl_addr;		// 拡張ガンマテーブルアドレス
	IPC_SHORT*					sat_lv_ext_csc;			// 暗部彩度制御(外部Mask)用CSC係数先頭アドレス
	IPC_SHORT*					sat_lv_int_csc;			// 暗部彩度制御(内部Mask)用CSC係数先頭アドレス
	IPC_SHORT*					light_sat_lv_ext_csc;	// 明部彩度制御(外部Mask)用CSC係数先頭アドレス
	IPC_SHORT*					light_sat_lv_int_csc;	// 明部彩度制御(内部Mask)用CSC係数先頭アドレス
} T_IPC_VHDR_MNG;
*/

/*--------------------------------------------------------------------------------------*/
/* Global Data																			*/
/*--------------------------------------------------------------------------------------*/
static volatile IPC_INT 		gIpcVhdr_isOpen = 0;			/**< Open/Init/Ctrl/Close 情報  */
//static volatile T_IM_SHDR_CTRL	gIpcVhdr_Shdr_Ctrl_Prm;					/**< 管理情報                   */
static volatile IPC_INT 		gIpcVhdr_StartFmdFlg = 0;		/**< StartFmd実施フラグ         */
static volatile IPC_INT 		gIpcVhdr_StartHdrFlg = 0;		/**< StartHdr実施フラグ         */
static volatile IPC_UINT		gIpcVhdr_GamNumFlg = 0;			/**< ガンマ展開実施フラグ       */
volatile IPC_UINT gIpcVhdrMergeHdrEn;							/**< HDR合成指示情報            */
//volatile T_IPC_VHDR_SHIFT gIpcVhdrShift;						/**< フレーム間位置ずれ検出情報 */
//volatile T_IPC_VHDR_SHIFT gIpcVhdrHalfShift;					/**< フレーム間位置ずれ検出(縦横1/2サイズ)情報 */
//volatile T_IPC_VHDR_SHIFT gIpcVhdrMargin;						/**< HDR合成用入出力マージン情報 */
static volatile FJ_STILL_IP_IMAGE		gIpcVhdr_Input_Prev_Frame_Info;
static volatile FJ_STILL_IP_IMAGE		gIpcVhdr_Input_Prev_Frame_Info_Sub[ 2 ];
static volatile FJ_STILL_IP_IMAGE		gIpcVhdr_Output_Prev_Comp_Info;

static	IPC_ULONG	gIpcVhdr_HeapAdd = 0;
static	IPC_UINT	gIpcVhdr_HeapSize = 0;
//static	IPC_ULONG	gIpcVhdr_HeapTable = 0;
//static	IPC_ULONG	gIpcVhdr_HeapMacro = 0;

static	T_IPC_VHDR_BLD_TBL	gIpcVhdr_BlendTable;

static	IPC_UINT	gIpcVhdr_width, gIpcVhdr_height;
static	IPC_INT		gIpcVhdr_gamma_tbl_type;
static	IPC_USHORT*	gIpcVhdr_gamma_tbl;


static volatile T_IM_SHDR_CTRL	gIpcVhdr_Shdr_Ctrl_Prm = {
	.image_set.in_bit_depth		= D_IM_SHDR_INBITDEPTH_8,
	.image_set.in_c_fmt			= D_IM_SHDR_INCFMT_420SP,
	.image_set.in_edge_cpy		= D_IM_SHDR_ON,
	.image_set.in_edge_cut		= D_IM_SHDR_ON,
//	.image_set.out_ctrl			= D_IM_SHDR_OUTCTRL_YCC,
	.image_set.out_ctrl			= D_IM_SHDR_OUTCTRL_RGB,
	.image_set.out_sel			= D_IM_SHDR_OUTSEL_YCC_16,
	.image_set.out_sel_en		= D_IM_SHDR_DISABLE,
	.image_mode.exp_mag			= 0x0100,
	.image_mode.fil_margin		= D_IM_SHDR_FLITER_MARGIN_16,
	.image_mode.rexp_mag_shift	= 0,
	.image_mode.rexp_mag		= 0x10000,
	.image_mode.shift_x			= 0,
	.image_mode.shift_y			= 0,
	.image_mode.fmd_en			= D_IM_SHDR_DISABLE,					// IQ.bin
	.image_mode.bypass_en		= D_IM_SHDR_DISABLE,
//	.image_mode.smc_in			= D_IM_SHDR_SMCIN_EXT_STN,
	.image_mode.smc_in			= D_IM_SHDR_SMCIN_STN_REF,
//	.image_mode.shdr_in			= D_IM_SHDR_SHDRIN_STN_SMCOUT,
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

static volatile T_IM_SHDR_POST	gIpcVhdr_Shdr_Post_Prm = {
	.sft1rnd.sft1rnd_en			= D_IM_SHDR_ENABLE,
//	.sft1rnd.sft1rnd_en			= D_IM_SHDR_DISABLE,
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
	.hdrcss_mode				= D_IM_SHDR_HDRCSSMD_YCC422_CENTER,
};

static volatile T_IM_SHDR_PMSK	gIpcVhdr_Shdr_Pmsk_Prm = {
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

static volatile T_IM_SHDR_RESIZE	gIpcVhdr_Shdr_Resize_Prm = {
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

static volatile T_IM_SHDR_PRE	gIpcVhdr_Shdr_Pre_Prm = {
//	.pre_mode.cus_mode			= D_IM_SHDR_CUS_YCC422_BILINEAR_B,
	.pre_mode.cus_mode			= D_IM_SHDR_CUS_YCC420_BILINEAR_B,
	.pre_mode.degamma_mode		= D_IM_SHDR_DEGAMMA_BITEXP,
	.y2r_coef[ 0 ] 				= 0x0100,								// IQ.bin
	.y2r_coef[ 1 ] 				= 0x0001,								// IQ.bin
	.y2r_coef[ 2 ] 				= 0x0166,								// IQ.bin
	.y2r_coef[ 3 ] 				= 0x0100,								// IQ.bin
	.y2r_coef[ 4 ] 				= 0x7FA8,								// IQ.bin
	.y2r_coef[ 5 ] 				= 0x7F48,								// IQ.bin
	.y2r_coef[ 6 ] 				= 0x0100,								// IQ.bin
	.y2r_coef[ 7 ] 				= 0x01C6,								// IQ.bin
	.y2r_coef[ 8 ] 				= 0x7FFE,								// IQ.bin
	.y2r_clp.upper_long0[ 0 ]	= 0x00FF,								// IQ.bin
	.y2r_clp.upper_long0[ 1 ]	= 0x00FF,								// IQ.bin
	.y2r_clp.upper_long0[ 2 ]	= 0x00FF,								// IQ.bin
	.y2r_clp.lower_long0[ 0 ]	= 0x0000,								// IQ.bin
	.y2r_clp.lower_long0[ 1 ]	= 0x0000,								// IQ.bin
	.y2r_clp.lower_long0[ 2 ]	= 0x0000,								// IQ.bin
	.y2r_clp.upper_short[ 0 ]	= 0xFF,									// IQ.bin
	.y2r_clp.upper_short[ 1 ]	= 0xFF,									// IQ.bin
	.y2r_clp.upper_short[ 2 ]	= 0xFF,									// IQ.bin
	.y2r_clp.lower_short[ 0 ]	= 0x00,									// IQ.bin
	.y2r_clp.lower_short[ 1 ]	= 0x00,									// IQ.bin
	.y2r_clp.lower_short[ 2 ]	= 0x00,									// IQ.bin
	.r2y_coef[ 0 ] 				= 0x004D,								// IQ.bin
	.r2y_coef[ 1 ] 				= 0x0096,								// IQ.bin
	.r2y_coef[ 2 ] 				= 0x001D,								// IQ.bin
	.r2y_coef[ 3 ] 				= 0x7FD5,								// IQ.bin
	.r2y_coef[ 4 ] 				= 0x7FAB,								// IQ.bin
	.r2y_coef[ 5 ] 				= 0x0080,								// IQ.bin
	.r2y_coef[ 6 ] 				= 0x0080,								// IQ.bin
	.r2y_coef[ 7 ] 				= 0x7F95,								// IQ.bin
	.r2y_coef[ 8 ] 				= 0x7FEB,								// IQ.bin
	.r2y_clp.upper_long_y		= 0x0FFF,								// IQ.bin
	.r2y_clp.upper_long_c[ 0 ]	= 0x07FF,								// IQ.bin
	.r2y_clp.upper_long_c[ 1 ]	= 0x07FF,								// IQ.bin
	.r2y_clp.lower_long_y		= 0x0000,								// IQ.bin
	.r2y_clp.lower_long_c[ 0 ]	= 0xF800,								// IQ.bin
	.r2y_clp.lower_long_c[ 1 ]	= 0xF800,								// IQ.bin
	.r2y_clp.upper_short_y		= 0xFFF,								// IQ.bin
	.r2y_clp.upper_short_c[ 0 ]	= 0x7FF,								// IQ.bin
	.r2y_clp.upper_short_c[ 1 ]	= 0x7FF,								// IQ.bin
	.r2y_clp.lower_short_y		= 0x000,								// IQ.bin
	.r2y_clp.lower_short_c[ 0 ]	= 0x800,								// IQ.bin
	.r2y_clp.lower_short_c[ 1 ]	= 0x800,								// IQ.bin
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

static volatile T_IM_SHDR_MSK	gIpcVhdr_Shdr_Msk_Prm = {
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
#ifdef D_DEBUG_SET_RECOMMENDED
	.alpha						= 0x3F,
#else
	.alpha						= 0x20,
#endif
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
#ifdef D_DEBUG_SET_RECOMMENDED
	.norm_gain[ 0 ]				= 0x0,									// IQ.bin
	.norm_gain[ 1 ]				= 0x0,									// IQ.bin
	.norm_gain[ 2 ]				= 0x0,									// IQ.bin
	.norm_offset[ 0 ]			= 0x0,									// IQ.bin
	.norm_offset[ 1 ]			= 0x0,									// IQ.bin
	.norm_offset[ 2 ]			= 0x0,									// IQ.bin
	.msk_mode					= D_IM_SHDR_MSKMD_LONG,
#else
	.norm_gain[ 0 ]				= 0x80,									// IQ.bin
	.norm_gain[ 1 ]				= 0x80,									// IQ.bin
	.norm_gain[ 2 ]				= 0x80,									// IQ.bin
	.norm_offset[ 0 ]			= 0x80,									// IQ.bin
	.norm_offset[ 1 ]			= 0x80,									// IQ.bin
	.norm_offset[ 2 ]			= 0x80,									// IQ.bin
	.msk_mode					= D_IM_SHDR_MSKMD_MSKONLY,

#endif
};

static volatile T_IM_SHDR_BLD	gIpcVhdr_Shdr_Bld_Prm = {
	.blend_sel					= D_IM_SHDR_BLDSEL_SHORT_LONG,
#if 1
	.alpha_sel					= D_IM_SHDR_ALPHASEL_DEVIATION,
	.alpha_val[ 0 ]				= 0xFF,
	.alpha_val[ 1 ]				= 0xFF,
	.alpha_val[ 2 ]				= 0xFF,
#else
	.alpha_sel					= D_IM_SHDR_ALPHASEL_FIXED,
	.alpha_val[ 0 ]				= 0x00,
	.alpha_val[ 1 ]				= 0x00,
	.alpha_val[ 2 ]				= 0x00,
#endif
};

static volatile T_IM_SHDR_SMC	gIpcVhdr_Smc_Prm = {
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

static volatile T_IM_SHDR_AXI	gIpcVhdr_Axi_Prm = {
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

static volatile USHORT	gIpcVhdr_Degamma_Prm[ 3 ][ D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ] = {
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

static volatile UINT32 gIpcVhdr_ShdrIn_Prm = D_IM_SHDR_SHDRIN_STN_SMCOUT;
//static volatile UINT32 *gpIpcVhdr_ShdrIn_Prm = &gIpcVhdr_ShdrIn_Prm;

T_IM_SHDR_CALLBACK gIpcVhdr_Shdr_Func_CB = NULL;

/*--------------------------------------------------------------------------------------*/
/* Macro																				*/
/*--------------------------------------------------------------------------------------*/
#define ipcVhdr_calc_pit( src, dst )	((UINT32)((UINT64)0x4000 * ((src) -1) / ((dst) -1)))
/*--------------------------------------------------------------------------------------*/
/* Local Method Definition																*/
/*--------------------------------------------------------------------------------------*/
#ifdef D_DEBUG_SET_RECOMMENDED
void lpf_blendtable( T_IPC_VHDR_BLD_TBL* bld_tbl );
#endif
/*--------------------------------------------------------------------------------------*/
/* Table																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Local Functions																		*/
/*--------------------------------------------------------------------------------------*/
/**
 * @brief	VHDR割り込みハンドラ
 *
 * @param[in]     bin_load_addr	: 調整パラメータバイナリ展開先領域先頭アドレス
 * @param[in]     init_prm		: 初期化パラメータ
 *
 * @retval  D_IPC_VHDR_OK
 * @retval  D_IPC_VHDR_NG_PARAM
 */

static VOID ipc_vhdr_int_handler_shdr( ULONG interrupt_flag_mask )
{
	if ( interrupt_flag_mask != 0 ) {
		FJ_Debug_Print_Err( "    [VHDR] gIpcVhdr_int_handler_shdr() : AXI Error = 0x%08lx\n", interrupt_flag_mask );
	}

	if(gIpcVhdr_Shdr_Func_CB != NULL)
	{
		gIpcVhdr_Shdr_Func_CB(interrupt_flag_mask);
	}
}


static IPC_INT ipc_vhdr_set_shdr( BOOL set_degamma )
{
#ifndef CO_DEBUG_ON_PC
	T_IM_SHDR_PMSK	IpcVhdr_Pmsk_Prm;
	T_IM_SHDR_SPNR	IpcVhdr_Spnr_Prm;
	INT32	ercd;

#if 1	// W/A for EPK-1754 (To avoid SHDR Resize Issue, HDR output is -127~127 ( instead of -128~127 ) )
	if( gIpcVhdr_Shdr_Pre_Prm.r2y_clp.lower_short_c[ 0 ] < (-127*16) ){
		gIpcVhdr_Shdr_Pre_Prm.r2y_clp.lower_short_c[ 0 ] = (-127*16);
	}
	if( gIpcVhdr_Shdr_Pre_Prm.r2y_clp.lower_short_c[ 1 ] < (-127*16) ){
		gIpcVhdr_Shdr_Pre_Prm.r2y_clp.lower_short_c[ 1 ] = (-127*16);
	}
	if( gIpcVhdr_Shdr_Pre_Prm.r2y_clp.lower_long_c [ 0 ] < (-127*16) ){
		gIpcVhdr_Shdr_Pre_Prm.r2y_clp.lower_long_c [ 0 ] = (-127*16);
	}
	if( gIpcVhdr_Shdr_Pre_Prm.r2y_clp.lower_long_c [ 1 ] < (-127*16) ){
		gIpcVhdr_Shdr_Pre_Prm.r2y_clp.lower_long_c [ 1 ] = (-127*16);
	}
#endif

	ercd = Im_SHDR_Set_Pre( (T_IM_SHDR_PRE*)&gIpcVhdr_Shdr_Pre_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] Im_SHDR_Set_Pre() error. ercd[0x%08x]\n", ercd );
		return D_IPC_VHDR_NG;
	}

	memset( &IpcVhdr_Pmsk_Prm, 0, sizeof(IpcVhdr_Pmsk_Prm) );
	ercd = Im_SHDR_Set_PreMask( (T_IM_SHDR_PMSK*)&IpcVhdr_Pmsk_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] Im_SHDR_Set_PreMask() error. ercd[0x%08x]\n", ercd );
		return D_IPC_VHDR_NG;
	}

	//	To disable SPNR
	memset( &IpcVhdr_Spnr_Prm, 0, sizeof(IpcVhdr_Spnr_Prm) );
	IpcVhdr_Spnr_Prm.spnr_en = D_IM_SHDR_DISABLE;

//	ercd = Im_SHDR_Set_Spnr( (T_IM_SHDR_SPNR*)&gIpcVhdr_Spnr_Prm );
	ercd = Im_SHDR_Set_Spnr( (T_IM_SHDR_SPNR*)&IpcVhdr_Spnr_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] Im_SHDR_Set_Spnr() error. ercd[0x%08x]\n", ercd );
		return D_IPC_VHDR_NG;
	}

	ercd = Im_SHDR_Set_Mask( (T_IM_SHDR_MSK*)&gIpcVhdr_Shdr_Msk_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] Im_SHDR_Set_Mask() error. ercd[0x%08x]\n", ercd );
		return D_IPC_VHDR_NG;
	}

	ercd = Im_SHDR_Set_Blend( (T_IM_SHDR_BLD*)&gIpcVhdr_Shdr_Bld_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] Im_SHDR_Set_Blend() error. ercd[0x%08x]\n", ercd );
		return D_IPC_VHDR_NG;
	}

	ercd = Im_SHDR_Set_Post( (T_IM_SHDR_POST*)&gIpcVhdr_Shdr_Post_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] Im_SHDR_Set_Post() error. ercd[0x%08x]\n", ercd );
		return D_IPC_VHDR_NG;
	}

	ercd = Im_SHDR_Set_Axi( (T_IM_SHDR_AXI*)&gIpcVhdr_Axi_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] Im_SHDR_Set_Axi() error. ercd[0x%08x]\n", ercd );
		return D_IPC_VHDR_NG;
	}

	if ( set_degamma ) {
		ercd = Im_SHDR_Set_Degamma( (USHORT*)&gIpcVhdr_Degamma_Prm[ 0 ][ 0 ],
									(USHORT*)&gIpcVhdr_Degamma_Prm[ 1 ][ 0 ],
									(USHORT*)&gIpcVhdr_Degamma_Prm[ 2 ][ 0 ],
									0,
									D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE );
		if ( ercd != D_DDIM_OK ) {
			FJ_Debug_Print_Err( "    [VHDR] Im_SHDR_Set_Degamma() error. ercd[0x%08x]\n", ercd );
			return D_IPC_VHDR_NG;
		}
	}
#else
	OS_User_Dly_Tsk( 1 );
#endif

	return D_IPC_VHDR_OK;
}


static UCHAR ipc_vhdr_get_pixel_avg_reduction_info( UINT32	srcSize, UINT32 dstSize, USHORT* pitch, UCHAR* rdc_md )
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
		*pitch	= (USHORT)ipcVhdr_calc_pit( srcSize, dstSize );
	}
	else if ( ratio < 4.0f ) {
		rdc_en	= D_IM_SHDR_ENABLE;
		*rdc_md	= D_IM_RDCMD_1_2;
		*pitch	= (USHORT)ipcVhdr_calc_pit( srcSize, dstSize * 2 );
	}
	else if ( ( ratio >= 4.0f ) && ( ratio <= 8.0f ) ) {
		rdc_en	= D_IM_SHDR_ENABLE;
		*rdc_md	= D_IM_RDCMD_1_4;
		*pitch	= (USHORT)ipcVhdr_calc_pit( srcSize, dstSize * 4 );
	}
	else {
		rdc_en	= D_IM_SHDR_ENABLE;
		*rdc_md	= D_IM_RDCMD_1_8;
		*pitch	= (USHORT)ipcVhdr_calc_pit( srcSize, dstSize * 8 );
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

	// WA 2.
	if ( ( srcSize < 3000 ) && ( dstSize == 720 ) ) {
		*pitch = *pitch - 0x0c;
	}

	return rdc_en;
}


static INT32 ipc_vhdr_start_shdr( BOOL							 bypassMode,
								 BOOL							 compMode,
								 BOOL							 edgeCpy,
								 BOOL							 edgeCut,
								 E_IPC_VHDR_FIL_MARGIN			 filMargin,
								 BOOL							 iq_update,
								 const	FJ_STILL_IP_IMAGE* const imgShort,
								 const	FJ_STILL_IP_IMAGE* const imgLong,
								 const	FJ_STILL_IP_IMAGE* const imgMe,
										FJ_STILL_IP_IMAGE* const imgHevc,
										FJ_STILL_IP_IMAGE* const imgH264_0,
										FJ_STILL_IP_IMAGE* const imgH264_1,
										FJ_STILL_IP_IMAGE* const imgDisp_Hdmi,
										FJ_STILL_IP_IMAGE* const imgDisp_Lcd,
										FJ_STILL_IP_IMAGE* const comp_w,
										FJ_STILL_IP_IMAGE* const comp_r )
{
	UCHAR tlut_mc[ 17 ] = { 0xFF, 0xEF, 0xDF, 0xCF, 0xBF, 0xAF, 0x9F, 0x8F, 0x7F, 0x6F, 0x5F, 0x4F, 0x3F, 0x2F, 0x1F, 0x0F, 0x00 };
	USHORT pitch[ 2 ];
	UCHAR rdc_md;
	UINT32 std_width;
	UINT32 std_height;
	INT32 ercd;

	// Bypass mode check(or first start check).
	if ( ( bypassMode ) || ( imgLong->imgInfo[ 0 ].pImgAdrs == NULL ) ) {
		// FLWCNT.
		gIpcVhdr_Shdr_Ctrl_Prm.image_mode.bypass_en		= D_IM_SHDR_ENABLE;
		gIpcVhdr_Shdr_Ctrl_Prm.image_mode.shdr_in		= D_IM_SHDR_SHDRIN_STN_REF;
		// SHDRIFRM.
		gIpcVhdr_Shdr_Ctrl_Prm.image_set.in_edge_cpy		= FALSE;
		if ( imgHevc->imgInfo[ 0 ].pImgAdrs != NULL ) {
			if ( imgShort->imgInfo[ 0 ].wImgSize != imgHevc->imgInfo[ 0 ].wImgSize ) {
				gIpcVhdr_Shdr_Ctrl_Prm.image_set.in_edge_cut	= TRUE;
			}
			else {
				gIpcVhdr_Shdr_Ctrl_Prm.image_set.in_edge_cut	= FALSE;
			}
		}
		else {
			gIpcVhdr_Shdr_Ctrl_Prm.image_set.in_edge_cut	= FALSE;
		}
		// POSTSEL0.
		gIpcVhdr_Shdr_Post_Prm.css_sel						= D_IM_SHDR_POSTSEL0_BYPASS_YCC8;
		// COMPIP.
		if ( compMode ) {
			gIpcVhdr_Shdr_Ctrl_Prm.compip.r_bypass_mode	= D_IM_SHDR_ENABLE;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.w_bypass_mode	= D_IM_SHDR_DISABLE;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.w_addr_y		= (ULONG)comp_w->imgInfo[ 0 ].pImgAdrs;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.w_addr_c		= (ULONG)comp_w->imgInfo[ 1 ].pImgAdrs;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.r_addr_y		= 0x00000000;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.r_addr_c		= 0x00000000;
		}
		else {
			gIpcVhdr_Shdr_Ctrl_Prm.compip.r_bypass_mode	= D_IM_SHDR_ENABLE;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.w_bypass_mode	= D_IM_SHDR_ENABLE;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.w_addr_y		= 0x00000000;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.w_addr_c		= 0x00000000;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.r_addr_y		= 0x00000000;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.r_addr_c		= 0x00000000;
		}
		gIpcVhdr_Shdr_Ctrl_Prm.compip.mc_mode			= D_IM_SHDR_COMP_MC_UNUSED;
		gIpcVhdr_Shdr_Ctrl_Prm.image_mode.shift_x		= 0;
		gIpcVhdr_Shdr_Ctrl_Prm.image_mode.shift_y		= 0;

		memset( (void*)&gIpcVhdr_Shdr_Pmsk_Prm.tlut, 0x00, sizeof(tlut_mc) );
	}
	else {
		// FLWCNT.
		gIpcVhdr_Shdr_Ctrl_Prm.image_mode.bypass_en		= D_IM_SHDR_DISABLE;
		// SHDRIFRM.
		gIpcVhdr_Shdr_Ctrl_Prm.image_set.in_edge_cpy	= edgeCpy;
		gIpcVhdr_Shdr_Ctrl_Prm.image_set.in_edge_cut	= edgeCut;
//		gIpcVhdr_Shdr_Ctrl_Prm.compip.mc_mode			= D_IM_SHDR_COMP_MC_USED;
		gIpcVhdr_Shdr_Ctrl_Prm.compip.mc_mode			= D_IM_SHDR_COMP_MC_UNUSED;

		// POSTSEL0.
		gIpcVhdr_Shdr_Post_Prm.css_sel						= D_IM_SHDR_POSTSEL0_BLEND_YCC16;
		// COMPIP.
		if ( compMode ) {
			gIpcVhdr_Shdr_Ctrl_Prm.compip.r_bypass_mode	= D_IM_SHDR_DISABLE;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.w_bypass_mode	= D_IM_SHDR_DISABLE;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.w_addr_y		= (ULONG)comp_w->imgInfo[ 0 ].pImgAdrs;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.w_addr_c		= (ULONG)comp_w->imgInfo[ 1 ].pImgAdrs;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.r_addr_y		= (ULONG)comp_r->imgInfo[ 0 ].pImgAdrs;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.r_addr_c		= (ULONG)comp_r->imgInfo[ 1 ].pImgAdrs;
		}
		else {
			gIpcVhdr_Shdr_Ctrl_Prm.compip.r_bypass_mode	= D_IM_SHDR_ENABLE;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.w_bypass_mode	= D_IM_SHDR_ENABLE;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.w_addr_y		= 0x00000000;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.w_addr_c		= 0x00000000;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.r_addr_y		= 0x00000000;
			gIpcVhdr_Shdr_Ctrl_Prm.compip.r_addr_c		= 0x00000000;
		}

		// ME/MC On.
		// FLWCNT.
		gIpcVhdr_Shdr_Ctrl_Prm.image_mode.bypass_en	= 0;
		gIpcVhdr_Shdr_Ctrl_Prm.image_mode.shdr_in	= D_IM_SHDR_SHDRIN_STN_REF;
//		gIpcVhdr_Shdr_Ctrl_Prm.image_mode.smc_in	= D_IM_SHDR_SMCIN_EXT_STN;
		gIpcVhdr_Shdr_Ctrl_Prm.image_mode.smc_in	= D_IM_SHDR_SMCIN_STN_REF;

		memset( (void*)&gIpcVhdr_Shdr_Pmsk_Prm.tlut, 0x00, sizeof(tlut_mc) );

		// SDRAM for read (MC input standard).
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_addr.b_y		= (ULONG)imgShort->imgInfo[ 0 ].pImgAdrs;		// Src image.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_addr.b_c		= (ULONG)imgShort->imgInfo[ 1 ].pImgAdrs;		// Src image.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_ghsize.b_y	= imgShort->imgInfo[ 0 ].gImgSize;			// Src image.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_ghsize.b_c	= imgShort->imgInfo[ 1 ].gImgSize;			// Src image.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_hsize.b		= imgShort->imgInfo[ 0 ].wImgSize;			// Src image.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_vsize.b		= imgShort->imgInfo[ 0 ].hImgSize;			// Src image.
		// SDRAM for read (MC input reference).
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_addr.a_y		= (ULONG)imgLong->imgInfo[ 0 ].pImgAdrs;		// Ref image.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_addr.a_c		= (ULONG)imgLong->imgInfo[ 1 ].pImgAdrs;		// Ref image.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_ghsize.a_y	= imgLong->imgInfo[ 0 ].gImgSize;			// Ref image.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_ghsize.a_c	= imgLong->imgInfo[ 1 ].gImgSize;			// Ref image.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_hsize.a		= imgLong->imgInfo[ 0 ].wImgSize;			// Ref image.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_vsize.a		= imgLong->imgInfo[ 0 ].hImgSize;			// Ref image.
		// SDRAM for read (Motion vector).
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_addr.d_y		= (ULONG)NULL;		// Motion vector.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_addr.d_c		= (ULONG)NULL;		// Motion vector.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_ghsize.d_y	= 0;			// Motion vector.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_ghsize.d_c	= 0;			// Motion vector.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_hsize.d		= 0;			// Motion vector.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_vsize.d		= 0;			// Motion vector.
#if defined(CO_IQBIN_KUBOTA)	// --- REMOVE_IQBIN_KUBOTA BEGIN ---
		memcpy( &gIpcVhdr_Pmsk_Prm.tlut, tlut_mc, sizeof(tlut_mc) );
#endif // CO_IQBIN_KUBOTA  --- REMOVE_IQBIN_KUBOTA END ---
	}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	if ( iq_update ) {
		ercd = ipc_vhdr_set_shdr( FALSE );
		if( ercd != D_DDIM_OK ) {
			FJ_Debug_Print_Err( "    [VHDR] gIpcVhdr_set_shdr() error. ercd=0x%08x.\n", ercd );
			return D_IPC_VHDR_NG;
		}
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	if ( iq_update ) {
		ercd = ipc_vhdr_set_shdr( FALSE );
		if( ercd != D_DDIM_OK ) {
			FJ_Debug_Print_Err( "    [VHDR] gIpcVhdr_set_shdr() error. ercd=0x%08x.\n", ercd );
			return D_IPC_VHDR_NG;
		}
	}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	if ( imgShort->imgForm == FJ_STILL_IP_IMAGE_FORM_420SP ) {
		gIpcVhdr_Shdr_Ctrl_Prm.image_set.in_c_fmt		= D_IM_SHDR_INCFMT_420SP;
	}
	else {
		gIpcVhdr_Shdr_Ctrl_Prm.image_set.in_c_fmt		= D_IM_SHDR_INCFMT_422SP;
	}

#ifdef D_DEBUG_SET_RECOMMENDED
	gIpcVhdr_Shdr_Ctrl_Prm.image_mode.fil_margin = 0; // set 0 otherwise there are uncombine are at top/bot/left/right 16pix
#else
	// MRGN.
	if ( filMargin == E_IPC_VHDR_FIL_MARGIN_16 ) {
		gIpcVhdr_Shdr_Ctrl_Prm.image_mode.fil_margin	= D_IM_SHDR_FLITER_MARGIN_16;
	}
	else {
		gIpcVhdr_Shdr_Ctrl_Prm.image_mode.fil_margin	= D_IM_SHDR_FLITER_MARGIN_32;
	}
#endif

	// SDRAM for read(Standard).
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_addr.b_y		= (ULONG)imgShort->imgInfo[ 0 ].pImgAdrs;			// Src image.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_addr.b_c		= (ULONG)imgShort->imgInfo[ 1 ].pImgAdrs;			// Src image.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_ghsize.b_y	= imgShort->imgInfo[ 0 ].gImgSize;				// Src image.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_ghsize.b_c	= imgShort->imgInfo[ 1 ].gImgSize;				// Src image.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_hsize.b		= imgShort->imgInfo[ 0 ].wImgSize;				// Src image.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.r_vsize.b		= imgShort->imgInfo[ 0 ].hImgSize;				// Src image.
	// SDRAM for write.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_baddr.a_y	= (ULONG)imgHevc->imgInfo[ 0 ].pImgAdrs;		// HEVC enc.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_baddr.a_c	= (ULONG)imgHevc->imgInfo[ 1 ].pImgAdrs;		// HEVC enc.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_hsize.a		= imgHevc->imgInfo[ 0 ].gImgSize;				// HEVC enc.
//	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_addr.b_y		= (ULONG)imgMeIn->imgInfo[ 0 ].pImgAdrs;		// Me input.
//	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_addr.b_c		= (ULONG)imgMeIn->imgInfo[ 1 ].pImgAdrs;		// Me input.
//	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_ghsize.b_y	= imgMeIn->imgInfo[ 0 ].gImgSize;				// Me input.
//	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_ghsize.b_c	= imgMeIn->imgInfo[ 1 ].gImgSize;				// Me input.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_baddr.c_y	= (ULONG)imgH264_0->imgInfo[ 0 ].pImgAdrs;		// H264 enc 0.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_baddr.c_c	= (ULONG)imgH264_0->imgInfo[ 1 ].pImgAdrs;		// H264 enc 0.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_hsize.c		= imgH264_0->imgInfo[ 0 ].gImgSize;				// H264 enc 0.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_baddr.d_y	= (ULONG)imgH264_1->imgInfo[ 0 ].pImgAdrs;		// H264 enc 1.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_baddr.d_c	= (ULONG)imgH264_1->imgInfo[ 1 ].pImgAdrs;		// H264 enc 1.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_hsize.d		= imgH264_1->imgInfo[ 0 ].gImgSize;				// H264 enc 1.
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_addr.e_y		= (ULONG)imgDisp_Hdmi->imgInfo[ 0 ].pImgAdrs;	// Disp(HDMI).
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_addr.e_c		= (ULONG)imgDisp_Hdmi->imgInfo[ 1 ].pImgAdrs;	// Disp(HDMI).
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_ghsize.e_y	= imgDisp_Hdmi->imgInfo[ 0 ].gImgSize;			// Disp(HDMI).
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_ghsize.e_c	= imgDisp_Hdmi->imgInfo[ 1 ].gImgSize;			// Disp(HDMI).
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_addr.f_y		= (ULONG)imgDisp_Lcd->imgInfo[ 0 ].pImgAdrs;	// Disp(LCD).
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_addr.f_c		= (ULONG)imgDisp_Lcd->imgInfo[ 1 ].pImgAdrs;	// Disp(LCD).
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_ghsize.f_y	= imgDisp_Lcd->imgInfo[ 0 ].gImgSize;			// Disp(LCD).
	gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_ghsize.f_c	= imgDisp_Lcd->imgInfo[ 1 ].gImgSize;			// Disp(LCD).

	// Resize.
	if ( imgHevc->imgInfo[ 0 ].pImgAdrs == NULL || imgHevc->imgForm == FJ_STILL_IP_IMAGE_FORM_RGB_P ) {
		gIpcVhdr_Shdr_Resize_Prm.chmode.a_w_en	= D_IM_SHDR_DISABLE;							// HEVC enc.
		std_width	= 0;
		std_height	= 0;
	}
	else {
		gIpcVhdr_Shdr_Resize_Prm.chmode.a_w_en	= D_IM_SHDR_ENABLE;								// HEVC enc.
		std_width	= imgHevc->imgInfo[ 0 ].wImgSize;
		std_height	= imgHevc->imgInfo[ 0 ].hImgSize;
	}
	gIpcVhdr_Shdr_Resize_Prm.chmode.a_drc_en	= D_IM_SHDR_DISABLE;							// HEVC enc.
	gIpcVhdr_Shdr_Resize_Prm.chmode.a_css_md	= D_IM_SHDR_CSSMD_YCC420_CENTER;				// HEVC enc.
	gIpcVhdr_Shdr_Resize_Prm.woutsz.a_hsize		= imgHevc->imgInfo[ 0 ].wImgSize;				// HEVC enc.
	gIpcVhdr_Shdr_Resize_Prm.woutsz.a_vsize		= imgHevc->imgInfo[ 0 ].hImgSize;				// HEVC enc.

	// unused.
	gIpcVhdr_Shdr_Resize_Prm.chmode.b_w_en		= D_IM_SHDR_DISABLE;							// Me input.
//	gIpcVhdr_Shdr_Resize_Prm.chmode.b_rdc_md		= D_IM_RDCMD1_1_4;								// Me input.
//	gIpcVhdr_Shdr_Resize_Prm.chmode.b_css_md		= D_IM_SHDR_CSSMD_YCC420_ODD;					// Me input.
//	gIpcVhdr_Shdr_Resize_Prm.woutsz.b_hsize		= imgMeIn->imgInfo[ 0 ].wImgSize;				// Me input.
//	gIpcVhdr_Shdr_Resize_Prm.woutsz.b_vsize		= imgMeIn->imgInfo[ 0 ].hImgSize;				// Me input.

	if ( imgH264_0->imgInfo[ 0 ].pImgAdrs == NULL ) {
		gIpcVhdr_Shdr_Resize_Prm.chmode.c_w_en	= D_IM_SHDR_DISABLE;							// H264 enc 0.
	}
	else {
		gIpcVhdr_Shdr_Resize_Prm.chmode.c_w_en	= D_IM_SHDR_ENABLE;								// H264 enc 0.
		if ( std_width == 0 ) {
			std_width	= imgH264_0->imgInfo[ 0 ].wImgSize;
			std_height	= imgH264_0->imgInfo[ 0 ].hImgSize;
		}
	}

	gIpcVhdr_Shdr_Resize_Prm.chmode.c_rsz_md	= D_IM_RSZMD_ODD0;								// H264 enc 0.
	gIpcVhdr_Shdr_Resize_Prm.chmode.c_trm_en	= D_IM_SHDR_DISABLE;							// H264 enc 0.
	gIpcVhdr_Shdr_Resize_Prm.chmode.c_drc_en	= D_IM_SHDR_DISABLE;							// H264 enc 0.
	gIpcVhdr_Shdr_Resize_Prm.chmode.c_css_md	= D_IM_SHDR_CSSMD_YCC420_ODD;					// H264 enc 0.
	gIpcVhdr_Shdr_Resize_Prm.woutsz.c_hsize		= imgH264_0->imgInfo[ 0 ].wImgSize;				// H264 enc 0.
	gIpcVhdr_Shdr_Resize_Prm.woutsz.c_vsize		= imgH264_0->imgInfo[ 0 ].hImgSize;				// H264 enc 0.
	gIpcVhdr_Shdr_Resize_Prm.chmode.c_rdc_en	= ipc_vhdr_get_pixel_avg_reduction_info( std_height,
																					imgH264_0->imgInfo[ 0 ].hImgSize,
																					&pitch[ 1 ],
																					&rdc_md );
	gIpcVhdr_Shdr_Resize_Prm.chmode.c_rdc_md		= rdc_md;
	ipc_vhdr_get_pixel_avg_reduction_info( std_width,
												imgH264_0->imgInfo[ 0 ].wImgSize,
												&pitch[ 0 ],
												&rdc_md );
	gIpcVhdr_Shdr_Resize_Prm.rszhcnt.c_hpit		= pitch[ 0 ];									// H264 enc 0.
	gIpcVhdr_Shdr_Resize_Prm.rszvcnt.c_vpit		= pitch[ 1 ];									// H264 enc 0.

	if ( imgH264_1->imgInfo[ 0 ].pImgAdrs == NULL ) {
		gIpcVhdr_Shdr_Resize_Prm.chmode.d_w_en	= D_IM_SHDR_DISABLE;							// H264 enc 1.
	}
	else {
		gIpcVhdr_Shdr_Resize_Prm.chmode.d_w_en	= D_IM_SHDR_ENABLE;								// H264 enc 1
		if ( std_width == 0 ) {
			std_width	= imgH264_1->imgInfo[ 0 ].wImgSize;
			std_height	= imgH264_1->imgInfo[ 0 ].hImgSize;
		}
	}
	gIpcVhdr_Shdr_Resize_Prm.chmode.d_rsz_md	= D_IM_RSZMD_ODD0;								// H264 enc 1.
	gIpcVhdr_Shdr_Resize_Prm.chmode.d_trm_en	= D_IM_SHDR_ENABLE;								// H264 enc 1.
	gIpcVhdr_Shdr_Resize_Prm.chmode.d_drc_en	= D_IM_SHDR_DISABLE;							// H264 enc 1.
	gIpcVhdr_Shdr_Resize_Prm.chmode.d_css_md	= D_IM_SHDR_CSSMD_YCC420_ODD;					// H264 enc 1.
	gIpcVhdr_Shdr_Resize_Prm.woutsz.d_hsize		= imgH264_1->imgInfo[ 0 ].wImgSize;				// H264 enc 1.
	gIpcVhdr_Shdr_Resize_Prm.woutsz.d_vsize		= imgH264_1->imgInfo[ 0 ].hImgSize;				// H264 enc 1.
	gIpcVhdr_Shdr_Resize_Prm.chmode.d_rdc_en	= ipc_vhdr_get_pixel_avg_reduction_info( std_height,
																					imgH264_1->imgInfo[ 0 ].hImgSize,
																					&pitch[ 1 ],
																					&rdc_md );
	gIpcVhdr_Shdr_Resize_Prm.chmode.d_rdc_md	= rdc_md;
	(VOID)ipc_vhdr_get_pixel_avg_reduction_info( std_width,
												imgH264_1->imgInfo[ 0 ].wImgSize,
												&pitch[ 0 ],
												&rdc_md );
	gIpcVhdr_Shdr_Resize_Prm.rszhcnt.d_hpit		= pitch[ 0 ];									// H264 enc 1.
	gIpcVhdr_Shdr_Resize_Prm.rszvcnt.d_vpit		= pitch[ 1 ];									// H264 enc 1.

	if ( imgDisp_Hdmi->imgInfo[ 0 ].pImgAdrs == NULL ) {
		gIpcVhdr_Shdr_Resize_Prm.chmode.e_w_en	= D_IM_SHDR_DISABLE;							// Disp(HDMI) or Main.
	}
	else {
		gIpcVhdr_Shdr_Resize_Prm.chmode.e_w_en	= D_IM_SHDR_ENABLE;								// Disp(HDMI) or Main.

		if ( ( (FLOAT)imgDisp_Hdmi->imgInfo[ 0 ].wImgSize / (FLOAT)imgDisp_Hdmi->imgInfo[ 0 ].hImgSize ) >= 1.7f ) {
			gIpcVhdr_Shdr_Resize_Prm.chmode.e_rdc_en	= ipc_vhdr_get_pixel_avg_reduction_info( std_height,
																						imgDisp_Hdmi->imgInfo[ 0 ].hImgSize,
																						&pitch[ 1 ],
																						&rdc_md );
			gIpcVhdr_Shdr_Resize_Prm.chmode.e_rdc_md	= rdc_md;
			(VOID)ipc_vhdr_get_pixel_avg_reduction_info( std_width,
														imgDisp_Hdmi->imgInfo[ 0 ].wImgSize,
														&pitch[ 0 ],
														&rdc_md );
		}
		else {
			gIpcVhdr_Shdr_Resize_Prm.chmode.e_rdc_en	= ipc_vhdr_get_pixel_avg_reduction_info( imgShort->imgInfo[ 0 ].hImgSize,
																						imgDisp_Hdmi->imgInfo[ 0 ].hImgSize,
																						&pitch[ 1 ],
																						&rdc_md );
			gIpcVhdr_Shdr_Resize_Prm.chmode.e_rdc_md	= rdc_md;
			(VOID)ipc_vhdr_get_pixel_avg_reduction_info( imgShort->imgInfo[ 0 ].wImgSize,
														imgDisp_Hdmi->imgInfo[ 0 ].wImgSize,
														&pitch[ 0 ],
														&rdc_md );
		}
		if( imgDisp_Hdmi->imgForm == FJ_STILL_IP_IMAGE_FORM_420SP ){
			gIpcVhdr_Shdr_Resize_Prm.chmode.e_css_md	= D_IM_SHDR_CSSMD_YCC420_ODD;
		}
		else{
			gIpcVhdr_Shdr_Resize_Prm.chmode.e_css_md	= D_IM_SHDR_CSSMD_YCC422;
		}
	}
	gIpcVhdr_Shdr_Resize_Prm.chmode.e_rsz_md		= D_IM_RSZMD_ODD0;								// Disp(HDMI) or Main..
	gIpcVhdr_Shdr_Resize_Prm.chmode.e_trm_en		= D_IM_SHDR_DISABLE;							// Disp(HDMI) or Main.
	gIpcVhdr_Shdr_Resize_Prm.woutsz.e_hsize		= imgDisp_Hdmi->imgInfo[ 0 ].wImgSize;			// Disp(HDMI) or Main.
	gIpcVhdr_Shdr_Resize_Prm.woutsz.e_vsize		= imgDisp_Hdmi->imgInfo[ 0 ].hImgSize;			// Disp(HDMI) or Main.
	gIpcVhdr_Shdr_Resize_Prm.rszhcnt.e_hpit		= pitch[ 0 ];									// Disp(HDMI) or Main.
	gIpcVhdr_Shdr_Resize_Prm.rszvcnt.e_vpit		= pitch[ 1 ];									// Disp(HDMI) or Main.

	if ( imgDisp_Lcd->imgInfo[ 0 ].pImgAdrs == NULL ) {
		gIpcVhdr_Shdr_Resize_Prm.chmode.f_w_en	= D_IM_SHDR_DISABLE;							// Disp(LCD) or Screennail.
	}
	else {
		gIpcVhdr_Shdr_Resize_Prm.chmode.f_w_en	= D_IM_SHDR_ENABLE;								// Disp(LCD) or Screennail.

		if ( ( (FLOAT)imgDisp_Lcd->imgInfo[ 0 ].wImgSize / (FLOAT)imgDisp_Lcd->imgInfo[ 0 ].hImgSize ) >= 1.7f ) {
			gIpcVhdr_Shdr_Resize_Prm.chmode.f_rdc_en		= ipc_vhdr_get_pixel_avg_reduction_info( std_height,
																							imgDisp_Lcd->imgInfo[ 0 ].hImgSize,
																							&pitch[ 1 ],
																							&rdc_md );
			gIpcVhdr_Shdr_Resize_Prm.chmode.f_rdc_md	= rdc_md;
			(VOID)ipc_vhdr_get_pixel_avg_reduction_info( std_width,
														imgDisp_Lcd->imgInfo[ 0 ].wImgSize,
														&pitch[ 0 ],
														&rdc_md );
		}
		else {
			gIpcVhdr_Shdr_Resize_Prm.chmode.f_rdc_en		= ipc_vhdr_get_pixel_avg_reduction_info( imgShort->imgInfo[ 0 ].hImgSize,
																							imgDisp_Lcd->imgInfo[ 0 ].hImgSize,
																							&pitch[ 1 ],
																							&rdc_md );
			gIpcVhdr_Shdr_Resize_Prm.chmode.f_rdc_md	= rdc_md;
			(VOID)ipc_vhdr_get_pixel_avg_reduction_info( imgShort->imgInfo[ 0 ].wImgSize,
														imgDisp_Lcd->imgInfo[ 0 ].wImgSize,
														&pitch[ 0 ],
														&rdc_md );
		}
		if( imgDisp_Lcd->imgForm == FJ_STILL_IP_IMAGE_FORM_420SP ){
			gIpcVhdr_Shdr_Resize_Prm.chmode.f_css_md	= D_IM_SHDR_CSSMD_YCC420_ODD;
		}
		else{
			gIpcVhdr_Shdr_Resize_Prm.chmode.f_css_md	= D_IM_SHDR_CSSMD_YCC422;
		}
	}

	if ( imgHevc->imgInfo[ 0 ].pImgAdrs != NULL && imgHevc->imgForm == FJ_STILL_IP_IMAGE_FORM_RGB_P ) {
		//	Output RGB.
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_addr.g_r_y	= (ULONG)imgHevc->imgInfo[ 0 ].pImgAdrs;
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_addr.g_g_c	= (ULONG)imgHevc->imgInfo[ 1 ].pImgAdrs;
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_addr.g_b		= (ULONG)imgHevc->imgInfo[ 2 ].pImgAdrs;
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_ghsize.g_r_y	= imgHevc->imgInfo[ 0 ].gImgSize;		
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_ghsize.g_g_c	= imgHevc->imgInfo[ 1 ].gImgSize;		
		gIpcVhdr_Shdr_Ctrl_Prm.sdram.w_ghsize.g_b	= imgHevc->imgInfo[ 2 ].gImgSize;		

		gIpcVhdr_Shdr_Ctrl_Prm.image_set.out_ctrl	= 1;
		gIpcVhdr_Shdr_Ctrl_Prm.image_set.out_sel	= gIpcVhdr_Shdr_Ctrl_Prm.image_set.out_ctrl;
		gIpcVhdr_Shdr_Ctrl_Prm.image_set.out_sel_en	= 1;
	}
	else	{
		gIpcVhdr_Shdr_Ctrl_Prm.image_set.out_ctrl	= 0;
		gIpcVhdr_Shdr_Ctrl_Prm.image_set.out_sel	= gIpcVhdr_Shdr_Ctrl_Prm.image_set.out_ctrl;
		gIpcVhdr_Shdr_Ctrl_Prm.image_set.out_sel_en	= 0;
	}

	gIpcVhdr_Shdr_Resize_Prm.chmode.f_rsz_md	= D_IM_RSZMD_ODD0;								// Disp(LCD) or Screennail.
	gIpcVhdr_Shdr_Resize_Prm.chmode.f_trm_en	= D_IM_SHDR_DISABLE;							// Disp(LCD) or Screennail.
	gIpcVhdr_Shdr_Resize_Prm.woutsz.f_hsize		= imgDisp_Lcd->imgInfo[ 0 ].wImgSize;			// Disp(LCD) or Screennail.
	gIpcVhdr_Shdr_Resize_Prm.woutsz.f_vsize		= imgDisp_Lcd->imgInfo[ 0 ].hImgSize;			// Disp(LCD) or Screennail.

	gIpcVhdr_Shdr_Resize_Prm.rszhcnt.f_hpit		= pitch[ 0 ];									// Disp(LCD) or Screennail.
	gIpcVhdr_Shdr_Resize_Prm.rszvcnt.f_vpit		= pitch[ 1 ];									// Disp(LCD) or Screennail.

	gIpcVhdr_Shdr_Ctrl_Prm.pCallBack = (T_IM_SHDR_CALLBACK)ipc_vhdr_int_handler_shdr;

	//	Disable FMD
	gIpcVhdr_Shdr_Ctrl_Prm.image_mode.fmd_en = D_IM_SHDR_DISABLE;

#ifndef CO_DEBUG_ON_PC
	UINT32	wait_factor_result = 0;

	// SHDR macro driver set/start/end.
	ercd = Im_SHDR_Ctrl_Common( (T_IM_SHDR_CTRL*)&gIpcVhdr_Shdr_Ctrl_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] Im_SHDR_Ctrl_Common() error. ercd[0x%08x]\n", ercd );
		return D_IPC_VHDR_NG;
	}

/*	ercd = Im_SHDR_Set_Post( (T_IM_SHDR_POST*)&gIpcVhdr_Shdr_Post_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "Im_SHDR_Set_Post() error. ercd[0x%08x]\n", ercd );
		return D_IPC_VHDR_NG;
	}
*/
	ercd = Im_SHDR_Set_Resize( (T_IM_SHDR_RESIZE*)&gIpcVhdr_Shdr_Resize_Prm );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] Im_SHDR_Set_Resize() error. ercd[0x%08x]\n", ercd );
		return D_IPC_VHDR_NG;
	}

	ercd = Im_SHDR_Start();
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] Im_SHDR_Start() error. ercd[0x%08x]\n", ercd );
		return D_IPC_VHDR_NG;
	}

	ercd = Im_SHDR_Wait_End( &wait_factor_result, D_IPC_VHDR_WAIT_TIME );
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] Im_SHDR_Wait_End() error. ercd[0x%08x],  wait_factor_result = 0x%x\n", ercd, wait_factor_result );

		ercd = Im_SHDR_Init( TRUE );
		if( ercd != D_DDIM_OK ) {
			FJ_Debug_Print_Err( "    [VHDR] Im_SHDR_Init error. ercd=0x%08x.\n", ercd );
			return D_IPC_VHDR_NG;
		}

		return D_IPC_VHDR_NG_TO;
	}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	ercd = Im_SHDR_Init( FALSE );
	if( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] Im_SHDR_Init error. ercd=0x%08x.\n", ercd );
		return D_IPC_VHDR_NG;
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

#else
	OS_User_Dly_Tsk( 14 );
#endif

	return D_IPC_VHDR_OK;
}


/*--------------------------------------------------------------------------------------*/
/* API Functions																		*/
/*--------------------------------------------------------------------------------------*/
/**
	Opens the VHDR library.
*/
IPC_INT IPC_VHDR_Open( IPC_VOID )//, IP_3DNR_OPEN_PRM* openPrm )
{
#ifndef IPC_VHDR_STUB
	IPC_INT				ercd;


	ercd = IPC_HDRPRM_Open();
	if( ercd != D_IPC_HDRPRM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] IPC_VHDR_Open() IPC_HDRPRM_Open error. ercd=0x%08x.\n", ercd );
		return D_IPC_VHDR_NG;
	}

	gIpcVhdr_isOpen		= D_IPC_VHDR_OPENBIT;

#endif
	return D_IPC_VHDR_OK;
}

/**
	Initializes the VHDR library.
*/
IPC_INT IPC_VHDR_Init( IPC_UINT heap_addr, IPC_UINT* heap_size,
						IPC_UINT width, IPC_UINT height,
						IPC_INT gamma_tbl_type, IPC_USHORT* gamma_tbl )
{
#ifndef IPC_VHDR_STUB
	IPC_INT	ercd;


	// ライブラリオープンフラグチェック
	if ( !gIpcVhdr_isOpen ) {
		FJ_Debug_Print_Err( "    [VHDR] IPC_VHDR_Init() NG_OPEN isOpen[0x%x]\n", gIpcVhdr_isOpen );
		return D_IPC_VHDR_OK;
	}

	// Initialization.
/*	memset( (void*)&gIpcVhdr_Start_Prm,                 0, sizeof( gIpcVhdr_Start_Prm ) );
	memset( (void*)&gIpcVhdr_Input_Prev_Frame_Info,     0, sizeof( gIpcVhdr_Input_Prev_Frame_Info ) );
	memset( (void*)&gIpcVhdr_Input_Prev_Frame_Info_Sub, 0, sizeof( gIpcVhdr_Input_Prev_Frame_Info_Sub ) );
	memset( (void*)&gIpcVhdr_Output_Prev_Comp_Info,     0, sizeof( gIpcVhdr_Output_Prev_Comp_Info ) );
*/
	gIpcVhdr_HeapAdd		= heap_addr;
	gIpcVhdr_HeapSize		= *heap_size;
	gIpcVhdr_width			= width;
	gIpcVhdr_height			= height;
	gIpcVhdr_gamma_tbl_type = gamma_tbl_type;
	gIpcVhdr_gamma_tbl		= gamma_tbl;

	ercd = IPC_HDRPRM_Init( gIpcVhdr_HeapAdd, &gIpcVhdr_HeapSize,
							gIpcVhdr_width, gIpcVhdr_height,
						    gIpcVhdr_gamma_tbl_type, gIpcVhdr_gamma_tbl);
	if( ercd != D_IPC_HDRPRM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] IPC_VHDR_Init() IPC_HDRPRM_Init error. ercd=0x%08x.\n", ercd );
		return D_IPC_VHDR_NG;
	}

	gIpcVhdr_isOpen |= D_IPC_VHDR_INITBIT ;

#endif
	return D_IPC_VHDR_OK;
}

/**
	Get the blend table.
*/
IPC_INT IPC_VHDR_Calc_Blend_Table( IPC_UCHAR th_y, IPC_UCHAR* lc_lut_lo2sh,
									T_IPC_VHDR_HIST* hist_lo, T_IPC_VHDR_HIST* hist_sh,
									IPC_INT calc_mode, IPC_UCHAR min_bld_ratio,
									T_IPC_VHDR_BLD_TBL* bld_tbl )
{
#ifndef IPC_VHDR_STUB
	INT32	ercd;


	ercd = IPC_HDRPRM_Get_Blend_Table( th_y, lc_lut_lo2sh,
										(T_IPC_HDRPRM_HIST*)hist_lo, (T_IPC_HDRPRM_HIST*)hist_sh,
										calc_mode, min_bld_ratio,
										(T_IPC_HDRPRM_BLD_TBL*)&gIpcVhdr_BlendTable );

	if( ercd != D_IPC_HDRPRM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] IPC_VHDR_Get_Blent_Table() IPC_HDRPRM_Get_Blend_Table error. ercd=0x%08x.\n", ercd );
		return D_IPC_VHDR_NG;
	}

	memcpy( bld_tbl, &gIpcVhdr_BlendTable, sizeof(gIpcVhdr_BlendTable) );

#endif
	return D_IPC_VHDR_OK;
}

/**
	Sets the static parameters.
*/
INT32 IPC_VHDR_Start(  T_IPC_VHDR_START_PRM* startPrm,
							IPC_BOOL iq_update, IPC_SHORT *evc, T_IM_SHDR_CALLBACK func_cb )
{
#ifndef IPC_VHDR_STUB
	IPC_INT		ercd = D_IPC_VHDR_OK;
	FJ_STILL_IP_IMAGE		*stillIpPtr;


//	FJ_Debug_Print_Err( "    [VHDR] IPC_VHDR_Ctrl() Start\n" );
	// ライブラリオープンフラグチェック
	if ( !(gIpcVhdr_isOpen & D_IPC_VHDR_OPENBIT) ) {
		FJ_Debug_Print_Err( "    [VHDR] IPC_VHDR_Start() NG_OPEN isOpen[0x%x]\n", gIpcVhdr_isOpen );
		// Open済みではない場合
		return D_IPC_VHDR_NG_OPEN;
	} else if ( !(gIpcVhdr_isOpen & D_IPC_VHDR_INITBIT) ) {
		FJ_Debug_Print_Err( "    [VHDR] IPC_VHDR_Start() NG_SEQ isOpen[0x%x]\n", gIpcVhdr_isOpen );
		// Init済みではない場合
		return D_IPC_VHDR_NG_SEQ;
	}


	//	Check parameters
	//	Ristriction 3 (only ES3)
#if 1
	if( (((startPrm->imgShort[0].imgForm == E_IPC_IMAGE_FORM_420P)||(startPrm->imgShort[0].imgForm == E_IPC_IMAGE_FORM_420SP))&&startPrm->imgShort[0].bitDepth==16)
		|| (((startPrm->imgShort[1].imgForm == E_IPC_IMAGE_FORM_420P)||(startPrm->imgShort[1].imgForm == E_IPC_IMAGE_FORM_420SP))&&startPrm->imgShort[1].bitDepth==16)
		|| (((startPrm->imgShort[2].imgForm == E_IPC_IMAGE_FORM_420P)||(startPrm->imgShort[2].imgForm == E_IPC_IMAGE_FORM_420SP))&&startPrm->imgShort[2].bitDepth==16) )
		return	D_IPC_VHDR_NG_ARG;
#else
	//	Ristriction 9 (only ES1)
	if( (startPrm->imgShort[0].bitDepth==16)
		||(startPrm->imgShort[1].bitDepth==16)
		||(startPrm->imgShort[2].bitDepth==16) )
		return	D_IPC_VHDR_NG_ARG;

	//	Ristriction 10 (only ES1)
	if( (((IPC_UINT)startPrm->imgShort[0].imgInfo[0].pImgAdrs & 0x7) != 0) || (((IPC_UINT)startPrm->imgShort[0].imgInfo[1].pImgAdrs & 0x7) != 0) || (((IPC_UINT)startPrm->imgShort[0].imgInfo[1].pImgAdrs & 0x7) != 0)
		|| (((IPC_UINT)startPrm->imgShort[1].imgInfo[0].pImgAdrs & 0x7) != 0) || (((IPC_UINT)startPrm->imgShort[1].imgInfo[1].pImgAdrs & 0x7) != 0) || (((IPC_UINT)startPrm->imgShort[1].imgInfo[1].pImgAdrs & 0x7) != 0) )
//		|| (((IPC_UINT)startPrm->imgShort[2].imgInfo[0].pImgAdrs & 0x7) != 0) || (((IPC_UINT)startPrm->imgShort[2].imgInfo[1].pImgAdrs & 0x7) != 0) || (((IPC_UINT)startPrm->imgShort[2].imgInfo[1].pImgAdrs & 0x7) != 0) )
		return	D_IPC_VHDR_NG_ARG;
#endif


	// SHDR macro driver open.
	OS_User_Wai_Sem( SID_IP_SHDR );

#ifndef CO_DEBUG_ON_PC
	ercd = Im_SHDR_Open( D_IPC_VHDR_WAIT_TIME );
#else
	ercd = D_DDIM_OK;
	OS_User_Dly_Tsk( 1 );
#endif
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] IPC_VHDR_Start() Im_SHDR_Open error. ercd=0x%08x.\n", ercd );
		OS_User_Sig_Sem( SID_IP_SHDR );
		return D_IPC_VHDR_NG;
	}

#ifndef CO_DEBUG_ON_PC
	ercd = Im_SHDR_Init( FALSE );
#else
	ercd = D_DDIM_OK;
	OS_User_Dly_Tsk( 1 );
#endif
	if( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] IPC_VHDR_Start() Im_SHDR_Init error. ercd=0x%08x.\n", ercd );

		ercd = Im_SHDR_Close();
		if ( ercd != D_DDIM_OK ) {
			FJ_Debug_Print_Err( "    [VHDR] IPC_VHDR_Start() Im_SHDR_Close error. ercd[0x%08x]\n", ercd );
		}

		OS_User_Sig_Sem( SID_IP_SHDR );
		return D_IPC_VHDR_NG;
	}


	gIpcVhdr_Shdr_Post_Prm.ev_kn_boundary[ 0 ]		= *(evc+0);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_boundary[ 1 ]		= *(evc+1+0*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_boundary[ 2 ]		= *(evc+2+0*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_boundary[ 3 ]		= *(evc+3+0*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_boundary[ 4 ]		= *(evc+4+0*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_boundary[ 5 ]		= *(evc+5+0*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_boundary[ 6 ]		= *(evc+6+0*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_boundary[ 7 ]		= *(evc+7+0*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_boundary[ 8 ]		= *(evc+8+0*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_boundary[ 9 ]		= *(evc+9+0*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_offset[ 0 ]		= *(evc+0+1*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_offset[ 1 ]		= *(evc+1+1*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_offset[ 2 ]		= *(evc+2+1*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_offset[ 3 ]		= *(evc+3+1*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_offset[ 4 ]		= *(evc+4+1*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_offset[ 5 ]		= *(evc+5+1*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_offset[ 6 ]		= *(evc+6+1*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_offset[ 7 ]		= *(evc+7+1*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_offset[ 8 ]		= *(evc+8+1*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_offset[ 9 ]		= *(evc+9+1*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_slope[ 0 ]			= *(evc+0+2*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_slope[ 1 ]			= *(evc+1+2*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_slope[ 2 ]			= *(evc+2+2*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_slope[ 3 ]			= *(evc+3+2*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_slope[ 4 ]			= *(evc+4+2*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_slope[ 5 ]			= *(evc+5+2*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_slope[ 6 ]			= *(evc+6+2*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_slope[ 7 ]			= *(evc+7+2*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_slope[ 8 ]			= *(evc+8+2*10);
	gIpcVhdr_Shdr_Post_Prm.ev_kn_slope[ 9 ]			= *(evc+9+2*10);
	
#if defined( D_DEBUG_VHDR_OUT_LONG ) || defined( D_DEBUG_VHDR_OUT_SHORT )
	// debug
	{
		INT32 debug_i;
		INT32 debug_ratio;
		INT32 debug_offset;
		
		#if defined( D_DEBUG_VHDR_OUT_LONG )
		debug_ratio = 0x00;
		debug_offset = 0;
		#elif defined( D_DEBUG_VHDR_OUT_SHORT )
		debug_ratio = 0xFF;
		debug_offset = 255;
		#endif
		gIpcVhdr_Shdr_Bld_Prm.alpha_sel = D_IM_SHDR_ALPHASEL_FIXED;
		gIpcVhdr_Shdr_Bld_Prm.alpha_val[ 0 ] = debug_ratio;
		gIpcVhdr_Shdr_Bld_Prm.alpha_val[ 1 ] = debug_ratio;
		gIpcVhdr_Shdr_Bld_Prm.alpha_val[ 2 ] = debug_ratio;
		
		for( debug_i = 0; debug_i < 7; debug_i++ ){
			gIpcVhdr_BlendTable.bldb[ debug_i ]		= 0;
			gIpcVhdr_BlendTable.bldofs[ debug_i ]	= debug_offset;
			gIpcVhdr_BlendTable.bldsl[ debug_i ]	= 0;
		}
		for( debug_i = 0; debug_i < 10; debug_i++ ){
			gIpcVhdr_Shdr_Post_Prm.ev_kn_boundary[ debug_i ]	= 0;
			gIpcVhdr_Shdr_Post_Prm.ev_kn_offset[ debug_i ]		= 512;
			gIpcVhdr_Shdr_Post_Prm.ev_kn_slope[ debug_i ]		= 0;
		}
		gIpcVhdr_BlendTable.bldb[ 6 ]				= 4095;
		gIpcVhdr_Shdr_Post_Prm.ev_kn_boundary[ 9 ]	= 4095;
	}
#endif


	// Previous image address saved.
	if ( startPrm->imgDst[ 0 ].imgInfo[ 0 ].pImgAdrs != NULL ) {
//		gIpcVhdr_Input_Prev_Frame_Info		= (FJ_STILL_IP_IMAGE)startPrm->imgDst[ 0 ];
		stillIpPtr = (FJ_STILL_IP_IMAGE*)&startPrm->imgDst[ 0 ];
		gIpcVhdr_Input_Prev_Frame_Info		= *stillIpPtr;
	}
	else {
//		gIpcVhdr_Input_Prev_Frame_Info		= (FJ_STILL_IP_IMAGE)startPrm->imgDst[ 2 ];
		stillIpPtr = (FJ_STILL_IP_IMAGE*)&startPrm->imgDst[ 2 ];
		gIpcVhdr_Input_Prev_Frame_Info		= *stillIpPtr;
	}
//	gIpcVhdr_Input_Prev_Frame_Info_Sub[ 0 ]	= (FJ_STILL_IP_IMAGE)startPrm->imgShort[ 1 ];
	stillIpPtr = (FJ_STILL_IP_IMAGE*)&startPrm->imgShort[ 1 ];
	gIpcVhdr_Input_Prev_Frame_Info_Sub[ 0 ]	= *stillIpPtr;
//	gIpcVhdr_Input_Prev_Frame_Info_Sub[ 1 ]	= (FJ_STILL_IP_IMAGE)startPrm->imgShort[ 2 ];
	stillIpPtr = (FJ_STILL_IP_IMAGE*)&startPrm->imgShort[ 2 ];
	gIpcVhdr_Input_Prev_Frame_Info_Sub[ 1 ]	= *stillIpPtr;
//	gIpcVhdr_Output_Prev_Comp_Info			= (FJ_STILL_IP_IMAGE)startPrm->cmpTbl[ 0 ];
	stillIpPtr = (FJ_STILL_IP_IMAGE*)&startPrm->cmpTbl[ 0 ];
	gIpcVhdr_Output_Prev_Comp_Info		= *stillIpPtr;
	
#ifdef D_DEBUG_SET_RECOMMENDED
	// to prevent hunting of blend ratio by LPF
	lpf_blendtable( &gIpcVhdr_BlendTable );
#endif
	{
		gIpcVhdr_Shdr_Msk_Prm.blend_offset[0] = gIpcVhdr_BlendTable.bldofs[0];
		gIpcVhdr_Shdr_Msk_Prm.blend_offset[1] = gIpcVhdr_BlendTable.bldofs[1];
		gIpcVhdr_Shdr_Msk_Prm.blend_offset[2] = gIpcVhdr_BlendTable.bldofs[2];
		gIpcVhdr_Shdr_Msk_Prm.blend_offset[3] = gIpcVhdr_BlendTable.bldofs[3];
		gIpcVhdr_Shdr_Msk_Prm.blend_offset[4] = gIpcVhdr_BlendTable.bldofs[4];
		gIpcVhdr_Shdr_Msk_Prm.blend_offset[5] = gIpcVhdr_BlendTable.bldofs[5];
		gIpcVhdr_Shdr_Msk_Prm.blend_offset[6] = gIpcVhdr_BlendTable.bldofs[6];
		memcpy( (void*)gIpcVhdr_Shdr_Msk_Prm.blend_slope   , gIpcVhdr_BlendTable.bldsl , sizeof(gIpcVhdr_BlendTable.bldsl) );
		memcpy( (void*)gIpcVhdr_Shdr_Msk_Prm.blend_boundary, gIpcVhdr_BlendTable.bldb  , sizeof(gIpcVhdr_BlendTable.bldb) );
	}

	gIpcVhdr_Shdr_Func_CB = func_cb;

//	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_3DNR, FJ_MBALOG_TYPE_START, "3DNR_SHDR" ));

	ercd = ipc_vhdr_start_shdr(	startPrm->bypassMode,									// [In]Bypass mode.
								startPrm->compMode,										// [In]Compression mode.
								startPrm->edgeCpy,										// [In]Edge copy.
								startPrm->edgeCut,										// [In]Edge cut.
								startPrm->filMargin,													// [In]filter margin.
								iq_update,												// [In]IQ update flag.
								(FJ_STILL_IP_IMAGE*)&startPrm->imgShort[ 0 ],									// [In]Source image(YUV8).
								(FJ_STILL_IP_IMAGE*)&startPrm->imgLong[ 0 ],									// [In]Reference image(YUV8).
								(FJ_STILL_IP_IMAGE*)&startPrm->imgMe,										// [In]Motion vector.
								(FJ_STILL_IP_IMAGE*)&startPrm->imgDst[ 0 ],									// [Out]HEVC enc.
								(FJ_STILL_IP_IMAGE*)&startPrm->imgDst[ 2 ],									// [Out]H264 enc 0.
								(FJ_STILL_IP_IMAGE*)&startPrm->imgDst[ 3 ],									// [Out]H264 enc 1.
								(FJ_STILL_IP_IMAGE*)&startPrm->imgDst[ 4 ],									// [Out]DISP HDMI(YUV8).
								(FJ_STILL_IP_IMAGE*)&startPrm->imgDst[ 5 ],									// [Out]DISP LCD(YUV8).
								(FJ_STILL_IP_IMAGE*)&startPrm->cmpTbl[ 0 ],									// [Out]Comp table write.
								(FJ_STILL_IP_IMAGE*)&startPrm->cmpTbl[ 1 ] );								// [In]Comp table read.

//	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_3DNR, FJ_MBALOG_TYPE_END, "3DNR_SHDR" ));

	// SHDR macro driver close.
#ifndef DEBUG_ON_PC
	ercd = Im_SHDR_Close();
	if ( ercd != D_DDIM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] IPC_VHDR_Start() Im_SHDR_Close error. ercd[0x%08x]\n", ercd );
		OS_User_Sig_Sem( SID_IP_SHDR );
		return D_IPC_VHDR_NG_CLOSE;
	}
#endif

	if ( ercd == D_IPC_VHDR_NG_TO ) {
		ercd = D_IPC_VHDR_OK;
	}

	OS_User_Sig_Sem( SID_IP_SHDR );

	return ercd;
#else
	return D_IPC_VHDR_OK;
#endif
}

/**
	Closes the VHDR library.
*/
IPC_INT IPC_VHDR_Close(IPC_VOID)
{
	IPC_INT		ercd = D_IPC_VHDR_OK;


	// ライブラリオープンフラグチェック
	if ( !(gIpcVhdr_isOpen & D_IPC_VHDR_OPENBIT) ) {
		FJ_Debug_Print_Err( "    [VHDR] IPC_VHDR_Close() NG_OPEN isOpen[0x%x]\n", gIpcVhdr_isOpen );
		// Open済みではない場合
		return D_IPC_VHDR_NG_SEQ;
	}

	gIpcVhdr_isOpen = 0;
	gIpcVhdr_GamNumFlg = 0;

	ercd = IPC_HDRPRM_Close();
	if( ercd != D_IPC_HDRPRM_OK ) {
		FJ_Debug_Print_Err( "    [VHDR] IPC_VHDR_Close() IPC_HDRPRM_Close error. ercd=0x%08x.\n", ercd );
		return D_IPC_VHDR_NG;
	}


	return D_IPC_VHDR_OK;
}


#if defined(CO_IQBIN_GENERAL)	// --- REMOVE_IQBIN_GENERAL BEGIN ---
VOID IPC_VHDR_Get_RegTbl( T_IM_SHDR_CTRL** shdr_ctrl, T_IM_SHDR_PRE** shdr_pre,
						  T_IM_SHDR_MSK** shdr_msk, T_IM_SHDR_BLD** shdr_bld,
						  T_IM_SHDR_POST** shdr_post, UINT32** shdr_in )
{
	*shdr_ctrl = (T_IM_SHDR_CTRL*)&gIpcVhdr_Shdr_Ctrl_Prm;
	*shdr_pre  = (T_IM_SHDR_PRE*)&gIpcVhdr_Shdr_Pre_Prm;
//	*shdr_fmd  = (T_IM_SHDR_FMD*)&gIpcVhdr_Fmd_Prm;
//	*shdr_spnr = (T_IM_SHDR_SPNR*)&gIpcVhdr_Spnr_Prm;
	*shdr_msk  = (T_IM_SHDR_MSK*)&gIpcVhdr_Shdr_Msk_Prm;
	*shdr_bld  = (T_IM_SHDR_BLD*)&gIpcVhdr_Shdr_Bld_Prm;
	*shdr_post = (T_IM_SHDR_POST*)&gIpcVhdr_Shdr_Post_Prm;
	*shdr_in   = (UINT32*)&gIpcVhdr_ShdrIn_Prm;
}
#elif defined(CO_IQBIN_KUBOTA)	// --- REMOVE_IQBIN_KUBOTA ELSEIF ---
VOID IPC_VHDR_Get_RegTbl( T_IM_SHDR_CTRL** shdr_ctrl, T_IM_SHDR_PRE** shdr_pre, T_IM_SHDR_FMD** shdr_fmd, T_IM_SHDR_SPNR** shdr_spnr, T_IM_SHDR_MSK** shdr_msk )
{
	*shdr_ctrl = (T_IM_SHDR_CTRL*)&gIpcVhdr_Shdr_Ctrl_Prm;
	*shdr_pre  = (T_IM_SHDR_PRE*)&gIpcVhdr_Shdr_Pre_Prm;
	*shdr_fmd  = (T_IM_SHDR_FMD*)&gIpcVhdr_Shdr_Fmd_Prm;
	*shdr_spnr = (T_IM_SHDR_SPNR*)&gIpcVhdr_Shdr_Spnr_Prm;
	*shdr_msk  = (T_IM_SHDR_MSK*)&gIpcVhdr_Shdr_Msk_Prm;
}
#endif // CO_IQBIN_xxxx  --- REMOVE_IQBIN_GENERAL END ---


/**
	Gets the VHDR library version.
*/
IPC_VOID IPC_VHDR_Get_Version(IPC_CHAR** str)
{
	*str = (IPC_CHAR*)strchr( V_Ipc_Vhdr_Version, ':' ) + 1;
	return;
}

#ifdef D_DEBUG_SET_RECOMMENDED
/**
	LPF of blend table
*/
void lpf_blendtable( T_IPC_VHDR_BLD_TBL* bld_tbl )
{
	static int cnt;
	static unsigned short work_bldofs[7][D_IPC_VHDR_BLD_LPF_TAP];
	static unsigned short work_bldb[7][D_IPC_VHDR_BLD_LPF_TAP];
	static int tap;
	int i, loop, sum;
	unsigned short ave_bldofs[8] = {0,0,0,0,0,0,0,255};
	unsigned short ave_bldb[8]   = {0,0,0,0,0,0,0,4095};
	unsigned short ave_bldsl[7]  = {0,0,0,0,0,0,0};

	if (tap < D_IPC_VHDR_BLD_LPF_TAP) {
		tap++;
	}

	for (i = 0; i < 7; i++) {
		if (cnt == D_IPC_VHDR_BLD_LPF_TAP) {
			cnt = 0;
		}
		// moving average of offset
		work_bldofs[i][cnt] = bld_tbl->bldofs[i];
		sum = 0;
		for (loop = 0; loop < tap; loop++) {
			sum += work_bldofs[i][loop];
		}
		ave_bldofs[i] = sum / tap;		
		// moving average of border
		work_bldb[i][cnt] = bld_tbl->bldb[i];
		sum = 0;
		for (loop = 0; loop < tap; loop++) {
			sum += work_bldb[i][loop];
		}
		ave_bldb[i] = sum / tap;
	}
	cnt++;
	// calc slope
	for (i = 0; i < 7; i++) {
		if (ave_bldb[i + 1] == ave_bldb[i]) {
			ave_bldsl[i] = 0; // avoid division by zero
		}
		else {
			ave_bldsl[i] = ((ave_bldofs[i + 1] - ave_bldofs[i]) << 11) / (ave_bldb[i + 1] - ave_bldb[i]);
		}
	}
	// update to moving average values.
	for (i = 0; i < 7; i++) {
		bld_tbl->bldb[i]   = ave_bldb[i];
		bld_tbl->bldofs[i] = ave_bldofs[i];
		bld_tbl->bldsl[i]  = ave_bldsl[i];
	}

}
#endif
