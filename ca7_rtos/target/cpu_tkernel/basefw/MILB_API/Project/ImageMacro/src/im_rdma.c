/**
 * @file		im_rdma.c
 * @brief		JDSRDMA Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "im_rdma.h"

#if defined(CO_ACT_ICLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
#endif
#include "dd_arm.h"

#include "ddim_user_custom.h"
#include "jdsrdma.h"

#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// for debug
#define CO_IM_RDMA_DEBUG

#ifdef CO_ACT_PCLOCK
#define CO_ACT_RDMA_PCLOCK
#endif //CO_ACT_PCLOCK
#ifdef CO_ACT_ICLOCK
#define CO_ACT_RDMA_ICLOCK
#endif //CO_ACT_ICLOCK
#ifdef CO_ACT_CLOCK
#define CO_ACT_RDMA_CLOCK
#endif //CO_ACT_CLOCK

#define D_IM_RDMA_APCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP14())
#define D_IM_RDMA_AXCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP14())
#define D_IM_RDMA_CLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP14())

#define D_IM_RDMA_PCLK_REG_BIT		D_DD_TOP_RDMAAP_BIT		// bit30
#define D_IM_RDMA_ICLK_REG_BIT		D_DD_TOP_RDMAAX_BIT		// bit28
#define D_IM_RDMA_CLK_REG_BIT		D_DD_TOP_RDMACK_BIT		// bit26

// trigger pattern for write (Core)
#define	D_IM_RDMA_TRG_START				(1)
#define	D_IM_RDMA_TRG_STOP				(2)
// trigger pattern for read (Core)
#define	D_IM_RDMA_TRG_STOPPED			(2)
#define	D_IM_RDMA_TRG_RUNNING			(3)

// trigger pattern for write (PRch)
#define	D_IM_RDMA_PRCH_TRG_STOP			(0)
#define	D_IM_RDMA_PRCH_TRG_START		(1)
#define	D_IM_RDMA_PRCH_TRG_STOP_FORCE	(2)

// trigger pattern for read (PRch)
#define	D_IM_RDMA_PRCH_TRG_STOPPED		(2)
#define	D_IM_RDMA_PRCH_TRG_RUNNING		(3)

// trigger status
// wait flag pattern
#define D_IM_RDMA_WAIT_FLGPTN_OR		(0x10000000)
#define D_IM_RDMA_WAIT_FLGPTN_AND		(0x20000000)
#define D_IM_RDMA_WAIT_FLGPTN_AND_ERR	(0x40000000)
#define D_IM_RDMA_WAIT_FLGPTN_ALL		(0x70000000)

// macro
#define Im_RDMA_Dsb()				Dd_ARM_Dsb_Pou()

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------

//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Clock control counter (0~255)
#ifdef CO_ACT_RDMA_PCLOCK
static volatile UCHAR gIM_Rdma_Pclk_Ctrl_Cnt = 0;
#endif // CO_ACT_RDMA_PCLOCK
#ifdef CO_ACT_RDMA_ICLOCK
static volatile UCHAR gIM_Rdma_Iclk_Ctrl_Cnt = 0;
#endif // CO_ACT_RDMA_ICLOCK
#ifdef CO_ACT_RDMA_CLOCK
static volatile UCHAR gIM_Rdma_Clk_Ctrl_Cnt = 0;
#endif // CO_ACT_RDMA_CLOCK

// interrupt mode
static volatile E_IM_RDMA_INT_MODE	gIM_Rdma_Int_Mode = E_IM_RDMA_INT_MODE_AND_ERR;

static volatile T_IM_RDMA_CALLBACK	gIM_Rdma_CallBack_Func = NULL;

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
 * @brief			APCLK ON for RDMA
 * @param[in]		None
 * @param[out]		None
 * @retval			None
 */
static VOID im_rdma_on_pclk( VOID )
{
#ifdef CO_ACT_RDMA_PCLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_Rdma_Pclk_Ctrl_Cnt, D_IM_RDMA_APCLK_REG_ADDR, ~D_IM_RDMA_PCLK_REG_BIT );
#endif	// CO_ACT_RDMA_PCLOCK
}

/**
 * @brief			APCLK OFF for RDMA
 * @param[in]		None
 * @param[out]		None
 * @retval			None
 */
static VOID im_rdma_off_pclk( VOID )
{
#ifdef CO_ACT_RDMA_PCLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_Rdma_Pclk_Ctrl_Cnt, D_IM_RDMA_APCLK_REG_ADDR, D_IM_RDMA_PCLK_REG_BIT );
#endif	// CO_ACT_RDMA_PCLOCK
}

/**
 * @brief			AXCLK&CLK ON for RDMA
 * @brief			
 * @param[in]		None
 * @param[out]		None
 * @retval			None
 */
static VOID im_rdma_on_clk( VOID )
{
#ifdef CO_ACT_RDMA_CLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_Rdma_Clk_Ctrl_Cnt, D_IM_RDMA_CLK_REG_ADDR, ~D_IM_RDMA_CLK_REG_BIT );
#endif	// CO_ACT_RDMA_CLOCK
#ifdef CO_ACT_RDMA_ICLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_Rdma_Iclk_Ctrl_Cnt, D_IM_RDMA_AXCLK_REG_ADDR, ~D_IM_RDMA_ICLK_REG_BIT );
#endif	// CO_ACT_RDMA_ICLOCK
}

/**
 * @brief			AXCLK&CLK OFF for RDMA
 * @param[in]		None
 * @param[out]		None
 * @retval			None
 */
static VOID im_rdma_off_clk( VOID )
{
#ifdef CO_ACT_RDMA_ICLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_Rdma_Iclk_Ctrl_Cnt, D_IM_RDMA_AXCLK_REG_ADDR, D_IM_RDMA_ICLK_REG_BIT );
#endif	// CO_ACT_RDMA_ICLOCK
#ifdef CO_ACT_RDMA_CLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_Rdma_Clk_Ctrl_Cnt, D_IM_RDMA_CLK_REG_ADDR, D_IM_RDMA_CLK_REG_BIT );
#endif	// CO_ACT_RDMA_CLOCK
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		The semaphore of RDMA is acquired
 * @param[in]	tmout	: Time-out period
 * @return		D_IM_RDMA_OK, D_IM_RDMA_INPUT_PARAM_ERR, D_IM_RDMA_SEM_TIMEOUT, D_IM_RDMA_SEM_NG
 * @note		None
 */
INT32 Im_RDMA_Open( INT32 tmout )
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if( tmout < D_DDIM_USER_SEM_WAIT_FEVR ){
		Ddim_Assertion(("I:Im_RDMA_Open: input param error. tmout = 0x%x\n", tmout));
		return D_IM_RDMA_INPUT_PARAM_ERR;
	}
#endif

	if( D_DDIM_USER_SEM_WAIT_POL == tmout ){
		ercd = DDIM_User_Pol_Sem( SID_IM_RDMA );	// pol_sem()
	}
	else{
		ercd = DDIM_User_Twai_Sem( SID_IM_RDMA, (DDIM_USER_TMO)tmout );		// twai_sem()
	}

	if( D_DDIM_USER_E_OK != ercd ){
		// twai_sem error
		if( D_DDIM_USER_E_TMOUT == ercd ){
			// Semaphore Get Time Out
			Ddim_Print(("I:Im_RDMA_Open() Error : Semaphore Get Time Out. ercd = 0x%x\n", ercd));
			return D_IM_RDMA_SEM_TIMEOUT;
		}
		// Semaphore Get Error
		Ddim_Print(("I:Im_RDMA_Open() Error : Semaphore Get Error. ercd = 0x%x\n", ercd));
		return D_IM_RDMA_SEM_NG;
	}

	return D_IM_RDMA_OK;
}

/**
 * @brief		The semaphore of RDMA is returned
 * @return		D_IM_RDMA_OK, D_IM_RDMA_SEM_NG
 * @note		None
 */
INT32 Im_RDMA_Close( VOID )
{
	DDIM_USER_ER ercd;

	ercd = DDIM_User_Sig_Sem( SID_IM_RDMA );				// sig_sem()
	if( D_DDIM_USER_E_OK != ercd ){
		// sig_sem error
		Ddim_Print(("I:Im_RDMA_Close: Semaphore error : ercd = 0x%x\n", ercd));
		return D_IM_RDMA_SEM_NG;
	}

	return D_IM_RDMA_OK;
}

/**
 * @brief		RDMA initialize
 * @param[in]   None
 * @param[out]	None
 * @return		None
 * @note		None
 */
INT32 Im_RDMA_Init( VOID )
{
	// check macro busy
	if( IO_RDMA.RDMA_CORE.RDMATRG.bit.RDMATRG != D_IM_RDMA_TRG_STOPPED ){
		Ddim_Print(("I:Im_RDMA_Init. macro not stopped error. \n"));
		return D_IM_RDMA_MACRO_BUSY_NG;
	}

	// clock ON
	im_rdma_on_clk();
	im_rdma_on_pclk();
	Im_RDMA_Dsb();

	// Software reset assert
	IO_RDMA.RDMA_TOP.SR.bit.SR = 1;

#ifdef CO_IM_RDMA_DEBUG
	Ddim_Print(("SR Assert: IO_RDMA.RDMA_TOP.SR.bit,SR =%d\n", IO_RDMA.RDMA_TOP.SR.bit.SR));
#endif

	// wait?
	

	// Software reset release
	IO_RDMA.RDMA_TOP.SR.bit.SR = 0;
	Im_RDMA_Dsb();

#ifdef CO_IM_RDMA_DEBUG
	Ddim_Print(("SR Release: IO_RDMA.RDMA_TOP.SR.bit,SR =%d\n", IO_RDMA.RDMA_TOP.SR.bit.SR));
#endif

	// set do not change register
	// PRch0
	IO_RDMA.PRCH0.PRCHCTL.bit.PRBMD		= 0;	// 0 fixed
	IO_RDMA.PRCH0.PRCHRS.bit.PRS		= 0;	// 0 fixed
	IO_RDMA.PRCH0.PRLSIZE.bit.PRLSIZE	= 0;	// 0 fixed
	IO_RDMA.PRCH0.PRVSIZE.bit.PRVSIZE	= 1;	// 1 fixed
	// PRch1
	IO_RDMA.PRCH1.PRCHCTL.bit.PRBMD		= 0;	// 0 fixed
	IO_RDMA.PRCH1.PRCHRS.bit.PRS		= 0;	// 0 fixed
	IO_RDMA.PRCH1.PRLSIZE.bit.PRLSIZE	= 0;	// 0 fixed
	IO_RDMA.PRCH1.PRVSIZE.bit.PRVSIZE	= 1;	// 1 fixed

	// clock OFF
	im_rdma_on_pclk();
	im_rdma_on_clk();
	Im_RDMA_Dsb();

	return D_IM_RDMA_OK;
}

/**
 * @brief 		RDMA driver control
 * @param[in]	*rdma_ctrl	: Control parameter
 * @return		D_IM_RDMA_OK, D_IM_RDMA_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_RDMA_Ctrl( T_IM_RDMA_CTRL* rdma_ctrl )
{
#ifdef CO_PARAM_CHECK
	if( rdma_ctrl == NULL ){
		// rdma_ctrl is Error because of NULL
		Ddim_Assertion(("I:Im_RDMA_Ctrl: Null check error. rdma_ctrl = NULL\n"));
		return D_IM_RDMA_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHfECK

	// clock ON
	im_rdma_on_pclk();
	Im_RDMA_Dsb();

	// set register
	// PRch0
	IO_RDMA.PRCH0.PRCHCTL.bit.PRLV		= rdma_ctrl->req_threshold;		// PRch request threshold
	IO_RDMA.PRCH0.PRHSIZE.bit.PRHSIZE	= rdma_ctrl->transfer_byte;		// RDMA : 4Byte Alignment
	IO_RDMA.PRCH0.PRSA.bit.PRSA			= rdma_ctrl->reg_addr_tbl_addr;	// RDMA : 4Byte Alignment
	// PRch1
	IO_RDMA.PRCH1.PRCHCTL.bit.PRLV		= rdma_ctrl->req_threshold;		// PRch request threshold
	IO_RDMA.PRCH1.PRHSIZE.bit.PRHSIZE	= rdma_ctrl->transfer_byte;		// RDMA : 4Byte Alignment
	IO_RDMA.PRCH1.PRSA.bit.PRSA			= rdma_ctrl->reg_data_top_addr;	// RDMA : 4Byte Alignment

	// set interrupt mode
	IO_RDMA.RDMA_TOP.INTCTL.bit.INTMD	= rdma_ctrl->int_mode;
	gIM_Rdma_Int_Mode					= rdma_ctrl->int_mode;
	// set interrupt
	IO_RDMA.RDMA_CORE.RDMAINTENB.bit.RDMAE	= 1;
	IO_RDMA.PRCH0.PRCHINTENB.bit.PRE		= 1;
	IO_RDMA.PRCH1.PRCHINTENB.bit.PRE		= 1;
	if( rdma_ctrl->int_mode == E_IM_RDMA_INT_MODE_AND_ERR ){
		IO_RDMA.RDMA_CORE.RDMAINTENB.bit.RDMAXE	= 1;
		IO_RDMA.PRCH0.PRCHINTENB.bit.PRXE		= 1;
		IO_RDMA.PRCH1.PRCHINTENB.bit.PRXE		= 1;
	}
	else{
		IO_RDMA.RDMA_CORE.RDMAINTENB.bit.RDMAXE	= 0;
		IO_RDMA.PRCH0.PRCHINTENB.bit.PRXE		= 0;
		IO_RDMA.PRCH1.PRCHINTENB.bit.PRXE		= 0;
	}

	// set callback
	gIM_Rdma_CallBack_Func				= rdma_ctrl->pCallBack;

	// clock OFF
	im_rdma_off_pclk();
	Im_RDMA_Dsb();

	return D_IM_RDMA_OK;
}

/**
 * @brief		The processing of RDMA begin (for sync)
 * @return		D_IM_RDMA_OK, D_IM_RDMA_NG, D_IM_RDMA_MACRO_BUSY_NG
 * @note		None
 */
INT32 Im_RDMA_Start_Sync( VOID )
{
	INT32 ercd = D_IM_RDMA_OK;

	// clock ON (pclk)
	im_rdma_on_pclk();
	Im_RDMA_Dsb();

	// Check RDMA running
	if( IO_RDMA.RDMA_CORE.RDMATRG.bit.RDMATRG != D_IM_RDMA_TRG_STOPPED ){
		im_rdma_off_pclk();
		Im_RDMA_Dsb();

		Ddim_Print(("I:Im_RDMA_Start_Sync: Macro busy error.\n"));
		return D_IM_RDMA_MACRO_BUSY_NG;
	}

	// clear interrupt
	DDIM_User_Clr_Flg( FID_IM_RDMA, ~D_IM_RDMA_WAIT_FLGPTN_ALL );

	// clock ON
	im_rdma_on_clk();
	Im_RDMA_Dsb();

	// start PRch(0/1)
	IO_RDMA.PRCH0.PRCHTRG.bit.PRCHTRG = D_IM_RDMA_PRCH_TRG_START;
	IO_RDMA.PRCH1.PRCHTRG.bit.PRCHTRG = D_IM_RDMA_PRCH_TRG_START;
	// start RDMA
	IO_RDMA.RDMA_CORE.RDMATRG.bit.RDMATRG = D_IM_RDMA_TRG_START;

	// Assert VD signal
	IO_RDMA.RDMA_TOP.VDGEN0.bit.VDGEN0	= 1;
	IO_RDMA.RDMA_TOP.VDGEN1.bit.VDGEN1	= 1;

	// wait RDMA process
	ercd = Im_RDMA_Wait_End();
	if( ercd != D_IM_RDMA_OK ){
		Ddim_Print(("I:Im_RDMA_Start_Sync: Wait_End error.\n"));
	}

	// stop PRch(0/1)
	IO_RDMA.PRCH0.PRCHTRG.bit.PRCHTRG = D_IM_RDMA_PRCH_TRG_STOP_FORCE;
	IO_RDMA.PRCH1.PRCHTRG.bit.PRCHTRG = D_IM_RDMA_PRCH_TRG_STOP_FORCE;
	// stop RDMA
	IO_RDMA.RDMA_CORE.RDMATRG.bit.RDMATRG = D_IM_RDMA_TRG_STOP;

	// clock OFF
	im_rdma_off_pclk();
	im_rdma_off_clk();
	Im_RDMA_Dsb();

	return ercd;
}

/**
 * @brief		The processing of RDMA begins(for async)
 * @return		D_IM_RDMA_OK, D_IM_RDMA_MACRO_BUSY_NG
 * @note		None
 */
INT32 Im_RDMA_Start_Async( VOID )
{
	// clock ON (pclk)
	im_rdma_on_pclk();
	Im_RDMA_Dsb();

	// Check RDMA running
	if( IO_RDMA.RDMA_CORE.RDMATRG.bit.RDMATRG != D_IM_RDMA_TRG_STOPPED ){
		im_rdma_off_pclk();
		Im_RDMA_Dsb();

		Ddim_Print(("I:Im_RDMA_Start_Sync: Macro busy error.\n"));
		return D_IM_RDMA_MACRO_BUSY_NG;
	}

	// clear interrupt
	DDIM_User_Clr_Flg( FID_IM_RDMA, ~D_IM_RDMA_WAIT_FLGPTN_ALL );

	// clock ON
	im_rdma_on_clk();
	Im_RDMA_Dsb();

	// start PRch(0/1)
	IO_RDMA.PRCH0.PRCHTRG.bit.PRCHTRG = D_IM_RDMA_PRCH_TRG_START;
	IO_RDMA.PRCH1.PRCHTRG.bit.PRCHTRG = D_IM_RDMA_PRCH_TRG_START;
	// start RDMA
	IO_RDMA.RDMA_CORE.RDMATRG.bit.RDMATRG = D_IM_RDMA_TRG_START;

	// Assert VD signal
	IO_RDMA.RDMA_TOP.VDGEN0.bit.VDGEN0	= 1;
	IO_RDMA.RDMA_TOP.VDGEN1.bit.VDGEN1	= 1;

	Im_RDMA_Dsb();

	return D_IM_RDMA_OK;
}

/**
 * @brief       Wait that the asynchronization processing ends
 * @return		D_IM_RDMA_OK, D_IM_RDMA_NG, D_IM_RDMA_SEM_TIMEOUT
 * @note        When the executed asynchronization processing ends, it is a return.
 */
INT32 Im_RDMA_Wait_End( VOID )
{
	DDIM_USER_FLGPTN	flg_ptn;
	DDIM_USER_FLGPTN	waiptn;
	DDIM_USER_ER		ercd;
	INT32				ret = D_IM_RDMA_OK;

	if( gIM_Rdma_Int_Mode == E_IM_RDMA_INT_MODE_OR ){
		waiptn = D_IM_RDMA_WAIT_FLGPTN_OR;
	}
	else if( gIM_Rdma_Int_Mode == E_IM_RDMA_INT_MODE_AND ){
		waiptn = D_IM_RDMA_WAIT_FLGPTN_AND;
	}
	else{
		waiptn = D_IM_RDMA_WAIT_FLGPTN_AND_ERR;
	}

#ifdef CO_DEBUG_ON_PC
	// normal end
	Im_RDMA_Int_Handler();
#endif	// CO_DEBUG_ON_PC

	// Wait
	ercd = DDIM_User_Twai_Flg( FID_IM_RDMA, waiptn, D_DDIM_USER_TWF_ORW, &flg_ptn, D_DDIM_WAIT_END_TIME );
	if( ercd == D_DDIM_USER_E_TMOUT ) {
		// A semaphore acquisition processing time out
		Ddim_Print(("I:Im_RDMA_Wait_End: time out. ercd = 0x%x\n", ercd));
		return D_IM_RDMA_SEM_TIMEOUT;
	}
	if( ercd != D_DDIM_USER_E_OK ){
		Ddim_Print(("I:Im_RDMA_Wait_End: DDIM_User_Twai_Flg error. ercd = 0x%x\n", ercd));
		ret = D_IM_RDMA_SEM_NG;
	}

	// stop PRch(0/1)
	IO_RDMA.PRCH0.PRCHTRG.bit.PRCHTRG = D_IM_RDMA_PRCH_TRG_STOP_FORCE;
	IO_RDMA.PRCH1.PRCHTRG.bit.PRCHTRG = D_IM_RDMA_PRCH_TRG_STOP_FORCE;
	// stop RDMA
	IO_RDMA.RDMA_CORE.RDMATRG.bit.RDMATRG = D_IM_RDMA_TRG_STOP;
#ifdef CO_DEBUG_ON_PC
	IO_RDMA.PRCH0.PRCHTRG.bit.PRCHTRG = D_IM_RDMA_PRCH_TRG_STOPPED;
	IO_RDMA.PRCH1.PRCHTRG.bit.PRCHTRG = D_IM_RDMA_PRCH_TRG_STOPPED;
	IO_RDMA.RDMA_CORE.RDMATRG.bit.RDMATRG = D_IM_RDMA_TRG_STOPPED;
#endif	// CO_DEBUG_ON_PC

	return ret;
}

/**
 * @brief		It is an interrupt handler that starts when the processing of RDMA ends
 * @note		None
 */
VOID Im_RDMA_Int_Handler( VOID )
{
	im_rdma_on_pclk();		// PCLK on
	Im_RDMA_Dsb();

	if( gIM_Rdma_Int_Mode == E_IM_RDMA_INT_MODE_OR ){
		union io_rmda_top_intmon1 intmon_1;

		// get status
		intmon_1.word = IO_RDMA.RDMA_TOP.INTMON1.word;

		// Clear interrupt
		IO_RDMA.RDMA_CORE.RDMAINTFLG.bit.RDMAF	= 1;
		IO_RDMA.PRCH0.PRCHINTFLG.bit.__PRF		= 1;
		IO_RDMA.PRCH1.PRCHINTFLG.bit.__PRF		= 1;

		// Set End Flag
		(VOID)DDIM_User_Set_Flg( FID_IM_RDMA, D_IM_RDMA_WAIT_FLGPTN_OR );

		// User Function Check
		if( gIM_Rdma_CallBack_Func != NULL ) {
			gIM_Rdma_CallBack_Func( (ULONG)intmon_1.word, 0 );
		}

#ifdef CO_DEBUG_ON_PC
		Ddim_Print(("I:Im_RDMA_Int_Handler. E_IM_RDMA_INT_MODE_OR: end:0x%08lx\n",
						(ULONG)intmon_1.word));
#endif	// CO_DEBUG_ON_PC
	}
	else if( gIM_Rdma_Int_Mode == E_IM_RDMA_INT_MODE_AND ){
#ifdef CO_DEBUG_ON_PC
		union io_rmda_top_intmon1 intmon_1;

		// get status
		intmon_1.word = IO_RDMA.RDMA_TOP.INTMON1.word;
#endif	// CO_DEBUG_ON_PC

		// Clear interrupt
		IO_RDMA.RDMA_CORE.RDMAINTFLG.bit.RDMAF	= 1;
		IO_RDMA.PRCH0.PRCHINTFLG.bit.__PRF		= 1;
		IO_RDMA.PRCH1.PRCHINTFLG.bit.__PRF		= 1;

		// Set End Flag
		(VOID)DDIM_User_Set_Flg( FID_IM_RDMA, D_IM_RDMA_WAIT_FLGPTN_AND );

		// User Function Check
		if( gIM_Rdma_CallBack_Func != NULL ) {
			gIM_Rdma_CallBack_Func( D_IM_RDMA_FLG_WAIT_END_ALL, 0 );
		}

#ifdef CO_DEBUG_ON_PC
		Ddim_Print(("I:Im_RDMA_Int_Handler. E_IM_RDMA_INT_MODE_AND: end:0x%08lx\n",
						(ULONG)intmon_1.word));
#endif	// CO_DEBUG_ON_PC
	}
	else{	// gIM_Rdma_Int_Mode == E_IM_RDMA_INT_MODE_AND_ERR
		union io_rmda_top_intmon1 intmon_1;
		union io_rmda_top_intmon2 intmon_2;

		// get error status
		intmon_1.word = IO_RDMA.RDMA_TOP.INTMON1.word;
		intmon_2.word = IO_RDMA.RDMA_TOP.INTMON2.word;

		// Clear interrupt
		IO_RDMA.RDMA_CORE.RDMAINTFLG.bit.RDMAF	= 1;
		IO_RDMA.PRCH0.PRCHINTFLG.bit.__PRF		= 1;
		IO_RDMA.PRCH1.PRCHINTFLG.bit.__PRF		= 1;
		IO_RDMA.RDMA_CORE.RDMAINTFLG.bit.RDMAXF	= 1;
		IO_RDMA.PRCH0.PRCHINTFLG.bit.__PRXF		= 1;
		IO_RDMA.PRCH1.PRCHINTFLG.bit.__PRXF		= 1;

		// Set End Flag
		(VOID)DDIM_User_Set_Flg( FID_IM_RDMA, D_IM_RDMA_WAIT_FLGPTN_AND_ERR );

		// User Function Check
		if( gIM_Rdma_CallBack_Func != NULL ) {
			gIM_Rdma_CallBack_Func( (ULONG)intmon_1.word, (ULONG)intmon_2.word );
		}

#ifdef CO_DEBUG_ON_PC
		Ddim_Print(("I:Im_RDMA_Int_Handler. E_IM_RDMA_INT_MODE_AND_ERR: end:0x%08lx, error:0x%08lx\n",
						(ULONG)intmon_1.word, (ULONG)intmon_2.word));
#endif	// CO_DEBUG_ON_PC
	}

	im_rdma_off_pclk();		// PCLK off
	Im_RDMA_Dsb();
}


/**
 * @brief		Get Im_RDMA_Set_Ctrl param
 * @param[out]	*rdma_ctrl	: Control parameter
 * @return		D_IM_RDMA_OK, D_IM_RDMA_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_RDMA_Get_Ctrl( T_IM_RDMA_CTRL* rdma_ctrl )
{
#ifdef CO_PARAM_CHECK
	if( rdma_ctrl == NULL ){
		// rdma_ctrl is Error because of NULL
		Ddim_Assertion(("I:Im_RDMA_Get_Ctrl: Null check error. rdma_ctrl = NULL\n"));
		return D_IM_RDMA_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	im_rdma_on_pclk();		// PCLK on
	Im_RDMA_Dsb();

	// get register
	// common
	rdma_ctrl->req_threshold		= IO_RDMA.PRCH0.PRCHCTL.bit.PRLV;
	rdma_ctrl->transfer_byte		= IO_RDMA.PRCH0.PRHSIZE.bit.PRHSIZE;
	// PRch0
	rdma_ctrl->reg_addr_tbl_addr	= IO_RDMA.PRCH0.PRSA.bit.PRSA;
	// PRch1
	rdma_ctrl->reg_data_top_addr	= IO_RDMA.PRCH0.PRSA.bit.PRSA;
	// Callback
	rdma_ctrl->pCallBack			= gIM_Rdma_CallBack_Func;

	im_rdma_off_pclk();		// PCLK off
	Im_RDMA_Dsb();

	return D_IM_RDMA_OK;
}

/**
 * @brief		Get RDMA interrupt mode
 * @return		E_IM_RDMA_INT_MODE
 * @note		None
 */
E_IM_RDMA_INT_MODE Im_RDMA_Get_Int_Mode( VOID )
{
	return gIM_Rdma_Int_Mode;
}

/**
 * @brief		Get PRch setting error
 * @return		TRUE:PRch setting error, FALSE:not PRch error
 * @note		None
 */
BOOL Im_RDMA_Get_PRch_Error_Status( VOID )
{
	BOOL error_flag;

	im_rdma_on_pclk();		// PCLK on
	Im_RDMA_Dsb();

	error_flag = IO_RDMA.RDMA_CORE.RDMAPRCHERR.bit.RDMAPRCHERR;

	im_rdma_off_pclk();		// PCLK off
	Im_RDMA_Dsb();

	return error_flag;
}

/**
 * @brief		Set RDMA Control AXI bus I/F Control
 * @param[in]	*axi_ctrl	: Control parameter
 * @return		D_IM_RDMA_OK, D_IM_RDMA_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_RDMA_Set_Axi( const T_IM_RDMA_AXI* const axi_ctrl )
{
#ifdef CO_PARAM_CHECK
	if( axi_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_RDMA_Ctrl_Axi. Parameter is NULL.\n"));
		return D_IM_RDMA_INPUT_PARAM_ERR;
	}
#endif

	im_rdma_on_pclk();
	Im_RDMA_Dsb();

	IO_RDMA.RDMA_CORE.RDMAAXCTL.bit.AWPROT	= axi_ctrl->write_protection_type;
	IO_RDMA.RDMA_CORE.RDMAAXCTL.bit.AWCACHE	= axi_ctrl->write_cache_type;

	im_rdma_off_pclk();
	Im_RDMA_Dsb();

	return D_IM_RDMA_OK;
}

/**
 * @brief		Get AXI bus I/F Status
 * @param[out]	*sts	: AXI status
 * @return		D_IM_RDMA_OK, D_IM_RDMA_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_RDMA_Get_Axi_Status( T_IM_RDMA_AXI_STATUS* const sts )
{
#ifdef CO_PARAM_CHECK
	if( sts == NULL ) {
		// sts setting error
		Ddim_Assertion(("I:Im_RDMA_Get_Axi_Status. Parameter is NULL.\n"));
		return D_IM_RDMA_INPUT_PARAM_ERR;
	}
#endif

	im_rdma_on_pclk();
	Im_RDMA_Dsb();

	sts->write_channel_response = IO_RDMA.RDMA_CORE.RDMABRESP.bit.RDMABRESP;

	im_rdma_off_pclk();
	Im_RDMA_Dsb();

	return D_IM_RDMA_OK;
}

#ifdef D_IM_RDMA_PRINT_ST
VOID Im_RDMA_Print_ClockStatus( VOID )
{
#ifdef CO_ACT_RDMA_PCLOCK
	Ddim_Print(( "CLKSTOP: AP=%u\n", Dd_Top_Get_CLKSTOP14_RDMAAP() ));
#endif
#ifdef CO_ACT_RDMA_ICLOCK
	Ddim_Print(( "CLKSTOP: AX=%u\n", Dd_Top_Get_CLKSTOP14_RDMAAX() ));
#endif
#ifdef CO_ACT_RDMA_CLOCK
	Ddim_Print(( "CLKSTOP: CK=%u\n", Dd_Top_Get_CLKSTOP14_RDMACK() ));
#endif

#ifdef CO_ACT_RDMA_PCLOCK
	Ddim_Print(( "CLKCNT: AP=%u\n", gIM_Rdma_Pclk_Ctrl_Cnt ));
#endif
#ifdef CO_ACT_RDMA_ICLOCK
	Ddim_Print(( "CLKCNT: AX=%u\n", gIM_Rdma_Iclk_Ctrl_Cnt ));
#endif
#ifdef CO_ACT_RDMA_CLOCK
	Ddim_Print(( "CLKCNT: CK=%u\n", gIM_Rdma_Clk_Ctrl_Cnt ));
#endif

}
#endif // D_IM_RDMA_PRINT_ST


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/**
 * @brief 		RDMA driver control/start sync
 * @param[in]	tmout	: Time-out period
 * @param[in]	*rdma_ctrl	: Control parameter
 * @return		D_IM_RDMA_OK, D_IM_RDMA_INPUT_PARAM_ERR
 * @note		None
 */
INT32 Im_RDMA_Ctrl_Quick_Start_Sync( INT32 tmout, T_IM_RDMA_CTRL* rdma_ctrl )
{
	INT32 ercd = D_IM_RDMA_OK;

#ifdef CO_PARAM_CHECK
	if( tmout < D_DDIM_USER_SEM_WAIT_FEVR ){
		Ddim_Assertion(("I:Im_RDMA_Ctrl_Quick_Start_Sync: input param error. tmout = 0x%x\n", tmout));
		return D_IM_RDMA_INPUT_PARAM_ERR;
	}
	if( rdma_ctrl == NULL ){
		// rdma_ctrl is Error because of NULL
		Ddim_Assertion(("I:Im_RDMA_Ctrl_Quick_Start_Sync: Null check error. rdma_ctrl = NULL\n"));
		return D_IM_RDMA_INPUT_PARAM_ERR;
	}
#endif // CO_PARAM_CHECK

	// Open
	ercd = Im_RDMA_Open( tmout );
	if( ercd != D_IM_RDMA_OK ){
		Ddim_Print(("I:Im_RDMA_Ctrl_Quick_Start_Sync: Im_RDMA_Open error.\n"));
		return ercd;
	}

	// clock ON
	im_rdma_on_pclk();
	Im_RDMA_Dsb();

	// set register
	// set do not change register
	// PRch0
	IO_RDMA.PRCH0.PRCHCTL.bit.PRBMD		= 0;	// 0 fixed
	IO_RDMA.PRCH0.PRCHRS.bit.PRS		= 0;	// 0 fixed
	IO_RDMA.PRCH0.PRLSIZE.bit.PRLSIZE	= 0;	// 0 fixed
	IO_RDMA.PRCH0.PRVSIZE.bit.PRVSIZE	= 1;	// 1 fixed
	// PRch1
	IO_RDMA.PRCH1.PRCHCTL.bit.PRBMD		= 0;	// 0 fixed
	IO_RDMA.PRCH1.PRCHRS.bit.PRS		= 0;	// 0 fixed
	IO_RDMA.PRCH1.PRLSIZE.bit.PRLSIZE	= 0;	// 0 fixed
	IO_RDMA.PRCH1.PRVSIZE.bit.PRVSIZE	= 1;	// 1 fixed

	// set interrupt mode
	IO_RDMA.RDMA_TOP.INTCTL.bit.INTMD	= rdma_ctrl->int_mode;
	gIM_Rdma_Int_Mode					= rdma_ctrl->int_mode;
	// set interrupt
	IO_RDMA.RDMA_CORE.RDMAINTENB.bit.RDMAE	= 1;
	IO_RDMA.PRCH0.PRCHINTENB.bit.PRE		= 1;
	IO_RDMA.PRCH1.PRCHINTENB.bit.PRE		= 1;
	if( rdma_ctrl->int_mode == E_IM_RDMA_INT_MODE_AND_ERR ){
		IO_RDMA.RDMA_CORE.RDMAINTENB.bit.RDMAXE	= 1;
		IO_RDMA.PRCH0.PRCHINTENB.bit.PRXE		= 1;
		IO_RDMA.PRCH1.PRCHINTENB.bit.PRXE		= 1;
	}
	else{
		IO_RDMA.RDMA_CORE.RDMAINTENB.bit.RDMAXE	= 0;
		IO_RDMA.PRCH0.PRCHINTENB.bit.PRXE		= 0;
		IO_RDMA.PRCH1.PRCHINTENB.bit.PRXE		= 0;
	}

	// Set parameter
	// PRch0
	IO_RDMA.PRCH0.PRCHCTL.bit.PRLV		= rdma_ctrl->req_threshold;		// PRch request threshold
	IO_RDMA.PRCH0.PRHSIZE.bit.PRHSIZE	= rdma_ctrl->transfer_byte;		// RDMA : 4Byte Alignment
	IO_RDMA.PRCH0.PRSA.bit.PRSA			= rdma_ctrl->reg_addr_tbl_addr;	// RDMA : 4Byte Alignment
	// PRch1
	IO_RDMA.PRCH1.PRCHCTL.bit.PRLV		= rdma_ctrl->req_threshold;		// PRch request threshold
	IO_RDMA.PRCH1.PRHSIZE.bit.PRHSIZE	= rdma_ctrl->transfer_byte;		// RDMA : 4Byte Alignment
	IO_RDMA.PRCH1.PRSA.bit.PRSA			= rdma_ctrl->reg_data_top_addr;	// RDMA : 4Byte Alignment

	// set callback
	gIM_Rdma_CallBack_Func				= rdma_ctrl->pCallBack;

	// clear interrupt
	DDIM_User_Clr_Flg( FID_IM_RDMA, ~D_IM_RDMA_WAIT_FLGPTN_ALL );

	// clock ON
	im_rdma_on_clk();
	Im_RDMA_Dsb();

	// enable VD signal
	IO_RDMA.RDMA_TOP.VDGEN0.bit.VDGEN0	= 1;
	IO_RDMA.RDMA_TOP.VDGEN1.bit.VDGEN1	= 1;

	// start PRch(0/1)
	IO_RDMA.PRCH0.PRCHTRG.bit.PRCHTRG = D_IM_RDMA_TRG_START;
	IO_RDMA.PRCH1.PRCHTRG.bit.PRCHTRG = D_IM_RDMA_TRG_START;
	// start RDMA
	IO_RDMA.RDMA_CORE.RDMATRG.bit.RDMATRG = D_IM_RDMA_TRG_START;

	// wait RDMA process
	ercd = Im_RDMA_Wait_End();
	if( ercd != D_IM_RDMA_OK ){
		Ddim_Print(("I:Im_RDMA_Ctrl_Quick_Start_Sync: Wait_End error.\n"));
	}

	// clock OFF
	im_rdma_off_pclk();
	im_rdma_off_clk();
	Im_RDMA_Dsb();

	// Close
	ercd = Im_RDMA_Close();
	if( ercd != D_IM_RDMA_OK ){
		Ddim_Print(("I:Im_RDMA_Ctrl_Quick_Start_Sync: Im_RDMA_Close error.\n"));
	}

	return ercd;
}

//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE

