/**
 * @file		im_sdramc.c
 * @brief		SDRAMC macro wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "sdramc.h"
#include "im_sdramc.h"
#include "dd_top.h"
#include "ddim_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define CO_SDRAMC_REG_TYPE_CHECK
//#define CO_SDRAMC_DEBUG_PRINT		// Debug.

// Data Synchronization Barrier.
#define im_sdramc_dsb()						Dd_ARM_Dsb_Pou()

// Wait
#define im_sdramc_wait_ns( nsec )			Dd_ARM_Wait_ns( nsec )
#define im_sdramc_wait_us( usec )			Dd_ARM_Wait_ns( usec * 1000 )

// The structure type matching check.
#ifdef CO_SDRAMC_REG_TYPE_CHECK
#define im_sdramc_check_reg_type( dst, src )	( dst ) = ( src );
#else
#define im_sdramc_check_reg_type( dst, src )
#endif

// Set signed register.
#define im_sdramc_set_reg_signed( reg, type, member, val )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_sdramc_check_reg_type( work, (reg) ); \
		}

// Get signed register.
#define im_sdramc_get_reg_signed( val, reg, type, member )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = -1L; \
			if( ((work.bit.member) == 0xFFFFFFFFUL) || ((reg.bit.member) <= ((work.bit.member) /2)) ) { \
				(val) = reg.bit.member; \
			} \
			else { \
				(val) = -(work.bit.member - reg.bit.member +1); \
			} \
			im_sdramc_check_reg_type( (reg), work );	/* The structure type matching check. */ \
		}

#define D_IM_SDRAMC_SHIFT_REMAPADDR		(26)

#ifdef CO_ACT_ICLOCK
#define	CO_ACT_SDRAMC_ICLOCK
#endif

#ifdef CO_ACT_PCLOCK
#define	CO_ACT_SDRMAC_PCLOCK
#endif

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
#ifdef CO_ACT_SDRAMC_PCLOCK
static volatile UCHAR					gIM_SDRAMC_PCLK_Counter[ E_IM_SDRAMC_CH_MAX ] = { 0, 0 };
#endif

static volatile T_IM_SDRAMC_CALLBACK	gIM_SDRAMC_CallBack_Func[ E_IM_SDRAMC_CH_MAX ] = { NULL, NULL };

static volatile ULONG					gIM_SDRAMC_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
// Mutex.
#define im_sdramc_lock()				Dd_ARM_Critical_Section_Start( gIM_SDRAMC_Spin_Lock )
#define im_sdramc_unlock()				Dd_ARM_Critical_Section_End( gIM_SDRAMC_Spin_Lock )

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
PCLK ON for SDRAMC.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			None.
*/
static VOID im_sdramc_on_pclk( E_IM_SDRAMC_CH ch )
{
#ifdef CO_ACT_SDRAMC_PCLOCK
	if ( ch == E_IM_SDRAMC_CH_0 ) {
		Dd_Top_Start_Clock( (UCHAR*)&gIM_SDRAMC_PCLK_Counter[ ch ], &Dd_Top_Get_CLKSTOP9(), ~D_DD_TOP_UMC0AP_BIT );
	}
	else  {
		Dd_Top_Start_Clock( (UCHAR*)&gIM_SDRAMC_PCLK_Counter[ ch ], &Dd_Top_Get_CLKSTOP9(), ~D_DD_TOP_UMC1AP_BIT );
	}
#endif
}

/**
PCLK OFF for SDRAMC.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			None.
*/
static VOID im_sdramc_off_pclk( E_IM_SDRAMC_CH ch )
{
#ifdef CO_ACT_SDRAMC_PCLOCK
	if ( ch == E_IM_SDRAMC_CH_0 ) {
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_SDRAMC_PCLK_Counter[ ch ], &Dd_Top_Get_CLKSTOP9(), D_DD_TOP_UMC0AP_BIT );
	}
	else {
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_SDRAMC_PCLK_Counter[ ch ], &Dd_Top_Get_CLKSTOP9(), D_DD_TOP_UMC1AP_BIT );
	}
#endif
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

/**
SDRAMC acceptance capabilit set.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[in]		rnum							read acceptance capability.
@param[in]		wnum							write acceptance capability.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_MX00/MX01/MX40 are setable.
*/
INT32 Im_SDRAMC_Set_AcceptanceCapability( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, const UCHAR rnum, const UCHAR wnum )
{
#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Set_AcceptanceCapability() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	switch(  port ) {
		case E_IM_SDRAMC_PORT_MX40:	// FALLTHROUGH
		case E_IM_SDRAMC_PORT_MX00:	// FALLTHROUGH
		case E_IM_SDRAMC_PORT_MX01:
			break;
		default:
			Ddim_Assertion(("Im_SDRAMC_Set_AcceptanceCapability() error. [port] = %d\n", port));
			return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	IO_SDRAMC[ ch ].EMBNUM[ port ].bit.WNUM = (ULONG)wnum;
	IO_SDRAMC[ ch ].EMBNUM[ port ].bit.RNUM = (ULONG)rnum;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC acceptance capabilit get.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[out]		rnum							read acceptance capability.
@param[out]		wnum							write acceptance capability.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_MX00/MX01/MX40 are setable.
*/
INT32 Im_SDRAMC_Get_AcceptanceCapability( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, UCHAR* const rnum, UCHAR* const wnum )
{
#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Get_AcceptanceCapability() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	switch(  port ) {
		case E_IM_SDRAMC_PORT_MX40:	// FALLTHROUGH
		case E_IM_SDRAMC_PORT_MX00:	// FALLTHROUGH
		case E_IM_SDRAMC_PORT_MX01:
			break;
		default:
			Ddim_Assertion(("Im_SDRAMC_Get_AcceptanceCapability() error. [port] = %d\n", port));
			return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	*wnum = (UCHAR)IO_SDRAMC[ ch ].EMBNUM[ port ].bit.WNUM;
	*rnum = (UCHAR)IO_SDRAMC[ ch ].EMBNUM[ port ].bit.RNUM;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC burst limit size set.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[in]		rsize							read bytes. @ref E_IM_SDRAMC_ACC_CAP.
@param[in]		wsize							write bytes. @ref E_IM_SDRAMC_ACC_CAP.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_MX00/MX01/MX40 are setable.
*/
INT32 Im_SDRAMC_Set_BurstLimit( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, const E_IM_SDRAMC_BURST_LIMIT rsize, const E_IM_SDRAMC_BURST_LIMIT wsize )
{
#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Set_BurstLimit() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	switch(  port ) {
		case E_IM_SDRAMC_PORT_MX40:	// FALLTHROUGH
		case E_IM_SDRAMC_PORT_MX00:	// FALLTHROUGH
		case E_IM_SDRAMC_PORT_MX01:
			break;
		default:
			Ddim_Assertion(("Im_SDRAMC_Set_BurstLimit() error. [port] = %d\n", port));
			return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	IO_SDRAMC[ ch ].EMBNUM[ port ].bit.RSIZE = (ULONG)rsize;
	IO_SDRAMC[ ch ].EMBNUM[ port ].bit.WSIZE = (ULONG)wsize;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC burst limit size get.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[out]		rsize							read bytes. @ref E_IM_SDRAMC_ACC_CAP.
@param[out]		wsize							write bytes. @ref E_IM_SDRAMC_ACC_CAP.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_MX00/MX01/MX40 are setable.
*/
INT32 Im_SDRAMC_Get_BurstLimit( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, E_IM_SDRAMC_BURST_LIMIT* const rsize, E_IM_SDRAMC_BURST_LIMIT* const wsize )
{
#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Get_BurstLimit() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	switch(  port ) {
		case E_IM_SDRAMC_PORT_MX40:	// FALLTHROUGH
		case E_IM_SDRAMC_PORT_MX00:	// FALLTHROUGH
		case E_IM_SDRAMC_PORT_MX01:
			break;
		default:
			Ddim_Assertion(("Im_SDRAMC_Get_BurstLimit() error. [port] = %d\n", port));
			return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	*rsize = (E_IM_SDRAMC_BURST_LIMIT)IO_SDRAMC[ ch ].EMBNUM[ port ].bit.RSIZE;
	*wsize = (E_IM_SDRAMC_BURST_LIMIT)IO_SDRAMC[ ch ].EMBNUM[ port ].bit.WSIZE;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC remap address set.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[in]		addr							Remap address(unit 512Mbit).
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_DS0/DS1/SDM/VE0/VE1/VE2/VD0/VD1/VD2 are setable.
*/
INT32 Im_SDRAMC_Set_RemapAddr( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, const LONG addr )
{
#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Set_RemapAddr() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( ( port < E_IM_SDRAMC_PORT_DS0 ) || ( port >= E_IM_SDRAMC_PORT_MAX ) ) {
		Ddim_Assertion(("Im_SDRAMC_Set_RemapAddr() error. [port] = %d\n", port));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	im_sdramc_set_reg_signed( IO_SDRAMC[ ch ].REMAPADDR[ port ], union io_sdramc_remapaddr, REMAPADDR, ( addr >> D_IM_SDRAMC_SHIFT_REMAPADDR ) );

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC remap address get.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[out]		addr							Remap address(unit 512Mbit).
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_DS0/DS1/SDM/VE0/VE1/VE2/VD0/VD1/VD2 are setable.
*/
INT32 Im_SDRAMC_Get_RemapAddr( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, LONG* const addr )
{
#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Get_RemapAddr() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( ( port < E_IM_SDRAMC_PORT_DS0 ) || ( port >= E_IM_SDRAMC_PORT_MAX ) ) {
		Ddim_Assertion(("Im_SDRAMC_Get_RemapAddr() error. [port] = %d\n", port));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( addr == NULL ) {
		Ddim_Assertion(("Im_SDRAMC_Get_RemapAddr() error. [addr] is NULL\n"));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	// Remap address get.
	im_sdramc_get_reg_signed( *addr, IO_SDRAMC[ ch ].REMAPADDR[ port ], union io_sdramc_remapaddr, REMAPADDR );
	*addr = ( *addr << D_IM_SDRAMC_SHIFT_REMAPADDR );

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
/**
SDRAMC CHSEL set.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_VE0/VE1/VD1 are setable.
*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/**
SDRAMC CHSEL set.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_VD1 are setable.
*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
INT32 Im_SDRAMC_Set_Ch_Select( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port)
{
#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Ch_Select() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	if ( ( port != E_IM_SDRAMC_PORT_VE0 ) &&
		 ( port != E_IM_SDRAMC_PORT_VE1 ) &&
		 ( port != E_IM_SDRAMC_PORT_VD1 ) ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Ch_Select() error. [port] = %d\n", port));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	if ( port != E_IM_SDRAMC_PORT_VD1 ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Ch_Select() error. [port] = %d\n", port));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	IO_SDRAMC[ ch ].CHSEL[ port ].bit.CH = ch;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
/**
SDRAMC CHSEL get.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[out]		chsel							CH select.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_VE0/VE1/VD1 are setable.
*/

// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/**
SDRAMC CHSEL get.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[out]		chsel							CH select.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		In "port", only E_IM_SDRAMC_PORT_VD1 are setable.
*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
INT32 Im_SDRAMC_Get_Ch_Select( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, E_IM_SDRAMC_CH* chsel )
{
#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Ch_Select() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	if ( ( port != E_IM_SDRAMC_PORT_VE0 ) &&
		 ( port != E_IM_SDRAMC_PORT_VE1 ) &&
		 ( port != E_IM_SDRAMC_PORT_VD1 ) ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Ch_Select() error. [port] = %d\n", port));
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	if ( port != E_IM_SDRAMC_PORT_VD1 ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Ch_Select() error. [port] = %d\n", port));
	}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	if ( chsel == NULL ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Ch_Select() error. [chsel] is NULL\n"));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	// CHSEL get.
	*chsel = IO_SDRAMC[ ch ].CHSEL[ port ].bit.CH;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC command arbitration set (rate contorol).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		cmd_arb							Commnad arbitration information for rate control @ref T_IM_SDRAMC_CMD_ARB_RATE.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Set_Cmd_Arbitration_Rate( E_IM_SDRAMC_CH ch, const T_IM_SDRAMC_CMD_ARB_RATE* const cmd_arb )
{
	UCHAR port;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Cmd_Arbitration_Rate() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( cmd_arb == NULL ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Cmd_Arbitration_Rate() error. [cmd_arb] is NULL\n"));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	for ( port = 0; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		if ( cmd_arb->enable[ port ] > E_IM_SDRAMC_EN_ENABLE ) {
			Ddim_Assertion(("Im_SDRAMC_Set_Cmd_Arbitration_Rate() error. [cmd_arb->enable[%d]] = %d\n", port, cmd_arb->enable[ port ]));
			return D_IM_SDRAMC_INPUT_PARAM_ERROR;
		}
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	for ( port = 0; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		// Rate control information set.
		IO_SDRAMC[ ch ].RATECONT[ port ].bit.CYCNUM				= cmd_arb->throughput[ port ];
		IO_SDRAMC[ ch ].RATECONT[ port ].bit.LATNUM				= cmd_arb->latency[ port ];
		// Rate control enable.
		IO_SDRAMC[ ch ].MASTEROPTION[ port ].bit.ENABLE_CAT_R	= cmd_arb->enable[ port ];
	}

	im_sdramc_dsb();

	// Rate control register reflect.
	IO_SDRAMC[ ch ].RATE_CHANGE.bit.RATE_CHG = 1;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC command arbitration get(rate contorol).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[out]		cmd_arb							Commnad arbitration information for rate control @ref T_IM_SDRAMC_CMD_ARB_RATE.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Get_Cmd_Arbitration_Rate( E_IM_SDRAMC_CH ch, T_IM_SDRAMC_CMD_ARB_RATE* const cmd_arb )
{
	UCHAR port;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Cmd_Arbitration_Rate() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( cmd_arb == NULL ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Cmd_Arbitration_Rate() error. [cmd_arb] is NULL\n"));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	for ( port = 0; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		// Rate control information get.
		cmd_arb->throughput[ port ]	= IO_SDRAMC[ ch ].RATECONT[ port ].bit.CYCNUM;
		cmd_arb->latency[ port ]	= IO_SDRAMC[ ch ].RATECONT[ port ].bit.LATNUM;
		cmd_arb->enable[ port ]		= IO_SDRAMC[ ch ].MASTEROPTION[ port ].bit.ENABLE_CAT_R;
	}

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC command arbitration set (priority contorol).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		cmd_arb							Commnad arbitration information for priority control @ref T_IM_SDRAMC_CMD_ARB_PRI.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		A priority is high so as to be small.
@remarks		It is forbidden setting the same value between ports.
*/
INT32 Im_SDRAMC_Set_Cmd_Arbitration_Priority( E_IM_SDRAMC_CH ch, const T_IM_SDRAMC_CMD_ARB_PRI* const cmd_arb )
{
	UCHAR port;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Cmd_Arbitration_Priority() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( cmd_arb == NULL ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Cmd_Arbitration_Priority() error. [cmd_arb] is NULL\n"));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	for ( port = 0; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		if ( cmd_arb->enable[ port ] > E_IM_SDRAMC_EN_ENABLE ) {
			Ddim_Assertion(("Im_SDRAMC_Set_Cmd_Arbitration_Priority() error. [cmd_arb->enable[%d]] = %d\n", port, cmd_arb->enable[ port ]));
			return D_IM_SDRAMC_INPUT_PARAM_ERROR;
		}
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	for ( port = 0; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		// Priority contrl infformation set.
		IO_SDRAMC[ ch ].PRIORIT_CAT[ port ].bit.PRIORIT_CAT_B = cmd_arb->priority[ port ];
		// Priority control enable.
		IO_SDRAMC[ ch ].MASTEROPTION[ port ].bit.ENABLE_CAT_B = cmd_arb->enable[ port ];
	}

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC command arbitration get (priority contorol).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		cmd_arb							Commnad arbitration information for priority control @ref T_IM_SDRAMC_CMD_ARB_PRI.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Get_Cmd_Arbitration_Priority( E_IM_SDRAMC_CH ch, T_IM_SDRAMC_CMD_ARB_PRI* const cmd_arb )
{
	UCHAR port;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Cmd_Arbitration_Priority() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( cmd_arb == NULL ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Cmd_Arbitration_Priority() error. [cmd_arb] is NULL\n"));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	for ( port = 0; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		// Priority contrl infformation get.
		cmd_arb->priority[ port ]	= IO_SDRAMC[ ch ].PRIORIT_CAT[ port ].bit.PRIORIT_CAT_B;
		cmd_arb->enable[ port ]		= IO_SDRAMC[ ch ].MASTEROPTION[ port ].bit.ENABLE_CAT_B;
	}

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC mode register set.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		rank							Access rank.
													<ul><li>@ref E_IM_SDRAMC_RANK_0.
														<li>@ref E_IM_SDRAMC_RANK_1.</ul>
@param[in]		ma								MA value.
@param[in]		data							Write data.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Set_Mode_Register( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_RANK rank, const UCHAR ma, const UCHAR data )
{
	union io_sdramc_umcmracs	umcmracs;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Mode_Register() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( rank > E_IM_SDRAMC_RANK_1 ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Mode_Register() error. [rank] = %d\n", rank));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	// Write data set.
	IO_SDRAMC[ ch ].UMCMRWDT.bit.WDATA = data;

	// Command setting.
	umcmracs.word			= 0;
	umcmracs.bit.MRMA		= ma;
	umcmracs.bit.RESERVED0	= 0x0000;
	umcmracs.bit.MRMODE		= 0xF;
	umcmracs.bit.MRRANK		= rank;
	umcmracs.bit.RESERVED1	= 0x3;

	IO_SDRAMC[ ch ].UMCMRACS.word = umcmracs.word;

	im_sdramc_dsb();

	// status check.
	while( IO_SDRAMC[ ch ].UMCMRACSST.bit.MRST == 0 ) {
		;	// Do nothing.
	}

	// status clear.
	IO_SDRAMC[ ch ].UMCMRACSST.bit.MRST = 1;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC mode register get.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		rank							Access rank.
													<ul><li>@ref E_IM_SDRAMC_RANK_0.
														<li>@ref E_IM_SDRAMC_RANK_1.</ul>
@param[in]		ma								MA value.
@param[out]		data							Read data.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Get_Mode_Register( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_RANK rank, const UCHAR ma, UCHAR* const data )
{
	union io_sdramc_umcmracs	umcmracs;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Mode_Register() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( rank > E_IM_SDRAMC_RANK_1 ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Mode_Register() error. [rank] = %d\n", rank));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( data == NULL ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Mode_Register() error. [data] is NULL\n"));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	// Command setting.
	umcmracs.word			= 0;
	umcmracs.bit.MRMA		= ma;
	umcmracs.bit.RESERVED0	= 0x0000;
	umcmracs.bit.MRMODE		= 0x7;
	umcmracs.bit.MRRANK		= rank;
	umcmracs.bit.RESERVED1	= 0x3;

	IO_SDRAMC[ ch ].UMCMRACS.word = umcmracs.word;

	im_sdramc_dsb();

	// status check.
	while( IO_SDRAMC[ ch ].UMCMRACSST.bit.MRST == 0 ) {
		;	// Do nothing.
	}

	// Read data get.
	*data = (UCHAR)IO_SDRAMC[ ch ].UMCMRRDT.bit.RDATA;

	// status clear.
	IO_SDRAMC[ ch ].UMCMRACSST.bit.MRST = 1;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC BusIF clock enable.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		port							Port number @ref E_IM_SDRAMC_PORT.
@param[in]		access							Read/Write access.
													<ul><li>@ref E_IM_SDRAMC_ACCESS_READ.
														<li>@ref E_IM_SDRAMC_ACCESS_WRITE.</ul>
@param[in]		enable							Clock enable.
													<ul><li>@ref E_IM_SDRAMC_EN_DISABLE.
														<li>@ref E_IM_SDRAMC_EN_ENABLE.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		If "port" is RBx or DSx or SDM or VDx, "access" is ignored.
@remarks2		If "port" is MX00 or MX01 and "access" is E_IM_SDRAMC_ACCESS_READ, "enable" is E_IM_SDRAMC_EN_DISABLE.
*/
INT32 Im_SDRAMC_Set_Clock_Enable_BusIf( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_PORT port, E_IM_SDRAMC_ACCESS access, E_IM_SDRAMC_EN enable )
{
	ULONG	clkifa;
	ULONG	clkifb;
	ULONG	clkifc;
	UCHAR	clkds0;
	UCHAR	clkds1;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Clock_Enable_BusIf() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( port >= E_IM_SDRAMC_PORT_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Clock_Enable_BusIf() error. [port] = %d\n", port));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( access > E_IM_SDRAMC_ACCESS_WRITE ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Clock_Enable_BusIf() error. [access] = %d\n", access));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( enable > E_IM_SDRAMC_EN_ENABLE ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Clock_Enable_BusIf() error. [enable] = %d\n", enable));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	if ( enable == E_IM_SDRAMC_EN_ENABLE ) {

		clkifa = IO_SDRAMC[ ch ].CLKENBIFA.word;
		clkifb = IO_SDRAMC[ ch ].CLKENBIFB.word;
		clkifc = IO_SDRAMC[ ch ].CLKENBIFC.word;

		if ( ( clkifa == 0 ) &&
			 ( clkifb == 0 ) &&
			 ( ( clkifc & 0x3FFC0000 ) == 0 ) ) {
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKBIF = E_IM_SDRAMC_EN_ENABLE;
		}

		if ( ( port == E_IM_SDRAMC_PORT_DS0 ) ||
			 ( port == E_IM_SDRAMC_PORT_DS1 ) ) {

			clkds0 = IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKDS0;
			clkds1 = IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKDS1;

			if ( ( clkds0 == E_IM_SDRAMC_EN_DISABLE ) &&
				 ( clkds1 == E_IM_SDRAMC_EN_DISABLE ) ) {
				IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKDSP = E_IM_SDRAMC_EN_ENABLE;
			}
		}
	}

	im_sdramc_dsb();

	switch ( port ) {
		case E_IM_SDRAMC_PORT_MX10:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX10 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX10 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX11:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX11 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX11 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX12:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX12 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX12 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX13:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX13 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX13 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX20:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX20 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX20 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX21:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX21 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX21 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX22:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX22 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX22 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX23:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX23 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX23 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX30:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX30 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX30 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX31:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX31 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX31 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX32:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX32 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX32 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX33:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX33 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX33 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX40:	// Read only.
			if ( access == E_IM_SDRAMC_ACCESS_READ ) {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX40 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX50:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX50 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX50 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX51:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX51 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX51 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_MX00:	// Write only port.
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX00 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX00 = E_IM_SDRAMC_EN_DISABLE;
			}
			break;
		case E_IM_SDRAMC_PORT_MX01:	// Write only port.
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX01 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX01 = E_IM_SDRAMC_EN_DISABLE;
			}
			break;
		case E_IM_SDRAMC_PORT_MX60:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWMX60 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRMX60 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_RB0:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKRB0 = enable;
			break;
		case E_IM_SDRAMC_PORT_RB1:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKRB1 = enable;
			break;
		case E_IM_SDRAMC_PORT_RB2:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKRB2 = enable;
			break;
		case E_IM_SDRAMC_PORT_DS0:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKDS0 = enable;
			break;
		case E_IM_SDRAMC_PORT_DS1:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKDS1 = enable;
			break;
		case E_IM_SDRAMC_PORT_SDM:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKSDM = enable;
			break;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		case E_IM_SDRAMC_PORT_VE0:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKVE0 = enable;
			break;
		case E_IM_SDRAMC_PORT_VE1:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKVE1 = enable;
			break;
		case E_IM_SDRAMC_PORT_VE2:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKVE2 = enable;
			break;
		case E_IM_SDRAMC_PORT_VD0:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKVD0 = enable;
			break;
		case E_IM_SDRAMC_PORT_VD1:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKVD1 = enable;
			break;
		case E_IM_SDRAMC_PORT_VD2:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKVD2 = enable;
			break;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		case E_IM_SDRAMC_PORT_VCOREW:	// Write only port.
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWVCOREW = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRVCOREW = E_IM_SDRAMC_EN_DISABLE;
			}
			break;
		case E_IM_SDRAMC_PORT_VCORER0:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWVCORER0 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRVCORER0 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_VCORER1:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWVCORER1 = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRVCORER1 = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_VD0:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKVD0 = enable;
			break;
		case E_IM_SDRAMC_PORT_VD1:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKVD1 = enable;
			break;
		case E_IM_SDRAMC_PORT_VD2:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKVD2 = enable;
			break;
		case E_IM_SDRAMC_PORT_VCPU:
			if ( access == E_IM_SDRAMC_ACCESS_WRITE ) {
				IO_SDRAMC[ ch ].CLKENBIFA.bit.CLKWVCPU = enable;
			}
			else {
				IO_SDRAMC[ ch ].CLKENBIFB.bit.CLKRVCPU = enable;
			}
			break;
		case E_IM_SDRAMC_PORT_VCOREM:
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKVOREM = enable;
			break;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		default :
			im_sdramc_off_pclk( ch );
			Ddim_Assertion(("Im_SDRAMC_Set_Clock_Enable_BusIf() error. [port] = %d\n", port));
			return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}

	im_sdramc_dsb();

	if ( enable == E_IM_SDRAMC_EN_DISABLE ) {

		clkifa = IO_SDRAMC[ ch ].CLKENBIFA.word;
		clkifb = IO_SDRAMC[ ch ].CLKENBIFB.word;
		clkifc = IO_SDRAMC[ ch ].CLKENBIFC.word;

		if ( ( clkifa == 0 ) &&
			 ( clkifb == 0 ) &&
			 ( ( clkifc & 0x3FFC0000 ) == 0 ) ) {
			IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKBIF = E_IM_SDRAMC_EN_DISABLE;
		}

		if ( ( port == E_IM_SDRAMC_PORT_DS0 ) ||
			 ( port == E_IM_SDRAMC_PORT_DS1 ) ) {

			clkds0 = IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKDS0;
			clkds1 = IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKDS1;

			if ( ( clkds0 == E_IM_SDRAMC_EN_DISABLE ) &&
				 ( clkds1 == E_IM_SDRAMC_EN_DISABLE ) ) {
				IO_SDRAMC[ ch ].CLKENBIFC.bit.CLKDSP = E_IM_SDRAMC_EN_DISABLE;
			}
		}
	}

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC Data buffer clock enable.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		dbno							Data buffer number @ref E_IM_SDRAMC_DATABUF.
@param[in]		enable							Clock enable.
													<ul><li>@ref E_IM_SDRAMC_EN_DISABLE.
														<li>@ref E_IM_SDRAMC_EN_ENABLE.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Set_Clock_Enable_DataBuf( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_DATABUF dbno, E_IM_SDRAMC_EN enable )
{
#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Clock_Enable_DataBuf() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( dbno >= E_IM_SDRAMC_DATABUF_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Clock_Enable_DataBuf() error. [dbno] = %d\n", dbno));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( enable > E_IM_SDRAMC_EN_ENABLE ) {
		Ddim_Assertion(("Im_SDRAMC_Set_Clock_Enable_DataBuf() error. [enable] = %d\n", enable));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	switch ( dbno ) {
		// Data buffer 0 (MX10/MX11).
		case E_IM_SDRAMC_DATABUF_0 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB0DA = enable;
			break;
		// Data buffer 1 (MX12/MX13).
		case E_IM_SDRAMC_DATABUF_1 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB1DA = enable;
			break;
		// Data buffer 2 (MX20/MX21).
		case E_IM_SDRAMC_DATABUF_2 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB2DA = enable;
			break;
		// Data buffer 3 (MX22/MX23).
		case E_IM_SDRAMC_DATABUF_3 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB3DA = enable;
			break;
		// Data buffer 4 (MX30/MX31).
		case E_IM_SDRAMC_DATABUF_4 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB4DA = enable;
			break;
		// Data buffer 5 (MX32/MX33).
		case E_IM_SDRAMC_DATABUF_5 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB5DA = enable;
			break;
		// Data buffer 6 (MX50/MX51).
		case E_IM_SDRAMC_DATABUF_6 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB6DA = enable;
			break;
		// Data buffer 7 (MX40).
		case E_IM_SDRAMC_DATABUF_7 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB7DA = enable;
			break;
		// Data buffer 8 (MX00/MX01).
		case E_IM_SDRAMC_DATABUF_8 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB8DA = enable;
			break;
		// Data buffer 9 (MX60).
		case E_IM_SDRAMC_DATABUF_9 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB9DA = enable;
			break;
		// Data buffer 10 (RB0).
		case E_IM_SDRAMC_DATABUF_10 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKBADA = enable;
			break;
		// Data buffer 11 (RB1).
		case E_IM_SDRAMC_DATABUF_11 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKBBDA = enable;
			break;
		// Data buffer 12 (RB2).
		case E_IM_SDRAMC_DATABUF_12 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKBCDA = enable;
			break;
		// Data buffer 13 (DS0/DS1).
		case E_IM_SDRAMC_DATABUF_13 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKBDDA = enable;
			break;
		// Data buffer 14 (SDM).
		case E_IM_SDRAMC_DATABUF_14 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKBEDA = enable;
			break;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		// Data buffer 15 (VE0/VE1/VE2).
		case E_IM_SDRAMC_DATABUF_15 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKBFDA = enable;
			break;
		// Data buffer 16 (VD0/VD1).
		case E_IM_SDRAMC_DATABUF_16 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB10DA = enable;
			break;
		// Data buffer 17 (VD2).
		case E_IM_SDRAMC_DATABUF_17:
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKDRBDA = enable;
			break;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		// Data buffer 16 (VD0/VD1).
		case E_IM_SDRAMC_DATABUF_16 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB10DA = enable;
			break;
		// Data buffer 18 (VCPU).
		case E_IM_SDRAMC_DATABUF_18 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB12DA = enable;
			break;
		// Data buffer 19 (VCOREW/VCORER0).
		case E_IM_SDRAMC_DATABUF_19 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB13DA = enable;
			break;
		// Data buffer 20 (VCORER1).
		case E_IM_SDRAMC_DATABUF_20 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB14DA = enable;
			break;
		// Data buffer 21 (VCOREM).
		case E_IM_SDRAMC_DATABUF_21 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKB15DA = enable;
			break;
		// Data buffer 22 (VD2).
		case E_IM_SDRAMC_DATABUF_22 :
			IO_SDRAMC[ ch ].CLKENDA.bit.CLKDRBDA = enable;
			break;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		default :
			im_sdramc_off_pclk( ch );
			Ddim_Assertion(("Im_SDRAMC_Set_Clock_Enable_DataBuf() error. [dbno] = %d\n", dbno));
			return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC self refresh mode start.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Strat_SelfRefresh( E_IM_SDRAMC_CH ch )
{
#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Strat_SelfRefresh() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	if ( IO_SDRAMC[ ch ].UMCSPCSTAT.bit.SLFRST == 0 ) {
		// Self refresh mode start.
		IO_SDRAMC[ ch ].UMCSPCSETA.bit.SRINEN = E_IM_SDRAMC_EN_ENABLE;

		im_sdramc_dsb();

		while( IO_SDRAMC[ ch ].UMCSPCSTAT.bit.SLFRST == 0 ) {
			;	// Do nothing.
		}
	}

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC self refresh mode stop.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Stop_SelfRefresh( E_IM_SDRAMC_CH ch )
{
#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Stop_SelfRefresh() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	if ( IO_SDRAMC[ ch ].UMCSPCSTAT.bit.SLFRST == 1 ) {
		// Self refresh mode stop.
		IO_SDRAMC[ ch ].UMCSPCSETA.bit.SROUTEN = E_IM_SDRAMC_EN_ENABLE;

		im_sdramc_dsb();

		while( IO_SDRAMC[ ch ].UMCSPCSTAT.bit.SLFRST == 1 ) {
			;	// Do nothing.
		}
	}

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC ILP_REQ manual mode start.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Start_ManualMode( E_IM_SDRAMC_CH ch )
{
	union io_sdramc_umcdfictla umcdfictla;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Start_ManualMode() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	umcdfictla.word				= 0;
	umcdfictla.bit.LPCTLMD		= 1;
	umcdfictla.bit.LPREQ_DQDQS	= 1;
	umcdfictla.bit.LPREQ_CACK	= 1;

	IO_SDRAMC[ ch ].UMCDFICTLA.word				= umcdfictla.word;
	IO_SDRAMC[ ch ].UMCDFICTLA.bit.LPREQ_CACK	= 0;
	IO_SDRAMC[ ch ].UMCDFICTLA.bit.LPREQ_DQDQS	= 0;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC ILP_REQ manual mode stop.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Stop_ManualMode( E_IM_SDRAMC_CH ch )
{
#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Stop_ManualMode() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	IO_SDRAMC[ ch ].UMCDFICTLA.bit.LPREQ_DQDQS	= 1;
	IO_SDRAMC[ ch ].UMCDFICTLA.bit.LPREQ_CACK	= 1;
	IO_SDRAMC[ ch ].UMCDFICTLA.bit.LPCTLMD		= 0;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC DLL Re-Lock.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_ReLock_DLL( E_IM_SDRAMC_CH ch )
{
	union io_sdramc_umcphya umcphya;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_ReLock_DLL() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	// DDR-PHY IDLLRSTX/ISOFTRSTX reset.
	umcphya.word					= 0;
	umcphya.bit.DLLRSTX_CACK		= 0x0;
	umcphya.bit.DLLRSTX_DQDQS		= 0x0;
	umcphya.bit.SOFTRSTX_CACK		= 0x0;
	umcphya.bit.SOFTRSTX_DQDQS		= 0x0;
	IO_SDRAMC[ ch ].UMCPHYCTLA.word = umcphya.word;

	// DDR-PHY IDLLRSTX release.
	IO_SDRAMC[ ch ].UMCPHYCTLA.bit.DLLRSTX_CACK			= 0x1;
	IO_SDRAMC[ ch ].UMCPHYCTLA.bit.DLLRSTX_DQDQS		= 0x1;

	// wait(tDLL_lock).
	//  333MHz/ 667Mbps:124us.
	//  400MHz/ 800Mbps: 85us.
	//  533MHz/1066Mbps: 48us.
	//  667MHz/1333Mbps: 30us.
	//  800MHz/1600Mbps: 21us.
	// 1066MHz/2133Mbps: 12us.
	im_sdramc_wait_us( 21 );

	// DLL Lock result update.
	IO_SDRAMC[ ch ].UMCPHYCTLB.bit.DLL_INIT_UPD_CACK	= 0x1;
	IO_SDRAMC[ ch ].UMCPHYCTLB.bit.DLL_INIT_UPD_DQDQS	= 0xF;

	// wait(80ns).
	im_sdramc_wait_ns( 80 );

	// DDR-PHY ISOFTRSTX release.
	IO_SDRAMC[ ch ].UMCPHYCTLA.bit.SOFTRSTX_CACK		= 0x1;
	IO_SDRAMC[ ch ].UMCPHYCTLA.bit.SOFTRSTX_DQDQS		= 0xF;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC DDR-PHY training start.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		mode							DDR-PHY training mode.
													<ul><li>@ref E_IM_SDRAMC_TRAINING_MODE_WRITE.
														<li>@ref E_IM_SDRAMC_TRAINING_MODE_GATE.
														<li>@ref E_IM_SDRAMC_TRAINING_MODE_RDATA_SYNC.
														<li>@ref E_IM_SDRAMC_TRAINING_MODE_RDATA_EYE.
														<li>@ref E_IM_SDRAMC_TRAINING_MODE_WDATA_EYE.
														<li>@ref E_IM_SDRAMC_TRAINING_MODE_RW_TEST.
														<li>@ref E_IM_SDRAMC_TRAINING_MODE_CA.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@retval			D_IM_SDRAMC_NG					Fail - Trainig ng.
*/
INT32 Im_SDRAMC_Start_Training( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_TRAINING_MODE mode )
{
	INT32 ret;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Training() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( mode > E_IM_SDRAMC_TRAINING_MODE_CA ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Training() error. [mode] = %d\n", mode));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	// training start.
	IO_SDRAMC[ ch ].UMCPHYCTLC.bit.ITR_MODE	= mode;
	IO_SDRAMC[ ch ].UMCPHYCTLC.bit.ITR_REQ	= 0x1;

	im_sdramc_dsb();

	// status check.
	while( IO_SDRAMC[ ch ].UMCPHYCTLC.bit.ITR_REQ == 0x1 ) {
		;	// Do nothing.
	}

	// result check.
	if ( IO_SDRAMC[ ch ].UMCPHYCTLC.bit.OTR_RESP == 0x0 ) {
		ret = D_DDIM_OK;
	}
	else {
		ret = D_IM_SDRAMC_NG;
	}

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return ret;
}

/**
SDRAMC access detection start (Particular & Non-implementation area).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		area							Access detection area.
													<ul><li>@ref E_IM_SDRAMC_AREA_A.
														<li>@ref E_IM_SDRAMC_AREA_B.</ul>
@param[in]		acs_det							Access detection table @ref T_IM_SDRAMC_ACCESS_DETECTION.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Start_Access_Detection( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_AREA area, const T_IM_SDRAMC_ACCESS_DETECTION* const acs_det )
{
	union io_sdramc_umcaccrsa*	umcaccrsa;
	union io_sdramc_umcaccrea*	umcaccrea;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	union io_sdramc_umcwaccst*	umcwaccst;
	union io_sdramc_umcraccst*	umcraccst;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	union io_sdramc_umcwaccst*	umcwaccstl;
	union io_sdramc_umcraccst*	umcraccstl;
	union io_sdramc_umcwaccst*	umcwaccstu;
	union io_sdramc_umcraccst*	umcraccstu;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	union io_sdramc_umcacchk*	umcacchk;
	ULONG						int_mask;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Access_Detection() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( area > E_IM_SDRAMC_AREA_B ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Access_Detection() error. [area] = %d\n", area));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( acs_det == NULL ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Access_Detection() error. [acs_det] is NULL\n"));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( ( acs_det->s_addr ) >= ( acs_det->e_addr ) ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Access_Detection() error. [s_addr]=0x%08lx >= [e_addr]=0x%08lx\n", acs_det->s_addr, acs_det->e_addr ));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	if ( area == E_IM_SDRAMC_AREA_A ) {
		umcaccrsa	= (union io_sdramc_umcaccrsa*)&IO_SDRAMC[ ch ].UMCACCARSAA;
		umcaccrea	= (union io_sdramc_umcaccrea*)&IO_SDRAMC[ ch ].UMCACCAREAA;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		umcwaccst	= (union io_sdramc_umcwaccst*)&IO_SDRAMC[ ch ].UMCWACCMSTA;
		umcraccst	= (union io_sdramc_umcraccst*)&IO_SDRAMC[ ch ].UMCRACCMSTA;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		umcwaccstl	= (union io_sdramc_umcwaccst*)&IO_SDRAMC[ ch ].UMCWACCMSTAL;
		umcraccstl	= (union io_sdramc_umcraccst*)&IO_SDRAMC[ ch ].UMCRACCMSTAL;
		umcwaccstu	= (union io_sdramc_umcwaccst*)&IO_SDRAMC[ ch ].UMCWACCMSTAU;
		umcraccstu	= (union io_sdramc_umcraccst*)&IO_SDRAMC[ ch ].UMCRACCMSTAU;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		umcacchk	= (union io_sdramc_umcacchk*)&IO_SDRAMC[ ch ].UMCACCCHKA;
	}
	else {
		umcaccrsa	= (union io_sdramc_umcaccrsa*)&IO_SDRAMC[ ch ].UMCACCARSAB;
		umcaccrea	= (union io_sdramc_umcaccrea*)&IO_SDRAMC[ ch ].UMCACCAREAB;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		umcwaccst	= (union io_sdramc_umcwaccst*)&IO_SDRAMC[ ch ].UMCWACCMSTB;
		umcraccst	= (union io_sdramc_umcraccst*)&IO_SDRAMC[ ch ].UMCRACCMSTB;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		umcwaccstl	= (union io_sdramc_umcwaccst*)&IO_SDRAMC[ ch ].UMCWACCMSTBL;
		umcraccstl	= (union io_sdramc_umcraccst*)&IO_SDRAMC[ ch ].UMCRACCMSTBL;
		umcwaccstu	= (union io_sdramc_umcwaccst*)&IO_SDRAMC[ ch ].UMCWACCMSTBU;
		umcraccstu	= (union io_sdramc_umcraccst*)&IO_SDRAMC[ ch ].UMCRACCMSTBU;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		umcacchk	= (union io_sdramc_umcacchk*)&IO_SDRAMC[ ch ].UMCACCCHKB;
	}

	// Access detection address set (Particular area).
	umcaccrsa->bit.STADDR						= ( acs_det->s_addr >> 12 );
	umcaccrea->bit.ENADDR						= ( ( acs_det->e_addr + 1 ) >> 12 );

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	// Access detection port set (Particular area).
	umcwaccst->bit.WMST							= acs_det->w_port;
	umcraccst->bit.RMST							= acs_det->r_port;

	// INT mask setting (Particular area).
	int_mask = ~( acs_det->w_port | acs_det->r_port );
	IO_SDRAMC[ ch ].UMCINTMASKM0.bit.INTMASK	= int_mask;
	IO_SDRAMC[ ch ].UMCINTMASKM0.bit.RESERVED	= 0x3;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	// Access detection port set (Particular area).
	umcwaccstl->bit.WMST						= acs_det->w_port_l;
	umcraccstl->bit.RMST						= acs_det->r_port_l;
	umcwaccstu->bit.WMST						= acs_det->w_port_u;
	umcraccstu->bit.RMST						= acs_det->r_port_u;

	// INT mask setting (Particular area).
	int_mask = ~( acs_det->w_port_l | acs_det->r_port_l );
	IO_SDRAMC[ ch ].UMCINTMASKM0.bit.INTMASK	= int_mask;
	IO_SDRAMC[ ch ].UMCINTMASKM0.bit.RESERVED	= 0x3;

	int_mask = ~( acs_det->w_port_u | acs_det->r_port_u );
	IO_SDRAMC[ ch ].UMCINTMASKM1.bit.INTMASK	= int_mask;
	IO_SDRAMC[ ch ].UMCINTMASKM1.bit.RESERVED	= 0x3;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	// Access detection setting (Particular area).
	umcacchk->bit.PRCT							= 1;
	umcacchk->bit.BUSERR						= 1;
	umcacchk->bit.INT							= 1;
	umcacchk->bit.CHKEN							= E_IM_SDRAMC_EN_ENABLE;

	// Access detection setting (Non-implementation area).
	IO_SDRAMC[ ch ].UMCOODCHK.bit.PRCT			= 1;
	IO_SDRAMC[ ch ].UMCOODCHK.bit.BUSERR		= 1;
	IO_SDRAMC[ ch ].UMCOODCHK.bit.INT			= 1;
	IO_SDRAMC[ ch ].UMCOODCHK.bit.CHKEN			= E_IM_SDRAMC_EN_ENABLE;

	// User callback set.
	gIM_SDRAMC_CallBack_Func[ ch ]				= acs_det->pCallBack;

	// Int enable.
	IO_SDRAMC[ ch ].IRQEN.bit.IRQEN				= E_IM_SDRAMC_EN_ENABLE;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC access detection stop (Particular & Non-implementation area).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		area							Access detection area.
													<ul><li>@ref E_IM_SDRAMC_AREA_A.
														<li>@ref E_IM_SDRAMC_AREA_B.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Stop_Access_Detection( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_AREA area )
{
	union io_sdramc_umcacchk*	umcacchk;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Stop_Access_Detection() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( area > E_IM_SDRAMC_AREA_B ) {
		Ddim_Assertion(("Im_SDRAMC_Stop_Access_Detection() error. [area] = %d\n", area));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	if ( area == E_IM_SDRAMC_AREA_A ) {
		umcacchk = (union io_sdramc_umcacchk*)&IO_SDRAMC[ ch ].UMCACCCHKA;
	}
	else {
		umcacchk = (union io_sdramc_umcacchk*)&IO_SDRAMC[ ch ].UMCACCCHKB;
	}

	// Int disable.
	IO_SDRAMC[ ch ].IRQEN.bit.IRQEN		= 0;

	// Access detection setting.
	umcacchk->bit.CHKEN					= 0;

	// Access detection setting (Non-implementation area).
	IO_SDRAMC[ ch ].UMCOODCHK.bit.CHKEN	= 0;

	// User callback clear.
	gIM_SDRAMC_CallBack_Func[ ch ]		= NULL;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC access detection information get (Particular area).<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[out]		area							Access detection area.
													<ul><li>@ref E_IM_SDRAMC_AREA_A.
														<li>@ref E_IM_SDRAMC_AREA_B.</ul>
@param[out]		port							Access detection Port number @ref E_IM_SDRAMC_PORT.
@param[out]		addr							Access detection Address.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		Call from user callback function.
@remarks		The unit of addr is 8 byte.
*/
INT32 Im_SDRAMC_Get_Access_Detection( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_AREA *area, E_IM_SDRAMC_PORT* port, ULONG* const addr )
{
#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Access_Detection() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( area == NULL ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Access_Detection() error. [area] is NULL\n"));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( port == NULL ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Access_Detection() error. [port] is NULL\n"));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( addr == NULL ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Access_Detection() error. [addr] is NULL\n"));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	if ( IO_SDRAMC[ ch ].ACCCHKFLG.bit.ACCCHKFLGA == 1 ) {
		IO_SDRAMC[ ch ].ACCCHKFLGCLR.bit.ACCCHKFLGCLRA = 1;
		*area = E_IM_SDRAMC_AREA_A;
	}
	else if ( IO_SDRAMC[ ch ].ACCCHKFLG.bit.ACCCHKFLGB == 1 ) {
		IO_SDRAMC[ ch ].ACCCHKFLGCLR.bit.ACCCHKFLGCLRB = 1;
		*area = E_IM_SDRAMC_AREA_B;
	}
	else {
#ifdef CO_SDRAMC_DEBUG_PRINT
		Ddim_Print(( "Im_SDRAMC_Get_Access_Detection() ACCCHKFLGA/B off\n" ));
#endif
	}

	*port	= (E_IM_SDRAMC_PORT)IO_SDRAMC[ ch ].ACCCHKLOGA.bit.PORTID;
	*addr	= IO_SDRAMC[ ch ].ACCCHKLOGB.bit.DRMADR;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC access monitor start.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		mode							Access monitor mode.
													<ul><li>@ref E_IM_SDRAMC_MONMODE_SIZE.
														<li>@ref E_IM_SDRAMC_MONMODE_COUNT.</ul>
@param[in]		write_access					Write access flag(TRUE:measurement on, FALSE:measurement off).
@param[in]		read_access						Read  access flag(TRUE:measurement on, FALSE:measurement off).
@param[in]		auto_clear						Status reg auto clear flag(TRUE:auto clear on, FALSE:auto clear off).
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Start_Monitor( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_MONMODE mode, BOOL write_access, BOOL read_access, BOOL auto_clear )
{
	UCHAR clkmon;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Monitor() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( mode > E_IM_SDRAMC_MONMODE_COUNT ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Monitor() error. [mode] = %d\n", mode));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( write_access > TRUE ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Monitor() error. [write_access] = %d\n", write_access));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( read_access > TRUE ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Monitor() error. [read_access] = %d\n", read_access));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( auto_clear > TRUE ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Monitor() error. [auto_clear] = %d\n", auto_clear));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	clkmon = IO_SDRAMC[ ch ].CLKENMON.bit.CLKMON;

	if ( clkmon == E_IM_SDRAMC_EN_DISABLE ) {
		// Access monitor clock enable.
		IO_SDRAMC[ ch ].CLKENMON.bit.CLKMON					= E_IM_SDRAMC_EN_ENABLE;

		// Access monitor mode set.
		IO_SDRAMC[ ch ].UMCPFMCTL.bit.PFM_MON_CMD			= mode;

		// Access monitor kind set (read/write).
		if ( read_access ) {
			IO_SDRAMC[ ch ].UMCPFMCTL.bit.PFM_MON_R_OFF		= 0;
		}
		else {
			IO_SDRAMC[ ch ].UMCPFMCTL.bit.PFM_MON_R_OFF		= 1;
		}

		if ( write_access ) {
			IO_SDRAMC[ ch ].UMCPFMCTL.bit.PFM_MON_W_OFF		= 0;
		}
		else {
			IO_SDRAMC[ ch ].UMCPFMCTL.bit.PFM_MON_W_OFF		= 1;
		}

		// Status register auto clear.
		IO_SDRAMC[ ch ].UMCPFMCTL.bit.PFM_RST_AFTER_READ	= auto_clear;

		// Status register clear (on).
		IO_SDRAMC[ ch ].UMCPFMCTL.bit.PFM_RST				= 1;

		// Access monitor start.
		IO_SDRAMC[ ch ].UMCPFMCTL.bit.PFM_ON				= 1;
	}

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC access monitor stop.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Stop_Monitor( E_IM_SDRAMC_CH ch )
{
	UCHAR clkmon;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Stop_Monitor() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	clkmon = IO_SDRAMC[ ch ].CLKENMON.bit.CLKMON;

	if ( clkmon == E_IM_SDRAMC_EN_ENABLE ) {
		// Access monitor stop.
		IO_SDRAMC[ ch ].UMCPFMCTL.bit.PFM_ON	= 0;

		// Access monitor clock disable.
		IO_SDRAMC[ ch ].CLKENMON.bit.CLKMON		= E_IM_SDRAMC_EN_DISABLE;
	}

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC access monitor get.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[out]		mon								Access monitor table @ref T_IM_SDRAMC_MONITOR.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The unit of transfer data size to SDRAM is 8 byte.(Access monitor mode = E_IM_SDRAMC_MONMODE_SIZE)
*/
INT32 Im_SDRAMC_Get_Monitor( E_IM_SDRAMC_CH ch, T_IM_SDRAMC_MONITOR* const mon )
{
	UCHAR port;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Monitor() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( mon == NULL ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Monitor() error. [mon] is null\n"));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	for ( port = 0; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		mon->data[ port ] = IO_SDRAMC[ ch ].UMCPFMST[ port ].bit.VLDDATA;
	}

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC access monitor start for MXIC.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[in]		mode							Access monitor mode.
													<ul><li>@ref E_IM_SDRAMC_MONMODE_SIZE.
														<li>@ref E_IM_SDRAMC_MONMODE_COUNT.</ul>
@param[in]		write_access					Write access flag(TRUE:measurement on, FALSE:measurement off).
@param[in]		read_access						Read  access flag(TRUE:measurement on, FALSE:measurement off).
@param[in]		auto_clear						Status reg auto clear flag(TRUE:auto clear on, FALSE:auto clear off).
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Start_Monitor_Mxic( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_MONMODE mode, BOOL write_access, BOOL read_access, BOOL auto_clear )
{
	UCHAR clkmon;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Monitor_Mxic() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( mode > E_IM_SDRAMC_MONMODE_COUNT ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Monitor_Mxic() error. [mode] = %d\n", mode));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( write_access > TRUE ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Monitor_Mxic() error. [write_access] = %d\n", write_access));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( read_access > TRUE ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Monitor_Mxic() error. [read_access] = %d\n", read_access));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( auto_clear > TRUE ) {
		Ddim_Assertion(("Im_SDRAMC_Start_Monitor_Mxic() error. [auto_clear] = %d\n", auto_clear));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	clkmon = IO_SDRAMC[ ch ].CLKENMON.bit.CLKMON;

	if ( clkmon == E_IM_SDRAMC_EN_DISABLE ) {
		// Access monitor clock enable.
		IO_SDRAMC[ ch ].CLKENMON.bit.CLKMON					= E_IM_SDRAMC_EN_ENABLE;

		// Access monitor mode set.
		IO_SDRAMC[ ch ].UMCMIDPFMCTL.bit.PFM_MON_CMD		= mode;

		// Access monitor kind set (read/write).
		if ( read_access ) {
			IO_SDRAMC[ ch ].UMCMIDPFMCTL.bit.PFM_MON_R_OFF	= 0;
		}
		else {
			IO_SDRAMC[ ch ].UMCMIDPFMCTL.bit.PFM_MON_R_OFF	= 1;
		}

		if ( write_access ) {
			IO_SDRAMC[ ch ].UMCMIDPFMCTL.bit.PFM_MON_W_OFF	= 0;
		}
		else {
			IO_SDRAMC[ ch ].UMCMIDPFMCTL.bit.PFM_MON_W_OFF	= 1;
		}

		// Status register auto clear.
		IO_SDRAMC[ ch ].UMCMIDPFMCTL.bit.PFM_RST_AFTER_READ	= auto_clear;

		// Status register clear (on).
		IO_SDRAMC[ ch ].UMCMIDPFMCTL.bit.PFM_RST			= 1;

		// Access monitor start.
		IO_SDRAMC[ ch ].UMCMIDPFMCTL.bit.PFM_ON				= 1;
	}

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC access monitor stop for MXIC.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
*/
INT32 Im_SDRAMC_Stop_Monitor_Mxic( E_IM_SDRAMC_CH ch )
{
	UCHAR clkmon;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Stop_Monitor_Mxic() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	clkmon = IO_SDRAMC[ ch ].CLKENMON.bit.CLKMON;

	if ( clkmon == E_IM_SDRAMC_EN_ENABLE ) {
		// Access monitor stop.
		IO_SDRAMC[ ch ].UMCMIDPFMCTL.bit.PFM_ON	= 0;

		// Access monitor clock disable.
		IO_SDRAMC[ ch ].CLKENMON.bit.CLKMON		= E_IM_SDRAMC_EN_DISABLE;
	}

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC access monitor get for MXIC.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@param[out]		mon								Access monitor table for MXIC @ref T_IM_SDRAMC_MONITOR_MXIC.
@retval			D_DDIM_OK						Success.
@retval			D_IM_SDRAMC_INPUT_PARAM_ERROR	Fail - Parameter error.
@remarks		The unit of transfer data size to SDRAM is 8 byte.
*/
INT32 Im_SDRAMC_Get_Monitor_Mxic( E_IM_SDRAMC_CH ch, T_IM_SDRAMC_MONITOR_MXIC* const mon )
{
	UCHAR sub_id;

#ifdef CO_PARAM_CHECK
	if ( ch >= E_IM_SDRAMC_CH_MAX ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Monitor_Mxic() error. [ch] = %d\n", ch));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
	if ( mon == NULL ) {
		Ddim_Assertion(("Im_SDRAMC_Get_Monitor_Mxic() error. [mon] is null\n"));
		return D_IM_SDRAMC_INPUT_PARAM_ERROR;
	}
#endif

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	for ( sub_id = 0; sub_id < E_IM_SDRAMC_SUBID_MAX; sub_id++ ) {
		mon->data[ sub_id ] = IO_SDRAMC[ ch ].UMCMIDPFMST[ sub_id ].bit.VLDDATA;
	}

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	return D_DDIM_OK;
}

/**
SDRAMC interupt handler.<br>
@param[in]		ch								SDRAMC channel.
													<ul><li>@ref E_IM_SDRAMC_CH_0.
														<li>@ref E_IM_SDRAMC_CH_1.</ul>
@retval			None.
*/
VOID Im_SDRAMC_Int_Handler( E_IM_SDRAMC_CH ch )
{
	ULONG int_cause;

	im_sdramc_on_pclk( ch );
	im_sdramc_dsb();

	// Int signal clear.
	IO_SDRAMC[ ch ].IRQCLR.bit.IRQCLR = 1;

	// INTCAUSE get.
	int_cause = IO_SDRAMC[ ch ].UMCINTCAUSEM0.bit.INTCAUSE;

	// INTCAUSE clear.
	IO_SDRAMC[ ch ].UMCINTCAUSEM0.bit.INTCAUSE	= 0x3FFFFFFF;
	IO_SDRAMC[ ch ].UMCINTCAUSEM0.bit.RESERVED	= 0x0;

	im_sdramc_off_pclk( ch );
	im_sdramc_dsb();

	// User callback function.
	if ( gIM_SDRAMC_CallBack_Func[ ch ] != NULL ) {
#ifdef CO_SDRAMC_DEBUG_PRINT
		Ddim_Print(( "Im_SDRAMC_Int_Handler() begin callback\n" ));
#endif

		gIM_SDRAMC_CallBack_Func[ ch ]( int_cause );

#ifdef CO_SDRAMC_DEBUG_PRINT
		Ddim_Print(( "Im_DRAMC_Int_Handler() end callback\n" ));
#endif
	}
}
