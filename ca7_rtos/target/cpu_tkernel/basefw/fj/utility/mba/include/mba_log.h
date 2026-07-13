/**
 * @file		mba_log.h
 * @brief		MBA Log Control Header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef _MBA_LOG_H_
#define _MBA_LOG_H_

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "mba_buf.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define CO_MBA_TIMER_USED
#define CO_MBA_TIME_UNIT_1US

//MBA Log Task ID Max Count
#define MBALOG_TASK_ID_MAX			256
#define MBALOG_TASK_ID_LINE_MAX		128
#define MBALOG_TASK_ID_TASK_MAX		(MBALOG_TASK_ID_MAX-MBALOG_TASK_ID_LINE_MAX)

//MBA Log Function ID Max Count
#define MBALOG_FUNC_ID_MAX		256

//MBA Log Line ID Name Max Length
#define MBALOG_TASK_ID_LENGTH	17

//MBA Log Function ID Name Max Length
#define MBALOG_FUNC_ID_LENGTH	13

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// MBA log 1 data structure
typedef struct{
	unsigned long			time;							// Measure time
	unsigned long			task_id;						// Task ID (Line name)
	unsigned char			func_id;						// Function ID
	unsigned char			type;							// Data type (0:point, 1:start, 2:end )
	const char				*fmt;							// printf 1st parameter
	unsigned int			arg[4];							// printf 2nd - 5th parameter
} T_MBA_LOG_DATA;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// MBA log data max count
extern unsigned int gMBA_Log_Max;

// MBA log data max count
extern unsigned int gMBA_Surface_Max;

// MBA write pointer
extern unsigned long* gMBA_Write_Pointer;

// MBA overlap flg
extern unsigned char* gMBA_Overlap_Flg;

// MBA log data array
extern T_MBA_LOG_DATA gMBA_Log_Data[][D_MBALOG_RECORD_MAX];

// Flag of log overwrite
extern int gMBA_Overwrite_Flag;

// MBA log output buffer size
extern unsigned int gMBA_Outbuf_Size_Main;
extern unsigned int gMBA_Outbuf_Size_Sub;

// MBA log output buffer
extern unsigned char gMBA_Outbuf[];

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern void MBA_Log_Start( void* const id_list );
extern void MBA_Log_End( void );
extern void MBA_Log_Printf( const unsigned int kind, const unsigned char func_id, const unsigned char type, const char* const fmt, va_list arg );
extern void MBA_Log_Printf_LineID( const unsigned int kind, const unsigned char line_id, const unsigned char func_id, const unsigned char type, const char* const fmt, va_list arg );
extern void MBA_Log_Output( unsigned char type, const char* const file_name );
extern void MBA_Log_Output_Select( unsigned char type, unsigned int surface_no, const char* file_name );
extern void MBA_Log_Output_All( unsigned char type, const char* const file_name );
extern void MBA_Log_Clear( void );
extern void MBA_Log_Remove( const char* const file_name );
extern void MBA_Log_Update_Surface_No( void );

#endif
