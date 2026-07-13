/**
 * @file		ct_im_iip_sl.c
 * @brief		IIP STORE unit ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// CT header
#include "ct_im_iip.h"
#include "ct_im_iip_sl.h"

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

static const T_IM_IIP_PARAM_STS_FILL g_ct_im_iip_param_sts_fill_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 26,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY,
	.BASE = {
		.SL_TOPCNF0.bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE,
//		.SL_TOPCNF0.bit.DATACONF,		// set later
//		.PIXIDDEF.bit.OPIXID,			// set later
		.PIXIDDEF.bit.CSEL = E_IM_IIP_PARAM_CSEL_0,	// dummy
		.PIXIDDEF.bit.DIV1ST = E_IM_IIP_PARAM_DIV1ST_1ST,
		.PIXIDDEF.bit.LKDIV = 0,

		.PLDUNIT_SL.bit.PLDUNIT_SL_LO = Im_IIP_Get_PLDUNIT_Lo( D_IM_IIP_PARAM_PLDUNIT_NONE ),
		.PLDUNIT_SL.bit.PLDUNIT_SL_HI = Im_IIP_Get_PLDUNIT_Hi( D_IM_IIP_PARAM_PLDUNIT_NONE ),

		.SL_CTL.bit.SL_MODE = 0,
		.SL_CTL.bit.LOADMD = 0,
		.SL_CTL.bit.MASKEN = E_IM_IIP_PARAM_MASKEN_NO_MASK,

		.OUTCOL.bit.OUTCOL = 0,

		.SL_HSIZE.bit.HSTART = 0,
		.SL_HSIZE.bit.HEND = D_IM_IIP_VGA_WIDTH - 1,
		.SL_VSIZE.bit.VSTART = 0,
		.SL_VSIZE.bit.VEND = D_IM_IIP_VGA_LINES - 1,
		.SL_PHSZ.bit.PHSZ1 =  64,
		.SL_PVSZ.bit.PVSZ1 =  8,
		.SL_PHSZ.bit.PHSZ0 = 64,
		.SL_PVSZ.bit.PVSZ0 = 8,

		.SEL_OPARA.bit.SEL_OPARA = 0,

		.OFSET_0.bit.OFSET = 0,
		.CLPTH_U_0.bit.CLPTH_U = 0x3FFF,
		.CLPTH_L_0.bit.CLPTH_L = -0x4000,
		.CLPVAL_U_0.bit.CLPVAL_U = 0x3FFF,
		.CLPVAL_L_0.bit.CLPVAL_L = -0x4000,
		.SFT_0.bit.SFTVAL = 0,
		.SFT_0.bit.SFTM = E_IM_IIP_PARAM_SFTM_ROUNDDOWN,
		.W14MD_0.bit.W14LSFT = 0,
		.W14MD_0.bit.W14SAT = 0,
	},

	.header1.bit.BeginningAddress = 0x0200,
	.header1.bit.WordLength = 3,
	.header1.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_LOAD_ONLY,
	.FILL_EN.bit.FILL_EN0 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN1 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN2 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN3 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN4 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN5 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN6 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN7 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN8 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN9 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN10 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN11 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN12 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN13 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN14 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN15 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN16 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN17 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN18 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_EN.bit.FILL_EN19 = E_IM_IIP_PARAM_FILL_EN_ENABLE,
	.FILL_DATA.bit.FILL_D0 = 128,
	.FILL_DATA.bit.FILL_D1 = 253,
	.FILL_DATA.bit.FILL_D2 = 2,

	.header2.bit.BeginningAddress = 0x0300,
	.header2.bit.WordLength = 41,
	.header2.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
	.FILL_SIZE = {
		 [0] = { .bit = { .FILL_HSTA =  16, .FILL_HEND =  25, .FILL_VSTA =  10, .FILL_VEND =  19, } },
		 [1] = { .bit = { .FILL_HSTA =  28, .FILL_HEND =  37, .FILL_VSTA =  12, .FILL_VEND =  21, } },
		 [2] = { .bit = { .FILL_HSTA =  40, .FILL_HEND =  49, .FILL_VSTA =  14, .FILL_VEND =  23, } },
		 [3] = { .bit = { .FILL_HSTA =  52, .FILL_HEND =  61, .FILL_VSTA =  16, .FILL_VEND =  25, } },

		 [4] = { .bit = { .FILL_HSTA =  24, .FILL_HEND =  33, .FILL_VSTA =  30, .FILL_VEND =  39, } },
		 [5] = { .bit = { .FILL_HSTA =  36, .FILL_HEND =  45, .FILL_VSTA =  32, .FILL_VEND =  41, } },
		 [6] = { .bit = { .FILL_HSTA =  48, .FILL_HEND =  57, .FILL_VSTA =  34, .FILL_VEND =  43, } },
		 [7] = { .bit = { .FILL_HSTA =  60, .FILL_HEND =  69, .FILL_VSTA =  36, .FILL_VEND =  45, } },

		 [8] = { .bit = { .FILL_HSTA =  32, .FILL_HEND =  41, .FILL_VSTA =  50, .FILL_VEND =  59, } },
		 [9] = { .bit = { .FILL_HSTA =  44, .FILL_HEND =  53, .FILL_VSTA =  52, .FILL_VEND =  61, } },
		[10] = { .bit = { .FILL_HSTA =  56, .FILL_HEND =  65, .FILL_VSTA =  54, .FILL_VEND =  63, } },
		[11] = { .bit = { .FILL_HSTA =  68, .FILL_HEND =  77, .FILL_VSTA =  56, .FILL_VEND =  65, } },

		[12] = { .bit = { .FILL_HSTA =  80, .FILL_HEND =  89, .FILL_VSTA =  70, .FILL_VEND =  79, } },
		[13] = { .bit = { .FILL_HSTA =  92, .FILL_HEND = 101, .FILL_VSTA =  72, .FILL_VEND =  81, } },
		[14] = { .bit = { .FILL_HSTA = 104, .FILL_HEND = 113, .FILL_VSTA =  74, .FILL_VEND =  83, } },
		[15] = { .bit = { .FILL_HSTA = 116, .FILL_HEND = 125, .FILL_VSTA =  76, .FILL_VEND =  85, } },

		[16] = { .bit = { .FILL_HSTA =   0, .FILL_HEND =  19, .FILL_VSTA =   0, .FILL_VEND =  19, } },	// left-top corner
		[17] = { .bit = { .FILL_HSTA =  34, .FILL_HEND =  43, .FILL_VSTA =   0, .FILL_VEND =  19, } },	// top side
		[18] = { .bit = { .FILL_HSTA = 630, .FILL_HEND = 639, .FILL_VSTA = 240, .FILL_VEND = 249, } },	// right side
		[19] = { .bit = { .FILL_HSTA = 320, .FILL_HEND = 339, .FILL_VSTA = 470, .FILL_VEND = 479, } },	// bottom side
	},
};

#ifdef __GNUC__
static T_IM_IIP_PARAM_STS_FILL	g_ct_im_iip_unit_param_sl_fill __attribute__((aligned(8)));		// Needs 8bytes align
#else
__align(8) static T_IM_IIP_PARAM_STS_FILL	g_ct_im_iip_unit_param_sl_fill;							// Needs 8bytes align
#endif

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

// SL Unit 10-1-1
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_10_1_1: "
INT32 CT_Im_IIP_10_1_1( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0;		// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;		// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_2;		// SL Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg[2];
	T_IM_IIP_UNIT_CFG				blend_cfg;
	T_IM_IIP_PARAM_1DL*				oned_unit_inf[2] = { &g_ct_im_iip_unit_param_1dl0, &g_ct_im_iip_unit_param_1dl1 };
	T_IM_IIP_PARAM_BLEND*			blend_unit_inf = &g_ct_im_iip_unit_param_blend;
	T_IM_IIP_PARAM_STS*				sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG				sl_cfg;
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL6END;
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_LD2 | D_IM_IIP_PARAM_PLDUNIT_BLEND0 | D_IM_IIP_PARAM_PLDUNIT_SL6;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_9 | E_IM_IIP_PIXID_10 | E_IM_IIP_PIXID_11;
	const E_IM_IIP_UNIT_ID			src_unitid[2] = { E_IM_IIP_UNIT_ID_LD1, E_IM_IIP_UNIT_ID_LD2 };
	const E_IM_IIP_UNIT_ID			filter1_unitid = E_IM_IIP_UNIT_ID_BLEND0;
	const E_IM_IIP_UNIT_ID			dst_unitid = E_IM_IIP_UNIT_ID_SL6;
	const E_IM_IIP_PARAM_PORTID		src_portid[2] = { E_IM_IIP_PARAM_PORTID_LD1, E_IM_IIP_PARAM_PORTID_LD2 };
	const E_IM_IIP_PARAM_PORTID		filter1_portid[2] = { E_IM_IIP_PARAM_PORTID_BLEND0_P0, E_IM_IIP_PARAM_PORTID_BLEND0_P1 };
	const E_IM_IIP_PARAM_PORTID		dst_portid = E_IM_IIP_PARAM_PORTID_SL6;
	const UINT32					src_pixid[2] = { 9, 10 };
	const UINT32					dst_pixid = 11;
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE1;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0.line_bytes.Y_G = D_IM_IIP_VGA_RGB565_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cb_B = 0;	// dummy
	pixfmt_tbl_0.line_bytes.Cr_R = 0;	// dummy
	pixfmt_tbl_0.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_RGB565;
	pixfmt_tbl_0.addr.Cb_B = 0;		// dummy
	pixfmt_tbl_0.addr.Cr_R = 0;		// dummy
	pixfmt_tbl_0.addr.Alpha = 0;	// dummy
	pixfmt_tbl_0.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.masterIF_Alpha = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.pix_format = E_IM_IIP_PFMT_RGB565;


	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_RGB565_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = 0;	// dummy
	pixfmt_tbl_1.line_bytes.Cr_R = 0;	// dummy
	pixfmt_tbl_1.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_4_RGB565;
	pixfmt_tbl_1.addr.Cb_B = 0;		// dummy
	pixfmt_tbl_1.addr.Cr_R = 0;		// dummy
	pixfmt_tbl_1.addr.Alpha = 0;	// dummy
	pixfmt_tbl_1.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.masterIF_Alpha = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.pix_format = E_IM_IIP_PFMT_RGB565;


	pixfmt_tbl_2 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_2.line_bytes.Y_G = D_IM_IIP_VGA_RGB565_GLOBAL_WIDTH;
	pixfmt_tbl_2.line_bytes.Cb_B = 0;	// dummy
	pixfmt_tbl_2.line_bytes.Cr_R = 0;	// dummy
	pixfmt_tbl_2.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_2.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_RGB565;
	pixfmt_tbl_2.addr.Cb_B = 0;		// dummy
	pixfmt_tbl_2.addr.Cr_R = 0;		// dummy
	pixfmt_tbl_2.addr.Alpha = 0;	// dummy
	pixfmt_tbl_2.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_2.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_2.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_2.masterIF_Alpha = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_2.pix_format = E_IM_IIP_PFMT_RGB565;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf[0] = 0x%x 0x%x\n", (UINT32)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[0], '\0', sizeof(*oned_unit_inf[0]) );
	*oned_unit_inf[0] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[0]->LD_TOPCNF0.bit.WAITCONF = filter1_portid[0];
	oned_unit_inf[0]->PIXIDDEF.bit.IPIXID = src_pixid[0];
	oned_unit_inf[0]->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC;
	oned_unit_inf[0]->LD_CTL.bit.CSEL = E_IM_IIP_PARAM_CSEL_1;

	oned_cfg[0].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[0].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[0].unit_param_addr = (ULONG)oned_unit_inf[0];
	oned_cfg[0].load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf[1] = 0x%x 0x%x\n", (UINT32)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[1], '\0', sizeof(*oned_unit_inf[1]) );
	*oned_unit_inf[1] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[1]->LD_TOPCNF0.bit.WAITCONF = filter1_portid[1];
	oned_unit_inf[1]->PIXIDDEF.bit.IPIXID = src_pixid[1];
	oned_unit_inf[1]->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC;
	oned_unit_inf[1]->LD_CTL.bit.CSEL = E_IM_IIP_PARAM_CSEL_1;

	oned_cfg[1].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[1].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[1].unit_param_addr = (ULONG)oned_unit_inf[1];
	oned_cfg[1].load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "blend_unit_inf = 0x%x 0x%x\n", (UINT32)blend_unit_inf, sizeof(T_IM_IIP_PARAM_BLEND) ));

	memset( blend_unit_inf, '\0', sizeof(*blend_unit_inf) );
	*blend_unit_inf = g_ct_im_iip_param_blend_base;
	blend_unit_inf->BLENDTOPCNF0.bit.WAITCONF_0 = dst_portid;
	blend_unit_inf->BLENDTOPCNF0.bit.DATACONF_0 = src_portid[0];
	blend_unit_inf->BLENDTOPCNF1.bit.DATACONF_1 = src_portid[1];

	blend_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	blend_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	blend_cfg.unit_param_addr = (ULONG)blend_unit_inf;
	blend_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = filter1_portid[0];
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;
	sl_unit_inf->BASE.CLPTH_U_0.bit.CLPTH_U = 32;
	sl_unit_inf->BASE.CLPTH_L_0.bit.CLPTH_L = 0;
	sl_unit_inf->BASE.CLPVAL_U_0.bit.CLPVAL_U = 32;
	sl_unit_inf->BASE.CLPVAL_L_0.bit.CLPVAL_L = 0;

	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)blend_unit_inf, sizeof(T_IM_IIP_PARAM_BLEND) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );


	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[0], &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[1], &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_2 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[0], &oned_cfg[0] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[1], &oned_cfg[1] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( filter1_unitid, &blend_cfg );
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
	Ddim_Print(( "ONED[2]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_LD2.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_LD2.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "BLEND[0]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_BLD0.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_BLD0.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "SL[6]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL6.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL6.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL6.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL6.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

	CT_Im_IIP_Print_Unitcfg_Byid( src_unitid[0] );
	CT_Im_IIP_Print_Unitcfg_Byid( src_unitid[1] );
	CT_Im_IIP_Print_Unitcfg_Byid( filter1_unitid );
	CT_Im_IIP_Print_Unitcfg_Byid( dst_unitid );
	CT_Im_IIP_Print_Pixfmttbl_Detail( src_pixid[0] );
	CT_Im_IIP_Print_Pixfmttbl_Detail( src_pixid[1] );
	CT_Im_IIP_Print_Pixfmttbl_Detail( dst_pixid );

	Dd_ARM_Dmb_Pou();

	// Need start sequence Store->CSC->1DL->SPF or Store->1DL->CSC->SPF
	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( filter1_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start3\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid[0], D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start4\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid[1], D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));


	Ddim_Print(( D_IM_IIP_FUNC_NAME "WaitEnd\n" ));
	ercd = Im_IIP_Wait_End( &wait_factor_result, wait_factor, D_IM_IIP_OR_WAIT, 30 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	if( ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME "Stop\n" ));
		ercd = Im_IIP_Stop( D_IM_IIP_ABORT );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	}


	Ddim_Print(( D_IM_IIP_FUNC_NAME "Im_IIP_Close_SWTRG\n" ));
	ercd = Im_IIP_Close_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


// SL Unit 10-1-2
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_10_1_2: "
INT32 CT_Im_IIP_10_1_2( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0;	// 1DL0 Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;	// SL2 Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg;
	T_IM_IIP_UNIT_CFG				sl_cfg;
	T_IM_IIP_PARAM_1DL*				oned_unit_inf = &g_ct_im_iip_unit_param_1dl0;
	T_IM_IIP_PARAM_STS_FILL*		sl_unit_inf = &g_ct_im_iip_unit_param_sl_fill;
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_3 | E_IM_IIP_PIXID_4;
	const E_IM_IIP_UNIT_ID			src_unitid = E_IM_IIP_UNIT_ID_LD0;
	const E_IM_IIP_PARAM_PORTID		src_portid = E_IM_IIP_PARAM_PORTID_LD0;
	const E_IM_IIP_UNIT_ID			dst_unitid = E_IM_IIP_UNIT_ID_SL2;
	const E_IM_IIP_PARAM_PORTID		dst_portid = E_IM_IIP_PARAM_PORTID_SL2;
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD0 | D_IM_IIP_PARAM_PLDUNIT_SL2;
	const UINT32					src_pixid = 3;
	const UINT32					dst_pixid = 4;
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


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
	pixfmt_tbl_1.pix_format = E_IM_IIP_PFMT_YCC420;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_YCC420_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_YCC420_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_YCC420_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = D_IM_IIP_VGA_YCC420_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_YCC420_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_YCC420_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_YCC420_U8_C;
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC420_U8_A;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf = 0x%x 0x%x\n", (UINT32)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf, '\0', sizeof(*oned_unit_inf) );
	*oned_unit_inf = g_ct_im_iip_param_1dl_base;
	oned_unit_inf->LD_TOPCNF0.bit.WAITCONF = dst_portid;
	oned_unit_inf->PIXIDDEF.bit.IPIXID = src_pixid;
	oned_unit_inf->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC;


	oned_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg.unit_param_addr = (ULONG)oned_unit_inf;
	oned_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS_FILL) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_fill_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;

	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS_FILL) );


	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &oned_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	CT_Im_IIP_Print_Unitcfg_Byid( src_unitid );
	CT_Im_IIP_Print_Unitcfg_Byid( dst_unitid );
	CT_Im_IIP_Print_Pixfmttbl_Detail( src_pixid );
	CT_Im_IIP_Print_Pixfmttbl_Detail( dst_pixid );

	Dd_ARM_Dmb_Pou();

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));


	Ddim_Print(( D_IM_IIP_FUNC_NAME "WaitEnd\n" ));
	ercd = Im_IIP_Wait_End( &wait_factor_result, wait_factor, D_IM_IIP_OR_WAIT, 30 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	if( ercd != D_DDIM_OK ) {
		Ddim_Print(( D_IM_IIP_FUNC_NAME "Stop\n" ));
		ercd = Im_IIP_Stop( D_IM_IIP_ABORT );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x factor=0x%x\n", ercd, wait_factor_result ));
	}


	Ddim_Print(( D_IM_IIP_FUNC_NAME "Im_IIP_Close_SWTRG\n" ));
	ercd = Im_IIP_Close_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


// SL Unit 10-1-X
INT32 CT_Im_IIP_Run_10_1( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-10-1-1
		case 1:
			return CT_Im_IIP_10_1_1();

		// Test-10-1-2
		case 2:
			return CT_Im_IIP_10_1_2();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#endif //CO_CT_IM_IIP_DISABLE

