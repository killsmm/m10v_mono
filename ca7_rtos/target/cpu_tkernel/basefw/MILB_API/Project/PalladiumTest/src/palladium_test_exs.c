/**
 * @file		palladium_test_exs.c
 * @brief		palladium test code for exs.
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

#include "ct_dd_exs.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PT_DD_EXS_CMD_LEN			(20)

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
static char* g_dd_exs_Cmd_Argv[20]; 

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

static void pt_dd_exs_execute_cmd(int cmd_argc, char* cmd_argv) 
{ 
	INT32 j; 
	INT32 cmd_pos = 0; 

	for (j = 0; j < cmd_argc; j++) { 
		g_dd_exs_Cmd_Argv[j] = (cmd_argv + (cmd_pos * D_PT_DD_EXS_CMD_LEN)); 
		cmd_pos++; 
	}

	Ct_Dd_EXS_Main(cmd_argc, g_dd_exs_Cmd_Argv); 
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
VOID Pt_Dd_EXS_Main( VOID )
{
	UCHAR	testno;
	
	// 開始ログ
	Ddim_Print(( "EXS Palladium test Start\n" ));

	// タイマー初期化(処理時間計測用)
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得(RSファイルに紐付く)
	testno	= gDDIM_Info.com._6a;

	Ddim_Print(( "EXS Palladium test No %d Start\n", testno ));

	// レジスタ取得＆設定(#define PC_DEBUG を定義すること)
	/******************************************************************/
	if ( testno == 1 ) {	// "XD_LPC (XDMAC LowPower Control)"レジスタのテスト

		// 1-1-1 , 1-1-2 , 1-1-3
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "xd_lpc", "csysreq", "0"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "xd_lpc", "csysreq"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "xd_lpc", "csysack"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "xd_lpc", "cactrive"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "xd_lpc", "csysreq", "1"};
		char test_argv01_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "xd_lpc", "csysreq"};
		char test_argv01_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "xd_lpc", "csysack"};
		char test_argv01_08[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "xd_lpc", "cactrive"};
		char test_argv01_09[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "xd_lpc", "csysreq", "0"};
		char test_argv01_10[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "xd_lpc", "csysreq"};
		char test_argv01_11[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "xd_lpc", "csysack"};
		char test_argv01_12[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "xd_lpc", "cactrive"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_07);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_08) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_08);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_09) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_09);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_10) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_10);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_11) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_11);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_12) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_12);

	/******************************************************************/
	} else if ( testno == 2 ) {		// "SOFTRESET"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "nfrstn", "0"};
		char test_argv00_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "nfrstnbch", "0"};
		char test_argv00_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "nfrstnreg", "0"};
		char test_argv00_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "netsecrst", "0"};
		char test_argv00_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "emcrst", "0"};
		char test_argv00_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "relcrst", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_06);

		// 1-1-4
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "nfrstn"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "nfrstn", "1"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "nfrstn"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "nfrstn", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "nfrstn"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

		// 1-1-5
		char test_argv02_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "nfrstnbch"};
		char test_argv02_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "nfrstnbch", "1"};
		char test_argv02_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "nfrstnbch"};
		char test_argv02_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "nfrstnbch", "0"};
		char test_argv02_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "nfrstnbch"};

		pt_dd_exs_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_05);

		// 1-1-6
		char test_argv03_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "nfrstnreg"};
		char test_argv03_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "nfrstnreg", "1"};
		char test_argv03_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "nfrstnreg"};
		char test_argv03_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "nfrstnreg", "0"};
		char test_argv03_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "nfrstnreg"};

		pt_dd_exs_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_05);

		// 1-1-7
		char test_argv04_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "netsecrst"};
		char test_argv04_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "netsecrst", "1"};
		char test_argv04_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "netsecrst"};
		char test_argv04_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "netsecrst", "0"};
		char test_argv04_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "netsecrst"};

		pt_dd_exs_execute_cmd((sizeof(test_argv04_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_05);

		// 1-1-8
		char test_argv05_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "emcrst"};
		char test_argv05_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "emcrst", "1"};
		char test_argv05_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "emcrst"};
		char test_argv05_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "emcrst", "0"};
		char test_argv05_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "emcrst"};

		pt_dd_exs_execute_cmd((sizeof(test_argv05_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_05);

		// 1-1-9
		char test_argv06_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "relcrst"};
		char test_argv06_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "relcrst", "1"};
		char test_argv06_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "relcrst"};
		char test_argv06_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "relcrst", "0"};
		char test_argv06_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset", "relcrst"};

		pt_dd_exs_execute_cmd((sizeof(test_argv06_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_05);

	/******************************************************************/
	} else if ( testno == 3 ) {		// "SOFTRESET2"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_sd3", "0"};
		char test_argv00_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_sd4", "0"};
		char test_argv00_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_pcie0", "0"};
		char test_argv00_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_pcie1", "0"};
		char test_argv00_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_usb2vbus", "0"};
		char test_argv00_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_usb2id", "0"};
		char test_argv00_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_usb3vbus", "0"};
		char test_argv00_08[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_usb3id", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_07);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_08) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_08);

		// 1-1-10
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_sd3"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_sd3", "1"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_sd3"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_sd3", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_sd3"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

		// 1-1-11
		char test_argv02_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_sd4"};
		char test_argv02_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_sd4", "1"};
		char test_argv02_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_sd3"};
		char test_argv02_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_sd4", "0"};
		char test_argv02_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_sd3"};

		pt_dd_exs_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_05);

		// 1-1-12
		char test_argv03_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_pcie0"};
		char test_argv03_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_pcie0", "1"};
		char test_argv03_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_pcie0"};
		char test_argv03_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_pcie0", "0"};
		char test_argv03_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_pcie0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_05);

		// 1-1-13
		char test_argv04_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_pcie1"};
		char test_argv04_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_pcie1", "1"};
		char test_argv04_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_pcie1"};
		char test_argv04_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_pcie1", "0"};
		char test_argv04_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_pcie1"};

		pt_dd_exs_execute_cmd((sizeof(test_argv04_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_05);

		// 1-1-14
		char test_argv05_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_usb2vbus"};
		char test_argv05_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_usb2vbus", "1"};
		char test_argv05_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_usb2vbus"};
		char test_argv05_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_usb2vbus", "0"};
		char test_argv05_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_usb2vbus"};

		pt_dd_exs_execute_cmd((sizeof(test_argv05_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_05);

		// 1-1-15
		char test_argv06_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_usb2id"};
		char test_argv06_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_usb2id", "1"};
		char test_argv06_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_usb2id"};
		char test_argv06_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_usb2id", "0"};
		char test_argv06_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_usb2id"};

		pt_dd_exs_execute_cmd((sizeof(test_argv06_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_05);

		// 1-1-16
		char test_argv07_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_usb3vbus"};
		char test_argv07_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_usb3vbus", "1"};
		char test_argv07_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_usb3vbus"};
		char test_argv07_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_usb3vbus", "0"};
		char test_argv07_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_usb3vbus"};

		pt_dd_exs_execute_cmd((sizeof(test_argv07_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_05);

		// 1-1-17
		char test_argv08_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_usb3id"};
		char test_argv08_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_usb3id", "1"};
		char test_argv08_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_usb3id"};
		char test_argv08_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_usb3id", "0"};
		char test_argv08_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "softreset2", "debretx_usb3id"};

		pt_dd_exs_execute_cmd((sizeof(test_argv08_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_05);

	/******************************************************************/
	} else if ( testno == 4 ) {	 		// "INTMSK"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk02", "0"};
		char test_argv00_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk03", "0"};
		char test_argv00_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk04", "0"};
		char test_argv00_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk05", "0"};
		char test_argv00_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk06", "0"};
		char test_argv00_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk07", "0"};
		char test_argv00_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk08", "0"};
		char test_argv00_08[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk09", "0"};
		char test_argv00_09[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk10", "0"};
		char test_argv00_10[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk11", "0"};
		char test_argv00_11[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk12", "0"};
		char test_argv00_12[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk13", "0"};
		char test_argv00_13[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk14", "0"};
		char test_argv00_14[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk15", "0"};
		char test_argv00_15[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk16", "0"};
		char test_argv00_16[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk17", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_07);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_08) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_08);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_09) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_09);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_10) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_10);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_11) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_11);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_12) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_12);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_13) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_13);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_14) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_14);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_15) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_15);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_16) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_16);

		// 1-1-18
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk02"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk02", "1"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk02"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk02", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk02"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

		// 1-1-19
		char test_argv02_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk03"};
		char test_argv02_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk03", "1"};
		char test_argv02_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk03"};
		char test_argv02_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk03", "0"};
		char test_argv02_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk03"};

		pt_dd_exs_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_05);

		// 1-1-20
		char test_argv03_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk04"};
		char test_argv03_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk04", "1"};
		char test_argv03_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk04"};
		char test_argv03_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk04", "0"};
		char test_argv03_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk04"};

		pt_dd_exs_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_05);

		// 1-1-21
		char test_argv04_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk05"};
		char test_argv04_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk05", "1"};
		char test_argv04_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk05"};
		char test_argv04_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk05", "0"};
		char test_argv04_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk05"};

		pt_dd_exs_execute_cmd((sizeof(test_argv04_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_05);

		// 1-1-22
		char test_argv05_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk06"};
		char test_argv05_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk06", "1"};
		char test_argv05_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk06"};
		char test_argv05_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk06", "0"};
		char test_argv05_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk06"};

		pt_dd_exs_execute_cmd((sizeof(test_argv05_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_05);

		// 1-1-23
		char test_argv06_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk07"};
		char test_argv06_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk07", "1"};
		char test_argv06_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk07"};
		char test_argv06_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk07", "0"};
		char test_argv06_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk07"};

		pt_dd_exs_execute_cmd((sizeof(test_argv06_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_05);

		// 1-1-24
		char test_argv07_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk08"};
		char test_argv07_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk08", "1"};
		char test_argv07_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk08"};
		char test_argv07_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk08", "0"};
		char test_argv07_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk08"};

		pt_dd_exs_execute_cmd((sizeof(test_argv07_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_05);

		// 1-1-25
		char test_argv08_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk09"};
		char test_argv08_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk09", "1"};
		char test_argv08_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk09"};
		char test_argv08_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk09", "0"};
		char test_argv08_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk09"};

		pt_dd_exs_execute_cmd((sizeof(test_argv08_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_05);

		// 1-1-26
		char test_argv09_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk10"};
		char test_argv09_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk10", "1"};
		char test_argv09_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk10"};
		char test_argv09_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk10", "0"};
		char test_argv09_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk10"};

		pt_dd_exs_execute_cmd((sizeof(test_argv09_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_05);

		// 1-1-27
		char test_argv10_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk11"};
		char test_argv10_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk11", "1"};
		char test_argv10_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk11"};
		char test_argv10_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk11", "0"};
		char test_argv10_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk11"};

		pt_dd_exs_execute_cmd((sizeof(test_argv10_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_05);

		// 1-1-28
		char test_argv11_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk12"};
		char test_argv11_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk12", "1"};
		char test_argv11_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk12"};
		char test_argv11_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk12", "0"};
		char test_argv11_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk12"};

		pt_dd_exs_execute_cmd((sizeof(test_argv11_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_05);

		// 1-1-29
		char test_argv12_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk13"};
		char test_argv12_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk13", "1"};
		char test_argv12_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk13"};
		char test_argv12_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk13", "0"};
		char test_argv12_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk13"};

		pt_dd_exs_execute_cmd((sizeof(test_argv12_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv12_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv12_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv12_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv12_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv12_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv12_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv12_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv12_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv12_05);

		// 1-1-30
		char test_argv13_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk14"};
		char test_argv13_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk14", "1"};
		char test_argv13_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk14"};
		char test_argv13_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk14", "0"};
		char test_argv13_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk14"};

		pt_dd_exs_execute_cmd((sizeof(test_argv13_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv13_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv13_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv13_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv13_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv13_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv13_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv13_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv13_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv13_05);

		// 1-1-31
		char test_argv14_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk15"};
		char test_argv14_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk15", "1"};
		char test_argv14_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk15"};
		char test_argv14_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk15", "0"};
		char test_argv14_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk15"};

		pt_dd_exs_execute_cmd((sizeof(test_argv14_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv14_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv14_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv14_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv14_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv14_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv14_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv14_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv14_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv14_05);

		// 1-1-32
		char test_argv15_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk16"};
		char test_argv15_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk16", "1"};
		char test_argv15_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk16"};
		char test_argv15_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk16", "0"};
		char test_argv15_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk16"};

		pt_dd_exs_execute_cmd((sizeof(test_argv15_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv15_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv15_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv15_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv15_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv15_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv15_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv15_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv15_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv15_05);

		// 1-1-33
		char test_argv16_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk17"};
		char test_argv16_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk17", "1"};
		char test_argv16_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk17"};
		char test_argv16_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "intmask", "intmsk17", "0"};
		char test_argv16_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "intmask", "intmsk17"};

		pt_dd_exs_execute_cmd((sizeof(test_argv16_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv16_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv16_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv16_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv16_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv16_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv16_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv16_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv16_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv16_05);

	/******************************************************************/
	} else if ( testno == 5 ) {		// "SIGMON"レジスタのテスト
									// 媒体挿抜による動作は実機確認で実施すること

		// 1-1-34
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon_sd3"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);

		// 1-1-35
		char test_argv02_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon_sd4"};

		pt_dd_exs_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_01);

		// 1-1-36
		char test_argv03_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon_pcie0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_01);

		// 1-1-37
		char test_argv04_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon_pcie1"};

		pt_dd_exs_execute_cmd((sizeof(test_argv04_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_01);

		// 1-1-38
		char test_argv05_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon_usb2vbus"};

		pt_dd_exs_execute_cmd((sizeof(test_argv05_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_01);

		// 1-1-39
		char test_argv06_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon_usb2id"};

		pt_dd_exs_execute_cmd((sizeof(test_argv06_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_01);

		// 1-1-40
		char test_argv07_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon_usb3vbus"};

		pt_dd_exs_execute_cmd((sizeof(test_argv07_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_01);

		// 1-1-41
		char test_argv08_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon_usb3id"};

		pt_dd_exs_execute_cmd((sizeof(test_argv08_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_01);

		// 1-1-42
		char test_argv09_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon2_sd3"};

		pt_dd_exs_execute_cmd((sizeof(test_argv09_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_01);

		// 1-1-43
		char test_argv10_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon2_sd4"};

		pt_dd_exs_execute_cmd((sizeof(test_argv10_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_01);

		// 1-1-44
		char test_argv11_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon2_pcie0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv11_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_01);

		// 1-1-45
		char test_argv12_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon2_pcie1"};

		pt_dd_exs_execute_cmd((sizeof(test_argv12_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv12_01);

		// 1-1-46
		char test_argv13_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon2_usb2vbus"};

		pt_dd_exs_execute_cmd((sizeof(test_argv13_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv13_01);

		// 1-1-47
		char test_argv14_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon2_usb2id"};

		pt_dd_exs_execute_cmd((sizeof(test_argv14_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv14_01);

		// 1-1-48
		char test_argv15_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon2_usb3vbus"};

		pt_dd_exs_execute_cmd((sizeof(test_argv15_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv15_01);

		// 1-1-49
		char test_argv16_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "sigmon", "sigmon2_usb3id"};

		pt_dd_exs_execute_cmd((sizeof(test_argv16_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv16_01);

	/******************************************************************/
	} else if ( testno == 6 ) {		// "DEBEN"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_sd3", "0"};
		char test_argv00_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "debretx_sd4", "0"};
		char test_argv00_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "deben_pcie0", "0"};
		char test_argv00_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "deben_pcie1", "0"};
		char test_argv00_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "deben_usb2vbus", "0"};
		char test_argv00_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "deben_usb2id", "0"};
		char test_argv00_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "deben_usb3vbus", "0"};
		char test_argv00_08[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "deben_usb3id", "0"};
		char test_argv00_09[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "sd3detim", "0"};
		char test_argv00_10[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "sd4detim", "0"};
		char test_argv00_11[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "pcie0detim", "0"};
		char test_argv00_12[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "pcie1detim", "0"};
		char test_argv00_13[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "usb2vbdetim", "0"};
		char test_argv00_14[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "usb2iddetim", "0"};
		char test_argv00_15[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "usb3vbdetim", "0"};
		char test_argv00_16[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset2", "usb3iddetim", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_07);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_08) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_08);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_09) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_09);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_10) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_10);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_11) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_11);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_12) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_12);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_13) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_13);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_14) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_14);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_15) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_15);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_16) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_16);

		char test_argv00_21[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_sd3", "0"};
		char test_argv00_22[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_sd4", "0"};
		char test_argv00_23[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_pcie0", "0"};
		char test_argv00_24[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_pcie1", "0"};
		char test_argv00_25[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_usb2vbus", "0"};
		char test_argv00_26[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_usb2id", "0"};
		char test_argv00_27[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_usb3vbus", "0"};
		char test_argv00_28[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_usb3id", "0"};
		char test_argv00_29[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "sd3detim", "0"};
		char test_argv00_30[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "sd4detim", "0"};
		char test_argv00_31[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "pcie0detim", "0"};
		char test_argv00_32[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "pcie1detim", "0"};
		char test_argv00_33[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "usb2vbdetim", "0"};
		char test_argv00_34[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "usb2iddetim", "0"};
		char test_argv00_35[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "usb3vbdetim", "0"};
		char test_argv00_36[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "usb3iddetim", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_21) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_21);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_22) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_22);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_23) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_23);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_24) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_24);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_25) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_25);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_26) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_26);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_27) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_27);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_28) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_28);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_29) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_29);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_30) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_30);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_31) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_31);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_32) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_32);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_33) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_33);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_34) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_34);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_35) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_35);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_36) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_36);

		// 1-1-50
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_sd3"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_sd3", "1"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_sd3"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_sd3", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_sd3"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

		// 1-1-51
		char test_argv02_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_sd4"};
		char test_argv02_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_sd4", "1"};
		char test_argv02_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_sd4"};
		char test_argv02_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_sd4", "0"};
		char test_argv02_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_sd4"};

		pt_dd_exs_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_05);

		// 1-1-52
		char test_argv03_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_pcie0"};
		char test_argv03_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_pcie0", "1"};
		char test_argv03_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_pcie0"};
		char test_argv03_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_pcie0", "0"};
		char test_argv03_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_pcie0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_05);

		// 1-1-53
		char test_argv04_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_pcie1"};
		char test_argv04_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_pcie1", "3"};
		char test_argv04_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_pcie1"};
		char test_argv04_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_pcie1", "0"};
		char test_argv04_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_pcie1"};

		pt_dd_exs_execute_cmd((sizeof(test_argv04_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_05);

		// 1-1-54
		char test_argv05_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_usb2vbus"};
		char test_argv05_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_usb2vbus", "3"};
		char test_argv05_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_usb2vbus"};
		char test_argv05_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_usb2vbus", "0"};
		char test_argv05_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_usb2vbus"};

		pt_dd_exs_execute_cmd((sizeof(test_argv05_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_05);

		// 1-1-55
		char test_argv06_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_usb2id"};
		char test_argv06_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_usb2id", "3"};
		char test_argv06_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_usb2id"};
		char test_argv06_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_usb2id", "0"};
		char test_argv06_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_usb2id"};

		pt_dd_exs_execute_cmd((sizeof(test_argv06_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_05);

		// 1-1-56
		char test_argv07_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_usb3vbus"};
		char test_argv07_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_usb3vbus", "3"};
		char test_argv07_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_usb3vbus"};
		char test_argv07_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_usb3vbus", "0"};
		char test_argv07_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_usb3vbus"};

		pt_dd_exs_execute_cmd((sizeof(test_argv07_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_05);

		// 1-1-57
		char test_argv08_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_usb3id"};
		char test_argv08_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_usb3id", "3"};
		char test_argv08_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_usb3id"};
		char test_argv08_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "deben_usb3id", "0"};
		char test_argv08_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "deben_usb3id"};

		pt_dd_exs_execute_cmd((sizeof(test_argv08_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_05);

		// 1-1-58
		char test_argv09_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "sd3detim"};
		char test_argv09_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "sd3detim", "3"};
		char test_argv09_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "sd3detim"};
		char test_argv09_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "sd3detim", "0"};
		char test_argv09_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "sd3detim"};

		pt_dd_exs_execute_cmd((sizeof(test_argv09_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_05);

		// 1-1-59
		char test_argv10_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "sd4detim"};
		char test_argv10_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "sd4detim", "3"};
		char test_argv10_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "sd4detim"};
		char test_argv10_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "sd4detim", "0"};
		char test_argv10_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "sd4detim"};

		pt_dd_exs_execute_cmd((sizeof(test_argv10_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_05);

		// 1-1-60
		char test_argv11_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "pcie0detim"};
		char test_argv11_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "pcie0detim", "3"};
		char test_argv11_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "pcie0detim"};
		char test_argv11_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "pcie0detim", "0"};
		char test_argv11_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "pcie0detim"};

		pt_dd_exs_execute_cmd((sizeof(test_argv11_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_05);

		// 1-1-61
		char test_argv12_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "pcie1detim"};
		char test_argv12_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "pcie1detim", "3"};
		char test_argv12_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "pcie1detim"};
		char test_argv12_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "pcie1detim", "0"};
		char test_argv12_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "pcie1detim"};

		pt_dd_exs_execute_cmd((sizeof(test_argv12_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv12_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv12_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv12_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv12_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv12_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv12_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv12_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv12_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv12_05);

		// 1-1-62
		char test_argv13_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "usb2vbdetim"};
		char test_argv13_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "usb2vbdetim", "3"};
		char test_argv13_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "usb2vbdetim"};
		char test_argv13_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "usb2vbdetim", "0"};
		char test_argv13_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "usb2vbdetim"};

		pt_dd_exs_execute_cmd((sizeof(test_argv13_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv13_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv13_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv13_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv13_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv13_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv13_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv13_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv13_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv13_05);

		// 1-1-63
		char test_argv14_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "usb2iddetim"};
		char test_argv14_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "usb2iddetim", "3"};
		char test_argv14_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "usb2iddetim"};
		char test_argv14_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "usb2iddetim", "0"};
		char test_argv14_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "usb2iddetim"};

		pt_dd_exs_execute_cmd((sizeof(test_argv14_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv14_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv14_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv14_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv14_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv14_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv14_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv14_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv14_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv14_05);

		// 1-1-64
		char test_argv15_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "usb3vbdetim"};
		char test_argv15_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "usb3vbdetim", "3"};
		char test_argv15_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "usb3vbdetim"};
		char test_argv15_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "usb3vbdetim", "0"};
		char test_argv15_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "usb3vbdetim"};

		pt_dd_exs_execute_cmd((sizeof(test_argv15_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv15_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv15_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv15_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv15_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv15_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv15_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv15_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv15_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv15_05);

		// 1-1-65
		char test_argv16_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "usb3iddetim"};
		char test_argv16_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "usb3iddetim", "3"};
		char test_argv16_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "usb3iddetim"};
		char test_argv16_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "deben", "usb3iddetim", "0"};
		char test_argv16_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "deben", "usb3iddetim"};

		pt_dd_exs_execute_cmd((sizeof(test_argv16_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv16_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv16_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv16_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv16_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv16_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv16_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv16_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv16_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv16_05);

	/******************************************************************/
	} else if ( testno == 7 ) {		// "CDINSMDCTL"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_sd3", "0"};
		char test_argv00_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_sd4", "0"};
		char test_argv00_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_pcie0", "0"};
		char test_argv00_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_pcie1", "0"};
		char test_argv00_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_usb2vbus", "0"};
		char test_argv00_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_usb2id", "0"};
		char test_argv00_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_usb3vbus", "0"};
		char test_argv00_08[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_usb3id", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_07);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_08) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_08);

		// 1-1-66
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_sd3"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_sd3", "1"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_sd3"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_sd3", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_sd3"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

		// 1-1-67
		char test_argv02_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_sd4"};
		char test_argv02_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_sd4", "1"};
		char test_argv02_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_sd4"};
		char test_argv02_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_sd4", "0"};
		char test_argv02_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_sd4"};

		pt_dd_exs_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_05);

		// 1-1-68
		char test_argv03_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_pcie0"};
		char test_argv03_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_pcie0", "1"};
		char test_argv03_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_pcie0"};
		char test_argv03_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_pcie0", "0"};
		char test_argv03_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_pcie0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_05);

		// 1-1-69
		char test_argv04_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_pcie1"};
		char test_argv04_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_pcie1", "1"};
		char test_argv04_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_pcie1"};
		char test_argv04_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_pcie1", "0"};
		char test_argv04_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_pcie1"};

		pt_dd_exs_execute_cmd((sizeof(test_argv04_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_05);

		// 1-1-70
		char test_argv05_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_usb2vbus"};
		char test_argv05_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_usb2vbus", "1"};
		char test_argv05_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_usb2vbus"};
		char test_argv05_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_usb2vbus", "0"};
		char test_argv05_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_usb2vbus"};

		pt_dd_exs_execute_cmd((sizeof(test_argv05_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_05);

		// 1-1-71
		char test_argv06_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_usb2id"};
		char test_argv06_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_usb2id", "1"};
		char test_argv06_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_usb2id"};
		char test_argv06_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_usb2id", "0"};
		char test_argv06_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_usb2id"};

		pt_dd_exs_execute_cmd((sizeof(test_argv06_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_05);

		// 1-1-72
		char test_argv07_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_usb3vbus"};
		char test_argv07_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_usb3vbus", "1"};
		char test_argv07_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_usb3vbus"};
		char test_argv07_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_usb3vbus", "0"};
		char test_argv07_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_usb3vbus"};

		pt_dd_exs_execute_cmd((sizeof(test_argv07_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_05);

		// 1-1-73
		char test_argv08_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_usb3id"};
		char test_argv08_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_usb3id", "1"};
		char test_argv08_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_usb3id"};
		char test_argv08_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "cdinsmdctl", "cdmod_usb3id", "0"};
		char test_argv08_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "cdinsmdctl", "cdmod_usb3id"};

		pt_dd_exs_execute_cmd((sizeof(test_argv08_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_05);

	/******************************************************************/
	} else if ( testno == 8 ) {		// "RLC_HPT_CTL"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "rlc_hpt_ctl", "rerhpt", "0"};
		char test_argv00_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "rlc_hpt_ctl", "rewhpt", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_02);

		// 1-1-74
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "rlc_hpt_ctl", "rerhpt"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "rlc_hpt_ctl", "rerhpt", "15"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "rlc_hpt_ctl", "rerhpt"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "rlc_hpt_ctl", "rerhpt", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "rlc_hpt_ctl", "rerhpt"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

		// 1-1-75
		char test_argv02_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "rlc_hpt_ctl", "rewhpt"};
		char test_argv02_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "rlc_hpt_ctl", "rewhpt", "15"};
		char test_argv02_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "rlc_hpt_ctl", "rewhpt"};
		char test_argv02_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "rlc_hpt_ctl", "rewhpt", "0"};
		char test_argv02_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "rlc_hpt_ctl", "rewhpt"};

		pt_dd_exs_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_05);

	/******************************************************************/
	} else if ( testno == 9 ) {		// "RAM_PD"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_sd2", "0"};
		char test_argv00_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_sd3", "0"};
		char test_argv00_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_sd4", "0"};
		char test_argv00_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_nf", "0"};
		char test_argv00_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_relc", "0"};
		char test_argv00_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_netsec", "0"};
		char test_argv00_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_usb2", "0"};
		char test_argv00_08[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_usb3", "0"};
		char test_argv00_09[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_pcie0", "0"};
		char test_argv00_10[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_pcie1", "0"};
		char test_argv00_11[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_emmc", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_07);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_08) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_08);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_09) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_09);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_10) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_10);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_11) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_11);

		// 1-1-76
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_sd2"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_sd2", "1"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_sd2"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_sd2", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_sd2"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

		// 1-1-77
		char test_argv02_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_sd3"};
		char test_argv02_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_sd3", "1"};
		char test_argv02_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_sd3"};
		char test_argv02_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_sd3", "0"};
		char test_argv02_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_sd3"};

		pt_dd_exs_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_05);

		// 1-1-78
		char test_argv03_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_sd4"};
		char test_argv03_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_sd4", "1"};
		char test_argv03_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_sd4"};
		char test_argv03_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_sd4", "0"};
		char test_argv03_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_sd4"};

		pt_dd_exs_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_05);

		// 1-1-79
		char test_argv04_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_nf"};
		char test_argv04_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_nf", "1"};
		char test_argv04_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_nf"};
		char test_argv04_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_nf", "0"};
		char test_argv04_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_nf"};

		pt_dd_exs_execute_cmd((sizeof(test_argv04_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_05);

		// 1-1-80
		char test_argv05_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_relc"};
		char test_argv05_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_relc", "1"};
		char test_argv05_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_relc"};
		char test_argv05_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_relc", "0"};
		char test_argv05_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_relc"};

		pt_dd_exs_execute_cmd((sizeof(test_argv05_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_05);

		// 1-1-81
		char test_argv06_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_netsec"};
		char test_argv06_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_netsec", "1"};
		char test_argv06_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_netsec"};
		char test_argv06_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_netsec", "0"};
		char test_argv06_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_netsec"};

		pt_dd_exs_execute_cmd((sizeof(test_argv06_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_05);

		// 1-1-82
		char test_argv07_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_usb2"};
		char test_argv07_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_usb2", "1"};
		char test_argv07_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_usb2"};
		char test_argv07_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_usb2", "0"};
		char test_argv07_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_usb2"};

		pt_dd_exs_execute_cmd((sizeof(test_argv07_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_05);

		// 1-1-83
		char test_argv08_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_usb3"};
		char test_argv08_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_usb3", "1"};
		char test_argv08_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_usb3"};
		char test_argv08_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_usb3", "0"};
		char test_argv08_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_usb3"};

		pt_dd_exs_execute_cmd((sizeof(test_argv08_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_05);

		// 1-1-84
		char test_argv09_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_pcie0"};
		char test_argv09_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_pcie0", "1"};
		char test_argv09_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_pcie0"};
		char test_argv09_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_pcie0", "0"};
		char test_argv09_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_pcie0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv09_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_05);

		// 1-1-85
		char test_argv10_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_pcie1"};
		char test_argv10_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_pcie1", "1"};
		char test_argv10_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_pcie1"};
		char test_argv10_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_pcie1", "0"};
		char test_argv10_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_pcie1"};

		pt_dd_exs_execute_cmd((sizeof(test_argv10_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_05);

		// 1-1-86
		char test_argv11_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_emmc"};
		char test_argv11_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_emmc", "1"};
		char test_argv11_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_emmc"};
		char test_argv11_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "ram_pd", "rampd_emmc", "0"};
		char test_argv11_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "ram_pd", "rampd_emmc"};

		pt_dd_exs_execute_cmd((sizeof(test_argv11_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_05);

	/******************************************************************/
	} else if ( testno == 10 ) {		// "BUSCKSTP"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "nf_ckstp", "1"};
		char test_argv00_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "pcie0_ckstp", "1"};
		char test_argv00_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "pcie1_ckstp", "1"};
		char test_argv00_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "xdmac_ckstp", "1"};
		char test_argv00_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "hdmac_ckstp", "1"};
		char test_argv00_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "emmc_ckstp", "1"};
		char test_argv00_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "sd2_ckstp", "1"};
		char test_argv00_08[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "sd3_ckstp", "1"};
		char test_argv00_09[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "sd4_ckstp", "1"};
		char test_argv00_10[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "netsec_ckstp", "1"};
		char test_argv00_11[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "relc_ckstp", "1"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_07);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_08) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_08);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_09) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_09);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_10) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_10);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_11) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_11);

		// 1-1-87
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "nf_ckstp"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "nf_ckstp", "1"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "nf_ckstp"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "nf_ckstp", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "nf_ckstp"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

		// 1-1-88
		char test_argv02_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "pcie0_ckstp"};
		char test_argv02_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "pcie0_ckstp", "1"};
		char test_argv02_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "pcie0_ckstp"};
		char test_argv02_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "pcie0_ckstp", "0"};
		char test_argv02_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "pcie0_ckstp"};

		pt_dd_exs_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_05);

		// 1-1-89
		char test_argv03_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "pcie1_ckstp"};
		char test_argv03_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "pcie1_ckstp", "1"};
		char test_argv03_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "pcie1_ckstp"};
		char test_argv03_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "pcie1_ckstp", "0"};
		char test_argv03_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "pcie1_ckstp"};

		pt_dd_exs_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_05);

		// 1-1-90
		char test_argv04_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "xdmac_ckstp"};
		char test_argv04_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "xdmac_ckstp", "1"};
		char test_argv04_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "xdmac_ckstp"};
		char test_argv04_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "xdmac_ckstp", "0"};
		char test_argv04_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "xdmac_ckstp"};

		pt_dd_exs_execute_cmd((sizeof(test_argv04_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_05);

		// 1-1-91
		char test_argv05_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "hdmac_ckstp"};
		char test_argv05_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "hdmac_ckstp", "1"};
		char test_argv05_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "hdmac_ckstp"};
		char test_argv05_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "hdmac_ckstp", "0"};
		char test_argv05_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "hdmac_ckstp"};

		pt_dd_exs_execute_cmd((sizeof(test_argv05_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv05_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv05_05);

		// 1-1-92
		char test_argv06_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "emmc_ckstp"};
		char test_argv06_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "emmc_ckstp", "1"};
		char test_argv06_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "emmc_ckstp"};
		char test_argv06_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "emmc_ckstp", "0"};
		char test_argv06_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "emmc_ckstp"};

		pt_dd_exs_execute_cmd((sizeof(test_argv06_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv06_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv06_05);

		// 1-1-93
		char test_argv07_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "sd2_ckstp"};
		char test_argv07_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "sd2_ckstp", "1"};
		char test_argv07_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "sd2_ckstp"};
		char test_argv07_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "sd2_ckstp", "0"};
		char test_argv07_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "sd2_ckstp"};

		pt_dd_exs_execute_cmd((sizeof(test_argv07_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv07_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv07_05);

		// 1-1-94
		char test_argv08_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "sd3_ckstp"};
		char test_argv08_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "sd3_ckstp", "1"};
		char test_argv08_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "sd3_ckstp"};
		char test_argv08_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "sd3_ckstp", "0"};
		char test_argv08_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "sd3_ckstp"};

		pt_dd_exs_execute_cmd((sizeof(test_argv08_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv08_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv08_05);

		// 1-1-95
		char test_argv09_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "sd4_ckstp"};
		char test_argv09_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "sd4_ckstp", "1"};
		char test_argv09_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "sd4_ckstp"};
		char test_argv09_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "sd4_ckstp", "0"};
		char test_argv09_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "sd4_ckstp"};

		pt_dd_exs_execute_cmd((sizeof(test_argv09_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv09_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv09_05);

		// 1-1-96
		char test_argv10_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "netsec_ckstp"};
		char test_argv10_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "netsec_ckstp", "1"};
		char test_argv10_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "netsec_ckstp"};
		char test_argv10_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "netsec_ckstp", "0"};
		char test_argv10_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "netsec_ckstp"};

		pt_dd_exs_execute_cmd((sizeof(test_argv10_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv10_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv10_05);

		// 1-1-97
		char test_argv11_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "relc_ckstp"};
		char test_argv11_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "relc_ckstp", "1"};
		char test_argv11_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "relc_ckstp"};
		char test_argv11_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "busckstp", "relc_ckstp", "0"};
		char test_argv11_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "busckstp", "relc_ckstp"};

		pt_dd_exs_execute_cmd((sizeof(test_argv11_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv11_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv11_05);

	/******************************************************************/
	} else if ( testno == 11 ) {		// "NSEC_CKSTP"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nsec_ckstp", "netsec_ptpstp", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);

		// 1-1-98
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nsec_ckstp", "netsec_ptpstp"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nsec_ckstp", "netsec_ptpstp", "1"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nsec_ckstp", "netsec_ptpstp"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nsec_ckstp", "netsec_ptpstp", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nsec_ckstp", "netsec_ptpstp"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

	/******************************************************************/
	} else if ( testno == 12 ) {		// "EMMC_CKSTP"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "emmc_ckstp", "emclkstp", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);

		// 1-1-99
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "emmc_ckstp", "emclkstp"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "emmc_ckstp", "emclkstp", "1"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "emmc_ckstp", "emclkstp"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "emmc_ckstp", "emclkstp", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "emmc_ckstp", "emclkstp"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

	/******************************************************************/
	} else if ( testno == 13 ) {		// "EM_DRVSTR"レジスタのテスト

		// 1-1-100
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_drvstr", "em_dsel"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);

	/******************************************************************/
	} else if ( testno == 14 ) {		// "EM_PV_DTIMEC"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "emcrst", "0"};
		char test_argv00_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_pv_dtimec", "pv_dtimec", "0x000000"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_02);

		// 1-1-101
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_pv_dtimec", "pv_dtimec"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_pv_dtimec", "pv_dtimec", "0xFFFFFF"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_pv_dtimec", "pv_dtimec"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_pv_dtimec", "pv_dtimec", "0xAAAAAA"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_pv_dtimec", "pv_dtimec"};
		char test_argv01_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_pv_dtimec", "pv_dtimec", "0x000000"};
		char test_argv01_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_pv_dtimec", "pv_dtimec"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_07);

	/******************************************************************/
	} else if ( testno == 15 ) {		// "EM_PV_AMPBL"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_pv_ampbl", "pv_ampbl", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);

		// 1-1-102
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_pv_ampbl", "pv_ampbl"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_pv_ampbl", "pv_ampbl", "15"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_pv_ampbl", "pv_ampbl"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_pv_ampbl", "pv_ampbl", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_pv_ampbl", "pv_ampbl"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

	/******************************************************************/
	} else if ( testno == 16 ) {		// "EM_CR_SLOTTYPE"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "emcrst", "0"};
		char test_argv00_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_cr_slottype", "cr_slottype", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_02);

		// 1-1-103
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_cr_slottype", "cr_slottype"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_cr_slottype", "cr_slottype", "1"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_cr_slottype", "cr_slottype"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_cr_slottype", "cr_slottype", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_cr_slottype", "cr_slottype"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

	/******************************************************************/
	} else if ( testno == 17 ) {		// "EM_CR_BCLKFREQ"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "emcrst", "0"};
		char test_argv00_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_cr_bclkfreq", "cr_bclkfreq", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_02);

		// 1-1-104
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_cr_bclkfreq", "cr_bclkfreq"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_cr_bclkfreq", "cr_bclkfreq", "1"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_cr_bclkfreq", "cr_bclkfreq"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_cr_bclkfreq", "cr_bclkfreq", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_cr_bclkfreq", "cr_bclkfreq"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

	/******************************************************************/
	} else if ( testno == 18 ) {		// "EM_CDDET"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "softreset", "emcrst", "0"};
		char test_argv00_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_cddet", "emcd", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_02);

		// 1-1-105
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_cddet", "emcd"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_cddet", "emcd", "1"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_cddet", "emcd"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "em_cddet", "emcd", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_cddet", "emcd"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

		// 1-1-106
		char test_argv02_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "em_cddet", "emphylk"};

		pt_dd_exs_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_01);

	/******************************************************************/
	} else if ( testno == 19 ) {		// "NF_IP_CTL"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_ip_ctl", "dscv_inhibit", "0"};
		char test_argv00_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_ip_ctl", "dscv_ignr_crc", "0"};
		char test_argv00_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_ip_ctl", "boot_en", "0"};
		char test_argv00_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_ip_ctl", "iddq_en", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_04);

		// 1-1-107
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_ip_ctl", "dscv_inhibit"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_ip_ctl", "dscv_inhibit", "1"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_ip_ctl", "dscv_inhibit"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_ip_ctl", "dscv_inhibit", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_ip_ctl", "dscv_inhibit"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

		// 1-1-108
		char test_argv02_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_ip_ctl", "dscv_ignr_crc"};
		char test_argv02_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_ip_ctl", "dscv_ignr_crc", "1"};
		char test_argv02_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_ip_ctl", "dscv_ignr_crc"};
		char test_argv02_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_ip_ctl", "dscv_ignr_crc", "0"};
		char test_argv02_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_ip_ctl", "dscv_ignr_crc"};

		pt_dd_exs_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_05);

		// 1-1-109
		char test_argv03_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_ip_ctl", "boot_en"};
		char test_argv03_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_ip_ctl", "boot_en", "1"};
		char test_argv03_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_ip_ctl", "boot_en"};
		char test_argv03_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_ip_ctl", "boot_en", "0"};
		char test_argv03_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_ip_ctl", "boot_en"};

		pt_dd_exs_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_05);

		// 1-1-110
		char test_argv04_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_ip_ctl", "iddq_en"};
		char test_argv04_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_ip_ctl", "iddq_en", "1"};
		char test_argv04_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_ip_ctl", "iddq_en"};
		char test_argv04_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_ip_ctl", "iddq_en", "0"};
		char test_argv04_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_ip_ctl", "iddq_en"};

		pt_dd_exs_execute_cmd((sizeof(test_argv04_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv04_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_05);

	/******************************************************************/
	} else if ( testno == 20 ) {		// "NF_BTSQ_STATE"レジスタのテスト

		// 1-1-111
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_btsq_state", "init_fail"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);

		// 1-1-112
		char test_argv02_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_btsq_state", "boot_comp"};

		pt_dd_exs_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_01);

		// 1-1-113
		char test_argv03_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_btsq_state", "boot_error"};

		pt_dd_exs_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_01);

		// 1-1-114
		char test_argv04_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_btsq_state", "boot_busy"};

		pt_dd_exs_execute_cmd((sizeof(test_argv04_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv04_01);

	/******************************************************************/
	} else if ( testno == 21 ) {		// "NF_RB_VALID_TM"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_rb_valid_tm", "rb_valid_time", "0x0000"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);

		// 1-1-115
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_rb_valid_tm", "rb_valid_time"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_rb_valid_tm", "rb_valid_time", "0xFFFF"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_rb_valid_tm", "rb_valid_time"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_rb_valid_tm", "rb_valid_time", "0xAAAA"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_rb_valid_tm", "rb_valid_time"};
		char test_argv01_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_rb_valid_tm", "rb_valid_time", "0x0000"};
		char test_argv01_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_rb_valid_tm", "rb_valid_time"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_07);

	/******************************************************************/
	} else if ( testno == 22 ) {		// "NF_PHY_CTRL_REG"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_ctrl_reg", "phy_ctrl", "0x00000000"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);

		// 1-1-116
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_ctrl_reg", "phy_ctrl"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_ctrl_reg", "phy_ctrl", "0xFFFFFFFF"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_ctrl_reg", "phy_ctrl"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_ctrl_reg", "phy_ctrl", "0xAAAAAAAA"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_ctrl_reg", "phy_ctrl"};
		char test_argv01_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_ctrl_reg", "phy_ctrl", "0x00000000"};
		char test_argv01_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_ctrl_reg", "phy_ctrl"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_07);

	/******************************************************************/
	} else if ( testno == 23 ) {		// "NF_PHY_DQ_TIM"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dq_tim", "phy_dq_timing", "0x00000000"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);

		// 1-1-117
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dq_tim", "phy_dq_timing"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dq_tim", "phy_dq_timing", "0xFFFFFFFF"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dq_tim", "phy_dq_timing"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dq_tim", "phy_dq_timing", "0xAAAAAAAA"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dq_tim", "phy_dq_timing"};
		char test_argv01_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dq_tim", "phy_dq_timing", "0x00000000"};
		char test_argv01_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dq_tim", "phy_dq_timing"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_07);

	/******************************************************************/
	} else if ( testno == 23 ) {		// "NF_PHY_DQS_TIM"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dqs_tim", "phy_dqs_timing", "0x00000000"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);

		// 1-1-118
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dqs_tim", "phy_dqs_timing"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dqs_tim", "phy_dqs_timing", "0xFFFFFFFF"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dqs_tim", "phy_dqs_timing"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dqs_tim", "phy_dqs_timing", "0xAAAAAAAA"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dqs_tim", "phy_dqs_timing"};
		char test_argv01_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dqs_tim", "phy_dqs_timing", "0x00000000"};
		char test_argv01_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dqs_tim", "phy_dqs_timing"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_07);

	/******************************************************************/
	} else if ( testno == 24 ) {		// "NF_PHY_GT_LPBK"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_gt_lpbk", "phy_gt_lpbk_ctl", "0x00000000"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);

		// 1-1-119
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_gt_lpbk", "phy_gt_lpbk_ctl"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_gt_lpbk", "phy_gt_lpbk_ctl", "0xFFFFFFFF"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_gt_lpbk", "phy_gt_lpbk_ctl"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_gt_lpbk", "phy_gt_lpbk_ctl", "0xAAAAAAAA"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_gt_lpbk", "phy_gt_lpbk_ctl"};
		char test_argv01_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_gt_lpbk", "phy_gt_lpbk_ctl", "0x00000000"};
		char test_argv01_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_gt_lpbk", "phy_gt_lpbk_ctl"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_07);

	/******************************************************************/
	} else if ( testno == 25 ) {		// "NF_PHY_DLL_MCTL"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dll_mctl", "phy_dll_mst_ctl", "0x00000000"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);

		// 1-1-120
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dll_mctl", "phy_dll_mst_ctl"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dll_mctl", "phy_dll_mst_ctl", "0xFFFFFFFF"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dll_mctl", "phy_dll_mst_ctl"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dll_mctl", "phy_dll_mst_ctl", "0xAAAAAAAA"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dll_mctl", "phy_dll_mst_ctl"};
		char test_argv01_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dll_mctl", "phy_dll_mst_ctl", "0x00000000"};
		char test_argv01_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dll_mctl", "phy_dll_mst_ctl"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_07);

	/******************************************************************/
	} else if ( testno == 26 ) {		// "NF_PHY_DLL_SCTL"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dll_sctl", "phy_dll_slv_ctl", "0x00000000"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);

		// 1-1-121
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dll_sctl", "phy_dll_slv_ctl"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dll_sctl", "phy_dll_slv_ctl", "0xFFFFFFFF"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dll_sctl", "phy_dll_slv_ctl"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dll_sctl", "phy_dll_slv_ctl", "0xAAAAAAAA"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dll_sctl", "phy_dll_slv_ctl"};
		char test_argv01_06[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_phy_dll_sctl", "phy_dll_slv_ctl", "0x00000000"};
		char test_argv01_07[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_phy_dll_sctl", "phy_dll_slv_ctl"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_06);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_07);

	/******************************************************************/
	} else if ( testno == 27 ) {		// "NF_BOOT_ECC_CTL"レジスタのテスト

		char test_argv00_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_boot_ecc_ctl", "boot_ecc_sec_siz", "0"};
		char test_argv00_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_boot_ecc_ctl", "bt_ecc_corr_str", "0"};
		char test_argv00_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_boot_ecc_ctl", "boot_ecc_en", "0"};

		pt_dd_exs_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv00_03);

		// 1-1-122
		char test_argv01_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_boot_ecc_ctl", "boot_ecc_sec_siz"};
		char test_argv01_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_boot_ecc_ctl", "boot_ecc_sec_siz", "1"};
		char test_argv01_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_boot_ecc_ctl", "boot_ecc_sec_siz"};
		char test_argv01_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_boot_ecc_ctl", "boot_ecc_sec_siz", "0"};
		char test_argv01_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_boot_ecc_ctl", "boot_ecc_sec_siz"};

		pt_dd_exs_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv01_05);

		// 1-1-123
		char test_argv02_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_boot_ecc_ctl", "bt_ecc_corr_str"};
		char test_argv02_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_boot_ecc_ctl", "bt_ecc_corr_str", "1"};
		char test_argv02_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_boot_ecc_ctl", "bt_ecc_corr_str"};
		char test_argv02_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_boot_ecc_ctl", "bt_ecc_corr_str", "0"};
		char test_argv02_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_boot_ecc_ctl", "bt_ecc_corr_str"};

		pt_dd_exs_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv02_05);

		// 1-1-124
		char test_argv03_01[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_boot_ecc_ctl", "boot_ecc_en"};
		char test_argv03_02[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_boot_ecc_ctl", "boot_ecc_en", "1"};
		char test_argv03_03[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_boot_ecc_ctl", "boot_ecc_en"};
		char test_argv03_04[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "set", "nf_boot_ecc_ctl", "boot_ecc_en", "0"};
		char test_argv03_05[][D_PT_DD_EXS_CMD_LEN]  = {"ddexstop", "get", "nf_boot_ecc_ctl", "boot_ecc_en"};

		pt_dd_exs_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_01);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_02);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_03);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_04);
		pt_dd_exs_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_EXS_CMD_LEN), (char*)test_argv03_05);

	/******************************************************************/
	} else {
		// 例外パラメータ指定
		Ddim_Print(( "Parameters of the excluded\n" ));
	}
	// タイマーログ出力(処理時間出力)
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );

	Ddim_Print(( "EXS Palladium test No %d End\n", testno ));

	// Force stop
	Palladium_Force_Stop();
}
