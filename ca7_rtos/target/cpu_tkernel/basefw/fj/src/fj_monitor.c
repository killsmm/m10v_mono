/**
 * @file		fj_monitor.c
 * @brief		Monitor process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_monitor.h"
#include "monitor.h"

/*--------------------------------------------------------------*/
/* Definition													*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Global Data													*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Extern  Function												*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Local Function												*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Global Function												*/
/*--------------------------------------------------------------*/
VOID FJ_Monitor_MXIC_Ctrl_On(VOID)
{
#ifdef CO_MXIC_MONITOR_ON
	fj_monitor_mxic_ctrl_on();
#endif
}

VOID FJ_Monitor_MXIC_Ctrl_Off(VOID)
{
#ifdef CO_MXIC_MONITOR_ON
	fj_monitor_mxic_ctrl_off();
#endif
}

FJ_ERR_CODE FJ_Monitor_MXIC_Config(FJ_MONITOR_MXIC_CFG* cfg)
{
#ifdef CO_MXIC_MONITOR_ON
	return fj_monitor_mxic_config(cfg);
#else
	return FJ_ERR_OK;
#endif
}

VOID FJ_Monitor_MXIC_Start(VOID)
{
#ifdef CO_MXIC_MONITOR_ON
	fj_monitor_mxic_start();
#endif
}

VOID FJ_Monitor_MXIC_Stop(FJ_MONITOR_MXIC_RESULT* mxic_result, UINT64* const measure_time)
{
#ifdef CO_MXIC_MONITOR_ON
	fj_monitor_mxic_stop(mxic_result, measure_time);
#endif
}

VOID FJ_Monitor_SDRAMC_Start(FJ_MONITOR_SDRAMC_CH sdramc_ch, FJ_MONITOR_SDRAMC_MONITOR_TYPE monitor_type, FJ_MONITOR_SDRAMC_ACCESS_TYPE access_type)
{
#ifdef CO_SDRAMC_MONITOR_ON
	fj_monitor_sdramc_start(sdramc_ch, monitor_type, access_type);
#endif
}

VOID FJ_Monitor_SDRAMC_Stop(FJ_MONITOR_SDRAMC_CH sdramc_ch)
{
#ifdef CO_SDRAMC_MONITOR_ON
	fj_monitor_sdramc_stop(sdramc_ch);
#endif
}

FJ_ERR_CODE FJ_Monitor_SDRAMC_Get_Result(FJ_MONITOR_SDRAMC_CH sdramc_ch, FJ_MONITOR_SDRAMC_RESULT* sdramc_result)
{
#ifdef CO_SDRAMC_MONITOR_ON
	return fj_monitor_sdramc_get_result( sdramc_ch, sdramc_result );
#else
	return FJ_ERR_OK;
#endif
}

VOID FJ_Monitor_PMU_Ctrl_Cycle(BOOL flag)
{
#ifdef CO_PMU_MONITOR_ON
	fj_monitor_pmu_ctrl_cycle(flag);
#endif
}

VOID FJ_Monitor_PMU_Ctrl_Event(FJ_MONITOR_PMU_COUNTER counter_num, FJ_MONITOR_PMU_EVENT event_num)
{
#ifdef CO_PMU_MONITOR_ON
	fj_monitor_pmu_ctrl_event(counter_num, event_num);
#endif
}

VOID FJ_Monitor_PMU_Start(FJ_MONITOR_PMU_RESET reset_counter, FJ_MONITOR_PMU_DIVIDER cycle_divider)
{
#ifdef CO_PMU_MONITOR_ON
	fj_monitor_pmu_start(reset_counter, cycle_divider);
#endif
}

VOID FJ_Monitor_PMU_Stop(VOID)
{
#ifdef CO_PMU_MONITOR_ON
	fj_monitor_pmu_stop();
#endif
}

UINT32 FJ_Monitor_PMU_Get_Cycle_Count(VOID)
{
#ifdef CO_PMU_MONITOR_ON
	return fj_monitor_pmu_get_cycle_count();
#else
	return 0;
#endif
}

UINT32 FJ_Monitor_PMU_Get_Event_Count(FJ_MONITOR_PMU_COUNTER counter_num)
{
#ifdef CO_PMU_MONITOR_ON
	return fj_monitor_pmu_get_event_count(counter_num);
#else
	return 0;
#endif
}

