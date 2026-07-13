/**
 * @file		mprofile.h
 * @brief		Mprofile Monitor
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _MPROFILE_H
#define _MPROFILE_H

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#ifdef CO_MPROFILE_ON
#define MPROFILE_INIT()			Mprofile_Init()
#define MPROFILE_RESET()		Mprofile_Reset()
#define MPROFILE_START()		Mprofile_Start()
#define MPROFILE_STOP()			Mprofile_Stop()
#define MPROFILE_CLOSE()		Mprofile_Close()
#else
#define MPROFILE_INIT()
#define MPROFILE_RESET()
#define MPROFILE_START()
#define MPROFILE_STOP()
#define MPROFILE_CLOSE()
#endif

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef CO_MPROFILE_ON
/**
 * Initialize Mprofile Monitor.
 */
VOID Mprofile_Init(VOID);

/**
 * Reset Mprofile Monitor Log.
 */
VOID Mprofile_Reset(VOID);

/**
 * Start Mprofile Monitor.
 */
VOID Mprofile_Start(VOID);

/**
 * Stop Mprofile Monitor.
 */
VOID Mprofile_Stop(VOID);

/**
 * Open Mprofile Monitor.
 */
VOID Mprofile_Open(VOID);

/**
 * Close Mprofile Monitor.
 */
VOID Mprofile_Close(VOID);

/**
 * Dump Mprofile Monitor.
 */
VOID Mprofile_Dump(const CHAR* file_name);

#endif	// CO_MPROFILE_ON

#ifdef __cplusplus
}
#endif

#endif	// _MPROFILE_H
