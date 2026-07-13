/**
 * @file		im_shdr.c
 * @brief		JDSSHDR macro wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "jdsshdr.h"
#include "im_shdr.h"
#include "dd_top.h"
#include "ddim_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define CO_SHDR_REG_TYPE_CHECK
//#define CO_SHDR_DEBUG_PRINT		// Debug.

// Data Synchronization Barrier.
#define im_shdr_dsb()						Dd_ARM_Dsb_Pou()

// Wait usec.
#define im_shdr_wait_usec( usec )			Dd_ARM_Wait_ns( ( usec * 1000 ) )

// The structure type matching check.
#ifdef CO_SHDR_REG_TYPE_CHECK
#define im_shdr_check_reg_type( dst, src )	( dst ) = ( src );
#else
#define im_shdr_check_reg_type( dst, src )
#endif

// Set signed register.
#define im_shdr_set_reg_signed( reg, type, member, val )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_shdr_check_reg_type( work, (reg) ); \
		}

// Set signed register (multi array).
#define im_shdr_set_reg_signed_a( reg, type, member, val )		\
		{ \
			type work = { .word[0] = 0xFFFFFFFFUL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_shdr_check_reg_type( work, (reg) ); \
		}

// Get signed register.
#define im_shdr_get_reg_signed( val, reg, type, member )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = -1L; \
			if( ((work.bit.member) == 0xFFFFFFFFUL) || ((reg.bit.member) <= ((work.bit.member) /2)) ) { \
				(val) = reg.bit.member; \
			} \
			else { \
				(val) = -(work.bit.member - reg.bit.member +1); \
			} \
			im_shdr_check_reg_type( (reg), work );	/* The structure type matching check. */ \
		}

// Get signed register (multi array).
#define im_shdr_get_reg_signed_a( val, reg, type, member )		\
		{ \
			type work = { .word[0] = 0UL }; \
			work.bit.member = -1L; \
			if( ((work.bit.member) == 0xFFFFFFFFUL) || ((reg.bit.member) <= ((work.bit.member) /2)) ) { \
				(val) = reg.bit.member; \
			} \
			else { \
				(val) = -(work.bit.member - reg.bit.member +1); \
			} \
			im_shdr_check_reg_type( (reg), work );	/* The structure type matching check. */ \
		}

#ifdef CO_PARAM_CHECK
#define im_shdr_check_wait_time( wait_time )	(( wait_time >= D_DDIM_USER_SEM_WAIT_FEVR ) ? TRUE : FALSE)
#endif	// CO_PARAM_CHECK

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_IMAGE_CLOCK)
#define	CO_ACT_SHDR_CLOCK
#endif

#if defined(CO_ACT_ICLOCK) || defined(CO_ACT_IMAGE_ICLOCK)
#define	CO_ACT_SHDR_ICLOCK
#endif

#if defined(CO_ACT_HCLOCK) || defined(CO_ACT_IMAGE_HCLOCK)
#define	CO_ACT_SHDR_HCLOCK
#endif

#if defined(CO_ACT_PCLOCK) || defined(CO_ACT_IMAGE_PCLOCK)
#define	CO_ACT_SHDR_PCLOCK
#endif

// Software reset.
#define	D_IM_SHDR_SR_RELEASE			(0)				// Software reset release.
#define	D_IM_SHDR_SR_RESET				(1)				// Software reset state.

// SRAM enable.
#define	D_IM_SHDR_SRAM_DISABLE			(0)				// SRAM disable.
#define	D_IM_SHDR_SRAM_ENABLE			(1)				// SRAM enable.

// Macro stat/stop trigger.
#define	D_IM_SHDR_TRG_STOP				(0)				// W:Stop.
#define	D_IM_SHDR_TRG_START				(1)				// W:Start.
#define	D_IM_SHDR_TRG_STATUS_STOP		(2)				// R:Stop status.
#define	D_IM_SHDR_TRG_STATUS_START		(3)				// R:Start status.

// SDHRINT1 register.
#define D_IM_SHDR_INTF_AXWEFLG			(0x0020)		// __AXWEFLG.
#define D_IM_SHDR_INTF_AXREFLG			(0x0010)		// __AXREFLG.
#define D_IM_SHDR_INTF_SHDRFLG			(0x0001)		// __SHDRFLG.

// Wait time.
#define D_IM_SHDR_SRAM_WAIT_USEC		(1)				// SRAM access wait time.
#define D_IM_SHDR_WAIT_END_TIME			(100)			// SHDR wait end time.

// Interrupt flag.
#define D_IM_SHDR_FLG_END				(0x00000001)	// End.
#define D_IM_SHDR_FLG_STOP				(0x00000002)	// Stop.

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
#ifdef CO_ACT_SHDR_CLOCK
static volatile UCHAR				gIM_SHDR_CLK_Counter	= 0;
#endif

#ifdef CO_ACT_SHDR_ICLOCK
static volatile UCHAR				gIM_SHDR_XCLK_Counter	= 0;
#endif

#ifdef CO_ACT_SHDR_HCLOCK
static volatile UCHAR				gIM_SHDR_HCLK_Counter	= 0;
#endif

#ifdef CO_ACT_SHDR_PCLOCK
static volatile UCHAR				gIM_SHDR_PCLK_Counter	= 0;
#endif

static volatile ULONG				gIM_SHDR_Axi_Err		= D_IM_SHDR_NORMAL_END;

static volatile UCHAR				gIM_SHDR_Out_Ctrl		= D_IM_SHDR_OUTCTRL_YCC;

static volatile ULONG				gIM_SHDR_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

static volatile T_IM_SHDR_CALLBACK			gIM_SHDR_CallBack_Func			= NULL;

static volatile T_IM_SHDR_LINEINT_CALLBACK	gIM_SHDR_LineInt_CallBack_Func	= NULL;

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
// Mutex.
#define im_shdr_lock()				Dd_ARM_Critical_Section_Start( gIM_SHDR_Spin_Lock )
#define im_shdr_unlock()			Dd_ARM_Critical_Section_End( gIM_SHDR_Spin_Lock )

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

/**
CLK ON for SHDR.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_shdr_on_clk( VOID )
{
#ifdef CO_ACT_SHDR_CLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_SHDR_CLK_Counter, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_SHDRCK_BIT );
#endif
}

/**
CLK OFF for SHDR.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_shdr_off_clk( VOID )
{
#ifdef CO_ACT_SHDR_CLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_SHDR_CLK_Counter, &Dd_Top_Get_CLKSTOP7(), D_DD_TOP_SHDRCK_BIT );
#endif
}

/**
XCLK ON for SHDR.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_shdr_on_xclk( VOID )
{
#ifdef CO_ACT_SHDR_ICLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_SHDR_XCLK_Counter, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_SHDRAX_BIT );
#endif
}

/**
XCLK OFF for SHDR.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_shdr_off_xclk( VOID )
{
#ifdef CO_ACT_SHDR_ICLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_SHDR_XCLK_Counter, &Dd_Top_Get_CLKSTOP7(), D_DD_TOP_SHDRAX_BIT );
#endif
}

/**
HCLK ON for SHDR.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_shdr_on_hclk( VOID )
{
#ifdef CO_ACT_SHDR_HCLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_SHDR_HCLK_Counter, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_SHDRAH_BIT );
#endif
}

/**
HCLK OFF for SHDR.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_shdr_off_hclk( VOID )
{
#ifdef CO_ACT_SHDR_HCLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_SHDR_HCLK_Counter, &Dd_Top_Get_CLKSTOP7(), D_DD_TOP_SHDRAH_BIT );
#endif
}

/**
PCLK ON for SHDR.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_shdr_on_pclk( VOID )
{
#ifdef CO_ACT_SHDR_PCLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_SHDR_PCLK_Counter, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_SHDRAP_BIT );
#endif
}

/**
PCLK OFF for SHDR.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_shdr_off_pclk( VOID )
{
#ifdef CO_ACT_SHDR_PCLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_SHDR_PCLK_Counter, &Dd_Top_Get_CLKSTOP7(), D_DD_TOP_SHDRAP_BIT );
#endif
}

/**
SHDR initialize.<br>
@param[in]		None.
@retval			None.
*/
static VOID im_shdr_init( VOID )
{
	im_shdr_wait_usec( D_IM_SHDR_SRAM_WAIT_USEC );

	// Software reset.
	IO_SHDR.SHDRACTL0.bit.SR = D_IM_SHDR_SR_RESET;

	im_shdr_dsb();

	// SHDRCLK       = MAX 400MHz	MIN 160MHz.
	// APB clock     = MAX  50MHz	MIN 12.5MHz.
	// -->   ( (1/SHDRCLK) / (1/APB clock) ) * 2 + 1.
	//     = ( (1/160)     / (1/50)        ) * 2 + 1.
	//     = 1.63 => 2 APB cycle.

	// wait 2 APB clock.
	IO_SHDR.SHDRACTL0.word;		/* pgr0695 */

	im_shdr_dsb();

	// Software reset release.
	IO_SHDR.SHDRACTL0.bit.SR = D_IM_SHDR_SR_RELEASE;

	im_shdr_dsb();

	// wait 2 APB clock.
	IO_SHDR.SHDRACTL0.word;		/* pgr0695 */

	im_shdr_dsb();

	// SRAM access enable.
	IO_SHDR.SHDRACTL2.bit.SRAMAEN = D_IM_SHDR_SRAM_ENABLE;

	// Wait 1us,
	im_shdr_wait_usec( D_IM_SHDR_SRAM_WAIT_USEC );

	im_shdr_dsb();
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
SHDR initialize.<br>
@param[in]		force_flg						Force init flg.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
INT32 Im_SHDR_Init( BOOL force_flg )
{
	INT32 ret = D_DDIM_OK;

	im_shdr_on_pclk();
//	im_shdr_on_xclk();
//	im_shdr_on_clk();

	if ( force_flg ) {

		im_shdr_init();
	}
	else {
		// Check SHDR macro status.
		if ( IO_SHDR.SHDRACTL1.bit.TRG == D_IM_SHDR_TRG_STATUS_STOP ) {

			im_shdr_init();
		}
		else {
			ret = D_IM_SHDR_MACRO_BUSY;
		}
	}

//	im_shdr_off_clk();
//	im_shdr_off_xclk();
	im_shdr_off_pclk();

	return ret;
}

/**
The semaphore of SHDR is acquired.<br>
@param[in]		wait_time						Time-out period.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_SEM_NG				Semaphore acquisition NG.
*/
INT32 Im_SHDR_Open( const INT32 wait_time )
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if ( im_shdr_check_wait_time( wait_time ) == FALSE ) {
		Ddim_Assertion(("Im_SHDR_Open parameter error\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	if ( D_DDIM_USER_SEM_WAIT_POL == wait_time ) {
		ercd = DDIM_User_Pol_Sem( SID_IM_SHDR );
	}
	else {
		ercd = DDIM_User_Twai_Sem( SID_IM_SHDR, (DDIM_USER_TMO)wait_time );
	}

	if ( D_DDIM_USER_E_OK == ercd ) {
		return D_DDIM_OK;
	}
	else {
		return D_IM_SHDR_SEM_NG;
	}
}

/**
The semaphore of SHDR is returned.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_SEM_NG				Semaphore acquisition NG.
*/
INT32 Im_SHDR_Close( VOID )
{
	DDIM_USER_ER	ercd;

	ercd = DDIM_User_Sig_Sem( SID_IM_SHDR );

	if ( D_DDIM_USER_E_OK == ercd ) {
		return D_DDIM_OK;
	}
	else {
		return D_IM_SHDR_SEM_NG;
	}
}

/**
SHDR common parameters set.<br>
@param[in]		shdr_ctrl						Setting of SHDR parameters. See @ref T_IM_SHDR_CTRL.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Ctrl_Common( const T_IM_SHDR_CTRL* const shdr_ctrl )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_ctrl == NULL ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. shdr_ctrl = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->image_set.in_bit_depth > D_IM_SHDR_INBITDEPTH_16 ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. in_bit_depth is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->image_set.in_c_fmt > D_IM_SHDR_INCFMT_422SP ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. in_c_fmt is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->image_set.in_edge_cpy > D_IM_SHDR_ON ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. in_edge_cpy is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->image_set.in_edge_cut > D_IM_SHDR_ON ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. in_edge_cut is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->image_set.out_ctrl > D_IM_SHDR_OUTCTRL_RGB ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. out_ctrl is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->image_set.out_sel > D_IM_SHDR_OUTSEL_RGB_16 ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. out_sel is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->image_set.out_sel_en > D_IM_SHDR_ENABLE ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. out_sel_en is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	if ( shdr_ctrl->image_mode.fil_margin > D_IM_SHDR_FLITER_MARGIN_96 ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. fil_margin is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	if ( shdr_ctrl->image_mode.fil_margin > D_IM_SHDR_FLITER_MARGIN_32 ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. fil_margin is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	if ( shdr_ctrl->image_mode.fmd_en > D_IM_SHDR_ENABLE ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. fmd_en is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->image_mode.bypass_en > D_IM_SHDR_ENABLE ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. bypass_en is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->image_mode.smc_in > D_IM_SHDR_SMCIN_EXT_REF ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. smc_in is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->image_mode.shdr_in > D_IM_SHDR_SHDRIN_SMCOUT_REF ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. shdr_in is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->image_mode.tile_size > D_IM_SHDR_TILESIZE_1024x512 ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. tile_size is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->sdram.r_vfm.a > D_IM_SHDR_VFMT_VIDEO ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. r_vfm is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->sdram.w_vfm.a > D_IM_SHDR_VFMT_COMPIP_RASTER ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. w_vfm.a is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->sdram.w_vfm.b > D_IM_SHDR_VFMT_VIDEO ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. w_vfm.b is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->compip.mode > D_IM_SHDR_COMP_MODE_LOSSLESS ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. compip.mode is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->compip.mc_mode > D_IM_SHDR_COMP_MC_USED ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. compip.mc_mode is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->compip.comp_incfmt > D_IM_SHDR_COMP_INCFMT_YUV422 ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. compip.comp_incfmt is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->compip.r_bypass_mode > D_IM_SHDR_ENABLE ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. compip.r_bypass_mode is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->compip.w_bypass_mode > D_IM_SHDR_ENABLE ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. compip.w_bypass_mode is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->compip.r_img_axi_size > D_IM_SHDR_COMP_AXI_256 ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. compip.r_img_axi_size is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->compip.r_comp_axi_size > D_IM_SHDR_COMP_AXI_256 ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. compip.r_comp_axi_size is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->compip.w_img_axi_size > D_IM_SHDR_COMP_AXI_256 ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. compip.w_img_axi_size is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->compip.lint_mask > D_IM_SHDR_ENABLE ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. compip.lint_mask is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_ctrl->compip.lint_sts > D_IM_SHDR_ENABLE ) {
		Ddim_Assertion(("Im_SHDR_Ctrl_Common() error. compip.lint_mask is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	// limitation check 1.
	if ( ( shdr_ctrl->image_set.in_bit_depth == D_IM_SHDR_INBITDEPTH_16 ) &&
		 ( shdr_ctrl->image_set.in_c_fmt     == D_IM_SHDR_INCFMT_420SP  ) ) {
		Ddim_Print(("Im_SHDR_Ctrl_Common() : limitation check 1 NG\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}

	// limitation check 2.
	if ( ( shdr_ctrl->image_mode.bypass_en  == D_IM_SHDR_DISABLE                ) &&
		 ( shdr_ctrl->image_set.in_edge_cpy != shdr_ctrl->image_set.in_edge_cut ) ) {
		Ddim_Print(("Im_SHDR_Ctrl_Common() : limitation check 2 NG\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}

	// limitation check 3.
	if ( ( shdr_ctrl->image_mode.shdr_in    == D_IM_SHDR_SHDRIN_SMCOUT_REF ) &&
		 ( shdr_ctrl->image_set.in_edge_cut == D_IM_SHDR_ON                ) ) {
		Ddim_Print(("Im_SHDR_Ctrl_Common() : limitation check 3 NG\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}

	// limitation check 4.
	if ( shdr_ctrl->image_set.out_ctrl != shdr_ctrl->image_set.out_sel ) {
		Ddim_Print(("Im_SHDR_Ctrl_Common() : limitation check 4 NG\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}

	// limitation check 5.
	if ( ( shdr_ctrl->image_set.in_edge_cpy == D_IM_SHDR_OFF ) &&
		 ( shdr_ctrl->image_mode.shift_x != 0 ) &&
		 ( shdr_ctrl->image_mode.shift_y != 0 ) ) {
		Ddim_Print(("Im_SHDR_Ctrl_Common() : limitation check 5 NG\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}

	im_shdr_on_pclk();
	im_shdr_dsb();

	// Wait SHDR Stop.
	while ( IO_SHDR.SHDRACTL1.bit.TRG == D_IM_SHDR_TRG_STATUS_START ) {

		DDIM_User_Dly_Tsk( 1 );

		Ddim_Print(("Im_SHDR_Ctrl_Common() : SHDR Processing\n"));
	}

	// User callback function setting.
	gIM_SHDR_Axi_Err				= D_IM_SHDR_NORMAL_END;
	gIM_SHDR_CallBack_Func			= shdr_ctrl->pCallBack;
	gIM_SHDR_LineInt_CallBack_Func	= shdr_ctrl->pLineInt_CallBack;

	gIM_SHDR_Out_Ctrl				= shdr_ctrl->image_set.out_ctrl;

	// Image setting.
	// Input.
	IO_SHDR.SHDRIFRM.bit.IBITS0					= shdr_ctrl->image_set.in_bit_depth;
	IO_SHDR.SHDRIFRM.bit.INCFMT					= shdr_ctrl->image_set.in_c_fmt;
	IO_SHDR.SHDRIFRM.bit.EDGECPY				= shdr_ctrl->image_set.in_edge_cpy;
	IO_SHDR.SHDRIFRM.bit.EDGECUT				= shdr_ctrl->image_set.in_edge_cut;

	// Output.
	IO_SHDR.SHDROFRM.bit.OUTCTRL				= shdr_ctrl->image_set.out_ctrl;
	IO_SHDR.SHDROFRM.bit.OUTSEL					= shdr_ctrl->image_set.out_sel;
	IO_SHDR.SHDROFRM.bit.OUTSELEN				= shdr_ctrl->image_set.out_sel_en;

	// Read SDRAM setting.
	// Address.
	IO_SHDR.SDRR_ADDR_A.bit.SDRR_ADDR_A_0		= shdr_ctrl->sdram.r_addr.a_y;
	IO_SHDR.SDRR_ADDR_A.bit.SDRR_ADDR_A_1		= shdr_ctrl->sdram.r_addr.a_c;
	IO_SHDR.SDRR_ADDR_B.bit.SDRR_ADDR_B_0		= shdr_ctrl->sdram.r_addr.b_y;
	IO_SHDR.SDRR_ADDR_B.bit.SDRR_ADDR_B_1		= shdr_ctrl->sdram.r_addr.b_c;
	IO_SHDR.SDRR_ADDR_C.bit.SDRR_ADDR_C_0		= shdr_ctrl->sdram.r_addr.c_y;
	IO_SHDR.SDRR_ADDR_C.bit.SDRR_ADDR_C_1		= shdr_ctrl->sdram.r_addr.c_c;
	IO_SHDR.SDRR_ADDR_D.bit.SDRR_ADDR_D_0		= shdr_ctrl->sdram.r_addr.d_y;
	IO_SHDR.SDRR_ADDR_D.bit.SDRR_ADDR_D_1		= shdr_ctrl->sdram.r_addr.d_c;
	IO_SHDR.SDRR_ADDR_E.bit.SDRR_ADDR_E_0		= shdr_ctrl->sdram.r_addr.e_y;
	IO_SHDR.SDRR_ADDR_E.bit.SDRR_ADDR_E_1		= shdr_ctrl->sdram.r_addr.e_c;
	IO_SHDR.SDRR_ADDR_F.bit.SDRR_ADDR_F_0		= shdr_ctrl->sdram.r_addr.f;

	// Global horizontal size.
	IO_SHDR.SDRR_GHSIZE_A.bit.SDRR_GHSIZE_A_0	= shdr_ctrl->sdram.r_ghsize.a_y;
	IO_SHDR.SDRR_GHSIZE_A.bit.SDRR_GHSIZE_A_1	= shdr_ctrl->sdram.r_ghsize.a_c;
	IO_SHDR.SDRR_GHSIZE_B.bit.SDRR_GHSIZE_B_0	= shdr_ctrl->sdram.r_ghsize.b_y;
	IO_SHDR.SDRR_GHSIZE_B.bit.SDRR_GHSIZE_B_1	= shdr_ctrl->sdram.r_ghsize.b_c;
	IO_SHDR.SDRR_GHSIZE_C.bit.SDRR_GHSIZE_C_0	= shdr_ctrl->sdram.r_ghsize.c_y;
	IO_SHDR.SDRR_GHSIZE_C.bit.SDRR_GHSIZE_C_1	= shdr_ctrl->sdram.r_ghsize.c_c;
	IO_SHDR.SDRR_GHSIZE_D.bit.SDRR_GHSIZE_D_0	= shdr_ctrl->sdram.r_ghsize.d_y;
	IO_SHDR.SDRR_GHSIZE_D.bit.SDRR_GHSIZE_D_1	= shdr_ctrl->sdram.r_ghsize.d_c;
	IO_SHDR.SDRR_GHSIZE_E.bit.SDRR_GHSIZE_E_0	= shdr_ctrl->sdram.r_ghsize.e_y;
	IO_SHDR.SDRR_GHSIZE_E.bit.SDRR_GHSIZE_E_1	= shdr_ctrl->sdram.r_ghsize.e_c;
	IO_SHDR.SDRR_GHSIZE_F.bit.SDRR_GHSIZE_F_0	= shdr_ctrl->sdram.r_ghsize.f;

	// Video format.
	IO_SHDR.SDRR_VFM.bit.SDRR_VFM_A				= shdr_ctrl->sdram.r_vfm.a;

	// Horizontal size.
	IO_SHDR.SDRR_HSIZE_A.bit.SDRR_HSIZE_A		= shdr_ctrl->sdram.r_hsize.a;
	IO_SHDR.SDRR_HSIZE_B.bit.SDRR_HSIZE_B		= shdr_ctrl->sdram.r_hsize.b;
	IO_SHDR.SDRR_HSIZE_C.bit.SDRR_HSIZE_C		= shdr_ctrl->sdram.r_hsize.c;
	IO_SHDR.SDRR_HSIZE_D.bit.SDRR_HSIZE_D		= shdr_ctrl->sdram.r_hsize.d;
	IO_SHDR.SDRR_HSIZE_E.bit.SDRR_HSIZE_E		= shdr_ctrl->sdram.r_hsize.e;

	// Vertical size.
	IO_SHDR.SDRR_VSIZE_A.bit.SDRR_VSIZE_A		= shdr_ctrl->sdram.r_vsize.a;
	IO_SHDR.SDRR_VSIZE_B.bit.SDRR_VSIZE_B		= shdr_ctrl->sdram.r_vsize.b;
	IO_SHDR.SDRR_VSIZE_C.bit.SDRR_VSIZE_C		= shdr_ctrl->sdram.r_vsize.c;
	IO_SHDR.SDRR_VSIZE_D.bit.SDRR_VSIZE_D		= shdr_ctrl->sdram.r_vsize.d;
	IO_SHDR.SDRR_VSIZE_E.bit.SDRR_VSIZE_E		= shdr_ctrl->sdram.r_vsize.e;

	// Write SDRAM setting.
	// Address.
	IO_SHDR.SDRW_ADDR_B.bit.SDRW_ADDR_B_0		= shdr_ctrl->sdram.w_addr.b_y;
	IO_SHDR.SDRW_ADDR_B.bit.SDRW_ADDR_B_1		= shdr_ctrl->sdram.w_addr.b_c;
	IO_SHDR.SDRW_ADDR_E.bit.SDRW_ADDR_E_0		= shdr_ctrl->sdram.w_addr.e_y;
	IO_SHDR.SDRW_ADDR_E.bit.SDRW_ADDR_E_1		= shdr_ctrl->sdram.w_addr.e_c;
	IO_SHDR.SDRW_ADDR_F.bit.SDRW_ADDR_F_0		= shdr_ctrl->sdram.w_addr.f_y;
	IO_SHDR.SDRW_ADDR_F.bit.SDRW_ADDR_F_1		= shdr_ctrl->sdram.w_addr.f_c;
	IO_SHDR.SDRW_ADDR_G.bit.SDRW_ADDR_G_0		= shdr_ctrl->sdram.w_addr.g_r_y;
	IO_SHDR.SDRW_ADDR_G.bit.SDRW_ADDR_G_1		= shdr_ctrl->sdram.w_addr.g_g_c;
	IO_SHDR.SDRW_ADDR_G.bit.SDRW_ADDR_G_2		= shdr_ctrl->sdram.w_addr.g_b;
	IO_SHDR.SDRW_ADDR_H.bit.SDRW_ADDR_H_0		= shdr_ctrl->sdram.w_addr.h_x;
	IO_SHDR.SDRW_ADDR_H.bit.SDRW_ADDR_H_1		= shdr_ctrl->sdram.w_addr.h_y;
	IO_SHDR.SDRW_ADDR_I.bit.SDRW_ADDR_I_0		= shdr_ctrl->sdram.w_addr.i;

	// Grlobal horizontal size.
	IO_SHDR.SDRW_GHSIZE_B.bit.SDRW_GHSIZE_B_0	= shdr_ctrl->sdram.w_ghsize.b_y;
	IO_SHDR.SDRW_GHSIZE_B.bit.SDRW_GHSIZE_B_1	= shdr_ctrl->sdram.w_ghsize.b_c;
	IO_SHDR.SDRW_GHSIZE_E.bit.SDRW_GHSIZE_E_0	= shdr_ctrl->sdram.w_ghsize.e_y;
	IO_SHDR.SDRW_GHSIZE_E.bit.SDRW_GHSIZE_E_1	= shdr_ctrl->sdram.w_ghsize.e_c;
	IO_SHDR.SDRW_GHSIZE_F.bit.SDRW_GHSIZE_F_0	= shdr_ctrl->sdram.w_ghsize.f_y;
	IO_SHDR.SDRW_GHSIZE_F.bit.SDRW_GHSIZE_F_1	= shdr_ctrl->sdram.w_ghsize.f_c;
	IO_SHDR.SDRW_GHSIZE_G.bit.SDRW_GHSIZE_G_0	= shdr_ctrl->sdram.w_ghsize.g_r_y;
	IO_SHDR.SDRW_GHSIZE_G.bit.SDRW_GHSIZE_G_1	= shdr_ctrl->sdram.w_ghsize.g_g_c;
	IO_SHDR.SDRW_GHSIZE_G.bit.SDRW_GHSIZE_G_2	= shdr_ctrl->sdram.w_ghsize.g_b;
	IO_SHDR.SDRW_GHSIZE_H.bit.SDRW_GHSIZE_H_0	= shdr_ctrl->sdram.w_ghsize.h_x;
	IO_SHDR.SDRW_GHSIZE_H.bit.SDRW_GHSIZE_H_1	= shdr_ctrl->sdram.w_ghsize.h_y;
	IO_SHDR.SDRW_GHSIZE_I.bit.SDRW_GHSIZE_I_0	= shdr_ctrl->sdram.w_ghsize.i;

	// Video format.
	IO_SHDR.SDRW_GHSIZE_I.bit.SDRW_VFM_A		= shdr_ctrl->sdram.w_vfm.a;
	IO_SHDR.SDRW_VFM.bit.SDRW_VFM_B				= shdr_ctrl->sdram.w_vfm.b;

	// Base address.
	IO_SHDR.SDRW_BA_A.bit.SDRW_BA_A_0			= shdr_ctrl->sdram.w_baddr.a_y;
	IO_SHDR.SDRW_BA_A.bit.SDRW_BA_A_1			= shdr_ctrl->sdram.w_baddr.a_c;
	IO_SHDR.SDRW_BA_C.bit.SDRW_BA_C_0			= shdr_ctrl->sdram.w_baddr.c_y;
	IO_SHDR.SDRW_BA_C.bit.SDRW_BA_C_1			= shdr_ctrl->sdram.w_baddr.c_c;
	IO_SHDR.SDRW_BA_D.bit.SDRW_BA_D_0			= shdr_ctrl->sdram.w_baddr.d_y;
	IO_SHDR.SDRW_BA_D.bit.SDRW_BA_D_1			= shdr_ctrl->sdram.w_baddr.d_c;

	// Horizontal size.
	IO_SHDR.SDRW_FW_A.bit.SDRW_FW_A				= shdr_ctrl->sdram.w_hsize.a;
	IO_SHDR.SDRW_FW_C.bit.SDRW_FW_C				= shdr_ctrl->sdram.w_hsize.c;
	IO_SHDR.SDRW_FW_D.bit.SDRW_FW_D				= shdr_ctrl->sdram.w_hsize.d;

	// Image mode.
	// Exposure magnification.
	IO_SHDR.EXMAG.bit.EXMAG						= shdr_ctrl->image_mode.exp_mag;

	// Reciprocal of the exposure magnification.
	IO_SHDR.REMVAL.bit.REXMAG					= shdr_ctrl->image_mode.rexp_mag;
	im_shdr_set_reg_signed( IO_SHDR.REMVAL, union io_shdr_remval, REMSFT, shdr_ctrl->image_mode.rexp_mag_shift );

	// Shift.
	im_shdr_set_reg_signed( IO_SHDR.SHIFT, union io_shdr_shift, SHIFTX, shdr_ctrl->image_mode.shift_x );
	im_shdr_set_reg_signed( IO_SHDR.SHIFT, union io_shdr_shift, SHIFTY, shdr_ctrl->image_mode.shift_y );

	// Filter margin.
	IO_SHDR.MRGN.bit.MRGN						= shdr_ctrl->image_mode.fil_margin;

	// FMC/Bypass enable. SMC/SHDR input select.
	IO_SHDR.FLWCNT.bit.FMDEN					= shdr_ctrl->image_mode.fmd_en;
	IO_SHDR.FLWCNT.bit.BPEN						= shdr_ctrl->image_mode.bypass_en;
	IO_SHDR.FLWCNT.bit.SMCIN					= shdr_ctrl->image_mode.smc_in;
	IO_SHDR.FLWCNT.bit.BRSEL					= shdr_ctrl->image_mode.shdr_in;

	// Tile size select.
	IO_SHDR.TMODE.bit.TMODE						= shdr_ctrl->image_mode.tile_size;

	// COMPIP.
	IO_SHDR.MODESEL.bit.MODE					= shdr_ctrl->compip.mode;
	IO_SHDR.MC_MODESEL.bit.MODE					= shdr_ctrl->compip.mc_mode;
	IO_SHDR.COMP_INCFMSEL.bit.MODE				= shdr_ctrl->compip.comp_incfmt;
	IO_SHDR.BYPASSON.bit.RMD					= shdr_ctrl->compip.r_bypass_mode;
	IO_SHDR.BYPASSON.bit.WMD					= shdr_ctrl->compip.w_bypass_mode;
	IO_SHDR.AXISIZE.bit.RYC						= shdr_ctrl->compip.r_img_axi_size;		// RFID=A0/A1.
	IO_SHDR.AXISIZE.bit.RCI						= shdr_ctrl->compip.r_comp_axi_size;	// RFID=A0/A1.
	IO_SHDR.AXISIZE.bit.WR						= shdr_ctrl->compip.w_img_axi_size;		// RFID=A0/A1.
	IO_SHDR.LINT.bit.NO							= shdr_ctrl->compip.lint_no;
	IO_SHDR.LINT.bit.MSK						= shdr_ctrl->compip.lint_mask;
	IO_SHDR.LINT.bit.ST							= shdr_ctrl->compip.lint_sts;
	IO_SHDR.WRSTARTADDR.bit.ADDR_Y				= shdr_ctrl->compip.w_addr_y;
	IO_SHDR.WRSTARTADDR.bit.ADDR_C				= shdr_ctrl->compip.w_addr_c;
	IO_SHDR.RDSTARTADDR.bit.ADDR_Y				= shdr_ctrl->compip.r_addr_y;
	IO_SHDR.RDSTARTADDR.bit.ADDR_C				= shdr_ctrl->compip.r_addr_c;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR common parameters get.<br>
@param[out]		shdr_ctrl						Setting of SHDR parameters. See @ref T_IM_SHDR_CTRL.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Get_Ctrl_Common( T_IM_SHDR_CTRL* const shdr_ctrl )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_ctrl == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_Ctrl_Common() error. shdr_ctrl = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// Image setting.
	// Input.
	shdr_ctrl->image_set.in_bit_depth	= IO_SHDR.SHDRIFRM.bit.IBITS0;
	shdr_ctrl->image_set.in_c_fmt		= IO_SHDR.SHDRIFRM.bit.INCFMT;
	shdr_ctrl->image_set.in_edge_cpy	= IO_SHDR.SHDRIFRM.bit.EDGECPY;
	shdr_ctrl->image_set.in_edge_cut	= IO_SHDR.SHDRIFRM.bit.EDGECUT;

	// Output.
	shdr_ctrl->image_set.out_ctrl		= IO_SHDR.SHDROFRM.bit.OUTCTRL;
	shdr_ctrl->image_set.out_sel		= IO_SHDR.SHDROFRM.bit.OUTSEL;
	shdr_ctrl->image_set.out_sel_en		= IO_SHDR.SHDROFRM.bit.OUTSELEN;

	// Read SDRAM setting.
	// Address.
	shdr_ctrl->sdram.r_addr.a_y			= IO_SHDR.SDRR_ADDR_A.bit.SDRR_ADDR_A_0;
	shdr_ctrl->sdram.r_addr.a_c			= IO_SHDR.SDRR_ADDR_A.bit.SDRR_ADDR_A_1;
	shdr_ctrl->sdram.r_addr.b_y			= IO_SHDR.SDRR_ADDR_B.bit.SDRR_ADDR_B_0;
	shdr_ctrl->sdram.r_addr.b_c			= IO_SHDR.SDRR_ADDR_B.bit.SDRR_ADDR_B_1;
	shdr_ctrl->sdram.r_addr.c_y			= IO_SHDR.SDRR_ADDR_C.bit.SDRR_ADDR_C_0;
	shdr_ctrl->sdram.r_addr.c_c			= IO_SHDR.SDRR_ADDR_C.bit.SDRR_ADDR_C_1;
	shdr_ctrl->sdram.r_addr.d_y			= IO_SHDR.SDRR_ADDR_D.bit.SDRR_ADDR_D_0;
	shdr_ctrl->sdram.r_addr.d_c			= IO_SHDR.SDRR_ADDR_D.bit.SDRR_ADDR_D_1;
	shdr_ctrl->sdram.r_addr.e_y			= IO_SHDR.SDRR_ADDR_E.bit.SDRR_ADDR_E_0;
	shdr_ctrl->sdram.r_addr.e_c			= IO_SHDR.SDRR_ADDR_E.bit.SDRR_ADDR_E_1;
	shdr_ctrl->sdram.r_addr.f			= IO_SHDR.SDRR_ADDR_F.bit.SDRR_ADDR_F_0;

	// Global horizontal size.
	shdr_ctrl->sdram.r_ghsize.a_y		= IO_SHDR.SDRR_GHSIZE_A.bit.SDRR_GHSIZE_A_0;
	shdr_ctrl->sdram.r_ghsize.a_c		= IO_SHDR.SDRR_GHSIZE_A.bit.SDRR_GHSIZE_A_1;
	shdr_ctrl->sdram.r_ghsize.b_y		= IO_SHDR.SDRR_GHSIZE_B.bit.SDRR_GHSIZE_B_0;
	shdr_ctrl->sdram.r_ghsize.b_c		= IO_SHDR.SDRR_GHSIZE_B.bit.SDRR_GHSIZE_B_1;
	shdr_ctrl->sdram.r_ghsize.c_y		= IO_SHDR.SDRR_GHSIZE_C.bit.SDRR_GHSIZE_C_0;
	shdr_ctrl->sdram.r_ghsize.c_c		= IO_SHDR.SDRR_GHSIZE_C.bit.SDRR_GHSIZE_C_1;
	shdr_ctrl->sdram.r_ghsize.d_y		= IO_SHDR.SDRR_GHSIZE_D.bit.SDRR_GHSIZE_D_0;
	shdr_ctrl->sdram.r_ghsize.d_c		= IO_SHDR.SDRR_GHSIZE_D.bit.SDRR_GHSIZE_D_1;
	shdr_ctrl->sdram.r_ghsize.e_y		= IO_SHDR.SDRR_GHSIZE_E.bit.SDRR_GHSIZE_E_0;
	shdr_ctrl->sdram.r_ghsize.e_c		= IO_SHDR.SDRR_GHSIZE_E.bit.SDRR_GHSIZE_E_1;
	shdr_ctrl->sdram.r_ghsize.f			= IO_SHDR.SDRR_GHSIZE_F.bit.SDRR_GHSIZE_F_0;

	// Video format.
	shdr_ctrl->sdram.r_vfm.a			= IO_SHDR.SDRR_VFM.bit.SDRR_VFM_A;

	// Horizontal size.
	shdr_ctrl->sdram.r_hsize.a			= IO_SHDR.SDRR_HSIZE_A.bit.SDRR_HSIZE_A;
	shdr_ctrl->sdram.r_hsize.b			= IO_SHDR.SDRR_HSIZE_B.bit.SDRR_HSIZE_B;
	shdr_ctrl->sdram.r_hsize.c			= IO_SHDR.SDRR_HSIZE_C.bit.SDRR_HSIZE_C;
	shdr_ctrl->sdram.r_hsize.d			= IO_SHDR.SDRR_HSIZE_D.bit.SDRR_HSIZE_D;
	shdr_ctrl->sdram.r_hsize.e			= IO_SHDR.SDRR_HSIZE_E.bit.SDRR_HSIZE_E;

	// Vertical size.
	shdr_ctrl->sdram.r_vsize.a			= IO_SHDR.SDRR_VSIZE_A.bit.SDRR_VSIZE_A;
	shdr_ctrl->sdram.r_vsize.b			= IO_SHDR.SDRR_VSIZE_B.bit.SDRR_VSIZE_B;
	shdr_ctrl->sdram.r_vsize.c			= IO_SHDR.SDRR_VSIZE_C.bit.SDRR_VSIZE_C;
	shdr_ctrl->sdram.r_vsize.d			= IO_SHDR.SDRR_VSIZE_D.bit.SDRR_VSIZE_D;
	shdr_ctrl->sdram.r_vsize.e			= IO_SHDR.SDRR_VSIZE_E.bit.SDRR_VSIZE_E;

	// Write SDRAM setting.
	// Address.
	shdr_ctrl->sdram.w_addr.b_y			= IO_SHDR.SDRW_ADDR_B.bit.SDRW_ADDR_B_0;
	shdr_ctrl->sdram.w_addr.b_c			= IO_SHDR.SDRW_ADDR_B.bit.SDRW_ADDR_B_1;
	shdr_ctrl->sdram.w_addr.e_y			= IO_SHDR.SDRW_ADDR_E.bit.SDRW_ADDR_E_0;
	shdr_ctrl->sdram.w_addr.e_c			= IO_SHDR.SDRW_ADDR_E.bit.SDRW_ADDR_E_1;
	shdr_ctrl->sdram.w_addr.f_y			= IO_SHDR.SDRW_ADDR_F.bit.SDRW_ADDR_F_0;
	shdr_ctrl->sdram.w_addr.f_c			= IO_SHDR.SDRW_ADDR_F.bit.SDRW_ADDR_F_1;
	shdr_ctrl->sdram.w_addr.g_r_y		= IO_SHDR.SDRW_ADDR_G.bit.SDRW_ADDR_G_0;
	shdr_ctrl->sdram.w_addr.g_g_c		= IO_SHDR.SDRW_ADDR_G.bit.SDRW_ADDR_G_1;
	shdr_ctrl->sdram.w_addr.g_b			= IO_SHDR.SDRW_ADDR_G.bit.SDRW_ADDR_G_2;
	shdr_ctrl->sdram.w_addr.h_x			= IO_SHDR.SDRW_ADDR_H.bit.SDRW_ADDR_H_0;
	shdr_ctrl->sdram.w_addr.h_y			= IO_SHDR.SDRW_ADDR_H.bit.SDRW_ADDR_H_1;
	shdr_ctrl->sdram.w_addr.i			= IO_SHDR.SDRW_ADDR_I.bit.SDRW_ADDR_I_0;

	// Grlobal horizontal size.
	shdr_ctrl->sdram.w_ghsize.b_y		= IO_SHDR.SDRW_GHSIZE_B.bit.SDRW_GHSIZE_B_0;
	shdr_ctrl->sdram.w_ghsize.b_c		= IO_SHDR.SDRW_GHSIZE_B.bit.SDRW_GHSIZE_B_1;
	shdr_ctrl->sdram.w_ghsize.e_y		= IO_SHDR.SDRW_GHSIZE_E.bit.SDRW_GHSIZE_E_0;
	shdr_ctrl->sdram.w_ghsize.e_c		= IO_SHDR.SDRW_GHSIZE_E.bit.SDRW_GHSIZE_E_1;
	shdr_ctrl->sdram.w_ghsize.f_y		= IO_SHDR.SDRW_GHSIZE_F.bit.SDRW_GHSIZE_F_0;
	shdr_ctrl->sdram.w_ghsize.f_c		= IO_SHDR.SDRW_GHSIZE_F.bit.SDRW_GHSIZE_F_1;
	shdr_ctrl->sdram.w_ghsize.g_r_y		= IO_SHDR.SDRW_GHSIZE_G.bit.SDRW_GHSIZE_G_0;
	shdr_ctrl->sdram.w_ghsize.g_g_c		= IO_SHDR.SDRW_GHSIZE_G.bit.SDRW_GHSIZE_G_1;
	shdr_ctrl->sdram.w_ghsize.g_b		= IO_SHDR.SDRW_GHSIZE_G.bit.SDRW_GHSIZE_G_2;
	shdr_ctrl->sdram.w_ghsize.h_x		= IO_SHDR.SDRW_GHSIZE_H.bit.SDRW_GHSIZE_H_0;
	shdr_ctrl->sdram.w_ghsize.h_y		= IO_SHDR.SDRW_GHSIZE_H.bit.SDRW_GHSIZE_H_1;
	shdr_ctrl->sdram.w_ghsize.i			= IO_SHDR.SDRW_GHSIZE_I.bit.SDRW_GHSIZE_I_0;

	// Video format.
	shdr_ctrl->sdram.w_vfm.a			= IO_SHDR.SDRW_GHSIZE_I.bit.SDRW_VFM_A;
	shdr_ctrl->sdram.w_vfm.b			= IO_SHDR.SDRW_VFM.bit.SDRW_VFM_B;

	// Base address.
	shdr_ctrl->sdram.w_baddr.a_y		= IO_SHDR.SDRW_BA_A.bit.SDRW_BA_A_0;
	shdr_ctrl->sdram.w_baddr.a_c		= IO_SHDR.SDRW_BA_A.bit.SDRW_BA_A_1;
	shdr_ctrl->sdram.w_baddr.c_y		= IO_SHDR.SDRW_BA_C.bit.SDRW_BA_C_0;
	shdr_ctrl->sdram.w_baddr.c_c		= IO_SHDR.SDRW_BA_C.bit.SDRW_BA_C_1;
	shdr_ctrl->sdram.w_baddr.d_y		= IO_SHDR.SDRW_BA_D.bit.SDRW_BA_D_0;
	shdr_ctrl->sdram.w_baddr.d_c		= IO_SHDR.SDRW_BA_D.bit.SDRW_BA_D_1;

	// Horizontal size.
	shdr_ctrl->sdram.w_hsize.a			= IO_SHDR.SDRW_FW_A.bit.SDRW_FW_A;
	shdr_ctrl->sdram.w_hsize.c			= IO_SHDR.SDRW_FW_C.bit.SDRW_FW_C;
	shdr_ctrl->sdram.w_hsize.d			= IO_SHDR.SDRW_FW_D.bit.SDRW_FW_D;

	// Image mode.
	// Exposure magnification.
	shdr_ctrl->image_mode.exp_mag		= IO_SHDR.EXMAG.bit.EXMAG;

	// Reciprocal of the exposure magnification.
	shdr_ctrl->image_mode.rexp_mag		= IO_SHDR.REMVAL.bit.REXMAG;
	im_shdr_get_reg_signed( shdr_ctrl->image_mode.rexp_mag_shift, IO_SHDR.REMVAL, union io_shdr_remval, REMSFT );

	// Shift.
	im_shdr_get_reg_signed( shdr_ctrl->image_mode.shift_x, IO_SHDR.SHIFT, union io_shdr_shift, SHIFTX );
	im_shdr_get_reg_signed( shdr_ctrl->image_mode.shift_y, IO_SHDR.SHIFT, union io_shdr_shift, SHIFTY );

	// Filter margin.
	shdr_ctrl->image_mode.fil_margin	= IO_SHDR.MRGN.bit.MRGN;

	// FMC/Bypass enable. SMC/SHDR input select.
	shdr_ctrl->image_mode.fmd_en		= IO_SHDR.FLWCNT.bit.FMDEN;
	shdr_ctrl->image_mode.bypass_en		= IO_SHDR.FLWCNT.bit.BPEN;
	shdr_ctrl->image_mode.smc_in		= IO_SHDR.FLWCNT.bit.SMCIN;
	shdr_ctrl->image_mode.shdr_in		= IO_SHDR.FLWCNT.bit.BRSEL;

	// Tile size select.
	shdr_ctrl->image_mode.tile_size		= IO_SHDR.TMODE.bit.TMODE;

	// COMPIP.
	shdr_ctrl->compip.mode				= IO_SHDR.MODESEL.bit.MODE;
	shdr_ctrl->compip.mc_mode			= IO_SHDR.MC_MODESEL.bit.MODE;
	shdr_ctrl->compip.comp_incfmt		= IO_SHDR.COMP_INCFMSEL.bit.MODE;
	shdr_ctrl->compip.r_bypass_mode		= IO_SHDR.BYPASSON.bit.RMD;
	shdr_ctrl->compip.w_bypass_mode		= IO_SHDR.BYPASSON.bit.WMD;
	shdr_ctrl->compip.r_img_axi_size	= IO_SHDR.AXISIZE.bit.RYC;
	shdr_ctrl->compip.r_comp_axi_size	= IO_SHDR.AXISIZE.bit.RCI;
	shdr_ctrl->compip.w_img_axi_size	= IO_SHDR.AXISIZE.bit.WR;
	shdr_ctrl->compip.lint_no			= IO_SHDR.LINT.bit.NO;
	shdr_ctrl->compip.lint_mask			= IO_SHDR.LINT.bit.MSK;
	shdr_ctrl->compip.lint_sts			= IO_SHDR.LINT.bit.ST;
	shdr_ctrl->compip.w_addr_y			= IO_SHDR.WRSTARTADDR.bit.ADDR_Y;
	shdr_ctrl->compip.w_addr_c			= IO_SHDR.WRSTARTADDR.bit.ADDR_C;
	shdr_ctrl->compip.r_addr_y			= IO_SHDR.RDSTARTADDR.bit.ADDR_Y;
	shdr_ctrl->compip.r_addr_c			= IO_SHDR.RDSTARTADDR.bit.ADDR_C;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR pre parameter set.<br>
@param[in]		shdr_pre						Setting of SHDR pre parameters. See @ref T_IM_SHDR_PRE.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Set_Pre( const T_IM_SHDR_PRE* const shdr_pre )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_pre == NULL ) {
		Ddim_Assertion(("Im_SHDR_Set_Pre() error. shdr_pre = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_pre->pre_mode.cus_mode > D_IM_SHDR_CUS_YCC420_PADDING ) {
		Ddim_Assertion(("Im_SHDR_Set_Pre() error. cus_mode is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_pre->pre_mode.degamma_mode > D_IM_SHDR_DEGAMMA_BITEXP ) {
		Ddim_Assertion(("Im_SHDR_Set_Pre() error. degamma_mode is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// PREMD.
	IO_SHDR.PREMD.bit.CUSMD	= shdr_pre->pre_mode.cus_mode;
	IO_SHDR.PREMD.bit.DGMD	= shdr_pre->pre_mode.degamma_mode;

	// Y2RCOEF.
	im_shdr_set_reg_signed_a( IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_0, shdr_pre->y2r_coef[ 0 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_1, shdr_pre->y2r_coef[ 1 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_2, shdr_pre->y2r_coef[ 2 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_3, shdr_pre->y2r_coef[ 3 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_4, shdr_pre->y2r_coef[ 4 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_5, shdr_pre->y2r_coef[ 5 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_6, shdr_pre->y2r_coef[ 6 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_7, shdr_pre->y2r_coef[ 7 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_8, shdr_pre->y2r_coef[ 8 ] );

	// PREY2RCLPUL0.
	IO_SHDR.PREY2RCLPUL0.bit.PREY2RCLPUL0_0	= shdr_pre->y2r_clp.upper_long0[ 0 ];
	IO_SHDR.PREY2RCLPUL0.bit.PREY2RCLPUL0_1	= shdr_pre->y2r_clp.upper_long0[ 1 ];
	IO_SHDR.PREY2RCLPUL0.bit.PREY2RCLPUL0_2	= shdr_pre->y2r_clp.upper_long0[ 2 ];

	// PREY2RCLPLL0.
	IO_SHDR.PREY2RCLPLL0.bit.PREY2RCLPLL0_0	= shdr_pre->y2r_clp.lower_long0[ 0 ];
	IO_SHDR.PREY2RCLPLL0.bit.PREY2RCLPLL0_1	= shdr_pre->y2r_clp.lower_long0[ 1 ];
	IO_SHDR.PREY2RCLPLL0.bit.PREY2RCLPLL0_2	= shdr_pre->y2r_clp.lower_long0[ 2 ];

	// PREY2RCLPUS.
	IO_SHDR.PREY2RCLPUS.bit.PREY2RCLPUS_0	= shdr_pre->y2r_clp.upper_short[ 0 ];
	IO_SHDR.PREY2RCLPUS.bit.PREY2RCLPUS_1	= shdr_pre->y2r_clp.upper_short[ 1 ];
	IO_SHDR.PREY2RCLPUS.bit.PREY2RCLPUS_2	= shdr_pre->y2r_clp.upper_short[ 2 ];

	// PREY2RCLPLS.
	IO_SHDR.PREY2RCLPLS.bit.PREY2RCLPLS_0	= shdr_pre->y2r_clp.lower_short[ 0 ];
	IO_SHDR.PREY2RCLPLS.bit.PREY2RCLPLS_1	= shdr_pre->y2r_clp.lower_short[ 1 ];
	IO_SHDR.PREY2RCLPLS.bit.PREY2RCLPLS_2	= shdr_pre->y2r_clp.lower_short[ 2 ];

	// R2YCOEF.
	im_shdr_set_reg_signed_a( IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_0, shdr_pre->r2y_coef[ 0 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_1, shdr_pre->r2y_coef[ 1 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_2, shdr_pre->r2y_coef[ 2 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_3, shdr_pre->r2y_coef[ 3 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_4, shdr_pre->r2y_coef[ 4 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_5, shdr_pre->r2y_coef[ 5 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_6, shdr_pre->r2y_coef[ 6 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_7, shdr_pre->r2y_coef[ 7 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_8, shdr_pre->r2y_coef[ 8 ] );

	// PRER2YCLPUL.
	IO_SHDR.PRER2YCLPUL.bit.PRER2YCLPUL_0	= shdr_pre->r2y_clp.upper_long_y;
	im_shdr_set_reg_signed_a( IO_SHDR.PRER2YCLPUL, union io_shdr_prer2yclpul, PRER2YCLPUL_1, shdr_pre->r2y_clp.upper_long_c[ 0 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.PRER2YCLPUL, union io_shdr_prer2yclpul, PRER2YCLPUL_2, shdr_pre->r2y_clp.upper_long_c[ 1 ] );

	// PRER2YCLPLL.
	IO_SHDR.PRER2YCLPLL.bit.PRER2YCLPLL_0	= shdr_pre->r2y_clp.lower_long_y;
	im_shdr_set_reg_signed_a( IO_SHDR.PRER2YCLPLL, union io_shdr_prer2yclpll, PRER2YCLPLL_1, shdr_pre->r2y_clp.lower_long_c[ 0 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.PRER2YCLPLL, union io_shdr_prer2yclpll, PRER2YCLPLL_2, shdr_pre->r2y_clp.lower_long_c[ 1 ] );

	// PRER2YCLPUS.
	IO_SHDR.PRER2YCLPUS.bit.PRER2YCLPUS_0	= shdr_pre->r2y_clp.upper_short_y;
	im_shdr_set_reg_signed_a( IO_SHDR.PRER2YCLPUS, union io_shdr_prer2yclpus, PRER2YCLPUS_1, shdr_pre->r2y_clp.upper_short_c[ 0 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.PRER2YCLPUS, union io_shdr_prer2yclpus, PRER2YCLPUS_2, shdr_pre->r2y_clp.upper_short_c[ 1 ] );

	// PRER2YCLPLS.
	IO_SHDR.PRER2YCLPLS.bit.PRER2YCLPLS_0	= shdr_pre->r2y_clp.lower_short_y;
	im_shdr_set_reg_signed_a( IO_SHDR.PRER2YCLPLS, union io_shdr_prer2yclpls, PRER2YCLPLS_1, shdr_pre->r2y_clp.lower_short_c[ 0 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.PRER2YCLPLS, union io_shdr_prer2yclpls, PRER2YCLPLS_2, shdr_pre->r2y_clp.lower_short_c[ 1 ] );

	// SKNOFS.
	IO_SHDR.SKNOFS.bit.SKNOFS_0	= shdr_pre->short_knee.offset[ 0 ];
	IO_SHDR.SKNOFS.bit.SKNOFS_1	= shdr_pre->short_knee.offset[ 1 ];
	IO_SHDR.SKNOFS.bit.SKNOFS_2	= shdr_pre->short_knee.offset[ 2 ];

	// SKNSL.
	IO_SHDR.SKNSL.bit.SKNSL_0	= shdr_pre->short_knee.slope[ 0 ];
	IO_SHDR.SKNSL.bit.SKNSL_1	= shdr_pre->short_knee.slope[ 1 ];
	IO_SHDR.SKNSL.bit.SKNSL_2	= shdr_pre->short_knee.slope[ 2 ];

	// SKNB1/2.
	IO_SHDR.SKNB1.bit.SKNB		= shdr_pre->short_knee.boundary[ 0 ];
	IO_SHDR.SKNB2.bit.SKNB		= shdr_pre->short_knee.boundary[ 1 ];

	// LKNOFS.
	IO_SHDR.LKNOFS.bit.LKNOFS_0	= shdr_pre->long_knee.offset[ 0 ];
	IO_SHDR.LKNOFS.bit.LKNOFS_1	= shdr_pre->long_knee.offset[ 1 ];
	IO_SHDR.LKNOFS.bit.LKNOFS_2	= shdr_pre->long_knee.offset[ 2 ];

	// LKNSL.
	IO_SHDR.LKNSL.bit.LKNSL_0	= shdr_pre->long_knee.slope[ 0 ];
	IO_SHDR.LKNSL.bit.LKNSL_1	= shdr_pre->long_knee.slope[ 1 ];
	IO_SHDR.LKNSL.bit.LKNSL_2	= shdr_pre->long_knee.slope[ 2 ];

	// LKNB1/2.
	IO_SHDR.LKNB1.bit.LKNB		= shdr_pre->long_knee.boundary[ 0 ];
	IO_SHDR.LKNB2.bit.LKNB		= shdr_pre->long_knee.boundary[ 1 ];

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR get pre parameter.<br>
@param[out]		shdr_pre						Setting of SHDR pre parameters. See @ref T_IM_SHDR_PRE.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Get_Pre( T_IM_SHDR_PRE* const shdr_pre )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_pre == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_Pre() error. shdr_pre = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// PREMD.
	shdr_pre->pre_mode.cus_mode			= IO_SHDR.PREMD.bit.CUSMD;
	shdr_pre->pre_mode.degamma_mode		= IO_SHDR.PREMD.bit.DGMD;

	// Y2RCOEF.
	im_shdr_get_reg_signed_a( shdr_pre->y2r_coef[ 0 ], IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_0 );
	im_shdr_get_reg_signed_a( shdr_pre->y2r_coef[ 1 ], IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_1 );
	im_shdr_get_reg_signed_a( shdr_pre->y2r_coef[ 2 ], IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_2 );
	im_shdr_get_reg_signed_a( shdr_pre->y2r_coef[ 3 ], IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_3 );
	im_shdr_get_reg_signed_a( shdr_pre->y2r_coef[ 4 ], IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_4 );
	im_shdr_get_reg_signed_a( shdr_pre->y2r_coef[ 5 ], IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_5 );
	im_shdr_get_reg_signed_a( shdr_pre->y2r_coef[ 6 ], IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_6 );
	im_shdr_get_reg_signed_a( shdr_pre->y2r_coef[ 7 ], IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_7 );
	im_shdr_get_reg_signed_a( shdr_pre->y2r_coef[ 8 ], IO_SHDR.Y2RCOEF, union io_shdr_y2rcoef, Y2RCOEF_8 );

	// PREY2RCLPUL0.
	shdr_pre->y2r_clp.upper_long0[ 0 ]	= IO_SHDR.PREY2RCLPUL0.bit.PREY2RCLPUL0_0;
	shdr_pre->y2r_clp.upper_long0[ 1 ]	= IO_SHDR.PREY2RCLPUL0.bit.PREY2RCLPUL0_1;
	shdr_pre->y2r_clp.upper_long0[ 2 ]	= IO_SHDR.PREY2RCLPUL0.bit.PREY2RCLPUL0_2;

	// PREY2RCLPLL0.
	shdr_pre->y2r_clp.lower_long0[ 0 ]	= IO_SHDR.PREY2RCLPLL0.bit.PREY2RCLPLL0_0;
	shdr_pre->y2r_clp.lower_long0[ 1 ]	= IO_SHDR.PREY2RCLPLL0.bit.PREY2RCLPLL0_1;
	shdr_pre->y2r_clp.lower_long0[ 2 ]	= IO_SHDR.PREY2RCLPLL0.bit.PREY2RCLPLL0_2;

	// PREY2RCLPUS.
	shdr_pre->y2r_clp.upper_short[ 0 ]	= IO_SHDR.PREY2RCLPUS.bit.PREY2RCLPUS_0;
	shdr_pre->y2r_clp.upper_short[ 1 ]	= IO_SHDR.PREY2RCLPUS.bit.PREY2RCLPUS_1;
	shdr_pre->y2r_clp.upper_short[ 2 ]	= IO_SHDR.PREY2RCLPUS.bit.PREY2RCLPUS_2;

	// PREY2RCLPLS.
	shdr_pre->y2r_clp.lower_short[ 0 ]	= IO_SHDR.PREY2RCLPLS.bit.PREY2RCLPLS_0;
	shdr_pre->y2r_clp.lower_short[ 1 ]	= IO_SHDR.PREY2RCLPLS.bit.PREY2RCLPLS_1;
	shdr_pre->y2r_clp.lower_short[ 2 ]	= IO_SHDR.PREY2RCLPLS.bit.PREY2RCLPLS_2;

	// R2YCOEF.
	im_shdr_get_reg_signed_a( shdr_pre->r2y_coef[ 0 ], IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_0 );
	im_shdr_get_reg_signed_a( shdr_pre->r2y_coef[ 1 ], IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_1 );
	im_shdr_get_reg_signed_a( shdr_pre->r2y_coef[ 2 ], IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_2 );
	im_shdr_get_reg_signed_a( shdr_pre->r2y_coef[ 3 ], IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_3 );
	im_shdr_get_reg_signed_a( shdr_pre->r2y_coef[ 4 ], IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_4 );
	im_shdr_get_reg_signed_a( shdr_pre->r2y_coef[ 5 ], IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_5 );
	im_shdr_get_reg_signed_a( shdr_pre->r2y_coef[ 6 ], IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_6 );
	im_shdr_get_reg_signed_a( shdr_pre->r2y_coef[ 7 ], IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_7 );
	im_shdr_get_reg_signed_a( shdr_pre->r2y_coef[ 8 ], IO_SHDR.R2YCOEF, union io_shdr_r2ycoef, R2YCOEF_8 );

	// PRER2YCLPUL.
	shdr_pre->r2y_clp.upper_long_y		= IO_SHDR.PRER2YCLPUL.bit.PRER2YCLPUL_0;
	im_shdr_get_reg_signed_a( shdr_pre->r2y_clp.upper_long_c[ 0 ], IO_SHDR.PRER2YCLPUL, union io_shdr_prer2yclpul, PRER2YCLPUL_1 );
	im_shdr_get_reg_signed_a( shdr_pre->r2y_clp.upper_long_c[ 1 ], IO_SHDR.PRER2YCLPUL, union io_shdr_prer2yclpul, PRER2YCLPUL_2 );

	// PRER2YCLPLL.
	shdr_pre->r2y_clp.lower_long_y		= IO_SHDR.PRER2YCLPLL.bit.PRER2YCLPLL_0;
	im_shdr_get_reg_signed_a( shdr_pre->r2y_clp.lower_long_c[ 0 ], IO_SHDR.PRER2YCLPLL, union io_shdr_prer2yclpll, PRER2YCLPLL_1 );
	im_shdr_get_reg_signed_a( shdr_pre->r2y_clp.lower_long_c[ 1 ], IO_SHDR.PRER2YCLPLL, union io_shdr_prer2yclpll, PRER2YCLPLL_2 );

	// PRER2YCLPUS.
	shdr_pre->r2y_clp.upper_short_y		= IO_SHDR.PRER2YCLPUS.bit.PRER2YCLPUS_0;
	im_shdr_get_reg_signed_a( shdr_pre->r2y_clp.upper_short_c[ 0 ], IO_SHDR.PRER2YCLPUS, union io_shdr_prer2yclpus, PRER2YCLPUS_1 );
	im_shdr_get_reg_signed_a( shdr_pre->r2y_clp.upper_short_c[ 1 ], IO_SHDR.PRER2YCLPUS, union io_shdr_prer2yclpus, PRER2YCLPUS_2 );

	// PRER2YCLPLS.
	shdr_pre->r2y_clp.lower_short_y		= IO_SHDR.PRER2YCLPLS.bit.PRER2YCLPLS_0;
	im_shdr_get_reg_signed_a( shdr_pre->r2y_clp.lower_short_c[ 0 ], IO_SHDR.PRER2YCLPLS, union io_shdr_prer2yclpls, PRER2YCLPLS_1 );
	im_shdr_get_reg_signed_a( shdr_pre->r2y_clp.lower_short_c[ 1 ], IO_SHDR.PRER2YCLPLS, union io_shdr_prer2yclpls, PRER2YCLPLS_2 );

	// SKNOFS.
	shdr_pre->short_knee.offset[ 0 ]	= IO_SHDR.SKNOFS.bit.SKNOFS_0;
	shdr_pre->short_knee.offset[ 1 ]	= IO_SHDR.SKNOFS.bit.SKNOFS_1;
	shdr_pre->short_knee.offset[ 2 ]	= IO_SHDR.SKNOFS.bit.SKNOFS_2;

	// SKNSL.
	shdr_pre->short_knee.slope[ 0 ]		= IO_SHDR.SKNSL.bit.SKNSL_0;
	shdr_pre->short_knee.slope[ 1 ]		= IO_SHDR.SKNSL.bit.SKNSL_1;
	shdr_pre->short_knee.slope[ 2 ]		= IO_SHDR.SKNSL.bit.SKNSL_2;

	// SKNB1/2.
	shdr_pre->short_knee.boundary[ 0 ]	= IO_SHDR.SKNB1.bit.SKNB;
	shdr_pre->short_knee.boundary[ 1 ]	= IO_SHDR.SKNB2.bit.SKNB;

	// LKNOFS.
	shdr_pre->long_knee.offset[ 0 ]		= IO_SHDR.LKNOFS.bit.LKNOFS_0;
	shdr_pre->long_knee.offset[ 1 ]		= IO_SHDR.LKNOFS.bit.LKNOFS_1;
	shdr_pre->long_knee.offset[ 2 ]		= IO_SHDR.LKNOFS.bit.LKNOFS_2;

	// LKNSL.
	shdr_pre->long_knee.slope[ 0 ]		= IO_SHDR.LKNSL.bit.LKNSL_0;
	shdr_pre->long_knee.slope[ 1 ]		= IO_SHDR.LKNSL.bit.LKNSL_1;
	shdr_pre->long_knee.slope[ 2 ]		= IO_SHDR.LKNSL.bit.LKNSL_2;

	// LKNB1/2.
	shdr_pre->long_knee.boundary[ 0 ]	= IO_SHDR.LKNB1.bit.LKNB;
	shdr_pre->long_knee.boundary[ 1 ]	= IO_SHDR.LKNB2.bit.LKNB;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR FMD parameter set.<br>
@param[in]		shdr_fmd						Setting of SHDR pre parameters. See @ref T_IM_SHDR_FMD.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Set_Fmd( const T_IM_SHDR_FMD* const shdr_fmd )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_fmd == NULL ) {
		Ddim_Assertion(("Im_SHDR_Set_Fmd() error. shdr_fmd = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_fmd->weight_base > D_IM_SHDR_WGHTBASE_RGB ) {
		Ddim_Assertion(("Im_SHDR_Set_Fmd() error. weight_base is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_fmd->exp77_mode > D_IM_SHDR_EXP77MD_PERIMAXVAL ) {
		Ddim_Assertion(("Im_SHDR_Set_Fmd() error. exp77_mode is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_fmd->fmd_sel > D_IM_SHDR_FMDSEL_REDUCTION ) {
		Ddim_Assertion(("Im_SHDR_Set_Fmd() error. fmd_sel is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// EPBD.
	IO_SHDR.EPBD.bit.EPBD_1			= shdr_fmd->ep_boundary[ 1 ];
	IO_SHDR.EPBD.bit.EPBD_2			= shdr_fmd->ep_boundary[ 2 ];
	IO_SHDR.EPBD.bit.EPBD_3			= shdr_fmd->ep_boundary[ 3 ];

	// EPOF.
	IO_SHDR.EPOF.bit.EPOF_0			= shdr_fmd->ep_offset[ 0 ];
	IO_SHDR.EPOF.bit.EPOF_1			= shdr_fmd->ep_offset[ 1 ];
	IO_SHDR.EPOF.bit.EPOF_2			= shdr_fmd->ep_offset[ 2 ];
	IO_SHDR.EPOF.bit.EPOF_3			= shdr_fmd->ep_offset[ 3 ];

	// EPBLD.
	IO_SHDR.EPBLD.bit.EPBLD			= shdr_fmd->ep_blend;

	// FDWEIGHT.
	IO_SHDR.FDWEIGHT.bit.FDWEIGHT	= shdr_fmd->fd_weight;

	// MVCNT.
	IO_SHDR.MVCNT.bit.MVBD			= shdr_fmd->mvcnt.bounds;
	IO_SHDR.MVCNT.bit.MVTH			= shdr_fmd->mvcnt.threshold;
	im_shdr_set_reg_signed( IO_SHDR.MVCNT, union io_shdr_mvcnt, MVGR, shdr_fmd->mvcnt.gr );

	// BLDROFS.
	IO_SHDR.BLDROFS.bit.BLDROFS_0	= shdr_fmd->bldr_offset[ 0 ];
	IO_SHDR.BLDROFS.bit.BLDROFS_1	= shdr_fmd->bldr_offset[ 1 ];
	IO_SHDR.BLDROFS.bit.BLDROFS_2	= shdr_fmd->bldr_offset[ 2 ];
	IO_SHDR.BLDROFS.bit.BLDROFS_3	= shdr_fmd->bldr_offset[ 3 ];
	IO_SHDR.BLDROFS.bit.BLDROFS_4	= shdr_fmd->bldr_offset[ 4 ];

	// BLDRSL.
	IO_SHDR.BLDRSL.bit.BLDRSL_0		= shdr_fmd->bldr_slope[ 0 ];
	IO_SHDR.BLDRSL.bit.BLDRSL_1		= shdr_fmd->bldr_slope[ 1 ];
	IO_SHDR.BLDRSL.bit.BLDRSL_2		= shdr_fmd->bldr_slope[ 2 ];
	IO_SHDR.BLDRSL.bit.BLDRSL_3		= shdr_fmd->bldr_slope[ 3 ];
	IO_SHDR.BLDRSL.bit.BLDRSL_4		= shdr_fmd->bldr_slope[ 4 ];

	// BLDRB.
	IO_SHDR.BLDRB.bit.BLDRB_1		= shdr_fmd->bldr_boundary[ 1 ];
	IO_SHDR.BLDRB.bit.BLDRB_2		= shdr_fmd->bldr_boundary[ 2 ];
	IO_SHDR.BLDRB.bit.BLDRB_3		= shdr_fmd->bldr_boundary[ 3 ];
	IO_SHDR.BLDRB.bit.BLDRB_4		= shdr_fmd->bldr_boundary[ 4 ];

	// WGHTR2YCOEF.
	im_shdr_set_reg_signed_a( IO_SHDR.WGHTR2YCOEF, union io_shdr_wghtr2ycoef, WGHTR2YCOEF_0, shdr_fmd->weight_r2y_coef[ 0 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.WGHTR2YCOEF, union io_shdr_wghtr2ycoef, WGHTR2YCOEF_1, shdr_fmd->weight_r2y_coef[ 1 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.WGHTR2YCOEF, union io_shdr_wghtr2ycoef, WGHTR2YCOEF_2, shdr_fmd->weight_r2y_coef[ 2 ] );

	// WGHTR2YCLP.
	IO_SHDR.WGHTR2YCLP.bit.WGHTR2YCLPU	= shdr_fmd->weight_r2y_clp.upper;
	IO_SHDR.WGHTR2YCLP.bit.WGHTR2YCLPL	= shdr_fmd->weight_r2y_clp.lower;

	// WGHTBASE.
	IO_SHDR.WGHTBASE.bit.WGHTBASE		= shdr_fmd->weight_base;

	// WGHTROFS.
	IO_SHDR.WGHTROFS.bit.WGHTROFS_0		= shdr_fmd->weight_offset[ 0 ][ 0 ];
	IO_SHDR.WGHTROFS.bit.WGHTROFS_1		= shdr_fmd->weight_offset[ 0 ][ 1 ];
	IO_SHDR.WGHTROFS.bit.WGHTROFS_2		= shdr_fmd->weight_offset[ 0 ][ 2 ];
	IO_SHDR.WGHTROFS.bit.WGHTROFS_3		= shdr_fmd->weight_offset[ 0 ][ 3 ];
	IO_SHDR.WGHTROFS.bit.WGHTROFS_4		= shdr_fmd->weight_offset[ 0 ][ 4 ];

	// WGHTGOFS.
	IO_SHDR.WGHTGOFS.bit.WGHTGOFS_0		= shdr_fmd->weight_offset[ 1 ][ 0 ];
	IO_SHDR.WGHTGOFS.bit.WGHTGOFS_1		= shdr_fmd->weight_offset[ 1 ][ 1 ];
	IO_SHDR.WGHTGOFS.bit.WGHTGOFS_2		= shdr_fmd->weight_offset[ 1 ][ 2 ];
	IO_SHDR.WGHTGOFS.bit.WGHTGOFS_3		= shdr_fmd->weight_offset[ 1 ][ 3 ];
	IO_SHDR.WGHTGOFS.bit.WGHTGOFS_4		= shdr_fmd->weight_offset[ 1 ][ 4 ];

	// WGHTBOFS.
	IO_SHDR.WGHTBOFS.bit.WGHTBOFS_0		= shdr_fmd->weight_offset[ 2 ][ 0 ];
	IO_SHDR.WGHTBOFS.bit.WGHTBOFS_1		= shdr_fmd->weight_offset[ 2 ][ 1 ];
	IO_SHDR.WGHTBOFS.bit.WGHTBOFS_2		= shdr_fmd->weight_offset[ 2 ][ 2 ];
	IO_SHDR.WGHTBOFS.bit.WGHTBOFS_3		= shdr_fmd->weight_offset[ 2 ][ 3 ];
	IO_SHDR.WGHTBOFS.bit.WGHTBOFS_4		= shdr_fmd->weight_offset[ 2 ][ 4 ];

	// WGHTRSL.
	IO_SHDR.WGHTRSL.bit.WGHTRSL_0		= shdr_fmd->weight_slope[ 0 ][ 0 ];
	IO_SHDR.WGHTRSL.bit.WGHTRSL_1		= shdr_fmd->weight_slope[ 0 ][ 1 ];
	IO_SHDR.WGHTRSL.bit.WGHTRSL_2		= shdr_fmd->weight_slope[ 0 ][ 2 ];
	IO_SHDR.WGHTRSL.bit.WGHTRSL_3		= shdr_fmd->weight_slope[ 0 ][ 3 ];
	IO_SHDR.WGHTRSL.bit.WGHTRSL_4		= shdr_fmd->weight_slope[ 0 ][ 4 ];

	// WGHTGSL.
	IO_SHDR.WGHTGSL.bit.WGHTGSL_0		= shdr_fmd->weight_slope[ 1 ][ 0 ];
	IO_SHDR.WGHTGSL.bit.WGHTGSL_1		= shdr_fmd->weight_slope[ 1 ][ 1 ];
	IO_SHDR.WGHTGSL.bit.WGHTGSL_2		= shdr_fmd->weight_slope[ 1 ][ 2 ];
	IO_SHDR.WGHTGSL.bit.WGHTGSL_3		= shdr_fmd->weight_slope[ 1 ][ 3 ];
	IO_SHDR.WGHTGSL.bit.WGHTGSL_4		= shdr_fmd->weight_slope[ 1 ][ 4 ];

	// WGHTBSL.
	IO_SHDR.WGHTBSL.bit.WGHTBSL_0		= shdr_fmd->weight_slope[ 2 ][ 0 ];
	IO_SHDR.WGHTBSL.bit.WGHTBSL_1		= shdr_fmd->weight_slope[ 2 ][ 1 ];
	IO_SHDR.WGHTBSL.bit.WGHTBSL_2		= shdr_fmd->weight_slope[ 2 ][ 2 ];
	IO_SHDR.WGHTBSL.bit.WGHTBSL_3		= shdr_fmd->weight_slope[ 2 ][ 3 ];
	IO_SHDR.WGHTBSL.bit.WGHTBSL_4		= shdr_fmd->weight_slope[ 2 ][ 4 ];

	// WGHTRB.
	IO_SHDR.WGHTRB.bit.WGHTRB_1			= shdr_fmd->weight_boundary[ 0 ][ 1 ];
	IO_SHDR.WGHTRB.bit.WGHTRB_2			= shdr_fmd->weight_boundary[ 0 ][ 2 ];
	IO_SHDR.WGHTRB.bit.WGHTRB_3			= shdr_fmd->weight_boundary[ 0 ][ 3 ];
	IO_SHDR.WGHTRB.bit.WGHTRB_4			= shdr_fmd->weight_boundary[ 0 ][ 4 ];

	// WGHTGB.
	IO_SHDR.WGHTGB.bit.WGHTGB_1			= shdr_fmd->weight_boundary[ 1 ][ 1 ];
	IO_SHDR.WGHTGB.bit.WGHTGB_2			= shdr_fmd->weight_boundary[ 1 ][ 2 ];
	IO_SHDR.WGHTGB.bit.WGHTGB_3			= shdr_fmd->weight_boundary[ 1 ][ 3 ];
	IO_SHDR.WGHTGB.bit.WGHTGB_4			= shdr_fmd->weight_boundary[ 1 ][ 4 ];

	// WGHTBB.
	IO_SHDR.WGHTBB.bit.WGHTBB_1			= shdr_fmd->weight_boundary[ 2 ][ 1 ];
	IO_SHDR.WGHTBB.bit.WGHTBB_2			= shdr_fmd->weight_boundary[ 2 ][ 2 ];
	IO_SHDR.WGHTBB.bit.WGHTBB_3			= shdr_fmd->weight_boundary[ 2 ][ 3 ];
	IO_SHDR.WGHTBB.bit.WGHTBB_4			= shdr_fmd->weight_boundary[ 2 ][ 4 ];

	// ISOTH.
	IO_SHDR.ISOTH.bit.ISOTH2			= shdr_fmd->iso_threshold;

	// ISOCOEF.
	IO_SHDR.ISOCOEF.bit.ISO2K0			= shdr_fmd->iso_coef[ 0 ];
	IO_SHDR.ISOCOEF.bit.ISO2K1			= shdr_fmd->iso_coef[ 1 ];
	IO_SHDR.ISOCOEF.bit.ISO2K2			= shdr_fmd->iso_coef[ 2 ];
	IO_SHDR.ISOCOEF.bit.ISO2K3			= shdr_fmd->iso_coef[ 3 ];
	IO_SHDR.ISOCOEF.bit.ISO2K4			= shdr_fmd->iso_coef[ 4 ];
	IO_SHDR.ISOCOEF.bit.ISO2K5			= shdr_fmd->iso_coef[ 5 ];

	// SATTH.
	IO_SHDR.SATTH.bit.SATTH				= shdr_fmd->sat_threshold;

	// EXP77MD.
	IO_SHDR.EXP77MD.bit.EXP77MD			= shdr_fmd->exp77_mode;

	// EXP77MD.
	IO_SHDR.FMDSEL.bit.FMDSEL			= shdr_fmd->fmd_sel;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR get FMD parameter.<br>
@param[out]		shdr_fmd						Setting of SHDR pre parameters. See @ref T_IM_SHDR_FMD.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Get_Fmd( T_IM_SHDR_FMD* const shdr_fmd )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_fmd == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_Fmd() error. shdr_fmd = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// EPBD.
	shdr_fmd->ep_boundary[ 1 ]		= IO_SHDR.EPBD.bit.EPBD_1;
	shdr_fmd->ep_boundary[ 2 ]		= IO_SHDR.EPBD.bit.EPBD_2;
	shdr_fmd->ep_boundary[ 3 ]		= IO_SHDR.EPBD.bit.EPBD_3;

	// EPOF.
	shdr_fmd->ep_offset[ 0 ]		= IO_SHDR.EPOF.bit.EPOF_0;
	shdr_fmd->ep_offset[ 1 ]		= IO_SHDR.EPOF.bit.EPOF_1;
	shdr_fmd->ep_offset[ 2 ]		= IO_SHDR.EPOF.bit.EPOF_2;
	shdr_fmd->ep_offset[ 3 ]		= IO_SHDR.EPOF.bit.EPOF_3;

	// EPBLD.
	shdr_fmd->ep_blend				= IO_SHDR.EPBLD.bit.EPBLD;

	// FDWEIGHT.
	shdr_fmd->fd_weight				= IO_SHDR.FDWEIGHT.bit.FDWEIGHT;

	// MVCNT.
	shdr_fmd->mvcnt.bounds			= IO_SHDR.MVCNT.bit.MVBD;
	shdr_fmd->mvcnt.threshold		= IO_SHDR.MVCNT.bit.MVTH;
	im_shdr_get_reg_signed( shdr_fmd->mvcnt.gr, IO_SHDR.MVCNT, union io_shdr_mvcnt, MVGR );

	// BLDROFS.
	shdr_fmd->bldr_offset[ 0 ]		= IO_SHDR.BLDROFS.bit.BLDROFS_0;
	shdr_fmd->bldr_offset[ 1 ]		= IO_SHDR.BLDROFS.bit.BLDROFS_1;
	shdr_fmd->bldr_offset[ 2 ]		= IO_SHDR.BLDROFS.bit.BLDROFS_2;
	shdr_fmd->bldr_offset[ 3 ]		= IO_SHDR.BLDROFS.bit.BLDROFS_3;
	shdr_fmd->bldr_offset[ 4 ]		= IO_SHDR.BLDROFS.bit.BLDROFS_4;

	// BLDRSL.
	shdr_fmd->bldr_slope[ 0 ]		= IO_SHDR.BLDRSL.bit.BLDRSL_0;
	shdr_fmd->bldr_slope[ 1 ]		= IO_SHDR.BLDRSL.bit.BLDRSL_1;
	shdr_fmd->bldr_slope[ 2 ]		= IO_SHDR.BLDRSL.bit.BLDRSL_2;
	shdr_fmd->bldr_slope[ 3 ]		= IO_SHDR.BLDRSL.bit.BLDRSL_3;
	shdr_fmd->bldr_slope[ 4 ]		= IO_SHDR.BLDRSL.bit.BLDRSL_4;

	// BLDRB.
	shdr_fmd->bldr_boundary[ 1 ]	= IO_SHDR.BLDRB.bit.BLDRB_1;
	shdr_fmd->bldr_boundary[ 2 ]	= IO_SHDR.BLDRB.bit.BLDRB_2;
	shdr_fmd->bldr_boundary[ 3 ]	= IO_SHDR.BLDRB.bit.BLDRB_3;
	shdr_fmd->bldr_boundary[ 4 ]	= IO_SHDR.BLDRB.bit.BLDRB_4;

	// WGHTR2YCOEF.
	im_shdr_get_reg_signed_a( shdr_fmd->weight_r2y_coef[ 0 ], IO_SHDR.WGHTR2YCOEF, union io_shdr_wghtr2ycoef, WGHTR2YCOEF_0 );
	im_shdr_get_reg_signed_a( shdr_fmd->weight_r2y_coef[ 1 ], IO_SHDR.WGHTR2YCOEF, union io_shdr_wghtr2ycoef, WGHTR2YCOEF_1 );
	im_shdr_get_reg_signed_a( shdr_fmd->weight_r2y_coef[ 2 ], IO_SHDR.WGHTR2YCOEF, union io_shdr_wghtr2ycoef, WGHTR2YCOEF_2 );

	// WGHTR2YCLP.
	shdr_fmd->weight_r2y_clp.upper	= IO_SHDR.WGHTR2YCLP.bit.WGHTR2YCLPU;
	shdr_fmd->weight_r2y_clp.lower	= IO_SHDR.WGHTR2YCLP.bit.WGHTR2YCLPL;

	// WGHTBASE.
	shdr_fmd->weight_base			= IO_SHDR.WGHTBASE.bit.WGHTBASE;

	// WGHTROFS.
	shdr_fmd->weight_offset[ 0 ][ 0 ]	= IO_SHDR.WGHTROFS.bit.WGHTROFS_0;
	shdr_fmd->weight_offset[ 0 ][ 1 ]	= IO_SHDR.WGHTROFS.bit.WGHTROFS_1;
	shdr_fmd->weight_offset[ 0 ][ 2 ]	= IO_SHDR.WGHTROFS.bit.WGHTROFS_2;
	shdr_fmd->weight_offset[ 0 ][ 3 ]	= IO_SHDR.WGHTROFS.bit.WGHTROFS_3;
	shdr_fmd->weight_offset[ 0 ][ 4 ]	= IO_SHDR.WGHTROFS.bit.WGHTROFS_4;

	// WGHTGOFS.
	shdr_fmd->weight_offset[ 1 ][ 0 ]	= IO_SHDR.WGHTGOFS.bit.WGHTGOFS_0;
	shdr_fmd->weight_offset[ 1 ][ 1 ]	= IO_SHDR.WGHTGOFS.bit.WGHTGOFS_1;
	shdr_fmd->weight_offset[ 1 ][ 2 ]	= IO_SHDR.WGHTGOFS.bit.WGHTGOFS_2;
	shdr_fmd->weight_offset[ 1 ][ 3 ]	= IO_SHDR.WGHTGOFS.bit.WGHTGOFS_3;
	shdr_fmd->weight_offset[ 1 ][ 4 ]	= IO_SHDR.WGHTGOFS.bit.WGHTGOFS_4;

	// WGHTBOFS.
	shdr_fmd->weight_offset[ 2 ][ 0 ]	= IO_SHDR.WGHTBOFS.bit.WGHTBOFS_0;
	shdr_fmd->weight_offset[ 2 ][ 1 ]	= IO_SHDR.WGHTBOFS.bit.WGHTBOFS_1;
	shdr_fmd->weight_offset[ 2 ][ 2 ]	= IO_SHDR.WGHTBOFS.bit.WGHTBOFS_2;
	shdr_fmd->weight_offset[ 2 ][ 3 ]	= IO_SHDR.WGHTBOFS.bit.WGHTBOFS_3;
	shdr_fmd->weight_offset[ 2 ][ 4 ]	= IO_SHDR.WGHTBOFS.bit.WGHTBOFS_4;

	// WGHTRSL.
	shdr_fmd->weight_slope[ 0 ][ 0 ]	= IO_SHDR.WGHTRSL.bit.WGHTRSL_0;
	shdr_fmd->weight_slope[ 0 ][ 1 ]	= IO_SHDR.WGHTRSL.bit.WGHTRSL_1;
	shdr_fmd->weight_slope[ 0 ][ 2 ]	= IO_SHDR.WGHTRSL.bit.WGHTRSL_2;
	shdr_fmd->weight_slope[ 0 ][ 3 ]	= IO_SHDR.WGHTRSL.bit.WGHTRSL_3;
	shdr_fmd->weight_slope[ 0 ][ 4 ]	= IO_SHDR.WGHTRSL.bit.WGHTRSL_4;

	// WGHTGSL.
	shdr_fmd->weight_slope[ 1 ][ 0 ]	= IO_SHDR.WGHTGSL.bit.WGHTGSL_0;
	shdr_fmd->weight_slope[ 1 ][ 1 ]	= IO_SHDR.WGHTGSL.bit.WGHTGSL_1;
	shdr_fmd->weight_slope[ 1 ][ 2 ]	= IO_SHDR.WGHTGSL.bit.WGHTGSL_2;
	shdr_fmd->weight_slope[ 1 ][ 3 ]	= IO_SHDR.WGHTGSL.bit.WGHTGSL_3;
	shdr_fmd->weight_slope[ 1 ][ 4 ]	= IO_SHDR.WGHTGSL.bit.WGHTGSL_4;

	// WGHTBSL.
	shdr_fmd->weight_slope[ 2 ][ 0 ]	= IO_SHDR.WGHTBSL.bit.WGHTBSL_0;
	shdr_fmd->weight_slope[ 2 ][ 1 ]	= IO_SHDR.WGHTBSL.bit.WGHTBSL_1;
	shdr_fmd->weight_slope[ 2 ][ 2 ]	= IO_SHDR.WGHTBSL.bit.WGHTBSL_2;
	shdr_fmd->weight_slope[ 2 ][ 3 ]	= IO_SHDR.WGHTBSL.bit.WGHTBSL_3;
	shdr_fmd->weight_slope[ 2 ][ 4 ]	= IO_SHDR.WGHTBSL.bit.WGHTBSL_4;

	// WGHTRB.
	shdr_fmd->weight_boundary[ 0 ][ 1 ]	= IO_SHDR.WGHTRB.bit.WGHTRB_1;
	shdr_fmd->weight_boundary[ 0 ][ 2 ]	= IO_SHDR.WGHTRB.bit.WGHTRB_2;
	shdr_fmd->weight_boundary[ 0 ][ 3 ]	= IO_SHDR.WGHTRB.bit.WGHTRB_3;
	shdr_fmd->weight_boundary[ 0 ][ 4 ]	= IO_SHDR.WGHTRB.bit.WGHTRB_4;

	// WGHTGB.
	shdr_fmd->weight_boundary[ 1 ][ 1 ]	= IO_SHDR.WGHTGB.bit.WGHTGB_1;
	shdr_fmd->weight_boundary[ 1 ][ 2 ]	= IO_SHDR.WGHTGB.bit.WGHTGB_2;
	shdr_fmd->weight_boundary[ 1 ][ 3 ]	= IO_SHDR.WGHTGB.bit.WGHTGB_3;
	shdr_fmd->weight_boundary[ 1 ][ 4 ]	= IO_SHDR.WGHTGB.bit.WGHTGB_4;

	// WGHTBB.
	shdr_fmd->weight_boundary[ 2 ][ 1 ]	= IO_SHDR.WGHTBB.bit.WGHTBB_1;
	shdr_fmd->weight_boundary[ 2 ][ 2 ]	= IO_SHDR.WGHTBB.bit.WGHTBB_2;
	shdr_fmd->weight_boundary[ 2 ][ 3 ]	= IO_SHDR.WGHTBB.bit.WGHTBB_3;
	shdr_fmd->weight_boundary[ 2 ][ 4 ]	= IO_SHDR.WGHTBB.bit.WGHTBB_4;

	// ISOTH.
	shdr_fmd->iso_threshold				= IO_SHDR.ISOTH.bit.ISOTH2;

	// ISOCOEF.
	shdr_fmd->iso_coef[ 0 ]				= IO_SHDR.ISOCOEF.bit.ISO2K0;
	shdr_fmd->iso_coef[ 1 ]				= IO_SHDR.ISOCOEF.bit.ISO2K1;
	shdr_fmd->iso_coef[ 2 ]				= IO_SHDR.ISOCOEF.bit.ISO2K2;
	shdr_fmd->iso_coef[ 3 ]				= IO_SHDR.ISOCOEF.bit.ISO2K3;
	shdr_fmd->iso_coef[ 4 ]				= IO_SHDR.ISOCOEF.bit.ISO2K4;
	shdr_fmd->iso_coef[ 5 ]				= IO_SHDR.ISOCOEF.bit.ISO2K5;

	// SATTH.
	shdr_fmd->sat_threshold				= IO_SHDR.SATTH.bit.SATTH;

	// EXP77MD.
	shdr_fmd->exp77_mode				= IO_SHDR.EXP77MD.bit.EXP77MD;

	// EXP77MD.
	shdr_fmd->fmd_sel					= IO_SHDR.FMDSEL.bit.FMDSEL;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR PMSK parameter set.<br>
@param[in]		shdr_pmsk						Setting of SHDR PMSK parameters. See @ref T_IM_SHDR_PMSK.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Set_PreMask( const T_IM_SHDR_PMSK* const shdr_pmsk )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_pmsk == NULL ) {
		Ddim_Assertion(("Im_SHDR_Set_PreMask() error. shdr_pmsk = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_pmsk->pmsk_mode > D_IM_SHDR_PMSKMD_AVE ) {
		Ddim_Assertion(("Im_SHDR_Set_PreMask() error. pmsk_mode is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// TLUT.
	IO_SHDR.TLUT.bit.TLUT_0		= shdr_pmsk->tlut[ 0 ];
	IO_SHDR.TLUT.bit.TLUT_1		= shdr_pmsk->tlut[ 1 ];
	IO_SHDR.TLUT.bit.TLUT_2		= shdr_pmsk->tlut[ 2 ];
	IO_SHDR.TLUT.bit.TLUT_3		= shdr_pmsk->tlut[ 3 ];
	IO_SHDR.TLUT.bit.TLUT_4		= shdr_pmsk->tlut[ 4 ];
	IO_SHDR.TLUT.bit.TLUT_5		= shdr_pmsk->tlut[ 5 ];
	IO_SHDR.TLUT.bit.TLUT_6		= shdr_pmsk->tlut[ 6 ];
	IO_SHDR.TLUT.bit.TLUT_7		= shdr_pmsk->tlut[ 7 ];
	IO_SHDR.TLUT.bit.TLUT_8		= shdr_pmsk->tlut[ 8 ];
	IO_SHDR.TLUT.bit.TLUT_9		= shdr_pmsk->tlut[ 9 ];
	IO_SHDR.TLUT.bit.TLUT_10	= shdr_pmsk->tlut[ 10 ];
	IO_SHDR.TLUT.bit.TLUT_11	= shdr_pmsk->tlut[ 11 ];
	IO_SHDR.TLUT.bit.TLUT_12	= shdr_pmsk->tlut[ 12 ];
	IO_SHDR.TLUT.bit.TLUT_13	= shdr_pmsk->tlut[ 13 ];
	IO_SHDR.TLUT.bit.TLUT_14	= shdr_pmsk->tlut[ 14 ];
	IO_SHDR.TLUT.bit.TLUT_15	= shdr_pmsk->tlut[ 15 ];
	IO_SHDR.TLUT.bit.TLUT_16	= shdr_pmsk->tlut[ 16 ];

	// PMSKMD.
	IO_SHDR.PMSKMD.bit.PMSKMD	= shdr_pmsk->pmsk_mode;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR get PMSK parameter.<br>
@param[out]		shdr_pmsk						Setting of SHDR PMSK parameters. See @ref T_IM_SHDR_PMSK.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Get_PreMask( T_IM_SHDR_PMSK* const shdr_pmsk )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_pmsk == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_PreMask() error. shdr_pmsk = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// TLUT.
	shdr_pmsk->tlut[ 0 ]	= IO_SHDR.TLUT.bit.TLUT_0;
	shdr_pmsk->tlut[ 1 ]	= IO_SHDR.TLUT.bit.TLUT_1;
	shdr_pmsk->tlut[ 2 ]	= IO_SHDR.TLUT.bit.TLUT_2;
	shdr_pmsk->tlut[ 3 ]	= IO_SHDR.TLUT.bit.TLUT_3;
	shdr_pmsk->tlut[ 4 ]	= IO_SHDR.TLUT.bit.TLUT_4;
	shdr_pmsk->tlut[ 5 ]	= IO_SHDR.TLUT.bit.TLUT_5;
	shdr_pmsk->tlut[ 6 ]	= IO_SHDR.TLUT.bit.TLUT_6;
	shdr_pmsk->tlut[ 7 ]	= IO_SHDR.TLUT.bit.TLUT_7;
	shdr_pmsk->tlut[ 8 ]	= IO_SHDR.TLUT.bit.TLUT_8;
	shdr_pmsk->tlut[ 9 ]	= IO_SHDR.TLUT.bit.TLUT_9;
	shdr_pmsk->tlut[ 10 ]	= IO_SHDR.TLUT.bit.TLUT_10;
	shdr_pmsk->tlut[ 11 ]	= IO_SHDR.TLUT.bit.TLUT_11;
	shdr_pmsk->tlut[ 12 ]	= IO_SHDR.TLUT.bit.TLUT_12;
	shdr_pmsk->tlut[ 13 ]	= IO_SHDR.TLUT.bit.TLUT_13;
	shdr_pmsk->tlut[ 14 ]	= IO_SHDR.TLUT.bit.TLUT_14;
	shdr_pmsk->tlut[ 15 ]	= IO_SHDR.TLUT.bit.TLUT_15;
	shdr_pmsk->tlut[ 16 ]	= IO_SHDR.TLUT.bit.TLUT_16;

	// PMSKMD.
	shdr_pmsk->pmsk_mode	= IO_SHDR.PMSKMD.bit.PMSKMD;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR SPNR parameter set.<br>
@param[in]		shdr_spnr						Setting of SHDR SPNR parameters. See @ref T_IM_SHDR_SPNR.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Set_Spnr( const T_IM_SHDR_SPNR* const shdr_spnr )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_spnr == NULL ) {
		Ddim_Assertion(("Im_SHDR_Set_Spnr() error. shdr_spnr = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_spnr->spnr_en > D_IM_SHDR_ENABLE ) {
		Ddim_Assertion(("Im_SHDR_Set_Spnr() error. spnr_en is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// SPNR.
	IO_SHDR.SPNREN.bit.SPNREN	= shdr_spnr->spnr_en;

	// SPEBD.
	IO_SHDR.SPEBD.bit.SPEBD_1	= shdr_spnr->spe_boundary[ 1 ];
	IO_SHDR.SPEBD.bit.SPEBD_2	= shdr_spnr->spe_boundary[ 2 ];
	IO_SHDR.SPEBD.bit.SPEBD_3	= shdr_spnr->spe_boundary[ 3 ];

	// SPEOF.
	IO_SHDR.SPEOF.bit.SPEOF_0	= shdr_spnr->spe_offset[ 0 ];
	IO_SHDR.SPEOF.bit.SPEOF_1	= shdr_spnr->spe_offset[ 1 ];
	IO_SHDR.SPEOF.bit.SPEOF_2	= shdr_spnr->spe_offset[ 2 ];
	IO_SHDR.SPEOF.bit.SPEOF_3	= shdr_spnr->spe_offset[ 3 ];

	// SPEBLD.
	IO_SHDR.SPEBLD.bit.SPEBLD	= shdr_spnr->spe_blend;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR get SPNR parameter.<br>
@param[out]		shdr_spnr						Setting of SHDR SPNR parameters. See @ref T_IM_SHDR_SPNR.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Get_Spnr( T_IM_SHDR_SPNR* const shdr_spnr )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_spnr == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_Spnr() error. shdr_spnr = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// SPNR.
	shdr_spnr->spnr_en				= IO_SHDR.SPNREN.bit.SPNREN;

	// SPEBD.
	shdr_spnr->spe_boundary[ 1 ]	= IO_SHDR.SPEBD.bit.SPEBD_1;
	shdr_spnr->spe_boundary[ 2 ]	= IO_SHDR.SPEBD.bit.SPEBD_2;
	shdr_spnr->spe_boundary[ 3 ]	= IO_SHDR.SPEBD.bit.SPEBD_3;

	// SPEOF.
	shdr_spnr->spe_offset[ 0 ]		= IO_SHDR.SPEOF.bit.SPEOF_0;
	shdr_spnr->spe_offset[ 1 ]		= IO_SHDR.SPEOF.bit.SPEOF_1;
	shdr_spnr->spe_offset[ 2 ]		= IO_SHDR.SPEOF.bit.SPEOF_2;
	shdr_spnr->spe_offset[ 3 ]		= IO_SHDR.SPEOF.bit.SPEOF_3;

	// SPEBLD.
	shdr_spnr->spe_blend			= IO_SHDR.SPEBLD.bit.SPEBLD;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR MSK parameter set.<br>
@param[in]		shdr_msk						Setting of SHDR MSK parameters. See @ref T_IM_SHDR_MSK.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Set_Mask( const T_IM_SHDR_MSK* const shdr_msk )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_msk == NULL ) {
		Ddim_Assertion(("Im_SHDR_Set_Mask() error. shdr_msk = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_msk->msk_mode > D_IM_SHDR_MSKMD_MSKONLY ) {
		Ddim_Assertion(("Im_SHDR_Set_Mask() error. msk_mode is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// MSKY2RCLPUL.
	IO_SHDR.MSKY2RCLPUL.bit.MSKY2RCLPUL_0	= shdr_msk->y2r_clp_upper_long[ 0 ];
	IO_SHDR.MSKY2RCLPUL.bit.MSKY2RCLPUL_1	= shdr_msk->y2r_clp_upper_long[ 1 ];
	IO_SHDR.MSKY2RCLPUL.bit.MSKY2RCLPUL_2	= shdr_msk->y2r_clp_upper_long[ 2 ];

	// MSKY2RCLPLL.
	IO_SHDR.MSKY2RCLPLL.bit.MSKY2RCLPLL_0	= shdr_msk->y2r_clp_lower_long[ 0 ];
	IO_SHDR.MSKY2RCLPLL.bit.MSKY2RCLPLL_1	= shdr_msk->y2r_clp_lower_long[ 1 ];
	IO_SHDR.MSKY2RCLPLL.bit.MSKY2RCLPLL_2	= shdr_msk->y2r_clp_lower_long[ 2 ];

	// MSKY2RCLPUS.
	IO_SHDR.MSKY2RCLPUS.bit.MSKY2RCLPUS_0	= shdr_msk->y2r_clp_upper_short[ 0 ];
	IO_SHDR.MSKY2RCLPUS.bit.MSKY2RCLPUS_1	= shdr_msk->y2r_clp_upper_short[ 1 ];
	IO_SHDR.MSKY2RCLPUS.bit.MSKY2RCLPUS_2	= shdr_msk->y2r_clp_upper_short[ 2 ];

	// MSKY2RCLPLS.
	IO_SHDR.MSKY2RCLPLS.bit.MSKY2RCLPLS_0	= shdr_msk->y2r_clp_lower_short[ 0 ];
	IO_SHDR.MSKY2RCLPLS.bit.MSKY2RCLPLS_1	= shdr_msk->y2r_clp_lower_short[ 1 ];
	IO_SHDR.MSKY2RCLPLS.bit.MSKY2RCLPLS_2	= shdr_msk->y2r_clp_lower_short[ 2 ];

	// ALPHA.
	IO_SHDR.ALPHA.bit.ALPHA					= shdr_msk->alpha;

	// LPFC.
	IO_SHDR.LPFC.bit.LPFC_0_0				= shdr_msk->lpfc[ 0 ][ 0 ];
	IO_SHDR.LPFC.bit.LPFC_0_1				= shdr_msk->lpfc[ 0 ][ 1 ];
	IO_SHDR.LPFC.bit.LPFC_0_2				= shdr_msk->lpfc[ 0 ][ 2 ];
	IO_SHDR.LPFC.bit.LPFC_1_0				= shdr_msk->lpfc[ 1 ][ 0 ];
	IO_SHDR.LPFC.bit.LPFC_1_1				= shdr_msk->lpfc[ 1 ][ 1 ];
	IO_SHDR.LPFC.bit.LPFC_1_2				= shdr_msk->lpfc[ 1 ][ 2 ];
	IO_SHDR.LPFC.bit.LPFC_2_0				= shdr_msk->lpfc[ 2 ][ 0 ];
	IO_SHDR.LPFC.bit.LPFC_2_1				= shdr_msk->lpfc[ 2 ][ 1 ];
	IO_SHDR.LPFC.bit.LPFC_2_2				= shdr_msk->lpfc[ 2 ][ 2 ];

	// BLDOFS.
	IO_SHDR.BLDOFS.bit.BLDOFS_0				= shdr_msk->blend_offset[ 0 ];
	IO_SHDR.BLDOFS.bit.BLDOFS_1				= shdr_msk->blend_offset[ 1 ];
	IO_SHDR.BLDOFS.bit.BLDOFS_2				= shdr_msk->blend_offset[ 2 ];
	IO_SHDR.BLDOFS.bit.BLDOFS_3				= shdr_msk->blend_offset[ 3 ];
	IO_SHDR.BLDOFS.bit.BLDOFS_4				= shdr_msk->blend_offset[ 4 ];
	IO_SHDR.BLDOFS.bit.BLDOFS_5				= shdr_msk->blend_offset[ 5 ];
	IO_SHDR.BLDOFS.bit.BLDOFS_6				= shdr_msk->blend_offset[ 6 ];

	// BLDSL.
	IO_SHDR.BLDSL.bit.BLDSL_0				= shdr_msk->blend_slope[ 0 ];
	IO_SHDR.BLDSL.bit.BLDSL_1				= shdr_msk->blend_slope[ 1 ];
	IO_SHDR.BLDSL.bit.BLDSL_2				= shdr_msk->blend_slope[ 2 ];
	IO_SHDR.BLDSL.bit.BLDSL_3				= shdr_msk->blend_slope[ 3 ];
	IO_SHDR.BLDSL.bit.BLDSL_4				= shdr_msk->blend_slope[ 4 ];
	IO_SHDR.BLDSL.bit.BLDSL_5				= shdr_msk->blend_slope[ 5 ];
	IO_SHDR.BLDSL.bit.BLDSL_6				= shdr_msk->blend_slope[ 6 ];

	// BLDB.
	IO_SHDR.BLDB.bit.BLDB_1					= shdr_msk->blend_boundary[ 1 ];
	IO_SHDR.BLDB.bit.BLDB_2					= shdr_msk->blend_boundary[ 2 ];
	IO_SHDR.BLDB.bit.BLDB_3					= shdr_msk->blend_boundary[ 3 ];
	IO_SHDR.BLDB.bit.BLDB_4					= shdr_msk->blend_boundary[ 4 ];
	IO_SHDR.BLDB.bit.BLDB_5					= shdr_msk->blend_boundary[ 5 ];
	IO_SHDR.BLDB.bit.BLDB_6					= shdr_msk->blend_boundary[ 6 ];

	// NORMGAIN.
	IO_SHDR.NORMGAIN.bit.NORMGAIN_0			= shdr_msk->norm_gain[ 0 ];
	IO_SHDR.NORMGAIN.bit.NORMGAIN_1			= shdr_msk->norm_gain[ 1 ];
	IO_SHDR.NORMGAIN.bit.NORMGAIN_2			= shdr_msk->norm_gain[ 2 ];

	// NORMOFS.
	IO_SHDR.NORMOFS.bit.NORMOFS_0			= shdr_msk->norm_offset[ 0 ];
	IO_SHDR.NORMOFS.bit.NORMOFS_1			= shdr_msk->norm_offset[ 1 ];
	IO_SHDR.NORMOFS.bit.NORMOFS_2			= shdr_msk->norm_offset[ 2 ];

	// MSKMD.
	IO_SHDR.MSKMD.bit.MSKMD					= shdr_msk->msk_mode;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR get MSK parameter.<br>
@param[out]		shdr_msk						Setting of SHDR MSK parameters. See @ref T_IM_SHDR_MSK.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Get_Mask( T_IM_SHDR_MSK* const shdr_msk )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_msk == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_Mask() error. shdr_msk = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// MSKY2RCLPUL.
	shdr_msk->y2r_clp_upper_long[ 0 ]	= IO_SHDR.MSKY2RCLPUL.bit.MSKY2RCLPUL_0;
	shdr_msk->y2r_clp_upper_long[ 1 ]	= IO_SHDR.MSKY2RCLPUL.bit.MSKY2RCLPUL_1;
	shdr_msk->y2r_clp_upper_long[ 2 ]	= IO_SHDR.MSKY2RCLPUL.bit.MSKY2RCLPUL_2;

	// MSKY2RCLPLL.
	shdr_msk->y2r_clp_lower_long[ 0 ]	= IO_SHDR.MSKY2RCLPLL.bit.MSKY2RCLPLL_0;
	shdr_msk->y2r_clp_lower_long[ 1 ]	= IO_SHDR.MSKY2RCLPLL.bit.MSKY2RCLPLL_1;
	shdr_msk->y2r_clp_lower_long[ 2 ]	= IO_SHDR.MSKY2RCLPLL.bit.MSKY2RCLPLL_2;

	// MSKY2RCLPUS.
	shdr_msk->y2r_clp_upper_short[ 0 ]	= IO_SHDR.MSKY2RCLPUS.bit.MSKY2RCLPUS_0;
	shdr_msk->y2r_clp_upper_short[ 1 ]	= IO_SHDR.MSKY2RCLPUS.bit.MSKY2RCLPUS_1;
	shdr_msk->y2r_clp_upper_short[ 2 ]	= IO_SHDR.MSKY2RCLPUS.bit.MSKY2RCLPUS_2;

	// MSKY2RCLPLS.
	shdr_msk->y2r_clp_lower_short[ 0 ]	= IO_SHDR.MSKY2RCLPLS.bit.MSKY2RCLPLS_0;
	shdr_msk->y2r_clp_lower_short[ 1 ]	= IO_SHDR.MSKY2RCLPLS.bit.MSKY2RCLPLS_1;
	shdr_msk->y2r_clp_lower_short[ 2 ]	= IO_SHDR.MSKY2RCLPLS.bit.MSKY2RCLPLS_2;

	// ALPHA.
	shdr_msk->alpha						= IO_SHDR.ALPHA.bit.ALPHA;

	// LPFC.
	shdr_msk->lpfc[ 0 ][ 0 ]			= IO_SHDR.LPFC.bit.LPFC_0_0;
	shdr_msk->lpfc[ 0 ][ 1 ]			= IO_SHDR.LPFC.bit.LPFC_0_1;
	shdr_msk->lpfc[ 0 ][ 2 ]			= IO_SHDR.LPFC.bit.LPFC_0_2;
	shdr_msk->lpfc[ 1 ][ 0 ]			= IO_SHDR.LPFC.bit.LPFC_1_0;
	shdr_msk->lpfc[ 1 ][ 1 ]			= IO_SHDR.LPFC.bit.LPFC_1_1;
	shdr_msk->lpfc[ 1 ][ 2 ]			= IO_SHDR.LPFC.bit.LPFC_1_2;
	shdr_msk->lpfc[ 2 ][ 0 ]			= IO_SHDR.LPFC.bit.LPFC_2_0;
	shdr_msk->lpfc[ 2 ][ 1 ]			= IO_SHDR.LPFC.bit.LPFC_2_1;
	shdr_msk->lpfc[ 2 ][ 2 ]			= IO_SHDR.LPFC.bit.LPFC_2_2;

	// BLDOFS.
	shdr_msk->blend_offset[ 0 ]			= IO_SHDR.BLDOFS.bit.BLDOFS_0;
	shdr_msk->blend_offset[ 1 ]			= IO_SHDR.BLDOFS.bit.BLDOFS_1;
	shdr_msk->blend_offset[ 2 ]			= IO_SHDR.BLDOFS.bit.BLDOFS_2;
	shdr_msk->blend_offset[ 3 ]			= IO_SHDR.BLDOFS.bit.BLDOFS_3;
	shdr_msk->blend_offset[ 4 ]			= IO_SHDR.BLDOFS.bit.BLDOFS_4;
	shdr_msk->blend_offset[ 5 ]			= IO_SHDR.BLDOFS.bit.BLDOFS_5;
	shdr_msk->blend_offset[ 6 ]			= IO_SHDR.BLDOFS.bit.BLDOFS_6;

	// BLDSL.
	shdr_msk->blend_slope[ 0 ]			= IO_SHDR.BLDSL.bit.BLDSL_0;
	shdr_msk->blend_slope[ 1 ]			= IO_SHDR.BLDSL.bit.BLDSL_1;
	shdr_msk->blend_slope[ 2 ]			= IO_SHDR.BLDSL.bit.BLDSL_2;
	shdr_msk->blend_slope[ 3 ]			= IO_SHDR.BLDSL.bit.BLDSL_3;
	shdr_msk->blend_slope[ 4 ]			= IO_SHDR.BLDSL.bit.BLDSL_4;
	shdr_msk->blend_slope[ 5 ]			= IO_SHDR.BLDSL.bit.BLDSL_5;
	shdr_msk->blend_slope[ 6 ]			= IO_SHDR.BLDSL.bit.BLDSL_6;

	// BLDB.
	shdr_msk->blend_boundary[ 1 ]		= IO_SHDR.BLDB.bit.BLDB_1;
	shdr_msk->blend_boundary[ 2 ]		= IO_SHDR.BLDB.bit.BLDB_2;
	shdr_msk->blend_boundary[ 3 ]		= IO_SHDR.BLDB.bit.BLDB_3;
	shdr_msk->blend_boundary[ 4 ]		= IO_SHDR.BLDB.bit.BLDB_4;
	shdr_msk->blend_boundary[ 5 ]		= IO_SHDR.BLDB.bit.BLDB_5;
	shdr_msk->blend_boundary[ 6 ]		= IO_SHDR.BLDB.bit.BLDB_6;

	// NORMGAIN.
	shdr_msk->norm_gain[ 0 ]			= IO_SHDR.NORMGAIN.bit.NORMGAIN_0;
	shdr_msk->norm_gain[ 1 ]			= IO_SHDR.NORMGAIN.bit.NORMGAIN_1;
	shdr_msk->norm_gain[ 2 ]			= IO_SHDR.NORMGAIN.bit.NORMGAIN_2;

	// NORMOFS.
	shdr_msk->norm_offset[ 0 ]			= IO_SHDR.NORMOFS.bit.NORMOFS_0;
	shdr_msk->norm_offset[ 1 ]			= IO_SHDR.NORMOFS.bit.NORMOFS_1;
	shdr_msk->norm_offset[ 2 ]			= IO_SHDR.NORMOFS.bit.NORMOFS_2;

	// MSKMD.
	shdr_msk->msk_mode					= IO_SHDR.MSKMD.bit.MSKMD;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR BLD parameter set.<br>
@param[in]		shdr_bld						Setting of SHDR BLD parameters. See @ref T_IM_SHDR_BLD.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Set_Blend( const T_IM_SHDR_BLD* const shdr_bld )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_bld == NULL ) {
		Ddim_Assertion(("Im_SHDR_Set_Blend() error. shdr_bld = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_bld->blend_sel > D_IM_SHDR_BLDSEL_LONG_SHORT ) {
		Ddim_Assertion(("Im_SHDR_Set_Blend() error. blend_sel is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_bld->alpha_sel > D_IM_SHDR_ALPHASEL_MERGEMASK ) {
		Ddim_Assertion(("Im_SHDR_Set_Blend() error. alpha_sel is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// BLDSEL.
	IO_SHDR.BLDSEL.bit.BLDSEL		= shdr_bld->blend_sel;

	// ALPHASEL.
	IO_SHDR.ALPHASEL.bit.ALPHASEL	= shdr_bld->alpha_sel;

	// ALPHAVAL.
	IO_SHDR.ALPHAVAL.bit.ALPHAVALY	= shdr_bld->alpha_val[ 0 ];
	IO_SHDR.ALPHAVAL.bit.ALPHAVALU	= shdr_bld->alpha_val[ 1 ];
	IO_SHDR.ALPHAVAL.bit.ALPHAVALV	= shdr_bld->alpha_val[ 2 ];

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR get BLD parameter.<br>
@param[out]		shdr_bld						Setting of SHDR BLD parameters. See @ref T_IM_SHDR_BLD.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Get_Blend( T_IM_SHDR_BLD* const shdr_bld )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_bld == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_Blend() error. shdr_bld = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// BLDSEL.
	shdr_bld->blend_sel			= IO_SHDR.BLDSEL.bit.BLDSEL;

	// ALPHASEL.
	shdr_bld->alpha_sel			= IO_SHDR.ALPHASEL.bit.ALPHASEL;

	// ALPHAVAL.
	shdr_bld->alpha_val[ 0 ]	= IO_SHDR.ALPHAVAL.bit.ALPHAVALY;
	shdr_bld->alpha_val[ 1 ]	= IO_SHDR.ALPHAVAL.bit.ALPHAVALU;
	shdr_bld->alpha_val[ 2 ]	= IO_SHDR.ALPHAVAL.bit.ALPHAVALV;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR POST parameter set.<br>
@param[in]		shdr_post						Setting of SHDR POST parameters. See @ref T_IM_SHDR_POST.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Set_Post( const T_IM_SHDR_POST* const shdr_post )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_post == NULL ) {
		Ddim_Assertion(("Im_SHDR_Set_Post() error. shdr_post = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_post->ev_sel > D_IM_SHDR_POSTSEL1_RGB ) {
		Ddim_Assertion(("Im_SHDR_Set_Post() error. ev_sel is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_post->mulqpos > D_IM_SHDR_POSTMULQPOS_Q2 ) {
		Ddim_Assertion(("Im_SHDR_Set_Post() error. mulqpos is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_post->css_sel > D_IM_SHDR_POSTSEL0_BYPASS_YCC8 ) {
		Ddim_Assertion(("Im_SHDR_Set_Post() error. css_sel is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_post->hdrcss_mode > D_IM_SHDR_HDRCSSMD_YCC422_CENTER ) {
		Ddim_Assertion(("Im_SHDR_Set_Post() error. hdrcss_mode is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// POSTSFT1RND.
	IO_SHDR.POSTSFT1RND.bit.POSTSFT1RNDEN		= shdr_post->sft1rnd.sft1rnd_en;

	// POSTY2RCOEF.
	im_shdr_set_reg_signed_a( IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_0, shdr_post->y2r_coef[ 0 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_1, shdr_post->y2r_coef[ 1 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_2, shdr_post->y2r_coef[ 2 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_3, shdr_post->y2r_coef[ 3 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_4, shdr_post->y2r_coef[ 4 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_5, shdr_post->y2r_coef[ 5 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_6, shdr_post->y2r_coef[ 6 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_7, shdr_post->y2r_coef[ 7 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_8, shdr_post->y2r_coef[ 8 ] );

	// POSTY2RCLPTHU.
	IO_SHDR.POSTY2RCLPTHU.bit.POSTY2RCLPTHU_0	= shdr_post->y2r_clp_th_upper[ 0 ];
	IO_SHDR.POSTY2RCLPTHU.bit.POSTY2RCLPTHU_1	= shdr_post->y2r_clp_th_upper[ 1 ];
	IO_SHDR.POSTY2RCLPTHU.bit.POSTY2RCLPTHU_2	= shdr_post->y2r_clp_th_upper[ 2 ];

	// POSTY2RCLPTHL.
	IO_SHDR.POSTY2RCLPTHL.bit.POSTY2RCLPTHL_0	= shdr_post->y2r_clp_th_lower[ 0 ];
	IO_SHDR.POSTY2RCLPTHL.bit.POSTY2RCLPTHL_1	= shdr_post->y2r_clp_th_lower[ 1 ];
	IO_SHDR.POSTY2RCLPTHL.bit.POSTY2RCLPTHL_2	= shdr_post->y2r_clp_th_lower[ 2 ];

	// POSTSEL1.
	IO_SHDR.POSTSEL1.bit.POSTSEL1				= shdr_post->ev_sel;

	// POSTMULQPOS.
	IO_SHDR.POSTMULQPOS.bit.POSTMULQPOS			= shdr_post->mulqpos;

	// EVKNB.
	IO_SHDR.EVKNB.bit.EVKNB_1					= shdr_post->ev_kn_boundary[ 1 ];
	IO_SHDR.EVKNB.bit.EVKNB_2					= shdr_post->ev_kn_boundary[ 2 ];
	IO_SHDR.EVKNB.bit.EVKNB_3					= shdr_post->ev_kn_boundary[ 3 ];
	IO_SHDR.EVKNB.bit.EVKNB_4					= shdr_post->ev_kn_boundary[ 4 ];
	IO_SHDR.EVKNB.bit.EVKNB_5					= shdr_post->ev_kn_boundary[ 5 ];
	IO_SHDR.EVKNB.bit.EVKNB_6					= shdr_post->ev_kn_boundary[ 6 ];
	IO_SHDR.EVKNB.bit.EVKNB_7					= shdr_post->ev_kn_boundary[ 7 ];
	IO_SHDR.EVKNB.bit.EVKNB_8					= shdr_post->ev_kn_boundary[ 8 ];
	IO_SHDR.EVKNB.bit.EVKNB_9					= shdr_post->ev_kn_boundary[ 9 ];

	// EVKNOFS.
	IO_SHDR.EVKNOFS.bit.EVKNOFS_0				= shdr_post->ev_kn_offset[ 0 ];
	IO_SHDR.EVKNOFS.bit.EVKNOFS_1				= shdr_post->ev_kn_offset[ 1 ];
	IO_SHDR.EVKNOFS.bit.EVKNOFS_2				= shdr_post->ev_kn_offset[ 2 ];
	IO_SHDR.EVKNOFS.bit.EVKNOFS_3				= shdr_post->ev_kn_offset[ 3 ];
	IO_SHDR.EVKNOFS.bit.EVKNOFS_4				= shdr_post->ev_kn_offset[ 4 ];
	IO_SHDR.EVKNOFS.bit.EVKNOFS_5				= shdr_post->ev_kn_offset[ 5 ];
	IO_SHDR.EVKNOFS.bit.EVKNOFS_6				= shdr_post->ev_kn_offset[ 6 ];
	IO_SHDR.EVKNOFS.bit.EVKNOFS_7				= shdr_post->ev_kn_offset[ 7 ];
	IO_SHDR.EVKNOFS.bit.EVKNOFS_8				= shdr_post->ev_kn_offset[ 8 ];
	IO_SHDR.EVKNOFS.bit.EVKNOFS_9				= shdr_post->ev_kn_offset[ 9 ];

	// EVKNSL.
	im_shdr_set_reg_signed_a( IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_0, shdr_post->ev_kn_slope[ 0 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_1, shdr_post->ev_kn_slope[ 1 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_2, shdr_post->ev_kn_slope[ 2 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_3, shdr_post->ev_kn_slope[ 3 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_4, shdr_post->ev_kn_slope[ 4 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_5, shdr_post->ev_kn_slope[ 5 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_6, shdr_post->ev_kn_slope[ 6 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_7, shdr_post->ev_kn_slope[ 7 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_8, shdr_post->ev_kn_slope[ 8 ] );
	im_shdr_set_reg_signed_a( IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_9, shdr_post->ev_kn_slope[ 9 ] );

	// POSTSEL0.
	IO_SHDR.POSTSEL0.bit.POSTSEL0				= shdr_post->css_sel;

	// HDRCSSMD.
	IO_SHDR.HDRCSSMD.bit.HDRCSSMD				= shdr_post->hdrcss_mode;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR get POST parameter.<br>
@param[out]		shdr_post						Setting of SHDR POST parameters. See @ref T_IM_SHDR_POST.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Get_Post( T_IM_SHDR_POST* const shdr_post )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_post == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_Post() error. shdr_post = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// POSTSFT1RND.
	shdr_post->sft1rnd.sft1rnd_en	= IO_SHDR.POSTSFT1RND.bit.POSTSFT1RNDEN;

	// POSTY2RCOEF.
	im_shdr_get_reg_signed_a( shdr_post->y2r_coef[ 0 ], IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_0 );
	im_shdr_get_reg_signed_a( shdr_post->y2r_coef[ 1 ], IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_1 );
	im_shdr_get_reg_signed_a( shdr_post->y2r_coef[ 2 ], IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_2 );
	im_shdr_get_reg_signed_a( shdr_post->y2r_coef[ 3 ], IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_3 );
	im_shdr_get_reg_signed_a( shdr_post->y2r_coef[ 4 ], IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_4 );
	im_shdr_get_reg_signed_a( shdr_post->y2r_coef[ 5 ], IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_5 );
	im_shdr_get_reg_signed_a( shdr_post->y2r_coef[ 6 ], IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_6 );
	im_shdr_get_reg_signed_a( shdr_post->y2r_coef[ 7 ], IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_7 );
	im_shdr_get_reg_signed_a( shdr_post->y2r_coef[ 8 ], IO_SHDR.POSTY2RCOEF, union io_shdr_posty2rcoef, POSTY2RCOEF_8 );

	// POSTY2RCLPTHU.
	shdr_post->y2r_clp_th_upper[ 0 ]	= IO_SHDR.POSTY2RCLPTHU.bit.POSTY2RCLPTHU_0;
	shdr_post->y2r_clp_th_upper[ 1 ]	= IO_SHDR.POSTY2RCLPTHU.bit.POSTY2RCLPTHU_1;
	shdr_post->y2r_clp_th_upper[ 2 ]	= IO_SHDR.POSTY2RCLPTHU.bit.POSTY2RCLPTHU_2;

	// POSTY2RCLPTHL.
	shdr_post->y2r_clp_th_lower[ 0 ]	= IO_SHDR.POSTY2RCLPTHL.bit.POSTY2RCLPTHL_0;
	shdr_post->y2r_clp_th_lower[ 1 ]	= IO_SHDR.POSTY2RCLPTHL.bit.POSTY2RCLPTHL_1;
	shdr_post->y2r_clp_th_lower[ 2 ]	= IO_SHDR.POSTY2RCLPTHL.bit.POSTY2RCLPTHL_2;

	// POSTSEL1.
	shdr_post->ev_sel					= IO_SHDR.POSTSEL1.bit.POSTSEL1;

	// POSTMULQPOS.
	shdr_post->mulqpos					= IO_SHDR.POSTMULQPOS.bit.POSTMULQPOS;

	// EVKNB.
	shdr_post->ev_kn_boundary[ 1 ]		= IO_SHDR.EVKNB.bit.EVKNB_1;
	shdr_post->ev_kn_boundary[ 2 ]		= IO_SHDR.EVKNB.bit.EVKNB_2;
	shdr_post->ev_kn_boundary[ 3 ]		= IO_SHDR.EVKNB.bit.EVKNB_3;
	shdr_post->ev_kn_boundary[ 4 ]		= IO_SHDR.EVKNB.bit.EVKNB_4;
	shdr_post->ev_kn_boundary[ 5 ]		= IO_SHDR.EVKNB.bit.EVKNB_5;
	shdr_post->ev_kn_boundary[ 6 ]		= IO_SHDR.EVKNB.bit.EVKNB_6;
	shdr_post->ev_kn_boundary[ 7 ]		= IO_SHDR.EVKNB.bit.EVKNB_7;
	shdr_post->ev_kn_boundary[ 8 ]		= IO_SHDR.EVKNB.bit.EVKNB_8;
	shdr_post->ev_kn_boundary[ 9 ]		= IO_SHDR.EVKNB.bit.EVKNB_9;

	// EVKNOFS.
	shdr_post->ev_kn_offset[ 0 ]		= IO_SHDR.EVKNOFS.bit.EVKNOFS_0;
	shdr_post->ev_kn_offset[ 1 ]		= IO_SHDR.EVKNOFS.bit.EVKNOFS_1;
	shdr_post->ev_kn_offset[ 2 ]		= IO_SHDR.EVKNOFS.bit.EVKNOFS_2;
	shdr_post->ev_kn_offset[ 3 ]		= IO_SHDR.EVKNOFS.bit.EVKNOFS_3;
	shdr_post->ev_kn_offset[ 4 ]		= IO_SHDR.EVKNOFS.bit.EVKNOFS_4;
	shdr_post->ev_kn_offset[ 5 ]		= IO_SHDR.EVKNOFS.bit.EVKNOFS_5;
	shdr_post->ev_kn_offset[ 6 ]		= IO_SHDR.EVKNOFS.bit.EVKNOFS_6;
	shdr_post->ev_kn_offset[ 7 ]		= IO_SHDR.EVKNOFS.bit.EVKNOFS_7;
	shdr_post->ev_kn_offset[ 8 ]		= IO_SHDR.EVKNOFS.bit.EVKNOFS_8;
	shdr_post->ev_kn_offset[ 9 ]		= IO_SHDR.EVKNOFS.bit.EVKNOFS_9;

	// EVKNSL.
	im_shdr_get_reg_signed_a( shdr_post->ev_kn_slope[ 0 ], IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_0 );
	im_shdr_get_reg_signed_a( shdr_post->ev_kn_slope[ 1 ], IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_1 );
	im_shdr_get_reg_signed_a( shdr_post->ev_kn_slope[ 2 ], IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_2 );
	im_shdr_get_reg_signed_a( shdr_post->ev_kn_slope[ 3 ], IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_3 );
	im_shdr_get_reg_signed_a( shdr_post->ev_kn_slope[ 4 ], IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_4 );
	im_shdr_get_reg_signed_a( shdr_post->ev_kn_slope[ 5 ], IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_5 );
	im_shdr_get_reg_signed_a( shdr_post->ev_kn_slope[ 6 ], IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_6 );
	im_shdr_get_reg_signed_a( shdr_post->ev_kn_slope[ 7 ], IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_7 );
	im_shdr_get_reg_signed_a( shdr_post->ev_kn_slope[ 8 ], IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_8 );
	im_shdr_get_reg_signed_a( shdr_post->ev_kn_slope[ 9 ], IO_SHDR.EVKNSL, union io_shdr_evknsl, EVKNSL_9 );

	// POSTSEL0.
	shdr_post->css_sel					= IO_SHDR.POSTSEL0.bit.POSTSEL0;

	// HDRCSSMD.
	shdr_post->hdrcss_mode				= IO_SHDR.HDRCSSMD.bit.HDRCSSMD;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR resize parameter set.<br>
@param[in]		shdr_resize					Setting of SHDR resize parameters. See @ref T_IM_SHDR_RESIZE.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Set_Resize( const T_IM_SHDR_RESIZE* const shdr_resize )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_resize == NULL ) {
		Ddim_Assertion(("Im_SHDR_Set_Resize() error. shdr_resize = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( shdr_resize->chmode.a_w_en > D_IM_SHDR_ENABLE ) ||
		 ( shdr_resize->chmode.b_w_en > D_IM_SHDR_ENABLE ) ||
		 ( shdr_resize->chmode.c_w_en > D_IM_SHDR_ENABLE ) ||
		 ( shdr_resize->chmode.d_w_en > D_IM_SHDR_ENABLE ) ||
		 ( shdr_resize->chmode.e_w_en > D_IM_SHDR_ENABLE ) ||
		 ( shdr_resize->chmode.f_w_en > D_IM_SHDR_ENABLE ) ) {
		Ddim_Assertion(("Im_SHDR_Set_Resize() error. x_w_en is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( shdr_resize->chmode.a_drc_en > D_IM_SHDR_ENABLE ) ||
		 ( shdr_resize->chmode.c_drc_en > D_IM_SHDR_ENABLE ) ||
		 ( shdr_resize->chmode.d_drc_en > D_IM_SHDR_ENABLE ) ) {
		Ddim_Assertion(("Im_SHDR_Set_Resize() error. x_drc_en is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( shdr_resize->chmode.a_css_md > D_IM_SHDR_CSSMD_YCC420_CENTER ) ||
		 ( shdr_resize->chmode.b_css_md > D_IM_SHDR_CSSMD_YCC420_CENTER ) ||
		 ( shdr_resize->chmode.c_css_md > D_IM_SHDR_CSSMD_YCC420_CENTER ) ||
		 ( shdr_resize->chmode.d_css_md > D_IM_SHDR_CSSMD_YCC420_CENTER ) ||
		 ( shdr_resize->chmode.e_css_md > D_IM_SHDR_CSSMD_YCC420_CENTER ) ||
		 ( shdr_resize->chmode.f_css_md > D_IM_SHDR_CSSMD_YCC420_CENTER ) ) {
		Ddim_Assertion(("Im_SHDR_Set_Resize() error. x_css_md is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( shdr_resize->chmode.b_rdc_md > D_IM_RDCMD1_1_32 ) ||
		 ( shdr_resize->chmode.c_rdc_md > D_IM_RDCMD1_1_32 ) ||
		 ( shdr_resize->chmode.d_rdc_md > D_IM_RDCMD1_1_32 ) ||
		 ( shdr_resize->chmode.e_rdc_md > D_IM_RDCMD1_1_32 ) ||
		 ( shdr_resize->chmode.f_rdc_md > D_IM_RDCMD1_1_32 ) ) {
		Ddim_Assertion(("Im_SHDR_Set_Resize() error. x_rdc_md is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( shdr_resize->chmode.c_rsz_md > D_IM_RSZMD_CENTER1 ) ||
		 ( shdr_resize->chmode.d_rsz_md > D_IM_RSZMD_CENTER1 ) ||
		 ( shdr_resize->chmode.e_rsz_md > D_IM_RSZMD_CENTER1 ) ||
		 ( shdr_resize->chmode.f_rsz_md > D_IM_RSZMD_CENTER1 ) ) {
		Ddim_Assertion(("Im_SHDR_Set_Resize() error. x_rsz_md is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( shdr_resize->chmode.c_trm_en > D_IM_SHDR_ENABLE ) ||
		 ( shdr_resize->chmode.d_trm_en > D_IM_SHDR_ENABLE ) ||
		 ( shdr_resize->chmode.e_trm_en > D_IM_SHDR_ENABLE ) ||
		 ( shdr_resize->chmode.f_trm_en > D_IM_SHDR_ENABLE ) ) {
		Ddim_Assertion(("Im_SHDR_Set_Resize() error. x_trm_en is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	// limitation check 1.
	if ( ( gIM_SHDR_Out_Ctrl == D_IM_SHDR_OUTCTRL_RGB ) &&
		 ( ( shdr_resize->chmode.a_w_en == D_IM_SHDR_ENABLE ) ||
		   ( shdr_resize->chmode.b_w_en == D_IM_SHDR_ENABLE ) ||
		   ( shdr_resize->chmode.c_w_en == D_IM_SHDR_ENABLE ) ||
		   ( shdr_resize->chmode.d_w_en == D_IM_SHDR_ENABLE ) ||
		   ( shdr_resize->chmode.e_w_en == D_IM_SHDR_ENABLE ) ||
		   ( shdr_resize->chmode.f_w_en == D_IM_SHDR_ENABLE ) ) ) {
		Ddim_Print(("Im_SHDR_Set_Resize() : limitation check 1 NG\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}

	im_shdr_on_pclk();
	im_shdr_dsb();

	// CHMODE.
	IO_SHDR.CH0MODE.bit.WEN0		= shdr_resize->chmode.a_w_en;
	IO_SHDR.CH0MODE.bit.DRCEN0		= shdr_resize->chmode.a_drc_en;
	IO_SHDR.CH0MODE.bit.CSSMD0		= shdr_resize->chmode.a_css_md;
	IO_SHDR.CH1MODE.bit.WEN1		= shdr_resize->chmode.b_w_en;
	IO_SHDR.CH1MODE.bit.RDCMD1		= shdr_resize->chmode.b_rdc_md;
	IO_SHDR.CH1MODE.bit.CSSMD1		= shdr_resize->chmode.b_css_md;
	IO_SHDR.CH2MODE.bit.WEN2		= shdr_resize->chmode.c_w_en;
	IO_SHDR.CH2MODE.bit.RSZMD2		= shdr_resize->chmode.c_rsz_md;
	IO_SHDR.CH2MODE.bit.RDCEN2		= shdr_resize->chmode.c_rdc_en;
	IO_SHDR.CH2MODE.bit.RDCMD2		= shdr_resize->chmode.c_rdc_md;
	IO_SHDR.CH2MODE.bit.TRMEN2		= shdr_resize->chmode.c_trm_en;
	IO_SHDR.CH2MODE.bit.DRCEN2		= shdr_resize->chmode.c_drc_en;
	IO_SHDR.CH2MODE.bit.CSSMD2		= shdr_resize->chmode.c_css_md;
	IO_SHDR.CH3MODE.bit.WEN3		= shdr_resize->chmode.d_w_en;
	IO_SHDR.CH3MODE.bit.RSZMD3		= shdr_resize->chmode.d_rsz_md;
	IO_SHDR.CH3MODE.bit.RDCEN3		= shdr_resize->chmode.d_rdc_en;
	IO_SHDR.CH3MODE.bit.RDCMD3		= shdr_resize->chmode.d_rdc_md;
	IO_SHDR.CH3MODE.bit.TRMEN3		= shdr_resize->chmode.d_trm_en;
	IO_SHDR.CH3MODE.bit.DRCEN3		= shdr_resize->chmode.d_drc_en;
	IO_SHDR.CH3MODE.bit.CSSMD3		= shdr_resize->chmode.d_css_md;
	IO_SHDR.CH4MODE.bit.WEN4		= shdr_resize->chmode.e_w_en;
	IO_SHDR.CH4MODE.bit.RSZMD4		= shdr_resize->chmode.e_rsz_md;
	IO_SHDR.CH4MODE.bit.RDCEN4		= shdr_resize->chmode.e_rdc_en;
	IO_SHDR.CH4MODE.bit.RDCMD4		= shdr_resize->chmode.e_rdc_md;
	IO_SHDR.CH4MODE.bit.TRMEN4		= shdr_resize->chmode.e_trm_en;
	IO_SHDR.CH4MODE.bit.CSSMD4		= shdr_resize->chmode.e_css_md;
	IO_SHDR.CH5MODE.bit.WEN5		= shdr_resize->chmode.f_w_en;
	IO_SHDR.CH5MODE.bit.RSZMD5		= shdr_resize->chmode.f_rsz_md;
	IO_SHDR.CH5MODE.bit.RDCEN5		= shdr_resize->chmode.f_rdc_en;
	IO_SHDR.CH5MODE.bit.RDCMD5		= shdr_resize->chmode.f_rdc_md;
	IO_SHDR.CH5MODE.bit.TRMEN5		= shdr_resize->chmode.f_trm_en;
	IO_SHDR.CH5MODE.bit.CSSMD5		= shdr_resize->chmode.f_css_md;

	// WOUTSZ.
	IO_SHDR.WOUTSZ.bit.WOUTHSZ_0	= shdr_resize->woutsz.a_hsize;
	IO_SHDR.WOUTSZ.bit.WOUTVSZ_0	= shdr_resize->woutsz.a_vsize;
	IO_SHDR.WOUTSZ.bit.WOUTHSZ_1	= shdr_resize->woutsz.b_hsize;
	IO_SHDR.WOUTSZ.bit.WOUTVSZ_1	= shdr_resize->woutsz.b_vsize;
	IO_SHDR.WOUTSZ.bit.WOUTHSZ_2	= shdr_resize->woutsz.c_hsize;
	IO_SHDR.WOUTSZ.bit.WOUTVSZ_2	= shdr_resize->woutsz.c_vsize;
	IO_SHDR.WOUTSZ.bit.WOUTHSZ_3	= shdr_resize->woutsz.d_hsize;
	IO_SHDR.WOUTSZ.bit.WOUTVSZ_3	= shdr_resize->woutsz.d_vsize;
	IO_SHDR.WOUTSZ.bit.WOUTHSZ_4	= shdr_resize->woutsz.e_hsize;
	IO_SHDR.WOUTSZ.bit.WOUTVSZ_4	= shdr_resize->woutsz.e_vsize;
	IO_SHDR.WOUTSZ.bit.WOUTHSZ_5	= shdr_resize->woutsz.f_hsize;
	IO_SHDR.WOUTSZ.bit.WOUTVSZ_5	= shdr_resize->woutsz.f_vsize;

	// RSZHCNT.
	IO_SHDR.RSZHCNT_2.bit.RSZHSTA	= shdr_resize->rszhcnt.c_hsta;
	IO_SHDR.RSZHCNT_2.bit.RSZHPIT	= shdr_resize->rszhcnt.c_hpit;
	IO_SHDR.RSZHCNT_3.bit.RSZHSTA	= shdr_resize->rszhcnt.d_hsta;
	IO_SHDR.RSZHCNT_3.bit.RSZHPIT	= shdr_resize->rszhcnt.d_hpit;
	IO_SHDR.RSZHCNT_4.bit.RSZHSTA	= shdr_resize->rszhcnt.e_hsta;
	IO_SHDR.RSZHCNT_4.bit.RSZHPIT	= shdr_resize->rszhcnt.e_hpit;
	IO_SHDR.RSZHCNT_5.bit.RSZHSTA	= shdr_resize->rszhcnt.f_hsta;
	IO_SHDR.RSZHCNT_5.bit.RSZHPIT	= shdr_resize->rszhcnt.f_hpit;

	// RSZVCNT.
	IO_SHDR.RSZVCNT_2.bit.RSZVSTA	= shdr_resize->rszvcnt.c_vsta;
	IO_SHDR.RSZVCNT_2.bit.RSZVPIT	= shdr_resize->rszvcnt.c_vpit;
	IO_SHDR.RSZVCNT_3.bit.RSZVSTA	= shdr_resize->rszvcnt.d_vsta;
	IO_SHDR.RSZVCNT_3.bit.RSZVPIT	= shdr_resize->rszvcnt.d_vpit;
	IO_SHDR.RSZVCNT_4.bit.RSZVSTA	= shdr_resize->rszvcnt.e_vsta;
	IO_SHDR.RSZVCNT_4.bit.RSZVPIT	= shdr_resize->rszvcnt.e_vpit;
	IO_SHDR.RSZVCNT_5.bit.RSZVSTA	= shdr_resize->rszvcnt.f_vsta;
	IO_SHDR.RSZVCNT_5.bit.RSZVPIT	= shdr_resize->rszvcnt.f_vpit;

	// TRMSTA.
	IO_SHDR.TRMSTA_2.bit.TRMHSTA	= shdr_resize->trmsta.c_hsta;
	IO_SHDR.TRMSTA_2.bit.TRMVSTA	= shdr_resize->trmsta.c_vsta;
	IO_SHDR.TRMSTA_3.bit.TRMHSTA	= shdr_resize->trmsta.d_hsta;
	IO_SHDR.TRMSTA_3.bit.TRMVSTA	= shdr_resize->trmsta.d_vsta;
	IO_SHDR.TRMSTA_4.bit.TRMHSTA	= shdr_resize->trmsta.e_hsta;
	IO_SHDR.TRMSTA_4.bit.TRMVSTA	= shdr_resize->trmsta.e_vsta;
	IO_SHDR.TRMSTA_5.bit.TRMHSTA	= shdr_resize->trmsta.f_hsta;
	IO_SHDR.TRMSTA_5.bit.TRMVSTA	= shdr_resize->trmsta.f_vsta;

	// DRCOFST.
	IO_SHDR.DRCOFST_0.bit.DRCYOFST	= shdr_resize->drcofst.a_y;
	im_shdr_set_reg_signed( IO_SHDR.DRCOFST_0, union io_shdr_drcofst, DRCCBOFST, shdr_resize->drcofst.a_cb );
	im_shdr_set_reg_signed( IO_SHDR.DRCOFST_0, union io_shdr_drcofst, DRCCROFST, shdr_resize->drcofst.a_cr );

	IO_SHDR.DRCOFST_2.bit.DRCYOFST	= shdr_resize->drcofst.c_y;
	im_shdr_set_reg_signed( IO_SHDR.DRCOFST_2, union io_shdr_drcofst, DRCCBOFST, shdr_resize->drcofst.c_cb );
	im_shdr_set_reg_signed( IO_SHDR.DRCOFST_2, union io_shdr_drcofst, DRCCROFST, shdr_resize->drcofst.c_cr );

	IO_SHDR.DRCOFST_3.bit.DRCYOFST	= shdr_resize->drcofst.d_y;
	im_shdr_set_reg_signed( IO_SHDR.DRCOFST_3, union io_shdr_drcofst, DRCCBOFST, shdr_resize->drcofst.d_cb );
	im_shdr_set_reg_signed( IO_SHDR.DRCOFST_3, union io_shdr_drcofst, DRCCROFST, shdr_resize->drcofst.d_cr );

	// DRCGAIN.
	IO_SHDR.DRCGAIN_0.bit.DRCYGAIN	= shdr_resize->drcgain.a_y;
	IO_SHDR.DRCGAIN_0.bit.DRCCBGAIN	= shdr_resize->drcgain.a_cb;
	IO_SHDR.DRCGAIN_0.bit.DRCCRGAIN	= shdr_resize->drcgain.a_cr;
	IO_SHDR.DRCGAIN_2.bit.DRCYGAIN	= shdr_resize->drcgain.c_y;
	IO_SHDR.DRCGAIN_2.bit.DRCCBGAIN	= shdr_resize->drcgain.c_cb;
	IO_SHDR.DRCGAIN_2.bit.DRCCRGAIN	= shdr_resize->drcgain.c_cr;
	IO_SHDR.DRCGAIN_3.bit.DRCYGAIN	= shdr_resize->drcgain.d_y;
	IO_SHDR.DRCGAIN_3.bit.DRCCBGAIN	= shdr_resize->drcgain.d_cb;
	IO_SHDR.DRCGAIN_3.bit.DRCCRGAIN	= shdr_resize->drcgain.d_cr;

	// DRCCLPH.
	IO_SHDR.DRCCLPH_0.bit.DRCYCLPH	= shdr_resize->drcclph.a_y;
	im_shdr_set_reg_signed( IO_SHDR.DRCCLPH_0, union io_shdr_drcclph, DRCCBCLPH, shdr_resize->drcclph.a_cb );
	im_shdr_set_reg_signed( IO_SHDR.DRCCLPH_0, union io_shdr_drcclph, DRCCRCLPH, shdr_resize->drcclph.a_cr );

	IO_SHDR.DRCCLPH_2.bit.DRCYCLPH	= shdr_resize->drcclph.c_y;
	im_shdr_set_reg_signed( IO_SHDR.DRCCLPH_2, union io_shdr_drcclph, DRCCBCLPH, shdr_resize->drcclph.c_cb );
	im_shdr_set_reg_signed( IO_SHDR.DRCCLPH_2, union io_shdr_drcclph, DRCCRCLPH, shdr_resize->drcclph.c_cr );

	IO_SHDR.DRCCLPH_3.bit.DRCYCLPH	= shdr_resize->drcclph.d_y;
	im_shdr_set_reg_signed( IO_SHDR.DRCCLPH_3, union io_shdr_drcclph, DRCCBCLPH, shdr_resize->drcclph.d_cb );
	im_shdr_set_reg_signed( IO_SHDR.DRCCLPH_3, union io_shdr_drcclph, DRCCRCLPH, shdr_resize->drcclph.d_cr );

	// DRCCLPL.
	IO_SHDR.DRCCLPL_0.bit.DRCYCLPL	= shdr_resize->drcclpl.a_y;
	im_shdr_set_reg_signed( IO_SHDR.DRCCLPL_0, union io_shdr_drcclpl, DRCCBCLPL, shdr_resize->drcclpl.a_cb );
	im_shdr_set_reg_signed( IO_SHDR.DRCCLPL_0, union io_shdr_drcclpl, DRCCRCLPL, shdr_resize->drcclpl.a_cr );

	IO_SHDR.DRCCLPL_2.bit.DRCYCLPL	= shdr_resize->drcclpl.c_y;
	im_shdr_set_reg_signed( IO_SHDR.DRCCLPL_2, union io_shdr_drcclpl, DRCCBCLPL, shdr_resize->drcclpl.c_cb );
	im_shdr_set_reg_signed( IO_SHDR.DRCCLPL_2, union io_shdr_drcclpl, DRCCRCLPL, shdr_resize->drcclpl.c_cr );

	IO_SHDR.DRCCLPL_3.bit.DRCYCLPL	= shdr_resize->drcclpl.d_y;
	im_shdr_set_reg_signed( IO_SHDR.DRCCLPL_3, union io_shdr_drcclpl, DRCCBCLPL, shdr_resize->drcclpl.d_cb );
	im_shdr_set_reg_signed( IO_SHDR.DRCCLPL_3, union io_shdr_drcclpl, DRCCRCLPL, shdr_resize->drcclpl.d_cr );

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR get resize parameter.<br>
@param[out]		shdr_resize					Setting of SHDR resize parameters. See @ref T_IM_SHDR_RESIZE.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Get_Resize( T_IM_SHDR_RESIZE* const shdr_resize )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_resize == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_Resize() error. shdr_resize = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// CHMODE.
	shdr_resize->chmode.a_w_en		= IO_SHDR.CH0MODE.bit.WEN0;
	shdr_resize->chmode.a_drc_en	= IO_SHDR.CH0MODE.bit.DRCEN0;
	shdr_resize->chmode.a_css_md	= IO_SHDR.CH0MODE.bit.CSSMD0;
	shdr_resize->chmode.b_w_en		= IO_SHDR.CH1MODE.bit.WEN1;
	shdr_resize->chmode.b_rdc_md	= IO_SHDR.CH1MODE.bit.RDCMD1;
	shdr_resize->chmode.b_css_md	= IO_SHDR.CH1MODE.bit.CSSMD1;
	shdr_resize->chmode.c_w_en		= IO_SHDR.CH2MODE.bit.WEN2;
	shdr_resize->chmode.c_rsz_md	= IO_SHDR.CH2MODE.bit.RSZMD2;
	shdr_resize->chmode.c_rdc_en	= IO_SHDR.CH2MODE.bit.RDCEN2;
	shdr_resize->chmode.c_rdc_md	= IO_SHDR.CH2MODE.bit.RDCMD2;
	shdr_resize->chmode.c_trm_en	= IO_SHDR.CH2MODE.bit.TRMEN2;
	shdr_resize->chmode.c_drc_en	= IO_SHDR.CH2MODE.bit.DRCEN2;
	shdr_resize->chmode.c_css_md	= IO_SHDR.CH2MODE.bit.CSSMD2;
	shdr_resize->chmode.d_w_en		= IO_SHDR.CH3MODE.bit.WEN3;
	shdr_resize->chmode.d_rsz_md	= IO_SHDR.CH3MODE.bit.RSZMD3;
	shdr_resize->chmode.d_rdc_en	= IO_SHDR.CH3MODE.bit.RDCEN3;
	shdr_resize->chmode.d_rdc_md	= IO_SHDR.CH3MODE.bit.RDCMD3;
	shdr_resize->chmode.d_trm_en	= IO_SHDR.CH3MODE.bit.TRMEN3;
	shdr_resize->chmode.d_drc_en	= IO_SHDR.CH3MODE.bit.DRCEN3;
	shdr_resize->chmode.d_css_md	= IO_SHDR.CH3MODE.bit.CSSMD3;
	shdr_resize->chmode.e_w_en		= IO_SHDR.CH4MODE.bit.WEN4;
	shdr_resize->chmode.e_rsz_md	= IO_SHDR.CH4MODE.bit.RSZMD4;
	shdr_resize->chmode.e_rdc_en	= IO_SHDR.CH4MODE.bit.RDCEN4;
	shdr_resize->chmode.e_rdc_md	= IO_SHDR.CH4MODE.bit.RDCMD4;
	shdr_resize->chmode.e_trm_en	= IO_SHDR.CH4MODE.bit.TRMEN4;
	shdr_resize->chmode.e_css_md	= IO_SHDR.CH4MODE.bit.CSSMD4;
	shdr_resize->chmode.f_w_en		= IO_SHDR.CH5MODE.bit.WEN5;
	shdr_resize->chmode.f_rsz_md	= IO_SHDR.CH5MODE.bit.RSZMD5;
	shdr_resize->chmode.f_rdc_en	= IO_SHDR.CH5MODE.bit.RDCEN5;
	shdr_resize->chmode.f_rdc_md	= IO_SHDR.CH5MODE.bit.RDCMD5;
	shdr_resize->chmode.f_trm_en	= IO_SHDR.CH5MODE.bit.TRMEN5;
	shdr_resize->chmode.f_css_md	= IO_SHDR.CH5MODE.bit.CSSMD5;

	// WOUTSZ.
	shdr_resize->woutsz.a_hsize		= IO_SHDR.WOUTSZ.bit.WOUTHSZ_0;
	shdr_resize->woutsz.a_vsize		= IO_SHDR.WOUTSZ.bit.WOUTVSZ_0;
	shdr_resize->woutsz.b_hsize		= IO_SHDR.WOUTSZ.bit.WOUTHSZ_1;
	shdr_resize->woutsz.b_vsize		= IO_SHDR.WOUTSZ.bit.WOUTVSZ_1;
	shdr_resize->woutsz.c_hsize		= IO_SHDR.WOUTSZ.bit.WOUTHSZ_2;
	shdr_resize->woutsz.c_vsize		= IO_SHDR.WOUTSZ.bit.WOUTVSZ_2;
	shdr_resize->woutsz.d_hsize		= IO_SHDR.WOUTSZ.bit.WOUTHSZ_3;
	shdr_resize->woutsz.d_vsize		= IO_SHDR.WOUTSZ.bit.WOUTVSZ_3;
	shdr_resize->woutsz.e_hsize		= IO_SHDR.WOUTSZ.bit.WOUTHSZ_4;
	shdr_resize->woutsz.e_vsize		= IO_SHDR.WOUTSZ.bit.WOUTVSZ_4;
	shdr_resize->woutsz.f_hsize		= IO_SHDR.WOUTSZ.bit.WOUTHSZ_5;
	shdr_resize->woutsz.f_vsize		= IO_SHDR.WOUTSZ.bit.WOUTVSZ_5;

	// RSZHCNT.
	shdr_resize->rszhcnt.c_hsta		= IO_SHDR.RSZHCNT_2.bit.RSZHSTA;
	shdr_resize->rszhcnt.c_hpit		= IO_SHDR.RSZHCNT_2.bit.RSZHPIT;
	shdr_resize->rszhcnt.d_hsta		= IO_SHDR.RSZHCNT_3.bit.RSZHSTA;
	shdr_resize->rszhcnt.d_hpit		= IO_SHDR.RSZHCNT_3.bit.RSZHPIT;
	shdr_resize->rszhcnt.e_hsta		= IO_SHDR.RSZHCNT_4.bit.RSZHSTA;
	shdr_resize->rszhcnt.e_hpit		= IO_SHDR.RSZHCNT_4.bit.RSZHPIT;
	shdr_resize->rszhcnt.f_hsta		= IO_SHDR.RSZHCNT_5.bit.RSZHSTA;
	shdr_resize->rszhcnt.f_hpit		= IO_SHDR.RSZHCNT_5.bit.RSZHPIT;

	// RSZVCNT.
	shdr_resize->rszvcnt.c_vsta		= IO_SHDR.RSZVCNT_2.bit.RSZVSTA;
	shdr_resize->rszvcnt.c_vpit		= IO_SHDR.RSZVCNT_2.bit.RSZVPIT;
	shdr_resize->rszvcnt.d_vsta		= IO_SHDR.RSZVCNT_3.bit.RSZVSTA;
	shdr_resize->rszvcnt.d_vpit		= IO_SHDR.RSZVCNT_3.bit.RSZVPIT;
	shdr_resize->rszvcnt.e_vsta		= IO_SHDR.RSZVCNT_4.bit.RSZVSTA;
	shdr_resize->rszvcnt.e_vpit		= IO_SHDR.RSZVCNT_4.bit.RSZVPIT;
	shdr_resize->rszvcnt.f_vsta		= IO_SHDR.RSZVCNT_5.bit.RSZVSTA;
	shdr_resize->rszvcnt.f_vpit		= IO_SHDR.RSZVCNT_5.bit.RSZVPIT;

	// TRMSTA.
	shdr_resize->trmsta.c_hsta		= IO_SHDR.TRMSTA_2.bit.TRMHSTA;
	shdr_resize->trmsta.c_vsta		= IO_SHDR.TRMSTA_2.bit.TRMVSTA;
	shdr_resize->trmsta.d_hsta		= IO_SHDR.TRMSTA_3.bit.TRMHSTA;
	shdr_resize->trmsta.d_vsta		= IO_SHDR.TRMSTA_3.bit.TRMVSTA;
	shdr_resize->trmsta.e_hsta		= IO_SHDR.TRMSTA_4.bit.TRMHSTA;
	shdr_resize->trmsta.e_vsta		= IO_SHDR.TRMSTA_4.bit.TRMVSTA;
	shdr_resize->trmsta.f_hsta		= IO_SHDR.TRMSTA_5.bit.TRMHSTA;
	shdr_resize->trmsta.f_vsta		= IO_SHDR.TRMSTA_5.bit.TRMVSTA;

	// DRCOFST.
	shdr_resize->drcofst.a_y		= IO_SHDR.DRCOFST_0.bit.DRCYOFST;
	im_shdr_get_reg_signed( shdr_resize->drcofst.a_cb, IO_SHDR.DRCOFST_0, union io_shdr_drcofst, DRCCBOFST );
	im_shdr_get_reg_signed( shdr_resize->drcofst.a_cr, IO_SHDR.DRCOFST_0, union io_shdr_drcofst, DRCCROFST );

	shdr_resize->drcofst.c_y		= IO_SHDR.DRCOFST_2.bit.DRCYOFST;
	im_shdr_get_reg_signed( shdr_resize->drcofst.c_cb, IO_SHDR.DRCOFST_2, union io_shdr_drcofst, DRCCBOFST );
	im_shdr_get_reg_signed( shdr_resize->drcofst.c_cr, IO_SHDR.DRCOFST_2, union io_shdr_drcofst, DRCCROFST );

	shdr_resize->drcofst.d_y		= IO_SHDR.DRCOFST_3.bit.DRCYOFST;
	im_shdr_get_reg_signed( shdr_resize->drcofst.d_cb, IO_SHDR.DRCOFST_3, union io_shdr_drcofst, DRCCBOFST );
	im_shdr_get_reg_signed( shdr_resize->drcofst.d_cr, IO_SHDR.DRCOFST_3, union io_shdr_drcofst, DRCCROFST );

	// DRCGAIN.
	shdr_resize->drcgain.a_y		= IO_SHDR.DRCGAIN_0.bit.DRCYGAIN;
	shdr_resize->drcgain.a_cb		= IO_SHDR.DRCGAIN_0.bit.DRCCBGAIN;
	shdr_resize->drcgain.a_cr		= IO_SHDR.DRCGAIN_0.bit.DRCCRGAIN;
	shdr_resize->drcgain.c_y		= IO_SHDR.DRCGAIN_2.bit.DRCYGAIN;
	shdr_resize->drcgain.c_cb		= IO_SHDR.DRCGAIN_2.bit.DRCCBGAIN;
	shdr_resize->drcgain.c_cr		= IO_SHDR.DRCGAIN_2.bit.DRCCRGAIN;
	shdr_resize->drcgain.d_y		= IO_SHDR.DRCGAIN_3.bit.DRCYGAIN;
	shdr_resize->drcgain.d_cb		= IO_SHDR.DRCGAIN_3.bit.DRCCBGAIN;
	shdr_resize->drcgain.d_cr		= IO_SHDR.DRCGAIN_3.bit.DRCCRGAIN;

	// DRCCLPH.
	shdr_resize->drcclph.a_y		= IO_SHDR.DRCCLPH_0.bit.DRCYCLPH;
	im_shdr_get_reg_signed( shdr_resize->drcclph.a_cb, IO_SHDR.DRCCLPH_0, union io_shdr_drcclph, DRCCBCLPH );
	im_shdr_get_reg_signed( shdr_resize->drcclph.a_cr, IO_SHDR.DRCCLPH_0, union io_shdr_drcclph, DRCCRCLPH );

	shdr_resize->drcclph.c_y		= IO_SHDR.DRCCLPH_2.bit.DRCYCLPH;
	im_shdr_get_reg_signed( shdr_resize->drcclph.c_cb, IO_SHDR.DRCCLPH_2, union io_shdr_drcclph, DRCCBCLPH );
	im_shdr_get_reg_signed( shdr_resize->drcclph.c_cr, IO_SHDR.DRCCLPH_2, union io_shdr_drcclph, DRCCRCLPH );

	shdr_resize->drcclph.d_y		= IO_SHDR.DRCCLPH_3.bit.DRCYCLPH;
	im_shdr_get_reg_signed( shdr_resize->drcclph.d_cb, IO_SHDR.DRCCLPH_3, union io_shdr_drcclph, DRCCBCLPH );
	im_shdr_get_reg_signed( shdr_resize->drcclph.d_cr, IO_SHDR.DRCCLPH_3, union io_shdr_drcclph, DRCCRCLPH );

	// DRCCLPL.
	shdr_resize->drcclpl.a_y		= IO_SHDR.DRCCLPL_0.bit.DRCYCLPL;
	im_shdr_get_reg_signed( shdr_resize->drcclpl.a_cb, IO_SHDR.DRCCLPL_0, union io_shdr_drcclpl, DRCCBCLPL );
	im_shdr_get_reg_signed( shdr_resize->drcclpl.a_cr, IO_SHDR.DRCCLPL_0, union io_shdr_drcclpl, DRCCRCLPL );

	shdr_resize->drcclpl.c_y		= IO_SHDR.DRCCLPL_2.bit.DRCYCLPL;
	im_shdr_get_reg_signed( shdr_resize->drcclpl.c_cb, IO_SHDR.DRCCLPL_2, union io_shdr_drcclpl, DRCCBCLPL );
	im_shdr_get_reg_signed( shdr_resize->drcclpl.c_cr, IO_SHDR.DRCCLPL_2, union io_shdr_drcclpl, DRCCRCLPL );

	shdr_resize->drcclpl.d_y		= IO_SHDR.DRCCLPL_3.bit.DRCYCLPL;
	im_shdr_get_reg_signed( shdr_resize->drcclpl.d_cb, IO_SHDR.DRCCLPL_3, union io_shdr_drcclpl, DRCCBCLPL );
	im_shdr_get_reg_signed( shdr_resize->drcclpl.d_cr, IO_SHDR.DRCCLPL_3, union io_shdr_drcclpl, DRCCRCLPL );

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR SMC parameter set.<br>
@param[in]		shdr_smc						Setting of SHDR SMC parameters. See @ref T_IM_SHDR_SMC.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Set_Smc( const T_IM_SHDR_SMC* const shdr_smc )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_smc == NULL ) {
		Ddim_Assertion(("Im_SHDR_Set_Smc() error. shdr_smc = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_smc->mcarmd.mca_mode > D_IM_MCAMD_MAX_MIN_VECTOR ) {
		Ddim_Assertion(("Im_SHDR_Set_Smc() error. shdr_smc->mcarmd.mca_mode is ivalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_smc->mcarmd.mcr_mode > D_IM_MCRMD_MC ) {
		Ddim_Assertion(("Im_SHDR_Set_Smc() error. shdr_smc->mcarmd.mcr_mode is ivalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_smc->mcmpmd.mcmp_mode > D_IM_SHDR_ON ) {
		Ddim_Assertion(("Im_SHDR_Set_Smc() error. shdr_smc->mcmpmd.mcmp_mode is ivalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_smc->mcmpmd.mvw_en > D_IM_SHDR_ENABLE ) {
		Ddim_Assertion(("Im_SHDR_Set_Smc() error. shdr_smc->mcmpmd.mvw_en is ivalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_smc->mvlcnt.mvl_en > D_IM_SHDR_ENABLE ) {
		Ddim_Assertion(("Im_SHDR_Set_Smc() error. shdr_smc->mvlcnt.mvl_en is ivalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// MCAMD.
	IO_SHDR.MCARMD.bit.MCAMD	= shdr_smc->mcarmd.mca_mode;
	IO_SHDR.MCARMD.bit.MCRMD	= shdr_smc->mcarmd.mcr_mode;

	// MCMPMD.
	IO_SHDR.MCMPMD.bit.MCMPMD	= shdr_smc->mcmpmd.mcmp_mode;
	IO_SHDR.MCMPMD.bit.MVWEN	= shdr_smc->mcmpmd.mvw_en;

	// MMVVAL.
	IO_SHDR.MMVVAL.bit.MMVX		= shdr_smc->mmvval.mmv_x;
	IO_SHDR.MMVVAL.bit.MMVY		= shdr_smc->mmvval.mmv_y;

	// MMVRCNT.
	IO_SHDR.MMVRCNT.bit.MMVROX	= shdr_smc->mmvrcnt.mmvro_x;
	IO_SHDR.MMVRCNT.bit.MMVROY	= shdr_smc->mmvrcnt.mmvro_y;
	IO_SHDR.MMVRCNT.bit.MMVRKX	= shdr_smc->mmvrcnt.mmvrk_x;
	IO_SHDR.MMVRCNT.bit.MMVRKY	= shdr_smc->mmvrcnt.mmvrk_y;

	// MDBCNT.
	IO_SHDR.MDBCNT.bit.MDBX		= shdr_smc->mdbcnt.mdb_x;
	IO_SHDR.MDBCNT.bit.MDBY		= shdr_smc->mdbcnt.mdb_y;

	// MDBROCNT.
	IO_SHDR.MDBROCNT.bit.MDBROX	= shdr_smc->mdbrocnt.mdbro_x;
	IO_SHDR.MDBROCNT.bit.MDBROY	= shdr_smc->mdbrocnt.mdbro_y;
	IO_SHDR.MDBROCNT.bit.MDBRKX	= shdr_smc->mdbrocnt.mdbrk_x;
	IO_SHDR.MDBROCNT.bit.MDBRKY	= shdr_smc->mdbrocnt.mdbrk_y;

	// MOLRCNT.
	IO_SHDR.MOLRCNT.bit.MOLRSM	= shdr_smc->molrcnt.molr_sm;
	IO_SHDR.MOLRCNT.bit.MOLRSS	= shdr_smc->molrcnt.molr_ss;
	IO_SHDR.MOLRCNT.bit.MOLRO	= shdr_smc->molrcnt.molr_o;
	IO_SHDR.MOLRCNT.bit.MOLRK	= shdr_smc->molrcnt.molr_k;

	// MCPCNT.
	IO_SHDR.MCPCNT.bit.MCPXS	= shdr_smc->mcpcnt.mcpx_s;
	IO_SHDR.MCPCNT.bit.MCPXO	= shdr_smc->mcpcnt.mcpx_o;
	IO_SHDR.MCPCNT.bit.MCPXK	= shdr_smc->mcpcnt.mcpx_k;

	// MMCRCNT.
	IO_SHDR.MMCRCNT.bit.MMCRS	= shdr_smc->mmcrcnt.mmcr_s;
	IO_SHDR.MMCRCNT.bit.MMCRK	= shdr_smc->mmcrcnt.mmcr_k;

	// MOLECNT.
	IO_SHDR.MOLECNT.bit.MOLES	= shdr_smc->molecnt.mole_s;
	IO_SHDR.MOLECNT.bit.MOLEO	= shdr_smc->molecnt.mole_o;
	IO_SHDR.MOLECNT.bit.MOLEK	= shdr_smc->molecnt.mole_k;

	// MMVECNT.
	IO_SHDR.MMVECNT.bit.MMVES	= shdr_smc->mmvecnt.mmve_s;
	IO_SHDR.MMVECNT.bit.MMVEO	= shdr_smc->mmvecnt.mmve_o;
	IO_SHDR.MMVECNT.bit.MMVEK	= shdr_smc->mmvecnt.mmve_k;

	// MCPXECNT.
	IO_SHDR.MCPXECNT.bit.MCPXES	= shdr_smc->mcpxecnt.mcpxe_s;
	IO_SHDR.MCPXECNT.bit.MCPXEO	= shdr_smc->mcpxecnt.mcpxe_o;
	IO_SHDR.MCPXECNT.bit.MCPXEK	= shdr_smc->mcpxecnt.mcpxe_k;

	// MVLCNT.
	IO_SHDR.MVLCNT.bit.MVLEN	= shdr_smc->mvlcnt.mvl_en;
	IO_SHDR.MVLCNT.bit.MVLTH	= shdr_smc->mvlcnt.mvl_threshold;
	IO_SHDR.MVLCNT.bit.MVLCTH	= shdr_smc->mvlcnt.mvl_count_threshold;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR get SMC parameter.<br>
@param[out]		shdr_smc						Setting of SHDR SMC parameters. See @ref T_IM_SHDR_SMC.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Get_Smc( T_IM_SHDR_SMC* const shdr_smc )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_smc == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_Smc() error. shdr_smc = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// MCAMD.
	shdr_smc->mcarmd.mca_mode		= IO_SHDR.MCARMD.bit.MCAMD;
	shdr_smc->mcarmd.mcr_mode		= IO_SHDR.MCARMD.bit.MCRMD;

	// MCMPMD.
	shdr_smc->mcmpmd.mcmp_mode		= IO_SHDR.MCMPMD.bit.MCMPMD;
	shdr_smc->mcmpmd.mvw_en			= IO_SHDR.MCMPMD.bit.MVWEN;

	// MMVVAL.
	shdr_smc->mmvval.mmv_x			= IO_SHDR.MMVVAL.bit.MMVX;
	shdr_smc->mmvval.mmv_y			= IO_SHDR.MMVVAL.bit.MMVY;

	// MMVRCNT.
	shdr_smc->mmvrcnt.mmvro_x		= IO_SHDR.MMVRCNT.bit.MMVROX;
	shdr_smc->mmvrcnt.mmvro_y		= IO_SHDR.MMVRCNT.bit.MMVROY;
	shdr_smc->mmvrcnt.mmvrk_x		= IO_SHDR.MMVRCNT.bit.MMVRKX;
	shdr_smc->mmvrcnt.mmvrk_y		= IO_SHDR.MMVRCNT.bit.MMVRKY;

	// MDBCNT.
	shdr_smc->mdbcnt.mdb_x			= IO_SHDR.MDBCNT.bit.MDBX;
	shdr_smc->mdbcnt.mdb_y			= IO_SHDR.MDBCNT.bit.MDBY;

	// MDBROCNT.
	shdr_smc->mdbrocnt.mdbro_x		= IO_SHDR.MDBROCNT.bit.MDBROX;
	shdr_smc->mdbrocnt.mdbro_y		= IO_SHDR.MDBROCNT.bit.MDBROY;
	shdr_smc->mdbrocnt.mdbrk_x		= IO_SHDR.MDBROCNT.bit.MDBRKX;
	shdr_smc->mdbrocnt.mdbrk_y		= IO_SHDR.MDBROCNT.bit.MDBRKY;

	// MOLRCNT.
	shdr_smc->molrcnt.molr_sm		= IO_SHDR.MOLRCNT.bit.MOLRSM;
	shdr_smc->molrcnt.molr_ss		= IO_SHDR.MOLRCNT.bit.MOLRSS;
	shdr_smc->molrcnt.molr_o		= IO_SHDR.MOLRCNT.bit.MOLRO;
	shdr_smc->molrcnt.molr_k		= IO_SHDR.MOLRCNT.bit.MOLRK;

	// MCPCNT.
	shdr_smc->mcpcnt.mcpx_s			= IO_SHDR.MCPCNT.bit.MCPXS;
	shdr_smc->mcpcnt.mcpx_o			= IO_SHDR.MCPCNT.bit.MCPXO;
	shdr_smc->mcpcnt.mcpx_k			= IO_SHDR.MCPCNT.bit.MCPXK;

	// MMCRCNT.
	shdr_smc->mmcrcnt.mmcr_s		= IO_SHDR.MMCRCNT.bit.MMCRS;
	shdr_smc->mmcrcnt.mmcr_k		= IO_SHDR.MMCRCNT.bit.MMCRK;

	// MOLECNT.
	shdr_smc->molecnt.mole_s		= IO_SHDR.MOLECNT.bit.MOLES;
	shdr_smc->molecnt.mole_o		= IO_SHDR.MOLECNT.bit.MOLEO;
	shdr_smc->molecnt.mole_k		= IO_SHDR.MOLECNT.bit.MOLEK;

	// MMVECNT.
	shdr_smc->mmvecnt.mmve_s		= IO_SHDR.MMVECNT.bit.MMVES;
	shdr_smc->mmvecnt.mmve_o		= IO_SHDR.MMVECNT.bit.MMVEO;
	shdr_smc->mmvecnt.mmve_k		= IO_SHDR.MMVECNT.bit.MMVEK;

	// MCPXECNT.
	shdr_smc->mcpxecnt.mcpxe_s		= IO_SHDR.MCPXECNT.bit.MCPXES;
	shdr_smc->mcpxecnt.mcpxe_o		= IO_SHDR.MCPXECNT.bit.MCPXEO;
	shdr_smc->mcpxecnt.mcpxe_k		= IO_SHDR.MCPXECNT.bit.MCPXEK;

	// MVLCNT.
	shdr_smc->mvlcnt.mvl_en					= IO_SHDR.MVLCNT.bit.MVLEN;
	shdr_smc->mvlcnt.mvl_threshold			= IO_SHDR.MVLCNT.bit.MVLTH;
	shdr_smc->mvlcnt.mvl_count_threshold	= IO_SHDR.MVLCNT.bit.MVLCTH;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR degamma R table set.<br>
@param[in]		src_tbl							Pointer of Table data (Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		write_ofs_num					Write offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
INT32 Im_SHDR_Set_Degamma_R( const USHORT* const src_tbl, const USHORT write_ofs_num, const USHORT array_num )
{
	INT32	ret = D_DDIM_OK;
	UINT32	loop_cnt;

#ifdef CO_PARAM_CHECK
	if ( src_tbl == NULL ) {
		Ddim_Assertion(("Im_SHDR_Set_Degamma_R() error. src_tbl = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( write_ofs_num + array_num ) > D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ) {
		Ddim_Assertion(("Im_SHDR_Set_Degamma_R(). write_ofs_num + array_num > MAX\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	DDIM_User_AhbReg_SpinLock();	// JPEG issue work around.

	im_shdr_on_pclk();
	im_shdr_on_hclk();
	im_shdr_dsb();

	// Check SHDR macro status.
	if ( IO_SHDR.SHDRACTL1.bit.TRG == D_IM_SHDR_TRG_STATUS_STOP ) {
		for ( loop_cnt = 0; loop_cnt < array_num; loop_cnt++ ) {
			IO_SHDR_TBL.DGTR.hword[ write_ofs_num + loop_cnt ] = src_tbl[ loop_cnt ];
		}
	}
	else {
		Ddim_Print(("Im_SHDR_Set_Degamma_R() : Macro busy\n"));
		ret = D_IM_SHDR_MACRO_BUSY;
	}

	im_shdr_off_hclk();
	im_shdr_off_pclk();
	im_shdr_dsb();

	DDIM_User_AhbReg_SpinUnLock();	// JPEG issue work around.

	return ret;
}

/**
SHDR degamma G table set.<br>
@param[in]		src_tbl							Pointer of Table data (Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		write_ofs_num					Write offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
INT32 Im_SHDR_Set_Degamma_G( const USHORT* const src_tbl, const USHORT write_ofs_num, const USHORT array_num )
{
	INT32	ret = D_DDIM_OK;
	UINT32	loop_cnt;

#ifdef CO_PARAM_CHECK
	if ( src_tbl == NULL ) {
		Ddim_Assertion(("Im_SHDR_Set_Degamma_G() error. src_tbl = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( write_ofs_num + array_num ) > D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ) {
		Ddim_Assertion(("Im_SHDR_Set_Degamma_G(). write_ofs_num + array_num > MAX\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	DDIM_User_AhbReg_SpinLock();	// JPEG issue work around.

	im_shdr_on_pclk();
	im_shdr_on_hclk();
	im_shdr_dsb();

	// Check SHDR macro status.
	if ( IO_SHDR.SHDRACTL1.bit.TRG == D_IM_SHDR_TRG_STATUS_STOP ) {
		for ( loop_cnt = 0; loop_cnt < array_num; loop_cnt++ ) {
			IO_SHDR_TBL.DGTG.hword[ write_ofs_num + loop_cnt ] = src_tbl[ loop_cnt ];
		}
	}
	else {
		Ddim_Print(("Im_SHDR_Set_Degamma_G() : Macro busy\n"));
		ret = D_IM_SHDR_MACRO_BUSY;
	}

	im_shdr_off_hclk();
	im_shdr_off_pclk();
	im_shdr_dsb();

	DDIM_User_AhbReg_SpinUnLock();	// JPEG issue work around.

	return ret;
}

/**
SHDR degamma B table set.<br>
@param[in]		src_tbl							Pointer of Table data (Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		write_ofs_num					Write offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
INT32 Im_SHDR_Set_Degamma_B( const USHORT* const src_tbl, const USHORT write_ofs_num, const USHORT array_num )
{
	INT32	ret = D_DDIM_OK;
	UINT32	loop_cnt;

#ifdef CO_PARAM_CHECK
	if ( src_tbl == NULL ) {
		Ddim_Assertion(("Im_SHDR_Set_Degamma_B() error. src_tbl = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( write_ofs_num + array_num ) > D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ) {
		Ddim_Assertion(("Im_SHDR_Set_Degamma_B(). write_ofs_num + array_num > MAX\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	DDIM_User_AhbReg_SpinLock();	// JPEG issue work around.

	im_shdr_on_pclk();
	im_shdr_on_hclk();
	im_shdr_dsb();

	// Check SHDR macro status.
	if ( IO_SHDR.SHDRACTL1.bit.TRG == D_IM_SHDR_TRG_STATUS_STOP ) {
		for ( loop_cnt = 0; loop_cnt < array_num; loop_cnt++ ) {
			IO_SHDR_TBL.DGTB.hword[ write_ofs_num + loop_cnt ] = src_tbl[ loop_cnt ];
		}
	}
	else {
		Ddim_Print(("Im_SHDR_Set_Degamma_B() : Macro busy\n"));
		ret = D_IM_SHDR_MACRO_BUSY;
	}

	im_shdr_off_hclk();
	im_shdr_off_pclk();
	im_shdr_dsb();

	DDIM_User_AhbReg_SpinUnLock();	// JPEG issue work around.

	return ret;
}

/**
SHDR degamma table set.<br>
@param[in]		src_tbl_r						Pointer of Table data R(Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		src_tbl_g						Pointer of Table data G(Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		src_tbl_b						Pointer of Table data B(Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		write_ofs_num					Write offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
INT32 Im_SHDR_Set_Degamma( const USHORT* const src_tbl_r, const USHORT* const src_tbl_g, const USHORT* const src_tbl_b, const USHORT write_ofs_num, const USHORT array_num )
{
	INT32	ret = D_DDIM_OK;
	UINT32	loop_cnt;

#ifdef CO_PARAM_CHECK
	if ( ( src_tbl_r == NULL ) || ( src_tbl_g == NULL ) || ( src_tbl_b == NULL ) ) {
		Ddim_Assertion(("Im_SHDR_Set_Degamma() error. src_tbl_x = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( write_ofs_num + array_num ) > D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ) {
		Ddim_Assertion(("Im_SHDR_Set_Degamma(). write_ofs_num + array_num > MAX\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	DDIM_User_AhbReg_SpinLock();	// JPEG issue work around.

	im_shdr_on_pclk();
	im_shdr_on_hclk();
	im_shdr_dsb();

	// Check SHDR macro status.
	if ( IO_SHDR.SHDRACTL1.bit.TRG == D_IM_SHDR_TRG_STATUS_STOP ) {

		for ( loop_cnt = 0; loop_cnt < array_num; loop_cnt++ ) {
			IO_SHDR_TBL.DGTR.hword[ write_ofs_num + loop_cnt ] = src_tbl_r[ loop_cnt ];
			IO_SHDR_TBL.DGTG.hword[ write_ofs_num + loop_cnt ] = src_tbl_g[ loop_cnt ];
			IO_SHDR_TBL.DGTB.hword[ write_ofs_num + loop_cnt ] = src_tbl_b[ loop_cnt ];
		}
	}
	else {
		Ddim_Print(("Im_SHDR_Set_Degamma() : Macro busy\n"));
		ret = D_IM_SHDR_MACRO_BUSY;
	}

	im_shdr_off_hclk();
	im_shdr_off_pclk();
	im_shdr_dsb();

	DDIM_User_AhbReg_SpinUnLock();	// JPEG issue work around.

	return ret;
}

/**
SHDR get degamma R table.<br>
@param[out]		dst_tbl							Pointer of Table data (Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		read_ofs_num					Read offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		read_sel						Degamma table read selector.<br>
													<ul><li>@ref D_IM_SHDR_DGTRSEL_PREFL.
														<li>@ref D_IM_SHDR_DGTRSEL_PREFS.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_ODD.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_ODD.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
INT32 Im_SHDR_Get_Degamma_R( USHORT* const dst_tbl, const USHORT read_ofs_num, const USHORT array_num, const UCHAR read_sel )
{
	INT32	ret = D_DDIM_OK;
	UINT32	loop_cnt;

#ifdef CO_PARAM_CHECK
	if ( dst_tbl == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_Degamma_R() error. dst_tbl = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( read_ofs_num + array_num ) > D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ) {
		Ddim_Assertion(("Im_SHDR_Get_Degamma_R(). read_ofs_num + array_num > MAX\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( read_sel > D_IM_SHDR_DGTRSEL_PREMBS_ODD ) {
		Ddim_Assertion(("Im_SHDR_Get_Degamma_R(). read_sel is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	DDIM_User_AhbReg_SpinLock();	// JPEG issue work around.

	im_shdr_on_pclk();
	im_shdr_on_hclk();
	im_shdr_dsb();

	// Check SHDR macro status.
	if ( IO_SHDR.SHDRACTL1.bit.TRG == D_IM_SHDR_TRG_STATUS_STOP ) {

		// DGT read selector set.
		IO_SHDR.DGT_RSEL.bit.DGT_RSEL = read_sel;

		for ( loop_cnt = 0; loop_cnt < array_num; loop_cnt++ ) {
			 dst_tbl[ loop_cnt ] = IO_SHDR_TBL.DGTR.hword[ read_ofs_num + loop_cnt ];
		}
	}
	else {
		Ddim_Print(("Im_SHDR_Get_Degamma_R() : Macro busy\n"));
		ret = D_IM_SHDR_MACRO_BUSY;
	}

	im_shdr_off_hclk();
	im_shdr_off_pclk();
	im_shdr_dsb();

	DDIM_User_AhbReg_SpinUnLock();	// JPEG issue work around.

	return ret;
}

/**
SHDR get degamma G table.<br>
@param[out]		dst_tbl							Pointer of Table data (Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		read_ofs_num					Read offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		read_sel						Degamma table read selector.<br>
													<ul><li>@ref D_IM_SHDR_DGTRSEL_PREFL.
														<li>@ref D_IM_SHDR_DGTRSEL_PREFS.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_ODD.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_ODD.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
INT32 Im_SHDR_Get_Degamma_G( USHORT* const dst_tbl, const USHORT read_ofs_num, const USHORT array_num, const UCHAR read_sel )
{
	INT32	ret = D_DDIM_OK;
	UINT32	loop_cnt;

#ifdef CO_PARAM_CHECK
	if ( dst_tbl == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_Degamma_G() error. dst_tbl = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( read_ofs_num + array_num ) > D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ) {
		Ddim_Assertion(("Im_SHDR_Get_Degamma_G(). read_ofs_num + array_num > MAX\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( read_sel > D_IM_SHDR_DGTRSEL_PREMBS_ODD ) {
		Ddim_Assertion(("Im_SHDR_Get_Degamma_G(). read_sel is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	DDIM_User_AhbReg_SpinLock();	// JPEG issue work around.

	im_shdr_on_pclk();
	im_shdr_on_hclk();
	im_shdr_dsb();

	// Check SHDR macro status.
	if ( IO_SHDR.SHDRACTL1.bit.TRG == D_IM_SHDR_TRG_STATUS_STOP ) {

		// DGT read selector set.
		IO_SHDR.DGT_RSEL.bit.DGT_RSEL = read_sel;

		for ( loop_cnt = 0; loop_cnt < array_num; loop_cnt++ ) {
			 dst_tbl[ loop_cnt ] = IO_SHDR_TBL.DGTG.hword[ read_ofs_num + loop_cnt ];
		}
	}
	else {
		Ddim_Print(("Im_SHDR_Get_Degamma_G() : Macro busy\n"));
		ret = D_IM_SHDR_MACRO_BUSY;
	}

	im_shdr_off_hclk();
	im_shdr_off_pclk();
	im_shdr_dsb();

	DDIM_User_AhbReg_SpinUnLock();	// JPEG issue work around.

	return ret;
}

/**
SHDR get degamma B table.<br>
@param[out]		dst_tbl							Pointer of Table data (Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		read_ofs_num					Read offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		read_sel						Degamma table read selector.<br>
													<ul><li>@ref D_IM_SHDR_DGTRSEL_PREFL.
														<li>@ref D_IM_SHDR_DGTRSEL_PREFS.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_ODD.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_ODD.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
INT32 Im_SHDR_Get_Degamma_B( USHORT* const dst_tbl, const USHORT read_ofs_num, const USHORT array_num, const UCHAR read_sel )
{
	INT32	ret = D_DDIM_OK;
	UINT32	loop_cnt;

#ifdef CO_PARAM_CHECK
	if ( dst_tbl == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_Degamma_B() error. dst_tbl = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( read_ofs_num + array_num ) > D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ) {
		Ddim_Assertion(("Im_SHDR_Get_Degamma_B(). read_ofs_num + array_num > MAX\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( read_sel > D_IM_SHDR_DGTRSEL_PREMBS_ODD ) {
		Ddim_Assertion(("Im_SHDR_Get_Degamma_B(). read_sel is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	DDIM_User_AhbReg_SpinLock();	// JPEG issue work around.

	im_shdr_on_pclk();
	im_shdr_on_hclk();
	im_shdr_dsb();

	// Check SHDR macro status.
	if ( IO_SHDR.SHDRACTL1.bit.TRG == D_IM_SHDR_TRG_STATUS_STOP ) {

		// DGT read selector set.
		IO_SHDR.DGT_RSEL.bit.DGT_RSEL = read_sel;

		for ( loop_cnt = 0; loop_cnt < array_num; loop_cnt++ ) {
			 dst_tbl[ loop_cnt ] = IO_SHDR_TBL.DGTB.hword[ read_ofs_num + loop_cnt ];
		}
	}
	else {
		Ddim_Print(("Im_SHDR_Get_Degamma_B() : Macro busy\n"));
		ret = D_IM_SHDR_MACRO_BUSY;
	}

	im_shdr_off_hclk();
	im_shdr_off_pclk();
	im_shdr_dsb();

	DDIM_User_AhbReg_SpinUnLock();	// JPEG issue work around.

	return ret;
}

/**
SHDR get degamma table.<br>
@param[out]		dst_tbl_r						Pointer of Table data R(Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[out]		dst_tbl_g						Pointer of Table data G(Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[out]		dst_tbl_b						Pointer of Table data B(Type of USHORT, Maximum array number is D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE).<br>
@param[in]		read_ofs_num					Read offset num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		array_num						Num of table constituent(USHORT data num, must set multiple of 2).<br>
@param[in]		read_sel						Degamma table read selector.<br>
													<ul><li>@ref D_IM_SHDR_DGTRSEL_PREFL.
														<li>@ref D_IM_SHDR_DGTRSEL_PREFS.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBL_ODD.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_EVEN.
														<li>@ref D_IM_SHDR_DGTRSEL_PREMBS_ODD.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
INT32 Im_SHDR_Get_Degamma( USHORT* const dst_tbl_r, USHORT* const dst_tbl_g, USHORT* const dst_tbl_b, const USHORT read_ofs_num, const USHORT array_num, const UCHAR read_sel )
{
	INT32	ret = D_DDIM_OK;
	UINT32	loop_cnt;

#ifdef CO_PARAM_CHECK
	if ( ( dst_tbl_r == NULL ) || ( dst_tbl_g == NULL ) || ( dst_tbl_b == NULL ) ) {
		Ddim_Assertion(("Im_SHDR_Get_Degamma() error. dst_tbl_x = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( read_ofs_num + array_num ) > D_IM_SHDR_TABLE_MAX_DEGAMMA_TABLE ) {
		Ddim_Assertion(("Im_SHDR_Get_Degamma(). read_ofs_num + array_num > MAX\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( read_sel > D_IM_SHDR_DGTRSEL_PREMBS_ODD ) {
		Ddim_Assertion(("Im_SHDR_Get_Degamma(). read_sel is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	DDIM_User_AhbReg_SpinLock();	// JPEG issue work around.

	im_shdr_on_pclk();
	im_shdr_on_hclk();
	im_shdr_dsb();

	// Check SHDR macro status.
	if ( IO_SHDR.SHDRACTL1.bit.TRG == D_IM_SHDR_TRG_STATUS_STOP ) {

		// DGT read selector set.
		IO_SHDR.DGT_RSEL.bit.DGT_RSEL = read_sel;

		for ( loop_cnt = 0; loop_cnt < array_num; loop_cnt++ ) {
			 dst_tbl_r[ loop_cnt ] = IO_SHDR_TBL.DGTR.hword[ read_ofs_num + loop_cnt ];
			 dst_tbl_g[ loop_cnt ] = IO_SHDR_TBL.DGTG.hword[ read_ofs_num + loop_cnt ];
			 dst_tbl_b[ loop_cnt ] = IO_SHDR_TBL.DGTB.hword[ read_ofs_num + loop_cnt ];
		}
	}
	else {
		Ddim_Print(("Im_SHDR_Get_Degamma() : Macro busy\n"));
		ret = D_IM_SHDR_MACRO_BUSY;
	}

	im_shdr_off_hclk();
	im_shdr_off_pclk();
	im_shdr_dsb();

	DDIM_User_AhbReg_SpinUnLock();	// JPEG issue work around.

	return ret;
}

/**
SHDR AXI parameter set.<br>
@param[in]		shdr_axi						Setting of SHDR AXI parameters. See @ref T_IM_SHDR_AXI.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Set_Axi( const T_IM_SHDR_AXI* const shdr_axi )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_axi == NULL ) {
		Ddim_Assertion(("Im_SHDR_Set_Axi() error. shdr_axi = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( ( shdr_axi->ax_mode.r_burst > D_IM_SHDR_BURSTLEN_256 ) ||
		 ( shdr_axi->ax_mode.w_burst > D_IM_SHDR_BURSTLEN_256 ) ) {
		Ddim_Assertion(("Im_SHDR_Set_Axi() error. r/w_burst is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
	if ( shdr_axi->ax_mode.w_w_mask > D_IM_SHDR_WWMASK_OFF ) {
		Ddim_Assertion(("Im_SHDR_Set_Axi() error. w_w_mask is invalid\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// AXMD.
	IO_SHDR.AXMD.bit.BURSTR				= shdr_axi->ax_mode.r_burst;	// RFID = B/C/D.
	IO_SHDR.AXMD.bit.BURSTW				= shdr_axi->ax_mode.w_burst;	// WFID = B/C/D/E/F/G.
	IO_SHDR.AXMD.bit.WSTBMSK			= shdr_axi->ax_mode.w_w_mask;	// WFID = B/C/D/E/F/G.

	// AXICTLAR.
	// A.
	IO_SHDR.AXICTLARA.bit.ARCACHEA_0	= shdr_axi->r_ctl.a_cahche[ 0 ];
	IO_SHDR.AXICTLARA.bit.ARCACHEA_1	= shdr_axi->r_ctl.a_cahche[ 1 ];
	IO_SHDR.AXICTLARA.bit.ARPROTA_0		= shdr_axi->r_ctl.a_protect[ 0 ];
	IO_SHDR.AXICTLARA.bit.ARPROTA_1		= shdr_axi->r_ctl.a_protect[ 1 ];

	// B.
	IO_SHDR.AXICTLARB.bit.ARCACHEB_0	= shdr_axi->r_ctl.b_cahche[ 0 ];
	IO_SHDR.AXICTLARB.bit.ARCACHEB_1	= shdr_axi->r_ctl.b_cahche[ 1 ];
	IO_SHDR.AXICTLARB.bit.ARPROTB_0		= shdr_axi->r_ctl.b_protect[ 0 ];
	IO_SHDR.AXICTLARB.bit.ARPROTB_1		= shdr_axi->r_ctl.b_protect[ 1 ];

	// C.
	IO_SHDR.AXICTLARC.bit.ARCACHEC_0	= shdr_axi->r_ctl.c_cahche[ 0 ];
	IO_SHDR.AXICTLARC.bit.ARCACHEC_1	= shdr_axi->r_ctl.c_cahche[ 1 ];
	IO_SHDR.AXICTLARC.bit.ARCACHEC_2	= shdr_axi->r_ctl.c_cahche[ 2 ];
	IO_SHDR.AXICTLARC.bit.ARCACHEC_3	= shdr_axi->r_ctl.c_cahche[ 3 ];
	IO_SHDR.AXICTLARC.bit.ARPROTC_0		= shdr_axi->r_ctl.c_protect[ 0 ];
	IO_SHDR.AXICTLARC.bit.ARPROTC_1		= shdr_axi->r_ctl.c_protect[ 1 ];
	IO_SHDR.AXICTLARC.bit.ARPROTC_2		= shdr_axi->r_ctl.c_protect[ 2 ];
	IO_SHDR.AXICTLARC.bit.ARPROTC_3		= shdr_axi->r_ctl.c_protect[ 3 ];

	// D.
	IO_SHDR.AXICTLARD.bit.ARCACHED_0	= shdr_axi->r_ctl.d_cahche[ 0 ];
	IO_SHDR.AXICTLARD.bit.ARCACHED_1	= shdr_axi->r_ctl.d_cahche[ 1 ];
	IO_SHDR.AXICTLARD.bit.ARPROTD_0		= shdr_axi->r_ctl.d_protect[ 0 ];
	IO_SHDR.AXICTLARD.bit.ARPROTD_1		= shdr_axi->r_ctl.d_protect[ 1 ];

	// AXICTLAW.
	// A.
	IO_SHDR.AXICTLAWA.bit.AWCACHE_0		= shdr_axi->w_ctl.a_cahche[ 0 ];
	IO_SHDR.AXICTLAWA.bit.AWCACHE_1		= shdr_axi->w_ctl.a_cahche[ 1 ];
	IO_SHDR.AXICTLAWA.bit.AWCACHE_2		= shdr_axi->w_ctl.a_cahche[ 2 ];
	IO_SHDR.AXICTLAWA.bit.AWCACHE_3		= shdr_axi->w_ctl.a_cahche[ 3 ];
	IO_SHDR.AXICTLAWA.bit.AWPROT_0		= shdr_axi->w_ctl.a_protect[ 0 ];
	IO_SHDR.AXICTLAWA.bit.AWPROT_1		= shdr_axi->w_ctl.a_protect[ 1 ];
	IO_SHDR.AXICTLAWA.bit.AWPROT_2		= shdr_axi->w_ctl.a_protect[ 2 ];
	IO_SHDR.AXICTLAWA.bit.AWPROT_3		= shdr_axi->w_ctl.a_protect[ 3 ];

	// B.
	IO_SHDR.AXICTLAWB.bit.AWCACHE_0		= shdr_axi->w_ctl.b_cahche[ 0 ];
	IO_SHDR.AXICTLAWB.bit.AWCACHE_1		= shdr_axi->w_ctl.b_cahche[ 1 ];
	IO_SHDR.AXICTLAWB.bit.AWCACHE_2		= shdr_axi->w_ctl.b_cahche[ 2 ];
	IO_SHDR.AXICTLAWB.bit.AWCACHE_3		= shdr_axi->w_ctl.b_cahche[ 3 ];
	IO_SHDR.AXICTLAWB.bit.AWCACHE_4		= shdr_axi->w_ctl.b_cahche[ 4 ];
	IO_SHDR.AXICTLAWB.bit.AWCACHE_5		= shdr_axi->w_ctl.b_cahche[ 5 ];
	IO_SHDR.AXICTLAWB.bit.AWPROT_0		= shdr_axi->w_ctl.b_protect[ 0 ];
	IO_SHDR.AXICTLAWB.bit.AWPROT_1		= shdr_axi->w_ctl.b_protect[ 1 ];
	IO_SHDR.AXICTLAWB.bit.AWPROT_2		= shdr_axi->w_ctl.b_protect[ 2 ];
	IO_SHDR.AXICTLAWB.bit.AWPROT_3		= shdr_axi->w_ctl.b_protect[ 3 ];
	IO_SHDR.AXICTLAWB.bit.AWPROT_4		= shdr_axi->w_ctl.b_protect[ 4 ];
	IO_SHDR.AXICTLAWB.bit.AWPROT_5		= shdr_axi->w_ctl.b_protect[ 5 ];

	// C.
	IO_SHDR.AXICTLAWC.bit.AWCACHE_0		= shdr_axi->w_ctl.c_cahche[ 0 ];
	IO_SHDR.AXICTLAWC.bit.AWCACHE_1		= shdr_axi->w_ctl.c_cahche[ 1 ];
	IO_SHDR.AXICTLAWC.bit.AWCACHE_2		= shdr_axi->w_ctl.c_cahche[ 2 ];
	IO_SHDR.AXICTLAWC.bit.AWCACHE_3		= shdr_axi->w_ctl.c_cahche[ 3 ];
	IO_SHDR.AXICTLAWC.bit.AWCACHE_4		= shdr_axi->w_ctl.c_cahche[ 4 ];
	IO_SHDR.AXICTLAWC.bit.AWCACHE_5		= shdr_axi->w_ctl.c_cahche[ 5 ];
	IO_SHDR.AXICTLAWC.bit.AWCACHE_6		= shdr_axi->w_ctl.c_cahche[ 6 ];
	IO_SHDR.AXICTLAWC.bit.AWPROT_0		= shdr_axi->w_ctl.c_protect[ 0 ];
	IO_SHDR.AXICTLAWC.bit.AWPROT_1		= shdr_axi->w_ctl.c_protect[ 1 ];
	IO_SHDR.AXICTLAWC.bit.AWPROT_2		= shdr_axi->w_ctl.c_protect[ 2 ];
	IO_SHDR.AXICTLAWC.bit.AWPROT_3		= shdr_axi->w_ctl.c_protect[ 3 ];
	IO_SHDR.AXICTLAWC.bit.AWPROT_4		= shdr_axi->w_ctl.c_protect[ 4 ];
	IO_SHDR.AXICTLAWC.bit.AWPROT_5		= shdr_axi->w_ctl.c_protect[ 5 ];
	IO_SHDR.AXICTLAWC.bit.AWPROT_6		= shdr_axi->w_ctl.c_protect[ 6 ];

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR get AXI parameter.<br>
@param[out]		shdr_axi						Setting of SHDR AXI parameters. See @ref T_IM_SHDR_AXI.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Get_Axi( T_IM_SHDR_AXI* const shdr_axi )
{
#ifdef CO_PARAM_CHECK
	if ( shdr_axi == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_Axi() error. shdr_axi = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// AXMD.
	shdr_axi->ax_mode.r_burst		= IO_SHDR.AXMD.bit.BURSTR;
	shdr_axi->ax_mode.w_burst		= IO_SHDR.AXMD.bit.BURSTW;
	shdr_axi->ax_mode.w_w_mask		= IO_SHDR.AXMD.bit.WSTBMSK;

	// AXICTLAR.
	// A.
	shdr_axi->r_ctl.a_cahche[ 0 ]	= IO_SHDR.AXICTLARA.bit.ARCACHEA_0;
	shdr_axi->r_ctl.a_cahche[ 1 ]	= IO_SHDR.AXICTLARA.bit.ARCACHEA_1;
	shdr_axi->r_ctl.a_protect[ 0 ]	= IO_SHDR.AXICTLARA.bit.ARPROTA_0;
	shdr_axi->r_ctl.a_protect[ 1 ]	= IO_SHDR.AXICTLARA.bit.ARPROTA_1;

	// B.
	shdr_axi->r_ctl.b_cahche[ 0 ]	= IO_SHDR.AXICTLARB.bit.ARCACHEB_0;
	shdr_axi->r_ctl.b_cahche[ 1 ]	= IO_SHDR.AXICTLARB.bit.ARCACHEB_1;
	shdr_axi->r_ctl.b_protect[ 0 ]	= IO_SHDR.AXICTLARB.bit.ARPROTB_0;
	shdr_axi->r_ctl.b_protect[ 1 ]	= IO_SHDR.AXICTLARB.bit.ARPROTB_1;

	// C.
	shdr_axi->r_ctl.c_cahche[ 0 ]	= IO_SHDR.AXICTLARC.bit.ARCACHEC_0;
	shdr_axi->r_ctl.c_cahche[ 1 ]	= IO_SHDR.AXICTLARC.bit.ARCACHEC_1;
	shdr_axi->r_ctl.c_cahche[ 2 ]	= IO_SHDR.AXICTLARC.bit.ARCACHEC_2;
	shdr_axi->r_ctl.c_cahche[ 3 ]	= IO_SHDR.AXICTLARC.bit.ARCACHEC_3;
	shdr_axi->r_ctl.c_protect[ 0 ]	= IO_SHDR.AXICTLARC.bit.ARPROTC_0;
	shdr_axi->r_ctl.c_protect[ 1 ]	= IO_SHDR.AXICTLARC.bit.ARPROTC_1;
	shdr_axi->r_ctl.c_protect[ 2 ]	= IO_SHDR.AXICTLARC.bit.ARPROTC_2;
	shdr_axi->r_ctl.c_protect[ 3 ]	= IO_SHDR.AXICTLARC.bit.ARPROTC_3;

	// D.
	shdr_axi->r_ctl.d_cahche[ 0 ]	= IO_SHDR.AXICTLARD.bit.ARCACHED_0;
	shdr_axi->r_ctl.d_cahche[ 1 ]	= IO_SHDR.AXICTLARD.bit.ARCACHED_1;
	shdr_axi->r_ctl.d_protect[ 0 ]	= IO_SHDR.AXICTLARD.bit.ARPROTD_0;
	shdr_axi->r_ctl.d_protect[ 1 ]	= IO_SHDR.AXICTLARD.bit.ARPROTD_1;

	// AXICTLAW.
	// A.
	shdr_axi->w_ctl.a_cahche[ 0 ]	= IO_SHDR.AXICTLAWA.bit.AWCACHE_0;
	shdr_axi->w_ctl.a_cahche[ 1 ]	= IO_SHDR.AXICTLAWA.bit.AWCACHE_1;
	shdr_axi->w_ctl.a_cahche[ 2 ]	= IO_SHDR.AXICTLAWA.bit.AWCACHE_2;
	shdr_axi->w_ctl.a_cahche[ 3 ]	= IO_SHDR.AXICTLAWA.bit.AWCACHE_3;
	shdr_axi->w_ctl.a_protect[ 0 ]	= IO_SHDR.AXICTLAWA.bit.AWPROT_0;
	shdr_axi->w_ctl.a_protect[ 1 ]	= IO_SHDR.AXICTLAWA.bit.AWPROT_1;
	shdr_axi->w_ctl.a_protect[ 2 ]	= IO_SHDR.AXICTLAWA.bit.AWPROT_2;
	shdr_axi->w_ctl.a_protect[ 3 ]	= IO_SHDR.AXICTLAWA.bit.AWPROT_3;

	// B.
	shdr_axi->w_ctl.b_cahche[ 0 ]	= IO_SHDR.AXICTLAWB.bit.AWCACHE_0;
	shdr_axi->w_ctl.b_cahche[ 1 ]	= IO_SHDR.AXICTLAWB.bit.AWCACHE_1;
	shdr_axi->w_ctl.b_cahche[ 2 ]	= IO_SHDR.AXICTLAWB.bit.AWCACHE_2;
	shdr_axi->w_ctl.b_cahche[ 3 ]	= IO_SHDR.AXICTLAWB.bit.AWCACHE_3;
	shdr_axi->w_ctl.b_cahche[ 4 ]	= IO_SHDR.AXICTLAWB.bit.AWCACHE_4;
	shdr_axi->w_ctl.b_cahche[ 5 ]	= IO_SHDR.AXICTLAWB.bit.AWCACHE_5;
	shdr_axi->w_ctl.b_protect[ 0 ]	= IO_SHDR.AXICTLAWB.bit.AWPROT_0;
	shdr_axi->w_ctl.b_protect[ 1 ]	= IO_SHDR.AXICTLAWB.bit.AWPROT_1;
	shdr_axi->w_ctl.b_protect[ 2 ]	= IO_SHDR.AXICTLAWB.bit.AWPROT_2;
	shdr_axi->w_ctl.b_protect[ 3 ]	= IO_SHDR.AXICTLAWB.bit.AWPROT_3;
	shdr_axi->w_ctl.b_protect[ 4 ]	= IO_SHDR.AXICTLAWB.bit.AWPROT_4;
	shdr_axi->w_ctl.b_protect[ 5 ]	= IO_SHDR.AXICTLAWB.bit.AWPROT_5;

	// C.
	shdr_axi->w_ctl.c_cahche[ 0 ]	= IO_SHDR.AXICTLAWC.bit.AWCACHE_0;
	shdr_axi->w_ctl.c_cahche[ 1 ]	= IO_SHDR.AXICTLAWC.bit.AWCACHE_1;
	shdr_axi->w_ctl.c_cahche[ 2 ]	= IO_SHDR.AXICTLAWC.bit.AWCACHE_2;
	shdr_axi->w_ctl.c_cahche[ 3 ]	= IO_SHDR.AXICTLAWC.bit.AWCACHE_3;
	shdr_axi->w_ctl.c_cahche[ 4 ]	= IO_SHDR.AXICTLAWC.bit.AWCACHE_4;
	shdr_axi->w_ctl.c_cahche[ 5 ]	= IO_SHDR.AXICTLAWC.bit.AWCACHE_5;
	shdr_axi->w_ctl.c_cahche[ 6 ]	= IO_SHDR.AXICTLAWC.bit.AWCACHE_6;
	shdr_axi->w_ctl.c_protect[ 0 ]	= IO_SHDR.AXICTLAWC.bit.AWPROT_0;
	shdr_axi->w_ctl.c_protect[ 1 ]	= IO_SHDR.AXICTLAWC.bit.AWPROT_1;
	shdr_axi->w_ctl.c_protect[ 2 ]	= IO_SHDR.AXICTLAWC.bit.AWPROT_2;
	shdr_axi->w_ctl.c_protect[ 3 ]	= IO_SHDR.AXICTLAWC.bit.AWPROT_3;
	shdr_axi->w_ctl.c_protect[ 4 ]	= IO_SHDR.AXICTLAWC.bit.AWPROT_4;
	shdr_axi->w_ctl.c_protect[ 5 ]	= IO_SHDR.AXICTLAWC.bit.AWPROT_5;
	shdr_axi->w_ctl.c_protect[ 6 ]	= IO_SHDR.AXICTLAWC.bit.AWPROT_6;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR get AXI response.<br>
@param[out]		axi_resp						AXI response. See @ref T_IM_SHDR_AXI_RESP.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
*/
INT32 Im_SHDR_Get_Axi_Response( T_IM_SHDR_AXI_RESP* const axi_resp )
{
#ifdef CO_PARAM_CHECK
	if ( axi_resp == NULL ) {
		Ddim_Assertion(("Im_SHDR_Get_Axi_Response() error. axi_resp = NULL\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	im_shdr_on_pclk();
	im_shdr_dsb();

	// Read channel response (A).
	axi_resp->r_resp_a[ 0 ] = IO_SHDR.AXICTLRA.bit.RRESPA_0;
	axi_resp->r_resp_a[ 1 ] = IO_SHDR.AXICTLRA.bit.RRESPA_1;

	// Read channel response (B).
	axi_resp->r_resp_b[ 0 ] = IO_SHDR.AXICTLRB.bit.RRESPB_0;
	axi_resp->r_resp_b[ 1 ] = IO_SHDR.AXICTLRB.bit.RRESPB_1;

	// Read channel response (C).
	axi_resp->r_resp_c[ 0 ] = IO_SHDR.AXICTLRC.bit.RRESPC_0;
	axi_resp->r_resp_c[ 1 ] = IO_SHDR.AXICTLRC.bit.RRESPC_1;
	axi_resp->r_resp_c[ 2 ] = IO_SHDR.AXICTLRC.bit.RRESPC_2;
	axi_resp->r_resp_c[ 3 ] = IO_SHDR.AXICTLRC.bit.RRESPC_3;

	// Read channel response (D).
	axi_resp->r_resp_d[ 0 ] = IO_SHDR.AXICTLRD.bit.RRESPD_0;
	axi_resp->r_resp_d[ 1 ] = IO_SHDR.AXICTLRD.bit.RRESPD_1;

	// Write channel response (A).
	axi_resp->w_resp_a[ 0 ] = IO_SHDR.AXICTLWA.bit.BRESPA_0;
	axi_resp->w_resp_a[ 1 ] = IO_SHDR.AXICTLWA.bit.BRESPA_1;
	axi_resp->w_resp_a[ 2 ] = IO_SHDR.AXICTLWA.bit.BRESPA_2;
	axi_resp->w_resp_a[ 3 ] = IO_SHDR.AXICTLWA.bit.BRESPA_3;

	// Write channel response (B).
	axi_resp->w_resp_b[ 0 ] = IO_SHDR.AXICTLWB.bit.BRESPB_0;
	axi_resp->w_resp_b[ 1 ] = IO_SHDR.AXICTLWB.bit.BRESPB_1;
	axi_resp->w_resp_b[ 2 ] = IO_SHDR.AXICTLWB.bit.BRESPB_2;
	axi_resp->w_resp_b[ 3 ] = IO_SHDR.AXICTLWB.bit.BRESPB_3;
	axi_resp->w_resp_b[ 4 ] = IO_SHDR.AXICTLWB.bit.BRESPB_4;
	axi_resp->w_resp_b[ 5 ] = IO_SHDR.AXICTLWB.bit.BRESPB_5;

	// Write channel response (C).
	axi_resp->w_resp_c[ 0 ] = IO_SHDR.AXICTLWC.bit.BRESPC_0;
	axi_resp->w_resp_c[ 1 ] = IO_SHDR.AXICTLWC.bit.BRESPC_1;
	axi_resp->w_resp_c[ 2 ] = IO_SHDR.AXICTLWC.bit.BRESPC_2;
	axi_resp->w_resp_c[ 3 ] = IO_SHDR.AXICTLWC.bit.BRESPC_3;
	axi_resp->w_resp_c[ 4 ] = IO_SHDR.AXICTLWC.bit.BRESPC_4;
	axi_resp->w_resp_c[ 5 ] = IO_SHDR.AXICTLWC.bit.BRESPC_5;
	axi_resp->w_resp_c[ 6 ] = IO_SHDR.AXICTLWC.bit.BRESPC_6;

	im_shdr_off_pclk();
	im_shdr_dsb();

	return D_DDIM_OK;
}

/**
SHDR start.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
*/
INT32 Im_SHDR_Start( VOID )
{
	INT32					ret = D_DDIM_OK;
	union io_shdr_shdrint1	shdrint1;

	im_shdr_on_pclk();
	im_shdr_dsb();

	im_shdr_lock();

	// Check SHDR macro status.
	if ( IO_SHDR.SHDRACTL1.bit.TRG == D_IM_SHDR_TRG_STATUS_STOP ) {

		(VOID)DDIM_User_Clr_Flg( FID_IM_SHDR, ~( D_IM_SHDR_FLG_END | D_IM_SHDR_FLG_STOP ) );

		// Interrupt enable.
		IO_SHDR.SHDRINT0.bit.AXWEEN	= 1;
		IO_SHDR.SHDRINT0.bit.AXREEN	= 1;
		IO_SHDR.SHDRINT0.bit.SHDREN	= 1;

		// Interrupt flag clear.
		shdrint1.word				= 0;
		shdrint1.bit.__AXWEFLG		= 1;
		shdrint1.bit.__AXREFLG		= 1;
		shdrint1.bit.__SHDRFLG		= 1;
		IO_SHDR.SHDRINT1.word		= shdrint1.word;

		im_shdr_on_xclk();
		im_shdr_on_clk();
		im_shdr_dsb();

		// SHDR macro start.
		IO_SHDR.SHDRACTL1.bit.TRG = D_IM_SHDR_TRG_START;
	}
	else {
		Ddim_Print(("Im_SHDR_Start() : Macro busy\n"));
		ret = D_IM_SHDR_MACRO_BUSY;
	}

	im_shdr_unlock();

	im_shdr_off_pclk();
	im_shdr_dsb();

	return ret;
}

/**
SHDR wait end.<br>
@param[out]		p_wait_factor					Pointer of factor parameter which release wait process.
@param[in]		wait_time						Wait time [msec]. The valid range is -1, 0, ... .
												If this parameter is set to -1, driver waits permanently unless system-call sets event-flag.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_INPUT_PARAM_ERROR		Parameter error.
@retval			D_IM_SHDR_TIMEOUT				Time out.
@retval			D_IM_SHDR_AXI_ERROR				AXI BUS error.
@retval			D_IM_SHDR_NG					System NG.
*/
INT32 Im_SHDR_Wait_End( UINT32* const p_wait_factor, const INT32 wait_time )
{
	DDIM_USER_FLGPTN	waitptn;
	DDIM_USER_FLGPTN	flgptn;
	DDIM_USER_ER		ercd;
	INT32				ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ( ( p_wait_factor == NULL ) ||
		 ( im_shdr_check_wait_time( wait_time ) == FALSE) ) {
		Ddim_Assertion(("Im_SHDR_Wait_End parameter error\n"));
		return D_IM_SHDR_INPUT_PARAM_ERROR;
	}
#endif

	waitptn = ( D_IM_SHDR_FLG_END | D_IM_SHDR_FLG_STOP );

	// SHDR wait end.
	ercd = DDIM_User_Twai_Flg( FID_IM_SHDR, waitptn, D_DDIM_USER_TWF_ORW, &flgptn, (DDIM_USER_TMO)wait_time );

	if ( ercd == D_DDIM_USER_E_TMOUT ) {

		Ddim_Print(("Im_SHDR_Wait_End() time out. ercd = %d\n", ercd));

		ret = D_IM_SHDR_TIMEOUT;
	}
	else if ( ercd != D_DDIM_USER_E_OK ) {

		Ddim_Print(("Im_SHDR_Wait_End() system error. ercd = %d\n", ercd));

		ret = D_IM_SHDR_NG;
	}
	else {
		flgptn = waitptn & flgptn;
		(VOID)DDIM_User_Clr_Flg( FID_IM_SHDR, ~flgptn );
	}

	*p_wait_factor = (UINT32)flgptn;

	if ( ret == D_DDIM_OK ) {
		// Check AXI error.
		if ( gIM_SHDR_Axi_Err != D_IM_SHDR_NORMAL_END ) {
			Ddim_Print(("Im_SHDR_Wait_End() AXI error. ercd = %lu\n", gIM_SHDR_Axi_Err));
			ret = D_IM_SHDR_AXI_ERROR;
		}
	}

	im_shdr_off_clk();
	im_shdr_off_xclk();
	im_shdr_dsb();

	return ret;
}

/**
SHDR stop.<br>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SHDR_MACRO_BUSY			Macro busy.
@retval			D_IM_SHDR_NG					System NG.
*/
INT32 Im_SHDR_Stop( VOID )
{
	INT32			ret = D_DDIM_OK;
	DDIM_USER_ER	ercd;

	im_shdr_on_pclk();
	im_shdr_on_xclk();
	im_shdr_on_clk();
	im_shdr_dsb();

	im_shdr_lock();

	// Check SHDR macro status.
	if ( IO_SHDR.SHDRACTL1.bit.TRG == D_IM_SHDR_TRG_STATUS_START ) {

		// SHDR macro stop.
		IO_SHDR.SHDRACTL1.bit.TRG = D_IM_SHDR_TRG_STOP;

		im_shdr_dsb();

		ercd = DDIM_User_Set_Flg( FID_IM_SHDR, D_IM_SHDR_FLG_STOP );
		if ( ercd != D_DDIM_USER_E_OK ) {
			Ddim_Print(("Im_SHDR_Stop : Error DDIM_User_Set_Flg [%d]\n", ercd));
			ret = D_IM_SHDR_NG;
		}

		// Check SHDR macro status.
		if ( IO_SHDR.SHDRACTL1.bit.TRG == D_IM_SHDR_TRG_STATUS_START ) {
			Ddim_Print(("Im_SHDR_Stop : Macro busy. [%d]\n", ercd));
			ret = D_IM_SHDR_MACRO_BUSY;
		}
	}

	im_shdr_unlock();

	im_shdr_off_clk();
	im_shdr_off_xclk();
	im_shdr_off_pclk();
	im_shdr_dsb();

	return ret;
}

/**
SHDR interupt handler.<br>
@retval			None.
*/
VOID Im_SHDR_Int_Handler( VOID )
{
	DDIM_USER_ER			ercd;
	union io_shdr_shdrint0	shdrint0;
	union io_shdr_shdrint1	shdrint1;

	gIM_SHDR_Axi_Err = D_IM_SHDR_NORMAL_END;

	im_shdr_on_pclk();
	im_shdr_dsb();

	shdrint0.word = IO_SHDR.SHDRINT0.word;
	shdrint1.word = IO_SHDR.SHDRINT1.word;

	// AXI write error check.
	if ( ( shdrint0.bit.AXWEEN == 1 ) && ( shdrint1.bit.__AXWEFLG == 1 ) ) {	/* pgr0872 */
#ifdef CO_SHDR_DEBUG_PRINT
		Ddim_Print(("BRESPA[0] = %d\n", IO_SHDR.AXICTLWA.bit.BRESPA_0));
		Ddim_Print(("BRESPA[1] = %d\n", IO_SHDR.AXICTLWA.bit.BRESPA_1));
		Ddim_Print(("BRESPA[2] = %d\n", IO_SHDR.AXICTLWA.bit.BRESPA_2));
		Ddim_Print(("BRESPA[3] = %d\n", IO_SHDR.AXICTLWA.bit.BRESPA_3));
		Ddim_Print(("BRESPB[0] = %d\n", IO_SHDR.AXICTLWB.bit.BRESPB_0));
		Ddim_Print(("BRESPB[1] = %d\n", IO_SHDR.AXICTLWB.bit.BRESPB_1));
		Ddim_Print(("BRESPB[2] = %d\n", IO_SHDR.AXICTLWB.bit.BRESPB_2));
		Ddim_Print(("BRESPB[3] = %d\n", IO_SHDR.AXICTLWB.bit.BRESPB_3));
		Ddim_Print(("BRESPB[4] = %d\n", IO_SHDR.AXICTLWB.bit.BRESPB_4));
		Ddim_Print(("BRESPB[5] = %d\n", IO_SHDR.AXICTLWB.bit.BRESPB_5));
		Ddim_Print(("BRESPC[0] = %d\n", IO_SHDR.AXICTLWC.bit.BRESPC_0));
		Ddim_Print(("BRESPC[1] = %d\n", IO_SHDR.AXICTLWC.bit.BRESPC_1));
		Ddim_Print(("BRESPC[2] = %d\n", IO_SHDR.AXICTLWC.bit.BRESPC_2));
		Ddim_Print(("BRESPC[3] = %d\n", IO_SHDR.AXICTLWC.bit.BRESPC_3));
		Ddim_Print(("BRESPC[4] = %d\n", IO_SHDR.AXICTLWC.bit.BRESPC_4));
		Ddim_Print(("BRESPC[5] = %d\n", IO_SHDR.AXICTLWC.bit.BRESPC_5));
		Ddim_Print(("BRESPC[6] = %d\n", IO_SHDR.AXICTLWC.bit.BRESPC_6));
#endif
		IO_SHDR.SHDRINT1.word = D_IM_SHDR_INTF_AXWEFLG;							// flag clear.
		gIM_SHDR_Axi_Err |= D_IM_SHDR_AXI_ERR_CH_W;
	}

	// AXI read error check.
	if ( ( shdrint0.bit.AXREEN == 1 ) && ( shdrint1.bit.__AXREFLG == 1 ) ) {	/* pgr0872 */
#ifdef CO_SHDR_DEBUG_PRINT
		Ddim_Print(("RRESPA[0] = %d\n", IO_SHDR.AXICTLRA.bit.RRESPA_0));
		Ddim_Print(("RRESPA[1] = %d\n", IO_SHDR.AXICTLRA.bit.RRESPA_1));
		Ddim_Print(("RRESPB[0] = %d\n", IO_SHDR.AXICTLRB.bit.RRESPB_0));
		Ddim_Print(("RRESPB[1] = %d\n", IO_SHDR.AXICTLRB.bit.RRESPB_1));
		Ddim_Print(("RRESPC[0] = %d\n", IO_SHDR.AXICTLRC.bit.RRESPC_0));
		Ddim_Print(("RRESPC[1] = %d\n", IO_SHDR.AXICTLRC.bit.RRESPC_1));
		Ddim_Print(("RRESPC[2] = %d\n", IO_SHDR.AXICTLRC.bit.RRESPC_2));
		Ddim_Print(("RRESPC[3] = %d\n", IO_SHDR.AXICTLRC.bit.RRESPC_3));
		Ddim_Print(("RRESPD[0] = %d\n", IO_SHDR.AXICTLRD.bit.RRESPD_0));
		Ddim_Print(("RRESPD[1] = %d\n", IO_SHDR.AXICTLRD.bit.RRESPD_1));
#endif
		IO_SHDR.SHDRINT1.word = D_IM_SHDR_INTF_AXREFLG;							// flag clear.
		gIM_SHDR_Axi_Err |= D_IM_SHDR_AXI_ERR_CH_R;
	}

	// SHDR trans end check.
	if ( ( shdrint0.bit.SHDREN == 1 ) && ( shdrint1.bit.__SHDRFLG == 1 ) ) {	/* pgr0872 */
		IO_SHDR.SHDRINT1.word = D_IM_SHDR_INTF_SHDRFLG;							// flag clear.
	}

	im_shdr_dsb();

#ifdef CO_SHDR_DEBUG_PRINT
	Ddim_Print(( "Im_SHDR_Int_Handler shdrint0=0x%lx shdrint1=0x%lx\n", shdrint0.word, shdrint1.word ));
#endif

	ercd = DDIM_User_Set_Flg( FID_IM_SHDR, D_IM_SHDR_FLG_END );
	if ( ercd != D_DDIM_USER_E_OK ) {
		Ddim_Print(("Im_SHDR_Int_Handler : Error DDIM_User_Set_Flg [%d]\n", ercd));
	}

	im_shdr_off_pclk();
	im_shdr_dsb();

	// User callback function.
	if ( gIM_SHDR_CallBack_Func != NULL ) {
#ifdef CO_SHDR_DEBUG_PRINT
		Ddim_Print(( "Im_SHDR_Int_Handler begin callback\n" ));
#endif
		gIM_SHDR_CallBack_Func( gIM_SHDR_Axi_Err );

#ifdef CO_SHDR_DEBUG_PRINT
		Ddim_Print(( "Im_SHDR_Int_Handler end callback\n" ));
#endif
	}
}

/**
SHDR line interupt handler.<br>
@retval			None.
*/
VOID Im_SHDR_Line_Int_Handler( VOID )
{
	// User callback function.
	if ( gIM_SHDR_LineInt_CallBack_Func != NULL ) {
#ifdef CO_SHDR_DEBUG_PRINT
		Ddim_Print(( "Im_SHDR_Line_Int_Handler begin callback\n" ));
#endif
		gIM_SHDR_LineInt_CallBack_Func( 0 );

#ifdef CO_SHDR_DEBUG_PRINT
		Ddim_Print(( "Im_SHDR_Line_Int_Handler end callback\n" ));
#endif
	}
}
