/**
 * @file		wdog.c
 * @brief		watchdog timer process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "dd_wdog.h"
#include "fj_peripheral.h"
#include "debug.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_WDOG_OPEN_WAIT_TIME	(200)	// Semaphore wait time
#define D_WDOG_PRESCALER_VALUE	(249)	// Adjust the case of PERIPHCLK=100MHz (resolution=2.5u)

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/**
Initialize Watchdog function.
*/
FJ_ERR_CODE fj_wdog_timer_init( VOID )
{
	Dd_WDOG_Init();
	Dd_WDOG_Clear_Interrupt();
	return FJ_ERR_OK;
}

/**
The Watchdog fuction is exclusively controlled.
*/
FJ_ERR_CODE fj_wdog_timer_open( VOID )
{
	INT32 ret;
	
	ret = Dd_WDOG_Open( D_WDOG_OPEN_WAIT_TIME );
	
	if ( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(( "fj_wdog_timer_open() - Error! : ret = %d\n", ret ));
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

/**
Control Watchdog function.
*/
FJ_ERR_CODE fj_wdog_timer_ctrl( FJ_WDOG_MODE wdog_mode, ULONG msec, VP_CALLBACK user_handler )
{
	INT32 ret;
	
	ULONG convert_counter;
	T_DD_WDOG_CTRL wdog_ctrl;
	
	ret = Dd_WDOG_Calculate( (ULONG)wdog_mode, msec, &convert_counter );
	if ( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(( "fj_wdog_timer_ctrl() - Dd_WDOG_Calculate() Error! : ret = %d\n", ret ));
		return FJ_ERR_NG;
	}
	
	wdog_ctrl.wdog_load			= convert_counter;			// Load value (Convert value)
	wdog_ctrl.pCallback			= NULL;						// User's Callback function
	if ( wdog_mode == FJ_WDOG_MODE_TIMER ) {				// Enable the interrupt event
		wdog_ctrl.int_enable = D_DD_WDOG_ENABLE_ON;
		wdog_ctrl.pCallback	 = user_handler;				// User's Callback function
	}
	else {
		wdog_ctrl.int_enable = D_DD_WDOG_ENABLE_OFF;
	}
	if ( wdog_mode == FJ_WDOG_MODE_WDOG ) {					// Enable Watchdog reset output
		wdog_ctrl.reset_enable = D_DD_WDOG_ENABLE_ON;
	}
	else {
		wdog_ctrl.reset_enable = D_DD_WDOG_ENABLE_OFF;
	}
	
	ret = Dd_WDOG_Ctrl( &wdog_ctrl );
	if ( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(( "fj_wdog_timer_ctrl() - Dd_WDOG_Ctrl() Error! : ret = %d\n", ret ));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
Refresh Watchdog function.
*/
FJ_ERR_CODE fj_wdog_timer_refresh( VOID )
{
	Dd_WDOG_Reload();
	return FJ_ERR_OK;
}

/**
Get Watchdog reset status.
*/
FJ_ERR_CODE fj_wdog_timer_get_reset_status( FJ_WDOG_RESET_STATUS* raw_wdog_int, FJ_WDOG_RESET_MASK_STATUS* mask_wdog_int )
{
	UINT32 raw_status;
	UINT32 mask_status;
	
	Dd_WDOG_Get_Status( (UCHAR*)&raw_status, (UCHAR*)&mask_status );
	
	if (raw_status == 1) {
		*raw_wdog_int = FJ_WDOG_RESET_STATUS_OCCUR;
	}
	else {
		*raw_wdog_int = FJ_WDOG_RESET_STATUS_NONE;
	}
	if (mask_status == 1) {
		*mask_wdog_int = FJ_WDOG_RESET_MASK_STATUS_OCCUR;
	}
	else {
		*mask_wdog_int = FJ_WDOG_RESET_MASK_STATUS_NONE;
	}
	return FJ_ERR_OK;
}

/**
Get Load_Value and Prescaler_Value.
*/
FJ_ERR_CODE fj_wdog_timer_get_load_value( ULONG* load_value )
{
	INT32 ret;

	ret = Dd_WDOG_Get_Load( load_value );
	if ( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(( "fj_wdog_timer_get_load_value() - Dd_WDOG_Get_Load() Error! : ret = %d\n", ret ));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
The exclusive control of Watchdog is released.
*/
FJ_ERR_CODE fj_wdog_timer_close( VOID )
{
	Dd_WDOG_Close();
	return FJ_ERR_OK;
}
