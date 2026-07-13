/**
 * @file		mba_if.c
 * @brief		MBA Log I/F
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

/************************************************************************
 Include
*************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include "ddim_typedef.h"
#include "dd_arm.h"
#include "mba_log.h"
#include "mba_if.h"
#include "mba_buf.h"
#include "fj_filesystem.h"
#include "measure_time.h"
#include "bf_arm.h"
#include "os_user_custom.h"
#include "dd_arm.h"
#include <stdio.h>
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
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#if defined(CO_DEBUG_MBALOG_ON) || defined(CO_BF_MBALOG_ON)
// MBA log data max count
UINT32			gMBA_Log_Max = D_MBALOG_RECORD_MAX;
UINT32			gMBA_Surface_Max = D_MBALOG_AREA_MAX;

// MBA write pointer
ULONG*			gMBA_Write_Pointer;
ULONG			gMBA_Write_Pointer_real;

// MBA overlap flg
UCHAR*			gMBA_Overlap_Flg;
UCHAR			gMBA_Overlap_Flg_real;

// MBA log data array
#ifdef DEBUG_ON_PC
T_MBA_LOG_DATA gMBA_Log_Data[D_MBALOG_AREA_MAX][D_MBALOG_RECORD_MAX];
#else
T_MBA_LOG_DATA gMBA_Log_Data[D_MBALOG_AREA_MAX][D_MBALOG_RECORD_MAX];
#endif

// Kind of MBA log
UINT32			gMBA_Kind = D_MBA_KIND_DEFAULT;

// Flag of log overwrite
BOOL			gMBA_Overwrite_Flag = FALSE;
UINT32			gMBA_Outbuf_Size_Main = MBALOG_OUTBUF_FLUSH_THRESH;
UINT32			gMBA_Outbuf_Size_Sub = MBALOG_OUTBUF_MAX_LENGTH;
UCHAR			gMBA_Outbuf[MBALOG_OUTBUF_MAX_LENGTH];

static UINT32	g_mba_if_critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
#endif	/* CO_DEBUG_MBALOG_ON or CO_BF_MBALOG_ON */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
#if defined(CO_DEBUG_MBALOG_ON) || defined(CO_BF_MBALOG_ON)
/************************************************************************
 Program (BaseFW->MBA Lib)
*************************************************************************/
/**
 * Start Memory Log (calls MBA.a)
 */
VOID BF_MBA_IF_Start( VOID )
{
//
// MBA log Kind set (Heap)
//
#if defined (CO_DEBUG_MBALOG_HEAP_ONLY)
	gMBA_Kind = D_MBA_KIND_HEAP;
#elif defined(CO_DEBUG_MBALOG_HEAP_ON)
	gMBA_Kind = (D_MBA_KIND_DEFAULT | D_MBA_KIND_HEAP);
#endif

//
// MBA log Overwrite flag set
//
#if defined(CO_DEBUG_MBALOG_OVERWRITE)
	gMBA_Overwrite_Flag = TRUE;
#else
	gMBA_Overwrite_Flag = FALSE;
#endif

	gMBA_Overlap_Flg = &gMBA_Overlap_Flg_real;		// tentative
	gMBA_Write_Pointer = &gMBA_Write_Pointer_real;	// tentative
	MBA_Log_Start( (void*)&gMBALog_Id_List );
}

/**
 * End Memory Log (calls MBA.a)
 */
VOID BF_MBA_IF_End( VOID )
{
	MBA_Log_End();
}

/**
 * Print Memory Log (calls MBA.a)
 */
VOID BF_MBA_IF_Printf_TaskID( const UINT32 kind, const FJ_MBALOG_FUNC_ID func_id, const FJ_MBALOG_TYPE type, const char* const fmt, ... )
{
	va_list	arg;
	va_start( arg, fmt );		// pgr0039
	MBA_Log_Printf( kind, (UCHAR)func_id, (UCHAR)type, fmt, arg );
	va_end( arg );
}

/**
 * Print Memory Log (calls MBA.a)
 */
VOID BF_MBA_IF_Printf_LineID( const UINT32 kind, const FJ_MBALOG_LINE_ID line_id, const FJ_MBALOG_FUNC_ID func_id, const FJ_MBALOG_TYPE type, const char* const fmt, ... )
{
	va_list	arg;
	va_start( arg, fmt );		// pgr0039
	MBA_Log_Printf_LineID( kind, (UCHAR)line_id, (UCHAR)func_id, (UCHAR)type, fmt, arg );
	va_end( arg );
}

/**
 * Output Memory Log (calls MBA.a)
 */
VOID BF_MBA_IF_Output( const E_MBALOG_OUT_TYPE type, const CHAR* const file_name )
{
	MBA_Log_Output( type, file_name );
}

/**
 * Output Memory Log (calls MBA.a)
 */
VOID BF_MBA_IF_Output_Select( const E_MBALOG_OUT_TYPE type, const UINT32 surface_no, const CHAR* const file_name )
{
	if( surface_no >= gMBA_Surface_Max ){
		return;
	}
	MBA_Log_Output_Select( type, surface_no, file_name );
}

/**
 * Output Memory Log (calls MBA.a)
 */
VOID BF_MBA_IF_Output_All( const E_MBALOG_OUT_TYPE type, const CHAR* const file_name )
{
	MBA_Log_Output_All( type, file_name );
}

/**
 * Clear Memory Log (calls MBA.a)
 */
VOID BF_MBA_IF_Clear( VOID )
{
	MBA_Log_Clear();
}

/**
 * Remove Log File (calls MBA.a)
 */
VOID BF_MBA_IF_Remove( const CHAR* const file_name )
{
	MBA_Log_Remove( file_name );
}

/**
 * Update Surface No. (calls MBA.a)
 */
VOID BF_MBA_IF_Update_Surface_No( VOID )
{
	MBA_Log_Update_Surface_No();
}

/************************************************************************
 Program (Host->BaseFW->MBA Lib)
*************************************************************************/
VOID fj_mbalog_printf( UCHAR func_id, FJ_MBALOG_TYPE type, const char* fmt, va_list arg )
{
#ifdef CO_DEBUG_MBALOG_ON
	MBA_Log_Printf( D_MBA_KIND_HOST, func_id, (UCHAR)type, fmt, arg );
#endif
}

/**
 * Output Memory Log as Text Format
 */
VOID fj_mbalog_output( const CHAR* file_name )
{
#ifdef CO_DEBUG_MBALOG_ON
	MBA_Log_Output( E_MBALOG_OUT_FILE, file_name );
#endif
}

/**
 * Output Memory Log as Binary Format
 */
VOID fj_mbalog_output_binary( const CHAR* file_name )
{
#ifdef CO_DEBUG_MBALOG_ON
	MBA_Log_Output( E_MBALOG_OUT_FILE_BINARY, file_name );
#endif
}

/**
 * Clear Memory Log
 */
VOID fj_mbalog_clear( VOID )
{
#ifdef CO_DEBUG_MBALOG_ON
	MBA_Log_Clear();
#endif
}

/**
 * Output Heap Log
 */
VOID fj_heap_log_printf( UCHAR func_id, FJ_MBALOG_TYPE type, const CHAR* fmt, va_list arg )
{
#ifdef CO_DEBUG_MBALOG_ON
	MBA_Log_Printf( D_MBA_KIND_HEAP, func_id, (UCHAR)type, fmt, arg );
#endif
}

/************************************************************************
 Program (MBA Lib->BaseFW)
*************************************************************************/
/**
 * Create Log File (called by MBA.a)
 */
ULONG MBA_IF_FSIF_Create( const char* fname )
{
	return FJ_Fs_Open( (CHAR*)fname, FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC );
}

/**
 * Close Log File (called by MBA.a)
 */
VOID MBA_IF_FSIF_Close( unsigned long fNo )
{
	FJ_Fs_Close( fNo );
}


/**
 * Write to Log File (called by MBA.a)
 */
INT32 MBA_IF_FSIF_Write( unsigned long fNo, const void* addr, unsigned long count, unsigned long* length )
{
	switch( FJ_Fs_Write( fNo, (UINT32*)addr, count, length ) ) {
		case FJ_ERR_OK:
			return 0;
		default:
			return -1;
	}
}

/**
 * Remove File (called by MBA.a)
 */
INT32 MBA_IF_FSIF_Remove( const char* fname )
{
	switch( FJ_Fs_Remove( (CHAR*)fname ) ) {
		case FJ_ERR_OK:
			return 0;
		default:
			return -1;
	}
}

UINT32 MBA_IF_User_Get_CpuID( VOID )
{
	return (UINT32)Dd_ARM_Get_CPU_ID();
}

BOOL MBA_IF_User_Sns_Ctx( VOID )
{
	return OS_User_Sns_Ctx();
}

OS_USER_ER MBA_IF_User_Get_Tid( OS_USER_ID* p_tskid )
{
	return (OS_USER_ER)OS_User_Get_Tid((OS_USER_ID*)p_tskid);
}

INT32 MBA_IF_User_Get_MbaTidFromTid( OS_USER_ID tid )
{
	return OS_User_Get_MbaTidFromTid( tid );
}

BOOL MBA_IF_User_Get_TidFromMbaTid( OS_USER_ID* const tid, const INT32 mba_tid )
{
	return OS_User_Get_TidFromMbaTid( tid, mba_tid );
}

VOID MBA_IF_User_Get_TaskNameFromMbaTid( const INT32 mba_tid, CHAR task_name[D_MBA_TASK_NAME_MAX] )
{
	if( (mba_tid == -1) || (mba_tid >= TID_NUM) ) {
		task_name[0] = '\0';
		return;
	}

	if( gTaskCreated[mba_tid] != 0 ) {
		if( gTaskName[mba_tid] != NULL ) {
			strncpy( task_name, gTaskName[mba_tid], D_MBA_TASK_NAME_MAX );
			task_name[D_MBA_TASK_NAME_MAX -1] = '\0';
			return;
		}
	}

	snprintf( task_name, D_MBA_TASK_NAME_MAX, "task%u", mba_tid );
	task_name[D_MBA_TASK_NAME_MAX -1] = '\0';
}

VOID MBA_IF_User_Get_TaskNameFromTid( const OS_USER_ID tid, CHAR task_name[D_MBA_TASK_NAME_MAX] )
{
	INT32		mba_tid;
	VOID*		task_main_addr;

	mba_tid = MBA_IF_User_Get_MbaTidFromTid( tid );
	if( mba_tid == -1 ) {
		if( OS_User_Td_Ref_Dsname( D_OS_USER_TN_TSK, tid, (OS_USER_UB*)task_name ) == D_OS_USER_E_OK ) {
			task_name[8] = '\0';
			return;
		}
		task_main_addr = OS_User_Get_TaskMainAddr( tid );
		snprintf( task_name, D_MBA_TASK_NAME_MAX, "func0x%lX", (ULONG)task_main_addr );
		task_name[D_MBA_TASK_NAME_MAX -1] = '\0';
		return;
	}

	MBA_IF_User_Get_TaskNameFromMbaTid( mba_tid, task_name );
}

VOID MBA_IF_User_EI( const UINT32 lock_key )
{
	BF_Arm_Unlock( &g_mba_if_critseq, lock_key );
}

UINT32 MBA_IF_User_DI( VOID )
{
    return BF_Arm_Lock( &g_mba_if_critseq );
}

ULONG MBA_IF_Get_Time_us( VOID )
{
	return BF_Get_Time_us();
}

#endif	/* CO_DEBUG_MBALOG_ON or CO_BF_MBALOG_ON */
