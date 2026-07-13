/**
 * @file		palladium_test_hdmi.c
 * @brief		palladium test code for im_hdmi.
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

#include "ct_im_hdmi.h"
#include "im_hdmi.h"
#include "im_disp.h"
#include "jdsdisp3a.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PT_HDMI_CMD_LEN				(10)

#define D_PT_HDMI_CLKSTOP_REG_ADDR		(&Dd_Top_Get_CLKSTOP3())

#define D_PT_HDMI_PCLK_ON				(0x00008000)	// bit[15:14]=10.
#define D_PT_HDMI_HCLK_ON				(0x00020000)	// bit[17:16]=10.
#define D_PT_HDMI_ICLK_ON				(0x00080000)	// bit[19:18]=10.

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
static char* gCmd_Argv[ D_PT_HDMI_CMD_LEN ];

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
static VOID pt_im_hdmi_enable_gic( VOID )
{
	Dd_GIC_Ctrl( E_DD_GIC_INTID_HDMI_MASTER_INT, 1, D_DD_GIC_PRI30, 1);		// int enable
}

static VOID pt_im_hdmi_start_clock( VOID )
{
	UCHAR im_hdmi_iclk_ctrl_cnt = 0;
	UCHAR im_hdmi_pclk_ctrl_cnt = 0;
	UCHAR im_hdmi_hclk_ctrl_cnt = 0;

	Dd_Top_Start_Clock( &im_hdmi_iclk_ctrl_cnt, D_PT_HDMI_CLKSTOP_REG_ADDR, D_PT_HDMI_ICLK_ON );
	Dd_Top_Start_Clock( &im_hdmi_pclk_ctrl_cnt, D_PT_HDMI_CLKSTOP_REG_ADDR, D_PT_HDMI_PCLK_ON );
	Dd_Top_Start_Clock( &im_hdmi_hclk_ctrl_cnt, D_PT_HDMI_CLKSTOP_REG_ADDR, D_PT_HDMI_HCLK_ON );
}

static void pt_im_hdmi_execute_cmd( int cmd_argc, char* cmd_argv ) 
{ 
	INT32 j;
	INT32 cmd_pos = 0;

	for( j = 0; j < cmd_argc; j++ ) { 
		gCmd_Argv[ j ] = ( cmd_argv + ( cmd_pos * D_PT_HDMI_CMD_LEN ) ); 
		cmd_pos++;
	}

	Ct_Im_Hdmi_Main( cmd_argc, gCmd_Argv ); 
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		Test code
 * @param		void
 * @return		void
 * @note		None
 * @attention	None
 */
VOID Pt_Im_Hdmi_Main( VOID )
{
	UINT32	testno;

	// 開始...
	Ddim_Print(( "****** HDMI begin Pt_Im_Hdmi_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	testno	= gDDIM_Info.com._6a;

	Ddim_Print(( "****** Palladium Test %d begin\n", testno ));

	pt_im_hdmi_enable_gic();
	pt_im_hdmi_start_clock();
	
	while( 1 ) {

		// Im_HDMI_Init.
		if ( testno == 0 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "1" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "2" };

			// test 1-1-1.
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);
			// test 1-1-2.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-3.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_Read_Sinks_E_EDID.
		else if ( testno == 1 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "sinks", "0", "0" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "sinks", "1" };
			char test_argv4[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "sinks", "2" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-4.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-5.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);
			// test 1-1-6.
			pt_im_hdmi_execute_cmd((sizeof(test_argv4) / D_PT_HDMI_CMD_LEN), (char*)test_argv4);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_Ctrl, Im_HDMI_Get_Ctrl.
		else if ( testno == 2 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "ctrl", "set", "0", "0" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "ctrl", "set", "1" };
			char test_argv4[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "ctrl", "set", "2", "0" };
			char test_argv5[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "ctrl", "set", "0", "2" };
			char test_argv6[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "ctrl", "get", "0" };
			char test_argv7[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "ctrl", "get", "1" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-7.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-8.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);
			// test 1-1-9.
			pt_im_hdmi_execute_cmd((sizeof(test_argv4) / D_PT_HDMI_CMD_LEN), (char*)test_argv4);
			// test 1-1-10.
			pt_im_hdmi_execute_cmd((sizeof(test_argv5) / D_PT_HDMI_CMD_LEN), (char*)test_argv5);
			// test 1-1-11.
			pt_im_hdmi_execute_cmd((sizeof(test_argv6) / D_PT_HDMI_CMD_LEN), (char*)test_argv6);
			// test 1-1-12.
			pt_im_hdmi_execute_cmd((sizeof(test_argv7) / D_PT_HDMI_CMD_LEN), (char*)test_argv7);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_Set_Clock, Im_HDMI_Get_Clock.
		else if ( testno == 3 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "clk", "set", "0" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "clk", "get", "0" };
			char test_argv4[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "clk", "get", "1" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-13.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-14.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);
			// test 1-1-15.
			pt_im_hdmi_execute_cmd((sizeof(test_argv4) / D_PT_HDMI_CMD_LEN), (char*)test_argv4);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_Set_Tx_Term, Im_HDMI_Get_Tx_Term.
		else if ( testno == 4 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "term", "set", "0" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "term", "get", "0" };
			char test_argv4[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "term", "get", "1" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-16.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-17.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);
			// test 1-1-18.
			pt_im_hdmi_execute_cmd((sizeof(test_argv4) / D_PT_HDMI_CMD_LEN), (char*)test_argv4);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_Set_Transmitter_Control, Im_HDMI_Get_Transmitter_Control.
		else if ( testno == 5 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "tc", "set", "0" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "tc", "get", "0" };
			char test_argv4[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "tc", "get", "1" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-19.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-20.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);
			// test 1-1-21.
			pt_im_hdmi_execute_cmd((sizeof(test_argv4) / D_PT_HDMI_CMD_LEN), (char*)test_argv4);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_Set_Voltage_Level, Im_HDMI_Get_Voltage_Level.
		else if ( testno == 6 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "vol", "set", "0" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "vol", "get", "0" };
			char test_argv4[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "vol", "get", "1" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-22.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-23.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);
			// test 1-1-24.
			pt_im_hdmi_execute_cmd((sizeof(test_argv4) / D_PT_HDMI_CMD_LEN), (char*)test_argv4);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_CSC.
		else if ( testno == 7 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "csc", "set", "0", "0" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "csc", "set", "1" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-25.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-26.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_Set_Video_Sampler, Im_HDMI_Get_Video_Sampler.
		else if ( testno == 8 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "vs", "set", "0" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "vs", "get", "0" };
			char test_argv4[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "vs", "get", "1" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-27.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-28.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);
			// test 1-1-29.
			pt_im_hdmi_execute_cmd((sizeof(test_argv4) / D_PT_HDMI_CMD_LEN), (char*)test_argv4);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_Set_Video_Packetizer, Im_HDMI_Set_Video_Packetizer.
		else if ( testno == 9 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "vp", "set", "0", "0" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "vp", "set", "1" };
			char test_argv4[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "vp", "get", "0" };
			char test_argv5[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "vp", "get", "1" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-30.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-31.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);
			// test 1-1-32.
			pt_im_hdmi_execute_cmd((sizeof(test_argv4) / D_PT_HDMI_CMD_LEN), (char*)test_argv4);
			// test 1-1-33.
			pt_im_hdmi_execute_cmd((sizeof(test_argv5) / D_PT_HDMI_CMD_LEN), (char*)test_argv5);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_Set_Frame_Composer, Im_HDMI_Get_Frame_Composer.
		else if ( testno == 10 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "fc", "set", "0", "0" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "fc", "set", "1" };
			char test_argv4[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "fc", "get", "0" };
			char test_argv5[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "fc", "get", "1" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-34.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-35.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);
			// test 1-1-36.
			pt_im_hdmi_execute_cmd((sizeof(test_argv4) / D_PT_HDMI_CMD_LEN), (char*)test_argv4);
			// test 1-1-37.
			pt_im_hdmi_execute_cmd((sizeof(test_argv5) / D_PT_HDMI_CMD_LEN), (char*)test_argv5);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_Set_Main_Controller, Im_HDMI_Get_Main_Controller.
		else if ( testno == 11 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "mc", "set", "0", "0" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "mc", "set", "1" };
			char test_argv4[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "mc", "get", "0" };
			char test_argv5[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "mc", "get", "1" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-38.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-39.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);
			// test 1-1-40.
			pt_im_hdmi_execute_cmd((sizeof(test_argv4) / D_PT_HDMI_CMD_LEN), (char*)test_argv4);
			// test 1-1-41.
			pt_im_hdmi_execute_cmd((sizeof(test_argv5) / D_PT_HDMI_CMD_LEN), (char*)test_argv5);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_Set_Audio, Im_HDMI_Get_Audio.
		else if ( testno == 12 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "mc", "set", "0", "0" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "mc", "set", "1" };
			char test_argv4[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "mc", "get", "0" };
			char test_argv5[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "mc", "get", "1" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-42.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-43.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);
			// test 1-1-44.
			pt_im_hdmi_execute_cmd((sizeof(test_argv4) / D_PT_HDMI_CMD_LEN), (char*)test_argv4);
			// test 1-1-45.
			pt_im_hdmi_execute_cmd((sizeof(test_argv5) / D_PT_HDMI_CMD_LEN), (char*)test_argv5);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_Set_Int_Callback.
		else if ( testno == 13 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "cb", "set", "1" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "cb", "set", "0" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-46.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-47.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_Set_Int_Mute, Im_HDMI_Get_Int_Mute.
		else if ( testno == 14 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "mute", "set", "0" };
			char test_argv3[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "mute", "get", "0" };
			char test_argv4[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "mute", "get", "1" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-48.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);
			// test 1-1-49.
			pt_im_hdmi_execute_cmd((sizeof(test_argv3) / D_PT_HDMI_CMD_LEN), (char*)test_argv3);
			// test 1-1-50.
			pt_im_hdmi_execute_cmd((sizeof(test_argv4) / D_PT_HDMI_CMD_LEN), (char*)test_argv4);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_HDMI_Int_Handler.
		else if ( testno == 15 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "init", "0", "0" };
			char test_argv2[][ D_PT_HDMI_CMD_LEN ] = { "imhdmi", "int", "0" };

			// Im_HDMI_Init
			pt_im_hdmi_execute_cmd((sizeof(test_argv1) / D_PT_HDMI_CMD_LEN), (char*)test_argv1);

			// test 1-1-51.
			pt_im_hdmi_execute_cmd((sizeof(test_argv2) / D_PT_HDMI_CMD_LEN), (char*)test_argv2);

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

	Ddim_Print(( "****** Palladium HDMI Test end\n" ));

	// Force stop
	Palladium_Force_Stop();
}




