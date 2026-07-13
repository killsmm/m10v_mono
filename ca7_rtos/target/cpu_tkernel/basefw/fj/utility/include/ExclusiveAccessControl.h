/**
 * @file		ExclusiveAccessControl.h
 * @brief		Exclusive access control
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _EXCLUSIVE_ACCESS_CONTROL_H
#define _EXCLUSIVE_ACCESS_CONTROL_H

#include "ddim_typedef.h"
#include "os_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
typedef OS_USER_ER				EAC_ER;
typedef OS_USER_ID				EAC_ID;

#define D_EAC_CPU_NUM			(2)
#define D_EAC_TASK_TBL_MAX		(128)
#define D_EAC_SEM_TBL_MAX		(32)
#define D_EAC_CMD_TBL_MAX		(32)
#define D_EAC_OK				(0)

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
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef CO_EXCLUSIVE_ACCESS_CONTROL
VOID eac_task(INT32 stacd);
EAC_ER eac_sig_sem_main(EAC_ID semid);
EAC_ER eac_wai_sem_main(EAC_ID semid);
VOID eac_ipcu_int_req_main(ULONG* data);

EAC_ER eac_sig_sem_sub(EAC_ID semid);
EAC_ER eac_wai_sem_sub(EAC_ID semid);
VOID eac_ipu_int_req_sub(ULONG* data);
#endif	// CO_EXCLUSIVE_ACCESS_CONTROL

#endif	// _EXCLUSIVE_ACCESS_CONTROL_H
