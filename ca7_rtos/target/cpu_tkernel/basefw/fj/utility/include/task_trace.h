/**
 * @file		task_trace.h
 * @brief		Current task information.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef __BF_TASK_TRACE_H__
#define __BF_TASK_TRACE_H__

#include "os_user_custom.h"
#include "ddim_typedef.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Function																		*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


extern VOID BF_TaskTrace_Enable_TskTrace( VOID );
extern VOID BF_TaskTrace_Disable_TskTrace( VOID );
extern VOID BF_TaskTrace_Fill_Stack( VOID );
extern ULONG BF_TaskTrace_Get_ReservedStackBytes( const OS_USER_ID tskid );


#ifdef __cplusplus
}
#endif

#endif // __BF_TASK_TRACE_H__

