/**
 * @file		dd_tmr64.h
 * @brief		peripheral 32 bit reload timer driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_tmr64
@{
	@code
	INT32 Timer64_Sample_Code(VOID)
	{
		T_DD_TMR64_CTRL tmr64_ctrl;

		tmr64_ctrl.csr1.bit.trg  = 0;	// Start at Dd_TMR64_Start()
		tmr64_ctrl.csr1.bit.cnte = 0;	// Enalbe at Dd_TMR64_Start()
		tmr64_ctrl.csr1.bit.csl  = 2;	// 32div
		tmr64_ctrl.csr2.bit.cvt  = 0;	// Counter load trigger off
		tmr64_ctrl.csr2.bit.tsen = 0;	// Enable at Dd_TMR64_Start()

		ret = Dd_TMR64_Open(3000);
		if (ret != D_DDIM_OK) {
			// ERROR
			return;
		}
		ret = Dd_TMR64_Ctrl(&tmr64_ctrl);
		if (ret != D_DDIM_OK) {
			// ERROR
			Dd_TMR64_Close();
			return;
		}
		ret = Dd_TMR64_Start(TRUE);		// Start count and enable time stamp
		if (ret != D_DDIM_OK) {
			// ERROR
			Dd_TMR64_Close();
			return;
		}

		// To get timer value at appropriate timing by following any API.
		//   Dd_TMR64_Get_Gyro_TimeStamp()
		//   Dd_TMR64_Get_FrameStart_TimeStamp()
		//   Dd_TMR64_Get_FrameEnd_TimeStamp()
		//   Dd_TMR64_Get_Counter()

		ret = Dd_TMR64_Stop();
		if (ret != D_DDIM_OK) {
			// ERROR
			return;
		}
		ret = Dd_TMR64_Close();
		if (ret != D_DDIM_OK) {
			// ERROR
			return;
		}
	}
	@endcode

*/

#ifndef _DD_TMR64_H_
#define _DD_TMR64_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* Return value of 64 bit reload timer processing result */
#define D_DD_TMR64_INPUT_PARAM_ERR		(D_DD_TMR64 | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error */
#define D_DD_TMR64_SEM_TIMEOUT			(D_DD_TMR64 | D_DDIM_SEM_TIMEOUT)		/**< Semaphore acquisition Time Out */
#define D_DD_TMR64_SEM_NG				(D_DD_TMR64 | D_DDIM_SEM_NG)			/**< Semaphore acquisition NG */
#define D_DD_TMR64_STATUS_ABNORMAL		(D_DD_TMR64 | D_DDIM_STATUS_ABNORMAL)	/**< Abnormal Status */
#define D_DD_TMR64_DMA_SS_ERROR			(D_DD_TMR64 | D_DDIM_BUS_ERROR)			/**< DMA stop status error  */

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
/** CSR1 of free run timer */
typedef union {
	ULONG		word;				/**< for ULONG access */
	struct {
		ULONG	trg			:1;		/**< Count start trriger (Read value is 0)		*/
		ULONG	cnte		:1;		/**< Timer counter enable						*/
		ULONG				:8;		/**< Reserved									*/
		ULONG	csl			:2;		/**< Count clock select (0:1/2, 1:1/8, 2:1/32)	*/
		ULONG				:4;		/**< Reserved									*/
		ULONG				:16;	/**< Reserved									*/
    } bit;							/**< for bit access								*/
} U_DD_TMR64_CSR1;

/** CSR2 of free run timer */
typedef union {
	ULONG		word;				/**< for ULONG access */
	struct {
		ULONG	cvt			:1;		/**< Counter load trigger(Read value is 0)	*/
		ULONG				:15;	/**< Reserved								*/
		ULONG	tsen		:1;		/**< Time stamp enable				 		*/
		ULONG				:15;	/**< Reserved								*/
    } bit;							/**< for bit access							*/
} U_DD_TMR64_CSR2;

/** Control information */
typedef struct {
	U_DD_TMR64_CSR1		csr1;		/**< Control status    TMCSR64_1 		*/
	U_DD_TMR64_CSR2		csr2;		/**< Control status    TMCSR64_2 		*/
} T_DD_TMR64_CTRL;

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
The 64bit Timer is exclusively controlled.<br>
If a pertinent 64bit Timer is unused, exclusive control OK is sent back as (Lock) while using it.<br>
Exclusive control NG is sent back when using it (Lock inside of present).
When timeout occurs, return "D_DD_TMR64_SEM_TIMEOUT".

@param [in]		tmout		Time of timeout<br>
							<ul><li>Positive Value(Time of timeout)
								<li>@ref D_DDIM_USER_SEM_WAIT_POL
								<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval D_DDIM_OK					OK
@retval D_DD_TMR64_INPUT_PARAM_ERR	Input Parameter Error
@retval D_DD_TMR64_SEM_NG			Lock Error
@retval D_DD_TMR64_SEM_TIMEOUT		Semaphore acquisition Time Out
@remarks This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
		 This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32	Dd_TMR64_Open(INT32 tmout);

/**
64bit Timer is released.

@retval D_DDIM_OK					OK
@retval D_DD_TMR64_SEM_NG			Lock Error
@remarks This API uses DDIM_User_Sig_Sem().
*/
extern	INT32	Dd_TMR64_Close(VOID);

/**
The operation condition of Timer is set.

@param [in]    *tmr_ctrl	Timer64 Control Information
@retval D_DDIM_OK					OK
@retval D_DD_TMR64_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR64_Ctrl(T_DD_TMR64_CTRL const* const tmr_ctrl);

/**
Value of T_DD_TMR64_CTRL is gotten.

@param [out]   *tmr_ctrl	Timer64 Control Information
@retval D_DDIM_OK						OK
@retval D_DD_TMR64_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR64_Get_Ctrl(T_DD_TMR64_CTRL* const tmr_ctrl);

/**
64bit Timer is started.

@param [in]    time_stamp_enable	Time stamp enable
@retval D_DDIM_OK					OK
*/
extern	INT32	Dd_TMR64_Start(BOOL time_stamp_enable);

/**
64bit Timer is stopped.

@retval D_DDIM_OK					OK
*/
extern	INT32	Dd_TMR64_Stop(VOID);

/**
64bit Timer Gyro Time Stamp value is gotten.

@param [out]   time_stamp		Time stamp value of Gyro
@retval D_DDIM_OK						OK
@retval D_DD_TMR64_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR64_Get_Gyro_TimeStamp(ULONG* const time_stamp);

/**
64bit Timer Frame Start counter value is gotten.

@param [in]    sensor_no		Sensor number
@param [in]    lotation_count	Lotation count
@param [out]   time_stamp		Time stamp value of Frame Start
@retval D_DDIM_OK					OK
@retval D_DD_TMR64_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR64_Get_FrameStart_TimeStamp(UINT32 sensor_no, UINT32 lotation_count, UINT64* const time_stamp);


/**
64bit Timer Frame End counter value is gotten.

@param [in]    sensor_no		Sensor number
@param [in]    lotation_count	Lotation count
@param [out]   time_stamp		Time stamp value of Frame End
@retval D_DDIM_OK					OK
@retval D_DD_TMR64_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR64_Get_FrameEnd_TimeStamp(UINT32 sensor_no, UINT32 lotation_count, UINT64* const time_stamp);

/**
64bit Timer counter value is gotten.

@param [out]   counter	Current counter value
@retval D_DDIM_OK					OK
@retval D_DD_TMR64_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_TMR64_Get_Counter(UINT64* const counter);

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/**
Enable/Disable 64bit Time Stamp.

@param [in]    time_stamp_enable	Time stamp enable
@retval D_DDIM_OK					OK
*/
extern	INT32	Dd_TMR64_Time_Stamp_Enable(BOOL time_stamp_enable);

//---------------------------- colabo section ----------------------------
// Nothing Special

#endif	// CO_DDIM_UTILITY_USE

#ifdef __cplusplus
}
#endif

#endif	// _DD_TMR64_H_

/*@}*/
