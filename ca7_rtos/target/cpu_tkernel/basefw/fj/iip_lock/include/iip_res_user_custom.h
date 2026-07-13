/**
 * @file		iip_res_user_custom.h
 * @brief		Header file of IIP resource management library wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _IIP_RES_USER_CUSTOM_H_
#define _IIP_RES_USER_CUSTOM_H_

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
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef VOID		(*IIP_RES_CUSTOM_FP)();		/**< Wrapper type of OS_USER_FP */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

extern INT32 IIP_Res_Custom_Open( INT32 tmout );
extern INT32 IIP_Res_Custom_Close( VOID );
extern INT32 IIP_Res_Custom_Dly_Tsk( UINT32 msec );
extern INT32 IIP_Res_Custom_Get_Tim( ULLONG* sys_time );
extern INT32 IIP_Res_Custom_Twai_Flg( ULONG waiptn, UINT32 wfmode, ULONG* p_flgptn, INT32 tmout );
extern INT32 IIP_Res_Custom_Set_Flg( ULONG setptn );
extern INT32 IIP_Res_Custom_Clr_Flg( ULONG clrptn );

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _IIP_RES_USER_CUSTOM_H_

