/**
 * @file		bf_startup.h
 * @brief		Base F/W startup process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _BF_STARTUP_H_
#define _BF_STARTUP_H_

#include "ddim_typedef.h"
#include "os_user_custom.h"
#include "fj_std.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define CO_USB_MASS_STORAGE_ON     //For USB Mass Storage(No filesystem. Media driver only)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

VOID fj_bootmessageprint(VOID);

VOID BF_Startup_Start_File_System(VOID);

VOID BF_Startup_Init_Display(VOID);

VOID BF_Startup_Init_BaseFW(VOID);
VOID BF_Startup_Boot_Sample(VOID);

VOID BF_Host_Init_Message(OS_USER_ID sem_id);

VOID fj_hostgetmessage( ULONG* pulMessage, ULONG* pulParam );
VOID fj_hostpostmessage( ULONG ulMessage, ULONG ulParam );
FJ_ERR_CODE fj_confighostmessage( USHORT wConfig,  ULONG wValue );

const CHAR* BF_Get_OK_Version_str(void);
unsigned short BF_Get_OK_MP_Version(VOID);
unsigned char BF_Get_OK_Sub_Version(VOID);
unsigned char BF_Get_OK_Debug_Version(VOID);
char* BF_Get_OK_FW_type(void);
char* BF_Get_OK_Board_type(void);
char* BF_Get_OK_Memory_size_type(void);

#ifdef __cplusplus
}
#endif

#endif	// _BF_STARTUP_H_
