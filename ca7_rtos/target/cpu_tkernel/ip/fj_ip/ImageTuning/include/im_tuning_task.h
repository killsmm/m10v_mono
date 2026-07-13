#ifndef IM_TUNING_TASK_H_INC
#define IM_TUNING_TASK_H_INC

// ****************************************************************************
//  Include
// ****************************************************************************
//#include "common.h"
#include "ddim_typedef.h"

// ****************************************************************************
//  Definition
// ****************************************************************************
#define TASK_IQTUNE_IDLE	0
#define TASK_IQTUNE_BUSY	1


// ****************************************************************************
//  Global Function Definition
// ****************************************************************************

extern VOID IQ_Command_Observer(INT32 stacd);
_GetResource* MIL_Get_Resource_List(VOID);
ULONG Mil_Get_BaudRate(VOID);


#endif // IM_TUNING_TASK_H_INC
