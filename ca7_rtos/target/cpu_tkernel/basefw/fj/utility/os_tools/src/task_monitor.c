/**
 * @file		task_monitor.c
 * @brief		task monitor log api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifdef CO_TASK_MONITOR_ON

#include <stdio.h>
#include <string.h>
#include "ddim_typedef.h"
#include "task_monitor.h"
#include "fs_if.h"
#include "debug.h"
#include "fj_filesystem.h"

extern UINT32 Task_Control_Get_Ctxtsk(UINT32 cpuid, UINT32 *tskid, char **oname);

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_TASK_MONI_TBL_LEN			(11)
#define D_TASK_MONI_TBL_SIZ			(1 << D_TASK_MONI_TBL_LEN)
#define D_TASK_MONI_TBL_SIZ_1		(D_TASK_MONI_TBL_SIZ - 1)

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
typedef struct {
	UINT32		cpuid;
	OS_USER_ID	tskid;
	OS_USER_UB	oname[12];
} T_TASK_MONI_LOG;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
volatile INT32				gTask_Moni_Tbl_Idx = 0;
volatile T_TASK_MONI_LOG	gTask_Moni_Tbl[D_TASK_MONI_TBL_SIZ];

//#define CO_CHECK_START_WHEN_BOOT
#ifdef CO_CHECK_START_WHEN_BOOT
BOOL gTaskMonitorFlag = TRUE;
#else
BOOL gTaskMonitorFlag = FALSE;
#endif

/*---------------------------------------------------------------*/
/* Macro														 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Local Function												 */
/*---------------------------------------------------------------*/
static VOID os_task_log_print_cell(UCHAR type, ULONG file_id, UINT32 index)
{
	CHAR	out_buf[32];
	ULONG	written_size;
	
	snprintf( out_buf,
			  sizeof(out_buf),
			  "%d,%08x,%s\n",
			  gTask_Moni_Tbl[index].cpuid,
  			  gTask_Moni_Tbl[index].tskid,
   			  gTask_Moni_Tbl[index].oname );
	if(type == 0){	// to terminal
		printf( (char*)out_buf );
	}
	else{			// to file
		FJ_Fs_Write( file_id, (UINT32*)out_buf, strlen((char*)out_buf), &written_size);
	}
}

/*---------------------------------------------------------------*/
/* Global Function												 */
/*---------------------------------------------------------------*/
VOID OS_Task_Mointor(VOID)
{
	UINT32			cpuid;
	OS_USER_ID		tskid;
	char*			oname;
	UINT32			ercd;
	
	if( gTaskMonitorFlag ) {
		for(cpuid = 0; cpuid < 3; cpuid++ ) {
			ercd = Task_Control_Get_Ctxtsk(cpuid, (UINT32*)&tskid, &oname);
			if ( ercd != 1 ) {
				gTask_Moni_Tbl[gTask_Moni_Tbl_Idx].cpuid = cpuid;
				gTask_Moni_Tbl[gTask_Moni_Tbl_Idx].tskid = 0;
				memset(&gTask_Moni_Tbl[gTask_Moni_Tbl_Idx].oname, 0, sizeof(gTask_Moni_Tbl[gTask_Moni_Tbl_Idx].oname));
				if ( ercd == 0 ) {
					gTask_Moni_Tbl[gTask_Moni_Tbl_Idx].tskid = tskid;
					for(int i=0; i<8; i++){
						*(char *)(gTask_Moni_Tbl[gTask_Moni_Tbl_Idx].oname + i) = *(char *)(oname + i);
					}
				}
				gTask_Moni_Tbl_Idx = gTask_Moni_Tbl_Idx + 1;
				if( gTask_Moni_Tbl_Idx > D_TASK_MONI_TBL_SIZ_1 ) {
					gTaskMonitorFlag = FALSE;
					return;
				}
			}
		}
	}
}

VOID OS_Task_Mon_Log_Output(UCHAR type, const CHAR* file_name)
{
	ULONG	file_id = 0;
	CHAR	file_path[16] = "I:\\";
	UINT32	index;
	
	// Log file open
	if( type == 1 ){	// to file
		// Parameter check
		if( (file_name == NULL) || (strlen((char*)file_name) > 8) ){
			BF_Debug_Print_Error(("OS_Task_Mon_Log_Output() File name error!\n"));
			return;
		}
		
		sprintf( (char*)&file_path[3], "%s.TSK", (char*)file_name );
		file_id = FJ_Fs_Open( (CHAR*)file_path, FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT );
		if( file_id == 0 ){  
			BF_Debug_Print_Error(("OS_Task_Mon_Log_Output() FJ_Fs_Open %s failed! (%d)\n", file_path, file_id));
			return;
		}
	}
	
	// Log data output
	for( index = gTask_Moni_Tbl_Idx; index < D_TASK_MONI_TBL_SIZ; index++ ) {
		os_task_log_print_cell( type, file_id, index );
	}
	for( index = 0; index < gTask_Moni_Tbl_Idx; index++ ){
		os_task_log_print_cell( type, file_id, index );
	}
	
	if( type == 1 ){	// to file
		FJ_Fs_Close( file_id );
	}
}

VOID OS_Task_Mon_Set_Flag(BOOL task_monitor_flag)
{
	gTaskMonitorFlag = task_monitor_flag;
}

VOID OS_Task_Mon_Log_Clear(VOID)
{
	gTaskMonitorFlag = FALSE;
	gTask_Moni_Tbl_Idx = 0;
	memset(&gTask_Moni_Tbl, 0, sizeof(gTask_Moni_Tbl));
}
#endif	// CO_TASK_MONITOR_ON
