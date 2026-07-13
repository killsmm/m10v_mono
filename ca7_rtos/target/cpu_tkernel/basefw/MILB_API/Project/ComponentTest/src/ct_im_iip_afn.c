/**
 * @file		ct_im_iip_afn.c
 * @brief		IIP AFN unit ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// CT header
#include "ct_im_iip.h"
#include "ct_im_iip_afn.h"

// im_iip header
#include "im_iip.h"

// MILB register header
#include "jdsiip.h"

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#include "palladium_test.h"
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

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

// FRECT table size (for VGA 640x480)
#define D_CT_IM_IIP_FRECT_TBL_X_SZ	(64)
#define D_CT_IM_IIP_FRECT_TBL_Y_SZ	(8)
// FRECT table number (for VGA 640x480)
#define D_CT_IM_IIP_FRECT_TBL_X_NUM	(10)		// (640 / 64)
#define D_CT_IM_IIP_FRECT_TBL_Y_NUM	(60)		// (480 / 8)
#define D_CT_IM_IIP_FRECT_TBL_NUM	(D_CT_IM_IIP_FRECT_TBL_X_NUM * D_CT_IM_IIP_FRECT_TBL_Y_NUM)


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



/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

// AFN Unit 3-1-1 YCC422 U8 VGA->VGA D-Zoom x1.0 (simple copy)
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_3_1_1: "
INT32 CT_Im_IIP_3_1_1( VOID )
{
	INT32						ercd;
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_0;		// AFN Unit input
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG			afn_cfg;
	T_IM_IIP_PARAM_AFN*			afn_unit_inf = &g_ct_im_iip_unit_param_afn;
	T_IM_IIP_PARAM_AFN_OPCOL_0*	afn_tbl = &g_ct_im_iip_afntbl;
	T_IM_IIP_PARAM_STS*			sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG			sl_cfg;
	UINT32						wait_factor_result = 0;
	const UINT32				wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL5END;
	const UINT32				unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_AFN0 | D_IM_IIP_PARAM_PLDUNIT_SL5;
	const UINT32				pixid_bitmask = E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID		src_unitid = E_IM_IIP_UNIT_ID_AFN0;
	const E_IM_IIP_UNIT_ID		dst_unitid = E_IM_IIP_UNIT_ID_SL5;
	const E_IM_IIP_PARAM_PORTID	src_portid = E_IM_IIP_PARAM_PORTID_AFN0;
	const E_IM_IIP_PARAM_PORTID	dst_portid = E_IM_IIP_PARAM_PORTID_SL5;
	const UINT32				src_pixid = 4;
	const UINT32				dst_pixid = 5;
	const UINT32				open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


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
#ifdef CO_CT_IM_IIP_3_1_1_FILL_TEST
	pixfmt_tbl_0.pix_outside_img = D_IM_IIP_OUTSIDE_IMG_FIXED_VALUE;
	pixfmt_tbl_0.Y_G_val_outside_img = 1;
	pixfmt_tbl_0.Cb_B_val_outside_img = 255;
	pixfmt_tbl_0.Cr_R_val_outside_img = 128;
#endif // CO_CT_IM_IIP_3_1_1_FILL_TEST

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_tbl = 0x%x 0x%x\n", (UINT32)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) ));

	memset( afn_tbl, '\0', sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
#ifdef CO_CT_IM_IIP_3_1_1_FILL_TEST
	afn_tbl->STAXY0.bit.STAX = M_CT_IM_IIP_AFNTBL_CALC_POS(-D_IM_IIP_VGA_WIDTH -2);
	afn_tbl->STAXY0.bit.STAY = M_CT_IM_IIP_AFNTBL_CALC_POS(-D_IM_IIP_VGA_LINES -2);
#else // CO_CT_IM_IIP_3_1_1_FILL_TEST
	afn_tbl->STAXY0.bit.STAX = M_CT_IM_IIP_AFNTBL_CALC_POS(0);
	afn_tbl->STAXY0.bit.STAY = M_CT_IM_IIP_AFNTBL_CALC_POS(0);
#endif // CO_CT_IM_IIP_3_1_1_FILL_TEST
	afn_tbl->D_OUT_IN_0.bit.DXX = M_CT_IM_IIP_AFNTBL_CALC_POS(1.0F);	// X increment X=1.0
	afn_tbl->D_OUT_IN_0.bit.DXY = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// X increment Y=0.0
	afn_tbl->D_OUT_IN_0.bit.DYX = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// Y increment X=0.0
	afn_tbl->D_OUT_IN_0.bit.DYY = M_CT_IM_IIP_AFNTBL_CALC_POS(1.0F);	// Y increment Y=1.0
	afn_tbl->SZ.bit.AFNDHSZ = D_IM_IIP_VGA_WIDTH;
	afn_tbl->SZ.bit.AFNDVSZ = D_IM_IIP_VGA_LINES;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_unit_inf = 0x%x 0x%x\n", (UINT32)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) ));
	Ddim_Print(( D_IM_IIP_FUNC_NAME "STAX=0x%x STAY=0x%x\n", (INT32)afn_tbl->STAXY0.bit.STAX, (INT32)afn_tbl->STAXY0.bit.STAY ));

	memset( afn_unit_inf, '\0', sizeof(*afn_unit_inf) );
	*afn_unit_inf = g_ct_im_iip_param_afn_base;
	afn_unit_inf->AFNTOPCNF.bit.WAITCONF = dst_portid;
	afn_unit_inf->PIXIDEF.bit.IPIXID = src_pixid;
	afn_unit_inf->AFNPA.word = (ULONG)afn_tbl;
#ifdef CO_CT_IM_IIP_3_1_1_FILL_TEST
	afn_unit_inf->CALMETHOD.bit.FILMD = 1;
	afn_unit_inf->FILVAL0.bit.FILVALYG = 1;
	afn_unit_inf->FILVAL0.bit.FILVALB = 255;
	afn_unit_inf->FILVAL1.bit.FILVALR = 128;
#endif // CO_CT_IM_IIP_3_1_1_FILL_TEST

	afn_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	afn_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	afn_cfg.unit_param_addr = (ULONG)afn_unit_inf;
	afn_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#if 0
	Palladium_Set_Out_Localstack( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	Palladium_Set_Out_Localstack( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &afn_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	Ddim_Print(( "AFN[0]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "SL[2]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

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
	// UnitINF, AFNtblダンプ
	Palladium_Set_Out_Localstack( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	Palladium_Set_Out_Localstack( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif


#if 0
	// dump UNITINF AFN[0]
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( src_unitid, gCtImIIP_Get_Unit_Param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF AFN[0] %u\n", param_bytes ));
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


// AFN Unit 3-1-2 YCC422 U8 VGA->VGA D-Zoom x2.0
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_3_1_2: "
INT32 CT_Im_IIP_3_1_2( VOID )
{
	INT32						ercd;
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_0;		// AFN Unit input
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG			afn_cfg;
	T_IM_IIP_PARAM_AFN*			afn_unit_inf = &g_ct_im_iip_unit_param_afn;
	T_IM_IIP_PARAM_AFN_OPCOL_0*	afn_tbl = &g_ct_im_iip_afntbl;
	T_IM_IIP_PARAM_STS*			sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG			sl_cfg;
	UINT32						wait_factor_result = 0;
	const UINT32				wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END;
	const UINT32				unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_AFN0 | D_IM_IIP_PARAM_PLDUNIT_SL2;
	const UINT32				pixid_bitmask = E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID		src_unitid = E_IM_IIP_UNIT_ID_AFN0;
	const E_IM_IIP_UNIT_ID		dst_unitid = E_IM_IIP_UNIT_ID_SL2;
	const E_IM_IIP_PARAM_PORTID	src_portid = E_IM_IIP_PARAM_PORTID_AFN0;
	const E_IM_IIP_PARAM_PORTID	dst_portid = E_IM_IIP_PARAM_PORTID_SL2;
	const UINT32				src_pixid = 4;
	const UINT32				dst_pixid = 5;
	const UINT32				open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


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

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_tbl = 0x%x 0x%x\n", (UINT32)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) ));

	memset( afn_tbl, '\0', sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	afn_tbl->STAXY0.bit.STAX = M_CT_IM_IIP_AFNTBL_CALC_POS(D_IM_IIP_VGA_WIDTH /4);
	afn_tbl->STAXY0.bit.STAY = M_CT_IM_IIP_AFNTBL_CALC_POS(D_IM_IIP_VGA_LINES /4);
	afn_tbl->D_OUT_IN_0.bit.DXX = M_CT_IM_IIP_AFNTBL_CALC_POS(0.5F);	// X increment X=0.5
	afn_tbl->D_OUT_IN_0.bit.DXY = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// X increment Y=0.0
	afn_tbl->D_OUT_IN_0.bit.DYX = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// Y increment X=0.0
	afn_tbl->D_OUT_IN_0.bit.DYY = M_CT_IM_IIP_AFNTBL_CALC_POS(0.5F);	// Y increment Y=0.5
	afn_tbl->SZ.bit.AFNDHSZ = D_IM_IIP_VGA_WIDTH;
	afn_tbl->SZ.bit.AFNDVSZ = D_IM_IIP_VGA_LINES;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_unit_inf = 0x%x 0x%x\n", (UINT32)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) ));

	memset( afn_unit_inf, '\0', sizeof(*afn_unit_inf) );
	*afn_unit_inf = g_ct_im_iip_param_afn_base;
	afn_unit_inf->AFNTOPCNF.bit.WAITCONF = dst_portid;
	afn_unit_inf->PIXIDEF.bit.IPIXID = src_pixid;
	afn_unit_inf->AFNPA.word = (ULONG)afn_tbl;

	afn_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	afn_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	afn_cfg.unit_param_addr = (ULONG)afn_unit_inf;
	afn_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#if 0
	Palladium_Set_Out_Localstack( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	Palladium_Set_Out_Localstack( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &afn_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	Ddim_Print(( "AFN[0]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL0.bit.HWEN
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
	// UnitINF, AFNtblダンプ
	Palladium_Set_Out_Localstack( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	Palladium_Set_Out_Localstack( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif


#if 0
	// dump UNITINF AFN[0]
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( src_unitid, gCtImIIP_Get_Unit_Param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF AFN[0] %u\n", param_bytes ));
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


// AFN Unit 3-1-3 Plane U16(valid 14bit) VGA->VGA D-Zoom x2.0
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_3_1_3: "
INT32 CT_Im_IIP_3_1_3( VOID )
{
	INT32						ercd;
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_0;		// AFN Unit input
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG			afn_cfg;
	T_IM_IIP_PARAM_AFN*			afn_unit_inf = &g_ct_im_iip_unit_param_afn;
	T_IM_IIP_PARAM_AFN_OPCOL_0*	afn_tbl = &g_ct_im_iip_afntbl;
	T_IM_IIP_PARAM_STS*			sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG			sl_cfg;
	UINT32						wait_factor_result = 0;
	const UINT32				wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END;
	const UINT32				unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_AFN0 | D_IM_IIP_PARAM_PLDUNIT_SL2;
	const UINT32				pixid_bitmask = E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID		src_unitid = E_IM_IIP_UNIT_ID_AFN0;
	const E_IM_IIP_UNIT_ID		dst_unitid = E_IM_IIP_UNIT_ID_SL2;
	const E_IM_IIP_PARAM_PORTID	src_portid = E_IM_IIP_PARAM_PORTID_AFN0;
	const E_IM_IIP_PARAM_PORTID	dst_portid = E_IM_IIP_PARAM_PORTID_SL2;
	const UINT32				src_pixid = 4;
	const UINT32				dst_pixid = 5;
	const UINT32				open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0.line_bytes.Y_G = D_IM_IIP_VGA_PLANE14BIT_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cb_B = D_IM_IIP_VGA_PLANE14BIT_GLOBAL_WIDTH;	// dummy
	pixfmt_tbl_0.line_bytes.Cr_R = D_IM_IIP_VGA_PLANE14BIT_GLOBAL_WIDTH;	// dummy
	pixfmt_tbl_0.line_bytes.Alpha = D_IM_IIP_VGA_PLANE14BIT_GLOBAL_WIDTH;	// dummy
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_PLANE14BIT;
	pixfmt_tbl_0.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_PLANE14BIT;			// dummy
	pixfmt_tbl_0.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_PLANE14BIT;			// dummy
	pixfmt_tbl_0.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_PLANE14BIT;			// dummy
	pixfmt_tbl_0.pix_format = E_IM_IIP_PFMT_PLANE;
	pixfmt_tbl_0.pix_depth = D_IM_IIP_PDEPTH_16BITS;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_PLANE14BIT_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_PLANE14BIT_GLOBAL_WIDTH;	// dummy
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_PLANE14BIT_GLOBAL_WIDTH;	// dummy
	pixfmt_tbl_1.line_bytes.Alpha = D_IM_IIP_VGA_PLANE14BIT_GLOBAL_WIDTH;	// dummy
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_PLANE14BIT;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_PLANE14BIT;			// dummy
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_PLANE14BIT;			// dummy
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_PLANE14BIT;			// dummy
	pixfmt_tbl_1.pix_format = E_IM_IIP_PFMT_PLANE;
	pixfmt_tbl_1.pix_depth = D_IM_IIP_PDEPTH_16BITS;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_tbl = 0x%x 0x%x\n", (UINT32)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) ));

	memset( afn_tbl, '\0', sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	afn_tbl->STAXY0.bit.STAX = M_CT_IM_IIP_AFNTBL_CALC_POS(D_IM_IIP_VGA_WIDTH /4);
	afn_tbl->STAXY0.bit.STAY = M_CT_IM_IIP_AFNTBL_CALC_POS(D_IM_IIP_VGA_LINES /4);
	afn_tbl->D_OUT_IN_0.bit.DXX = M_CT_IM_IIP_AFNTBL_CALC_POS(0.5F);	// X increment X=0.5
	afn_tbl->D_OUT_IN_0.bit.DXY = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// X increment Y=0.0
	afn_tbl->D_OUT_IN_0.bit.DYX = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// Y increment X=0.0
	afn_tbl->D_OUT_IN_0.bit.DYY = M_CT_IM_IIP_AFNTBL_CALC_POS(0.5F);	// Y increment Y=0.5
	afn_tbl->SZ.bit.AFNDHSZ = D_IM_IIP_VGA_WIDTH;
	afn_tbl->SZ.bit.AFNDVSZ = D_IM_IIP_VGA_LINES;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_unit_inf = 0x%x 0x%x\n", (UINT32)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) ));

	memset( afn_unit_inf, '\0', sizeof(*afn_unit_inf) );
	*afn_unit_inf = g_ct_im_iip_param_afn_base;
	afn_unit_inf->AFNTOPCNF.bit.WAITCONF = dst_portid;
	afn_unit_inf->PIXIDEF.bit.IPIXID = src_pixid;
	afn_unit_inf->AFNPA.word = (ULONG)afn_tbl;

	afn_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	afn_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	afn_cfg.unit_param_addr = (ULONG)afn_unit_inf;
	afn_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#if 0
	Palladium_Set_Out_Localstack( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	Palladium_Set_Out_Localstack( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &afn_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	Ddim_Print(( "AFN[0]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL0.bit.HWEN
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
	// UnitINF, AFNtblダンプ
	Palladium_Set_Out_Localstack( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	Palladium_Set_Out_Localstack( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif


#if 0
	// dump UNITINF AFN[0]
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( src_unitid, gCtImIIP_Get_Unit_Param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF AFN[0] %u\n", param_bytes ));
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


// AFN Unit 3-1-4 YCC422 U8 VGA->VGA D-Zoom x2.0 BI-Cubic
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_3_1_4: "
INT32 CT_Im_IIP_3_1_4( VOID )
{
	INT32						ercd;
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_0;		// AFN Unit input
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG			afn_cfg;
	T_IM_IIP_PARAM_AFN*			afn_unit_inf = &g_ct_im_iip_unit_param_afn;
	T_IM_IIP_PARAM_AFN_OPCOL_0*	afn_tbl = &g_ct_im_iip_afntbl;
	T_IM_IIP_PARAM_STS*			sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG			sl_cfg;
	UINT32						wait_factor_result = 0;
	const UINT32				wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END;
	const UINT32				unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_AFN0 | D_IM_IIP_PARAM_PLDUNIT_SL2;
	const UINT32				pixid_bitmask = E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID		src_unitid = E_IM_IIP_UNIT_ID_AFN0;
	const E_IM_IIP_UNIT_ID		dst_unitid = E_IM_IIP_UNIT_ID_SL2;
	const E_IM_IIP_PARAM_PORTID	src_portid = E_IM_IIP_PARAM_PORTID_AFN0;
	const E_IM_IIP_PARAM_PORTID	dst_portid = E_IM_IIP_PARAM_PORTID_SL2;
	const UINT32				src_pixid = 4;
	const UINT32				dst_pixid = 5;
	const UINT32				open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


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

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_tbl = 0x%x 0x%x\n", (UINT32)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) ));

	memset( afn_tbl, '\0', sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	afn_tbl->STAXY0.bit.STAX = M_CT_IM_IIP_AFNTBL_CALC_POS(D_IM_IIP_VGA_WIDTH /4);
	afn_tbl->STAXY0.bit.STAY = M_CT_IM_IIP_AFNTBL_CALC_POS(D_IM_IIP_VGA_LINES /4);
	afn_tbl->D_OUT_IN_0.bit.DXX = M_CT_IM_IIP_AFNTBL_CALC_POS(0.5F);	// X increment X=0.5
	afn_tbl->D_OUT_IN_0.bit.DXY = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// X increment Y=0.0
	afn_tbl->D_OUT_IN_0.bit.DYX = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// Y increment X=0.0
	afn_tbl->D_OUT_IN_0.bit.DYY = M_CT_IM_IIP_AFNTBL_CALC_POS(0.5F);	// Y increment Y=0.5
	afn_tbl->SZ.bit.AFNDHSZ = D_IM_IIP_VGA_WIDTH;
	afn_tbl->SZ.bit.AFNDVSZ = D_IM_IIP_VGA_LINES;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_unit_inf = 0x%x 0x%x\n", (UINT32)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) ));

	memset( afn_unit_inf, '\0', sizeof(*afn_unit_inf) );
	*afn_unit_inf = g_ct_im_iip_param_afn_base;
	afn_unit_inf->AFNTOPCNF.bit.WAITCONF = dst_portid;
	afn_unit_inf->PIXIDEF.bit.IPIXID = src_pixid;
	afn_unit_inf->AFNPA.word = (ULONG)afn_tbl;

	// Enable BI-Cubic
	afn_unit_inf->CALMETHOD.bit.ITMD = E_IM_IIP_PARAM_ITMD_BI_CUBIC;
	afn_unit_inf->CALMETHOD.bit.CUBSEL = E_IM_IIP_PARAM_CUBSEL_AFN0;
	afn_unit_inf->CALMETHOD.bit.SCUB = E_IM_IIP_PARAM_SCUB_0;
	// Enable Clipping (0~255)
	afn_unit_inf->OPMD.bit.OPMD0 = E_IM_IIP_PARAM_OPMD_ADD;
	afn_unit_inf->OPY.bit.OPBY = 0;		// +0
	afn_unit_inf->OPY.bit.OPAY = 0x10;	// x1.0
	afn_unit_inf->OPB.bit.OPBB = 0;		// +0
	afn_unit_inf->OPB.bit.OPAB = 0x10;	// x1.0
	afn_unit_inf->OPR.bit.OPBR = 0;		// +0
	afn_unit_inf->OPR.bit.OPAR = 0x10;	// x1.0
	afn_unit_inf->OPA.bit.OPBA = 0;		// +0
	afn_unit_inf->OPA.bit.OPAA = 0x10;	// x1.0
	afn_unit_inf->CLIPLVLY.bit.CLIPLVLYH = 0xFF;
	afn_unit_inf->CLIPLVLY.bit.CLIPLVLYL = 0;
	afn_unit_inf->CLIPLVLB.bit.CLIPLVLBH = 0xFF;
	afn_unit_inf->CLIPLVLB.bit.CLIPLVLBL = 0;
	afn_unit_inf->CLIPLVLR.bit.CLIPLVLRH = 0xFF;
	afn_unit_inf->CLIPLVLR.bit.CLIPLVLRL = 0;
	afn_unit_inf->CLIPLVLA.bit.CLIPLVLAH = 0xFF;
	afn_unit_inf->CLIPLVLA.bit.CLIPLVLAL = 0;

	afn_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	afn_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	afn_cfg.unit_param_addr = (ULONG)afn_unit_inf;
	afn_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#if 1
	Palladium_Set_Out_Localstack( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	Palladium_Set_Out_Localstack( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &afn_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	Ddim_Print(( "AFN[0]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL0.bit.HWEN
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
	// UnitINF, AFNtblダンプ
	Palladium_Set_Out_Localstack( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	Palladium_Set_Out_Localstack( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif


#if 0
	// dump UNITINF AFN[0]
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( src_unitid, gCtImIIP_Get_Unit_Param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF AFN[0] %u\n", param_bytes ));
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


// AFN Unit 3-1-X
INT32 CT_Im_IIP_Run_3_1( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-3-1-1
		case 1:
			return CT_Im_IIP_3_1_1();

		// Test-3-1-2
		case 2:
			return CT_Im_IIP_3_1_2();

		// Test-3-1-3
		case 3:
			return CT_Im_IIP_3_1_3();

		// Test-3-1-4
		case 4:
			return CT_Im_IIP_3_1_4();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


// AFN Unit 3-2-1 YCC422 U8 FullHD->Ribery VGA D-Zoom x1.0 (Resize & format comvert)
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_3_2_1: "
INT32 CT_Im_IIP_3_2_1( VOID )
{
	INT32						ercd;
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_0;		// AFN Unit input
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG			afn_cfg;
	T_IM_IIP_PARAM_AFN*			afn_unit_inf = &g_ct_im_iip_unit_param_afn;
	T_IM_IIP_PARAM_AFN_OPCOL_0*	afn_tbl = &g_ct_im_iip_afntbl;
	T_IM_IIP_PARAM_STS*			sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG			sl_cfg;
	UINT32						wait_factor_result = 0;
	const UINT32				wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL5END;
	const UINT32				unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_AFN0 | D_IM_IIP_PARAM_PLDUNIT_SL5;
	const UINT32				pixid_bitmask = E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID		src_unitid = E_IM_IIP_UNIT_ID_AFN0;
	const E_IM_IIP_UNIT_ID		dst_unitid = E_IM_IIP_UNIT_ID_SL5;
	const E_IM_IIP_PARAM_PORTID	src_portid = E_IM_IIP_PARAM_PORTID_AFN0;
	const E_IM_IIP_PARAM_PORTID	dst_portid = E_IM_IIP_PARAM_PORTID_SL5;
	const UINT32				src_pixid = 4;
	const UINT32				dst_pixid = 5;
	const UINT32				open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0.line_bytes.Y_G = D_IM_IIP_FULLHD_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cb_B = D_IM_IIP_FULLHD_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cr_R = D_IM_IIP_FULLHD_YCC422_U8_C_GLOBAL_WIDTH;	// dummy
	pixfmt_tbl_0.line_bytes.Alpha = D_IM_IIP_FULLHD_YCC422_U8_A_GLOBAL_WIDTH;	// dummy
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_YCC422_U8_Y;
	pixfmt_tbl_0.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_YCC422_U8_C;
	pixfmt_tbl_0.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_YCC422_U8_C;	// dummy
	pixfmt_tbl_0.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_YCC422_U8_A;	// dummy
	pixfmt_tbl_0.width = D_IM_IIP_FULLHD_WIDTH;
	pixfmt_tbl_0.lines = D_IM_IIP_FULLHD_LINES;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_16_9_RIBERY_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_16_9_RIBERY_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_16_9_RIBERY_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_VGA_16_9_RIBERY_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_VGA_16_9_RIBERY_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_VGA_16_9_RIBERY_U8_C;	// dummy
	pixfmt_tbl_1.addr.Alpha = (ULONG)NULL;	// dummy
	pixfmt_tbl_1.width = D_IM_IIP_VGA_16_9_WIDTH;
	pixfmt_tbl_1.lines = D_IM_IIP_VGA_16_9_LINES;
	pixfmt_tbl_1.pix_format = E_IM_IIP_PFMT_VIDEO;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_tbl = 0x%x 0x%x\n", (UINT32)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) ));

	memset( afn_tbl, '\0', sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	afn_tbl->STAXY0.bit.STAX = M_CT_IM_IIP_AFNTBL_CALC_POS(0);
	afn_tbl->STAXY0.bit.STAY = M_CT_IM_IIP_AFNTBL_CALC_POS(0);
	afn_tbl->D_OUT_IN_0.bit.DXX = M_CT_IM_IIP_AFNTBL_CALC_PIT( pixfmt_tbl_1.width, pixfmt_tbl_0.width );
	afn_tbl->D_OUT_IN_0.bit.DXY = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// X increment Y=0.0
	afn_tbl->D_OUT_IN_0.bit.DYX = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// Y increment X=0.0
	afn_tbl->D_OUT_IN_0.bit.DYY = M_CT_IM_IIP_AFNTBL_CALC_PIT( pixfmt_tbl_1.lines, pixfmt_tbl_0.lines );
	afn_tbl->SZ.bit.AFNDHSZ = pixfmt_tbl_1.width;
	afn_tbl->SZ.bit.AFNDVSZ = pixfmt_tbl_1.lines;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_unit_inf = 0x%x 0x%x\n", (UINT32)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) ));
	Ddim_Print(( D_IM_IIP_FUNC_NAME "STAX=0x%x STAY=0x%x\n", (INT32)afn_tbl->STAXY0.bit.STAX, (INT32)afn_tbl->STAXY0.bit.STAY ));

	memset( afn_unit_inf, '\0', sizeof(*afn_unit_inf) );
	*afn_unit_inf = g_ct_im_iip_param_afn_base;
	afn_unit_inf->AFNTOPCNF.bit.WAITCONF = dst_portid;
	afn_unit_inf->PIXIDEF.bit.IPIXID = src_pixid;
	afn_unit_inf->AFNPA.word = (ULONG)afn_tbl;

	afn_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	afn_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	afn_cfg.unit_param_addr = (ULONG)afn_unit_inf;
	afn_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;
	sl_unit_inf->BASE.SL_HSIZE.bit.HEND = pixfmt_tbl_1.width - 1;
	sl_unit_inf->BASE.SL_VSIZE.bit.VEND = pixfmt_tbl_1.lines - 1;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );


	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &afn_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	Ddim_Print(( "AFN[0]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "SL[2]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

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


// AFN Unit 3-2-2 YCC422 U8 FullHD->Ribery HD D-Zoom x1.0 (Resize & format comvert)
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_3_2_2: "
INT32 CT_Im_IIP_3_2_2( VOID )
{
	INT32						ercd;
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_0;		// AFN Unit input
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG			afn_cfg;
	T_IM_IIP_PARAM_AFN*			afn_unit_inf = &g_ct_im_iip_unit_param_afn;
	T_IM_IIP_PARAM_AFN_OPCOL_0*	afn_tbl = &g_ct_im_iip_afntbl;
	T_IM_IIP_PARAM_STS*			sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG			sl_cfg;
	UINT32						wait_factor_result = 0;
	const UINT32				wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL5END;
	const UINT32				unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_AFN0 | D_IM_IIP_PARAM_PLDUNIT_SL5;
	const UINT32				pixid_bitmask = E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID		src_unitid = E_IM_IIP_UNIT_ID_AFN0;
	const E_IM_IIP_UNIT_ID		dst_unitid = E_IM_IIP_UNIT_ID_SL5;
	const E_IM_IIP_PARAM_PORTID	src_portid = E_IM_IIP_PARAM_PORTID_AFN0;
	const E_IM_IIP_PARAM_PORTID	dst_portid = E_IM_IIP_PARAM_PORTID_SL5;
	const UINT32				src_pixid = 4;
	const UINT32				dst_pixid = 5;
	const UINT32				open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0.line_bytes.Y_G = D_IM_IIP_FULLHD_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cb_B = D_IM_IIP_FULLHD_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cr_R = D_IM_IIP_FULLHD_YCC422_U8_C_GLOBAL_WIDTH;	// dummy
	pixfmt_tbl_0.line_bytes.Alpha = D_IM_IIP_FULLHD_YCC422_U8_A_GLOBAL_WIDTH;	// dummy
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_YCC422_U8_Y;
	pixfmt_tbl_0.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_YCC422_U8_C;
	pixfmt_tbl_0.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_YCC422_U8_C;	// dummy
	pixfmt_tbl_0.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_YCC422_U8_A;	// dummy
	pixfmt_tbl_0.width = D_IM_IIP_FULLHD_WIDTH;
	pixfmt_tbl_0.lines = D_IM_IIP_FULLHD_LINES;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_HD_16_9_RIBERY_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_HD_16_9_RIBERY_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_HD_16_9_RIBERY_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_HD_16_9_RIBERY_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_HD_16_9_RIBERY_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_HD_16_9_RIBERY_U8_C;	// dummy
	pixfmt_tbl_1.addr.Alpha = (ULONG)NULL;	// dummy
	pixfmt_tbl_1.width = D_IM_IIP_HD_16_9_WIDTH;
	pixfmt_tbl_1.lines = D_IM_IIP_HD_16_9_LINES;
	pixfmt_tbl_1.pix_format = E_IM_IIP_PFMT_VIDEO;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_tbl = 0x%x 0x%x\n", (UINT32)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) ));

	memset( afn_tbl, '\0', sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	afn_tbl->STAXY0.bit.STAX = M_CT_IM_IIP_AFNTBL_CALC_POS(0);
	afn_tbl->STAXY0.bit.STAY = M_CT_IM_IIP_AFNTBL_CALC_POS(0);
	afn_tbl->D_OUT_IN_0.bit.DXX = M_CT_IM_IIP_AFNTBL_CALC_PIT( pixfmt_tbl_1.width, pixfmt_tbl_0.width );
	afn_tbl->D_OUT_IN_0.bit.DXY = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// X increment Y=0.0
	afn_tbl->D_OUT_IN_0.bit.DYX = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// Y increment X=0.0
	afn_tbl->D_OUT_IN_0.bit.DYY = M_CT_IM_IIP_AFNTBL_CALC_PIT( pixfmt_tbl_1.lines, pixfmt_tbl_0.lines );
	afn_tbl->SZ.bit.AFNDHSZ = pixfmt_tbl_1.width;
	afn_tbl->SZ.bit.AFNDVSZ = pixfmt_tbl_1.lines;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_unit_inf = 0x%x 0x%x\n", (UINT32)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) ));
	Ddim_Print(( D_IM_IIP_FUNC_NAME "STAX=0x%x STAY=0x%x\n", (INT32)afn_tbl->STAXY0.bit.STAX, (INT32)afn_tbl->STAXY0.bit.STAY ));

	memset( afn_unit_inf, '\0', sizeof(*afn_unit_inf) );
	*afn_unit_inf = g_ct_im_iip_param_afn_base;
	afn_unit_inf->AFNTOPCNF.bit.WAITCONF = dst_portid;
	afn_unit_inf->PIXIDEF.bit.IPIXID = src_pixid;
	afn_unit_inf->AFNPA.word = (ULONG)afn_tbl;

	afn_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	afn_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	afn_cfg.unit_param_addr = (ULONG)afn_unit_inf;
	afn_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;
	sl_unit_inf->BASE.SL_HSIZE.bit.HEND = pixfmt_tbl_1.width - 1;
	sl_unit_inf->BASE.SL_VSIZE.bit.VEND = pixfmt_tbl_1.lines - 1;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );


	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &afn_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	Ddim_Print(( "AFN[0]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "SL[2]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

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


// AFN Unit 3-2-3 YCC422 U8 FullHD->Ribery HD D-Zoom x1.0 (Resize & format comvert)
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_3_2_3: "
INT32 CT_Im_IIP_3_2_3( VOID )
{
	INT32						ercd;
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_0;		// AFN Unit input
	T_IM_IIP_PIXFMTTBL			pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG			afn_cfg;
	T_IM_IIP_PARAM_AFN*			afn_unit_inf = &g_ct_im_iip_unit_param_afn;
	T_IM_IIP_PARAM_AFN_OPCOL_0*	afn_tbl = &g_ct_im_iip_afntbl;
	T_IM_IIP_PARAM_STS*			sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG			sl_cfg;
	UINT32						wait_factor_result = 0;
	const UINT32				wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL5END;
	const UINT32				unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_AFN0 | D_IM_IIP_PARAM_PLDUNIT_SL5;
	const UINT32				pixid_bitmask = E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID		src_unitid = E_IM_IIP_UNIT_ID_AFN0;
	const E_IM_IIP_UNIT_ID		dst_unitid = E_IM_IIP_UNIT_ID_SL5;
	const E_IM_IIP_PARAM_PORTID	src_portid = E_IM_IIP_PARAM_PORTID_AFN0;
	const E_IM_IIP_PARAM_PORTID	dst_portid = E_IM_IIP_PARAM_PORTID_SL5;
	const UINT32				src_pixid = 4;
	const UINT32				dst_pixid = 5;
	const UINT32				open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0.line_bytes.Y_G = D_IM_IIP_FULLHD_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cb_B = D_IM_IIP_FULLHD_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0.line_bytes.Cr_R = D_IM_IIP_FULLHD_YCC422_U8_C_GLOBAL_WIDTH;	// dummy
	pixfmt_tbl_0.line_bytes.Alpha = D_IM_IIP_FULLHD_YCC422_U8_A_GLOBAL_WIDTH;	// dummy
	pixfmt_tbl_0.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_YCC422_U8_Y;
	pixfmt_tbl_0.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_YCC422_U8_C;
	pixfmt_tbl_0.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_YCC422_U8_C;	// dummy
	pixfmt_tbl_0.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_FULLHD_YCC422_U8_A;	// dummy
	pixfmt_tbl_0.width = D_IM_IIP_FULLHD_WIDTH;
	pixfmt_tbl_0.lines = D_IM_IIP_FULLHD_LINES;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_FULLHD_RIBERY_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_FULLHD_RIBERY_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_FULLHD_RIBERY_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = 0;	// dummy
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_FULLHD_RIBERY_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_FULLHD_RIBERY_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_FULLHD_RIBERY_U8_C;	// dummy
	pixfmt_tbl_1.addr.Alpha = (ULONG)NULL;	// dummy
	pixfmt_tbl_1.width = D_IM_IIP_FULLHD_WIDTH;
	pixfmt_tbl_1.lines = D_IM_IIP_FULLHD_LINES;
	pixfmt_tbl_1.pix_format = E_IM_IIP_PFMT_VIDEO;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_tbl = 0x%x 0x%x\n", (UINT32)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) ));

	memset( afn_tbl, '\0', sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	afn_tbl->STAXY0.bit.STAX = M_CT_IM_IIP_AFNTBL_CALC_POS(0);
	afn_tbl->STAXY0.bit.STAY = M_CT_IM_IIP_AFNTBL_CALC_POS(0);
	afn_tbl->D_OUT_IN_0.bit.DXX = M_CT_IM_IIP_AFNTBL_CALC_PIT( pixfmt_tbl_1.width, pixfmt_tbl_0.width );
	afn_tbl->D_OUT_IN_0.bit.DXY = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// X increment Y=0.0
	afn_tbl->D_OUT_IN_0.bit.DYX = M_CT_IM_IIP_AFNTBL_CALC_POS(0.0F);	// Y increment X=0.0
	afn_tbl->D_OUT_IN_0.bit.DYY = M_CT_IM_IIP_AFNTBL_CALC_PIT( pixfmt_tbl_1.lines, pixfmt_tbl_0.lines );
	afn_tbl->SZ.bit.AFNDHSZ = pixfmt_tbl_1.width;
	afn_tbl->SZ.bit.AFNDVSZ = pixfmt_tbl_1.lines;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "afn_unit_inf = 0x%x 0x%x\n", (UINT32)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) ));
	Ddim_Print(( D_IM_IIP_FUNC_NAME "STAX=0x%x STAY=0x%x\n", (INT32)afn_tbl->STAXY0.bit.STAX, (INT32)afn_tbl->STAXY0.bit.STAY ));

	memset( afn_unit_inf, '\0', sizeof(*afn_unit_inf) );
	*afn_unit_inf = g_ct_im_iip_param_afn_base;
	afn_unit_inf->AFNTOPCNF.bit.WAITCONF = dst_portid;
	afn_unit_inf->PIXIDEF.bit.IPIXID = src_pixid;
	afn_unit_inf->AFNPA.word = (ULONG)afn_tbl;

	afn_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	afn_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	afn_cfg.unit_param_addr = (ULONG)afn_unit_inf;
	afn_cfg.load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "sl_unit_inf = 0x%x 0x%x\n", (UINT32)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) ));

	memset( sl_unit_inf, '\0', sizeof(*sl_unit_inf) );
	*sl_unit_inf = g_ct_im_iip_param_sts_base;
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = src_portid;
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;
	sl_unit_inf->BASE.SL_HSIZE.bit.HEND = pixfmt_tbl_1.width - 1;
	sl_unit_inf->BASE.SL_VSIZE.bit.VEND = pixfmt_tbl_1.lines - 1;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_tbl, sizeof(T_IM_IIP_PARAM_AFN_OPCOL_0) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)afn_unit_inf, sizeof(T_IM_IIP_PARAM_AFN) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );


	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid, &pixfmt_tbl_0 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid, &afn_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Im_IIP_On_Pclk();
	Ddim_Print(( "AFN[0]: PADRS=0x%x HWEN=%u\n",
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_AFN0.UNITINFTBL0.bit.HWEN
			   ));
	Ddim_Print(( "SL[2]: PADRS=0x%x HWEN=%u PLDUNIT=0x%08x%08x\n",
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL2.bit.PADRS,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL0.bit.HWEN,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_HI,
				(UINT32)IO_IIP.UNITINFTBL_SL2.UNITINFTBL1.bit.PLDUNIT_LO
			   ));
	Im_IIP_Off_Pclk();

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


// AFN Unit 3-2-X
INT32 CT_Im_IIP_Run_3_2( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-3-2-1
		case 1:
			return CT_Im_IIP_3_2_1();

		// Test-3-2-2
		case 2:
			return CT_Im_IIP_3_2_2();

		// Test-3-2-3
		case 3:
			return CT_Im_IIP_3_2_3();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#endif //CO_CT_IM_IIP_DISABLE

