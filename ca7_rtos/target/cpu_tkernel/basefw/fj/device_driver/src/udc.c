/**
 * @file	  : udc.c
 * @brief	  : Up down counter
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>\n
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "fj_peripheral.h"
#include "udc.h"
#include "dd_udc.h"
#include "dd_top.h"
#include "debug.h"
#include "os_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// UDC channel count
#define D_BF_UDC_CH_COUNT		(6)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Information for UDC channel control.
typedef struct {
	UDC_CALLBACK	host_cb;	// Callback function pointer.
} T_BF_UDC_MNG;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Information for UDC channel control.
static volatile T_BF_UDC_MNG gBF_UDC_Mng[D_BF_UDC_CH_COUNT];

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
Callback function for Up/Down and Phase-Lag function.
*/
static VOID udc_bf_callback( UCHAR ch, INT32 factor )
{
	FJ_UDC_INT_FACTOR int_factor;

	/* Factor converting */
	switch (factor) {
		case D_DD_UDC_CMPF_OK:
			int_factor = FJ_UDC_INT_FACTOR_COMPARE;
			break;

		case D_DD_UDC_OVER_FLOW:
			int_factor = FJ_UDC_INT_FACTOR_OVER_FLOW;
			break;

		case D_DD_UDC_UNDER_FLOW:
			int_factor = FJ_UDC_INT_FACTOR_UNDER_FLOW;
			break;

		case D_DD_UDC_COUNT_DIRECTION:
			int_factor = FJ_UDC_INT_FACTOR_CNT_DIR;
			break;

		default:
			// DO NOTHING
			return;
	}
	if (gBF_UDC_Mng[ch].host_cb != NULL) {
		(*gBF_UDC_Mng[ch].host_cb)((FJ_PERI_CH)ch, int_factor);
	}
}

/**
Callback function for timer function.
*/
static VOID udc_bf_callback_timer( UCHAR ch )
{
	if (gBF_UDC_Mng[ch].host_cb != NULL) {
		(*gBF_UDC_Mng[ch].host_cb)((FJ_PERI_CH)ch, FJ_UDC_INT_FACTOR_UNDER_FLOW);
	}
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		Initialize the up down counter for all channels.
 *	@param		i:zin_ctrl	Select Z-terminal enable.
 *	@return		FJ_ERR_OK				Success.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_udc_init( FJ_UDC_ZIN_CTRL zin_ctrl )
{
// MILB_Porting
#if 0
	Dd_Top_Set_PERSEL_UDCTRG( zin_ctrl );
	Dd_UDC_Init();
#endif
	return FJ_ERR_OK;
}

/**
 *	@brief		Check that the specified channel is not used.
				Then open up down counter.
 *	@param		i:ch					Channel number(range of 0..3).
 *	@return		FJ_ERR_OK				Success.
 *	@return		FJ_ERR_NG				Channel open error. (System using designated channel number)
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@note		None
 *	@attention	None
 */

FJ_ERR_CODE fj_udc_open( FJ_PERI_CH ch )
{
	INT32 result;

	if( ch >= D_BF_UDC_CH_COUNT ){
		// Channel number is illegal.
		BF_Debug_Print_Warning(("fj_udc_open() - Channel number error! : ch=%d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Open channel
	result = Dd_UDC_Open( ch, D_DDIM_WAIT_END_TIME );
	if( result != D_DDIM_OK ){
		// UDC macro returned error.
		BF_Debug_Print_Warning(("fj_udc_open() - Channel is locked! : ch=%d, result=%d\n", ch, result));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Close up down counter, and specified channel release.
 *	@param		i:ch					Channel number(range of 0..3).
 *	@return		FJ_ERR_OK				Success.
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail(corresponding to an unexpected error).
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_udc_close( FJ_PERI_CH ch )
{
	INT32 result;

	if( ch >= D_BF_UDC_CH_COUNT ){
		// Channel number is illegal.
		BF_Debug_Print_Warning(("fj_udc_close() - Channel number error! : ch=%d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Close channel
	result = Dd_UDC_Close( ch );
	if (result != D_DDIM_OK) {
		// UDC macro returned error.
		BF_Debug_Print_Warning(("fj_udc_close() - Channel close error! : ch=%d, result=%d\n", ch, result));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Start the operation of up down counter.
 *	@param		i:ch					Channel number(range of 0..3).
 *	@return		FJ_ERR_OK				Success.
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail(corresponding to an unexpected error).
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_udc_start( FJ_PERI_CH ch )
{
	INT32 result;

	if( ch >= D_BF_UDC_CH_COUNT ){
		// Channel number is illegal.
		BF_Debug_Print_Warning(("fj_udc_start() - Channel number error! : ch=%d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Start UDC
	result = Dd_UDC_Start( ch );
	if (result != D_DDIM_OK) {
		// UDC macro returned error.
		BF_Debug_Print_Warning(("fj_udc_start() - Channel start error! : ch=%d, result=%d\n", ch, result));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Stop the operation of up down counter.
 *	@param		i:ch					Channel number(range of 0..3).
 *	@return		FJ_ERR_OK				Success.
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@return		FJ_ERR_NG				Fail(corresponding to an unexpected error).
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_udc_stop( FJ_PERI_CH ch )
{
	INT32 result;

	if( ch >= D_BF_UDC_CH_COUNT ){
		// Channel number is illegal.
		BF_Debug_Print_Warning(("fj_udc_stop() - Channel number error! : ch=%d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Stop UDC.
	result = Dd_UDC_Stop( ch );
	if (result != D_DDIM_OK) {
		// UDC macro returned error.
		BF_Debug_Print_Warning(("fj_udc_stop() - Channel stop error! : ch=%d, result=%d\n", ch, result));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Set parameter and callback function for timer mode
 *	@param		i:ch					Channel number.
 *	@param		i:usec					Count time(usec order).<br>
 *	@param		in:pCallback			Pointer of function that does call backing when timer ends.
										The "factor" parameter of the callback function is unnecessary.
										Please disregard the value returned by the "factor" parameter.
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error
 *	@return		FJ_ERR_NG_CONDITION		Up down counter busy.
 *	@return		FJ_ERR_NG				Fail(corresponding to an unexpected error).
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_udc_set_timer_mode( FJ_PERI_CH ch, ULONG usec, UDC_CALLBACK pCallback )
{
	INT32 result;

	if( ch >= D_BF_UDC_CH_COUNT ){
		// Channel number is illegal.
		BF_Debug_Print_Warning(("fj_udc_set_timermode() - Channel number error! : ch=%d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( usec == 0 ){
		// usec parameter is too small.
		BF_Debug_Print_Warning(("fj_udc_set_timermode() - usec parameter error! : usec=%lu\n", usec));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Save host callback function.
	gBF_UDC_Mng[ch].host_cb = pCallback;

	// Set BaseF/W callback function to UDC driver.
	result = Dd_UDC_Write_Timer_Mode( ch, usec, udc_bf_callback_timer );
	if( result != D_DDIM_OK ){
		if ( result == D_DD_UDC_ERROR_BUSY ) {
			// UDC macro busy error.
			BF_Debug_Print_Warning(("fj_udc_set_timermode() - UDC macro busy error! : ch=%d, result=%d\n", ch, result));
			return FJ_ERR_NG_CONDITION;
		}
		else {
			// UDC macro returned error.
			BF_Debug_Print_Warning(("fj_udc_set_timermode() - Timer setting error! : ch=%d, result=%d\n", ch, result));
			return FJ_ERR_NG;
		}
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Set parameter and callback function for up down count mode
 *	@param		i:ch					Channel number(range of 0..3).
 *	@param		i:edge					Edge selection.
 *	@param		i:udc_ctrl				Each mode common settings parameter pointer.
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input parameter error
 *	@return		FJ_ERR_NG_CONDITION		Up down counter function busy
 *	@return		FJ_ERR_NG				Fail(corresponding to an unexpected error).
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_udc_set_up_down_count_mode( FJ_PERI_CH ch, FJ_UDC_COUNT_EDGE edge, FJ_UDC_CTRL const* const udc_crl )
{
	INT32 result;
	T_DD_UDC_CTRL_CMN ctrl_cmn;

	// Parameter check
	if( ch >= D_BF_UDC_CH_COUNT ){
		// Channel number is illegal.
		BF_Debug_Print_Warning(("fj_udc_set_updowncountmode() - Channel number error! : ch=%d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( udc_crl == NULL ){
		// udc_crl parameter is NULL.
		BF_Debug_Print_Warning(("fj_udc_set_updowncountmode() - udc_crl parameter is NULL!"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	switch (edge) {
		case FJ_UDC_COUNT_EDGE_DISABLE:
		case FJ_UDC_COUNT_EDGE_FALL:
		case FJ_UDC_COUNT_EDGE_RISE:
		case FJ_UDC_COUNT_EDGE_BOTH:
			// DO NOTHING
			break;
		default:
			// edge parameter is illegal.
			BF_Debug_Print_Warning(("fj_udc_set_updowncountmode() - edge parameter error! : edge=%d\n", edge));
			return FJ_ERR_NG_INPUT_PARAM;
	}

	// Save host callback function.
	gBF_UDC_Mng[ch].host_cb = udc_crl->pCallBack;

	ctrl_cmn.count_mode  = D_DD_UDC_CMS_UP_DOWN;
	ctrl_cmn.udcr        = udc_crl->up_down_counter;
	ctrl_cmn.rcr         = udc_crl->comp_counter;
	ctrl_cmn.comp_clear  = udc_crl->comp_clear;
	ctrl_cmn.reload      = udc_crl->reload;
	ctrl_cmn.zin_mode    = udc_crl->zin_mode;
	ctrl_cmn.zin_edge    = udc_crl->zin_edge;
	ctrl_cmn.cmp_int     = udc_crl->comp_cb_enable;
	ctrl_cmn.under_over  = udc_crl->under_over_cb_enable;
	ctrl_cmn.cnt_dir_int = udc_crl->cnt_dir_cb_enable;
	ctrl_cmn.pCallBack   = udc_bf_callback;

	// Set common parameter
	result = Dd_UDC_Ctrl_Common( ch, &ctrl_cmn );
	if( result != D_DDIM_OK ){
		if ( result == D_DD_UDC_ERROR_BUSY ) {
			// UDC macro busy error.
			BF_Debug_Print_Warning(("fj_udc_set_updowncountmode() - UDC macro busy error! : ch=%d, result=%d\n", ch, result));
			return FJ_ERR_NG_CONDITION;
		}
		else {
			// UDC macro returned error.
			BF_Debug_Print_Warning(("fj_udc_set_updowncountmode() - UDC common setting error! : ch=%d, result=%d\n", ch, result));
			return FJ_ERR_NG;
		}
	}

	// Set up/down counter parameter
	result = Dd_UDC_Ctrl_Up_Down( ch, edge );
	if( result != D_DDIM_OK ) {
		// UDC macro returned error.
		BF_Debug_Print_Warning(("fj_udc_set_updowncountmode() - UDC up/down count setting error! : ch=%d, result=%d\n", ch, result));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Set parameter and callback function for phase lag mode.
 *	@param		i:ch					Channel number(range of 0..3).
 *	@param		i:count_mode			Phase lag count mode selection.
 *	@param 		i:udc_ctrl				Each mode common settings parameter pointer.
 *	@return		FJ_ERR_OK				Success
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input parameter error
 *	@return		FJ_ERR_NG_CONDITION		Up down counter function busy
 *	@return		FJ_ERR_NG				Fail(corresponding to an unexpected error).
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_udc_set_phase_lag_mode( FJ_PERI_CH ch, FJ_UDC_PHASE_COUNT count_mode, FJ_UDC_CTRL const* const udc_crl )
{
	INT32 result;
	T_DD_UDC_CTRL_CMN ctrl_cmn;

	// Parameter check
	if( ch >= D_BF_UDC_CH_COUNT ){
		// Channel number is illegal.
		BF_Debug_Print_Warning(("fj_udc_set_phase_lag_mode() - Channel number error! : ch=%d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( udc_crl == NULL ){
		// udc_crl parameter is NULL.
		BF_Debug_Print_Warning(("fj_udc_set_phase_lag_mode() - udc_crl parameter is NULL!"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	switch (count_mode) {
		case FJ_UDC_PHASE_COUNT_SINGLE:
			ctrl_cmn.count_mode = D_DD_UDC_CMS_PHASE_LAG_2;
			break;

		case FJ_UDC_PHASE_COUNT_DOUBLE:
			ctrl_cmn.count_mode = D_DD_UDC_CMS_PHASE_LAG_4;
			break;

		default:
			// count_mode parameter is illegal.
			BF_Debug_Print_Warning(("fj_udc_set_phase_lag_mode() - count_mode parameter error! : count_mode=%d\n", count_mode));
			return FJ_ERR_NG_INPUT_PARAM;
	}

	// Save host callback function.
	gBF_UDC_Mng[ch].host_cb = udc_crl->pCallBack;

	ctrl_cmn.udcr        = udc_crl->up_down_counter;
	ctrl_cmn.rcr         = udc_crl->comp_counter;
	ctrl_cmn.comp_clear  = udc_crl->comp_clear;
	ctrl_cmn.reload      = udc_crl->reload;
	ctrl_cmn.zin_mode    = udc_crl->zin_mode;
	ctrl_cmn.zin_edge    = udc_crl->zin_edge;
	ctrl_cmn.cmp_int     = udc_crl->comp_cb_enable;
	ctrl_cmn.under_over  = udc_crl->under_over_cb_enable;
	ctrl_cmn.cnt_dir_int = udc_crl->cnt_dir_cb_enable;
	ctrl_cmn.pCallBack   = udc_bf_callback;

	// Set common parameter
	result = Dd_UDC_Ctrl_Common( ch, &ctrl_cmn );
	if( result != D_DDIM_OK ){
		if ( result == D_DD_UDC_ERROR_BUSY ) {
			// UDC macro busy error.
			BF_Debug_Print_Warning(("fj_udc_set_phaselagmode() - UDC macro busy error! : ch=%d, result=%d\n", ch, result));
			return FJ_ERR_NG_CONDITION;
		}
		else {
			// UDC macro returned error.
			BF_Debug_Print_Warning(("fj_udc_set_phaselagmode() - UDC common setting error! : ch=%d, result=%d\n", ch, result));
			return FJ_ERR_NG;
		}
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Set counter of the up down counter.
 *	@param		i:ch					Channel number(range of 0..3).
 *	@param		i:up_down_counter		Counter of the up down counter.
 *	@return		FJ_ERR_OK				Success.
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input parameter error.
 *	@return		FJ_ERR_NG_CONDITION		Up down counter function busy.
 *	@return		FJ_ERR_NG				Fail(corresponding to an unexpected error).
 *	@note		None
 *	@attention	When the channel is working, it becomes an error.
 */
FJ_ERR_CODE fj_udc_set_up_down_counter( FJ_PERI_CH ch, USHORT up_down_counter )
{
	INT32 result;

	// Parameter check
	if( ch >= D_BF_UDC_CH_COUNT ){
		// Channel number is illegal.
		BF_Debug_Print_Warning(("fj_udc_set_updown_counter() - Channel number error! : ch=%d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Set UDCR
	result = Dd_UDC_Set_UDCR_Counter( ch, up_down_counter );
	if( result != D_DDIM_OK ){
		if ( result == D_DD_UDC_ERROR_BUSY ) {
			// UDC macro busy error.
			BF_Debug_Print_Warning(("fj_udc_set_updown_counter() - UDC macro busy error! : ch=%d, result=%d\n", ch, result));
			return FJ_ERR_NG_CONDITION;
		}
		else {
			// UDC macro returned error.
			BF_Debug_Print_Warning(("fj_udc_set_updown_counter() - UDC macro setting error! : ch=%d, result=%d\n", ch, result));
			return FJ_ERR_NG;
		}
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Return counter of the up down counter.
 *	@param		i:ch					Channel number(range of 0..3).
 *	@return 	0						Input Parameter Error.
 *	@return		USHORT 					Counter of the up down counter.
 *	@note		None
 *	@attention	None
 */
USHORT fj_udc_get_up_down_counter( FJ_PERI_CH ch )
{
	// Parameter check
	if( ch >= D_BF_UDC_CH_COUNT ){
		// Channel number is illegal.
		BF_Debug_Print_Warning(("fj_udc_get_updown_counter() - Channel number error! : ch=%d\n", ch));
		return 0;
	}
	return Dd_UDC_Get_UDCR_Counter( ch );
}

/**
 *	@brief		Clear counter of the up down counter.
 *	@param		i:ch					Channel number(range of 0..3).
 *	@return		FJ_ERR_OK				Success.
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@retval		FJ_ERR_NG				Fail(corresponding to an unexpected error).
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_udc_clear_up_down_counter( FJ_PERI_CH ch )
{
	INT32 result;

	// Parameter check
	if( ch >= D_BF_UDC_CH_COUNT ){
		// Channel number is illegal.
		BF_Debug_Print_Warning(("fj_udc_clear_updown_counter() - Channel number error! : ch=%d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	// Clear UDCR
	result = Dd_UDC_Force_Clear_UDCR( ch );
	if( result != D_DDIM_OK ){
		// UDC macro returned error.
		BF_Debug_Print_Warning(("fj_udc_clear_updown_counter() - Up/down counter clear error! : ch=%d, result=%d\n", ch, result));
		return FJ_ERR_NG;
	}	
	return FJ_ERR_OK;
}

/**
 *	@brief		Set reload/compare counter of the up down counter.
 *	@param		i:ch					Channel number(range of 0..3).
 *	@return		FJ_ERR_OK				Success.
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input parameter error.
 *	@return		FJ_ERR_NG				Fail(corresponding to an unexpected error).
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_udc_set_comp_counter( FJ_PERI_CH ch, USHORT comp_counter )
{
	INT32 result;

	// Parameter check
	if( ch >= D_BF_UDC_CH_COUNT ){
		// Channel number is illegal.
		BF_Debug_Print_Warning(("fj_udc_set_comp_counter() - Channel number error! : ch=%d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Set RCR
	result = Dd_UDC_Set_RCR_Counter( ch, comp_counter );
	if( result != D_DDIM_OK ){
		// UDC macro returned error.
		BF_Debug_Print_Warning(("fj_udc_set_comp_counter() - Compare counter setting error! : ch=%d, result=%d\n", ch, result));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Get direction of the count immediately before.
 *	@param		i:ch						Channel number(range of 0..3).
 *	@return		FJ_UDC_COUNT_DIR_NO_INPUT	Input Parameter Error.
 *	@return		FJ_UDC_COUNT_DIR 			Flag of direction of the count.
 *	@note		None
 *	@attention	None
 */
FJ_UDC_COUNT_DIR fj_udc_get_up_down_flag( FJ_PERI_CH ch )
{
	// Parameter check
	if( ch >= D_BF_UDC_CH_COUNT ){
		// Channel number is illegal.
		BF_Debug_Print_Warning(("fj_udc_get_updown_flag() - Channel number error! : ch=%d\n", ch));
		return FJ_UDC_COUNT_DIR_NO_INPUT;
	}
	return (FJ_UDC_COUNT_DIR)Dd_UDC_Get_Up_Down_Flg( ch );
}

/**
 *	@brief		Set callback function for when an interrupt occurs.
 *	@param		i:ch					Channel number(range of 0..3).
 *	@param		i:pCallBack				Call Back function pointer.
 *	@return		FJ_ERR_OK				Success.
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_udc_set_callback( FJ_PERI_CH ch, UDC_CALLBACK pCallBack )
{
	// Parameter check
	if( ch >= D_BF_UDC_CH_COUNT ){
		// Channel number is illegal.
		BF_Debug_Print_Warning(("fj_udc_set_callback() - Channel number error! : ch=%d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	gBF_UDC_Mng[ch].host_cb = pCallBack;
	return FJ_ERR_OK;
}

