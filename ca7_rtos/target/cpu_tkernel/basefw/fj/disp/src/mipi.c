/**
 * @file		mipi.c
 * @brief		MIPI-DSI Command and Image control
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "mipi.h"
#include "osd_draw_comm.h"
#include "osd_attr.h"
#include "debug.h"
#include "dd_arm.h"
#include "dd_hdmac0.h"
#include "im_disp.h"
#include "im_mipi.h"

//#define DBG_MIPI_PROC_TIME
#ifdef DBG_MIPI_PROC_TIME
#include "measure_time.h"
#endif

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
#define D_DISP_MIPI_CMD_PARAM_MAX	14		/* parameter maximum number	*/
#define D_DISP_MIPI_CMD_ID_MAX		16		/* command maximum number	*/

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/** MIPI Clock Frequency */
typedef enum {
	E_BF_MIPI_CLK_150 = 0,	/**< 150MHz		*/	// 20MHz * 30 / 4 
	E_BF_MIPI_CLK_155,		/**< 155MHz		*/	// 20MHz * 31 / 4 
	E_BF_MIPI_CLK_160,		/**< 160MHz		*/	// 20MHz * 32 / 4 
	E_BF_MIPI_CLK_165,		/**< 165MHz		*/	// 20MHz * 33 / 4 
	E_BF_MIPI_CLK_170,		/**< 170MHz		*/	// 20MHz * 34 / 4 
	E_BF_MIPI_CLK_175,		/**< 175MHz		*/	// 20MHz * 35 / 4 
	E_BF_MIPI_CLK_180,		/**< 180MHz		*/	// 20MHz * 36 / 4 
	E_BF_MIPI_CLK_185,		/**< 185MHz		*/	// 20MHz * 37 / 4 
	E_BF_MIPI_CLK_190,		/**< 190MHz		*/	// 20MHz * 38 / 4 
	E_BF_MIPI_CLK_195,		/**< 195MHz		*/	// 20MHz * 39 / 4 
	E_BF_MIPI_CLK_200,		/**< 200MHz		*/	// 20MHz * 40 / 4 
	E_BF_MIPI_CLK_205,		/**< 205MHz		*/	// 20MHz * 41 / 4 
	E_BF_MIPI_CLK_210,		/**< 210MHz		*/	// 20MHz * 42 / 4 
	E_BF_MIPI_CLK_215,		/**< 215MHz		*/	// 20MHz * 43 / 4 
	E_BF_MIPI_CLK_220,		/**< 220MHz		*/	// 20MHz * 44 / 4 
	E_BF_MIPI_CLK_225,		/**< 225MHz		*/	// 20MHz * 45 / 4 
	E_BF_MIPI_CLK_230,		/**< 230MHz		*/	// 20MHz * 46 / 4 
	E_BF_MIPI_CLK_235,		/**< 235MHz		*/	// 20MHz * 47 / 4 
	E_BF_MIPI_CLK_240,		/**< 240MHz		*/	// 20MHz * 48 / 4 
	E_BF_MIPI_CLK_245,		/**< 245MHz		*/	// 20MHz * 49 / 4 
	E_BF_MIPI_CLK_250,		/**< 250MHz		*/	// 20MHz * 50 / 4 
	E_BF_MIPI_CLK_255,		/**< 255MHz		*/	// 20MHz * 51 / 4 
	E_BF_MIPI_CLK_260,		/**< 260MHz		*/	// 20MHz * 52 / 4 
	E_BF_MIPI_CLK_265,		/**< 265MHz		*/	// 20MHz * 53 / 4 
	E_BF_MIPI_CLK_270,		/**< 270MHz		*/	// 20MHz * 54 / 4 
	E_BF_MIPI_CLK_275,		/**< 275MHz		*/	// 20MHz * 55 / 4 
	E_BF_MIPI_CLK_280,		/**< 280MHz		*/	// 20MHz * 56 / 4 
	E_BF_MIPI_CLK_285,		/**< 285MHz		*/	// 20MHz * 57 / 4 
	E_BF_MIPI_CLK_290,		/**< 290MHz		*/	// 20MHz * 58 / 4 
	E_BF_MIPI_CLK_295,		/**< 295MHz		*/	// 20MHz * 59 / 4 
	E_BF_MIPI_CLK_300,		/**< 300MHz		*/	// 20MHz * 60 / 4 
	E_BF_MIPI_CLK_305,		/**< 305MHz		*/	// 20MHz * 61 / 4 
	E_BF_MIPI_CLK_310,		/**< 310MHz		*/	// 20MHz * 62 / 4 
	E_BF_MIPI_CLK_315,		/**< 315MHz		*/	// 20MHz * 63 / 4 
	E_BF_MIPI_CLK_320,		/**< 320MHz		*/	// 20MHz * 64 / 4 
	E_BF_MIPI_CLK_325,		/**< 325MHz		*/	// 20MHz * 65 / 4 
	E_BF_MIPI_CLK_330,		/**< 330MHz		*/	// 20MHz * 66 / 4 
	E_BF_MIPI_CLK_335,		/**< 335MHz		*/	// 20MHz * 67 / 4 
	E_BF_MIPI_CLK_340,		/**< 340MHz		*/	// 20MHz * 68 / 4 
	E_BF_MIPI_CLK_345,		/**< 345MHz		*/	// 20MHz * 69 / 4 
	E_BF_MIPI_CLK_350,		/**< 350MHz		*/	// 20MHz * 70 / 4 
	E_BF_MIPI_CLK_355,		/**< 355MHz		*/	// 20MHz * 71 / 4 
	E_BF_MIPI_CLK_360,		/**< 360MHz		*/	// 20MHz * 72 / 4 
	E_BF_MIPI_CLK_365,		/**< 365MHz		*/	// 20MHz * 73 / 4
	E_BF_MIPI_CLK_370,		/**< 370MHz		*/	// 20MHz * 74 / 4
	E_BF_MIPI_CLK_375,		/**< 375MHz		*/	// 20MHz * 75 / 4
} E_BF_MIPI_CLK;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/** PHY_TMR_LPCLK_CFG Table */
typedef struct {
	UCHAR phy_clkhs2lp_time;
	UCHAR phy_clklp2hs_time;
} T_BF_MIPI_PHY_TMR_LPCLK_CFG;

/** PHY_TMR_CFG Table */
typedef struct {
	UCHAR phy_lp2hs_time;
	UCHAR phy_hs2lp_time;
} T_BF_MIPI_PHY_TMR_CFG;

/** PHY_SETUP_CL Table */
typedef struct {
	UCHAR sap_tlpx_c;
	UCHAR sap_hs0_c;
	UCHAR sap_trail_c;
	UCHAR sap_pre_c;
} T_BF_MIPI_PHY_SETUP_CL;

/** PHY_SETUP_DL Table */
typedef struct {
	UCHAR sap_tlpx_d;
	UCHAR sap_hs0_d;
	UCHAR sap_trail_d;
	UCHAR sap_pre_d;
} T_BF_MIPI_PHY_SETUP_DL;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/** MIPI Clock */
static volatile UCHAR	gBF_Mipi_Clk = E_BF_MIPI_CLK_375;

/** PHY_TMR_LPCLK_CFG Table */
static volatile T_BF_MIPI_PHY_TMR_LPCLK_CFG	gBF_Mipi_Phy_Tmr_Lpclk_Cfg[] = {
//	  phy_clkhs2lp_time		phy_clklp2hs_time
	{ 15,					12 },	// 150MHz
	{ 15,					12 },	// 155MHz
	{ 15,					12 },	// 160MHz
	{ 16,					13 },	// 165MHz
	{ 16,					13 },	// 170MHz
	{ 16,					13 },	// 175MHz
	{ 16,					13 },	// 180MHz
	{ 16,					13 },	// 185MHz
	{ 16,					13 },	// 190MHz
	{ 16,					13 },	// 195MHz
	{ 16,					13 },	// 200MHz
	{ 17,					16 },	// 205MHz
	{ 17,					16 },	// 210MHz
	{ 17,					16 },	// 215MHz
	{ 17,					16 },	// 220MHz
	{ 17,					16 },	// 225MHz
	{ 17,					16 },	// 230MHz
	{ 17,					16 },	// 235MHz
	{ 17,					16 },	// 240MHz
	{ 17,					16 },	// 245MHz
	{ 17,					16 },	// 250MHz
	{ 17,					23 },	// 255MHz
	{ 17,					23 },	// 260MHz
	{ 17,					23 },	// 265MHz
	{ 17,					23 },	// 270MHz
	{ 17,					23 },	// 275MHz
	{ 17,					23 },	// 280MHz
	{ 17,					23 },	// 285MHz
	{ 17,					23 },	// 290MHz
	{ 17,					23 },	// 295MHz
	{ 17,					23 },	// 300MHz
	{ 17,					23 },	// 305MHz
	{ 17,					23 },	// 310MHz
	{ 17,					23 },	// 315MHz
	{ 17,					23 },	// 320MHz
	{ 18,					23 },	// 325MHz
	{ 18,					23 },	// 330MHz
	{ 18,					23 },	// 335MHz
	{ 18,					23 },	// 340MHz
	{ 18,					23 },	// 345MHz
	{ 18,					23 },	// 350MHz
	{ 18,					24 },	// 355MHz
	{ 18,					24 },	// 360MHz
	{ 18,					24 },	// 365MHz
	{ 18,					24 },	// 370MHz
	{ 18,					24 },	// 375MHz
};

/** PHY_TMR_CFG Table */
static volatile T_BF_MIPI_PHY_TMR_CFG	gBF_Mipi_Phy_Tmr_Cfg[] = {
//	  phy_lp2hs_time		phy_hs2lp_time
	{ 8,					7  },	// 150MHz
	{ 8,					7  },	// 155MHz
	{ 8,					7  },	// 160MHz
	{ 9,					7  },	// 165MHz
	{ 9,					7  },	// 170MHz
	{ 9,					7  },	// 175MHz
	{ 9,					7  },	// 180MHz
	{ 9,					7  },	// 185MHz
	{ 9,					7  },	// 190MHz
	{ 9,					7  },	// 195MHz
	{ 9,					7  },	// 200MHz
	{ 10,					7  },	// 205MHz
	{ 10,					7  },	// 210MHz
	{ 10,					7  },	// 215MHz
	{ 10,					7  },	// 220MHz
	{ 10,					9  },	// 225MHz
	{ 10,					9  },	// 230MHz
	{ 10,					9  },	// 235MHz
	{ 10,					9  },	// 240MHz
	{ 10,					9  },	// 245MHz
	{ 10,					9  },	// 250MHz
	{ 12,					9  },	// 255MHz
	{ 12,					9  },	// 260MHz
	{ 12,					9  },	// 265MHz
	{ 12,					9  },	// 270MHz
	{ 12,					9  },	// 275MHz
	{ 12,					9  },	// 280MHz
	{ 12,					9  },	// 285MHz
	{ 12,					9  },	// 290MHz
	{ 13,					9  },	// 295MHz
	{ 13,					9  },	// 300MHz
	{ 13,					9  },	// 305MHz
	{ 13,					9  },	// 310MHz
	{ 13,					9  },	// 315MHz
	{ 13,					9  },	// 320MHz
	{ 13,					12 },	// 325MHz
	{ 13,					12 },	// 330MHz
	{ 13,					12 },	// 335MHz
	{ 13,					12 },	// 340MHz
	{ 13,					12 },	// 345MHz
	{ 13,					12 },	// 350MHz
	{ 15,					12 },	// 355MHz
	{ 15,					12 },	// 360MHz
	{ 15,					12 },	// 365MHz
	{ 15,					12 },	// 370MHz
	{ 15,					12 },	// 375MHz
};

/** PHY_SETUP_CL Table */
static volatile T_BF_MIPI_PHY_SETUP_CL	gBF_Mipi_Phy_Setup_Cl[] = {
//	  SAP_TLPX_C	SAP_HS0_C	SAP_TRAIL_C	SAP_PRE_C
	{ 2,			6,			2,			11 },	// 150MHz
	{ 2,			6,			2,			11 },	// 155MHz
	{ 2,			6,			2,			11 },	// 160MHz
	{ 2,			7,			3,			10 },	// 165MHz
	{ 2,			7,			3,			10 },	// 170MHz
	{ 2,			7,			3,			10 },	// 175MHz
	{ 2,			7,			3,			10 },	// 180MHz
	{ 2,			7,			3,			10 },	// 185MHz
	{ 2,			7,			3,			10 },	// 190MHz
	{ 2,			7,			3,			10 },	// 195MHz
	{ 2,			7,			3,			10 },	// 200MHz
	{ 3,			9,			4,			9  },	// 205MHz
	{ 3,			9,			4,			9  },	// 210MHz
	{ 3,			9,			4,			9  },	// 215MHz
	{ 3,			9,			4,			9  },	// 220MHz
	{ 3,			9,			4,			9  },	// 225MHz
	{ 3,			9,			4,			9  },	// 230MHz
	{ 3,			9,			4,			9  },	// 235MHz
	{ 3,			9,			4,			9  },	// 240MHz
	{ 3,			9,			4,			9  },	// 245MHz
	{ 3,			9,			4,			9  },	// 250MHz
	{ 3,			14,			4,			8  },	// 255MHz
	{ 3,			14,			4,			8  },	// 260MHz
	{ 3,			14,			4,			8  },	// 265MHz
	{ 3,			14,			4,			8  },	// 270MHz
	{ 3,			14,			4,			8  },	// 275MHz
	{ 3,			14,			4,			8  },	// 280MHz
	{ 3,			14,			4,			8  },	// 285MHz
	{ 3,			14,			4,			8  },	// 290MHz
	{ 4,			14,			4,			7  },	// 295MHz
	{ 4,			14,			4,			7  },	// 300MHz
	{ 4,			14,			4,			7  },	// 305MHz
	{ 4,			14,			4,			7  },	// 310MHz
	{ 4,			14,			4,			7  },	// 315MHz
	{ 4,			14,			4,			7  },	// 320MHz
	{ 4,			14,			4,			7  },	// 325MHz
	{ 4,			14,			4,			7  },	// 330MHz
	{ 4,			14,			4,			7  },	// 335MHz
	{ 4,			14,			4,			7  },	// 340MHz
	{ 4,			14,			4,			7  },	// 345MHz
	{ 4,			14,			4,			7  },	// 350MHz
	{ 4,			14,			4,			6  },	// 355MHz
	{ 4,			14,			4,			6  },	// 360MHz
	{ 4,			14,			4,			6  },	// 365MHz
	{ 4,			14,			4,			6  },	// 370MHz
	{ 4,			14,			4,			6  },	// 375MHz
};

/** PHY_SETUP_DL Table */
static volatile T_BF_MIPI_PHY_SETUP_DL	gBF_Mipi_Phy_Setup_Dl[] = {
//	  SAP_TLPX_D	SAP_HS0_D	SAP_TRAIL_D	SAP_PRE_D
	{ 2,			5,			3,			14 },	// 150MHz
	{ 2,			5,			3,			14 },	// 155MHz
	{ 2,			5,			3,			14 },	// 160MHz
	{ 2,			6,			3,			13 },	// 165MHz
	{ 2,			6,			3,			13 },	// 170MHz
	{ 2,			6,			3,			13 },	// 175MHz
	{ 2,			6,			3,			13 },	// 180MHz
	{ 2,			6,			3,			13 },	// 185MHz
	{ 2,			6,			3,			13 },	// 190MHz
	{ 2,			6,			3,			13 },	// 195MHz
	{ 2,			6,			3,			13 },	// 200MHz
	{ 2,			7,			3,			12 },	// 205MHz
	{ 2,			7,			3,			12 },	// 210MHz
	{ 2,			7,			3,			12 },	// 215MHz
	{ 2,			7,			3,			12 },	// 220MHz
	{ 2,			7,			4,			12 },	// 225MHz
	{ 2,			7,			4,			12 },	// 230MHz
	{ 2,			7,			4,			12 },	// 235MHz
	{ 2,			7,			4,			12 },	// 240MHz
	{ 2,			7,			4,			12 },	// 245MHz
	{ 2,			7,			4,			12 },	// 250MHz
	{ 3,			7,			4,			10 },	// 255MHz
	{ 3,			7,			4,			10 },	// 260MHz
	{ 3,			7,			4,			10 },	// 265MHz
	{ 3,			7,			4,			10 },	// 270MHz
	{ 3,			7,			4,			10 },	// 275MHz
	{ 3,			7,			4,			10 },	// 280MHz
	{ 3,			7,			4,			10 },	// 285MHz
	{ 3,			7,			4,			10 },	// 290MHz
	{ 3,			8,			4,			9  },	// 295MHz
	{ 3,			8,			4,			9  },	// 300MHz
	{ 3,			8,			4,			9  },	// 305MHz
	{ 3,			8,			4,			9  },	// 310MHz
	{ 3,			8,			4,			9  },	// 315MHz
	{ 3,			8,			4,			9  },	// 320MHz
	{ 3,			8,			5,			9  },	// 325MHz
	{ 3,			8,			5,			9  },	// 330MHz
	{ 3,			8,			5,			9  },	// 335MHz
	{ 3,			8,			5,			9  },	// 340MHz
	{ 3,			8,			5,			9  },	// 345MHz
	{ 3,			8,			5,			9  },	// 350MHz
	{ 3,			10,			5,			9  },	// 355MHz
	{ 3,			10,			5,			9  },	// 360MHz
	{ 3,			10,			5,			9  },	// 365MHz
	{ 3,			10,			5,			9  },	// 370MHz
	{ 3,			10,			5,			9  },	// 375MHz
};

/** PHY_TCLKPOST Table */
static volatile UCHAR	gBF_Mipi_Phy_Tclkpost[] = {
	13,	// 150MHz
	13,	// 155MHz
	13,	// 160MHz
	13,	// 165MHz
	13,	// 170MHz
	13,	// 175MHz
	13,	// 180MHz
	13,	// 185MHz
	13,	// 190MHz
	13,	// 195MHz
	13,	// 200MHz
	13,	// 205MHz
	13,	// 210MHz
	13,	// 215MHz
	13,	// 220MHz
	14,	// 225MHz
	14,	// 230MHz
	14,	// 235MHz
	14,	// 240MHz
	14,	// 245MHz
	14,	// 250MHz
	14,	// 255MHz
	14,	// 260MHz
	14,	// 265MHz
	14,	// 270MHz
	14,	// 275MHz
	14,	// 280MHz
	14,	// 285MHz
	14,	// 290MHz
	14,	// 295MHz
	14,	// 300MHz
	14,	// 305MHz
	14,	// 310MHz
	14,	// 315MHz
	14,	// 320MHz
	16,	// 325MHz
	16,	// 330MHz
	16,	// 335MHz
	16,	// 340MHz
	16,	// 345MHz
	16,	// 350MHz
	16,	// 355MHz
	16,	// 360MHz
	16,	// 365MHz
	16,	// 370MHz
	16,	// 375MHz
};
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
#define im_mipi_wait_us( usec )				Dd_ARM_Wait_ns( usec * 1000 )

/*----------------------------------------------------------------------*/
/* Local Function Prototype Definition                                  */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
static FJ_ERR_CODE mipi_disp_init( UCHAR freq_ctrl )
{
	INT32	result = D_DDIM_OK;
	UINT32	lcd_status;

	// Clock Setting(MIFCLK)
	// Stop MIF clock (for set CLKSEL)
	Dd_Top_Set_CLKSTOP3_MIFCK( 1 );
	// Set P05CHG.
	if( Dd_Top_Get_PLLCNT9_P05CHG() != freq_ctrl ){
		Dd_Top_Stop_Pll05A();
		Dd_Top_Set_PLLCNT9_P05CHG(freq_ctrl);
		result = Dd_Top_Start_Pll05A();
		if( result != D_DDIM_OK ){
			BF_Debug_Print_Error(( "mipi_disp_init() Error!!:Dd_Top_Start_Pll05A() error. result=%x\n", result ));
			return FJ_ERR_NG;
		}
	}
	// Change MIF clock
	Dd_Top_Set_CLKSEL2_MIFCLK(E_BF_MIFCLK_SEL_49_5);	// 49.5MHz(=1188MHz/24)

	// DSI_PLLCLK Set(MIPICLK=500MHz)
	Dd_Top_Stop_Pll08();
	Dd_Top_Set_PLLCNT9_P08PLLDIV(0x18);
	result = Dd_Top_Start_Pll08();
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "mipi_disp_init() Error!!:Dd_Top_Start_Pll08() error. result=%x\n", result ));
		return FJ_ERR_NG;
	}
	// Start MIF clock
	Dd_Top_Set_CLKSTOP3_MIFCK( 0 );

	Im_DISP_Get_Status( NULL, &lcd_status );
	if( (lcd_status & E_IM_DISP_SEL_LAYER_DCORE) == E_IM_DISP_SEL_LAYER_DCORE ){	// DCORE ACT
		// TRG stop
		result = Im_DISP_Stop(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_DCORE, 0, E_IM_DISP_STOP_TYPE_WITH_WAIT);
		if( result != D_DDIM_OK ){
			Ddim_Print(("mipi_disp_init() : Im_DISP_Stop DCORE STOP Error\n"));
			return result;
		}
	}

	// DISP macro soft-reset (for display switch)
	result = Im_DISP_Set_SR( E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_DCORE, D_IM_DISP_SR_RESET );
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "mipi_disp_init() Error!!:Im_DISP_Set_SR() error. result=%x\n", result ));
		return FJ_ERR_NG;
	}

	// IFS Set(MIPI-DSI)
	result = BF_Osd_Attr_Set_Display_Interface( FJ_DISP_TYPE_LCD, E_FJ_DISP_IFS_MIPI );
	if( result != FJ_ERR_OK ){
		BF_Debug_Print_Error(( "mipi_disp_init() Error!!:BF_Osd_Attr_Set_Display_Interface() error. result=%x\n", result ));
		return FJ_ERR_NG;
	}

	// DISP macro to clear soft-reset bit
	Im_DISP_Init();

	return D_DDIM_OK;
}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
static FJ_ERR_CODE mipi_disp_init( UCHAR freq_ctrl, E_BF_MIPI_CLK mipiclk )
{
	INT32	result = D_DDIM_OK;
	UCHAR	p08plldiv;
	UINT32	lcd_status;

	// Clock Setting(MIFCLK)
	// Stop MIF clock (for set CLKSEL)
	Dd_Top_Set_CLKSTOP3_MIFCK( 1 );
	// Set P05CHG.
	if( Dd_Top_Get_PLLCNT9_P05CHG() != freq_ctrl ){
		Dd_Top_Stop_Pll05A();
		Dd_Top_Set_PLLCNT9_P05CHG(freq_ctrl);
		result = Dd_Top_Start_Pll05A();
		if( result != D_DDIM_OK ){
			BF_Debug_Print_Error(( "mipi_disp_init() Error!!:Dd_Top_Start_Pll05A() error. result=0x%x\n", result ));
			return FJ_ERR_NG;
		}
	}
	// Change MIF clock
	Dd_Top_Set_CLKSEL2_MIFCLK(E_BF_MIFCLK_SEL_49_5);	// 49.5MHz(=1188MHz/24): MIFCLK = MIPICLK / 8bit / 3(RGB), Ex) 12.5MHz = 300MHz / 8 / 3

	// DSI_PLLCLK Set
	Dd_Top_Stop_Pll08();

	if( mipiclk >= E_BF_MIPI_CLK_150 && mipiclk <= E_BF_MIPI_CLK_375 ){
		Dd_Top_Set_CLKSEL7_MIPISEL( 3 );	// 150MHz(N=30) - 375MHz(N=75)
		p08plldiv = 0x1d + mipiclk;
	}
	else{
		BF_Debug_Print_Error(( "mipi_disp_init() Error!!:MIPICLK error. mipiclk=%d\n", mipiclk ));
		return FJ_ERR_NG;
	}

	Dd_Top_Set_PLLCNT9_P08PLLDIV(p08plldiv);
	result = Dd_Top_Start_Pll08();
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "mipi_disp_init() Error!!:Dd_Top_Start_Pll08() error. result=0x%x\n", result ));
		return FJ_ERR_NG;
	}
	// Start MIF clock
	Dd_Top_Set_CLKSTOP3_MIFCK( 0 );

	Im_DISP_Get_Status( NULL, &lcd_status );
#ifdef DBG_MIPI_PROC_TIME
	printf( "lcd_status = 0x%08X\n", lcd_status );
#endif	// debug end
	if( lcd_status != E_IM_DISP_SEL_LAYER_NONE ){
		// TRG stop
		result = Im_DISP_Stop(E_IM_DISP_SEL_LAYER_NONE, lcd_status, 0, E_IM_DISP_STOP_TYPE_WITHOUT_WAIT);
		if( result != D_DDIM_OK ){
			Ddim_Print(("mipi_disp_init() : Im_DISP_Stop Error (result=%d, lcd_status=0x%08X)\n", result, lcd_status));
			return result;
		}
	}

	// DISP macro soft-reset (for display switch)
	result = Im_DISP_Set_SR( E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_DCORE, D_IM_DISP_SR_RESET );
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "mipi_disp_init() Error!!:Im_DISP_Set_SR() error. result=0x%x\n", result ));
		return FJ_ERR_NG;
	}

	// IFS Set(MIPI-DSI)
	result = BF_Osd_Attr_Set_Display_Interface( FJ_DISP_TYPE_LCD, E_FJ_DISP_IFS_MIPI );
	if( result != FJ_ERR_OK ){
		BF_Debug_Print_Error(( "mipi_disp_init() Error!!:BF_Osd_Attr_Set_Display_Interface() error. result=0x%x\n", result ));
		return FJ_ERR_NG;
	}

	// DISP macro to clear soft-reset bit
	Im_DISP_Init();

	return D_DDIM_OK;
}

// Set MIPI clock.
static VOID mipi_set_clk(E_BF_MIPI_CLK mipiclk)
{
	gBF_Mipi_Clk = mipiclk;
}

// Get MIPI clock.
static E_BF_MIPI_CLK	mipi_get_clk()
{
	return gBF_Mipi_Clk;
}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
/* Set MIPI timing generator to MIPI macro.  */
FJ_ERR_CODE fj_mipi_video_ctrl( FJ_MIPI_CTRL_VIDEO* mipi_video_ctrl )
{
	INT32								result = D_DDIM_OK;
	T_IM_MIPI_DPI_RESOLUTION			mipi_param;
	T_IM_MIPI_TRANS_CONFIG				mipi_trans_cfg;

	if( mipi_video_ctrl == NULL ){
		BF_Debug_Print_Error(( "fj_mipi_video_ctrl() Error!!:mipi_video_ctrl is NULL.\n" ));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Set DPI Resolution data.
	memset( &mipi_param, 0, sizeof(T_IM_MIPI_DPI_RESOLUTION) );
	result = Im_MIPI_Get_DPI_Resolution(&mipi_param);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_video_ctrl() Error!!:Im_MIPI_Get_DPI_Resolution() error. result=%x\n", result ));
		return FJ_ERR_NG;
	}

	mipi_param.lane					= mipi_video_ctrl->lane;
	mipi_param.vcid					= 0x00;
	mipi_param.color_coding			= mipi_video_ctrl->color_coding;
	mipi_param.cfg_pol.word			= 0x06;
	mipi_param.vid_pkt_size			= mipi_video_ctrl->vid_pkt_size;
	mipi_param.vid_hsa_time			= mipi_video_ctrl->vid_hsa_time;
	mipi_param.vid_hbp_time			= mipi_video_ctrl->vid_hbp_time;
	mipi_param.vid_hline_time		= mipi_video_ctrl->vid_hline_time;
	mipi_param.vid_vsa_lines		= mipi_video_ctrl->vid_vsa_lines;
	mipi_param.vid_vbp_lines		= mipi_video_ctrl->vid_vbp_lines;
	mipi_param.vid_vfp_lines		= mipi_video_ctrl->vid_vfp_lines;
	mipi_param.vid_vactive_lines	= mipi_video_ctrl->vid_vactive_lines;
	result = Im_MIPI_Set_DPI_Resolution(&mipi_param);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_video_ctrl() Error!!:Im_MIPI_Set_DPI_Resolution() error. result=%x\n", result ));
		return FJ_ERR_NG;
	}

	// Set Transmission Configuration.
	memset( &mipi_trans_cfg, 0, sizeof(T_IM_MIPI_TRANS_CONFIG) );
	result = Im_MIPI_Get_Transmission_Configuration(&mipi_trans_cfg);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_video_ctrl() Error!!:Im_MIPI_Get_Transmission_Configuration() error. result=%x\n", result ));
		return FJ_ERR_NG;
	}
	// PHY_TMR_LPCLK
	mipi_trans_cfg.phy_tmr_lpclk_cfg.word	= 0x00210015;
	// PHY_TMR
	mipi_trans_cfg.phy_tmr_cfg.word			= 0x0e130000;
	result = Im_MIPI_Set_Transmission_Configuration(&mipi_trans_cfg);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_video_ctrl() Error!!:Im_MIPI_Set_Transmission_Configuration() error. result=%x\n", result ));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/* Initialize to MIPI macro. */
FJ_ERR_CODE fj_mipi_init( UCHAR freq_ctrl )
{
	INT32 result				= D_DDIM_OK;
	FJ_ERR_CODE ret				= FJ_ERR_NG;
	T_IM_MIPI_DPI_LANE_INFO		mipi_lane_info;
	T_IM_MIPI_TRANS_CONFIG		mipi_trans_cfg;


	if( freq_ctrl > FJ_DISP_MIPI_FREQ_CTRL_ACTIVE ){
		BF_Debug_Print_Error(( "fj_mipi_init() Error!!:freq_ctrl is illegal.\n" ));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	ret = mipi_disp_init(freq_ctrl);
	if( ret != FJ_ERR_OK ){
		BF_Debug_Print_Error(( "fj_mipi_init() Error!!:mipi_disp_init() error. result=%x\n", result ));
		return FJ_ERR_NG;
	}

	// MIPI-DSI Initializ
	Im_MIPI_Init(E_IM_MIPI_ACTIVE_DATA_LANE_1);
	// D-PHY PPI txrequestclkhs signal(phy_txrequestclkhs=0)
	Im_MIPI_Lpclk_Ctrl(D_IM_MIPI_ENABLE_OFF);

	// Set clock lane / data lane / TCLK POST.
	memset( &mipi_lane_info, 0, sizeof(T_IM_MIPI_DPI_LANE_INFO) );
	// Datalane/Clocklane/Tclkp-post Setting
	// PHY_SETUP_CL(500MHz)
	mipi_lane_info.clock_lane.bit.SAP_TLPX	= 5;
	mipi_lane_info.clock_lane.bit.SAP_HS0	= 21;
	mipi_lane_info.clock_lane.bit.SAP_TRAIL	= 6;
	mipi_lane_info.clock_lane.bit.SAP_PRE	= 5;
	// PHY_SETUP_DL(500MHz)
	mipi_lane_info.data_lane.bit.SAP_TLPX	= 4;
	mipi_lane_info.data_lane.bit.SAP_HS0	= 12;
	mipi_lane_info.data_lane.bit.SAP_TRAIL	= 6;
	mipi_lane_info.data_lane.bit.SAP_PRE	= 7;
	// PHY_TCLKPOST(500MHz)
	mipi_lane_info.tclkp_post				= 18;
	result = Im_MIPI_Set_Lane_Info(&mipi_lane_info);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_init() Error!!:Im_MIPI_Set_Lane_Info() error. result=%x\n", result ));
		return FJ_ERR_NG;
	}

	// Set Transmission Configuration.
	memset( &mipi_trans_cfg, 0, sizeof(T_IM_MIPI_TRANS_CONFIG) );
	result = Im_MIPI_Get_Transmission_Configuration(&mipi_trans_cfg);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_init() Error!!:Im_MIPI_Get_Transmission_Configuration() error. result=%x\n", result ));
		return FJ_ERR_NG;
	}
	// CLKMGR_CFG
	mipi_trans_cfg.clkmgr_cfg.bit.to_clk_division		= 1;
	mipi_trans_cfg.clkmgr_cfg.bit.tx_esc_clk_division	= 7;
	result = Im_MIPI_Set_Transmission_Configuration(&mipi_trans_cfg);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_init() Error!!:Im_MIPI_Set_Transmission_Configuration() error. result=%x\n", result ));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/* Set MIPI timing generator to MIPI macro.  */
FJ_ERR_CODE fj_mipi_video_ctrl( FJ_MIPI_CTRL_VIDEO* mipi_video_ctrl )
{
	INT32								result = D_DDIM_OK;
	T_IM_MIPI_DPI_RESOLUTION			mipi_param;
	T_IM_MIPI_TRANS_CONFIG				mipi_trans_cfg;
	E_BF_MIPI_CLK						mipiclk;

	if( mipi_video_ctrl == NULL ){
		BF_Debug_Print_Error(( "fj_mipi_video_ctrl() Error!!:mipi_video_ctrl is NULL.\n" ));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Set DPI Resolution data.
	memset( &mipi_param, 0, sizeof(T_IM_MIPI_DPI_RESOLUTION) );
	result = Im_MIPI_Get_DPI_Resolution(&mipi_param);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_video_ctrl() Error!!:Im_MIPI_Get_DPI_Resolution() error. result=0x%x\n", result ));
		return FJ_ERR_NG;
	}

	mipi_param.lane					= mipi_video_ctrl->lane;
	mipi_param.vcid					= 0x00;
	mipi_param.color_coding			= mipi_video_ctrl->color_coding;
	mipi_param.cfg_pol.word			= 0x06;
	mipi_param.vid_pkt_size			= mipi_video_ctrl->vid_pkt_size;
	mipi_param.vid_hsa_time			= mipi_video_ctrl->vid_hsa_time;
	mipi_param.vid_hbp_time			= mipi_video_ctrl->vid_hbp_time;
	mipi_param.vid_hline_time		= mipi_video_ctrl->vid_hline_time;
	mipi_param.vid_vsa_lines		= mipi_video_ctrl->vid_vsa_lines;
	mipi_param.vid_vbp_lines		= mipi_video_ctrl->vid_vbp_lines;
	mipi_param.vid_vfp_lines		= mipi_video_ctrl->vid_vfp_lines;
	mipi_param.vid_vactive_lines	= mipi_video_ctrl->vid_vactive_lines;
	result = Im_MIPI_Set_DPI_Resolution(&mipi_param);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_video_ctrl() Error!!:Im_MIPI_Set_DPI_Resolution() error. result=0x%x\n", result ));
		return FJ_ERR_NG;
	}

	// Set Transmission Configuration.
	memset( &mipi_trans_cfg, 0, sizeof(T_IM_MIPI_TRANS_CONFIG) );
	result = Im_MIPI_Get_Transmission_Configuration(&mipi_trans_cfg);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_video_ctrl() Error!!:Im_MIPI_Get_Transmission_Configuration() error. result=0x%x\n", result ));
		return FJ_ERR_NG;
	}

	mipiclk = mipi_get_clk();

	// PHY_TMR_LPCLK_CFG
	mipi_trans_cfg.phy_tmr_lpclk_cfg.bit.phy_clklp2hs_time	= gBF_Mipi_Phy_Tmr_Lpclk_Cfg[mipiclk].phy_clklp2hs_time;
	mipi_trans_cfg.phy_tmr_lpclk_cfg.bit.phy_clkhs2lp_time	= gBF_Mipi_Phy_Tmr_Lpclk_Cfg[mipiclk].phy_clkhs2lp_time;
	// PHY_TMR_CFG
	mipi_trans_cfg.phy_tmr_cfg.bit.phy_hs2lp_time			= gBF_Mipi_Phy_Tmr_Cfg[mipiclk].phy_hs2lp_time;
	mipi_trans_cfg.phy_tmr_cfg.bit.phy_lp2hs_time			= gBF_Mipi_Phy_Tmr_Cfg[mipiclk].phy_lp2hs_time;
	result = Im_MIPI_Set_Transmission_Configuration(&mipi_trans_cfg);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_video_ctrl() Error!!:Im_MIPI_Set_Transmission_Configuration() error. result=0x%x\n", result ));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/* Initialize to MIPI macro. */
FJ_ERR_CODE fj_mipi_init( UCHAR freq_ctrl  )
{
	INT32 result				= D_DDIM_OK;
	FJ_ERR_CODE ret				= FJ_ERR_NG;
	T_IM_MIPI_DPI_LANE_INFO		mipi_lane_info;
	T_IM_MIPI_TRANS_CONFIG		mipi_trans_cfg;
	E_BF_MIPI_CLK mipiclk		= E_BF_MIPI_CLK_300;

#ifdef DBG_MIPI_PROC_TIME
		extern	ULONG BF_Get_Time_us(VOID);
		printf( "\nfj_mipi_init start time = %d(us)\n", (UINT32)BF_Get_Time_us() );
#endif	// debug end

	if( freq_ctrl > FJ_DISP_MIPI_FREQ_CTRL_ACTIVE ){
		BF_Debug_Print_Error(( "fj_mipi_init() Error!!:freq_ctrl is illegal.\n" ));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	ret = mipi_disp_init(freq_ctrl, mipiclk);
	if( ret != FJ_ERR_OK ){
		BF_Debug_Print_Error(( "fj_mipi_init() Error!!:mipi_disp_init() error. result=0x%x\n", result ));
		return FJ_ERR_NG;
	}

	mipi_set_clk( mipiclk );

	// MIPI-DSI Initializ
	Im_MIPI_Init(E_IM_MIPI_ACTIVE_DATA_LANE_1);
	// D-PHY PPI txrequestclkhs signal(phy_txrequestclkhs=0)
	Im_MIPI_Lpclk_Ctrl(D_IM_MIPI_ENABLE_OFF);

	// Set clock lane / data lane / TCLK POST.
	memset( &mipi_lane_info, 0, sizeof(T_IM_MIPI_DPI_LANE_INFO) );
	// Datalane/Clocklane/Tclkp-post Setting
	// PHY_SETUP_CL
	mipi_lane_info.clock_lane.bit.SAP_TLPX	= gBF_Mipi_Phy_Setup_Cl[mipiclk].sap_tlpx_c;
	mipi_lane_info.clock_lane.bit.SAP_HS0	= gBF_Mipi_Phy_Setup_Cl[mipiclk].sap_hs0_c;
	mipi_lane_info.clock_lane.bit.SAP_TRAIL	= gBF_Mipi_Phy_Setup_Cl[mipiclk].sap_trail_c;
	mipi_lane_info.clock_lane.bit.SAP_PRE	= gBF_Mipi_Phy_Setup_Cl[mipiclk].sap_pre_c;
	// PHY_SETUP_DL
	mipi_lane_info.data_lane.bit.SAP_TLPX	= gBF_Mipi_Phy_Setup_Dl[mipiclk].sap_tlpx_d;
	mipi_lane_info.data_lane.bit.SAP_HS0	= gBF_Mipi_Phy_Setup_Dl[mipiclk].sap_hs0_d;
	mipi_lane_info.data_lane.bit.SAP_TRAIL	= gBF_Mipi_Phy_Setup_Dl[mipiclk].sap_trail_d;
	mipi_lane_info.data_lane.bit.SAP_PRE	= gBF_Mipi_Phy_Setup_Dl[mipiclk].sap_pre_d;
	// PHY_TCLKPOST
	mipi_lane_info.tclkp_post				= gBF_Mipi_Phy_Tclkpost[mipiclk];
	result = Im_MIPI_Set_Lane_Info(&mipi_lane_info);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_init() Error!!:Im_MIPI_Set_Lane_Info() error. result=0x%x\n", result ));
		return FJ_ERR_NG;
	}

	// Set Transmission Configuration.
	memset( &mipi_trans_cfg, 0, sizeof(T_IM_MIPI_TRANS_CONFIG) );
	result = Im_MIPI_Get_Transmission_Configuration(&mipi_trans_cfg);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_init() Error!!:Im_MIPI_Get_Transmission_Configuration() error. result=0x%x\n", result ));
		return FJ_ERR_NG;
	}
	// CLKMGR_CFG
	mipi_trans_cfg.clkmgr_cfg.bit.to_clk_division		= 1;	// Unused. Ratio of dividing frequency of clock used with timer in IP.
	mipi_trans_cfg.clkmgr_cfg.bit.tx_esc_clk_division	= 7;	// LP mode clock. MIPICLK / 8 / tx_esc_clk_division <= 20MHz
	result = Im_MIPI_Set_Transmission_Configuration(&mipi_trans_cfg);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_init() Error!!:Im_MIPI_Set_Transmission_Configuration() error. result=0x%x\n", result ));
		return FJ_ERR_NG;
	}
#ifdef DBG_MIPI_PROC_TIME
		printf( "fj_mipi_init end time = %d(us)\n", (UINT32)BF_Get_Time_us() );
#endif	// debug end
	return FJ_ERR_OK;
}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/* MIPI Mode Setting control. */
FJ_ERR_CODE fj_mipi_set_mode( UCHAR mode )
{
	INT32 result				= D_DDIM_OK;
	T_IM_MIPI_DPI_RESOLUTION	mipi_param;

	if( (mode != FJ_DISP_MIPI_MODE_VIDEO) && (mode != FJ_DISP_MIPI_MODE_CMD) ){
		BF_Debug_Print_Error(("fj_mipi_set_mode() Error!!:mode(%d) is illegal.\n", mode));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	// Command(LP MODE Transfer) or Video(HS MODE Transfer) Transfer Mode Seting.
	memset( &mipi_param, 0, sizeof(T_IM_MIPI_DPI_RESOLUTION) );
	result = Im_MIPI_Get_DPI_Resolution(&mipi_param);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_set_mode() Error!!:Im_MIPI_Get_DPI_Resolution() error. result=%x\n", result ));
		return FJ_ERR_NG;
	}

	if( mode == FJ_DISP_MIPI_MODE_CMD ){
		// Command mode.
		mipi_param.cmd_mode_cfg.bit.dcs_lw_tx		= 1;	// CMD_MODE_CFG(Low-power)
		mipi_param.cmd_mode_cfg.bit.dcs_sw_1p_tx	= 1;	// CMD_MODE_CFG(Low-power)
		mipi_param.cmd_mode_cfg.bit.dcs_sw_0p_tx	= 1;	// CMD_MODE_CFG(Low-power)

		mipi_param.mode_cfg = E_IM_MIPI_MODE_CFG_CMD;		// MODE_CFG(Command mode=1)
		result = Im_MIPI_Set_DPI_Resolution(&mipi_param);
		if( result != D_DDIM_OK ){
			BF_Debug_Print_Error(( "fj_mipi_set_mode() Error!!:Im_MIPI_Set_DPI_Resolution() error. result=%x\n", result ));
			return FJ_ERR_NG;
		}
		// stop clock transmission in Clock Lane.
		Im_MIPI_Lpclk_Ctrl(D_IM_MIPI_ENABLE_OFF);			// LPCLK_CTRL(phy_txrequestclkhs=0)
	}
	else {
		// Vodeo mode.
		mipi_param.vid_mode_cfg.word	= 0x00003f02;		// VID_MODE_CFG

		mipi_param.mode_cfg = E_IM_MIPI_MODE_CFG_VIDEO;		// MODE_CFG(Video mode=0)
		result = Im_MIPI_Set_DPI_Resolution(&mipi_param);
		if( result != D_DDIM_OK ){
			BF_Debug_Print_Error(( "fj_mipi_set_mode() Error!!:Im_MIPI_Set_DPI_Resolution() error. result=%x\n", result ));
			return FJ_ERR_NG;
		}
		// start clock transmission in Clock Lane.
		Im_MIPI_Lpclk_Ctrl(D_IM_MIPI_ENABLE_ON);			// LPCLK_CTRL(phy_txrequestclkhs=1)
	}

	OS_User_Dly_Tsk(1);

	return FJ_ERR_OK;
}

/* MIPI Command transfer control. */
FJ_ERR_CODE fj_mipi_command_ctrl( FJ_MIPI_CMD_INFO* mipi_command_info )
{
	UINT32 cmdcnt;
	UCHAR paramcnt;
	UCHAR header_num			= 0;
	UCHAR payload_num			= 0;
	UINT32 pcnt					= 0;
	UINT32 scnt					= 1;
	ULONG data					= 0x00000000;
	ULONG data1					= 0x00000000;
	INT32 result				= D_DDIM_OK;
	U_IM_MIPI_GEN_HDR			header[D_DISP_MIPI_CMD_ID_MAX];			// GEN_HDR
	ULONG						payload[D_DISP_MIPI_CMD_PARAM_MAX];		// GEN_PLD_DATA

	if( mipi_command_info == NULL ){
		BF_Debug_Print_Error(( "fj_mipi_command_ctrl() Error!!:mipi_command_info is NULL.\n" ));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( mipi_command_info->cmd_num > D_DISP_MIPI_CMD_ID_MAX ){
		BF_Debug_Print_Error(( "fj_mipi_command_ctrl() Error!!:cmd_num(%d) is illegal.\n", mipi_command_info->cmd_num ));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	memset( &header[0], 0, sizeof(U_IM_MIPI_GEN_HDR)*D_DISP_MIPI_CMD_ID_MAX );
	memset( &payload[0], '\n', sizeof(ULONG)*D_DISP_MIPI_CMD_PARAM_MAX );

	for( cmdcnt = 0; cmdcnt < mipi_command_info->cmd_num; cmdcnt++ ){
		if( mipi_command_info->cmd_val[cmdcnt].param_size == 0 ){
			// Short Packet(Command Only)
			header[cmdcnt].bit.gen_dt			= E_IM_MIPI_CMD_ONLY;
			header[cmdcnt].bit.gen_wc_lsbyte	= mipi_command_info->cmd_val[cmdcnt].cmd_id;
		}
		else if( mipi_command_info->cmd_val[cmdcnt].param_size == 1 ){
			// Short Packet(Command And Data)
			header[cmdcnt].bit.gen_dt			= E_IM_MIPI_CMD_DATA;
			header[cmdcnt].bit.gen_wc_lsbyte	= mipi_command_info->cmd_val[cmdcnt].cmd_id;
			header[cmdcnt].bit.gen_wc_msbyte	= mipi_command_info->cmd_val[cmdcnt].param[0];
		}
		else {
			// Long Packet
			header[cmdcnt].bit.gen_dt			= E_IM_MIPI_LONGPKT;
			header[cmdcnt].bit.gen_wc_lsbyte	= (mipi_command_info->cmd_val[cmdcnt].param_size+1);

			data = mipi_command_info->cmd_val[cmdcnt].cmd_id;
			scnt = 1;
			for( paramcnt = 0; paramcnt < mipi_command_info->cmd_val[cmdcnt].param_size; paramcnt++ ){
				if( paramcnt < 3 ){
					data1 = (mipi_command_info->cmd_val[cmdcnt].param[paramcnt] << (8*scnt) );
					data = (data|data1);
					scnt++;
					if( (mipi_command_info->cmd_val[cmdcnt].param_size == (paramcnt+1) ) || (scnt == 4) ){
						payload[pcnt] = data;
						pcnt++;
						scnt = 0;
						data = 0x00000000;
					}
				}
				else {
					data1 = (mipi_command_info->cmd_val[cmdcnt].param[paramcnt] << (8*scnt) );
					data = (data|data1);
					scnt++;
					if( (mipi_command_info->cmd_val[cmdcnt].param_size == (paramcnt+1) ) || (scnt == 4) ){
						payload[pcnt] = data;
						pcnt++;
						scnt = 0;
						data = 0x00000000;
					}
				}
			}
		}
	}
	
	header_num	= mipi_command_info->cmd_num;
	payload_num = pcnt;
	// Packet Setting
	result = Im_MIPI_Packet_Transmission(header_num, payload_num, &header[0], &payload[0]);
	if( result != D_DDIM_OK ){
		BF_Debug_Print_Error(( "fj_mipi_command_ctrl() Error!!:Im_MIPI_Packet_Transmission() error. result=%x\n", result ));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}
