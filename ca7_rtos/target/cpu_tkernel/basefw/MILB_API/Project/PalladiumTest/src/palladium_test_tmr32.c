/**
 * @file		palladium_test_tmr32.c
 * @brief		palladium_test_tmr32 process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include "palladium_test_common.h"
#include "palladium_test.h"
#include "palladium_test_interrupt.h"


#include "dd_gic.h"
#include "dd_top.h"

#include "ct_dd_tmr32.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define CMD_PRM_LEN (16) 
#define TFR_PTEST_CMD_TOKEN_NUM (10) 

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#if 0
static char* gArgv[TFR_PTEST_CMD_TOKEN_NUM]; 
#endif

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
extern VOID Ct_Dd_TMR32_Main( int argc, char** argv );


#if 0
static void pt_dd_tmr32_execute_cmd(int cmd_argc, char* cmd_argv) 
{ 
    int   j; 
    int   cmd_pos = 0; 

    for (j = 0; j < cmd_argc; j++) { 
        gArgv[j] = (cmd_argv + (cmd_pos * CMD_PRM_LEN)); 
        cmd_pos++; 
    }
    Ct_Dd_TMR32_Main(cmd_argc, gArgv); 
}
#else
static void pt_dd_tmr32_execute_cmd(char* cmd_argv[]) 
{
    int   j;
    int   cmd_pos = 0;

    for (j = 0; cmd_argv[j] != NULL; j++) {
        cmd_pos++;
    }
    Ct_Dd_TMR32_Main(cmd_pos, cmd_argv);
}
#endif

static VOID pt_dd_tmr32_start_clock( VOID )
{
	UCHAR im_tmr32_rclk_counter = 0;
	UCHAR im_tmr32_apbclk_counter = 0;

	Dd_Top_Start_Clock(&im_tmr32_rclk_counter, &Dd_Top_Get_CLKSTOP2(), ~D_DD_TOP_RCK_BIT);
	Dd_Top_Start_Clock(&im_tmr32_apbclk_counter, &Dd_Top_Get_CLKSTOP14(), ~D_DD_TOP_PERIAP_BIT);
}

static VOID pt_dd_tmr32_enable_gic( VOID )
{
//	Dd_GIC_Init();
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH0_INT,  1, D_DD_GIC_PRI30, 1 );		// intid=INT_TMR32_CH0, enable, pri=30, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH1_INT,  1, D_DD_GIC_PRI30, 1 );		// intid=INT_TMR32_CH1, enable, pri=30, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH2_INT,  1, D_DD_GIC_PRI30, 1 );		// intid=INT_TMR32_CH2, enable, pri=30, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH3_INT,  1, D_DD_GIC_PRI30, 1 );		// intid=INT_TMR32_CH3, enable, pri=30, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH4_INT,  1, D_DD_GIC_PRI30, 1 );		// intid=INT_TMR32_CH4, enable, pri=30, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH5_INT,  1, D_DD_GIC_PRI30, 1 );		// intid=INT_TMR32_CH5, enable, pri=30, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH6_INT,  1, D_DD_GIC_PRI30, 1 );		// intid=INT_TMR32_CH6, enable, pri=30, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH7_INT,  1, D_DD_GIC_PRI30, 1 );		// intid=INT_TMR32_CH7, enable, pri=30, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH8_INT,  1, D_DD_GIC_PRI30, 1 );		// intid=INT_TMR32_CH8, enable, pri=30, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH9_INT,  1, D_DD_GIC_PRI30, 1 );		// intid=INT_TMR32_CH9, enable, pri=30, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH10_INT, 1, D_DD_GIC_PRI30, 1 );		// intid=INT_TMR32_CH10, enable, pri=30, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH11_INT, 1, D_DD_GIC_PRI30, 1 );		// intid=INT_TMR32_CH11, enable, pri=30, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH12_INT, 1, D_DD_GIC_PRI30, 1 );		// intid=INT_TMR32_CH12, enable, pri=30, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH13_INT, 1, D_DD_GIC_PRI30, 1 );		// intid=INT_TMR32_CH13, enable, pri=30, cpu=1
}






void Pt_Dd_TMR32_Main(void)
{
	ULONG	time = 10; // 10ms wait				ULONG	time = 50; // 50ms wait        //50 ms or 10 ms ???????

	// GIC interrupt enable
	pt_dd_tmr32_enable_gic();

	// Clock start
	pt_dd_tmr32_start_clock();

	/*	32 div = 10ms Timer
		 8 div = 10ms Timer
		 2 div = 10ms Timer */
	
		 
	// タイマー初期化
    Palladium_Timer(P_TIMER_ID_COMMON0, P_TIMER_INIT);
	// タイマー起動
    Palladium_Timer(P_TIMER_ID_COMMON0, P_TIMER_START);


	// テストパラメータ取得(RSファイルに紐付く)
	UCHAR testno = gDDIM_Info.com._6a;




	// chの選択ができるように変更 (ch数が増加の為、増加したchのテストの為に変更 (新たなch = 10-13)
	int num;
	char *ch_num[]={"5", "10", "13", NULL}; // without ch1 and ch4 for PT
	for (num = 0; ch_num[num] != NULL; num++) { 
        Ddim_Print(("ch_num( %d ) = %s\n", num, ch_num[num]));
    }
	Ddim_Print(("dd_tmr32 Test Start.. \n"));
	
	
	
	
	// main
	for(num = 0; ch_num[num] != NULL; num++){								// 複数のchを設定できるように変更
		Ddim_Print(("ch_num( %d ) = %s\n", num, ch_num[num]));
		
		if ( testno == 1 ) {	// 1-1
			Ddim_Print(("TMR32 1-1 S\n"));
			Ddim_Print(("TMR32 1-1 Open S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "open", ch_num[num], "0", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-1 Open E\n"));
			Ddim_Print(("TMR32 1-1 Ctrl S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "ctrl", ch_num[num], "2", "reload", "270000", "270000", "1", "u", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-1 Ctrl E\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_table", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-1 Start S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "start", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			DDIM_User_Dly_Tsk(time);
			Ddim_Print(("TMR32 1-1 Start E\n"));
			Ddim_Print(("TMR32 1-1 Stop S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "stop", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-1 Stop E\n"));
			Ddim_Print(("TMR32 1-1 Close S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "close", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-1 Close E\n"));
			Ddim_Print(("TMR32 1-1 E\n"));
	
		////////////////////////////////////////////////////////////////////
		} else if ( testno == 2 ) {		// 1-2
			Ddim_Print(("TMR32 1-2 S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "open", ch_num[num], "-1", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "ctrl", ch_num[num], "8", "one", "67500", "67500", "1", "u", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_table", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "start", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			DDIM_User_Dly_Tsk(time);
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "stop", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "close", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-2 E\n"));	
	
		////////////////////////////////////////////////////////////////////
		} else if ( testno == 3 ) {		// 1-3
			Ddim_Print(("TMR32 1-3 S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "open", ch_num[num], "5000", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "ctrl", ch_num[num], "32", "reload", "16875", "16875", "1", "d", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_table", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "start", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			DDIM_User_Dly_Tsk(time);
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "stop", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "close", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-3 E\n"));
	
		////////////////////////////////////////////////////////////////////
		} else if ( testno == 4 ) {		// 1-4
			Ddim_Print(("TMR32 1-4 S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "open", ch_num[num], "0", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "ctrl", ch_num[num], "32", "reload", "16875", "16875", "2", "b", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_table", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "start", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			DDIM_User_Dly_Tsk(time);
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "stop", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "close", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-4 E\n"));
	
		////////////////////////////////////////////////////////////////////
		} else if ( testno == 5 ) {		// 1-5
			Ddim_Print(("TMR32 1-5 S\n"));
			Ddim_Print(("TMR32 1-5 Open S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "open", ch_num[num], "-1", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-5 Open E\n"));
			Ddim_Print(("TMR32 1-5 Ctrl S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "ctrl", ch_num[num], "2", "reload", "270000", "270000", "2", "d", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-5 Ctrl E\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_table", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_reload", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "set_reload", ch_num[num], "0", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_reload", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-5 Start S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "start", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			DDIM_User_Dly_Tsk(time);
			Ddim_Print(("TMR32 1-5 Start E\n"));
			Ddim_Print(("TMR32 1-5 Stop S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "stop", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-5 Stop E\n"));
			Ddim_Print(("TMR32 1-5 Close S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "close", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-5 Close E\n"));
			Ddim_Print(("TMR32 1-5 E\n"));
	
		////////////////////////////////////////////////////////////////////
		} else if ( testno == 6 ) {		// 1-6
			Ddim_Print(("TMR32 1-6 S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "open", ch_num[num], "4000", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "ctrl", ch_num[num], "8", "reload", "67500", "67500", "2", "d", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_table", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "start", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			DDIM_User_Dly_Tsk(time);
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "stop", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "close", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-6 E\n"));
	
		////////////////////////////////////////////////////////////////////
		} else if ( testno == 7 ) {		// 1-7
			Ddim_Print(("TMR32 1-7 S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "open", ch_num[num], "0", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "ctrl", ch_num[num], "32", "reload", "16875", "16875", "1", "d", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_table", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "start", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			DDIM_User_Dly_Tsk(time);
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "pause", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("Timer Pause. Start\n"));	
	
			DDIM_User_Dly_Tsk(time);	
	
			Ddim_Print(("Timer Pause. End\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_counter", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_table", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "start", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			DDIM_User_Dly_Tsk(time);
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "stop", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "close", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-7 E\n"));
	
		////////////////////////////////////////////////////////////////////
		} else if ( testno == 8 ) {		// 1-8
			Ddim_Print(("TMR32 1-8 S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "set_softcounter", ch_num[num], "100", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_softcounter", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-8 E\n"));
	
		////////////////////////////////////////////////////////////////////
		} else if ( testno == 9 ) {		// 1-9
			Ddim_Print(("TMR32 1-9 S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "set_reload", ch_num[num], "255", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "set_reload", ch_num[num], "1", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_reload", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-9 E\n"));

		////////////////////////////////////////////////////////////////////
		} else if ( testno == 10 ) {		// 1-10
			Ddim_Print(("TMR32 1-10 S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "open", ch_num[num], "0", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "u_set", ch_num[num], "10000", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_table", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "u_start_reload", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			DDIM_User_Dly_Tsk(time);
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "stop", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "close", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-10 E\n"));
	
		////////////////////////////////////////////////////////////////////
		} else if ( testno == 11 ) {		// 1-11
			Ddim_Print(("TMR32 1-11 S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "open", ch_num[num], "0", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "u_set", ch_num[num], "10000", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_table", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "u_start_one", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			DDIM_User_Dly_Tsk(time);
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "stop", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "close", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 1-11 E\n"));
	
		////////////////////////////////////////////////////////////////////
		} else if ( testno == 21 ) {		// 2-1
			Ddim_Print(("TMR32 2-1 S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "open", ch_num[num], "0", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "open", ch_num[num], "0", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "close", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 2-1 E\n"));
	
		////////////////////////////////////////////////////////////////////
#if 0
		} else if ( testno == 22 ) {		// 2-2	(PT対象外)
			Ddim_Print(("TMR32 2-2 S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "open", ch_num[num], "0", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "close", ch_num[num], "0", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "ctrl", ch_num[num], "32", "one", "16875", "16875", "1", "d", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "start", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "stop", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_counter", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_table", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "set_softcounter", ch_num[num], "1", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_softcounter", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "set_reload", ch_num[num], "1", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "get_reload", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "u_set", ch_num[num], "1000", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "u_start_reload", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "u_start_one", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			Ddim_Print(("TMR32 2-2 E\n"));
#endif

		////////////////////////////////////////////////////////////////////
#if 0
		} else if ( testno == 23 ) {		// 2-3	(PT対象外)
			Ddim_Print(("TMR32 2-3 S\n"));
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "open", ch_num[num], "-2", NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);
			}
			{
				char *tmr_argv[CMD_PRM_LEN] = { "tmr32", "close", ch_num[num], NULL };
				pt_dd_tmr32_execute_cmd(tmr_argv);	
			}
			Ddim_Print(("TMR32 2-3 E\n"));
#endif

#if 0
			// 3-1	(PT対象外)
			Ddim_Print(("TMR32 3-1 E\n"));	// There is no code for Test 3-3. ????????
#endif
		}
	} //for loop of verious ch_num
	
	
	
	
	
	/*
	
	// SVN version (2016/2/29 version)
	
	////////////////////////////////////////////////////////////////////
	if ( testno == 1 ) {	// 1-1
		Ddim_Print(("TMR32 1-1 S\n"));
		Ddim_Print(("TMR32 1-1 Open S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "open", "5", "0", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-1 Open E\n"));
		Ddim_Print(("TMR32 1-1 Ctrl S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "ctrl", "5", "2", "reload", "270000", "270000", "1", "d", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-1 Ctrl E\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_table", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-1 Start S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "start", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		DDIM_User_Dly_Tsk(time);
		Ddim_Print(("TMR32 1-1 Start E\n"));
		Ddim_Print(("TMR32 1-1 Stop S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "stop", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-1 Stop E\n"));
		Ddim_Print(("TMR32 1-1 Close S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "close", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-1 Close E\n"));
		Ddim_Print(("TMR32 1-1 E\n"));

	////////////////////////////////////////////////////////////////////
	} else if ( testno == 2 ) {		// 1-2
		Ddim_Print(("TMR32 1-2 S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "open", "5", "-1", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "ctrl", "5", "8", "one", "67500", "67500", "1", "d", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_table", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "start", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		DDIM_User_Dly_Tsk(time);
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "stop", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "close", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-2 E\n"));

	////////////////////////////////////////////////////////////////////
	} else if ( testno == 3 ) {		// 1-3
		Ddim_Print(("TMR32 1-3 S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "open", "5", "5000", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "ctrl", "5", "32", "reload", "16875", "16875", "1", "d", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_table", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "start", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		DDIM_User_Dly_Tsk(time);
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "stop", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "close", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-3 E\n"));

	////////////////////////////////////////////////////////////////////
	} else if ( testno == 4 ) {		// 1-4
		Ddim_Print(("TMR32 1-4 S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "open", "5", "0", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "ctrl", "5", "32", "reload", "16875", "16875", "2", "d", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_table", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "start", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		DDIM_User_Dly_Tsk(time);
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "stop", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "close", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-4 E\n"));

	////////////////////////////////////////////////////////////////////
	} else if ( testno == 5 ) {		// 1-5
		Ddim_Print(("TMR32 1-5 S\n"));
		Ddim_Print(("TMR32 1-5 Open S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "open", "5", "-1", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-5 Open E\n"));
		Ddim_Print(("TMR32 1-5 Ctrl S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "ctrl", "5", "2", "reload", "270000", "270000", "2", "d", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-5 Ctrl E\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_table", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_reload", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "set_reload", "5", "0", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_reload", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-5 Start S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "start", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		DDIM_User_Dly_Tsk(time);
		Ddim_Print(("TMR32 1-5 Start E\n"));
		Ddim_Print(("TMR32 1-5 Stop S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "stop", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-5 Stop E\n"));
		Ddim_Print(("TMR32 1-5 Close S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "close", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-5 Close E\n"));
		Ddim_Print(("TMR32 1-5 E\n"));

	////////////////////////////////////////////////////////////////////
	} else if ( testno == 6 ) {		// 1-6
		Ddim_Print(("TMR32 1-6 S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "open", "5", "4000", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "ctrl", "5", "8", "reload", "67500", "67500", "2", "d", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_table", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "start", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		DDIM_User_Dly_Tsk(time);
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "stop", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "close", "5", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-6 E\n"));

	////////////////////////////////////////////////////////////////////
	} else if ( testno == 7 ) {		// 1-7
		Ddim_Print(("TMR32 1-7 S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "open", "6", "0", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "ctrl", "6", "32", "reload", "16875", "16875", "1", "d", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_table", "6", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "start", "6", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		DDIM_User_Dly_Tsk(time);
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "pause", "6", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("Timer Pause. Start\n"));

		DDIM_User_Dly_Tsk(time);

		Ddim_Print(("Timer Pause. End\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_counter", "6", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_table", "6", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "start", "6", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		DDIM_User_Dly_Tsk(time);
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "stop", "6", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "close", "6", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-7 E\n"));

	////////////////////////////////////////////////////////////////////
	} else if ( testno == 8 ) {		// 1-8
		Ddim_Print(("TMR32 1-8 S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "set_softcounter", "7", "100", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_softcounter", "7", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-8 E\n"));

	////////////////////////////////////////////////////////////////////
	} else if ( testno == 9 ) {		// 1-9
		Ddim_Print(("TMR32 1-9 S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "set_reload", "8", "255", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "set_reload", "8", "1", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_reload", "8", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-9 E\n"));

	////////////////////////////////////////////////////////////////////
	} else if ( testno == 10 ) {		// 1-10
		Ddim_Print(("TMR32 1-10 S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "open", "9", "0", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "u_set", "9", "10000", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_table", "9", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "u_start_reload", "9", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		DDIM_User_Dly_Tsk(time);
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "stop", "9", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "close", "9", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-10 E\n"));

	////////////////////////////////////////////////////////////////////
	} else if ( testno == 11 ) {		// 1-11
		Ddim_Print(("TMR32 1-11 S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "open", "9", "0", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "u_set", "9", "10000", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_table", "9", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "u_start_one", "9", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		DDIM_User_Dly_Tsk(time);
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "stop", "9", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "close", "9", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 1-11 E\n"));

	////////////////////////////////////////////////////////////////////
	} else if ( testno == 21 ) {		// 2-1
		Ddim_Print(("TMR32 2-1 S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "open", "9", "0", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "open", "9", "0", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "close", "9", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 2-1 E\n"));

	////////////////////////////////////////////////////////////////////
#if 0
	} else if ( testno == 22 ) {		// 2-2
		Ddim_Print(("TMR32 2-2 S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "open", "12", "0", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "close", "12", "0", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "ctrl", "12", "32", "one", "16875", "16875", "1", "d", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "start", "12", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "stop", "12", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_counter", "12", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_table", "12", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "set_softcounter", "12", "1", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_softcounter", "12", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "set_reload", "12", "1", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "get_reload", "12", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "u_set", "12", "1000", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "u_start_reload", "12", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "u_start_one", "12", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 2-2 E\n"));
#endif

	////////////////////////////////////////////////////////////////////
#if 0
	} else if ( testno == 23 ) {		// 2-3
		Ddim_Print(("TMR32 2-3 S\n"));
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "open", "1", "-2", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		{
			char *tmr_argv[CMD_PRM_LEN] = {"tmr32", "close", "1", NULL};
			pt_dd_tmr32_execute_cmd(tmr_argv);
		}
		Ddim_Print(("TMR32 2-3 E\n"));
#endif
	}

*/


	Ddim_Print( ("dd_tmr32 Test End \n") );

    // タイマー停止
    Palladium_Timer(P_TIMER_ID_COMMON0, P_TIMER_END);

    // タイマーログ出力
    Palladium_Timer( P_TIMER_ID_COMMON0 , P_TIMER_PRINT );



	// meaning???
	Palladium_Force_Stop();
	
	
	
	
	

}
