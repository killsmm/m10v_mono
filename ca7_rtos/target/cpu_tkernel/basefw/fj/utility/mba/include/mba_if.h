/**
 * @file		mba_if.h
 * @brief		MBA Log I/F
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef _MBA_IF_H_
#define _MBA_IF_H_

#include <string.h>
// --- REMOVE_MBA BEGIN ---
#include <stdarg.h>
#include "ddim_typedef.h"
#include "fj_host_debug.h"
#include "fs_if.h"
// --- REMOVE_MBA END ---

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// --- REMOVE_MBA BEGIN ---
/* Kind of MBA LOG */
#define D_MBA_KIND_HOST				(0x00000001)
#define D_MBA_KIND_BOOT				(0x00000002)
#define D_MBA_KIND_CAPTURE			(0x00000004)
#define D_MBA_KIND_PREVIEW			(0x00000008)
#define D_MBA_KIND_HEAP				(0x00000010)
#define D_MBA_KIND_MOVIE			(0x00000020)

#define D_MBA_KIND_DEBUG			(0x00010000)

#define D_MBA_KIND_HOST_DEBUG		(D_MBA_KIND_DEBUG | D_MBA_KIND_HOST)
#define D_MBA_KIND_BOOT_DEBUG		(D_MBA_KIND_DEBUG | D_MBA_KIND_BOOT)
#define D_MBA_KIND_CAPTURE_DEBUG	(D_MBA_KIND_DEBUG | D_MBA_KIND_CAPTURE)
#define D_MBA_KIND_PREVIEW_DEBUG	(D_MBA_KIND_DEBUG | D_MBA_KIND_PREVIEW)
#define D_MBA_KIND_HEAP_DEBUG		(D_MBA_KIND_DEBUG | D_MBA_KIND_HEAP)

/* Default Kind */
#define D_MBA_KIND_DEFAULT			(D_MBA_KIND_BOOT | D_MBA_KIND_HOST | D_MBA_KIND_PREVIEW | D_MBA_KIND_MOVIE)
//#define D_MBA_KIND_DEFAULT		(D_MBA_KIND_BOOT)
//#define D_MBA_KIND_DEFAULT		(D_MBA_KIND_CAPTURE)
//#define D_MBA_KIND_DEFAULT		(D_MBA_KIND_PREVIEW)
//#define D_MBA_KIND_DEFAULT		(D_MBA_KIND_HEAP)
//#define D_MBA_KIND_DEFAULT		(D_MBA_KIND_HOST | D_MBA_KIND_BOOT)
//#define D_MBA_KIND_DEFAULT		(D_MBA_KIND_CAPTURE | D_MBA_KIND_HOST | D_MBA_KIND_PREVIEW)

#define D_MBA_TASK_NAME_MAX			(48)
// --- REMOVE_MBA END ---

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// --- REMOVE_MBA BEGIN ---
// Log output type
typedef enum {
	E_MBALOG_OUT_TERMINAL = 0,
	E_MBALOG_OUT_FILE,
	E_MBALOG_OUT_FILE_BINARY
} E_MBALOG_OUT_TYPE;
// --- REMOVE_MBA END ---

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// --- REMOVE_MBA BEGIN ---
// Kind of MBA log
extern UINT32 gMBA_Kind;

// MBA Log ID Name List Data
extern FJ_MBALOG_ID_LIST gMBALog_Id_List;
// --- REMOVE_MBA END ---

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// --- REMOVE_MBA BEGIN ---
//#define __file__ (strrchr(__FILE__,'\\' ) + 1 )
// --- REMOVE_MBA END ---

// --- REMOVE_MBA BEGIN ---
#if defined(CO_DEBUG_MBALOG_ON) || defined(CO_BF_MBALOG_ON)
	#define M_MBA_START()									(BF_MBA_IF_Start())
#else
// --- REMOVE_MBA END ---
	#define M_MBA_START()
// --- REMOVE_MBA BEGIN ---
#endif
// --- REMOVE_MBA END ---

// --- REMOVE_MBA BEGIN ---
#ifdef CO_BF_MBALOG_ON
	#define M_MBA_END()										(BF_MBA_IF_End())
	#define M_MBA_PRINTF(arg)								(BF_MBA_IF_Printf_TaskID arg)
	#define M_MBA_OUTPUT(type, file_name)					(BF_MBA_IF_Output(type, file_name))
	#define M_MBA_OUTPUT_SEL(type, surface_no, file_name)	(BF_MBA_IF_Output_Select(type, surface_no, file_name))
	#define M_MBA_OUTPUT_ALL(type, file_name)				(BF_MBA_IF_Output_All(type, file_name))
	#define M_MBA_CLEAR()									(BF_MBA_IF_Clear())
	#define M_MBA_REMOVE(file_name)							(BF_MBA_IF_Remove(file_name))
	#define M_MBA_UPDATE_SURFACE_NO()						(BF_MBA_IF_Update_Surface_No())
#else
// --- REMOVE_MBA END ---
	#define M_MBA_END()
	#define M_MBA_PRINTF(arg)
	#define M_MBA_OUTPUT(type, file_name)
	#define M_MBA_OUTPUT_SEL(type, sel, file_name)
	#define M_MBA_OUTPUT_ALL(type, file_name)
	#define M_MBA_CLEAR()
	#define M_MBA_REMOVE(file_name)
	#define M_MBA_UPDATE_SURFACE_NO()
// --- REMOVE_MBA BEGIN ---
#endif
// --- REMOVE_MBA END ---

/*----------------------------------------------------------------------*/
/* Function Prototype Definition 										*/
/*----------------------------------------------------------------------*/
// --- REMOVE_MBA BEGIN ---
extern VOID BF_MBA_IF_Start( VOID );
extern VOID BF_MBA_IF_End( VOID );
extern VOID BF_MBA_IF_Printf_TaskID( const UINT32 kind, const FJ_MBALOG_FUNC_ID func_id, const FJ_MBALOG_TYPE type, const char* fmt, ... ) __attribute__ ((format(printf, 4, 5)));
extern VOID BF_MBA_IF_Printf_LineID( const UINT32 kind, const FJ_MBALOG_LINE_ID, const FJ_MBALOG_FUNC_ID func_id, const FJ_MBALOG_TYPE type, const char* fmt, ... ) __attribute__ ((format(printf, 5, 6)));
extern VOID BF_MBA_IF_Output( const E_MBALOG_OUT_TYPE type, const CHAR* const file_name );
extern VOID BF_MBA_IF_Output_Select( const E_MBALOG_OUT_TYPE type, const UINT32 surface_no, const CHAR* const file_name );
extern VOID BF_MBA_IF_Output_All( const E_MBALOG_OUT_TYPE type, const CHAR* const file_name );
extern VOID BF_MBA_IF_Clear( VOID );
extern VOID BF_MBA_IF_Remove( const CHAR* const file_name );
extern VOID BF_MBA_IF_Update_Surface_No( VOID );

extern VOID fj_mbalog_printf( UCHAR func_id, FJ_MBALOG_TYPE type, const char* fmt, va_list arg );
extern VOID fj_mbalog_output( const CHAR* file_name );
extern VOID fj_mbalog_output_binary( const CHAR* file_name );
extern VOID fj_mbalog_clear( VOID );
extern VOID fj_heap_log_printf( UCHAR func_id, FJ_MBALOG_TYPE type, const CHAR* fmt, va_list arg );

extern ULONG MBA_IF_FSIF_Create( const char* fname );
extern VOID MBA_IF_FSIF_Close( unsigned long fNo );
extern INT32 MBA_IF_FSIF_Write( unsigned long fNo, const void* addr, unsigned long count, unsigned long* length );
extern INT32 MBA_IF_FSIF_Remove( const char* fname );
extern UINT32 MBA_IF_User_Get_CpuID( VOID );
extern BOOL MBA_IF_User_Sns_Ctx( VOID );
extern OS_USER_ER MBA_IF_User_Get_Tid( OS_USER_ID* p_tskid );
extern INT32 MBA_IF_User_Get_MbaTidFromTid( const OS_USER_ID tid );
extern BOOL MBA_IF_User_Get_TidFromMbaTid( OS_USER_ID* const tid, const INT32 mba_tid );
extern VOID MBA_IF_User_Get_TaskNameFromMbaTid( const INT32 mba_tid, CHAR task_name[D_MBA_TASK_NAME_MAX] );
extern VOID MBA_IF_User_Get_TaskNameFromTid( const OS_USER_ID tid, CHAR task_name[D_MBA_TASK_NAME_MAX] );
extern VOID MBA_IF_User_EI( const UINT32 lock_key );
extern UINT32 MBA_IF_User_DI( VOID );
extern ULONG MBA_IF_Get_Time_us( VOID );

// --- REMOVE_MBA END ---

#endif
