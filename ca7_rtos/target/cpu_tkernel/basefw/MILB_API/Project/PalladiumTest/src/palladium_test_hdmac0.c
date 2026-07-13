/**
 * @file		palladium_test_hdmac0.c
 * @brief		palladium test code for HDMAC0
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include "palladium_test.h"
#include "ct_dd_hdmac0.h"
#include "dd_gic.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define CMD_PRM_LEN (15)
#define CMD_PRM_NUM (12)

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
static char* gArgv[CMD_PRM_NUM];

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
static VOID pt_dd_hdmac0_execute_cmd(int cmd_argc, char* cmd_argv)
{
    INT32   j;
    INT32   cmd_pos = 0;

    for (j = 0; j < cmd_argc; j++) {
        gArgv[j] = (cmd_argv + (cmd_pos * CMD_PRM_LEN));
        cmd_pos++;
    }

    Ct_Dd_HDMAC0_Main(cmd_argc, gArgv);
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
void Pt_Dd_HDMAC0_Main( void )
{
	UCHAR	testno;

	// Test Code Start
//	Dd_GIC_Init();
	Dd_GIC_Ctrl(E_DD_GIC_INTID_HDMAC0_CH0_INT, 1, D_DD_GIC_PRI30, 1);
	Dd_GIC_Ctrl(E_DD_GIC_INTID_HDMAC0_CH1_INT, 1, D_DD_GIC_PRI30, 1);
	Dd_GIC_Ctrl(E_DD_GIC_INTID_HDMAC0_CH2_INT, 1, D_DD_GIC_PRI30, 1);
	Dd_GIC_Ctrl(E_DD_GIC_INTID_HDMAC0_CH3_INT, 1, D_DD_GIC_PRI30, 1);

	// テストパラメータ取得(RSファイルに紐付く)
	testno	= gDDIM_Info.com._6a;

	// set HDMAC0 test data
	Palladium_Set_In_Localstack(D_HDMAC0_AUTO_TEST_ADR_SRC, D_HDMAC0_AUTO_TEST_SIZE);

	// レジスタ取得＆設定(#define PC_DEBUG を定義すること)
	/******************************************************************/
	if ( testno == 1 ) {	// HDMAC0 Auto Test 1

		char hdmac0_argv_1[][CMD_PRM_LEN] = {"hdmac0", "auto", "1"};

		Ddim_Print(("HDMAC0 Auto Test 1 start\n"));
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_1);
		Ddim_Print(("HDMAC0 Auto Test 1 end\n"));

	/******************************************************************/
	} else if ( testno == 2 ) {		// HDMAC0 Auto Test 2

		char hdmac0_argv_1[][CMD_PRM_LEN] = {"hdmac0", "auto", "2"};

		Ddim_Print(("HDMAC0 Auto Test 2 start\n"));
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_1);
		Ddim_Print(("HDMAC0 Auto Test 2 end\n"));

	/******************************************************************/
	} else if ( testno == 3 ) {		// HDMAC0 Auto Test 3

		char hdmac0_argv_1[][CMD_PRM_LEN] = {"hdmac0", "auto", "3"};

		Ddim_Print(("HDMAC0 Auto Test 3 start\n"));
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_1);
		Ddim_Print(("HDMAC0 Auto Test 3 end\n"));

	/******************************************************************/
	} else if ( testno == 4 ) {		// HDMAC0 Auto Test 4

		char hdmac0_argv_1[][CMD_PRM_LEN] = {"hdmac0", "auto", "4"};

		Ddim_Print(("HDMAC0 Auto Test 4 start\n"));
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_1);
		Ddim_Print(("HDMAC0 Auto Test 4 end\n"));

	/******************************************************************/
	} else if ( testno == 5 ) {		// HDMAC0 Auto Test 5

		char hdmac0_argv_1[][CMD_PRM_LEN] = {"hdmac0", "auto", "5"};

		Ddim_Print(("HDMAC0 Auto Test 5 start\n"));
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_1);
		Ddim_Print(("HDMAC0 Auto Test 5 end\n"));

	/******************************************************************/
	} else if ( testno == 6 ) {		// HDMAC0 Auto Test 6

		char hdmac0_argv_1[][CMD_PRM_LEN] = {"hdmac0", "auto", "6"};

		Ddim_Print(("HDMAC0 Auto Test 6 start\n"));
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_1);
		Ddim_Print(("HDMAC0 Auto Test 6 end\n"));

	/******************************************************************/
	} else if ( testno == 11 ) {		// Arb Clear/Get

		char hdmac0_argv_1[][CMD_PRM_LEN] = {"hdmac0", "arb_clear"};
		char hdmac0_argv_2[][CMD_PRM_LEN] = {"hdmac0", "arb_get"};

		Ddim_Print(("HDMAC0 Arbitration Clear Test start\n"));
		pt_dd_hdmac0_execute_cmd(2, (char*)hdmac0_argv_1);
		pt_dd_hdmac0_execute_cmd(2, (char*)hdmac0_argv_2);
		Ddim_Print(("HDMAC0 Arbitration Clear Test end\n"));

	/******************************************************************/
	} else if ( testno == 12 ) {		// Arb Set/Get

		char hdmac0_argv_1[][CMD_PRM_LEN] = {"hdmac0", "arb_set", "rotate"};
		char hdmac0_argv_2[][CMD_PRM_LEN] = {"hdmac0", "arb_get"};

		Ddim_Print(("HDMAC0 Arbitration Set Test start\n"));
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_1);
		pt_dd_hdmac0_execute_cmd(2, (char*)hdmac0_argv_2);
		Ddim_Print(("HDMAC0 Arbitration Set Test end\n"));

	/******************************************************************/
	} else if ( testno == 21 ) {		// HDMAC0 Transfer Width get 1

		char hdmac0_argv_1[][CMD_PRM_LEN] = {"hdmac0", "get_width", "45000000", "46000000", "256"};

		Ddim_Print(("HDMAC0 Transfer Width get 1 start\n"));
		pt_dd_hdmac0_execute_cmd(5, (char*)hdmac0_argv_1);
		Ddim_Print(("HDMAC0 Transfer Width get 1 end\n"));

	/******************************************************************/
	} else if ( testno == 22 ) {		// HDMAC0 Transfer Width get 2

		char hdmac0_argv_1[][CMD_PRM_LEN] = {"hdmac0", "get_width", "45000000", "46000002", "256"};

		Ddim_Print(("HDMAC0 Transfer Width get 2 start\n"));
		pt_dd_hdmac0_execute_cmd(5, (char*)hdmac0_argv_1);
		Ddim_Print(("HDMAC0 Transfer Width get 2 end\n"));

	/******************************************************************/
	} else if ( testno == 23 ) {		// HDMAC0 Transfer Width get 3

		char hdmac0_argv_1[][CMD_PRM_LEN] = {"hdmac0", "get_width", "45000000", "46000002", "257"};

		Ddim_Print(("HDMAC0 Transfer Width get 3 start\n"));
		pt_dd_hdmac0_execute_cmd(5, (char*)hdmac0_argv_1);
		Ddim_Print(("HDMAC0 Transfer Width get 3 end\n"));

	/******************************************************************/
	} else if ( testno == 31 ) {		// HDMAC0 Pause/Resume GetSize test

		char hdmac0_argv_1[][CMD_PRM_LEN]  = {"hdmac0", "ctrl_trns_md", "0", "soft", "blk", "src_incr", "dst_incr", "normal"};
		char hdmac0_argv_2[][CMD_PRM_LEN]  = {"hdmac0", "ctrl_trns_sz", "0", "1", "0x96000", "0x41000000", "0x41096000"};
		char hdmac0_argv_3[][CMD_PRM_LEN]  = {"hdmac0", "ctrl_trns", "0"};
		char hdmac0_argv_4[][CMD_PRM_LEN]  = {"hdmac0", "start_async", "0"};
		char hdmac0_argv_5[][CMD_PRM_LEN]  = {"hdmac0", "pause", "0"};
		char hdmac0_argv_6[][CMD_PRM_LEN]  = {"hdmac0", "status_read", "0"};
		char hdmac0_argv_7[][CMD_PRM_LEN]  = {"hdmac0", "status_clear", "0"};
		char hdmac0_argv_8[][CMD_PRM_LEN]  = {"hdmac0", "status_read", "0"};
		char hdmac0_argv_9[][CMD_PRM_LEN]  = {"hdmac0", "trns_size_get", "0"};
		char hdmac0_argv_10[][CMD_PRM_LEN] = {"hdmac0", "rmn_size_get", "0"};
		char hdmac0_argv_11[][CMD_PRM_LEN] = {"hdmac0", "total_size_get", "0"};
		char hdmac0_argv_12[][CMD_PRM_LEN] = {"hdmac0", "src_addr_get", "0"};
		char hdmac0_argv_13[][CMD_PRM_LEN] = {"hdmac0", "dst_addr_get", "0"};
		char hdmac0_argv_14[][CMD_PRM_LEN] = {"hdmac0", "resume", "0"};
		char hdmac0_argv_15[][CMD_PRM_LEN] = {"hdmac0", "wait", "0", "event"};

		Ddim_Print(("HDMAC0 Pause/Resume Test start\n"));
		pt_dd_hdmac0_execute_cmd(8, (char*)hdmac0_argv_1);		// ctrl_trns_md
		pt_dd_hdmac0_execute_cmd(7, (char*)hdmac0_argv_2);		// ctrl_trns_sz
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_3);		// ctrl_trns
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_4);		// start_async
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_5);		// pause
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_6);		// status_read
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_7);		// status_clear
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_8);		// status_read
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_9);		// trns_size_get
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_10);		// rmn_size_get
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_11);		// total_size_get
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_12);		// src_addr_get
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_13);		// dst_addr_get
		pt_dd_hdmac0_execute_cmd(3, (char*)hdmac0_argv_14);		// resume
		pt_dd_hdmac0_execute_cmd(4, (char*)hdmac0_argv_15);		// wait
	}

	Palladium_Set_Out_Localstack(D_HDMAC0_AUTO_TEST_ADR_DST, D_HDMAC0_AUTO_TEST_SIZE);

//#endif
}