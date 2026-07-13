/**
 * @file		emergency_log.h
 * @brief		Emergency Log Save
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _EMERGENCY_LOG_H
#define _EMERGENCY_LOG_H

#include "ddim_typedef.h"

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
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/
extern FJ_ERR_CODE fj_emergencylog_set_log_start_sector(ULONG sector);
extern FJ_ERR_CODE fj_emergencylog_store_memory(ULONG startAddr, INT32 headerInfo);
extern FJ_ERR_CODE fj_emergencylog_store_sdram(ULONG startAddr, INT32 headerInfo);
extern FJ_ERR_CODE fj_emergencylog_save(UCHAR* filename);

#endif	// _EMERGENCY_LOG_H
