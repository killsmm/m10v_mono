/**
 * @file		pmu.s
 * @brief		PMU (Performance Monitoring Unit)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//---------------------------------------------------------------
//   Condition Assenble Control
//---------------------------------------------------------------
	.text
	.section .PMU_CODE
    .align 2

//---------------------------------------------------------------
//  Note
//---------------------------------------------------------------
// Please refer to following document.
// 	DDI0464F_cortex_a7_mpcore_r0p5_trm.pdf
// 	DDI0406C_b_arm_architecture_reference_manual.pdf
// 	- Chapter 12 Performance Monitoring Unit

//---------------------------------------------------------------
//  Definition External Valiable(s)
//---------------------------------------------------------------

//---------------------------------------------------------------
//  Function(s)
//---------------------------------------------------------------
//-------------------------------------------
// PMU_Init
//-------------------------------------------
	.type	BF_PMU_Init, %function
	.global BF_PMU_Init
BF_PMU_Init:
											// initialize PMU

	LDR		R2, =0x80000000					// [31]:Cycle counter enable
	MCR		p15, 0, R2, c9, c12, 1			// write Count Enable Set Register (PMCNTENSET)
	
//	MCR		p15, 0, R2, c9, c12, 3			// write Overflow Flag Status Register (PMSOVSR)
	
	BX		LR								// return to caller


//-------------------------------------------
// BF_PMU_Ctrl_Cycle_Enable
//-------------------------------------------
	.type	BF_PMU_Ctrl_Cycle_Enable, %function
	.global BF_PMU_Ctrl_Cycle_Enable
BF_PMU_Ctrl_Cycle_Enable:
											// enable Cycle Counter

	MRC		p15, 0, R2, c9, c12, 1			// read Count Enable Set Register (PMCNTENSET)
	LDR		R1, =0x00000001					// 1:Counter enable
	ORR		R2, R2, R1, LSL #31				// add [31]:Cycle Counter
	MCR		p15, 0, R2, c9, c12, 1			// write Count Enable Set Register (PMCNTENSET)
	
//	MCR		p15, 0, R2, c9, c12, 3			// write Overflow Flag Status Register (PMSOVSR)
	
	BX		LR								// return to caller


//-------------------------------------------
// BF_PMU_Ctrl_Cycle_Disable
//-------------------------------------------
	.type	BF_PMU_Ctrl_Cycle_Disable, %function
	.global BF_PMU_Ctrl_Cycle_Disable
BF_PMU_Ctrl_Cycle_Disable:
											// disable Cycle Counter

	LDR		R1, =0x00000001					// 1:Counter enable clear
	LSL		R2, R1, #31						// [31]:Cycle Counter
	MCR		p15, 0, R2, c9, c12, 2			// write Count Enable Clear Register (PMCNTENCLR)
	
//	MRC		p15, 0, R2, c9, c12, 3			// read Overflow Flag Status Register (PMSOVSR)
//	BIC		R2, R2, #(1 << 31)				// [31]:Cycle Counter disable
//	MCR		p15, 0, R2, c9, c12, 3			// write Overflow Flag Status Register (PMSOVSR)
	
	BX		LR								// return to caller


//-------------------------------------------
// BF_PMU_Ctrl_Event_Set
//-------------------------------------------
	.type	BF_PMU_Ctrl_Event_Set, %function
	.global BF_PMU_Ctrl_Event_Set
BF_PMU_Ctrl_Event_Set:
											// set Event Counter
											// 	R0:counter_num
											// 	(0:Counter #0, 1:Counter #1, 2:Counter #2, 3:Counter #3)
											// 	R1:event_num

	AND		R0, R0, #0x1F					// [4:0]:Counter select
                                            // (b00000=Counter #0, b00001=Counter #1, b00010=Counter #2, b00011=Counter #3)
	MCR		p15, 0, R0, c9, c12, 5			// write Event Counter Selection Register (PMSELR)
	
	LDR		R2, =0x00000001					// 1:Counter enable
	LSL		R3, R2, R0						// [3]:Counter #3, [2]:Counter #2, [1]:Counter #1, [0]:Counter #0
	MRC		p15, 0, R2, c9, c12, 1			// read Count Enable Set Register (PMCNTENSET)
	ORR		R2, R2, R3						// add [3]:Counter #3 or [2]:Counter #2 or [1]:Counter #1 or [0]:Counter #0
	MCR		p15, 0, R2, c9, c12, 1			// write Count Enable Set Register (PMCNTENSET)
	
//	MCR		p15, 0, R2, c9, c12, 3			// write Overflow Flag Status Register (PMSOVSR)
	
	AND		R1, R1, #0xFF					// [7:0]:Event number
	MCR		p15, 0, R1, c9, c13, 1			// write Event Type Select Register (PMXEVTYPER)
	
	BX		LR								// return to caller


//-------------------------------------------
// BF_PMU_Ctrl_Event_Disable
//-------------------------------------------
	.type	BF_PMU_Ctrl_Event_Disable, %function
	.global BF_PMU_Ctrl_Event_Disable
BF_PMU_Ctrl_Event_Disable:
											// disable Event Counter
											// 	R0:counter_num
											// 	(0:Counter #0, 1:Counter #1, 2:Counter #2, 3:Counter #3)

	LDR		R1, =0x00000001					// 1:Counter enable clear
	LSL		R2, R1, R0						// [3]:Counter #3, [2]:Counter #2, [1]:Counter #1, [0]:Counter #0
	MCR		p15, 0, R2, c9, c12, 2			// write Count Enable Clear Register (PMCNTENCLR)
	
//	MRC		p15, 0, R2, c9, c12, 3			// read Overflow Flag Status Register (PMSOVSR)
//	BIC		R2, R2, (#1 << R0)				// [1]:Counter #1, [0]:Counter #0
//	MCR		p15, 0, R2, c9, c12, 3			// write Overflow Flag Status Register (PMSOVSR)
	
	BX		LR								// return to caller


//-------------------------------------------
// PMU_Start
//-------------------------------------------
	.type	BF_PMU_Start, %function
	.global BF_PMU_Start
BF_PMU_Start:
											// start PMU
											// 	R0:reset_counter (0:No Action, 1:Evnet Counters, 2:Cycle Counter, 3:All Counters)
											// 	R1:cycle_divider (0:Count Every Clock Cycle, 1:Count Every 64th Clock Cycle)

	LSL		R2, R1, #3						// [3]:Cycle count divider
	ORR		R2, R2, R0, LSL #1				// [2]:Cycle counter reset, [1]:Event counter reset
	ORR		R2, R2, #1						// [0]:Enable all counters
	MCR		p15, 0, R2, c9, c12, 0			// write Performance Monitor Control Register (PMCR)
	
	BX		LR								// return to caller


//-------------------------------------------
// PMU_Stop
//-------------------------------------------
	.type	BF_PMU_Stop, %function
	.global BF_PMU_Stop
BF_PMU_Stop:
											// stop PMU

	MRC		p15, 0, R0, c9, c12, 0			// read Performance Monitor Control Register (PMCR)
	AND		R0, R0, #0xFFFFFFFE				// [31:1]:mask
	MCR		p15, 0, R0, c9, c12, 0			// write Performance Monitor Control Register (PMCR)
	
	BX		LR								// return to caller


//-------------------------------------------
// PMU_Get_Cycle_Count
//-------------------------------------------
	.type	BF_PMU_Get_Cycle_Count, %function
	.global BF_PMU_Get_Cycle_Count
BF_PMU_Get_Cycle_Count:
											// return Cycle Counter Value

	MRC		p15, 0, R0, c9, c13, 0			// read Cycle Count Register (PMCCNTR)
	
	BX		LR								// return to caller


//-------------------------------------------
// PMU_Get_Event_Count
//-------------------------------------------
	.type	BF_PMU_Get_Event_Count, %function
	.global BF_PMU_Get_Event_Count
BF_PMU_Get_Event_Count:
											// return Event Counter Value
											// 	R0:counter_num
											// 	(0:Counter #0, 1:Counter #1, 2:Counter #2, 3:Counter #3)

	AND		R0, R0, #0x1F					// [4:0]:Counter select
	MCR		p15, 0, R0, c9, c12, 5			// write Event Counter Selection Register (PMSELR)
	
	MRC		p15, 0, R0, c9, c13, 2			// read Event Count Registers (PMXEVCNTR)
	
	BX		LR								// return to caller


//-------------------------------------------
// PMU_Get_Config
//-------------------------------------------
	.type	BF_PMU_Get_Event_Number, %function
	.global BF_PMU_Get_Event_Number
BF_PMU_Get_Event_Number:
											// return Event Number
											// 	R0:counter_num
											// 	(0:Counter #0, 1:Counter #1, 2:Counter #2, 3:Counter #3)

	AND		R0, R0, #0x1F					// [4:0]:Counter select
	MCR		p15, 0, R0, c9, c12, 5			// write Event Counter Selection Register (PMSELR)
	
	MRC		p15, 0, R0, c9, c13, 1			// read Event Type Select Register (PMXEVTYPER)
	
	BX		LR								// return to caller

	.end
