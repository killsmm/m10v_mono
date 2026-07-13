/**
 * @file		measure_time.h
 * @brief		Time Measurement
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _MEASURE_TIME_H
#define _MEASURE_TIME_H

#include "ddim_typedef.h"
#include "os_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/**
 * time measurement id
 * @note define it according to a category of the time measurement.
 */
typedef enum {
	E_BF_MEASURE_TIME_ID_BOOT = 0,
	E_BF_MEASURE_TIME_ID_FILE_SYSTEM,
	E_BF_MEASURE_TIME_ID_FILE_WRITE,
	E_BF_MEASURE_TIME_ID_FILE_READ,
	E_BF_MEASURE_TIME_ID_DCF_DB,
	E_BF_MEASURE_TIME_ID_SYSTEM,
	E_BF_MEASURE_TIME_ID_AUDIO,
	E_BF_MEASURE_TIME_ID_CAPTURE_TOTAL,
	E_BF_MEASURE_TIME_ID_CAPTURE_PART,
	E_BF_MEASURE_TIME_ID_LENS_MAIN,
	E_BF_MEASURE_TIME_ID_LENS_SUB,
	E_BF_MEASURE_TIME_ID_3A_0,
	E_BF_MEASURE_TIME_ID_MJP_ENC_REC,
	E_BF_MEASURE_TIME_ID_MJP_ENC_GCH,
	E_BF_MEASURE_TIME_ID_PLAYBACK,
	E_BF_MEASURE_TIME_ID_PB_ZOOM,
	E_BF_MEASURE_TIME_ID_SLIDE,
	E_BF_MEASURE_TIME_ID_OSD,
	E_BF_MEASURE_TIME_ID_JPGTHUMB,
	E_BF_MEASURE_TIME_ID_JPGMAIN,
	E_BF_MEASURE_TIME_ID_FD,
	E_BF_MEASURE_TIME_ID_MD,
	E_BF_MEASURE_TIME_ID_HRZ,
	E_BF_MEASURE_TIME_ID_IP_0,
	E_BF_MEASURE_TIME_ID_IP_1,
	E_BF_MEASURE_TIME_ID_IP_2,
	E_BF_MEASURE_TIME_ID_IP_3,
	E_BF_MEASURE_TIME_ID_IP_4,
	E_BF_MEASURE_TIME_ID_DEBUG_0,
	E_BF_MEASURE_TIME_ID_DEBUG_1,
	E_BF_MEASURE_TIME_ID_DEBUG_2,
	E_BF_MEASURE_TIME_ID_DEBUG_3,
	E_BF_MEASURE_TIME_ID_DEBUG_4,
	E_BF_MEASURE_TIME_ID_BUFFER_MAX
} E_BF_MEASURE_TIME_ID;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * start measure of time
 * @return Time[usec]
 */
ULONG BF_Get_Time_us(VOID);

/**
 * start measure of time
 * @return Time[msec]
 */
ULONG BF_Get_Time_ms(VOID);

/**
 * start measure of time
 * @return Time[usec]
 */
UINT64 BF_Get_Time_us_64(VOID);

/**
 * init measure of time
 * @return None
 */
VOID BF_Measure_Time_Init(VOID);

/**
 * start measure of time (micro second time)
 * @param E_BF_MEASURE_TIME_ID id
 * @return int: 0(normal end), -1(id is abnormal)
 * @note
 * @attention only once, must be call before measure of time
 */
INT32 BF_Measure_Time_Start_usec(E_BF_MEASURE_TIME_ID id);

/**
 * start measure of time (milli second time)
 * @param E_BF_MEASURE_TIME_ID id
 * @return int: 0(normal end), -1(id is abnormal)
 * @note
 * @attention only once, must be call before measure of time
 */
INT32 BF_Measure_Time_Start(E_BF_MEASURE_TIME_ID id);

/**
 * get measure of time (micro second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @param o:OS_USER_SYSTIM* m_time
 * @return int: 0(normal end), -1(id is abnormal)
 */
INT32 BF_Measure_Time_Get_usec(E_BF_MEASURE_TIME_ID id, ULONG* const us_time);

/**
 * get measure of time (milli second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @param o:OS_USER_SYSTIM* m_time
 * @return int: 0(normal end), -1(id is abnormal)
 */
INT32 BF_Measure_Time_Get(E_BF_MEASURE_TIME_ID id, OS_USER_SYSTIM* const m_time);

/**
 * get measure of long time (micro second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @return int: >0(long time), 0(id is abnormal)
 */
ULONG BF_Measure_Time_Get_Long_usec(E_BF_MEASURE_TIME_ID id);

/**
 * get measure of long time (milli second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @return int: >0(long time), 0(id is abnormal)
 */
ULONG BF_Measure_Time_Get_Long(E_BF_MEASURE_TIME_ID id);

/**
 * get & print measure of time (micro second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @param i:const char* comment
 * @param o:SYSTIM* m_time
 * @return int: 0(normal end), -1(id is abnormal)
 * @note print measure of time by unified format
 */
INT32 BF_Measure_Time_Get_Print_usec(E_BF_MEASURE_TIME_ID id, const char* const comment, ULONG* const us_time);

/**
 * get & print measure of time (milli second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @param i:const char* comment
 * @param o:SYSTIM* m_time
 * @return int: 0(normal end), -1(id is abnormal)
 * @note print measure of time by unified format
 */
INT32 BF_Measure_Time_Get_Print(E_BF_MEASURE_TIME_ID id, const char* const comment, OS_USER_SYSTIM* const m_time);

/**
 * print measure of time (micro second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @param i:const char* comment
 * @return int: 0(normal end), -1(id is abnormal)
 * @note print measure of time by unified format
 */
INT32 BF_Measure_Time_Print_usec(E_BF_MEASURE_TIME_ID id, const char* const comment);

/**
 * print measure of time (milli second time)
 * @param i:E_BF_MEASURE_TIME_ID id
 * @param i:const char* comment
 * @return int: 0(normal end), -1(id is abnormal)
 * @note print measure of time by unified format
 */
INT32 BF_Measure_Time_Print(E_BF_MEASURE_TIME_ID id, const char* const comment);

#ifdef __cplusplus
}
#endif

#endif	// _MEASURE_TIME_H
