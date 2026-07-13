/**
 * @file		dd_pwm.h
 * @brief		PWM driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_pwm
@{
	- @ref PWM_initialization <br>
	- @ref PWM_start_ABCD <br>
	- @ref PWM_start_E <br>
	- @ref PWM_start_timer <br>
	- @ref PWM_end_ABCD <br>
	- @ref PWM_end_E <br>
	- @ref PWM_Sample_TimerABCD <br>
	- @ref PWM_Sample_TimerE <br>
	- @ref PWM_Sample_Oneshot <br>
	- @ref PWM_Sample_Step <br>

	<hr>
	@section PWM_initialization	Sequence of PWM Initialization
	@image html dd_pwm_init.png

	@section PWM_start_ABCD	Sequence of PWM Operation Start by Software Trigger(for Timer ABCD)
	@image html dd_pwm_start_abcd.png

	@section PWM_start_E	Sequence of PWM Operation Start by Software Trigger(for Timer E)
	@image html dd_pwm_start_e.png

	@section PWM_start_timer	Sequence of PWM Operation Start by Timer Trigger(for Timer ABCD)
	@image html dd_pwm_start_timer.png

	@section PWM_end_ABCD	Sequence of PWM Operation End(for Timer ABCD)
	@image html dd_pwm_end_abcd.png

	@section PWM_end_E	Sequence of PWM Operation End(for Timer E)
	@image html dd_pwm_end_e.png

*/

#ifndef _DD_PWM_H_
#define _DD_PWM_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// Return value of PWM timer processing result
#define	D_DD_PWM_INPUT_PARAM_ERROR	(D_DD_PWM | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error */
#define	D_DD_PWM_SEM_TIMEOUT		(D_DD_PWM | D_DDIM_SEM_TIMEOUT)			/**< Semaphore acquisition Time Out */
#define	D_DD_PWM_SEM_NG				(D_DD_PWM | D_DDIM_SEM_NG)				/**< Semaphore acquisition NG */
#define	D_DD_PWM_OVERFLOW_ERROR		(D_DD_PWM | D_DDIM_OVERFLOW_ERR)		/**< Counter overflow and clock change NG */
#define	D_DD_PWM_OVERFLOW_OK		(D_DD_PWM | D_DDIM_OVERFLOW_OK)			/**< Counter overflow and clock change OK */

// Callback status
#define	D_DD_PWM_INT_STATUS_TRG_INPUT		(0)			/**< Callback status when Trigger input */
#define	D_DD_PWM_INT_STATUS_CYCLE_MATCH		(1)			/**< Callback status when Agreement cycle */
#define	D_DD_PWM_INT_STATUS_DUTY_MATCH		(2)			/**< Callback status when Agreement duty */
#define	D_DD_PWM_INT_STATUS_CYC_DUTY_MATCH	(3)			/**< Callback status when Agreement duty */
#define	D_DD_PWM_INT_STATUS_UNDER_RUN		(4)			/**< Callback status when Under run error (for Timer E) */
#define	D_DD_PWM_INT_STATUS_FORCE_STOP		(5)			/**< Callback status when Force stop (for Timer E) */
#define	D_DD_PWM_INT_STATUS_AUTO_STOP		(6)			/**< Callback status when Auto stop (for Timer E) */

// Num of channel
#define D_DD_PWM_CH_NUM_MAX					(16)			/**< The maximum values of number of channels */
#define D_DD_PWM_BLOCK_NUM					(4) 			/**< The maximum values of number of HW blocks */
#define D_DD_PWM_CH_NUM_IN_BLOCK			(4) 			/**< The maximum values of number of channels in HW block */
#define D_DD_PWM_TIMER_E_STEPS				(8) 			/**< The maximum values of number of steps in timer E */
#define D_DD_PWM_TIMER_E_STEPS_UNDER_RUN	(4) 			/**< The maximum values of number of steps in timer E under run*/

// timer operation enable
#define	D_DD_PWM_PCNHL_CNTE_OFF			(0x0) 				/**< timer disable */
#define	D_DD_PWM_PCNHL_CNTE_ON			(0x1) 				/**< timer enable */

// operation mode
#define	D_DD_PWM_PCNHL_MDSE_PWM			(0x0) 				/**< PWM */
#define	D_DD_PWM_PCNHL_MDSE_ONE			(0x1) 				/**< One-shot */

// restart enable
#define	D_DD_PWM_PCNHL_RTRG_OFF			(0x0) 				/**< disable */
#define	D_DD_PWM_PCNHL_RTRG_ON			(0x1) 				/**< enbale */

// counter clock
#define	D_DD_PWM_PCNHL_CKS_DIV_0		(0x0) 				/**< not dividing frequency	*/
#define	D_DD_PWM_PCNHL_CKS_DIV_4		(0x1) 				/**< 4 dividing frequency */
#define	D_DD_PWM_PCNHL_CKS_DIV_16		(0x2) 				/**< 16 dividing frequency */
#define	D_DD_PWM_PCNHL_CKS_DIV_64		(0x3) 				/**< 64 dividing frequency */

// output mask
#define	D_DD_PWM_PCNHL_PGMS_L			(0x0) 				/**< output is "Low" */
#define	D_DD_PWM_PCNHL_PGMS_H			(0x1) 				/**< output is "High" */

// trigger edge
#define	D_DD_PWM_PCNHL_EGS_NOT			(0x0) 				/**< invalid */
#define	D_DD_PWM_PCNHL_EGS_RISE			(0x1) 				/**< rise edge */
#define	D_DD_PWM_PCNHL_EGS_FALL			(0x2) 				/**< fall edge */
#define	D_DD_PWM_PCNHL_EGS_BOTH			(0x3) 				/**< both edge */

// interrupt enable
#define	D_DD_PWM_PCNHL_IREN_OFF			(0x0) 				/**< interrupt disable */
#define	D_DD_PWM_PCNHL_IREN_ON			(0x1) 				/**< interrupt enbale */

// interrupt factor
#define	D_DD_PWM_PCNHL_IRS_TRG			(0x0) 				/**< trigger input */
#define	D_DD_PWM_PCNHL_IRS_CYCLE		(0x1) 				/**< Agreement cycle */
#define	D_DD_PWM_PCNHL_IRS_DUTY			(0x2) 				/**< Agreement duty */
#define	D_DD_PWM_PCNHL_IRS_CYC_DTY		(0x3) 				/**< Agreement cycle or duty */

// output polarity
#define	D_DD_PWM_PCNHL_OSEL_NORMAL		(0x0) 				/**< output disable */
#define	D_DD_PWM_PCNHL_OSEL_REVERSE		(0x1) 				/**< output enbale */

// trigger input factor
#define	D_DD_PWM_GCN_TSEL_EM0			(0x00)				/**< EN0 bit */
#define	D_DD_PWM_GCN_TSEL_EM1			(0x01)				/**< EN1 bit */
#define	D_DD_PWM_GCN_TSEL_EM2			(0x02)				/**< EN2 bit */
#define	D_DD_PWM_GCN_TSEL_EM3			(0x03)				/**< EN3 bit */
#define	D_DD_PWM_GCN_TSEL_TIM0			(0x04)				/**< Timer ch0 */
#define	D_DD_PWM_GCN_TSEL_TIM1			(0x05)				/**< Timer ch1 */
#define	D_DD_PWM_GCN_TSEL_EXT0			(0x08)				/**< External trigger0 */
#define	D_DD_PWM_GCN_TSEL_EXT1			(0x09)				/**< External trigger1 */
#define	D_DD_PWM_GCN_TSEL_EXT2			(0x0A)				/**< External trigger2 */
#define	D_DD_PWM_GCN_TSEL_EXT3			(0x0B)				/**< External trigger3 */

// timer E enable
#define	D_DD_PWM_GCN_TMEEN_OFF			(0x0) 				/**< timer E disable */
#define	D_DD_PWM_GCN_TMEEN_ON			(0x1) 				/**< timer E enbale */

// channel assignment factor
#define	D_DD_PWM_GCN_CHAS_ABCD			(0x0) 				/**< channel order : A B C D */
#define	D_DD_PWM_GCN_CHAS_CBAD			(0x1) 				/**< channel order : C B A D */

// trigger input factor
#define	D_DD_PWM_TSEL_CPU				(0x00)				/**< CPU access	*/
#define	D_DD_PWM_TSEL_INT_CH0			(0x01)				/**< interrupt of ch0 */
#define	D_DD_PWM_TSEL_INT_CH1			(0x02)				/**< interrupt of ch1 */
#define	D_DD_PWM_TSEL_INT_CH2			(0x03)				/**< interrupt of ch2 */

// DMA interrupt factor
#define	D_DD_PWM_PCHSET_RQTIM_2			(0x00)				/**< empty 2 entries */
#define	D_DD_PWM_PCHSET_RQTIM_4			(0x01)				/**< empty 4 entries */
#define	D_DD_PWM_PCHSET_RQTIM_6			(0x02)				/**< empty 6 entries */

// under run error control enable
#define	D_DD_PWM_PCHSET_UDEN_OFF		(0x0) 				/**< disable */
#define	D_DD_PWM_PCHSET_UDEN_ON			(0x1) 				/**< enable */

// under run error interrupt enable
#define	D_DD_PWM_PINTCT_UIE_OFF			(0x0) 				/**< disable */
#define	D_DD_PWM_PINTCT_UIE_ON			(0x1) 				/**< enable */

// force stop interrupt enable
#define	D_DD_PWM_PINTCT_FIE_OFF			(0x0) 				/**< disable */
#define	D_DD_PWM_PINTCT_FIE_ON			(0x1) 				/**< enable */

// auto stop interrupt enable
#define	D_DD_PWM_PINTCT_AIE_OFF			(0x0) 				/**< disable */
#define	D_DD_PWM_PINTCT_AIE_ON			(0x1) 				/**< enable */

// DMA interrupt enable
#define	D_DD_PWM_PINTCT_DRQE_OFF		(0x0) 				/**< disable */
#define	D_DD_PWM_PINTCT_DRQE_ON			(0x1) 				/**< enable */

// PREV enable
#define	D_DD_PWM_PREV_OFF				(0x0) 				/**< disable */
#define	D_DD_PWM_PREV_ON				(0x1) 				/**< enable */


#define D_DD_PWM_TIMERE_DATA_MAX		(30) 			/**< The maximum values of number of steps in timer E */

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
// Force value for force_val of T_DD_PWM_STEP
#define	D_DD_PWM_STEP_FORCE_DISABLE	 	(-1)				/**< Force disable for force_val of T_DD_PWM_STEP */
#define	D_DD_PWM_STEP_FORCE_0			(0)					/**< Force to 0 for force_val of T_DD_PWM_STEP */
#define	D_DD_PWM_STEP_FORCE_1			(1)					/**< Force to 1 for force_val of T_DD_PWM_STEP */

#endif	/* CO_DDIM_UTILITY_USE */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
typedef void (*VP_CALLBACK_PWM)( UINT32 ch, UINT32 status );	/**< Type is defined to Callback function pointer for PWM */

/** PWM control info */
typedef struct {
	UCHAR drv_mode; 			/**< PWM or One-Shot operation	PCNH
									 <ul><li>@ref D_DD_PWM_PCNHL_MDSE_PWM 
										 <li>@ref D_DD_PWM_PCNHL_MDSE_ONE</ul>		*/
	UCHAR restart_trig; 		/**< restart enable				PCNH
									 <ul><li>@ref D_DD_PWM_PCNHL_RTRG_OFF 
										 <li>@ref D_DD_PWM_PCNHL_RTRG_ON</ul>		*/
	UCHAR clk_sel;				/**< clock select				PCNH
									 <ul><li>@ref D_DD_PWM_PCNHL_CKS_DIV_0 
										 <li>@ref D_DD_PWM_PCNHL_CKS_DIV_4
										 <li>@ref D_DD_PWM_PCNHL_CKS_DIV_16
										 <li>@ref D_DD_PWM_PCNHL_CKS_DIV_64</ul>	*/
	UCHAR out_mask; 			/**< output level mask			PCNH
									 <ul><li>@ref D_DD_PWM_PCNHL_PGMS_L 
										 <li>@ref D_DD_PWM_PCNHL_PGMS_H</ul>		*/
	UCHAR edge; 				/**< edge enable select			PCNH
									 <ul><li>@ref D_DD_PWM_PCNHL_EGS_NOT 
										 <li>@ref D_DD_PWM_PCNHL_EGS_RISE
										 <li>@ref D_DD_PWM_PCNHL_EGS_FALL
										 <li>@ref D_DD_PWM_PCNHL_EGS_BOTH</ul>		*/
	UCHAR int_enable;			/**< interrupt enable			PCNH
									 <ul><li>@ref D_DD_PWM_PCNHL_IREN_OFF 
										 <li>@ref D_DD_PWM_PCNHL_IREN_ON</ul>		*/
	UCHAR trigger;				/**< interrupt factor			PCNH
									 <ul><li>@ref D_DD_PWM_PCNHL_IRS_TRG 
										 <li>@ref D_DD_PWM_PCNHL_IRS_CYCLE
										 <li>@ref D_DD_PWM_PCNHL_IRS_DUTY
										 <li>@ref D_DD_PWM_PCNHL_IRS_CYC_DTY</ul>	*/
	UCHAR polarity; 			/**< output polarity			PCNH
									 <ul><li>@ref D_DD_PWM_PCNHL_OSEL_NORMAL 
										 <li>@ref D_DD_PWM_PCNHL_OSEL_REVERSE</ul>	*/
	UCHAR in_trigger;			/**< trigger input factor		GCN1
									 <ul><li>@ref D_DD_PWM_GCN_TSEL_EM0 
										 <li>@ref D_DD_PWM_GCN_TSEL_EM1
										 <li>@ref D_DD_PWM_GCN_TSEL_EM2
										 <li>@ref D_DD_PWM_GCN_TSEL_EM3
										 <li>@ref D_DD_PWM_GCN_TSEL_TIM0
										 <li>@ref D_DD_PWM_GCN_TSEL_TIM1
										 <li>@ref D_DD_PWM_GCN_TSEL_EXT0
										 <li>@ref D_DD_PWM_GCN_TSEL_EXT1
										 <li>@ref D_DD_PWM_GCN_TSEL_EXT2
										 <li>@ref D_DD_PWM_GCN_TSEL_EXT3</ul>		*/
	UCHAR tim_e_enable; 		/**< timer E enable				GCN2
									 <ul><li>@ref D_DD_PWM_GCN_TMEEN_OFF 
										 <li>@ref D_DD_PWM_GCN_TMEEN_ON</ul>		*/
	UCHAR ch_assign;			/**< channel assignment			GCN2
									 <ul><li>@ref D_DD_PWM_GCN_CHAS_ABCD 
										 <li>@ref D_DD_PWM_GCN_CHAS_CBAD</ul>		*/
	VP_CALLBACK_PWM	pCallBack;	/**< callback function pointer		 */
} T_DD_PWM_CTRL_CMN;

/** PWM timer A,B,C,D control info */
typedef struct {
	USHORT cyc_time;			/**< wave cycle(usec)				*/
	USHORT cyc_num; 			/**< number of cycle				*/
	USHORT duty_num;			/**< number of duty					*/
} T_DD_PWM_CTRL_TIMER_ABCD;


/** PWM timer A,B,C,D control info */
typedef struct {
	USHORT cyc_time[2];			/**< wave cycle(usec)				*/
	USHORT cyc_num[2]; 			/**< number of cycle				*/
	USHORT duty_num[2];			/**< number of duty					*/
} T_DD_PWM_CTRL_TIMER_ABCD_2ch;


/** PWM timer E pulse and link data */
typedef union {
	USHORT  hword;					/**< for USHORT access 			*/
	struct {
		USHORT	pulse_num	:10;	/**< number of pulse			*/
		USHORT				:5;		/**< Reserved					*/
		USHORT	link		:1;		/**< link enable				*/
	} bit;							/**< for bit access				*/
} T_DD_PWM_TIMER_E_PULSE_LINK;

/** PWM timer E steps data */
typedef struct {
	USHORT cyc_num;							/**< number of cycle				*/
	USHORT duty_num;						/**< number of duty					*/
	T_DD_PWM_TIMER_E_PULSE_LINK pulse_link;	/**< number of pulse and link		*/
} T_DD_PWM_TIMER_E_STEPS_DATA;

/** PWM timer E control info */
typedef struct {
	UCHAR hold[D_DD_PWM_CH_NUM_IN_BLOCK];											/**< hold setting at stop bit	(0:disable	1:enable)	PCHSETH */
	UCHAR force_val[D_DD_PWM_CH_NUM_IN_BLOCK]; 										/**< force value at stop bit	(0 or 1)				PCHSETH */
	UCHAR force[D_DD_PWM_CH_NUM_IN_BLOCK]; 											/**< force setting at stop bit	(0:disable	1:enable)	PCHSETH */
	UCHAR empty_dma_req;															/**< Inerrupt request timing for DMA	PCHSETL
																					 <ul><li>@ref D_DD_PWM_PCHSET_RQTIM_2 
																					     <li>@ref D_DD_PWM_PCHSET_RQTIM_4
																					     <li>@ref D_DD_PWM_PCHSET_RQTIM_6</ul>		*/
	UCHAR undr_run_enable;															/**< speed down setting under run error	PCHSETL
																					 <ul><li>@ref D_DD_PWM_PCHSET_UDEN_OFF 
																					     <li>@ref D_DD_PWM_PCHSET_UDEN_ON</ul>		*/
	UCHAR undr_run_int_err_enable;													/**< interrupt enable under run error	PINTCTH
																					 <ul><li>@ref D_DD_PWM_PINTCT_UIE_OFF 
																					     <li>@ref D_DD_PWM_PINTCT_UIE_ON</ul>		*/
	UCHAR force_stop_int_enable;													/**< interrupt enable for force stop	PINTCTH
																					 <ul><li>@ref D_DD_PWM_PINTCT_FIE_OFF 
																					     <li>@ref D_DD_PWM_PINTCT_FIE_ON</ul>		*/
	UCHAR auto_stop_int_enable; 													/**< interrupt enable for auto stop		PINTCTH
																					 <ul><li>@ref D_DD_PWM_PINTCT_AIE_OFF 
																					     <li>@ref D_DD_PWM_PINTCT_AIE_ON</ul>		*/
	UCHAR dma_int_enable;															/**< interrupt enable for dma			PINTCTL
																					 <ul><li>@ref D_DD_PWM_PINTCT_DRQE_OFF 
																					     <li>@ref D_DD_PWM_PINTCT_DRQE_ON</ul>		*/
	T_DD_PWM_TIMER_E_STEPS_DATA steps_data[D_DD_PWM_TIMER_E_STEPS];					/**< steps data(cycle,duty,pulse,link)			*/
	T_DD_PWM_TIMER_E_STEPS_DATA under_run_data[D_DD_PWM_TIMER_E_STEPS_UNDER_RUN];	/**< under run data(cycle,duty,pulse,link)		*/
} T_DD_PWM_CTRL_TIMER_E;

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
/** PWM stepping motor control info */
typedef struct {
	UCHAR clk_sel;									/**< clock select						PCNH
													 <ul><li>@ref D_DD_PWM_PCNHL_CKS_DIV_0 
													 <li>@ref D_DD_PWM_PCNHL_CKS_DIV_4
													 <li>@ref D_DD_PWM_PCNHL_CKS_DIV_16
													 <li>@ref D_DD_PWM_PCNHL_CKS_DIV_64</ul>		*/
	UCHAR polarity; 								/**< output polarity					PCNH
													 <ul><li>@ref D_DD_PWM_PCNHL_OSEL_NORMAL 
													 <li>@ref D_DD_PWM_PCNHL_OSEL_REVERSE</ul>		*/
	UCHAR restart_trig; 							/**< restart enable						PCNH
													 <ul><li>@ref D_DD_PWM_PCNHL_RTRG_OFF 
														 <li>@ref D_DD_PWM_PCNHL_RTRG_ON</ul>		*/
	UCHAR ch_assign;								/**< channel assignment					GCN2
													 <ul><li>@ref D_DD_PWM_GCN_CHAS_ABCD 
														 <li>@ref D_DD_PWM_GCN_CHAS_CBAD</ul>		*/
	UCHAR hold[D_DD_PWM_CH_NUM_IN_BLOCK];			/**< hold setting at stop	(0:disable	1:enable)							PCHSETH */
	INT8 force[D_DD_PWM_CH_NUM_IN_BLOCK]; 			/**< force value at stop	(-1:force disable, 0:Force to 0, 1:Force to 1)	PCHSETH */
	UCHAR step_data_num;							/**< No. of stepping motor data(1 to 255)			*/
	UCHAR undr_run_data_num;						/**< No. of under run stepping motor data(0 to 4)	*/
	T_DD_PWM_TIMER_E_STEPS_DATA* step_data;			/**< stepping motor data(cycle,duty,pulse,link)	*/
	T_DD_PWM_TIMER_E_STEPS_DATA* under_run_data;	/**< under run data(cycle,duty,pulse,link)		*/
	VP_CALLBACK_PWM	pCallBack;						/**< callback function pointer					*/
} T_DD_PWM_STEP;
#endif	/* CO_DDIM_UTILITY_USE */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function 															*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

//---------------------- driver  section -------------------------------
/**
The PWM control register (all channels) is initialized.
*/
VOID Dd_PWM_Init(VOID);

/**
Execute exclusive control for designated PWM channel.<br>
When the designated channel is idle (un-lock) state, change the channel state to use(lock) and return "D_DDIM_OK",<br>
if the channel state is in-use (lock), return "D_DD_PWM_SEM_NG".<br>
When timeout occurs, return "D_DD_PWM_SEM_TIMEOUT".

@param [in] ch		Channel number (0 to 15)<br>
@param [in] tmout	Time of timeout<br>
					<ul><li>Positive Value(Time of timeout)
						<li>@ref D_DDIM_USER_SEM_WAIT_POL
						<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_PWM_SEM_TIMEOUT		Semaphore acquisition Time Out
@retval D_DD_PWM_SEM_NG				Semaphore acquisition NG
@remarks Please call Dd_PWM_Open_Timer_E when timer E is used.
@remarks This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
		 This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
@remarks This API uses DDIM_User_Sig_Sem().
*/
extern INT32 Dd_PWM_Open(UINT32 ch, INT32 tmout);

/**
Cancel exclusive control for designated PWM channel.<br>

@param [in] ch	Channel number (0 to 15)<br>
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
@retval	D_DD_UART_SEM_NG			Semaphore release NG
@remarks
		- Please note that stop & exclusive control is compulsorily released when PWM is starting.
		- Please call Dd_PWM_Close_Timer_E when timer E is used.
@remarks	This API uses DDIM_User_Sig_Sem().
*/
extern INT32 Dd_PWM_Close(UINT32 ch);

/**
Execute exclusive control for designated PWM Tier-E channel.<br>
When the designated channel is idle (un-lock) state, change the channel state to use(lock) and return "D_DDIM_OK",<br>
if the channel state is in-use (lock), return "D_DD_PWM_SEM_NG".<br>
When timeout occurs, return "D_DD_PWM_SEM_TIMEOUT".

@param [in] ch	Channel number (0, 4, 8, 12)
@param [in] tmout	Time of timeout<br>
					<ul><li>Positive Value(Time of timeout)
						<li>@ref D_DDIM_USER_SEM_WAIT_POL
						<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_PWM_SEM_TIMEOUT		Semaphore acquisition Time Out
@retval D_DD_PWM_SEM_NG				Semaphore acquisition NG
@remarks This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
		 This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern INT32 Dd_PWM_Open_Timer_E(UINT32 ch, INT32 tmout);

/**
Cancel exclusive control for designated PWM channel for Timer E.

@param [in] ch	Channel number (0, 4, 8, 12)
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
@retval	D_DD_UART_SEM_NG			Semaphore release NG
@remarks Please note that stop & exclusive control is compulsorily released when PWM is starting.
@remarks This API uses DDIM_User_Sig_Sem().
*/
extern INT32 Dd_PWM_Close_Timer_E(UINT32 ch);

/**
The PWM operational condition of a control channel is set.<br>
Set the callback function when interruption is generate to pCallBack.

@param [in]		ch				Channel number (0 to 15).<br>
	When timer E is set, (0 to 3) means hardware block number is 0,<br>
	(4 to 7) means hardware block number is 1, <br>
	(8 to 11) means hardware block number is 2,<br>
	(12 to 15) means hardware block number is 3.
@param [in]    *pwm_ctrl_cmn	PWM Start table pointer. See @ref T_DD_PWM_CTRL_CMN.<br>
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Ctrl_Common(UINT32 ch, T_DD_PWM_CTRL_CMN const* const pwm_ctrl_cmn);

/**
The PWM operational condition of a control channel is set.<br>
Set the callback function when interruption is generate to pCallBack.

@param [in]		ch				Channel number (0 to 15).<br>
	When timer E is set, (0 to 3) means hardware block number is 0,<br>
	(4 to 7) means hardware block number is 1, <br>
	(8 to 11) means hardware block number is 2,<br>
	(12 to 15) means hardware block number is 3.
@param [in]    *pwm_ctrl_cmn	PWM Start table pointer. See @ref T_DD_PWM_CTRL_CMN.<br>
@param [in]    prev_en	reverse resister enable(1)/disable(0).<br>
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Ctrl_Common2(UINT32 ch, T_DD_PWM_CTRL_CMN const* const pwm_ctrl_cmn, UCHAR prev_en);

/**
The PWM operational condition of a control channel is get.<br>

@param [in]		ch				Channel number (0 to 15).<br>
@param [in]    *pwm_ctrl_cmn	PWM Start table pointer. See @ref T_DD_PWM_CTRL_CMN.<br>
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Get_Ctrl_Common(UINT32 ch, T_DD_PWM_CTRL_CMN* const pwm_ctrl_cmn);

/**
The PWM Timer A,B,C,D is set.<br>

@param [in]		ch				Channel number (0 to 15).<br>
@param [in]    *pwm_ctrl_timer_abcd	PWM Start table pointer. See @ref T_DD_PWM_CTRL_TIMER_ABCD.<br>
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Ctrl_Timer_ABCD(UINT32 ch, T_DD_PWM_CTRL_TIMER_ABCD const* const pwm_ctrl_timer_abcd);

/**
The PWM Timer A,B,C,D is set.<br>

@param [in]		ch				Channel number (0 to 15).<br>
@param [in]    *pwm_ctrl_timer_abcd	PWM Start table pointer. See @ref T_DD_PWM_CTRL_TIMER_ABCD.<br>
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Ctrl_Timer_ABCD_2ch(UINT32 ch, T_DD_PWM_CTRL_TIMER_ABCD_2ch const* const pwm_ctrl_timer_abcd_2ch);

/**
The PWM Timer E is set.<br>

@param [in]		ch		Channel number (0 to 15).<br>
						(0 to 3) means hardware block number is 0,<br>
						(4 to 7) means hardware block number is 1,<br>
						(8 to 11) means hardware block number is 2,<br>
						(12 to 15) means hardware block number is 3.
@param [in]    *pwm_ctrl_timer_e	PWM Start table pointer. See @ref T_DD_PWM_CTRL_TIMER_E.<br>
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Ctrl_Timer_E(UINT32 ch, T_DD_PWM_CTRL_TIMER_E const* const pwm_ctrl_timer_e);

/**
The PWM of a control channel is started.

@param [in]		ch			Channel number (0 to 15)
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
@remarks The following status is notified by the callback.<br>
	@code
	D_DD_PWM_INT_STATUS_TRG_INPUT		: PWM Status is Trigger input
	D_DD_PWM_INT_STATUS_CYCLE_MATCH		: PWM Status is Agreement cycle
	D_DD_PWM_INT_STATUS_DUTY_MATCH		: PWM Status is Agreement duty
	D_DD_PWM_INT_STATUS_CYC_DUTY_MATCH	: PWM Status is Agreement duty
	D_DD_PWM_INT_STATUS_UNDER_RUN		: PWM Status is Under run error for stepping motor
	D_DD_PWM_INT_STATUS_FORCE_STOP		: PWM Status is Force stop for stepping motor
	D_DD_PWM_INT_STATUS_AUTO_STOP		: PWM Status is Auto stop for stepping motor
	@endcode
*/
extern INT32 Dd_PWM_Start(UINT32 ch);

/**
The PWM of 2 control channel is started.

@param [in]		ch			Channel number (0 to 15)
@param [in]		ch			Channel number (0 to 15)
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
@remarks The following status is notified by the callback.<br>
	@code
	D_DD_PWM_INT_STATUS_TRG_INPUT		: PWM Status is Trigger input
	D_DD_PWM_INT_STATUS_CYCLE_MATCH		: PWM Status is Agreement cycle
	D_DD_PWM_INT_STATUS_DUTY_MATCH		: PWM Status is Agreement duty
	D_DD_PWM_INT_STATUS_CYC_DUTY_MATCH	: PWM Status is Agreement duty
	D_DD_PWM_INT_STATUS_UNDER_RUN		: PWM Status is Under run error for stepping motor
	D_DD_PWM_INT_STATUS_FORCE_STOP		: PWM Status is Force stop for stepping motor
	D_DD_PWM_INT_STATUS_AUTO_STOP		: PWM Status is Auto stop for stepping motor
	@endcode
*/
extern INT32 Dd_PWM_2ch_Start(UINT32 ch_a, UINT32 ch_b);

/**
The PWM of a control channel is stopped.

@param [in]		ch			Channel number (0 to 15)
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Stop(UINT32 ch);

/**
Get value of timer register.

@param [in]		ch				Channel number (0 to 15).<br>
@param [out]	*timer_val		timer register value<br>
	If timer_val = 0, it does not get value.
@param [out]	*timer_e_val	timer E register value<br>
	channel 0 to 3	means hardware block number is 0,<br>
	channel 4 to 7	means hardware block number is 1,<br>
	channel 8 to 11 means hardware block number is 2,<br>
	channel 12 to 15 means hardware block number is 3.<br>
	If timer_e_val = 0, it does not get value.
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Get_Timer(UINT32 ch, USHORT* timer_val, USHORT* timer_e_val);

/**
Get number of output pulse.

@param [in]		ch				Channel number (0 to 15).<br>
	channel 0 to 3	means hardware block number is 0,<br>
	channel 4 to 7	means hardware block number is 1,<br>
	channel 8 to 11 means hardware block number is 2,<br>
	channel 12 to 15 means hardware block number is 3.
@param [out]	*pulse_num		output pulse number
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Get_Pulse_Num(UINT32 ch, ULONG* pulse_num);

/**
Get interrupt flag and enable for timer E.

@param [in]		ch				Channel number (0 to 15).<br>
	channel 0 to 3	means hardware block number is 0,<br>
	channel 4 to 7	means hardware block number is 1,<br>
	channel 8 to 11 means hardware block number is 2,<br>
	channel 12 to 15 means hardware block number is 3.
@param [out]	*int_flg_ctrl_0		flag and enable<br>
	bit12 UIS (under run error interrupt status flag)<br>
	bit9  FIS (force stop interrupt status flag)<br>
	bit8  AIS (auto stop interrupt status flag)<br>
	bit4  UIE (under run error interrupt enable control)<br>
	bit1  FIE (force stop interrupt enable control)<br>
	bit0  AIE (auto stop interrupt enable control)
@param [out]	*int_flg_ctrl_1		flag and enable<br>
	bit15 DRQS (DMA interrupt status flag)<br>
	bit7  DRQE (DMA interrupt enable control)
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Get_Interrupt_Flag(UINT32 ch, USHORT* int_flg_ctrl_0, USHORT* int_flg_ctrl_1);

/**
Set interrupt flag and enable for timer E.

@param [in]		ch				Channel number (0 to 15).<br>
	channel 0 to 3	means hardware block number is 0,<br>
	channel 4 to 7	means hardware block number is 1,<br>
	channel 8 to 11 means hardware block number is 2,<br>
	channel 12 to 15 means hardware block number is 3.
@param [in]	int_flg_ctrl_0		flag and enable<br>
	bit12 UIS (under run error interrupt status flag)<br>
	bit9  FIS (force stop interrupt status flag)<br>
	bit8  AIS (auto stop interrupt status flag)<br>
	bit4  UIE (under run error interrupt enable control)<br>
	bit1  FIE (force stop interrupt enable control)<br>
	bit0  AIE (auto stop interrupt enable control)
@param [in]	int_flg_ctrl_1		flag and enable<br>
	bit15 DRQS (DMA interrupt status flag)<br>
	bit7  DRQE (DMA interrupt enable control)
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Set_Interrupt_Flag(UINT32 ch, USHORT int_flg_ctrl_0, USHORT int_flg_ctrl_1);

/**
Get channel assignment.

@param [in]		ch				Channel number (0 to 15).<br>
	channel 0 to 3	means hardware block number is 0,<br>
	channel 4 to 7	means hardware block number is 1,<br>
	channel 8 to 11 means hardware block number is 2,<br>
	channel 12 to 15 means hardware block number is 3.
@param [out]	*ch_assign		channel assignment.<br>
	@code
	D_DD_PWM_GCN_CHAS_ABCD	: Channel order : A B C D
	D_DD_PWM_GCN_CHAS_CBAD	: Channel order : C B A D
	@endcode
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Get_Channel_Assign(UINT32 ch, UCHAR* ch_assign);

/**
Set channel assignment.

@param [in]		ch				Channel number (0 to 15).<br>
	channel 0 to 3	means hardware block number is 0,<br>
	channel 4 to 7	means hardware block number is 1,<br>
	channel 8 to 11 means hardware block number is 2,<br>
	channel 12 to 15 means hardware block number is 3.
@param [out]	*ch_assign		channel assignment.<br>
	@code
	D_DD_PWM_GCN_CHAS_ABCD	: Channel order : A B C D
	D_DD_PWM_GCN_CHAS_CBAD	: Channel order : C B A D
	@endcode
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Set_Channel_Assign(UINT32 ch, UCHAR ch_assign);

/**
Get Interrupt Status of PWM. (Not PWM_STP)

@param [in]		ch				Channel number (0 to 15)
@retval TRUE	Interrupt request occur
@retval FALSE	No interrupt request
*/
extern BOOL Dd_PWM_Get_Int_Stat(UINT32 ch);

/**
It is Interrupt Handler of PWM.<br>
The interruption flag is cleared, and the CallBack function is called.<br>
(Only an interruption clear flag of CallBack when it is unregistered.)
@param [in]		ch	Channel number (0, 4, 8, 12)
*/
extern VOID Dd_PWM_Int_Handler(UINT32 ch);

/**
It is Interrupt Handler of PWM STP.<br>
The interruption flag is cleared, and the CallBack function is called.<br>
(Only an interruption clear flag of CallBack when it is unregistered.)
@param [in]		ch	Channel number (0, 4, 8, 12)
*/
extern VOID Dd_PWM_Int_Handler_STP(UINT32 ch);

/**
The Mask status is set. Only when the corresponding timer ABCD.
@param [in]	ch						Channel number (0 to 15)
@param [in]	mask					Mask Set(0 or 1)
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Set_Stopped_OutPut_Mask(UINT32 ch, UCHAR mask);

/**
Get setting value of the mask. Only when the corresponding timer ABCD.
@param [in]		ch					Channel number (0 to 15)
@param [out]	*mask				Mask Status
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
 */
extern INT32 Dd_PWM_Get_Stopped_OutPut_Mask(UINT32 ch, UCHAR* mask);

/**
Get setting value of the conter clock of TimerE
@param [in]		ch					Channel number (0 to 15)
@param [out]	*clock				clock_sel
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
 */
extern INT32 Dd_PWM_Get_Clock_Sel_TimerE(UINT32 ch, UCHAR* clock);

/**
Get the PWM output state. Only stopped.
@param [in]		ch					Channel number (0 to 15)
@param [out]	*ch_status			Output Status
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Get_Stopped_OutPut_Status(UINT32 ch, UCHAR* ch_status);

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/** @name Utility Functions
@{*/

/**
Cycle parameters are calculated according to the designated time (u sec).<br>
If it doesn't calculate, Input Parameter Error is returned.<br>
One dividing frequency that can be divided is raised when the count value <br>
overflows since it calculates and it calculates again.<br>
In that case, "D_DD_PWM_OVERFLOW_OK" is returned.<br>
"D_DD_PWM_OVERFLOW_ERROR" is returned when it cannot be divded even if it calculates again.

@param [in]		usec	The designated time of PWM
@param [out]   *clk		Number of clocks calculated at specified time
@param [out]   *cycle	Cycle calculated at specified time number
@retval D_DDIM_OK				OK
@retval D_DD_PWM_OVERFLOW_OK	Overflow OK
@retval D_DD_PWM_OVERFLOW_ERROR	Overflow NG
*/
extern INT32 Dd_PWM_Calculate(ULONG usec, UCHAR* const clk, USHORT* const cycle);

/**
Cycle parameters are calculated according to the designated time (u sec)<br>
and selected index of clock divider.<br>
If it doesn't calculate, Input Parameter Error is returned.<br>
In that case, "D_DD_PWM_OVERFLOW_OK" is returned.<br>
"D_DD_PWM_OVERFLOW_ERROR" is returned when it cannot be divded even if it calculates again.

@param [in]		usec	The designated time of PWM
@param [in]     clk		index of clock divider.
@param [out]   *cycle	Cycle calculated at specified time number
@retval D_DDIM_OK				OK
@retval D_DD_PWM_OVERFLOW_ERROR	Overflow NG
*/
extern INT32 Dd_PWM_Calculate_Cycle(ULONG usec, UCHAR const clk, USHORT* const cycle);

/**
PWM output control channel wave form is set (change).<br>
The setting is reflected at the counter borrow opportunity.
@param [in] ch		Channel number (0 to 15)
@param [in] cycle	Cycle for PWM
@param [in] duty	Duty ratio for PWM(0-100%)
@param [in] clk_sel	Selection of PWM clock(PHCLK, PHCLK/4, PHCLK/16, PHCLK/64)
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
@remarks set to become cycle>=duty the relation between set cycle and duty.
*/
extern INT32 Dd_PWM_Write_Cycle_Duty(UINT32 ch, USHORT cycle, UCHAR duty, UCHAR clk_sel);

/**
PWM output control channel wave form is set for reverse(change).<br>
The setting is reflected at the counter borrow opportunity.
@param [in] ch		Channel number (0 to 15)
@param [in] cycle	Cycle of reverse for PWM
@param [in] duty	Duty ratio of reverse for PWM(0-100%)
@param [in] clk_sel	Selection of PWM clock(PHCLK, PHCLK/4, PHCLK/16, PHCLK/64)
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
@remarks Please set to become cycle>=duty the relation between set cycle and duty.
*/
extern INT32 Dd_PWM_Write_Cycle_Duty_Reverse(UINT32 ch, USHORT cycle, UCHAR duty, UCHAR clk_sel);

/**
PWM output control channel wave form is set for queue(change).<br>
The setting is reflected at the counter borrow opportunity.
@param [in] ch		Channel number (0 to 15)
@param [in] cycle	Cycle of queue for PWM
@param [in] duty	Duty ratio of queue for PWM(0-100%)
@param [in] clk_sel	Selection of PWM clock(PHCLK, PHCLK/4, PHCLK/16, PHCLK/64)
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
@remarks Please set to become cycle>=duty the relation between set cycle and duty.
*/
extern INT32 Dd_PWM_Write_Cycle_Duty_Queue(UINT32 ch, USHORT cycle, UCHAR duty, UCHAR clk_sel);

/**
Set an input trigger for PWM specified channel.<br>
The setting is reflected at the counter borrow opportunity.
@param [in] ch		Channel number (0 to 15)
@param [in] gcn_tsel	Input trigger
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
@remarks Please set to become cycle>=duty the relation between set cycle and duty.
*/
extern INT32 Dd_PWM_Write_Input_Trg(UINT32 ch, UCHAR gcn_tsel);

/**
Switch a writing trigger for PWM specified channel.
@param [in] ch		Channel number (0 to 15)
@param [in] tsel	Writing trigger select
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Write_Cycle_Duty_Switch(UINT32 ch, UCHAR tsel);

/**
Set a setting for PWM specified channel.
@param [in] ch		Channel number (0 to 15)
@param [in] pcn		Control status
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Write_Pcn(UINT32 ch, USHORT pcn);

/**
The PWM Timer Onshot is set.
@param [in]	ch						Channel number (0 to 15)
@param [in]	duty_time				Duty time(usec)
@param [in]	*pwm_ctrl_cmn			PWM Start table pointer. See @ref T_DD_PWM_CTRL_CMN
@param [in]	*pwm_ctrl_timer_abcd	PWM Start table pointer. See @ref T_DD_PWM_CTRL_TIMER_ABCD
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Write_Oneshot(UINT32 ch,
								  USHORT duty_time,
								  T_DD_PWM_CTRL_CMN* const pwm_ctrl_cmn,
								  T_DD_PWM_CTRL_TIMER_ABCD* const pwm_ctrl_timer_abcd);

//---------------------- colabo  section -------------------------------
/**
The PWM Timer E and DMA is set for stepping motor.
@param [in]	pwm_ch			PWM Channel number (0 or 4 or 8 or 12).<br>
@param [in]	dma_ch			HDMAC0 Channel number (0 to 7).<br>
@param [in]	*pwm_step	PWM Start table pointer. See @ref T_DD_PWM_STEP.<br>
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Write_Step(UINT32 pwm_ch, UINT32 dma_ch, T_DD_PWM_STEP const* const pwm_step);

/**
The PWM Timer E and DMA is set for stepping motor.
@param [in]	pwm_ch			PWM Channel number (0 or 4 or 8 or 12).<br>
@param [in]	dma_ch			HDMAC0 Channel number (0 to 7).<br>
@param [in]	*pwm_step	PWM Start table pointer. See @ref T_DD_PWM_STEP.<br>
@param [in]	data_num		the number of entry data<br>
@retval D_DDIM_OK					OK
@retval D_DD_PWM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_PWM_Write_Step_Only_Buf_Data_Window(UINT32 pwm_ch, UINT32 dma_ch, T_DD_PWM_TIMER_E_STEPS_DATA const* const pwm_step, UCHAR data_num );

/*@}*/
/*@}*/

/** @weakgroup dd_pwm
@{
	<hr>
	Utility Functions <br>
	- @ref PWM_start_reverse_queue <br>
	- @ref PWM_oneshot <br>
	- @ref PWM_step <br>

	@section PWM_start_reverse_queue	Sequence of PWM set and change for cycle and duty
	@image html dd_pwm_start_reverse_queue.png

	@section PWM_oneshot	Sequence of PWM oneshot
	@image html dd_pwm_oneshot.png

	@section PWM_step	Sequence of PWM stepping motor
	@image html dd_pwm_step.png

*/
/*@}*/
#endif	/* CO_DDIM_UTILITY_USE */

#ifdef __cplusplus
}
#endif

#endif

/**
@weakgroup dd_pwm
@{

	<br>
	<br>
	The example of PWM is as follows.
	<hr>
	@section PWM_Sample_TimerABCD		Sample of PWM with Timer A,B,C,D

@code
	void PWM_Sample_TimerABCD()
	{
		INT32						ret;
		T_DD_PWM_CTRL_CMN			ctrl_cmn;
		T_DD_PWM_CTRL_TIMER_ABCD	ctrl_abcd;
		USHORT						cyc_time	= 60000;	// wave cycle(usec)
		UCHAR						duty		= 50;		// 50%

		Dd_PWM_Init();			// After power on and the reset, this is executed only once.

		ret = Dd_PWM_Open(ch, D_DDIM_USER_SEM_WAIT_POL);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Open !!\n"));
			return;
		}

		ctrl_cmn.drv_mode		= D_DD_PWM_PCNHL_MDSE_PWM;		// PWM or One-Shot operation
		ctrl_cmn.restart_trig	= D_DD_PWM_PCNHL_RTRG_OFF;		// restart enable
		ctrl_cmn.out_mask		= D_DD_PWM_PCNHL_PGMS_L;		// output level mask
		ctrl_cmn.edge			= D_DD_PWM_PCNHL_EGS_RISE;		// edge enable select
		ctrl_cmn.int_enable		= D_DD_PWM_PCNHL_IREN_OFF;		// interrupt enable
		ctrl_cmn.trigger		= D_DD_PWM_PCNHL_IRS_TRG;		// interrupt factor
		ctrl_cmn.polarity		= D_DD_PWM_PCNHL_OSEL_NORMAL;	// output polarity
		ctrl_cmn.in_trigger		= D_DD_PWM_GCN_TSEL_EM0;		// trigger input factor
		ctrl_cmn.tim_e_enable	= D_DD_PWM_GCN_TMEEN_OFF;		// Timer E disable
		ctrl_cmn.ch_assign		= D_DD_PWM_GCN_CHAS_ABCD;		// channel assignment
		ctrl_cmn.pCallBack		= (VP_CALLBACK_PWM)sample_func;	// callback function pointer

		ctrl_abcd.cyc_time		= cyc_time;						// wave cycle(usec)

		ret = Dd_PWM_Calculate(ctrl_abcd.cyc_time, &ctrl_cmn.clk_sel, &ctrl_abcd.cyc_num);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM cycle calculate !!\n"));
			return;
		}

		ret = Dd_PWM_Ctrl_Common(ch, &ctrl_cmn);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Ctrl !!\n"));
		}

		ret = Dd_PWM_Ctrl_Timer_ABCD(ch, &ctrl_abcd);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Timer A,B,C,D Ctrl !!\n"));
		}

		ret = Dd_PWM_Write_Cycle_Duty(ch, ctrl_abcd.cyc_num, duty, ctrl_cmn.clk_sel);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Change cycle and duty !!\n"));
		}

		ret = Dd_PWM_Start(ch);
		Ddim_Print(("PWM Start !!\n"));
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Start !!\n"));
		}

	//--------------------------------------------------------
	// It is a termination as follows.
	// Please execute it after completing processing.

		ret = Dd_PWM_Stop(ch);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Stop !!\n"));
		}

		ret = Dd_PWM_Close(ch);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Close !!\n"));
		}
	}
@endcode

	<br>
	<br>
	@section PWM_Sample_TimerE			Sample of PWM with Timer E

@code
	void PWM_Sample_TimerE()
	{
		INT32						ret;
		T_DD_PWM_CTRL_CMN			ctrl_cmn;
		T_DD_PWM_CTRL_TIMER_E		ctrl_e;
		UINT32						ch			= 4;		// used channel number. 0, 4, 8, 12 are valid.
		USHORT						cyc_time	= 60000;	// wave cycle(usec)
		USHORT						cyc_num;
		UCHAR						ch_in_blk;

		ch_in_blk = (D_DD_PWM_CH_NUM_IN_BLOCK - 1) & ch;

		Dd_PWM_Init();			// After power on and the reset, this is executed only once.

		ret = Dd_PWM_Open_Timer_E(ch, D_DDIM_USER_SEM_WAIT_POL);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Open !!\n"));
			return;
		}

		ctrl_cmn.drv_mode				= D_DD_PWM_PCNHL_MDSE_PWM;		// PWM or One-Shot operation
		ctrl_cmn.restart_trig			= D_DD_PWM_PCNHL_RTRG_OFF;		// restart disable
		ctrl_cmn.out_mask				= D_DD_PWM_PCNHL_PGMS_L;		// output level mask
		ctrl_cmn.edge					= D_DD_PWM_PCNHL_EGS_RISE;		// edge enable select
		ctrl_cmn.int_enable				= D_DD_PWM_PCNHL_IREN_OFF;		// interrupt disable
		ctrl_cmn.trigger				= D_DD_PWM_PCNHL_IRS_TRG;		// interrupt factor
		ctrl_cmn.polarity				= D_DD_PWM_PCNHL_OSEL_NORMAL;	// output polarity
		ctrl_cmn.in_trigger				= D_DD_PWM_GCN_TSEL_EM0;		// trigger input factor
		ctrl_cmn.tim_e_enable			= D_DD_PWM_GCN_TMEEN_ON;		// Timer E disable
		ctrl_cmn.ch_assign				= D_DD_PWM_GCN_CHAS_ABCD;		// channel assignment
		ctrl_cmn.pCallBack				= (VP_CALLBACK_PWM)sample_func;	// callback function pointer

		ctrl_e.hold[0]					= 1;
		ctrl_e.hold[1]					= 1;
		ctrl_e.hold[2]					= 1;
		ctrl_e.hold[3]					= 1;
		ctrl_e.force_val[0]				= 0;
		ctrl_e.force_val[1]				= 1;
		ctrl_e.force_val[2]				= 1;
		ctrl_e.force_val[3]				= 0;
		ctrl_e.force[0]					= 1;
		ctrl_e.force[1]					= 1;
		ctrl_e.force[2]					= 1;
		ctrl_e.force[3]					= 1;
		ctrl_e.empty_dma_req			= D_DD_PWM_PCHSET_RQTIM_4;		// Inerrupt request timing for DMA
		ctrl_e.undr_run_enable			= D_DD_PWM_PCHSET_UDEN_OFF;		// speed down setting under run error
		ctrl_e.undr_run_int_err_enable	= D_DD_PWM_PINTCT_UIE_OFF;		// interrupt enable under run error
		ctrl_e.force_stop_int_enable	= D_DD_PWM_PINTCT_FIE_OFF;		// interrupt enable for force stop
		ctrl_e.auto_stop_int_enable		= D_DD_PWM_PINTCT_AIE_OFF;		// interrupt enable for auto stop
		ctrl_e.dma_int_enable			= D_DD_PWM_PINTCT_DRQE_OFF;		// interrupt enable for dma

		ret = Dd_PWM_Calculate(cyc_time, &ctrl_cmn.clk_sel, &cyc_num);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM cycle calculate !!\n"));
			return;
		}

		cyc_num[0]		= cyc_num;				// cycle : 60msec
		duty_num[0]		= cyc_num >> 1;			// duty  : 50%	(30msec)
		pulse_num[0]	= 30;					// 30 times
		link[0]			= 1;					// continue

		cyc_num[1]		= cyc_num / 6;			// cycle : 10msec
		duty_num[1]		= (cyc_num / 6) >> 1;	// duty  : 50%	(5msec)
		pulse_num[1]	= 15;					// 15 times
		link[1]			= 0;					// last

		ret = Dd_PWM_Ctrl_Common(ch, &ctrl_cmn);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Ctrl !!\n"));
		}

		ret = Dd_PWM_Ctrl_Timer_E(ch, &ctrl_e);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Timer E Ctrl !!\n"));
		}

		ret = Dd_PWM_Start(ch);
		Ddim_Print(("PWM Start !!\n"));
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Start !!\n"));
		}

	//--------------------------------------------------------
	// It is a termination as follows.
	// Please execute it after completing processing.

		ret = Dd_PWM_Stop(ch);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Stop !!\n"));
		}

		ret = Dd_PWM_Close_Timer_E(ch);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Close !!\n"));
		}
	}
@endcode

	<br>
	<br>
	@section PWM_Sample_Oneshot			Sample of Oneshot utility

@code
	void PWM_Sample_Oneshot()
	{
		INT32						ret;
		T_DD_PWM_CTRL_CMN			ctrl_cmn;
		T_DD_PWM_CTRL_TIMER_ABCD	ctrl_abcd;
		UINT32						ch			= 2;		// used channel number
		USHORT						cyc_time	= 60000;	// wave cycle(usec)
		USHORT						duty_time	= 30000;	// duty time(usec)

		Dd_PWM_Init();			// After power on and the reset, this is executed only once.

		ret = Dd_PWM_Open(ch, D_DDIM_USER_SEM_WAIT_POL);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Open !!\n"));
			return;
		}

		ctrl_cmn.restart_trig	= D_DD_PWM_PCNHL_RTRG_OFF;		// restart enable
		ctrl_cmn.out_mask		= D_DD_PWM_PCNHL_PGMS_L;		// output level mask
		ctrl_cmn.edge			= D_DD_PWM_PCNHL_EGS_RISE;		// edge enable select
		ctrl_cmn.int_enable		= D_DD_PWM_PCNHL_IREN_OFF;		// interrupt enable
		ctrl_cmn.trigger		= D_DD_PWM_PCNHL_IRS_TRG;		// interrupt factor
		ctrl_cmn.polarity		= D_DD_PWM_PCNHL_OSEL_NORMAL;	// output polarity
		ctrl_cmn.in_trigger		= ch;							// trigger input factor
		ctrl_cmn.tim_e_enable	= D_DD_PWM_GCN_TMEEN_OFF;		// Timer E disable
		ctrl_cmn.ch_assign		= D_DD_PWM_GCN_CHAS_ABCD;		// channel assignment
		ctrl_cmn.pCallBack		= (VP_CALLBACK_PWM)sample_func;	// callback function pointer

		ctrl_abcd.cyc_time		= cyc_time;						// wave cycle(usec)

		ret = Dd_PWM_Write_Oneshot(ch, duty_time, &ctrl_cmn, &ctrl_abcd);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Set onshot !!\n"));
			return;
		}

		ret = Dd_PWM_Start(ch);
		Ddim_Print(("PWM Start !!\n"));
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Start !!\n"));
		}
	}
@endcode

	<br>
	<br>
	@section PWM_Sample_Step			Sample of Stepping motor utility

@code
	// PWM stepping data
	static T_DD_PWM_TIMER_E_STEPS_DATA gSample_DD_PWM_Step_Data[16] = {
	//	cyc_num,	duty_num,				pulse_num(link)
		50625,		(50625 / 8 * 6),		512 + (1 << 15),
		50625,		(50625 / 8 * 6),		512 + (1 << 15),
		50625,		(50625 / 8 * 6),		512 + (1 << 15),
		50625,		(50625 / 8 * 6),		512 + (1 << 15),
		50625,		(50625 / 8 * 4),		512 + (1 << 15),
		50625,		(50625 / 8 * 4),		512 + (1 << 15),
		50625,		(50625 / 8 * 4),		512 + (1 << 15),
		50625,		(50625 / 8 * 4),		512 + (1 << 15),
		50625,		(50625 / 8 * 3),		512 + (1 << 15),
		50625,		(50625 / 8 * 3),		512 + (1 << 15),
		50625,		(50625 / 8 * 3),		512 + (1 << 15),
		50625,		(50625 / 8 * 3),		512 + (1 << 15),
		50625,		(50625 / 8 * 2),		512 + (1 << 15),
		50625,		(50625 / 8 * 2),		512 + (1 << 15),
		50625,		(50625 / 8 * 2),		512 + (1 << 15),
		50625,		(50625 / 8 * 2),		512 + (0 << 15)
	};
	static T_DD_PWM_TIMER_E_STEPS_DATA* gSample_DD_PWM_Step_Data_p = gSample_DD_PWM_Step_Data;

	// PWM Under run data
	static T_DD_PWM_TIMER_E_STEPS_DATA gSample_PWM_Under_Run_Data[4] = {
	//	cyc_num,	duty_num,				pulse_num(link)
		52000,		(52000 / 8 * 2),		512 + (1 << 15),
		54000,		(54000 / 8 * 3),		512 + (1 << 15),
		56000,		(56000 / 8 * 4),		512 + (1 << 15),
		58000,		(58000 / 8 * 6),		512 + (0 << 15)
	};
	static T_DD_PWM_TIMER_E_STEPS_DATA* gSample_PWM_Under_Run_Data_p = gSample_PWM_Under_Run_Data;


	void PWM_Sample_Step()
	{
		INT32			ret;
		T_DD_PWM_STEP	pwm_stp;
		UINT32			ch	= 4;		// used channel number

		Dd_PWM_Init();					// After power on and the reset, this is executed only once.

		ret = Dd_PWM_Open_Timer_E(ch, D_DDIM_USER_SEM_WAIT_POL);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Open E !!\n"));
			return;
		}

		pwm_stp.clk_sel				= D_DD_PWM_PCNHL_CKS_DIV_16;
		pwm_stp.polarity			= D_DD_PWM_PCNHL_OSEL_NORMAL;
		pwm_stp.ch_assign			= D_DD_PWM_GCN_CHAS_ABCD;
		pwm_stp.hold[0]				= 1;
		pwm_stp.hold[1]				= 1;
		pwm_stp.hold[2]				= 1;
		pwm_stp.hold[3]				= 1;
		pwm_stp.force[0]			= 0;
		pwm_stp.force[1]			= 1;
		pwm_stp.force[2]			= 1;
		pwm_stp.force[3]			= 0;
		pwm_stp.step_data_num		= 16;
		pwm_stp.step_data			= gSample_DD_PWM_Step_Data_p;
		pwm_stp.undr_run_data_num	= 4;
		pwm_stp.under_run_data		= gSample_PWM_Under_Run_Data_p;
		pwm_stp.pCallBack			= (VP_CALLBACK_PWM)sample_func;

		ret = Dd_PWM_Write_Step(ch, 0, &pwm_stp);
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Set step !!\n"));
			return;
		}

		ret = Dd_PWM_Start(ch);
		Ddim_Print(("PWM Start !!\n"));
		if (ret != D_DDIM_OK){
			Ddim_Print(("Error PWM Start !!\n"));
		}
	}
@endcode
@}*/
