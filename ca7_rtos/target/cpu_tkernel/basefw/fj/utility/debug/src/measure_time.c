/**
 * @file		measure_time.c
 * @brief		Time Measurement
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#ifdef CO_MEASURE_TIMER
#include "dd_timestamp.h"
#endif
#include "measure_time.h"
#include "debug.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define FJ_DEBUG_PRINT_ON

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/**
 * measure of time buffer
 * @note be defined according to a category of the measure of time.
 */
#ifdef FJ_DEBUG_PRINT_ON
static ULONG gBF_Measure_Time_Timer_buf_us[E_BF_MEASURE_TIME_ID_BUFFER_MAX] = {0};
static OS_USER_SYSTIM gBF_Measure_Time_Timer_buf[E_BF_MEASURE_TIME_ID_BUFFER_MAX] = {0};
#endif
static ULONG gBF_Measure_Time_frequency = 0;

/*----------------------------------------------------------------------*/
/* Macro														 		*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
#ifdef FJ_DEBUG_PRINT_ON
static VOID measure_time_print_us(ULONG us_time, const char* const comment)
{
	BF_Debug_Print_Information(("\x1b[4mMT=%lu.%03lu(ms)\x1b[0m: %s", (us_time / 1000), (us_time % 1000), comment));
}

/**
 * print measure of time
 * @param i:ULONG ltime
 * @param i:const char* comment
 * @note print measure of time by unified format
 */
static VOID measure_time_print(ULONG ltime, const char* const comment)
{
	BF_Debug_Print_Information(("\x1b[4mMT=%lu(ms)\x1b[0m: %s", ltime, comment));
}
#endif

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * start measure of time
 * @return Time[usec]
 */
ULONG BF_Get_Time_us(VOID)
{
#ifdef CO_MEASURE_TIMER
	UINT64 timestamp_counter;
	
	Dd_TIMESTAMP_Get_Counter( &timestamp_counter );
	
	return (ULONG)(timestamp_counter / (gBF_Measure_Time_frequency / 1000000));
#else
	OS_USER_SYSTIM systim;
	OS_User_Get_Tim( &systim );
	return ((ULONG)systim) * 1000;
#endif
}

/**
 * start measure of time
 * @return Time[msec]
 */
ULONG BF_Get_Time_ms(VOID)
{
#ifdef CO_MEASURE_TIMER
	UINT64 timestamp_counter;
	
	Dd_TIMESTAMP_Get_Counter( &timestamp_counter );
	
	return (ULONG)(timestamp_counter / (gBF_Measure_Time_frequency / 1000));
#else
	OS_USER_SYSTIM systim;
	OS_User_Get_Tim( &systim );
	return ((ULONG)systim);
#endif
}

/**
 * start measure of time
 * @return Time[usec]
 */
UINT64 BF_Get_Time_us_64(VOID)
{
#ifdef CO_MEASURE_TIMER
	UINT64 timestamp_counter;
	
	Dd_TIMESTAMP_Get_Counter( &timestamp_counter );
	
	return timestamp_counter / (gBF_Measure_Time_frequency / 1000000);
#else
	OS_USER_SYSTIM systim;
	OS_User_Get_Tim( &systim );
	return systim * 1000;
#endif
}

/**
 * init measure of time
 * @return None
 */
VOID BF_Measure_Time_Init(VOID)
{
#ifdef CO_MEASURE_TIMER
	T_DD_TIMESTAMP_CTRL timestamp_ctrl;

	Dd_TIMESTAMP_Get_Control(&timestamp_ctrl);
	gBF_Measure_Time_frequency = timestamp_ctrl.frequency;
#endif
}

/**
 * start measure of time (micro second time)
 * @param E_BF_MEASURE_TIME_ID id
 * @return int: 0(normal end), -1(id is abnormal)
 * @note
 * @attention only once, must be call before measure of time
 */
INT32 BF_Measure_Time_Start_usec(E_BF_MEASURE_TIME_ID id)
{
#ifdef FJ_DEBUG_PRINT_ON
#ifdef CO_MEASURE_TIMER
	OS_USER_SYSTIM sys_time;
	
	if (id >= E_BF_MEASURE_TIME_ID_BUFFER_MAX){
		return -1;
	}
	sys_time = BF_Get_Time_us();
	gBF_Measure_Time_Timer_buf_us[id] = sys_time;
#else
	OS_USER_SYSTIM sys_time;
	
	if (id >= E_BF_MEASURE_TIME_ID_BUFFER_MAX){
		return -1;
	}
	OS_User_Get_Tim(&sys_time);
	gBF_Measure_Time_Timer_buf_us[id] = ((ULONG)sys_time) * 1000;
#endif
#endif
	return 0;
}

/**
 * start measure of time (milli second time)
 * @param E_BF_MEASURE_TIME_ID id
 * @return int: 0(normal end), -1(id is abnormal)
 * @note
 * @attention only once, must be call before measure of time
 */
INT32 BF_Measure_Time_Start(E_BF_MEASURE_TIME_ID id)
{
#ifdef FJ_DEBUG_PRINT_ON
	OS_USER_SYSTIM sys_time;
	
	if (id >= E_BF_MEASURE_TIME_ID_BUFFER_MAX){
		return -1;
	}
#ifdef CO_MEASURE_TIMER
	sys_time = BF_Get_Time_ms();
#else
	OS_User_Get_Tim(&sys_time);
#endif
	gBF_Measure_Time_Timer_buf[id] = sys_time;
#endif
	return 0;
}

/**
 * get measure of time (micro second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @param o:SYSTIM* m_time
 * @return int: 0(normal end), -1(id is abnormal)
 */
INT32 BF_Measure_Time_Get_usec(E_BF_MEASURE_TIME_ID id, ULONG* const us_time)
{
#ifdef FJ_DEBUG_PRINT_ON
#ifdef CO_MEASURE_TIMER
	ULONG sys_time;

	if (id >= E_BF_MEASURE_TIME_ID_BUFFER_MAX){
		return -1;
	}
	sys_time = BF_Get_Time_us();
	*us_time = sys_time - gBF_Measure_Time_Timer_buf_us[id];
#else
	OS_USER_SYSTIM sys_time;
	
	if (id >= E_BF_MEASURE_TIME_ID_BUFFER_MAX){
		return -1;
	}
	OS_User_Get_Tim(&sys_time);
	*us_time = ((ULONG)sys_time) * 1000 - gBF_Measure_Time_Timer_buf_us[id];
#endif
#else
	*us_time = 0;	// guarantee of return value
#endif
	return 0;
}

/**
 * get measure of time (milli second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @param o:SYSTIM* m_time
 * @return int: 0(normal end), -1(id is abnormal)
 */
INT32 BF_Measure_Time_Get(E_BF_MEASURE_TIME_ID id, OS_USER_SYSTIM* const m_time)
{
#ifdef FJ_DEBUG_PRINT_ON
	OS_USER_SYSTIM sys_time;
	
	if (id >= E_BF_MEASURE_TIME_ID_BUFFER_MAX){
		return -1;
	}
#ifdef CO_MEASURE_TIMER
	sys_time = BF_Get_Time_ms();
#else
	OS_User_Get_Tim(&sys_time);
#endif
	*m_time = sys_time - gBF_Measure_Time_Timer_buf[id];
#else
	*m_time = 0;	// guarantee of return value
#endif
	return 0;
}

/**
 * get measure of long time (micro second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @return int: >=0(long time), 0(id is abnormal)
 */
ULONG BF_Measure_Time_Get_Long_usec(E_BF_MEASURE_TIME_ID id)
{
#ifdef FJ_DEBUG_PRINT_ON
	ULONG us_time;
	
	if (BF_Measure_Time_Get_usec(id, &us_time) < 0){
		return 0;
	}
	return us_time;
#else
	return 0;	// guarantee of return value
#endif
}

/**
 * get measure of long time (milli second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @return int: >=0(long time), 0(id is abnormal)
 */
ULONG BF_Measure_Time_Get_Long(E_BF_MEASURE_TIME_ID id)
{
#ifdef FJ_DEBUG_PRINT_ON
	OS_USER_SYSTIM m_time;
	
	if (BF_Measure_Time_Get(id, &m_time) < 0){
		return 0;
	}
	return (ULONG)m_time;
#else
	return 0;	// guarantee of return value
#endif
}

/**
 * print measure of time (micro second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @param i:const char* comment
 * @return int: 0(normal end), -1(id is abnormal)
 * @note print measure of time by unified format
 */
INT32 BF_Measure_Time_Print_usec(E_BF_MEASURE_TIME_ID id, const char* const comment)
{
#ifdef FJ_DEBUG_PRINT_ON
	ULONG us_time;
	
	if (BF_Measure_Time_Get_usec(id, &us_time) != 0){
		return -1;
	}
	measure_time_print_us(us_time, comment);
#endif
	return 0;
}

/**
 * print measure of time (milli second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @param i:const char* comment
 * @return int: 0(normal end), -1(id is abnormal)
 * @note print measure of time by unified format
 */
INT32 BF_Measure_Time_Print(E_BF_MEASURE_TIME_ID id, const char* const comment)
{
#ifdef FJ_DEBUG_PRINT_ON
	OS_USER_SYSTIM m_time;
	
	if (BF_Measure_Time_Get(id, &m_time) != 0){
		return -1;
	}
	measure_time_print((ULONG)m_time, comment);
#endif
	return 0;
}

/**
 * get & print measure of time (micro second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @param i:const char* comment
 * @param o:SYSTIM* m_time
 * @return int: 0(normal end), -1(id is abnormal)
 * @note print measure of time by unified format
 */
INT32 BF_Measure_Time_Get_Print_usec(E_BF_MEASURE_TIME_ID id, const char* const comment, ULONG* const us_time)
{
#ifdef FJ_DEBUG_PRINT_ON
	if (BF_Measure_Time_Get_usec(id, us_time) != 0){
		return -1;
	}
	measure_time_print_us(*us_time, comment);
#else
	*m_time = 0;	// guarantee of return value
#endif
	return 0;
}

/**
 * get & print measure of time (milli second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @param i:const char* comment
 * @param o:SYSTIM* m_time
 * @return int: 0(normal end), -1(id is abnormal)
 * @note print measure of time by unified format
 */
INT32 BF_Measure_Time_Get_Print(E_BF_MEASURE_TIME_ID id, const char* const comment, OS_USER_SYSTIM* const m_time)
{
#ifdef FJ_DEBUG_PRINT_ON
	if (BF_Measure_Time_Get(id, m_time) != 0){
		return -1;
	}
	measure_time_print((ULONG)*m_time, comment);
#else
	*m_time = 0;	// guarantee of return value
#endif
	return 0;
}

