/**
 * @file		dispatch_log.c
 * @brief		task dispatch log api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifdef CO_DISPATCH_CHECK_ON

#include <string.h>
#include "ddim_typedef.h"
#include "dispatch_log.h"
#include "os_user_custom.h"
#include "measure_time.h"
#include "fs_if.h"
#include "debug.h"
#include "dd_arm.h"
#include "fj_filesystem.h"

extern UINT32 Task_Control_Get_Ctxtsk_Oname(UINT32 tskid, UINT32 cpuid, char **oname);

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_DISPATCH_LOG_CPU_NUM		(3)
#define D_DISPATCH_LOG_TBL_LEN		(13)
#define D_DISPATCH_LOG_TBL_SIZ		(1 << D_DISPATCH_LOG_TBL_LEN)
#define D_DISPATCH_LOG_TBL_SIZ_1	(D_DISPATCH_LOG_TBL_SIZ - 1)

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
typedef struct {
	ULONG			tim;
	OS_USER_UINT	cpuid;
	OS_USER_ID		tskid;
	OS_USER_UB		oname[12];
} T_DISPATCH_LOG;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
volatile INT32			gDispatch_Log_Tbl_Idx[D_DISPATCH_LOG_CPU_NUM] = {0,0,0};
volatile T_DISPATCH_LOG	gDispatch_Log_Tbl[D_DISPATCH_LOG_CPU_NUM][D_DISPATCH_LOG_TBL_SIZ];

/*---------------------------------------------------------------*/
/* Macro														 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Local Function												 */
/*---------------------------------------------------------------*/
static VOID os_dispatch_log_print_cell( UCHAR type, ULONG file_id, UINT32 cpuid, UINT32 index )
{
	CHAR	out_buf[64];
	ULONG	written_size;

	snprintf( out_buf,
			sizeof(out_buf),
			 "%lu.%06lu,%d,%08x,%s\n",
			 gDispatch_Log_Tbl[cpuid][index].tim / 1000000,
			 gDispatch_Log_Tbl[cpuid][index].tim % 1000000,
			 (int)(gDispatch_Log_Tbl[cpuid][index].cpuid),
 			 (int)(gDispatch_Log_Tbl[cpuid][index].tskid),
			 gDispatch_Log_Tbl[cpuid][index].oname );
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
VOID os_dispatch_log_hook_exec( OS_USER_ID taskid, OS_USER_INT lsid )
{
	UINT32			cpuid;
	char*			oname;
	UINT32			ercd;
	
	cpuid = Dd_ARM_Get_CPU_ID();
	gDispatch_Log_Tbl[cpuid][gDispatch_Log_Tbl_Idx[cpuid]].tim   = BF_Get_Time_us();
	gDispatch_Log_Tbl[cpuid][gDispatch_Log_Tbl_Idx[cpuid]].cpuid = cpuid;
	gDispatch_Log_Tbl[cpuid][gDispatch_Log_Tbl_Idx[cpuid]].tskid = taskid;
	ercd = Task_Control_Get_Ctxtsk_Oname(taskid, cpuid, &oname);
	if ( ercd == 0 ) {
		memset(&gDispatch_Log_Tbl[cpuid][gDispatch_Log_Tbl_Idx[cpuid]].oname, 0, sizeof(gDispatch_Log_Tbl[cpuid][gDispatch_Log_Tbl_Idx[cpuid]].oname));
		for(int i=0; i<8; i++){
			*(char *)(gDispatch_Log_Tbl[cpuid][gDispatch_Log_Tbl_Idx[cpuid]].oname + i) = *(char *)(oname + i);
		}
	}
	else {
		memset(&gDispatch_Log_Tbl[cpuid][gDispatch_Log_Tbl_Idx[cpuid]].oname, 0, sizeof(gDispatch_Log_Tbl[cpuid][gDispatch_Log_Tbl_Idx[cpuid]].oname));
	}
	gDispatch_Log_Tbl_Idx[cpuid] = D_DISPATCH_LOG_TBL_SIZ_1 & (gDispatch_Log_Tbl_Idx[cpuid] + 1);
}

VOID os_dispatch_log_hook_stop( OS_USER_ID tskid, OS_USER_INT lsid, OS_USER_UINT tskstat )
{
}

VOID OS_Dispatch_Log_Start(VOID)
{
	TD_OS_USER_HDSP	hdsp;

	hdsp.exec = os_dispatch_log_hook_exec;
	hdsp.stop = os_dispatch_log_hook_stop;
	OS_User_Td_Hok_Dsp(&hdsp);
}

VOID OS_Dispatch_Log_Stop(VOID)
{
	OS_User_Td_Hok_Dsp(NULL);
}

VOID OS_Dispatch_Log_Clear(VOID)
{
	UINT32	cpuid;

	for(cpuid=0; cpuid<D_DISPATCH_LOG_CPU_NUM; cpuid++){
		gDispatch_Log_Tbl_Idx[cpuid] = 0;
		memset(&gDispatch_Log_Tbl[cpuid], 0, sizeof(gDispatch_Log_Tbl[cpuid]));
	}
}

VOID OS_Dispatch_Log_Output( UCHAR type, const char* file_name )
{
	ULONG	file_id = 0;
	UINT32	index;
	ULONG	lasttim = 0;
	UINT32	cpuid;

	// Log file open
	if( type == 1 ){	// to file
		// Parameter check
		if( (file_name == NULL) || (strlen((char*)file_name) > 16) ){
			BF_Debug_Print_Error(("OS_Dispatch_Log_Output() File name error!\n"));
			return;
		}

		file_id = FJ_Fs_Open( (CHAR*)file_name, FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT );
		if( file_id == 0 ){  
			BF_Debug_Print_Error(("OS_Dispatch_Log_Output() FJ_Fs_Open %s failed! (%d)\n", file_name, file_id));
			return;
		}
	}

	// Log data output
	for(cpuid=0; cpuid<D_DISPATCH_LOG_CPU_NUM; cpuid++){
		lasttim = 0;
		if( gDispatch_Log_Tbl[cpuid][gDispatch_Log_Tbl_Idx[cpuid]].tim > 0 ){
			for( index = gDispatch_Log_Tbl_Idx[cpuid]; index < D_DISPATCH_LOG_TBL_SIZ; index++ ) {
				if( gDispatch_Log_Tbl[cpuid][index].tim > lasttim ){
					os_dispatch_log_print_cell( type, file_id, cpuid, index );
					lasttim = gDispatch_Log_Tbl[cpuid][index].tim;
				}
			}
		}
		for( index = 0; index < gDispatch_Log_Tbl_Idx[cpuid]; index++ ){
			if( gDispatch_Log_Tbl[cpuid][index].tim > lasttim ){
				os_dispatch_log_print_cell( type, file_id, cpuid, index );
				lasttim = gDispatch_Log_Tbl[cpuid][index].tim;
			}
		}
	}

	if( type == 1 ){	// to file
		FJ_Fs_Close( file_id );
	}
}
#endif	// CO_DISPATCH_CHECK_ON
