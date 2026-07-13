/**
 * @file		fj_debug_mba.c
 * @brief		SocioNext Custom Definition
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_debug_mba.h"
#include "mba_if.h"

/******************************************************************************
 Program
******************************************************************************/
/**
 * Print Memory Log
 */
VOID FJ_MBALog_Printf( UCHAR func_id, FJ_MBALOG_TYPE type, const char* fmt, ... )
{
#ifdef CO_DEBUG_MBALOG_ON
	va_list	arg;

	va_start( arg, fmt );		// pgr0039
	fj_mbalog_printf( func_id, type, fmt, arg );
	va_end( arg );
#endif
}

/**
 * Output Memory Log as text format
 */
VOID FJ_MBALog_Output( const CHAR* file_name )
{
#ifdef CO_DEBUG_MBALOG_ON
	fj_mbalog_output( file_name );
#endif
}

/**
 * Clear Memory Log
 */
VOID FJ_MBALog_Clear( VOID )
{
#ifdef CO_DEBUG_MBALOG_ON
	fj_mbalog_clear( );
#endif
}

/**
 * Print Heap Log
 */
VOID FJ_Heap_Log_Printf( UCHAR func_id, FJ_MBALOG_TYPE type, const CHAR* fmt, ... )
{
#if defined(CO_DEBUG_MBALOG_ON) && defined(CO_DEBUG_MBALOG_HEAP_ON)
	va_list	arg;

	va_start( arg, fmt );
	fj_heap_log_printf( func_id, type, fmt, arg );
	va_end( arg );
#endif
}
