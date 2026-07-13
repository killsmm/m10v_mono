/**
 * @file		dd_udc.h
 * @brief		UpDown Counter driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_udc
@{

	- @ref UDC_start_up_down_mode
	- @ref UDC_start_phase_lag_mode_2
	- @ref UDC_start_phase_lag_mode_4
	- @ref UDC_start_timer_mode
	- @ref UDC_start_timer_utility
	- @ref dd_udc_sample_section1
	- @ref dd_udc_sample_section2
	- @ref dd_udc_sample_section3
	- @ref dd_udc_sample_section4
	- @ref dd_udc_sample_section5

	<hr>
	@section UDC_start_up_down_mode	Sequence of UDC up/down count mode
	@image html dd_udc_up_down_mode.png

	@section UDC_start_phase_lag_mode_2	Sequence of UDC phase lag count mode (2 multiply)
	@image html dd_udc_phase_lag2_mod.png

	@section UDC_start_phase_lag_mode_4	Sequence of UDC phase lag count mode (4 multiply)
	@image html dd_udc_phase_lag4_mod.png

	@section UDC_start_timer_mode	Sequence of UDC timer mode
	@image html dd_udc_timer_mode.png

	@section UDC_start_timer_utility	Sequence of timer utility
	@image html dd_udc_timer_utility.png
*/

#ifndef _DD_UDC_H_
#define _DD_UDC_H_

#include "driver_common.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
//---------------------- driver  section ------------------------
// Return value of UDC timer processing result
#define	D_DD_UDC_INPUT_PARAM_ERROR	(D_DD_UDC | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error	*/
#define	D_DD_UDC_EXT_LOCK_NG		(D_DD_UDC | D_DDIM_EXT_LOCK_NG)			/**< Exclusive control NG	*/
#define D_DD_UDC_ERROR_BUSY			(D_DD_UDC | D_DDIM_MACRO_BUSY_NG)		/**< Macro busy				*/

#define D_DD_UDC_SEM_TIMEOUT		(D_DD_UDC | D_DDIM_SEM_TIMEOUT)			/**< Semaphore acquisition Time Out */
#define D_DD_UDC_SEM_NG				(D_DD_UDC | D_DDIM_SEM_NG)				/**< Semaphore acquisition NG */

// UDC Handler result value
#define D_DD_UDC_CMPF_OK			(D_DD_UDC | 0x00000101)	/**< Compare OK 					*/
#define D_DD_UDC_OVER_FLOW			(D_DD_UDC | 0x00000102)	/**< Overflow generation 			*/
#define D_DD_UDC_UNDER_FLOW			(D_DD_UDC | 0x00000103)	/**< Underflow generation 			*/
#define D_DD_UDC_COUNT_DIRECTION	(D_DD_UDC | 0x00000104)	/**< Count direction change			*/
#define	D_DD_UDC_UNDETECTION		(D_DD_UDC | 0x00000105)	/**< Flag undetection				*/

// CCRH/L: Count mode
#define D_DD_UDC_CMS_TIMER			(0x00)	/**< Count mode: Timer mode							*/
#define D_DD_UDC_CMS_UP_DOWN		(0x01)	/**< Count mode: Up/Down count mode					*/
#define D_DD_UDC_CMS_PHASE_LAG_2	(0x02)	/**< Count mode: Phase lag count mode (2 multiply)	*/
#define D_DD_UDC_CMS_PHASE_LAG_4	(0x03)	/**< Count mode: Phase lag count mode (4 multiply)	*/

// CCRH/L: Compare clear enable
#define	D_DD_UDC_UCRE_OFF			(0x00)	/**< Compare clear enable: disable					*/
#define	D_DD_UDC_UCRE_ON			(0x01)	/**< Compare clear enable: enable					*/

// CCRH/L: Reload enable
#define	D_DD_UDC_RLDE_OFF			(0x00)	/**< Reload enable: disable							*/
#define	D_DD_UDC_RLDE_ON			(0x01)	/**< Reload enable: enable							*/

// CCRH/L: Function of terminal ZIN
#define	D_DD_UDC_CGSC_CLEAR			(0x00)	/**< Function of terminal ZIN: Counter clear		*/
#define	D_DD_UDC_CGSC_GATE			(0x01)	/**< Function of terminal ZIN: Gate					*/

// CCRH/L: Detect edge of terminal ZIN
#define	D_DD_UDC_CGE_NOT			(0x00)	/**< Detect edge of terminal ZIN: Detect disable	*/
#define	D_DD_UDC_CGE_FALL_L			(0x01)	/**< Detect edge of terminal ZIN: Fall edge / L		*/
#define	D_DD_UDC_CGE_RISE_H			(0x02)	/**< Detect edge of terminal ZIN: Rise edge / H		*/

// CCRH/L: Prescaler select
#define D_DD_UDC_CLKS_2_CLOCK		(0x00)	/**< Prescaler select: 2 clock	*/
#define D_DD_UDC_CLKS_8_CLOCK		(0x01)	/**< Prescaler select: 8 clock	*/

// CCRH/L: Count clock edge select
#define	D_DD_UDC_CES_NOT			(0x00)	/**< Count clock edge select: Detect disable		*/
#define	D_DD_UDC_CES_RISE			(0x01)	/**< Count clock edge select: Rise edge				*/
#define	D_DD_UDC_CES_FALL			(0x02)	/**< Count clock edge select: Fall edge				*/
#define	D_DD_UDC_CES_BOTH			(0x03)	/**< Count clock edge select: Both edge				*/

// CSR: Up/Down flag
#define D_DD_UDC_UDF_NON			(0x00)	/**< Up/Down flag: No input							*/
#define D_DD_UDC_UDF_DOWN			(0x01)	/**< Up/Down flag: Down count						*/
#define D_DD_UDC_UDF_UP				(0x02)	/**< Up/Down flag: Up count							*/
#define D_DD_UDC_UDF_BOTH			(0x03)	/**< Up/Down flag: Up and down count				*/

// CSR: Compare interrupt enable
#define	D_DD_UDC_CITE_OFF			(0x00)	/**< Compare interrupt enable: disable				*/
#define	D_DD_UDC_CITE_ON			(0x01)	/**< Compare interrupt enable: enable				*/

// CSR: Under/Over-flow interrupt enable
#define	D_DD_UDC_UDIE_OFF			(0x00)	/**< Under/Over-flow interrupt enable: disable		*/
#define	D_DD_UDC_UDIE_ON			(0x01)	/**< Under/Over-flow interrupt enable: enable		*/

// CCRH/L: Count direction interrupt enable
#define	D_DD_UDC_CFIE_OFF			(0x00)	/**< Count direction interrupt enable: disable		*/
#define	D_DD_UDC_CFIE_ON			(0x01)	/**< Count direction interrupt enable: enable		*/

// Channel number
#define D_DD_UDC_CH0				(0x00)	/**< UDC ch0	*/
#define D_DD_UDC_CH1				(0x01)	/**< UDC ch1	*/
#define D_DD_UDC_CH2				(0x02)	/**< UDC ch2	*/
#define D_DD_UDC_CH3				(0x03)	/**< UDC ch3	*/

#define D_DD_UDC_CH4				(0x04)	/**< UDC ch4	*/
#define D_DD_UDC_CH5				(0x05)	/**< UDC ch5	*/

typedef VOID (*DD_UDC_CALLBACK)(UCHAR ch, INT32 factor);	/**< Type is defined to Callback function pointer.<br>
																 The meaning of the argument is as follows.<br>
																 - UCHAR ch    : Channel number.<br>
																	<ul><li>@ref D_DD_UDC_CH0
																	    <li>@ref D_DD_UDC_CH1
																	    <li>@ref D_DD_UDC_CH2
																	    <li>@ref D_DD_UDC_CH3
																		<li>@ref D_DD_UDC_CH4
																		<li>@ref D_DD_UDC_CH5</ul>
																 - UCHAR factor: Factor of callback.
																	<ul><li>@ref D_DD_UDC_CMPF_OK
																	    <li>@ref D_DD_UDC_OVER_FLOW
																	    <li>@ref D_DD_UDC_UNDER_FLOW
																	    <li>@ref D_DD_UDC_COUNT_DIRECTION</ul> */

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------
/** @name Utility Typedefs
@{*/

typedef VOID (*DD_UDC_TIMER_CALLBACK)( UCHAR ch );			/**< Type is defined to Callback function pointer
																 for timer utility function.<br>The meaning of 
																 the argument is as follows.<br>
																 - UCHAR ch    : Channel number.<br>
																	<ul><li>@ref D_DD_UDC_CH0
																	    <li>@ref D_DD_UDC_CH1
																	    <li>@ref D_DD_UDC_CH2
																	    <li>@ref D_DD_UDC_CH3
																		<li>@ref D_DD_UDC_CH4
																		<li>@ref D_DD_UDC_CH5</ul> */

//---------------------- colabo  section -------------------------
/*@}*/
#endif

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special
//---------------------- driver  section -------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------
//---------------------- colabo  section -------------------------
#endif

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
//---------------------- driver  section -------------------------
/** Control info */
typedef struct {
	USHORT		udcr;			/**< UDCR: Up/Down counter value.(0x0000 to 0xFFFF)		*/
	USHORT		rcr;			/**< RCR : Reload/Compare value.(0x0000 to 0xFFFF)		*/

	UCHAR		count_mode;		/**< CCR.CMS: Count mode.<br>
									 <ul><li>@ref D_DD_UDC_CMS_TIMER 
										 <li>@ref D_DD_UDC_CMS_UP_DOWN
										 <li>@ref D_DD_UDC_CMS_PHASE_LAG_2
										 <li>@ref D_DD_UDC_CMS_PHASE_LAG_4</ul>			*/
	UCHAR		comp_clear;		/**< CCR.UCRE: Compare clear enable.<br>
									 <ul><li>@ref D_DD_UDC_UCRE_OFF 
										 <li>@ref D_DD_UDC_UCRE_ON</ul>					*/
	UCHAR		reload;			/**< CCR.RLDE: Reload enable.<br>
									 <ul><li>@ref D_DD_UDC_RLDE_OFF 
										 <li>@ref D_DD_UDC_RLDE_ON</ul>					*/

	UCHAR		zin_mode;		/**< CCR.CGSC: Function of terminal ZIN.<br>
									 <ul><li>@ref D_DD_UDC_CGSC_CLEAR 
										 <li>@ref D_DD_UDC_CGSC_GATE</ul>				*/
	UCHAR		zin_edge;		/**< CCR.CGE : Detect edge of terminal ZIN.<br>
									 <ul><li>@ref D_DD_UDC_CGE_NOT
										 <li>@ref D_DD_UDC_CGE_FALL_L
										 <li>@ref D_DD_UDC_CGE_RISE_H</ul>				*/

	UCHAR		cmp_int;		/**< CSR.CITE: Compare interrupt enable.<br>
									 <ul><li>@ref D_DD_UDC_CITE_OFF 
										 <li>@ref D_DD_UDC_CITE_ON</ul>					*/
	UCHAR		under_over;		/**< CSR.UDIE: Under/Over-flow interrupt enable.<br>
									 <ul><li>@ref D_DD_UDC_UDIE_OFF 
										 <li>@ref D_DD_UDC_UDIE_ON</ul>					*/
	UCHAR		cnt_dir_int;	/**< CCR.CFIE: Count direction interrupt enable.<br>
									 <ul><li>@ref D_DD_UDC_CFIE_OFF 
										 <li>@ref D_DD_UDC_CFIE_ON</ul>
									 After system reset, the state of count direction 
									 is down count.<br>
									 Therefore, the count direction interruption is 
									 generated in the up count after system reset.<br>
									 Please confirm the state of count direction by 
									 the @ref Dd_UDC_Get_Up_Down_Flg function when 
									 you use channel since the second times.<br>		*/

	DD_UDC_CALLBACK	pCallBack;	/**< callback function pointer							*/
} T_DD_UDC_CTRL_CMN;

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------
//---------------------- colabo  section -------------------------
#endif

/*---------------------------------------------------------------*/
/* Global Data											  		 */
/*---------------------------------------------------------------*/
// Nothing Special
//---------------------- driver  section -------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------
//---------------------- colabo  section -------------------------
#endif

/*----------------------------------------------------------------*/
/* Macro				 										  */
/*----------------------------------------------------------------*/
// Nothing Special
//---------------------- driver  section -------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------
//---------------------- colabo  section -------------------------
#endif

/*----------------------------------------------------------------*/
/* Function  													  */
/*----------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

//---------------------- driver  section -------------------------------
/**
The UDC control register (all channels) is initialized.
*/
extern	VOID	Dd_UDC_Init(VOID);

/**
The UDC channel is exclusively controlled.<br>
If UDC is unused, exclusive control OK is sent back as (Lock) while using it. <br>
Exclusive control NG is sent back when using it (Lock inside of present).
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@param [in]	tmout	semaphore timeout value(milli seconds)/D_DDIM_USER_SEM_WAIT_POL(polling semaphore).<br>
@retval D_DDIM_OK						OK
@retval D_DD_UDC_EXT_LOCK_NG			Lock Error (System Using designated channel number)
@retval D_DD_UDC_INPUT_PARAM_ERROR		Input Parameter Error
@retval D_DD_UDC_SEM_TIMEOUT			Semaphore Timeout Error
@remarks This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
		 This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32	Dd_UDC_Open(UCHAR ch,INT32 tmout);

/**
The exclusive control of control Channel it is released.
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@retval D_DDIM_OK				OK
@retval D_DD_UDC_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_UDC_SEM_NG	Semaphore acquisition NG
@remarks Please note that stop & exclusive control is compulsorily released when UDC is starting.
@remarks This API uses DDIM_User_Sig_Sem().
*/
extern	INT32	Dd_UDC_Close(UCHAR ch);

/**
Set common setting to each mode of UDC.<br>
Set the callback function when interruption is generate to pCallBack.
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@param [in]	udc_crl	UDC Start table pointer. See @ref T_DD_UDC_CTRL_CMN
@retval D_DDIM_OK				OK
@retval D_DD_UDC_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_UDC_ERROR_BUSY		Macro Busy Error
@remarks	The mode and the function that can be used with each channel change by setting PERSEL2.UDCTRG register.<br>
			Please refer to the following tables for details.<br><br>
			<b>When PERSEL2.UDCTRG=0</b>
			<table><tr><th rowspan=3>Channel</th>
			           <th colspan=5>Count mode</th>
			           <th rowspan=3>ZIN function <br> (*1)</th></tr>
			       <tr><th rowspan=2>Timer</th>
			           <th colspan=2>Up/Down count</th>
			           <th rowspan=2>Phase lag count <br> (2 multiply)</th>
			           <th rowspan=2>Phase lag count <br> (4 multiply)</th></tr>
			       <tr><th>Up count <br> (AIN)</th>
			           <th>Down count <br> (BIN)</th></tr>
			       <tr align=center><td>0</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td></tr>
			       <tr align=center><td>1</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td></tr>
			       <tr align=center><td>2</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td></tr>
   			       <tr align=center><td>3</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td></tr>
			       <tr align=center><td>4</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td></tr>
			       <tr align=center><td>5</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td></tr></table>
			<br>
			<b>When PERSEL2.UDCTRG=1</b>
			<table><tr><th rowspan=3>Channel</th>
			           <th colspan=5>Count mode</th>
			           <th rowspan=3>ZIN function <br> (*1)</th></tr>
			       <tr><th rowspan=2>Timer</th>
			           <th colspan=2>Up/Down count</th>
			           <th rowspan=2>Phase lag count <br> (2 multiply)</th>
			           <th rowspan=2>Phase lag count <br> (4 multiply)</th></tr>
			       <tr><th>Up count <br> (AIN)</th>
			           <th>Down count <br> (BIN)</th></tr>
			       <tr align=center><td>0</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td></tr>
			       <tr align=center><td>1</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td></tr>
			       <tr align=center><td>2</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td> <td>x</td> <td>x</td></tr>
			       <tr align=center><td>3</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td> <td>x</td> <td>x</td></tr>
			       <tr align=center><td>4</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td> <td>x</td> <td>x</td></tr>
			       <tr align=center><td>5</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td> <td>x</td> <td>x</td></tr></table>
			<br>
			(*1) Please specify the following parameter when you do not use the ZIN function.<br>
			@code
			udc_crl.zin_mode = D_DD_UDC_CGSC_CLEAR;
			udc_crl.zin_edge = D_DD_UDC_CGE_NOT;
			@endcode
*/
extern INT32   Dd_UDC_Ctrl_Common(UCHAR ch, T_DD_UDC_CTRL_CMN const* const udc_crl);

/**
Get common setting to each mode of UDC.<br>
@param[in]	ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@param[out]	udc_crl	UDC Start table pointer. See @ref T_DD_UDC_CTRL_CMN
@retval D_DDIM_OK				OK
@retval D_DD_UDC_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32   Dd_UDC_Get_Ctrl_Common(UCHAR ch, T_DD_UDC_CTRL_CMN* const udc_crl);

/**
Set parameter only for the timer mode.
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@param [in]	prescaler	Built-in prescaler selection.(CCR.CLKS)<br>
						Please specify either the following.<br>
						- @ref D_DD_UDC_CLKS_2_CLOCK
						- @ref D_DD_UDC_CLKS_8_CLOCK
@retval D_DDIM_OK				OK
@retval D_DD_UDC_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32   Dd_UDC_Ctrl_Timer(UCHAR ch, UCHAR prescaler);

/**
Get parameter only for the timer mode.
@param[in]	ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@param[out]	prescaler	Built-in prescaler selection.(CCR.CLKS)<br>
						- @ref D_DD_UDC_CLKS_2_CLOCK
						- @ref D_DD_UDC_CLKS_8_CLOCK
@retval D_DDIM_OK				OK
@retval D_DD_UDC_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32   Dd_UDC_Get_Ctrl_Timer(UCHAR ch, UCHAR* const prescaler);

/**
Set parameter only for the up/down count mode.
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@param [in]	edge	Count clock edge.(CCR.CES)<br>
					Please specify either the following.<br>
					- @ref D_DD_UDC_CES_NOT
					- @ref D_DD_UDC_CES_RISE
					- @ref D_DD_UDC_CES_FALL
					- @ref D_DD_UDC_CES_BOTH
@retval D_DDIM_OK				OK
@retval D_DD_UDC_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32   Dd_UDC_Ctrl_Up_Down(UCHAR ch, UCHAR edge);

/**
Get parameter only for the up/down count mode.
@param[in]	ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@param[out]	edge	Count clock edge.(CCR.CES)<br>
					Please specify either the following.<br>
					- @ref D_DD_UDC_CES_NOT
					- @ref D_DD_UDC_CES_RISE
					- @ref D_DD_UDC_CES_FALL
					- @ref D_DD_UDC_CES_BOTH
@retval D_DDIM_OK				OK
@retval D_DD_UDC_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32   Dd_UDC_Get_Ctrl_Up_Down(UCHAR ch, UCHAR* const edge);

/**
The UDC of a control channel is started.<br>
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@retval D_DDIM_OK				OK
@retval D_DD_UDC_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_UDC_Start(UCHAR ch);

/**
The UDC of a control channel is stopped.<br>
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@retval D_DDIM_OK				OK
@retval D_DD_UDC_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_UDC_Stop(UCHAR ch);

/**
The interruption flag is cleared, and the CallBack function for UpDown Counter called.
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
*/
extern	VOID	Dd_UDC_Int_Handler(UCHAR ch);

/**
Set count mode.
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@param [in]	count_mode	Count mode.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CMS_TIMER 
				- @ref D_DD_UDC_CMS_UP_DOWN
				- @ref D_DD_UDC_CMS_PHASE_LAG_2
				- @ref D_DD_UDC_CMS_PHASE_LAG_4
@retval D_DDIM_OK				OK
@retval D_DD_UDC_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_UDC_Set_Count_Mode( UCHAR ch, UCHAR count_mode );

/**
Set the callback function when interruption is generate to pCallBack.
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@param [in]	pCallBack	Call Back function pointer
*/
extern	VOID	Dd_UDC_Set_CallBack( UCHAR ch, DD_UDC_CALLBACK pCallBack );

/**
Set reload / compare counter.
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@param [in] rcr		Set reload / compare counter
@retval D_DDIM_OK				OK
@retval D_DD_UDC_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_UDC_Set_RCR_Counter(UCHAR ch, USHORT rcr);

/**
The Up/Down counter value is returned.
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@retval UDCR counter value.<br>
*/
extern	USHORT	Dd_UDC_Get_UDCR_Counter(UCHAR ch);

/**
The input counter is set to UDCR register.
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@param [in] udcr	Set up/down counter value.
@retval D_DDIM_OK				OK
@retval D_DD_UDC_ERROR_BUSY		Macro Busy Error
@retval D_DD_UDC_INPUT_PARAM_ERROR	Input Parameter Error
@remarks	When the channel is working, it becomes an error.
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
extern	INT32	Dd_UDC_Set_UDCR_Counter(UCHAR ch, USHORT udcr);

/**
The RDCC bit of the CCR register of the specified channel is cleared.
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@retval D_DDIM_OK				OK
@retval D_DD_UDC_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_UDC_Force_Clear_UDCR(UCHAR ch);

/**
Get kind of the count immediately before. 
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@retval	D_DD_UDC_UDF_NON	no input
@retval	D_DD_UDC_UDF_DOWN	down count
@retval	D_DD_UDC_UDF_UP		up count
@retval	D_DD_UDC_UDF_BOTH	up and down count
*/
extern	UCHAR	Dd_UDC_Get_Up_Down_Flg(UCHAR ch);

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/** @name Utility Functions
@{*/

/**
Set parameter for timer utility function.
@param [in] ch	Channel number.<br>
				Please specify either the following.<br>
				- @ref D_DD_UDC_CH0
				- @ref D_DD_UDC_CH1
				- @ref D_DD_UDC_CH2
				- @ref D_DD_UDC_CH3
				- @ref D_DD_UDC_CH4
				- @ref D_DD_UDC_CH5
@param [in] usec	Setting time.(usec order, range of 0..4294967295)<br>
@param [in] timer_callback	Callback function for timer utility function.<br>
@retval D_DDIM_OK					OK
@retval D_DD_UDC_ERROR_BUSY			Macro Busy Error
@retval D_DD_UDC_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_UDC_Write_Timer_Mode( UCHAR ch, ULONG usec, DD_UDC_TIMER_CALLBACK timer_callback );
//---------------------- colabo  section -------------------------------

/*@}*/
/*@}*/

/**
@weakgroup dd_udc
@{
<hr>
Utility Functions <br>
	- @ref UDC_timer_utility
	- @ref dd_udc_sample_section5

	@section UDC_timer_utility	Sequence of UDC timer utility function
*/
/*@}*/

#endif

#endif

/**
@weakgroup dd_udc
@{
<hr>

@section dd_udc_sample_section1	Sample of Up/Down count mode.
	The precondition of the this sample is as follows.<br>
	<ul>
	  <li>Channel 1 is used.</li>
	  <li>Down count is done by the rising edge of BIN.</li>
	  <li>It processes by the first 20 counts, and it stops by the following 80 counts.</li>
	</ul>
    @code
	UCHAR gCounter = 0;

	VOID udc_sample_execute_down_count(VOID)
	{
		T_DD_UDC_CTRL_CMN udc_crl;
		UCHAR ch;
		INT32 ercd;

		ch       = D_DD_UDC_CH1;
		gCounter = 0;

		udc_crl.count_mode  = D_DD_UDC_CMS_UP_DOWN;
		udc_crl.udcr        = 20;
		udc_crl.rcr         = 80;
		udc_crl.comp_clear  = D_DD_UDC_UCRE_OFF;
		udc_crl.reload      = D_DD_UDC_RLDE_ON;
		udc_crl.zin_mode    = D_DD_UDC_CGSC_CLEAR;
		udc_crl.zin_edge    = D_DD_UDC_CGE_NOT;
		udc_crl.cmp_int     = D_DD_UDC_CITE_OFF;
		udc_crl.under_over  = D_DD_UDC_UDIE_ON;
		udc_crl.cnt_dir_int = D_DD_UDC_CFIE_OFF;
		udc_crl.pCallBack   = udc_sample_callback;

		// UDC open.
		ercd = Dd_UDC_Open(ch,D_DDIM_USER_SEM_WAIT_POL);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}

		// Set common execution parameter.
		ercd = Dd_UDC_Ctrl_Common(ch, &udc_crl);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}
		// Set execution parameter only for up/doun count mode.
		ercd = Dd_UDC_Ctrl_Up_Down(ch, D_DD_UDC_CES_RISE);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}

		// AIN pull-down enable (Because AIN doesn't use in down count)
		Dd_Top_Set_GPIO5_Control(PUDCR, 3, 0);
		Dd_Top_Set_GPIO5_Control(PUDER, 3, 1);

		// UDC start
		ercd = Dd_UDC_Start(ch);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}
	}

	VOID udc_sample_callback(UCHAR ch, INT32 factor)
	{
		INT32 ercd;

		if ((ch == D_DD_UDC_CH1) && (factor == D_DD_UDC_UNDER_FLOW)) {

			gCounter++;
			
			if (gCounter == 1) {
				// The processing of first 20 counts is executed here.
			}
			else if (gCounter >= 2) {
				// UDC stop.
				ercd = Dd_UDC_Stop(ch);
				if (result != D_DDIM_OK) {
					// Error processing
					return;
				}

				// AIN pull-down disable
				Dd_Top_Set_GPIO5_Control(PUDER, 3, 0);

				// UDC close.
				ercd = Dd_UDC_Close(ch);
				if (result != D_DDIM_OK) {
					// Error processing
					return;
				}

				// The processing of sencond 80 counts is executed here.
			}
		}
	}
	@endcode

@section dd_udc_sample_section2	Sample of phase lag count mode (2 multiply).
	The precondition of the this sample is as follows.<br>
	<ul>
	  <li>Channel 0 is used.</li>
	  <li>When the direction of the count changes, arbitrary processing is done.</li>
	  <li>It doesn't count between ZIN input level is high.</li>
	</ul>
    @code
	VOID udc_sample_execute_down_count(VOID)
	{
		T_DD_UDC_CTRL_CMN udc_crl;
		UCHAR ch;
		INT32 ercd;

		ch = D_DD_UDC_CH0;

		udc_crl.count_mode  = D_DD_UDC_CMS_PHASE_LAG_2;
		udc_crl.udcr        = 0;
		udc_crl.rcr         = 0;
		udc_crl.comp_clear  = D_DD_UDC_UCRE_OFF;
		udc_crl.reload      = D_DD_UDC_UCRE_OFF;
		udc_crl.zin_mode    = D_DD_UDC_CGSC_GATE;
		udc_crl.zin_edge    = D_DD_UDC_CGE_RISE_H;
		udc_crl.cmp_int     = D_DD_UDC_CITE_OFF;
		udc_crl.under_over  = D_DD_UDC_UCRE_OFF;
		udc_crl.cnt_dir_int = D_DD_UDC_CFIE_ON;
		udc_crl.pCallBack   = udc_sample_callback;

		// UDC open.
		ercd = Dd_UDC_Open(ch,D_DDIM_USER_SEM_WAIT_POL);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}

		// Set execution parameter.
		ercd = Dd_UDC_Ctrl_Common(ch, &udc_crl);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}

		// It changes to the setting for which ZIN control can be used by channel 0, 1. 
		Dd_Top_Set_PERSEL2_UDCTRG(1);

		// UDC start
		ercd = Dd_UDC_Start(ch);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}
	}

	VOID udc_sample_callback(UCHAR ch, INT32 factor)
	{
		if ((ch == D_DD_UDC_CH0) && (factor == D_DD_UDC_COUNT_DIRECTION)) {
			// The processing is executed here.
		}
	}
	@endcode

@section dd_udc_sample_section3	Sample of phase lag count mode (4 multiply).
	The precondition of the this sample is as follows.<br>
	<ul>
	  <li>Channel 2 is used.</li>
	  <li>When the direction of the count changes into the down count, arbitrary processing is done.</li>
	</ul>
    @code
	VOID udc_sample_execute_down_count(VOID)
	{
		T_DD_UDC_CTRL_CMN udc_crl;
		UCHAR ch;
		INT32 ercd;

		ch = D_DD_UDC_CH2;

		udc_crl.count_mode  = D_DD_UDC_CMS_PHASE_LAG_4;
		udc_crl.udcr        = 0;
		udc_crl.rcr         = 0;
		udc_crl.comp_clear  = D_DD_UDC_UCRE_OFF;
		udc_crl.reload      = D_DD_UDC_UCRE_OFF;
		udc_crl.zin_mode    = D_DD_UDC_CGSC_CLEAR;
		udc_crl.zin_edge    = D_DD_UDC_CGE_NOT;
		udc_crl.cmp_int     = D_DD_UDC_CITE_OFF;
		udc_crl.under_over  = D_DD_UDC_UCRE_OFF;
		udc_crl.cnt_dir_int = D_DD_UDC_CFIE_ON;
		udc_crl.pCallBack   = udc_sample_callback;

		// UDC open.
		ercd = Dd_UDC_Open(ch,D_DDIM_USER_SEM_WAIT_POL);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}

		// Set execution parameter.
		ercd = Dd_UDC_Ctrl_Common(ch, &udc_crl);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}

		// It changes to the setting for which phase lag count mode can be used by channel 2, 3. 
		Dd_Top_Set_PERSEL2_UDCTRG(0);

		// UDC start
		ercd = Dd_UDC_Start(ch);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}
	}

	VOID udc_sample_callback(UCHAR ch, INT32 factor)
	{
		UCHAR up_down_flag;

		if ((ch == D_DD_UDC_CH2) && (factor == D_DD_UDC_COUNT_DIRECTION)) {
			// Get up/down flag
			up_down_flag = Dd_UDC_Get_Up_Down_Flg(ch);

			if ((up_down_flag == D_DD_UDC_UDF_DOWN) || (up_down_flag == D_DD_UDC_UDF_BOTH)) {
				// The processing is executed here.
			}
		}
	}
	@endcode

@section dd_udc_sample_section4	Sample of Timer mode.
	The precondition of the this sample is as follows.<br>
	<ul>
	  <li>Channel 0 is used.</li>
	  <li>It processes every 100 counts, and it stops by 1000 counts.</li>
	</ul>
    @code
	UCHAR gCounter = 0;

	VOID udc_sample_execute_timer(VOID)
	{
		T_DD_UDC_CTRL_CMN udc_crl;
		UCHAR ch;
		INT32 ercd;

		ch       = D_DD_UDC_CH0;
		gCounter = 0;

		udc_crl.count_mode  = D_DD_UDC_CMS_TIMER;
		udc_crl.udcr        = 100;
		udc_crl.rcr         = 100;
		udc_crl.comp_clear  = D_DD_UDC_UCRE_OFF;
		udc_crl.reload      = D_DD_UDC_RLDE_ON;
		udc_crl.zin_mode    = D_DD_UDC_CGSC_CLEAR;
		udc_crl.zin_edge    = D_DD_UDC_CGE_NOT;
		udc_crl.cmp_int     = D_DD_UDC_CITE_OFF;
		udc_crl.under_over  = D_DD_UDC_UDIE_ON;
		udc_crl.cnt_dir_int = D_DD_UDC_CFIE_OFF;
		udc_crl.pCallBack   = udc_sample_callback;

		// UDC open.
		ercd = Dd_UDC_Open(ch,D_DDIM_USER_SEM_WAIT_POL);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}

		// Set common execution parameter.
		ercd = Dd_UDC_Ctrl_Common(ch, &udc_crl);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}
		// Set execution parameter only for timer mode.
		ercd = Dd_UDC_Ctrl_Timer(ch, D_DD_UDC_CLKS_2_CLOCK);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}

		// UDC start
		ercd = Dd_UDC_Start(ch);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}
	}

	VOID udc_sample_callback(UCHAR ch, INT32 factor)
	{
		INT32 ercd;

		if ((ch == D_DD_UDC_CH0) && (factor == D_DD_UDC_UNDER_FLOW)) {

			gCounter++;
			
			if (gCounter >= 10) {
				// UDC stop.
				ercd = Dd_UDC_Stop(ch);
				if (result != D_DDIM_OK) {
					// Error processing
					return;
				}

				// UDC close.
				ercd = Dd_UDC_Close(ch);
				if (result != D_DDIM_OK) {
					// Error processing
					return;
				}
			}

			// The processing of every 100 counts is executed here.
		}
	}
	@endcode

@section dd_udc_sample_section5	Sample of timer utility function.
	The precondition of the this sample is as follows.
	<ul>
	  <li>Channel 0 is used.</li>
	  <li>100ms is counted. </li>
	</ul>
	@code
	VOID udc_sample_timer_utility_function(VOID)
	{
		UCHAR ch;
		INT32 ercd;

		ch = D_DD_UDC_CH0;

		// UDC open.
		ercd = Dd_UDC_Open(ch,D_DDIM_USER_SEM_WAIT_POL);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}

		//Set execution parameter.
		ercd = Dd_UDC_Write_Timer_Mode( ch, 100000, udc_sample_callback );
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}

		// UDC start
		ercd = Dd_UDC_Start(ch);
		if (result != D_DDIM_OK) {
			// Error processing
			return;
		}
	}

	VOID udc_sample_callback(UCHAR ch)
	{
		INT32 ercd;

		if ((ch == D_DD_UDC_CH0) && (factor == D_DD_UDC_UNDER_FLOW)) {

			gTimer_Flag == 1;

			if (gTimer_Flag != 0) {
				// UDC close.
				ercd = Dd_UDC_Close(ch);
				if (ercd != D_DDIM_OK) {
					// Error processing
					return;
				}
			}
		}
	}

	@endcode
*/

/*@}*/
