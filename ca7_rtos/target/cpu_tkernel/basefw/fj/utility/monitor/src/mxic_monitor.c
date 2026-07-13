/**
 * @file		mxic_monitor.c
 * @brief		MXIC Monitor
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include <stdlib.h>
#include "monitor.h"
#include "dd_tmr32.h"
#include "im_mxic.h"
#include "debug.h"
#include "fs_if.h"
#include "l1l2cache.h"
#include "fj_filesystem.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_MONITOR_MXIC_TIMER_CH			(4)
#define D_MONITOR_RESULT_TARGET_COUNT	(90)
#define D_MONITOR_RESULT_MAX_COUNT		(200)
#define D_MONITOR_DRAM_BW_INFO_COUNT	(64)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct{
	USHORT		monitor_target;					/* Selection monitor target */
	UCHAR		target_name[48];				/* target name */
} T_MONITOR_TARGET_INFO;

typedef struct{
	UINT32		result_cnt;
	UINT32		proc_time;
	UCHAR		monitor_sel;
} T_MONITOR_RESULT_INFO;

typedef struct{
	T_IM_MXIC_MONITOR_TARGET mxic_target[4];
	UINT32 mxic_result_count[4];
	UINT32 mxic_result_total_count;
} T_MONITOR_DRAM_BW_INFO;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#ifdef CO_MXIC_MONITOR_ON
static BOOL gMonitor_MXIC_flg = FALSE;
static T_MONITOR_TARGET_INFO gMonitor_Target_tbl[] = {
	{ D_IM_MXIC_3_W_STAT,		"STAT (Write)" },
	{ D_IM_MXIC_3_W_IIP_A,		"IIPA (Write)" },
	{ D_IM_MXIC_3_W_IIP_B,		"IIPB (Write)" },
	{ D_IM_MXIC_3_W_IIP_C,		"IIPC (Write)" },
	{ D_IM_MXIC_3_W_SHDR_A,		"SHDRA (Write)" },
	{ D_IM_MXIC_3_W_SHDR_B,		"SHDRB (Write)" },
	{ D_IM_MXIC_3_W_SHDR_C,		"SHDRC (Write)" },
	{ D_IM_MXIC_3_W_ME,			"ME (Write)" },
	{ D_IM_MXIC_3_W_IPU_A,		"IPUA (Write)" },
	{ D_IM_MXIC_3_W_IPU_C,		"IPUC (Write)" },
	{ D_IM_MXIC_3_W_IPU_D,		"IPUD (Write)" },
	{ D_IM_MXIC_3_W_IPU_E,		"IPUE (Write)" },
	{ D_IM_MXIC_3_W_DSP_A,		"DSPA (Write)" },
	{ D_IM_MXIC_3_W_DSP_B,		"DSPB (Write)" },
	{ D_IM_MXIC_3_W_GPU,		"GPU (Write)" },
	{ D_IM_MXIC_3_W_DW,			"DW (Write)" },
	{ D_IM_MXIC_3_W_JPEG,		"JPEG (Write)" },
	{ D_IM_MXIC_3_W_RAW,		"RAW (Write)" },
	{ D_IM_MXIC_3_W_GROUP0_M0,	"GROUP0_M0 (Write)" },
	{ D_IM_MXIC_3_W_GROUP0_M1,	"GROUP0_M1 (Write)" },
	{ D_IM_MXIC_3_W_ELA,		"ELA (Write)" },
	{ D_IM_MXIC_3_W_XCH,		"XCH (Write)" },
	{ D_IM_MXIC_3_W_FPT_0,		"FPT0 (Write)" },
	{ D_IM_MXIC_3_W_FPT_1,		"FPT1 (Write)" },
	{ D_IM_MXIC_3_R_STAT,		"STAT (Read)" },
	{ D_IM_MXIC_3_R_IIP_A,		"IIPA (Read)" },
	{ D_IM_MXIC_3_R_IIP_B,		"IIPB (Read)" },
	{ D_IM_MXIC_3_R_IIP_C,		"IIPC (Read)" },
	{ D_IM_MXIC_3_R_SHDR_A,		"SHDRA (Read)" },
	{ D_IM_MXIC_3_R_SHDR_B,		"SHDRB (Read)" },
	{ D_IM_MXIC_3_R_SHDR_C,		"SHDRC (Read)" },
	{ D_IM_MXIC_3_R_SHDR_D,		"SHDRD (Read)" },
	{ D_IM_MXIC_3_R_ME,			"ME (Read)" },
	{ D_IM_MXIC_3_R_IPU_A,		"IPUA (Read)" },
	{ D_IM_MXIC_3_R_IPU_B,		"IPUB (Read)" },
	{ D_IM_MXIC_3_R_IPU_C,		"IPUC (Read)" },
	{ D_IM_MXIC_3_R_IPU_D,		"IPUD (Read)" },
	{ D_IM_MXIC_3_R_DSP_A,		"DSPA (Read)" },
	{ D_IM_MXIC_3_R_DSP_B,		"DSPB (Read)" },
	{ D_IM_MXIC_3_R_GPU,		"GPU (Read)" },
	{ D_IM_MXIC_3_R_JPEG,		"JPEG (Read)" },
	{ D_IM_MXIC_3_R_RAW,		"RAW (Read)" },
	{ D_IM_MXIC_3_R_RDMA,		"RDMA (Read)" },
	{ D_IM_MXIC_TARGET_NOTHING,	"Monitor target nothing" }
};

static volatile T_MONITOR_RESULT_INFO gMonitorResultTable[D_MONITOR_RESULT_TARGET_COUNT][D_MONITOR_RESULT_MAX_COUNT];
static volatile USHORT gMonitorResultTableIndex[D_MONITOR_RESULT_TARGET_COUNT] = { 0 };
#if 0
static volatile UINT32 gMonitorDramBwInfoCount = 0;
static volatile T_MONITOR_DRAM_BW_INFO gMonitorDramBwInfo[D_MONITOR_DRAM_BW_INFO_COUNT] = { 0 };
static volatile T_IM_MXIC_MONITOR_TARGET gMonitorDramBwTarget[4] = { 0 };
static volatile UINT32 gMonitorBwHist_work[D_MONITOR_DRAM_BW_INFO_COUNT * 2] = { 0 };
#endif
#endif

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
#ifdef CO_MXIC_MONITOR_ON
static VOID monitor_info_out(UCHAR type, ULONG file_id, const CHAR* out_buf)
{
	ULONG written_size;
	
	if(type == 0)
	{
		// to terminal
		printf(out_buf);
	}
	else
	{
		// to file
		FJ_Fs_Write(file_id, (UINT32*)out_buf, strlen(out_buf), &written_size);
	}
}

static VOID monitor_mxic_handler(VOID)
{
	if(gMonitor_MXIC_flg) {
		Monitor_MXIC_Save_Result();
	}
}
#endif

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
#ifdef CO_MXIC_MONITOR_ON
static ULONG	gStart_Time = 0;

/**
 * MXIC Set Flag of Getting by Timer.
 * @param[in]	flg			: Enabled getting by timer
 */
VOID Monitor_MXIC_Set_Timer_Getting_Flg(BOOL flg)
{
	gMonitor_MXIC_flg = flg;
}


/**
 * MXIC Reload Timer Start.
 * @param[in]	usec		: Reload time[usec]
 * @return		result
 */
INT32 Monitor_MXIC_Start_Timer(UINT32 usec)
{
	INT32 ret;
	
	ret = Dd_TMR32_Open(D_MONITOR_MXIC_TIMER_CH, D_DDIM_USER_SEM_WAIT_POL);
	if (ret != D_DDIM_OK) {
		return ret;
	}
	if (Dd_TMR32_SetTimer(D_MONITOR_MXIC_TIMER_CH, usec, monitor_mxic_handler) == D_DDIM_OK) {
		Dd_TMR32_StartReload(D_MONITOR_MXIC_TIMER_CH);
	}
	
	return ret;
}


/**
 * MXIC Reload Timer Stop.
 */
VOID Monitor_MXIC_Stop_Timer(VOID)
{
	Dd_TMR32_Stop(D_MONITOR_MXIC_TIMER_CH);
	Dd_TMR32_Close(D_MONITOR_MXIC_TIMER_CH);
}


/**
 * MXIC Monitor Start.
 */
VOID Monitor_MXIC_Start(VOID)
{
	T_IM_MXIC_CLOCK lcfg;
	
	Im_MXIC_Get_Clock_Enable( E_IM_MXIC_UNIT_3, &lcfg );
	lcfg.clk_en_monitor = D_IM_MXIC_ON;
	Im_MXIC_Set_Clock_Enable( E_IM_MXIC_UNIT_3, &lcfg);
	Im_MXIC_Start_Monitor();
	gStart_Time = BF_Get_Time_us();
}


/**
 * MXIC Monitor Stop.
 */
VOID Monitor_MXIC_Stop(VOID)
{
	UINT32	tm_chk;
	INT32									cnt;
	INT32									idx;
	T_IM_MXIC_ALL_ACCESS_OR_TRANS_MONITOR	monitor_result;
	T_IM_MXIC_MONITOR_PARAMETER				cfg;
	T_IM_MXIC_CLOCK							lcfg;
	
	/* Get Measure Time */
	tm_chk = BF_Get_Time_us() - gStart_Time;
	/* Get Monitor Result */
	Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry(&monitor_result);
	/* Get Monitor Parameter */
	Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter(&cfg);
	
	/* Result Output */
	BF_Debug_Print_Information(("-- monitoring result --\n"));
	for ( cnt = 0; cnt < D_IM_MXIC_MAX_MONITOR_TARGET_NUM; cnt++ ) {
		idx = 0;
		while ( gMonitor_Target_tbl[idx].monitor_target != 0 ) {
			if ( gMonitor_Target_tbl[idx].monitor_target == cfg.target[cnt].monitor_target ) {
				BF_Debug_Print_Information(("%24s", gMonitor_Target_tbl[idx].target_name));
				
				/* Selection monitor */
				if (cfg.target[cnt].monitor_sel == D_IM_MXIC_MONITOR_ACCESS ) {
					printf("%s Access (0x%X)  ", gMonitor_Target_tbl[idx].target_name, gMonitor_Target_tbl[idx].monitor_target);
				}
				else {
					printf("%s Transfer (0x%X)", gMonitor_Target_tbl[idx].target_name, gMonitor_Target_tbl[idx].monitor_target);
				}
				/* Monitor Result */
				printf(" : %010u\n", monitor_result.count[cnt]);
				break;
			}
			idx++;
		}
	}
	BF_Debug_Print_Information((" ProcTime : %010u\n", tm_chk ));
	BF_Debug_Print_Information(("-----------------------\n"));
	
	/* Stop monitoring */
	Im_MXIC_Stop_Monitor();
	Im_MXIC_Get_Clock_Enable( E_IM_MXIC_UNIT_3, &lcfg );
	lcfg.clk_en_monitor = D_IM_MXIC_OFF;
	Im_MXIC_Set_Clock_Enable( E_IM_MXIC_UNIT_3, &lcfg );
}


/**
 * MXIC Set Parameter.
 * @param[in]	target_cnt	: Target count
 * @param[in]	*param		: Parameter information
 */
VOID Monitor_MXIC_Set_Parameter(INT32 target_cnt, T_MONITOR_MXIC_PARAMETER* param)
{
	INT32						cnt;
	T_IM_MXIC_MONITOR_PARAMETER	cfg;

	/* check parameter */
	if ((target_cnt <= 0) || (target_cnt > D_IM_MXIC_MAX_MONITOR_TARGET_NUM)) {
		BF_Debug_Print_Error(("Parameter NG : target_cnt=%d\n", target_cnt));
		return;
	}
	if ( param == NULL ) {
		BF_Debug_Print_Error(("Parameter NG : param=NULL\n"));
		return;
	}

	/* Moniter Target */
	for ( cnt = 0; cnt < target_cnt; cnt++ ) {
		cfg.target[cnt].monitor_sel		= param->target[cnt].monitor_sel;
		cfg.target[cnt].monitor_target	= param->target[cnt].monitor_target;
	}
	for ( ; cnt < D_IM_MXIC_MAX_MONITOR_TARGET_NUM; cnt++ ) {
		cfg.target[cnt].monitor_sel		= 0;
		cfg.target[cnt].monitor_target	= D_IM_MXIC_TARGET_NOTHING;
	}

	/* Monitor Mode */
	cfg.start_condition			= D_IM_MXIC_MONITOR_START_TRG;
	cfg.end_condition			= D_IM_MXIC_MONITOR_ENDLESS;
	cfg.update_enable			= D_IM_MXIC_ON;
	cfg.update_trigger_interval	= D_IM_MXIC_MONITOR_UPDATE_1;
	cfg.update_clear_en			= D_IM_MXIC_OFF;
	cfg.start_clear_en			= D_IM_MXIC_ON;
	cfg.cnt_val					= D_IM_MXIC_VALUE_WHEN_UPDATE_TRG;
	/* Limit judgment conditions */
	cfg.limit_access_trans		= D_IM_MXIC_MONITOR_TRANSFER;
	cfg.limit_detection_target	= D_IM_MXIC_TARGET_NOTHING;
	cfg.limit					= 0x00000001;
	/* Callback for TMF interrupt */
	cfg.pCallBack				= NULL;
	
	/* Set Parameter */
	Im_MXIC_Set_Access_Or_Trans_Monitor_Parameter(&cfg);
}


/**
 * MXIC Monitor result Save.
 */
VOID Monitor_MXIC_Save_Result(VOID)
{
	UINT32	tm_chk;
	INT32									cnt;
	INT32									idx;
	T_IM_MXIC_ALL_ACCESS_OR_TRANS_MONITOR	monitor_result;
	T_IM_MXIC_MONITOR_PARAMETER				cfg;
	
	/* Get Measure Time */
	tm_chk = BF_Get_Time_us() - gStart_Time;
	/* Get Monitor Result */
	Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry(&monitor_result);
	/* Get Monitor Parameter */
	Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter(&cfg);
	
	/* Result Output */
	for ( cnt = 0; cnt < D_IM_MXIC_MAX_MONITOR_TARGET_NUM; cnt++ ) {
		idx = 0;
		while ( gMonitor_Target_tbl[idx].monitor_target != 0 ) {
			if ( gMonitor_Target_tbl[idx].monitor_target == cfg.target[cnt].monitor_target ) {
				if( gMonitorResultTableIndex[idx] < D_MONITOR_RESULT_MAX_COUNT ) {
					gMonitorResultTable[idx][gMonitorResultTableIndex[idx]].monitor_sel = cfg.target[cnt].monitor_sel;
					gMonitorResultTable[idx][gMonitorResultTableIndex[idx]].result_cnt  = monitor_result.count[cnt];
					gMonitorResultTable[idx][gMonitorResultTableIndex[idx]].proc_time   = tm_chk;
					gMonitorResultTableIndex[idx] ++;
				}
				break;
			}
			idx++;
		}
	}
}


/**
 * MXIC Monitor result Output.
 * @param[in]	type		: Monitor type
 * @param[in]	file_name	: Log file name
 */
VOID Monitor_MXIC_Output_Result(UCHAR type, const CHAR* file_name)
{
	ULONG file_id = 0;
	CHAR file_path[16] = "I:\\";
	CHAR out_buf[256];
	UCHAR cnt = 0;
	USHORT cnt2 = 0;
	UINT32 idx = 0;
	T_IM_MXIC_MONITOR_PARAMETER		cfg;
	UINT32		result_cnt;
	UINT32		proc_time;
	UINT64		cps;
	
	// Log file open
	if(type == 1)
	{
		// Parameter check
		if((file_name == NULL) || (strlen((CHAR*)file_name) > 8))
		{
			printf("Monitor_Result_Output() File name error!\n");
			return;
		}
		sprintf((CHAR*)&file_path[3], "%s.CSV", (CHAR*)file_name);
		file_id = FJ_Fs_Open((CHAR*)file_path, FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT);
		if(file_id == 0)
		{
			printf("Monitor_Result_Output() FJ_Fs_Open %s failed! (%ld)\n", file_path, file_id);
			return;
		}
	}
	
	// Get Monitor Parameter
	Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter(&cfg);
	
	// Result Output
	for ( cnt = 0; cnt < D_IM_MXIC_MAX_MONITOR_TARGET_NUM; cnt++ ) {
		idx = 0;
		while ( gMonitor_Target_tbl[idx].monitor_target != 0 ) {
			if ( gMonitor_Target_tbl[idx].monitor_target == cfg.target[cnt].monitor_target ) {
				if (cfg.target[cnt].monitor_sel == D_IM_MXIC_MONITOR_ACCESS ) {
					snprintf(out_buf, sizeof(out_buf), "%s Access (0x%X)\n", gMonitor_Target_tbl[idx].target_name, gMonitor_Target_tbl[idx].monitor_target);
				}
				else {
					snprintf(out_buf, sizeof(out_buf), "%s Transfer (0x%X)\n", gMonitor_Target_tbl[idx].target_name, gMonitor_Target_tbl[idx].monitor_target);
				}
				monitor_info_out(type, file_id, out_buf);
				monitor_info_out(type, file_id, "abs_time,abs_count,diff_time,diff_count,count_par_sec,kiro_count_per_sec\n");
				result_cnt = 0;
				proc_time = 0;
				for ( cnt2 = 0; cnt2 < gMonitorResultTableIndex[idx]; cnt2++) {
					cps = ((UINT64)(gMonitorResultTable[idx][cnt2].result_cnt - result_cnt)) * 1000000ULL / (gMonitorResultTable[idx][cnt2].proc_time - proc_time);
					snprintf(out_buf, sizeof(out_buf), "%u,%u,%u,%u,%llu,%llu\n", gMonitorResultTable[idx][cnt2].proc_time,
																				  gMonitorResultTable[idx][cnt2].result_cnt,
																				  gMonitorResultTable[idx][cnt2].proc_time - proc_time,
																				  gMonitorResultTable[idx][cnt2].result_cnt - result_cnt,
																				  cps,
																				  cps / 1000);
					monitor_info_out(type, file_id, out_buf);
					proc_time = gMonitorResultTable[idx][cnt2].proc_time;
					result_cnt = gMonitorResultTable[idx][cnt2].result_cnt;
				}
				break;
			}
			idx++;
		}
	}
	
	// Log file close
	if(type == 1)
	{
		FJ_Fs_Close( file_id );
	}
}


/**
 * MXIC Monitor result Clear.
 */
VOID Monitor_MXIC_Clear_Result(VOID)
{
	memset(&gMonitorResultTable, 0, sizeof(gMonitorResultTable));
	memset(&gMonitorResultTableIndex, 0, sizeof(gMonitorResultTableIndex));
}


#if 0
/**
 * Initialize BandWidth info Monitoring
 */
VOID Monitor_MXIC_BandWidth_Init(VOID)
{
	memset(&gMonitorDramBwInfo, 0, sizeof(gMonitorDramBwInfo));
	gMonitorDramBwInfoCount = 0;
}


/**
 * Start DRAM BandWidth info Monitoring
 * @param[in]	ch			: MXIC channel
 */
VOID Monitor_MXIC_BandWidth_Start(UCHAR ch)
{
	T_MONITOR_MXIC_PARAMETER	mxic_param;
	
	if( ch == 0 ) {
		mxic_param.target[0].monitor_target = D_IM_MXIC_SLAVE_W1_PORT0;
		mxic_param.target[1].monitor_target = D_IM_MXIC_SLAVE_W1_PORT1;
		mxic_param.target[2].monitor_target = D_IM_MXIC_SLAVE_R1_PORT0;
		mxic_param.target[3].monitor_target = D_IM_MXIC_SLAVE_R1_PORT1;
	} else {
		mxic_param.target[0].monitor_target = D_IM_MXIC_SLAVE_W2_PORT0;
		mxic_param.target[1].monitor_target = D_IM_MXIC_SLAVE_W2_PORT1;
		mxic_param.target[2].monitor_target = D_IM_MXIC_SLAVE_R2_PORT0;
		mxic_param.target[3].monitor_target = D_IM_MXIC_SLAVE_R2_PORT1;
	}
	mxic_param.target[0].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
	mxic_param.target[1].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
	mxic_param.target[2].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
	mxic_param.target[3].monitor_sel    = D_IM_MXIC_MONITOR_TRANSFER;
	
	gMonitorDramBwTarget[0].monitor_sel		= mxic_param.target[0].monitor_sel;
	gMonitorDramBwTarget[0].monitor_target	= mxic_param.target[0].monitor_target;
	gMonitorDramBwTarget[1].monitor_sel		= mxic_param.target[1].monitor_sel;
	gMonitorDramBwTarget[1].monitor_target	= mxic_param.target[1].monitor_target;
	gMonitorDramBwTarget[2].monitor_sel		= mxic_param.target[2].monitor_sel;
	gMonitorDramBwTarget[2].monitor_target	= mxic_param.target[2].monitor_target;
	gMonitorDramBwTarget[3].monitor_sel		= mxic_param.target[3].monitor_sel;
	gMonitorDramBwTarget[3].monitor_target	= mxic_param.target[3].monitor_target;
	
	// MXIC Monitor Setting
	Monitor_MXIC_Set_Parameter( 4, &mxic_param );
	
	// Cache Cleaning & Invalidate
	BF_L1l2cache_Clean_Flush_All();
	
	// Monitoring start
	Monitor_MXIC_Start();
}


/**
 * Stop DRAM BandWidth info Monitoring
 * @param[in]	ch			: MXIC channel
 */
VOID Monitor_MXIC_BandWidth_Stop(VOID)
{
	T_IM_MXIC_ALL_ACCESS_OR_TRANS_MONITOR	monitor_result;
	T_IM_MXIC_CLOCK							lcfg;
	
	// Cache Cleaning & Invalidate
	BF_L1l2cache_Clean_Flush_All();
	
	if( gMonitorDramBwInfoCount >= D_MONITOR_DRAM_BW_INFO_COUNT ) {
		gMonitorDramBwInfoCount = 0;
		memcpy( (void*)gMonitorBwHist_work, (void*)&gMonitorBwHist_work[D_MONITOR_DRAM_BW_INFO_COUNT], sizeof(ULONG)*D_MONITOR_DRAM_BW_INFO_COUNT );
	}
	
	// Get MXIC Monitoring Result
	Im_MXIC_Get_Access_Or_Trans_Monitor_All_Entry(&monitor_result);
#if 0
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_result_count[0]			= monitor_result.count[0];
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[0].monitor_sel		= gMonitorDramBwTarget[0].monitor_sel;
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[0].monitor_target	= gMonitorDramBwTarget[0].monitor_target;
	
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_result_count[1]			= monitor_result.count[1];
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[1].monitor_sel		= gMonitorDramBwTarget[1].monitor_sel;
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[1].monitor_target	= gMonitorDramBwTarget[1].monitor_target;
	
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_result_count[2]			= monitor_result.count[2];
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[2].monitor_sel		= gMonitorDramBwTarget[2].monitor_sel;
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[2].monitor_target	= gMonitorDramBwTarget[2].monitor_target;
	
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_result_count[3]			= monitor_result.count[3];
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[3].monitor_sel		= gMonitorDramBwTarget[3].monitor_sel;
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[3].monitor_target	= gMonitorDramBwTarget[3].monitor_target;
	
	gMonitorDramBwInfoCount++;
#else
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_result_count[0]			= monitor_result.count[0];
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[0].monitor_sel		= gMonitorDramBwTarget[0].monitor_sel;
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[0].monitor_target	= gMonitorDramBwTarget[0].monitor_target;
	
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_result_count[1]			= monitor_result.count[1];
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[1].monitor_sel		= gMonitorDramBwTarget[1].monitor_sel;
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[1].monitor_target	= gMonitorDramBwTarget[1].monitor_target;
	
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_result_count[2]			= monitor_result.count[2];
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[2].monitor_sel		= gMonitorDramBwTarget[2].monitor_sel;
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[2].monitor_target	= gMonitorDramBwTarget[2].monitor_target;
	
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_result_count[3]			= monitor_result.count[3];
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[3].monitor_sel		= gMonitorDramBwTarget[3].monitor_sel;
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_target[3].monitor_target	= gMonitorDramBwTarget[3].monitor_target;
	
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_result_total_count			+= monitor_result.count[0];
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_result_total_count			+= monitor_result.count[1];
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_result_total_count			+= monitor_result.count[2];
	gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_result_total_count			+= monitor_result.count[3];
	
	if( gMonitorDramBwTarget[0].monitor_target == D_IM_MXIC_SLAVE_W2_PORT0 ) {
		// for Displaying Histgram
		gMonitorBwHist_work[ D_MONITOR_DRAM_BW_INFO_COUNT + gMonitorDramBwInfoCount ] = (UINT32)( (double)gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_result_total_count * 7.576 );
		gMonitorDramBwInfo[gMonitorDramBwInfoCount].mxic_result_total_count = 0;
		gMonitorDramBwInfoCount++;
	}
#endif
	/* Stop monitoring */
	Im_MXIC_Stop_Monitor();
	lcfg.clk_en_slave_1 = 1;
	lcfg.clk_en_slave_2 = 1;
	lcfg.clk_en_group_0 = 1;
	lcfg.clk_en_group_1 = 1;
	lcfg.clk_en_dec_err = 1;
	lcfg.clk_en_monitor = 0;
	Im_MXIC_Set_Clock_Enable(&lcfg);
}


/**
 * Output DRAM BandWidth info Monitoring Result
 */
VOID Monitor_MXIC_BandWidth_Output_Info(VOID)
{
	UINT32	i;
	UINT32	idx;
	UINT32	cnt;
	T_IM_MXIC_MONITOR_PARAMETER	cfg;
	
	// Get Monitoring Parameter
	Im_MXIC_Get_Access_Or_Trans_Monitor_Parameter(&cfg);
	
	for ( cnt = 0; cnt < D_IM_MXIC_MAX_MONITOR_TARGET_NUM; cnt++ ) {
		idx = 0;
		while ( gMonitorDramBwInfo[idx].mxic_target[cnt].monitor_target != 0 ) {
			if ( gMonitorDramBwInfo[idx].mxic_target[cnt].monitor_target == cfg.target[cnt].monitor_target ) {
				printf("%48s,", gMonitor_Target_tbl[idx].target_name);
				break;
			}
			idx++;
		}
	}
	printf("\n");
	
	for( i = 0; i < D_MONITOR_DRAM_BW_INFO_COUNT; i++ ) {
		printf("%10d,%10d,%10d,%10d\n",	gMonitorDramBwInfo[i].mxic_result_count[0],
										gMonitorDramBwInfo[i].mxic_result_count[1],
										gMonitorDramBwInfo[i].mxic_result_count[2],
										gMonitorDramBwInfo[i].mxic_result_count[3]
										);
	}
}


/**
 * Get DRAM BandWidth info Monitoring Result
 */
VOID* Monitor_MXIC_BandWidth_Get_Info(VOID)
{
	return (void*)&gMonitorBwHist_work[gMonitorDramBwInfoCount];
}
#endif
#endif
