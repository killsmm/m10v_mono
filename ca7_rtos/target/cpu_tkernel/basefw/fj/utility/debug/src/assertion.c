/**
 * @file		assertion.c
 * @brief		Assertion Log
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "assertion.h"
#include "measure_time.h"
#include "dd_arm.h"

extern UINT32 Task_Control_Get_Ctxtsk(UINT32 cpuid, UINT32 *tskid, char **oname);

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
#ifdef CO_ASSERT_LOG_ON
volatile int				gAssertion_tbl_idx = 0;
T_ASSERTION_DATA			gAssertion_tbl[ASSERTION_LOG_TBL_SIZE];
#endif

/*---------------------------------------------------------------*/
/* Macro														 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Local Function												 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Global Function												 */
/*---------------------------------------------------------------*/
VOID Assertion_Log_Init(VOID)
{
#ifdef CO_ASSERT_LOG_ON
	memset( gAssertion_tbl, 0, sizeof(gAssertion_tbl) );
#endif
}

VOID Assertion_Log_Write(ULONG ercd, ULONG calling_func, VOID *called_func)
{
#ifdef CO_ASSERT_LOG_ON
	UINT32			cpuid = Dd_ARM_Get_CPU_ID();
	OS_USER_ID		taskid;
	char*			oname;
	UINT32			ercd_tsk;
	
	gAssertion_tbl[gAssertion_tbl_idx].time_us = BF_Get_Time_us();
	gAssertion_tbl[gAssertion_tbl_idx].ercd = ercd;
	gAssertion_tbl[gAssertion_tbl_idx].calling_func = (ULONG)calling_func;
	gAssertion_tbl[gAssertion_tbl_idx].called_func = (ULONG)called_func;
	gAssertion_tbl[gAssertion_tbl_idx].tskid = 0;
	memset(&gAssertion_tbl[gAssertion_tbl_idx].oname, 0, sizeof(gAssertion_tbl[gAssertion_tbl_idx].oname));
	ercd_tsk = Task_Control_Get_Ctxtsk(cpuid, (UINT32*)&taskid, &oname);
	if ( ercd_tsk == 0 ) {
		gAssertion_tbl[gAssertion_tbl_idx].tskid = taskid;
		for(int i=0; i<8; i++){
			*(char *)(gAssertion_tbl[gAssertion_tbl_idx].oname + i) = *(char *)(oname + i);
		}
	}
	gAssertion_tbl_idx = (gAssertion_tbl_idx >= (ASSERTION_LOG_TBL_SIZE-1)) ? 0 : (gAssertion_tbl_idx+1);
#endif
}

/* File End */
