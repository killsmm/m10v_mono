/**
 * @file		dd_tmr32.h
 * @brief		peripheral 32 bit reload timer driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_tmr32
@{
	- @ref Timer32_start_base

	<hr>
	@section Timer32_start_base		Basic sequence of 32 bit Timer
	@image html dd_tmr32_base.png

	@code
	INT32 Timer32_Sample_Code(VOID)
	{
		UINT32		ch = 1;
		INT32		tmout;
		T_DD_TMR32_CTRL 	tmr32_ctrl;
		
		tmr32_ctrl.csr.bit.rels       = 0;
		tmr32_ctrl.csr.bit.csl        = 2;		// 32div
		tmr32_ctrl.csr.bit.mod        = 0;
		tmr32_ctrl.csr.bit.outl       = 0;
		tmr32_ctrl.csr.bit.reld       = 1;		// Reload
		tmr32_ctrl.csr.bit.inte       = 1;		// Interrupt enable
		tmr32_ctrl.csr.bit.uf_fixed1  = 0;
		tmr32_ctrl.csr.bit.cnte       = 1;		// Timer counter enable
		tmr32_ctrl.csr.bit.trg_fixed0 = 0;
		
		tmr32_ctrl.rlr1         = (1687500 - 1);		// Timer Count (1sec)
		tmr32_ctrl.rlr2         = 0;
		tmr32_ctrl.soft_counter = 1;					// Soft counter
		tmr32_ctrl.pCallBack    = Sample_Code_CB;		// User's Callback Function

		ret = Dd_TMR32_Open(ch, tmout);
		if (ret != D_DDIM_OK) {
			// ERROR
			return ;
		}
		ret = Dd_TMR32_Ctrl(ch, &tmr32_ctrl);
		if (ret != D_DDIM_OK) {
			// ERROR
			Dd_TMR32_Close(ch, tmout);
			return ;
		}
		ret = Dd_TMR32_Start(ch);
		if (ret != D_DDIM_OK) {
			// ERROR
			Dd_TMR32_Close(ch, tmout);
			return ;
		}
	}
	@endcode

*/

#ifndef _DD_TMR32_H_
#define _DD_TMR32_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* Return value of 32 bit reload timer processing result */
#define D_DD_TMR32_INPUT_PARAM_ERR		(D_DD_TMR32 | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error */
#define D_DD_TMR32_SEM_TIMEOUT			(D_DD_TMR32 | D_DDIM_SEM_TIMEOUT)		/**< Semaphore acquisition Time Out */
#define D_DD_TMR32_SEM_NG				(D_DD_TMR32 | D_DDIM_SEM_NG)			/**< Semaphore acquisition NG */

/* Channel Number max */
#define D_DD_TMR32_CH_NUM_MAX			(14)		/**< Channel max number */

/* External Trriger Max Channel Number */
#define D_DD_TMR32_EXT_TRG_CH_NUM_MAX	(6)			/**< Channel max number that can use external trigger */

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/** CSR of reload timer */
typedef union {
	ULONG		word;			/**< for ULONG access */
	struct {
		ULONG	trg_fixed0	:1;	/**< TRG is "0" fiexed ( 1 )			*/
		ULONG	cnte		:1;	/**< Timer counter Enable ( 0 or 1 )	*/
		ULONG	uf_fixed1	:1;	/**< UF is "1" fiexed ( 1 )				*/
		ULONG	inte		:1;	/**< Interrupt Enable ( 0 or 1 )		*/
		ULONG	reld		:1;	/**< Reload Enable ( 0 or 1 )			*/
		ULONG	outl		:1;	/**< Out Level ( 0 or 1 )				*/
		ULONG				:1;	/**< Reserved							*/
		ULONG	mod			:3;	/**< Mode select ( 0 to 7 )				*/
		ULONG	csl			:2;	/**< Count clock select ( 0 to 3 )		*/
		ULONG	rels		:1;	/**< Reload register select ( 0 or 1 )	*/
		ULONG				:3;	/**< Reserved							*/
		ULONG				:16;/**< Reserved							*/
    } bit;						/**< for bit access						*/
} U_DD_TMR32_CSR;

/** Control information */
typedef struct {
	U_DD_TMR32_CSR		csr;			/**< Control status    TMCSR 								*/
	ULONG				rlr1;			/**< Reload register1  TMRLR1 ( 0x00000000 tor 0xFFFFFFFF )	*/
	ULONG				rlr2;			/**< Reload register2  TMRLR2 ( 0x00000000 tor 0xFFFFFFFF )	*/
	ULONG				soft_counter;	/**< soft counter ( 0x00000001 tor 0xFFFFFFFF )				*/
	VP_CALLBACK			pcallback;		/**< Call Back function pointer	*/
} T_DD_TMR32_CTRL;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Grobal Data															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
#ifdef __cplusplus
extern "C" {
#endif

/**
The 32bit Reload Timer channel is exclusively controlled.<br>
If a pertinent channel of 32bit Reload Timer is unused, exclusive control OK is sent back as (Lock) while using it.<br>
Exclusive control NG is sent back when using it (Lock inside of present).
When timeout occurs, return "D_DD_TMR32_SEM_TIMEOUT".

@param [in] 	ch	Channel number ( 0 to 13 )
@param [in]		tmout		Time of timeout<br>
							<ul><li>Positive Value(Time of timeout)
								<li>@ref D_DDIM_USER_SEM_WAIT_POL
								<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
@retval D_DD_TMR32_SEM_NG				Lock Error (System Using designated channel number)
@retval D_DD_TMR32_SEM_TIMEOUT		Semaphore acquisition Time Out
@remarks This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
		 This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32	Dd_TMR32_Open(UINT32 ch, INT32 tmout);

/**
32bit Reload Timer(Channel) is released.

@param [in] 	ch	Channel number ( 0 to 13 )
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
@remarks This API uses DDIM_User_Sig_Sem().
*/
extern	INT32	Dd_TMR32_Close(UINT32 ch);

/**
The operation condition of 32bit Reload Timer (Channel) is set.<br>
Please set the callback function when the interruption is generated to "pCallBack".<br>
"soft_counter" is the one that how many times "load_value" is repeated is specified.<br>
When the number of "soft_counter" is repeated, the function registered in "pCallBack" is called. 

@param [in] 	ch			Channel number ( 0 to 13 )
@param [in]    *tmr_ctrl	Timer32 Control Information
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_Ctrl(UINT32 ch, T_DD_TMR32_CTRL const* const tmr_ctrl);

/**
The value of T_DD_TMR32_CTRL that corresponds at the time specified by "usec" is calculated.<br>
The unit of "usec" is a micro second.<br>
If it doesn't calculate, Input Parameter Error is returned.<br>

@param [in] 	usec			The designated time(micro second)
@param [out]   *tmr_ctrl		Timer32 Control Information
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_Calculate(ULONG usec, T_DD_TMR32_CTRL* const tmr_ctrl);


/**
The value of T_DD_TMR32_CTRL that corresponds.<br>

@param [in] 	pwm_ch			pwm_ch
@param [in] 	mode			0:for 60fps, 1:for 240fps
@param [out]   *tmr_ctrl		Timer32 Control Information
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_Calculate_Pwm_Sync(UINT32 pwm_ch, UCHAR mode, T_DD_TMR32_CTRL* const tmr32_ctrl);


/**
32bit Reload Timer(Channel) is started.

@param [in] 	ch	Channel number ( 0 to 13 )
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_Start(UINT32 ch);

/**
32bit Reload Timer(Channel) 32bit Reload Timer(Channel) wait for external trigger.

@param [in] 	ch	Channel number. value range ( 0 to 5 )
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_External_Start(UINT32 ch);

/**
32bit Reload Timer(Channel) is stopped.

@param [in] 	ch	Channel number ( 0 to 13 )
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_Stop(UINT32 ch);

/**
32bit Reload Timer(Channel) is paused.

@param [in] 	ch	Channel number ( 0 to 13 )
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_Pause(UINT32 ch);

/**
Dual Input Timer counter value is gotten.

@param [in] 	ch		Channel number ( 0 to 13 )
@param [out]   *h_count	Pointer of TimerValue
@param [out]   *s_count	Pointer of Soft Counter
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_Get_Counter(UINT32 ch, ULONG* const h_count, ULONG* const s_count);

/**
Value of T_DD_TMR32_CTRL is gotten.

@param [in] 	ch			Channel number ( 0 to 13 )
@param [out]   *tmr_ctrl	Timer32 Control Information
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
@remarks	Because TMRLR1 and TMRLR2 are the register of Write Only, the value cannot be acquired. 
*/
extern	INT32	Dd_TMR32_Get_Ctrl(UINT32 ch, T_DD_TMR32_CTRL* const tmr_ctrl);

/**
The value of soft_counter is set.

@param [in] 	ch				Channel number ( 0 to 13 )
@param [in]		soft_counter	Soft counter ( 0x00000001 to 0xFFFFFFFF )
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_Set_Softcounter(UINT32 ch, ULONG soft_counter);

/**
The soft count value is gotten.

@param [in] 	ch		Channel number ( 0 to 13 )
@retval soft_counter	Soft counter
*/
extern	ULONG	Dd_TMR32_Get_Softcounter(UINT32 ch);

/**
The value of reload_flg is set.<br>
If the reload_flg is "0", timer processing is not retried(=one shot).<br>
And if the reload_flg is "1", timer processing is retried(counter is reloaded when timer processing is completed).

@param [in]		ch		Channel number ( 0 to 13 )
@param [in]		flg		reload flag ( 0 to 1 )
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_Set_Reload_Flg(UINT32 ch, UCHAR flg);

/**
The reload flag is gotten.

@param [in]		ch		Channel number ( 0 to 13 )
@retval gTimer32_Set_Reload_flg[ch]	The reload flag of designated ch
*/
extern	INT32	Dd_TMR32_Get_Reload_Flg(UINT32 ch);

/**
The interruption flag is cleared, and the CallBack function for 32bit Reload Timer is called.<br>
(Only an interruption clear flag of CallBack when it is unregistered.)<br>
The counter value is calculated according to specified timing.

@param [in] 	ch		Channel number ( 0 to 13 )
*/
extern	VOID	Dd_TMR32_Int_Handler(UINT32 ch);

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/** @name Utility Functions
@{*/
/**
The operation condition of Reload Timer (Channel) is set according to designate time parameter.<br>
Please set the callback function when the interruption is generated to pCallBack.<br>
If the reload_flg is "0", timer processing is not retried(= one shot).<br>
And if the reload_flg is "1", timer processing is retried (counter is reloaded when timer processing is completed).

@param [in] 	ch				Channel number ( 0 to 13 )
@param [in]		usec			Designated time(micro second)
@param [in]	  	pCallBack		Call Back function pointer
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_SetTimer(UINT32 ch, ULONG usec, VP_CALLBACK pCallBack);

/**
The operation condition of Reload Timer (Channel) is set according to designate time parameter.<br>
Please set the callback function when the interruption is generated to pCallBack.<br>
If the reload_flg is "0", timer processing is not retried(= one shot).<br>
And if the reload_flg is "1", timer processing is retried (counter is reloaded when timer processing is completed).

@param [in] 	reload_ch		Channel number ( 0 to 13 )
@param [in] 	pwm_ch			Channel number ( 4 or 8 )
@param [in]	  	mode			0:for 60fps, 1:for 240fps
@param [in]	  	pCallBack		Call Back function pointer
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_SetTimer_pwm(UINT32 reload_ch, UINT32 pwm_ch, UCHAR mode, VP_CALLBACK pcallback);


/**
Designated Channel of 32bit Reload Timer is started for reload.
@param [in]		ch			Channel number ( 0 to 13 )
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_StartReload(UINT32 ch);

/**
Designated Channel of 32bit Reload Timer is started for oneshot.

@param [in]		ch			Channel number ( 0 to 13 )
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_StartOneshot(UINT32 ch);

/**
The out_level(OUTL) is set.
@param [in]		out_level		Out level ( 0 to 1 )
@retval D_DDIM_OK						OK
@retval D_DD_TMR32_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR32_Set_Outlevel(UINT32 out_level);


/*@}*/
/*@}*/

/************************************************************************/
/* Utility																*/
/************************************************************************/
/** @weakgroup dd_tmr32
@{
	<hr>
	Utility Functions <br>
	- @ref Timer32_start_utility_OneShot <br>
	- @ref Timer32_start_utility_Reload  <br>

	<hr>
	@section Timer32_start_utility_OneShot	Sequence of the One-Shot of 32 bit Timer. (Utility)
	@image html dd_tmr32_utility_OneShot.png

	@section Timer32_start_utility_Reload	Sequence of the Reload of 32 bit Timer. (Utility)
	@image html dd_tmr32_utility_Reload.png

	@code
	INT32 Timer32_Utility_Reload_Sample_Code(VOID)
	{
		UINT32		ch;
		INT32		tmout;
		ULONG		time;
		VP_CALLBACK	call_back;

		// Parameter Setting
		ch        = 1;
		time      = 1000000;			// 1 sec
		call_back = Sample_Code_CB;		// User's Callback Function

		ret = Dd_TMR32_Open(ch, tmout);
		if (ret != D_DDIM_OK) {
			// ERROR
			return ;
		}
		ret = Dd_TMR32_SetTimer(ch, time, call_back);
		if (ret != D_DDIM_OK) {
			// ERROR
			Dd_TMR32_Close(ch);
			return ;
		}
		ret = Dd_TMR32_StartReload(ch);
		if (ret != D_DDIM_OK) {
			// ERROR
			Dd_TMR32_Close(ch);
			return ;
		}
	}

	INT32 Timer32_Utility_OneShot_Sample_Code(VOID)
	{
		UINT32		ch;
		INT32		tmout;
		ULONG		time;
		VP_CALLBACK	call_back;

		// Parameter Setting
		ch        = 1;
		time      = 1000000;	// 1 sec
		call_back = 0;			// User's Callback Function None

		ret = Dd_TMR32_Open(ch, tmout);
		if (ret != D_DDIM_OK) {
			// ERROR
			return ;
		}
		ret = Dd_TMR32_SetTimer(ch, time, call_back);
		if (ret != D_DDIM_OK) {
			// ERROR
			Dd_TMR32_Close(ch);
			return ;
		}
		ret = Dd_TMR32_StartOneshot(ch);
		if (ret != D_DDIM_OK) {
			// ERROR
			Dd_TMR32_Close(ch);
			return ;
		}
	}
	@endcode
*/
/*@}*/

//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

#ifdef __cplusplus
}
#endif


#endif
