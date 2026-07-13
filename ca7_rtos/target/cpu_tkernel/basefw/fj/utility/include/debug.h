/**
 * @file		debug.h
 * @brief		Debug (printf wrapper)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>
#include <stdarg.h>
#include "ddim_typedef.h"
#include "fj_custom.h"
#include "fj_std.h"
#include "fj_debug.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define CO_BF_DEBUG_ESCAPE_CHARACTER_OFF		/* Escape Code Off */
#define FJ_DBG_PRMERR			(0x0010)		/* Parameter check Error */
#define FJ_DBG_CRTERR			(0x0020)		/* Create file Error */
#define FJ_DBG_WRTERR			(0x0040)		/* Write file Error */

#define FJ_DEBUG_PRINT_AUDIO_ON					/* For Audio Debug Log */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Print Level
typedef enum {
	E_BF_DEBUG_PRINT_LEVEL_OFF = 0,		// All OFF
	E_BF_DEBUG_PRINT_LEVEL_ERR,         // Err ON
	E_BF_DEBUG_PRINT_LEVEL_WRN,         // Warn ON
	E_BF_DEBUG_PRINT_LEVEL_INFO,        // Info ON
	E_BF_DEBUG_PRINT_LEVEL_DBG          // Debug ON (All ON)
} E_BF_DEBUG_PRINT_LEVEL;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#ifndef CO_BF_DEBUG_ESCAPE_CHARACTER_OFF

// character color set
#define BF_Debug_Change_Term_Char_Color_To(color) (printf("[%dm", color))

// character highlight set
#define BF_Debug_Change_Term_Char_Color_To_HIGHLIGHT() (printf("[1m"))

// character color reverse set
#define BF_Debug_Change_Term_Char_Color_R_To(color) (printf("[7;%dm", color))

// initialize character
#define BF_Debug_Change_Term_Char_Init() (printf("[0m"))

// character underline set
#define BF_Debug_Change_Term_Char_Underline() (printf("[4m"))

// terminal clear (cursor move to top, font color to default)
#define BF_Debug_Change_Term_Clear() (printf("[0m[2J[0;0H"))

#define BF_Debug_Move_Line_To(line) (printf("[%d;0H", line))

#else

// character color set
#define BF_Debug_Change_Term_Char_Color_To(color) ( color)

// character highlight set
#define BF_Debug_Change_Term_Char_Color_To_HIGHLIGHT() 

// character color reverse set
#define BF_Debug_Change_Term_Char_Color_R_To(color) ( color)

// initialize character
#define BF_Debug_Change_Term_Char_Init() 

// character underline set
#define BF_Debug_Change_Term_Char_Underline() 

// terminal clear (cursor move to top, font color to default)
#define BF_Debug_Change_Term_Clear() 

#define BF_Debug_Move_Line_To(line) (line)

#endif	// CO_BF_DEBUG_ESCAPE_CHARACTER_OFF

// character color set
#define BF_Debug_Change_Term_Char_Color_To_Black() (BF_Debug_Change_Term_Char_Color_To(30))
#define BF_Debug_Change_Term_Char_Color_To_Red() (BF_Debug_Change_Term_Char_Color_To(31))
#define BF_Debug_Change_Term_Char_Color_To_Green() (BF_Debug_Change_Term_Char_Color_To(32))
#define BF_Debug_Change_Term_Char_Color_To_Yellow() (BF_Debug_Change_Term_Char_Color_To(33))
#define BF_Debug_Change_Term_Char_Color_To_Blue() (BF_Debug_Change_Term_Char_Color_To(34))
#define BF_Debug_Change_Term_Char_Color_To_Purple() (BF_Debug_Change_Term_Char_Color_To(35))
#define BF_Debug_Change_Term_Char_Color_To_Skyblue() (BF_Debug_Change_Term_Char_Color_To(36))
#define BF_Debug_Change_Term_Char_Color_To_White() (BF_Debug_Change_Term_Char_Color_To(37))

// character color reversal set
#define BF_Debug_Change_Term_Char_Color_R_To_Black() (BF_Debug_Change_Term_Char_Color_R_To(30))
#define BF_Debug_Change_Term_Char_Color_R_To_Red() (BF_Debug_Change_Term_Char_Color_R_To(31))
#define BF_Debug_Change_Term_Char_Color_R_To_Green() (BF_Debug_Change_Term_Char_Color_R_To(32))
#define BF_Debug_Change_Term_Char_Color_R_To_Yellow() (BF_Debug_Change_Term_Char_Color_R_To(33))
#define BF_Debug_Change_Term_Char_Color_R_To_Blue() (BF_Debug_Change_Term_Char_Color_R_To(34))
#define BF_Debug_Change_Term_Char_Color_R_To_Purple() (BF_Debug_Change_Term_Char_Color_R_To(35))
#define BF_Debug_Change_Term_Char_Color_R_To_Skyblue() (BF_Debug_Change_Term_Char_Color_R_To(36))
#define BF_Debug_Change_Term_Char_Color_R_To_White() (BF_Debug_Change_Term_Char_Color_R_To(37))

// character back color set
#define BF_Debug_Change_Term_Char_BK_Color_To_Black() (BF_Debug_Change_Term_Char_Color_To(40))
#define BF_Debug_Change_Term_Char_BK_Color_To_Red() (BF_Debug_Change_Term_Char_Color_To(41))
#define BF_Debug_Change_Term_Char_BK_Color_To_Green() (BF_Debug_Change_Term_Char_Color_To(42))
#define BF_Debug_Change_Term_Char_BK_Color_To_Yellow() (BF_Debug_Change_Term_Char_Color_To(43))
#define BF_Debug_Change_Term_Char_BK_Color_To_Blue() (BF_Debug_Change_Term_Char_Color_To(44))
#define BF_Debug_Change_Term_Char_BK_Color_To_Purple() (BF_Debug_Change_Term_Char_Color_To(45))
#define BF_Debug_Change_Term_Char_BK_Color_To_Skyblue() (BF_Debug_Change_Term_Char_Color_To(46))
#define BF_Debug_Change_Term_Char_BK_Color_To_White() (BF_Debug_Change_Term_Char_Color_To(47))

#ifdef FJ_DEBUG_PRINT_DBG_ON
#define BF_Debug_Print_Debug(arg)			(BF_Debug_Print_Dbg arg)
#else
#define BF_Debug_Print_Debug(arg)
#endif	/* FJ_DEBUG_PRINT_DBG_ON */

#ifdef FJ_DEBUG_PRINT_INFO_ON
#define BF_Debug_Print_Information(arg)		(BF_Debug_Print_Info arg)
#else
#define BF_Debug_Print_Information(arg)
#endif	/* FJ_DEBUG_PRINT_INFO_ON */

#ifdef FJ_DEBUG_PRINT_WRN_ON
#define BF_Debug_Print_Warning(arg)			(BF_Debug_Print_Wrn arg)
#else
#define BF_Debug_Print_Warning(arg)
#endif	/* FJ_DEBUG_PRINT_WRN_ON */

#ifdef FJ_DEBUG_PRINT_ERR_ON
#define BF_Debug_Print_Error(arg)			(BF_Debug_Print_Err arg)
#else
#define BF_Debug_Print_Error(arg)
#endif	/* FJ_DEBUG_PRINT_ERR_ON */

#ifdef FJ_DEBUG_PRINT_AUDIO_ON
#define BF_Debug_Print_Audio(arg)		(BF_Debug_Print_Aud arg)
#else
#define BF_Debug_Print_Audio(arg)
#endif	/* FJ_DEBUG_PRINT_AUDIO_ON */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef D_COM_BUF_FOR_TERMINAL
void BF_Com_Buff_Ini(void);
unsigned BF_Com_Buff_Is(void);
void BF_Com_Buff_Put_Char(char send_data);
int BF_Com_Buff_Get_Char(char* receive_data);
#endif	// D_COM_BUF_FOR_TERMINAL

void myvprintf(const char *fmt, va_list ap);
void myprintf(const char *fmt, ...) __attribute__ ((format(printf, 1, 2)));
void myvsprintf(const char *fmt, va_list ap);
void mysprintf(char *str, const char *fmt, ...) __attribute__ ((format(printf, 2, 3)));
int myvnsprintf(int n, const char *fmt, va_list ap);
int mysnprintf(char *str, int n, const char *fmt, ...) __attribute__ ((format(printf, 3, 4)));
int __wrap_vprintf(const char* format, __VALIST ap);
int __wrap_printf(const char* format, ...) __attribute__ ((format(printf, 1, 2)));

/**
 * set to print level
 *    0:All OFF
 *    1:Err ON
 *    2:Warn ON
 *    3:Info ON
 *    4:Debug ON (All ON)
 */
VOID BF_Debug_Set_Print_Level(E_BF_DEBUG_PRINT_LEVEL level);
VOID BF_Debug_Set_Print_Level_Host(E_BF_DEBUG_PRINT_LEVEL level);	/* for FJ Host */

/**
 * set to print head enable
 */
VOID BF_Debug_Set_Print_Head_Enable(UCHAR enable);

/**
 * set to print time enable
 */
VOID BF_Debug_Set_Print_Time_Enable(UCHAR enable);

/**
 * set debug output enable
 */
VOID BF_Debug_Set_Ooutput_Enable(UCHAR enable);

/**
 * Save the memory to file
 * @param i:start_addr	start memory address
 * @param i:size		memory size
 * @param i:filename	file name
 * @note
 */
VOID BF_Debug_Save_Mem_To_File(char* start_addr, ULONG size, const char* filename);


VOID BF_Debug_TermOut(const CHAR* head, const CHAR* format, va_list ap);
VOID BF_Debug_Print_Dbg(const char* format, ...) __attribute__ ((format(printf, 1, 2)));
VOID BF_Debug_Print_Info(const char* format, ...) __attribute__ ((format(printf, 1, 2)));
VOID BF_Debug_Print_Wrn(const char* format, ...) __attribute__ ((format(printf, 1, 2)));
VOID BF_Debug_Print_Err(const char* format, ...) __attribute__ ((format(printf, 1, 2)));
VOID BF_Debug_Print_Aud(const char* format, ...) __attribute__ ((format(printf, 1, 2)));

/**
 * print message to terminal as debug
 */
VOID fj_debug_print_dbg(const char* format, va_list ap);

/**
 * print message to terminal as information
 */
VOID fj_debug_print_info(const char* format, va_list ap);

/**
 * print message to terminal as warning
 */
VOID fj_debug_print_wrn(const char* format, va_list ap);

/**
 */
VOID fj_debug_print_err(const char* format, va_list ap);

/**
 * print message to terminal as audio information
 */
VOID fj_debug_print_audio(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif	// _DEBUG_H
