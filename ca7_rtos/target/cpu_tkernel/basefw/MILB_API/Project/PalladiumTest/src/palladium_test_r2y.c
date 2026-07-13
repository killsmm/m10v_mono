/**
 * @file		palladium_test_r2y.c
 * @brief		palladium test r2y code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ct_im_r2y.h"
#include "im_r2y.h"
#include "dd_gic.h"
#include "dd_top.h"

#include "driver_common.h"
#include "palladium_test.h"

#include <string.h>

#define D_IM_R2Y1_CLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP11())
#define D_IM_R2Y1_ICLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP11())
#define D_IM_R2Y1_HCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP11())
#define D_IM_R2Y1_PCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP11())

#define D_IM_R2Y1_CLK_REG_BIT	D_DD_TOP_R2Y1CK_BIT	// bit8~9
#define D_IM_R2Y1_ICLK_REG_BIT	D_DD_TOP_R2Y1AX_BIT	// bit10~11
#define D_IM_R2Y1_HCLK_REG_BIT	D_DD_TOP_R2Y1AH_BIT	// bit12~13
#define D_IM_R2Y1_PCLK_REG_BIT	D_DD_TOP_R2Y1AP_BIT	// bit14~15

#define D_IM_R2Y2_CLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP13())
#define D_IM_R2Y2_ICLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP13())
#define D_IM_R2Y2_HCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP13())
#define D_IM_R2Y2_PCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP13())

#define D_IM_R2Y2_CLK_REG_BIT	D_DD_TOP_R2Y2CK_BIT	// bit8~9
#define D_IM_R2Y2_ICLK_REG_BIT	D_DD_TOP_R2Y2AX_BIT	// bit10~11
#define D_IM_R2Y2_HCLK_REG_BIT	D_DD_TOP_R2Y2AH_BIT	// bit12~13
#define D_IM_R2Y2_PCLK_REG_BIT	D_DD_TOP_R2Y2AP_BIT	// bit14~15

static VOID pt_im_r2y_enable_gic( VOID )
{
//	Dd_GIC_Init();
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH0_INT, 1, D_DD_GIC_PRI30, 1 );		// cpu=0, intid=118, enable, pri=30
	Dd_GIC_Ctrl( E_DD_GIC_INTID_R2Y_UNIT1_INT, 1, D_DD_GIC_PRI30, 1 );		// int enable
	Dd_GIC_Ctrl( E_DD_GIC_INTID_R2Y_UNIT2_INT, 1, D_DD_GIC_PRI30, 1 );		// int enable

	Dd_GIC_Ctrl( E_DD_GIC_INTID_B2R_UNIT1_INT, 1, D_DD_GIC_PRI30, 1 );		// cpu=0, intid=233, enable, pri=30 B2R_UNIT1_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_B2R_UNIT2_INT, 1, D_DD_GIC_PRI30, 1 );		// cpu=0, intid=234, enable, pri=30 B2R_UNIT2_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_LTM_UNIT1_INT, 1, D_DD_GIC_PRI30, 1 );		// cpu=0, intid=235, enable, pri=30 LTM_UNIT1_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_LTM_UNIT2_INT, 1, D_DD_GIC_PRI30, 1 );		// cpu=0, intid=236, enable, pri=30 LTM_UNIT2_INT
}


static VOID pt_im_r2y_start_clock( VOID )
{
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_PCLOCK) || defined(CO_ACT_HCLOCK)
#else
	UCHAR im_r2y1_clk_ctrl_cnt = 0;
	UCHAR im_r2y1_hclk_ctrl_cnt = 0;
	UCHAR im_r2y1_pclk_ctrl_cnt = 0;
	UCHAR im_r2y1_iclk_ctrl_cnt = 0;
	UCHAR im_r2y2_clk_ctrl_cnt = 0;
	UCHAR im_r2y2_hclk_ctrl_cnt = 0;
	UCHAR im_r2y2_pclk_ctrl_cnt = 0;
	UCHAR im_r2y2_iclk_ctrl_cnt = 0;

	Dd_Top_Start_Clock( &im_r2y1_pclk_ctrl_cnt, D_IM_R2Y1_PCLK_REG_ADDR, ~D_IM_R2Y1_PCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_r2y1_hclk_ctrl_cnt, D_IM_R2Y1_HCLK_REG_ADDR, ~D_IM_R2Y1_HCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_r2y1_iclk_ctrl_cnt, D_IM_R2Y1_ICLK_REG_ADDR, ~D_IM_R2Y1_ICLK_REG_BIT );
	Dd_Top_Start_Clock( &im_r2y1_clk_ctrl_cnt,  D_IM_R2Y1_CLK_REG_ADDR,  ~D_IM_R2Y1_CLK_REG_BIT );

	Dd_Top_Start_Clock( &im_r2y2_pclk_ctrl_cnt, D_IM_R2Y2_PCLK_REG_ADDR, ~D_IM_R2Y2_PCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_r2y2_hclk_ctrl_cnt, D_IM_R2Y2_HCLK_REG_ADDR, ~D_IM_R2Y2_HCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_r2y2_iclk_ctrl_cnt, D_IM_R2Y2_ICLK_REG_ADDR, ~D_IM_R2Y2_ICLK_REG_BIT );
	Dd_Top_Start_Clock( &im_r2y2_clk_ctrl_cnt,  D_IM_R2Y2_CLK_REG_ADDR,  ~D_IM_R2Y2_CLK_REG_BIT );
#endif
}


#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "1_1"
static VOID pt_im_r2y_do_test_1_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		1, 14, 17, 18, 19, 20, 21, 22, 23, 25, 27,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* R2Y begin CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_R2Y_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* R2Y end CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "1_28"
static VOID pt_im_r2y_do_test_1_28( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		28, 29, 30, 32, 33, 34, 36, 37, 38, 39, 41,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* R2Y begin CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_R2Y_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* R2Y end CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "1_42"
static VOID pt_im_r2y_do_test_1_42( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		42, 43, 44, 45,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* R2Y begin CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_R2Y_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* R2Y end CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "1_46"
static VOID pt_im_r2y_do_test_1_46( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		46, 47, 48, 49, 50, 51, 52,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* R2Y begin CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_R2Y_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* R2Y end CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "1_53"
static VOID pt_im_r2y_do_test_1_53( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		53, 55, 56, 57, 58, 59, 61, 62, 63, 65,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* R2Y begin CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_R2Y_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* R2Y end CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "1_54"
static VOID pt_im_r2y_do_test_1_54( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "=-=-=-=-\n" ));
	Ddim_Print(( "* R2Y begin CT_Im_R2y_%u_%u\n", ct_idx_1st, 54 ));
	CT_Im_R2Y_Run( ct_idx_1st, 54, 0 );
	Ddim_Print(( "* R2Y end CT_Im_R2y_%u_%u\n", ct_idx_1st, 54 ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "1_66"
static VOID pt_im_r2y_do_test_1_66( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* R2Y begin CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_R2Y_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* R2Y end CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "1_77"
static VOID pt_im_r2y_do_test_1_77( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		77, 78, 80, 81, 82, 83, 84, 85, 86, 87, 88,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* R2Y begin CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_R2Y_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* R2Y end CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "1_89"
static VOID pt_im_r2y_do_test_1_89( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		89, 90, 93, 94, 95, 96, 97, 98, 99, 100, 133,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* R2Y begin CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_R2Y_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* R2Y end CT_Im_R2y_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


static VOID pt_im_r2y_run_1( const UINT32 type1 )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6b;

	switch( type2 ) {
		case 1:
			pt_im_r2y_do_test_1_1( type1, type2 );
			break;
		case 28:
			pt_im_r2y_do_test_1_28( type1, type2 );
			break;
		case 42:
			pt_im_r2y_do_test_1_42( type1, type2 );
			break;
		case 46:
			pt_im_r2y_do_test_1_46( type1, type2 );
			break;
		case 53:
			pt_im_r2y_do_test_1_53( type1, type2 );
			break;
		case 54:
			pt_im_r2y_do_test_1_54( type1, type2 );
			break;
		case 66:
			pt_im_r2y_do_test_1_66( type1, type2 );
			break;
		case 77:
			pt_im_r2y_do_test_1_77( type1, type2 );
			break;
		case 89:
			pt_im_r2y_do_test_1_89( type1, type2 );
			break;
		default:
			Ddim_Print(("*** R2Y test skip\n" ));
			break;
	}
}



#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "2_1"
static VOID pt_im_r2y_do_test_2_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, 0 );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


static VOID pt_im_r2y_run_2( const UINT32 type1 )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6b;

	switch( type2 ) {
		case 1:
			pt_im_r2y_do_test_2_1( type1, type2 );
			break;
		default:
			Ddim_Print(("*** R2Y test skip\n" ));
			break;
	}
}



// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_1"
static VOID pt_im_r2y_do_test_3_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB P12(for B2R 16bit non-packed raw),  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_2"
static VOID pt_im_r2y_do_test_3_2( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_BAY_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_BAY_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT2_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB P12(for LTM 16bit non-packed raw),  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_3"
static VOID pt_im_r2y_do_test_3_3( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_BAY_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_BAY_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT2_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_4"
static VOID pt_im_r2y_do_test_3_4( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_5"
static VOID pt_im_r2y_do_test_3_5( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_6"
static VOID pt_im_r2y_do_test_3_6( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_12"
static VOID pt_im_r2y_do_test_3_12( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_13"
static VOID pt_im_r2y_do_test_3_13( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_14"
static VOID pt_im_r2y_do_test_3_14( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_15"
static VOID pt_im_r2y_do_test_3_15( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: Y only U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_16"
static VOID pt_im_r2y_do_test_3_16( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_17"
static VOID pt_im_r2y_do_test_3_17( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', (1280 * 960 * 2 * 3) );
	memset( (VOID*)( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP + (1280 * 960 * 2 * 3)), '\xee', (1280 * 960 * 2) + ( D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 256 ));
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, (1280 * 960 * 2 * 3) );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack(( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP + (1280 * 960 * 2 * 3)), D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES * 4 );
	Palladium_Set_Out_Localstack(( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP + (1280 * 960 * 2 * 3) + (1280 * 960 * 2)), D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 256 );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: Y only U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_18"
static VOID pt_im_r2y_do_test_3_18( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES + D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC422_Y_ADDR_TOP_1, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U8,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_19"
static VOID pt_im_r2y_do_test_3_19( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U8,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_20"
static VOID pt_im_r2y_do_test_3_20( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U8,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_21"
static VOID pt_im_r2y_do_test_3_21( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U8,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_22"
static VOID pt_im_r2y_do_test_3_22( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U8,  Output: Y only U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_23"
static VOID pt_im_r2y_do_test_3_23( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U8,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_24"
static VOID pt_im_r2y_do_test_3_24( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', (1280 * 960 * 2) + ( D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 256 ));
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES * 4 );
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP + (1280 * 960 * 2), D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 256 );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U8,  Output: Y only U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_25"
static VOID pt_im_r2y_do_test_3_25( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES + D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC422_Y_ADDR_TOP_1, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB P12,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_26"
static VOID pt_im_r2y_do_test_3_26( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB P12,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_27"
static VOID pt_im_r2y_do_test_3_27( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB P12,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_28"
static VOID pt_im_r2y_do_test_3_28( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB P12,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_29"
static VOID pt_im_r2y_do_test_3_29( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB P12,  Output: Y only U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_30"
static VOID pt_im_r2y_do_test_3_30( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB P12,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_31"
static VOID pt_im_r2y_do_test_3_31( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', (1280 * 960 * 3 / 2 * 3) );
	memset( (VOID*)(D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP + (1280 * 960 * 3 / 2 * 3)), '\xee', (1280 * 960 * 2) + ( D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 256 ));
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, (1280 * 960 * 3 / 2 * 3));

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack(( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP + (1280 * 960 * 3 / 2 * 3)), D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES * 4 );
	Palladium_Set_Out_Localstack(( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP + (1280 * 960 * 3 / 2 * 3) + (1280 * 960 * 2)), D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 256 );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB P12,  Output: Y only U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_32"
static VOID pt_im_r2y_do_test_3_32( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES + D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC422_Y_ADDR_TOP_1, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB P12,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_33"
static VOID pt_im_r2y_do_test_3_33( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_BAY_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_BAY_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB P12,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_34"
static VOID pt_im_r2y_do_test_3_34( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_BAY_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_BAY_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB P12,  Output: Y only U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_35"
static VOID pt_im_r2y_do_test_3_35( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_BAY_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_BAY_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB P12,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_36"
static VOID pt_im_r2y_do_test_3_36( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_BAY_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_BAY_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP + D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U8,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_41"
static VOID pt_im_r2y_do_test_3_41( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_42"
static VOID pt_im_r2y_do_test_3_42( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_43"
static VOID pt_im_r2y_do_test_3_43( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_44"
static VOID pt_im_r2y_do_test_3_44( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_45"
static VOID pt_im_r2y_do_test_3_45( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: Y only U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_46"
static VOID pt_im_r2y_do_test_3_46( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U8,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_47"
static VOID pt_im_r2y_do_test_3_47( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U8_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB P12,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_48"
static VOID pt_im_r2y_do_test_3_48( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_P12_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_49"
static VOID pt_im_r2y_do_test_3_49( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC444 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_50"
static VOID pt_im_r2y_do_test_3_50( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC444_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC444_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_51"
static VOID pt_im_r2y_do_test_3_51( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_52"
static VOID pt_im_r2y_do_test_3_52( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC420 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_53"
static VOID pt_im_r2y_do_test_3_53( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC420_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC420_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC420 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_54"
static VOID pt_im_r2y_do_test_3_54( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC420_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC420_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC420 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_55"
static VOID pt_im_r2y_do_test_3_55( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC420_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC420_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC444 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_56"
static VOID pt_im_r2y_do_test_3_56( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC444_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC444_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_57"
static VOID pt_im_r2y_do_test_3_57( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));


#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_58"
static VOID pt_im_r2y_do_test_3_58( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_59"
static VOID pt_im_r2y_do_test_3_59( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: Y only U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_60"
static VOID pt_im_r2y_do_test_3_60( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 2 );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_61"
static VOID pt_im_r2y_do_test_3_61( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', (1280 * 960 * 2 * 3) );
	memset( (VOID*)(D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP + (1280 * 960 * 2 * 3)), '\xee', (1280 * 960 * 2) + ( D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 256 ));
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, (1280 * 960 * 2 * 3) );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack(( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP + (1280 * 960 * 2 * 3)), D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES * 4 );
	Palladium_Set_Out_Localstack(( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP + (1280 * 960 * 2 * 3) + (1280 * 960 * 2)), D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 256 );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_62"
static VOID pt_im_r2y_do_test_3_62( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_63"
static VOID pt_im_r2y_do_test_3_63( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_64"
static VOID pt_im_r2y_do_test_3_64( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_65"
static VOID pt_im_r2y_do_test_3_65( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_66"
static VOID pt_im_r2y_do_test_3_66( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_67"
static VOID pt_im_r2y_do_test_3_67( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_68"
static VOID pt_im_r2y_do_test_3_68( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_69"
static VOID pt_im_r2y_do_test_3_69( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_70"
static VOID pt_im_r2y_do_test_3_70( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_71"
static VOID pt_im_r2y_do_test_3_71( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', (1280 * 960 * 2 * 3) );
	memset( (VOID*)(D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP + (1280 * 960 * 2 * 3)), '\xee', (1280 * 960 * 2) + ( D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 256 ));
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, (1280 * 960 * 2 * 3) );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack(( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP + (1280 * 960 * 2 * 3)), D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES * 4 );
	Palladium_Set_Out_Localstack(( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP + (1280 * 960 * 2 * 3) + (1280 * 960 * 2)), D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES / 256 );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_72"
static VOID pt_im_r2y_do_test_3_72( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
//	Palladium_Set_Out_Localstack( D_IM_R2Y_TBL_MEM_OUT_ADDR_TOP_TCTDATA, D_IM_R2Y_TBL_MEM_OUT_TCTDATA );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


// Input: RGB U16,  Output: YCC422 U8
#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "3_73"
static VOID pt_im_r2y_do_test_3_73( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );
	memset( (VOID*)D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, '\xee', D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_R2Y_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_R2Y_IMG_MEM_IN_RGB_U16_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* R2Y begin CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));
	CT_Im_R2Y_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* R2Y end CT_Im_R2y_" D_IM_R2Y_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_R2Y_IMG_MEM_OUT_YCC_ADDR_TOP, D_IM_R2Y_IMG_MEM_OUT_YCC422_BYTES );
//	Palladium_Set_Out_Localstack( D_IM_R2Y_TBL_MEM_OUT_ADDR_TOP_TCHSYDATA, D_IM_R2Y_TBL_MEM_OUT_TCHSYDATA );
//	Palladium_Set_Out_Localstack( D_IM_R2Y_TBL_MEM_OUT_ADDR_TOP_TCHSRDATA, D_IM_R2Y_TBL_MEM_OUT_TCHSRGBDATA );
//	Palladium_Set_Out_Localstack( D_IM_R2Y_TBL_MEM_OUT_ADDR_TOP_TCHSGDATA, D_IM_R2Y_TBL_MEM_OUT_TCHSRGBDATA );
//	Palladium_Set_Out_Localstack( D_IM_R2Y_TBL_MEM_OUT_ADDR_TOP_TCHSBDATA, D_IM_R2Y_TBL_MEM_OUT_TCHSRGBDATA );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO " end\n" ));
}


static VOID pt_im_r2y_run_3( const UINT32 type1 )
{
	UCHAR			type2;
	UCHAR			type3;

	type2	= gDDIM_Info.com._6b;
	type3	= gDDIM_Info.com._6c;

	switch( type2 ) {
		case 1:
			pt_im_r2y_do_test_3_1( type1, type2, type3 );
			break;
		case 2:
			pt_im_r2y_do_test_3_2( type1, type2, type3 );
			break;
		case 3:
			pt_im_r2y_do_test_3_3( type1, type2, type3 );
			break;
		case 4:
			pt_im_r2y_do_test_3_4( type1, type2, type3 );
			break;
		case 5:
			pt_im_r2y_do_test_3_5( type1, type2, type3 );
			break;
		case 6:
			pt_im_r2y_do_test_3_6( type1, type2, type3 );
			break;
		case 12:
			pt_im_r2y_do_test_3_12( type1, type2, type3 );
			break;
		case 13:
			pt_im_r2y_do_test_3_13( type1, type2, type3 );
			break;
		case 14:
			pt_im_r2y_do_test_3_14( type1, type2, type3 );
			break;
		case 15:
			pt_im_r2y_do_test_3_15( type1, type2, type3 );
			break;
		case 16:
			pt_im_r2y_do_test_3_16( type1, type2, type3 );
			break;
		case 17:
			pt_im_r2y_do_test_3_17( type1, type2, type3 );
			break;
		case 18:
			pt_im_r2y_do_test_3_18( type1, type2, type3 );
			break;
		case 19:
			pt_im_r2y_do_test_3_19( type1, type2, type3 );
			break;
		case 20:
			pt_im_r2y_do_test_3_20( type1, type2, type3 );
			break;
		case 21:
			pt_im_r2y_do_test_3_21( type1, type2, type3 );
			break;
		case 22:
			pt_im_r2y_do_test_3_22( type1, type2, type3 );
			break;
		case 23:
			pt_im_r2y_do_test_3_23( type1, type2, type3 );
			break;
		case 24:
			pt_im_r2y_do_test_3_24( type1, type2, type3 );
			break;
		case 25:
			pt_im_r2y_do_test_3_25( type1, type2, type3 );
			break;
		case 26:
			pt_im_r2y_do_test_3_26( type1, type2, type3 );
			break;
		case 27:
			pt_im_r2y_do_test_3_27( type1, type2, type3 );
			break;
		case 28:
			pt_im_r2y_do_test_3_28( type1, type2, type3 );
			break;
		case 29:
			pt_im_r2y_do_test_3_29( type1, type2, type3 );
			break;
		case 30:
			pt_im_r2y_do_test_3_30( type1, type2, type3 );
			break;
		case 31:
			pt_im_r2y_do_test_3_31( type1, type2, type3 );
			break;
		case 32:
			pt_im_r2y_do_test_3_32( type1, type2, type3 );
			break;
		case 33:
			pt_im_r2y_do_test_3_33( type1, type2, type3 );
			break;
		case 34:
			pt_im_r2y_do_test_3_34( type1, type2, type3 );
			break;
		case 35:
			pt_im_r2y_do_test_3_35( type1, type2, type3 );
			break;
		case 36:
			pt_im_r2y_do_test_3_36( type1, type2, type3 );
			break;
//		case 37:
//			pt_im_r2y_do_test_3_37( type1, type2, type3 );
//			break;
//		case 38:
//			pt_im_r2y_do_test_3_38( type1, type2, type3 );
//			break;
//		case 39:
//			pt_im_r2y_do_test_3_39( type1, type2, type3 );
//			break;
//		case 40:
//			pt_im_r2y_do_test_3_40( type1, type2, type3 );
//			break;
		case 41:
			pt_im_r2y_do_test_3_41( type1, type2, type3 );
			break;
		case 42:
			pt_im_r2y_do_test_3_42( type1, type2, type3 );
			break;
		case 43:
			pt_im_r2y_do_test_3_43( type1, type2, type3 );
			break;
		case 44:
			pt_im_r2y_do_test_3_44( type1, type2, type3 );
			break;
		case 45:
			pt_im_r2y_do_test_3_45( type1, type2, type3 );
			break;
		case 46:
			pt_im_r2y_do_test_3_46( type1, type2, type3 );
			break;
		case 47:
			pt_im_r2y_do_test_3_47( type1, type2, type3 );
			break;
		case 48:
			pt_im_r2y_do_test_3_48( type1, type2, type3 );
			break;
		case 49:
			pt_im_r2y_do_test_3_49( type1, type2, type3 );
			break;
		case 50:
			pt_im_r2y_do_test_3_50( type1, type2, type3 );
			break;
		case 51:
			pt_im_r2y_do_test_3_51( type1, type2, type3 );
			break;
		case 52:
			pt_im_r2y_do_test_3_52( type1, type2, type3 );
			break;
		case 53:
			pt_im_r2y_do_test_3_53( type1, type2, type3 );
			break;
		case 54:
			pt_im_r2y_do_test_3_54( type1, type2, type3 );
			break;
		case 55:
			pt_im_r2y_do_test_3_55( type1, type2, type3 );
			break;
		case 56:
			pt_im_r2y_do_test_3_56( type1, type2, type3 );
			break;
		case 57:
			pt_im_r2y_do_test_3_57( type1, type2, type3 );
			break;
		case 58:
			pt_im_r2y_do_test_3_58( type1, type2, type3 );
			break;
		case 59:
			pt_im_r2y_do_test_3_59( type1, type2, type3 );
			break;
		case 60:
			pt_im_r2y_do_test_3_60( type1, type2, type3 );
			break;
		case 61:
			pt_im_r2y_do_test_3_61( type1, type2, type3 );
			break;
		case 62:
			pt_im_r2y_do_test_3_62( type1, type2, type3 );
			break;
		case 63:
			pt_im_r2y_do_test_3_63( type1, type2, type3 );
			break;
		case 64:
			pt_im_r2y_do_test_3_64( type1, type2, type3 );
			break;
		case 65:
			pt_im_r2y_do_test_3_65( type1, type2, type3 );
			break;
		case 66:
			pt_im_r2y_do_test_3_66( type1, type2, type3 );
			break;
		case 67:
			pt_im_r2y_do_test_3_67( type1, type2, type3 );
			break;
		case 68:
			pt_im_r2y_do_test_3_68( type1, type2, type3 );
			break;
		case 69:
			pt_im_r2y_do_test_3_69( type1, type2, type3 );
			break;
		case 70:
			pt_im_r2y_do_test_3_70( type1, type2, type3 );
			break;
		case 71:
			pt_im_r2y_do_test_3_71( type1, type2, type3 );
			break;
		case 72:
			pt_im_r2y_do_test_3_72( type1, type2, type3 );
			break;
		case 73:
			pt_im_r2y_do_test_3_73( type1, type2, type3 );
			break;
		default:
			Ddim_Print(("*** R2Y test skip\n" ));
			break;
	}
}


#undef D_IM_R2Y_TEST_NO
#define D_IM_R2Y_TEST_NO "1_"
static VOID pt_im_r2y_run_9( const UINT32 type1 )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6b;

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO "%u begin\n", type2 ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "=-=-=-=-\n" ));
	Ddim_Print(( "* R2Y begin CT_Im_R2y_1_%u\n", type2 ));
	CT_Im_R2Y_Run( 1, type2, 0 );
	Ddim_Print(( "* R2Y end CT_Im_R2y_1_%u\n", type2 ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** R2Y test " D_IM_R2Y_TEST_NO "%u end\n", type2 ));
}


VOID Pt_Im_R2Y_Main( VOID )
{
	UCHAR			type1;

	// 開始...
	Ddim_Print(( "****** R2Y begin Pt_Im_R2Y_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	type1	= gDDIM_Info.com._6a;

	Dd_Top_Set_CLKSTOP11_R2Y1CK( 1 );
	Dd_Top_Set_CLKSEL4_R2Y1CLK( 0 );	// 300MHz
	Dd_Top_Set_CLKSTOP11_R2Y1CK( 0 );
	Dd_Top_Set_CLKSTOP13_R2Y2CK( 1 );
	Dd_Top_Set_CLKSEL6_R2Y2CLK( 0 );	// 300MHz
	Dd_Top_Set_CLKSTOP13_R2Y2CK( 0 );


	Ddim_Print(( "*** R2Y begin Im_R2Y_Init\n" ));
	Im_R2Y_Init( D_IM_R2Y_PIPE12 );
	Ddim_Print(( "*** R2Y end Im_R2Y_Init\n" ));

	pt_im_r2y_enable_gic();
	pt_im_r2y_start_clock();

	switch( type1 ) {
		case 1:
			pt_im_r2y_run_1( type1 );
			break;
		case 2:
			pt_im_r2y_run_2( type1 );
			break;
		case 3:
			pt_im_r2y_run_3( type1 );
			break;
		case 9:
			pt_im_r2y_run_9( type1 );
			break;
		default:
			Ddim_Print(("*** R2Y test skip\n" ));
			break;
	}

	// タイマーログ出力
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );


	Ddim_Print(( "(CLKSEL-4)\n" ));
	Ddim_Print(( "R2Y1CLK = %d\n", Dd_Top_Get_CLKSEL4_R2Y1CLK() ));
	Ddim_Print(( "(CLKSTOP-11)\n" ));
	Ddim_Print(( "R2Y1CK = %d\n", Dd_Top_Get_CLKSTOP11_R2Y1CK() ));
	Ddim_Print(( "R2Y1AP = %d\n", Dd_Top_Get_CLKSTOP11_R2Y1AP() ));
	Ddim_Print(( "R2Y1AH = %d\n", Dd_Top_Get_CLKSTOP11_R2Y1AH() ));
	Ddim_Print(( "R2Y1AX = %d\n", Dd_Top_Get_CLKSTOP11_R2Y1AX() ));
	Ddim_Print(( "(CLKSEL-6)\n" ));
	Ddim_Print(( "R2Y2CLK = %d\n", Dd_Top_Get_CLKSEL6_R2Y2CLK() ));
	Ddim_Print(( "(CLKSTOP-13)\n" ));
	Ddim_Print(( "R2Y2CK = %d\n", Dd_Top_Get_CLKSTOP13_R2Y2CK() ));
	Ddim_Print(( "R2Y2AP = %d\n", Dd_Top_Get_CLKSTOP13_R2Y2AP() ));
	Ddim_Print(( "R2Y2AH = %d\n", Dd_Top_Get_CLKSTOP13_R2Y2AH() ));
	Ddim_Print(( "R2Y2AX = %d\n", Dd_Top_Get_CLKSTOP13_R2Y2AX() ));
	Ddim_Print(( "****** R2Y end Pt_Im_R2Y_Main\n" ));

	// テスト強制終了処理
	Palladium_Force_Stop();
}

