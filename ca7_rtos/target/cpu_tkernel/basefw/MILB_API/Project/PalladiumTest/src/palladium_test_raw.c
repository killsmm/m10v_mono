/**
 * @file		palladium_test_raw.c
 * @brief		palladium test raw code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ct_im_raw.h"
#include "im_raw.h"
#include "dd_gic.h"
#include "dd_top.h"
//#include "dd_mmu.h"

#include "driver_common.h"
#include "palladium_test.h"

#include <string.h>

#define D_PT_RAW_CMD_LEN		(11)

static char* gCmd_Argv[D_PT_RAW_CMD_LEN];

static VOID pt_im_raw_enable_gic( VOID )
{
//	Dd_GIC_Init();
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH0_INT, 1, D_DD_GIC_PRI30, 1 );		// cpu=0, intid=118, enable, pri=30
	Dd_GIC_Ctrl( E_DD_GIC_INTID_RAW_INT,       1, D_DD_GIC_PRI30, 1 );		// int enable
}


static VOID pt_im_raw_start_clock( VOID )
{
	UCHAR im_raw_pclk_ctrl_cnt = 0;
	UCHAR im_raw_clk_ctrl_cnt = 0;
	UCHAR im_raw_iclk_ctrl_cnt = 0;

	Dd_Top_Start_Clock( (UCHAR*)&im_raw_pclk_ctrl_cnt, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_RAWAP_BIT );

	Dd_Top_Start_Clock( (UCHAR*)&im_raw_iclk_ctrl_cnt, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_RAWAX_BIT );

	Dd_Top_Start_Clock( (UCHAR*)&im_raw_clk_ctrl_cnt, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_RAWCK_BIT );
}

static void pt_im_raw_execute_cmd(int cmd_argc, char* cmd_argv) 
{ 
	INT32 j; 
	INT32 cmd_pos = 0; 

	for (j = 0; j < cmd_argc; j++) { 
		gCmd_Argv[j] = (cmd_argv + (cmd_pos * D_PT_RAW_CMD_LEN)); 
		cmd_pos++; 
	}

	Ct_Im_RAW_Main(cmd_argc, gCmd_Argv); 
}

// Input: 12bit packed raw ch0
#undef D_IM_RAW_TEST_NO
#define D_IM_RAW_TEST_NO "2_0"
static VOID pt_im_raw_do_test_2_0( VOID )
{
	char test_argv[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "0"};

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " begin\n" ));

	// 入力データ(不要：エラールートチェックの為)
//	Palladium_Set_In_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* RAW begin CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));
	pt_im_raw_execute_cmd( (sizeof(test_argv) / D_PT_RAW_CMD_LEN), (char*)test_argv );
	Ddim_Print(( "* RAW end CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ(不要：エラールートチェックの為)
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ENC_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " end\n" ));
}

// Input: 12bit packed raw ch1
#undef D_IM_RAW_TEST_NO
#define D_IM_RAW_TEST_NO "2_1"
static VOID pt_im_raw_do_test_2_1( VOID )
{
	char test_argv[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "1"};

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " begin\n" ));

	// 入力データ(不要：初期化処理チェックの為)
//	Palladium_Set_In_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* RAW begin CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));
	pt_im_raw_execute_cmd( (sizeof(test_argv) / D_PT_RAW_CMD_LEN), (char*)test_argv );
	Ddim_Print(( "* RAW end CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ(不要：初期化処理チェックの為)
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ENC_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );
	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " end\n" ));
}

#undef D_IM_RAW_TEST_NO
#define D_IM_RAW_TEST_NO "2_2"
static VOID pt_im_raw_do_test_2_2( VOID )
{
	char test_argv[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "2"};

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	// 入力データダンプ
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* RAW begin CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));
	pt_im_raw_execute_cmd( (sizeof(test_argv) / D_PT_RAW_CMD_LEN), (char*)test_argv );
	Ddim_Print(( "* RAW end CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ENC_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_DEC_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	
	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " end\n" ));
}

#undef D_IM_RAW_TEST_NO
#define D_IM_RAW_TEST_NO "2_3"
static VOID pt_im_raw_do_test_2_3( VOID )
{
	char test_argv[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "3"};

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	// 入力データダンプ
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* RAW begin CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));
	pt_im_raw_execute_cmd( (sizeof(test_argv) / D_PT_RAW_CMD_LEN), (char*)test_argv );
	Ddim_Print(( "* RAW end CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ENC_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_DEC_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	
	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " end\n" ));
}

#undef D_IM_RAW_TEST_NO
#define D_IM_RAW_TEST_NO "2_4"
static VOID pt_im_raw_do_test_2_4( VOID )
{
	char test_argv[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "4"};

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_Y_0_SIZ );
	// 入力データダンプ
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_Y_0_SIZ );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* RAW begin CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));
	pt_im_raw_execute_cmd( (sizeof(test_argv) / D_PT_RAW_CMD_LEN), (char*)test_argv );
	Ddim_Print(( "* RAW end CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ENC_ADDR, D_CT_IM_RAW_INPUT_Y_0_SIZ );
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_DEC_ADDR, D_CT_IM_RAW_INPUT_Y_0_SIZ );
	
	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " end\n" ));
}

#undef D_IM_RAW_TEST_NO
#define D_IM_RAW_TEST_NO "2_5"
static VOID pt_im_raw_do_test_2_5( VOID )
{
	char test_argv[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "5"};

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_Y_0_SIZ );
	// 入力データダンプ
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_Y_0_SIZ );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* RAW begin CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));
	pt_im_raw_execute_cmd( (sizeof(test_argv) / D_PT_RAW_CMD_LEN), (char*)test_argv );
	Ddim_Print(( "* RAW end CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ENC_ADDR, D_CT_IM_RAW_INPUT_Y_0_SIZ );
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_DEC_ADDR, D_CT_IM_RAW_INPUT_Y_0_SIZ );
	
	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " end\n" ));
}

#undef D_IM_RAW_TEST_NO
#define D_IM_RAW_TEST_NO "2_6"
static VOID pt_im_raw_do_test_2_6( VOID )
{
	char test_argv[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "6"};

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	// 入力データダンプ
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* RAW begin CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));
	pt_im_raw_execute_cmd( (sizeof(test_argv) / D_PT_RAW_CMD_LEN), (char*)test_argv );
	Ddim_Print(( "* RAW end CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ENC_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_DEC_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	
	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " end\n" ));
}

#undef D_IM_RAW_TEST_NO
#define D_IM_RAW_TEST_NO "2_7"
static VOID pt_im_raw_do_test_2_7( VOID )
{
	char test_argv[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "7"};

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	// 入力データダンプ
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* RAW begin CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));
	pt_im_raw_execute_cmd( (sizeof(test_argv) / D_PT_RAW_CMD_LEN), (char*)test_argv );
	Ddim_Print(( "* RAW end CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ENC_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	
	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " end\n" ));
}

#undef D_IM_RAW_TEST_NO
#define D_IM_RAW_TEST_NO "2_8"
static VOID pt_im_raw_do_test_2_8( VOID )
{
	char test_argv[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "8"};

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	// 入力データダンプ
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* RAW begin CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));
	pt_im_raw_execute_cmd( (sizeof(test_argv) / D_PT_RAW_CMD_LEN), (char*)test_argv );
	Ddim_Print(( "* RAW end CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ENC_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_DEC_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	
	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " end\n" ));
}

#undef D_IM_RAW_TEST_NO
#define D_IM_RAW_TEST_NO "2_9"
static VOID pt_im_raw_do_test_2_9( VOID )
{
	char test_argv[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "9"};

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	// 入力データダンプ
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* RAW begin CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));
	pt_im_raw_execute_cmd( (sizeof(test_argv) / D_PT_RAW_CMD_LEN), (char*)test_argv );
	Ddim_Print(( "* RAW end CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ENC_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_DEC_ADDR, D_CT_IM_RAW_INPUT_BAYER_1_SIZ );
	
	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " end\n" ));
}

#undef D_IM_RAW_TEST_NO
#define D_IM_RAW_TEST_NO "2_10"
static VOID pt_im_raw_do_test_2_10( VOID )
{
	char test_argv[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "10"};

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " begin\n" ));

	// 入力データ(不要：セマフォ動作確認処理の為)
//	Palladium_Set_In_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_SIZ_4K2K );
	// 入力データダンプ
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_SIZ_4K2K );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* RAW begin CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));
	pt_im_raw_execute_cmd( (sizeof(test_argv) / D_PT_RAW_CMD_LEN), (char*)test_argv );
	Ddim_Print(( "* RAW end CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ(不要：セマフォ動作確認処理の為)
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ENC_ADDR, D_CT_IM_RAW_SIZ_4K2K );
	
	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " end\n" ));
}

#undef D_IM_RAW_TEST_NO
#define D_IM_RAW_TEST_NO "2_11"
static VOID pt_im_raw_do_test_2_11( VOID )
{
	char test_argv[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "11"};

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );
	// 入力データダンプ
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* RAW begin CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));
	pt_im_raw_execute_cmd( (sizeof(test_argv) / D_PT_RAW_CMD_LEN), (char*)test_argv );
	Ddim_Print(( "* RAW end CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ENC_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_DEC_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );
	
	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " end\n" ));
}

#undef D_IM_RAW_TEST_NO
#define D_IM_RAW_TEST_NO "2_13"
static VOID pt_im_raw_do_test_2_13( VOID )
{
	char test_argv[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "13"};

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " begin\n" ));

	// 入力データ(不要：AXI Errorチェックの為)
//	Palladium_Set_In_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );
	// 入力データダンプ
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* RAW begin CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));
	pt_im_raw_execute_cmd( (sizeof(test_argv) / D_PT_RAW_CMD_LEN), (char*)test_argv );
	Ddim_Print(( "* RAW end CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ(不要：AXI Errorチェックの為)
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ENC_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );
	
	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " end\n" ));
}

#undef D_IM_RAW_TEST_NO
#define D_IM_RAW_TEST_NO "2_14"
static VOID pt_im_raw_do_test_2_14( VOID )
{
	char test_argv_1[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "14", "1", "1", "4704", "3500", "0", "0", "0", "0" };
	char test_argv_2[][D_PT_RAW_CMD_LEN] = {"imraw", "2", "14", "1", "1", "4704", "3500", "0", "0", "0", "1" };

	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );
	// 入力データダンプ
//	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ORG_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* RAW begin CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));
	pt_im_raw_execute_cmd( (sizeof(test_argv_1) / D_PT_RAW_CMD_LEN), (char*)test_argv_1 );
	
	pt_im_raw_execute_cmd( (sizeof(test_argv_2) / D_PT_RAW_CMD_LEN), (char*)test_argv_2 );
	Ddim_Print(( "* RAW end CT_Im_Raw_" D_IM_RAW_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_ENC_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );
	Palladium_Set_Out_Localstack( D_CT_IM_RAW_DEC_ADDR, D_CT_IM_RAW_INPUT_BAYER_0_SIZ );
	
	Ddim_Print(( "*** RAW test " D_IM_RAW_TEST_NO " end\n" ));
}

VOID Pt_Im_RAW_Main( VOID )
{
	UCHAR			type1;
	INT32	retval;
	
	// 開始...
	Ddim_Print(( "****** RAW begin Pt_Im_RAW_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	type1	= gDDIM_Info.com._6a;


	Ddim_Print(( "*** RAW begin Im_RAW_Init\n" ));

	Ddim_Print(("*** RAW RTRG 0x288A0004 = 0x%08lx\n", *(volatile ULONG*)0x288A0004));

	retval = Im_RAW_Init();
	if( retval != D_IM_RAW_RETVAL_OK ){

		Ddim_Print(("****** RAW begin Im_RAW_Init() Error !!!!!,  retval = 0x%x\n", retval));

		Ddim_Print(("*** RAW RTRG 0x288A0004 = 0x%08lx\n", *(volatile ULONG*)0x288A0004));

		// テスト強制終了処理
		Palladium_Force_Stop();
		return;
	}

	Ddim_Print(("*** RAW RTRG 0x288A0004 = 0x%08lx\n", *(volatile ULONG*)0x288A0004));

	Ddim_Print(( "*** RAW end Im_RAW_Init\n" ));

	pt_im_raw_enable_gic();
	pt_im_raw_start_clock();

	switch( type1 ) {
		case 0:
			pt_im_raw_do_test_2_0();
			Ddim_Print(( "\n" ));
			break;
		case 1:
			pt_im_raw_do_test_2_1();
			Ddim_Print(( "\n" ));
			break;
		case 2:
			pt_im_raw_do_test_2_2();
			Ddim_Print(( "\n" ));
			break;
		case 3:
			pt_im_raw_do_test_2_3();
			Ddim_Print(( "\n" ));
			break;
		case 4:
			pt_im_raw_do_test_2_4();
			// RAWレジスタダンプ
			Ddim_Print(( "\n" ));
			break;
		case 5:
			pt_im_raw_do_test_2_5();
			Ddim_Print(( "\n" ));
			break;
		case 6:
			pt_im_raw_do_test_2_6();
			Ddim_Print(( "\n" ));
			break;
		case 7:
			pt_im_raw_do_test_2_7();
			Ddim_Print(( "\n" ));
			break;
		case 8:
			pt_im_raw_do_test_2_8();
			Ddim_Print(( "\n" ));
			break;
		case 9:
			pt_im_raw_do_test_2_9();
			Ddim_Print(( "\n" ));
			break;
		case 10:
			pt_im_raw_do_test_2_10();
			Ddim_Print(( "\n" ));
			break;
		case 11:
			pt_im_raw_do_test_2_11();
			Ddim_Print(( "\n" ));
			break;
		case 13:
			pt_im_raw_do_test_2_13();
			Ddim_Print(( "\n" ));
			break;
		case 14:
			pt_im_raw_do_test_2_14();
			Ddim_Print(( "\n" ));
			break;
		default:
			Ddim_Print(("*** RAW test skip\n" ));
			break;
	}

	// タイマーログ出力
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );
	
#if 0
	Ddim_Print(( "\n" ));
	Ddim_Print(( "(CLKSEL-B)\n" ));
	Ddim_Print(( "RAWCLK[0] = %d\n", Dd_Top_Get_CLKSEL_RAW0CLK() ));
	Ddim_Print(( "RAWCLK[1] = %d\n", Dd_Top_Get_CLKSEL_RAW1CLK() ));
	Ddim_Print(( "(CLKSTOP-F)\n" ));
	Ddim_Print(( "RAWCK[0] = %d\n", Dd_Top_Get_CLKSTOP_RAW0CK() ));
	Ddim_Print(( "RAWCK[1] = %d\n", Dd_Top_Get_CLKSTOP_RAW1CK() ));
	Ddim_Print(( "RAWAP[0] = %d\n", Dd_Top_Get_CLKSTOP_RAW0AP() ));
	Ddim_Print(( "RAWAP[1] = %d\n", Dd_Top_Get_CLKSTOP_RAW1AP() ));
	Ddim_Print(( "RAWAX[0] = %d\n", Dd_Top_Get_CLKSTOP_RAW0AX() ));
	Ddim_Print(( "RAWAX[1] = %d\n", Dd_Top_Get_CLKSTOP_RAW1AX() ));
	Ddim_Print(( "****** RAW end Pt_Im_RAW_Main\n" ));
#endif
	// テスト強制終了処理
	Palladium_Force_Stop();
}

