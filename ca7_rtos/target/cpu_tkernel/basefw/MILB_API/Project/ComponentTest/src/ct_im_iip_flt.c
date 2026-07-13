/**
 * @file		ct_im_iip_flt.c
 * @brief		IIP FLT unit ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// CT header
#include "ct_im_iip.h"
#include "ct_im_iip_flt.h"

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
// Nothing Special


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

// FLT Unit SLINK 9-1-1 YCC422 U8 VGA->VGA not convert
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_9_1_1: "
INT32 CT_Im_IIP_9_1_1( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0;		// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg;
	T_IM_IIP_UNIT_CFG				csc_cfg;
	T_IM_IIP_UNIT_CFG				spf_cfg;
	T_IM_IIP_PARAM_1DL*				oned_unit_inf = &g_ct_im_iip_unit_param_1dl0;
	T_IM_IIP_PARAM_CSC*				csc_unit_inf = &g_ct_im_iip_unit_param_csc;
	T_IM_IIP_PARAM_FLT*				flt_unit_inf = &g_ct_im_iip_unit_param_flt;
	T_IM_IIP_PARAM_STS*				sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG				sl_cfg;
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END;
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_CSC1 | D_IM_IIP_PARAM_PLDUNIT_FLT | D_IM_IIP_PARAM_PLDUNIT_SL2;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID			src_unitid = E_IM_IIP_UNIT_ID_LD1;
	const E_IM_IIP_UNIT_ID			filter1_unitid = E_IM_IIP_UNIT_ID_CSC1;
	const E_IM_IIP_UNIT_ID			filter2_unitid = E_IM_IIP_UNIT_ID_FLT;
	const E_IM_IIP_UNIT_ID			dst_unitid = E_IM_IIP_UNIT_ID_SL2;
	const E_IM_IIP_PARAM_PORTID		src_portid = E_IM_IIP_PARAM_PORTID_LD1;
	const E_IM_IIP_PARAM_PORTID		filter1_portid = E_IM_IIP_PARAM_PORTID_CSC1;
	const E_IM_IIP_PARAM_PORTID		filter2_portid = E_IM_IIP_PARAM_PORTID_FLT_P0;
	const E_IM_IIP_PARAM_PORTID		dst_portid = E_IM_IIP_PARAM_PORTID_SL2;
	const UINT32					src_pixid = 4;
	const UINT32					dst_pixid = 5;
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
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf = 0x%x 0x%x\n", (UINT32)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf, '\0', sizeof(*oned_unit_inf) );
	*oned_unit_inf = g_ct_im_iip_param_1dl_base;
	oned_unit_inf->LD_TOPCNF0.bit.WAITCONF = filter1_portid;
	oned_unit_inf->PIXIDDEF.bit.IPIXID = src_pixid;
	oned_unit_inf->LD_TOPCNF1.bit.LINK_UID = filter2_portid;
	oned_unit_inf->LD_CTL.bit.LD_MODE = E_IM_IIP_PARAM_LD_MODE_LINK;
	oned_unit_inf->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC;

	oned_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg.unit_param_addr = (ULONG)oned_unit_inf;
	oned_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "csc_unit_inf = 0x%x 0x%x\n", (UINT32)csc_unit_inf, sizeof(T_IM_IIP_PARAM_CSC) ));

	memset( csc_unit_inf, '\0', sizeof(*csc_unit_inf) );
	*csc_unit_inf = g_ct_im_iip_param_csc_base;
	csc_unit_inf->CSCTOPCNF.bit.DATACONF = src_portid;
	csc_unit_inf->CSCTOPCNF.bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE;

	csc_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	csc_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	csc_cfg.unit_param_addr = (ULONG)csc_unit_inf;
	csc_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "flt_unit_inf = 0x%x 0x%x\n", (UINT32)flt_unit_inf, sizeof(T_IM_IIP_PARAM_FLT) ));

	memset( flt_unit_inf, '\0', sizeof(*flt_unit_inf) );
	*flt_unit_inf = g_ct_im_iip_param_flt_base;
	flt_unit_inf->TDLTOPCNF.bit.DATACONF = filter1_portid;
	flt_unit_inf->TDLTOPCNF.bit.WAITCONF_0 = dst_portid;
	flt_unit_inf->TDLTOPCNF.bit.WAITCONF_1 = E_IM_IIP_PARAM_PORTID_NONE;
	flt_unit_inf->PIXIDDEF.bit.IPIXID0 = src_pixid;
	flt_unit_inf->PIXIDDEF.bit.IPIXID1 = src_pixid;
	flt_unit_inf->TDL_CTL.bit.INPUT_MODE = E_IM_IIP_PARAM_INPUT_MODE_SLINK;
	flt_unit_inf->TDL_PF.bit.PF_ON = E_IM_IIP_PARAM_PF_ON_INVALID;

	spf_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	spf_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	spf_cfg.unit_param_addr = (ULONG)flt_unit_inf;
	spf_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = filter2_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)csc_unit_inf, sizeof(T_IM_IIP_PARAM_CSC) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)flt_unit_inf, sizeof(T_IM_IIP_PARAM_FLT) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );


	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &oned_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( filter1_unitid, &csc_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( filter2_unitid, &spf_cfg );
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
	Ddim_Print(( "CSC1: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_CSC1.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_CSC1.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "FLT: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_FLT.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_FLT.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "SL[2]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

	Dd_ARM_Dmb_Pou();

	// Need start sequence Store->CSC->1DL->FLT or Store->1DL->CSC->FLT
	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( filter1_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start3\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start4\n" ));
	ercd = Im_IIP_Start_SWTRG( filter2_unitid, D_IM_IIP_SWTRG_ON );
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


// FLT Unit SLINK 9-1-2 Bayer 12bit packed VGA->VGA not convert
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_9_1_2: "
INT32 CT_Im_IIP_9_1_2( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0;		// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg;
	T_IM_IIP_UNIT_CFG				csc_cfg;
	T_IM_IIP_UNIT_CFG				spf_cfg;
	T_IM_IIP_PARAM_1DL*				oned_unit_inf = &g_ct_im_iip_unit_param_1dl0;
	T_IM_IIP_PARAM_CSC*				csc_unit_inf = &g_ct_im_iip_unit_param_csc;
	T_IM_IIP_PARAM_FLT*				flt_unit_inf = &g_ct_im_iip_unit_param_flt;
	T_IM_IIP_PARAM_STS*				sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG				sl_cfg;
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL6END;
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_CSC2 | D_IM_IIP_PARAM_PLDUNIT_FLT | D_IM_IIP_PARAM_PLDUNIT_SL6;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_10 | E_IM_IIP_PIXID_11;
	const E_IM_IIP_UNIT_ID			src_unitid = E_IM_IIP_UNIT_ID_LD1;
	const E_IM_IIP_UNIT_ID			filter1_unitid = E_IM_IIP_UNIT_ID_CSC2;
	const E_IM_IIP_UNIT_ID			filter2_unitid = E_IM_IIP_UNIT_ID_FLT;
	const E_IM_IIP_UNIT_ID			dst_unitid = E_IM_IIP_UNIT_ID_SL6;
	const E_IM_IIP_PARAM_PORTID		src_portid = E_IM_IIP_PARAM_PORTID_LD1;
	const E_IM_IIP_PARAM_PORTID		filter1_portid = E_IM_IIP_PARAM_PORTID_CSC2;
	const E_IM_IIP_PARAM_PORTID		filter2_portid = E_IM_IIP_PARAM_PORTID_FLT_P0;
	const E_IM_IIP_PARAM_PORTID		dst_portid = E_IM_IIP_PARAM_PORTID_SL6;
	const UINT32					src_pixid = 10;
	const UINT32					dst_pixid = 11;
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE1;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0.line_bytes.Y_G = D_IM_IIP_VGA_RAW12BIT_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cb_B = 0;	// dummy
	pixfmt_tbl_0.line_bytes.Cr_R = 0;	// dummy
	pixfmt_tbl_0.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_RAW12BIT;
	pixfmt_tbl_0.addr.Cb_B = 0;		// dummy
	pixfmt_tbl_0.addr.Cr_R = 0;		// dummy
	pixfmt_tbl_0.addr.Alpha = 0;	// dummy
	pixfmt_tbl_0.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.masterIF_Alpha = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.pix_format = E_IM_IIP_PFMT_BAYER;
	pixfmt_tbl_0.pix_depth = D_IM_IIP_PDEPTH_12BITS;


	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_RAW12BIT_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = 0;	// dummy
	pixfmt_tbl_1.line_bytes.Cr_R = 0;	// dummy
	pixfmt_tbl_1.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_RAW12BIT;
	pixfmt_tbl_1.addr.Cb_B = 0;		// dummy
	pixfmt_tbl_1.addr.Cr_R = 0;		// dummy
	pixfmt_tbl_1.addr.Alpha = 0;	// dummy
	pixfmt_tbl_1.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.masterIF_Alpha = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.pix_format = E_IM_IIP_PFMT_BAYER;
	pixfmt_tbl_1.pix_depth = D_IM_IIP_PDEPTH_12BITS;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf = 0x%x 0x%x\n", (UINT32)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf, '\0', sizeof(*oned_unit_inf) );
	*oned_unit_inf = g_ct_im_iip_param_1dl_base;
	oned_unit_inf->LD_TOPCNF0.bit.WAITCONF = filter1_portid;
	oned_unit_inf->PIXIDDEF.bit.IPIXID = src_pixid;
	oned_unit_inf->LD_TOPCNF1.bit.LINK_UID = filter2_portid;
	oned_unit_inf->LD_CTL.bit.LD_MODE = E_IM_IIP_PARAM_LD_MODE_LINK;
	oned_unit_inf->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC;
	oned_unit_inf->LD_CTL.bit.CSEL = E_IM_IIP_PARAM_CSEL_1;

	oned_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg.unit_param_addr = (ULONG)oned_unit_inf;
	oned_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "csc_unit_inf = 0x%x 0x%x\n", (UINT32)csc_unit_inf, sizeof(T_IM_IIP_PARAM_CSC) ));

	memset( csc_unit_inf, '\0', sizeof(*csc_unit_inf) );
	*csc_unit_inf = g_ct_im_iip_param_csc_base;
	csc_unit_inf->CSCTOPCNF.bit.DATACONF = src_portid;
	csc_unit_inf->CSCTOPCNF.bit.WAITCONF = E_IM_IIP_PARAM_PORTID_NONE;

	csc_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	csc_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	csc_cfg.unit_param_addr = (ULONG)csc_unit_inf;
	csc_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "flt_unit_inf = 0x%x 0x%x\n", (UINT32)flt_unit_inf, sizeof(T_IM_IIP_PARAM_FLT) ));

	memset( flt_unit_inf, '\0', sizeof(*flt_unit_inf) );
	*flt_unit_inf = g_ct_im_iip_param_flt_base;
	flt_unit_inf->TDLTOPCNF.bit.DATACONF = filter1_portid;
	flt_unit_inf->TDLTOPCNF.bit.WAITCONF_0 = dst_portid;
	flt_unit_inf->TDLTOPCNF.bit.WAITCONF_1 = E_IM_IIP_PARAM_PORTID_NONE;
	flt_unit_inf->PIXIDDEF.bit.IPIXID0 = src_pixid;
	flt_unit_inf->PIXIDDEF.bit.IPIXID1 = src_pixid;
	flt_unit_inf->TDL_CTL.bit.INPUT_MODE = E_IM_IIP_PARAM_INPUT_MODE_SLINK;
	flt_unit_inf->TDL_PF.bit.PF_ON = E_IM_IIP_PARAM_PF_ON_INVALID;
	flt_unit_inf->TDL_CTL.bit.CSEL = E_IM_IIP_PARAM_CSEL_1;
	flt_unit_inf->UNIT_OUT.bit.UNIT_OUT_A_D0 = E_IM_IIP_PARAM_UNIT_OUT_ISO;			// port 0
	flt_unit_inf->UNIT_OUT.bit.UNIT_OUT_A_D1 = E_IM_IIP_PARAM_UNIT_OUT_DISABLE;		// port 0
	flt_unit_inf->UNIT_OUT.bit.UNIT_OUT_A_D2 = E_IM_IIP_PARAM_UNIT_OUT_DISABLE;		// port 0
	flt_unit_inf->ISO_TH_D[0].bit.ISO_UTH_D = 10;
	flt_unit_inf->ISO_TH_D[0].bit.ISO_LTH_D = 10;
	flt_unit_inf->ISO_TH_D[1].bit.ISO_UTH_D = 10;
	flt_unit_inf->ISO_TH_D[1].bit.ISO_LTH_D = 10;
	flt_unit_inf->ISO_TH_D[2].bit.ISO_UTH_D = 10;
	flt_unit_inf->ISO_TH_D[2].bit.ISO_LTH_D = 10;
	flt_unit_inf->ISO_TH_D[3].bit.ISO_UTH_D = 10;
	flt_unit_inf->ISO_TH_D[3].bit.ISO_LTH_D = 10;

	spf_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	spf_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	spf_cfg.unit_param_addr = (ULONG)flt_unit_inf;
	spf_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = filter2_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf, sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)csc_unit_inf, sizeof(T_IM_IIP_PARAM_CSC) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)flt_unit_inf, sizeof(T_IM_IIP_PARAM_FLT) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );


	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &oned_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( filter1_unitid, &csc_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( filter2_unitid, &spf_cfg );
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
	Ddim_Print(( "CSC1: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_CSC1.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_CSC1.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "FLT: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_FLT.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_FLT.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "SL[2]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

	Dd_ARM_Dmb_Pou();

	// Need start sequence Store->CSC->1DL->FLT or Store->1DL->CSC->FLT
	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( filter1_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start3\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start4\n" ));
	ercd = Im_IIP_Start_SWTRG( filter2_unitid, D_IM_IIP_SWTRG_ON );
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


// FLT Unit SLINK 9-1-X
INT32 CT_Im_IIP_Run_9_1( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-9-1-1
		case 1:
			return CT_Im_IIP_9_1_1();

		// Test-9-1-2
		case 2:
			return CT_Im_IIP_9_1_2();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


// FLT Unit DLINK 9-2-1
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_9_2_1: "
INT32 CT_Im_IIP_9_2_1( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0;		// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;		// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_2;		// SL Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg[2];
	T_IM_IIP_UNIT_CFG				blend_cfg;
	T_IM_IIP_UNIT_CFG				spf_cfg;
	T_IM_IIP_PARAM_1DL*				oned_unit_inf[2] = { &g_ct_im_iip_unit_param_1dl0, &g_ct_im_iip_unit_param_1dl1 };
	T_IM_IIP_PARAM_BLEND*			blend_unit_inf = &g_ct_im_iip_unit_param_blend;
	T_IM_IIP_PARAM_FLT*				flt_unit_inf = &g_ct_im_iip_unit_param_flt;
	T_IM_IIP_PARAM_STS*				sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG				sl_cfg;
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL6END;
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_LD2 | D_IM_IIP_PARAM_PLDUNIT_BLEND0 | D_IM_IIP_PARAM_PLDUNIT_FLT | D_IM_IIP_PARAM_PLDUNIT_SL6;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_9 | E_IM_IIP_PIXID_10 | E_IM_IIP_PIXID_11;
	const E_IM_IIP_UNIT_ID			src_unitid[2] = { E_IM_IIP_UNIT_ID_LD1, E_IM_IIP_UNIT_ID_LD2 };
	const E_IM_IIP_UNIT_ID			filter1_unitid = E_IM_IIP_UNIT_ID_BLEND0;
	const E_IM_IIP_UNIT_ID			filter2_unitid = E_IM_IIP_UNIT_ID_FLT;
	const E_IM_IIP_UNIT_ID			dst_unitid = E_IM_IIP_UNIT_ID_SL6;
	const E_IM_IIP_PARAM_PORTID		src_portid[2] = { E_IM_IIP_PARAM_PORTID_LD1, E_IM_IIP_PARAM_PORTID_LD2 };
	const E_IM_IIP_PARAM_PORTID		filter1_portid[2] = { E_IM_IIP_PARAM_PORTID_BLEND0_P0, E_IM_IIP_PARAM_PORTID_BLEND0_P1 };
	const E_IM_IIP_PARAM_PORTID		filter2_portid = E_IM_IIP_PARAM_PORTID_FLT_P0;
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
	pixfmt_tbl_0.line_bytes.Y_G = D_IM_IIP_VGA_RGBA8888_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cb_B = 0;	// dummy
	pixfmt_tbl_0.line_bytes.Cr_R = 0;	// dummy
	pixfmt_tbl_0.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_RGBA8888;
	pixfmt_tbl_0.addr.Cb_B = 0;		// dummy
	pixfmt_tbl_0.addr.Cr_R = 0;		// dummy
	pixfmt_tbl_0.addr.Alpha = 0;	// dummy
	pixfmt_tbl_0.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.masterIF_Alpha = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.frame_type = E_IM_IIP_FTYPE_CHUNKY;	// RGBA8888
	pixfmt_tbl_0.pix_format = E_IM_IIP_PFMT_YCC444;		// RGBA8888
	pixfmt_tbl_0.alpha_depth = E_IM_IIP_ADEPTH_8BITS;	// RGBA8888
	pixfmt_tbl_0.alpha = D_IM_IIP_ALPHA_TRUE;			// RGBA8888


	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_RGBA8888_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = 0;	// dummy
	pixfmt_tbl_1.line_bytes.Cr_R = 0;	// dummy
	pixfmt_tbl_1.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_4_RGBA8888;
	pixfmt_tbl_1.addr.Cb_B = 0;		// dummy
	pixfmt_tbl_1.addr.Cr_R = 0;		// dummy
	pixfmt_tbl_1.addr.Alpha = 0;	// dummy
	pixfmt_tbl_1.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.masterIF_Alpha = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.frame_type = E_IM_IIP_FTYPE_CHUNKY;	// RGBA8888
	pixfmt_tbl_1.pix_format = E_IM_IIP_PFMT_YCC444;		// RGBA8888
	pixfmt_tbl_1.alpha_depth = E_IM_IIP_ADEPTH_8BITS;	// RGBA8888
	pixfmt_tbl_1.alpha = D_IM_IIP_ALPHA_TRUE;			// RGBA8888


	pixfmt_tbl_2 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_2.line_bytes.Y_G = D_IM_IIP_VGA_RGBA8888_GLOBAL_WIDTH;
	pixfmt_tbl_2.line_bytes.Cb_B = 0;	// dummy
	pixfmt_tbl_2.line_bytes.Cr_R = 0;	// dummy
	pixfmt_tbl_2.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_2.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_RGBA8888;
	pixfmt_tbl_2.addr.Cb_B = 0;		// dummy
	pixfmt_tbl_2.addr.Cr_R = 0;		// dummy
	pixfmt_tbl_2.addr.Alpha = 0;	// dummy
	pixfmt_tbl_2.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_2.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_2.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_2.masterIF_Alpha = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_2.frame_type = E_IM_IIP_FTYPE_CHUNKY;	// RGBA8888
	pixfmt_tbl_2.pix_format = E_IM_IIP_PFMT_YCC444;		// RGBA8888
	pixfmt_tbl_2.alpha_depth = E_IM_IIP_ADEPTH_8BITS;	// RGBA8888
	pixfmt_tbl_2.alpha = D_IM_IIP_ALPHA_TRUE;			// RGBA8888


	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf[0] = 0x%x 0x%x\n", (UINT32)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[0], '\0', sizeof(*oned_unit_inf[0]) );
	*oned_unit_inf[0] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[0]->LD_TOPCNF0.bit.WAITCONF = filter1_portid[0];
	oned_unit_inf[0]->PIXIDDEF.bit.IPIXID = src_pixid[0];
	oned_unit_inf[0]->LD_TOPCNF1.bit.LINK_UID = filter2_portid;
	oned_unit_inf[0]->LD_CTL.bit.LD_MODE = E_IM_IIP_PARAM_LD_MODE_LINK;
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
	oned_unit_inf[1]->LD_TOPCNF1.bit.LINK_UID = filter2_portid;
	oned_unit_inf[1]->LD_CTL.bit.LD_MODE = E_IM_IIP_PARAM_LD_MODE_LINK;
	oned_unit_inf[1]->LD_CTL.bit.SLOW = E_IM_IIP_PARAM_SLOW_1PIX_CYC;
	oned_unit_inf[1]->LD_CTL.bit.CSEL = E_IM_IIP_PARAM_CSEL_1;

	oned_cfg[1].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[1].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[1].unit_param_addr = (ULONG)oned_unit_inf[1];
	oned_cfg[1].load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "blend_unit_inf = 0x%x 0x%x\n", (UINT32)blend_unit_inf, sizeof(T_IM_IIP_PARAM_BLEND) ));

	memset( blend_unit_inf, '\0', sizeof(*blend_unit_inf) );
	*blend_unit_inf = g_ct_im_iip_param_blend_base;
	blend_unit_inf->BLENDTOPCNF0.bit.WAITCONF_0 = E_IM_IIP_PARAM_PORTID_NONE;
	blend_unit_inf->BLENDTOPCNF0.bit.DATACONF_0 = src_portid[0];
	blend_unit_inf->BLENDTOPCNF1.bit.DATACONF_1 = src_portid[1];
	blend_unit_inf->BLENDCTL.bit.ALPHA_OUT = E_IM_IIP_PARAM_ALPHA_OUT_P0_ALPHA;
	blend_unit_inf->ALPHACTL0.bit.ALPHA_SEL = E_IM_IIP_PARAM_ALPHA_SEL_P0_ALPHA;

	blend_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	blend_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	blend_cfg.unit_param_addr = (ULONG)blend_unit_inf;
	blend_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "flt_unit_inf = 0x%x 0x%x\n", (UINT32)flt_unit_inf, sizeof(T_IM_IIP_PARAM_FLT) ));

	memset( flt_unit_inf, '\0', sizeof(*flt_unit_inf) );
	*flt_unit_inf = g_ct_im_iip_param_flt_base;
	flt_unit_inf->TDLTOPCNF.bit.DATACONF = filter1_portid[0];
	flt_unit_inf->TDLTOPCNF.bit.WAITCONF_0 = dst_portid;
	flt_unit_inf->TDLTOPCNF.bit.WAITCONF_1 = E_IM_IIP_PARAM_PORTID_NONE;
	flt_unit_inf->PIXIDDEF.bit.IPIXID0 = src_pixid[0];
	flt_unit_inf->PIXIDDEF.bit.IPIXID1 = src_pixid[1];
	flt_unit_inf->TDL_CTL.bit.INPUT_MODE = E_IM_IIP_PARAM_INPUT_MODE_DLINK;
	flt_unit_inf->TDL_PF.bit.PF_ON = E_IM_IIP_PARAM_PF_ON_INVALID;
	flt_unit_inf->TDL_CTL.bit.CSEL = E_IM_IIP_PARAM_CSEL_1;
	flt_unit_inf->UNIT_OUT.bit.UNIT_OUT_A_D0 = E_IM_IIP_PARAM_UNIT_OUT_EP,		// port 0
	flt_unit_inf->UNIT_OUT.bit.UNIT_OUT_A_D1 = E_IM_IIP_PARAM_UNIT_OUT_EP,		// port 0
	flt_unit_inf->UNIT_OUT.bit.UNIT_OUT_A_D2 = E_IM_IIP_PARAM_UNIT_OUT_EP,		// port 0

	spf_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	spf_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	spf_cfg.unit_param_addr = (ULONG)flt_unit_inf;
	spf_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = filter2_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)blend_unit_inf, sizeof(T_IM_IIP_PARAM_BLEND) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)flt_unit_inf, sizeof(T_IM_IIP_PARAM_FLT) );
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

	ercd = Im_IIP_Ctrl_SWTRG_Unit( filter2_unitid, &spf_cfg );
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
	Ddim_Print(( "FLT: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_FLT.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_FLT.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "SL[6]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL6.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL6.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL6.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL6.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

	Dd_ARM_Dmb_Pou();

	// Need start sequence Store->CSC->1DL->FLT or Store->1DL->CSC->FLT
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

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start5\n" ));
	ercd = Im_IIP_Start_SWTRG( filter2_unitid, D_IM_IIP_SWTRG_ON );
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


// FLT Unit DLINK 9-2-X
INT32 CT_Im_IIP_Run_9_2( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-9-2-1
		case 1:
			return CT_Im_IIP_9_2_1();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


// FLT Unit NORMAL 9-3-1
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_9_3_1: "
INT32 CT_Im_IIP_9_3_1( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0;		// FLT Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;		// SL Unit input
	T_IM_IIP_UNIT_CFG				spf_cfg;
	T_IM_IIP_PARAM_FLT*				flt_unit_inf = &g_ct_im_iip_unit_param_flt;
	T_IM_IIP_PARAM_STS*				sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG				sl_cfg;
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL6END;
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_FLT | D_IM_IIP_PARAM_PLDUNIT_SL6;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_10 | E_IM_IIP_PIXID_11;
	const E_IM_IIP_UNIT_ID			src_unitid = E_IM_IIP_UNIT_ID_FLT;
	const E_IM_IIP_UNIT_ID			dst_unitid = E_IM_IIP_UNIT_ID_SL6;
	const E_IM_IIP_PARAM_PORTID		src_portid = E_IM_IIP_PARAM_PORTID_FLT_P0;
	const E_IM_IIP_PARAM_PORTID		dst_portid = E_IM_IIP_PARAM_PORTID_SL6;
	const UINT32					src_pixid = 10;
	const UINT32					dst_pixid = 11;
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE1;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0.line_bytes.Y_G = D_IM_IIP_VGA_RGBA5551_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cb_B = 0;	// dummy
	pixfmt_tbl_0.line_bytes.Cr_R = 0;	// dummy
	pixfmt_tbl_0.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_RGBA5551;
	pixfmt_tbl_0.addr.Cb_B = 0;		// dummy
	pixfmt_tbl_0.addr.Cr_R = 0;		// dummy
	pixfmt_tbl_0.addr.Alpha = 0;	// dummy
	pixfmt_tbl_0.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.masterIF_Alpha = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_0.pix_format = E_IM_IIP_PFMT_RGBA5551;
	pixfmt_tbl_0.alpha = D_IM_IIP_ALPHA_TRUE;


	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_RGBA5551_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = 0;	// dummy
	pixfmt_tbl_1.line_bytes.Cr_R = 0;	// dummy
	pixfmt_tbl_1.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_RGBA5551;
	pixfmt_tbl_1.addr.Cb_B = 0;		// dummy
	pixfmt_tbl_1.addr.Cr_R = 0;		// dummy
	pixfmt_tbl_1.addr.Alpha = 0;	// dummy
	pixfmt_tbl_1.masterIF_Y_G = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.masterIF_Cb_B = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.masterIF_Cr_R = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.masterIF_Alpha = D_IM_IIP_MASTER_IF1;
	pixfmt_tbl_1.pix_format = E_IM_IIP_PFMT_RGBA5551;
	pixfmt_tbl_1.alpha = D_IM_IIP_ALPHA_TRUE;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "flt_unit_inf = 0x%x 0x%x\n", (UINT32)flt_unit_inf, sizeof(T_IM_IIP_PARAM_FLT) ));

	memset( flt_unit_inf, '\0', sizeof(*flt_unit_inf) );
	*flt_unit_inf = g_ct_im_iip_param_flt_base;
	flt_unit_inf->TDLTOPCNF.bit.DATACONF = E_IM_IIP_PARAM_PORTID_NONE;
	flt_unit_inf->TDLTOPCNF.bit.WAITCONF_0 = dst_portid;
	flt_unit_inf->TDLTOPCNF.bit.WAITCONF_1 = E_IM_IIP_PARAM_PORTID_NONE;
	flt_unit_inf->PIXIDDEF.bit.IPIXID0 = src_pixid;
	flt_unit_inf->PIXIDDEF.bit.IPIXID1 = src_pixid;
	flt_unit_inf->TDL_CTL.bit.CSEL = E_IM_IIP_PARAM_CSEL_1;
	flt_unit_inf->UNIT_OUT.bit.UNIT_OUT_A_D0 = E_IM_IIP_PARAM_UNIT_OUT_EP,		// port 0
	flt_unit_inf->UNIT_OUT.bit.UNIT_OUT_A_D1 = E_IM_IIP_PARAM_UNIT_OUT_EP,		// port 0
	flt_unit_inf->UNIT_OUT.bit.UNIT_OUT_A_D2 = E_IM_IIP_PARAM_UNIT_OUT_EP,		// port 0

	spf_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	spf_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	spf_cfg.unit_param_addr = (ULONG)flt_unit_inf;
	spf_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)flt_unit_inf, sizeof(T_IM_IIP_PARAM_FLT) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );


	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &spf_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	Ddim_Print(( "FLT: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_FLT.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_FLT.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "SL[6]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL6.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL6.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL6.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL6.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

	CT_Im_IIP_Print_Unitcfg_Byid( src_unitid );
	CT_Im_IIP_Print_Unitcfg_Byid( dst_unitid );
	CT_Im_IIP_Print_Pixfmttbl_Detail( src_pixid );
	CT_Im_IIP_Print_Pixfmttbl_Detail( dst_pixid );

	Dd_ARM_Dmb_Pou();

	// Need start sequence Store->CSC->1DL->FLT or Store->1DL->CSC->FLT
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

// FLT Unit NORMAL 9-3-X
INT32 CT_Im_IIP_Run_9_3( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-9-3-1
		case 1:
			return CT_Im_IIP_9_3_1();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#endif //CO_CT_IM_IIP_DISABLE

