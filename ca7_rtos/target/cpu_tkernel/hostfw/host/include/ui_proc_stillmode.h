/**
 * @file		ui_proc_stillmode.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _UI_PROC_STILLMODE_H_
#define _UI_PROC_STILLMODE_H_

#ifdef CO_UI_PROC
#include "user.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

extern ULONG UI_Proc_StillMode_Init(ULONG param);
extern ULONG UI_Proc_StillMode_Start(ULONG param);
extern ULONG UI_Proc_StillMode_Stop(ULONG param);
extern ULONG UI_Proc_StillMode_Exit_UsbMsg(ULONG param);
extern ULONG UI_Proc_StillMode_EventTrig(ULONG param);

#ifdef __cplusplus
}
#endif

#endif
#endif	// _UI_PROC_STILLMODE_H_

