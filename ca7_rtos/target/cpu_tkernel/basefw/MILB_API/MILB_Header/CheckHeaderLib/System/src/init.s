;---------------------------------------------------------------
;   Condition Assenble Control
;---------------------------------------------------------------
	PRESERVE8
	AREA	STARTUP_CODE, CODE, ALIGN = 4, READONLY
	
	ENTRY
	
	EXPORT	_Reset_Handler
_Reset_Handler

;; Total Stack Size is 0xC000
ABT_STKSIZE	EQU	0x1000
UND_STKSIZE	EQU	0x1000
SVC_STKSIZE	EQU	0x1000
FIQ_STKSIZE	EQU	0x1000
IRQ_STKSIZE	EQU	0x4000
SYS_STKSIZE	EQU	0x4000

Mode_USR	EQU	0x10
Mode_FIQ	EQU	0x11
Mode_IRQ	EQU	0x12
Mode_SVC	EQU	0x13
Mode_ABT	EQU	0x17
Mode_UNDEF	EQU	0x1B
Mode_SYS	EQU	0x1F

F_Bit		EQU		0x40
I_Bit		EQU		0x80

	IMPORT	||Image$$STACK_AREA$$Base||
	
	LDR	r0, =||Image$$STACK_AREA$$Base||
	
	ADD	r0, r0, #ABT_STKSIZE
	MSR	cpsr_c, #Mode_ABT:OR:I_Bit:OR:F_Bit		; ABT Mode
	MOV	sp, r0						; Stack Set
	ADD	r0, r0, #UND_STKSIZE
	MSR	cpsr_c, #Mode_UNDEF:OR:I_Bit:OR:F_Bit	; UNDEF Mode
	MOV	sp, r0						; Stack Set
	ADD	r0, r0, #SVC_STKSIZE
	MSR	cpsr_c, #Mode_SVC:OR:I_Bit:OR:F_Bit		; SVC Mode
	MOV	sp, r0						; Stack Set
	ADD	r0, r0, #FIQ_STKSIZE
	MSR	cpsr_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit		; FIQ Mode
	MOV	sp, r0						; Stack Set
	ADD	r0, r0, #IRQ_STKSIZE
	MSR	cpsr_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit		; IRQ Mode
	MOV	sp, r0						; Stack Set
	ADD	r0, r0, #SYS_STKSIZE
	MSR	cpsr_c, #Mode_SYS:OR:I_Bit:OR:F_Bit		; SYS Mode
	MOV	sp, r0						; Stack Set

	;;----------------------------------------------------------------------
	;; RO/RW Area Copy
	;; Initialize ZI Area (Use ARM Library)
	;;----------------------------------------------------------------------
	IMPORT	__main
	B	__main
	
	END

