/**
 * @file		idle_task_monitor.h
 * @brief		Measure CPU load average by IDLE task.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef _IDLE_TASK_MONITOR_H
#define _IDLE_TASK_MONITOR_H

#include "ddim_typedef.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure  															*/
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
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


extern VOID Monitor_Idle_Measure( UINT32 count, UINT32 measure_time );
extern VOID Monitor_Idle_Task( INT32 stacd );


#ifdef __cplusplus
}
#endif

#endif	// _IDLE_TASK_MONITOR_H
