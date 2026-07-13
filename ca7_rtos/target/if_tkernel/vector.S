/*
 *	icrt0.S (CA7MPCORE)
 *	System Startup
 */
	
#define	_in_asm_source_

#include "porting.h"

/*
 *************************************************************************
 *
 * EIT
 *
 *************************************************************************
*/
// MILB_SDK Update Start
//	.text
	.section .INIT_VECTOR_TBL, "ax"
// MILB_SDK Update End
	.global vector_base
	.arm

// MILB_SDK Add Start
	.extern	gProfile_Address0
	.extern	gProfile_Address1
	.extern	gProfile_Address2
	.extern	gProfile_End_Address0
	.extern	gProfile_End_Address1
	.extern	gProfile_End_Address2
	.extern	BF_L1cache_All
	.equ	TMR_TMCSR4,	(0x1E000040)
	.equ	TMR_TMCSR11,	(0x1E0000B0)
	.equ	TMR_TMCSR12,	(0x1E0000C0)
	.equ	GIC_IAR,	(0x1D00200C)
	.equ	GIC_EOIR,	(0x1D002010)
	.equ	INTID_MASK,	0x3FF
// MILB_SDK Add End

vector_base:
	b	_Reset_Handler
	b	_undefined_instruction_handler
	b	svc_handler
	b	_prefetch_abort_handler
	b	_data_abort_handler
	nop	/* reserved */
	b	irq_handler
// MILB_SDK Update Start
//	b	fiq
	PUSH		{r0-r7}
	LDR		r10, =GIC_IAR
	LDR		r11, [r10, #0x0]
	PUSH		{r11}
	LDR		r12, =INTID_MASK
	AND		r11, r11, r12
	CMP		r11, #0xf
	BEQ		cache
	BNE		mprofile

cache:
	PUSH		{r12, lr}
	LDR		r12, =BF_L1cache_All
	BLX		r12
	POP		{r12, lr}
	B		gic_end

mprofile:
	MRC		p15, 0, r8, c0, c0, 5
	AND		r8, r8, #0xf
	CMP	    r8, #1
	BEQ		set_cpu1
	CMP	    r8, #2
	BEQ		set_cpu2
set_cpu0:
	LDR		r8, =gProfile_Address0
	MOV		r11,r8
	LDR		r8, [r8, #0x0]
	LDR		r9, =gProfile_End_Address0
	LDR		r9, [r9, #0x0]
	CMP		r8, r9
	SUBNE	r10, lr, #4
	STRNE	r10, [r8]				// update pc-history-buffer
	ADDNE	r8, r8, #4				// countup SDRAM address
	STRNE	r8, [r11, #0x0]
	DMB
	B		tmr_cpu0
set_cpu1:
	LDR		r8, =gProfile_Address1
	MOV		r11,r8
	LDR		r8, [r8, #0x0]
	LDR		r9, =gProfile_End_Address1
	LDR		r9, [r9, #0x0]
	CMP		r8, r9
	SUBNE	r10, lr, #4
	STRNE	r10, [r8]				// update pc-history-buffer
	ADDNE	r8, r8, #4				// countup SDRAM address
	STRNE	r8, [r11, #0x0]
	DMB
	B		tmr_cpu1
set_cpu2:
	LDR		r8, =gProfile_Address2
	MOV		r11,r8
	LDR		r8, [r8, #0x0]
	LDR		r9, =gProfile_End_Address2
	LDR		r9, [r9, #0x0]
	CMP		r8, r9
	SUBNE	r10, lr, #4
	STRNE	r10, [r8]				// update pc-history-buffer
	ADDNE	r8, r8, #4				// countup SDRAM address
	STRNE	r8, [r11, #0x0]
	DMB
	B		tmr_cpu2

unuse_cpu_trap_for_ice:				// for T-Kernel only ICE debug.
	CPSID	ifa
unuse_cpu_loop_for_ice:
	WFE
	B		unuse_cpu_loop_for_ice

tmr_cpu0:
	LDR		r10, =TMR_TMCSR4
	B		tmr_clr
tmr_cpu1:
	LDR		r10, =TMR_TMCSR11
	B		tmr_clr
tmr_cpu2:
	LDR		r10, =TMR_TMCSR12
	B		tmr_clr

tmr_clr:
	LDR		r11, [r10, #0x0]		// read TMR32[4 or 11 or 12] TMCSR
	AND		r11, r11, #0xFFFFFFBB	// update TMR32[4 or 11 or 12] TMCSR
	STR		r11, [r10, #0x0]
	BIC		r11, r11, #(1 << 2)		// update TMR32[4 or 11 or 12] TMCSR
	STR		r11, [r10, #0x0]
	DMB

gic_end:
	POP		{r11}
	LDR		r10, =GIC_EOIR
	STR		r11, [r10, #0x0]		// write GIC_EOIR register
	DMB

fiq_end:
	POP		{r0-r7}
	SUBS	pc, lr, #4
// MILB_SDK Update End

/*
 *************************************************************************
 *
 * Interrupt handling
 *
 *************************************************************************
 */

_undefined_instruction_handler:
	PUSH	{r0-r1,lr}
	MOV		r0, #0x04
	LDR		r1, =_Exception_Handler
	BLX		r1
	POP		{r0-r1,lr}
	B		undefined_instruction_handler

_prefetch_abort_handler:
	PUSH	{r0-r1,lr}
	MOV		r0, #0x0C
	LDR		r1, =_Exception_Handler
	BLX		r1
	POP		{r0-r1,lr}
	B		prefetch_abort_handler

_data_abort_handler:
	PUSH	{r0-r1,lr}
	MOV		r0, #0x10
	LDR		r1, =_Exception_Handler
	BLX		r1
	POP		{r0-r1,lr}
	B		data_abort_handler

_Exception_Handler:
	MRS		r1, cpsr
	PUSH	{r0-r12,lr}
	MRS		r0, spsr
	PUSH	{r0,r1}
	MRC		p15, 0, r0, c5, c0, 0			// Read DFSR (Data Fault Status Register)
	MRC		p15, 0, r1, c5, c0, 1			// Read IFSR (Instruction Fault Status Register)
	MRC		p15, 0, r2, c6, c0, 0			// Read DFAR (Data Fault Address Register)
	MRC		p15, 0, r3, c6, c0, 2			// Read IFAR (Instruction Fault Address Register)
	PUSH	{r0-r3}
	MOV		r0, sp
	CPSID	if, #0x13						// SVC Mode
	NOP
	MOV		r1, sp
	MOV		sp, r0
	PUSH	{r1,lr}
	MOV		r0, sp
	MOV		sp, r1
	CPSID	if, #0x17						// ABT Mode
	NOP
	MOV		sp, r0
	
	.extern	gException_Cnt
	LDR		r12, =gException_Cnt
_Exception_Handler_wait_loop:
	LDR		r4, [r12]
	SUBS	R4, #1
	BNE		_Exception_Handler_wait_loop
	STR		r4, [r12]
	
	.extern	Exception_Handler
	PUSH	{lr}
	LDR		r12, =Exception_Handler
	BLX		r12
	POP		{lr}
	POP		{r1,lr}
	POP		{r0-r3}
	POP		{r0,r1}
	POP		{r0-r12,lr}
	BX		lr
