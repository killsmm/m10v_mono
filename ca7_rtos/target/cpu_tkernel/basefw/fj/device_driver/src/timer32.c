/**
 * @file	  : timer32.c
 * @brief	  : Peripheral 32 bit reload timer
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>\n
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"


#include "fj_peripheral.h"
#include "timer32.h"
#include "dd_tmr32.h"
#include "debug.h"
#include "os_user_custom.h"

#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_BF_TMR32_TOUT_CH		(3)		// TOUT channel

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		Open Reload timer (assignment 32bit reload timer)
 *	@param		i:ch 		= Ch number
 *	@return		FJ_ERR_OK	= Success
 *				FJ_ERR_NG	= Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_tmr32_open( FJ_PERI_CH ch )
{
	INT32 ret;

	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		BF_Debug_Print_Warning(("Timer32_Open() - Error! : ch = %d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Open dual input timer
	ret = Dd_TMR32_Open(ch, D_DDIM_WAIT_END_TIME);
	if (ret != D_DDIM_OK) {	// dual input timer Open NG?
		BF_Debug_Print_Warning(("Timer32_Open() - Error! : ret = %d\n", ret));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Set Timer control (assignment 32bit reload timer)
 *	@param		i:ch 		= Ch number
 *				i:usec		= waitting time (usec order)
 *				i:pcallback	= Read out value.
 *	@return		FJ_ERR_OK	= Success
 *				FJ_ERR_NG	= Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_tmr32_set( FJ_PERI_CH ch, ULONG usec, VP_CALLBACK pcallback )
{
	INT32 ret;

	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		BF_Debug_Print_Warning(("Timer32_Set() - Error! : ch = %d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Set dual input timer
	ret = Dd_TMR32_SetTimer(ch, usec, pcallback);
	if (ret != D_DDIM_OK) {	// Dual input timer Set NG?
		BF_Debug_Print_Warning(("Timer32_Set() - Error! : ret = %d\n", ret));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}


/**
 *	@brief		Set Timer control (assignment 32bit reload timer)
 *	@param		i:reload_ch = Ch number
 *				i:pwm_ch	= Ch number
 				i:mode		= 0:for 60fps, 1:for 240fps
 *				i:pcallback	= Read out value.
 *	@return		FJ_ERR_OK	= Success
 *				FJ_ERR_NG	= Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_tmr32_set_pwm( FJ_PERI_CH reload_ch, FJ_PERI_CH pwm_ch, UCHAR mode, VP_CALLBACK pcallback )
{
	INT32 ret;

	if (reload_ch >= D_DD_TMR32_CH_NUM_MAX) {
		BF_Debug_Print_Warning(("fj_tmr32_set_pwm() - Error! : reload_ch = %d\n", reload_ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if ( (pwm_ch != 4) && (pwm_ch != 8) ) {
		BF_Debug_Print_Warning(("fj_tmr32_set_pwm() - Error! : pwm_ch = %d\n", pwm_ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if ( (mode != 0) && (mode != 1) ) {
		BF_Debug_Print_Warning(("fj_tmr32_set_pwm() - Error! : mode = %d\n", mode));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Set dual input timer
	ret = Dd_TMR32_SetTimer_pwm(reload_ch, pwm_ch, mode, pcallback);
	if (ret != D_DDIM_OK) {	// Dual input timer Set NG?
		BF_Debug_Print_Warning(("Dd_TMR32_SetTimer_pwm() - Error! : ret = %d\n", ret));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Set Timer control parameters (assignment 32bit reload timer)
 *	@param		i:ch 		= Ch number
 *				i:ctrl_param= Control Parameters
 *				i:pcallback	= Read out value.
 *	@return		FJ_ERR_OK	= Success
 *				FJ_ERR_NG	= Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_tmr32_ctrl( FJ_PERI_CH ch, FJ_TMR32_CTRL const* const ctrl_param, VP_CALLBACK pcallback )
{
	INT32 ret;
	T_DD_TMR32_CTRL tmr32_ctrl;

	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		BF_Debug_Print_Warning(("Timer32_Ctrl() - Error! : ch = %d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Set Control Parameters
	memset(&tmr32_ctrl, 0, sizeof(T_DD_TMR32_CTRL));
	tmr32_ctrl.csr.word		= ctrl_param->csr.word;
	tmr32_ctrl.rlr1 		= ctrl_param->rlr1;
	tmr32_ctrl.rlr2 		= ctrl_param->rlr2;
	tmr32_ctrl.pcallback	= pcallback;
	
	// Set control parameters
	ret = Dd_TMR32_Ctrl(ch, &tmr32_ctrl);
	if (ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("Timer32_Ctrl() - Error! : ret = %d\n", ret));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Set TOUT contrl (assignment 32bit reload timer ch3)
 *	@param		i:ch 		= Ch number
 *				i:enable	= TOUT enable. (=TRUE:enable, =FALSE:disable)
 *				i:out_level	= Out level (0/1)
 *	@return		FJ_ERR_OK	= Success
 *				FJ_ERR_NG	= Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_tmr32_tout_ctrl( FJ_PERI_CH ch, BOOL enable, UINT32 out_level )
{
	INT32 ret;

	if (ch != D_BF_TMR32_TOUT_CH) {
		BF_Debug_Print_Warning(("Timer32_TOUT_Ctrl() - Ch number Error! : ch = %d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if (enable == TRUE) {
		FJ_Gpio_Set_Function( FJ_GPIO_P82, FJ_GPIO_FUNC_HWMODE);

		// Out level set
		ret = Dd_TMR32_Set_Outlevel(out_level);
		if (ret != D_DDIM_OK) {
			BF_Debug_Print_Warning(("Timer32_TOUT_Ctrl() - Out level Error! : ret = %d\n", ret));
			return FJ_ERR_NG;
		}
	}
	else {
		FJ_Gpio_Set_Function( FJ_GPIO_P82, FJ_GPIO_FUNC_GPIO);
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Start Reload timer (assignment 32bit reload timer)
 *	@param		i:ch 		= Ch number
 *				i:reload	= Operation mode (=TRUE:Reload mode, =FALSE:One-shot mode)
 *	@return		FJ_ERR_OK	= Success
 *				FJ_ERR_NG	= Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_tmr32_start( FJ_PERI_CH ch, BOOL reload )
{
	INT32 ret;

	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		BF_Debug_Print_Warning(("Timer32_Start() - Error! : ch = %d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Timer start
	if (reload == TRUE) {	// Reload mode?
		ret = Dd_TMR32_StartReload(ch);
	}
	else {					// One-shot mode
		ret = Dd_TMR32_StartOneshot(ch);
	}

	if (ret != D_DDIM_OK) {	// Dual input timer Start NG?
		BF_Debug_Print_Warning(("Timer32_Start() - Error! : ret = %d\n", ret));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Stop Reload timer (assignment 32bit reload timer)
 *	@param		i:ch 		= Ch number
 *	@return		FJ_ERR_OK	= Success
 *	@return		FJ_ERR_NG	= Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_tmr32_stop( FJ_PERI_CH ch )
{
	INT32 ret;

	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		BF_Debug_Print_Warning(("Timer32_Stop() - Error! : ch = %d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	ret = Dd_TMR32_Stop(ch);
	if(ret != D_DDIM_OK){
		BF_Debug_Print_Warning(("Timer32_Stop() - Error! : ret = %d\n", ret));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Close Reload timer (assignment 32bit reload timer)
 *	@param		i:ch 		= Ch number
 *	@return		FJ_ERR_OK	= Success
 *	@return		FJ_ERR_NG	= Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_tmr32_close( FJ_PERI_CH ch )
{
	INT32 ret;

	if (ch >= D_DD_TMR32_CH_NUM_MAX) {
		BF_Debug_Print_Warning(("Timer32_Close() - Error! : ch = %d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	ret = Dd_TMR32_Close(ch);
	if(ret != D_DDIM_OK){
		BF_Debug_Print_Warning(("Timer32_Close() - Error! : ret = %d\n", ret));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}
