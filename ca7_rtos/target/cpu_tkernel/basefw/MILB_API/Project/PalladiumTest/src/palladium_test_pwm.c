/**
 * @file		palladium_test_pwm.c
 * @brief		palladium test code for pwm.
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

#include "ct_dd_pwm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PT_DD_PWM_CMD_LEN			(20)

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
static char* g_dd_pwm_Cmd_Argv[20]; 

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

static void pt_dd_pwm_execute_cmd(int cmd_argc, char* cmd_argv) 
{ 
	INT32 j; 
	INT32 cmd_pos = 0; 

	for (j = 0; j < cmd_argc; j++) { 
		g_dd_pwm_Cmd_Argv[j] = (cmd_argv + (cmd_pos * D_PT_DD_PWM_CMD_LEN)); 
		cmd_pos++; 
	}

	Ct_Dd_PWM_Main(cmd_argc, g_dd_pwm_Cmd_Argv); 
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
VOID Pt_Dd_PWM_Main( VOID )
{
	UCHAR	testno;

	// 開始ログ
	Ddim_Print(( "PWM Palladium test Start\n" ));

	// タイマー初期化(処理時間計測用)
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得(RSファイルに紐付く)
	testno	= gDDIM_Info.com._6a;

	Ddim_Print(( "PWM Palladium test No %d Start\n", testno ));

	// レジスタ取得＆設定(#define PC_DEBUG を定義すること)
	/******************************************************************/
	if ( testno == 1 ) {	// 1-1-1

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test", "0", "0"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start", "0"};
		char test_argv_05[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "stop", "0"};
		char test_argv_06[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "close", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_05) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_05);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_06) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_06);

	/******************************************************************/
	} else if ( testno == 2 ) {		// 1-1-2

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test", "0", "1"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start", "0"};
		char test_argv_05[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "stop", "0"};
		char test_argv_06[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "close", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_05) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_05);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_06) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_06);

	/******************************************************************/
	} else if ( testno == 3 ) {		// 1-1-3

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddtmr32", "stop", "1"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddtmr32", "close", "1"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_05[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test", "0", "2"};
		char test_argv_06[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start", "0"};
		char test_argv_07[][D_PT_DD_PWM_CMD_LEN]  = {"ddtmr32", "open", "1"};
		char test_argv_08[][D_PT_DD_PWM_CMD_LEN]  = {"ddtmr32", "ctrl", "1", "2", "reload", "2700000", "2700000", "1"};
		char test_argv_09[][D_PT_DD_PWM_CMD_LEN]  = {"ddtmr32", "start", "1"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_05) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_05);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_06) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_06);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_07) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_07);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_08) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_08);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_09) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_09);

	/******************************************************************/
	} else if ( testno == 4 ) {		// 1-1-4

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "stop", "0"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "close", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);

	/******************************************************************/
	} else if ( testno == 5 ) {		// 1-1-5

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test", "1"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "chg_wave", "0", "r", "60000", "40"};
		char test_argv_05[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start", "0"};
		char test_argv_06[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "stop", "0"};
		char test_argv_07[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "close", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_05) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_05);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_06) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_06);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_07) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_07);

	/******************************************************************/
	} else if ( testno == 6 ) {		// 1-1-6

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddtmr32", "stop", "1"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddtmr32", "close", "1"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_05[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test", "0", "2"};
		char test_argv_06[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "chg_wave", "0", "r", "60000", "40"};
		char test_argv_07[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "chg_wave", "0", "q", "60000", "25"};
		char test_argv_08[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "w_fact", "0", "2"};
		char test_argv_09[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start", "0"};
		char test_argv_10[][D_PT_DD_PWM_CMD_LEN]  = {"ddtmr32", "open", "1", "0"};
		char test_argv_11[][D_PT_DD_PWM_CMD_LEN]  = {"ddtmr32", "ctrl", "1", "2", "reload", "2700000", "2700000", "1"};
		char test_argv_12[][D_PT_DD_PWM_CMD_LEN]  = {"ddtmr32", "start", "1"};
		char test_argv_13[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "stop", "0"};
		char test_argv_14[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "close", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_05) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_05);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_06) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_06);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_07) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_07);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_08) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_08);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_09) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_09);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_10) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_10);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_11) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_11);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_12) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_12);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_13) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_13);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_14) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_14);

	/******************************************************************/
	} else if ( testno == 7 ) {		// 1-1-7

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test", "0", "1"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start", "0"};
		char test_argv_05[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "chg_wave", "0", "2"};
		char test_argv_06[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "chg_wave", "1", "n", "60000", "40"};
		char test_argv_07[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "stop", "0"};
		char test_argv_08[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "close", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_05) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_05);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_06) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_06);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_07) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_07);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_08) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_08);

	/******************************************************************/
	} else if ( testno == 8 ) {		// 1-1-8

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open_e", "8"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test_e", "8", "0", "1"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start_e", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 9 ) {		// 1-1-9

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open_e", "8"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test_e", "8", "0", "2"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start_e", "8"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 10 ) {		// 1-1-10

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open_e", "8"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test_e", "8", "0", "3", "16"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start", "8"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 11 ) {		// 1-1-11

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open_e", "8"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test_e", "8", "0", "3", "15"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start_e", "8"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 12 ) {		// 1-1-12

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open_e", "8"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test_e", "8", "0", "3", "4"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start_e", "8"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 13 ) {		// 1-1-13

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test", "0", "1"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start", "0"};
		char test_argv_05[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "get_pls", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_05) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_05);

	/******************************************************************/
	} else if ( testno == 14 ) {		// 1-1-14

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open_e", "0", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test_e", "0", "0", "3", "16"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start", "0"};
		char test_argv_05[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "get_pls", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_05) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_05);

	/******************************************************************/
	} else if ( testno == 15 ) {		// 1-1-15

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open_e", "0", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test_e", "0", "0", "3", "16"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start", "0"};
		char test_argv_05[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "get_int", "0"};
		char test_argv_06[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_int", "0"};
		char test_argv_07[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "get_int", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_05) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_05);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_06) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_06);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_07) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_07);

	/******************************************************************/
	} else if ( testno == 16 ) {		// 1-1-16

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "cal_cycc", "20000", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);

	/******************************************************************/
	} else if ( testno == 17 ) {		// 1-1-17

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "cal_cycc", "20000", "3"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);

	/******************************************************************/
	} else if ( testno == 18 ) {		// 1-1-18

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_trg", "3", "3"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_trg", "4", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_trg", "9", "4"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_trg", "10", "10"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 19 ) {		// 1-1-19

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_trg", "0", "12"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_trg", "5", "72"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_trg", "10", "132"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_trg", "11", "192"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 20 ) {		// 1-1-20

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_one", "0", "60000", "30000"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 21 ) {		// 1-1-21

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "stop_e", "0"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "close_e", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);

	/******************************************************************/
	} else if ( testno == 22 ) {		// 1-1-22

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test", "0", "1"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start", "0"};
		char test_argv_05[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "get_intstat", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_05) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_05);

	/******************************************************************/
	} else if ( testno == 23 ) {		// 1-1-23

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_test", "0", "0"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "get_cmn", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);

	/******************************************************************/
	} else if ( testno == 24 ) {		// 1-2-1

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "0"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "1"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "2"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "3"};
		char test_argv_05[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "4"};
		char test_argv_06[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "5"};
		char test_argv_07[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "6"};
		char test_argv_08[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "7"};
		char test_argv_09[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "8"};
		char test_argv_10[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "9"};
		char test_argv_11[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "10"};
		char test_argv_12[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "11"};
		char test_argv_13[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "12"};
		char test_argv_14[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "13"};
		char test_argv_15[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "14"};
		char test_argv_16[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "15"};
		char test_argv_17[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "16"};
		char test_argv_18[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "17"};
		char test_argv_19[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "18"};
		char test_argv_20[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "19"};
		char test_argv_21[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "20"};
		char test_argv_22[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "21"};
		char test_argv_23[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "22"};
		char test_argv_24[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "23"};
		char test_argv_25[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "24"};
		char test_argv_26[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "25"};
		char test_argv_27[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "26"};
		char test_argv_28[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "27"};
		char test_argv_29[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "28"};
		char test_argv_30[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "29"};
		char test_argv_31[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "30"};
		char test_argv_32[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "31"};
		char test_argv_33[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "32"};
		char test_argv_34[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "33"};
		char test_argv_35[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "34"};
		char test_argv_36[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "35"};
		char test_argv_37[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "36"};
		char test_argv_38[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "37"};
		char test_argv_39[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "38"};
		char test_argv_40[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "39"};
		char test_argv_41[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "40"};
		char test_argv_42[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "41"};
		char test_argv_43[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "42"};
		char test_argv_44[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "43"};
		char test_argv_45[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "44"};
		char test_argv_46[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "45"};
		char test_argv_47[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "46"};
		char test_argv_48[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "47"};
		char test_argv_49[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "48"};
		char test_argv_50[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "49"};
		char test_argv_51[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "50"};
		char test_argv_52[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "51"};
		char test_argv_53[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "52"};
		char test_argv_54[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "53"};
		char test_argv_55[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "54"};
		char test_argv_56[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "55"};
		char test_argv_57[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "err", "56"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_05) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_05);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_06) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_06);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_07) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_07);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_08) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_08);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_09) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_09);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_10) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_10);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_11) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_11);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_12) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_12);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_13) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_13);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_14) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_14);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_15) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_15);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_16) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_16);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_17) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_17);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_18) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_18);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_19) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_19);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_20) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_20);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_21) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_21);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_22) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_22);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_23) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_23);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_24) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_24);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_25) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_25);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_26) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_26);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_27) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_27);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_28) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_28);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_29) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_29);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_30) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_30);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_31) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_31);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_32) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_32);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_33) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_33);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_34) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_34);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_35) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_35);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_36) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_36);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_37) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_37);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_38) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_38);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_39) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_39);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_40) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_40);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_41) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_41);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_42) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_42);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_43) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_43);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_44) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_44);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_45) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_45);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_46) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_46);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_47) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_47);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_48) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_48);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_49) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_49);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_50) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_50);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_51) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_51);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_52) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_52);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_53) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_53);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_54) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_54);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_55) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_55);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_56) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_56);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_57) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_07);

	/******************************************************************/
	} else if ( testno == 25 ) {		// 1-2-2

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);

	/******************************************************************/
	} else if ( testno == 26 ) {		// 1-2-3

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0", "-1"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);

	/******************************************************************/
	} else if ( testno == 27 ) {		// 1-2-4

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0", "10000"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);

	/******************************************************************/
	} else if ( testno == 28 ) {		// 1-2-5

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open_e", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open_e", "0", "0"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);

	/******************************************************************/
	} else if ( testno == 29 ) {		// 1-2-6

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open_e", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open_e", "0", "-1"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);

	/******************************************************************/
	} else if ( testno == 30 ) {		// 1-2-7

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open_e", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open_e", "0", "10000"};

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);

	/******************************************************************/
	} else if ( testno == 31 ) {		// 1-2-8  1-2-10  1-2-12

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start", "0"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "stop", "0"};
		char test_argv_05[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_stp_mask", "0", "1"};	//	1-2-8
		char test_argv_06[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "stop", "0"};
		char test_argv_07[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_stp_mask", "0", "1"};	//	1-2-10
		char test_argv_08[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "stop", "0"};
		char test_argv_09[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "get_stp_out", "0"};			//	1-2-12

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_05) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_05);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_06) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_06);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_07) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_07);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_08) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_08);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_09) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_09);

	/******************************************************************/
	} else if ( testno == 31 ) {		// 1-2-9  1-2-11  1-2-13

		char test_argv_01[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "init"};
		char test_argv_02[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "open", "0"};
		char test_argv_03[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "start", "0"};
		char test_argv_04[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "stop", "0"};
		char test_argv_05[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_stp_mask", "0", "0"};	//	1-2-9
		char test_argv_06[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "stop", "0"};
		char test_argv_07[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "set_stp_mask", "0", "0"};	//	1-2-11
		char test_argv_08[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "stop", "0"};
		char test_argv_09[][D_PT_DD_PWM_CMD_LEN]  = {"ddpwm", "get_stp_out", "0"};			//	1-2-13

		pt_dd_pwm_execute_cmd((sizeof(test_argv_01) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_01);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_02) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_02);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_03) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_03);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_04) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_04);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_05) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_05);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_06) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_06);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_07) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_07);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_08) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_08);
		pt_dd_pwm_execute_cmd((sizeof(test_argv_09) / D_PT_DD_PWM_CMD_LEN), (char*)test_argv_09);

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
