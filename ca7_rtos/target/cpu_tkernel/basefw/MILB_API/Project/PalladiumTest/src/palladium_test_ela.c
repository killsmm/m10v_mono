/**
 * @file		palladium_test_ela.c
 * @brief		palladium test ela code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ct_im_ela.h"
#include "im_ela.h"
#include "dd_gic.h"
#include "dd_top.h"
//#include "dd_mmu.h"

#include "driver_common.h"
#include "palladium_test.h"

#include <string.h>

#define D_IM_ELA_CLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP6())
#define D_IM_ELA_PCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP6())
#define D_IM_ELA_ICLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP6())

#define D_IM_ELA_CLK_REG_BIT	D_DD_TOP_ELACK_BIT	// bit24
#define D_IM_ELA_PCLK_REG_BIT	D_DD_TOP_ELAAP_BIT	// bit25
#define D_IM_ELA_ICLK_REG_BIT	D_DD_TOP_ELAAX_BIT	// bit27

static VOID pt_im_ela_enable_gic( VOID )
{
//	Dd_GIC_Init();
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH0_INT, 1, D_DD_GIC_PRI30, 1 );		// cpu=0, intid=118, enable, pri=30
	Dd_GIC_Ctrl( E_DD_GIC_INTID_ELA_UNIT_INT,  1, D_DD_GIC_PRI30, 1 );		// int enable
}


static VOID pt_im_ela_start_clock( VOID )
{
	UCHAR im_ela_pclk_ctrl_cnt = 0;
	UCHAR im_ela_clk_ctrl_cnt = 0;
	UCHAR im_ela_iclk_ctrl_cnt = 0;

	Dd_Top_Start_Clock( (UCHAR*)&im_ela_pclk_ctrl_cnt, D_IM_ELA_PCLK_REG_ADDR, ~D_IM_ELA_PCLK_REG_BIT );

	Dd_Top_Start_Clock( (UCHAR*)&im_ela_iclk_ctrl_cnt, D_IM_ELA_ICLK_REG_ADDR, ~D_IM_ELA_ICLK_REG_BIT );

	Dd_Top_Start_Clock( (UCHAR*)&im_ela_clk_ctrl_cnt, D_IM_ELA_CLK_REG_ADDR, ~D_IM_ELA_CLK_REG_BIT );
}


// Input: 12bit packed raw ch0
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_01"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_01
static VOID pt_im_ela_do_test_2_01( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_12M );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

// Input: 12bit packed raw ch1
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_02"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_01
static VOID pt_im_ela_do_test_2_02( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_03"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_02
static VOID pt_im_ela_do_test_2_03( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );
	// 入力データダンプ
//	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}


#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_04"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_02
static VOID pt_im_ela_do_test_2_04( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_05"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_03
static VOID pt_im_ela_do_test_2_05( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}


#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_06"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_03
static VOID pt_im_ela_do_test_2_06( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_12M );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_07"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_04
static VOID pt_im_ela_do_test_2_07( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_08"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_04
static VOID pt_im_ela_do_test_2_08( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_09"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_05
static VOID pt_im_ela_do_test_2_09( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_10"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_05
static VOID pt_im_ela_do_test_2_10( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}


#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_11"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_06
static VOID pt_im_ela_do_test_2_11( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_12"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_06
static VOID pt_im_ela_do_test_2_12( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}



#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_13"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_07
static VOID pt_im_ela_do_test_2_13( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_14"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_07
static VOID pt_im_ela_do_test_2_14( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_15"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_08
static VOID pt_im_ela_do_test_2_15( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_16"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_08
static VOID pt_im_ela_do_test_2_16( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_17"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_09
static VOID pt_im_ela_do_test_2_17( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_18"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_09
static VOID pt_im_ela_do_test_2_18( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, S_SDRAM_ELA_NOISE_SUPPRESS );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}


#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_19"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_10
static VOID pt_im_ela_do_test_2_19( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, (1152 * 628 * 16) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, (S_SDRAM_ELA_OUT_BAYER / 2) );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_20"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_10
static VOID pt_im_ela_do_test_2_20( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, (1152 * 628 * 16) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, (S_SDRAM_ELA_OUT_BAYER / 2) );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}


#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_21"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_11
static VOID pt_im_ela_do_test_2_21( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, (1152 * 628 * 16) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_22"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_11
static VOID pt_im_ela_do_test_2_22( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );	// 長波長ノイズ抑制データ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, (1152 * 628 * 16) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );
	
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}



// Input: 12bit packed raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_23"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_12
static VOID pt_im_ela_do_test_2_23( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

// Input: 12bit packed raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_24"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_12
static VOID pt_im_ela_do_test_2_24( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

#if 0
	memset((VOID*)S_SDRAM_ELA_ADR_NOISE_SUPPRESS, 0xFF, 4);
	memset((VOID*)S_SDRAM_ELA_ADR_EXTRACT_NOISE, 0xAA, 4);
	memset((VOID*)S_SDRAM_ELA_ADR_OUT_BAYER, 0x55, 4);

//	Dd_MMU_Clean_Flush_Dcache_All();
	DDIM_User_L1l2cache_Clean_Flush_All();
#endif

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

// Input: 12bit packed raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_25"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_13
static VOID pt_im_ela_do_test_2_25( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

// Input: 12bit packed raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_26"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_13
static VOID pt_im_ela_do_test_2_26( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}


#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_27"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_14
static VOID pt_im_ela_do_test_2_27( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

// Input: 12bit packed raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_28"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_14
static VOID pt_im_ela_do_test_2_28( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_29"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_15
static VOID pt_im_ela_do_test_2_29( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

// Input: 12bit packed raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_30"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_15
static VOID pt_im_ela_do_test_2_30( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

// Input: 8bit packed raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_31"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_16
static VOID pt_im_ela_do_test_2_31( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, (4480 * 2492));

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_32"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_16
static VOID pt_im_ela_do_test_2_32( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, (4480 * 2492));

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}


#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_33"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_17
static VOID pt_im_ela_do_test_2_33( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

// Input: 12bit packed raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_34"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_17
static VOID pt_im_ela_do_test_2_34( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}


// Input: 16bit packed raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_35"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_18
static VOID pt_im_ela_do_test_2_35( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, (640*480*2) );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_36"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_18
static VOID pt_im_ela_do_test_2_36( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, (640*480*2) );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}


#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_37"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_19
static VOID pt_im_ela_do_test_2_37( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

// Input: 12bit packed raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_38"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_19
static VOID pt_im_ela_do_test_2_38( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_39"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_20
static VOID pt_im_ela_do_test_2_39( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

// Input: 12bit packed raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_40"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_20
static VOID pt_im_ela_do_test_2_40( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}


#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_41"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_21
static VOID pt_im_ela_do_test_2_41( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

// Input: 12bit packed raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_42"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_21
static VOID pt_im_ela_do_test_2_42( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}


// Input: 8bit raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_43"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_22
static VOID pt_im_ela_do_test_2_43( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, (640*480) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, (640*480) );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

// Input: 8bit raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_44"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_22
static VOID pt_im_ela_do_test_2_44( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, (640*480) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, (640*480) );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_45"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_23
static VOID pt_im_ela_do_test_2_45( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

// Input: 12bit packed raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_46"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_23
static VOID pt_im_ela_do_test_2_46( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_47"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_24
static VOID pt_im_ela_do_test_2_47( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}

// Input: 12bit packed raw
#undef D_IM_ELA_TEST_NO
#define D_IM_ELA_TEST_NO "2_48"
#undef D_IM_ELA_CT_FUNC
#define D_IM_ELA_CT_FUNC CT_Im_Ela_2_24
static VOID pt_im_ela_do_test_2_48( VOID )
{
	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( S_SDRAM_ELA_ADR_IN_BAYER, S_SDRAM_ELA_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* ELA begin CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));
	D_IM_ELA_CT_FUNC();
	Ddim_Print(( "* ELA end CT_Im_Ela_" D_IM_ELA_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_NOISE_SUPPRESS, (4480 * 2492) );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_EXTRACT_NOISE, S_SDRAM_ELA_EXTRACT_NOISE );
	Palladium_Set_Out_Localstack( S_SDRAM_ELA_ADR_OUT_BAYER, S_SDRAM_ELA_OUT_BAYER );

	Ddim_Print(( "*** ELA test " D_IM_ELA_TEST_NO " end\n" ));
}


VOID Pt_Im_ELA_Main( VOID )
{
	UCHAR			type1;
	
	// 開始...
	Ddim_Print(( "****** ELA begin Pt_Im_ELA_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	type1	= gDDIM_Info.com._6a;


	Ddim_Print(( "*** ELA begin Im_ELA_Init\n" ));
	Im_ELA_Init();
	Ddim_Print(( "*** ELA end Im_ELA_Init\n" ));

	pt_im_ela_enable_gic();
	pt_im_ela_start_clock();

	switch( type1 ) {
		case 1:
			pt_im_ela_do_test_2_01();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 2:
			pt_im_ela_do_test_2_02();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 3:
			pt_im_ela_do_test_2_03();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 4:
			pt_im_ela_do_test_2_04();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 5:
			pt_im_ela_do_test_2_05();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 6:
			pt_im_ela_do_test_2_06();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 7:
			pt_im_ela_do_test_2_07();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 8:
			pt_im_ela_do_test_2_08();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 9:
			pt_im_ela_do_test_2_09();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 10:
			pt_im_ela_do_test_2_10();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 11:
			pt_im_ela_do_test_2_11();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 12:
			pt_im_ela_do_test_2_12();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 13:
			pt_im_ela_do_test_2_13();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 14:
			pt_im_ela_do_test_2_14();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 15:
			pt_im_ela_do_test_2_15();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 16:
			pt_im_ela_do_test_2_16();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 17:
			pt_im_ela_do_test_2_17();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 18:
			pt_im_ela_do_test_2_18();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 19:
			pt_im_ela_do_test_2_19();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 20:
			pt_im_ela_do_test_2_20();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 21:
			pt_im_ela_do_test_2_21();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 22:
			pt_im_ela_do_test_2_22();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 23:
			pt_im_ela_do_test_2_23();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 24:
			pt_im_ela_do_test_2_24();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 25:
			pt_im_ela_do_test_2_25();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 26:
			pt_im_ela_do_test_2_26();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 27:
			pt_im_ela_do_test_2_27();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 28:
			pt_im_ela_do_test_2_28();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 29:
			pt_im_ela_do_test_2_29();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 30:
			pt_im_ela_do_test_2_30();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 31:
			pt_im_ela_do_test_2_31();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 32:
			pt_im_ela_do_test_2_32();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 33:
			pt_im_ela_do_test_2_33();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 34:
			pt_im_ela_do_test_2_34();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 35:
			pt_im_ela_do_test_2_35();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 36:
			pt_im_ela_do_test_2_36();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 37:
			pt_im_ela_do_test_2_37();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 38:
			pt_im_ela_do_test_2_38();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 39:
			pt_im_ela_do_test_2_39();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 40:
			pt_im_ela_do_test_2_40();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 41:
			pt_im_ela_do_test_2_41();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 42:
			pt_im_ela_do_test_2_42();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 43:
			pt_im_ela_do_test_2_43();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 44:
			pt_im_ela_do_test_2_44();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 45:
			pt_im_ela_do_test_2_45();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 46:
			pt_im_ela_do_test_2_46();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 47:
			pt_im_ela_do_test_2_47();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		case 48:
			pt_im_ela_do_test_2_48();
			// ELAレジスタダンプ
			Ddim_Print(( "\n" ));
			CT_Im_Ela_Reg_Print();
			break;
		default:
			Ddim_Print(("*** ELA test skip\n" ));
			break;
	}

	// タイマーログ出力
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );
	
#if 0
	Ddim_Print(( "\n" ));
	Ddim_Print(( "(CLKSEL-B)\n" ));
	Ddim_Print(( "ELACLK[0] = %d\n", Dd_Top_Get_CLKSEL_ELA0CLK() ));
	Ddim_Print(( "ELACLK[1] = %d\n", Dd_Top_Get_CLKSEL_ELA1CLK() ));
	Ddim_Print(( "(CLKSTOP-F)\n" ));
	Ddim_Print(( "ELACK[0] = %d\n", Dd_Top_Get_CLKSTOP_ELA0CK() ));
	Ddim_Print(( "ELACK[1] = %d\n", Dd_Top_Get_CLKSTOP_ELA1CK() ));
	Ddim_Print(( "ELAAP[0] = %d\n", Dd_Top_Get_CLKSTOP_ELA0AP() ));
	Ddim_Print(( "ELAAP[1] = %d\n", Dd_Top_Get_CLKSTOP_ELA1AP() ));
	Ddim_Print(( "ELAAX[0] = %d\n", Dd_Top_Get_CLKSTOP_ELA0AX() ));
	Ddim_Print(( "ELAAX[1] = %d\n", Dd_Top_Get_CLKSTOP_ELA1AX() ));
	Ddim_Print(( "****** ELA end Pt_Im_ELA_Main\n" ));
#endif
	// テスト強制終了処理
	Palladium_Force_Stop();
}

