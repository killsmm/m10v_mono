/**
 * @file		palladium_test_img.c
 * @brief		IMG test code on palladium
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "im_img.h"
#include "ct_im_img.h"
#include "driver_common.h"
#include "palladium_test.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PT_IMG_CMD_LEN			(20)

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
static char* gCmd_Argv[ D_PT_IMG_CMD_LEN ];

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
static void pt_im_img_execute_cmd( int cmd_argc, char* cmd_argv ) 
{ 
	INT32 j;
	INT32 cmd_pos = 0;

	for( j = 0; j < cmd_argc; j++ ) { 
		gCmd_Argv[ j ] = ( cmd_argv + ( cmd_pos * D_PT_IMG_CMD_LEN ) ); 
		cmd_pos++;
	}

	Ct_Im_IMG_Main( cmd_argc, gCmd_Argv ); 
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		IMG main code
 * @param		void
 * @return		void
 * @note		None
 * @attention	None
 */
void Pt_Im_IMG_Main( void )
{
	UINT32	testno;

	// 開始...
	Ddim_Print(( "****** IMG begin Pt_Im_IMG_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	testno	= gDDIM_Info.com._6a;

	Ddim_Print(( "****** Palladium Test %d begin\n", testno ));

	while( 1 ) {

		// IMG INIT.
		if ( testno == 0 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "2", "0" };

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// MXICMON_VDEN.
		else if ( testno == 1 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "0", "1" };
			char test_argv3[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "0", "0" };

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// enable
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// disable
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// MXICMON_HDEN.
		else if ( testno == 2 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "1", "1" };
			char test_argv3[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "1", "0" };

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// enable
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// disable
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// MXICMON_SEN_ERROR.
		else if ( testno == 3 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "2", "1" };
			char test_argv3[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "2", "0" };

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// enable
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// disable
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// MXICMON_SRO1_ERROR.
		else if ( testno == 4 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "3", "1" };
			char test_argv3[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "3", "0" };

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// enable
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// disable
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// MXICMON_SRO2_ERROR.
		else if ( testno == 5 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "4", "1" };
			char test_argv3[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "4", "0" };

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// enable
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// disable
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// MXICMON_B2B1_ERROR.
		else if ( testno == 6 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "5", "1" };
			char test_argv3[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "5", "0" };

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// enable
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// disable
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// MXICMON_B2B2_ERROR.
		else if ( testno == 7 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "6", "1" };
			char test_argv3[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "6", "0" };

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// enable
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// disable
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// MXICMON_R2Y1_ERROR.
		else if ( testno == 8 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "7", "1" };
			char test_argv3[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "7", "0" };

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// enable
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// disable
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// MXICMON_R2Y2_ERROR.
		else if ( testno == 9 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "8", "1" };
			char test_argv3[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "8", "0" };

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// enable
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// disable
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// MXICMON_STAT_ERROR.
		else if ( testno == 10 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "9", "1" };
			char test_argv3[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "9", "0" };

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// enable
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// disable
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// MXICMON_PAS_ERROR.
		else if ( testno == 11 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "10", "1" };
			char test_argv3[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "10", "0" };

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// enable
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// disable
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// MXICMON_DISP_ERROR.
		else if ( testno == 12 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "11", "1" };
			char test_argv3[][ D_PT_IMG_CMD_LEN ] = { "imimg", "2", "11", "0" };

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// enable
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// disable
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// IMGPIPEPSW.
		else if ( testno == 13 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "3", "0", "0" };		// R2Y input - B2R.
			char test_argv3[][ D_PT_IMG_CMD_LEN ] = { "imimg", "3", "0", "1" };		// R2Y input - LTM.
			char test_argv4[][ D_PT_IMG_CMD_LEN ] = { "imimg", "3", "0", "2" };		// R2Y input - NONE.

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// R2Y-input - B2R.
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// R2Y-input - LTM.
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// R2Y-input - NONE.
			pt_im_img_execute_cmd((sizeof(test_argv4) / D_PT_IMG_CMD_LEN), (char*)test_argv4);


			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// IMGPIPEPSW.
		else if ( testno == 14 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ] = { "imimg", "1", "1", "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ] = { "imimg", "3", "1", "0" };		// B2R connect - NONE.
			char test_argv3[][ D_PT_IMG_CMD_LEN ] = { "imimg", "3", "1", "1" };		// B2R connect - LTM.
			char test_argv4[][ D_PT_IMG_CMD_LEN ] = { "imimg", "3", "1", "2" };		// B2R connect - NONE.

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// B2R connect - NONE.
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// B2R connect - LTM.
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// B2R connect - NONE.
			pt_im_img_execute_cmd((sizeof(test_argv4) / D_PT_IMG_CMD_LEN), (char*)test_argv4);


			// タイマー停止
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
		}
		// SRAMPD.
		else if ( testno == 15 ) {
			// タイマー起動
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			char test_argv1[][ D_PT_IMG_CMD_LEN ]  = { "imimg", "1", "1",   "0" };
			char test_argv2[][ D_PT_IMG_CMD_LEN ]  = { "imimg", "4", "0",   "1" };
			char test_argv3[][ D_PT_IMG_CMD_LEN ]  = { "imimg", "4", "0",   "0" };
			char test_argv4[][ D_PT_IMG_CMD_LEN ]  = { "imimg", "4", "1",   "1" };
			char test_argv5[][ D_PT_IMG_CMD_LEN ]  = { "imimg", "4", "1",   "0" };
			char test_argv6[][ D_PT_IMG_CMD_LEN ]  = { "imimg", "4", "2",   "1" };
			char test_argv7[][ D_PT_IMG_CMD_LEN ]  = { "imimg", "4", "2",   "0" };

			// init
			pt_im_img_execute_cmd((sizeof(test_argv1) / D_PT_IMG_CMD_LEN), (char*)test_argv1);

			// Y0-1 SRAM Power down off
			pt_im_img_execute_cmd((sizeof(test_argv2) / D_PT_IMG_CMD_LEN), (char*)test_argv2);

			// Y0-1 SRAM Power down on
			pt_im_img_execute_cmd((sizeof(test_argv3) / D_PT_IMG_CMD_LEN), (char*)test_argv3);

			// Y2-3 SRAM Power down off
			pt_im_img_execute_cmd((sizeof(test_argv4) / D_PT_IMG_CMD_LEN), (char*)test_argv4);

			// Y2-3 SRAM Power down on
			pt_im_img_execute_cmd((sizeof(test_argv5) / D_PT_IMG_CMD_LEN), (char*)test_argv5);

			// C SRAM Power down off
			pt_im_img_execute_cmd((sizeof(test_argv6) / D_PT_IMG_CMD_LEN), (char*)test_argv6);

			// C SRAM Power down on
			pt_im_img_execute_cmd((sizeof(test_argv7) / D_PT_IMG_CMD_LEN), (char*)test_argv7);

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




