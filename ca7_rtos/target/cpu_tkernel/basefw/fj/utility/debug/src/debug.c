/**
 * @file		debug.c
 * @brief		Debug (printf wrapper)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "fj_filesystem.h"
#include "fs_if.h"
#include "assertion.h"
#include "debug.h"
#include "os_wrapper.h"
#include "os_user_custom.h"
#include "dd_arm.h"
#include "dd_uart.h"
#include "measure_time.h"
#include "uart.h"
#include "sdram_map_common.h"

#ifdef CO_ASSERT_LOG_ON
extern volatile int gAssertion_tbl_idx;
extern T_ASSERTION_DATA gAssertion_tbl[ASSERTION_LOG_TBL_SIZE];
#endif

#if 0
extern volatile UCHAR gBF_UART_Debug_Send_Buf[D_UART_DEBUG_SEND_BUFF_SIZE];
#else
extern volatile UCHAR gBF_UART_Debug_Send_Buf[D_UART_DEBUG_SEND_BUFF_SIZE];
#endif

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
static volatile E_BF_DEBUG_PRINT_LEVEL gBF_Debug_Print_Levlel = E_BF_DEBUG_PRINT_LEVEL_DBG;
static volatile UCHAR gBF_Debug_Print_Time = 0;
static volatile UCHAR gBF_Debug_Print_Head = 1;
static volatile E_BF_DEBUG_PRINT_LEVEL gFJ_Debug_Print_Levlel = E_BF_DEBUG_PRINT_LEVEL_DBG;

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
int _EXFUN(__real_vprintf, (const char *, __VALIST) _ATTRIBUTE ((__format__ (__printf__, 1, 0))));
int _EXFUN(__real_printf, (const char *__restrict, ...) _ATTRIBUTE ((__format__ (__printf__, 1, 2))));

/**
 * output to terminal
 * @param i:const char *head
 * @param i:const char* format
 * @param i:va_list ap
 */
VOID BF_Debug_TermOut(const CHAR* head, const CHAR* format, va_list ap)
{
	OS_USER_ID	tskid;
	BOOL		is_intr;
	ULONG		cpuid;

	tskid = BF_Os_Wrapper_tid_self();

	is_intr = OS_User_Sns_Ctx();
	
#ifdef CO_DISABLE_PRINT_IN_INTERRUPT
	if( is_intr ) {
		return;
	}
#endif	// CO_DISABLE_PRINT_IN_INTERRUPT

	cpuid = Dd_ARM_Get_CPU_ID();

	myprintf("[0m");

	if( gBF_Debug_Print_Time ){
		myprintf("%010lu:", BF_Get_Time_ms());
	}
#ifndef CO_BF_DEBUG_ESCAPE_CHARACTER_OFF
	if( gBF_Debug_Print_Head ){
		if (*head == 'W'){
			// FONT=BLACK, BGCOLOR=YELLOW
			myprintf("[43;30m%s[0m:", head);
		}
		else if (*head == 'E'){
			// FONT=BLACK, BGCOLOR=RED
			myprintf("[41;30m%s[0m:", head);
		}
		else {
			myprintf("%s:", head);
		}

		if( is_intr ) {
			myprintf("%lu:INTERRUPT: ", cpuid);
		}
		else {
			myprintf("%lu:T%08X: ", cpuid, tskid);
		}
	}
#else
	if (*head == 'W'){
		// FONT=BLACK, BGCOLOR=YELLOW
		myprintf(" %s :", head);
	}
	else if (*head == 'E'){
		// FONT=BLACK, BGCOLOR=RED
		myprintf(" %s :", head);
	}
	else {
		myprintf(" %s :", head);
	}

	if( is_intr ) {
		myprintf("%lu:INTERRUPT:%s: ", cpuid, tskid, gSysmode_Name[Get_Act_Sysmode()]);
	}
	else {
		myprintf("%lu:T%08X:%s: ", cpuid, tskid, gSysmode_Name[Get_Act_Sysmode()]);
	}
#endif

	__wrap_vprintf(format, ap);
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/**
 * set to print level
 *    0:All OFF
 *    1:Err ON
 *    2:Warn ON
 *    3:Info ON
 *    4:Debug ON (All ON)
 */
VOID BF_Debug_Set_Print_Level(E_BF_DEBUG_PRINT_LEVEL level)
{
	gBF_Debug_Print_Levlel = level;
}

VOID BF_Debug_Set_Print_Level_Host(E_BF_DEBUG_PRINT_LEVEL level)
{
	gFJ_Debug_Print_Levlel = level;
}

/**
 * set to print head enable
 */
VOID BF_Debug_Set_Print_Head_Enable(UCHAR enable)
{
	gBF_Debug_Print_Head = enable;
}

/**
 * set to print time enable
 */
VOID BF_Debug_Set_Print_Time_Enable(UCHAR enable)
{
	gBF_Debug_Print_Time = enable;
}

/**
 * Save the memory to file
 * @param i:start_addr	start memory address
 * @param i:size		memory size
 * @param i:filename	file name
 * @note
 */
VOID BF_Debug_Save_Mem_To_File(CHAR* start_addr, ULONG size, const CHAR* filename)
{
	INT32 fno;
	ULONG wrote_length;
	INT32 ret;

	if ((fno = BF_Fs_If_Create((const FW_CHAR *)filename, D_BF_FS_IF_ATTR_NORMAL)) == 0) {
		BF_Debug_Print_Error(("BF_Debug_Save_Mem_To_File: FSIF_Create Error=%d\n", fno));
		return;
	}

	ret = BF_Fs_If_Write(fno, start_addr, size, &wrote_length);
	if ( ret != FJ_ERR_OK ) {
		BF_Debug_Print_Error(("BF_Debug_Save_Mem_To_File: FSIF_Write Error=%d\n", fno));
		BF_Fs_If_Close(fno);
		return;
	}
	BF_Fs_If_Close(fno);
}

/**
 * print message to terminal as debug
 * @param i:const char *format
 * @param i:...
 * @note use likes printf()
 */
VOID BF_Debug_Print_Dbg(const char* format, ...)
{
#ifdef FJ_DEBUG_PRINT_DBG_ON
	va_list ap;

	if (gBF_Debug_Print_Levlel < E_BF_DEBUG_PRINT_LEVEL_DBG){
		return;
	}

	va_start(ap, format);	/* pgr0039 */
	BF_Debug_TermOut("D", format, ap);
	va_end(ap);
#endif	/* FJ_DEBUG_PRINT_DBG_ON */
}

/**
 * print message to terminal as information
 * @param i:const char *format
 * @param i:...
 * @note use likes printf()
 */
VOID BF_Debug_Print_Info(const char* format, ...)
{
#ifdef FJ_DEBUG_PRINT_INFO_ON
	va_list ap;

	if (gBF_Debug_Print_Levlel < E_BF_DEBUG_PRINT_LEVEL_INFO){
		return;
	}

	va_start(ap, format);	/* pgr0039 */
	BF_Debug_TermOut("I", format, ap);
	va_end(ap);
#endif	/* FJ_DEBUG_PRINT_INFO_ON */
}

/**
 * print message to terminal as warning
 * @param i:const char *format
 * @param i:...
 * @note use likes printf()
 */
VOID BF_Debug_Print_Wrn(const char* format, ...)
{
#ifdef FJ_DEBUG_PRINT_WRN_ON
	va_list ap;

	if (gBF_Debug_Print_Levlel < E_BF_DEBUG_PRINT_LEVEL_WRN){
		return;
	}

	va_start(ap, format);	/* pgr0039 */
	BF_Debug_TermOut("W", format, ap);
	va_end(ap);
#endif	/* FJ_DEBUG_PRINT_WRN_ON */
}

/**
 * print message to terminal as error
 * @param i:const char *format
 * @param i:...
 * @note use likes printf()
 */
VOID BF_Debug_Print_Err(const char* format, ...)
{
#ifdef FJ_DEBUG_PRINT_ERR_ON
	va_list ap;
	
	if (gBF_Debug_Print_Levlel < E_BF_DEBUG_PRINT_LEVEL_ERR){
		return;
	}

	va_start(ap, format);	/* pgr0039 */
	BF_Debug_TermOut("E", format, ap);
	va_end(ap);
#endif	/* FJ_DEBUG_PRINT_ERR_ON */
}

/**
 * print message to terminal as audio information
 * @param i:const char *format
 * @param i:...
 * @note use likes printf()
 */
VOID BF_Debug_Print_Aud(const char* format, ...)
{
#ifdef FJ_DEBUG_PRINT_AUDIO_ON
	va_list ap;

	va_start(ap, format);	/* pgr0039 */
	BF_Debug_TermOut("A", format, ap);
	va_end(ap);
#endif	/* FJ_DEBUG_PRINT_AUDIO_ON */
}

/**
 * print message to terminal as debug
 * @param i:const char *format
 * @param i:...
 * @note use likes printf()
 */
VOID fj_debug_print_dbg(const CHAR* format, va_list ap)
{
#ifdef FJ_DEBUG_PRINT_DBG_ON
	if (gFJ_Debug_Print_Levlel < E_BF_DEBUG_PRINT_LEVEL_DBG){
		return;
	}

	BF_Debug_TermOut("D", format, ap);
#endif	/* FJ_DEBUG_PRINT_DBG_ON */
}

/**
 * print message to terminal as information
 * @param i:const char *format
 * @param i:...
 * @note use likes printf()
 */
VOID fj_debug_print_info(const CHAR* format, va_list ap)
{
#ifdef FJ_DEBUG_PRINT_INFO_ON
	if (gFJ_Debug_Print_Levlel < E_BF_DEBUG_PRINT_LEVEL_INFO){
		return;
	}

	BF_Debug_TermOut("I", format, ap);
#endif	/* FJ_DEBUG_PRINT_INFO_ON */
}

/**
 * print message to terminal as warning
 * @param i:const char *format
 * @param i:...
 * @note use likes printf()
 */
VOID fj_debug_print_wrn(const CHAR* format, va_list ap)
{
#ifdef FJ_DEBUG_PRINT_WRN_ON
	if (gFJ_Debug_Print_Levlel < E_BF_DEBUG_PRINT_LEVEL_WRN){
		return;
	}

	BF_Debug_TermOut("W", format, ap);
#endif	/* FJ_DEBUG_PRINT_WRN_ON */
}

/**
 * print message to terminal as error
 * @param i:const char *format
 * @param i:...
 * @note use likes printf()
 */
VOID fj_debug_print_err(const CHAR* format, va_list ap)
{
#ifdef FJ_DEBUG_PRINT_ERR_ON
	if (gFJ_Debug_Print_Levlel < E_BF_DEBUG_PRINT_LEVEL_ERR){
		return;
	}

	BF_Debug_TermOut("E", format, ap);
#endif	/* FJ_DEBUG_PRINT_ERR_ON */
}

/**
 * print message to terminal as audio information
 * @param i:const char *format
 * @param i:...
 * @note use likes printf()
 */
VOID fj_debug_print_audio(const char* format, ...)
{
#ifdef FJ_DEBUG_PRINT_AUDIO_ON
	va_list ap;

	va_start(ap, format);	/* pgr0039 */
	BF_Debug_TermOut("A", format, ap);
	va_end(ap);
#endif	/* FJ_DEBUG_PRINT_AUDIO_ON */
}
