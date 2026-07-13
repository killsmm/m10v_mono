/**
 * @file		palladium_test_b2r.c
 * @brief		palladium test b2r code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ct_im_b2r.h"
#include "im_b2r.h"
#include "dd_gic.h"
#include "dd_top.h"

#include "driver_common.h"
#include "palladium_test.h"

#include <string.h>

#define D_IM_B2R1_CLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP10())
#define D_IM_B2R1_ICLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP10())
#define D_IM_B2R1_HCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP10())
#define D_IM_B2R1_PCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP10())

#define D_IM_B2R1_CLK_REG_BIT	D_DD_TOP_B2R1CK_BIT	// bit20~21
#define D_IM_B2R1_ICLK_REG_BIT	D_DD_TOP_B2R1AX_BIT	// bit22~23
#define D_IM_B2R1_HCLK_REG_BIT	D_DD_TOP_B2R1AH_BIT	// bit24~25
#define D_IM_B2R1_PCLK_REG_BIT	D_DD_TOP_B2R1AP_BIT	// bit26~27

#define D_IM_B2R2_CLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP12())
#define D_IM_B2R2_ICLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP12())
#define D_IM_B2R2_HCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP12())
#define D_IM_B2R2_PCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP12())

#define D_IM_B2R2_CLK_REG_BIT	D_DD_TOP_B2R2CK_BIT	// bit20~21
#define D_IM_B2R2_ICLK_REG_BIT	D_DD_TOP_B2R2AX_BIT	// bit22~23
#define D_IM_B2R2_HCLK_REG_BIT	D_DD_TOP_B2R2AH_BIT	// bit24~25
#define D_IM_B2R2_PCLK_REG_BIT	D_DD_TOP_B2R2AP_BIT	// bit26~27

static VOID pt_im_b2r_enable_gic( VOID )
{
//	Dd_GIC_Init();
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH0_INT,    1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=118, enable, pri=30
	Dd_GIC_Ctrl( E_DD_GIC_INTID_B2R_UNIT1_INT,    1, D_DD_GIC_PRI30, 1 );		// int enable
	Dd_GIC_Ctrl( E_DD_GIC_INTID_B2R_UNIT2_INT,    1, D_DD_GIC_PRI30, 1 );		// int enable

	Dd_GIC_Ctrl( E_DD_GIC_INTID_B2B_UNIT1_INT,	  1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=223, enable, pri=30 B2B_UNIT1_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_B2B_UNIT1_VD_INT, 1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=224, enable, pri=30 B2B_UNIT1_VD_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_B2B_UNIT1_HD_INT, 1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=225, enable, pri=30 B2B_UNIT1_HD_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_B2B_UNIT2_INT,	  1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=226, enable, pri=30 B2B_UNIT2_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_B2B_UNIT2_VD_INT, 1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=227, enable, pri=30 B2B_UNIT2_VD_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_B2B_UNIT2_HD_INT, 1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=228, enable, pri=30 B2B_UNIT2_HD_INT
}


static VOID pt_im_b2r_start_clock( VOID )
{
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_PCLOCK) || defined(CO_ACT_HCLOCK)
#else
	UCHAR im_b2r1_clk_ctrl_cnt = 0;
	UCHAR im_b2r1_hclk_ctrl_cnt = 0;
	UCHAR im_b2r1_pclk_ctrl_cnt = 0;
	UCHAR im_b2r1_iclk_ctrl_cnt = 0;
	UCHAR im_b2r2_clk_ctrl_cnt = 0;
	UCHAR im_b2r2_hclk_ctrl_cnt = 0;
	UCHAR im_b2r2_pclk_ctrl_cnt = 0;
	UCHAR im_b2r2_iclk_ctrl_cnt = 0;

	Dd_Top_Start_Clock( &im_b2r1_pclk_ctrl_cnt, D_IM_B2R1_PCLK_REG_ADDR, ~D_IM_B2R1_PCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_b2r1_hclk_ctrl_cnt, D_IM_B2R1_HCLK_REG_ADDR, ~D_IM_B2R1_HCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_b2r1_iclk_ctrl_cnt, D_IM_B2R1_ICLK_REG_ADDR, ~D_IM_B2R1_ICLK_REG_BIT );
	Dd_Top_Start_Clock( &im_b2r1_clk_ctrl_cnt,  D_IM_B2R1_CLK_REG_ADDR,  ~D_IM_B2R1_CLK_REG_BIT );

	Dd_Top_Start_Clock( &im_b2r2_pclk_ctrl_cnt, D_IM_B2R2_PCLK_REG_ADDR, ~D_IM_B2R2_PCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_b2r2_hclk_ctrl_cnt, D_IM_B2R2_HCLK_REG_ADDR, ~D_IM_B2R2_HCLK_REG_BIT );
	Dd_Top_Start_Clock( &im_b2r2_iclk_ctrl_cnt, D_IM_B2R2_ICLK_REG_ADDR, ~D_IM_B2R2_ICLK_REG_BIT );
	Dd_Top_Start_Clock( &im_b2r2_clk_ctrl_cnt,  D_IM_B2R2_CLK_REG_ADDR,  ~D_IM_B2R2_CLK_REG_BIT );
#endif
}


#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "1_1"
static VOID pt_im_b2r_do_test_1_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		1, 14, 17, 18, 19, 20, 21,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* B2R begin CT_Im_B2r_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_B2R_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* B2R end CT_Im_B2r_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "1_22"
static VOID pt_im_b2r_do_test_1_22( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		22, 23, 25, 27, 28, 30,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* B2R begin CT_Im_B2r_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_B2R_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* B2R end CT_Im_B2r_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "1_32"
static VOID pt_im_b2r_do_test_1_32( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		32, 33, 34, 35, 36, 37,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* B2R begin CT_Im_B2r_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_B2R_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* B2R end CT_Im_B2r_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "1_38"
static VOID pt_im_b2r_do_test_1_38( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		38, 39, 40, 41, 42, 43, 44, 45
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* B2R begin CT_Im_B2r_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_B2R_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* B2R end CT_Im_B2r_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "1_46"
static VOID pt_im_b2r_do_test_1_46( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		46, 47, 48, 49, 50, 51, 52, 53, 54
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* B2R begin CT_Im_B2r_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_B2R_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* B2R end CT_Im_B2r_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


static VOID pt_im_b2r_run_1( const UINT32 type1 )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6b;

	switch( type2 ) {
		case 1:
			pt_im_b2r_do_test_1_1( type1, type2 );
			break;
		case 22:
			pt_im_b2r_do_test_1_22( type1, type2 );
			break;
		case 32:
			pt_im_b2r_do_test_1_32( type1, type2 );
			break;
		case 38:
			pt_im_b2r_do_test_1_38( type1, type2 );
			break;
		case 46:
			pt_im_b2r_do_test_1_46( type1, type2 );
			break;
		default:
			Ddim_Print(("*** B2R test skip\n" ));
			break;
	}
}



#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "2_1"
static VOID pt_im_b2r_do_test_2_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, 0 );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


static VOID pt_im_b2r_run_2( const UINT32 type1 )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6b;

	switch( type2 ) {
		case 1:
			pt_im_b2r_do_test_2_1( type1, type2 );
			break;
		default:
			Ddim_Print(("*** B2R test skip\n" ));
			break;
	}
}



// Input: 16bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_1"
static VOID pt_im_b2r_do_test_3_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw,  Output: RGB P12
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_2"
static VOID pt_im_b2r_do_test_3_2( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_P12_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_P12_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 12bit packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_3"
static VOID pt_im_b2r_do_test_3_3( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 12bit packed raw,  Output: RGB P12
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_4"
static VOID pt_im_b2r_do_test_3_4( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_P12_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_P12_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 10bit packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_5"
static VOID pt_im_b2r_do_test_3_5( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_10BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_10BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 10bit packed raw,  Output: RGB P12
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_6"
static VOID pt_im_b2r_do_test_3_6( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_10BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_P12_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_10BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_P12_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 8bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_7"
static VOID pt_im_b2r_do_test_3_7( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_8BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_8BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}

// Input: 8bit non-packed raw,  Output: RGB P12
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_8"
static VOID pt_im_b2r_do_test_3_8( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_8BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_P12_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_8BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_P12_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 12bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_9"
static VOID pt_im_b2r_do_test_3_9( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 12bit non-packed raw,  Output: RGB P12
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_10"
static VOID pt_im_b2r_do_test_3_10( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_P12_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_P12_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_11"
static VOID pt_im_b2r_do_test_3_11( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_17"
static VOID pt_im_b2r_do_test_3_17( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_18"
static VOID pt_im_b2r_do_test_3_18( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_19"
static VOID pt_im_b2r_do_test_3_19( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_20"
static VOID pt_im_b2r_do_test_3_20( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 12bit packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_21"
static VOID pt_im_b2r_do_test_3_21( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 12bit packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_22"
static VOID pt_im_b2r_do_test_3_22( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 10bit packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_23"
static VOID pt_im_b2r_do_test_3_23( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_10BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_10BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 10bit packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_24"
static VOID pt_im_b2r_do_test_3_24( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_10BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_10BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 8bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_25"
static VOID pt_im_b2r_do_test_3_25( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_8BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_8BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 8bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_26"
static VOID pt_im_b2r_do_test_3_26( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_8BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_8BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 12bit packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_27"
static VOID pt_im_b2r_do_test_3_27( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT2_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_12BIT );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 8bit non-packed raw,  Output: RGB P12
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_28"
static VOID pt_im_b2r_do_test_3_28( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_8BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_P12_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_8BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_P12_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_29"
static VOID pt_im_b2r_do_test_3_29( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_30"
static VOID pt_im_b2r_do_test_3_30( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_31"
static VOID pt_im_b2r_do_test_3_31( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_32"
static VOID pt_im_b2r_do_test_3_32( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_33"
static VOID pt_im_b2r_do_test_3_33( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw,  Output: RGB U8
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_34"
static VOID pt_im_b2r_do_test_3_34( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U8_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw,  Output: RGB U16
#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "3_35"
static VOID pt_im_b2r_do_test_3_35( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );
	memset( (VOID*)D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_B2R_IMG_MEM_OUT_RGB_U16_BYTES );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_B2R_IMG_MEM_IN_BAY_BYTES_16BIT );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* B2R begin CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));
	CT_Im_B2R_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* B2R end CT_Im_B2r_" D_IM_B2R_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_B2R_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_B2R_IMG_MEM_OUT_RGB_U16_BYTES );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO " end\n" ));
}


static VOID pt_im_b2r_run_3( const UINT32 type1 )
{
	UCHAR			type2;
	UCHAR			type3;

	type2	= gDDIM_Info.com._6b;
	type3	= gDDIM_Info.com._6c;

	switch( type2 ) {
		case 1:
			pt_im_b2r_do_test_3_1( type1, type2, type3 );
			break;
		case 2:
			pt_im_b2r_do_test_3_2( type1, type2, type3 );
			break;
		case 3:
			pt_im_b2r_do_test_3_3( type1, type2, type3 );
			break;
		case 4:
			pt_im_b2r_do_test_3_4( type1, type2, type3 );
			break;
		case 5:
			pt_im_b2r_do_test_3_5( type1, type2, type3 );
			break;
		case 6:
			pt_im_b2r_do_test_3_6( type1, type2, type3 );
			break;
		case 7:
			pt_im_b2r_do_test_3_7( type1, type2, type3 );
			break;
		case 8:
			pt_im_b2r_do_test_3_8( type1, type2, type3 );
			break;
		case 9:
			pt_im_b2r_do_test_3_9( type1, type2, type3 );
			break;
		case 10:
			pt_im_b2r_do_test_3_10( type1, type2, type3 );
			break;
		case 11:
			pt_im_b2r_do_test_3_11( type1, type2, type3 );
			break;
		case 17:
			pt_im_b2r_do_test_3_17( type1, type2, type3 );
			break;
		case 18:
			pt_im_b2r_do_test_3_18( type1, type2, type3 );
			break;
		case 19:
			pt_im_b2r_do_test_3_19( type1, type2, type3 );
			break;
		case 20:
			pt_im_b2r_do_test_3_20( type1, type2, type3 );
			break;
		case 21:
			pt_im_b2r_do_test_3_21( type1, type2, type3 );
			break;
		case 22:
			pt_im_b2r_do_test_3_22( type1, type2, type3 );
			break;
		case 23:
			pt_im_b2r_do_test_3_23( type1, type2, type3 );
			break;
		case 24:
			pt_im_b2r_do_test_3_24( type1, type2, type3 );
			break;
		case 25:
			pt_im_b2r_do_test_3_25( type1, type2, type3 );
			break;
		case 26:
			pt_im_b2r_do_test_3_26( type1, type2, type3 );
			break;
		case 27:
			pt_im_b2r_do_test_3_27( type1, type2, type3 );
			break;
		case 28:
			pt_im_b2r_do_test_3_28( type1, type2, type3 );
			break;
		case 29:
			pt_im_b2r_do_test_3_29( type1, type2, type3 );
			break;
		case 30:
			pt_im_b2r_do_test_3_30( type1, type2, type3 );
			break;
		case 31:
			pt_im_b2r_do_test_3_31( type1, type2, type3 );
			break;
		case 32:
			pt_im_b2r_do_test_3_32( type1, type2, type3 );
			break;
		case 33:
			pt_im_b2r_do_test_3_33( type1, type2, type3 );
			break;
		case 34:
			pt_im_b2r_do_test_3_34( type1, type2, type3 );
			break;
		case 35:
			pt_im_b2r_do_test_3_35( type1, type2, type3 );
			break;
		default:
			Ddim_Print(("*** B2R test skip\n" ));
			break;
	}
}


#undef D_IM_B2R_TEST_NO
#define D_IM_B2R_TEST_NO "1_"
static VOID pt_im_b2r_run_9( const UINT32 type1 )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6b;

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO "%u begin\n", type2 ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "=-=-=-=-\n" ));
	Ddim_Print(( "* B2R begin CT_Im_B2r_1_%u\n", type2 ));
	CT_Im_B2R_Run( 1, type2, 0 );
	Ddim_Print(( "* B2R end CT_Im_B2r_1_%u\n", type2 ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** B2R test " D_IM_B2R_TEST_NO "%u end\n", type2 ));
}


VOID Pt_Im_B2R_Main( VOID )
{
	UCHAR			type1;

	// 開始...
	Ddim_Print(( "****** B2R begin Pt_Im_B2R_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	type1	= gDDIM_Info.com._6a;

	Dd_Top_Set_CLKSTOP10_B2R1CK( 1 );
	Dd_Top_Set_CLKSEL4_B2R1CLK( 0 );	// 300MHz
	Dd_Top_Set_CLKSTOP10_B2R1CK( 0 );
	Dd_Top_Set_CLKSTOP12_B2R2CK( 1 );
	Dd_Top_Set_CLKSEL6_B2R2CLK( 0 );	// 300MHz
	Dd_Top_Set_CLKSTOP12_B2R2CK( 0 );


	Ddim_Print(( "*** B2R begin Im_B2R_Init\n" ));
	Im_B2R_Init(D_IM_B2R_PIPE12);
	Ddim_Print(( "*** B2R end Im_B2R_Init\n" ));

	pt_im_b2r_enable_gic();
	pt_im_b2r_start_clock();

	switch( type1 ) {
		case 1:
			pt_im_b2r_run_1( type1 );
			break;
		case 2:
			pt_im_b2r_run_2( type1 );
			break;
		case 3:
			pt_im_b2r_run_3( type1 );
			break;
		case 9:
			pt_im_b2r_run_9( type1 );
			break;
		default:
			Ddim_Print(("*** B2R test skip\n" ));
			break;
	}

	// タイマーログ出力
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );


	Ddim_Print(( "(CLKSEL-4)\n" ));
	Ddim_Print(( "B2R1CLK = %d\n", Dd_Top_Get_CLKSEL4_B2R1CLK() ));
	Ddim_Print(( "(CLKSTOP-10)\n" ));
	Ddim_Print(( "B2R1CK = %d\n", Dd_Top_Get_CLKSTOP10_B2R1CK() ));
	Ddim_Print(( "B2R1AP = %d\n", Dd_Top_Get_CLKSTOP10_B2R1AP() ));
	Ddim_Print(( "B2R1AH = %d\n", Dd_Top_Get_CLKSTOP10_B2R1AH() ));
	Ddim_Print(( "B2R1AX = %d\n", Dd_Top_Get_CLKSTOP10_B2R1AX() ));
	Ddim_Print(( "(CLKSEL-6)\n" ));
	Ddim_Print(( "B2R2CLK = %d\n", Dd_Top_Get_CLKSEL6_B2R2CLK() ));
	Ddim_Print(( "(CLKSTOP-12)\n" ));
	Ddim_Print(( "B2R2CK = %d\n", Dd_Top_Get_CLKSTOP12_B2R2CK() ));
	Ddim_Print(( "B2R2AP = %d\n", Dd_Top_Get_CLKSTOP12_B2R2AP() ));
	Ddim_Print(( "B2R2AH = %d\n", Dd_Top_Get_CLKSTOP12_B2R2AH() ));
	Ddim_Print(( "B2R2AX = %d\n", Dd_Top_Get_CLKSTOP12_B2R2AX() ));
	Ddim_Print(( "****** B2R end Pt_Im_B2R_Main\n" ));

	// テスト強制終了処理
	Palladium_Force_Stop();
}

