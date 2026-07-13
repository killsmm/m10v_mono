/**
 * @file		im_me.c
 * @brief		JDSME macro wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include "jdsme.h"
#include "im_me.h"
#include "dd_top.h"
#include "ddim_user_custom.h"
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define CO_ME_REG_TYPE_CHECK
//#define CO_ME_DEBUG_PRINT		// Debug.

// Data Synchronization Barrier.
#define im_me_dsb()						Dd_ARM_Dsb_Pou()

// Wait usec.
#define im_me_wait_usec( usec )			Dd_ARM_Wait_ns( ( usec * 1000 ) )

// The structure type matching check.
#ifdef CO_ME_REG_TYPE_CHECK
#define im_me_check_reg_type( dst, src )	( dst ) = ( src );
#else
#define im_me_check_reg_type( dst, src )
#endif

// Set signed register.
#define im_me_set_reg_signed( reg, type, member, val )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_me_check_reg_type( work, (reg) ); \
		}

// Set signed register (multi array).
#define im_me_set_reg_signed_a( reg, type, member, val )	\
		{ \
			type work = { .word[0] = 0xFFFFFFFFUL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_me_check_reg_type( work, (reg) ); \
		}

// Get signed register.
#define im_me_get_reg_signed( val, reg, type, member )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = -1L; \
			if( ((work.bit.member) == 0xFFFFFFFFUL) || ((reg.bit.member) <= ((work.bit.member) /2)) ) { \
				(val) = reg.bit.member; \
			} \
			else { \
				(val) = -(work.bit.member - reg.bit.member +1); \
			} \
			im_me_check_reg_type( (reg), work );	/* The structure type matching check. */ \
		}

// Get signed register (multi array).
#define im_me_get_reg_signed_a( val, reg, type, member )	\
		{ \
			type work = { .word[0] = 0UL }; \
			work.bit.member = -1L; \
			if( ((work.bit.member) == 0xFFFFFFFFUL) || ((reg.bit.member) <= ((work.bit.member) /2)) ) { \
				(val) = reg.bit.member; \
			} \
			else { \
				(val) = -(work.bit.member - reg.bit.member +1); \
			} \
			im_me_check_reg_type( (reg), work );	/* The structure type matching check. */ \
		}

#ifdef CO_PARAM_CHECK
#define im_me_check_wait_time( wait_time )	(( wait_time >= D_DDIM_USER_SEM_WAIT_FEVR ) ? TRUE : FALSE)
#endif	// CO_PARAM_CHECK

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_IMAGE_CLOCK)
#define	CO_ACT_ME_CLOCK
#endif

#if defined(CO_ACT_ICLOCK) || defined(CO_ACT_IMAGE_ICLOCK)
#define	CO_ACT_ME_ICLOCK
#endif

#if defined(CO_ACT_PCLOCK) || defined(CO_ACT_IMAGE_PCLOCK)
#define	CO_ACT_ME_PCLOCK
#endif

// Software reset.
#define	D_IM_ME_SR_RELEASE			(0)				// Software reset release.
#define	D_IM_ME_SR_RESET			(1)				// Software reset state.

// ME1 state.
#define	D_IM_ME_ME1_STATE_STOP		(0)				// Stop  state.
#define	D_IM_ME_ME1_STATE_START		(2)				// Start state.
#define	D_IM_ME_ME1_STATE_PAUSE		(4)				// Pause state.

// ME1 data prefetch state.
#define	D_IM_ME_STR_STOP			(0)				// Stop  state.
#define	D_IM_ME_STR_START			(1)				// Start state.

// Wait time.
#define D_IM_ME_WAIT_END_TIME		(100)			// ME wait end time.

// Interrupt flag.
#define D_IM_ME_FLG_END				(0x00000001)	// End.

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special.

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special.

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#ifdef CO_ACT_ME_CLOCK
static volatile UCHAR				gIM_ME_CLK_Counter		= 0;
#endif

#ifdef CO_ACT_ME_ICLOCK
static volatile UCHAR				gIM_ME_XCLK_Counter		= 0;
#endif

#ifdef CO_ACT_ME_PCLOCK
static volatile UCHAR				gIM_ME_PCLK_Counter		= 0;
#endif

static volatile ULONG				gIM_ME_Axi_Err			= D_IM_ME_AXI_NORMAL_END;

static volatile T_IM_ME_CALLBACK	gIM_ME_CallBack_Func	= NULL;

static volatile T_IM_ME_CTRL		gIM_ME_Ctrl;

static volatile UCHAR				gIM_ME_Int_Cnt			= 0;

static volatile ULONG				gIM_ME_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
// Mutex.
#define im_me_lock()		Dd_ARM_Critical_Section_Start( gIM_ME_Spin_Lock )
#define im_me_unlock()		Dd_ARM_Critical_Section_End( gIM_ME_Spin_Lock )

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

/**
CLK ON for ME.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_me_on_clk( VOID )
{
#ifdef CO_ACT_ME_CLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_ME_CLK_Counter, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_MECK_BIT );
#endif
}

/**
CLK OFF for ME.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_me_off_clk( VOID )
{
#ifdef CO_ACT_ME_CLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_ME_CLK_Counter, &Dd_Top_Get_CLKSTOP7(), D_DD_TOP_MECK_BIT );
#endif
}

/**
XCLK ON for ME.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_me_on_xclk( VOID )
{
#ifdef CO_ACT_ME_ICLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_ME_XCLK_Counter, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_MEAX_BIT );
#endif
}

/**
XCLK OFF for ME.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_me_off_xclk( VOID )
{
#ifdef CO_ACT_ME_ICLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_ME_XCLK_Counter, &Dd_Top_Get_CLKSTOP7(), D_DD_TOP_MEAX_BIT );
#endif
}

/**
PCLK ON for ME.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_me_on_pclk( VOID )
{
#ifdef CO_ACT_ME_PCLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_ME_PCLK_Counter, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_MEAP_BIT );
#endif
}

/**
PCLK OFF for ME.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_me_off_pclk( VOID )
{
#ifdef CO_ACT_ME_PCLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_ME_PCLK_Counter, &Dd_Top_Get_CLKSTOP7(), D_DD_TOP_MEAP_BIT );
#endif
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
ME initialize.<br>
@retval			D_DDIM_OK					Success.
*/
INT32 Im_ME_Init( VOID )
{
	im_me_on_pclk();
	im_me_on_xclk();
	im_me_on_clk();
	im_me_dsb();

	gIM_ME_Int_Cnt = 0;

	memset( (VOID*)&gIM_ME_Ctrl, 0, sizeof(T_IM_ME_CTRL) );

	// Software reset (APBBRG).
	IO_ME.SRESET.bit.CMDR	= D_IM_ME_SR_RESET;

	// Software reset release.
	IO_ME.SRESET.bit.CMDR	= D_IM_ME_SR_RELEASE;

	im_me_dsb();

	for ( int i = 0; i < E_IM_ME_PPNUM_MAX; i++ ) {

		// Software reset (PTCONT/PABT).
		IO_ME.PME[ i ].CR.bit.CMDR				= D_IM_ME_SR_RESET;

		// Software reset release.
		IO_ME.PME[ i ].CR.bit.CMDR				= D_IM_ME_SR_RELEASE;

		im_me_dsb();

		// Clock enable.
		IO_ME.PME[ i ].PTCC.bit.CKA				= D_IM_ME_ENABLE;		// PTCONT (Clock on).
		IO_ME.PME[ i ].PACC.bit.PABT_CKA		= D_IM_ME_ENABLE;		// PABT   (Clock on).
		IO_ME.PME[ i ].PACC.bit.CKM				= D_IM_ME_DISABLE;		// PABT   (Normal mode).

		// Interrupt enable.
		IO_ME.PME[ i ].IRQE.bit.IRQE0			= D_IM_ME_DISABLE;
		IO_ME.PME[ i ].IRQE.bit.IRQE1			= D_IM_ME_DISABLE;
		IO_ME.PME[ i ].IRQE.bit.IRQE2			= D_IM_ME_DISABLE;
		IO_ME.PME[ i ].IRQE.bit.IRQE3			= D_IM_ME_DISABLE;
		IO_ME.PME[ i ].IRQE.bit.IRQE4			= D_IM_ME_DISABLE;
		IO_ME.PME[ i ].IRQE.bit.IRQE5			= D_IM_ME_DISABLE;
		IO_ME.PME[ i ].IRQE.bit.IRQE6			= D_IM_ME_ENABLE;
		IO_ME.PME[ i ].IRQE.bit.IRQE7			= D_IM_ME_DISABLE;
		IO_ME.PME[ i ].IRQE.bit.IRQE8			= D_IM_ME_DISABLE;

		// Software reset (PME/SRST).
		IO_ME.PME[ i ].SRST.bit.SRST			= D_IM_ME_SR_RESET;

		// Software reset release.
		IO_ME.PME[ i ].SRST.bit.SRST			= D_IM_ME_SR_RELEASE;

		// Software reset (PME/CMDRST).
		IO_ME.PME[ i ].PMBUF_CTL.bit.CMDRST		= D_IM_ME_SR_RESET;

		// Software reset release.
		IO_ME.PME[ i ].PMBUF_CTL.bit.CMDRST		= D_IM_ME_SR_RELEASE;

		// PMBUF through.
		IO_ME.PME[ i ].PMBUF_CTL.bit.THRU		= D_IM_ME_OFF;

		im_me_dsb();

		// Clock enable.
		IO_ME.PME[ i ].ACTIVE.bit.CLKACT		= D_IM_ME_ENABLE;		// PME (Clock on).
		IO_ME.PME[ i ].ACTIVE.bit.CKEN_MODE0	= D_IM_ME_DISABLE;		// M6CLK_PM0 (Normal mode).
		IO_ME.PME[ i ].ACTIVE.bit.CKEN_MODE1	= D_IM_ME_DISABLE;		// M6CLK_PM0 (Normal mode).

		im_me_dsb();
	}

	// Software reset (RMCIF).
	IO_ME.CMD_RST.bit.CMD_RST		= D_IM_ME_SR_RESET;

	// Software reset release.
	IO_ME.CMD_RST.bit.CMD_RST		= D_IM_ME_SR_RELEASE;

	im_me_dsb();

	// Clock enable.
	IO_ME.CLKENB1.bit.M6_PFM_EN		= D_IM_ME_DISABLE;					// M6CLK_PFM (Normal mode).
	IO_ME.CLKENB4.bit.M6_P4M_EN		= D_IM_ME_DISABLE;					// M6CLK_P4M (Normal mode).
	IO_ME.CLKACT1.bit.M6_PFM_ACT	= D_IM_ME_ENABLE;					// M6CLK_PFM (Clock on : hard autonomous control).
	IO_ME.CLKACT4.bit.M6_P4M_ACT	= D_IM_ME_ENABLE;					// M6CLK_P4M (Clock on : hard autonomous control).

	im_me_off_clk();
	im_me_off_xclk();
	im_me_off_pclk();
	im_me_dsb();

	return D_DDIM_OK;
}

/**
The semaphore of ME is acquired.<br>
@param[in]		wait_time					Time-out period.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_ME_INPUT_PARAM_ERROR	Parameter error.
@retval			D_IM_ME_SEM_NG				Semaphore acquisition NG.
*/
INT32 Im_ME_Open( const INT32 wait_time )
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if ( im_me_check_wait_time( wait_time ) == FALSE ) {
		Ddim_Assertion(("Im_ME_Open parameter error\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
#endif

	if ( D_DDIM_USER_SEM_WAIT_POL == wait_time ) {
		ercd = DDIM_User_Pol_Sem( SID_IM_ME );
	}
	else {
		ercd = DDIM_User_Twai_Sem( SID_IM_ME, (DDIM_USER_TMO)wait_time );
	}

	if ( D_DDIM_USER_E_OK == ercd ) {
		return D_DDIM_OK;
	}
	else {
		return D_IM_ME_SEM_NG;
	}
}

/**
The semaphore of ME is returned.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_ME_SEM_NG				Semaphore acquisition NG.
*/
INT32 Im_ME_Close( VOID )
{
	DDIM_USER_ER	ercd;

	ercd = DDIM_User_Sig_Sem( SID_IM_ME );

	if ( D_DDIM_USER_E_OK == ercd ) {
		return D_DDIM_OK;
	}
	else {
		return D_IM_ME_SEM_NG;
	}
}

/**
ME control parameters set.<br>
@param[in]		me_ctrl							ME control parameters. See @ref T_IM_ME_CTRL.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_ME_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_ME_Ctrl( const T_IM_ME_CTRL* const me_ctrl )
{
#ifdef CO_PARAM_CHECK
	if ( me_ctrl == NULL ) {
		Ddim_Assertion(("Im_ME_Ctrl() error. me_ctrl = NULL.\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
	if ( me_ctrl->apbbrg.int_mode > D_IM_ME_INTMODE_LOGICAL_OR ) {
		Ddim_Assertion(("Im_ME_Ctrl() error. int_mod is invalid.\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
	for ( int i = 0; i < E_IM_ME_PPNUM_MAX; i++ ) {
		if ( me_ctrl->apbbrg.pp_enable[ i ] > D_IM_ME_ENABLE ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. pp_enable[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].input_mode > D_IM_ME_INPUTMODE_PROGRESSIVE ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. input_mode[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].ref_txb > D_IM_ME_REFTXB_BOTTOM ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. ref_txb[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].out_limit > D_IM_ME_OUTLIMIT_TOP ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. out_limit[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( ( me_ctrl->pme[ i ].bank_num == 0 ) ||
			 ( me_ctrl->pme[ i ].bank_num > D_IM_ME_BANKNUM_4 ) ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. bank_num[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		for ( int j = 0; j < E_IM_ME_BANKNUM_MAX; j++ ) {
			if ( me_ctrl->pme[ i ].m1org_mode[ j ].direction > D_IM_ME_M1ORG_DIR_BWD ) {
				Ddim_Assertion(("Im_ME_Ctrl() error. m1org_mode[ %d ][ %d ].direction is invalid.\n", i, j));
				return D_IM_ME_INPUT_PARAM_ERROR;
			}
			if ( me_ctrl->pme[ i ].m1org_mode[ j ].parity > D_IM_ME_M1ORG_PARI_BOTTOM ) {
				Ddim_Assertion(("Im_ME_Ctrl() error. m1org_mode[ %d ][ %d ].parity is invalid.\n", i, j));
				return D_IM_ME_INPUT_PARAM_ERROR;
			}
			if ( me_ctrl->pme[ i ].m1org_mode[ j ].read_mode > D_IM_ME_M1ORG_READ_SDRAM ) {
				Ddim_Assertion(("Im_ME_Ctrl() error. m1org_mode[ %d ][ %d ].read_mode is invalid.\n", i, j));
				return D_IM_ME_INPUT_PARAM_ERROR;
			}
			if ( me_ctrl->pme[ i ].m1org_mode[ j ].sad0_mode > D_IM_ME_M1ORG_SAD_MIN1 ) {
				Ddim_Assertion(("Im_ME_Ctrl() error. m1org_mode[ %d ][ %d ].sad0_mode is invalid.\n", i, j));
				return D_IM_ME_INPUT_PARAM_ERROR;
			}
			if ( me_ctrl->pme[ i ].m1org_mode[ j ].sad1_mode > D_IM_ME_M1ORG_SAD_MIN1 ) {
				Ddim_Assertion(("Im_ME_Ctrl() error. m1org_mode[ %d ][ %d ].sad1_mode is invalid.\n", i, j));
				return D_IM_ME_INPUT_PARAM_ERROR;
			}
			if ( me_ctrl->pme[ i ].m1org_mode[ j ].vec_mode > D_IM_ME_M1ORG_VECM_WRITE1 ) {
				Ddim_Assertion(("Im_ME_Ctrl() error. m1org_mode[ %d ][ %d ].vec_mode is invalid.\n", i, j));
				return D_IM_ME_INPUT_PARAM_ERROR;
			}
			if ( ( me_ctrl->pme[ i ].m1org_mode[ j ].vec_select != D_IM_ME_M1ORG_VECS_SAVE  ) &&
				 ( me_ctrl->pme[ i ].m1org_mode[ j ].vec_select != D_IM_ME_M1ORG_VECS_COPY0 ) &&
				 ( me_ctrl->pme[ i ].m1org_mode[ j ].vec_select != D_IM_ME_M1ORG_VECS_COPY1 ) ) {
				Ddim_Assertion(("Im_ME_Ctrl() error. m1org_mode[ %d ][ %d ].vec_select is invalid.\n", i, j));
				return D_IM_ME_INPUT_PARAM_ERROR;
			}
			if ( me_ctrl->pme[ i ].m1org_mode[ j ].pic_type > D_IM_ME_M1ORG_PICT_I ) {
				Ddim_Assertion(("Im_ME_Ctrl() error. m1org_mode[ %d ][ %d ].pic_type is invalid.\n", i, j));
				return D_IM_ME_INPUT_PARAM_ERROR;
			}
		}
		if ( me_ctrl->pme[ i ].m1correct.front_ky > D_IM_ME_M1CRCT_KY_MAX ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. m1correct.front_ky[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].m1correct.back_ky > D_IM_ME_M1CRCT_KY_MAX ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. m1correct.back_ky[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].m1correct.front_kx > D_IM_ME_M1CRCT_KX_MAX ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. m1correct.front_kx[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].m1correct.back_kx > D_IM_ME_M1CRCT_KX_MAX ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. m1correct.back_kx[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].m1splt.mode > D_IM_ME_SPLITMODE_VERTICAL ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. m1splt.mode[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].m1skip > D_IM_ME_ON ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. m1skip[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].m1adjust.wait_onoff > D_IM_ME_ON ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. m1adjust.wait_onoff[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].m1pause.pause_onoff > D_IM_ME_ON ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. m1pause.pause_onoff[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].m1limit.v_plus > D_IM_ME_VEC_LIMITV_MAX ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. m1limit.v_plus[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].m1limit.v_minus > D_IM_ME_VEC_LIMITV_MAX ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. m1limit.v_minus[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].m1limit.h_plus > D_IM_ME_VEC_LIMITH_MAX ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. m1limit.h_plus[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].m1limit.h_minus > D_IM_ME_VEC_LIMITH_MAX ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. m1limit.h_minus[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_ctrl->pme[ i ].core_mode.line_interval > D_IM_ME_LINEINTV_3QMB ) {
			Ddim_Assertion(("Im_ME_Ctrl() error. core_mode.line_interval[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
	}
	if ( me_ctrl->rmcif.pme_num > ( E_IM_ME_PPNUM_MAX - 1 ) ) {
		Ddim_Assertion(("Im_ME_Ctrl() error. pme_num is invalid.\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
	if ( me_ctrl->rmcif.abt_type > D_IM_ME_ABT_ROUND ) {
		Ddim_Assertion(("Im_ME_Ctrl() error. abt_type is invalid.\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
#endif

	im_me_on_pclk();
	im_me_on_xclk();
	im_me_on_clk();
	im_me_dsb();

	// Wait ME Stop.
	while ( ( IO_ME.PME[ E_IM_ME_PPNUM_PPA ].STATE.bit.ME1_STATE != D_IM_ME_ME1_STATE_STOP ) ||
			( IO_ME.PME[ E_IM_ME_PPNUM_PPB ].STATE.bit.ME1_STATE != D_IM_ME_ME1_STATE_STOP ) ||
			( IO_ME.PME[ E_IM_ME_PPNUM_PPC ].STATE.bit.ME1_STATE != D_IM_ME_ME1_STATE_STOP ) ||
			( IO_ME.PF1_START.bit.STR != D_IM_ME_STR_STOP ) ) {

		DDIM_User_Dly_Tsk( 1 );

		Ddim_Print(("Im_ME_Ctrl() : ME Processing\n"));
	}

	// User callback function setting.
	gIM_ME_Axi_Err			= D_IM_ME_AXI_NORMAL_END;
	gIM_ME_CallBack_Func	= me_ctrl->pCallBack;

	// Parameter saved.
	gIM_ME_Ctrl = *me_ctrl;

	// Interrupt enable.
	IO_ME.INTMODE.bit.PPAVALID	= me_ctrl->apbbrg.pp_enable[ E_IM_ME_PPNUM_PPA ];	// PPA.
	IO_ME.INTMODE.bit.PPBVALID	= me_ctrl->apbbrg.pp_enable[ E_IM_ME_PPNUM_PPB ];	// PPB.
	IO_ME.INTMODE.bit.PPCVALID	= me_ctrl->apbbrg.pp_enable[ E_IM_ME_PPNUM_PPC ];	// PPC.
	IO_ME.INTSTATE.bit.PPAINT	= me_ctrl->apbbrg.pp_enable[ E_IM_ME_PPNUM_PPA ];	// PPA.
	IO_ME.INTSTATE.bit.PPBINT	= me_ctrl->apbbrg.pp_enable[ E_IM_ME_PPNUM_PPB ];	// PPB.
	IO_ME.INTSTATE.bit.PPCINT	= me_ctrl->apbbrg.pp_enable[ E_IM_ME_PPNUM_PPC ];	// PPC.
	IO_ME.INTMODE.bit.INTMODE	= me_ctrl->apbbrg.int_mode;							// Interrupt mode.
	IO_ME.INTCNTR.bit.INTERVAL	= me_ctrl->apbbrg.interval;							// Interval.

	for ( int i = 0; i < E_IM_ME_PPNUM_MAX; i++ ) {

		if ( me_ctrl->apbbrg.pp_enable[ i ] == D_IM_ME_ON ) {
			// Input mode.
			IO_ME.PME[ i ].INPUTMODE.bit.INPUTMODE	= me_ctrl->pme[ i ].input_mode;
			IO_ME.PME[ i ].INPUTMODE.bit.REF_TXB	= me_ctrl->pme[ i ].ref_txb;

			// The limit of the use of off-screen as a reference pixel.
			IO_ME.PME[ i ].OUTLIMIT.bit.LIMIT		= me_ctrl->pme[ i ].out_limit;

			// PME processing target number of screens.
			IO_ME.PME[ i ].M1BANKNUM.bit.BANKNUM	= me_ctrl->pme[ i ].bank_num;

			// M1 original image mode.
			for ( int j = 0; j < E_IM_ME_BANKNUM_MAX; j++ ) {
				IO_ME.PME[ i ].M1ORGMODE[ j ].bit.DIRECTION	= me_ctrl->pme[ i ].m1org_mode[ j ].direction;
				IO_ME.PME[ i ].M1ORGMODE[ j ].bit.PARITY	= me_ctrl->pme[ i ].m1org_mode[ j ].parity;
				IO_ME.PME[ i ].M1ORGMODE[ j ].bit.READMODE	= me_ctrl->pme[ i ].m1org_mode[ j ].read_mode;
				IO_ME.PME[ i ].M1ORGMODE[ j ].bit.SAD0MODE	= me_ctrl->pme[ i ].m1org_mode[ j ].sad0_mode;
				IO_ME.PME[ i ].M1ORGMODE[ j ].bit.SAD1MODE	= me_ctrl->pme[ i ].m1org_mode[ j ].sad1_mode;
				IO_ME.PME[ i ].M1ORGMODE[ j ].bit.VECMODE	= me_ctrl->pme[ i ].m1org_mode[ j ].vec_mode;
				IO_ME.PME[ i ].M1ORGMODE[ j ].bit.VECSEL	= me_ctrl->pme[ i ].m1org_mode[ j ].vec_select;
				IO_ME.PME[ i ].M1ORGMODE[ j ].bit.PICTYPE	= me_ctrl->pme[ i ].m1org_mode[ j ].pic_type;
			}

			// M1 correct.
			IO_ME.PME[ i ].M1CORRECT.bit.FKY		= me_ctrl->pme[ i ].m1correct.front_ky;
			IO_ME.PME[ i ].M1CORRECT.bit.BKY		= me_ctrl->pme[ i ].m1correct.back_ky;
			IO_ME.PME[ i ].M1CORRECT.bit.FKX		= me_ctrl->pme[ i ].m1correct.front_kx;
			IO_ME.PME[ i ].M1CORRECT.bit.BKX		= me_ctrl->pme[ i ].m1correct.back_kx;
			IO_ME.PME[ i ].M1CORRECT.bit.MODE		= me_ctrl->pme[ i ].m1correct.mode;
			IO_ME.PME[ i ].M1CORRECT.bit.FH			= me_ctrl->pme[ i ].m1correct.front_h;
			IO_ME.PME[ i ].M1CORRECT.bit.FV			= me_ctrl->pme[ i ].m1correct.front_v;
			IO_ME.PME[ i ].M1CORRECT.bit.BH			= me_ctrl->pme[ i ].m1correct.back_h;
			IO_ME.PME[ i ].M1CORRECT.bit.BV			= me_ctrl->pme[ i ].m1correct.back_v;

			// M1 MC on/off.
			IO_ME.PME[ i ].M1MC.bit.P				= me_ctrl->pme[ i ].m1mc.p_pic;
			IO_ME.PME[ i ].M1MC.bit.B				= me_ctrl->pme[ i ].m1mc.b_pic;

			// M1 Split.
			IO_ME.PME[ i ].M1SPLT.bit.MODE			= me_ctrl->pme[ i ].m1splt.mode;
			IO_ME.PME[ i ].M1SPLT.bit.TH			= me_ctrl->pme[ i ].m1splt.th_h;
			IO_ME.PME[ i ].M1SPLT.bit.TV			= me_ctrl->pme[ i ].m1splt.th_v;

			// M1 Skip.
			IO_ME.PME[ i ].M1SKIP.bit.SKIP			= me_ctrl->pme[ i ].m1skip;

			// M1 Sad.
			IO_ME.PME[ i ].M1SADSUM.bit.U			= me_ctrl->pme[ i ].m1sad.upper;
			IO_ME.PME[ i ].M1SADSUM.bit.L			= me_ctrl->pme[ i ].m1sad.lower;

			// M1 Adjust.
			IO_ME.PME[ i ].M1ADJUST.bit.CYCLE		= me_ctrl->pme[ i ].m1adjust.wait_cycle;
			IO_ME.PME[ i ].M1ADJUST.bit.ON			= me_ctrl->pme[ i ].m1adjust.wait_onoff;

			// M1 Pause.
			IO_ME.PME[ i ].M1PAUSE.bit.LINE			= me_ctrl->pme[ i ].m1pause.pause_line;
			IO_ME.PME[ i ].M1PAUSE.bit.ON			= me_ctrl->pme[ i ].m1pause.pause_onoff;

			// Vector search limit parameter (vertical).
			IO_ME.PME[ i ].M1LIMITMV[ E_IM_ME_BANKNUM_0 ].bit.P	= me_ctrl->pme[ i ].m1limit.v_plus;
			IO_ME.PME[ i ].M1LIMITMV[ E_IM_ME_BANKNUM_0 ].bit.M	= me_ctrl->pme[ i ].m1limit.v_minus;

			// Vector search limit parameter (horizon).
			IO_ME.PME[ i ].M1LIMITMH[ E_IM_ME_BANKNUM_0 ].bit.P	= me_ctrl->pme[ i ].m1limit.h_plus;
			IO_ME.PME[ i ].M1LIMITMH[ E_IM_ME_BANKNUM_0 ].bit.M	= me_ctrl->pme[ i ].m1limit.h_minus;

			// PME Core mode.
			IO_ME.PME[ i ].COREMODE.bit.LINEINTV	= me_ctrl->pme[ i ].core_mode.line_interval;
			IO_ME.PME[ i ].COREMODE.bit.INITPOSY	= me_ctrl->pme[ i ].core_mode.init_pos_y;
		}
	}

	// PME number.
	IO_ME.PF1_PMENUM.bit.PMENUM			= me_ctrl->rmcif.pme_num;

	// Request arbitration method selection.
	IO_ME.ABT_TYPE.bit.ABT_TYPE			= me_ctrl->rmcif.abt_type;

	// Bank table parameter.
	IO_ME.BANK_TBL_PVEC.bit.FWD_PVEC	= me_ctrl->rmcif.bank_tbl.fwd_pvec;
	IO_ME.BANK_TBL_8PELREF.bit.TOP		= me_ctrl->rmcif.bank_tbl.top;
	IO_ME.BANK_TBL_8PEL.bit.FWD_8PEL	= me_ctrl->rmcif.bank_tbl.fwd_8pel;

	// AXI setting.
	IO_ME.AXI_CNTL_SET.bit.ARCACHE		= me_ctrl->rmcif.axi_if.r_cache_type;
	IO_ME.AXI_CNTL_SET.bit.ARPROT		= me_ctrl->rmcif.axi_if.r_protect_type;
	IO_ME.AXI_CNTL_SET.bit.AWCACHE		= me_ctrl->rmcif.axi_if.w_cache_type;
	IO_ME.AXI_CNTL_SET.bit.AWPROT		= me_ctrl->rmcif.axi_if.w_protect_type;

	// Fidxed value.
	IO_ME.PF1_ON.bit.PF1_ON				= D_IM_ME_ON;
	IO_ME.PF1_ON.bit.PFBUF_EN			= D_IM_ME_ON;
	IO_ME.PF1_TBSEL.bit.TBSEL			= D_IM_ME_OFF;

	im_me_off_clk();
	im_me_off_xclk();
	im_me_off_pclk();
	im_me_dsb();

	return D_DDIM_OK;
}

/**
ME start.<br>
@param[in]		me_start					ME start parameters. See @ref T_IM_ME_START.<br>
@retval			D_DDIM_OK					Success.
@retval			D_IM_ME_INPUT_PARAM_ERROR	Parameter error.
*/
INT32 Im_ME_Start( const T_IM_ME_START* const me_start )
{
	union io_me_irqsc	meint;

#ifdef CO_PARAM_CHECK
	if ( me_start == NULL ) {
		Ddim_Assertion(("Im_ME_Start() error. me_start = NULL.\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
	if ( ( me_start->rmcif.search_width < D_IM_ME_SRCH_WIDTH_MIN ) ||
		 ( me_start->rmcif.search_width > D_IM_ME_SRCH_WIDTH_MAX ) ) {
		Ddim_Assertion(("Im_ME_Start() error. search_width is invalid.\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
	if ( ( me_start->rmcif.pre_width < D_IM_ME_PRE_WIDTH_MIN ) ||
		 ( me_start->rmcif.pre_width > D_IM_ME_PRE_WIDTH_MAX ) ) {
		Ddim_Assertion(("Im_ME_Start() error. pre_width is invalid.\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
	if ( ( me_start->rmcif.pre_height < D_IM_ME_PRE_HEIGHT_MIN ) ||
		 ( me_start->rmcif.pre_height > D_IM_ME_PRE_HEIGHT_MAX ) ) {
		Ddim_Assertion(("Im_ME_Start() error. pre_height is invalid.\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
	if ( ( me_start->rmcif.cur_margin_width != D_IM_ME_CUR_MARGIN_0 ) &&
		 ( me_start->rmcif.cur_margin_width != D_IM_ME_CUR_MARGIN_4 ) &&
		 ( me_start->rmcif.cur_margin_width != D_IM_ME_CUR_MARGIN_8 ) ) {
		Ddim_Assertion(("Im_ME_Start() error. cur_margin_width is invalid.\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
	if ( ( me_start->rmcif.cur_margin_height != D_IM_ME_CUR_MARGIN_0 ) &&
		 ( me_start->rmcif.cur_margin_height != D_IM_ME_CUR_MARGIN_4 ) &&
		 ( me_start->rmcif.cur_margin_height != D_IM_ME_CUR_MARGIN_8 ) ) {
		Ddim_Assertion(("Im_ME_Start() error. cur_margin_height is invalid.\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
	if ( ( me_start->rmcif.prefetch_width < D_IM_ME_PREF_WIDTH_MIN ) ||
		 ( me_start->rmcif.prefetch_width > D_IM_ME_PREF_WIDTH_MAX ) ) {
		Ddim_Assertion(("Im_ME_Start() error. prefetch_width is invalid.\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
	if ( ( me_start->rmcif.prefetch_height < D_IM_ME_PREF_HEIGHT_MIN ) ||
		 ( me_start->rmcif.prefetch_height > D_IM_ME_PREF_HEIGHT_MAX ) ) {
		Ddim_Assertion(("Im_ME_Start() error. prefetch_height is invalid.\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
	if ( ( me_start->rmcif.search_height < D_IM_ME_SRCH_HEIGHT_MIN ) ||
		 ( me_start->rmcif.search_height > D_IM_ME_SRCH_HEIGHT_MAX ) ) {
		Ddim_Assertion(("Im_ME_Start() error. search_height is invalid.\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
	for ( int i = 0; i < E_IM_ME_PPNUM_MAX; i++ ) {
		if ( me_start->pme[ i ].mbnum_h > D_IM_ME_MBNUM_H_MAX ) {
			Ddim_Assertion(("Im_ME_Start() error. mbnum_h[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_start->pme[ i ].mbnum_v > D_IM_ME_MBNUM_V_MAX ) {
			Ddim_Assertion(("Im_ME_Start() error. mbnum_v[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( me_start->pme[ i ].cancel_mode > D_IM_ME_VEC_CANCEL_MODE_CUR2 ) {
			Ddim_Assertion(("Im_ME_Start() error. pme[ %d ].cancel_mode is invalid.\n", i ));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
		if ( ( me_start->pme[ i ].vec_hsize != D_IM_ME_PRE_VEC_HSIZE_512  ) &&
			 ( me_start->pme[ i ].vec_hsize != D_IM_ME_PRE_VEC_HSIZE_1024 ) ) {
			Ddim_Assertion(("Im_ME_Start() error. vec_hsize[ %d ] is invalid.\n", i));
			return D_IM_ME_INPUT_PARAM_ERROR;
		}
	}
#endif

	im_me_on_pclk();
	im_me_on_xclk();
	im_me_on_clk();
	im_me_dsb();

	im_me_lock();

	// Wait ME Stop.
	while ( ( IO_ME.PME[ E_IM_ME_PPNUM_PPA ].STATE.bit.ME1_STATE != D_IM_ME_ME1_STATE_STOP ) ||
			( IO_ME.PME[ E_IM_ME_PPNUM_PPB ].STATE.bit.ME1_STATE != D_IM_ME_ME1_STATE_STOP ) ||
			( IO_ME.PME[ E_IM_ME_PPNUM_PPC ].STATE.bit.ME1_STATE != D_IM_ME_ME1_STATE_STOP ) ||
			( IO_ME.PF1_START.bit.STR != D_IM_ME_STR_STOP ) ) {

		DDIM_User_Dly_Tsk( 1 );

		Ddim_Print(("Im_ME_Start() : ME Processing\n"));
	}

	(VOID)DDIM_User_Clr_Flg( FID_IM_ME, ~D_IM_ME_FLG_END );

	// Interrupt flag clear.
	meint.word			= D_IM_ME_OFF;
	meint.bit.__IRQ0	= D_IM_ME_ON;
	meint.bit.__IRQ1	= D_IM_ME_ON;
	meint.bit.__IRQ2	= D_IM_ME_ON;
	meint.bit.__IRQ3	= D_IM_ME_ON;
	meint.bit.__IRQ4	= D_IM_ME_ON;
	meint.bit.__IRQ5	= D_IM_ME_ON;
	meint.bit.__IRQ6	= D_IM_ME_ON;
	meint.bit.__IRQ7	= D_IM_ME_ON;
	meint.bit.__IRQ8	= D_IM_ME_ON;
	IO_ME.PME[ E_IM_ME_PPNUM_PPA ].IRQSC.word	= meint.word;
	IO_ME.PME[ E_IM_ME_PPNUM_PPB ].IRQSC.word	= meint.word;
	IO_ME.PME[ E_IM_ME_PPNUM_PPC ].IRQSC.word	= meint.word;

	gIM_ME_Int_Cnt = 0;

	// Previous shift.
	im_me_set_reg_signed_a( IO_ME.SHIFT_3DNRME, union io_me_shift3dnrme, X, me_start->rmcif.pre_shift_x );
	im_me_set_reg_signed_a( IO_ME.SHIFT_3DNRME, union io_me_shift3dnrme, Y, me_start->rmcif.pre_shift_y );

	// Previous image read address.
	IO_ME.PREV_GBO.bit.H			= me_start->rmcif.pre_r_addr_hi;
	IO_ME.PREV_GBO.bit.L			= me_start->rmcif.pre_r_addr_lo;

	// Current image read address.
	IO_ME.M1_GBO.bit.H				= me_start->rmcif.cur_r_addr_hi;
	IO_ME.M1_GBO.bit.L				= me_start->rmcif.cur_r_addr_lo;

	// Vector image write address.
	IO_ME.PVEC_GBO.bit.H			= me_start->rmcif.vec_w_addr_hi;
	IO_ME.PVEC_GBO.bit.L			= me_start->rmcif.vec_w_addr_lo;

	// Previous image size (1/4 size).
	IO_ME.PREV_GLSS.bit.GLSS		= me_start->rmcif.pre_gwidth;
	IO_ME.PREVSZ_3DNRME.bit.H		= me_start->rmcif.pre_width;
	IO_ME.PREVSZ_3DNRME.bit.V		= me_start->rmcif.pre_height;

	// Current image size (1/4 size).
	IO_ME.M1_GLSS.bit.GLSS			= me_start->rmcif.cur_gwidth;

	// Current image margin size.
	IO_ME.MRGN_3DNRME.bit.H			= me_start->rmcif.cur_margin_width;
	IO_ME.MRGN_3DNRME.bit.V			= me_start->rmcif.cur_margin_height;

	// Prefetch size.
	IO_ME.PF_SIZE.bit.H				= me_start->rmcif.prefetch_width;
	IO_ME.PF_SIZE.bit.V				= me_start->rmcif.prefetch_height;

	// ME1 initialize search size.
	IO_ME.PF1_INISIZE.bit.SIZE_X	= me_start->rmcif.search_width;
	IO_ME.PF1_INISIZE.bit.SIZE_Y	= me_start->rmcif.search_height;

	// Prefetch start.
	IO_ME.PF1_START.bit.STR			= D_IM_ME_STR_START;

	for ( int i = 0; i < E_IM_ME_PPNUM_MAX; i++ ) {

		if ( gIM_ME_Ctrl.apbbrg.pp_enable[ i ] == D_IM_ME_ON ) {
			// MB number.
			IO_ME.PME[ i ].MBNUM.bit.H				= me_start->pme[ i ].mbnum_h;
			IO_ME.PME[ i ].MBNUM.bit.V				= me_start->pme[ i ].mbnum_v;

			// Vector cancel function.
			IO_ME.PME[ i ].CNCL.bit.MODE			= me_start->pme[ i ].cancel_mode;
			IO_ME.PME[ i ].CNCL.bit.THH				= me_start->pme[ i ].cancel_threshold_hi;
			IO_ME.PME[ i ].CNCL.bit.THL				= me_start->pme[ i ].cancel_threshold_lo;

			// Vector output.
			IO_ME.PME[ i ].VOUT_FMT.bit.HSIZE		= me_start->pme[ i ].vec_hsize;

			// PME start.
			IO_ME.PME[ i ].M1START.bit.M1PIC_STR	= D_IM_ME_ON;
		}
	}

	im_me_unlock();

	im_me_off_pclk();
	im_me_dsb();

	return D_DDIM_OK;
}

/**
ME wait end.<br>
@param[out]		p_wait_factor				Pointer of factor parameter which release wait process.
@param[in]		wait_time					Wait time [msec]. The valid range is -1, 0, ... .
											If this parameter is set to -1, driver waits permanently unless system-call sets event-flag.
@retval			D_DDIM_OK					Success.
@retval			D_IM_ME_INPUT_PARAM_ERROR	Parameter error.
@retval			D_IM_ME_TIMEOUT				Time out.
@retval			D_IM_ME_AXI_ERROR			AXI BUS error.
@retval			D_IM_ME_NG					System NG.
*/
INT32 Im_ME_Wait_End( UINT32* const p_wait_factor, const INT32 wait_time )
{
	DDIM_USER_FLGPTN	waitptn;
	DDIM_USER_FLGPTN	flgptn;
	DDIM_USER_ER		ercd;
	INT32				ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ( ( p_wait_factor == NULL ) ||
		 ( im_me_check_wait_time( wait_time ) == FALSE) ) {
		Ddim_Assertion(("Im_ME_Wait_End parameter error\n"));
		return D_IM_ME_INPUT_PARAM_ERROR;
	}
#endif

	waitptn = D_IM_ME_FLG_END;

	// ME wait end.
	ercd = DDIM_User_Twai_Flg( FID_IM_ME, waitptn, D_DDIM_USER_TWF_ORW, &flgptn, (DDIM_USER_TMO)wait_time );

	if ( ercd == D_DDIM_USER_E_TMOUT ) {

		Ddim_Print(("Im_ME_Wait_End() time out. ercd = %d\n", ercd));

		ret = D_IM_ME_TIMEOUT;
	}
	else if ( ercd != D_DDIM_USER_E_OK ) {

		Ddim_Print(("Im_ME_Wait_End() system error. ercd = %d\n", ercd));

		ret = D_IM_ME_NG;
	}
	else {
		flgptn = waitptn & flgptn;
		(VOID)DDIM_User_Clr_Flg( FID_IM_ME, ~flgptn );
	}

	*p_wait_factor = (UINT32)flgptn;

	im_me_on_pclk();
	im_me_dsb();

	if ( ret == D_DDIM_OK ) {
		// Check AXI error.
		if ( gIM_ME_Axi_Err != D_IM_ME_AXI_NORMAL_END ) {
			Ddim_Print(("Im_ME_Wait_End() AXI error. ercd = %lu\n", gIM_ME_Axi_Err));
#ifdef CO_ME_DEBUG_PRINT
			Ddim_Print(("RESP_CIR = %d\n", IO_ME.AXI_ERR_STATUS.bit.RESP_CIR));
			Ddim_Print(("RESP_PRE = %d\n", IO_ME.AXI_ERR_STATUS.bit.RESP_PRE));
			Ddim_Print(("RESP_VEC = %d\n", IO_ME.AXI_ERR_STATUS.bit.RESP_VEC));
#endif
			ret = D_IM_ME_AXI_ERROR;
		}
	}

	// AXI error clear.
	IO_ME.AXI_ERR_CLR.bit.CUR_CLR = D_IM_ME_ON;
	IO_ME.AXI_ERR_CLR.bit.PRE_CLR = D_IM_ME_ON;
	IO_ME.AXI_ERR_CLR.bit.VEC_CLR = D_IM_ME_ON;

	im_me_off_clk();
	im_me_off_xclk();
	im_me_off_pclk();
	im_me_dsb();

	return ret;
}

/**
ME interupt handler.<br>
@retval			None.
*/
VOID Im_ME_Int_Handler( VOID )
{
	DDIM_USER_ER				ercd;
	union io_me_axierrstatus	axi_err_status;
	union io_me_irqsc			meint;

	gIM_ME_Axi_Err = D_IM_ME_AXI_NORMAL_END;

	axi_err_status.word = IO_ME.AXI_ERR_STATUS.word;

	// AXI error check.
	if ( axi_err_status.bit.CUR_STS != 0 ) {	/* pgr0872 */
		gIM_ME_Axi_Err |= D_IM_ME_AXI_ERR_CH_R_CUR;
	}

	// AXI error check.
	if ( axi_err_status.bit.PRE_STS != 0 ) {	/* pgr0872 */
		gIM_ME_Axi_Err |= D_IM_ME_AXI_ERR_CH_W_PRE;
	}

	// AXI error check.
	if ( axi_err_status.bit.VEC_STS != 0 ) {	/* pgr0872 */
		gIM_ME_Axi_Err |= D_IM_ME_AXI_ERR_CH_W_VEC;
	}

#ifdef CO_ME_DEBUG_PRINT
	Ddim_Print(( "Im_ME_Int_Handler axi_err_status=0x%lx\n", axi_err_status.word ));
#endif

	im_me_on_pclk();
	im_me_dsb();

	meint.word = IO_ME.PME[ E_IM_ME_PPNUM_PPA ].IRQSC.word;
	if( meint.bit.__IRQ6 == D_IM_ME_ON ) {		/* pgr0872 */
		gIM_ME_Int_Cnt++;
		IO_ME.PME[ E_IM_ME_PPNUM_PPA ].IRQSC.word = meint.word;
	}

	meint.word = IO_ME.PME[ E_IM_ME_PPNUM_PPB ].IRQSC.word;
	if( meint.bit.__IRQ6 == D_IM_ME_ON ) {		/* pgr0872 */
		gIM_ME_Int_Cnt++;
		IO_ME.PME[ E_IM_ME_PPNUM_PPB ].IRQSC.word = meint.word;
	}

	meint.word = IO_ME.PME[ E_IM_ME_PPNUM_PPC ].IRQSC.word;
	if( meint.bit.__IRQ6 == D_IM_ME_ON ) {		/* pgr0872 */
		gIM_ME_Int_Cnt++;
		IO_ME.PME[ E_IM_ME_PPNUM_PPC ].IRQSC.word = meint.word;
	}

	if ( gIM_ME_Int_Cnt >= 3 ) {
		gIM_ME_Int_Cnt = 0;
		ercd = DDIM_User_Set_Flg( FID_IM_ME, D_IM_ME_FLG_END );
		if ( ercd != D_DDIM_USER_E_OK ) {
			Ddim_Print(("Im_ME_Int_Handler : Error DDIM_User_Set_Flg [%d]\n", ercd));
		}
	}

	im_me_off_pclk();
	im_me_dsb();

	// User callback function.
	if ( gIM_ME_CallBack_Func != NULL ) {
#ifdef CO_ME_DEBUG_PRINT
		Ddim_Print(( "Im_ME_Int_Handler begin callback\n" ));
#endif
		gIM_ME_CallBack_Func( gIM_ME_Axi_Err );

#ifdef CO_ME_DEBUG_PRINT
		Ddim_Print(( "Im_ME_Int_Handler end callback\n" ));
#endif
	}
}

