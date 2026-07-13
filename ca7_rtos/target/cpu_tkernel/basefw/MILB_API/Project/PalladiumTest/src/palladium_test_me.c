/**
 * @file		palladium_test_me.c
 * @brief		ME test code on palladium
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include "im_me.h"
#include "ct_im_me.h"
#include "dd_gic.h"
#include "dd_top.h"
#include "driver_common.h"
#include "palladium_test.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PT_ME_CMD_LEN			(20)

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
static char* gCmd_Argv[ D_PT_ME_CMD_LEN ];

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
static VOID pt_im_me_enable_gic( VOID )
{
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH0_INT, 1, D_DD_GIC_PRI30, 1 );		// cpu=0, intid=118, enable, pri=30
	Dd_GIC_Ctrl( E_DD_GIC_INTID_ME_INT,        1, D_DD_GIC_PRI30, 1 );		// int enable
}


static VOID pt_im_me_start_clock( VOID )
{
	UCHAR im_me_pclk_ctrl_cnt = 0;
	UCHAR im_me_clk_ctrl_cnt = 0;
	UCHAR im_me_iclk_ctrl_cnt = 0;

	Dd_Top_Start_Clock( (UCHAR*)&im_me_pclk_ctrl_cnt, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_MEAP_BIT );

	Dd_Top_Start_Clock( (UCHAR*)&im_me_iclk_ctrl_cnt, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_MEAX_BIT );

	Dd_Top_Start_Clock( (UCHAR*)&im_me_clk_ctrl_cnt, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_MECK_BIT );
}

static void pt_im_me_execute_cmd( int cmd_argc, char* cmd_argv ) 
{ 
	INT32 j;
	INT32 cmd_pos = 0;

	for( j = 0; j < cmd_argc; j++ ) { 
		gCmd_Argv[ j ] = ( cmd_argv + ( cmd_pos * D_PT_ME_CMD_LEN ) ); 
		cmd_pos++;
	}

	Ct_Im_ME_Main( cmd_argc, gCmd_Argv ); 
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		ME main code
 * @param		void
 * @return		void
 * @note		None
 * @attention	None
 */
void Pt_Im_ME_Main( void )
{
	UINT32	testno;

	// 開始...
	Ddim_Print(( "****** ME begin Pt_Im_ME_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	testno	= gDDIM_Info.com._6a;

	Ddim_Print(( "****** Palladium Test %d begin\n", testno ));

	pt_im_me_enable_gic();
	pt_im_me_start_clock();

	while( 1 ) {

		if ( testno == 1 ) {
			// 入力データ.
			Palladium_Set_In_Localstack( D_IM_ME_IMG_MEM_IN_YCC_REF_ADDR_TOP, D_IM_ME_IMG_MEM_4K2K_YCC_REF_U8_BYTES );
			Palladium_Set_In_Localstack( D_IM_ME_IMG_MEM_IN_YCC_STD_ADDR_TOP, D_IM_ME_IMG_MEM_4K2K_YCC_STD_U8_BYTES );

			// タイマー起動.
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			// テストコード起動.
			char test_argv1[][ D_PT_ME_CMD_LEN ] = { "imme", "2", "1" };
			pt_im_me_execute_cmd((sizeof(test_argv1) / D_PT_ME_CMD_LEN), (char*)test_argv1);

			// タイマー停止.
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

			// 出力ダンプ
			Palladium_Set_Out_Localstack( D_IM_ME_IMG_MEM_OUT_HEVC_ADDR_TOP,      D_IM_ME_IMG_MEM_HEVC_U8_BYTES );
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

