/**
 * @file		dd_tmr64.c
 * @brief		peripheral 64 bit reload timer driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dd_tmr64.h"
#include "peripheral.h"
#include "dd_hdmac1.h"
#include "dd_arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
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
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Global Data															*/
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
/* Local Function														*/
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
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/**
 * @brief  The 64bit Timer is exclusively controlled.
 * @param  INT32 tmout
 * @return INT32 D_DDIM_OK/D_DD_TMR64_SEM_NG/D_DD_TMR64_INPUT_PARAM_ERR/D_DD_TMR64_SEM_TIMEOUT
 */
INT32 Dd_TMR64_Open(INT32 tmout)
{

	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if (tmout < D_DDIM_USER_SEM_WAIT_FEVR) {
		Ddim_Assertion(("Dd_TMR64_Open: input param error. tmout = %x\n", tmout));
		return D_DD_TMR64_INPUT_PARAM_ERR;
	}
#endif

	if( D_DDIM_USER_SEM_WAIT_POL == tmout ) {
		ercd = DDIM_User_Pol_Sem(SID_DD_TMR64);							// pol_sem()
	}
	else{
		ercd = DDIM_User_Twai_Sem(SID_DD_TMR64, (DDIM_USER_TMO)tmout);	// twai_sem()
	}

	if( D_DDIM_USER_E_OK != ercd ) {
		if( D_DDIM_USER_E_TMOUT == ercd ) {
			return D_DD_TMR64_SEM_TIMEOUT;
		}
		return D_DD_TMR64_SEM_NG;
	}
	return D_DDIM_OK;

}

/**
 * @brief  The exclusive control of control it is released.
 * @return UINT32 D_DDIM_OK/D_DD_TMR64_SEM_NG
 */
INT32 Dd_TMR64_Close(VOID)
{

	DDIM_USER_ER	ercd;

	ercd = DDIM_User_Sig_Sem( SID_DD_TMR64 );				// sig_sem()

	if( D_DDIM_USER_E_OK != ercd ) {
		return D_DD_TMR64_SEM_NG;
	}
	return D_DDIM_OK;

}

/**
 * @brief  The operation condition of Timer is set.
 * @param  T_DD_TMR64_CTRL* tmr64_ctrl
 * @return INT32 D_DDIM_OK/D_DD_TMR64_INPUT_PARAM_ERR
 */
INT32 Dd_TMR64_Ctrl(T_DD_TMR64_CTRL const* const tmr64_ctrl)
{
#ifdef CO_PARAM_CHECK
	if (tmr64_ctrl == NULL) {
		Ddim_Assertion(("Dd_TMR64_Ctrl: input param error. [*tmr64_ctrl] NULL\n"));
		return D_DD_TMR64_INPUT_PARAM_ERR;
	}
#endif
	// It sets it to the register
	IO_PERI.TMR64.TMCSR64_1.word		= tmr64_ctrl->csr1.word;
	IO_PERI.TMR64.TMCSR64_1.word		&= 0xFFFFFFFC;
	IO_PERI.TMR64.TMCSR64_2.word		= tmr64_ctrl->csr2.word;

	return D_DDIM_OK;
}

/**
 * @brief  All the register values of 64bit reload timer are acquired.
 * @param  T_DD_TMR64_CTRL* tmr64_ctrl
 * @return INT32 D_DDIM_OK/D_DD_TMR64_INPUT_PARAM_ERR
 */
INT32 Dd_TMR64_Get_Ctrl(T_DD_TMR64_CTRL* const tmr64_ctrl)
{
#ifdef CO_PARAM_CHECK
	if (tmr64_ctrl == NULL) {
		Ddim_Assertion(("Dd_TMR64_Get_Ctrl: input param error. [*tmr64_ctrl] NULL\n"));
		return D_DD_TMR64_INPUT_PARAM_ERR;
	}
#endif

	// Register value acquisition
	tmr64_ctrl->csr1.word		= IO_PERI.TMR64.TMCSR64_1.word;
	tmr64_ctrl->csr2.word		= IO_PERI.TMR64.TMCSR64_2.word;

	return D_DDIM_OK;
}

/**
 * @brief  64bit Timer is started.
 * @param  BOOL time_stamp_enable
 * @return INT32 D_DDIM_OK
 */
INT32 Dd_TMR64_Start(BOOL time_stamp_enable)
{
	if (time_stamp_enable) {
		IO_PERI.TMR64.TMCSR64_2.word |= 0x00010000;	// TSEN=1
	}
	else {
		IO_PERI.TMR64.TMCSR64_2.word &= 0xFFFEFFFF;	// TSEN=0
	}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	IO_PERI.TMR64.TMCSR64_1.word |= 0x00000003;		// CNTE=1,TRG=1
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	// Z-EVAL - QA #4772
	IO_PERI.TMR64.TMCSR64_1.word |= 0x00000013;		// CNTE=1,TRG=1, bit4=1
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	Dd_ARM_Dsb_Pou();

	return D_DDIM_OK;
}

/**
 * @brief  64bit Timer is Stopped.
 * @return INT32 D_DDIM_OK
 */
INT32 Dd_TMR64_Stop(VOID)
{
	IO_PERI.TMR64.TMCSR64_2.word &= 0xFFFEFFFF;	// TSEN=0
	IO_PERI.TMR64.TMCSR64_1.word &= 0xFFFFFFFC;	// CNTE=0,TRG=0
	Dd_ARM_Dsb_Pou();

	return D_DDIM_OK;
}

/**
 * @brief  Get 64bit Timer Gyro Time Stamp value.
 * @param  ULONG* time_stamp
 * @return INT32 D_DDIM_OK/D_DD_TMR64_INPUT_PARAM_ERR
 */
INT32 Dd_TMR64_Get_Gyro_TimeStamp(ULONG* const time_stamp)
{
#ifdef CO_PARAM_CHECK
	if (time_stamp == NULL) {
		Ddim_Assertion(("Dd_TMR64_Get_Gyro_TimeStamp: input param error. [*time_stamp] is NULL\n"));
		return D_DD_TMR64_INPUT_PARAM_ERR;
	}
#endif

	*time_stamp = IO_PERI.TMR64.GSTMP;

	return D_DDIM_OK;
}

/**
 * @brief  Get 64bit Timer Frame Start Time Stamp value.
 * @param  UINT32 sensor_no
 * @param  UINT32 lotation_count
 * @param  UINT64* time_stamp
 * @return INT32 D_DDIM_OK/D_DD_TMR64_INPUT_PARAM_ERR
 */
INT32 Dd_TMR64_Get_FrameStart_TimeStamp(UINT32 sensor_no, UINT32 lotation_count, UINT64* const time_stamp)
{
	ULONG l_count = 0;
	ULONG h_count = 0;

#ifdef CO_PARAM_CHECK
	if (sensor_no >= 4 || lotation_count >= 3) {
		Ddim_Assertion(("Dd_TMR64_Get_FrameStart_TimeStamp: input param error. sensor_no or lotation_count is Over\n"));
		return D_DD_TMR64_INPUT_PARAM_ERR;
	}
	else if (time_stamp == NULL) {
		Ddim_Assertion(("Dd_TMR64_Get_FrameStart_TimeStamp: input param error. [*time_stamp] is NULL\n"));
		return D_DD_TMR64_INPUT_PARAM_ERR;
	}
#endif

	if (sensor_no == 0) {
		switch (lotation_count) {
			case 0:
				l_count = IO_PERI.TMR64.FSSTMP00L;
				h_count = IO_PERI.TMR64.FSSTMP00H;
				break;
			case 1:
				l_count = IO_PERI.TMR64.FSSTMP01L;
				h_count = IO_PERI.TMR64.FSSTMP01H;
				break;
			case 2:
				l_count = IO_PERI.TMR64.FSSTMP02L;
				h_count = IO_PERI.TMR64.FSSTMP02H;
				break;
			default:
				Ddim_Print(("Dd_TMR64_Get_FrameStart_TimeStamp : lotation_count is over.\n"));
				break;
		}
	}
	else if (sensor_no == 1) {
		switch (lotation_count) {
			case 0:
				l_count = IO_PERI.TMR64.FSSTMP10L;
				h_count = IO_PERI.TMR64.FSSTMP10H;
				break;
			case 1:
				l_count = IO_PERI.TMR64.FSSTMP11L;
				h_count = IO_PERI.TMR64.FSSTMP11H;
				break;
			case 2:
				l_count = IO_PERI.TMR64.FSSTMP12L;
				h_count = IO_PERI.TMR64.FSSTMP12H;
				break;
			default:
				Ddim_Print(("Dd_TMR64_Get_FrameStart_TimeStamp : lotation_count is over.\n"));
				break;
		}
	}
	else if (sensor_no == 2) {
		switch (lotation_count) {
			case 0:
				l_count = IO_PERI.TMR64.FSSTMP20L;
				h_count = IO_PERI.TMR64.FSSTMP20H;
				break;
			case 1:
				l_count = IO_PERI.TMR64.FSSTMP21L;
				h_count = IO_PERI.TMR64.FSSTMP21H;
				break;
			case 2:
				l_count = IO_PERI.TMR64.FSSTMP22L;
				h_count = IO_PERI.TMR64.FSSTMP22H;
				break;
			default:
				Ddim_Print(("Dd_TMR64_Get_FrameStart_TimeStamp : lotation_count is over.\n"));
				break;
		}
	}
	else if (sensor_no == 3) {
		switch (lotation_count) {
			case 0:
				l_count = IO_PERI.TMR64.FSSTMP30L;
				h_count = IO_PERI.TMR64.FSSTMP30H;
				break;
			case 1:
				l_count = IO_PERI.TMR64.FSSTMP31L;
				h_count = IO_PERI.TMR64.FSSTMP31H;
				break;
			case 2:
				l_count = IO_PERI.TMR64.FSSTMP32L;
				h_count = IO_PERI.TMR64.FSSTMP32H;
				break;
			default:
				Ddim_Print(("Dd_TMR64_Get_FrameStart_TimeStamp : lotation_count is over.\n"));
				break;
		}
	}
	else{
		Ddim_Print(("Dd_TMR64_Get_FrameStart_TimeStamp : sensor_no is over.\n"));
	}

	*time_stamp = ((UINT64)h_count << 32) + l_count;

	return D_DDIM_OK;
}

/**
 * @brief  Get 64bit Timer Frame End Time Stamp  value.
 * @param  UINT32 sensor_no
 * @param  UINT32 lotation_count
 * @param  UINT64* time_stamp
 * @return INT32 D_DDIM_OK/D_DD_TMR64_INPUT_PARAM_ERR
 */
INT32 Dd_TMR64_Get_FrameEnd_TimeStamp(UINT32 sensor_no, UINT32 lotation_count, UINT64* const time_stamp)
{
	ULONG l_count = 0;
	ULONG h_count = 0;

#ifdef CO_PARAM_CHECK
	if (sensor_no >= 4 || lotation_count >= 3) {
		Ddim_Assertion(("Dd_TMR64_Get_FrameEnd_TimeStamp: input param error. sensor_no or lotation_count is Over\n"));
		return D_DD_TMR64_INPUT_PARAM_ERR;
	}
	else if (time_stamp == NULL) {
		Ddim_Assertion(("Dd_TMR64_Get_FrameEnd_TimeStamp: input param error. [*time_stamp] is NULL\n"));
		return D_DD_TMR64_INPUT_PARAM_ERR;
	}
#endif

	if (sensor_no == 0) {
		switch (lotation_count) {
			case 0:
				l_count = IO_PERI.TMR64.FESTMP00L;
				h_count = IO_PERI.TMR64.FESTMP00H;
				break;
			case 1:
				l_count = IO_PERI.TMR64.FESTMP01L;
				h_count = IO_PERI.TMR64.FESTMP01H;
				break;
			case 2:
				l_count = IO_PERI.TMR64.FESTMP02L;
				h_count = IO_PERI.TMR64.FESTMP02H;
				break;
			default:
				Ddim_Print(("Dd_TMR64_Get_FrameEnd_TimeStamp : lotation_count is over.\n"));
				break;
		}
	}
	else if (sensor_no == 1) {
		switch (lotation_count) {
			case 0:
				l_count = IO_PERI.TMR64.FESTMP10L;
				h_count = IO_PERI.TMR64.FESTMP10H;
				break;
			case 1:
				l_count = IO_PERI.TMR64.FESTMP11L;
				h_count = IO_PERI.TMR64.FESTMP11H;
				break;
			case 2:
				l_count = IO_PERI.TMR64.FESTMP12L;
				h_count = IO_PERI.TMR64.FESTMP12H;
				break;
			default:
				Ddim_Print(("Dd_TMR64_Get_FrameEnd_TimeStamp : lotation_count is over.\n"));
				break;
		}
	}
	else if (sensor_no == 2) {
		switch (lotation_count) {
			case 0:
				l_count = IO_PERI.TMR64.FESTMP20L;
				h_count = IO_PERI.TMR64.FESTMP20H;
				break;
			case 1:
				l_count = IO_PERI.TMR64.FESTMP21L;
				h_count = IO_PERI.TMR64.FESTMP21H;
				break;
			case 2:
				l_count = IO_PERI.TMR64.FESTMP22L;
				h_count = IO_PERI.TMR64.FESTMP22H;
				break;
			default:
				Ddim_Print(("Dd_TMR64_Get_FrameEnd_TimeStamp : lotation_count is over.\n"));
				break;
		}
	}
	else if (sensor_no == 3) {
		switch (lotation_count) {
			case 0:
				l_count = IO_PERI.TMR64.FESTMP30L;
				h_count = IO_PERI.TMR64.FESTMP30H;
				break;
			case 1:
				l_count = IO_PERI.TMR64.FESTMP31L;
				h_count = IO_PERI.TMR64.FESTMP31H;
				break;
			case 2:
				l_count = IO_PERI.TMR64.FESTMP32L;
				h_count = IO_PERI.TMR64.FESTMP32H;
				break;
			default:
				Ddim_Print(("Dd_TMR64_Get_FrameEnd_TimeStamp : lotation_count is over.\n"));
				break;
		}
	}
	else{
		Ddim_Print(("Dd_TMR64_Get_FrameEnd_TimeStamp : sensor_no is over.\n"));
	}

	*time_stamp = ((UINT64)h_count << 32) + l_count;

	return D_DDIM_OK;
}

/**
 * @brief  Get 64bit Timer counter value.
 * @param  UINT64* counter
 * @return INT32 D_DDIM_OK/D_DD_TMR64_INPUT_PARAM_ERR
 */
INT32 Dd_TMR64_Get_Counter(UINT64* const counter)
{
#ifdef CO_PARAM_CHECK
	if (counter == NULL) {
		Ddim_Assertion(("Dd_TMR64_Get_Counter: input param error. [*counter] is NULL\n"));
		return D_DD_TMR64_INPUT_PARAM_ERR;
	}
#endif

	// Counter load trigger ON
	IO_PERI.TMR64.TMCSR64_2.word |= 0x00000001;		// CVT

	// Get Counter
	*counter = ((UINT64)IO_PERI.TMR64.TMR64_H << 32) + IO_PERI.TMR64.TMR64_L;

	return D_DDIM_OK;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/**
 * @brief  Enable/Disable 64bit Time Stamp.
 * @param  BOOL time_stamp_enable
 * @return INT32 D_DDIM_OK
 */
INT32 Dd_TMR64_Time_Stamp_Enable(BOOL time_stamp_enable)
{
	if (time_stamp_enable) {
		IO_PERI.TMR64.TMCSR64_2.word |= 0x00010000;	// TSEN=1
	}
	else {
		IO_PERI.TMR64.TMCSR64_2.word &= 0xFFFEFFFF;	// TSEN=0
	}

	Dd_ARM_Dsb_Pou();

	return D_DDIM_OK;
}


//---------------------------- colabo section ----------------------------
// Nothing Special

#endif
