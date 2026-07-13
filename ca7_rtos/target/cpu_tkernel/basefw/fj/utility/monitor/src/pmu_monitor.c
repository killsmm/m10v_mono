/**
 * @file		pmu_monitor.c
 * @brief		PMU Monitor
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifdef CO_PMU_MONITOR_ON
#include <string.h>
#include <stdlib.h>
#include "dd_arm.h"
#include "dd_tmr32.h"
#include "debug.h"
#include "fs_if.h"
#include "l1l2cache.h"
#include "measure_time.h"
#include "monitor.h"
#include "pmu.h"
#include "interrupt_set.h"
#include "dd_gic.h"
#include "os_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PMU_MONITOR_CPU_NUM			(3)
#define D_PMU_MONITOR_CPU0				(0)
#define D_PMU_MONITOR_CPU1				(1)
#define D_PMU_MONITOR_CPU2				(2)
#define D_PMU_MONITOR_TIMER_CH4			(4)		// Target:CPU0
#define D_PMU_MONITOR_TIMER_CH11		(11)	// Target:CPU1
#define D_PMU_MONITOR_TIMER_CH12		(12)	// Target:CPU2
#define D_PMU_MONITOR_INFO_LENGTH		(10)
#define D_PMU_MONITOR_INFO_COUNT		(1 << D_PMU_MONITOR_INFO_LENGTH)
#define D_PMU_MONITOR_INFO_COUNT_1		(D_PMU_MONITOR_INFO_COUNT - 1)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct{
	UINT64		cycle_count;
	UINT32		event_count[E_PMU_COUNTER_NUM_MAX];
	DOUBLE		idle_ratio;
} T_MONITOR_PMU_INFO;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
E_PMU_DIVIDER gMonitor_PMU_Divide_Flag = E_PMU_DIVIDER_NO_DIVIDE;
static volatile BOOL gMonitor_PMU_Get_Flag = FALSE;
static volatile BOOL gMonitor_PMU_Save_Flag = FALSE;
static volatile UINT32 gMonitor_PMU_Info_Count[D_PMU_MONITOR_CPU_NUM] = {0, 0, 0};
static volatile T_MONITOR_PMU_INFO gMonitor_PMU_Info[D_PMU_MONITOR_CPU_NUM][D_PMU_MONITOR_INFO_COUNT];
static volatile UINT32 gMonitor_Core_Clock = D_DD_TOP_UCLK40I*33/2;
static volatile UINT32 gMonitor_Timer_Cycle[D_PMU_MONITOR_CPU_NUM] = {0, 0, 0};
static volatile ULONG gMonitor_Last_Time[D_PMU_MONITOR_CPU_NUM] = {0, 0, 0};
static volatile UINT32 gMonitor_Event_Number[E_PMU_COUNTER_NUM_MAX] = {E_PMU_EVENT_SW_INCR, E_PMU_EVENT_SW_INCR, E_PMU_EVENT_SW_INCR, E_PMU_EVENT_SW_INCR};
static volatile BOOL gMonitor_cycle_flag;
static volatile E_PMU_COUNTER gMonitor_cfg_counter;
static volatile INT32 gMonitor_cfg_event;
static volatile E_PMU_COUNTER gMonitor_event_count;
static volatile BOOL gMonitor_save_result_flag;
static const BOOL g_bf_pmu_monitor_active_cpu[D_DD_ARM_NUM_OF_CORES] = {
	TRUE,
	TRUE,
	TRUE,
	FALSE,
};

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

static VOID bf_monitor_pmu_save_result(BOOL mode);
static VOID bf_monitor_pmu_stop(VOID);

static VOID monitor_pmuinfo_out(UINT32 cpuid, UCHAR type, INT32 file_id, UINT32 index)
{
	CHAR out_buf[100];
	ULONG written_size;

	if(type == 0){
		// to display
		printf("cycle u32  : %lu\n", (ULONG)((gMonitor_PMU_Info[cpuid][index].cycle_count & 0xFFFFFFFF00000000) >> 32));
		printf("cycle l32  : %lu\n", (ULONG)(gMonitor_PMU_Info[cpuid][index].cycle_count & 0x00000000FFFFFFFF));
		printf("0 Event#%d : %u\n", gMonitor_Event_Number[E_PMU_COUNTER_NUM0], gMonitor_PMU_Info[cpuid][index].event_count[E_PMU_COUNTER_NUM0]);
		printf("1 Event#%d : %u\n", gMonitor_Event_Number[E_PMU_COUNTER_NUM1], gMonitor_PMU_Info[cpuid][index].event_count[E_PMU_COUNTER_NUM1]);
		printf("2 Event#%d : %u\n", gMonitor_Event_Number[E_PMU_COUNTER_NUM2], gMonitor_PMU_Info[cpuid][index].event_count[E_PMU_COUNTER_NUM2]);
		printf("3 Event#%d : %u\n", gMonitor_Event_Number[E_PMU_COUNTER_NUM3], gMonitor_PMU_Info[cpuid][index].event_count[E_PMU_COUNTER_NUM3]);
#if CO_WAIT_FOR_INTERRUPT_ON
		printf("idle_ratio : %.5f\n", gMonitor_PMU_Info[cpuid][index].idle_ratio);
#endif
	}else{
		// to file
		snprintf(out_buf, sizeof(out_buf), "cycle u32  : %lu\n", (ULONG)((gMonitor_PMU_Info[cpuid][index].cycle_count & 0xFFFFFFFF00000000) >> 32));
		BF_Fs_If_Write(file_id, out_buf, strlen((CHAR*)out_buf), &written_size);
		snprintf(out_buf, sizeof(out_buf), "cycle l32  : %lu\n", (ULONG)(gMonitor_PMU_Info[cpuid][index].cycle_count & 0x00000000FFFFFFFF));
		BF_Fs_If_Write(file_id, out_buf, strlen((CHAR*)out_buf), &written_size);
		snprintf(out_buf, sizeof(out_buf), "0 Event#%d : %u\n", gMonitor_Event_Number[E_PMU_COUNTER_NUM0], gMonitor_PMU_Info[cpuid][index].event_count[E_PMU_COUNTER_NUM0]);
		BF_Fs_If_Write(file_id, out_buf, strlen((CHAR*)out_buf), &written_size);
		snprintf(out_buf, sizeof(out_buf), "1 Event#%d : %u\n", gMonitor_Event_Number[E_PMU_COUNTER_NUM1], gMonitor_PMU_Info[cpuid][index].event_count[E_PMU_COUNTER_NUM1]);
		BF_Fs_If_Write(file_id, out_buf, strlen((CHAR*)out_buf), &written_size);
		snprintf(out_buf, sizeof(out_buf), "2 Event#%d : %u\n", gMonitor_Event_Number[E_PMU_COUNTER_NUM2], gMonitor_PMU_Info[cpuid][index].event_count[E_PMU_COUNTER_NUM2]);
		BF_Fs_If_Write(file_id, out_buf, strlen((CHAR*)out_buf), &written_size);
		snprintf(out_buf, sizeof(out_buf), "3 Event#%d : %u\n", gMonitor_Event_Number[E_PMU_COUNTER_NUM3], gMonitor_PMU_Info[cpuid][index].event_count[E_PMU_COUNTER_NUM3]);
		BF_Fs_If_Write(file_id, out_buf, strlen((CHAR*)out_buf), &written_size);
#if CO_WAIT_FOR_INTERRUPT_ON
		snprintf(out_buf, sizeof(out_buf), "idle_ratio : %.5f\n", gMonitor_PMU_Info[cpuid][index].idle_ratio);
		BF_Fs_If_Write(file_id, out_buf, strlen((CHAR*)out_buf), &written_size);
#endif
	}
}

static VOID monitor_pmu_handler(VOID)
{
	if(gMonitor_PMU_Get_Flag){
		bf_monitor_pmu_save_result(0);
	}
}

static VOID bf_monitor_pmu_ctrl_cycle(BOOL flag)
{
	if(flag){
		BF_PMU_Ctrl_Cycle_Enable();
	}else{
		BF_PMU_Ctrl_Cycle_Disable();
	}
}

static VOID bf_monitor_pmu_init(VOID)
{
	UINT32 cpuid;
	
	cpuid = Dd_ARM_Get_CPU_ID();
	memset(&gMonitor_PMU_Info[cpuid], 0, sizeof(gMonitor_PMU_Info[cpuid]));
	gMonitor_PMU_Info_Count[cpuid] = 0;
	BF_PMU_Ctrl_Cycle_Disable();
	BF_PMU_Ctrl_Event_Set(E_PMU_COUNTER_NUM0, E_PMU_EVENT_SW_INCR);
	BF_PMU_Ctrl_Event_Set(E_PMU_COUNTER_NUM1, E_PMU_EVENT_SW_INCR);
	BF_PMU_Ctrl_Event_Set(E_PMU_COUNTER_NUM2, E_PMU_EVENT_SW_INCR);
	BF_PMU_Ctrl_Event_Set(E_PMU_COUNTER_NUM3, E_PMU_EVENT_SW_INCR);
	BF_PMU_Ctrl_Event_Disable(E_PMU_COUNTER_NUM0);
	BF_PMU_Ctrl_Event_Disable(E_PMU_COUNTER_NUM1);
	BF_PMU_Ctrl_Event_Disable(E_PMU_COUNTER_NUM2);
	BF_PMU_Ctrl_Event_Disable(E_PMU_COUNTER_NUM3);
	gMonitor_Event_Number[E_PMU_COUNTER_NUM0] = E_PMU_EVENT_SW_INCR;
	gMonitor_Event_Number[E_PMU_COUNTER_NUM1] = E_PMU_EVENT_SW_INCR;
	gMonitor_Event_Number[E_PMU_COUNTER_NUM2] = E_PMU_EVENT_SW_INCR;
	gMonitor_Event_Number[E_PMU_COUNTER_NUM3] = E_PMU_EVENT_SW_INCR;
	gMonitor_PMU_Get_Flag = FALSE;
	gMonitor_PMU_Save_Flag = FALSE;
}

static VOID bf_monitor_pmu_start(VOID)
{
	UINT32 cpuid;
	
	cpuid = Dd_ARM_Get_CPU_ID();
	gMonitor_Last_Time[cpuid] = BF_Get_Time_us();
	BF_PMU_Start(E_PMU_RESET_ALL_COUNTERS, gMonitor_PMU_Divide_Flag);
}

static VOID bf_monitor_pmu_stop(VOID)
{
	BF_PMU_Stop();
	gMonitor_PMU_Save_Flag = FALSE;
}

static VOID bf_monitor_pmu_save_result(BOOL mode)
{
	UINT64 cycle_count;
	UINT32 cpuid;
	
	cpuid = Dd_ARM_Get_CPU_ID();
	
	BF_PMU_Stop();
	
	if(mode){
		gMonitor_Timer_Cycle[cpuid] = BF_Get_Time_us() - gMonitor_Last_Time[cpuid];
	}
	
	if(gMonitor_PMU_Save_Flag || mode){		// exclude 1st period data when getting by timer.
		cycle_count = BF_PMU_Get_Cycle_Count() << (gMonitor_PMU_Divide_Flag * 6);
		gMonitor_PMU_Info[cpuid][gMonitor_PMU_Info_Count[cpuid]].cycle_count = cycle_count;
		gMonitor_PMU_Info[cpuid][gMonitor_PMU_Info_Count[cpuid]].event_count[E_PMU_COUNTER_NUM0] = BF_PMU_Get_Event_Count(E_PMU_COUNTER_NUM0);
		gMonitor_PMU_Info[cpuid][gMonitor_PMU_Info_Count[cpuid]].event_count[E_PMU_COUNTER_NUM1] = BF_PMU_Get_Event_Count(E_PMU_COUNTER_NUM1);
		gMonitor_PMU_Info[cpuid][gMonitor_PMU_Info_Count[cpuid]].event_count[E_PMU_COUNTER_NUM2] = BF_PMU_Get_Event_Count(E_PMU_COUNTER_NUM2);
		gMonitor_PMU_Info[cpuid][gMonitor_PMU_Info_Count[cpuid]].event_count[E_PMU_COUNTER_NUM3] = BF_PMU_Get_Event_Count(E_PMU_COUNTER_NUM3);
#if CO_WAIT_FOR_INTERRUPT_ON
		gMonitor_PMU_Info[cpuid][gMonitor_PMU_Info_Count[cpuid]].idle_ratio = 1 - (DOUBLE)cycle_count / (gMonitor_Core_Clock / 1000 / 1000 * gMonitor_Timer_Cycle[cpuid]);
#endif
		gMonitor_PMU_Info_Count[cpuid] = D_PMU_MONITOR_INFO_COUNT_1 & (gMonitor_PMU_Info_Count[cpuid] + 1);
	}else{
		gMonitor_PMU_Save_Flag = TRUE;
	}
	
	gMonitor_Last_Time[cpuid] = BF_Get_Time_us();
	BF_PMU_Start(E_PMU_RESET_ALL_COUNTERS, gMonitor_PMU_Divide_Flag);
}

static VOID bf_monitor_pmu_get_cycle_count(VOID)
{
	UINT32 count;
	UINT32 cpuid;
	
	cpuid = Dd_ARM_Get_CPU_ID();
	count = BF_PMU_Get_Cycle_Count();
	printf("--- CPU%d Cycle Count=%u\n", cpuid, count);
}

static VOID bf_monitor_pmu_get_event_count(E_PMU_COUNTER counter_num)
{
	UINT32 count;
	UINT32 cpuid;
	
	cpuid = Dd_ARM_Get_CPU_ID();
	count = BF_PMU_Get_Event_Count(counter_num);
	printf("--- CPU%d Event Count=%u\n", cpuid, count);
}

static VOID bf_monitor_pmu_start2(VOID)
{
	bf_monitor_pmu_init();
	bf_monitor_pmu_ctrl_cycle( TRUE );
	Monitor_PMU_Set_Timer_Getting_Flg( TRUE );
	bf_monitor_pmu_start();
}

static VOID bf_monitor_pmu_stop2(VOID)
{
	Monitor_PMU_Set_Timer_Getting_Flg( FALSE );
	bf_monitor_pmu_stop();
}

static VOID bf_monitor_pmu_set_counter(E_PMU_COUNTER counter_num, INT32 event_num)
{
	if(event_num >= 0){
		BF_PMU_Ctrl_Event_Set(counter_num, (E_PMU_EVENT)event_num);
		gMonitor_Event_Number[counter_num] = event_num;
	}else{
		BF_PMU_Ctrl_Event_Disable(counter_num);
		gMonitor_Event_Number[counter_num] = E_PMU_EVENT_SW_INCR;
	}
}

#ifndef CO_WAIT_FOR_INTERRUPT_ON
static VOID bf_monitor_pmu_cycle_init(VOID)
{
	BF_PMU_Init();
}

static VOID bf_monitor_pmu_cycle_start(VOID)
{
	BF_PMU_Start(E_PMU_RESET_CYCLE_COUNTER, gMonitor_PMU_Divide_Flag);
}

static VOID bf_monitor_pmu_get_cycle(VOID)
{
	UINT32 Count;
	UINT32 cpuid;
	
	cpuid = Dd_ARM_Get_CPU_ID();
	Count = BF_PMU_Get_Cycle_Count();
	printf("--- CPU%d PMU Cycle Count=%u\n", cpuid, Count);
}

static VOID bf_monitor_pmu_cycle_stop(VOID)
{
	BF_PMU_Stop();
}

static VOID bf_monitor_pmu_measure_time_start(VOID)
{
	BF_PMU_Start(E_PMU_RESET_CYCLE_COUNTER, gMonitor_PMU_Divide_Flag);
}

static VOID bf_monitor_pmu_measure_time_stop(VOID)
{
	UINT64 count;
	UINT32 cpuid;
	
	cpuid = Dd_ARM_Get_CPU_ID();
	BF_PMU_Stop();
	count = BF_PMU_Get_Cycle_Count() << (gMonitor_PMU_Divide_Flag * 6);
	
	printf("--- CPU%d PMU Cycle Measure Time=%u[usec]\n", cpuid, (UINT32)(count / ((D_DD_TOP_UCLK40I*33/2) / 1000 / 1000)));
}
#endif	// CO_WAIT_FOR_INTERRUPT_ON

static OS_USER_ER bf_monitor_pmu_get_fid( OS_USER_ID* const fid, UINT32 cpu_id )
{
	switch( cpu_id ) {
		case 0:
			*fid = FID_PMU0;
			return D_OS_USER_E_OK;
		case 1:
			*fid = FID_PMU1;
			return D_OS_USER_E_OK;
		case 2:
			*fid = FID_PMU2;
			return D_OS_USER_E_OK;
		default:
			return D_OS_USER_E_PAR;
	}
}

VOID bf_monitor_pmu_run_task_cmd( const UINT32 cpu_id, const OS_USER_FLGPTN flg )
{
	OS_USER_ID		fid;
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flgptn = 0;

	if( g_bf_pmu_monitor_active_cpu[cpu_id] == FALSE ) {
		return;
	}

	if( bf_monitor_pmu_get_fid( &fid, cpu_id ) != D_OS_USER_E_OK ) {
		printf( "%s() Unknown start code. %u\n", __func__, cpu_id );	/* pgr0822 */
		return;
	}

	ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_TASK_RESULT );
	if( ercd != D_OS_USER_E_OK ) {
		printf( "%s() ClrFlg error. %d\n", __func__, ercd );	/* pgr0822 */
	}
	ercd = OS_User_Set_Flg( fid, flg );
	if( ercd != D_OS_USER_E_OK ) {
		printf( "%s() SetFlg error. %d\n", __func__, ercd );	/* pgr0822 */
	}
	ercd = OS_User_Twai_Flg( fid, FLG_MONITOR_PMU_CONTROLLER_TASK_RESULT, D_OS_USER_TWF_ORW, &flgptn, 1000 );
	if( ercd != D_OS_USER_E_OK ) {
		printf( "%s() WaiFlg error. %d\n", __func__, ercd );	/* pgr0822 */
	}
}


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
VOID Monitor_PMU_Controller(INT32 stacd)
{
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flg = 0;
	OS_USER_FLGPTN	flgptn = 0;
	OS_USER_ID		fid;

	if( bf_monitor_pmu_get_fid( &fid, stacd ) != D_OS_USER_E_OK ) {
		printf( "%s() Unknown start code. %u\n", __func__, stacd );	/* pgr0822 */
		OS_User_Ext_Tsk();
		return;
	}

	flg = FLG_MONITOR_PMU_CONTROLLER_INIT;
	flg |= FLG_MONITOR_PMU_CONTROLLER_CYCLE;
	flg |= FLG_MONITOR_PMU_CONTROLLER_CFG;
	flg |= FLG_MONITOR_PMU_CONTROLLER_START;
	flg |= FLG_MONITOR_PMU_CONTROLLER_STOP;
	flg |= FLG_MONITOR_PMU_CONTROLLER_GET;
	flg |= FLG_MONITOR_PMU_CONTROLLER_GET_CYCLE;
	flg |= FLG_MONITOR_PMU_CONTROLLER_GET_EVENT_COUNT;
	flg |= FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_INIT;
	flg |= FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_START;
	flg |= FLG_MONITOR_PMU_CONTROLLER_NOTWFI_GET_CYCLE;
	flg |= FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_STOP;
	flg |= FLG_MONITOR_PMU_CONTROLLER_NOTWFI_TIME_START;
	flg |= FLG_MONITOR_PMU_CONTROLLER_NOTWFI_TIME_STOP;
	flg |= FLG_MONITOR_PMU_CONTROLLER_START2;
	flg |= FLG_MONITOR_PMU_CONTROLLER_STOP2;
	ercd = OS_User_Clr_Flg( fid, ~flg );
	for(;;){
		ercd = OS_User_Wai_Flg( fid, flg, D_OS_USER_TWF_ORW, &flgptn);
		if( ercd != D_OS_USER_E_OK ) {
			BF_Debug_Print_Error(("[Monitor_PMU_Controller]wai_flg error(%d) L=%d\n", ercd, __LINE__));
		}
		if (flgptn & FLG_MONITOR_PMU_CONTROLLER_INIT) {
			ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_INIT );
			bf_monitor_pmu_init();
		}
		if (flgptn & FLG_MONITOR_PMU_CONTROLLER_CYCLE) {
			ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_CYCLE );
			bf_monitor_pmu_ctrl_cycle(gMonitor_cycle_flag);
		}
		if (flgptn & FLG_MONITOR_PMU_CONTROLLER_CFG) {
			ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_CFG );
			bf_monitor_pmu_set_counter(gMonitor_cfg_counter, gMonitor_cfg_event);
		}
		if (flgptn & FLG_MONITOR_PMU_CONTROLLER_START) {
			ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_START );
			bf_monitor_pmu_start();
		}
		if (flgptn & FLG_MONITOR_PMU_CONTROLLER_STOP) {
			ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_STOP );
			bf_monitor_pmu_stop();
		}
		if (flgptn & FLG_MONITOR_PMU_CONTROLLER_GET) {
			ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_GET );
			bf_monitor_pmu_save_result(gMonitor_save_result_flag);
		}
		if (flgptn & FLG_MONITOR_PMU_CONTROLLER_GET_CYCLE) {
			ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_GET_CYCLE );
			bf_monitor_pmu_get_cycle_count();
		}
		if (flgptn & FLG_MONITOR_PMU_CONTROLLER_GET_EVENT_COUNT) {
			ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_GET_EVENT_COUNT );
			bf_monitor_pmu_get_event_count(gMonitor_event_count);
		}
#ifndef CO_WAIT_FOR_INTERRUPT_ON
		{
			if (flgptn & FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_INIT) {
				ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_INIT );
				bf_monitor_pmu_cycle_init();
			}
			if (flgptn & FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_START) {
				ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_START );
				bf_monitor_pmu_cycle_start();
			}
			if (flgptn & FLG_MONITOR_PMU_CONTROLLER_NOTWFI_GET_CYCLE) {
				ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_NOTWFI_GET_CYCLE );
				bf_monitor_pmu_get_cycle();
			}
			if (flgptn & FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_STOP) {
				ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_STOP );
				bf_monitor_pmu_cycle_stop();
			}
			if (flgptn & FLG_MONITOR_PMU_CONTROLLER_NOTWFI_TIME_START) {
				ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_NOTWFI_TIME_START );
				bf_monitor_pmu_measure_time_start();
			}
			if (flgptn & FLG_MONITOR_PMU_CONTROLLER_NOTWFI_TIME_STOP) {
				ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_NOTWFI_TIME_STOP );
				bf_monitor_pmu_measure_time_stop();
			}
		}
#endif
		if (flgptn & FLG_MONITOR_PMU_CONTROLLER_START2) {
			ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_START2 );
			bf_monitor_pmu_start2();
		}
		if (flgptn & FLG_MONITOR_PMU_CONTROLLER_STOP2) {
			ercd = OS_User_Clr_Flg( fid, ~FLG_MONITOR_PMU_CONTROLLER_STOP2 );
			bf_monitor_pmu_stop2();
		}

		OS_User_Set_Flg( fid, FLG_MONITOR_PMU_CONTROLLER_TASK_RESULT );
	}
	OS_User_Ext_Tsk();
}

VOID Monitor_PMU_Set_Timer_Getting_Flg(BOOL flg)
{
	gMonitor_PMU_Get_Flag = flg;
}

INT32 Monitor_PMU_Start_Timer(UINT32 usec)
{
	INT32 ret;
	
	Interrupt_ReSet_Enable(E_DD_GIC_INTID_TIMER_CH0_INT + D_PMU_MONITOR_TIMER_CH4, D_DD_GIC_PRI15, D_OS_USER_TP_PRC1);
	Interrupt_ReSet_Enable(E_DD_GIC_INTID_TIMER_CH0_INT + D_PMU_MONITOR_TIMER_CH11, D_DD_GIC_PRI15, D_OS_USER_TP_PRC2);
	Interrupt_ReSet_Enable(E_DD_GIC_INTID_TIMER_CH0_INT + D_PMU_MONITOR_TIMER_CH12, D_DD_GIC_PRI15, D_OS_USER_TP_PRC3);
	
	ret = Dd_TMR32_Open(D_PMU_MONITOR_TIMER_CH4, D_DDIM_USER_SEM_WAIT_POL);
	if(ret != D_DDIM_OK){
		return ret;
	}
	ret = Dd_TMR32_Open(D_PMU_MONITOR_TIMER_CH11, D_DDIM_USER_SEM_WAIT_POL);
	if(ret != D_DDIM_OK){
		Dd_TMR32_Close(D_PMU_MONITOR_TIMER_CH4);
		return ret;
	}
	ret = Dd_TMR32_Open(D_PMU_MONITOR_TIMER_CH12, D_DDIM_USER_SEM_WAIT_POL);
	if(ret != D_DDIM_OK){
		Dd_TMR32_Close(D_PMU_MONITOR_TIMER_CH4);
		Dd_TMR32_Close(D_PMU_MONITOR_TIMER_CH11);
		return ret;
	}
	if(Dd_TMR32_SetTimer(D_PMU_MONITOR_TIMER_CH4, usec, monitor_pmu_handler) == D_DDIM_OK){
		Dd_TMR32_StartReload(D_PMU_MONITOR_TIMER_CH4);
	}

	if(Dd_TMR32_SetTimer(D_PMU_MONITOR_TIMER_CH11, usec, monitor_pmu_handler) == D_DDIM_OK){
		Dd_TMR32_StartReload(D_PMU_MONITOR_TIMER_CH11);
	}

	if(Dd_TMR32_SetTimer(D_PMU_MONITOR_TIMER_CH12, usec, monitor_pmu_handler) == D_DDIM_OK){
		Dd_TMR32_StartReload(D_PMU_MONITOR_TIMER_CH12);
	}
	gMonitor_Timer_Cycle[D_PMU_MONITOR_CPU0] = usec;
	gMonitor_Timer_Cycle[D_PMU_MONITOR_CPU1] = usec;
	gMonitor_Timer_Cycle[D_PMU_MONITOR_CPU2] = usec;
	
	return ret;
}

VOID Monitor_PMU_Stop_Timer(VOID)
{
	Monitor_PMU_Set_Timer_Getting_Flg(FALSE);
	Dd_TMR32_Stop(D_PMU_MONITOR_TIMER_CH4);
	Dd_TMR32_Close(D_PMU_MONITOR_TIMER_CH4);
	Dd_TMR32_Stop(D_PMU_MONITOR_TIMER_CH11);
	Dd_TMR32_Close(D_PMU_MONITOR_TIMER_CH11);
	Dd_TMR32_Stop(D_PMU_MONITOR_TIMER_CH12);
	Dd_TMR32_Close(D_PMU_MONITOR_TIMER_CH12);
}

VOID Monitor_PMU_Ctrl_Cycle(BOOL flag)
{
	gMonitor_cycle_flag = 1;
	for( UINT32 cpu_id = 0; cpu_id < D_DD_ARM_NUM_OF_CORES; cpu_id++ ) {
		bf_monitor_pmu_run_task_cmd( cpu_id, FLG_MONITOR_PMU_CONTROLLER_CYCLE );
	}
}

VOID Monitor_PMU_Set_Counter(E_PMU_COUNTER counter_num, INT32 event_num)
{
	gMonitor_cfg_counter = counter_num;
	gMonitor_cfg_event = event_num;
	for( UINT32 cpu_id = 0; cpu_id < D_DD_ARM_NUM_OF_CORES; cpu_id++ ) {
		bf_monitor_pmu_run_task_cmd( cpu_id, FLG_MONITOR_PMU_CONTROLLER_CFG );
	}
}

VOID Monitor_PMU_Set_Divider(UINT32 div_flg)
{
	gMonitor_PMU_Divide_Flag = (E_PMU_DIVIDER)div_flg;
}

VOID Monitor_PMU_Init(VOID)
{
	for( UINT32 cpu_id = 0; cpu_id < D_DD_ARM_NUM_OF_CORES; cpu_id++ ) {
		bf_monitor_pmu_run_task_cmd( cpu_id, FLG_MONITOR_PMU_CONTROLLER_INIT );
	}
}

VOID Monitor_PMU_Start(VOID)
{
	for( UINT32 cpu_id = 0; cpu_id < D_DD_ARM_NUM_OF_CORES; cpu_id++ ) {
		bf_monitor_pmu_run_task_cmd( cpu_id, FLG_MONITOR_PMU_CONTROLLER_START );
	}
}

VOID Monitor_PMU_Stop(VOID)
{
	for( UINT32 cpu_id = 0; cpu_id < D_DD_ARM_NUM_OF_CORES; cpu_id++ ) {
		bf_monitor_pmu_run_task_cmd( cpu_id, FLG_MONITOR_PMU_CONTROLLER_STOP );
	}
}

VOID Monitor_PMU_Save_Result(BOOL mode)
{
	gMonitor_save_result_flag = mode;
	for( UINT32 cpu_id = 0; cpu_id < D_DD_ARM_NUM_OF_CORES; cpu_id++ ) {
		bf_monitor_pmu_run_task_cmd( cpu_id, FLG_MONITOR_PMU_CONTROLLER_GET );
	}
}

VOID Monitor_PMU_Output_Result(UCHAR type, const CHAR* file_name)
{
	INT32 file_id = 0;
	CHAR file_path[16] = "I:\\";
	UINT32 index;
	CHAR out_buf[100];
	ULONG written_size;
	UINT32 cpuid;
	
	// Log file open
	if(type == 1){
		// Parameter check
		if((file_name == NULL) || (strlen((CHAR*)file_name) > 8)){
			printf("Monitor_PMU_Result_Output() File name error!\n");
			return;
		}
		sprintf((CHAR*)&file_path[3], "%s.CSV", (CHAR*)file_name);
		file_id = BF_Fs_If_Create(file_path, D_BF_FS_IF_ATTR_NORMAL);
		if(file_id == 0){
			printf("Monitor_PMU_Result_Output() FSIF_Create %s failed! (%d)\n", file_path, file_id);
			return;
		}
	}
	
	for(cpuid=D_PMU_MONITOR_CPU0; cpuid<=D_PMU_MONITOR_CPU2; cpuid++){
	// Log data output
		snprintf(out_buf, sizeof(out_buf), "CPU%d:\n", cpuid);
		if(type == 0){
			// to terminal
			printf((CHAR*)out_buf);
		}else{
			// to file
			BF_Fs_If_Write(file_id, out_buf, strlen((CHAR*)out_buf), &written_size);
		}
		if(gMonitor_PMU_Info[cpuid][gMonitor_PMU_Info_Count[cpuid]].cycle_count){
			for(index = gMonitor_PMU_Info_Count[cpuid]; index < D_PMU_MONITOR_INFO_COUNT; index++){
				monitor_pmuinfo_out(cpuid, type, file_id, index);
			}
		}
		for(index = 0; index < gMonitor_PMU_Info_Count[cpuid]; index++){
			monitor_pmuinfo_out(cpuid, type, file_id, index);
		}
		
	}
	// Log file close
	if(type == 1){
		BF_Fs_If_Close(file_id);
	}
}

VOID Monitor_PMU_Disp_Idle_Ratio(VOID)
{
	UINT32 index = 0;
	UINT32 data_num;
	DOUBLE total_idle_ratio;
	UINT32 cpuid;
	
	for(cpuid=D_PMU_MONITOR_CPU0; cpuid<=D_PMU_MONITOR_CPU2; cpuid++){
		data_num = 0;
		total_idle_ratio = 0;
		if(gMonitor_PMU_Info[cpuid][gMonitor_PMU_Info_Count[cpuid]].cycle_count){
			for(index = gMonitor_PMU_Info_Count[cpuid]; index < D_PMU_MONITOR_INFO_COUNT; index++){
				total_idle_ratio += gMonitor_PMU_Info[cpuid][index].idle_ratio;
				data_num++;
			}
		}

		for(index = 0; index < gMonitor_PMU_Info_Count[cpuid]; index++){
			total_idle_ratio += gMonitor_PMU_Info[cpuid][index].idle_ratio;
			data_num++;
		}

		if(data_num > 0){
			total_idle_ratio = total_idle_ratio / data_num;
			printf("--- CPU%d PMU Idle_Ratio[%.5f] (%.2f %%)\n", cpuid, total_idle_ratio, total_idle_ratio *100 );
			printf("--- CPU%d PMU Load_Ratio[%.5f] (%.2f %%)\n", cpuid, (1 - total_idle_ratio), (1 - total_idle_ratio) *100 );
		}
	}
}

VOID Monitor_PMU_Disp_Cpu_Usage(VOID)
{
	UINT32 index = 0;
	UINT32 data_num;
	DOUBLE total_idle_ratio;
	UINT32 cpuid;
	
	for(cpuid=D_PMU_MONITOR_CPU0; cpuid<=D_PMU_MONITOR_CPU2; cpuid++){
		data_num = 0;
		total_idle_ratio = 0;
		if(gMonitor_PMU_Info[cpuid][gMonitor_PMU_Info_Count[cpuid]].cycle_count){
			for(index = gMonitor_PMU_Info_Count[cpuid]; index < D_PMU_MONITOR_INFO_COUNT; index++){
				total_idle_ratio += gMonitor_PMU_Info[cpuid][index].idle_ratio;
				data_num++;
			}
		}

		for(index = 0; index < gMonitor_PMU_Info_Count[cpuid]; index++){
			total_idle_ratio += gMonitor_PMU_Info[cpuid][index].idle_ratio;
			data_num++;
		}

		if(data_num > 0){
			total_idle_ratio = total_idle_ratio / data_num;
			printf("--- CPU%d PMU CPU Usage[%03.2f %%]\n", cpuid, 100 - (total_idle_ratio*100) );
		}
	}
}

VOID Monitor_PMU_Get_Cycle_Count(VOID)
{
	for( UINT32 cpu_id = 0; cpu_id < D_DD_ARM_NUM_OF_CORES; cpu_id++ ) {
		bf_monitor_pmu_run_task_cmd( cpu_id, FLG_MONITOR_PMU_CONTROLLER_GET_CYCLE );
	}
}

VOID Monitor_PMU_Get_Event_Count(E_PMU_COUNTER counter_num)
{
	gMonitor_event_count = counter_num;
	for( UINT32 cpu_id = 0; cpu_id < D_DD_ARM_NUM_OF_CORES; cpu_id++ ) {
		bf_monitor_pmu_run_task_cmd( cpu_id, FLG_MONITOR_PMU_CONTROLLER_GET_EVENT_COUNT );
	}
}

VOID Monitor_PMU_Get_Event_Number(E_PMU_COUNTER counter_num)
{
	if(counter_num < E_PMU_COUNTER_NUM_MAX){
		printf("--- Event Number=%u\n", gMonitor_Event_Number[counter_num]);
	}
}

VOID Monitor_PMU_Clear_Result(VOID)
{
	memset(&gMonitor_PMU_Info[D_PMU_MONITOR_CPU0], 0, sizeof(gMonitor_PMU_Info[D_PMU_MONITOR_CPU0]));
	memset(&gMonitor_PMU_Info[D_PMU_MONITOR_CPU1], 0, sizeof(gMonitor_PMU_Info[D_PMU_MONITOR_CPU1]));
	memset(&gMonitor_PMU_Info[D_PMU_MONITOR_CPU2], 0, sizeof(gMonitor_PMU_Info[D_PMU_MONITOR_CPU2]));
	gMonitor_PMU_Info_Count[D_PMU_MONITOR_CPU0] = 0;
	gMonitor_PMU_Info_Count[D_PMU_MONITOR_CPU1] = 0;
	gMonitor_PMU_Info_Count[D_PMU_MONITOR_CPU2] = 0;
}

VOID Monitor_PMU_Start2(VOID)
{
	for( UINT32 cpu_id = 0; cpu_id < D_DD_ARM_NUM_OF_CORES; cpu_id++ ) {
		bf_monitor_pmu_run_task_cmd( cpu_id, FLG_MONITOR_PMU_CONTROLLER_START2 );
	}
}

VOID Monitor_PMU_Stop2(VOID)
{
	for( UINT32 cpu_id = 0; cpu_id < D_DD_ARM_NUM_OF_CORES; cpu_id++ ) {
		bf_monitor_pmu_run_task_cmd( cpu_id, FLG_MONITOR_PMU_CONTROLLER_STOP2 );
	}
}

#ifndef CO_WAIT_FOR_INTERRUPT_ON
VOID Monitor_PMU_Cycle_Init(VOID)
{
	for( UINT32 cpu_id = 0; cpu_id < D_DD_ARM_NUM_OF_CORES; cpu_id++ ) {
		bf_monitor_pmu_run_task_cmd( cpu_id, FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_INIT );
	}
}

VOID Monitor_PMU_Cycle_Start(VOID)
{
	for( UINT32 cpu_id = 0; cpu_id < D_DD_ARM_NUM_OF_CORES; cpu_id++ ) {
		bf_monitor_pmu_run_task_cmd( cpu_id, FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_START );
	}
}

VOID Monitor_PMU_Get_Cycle(VOID)
{
	for( UINT32 cpu_id = 0; cpu_id < D_DD_ARM_NUM_OF_CORES; cpu_id++ ) {
		bf_monitor_pmu_run_task_cmd( cpu_id, FLG_MONITOR_PMU_CONTROLLER_NOTWFI_GET_CYCLE );
	}
}

VOID Monitor_PMU_Cycle_Stop(VOID)
{
	for( UINT32 cpu_id = 0; cpu_id < D_DD_ARM_NUM_OF_CORES; cpu_id++ ) {
		bf_monitor_pmu_run_task_cmd( cpu_id, FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_STOP );
	}
}

VOID Monitor_PMU_Measure_Time_Start(VOID)
{
	BF_PMU_Start(E_PMU_RESET_CYCLE_COUNTER, gMonitor_PMU_Divide_Flag);
}

VOID Monitor_PMU_Measure_Time_Stop(VOID)
{
	UINT64 count;
	UINT32 cpuid;
	
	cpuid = Dd_ARM_Get_CPU_ID();
	BF_PMU_Stop();
	count = BF_PMU_Get_Cycle_Count() << (gMonitor_PMU_Divide_Flag * 6);
	
	printf("--- CPU%d PMU Cycle Measure Time=%u[usec]\n", cpuid, (UINT32)(count / ((D_DD_TOP_UCLK40I*33/2) / 1000 / 1000)));
}

#endif	// CO_WAIT_FOR_INTERRUPT_ON
#endif	// CO_PMU_MONITOR_ON
