/**
 * @file		palladium_test_mxic.c
 * @brief		MXIC test code on palladium
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "im_mxic.h"
#include "ct_im_mxic.h"
#include "dd_gic.h"
#include "dd_top.h"
#include "driver_common.h"
#include "palladium_test.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PT_MXIC_CMD_LEN			(20)

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
static char* gCmd_Argv[ D_PT_MXIC_CMD_LEN ];

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
static VOID pt_im_mxic_enable_gic( VOID )
{
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH0_INT,  1, D_DD_GIC_PRI30, 1 );		// cpu=0, intid=118, enable, pri=30
	Dd_GIC_Ctrl( E_DD_GIC_INTID_MXIC_UNIT0_INT, 1, D_DD_GIC_PRI30, 1 );		// int enable
	Dd_GIC_Ctrl( E_DD_GIC_INTID_MXIC_UNIT1_INT, 1, D_DD_GIC_PRI30, 1 );		// int enable
	Dd_GIC_Ctrl( E_DD_GIC_INTID_MXIC_UNIT2_INT, 1, D_DD_GIC_PRI30, 1 );		// int enable
	Dd_GIC_Ctrl( E_DD_GIC_INTID_MXIC_UNIT3_INT, 1, D_DD_GIC_PRI30, 1 );		// int enable
	Dd_GIC_Ctrl( E_DD_GIC_INTID_MXIC_UNIT4_INT, 1, D_DD_GIC_PRI30, 1 );		// int enable
	Dd_GIC_Ctrl( E_DD_GIC_INTID_MXIC_UNIT5_INT, 1, D_DD_GIC_PRI30, 1 );		// int enable
	Dd_GIC_Ctrl( E_DD_GIC_INTID_MXIC_UNIT6_INT, 1, D_DD_GIC_PRI30, 1 );		// int enable
}


static VOID pt_im_mxic_start_clock( VOID )
{
	UCHAR im_mxic_pclk_ctrl_cnt[ 7 ] = { 0, 0, 0, 0, 0, 0, 0 };
	UCHAR im_mxic_hclk_ctrl_cnt[ 3 ] = { 0, 0, 0 };

	Dd_Top_Start_Clock( (UCHAR*)&im_mxic_pclk_ctrl_cnt[ 0 ], &Dd_Top_Get_CLKSTOP5(), ~D_DD_TOP_MICAP0_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&im_mxic_pclk_ctrl_cnt[ 1 ], &Dd_Top_Get_CLKSTOP5(), ~D_DD_TOP_MICAP1_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&im_mxic_pclk_ctrl_cnt[ 2 ], &Dd_Top_Get_CLKSTOP5(), ~D_DD_TOP_MICAP2_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&im_mxic_pclk_ctrl_cnt[ 3 ], &Dd_Top_Get_CLKSTOP5(), ~D_DD_TOP_MICAP3_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&im_mxic_pclk_ctrl_cnt[ 4 ], &Dd_Top_Get_CLKSTOP5(), ~D_DD_TOP_MICAP4_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&im_mxic_pclk_ctrl_cnt[ 5 ], &Dd_Top_Get_CLKSTOP5(), ~D_DD_TOP_MICAP5_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&im_mxic_pclk_ctrl_cnt[ 6 ], &Dd_Top_Get_CLKSTOP5(), ~D_DD_TOP_MICAP6_BIT );

	Dd_Top_Start_Clock( (UCHAR*)&im_mxic_hclk_ctrl_cnt[ 0 ], &Dd_Top_Get_CLKSTOP6(), ~D_DD_TOP_MICAH1_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&im_mxic_hclk_ctrl_cnt[ 1 ], &Dd_Top_Get_CLKSTOP6(), ~D_DD_TOP_MICAH2_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&im_mxic_hclk_ctrl_cnt[ 2 ], &Dd_Top_Get_CLKSTOP6(), ~D_DD_TOP_MICAH3_BIT );
}

static void pt_im_mxic_execute_cmd( int cmd_argc, char* cmd_argv ) 
{ 
	INT32 j;
	INT32 cmd_pos = 0;

	if( cmd_argc > D_PT_MXIC_CMD_LEN ) {
		cmd_argc = D_PT_MXIC_CMD_LEN;
	}
	for( j = 0; j < cmd_argc; j++ ) { 
		gCmd_Argv[ j ] = ( cmd_argv + ( cmd_pos * D_PT_MXIC_CMD_LEN ) ); 
		cmd_pos++;
	}

	Ct_Im_MXIC_Main( cmd_argc, gCmd_Argv ); 
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		MXIC main code
 * @param		void
 * @return		void
 * @note		None
 * @attention	None
 */
void Pt_Im_MXIC_Main( void )
{
	UINT32	testno;

	// 開始...
	Ddim_Print(( "****** MXIC begin Pt_Im_MXIC_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	testno	= gDDIM_Info.com._6a;

	Ddim_Print(( "****** Palladium Test %d begin\n", testno ));

	pt_im_mxic_enable_gic();
	pt_im_mxic_start_clock();

	while( 1 ) {

		// 初期化.
		if ( testno == 0 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "init",      "3"                       };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "master",    "all",  "get", "3"        };
			char test_argv3[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "pri",       "slot", "all", "get", "3" };
			char test_argv4[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "levelport", "all",  "get", "3"        };

			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);
			pt_im_mxic_execute_cmd((sizeof(test_argv3) / D_PT_MXIC_CMD_LEN), (char*)test_argv3);
			pt_im_mxic_execute_cmd((sizeof(test_argv4) / D_PT_MXIC_CMD_LEN), (char*)test_argv4);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// コンフィグレーション起動.
		else if ( testno == 1 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "start", "3", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1" };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "start", "3", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0" };

			// enable/disable
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// クロック供給制御.
		else if ( testno == 2 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "clock", "set", "3", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1", "1" };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "clock", "set", "3", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0" };
			char test_argv3[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "clock", "get", "3" };

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv3) / D_PT_MXIC_CMD_LEN), (char*)test_argv3);

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);
			pt_im_mxic_execute_cmd((sizeof(test_argv3) / D_PT_MXIC_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// acceptance capability設定.
		else if ( testno == 3 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "acc_capa", "all",   "set", "1", "1" };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "acc_capa", "all",   "get", "1"      };
			char test_argv3[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "acc_capa", "slave", "set", "1", "0", "3", "3", "8"  };
			char test_argv4[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "acc_capa", "slave", "get", "1", "0", "3", "3"       };
			char test_argv5[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "acc_capa", "group", "set",      "0", "7", "1", "1"  };
			char test_argv6[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "acc_capa", "group", "get",      "0", "7", "1"       };

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv3) / D_PT_MXIC_CMD_LEN), (char*)test_argv3);
			pt_im_mxic_execute_cmd((sizeof(test_argv4) / D_PT_MXIC_CMD_LEN), (char*)test_argv4);

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv5) / D_PT_MXIC_CMD_LEN), (char*)test_argv5);
			pt_im_mxic_execute_cmd((sizeof(test_argv6) / D_PT_MXIC_CMD_LEN), (char*)test_argv6);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// スレーブ領域設定.
		else if ( testno == 4 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "slave_area", "all", "set", "3", "0x20000000", "0x40000000", "0x20000000", "0x60000000", "0x20000000", "0x80000000", "0x20000000", "0xA0000000" };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "slave_area", "all", "get", "3", };
			char test_argv3[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "slave_area", "one", "set", "3", "0", "0x40000000", "0x40000000" };
			char test_argv4[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "slave_area", "one", "get", "3", "0" };

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv3) / D_PT_MXIC_CMD_LEN), (char*)test_argv3);
			pt_im_mxic_execute_cmd((sizeof(test_argv4) / D_PT_MXIC_CMD_LEN), (char*)test_argv4);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// マスタ設定.
		else if ( testno == 5 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "master", "all", "set", "3"      };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "master", "all", "get", "3"      };
			char test_argv3[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "master", "w",   "set", "3", "0" };
			char test_argv4[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "master", "w",   "get", "3"      };
			char test_argv5[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "master", "r",   "set", "3", "0" };
			char test_argv6[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "master", "r",   "get", "3"      };

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv3) / D_PT_MXIC_CMD_LEN), (char*)test_argv3);
			pt_im_mxic_execute_cmd((sizeof(test_argv4) / D_PT_MXIC_CMD_LEN), (char*)test_argv4);

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv5) / D_PT_MXIC_CMD_LEN), (char*)test_argv5);
			pt_im_mxic_execute_cmd((sizeof(test_argv6) / D_PT_MXIC_CMD_LEN), (char*)test_argv6);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// レベル間優先順位制御.
		else if ( testno == 6 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "pri", "slot", "all", "set", "3" };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "pri", "slot", "all", "get", "3" };
			char test_argv3[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "pri", "slot", "one", "set", "3", "0", "0", "0", "0", "0", "0", "63" };
			char test_argv4[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "pri", "slot", "one", "get", "3", "0", "0", "0", "0"            };

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv3) / D_PT_MXIC_CMD_LEN), (char*)test_argv3);
			pt_im_mxic_execute_cmd((sizeof(test_argv4) / D_PT_MXIC_CMD_LEN), (char*)test_argv4);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// レベル内優先順位制御.
		else if ( testno == 7 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "pri", "master", "set", "3", "1", "0" };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "pri", "master", "get", "3", "1"      };

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// スロットマスク制御.
		else if ( testno == 8 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "mask", "slot", "set", "3", "0", "0", "0", "0", "1" };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "mask", "slot", "get", "3", "0", "0", "0", "0"      };

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// マスタ転送要求マスク制御.
		else if ( testno == 9 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "mask", "master", "enable", "set", "3", "1", "0", "4" };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "mask", "master", "enable", "get", "3", "1"           };

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// マスタ転送要求マスク要因・対象・状態選択設定.
		else if ( testno == 10 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "mask", "master", "factor", "set", "3", "0x1", "0", "1" };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "mask", "master", "factor", "get", "3", "0x1", "0"      };
			char test_argv3[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "mask", "master", "target", "set", "3", "0x2", "0", "1" };
			char test_argv4[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "mask", "master", "target", "get", "3", "0x2", "0"      };
			char test_argv5[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "mask", "master", "select", "set", "3", "0x1", "1"      };
			char test_argv6[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "mask", "master", "select", "get", "3", "0x1"           };

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv3) / D_PT_MXIC_CMD_LEN), (char*)test_argv3);
			pt_im_mxic_execute_cmd((sizeof(test_argv4) / D_PT_MXIC_CMD_LEN), (char*)test_argv4);

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv5) / D_PT_MXIC_CMD_LEN), (char*)test_argv5);
			pt_im_mxic_execute_cmd((sizeof(test_argv6) / D_PT_MXIC_CMD_LEN), (char*)test_argv6);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// マスタ転送要求トランザクション数マスク制御.
		else if ( testno == 11 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "mask", "master", "trn", "set", "3", "0x1", "1" };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "mask", "master", "trn", "get", "3", "0x1"      };

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// マスタアクセスレベルポート設定.
		else if ( testno == 12 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "levelport", "all",   "set", "3" };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "levelport", "all",   "get", "3" };
			char test_argv3[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "levelport", "port",  "set", "3", "0", "0", "0x11", "0" };
			char test_argv4[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "levelport", "port",  "get", "3", "0", "0", "0x11"      };
			char test_argv5[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "levelport", "level", "set", "3", "0", "0", "0x11", "0" };
			char test_argv6[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "levelport", "level", "get", "3", "0", "0", "0x11"      };

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv3) / D_PT_MXIC_CMD_LEN), (char*)test_argv3);
			pt_im_mxic_execute_cmd((sizeof(test_argv4) / D_PT_MXIC_CMD_LEN), (char*)test_argv4);

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv5) / D_PT_MXIC_CMD_LEN), (char*)test_argv5);
			pt_im_mxic_execute_cmd((sizeof(test_argv6) / D_PT_MXIC_CMD_LEN), (char*)test_argv6);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// マスタ出力ポート設定
		else if ( testno == 13 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "port", "assign", "all", "set", "1"         };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "port", "assign", "all", "get"              };
			char test_argv3[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "port", "assign", "one", "set", "0x40", "1" };
			char test_argv4[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "port", "assign", "one", "get", "0x40"      };

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv3) / D_PT_MXIC_CMD_LEN), (char*)test_argv3);
			pt_im_mxic_execute_cmd((sizeof(test_argv4) / D_PT_MXIC_CMD_LEN), (char*)test_argv4);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// アクセスモニタ
		else if ( testno == 14 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ]  = { "immxic", "monitor", "acc",    "param",  "set",   "0"           };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ]  = { "immxic", "monitor", "acc",    "param",  "get",                 };
			char test_argv3[][ D_PT_MXIC_CMD_LEN ]  = { "immxic", "monitor", "start"                                    };
			char test_argv4[][ D_PT_MXIC_CMD_LEN ]  = { "immxic", "monitor", "stop"                                     };
			char test_argv5[][ D_PT_MXIC_CMD_LEN ]  = { "immxic", "monitor", "hist",   "thstop", "set",   "1"           };
			char test_argv6[][ D_PT_MXIC_CMD_LEN ]  = { "immxic", "monitor", "hist",   "thstop", "get"                  };
			char test_argv7[][ D_PT_MXIC_CMD_LEN ]  = { "immxic", "monitor", "hist",   "all"                            };
			char test_argv8[][ D_PT_MXIC_CMD_LEN ]  = { "immxic", "monitor", "hist",   "one",             "0", "0", "0" };
			char test_argv9[][ D_PT_MXIC_CMD_LEN ]  = { "immxic", "monitor", "status", "slot",   "all"                  };
			char test_argv10[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "monitor", "status", "slot",   "3",     "w", "0"      };
			char test_argv11[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "monitor", "status", "slot",   "3",     "r", "0"      };
			char test_argv12[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "monitor", "status", "slot",   "wgr",   "0"           };
			char test_argv13[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "monitor", "status", "slot",   "rgr",   "0"           };
			char test_argv14[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "monitor", "status", "master", "3"                    };
			char test_argv15[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "monitor", "acc",    "result", "all"                  };
			char test_argv16[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "monitor", "acc",    "result", "one",   "0"           };
			char test_argv17[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "monitor", "acc",    "state"                          };

			pt_im_mxic_execute_cmd((sizeof(test_argv1)  / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2)  / D_PT_MXIC_CMD_LEN), (char*)test_argv2);
			pt_im_mxic_execute_cmd((sizeof(test_argv3)  / D_PT_MXIC_CMD_LEN), (char*)test_argv3);
			pt_im_mxic_execute_cmd((sizeof(test_argv4)  / D_PT_MXIC_CMD_LEN), (char*)test_argv4);
			pt_im_mxic_execute_cmd((sizeof(test_argv5)  / D_PT_MXIC_CMD_LEN), (char*)test_argv5);
			pt_im_mxic_execute_cmd((sizeof(test_argv6)  / D_PT_MXIC_CMD_LEN), (char*)test_argv6);
			pt_im_mxic_execute_cmd((sizeof(test_argv7)  / D_PT_MXIC_CMD_LEN), (char*)test_argv7);
			pt_im_mxic_execute_cmd((sizeof(test_argv8)  / D_PT_MXIC_CMD_LEN), (char*)test_argv8);
			pt_im_mxic_execute_cmd((sizeof(test_argv9)  / D_PT_MXIC_CMD_LEN), (char*)test_argv9);
			pt_im_mxic_execute_cmd((sizeof(test_argv10) / D_PT_MXIC_CMD_LEN), (char*)test_argv10);
			pt_im_mxic_execute_cmd((sizeof(test_argv11) / D_PT_MXIC_CMD_LEN), (char*)test_argv11);
			pt_im_mxic_execute_cmd((sizeof(test_argv12) / D_PT_MXIC_CMD_LEN), (char*)test_argv12);
			pt_im_mxic_execute_cmd((sizeof(test_argv13) / D_PT_MXIC_CMD_LEN), (char*)test_argv13);
			pt_im_mxic_execute_cmd((sizeof(test_argv14) / D_PT_MXIC_CMD_LEN), (char*)test_argv14);
			pt_im_mxic_execute_cmd((sizeof(test_argv15) / D_PT_MXIC_CMD_LEN), (char*)test_argv15);
			pt_im_mxic_execute_cmd((sizeof(test_argv16) / D_PT_MXIC_CMD_LEN), (char*)test_argv16);
			pt_im_mxic_execute_cmd((sizeof(test_argv17) / D_PT_MXIC_CMD_LEN), (char*)test_argv17);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// メモリアクセス検出
		else if ( testno == 15 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "memacc", "set",   "3", "9", "0" };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "memacc", "start", "3", "9"      };
			char test_argv3[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "memacc", "get",   "3"           };
			char test_argv4[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "memacc", "stop",  "3"           };

			// set/start/get/stop
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);
			pt_im_mxic_execute_cmd((sizeof(test_argv3) / D_PT_MXIC_CMD_LEN), (char*)test_argv3);
			pt_im_mxic_execute_cmd((sizeof(test_argv4) / D_PT_MXIC_CMD_LEN), (char*)test_argv4);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// DecErr
		else if ( testno == 16 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "dec_err", "set_int", "3", "1", "1", "1" };
			char test_argv2[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "dec_err", "get_int", "3"                };
			char test_argv3[][ D_PT_MXIC_CMD_LEN ] = { "immxic", "dec_err", "get",     "3"                };

			// set/get
			pt_im_mxic_execute_cmd((sizeof(test_argv1) / D_PT_MXIC_CMD_LEN), (char*)test_argv1);
			pt_im_mxic_execute_cmd((sizeof(test_argv2) / D_PT_MXIC_CMD_LEN), (char*)test_argv2);
			pt_im_mxic_execute_cmd((sizeof(test_argv3) / D_PT_MXIC_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		else {
			Ddim_Print(("*** RS Parameter Error\n" ));
		}

		break;
	}

	// タイマーログ出力
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );

	Ddim_Print(( "****** Palladium Test %d end\n", testno ));

	// Force stop
	Palladium_Force_Stop();
}




