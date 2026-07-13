/**
 * @file		palladium_test_shdr.c
 * @brief		SHDR test code on palladium
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "im_shdr.h"
#include "ct_im_shdr.h"
#include "dd_gic.h"
#include "dd_top.h"
#include "driver_common.h"
#include "palladium_test.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PT_SHDR_CMD_LEN			(20)

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
static char* gCmd_Argv[ D_PT_SHDR_CMD_LEN ];

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
static VOID pt_im_shdr_enable_gic( VOID )
{
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH0_INT, 1, D_DD_GIC_PRI30, 1 );		// cpu=0, intid=118, enable, pri=30
	Dd_GIC_Ctrl( E_DD_GIC_INTID_SHDR_INT,      1, D_DD_GIC_PRI30, 1 );		// int enable
	Dd_GIC_Ctrl( E_DD_GIC_INTID_SHDR_LINE_INT, 1, D_DD_GIC_PRI30, 1 );		// int enable
}


static VOID pt_im_shdr_start_clock( VOID )
{
	UCHAR im_shdr_pclk_ctrl_cnt = 0;
	UCHAR im_shdr_clk_ctrl_cnt = 0;
	UCHAR im_shdr_iclk_ctrl_cnt = 0;
	UCHAR im_shdr_hclk_ctrl_cnt = 0;

	Dd_Top_Start_Clock( (UCHAR*)&im_shdr_pclk_ctrl_cnt, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_SHDRAP_BIT );

	Dd_Top_Start_Clock( (UCHAR*)&im_shdr_iclk_ctrl_cnt, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_SHDRAX_BIT );

	Dd_Top_Start_Clock( (UCHAR*)&im_shdr_clk_ctrl_cnt, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_SHDRCK_BIT );

	Dd_Top_Start_Clock( (UCHAR*)&im_shdr_hclk_ctrl_cnt, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_SHDRAH_BIT );
}

static void pt_im_shdr_execute_cmd( int cmd_argc, char* cmd_argv ) 
{ 
	INT32 j;
	INT32 cmd_pos = 0;

	for( j = 0; j < cmd_argc; j++ ) { 
		gCmd_Argv[ j ] = ( cmd_argv + ( cmd_pos * D_PT_SHDR_CMD_LEN ) ); 
		cmd_pos++;
	}

	Ct_Im_SHDR_Main( cmd_argc, gCmd_Argv ); 
}

static void pt_im_shdr_register_dump_cmd() 
{
	char test_argv1[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "ctrl" };
	char test_argv2[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "pre" };
	char test_argv3[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "fmd" };
	char test_argv4[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "pmsk" };
	char test_argv5[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "spnr" };
	char test_argv6[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "mask" };
	char test_argv7[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "blend" };
	char test_argv8[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "post" };
	char test_argv9[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "resize" };
	char test_argv10[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "smc" };
	char test_argv11[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "dgr" };
	char test_argv12[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "dgg" };
	char test_argv13[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "dgb" };
	char test_argv14[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "axi" };
	char test_argv15[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "get", "axires" };

	pt_im_shdr_execute_cmd((sizeof(test_argv1) / D_PT_SHDR_CMD_LEN), (char*)test_argv1);
	pt_im_shdr_execute_cmd((sizeof(test_argv2) / D_PT_SHDR_CMD_LEN), (char*)test_argv2);
	pt_im_shdr_execute_cmd((sizeof(test_argv3) / D_PT_SHDR_CMD_LEN), (char*)test_argv3);
	pt_im_shdr_execute_cmd((sizeof(test_argv4) / D_PT_SHDR_CMD_LEN), (char*)test_argv4);
	pt_im_shdr_execute_cmd((sizeof(test_argv5) / D_PT_SHDR_CMD_LEN), (char*)test_argv5);
	pt_im_shdr_execute_cmd((sizeof(test_argv6) / D_PT_SHDR_CMD_LEN), (char*)test_argv6);
	pt_im_shdr_execute_cmd((sizeof(test_argv7) / D_PT_SHDR_CMD_LEN), (char*)test_argv7);
	pt_im_shdr_execute_cmd((sizeof(test_argv8) / D_PT_SHDR_CMD_LEN), (char*)test_argv8);
	pt_im_shdr_execute_cmd((sizeof(test_argv9) / D_PT_SHDR_CMD_LEN), (char*)test_argv9);
	pt_im_shdr_execute_cmd((sizeof(test_argv10) / D_PT_SHDR_CMD_LEN), (char*)test_argv10);
	pt_im_shdr_execute_cmd((sizeof(test_argv11) / D_PT_SHDR_CMD_LEN), (char*)test_argv11);
	pt_im_shdr_execute_cmd((sizeof(test_argv12) / D_PT_SHDR_CMD_LEN), (char*)test_argv12);
	pt_im_shdr_execute_cmd((sizeof(test_argv13) / D_PT_SHDR_CMD_LEN), (char*)test_argv13);
	pt_im_shdr_execute_cmd((sizeof(test_argv14) / D_PT_SHDR_CMD_LEN), (char*)test_argv14);
	pt_im_shdr_execute_cmd((sizeof(test_argv15) / D_PT_SHDR_CMD_LEN), (char*)test_argv15);
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		SHDR main code
 * @param		void
 * @return		void
 * @note		None
 * @attention	None
 */
void Pt_Im_SHDR_Main( void )
{
	UINT32	testno;

	// 開始...
	Ddim_Print(( "****** SHDR begin Pt_Im_SHDR_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	testno	= gDDIM_Info.com._6a;

	Ddim_Print(( "****** Palladium Test %d begin\n", testno ));

	pt_im_shdr_enable_gic();
	pt_im_shdr_start_clock();

	while( 1 ) {

		// 3DNR w/o MC.
		if ( testno == 1 ) {
			// 入力データ.
			Palladium_Set_In_Localstack( D_IM_SHDR_IMG_MEM_IN_YCC_REF_ADDR_TOP, D_IM_SHDR_IMG_MEM_4K2K_YCC_REF_U8_BYTES );
			Palladium_Set_In_Localstack( D_IM_SHDR_IMG_MEM_IN_YCC_STD_ADDR_TOP, D_IM_SHDR_IMG_MEM_4K2K_YCC_STD_U8_BYTES );

			// タイマー起動.
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			// テストコード起動.
			char test_argv1[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "2", "1" };
			pt_im_shdr_execute_cmd((sizeof(test_argv1) / D_PT_SHDR_CMD_LEN), (char*)test_argv1);

			// レジスタダンプ.
			pt_im_shdr_register_dump_cmd();

			// タイマー停止.
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

			// 出力ダンプ
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_HEVC_ADDR_TOP,      D_IM_SHDR_IMG_MEM_HEVC_U8_BYTES );
//			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_MEIN_ADDR_TOP,      D_IM_SHDR_IMG_MEM_MEIN_YCC_U8_BYTES );
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_RIB_FHD_ADDR_TOP,   D_IM_SHDR_IMG_MEM_RIB_FHD_U8_BYTES );
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_RIB_HD_ADDR_TOP,    D_IM_SHDR_IMG_MEM_RIB_HD_U8_BYTES );
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_DISP_HDMI_ADDR_TOP, D_IM_SHDR_IMG_MEM_DISP_HDMI_YCC_U8_BYTES );
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_DISP_LCD_ADDR_TOP,  D_IM_SHDR_IMG_MEM_DISP_LCD_YCC_U8_BYTES );
		}
		// Video-HDR with MC.
		else if ( testno == 3 ) {
			// 入力データ.
			Palladium_Set_In_Localstack( D_IM_SHDR_IMG_MEM_IN_YCC_REF_ADDR_TOP, D_IM_SHDR_IMG_MEM_4K2K_YCC_REF_U8_BYTES );
			Palladium_Set_In_Localstack( D_IM_SHDR_IMG_MEM_IN_YCC_STD_ADDR_TOP, D_IM_SHDR_IMG_MEM_4K2K_YCC_STD_U8_BYTES );

			// タイマー起動.
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			// テストコード起動.
			char test_argv1[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "2", "3" };
			pt_im_shdr_execute_cmd((sizeof(test_argv1) / D_PT_SHDR_CMD_LEN), (char*)test_argv1);

			// レジスタダンプ.
			pt_im_shdr_register_dump_cmd();

			// タイマー停止.
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

			// 出力ダンプ
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_HEVC_ADDR_TOP,      D_IM_SHDR_IMG_MEM_HEVC_U8_BYTES );
//			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_MEIN_ADDR_TOP,      D_IM_SHDR_IMG_MEM_MEIN_YCC_U8_BYTES );
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_RIB_FHD_ADDR_TOP,   D_IM_SHDR_IMG_MEM_RIB_FHD_U8_BYTES );
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_RIB_HD_ADDR_TOP,    D_IM_SHDR_IMG_MEM_RIB_HD_U8_BYTES );
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_DISP_HDMI_ADDR_TOP, D_IM_SHDR_IMG_MEM_DISP_HDMI_YCC_U8_BYTES );
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_DISP_LCD_ADDR_TOP,  D_IM_SHDR_IMG_MEM_DISP_LCD_YCC_U8_BYTES );
		}
		// Bypass-YCC.
		else if ( testno == 5 ) {
			// 入力データ.
			Palladium_Set_In_Localstack( D_IM_SHDR_IMG_MEM_IN_YCC_REF_ADDR_TOP, D_IM_SHDR_IMG_MEM_4K2K_YCC_REF_U8_BYTES );

			// タイマー起動.
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

			// テストコード起動.
			char test_argv1[][ D_PT_SHDR_CMD_LEN ] = { "imshdr", "2", "5" };
			pt_im_shdr_execute_cmd((sizeof(test_argv1) / D_PT_SHDR_CMD_LEN), (char*)test_argv1);

			// レジスタダンプ.
			pt_im_shdr_register_dump_cmd();

			// タイマー停止.
			Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

			// 出力ダンプ
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_HEVC_ADDR_TOP,      D_IM_SHDR_IMG_MEM_HEVC_U8_BYTES );
//			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_MEIN_ADDR_TOP,      D_IM_SHDR_IMG_MEM_MEIN_YCC_U8_BYTES );
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_RIB_FHD_ADDR_TOP,   D_IM_SHDR_IMG_MEM_RIB_FHD_U8_BYTES );
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_RIB_HD_ADDR_TOP,    D_IM_SHDR_IMG_MEM_RIB_HD_U8_BYTES );
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_DISP_HDMI_ADDR_TOP, D_IM_SHDR_IMG_MEM_DISP_HDMI_YCC_U8_BYTES );
			Palladium_Set_Out_Localstack( D_IM_SHDR_IMG_MEM_OUT_DISP_LCD_ADDR_TOP,  D_IM_SHDR_IMG_MEM_DISP_LCD_YCC_U8_BYTES );
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




