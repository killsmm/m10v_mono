/**
 * <p>Copyright 2015 Socionext Inc.
 * AE procedure
 * @file ae_procedure.h
 * @note 
 * @attention 
 */
#include "ddim_typedef.h"

#define FLG_AE_PROC_EXECUTE		(0x00000001)
#define FLG_AE_PROC_TERMINATE	(0x00000002)
#define FLG_AE_PROC_SET_BRIGHT	(0x00000008)

#define AE_PROC_INTERVAL_30FPS	(1)
#define AE_PROC_INTERVAL_60FPS	(2)


VOID Ae_Proc_Init(VOID);
VOID Ae_Proc_Set_Prm_Draft(VOID);
VOID Ae_Proc_Set_Prm_Still(VOID);
VOID Ae_Proc_Start(VOID);
VOID Ae_Proc_Terminate(VOID);
VOID Ae_Proc_Execute(VOID);
VOID Ae_Proc_Update_EV(int ev);
VOID Ae_Proc_Update_EV_Offset(int ev_offset);
VOID Ae_Proc_Ae_Lock(VOID);
VOID Ae_Proc_Ae_Unlock(VOID);
VOID Ae_Proc_Still_End(VOID);
VOID Ae_Proc_Set_Interval(UCHAR interval);
VOID Ae_Proc_Set_Ev_Offset(double ev_offset);

INT32 Ae_Proc_Get_Ev(VOID);

VOID Ae_Proc_Set_Task_Priority(UCHAR priority);
