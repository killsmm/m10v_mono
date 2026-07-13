	.text
	.section .FPT_CODE
	.align 2

	.type Im_FPTAsm_Wait_CpuCyc, %function
	.global	Im_FPTAsm_Wait_CpuCyc
Im_FPTAsm_Wait_CpuCyc:
L1.0:
	SUBS	r0,r0,#1
	BNE		L1.0
	BX		lr

	.end
