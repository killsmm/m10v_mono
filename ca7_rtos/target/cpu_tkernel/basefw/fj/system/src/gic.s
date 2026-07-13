/**
 * @file		gic.s
 * @brief		gic initialize and setting
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//---------------------------------------------------------------
//   Condition Assenble Control
//---------------------------------------------------------------
	.text
	.section .INIT_GIC
    .align 2

//---------------------------------------------------------------
//   Define(s)
//---------------------------------------------------------------
	.equ	GIC_BASE,				0x1D000000 					// GIC-400(Interrupt Interface)
	.equ	GIC_DIST_BASE,			(GIC_BASE + 0x1000)			// GIC-400(Distributor)
	.equ	GIC_CPU_BASE,			(GIC_BASE + 0x2000)			// GIC-400(CPU Interface)

	.equ	GICD_CTLR,				(GIC_DIST_BASE + 0x000)		// Distributor Control Register
	.equ	GICD_TYPER,				(GIC_DIST_BASE + 0x004)		// Interrupt Controller Type Register
	.equ	GICD_IIDR,				(GIC_DIST_BASE + 0x008)		// Distributor Implementer Identification Register
	.equ	GICD_IGROUPR,			(GIC_DIST_BASE + 0x080)		// Interrupt Group Registers
	.equ	GICD_ISENABLER,			(GIC_DIST_BASE + 0x100)		// Interrupt Set-Enable Registers
	.equ	GICD_ICENABLER,			(GIC_DIST_BASE + 0x180)		// Interrupt Clear-Enable Registers
	.equ	GICD_ISPENDR,			(GIC_DIST_BASE + 0x200)		// Interrupt Set-Pending Registers
	.equ	GICD_ICPENDR,			(GIC_DIST_BASE + 0x280)		// Interrupt Clear-Pending Registers
	.equ	GICD_ISACTIVER,			(GIC_DIST_BASE + 0x300)		// Interrupt Set-Active Registers
	.equ	GICD_ICACTIVER,			(GIC_DIST_BASE + 0x380)		// Interrupt Clear-Active Registers
	.equ	GICD_IPRIORITYR,		(GIC_DIST_BASE + 0x400)		// Interrupt Priority Registers
	.equ	GICD_ITARGETSR,			(GIC_DIST_BASE + 0x800)		// Interrupt Processor Targets Registers
	.equ	GICD_ICFGR,				(GIC_DIST_BASE + 0xC00)		// Interrupt Configuration Registers
	.equ	GICD_PPISR,				(GIC_DIST_BASE + 0xD00)		// Private Peripheral Interrupt Status Register
	.equ	GICD_SPISR,				(GIC_DIST_BASE + 0xD04)		// Shared Peripheral Interrupt Status Registers
	.equ	GICD_SGIR,				(GIC_DIST_BASE + 0xF00)		// Software Generated Interrupt Register
	.equ	GICD_CPENDSGIR,			(GIC_DIST_BASE + 0xF10)		// SGI Clear-Pending Registers
	.equ	GICD_SPENDSGIR,			(GIC_DIST_BASE + 0xF20)		// SGI Set-Pending Registers

	.equ	GICC_CTLR,				(GIC_CPU_BASE + 0x000)		// CPU Interface Control Register
	.equ	GICC_PMR,				(GIC_CPU_BASE + 0x004)		// Interrupt Priority Mask Register
	.equ	GICC_BPR,				(GIC_CPU_BASE + 0x008)		// Binary Point Register
	.equ	GICC_IAR,				(GIC_CPU_BASE + 0x00C)		// Interrupt Acknowledge Register
	.equ	GICC_EOIR,				(GIC_CPU_BASE + 0x010)		// End of Interrupt Register
	.equ	GICC_RPR,				(GIC_CPU_BASE + 0x014)		// Running Priority Register
	.equ	GICC_HPPIR,				(GIC_CPU_BASE + 0x018)		// Highest Priority Pending Interrupt Register
	.equ	GICC_ABPR,				(GIC_CPU_BASE + 0x01C)		// Aliased Binary Point Register
	.equ	GICC_AIAR,				(GIC_CPU_BASE + 0x020)		// Aliased Interrupt Acknowledge Register
	.equ	GICC_AEOIR,				(GIC_CPU_BASE + 0x024)		// Aliased End of Interrupt Register
	.equ	GICC_AHPPIR,			(GIC_CPU_BASE + 0x028)		// Aliased Highest Priority Pending Interrupt Register
	.equ	GICC_APR0,				(GIC_CPU_BASE + 0x0D0)		// Active Priority Register
	.equ	GICC_NSAPR0,			(GIC_CPU_BASE + 0x0E0)		// Non-Secure Active Priority Register
	.equ	GICC_IIDR,				(GIC_CPU_BASE + 0x0FC)		// CPU Interface Identification Register
	.equ	GICC_DIR,				(GIC_CPU_BASE + 0x1000)		// Deactivate Interrupt Register

	.equ	GICD_IGROUPR_SPI,		GICD_IGROUPR + 0x4
	.equ	GICD_IGROUPR_WCNT,		9							// (320/32)-(32/32) : (MAXINTID/32)-((SGI+PPI)/32)
	.equ	GICD_ISENABLER_SPI,		GICD_ISENABLER + 0x4
	.equ	GICD_ISENABLER_WCNT,	9							// (320/32)-(32/32) : (MAXINTID/32)-((SGI+PPI)/32)
	.equ	GICD_ICENABLER_SPI,		GICD_ICENABLER + 0x4
	.equ	GICD_ICENABLER_WCNT,	9							// (320/32)-(32/32) : (MAXINTID/32)-((SGI+PPI)/32)
	.equ	GICD_ICPENDR_SPI,		GICD_ICPENDR + 0x4
	.equ	GICD_ICPENDR_WCNT,		9							// (320/32)-(32/32) : (MAXINTID/32)-((SGI+PPI)/32)
	.equ	GICD_IPRIORITYR_SPI,	GICD_IPRIORITYR + 0x20
	.equ	GICD_IPRIORITYR_WCNT,	72							// (320/4)-(32/4) : (MAXINTID/4)-((SGI+PPI)/4)
	.equ	GICD_ITARGETSR_SPI,		GICD_ITARGETSR + 0x20
	.equ	GICD_ITARGETSR_WCNT,	72							// (320/4)-(32/4) : (MAXINTID/4)-((SGI+PPI)/4)
	.equ	GICD_ICFGR_SPI,			GICD_ICFGR + 0x8
	.equ	GICD_ICFGR_WCNT,		18							// (320/16)-(32/16) : (MAXINTID/16)-((SGI+PPI)/16)

//---------------------------------------------------------------
// INIT_GIC
//---------------------------------------------------------------
	.type	initialize_gic, %function
	.global initialize_gic
initialize_gic:
	LDR		r0, = GICD_CTLR
	LDR		r1, = 0x00000000
	STR		r1, [r0]

	LDR		r0, = GICD_IGROUPR_SPI
	LDR		r1, = 0xFFFFFFFF
	MOV		r2, #GICD_IGROUPR_WCNT
igrp_loop:
	STR		r1, [r0], #4
	SUBS	r2, r2, #1
	BNE		igrp_loop

	LDR		r0, = GICD_ICENABLER_SPI
	LDR		r1, = 0xFFFFFFFF
	MOV		r2, #GICD_ICENABLER_WCNT
icer_loop:
	STR		r1, [r0], #4
	SUBS	r2, r2, #1
	BNE		icer_loop

	LDR		r0, = GICD_ICPENDR_SPI
	LDR		r1, = 0xFFFFFFFF
	MOV		r2, #GICD_ICPENDR_WCNT
icpr_loop:
	STR		r1, [r0], #4
	SUBS	r2, r2, #1
	BNE		icpr_loop

	LDR		r0, = GICD_IPRIORITYR_SPI
	LDR		r1, = 0x00000000
	MOV		r2, #GICD_IPRIORITYR_WCNT
ipr_loop:
	STR		r1, [r0], #4
	SUBS	r2, r2, #1
	BNE		ipr_loop

	LDR		r0, = GICD_ITARGETSR_SPI
	LDR		r1, = 0x00000000
	MOV		r2, #GICD_ITARGETSR_WCNT
iptr_loop:
	STR		r1, [r0], #4
	SUBS	r2, r2, #1
	BNE		iptr_loop

	LDR		r0, = GICD_ICFGR_SPI
	LDR		r1, = 0x55555555
	MOV		r2, #GICD_ICFGR_WCNT
icfr_loop:
	STR		r1, [r0], #4
	SUBS	r2, r2, #1
	BNE		icfr_loop

	BLX		lr


//---------------------------------------------------------------
// ENABLE_GIC
//---------------------------------------------------------------
	.type	enable_gic, %function
	.global enable_gic
enable_gic:
	LDR		r0, = GICD_CTLR
	LDR		r1, = 0x00000003
	STR		r1, [r0]

	BLX		lr


//---------------------------------------------------------------
// INIT_DIST_BANK_GIC
//---------------------------------------------------------------
	.type	init_dist_bank_gic, %function
	.global init_dist_bank_gic
init_dist_bank_gic:
	LDR		r0, = GICD_IGROUPR
//	LDR		r1, = 0xFFFFFFFF
	LDR		r1, = 0xFFFF7FFF
	STR		r1, [r0, #0x00]

	LDR		r0, = GICD_ICENABLER
	LDR		r1, = 0xFFFFFFFF
	STR		r1, [r0]

	LDR		r0, = GICD_ICPENDR
	LDR		r1, = 0xFFFFFFFF
	STR		r1, [r0]

	LDR		r0, = GICD_IPRIORITYR
	LDR		r1, = 0x00000000
	STR		r1, [r0]

	BLX		lr


//---------------------------------------------------------------
// INIT_CPU_BANK_GIC
//---------------------------------------------------------------
	.type	init_cpu_bank_gic, %function
	.global init_cpu_bank_gic
init_cpu_bank_gic:
	LDR		r0, = GICC_CTLR
	LDR		r1, = 0x00000000
	STR		r1, [r0]

	LDR		r0, = GICC_PMR
	LDR		r1, = 0x000000F8
	STR		r1, [r0]

	LDR		r0, = GICC_BPR
	LDR		r1, = 0x00000002
	STR		r1, [r0]

	LDR		r0, = GICC_CTLR
	LDR		r1, = 0x0000001F
	STR		r1, [r0]

	BLX		lr

	.end
