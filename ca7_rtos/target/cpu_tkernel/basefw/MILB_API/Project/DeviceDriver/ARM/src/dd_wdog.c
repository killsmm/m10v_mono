/**
 * @file		dd_wdog.c
 * @brief		Watchdog Timer driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dd_wdog.h"
#include "dd_arm.h"
#include "dd_top.h"
#include "arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Initialize of Watchdog Load (32bit) */
#define D_DD_WDOG_LOAD_INIT					(0xFFFFFFFF)
/* Max value of Watchdog Counter (32bit) */
#define D_DD_WDOG_COUNTER_MAX_VALUE			(0xFFFFFFFF)

/* Watchdog Lock Register - enables write access to all other registers */
#define D_DD_WDOG_REGISTER_ACCESS_ENABLE	(0x1ACCE551)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//* Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static volatile ULONG gDD_Wdog_Soft_Counter = 0;				/** How many Watchdog Counter is reloaded */
static volatile ULONG gDD_Wdog_Remain_Number = 0;				/** Rest of Watchdog Counter divided by 0xFFFFFFFF multiple */
static volatile VP_CALLBACK	gDD_Wdog_Callback = NULL;			/** Pointer of Interrupt Callback Function */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/**
 * @brief  	Initialize Watchdog Register.
 */
VOID Dd_WDOG_Init(VOID)
{
	gDD_Wdog_Soft_Counter			= 0;
	gDD_Wdog_Remain_Number			= 0;
	
	// register write access enable.
	Dd_WDOG_Set_Lock(D_DD_WDOG_ENABLE_ON);
	// register write test disable.
	Dd_WDOG_Set_Test_Mode(D_DD_WDOG_ENABLE_OFF, D_DD_WDOG_ENABLE_OFF, D_DD_WDOG_ENABLE_OFF);

	// WDOGCONTROL
	IO_WDOG.WDOGCONTROL.bit.RESEN	= D_DD_WDOG_ENABLE_OFF;		// disable the reset.
	IO_WDOG.WDOGCONTROL.bit.INTEN	= D_DD_WDOG_ENABLE_OFF;		// disable the interrupt.
	// WDOGLOAD
//	IO_WDOG.WDOGLOAD				= D_DD_WDOG_LOAD_INIT;		// Initialize Watchdog Load
}

/**
 * @brief	The Watchdog fuction is exclusively controlled.
 */
INT32 Dd_WDOG_Open(INT32 tmout)
{
	DDIM_USER_ER	ercd;
	DDIM_USER_ID	sid;
	
#ifdef CO_PARAM_CHECK
	if( tmout < D_DDIM_USER_SEM_WAIT_FEVR ){
		Ddim_Assertion(("Dd_WDOG_Open: input param error. tmout = %d\n",tmout));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif	// CO_PARAM_CHECK
	sid = SID_DD_WDOG;
	
	if( D_DDIM_USER_SEM_WAIT_POL == tmout ){
		ercd = DDIM_User_Pol_Sem(sid);							// pol_sem()
	}
	else{
		ercd = DDIM_User_Twai_Sem(sid, (DDIM_USER_TMO)tmout);	// twai_sem()
	}
	
	if( D_DDIM_USER_E_OK != ercd ){
		if( D_DDIM_USER_E_TMOUT == ercd ){
			return D_DD_WDOG_SEM_TIMEOUT;
		}
		return D_DD_WDOG_SEM_NG;
	}
	return D_DDIM_OK;
}

/**
 * @brief	Setting of common control variable for Watchdog Counter.
 */
INT32 Dd_WDOG_Ctrl(T_DD_WDOG_CTRL* const wdog_ctrl)
{
#ifdef CO_PARAM_CHECK
	if( wdog_ctrl == NULL ){
		Ddim_Assertion(("Dd_WDOG_Ctrl: input param error. [*wdog_ctrl] NULL\n"));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif
	// Set Callback function.
	gDD_Wdog_Callback = wdog_ctrl->pCallback;

	Dd_WDOG_Set_Load(wdog_ctrl->wdog_load);					// Set value of WDOGLOAD register
	Dd_WDOG_Set_Control(wdog_ctrl);							// Set value of WDOGCONTROL register
	
	return D_DDIM_OK;
}

/**
 * @brief	Reload Watchdog Counter.
 */
VOID Dd_WDOG_Reload(VOID)
{
	Dd_WDOG_Set_Load(IO_WDOG.WDOGLOAD);						// Set value of WDOGLOAD register
	Dd_ARM_Dsb_Pou();
}

/**
 * @brief	The exclusive control of Watchdog is released.
 */
INT32 Dd_WDOG_Close(VOID)
{
	DDIM_USER_ER	ercd;
	DDIM_USER_ID	sid;
	
	sid = SID_DD_WDOG;
	
	ercd = DDIM_User_Sig_Sem(sid);							// sig_sem()
	
	if( D_DDIM_USER_E_OK != ercd ){
		return D_DD_WDOG_SEM_NG;
	}
	return D_DDIM_OK;
}

/**
 * @brief	Get value of WDOGLOAD register.
 */
INT32 Dd_WDOG_Get_Load(ULONG* wdog_load)
{
#ifdef CO_PARAM_CHECK
	if( wdog_load == NULL ){
		Ddim_Assertion(("Dd_WDOG_Get_Load: input param error. [*wdog_load] NULL\n"));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif
	*wdog_load = IO_WDOG.WDOGLOAD;							// Get value of WDOGLOAD register
	
	return D_DDIM_OK;
}

/**
 * @brief	Get value of WDOGVALUE register.
 */
INT32 Dd_WDOG_Get_Counter(ULONG* wdog_counter)
{
#ifdef CO_PARAM_CHECK
	if( wdog_counter == NULL ){
		Ddim_Assertion(("Dd_WDOG_Get_Counter: input param error. [*wdog_counter] NULL\n"));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif
	*wdog_counter	= IO_WDOG.WDOGVALUE;					// Get value of WDOGVALUE register
	
	return D_DDIM_OK;
}

/**
 * @brief	Get value of WDOGCONTROL register.
 */
INT32 Dd_WDOG_Get_Control(T_DD_WDOG_CTRL* wdog_ctrl)
{
#ifdef CO_PARAM_CHECK
	if( wdog_ctrl == NULL ){
		Ddim_Assertion(("Dd_WDOG_Get_Control: input param error. [*wdog_ctrl] NULL\n"));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif
	wdog_ctrl->reset_enable	= IO_WDOG.WDOGCONTROL.bit.RESEN;			// Get value of reset enable.
	wdog_ctrl->int_enable	= IO_WDOG.WDOGCONTROL.bit.INTEN;			// Get value of interrupt enable.
	
	return D_DDIM_OK;
}

/**
 * @brief	Get value of WDOGRIS/WDOGMIS register.
 */
INT32 Dd_WDOG_Get_Status(UCHAR* raw_wdog_int, UCHAR* mask_wdog_int)
{
#ifdef CO_PARAM_CHECK
	if((raw_wdog_int == NULL) || (mask_wdog_int == NULL)){
		Ddim_Assertion(("Dd_WDOG_Get_Status: input param error. [*wdog_interrupt] NULL\n"));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif
	*raw_wdog_int	= IO_WDOG.WDOGRIS.bit.STATUS;			// Get value of WDOGRIS register
	*mask_wdog_int	= IO_WDOG.WDOGMIS.bit.STATUS;			// Get value of WDOGMIS register
	
	return D_DDIM_OK;
}

/**
 * @brief	Get value of WDOGLOCK register.
 */
INT32 Dd_WDOG_Get_Lock(UCHAR* wdog_lock)
{
#ifdef CO_PARAM_CHECK
	if( wdog_lock == NULL ){
		Ddim_Assertion(("Dd_WDOG_Get_Lock: input param error. [wdog_lock] NULL\n"));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif
	// Get WDOGLOCK
	*wdog_lock	= IO_WDOG.WDOGLOCK.bit.EN_STATUS;
	
	return D_DDIM_OK;
}

/**
 * @brief	Get value of WDOGITCR register.
 */
INT32 Dd_WDOG_Get_Test_Mode(UCHAR* test_mode)
{
#ifdef CO_PARAM_CHECK
	if( test_mode == NULL ){
		Ddim_Assertion(("Dd_WDOG_Get_Test_Mode: input param error. [test_mode] NULL\n"));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif
	// Get WDOGITCR.
	*test_mode	= IO_WDOG.WDOGITCR.bit.ITEN;
	
	return D_DDIM_OK;
}

/**
 * @brief	Get value of WDOGPERIPHID0~3 register.
 */
INT32 Dd_WDOG_Get_Peri_Identification(T_DD_WDOG_PERI_IDENTIFICATION* identification)
{
#ifdef CO_PARAM_CHECK
	if( identification == NULL ){
		Ddim_Assertion(("Dd_WDOG_Get_Peri_Identification: input param error. [identification] NULL\n"));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif
	// Get WDOGPERIPHID0~3
	identification->part_number		= IO_WDOG.WDOGPERIPHID0.bit.PARTNUMBER0 | (IO_WDOG.WDOGPERIPHID1.bit.PARTNUMBER1 << 8);
	identification->designer		= IO_WDOG.WDOGPERIPHID1.bit.DESIGNER0 | (IO_WDOG.WDOGPERIPHID2.bit.DESIGNER1 << 4);
	identification->revision_number	= IO_WDOG.WDOGPERIPHID2.bit.REVISION;
	identification->configuration	= IO_WDOG.WDOGPERIPHID3.bit.CONFIGURATION;
	
	return D_DDIM_OK;
}

/**
 * @brief	Get value of WDOGPCELLID0~3 register.
 */
INT32 Dd_WDOG_Get_PCell_Identification(ULONG* pcell)
{
#ifdef CO_PARAM_CHECK
	if( pcell == NULL ) {
		Ddim_Assertion(("Dd_WDOG_Get_PCell_Identification: input param error. [pcell] NULL\n"));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif
	// Get WDOGPCELLID0~3
	*pcell	= IO_WDOG.WDOGPCELLID[0].bit.WDOGPCELLID | (IO_WDOG.WDOGPCELLID[1].bit.WDOGPCELLID << 8) |
				(IO_WDOG.WDOGPCELLID[2].bit.WDOGPCELLID << 16) | (IO_WDOG.WDOGPCELLID[3].bit.WDOGPCELLID << 24);

	return D_DDIM_OK;
}

/**
 * @brief	Set value of WDOGLOAD register.
 */
INT32 Dd_WDOG_Set_Load(ULONG wdog_load)
{
#ifdef CO_PARAM_CHECK
	if( wdog_load == 0 ){
		Ddim_Assertion(("Dd_WDOG_Set_Load: input param error. wdog_load = 0\n"));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif
	IO_WDOG.WDOGLOAD = wdog_load;					// Set value of WDOGLOAD register
	
	return D_DDIM_OK;
}

/**
 * @brief	Set value of WDOGCONTROL register.
 */
INT32 Dd_WDOG_Set_Control(T_DD_WDOG_CTRL* wdog_ctrl)
{
#ifdef CO_PARAM_CHECK
	if( wdog_ctrl == NULL ){
		Ddim_Assertion(("Dd_WDOG_Set_Control: input param error. [*wdog_ctrl] NULL\n"));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
	if((wdog_ctrl->reset_enable != D_DD_WDOG_ENABLE_ON) && (wdog_ctrl->reset_enable != D_DD_WDOG_ENABLE_OFF)){
		Ddim_Assertion(("Dd_WDOG_Set_Control: input param error. reset_enable=%d\n", wdog_ctrl->reset_enable));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
	if((wdog_ctrl->int_enable != D_DD_WDOG_ENABLE_ON) && (wdog_ctrl->int_enable != D_DD_WDOG_ENABLE_OFF)){
		Ddim_Assertion(("Dd_WDOG_Set_Control: input param error. int_enable=%d\n", wdog_ctrl->int_enable));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif
	// Set WdogControl
	if( wdog_ctrl->reset_enable == D_DD_WDOG_ENABLE_ON ) {
		IO_WDOG.WDOGCONTROL.bit.RESEN	= D_DD_WDOG_ENABLE_ON;
		IO_WDOG.WDOGCONTROL.bit.INTEN	= 1;
	}
	if( wdog_ctrl->int_enable == D_DD_WDOG_ENABLE_ON ) {
		IO_WDOG.WDOGCONTROL.bit.INTEN	= D_DD_WDOG_ENABLE_ON;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief	Clear Interrupt flag (Set WDOGINTCLR register).
 */
INT32 Dd_WDOG_Clear_Interrupt(VOID)
{
	IO_WDOG.WDOGINTCLR = 1;			// Set value of WdogIntClr register
	
	return D_DDIM_OK;
}

/**
 * @brief	Set value of WDOGLOCK register.
 */
INT32 Dd_WDOG_Set_Lock(UCHAR wdog_lock)
{
#ifdef CO_PARAM_CHECK
	if((wdog_lock != D_DD_WDOG_ENABLE_ON) && (wdog_lock != D_DD_WDOG_ENABLE_OFF)){
		Ddim_Assertion(("Dd_WDOG_Set_Lock: input param error. wdog_lock=%d\n", wdog_lock));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif
	if( wdog_lock == D_DD_WDOG_ENABLE_ON ){
		// Unlock(write access to all other registers is enabled).
		IO_WDOG.WDOGLOCK.word	= D_DD_WDOG_REGISTER_ACCESS_ENABLE;
	}
	else{
		// Lock(write access to all other registers is disable).
		IO_WDOG.WDOGLOCK.word	= 0;
	}
	return D_DDIM_OK;
}

/**
 * @brief	Set value of WDOGITCR/WDOGITOP register.
 */
INT32 Dd_WDOG_Set_Test_Mode(UCHAR test_mode, UCHAR test_res, UCHAR test_int)
{
#ifdef CO_PARAM_CHECK
	if((test_mode != D_DD_WDOG_ENABLE_ON) && (test_mode != D_DD_WDOG_ENABLE_OFF)){
		Ddim_Assertion(("Dd_WDOG_Set_Test_Mode: input param error. test_mode=%d\n", test_mode));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
	if((test_res != D_DD_WDOG_ENABLE_ON) && (test_res != D_DD_WDOG_ENABLE_OFF)){
		Ddim_Assertion(("Dd_WDOG_Set_Test_Mode: input param error. test_res=%d\n", test_res));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
	if((test_int != D_DD_WDOG_ENABLE_ON) && (test_int != D_DD_WDOG_ENABLE_OFF)){
		Ddim_Assertion(("Dd_WDOG_Set_Test_Mode: input param error. test_int=%d\n", test_int));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif
	// Set WDOGITCR.
	IO_WDOG.WDOGITCR.bit.ITEN		= test_mode;
	// Set WDOGITOP.
	IO_WDOG.WDOGITOP.bit.WDOGRES	= test_res;
	IO_WDOG.WDOGITOP.bit.WDOGINT	= test_int;
	
	return D_DDIM_OK;
}

/**
 * @brief	It is Interrupt Handler of Watchdog.<br>
 *			The CallBack function is called.
*/
VOID Dd_WDOG_Int_Handler(VOID)
{
	// Soft Counter check
	if( gDD_Wdog_Soft_Counter > 1 ){
		gDD_Wdog_Soft_Counter--;
		
		// Reload
		Dd_WDOG_Set_Load(D_DD_WDOG_COUNTER_MAX_VALUE);
		// Clear Interrupt
		Dd_WDOG_Clear_Interrupt();
		Dd_ARM_Dsb_Pou();
	}
	else if( gDD_Wdog_Soft_Counter == 1 ){
		gDD_Wdog_Soft_Counter--;
		
		// Reload
		Dd_WDOG_Set_Load(gDD_Wdog_Remain_Number);
		// Clear Interrupt
		Dd_WDOG_Clear_Interrupt();
		Dd_ARM_Dsb_Pou();
	}
	else{
		if( gDD_Wdog_Callback != NULL ){
			// Callback.
			(*gDD_Wdog_Callback)();
		}
		
		// Clear Interrupt
		Dd_WDOG_Clear_Interrupt();
		Dd_ARM_Dsb_Pou();
	}
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/**
 * @brief  To input the value at the passed time to WdogLoad, it converts it into the number of clocks. 
 */
INT32 Dd_WDOG_Calculate(ULONG wdog_wdmode, ULONG msec, ULONG* convert_counter)
{
	ULLONG peri_clk;
	ULLONG msec_count;
	
#ifdef CO_PARAM_CHECK
	if( wdog_wdmode > 1 ){
		Ddim_Assertion(("Dd_WDOG_Calculate: input param error. wdog_wdmode > 1\n"));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
	if( msec == 0 ){
		Ddim_Assertion(("Dd_WDOG_Calculate: input param error. [msec] = 0\n"));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
	if( convert_counter == NULL ){
		Ddim_Assertion(("Dd_WDOG_Calculate: input param error. [convert_counter] = NULL\n"));
		return D_DD_WDOG_INPUT_PARAM_ERR;
	}
#endif
	peri_clk = Dd_Top_Get_PCLK();														// Call ChipTop API
	msec_count = peri_clk * msec / 1000;												// Convert to counter value
	if( wdog_wdmode == 1 ){
		// Watchdog mode
		if( msec_count > D_DD_WDOG_COUNTER_MAX_VALUE ){
			return D_DD_WDOG_OVERFLOW;
		}
	}
	
	gDD_Wdog_Soft_Counter = msec_count / ((ULLONG)D_DD_WDOG_COUNTER_MAX_VALUE + 1);		// Calculate to input value divided by the maximum value of 32bit
	gDD_Wdog_Remain_Number = msec_count & D_DD_WDOG_COUNTER_MAX_VALUE;					// Calculate to the remainder into which the input value is divided by the maximum value of 32bit is acquired
	if( gDD_Wdog_Soft_Counter > 0 ){
		msec_count = D_DD_WDOG_COUNTER_MAX_VALUE;										// Max value of watchdog counter
	}
	
	*convert_counter = msec_count;
	
	return D_DDIM_OK;
}

/**
 * @brief  The passed time to WdogLoad to set the watchdog timer, msec converts into the number of clocks. 
 */
INT32 Dd_WDOG_Set_Timer(ULONG wdog_wdmode, ULONG msec)
{
	ULONG convert_counter;
	INT32 ercd;
	
	ercd = Dd_WDOG_Calculate(wdog_wdmode, msec, &convert_counter);
	if( ercd != D_DDIM_OK ){
		return ercd;
	}
	
	Dd_WDOG_Set_Load(convert_counter);					// Set value of WdogLoad register
	
	return D_DDIM_OK;
}
#endif	// CO_DDIM_UTILITY_USE
