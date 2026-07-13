/**
 * @file		im_xch.c
 * @brief		XH2D Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "im_xch.h"

#if defined(CO_ACT_ICLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
#endif
#include "dd_arm.h"

#include "ddim_user_custom.h"
#include "jdsxch.h"

#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// for debug
//#define CO_IM_XCH_DEBUG

#ifdef CO_ACT_PCLOCK
#define CO_ACT_XCH_PCLOCK
#endif //CO_ACT_PCLOCK
#ifdef CO_ACT_ICLOCK
#define CO_ACT_XCH_ICLOCK
#endif //CO_ACT_ICLOCK

#define D_IM_XCH_APCLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP6())
#define D_IM_XCH_AXCLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP6())

#define D_IM_XCH_PCLK_REG_BIT		D_DD_TOP_XCHAP_BIT	// bit1
#define D_IM_XCH_ICLK_REG_BIT		D_DD_TOP_XCHAX_BIT	// bit3

#define D_IM_XCH_ICF_XF0_OFF		(0x0001)			// XCHICF XF0 OFF
#define D_IM_XCH_ICF_XF1_OFF		(0x0010)			// XCHICF XF1 OFF

// XCHCTL0.XTRG.bit.TRG
// XCHCTL1.XTRG.bit.TRG
#define	D_IM_XCH_STOP				(0)
#define	D_IM_XCH_START				(1)
#define	D_IM_XCH_STOPPED			(2)
#define	D_IM_XCH_RUNNING			(3)

// macro
#define Im_XCH_Dsb()				Dd_ARM_Dsb_Pou()

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Synchronous mode when Xch is executed
typedef enum {
	E_IM_XCH_SYNC_TYPE_SYNC = 0,	// Sync Processing
	E_IM_XCH_SYNC_TYPE_ASYNC		// Async Processing
} E_IM_XCH_SYNC_TYPE;

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------

//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	E_IM_XCH_SYNC_TYPE	sync_type;	// Synchronous mode
	UCHAR				xdtype;		// @XDTYPE	: Input data width
	UCHAR				xmd;		// @XMD		: Operation mode
	UCHAR				xfdt;		// @XFDT	: Fill data
	UCHAR				xhcyc;		// @XHCYC	: Horizontal downsampling cycle
	UCHAR				xvcyc;		// @XVCYC	: Vertical downsampling cycle
	ULONG				xhen;		// @XHEN	: Horizontal import data
	ULONG				xven;		// @XVEN	: Vertical import data
	USHORT				xshw;		// @XSHW	: Source image horizontal size
	USHORT				xghw;		// @XGHW	: Global image horizontal size
	USHORT				xdhw;		// @XDHW	: Destination image horizontal size
	USHORT				xdvw;		// @XDVW	: Destination image vertical size
	ULONG				xsa;		// @XSA		: Source image area address
	ULONG				xda;		// @XDA		: Destination image area address
	UCHAR				xhcnt;		// @XHCNT	: Continuance of histogram data acquisition
	VP_CALLBACK			pCallBack;	// Call Back function pointer
} T_IM_XCH_MNG_SAVE;

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Xch control table
static volatile T_IM_XCH_MNG_SAVE	gIM_Xch_Mng_Save[2];

// Clock control counter (0~255)
#ifdef CO_ACT_XCH_PCLOCK
static volatile UCHAR gIM_Xch_Pclk_Ctrl_Cnt = 0;
#endif // CO_ACT_XCH_PCLOCK
#ifdef CO_ACT_XCH_ICLOCK
static volatile UCHAR gIM_Xch_Iclk_Ctrl_Cnt = 0;
#endif // CO_ACT_XCH_ICLOCK

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/
/**
 * @brief			APCLK ON for XCH
 * @param[in]		None
 * @param[out]		None
 * @retval			None
 */
static VOID im_xch_on_pclk( VOID )
{
#ifdef CO_ACT_XCH_PCLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_Xch_Pclk_Ctrl_Cnt, D_IM_XCH_APCLK_REG_ADDR, ~D_IM_XCH_PCLK_REG_BIT );
#endif	// CO_ACT_XCH_PCLOCK
}

/**
 * @brief			APCLK OFF for XCH
 * @param[in]		None
 * @param[out]		None
 * @retval			None
 */
static VOID im_xch_off_pclk( VOID )
{
#ifdef CO_ACT_XCH_PCLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_Xch_Pclk_Ctrl_Cnt, D_IM_XCH_APCLK_REG_ADDR, D_IM_XCH_PCLK_REG_BIT );
#endif	// CO_ACT_XCH_PCLOCK
}

/**
 * @brief			AXCLK ON for XCH
 * @brief			
 * @param[in]		None
 * @param[out]		None
 * @retval			None
 */
static VOID im_xch_on_iclk( VOID )
{
#ifdef CO_ACT_XCH_ICLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_Xch_Iclk_Ctrl_Cnt, D_IM_XCH_AXCLK_REG_ADDR, ~D_IM_XCH_ICLK_REG_BIT );
#endif	// CO_ACT_XCH_ICLOCK
}

/**
 * @brief			AXCLK OFF for XCH
 * @param[in]		None
 * @param[out]		None
 * @retval			None
 */
static VOID im_xch_off_iclk( VOID )
{
#ifdef CO_ACT_XCH_ICLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_Xch_Iclk_Ctrl_Cnt, D_IM_XCH_AXCLK_REG_ADDR, D_IM_XCH_ICLK_REG_BIT );
#endif	// CO_ACT_XCH_ICLOCK
}

/**
 * @brief		Xch software reset
 * @param[in]   None
 * @param[out]	None
 * @return		D_IM_XCH_OK, D_IM_XCH_MACRO_BUSY_NG
 * @note		None
 */
INT32 im_xch_softreset( VOID )
{
	if( (IO_XCH.XCHCTL0.XTRG.bit.XTRG == D_IM_XCH_STOPPED) && (IO_XCH.XCHCTL1.XTRG.bit.XTRG == D_IM_XCH_STOPPED) ){
		// Software reset
		IO_XCH.SR.bit.SR = 1;
		Im_XCH_Dsb();

		// AXICLK        = MAX 200.0MHz(400/2)	MIN 41.7MHz(250/6)
		// APB clock     = MAX  66.7MHz			MIN 20.8MHz
		// --> Lowest-speed clock setting( AXICLK:83.33  APBCLK:41.67 )
		//     Please refer to chapter 6.3.6.1 of MB86S27A LSI specification.
		// -->   ( (1/AXICLK)  / (1/APB clock)  ) * 2 + 1
		//     = ( (1/83.33)   / (1/41.67)      ) * 2 + 1
		//     = ( (0.0120004800...) / (0.0239980802...) ) * 2 + 1
		//     = 2.0001200048  => 3 APB cycle

		// wait
		IO_XCH.SR.word;	// 2 cycle
		IO_XCH.SR.word;	// 2 cycle

		Im_XCH_Dsb();

		// Software reset release
		IO_XCH.SR.bit.SR = 0;
		Im_XCH_Dsb();

		// wait
		IO_XCH.SR.word;	// 2 cycle
		IO_XCH.SR.word;	// 2 cycle

		Im_XCH_Dsb();
	}
	else{
		return D_IM_XCH_MACRO_BUSY_NG;
	}

	return D_IM_XCH_OK;
}

/**
 * @brief		The common processing of Xch begins
 * @param[in]   E_IM_XCH_CH_SEL	xch		: Channel selection of Xch
 * @param[out]  None
 * @return		INT32	D_IM_XCH_OK, D_IM_XCH_NG
 * @note		None
 * @attention	None
 */
static INT32 im_xch_start_process( E_IM_XCH_CH_SEL xch )
{
	DDIM_USER_ER ercd;
	DDIM_USER_FLGPTN flg_ptn;

	if( xch == E_IM_XCH_CH_SEL_0 ){
		im_xch_on_pclk();	// PCLK on
		Im_XCH_Dsb();

		// Set parameter
		IO_XCH.XCHCTL0.XMD.bit.XMD				= gIM_Xch_Mng_Save[xch].xmd;
		IO_XCH.XCHCTL0.XFDT.bit.XFDT			= gIM_Xch_Mng_Save[xch].xfdt;
		IO_XCH.XCHCTL0.XDTYPE.bit.XSDTYPE		= gIM_Xch_Mng_Save[xch].xdtype;
		IO_XCH.XCHCTL0.XDTYPE.bit.XDDTYPE		= gIM_Xch_Mng_Save[xch].xdtype;
		IO_XCH.XCHCTL0.XCYC.bit.XHCYC			= gIM_Xch_Mng_Save[xch].xhcyc;
		IO_XCH.XCHCTL0.XCYC.bit.XVCYC			= gIM_Xch_Mng_Save[xch].xvcyc;
		IO_XCH.XCHCTL0.XHEN.bit.XHEN			= gIM_Xch_Mng_Save[xch].xhen;
		IO_XCH.XCHCTL0.XVEN.bit.XVEN			= gIM_Xch_Mng_Save[xch].xven;
		IO_XCH.XCHCTL0.XSHW.bit.XSHW			= gIM_Xch_Mng_Save[xch].xshw;
		IO_XCH.XCHCTL0.XGHW.bit.XGHW			= gIM_Xch_Mng_Save[xch].xghw;
		IO_XCH.XCHCTL0.XDHW.bit.XDHW			= gIM_Xch_Mng_Save[xch].xdhw;
		IO_XCH.XCHCTL0.XDVW.bit.XDVW			= gIM_Xch_Mng_Save[xch].xdvw;
		IO_XCH.XCHCTL0.XSA.word					= gIM_Xch_Mng_Save[xch].xsa;
		IO_XCH.XCHCTL0.XDA.word					= gIM_Xch_Mng_Save[xch].xda;
		IO_XCH.XHISTG0.bit.XHCNT0				= gIM_Xch_Mng_Save[xch].xhcnt;

#ifdef CO_IM_XCH_DEBUG
		Ddim_Print(("IO_XCH.XCHCTL0.XMD.bit.XMD        =%d\n", IO_XCH.XCHCTL0.XMD.bit.XMD));
		Ddim_Print(("IO_XCH.XCHCTL0.XFDT.bit.XFDT      =%d\n", IO_XCH.XCHCTL0.XFDT.bit.XFDT));
		Ddim_Print(("IO_XCH.XCHCTL0.XDTYPE.bit.XSDTYPE =%d\n", IO_XCH.XCHCTL0.XDTYPE.bit.XSDTYPE));
		Ddim_Print(("IO_XCH.XCHCTL0.XDTYPE.bit.XDDTYPE =%d\n", IO_XCH.XCHCTL0.XDTYPE.bit.XDDTYPE));
		Ddim_Print(("IO_XCH.XCHCTL0.XCYC.bit.XHCYC     =%d\n", IO_XCH.XCHCTL0.XCYC.bit.XHCYC));
		Ddim_Print(("IO_XCH.XCHCTL0.XCYC.bit.XVCYC     =%d\n", IO_XCH.XCHCTL0.XCYC.bit.XVCYC));
		Ddim_Print(("IO_XCH.XCHCTL0.XHEN.bit.XHEN      =0x%lX\n", IO_XCH.XCHCTL0.XHEN.bit.XHEN));
		Ddim_Print(("IO_XCH.XCHCTL0.XVEN.bit.XVEN      =0x%lX\n", IO_XCH.XCHCTL0.XVEN.bit.XVEN));
		Ddim_Print(("IO_XCH.XCHCTL0.XSHW.bit.XSHW      =%d\n", IO_XCH.XCHCTL0.XSHW.bit.XSHW));
		Ddim_Print(("IO_XCH.XCHCTL0.XGHW.bit.XGHW      =%d\n", IO_XCH.XCHCTL0.XGHW.bit.XGHW));
		Ddim_Print(("IO_XCH.XCHCTL0.XDHW.bit.XDHW      =%d\n", IO_XCH.XCHCTL0.XDHW.bit.XDHW));
		Ddim_Print(("IO_XCH.XCHCTL0.XDVW.bit.XDVW      =%d\n", IO_XCH.XCHCTL0.XDVW.bit.XDVW));
		Ddim_Print(("IO_XCH.XCHCTL0.XSA.word           =0x%lX\n", IO_XCH.XCHCTL0.XSA.word));
		Ddim_Print(("IO_XCH.XCHCTL0.XDA.word           =0x%lX\n", IO_XCH.XCHCTL0.XDA.word));
		Ddim_Print(("IO_XCH.XHISTG0.bit.XHCNT0         =%d\n", IO_XCH.XHISTG0.bit.XHCNT0));
#endif

		// X0ch
		ercd = DDIM_User_Clr_Flg( FID_IM_XCH, ~D_IM_XCH_FLG_0_WAIT_END );
		if( ercd != D_DDIM_USER_E_OK ){
			im_xch_off_pclk();	// PCLK off
			Im_XCH_Dsb();
			// DDIM_User_Clr_Flg error
			Ddim_Print(("I:im_xch_start_process. DDIM_User_Clr_Flg error. ercd = %d\n", ercd));
			return D_IM_XCH_NG;
		}

		// X0ch start
		IO_XCH.XCHCTL0.XTRG.bit.XTRG = D_IM_XCH_START;

		im_xch_off_pclk();	// PCLK off
		Im_XCH_Dsb();

		if( gIM_Xch_Mng_Save[xch].sync_type == E_IM_XCH_SYNC_TYPE_SYNC ){
#ifdef CO_DEBUG_ON_PC
//			Ddim_Print(("Call Im_Xch_Int_Handler() for PC route check.\n"));
			IO_XCH.XCHICE.bit.XE0	= 1;
			IO_XCH.XCHICF.bit.__XF0	= 1;
			IO_XCH.XCHCTL0.XTRG.bit.XTRG = D_IM_XCH_STOPPED;
			Im_Xch_Int_Handler();
#endif

			// Xch end wait
			ercd = DDIM_User_Twai_Flg( FID_IM_XCH, D_IM_XCH_FLG_0_WAIT_END, D_DDIM_USER_TWF_ORW, &flg_ptn, D_DDIM_WAIT_END_TIME );
			if( ercd != D_DDIM_USER_E_OK ){
				// DDIM_User_Twai_Flg error
				Ddim_Print(("I:im_xch_start_process. DDIM_User_Twai_Flg error. ercd = %d\n", ercd));
				return D_IM_XCH_NG;
			}
		}
	}
	else {
		im_xch_on_pclk();	// PCLK on
		Im_XCH_Dsb();

		IO_XCH.XCHCTL1.XMD.bit.XMD				= gIM_Xch_Mng_Save[xch].xmd;
		IO_XCH.XCHCTL1.XFDT.bit.XFDT			= gIM_Xch_Mng_Save[xch].xfdt;
		IO_XCH.XCHCTL1.XDTYPE.bit.XSDTYPE		= gIM_Xch_Mng_Save[xch].xdtype;
		IO_XCH.XCHCTL1.XDTYPE.bit.XDDTYPE		= gIM_Xch_Mng_Save[xch].xdtype;
		IO_XCH.XCHCTL1.XCYC.bit.XHCYC			= gIM_Xch_Mng_Save[xch].xhcyc;
		IO_XCH.XCHCTL1.XCYC.bit.XVCYC			= gIM_Xch_Mng_Save[xch].xvcyc;
		IO_XCH.XCHCTL1.XHEN.bit.XHEN			= gIM_Xch_Mng_Save[xch].xhen;
		IO_XCH.XCHCTL1.XVEN.bit.XVEN			= gIM_Xch_Mng_Save[xch].xven;
		IO_XCH.XCHCTL1.XSHW.bit.XSHW			= gIM_Xch_Mng_Save[xch].xshw;
		IO_XCH.XCHCTL1.XGHW.bit.XGHW			= gIM_Xch_Mng_Save[xch].xghw;
		IO_XCH.XCHCTL1.XDHW.bit.XDHW			= gIM_Xch_Mng_Save[xch].xdhw;
		IO_XCH.XCHCTL1.XDVW.bit.XDVW			= gIM_Xch_Mng_Save[xch].xdvw;
		IO_XCH.XCHCTL1.XSA.word					= gIM_Xch_Mng_Save[xch].xsa;
		IO_XCH.XCHCTL1.XDA.word					= gIM_Xch_Mng_Save[xch].xda;

#ifdef CO_IM_XCH_DEBUG
		Ddim_Print(("IO_XCH.XCHCTL1.XMD.bit.XMD        =%d\n", IO_XCH.XCHCTL1.XMD.bit.XMD));
		Ddim_Print(("IO_XCH.XCHCTL1.XFDT.bit.XFDT      =%d\n", IO_XCH.XCHCTL1.XFDT.bit.XFDT));
		Ddim_Print(("IO_XCH.XCHCTL1.XDTYPE.bit.XSDTYPE =%d\n", IO_XCH.XCHCTL1.XDTYPE.bit.XSDTYPE));
		Ddim_Print(("IO_XCH.XCHCTL1.XDTYPE.bit.XDDTYPE =%d\n", IO_XCH.XCHCTL1.XDTYPE.bit.XDDTYPE));
		Ddim_Print(("IO_XCH.XCHCTL1.XCYC.bit.XHCYC     =%d\n", IO_XCH.XCHCTL1.XCYC.bit.XHCYC));
		Ddim_Print(("IO_XCH.XCHCTL1.XCYC.bit.XVCYC     =%d\n", IO_XCH.XCHCTL1.XCYC.bit.XVCYC));
		Ddim_Print(("IO_XCH.XCHCTL1.XHEN.bit.XHEN      =0x%lX\n", IO_XCH.XCHCTL1.XHEN.bit.XHEN));
		Ddim_Print(("IO_XCH.XCHCTL1.XVEN.bit.XVEN      =0x%lX\n", IO_XCH.XCHCTL1.XVEN.bit.XVEN));
		Ddim_Print(("IO_XCH.XCHCTL1.XSHW.bit.XSHW      =%d\n", IO_XCH.XCHCTL1.XSHW.bit.XSHW));
		Ddim_Print(("IO_XCH.XCHCTL1.XGHW.bit.XGHW      =%d\n", IO_XCH.XCHCTL1.XGHW.bit.XGHW));
		Ddim_Print(("IO_XCH.XCHCTL1.XDHW.bit.XDHW      =%d\n", IO_XCH.XCHCTL1.XDHW.bit.XDHW));
		Ddim_Print(("IO_XCH.XCHCTL1.XDVW.bit.XDVW      =%d\n", IO_XCH.XCHCTL1.XDVW.bit.XDVW));
		Ddim_Print(("IO_XCH.XCHCTL1.XSA.word           =0x%lX\n", IO_XCH.XCHCTL1.XSA.word));
		Ddim_Print(("IO_XCH.XCHCTL1.XDA.word           =0x%lX\n", IO_XCH.XCHCTL1.XDA.word));
#endif

		// X1ch
		ercd = DDIM_User_Clr_Flg( FID_IM_XCH, ~D_IM_XCH_FLG_1_WAIT_END );
		if( ercd != D_DDIM_USER_E_OK ){
			im_xch_off_pclk();	// PCLK off
			Im_XCH_Dsb();
			// DDIM_User_Clr_Flg error
			Ddim_Print(("I:im_xch_start_process. DDIM_User_Clr_Flg error. ercd = %d\n", ercd));
			return D_IM_XCH_NG;
		}

		// X1ch start
		IO_XCH.XCHCTL1.XTRG.bit.XTRG = D_IM_XCH_START;

		im_xch_off_pclk();	// PCLK off
		Im_XCH_Dsb();

		if( gIM_Xch_Mng_Save[xch].sync_type == E_IM_XCH_SYNC_TYPE_SYNC ){
#ifdef CO_DEBUG_ON_PC
//			Ddim_Print(("Call Im_Xch_Int_Handler() for PC route check.\n"));
			IO_XCH.XCHICE.bit.XE1	= 1;
			IO_XCH.XCHICF.bit.__XF1	= 1;
			IO_XCH.XCHCTL1.XTRG.bit.XTRG = D_IM_XCH_STOPPED;
			Im_Xch_Int_Handler();
#endif
			// XCH end wait
			ercd = DDIM_User_Twai_Flg( FID_IM_XCH, D_IM_XCH_FLG_1_WAIT_END, D_DDIM_USER_TWF_ORW, &flg_ptn, D_DDIM_WAIT_END_TIME );
			if( ercd != D_DDIM_USER_E_OK ){
				// DDIM_User_Twai_Flg error
				Ddim_Print(("I:im_xch_start_process. DDIM_User_Twai_Flg error. ercd = %d\n", ercd));
				return D_IM_XCH_NG;
			}
		}
	}

	return D_IM_XCH_OK;
}

/**
 * @brief		It is a subprogram to be called it from interrupt handler
 * @param[in]   E_IM_XCH_CH_SEL	xch		: Channel selection of Xch
 * @param[out]  None
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID im_xch_inthand_sub( E_IM_XCH_CH_SEL xch )
{
	DDIM_USER_ER	ercd;

	if( xch == E_IM_XCH_CH_SEL_0 ){
		// X0ch
		ercd = DDIM_User_Set_Flg( FID_IM_XCH, D_IM_XCH_FLG_0_WAIT_END );
		if( ercd != D_DDIM_USER_E_OK ){
			// DDIM_User_Set_Flg error
			Ddim_Print(("I:Im_Xch_Int_Handler. DDIM_User_Set_Flg error. ercd = %d\n", ercd));
		}

#ifdef CO_DEBUG_ON_PC
//		Ddim_Print(("I:im_xch_inthand_sub. X0Ch OK\n"));
#endif //CO_DEBUG_ON_PC

	}
	else{
		// X1ch
		ercd = DDIM_User_Set_Flg( FID_IM_XCH, D_IM_XCH_FLG_1_WAIT_END );
		if( ercd != D_DDIM_USER_E_OK ){
			// DDIM_User_Set_Flg error
			Ddim_Print(("I:Im_Xch_Int_Handler. DDIM_User_Set_Flg error. ercd = %d\n", ercd));
		}

#ifdef CO_DEBUG_ON_PC
//		Ddim_Print(("I:im_xch_inthand_sub. X1Ch OK\n"));
#endif //CO_DEBUG_ON_PC

	}

	if( (gIM_Xch_Mng_Save[xch].sync_type == E_IM_XCH_SYNC_TYPE_ASYNC) && (gIM_Xch_Mng_Save[xch].pCallBack) ){
		// Call Back function
		((VOID (*)()) gIM_Xch_Mng_Save[xch].pCallBack)();
	}
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/**
 * @brief		Disassemble the copy_param
 * @param[in]   T_IM_XCH_COPY_EX*		copy_param		: copy necessary data
 * @param[out]	T_IM_XCH_CTRL_CMN*		xch_ctrl_cmn	: common control data
 * @param[out]	T_IM_XCH_CTRL_THIN*		xch_ctrl_thin	: down sampling control data
 * @param[out]	T_IM_XCH_CTRL_COPY*		xch_ctrl_copy	: copy control data
 * @return		D_IM_XCH_OK
 * @note		None
 * @attention	None
 */
INT32 im_xch_conv_copy_param( T_IM_XCH_COPY_EX* copy_param, T_IM_XCH_CTRL_CMN* xch_ctrl_cmn, T_IM_XCH_CTRL_THIN* xch_ctrl_thin, T_IM_XCH_CTRL_COPY* xch_ctrl_copy )
{
	if( copy_param->copy_type == E_IM_XCH_COPY_SEL_HI ){
		// hi-speed copy
		xch_ctrl_cmn->mode = D_IM_XCH_MD_COPY;

		xch_ctrl_copy->vcyc		= 0x0;
		xch_ctrl_copy->ven		= 0xFFFFFFFF;
		xch_ctrl_copy->s_hsize	= copy_param->size.src_gl_width;
		xch_ctrl_copy->src_addr	= copy_param->addr.src;
	}
	else{
		// normal-speed copy
		xch_ctrl_cmn->mode = D_IM_XCH_MD_DOWN_HI_SPEED;

		xch_ctrl_thin->hcyc		= 0x0;
		xch_ctrl_thin->vcyc		= 0x0;
		xch_ctrl_thin->hen		= 0xFFFFFFFF;
		xch_ctrl_thin->ven		= 0xFFFFFFFF;
		xch_ctrl_thin->s_hsize	= copy_param->size.src_gl_width;
		xch_ctrl_thin->src_addr	= copy_param->addr.src;
	}

	// set control structure
	xch_ctrl_cmn->g_hsize	= copy_param->size.dst_gl_width;
	xch_ctrl_cmn->d_hsize	= copy_param->size.width;
	xch_ctrl_cmn->d_vsize	= copy_param->size.lines;
	xch_ctrl_cmn->dst_addr	= copy_param->addr.dst;

	return D_IM_XCH_OK;
}

/**
 * @brief		Disassemble the fill_param
 * @param[in]   T_IM_XCH_FILL_EX*	fill_param		: fill necessary data
 * @param[out]	T_IM_XCH_CTRL_CMN*	xch_ctrl_cmn	: common control data
 * @return		None
 * @note		None
 * @attention	None
 */
VOID im_xch_conv_fill_param( T_IM_XCH_FILL_EX* fill_param, T_IM_XCH_CTRL_CMN* xch_ctrl_cmn )
{
	// set common control structure
	xch_ctrl_cmn->mode		= D_IM_XCH_MD_FILL;
	xch_ctrl_cmn->g_hsize	= fill_param->size.gl_width;
	xch_ctrl_cmn->d_hsize	= fill_param->size.width;
	xch_ctrl_cmn->d_vsize	= fill_param->size.lines;
	xch_ctrl_cmn->dst_addr	= fill_param->fill_addr;

	return;
}

/**
 * @brief		Disassemble the hist_param
 * @param[in]   T_IM_XCH_HIST_EX*	hist_param		: histogram necessary data
 * @param[out]	T_IM_XCH_CTRL_CMN*	xch_ctrl_cmn	: common control data
 * @param[out]	T_IM_XCH_CTRL_HIST*	xch_ctrl_hist	: histogram control data
 * @return		None
 * @note		None
 * @attention	None
 */
VOID im_xch_conv_hist_param( T_IM_XCH_EXAMINE_HIST* hist_param, T_IM_XCH_CTRL_CMN* xch_ctrl_cmn, T_IM_XCH_CTRL_HIST* xch_ctrl_hist )
{
	// set common control structure
	xch_ctrl_cmn->mode		= D_IM_XCH_MD_HISTGRAM;
	xch_ctrl_cmn->g_hsize	= hist_param->size.width;
	xch_ctrl_cmn->d_hsize	= hist_param->size.width;
	xch_ctrl_cmn->d_vsize	= hist_param->size.lines;
	xch_ctrl_cmn->dst_addr	= hist_param->hist_addr;

	// set histogram control structure
	xch_ctrl_hist->xdtype	= D_IM_XCH_XDTYPE_8;
	xch_ctrl_hist->hcyc		= 0x00;
	xch_ctrl_hist->vcyc		= 0x00;
	xch_ctrl_hist->hen		= 0xFFFFFFFF;
	xch_ctrl_hist->ven		= 0xFFFFFFFF;
	xch_ctrl_hist->s_hsize	= hist_param->size.width;
	xch_ctrl_hist->src_addr	= hist_param->hist_addr;
	xch_ctrl_hist->hcnt		= 0;

	return;
}

/**
 * @brief		Disassemble the thin_param
 * @param[in]   T_IM_XCH_CONV_THIN*		thin_param		: down sampling necessary data
 * @param[out]	T_IM_XCH_CTRL_CMN*		xch_ctrl_cmn	: common control data
 * @param[out]	T_IM_XCH_CTRL_THIN*		xch_ctrl_thin	: down sampling control data
 * @return		D_IM_XCH_OK
 * @note		None
 * @attention	None
 */
INT32 im_xch_conv_thin_param( T_IM_XCH_CONV_THIN* thin_param, T_IM_XCH_CTRL_CMN* xch_ctrl_cmn, T_IM_XCH_CTRL_THIN* xch_ctrl_thin )
{
	switch( thin_param->thin_width ){
		case D_IM_XCH_THIN_SEL_1_2:
			// 1/2 down sampling
			xch_ctrl_cmn->d_hsize	= (thin_param->size.width >> 1);
			xch_ctrl_thin->hen		= 0xAAAAAAAA;
			break;
		case D_IM_XCH_THIN_SEL_1_4:
			// 1/4 down sampling
			xch_ctrl_cmn->d_hsize	= (thin_param->size.width >> 2);
			xch_ctrl_thin->hen		= 0x44444444;
			break;
		case D_IM_XCH_THIN_SEL_3_4:
			// 3/4 down sampling
			xch_ctrl_cmn->d_hsize	= ((thin_param->size.width * 3) >> 2);
			xch_ctrl_thin->hen		= 0xBBBBBBBB;
			break;
		case D_IM_XCH_THIN_SEL_1_1:
		default:
			// not down sampling
			xch_ctrl_cmn->d_hsize	= thin_param->size.width;
			xch_ctrl_thin->hen		= 0xFFFFFFFF;
			break;
	}

	switch( thin_param->thin_lines ){
		case D_IM_XCH_THIN_SEL_1_2:
			// 1/2 down sampling
			xch_ctrl_cmn->d_vsize	= (thin_param->size.lines >> 1);
			xch_ctrl_thin->ven		= 0xAAAAAAAA;
			break;
		case D_IM_XCH_THIN_SEL_1_4:
			// 1/4 down sampling
			xch_ctrl_cmn->d_vsize	= (thin_param->size.lines >> 2);
			xch_ctrl_thin->ven		= 0x44444444;
			break;
		case D_IM_XCH_THIN_SEL_3_4:
			// 3/4 down sampling
			xch_ctrl_cmn->d_vsize	= ((thin_param->size.lines * 3) >> 2);
			xch_ctrl_thin->ven		= 0xBBBBBBBB;
			break;
		case D_IM_XCH_THIN_SEL_1_1:
		default:
			// not down sampling
			xch_ctrl_cmn->d_vsize	= thin_param->size.lines;
			xch_ctrl_thin->ven		= 0xFFFFFFFF;
			break;
	}

	// set control structure
	xch_ctrl_cmn->mode			= D_IM_XCH_MD_DOWN_HI_SPEED;
	xch_ctrl_cmn->g_hsize		= thin_param->size.dst_gl_width;
	xch_ctrl_cmn->dst_addr		= thin_param->addr.dst;

	xch_ctrl_thin->xdtype		= D_IM_XCH_XDTYPE_8;
	xch_ctrl_thin->s_hsize		= thin_param->size.src_gl_width;
	xch_ctrl_thin->src_addr		= thin_param->addr.src;
	xch_ctrl_thin->hcyc			= 0x0;	// 32 byte cycle
	xch_ctrl_thin->vcyc			= 0x0;	// 32 line cycle

	return D_IM_XCH_OK;
}

/**
 * @brief		Copy process using the two channels
 * @param[in]   src_addr		: copy source address
 * @param[in]   src_gl_width	: copy source global width
 * @param[in]   dst_gl_width	: copy destination global width
 * @param[in]   dst_addr		: copy destination address
 * @param[in]   width			: copy area width
 * @param[in]   lines			: copy area lines
 * @return		INT32	D_IM_XCH_OK/ D_IM_XCH_NG/ D_IM_XCH_SEM_TIMEOUT/ D_IM_XCH_SEM_NG
 * @note		None
 * @attention	None
 */
static int im_xch_copy_2ch( UINT32 src_addr, USHORT src_gl_width, USHORT dst_gl_width, UINT32 dst_addr, USHORT width, USHORT lines )
{
	INT32				ret;
	USHORT				lines_0ch;
	USHORT				lines_1ch;
	UINT32				src_second_addr;
	UINT32				dst_second_addr;
	T_IM_XCH_COPY_EX	xch_copy_0ch;
	T_IM_XCH_COPY_EX	xch_copy_1ch;

	lines_0ch = lines >> 1;			// 0ch lines
	lines_1ch = lines - lines_0ch;	// 1ch lines
	src_second_addr = src_addr + src_gl_width * lines_0ch;
	dst_second_addr = dst_addr + dst_gl_width * lines_0ch;

	// initialize
	memset( &xch_copy_0ch, 0, sizeof(T_IM_XCH_COPY_EX) );
	memset( &xch_copy_1ch, 0, sizeof(T_IM_XCH_COPY_EX) );

	// X0ch open
	ret = Im_Xch_Open( E_IM_XCH_CH_SEL_0, D_DDIM_WAIT_END_FOREVER );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Open error
		Ddim_Print(("I:im_xch_copy_2ch: Im_Xch_Open(0ch) error. ret = 0x%X\n", ret));
		return ret;
	}

	// X1ch open
	ret = Im_Xch_Open( E_IM_XCH_CH_SEL_1, D_DDIM_WAIT_END_FOREVER );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Open error
		Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
		Ddim_Print(("I:im_xch_copy_2ch: Im_Xch_Open(1ch) error. ret = 0x%X\n", ret));
		return ret;
	}

	// set 0ch control
	if( ((src_addr & 0xF) != 0) || ((dst_addr & 0xF) != 0) ){
		// normal copy mode
		xch_copy_0ch.copy_type	= E_IM_XCH_COPY_SEL_NORMAL;
	}
	else{
		// hi-speed copy mode
		xch_copy_0ch.copy_type	= E_IM_XCH_COPY_SEL_HI;
	}

	xch_copy_0ch.xch	= E_IM_XCH_CH_SEL_0;

	xch_copy_0ch.size.width			= width;
	xch_copy_0ch.size.lines			= lines_0ch;
	xch_copy_0ch.size.src_gl_width	= src_gl_width;
	xch_copy_0ch.size.dst_gl_width	= dst_gl_width;

	xch_copy_0ch.addr.src		= src_addr;
	xch_copy_0ch.addr.dst		= dst_addr;

	// set 1ch control
	if( ((src_second_addr & 0xF) != 0) || ((dst_second_addr & 0xF) != 0) ){
		// normal copy mode
		xch_copy_1ch.copy_type	= E_IM_XCH_COPY_SEL_NORMAL;
	}
	else{
		// hi-speed copy mode
		xch_copy_1ch.copy_type	= E_IM_XCH_COPY_SEL_HI;
	}

	xch_copy_1ch.xch	= E_IM_XCH_CH_SEL_1;

	xch_copy_1ch.size.width			= width;
	xch_copy_1ch.size.lines			= lines_1ch;
	xch_copy_1ch.size.src_gl_width	= src_gl_width;
	xch_copy_1ch.size.dst_gl_width	= dst_gl_width;

	xch_copy_1ch.addr.src	= src_second_addr;
	xch_copy_1ch.addr.dst	= dst_second_addr;

#ifdef CO_DEBUG_ON_PC
	IO_XCH.XCHCTL0.XTRG.bit.XTRG	= 2;
	IO_XCH.XCHCTL1.XTRG.bit.XTRG	= 2;
#endif

	// Xch start copy process
	ret = Im_Xch_Copy_Ex_Async( &xch_copy_0ch, NULL );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Copy_Ex_Async error(0ch)
		Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
		Im_Xch_Close( E_IM_XCH_CH_SEL_1 );
		Ddim_Print(("I:im_xch_copy_2ch: Im_Xch_Copy_Ex_Async(0ch) error. ret = 0x%X\n", ret));
		return ret;
	}

	ret = Im_Xch_Copy_Ex_Async( &xch_copy_1ch, NULL );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Copy_Ex_Async error(1ch)
		Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
		Im_Xch_Close( E_IM_XCH_CH_SEL_1 );
		Ddim_Print(("I:im_xch_copy_2ch: Im_Xch_Copy_Ex_Async(1ch) error. ret = 0x%X\n", ret));
		return ret;
	}

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("Call Im_Xch_Int_Handler() for PC route check.\n"));
	IO_XCH.XCHICE.bit.XE0	= 1;
	IO_XCH.XCHICF.bit.__XF0	= 1;
	IO_XCH.XCHICE.bit.XE1	= 0;
	IO_XCH.XCHICF.bit.__XF1	= 0;
	Im_Xch_Int_Handler();
#endif

	// Xch wait end
	ret = Im_Xch_Wait_End( E_IM_XCH_CH_SEL_0 );
	if( ret != D_IM_XCH_OK ){
		Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
		Im_Xch_Close( E_IM_XCH_CH_SEL_1 );
		Ddim_Print(("I:im_xch_copy_2ch: Im_Xch_Wait_End(0ch) error. ret = 0x%X\n", ret));
		return ret;
	}

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("Call Im_Xch_Int_Handler() for PC route check.\n"));
	IO_XCH.XCHICE.bit.XE0	= 0;
	IO_XCH.XCHICF.bit.__XF0	= 0;
	IO_XCH.XCHICE.bit.XE1	= 1;
	IO_XCH.XCHICF.bit.__XF1	= 1;
	Im_Xch_Int_Handler();
#endif

	ret = Im_Xch_Wait_End( E_IM_XCH_CH_SEL_1 );
	if( ret != D_IM_XCH_OK ){
		Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
		Im_Xch_Close( E_IM_XCH_CH_SEL_1 );
		Ddim_Print(("I:im_xch_copy_2ch: Im_Xch_Wait_End(1ch) error. ret = 0x%X\n", ret));
		return ret;
	}

	// 0ch Close
	ret = Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Close error(0ch)
		Im_Xch_Close( E_IM_XCH_CH_SEL_1 );
		Ddim_Print(("I:im_xch_copy_2ch: Im_Xch_Close(0ch) error. ret = 0x%X\n", ret));
		return ret;
	}

	// 1ch Close
	ret = Im_Xch_Close( E_IM_XCH_CH_SEL_1 );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Close error(1ch)
		Ddim_Print(("I:im_xch_copy_2ch: Im_Xch_Close(1ch) error. ret = 0x%X\n", ret));
		return ret;
	}

	return D_IM_XCH_OK;
}

/**
 * @brief		Copy process using the two channels ( byte unit )
 * @param[in]   src_addr		: copy source address
 * @param[in]   src_gl_width	: copy source global width
 * @param[in]   dst_gl_width	: copy destination global width
 * @param[in]   dst_addr		: copy destination address
 * @param[in]   width			: copy area width
 * @param[in]   lines			: copy area lines
 * @return		INT32	D_IM_XCH_OK/ D_IM_XCH_NG/ D_IM_XCH_SEM_TIMEOUT/ D_IM_XCH_SEM_NG
 * @note		None
 * @attention	None
 */
static INT32 im_xch_copy_byte( UINT32 src_addr, USHORT src_gl_width, USHORT dst_gl_width, UINT32 dst_addr, USHORT width, USHORT lines )
{
	INT32				ret;
	T_IM_XCH_COPY_EX	xch_copy;
	T_DDIM_USER_RSEM	xch_sem_param;
	E_IM_XCH_CH_SEL		xch	= E_IM_XCH_CH_SEL_0;

	// initialize
	memset( &xch_copy, 0, sizeof(T_IM_XCH_COPY_EX) );

	DDIM_User_Ref_Sem( SID_IM_XCH(E_IM_XCH_CH_SEL_0), &xch_sem_param );

	if( xch_sem_param.semcnt == 0 ){

		// The X0ch has not become empty.
		DDIM_User_Ref_Sem( SID_IM_XCH( E_IM_XCH_CH_SEL_1 ), &xch_sem_param );

		if( xch_sem_param.semcnt == 0 ){
			// The X1ch has not become empty.
			Ddim_Print(("I:im_xch_copy_byte: The channel has not become empty. . \n"));
			return D_IM_XCH_NG;
		}
		xch = E_IM_XCH_CH_SEL_1;
	}
	else{
		// The channel has become empty.
		xch = E_IM_XCH_CH_SEL_0;
	}

	// Xch open
	ret = Im_Xch_Open( xch, D_DDIM_WAIT_END_FOREVER );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Open error
		Ddim_Print(("I:im_xch_copy_byte: Im_Xch_Open error. ret = 0x%X\n", ret));
		return ret;
	}

	// set copy control
	if( ((src_addr & 0xF) != 0) || ((dst_addr & 0xF) != 0) || ((width & 0xF) != 0) ){
		// normal copy mode
		xch_copy.copy_type	= E_IM_XCH_COPY_SEL_NORMAL;
	}
	else{
		// hi-speed copy mode
		xch_copy.copy_type	= E_IM_XCH_COPY_SEL_HI;
	}

	xch_copy.xch				= xch;
	xch_copy.size.width			= width;
	xch_copy.size.lines			= lines;
	xch_copy.size.src_gl_width	= src_gl_width;
	xch_copy.size.dst_gl_width	= dst_gl_width;

	xch_copy.addr.src			= src_addr;
	xch_copy.addr.dst			= dst_addr;

#ifdef CO_DEBUG_ON_PC
	IO_XCH.XCHCTL0.XTRG.bit.XTRG	= 2;
	IO_XCH.XCHCTL1.XTRG.bit.XTRG	= 2;
#endif

	// Xch start copy process
	ret = Im_Xch_Copy_Ex_Async( &xch_copy, NULL );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Copy_Ex_Async error
		Im_Xch_Close( xch );
		Ddim_Print(("I:im_xch_copy_byte: Im_Xch_Copy_Ex_Async error. ret = 0x%X\n", ret));
		return ret;
	}

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("Call Im_Xch_Int_Handler() for PC route check.\n"));
	if( xch == E_IM_XCH_CH_SEL_0 ){
		IO_XCH.XCHICE.bit.XE0	= 1;
		IO_XCH.XCHICF.bit.__XF0	= 1;
		IO_XCH.XCHICE.bit.XE1	= 0;
		IO_XCH.XCHICF.bit.__XF1	= 0;
	}
	else{
		IO_XCH.XCHICE.bit.XE0	= 0;
		IO_XCH.XCHICF.bit.__XF0	= 0;
		IO_XCH.XCHICE.bit.XE1	= 1;
		IO_XCH.XCHICF.bit.__XF1	= 1;
	}
	Im_Xch_Int_Handler();
#endif


	// Xch wait end
	ret = Im_Xch_Wait_End( xch );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Copy_Ex_Async error
		Im_Xch_Close( xch );
		Ddim_Print(("I:im_xch_copy_byte: Im_Xch_Wait_End error. ret = 0x%X\n", ret));
		return ret;
	}

	// Xch Close
	ret = Im_Xch_Close( xch );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Close error
		Ddim_Print(("I:im_xch_copy_byte: Im_Xch_Close error. ret = 0x%X\n", ret));
		return ret;
	}

	return D_IM_XCH_OK;
}

//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		The semaphore of Xch is acquired
 * @param[in]   xch		: Channel selection of Xch
 * @param[in]	tmout	: Time-out period
 * @return		D_IM_XCH_OK, D_IM_XCH_INPUT_PARAM_ERR, D_IM_XCH_SEM_TIMEOUT, D_IM_XCH_SEM_NG
 * @note		None
 */
INT32 Im_Xch_Open( E_IM_XCH_CH_SEL xch, INT32 tmout )
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if( tmout < D_DDIM_USER_SEM_WAIT_FEVR ){
		Ddim_Assertion(("I:Im_Xch_Open: input param error. tmout = %x\n", tmout));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif

	if( D_DDIM_USER_SEM_WAIT_POL == tmout ){
		ercd = DDIM_User_Pol_Sem( SID_IM_XCH(xch) );						// pol_sem()
	}
	else{
		ercd = DDIM_User_Twai_Sem( SID_IM_XCH(xch), (DDIM_USER_TMO)tmout );	// twai_sem()
	}

	if( D_DDIM_USER_E_OK != ercd ){
		// twai_sem error
		if( D_DDIM_USER_E_TMOUT == ercd ){
			// Semaphore Get Time Out
			Ddim_Print(("I:Im_Xch_Open() Error : Semaphore Get Time Out. ercd = %d\n", ercd));
			return D_IM_XCH_SEM_TIMEOUT;
		}
		// Semaphore Get Error
		Ddim_Print(("I:Im_Xch_Open() Error : Semaphore Get Error. ercd = %d\n", ercd));
		return D_IM_XCH_SEM_NG;
	}

	im_xch_on_iclk();
	Im_XCH_Dsb();

	return D_IM_XCH_OK;
}

/**
 * @brief		Xch initialize
 * @param[in]   None
 * @param[out]	None
 * @return		None
 * @note		None
 */
VOID Im_Xch_Init( VOID )
{
	im_xch_on_pclk();	// PCLK on

	// Software reset release
	IO_XCH.SR.bit.SR = 0;
	Im_XCH_Dsb();

#ifdef CO_IM_XCH_DEBUG
	Ddim_Print(("IO_XCH.SR.bit.SR        =%d\n", IO_XCH.SR.bit.SR));
#endif

	im_xch_off_pclk();	// PCLK off
	Im_XCH_Dsb();

#ifdef CO_DEBUG_ON_PC
	IO_XCH.XCHCTL0.XTRG.bit.XTRG = D_IM_XCH_STOPPED;
	IO_XCH.XCHCTL1.XTRG.bit.XTRG = D_IM_XCH_STOPPED;
#endif

	return;
}

/**
 * @brief 		Xch driver common control
 * @param[in]   xch				: Channel selection of Xch
 * @param[in]	*xch_ctrl_cmn	: Control parameter
 * @return		D_IM_XCH_OK, D_IM_XCH_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_CMN* xch_ctrl_cmn )
{
#ifdef CO_PARAM_CHECK
	if( xch_ctrl_cmn == NULL ){
		// xch_ctrl_cmn is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Ctrl_Common: Null check error. xch_ctrl_cmn = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
	if( (xch != E_IM_XCH_CH_SEL_0) && ( (xch_ctrl_cmn->mode == D_IM_XCH_MD_HISTGRAM_DOWN) || (xch_ctrl_cmn->mode == D_IM_XCH_MD_HISTGRAM) ) ){
		// A channel does not match Mode error
		Ddim_Assertion(("I:Im_Xch_Ctrl_Common: channel does not match histogram mode error. Xch = %d\n", xch));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	// Initialize the save user data.
	memset( (USHORT*)&gIM_Xch_Mng_Save[xch], 0, sizeof(T_IM_XCH_MNG_SAVE) );

	gIM_Xch_Mng_Save[xch].xmd		= xch_ctrl_cmn->mode;
	gIM_Xch_Mng_Save[xch].xghw		= xch_ctrl_cmn->g_hsize;
	gIM_Xch_Mng_Save[xch].xdhw		= xch_ctrl_cmn->d_hsize;
	gIM_Xch_Mng_Save[xch].xdvw		= xch_ctrl_cmn->d_vsize;
	gIM_Xch_Mng_Save[xch].xda		= xch_ctrl_cmn->dst_addr;
	gIM_Xch_Mng_Save[xch].pCallBack	= xch_ctrl_cmn->pCallBack;

#ifdef CO_IM_XCH_DEBUG
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xmd        =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xmd));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xghw       =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xghw));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xdhw       =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xdhw));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xdvw       =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xdvw));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xda        =0x%lX\n", xch, gIM_Xch_Mng_Save[xch].xda));
#endif

	return D_IM_XCH_OK;
}

/**
 * @brief		Xch driver downsampling control
 * @param[in]   xch				: Channel selection of Xch
 * @param[in]	*xch_ctrl_thin	: Control parameter
 * @return		D_IM_XCH_OK, D_IM_XCH_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_Xch_Ctrl_Thin( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_THIN* xch_ctrl_thin )
{
#ifdef CO_PARAM_CHECK
	if( xch_ctrl_thin == NULL ){
		// xch_ctrl_thin is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Ctrl_Thin: Null check error. xch_ctrl_thin = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	gIM_Xch_Mng_Save[xch].xdtype	= xch_ctrl_thin->xdtype;
	gIM_Xch_Mng_Save[xch].xhcyc		= xch_ctrl_thin->hcyc;
	gIM_Xch_Mng_Save[xch].xvcyc		= xch_ctrl_thin->vcyc;
	gIM_Xch_Mng_Save[xch].xhen		= xch_ctrl_thin->hen;
	gIM_Xch_Mng_Save[xch].xven		= xch_ctrl_thin->ven;
	gIM_Xch_Mng_Save[xch].xshw		= xch_ctrl_thin->s_hsize;
	gIM_Xch_Mng_Save[xch].xsa		= xch_ctrl_thin->src_addr;

#ifdef CO_IM_XCH_DEBUG
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xdtype     =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xdtype));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xhcyc      =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xhcyc ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xvcyc      =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xvcyc ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xhen       =0x%lX\n", xch, gIM_Xch_Mng_Save[xch].xhen  ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xven       =0x%lX\n", xch, gIM_Xch_Mng_Save[xch].xven  ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xshw       =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xshw  ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xsa        =0x%lX\n", xch, gIM_Xch_Mng_Save[xch].xsa   ));
#endif

	return D_IM_XCH_OK;
}

/**
 * @brief Xch driver histogram control
 * @param[in]   xch				: Channel selection of Xch
 * @param[in]	*xch_ctrl_hist	: Control parameter
 * @return		D_IM_XCH_OK, D_IM_XCH_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_Xch_Ctrl_Hist( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_HIST* xch_ctrl_hist )
{
#ifdef CO_PARAM_CHECK
	if( xch != E_IM_XCH_CH_SEL_0 ){
		// Channel number error. ( Only Histogram Mode : X0ch )
		Ddim_Assertion(("I:Im_Xch_Ctrl_Hist: Channel number error. Xch = %d\n", xch));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
	if( xch_ctrl_hist == NULL ){
		// xch_ctrl_hist is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Ctrl_Hist: Null check error. xch_ctrl_hist = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	gIM_Xch_Mng_Save[xch].xdtype	= xch_ctrl_hist->xdtype;
	gIM_Xch_Mng_Save[xch].xhcyc		= xch_ctrl_hist->hcyc;
	gIM_Xch_Mng_Save[xch].xvcyc		= xch_ctrl_hist->vcyc;
	gIM_Xch_Mng_Save[xch].xhen		= xch_ctrl_hist->hen;
	gIM_Xch_Mng_Save[xch].xven		= xch_ctrl_hist->ven;
	gIM_Xch_Mng_Save[xch].xshw		= xch_ctrl_hist->s_hsize;
	gIM_Xch_Mng_Save[xch].xsa		= xch_ctrl_hist->src_addr;
	gIM_Xch_Mng_Save[xch].xhcnt		= xch_ctrl_hist->hcnt;

#ifdef CO_IM_XCH_DEBUG
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xdtype     =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xdtype));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xhcyc      =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xhcyc ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xvcyc      =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xvcyc ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xhen       =0x%lX\n", xch, gIM_Xch_Mng_Save[xch].xhen  ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xven       =0x%lX\n", xch, gIM_Xch_Mng_Save[xch].xven  ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xshw       =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xshw  ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xsa        =0x%lX\n", xch, gIM_Xch_Mng_Save[xch].xsa   ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xhcnt      =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xhcnt ));
#endif

	return D_IM_XCH_OK;
}

/**
 * @brief		Xch driver half downsampling control
 * @param[in]   xch						: Channel selection of Xch
 * @param[in]	*xch_ctrl_half_thin		: Control parameter
 * @return		D_IM_XCH_OK, D_IM_XCH_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_Xch_Ctrl_Half_Thin( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_HALF_THIN* xch_ctrl_half_thin )
{
#ifdef CO_PARAM_CHECK
	if( xch_ctrl_half_thin == NULL ){
		// xch_ctrl_half_thin is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Ctrl_Half_Thin: Null check error. xch_ctrl_half_thin = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	gIM_Xch_Mng_Save[xch].xvcyc	= xch_ctrl_half_thin->vcyc;
	gIM_Xch_Mng_Save[xch].xven	= xch_ctrl_half_thin->ven;
	gIM_Xch_Mng_Save[xch].xshw	= xch_ctrl_half_thin->s_hsize;
	gIM_Xch_Mng_Save[xch].xsa	= xch_ctrl_half_thin->src_addr;

#ifdef CO_IM_XCH_DEBUG
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xvcyc      =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xvcyc ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xven       =0x%lX\n", xch, gIM_Xch_Mng_Save[xch].xven  ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xshw       =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xshw  ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xsa        =0x%lX\n", xch, gIM_Xch_Mng_Save[xch].xsa   ));
#endif

	return D_IM_XCH_OK;
}

/**
 * @brief		Xch driver copy control
 * @param[in]   xch				: Channel selection of Xch
 * @param[in]	*xch_ctrl_copy	: Control parameter
 * @return		D_IM_XCH_OK, D_IM_XCH_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_Xch_Ctrl_Copy( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_COPY* xch_ctrl_copy )
{
#ifdef CO_PARAM_CHECK
	if( xch_ctrl_copy == NULL ){
		// xch_ctrl_copy is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Ctrl_Copy: Null check error. xch_ctrl_copy = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	gIM_Xch_Mng_Save[xch].xvcyc	= xch_ctrl_copy->vcyc;
	gIM_Xch_Mng_Save[xch].xven	= xch_ctrl_copy->ven;
	gIM_Xch_Mng_Save[xch].xshw	= xch_ctrl_copy->s_hsize;
	gIM_Xch_Mng_Save[xch].xsa	= xch_ctrl_copy->src_addr;

#ifdef CO_IM_XCH_DEBUG
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xvcyc      =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xvcyc ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xven       =0x%lX\n", xch, gIM_Xch_Mng_Save[xch].xven  ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xshw       =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xshw  ));
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xsa        =0x%lX\n", xch, gIM_Xch_Mng_Save[xch].xsa   ));
#endif

	return D_IM_XCH_OK;
}

/**
 * @brief Xch driver fill control
 * @param[in]   xch			: Channel selection of Xch
 * @param[in]	fill_data	: Data to fill
 * @return		D_IM_XCH_OK
 * @note		None
 */
INT32 Im_Xch_Ctrl_Fill( E_IM_XCH_CH_SEL xch, UCHAR fill_data )
{
	gIM_Xch_Mng_Save[xch].xfdt	= fill_data;

#ifdef CO_IM_XCH_DEBUG
	Ddim_Print(("gIM_Xch_Mng_Save[%d].xfdt      =0x%X\n", xch, gIM_Xch_Mng_Save[xch].xfdt ));
#endif

	return D_IM_XCH_OK;
}


/**
 * @brief		The processing of Xch begins(for sync)
 * @param[in]   xch		: Channel selection of Xch
 * @return		D_IM_XCH_OK, D_IM_XCH_NG, D_IM_XCH_MACRO_BUSY_NG
 * @note		None
 */
INT32 Im_Xch_Start_Sync( E_IM_XCH_CH_SEL xch )
{
	INT32 ercd;

	if( xch == E_IM_XCH_CH_SEL_0 ){
		//X0ch
		im_xch_on_pclk();	// PCLK on
		Im_XCH_Dsb();

		// Check Xch running
		if( IO_XCH.XCHCTL0.XTRG.bit.XTRG != D_IM_XCH_STOPPED ){

			im_xch_off_pclk();	// PCLK off
			Im_XCH_Dsb();

			Ddim_Print(("I:Im_Xch_Start_Sync: Macro busy error.\n"));
			return D_IM_XCH_MACRO_BUSY_NG;
		}

		IO_XCH.XCHICF.word		&= D_IM_XCH_ICF_XF0_OFF;
		IO_XCH.XCHICE.bit.XE0	= 1;
		Im_XCH_Dsb();

		im_xch_off_pclk();	// PCLK off
		Im_XCH_Dsb();
	}
	else{
		// X1ch
		im_xch_on_pclk();	// PCLK on
		Im_XCH_Dsb();

		// Check Xch running
		if( IO_XCH.XCHCTL1.XTRG.bit.XTRG != D_IM_XCH_STOPPED ){

			im_xch_off_pclk();	// PCLK off
			Im_XCH_Dsb();

			Ddim_Print(("I:Im_Xch_Start_Sync: Macro busy error.\n"));
			return D_IM_XCH_MACRO_BUSY_NG;
		}

		IO_XCH.XCHICF.word		&= D_IM_XCH_ICF_XF1_OFF;
		IO_XCH.XCHICE.bit.XE1	= 1;
		Im_XCH_Dsb();

		im_xch_off_pclk();	// PCLK off
		Im_XCH_Dsb();
	}

	gIM_Xch_Mng_Save[xch].sync_type = E_IM_XCH_SYNC_TYPE_SYNC;

	// Processing Start
	ercd = im_xch_start_process( xch );
	if( ercd != D_IM_XCH_OK ){
		Ddim_Print(("I:Im_Xch_Start_Sync: im_xch_start_process error. ercd = 0x%X\n", ercd));
		return D_IM_XCH_NG;
	}

	return D_IM_XCH_OK;
}

/**
 * @brief		The processing of Xch begins(for async)
 * @param[in]   xch		: Channel selection of Xch
 * @return		D_IM_XCH_OK, D_IM_XCH_MACRO_BUSY_NG
 * @note		None
 */
INT32 Im_Xch_Start_Async( E_IM_XCH_CH_SEL xch )
{
	INT32 ercd;

	if( xch == E_IM_XCH_CH_SEL_0 ){
		// X0ch
		im_xch_on_pclk();	// PCLK on
		Im_XCH_Dsb();

		// Check Xch running
		if( IO_XCH.XCHCTL0.XTRG.bit.XTRG != D_IM_XCH_STOPPED ){

			im_xch_off_pclk();	// PCLK off
			Im_XCH_Dsb();

			Ddim_Print(("I:Im_Xch_Start_Async: Macro busy error.\n"));
			return D_IM_XCH_MACRO_BUSY_NG;
		}

		IO_XCH.XCHICF.word &= D_IM_XCH_ICF_XF0_OFF;
		IO_XCH.XCHICE.bit.XE0 = 1;
		Im_XCH_Dsb();

		im_xch_off_pclk();	// PCLK off
		Im_XCH_Dsb();
	}
	else{
		// X1ch
		im_xch_on_pclk();	// PCLK on
		Im_XCH_Dsb();

		// Check Xch running
		if( IO_XCH.XCHCTL1.XTRG.bit.XTRG != D_IM_XCH_STOPPED ){
			im_xch_off_pclk();	// PCLK off
			Im_XCH_Dsb();

			Ddim_Print(("I:Im_Xch_Start_Async: Macro busy error.\n"));
			return D_IM_XCH_MACRO_BUSY_NG;
		}

		IO_XCH.XCHICF.word &= D_IM_XCH_ICF_XF1_OFF;
		IO_XCH.XCHICE.bit.XE1 = 1;
		Im_XCH_Dsb();

		im_xch_off_pclk();	// PCLK off
		Im_XCH_Dsb();
	}

	gIM_Xch_Mng_Save[xch].sync_type = E_IM_XCH_SYNC_TYPE_ASYNC;

	ercd = im_xch_start_process( xch );
	if( ercd != D_IM_XCH_OK ){
		Ddim_Print(("I:Im_Xch_Start_Async: im_xch_start_process error. ercd = 0x%X\n", ercd));
		return D_IM_XCH_NG;
	}

	return D_IM_XCH_OK;
}

/**
 * @brief		Function to get the maximum frequency of the histogram
 * @return		The maximum appearance frequency of the histogram
 * @note		None
 */
UCHAR Im_Xch_Get_Histogram_Max( VOID )
{
	UCHAR ret;

	im_xch_on_pclk();	// PCLK on
	Im_XCH_Dsb();

	ret = (UCHAR)IO_XCH.XHISTG0.bit.XHMAX0;

	im_xch_off_pclk();	// PCLK off
	Im_XCH_Dsb();

	return ret;
}

/**
 * @brief		Function to get the histogram buffer.
 * @param[out]	*histogram_buf	: Histogram buffer
 * @return		D_IM_XCH_OK, D_IM_XCH_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_Xch_Get_Histogram( ULONG* histogram_buf )
{
	UINT32 cnt;

#ifdef CO_PARAM_CHECK
	if( histogram_buf == NULL ){
		// histogram_buf is Error because of NULL
		Ddim_Assertion(("I:Im_X0ch_Get_Histgram: Null check error. histogram_buf = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	im_xch_on_pclk();	// PCLK on
	Im_XCH_Dsb();

	 if( IO_XCH.XCHCTL0.XTRG.bit.XTRG != D_IM_XCH_STOPPED ){
		im_xch_off_pclk();	// PCLK off
		Im_XCH_Dsb();

		Ddim_Print(("I:Im_Xch_Get_Histogram: Macro busy error.\n"));
		return D_IM_XCH_MACRO_BUSY_NG;
	}

	// Read access enable
	IO_XCH.XHISTG0.bit.XHREN0 = 1;
	Im_XCH_Dsb();

	for( cnt = 0; cnt < 256; cnt++ ){
		histogram_buf[cnt] = IO_XCH.XHBF[cnt].word;
	}

	// Read access disable
	IO_XCH.XHISTG0.bit.XHREN0 = 0;

	im_xch_off_pclk();	// PCLK off
	Im_XCH_Dsb();

	return D_IM_XCH_OK;
}

/**
 * @brief       Wait that the asynchronization processing ends
 * @param[in]   xch		: Channel selection of Xch
 * @return		D_IM_XCH_OK, D_IM_XCH_NG
 * @note        When the executed asynchronization processing ends, it is a return.
 */
INT32 Im_Xch_Wait_End( E_IM_XCH_CH_SEL xch )
{
	DDIM_USER_FLGPTN flg_ptn;
	DDIM_USER_ER ercd;

	if( xch == E_IM_XCH_CH_SEL_0 ){
		// X0ch
		ercd = DDIM_User_Twai_Flg( FID_IM_XCH, D_IM_XCH_FLG_0_WAIT_END, D_DDIM_USER_TWF_ORW, &flg_ptn, D_DDIM_WAIT_END_TIME );
		if( ercd != D_DDIM_USER_E_OK ){
			Ddim_Print(("I:Im_Xch_Wait_End: DDIM_User_Twai_Flg error. ercd = %d\n", ercd));
			return D_IM_XCH_NG;
		}
	}
	else{
		// X1ch
		ercd = DDIM_User_Twai_Flg( FID_IM_XCH, D_IM_XCH_FLG_1_WAIT_END, D_DDIM_USER_TWF_ORW, &flg_ptn, D_DDIM_WAIT_END_TIME );
		if( ercd != D_DDIM_USER_E_OK ){
			Ddim_Print(("I:Im_Xch_Wait_End: DDIM_User_Twai_Flg error. ercd = %d\n", ercd));
			return D_IM_XCH_NG;
		}
	}
	return D_IM_XCH_OK;
}

/**
 * @brief		The semaphore of Xch is returned
 * @param[in]   xch		: Channel selection of Xch
 * @return		D_IM_XCH_OK, D_IM_XCH_SEM_NG
 * @note		None
 */
INT32 Im_Xch_Close( E_IM_XCH_CH_SEL xch )
{
	DDIM_USER_ER ercd;

	// ICLK off
	im_xch_off_iclk();
	Im_XCH_Dsb();

	ercd = DDIM_User_Sig_Sem( SID_IM_XCH(xch) );				// sig_sem()

	if( D_DDIM_USER_E_OK != ercd ){
		// sig_sem error
		Ddim_Print(("I:Im_Xch_Close: Semaphore error : ercd = %d\n", ercd));
		return D_IM_XCH_SEM_NG;
	}

	return D_IM_XCH_OK;
}

/**
 * @brief		Xch function is stopped
 * @return		INT32 D_IM_XCH_OK
 * @attention	None
 */
INT32 Im_Xch_Stop( VOID )
{
	im_xch_on_pclk();	// PCLK on
	Im_XCH_Dsb();

	if( IO_XCH.XCHCTL0.XTRG.bit.XTRG == D_IM_XCH_RUNNING ){
		// Macro Stop
		IO_XCH.XCHCTL0.XTRG.bit.XTRG = D_IM_XCH_STOP;
		Im_XCH_Dsb();

		while( IO_XCH.XCHCTL0.XTRG.bit.XTRG != D_IM_XCH_STOPPED ){

#ifdef CO_DEBUG_ON_PC
			IO_XCH.XCHCTL0.XTRG.bit.XTRG = D_IM_XCH_STOPPED;
#endif //CO_DEBUG_ON_PC

			// Wait until stopped
			DDIM_User_Dly_Tsk(1);
		}
	}
	if( IO_XCH.XCHCTL1.XTRG.bit.XTRG == D_IM_XCH_RUNNING ){
		// Macro Stop
		IO_XCH.XCHCTL1.XTRG.bit.XTRG = D_IM_XCH_STOP;
		Im_XCH_Dsb();

		while( IO_XCH.XCHCTL1.XTRG.bit.XTRG != D_IM_XCH_STOPPED ){
#ifdef CO_DEBUG_ON_PC
			IO_XCH.XCHCTL1.XTRG.bit.XTRG = D_IM_XCH_STOPPED;
#endif //CO_DEBUG_ON_PC

			// Wait until stopped
			DDIM_User_Dly_Tsk(1);
		}
	}

	// Soft reset
	im_xch_softreset();

	im_xch_off_pclk();	// PCLK off
	Im_XCH_Dsb();

	return D_IM_XCH_OK;
}

/**
 * @brief		It is an interrupt handler that starts when the processing of Xch ends
 * @note		None
 */
VOID Im_Xch_Int_Handler( VOID )
{
	im_xch_on_pclk();	// PCLK on
	Im_XCH_Dsb();

	if( IO_XCH.XCHICE.bit.XE0 == 0 && IO_XCH.XCHICE.bit.XE1 == 0 ){
		// Unknown channel error
		Ddim_Print(("I:Im_Xch_Int_Handler: Unknown channel error\n"));
	}
	else if( IO_XCH.XCHICF.bit.__XF0 == 0 && IO_XCH.XCHICF.bit.__XF1 == 0 ){
		// Xch doesn't start error
		Ddim_Print(("I:Im_Xch_Int_Handler: Xch doesn't end error\n"));
	}
	else{
		if( IO_XCH.XCHICE.bit.XE0 == 1 ){
			// X0ch
			if( (IO_XCH.XCHICE.bit.XE0 == 1) && (IO_XCH.XCHICF.bit.__XF0 == 1) ){
				IO_XCH.XCHICF.word &= D_IM_XCH_ICF_XF0_OFF;	// clear interrupt factor flag X0ch
				im_xch_inthand_sub( E_IM_XCH_CH_SEL_0 );
			}
		}
		if( IO_XCH.XCHICE.bit.XE1 == 1 ){
			// X1ch
			if( (IO_XCH.XCHICE.bit.XE1 == 1) && (IO_XCH.XCHICF.bit.__XF1 == 1) ){
				IO_XCH.XCHICF.word &= D_IM_XCH_ICF_XF1_OFF;	// clear interrupt factor flag X1ch
				im_xch_inthand_sub( E_IM_XCH_CH_SEL_1 );
			}
		}
	}

	im_xch_off_pclk();	// PCLK off
	Im_XCH_Dsb();
}


/**
 * @brief		Get Im_Xch_Ctrl_Common param
 * @param[in]	xch				: Channel selection of Xch
 * @param[out]	*xch_ctrl_cmn	: Control parameter
 * @return		D_IM_XCH_OK, D_IM_XCH_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_Xch_Get_Ctrl_Common( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_CMN* xch_ctrl_cmn )
{
#ifdef CO_PARAM_CHECK
	if( xch_ctrl_cmn == NULL ){
		// xch_ctrl_cmn is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Get_Ctrl_Common: Null check error. xch_ctrl_cmn = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	im_xch_on_pclk();	// PCLK on
	Im_XCH_Dsb();

	if( xch == E_IM_XCH_CH_SEL_0 ){
		// X0ch
		xch_ctrl_cmn->mode		= IO_XCH.XCHCTL0.XMD.bit.XMD;
		xch_ctrl_cmn->g_hsize	= IO_XCH.XCHCTL0.XGHW.bit.XGHW;
		xch_ctrl_cmn->d_hsize	= IO_XCH.XCHCTL0.XDHW.bit.XDHW;
		xch_ctrl_cmn->d_vsize	= IO_XCH.XCHCTL0.XDVW.bit.XDVW;
		xch_ctrl_cmn->dst_addr	= IO_XCH.XCHCTL0.XDA.word;
	}
	else{
		// X1ch
		xch_ctrl_cmn->mode		= IO_XCH.XCHCTL1.XMD.bit.XMD;
		xch_ctrl_cmn->g_hsize	= IO_XCH.XCHCTL1.XGHW.bit.XGHW;
		xch_ctrl_cmn->d_hsize	= IO_XCH.XCHCTL1.XDHW.bit.XDHW;
		xch_ctrl_cmn->d_vsize	= IO_XCH.XCHCTL1.XDVW.bit.XDVW;
		xch_ctrl_cmn->dst_addr	= IO_XCH.XCHCTL1.XDA.word;
	}

	im_xch_off_pclk();	// PCLK off
	Im_XCH_Dsb();

	return D_IM_XCH_OK;
}

/**
 * @brief		Get Im_Xch_Ctrl_Thin param
 * @param[in]   xch				: Channel selection of Xch
 * @param[out]	*xch_ctrl_thin	: Control parameter
 * @return		D_IM_XCH_OK, D_IM_XCH_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_Xch_Get_Ctrl_Thin( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_THIN* xch_ctrl_thin )
{
#ifdef CO_PARAM_CHECK
	if( xch_ctrl_thin == NULL ){
		// xch_ctrl_thin is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Get_Ctrl_Thin: Null check error. xch_ctrl_thin = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	im_xch_on_pclk();	// PCLK on
	Im_XCH_Dsb();

	if( xch == E_IM_XCH_CH_SEL_0 ){
		// X0ch
		xch_ctrl_thin->xdtype	= IO_XCH.XCHCTL0.XDTYPE.bit.XSDTYPE;
		xch_ctrl_thin->hcyc		= IO_XCH.XCHCTL0.XCYC.bit.XHCYC;
		xch_ctrl_thin->vcyc		= IO_XCH.XCHCTL0.XCYC.bit.XVCYC;
		xch_ctrl_thin->hen		= IO_XCH.XCHCTL0.XHEN.bit.XHEN;
		xch_ctrl_thin->ven		= IO_XCH.XCHCTL0.XVEN.bit.XVEN;
		xch_ctrl_thin->s_hsize	= IO_XCH.XCHCTL0.XSHW.bit.XSHW;
		xch_ctrl_thin->src_addr	= IO_XCH.XCHCTL0.XSA.word;
	}
	else{
		// X1ch
		xch_ctrl_thin->xdtype	= IO_XCH.XCHCTL1.XDTYPE.bit.XSDTYPE;
		xch_ctrl_thin->hcyc		= IO_XCH.XCHCTL1.XCYC.bit.XHCYC;
		xch_ctrl_thin->vcyc		= IO_XCH.XCHCTL1.XCYC.bit.XVCYC;
		xch_ctrl_thin->hen		= IO_XCH.XCHCTL1.XHEN.bit.XHEN;
		xch_ctrl_thin->ven		= IO_XCH.XCHCTL1.XVEN.bit.XVEN;
		xch_ctrl_thin->s_hsize	= IO_XCH.XCHCTL1.XSHW.bit.XSHW;
		xch_ctrl_thin->src_addr	= IO_XCH.XCHCTL1.XSA.word;
	}

	im_xch_off_pclk();	// PCLK off
	Im_XCH_Dsb();

	return D_IM_XCH_OK;
}

/**
 * @brief		Get Im_Xch_Ctrl_Hist param
 * @param[in]   xch				: Channel selection of Xch
 * @param[out]	*xch_ctrl_hist	: Control parameter
 * @return		D_IM_XCH_OK, D_IM_XCH_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_Xch_Get_Ctrl_Hist( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_HIST* xch_ctrl_hist )
{
#ifdef CO_PARAM_CHECK
	if( xch != E_IM_XCH_CH_SEL_0 ){
		Ddim_Assertion(("I:Im_Xch_Get_Ctrl_Hist: Channel number error. Xch = %d\n", xch));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
	if( xch_ctrl_hist == NULL ){
		// xch_ctrl_hist is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Get_Ctrl_Hist: NULL check error. xch_ctrl_hist=NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	im_xch_on_pclk();	// PCLK on
	Im_XCH_Dsb();

	xch_ctrl_hist->xdtype	= IO_XCH.XCHCTL0.XDTYPE.bit.XSDTYPE;
	xch_ctrl_hist->hcyc		= IO_XCH.XCHCTL0.XCYC.bit.XHCYC;
	xch_ctrl_hist->vcyc		= IO_XCH.XCHCTL0.XCYC.bit.XVCYC;
	xch_ctrl_hist->hen		= IO_XCH.XCHCTL0.XHEN.bit.XHEN;
	xch_ctrl_hist->ven		= IO_XCH.XCHCTL0.XVEN.bit.XVEN;
	xch_ctrl_hist->s_hsize	= IO_XCH.XCHCTL0.XSHW.bit.XSHW;
	xch_ctrl_hist->src_addr	= IO_XCH.XCHCTL0.XSA.word;
	xch_ctrl_hist->hcnt		= IO_XCH.XHISTG0.bit.XHCNT0;

	im_xch_off_pclk();	// PCLK off
	Im_XCH_Dsb();

	return D_IM_XCH_OK;
}

/**
 * @brief		Get Im_Xch_Ctrl_Hist param
 * @param[in]	xch					: Channel selection of Xch
 * @param[out]	*xch_ctrl_half_thin	: Control parameter
 * @return		D_IM_XCH_OK, D_IM_XCH_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_Xch_Get_Ctrl_Half_Thin( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_HALF_THIN* xch_ctrl_half_thin )
{
#ifdef CO_PARAM_CHECK
	if( xch_ctrl_half_thin == NULL ){
		// xch_ctrl_half_thin is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Get_Ctrl_Half_Thin: NULL check error. xch_ctrl_half_thin = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	im_xch_on_pclk();	// PCLK on
	Im_XCH_Dsb();

	if( xch == E_IM_XCH_CH_SEL_0 ){
		// X0ch
		xch_ctrl_half_thin->vcyc		= IO_XCH.XCHCTL0.XCYC.bit.XVCYC;
		xch_ctrl_half_thin->ven			= IO_XCH.XCHCTL0.XVEN.bit.XVEN;
		xch_ctrl_half_thin->s_hsize		= IO_XCH.XCHCTL0.XSHW.bit.XSHW;
		xch_ctrl_half_thin->src_addr	= IO_XCH.XCHCTL0.XSA.word;
	}
	else{
		// X1ch
		xch_ctrl_half_thin->vcyc		= IO_XCH.XCHCTL1.XCYC.bit.XVCYC;
		xch_ctrl_half_thin->ven			= IO_XCH.XCHCTL1.XVEN.bit.XVEN;
		xch_ctrl_half_thin->s_hsize		= IO_XCH.XCHCTL1.XSHW.bit.XSHW;
		xch_ctrl_half_thin->src_addr	= IO_XCH.XCHCTL1.XSA.word;
	}

	im_xch_off_pclk();	// PCLK off
	Im_XCH_Dsb();

	return D_IM_XCH_OK;
}

/**
 * @brief		Get Im_Xch_Ctrl_Hist param
 * @param[in]   xch				: Channel selection of Xch
 * @param[out]	*xch_ctrl_copy	: Control parameter
 * @return		D_IM_XCH_OK, D_IM_XCH_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_Xch_Get_Ctrl_Copy( E_IM_XCH_CH_SEL xch, T_IM_XCH_CTRL_COPY* xch_ctrl_copy )
{
#ifdef CO_PARAM_CHECK
	if( xch_ctrl_copy == NULL ){
		// xch_ctrl_copy is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Get_Ctrl_Copy. NULL check error. xch_ctrl_copy = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	im_xch_on_pclk();	// PCLK on
	Im_XCH_Dsb();

	if( xch == E_IM_XCH_CH_SEL_0 ){
		// X0ch
		xch_ctrl_copy->vcyc		= IO_XCH.XCHCTL0.XCYC.bit.XVCYC;
		xch_ctrl_copy->ven		= IO_XCH.XCHCTL0.XVEN.bit.XVEN;
		xch_ctrl_copy->s_hsize	= IO_XCH.XCHCTL0.XSHW.bit.XSHW;
		xch_ctrl_copy->src_addr	= IO_XCH.XCHCTL0.XSA.word;
	}
	else{
		// X1ch
		xch_ctrl_copy->vcyc		= IO_XCH.XCHCTL1.XCYC.bit.XVCYC;
		xch_ctrl_copy->ven		= IO_XCH.XCHCTL1.XVEN.bit.XVEN;
		xch_ctrl_copy->s_hsize	= IO_XCH.XCHCTL1.XSHW.bit.XSHW;
		xch_ctrl_copy->src_addr	= IO_XCH.XCHCTL1.XSA.word;
	}

	im_xch_off_pclk();	// PCLK off
	Im_XCH_Dsb();

	return D_IM_XCH_OK;
}

/**
 * @brief		Get Im_Xch_Ctrl_Fill param
 * @param[in]   xch			: Channel selection of Xch
 * @param[out]	*fill_data	: Data to fill
 * @return		D_IM_XCH_OK, D_IM_XCH_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_Xch_Get_Ctrl_Fill( E_IM_XCH_CH_SEL xch, UCHAR* fill_data )
{
#ifdef CO_PARAM_CHECK
	if( fill_data == NULL ) {
		// Channel number error
		Ddim_Assertion(("I:Im_Xch_Get_Ctrl_Fill: NULL check error. fill_data = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	im_xch_on_pclk();	// PCLK on
	Im_XCH_Dsb();

	if( xch == E_IM_XCH_CH_SEL_0 ){
		// X0ch
		*fill_data = IO_XCH.XCHCTL0.XFDT.bit.XFDT;
	}
	else{
		// X1ch
		*fill_data = IO_XCH.XCHCTL1.XFDT.bit.XFDT;
	}

	im_xch_off_pclk();	// PCLK off
	Im_XCH_Dsb();

	return D_IM_XCH_OK;
}

/**
 * @brief		Get channel status from start control register
 * @param[in]   xch		: Channel selection of Xch
 * @param[out]  *status	: Status
 * @return		D_IM_XCH_OK, D_IM_XCH_INPUT_PARAM_ERR
 * @note		None
 * @attention	None
 */
INT32 Im_Xch_Get_Xtrg( E_IM_XCH_CH_SEL xch, E_IM_XCH_ST* status )
{
#ifdef CO_PARAM_CHECK
	if( status == NULL ){
		// Channel number error
		Ddim_Assertion(("I:Im_Xch_Get_Xtrg: NULL check error. status = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	im_xch_on_pclk();	// PCLK on
	Im_XCH_Dsb();

	if( xch == E_IM_XCH_CH_SEL_0 ){
		// X0ch
		if( IO_XCH.XCHCTL0.XTRG.bit.XTRG == D_IM_XCH_RUNNING ){
			*status = E_IM_XCH_ST_RUNNING;
		}
		else{
			*status = E_IM_XCH_ST_STOPPED;
		}
	}
	else{
		// X1ch
		if( IO_XCH.XCHCTL1.XTRG.bit.XTRG == D_IM_XCH_RUNNING ){
			*status = E_IM_XCH_ST_RUNNING;
		}
		else{
			*status = E_IM_XCH_ST_STOPPED;
		}
	}

	im_xch_off_pclk();	// PCLK off
	Im_XCH_Dsb();

	return D_IM_XCH_OK;
}

#ifdef D_IM_XCH_PRINT_ST
VOID Im_Xch_Print_ClockStatus( VOID )
{
#ifdef CO_ACT_XCH_PCLOCK
	Ddim_Print(( "CLKSTOP: AP=%u\n", Dd_Top_Get_CLKSTOP_XCHAP() ));
#endif
#ifdef CO_ACT_XCH_ICLOCK
	Ddim_Print(( "CLKSTOP: AX=%u\n", Dd_Top_Get_CLKSTOP_XCHAX() ));
#endif

#ifdef CO_ACT_XCH_PCLOCK
	Ddim_Print(( "CLKCNT: AP=%u\n", gIM_Xch_Pclk_Ctrl_Cnt ));
#endif
#ifdef CO_ACT_XCH_ICLOCK
	Ddim_Print(( "CLKCNT: AX=%u\n", gIM_Xch_Iclk_Ctrl_Cnt ));
#endif

}
#endif // D_IM_XCH_PRINT_ST


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/**
 * @brief		The processing of Xch copy(for sync)
 * @param[in]   E_IM_XCH_CH_SEL	xch			:Channel selection of Xch
 * @param[in]	UINT32			src_addr
 * @param[in]	USHORT			src_gl_width
 * @param[in]	USHORT			dst_gl_width
 * @param[in]	UINT32			dst_addr
 * @param[in]	USHORT			width
 * @param[in]	USHORT			lines
 * @return		D_IM_XCH_OK, D_IM_XCH_NG, D_IM_XCH_SEM_TIMEOUT,
 				D_IM_XCH_SEM_NG, D_IM_XCH_INPUT_PARAM_ERR, D_IM_XCH_MACRO_BUSY_NG
 * @note		None
 * @attention	None
 */
INT32 Im_Xch_Copy( E_IM_XCH_CH_SEL xch, UINT32 src_addr, USHORT src_gl_width, USHORT dst_gl_width, UINT32 dst_addr, USHORT width, USHORT lines )
{
	INT32 ret;

	T_IM_XCH_CTRL_CMN	xch_ctrl_cmn;
	T_IM_XCH_CTRL_COPY	xch_ctrl_copy;

#ifdef CO_PARAM_CHECK
	if( (xch != E_IM_XCH_CH_SEL_0) && (xch != E_IM_XCH_CH_SEL_1) ){
		// Channel number error
		Ddim_Assertion(("I:Im_Xch_Copy: Unknown channel error. Xch = %d\n", xch));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	memset(&xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN));
	memset(&xch_ctrl_copy, 0, sizeof(T_IM_XCH_CTRL_COPY));

	xch_ctrl_cmn.mode		= D_IM_XCH_MD_COPY;
	xch_ctrl_cmn.g_hsize	= dst_gl_width;
	xch_ctrl_cmn.d_hsize	= width;
	xch_ctrl_cmn.d_vsize	= lines;
	xch_ctrl_cmn.dst_addr	= dst_addr;
	xch_ctrl_cmn.pCallBack	= NULL;

	xch_ctrl_copy.vcyc		= 0;
	xch_ctrl_copy.ven		= 0xFFFFFFFF;
	xch_ctrl_copy.s_hsize	= src_gl_width;
	xch_ctrl_copy.src_addr	= src_addr;

	ret = Im_Xch_Open( xch, D_DDIM_WAIT_END_TIME );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Open error
		Ddim_Print(("I:Im_Xch_Copy: Im_Xch_Open error. ret = 0x%X\n", ret));
		return ret;
	}

	ret = Im_Xch_Ctrl_Common( xch, &xch_ctrl_cmn );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Common error
		Im_Xch_Close( xch );
		Ddim_Print(("I:Im_Xch_Copy: Im_Xch_Ctrl_Common error. ret = 0x%X\n", ret));
		return ret;
	}

	ret = Im_Xch_Ctrl_Copy( xch, &xch_ctrl_copy );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Copy error
		Im_Xch_Close( xch );
		Ddim_Print(("I:Im_Xch_Copy: Im_Xch_Ctrl_Copy error. ret = 0x%X\n", ret));
		return ret;
	}

	ret = Im_Xch_Start_Sync( xch );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Close error
		Im_Xch_Close( xch );
		Ddim_Print(("I:Im_Xch_Copy: Im_Xch_Start_Sync error. ret = 0x%X\n", ret));
		return ret;
	}

	ret = Im_Xch_Close( xch );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Close error
		Ddim_Print(("I:Im_Xch_Copy: Im_Xch_Close error. ret = 0x%X\n", ret));
		return ret;
	}

	return D_IM_XCH_OK;

}

/**
 * @brief		The processing of Xch Ex copy(for Sync)
 * @param[in]   *copy_param		: copy necessary data
 * @return		D_IM_XCH_OK, D_IM_XCH_NG, D_IM_XCH_SEM_TIMEOUT,
 				D_IM_XCH_SEM_NG, D_IM_XCH_INPUT_PARAM_ERR, D_IM_XCH_MACRO_BUSY_NG
 * @note		None
 * @attention	None
 */
INT32 Im_Xch_Copy_Ex_Sync( T_IM_XCH_COPY_EX* copy_param )
{
	T_IM_XCH_CTRL_CMN		xch_ctrl_cmn;
	T_IM_XCH_CTRL_THIN		xch_ctrl_thin;
	T_IM_XCH_CTRL_COPY		xch_ctrl_copy;
	INT32					ret = D_IM_XCH_OK;

#ifdef CO_PARAM_CHECK
	if( copy_param == NULL ){
		// copy_param is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Copy_Ex_Sync: Null check error. copy_param = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif	// CO_PARAM_CHECK

	// initialize
	memset( &xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN) );
	memset( &xch_ctrl_thin, 0, sizeof(T_IM_XCH_CTRL_THIN) );
	memset( &xch_ctrl_copy, 0, sizeof(T_IM_XCH_CTRL_COPY) );

	// set call back function
	xch_ctrl_cmn.pCallBack	= NULL;

	// set control structure
	ret = im_xch_conv_copy_param( copy_param, &xch_ctrl_cmn, &xch_ctrl_thin, &xch_ctrl_copy );
	if( ret != D_IM_XCH_OK ){
		// im_xch_prepare_copy_ex_param error
		Ddim_Print(("I:Im_Xch_Copy_Ex_Sync: im_xch_conv_copy_param error. ret = 0x%X\n", ret));
		return ret;
	}

	// Xch open
	ret = Im_Xch_Open( copy_param->xch, D_DDIM_WAIT_END_TIME );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Open error
		Ddim_Print(("I:Im_Xch_Copy_Ex_Sync: Im_Xch_Open error. ret = 0x%X\n", ret));
		return ret;
	}

	// set control
	ret = Im_Xch_Ctrl_Common( copy_param->xch, &xch_ctrl_cmn );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Common error
		Im_Xch_Close( copy_param->xch );
		Ddim_Print(("I:Im_Xch_Copy_Ex_Sync: Im_Xch_Ctrl_Common error. ret = 0x%X\n", ret));
		return ret;
	}

	if( copy_param->copy_type == E_IM_XCH_COPY_SEL_HI ){
		// hi-speed copy
		ret = Im_Xch_Ctrl_Copy( copy_param->xch, &xch_ctrl_copy );
		if( ret != D_IM_XCH_OK ){
			// Im_Xch_Ctrl_Copy error
			Im_Xch_Close( copy_param->xch );
			Ddim_Print(("I:Im_Xch_Copy_Ex_Sync: Im_Xch_Ctrl_Copy error. ret = 0x%X\n", ret));
			return ret;
		}
	}
	else{
		// normal-speed copy
		ret = Im_Xch_Ctrl_Thin( copy_param->xch, &xch_ctrl_thin );
		if( ret != D_IM_XCH_OK ){
			// Im_Xch_Ctrl_Thin error
			Im_Xch_Close( copy_param->xch );
			Ddim_Print(("I:Im_Xch_Copy_Ex_Sync: Im_Xch_Ctrl_Thin error. ret = 0x%X\n", ret));
			return ret;
		}
	}

	// Xch start(Sync)
	ret = Im_Xch_Start_Sync( copy_param->xch );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Start_Sync error
		Im_Xch_Close( copy_param->xch );
		Ddim_Print(("I:Im_Xch_Copy_Ex_Sync: Im_Xch_Start_Sync error. ret = 0x%X\n", ret));
		return ret;
	}

	// Xch close
	ret = Im_Xch_Close( copy_param->xch );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Close error
		Ddim_Print(("I:Im_Xch_Copy_Ex_Sync: Im_Xch_Close error. ret = 0x%X\n", ret));
		return ret;
	}

	return ret;
}

/**
 * @brief		The processing of Xch Ex copy(for Async)
 * @param[in]   *copy_param	: copy necessary data
 * @param[in]   pCallBack	: Call Back function pointer
 * @return		D_IM_XCH_OK, D_IM_XCH_NG, D_IM_XCH_SEM_TIMEOUT,
 				D_IM_XCH_SEM_NG, D_IM_XCH_INPUT_PARAM_ERR, D_IM_XCH_MACRO_BUSY_NG
 * @note		None
 * @attention	None
 */
INT32 Im_Xch_Copy_Ex_Async( T_IM_XCH_COPY_EX* copy_param, VP_CALLBACK pCallBack )
{
	T_IM_XCH_CTRL_CMN		xch_ctrl_cmn;
	T_IM_XCH_CTRL_THIN		xch_ctrl_thin;
	T_IM_XCH_CTRL_COPY		xch_ctrl_copy;
	INT32					ret = D_IM_XCH_OK;

#ifdef CO_PARAM_CHECK
	if( copy_param == NULL ){
		// copy_param is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Copy_Ex_Async: Null check error. copy_param = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif	// CO_PARAM_CHECK

	// initialize
	memset( &xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN) );
	memset( &xch_ctrl_thin, 0, sizeof(T_IM_XCH_CTRL_THIN) );
	memset( &xch_ctrl_copy, 0, sizeof(T_IM_XCH_CTRL_COPY) );

	// set call back function
	xch_ctrl_cmn.pCallBack	= pCallBack;

	// set control structure
	ret = im_xch_conv_copy_param( copy_param, &xch_ctrl_cmn, &xch_ctrl_thin, &xch_ctrl_copy );
	if( ret != D_IM_XCH_OK ){
		// im_xch_prepare_copy_ex_param error
		Ddim_Print(("I:Im_Xch_Copy_Ex_Async: im_xch_conv_copy_param error. ret = 0x%X\n", ret));
		return ret;
	}

	// set control
	ret = Im_Xch_Ctrl_Common( copy_param->xch, &xch_ctrl_cmn );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Common error
		Ddim_Print(("I:Im_Xch_Copy_Ex_Async: Im_Xch_Ctrl_Common error. ret = 0x%X\n", ret));
		return ret;
	}

	if( copy_param->copy_type == E_IM_XCH_COPY_SEL_HI ){
		// hi-speed copy
		ret = Im_Xch_Ctrl_Copy( copy_param->xch, &xch_ctrl_copy );
		if( ret != D_IM_XCH_OK ){
			// Im_Xch_Ctrl_Copy error
			Ddim_Print(("I:Im_Xch_Copy_Ex_Async: Im_Xch_Ctrl_Copy error. ret = 0x%X\n", ret));
			return ret;
		}
	}
	else{
		// normal-speed copy
		ret = Im_Xch_Ctrl_Thin( copy_param->xch, &xch_ctrl_thin );
		if( ret != D_IM_XCH_OK ){
			// Im_Xch_Ctrl_Thin error
			Ddim_Print(("I:Im_Xch_Copy_Ex_Async: Im_Xch_Ctrl_Thin error. ret = 0x%X\n", ret));
			return ret;
		}
	}

	// Xch start(Async)
	ret = Im_Xch_Start_Async( copy_param->xch );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Start_Async error
		Ddim_Print(("I:Im_Xch_Copy_Ex_Async: Im_Xch_Start_Async error. ret = 0x%X\n", ret));
		return ret;
	}

	return ret;
}

/**
 * @brief		The processing of Xch fill(for sync)
 * @param[in]   E_IM_XCH_CH_SEL	xch			: Channel selection of Xch
 * @param[in]	UCHAR			fill_data
 * @param[in]	USHORT			gl_width
 * @param[in]	UINT32			dst_addr
 * @param[in]	USHORT			width
 * @param[in]	USHORT			lines
 * @return		D_IM_XCH_OK, D_IM_XCH_NG, D_IM_XCH_SEM_TIMEOUT,
 				D_IM_XCH_SEM_NG, D_IM_XCH_INPUT_PARAM_ERR, D_IM_XCH_MACRO_BUSY_NG
 * @note		None
 * @attention	None
 */
INT32 Im_Xch_Fill( E_IM_XCH_CH_SEL xch, UCHAR fill_data, USHORT gl_width, UINT32 dst_addr, USHORT width, USHORT lines )
{
	INT32 ret;
	T_IM_XCH_CTRL_CMN	xch_ctrl_cmn;

#ifdef CO_PARAM_CHECK
	if( (xch != E_IM_XCH_CH_SEL_0) && (xch != E_IM_XCH_CH_SEL_1) ){
		// Channel number error
		Ddim_Assertion(("I:Im_Xch_Fill: Channel number error. xch = %d\n", xch));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	memset(&xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN));

	// set control structure
	xch_ctrl_cmn.mode		= D_IM_XCH_MD_FILL;
	xch_ctrl_cmn.g_hsize	= gl_width;
	xch_ctrl_cmn.d_hsize	= width;
	xch_ctrl_cmn.d_vsize	= lines;
	xch_ctrl_cmn.dst_addr	= dst_addr;
	xch_ctrl_cmn.pCallBack	= NULL;

	ret = Im_Xch_Open( xch, D_DDIM_WAIT_END_TIME );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Open error
		Ddim_Print(("I:Im_Xch_Fill: Im_Xch_Open error. ret = 0x%X\n", ret));
		return ret;
	}

	ret = Im_Xch_Ctrl_Common( xch, &xch_ctrl_cmn );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Common error
		Im_Xch_Close( xch );
		Ddim_Print(("I:Im_Xch_Fill: Im_Xch_Ctrl_Common error. ret = 0x%X\n", ret));
		return ret;
	}

	ret = Im_Xch_Ctrl_Fill( xch, fill_data );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Fill error
		Im_Xch_Close( xch );
		Ddim_Print(("I:Im_Xch_Fill: Im_Xch_Ctrl_Fill error. ret = 0x%X\n", ret));
		return ret;
	}

	ret = Im_Xch_Start_Sync(xch);
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Start_Sync error
		Im_Xch_Close( xch );
		Ddim_Print(("I:Im_Xch_Fill: Im_Xch_Start_Sync error. ret = 0x%X\n", ret));
	}

	ret = Im_Xch_Close(xch);
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Close error
		Ddim_Print(("I:Im_Xch_Fill: Im_Xch_Close error. ret = 0x%X\n", ret));
		return ret;
	}

	return D_IM_XCH_OK;
}

/**
 * @brief		The processing of Xch fill(for Sync)
 * @param[in]   *fill_param		: fill necessary data
 * @return		D_IM_XCH_OK, D_IM_XCH_NG, D_IM_XCH_SEM_TIMEOUT,
 				D_IM_XCH_SEM_NG, D_IM_XCH_INPUT_PARAM_ERR, D_IM_XCH_MACRO_BUSY_NG
 * @note		None
 * @attention	None
 */
INT32 Im_Xch_Fill_Ex_Sync( T_IM_XCH_FILL_EX* fill_param )
{
	INT32				ret = D_IM_XCH_OK;
	T_IM_XCH_CTRL_CMN	xch_ctrl_cmn;

#ifdef CO_PARAM_CHECK
	if( fill_param == NULL ){
		// fill_param is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Fill_Ex_Sync: Null check error. fill_param = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif	// CO_PARAM_CHECK

	// initialize
	memset( &xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN) );

	// set control structur
	xch_ctrl_cmn.pCallBack	= NULL;

	// set common control structure
	im_xch_conv_fill_param( fill_param, &xch_ctrl_cmn );

	// Xch open
	ret = Im_Xch_Open( fill_param->xch, D_DDIM_WAIT_END_TIME );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Open error
		Ddim_Print(("I:Im_Xch_Fill_Ex_Sync: Im_Xch_Open error. ret = 0x%X\n", ret));
		return ret;
	}

	// set control
	ret = Im_Xch_Ctrl_Common( fill_param->xch, &xch_ctrl_cmn );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Common error
		Im_Xch_Close( fill_param->xch );
		Ddim_Print(("I:Im_Xch_Fill_Ex_Sync: Im_Xch_Ctrl_Common error. ret = 0x%X\n", ret));
		return ret;
	}
	ret = Im_Xch_Ctrl_Fill( fill_param->xch, fill_param->fill_data );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Fill error
		Im_Xch_Close( fill_param->xch );
		Ddim_Print(("I:Im_Xch_Fill_Ex_Sync: Im_Xch_Ctrl_Fill error. ret = 0x%X\n", ret));
		return ret;
	}

	// Xch start(Sync)
	ret = Im_Xch_Start_Sync( fill_param->xch );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Start_Sync error
		Im_Xch_Close( fill_param->xch );
		Ddim_Print(("I:Im_Xch_Fill_Ex_Sync: Im_Xch_Start_Sync error. ret = 0x%X\n", ret));
		return ret;
	}

	// Xch close
	ret = Im_Xch_Close( fill_param->xch );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Close error
		Ddim_Print(("I:Im_Xch_Fill_Ex_Sync: Im_Xch_Close error. ret = 0x%X\n", ret));
		return ret;
	}

	return ret;
}

/**
 * @brief		The processing of Xch fill(for Async)
 * @param[in]   *fill_param	: fill necessary data
 * @param[in]	pCallBack	: Call Back function pointer
 * @return		D_IM_XCH_OK, D_IM_XCH_NG, D_IM_XCH_SEM_TIMEOUT,
 				D_IM_XCH_SEM_NG, D_IM_XCH_INPUT_PARAM_ERR, D_IM_XCH_MACRO_BUSY_NG
 * @note		None
 * @attention	None
 */
INT32 Im_Xch_Fill_Ex_Async( T_IM_XCH_FILL_EX* fill_param, VP_CALLBACK pCallBack )
{
	T_IM_XCH_CTRL_CMN	xch_ctrl_cmn;
	INT32				ret = D_IM_XCH_OK;

#ifdef CO_PARAM_CHECK
	if( fill_param == NULL ){
		// fill_param is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Fill_Ex_Async: Null check error. fill_param = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif	// CO_PARAM_CHECK

	// initialize
	memset( &xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN) );

	// set control structure
	xch_ctrl_cmn.pCallBack	= pCallBack;

	// set common control structure
	im_xch_conv_fill_param( fill_param, &xch_ctrl_cmn );

	// set control
	ret = Im_Xch_Ctrl_Common( fill_param->xch, &xch_ctrl_cmn );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Common error
		Ddim_Print(("I:Im_Xch_Fill_Ex_Async: Im_Xch_Ctrl_Common error. ret = 0x%X\n", ret));
		return ret;
	}
	ret = Im_Xch_Ctrl_Fill( fill_param->xch, fill_param->fill_data );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Fill error
		Ddim_Print(("I:Im_Xch_Fill_Ex_Async: Im_Xch_Ctrl_Fill error. ret = 0x%X\n", ret));
		return ret;
	}

	// Xch start(Async)
	ret = Im_Xch_Start_Async( fill_param->xch );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Start_Async error
		Ddim_Print(("I:Im_Xch_Fill_Ex_Async: Im_Xch_Start_Async error. ret = 0x%X\n", ret));
		return ret;
	}

	return ret;
}

/**
 * @brief		The processing of Xch histogram(for Sync)
 * @param[in]	*hist_param			: histogram necessary data
 * @param[out]	*hist_buffer			: histogram data buffer
 * @param[out]	*hist_max			: maximum appearance frequency
 * @return		D_IM_XCH_OK, D_IM_XCH_NG, D_IM_XCH_SEM_TIMEOUT,
 				D_IM_XCH_SEM_NG, D_IM_XCH_INPUT_PARAM_ERR, D_IM_XCH_MACRO_BUSY_NG
 * @note		None
 * @attention   None
 */
INT32 Im_Xch_Examine_Histogram_Sync( T_IM_XCH_EXAMINE_HIST* hist_param, ULONG* hist_buffer, UCHAR* hist_max )
{
	T_IM_XCH_CTRL_CMN	xch_ctrl_cmn;
	T_IM_XCH_CTRL_HIST	xch_ctrl_hist;
	INT32				ret = D_IM_XCH_OK;

#ifdef CO_PARAM_CHECK
	if( hist_param == NULL ){
		// hist_param is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Examine_Histogram_Sync: Null check error. hist_param = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
	if( hist_max == NULL ){
		// hist_param is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Examine_Histogram_Sync: Null check error. hist_max = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif	// CO_PARAM_CHECK

	// initialize
	memset( &xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN) );
	memset( &xch_ctrl_hist, 0, sizeof(T_IM_XCH_CTRL_HIST) );

	// set call back function
	xch_ctrl_cmn.pCallBack	= NULL;

	// set control structure
	im_xch_conv_hist_param( hist_param, &xch_ctrl_cmn, &xch_ctrl_hist );

	// X0ch open
	ret = Im_Xch_Open( E_IM_XCH_CH_SEL_0, D_DDIM_WAIT_END_TIME );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Open error
		Ddim_Print(("I:Im_Xch_Examine_Histogram_Sync: Im_Xch_Open error. ret = 0x%X\n", ret));
		return ret;
	}

	// set control
	ret = Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL_0, &xch_ctrl_cmn );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Common error
		Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
		Ddim_Print(("I:Im_Xch_Examine_Histogram_Sync: Im_Xch_Ctrl_Common error. ret = 0x%X\n", ret));
		return ret;
	}

	ret = Im_Xch_Ctrl_Hist( E_IM_XCH_CH_SEL_0, &xch_ctrl_hist );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Hist error
		Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
		Ddim_Print(("I:Im_Xch_Examine_Histogram_Sync: Im_Xch_Ctrl_Hist error. ret = 0x%X\n", ret));
		return ret;
	}

	// Xch start(Sync)
	ret = Im_Xch_Start_Sync( E_IM_XCH_CH_SEL_0 );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Start_Sync error
		Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
		Ddim_Print(("I:Im_Xch_Examine_Histogram_Sync: Im_Xch_Start_Sync error. ret = 0x%X\n", ret));
		return ret;
	}

#ifdef CO_DEBUG_ON_PC
	IO_XCH.XCHCTL0.XTRG.bit.XTRG = 2;
#endif //CO_DEBUG_ON_PC

	// Get histogram buffer
	ret = Im_Xch_Get_Histogram( hist_buffer );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Start_Sync error
		Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
		Ddim_Print(("I:Im_Xch_Examine_Histogram_Sync: Im_Xch_Get_Histogram error. ret = 0x%X\n", ret));
		return ret;
	}

	// Get histogram max
	*hist_max = Im_Xch_Get_Histogram_Max();

	// X0ch close
	ret = Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Close error
		Ddim_Print(("I:Im_Xch_Examine_Histogram_Sync: Im_Xch_Close error. ret = 0x%X\n", ret));
		return ret;
	}

	return ret;
}

/**
 * @brief		The processing of Xch histogram(for Async)
 * @param[in]	*hist_param			: histogram necessary data
 * @param[in]	pCallBack			: Call Back function pointer
 * @return		D_IM_XCH_OK, D_IM_XCH_NG, D_IM_XCH_SEM_TIMEOUT,
 				D_IM_XCH_SEM_NG, D_IM_XCH_INPUT_PARAM_ERR, D_IM_XCH_MACRO_BUSY_NG
 * @note		None
 * @attention   None
 */
INT32 Im_Xch_Examine_Histogram_Async( T_IM_XCH_EXAMINE_HIST* hist_param, VP_CALLBACK pCallBack )
{
	T_IM_XCH_CTRL_CMN	xch_ctrl_cmn;
	T_IM_XCH_CTRL_HIST	xch_ctrl_hist;
	INT32				ret = D_IM_XCH_OK;

#ifdef CO_PARAM_CHECK
	if( hist_param == NULL ){
		// hist_param is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Examine_Histogram_Async: Null check error. hist_param = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif	// CO_PARAM_CHECK

	// initialize
	memset( &xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN) );
	memset( &xch_ctrl_hist, 0, sizeof(T_IM_XCH_CTRL_HIST) );

	// set call back function
	xch_ctrl_cmn.pCallBack	= pCallBack;

	// set control structure
	im_xch_conv_hist_param( hist_param, &xch_ctrl_cmn, &xch_ctrl_hist );

	// set control
	ret = Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL_0, &xch_ctrl_cmn );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Common error
		Ddim_Print(("I:Im_Xch_Examine_Histogram_Async: Im_Xch_Ctrl_Common error. ret = 0x%X\n", ret));
		return ret;
	}
	ret = Im_Xch_Ctrl_Hist( E_IM_XCH_CH_SEL_0, &xch_ctrl_hist );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Hist error
		Ddim_Print(("I:Im_Xch_Examine_Histogram_Async: Im_Xch_Ctrl_Hist error. ret = 0x%X\n", ret));
		return ret;
	}

	// Xch start(Async)
	ret = Im_Xch_Start_Async( E_IM_XCH_CH_SEL_0 );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Start_Async error
		Ddim_Print(("I:Im_Xch_Examine_Histogram_Async: Im_Xch_Start_Async error. ret = 0x%X\n", ret));
		return ret;
	}

	return ret;
}

/**
 * @brief		The processing of Xch down sampling(for Sync)
 * @param[in]   *thin_param		: Down sampling necessary data
 * @return		D_IM_XCH_OK, D_IM_XCH_NG, D_IM_XCH_SEM_TIMEOUT,
 				D_IM_XCH_SEM_NG, D_IM_XCH_INPUT_PARAM_ERR, D_IM_XCH_MACRO_BUSY_NG
 * @note		None
 * @attention	None
 */
INT32 Im_Xch_Convert_Thinning_Sync( T_IM_XCH_CONV_THIN* thin_param )
{
	T_IM_XCH_CTRL_CMN		xch_ctrl_cmn;
	T_IM_XCH_CTRL_THIN		xch_ctrl_thin;
	INT32					ret = D_IM_XCH_OK;

#ifdef CO_PARAM_CHECK
	if( thin_param == NULL ){
		// thin_param is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Convert_Thinning_Sync. Null check error. thin_param = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif	// CO_PARAM_CHECK

	// initialize
	memset( &xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN) );
	memset( &xch_ctrl_thin, 0, sizeof(T_IM_XCH_CTRL_THIN) );

	// set call back function
	xch_ctrl_cmn.pCallBack	= NULL;

	// set control structure
	ret = im_xch_conv_thin_param( thin_param, &xch_ctrl_cmn, &xch_ctrl_thin );
	if( ret != D_IM_XCH_OK ){
		// im_xch_conv_thin_param error
		Ddim_Print(("I:Im_Xch_Convert_Thinning_Sync: im_xch_conv_thin_param error. ret = 0x%X\n", ret));
		return ret;
	}

	// Xch open
	ret = Im_Xch_Open( thin_param->xch, D_DDIM_WAIT_END_TIME );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Open error
		Ddim_Print(("I:Im_Xch_Convert_Thinning_Sync: Im_Xch_Open error. ret = 0x%X\n", ret));
		return ret;
	}

	// set control
	ret = Im_Xch_Ctrl_Common( thin_param->xch, &xch_ctrl_cmn );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Common error
		Im_Xch_Close( thin_param->xch );
		Ddim_Print(("I:Im_Xch_Convert_Thinning_Sync: Im_Xch_Ctrl_Common error. ret = 0x%X\n", ret));
		return ret;
	}
	ret = Im_Xch_Ctrl_Thin( thin_param->xch, &xch_ctrl_thin );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Fill error
		Im_Xch_Close( thin_param->xch );
		Ddim_Print(("I:Im_Xch_Convert_Thinning_Sync: Im_Xch_Ctrl_Thin error. ret = 0x%X\n", ret));
		return ret;
	}

	// Xch start(Sync)
	ret = Im_Xch_Start_Sync( thin_param->xch );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Start_Sync error
		Im_Xch_Close( thin_param->xch );
		Ddim_Print(("I:Im_Xch_Convert_Thinning_Sync: Im_Xch_Start_Sync error. ret = 0x%X\n", ret));
		return ret;
	}

	// Xch close
	ret = Im_Xch_Close( thin_param->xch );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Close error
		Ddim_Print(("I:Im_Xch_Convert_Thinning_Sync: Im_Xch_Close error. ret = 0x%X\n", ret));
		return ret;
	}

	return ret;
}

/**
 * @brief		The processing of Xch Down sampling(for Async)
 * @param[in]   *thin_param		: Down sampling necessary data
 * @param[in]	pCallBack		: Call Back function pointer
 * @return		D_IM_XCH_OK, D_IM_XCH_NG, D_IM_XCH_SEM_TIMEOUT,
 				D_IM_XCH_SEM_NG, D_IM_XCH_INPUT_PARAM_ERR, D_IM_XCH_MACRO_BUSY_NG
 * @note		None
 * @attention	None
 */
INT32 Im_Xch_Convert_Thinning_Async( T_IM_XCH_CONV_THIN* thin_param, VP_CALLBACK pCallBack )
{
	T_IM_XCH_CTRL_CMN		xch_ctrl_cmn;
	T_IM_XCH_CTRL_THIN		xch_ctrl_thin;
	INT32					ret = D_IM_XCH_OK;

#ifdef CO_PARAM_CHECK
	if( thin_param == NULL ){
		// thin_param is Error because of NULL
		Ddim_Assertion(("I:Im_Xch_Convert_Thinning_Async. thin_param = NULL\n"));
		return D_IM_XCH_INPUT_PARAM_ERR;
	}
#endif	// CO_PARAM_CHECK

	// initialize
	memset( &xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN) );
	memset( &xch_ctrl_thin, 0, sizeof(T_IM_XCH_CTRL_THIN) );

	// set call back function
	xch_ctrl_cmn.pCallBack	= pCallBack;

	// set control structure
	ret = im_xch_conv_thin_param( thin_param, &xch_ctrl_cmn, &xch_ctrl_thin );
	if( ret != D_IM_XCH_OK ){
		// im_xch_conv_thin_param error
		Ddim_Print(("I:Im_Xch_Convert_Thinning_Async: im_xch_conv_thin_param error. ret = 0x%X\n", ret));
		return ret;
	}

	// set control
	ret = Im_Xch_Ctrl_Common( thin_param->xch, &xch_ctrl_cmn );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Common error
		Ddim_Print(("I:Im_Xch_Convert_Thinning_Async: Im_Xch_Ctrl_Common error. ret = 0x%X\n", ret));
		return ret;
	}

	ret = Im_Xch_Ctrl_Thin( thin_param->xch, &xch_ctrl_thin );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Ctrl_Fill error
		Ddim_Print(("I:Im_Xch_Convert_Thinning_Async: Im_Xch_Ctrl_Thin error. ret = 0x%X\n", ret));
		return ret;
	}

	// Xch start(Async)
	ret = Im_Xch_Start_Async( thin_param->xch );
	if( ret != D_IM_XCH_OK ){
		// Im_Xch_Start_Async error
		Ddim_Print(("I:Im_Xch_Convert_Thinning_Async: Im_Xch_Start_Async error. ret = 0x%X\n", ret));
		return ret;
	}

	return ret;
}

/**
 * @brief		The processing of Xch copy (Sync)
 * @param[in]	dst_addr	: copy destination address
 * @param[in]	src_addr	: copy source address
 * @param[in]	size		: copy size
 * @return		D_IM_XCH_OK, D_IM_XCH_NG, D_IM_XCH_SEM_TIMEOUT,
 				D_IM_XCH_SEM_NG, D_IM_XCH_INPUT_PARAM_ERR, D_IM_XCH_MACRO_BUSY_NG
 * @note		Processed using a double-channel.
 * @attention	None
 */
INT32 Im_Xch_Copy_Dual_Channel( VOID* dst_addr, VOID* src_addr, INT32 size )
{
	INT32	ret = D_IM_XCH_OK;
	UINT32	src;
	UINT32	dst;
	USHORT	vsize;

	src = (UINT32)src_addr;
	dst = (UINT32)dst_addr;

	while( size >= 0x4000000 ){
#ifdef CO_DEBUG_ON_PC
		Ddim_Print(("I:Im_Xch_Copy_Dual_Channel: 0x4000000 copy root.\n"));
#endif //CO_DEBUG_ON_PC

		// 8192 x 8192 byte copy
		ret = im_xch_copy_2ch( src, 0x2000, 0x2000, dst, 0x2000, 0x2000 );
		if( ret != D_IM_XCH_OK ){
			// im_xch_copy_2ch error
			Ddim_Print(("I:Im_Xch_Copy_Dual_Channel: im_xch_copy_2ch error. ret = 0x%X\n", ret));
			return ret;
		}
		size -= 0x4000000;
		src += 0x4000000;
		dst += 0x4000000;
	}

	while( size >= 0x4000 ){
#ifdef CO_DEBUG_ON_PC
		Ddim_Print(("I:Im_Xch_Copy_Dual_Channel: 0x4000 copy root.\n"));
#endif //CO_DEBUG_ON_PC

		// 8192 * 2 byte copy
		vsize = size >> 13;
		ret = im_xch_copy_2ch( src, 0x2000, 0x2000, dst, 0x2000, vsize );
		if( ret != D_IM_XCH_OK ){
			// im_xch_copy_2ch error
			Ddim_Print(("I:Im_Xch_Copy_Dual_Channel: im_xch_copy_2ch error. ret = 0x%X\n", ret));
			return ret;
		}
		size -= vsize * 0x2000;
		src += vsize * 0x2000;
		dst += vsize * 0x2000;
	}

	while( size >= 0x2000 ){
#ifdef CO_DEBUG_ON_PC
		Ddim_Print(("I:Im_Xch_Copy_Dual_Channel: 0x2000 copy root.\n"));
#endif //CO_DEBUG_ON_PC

		// 8192 byte copy
		vsize = size >> 13;
		ret = im_xch_copy_byte( src, 0x2000, 0x2000, dst, 0x2000, vsize );
		if( ret != D_IM_XCH_OK ){
			// im_xch_copy_2ch error
			Ddim_Print(("I:Im_Xch_Copy_Dual_Channel: im_xch_copy_byte error. ret = 0x%X\n", ret));
			return ret;
		}
		size -= vsize * 0x2000;
		src += vsize * 0x2000;
		dst += vsize * 0x2000;
	}

	if( size > 0 ){
#ifdef CO_DEBUG_ON_PC
		Ddim_Print(("I:Im_Xch_Copy_Dual_Channel: 0 copy root.\n"));
#endif //CO_DEBUG_ON_PC

		// 
		ret = im_xch_copy_byte( src, 0x2000, 0x2000, dst, size, 1 );
		if( ret != D_IM_XCH_OK ){
			// im_xch_copy_byte error
			Ddim_Print(("I:Im_Xch_Copy_Dual_Channel: im_xch_copy_byte error. ret = 0x%X\n", ret));
			return ret;
		}
	}

	return ret;
}

//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE
