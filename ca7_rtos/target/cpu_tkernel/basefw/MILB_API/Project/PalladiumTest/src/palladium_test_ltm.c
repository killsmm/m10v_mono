/**
 * @file		palladium_test_ltm.c
 * @brief		palladium test ltm code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include "ct_im_b2r.h"
#include "ct_im_ltm.h"
#include "im_ltm.h"
#include "dd_gic.h"
#include "dd_top.h"

#include "driver_common.h"
#include "palladium_test.h"

#include <string.h>

#define D_IM_LTM1_RBKCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP10())
#define D_IM_LTM1_CLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP11())
#define D_IM_LTM1_HCLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP11())
#define D_IM_LTM1_PCLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP11())
#define D_IM_LTM1_ACLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP11())

#define D_IM_LTM2_RBKCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP12())
#define D_IM_LTM2_CLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP13())
#define D_IM_LTM2_HCLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP13())
#define D_IM_LTM2_PCLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP13())
#define D_IM_LTM2_ACLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP13())

#define D_IM_LTM1_RBKCLK_REG_BIT	D_DD_TOP_LTMRBK1CK_BIT
#define D_IM_LTM1_CLK_REG_BIT		D_DD_TOP_LTM1CK_BIT
#define D_IM_LTM1_HCLK_REG_BIT		D_DD_TOP_LTM1AH_BIT
#define D_IM_LTM1_PCLK_REG_BIT		D_DD_TOP_LTM1AP_BIT
#define D_IM_LTM1_ACLK_REG_BIT		D_DD_TOP_LTM1AX_BIT

#define D_IM_LTM2_RBKCLK_REG_BIT	D_DD_TOP_LTMRBK2CK_BIT
#define D_IM_LTM2_CLK_REG_BIT		D_DD_TOP_LTM2CK_BIT
#define D_IM_LTM2_HCLK_REG_BIT		D_DD_TOP_LTM2AH_BIT
#define D_IM_LTM2_PCLK_REG_BIT		D_DD_TOP_LTM2AP_BIT
#define D_IM_LTM2_ACLK_REG_BIT		D_DD_TOP_LTM2AX_BIT

static VOID pt_im_ltm_enable_gic( VOID )
{
//	Dd_GIC_Init();
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH0_INT,    1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=118, enable, pri=30
	Dd_GIC_Ctrl( E_DD_GIC_INTID_LTM_UNIT1_INT,    1, D_DD_GIC_PRI30, 1 );		// int enable
	Dd_GIC_Ctrl( E_DD_GIC_INTID_LTM_UNIT2_INT,    1, D_DD_GIC_PRI30, 1 );		// int enable

	Dd_GIC_Ctrl( E_DD_GIC_INTID_SRO_UNIT1_INT,    1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=217, enable, pri=30 SRO_UNIT1_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_SRO_UNIT1_VD_INT, 1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=218, enable, pri=30 SRO_UNIT1_VD_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_SRO_UNIT1_HD_INT, 1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=219, enable, pri=30 SRO_UNIT1_HD_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_SRO_UNIT2_INT,    1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=220, enable, pri=30 SRO_UNIT2_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_SRO_UNIT2_VD_INT, 1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=221, enable, pri=30 SRO_UNIT2_VD_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_SRO_UNIT2_HD_INT, 1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=222, enable, pri=30 SRO_UNIT2_HD_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_B2R_UNIT1_INT,    1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=233, enable, pri=30 B2R_UNIT1_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_B2R_UNIT2_INT,    1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=234, enable, pri=30 B2R_UNIT2_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_R2Y_UNIT1_INT,    1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=237, enable, pri=30 R2Y_UNIT1_INT
	Dd_GIC_Ctrl( E_DD_GIC_INTID_R2Y_UNIT2_INT,    1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=238, enable, pri=30 R2Y_UNIT2_INT
}


static VOID pt_im_ltm_start_clock( VOID )
{
#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_PCLOCK) || defined(CO_ACT_HCLOCK)
#else
	UCHAR im_ltm1_rbkclk_ctrl_cnt = 0;
	UCHAR im_ltm1_clk_ctrl_cnt    = 0;
	UCHAR im_ltm1_hclk_ctrl_cnt   = 0;
	UCHAR im_ltm1_pclk_ctrl_cnt   = 0;
	UCHAR im_ltm1_iclk_ctrl_cnt   = 0;
	UCHAR im_ltm2_rbkclk_ctrl_cnt = 0;
	UCHAR im_ltm2_clk_ctrl_cnt    = 0;
	UCHAR im_ltm2_hclk_ctrl_cnt   = 0;
	UCHAR im_ltm2_pclk_ctrl_cnt   = 0;
	UCHAR im_ltm2_iclk_ctrl_cnt   = 0;

	Dd_Top_Start_Clock( &im_ltm1_pclk_ctrl_cnt,    D_IM_LTM1_PCLK_REG_ADDR,    ~D_IM_LTM1_PCLK_REG_BIT   );
	Dd_Top_Start_Clock( &im_ltm1_hclk_ctrl_cnt,    D_IM_LTM1_HCLK_REG_ADDR,    ~D_IM_LTM1_HCLK_REG_BIT   );
	Dd_Top_Start_Clock( &im_ltm1_iclk_ctrl_cnt,    D_IM_LTM1_ACLK_REG_ADDR,    ~D_IM_LTM1_ACLK_REG_BIT   );
	Dd_Top_Start_Clock( &im_ltm1_clk_ctrl_cnt,     D_IM_LTM1_CLK_REG_ADDR,     ~D_IM_LTM1_CLK_REG_BIT    );
	Dd_Top_Start_Clock( &im_ltm1_rbkclk_ctrl_cnt,  D_IM_LTM1_RBKCLK_REG_ADDR,  ~D_IM_LTM1_RBKCLK_REG_BIT );

	Dd_Top_Start_Clock( &im_ltm2_pclk_ctrl_cnt,    D_IM_LTM2_PCLK_REG_ADDR,    ~D_IM_LTM2_PCLK_REG_BIT   );
	Dd_Top_Start_Clock( &im_ltm2_hclk_ctrl_cnt,    D_IM_LTM2_HCLK_REG_ADDR,    ~D_IM_LTM2_HCLK_REG_BIT   );
	Dd_Top_Start_Clock( &im_ltm2_iclk_ctrl_cnt,    D_IM_LTM2_ACLK_REG_ADDR,    ~D_IM_LTM2_ACLK_REG_BIT   );
	Dd_Top_Start_Clock( &im_ltm2_clk_ctrl_cnt,     D_IM_LTM2_CLK_REG_ADDR,     ~D_IM_LTM2_CLK_REG_BIT    );
	Dd_Top_Start_Clock( &im_ltm2_rbkclk_ctrl_cnt,  D_IM_LTM2_RBKCLK_REG_ADDR,  ~D_IM_LTM2_RBKCLK_REG_BIT );
#endif
}


#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "1_1"
static VOID pt_im_ltm_rbk_do_test_1_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		1, 2, 3, 4, 5, 6, 7, 8,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_RBK_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "1_9"
static VOID pt_im_ltm_rbk_do_test_1_9( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		9, 10, 11, 12, 13, 14, 16, 17,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_RBK_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


static VOID pt_im_ltm_rbk_run_1( const UINT32 type2 )
{
	UCHAR			type3;

	type3	= gDDIM_Info.com._6c;

	switch( type3 ) {
		case 1:
			pt_im_ltm_rbk_do_test_1_1( type2, type3 );
			break;
		case 9:
			pt_im_ltm_rbk_do_test_1_9( type2, type3 );
			break;
		default:
			Ddim_Print(("*** LTM RBK test skip\n" ));
			break;
	}
}



#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "2_1"
static VOID pt_im_ltm_rbk_do_test_2_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		1, 2, 3, 4, 5, 6,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_RBK_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "2_7"
static VOID pt_im_ltm_rbk_do_test_2_7( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		7, 8, 9, 10, 11, 12, 13,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_RBK_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


static VOID pt_im_ltm_rbk_run_2( const UINT32 type2 )
{
	UCHAR			type3;

	type3	= gDDIM_Info.com._6c;

	switch( type3 ) {
		case 1:
			pt_im_ltm_rbk_do_test_2_1( type2, type3 );
			break;
		case 7:
			pt_im_ltm_rbk_do_test_2_7( type2, type3 );
			break;
		default:
			Ddim_Print(("*** LTM RBK test skip\n" ));
			break;
	}
}



#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_1"
static VOID pt_im_ltm_rbk_do_test_3_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_2"
static VOID pt_im_ltm_rbk_do_test_3_2( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_3"
static VOID pt_im_ltm_rbk_do_test_3_3( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_4"
static VOID pt_im_ltm_rbk_do_test_3_4( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP / 4 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP / 4 );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_5"
static VOID pt_im_ltm_rbk_do_test_3_5( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP / 4 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP / 4 );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_6"
static VOID pt_im_ltm_rbk_do_test_3_6( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP / 16 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP / 16 );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_7"
static VOID pt_im_ltm_rbk_do_test_3_7( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP / 64 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP / 64 );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}

// Input: 16bit non-packed RGB planar,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_8"
static VOID pt_im_ltm_rbk_do_test_3_8( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP / 4 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP / 4 );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit packed raw,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_9"
static VOID pt_im_ltm_rbk_do_test_3_9( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_BAY_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_BAY_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit packed raw,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_10"
static VOID pt_im_ltm_rbk_do_test_3_10( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_BAY_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_BAY_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit packed raw,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_11"
static VOID pt_im_ltm_rbk_do_test_3_11( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_BAY_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP / 4 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_BAY_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP / 4 );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit packed raw,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_12"
static VOID pt_im_ltm_rbk_do_test_3_12( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_BAY_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP / 4 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_BAY_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP / 4 );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit packed raw,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_13"
static VOID pt_im_ltm_rbk_do_test_3_13( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_BAY_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_BAY_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit packed raw,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_14"
static VOID pt_im_ltm_rbk_do_test_3_14( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_BAY_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_BAY_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit packed raw,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_15"
static VOID pt_im_ltm_rbk_do_test_3_15( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_BAY_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP / 4 );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_BAY_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP / 4 );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit packed raw,  Output: 12bit non-packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_16"
static VOID pt_im_ltm_rbk_do_test_3_16( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, '\xee', (D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH + 2) * 3 / 2 * (D_IM_LTM_RBK_SRC_IMG_PIXS_LINES + 2) );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_BAY_ADDR_TOP, (D_IM_LTM_RBK_SRC_IMG_PIXS_WIDTH + 2) * 3 / 2 * (D_IM_LTM_RBK_SRC_IMG_PIXS_LINES + 2) );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar,  Output: 12bit packed RGB dot
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_18"
static VOID pt_im_ltm_rbk_do_test_3_18( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, '\xee', D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, '\xee', D_IM_LTM_RBK_DST_IMG_BYTES_P );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_RBK_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_RBK_IMG_MEM_OUT_RGB_ADDR_TOP, D_IM_LTM_RBK_DST_IMG_BYTES_P );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO " end\n" ));
}


static VOID pt_im_ltm_rbk_run_3( const UINT32 type2 )
{
	UCHAR			type3;
	UCHAR			type4;

	type3	= gDDIM_Info.com._6c;
	type4	= gDDIM_Info.com._6d;

	switch( type3 ) {
		case 1:
			pt_im_ltm_rbk_do_test_3_1( type2, type3, type4 );
			break;
		case 2:
			pt_im_ltm_rbk_do_test_3_2( type2, type3, type4 );
			break;
		case 3:
			pt_im_ltm_rbk_do_test_3_3( type2, type3, type4 );
			break;
		case 4:
			pt_im_ltm_rbk_do_test_3_4( type2, type3, type4 );
			break;
		case 5:
			pt_im_ltm_rbk_do_test_3_5( type2, type3, type4 );
			break;
		case 6:
			pt_im_ltm_rbk_do_test_3_6( type2, type3, type4 );
			break;
		case 7:
			pt_im_ltm_rbk_do_test_3_7( type2, type3, type4 );
			break;
		case 8:
			pt_im_ltm_rbk_do_test_3_8( type2, type3, type4 );
			break;
		case 9:
			pt_im_ltm_rbk_do_test_3_9( type2, type3, type4 );
			break;
		case 10:
			pt_im_ltm_rbk_do_test_3_10( type2, type3, type4 );
			break;
		case 11:
			pt_im_ltm_rbk_do_test_3_11( type2, type3, type4 );
			break;
		case 12:
			pt_im_ltm_rbk_do_test_3_12( type2, type3, type4 );
			break;
		case 13:
			pt_im_ltm_rbk_do_test_3_13( type2, type3, type4 );
			break;
		case 14:
			pt_im_ltm_rbk_do_test_3_14( type2, type3, type4 );
			break;
		case 15:
			pt_im_ltm_rbk_do_test_3_15( type2, type3, type4 );
			break;
		case 16:
			pt_im_ltm_rbk_do_test_3_16( type2, type3, type4 );
			break;
		case 18:
			pt_im_ltm_rbk_do_test_3_18( type2, type3, type4 );
			break;
		default:
			Ddim_Print(("*** LTM RBK test skip\n" ));
			break;
	}
}



#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "9_"
static VOID pt_im_ltm_rbk_run_9( const UINT32 type1 )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6c;

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO "%u begin\n", type2 ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "=-=-=-=-\n" ));
	Ddim_Print(( "* LTM RBK begin CT_Im_Ltm_Rbk_1_%u\n", type2 ));
	CT_Im_LTM_RBK_Run( 1, type2, 0 );
	Ddim_Print(( "* LTM RBK end CT_Im_Ltm_Rbk_1_%u\n", type2 ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM RBK test " D_IM_LTM_TEST_NO "%u end\n", type2 ));
}


VOID pt_im_ltm_rbk_run( VOID )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6b;

	switch( type2 ) {
		case 1:
			pt_im_ltm_rbk_run_1( type2 );
			break;
		case 2:
			pt_im_ltm_rbk_run_2( type2 );
			break;
		case 3:
			pt_im_ltm_rbk_run_3( type2 );
			break;
		case 9:
			pt_im_ltm_rbk_run_9( type2 );
			break;
		default:
			Ddim_Print(("*** LTM RBK test skip\n" ));
			break;
	}
}


#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "1_1"
static VOID pt_im_ltm_map_do_test_1_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		1, 2, 3, 4, 5, 6, 7,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_MAP_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "1_8"
static VOID pt_im_ltm_map_do_test_1_8( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		8, 9, 10, 11, 12, 13,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_MAP_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "1_15"
static VOID pt_im_ltm_map_do_test_1_15( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		15, 16, 17, 18, 19, 20, 21,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_MAP_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


static VOID pt_im_ltm_map_run_1( const UINT32 type2 )
{
	UCHAR			type3;

	type3	= gDDIM_Info.com._6c;

	switch( type3 ) {
		case 1:
			pt_im_ltm_map_do_test_1_1( type2, type3 );
			break;
		case 8:
			pt_im_ltm_map_do_test_1_8( type2, type3 );
			break;
		case 15:
			pt_im_ltm_map_do_test_1_15( type2, type3 );
			break;
		default:
			Ddim_Print(("*** LTM MAP test skip\n" ));
			break;
	}
}



#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "2_1"
static VOID pt_im_ltm_map_do_test_2_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		1, 2, 3, 4, 5, 6, 7, 8, 9,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_MAP_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "2_10"
static VOID pt_im_ltm_map_do_test_2_10( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		10, 11, 12, 13, 14, 15, 16, 17,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_MAP_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


static VOID pt_im_ltm_map_run_2( const UINT32 type2 )
{
	UCHAR			type3;

	type3	= gDDIM_Info.com._6c;

	switch( type3 ) {
		case 1:
			pt_im_ltm_map_do_test_2_1( type2, type3 );
			break;
		case 10:
			pt_im_ltm_map_do_test_2_10( type2, type3 );
			break;
		default:
			Ddim_Print(("*** LTM MAP test skip\n" ));
			break;
	}
}



#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_1"
static VOID pt_im_ltm_map_do_test_3_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_MAP_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit non-packed RGB dot,  Output: 12bit non-packed Y(YP/YN)
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_2"
static VOID pt_im_ltm_map_do_test_3_2( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_LTM_MAP_SRC_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_LTM_MAP_SRC_IMG_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_MAP_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit packed RGB dot,  Output: 12bit non-packed Y(YP/YN)
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_3"
static VOID pt_im_ltm_map_do_test_3_3( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_LTM_MAP_SRC_IMG_BYTES_P );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_LTM_MAP_SRC_IMG_BYTES_P );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_MAP_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit non-packed RGB dot,  Output: 12bit non-packed Y(YP/YN)
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_4"
static VOID pt_im_ltm_map_do_test_3_4( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_LTM_MAP_SRC_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_LTM_MAP_SRC_IMG_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_MAP_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit non-packed RGB dot,  Output: 12bit non-packed Y(YP/YN)
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_5"
static VOID pt_im_ltm_map_do_test_3_5( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_LTM_MAP_SRC_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_LTM_MAP_SRC_IMG_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_MAP_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit non-packed RGB dot,  Output: 12bit non-packed Y(YP/YN)
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_6"
static VOID pt_im_ltm_map_do_test_3_6( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_LTM_MAP_SRC_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_LTM_MAP_SRC_IMG_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_MAP_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit non-packed RGB dot,  Output: 12bit non-packed Y(YP/YN)
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_7"
static VOID pt_im_ltm_map_do_test_3_7( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_LTM_MAP_SRC_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_LTM_MAP_SRC_IMG_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_MAP_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}

// Input: 12bit non-packed RGB dot,  Output: 12bit non-packed Y(YP/YN)
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_8"
static VOID pt_im_ltm_map_do_test_3_8( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_LTM_MAP_SRC_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_LTM_MAP_SRC_IMG_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_MAP_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit non-packed RGB dot,  Output: 12bit non-packed Y(YP/YN)
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_9"
static VOID pt_im_ltm_map_do_test_3_9( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_LTM_MAP_SRC_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_LTM_MAP_SRC_IMG_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_MAP_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit non-packed RGB dot,  Output: 12bit non-packed Y(YP/YN)
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_10"
static VOID pt_im_ltm_map_do_test_3_10( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_LTM_MAP_SRC_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_LTM_MAP_SRC_IMG_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_MAP_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar,  Output: 12bit non-packed Y(YP/YN)
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_11"
static VOID pt_im_ltm_map_do_test_3_11( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP,   '\xee', D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP  );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_UP  );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_RBK_IMG_MEM_IN_R_ADDR_TOP, D_IM_LTM_RBK_SRC_IMG_RGB_BYTES );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_MAP_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 12bit non-packed RGB dot,  Output: 12bit packed Y(YP/YN)
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_12"
static VOID pt_im_ltm_map_do_test_3_12( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, '\xee', D_IM_LTM_MAP_SRC_IMG_BYTES_UP );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_P );
	memset( (VOID*)D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, '\xee', D_IM_LTM_MAP_DST_IMG_BYTES_P );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_MAP_IMG_MEM_IN_RGB_ADDR_TOP, D_IM_LTM_MAP_SRC_IMG_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_MAP_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YP_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_P );
	Palladium_Set_Out_Localstack( D_IM_LTM_MAP_IMG_MEM_OUT_YN_ADDR_TOP, D_IM_LTM_MAP_DST_IMG_BYTES_P );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO " end\n" ));
}


static VOID pt_im_ltm_map_run_3( const UINT32 type2 )
{
	UCHAR			type3;
	UCHAR			type4;

	type3	= gDDIM_Info.com._6c;
	type4	= gDDIM_Info.com._6d;

	switch( type3 ) {
		case 1:
			pt_im_ltm_map_do_test_3_1( type2, type3, type4 );
			break;
		case 2:
			pt_im_ltm_map_do_test_3_2( type2, type3, type4 );
			break;
		case 3:
			pt_im_ltm_map_do_test_3_3( type2, type3, type4 );
			break;
		case 4:
			pt_im_ltm_map_do_test_3_4( type2, type3, type4 );
			break;
		case 5:
			pt_im_ltm_map_do_test_3_5( type2, type3, type4 );
			break;
		case 6:
			pt_im_ltm_map_do_test_3_6( type2, type3, type4 );
			break;
		case 7:
			pt_im_ltm_map_do_test_3_7( type2, type3, type4 );
			break;
		case 8:
			pt_im_ltm_map_do_test_3_8( type2, type3, type4 );
			break;
		case 9:
			pt_im_ltm_map_do_test_3_9( type2, type3, type4 );
			break;
		case 10:
			pt_im_ltm_map_do_test_3_10( type2, type3, type4 );
			break;
		case 11:
			pt_im_ltm_map_do_test_3_11( type2, type3, type4 );
			break;
		case 12:
			pt_im_ltm_map_do_test_3_12( type2, type3, type4 );
			break;
		default:
			Ddim_Print(("*** LTM MAP test skip\n" ));
			break;
	}
}



#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "9_"
static VOID pt_im_ltm_map_run_9( const UINT32 type1 )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6c;

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO "%u begin\n", type2 ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "=-=-=-=-\n" ));
	Ddim_Print(( "* LTM MAP begin CT_Im_Ltm_Map_1_%u\n", type2 ));
	CT_Im_LTM_MAP_Run( 1, type2, 0 );
	Ddim_Print(( "* LTM MAP end CT_Im_Ltm_Map_1_%u\n", type2 ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM MAP test " D_IM_LTM_TEST_NO "%u end\n", type2 ));
}


VOID pt_im_ltm_map_run( VOID )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6b;

	switch( type2 ) {
		case 1:
			pt_im_ltm_map_run_1( type2 );
			break;
		case 2:
			pt_im_ltm_map_run_2( type2 );
			break;
		case 3:
			pt_im_ltm_map_run_3( type2 );
			break;
		case 9:
			pt_im_ltm_map_run_9( type2 );
			break;
		default:
			Ddim_Print(("*** LTM MAP test skip\n" ));
			break;
	}
}


#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "1_1"
static VOID pt_im_ltm_ltm_do_test_1_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		1, 2, 3, 4, 5, 6, 7, 8,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_LTM_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "1_9"
static VOID pt_im_ltm_ltm_do_test_1_9( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		9, 10, 11, 12, 13, 14, 15, 16,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_LTM_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "1_18"
static VOID pt_im_ltm_ltm_do_test_1_18( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		18, 19, 20, 21, 22, 23, 24, 25,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_LTM_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


static VOID pt_im_ltm_ltm_run_1( const UINT32 type2 )
{
	UCHAR			type3;

	type3	= gDDIM_Info.com._6c;

	switch( type3 ) {
		case 1:
			pt_im_ltm_ltm_do_test_1_1( type2, type3 );
			break;
		case 9:
			pt_im_ltm_ltm_do_test_1_9( type2, type3 );
			break;
		case 18:
			pt_im_ltm_ltm_do_test_1_18( type2, type3 );
			break;
		default:
			Ddim_Print(("*** LTM LTM test skip\n" ));
			break;
	}
}



#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "2_1"
static VOID pt_im_ltm_ltm_do_test_2_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		1, 2, 3, 4, 5, 6, 7,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_LTM_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "2_8"
static VOID pt_im_ltm_ltm_do_test_2_8( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		8, 9, 10, 11, 12, 13, 14,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_LTM_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "2_15"
static VOID pt_im_ltm_ltm_do_test_2_15( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd )
{
	static const UCHAR	no_2nd_tbl[] = {
		15, 16, 17, 18, 19, 20, 21,
	};
	UINT32				loopcnt;

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	for( loopcnt = 0; loopcnt < (sizeof(no_2nd_tbl) / sizeof(no_2nd_tbl[0])); loopcnt++ ) {
		Ddim_Print(( "=-=-=-=-\n" ));
		Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
		CT_Im_LTM_LTM_Run( ct_idx_1st, no_2nd_tbl[loopcnt], 0 );
		Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_%u_%u\n", ct_idx_1st, no_2nd_tbl[loopcnt] ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


static VOID pt_im_ltm_ltm_run_2( const UINT32 type2 )
{
	UCHAR			type3;

	type3	= gDDIM_Info.com._6c;

	switch( type3 ) {
		case 1:
			pt_im_ltm_ltm_do_test_2_1( type2, type3 );
			break;
		case 8:
			pt_im_ltm_ltm_do_test_2_8( type2, type3 );
			break;
		case 15:
			pt_im_ltm_ltm_do_test_2_15( type2, type3 );
			break;
		default:
			Ddim_Print(("*** LTM LTM test skip\n" ));
			break;
	}
}



#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_1"
static VOID pt_im_ltm_ltm_do_test_3_1( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar / 12bit non-packed Y(YP/YN),  Output: 12bit non-packed RGB planar
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_2"
static VOID pt_im_ltm_ltm_do_test_3_2( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  '\xee', D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, '\xee', D_IM_LTM_LTM_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, D_IM_LTM_LTM_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar / 12bit packed Y(YP/YN),  Output: 12bit non-packed RGB planar
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_3"
static VOID pt_im_ltm_ltm_do_test_3_3( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  '\xee', D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_P );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_P );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, '\xee', D_IM_LTM_LTM_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_P );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_P );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, D_IM_LTM_LTM_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar / 12bit non-packed Y(YP/YN),  Output: 12bit non-packed RGB planar
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_4"
static VOID pt_im_ltm_ltm_do_test_3_4( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  '\xee', D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, '\xee', D_IM_LTM_LTM_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, D_IM_LTM_LTM_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar / 12bit non-packed Y(YP/YN),  Output: 12bit non-packed RGB planar
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_5"
static VOID pt_im_ltm_ltm_do_test_3_5( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  '\xee', D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, '\xee', D_IM_LTM_LTM_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, D_IM_LTM_LTM_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar / 12bit non-packed Y(YP/YN),  Output: 12bit non-packed RGB planar
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_6"
static VOID pt_im_ltm_ltm_do_test_3_6( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  '\xee', D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, '\xee', D_IM_LTM_LTM_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, D_IM_LTM_LTM_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar / 12bit non-packed Y(YP/YN),  Output: 12bit non-packed RGB planar
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_7"
static VOID pt_im_ltm_ltm_do_test_3_7( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  '\xee', D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, '\xee', D_IM_LTM_LTM_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, D_IM_LTM_LTM_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}

// Input: 16bit non-packed RGB planar / 12bit non-packed Y(YP/YN),  Output: 12bit non-packed RGB planar
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_8"
static VOID pt_im_ltm_ltm_do_test_3_8( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  '\xee', D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, '\xee', D_IM_LTM_LTM_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, D_IM_LTM_LTM_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar / 12bit non-packed Y(YP/YN),  Output: 12bit non-packed RGB planar
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_9"
static VOID pt_im_ltm_ltm_do_test_3_9( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  '\xee', D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, '\xee', D_IM_LTM_LTM_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, D_IM_LTM_LTM_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar / 12bit non-packed Y(YP/YN),  Output: 12bit non-packed RGB planar
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_10"
static VOID pt_im_ltm_ltm_do_test_3_10( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  '\xee', D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, '\xee', D_IM_LTM_LTM_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, D_IM_LTM_LTM_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar / 12bit non-packed Y(YP/YN),  Output: 12bit non-packed RGB planar
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_11"
static VOID pt_im_ltm_ltm_do_test_3_11( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  '\xee', D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, '\xee', D_IM_LTM_LTM_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, D_IM_LTM_LTM_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar / 12bit non-packed Y(YP/YN),  Output: 12bit non-packed RGB planar
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_12"
static VOID pt_im_ltm_ltm_do_test_3_12( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  '\xee', D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, '\xee', D_IM_LTM_LTM_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, D_IM_LTM_LTM_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed RGB planar / 12bit non-packed Y(YP/YN),  Output: 12bit packed RGB planar
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_13"
static VOID pt_im_ltm_ltm_do_test_3_13( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  '\xee', D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, '\xee', D_IM_LTM_LTM_DST_IMG_BYTES_P );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_R_ADDR_TOP,  D_IM_LTM_LTM_SRC_IMG_RGB_BYTES );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, D_IM_LTM_LTM_DST_IMG_BYTES_P );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw / 12bit non-packed Y(YP/YN),  Output: 12bit non-packed RGB planar
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_14"
static VOID pt_im_ltm_ltm_do_test_3_14( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP,    '\xee', D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_16BIT_NONPACK );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, '\xee', D_IM_LTM_LTM_DST_IMG_BYTES_UP );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP,    D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_16BIT_NONPACK );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_LTM_IMG_MEM_OUT_R_ADDR_TOP, D_IM_LTM_LTM_DST_IMG_BYTES_UP );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


// Input: 16bit non-packed raw / 12bit non-packed Y(YP/YN),  Output: YCC422 U8
#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "3_15"
static VOID pt_im_ltm_ltm_do_test_3_15( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " begin\n" ));

#if 0
	// 入出力エリアをFill
	memset( (VOID*)D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP,    '\xee', D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_16BIT_NONPACK );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, '\xee', D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	memset( (VOID*)D_IM_LTM_LTM_IMG_MEM_OUT_Y_ADDR_TOP, '\xee', D_IM_LTM_LTM_DST_IMG_BYTES_YCC );
#endif

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_B2R_IMG_MEM_IN_BAY_ADDR_TOP,    D_IM_B2R_SRC_IMG_GLOBAL_WIDTH_BAY_16BIT_NONPACK );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YP_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );
	Palladium_Set_In_Localstack( D_IM_LTM_LTM_IMG_MEM_IN_YN_ADDR_TOP, D_IM_LTM_LTM_SRC_IMG_MAP_BYTES_UP );

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));
	CT_Im_LTM_LTM_Run( ct_idx_1st, ct_idx_2nd, ct_idx_3rd );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_" D_IM_LTM_TEST_NO "\n" ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_LTM_LTM_IMG_MEM_OUT_Y_ADDR_TOP, D_IM_LTM_LTM_DST_IMG_BYTES_YCC );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO " end\n" ));
}


static VOID pt_im_ltm_ltm_run_3( const UINT32 type2 )
{
	UCHAR			type3;
	UCHAR			type4;

	type3	= gDDIM_Info.com._6c;
	type4	= gDDIM_Info.com._6d;

	switch( type3 ) {
		case 1:
			pt_im_ltm_ltm_do_test_3_1( type2, type3, type4 );
			break;
		case 2:
			pt_im_ltm_ltm_do_test_3_2( type2, type3, type4 );
			break;
		case 3:
			pt_im_ltm_ltm_do_test_3_3( type2, type3, type4 );
			break;
		case 4:
			pt_im_ltm_ltm_do_test_3_4( type2, type3, type4 );
			break;
		case 5:
			pt_im_ltm_ltm_do_test_3_5( type2, type3, type4 );
			break;
		case 6:
			pt_im_ltm_ltm_do_test_3_6( type2, type3, type4 );
			break;
		case 7:
			pt_im_ltm_ltm_do_test_3_7( type2, type3, type4 );
			break;
		case 8:
			pt_im_ltm_ltm_do_test_3_8( type2, type3, type4 );
			break;
		case 9:
			pt_im_ltm_ltm_do_test_3_9( type2, type3, type4 );
			break;
		case 10:
			pt_im_ltm_ltm_do_test_3_10( type2, type3, type4 );
			break;
		case 11:
			pt_im_ltm_ltm_do_test_3_11( type2, type3, type4 );
			break;
		case 12:
			pt_im_ltm_ltm_do_test_3_12( type2, type3, type4 );
			break;
		case 13:
			pt_im_ltm_ltm_do_test_3_13( type2, type3, type4 );
			break;
		case 14:
			pt_im_ltm_ltm_do_test_3_14( type2, type3, type4 );
			break;
		case 15:
			pt_im_ltm_ltm_do_test_3_15( type2, type3, type4 );
			break;
		default:
			Ddim_Print(("*** LTM LTM test skip\n" ));
			break;
	}
}



#undef D_IM_LTM_TEST_NO
#define D_IM_LTM_TEST_NO "9_"
static VOID pt_im_ltm_ltm_run_9( const UINT32 type1 )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6c;

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO "%u begin\n", type2 ));


	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	// テストコード起動
	Ddim_Print(( "=-=-=-=-\n" ));
	Ddim_Print(( "* LTM LTM begin CT_Im_Ltm_Ltm_1_%u\n", type2 ));
	CT_Im_LTM_LTM_Run( 1, type2, 0 );
	Ddim_Print(( "* LTM LTM end CT_Im_Ltm_Ltm_1_%u\n", type2 ));

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "*** LTM LTM test " D_IM_LTM_TEST_NO "%u end\n", type2 ));
}


VOID pt_im_ltm_ltm_run( VOID )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6b;

	switch( type2 ) {
		case 1:
			pt_im_ltm_ltm_run_1( type2 );
			break;
		case 2:
			pt_im_ltm_ltm_run_2( type2 );
			break;
		case 3:
			pt_im_ltm_ltm_run_3( type2 );
			break;
		case 9:
			pt_im_ltm_ltm_run_9( type2 );
			break;
		default:
			Ddim_Print(("*** LTM LTM test skip\n" ));
			break;
	}
}


VOID Pt_Im_LTM_Main( VOID )
{
	UCHAR			type1;

	// 開始...
	Ddim_Print(( "****** LTM begin Pt_Im_LTM_Main\n" ));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	type1	= gDDIM_Info.com._6a;

	Dd_Top_Set_CLKSTOP11_LTM1CK( 1 );
	Dd_Top_Set_CLKSEL4_LTM1CLK( 0 );	// 300MHz
	Dd_Top_Set_CLKSTOP11_LTM1CK( 0 );
	Dd_Top_Set_CLKSTOP13_LTM2CK( 1 );
	Dd_Top_Set_CLKSEL6_LTM2CLK( 0 );	// 300MHz
	Dd_Top_Set_CLKSTOP13_LTM2CK( 0 );


	Ddim_Print(( "*** LTM begin Im_LTM_Init\n" ));
	Im_LTM_Init_All_Block( D_IM_LTM_PIPE12 );
	Ddim_Print(( "*** LTM end Im_LTM_Init\n" ));

	pt_im_ltm_enable_gic();
	pt_im_ltm_start_clock();

	switch( type1 ) {
		case 1:
			pt_im_ltm_rbk_run();
			break;
		case 2:
			pt_im_ltm_map_run();
			break;
		case 3:
			pt_im_ltm_ltm_run();
			break;
		default:
			Ddim_Print(("*** LTM test skip\n" ));
			break;
	}

	// タイマーログ出力
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );


	Ddim_Print(( "(CLKSEL-4)\n" ));
	Ddim_Print(( "LTM1CLK = %d\n", Dd_Top_Get_CLKSEL4_LTM1CLK() ));
	Ddim_Print(( "(CLKSEL-5)\n" ));
	Ddim_Print(( "LTMRBK1CLK = %d\n", Dd_Top_Get_CLKSEL5_LTMRBK1CLK() ));
	Ddim_Print(( "(CLKSTOP-10)\n" ));
	Ddim_Print(( "LTMRBK1CK = %d\n", Dd_Top_Get_CLKSTOP10_LTMRBK1CK() ));
	Ddim_Print(( "(CLKSTOP-11)\n" ));
	Ddim_Print(( "LTM1CK = %d\n", Dd_Top_Get_CLKSTOP11_LTM1CK() ));
	Ddim_Print(( "LTM1AP = %d\n", Dd_Top_Get_CLKSTOP11_LTM1AP() ));
	Ddim_Print(( "LTM1AH = %d\n", Dd_Top_Get_CLKSTOP11_LTM1AH() ));
	Ddim_Print(( "LTM1AX = %d\n", Dd_Top_Get_CLKSTOP11_LTM1AX() ));
	Ddim_Print(( "(CLKSEL-6)\n" ));
	Ddim_Print(( "LTM2CLK = %d\n", Dd_Top_Get_CLKSEL6_LTM2CLK() ));
	Ddim_Print(( "(CLKSEL-7)\n" ));
	Ddim_Print(( "LTMRBK2CLK = %d\n", Dd_Top_Get_CLKSEL7_LTMRBK2CLK() ));
	Ddim_Print(( "(CLKSTOP-12)\n" ));
	Ddim_Print(( "LTMRBK2CK = %d\n", Dd_Top_Get_CLKSTOP12_LTMRBK2CK() ));
	Ddim_Print(( "(CLKSTOP-13)\n" ));
	Ddim_Print(( "LTM2CK = %d\n", Dd_Top_Get_CLKSTOP13_LTM2CK() ));
	Ddim_Print(( "LTM2AP = %d\n", Dd_Top_Get_CLKSTOP13_LTM2AP() ));
	Ddim_Print(( "LTM2AH = %d\n", Dd_Top_Get_CLKSTOP13_LTM2AH() ));
	Ddim_Print(( "LTM2AX = %d\n", Dd_Top_Get_CLKSTOP13_LTM2AX() ));
	Ddim_Print(( "****** LTM end Pt_Im_LTM_Main\n" ));

	// テスト強制終了処理
	Palladium_Force_Stop();
}

