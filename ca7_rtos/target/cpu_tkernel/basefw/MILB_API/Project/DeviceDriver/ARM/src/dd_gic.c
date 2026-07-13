/**
 * @file		dd_gic.c
 * @brief		GIC (Generic Interrupt Controller) driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dd_gic.h"
#include "dd_arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

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
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief	Initialize Generic Interrupt Controller.
 */
VOID Dd_GIC_Init(VOID)
{
	USHORT	i;
	
	//----------------------------------------------------------------------
	// Distributor side Disable
	// Distributor Control Register (GICD_CTLR)
	//----------------------------------------------------------------------
	Dd_GIC_DIST_Set_Ctlr_EnableGrp0(0x0);
	Dd_GIC_DIST_Set_Ctlr_EnableGrp1(0x0);
	
	//----------------------------------------------------------------------
	// Distributor Bank Register Initialize
	//----------------------------------------------------------------------
	Dd_GIC_Dist_Bank_Init();
	
	//----------------------------------------------------------------------
	// Disable Interrupt
	// Interrupt Clear-Enable Register (GICD_ICENABLER)
	//----------------------------------------------------------------------
	// SGIs are always enabled.
	for( i=1; i<(D_DD_GIC_INTID_MAX/32); i++ ){
		Dd_GIC_DIST_Set_ICEnabler(i, 0xFFFFFFFF);
	}
	
	//----------------------------------------------------------------------
	// Clear Interrupt Pending
	// Interrupt Clear-Pending Register (GICD_ICPENDR)
	//----------------------------------------------------------------------
	// SGIs are always enabled.
	for( i=1; i<(D_DD_GIC_INTID_MAX/32); i++ ){
		Dd_GIC_DIST_Set_ICPendr(i, 0xFFFFFFFF);
	}
	
	//----------------------------------------------------------------------
	// Clear Interupt Priority
	// Interrupt Priority Register (GICD_IPRIORITYR)
	//----------------------------------------------------------------------
	for( i=32; i<D_DD_GIC_INTID_MAX; i++ ){
		Dd_GIC_DIST_Set_IPriorityr(i, 0x0);
	}
	
	//----------------------------------------------------------------------
	// Clear Target CPU
	// Interrupt Processor Targets Register (GICD_ITARGETSR)
	//----------------------------------------------------------------------
	// The Distributor does not provide registers for INTIDs < 32.(SGI and PPI)
	for( i=32; i<D_DD_GIC_INTID_MAX; i++ ){
		Dd_GIC_DIST_Set_ITargetsr(i, 0x0);
	}
	
	//----------------------------------------------------------------------
	// "Level-sensitive" or "Edge-triggered"
	// Interrupt Configuration Register (GICD_ICFGR)
	//----------------------------------------------------------------------
	// SGIs are always pulse-sensitive
	// PPIs are configured during the GIC configuration process
	for( i=1; i<(D_DD_GIC_INTID_MAX/16); i++ ){
		Dd_GIC_DIST_Set_ICfgr(i, 0x55555555);
	}
	
	//----------------------------------------------------------------------
	// CPU Interface Control Register Initialize
	//----------------------------------------------------------------------
	Dd_GIC_Cpu_Init();
	
	//----------------------------------------------------------------------
	// Distributor side Enable
	// Distributor Control Register (GICD_CTLR)
	//----------------------------------------------------------------------
	Dd_GIC_DIST_Set_Ctlr_EnableGrp0(0x1);
	Dd_GIC_DIST_Set_Ctlr_EnableGrp1(0x0);
	
	Dd_ARM_Dsb_Pou();
}

/**
 * @brief	Initialize Distributor Bank Register.
 */
VOID Dd_GIC_Dist_Bank_Init(VOID)
{
	USHORT	i;
	
	//----------------------------------------------------------------------
	// Disable Interrupt
	// Interrupt Clear-Enable Register (GICD_ICENABLER)
	//----------------------------------------------------------------------
	// SGIs are always enabled.
	Dd_GIC_DIST_Set_ICEnabler(0, 0xFFFFFFFF);
	
	//----------------------------------------------------------------------
	// Clear Interrupt Pending
	// Interrupt Clear-Pending Register (GICD_ICPENDR)
	//----------------------------------------------------------------------
	// SGIs are always enabled.
	Dd_GIC_DIST_Set_ICPendr(0, 0xFFFFFFFF);
	
	//----------------------------------------------------------------------
	// Clear Interrupt Priority
	// Interrupt Priority Register (GICD_IPRIORITYR)
	//----------------------------------------------------------------------
	for( i=0; i<32; i++ ){
		Dd_GIC_DIST_Set_IPriorityr(i, 0x0);
	}
	
	Dd_ARM_Dsb_Pou();
}

/**
 * @brief	Initialize CPU Interface Control Register.
 */
VOID Dd_GIC_Cpu_Init(VOID)
{
	//----------------------------------------------------------------------
	// CPU I/F side Disable
	// CPU Interface Control Register (GICC_CTLR)
	//----------------------------------------------------------------------
	Dd_GIC_CPU_Set_Ctlr_EnableGrp0(0x0);
	Dd_GIC_CPU_Set_Ctlr_EnableGrp1(0x0);
	
	//----------------------------------------------------------------------
	// Set Priority Mask
	// Interrupt Priority Mask Register (GICC_PMR)
	//----------------------------------------------------------------------
	Dd_GIC_CPU_Set_Pmr_Priority(0xF8);
	
	//----------------------------------------------------------------------
	// Set Binary point
	// Binary Point Register (GICC_BPR)
	//----------------------------------------------------------------------
	Dd_GIC_CPU_Set_Bpr_BinaryPoint(0x2);
	
	//----------------------------------------------------------------------
	// When Exist Interrupt Acknowledge Register
	// Set End of Interrupt Register
	//----------------------------------------------------------------------
	do {
		UINT32 temp;
		
		temp = Dd_GIC_CPU_Get_Iar;
		if( (temp & D_DD_GIC_INTERRUPT_MASK) == D_DD_GIC_SPURIOUS_INTERRUPT ){
			break;
		}
		Dd_GIC_CPU_Set_AEoir(temp);
	} while(1);
	
	//----------------------------------------------------------------------
	// CPU I/F side Enable
	// CPU Interface Control Register (GICC_CTLR)
	//----------------------------------------------------------------------
	Dd_GIC_CPU_Set_Ctlr_EnableGrp0(0x1);
	Dd_GIC_CPU_Set_Ctlr_EnableGrp1(0x0);
	
	Dd_ARM_Dsb_Pou();
}

/**
 * @brief	End Generic Interrupt Controller.
 */
VOID Dd_GIC_End(VOID)
{
	//----------------------------------------------------------------------
	// CPU I/F side
	// CPU Interface Control Register (GICC_CTLR)
	//----------------------------------------------------------------------
	Dd_GIC_CPU_Set_Ctlr_EnableGrp0(0x0);
	Dd_GIC_CPU_Set_Ctlr_EnableGrp1(0x0);
	
	//----------------------------------------------------------------------
	// Distributor side
	// Distributor Control Register (GICD_CTLR)
	//----------------------------------------------------------------------
	Dd_GIC_DIST_Set_Ctlr_EnableGrp0(0x0);
	Dd_GIC_DIST_Set_Ctlr_EnableGrp1(0x0);
	
	Dd_ARM_Dsb_Pou();
}

/**
 * @brief	Control Generic Interrupt Controller to set resource.
 * @param	E_DD_GIC_INTID	intid
			ULONG			set_enable
			LONG			intrpt_pri
			LONG			cpu_target
 */
INT32 Dd_GIC_Ctrl(E_DD_GIC_INTID intid, ULONG set_enable, LONG intrpt_pri, LONG cpu_target)
{
	ULONG num;
	INT32 sho;
	
#ifdef CO_PARAM_CHECK
	USHORT flg;
	
	if( intid > D_DD_GIC_INTID_SPI_MAX ){
		Ddim_Assertion(("GIC: input param error. [intid] = %x\n", (UINT32)intid));
		return D_DD_GIC_INPUT_PARAM_ERR;
	}
	if( set_enable > 1 ){
		Ddim_Assertion(("GIC: input param error. [set_enable] = %x\n", (UINT32)set_enable));
		return D_DD_GIC_INPUT_PARAM_ERR;
	}
	flg = 0;
	num = 0;
	while( num <= 248 && intrpt_pri >= 0 ){
		if( intrpt_pri == num ){
			flg = 1;
			break;
		}
		if( intrpt_pri < num ){
			break;
		}
		num += 8;
	}
	if( intrpt_pri != -1 && flg == 0 ){
		Ddim_Assertion(("GIC: input param error. [intrpt_pri] = %x\n", (INT32)intrpt_pri));
		return D_DD_GIC_INPUT_PARAM_ERR;
	}
	if( cpu_target < -1 || cpu_target > 15 ){
		Ddim_Assertion(("GIC: input param error. [cpu_target] = %x\n", (INT32)cpu_target));
		return D_DD_GIC_INPUT_PARAM_ERR;
	}
#endif
	//----------------------------------------------------------------------
	// Disable Interrupt
	// Interrupt Clear-Enable Register (GICD_ICENABLER)
	//----------------------------------------------------------------------
	// SGIs are always enabled.
	if( intid > D_DD_GIC_INTID_SGI_MAX && intid <= D_DD_GIC_INTID_SPI_MAX ){
		sho = intid / 32;
		num = 1 << (intid - sho * 32 );
		Dd_GIC_DIST_Set_ICEnabler(sho, num);
	}
	
	//----------------------------------------------------------------------
	// Set Interupt Priority
	// Interrupt Priority Register (GICD_IPRIORITYR)
	//----------------------------------------------------------------------
	Dd_GIC_Set_Priority(intid, intrpt_pri);
	
	//----------------------------------------------------------------------
	// Set Target CPU
	// Interrupt Processor Targets Register (GICD_ITARGETSR)
	//----------------------------------------------------------------------
	Dd_GIC_Set_Target_Cpu(intid, cpu_target);
	
	//----------------------------------------------------------------------
	// Enable Interrupt
	// Interrupt Set-Enable Registers (GICD_ISENABLER)
	//----------------------------------------------------------------------
	// SGIs are always enabled.
	if(set_enable){
		if( intid > D_DD_GIC_INTID_SGI_MAX && intid <= D_DD_GIC_INTID_SPI_MAX ){
			sho = intid / 32;
			num = set_enable << (intid - sho * 32);
			Dd_GIC_DIST_Set_ISEnabler(sho, num);
		}
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Interrupt Priority Registers.
 * @param	ULONG	intid
			LONG	intrpt_pri
 */
INT32 Dd_GIC_Set_Priority(E_DD_GIC_INTID intid, LONG intrpt_pri)
{
#ifdef CO_PARAM_CHECK
	USHORT flg;
	ULONG num;
	
	if( intid > D_DD_GIC_INTID_SPI_MAX ){
		Ddim_Assertion(("GIC: input param error. [intid] = %x\n", (UINT32)intid));
		return D_DD_GIC_INPUT_PARAM_ERR;
	}
	flg = 0;
	num = 0;
	while( num <= 248 && intrpt_pri >= 0 ){
		if( intrpt_pri == num ){
			flg = 1;
			break;
		}
		if( intrpt_pri < num ){
			break;
		}
		num += 8;
	}
	if( intrpt_pri != -1 && flg == 0 ){
		Ddim_Assertion(("GIC: input param error. [intrpt_pri] = %x\n", (INT32)intrpt_pri));
		return D_DD_GIC_INPUT_PARAM_ERR;
	}
#endif
	//----------------------------------------------------------------------
	// Set Interupt Priority
	// Interrupt Priority Register (GICD_IPRIORITYR)
	//----------------------------------------------------------------------
	if( intrpt_pri != -1 ){
		if( intid <= D_DD_GIC_INTID_SPI_MAX ){
			Dd_GIC_DIST_Set_IPriorityr(intid, intrpt_pri);
		}
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Interrupt Processor Targets Registers.
 * @param	ULONG	intid
			LONG	cpu_target
 */
INT32 Dd_GIC_Set_Target_Cpu(E_DD_GIC_INTID intid, LONG cpu_target)
{
#ifdef CO_PARAM_CHECK
	if( intid > D_DD_GIC_INTID_SPI_MAX ){
		Ddim_Assertion(("GIC: input param error. [intid] = %x\n", (UINT32)intid));
		return D_DD_GIC_INPUT_PARAM_ERR;
	}
	if( cpu_target < -1 || cpu_target > 15 ){
		Ddim_Assertion(("GIC: input param error. [cpu_target] = %x\n", (INT32)cpu_target));
		return D_DD_GIC_INPUT_PARAM_ERR;
	}
#endif
	//----------------------------------------------------------------------
	// Target CPU
	// Interrupt Processor Targets Register (GICD_ITARGETSR)
	//----------------------------------------------------------------------
	if( cpu_target != -1 ){
		if( intid <= D_DD_GIC_INTID_SPI_MAX ){
			Dd_GIC_DIST_Set_ITargetsr(intid, cpu_target);
		}
	}
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Software Generated Interrupt Registers.
 * @param	ULONG	intid
			ULONG	target_filter
			ULONG	cpu_target
 */
INT32 Dd_GIC_Send_Sgi(E_DD_GIC_INTID intid, ULONG target_filter, ULONG cpu_target)
{
#ifdef CO_PARAM_CHECK
	if( intid > D_DD_GIC_INTID_SGI_MAX ){
		Ddim_Assertion(("GIC: input param error. [intid] = %x\n", (UINT32)intid));
		return D_DD_GIC_INPUT_PARAM_ERR;
	}
	if( target_filter > 2 ){
		Ddim_Assertion(("GIC: input param error. [target_filter] = %x\n", (INT32)target_filter));
		return D_DD_GIC_INPUT_PARAM_ERR;
	}
	if( cpu_target > 15 ){
		Ddim_Assertion(("GIC: input param error. [cpu_target] = %x\n", (INT32)cpu_target));
		return D_DD_GIC_INPUT_PARAM_ERR;
	}
#endif
	ULONG sgi_set;
	
	// Select SATT bit value always secure.
	sgi_set = (ULONG)intid + (target_filter << 24) + (cpu_target << 16);
	
	Dd_GIC_DIST_Set_Sgir(sgi_set);
	
	Dd_ARM_Dsb_Pou();
	
	return D_DDIM_OK;
}
