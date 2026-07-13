/**
 * @file		ct_im_iip_cfl.c
 * @brief		IIP CFL unit ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// CT header
#include "ct_im_iip.h"
#include "ct_im_iip_cfl.h"

// im_iip header
#include "im_iip.h"

// MILB register header
#include "jdsiip.h"

// Memory barrier
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

static const T_IM_IIP_PARAM_CFLOW g_ct_im_iip_param_cfl_base = {
	.header0.bit.BeginningAddress = 0x0000,
	.header0.bit.WordLength = 9,
	.header0.bit.CtrlCode = E_IM_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET,
//	.CFTOPCONF.bit.DATACONF_0 = ,		// set later
//	.CFTOPCONF.bit.DATACONF_1 = ,		// set later
	.DIFF_PREWAIT.bit.DIFF_PREWAIT = 8192,		// About. (ImgCache 32kByte /4)
	.DIFF_POSTWAIT.bit.DIFF_POSTWAIT = 64 * 8,	// About. (PHSZ1 x PVSZ1)
	.MAGPOW.bit.PREMAG = 0x10,
	.MAGPOW.bit.POSTMAG = 0x10,
	.CFMODE.bit.SPC_ON = 0,
};
#ifdef __GNUC__
static T_IM_IIP_PARAM_CFLOW			g_ct_im_iip_unit_param_cfl __attribute__((aligned(8)));		// Needs 8bytes align
#else
static __align(8) T_IM_IIP_PARAM_CFLOW			g_ct_im_iip_unit_param_cfl;								// Needs 8bytes align
#endif

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

// CFL Unit 12-1-1
// LD(YCC422)->SL(S)->LD->SL(S)
// +----+    +----+      +----+    +----+
// |LD0 |----|SL0 |------|LD1 |----|SL3 |
// +----+    +----+      +----+    +----+
//           D=LD0                 D=LD1
// W=SL0     W=CFL0_0    W=SL3     W=CFL0_1
// pix=3     pix=4       pix=4     pix=5
//              |                     |
//              |        +----+       |
//              +--------|CFL0|-------+
//                       +----+
//                    D0=SL0 D1=SL3
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_12_1_1: "
INT32 CT_Im_IIP_12_1_1( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0;	// 1DL0 Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;	// SL0 Unit output / 1DL1 Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_2;	// SL3 Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg[2];
	T_IM_IIP_UNIT_CFG				sl_cfg[2];
	T_IM_IIP_UNIT_CFG				cfl_cfg;
	T_IM_IIP_PARAM_1DL*				oned_unit_inf[2] = { &g_ct_im_iip_unit_param_1dl0, &g_ct_im_iip_unit_param_1dl1 };
	T_IM_IIP_PARAM_STS*				sl_unit_inf[2] = { &g_ct_im_iip_unit_param_sl0, &g_ct_im_iip_unit_param_sl1 };
	T_IM_IIP_PARAM_CFLOW*			cfl_unit_inf = &g_ct_im_iip_unit_param_cfl;
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL3END;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_3 | E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID			src_unitid[2] = { E_IM_IIP_UNIT_ID_LD0, E_IM_IIP_UNIT_ID_LD1 };
	const E_IM_IIP_PARAM_PORTID		src_portid[2] = { E_IM_IIP_PARAM_PORTID_LD0, E_IM_IIP_PARAM_PORTID_LD1 };
	const E_IM_IIP_UNIT_ID			cfl_unitid    = E_IM_IIP_UNIT_ID_CFL0;
	const E_IM_IIP_PARAM_PORTID		cfl_portid[2] = { E_IM_IIP_PARAM_PORTID_CFL0_P0, E_IM_IIP_PARAM_PORTID_CFL0_P1 };
	const E_IM_IIP_UNIT_ID			dst_unitid[2] = { E_IM_IIP_UNIT_ID_SL2, E_IM_IIP_UNIT_ID_SL3 };
	const E_IM_IIP_PARAM_PORTID		dst_portid[2] = { E_IM_IIP_PARAM_PORTID_SL2, E_IM_IIP_PARAM_PORTID_SL3 };
	const UINT32					unitid_bitmask[2] = { D_IM_IIP_PARAM_PLDUNIT_LD0 | D_IM_IIP_PARAM_PLDUNIT_SL2,
														  D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_SL3 };
	const UINT32					src_pixid[2] = { 3, 4 };
	const UINT32					dst_pixid[2] = { 4, 5 };
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask[0] | unitid_bitmask[1] | D_IM_IIP_PARAM_PLDUNIT_CFL0, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
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
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_2_YCC422_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_2_YCC422_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_2_YCC422_U8_C;
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_2_YCC422_U8_A;

	pixfmt_tbl_2 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_2.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_2.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_2.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_2.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_2.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y;
	pixfmt_tbl_2.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_2.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_2.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf[0] = 0x%x 0x%x\n", (UINT32)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[0], '\0', sizeof(*oned_unit_inf[0]) );
	*oned_unit_inf[0] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[0]->LD_TOPCNF0.bit.WAITCONF = dst_portid[0];
	oned_unit_inf[0]->PIXIDDEF.bit.IPIXID = src_pixid[0];
	oned_unit_inf[0]->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC;


	oned_cfg[0].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[0].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[0].unit_param_addr = (ULONG)oned_unit_inf[0];
	oned_cfg[0].load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf[1] = 0x%x 0x%x\n", (UINT32)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[1], '\0', sizeof(*oned_unit_inf[1]) );
	*oned_unit_inf[1] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[1]->LD_TOPCNF0.bit.WAITCONF = dst_portid[1];
	oned_unit_inf[1]->PIXIDDEF.bit.IPIXID = src_pixid[1];
	oned_unit_inf[1]->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC;

	oned_cfg[1].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[1].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[1].unit_param_addr = (ULONG)oned_unit_inf[1];
	oned_cfg[1].load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "cfl_unit_inf = 0x%x 0x%x\n", (UINT32)cfl_unit_inf, sizeof(T_IM_IIP_PARAM_CFLOW) ));

	memset( cfl_unit_inf, '\0', sizeof(*cfl_unit_inf) );
	*cfl_unit_inf = g_ct_im_iip_param_cfl_base;
	cfl_unit_inf->CFTOPCONF.bit.DATACONF_0 = dst_portid[0];
	cfl_unit_inf->CFTOPCONF.bit.DATACONF_1 = dst_portid[1];

	cfl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	cfl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	cfl_cfg.unit_param_addr = (ULONG)cfl_unit_inf;
	cfl_cfg.load_unit_param_flag = 0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf[0] = 0x%x 0x%x\n", (UINT32)sl_unit_inf[0], sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf[0], '\0', sizeof(*sl_unit_inf[0]) );
	*sl_unit_inf[0] = g_ct_im_iip_param_sts_base;
	sl_unit_inf[0]->BASE.SL_TOPCNF0.bit.DATACONF = src_portid[0];
	sl_unit_inf[0]->BASE.SL_TOPCNF0.bit.WAITCONF = cfl_portid[0];
	sl_unit_inf[0]->BASE.PIXIDDEF.bit.OPIXID = dst_pixid[0];

	sl_cfg[0].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg[0].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg[0].unit_param_addr = (ULONG)sl_unit_inf[0];
	sl_cfg[0].load_unit_param_flag = unitid_bitmask[0];

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf[1] = 0x%x 0x%x\n", (UINT32)sl_unit_inf[1], sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf[1], '\0', sizeof(*sl_unit_inf[1]) );
	*sl_unit_inf[1] = g_ct_im_iip_param_sts_base;
	sl_unit_inf[1]->BASE.SL_TOPCNF0.bit.DATACONF = src_portid[1];
	sl_unit_inf[1]->BASE.SL_TOPCNF0.bit.WAITCONF = cfl_portid[1];
	sl_unit_inf[1]->BASE.PIXIDDEF.bit.OPIXID = dst_pixid[1];

	sl_cfg[1].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg[1].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg[1].unit_param_addr = (ULONG)sl_unit_inf[1];
	sl_cfg[1].load_unit_param_flag = unitid_bitmask[1];

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)cfl_unit_inf, sizeof(T_IM_IIP_PARAM_CFLOW) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf[0], sizeof(T_IM_IIP_PARAM_STS) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf[1], sizeof(T_IM_IIP_PARAM_STS) );


	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[0], &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[1], &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid[0], &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid[1], &pixfmt_tbl_2 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[0], &oned_cfg[0] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[1], &oned_cfg[1] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid[0], &sl_cfg[0] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid[1], &sl_cfg[1] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( cfl_unitid, &cfl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Dd_ARM_Dmb_Pou();

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( cfl_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid[0], D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start3\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid[1], D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start4\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid[0], D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start5\n" ));
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
	ercd = Im_IIP_Close_SWTRG( unitid_bitmask[0] | unitid_bitmask[1] | D_IM_IIP_PARAM_PLDUNIT_CFL0, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


// CFL Unit 12-1-2
// LD(YCC422)->SL(S)->LD->SL(S)
// +----+    +----+      +----+    +----+
// |LD2 |----|SL1 |------|FLT |----|SL4 |
// +----+    +----+      +----+    +----+
//           D=LD2                 D=FLT
// W=SL0     W=CFL0_0    W=SL4     W=CFL0_1
// pix=3     pix=4       pix=4     pix=5
//              |                     |
//              |        +----+       |
//              +--------|CFL0|-------+
//                       +----+
//                    D0=SL0 D1=SL3
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_12_1_2: "
INT32 CT_Im_IIP_12_1_2( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0;	// 1DL0 Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;	// SL0 Unit output / 1DL1 Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_2;	// SL3 Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg;
	T_IM_IIP_UNIT_CFG				sl_cfg[2];
	T_IM_IIP_UNIT_CFG				flt_cfg;
	T_IM_IIP_UNIT_CFG				cfl_cfg;
	T_IM_IIP_PARAM_1DL*				oned_unit_inf = &g_ct_im_iip_unit_param_1dl0;
	T_IM_IIP_PARAM_FLT*				flt_unit_inf = &g_ct_im_iip_unit_param_flt;
	T_IM_IIP_PARAM_STS*				sl_unit_inf[2] = { &g_ct_im_iip_unit_param_sl0, &g_ct_im_iip_unit_param_sl1 };
	T_IM_IIP_PARAM_CFLOW*			cfl_unit_inf = &g_ct_im_iip_unit_param_cfl;
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL4END;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_3 | E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID			src_unitid[2] = { E_IM_IIP_UNIT_ID_LD2, E_IM_IIP_UNIT_ID_FLT };
	const E_IM_IIP_PARAM_PORTID		src_portid[2] = { E_IM_IIP_PARAM_PORTID_LD2, E_IM_IIP_PARAM_PORTID_FLT_P0 };
	const E_IM_IIP_UNIT_ID			cfl_unitid    = E_IM_IIP_UNIT_ID_CFL0;
	const E_IM_IIP_PARAM_PORTID		cfl_portid[2] = { E_IM_IIP_PARAM_PORTID_CFL0_P0, E_IM_IIP_PARAM_PORTID_CFL0_P1 };
	const E_IM_IIP_UNIT_ID			dst_unitid[2] = { E_IM_IIP_UNIT_ID_SL2, E_IM_IIP_UNIT_ID_SL4 };
	const E_IM_IIP_PARAM_PORTID		dst_portid[2] = { E_IM_IIP_PARAM_PORTID_SL2, E_IM_IIP_PARAM_PORTID_SL4 };
	const UINT32					unitid_bitmask[2] = { D_IM_IIP_PARAM_PLDUNIT_LD2 | D_IM_IIP_PARAM_PLDUNIT_SL2,
														  D_IM_IIP_PARAM_PLDUNIT_FLT | D_IM_IIP_PARAM_PLDUNIT_SL4 };
	const UINT32					src_pixid[2] = { 3, 4 };
	const UINT32					dst_pixid[2] = { 4, 5 };
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask[0] | unitid_bitmask[1] | D_IM_IIP_PARAM_PLDUNIT_CFL0, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0.line_bytes.Y_G = 160;
	pixfmt_tbl_0.line_bytes.Cb_B = 0;	// dummy
	pixfmt_tbl_0.line_bytes.Cr_R = 0;	// dummy
	pixfmt_tbl_0.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_RGBA4444;
	pixfmt_tbl_0.addr.Cb_B = 0;		// dummy
	pixfmt_tbl_0.addr.Cr_R = 0;		// dummy
	pixfmt_tbl_0.addr.Alpha = 0;	// dummy
	pixfmt_tbl_0.pix_format = E_IM_IIP_PFMT_RGBA4444;
	pixfmt_tbl_0.width = 80;
	pixfmt_tbl_0.lines = 54;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.pix_format = E_IM_IIP_PFMT_RGBA4444;
	pixfmt_tbl_1.width = 80;
	pixfmt_tbl_1.lines = 54;
	pixfmt_tbl_1.line_bytes.Y_G = 160;
	pixfmt_tbl_1.line_bytes.Cb_B = 0;	// dummy
	pixfmt_tbl_1.line_bytes.Cr_R = 0;	// dummy
	pixfmt_tbl_1.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_2_RGBA4444;
	pixfmt_tbl_1.addr.Cb_B = 0;		// dummy
	pixfmt_tbl_1.addr.Cr_R = 0;		// dummy
	pixfmt_tbl_1.addr.Alpha = 0;	// dummy

	pixfmt_tbl_2 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_2.line_bytes.Y_G = 160;
	pixfmt_tbl_2.line_bytes.Cb_B = 0;	// dummy
	pixfmt_tbl_2.line_bytes.Cr_R = 0;	// dummy
	pixfmt_tbl_2.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_2.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_RGBA4444;
	pixfmt_tbl_2.addr.Cb_B = 0;		// dummy
	pixfmt_tbl_2.addr.Cr_R = 0;		// dummy
	pixfmt_tbl_2.addr.Alpha = 0;	// dummy
	pixfmt_tbl_2.pix_format = E_IM_IIP_PFMT_RGBA4444;
	pixfmt_tbl_2.width = 80;
	pixfmt_tbl_2.lines = 54;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf = 0x%x 0x%x\n", (UINT32)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) ));

	*oned_unit_inf = g_ct_im_iip_param_1dl_base;
	oned_unit_inf->LD_TOPCNF0.bit.WAITCONF = dst_portid[0];
	oned_unit_inf->PIXIDDEF.bit.IPIXID = src_pixid[0];
	oned_unit_inf->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC;
	oned_unit_inf->LD_HSIZE.bit.HEND = 80 - 1;
	oned_unit_inf->LD_VSIZE.bit.VEND = 54 - 1;


	oned_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg.unit_param_addr = (ULONG)oned_unit_inf;
	oned_cfg.load_unit_param_flag = 0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "cfl_unit_inf = 0x%x 0x%x\n", (UINT32)cfl_unit_inf, sizeof(T_IM_IIP_PARAM_CFLOW) ));

	memset( cfl_unit_inf, '\0', sizeof(*cfl_unit_inf) );
	*cfl_unit_inf = g_ct_im_iip_param_cfl_base;
	cfl_unit_inf->CFTOPCONF.bit.DATACONF_0 = dst_portid[0];
	cfl_unit_inf->CFTOPCONF.bit.DATACONF_1 = dst_portid[1];
	cfl_unit_inf->DIFF_PREWAIT.bit.DIFF_PREWAIT = 8192;		// About. (ImgCache 32kByte /4)
	cfl_unit_inf->DIFF_POSTWAIT.bit.DIFF_POSTWAIT = 0;

	cfl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	cfl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	cfl_cfg.unit_param_addr = (ULONG)cfl_unit_inf;
	cfl_cfg.load_unit_param_flag = 0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "flt_unit_inf = 0x%x 0x%x\n", (UINT32)flt_unit_inf, sizeof(T_IM_IIP_PARAM_FLT) ));

	*flt_unit_inf = g_ct_im_iip_param_flt_base;
	flt_unit_inf->TDLTOPCNF.bit.DATACONF = E_IM_IIP_PARAM_PORTID_NONE;
	flt_unit_inf->TDLTOPCNF.bit.WAITCONF_0 = dst_portid[1];
	flt_unit_inf->TDLTOPCNF.bit.WAITCONF_1 = E_IM_IIP_PARAM_PORTID_NONE;
	flt_unit_inf->PIXIDDEF.bit.IPIXID0 = src_pixid[1];
//	flt_unit_inf->PIXIDDEF.bit.IPIXID1 = src_pixid[1];
	flt_unit_inf->TDL_HSIZE.bit.HEND = 80 - 1;
	flt_unit_inf->TDL_VSIZE.bit.VEND = 54 - 1;

	flt_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	flt_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	flt_cfg.unit_param_addr = (ULONG)flt_unit_inf;
	flt_cfg.load_unit_param_flag = 0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf[0] = 0x%x 0x%x\n", (UINT32)sl_unit_inf[0], sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf[0], '\0', sizeof(*sl_unit_inf[0]) );
	*sl_unit_inf[0] = g_ct_im_iip_param_sts_base;
	sl_unit_inf[0]->BASE.SL_TOPCNF0.bit.DATACONF = src_portid[0];
	sl_unit_inf[0]->BASE.SL_TOPCNF0.bit.WAITCONF = cfl_portid[0];
	sl_unit_inf[0]->BASE.PIXIDDEF.bit.OPIXID = dst_pixid[0];
	sl_unit_inf[0]->BASE.SL_HSIZE.bit.HEND = 80 - 1;
	sl_unit_inf[0]->BASE.SL_VSIZE.bit.VEND = 54 - 1;

	sl_cfg[0].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg[0].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg[0].unit_param_addr = (ULONG)sl_unit_inf[0];
	sl_cfg[0].load_unit_param_flag = unitid_bitmask[0];

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf[1] = 0x%x 0x%x\n", (UINT32)sl_unit_inf[1], sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf[1], '\0', sizeof(*sl_unit_inf[1]) );
	*sl_unit_inf[1] = g_ct_im_iip_param_sts_base;
	sl_unit_inf[1]->BASE.SL_TOPCNF0.bit.DATACONF = src_portid[1];
	sl_unit_inf[1]->BASE.SL_TOPCNF0.bit.WAITCONF = cfl_portid[1];
	sl_unit_inf[1]->BASE.PIXIDDEF.bit.OPIXID = dst_pixid[1];
	sl_unit_inf[1]->BASE.SL_HSIZE.bit.HEND = 80 - 1;
	sl_unit_inf[1]->BASE.SL_VSIZE.bit.VEND = 54 - 1;

	sl_cfg[1].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg[1].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg[1].unit_param_addr = (ULONG)sl_unit_inf[1];
	sl_cfg[1].load_unit_param_flag = unitid_bitmask[1];

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)flt_unit_inf, sizeof(T_IM_IIP_PARAM_FLT) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)cfl_unit_inf, sizeof(T_IM_IIP_PARAM_CFLOW) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf[0], sizeof(T_IM_IIP_PARAM_STS) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf[1], sizeof(T_IM_IIP_PARAM_STS) );


	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[0], &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[1], &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid[0], &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid[1], &pixfmt_tbl_2 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[0], &oned_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[1], &flt_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid[0], &sl_cfg[0] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid[1], &sl_cfg[1] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( cfl_unitid, &cfl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Dd_ARM_Dmb_Pou();

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( cfl_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid[0], D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start3\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid[1], D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start4\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid[0], D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start5\n" ));
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
	ercd = Im_IIP_Close_SWTRG( unitid_bitmask[0] | unitid_bitmask[1] | D_IM_IIP_PARAM_PLDUNIT_CFL0, pixid_bitmask, open_res_bitmask );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


// CFL Unit 12-1-X
INT32 CT_Im_IIP_Run_12_1( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-12-1-1
		case 1:
			return CT_Im_IIP_12_1_1();

		// Test-12-1-2
		case 2:
			return CT_Im_IIP_12_1_2();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#endif //CO_CT_IM_IIP_DISABLE

