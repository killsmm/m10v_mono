/**
 * @file		palladium_test_xch.c
 * @brief		palladium test xch code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ct_im_xch.h"
#include "im_xch.h"
#include "dd_gic.h"
#include "dd_top.h"

#include "driver_common.h"
#include "palladium_test.h"

#include <string.h>

#define D_PT_XCH_CMD_LEN		(11)

static char* gCmd_Argv[D_PT_XCH_CMD_LEN];

static VOID pt_im_xch_enable_gic( VOID )
{
//	Dd_GIC_Init();
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH0_INT, 1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=118, enable, pri=30
	Dd_GIC_Ctrl( E_DD_GIC_INTID_XCH_INT,       1, D_DD_GIC_PRI30, 1 );		// int enable
}


static VOID pt_im_xch_start_clock( VOID )
{
	UCHAR im_xch_pclk_ctrl_cnt = 0;
	UCHAR im_xch_iclk_ctrl_cnt = 0;

	Dd_Top_Start_Clock( (UCHAR*)&im_xch_pclk_ctrl_cnt, &Dd_Top_Get_CLKSTOP6(), ~D_DD_TOP_XCHAP_BIT );

	Dd_Top_Start_Clock( (UCHAR*)&im_xch_iclk_ctrl_cnt, &Dd_Top_Get_CLKSTOP6(), ~D_DD_TOP_XCHAX_BIT );
}

static void pt_im_xch_execute_cmd(int cmd_argc, char* cmd_argv)
{
	INT32 j;
	INT32 cmd_pos = 0;

	for (j = 0; j < cmd_argc; j++) {
		gCmd_Argv[j] = (cmd_argv + (cmd_pos * D_PT_XCH_CMD_LEN));
		cmd_pos++;
	}

	Ct_Im_Xch_Main(cmd_argc, gCmd_Argv);
}

// high speed subsumpling 1/4 8bit
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_1"
static VOID pt_im_xch_do_test_2_1( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "0"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "0", "816", "816", "612", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "4", "0", "0", "0", "0x44444444", "3264"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "0"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 816*612 );
	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// high speed subsumpling 1/4 16bit
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_2"
static VOID pt_im_xch_do_test_2_2( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "0"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "0", "816", "816", "612", "1"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "4", "0", "1", "0", "0x44444444", "3264"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "2"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448*2 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 816*612*2 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// high speed subsumpling 3/4 16bit
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_3"
static VOID pt_im_xch_do_test_2_3( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "1"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "1", "0", "2448", "2448", "1836", "1"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "4", "1", "1", "0", "0xBBBBBBBB", "3264"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "1", "2"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "1"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448*2 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 2448*1836*2 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// subsumpling 1/4 8bit
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_4"
static VOID pt_im_xch_do_test_2_4( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "0"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "1", "816", "816", "612", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "4", "0", "0", "0", "0x44444444", "3264"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "0"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 816*612 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// subsumpling 1/4 16bit
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_5"
static VOID pt_im_xch_do_test_2_5( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "0"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "1", "816", "816", "612", "1"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "4", "0", "1", "0", "0x44444444", "3264"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "2"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448*2 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 816*612*2 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// subsumpling 3/4 16bit
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_6"
static VOID pt_im_xch_do_test_2_6( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "1"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "1", "1", "2448", "2448", "1836", "1"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "4", "1", "1", "0", "0xBBBBBBBB", "3264"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "1", "2"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "1"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448*2 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 2448*1836*2 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// histogram subsumpling 1/4 8bit
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_7"
static VOID pt_im_xch_do_test_2_7( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "0"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "2", "816", "816", "612", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "5", "0", "0", "0", "0x44444444", "3264", "0"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "0"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "10"};
	char test_argv_6[][D_PT_XCH_CMD_LEN] = {"imxch", "11"};
	char test_argv_7[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_6) / D_PT_XCH_CMD_LEN), (char*)test_argv_6 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_7) / D_PT_XCH_CMD_LEN), (char*)test_argv_7 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 816*612 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// histogram subsumpling 1/4 16bit
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_8"
static VOID pt_im_xch_do_test_2_8( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "0"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "2", "1632", "1632", "612", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "5", "0", "1", "0", "0x44444444", "6628", "0"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "2"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "10"};
	char test_argv_6[][D_PT_XCH_CMD_LEN] = {"imxch", "11"};
	char test_argv_7[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448*2 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_6) / D_PT_XCH_CMD_LEN), (char*)test_argv_6 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_7) / D_PT_XCH_CMD_LEN), (char*)test_argv_7 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 816*612*2 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// histogram subsumpling 1/4 8bit
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_9"
static VOID pt_im_xch_do_test_2_9( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "0"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "2", "816", "816", "612", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "5", "0", "0", "0", "0x44444444", "3264", "0"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "0"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "10"};
	char test_argv_6[][D_PT_XCH_CMD_LEN] = {"imxch", "11"};
	char test_argv_7[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "2", "816", "816", "612", "0"};
	char test_argv_8[][D_PT_XCH_CMD_LEN] = {"imxch", "5", "0", "0", "0", "0x44444444", "3264", "1"};
	char test_argv_9[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "0"};
	char test_argv_10[][D_PT_XCH_CMD_LEN] = {"imxch", "10"};
	char test_argv_11[][D_PT_XCH_CMD_LEN] = {"imxch", "11"};
	char test_argv_12[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_6) / D_PT_XCH_CMD_LEN), (char*)test_argv_6 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_7) / D_PT_XCH_CMD_LEN), (char*)test_argv_7 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_8) / D_PT_XCH_CMD_LEN), (char*)test_argv_8 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_9) / D_PT_XCH_CMD_LEN), (char*)test_argv_9 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_10) / D_PT_XCH_CMD_LEN), (char*)test_argv_10 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_11) / D_PT_XCH_CMD_LEN), (char*)test_argv_11 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_12) / D_PT_XCH_CMD_LEN), (char*)test_argv_12 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 816*612 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// histogram subsumpling 1/4 8bit
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_10"
static VOID pt_im_xch_do_test_2_10( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "0"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "3", "816", "816", "612", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "5", "0", "0", "0", "0x44444444", "3264", "0"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "0"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "10"};
	char test_argv_6[][D_PT_XCH_CMD_LEN] = {"imxch", "11"};
	char test_argv_7[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_6) / D_PT_XCH_CMD_LEN), (char*)test_argv_6 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_7) / D_PT_XCH_CMD_LEN), (char*)test_argv_7 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 816*612 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// histogram subsumpling 1/4 16bit
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_11"
static VOID pt_im_xch_do_test_2_11( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "0"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "3", "816", "816", "612", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "5", "0", "1", "0", "0x44444444", "3264", "0"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "2"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "10"};
	char test_argv_6[][D_PT_XCH_CMD_LEN] = {"imxch", "11"};
	char test_argv_7[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448*2 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_6) / D_PT_XCH_CMD_LEN), (char*)test_argv_6 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_7) / D_PT_XCH_CMD_LEN), (char*)test_argv_7 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 816*612*2 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// histogram subsumpling 1/4 8bit
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_12"
static VOID pt_im_xch_do_test_2_12( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "0"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "3", "816", "816", "612", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "5", "0", "0", "0", "0x44444444", "3264", "0"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "0"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "10"};
	char test_argv_6[][D_PT_XCH_CMD_LEN] = {"imxch", "11"};
	char test_argv_7[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "3", "816", "816", "612", "0"};
	char test_argv_8[][D_PT_XCH_CMD_LEN] = {"imxch", "5", "0", "0", "0", "0x44444444", "3264", "1"};
	char test_argv_9[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "0"};
	char test_argv_10[][D_PT_XCH_CMD_LEN] = {"imxch", "10"};
	char test_argv_11[][D_PT_XCH_CMD_LEN] = {"imxch", "11"};
	char test_argv_12[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_6) / D_PT_XCH_CMD_LEN), (char*)test_argv_6 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_7) / D_PT_XCH_CMD_LEN), (char*)test_argv_7 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_8) / D_PT_XCH_CMD_LEN), (char*)test_argv_8 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_9) / D_PT_XCH_CMD_LEN), (char*)test_argv_9 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_10) / D_PT_XCH_CMD_LEN), (char*)test_argv_10 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_11) / D_PT_XCH_CMD_LEN), (char*)test_argv_11 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_12) / D_PT_XCH_CMD_LEN), (char*)test_argv_12 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 816*612 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// 1/2 subsumpling even (sync)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_13"
static VOID pt_im_xch_do_test_2_13( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "0"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "4", "1632", "1632", "2448", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "6", "0", "0", "0xFFFFFFFF", "3264"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "0"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, (3264/2)*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// 1/2 subsumpling even (async)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_14"
static VOID pt_im_xch_do_test_2_14( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "1"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "1", "4", "1632", "1632", "1224", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "6", "1", "0", "0xAAAAAAAA", "3264"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "1", "1"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "1"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*(2448/2) );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// 1/2 subsumpling odd (sync)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_15"
static VOID pt_im_xch_do_test_2_15( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "0"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "5", "1632", "1632", "2448", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "6", "0", "0", "0xFFFFFFFF", "3264"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "0"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, (3264/2)*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// 1/2 subsumpling odd (async)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_16"
static VOID pt_im_xch_do_test_2_16( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "1"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "1", "5", "1632", "1632", "1224", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "6", "1", "0", "0xAAAAAAAA", "3264"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "1", "1"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "1"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*(2448/2) );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// copy (sync)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_17"
static VOID pt_im_xch_do_test_2_17( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "0"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "6", "3264", "3264", "2448", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "6", "0", "0", "0xFFFFFFFF", "3264"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "0"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// copy (async)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_18"
static VOID pt_im_xch_do_test_2_18( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "1"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "1", "6", "3264", "3264", "2448", "1"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "6", "1", "0", "0xFFFFFFFF", "3264"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "1", "2"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "1"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// fill (sync)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_19"
static VOID pt_im_xch_do_test_2_19( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "0"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "0", "7", "3264", "640", "480", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "8", "0", "128"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "0", "0"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// fill (async)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_20"
static VOID pt_im_xch_do_test_2_20( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "1", "1"};
	char test_argv_2[][D_PT_XCH_CMD_LEN] = {"imxch", "3", "1", "7", "3264", "640", "480", "0"};
	char test_argv_3[][D_PT_XCH_CMD_LEN] = {"imxch", "8", "1", "0"};
	char test_argv_4[][D_PT_XCH_CMD_LEN] = {"imxch", "9", "1", "1"};
	char test_argv_5[][D_PT_XCH_CMD_LEN] = {"imxch", "2", "1"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_2) / D_PT_XCH_CMD_LEN), (char*)test_argv_2 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_3) / D_PT_XCH_CMD_LEN), (char*)test_argv_3 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_4) / D_PT_XCH_CMD_LEN), (char*)test_argv_4 );
	pt_im_xch_execute_cmd( (sizeof(test_argv_5) / D_PT_XCH_CMD_LEN), (char*)test_argv_5 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : copy (sync)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_21"
static VOID pt_im_xch_do_test_2_21( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "21", "0", "3264", "2448"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : copy (async)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_22"
static VOID pt_im_xch_do_test_2_22( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "21", "1", "3264", "2448"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : copy_ex_sync
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_23"
static VOID pt_im_xch_do_test_2_23( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "22", "0", "0", "3264", "3264", "2448", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : copy_ex_async
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_24"
static VOID pt_im_xch_do_test_2_24( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "22", "1", "1", "3264", "3264", "2448", "1"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : fill
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_25"
static VOID pt_im_xch_do_test_2_25( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "23", "0", "3264", "640", "480", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : fill_ex_sync
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_26"
static VOID pt_im_xch_do_test_2_26( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "24", "0", "3264", "640", "480", "128", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : fill_ex_async
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_27"
static VOID pt_im_xch_do_test_2_27( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "24", "1", "3264", "640", "480", "0", "1"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : convert subsumpling sync (1/1 subsumpling)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_28"
static VOID pt_im_xch_do_test_2_28( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "25", "0", "0", "0", "3264", "3264", "2448", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : convert subsumpling sync (1/2 subsumpling)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_29"
static VOID pt_im_xch_do_test_2_29( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "25", "0", "1", "1", "1632", "3264", "2448", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, (3264*2448)/4 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : convert subsumpling sync (1/4 subsumpling)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_30"
static VOID pt_im_xch_do_test_2_30( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "25", "0", "2", "2", "816", "3264", "2448", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, (3264*2448)/16 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : convert subsumpling sync (3/4 subsumpling)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_31"
static VOID pt_im_xch_do_test_2_31( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "25", "0", "3", "3", "2448", "3264", "2448", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 2448*1836 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : convert subsumpling async (1/2 subsumpling)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_32"
static VOID pt_im_xch_do_test_2_32( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "25", "1", "1", "1", "1632", "3264", "2448", "1"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, (3264*2448)/4 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : histogram sync
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_33"
static VOID pt_im_xch_do_test_2_33( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "26", "3264", "2448", "0"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : histogram async
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_34"
static VOID pt_im_xch_do_test_2_34( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "26", "3264", "2448", "1"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : copy dual channel
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_35"
static VOID pt_im_xch_do_test_2_35( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "27", "65536", "1024"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : copy dual channel (fill-1)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_36"
static VOID pt_im_xch_do_test_2_36( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "27", "2048", "8"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : copy dual channel (fill-2)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_37"
static VOID pt_im_xch_do_test_2_37( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "27", "1024", "8"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

// utility : copy dual channel (fill-3)
#undef D_IM_XCH_TEST_NO
#define D_IM_XCH_TEST_NO "2_38"
static VOID pt_im_xch_do_test_2_38( VOID )
{
	char test_argv_1[][D_PT_XCH_CMD_LEN] = {"imxch", "27", "512", "8"};

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " begin\n" ));

	// 入力データ
	Palladium_Set_In_Localstack( D_CT_XCH_SRC_TOP_ADDR, 3264*2448 );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* XCH begin CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));
	pt_im_xch_execute_cmd( (sizeof(test_argv_1) / D_PT_XCH_CMD_LEN), (char*)test_argv_1 );
	Ddim_Print(( "* XCH end CT_Im_Xch_" D_IM_XCH_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_CT_XCH_DST_TOP_ADDR, 3264*2448 );

	Ddim_Print(( "*** XCH test " D_IM_XCH_TEST_NO " end\n" ));
}

VOID Pt_Im_XCH_Main( VOID )
{
	UCHAR			type1;

	// 開始...
	Ddim_Print(( "****** XCH begin Pt_Im_XCH_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	type1	= gDDIM_Info.com._6a;


	Ddim_Print(( "*** XCH begin Im_XCH_Init\n" ));
	Im_Xch_Init();
	Ddim_Print(( "*** XCH end Im_XCH_Init\n" ));

	pt_im_xch_enable_gic();
	pt_im_xch_start_clock();

	switch( type1 ) {
		case 1:
			pt_im_xch_do_test_2_1();
			Ddim_Print(( "\n" ));
			break;
		case 2:
			pt_im_xch_do_test_2_2();
			Ddim_Print(( "\n" ));
			break;
		case 3:
			pt_im_xch_do_test_2_3();
			Ddim_Print(( "\n" ));
			break;
		case 4:
			pt_im_xch_do_test_2_4();
			Ddim_Print(( "\n" ));
			break;
		case 5:
			pt_im_xch_do_test_2_5();
			Ddim_Print(( "\n" ));
			break;
		case 6:
			pt_im_xch_do_test_2_6();
			Ddim_Print(( "\n" ));
			break;
		case 7:
			pt_im_xch_do_test_2_7();
			Ddim_Print(( "\n" ));
			break;
		case 8:
			pt_im_xch_do_test_2_8();
			Ddim_Print(( "\n" ));
			break;
		case 9:
			pt_im_xch_do_test_2_9();
			Ddim_Print(( "\n" ));
			break;
		case 10:
			pt_im_xch_do_test_2_10();
			Ddim_Print(( "\n" ));
			break;
		case 11:
			pt_im_xch_do_test_2_11();
			Ddim_Print(( "\n" ));
			break;
		case 12:
			pt_im_xch_do_test_2_12();
			Ddim_Print(( "\n" ));
			break;
		case 13:
			pt_im_xch_do_test_2_13();
			Ddim_Print(( "\n" ));
			break;
		case 14:
			pt_im_xch_do_test_2_14();
			Ddim_Print(( "\n" ));
			break;
		case 15:
			pt_im_xch_do_test_2_15();
			Ddim_Print(( "\n" ));
			break;
		case 16:
			pt_im_xch_do_test_2_16();
			Ddim_Print(( "\n" ));
			break;
		case 17:
			pt_im_xch_do_test_2_17();
			Ddim_Print(( "\n" ));
			break;
		case 18:
			pt_im_xch_do_test_2_18();
			Ddim_Print(( "\n" ));
			break;
		case 19:
			pt_im_xch_do_test_2_19();
			Ddim_Print(( "\n" ));
			break;
		case 20:
			pt_im_xch_do_test_2_20();
			Ddim_Print(( "\n" ));
			break;
		case 21:
			pt_im_xch_do_test_2_21();
			Ddim_Print(( "\n" ));
			break;
		case 22:
			pt_im_xch_do_test_2_22();
			Ddim_Print(( "\n" ));
			break;
		case 23:
			pt_im_xch_do_test_2_23();
			Ddim_Print(( "\n" ));
			break;
		case 24:
			pt_im_xch_do_test_2_24();
			Ddim_Print(( "\n" ));
			break;
		case 25:
			pt_im_xch_do_test_2_25();
			Ddim_Print(( "\n" ));
			break;
		case 26:
			pt_im_xch_do_test_2_26();
			Ddim_Print(( "\n" ));
			break;
		case 27:
			pt_im_xch_do_test_2_27();
			Ddim_Print(( "\n" ));
			break;
		case 28:
			pt_im_xch_do_test_2_28();
			Ddim_Print(( "\n" ));
			break;
		case 29:
			pt_im_xch_do_test_2_29();
			Ddim_Print(( "\n" ));
			break;
		case 30:
			pt_im_xch_do_test_2_30();
			Ddim_Print(( "\n" ));
			break;
		case 31:
			pt_im_xch_do_test_2_31();
			Ddim_Print(( "\n" ));
			break;
		case 32:
			pt_im_xch_do_test_2_32();
			Ddim_Print(( "\n" ));
			break;
		case 33:
			pt_im_xch_do_test_2_33();
			Ddim_Print(( "\n" ));
			break;
		case 34:
			pt_im_xch_do_test_2_34();
			Ddim_Print(( "\n" ));
			break;
		case 35:
			pt_im_xch_do_test_2_35();
			Ddim_Print(( "\n" ));
			break;
		case 36:
			pt_im_xch_do_test_2_36();
			Ddim_Print(( "\n" ));
			break;
		case 37:
			pt_im_xch_do_test_2_37();
			Ddim_Print(( "\n" ));
			break;
		case 38:
			pt_im_xch_do_test_2_38();
			Ddim_Print(( "\n" ));
			break;
		default:
			Ddim_Print(("*** XCH test skip\n" ));
			break;
	}

	// タイマーログ出力
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );

	// テスト強制終了処理
	Palladium_Force_Stop();
}

