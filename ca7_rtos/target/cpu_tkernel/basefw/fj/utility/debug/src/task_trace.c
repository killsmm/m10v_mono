/**
 * @file		task_trace.c
 * @brief		Current task information.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

// Utility header
#include "task_trace.h"
#include "debug.h"

// OS header
#include "os_user_custom.h"
#include "TKernel_id.h"

// Driver header
#include "ddim_typedef.h"
#include "dd_arm.h"

// STD-C header
#include <string.h>


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

#define D_TASK_TRACE_SYSTEM_TASK_NUM	(10)


/*==============================================================================*/
/* Macro declaration															*/
/*==============================================================================*/

#define bf_task_trace_tid_to_tbl_idx( tid )	((tid) & 0xffff)


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

// T_TASK_TRACE_CUR_INFO
typedef struct {
	OS_USER_ID	tid;
	BOOL		is_int;
} T_TASK_TRACE_CUR_INFO;

// T_TASK_TRACE_TASK_INFO
typedef struct {
	VOID*	stack_top;
	VOID*	stack_cur;
	VOID*	stack_bottom;
	VOID*	pc;
	VOID*	lr;
} T_TASK_TRACE_TASK_INFO;


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/

static volatile T_TASK_TRACE_CUR_INFO	g_bf_task_trace_cur_info[D_DD_ARM_NUM_OF_CORES];
static volatile T_TASK_TRACE_TASK_INFO	g_bf_task_trace_task_info[TID_NUM + D_TASK_TRACE_SYSTEM_TASK_NUM];
static volatile BOOL					g_bf_task_stack_filled = FALSE;


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/

static VOID bf_task_trace_tsk_exec_cb( OS_USER_ID tskid, INT32 lsid )
{
	register UINT32 cpuid;
	cpuid = Dd_ARM_Get_CPU_ID();

	g_bf_task_trace_cur_info[cpuid].tid = tskid;
}

static VOID bf_task_trace_tsk_stop_cb( OS_USER_ID tskid, INT32 lsid, UINT32 tskstat )
{
	UINT32			tbl_idx;
	TD_OS_USER_RTSK	t_rtsk;
	T_OS_USER_REGS	k_regs;
	T_OS_USER_EIT	k_eit;
	T_OS_USER_CREGS	k_cregs;

	if( OS_User_Td_Ref_Tsk( tskid, &t_rtsk ) != D_OS_USER_E_OK ) {
		return;
	}

	if( OS_User_Td_Get_Reg( tskid, &k_regs, &k_eit, &k_cregs ) != D_OS_USER_E_OK ) {
		return;
	}

	tbl_idx = bf_task_trace_tid_to_tbl_idx( tskid );
	if( tbl_idx >= (sizeof(g_bf_task_trace_task_info) / sizeof(g_bf_task_trace_task_info[0])) ) {
		return;
	}

	g_bf_task_trace_task_info[tbl_idx].pc = k_eit.pc;
	g_bf_task_trace_task_info[tbl_idx].lr = k_regs.lr;
	g_bf_task_trace_task_info[tbl_idx].stack_cur = k_cregs.ssp;
	if( ((VOID*)k_cregs.ssp) < (VOID*)(t_rtsk.isstack - t_rtsk.sstksz) ) {
		// Stack overflow!
		while( 1 ) {
			// error trap
			;
		}
	}
}

static VOID bf_task_trace_int_enter_cb( UINT32 intno )
{
	register UINT32 cpuid;
	cpuid = Dd_ARM_Get_CPU_ID();

	g_bf_task_trace_cur_info[cpuid].is_int = 1;
}

static VOID bf_task_trace_int_leave_cb( UINT32 intno )
{
	register UINT32 cpuid;
	cpuid = Dd_ARM_Get_CPU_ID();

	g_bf_task_trace_cur_info[cpuid].is_int = 0;
}

static VOID bf_task_trace_get_stack_area( VOID )
{
	UINT32			loopcnt;
	OS_USER_ID		tid_list[TID_NUM];
	OS_USER_INT		task_cnt;
	TD_OS_USER_RTSK	rtsk;
	UINT32			tbl_idx;
	OS_USER_ER		ercd;

	task_cnt = OS_User_Td_Lst_Tsk( tid_list, TID_NUM );

	if( task_cnt == 0 ) {
		BF_Debug_Print_Debug(( "OS_User_Td_Lst_Tsk() error or task not found. cnt=%d\n", task_cnt ));
		return;
	}

	if( task_cnt > (TID_NUM) ) {
		task_cnt = TID_NUM;
	}

	for( loopcnt = 0; loopcnt < task_cnt; loopcnt++ ) {
		ercd = OS_User_Td_Ref_Tsk( tid_list[loopcnt] , &rtsk );
		if( ercd != D_OS_USER_E_OK ) {
			BF_Debug_Print_Debug(( "OS_User_Ref_Tsk() error. %d\n", ercd ));
			return;
		}

		tbl_idx = bf_task_trace_tid_to_tbl_idx( tid_list[loopcnt] );
		if( tbl_idx >= (sizeof(g_bf_task_trace_task_info) / sizeof(g_bf_task_trace_task_info[0])) ) {
			continue;
		}

		g_bf_task_trace_task_info[tbl_idx].stack_top    = (VOID*)(rtsk.isstack - rtsk.sstksz);
		g_bf_task_trace_task_info[tbl_idx].stack_bottom = (VOID*)rtsk.isstack;
	}
}

static VOID bf_task_trace_fill_stack_area( const OS_USER_ID tskid )
{
	OS_USER_ER		ercd;
	TD_OS_USER_RTSK	rtsk;

	ercd = OS_User_Td_Ref_Tsk( tskid, &rtsk );
	if( ercd != D_OS_USER_E_OK ) {
//		BF_Debug_Print_Debug(( "OS_User_Ref_Tsk() error. %d\n", ercd ));
		return;
	}

	memset( (VOID*)(rtsk.isstack - rtsk.sstksz), '\xee', rtsk.sstksz );
}

static VOID bf_task_trace_fill_stack_area_all_task( VOID )
{
	UINT32			loopcnt;
	OS_USER_ID		tid_list[TID_NUM + D_TASK_TRACE_SYSTEM_TASK_NUM];
	OS_USER_INT		task_cnt;
	OS_USER_ER		ercd;
	OS_USER_ID		my_tid;

	ercd = OS_User_Get_Tid( &my_tid );
	if( ercd != D_OS_USER_E_OK ) {
//		BF_Debug_Print_Debug(( "OS_User_Get_Tid() error. %d\n", ercd ));
		return;
	}
	
	task_cnt = OS_User_Td_Lst_Tsk( tid_list, TID_NUM + D_TASK_TRACE_SYSTEM_TASK_NUM );

	if( task_cnt == 0 ) {
//		BF_Debug_Print_Debug(( "OS_User_Td_Lst_Tsk() error or task not found. cnt=%d\n", task_cnt ));
		return;
	}

	if( task_cnt > (TID_NUM + D_TASK_TRACE_SYSTEM_TASK_NUM) ) {
		task_cnt = TID_NUM + D_TASK_TRACE_SYSTEM_TASK_NUM;
	}

	for( loopcnt = 0; loopcnt < task_cnt; loopcnt++ ) {
		if( tid_list[loopcnt] == my_tid ) {
			continue;
		}

		bf_task_trace_fill_stack_area( tid_list[loopcnt] );
	}

	g_bf_task_stack_filled = TRUE;
}

static ULONG bf_task_trace_get_reserved_stack_bytes( const OS_USER_ID tskid )
{
	OS_USER_ER		ercd;
	TD_OS_USER_RTSK	rtsk;
	ULONG*			stack_top;
	ULONG			reserved_words;

	if( g_bf_task_stack_filled == FALSE ) {
		BF_Debug_Print_Debug(( "Stack not filled. tid=0x%x\n", tskid ));
		return 0xFFFFFFFFul;
	}

	ercd = OS_User_Td_Ref_Tsk( tskid, &rtsk );
	if( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Debug(( "OS_User_Ref_Tsk() error. %d tid=0x%x\n", ercd, tskid ));
		return 0xFFFFFFFFul;
	}

	stack_top = (ULONG*)(rtsk.isstack - rtsk.sstksz);
	for( reserved_words = 0; reserved_words < (rtsk.sstksz / sizeof(stack_top[0])); reserved_words++ ) {
		if( stack_top[reserved_words] != 0xEEEEEEEEul ) {
			return reserved_words * sizeof(stack_top[0]);
		}
	}

	// Task Not running.
	return 0xFFFFFFFFul;
}


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

VOID BF_TaskTrace_Enable_TskTrace( VOID )
{
	static TD_OS_USER_HDSP hdsp = {
		.exec = bf_task_trace_tsk_exec_cb,
		.stop = bf_task_trace_tsk_stop_cb,
	};
	static TD_OS_USER_HINT hint = {
		.enter = bf_task_trace_int_enter_cb,
		.leave = bf_task_trace_int_leave_cb,
	};
	OS_USER_ER	ercd;

	bf_task_trace_get_stack_area();

	ercd = OS_User_Td_Hok_Dsp( &hdsp );
	if( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Debug(( "%s: OS_User_Td_Hok_Dsp() error. %d\n", __func__, ercd ));
		return;
	}

	ercd = OS_User_Td_Hok_Int( &hint );
	if( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Debug(( "%s: OS_User_Td_Hok_Int() error. %d\n", __func__, ercd ));
		return;
	}
}


VOID BF_TaskTrace_Disable_TskTrace( VOID )
{
	OS_USER_ER	ercd;

	ercd = OS_User_Td_Hok_Dsp( NULL );
	if( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Debug(( "%s: OS_User_Td_Hok_Dsp() error. %d\n", __func__, ercd ));
		return;
	}

	ercd = OS_User_Td_Hok_Int( NULL );
	if( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Debug(( "%s: OS_User_Td_Hok_Int() error. %d\n", __func__, ercd ));
		return;
	}
}


VOID BF_TaskTrace_Fill_Stack( VOID )
{
	bf_task_trace_fill_stack_area_all_task();
}


ULONG BF_TaskTrace_Get_ReservedStackBytes( const OS_USER_ID tskid )
{
	return bf_task_trace_get_reserved_stack_bytes( tskid );
}

