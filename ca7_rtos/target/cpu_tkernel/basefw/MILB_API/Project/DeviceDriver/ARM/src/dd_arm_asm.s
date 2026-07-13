/**
 * @file		dd_arm_asm.s
 * @brief		ARM driver
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//---------------------------------------------------------------
//  Condition Assenble Control                                   
//---------------------------------------------------------------
	.text
	.section .DD_ARM_ASM
	.align 2

//---------------------------------------------------------------
//  Include Files                                                
//---------------------------------------------------------------
	#include "milb.inc"

//---------------------------------------------------------------
//  Definition External Valiable                                 
//---------------------------------------------------------------
// Nothing Special

//---------------------------------------------------------------
//  Definition Constant                                          
//---------------------------------------------------------------
	.equ	IRQ_FIQ_BIT,	(I_Bit | F_Bit)

//---------------------------------------------------------------
//  Local Function                                               
//---------------------------------------------------------------
// Nothing Special

//---------------------------------------------------------------
//  Global Function                                              
//---------------------------------------------------------------

//-------------------------------------------
// Wait Count
//-------------------------------------------
	.type Dd_ARM_Wait_Count, %function
	.global	Dd_ARM_Wait_Count
Dd_ARM_Wait_Count:
									// Argument
									// r0 : Wait Time[ns]
L1.0:
	SUBS	r0,r0,#1
	BNE		L1.0
	BX		lr

//-------------------------------------------
// EI (IRQ Enable)
//-------------------------------------------
	.type Dd_ARM_EI, %function
	.global Dd_ARM_EI
Dd_ARM_EI:
									// Argument
									// r0 : Program status
	MRS		r1, cpsr
	AND		r0, #I_Bit
	BIC	 	r1, #I_Bit				// IRQ Mask bit => 0
	ORR		r1, r1, r0
	MSR 	cpsr_c, r1
	BX		lr						// return to caller

//-------------------------------------------
// DI (IRQ Disable)
//-------------------------------------------
	.type Dd_ARM_DI, %function
	.global Dd_ARM_DI
Dd_ARM_DI:
	MRS 	r0, cpsr
	CPSID	i						// IRQ Mask bit => 1
	BX		lr						// return to caller

//-------------------------------------------
// Spin_Lock
//-------------------------------------------
	.type Dd_ARM_Spin_Lock, %function
	.global Dd_ARM_Spin_Lock
Dd_ARM_Spin_Lock:
									// Argument
									// r0 : Lock address
	MOV		r1, #0x1				// Load 'lock taken' value
try_for_lock:
	CPSID	f
	LDREX	r2, [r0]				// Load lock value
	CMP		r2, #0					// Is lock free?
	BNE		1f
	STREX	r2, r1, [r0]			// Yes, so try to claim
	CPSIE	f
	CMP		r2, #0					// Succeeded?
	BNE		try_for_lock			// No, so try again
									// Yes, locked
	BX		lr
1:									// spin lock address is locked.
	CPSIE	f
	WFE								// go into standby
	B		try_for_lock

//-------------------------------------------
// Spin_Lock_Ret
//-------------------------------------------
	.type Dd_ARM_Spin_Lock_Ret, %function
	.global Dd_ARM_Spin_Lock_Ret
Dd_ARM_Spin_Lock_Ret:
									// Argument
									// r0 : Lock address
	MOV		r1, #0x1				// Load 'lock taken' value
	CPSID	f
	LDREX	r2, [r0]				// Load lock value
	CMP		r2, #0					// Is lock free?
	BNE		lock_ng
	STREX	r2, r1, [r0]			// Yes, so try to claim
	CPSIE	f
	CMP		r2, #0					// Succeeded?
	BEQ		lock_ok					// Yes, locked
lock_ng:
	CPSIE	f
	MOV		r0, #1
	BX		lr
lock_ok:
	MOV		r0, #0
	BX		lr

//-------------------------------------------
// Spin_Lock_Composite
//-------------------------------------------
	.type Dd_ARM_Spin_Lock_Composite, %function
	.global Dd_ARM_Spin_Lock_Composite
Dd_ARM_Spin_Lock_Composite:
	MRS		r3, cpsr
	PUSH	{r3,lr}
	TST		r3, #I_Bit
	MOV		r3, r0
	BEQ		7f
2:									// disable IRQ
	BLX		Dd_ARM_Spin_Lock_Ret
	CMP		r0, #0
	BEQ		9f
	MOV		r0, r3
	WFE
	B		2b
7:									// enable IRQ
	CPSID	i						// IRQ Mask bit => 1
	BLX		Dd_ARM_Spin_Lock_Ret
	CMP		r0, #0
	BEQ		9f
	CPSIE	i						// IRQ Mask bit => 0
	MOV		r0, r3
	WFE
	B		7b
9:
	POP		{r0,pc}

//-------------------------------------------
// Spin_Unlock
//-------------------------------------------
	.type Dd_ARM_Spin_Unlock, %function
	.global Dd_ARM_Spin_Unlock
Dd_ARM_Spin_Unlock:
									// Argument
									// r0 : Lock address
	MOV		r1, #0x0				// Store 'unlock taken' value
	DMB
	STR		r1, [r0]
	DSB
	SEV								// Send event to other CPUs(using WFE)
	BX		lr

//-------------------------------------------
// Get CPU ID
//-------------------------------------------
	.type Dd_ARM_Get_CPU_ID, %function
	.global Dd_ARM_Get_CPU_ID
Dd_ARM_Get_CPU_ID:
	MRC		p15, 0, r0, c0, c0, 5	// Read MPIDR
	AND		r0, #0x3				// Get CPU ID bit
	BX		lr

//-------------------------------------------
// Get Translation Table Base
//-------------------------------------------
	.type Dd_ARM_Get_TTB, %function
	.global Dd_ARM_Get_TTB
Dd_ARM_Get_TTB:
	MRC		p15, 0, r0, c2, c0, 0
	BX		lr

//-------------------------------------------
// Change Vector Table
//-------------------------------------------
	.type Dd_ARM_Change_Vector_Table, %function
	.global Dd_ARM_Change_Vector_Table
Dd_ARM_Change_Vector_Table:
									// Argument
									// r0 : Base address
	MCR		p15, 0, r0, c12, c0, 0
	BX		lr

//-------------------------------------------
// L1 Prefetch Disable
//-------------------------------------------
	.type Dd_ARM_L1_Prefetch_Disable, %function
	.global Dd_ARM_L1_Prefetch_Disable
Dd_ARM_L1_Prefetch_Disable:
	MRC		p15, 0, r0, c1, c0, 1
	BIC		r0, r0, #(3 << 13)
	MCR		p15, 0, r0, c1, c0, 1
	DSB
	BX		lr

//-------------------------------------------
// L1 Prefetch Enable
//-------------------------------------------
	.type Dd_ARM_L1_Prefetch_Enable, %function
	.global Dd_ARM_L1_Prefetch_Enable
Dd_ARM_L1_Prefetch_Enable:
	MRC		p15, 0, r0, c1, c0, 1
	ORR		r0, r0, #(3 << 13)
	MCR		p15, 0, r0, c1, c0, 1
	DSB
	BX		lr

//-------------------------------------------
// EIF (Enable IRQ & FIQ)
//-------------------------------------------
	.type Dd_ARM_EIF, %function
	.global Dd_ARM_EIF
Dd_ARM_EIF:
									// Argument
									// r0 : Program status
	MRS		r1, cpsr
	AND		r0, #IRQ_FIQ_BIT
	BIC		r1, #IRQ_FIQ_BIT		// IRQ & FIQ Mask bit => 0
	ORR		r1, r1, r0
	MSR		cpsr, r1
	BX		lr

//-------------------------------------------
// DIF (Disable IRQ & FIQ)
//-------------------------------------------
	.type Dd_ARM_DIF, %function
	.global Dd_ARM_DIF
Dd_ARM_DIF:
	MRS		r0, cpsr
	ORR		r1, r0, #IRQ_FIQ_BIT	// IRQ & FIQ Mask bit => 1
	MSR		cpsr, r1
	BX		lr

//-------------------------------------------
// Disable MMU
//-------------------------------------------
	.type Dd_ARM_MMU_Disable, %function
	.global Dd_ARM_MMU_Disable
Dd_ARM_MMU_Disable:
	LDR		r1, =0xFFFFFFFA
	MRC		p15, 0, r0, c1, c0, 0
	AND		r0, r0, r1
	MCR		p15, 0, r0, c1, c0, 0
	BX		lr

//-------------------------------------------
// Enable MMU
//-------------------------------------------
	.type Dd_ARM_MMU_Enable, %function
	.global Dd_ARM_MMU_Enable
Dd_ARM_MMU_Enable:
	LDR		r1, =0x00000005
	MRC		p15, 0, r0, c1, c0, 0
	ORR		r0, r0, r1
	MCR		p15, 0, r0, c1, c0, 0
	BX		lr

	.end
