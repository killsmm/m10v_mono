/**
 * @file		palladium_test_jpeg.c
 * @brief		palladium test code for im_jpeg.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include "ddim_user_custom.h"
#include "palladium_test_common.h"
#include "palladium_test.h"
#include "dd_gic.h"
#include "dd_top.h"

#include "ct_im_jpeg.h"
#include "im_jpeg.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PT_JPEG_CLKSTOP_REG_ADDR		(&Dd_Top_Get_CLKSTOP3())

#define D_PT_JPEG_CLK_ON				(0x00200000)	// bit[21:22]=10.
#define D_PT_JPEG_HCLK_ON				(0x02000000)	// bit[25:24]=10.
#define D_PT_JPEG_ICLK_ON				(0x08000000)	// bit[27:26]=10.

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
/* Nothing Special */

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
static VOID pt_im_jpeg_enc_enable_gic( VOID )
{
	Dd_GIC_Ctrl( E_DD_GIC_INTID_JPEG_INT, 1, D_DD_GIC_PRI30, 1);		// int enable
}

static VOID pt_im_jpeg_enc_start_clock( VOID )
{
	UCHAR im_jpeg_clk_ctrl_cnt = 0;
	UCHAR im_jpeg_hclk_ctrl_cnt = 0;
	UCHAR im_jpeg_iclk_ctrl_cnt = 0;

	Dd_Top_Start_Clock( &im_jpeg_clk_ctrl_cnt,  D_PT_JPEG_CLKSTOP_REG_ADDR, D_PT_JPEG_CLK_ON );
	Dd_Top_Start_Clock( &im_jpeg_hclk_ctrl_cnt, D_PT_JPEG_CLKSTOP_REG_ADDR, D_PT_JPEG_HCLK_ON );
	Dd_Top_Start_Clock( &im_jpeg_iclk_ctrl_cnt, D_PT_JPEG_CLKSTOP_REG_ADDR, D_PT_JPEG_ICLK_ON );
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
VOID Pt_Im_JPEG_Main( VOID )
{
	UINT32	param1;
	UINT32	param2;
	UINT32	param3;

	// 開始...
	Ddim_Print(( "****** Palladium JPEG Test : Pt_Im_JPEG_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	param1	= gDDIM_Info.com._6a;
	param2	= gDDIM_Info.com._6b;
	param3	= gDDIM_Info.com._6c;

	pt_im_jpeg_enc_enable_gic();
	pt_im_jpeg_enc_start_clock();

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );
	
	// TEST処理実行
	if (param1 == 1) {
		Ct_Im_Jpeg_Run_1(param2, param3);
	}
	else if (param1 == 2) {
		Ct_Im_Jpeg_Run_2(param2, param3);
	}
	else {
		Ddim_Print(("input parameter error\n"));
	}
	
	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );
	
	// タイマーログ出力
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );

	Ddim_Print(( "****** Palladium JPEG Test end\n" ));

	// Force stop
	Palladium_Force_Stop();
}




