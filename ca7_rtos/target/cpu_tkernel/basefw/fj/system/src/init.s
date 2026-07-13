/**
 * @file		init.s
 * @brief		Initialize Procedure
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//---------------------------------------------------------------
//   Condition Assenble Control
//---------------------------------------------------------------
	.text
	.section .INIT_CODE
    .align 2

//---------------------------------------------------------------
// Init_CODE
//---------------------------------------------------------------
	.type	Init_CODE, %function
	.global	Init_CODE
Init_CODE:

//---------------------------------------------------------------
//   Include Files(s)
//---------------------------------------------------------------
	#include "milb.inc"

	.extern _c_start
	.extern	MMU_PageTable
	.extern	invalidate_dcache

//---------------------------------------------------------------
//   Define(s)
//---------------------------------------------------------------
	.equ	CPU0_NUM,		0
	.equ	CPU1_NUM,		1
	.equ	CPU2_NUM,		2
	.equ	CPU3_NUM,		3
	.equ	DEBUG_ADR,		0x0100FFF8
	.equ	DEBUG_VAL,		0xBEEFCAFE
	.equ	WAIT_ADR,		0x0100FFF0
	.equ	WAIT_VAL,		0xBEEFCAFE

//---------------------------------------------------------------
//   Macro(s)
//---------------------------------------------------------------
	.macro get_cpuid  reg
	MRC		p15, 0, \reg, c0, c0, 5		// Read CP15 Main ID Register (MIDR) into \reg
	AND		\reg, \reg, #0xf
	.endm

	.macro disable_mem  reg
	MRC		p15, 0, \reg, c1, c0, 0		// Read CP15 System Control Register (SCTLR) into \reg
	BIC		\reg, \reg, #(1 << 0)		// Clear MMU enable
	BIC		\reg, \reg, #(1 << 2)		// Disable D-cache
	BIC		\reg, \reg, #(1 << 11)      // Branch Prediction Disable
	BIC		\reg, \reg, #(1 << 12)		// Disable I-cache
	MCR		p15, 0, \reg, c1, c0, 0		// Write value back
	.endm

//---------------------------------------------------------------
// Entry
//---------------------------------------------------------------
	MOV		r0, #0
	MOV		r1, #0
	MOV		r2, #0
	MOV		r3, #0
	MOV		r4, #0
	MOV		r5, #0
	MOV		r6, #0
	MOV		r7, #0
	MOV		r8, #0
	MOV		r9, #0
	MOV		r10, #0
	MOV		r11, #0
	MOV		r12, #0
	LDR		sp, =0x0
	MOV		lr, #0

#ifdef CO_TKERNEL_ONLY
	// MMU disable
	// All data accesses are treated as Non-cacheable and Strongly-ordered
	// All instruction accesses are Non-cacheable, because SCTLR. I-bit = 0 for Cortex-A5.
	disable_mem r0
#endif

	// configure CPU
	MRC		p15, 0, r0, c1, c0, 1				// Read CP15 Auxiliary Control Register (ACTLR)
	BIC		r0, r0, #(3 << 13)					// Data prefetching disable
	ORR		r0, #(1 << 6)						// SMP Mode
	MCR		p15, 0, r0, c1, c0, 1				// Write value back
	DSB
	ISB

#ifdef CO_TKERNEL_ONLY
	// You must invalidate the instruction cache, the data cache, TLB, and BTAC before using them.[TRM]
	// Invalidate L1 Caches
	MOV		r0, #0
	MCR		p15, 0, r0, c8, c7, 0				// TLBIALL - Invalidate entire Unifed TLB
	MCR		p15, 0, r0, c7, c5, 6				// BPIALL - Invalidate entire branch predictor array
	MCR		p15, 0, r0, c7, c5, 0				// ICIALLU - Invalidate all instruction cache to PoU
#endif

	get_cpuid r1
	CMP		r1, #CPU3_NUM
	BEQ		__cpu_idle_loop
//	CMP		r1, #CPU0_NUM
//	BNE		__cpu_idle_loop

#ifdef CO_PMU_MONITOR_ON
	MOV		r0, #0x1							// enable user mode access to the PMU registers
	MCR		p15, 0, r0, c9, c14, 0				// Write CP15 User Enable Register (PMUSERENR)
#endif

	// Set PageTable
	LDR		r0, =MMU_PageTable					// table walk address is PA from TTBR0 and VA-index

	// sets up TTBR0
	MCR 	p15, 0, r0, c2, c0, 0				// Write CP15 Translation Table Base Register 0 (TTBR0)

	// TTBCR 
	// if N == 0 then always use TTBR0 [ARMARM B3-118]
	MOV		r0, #0x0
	MCR		p15, 0, r0, c2, c0, 2				// Write CP15 Translation Table Base Control Register (TTBCR)

	LDR		r0, =0x55555555						// All Client
	MCR		p15, 0, r0, c3, c0, 0				// Write CP15 Domain Access Control Register (DACR)

	//--------------------------------------------------------------
	// I, C bit of CP15r1 (I/D Cache)
	//--------------------------------------------------------------
	MRC		p15, 0, r0, c1, c0, 0				// Read CP15 System Control Register (SCTLR)
	ORR		r0, r0, #(1 << 12)					// Enable I-cache
	ORR		r0, r0, #(1 << 10)					// SWP, SWPB Enable
	ORR		r0, r0, #(1 << 2)					// Enable D-cache
	BIC		r0, r0, #(1 << 1)					// Disable Alignment falut
	ORR		r0, r0, #(1 << 0)					// Enable MMU
	MCR		p15, 0, r0, c1, c0, 0				// Write value back
	DSB
	ISB

    LDR		r0, =RAM_AREA
	MCR 	p15, 0, r0, c12, c0, 0				// Write CP15 Vector Base Address Register (VBAR)

#ifdef CO_TKERNEL_ONLY
	__gic_start:
#else
	LDR		r0, = DEBUG_ADR
	LDR		r1, = DEBUG_VAL
    LDR		r2, [r0]
    CMP		r2, r1
	BEQ		__gic_start
	BNE		__gic_end
#endif

__gic_start:
	get_cpuid r1
	CMP		r1, #CPU0_NUM
	BEQ		__gic_set
	BNE		__gic_bank_set

__gic_set:
	BL		initialize_gic
	BL		init_dist_bank_gic
	BL		init_cpu_bank_gic
	BL		enable_gic
	B		__gic_end

__gic_bank_set:
	LDR		r0, = GICD_CTLR
	LDR		r1, [r0]
	CMP		r1, #0x00000003
	BNE		__gic_bank_set
	BL		init_dist_bank_gic
	BL		init_cpu_bank_gic

__gic_end:

	get_cpuid r0
	CMP	    r0, #CPU1_NUM
	BEQ		set_cpu1
	CMP	    r0, #CPU2_NUM
	BEQ		set_cpu2

	LDR		r1, = WAIT_ADR
	LDR		r0, = 0x0000000
	STR		r0, [r1]

// Copy data  
	LDR		r0, =__data_org
	LDR		r2, =__data_start
	LDR		r4, =__data_size
1:
	LDMIA   r0!, {r5-r12}
	STMIA   r2!, {r5-r12}
	SUBS    r4, #32
	BNE     1b

	/* bss */
	ldr     r1, =__bss_start
	ldr     r2, =__bss_end
	cmp     r1, r2
	bhs     2f
	mov     r0, #0
1:
	str     r0, [r1], #4
	cmp     r1, r2
	blo     1b
2:
	LDR		r1, = WAIT_ADR
	LDR		r0, = WAIT_VAL
	STR		r0, [r1]
	SEV

	ldr     r12, =_c_start
	blx     r12
3:
	b       3b

//----------------------------------------------------------------------
// Jump to SUB core code
//----------------------------------------------------------------------
__cpu_idle_loop:
	WFE
	B		__cpu_idle_loop

set_cpu1:
set_cpu1_wait:
	WFE
	LDR		r1, = WAIT_ADR
	LDR		r2, = WAIT_VAL
__cpu1_loop:
    LDR		r0, [r1]
    CMP		r0, r2
    BNE		__cpu1_loop
	DMB

	LDR     r12, =_c_start
	BLX     r12

set_cpu2:
set_cpu2_wait:
	WFE
	LDR		r1, = WAIT_ADR
	LDR		r2, = WAIT_VAL
__cpu2_loop:
    LDR		r0, [r1]
    CMP		r0, r2
    BNE		__cpu2_loop
	DMB

	LDR     r12, =_c_start
	BLX     r12

	.end
