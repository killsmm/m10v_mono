/**
 * @file		palladium_test_fpt.c
 * @brief		palladium test fpt code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include "ct_im_fpt.h"
#include "im_fpt.h"
#include "dd_gic.h"
#include "dd_top.h"

#include "driver_common.h"
#include "palladium_test.h"

#include <string.h>

#define D_IM_FPT0_CLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP4())
#define D_IM_FPT0_ICLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP4())
#define D_IM_FPT0_HCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP4())
#define D_IM_FPT0_PCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP4())

#define D_IM_FPT0_CLK_REG_BIT	D_DD_TOP_FPT0CK_BIT	// bit8~9
#define D_IM_FPT0_ICLK_REG_BIT	D_DD_TOP_FPT0AX_BIT	// bit10~11
#define D_IM_FPT0_HCLK_REG_BIT	D_DD_TOP_FPT0AH_BIT	// bit12~13
#define D_IM_FPT0_PCLK_REG_BIT	D_DD_TOP_FPT0AP_BIT	// bit14~15

#define D_IM_FPT1_CLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP4())
#define D_IM_FPT1_ICLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP4())
#define D_IM_FPT1_HCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP4())
#define D_IM_FPT1_PCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP4())

#define D_IM_FPT1_CLK_REG_BIT	D_DD_TOP_FPT1CK_BIT	// bit16~17
#define D_IM_FPT1_ICLK_REG_BIT	D_DD_TOP_FPT1AX_BIT	// bit18~19
#define D_IM_FPT1_HCLK_REG_BIT	D_DD_TOP_FPT1AH_BIT	// bit20~21
#define D_IM_FPT1_PCLK_REG_BIT	D_DD_TOP_FPT1AP_BIT	// bit22~23


#define D_IM_FPT_IMG_MEM_IN_Y_ADDR_TOP						(0x60000000)
#define D_IM_FPT_IMG_MEM_IN_BASE_Y_ADDR_TOP					(0x60000000)
#define D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP				(0x64000000)
#define D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP	(0x68000000)
#define D_IM_FPT_IMG_MEM_OUT_MAP_ADDR_TOP					(0x68000000)
#define D_IM_FPT_IMG_MEM_OUT_HIST_ADDR_TOP					(0x70000000)
#define D_IM_FPT_IMG_MEM_OUT_FEATURE_AMOUNT_ADDR_TOP		(0x70000000)
#define D_IM_FPT_IMG_MEM_OUT_VECTOR_ADDR_TOP				(0x70000000)

#define D_IM_FPT_IMG_MEM_IN_Y_BYTES							(0x10680)		// 224 * 150 * 2
#define D_IM_FPT_IMG_MEM_IN_BASE_Y_BYTES					(0x10680)		// 224 * 150 * 2
#define D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES					(0x10680)		// 224 * 150 * 2
#define D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES		(0x200)
#define D_IM_FPT_IMG_MEM_OUT_MAP_BYTES						(0x10680)		// 224 * 150 * 2
#define D_IM_FPT_IMG_MEM_OUT_HIST_BYTES						(0x200)
#define D_IM_FPT_IMG_MEM_OUT_FEATURE_AMOUNT_BYTES			(0x200)
#define D_IM_FPT_IMG_MEM_OUT_VECTOR_BYTES					(0x200)


static VOID pt_im_fpt_enable_gic( VOID )
{
//	Dd_GIC_Init();
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH0_INT,    1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=118, enable, pri=30
	Dd_GIC_Ctrl( E_DD_GIC_INTID_FPT_UNIT0_INT,    1, D_DD_GIC_PRI30, 1 );		// int enable
	Dd_GIC_Ctrl( E_DD_GIC_INTID_FPT_UNIT1_INT,    1, D_DD_GIC_PRI30, 1 );		// int enable
}


static VOID pt_im_fpt_start_clock( VOID )
{
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_PCLOCK) || defined(CO_ACT_HCLOCK)
#else
	UCHAR im_fpt0_clk_ctrl_cnt = 0;
	UCHAR im_fpt0_hclk_ctrl_cnt = 0;
	UCHAR im_fpt0_pclk_ctrl_cnt = 0;
	UCHAR im_fpt0_iclk_ctrl_cnt = 0;
	UCHAR im_fpt1_clk_ctrl_cnt = 0;
	UCHAR im_fpt1_hclk_ctrl_cnt = 0;
	UCHAR im_fpt1_pclk_ctrl_cnt = 0;
	UCHAR im_fpt1_iclk_ctrl_cnt = 0;

	Dd_Top_Start_Clock( &im_fpt0_pclk_ctrl_cnt, D_IM_FPT0_PCLK_REG_ADDR, ~D_IM_FPT0_PCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_fpt0_hclk_ctrl_cnt, D_IM_FPT0_HCLK_REG_ADDR, ~D_IM_FPT0_HCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_fpt0_iclk_ctrl_cnt, D_IM_FPT0_ICLK_REG_ADDR, ~D_IM_FPT0_ICLK_REG_BIT );
	Dd_Top_Start_Clock( &im_fpt0_clk_ctrl_cnt,  D_IM_FPT0_CLK_REG_ADDR,  ~D_IM_FPT0_CLK_REG_BIT );

	Dd_Top_Start_Clock( &im_fpt1_pclk_ctrl_cnt, D_IM_FPT1_PCLK_REG_ADDR, ~D_IM_FPT1_PCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_fpt1_hclk_ctrl_cnt, D_IM_FPT1_HCLK_REG_ADDR, ~D_IM_FPT1_HCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_fpt1_iclk_ctrl_cnt, D_IM_FPT1_ICLK_REG_ADDR, ~D_IM_FPT1_ICLK_REG_BIT );
	Dd_Top_Start_Clock( &im_fpt1_clk_ctrl_cnt,  D_IM_FPT1_CLK_REG_ADDR,  ~D_IM_FPT1_CLK_REG_BIT );
#endif
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "1_1"
static VOID pt_im_fpt_do_test_1_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		1, 2, 3, 4, 5, 6,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* FPT begin CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_FPT_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* FPT end CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "1_7"
static VOID pt_im_fpt_do_test_1_7( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		7, 8, 9, 10, 11,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* FPT begin CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_FPT_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* FPT end CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


static VOID pt_im_fpt_run_1( const UINT32 type1 )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6b;

	switch( type2 ) {
		case 1:
			pt_im_fpt_do_test_1_1( type1, type2 );
			break;
		case 7:
			pt_im_fpt_do_test_1_7( type1, type2 );
			break;
		default:
			Ddim_Print(("*** FPT test skip\n" ));
			break;
	}
}



#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "2_1"
static VOID pt_im_fpt_do_test_2_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		1, 2, 3, 4, 5, 6,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* FPT begin CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_FPT_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* FPT end CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "2_7"
static VOID pt_im_fpt_do_test_2_7( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		7, 8, 9, 10, 11, 12,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* FPT begin CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_FPT_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* FPT end CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


static VOID pt_im_fpt_run_2( const UINT32 type1 )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6b;

	switch( type2 ) {
		case 1:
			pt_im_fpt_do_test_2_1( type1, type2 );
			break;
		case 7:
			pt_im_fpt_do_test_2_7( type1, type2 );
			break;
		default:
			Ddim_Print(("*** FPT test skip\n" ));
			break;
	}
}



#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "3_1"
static VOID pt_im_fpt_do_test_3_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* FPT begin CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_FPT_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* FPT end CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "3_11"
static VOID pt_im_fpt_do_test_3_11( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* FPT begin CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_FPT_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* FPT end CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "3_21"
static VOID pt_im_fpt_do_test_3_21( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* FPT begin CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_FPT_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* FPT end CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


static VOID pt_im_fpt_run_3( const UINT32 type1 )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6b;

	switch( type2 ) {
		case 1:
			pt_im_fpt_do_test_3_1( type1, type2 );
			break;
		case 11:
			pt_im_fpt_do_test_3_11( type1, type2 );
			break;
		case 21:
			pt_im_fpt_do_test_3_21( type1, type2 );
			break;
		default:
			Ddim_Print(("*** FPT test skip\n" ));
			break;
	}
}



#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "4_1"
static VOID pt_im_fpt_do_test_4_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	static const UCHAR	no_2nd_tbl[] = {
		1, 2, 3,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* FPT begin CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_FPT_Run( ct_idx_1st, no_2nd_tbl[loopcnt], ct_idx_3rd );
		Ddim_Print(( "* FPT end CT_Im_Fpt_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "4_4"
static VOID pt_im_fpt_do_test_4_4( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_OUT_MAP_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_OUT_MAP_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_OUT_HIST_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_OUT_HIST_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_Y_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* FPT begin CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));
	CT_Im_FPT_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* FPT end CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_FPT_IMG_MEM_OUT_MAP_ADDR_TOP, D_IM_FPT_IMG_MEM_OUT_MAP_BYTES );
	Palladium_Set_Out_Localstack( D_IM_FPT_IMG_MEM_OUT_HIST_ADDR_TOP, D_IM_FPT_IMG_MEM_OUT_HIST_BYTES );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "4_5"
static VOID pt_im_fpt_do_test_4_5( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_OUT_MAP_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_OUT_MAP_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_Y_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* FPT begin CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));
	CT_Im_FPT_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* FPT end CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_FPT_IMG_MEM_OUT_MAP_ADDR_TOP, D_IM_FPT_IMG_MEM_OUT_MAP_BYTES );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "4_6"
static VOID pt_im_fpt_do_test_4_6( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_OUT_FEATURE_AMOUNT_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_OUT_FEATURE_AMOUNT_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* FPT begin CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));
	CT_Im_FPT_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* FPT end CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_FPT_IMG_MEM_OUT_FEATURE_AMOUNT_ADDR_TOP, D_IM_FPT_IMG_MEM_OUT_FEATURE_AMOUNT_BYTES );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "4_7"
static VOID pt_im_fpt_do_test_4_7( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* FPT begin CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));
	CT_Im_FPT_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* FPT end CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "4_8"
static VOID pt_im_fpt_do_test_4_8( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_BASE_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_BASE_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_OUT_VECTOR_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_OUT_VECTOR_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_BASE_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_BASE_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* FPT begin CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));
	CT_Im_FPT_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* FPT end CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_FPT_IMG_MEM_OUT_VECTOR_ADDR_TOP, D_IM_FPT_IMG_MEM_OUT_VECTOR_BYTES );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "4_9"
static VOID pt_im_fpt_do_test_4_9( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* FPT begin CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));
	CT_Im_FPT_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* FPT end CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "4_10"
static VOID pt_im_fpt_do_test_4_10( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_BASE_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_BASE_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_OUT_VECTOR_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_OUT_VECTOR_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_BASE_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_BASE_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* FPT begin CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));
	CT_Im_FPT_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* FPT end CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_FPT_IMG_MEM_OUT_VECTOR_ADDR_TOP, D_IM_FPT_IMG_MEM_OUT_VECTOR_BYTES );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "4_11"
static VOID pt_im_fpt_do_test_4_11( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* FPT begin CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));
	CT_Im_FPT_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* FPT end CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "4_12"
static VOID pt_im_fpt_do_test_4_12( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_BASE_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_BASE_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_OUT_VECTOR_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_OUT_VECTOR_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_BASE_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_BASE_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* FPT begin CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));
	CT_Im_FPT_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* FPT end CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_FPT_IMG_MEM_OUT_VECTOR_ADDR_TOP, D_IM_FPT_IMG_MEM_OUT_VECTOR_BYTES );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "4_13"
static VOID pt_im_fpt_do_test_4_13( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_BASE_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_BASE_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_OUT_FEATURE_AMOUNT_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_OUT_FEATURE_AMOUNT_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_BASE_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_BASE_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* FPT begin CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));
	CT_Im_FPT_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* FPT end CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_FPT_IMG_MEM_OUT_FEATURE_AMOUNT_ADDR_TOP, D_IM_FPT_IMG_MEM_OUT_FEATURE_AMOUNT_BYTES );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


#undef D_IM_FPT_TEST_NO
#define D_IM_FPT_TEST_NO "4_14"
static VOID pt_im_fpt_do_test_4_14( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_BASE_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_BASE_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );
	memset( (VOID*)D_IM_FPT_IMG_MEM_OUT_VECTOR_ADDR_TOP, '\xee', D_IM_FPT_IMG_MEM_OUT_VECTOR_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_BASE_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_BASE_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_TARGET_Y_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_TARGET_Y_BYTES );
	Palladium_Set_In_Localstack( D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_ADDR_TOP, D_IM_FPT_IMG_MEM_IN_BASE_FEATURE_AMOUNT_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* FPT begin CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));
	CT_Im_FPT_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* FPT end CT_Im_Fpt_" D_IM_FPT_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_FPT_IMG_MEM_OUT_VECTOR_ADDR_TOP, D_IM_FPT_IMG_MEM_OUT_VECTOR_BYTES );

	Ddim_Print(( "*** FPT test " D_IM_FPT_TEST_NO " end\n" ));
}


static VOID pt_im_fpt_run_4( const UINT32 type1 )
{
	UCHAR			type2;
	UCHAR			type3;

	type2	= gDDIM_Info.com._6b;
	type3	= gDDIM_Info.com._6c;

	switch( type2 ) {
		case 1:
			pt_im_fpt_do_test_4_1( type1, type2, type3 );
			break;
		case 4:
			pt_im_fpt_do_test_4_4( type1, type2, type3 );
			break;
		case 5:
			pt_im_fpt_do_test_4_5( type1, type2, type3 );
			break;
		case 6:
			pt_im_fpt_do_test_4_6( type1, type2, type3 );
			break;
		case 7:
			pt_im_fpt_do_test_4_7( type1, type2, type3 );
			break;
		case 8:
			pt_im_fpt_do_test_4_8( type1, type2, type3 );
			break;
		case 9:
			pt_im_fpt_do_test_4_9( type1, type2, type3 );
			break;
		case 10:
			pt_im_fpt_do_test_4_10( type1, type2, type3 );
			break;
		case 11:
			pt_im_fpt_do_test_4_11( type1, type2, type3 );
			break;
		case 12:
			pt_im_fpt_do_test_4_12( type1, type2, type3 );
			break;
		case 13:
			pt_im_fpt_do_test_4_13( type1, type2, type3 );
			break;
		case 14:
			pt_im_fpt_do_test_4_14( type1, type2, type3 );
			break;
		default:
			Ddim_Print(("*** FPT test skip\n" ));
			break;
	}
}


VOID Pt_Im_FPT_Main( VOID )
{
	UCHAR			type1;

	// 開始...
	Ddim_Print(( "****** FPT begin Pt_Im_FPT_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	type1	= gDDIM_Info.com._6a;

	Dd_Top_Set_CLKSTOP4_FPT0CK( 1 );
	Dd_Top_Set_CLKSEL12_FPT0CLK( 0 );	// 300MHz
	Dd_Top_Set_CLKSTOP4_FPT0CK( 0 );
	Dd_Top_Set_CLKSTOP4_FPT1CK( 1 );
	Dd_Top_Set_CLKSEL12_FPT1CLK( 0 );	// 300MHz
	Dd_Top_Set_CLKSTOP4_FPT1CK( 0 );

	pt_im_fpt_enable_gic();
	pt_im_fpt_start_clock();

	switch( type1 ) {
		case 1:
			pt_im_fpt_run_1( type1 );
			break;
		case 2:
			pt_im_fpt_run_2( type1 );
			break;
		case 3:
			pt_im_fpt_run_3( type1 );
			break;
		case 4:
			pt_im_fpt_run_4( type1 );
			break;
		default:
			Ddim_Print(("*** FPT test skip\n" ));
			break;
	}

	// タイマーログ出力
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );


	Ddim_Print(( "(CLKSEL-12)\n" ));
	Ddim_Print(( "FPT0CLK = %d\n", Dd_Top_Get_CLKSEL12_FPT0CLK() ));
	Ddim_Print(( "(CLKSTOP-4)\n" ));
	Ddim_Print(( "FPT0CK = %d\n", Dd_Top_Get_CLKSTOP4_FPT0CK() ));
	Ddim_Print(( "FPT0AP = %d\n", Dd_Top_Get_CLKSTOP4_FPT0AP() ));
	Ddim_Print(( "FPT0AH = %d\n", Dd_Top_Get_CLKSTOP4_FPT0AH() ));
	Ddim_Print(( "FPT0AX = %d\n", Dd_Top_Get_CLKSTOP4_FPT0AX() ));
	Ddim_Print(( "(CLKSEL-12)\n" ));
	Ddim_Print(( "FPT1CLK = %d\n", Dd_Top_Get_CLKSEL12_FPT1CLK() ));
	Ddim_Print(( "(CLKSTOP-4)\n" ));
	Ddim_Print(( "FPT1CK = %d\n", Dd_Top_Get_CLKSTOP4_FPT1CK() ));
	Ddim_Print(( "FPT1AP = %d\n", Dd_Top_Get_CLKSTOP4_FPT1AP() ));
	Ddim_Print(( "FPT1AH = %d\n", Dd_Top_Get_CLKSTOP4_FPT1AH() ));
	Ddim_Print(( "FPT1AX = %d\n", Dd_Top_Get_CLKSTOP4_FPT1AX() ));
	Ddim_Print(( "****** FPT end Pt_Im_FPT_Main\n" ));

	// テスト強制終了処理
	Palladium_Force_Stop();
}

