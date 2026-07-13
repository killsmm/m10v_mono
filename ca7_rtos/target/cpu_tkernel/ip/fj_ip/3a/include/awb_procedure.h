/**
 * <p>Copyright 2015 Socionext Inc.
 * AWB procedure
 * @file awb_procedure.h
 * @note 
 * @attention 
 */
#include "ddim_typedef.h"

#define FLG_AWB_PROC_EXECUTE	(0x00000001)
#define FLG_AWB_PROC_TERMINATE	(0x00000002)

#define AWB_PROC_INTERVAL_30FPS	(2)
#define AWB_PROC_INTERVAL_60FPS	(4)

typedef enum {
	E_AWB_PROC_MODE_AUTO = 0,
	E_AWB_PROC_MODE_CLOUDY,
	E_AWB_PROC_MODE_INCANDESCENT_LAMP,
	E_AWB_PROC_MODE_FLUORESCENT_LAMP,
	E_AWB_PROC_MODE_SUNLIGHT,
	E_AWB_PROC_MODE_MANUAL
} E_AWB_PROC_MODE;

VOID Awb_Proc_Init(VOID);
VOID Awb_Proc_Set_Prm_Draft(VOID);
VOID Awb_Proc_Set_Prm_Still(VOID);
VOID Awb_Proc_Start(VOID);
VOID Awb_Proc_Terminate(VOID);
VOID Awb_Proc_Execute(VOID);
VOID Awb_Proc_Change_Mode(E_AWB_PROC_MODE wb_mode);
VOID Awb_Proc_Set_Interval(UCHAR interval);

VOID Awb_Proc_Set_Manual(VOID);
VOID Awb_Proc_Set_Auto(VOID);
VOID Awb_Proc_Set_ManualGain( USHORT r_gain, USHORT g_gain, USHORT b_gain );

VOID Awb_Proc_Set_Task_Priority(UCHAR priority);
