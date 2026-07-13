/**
 * @file		ui_proc_usbmode.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _UI_PROC_USBMODE_H_
#define _UI_PROC_USBMODE_H_

#ifdef CO_UI_PROC
#include "user.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define UI_PROC_USBMODE_CONNECT_OFF		(0)
#define UI_PROC_USBMODE_CONNECT_ON		(1)

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

extern ULONG UI_Proc_UsbMode_Init(ULONG param);
extern ULONG UI_Proc_UsbMode_Start(ULONG param);
extern ULONG UI_Proc_UsbMode_Stop(ULONG param);
extern ULONG UI_Proc_UsbMode_Exit_StillMsg(ULONG param);
extern ULONG UI_Proc_UsbMode_EventTrig(ULONG param);

#ifdef __cplusplus
}
#endif

#endif
#endif	// _UI_PROC_USBMODE_H_

