/**
 * @file		fj_debug.c
 * @brief		SocioNext Custom Definition
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include <stdarg.h>
#include "ddim_typedef.h"
#include "fj_debug.h"
#include "debug.h"
#include "mode.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/////////////////////////////////////////////////////
// romarea_boot
/////////////////////////////////////////////////////
VOID FJ_Debug_Print_Dbg(const char* format, ...)
{
#ifdef FJ_DEBUG_PRINT_DBG_ON
	va_list ap;

	va_start(ap, format);	/* pgr0039 */
	fj_debug_print_dbg(format, ap);
	va_end(ap);
#endif	/* FJ_DEBUG_PRINT_DBG_ON */
}

VOID FJ_Debug_Print_Info(const char* format, ...)
{
#ifdef FJ_DEBUG_PRINT_INFO_ON
	va_list ap;

	va_start(ap, format);	/* pgr0039 */
	fj_debug_print_info(format, ap);
	va_end(ap);
#endif	/* FJ_DEBUG_PRINT_INFO_ON */
}

VOID FJ_Debug_Print_Wrn(const char* format, ...)
{
#ifdef FJ_DEBUG_PRINT_WRN_ON
	va_list ap;

	va_start(ap, format);	/* pgr0039 */
	fj_debug_print_wrn(format, ap);
	va_end(ap);
#endif	/* FJ_DEBUG_PRINT_WRN_ON */
}

VOID FJ_Debug_Print_Err(const char* format, ...)
{
#ifdef FJ_DEBUG_PRINT_ERR_ON
	va_list ap;

	va_start(ap, format);	/* pgr0039 */
	fj_debug_print_err(format, ap);
	va_end(ap);
#endif	/* FJ_DEBUG_PRINT_ERR_ON */
}

