/**
 * @file		ct_im_iip_lut.c
 * @brief		IIP LUT unit ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// CT header
#include "ct_im_iip.h"
#include "ct_im_iip_lut.h"

// ### REMOVE_RELEASE BEGIN
// PT header
#ifdef CO_PT_ENABLE
#include "palladium_test.h"
#endif //CO_PT_ENABLE

// ### REMOVE_RELEASE END
// im_iip header
#include "im_iip.h"

// MILB register header
#include "jdsiip.h"

// for Memory barrier
#include "dd_arm.h"

// for memset
#include <string.h>


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#ifndef CO_CT_IM_IIP_DISABLE

#if 1
#include <stdio.h>
#undef Ddim_Print
#define Ddim_Print(arg)		{DDIM_User_Printf arg;}		/**< printf function inside DDIM */
#endif



/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Local Method															*/
/*----------------------------------------------------------------------*/

#if !defined(CO_CT_IM_IIP_5_1_1_DISABLE_LUT) || !defined(CO_CT_IM_IIP_5_1_2_DISABLE_LUT)
static const T_IM_IIP_PARAM_LUT g_ct_im_iip_param_lut_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 57,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
	.LUTTOPCNF = {
//		[0] = {
//				.bit.WAITCONF,						// set later
//				.bit.DATACONF,						// set later
//		},
		[1] = {
				.bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE,
				.bit.DATACONF = E_IM_IIP_PARAM_PORTID_NONE,
		},
		[2] = {
				.bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE,
				.bit.DATACONF = E_IM_IIP_PARAM_PORTID_NONE,
		},
		[3] = {
				.bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE,
				.bit.DATACONF = E_IM_IIP_PARAM_PORTID_NONE,
		},
	},
	.LUTUNITCTL.bit.PORTEN = 3,		// D0=LUTA, D1=LUTB, D2=LUTC D3=through
	.LUTUNITCTL.bit.PORT3SELI = 0,
	.LUTUNITCTL.bit.PORT3SELO = 0,
	.LUTUNITCTL.bit.BRANCH = 0,
	.LUTUNITCTL.bit.PORT2SELI = 0,
	.LUTUNITCTL.bit.PORT2SELO = 0,
	.LUTUNITCTL.bit.OUTMD0 = 0,
	.LUTUNITCTL.bit.OUTMD1 = 0,
	.LUTUNITCTL.bit.OUTMD2 = 0,
	.LUTUNITCTL.bit.OUTMD3 = 0,
	.LUTCTL_A.bit = {
		.LUTBIT = 8,
		.USE = E_IM_IIP_PARAM_LUTUSE_LUT_OUT,
		.SGN = E_IM_IIP_PARAM_LUTSGN_UNSIGN,
		.SPLBIT = 0,
	},
	.LUTPRECAL_A.bit = {
		.SHIFT = 0,
		.OFSET = 0,
		.CLPMAX = 0x3fff,
		.THMAX = 0x3fff,
		.CLPMIN = 0x4000,
		.THMIN = 0x4000,
		.ABS = 1,
		.LUTADRS = 0,
	},
	.LUTCTL_B.bit = {
		.LUTBIT = 8,
		.USE = E_IM_IIP_PARAM_LUTUSE_LUT_OUT,
		.SGN = E_IM_IIP_PARAM_LUTSGN_UNSIGN,
		.SPLBIT = 0,
	},
	.LUTPRECAL_B.bit = {
		.SHIFT = 0,
		.OFSET = 0,
		.CLPMAX = 0x3fff,
		.THMAX = 0x3fff,
		.CLPMIN = 0x4000,
		.THMIN = 0x4000,
		.ABS = 1,
		.LUTADRS = 0,
	},
	.LUTCTL_C.bit = {
		.LUTBIT = 8,
		.USE = E_IM_IIP_PARAM_LUTUSE_LUT_OUT,
		.SGN = E_IM_IIP_PARAM_LUTSGN_UNSIGN,
		.SPLBIT = 0,
	},
	.LUTPRECAL_C.bit = {
		.SHIFT = 0,
		.OFSET = 0,
		.CLPMAX = 0x3fff,
		.THMAX = 0x3fff,
		.CLPMIN = 0x4000,
		.THMIN = 0x4000,
		.ABS = 1,
		.LUTADRS = 0,
	},
	.LUTCTL_D.bit = {
		.LUTBIT = 8,
		.USE = E_IM_IIP_PARAM_LUTUSE_LUT_OUT,
		.SGN = E_IM_IIP_PARAM_LUTSGN_UNSIGN,
		.SPLBIT = 0,
	},
	.LUTPRECAL_D.bit = {
		.SHIFT = 0,
		.OFSET = 0,
		.CLPMAX = 0x3fff,
		.THMAX = 0x3fff,
		.CLPMIN = 0x4000,
		.THMIN = 0x4000,
		.ABS = 1,
		.LUTADRS = 0,
	},
	.LUTCTL_E.bit = {
		.LUTBIT = 8,
		.USE = E_IM_IIP_PARAM_LUTUSE_LUT_OUT,
		.SGN = E_IM_IIP_PARAM_LUTSGN_UNSIGN,
		.SPLBIT = 0,
	},
	.LUTPRECAL_E.bit = {
		.SHIFT = 0,
		.OFSET = 0,
		.CLPMAX = 0x3fff,
		.THMAX = 0x3fff,
		.CLPMIN = 0x4000,
		.THMIN = 0x4000,
		.ABS = 1,
		.LUTADRS = 0,
	},
	.LUTCTL_F.bit = {
		.LUTBIT = 8,
		.USE = E_IM_IIP_PARAM_LUTUSE_LUT_OUT,
		.SGN = E_IM_IIP_PARAM_LUTSGN_UNSIGN,
		.SPLBIT = 0,
	},
	.LUTPRECAL_F.bit = {
		.SHIFT = 0,
		.OFSET = 0,
		.CLPMAX = 0x3fff,
		.THMAX = 0x3fff,
		.CLPMIN = 0x4000,
		.THMIN = 0x4000,
		.ABS = 1,
		.LUTADRS = 0,
	},
};
#endif //!CO_CT_IM_IIP_5_1_1_DISABLE_LUT && !CO_CT_IM_IIP_5_1_2_DISABLE_LUT

#ifdef __GNUC__
static T_IM_IIP_PARAM_LUT			g_ct_im_iip_unit_param_lut __attribute__((aligned(8)));		// Needs 8bytes align
#else
static __align(8) T_IM_IIP_PARAM_LUT			g_ct_im_iip_unit_param_lut;								// Needs 8bytes align
#endif

#if !defined(CO_CT_IM_IIP_5_1_1_DISABLE_LUT)
static const USHORT g_ct_im_iip_lutram[256] = {
		   0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,   15,	//    0
		  16,   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31,	//   16
		  32,   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,	//   32
		  48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63,	//   48
		  64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79,	//   64
		  80,   81,   82,   83,   84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95,	//   80
		  96,   97,   98,   99,  100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111,	//   96
		 112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127,	//  112
		 128,  129,  130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  140,  141,  142,  143,	//  128
		 144,  145,  146,  147,  148,  149,  150,  151,  152,  153,  154,  155,  156,  157,  158,  159,	//  144
		 160,  161,  162,  163,  164,  165,  166,  167,  168,  169,  170,  171,  172,  173,  174,  175,	//  160
		 176,  177,  178,  179,  180,  181,  182,  183,  184,  185,  186,  187,  188,  189,  190,  191,	//  176
		 192,  193,  194,  195,  196,  197,  198,  199,  200,  201,  202,  203,  204,  205,  206,  207,	//  192
		 208,  209,  210,  211,  212,  213,  214,  215,  216,  217,  218,  219,  220,  221,  222,  223,	//  208
		 224,  225,  226,  227,  228,  229,  230,  231,  232,  233,  234,  235,  236,  237,  238,  239,	//  224
		 240,  241,  242,  243,  244,  245,  246,  247,  248,  249,  250,  251,  252,  253,  254,  255,	//  240
};
#endif //!CO_CT_IM_IIP_5_1_1_DISABLE_LUT

#ifndef CO_CT_IM_IIP_5_1_2_DISABLE_LUT
static const USHORT g_ct_im_iip_lutram_invert[256] = {
		 255,  254,  253,  252,  251,  250,  249,  248,  247,  246,  245,  244,  243,  242,  241,  240,	//    0
		 239,  238,  237,  236,  235,  234,  233,  232,  231,  230,  229,  228,  227,  226,  225,  224,	//   16
		 223,  222,  221,  220,  219,  218,  217,  216,  215,  214,  213,  212,  211,  210,  209,  208,	//   32
		 207,  206,  205,  204,  203,  202,  201,  200,  199,  198,  197,  196,  195,  194,  193,  192,	//   48
		 191,  190,  189,  188,  187,  186,  185,  184,  183,  182,  181,  180,  179,  178,  177,  176,	//   64
		 175,  174,  173,  172,  171,  170,  169,  168,  167,  166,  165,  164,  163,  162,  161,  160,	//   80
		 159,  158,  157,  156,  155,  154,  153,  152,  151,  150,  149,  148,  147,  146,  145,  144,	//   96
		 143,  142,  141,  140,  139,  138,  137,  136,  135,  134,  133,  132,  131,  130,  129,  128,	//  112
		 127,  126,  125,  124,  123,  122,  121,  120,  119,  118,  117,  116,  115,  114,  113,  112,	//  128
		 111,  110,  109,  108,  107,  106,  105,  104,  103,  102,  101,  100,   99,   98,   97,   96,	//  144
		  95,   94,   93,   92,   91,   90,   89,   88,   87,   86,   85,   84,   83,   82,   81,   80,	//  160
		  79,   78,   77,   76,   75,   74,   73,   72,   71,   70,   69,   68,   67,   66,   65,   64,	//  176
		  63,   62,   61,   60,   59,   58,   57,   56,   55,   54,   53,   52,   51,   50,   49,   48,	//  192
		  47,   46,   45,   44,   43,   42,   41,   40,   39,   38,   37,   36,   35,   34,   33,   32,	//  208
		  31,   30,   29,   28,   27,   26,   25,   24,   23,   22,   21,   20,   19,   18,   17,   16,	//  224
		  15,   14,   13,   12,   11,   10,    9,    8,    7,    6,    5,    4,    3,    2,    1,    0,	//  240
};
#endif //!CO_CT_IM_IIP_5_1_2_DISABLE_LUT


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

// LUT Unit 5-1-1 YCC422 U8 VGA->VGA
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_5_1_1: "
INT32 CT_Im_IIP_5_1_1( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0;		// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg;
#ifndef CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	T_IM_IIP_UNIT_CFG				lut_cfg;
#endif //CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	T_IM_IIP_PARAM_1DL*				oned_unit_inf = &g_ct_im_iip_unit_param_1dl0;
	T_IM_IIP_PARAM_LUT*				lut_unit_inf = &g_ct_im_iip_unit_param_lut;
	T_IM_IIP_PARAM_STS*				sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG				sl_cfg;
#ifndef CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	T_IM_IIP_LUT					lut_ctrl;
#endif //CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END;
#ifndef CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_LUT | D_IM_IIP_PARAM_PLDUNIT_SL2;
#else //CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_SL2;
#endif //CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID			src_unitid = E_IM_IIP_UNIT_ID_LD1;
#ifndef CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	const E_IM_IIP_UNIT_ID			filter_unitid = E_IM_IIP_UNIT_ID_LUT;
#endif //CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	const E_IM_IIP_UNIT_ID			dst_unitid = E_IM_IIP_UNIT_ID_SL2;
	const E_IM_IIP_PARAM_PORTID		src_portid = E_IM_IIP_PARAM_PORTID_LD1;
#ifndef CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	const E_IM_IIP_PARAM_PORTID		filter_portid = E_IM_IIP_PARAM_PORTID_LUT_P0;
#endif //CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	const E_IM_IIP_PARAM_PORTID		dst_portid = E_IM_IIP_PARAM_PORTID_SL2;
	const UINT32					src_pixid = 4;
	const UINT32					dst_pixid = 5;
#ifndef CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0 | E_IM_IIP_OPEN_RES_LUT_A | E_IM_IIP_OPEN_RES_LUT_B | E_IM_IIP_OPEN_RES_LUT_C | E_IM_IIP_OPEN_RES_LUT_D | E_IM_IIP_OPEN_RES_LUT_E | E_IM_IIP_OPEN_RES_LUT_F;
#else //CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;
#endif //CO_CT_IM_IIP_5_1_1_DISABLE_LUT


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y;
	pixfmt_tbl_0.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A;

#ifndef CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	Ddim_Print(( D_IM_IIP_FUNC_NAME "lutram = 0x%x 0x%x\n", (UINT32)g_ct_im_iip_lutram, sizeof(g_ct_im_iip_lutram) ));

	lut_ctrl.buffer_bytes = sizeof(g_ct_im_iip_lutram);
	lut_ctrl.buffer_addr = g_ct_im_iip_lutram;
	lut_ctrl.lutram_type = E_IM_IIP_LUTRAM_TYPE_A;
	ercd = Im_IIP_Ctrl_LUT( &lut_ctrl );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x lutram_type=%u\n", ercd, lut_ctrl.lutram_type ));
	lut_ctrl.lutram_type = E_IM_IIP_LUTRAM_TYPE_B;
	ercd = Im_IIP_Ctrl_LUT( &lut_ctrl );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x lutram_type=%u\n", ercd, lut_ctrl.lutram_type ));
	lut_ctrl.lutram_type = E_IM_IIP_LUTRAM_TYPE_C;
	ercd = Im_IIP_Ctrl_LUT( &lut_ctrl );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x lutram_type=%u\n", ercd, lut_ctrl.lutram_type ));
#endif //CO_CT_IM_IIP_5_1_1_DISABLE_LUT

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf = 0x%x 0x%x\n", (UINT32)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf, '\0', sizeof(*oned_unit_inf) );
	*oned_unit_inf = g_ct_im_iip_param_1dl_base;
#ifndef CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	oned_unit_inf->LD_TOPCNF0.bit.WAITCONF = filter_portid;
#else //CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	oned_unit_inf->LD_TOPCNF0.bit.WAITCONF = dst_portid;
#endif //CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	oned_unit_inf->PIXIDDEF.bit.IPIXID = src_pixid;

	oned_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg.unit_param_addr = (ULONG)oned_unit_inf;
	oned_cfg.load_unit_param_flag = 0;

#ifndef CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	Ddim_Print(( D_IM_IIP_FUNC_NAME "lut_unit_inf = 0x%x 0x%x\n", (UINT32)lut_unit_inf, sizeof(T_IM_IIP_PARAM_LUT) ));

	memset( lut_unit_inf, '\0', sizeof(*lut_unit_inf) );
	*lut_unit_inf = g_ct_im_iip_param_lut_base;
	lut_unit_inf->LUTTOPCNF[0].bit.DATACONF = src_portid;
	lut_unit_inf->LUTTOPCNF[0].bit.WAITCONF = dst_portid;

	lut_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	lut_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	lut_cfg.unit_param_addr = (ULONG)lut_unit_inf;
	lut_cfg.load_unit_param_flag = 0;
#endif //CO_CT_IM_IIP_5_1_1_DISABLE_LUT

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
#ifndef CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = filter_portid;
#else //CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = src_portid;
#endif //CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)lut_unit_inf, sizeof(T_IM_IIP_PARAM_LUT) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#if 0
	Palladium_Set_Out_Localstack( (ULONG)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) );
	Palladium_Set_Out_Localstack( (ULONG)lut_unit_inf, sizeof(T_IM_IIP_PARAM_LUT) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &oned_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

#ifndef CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	ercd = Im_IIP_Ctrl_SWTRG_Unit( filter_unitid, &lut_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
#endif //CO_CT_IM_IIP_5_1_1_DISABLE_LUT

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	Ddim_Print(( "ONED[1]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_LD1.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_LD1.UNITINFTBL0.bit.HWEN
			   ));
#ifndef CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	Ddim_Print(( "LUT: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_LUT.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_LUT.UNITINFTBL0.bit.HWEN
			   ));
#endif //CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	Ddim_Print(( "SL[2]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

	Dd_ARM_Dmb_Pou();

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

#ifndef CO_CT_IM_IIP_5_1_1_DISABLE_LUT
	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( filter_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
#endif //CO_CT_IM_IIP_5_1_1_DISABLE_LUT

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start3\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

#if 0
	for( UINT32 loopcnt = 0; loopcnt < 10; loopcnt++ ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME " SLALE=%u INTIZ0=0x%lx INTIZ1=0x%lx\n",
					IO_IIP.IZACTL1.bit.SLALE,
					IO_IIP.INTIZ0.word,
					IO_IIP.INTIZ1.word
				   ));
		DDIM_User_Dly_Tsk( 1 );
	}
#endif

	Ddim_Print(( D_IM_IIP_FUNC_NAME "WaitEnd\n" ));
	ercd = Im_IIP_Wait_End( &wait_factor_result, wait_factor, D_IM_IIP_OR_WAIT, 30 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	if( ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME "Stop\n" ));
		ercd = Im_IIP_Stop( D_IM_IIP_ABORT );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	}

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#if 1
	// UnitINFダンプ
	Palladium_Set_Out_Localstack( (ULONG)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) );
	Palladium_Set_Out_Localstack( (ULONG)lut_unit_inf, sizeof(T_IM_IIP_PARAM_LUT) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif


#if 1
	// dump UNITINF ONED[1]
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( src_unitid, gCtImIIP_Get_Unit_Param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF ONED[1] %u\n", param_bytes ));
			ct_im_iip_clean_l1l2_dcache_addr( (ULONG)gCtImIIP_Get_Unit_Param, param_bytes );
			Palladium_Set_Out_Localstack( (ULONG)gCtImIIP_Get_Unit_Param, param_bytes );
		}
	}
#endif


#if 0
	// dump UNITINF LUT
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( filter_unitid, gCtImIIP_Get_Unit_Param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF LUT %u\n", param_bytes ));
			ct_im_iip_clean_l1l2_dcache_addr( (ULONG)gCtImIIP_Get_Unit_Param, param_bytes );
			Palladium_Set_Out_Localstack( (ULONG)gCtImIIP_Get_Unit_Param, param_bytes );
		}
	}
#endif


#if 1
	// dump UNITINF SL[2]
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( dst_unitid, gCtImIIP_Get_Unit_Param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF SL[2] %u\n", param_bytes ));
			ct_im_iip_clean_l1l2_dcache_addr( (ULONG)gCtImIIP_Get_Unit_Param, param_bytes );
			Palladium_Set_Out_Localstack( (ULONG)gCtImIIP_Get_Unit_Param, param_bytes );
		}
	}
#endif


#if 0
#ifndef CO_DEBUG_ON_PC
	// dump IO_IIP
	{
		ULONG regdump_addr = D_IM_IIP_REGDUMP_ADDR;
		ULONG regdump_bytes = sizeof(IO_IIP);
		Ddim_Print(( D_IM_IIP_FUNC_NAME "dump IO_IIP %u\n", regdump_bytes ));
		memcpy( (VOID*)regdump_addr, (VOID*)&IO_IIP, regdump_bytes );
		Palladium_Set_Out_Localstack( regdump_addr, regdump_bytes );
	}
#endif //!CO_DEBUG_ON_PC
#endif
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Im_IIP_Close_SWTRG\n" ));
	ercd = Im_IIP_Close_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


// LUT Unit 5-1-2 YCC422 U8 VGA->VGA pixval is inverting
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_5_1_2: "
INT32 CT_Im_IIP_5_1_2( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0;		// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg;
	T_IM_IIP_UNIT_CFG				lut_cfg;
	T_IM_IIP_PARAM_1DL*				oned_unit_inf = &g_ct_im_iip_unit_param_1dl0;
#ifndef CO_CT_IM_IIP_5_1_2_DISABLE_LUT
	T_IM_IIP_PARAM_LUT*				lut_unit_inf = &g_ct_im_iip_unit_param_lut;
#endif //CO_CT_IM_IIP_5_1_2_DISABLE_LUT
	T_IM_IIP_PARAM_STS*				sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG				sl_cfg;
#ifndef CO_CT_IM_IIP_5_1_2_DISABLE_LUT
	T_IM_IIP_LUT					lut_ctrl;
#endif //CO_CT_IM_IIP_5_1_2_DISABLE_LUT
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END;
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_LUT | D_IM_IIP_PARAM_PLDUNIT_SL2;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID			src_unitid = E_IM_IIP_UNIT_ID_LD1;
	const E_IM_IIP_UNIT_ID			filter_unitid = E_IM_IIP_UNIT_ID_LUT;
	const E_IM_IIP_UNIT_ID			dst_unitid = E_IM_IIP_UNIT_ID_SL2;
	const E_IM_IIP_PARAM_PORTID		filter_portid = E_IM_IIP_PARAM_PORTID_LUT_P0;
#ifndef CO_CT_IM_IIP_5_1_2_DISABLE_LUT
	const E_IM_IIP_PARAM_PORTID		src_portid = E_IM_IIP_PARAM_PORTID_LD1;
	const E_IM_IIP_PARAM_PORTID		dst_portid = E_IM_IIP_PARAM_PORTID_SL2;
#endif //CO_CT_IM_IIP_5_1_2_DISABLE_LUT
	const UINT32					src_pixid = 4;
	const UINT32					dst_pixid = 5;
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0 | E_IM_IIP_OPEN_RES_LUT_A | E_IM_IIP_OPEN_RES_LUT_B | E_IM_IIP_OPEN_RES_LUT_C;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y;
	pixfmt_tbl_0.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A;

#ifndef CO_CT_IM_IIP_5_1_2_DISABLE_LUT
	Ddim_Print(( D_IM_IIP_FUNC_NAME "lutram = 0x%x 0x%x\n", (UINT32)g_ct_im_iip_lutram_invert, sizeof(g_ct_im_iip_lutram_invert) ));

	lut_ctrl.buffer_bytes = sizeof(g_ct_im_iip_lutram_invert);
	lut_ctrl.buffer_addr = g_ct_im_iip_lutram_invert;
	lut_ctrl.lutram_type = E_IM_IIP_LUTRAM_TYPE_A;
	ercd = Im_IIP_Ctrl_LUT( &lut_ctrl );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x lutram_type=%u\n", ercd, lut_ctrl.lutram_type ));
	lut_ctrl.lutram_type = E_IM_IIP_LUTRAM_TYPE_B;
	ercd = Im_IIP_Ctrl_LUT( &lut_ctrl );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x lutram_type=%u\n", ercd, lut_ctrl.lutram_type ));
	lut_ctrl.lutram_type = E_IM_IIP_LUTRAM_TYPE_C;
	ercd = Im_IIP_Ctrl_LUT( &lut_ctrl );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x lutram_type=%u\n", ercd, lut_ctrl.lutram_type ));
#endif //CO_CT_IM_IIP_5_1_2_DISABLE_LUT

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf = 0x%x 0x%x\n", (UINT32)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf, '\0', sizeof(*oned_unit_inf) );
	*oned_unit_inf = g_ct_im_iip_param_1dl_base;
	oned_unit_inf->LD_TOPCNF0.bit.WAITCONF = filter_portid;
	oned_unit_inf->PIXIDDEF.bit.IPIXID = src_pixid;

	oned_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg.unit_param_addr = (ULONG)oned_unit_inf;
	oned_cfg.load_unit_param_flag = 0;

#ifndef CO_CT_IM_IIP_5_1_2_DISABLE_LUT
	Ddim_Print(( D_IM_IIP_FUNC_NAME "lut_unit_inf = 0x%x 0x%x\n", (UINT32)lut_unit_inf, sizeof(T_IM_IIP_PARAM_LUT) ));

	memset( lut_unit_inf, '\0', sizeof(*lut_unit_inf) );
	*lut_unit_inf = g_ct_im_iip_param_lut_base;
	lut_unit_inf->LUTTOPCNF[0].bit.DATACONF = src_portid;
	lut_unit_inf->LUTTOPCNF[0].bit.WAITCONF = dst_portid;

	lut_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	lut_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	lut_cfg.unit_param_addr = (ULONG)lut_unit_inf;
	lut_cfg.load_unit_param_flag = 0;
#endif //CO_CT_IM_IIP_5_1_2_DISABLE_LUT

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = filter_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) );
#ifndef CO_CT_IM_IIP_5_1_2_DISABLE_LUT
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)lut_unit_inf, sizeof(T_IM_IIP_PARAM_LUT) );
#endif //CO_CT_IM_IIP_5_1_2_DISABLE_LUT
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#if 0
	Palladium_Set_Out_Localstack( (ULONG)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) );
	Palladium_Set_Out_Localstack( (ULONG)lut_unit_inf, sizeof(T_IM_IIP_PARAM_LUT) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &oned_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( filter_unitid, &lut_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	Ddim_Print(( "ONED[1]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_LD1.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_LD1.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "LUT: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_LUT.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_LUT.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "SL[2]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

	Dd_ARM_Dmb_Pou();

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( filter_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start3\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

#if 0
	for( UINT32 loopcnt = 0; loopcnt < 10; loopcnt++ ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME " SLALE=%u INTIZ0=0x%lx INTIZ1=0x%lx\n",
					IO_IIP.IZACTL1.bit.SLALE,
					IO_IIP.INTIZ0.word,
					IO_IIP.INTIZ1.word
				   ));
		DDIM_User_Dly_Tsk( 1 );
	}
#endif

	Ddim_Print(( D_IM_IIP_FUNC_NAME "WaitEnd\n" ));
	ercd = Im_IIP_Wait_End( &wait_factor_result, wait_factor, D_IM_IIP_OR_WAIT, 30 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	if( ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME "Stop\n" ));
		ercd = Im_IIP_Stop( D_IM_IIP_ABORT );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	}

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#if 0
	// UnitINFダンプ
	Palladium_Set_Out_Localstack( (ULONG)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) );
	Palladium_Set_Out_Localstack( (ULONG)lut_unit_inf, sizeof(T_IM_IIP_PARAM_LUT) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif


#if 0
	// dump UNITINF ONED[1]
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( src_unitid, gCtImIIP_Get_Unit_Param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF ONED[1] %u\n", param_bytes ));
			ct_im_iip_clean_l1l2_dcache_addr( (ULONG)gCtImIIP_Get_Unit_Param, param_bytes );
			Palladium_Set_Out_Localstack( (ULONG)gCtImIIP_Get_Unit_Param, param_bytes );
		}
	}
#endif


#if 0
	// dump UNITINF LUT
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( filter_unitid, gCtImIIP_Get_Unit_Param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF ONED[1] %u\n", param_bytes ));
			ct_im_iip_clean_l1l2_dcache_addr( (ULONG)gCtImIIP_Get_Unit_Param, param_bytes );
			Palladium_Set_Out_Localstack( (ULONG)gCtImIIP_Get_Unit_Param, param_bytes );
		}
	}
#endif


#if 0
	// dump UNITINF SL[2]
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( dst_unitid, gCtImIIP_Get_Unit_Param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF SL[2] %u\n", param_bytes ));
			ct_im_iip_clean_l1l2_dcache_addr( (ULONG)gCtImIIP_Get_Unit_Param, param_bytes );
			Palladium_Set_Out_Localstack( (ULONG)gCtImIIP_Get_Unit_Param, param_bytes );
		}
	}
#endif


#if 0
#ifndef CO_DEBUG_ON_PC
	// dump IO_IIP
	{
		ULONG regdump_addr = D_IM_IIP_REGDUMP_ADDR;
		ULONG regdump_bytes = sizeof(IO_IIP);
		Ddim_Print(( D_IM_IIP_FUNC_NAME "dump IO_IIP %u\n", regdump_bytes ));
		memcpy( (VOID*)regdump_addr, (VOID*)&IO_IIP, regdump_bytes );
		Palladium_Set_Out_Localstack( regdump_addr, regdump_bytes );
	}
#endif //!CO_DEBUG_ON_PC
#endif
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Im_IIP_Close_SWTRG\n" ));
	ercd = Im_IIP_Close_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


// LUT Unit 5-1-X
INT32 CT_Im_IIP_Run_5_1( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-5-1-1
		case 1:
			return CT_Im_IIP_5_1_1();

		// Test-5-1-2
		case 2:
			return CT_Im_IIP_5_1_2();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#endif //CO_CT_IM_IIP_DISABLE

