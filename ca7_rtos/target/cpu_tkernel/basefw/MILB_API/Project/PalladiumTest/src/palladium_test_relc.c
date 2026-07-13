/**
 * @file		palladium_test_relc.c
 * @brief		palladium test code for relc.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ddim_user_custom.h"
#include "palladium_test_common.h"
#include "palladium_test.h"
#include "dd_gic.h"
#include "dd_top.h"

#include "ct_dd_relc.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PT_DD_RELC_CMD_LEN			(20)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static char* g_dd_relc_Cmd_Argv[20]; 

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

static void pt_dd_relc_execute_cmd(int cmd_argc, char* cmd_argv) 
{ 
	INT32 j; 
	INT32 cmd_pos = 0; 

	for (j = 0; j < cmd_argc; j++) { 
		g_dd_relc_Cmd_Argv[j] = (cmd_argv + (cmd_pos * D_PT_DD_RELC_CMD_LEN)); 
		cmd_pos++; 
	}

	Ct_Dd_RELC_Main(cmd_argc, g_dd_relc_Cmd_Argv); 
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		sample code
 * @param		void
 * @return		void
 * @note		None
 * @attention	None
 */
VOID Pt_Dd_RELC_Main( VOID )
{
	UCHAR	testno;
	
	// 開始ログ
	Ddim_Print(( "RELC Palladium test Start\n" ));

	// タイマー初期化(処理時間計測用)
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得(RSファイルに紐付く)
	testno	= gDDIM_Info.com._6a;

	Ddim_Print(( "RELC Palladium test No %d Start\n", testno ));

	// レジスタ取得＆設定(#define PC_DEBUG を定義すること)
	/******************************************************************/
	if ( testno == 1 ) {	// 1-1-1

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

	/******************************************************************/
	} else if ( testno == 2 ) {		// 1-1-2 , 1-1-3

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);

	/******************************************************************/
	} else if ( testno == 3 ) {		// 1-1-4

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "start", "sync"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 4 ) {		// 1-1-5

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "start", "async"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 5 ) {		// 1-1-6

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "stop"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 5 ) {		// 1-1-7

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "ctrlcomm", "reg", "nmod", "1"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 7 ) {		// 1-1-8

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "ctrlcomm", "desc", "cmod", "2"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 8 ) {		// 1-1-9

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "ctrlreg", "imaxd", "omaxd", "0x40100000", "0x40180000", "0x40200000", "0x402C0000"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 9 ) {		// 1-1-10

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "ctrlreg", "imaxe", "omaxe", "0x40100000", "0x40180000", "0x40200000", "0x402C0000"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 10 ) {		// 1-1-11

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "ctrldesc", "0x40408000"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 11 ) {		// 1-1-12

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "set", "in_sta_addr", "0x40120000"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 12 ) {		// 1-1-13

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "set", "in_end_addr", "0x401A0000"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 13 ) {		// 1-1-14

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "set", "out_sta_addr", "0x40220000"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 14 ) {		// 1-1-15

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "set", "out_end_addr", "0x402E0000"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 15 ) {		// 1-1-16

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "ctrlcomm", "reg", "nmod", "1"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "get", "ctrlcomm"};
		char test_argv_05[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);
		pt_dd_relc_execute_cmd((sizeof(test_argv_05) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_05);

	/******************************************************************/
	} else if ( testno == 16 ) {		// 1-1-17

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "ctrlreg", "imaxd", "omaxd", "0x40100000", "0x40180000", "0x40200000", "0x402C0000"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "get", "ctrlreg"};
		char test_argv_05[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);
		pt_dd_relc_execute_cmd((sizeof(test_argv_05) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_05);

	/******************************************************************/
	} else if ( testno == 17 ) {		// 1-1-18

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "ctrldesc", "0x40408000"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "get", "ctrldesc"};
		char test_argv_05[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "close"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);
		pt_dd_relc_execute_cmd((sizeof(test_argv_05) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_05);

	/******************************************************************/
	} else if ( testno == 18 ) {		// 1-1-19

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "get", "msta"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

	/******************************************************************/
	} else if ( testno == 19 ) {		// 1-1-20

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "get", "sleep"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

	/******************************************************************/
	} else if ( testno == 20 ) {		// 1-1-21

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "get", "errsta"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

	/******************************************************************/
	} else if ( testno == 21 ) {		// 1-1-22

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "get", "err"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

	/******************************************************************/
	} else if ( testno == 22 ) {		// 1-1-23

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "get", "psta"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

	/******************************************************************/
	} else if ( testno == 23 ) {		// 1-1-24

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "get", "bufsize"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);

	/******************************************************************/
	} else if ( testno == 24 ) {		// 1-1-25

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "set", "inbuf", "0x58000000", "64"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);

	/******************************************************************/
	} else if ( testno == 25 ) {		// 1-1-26

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "set", "inbufm", "0x58000000", "64"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);

	/******************************************************************/
	} else if ( testno == 26 ) {		// 1-1-27

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "set", "refbuf", "0x58000000", "8192"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);

	/******************************************************************/
	} else if ( testno == 27 ) {		// 1-1-28

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "get", "inbuf", "0x58000000", "64"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);

	/******************************************************************/
	} else if ( testno == 28 ) {		// 1-1-29

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "get", "inbufm", "0x58000000", "64"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);

	/******************************************************************/
	} else if ( testno == 29 ) {		// 1-1-30

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "init", "B", "B"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "open"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "get", "refbuf", "0x58000000", "8192"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);

	/******************************************************************/
	} else if ( testno == 30 ) {		// 1-1-31

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "norm", "async", "1"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

	/******************************************************************/
	} else if ( testno == 31 ) {		// 1-1-32

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "norm", "async", "2"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

	/******************************************************************/
	} else if ( testno == 32 ) {		// 1-1-33

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "desc", "async", "1"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

	/******************************************************************/
	} else if ( testno == 33 ) {		// 1-1-34

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "desc", "async", "2"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

	/******************************************************************/
	} else if ( testno == 34 ) {		// 1-1-35

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "norm", "sync", "1"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

	/******************************************************************/
	} else if ( testno == 35 ) {		// 1-1-36

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "norm", "sync", "2"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

	/******************************************************************/
	} else if ( testno == 36 ) {		// 1-1-37

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "desc", "sync", "1"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

	/******************************************************************/
	} else if ( testno == 37 ) {		// 1-1-38

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "desc", "sync", "2"};

		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

	/******************************************************************/
	} else if ( testno == 38 ) {		// 1-2-1 -- 1-2-20

		char test_argv_01[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "open"};
		char test_argv_02[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "ctrlcomm"};
		char test_argv_03[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "ctrlreg"};
		char test_argv_04[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "ctrldesc"};
		char test_argv_05[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "set_in_sta_addr"};
		char test_argv_06[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "set_in_end_addr"};
		char test_argv_07[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "set_out_sta_addr"};
		char test_argv_08[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "set_out_end_addr"};
		char test_argv_09[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "set_inbuf"};
		char test_argv_10[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "set_inbufm"};
		char test_argv_11[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "set_refbuf"};
		char test_argv_12[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "get_ctrlcomm"};
		char test_argv_13[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "get_ctrlreg"};
		char test_argv_14[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "get_ctrldesc"};
		char test_argv_15[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "get_psta"};
		char test_argv_16[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "get_bufsize1"};
		char test_argv_17[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "get_bufsize2"};
		char test_argv_18[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "get_inbuf"};
		char test_argv_19[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "get_inbufm"};
		char test_argv_20[][D_PT_DD_RELC_CMD_LEN]  = {"ddrelc", "err", "get_refbuf"};

		Ddim_Print(("RELC Err Confirm 1-2-1 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_01) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_01);

		Ddim_Print(("RELC Err Confirm 1-2-2 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_02) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_02);

		Ddim_Print(("RELC Err Confirm 1-2-3 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_03) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_03);

		Ddim_Print(("RELC Err Confirm 1-2-4 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_04) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_04);

		Ddim_Print(("RELC Err Confirm 1-2-5 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_05) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_05);

		Ddim_Print(("RELC Err Confirm 1-2-6 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_06) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_06);

		Ddim_Print(("RELC Err Confirm 1-2-7 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_07) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_07);

		Ddim_Print(("RELC Err Confirm 1-2-8 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_08) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_08);

		Ddim_Print(("RELC Err Confirm 1-2-9 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_09) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_09);

		Ddim_Print(("RELC Err Confirm 1-2-10 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_10) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_10);

		Ddim_Print(("RELC Err Confirm 1-2-11 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_11) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_11);

		Ddim_Print(("RELC Err Confirm 1-2-12 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_12) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_12);

		Ddim_Print(("RELC Err Confirm 1-2-13 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_13) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_13);

		Ddim_Print(("RELC Err Confirm 1-2-14 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_14) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_14);

		Ddim_Print(("RELC Err Confirm 1-2-15 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_15) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_15);

		Ddim_Print(("RELC Err Confirm 1-2-16 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_16) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_16);

		Ddim_Print(("RELC Err Confirm 1-2-17 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_17) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_17);

		Ddim_Print(("RELC Err Confirm 1-2-18 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_18) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_18);

		Ddim_Print(("RELC Err Confirm 1-2-19 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_19) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_19);

		Ddim_Print(("RELC Err Confirm 1-2-20 \n"));
		pt_dd_relc_execute_cmd((sizeof(test_argv_20) / D_PT_DD_RELC_CMD_LEN), (char*)test_argv_20);

	/******************************************************************/
	} else {
		// 例外パラメータ指定
		Ddim_Print(( "Parameters of the excluded\n" ));
	}
	// タイマーログ出力(処理時間出力)
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );

	Ddim_Print(( "RELC Palladium test No %d End\n", testno ));

	// Force stop
	Palladium_Force_Stop();
}
