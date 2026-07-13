/**
 * @file		monitor.c
 * @brief		MXIC, SDRAMC and PMU Monitor
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_monitor.h"
#include "monitor.h"
#include "measure_time.h"

/*--------------------------------------------------------------*/
/* Definition													*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Enumeration													*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Structure													*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Global Data													*/
/*--------------------------------------------------------------*/
static ULONG	gStart_Time = 0;

/*--------------------------------------------------------------*/
/* Macro														*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Local Function												*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*--------------------------------------------------------------*/
/* Global Function												*/
/*--------------------------------------------------------------*/
VOID fj_monitor_mxic_ctrl_on(VOID)
{
	T_IM_MXIC_CLOCK clk;
	
	/* Enable Monitor */
	Im_MXIC_Get_Clock_Enable( E_IM_MXIC_UNIT_3, &clk );
	clk.clk_en_monitor = D_IM_MXIC_ON;
	Im_MXIC_Set_Clock_Enable( E_IM_MXIC_UNIT_3, &clk );
}

VOID fj_monitor_mxic_ctrl_off(VOID)
{
	T_IM_MXIC_CLOCK clk;
	
	/* Disable Monitor */
	Im_MXIC_Get_Clock_Enable( E_IM_MXIC_UNIT_3, &clk );
	clk.clk_en_monitor = D_IM_MXIC_OFF;
	Im_MXIC_Set_Clock_Enable( E_IM_MXIC_UNIT_3, &clk );
}

FJ_ERR_CODE fj_monitor_mxic_config(FJ_MONITOR_MXIC_CFG* cfg)
{
	INT32 ret;
	T_IM_MXIC_MONITOR_PARAMETER param;
	
	if(cfg == NULL){
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	// Monitor Configure
	param.start_condition         = D_IM_MXIC_MONITOR_START_TRG;
	param.end_condition           = D_IM_MXIC_MONITOR_ENDLESS;
	param.update_enable           = D_IM_MXIC_ON;
	param.update_trigger_interval = D_IM_MXIC_MONITOR_UPDATE_1;
	param.update_clear_en         = D_IM_MXIC_OFF;
	param.start_clear_en          = D_IM_MXIC_OFF;
	param.cnt_val                 = D_IM_MXIC_VALUE_WHEN_UPDATE_TRG;
	param.limit_access_trans      = D_IM_MXIC_MONITOR_TRANSFER;
	param.limit_detection_target  = D_IM_MXIC_TARGET_NOTHING;
	param.limit                   = 0x00000001;
	param.pCallBack = NULL;
	
	// Monitor Type(Access or Transfer) and Target
	param.target[0].monitor_sel    = cfg->monitor_type[0];
	param.target[1].monitor_sel    = cfg->monitor_type[1];
	param.target[2].monitor_sel    = cfg->monitor_type[2];
	param.target[3].monitor_sel    = cfg->monitor_type[3];
	param.target[0].monitor_target = cfg->monitor_target[0];
	param.target[1].monitor_target = cfg->monitor_target[1];
	param.target[2].monitor_target = cfg->monitor_target[2];
	param.target[3].monitor_target = cfg->monitor_target[3];
	
	// Set Monitor Parameter
	ret = Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter(&param);
	if(ret != D_DDIM_OK){
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

VOID fj_monitor_mxic_start(VOID)
{
	/* Start monitoring */
	Im_MXIC_Start_Monitor();
	
	/* Get Measure Time */
	gStart_Time = BF_Get_Time_us_64();
}

VOID fj_monitor_mxic_stop(FJ_MONITOR_MXIC_RESULT* mxic_result, UINT64* const measure_time)
{
	T_IM_MXIC_ALL_ACCESS_OR_TRANS_MONITOR result;
	
	/* Get Measure Time */
	*measure_time = BF_Get_Time_us_64() - gStart_Time;
	
	/* Get Monitor Result */
	Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry(&result);
	
	/* Set Monitor Result */
	mxic_result->count[0] = result.count[0];
	mxic_result->count[1] = result.count[1];
	mxic_result->count[2] = result.count[2];
	mxic_result->count[3] = result.count[3];
	
	/* Stop Monitoring */
	Im_MXIC_Stop_Monitor();
}

VOID fj_monitor_sdramc_start( FJ_MONITOR_SDRAMC_CH sdramc_ch, FJ_MONITOR_SDRAMC_MONITOR_TYPE monitor_type, FJ_MONITOR_SDRAMC_ACCESS_TYPE access_type )
{
	BOOL write;
	BOOL read;

	if ( access_type == FJ_MONITOR_SDRAMC_ACCESS_TYPE_WRITE_READ ) {
		write	= TRUE;
		read	= TRUE;
	}
	else if ( access_type == FJ_MONITOR_SDRAMC_ACCESS_TYPE_WRITE ) {
		write	= TRUE;
		read	= FALSE;
	}
	else {
		write	= FALSE;
		read	= TRUE;
	}

	Im_SDRAMC_Start_Monitor( (E_IM_SDRAMC_CH)sdramc_ch, (E_IM_SDRAMC_MONMODE)monitor_type, write, read, TRUE );
}

VOID fj_monitor_sdramc_stop( FJ_MONITOR_SDRAMC_CH sdramc_ch )
{
	Im_SDRAMC_Stop_Monitor( (E_IM_SDRAMC_CH)sdramc_ch );
}

FJ_ERR_CODE fj_monitor_sdramc_get_result( FJ_MONITOR_SDRAMC_CH sdramc_ch, FJ_MONITOR_SDRAMC_RESULT* sdramc_result )
{
	INT32 ret;
	T_IM_SDRAMC_MONITOR mon;

	/* Get Monitor Result */
	ret = Im_SDRAMC_Get_Monitor( (E_IM_SDRAMC_CH)sdramc_ch, &mon );
	if ( ret != D_DDIM_OK ) {
		return FJ_ERR_NG;
	}

	/* Set Monitor Result */
	sdramc_result->mxic00		= mon.data[E_IM_SDRAMC_PORT_MX00];
	sdramc_result->mxic01		= mon.data[E_IM_SDRAMC_PORT_MX01];
	sdramc_result->mxic10		= mon.data[E_IM_SDRAMC_PORT_MX10];
	sdramc_result->mxic11		= mon.data[E_IM_SDRAMC_PORT_MX11];
	sdramc_result->mxic12		= mon.data[E_IM_SDRAMC_PORT_MX12];
	sdramc_result->mxic13		= mon.data[E_IM_SDRAMC_PORT_MX13];
	sdramc_result->mxic20		= mon.data[E_IM_SDRAMC_PORT_MX20];
	sdramc_result->mxic21		= mon.data[E_IM_SDRAMC_PORT_MX21];
	sdramc_result->mxic22		= mon.data[E_IM_SDRAMC_PORT_MX22];
	sdramc_result->mxic23		= mon.data[E_IM_SDRAMC_PORT_MX23];
	sdramc_result->mxic30		= mon.data[E_IM_SDRAMC_PORT_MX30];
	sdramc_result->mxic31		= mon.data[E_IM_SDRAMC_PORT_MX31];
	sdramc_result->mxic32		= mon.data[E_IM_SDRAMC_PORT_MX32];
	sdramc_result->mxic33		= mon.data[E_IM_SDRAMC_PORT_MX33];
	sdramc_result->mxic40		= mon.data[E_IM_SDRAMC_PORT_MX40];
	sdramc_result->mxic50		= mon.data[E_IM_SDRAMC_PORT_MX50];
	sdramc_result->mxic51		= mon.data[E_IM_SDRAMC_PORT_MX51];
	sdramc_result->mxic60		= mon.data[E_IM_SDRAMC_PORT_MX60];
	sdramc_result->ribery0		= mon.data[E_IM_SDRAMC_PORT_RB0];
	sdramc_result->ribery1		= mon.data[E_IM_SDRAMC_PORT_RB1];
	sdramc_result->ribery2		= mon.data[E_IM_SDRAMC_PORT_RB2];
	sdramc_result->h265_com0	= mon.data[E_IM_SDRAMC_PORT_DS0];
	sdramc_result->h265_com1	= mon.data[E_IM_SDRAMC_PORT_DS1];
	sdramc_result->h265_com2	= mon.data[E_IM_SDRAMC_PORT_SDM];
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	sdramc_result->h265_enc0	= mon.data[E_IM_SDRAMC_PORT_VE0];
	sdramc_result->h265_enc1	= mon.data[E_IM_SDRAMC_PORT_VE1];
	sdramc_result->h265_enc2	= mon.data[E_IM_SDRAMC_PORT_VE2];
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	sdramc_result->h265_enc0	= mon.data[E_IM_SDRAMC_PORT_VCOREW];
	sdramc_result->h265_enc1	= mon.data[E_IM_SDRAMC_PORT_VCORER0];
	sdramc_result->h265_enc2	= mon.data[E_IM_SDRAMC_PORT_VCORER1];
	sdramc_result->h265_enc3	= mon.data[E_IM_SDRAMC_PORT_VCPU];
	sdramc_result->h265_enc4	= mon.data[E_IM_SDRAMC_PORT_VCOREM];
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	sdramc_result->h265_dec0	= mon.data[E_IM_SDRAMC_PORT_VD0];
	sdramc_result->h265_dec1	= mon.data[E_IM_SDRAMC_PORT_VD1];
	sdramc_result->h265_dec2	= mon.data[E_IM_SDRAMC_PORT_VD2];

	return FJ_ERR_OK;
}

VOID fj_monitor_pmu_ctrl_cycle(BOOL flag)
{
	if(flag){
		BF_PMU_Ctrl_Cycle_Enable();
	}else{
		BF_PMU_Ctrl_Cycle_Disable();
	}
}

VOID fj_monitor_pmu_ctrl_event(FJ_MONITOR_PMU_COUNTER counter_num, FJ_MONITOR_PMU_EVENT event_num)
{
	if(event_num >= 0){
		BF_PMU_Ctrl_Event_Set((E_PMU_COUNTER)counter_num, (E_PMU_EVENT)event_num);
	}else{
		BF_PMU_Ctrl_Event_Disable((E_PMU_COUNTER)counter_num);
	}
}

VOID fj_monitor_pmu_start(FJ_MONITOR_PMU_RESET reset_counter, FJ_MONITOR_PMU_DIVIDER cycle_divider)
{
	BF_PMU_Start((E_PMU_RESET)reset_counter, (E_PMU_DIVIDER)cycle_divider);
}

VOID fj_monitor_pmu_stop(VOID)
{
	BF_PMU_Stop();
}

UINT32 fj_monitor_pmu_get_cycle_count(VOID)
{
	return BF_PMU_Get_Cycle_Count();
}

UINT32 fj_monitor_pmu_get_event_count(FJ_MONITOR_PMU_COUNTER counter_num)
{
	return BF_PMU_Get_Event_Count((E_PMU_COUNTER)counter_num);
}

