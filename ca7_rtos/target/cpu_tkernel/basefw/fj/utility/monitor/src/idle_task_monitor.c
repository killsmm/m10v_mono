/**
 * @file		idle_task_monitor.c
 * @brief		Measure CPU load average by IDLE task.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

// Utility header
#include "idle_task_monitor.h"
#include "debug.h"

// OS header
#include "os_user_custom.h"

// Driver header
#include "ddim_typedef.h"
#include "dd_arm.h"

// STD-C header
#include <string.h>


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

// T_BF_IDLE_TASK_MONITOR_INFO
typedef struct {
	BOOL		core_valid;
	OS_USER_ID	idle_task_id;
	FLOAT		cpu_usage;
} T_BF_IDLE_TASK_MONITOR_INFO;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

static BOOL monitor_idle_measure_main( const UINT32 measure_time, T_BF_IDLE_TASK_MONITOR_INFO monitor_info[D_DD_ARM_NUM_OF_CORES] )
{
	OS_USER_PRI		task_prio;
	OS_USER_ER		os_ercd = D_OS_USER_E_SYS;
	T_OS_USER_ITSK	begin_time[D_DD_ARM_NUM_OF_CORES];
	T_OS_USER_ITSK	end_time[D_DD_ARM_NUM_OF_CORES];
	UINT32			loopcnt;
	OS_USER_RELTIM	running_time;			// msec

	memset( &begin_time, '\0', sizeof(begin_time) );
	memset( &end_time,   '\0', sizeof(end_time) );

	OS_User_Get_Pri( D_OS_USER_TSK_SELF, &task_prio );
	OS_User_Chg_Pri( D_OS_USER_TSK_SELF, 1 );	// Maximum priority.

	while( 1 ) {
		OS_User_Dly_Tsk( 1 );

		for( loopcnt = 0; loopcnt < D_DD_ARM_NUM_OF_CORES; loopcnt++ ) {
			if( monitor_info[loopcnt].core_valid == FALSE ) {
				continue;
			}

			os_ercd = OS_User_Inf_Tsk( monitor_info[loopcnt].idle_task_id, &begin_time[loopcnt], FALSE );
			if( os_ercd != D_OS_USER_E_OK ) {
				break;
			}
		}

		if( os_ercd != D_OS_USER_E_OK ) {
			break;
		}

		OS_User_Dly_Tsk( measure_time -1 );

		for( loopcnt = 0; loopcnt < D_DD_ARM_NUM_OF_CORES; loopcnt++ ) {
			if( monitor_info[loopcnt].core_valid == FALSE ) {
				continue;
			}

			os_ercd = OS_User_Inf_Tsk( monitor_info[loopcnt].idle_task_id, &end_time[loopcnt], FALSE );
			if( os_ercd != D_OS_USER_E_OK ) {
				break;
			}
		}

		break;
	}

	OS_User_Chg_Pri( D_OS_USER_TSK_SELF, task_prio );

	if( os_ercd != D_OS_USER_E_OK ) {
		return FALSE;
	}

	for( loopcnt = 0; loopcnt < D_DD_ARM_NUM_OF_CORES; loopcnt++ ) {
		if( monitor_info[loopcnt].core_valid == FALSE ) {
			continue;
		}

		running_time = (end_time[loopcnt].stime   + end_time[loopcnt].utime)
					 - (begin_time[loopcnt].stime + begin_time[loopcnt].utime);
		if( measure_time < running_time ) {
			monitor_info[loopcnt].cpu_usage = 0.0f;
		}
		else {
			monitor_info[loopcnt].cpu_usage = (100.0 * (measure_time - running_time)) / measure_time;
		}
	}

	return TRUE;
}


static VOID monitor_idle_max( FLOAT cpu_usage_max[D_DD_ARM_NUM_OF_CORES], const T_BF_IDLE_TASK_MONITOR_INFO monitor_info[D_DD_ARM_NUM_OF_CORES] )
{
	UINT32	loopcnt;

	for( loopcnt = 0; loopcnt < D_DD_ARM_NUM_OF_CORES; loopcnt++ ) {
		if( cpu_usage_max[loopcnt] < monitor_info[loopcnt].cpu_usage ) {
			cpu_usage_max[loopcnt] = monitor_info[loopcnt].cpu_usage;
		}
	}
}


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

VOID Monitor_Idle_Measure( UINT32 count, UINT32 measure_time )
{
	T_BF_IDLE_TASK_MONITOR_INFO	monitor_info[D_DD_ARM_NUM_OF_CORES] = {
		{ .core_valid = TRUE, .idle_task_id = TID_IDLE0, },
		{ .core_valid = TRUE, .idle_task_id = TID_IDLE1, },
#ifndef CO_RTOS_CPUx2
		{ .core_valid = TRUE, .idle_task_id = TID_IDLE2, },
#else
		{ .core_valid = FALSE, .idle_task_id = TID_IDLE2, },
#endif
	};
	FLOAT						cpu_usage_max[D_DD_ARM_NUM_OF_CORES];
	UINT32						loopcnt;

	if( count == 0 ) {
		count = 1;
	}
	if( measure_time == 0 ) {
		measure_time = 1000;
	}

	for( loopcnt = 0; loopcnt < D_DD_ARM_NUM_OF_CORES; loopcnt++ ) {
		cpu_usage_max[loopcnt] = 0.0f;
	}

	for( loopcnt = 0; loopcnt < count; loopcnt++ ) {
		BF_Debug_Print_Information(( "measure %u of %u (%u msec)\n", (loopcnt +1), count, measure_time ));
		monitor_idle_measure_main( measure_time, monitor_info );
		monitor_idle_max( cpu_usage_max, monitor_info );
	}

	for( loopcnt = 0; loopcnt < D_DD_ARM_NUM_OF_CORES; loopcnt++ ) {
		if( monitor_info[loopcnt].core_valid == FALSE ) {
			continue;
		}

		BF_Debug_Print_Information(( "Core%u: %5.1f%%\n", loopcnt, cpu_usage_max[loopcnt] ));
	}
}


VOID Monitor_Idle_Task( INT32 stacd )
{
	while(1) __asm__ __volatile__("WFE");
}

