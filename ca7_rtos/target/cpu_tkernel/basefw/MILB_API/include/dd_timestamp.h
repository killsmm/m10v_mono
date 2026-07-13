/**
 * @file		dd_timestamp.h
 * @brief		Timestamp Generator driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_timestamp
@{
	- @ref Timestamp_start_base
	- @ref Timestamp_Sample_Basic
*/

#ifndef _DD_TIMESTAMP_H_
#define _DD_TIMESTAMP_H_

#include "driver_common.h"
#include "arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Return value of TIMESTAMP processing result
#define	D_DD_TIMESTAMP_INPUT_PARAM_ERR		(D_DD_TIMESTAMP | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error */
#define D_DD_TIMESTAMP_SEM_TIMEOUT			(D_DD_TIMESTAMP | D_DDIM_SEM_TIMEOUT)		/**< Semaphore acquisition Time Out */
#define D_DD_TIMESTAMP_SEM_NG				(D_DD_TIMESTAMP | D_DDIM_SEM_NG)			/**< Semaphore acquisition NG */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
/** Control value */
typedef struct {
	ULONG 		hdbg;			/**< Halt on Debug of TIMESTAMP CNTCR register<br>
									 <ul>
									 <li>0:has no effect
									 <li>1:halts the counter
									 </ul> */
	UINT64 		counter;		/**< Counter of TIMESTAMP CNTCVL and CNTCVU register<br>
									 64bit counter value. */

	ULONG 		frequency;		/**< Frequency of TIMESTAMP CNTFID0 register<br>
									 Value range:0x00000001 to 0xEE6B2800(4GHz) */
}T_DD_TIMESTAMP_CTRL;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Counter Control Register (CNTCR)
#define Dd_TIMESTAMP_Get_CNTCR_En			(IO_TIMESTAMP.CNTCR.bit.EN)					/**< Get Enable (0:Disable 1:Enable) */
#define Dd_TIMESTAMP_Set_CNTCR_En(val)		{IO_TIMESTAMP.CNTCR.bit.EN = (val);}		/**< Set Enable (0:Disable 1:Enable) */

#define Dd_TIMESTAMP_Get_CNTCR_HDbg			(IO_TIMESTAMP.CNTCR.bit.HDBG)				/**< Get Halt on Debug (0:has no effect 1:halts the counter) */
#define Dd_TIMESTAMP_Set_CNTCR_HDbg(val)	{IO_TIMESTAMP.CNTCR.bit.HDBG = (val);}		/**< Set Halt on Debug (0:has no effect 1:halts the counter) */

// Counter Status Register (CNTSR)
#define Dd_TIMESTAMP_Get_CNTSR_Dbgh			(IO_TIMESTAMP.CNTSR.bit.DBGH)				/**< Get Enable (0:Not Debug Halted 1:Debug Halted) */

// Current Counter Value Lower register (CNTCVL)
#define Dd_TIMESTAMP_Get_CNTCVL				(IO_TIMESTAMP.CNTCVL)						/**< Get Counter Lower (0x00000000 to 0xFFFFFFFF) */
#define Dd_TIMESTAMP_Set_CNTCVL(val)		{IO_TIMESTAMP.CNTCVL = (val);}				/**< Set Counter Lower (0x00000000 to 0xFFFFFFFF) */

// Current Counter Value Upper register (CNTCVU)
#define Dd_TIMESTAMP_Get_CNTCVU				(IO_TIMESTAMP.CNTCVU)						/**< Get Counter Upper (0x00000000 to 0xFFFFFFFF) */
#define Dd_TIMESTAMP_Set_CNTCVU(val)		{IO_TIMESTAMP.CNTCVU = (val);}				/**< Set Counter Upper (0x00000000 to 0xFFFFFFFF) */

// Base Frequency ID register (CNTFID0)
#define Dd_TIMESTAMP_Get_CNTFID0			(IO_TIMESTAMP.CNTFID0)						/**< Get Frequency (0x00000000 to 0xEE6B2800(4GHz)) */
#define Dd_TIMESTAMP_Set_CNTFID0(val)		{IO_TIMESTAMP.CNTFID0 = (val);}				/**< Set Frequency (0x00000000 to 0xEE6B2800(4GHz)) */

// Peripheral ID4 Register (PERIPID4)
#define Dd_TIMESTAMP_Get_PIDR4_Des_2		(IO_TIMESTAMP.PIDR4.bit.DES_2)				/**< Get Designer of the component (0b0100:JEDEC code) */
#define Dd_TIMESTAMP_Get_PIDR4_Size			(IO_TIMESTAMP.PIDR4.bit.SIZE)				/**< Get Designer of the component (0b0000:4KB memory) */

// Peripheral ID0 Register (PERIPID0)
#define Dd_TIMESTAMP_Get_PIDR0_Part_0		(IO_TIMESTAMP.PIDR0.bit.PART_0)				/**< Get Part number of the component (0x01:Part number) */

// Peripheral ID1 Register (PERIPID1)
#define Dd_TIMESTAMP_Get_PIDR1_Des_0		(IO_TIMESTAMP.PIDR1.bit.DES_0)				/**< Get Designer of the component (0b1011:JEDEC code) */
#define Dd_TIMESTAMP_Get_PIDR1_Part_1		(IO_TIMESTAMP.PIDR1.bit.PART_1)				/**< Get Part number of the component (0b0001:Part number) */

// Peripheral ID2 Register (PERIPID2)
#define Dd_TIMESTAMP_Get_PIDR2_Des_1		(IO_TIMESTAMP.PIDR2.bit.DES_1)				/**< Get Designer of the component (0b011:JEDEC code) */
#define Dd_TIMESTAMP_Get_PIDR2_Jedec		(IO_TIMESTAMP.PIDR2.bit.JEDEC)				/**< Get Designer ID (0b1:JEDEC-assigned) */
#define Dd_TIMESTAMP_Get_PIDR2_Revision		(IO_TIMESTAMP.PIDR2.bit.REVISION)			/**< Get Device (0b0001:r0p1) */

// Peripheral ID3 Register (PERIPID3)
#define Dd_TIMESTAMP_Get_PIDR3_Cmod			(IO_TIMESTAMP.PIDR3.bit.CMOD)				/**< Get Errata (0b0000:No errata) */
#define Dd_TIMESTAMP_Get_PIDR3_Revand		(IO_TIMESTAMP.PIDR3.bit.REVAND)				/**< Get Modify of component (0b0000:Not modified) */

// Component ID0 Register (CIDR0)
#define Dd_TIMESTAMP_Get_CIDR0_Prmbl_0		(IO_TIMESTAMP.CIDR0.bit.PRMBL_0)			/**< Get Identification code (0x0D) */

// Component ID1 Register (CIDR1)
#define Dd_TIMESTAMP_Get_CIDR1_Prmbl_1		(IO_TIMESTAMP.CIDR1.bit.PRMBL_1)			/**< Get Identification code (0b0000) */
#define Dd_TIMESTAMP_Get_CIDR1_Class		(IO_TIMESTAMP.CIDR1.bit.CLASS)				/**< Get Class of the component (0b1111:CoreSight SoC-400) */

// Component ID2 Register (CIDR2)
#define Dd_TIMESTAMP_Get_CIDR2_Prmbl_2		(IO_TIMESTAMP.CIDR2.bit.PRMBL_2)			/**< Get Identification code (0x05) */

// Component ID3 Register (CIDR3)
#define Dd_TIMESTAMP_Get_CIDR3_Prmbl_3		(IO_TIMESTAMP.CIDR3.bit.PRMBL_3)			/**< Get Identification code (0xB1) */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
Initialize all Timestamp Register.
*/
extern 	VOID	Dd_TIMESTAMP_Init(VOID);

/**
Timestamp fuction is exclusively controlled.
@param [in]		tmout					: Time of timeout
										<ul>
										<li>Positive Value
										<li>@ref D_DDIM_USER_SEM_WAIT_POL
										<li>@ref D_DDIM_USER_SEM_WAIT_FEVR
										</ul>
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_TIMESTAMP_INPUT_PARAM_ERR	: Input Parameter Error
@retval	D_DD_TIMESTAMP_SEM_NG			: Lock Error (System Using designated channel number)
@retval	D_DD_TIMESTAMP_SEM_TIMEOUT		: Semaphore acquisition Time Out
@remarks	If Timestamp is unused, exclusive control OK is sent back as (Lock) while using it.<br>
			Exclusive control NG is sent back when using it (Lock inside of present).<br>
			When timeout occurs, return "D_DD_TIMESTAMP_SEM_TIMEOUT".
@remarks	This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
			This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32	Dd_TIMESTAMP_Open(INT32 tmout);

/**
Setting of common control variable for Timestamp Counter.
@param [in]		timestamp_ctrl			: Tiemstamp Control value. See @ref T_DD_TIMESTAMP_CTRL
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_TIMESTAMP_INPUT_PARAM_ERR	: Input Parameter Error
*/
extern	INT32	Dd_TIMESTAMP_Ctrl(T_DD_TIMESTAMP_CTRL* timestamp_ctrl);

/**
Timestamp is started.
*/
extern	VOID	Dd_TIMESTAMP_Start(VOID);

/**
Timestamp is stopped.
*/
extern	VOID	Dd_TIMESTAMP_Stop(VOID);

/**
Timestamp is released.
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_TIMESTAMP_SEM_NG			: UnLock Error
@remarks	This API uses DDIM_User_Sig_Sem().
*/
extern	INT32	Dd_TIMESTAMP_Close(VOID);

/**
Get value of TIMESTAMP CNTCVL and CNTCVU register.
@param [out]	timestamp_counter		: Timestamp Counter
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_TIMESTAMP_INPUT_PARAM_ERR	: Input Parameter Error
*/
extern	INT32	Dd_TIMESTAMP_Get_Counter(UINT64* timestamp_counter);

/**
Get value of Timestamp Control value.
@param [out]	timestamp_ctrl			: Timestamp Control value. See @ref T_DD_TIMESTAMP_CTRL
@retval	D_DDIM_OK						: Noraml end
@retval	D_DD_TIMESTAMP_INPUT_PARAM_ERR	: Input Parameter Error
*/
extern	INT32	Dd_TIMESTAMP_Get_Control(T_DD_TIMESTAMP_CTRL* timestamp_ctrl);

/**
Set value of TIMESTAMP CNTCVL and CNTCVU register.
@param [in]		timestamp_counter		: Timestamp Counter
@retval	D_DDIM_OK						: Normal end
*/
extern	INT32	Dd_TIMESTAMP_Set_Counter(UINT64 timestamp_counter);

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/**
@name Utility Functions
@{*/
/**
The operation condition of Timestamp is set according to designate time parameter.
@param [in]	usec						: Designated time(micro second)
@param [in]	frequency					: Frequency(0x00000001 to 0xEE6B2800(4GHz))
@retval	D_DDIM_OK						: Noraml end
@retval	D_DD_TIMESTAMP_INPUT_PARAM_ERR		: Input Parameter Error
*/
extern	INT32	Dd_TIMESTAMP_Set_Timer(ULONG usec, ULONG frequency);

/*@}*/
#endif	// CO_DDIM_UTILITY_USE

#ifdef __cplusplus
}
#endif

#endif	// _DD_TIMESTAMP_H_

/*@}*/

/**
@weakgroup dd_timestamp
@{
	@section Timestamp_start_base			Basic sequence of Timestamp
	@image html dd_timestamp_base.png
	<br>
	@section Timestamp_Sample_Basic			Sample of Timestamp (Basic sequence)
	The example of executing Timestamp counter start to end is as follows.<br>
	<b>Please call these APIs from either of one of multiple cores.<br>
	These are not under exclusive control between the cores.</b>
	@code
	INT32	ret;									// Return value
	T_DD_TIMESTAMP_CTRL	timestamp_ctrl;
	
	timestamp_ctrl.hdbg = 0;						// has no effect
	timestamp_ctrl.counter = 0x0000000000000000;	// Counter
	timestamp_ctrl.frequency = 0xEE6B2800;			// 4GHz
	
	Dd_TIMESTAMP_Init();
	
	ret = Dd_TIMESTAMP_Open(D_DDIM_USER_SEM_WAIT_POL);
	if (ret != 0) {
		// ERROR
		return ;
	}
	
	ret = Dd_TIMESTAMP_Ctrl(&timestamp_ctrl);
	if (ret != 0) {
		// ERROR
		return ;
	}
	
	Dd_TIMESTAMP_Start();
	
	// Stop sequence
	Dd_TIMESTAMP_Stop();
	
	ret = Dd_TIMESTAMP_Close();
	if (ret != 0) {
		// ERROR
	}
    @endcode
@}*/
