/**
 * @file		sdramc_monitor.c
 * @brief		Arbitration Access/Size Monitor Measurement
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include <stdlib.h>
#include "monitor.h"
#include "dd_tmr32.h"
#include "im_sdramc.h"
#include "debug.h"
#include "fs_if.h"
#include "os_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_SDRAMC_MONITOR_TIMER_CH		(4)
#define D_SDRAMC_MONITOR_INFO_LENGTH	(10)
#define D_SDRAMC_MONITOR_INFO_COUNT		(1 << D_SDRAMC_MONITOR_INFO_LENGTH)
#define D_SDRAMC_MONITOR_INFO_COUNT_1	(D_SDRAMC_MONITOR_INFO_COUNT - 1)

#define Monitor_SDRAMC_Roundup_MiB(val)	(((val) + (1024 * 1024 - 1)) / 1024 / 1024 )

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct{
	UINT32				time;
	T_IM_SDRAMC_MONITOR	result;
} T_MONITOR_SDRAMC_INFO;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#ifdef CO_SDRAMC_MONITOR_ON
static UINT32	gSDRAMC_Ch					= 0;
static UINT32	gSDRAMC_Type				= 0;
static BOOL		gSDRAMC_Write				= TRUE;
static BOOL		gSDRAMC_Read				= TRUE;
static BOOL		gSDRAMC_Mode				= TRUE;
static BOOL		gMonitor_SDRAMC_Get_Flag	= FALSE;
static BOOL		gMonitor_SDRAMC_Save_Flag	= FALSE;
static UINT32	gMonitor_SDRAMC_Info_Count	= 0;
static ULONG	gMonitor_Last_Time			= 0;
static T_MONITOR_SDRAMC_INFO gMonitor_SDRAMC_Info[D_SDRAMC_MONITOR_INFO_COUNT];
#endif

/*----------------------------------------------------------------------*/
/* Macro														 		*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
#ifdef CO_SDRAMC_MONITOR_ON
static VOID monitor_sdramc_handler( VOID )
{
	if ( gMonitor_SDRAMC_Get_Flag ) {
		Monitor_SDRAMC_Save_Result( 0 );
	}
}
#endif

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/
#ifdef CO_SDRAMC_MONITOR_ON
/**
 * SDRAMC Set Flag of Getting by Timer.
 */
VOID Monitor_SDRAMC_Set_Timer_Getting_Flg( BOOL flg )
{
	gMonitor_SDRAMC_Get_Flag = flg;
}

/**
 * SDRAMC Reload Timer Start.
 */
INT32 Monitor_SDRAMC_Start_Timer( UINT32 usec )
{
	INT32 ret;

	ret = Dd_TMR32_Open( D_SDRAMC_MONITOR_TIMER_CH, D_DDIM_USER_SEM_WAIT_POL );
	if ( ret != D_DDIM_OK ) {
		return ret;
	}

	if ( Dd_TMR32_SetTimer( D_SDRAMC_MONITOR_TIMER_CH, usec, monitor_sdramc_handler ) == D_DDIM_OK ) {
		Dd_TMR32_StartReload( D_SDRAMC_MONITOR_TIMER_CH );
	}

	return ret;
}

/**
 * SDRAMC Reload Timer Stop.
 */
VOID Monitor_SDRAMC_Stop_Timer( VOID )
{
	Dd_TMR32_Stop( D_SDRAMC_MONITOR_TIMER_CH );
	Dd_TMR32_Close( D_SDRAMC_MONITOR_TIMER_CH );
}

/**
 * SDRAMC Monitor Start.
 */
VOID Monitor_SDRAMC_Start( E_IM_SDRAMC_CH ch, E_IM_SDRAMC_MONMODE type, BOOL write, BOOL read, BOOL update_mode )
{
	/* Start arbitration access monitor */
	memset( &gMonitor_SDRAMC_Info, 0, sizeof(gMonitor_SDRAMC_Info) );

	Im_SDRAMC_Start_Monitor( ch, type, write, read, update_mode );

	gSDRAMC_Ch					= ch;
	gSDRAMC_Type				= type;
	gSDRAMC_Write				= write;
	gSDRAMC_Read				= read;
	gSDRAMC_Mode				= update_mode;
	gMonitor_Last_Time			= BF_Get_Time_us();
	gMonitor_SDRAMC_Save_Flag	= FALSE;
	gMonitor_SDRAMC_Info_Count	= 0;
}

/**
 * SDRAMC Monitor Stop.
 */
INT32 Monitor_SDRAMC_Stop( T_IM_SDRAMC_MONITOR *sdramc_mon )
{
	INT32 ret;

	/* Stop arbitration access monitor */
	Im_SDRAMC_Stop_Monitor( (E_IM_SDRAMC_CH)gSDRAMC_Ch );

	/* Get arbitration access size monitor */
	ret = Im_SDRAMC_Get_Monitor( (E_IM_SDRAMC_CH)gSDRAMC_Ch, sdramc_mon );
	if ( ret != D_DDIM_OK ) {
		return -1;
	}

	return 0;
}

/**
 * SDRAMC Monitor result Save.
 */
VOID Monitor_SDRAMC_Save_Result( BOOL mode )
{
	T_IM_SDRAMC_MONITOR sdramc_mon;
	
	/* Get arbitration access size monitor */
	Im_SDRAMC_Stop_Monitor( (E_IM_SDRAMC_CH)gSDRAMC_Ch );
	Im_SDRAMC_Get_Monitor( (E_IM_SDRAMC_CH)gSDRAMC_Ch, &sdramc_mon );
	if (gMonitor_SDRAMC_Save_Flag || mode){		// exclude 1st period data when getting by timer.
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].time = BF_Get_Time_us() - gMonitor_Last_Time;
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX00]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX00];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX01]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX01];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX10]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX10];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX11]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX11];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX12]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX12];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX13]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX13];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX20]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX20];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX21]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX21];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX22]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX22];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX23]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX23];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX30]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX30];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX31]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX31];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX32]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX32];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX33]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX33];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX40]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX40];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX50]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX50];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX51]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX51];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_MX60]	= sdramc_mon.data[E_IM_SDRAMC_PORT_MX60];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_RB0]	= sdramc_mon.data[E_IM_SDRAMC_PORT_RB0];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_RB1]	= sdramc_mon.data[E_IM_SDRAMC_PORT_RB1];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_RB2]	= sdramc_mon.data[E_IM_SDRAMC_PORT_RB2];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_DS0]	= sdramc_mon.data[E_IM_SDRAMC_PORT_DS0];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_DS1]	= sdramc_mon.data[E_IM_SDRAMC_PORT_DS1];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_SDM]	= sdramc_mon.data[E_IM_SDRAMC_PORT_SDM];
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_VE0]	= sdramc_mon.data[E_IM_SDRAMC_PORT_VE0];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_VE1]	= sdramc_mon.data[E_IM_SDRAMC_PORT_VE1];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_VE2]	= sdramc_mon.data[E_IM_SDRAMC_PORT_VE2];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_VD0]	= sdramc_mon.data[E_IM_SDRAMC_PORT_VD0];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_VD1]	= sdramc_mon.data[E_IM_SDRAMC_PORT_VD1];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_VD2]	= sdramc_mon.data[E_IM_SDRAMC_PORT_VD2];
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_VCOREW]	= sdramc_mon.data[E_IM_SDRAMC_PORT_VCOREW];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_VCORER0]	= sdramc_mon.data[E_IM_SDRAMC_PORT_VCORER0];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_VCORER1]	= sdramc_mon.data[E_IM_SDRAMC_PORT_VCORER1];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_VD0]		= sdramc_mon.data[E_IM_SDRAMC_PORT_VD0];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_VD1]		= sdramc_mon.data[E_IM_SDRAMC_PORT_VD1];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_VD2]		= sdramc_mon.data[E_IM_SDRAMC_PORT_VD2];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_VCPU]		= sdramc_mon.data[E_IM_SDRAMC_PORT_VCPU];
		gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].result.data[E_IM_SDRAMC_PORT_VCOREM]	= sdramc_mon.data[E_IM_SDRAMC_PORT_VCOREM];
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		gMonitor_SDRAMC_Info_Count = D_SDRAMC_MONITOR_INFO_COUNT_1 & (gMonitor_SDRAMC_Info_Count + 1);
	}else{
		gMonitor_SDRAMC_Save_Flag = TRUE;
	}
	Im_SDRAMC_Start_Monitor( (E_IM_SDRAMC_CH)gSDRAMC_Ch, (E_IM_SDRAMC_MONMODE)gSDRAMC_Type, gSDRAMC_Write, gSDRAMC_Read, gSDRAMC_Mode );
	gMonitor_Last_Time = BF_Get_Time_us();
}

/**
 * SDRAMC Monitor result Output.
 */
VOID Monitor_SDRAMC_Output_Result( UCHAR type, const CHAR* file_name )
{
	INT32 file_id = 0;
	CHAR file_path[16] = "I:\\";
	CHAR buff[512];
	ULONG written_size;
	UINT32 idx;
	UINT32 cnt = 0;
	UINT64 ttl_time			= 0;
	UINT64 ttl_mxic0		= 0;
	UINT64 ttl_mxic1		= 0;
	UINT64 ttl_mxic2		= 0;
	UINT64 ttl_mxic3		= 0;
	UINT64 ttl_mxic4		= 0;
	UINT64 ttl_mxic5		= 0;
	UINT64 ttl_mxic6		= 0;
	UINT64 ttl_mxic			= 0;
	UINT64 ttl_ribery0		= 0;
	UINT64 ttl_ribery1		= 0;
	UINT64 ttl_ribery2		= 0;
	UINT64 ttl_ribery		= 0;
	UINT64 ttl_h265_com0	= 0;
	UINT64 ttl_h265_com1	= 0;
	UINT64 ttl_h265_com2	= 0;
	UINT64 ttl_h265_enc0	= 0;
	UINT64 ttl_h265_enc1	= 0;
	UINT64 ttl_h265_enc2	= 0;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	UINT64 ttl_h265_enc3	= 0;
	UINT64 ttl_h265_enc4	= 0;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	UINT64 ttl_h265_dec0	= 0;
	UINT64 ttl_h265_dec1	= 0;
	UINT64 ttl_h265_dec2	= 0;
	UINT64 ttl_h265			= 0;
	UINT64 ttl				= 0;
	
	// Log file open
	if(type == 1){
		// Parameter check
		if((file_name == NULL) || (strlen((CHAR*)file_name) > 8)){
			printf("Monitor_SDRAMC_Result_Output() File name error!\n");
			return;
		}
		sprintf((CHAR*)&file_path[3], "%s.CSV", (CHAR*)file_name);
		file_id = BF_Fs_If_Create(file_path, D_BF_FS_IF_ATTR_NORMAL);
		if(file_id == 0){
			printf("Monitor_SDRAMC_Result_Output() FSIF_Create %s failed! (%d)\n", file_path, file_id);
			return;
		}
	}
	
	// Log data output
	snprintf(buff, sizeof(buff), "Time[us],mxic0,mxic1,mxic2,mxic3,mxic4,mxic5,mxic6,ribery0,ribery1,ribery2,h265_com,h265_enc,h265_dec\n");
	if(type == 1){
		BF_Fs_If_Write(file_id, buff, strlen(buff), &written_size);
	}
	
	if(gMonitor_SDRAMC_Info[gMonitor_SDRAMC_Info_Count].time) {
		for(idx = gMonitor_SDRAMC_Info_Count; idx < D_SDRAMC_MONITOR_INFO_COUNT; idx++){
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			snprintf(buff, sizeof(buff), "%u,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu\n",
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
			snprintf(buff, sizeof(buff), "%u,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu\n",
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
										gMonitor_SDRAMC_Info[idx].time,
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX00],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX01],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX10],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX11],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX12],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX13],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX20],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX21],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX22],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX23],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX30],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX31],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX32],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX33],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX40],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX50],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX51],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX60],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_RB0],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_RB1],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_RB2],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_DS0],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_DS1],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_SDM],
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VE0],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VE1],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VE2],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD0],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD1],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD2]);
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCOREW],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCORER0],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCORER1],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD0],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD1],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD2],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCPU],
										gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCOREM]);
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

			ttl_time      += gMonitor_SDRAMC_Info[idx].time;
			ttl_mxic0     += (gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX00] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX01]);
			ttl_mxic1     += (gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX10] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX11] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX12] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX13]);
			ttl_mxic2     += (gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX10] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX21] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX22] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX23]);
			ttl_mxic3     += (gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX10] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX31] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX32] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX33]);
			ttl_mxic4     +=  gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX40];
			ttl_mxic5     += (gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX50] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX51]);
			ttl_mxic6     +=  gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX60];
			ttl_ribery0   += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_RB0];
			ttl_ribery1   += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_RB1];
			ttl_ribery2   += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_RB2];
			ttl_h265_com0 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_DS0];
			ttl_h265_com1 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_DS1];
			ttl_h265_com2 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_SDM];
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			ttl_h265_enc0 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VE0];
			ttl_h265_enc1 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VE1];
			ttl_h265_enc2 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VE2];
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
			ttl_h265_enc0 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCOREW];
			ttl_h265_enc1 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCORER0];
			ttl_h265_enc2 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCORER1];
			ttl_h265_enc3 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCPU];
			ttl_h265_enc4 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCOREM];
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
			ttl_h265_dec0 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD0];
			ttl_h265_dec1 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD1];
			ttl_h265_dec2 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD2];

			if ( type == 1 ) {
				BF_Fs_If_Write( file_id, buff, strlen(buff), &written_size );
			}
			cnt++;
		}
	}

	for( idx = 0; idx < gMonitor_SDRAMC_Info_Count; idx++ ) {
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		snprintf(buff, sizeof(buff), "%u,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu\n",
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		snprintf(buff, sizeof(buff), "%u,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu\n",
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
									gMonitor_SDRAMC_Info[idx].time,
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX00],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX01],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX10],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX11],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX12],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX13],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX20],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX21],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX22],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX23],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX30],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX31],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX32],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX33],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX40],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX50],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX51],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX60],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_RB0],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_RB1],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_RB2],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_DS0],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_DS1],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_SDM],
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VE0],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VE1],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VE2],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD0],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD1],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD2]);
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCOREW],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCORER0],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCORER1],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD0],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD1],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD2],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCPU],
									gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCOREM]);
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

		ttl_time		+= gMonitor_SDRAMC_Info[idx].time;
		ttl_mxic0     += (gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX00] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX01]);
		ttl_mxic1     += (gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX10] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX11] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX12] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX13]);
		ttl_mxic2     += (gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX10] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX21] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX22] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX23]);
		ttl_mxic3     += (gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX10] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX31] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX32] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX33]);
		ttl_mxic4     +=  gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX40];
		ttl_mxic5     += (gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX50] + gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX51]);
		ttl_mxic6     +=  gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_MX60];
		ttl_ribery0   += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_RB0];
		ttl_ribery1   += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_RB1];
		ttl_ribery2   += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_RB2];
		ttl_h265_com0 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_DS0];
		ttl_h265_com1 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_DS1];
		ttl_h265_com2 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_SDM];
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		ttl_h265_enc0 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VE0];
		ttl_h265_enc1 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VE1];
		ttl_h265_enc2 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VE2];
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		ttl_h265_enc0 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCOREW];
		ttl_h265_enc1 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCORER0];
		ttl_h265_enc2 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCORER1];
		ttl_h265_enc3 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCPU];
		ttl_h265_enc4 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VCOREM];
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		ttl_h265_dec0 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD0];
		ttl_h265_dec1 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD1];
		ttl_h265_dec2 += gMonitor_SDRAMC_Info[idx].result.data[E_IM_SDRAMC_PORT_VD2];
		if ( type == 1 ) {
			BF_Fs_If_Write( file_id, buff, strlen(buff), &written_size );
		}
		cnt++;
	}

	if((type == 0) && (cnt > 0)){
		ttl_mxic	= ttl_mxic0 + ttl_mxic1 + ttl_mxic2 + ttl_mxic3 + ttl_mxic4 + ttl_mxic5 + ttl_mxic6;
		ttl_ribery	= ttl_ribery0 + ttl_ribery1 + ttl_ribery2;
		ttl_h265	= ttl_h265_com0 + ttl_h265_com1 + ttl_h265_com2
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
					+ ttl_h265_enc0 + ttl_h265_enc1 + ttl_h265_enc2
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
					+ ttl_h265_enc0 + ttl_h265_enc1 + ttl_h265_enc2 + ttl_h265_enc3 + ttl_h265_enc4
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
					+ ttl_h265_dec0 + ttl_h265_dec1 + ttl_h265_dec2;
		ttl			= ttl_mxic + ttl_ribery + ttl_h265;

		printf("- SDRAMC %dch -\n", gSDRAMC_Ch);
		printf("Samples = %d\n", cnt);
		printf("Total Time = %llu[msec]\n", ttl_time / 1000);

		printf("mxic0      = %llu (Ave=%llu)\n", ttl_mxic0, ttl_mxic0 / cnt);
		printf("mxic1      = %llu (Ave=%llu)\n", ttl_mxic1, ttl_mxic1 / cnt);
		printf("mxic2      = %llu (Ave=%llu)\n", ttl_mxic2, ttl_mxic2 / cnt);
		printf("mxic3      = %llu (Ave=%llu)\n", ttl_mxic3, ttl_mxic3 / cnt);
		printf("mxic4      = %llu (Ave=%llu)\n", ttl_mxic4, ttl_mxic4 / cnt);
		printf("mxic5      = %llu (Ave=%llu)\n", ttl_mxic5, ttl_mxic5 / cnt);
		printf("mxic6      = %llu (Ave=%llu)\n", ttl_mxic6, ttl_mxic6 / cnt);

		printf("ribery0    = %llu (Ave=%llu)\n", ttl_ribery0, ttl_ribery0 / cnt);
		printf("ribery1    = %llu (Ave=%llu)\n", ttl_ribery1, ttl_ribery1 / cnt);
		printf("ribery1    = %llu (Ave=%llu)\n", ttl_ribery2, ttl_ribery2 / cnt);

		printf("h265_com0  = %llu (Ave=%llu)\n", ttl_h265_com0, ttl_h265_com0 / cnt);
		printf("h265_com1  = %llu (Ave=%llu)\n", ttl_h265_com1, ttl_h265_com1 / cnt);
		printf("h265_com2  = %llu (Ave=%llu)\n", ttl_h265_com2, ttl_h265_com2 / cnt);

		printf("h265_enc0  = %llu (Ave=%llu)\n", ttl_h265_enc0, ttl_h265_enc0 / cnt);
		printf("h265_enc1  = %llu (Ave=%llu)\n", ttl_h265_enc1, ttl_h265_enc1 / cnt);
		printf("h265_enc2  = %llu (Ave=%llu)\n", ttl_h265_enc2, ttl_h265_enc2 / cnt);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		printf("h265_enc3  = %llu (Ave=%llu)\n", ttl_h265_enc3, ttl_h265_enc3 / cnt);
		printf("h265_enc4  = %llu (Ave=%llu)\n", ttl_h265_enc4, ttl_h265_enc4 / cnt);
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

		printf("h265_dec0  = %llu (Ave=%llu)\n", ttl_h265_dec0, ttl_h265_dec0 / cnt);
		printf("h265_dec1  = %llu (Ave=%llu)\n", ttl_h265_dec1, ttl_h265_dec1 / cnt);
		printf("h265_dec2  = %llu (Ave=%llu)\n", ttl_h265_dec2, ttl_h265_dec2 / cnt);

		printf("total      = %llu (Ave=%llu)\n", ttl, ttl / cnt);
	}
	
	// Log file close
	if(type == 1){
		BF_Fs_If_Close(file_id);
	}
}


/**
 * SDRAMC Monitor Start in specified time.
 */
INT32 Monitor_SDRAMC_Time( E_IM_SDRAMC_CH ch_no, UINT32 monitor_time, BOOL write, BOOL read, T_IM_SDRAMC_MONITOR *sdramc_mon )
{
	INT32 ret;

	if (monitor_time <= 0) {
		return -1;
	}

	/* start measure of arbitration access */
	Monitor_SDRAMC_Start( ch_no, E_IM_SDRAMC_MONMODE_SIZE, write, read, TRUE );

	/* delay task */
	OS_User_Dly_Tsk( monitor_time );

	/* stop measure of arbitration access */
	ret = Monitor_SDRAMC_Stop( sdramc_mon );
	if ( ret != 0 ) {
		return -1;
	}

	return 0;
}

VOID Monitor_SDRAMC_Add( T_IM_SDRAMC_MONITOR* const sdramc_mon_dst, const T_IM_SDRAMC_MONITOR* const sdramc_mon_arg1, const T_IM_SDRAMC_MONITOR* const sdramc_mon_arg2 )
{
	UINT32	loopcnt;

	for( loopcnt = 0; loopcnt < (sizeof(sdramc_mon_dst->data) / sizeof(sdramc_mon_dst->data[0])); loopcnt++ ) {
		sdramc_mon_dst->data[loopcnt] = sdramc_mon_arg1->data[loopcnt] + sdramc_mon_arg2->data[loopcnt];
	}
}

VOID Monitor_SDRAMC_Max( T_IM_SDRAMC_MONITOR* const sdramc_mon_dst, const T_IM_SDRAMC_MONITOR* const sdramc_mon_arg1, const T_IM_SDRAMC_MONITOR* const sdramc_mon_arg2 )
{
	UINT32	loopcnt;

	for( loopcnt = 0; loopcnt < (sizeof(sdramc_mon_dst->data) / sizeof(sdramc_mon_dst->data[0])); loopcnt++ ) {
		if( sdramc_mon_arg1->data[loopcnt] > sdramc_mon_arg2->data[loopcnt] ) {
			sdramc_mon_dst->data[loopcnt] = sdramc_mon_arg1->data[loopcnt];
		}
		else {
			sdramc_mon_dst->data[loopcnt] = sdramc_mon_arg2->data[loopcnt];
		}
	}
}

VOID Monitor_SDRAMC_Print( const T_IM_SDRAMC_MONITOR* const sdramc_mon )
{
	ULLONG				total = 0;
	ULONG				total_upper;
	ULONG				total_lower;
	E_IM_SDRAMC_PORT	port;
	UINT32				loopcnt;
	ULONG				data_mib;
	static const CHAR* const port_name_tbl[] = {
		[E_IM_SDRAMC_PORT_MX00]	= "access_size_mxic00",
		[E_IM_SDRAMC_PORT_MX01]	= "access_size_mxic01",
		[E_IM_SDRAMC_PORT_MX10]	= "access_size_mxic10",
		[E_IM_SDRAMC_PORT_MX11]	= "access_size_mxic11",
		[E_IM_SDRAMC_PORT_MX12]	= "access_size_mxic12",
		[E_IM_SDRAMC_PORT_MX13]	= "access_size_mxic13",
		[E_IM_SDRAMC_PORT_MX20]	= "access_size_mxic20",
		[E_IM_SDRAMC_PORT_MX21]	= "access_size_mxic21",
		[E_IM_SDRAMC_PORT_MX22]	= "access_size_mxic22",
		[E_IM_SDRAMC_PORT_MX23]	= "access_size_mxic23",
		[E_IM_SDRAMC_PORT_MX30]	= "access_size_mxic30",
		[E_IM_SDRAMC_PORT_MX31]	= "access_size_mxic31",
		[E_IM_SDRAMC_PORT_MX32]	= "access_size_mxic32",
		[E_IM_SDRAMC_PORT_MX33]	= "access_size_mxic33",
		[E_IM_SDRAMC_PORT_MX40]	= "access_size_mxic40",
		[E_IM_SDRAMC_PORT_MX50]	= "access_size_mxic50",
		[E_IM_SDRAMC_PORT_MX51]	= "access_size_mxic51",
		[E_IM_SDRAMC_PORT_MX60]	= "access_size_mxic60",
		[E_IM_SDRAMC_PORT_RB0]	= "access_size_ribery0",
		[E_IM_SDRAMC_PORT_RB1]	= "access_size_ribery1",
		[E_IM_SDRAMC_PORT_RB2]	= "access_size_ribery2",
		[E_IM_SDRAMC_PORT_DS0]	= "access_size_h265_com0(DS0)",
		[E_IM_SDRAMC_PORT_DS1]	= "access_size_h265_com1(DS1)",
		[E_IM_SDRAMC_PORT_SDM]	= "access_size_h265_com2(SDM)",
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		[E_IM_SDRAMC_PORT_VE0]	= "access_size_h265_enc0(VE0)",
		[E_IM_SDRAMC_PORT_VE1]	= "access_size_h265_enc1(VE1)",
		[E_IM_SDRAMC_PORT_VE2]	= "access_size_h265_enc2(VE2)",
		[E_IM_SDRAMC_PORT_VD0]	= "access_size_h265_dec0(VD0)",
		[E_IM_SDRAMC_PORT_VD1]	= "access_size_h265_dec1(VD1)",
		[E_IM_SDRAMC_PORT_VD2]	= "access_size_h265_dec2(VD2)",

// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		[E_IM_SDRAMC_PORT_VCOREW]	= "access_size_h265_enc0(VCOREW)",
		[E_IM_SDRAMC_PORT_VCORER0]	= "access_size_h265_enc1(VCORER0)",
		[E_IM_SDRAMC_PORT_VCORER1]	= "access_size_h265_enc2(VCORER1)",
		[E_IM_SDRAMC_PORT_VD0]		= "access_size_h265_dec0(VD0)",
		[E_IM_SDRAMC_PORT_VD1]		= "access_size_h265_dec1(VD1)",
		[E_IM_SDRAMC_PORT_VD2]		= "access_size_h265_dec2(VD2)",
		[E_IM_SDRAMC_PORT_VCPU]		= "access_size_h265_enc3(VCPU)",
		[E_IM_SDRAMC_PORT_VCOREM]	= "access_size_h265_enc4(VCOREM)",
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	};
	static const E_IM_SDRAMC_PORT port_list[] = {
		E_IM_SDRAMC_PORT_MX00,
		E_IM_SDRAMC_PORT_MX01,
		E_IM_SDRAMC_PORT_MX10,
		E_IM_SDRAMC_PORT_MX11,
		E_IM_SDRAMC_PORT_MX12,
		E_IM_SDRAMC_PORT_MX13,
		E_IM_SDRAMC_PORT_MX20,
		E_IM_SDRAMC_PORT_MX21,
		E_IM_SDRAMC_PORT_MX22,
		E_IM_SDRAMC_PORT_MX23,
		E_IM_SDRAMC_PORT_MX30,
		E_IM_SDRAMC_PORT_MX31,
		E_IM_SDRAMC_PORT_MX32,
		E_IM_SDRAMC_PORT_MX33,
		E_IM_SDRAMC_PORT_MX40,
		E_IM_SDRAMC_PORT_MX50,
		E_IM_SDRAMC_PORT_MX51,
		E_IM_SDRAMC_PORT_MX60,
		E_IM_SDRAMC_PORT_RB0,
		E_IM_SDRAMC_PORT_RB1,
		E_IM_SDRAMC_PORT_RB2,
		E_IM_SDRAMC_PORT_DS0,
		E_IM_SDRAMC_PORT_DS1,
		E_IM_SDRAMC_PORT_SDM,
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		E_IM_SDRAMC_PORT_VE0,
		E_IM_SDRAMC_PORT_VE1,
		E_IM_SDRAMC_PORT_VE2,
		E_IM_SDRAMC_PORT_VD0,
		E_IM_SDRAMC_PORT_VD1,
		E_IM_SDRAMC_PORT_VD2,
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		E_IM_SDRAMC_PORT_VCOREW,
		E_IM_SDRAMC_PORT_VCORER0,
		E_IM_SDRAMC_PORT_VCORER1,
		E_IM_SDRAMC_PORT_VD0,
		E_IM_SDRAMC_PORT_VD1,
		E_IM_SDRAMC_PORT_VD2,
		E_IM_SDRAMC_PORT_VCPU,
		E_IM_SDRAMC_PORT_VCOREM,
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	};

	for ( loopcnt = 0; loopcnt < (sizeof(port_list) / sizeof(port_list[0])); loopcnt++ ) {
		port = port_list[loopcnt];
		data_mib = sdramc_mon->data[ port ] * 8 / 1024 / 1024;
		total += sdramc_mon->data[ port ];
		printf("%-26s = %5luM (%10lu)\n",
				port_name_tbl[port],
				data_mib,
				sdramc_mon->data[port] *8
				);
	}

	total_upper = (ULONG)((total *8) / 1000000000ull);
	total_lower = (ULONG)((total *8) % 1000000000ull);
	if( total_upper == 0ul ) {
		printf("access_size_total(all)     = %5luM (%10lu)\n", (ULONG)(total * 8 / 1024 / 1024), total_lower);
	}
	else {
		printf("access_size_total(all)     = %5luM (%lu%9.9lu)\n", (ULONG)(total * 8 / 1024 / 1024), total_upper, total_lower);
	}
}

VOID Monitor_SDRAMC_Print_MB( const T_IM_SDRAMC_MONITOR* const sdramc_mon )
{
	ULLONG				total = 0;
	ULLONG				sub_total = 0;
	E_IM_SDRAMC_PORT	port;
	UINT32				print_set_idx;
	UINT32				port_loopcnt;
	const CHAR*			spacer_str;
	static const CHAR* const port_name_tbl[] = {
		[E_IM_SDRAMC_PORT_MX00]	= "mxic00",
		[E_IM_SDRAMC_PORT_MX01]	= "mxic01",
		[E_IM_SDRAMC_PORT_MX10]	= "mxic10",
		[E_IM_SDRAMC_PORT_MX11]	= "mxic11",
		[E_IM_SDRAMC_PORT_MX12]	= "mxic12",
		[E_IM_SDRAMC_PORT_MX13]	= "mxic13",
		[E_IM_SDRAMC_PORT_MX20]	= "mxic20",
		[E_IM_SDRAMC_PORT_MX21]	= "mxic21",
		[E_IM_SDRAMC_PORT_MX22]	= "mxic22",
		[E_IM_SDRAMC_PORT_MX23]	= "mxic23",
		[E_IM_SDRAMC_PORT_MX30]	= "mxic30",
		[E_IM_SDRAMC_PORT_MX31]	= "mxic31",
		[E_IM_SDRAMC_PORT_MX32]	= "mxic32",
		[E_IM_SDRAMC_PORT_MX33]	= "mxic33",
		[E_IM_SDRAMC_PORT_MX40]	= "mxic40",
		[E_IM_SDRAMC_PORT_MX50]	= "mxic50",
		[E_IM_SDRAMC_PORT_MX51]	= "mxic51",
		[E_IM_SDRAMC_PORT_MX60]	= "mxic60",
		[E_IM_SDRAMC_PORT_RB0]	= "ribery0",
		[E_IM_SDRAMC_PORT_RB1]	= "ribery1",
		[E_IM_SDRAMC_PORT_RB2]	= "ribery2",
		[E_IM_SDRAMC_PORT_DS0]	= "com0(DS0)",
		[E_IM_SDRAMC_PORT_DS1]	= "com1(DS1)",
		[E_IM_SDRAMC_PORT_SDM]	= "com2(SDM)",
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		[E_IM_SDRAMC_PORT_VE0]	= "enc0(VE0)",
		[E_IM_SDRAMC_PORT_VE1]	= "enc1(VE1)",
		[E_IM_SDRAMC_PORT_VE2]	= "enc2(VE2)",
		[E_IM_SDRAMC_PORT_VD0]	= "dec0(VD0)",
		[E_IM_SDRAMC_PORT_VD1]	= "dec1(VD1)",
		[E_IM_SDRAMC_PORT_VD2]	= "dec2(VD2)",
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		[E_IM_SDRAMC_PORT_VCOREW]	= "enc0(VCOREW)",
		[E_IM_SDRAMC_PORT_VCORER0]	= "enc1(VCORER0)",
		[E_IM_SDRAMC_PORT_VCORER1]	= "enc2(VCORER1)",
		[E_IM_SDRAMC_PORT_VD0]		= "dec0(VD0)",
		[E_IM_SDRAMC_PORT_VD1]		= "dec1(VD1)",
		[E_IM_SDRAMC_PORT_VD2]		= "dec2(VD2)",
		[E_IM_SDRAMC_PORT_VCPU]		= "enc3(VCPU)",
		[E_IM_SDRAMC_PORT_VCOREM]	= "enc4(VCOREM)",
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	};
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	static const struct {
		const CHAR*		sub_total_name;
		E_IM_SDRAMC_PORT port_list[4];
	} print_set[] = {
		{
			.sub_total_name = "mxic0",
			.port_list = { E_IM_SDRAMC_PORT_MX00, E_IM_SDRAMC_PORT_MX01, E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "mxic1",
			.port_list = { E_IM_SDRAMC_PORT_MX10, E_IM_SDRAMC_PORT_MX11, E_IM_SDRAMC_PORT_MX12, E_IM_SDRAMC_PORT_MX13, },
		},
		{
			.sub_total_name = "mxic2",
			.port_list = { E_IM_SDRAMC_PORT_MX20, E_IM_SDRAMC_PORT_MX21, E_IM_SDRAMC_PORT_MX22, E_IM_SDRAMC_PORT_MX23, },
		},
		{
			.sub_total_name = "mxic3",
			.port_list = { E_IM_SDRAMC_PORT_MX30, E_IM_SDRAMC_PORT_MX31, E_IM_SDRAMC_PORT_MX32, E_IM_SDRAMC_PORT_MX33, },
		},
		{
			.sub_total_name = "mxic4",
			.port_list = { E_IM_SDRAMC_PORT_MX40, E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "mxic5",
			.port_list = { E_IM_SDRAMC_PORT_MX50, E_IM_SDRAMC_PORT_MX51, E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "mxic6",
			.port_list = { E_IM_SDRAMC_PORT_MX60, E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "ribery",
			.port_list = { E_IM_SDRAMC_PORT_RB0,  E_IM_SDRAMC_PORT_RB1,  E_IM_SDRAMC_PORT_RB2,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "h265_com",
			.port_list = { E_IM_SDRAMC_PORT_DS0,  E_IM_SDRAMC_PORT_DS1,  E_IM_SDRAMC_PORT_SDM,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "h265_enc",
			.port_list = { E_IM_SDRAMC_PORT_VE0,  E_IM_SDRAMC_PORT_VE1,  E_IM_SDRAMC_PORT_VE2,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "h265_dec",
			.port_list = { E_IM_SDRAMC_PORT_VD0,  E_IM_SDRAMC_PORT_VD1,  E_IM_SDRAMC_PORT_VD2,  E_IM_SDRAMC_PORT_MAX, },
		},
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	static const struct {
		const CHAR*		sub_total_name;
		E_IM_SDRAMC_PORT port_list[5];
	} print_set[] = {
		{
			.sub_total_name = "mxic0",
			.port_list = { E_IM_SDRAMC_PORT_MX00, E_IM_SDRAMC_PORT_MX01, E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "mxic1",
			.port_list = { E_IM_SDRAMC_PORT_MX10, E_IM_SDRAMC_PORT_MX11, E_IM_SDRAMC_PORT_MX12, E_IM_SDRAMC_PORT_MX13,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "mxic2",
			.port_list = { E_IM_SDRAMC_PORT_MX20, E_IM_SDRAMC_PORT_MX21, E_IM_SDRAMC_PORT_MX22, E_IM_SDRAMC_PORT_MX23,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "mxic3",
			.port_list = { E_IM_SDRAMC_PORT_MX30, E_IM_SDRAMC_PORT_MX31, E_IM_SDRAMC_PORT_MX32, E_IM_SDRAMC_PORT_MX33,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "mxic4",
			.port_list = { E_IM_SDRAMC_PORT_MX40, E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "mxic5",
			.port_list = { E_IM_SDRAMC_PORT_MX50, E_IM_SDRAMC_PORT_MX51, E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "mxic6",
			.port_list = { E_IM_SDRAMC_PORT_MX60, E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "ribery",
			.port_list = { E_IM_SDRAMC_PORT_RB0,  E_IM_SDRAMC_PORT_RB1,  E_IM_SDRAMC_PORT_RB2,  E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "h265_com",
			.port_list = { E_IM_SDRAMC_PORT_DS0,  E_IM_SDRAMC_PORT_DS1,  E_IM_SDRAMC_PORT_SDM,  E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX, },
		},
		{
			.sub_total_name = "h265_enc",
			.port_list = { E_IM_SDRAMC_PORT_VCOREW,  E_IM_SDRAMC_PORT_VCORER0,  E_IM_SDRAMC_PORT_VCORER1,  E_IM_SDRAMC_PORT_VCPU,  E_IM_SDRAMC_PORT_VCOREM, },
		},
		{
			.sub_total_name = "h265_dec",
			.port_list = { E_IM_SDRAMC_PORT_VD0,  E_IM_SDRAMC_PORT_VD1,  E_IM_SDRAMC_PORT_VD2,  E_IM_SDRAMC_PORT_MAX,  E_IM_SDRAMC_PORT_MAX, },
		},
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	};

	for( print_set_idx = 0; print_set_idx < (sizeof(print_set) / sizeof(print_set[0])); print_set_idx++ ) {
		// Calc subtotal.
		sub_total = 0ull;
		for( port_loopcnt = 0; port_loopcnt < (sizeof(print_set[0].port_list) / sizeof(print_set[0].port_list[0])); port_loopcnt++ ) {
			port = print_set[print_set_idx].port_list[port_loopcnt];
			if( port == E_IM_SDRAMC_PORT_MAX ) {
				continue;
			}
			sub_total += sdramc_mon->data[ port ];
		}

		// Print subtotal.
		printf("%-8s = %5lu   (",
				print_set[print_set_idx].sub_total_name,
				(ULONG)Monitor_SDRAMC_Roundup_MiB( sub_total * 8 )
			  );

		// Print indivisual.
		spacer_str = "";
		for( port_loopcnt = 0; port_loopcnt < (sizeof(print_set[0].port_list) / sizeof(print_set[0].port_list[0])); port_loopcnt++ ) {
			port = print_set[print_set_idx].port_list[port_loopcnt];
			if( port == E_IM_SDRAMC_PORT_MAX ) {
				continue;
			}
			printf("%s%-9s = %5lu",
					spacer_str,
					port_name_tbl[port],
					Monitor_SDRAMC_Roundup_MiB( sdramc_mon->data[ port ] * 8 )
					);
			spacer_str = "   ";
		}
		total += sub_total;
		printf( ")\n" );
	}

	// Print total.
	printf("total    = %5lu[MiB]\n", (ULONG)Monitor_SDRAMC_Roundup_MiB(total * 8));
}
#endif
