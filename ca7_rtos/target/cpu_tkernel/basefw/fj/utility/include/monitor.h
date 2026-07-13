/**
 * @file		monitor.h
 * @brief		Monitor
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _MONITOR_H
#define _MONITOR_H

#include "ddim_typedef.h"
#include "im_mxic.h"
#include "im_sdramc.h"
#include "pmu.h"
#include "fj_monitor.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* FID_PMU */
// Input
#define FLG_MONITOR_PMU_CONTROLLER_INIT					(0x00000001UL)
#define FLG_MONITOR_PMU_CONTROLLER_CYCLE				(0x00000002UL)
#define FLG_MONITOR_PMU_CONTROLLER_CFG					(0x00000004UL)
#define FLG_MONITOR_PMU_CONTROLLER_START				(0x00000008UL)
#define FLG_MONITOR_PMU_CONTROLLER_STOP					(0x00000010UL)
#define FLG_MONITOR_PMU_CONTROLLER_GET					(0x00000020UL)
#define FLG_MONITOR_PMU_CONTROLLER_GET_CYCLE			(0x00000040UL)
#define FLG_MONITOR_PMU_CONTROLLER_GET_EVENT_COUNT		(0x00000080UL)
#define FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_INIT	(0x00000100UL)
#define FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_START	(0x00000200UL)
#define FLG_MONITOR_PMU_CONTROLLER_NOTWFI_GET_CYCLE		(0x00000400UL)
#define FLG_MONITOR_PMU_CONTROLLER_NOTWFI_CYCLE_STOP	(0x00000800UL)
#define FLG_MONITOR_PMU_CONTROLLER_NOTWFI_TIME_START	(0x00001000UL)
#define FLG_MONITOR_PMU_CONTROLLER_NOTWFI_TIME_STOP		(0x00002000UL)
#define FLG_MONITOR_PMU_CONTROLLER_START2				(0x00004000UL)
#define FLG_MONITOR_PMU_CONTROLLER_STOP2				(0x00008000UL)
#define FLG_MONITOR_PMU_CONTROLLER_TASK_RESULT			(0x80000000UL)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
typedef struct{
	T_IM_MXIC_MONITOR_TARGET	target[D_IM_MXIC_MAX_MONITOR_TARGET_NUM];	/**< Information monitor target */
} T_MONITOR_MXIC_PARAMETER;

// T_CMD_CT_SDRAMC_BG_TASK_LOG
typedef struct {
	ULONG				time_us;
	T_IM_SDRAMC_MONITOR	sdramc_mon;
} T_CMD_CT_SDRAMC_BG_TASK_LOG;

#define D_CMD_CT_SDRAMC_BG_TASK_LOG_CYCLE_MSEC	(50)
#define D_CMD_CT_SDRAMC_BG_TASK_LOG_1SEC_COUNT	(1000 / D_CMD_CT_SDRAMC_BG_TASK_LOG_CYCLE_MSEC)
#define D_CMD_CT_SDRAMC_BG_TASK_LOG_NUM			(D_CMD_CT_SDRAMC_BG_TASK_LOG_1SEC_COUNT * 10)	// 10 sec

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern E_PMU_DIVIDER gMonitor_PMU_Divide_Flag;

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

VOID fj_monitor_mxic_ctrl_on(VOID);
VOID fj_monitor_mxic_ctrl_off(VOID);
FJ_ERR_CODE fj_monitor_mxic_config(FJ_MONITOR_MXIC_CFG* cfg);
VOID fj_monitor_mxic_start(VOID);
VOID fj_monitor_mxic_stop(FJ_MONITOR_MXIC_RESULT* mxic_result, UINT64* const measure_time);

VOID fj_monitor_sdramc_start(FJ_MONITOR_SDRAMC_CH sdramc_ch, FJ_MONITOR_SDRAMC_MONITOR_TYPE monitor_type, FJ_MONITOR_SDRAMC_ACCESS_TYPE access_type);
VOID fj_monitor_sdramc_stop(FJ_MONITOR_SDRAMC_CH sdramc_ch);
FJ_ERR_CODE fj_monitor_sdramc_get_result(FJ_MONITOR_SDRAMC_CH sdramc_ch, FJ_MONITOR_SDRAMC_RESULT* sdramc_result);

VOID fj_monitor_pmu_ctrl_cycle(BOOL flag);
VOID fj_monitor_pmu_ctrl_event(FJ_MONITOR_PMU_COUNTER counter_num, FJ_MONITOR_PMU_EVENT event_num);
VOID fj_monitor_pmu_start(FJ_MONITOR_PMU_RESET reset_counter, FJ_MONITOR_PMU_DIVIDER cycle_divider);
VOID fj_monitor_pmu_stop(VOID);
UINT32 fj_monitor_pmu_get_cycle_count(VOID);
UINT32 fj_monitor_pmu_get_event_count(FJ_MONITOR_PMU_COUNTER counter_num);

#ifdef CO_MXIC_MONITOR_ON
/**
 * MXIC Set Flag for Getting by Timer.
 * @param[in]	flg			: Enabled getting by timer
 */
VOID Monitor_MXIC_Set_Timer_Getting_Flg(BOOL flg);

/**
 * MXIC Reload Timer Start.
 * @param[in]	usec		: Reload time[usec]
 * @return		result
 */
INT32 Monitor_MXIC_Start_Timer(UINT32 usec);

/**
 * MXIC Reload Timer Stop.
 */
VOID Monitor_MXIC_Stop_Timer(VOID);

/**
 * MXIC Monitor Start.
 */
VOID Monitor_MXIC_Start(VOID);

/**
 * MXIC Monitor Stop.
 */
VOID Monitor_MXIC_Stop(VOID);

/**
 * MXIC Monitor result Save.
 */
VOID Monitor_MXIC_Save_Result(VOID);

/**
 * MXIC Monitor result Output.
 * @param[in]	type		: Monitor type
 * @param[in]	file_name	: Log file name
 */
VOID Monitor_MXIC_Output_Result(UCHAR type, const CHAR* file_name);

/**
 * MXIC Monitor result Clear.
 */
VOID Monitor_MXIC_Clear_Result(VOID);

/**
 * MXIC Set Parameter.
 * @param[in]	target_cnt	: Target count
 * @param[in]	*param		: Parameter information
 */
VOID Monitor_MXIC_Set_Parameter(INT32 target_cnt, T_MONITOR_MXIC_PARAMETER* param);

/**
 * Initialize BandWidth info Monitoring
 */
VOID Monitor_MXIC_BandWidth_Init(VOID);

/**
 * Start DRAM BandWidth info Monitoring
 * @param[in]	ch			: MXIC channel
 */
VOID Monitor_MXIC_BandWidth_Start(UCHAR ch);

/**
 * Stop DRAM BandWidth info Monitoring
 */
VOID Monitor_MXIC_BandWidth_Stop(VOID);

/**
 * Output DRAM BandWidth info Monitoring Result
 */
VOID Monitor_MXIC_BandWidth_Output_Info(VOID);

/**
 * Get DRAM BandWidth info Monitoring Result
 */
VOID* Monitor_MXIC_BandWidth_Get_Info(VOID);
#endif

#ifdef CO_PMU_MONITOR_ON
/**
 * PMU Monitor Set Flag for Getting by Timer.
 * @param[in]	flg			: Enabled getting by timer
 */
VOID Monitor_PMU_Set_Timer_Getting_Flg(BOOL flg);

/**
 * PMU Monitor Timer Start.
 * @param[in]	usec		: Reload time[usec]
 * @return		result
 */
INT32 Monitor_PMU_Start_Timer(UINT32 usec);

/**
 * PMU Monitor Timer Stop.
 */
VOID Monitor_PMU_Stop_Timer(VOID);

/**
 * PMU Monitor Set Parameter.
 * @param[in]	counter_num	: Counter number
 * @param[in]	event_num	: Event number
 */
VOID Monitor_PMU_Set_Counter(E_PMU_COUNTER counter_num, INT32 event_num);

/**
 * PMU Monitor Cycle Counter Control.
 */
VOID Monitor_PMU_Ctrl_Cycle(BOOL cycle);

/**
PMU Monitor Set Divide Flag.
*/
VOID Monitor_PMU_Set_Divider(UINT32 div_flg);

/**
 * PMU Monitor Initialize.
 */
VOID Monitor_PMU_Init(VOID);

/**
 * PMU Monitor Start.
 */
VOID Monitor_PMU_Start(VOID);

/**
 * PMU Monitor Stop.
 */
VOID Monitor_PMU_Stop(VOID);

/**
 * PMU Monitor Result Save.
 */
VOID Monitor_PMU_Save_Result(BOOL mode);

/**
 * PMU Monitor Result Out.
 * @param[in]	type		: Monitor type
 * @param[in]	file_name	: Log file name
 */
VOID Monitor_PMU_Output_Result(UCHAR type, const CHAR* file_name);

/**
 * PMU Monitor Display Idle Ratio.
 */
VOID Monitor_PMU_Disp_Idle_Ratio(VOID);

/**
 * PMU Monitor Display CPU Usage.
 */
VOID Monitor_PMU_Disp_Cpu_Usage(VOID);

/**
 * PMU Monitor Get Cycle Count.
 */
VOID Monitor_PMU_Get_Cycle_Count(VOID);

/**
 * PMU Monitor Get Event Count.
 */
VOID Monitor_PMU_Get_Event_Count(E_PMU_COUNTER counter_num);
/**
 * PMU Monitor Get Event Number.
 */
VOID Monitor_PMU_Get_Event_Number(E_PMU_COUNTER counter_num);

/**
 * PMU Monitor Result Clear.
 */
VOID Monitor_PMU_Clear_Result(VOID);

/**
 * PMU Monitor Start2.
 */
VOID Monitor_PMU_Start2(VOID);

/**
 * PMU Monitor Stop2.
 */
VOID Monitor_PMU_Stop2(VOID);

#ifndef CO_WAIT_FOR_INTERRUPT_ON
VOID Monitor_PMU_Cycle_Init(VOID);
VOID Monitor_PMU_Cycle_Start(VOID);
VOID Monitor_PMU_Get_Cycle(VOID);
VOID Monitor_PMU_Cycle_Stop(VOID);
VOID Monitor_PMU_Measure_Time_Start(VOID);
VOID Monitor_PMU_Measure_Time_Stop(VOID);
#endif
#endif

#ifdef CO_SDRAMC_MONITOR_ON

/**
 * SDRAMC Monitor Start.
 * @param[in]	ch			: SDRAMC Ch
 * @param[in]	type		: SDRAMC Monitor type
 * @param[in]	write		: Write access flag(TRUE:measurement on, FALSE:measurement off).
 * @param[in]	read		: Read  access flag(TRUE:measurement on, FALSE:measurement off).
 * @param[in]	update_mode	: Update mode
 * @note	update_mode=TRUE	: Arbitration access monitor update mode is clear mode.<br>
 *			update_mode=FALSE	: Arbitration access monitor update mode is keep mode.<br>
 */
VOID Monitor_SDRAMC_Start(E_IM_SDRAMC_CH ch, E_IM_SDRAMC_MONMODE type, BOOL write, BOOL read, BOOL update_mode);

/**
 * SDRAMC Monitor Stop.
 * @param[out]	sdramc_mon
 * @return		result
 */
INT32 Monitor_SDRAMC_Stop(T_IM_SDRAMC_MONITOR *sdramc_mon);

/**
 * SDRAMC Set Flag of Getting by Timer.
 * @param[in]	flg			: Enabled getting by timer
 */
VOID Monitor_SDRAMC_Set_Timer_Getting_Flg(BOOL flg);

/**
 * SDRAMC Reload Timer Start.
 * @param[in]	usec		: Reload time[usec]
 * @return		result
 */
INT32 Monitor_SDRAMC_Start_Timer(UINT32 usec);

/**
 * SDRAMC Reload Timer Stop.
 */
VOID Monitor_SDRAMC_Stop_Timer(VOID);

/**
 * SDRAMC Monitor result Save.
 * @param[in]	mode		: Mode
 */
VOID Monitor_SDRAMC_Save_Result(BOOL mode);

/**
 * SDRAMC Monitor result Output.
 * @param[in]	type		: Monitor type
 * @param[in]	file_name	: Log file name
 */
VOID Monitor_SDRAMC_Output_Result(UCHAR type, const CHAR* file_name);

/**
 * SDRAMC Monitor Start in specified time.
 * @param[in]	ch				: SDRAMC ch
 * @param[in]	monitor_time	: Monitor time[msec]
 * @param[out]	sdramc_mon
 * @return		result
 */
INT32 Monitor_SDRAMC_Time(E_IM_SDRAMC_CH ch_no, UINT32 monitor_time, BOOL write, BOOL read, T_IM_SDRAMC_MONITOR *sdramc_mon);

VOID Monitor_SDRAMC_Add( T_IM_SDRAMC_MONITOR* const sdramc_mon_dst, const T_IM_SDRAMC_MONITOR* const sdramc_mon_arg1, const T_IM_SDRAMC_MONITOR* const sdramc_mon_arg2 );

VOID Monitor_SDRAMC_Max( T_IM_SDRAMC_MONITOR* const sdramc_mon_dst, const T_IM_SDRAMC_MONITOR* const sdramc_mon_arg1, const T_IM_SDRAMC_MONITOR* const sdramc_mon_arg2 );

VOID Monitor_SDRAMC_Print( const T_IM_SDRAMC_MONITOR* const sdramc_mon );

VOID Monitor_SDRAMC_Print_MB( const T_IM_SDRAMC_MONITOR* const sdramc_mon );

#endif

#ifdef __cplusplus
}
#endif

#endif	// _MONITOR_H
