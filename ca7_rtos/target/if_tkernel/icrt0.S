/*
 *----------------------------------------------------------------------
 *    SMP T-Kernel
 *
 *    Copyright (C) 2007-2010 Ken Sakamura. All Rights Reserved.
 *    SMP T-Kernel is distributed under the T-License for SMP T-Kernel.
 *----------------------------------------------------------------------
 *
 *    Version:   1.00.01
 *    Released by T-Engine Forum(http://www.t-engine.org) at 2010/02/19.
 *
 *----------------------------------------------------------------------
 */

/*
 *	icrt0.S (CA7MPCORE)
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
	.section .ICRT0
	.align 4
	.global	_c_start	
	.type _c_start, %function
	
_c_start:
	// distinguish core0 and core1
	mrc	p15, 0, r5, c0, c0, 5
	and	r5, r5, #3
	cmp	r5, #0
	beq	core0_boot
	cmp	r5, #1
	beq	core1_boot
	cmp	r5, #2
	beq	core2_boot

core0_boot:
	// We don't use System mode nor User mode of ARM core.
	ldr	sp, =core0_svc_stack_top

//	ldr	r4, =0			// Clear 'bss' area
//	ldr	r5, =__bss_start
//	ldr	r6, =__bss_end
//	cmp	r5, r6
//	bhs	nobss
//clrbss:
//	str	r4, [r5], #4
//	cmp	r5, r6
//	blo	clrbss
//	dmb
//nobss:

	ldr	fp, =core0_svc_stack_top

	// Initialize SYS mode
	mrs	r5, cpsr
	orr	r6, r5, #PSR_SYS
	orr	r6, r6, #PSR_I|PSR_F|PSR_A
	msr	cpsr_cx, r6
	ldr	sp, =core0_sys_stack_top

	// Again, we are in Supervisor mode(SVC)
	msr	cpsr_cx, r5

	dmb

enter_cmain:
	blx	Csym(pre_start_kernel)
l_end:				// Not suppose to return from 'main'
	b	l_end		// but, just in case, prepare for out of control

core1_boot:
	ldr	sp, =core1_svc_stack_top
	ldr	fp, =core1_svc_stack_top

	// Initializing core1's SYS mode
	mrs	r5, cpsr
	orr	r6, r5, #PSR_SYS
	orr	r6, r6, #PSR_I|PSR_F|PSR_A
	msr	cpsr_cx, r6
	ldr	sp, =core1_sys_stack_top

	// Back to SVC mode
	msr	cpsr_cx, r5

	blx	Csym(pre_start_kernel)
r_end1:					// Not suppose to return from 'main'
	b	r_end1			// but, just in case, prepare for out of control

core2_boot:
	ldr	sp, =core2_svc_stack_top
	ldr	fp, =core2_svc_stack_top

	// Initializing core1's SYS mode
	mrs	r5, cpsr
	orr	r6, r5, #PSR_SYS
	orr	r6, r6, #PSR_I|PSR_F|PSR_A
	msr	cpsr_cx, r6
	ldr	sp, =core2_sys_stack_top

	// Back to SVC mode
	msr	cpsr_cx, r5

	blx	Csym(pre_start_kernel)
r_end2:					// Not suppose to return from 'main'
	b	r_end2			// but, just in case, prepare for out of control

	.align	4			// for making assembler happy

	.bss
	.align	4
//0x200
	.space	0x4000
core2_svc_stack_top:

	.align	4
	.space	0x4000
core2_sys_stack_top:

	.space	0x4000
core1_svc_stack_top:

	.align	4
	.space	0x4000
core1_sys_stack_top:
	
	.align	4
	.space	0x4000
core0_svc_stack_top:

	.align	4
	.space	0x4000
core0_sys_stack_top:


