/**
 * @file		ct_im_iip_blend.c
 * @brief		IIP BLEND unit ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// CT header
#include "ct_im_iip.h"
#include "ct_im_iip_blend.h"

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

// BLEND Unit 6-1-1 YCC422 U8 VGA[2]->VGA
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_6_1_1: "
INT32 CT_Im_IIP_6_1_1( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0[2];	// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg[2];
	T_IM_IIP_UNIT_CFG				blend_cfg;
	T_IM_IIP_PARAM_1DL*				oned_unit_inf[2] = { &g_ct_im_iip_unit_param_1dl0, &g_ct_im_iip_unit_param_1dl1 };
	T_IM_IIP_PARAM_BLEND*			blend_unit_inf = &g_ct_im_iip_unit_param_blend;
	T_IM_IIP_PARAM_STS*				sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG				sl_cfg;
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END;
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_LD2 | D_IM_IIP_PARAM_PLDUNIT_BLEND0 | D_IM_IIP_PARAM_PLDUNIT_SL2;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_3 | E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID			src_unitid[2] = { E_IM_IIP_UNIT_ID_LD1, E_IM_IIP_UNIT_ID_LD2 };
	const E_IM_IIP_UNIT_ID			filter_unitid = E_IM_IIP_UNIT_ID_BLEND0;
	const E_IM_IIP_UNIT_ID			dst_unitid = E_IM_IIP_UNIT_ID_SL2;
	const E_IM_IIP_PARAM_PORTID		src_portid[2] = { E_IM_IIP_PARAM_PORTID_LD1, E_IM_IIP_PARAM_PORTID_LD2 };
	const E_IM_IIP_PARAM_PORTID		filter_portid[2] = { E_IM_IIP_PARAM_PORTID_BLEND0_P0, E_IM_IIP_PARAM_PORTID_BLEND0_P1 };
	const E_IM_IIP_PARAM_PORTID		dst_portid = E_IM_IIP_PARAM_PORTID_SL2;
	const UINT32					src_pixid[2] = { 3, 4 };
	const UINT32					dst_pixid = 5;
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0[0] = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0[0].line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y;
	pixfmt_tbl_0[0].addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0[0].addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C;
	pixfmt_tbl_0[0].addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A;

	pixfmt_tbl_0[1] = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0[1].line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_Y;
	pixfmt_tbl_0[1].addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_C;
	pixfmt_tbl_0[1].addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_C;
	pixfmt_tbl_0[1].addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_A;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH;
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C;
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf[0] = 0x%x 0x%x\n", (UINT32)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[0], '\0', sizeof(*oned_unit_inf[0]) );
	*oned_unit_inf[0] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[0]->LD_TOPCNF0.bit.WAITCONF = filter_portid[0];
	oned_unit_inf[0]->PIXIDDEF.bit.IPIXID = src_pixid[0];

	oned_cfg[0].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[0].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[0].unit_param_addr = (ULONG)oned_unit_inf[0];
	oned_cfg[0].load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf[1] = 0x%x 0x%x\n", (UINT32)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[1], '\0', sizeof(*oned_unit_inf[1]) );
	*oned_unit_inf[1] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[1]->LD_TOPCNF0.bit.WAITCONF = filter_portid[1];
	oned_unit_inf[1]->PIXIDDEF.bit.IPIXID = src_pixid[1];

	oned_cfg[1].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[1].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[1].unit_param_addr = (ULONG)oned_unit_inf[1];
	oned_cfg[1].load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "blend_unit_inf = 0x%x 0x%x\n", (UINT32)blend_unit_inf, sizeof(T_IM_IIP_PARAM_BLEND) ));

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
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = filter_portid[0];
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)blend_unit_inf, sizeof(T_IM_IIP_PARAM_BLEND) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );

// ### REMOVE_RELEASE BEGIN
#ifdef CO_PT_ENABLE
#if 0
	Palladium_Set_Out_Localstack( (ULONG)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) );
	Palladium_Set_Out_Localstack( (ULONG)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) );
	Palladium_Set_Out_Localstack( (ULONG)blend_unit_inf, sizeof(T_IM_IIP_PARAM_BLEND) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif
#endif //CO_PT_ENABLE
// ### REMOVE_RELEASE END

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[0], &pixfmt_tbl_0[0] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[1], &pixfmt_tbl_0[1] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[0], &oned_cfg[0] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[1], &oned_cfg[1] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( filter_unitid, &blend_cfg );
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
	ercd = Im_IIP_Start_SWTRG( src_unitid[0], D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start4\n" ));
	ercd = Im_IIP_Start_SWTRG( src_unitid[1], D_IM_IIP_SWTRG_ON );
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
	Palladium_Set_Out_Localstack( (ULONG)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) );
	Palladium_Set_Out_Localstack( (ULONG)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) );
	Palladium_Set_Out_Localstack( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );
#endif


#if 0
	// dump UNITINF ONED[1]
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( src_unitid[0], g_get_unit_param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF ONED[1] %u\n", param_bytes ));
			ct_im_iip_clean_l1l2_dcache_addr( (ULONG)g_get_unit_param, param_bytes );
			Palladium_Set_Out_Localstack( (ULONG)g_get_unit_param, param_bytes );
		}
	}
#endif


#if 0
	// dump UNITINF ONED[2]
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( src_unitid[1], g_get_unit_param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF ONED[2] %u\n", param_bytes ));
			ct_im_iip_clean_l1l2_dcache_addr( (ULONG)g_get_unit_param, param_bytes );
			Palladium_Set_Out_Localstack( (ULONG)g_get_unit_param, param_bytes );
		}
	}
#endif


#if 0
	// dump UNITINF BLEND
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( filter_unitid, g_get_unit_param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF BLEND %u\n", param_bytes ));
			ct_im_iip_clean_l1l2_dcache_addr( (ULONG)g_get_unit_param, param_bytes );
			Palladium_Set_Out_Localstack( (ULONG)g_get_unit_param, param_bytes );
		}
	}
#endif


#if 0
	// dump UNITINF SL[2]
	{
		UINT32 param_bytes;
		ercd = Im_IIP_Get_UNIT_PARAM( dst_unitid, g_get_unit_param, &param_bytes );
		Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = %d bytes = %u\n", ercd, param_bytes ));
		if( ercd == D_IM_IIP_OK ) {
			Ddim_Print(( D_IM_IIP_FUNC_NAME "dump UNITINF SL[2] %u\n", param_bytes ));
			ct_im_iip_clean_l1l2_dcache_addr( (ULONG)g_get_unit_param, param_bytes );
			Palladium_Set_Out_Localstack( (ULONG)g_get_unit_param, param_bytes );
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


// BLEND Unit 6-1-X
INT32 CT_Im_IIP_Run_6_1( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-6-1-1
		case 1:
			return CT_Im_IIP_6_1_1();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


// BLEND Unit 6-2-1 YCC422 U14 VGA[2]->VGA
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_6_2_1: "
INT32 CT_Im_IIP_6_2_1( VOID )
{
	INT32							ercd;
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_0[2];	// 1DL Unit input
	T_IM_IIP_PIXFMTTBL				pixfmt_tbl_1;		// SL Unit output
	T_IM_IIP_UNIT_CFG				oned_cfg[2];
	T_IM_IIP_UNIT_CFG				blend_cfg;
	T_IM_IIP_PARAM_1DL*				oned_unit_inf[2] = { &g_ct_im_iip_unit_param_1dl0, &g_ct_im_iip_unit_param_1dl1 };
	T_IM_IIP_PARAM_BLEND*			blend_unit_inf = &g_ct_im_iip_unit_param_blend;
	T_IM_IIP_PARAM_STS*				sl_unit_inf = &g_ct_im_iip_unit_param_sl0;
	T_IM_IIP_UNIT_CFG				sl_cfg;
	UINT32							wait_factor_result = 0;
	const UINT32					wait_factor = D_IM_IIP_INT_FACTOR_AXIERR | D_IM_IIP_INT_FACTOR_SL2END;
	const UINT32					unitid_bitmask = D_IM_IIP_PARAM_PLDUNIT_LD1 | D_IM_IIP_PARAM_PLDUNIT_LD2 | D_IM_IIP_PARAM_PLDUNIT_BLEND0 | D_IM_IIP_PARAM_PLDUNIT_SL2;
	const UINT32					pixid_bitmask = E_IM_IIP_PIXID_3 | E_IM_IIP_PIXID_4 | E_IM_IIP_PIXID_5;
	const E_IM_IIP_UNIT_ID			src_unitid[2] = { E_IM_IIP_UNIT_ID_LD1, E_IM_IIP_UNIT_ID_LD2 };
	const E_IM_IIP_UNIT_ID			filter_unitid = E_IM_IIP_UNIT_ID_BLEND0;
	const E_IM_IIP_UNIT_ID			dst_unitid = E_IM_IIP_UNIT_ID_SL2;
	const E_IM_IIP_PARAM_PORTID		src_portid[2] = { E_IM_IIP_PARAM_PORTID_LD1, E_IM_IIP_PARAM_PORTID_LD2 };
	const E_IM_IIP_PARAM_PORTID		filter_portid[2] = { E_IM_IIP_PARAM_PORTID_BLEND0_P0, E_IM_IIP_PARAM_PORTID_BLEND0_P1 };
	const E_IM_IIP_PARAM_PORTID		dst_portid = E_IM_IIP_PARAM_PORTID_SL2;
	const UINT32					src_pixid[2] = { 3, 4 };
	const UINT32					dst_pixid = 5;
	const UINT32					open_res_bitmask = E_IM_IIP_OPEN_RES_CACHE0;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = Im_IIP_Open_SWTRG( unitid_bitmask, pixid_bitmask, open_res_bitmask, D_CT_IM_IIP_OPEN_TIMEOUT_MSEC );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));
	if( ercd != D_IM_IIP_OK ) {
		return ercd;
	}

	pixfmt_tbl_0[0] = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0[0].line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U16_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U16_A_GLOBAL_WIDTH;
	pixfmt_tbl_0[0].addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U16_Y;
	pixfmt_tbl_0[0].addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U16_C;
	pixfmt_tbl_0[0].addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U16_C;
	pixfmt_tbl_0[0].addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U16_A;
	pixfmt_tbl_0[0].pix_depth = D_IM_IIP_PDEPTH_16BITS;

	pixfmt_tbl_0[1] = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_0[1].line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U16_Y_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U16_A_GLOBAL_WIDTH;
	pixfmt_tbl_0[1].addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U16_Y;
	pixfmt_tbl_0[1].addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U16_C;
	pixfmt_tbl_0[1].addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U16_C;
	pixfmt_tbl_0[1].addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U16_A;
	pixfmt_tbl_0[1].pix_depth = D_IM_IIP_PDEPTH_16BITS;

	pixfmt_tbl_1 = g_ct_im_iip_pixfmttbl_base;
	pixfmt_tbl_1.line_bytes.Y_G = D_IM_IIP_VGA_YCC422_U16_Y_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cb_B = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Cr_R = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH;
	pixfmt_tbl_1.line_bytes.Alpha = D_IM_IIP_VGA_YCC422_U16_A_GLOBAL_WIDTH;
	pixfmt_tbl_1.addr.Y_G = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U16_Y;
	pixfmt_tbl_1.addr.Cb_B = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U16_C;
	pixfmt_tbl_1.addr.Cr_R = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U16_C;
	pixfmt_tbl_1.addr.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U16_A;
	pixfmt_tbl_1.pix_depth = D_IM_IIP_PDEPTH_16BITS;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf[0] = 0x%x 0x%x\n", (UINT32)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[0], '\0', sizeof(*oned_unit_inf[0]) );
	*oned_unit_inf[0] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[0]->LD_TOPCNF0.bit.WAITCONF = filter_portid[0];
	oned_unit_inf[0]->PIXIDDEF.bit.IPIXID = src_pixid[0];

	oned_cfg[0].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[0].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[0].unit_param_addr = (ULONG)oned_unit_inf[0];
	oned_cfg[0].load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "oned_unit_inf[1] = 0x%x 0x%x\n", (UINT32)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) ));

	memset( oned_unit_inf[1], '\0', sizeof(*oned_unit_inf[1]) );
	*oned_unit_inf[1] = g_ct_im_iip_param_1dl_base;
	oned_unit_inf[1]->LD_TOPCNF0.bit.WAITCONF = filter_portid[1];
	oned_unit_inf[1]->PIXIDDEF.bit.IPIXID = src_pixid[1];

	oned_cfg[1].unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	oned_cfg[1].chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	oned_cfg[1].unit_param_addr = (ULONG)oned_unit_inf[1];
	oned_cfg[1].load_unit_param_flag = 0;

	Ddim_Print(( D_IM_IIP_FUNC_NAME "blend_unit_inf = 0x%x 0x%x\n", (UINT32)blend_unit_inf, sizeof(T_IM_IIP_PARAM_BLEND) ));

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
	sl_unit_inf->BASE.SL_TOPCNF0.bit.DATACONF = filter_portid[0];
	sl_unit_inf->BASE.PIXIDDEF.bit.OPIXID = dst_pixid;


	sl_cfg.unit_ctrl = D_IM_IIP_HW_CTRL_SWTRG;
	sl_cfg.chain_enable = D_IM_IIP_PLDUNIT_CHAIN_DISABLE;
	sl_cfg.unit_param_addr = (ULONG)sl_unit_inf;
	sl_cfg.load_unit_param_flag = unitid_bitmask;

	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[0], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)oned_unit_inf[1], sizeof(T_IM_IIP_PARAM_1DL) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)blend_unit_inf, sizeof(T_IM_IIP_PARAM_BLEND) );
	ct_im_iip_clean_l1l2_dcache_addr( (ULONG)sl_unit_inf, sizeof(T_IM_IIP_PARAM_STS) );

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[0], &pixfmt_tbl_0[0] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( src_pixid[1], &pixfmt_tbl_0[1] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_PIXFMTTBL( dst_pixid, &pixfmt_tbl_1 );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[0], &oned_cfg[0] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( src_unitid[1], &oned_cfg[1] );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( filter_unitid, &blend_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Ctrl_SWTRG_Unit( dst_unitid, &sl_cfg );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	ercd = Im_IIP_Set_Interrupt( wait_factor, D_IM_IIP_ENABLE_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Dd_ARM_Dmb_Pou();

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start1\n" ));
	ercd = Im_IIP_Start_SWTRG( dst_unitid, D_IM_IIP_SWTRG_ON );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "ercd = 0x%x\n", ercd ));

	Ddim_Print(( D_IM_IIP_FUNC_NAME "Start2\n" ));
	ercd = Im_IIP_Start_SWTRG( filter_unitid, D_IM_IIP_SWTRG_ON );
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

// BLEND Unit 6-2-X
INT32 CT_Im_IIP_Run_6_2( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-6-2-1
		case 1:
			return CT_Im_IIP_6_2_1();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#endif //CO_CT_IM_IIP_DISABLE

