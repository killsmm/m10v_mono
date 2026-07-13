/**
 * @file		palladium_test_iip.c
 * @brief		palladium test iip code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "palladium_test_iip.h"
#include "ct_im_iip.h"
#include "im_iip.h"
//#include "im_b2y.h"
#include "dd_gic.h"
#include "dd_top.h"

#include "driver_common.h"
#include "palladium_test.h"

// MILB register header
#include "jdsiip.h"

#include <string.h>

#define D_IM_IIP_CLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP3())
#define D_IM_IIP_HCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP3())
#define D_IM_IIP_PCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP3())
#define D_IM_IIP_ICLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP3())
#define D_IM_B2Y_CLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP11())
#define D_IM_B2Y_HCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP11())
#define D_IM_B2Y_PCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP11())
#define D_IM_B2Y_ICLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP11())

#define D_IM_IIP_CLK_REG_BIT	D_DD_TOP_IIPCK_BIT
#define D_IM_IIP_PCLK_REG_BIT	D_DD_TOP_IIPAP_BIT
#define D_IM_IIP_HCLK_REG_BIT	D_DD_TOP_IIPAH_BIT
#define D_IM_IIP_ICLK_REG_BIT	D_DD_TOP_IIPAX_BIT
#define D_IM_B2Y_CLK_REG_BIT	D_DD_TOP_R2Y1CK_BIT	// bit4
#define D_IM_B2Y_HCLK_REG_BIT	D_DD_TOP_R2Y1AH_BIT	// bit5
#define D_IM_B2Y_PCLK_REG_BIT	D_DD_TOP_R2Y1AP_BIT	// bit6
#define D_IM_B2Y_ICLK_REG_BIT	D_DD_TOP_R2Y1AX_BIT	// bit7

// Image SRAM ring buffer address.
#define D_PT_IM_IIP_IMG_SRAM_ADDR0	(0x30000000UL)
#define D_PT_IM_IIP_IMG_SRAM_ADDR1	(0x34000000UL)
#define D_PT_IM_IIP_IMG_SRAM_ADDR2	(0x38000000UL)
#define D_PT_IM_IIP_IMG_SRAM_ADDR3	(0x3C000000UL)


static const CHAR errmsg_skip[] = "*** IIP test skip %u-%u-%u\n";


static VOID pt_im_iip_enable_gic( VOID )
{
//	Dd_GIC_Init();
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH0_INT, 1, D_DD_GIC_PRI30, 1 );		// cpu=0, intid=118, enable, pri=30
	Dd_GIC_Ctrl( E_DD_GIC_INTID_IIP_INT,       1, D_DD_GIC_PRI30, 1 );		// int enable
//	Dd_GIC_Ctrl( E_DD_GIC_INTID_B2Y_INT,       1, D_DD_GIC_PRI30, 1 );		// int enable
}


static VOID pt_im_iip_start_clock( VOID )
{
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_PCLOCK) || defined(CO_ACT_HCLOCK)
#else
	UCHAR im_iip_clk_ctrl_cnt = 0;
	UCHAR im_iip_hclk_ctrl_cnt = 0;
	UCHAR im_iip_pclk_ctrl_cnt = 0;
	UCHAR im_iip_iclk_ctrl_cnt = 0;
	UCHAR im_b2y_clk_ctrl_cnt = 0;
	UCHAR im_b2y_hclk_ctrl_cnt = 0;
	UCHAR im_b2y_pclk_ctrl_cnt = 0;
	UCHAR im_b2y_iclk_ctrl_cnt = 0;

	Dd_Top_Start_Clock( &im_iip_pclk_ctrl_cnt, D_IM_IIP_PCLK_REG_ADDR, ~D_IM_IIP_PCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_iip_hclk_ctrl_cnt, D_IM_IIP_HCLK_REG_ADDR, ~D_IM_IIP_HCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_iip_iclk_ctrl_cnt, D_IM_IIP_ICLK_REG_ADDR, ~D_IM_IIP_ICLK_REG_BIT );
	Dd_Top_Start_Clock( &im_iip_clk_ctrl_cnt,  D_IM_IIP_CLK_REG_ADDR,  ~D_IM_IIP_CLK_REG_BIT );

	Dd_Top_Start_Clock( &im_b2y_pclk_ctrl_cnt, D_IM_B2Y_PCLK_REG_ADDR, ~D_IM_B2Y_PCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_b2y_hclk_ctrl_cnt, D_IM_B2Y_HCLK_REG_ADDR, ~D_IM_B2Y_HCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_b2y_iclk_ctrl_cnt, D_IM_B2Y_ICLK_REG_ADDR, ~D_IM_B2Y_ICLK_REG_BIT );
	Dd_Top_Start_Clock( &im_b2y_clk_ctrl_cnt,  D_IM_B2Y_CLK_REG_ADDR,  ~D_IM_B2Y_CLK_REG_BIT );
#endif
}


#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "1_1_4"
static VOID pt_im_iip_run_1_1_4( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	static const UCHAR	no_3rd_tbl[] = {
		4, // 5 same as 4
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );


	for( loopcnt = 0; loopcnt < (sizeof(no_3rd_tbl) / sizeof(no_3rd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* IIP begin CT_Im_Iip_%u_%u_%u\n", ct_no_1st, ct_no_2nd, no_3rd_tbl[loopcnt] ));
		CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, no_3rd_tbl[loopcnt] );
		Ddim_Print(( "* IIP end CT_Im_Iip_%u_%u_%u\n", ct_no_1st, ct_no_2nd, no_3rd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "1_1_13"
static VOID pt_im_iip_run_1_1_13( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	static const UCHAR	no_3rd_tbl[] = {
		13, 16, 17, 18, 35, 36, 39, 40, // 41 same as 39
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );


	for( loopcnt = 0; loopcnt < (sizeof(no_3rd_tbl) / sizeof(no_3rd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* IIP begin CT_Im_Iip_%u_%u_%u\n", ct_no_1st, ct_no_2nd, no_3rd_tbl[loopcnt] ));
		CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, no_3rd_tbl[loopcnt] );
		Ddim_Print(( "* IIP end CT_Im_Iip_%u_%u_%u\n", ct_no_1st, ct_no_2nd, no_3rd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_1_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 4:
			pt_im_iip_run_1_1_4( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 13:
			pt_im_iip_run_1_1_13( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "2_1_x"
static VOID pt_im_iip_run_2_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	static const UCHAR	no_3rd_tbl[] = {
		1, 2, 3,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );


	for( loopcnt = 0; loopcnt < (sizeof(no_3rd_tbl) / sizeof(no_3rd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* IIP begin CT_Im_Iip_%u_%u_%u\n", ct_no_1st, ct_no_2nd, no_3rd_tbl[loopcnt] ));
		CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, no_3rd_tbl[loopcnt] );
		Ddim_Print(( "* IIP end CT_Im_Iip_%u_%u_%u\n", ct_no_1st, ct_no_2nd, no_3rd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// Im_IIP_Stop 2-2-1
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "2_2_1"
static VOID pt_im_iip_do_test_2_2_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// Im_IIP_Stop 2-2-2
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "2_2_2"
static VOID pt_im_iip_do_test_2_2_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_2_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_2_2_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_2_2_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// Im_IIP_Stop 2-4-1
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "2_4_1"
static VOID pt_im_iip_do_test_2_4_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_2_4( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_2_4_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// B2Y-IIP link 2-5-1, AFN Unit YCC422 U8 VGA->VGA D-Zoom x1.0 (simple copy)
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "2_5_1"
static VOID pt_im_iip_do_test_2_5_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_RAW16BIT_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_2_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );
#ifndef CO_DEBUG_ON_PC
	memcpy( (VOID*)D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_Y,  (VOID*)D_PT_IM_IIP_IMG_SRAM_ADDR0, D_IM_IIP_VGA_YCC422_U8_BYTES );	// Copy Y
	DDIM_User_L1l2cache_Clean_Addr( D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_Y, D_IM_IIP_VGA_YCC422_U8_BYTES );
	memcpy( (VOID*)D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_C, (VOID*)D_PT_IM_IIP_IMG_SRAM_ADDR1, D_IM_IIP_VGA_YCC422_U8_BYTES );	// Copy CbCr
	DDIM_User_L1l2cache_Clean_Addr( D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_C, D_IM_IIP_VGA_YCC422_U8_BYTES /2 );
#endif
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// IIP-IIP link 2-5-2, FRECT->AFN Unit YCC420 U8 FullHD->FullHD D-Zoom x1.0 (simple copy)
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "2_5_2"
static VOID pt_im_iip_do_test_2_5_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_FULLHD_YCC420_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_2_TOP, D_IM_IIP_FULLHD_YCC420_U8_BYTES );
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_FULLHD_YCC420_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// IIP-IIP link 2-5-3, FRECT[0,1]->AFN Unit YCC420 U8 FullHD->FullHD D-Zoom x1.0 (simple copy)
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "2_5_3"
static VOID pt_im_iip_do_test_2_5_3( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_FULLHD_VIS_YCC420_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_2_TOP, D_IM_IIP_FULLHD_YCC420_U8_BYTES );	// afn
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_FULLHD_YCC420_U8_BYTES );	// frect0
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_3_TOP, D_IM_IIP_FULLHD_YCC420_U8_BYTES );	// frect1

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// IIP-IIP link 2-5-4, FRECT[0,1] YCC420 U8 FullHD->1920x128
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "2_5_4"
static VOID pt_im_iip_do_test_2_5_4( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_FULLHD_VIS_YCC420_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_2_TOP, D_IM_IIP_FULLHD_YCC420_U8_BYTES );	// afn
#ifndef CO_DEBUG_ON_PC
	memcpy( (VOID*)D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_Y,  (VOID*)D_PT_IM_IIP_IMG_SRAM_ADDR0, 0x40000 );	// Copy Y 256kB
	DDIM_User_L1l2cache_Clean_Addr( D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_Y, 0x40000 );
	memcpy( (VOID*)D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_C, (VOID*)D_PT_IM_IIP_IMG_SRAM_ADDR1, 0x20000 );	// Copy CbCr 128kB
	DDIM_User_L1l2cache_Clean_Addr( D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_C, 0x20000 );
#endif

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// IIP-IIP link 2-5-5, AFN->LD Unit YCC420 U8 FullHD->FullHD D-Zoom x1.0 (simple copy)
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "2_5_5"
static VOID pt_im_iip_do_test_2_5_5( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_FULLHD_YCC420_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_2_TOP, D_IM_IIP_FULLHD_YCC420_U8_BYTES );
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_FULLHD_YCC420_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_2_5( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
//	Im_B2Y_Init();

	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_2_5_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_2_5_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 3:
			pt_im_iip_do_test_2_5_3( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 4:
			pt_im_iip_do_test_2_5_4( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 5:
			pt_im_iip_do_test_2_5_5( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// UNIT register dump 2-6-1
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "2_6_1"
static VOID pt_im_iip_do_test_2_6_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, 65536 );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_2_6( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_2_6_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// UNIT register dump 2-7-1
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "2_7_1"
static VOID pt_im_iip_do_test_2_7_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_2_7( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_2_7_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// AFN Unit 3-1-1 YCC422 U8 VGA->VGA D-Zoom x1.0 (simple copy)
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "3_1_1"
static VOID pt_im_iip_do_test_3_1_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

#if 1
	// 入出力エリアをFill
#ifndef CO_DEBUG_ON_PC
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_0_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_1_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
#endif //CO_DEBUG_ON_PC
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// AFN Unit 3-1-2 YCC422 U8 VGA->VGA D-Zoom x2.0
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "3_1_2"
static VOID pt_im_iip_do_test_3_1_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
#ifndef CO_DEBUG_ON_PC
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_0_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_1_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
#endif //CO_DEBUG_ON_PC
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// AFN Unit 3-1-3 Plane U16(valid 14bit) VGA->VGA D-Zoom x2.0
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "3_1_3"
static VOID pt_im_iip_do_test_3_1_3( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
#ifndef CO_DEBUG_ON_PC
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_0_TOP, '\xee', D_IM_IIP_VGA_PLANE14BIT_BYTES );
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_1_TOP, '\xee', D_IM_IIP_VGA_PLANE14BIT_BYTES );
#endif //CO_DEBUG_ON_PC
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_PLANE14BIT_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_PLANE14BIT_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// AFN Unit 3-1-4 YCC422 U8 VGA->VGA D-Zoom x2.0 BI-Cubic
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "3_1_4"
static VOID pt_im_iip_do_test_3_1_4( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

#if 1
	// 入出力エリアをFill
#ifndef CO_DEBUG_ON_PC
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_0_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_1_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
	DDIM_User_L1l2cache_Clean_Addr( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );
	DDIM_User_L1l2cache_Clean_Addr( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );
#endif //CO_DEBUG_ON_PC
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_3_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_3_1_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_3_1_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 3:
			pt_im_iip_do_test_3_1_3( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 4:
			pt_im_iip_do_test_3_1_4( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}

}


// AFN Unit 3-2-1 YCC422 U8 FullHD->Ribery VGA D-Zoom x1.0 (Resize & format comvert)
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "3_2_1"
static VOID pt_im_iip_do_test_3_2_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_FULLHD_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_16_9_RIBERY_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// AFN Unit 3-2-2 YCC422 U8 FullHD->Ribery HD D-Zoom x1.0 (Resize & format comvert)
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "3_2_2"
static VOID pt_im_iip_do_test_3_2_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_FULLHD_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_HD_16_9_RIBERY_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// AFN Unit 3-2-3 YCC422 U8 FullHD->Ribery FullHD D-Zoom x1.0 (Resize & format comvert)
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "3_2_3"
static VOID pt_im_iip_do_test_3_2_3( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_FULLHD_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_FULLHD_RIBERY_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_3_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_3_2_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_3_2_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 3:
			pt_im_iip_do_test_3_2_3( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}

}


// FRECT Unit 4-1-1 YCC422 U8 VGA->VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "4_1_1"
static VOID pt_im_iip_do_test_4_1_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
#ifndef CO_DEBUG_ON_PC
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_0_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_1_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
#endif //CO_DEBUG_ON_PC
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_4_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_4_1_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}

}


// LUT Unit 5-1-1 YCC422 U8 VGA->VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "5_1_1"
static VOID pt_im_iip_do_test_5_1_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
#ifndef CO_DEBUG_ON_PC
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_0_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_1_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
#endif //CO_DEBUG_ON_PC
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// LUT Unit 5-1-2 YCC422 U8 VGA->VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "5_1_2"
static VOID pt_im_iip_do_test_5_1_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
#ifndef CO_DEBUG_ON_PC
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_0_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_1_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
#endif //CO_DEBUG_ON_PC
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_5_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_5_1_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_5_1_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// BLEND Unit 6-1-1 YCC422 U8 VGA[2]->VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "6_1_1"
static VOID pt_im_iip_do_test_6_1_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
#ifndef CO_DEBUG_ON_PC
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_0_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_4_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_1_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
#endif //CO_DEBUG_ON_PC
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_6_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_6_1_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// BLEND Unit 6-2-1 YCC422 U14 VGA[2]->VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "6_2_1"
static VOID pt_im_iip_do_test_6_2_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U16_BYTES + D_IM_IIP_VGA_YCC422_U16_A_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_YCC422_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U16_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_6_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_6_2_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// CSC[0] Unit 7-1-1 YCC422 U8 VGA->VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "7_1_1"
static VOID pt_im_iip_do_test_7_1_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
#ifndef CO_DEBUG_ON_PC
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_0_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_1_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
#endif //CO_DEBUG_ON_PC
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// CSC[0] Unit 7-1-2 YCC422 U8 VGA->VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "7_1_2"
static VOID pt_im_iip_do_test_7_1_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
#ifndef CO_DEBUG_ON_PC
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_0_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
	memset( (VOID*)D_IM_IIP_IMG_MEM_ADDR_1_TOP, '\xee', D_IM_IIP_VGA_YCC422_U8_BYTES );
#endif //CO_DEBUG_ON_PC
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_7_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_7_1_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_7_1_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// 1D Unit 8-1-1 RGBA4444 VGA->VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "8_1_1"
static VOID pt_im_iip_do_test_8_1_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_RGBA4444_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_RGBA4444_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// 1D Unit 8-1-2 RGBA4444 VGA->VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "8_1_2"
static VOID pt_im_iip_do_test_8_1_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_RGBA4444_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_RGBA4444_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// 1D Unit 8-1-3 YUV422Planar VGA-> YUV422Interlived VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "8_1_3"
static VOID pt_im_iip_do_test_8_1_3( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_8_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_8_1_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_8_1_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 3:
			pt_im_iip_do_test_8_1_3( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// FLT Unit SLINK 9-1-1 YCC422 U8 VGA->VGA not convert
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "9_1_1"
static VOID pt_im_iip_do_test_9_1_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// FLT Unit SLINK 9-1-2 Bayer 12bit packed VGA->VGA not convert
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "9_1_2"
static VOID pt_im_iip_do_test_9_1_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_RAW12BIT_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_RAW12BIT_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_9_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_9_1_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_9_1_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// FLT Unit DLINK 9-2-1
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "9_2_1"
static VOID pt_im_iip_do_test_9_2_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_RGBA8888_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_RGBA8888_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_RGBA8888_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_9_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_9_2_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// FLT Unit NORMAL 9-3-1
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "9_3_1"
static VOID pt_im_iip_do_test_9_3_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_RGBA5551_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_RGBA5551_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_9_3( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_9_3_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// SL Unit 10-1-1
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "10_1_1"
static VOID pt_im_iip_do_test_10_1_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_RGB565_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_RGB565_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_RGB565_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// SL Unit 10-1-2
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "10_1_2"
static VOID pt_im_iip_do_test_10_1_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC420_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_10_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_10_1_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_10_1_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// MON Unit 11-1-1 YUV422 VGA-> YUV422 VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "11_1_1"
static VOID pt_im_iip_do_test_11_1_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// MON Unit 11-1-2 YUV422 VGA-> YUV422 VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "11_1_2"
static VOID pt_im_iip_do_test_11_1_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U16_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_11_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_11_1_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_11_1_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// CFL Unit 12-1-1
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "12_1_1"
static VOID pt_im_iip_do_test_12_1_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// CFL Unit 12-1-2
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "12_1_2"
static VOID pt_im_iip_do_test_12_1_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_12_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_12_1_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_12_1_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// GPC Unit 13-1-1
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "13_1_1"
static VOID pt_im_iip_do_test_13_1_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// GPC Unit 13-1-2
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "13_1_2"
static VOID pt_im_iip_do_test_13_1_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// GPC Unit 13-1-3 YUV422 VGA + YUV420A VGA-> YUV422 VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "13_1_3"
static VOID pt_im_iip_do_test_13_1_3( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_YCC420_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_YCC420_U8_A, D_IM_IIP_VGA_YCC420_U8_A_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}

// GPC Unit 13-1-4 YUV422 VGA (10%) + YUV420A VGA-> YUV422 VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "13_1_4"
static VOID pt_im_iip_do_test_13_1_4( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_1_10_YCC420_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_VGA_1_10_YCC420_U8_A, D_IM_IIP_VGA_1_10_YCC420_U8_A_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_1_10_YCC420_U8_BYTES );
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_VGA_1_10_YCC420_U8_A, D_IM_IIP_VGA_1_10_YCC420_U8_A_BYTES );


	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}

// GPC Unit 13-1-5 Ribery VGA + YUV420A VGA-> Ribery VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "13_1_5"
static VOID pt_im_iip_do_test_13_1_5( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_RIBERY_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_YCC420_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_YCC420_U8_A, D_IM_IIP_VGA_YCC420_U8_A_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_RIBERY_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}

// GPC Unit 13-1-6 Ribery VGA (10%) + YUV420A VGA-> YUV422 VGA
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "13_1_6"
static VOID pt_im_iip_do_test_13_1_6( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_RIBERY_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_1_10_YCC420_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_VGA_1_10_YCC420_U8_A, D_IM_IIP_VGA_1_10_YCC420_U8_A_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_RIBERY_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}

// GPC Unit 13-1-7 YUV422 FHD (10%) + YUV420A (FHD10%)-> YUV422 FHD
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "13_1_7"
static VOID pt_im_iip_do_test_13_1_7( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_FULLHD_YCC422_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_FULLHD_1_10_YCC420_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_FULLHD_1_10_YCC420_U8_A, D_IM_IIP_FULLHD_1_10_YCC420_U8_A_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_FULLHD_YCC422_U8_BYTES );
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_FULLHD_1_10_YCC420_U8_BYTES );
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_FULLHD_1_10_YCC420_U8_A, D_IM_IIP_FULLHD_1_10_YCC420_U8_A_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}

// GPC Unit 13-1-8 Ribery FHD (10%) + YUV420A (FHD10%)-> Ribery FHD
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "13_1_8"
static VOID pt_im_iip_do_test_13_1_8( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_FULLHD_RIBERY_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_FULLHD_1_10_YCC420_U8_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_FULLHD_1_10_YCC420_U8_A, D_IM_IIP_FULLHD_1_10_YCC420_U8_A_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_FULLHD_RIBERY_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}

static VOID pt_im_iip_run_13_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_13_1_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_13_1_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 3:
			pt_im_iip_do_test_13_1_3( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 4:
			pt_im_iip_do_test_13_1_4( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 5:
			pt_im_iip_do_test_13_1_5( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 6:
			pt_im_iip_do_test_13_1_6( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 7:
			pt_im_iip_do_test_13_1_7( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 8:
			pt_im_iip_do_test_13_1_8( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// Im_IIP_Utility_ResizeRotate 14-1-1
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "14_1_1"
static VOID pt_im_iip_do_test_14_1_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_2_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// Im_IIP_Utility_ResizeRotate 14-1-2
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "14_1_2"
static VOID pt_im_iip_do_test_14_1_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_2_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// Im_IIP_Utility_ResizeRotate 14-1-3
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "14_1_3"
static VOID pt_im_iip_do_test_14_1_3( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_HD_4_3_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// Im_IIP_Utility_ResizeRotate 14-1-4
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "14_1_4"
static VOID pt_im_iip_do_test_14_1_4( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U16_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_14_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_14_1_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_14_1_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 3:
			pt_im_iip_do_test_14_1_3( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 4:
			pt_im_iip_do_test_14_1_4( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// Im_IIP_Utility_CSC 14-2-1
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "14_2_1"
static VOID pt_im_iip_do_test_14_2_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_RGBA4444_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_RGBA4444_BYTES );
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_2_TOP, D_IM_IIP_VGA_RGBA4444_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// Im_IIP_Utility_CSC 14-2-2
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "14_2_2"
static VOID pt_im_iip_do_test_14_2_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, 320 *2 *240 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, 320 *2 *240 );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_14_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_14_2_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_14_2_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// Im_IIP_Utility_LUT 14-3-1
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "14_3_1"
static VOID pt_im_iip_do_test_14_3_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// Im_IIP_Utility_LUT 14-3-2
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "14_3_2"
static VOID pt_im_iip_do_test_14_3_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U12_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U12_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_14_3( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_14_3_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_14_3_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// IM_IIP_Utility_AlphaBlend 14-4-1
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "14_4_1"
static VOID pt_im_iip_do_test_14_4_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, (D_IM_IIP_VGA_YCC422_U8_BYTES + D_IM_IIP_VGA_YCC422_U8_A_BYTES) );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// IM_IIP_Utility_AlphaBlend 14-4-2
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "14_4_2"
static VOID pt_im_iip_do_test_14_4_2( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_RGBA8888_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_RGBA8888_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_RGBA8888_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// IM_IIP_Utility_AlphaBlend 14-4-3
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "14_4_3"
static VOID pt_im_iip_do_test_14_4_3( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC444_U16_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_YCC444_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC444_U16_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


// IM_IIP_Utility_AlphaBlend 14-4-4
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "14_4_4"
static VOID pt_im_iip_do_test_14_4_4( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U16_BYTES + D_IM_IIP_VGA_YCC422_U16_A_BYTES );
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_4_TOP, D_IM_IIP_VGA_YCC422_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U16_BYTES + D_IM_IIP_VGA_YCC422_U16_A_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_14_4( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_14_4_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 2:
			pt_im_iip_do_test_14_4_2( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 3:
			pt_im_iip_do_test_14_4_3( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		case 4:
			pt_im_iip_do_test_14_4_4( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


// MFT Unit 15-1-1 YCC422 U8 VGA->VGA Max filter
#undef D_IM_IIP_TEST_NO
#define D_IM_IIP_TEST_NO "15_1_1"
static VOID pt_im_iip_do_test_15_1_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_IIP_IMG_MEM_ADDR_0_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* IIP begin CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));
	CT_Im_IIP_Run( ct_no_1st, ct_no_2nd, ct_no_3rd );
	Ddim_Print(( "* IIP end CT_Im_Iip_" D_IM_IIP_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_IIP_IMG_MEM_ADDR_1_TOP, D_IM_IIP_VGA_YCC422_U8_BYTES );

	Ddim_Print(( "*** IIP test " D_IM_IIP_TEST_NO " end\n" ));
}


static VOID pt_im_iip_run_15_1( UCHAR const ct_no_1st, UCHAR const ct_no_2nd, UCHAR const ct_no_3rd )
{
	switch( ct_no_3rd ) {
		case 1:
			pt_im_iip_do_test_15_1_1( ct_no_1st, ct_no_2nd, ct_no_3rd );
			break;
		default:
			Ddim_Print(( errmsg_skip, ct_no_1st, ct_no_2nd, ct_no_3rd ));
			break;
	}
}


VOID Pt_Im_IIP_Main( VOID )
{
	UCHAR			type1;
	UCHAR			type2;
	UCHAR			type3;
	INT32			ercd;

	// 開始...
	Ddim_Print(( "****** IIP begin Pt_Im_IIP_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	type1	= gDDIM_Info.com._6a;
	type2	= gDDIM_Info.com._6b;
	type3	= gDDIM_Info.com._6c;

	Ddim_Print(( "*** IIP begin Im_IIP_Init\n" ));

#ifdef CO_DEBUG_ON_PC
	memset( (VOID*)&IO_IIP, '\0', sizeof(IO_IIP) );
	IO_IIP.IZACTL1.bit.TRG = 2;
	IO_IIP.IZACTL2.bit.ABORT = 2;
	Dd_Top_Set_CLKSTOP3_IIPAP( 1 );
	Dd_Top_Set_CLKSTOP3_IIPAH( 1 );
	Dd_Top_Set_CLKSTOP3_IIPAX( 1 );
	Dd_Top_Set_CLKSTOP3_IIPCK( 1 );
#endif // CO_DEBUG_ON_PC

	ercd = Im_IIP_Init();
	if( ercd != D_IM_IIP_OK ) {
		Ddim_Print(( "Im_IIP_Init() error 0x%x\n", ercd ));

		// テスト強制終了処理
		Palladium_Force_Stop();

		return;
	}
	Ddim_Print(( "*** IIP end Im_IIP_Init\n" ));

	pt_im_iip_enable_gic();
	pt_im_iip_start_clock();

	switch( type1 ) {
		case 1:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_1_1( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		case 2:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_2_1( type1, type2, type3 );
					break;
				case 2:
					pt_im_iip_run_2_2( type1, type2, type3 );
					break;
				case 4:
					pt_im_iip_run_2_4( type1, type2, type3 );
					break;
				case 5:
					pt_im_iip_run_2_5( type1, type2, type3 );
					break;
				case 6:
					pt_im_iip_run_2_6( type1, type2, type3 );
					break;
				case 7:
					pt_im_iip_run_2_7( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		case 3:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_3_1( type1, type2, type3 );
					break;
				case 2:
					pt_im_iip_run_3_2( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		case 4:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_4_1( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		case 5:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_5_1( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		case 6:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_6_1( type1, type2, type3 );
					break;
				case 2:
					pt_im_iip_run_6_2( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		case 7:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_7_1( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		case 8:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_8_1( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		case 9:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_9_1( type1, type2, type3 );
					break;
				case 2:
					pt_im_iip_run_9_2( type1, type2, type3 );
					break;
				case 3:
					pt_im_iip_run_9_3( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		case 10:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_10_1( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		case 11:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_11_1( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		case 12:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_12_1( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		case 13:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_13_1( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		case 14:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_14_1( type1, type2, type3 );
					break;
				case 2:
					pt_im_iip_run_14_2( type1, type2, type3 );
					break;
				case 3:
					pt_im_iip_run_14_3( type1, type2, type3 );
					break;
				case 4:
					pt_im_iip_run_14_4( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		case 15:
			switch( type2 ) {
				case 1:
					pt_im_iip_run_15_1( type1, type2, type3 );
					break;
				default:
					Ddim_Print(( errmsg_skip, type1, type2, type3 ));
					break;
			}
			break;
		default:
			Ddim_Print(( errmsg_skip, type1, type2, type3 ));
			break;
	}

	// タイマーログ出力
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );


	Ddim_Print(( "****** IIP end Pt_Im_IIP_Main\n" ));

	// テスト強制終了処理
	Palladium_Force_Stop();
}

