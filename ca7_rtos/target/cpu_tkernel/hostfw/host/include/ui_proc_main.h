/**
 * @file		ui_proc_main.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _UI_PROC_MAIN_H_
#define _UI_PROC_MAIN_H_

#ifdef CO_UI_PROC
#include "user.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define UI_PROC_AUTO_VIEW_ON

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

typedef enum{
	E_UI_PROC_OP_MODE_INIT = 0,
	E_UI_PROC_OP_MODE_USB,
	E_UI_PROC_OP_MODE_STILL,
	E_UI_PROC_OP_MODE_MAX
} E_UI_PROC_OP_MODE;

typedef enum{
	E_UI_PROC_OP_STATE_INIT = 0,
	E_UI_PROC_OP_STATE_RUN,
	E_UI_PROC_OP_STATE_NG
} E_UI_PROC_OP_STATE;

typedef enum{
	E_UI_PROC_OP_MANAGE_INIT = 0,
	E_UI_PROC_OP_MANAGE_TRIG,
	E_UI_PROC_OP_MANAGE_STOP,
	E_UI_PROC_OP_MANAGE_MAX
} E_UI_PROC_OP_MANAGE;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	ULONG msgid;
	ULONG ulPara1;
	VOID* vpPara2;
}T_UI_PROC_MSG;

typedef struct {
	ULONG	(*func)(ULONG) ;
} T_UI_PROC_FUNC;

typedef struct {
	T_UI_PROC_FUNC			exec;
} T_UI_PROC_CUR_TBL;

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

extern VOID UI_Proc_Main_Init(VOID);
extern VOID UI_Proc_Main_Start(VOID);
extern VOID UI_Proc_Main_EventTrig(T_UI_PROC_MSG *msg);

#ifdef __cplusplus
}
#endif

#endif
#endif	// _UI_PROC_MAIN_H_

