/*
 *	boot.S (CA7MPCORE)
 *	System Startup
 */

#define	_in_asm_source_

#include "porting.h"


/*
 *************************************************************************
 *
 * Startup codes
 *
 *************************************************************************
*/
	.text
// MILB_SDK Add Start
	.section .BOOT_GCC
// MILB_SDK Add End
	.align 4
	.global	_Reset_Handler
	.type	_Reset_Handler, %function
// MILB_SDK Add Start
//	.extern	Init_CODE
	.extern	MMU_PageTable

	.macro get_cpuid  reg
	MRC		p15, 0, \reg, c0, c0, 5
	AND		\reg, \reg, #0xf
	.endm

// MILB_SDK Add End
_Reset_Handler:
	//SVC mode/Interrupt disable	
	msr	cpsr_c, #PSR_SVC|PSR_I|PSR_F

// MILB_SDK Add Start
	BL  Init_CODE
// MILB_SDK Add End

// MILB_SDK Update Start
#if 0
	get_cpuid r1
	CMP		r1, #0
	BNE		__cpu_idle_loop

	//Set VBAR : modify excep/int vector to the 'vector_base'
//	ldr	r4, =vector_base
	ldr	r4, =0xa0000000
	mcr	p15, 0, r4, c12, c0, 0

	// Invalidate L1 I/D
	dsb
	mov	r0, #0			// set up for MCR
	mcr	p15, 0, r0, c8, c7, 0	// invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0	// invalidate icache
	mcr	p15, 0, r0, c7, c5, 6	// invalidate BP array
	mcr	p15, 0, r0, c8, c3, 0	// invalidate UIS
	dsb
	isb

	dmb
	
	// disable i/d cache and mmu
	mrc	p15, 0, r4, c1, c0, 0
	bic	r4, r4, #CR1_C
	bic	r4, r4, #CR1_I
	bic	r4, r4, #CR1_Z
	bic	r4, r4, #CR1_M
	mcr	p15, 0, r4, c1, c0, 0

	// Set SMP bit
	mrc	p15, 0, r4, c1, c0, 1
	orr	r4, r4, #ACTLR_SMP
	mcr	p15, 0, r4, c1, c0, 1

	// distinguish core0 and core1
	mrc	p15, 0, r5, c0, c0, 5
	and	r5, r5, #3
	cmp	r5, #0
	
//	bleq	init_page_table
//
//	// set TTBR0
//	ldr	r4, =0x7FFF0000		// Cubieboard2 default translation table
//	mcr	p15, 0, r4, c2, c0, 0	// update TTBR0
//
//	ldr	r4, =0x7FFFFFFF		// The domain 15th is only client.
//	mcr	p15, 0, r4, c3, c0, 0	// update DACR

	LDR		r0, =MMU_PageTable
	// sets up TTBR0
	MCR 	p15, 0, r0, c2, c0, 0				// Write CP15 Translation Table Base Register 0 (TTBR0)
	// TTBCR 
	// if N == 0 then always use TTBR0 [ARMARM B3-118]
	MOV		r0, #0x0
	MCR		p15, 0, r0, c2, c0, 2				// Write CP15 Translation Table Base Control Register (TTBCR)
	LDR		r0, =0x55555555						// All Client
	MCR		p15, 0, r0, c3, c0, 0				// Write CP15 Domain Access Control Register (DACR)

	// exception vector : don't use high vectors
	mrc	p15, 0, r4, c1, c0, 0	// Read cp15 SCTRL Register
	bic	r4, r4, #CR1_V		// Clear V bit becasue we ganna use excep. vector with own defined address.
	orr	r4, r4, #CR1_M
	orr	r4, r4, #CR1_I
	orr	r4, r4, #CR1_C
	orr	r4, r4, #CR1_Z
	mcr	p15, 0, r4, c1, c0, 0	// Write cp15 SCTRL Register
	dsb					/* dsb	allow the MMU to start up */
	isb					/* isb	flush prefetch buffer */

	b _c_start

__cpu_idle_loop:
	WFE
	B		__cpu_idle_loop
#endif
// MILB_SDK Update End
