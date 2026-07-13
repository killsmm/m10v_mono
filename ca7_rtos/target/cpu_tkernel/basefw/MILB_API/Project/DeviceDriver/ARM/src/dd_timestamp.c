/**
 * @file		dd_timestamp.c
 * @brief		Timestamp Generator driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dd_timestamp.h"
#include "dd_arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_DD_TIMESTAMP_COUNTER_INIT		(0x00000000)			/**< Initialize TIMESTAMP Counter */
#define D_DD_TIMESTAMP_FREQUENCY_INIT	(0x00000000)			/**< Initialize TIMESTAMP Frequency */
#define D_DD_TIMESTAMP_FREQUENCY_MAX	(0xEE6B2800)			/**< Max TIMESTAMP Frequency */

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
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/**
 * @brief  	Initialize all Timestamp Register.
 */
VOID Dd_TIMESTAMP_Init(VOID)
{
	Dd_TIMESTAMP_Set_CNTCR_En(0);
	Dd_TIMESTAMP_Set_CNTCR_HDbg(0);
	Dd_TIMESTAMP_Set_CNTCVL(D_DD_TIMESTAMP_COUNTER_INIT);
	Dd_TIMESTAMP_Set_CNTCVU(D_DD_TIMESTAMP_COUNTER_INIT);
	Dd_TIMESTAMP_Set_CNTFID0(D_DD_TIMESTAMP_FREQUENCY_INIT);
}

/**
 * @brief	Timestamp fuction is exclusively controlled.
 * @param	INT32 tmout
 * @return	INT32 D_DDIM_OK/D_DD_TIMESTAMP_SEM_NG/D_DD_TIMESTAMP_INPUT_PARAM_ERR/D_DD_TIMESTAMP_SEM_TIMEOUT
 */
INT32 Dd_TIMESTAMP_Open(INT32 tmout)
{
	DDIM_USER_ER	ercd;
	DDIM_USER_ID	sid;
	
#ifdef CO_PARAM_CHECK
	if( tmout < D_DDIM_USER_SEM_WAIT_FEVR ){
		Ddim_Assertion(("Dd_TIMESTAMP_Open: input param error. tmout = %d\n", tmout));
		return D_DD_TIMESTAMP_INPUT_PARAM_ERR;
	}
#endif
	sid = SID_DD_TIMESTAMP;
	
	if( D_DDIM_USER_SEM_WAIT_POL == tmout ){
		ercd = DDIM_User_Pol_Sem(sid);							// pol_sem()
	}
	else{
		ercd = DDIM_User_Twai_Sem(sid, (DDIM_USER_TMO)tmout);	// twai_sem()
	}
	
	if( D_DDIM_USER_E_OK != ercd ){
		if( D_DDIM_USER_E_TMOUT == ercd ){
			return D_DD_TIMESTAMP_SEM_TIMEOUT;
		}
		return D_DD_TIMESTAMP_SEM_NG;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief	Operation condition of Timestamp is set.
 * @return	INT32 D_DDIM_OK/D_DD_TIMESTAMP_INPUT_PARAM_ERR
 */
INT32 Dd_TIMESTAMP_Ctrl(T_DD_TIMESTAMP_CTRL* timestamp_ctrl)
{
#ifdef CO_PARAM_CHECK
	if( timestamp_ctrl == NULL ){
		Ddim_Assertion(("Dd_TIMESTAMP_Ctrl: input param error. [*timestamp_ctrl] NULL\n"));
		return D_DD_TIMESTAMP_INPUT_PARAM_ERR;
	}
	if( timestamp_ctrl->frequency == 0 ){
		Ddim_Assertion(("Dd_TIMESTAMP_Ctrl: input param error. [frequency] 0\n"));
		return D_DD_TIMESTAMP_INPUT_PARAM_ERR;
	}
	if( timestamp_ctrl->frequency > D_DD_TIMESTAMP_FREQUENCY_MAX ){
		Ddim_Assertion(("Dd_TIMESTAMP_Ctrl: input param error. [frequency] MAX Over\n"));
		return D_DD_TIMESTAMP_INPUT_PARAM_ERR;
	}
#endif
	Dd_TIMESTAMP_Set_CNTCR_HDbg(timestamp_ctrl->hdbg);										// Set value of CNTCR register
	Dd_TIMESTAMP_Set_CNTCVL((ULONG)(timestamp_ctrl->counter & 0x00000000FFFFFFFF));			// Set value of CNTCVL register
	Dd_TIMESTAMP_Set_CNTCVU((ULONG)((timestamp_ctrl->counter & 0xFFFFFFFF00000000) >> 32));	// Set value of CNTCVU register
	Dd_TIMESTAMP_Set_CNTFID0(timestamp_ctrl->frequency);									// Set value of CNTFID0 register
	
	return D_DDIM_OK;
}

/**
 * @brief	Start Global timer Counter function.
 */
VOID Dd_TIMESTAMP_Start(VOID)
{
	Dd_TIMESTAMP_Set_CNTCR_En(1);								// Timestamp Counter Increment
	Dd_ARM_Dsb_Pou();
}

/**
 * @brief	Stop Global timer function.
 */
VOID Dd_TIMESTAMP_Stop(VOID)
{
	Dd_TIMESTAMP_Set_CNTCR_En(0);								// Timestamp Counter no Increment
	Dd_ARM_Dsb_Pou();
}

/**
 * @brief	Exclusive control of Timestamp is released.
 * @return	INT32 D_DDIM_OK/D_DD_TIMESTAMP_SEM_NG
 */
INT32 Dd_TIMESTAMP_Close(VOID) 
{
	DDIM_USER_ER	ercd;
	DDIM_USER_ID	sid;
	
	sid = SID_DD_TIMESTAMP;
	
	ercd = DDIM_User_Sig_Sem(sid);								// sig_sem()
	if( D_DDIM_USER_E_OK != ercd ){
		return D_DD_TIMESTAMP_SEM_NG;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief	Get value of TIMESTAMP CNTCVL and CNTCVU register.
 * @param	UINT64* timestamp_counter
 * @return	INT32 D_DDIM_OK/D_DD_TIMESTAMP_INPUT_PARAM_ERR
 */
INT32 Dd_TIMESTAMP_Get_Counter(UINT64* timestamp_counter)
{
	ULONG up_counter1 = 0;
	ULONG up_counter2 = 0;
	ULONG low_counter = 0;
	
#ifdef CO_PARAM_CHECK
	if( timestamp_counter == NULL ){
		Ddim_Assertion(("Dd_TIMESTAMP_Get_Counter: input param error. [*timestamp_counter] NULL\n"));
		return D_DD_TIMESTAMP_INPUT_PARAM_ERR;
	}
#endif
	up_counter1 = Dd_TIMESTAMP_Get_CNTCVU;						// Get value of TIMESTAMP CNTCVU register
	low_counter = Dd_TIMESTAMP_Get_CNTCVL;						// Get value of TIMESTAMP CNTCVL register
	up_counter2 = Dd_TIMESTAMP_Get_CNTCVU;						// Get value of TIMESTAMP CNTCVU register
	while( up_counter1 != up_counter2 ){
		up_counter1 = up_counter2;
		low_counter = Dd_TIMESTAMP_Get_CNTCVL;					// Get value of TIMESTAMP CNTCVL register
		up_counter2 = Dd_TIMESTAMP_Get_CNTCVU;					// Get value of TIMESTAMP CNTCVU register
	}
	*timestamp_counter = ((UINT64)up_counter2 << 32) + low_counter;

	return D_DDIM_OK;
}

/**
 * @brief	Get value of Timestamp Control value.
 * @param	T_DD_TIMESTAMP_CTRL* timestamp_ctrl
 * @return	INT32 D_DDIM_OK/D_DD_TIMESTAMP_INPUT_PARAM_ERR
 */
INT32 Dd_TIMESTAMP_Get_Control(T_DD_TIMESTAMP_CTRL* timestamp_ctrl)
{
#ifdef CO_PARAM_CHECK
	if( timestamp_ctrl == NULL ){
		Ddim_Assertion(("Dd_TIMESTAMP_Get_Control: input param error. [*timestamp_ctrl] NULL\n"));
		return D_DD_TIMESTAMP_INPUT_PARAM_ERR;
	}
#endif
	timestamp_ctrl->hdbg = Dd_TIMESTAMP_Get_CNTCR_HDbg;			// Get value of CNTCR register
	Dd_TIMESTAMP_Get_Counter(&(timestamp_ctrl->counter));		// Get value of CNTCVL and CNTCVU register
	timestamp_ctrl->frequency = Dd_TIMESTAMP_Get_CNTFID0;		// Get value of CNTFID0 register
	
	return D_DDIM_OK;
}

/**
 * @brief	Set value of TIMESTAMP CNTCVL and CNTCVU register.
 * @param	UINT64 timestamp_counter
 * @return	INT32 D_DDIM_OK
 */
INT32 Dd_TIMESTAMP_Set_Counter(UINT64 timestamp_counter)
{
	Dd_TIMESTAMP_Set_CNTCR_En(0);														// Timestamp Counter no Increment
	Dd_TIMESTAMP_Set_CNTCVL((ULONG)(timestamp_counter & 0x00000000FFFFFFFF));			// Set value of CNTCVL register
	Dd_TIMESTAMP_Set_CNTCVU((ULONG)((timestamp_counter & 0xFFFFFFFF00000000) >> 32));	// Set value of CNTCVU register
	
	return D_DDIM_OK;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/**
 * @brief	The operation condition of Timestamp is set. VALUE unit:[us]
 * @param	ULONG usec, ULONG frequency
 * @return	INT32 D_DDIM_OK/D_DD_TIMESTAMP_INPUT_PARAM_ERR
 */
INT32 Dd_TIMESTAMP_Set_Timer(ULONG usec, ULONG frequency)
{
//	UINT64 usec_count;
	ULONG usec_count_l;
	ULONG usec_count_u;
	ULONG usec_31_24;
	ULONG usec_23_12;
	ULONG usec_11_0;
	ULONG usec_count_31_24;
	ULONG usec_count_23_12;
	ULONG usec_count_11_0;
	ULONG usec_count_l_wk;
	ULONG usec_count_u_wk;

#ifdef CO_PARAM_CHECK
	if( frequency == 0 ){
		Ddim_Assertion(("Dd_TIMESTAMP_Set_Timer: input param error. [frequency] 0\n"));
		return D_DD_TIMESTAMP_INPUT_PARAM_ERR;
	}
	if( frequency > D_DD_TIMESTAMP_FREQUENCY_MAX ){
		Ddim_Assertion(("Dd_TIMESTAMP_Set_Timer: input param error. [frequency] MAX Over\n"));
		return D_DD_TIMESTAMP_INPUT_PARAM_ERR;
	}
#endif
	Dd_TIMESTAMP_Init();
//	usec_count = (usec * 1000000) / frequency;					// Counter value
	usec_31_24 = ((usec & 0xFF000000) >> 24);
	usec_23_12 = ((usec & 0x00FFF000) >> 12);
	usec_11_0 = usec & 0x00000FFF;
	if( frequency > 0xFFFFFF ){
		usec_count_31_24 = ((usec_31_24 * 1000000) / (frequency / 0x1000000));
		usec_count_23_12 = ((usec_23_12 * 1000000) / (frequency / 0x1000000));
		usec_count_11_0 = ((usec_11_0 * 1000000) / (frequency / 0x1000000));
	}
	else{
		usec_count_31_24 = (usec_31_24 * 1000000) / frequency;
		usec_count_23_12 = (usec_23_12 * 1000000) / frequency;
		usec_count_11_0 = (usec_11_0 * 1000000) / frequency;
	}
	usec_count_l_wk = ((usec_count_31_24 & 0x0000000F) << 24) + \
	                  ((usec_count_23_12 & 0x0000FFFF) << 12) + \
	                  (usec_count_11_0 & 0x0FFFFFFF);
	usec_count_u_wk = ((usec_count_31_24 & 0xFFFFFFF0) >> 4) + \
	                  ((usec_count_23_12 & 0xFFFF0000) >> 16) + \
	                  ((usec_count_11_0 & 0xF0000000) >> 28) + \
	                  ((usec_count_l_wk & 0xF0000000) >> 28);
    usec_count_l = ((usec_count_u_wk & 0x0000000F) << 28) + (usec_count_l_wk & 0x0FFFFFF);
    usec_count_u = ((usec_count_u_wk & 0xFFFFFFF0) >> 4);
    
	Dd_TIMESTAMP_Set_CNTFID0(frequency);
//	Dd_TIMESTAMP_Set_Counter(usec_count);
	if( frequency > 0xFFFFFF ){
		Dd_TIMESTAMP_Set_CNTCVL((usec_count_l / 0x1000000) | ((usec_count_u & 0x00FFFFFF) << 8));
		Dd_TIMESTAMP_Set_CNTCVU(usec_count_u / 0x1000000);
	}
	else{
		Dd_TIMESTAMP_Set_CNTCVL(usec_count_l);
		Dd_TIMESTAMP_Set_CNTCVU(usec_count_u);
	}
	Dd_TIMESTAMP_Set_CNTCR_En(1);
	
	return D_DDIM_OK;
}
#endif	// CO_DDIM_UTILITY_USE
