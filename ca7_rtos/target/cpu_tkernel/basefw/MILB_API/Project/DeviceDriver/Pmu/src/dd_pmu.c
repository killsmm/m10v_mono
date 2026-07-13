/**
 * @file		dd_pmu.c
 * @brief		PMU driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include <string.h>
#include "pmc.h"
#include "dd_pmu.h"
#include "dd_arm.h"
#include "ddim_user_custom.h"

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
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static ULONG gDD_PMU_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief  Get status of PMU PD function.
*/
INT32 Dd_PMU_Get_Status(E_DD_PMU_PD_TYPE pd_type, ULONG* value)
{
	if (value == NULL) {
		return D_DD_PMU_INPUT_PARAM_ERR;
	}
	*value = 0x0;
	if (pd_type == E_DD_PMU_PD_TYPE_RESERVED || pd_type >= E_DD_PMU_PD_TYPE_MAX) {
		return D_DD_PMU_INPUT_PARAM_ERR;
	}
	
	// Get Power status.
	Dd_ARM_Critical_Section_Start( gDD_PMU_Spin_Lock );
	*value = IO_PMC.PMU.PWR_STR & (1 << pd_type);		// PD#0~6
	Dd_ARM_Critical_Section_End( gDD_PMU_Spin_Lock );
	*value = (*value >> pd_type) & 0x1;
	
	return D_DDIM_OK;
}

/**
 * @brief  PMU PD Off function.
*/
INT32 Dd_PMU_PD_Off(E_DD_PMU_PD_TYPE pd_type)
{
	int iDelayCount;
	ULONG pmuStatus = 0;
	
	if (pd_type == E_DD_PMU_PD_TYPE_RESERVED || pd_type >= E_DD_PMU_PD_TYPE_MAX) {
		return D_DD_PMU_INPUT_PARAM_ERR;
	}
	
	Dd_ARM_Critical_Section_Start( gDD_PMU_Spin_Lock );
	// Get Power status.
	pmuStatus = IO_PMC.PMU.PWR_STR & 0x0000007F;		// PD#0~6
	
	// To make sure that the operation of the PD target macro has been completed.
	
	// On chip PG Power off.
	if (pmuStatus != 0) {
//		pmuStatus = ~pmuStatus;		// PD#0~6
//		pmuStatus &= (0xffffffff - (1 << pd_type));
		pmuStatus = (1 << pd_type);
		
		// Set the Power OFF request.
//		IO_PMC.PMU.PWR_DN_EN |= pmuStatus;
		IO_PMC.PMU.PWR_DN_EN = pmuStatus;
		
		// Power-switch-down done Interrupt Clear.
		IO_PMC.PMU.PMU_INT_CLR.bit.PDNIC = 1;
		
		// Power-switch-down done Interrupt Enalbe.
		IO_PMC.PMU.PMU_INT_EN.bit.PDNIE = 1;
		
		// Power Down SET.
		IO_PMC.PMU.PMU_CTL.word |= 0x0000AA02;
		
		for (iDelayCount = 0; iDelayCount < 100; iDelayCount++) {
			// Check Power-switch-down done Interrupt.
			if (IO_PMC.PMU.PMU_INT_ST.bit.PDNI == 1) {
				break;
			}
			// 1ms wait.
			DDIM_User_Dly_Tsk(1);
		}
		
		// Power-switch-down done Interrupt Clear.
		IO_PMC.PMU.PMU_INT_CLR.bit.PDNIC = 1;
	}
	Dd_ARM_Critical_Section_End( gDD_PMU_Spin_Lock );
	DDIM_User_Dly_Tsk(0);
	
	return D_DDIM_OK;
}

/**
 * @brief  PMU PD on function.
*/
INT32 Dd_PMU_PD_On(E_DD_PMU_PD_TYPE pd_type)
{
	int iDelayCount;
	ULONG pmuStatus = 0;
	
	if (pd_type == E_DD_PMU_PD_TYPE_RESERVED || pd_type >= E_DD_PMU_PD_TYPE_MAX) {
		return D_DD_PMU_INPUT_PARAM_ERR;
	}
	
	Dd_ARM_Critical_Section_Start( gDD_PMU_Spin_Lock );
	// Get Power status.
	pmuStatus = IO_PMC.PMU.PWR_STR & 0x0000007F;		// PD#0~6
	
	// On chip PG Power on.
	if ((pmuStatus & (1 << pd_type)) == 0) {
//		pmuStatus &= (0xffffffff - (1 << pd_type));
		pmuStatus = (1 << pd_type);
		
//		IO_PMC.PMU.PWR_ON_EN0 |= pmuStatus;
		IO_PMC.PMU.PWR_ON_EN0 = pmuStatus;
		
		// Power-switch-ON done Interrupt Clear.
		IO_PMC.PMU.PMU_INT_CLR.bit.PONIC = 1;
		
		// Power-switch-ON done Interrupt Enalbe.
		IO_PMC.PMU.PMU_INT_EN.bit.PONIE = 1;
		
		// Power Up SET.
		IO_PMC.PMU.PMU_CTL.word |= 0x0000AA01;
		
		for (iDelayCount = 0; iDelayCount < 100; iDelayCount++) {
			// Check Power-switch-ON done Interrupt.
			if (IO_PMC.PMU.PMU_INT_ST.bit.PONI == 1) {
				break;
			}
			// 1ms wait.
			DDIM_User_Dly_Tsk(1);
		}
		
		// Power-switch-ON done Interrupt Clear.
		IO_PMC.PMU.PMU_INT_CLR.bit.PONIC = 1;
	}
	Dd_ARM_Critical_Section_End( gDD_PMU_Spin_Lock );
	DDIM_User_Dly_Tsk(0);
	
	return D_DDIM_OK;
}

