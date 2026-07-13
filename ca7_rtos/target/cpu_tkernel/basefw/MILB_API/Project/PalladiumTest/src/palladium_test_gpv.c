/**
 * @file		palladium_test_gpv.c
 * @brief		palladium test code for gpv.
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

#include "ct_dd_gpv.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PT_DD_GPV_CMD_LEN			(20)

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
static char* g_dd_gpv_Cmd_Argv[20]; 

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

static void pt_dd_gpv_execute_cmd(int cmd_argc, char* cmd_argv) 
{ 
	INT32 j; 
	INT32 cmd_pos = 0; 

	for (j = 0; j < cmd_argc; j++) { 
		g_dd_gpv_Cmd_Argv[j] = (cmd_argv + (cmd_pos * D_PT_DD_GPV_CMD_LEN)); 
		cmd_pos++; 
	}

	Ct_Dd_GPV_Main(cmd_argc, g_dd_gpv_Cmd_Argv); 
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
VOID Pt_Dd_GPV_Main( VOID )
{
	UCHAR	testno;
	
	// 開始ログ
	Ddim_Print(( "GPV Palladium test Start\n" ));

	// タイマー初期化(処理時間計測用)
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得(RSファイルに紐付く)
	testno	= gDDIM_Info.com._6a;

	Ddim_Print(( "GPV Palladium test No %d Start\n", testno ));

	// レジスタ取得＆設定(#define PC_DEBUG を定義すること)
	/******************************************************************/
	if ( testno == 1 ) {	// "M_Usb3_r_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fm2", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fml", "0"};
		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fm", "0"};

		char test_argv00_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_usb3_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_05);

		// 1-1-1 , 1-1-2 , 1-1-3 , 1-1-4
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_usb3_r_ib"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_usb3_r_ib"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fml", "3"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_usb3_r_ib"};
		char test_argv01_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fm", "3"};
		char test_argv01_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_usb3_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_08);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_usb3_r_ib"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_usb3_r_ib"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fml", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_usb3_r_ib"};
		char test_argv02_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fm", "0"};
		char test_argv02_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_usb3_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_08);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fmib", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_usb3_r_ib"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fm2", "2"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_usb3_r_ib"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fml", "4"};		// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_usb3_r_ib"};
		char test_argv03_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_usb3_r_ib", "fm", "4"};		// err confirm
		char test_argv03_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_usb3_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_08);

	/******************************************************************/
	} else if ( testno == 2 ) {		// "M_Emmc_r_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fm2", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fml", "0"};
		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fm", "0"};

		char test_argv00_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_emmc_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_05);

		// 1-1-5 , 1-1-5 , 1-1-7 , 1-1-8
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_emmc_r_ib"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_emmc_r_ib"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fml", "3"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_emmc_r_ib"};
		char test_argv01_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fm", "3"};
		char test_argv01_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_emmc_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_08);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_emmc_r_ib"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_emmc_r_ib"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fml", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_emmc_r_ib"};
		char test_argv02_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fm", "0"};
		char test_argv02_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_emmc_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_08);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fmib", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_emmc_r_ib"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fm2", "2"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_emmc_r_ib"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fml", "4"};		// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_emmc_r_ib"};
		char test_argv03_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_emmc_r_ib", "fm", "4"};		// err confirm
		char test_argv03_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_emmc_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_08);

	/******************************************************************/
	} else if ( testno == 3 ) {		// "M_Uhs2_r_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fm2", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fml", "0"};
		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fm", "0"};

		char test_argv00_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_uhs2_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_05);

		// 1-1-9 , 1-1-10 , 1-1-11 , 1-1-12
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_uhs2_r_ib"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_uhs2_r_ib"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fml", "3"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_uhs2_r_ib"};
		char test_argv01_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fm", "3"};
		char test_argv01_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_uhs2_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_08);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_uhs2_r_ib"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_uhs2_r_ib"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fml", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_uhs2_r_ib"};
		char test_argv02_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fm", "0"};
		char test_argv02_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_uhs2_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_08);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fmib", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_uhs2_r_ib"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fm2", "2"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_uhs2_r_ib"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fml", "4"};		// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_uhs2_r_ib"};
		char test_argv03_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_uhs2_r_ib", "fm", "4"};		// err confirm
		char test_argv03_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_uhs2_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_08);

	/******************************************************************/
	} else if ( testno == 4 ) {		// "M_Mxic1_m_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic1_m_ib", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic1_m_ib", "wt", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic1_m_ib", "fm", "0"};

		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic1_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);

		// 1-1-13 , 1-1-14 , 1-1-15
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic1_m_ib", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic1_m_ib"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic1_m_ib", "wt", "8"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic1_m_ib"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic1_m_ib", "fm", "3"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic1_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic1_m_ib", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic1_m_ib"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic1_m_ib", "wt", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic1_m_ib"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic1_m_ib", "fm", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic1_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic1_m_ib", "fmib", "4"};	// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic1_m_ib"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic1_m_ib", "wt", "9"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic1_m_ib"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic1_m_ib", "fm", "4"};		// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic1_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);

	/******************************************************************/
	} else if ( testno == 5 ) {		// "M_Mxic2_m_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic2_m_ib", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic2_m_ib", "wt", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic2_m_ib", "fm", "0"};

		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic2_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);

		// 1-1-16 , 1-1-17 , 1-1-18
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic2_m_ib", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic2_m_ib"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic2_m_ib", "wt", "8"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic2_m_ib"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic2_m_ib", "fm", "3"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic2_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic2_m_ib", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic2_m_ib"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic2_m_ib", "wt", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic2_m_ib"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic2_m_ib", "fm", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic2_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic2_m_ib", "fmib", "4"};	// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic2_m_ib"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic2_m_ib", "wt", "9"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic2_m_ib"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_mxic2_m_ib", "fm", "4"};		// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_mxic2_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);

	/******************************************************************/
	} else if ( testno == 6 ) {		// "M_Xdmac_r_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_xdmac_r_ib", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_xdmac_r_ib", "fm2", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_xdmac_r_ib", "fm", "0"};

		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_xdmac_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);

		// 1-1-19 , 1-1-20 , 1-1-21
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_xdmac_r_ib", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_xdmac_r_ib"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_xdmac_r_ib", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_xdmac_r_ib"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_xdmac_r_ib", "fm", "3"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_xdmac_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_xdmac_r_ib", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_xdmac_r_ib"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_xdmac_r_ib", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_xdmac_r_ib"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_xdmac_r_ib", "fm", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_xdmac_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_xdmac_r_ib", "fmib", "4"};	// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_xdmac_r_ib"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_xdmac_r_ib", "fm2", "2"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_xdmac_r_ib"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_xdmac_r_ib", "fm", "4"};		// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_xdmac_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);

	/******************************************************************/
	} else if ( testno == 7 ) {		// "M_Nf_r_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fm2", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fml", "0"};
		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fm", "0"};

		char test_argv00_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nf_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_05);

		// 1-1-22 , 1-1-23 , 1-1-24 , 1-1-25
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nf_r_ib"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nf_r_ib"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fml", "3"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nf_r_ib"};
		char test_argv01_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fm", "3"};
		char test_argv01_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nf_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_08);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nf_r_ib"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nf_r_ib"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fml", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nf_r_ib"};
		char test_argv02_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fm", "0"};
		char test_argv02_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nf_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_08);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fmib", "4"};	// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nf_r_ib"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fm2", "2"};	// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nf_r_ib"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fml", "4"};	// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nf_r_ib"};
		char test_argv03_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nf_r_ib", "fm", "4"};		// err confirm
		char test_argv03_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nf_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_08);

	/******************************************************************/
	} else if ( testno == 8 ) {		// "M_Pcie0_r_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fm2", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fml", "0"};
		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fm", "0"};

		char test_argv00_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie0_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_05);

		// 1-1-26 , 1-1-27 , 1-1-28 , 1-1-29
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie0_r_ib"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie0_r_ib"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fml", "3"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie0_r_ib"};
		char test_argv01_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fm", "3"};
		char test_argv01_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie0_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_08);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie0_r_ib"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie0_r_ib"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fml", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie0_r_ib"};
		char test_argv02_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fm", "0"};
		char test_argv02_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie0_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_08);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fmib", "4"};	// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie0_r_ib"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fm2", "2"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie0_r_ib"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fml", "4"};		// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie0_r_ib"};
		char test_argv03_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie0_r_ib", "fm", "4"};		// err confirm
		char test_argv03_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie0_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_08);

	/******************************************************************/
	} else if ( testno == 9 ) {		// "M_Pcie1_r_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fm2", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fml", "0"};
		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fm", "0"};

		char test_argv00_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie1_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_05);

		// 1-1-30 , 1-1-31 , 1-1-32 , 1-1-33
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie1_r_ib"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie1_r_ib"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fml", "3"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie1_r_ib"};
		char test_argv01_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fm", "3"};
		char test_argv01_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie1_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_08);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie1_r_ib"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie1_r_ib"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fml", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie1_r_ib"};
		char test_argv02_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fm", "0"};
		char test_argv02_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie1_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_08);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fmib", "4"};	// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie1_r_ib"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fm2", "2"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie1_r_ib"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fml", "4"};		// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie1_r_ib"};
		char test_argv03_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_pcie1_r_ib", "fm", "4"};		// err confirm
		char test_argv03_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_pcie1_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_08);

	/******************************************************************/
	} else if ( testno == 10 ) {		// "M_NicPeriAHB_m_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicperiahb_m", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicperiahb_m", "fm2", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicperiahb_m", "ac", "0"};

		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicperiahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);

		// 1-1-34 , 1-1-35 , 1-1-36
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicperiahb_m", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicperiahb_m"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicperiahb_m", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicperiahb_m"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicperiahb_m", "ac", "3"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicperiahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicperiahb_m", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicperiahb_m"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicperiahb_m", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicperiahb_m"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicperiahb_m", "ac", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicperiahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicperiahb_m", "fmib", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicperiahb_m"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicperiahb_m", "fm2", "2"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicperiahb_m"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicperiahb_m", "ac", "4"};		// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicperiahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);

	/******************************************************************/
	} else if ( testno == 11 ) {		// "M_NicBmAHB1_r"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb1_r", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb1_r", "ac", "0"};

		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb1_r"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);

		// 1-1-37 , 1-1-38
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb1_r", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb1_r"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb1_r", "ac", "3"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb1_r"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb1_r", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb1_r"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb1_r", "ac", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb1_r"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb1_r", "fmib", "4"};	// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb1_r"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb1_r", "ac", "4"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb1_r"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);

	/******************************************************************/
	} else if ( testno == 12 ) {		// "NsecX2P_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "nsecx2p_ib", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "nsecx2p_ib", "fm2", "0"};

		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "nsecx2p_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);

		// 1-1-39 , 1-1-40
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "nsecx2p_ib", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "nsecx2p_ib"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "nsecx2p_ib", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "nsecx2p_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "nsecx2p_ib", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "nsecx2p_ib"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "nsecx2p_ib", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "nsecx2p_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "nsecx2p_ib", "fmib", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "nsecx2p_ib"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "nsecx2p_ib", "fm2", "2"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "nsecx2p_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);

	/******************************************************************/
	} else if ( testno == 13 ) {		// "ToprX2P_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "toprx2p_ib", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "toprx2p_ib", "fm2", "0"};

		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "toprx2p_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);

		// 1-1-41 , 1-1-42
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "toprx2p_ib", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "toprx2p_ib"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "toprx2p_ib", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "toprx2p_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "toprx2p_ib", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "toprx2p_ib"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "toprx2p_ib", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "toprx2p_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "toprx2p_ib", "fmib", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "toprx2p_ib"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "toprx2p_ib", "fm2", "2"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "toprx2p_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);

	/******************************************************************/
	} else if ( testno == 14 ) {		// "M_NicBmAHB2_r_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb2_r_ib", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb2_r_ib", "fm2", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb2_r_ib", "ac", "0"};

		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb2_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);

		// 1-1-43 , 1-1-44 , 1-1-45
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb2_r_ib", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb2_r_ib"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb2_r_ib", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb2_r_ib"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb2_r_ib", "ac", "3"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb2_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb2_r_ib", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb2_r_ib"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb2_r_ib", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb2_r_ib"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb2_r_ib", "ac", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb2_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb2_r_ib", "fmib", "4"};	// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb2_r_ib"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb2_r_ib", "fm2", "2"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb2_r_ib"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "m_nicbmahb2_r_ib", "ac", "4"};		// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "m_nicbmahb2_r_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);

	/******************************************************************/
	} else if ( testno == 15 ) {		// "S_Main_rm_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_main_rm_ib", "fm", "0"};

		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_main_rm_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		// 1-1-46
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_main_rm_ib", "fm", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_main_rm_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_main_rm_ib", "fm", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_main_rm_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_main_rm_ib", "fm", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_main_rm_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);

	/******************************************************************/
	} else if ( testno == 16 ) {		// "S_Xdmac_m_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_xdmac_m_ib", "fm", "0"};

		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_xdmac_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		// 1-1-47
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_xdmac_m_ib", "fm", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_xdmac_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_xdmac_m_ib", "fm", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_xdmac_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_xdmac_m_ib", "fm", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_xdmac_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);

	/******************************************************************/
	} else if ( testno == 17 ) {		// "S_Netsec_m"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_netsec_m", "fm", "0"};

		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_netsec_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		// 1-1-48
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_netsec_m", "fm", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_netsec_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_netsec_m", "fm", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_netsec_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_netsec_m", "fm", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_netsec_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);


	/******************************************************************/
	} else if ( testno == 18 ) {		// "S_WifiAHB_m_ib"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_wifiahb_m_ib", "fma", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_wifiahb_m_ib", "fm", "0"};

		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_wifiahb_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);

		// 1-1-49 , 1-1-50
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_wifiahb_m_ib", "fma", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_wifiahb_m_ib"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_wifiahb_m_ib", "fm", "3"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_wifiahb_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_wifiahb_m_ib", "fma", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_wifiahb_m_ib"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_wifiahb_m_ib", "fm", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_wifiahb_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_wifiahb_m_ib", "fma", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_wifiahb_m_ib"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_wifiahb_m_ib", "fm", "4"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_wifiahb_m_ib"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);

	/******************************************************************/
	} else if ( testno == 19 ) {		// "S_Uhs2AHB_m"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs2ahb_m", "fma", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs2ahb_m", "fm", "0"};

		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_uhs2ahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);

		// 1-1-51 , 1-1-52
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs2ahb_m", "fma", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_uhs2ahb_m"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs2ahb_m", "fm", "3"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_uhs2ahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs2ahb_m", "fma", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_uhs2ahb_m"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs2ahb_m", "fm", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_uhs2ahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs2ahb_m", "fma", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_uhs2ahb_m"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs2ahb_m", "fm", "4"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_uhs2ahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);

	/******************************************************************/
	} else if ( testno == 20 ) {		// "S_RelcrAHB_m"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcrahb_m", "fma", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcrahb_m", "fm", "0"};

		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_relcrahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);

		// 1-1-53 , 1-1-54
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcrahb_m", "fma", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_relcrahb_m"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcrahb_m", "fm", "3"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_relcrahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcrahb_m", "fma", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_relcrahb_m"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcrahb_m", "fm", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_relcrahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcrahb_m", "fma", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_relcrahb_m"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcrahb_m", "fm", "4"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_relcrahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);

	/******************************************************************/
	} else if ( testno == 21 ) {		// "S_Uhs1AHB_m"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs1ahb_m", "fma", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs1ahb_m", "fm", "0"};

		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_uhs1ahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);

		// 1-1-55 , 1-1-56
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs1ahb_m", "fma", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_uhs1ahb_m"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs1ahb_m", "fm", "3"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_uhs1ahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs1ahb_m", "fma", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_uhs1ahb_m"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs1ahb_m", "fm", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_uhs1ahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs1ahb_m", "fma", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_uhs1ahb_m"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_uhs1ahb_m", "fm", "4"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_uhs1ahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);

	/******************************************************************/
	} else if ( testno == 22 ) {		// "S_RelcwAHB_m"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcwahb_m", "fma", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcwahb_m", "fm", "0"};

		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_relcwahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);

		// 1-1-57 , 1-1-58
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcwahb_m", "fma", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_relcwahb_m"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcwahb_m", "fm", "3"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_relcwahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcwahb_m", "fma", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_relcwahb_m"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcwahb_m", "fm", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_relcwahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcwahb_m", "fma", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_relcwahb_m"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_relcwahb_m", "fm", "4"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_relcwahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);

	/******************************************************************/
	} else if ( testno == 23 ) {		// "S_BmNicAHB_m"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_bmnicahb_m", "fma", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_bmnicahb_m", "fm", "0"};

		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_bmnicahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);

		// 1-1-59 , 1-1-60
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_bmnicahb_m", "fma", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_bmnicahb_m"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_bmnicahb_m", "fm", "3"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_bmnicahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_bmnicahb_m", "fma", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_bmnicahb_m"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_bmnicahb_m", "fm", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_bmnicahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_bmnicahb_m", "fma", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_bmnicahb_m"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_bmnicahb_m", "fm", "4"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_bmnicahb_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);

	/******************************************************************/
	} else if ( testno == 24 ) {		// "S_Usb2AHB_ehci"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ehci", "fma", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ehci", "fm", "0"};

		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_usb2ahb_ehci"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);

		// 1-1-61 , 1-1-62
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ehci", "fma", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_usb2ahb_ehci"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ehci", "fm", "3"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_usb2ahb_ehci"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ehci", "fma", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_usb2ahb_ehci"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ehci", "fm", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_usb2ahb_ehci"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ehci", "fma", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_usb2ahb_ehci"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ehci", "fm", "4"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_usb2ahb_ehci"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);

	/******************************************************************/
	} else if ( testno == 25 ) {		// "S_Usb2AHB_ohci"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ohci", "fma", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ohci", "fm", "0"};

		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_usb2ahb_ohci"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);

		// 1-1-63 , 1-1-64
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ohci", "fma", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_usb2ahb_ohci"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ohci", "fm", "3"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_usb2ahb_ohci"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ohci", "fma", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_usb2ahb_ohci"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ohci", "fm", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_usb2ahb_ohci"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ohci", "fma", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_usb2ahb_ohci"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_usb2ahb_ohci", "fm", "4"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_usb2ahb_ohci"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);

	/******************************************************************/
	} else if ( testno == 26 ) {		// "S_Netsec2_m"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_netsec2_m", "fm", "0"};

		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_netsec2_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		// 1-1-65
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_netsec2_m", "fm", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_netsec2_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_netsec2_m", "fm", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_netsec2_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "s_netsec2_m", "fm", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "s_netsec2_m"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);

	/******************************************************************/
	} else if ( testno == 27 ) {		// "ib8"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "fm2", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "wt", "0"};
		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "fm", "0"};

		char test_argv00_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib8"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_05);

		// 1-1-66 , 1-1-67 , 1-1-68 , 1-1-69
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib8"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib8"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "wt", "8"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib8"};
		char test_argv01_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "fm", "3"};
		char test_argv01_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib8"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_08);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib8"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib8"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "wt", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib8"};
		char test_argv02_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "fm", "0"};
		char test_argv02_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib8"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_08);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "fmib", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib8"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "fm2", "2"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib8"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "wt", "9"};		// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib8"};
		char test_argv03_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib8", "fm", "4"};		// err confirm
		char test_argv03_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib8"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_08);

	/******************************************************************/
	} else if ( testno == 28 ) {		// "ib13"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fm2", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fml", "0"};
		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fm", "0"};

		char test_argv00_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib13"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_05);

		// 1-1-70 , 1-1-71 , 1-1-72 , 1-1-73
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib13"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib13"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fml", "3"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib13"};
		char test_argv01_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fm", "3"};
		char test_argv01_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib13"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_08);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib13"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib13"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fml", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib13"};
		char test_argv02_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fm", "0"};
		char test_argv02_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib13"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_08);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fmib", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib13"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fm2", "2"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib13"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fml", "4"};		// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib13"};
		char test_argv03_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib13", "fm", "4"};		// err confirm
		char test_argv03_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib13"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_08);

	/******************************************************************/
	} else if ( testno == 29 ) {		// "ib12"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib12", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib12", "fm2", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib12", "fm", "0"};

		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib12"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);

		// 1-1-74 , 1-1-75 , 1-1-76
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib12", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib12"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib12", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib12"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib12", "fm", "3"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib12"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib12", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib12"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib12", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib12"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib12", "fm", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib12"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib12", "fmib", "4"};	// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib12"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib12", "fm2", "2"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib12"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib12", "fm", "4"};		// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib12"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);

	/******************************************************************/
	} else if ( testno == 30 ) {		// "ib9"レジスタのテスト

		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "fmib", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "fm2", "0"};
		char test_argv00_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "wt", "0"};
		char test_argv00_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "fm", "0"};

		char test_argv00_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib9"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_05);

		// 1-1-77 , 1-1-78 , 1-1-79 , 1-1-80
		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "fmib", "3"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib9"};
		char test_argv01_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "fm2", "1"};
		char test_argv01_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib9"};
		char test_argv01_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "wt", "8"};
		char test_argv01_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib9"};
		char test_argv01_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "fm", "3"};
		char test_argv01_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib9"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_08);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "fmib", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib9"};
		char test_argv02_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "fm2", "0"};
		char test_argv02_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib9"};
		char test_argv02_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "wt", "0"};
		char test_argv02_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib9"};
		char test_argv02_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "fm", "0"};
		char test_argv02_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib9"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_08);

		char test_argv03_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "fmib", "4"};		// err confirm
		char test_argv03_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib9"};
		char test_argv03_03[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "fm2", "2"};		// err confirm
		char test_argv03_04[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib9"};
		char test_argv03_05[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "wt", "9"};		// err confirm
		char test_argv03_06[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib9"};
		char test_argv03_07[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "set", "ib9", "fm", "4"};		// err confirm
		char test_argv03_08[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "get", "ib9"};

		pt_dd_gpv_execute_cmd((sizeof(test_argv03_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_02);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_03) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_03);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_04) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_04);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_05) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_05);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_06) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_06);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_07) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_07);
		pt_dd_gpv_execute_cmd((sizeof(test_argv03_08) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv03_08);

	/******************************************************************/
	} else if ( testno == 51 ) {		// "M_Usb3_r_ib"レジスタの一括設定・取得テスト

		// 1-2-1
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_usb3_r_ib", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_usb3_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_usb3_r_ib", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_usb3_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_usb3_r_ib", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_usb3_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 52 ) {		// "M_Emmc_r_ib"レジスタの一括設定・取得テスト

		// 1-2-2
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_emmc_r_ib", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_emmc_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_emmc_r_ib", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_emmc_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_emmc_r_ib", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_emmc_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 53 ) {		// "M_Uhs2_r_ib"レジスタの一括設定・取得テスト

		// 1-2-3
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_uhs2_r_ib", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_uhs2_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_uhs2_r_ib", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_uhs2_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_uhs2_r_ib", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_uhs2_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 54 ) {		// "M_Mxic1_m_ib"レジスタの一括設定・取得テスト

		// 1-2-4
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_mxic1_m_ib", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_mxic1_m_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_mxic1_m_ib", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_mxic1_m_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_mxic1_m_ib", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_mxic1_m_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 55 ) {		// "M_Mxic2_m_ib"レジスタの一括設定・取得テスト

		// 1-2-5
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_mxic2_m_ib", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_mxic2_m_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_mxic2_m_ib", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_mxic2_m_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_mxic2_m_ib", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_mxic2_m_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 56 ) {		// "M_Xdmac_r_ib"レジスタの一括設定・取得テスト

		// 1-2-6
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_xdmac_r_ib", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_xdmac_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_xdmac_r_ib", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_xdmac_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_xdmac_r_ib", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_xdmac_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 57 ) {		// "M_Nf_r_ib"レジスタの一括設定・取得テスト

		// 1-2-7
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nf_r_ib", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nf_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nf_r_ib", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nf_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nf_r_ib", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nf_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 58 ) {		// "M_Pcie0_r_ib"レジスタの一括設定・取得テスト

		// 1-2-8
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_pcie0_r_ib", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_pcie0_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_pcie0_r_ib", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_pcie0_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_pcie0_r_ib", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_pcie0_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 59 ) {		// "M_Pcie1_r_ib"レジスタの一括設定・取得テスト

		// 1-2-9
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_pcie1_r_ib", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_pcie1_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_pcie1_r_ib", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_pcie1_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_pcie1_r_ib", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_pcie1_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 60 ) {		// "M_NicPeriAHB_m_ib"レジスタの一括設定・取得テスト

		// 1-2-10
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicperiahb_m", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicperiahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicperiahb_m", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicperiahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicperiahb_m", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicperiahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 61 ) {		// "M_NicBmAHB1_r_ib"レジスタの一括設定・取得テスト

		// 1-2-11
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicbmahb1_r", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicbmahb1_r", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicbmahb1_r", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicbmahb1_r", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicbmahb1_r", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicbmahb1_r", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 62 ) {		// "NsecX2P_ib"レジスタの一括設定・取得テスト

		// 1-2-12
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "nsecx2p_ib", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "nsecx2p_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "nsecx2p_ib", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "nsecx2p_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "nsecx2p_ib", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "nsecx2p_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 63 ) {		// "ToprX2P_ib"レジスタの一括設定・取得テスト

		// 1-2-13
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "toprx2p_ib", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "toprx2p_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "toprx2p_ib", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "toprx2p_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "toprx2p_ib", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "toprx2p_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 64 ) {		// "M_NicBmAHB2_r_ib"レジスタの一括設定・取得テスト

		// 1-2-14
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicbmahb2_r_ib", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicbmahb2_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicbmahb2_r_ib", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicbmahb2_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicbmahb2_r_ib", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "m_nicbmahb2_r_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 65 ) {		// "S_WifiAHB_m_ib"レジスタの一括設定・取得テスト

		// 1-2-15
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_wifiahb_m_ib", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_wifiahb_m_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_wifiahb_m_ib", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_wifiahb_m_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_wifiahb_m_ib", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_wifiahb_m_ib", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 66 ) {		// "S_Uhs2AHB_m"レジスタの一括設定・取得テスト

		// 1-2-16
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_uhs2ahb_m", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_uhs2ahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_uhs2ahb_m", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_uhs2ahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_uhs2ahb_m", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_uhs2ahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 67 ) {		// "S_RelcrAHB_m"レジスタの一括設定・取得テスト

		// 1-2-17
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_relcrahb_m", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_relcrahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_relcrahb_m", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_relcrahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_relcrahb_m", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_relcrahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 68 ) {		// "S_Uhs1AHB_m"レジスタの一括設定・取得テスト

		// 1-2-18
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_uhs1ahb_m", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_uhs1ahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_uhs1ahb_m", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_uhs1ahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_uhs1ahb_m", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_uhs1ahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 69 ) {		// "S_RelcwAHB_m"レジスタの一括設定・取得テスト

		// 1-2-19
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_relcwahb_m", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_relcwahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_relcwahb_m", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_relcwahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_relcwahb_m", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_relcwahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 70 ) {		// "S_BmNicAHB_m"レジスタの一括設定・取得テスト

		// 1-2-20
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_bmnicahb_m", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_bmnicahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_bmnicahb_m", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_bmnicahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_bmnicahb_m", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_bmnicahb_m", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 71 ) {		// "S_Usb2AHB_ehci"レジスタの一括設定・取得テスト

		// 1-2-21
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_usb2ahb_ehci", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_usb2ahb_ehci", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_usb2ahb_ehci", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_usb2ahb_ehci", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_usb2ahb_ehci", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_usb2ahb_ehci", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 72 ) {		// "S_Usb2AHB_ohci"レジスタの一括設定・取得テスト

		// 1-2-22
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_usb2ahb_ohci", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_usb2ahb_ohci", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_usb2ahb_ohci", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_usb2ahb_ohci", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_usb2ahb_ohci", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "s_usb2ahb_ohci", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 73 ) {		// "IB8"レジスタの一括設定・取得テスト

		// 1-2-23
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib8", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib8", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib8", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib8", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib8", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib8", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 74 ) {		// "IB13"レジスタの一括設定・取得テスト

		// 1-2-24
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib13", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib13", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib13", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib13", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib13", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib13", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 75 ) {		// "IB12"レジスタの一括設定・取得テスト

		// 1-2-25
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib12", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib12", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib12", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib12", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib12", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib12", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else if ( testno == 76 ) {		// "IB9"レジスタの一括設定・取得テスト

		// 1-2-26
		char test_argv00_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib9", "set", "0"};
		char test_argv00_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib9", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv00_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv00_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv00_02);

		char test_argv01_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib9", "set", "1"};
		char test_argv01_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib9", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv01_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv01_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv01_02);

		char test_argv02_01[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib9", "set", "0"};
		char test_argv02_02[][D_PT_DD_GPV_CMD_LEN]  = {"ddgpv", "all", "ib9", "get",};

		pt_dd_gpv_execute_cmd((sizeof(test_argv02_01) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_01);
		pt_dd_gpv_execute_cmd((sizeof(test_argv02_02) / D_PT_DD_GPV_CMD_LEN), (char*)test_argv02_02);

	/******************************************************************/
	} else {
		// 例外パラメータ指定
		Ddim_Print(( "Parameters of the excluded\n" ));
	}
	// タイマーログ出力(処理時間出力)
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );

	Ddim_Print(( "GPV Palladium test No %d End\n", testno ));

	// Force stop
	Palladium_Force_Stop();
}
