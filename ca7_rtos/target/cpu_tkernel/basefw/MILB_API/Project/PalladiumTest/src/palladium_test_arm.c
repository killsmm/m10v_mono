/**
 * @file		palladium_test_arm.c
 * @brief		palladium test arm for ARM
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include "palladium_test.h"
//#include "ct_dd_hdmac.h"
//#include "dd_gic.h"
#include "dd_arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define CMD_PRM_LEN (15)
//#define CMD_PRM_NUM (12)

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
//static char* gArgv[CMD_PRM_NUM];
static ULONG gDD_Pt_Arm_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

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
/*
static VOID pt_dd_hdmac_execute_cmd(int cmd_argc, char* cmd_argv)
{
    INT32   j;
    INT32   cmd_pos = 0;

    for (j = 0; j < cmd_argc; j++) {
        gArgv[j] = (cmd_argv + (cmd_pos * CMD_PRM_LEN));
        cmd_pos++;
    }

    Ct_Dd_HDMAC_Main(cmd_argc, gArgv);
}
*/
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
void Pt_Dd_ARM_Main( void )
{
	UCHAR	testno;

	// Test Code Start
//	Dd_GIC_Init();
//	Dd_GIC_Ctrl(E_DD_GIC_INTID_EXS_HDMAC_CH0_INT, 1, D_DD_GIC_PRI30, 1);
//	Dd_GIC_Ctrl(E_DD_GIC_INTID_EXS_HDMAC_CH1_INT, 1, D_DD_GIC_PRI30, 1);
//	Dd_GIC_Ctrl(E_DD_GIC_INTID_EXS_HDMAC_CH2_INT, 1, D_DD_GIC_PRI30, 1);
//	Dd_GIC_Ctrl(E_DD_GIC_INTID_EXS_HDMAC_CH3_INT, 1, D_DD_GIC_PRI30, 1);

	// テストパラメータ取得(RSファイルに紐付く)
	testno	= gDDIM_Info.com._6a;

	// set HDMAC test data
//	Palladium_Set_In_Localstack(D_HDMAC_AUTO_TEST_ADR_SRC, D_HDMAC_AUTO_TEST_SIZE);

	// レジスタ取得＆設定(#define PC_DEBUG を定義すること)
	/******************************************************************/
	if ( testno == 1 ) {	// HDMAC Auto Test 1

//		char hdmac_argv_1[][CMD_PRM_LEN] = {"hdmac", "auto", "1"};
//
		Ddim_Print(("ARM Test 1 start\n"));
		Dd_ARM_Critical_Section_Start(gDD_Pt_Arm_Spin_Lock);
		Dd_ARM_Critical_Section_End(gDD_Pt_Arm_Spin_Lock);
		Ddim_Print(("ARM Test 1 end\n"));
	}

//	Palladium_Set_Out_Localstack(D_HDMAC_AUTO_TEST_ADR_DST, D_HDMAC_AUTO_TEST_SIZE);

//#endif
}