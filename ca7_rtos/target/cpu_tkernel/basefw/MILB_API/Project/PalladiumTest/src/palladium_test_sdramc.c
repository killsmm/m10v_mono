/**
 * @file		palladium_test_sdramc.c
 * @brief		SDRAMC test code on palladium
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "im_sdramc.h"
#include "ct_im_sdramc.h"
#include "dd_gic.h"
#include "dd_top.h"
#include "driver_common.h"
#include "palladium_test.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PT_SDRAMC_CMD_LEN			(20)

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
//static char* gCmd_Argv[ D_PT_SDRAMC_CMD_LEN ];

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
static VOID pt_im_sdramc_enable_gic( VOID )
{
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH0_INT,  1, D_DD_GIC_PRI30, 1 );		// cpu=0, intid=118, enable, pri=30
	Dd_GIC_Ctrl( E_DD_GIC_INTID_SDRAMC_CH0_INT, 1, D_DD_GIC_PRI30, 1 );		// int enable
	Dd_GIC_Ctrl( E_DD_GIC_INTID_SDRAMC_CH1_INT, 1, D_DD_GIC_PRI30, 1 );		// int enable
}


static VOID pt_im_sdramc_start_clock( VOID )
{
	UCHAR im_sdramc_pclk_ctrl_cnt[ 2 ] = { 0, 0 };

	Dd_Top_Start_Clock( (UCHAR*)&im_sdramc_pclk_ctrl_cnt[ 0 ], &Dd_Top_Get_CLKSTOP9(), ~D_DD_TOP_UMC0AP_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&im_sdramc_pclk_ctrl_cnt[ 1 ], &Dd_Top_Get_CLKSTOP9(), ~D_DD_TOP_UMC1AP_BIT );
}

#if 0
static void pt_im_sdramc_execute_cmd( int cmd_argc, char* cmd_argv ) 
{ 
	INT32 j;
	INT32 cmd_pos = 0;

	for( j = 0; j < cmd_argc; j++ ) { 
		gCmd_Argv[ j ] = ( cmd_argv + ( cmd_pos * D_PT_SDRAMC_CMD_LEN ) ); 
		cmd_pos++;
	}

	Ct_Im_SDRAMC_Main( cmd_argc, gCmd_Argv ); 
}
#endif

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		SDRAMC main code
 * @param		void
 * @return		void
 * @note		None
 * @attention	None
 */
void Pt_Im_SDRAMC_Main( void )
{
	UINT32	testno;

	// 開始...
	Ddim_Print(( "****** SDRAMC begin Pt_Im_SDRAMC_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	testno	= gDDIM_Info.com._6a;

	Ddim_Print(( "****** Palladium Test %d begin\n", testno ));

	pt_im_sdramc_enable_gic();
	pt_im_sdramc_start_clock();

	while( 1 ) {

		if ( testno == 0 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			CT_Im_SDRAMC_1_01( 0 );

			CT_Im_SDRAMC_1_01( 1 );

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		else if ( testno == 1 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			CT_Im_SDRAMC_1_02( 0 );

			CT_Im_SDRAMC_1_02( 1 );

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		else if ( testno == 2 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			CT_Im_SDRAMC_1_03( 0 );

			CT_Im_SDRAMC_1_03( 1 );

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		else if ( testno == 3 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			CT_Im_SDRAMC_1_04( 0 );

			CT_Im_SDRAMC_1_04( 1 );

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		else if ( testno == 4 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			CT_Im_SDRAMC_1_05( 0 );

			CT_Im_SDRAMC_1_05( 1 );

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Self refresh mode start/stop.
		else if ( testno == 5 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			CT_Im_SDRAMC_1_06( 0 );

			CT_Im_SDRAMC_1_06( 1 );

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// Access detection start/stop/get.
		else if ( testno == 6 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			CT_Im_SDRAMC_1_07( 0 );

			CT_Im_SDRAMC_1_07( 1 );

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		else if ( testno == 7 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			CT_Im_SDRAMC_1_08( 0 );

			CT_Im_SDRAMC_1_08( 1 );

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		else if ( testno == 8 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			CT_Im_SDRAMC_1_09( 0 );

			CT_Im_SDRAMC_1_09( 1 );

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		else if ( testno == 9 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			CT_Im_SDRAMC_1_10( 0 );

			CT_Im_SDRAMC_1_10( 1 );

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		else if ( testno == 10 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			CT_Im_SDRAMC_1_11( 0 );

			CT_Im_SDRAMC_1_11( 1 );

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		else if ( testno == 11 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			CT_Im_SDRAMC_1_12( 0 );

			CT_Im_SDRAMC_1_12( 1 );

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




