/**
 * @file		palladium_test_mipi.c
 * @brief		palladium test code for im_mipi.
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

#include "ct_im_mipi.h"
#include "im_mipi.h"
#include "im_disp.h"
#include "jdsdisp3a.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PT_MIPI_CMD_LEN				(10)

#define D_PT_MIPI_CLKSTOP_REG_ADDR		(&Dd_Top_Get_CLKSTOP3())

#define D_PT_MIPI_PCLK_ON				(0x00008000)	// bit[15:14]=10.
#define D_PT_MIPI_HCLK_ON				(0x00020000)	// bit[17:16]=10.
#define D_PT_MIPI_ICLK_ON				(0x00080000)	// bit[19:18]=10.

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
static char* gCmd_Argv[ D_PT_MIPI_CMD_LEN ];

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
static VOID pt_im_mipi_enable_gic( VOID )
{
	Dd_GIC_Ctrl( E_DD_GIC_INTID_MIPI_DSI_Tx_INT, 1, D_DD_GIC_PRI30, 1);		// int enable
}

static VOID pt_im_mipi_start_clock( VOID )
{
	UCHAR im_mipi_iclk_ctrl_cnt = 0;
	UCHAR im_mipi_pclk_ctrl_cnt = 0;
	UCHAR im_mipi_hclk_ctrl_cnt = 0;

	Dd_Top_Start_Clock( &im_mipi_iclk_ctrl_cnt, D_PT_MIPI_CLKSTOP_REG_ADDR, D_PT_MIPI_ICLK_ON );
	Dd_Top_Start_Clock( &im_mipi_pclk_ctrl_cnt, D_PT_MIPI_CLKSTOP_REG_ADDR, D_PT_MIPI_PCLK_ON );
	Dd_Top_Start_Clock( &im_mipi_hclk_ctrl_cnt, D_PT_MIPI_CLKSTOP_REG_ADDR, D_PT_MIPI_HCLK_ON );
}

static void pt_im_mipi_execute_cmd( int cmd_argc, char* cmd_argv ) 
{ 
	INT32 j;
	INT32 cmd_pos = 0;

	for( j = 0; j < cmd_argc; j++ ) { 
		gCmd_Argv[ j ] = ( cmd_argv + ( cmd_pos * D_PT_MIPI_CMD_LEN ) ); 
		cmd_pos++;
	}

	Ct_Im_Mipi_Main( cmd_argc, gCmd_Argv ); 
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
VOID Pt_Im_Mipi_Main( VOID )
{
	UINT32	testno;

	// 開始...
	Ddim_Print(( "****** MIPI begin Pt_Im_Mipi_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	testno	= gDDIM_Info.com._6a;

	Ddim_Print(( "****** Palladium Test %d begin\n", testno ));

	pt_im_mipi_enable_gic();
	pt_im_mipi_start_clock();
	
	while( 1 ) {

		// Im_MIPI_Init.
		if ( testno == 0 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "init", "0" };

			// test 1-1-1.
			pt_im_mipi_execute_cmd((sizeof(test_argv1) / D_PT_MIPI_CMD_LEN), (char*)test_argv1);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_MIPI_Power_Ctrl.
		else if ( testno == 1 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "pc", "1" };

			// test 1-1-2.
			pt_im_mipi_execute_cmd((sizeof(test_argv1) / D_PT_MIPI_CMD_LEN), (char*)test_argv1);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_MIPI_Enter_Ulps, Im_MIPI_Exit_Ulps.
		else if ( testno == 2 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "init", "0" };
			char test_argv2[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "ulps", "enter", "0", "0" };
			char test_argv3[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "ulps", "enter", "1" };
			char test_argv4[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "ulps", "enter", "2" };
			char test_argv5[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "ulps", "exit", "0", "0" };
			char test_argv6[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "ulps", "exit", "1" };

			// Im_MIPI_Init
			pt_im_mipi_execute_cmd((sizeof(test_argv1) / D_PT_MIPI_CMD_LEN), (char*)test_argv1);

			// test 1-1-3.
			pt_im_mipi_execute_cmd((sizeof(test_argv2) / D_PT_MIPI_CMD_LEN), (char*)test_argv2);
			// test 1-1-4.
			pt_im_mipi_execute_cmd((sizeof(test_argv3) / D_PT_MIPI_CMD_LEN), (char*)test_argv3);
			pt_im_mipi_execute_cmd((sizeof(test_argv4) / D_PT_MIPI_CMD_LEN), (char*)test_argv4);
			// test 1-1-5.
			pt_im_mipi_execute_cmd((sizeof(test_argv5) / D_PT_MIPI_CMD_LEN), (char*)test_argv5);
			// test 1-1-6.
			pt_im_mipi_execute_cmd((sizeof(test_argv6) / D_PT_MIPI_CMD_LEN), (char*)test_argv6);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_MIPI_Set_Lane_Info, Im_MIPI_Get_Lane_Info.
		else if ( testno == 3 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "init", "0" };
			char test_argv2[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "lane", "set", "0", "0" };
			char test_argv3[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "lane", "set", "1" };
			char test_argv4[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "lane", "get", "0", "0" };
			char test_argv5[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "lane", "get", "1" };

			// Im_MIPI_Init
			pt_im_mipi_execute_cmd((sizeof(test_argv1) / D_PT_MIPI_CMD_LEN), (char*)test_argv1);

			// test 1-1-7.
			pt_im_mipi_execute_cmd((sizeof(test_argv2) / D_PT_MIPI_CMD_LEN), (char*)test_argv2);
			// test 1-1-8.
			pt_im_mipi_execute_cmd((sizeof(test_argv3) / D_PT_MIPI_CMD_LEN), (char*)test_argv3);
			// test 1-1-9.
			pt_im_mipi_execute_cmd((sizeof(test_argv4) / D_PT_MIPI_CMD_LEN), (char*)test_argv4);
			// test 1-1-10.
			pt_im_mipi_execute_cmd((sizeof(test_argv5) / D_PT_MIPI_CMD_LEN), (char*)test_argv5);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_MIPI_Set_DPI_Resolution, Im_MIPI_Get_DPI_Resolution / Im_MIPI_Update_Dpi.
		else if ( testno == 4 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "init", "0" };
			char test_argv2[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "dpi", "set", "0", "0" };
			char test_argv3[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "dpi", "set", "1" };
			char test_argv4[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "dpi", "get", "0", "0" };
			char test_argv5[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "dpi", "get", "1" };
			char test_argv6[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "dpi", "update", "0" };
			char test_argv7[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "dpi", "update", "1" };
			char test_argv8[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "dpi", "update", "2" };

			// Im_MIPI_Init
			pt_im_mipi_execute_cmd((sizeof(test_argv1) / D_PT_MIPI_CMD_LEN), (char*)test_argv1);

			// test 1-1-11.
			pt_im_mipi_execute_cmd((sizeof(test_argv2) / D_PT_MIPI_CMD_LEN), (char*)test_argv2);
			// test 1-1-12.
			pt_im_mipi_execute_cmd((sizeof(test_argv3) / D_PT_MIPI_CMD_LEN), (char*)test_argv3);
			// test 1-1-13.
			pt_im_mipi_execute_cmd((sizeof(test_argv4) / D_PT_MIPI_CMD_LEN), (char*)test_argv4);
			// test 1-1-14.
			pt_im_mipi_execute_cmd((sizeof(test_argv5) / D_PT_MIPI_CMD_LEN), (char*)test_argv5);
			// test 1-1-15.
			pt_im_mipi_execute_cmd((sizeof(test_argv6) / D_PT_MIPI_CMD_LEN), (char*)test_argv6);
			// test 1-1-16.
			pt_im_mipi_execute_cmd((sizeof(test_argv7) / D_PT_MIPI_CMD_LEN), (char*)test_argv7);
			pt_im_mipi_execute_cmd((sizeof(test_argv8) / D_PT_MIPI_CMD_LEN), (char*)test_argv8);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_MIPI_Set_Transmission_Configuration, Im_MIPI_Get_Transmission_Configuration.
		else if ( testno == 5 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "init", "0" };
			char test_argv2[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "tc", "set", "0", "0" };
			char test_argv3[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "tc", "set", "1" };
			char test_argv4[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "tc", "get", "0", "0" };
			char test_argv5[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "tc", "get", "1" };

			// Im_MIPI_Init
			pt_im_mipi_execute_cmd((sizeof(test_argv1) / D_PT_MIPI_CMD_LEN), (char*)test_argv1);

			// test 1-1-17.
			pt_im_mipi_execute_cmd((sizeof(test_argv2) / D_PT_MIPI_CMD_LEN), (char*)test_argv2);
			// test 1-1-18.
			pt_im_mipi_execute_cmd((sizeof(test_argv3) / D_PT_MIPI_CMD_LEN), (char*)test_argv3);
			// test 1-1-19.
			pt_im_mipi_execute_cmd((sizeof(test_argv4) / D_PT_MIPI_CMD_LEN), (char*)test_argv4);
			// test 1-1-20.
			pt_im_mipi_execute_cmd((sizeof(test_argv5) / D_PT_MIPI_CMD_LEN), (char*)test_argv5);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_MIPI_Packet_Transmission, Im_MIPI_Packet_Transmission.
		else if ( testno == 6 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "init", "0" };
			char test_argv2[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "pt", "0", "4" };
			char test_argv3[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "pt", "1" };

			// Im_MIPI_Init
			pt_im_mipi_execute_cmd((sizeof(test_argv1) / D_PT_MIPI_CMD_LEN), (char*)test_argv1);

			// test 1-1-21.
			pt_im_mipi_execute_cmd((sizeof(test_argv2) / D_PT_MIPI_CMD_LEN), (char*)test_argv2);
			// test 1-1-22.
			pt_im_mipi_execute_cmd((sizeof(test_argv3) / D_PT_MIPI_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_MIPI_Set_Timer_Count.
		else if ( testno == 7 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "init", "0" };
			char test_argv2[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "timer", "set", "0" };

			// Im_MIPI_Init
			pt_im_mipi_execute_cmd((sizeof(test_argv1) / D_PT_MIPI_CMD_LEN), (char*)test_argv1);

			// test 1-1-23.
			pt_im_mipi_execute_cmd((sizeof(test_argv2) / D_PT_MIPI_CMD_LEN), (char*)test_argv2);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_MIPI_Set_Int_Mask, Im_MIPI_Get_Int_Mask.
		else if ( testno == 8 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "init", "0" };
			char test_argv2[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "mask", "set", "0" };
			char test_argv3[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "mask", "get", "0" };
			char test_argv4[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "mask", "get", "1" };

			// Im_MIPI_Init
			pt_im_mipi_execute_cmd((sizeof(test_argv1) / D_PT_MIPI_CMD_LEN), (char*)test_argv1);

			// test 1-1-24.
			pt_im_mipi_execute_cmd((sizeof(test_argv2) / D_PT_MIPI_CMD_LEN), (char*)test_argv2);
			// test 1-1-25.
			pt_im_mipi_execute_cmd((sizeof(test_argv3) / D_PT_MIPI_CMD_LEN), (char*)test_argv3);
			// test 1-1-26.
			pt_im_mipi_execute_cmd((sizeof(test_argv4) / D_PT_MIPI_CMD_LEN), (char*)test_argv4);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_MIPI_Set_Int_Callback.
		else if ( testno == 9 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "init", "0" };
			char test_argv2[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "cb", "set", "0", "1" };
			char test_argv3[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "cb", "set", "5", "0" };

			// Im_MIPI_Init
			pt_im_mipi_execute_cmd((sizeof(test_argv1) / D_PT_MIPI_CMD_LEN), (char*)test_argv1);

			// test 1-1-27.
			pt_im_mipi_execute_cmd((sizeof(test_argv2) / D_PT_MIPI_CMD_LEN), (char*)test_argv2);
			// test 1-1-28.
			pt_im_mipi_execute_cmd((sizeof(test_argv3) / D_PT_MIPI_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Im_MIPI_Int_Handler.
		else if ( testno == 10 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "init", "0" };
			char test_argv2[][ D_PT_MIPI_CMD_LEN ] = { "immipi", "int", "0" };

			// Im_MIPI_Init
			pt_im_mipi_execute_cmd((sizeof(test_argv1) / D_PT_MIPI_CMD_LEN), (char*)test_argv1);

			// test 1-1-29.
			pt_im_mipi_execute_cmd((sizeof(test_argv2) / D_PT_MIPI_CMD_LEN), (char*)test_argv2);

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

	Ddim_Print(( "****** Palladium MIPI Test end\n" ));

	// Force stop
	Palladium_Force_Stop();
}




