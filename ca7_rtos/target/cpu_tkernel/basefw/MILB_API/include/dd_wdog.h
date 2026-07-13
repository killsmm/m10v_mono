/**
 * @file		dd_wdog.h
 * @brief		Watchdog Timer driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_wdog
@{
	- @ref Watchdog_basic_sequence
	- @ref Wdog_Sample_Basic
*/

#ifndef _DD_WDOG_H_
#define _DD_WDOG_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Return value of watchdog timer processing result */
#define D_DD_WDOG_INPUT_PARAM_ERR	(D_DD_WDOG | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error */
#define D_DD_WDOG_SEM_TIMEOUT		(D_DD_WDOG | D_DDIM_SEM_TIMEOUT)		/**< Semaphore acquisition Time Out */
#define D_DD_WDOG_SEM_NG			(D_DD_WDOG | D_DDIM_SEM_NG)				/**< Semaphore acquisition NG */
#define D_DD_WDOG_OVERFLOW			(D_DD_WDOG | D_DDIM_OVERFLOW_ERR)		/**< Load value is overflow */

// General-purpose definition of register value (Effectiveness selection)
#define D_DD_WDOG_ENABLE_OFF						(0)		/**< OFF	*/
#define D_DD_WDOG_ENABLE_ON							(1)		/**< ON		*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
/** Control value */
typedef struct {
	ULONG 		wdog_load;			/**< WDOGLOAD register.<br>
										 Value range : 0x00000000~0xFFFFFFFF						*/
	UCHAR 		reset_enable;		/**< WDOGCONTROL.RESEN : Enable Watchdog reset output.<br>
										 <ul><li>@ref D_DD_WDOG_ENABLE_OFF
											 <li>@ref D_DD_WDOG_ENABLE_ON</ul>						*/
	UCHAR 		int_enable;			/**< WDOGCONTROL.INTEN : Enable the interrupt event.<br>
										 <ul><li>@ref D_DD_WDOG_ENABLE_OFF
											 <li>@ref D_DD_WDOG_ENABLE_ON</ul>						*/
	VP_CALLBACK pCallback;			/**< Callback function called when interrupt is generated		*/
}T_DD_WDOG_CTRL;

/** Peripheral Identification */
typedef struct {
	USHORT		part_number;		/**< This identifies the peripheral. Value:0x805					*/
	UCHAR		designer;			/**< Designer. Value:0x41 (ARM Limited is 0x41, ASCII A.)			*/
	UCHAR		revision_number;	/**< Revision number of the peripheral. Value:0~15 (starts from 0.)	*/
	UCHAR		configuration;		/**< This is the configuration option of the peripheral. Value:0	*/
}T_DD_WDOG_PERI_IDENTIFICATION;

/*----------------------------------------------------------------------*/
/* Grobal Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

//---------------------------- driver section ----------------------------
/**
 * Initialize Watchdog Register.<br>
 * Default status : WDOGCONTROL = 0, WDOGLOAD = 0xFFFFFFFF.<br>
 */
extern 	VOID	Dd_WDOG_Init(VOID);

/**
 * The Watchdog fuction is exclusively controlled.<br>
 * If Watchdog is unused, exclusive control OK is sent back as (Lock) while using it.<br>
 * Exclusive control NG is sent back when using it (Lock inside of present) or Input Parameter Error.
 * @param[in]	tmout					: Semaphore Timeout Value.<br>
 * 										  <ul><li>Positive Value
 * 											  <li>@ref D_DDIM_USER_SEM_WAIT_POL
 * 											  <li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_SEM_NG			: Lock Error (System has already been used.)
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 * @retval	D_DD_WDOG_SEM_TIMEOUT		: Semaphore Timeout Error
 * @remarks	This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
			This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
 */
extern 	INT32	Dd_WDOG_Open(INT32 tmout);

/**
 * Setting of common control variable for Watchdog Counter.
 * @param[in]	wdog_ctrl				: Watchdog Control value. See @ref T_DD_WDOG_CTRL
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 */
extern	INT32	Dd_WDOG_Ctrl(T_DD_WDOG_CTRL* wdog_ctrl);

/**
 * Reload Watchdog Counter.
 */
extern	VOID	Dd_WDOG_Reload(VOID);

/**
 * The exclusive control of Watchdog is released.
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_SEM_NG			: Semaphore acquisition NG
 * @remarks	This API uses DDIM_User_Sig_Sem().
 */
extern	INT32	Dd_WDOG_Close(VOID);

/**
 * Get value of WDOGLOAD register.
 * @param[out]	wdog_load				: Watchdog Load
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 */
extern	INT32	Dd_WDOG_Get_Load(ULONG* wdog_load);

/**
 * Get value of WDOGVALUE register.
 * @param[out]	wdog_counter			: Watchdog Counter
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 */
extern	INT32	Dd_WDOG_Get_Counter(ULONG* wdog_counter);

/**
 * Get value of WDOGCONTROL register.
 * @param[out]	wdog_ctrl				: Watchdog Control value. See @ref T_DD_WDOG_CTRL
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 */
extern	INT32	Dd_WDOG_Get_Control(T_DD_WDOG_CTRL* wdog_ctrl);

/**
 * Get value of WDOGRIS/WDOGMIS register.
 * @param[out]	raw_wdog_int			: Raw Watchdog Interrupt
 * @param[out]	mask_wdog_int			: Watchdog Interrupt
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 */
extern	INT32	Dd_WDOG_Get_Status(UCHAR* raw_wdog_int, UCHAR* mask_wdog_int);

/**
 * @brief	Get value of WDOGLOCK register.
 * @param[out]	wdog_lock				: Lock status<br>
 * 										  <ul><li>D_DD_WDOG_ENABLE_OFF : Unlock
 * 											  <li>D_DD_WDOG_ENABLE_ON : Lock</ul>
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 */
extern	INT32	Dd_WDOG_Get_Lock(UCHAR* wdog_lock);

/**
 * Get value of WDOGITCR register.
 * @param[out]	test_mode				: Integration Test Mode Enable
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 */
extern	INT32	Dd_WDOG_Get_Test_Mode(UCHAR* test_mode);

/**
 * Get value of WDOGPERIPHID0~3 register.
 * @param[out]	identification			: Peripheral Identification. See @ref T_DD_WDOG_PERI_IDENTIFICATION
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 */
extern	INT32	Dd_WDOG_Get_Peri_Identification(T_DD_WDOG_PERI_IDENTIFICATION* identification);

/**
 * Get value of WDOGPCELLID0~3 register.
 * @param[out]	pcell					: PrimeCell Identification. Value:0xB105F00D
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 */
extern	INT32	Dd_WDOG_Get_PCell_Identification(ULONG* pcell);

/**
 * Set value of WDOGLOAD register.
 * @param[in]	wdog_load				: Watchdog Load. Value range:0x00000000~0xFFFFFFFF
 * @retval	D_DDIM_OK					: Normal end
 */
extern	INT32	Dd_WDOG_Set_Load(ULONG wdog_load);

/**
 * Set value of WDOGCONTROL register.
 * @param[in]	wdog_ctrl				: Watchdog Control value. See @ref T_DD_WDOG_CTRL
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 */
extern	INT32	Dd_WDOG_Set_Control(T_DD_WDOG_CTRL* wdog_ctrl);

/**
 * Clear Interrupt flag (Set WDOGINTCLR register).
 * @return	D_DDIM_OK					: Normal end
 */
extern	INT32	Dd_WDOG_Clear_Interrupt(VOID);

/**
 * Set value of WDOGLOCK register. 
 * @param[in]	wdog_lock				: Watchdog Lock setting<br>
 * 										  <ul><li>D_DD_WDOG_ENABLE_OFF : Unlock
 * 											  <li>D_DD_WDOG_ENABLE_ON : Lock</ul>
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 */
extern	INT32	Dd_WDOG_Set_Lock(UCHAR wdog_lock);

/**
 * Set value of WDOGITCR/WDOGITOP register.
 * @param[in]	test_mode				: Integration Test Mode Enable<br>
 * 										  <ul><li>D_DD_WDOG_ENABLE_OFF
 * 											  <li>D_DD_WDOG_ENABLE_ON</ul>
 * @param[in]	test_res				: Integration Test WDOGINT value<br>
 * 										  <ul><li>D_DD_WDOG_ENABLE_OFF
 * 											  <li>D_DD_WDOG_ENABLE_ON</ul>
 * @param[in]	test_int				: Integration Test WDOGRES value<br>
 * 										  <ul><li>D_DD_WDOG_ENABLE_OFF
 * 											  <li>D_DD_WDOG_ENABLE_ON</ul>
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 */
extern	INT32	Dd_WDOG_Set_Test_Mode(UCHAR test_mode, UCHAR test_res, UCHAR test_int);

/**
 * It is Interrupt Handler of Watchdog.<br>
 * The CallBack function is called.
 */
extern	VOID	Dd_WDOG_Int_Handler(VOID);

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/** @name Utility Functions
@{*/
/**
 * To input the value at the passed time to WDOG_LOAD, it converts it into the number of clocks.
 * @param[in]		wdog_wdmode			: Watchdog Mode
 * 										  <ul><li>0:INTEN(Interrupt event)
 * 											  <li>1:RESEN(Watchdog reset)</ul>
 * @param[in]		msec				: Designated time(milli seconds)
 * @param[out]		convert_counter		: Value of converted msec
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 * @retval	D_DD_WDOG_OVERFLOW			: Load value is overflow
 * @remarks
 * In the RESEN mode, If PCLK is 50[MHz], range is 1 to 85899[msec].<br>
 * In the INTEN mode, If msec exceeded the maximum value of WDOGLOAD(4294967295=0xFFFFFFFF) as a result of calculation, Dd_WDOG_Calculate uses soft counter.<br>
 * The soft counter enables the count that exceeds the maximum value.<br>
 * The counter maintains the value in which msec is divided by maximum value of WDOGLOAD(4294967295).<br>
 * If soft counter maintains the value when timer is expired, WDOGLOAD is reloaded by 0xFFFFFFFF and soft counter is decrement.<br>
 * If soft counter is zero and timer is expired, WDOGLOAD is reloaded by remained number for msec divided by 4294967295.
 */
extern INT32 Dd_WDOG_Calculate(ULONG wdog_wdmode, ULONG msec, ULONG* convert_counter);

/**
 * The passed time to WDOG_LOAD to set the Watchdog timer, msec converts into the number of clocks. 
 * @param[in]		wdog_wdmode			: Watchdog Mode
 * 										  <ul><li>0:INTEN(Interrupt event)
 * 											  <li>1:RESEN(Watchdog reset)</ul>
 * @param[in]		msec				: Designated time(milli seconds)
 * @retval	D_DDIM_OK					: Normal end
 * @retval	D_DD_WDOG_INPUT_PARAM_ERR	: Input Parameter Error
 * @retval	D_DD_WDOG_OVERFLOW			: Load value is overflow
 * @remarks
 * In the RESEN mode, If PCLK is 50[MHz], range is 1 to 85899[msec].<br>
 * In the INTEN mode, If msec exceeded the maximum value of WDOGLOAD(4294967295=0xFFFFFFFF) as a result of calculation, Dd_WDOG_Calculate uses soft counter.<br>
 * The soft counter enables the count that exceeds the maximum value.<br>
 * The counter maintains the value in which msec is divided by maximum value of WDOGLOAD(4294967295).<br>
 * If soft counter maintains the value when timer is expired, WDOGLOAD is reloaded by 0xFFFFFFFF and soft counter is decrement.<br>
 * If soft counter is zero and timer is expired, WDOGLOAD is reloaded by remained number for msec divided by 4294967295.
 */
extern	INT32 Dd_WDOG_Set_Timer(ULONG wdog_wdmode, ULONG msec);
/*@}*/
#endif

#ifdef __cplusplus
}
#endif

#endif	// _DD_WDOG_H_

/*@}*/
/** @weakgroup dd_wdog
@{
	@section Watchdog_basic_sequence	Watchdog Basic sequence
	@image html dd_watchdog_basic_sequence.png
	<br>
	@section Wdog_Sample_Basic			Sample of Watchdog(Basic sequence)
	The example of executing watchdog counter start to end is as follows.
	@code
	INT32	ret;								// Return value
	T_DD_WDOG_CTRL	wdog_ctrl;
	
	wdog_ctrl.wdog_load = 1000000000;					// 1000000000[count] = 10[s]
	wdog_ctrl.reset_enable = D_DD_WDOG_ENABLE_OFF;		// Reset disable
	wdog_ctrl.int_enable = D_DD_WDOG_ENABLE_ON;			// Interrupt enable
	wdog_ctrl.pCallback = Sample_Callback0;				// User's Callback function
	
	Dd_WDOG_Init();
	
	ret = Dd_WDOG_Open(D_DDIM_USER_SEM_WAIT_POL);
	if (ret != 0) {
		// ERROR
		return ;
	}
	
	ret = Dd_WDOG_Ctrl(&wdog_ctrl);
	if (ret != 0) {
		// ERROR
		return ;
	}
	
	// Please wait for more than 33 second code here. The callback is called.
	
	// Close
	ret = Dd_WDOG_Close();
	if (ret != 0) {
		// ERROR
	}
	@endcode
@}*/
