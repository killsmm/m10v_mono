/*
 * boot.S: Stubby initial boot stage
 *
 * (C) Copyright 2013 Linaro Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
.extern stubby_end

.extern USER_STACK_AREA
.extern STACK_AREA_IDLE
.extern STACK_AREA_UNDEF
.extern STACK_AREA_ABT
.extern STACK_AREA_SVC
.extern STACK_AREA_FIQ
.extern STACK_AREA_IRQ


.section .initialtext, "x"
//---------------------------------------------------------------
//   Define(s)
//---------------------------------------------------------------
	.equ	CPU0_NUM,			3
	.equ	CPU1_NUM,			1
	.equ	CPU2_NUM,			2
	.equ	CPU3_NUM,			0
	.equ	TIMESTAMP_BASE,		0x1D010000
	.equ	TIMESTAMP_FID0,		0xEE6B2800
	.equ	GICD_CTLR,			0x1D001000
	.equ	WAIT_ADR,			0x4017FFF0
	.equ	WAIT_VAL,			0xD1C3B5A7
	.equ	FIQ_MODE,			0x11
	.equ	IRQ_MODE,			0x12
	.equ	SVC_MODE,			0x13
	.equ	ABT_MODE,			0x17
	.equ	UND_MODE,			0x1B
	.equ	SYS_MODE,			0x1F
	.equ	EXSTOP_BASE,			0x1B110000 			// EXSTOP
	.equ	SD4_UHS1_BASE,			0x1B010000 			// SD4(UHS1 Card)
	.equ	GPIO_BASE,			0x1D022000 			// GPIO


	.equ	GIC_BASE,			0x1D000000 				// GIC-400(Interrupt Interface)
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

	.equ	GPVBASE,				0x19000000
	.equ	GPV_NANDFLASH,			0x01
	.equ	GPV_SD1,				0x02
	.equ	GPV_SD3,				0x04
	.equ	GPV_PCIE,				0x08
	.equ	GPV_USB3,				0x10
	.equ	GPV_SD_RELC_HDMAC_USB2,	0x20
	.equ	GPV_NETSEC,				0x40

	.equ	EXSTOP_SOFTRESET,			(EXSTOP_BASE + 0x000c)		// SOFTREST
	.equ	EXSTOP_SOFTRESET2,			(EXSTOP_BASE + 0x0010)		// SOFTRESET2
	.equ	EXSTOP_RAM_PD,				(EXSTOP_BASE + 0x0040)		// RAM PD Control

	.equ	PUDERPY,				(GPIO_BASE + 0x0330)		// PUDERPY
	.equ	PUDCRPY,				(GPIO_BASE + 0x0430)		// PUDCRPY

	.equ	SD4_UHS1_SOFT_RESET,			(SD4_UHS1_BASE + 0x0200)	// SOFT_REST
    .equ KERNEL_UNBOOT_FLAG,   0x12345678
    .equ TRAPOLINE_ADDRESS,    CONFIG_TRAPOLINE_ADDRESS
    .equ DRAM_MULTI_CORE_CODE_AREA,	(TRAPOLINE_ADDRESS + 4*4)


//---------------------------------------------------------------
//  Macro                                                        
//---------------------------------------------------------------
	.macro	do_cache_op	CRm, lev
                                       // DDI0406C_b_arm_architecture_reference_manual.pdf B2-1286
    mrc     p15, 1, r4, c0, c0, 1      // read clidr
    ands    r3, r4, #0x7000000         // extract LoC from CLIDR(Cache Level ID Register)
    mov     r3, r3, lsr #23            // left align loc bit field
    beq     5f				           // if loc is 0, then no need to clean
    mov     r10, #+\lev                // start clean at cache level 0 (in r10)
1:
    add     r2, r10, r10, LSR #1       // work out 3x current cache level
    mov     r12, r4, LSR r2            // extract cache type bits from clidr
    and     r12, r12, #7               // mask of the bits for current cache only
    cmp     r12, #2                    // see what cache we have at this level
    blt     4f			               // skip if no cache, or just i-cache
    mcr     p15, 2, r10, c0, c0, 0     // select current cache level in cssr
    isb
    mrc     p15, 1, r12, c0, c0, 0     // read the new csidr
    and     r2, r12, #7                // extract the length of the cache lines
    add     r2, r2, #4                 // add 4 (line length offset)
    ldr     r6, =0x3ff
    ands    r6, r6, r12, LSR #3        // find maximum number on the way size
    clz     r5, r6                     // find bit position of way size increment
    mov     r8, r6                     // create working copy of max way size
2:
    ldr     r7, =0x7fff
    ands    r7, r7, r12, LSR #13       // extract max number of the index size
3:
    orr     r11, r10, r8, LSL r5       // factor way and cache number into r11
    orr     r11, r11, r7, LSL r2       // factor index number into r11
    mcr     p15, 0, r11, c7, \CRm, 2   // invalidate/clean/both by set/way
    subs    r7, r7, #1                 // decrement the index
    bge     3b
    subs    r8, r8, #1                 // decrement the way
    bge     2b
4:
// non loop(Level 1 only Or Level 2 only)
//    add     r10, r10, #2               // increment cache number
//    cmp     r3, r10
//    bgt     1b
5:
	dsb
	.endm

	// get_cpuid
	.macro get_cpuid  reg
	mrc		p15, 0, \reg, c0, c0, 5		// Read CP15 Main ID Register (MIDR) into \reg
	and		\reg, \reg, #0xf
	.endm

	// disable_mem
	.macro disable_mem  reg
	mrc		p15, 0, \reg, c1, c0, 0		// Read CP15 System Control Register (SCTLR) into \reg
	bic		\reg, \reg, #(1 << 0)		// Clear MMU enable
	bic		\reg, \reg, #(1 << 2)		// Disable D-cache
	bic		\reg, \reg, #(1 << 11)      // Branch Prediction Disable
	bic		\reg, \reg, #(1 << 12)		// Disable I-cache
	mcr		p15, 0, \reg, c1, c0, 0		// Write value back
	.endm

	.macro init_dist_bank_gic
#ifdef CONFIG_GIC_FIQ
	LDR		r0, = GICD_IGROUPR
	LDR		r1, = 0xFFFFFFFF
	STR		r1, [r0, #0x00]
#endif
	ldr		r0, = GICD_ICENABLER
	ldr		r1, = 0xFFFFFFFF
	str		r1, [r0]

	ldr		r0, = GICD_ICPENDR
	ldr		r1, = 0xFFFFFFFF
	str		r1, [r0]

	ldr		r0, = GICD_IPRIORITYR
	ldr		r1, = 0x00000000
	str		r1, [r0]
	.endm

	.macro init_cpu_bank_gic
	ldr		r0, = GICC_CTLR
	ldr		r1, = 0x00000000
	str		r1, [r0]

	ldr		r0, = GICC_PMR
	ldr		r1, = 0x000000F8
	str		r1, [r0]

	ldr		r0, = GICC_BPR
	ldr		r1, = 0x00000002
	str		r1, [r0]

	ldr		r0, = GICC_CTLR
#ifdef CONFIG_GIC_FIQ
	LDR		r1, = 0x0000001F
#else
	ldr		r1, = 0x00000001
#endif
	str		r1, [r0]
	.endm


//---------------------------------------------------------------
// Reset Entry
//---------------------------------------------------------------
# simulated reset and "bootloader"
# reset vectors
.global reset
reset:
		b	code
		b	undef_abort
		b	code
		b	pref_abort
		b	data_abort
		
	undef_abort:
		mov	r0, #errstring_undef
		b	_abort
	pref_abort:
		mov	r0, #errstring_pref
		b	_abort
	data_abort:
		mov	r0, #errstring_data
		b	_abort

	_abort:
		mov	r12, r14
		mov	r8, pc
		ldr	r2, =0xffff8000
		and	r8, r2
		add	r0, r8
		bl	puts
		
		mov	r0, r12
		bl	print32
		mov	r0, #errstring_2
		add	r0,r8
		bl	puts
		MRC p15, 0, r0, c6, c0, 0
		bl	print32
	spin:
		b	spin
	errstring_undef:
		.ascii	"undef abort, pc=0x\0"
	errstring_pref:
		.ascii	"pref abort, pc=0x\0"
	errstring_data:
		.ascii	"data abort, pc=0x\0"
	errstring_2:
		.ascii	", fault=0x\0"

		.align 2

	code:
#ifdef CONFIG_MULTI_CORE_LINUX
    #ifdef CONFIG_MLB01_BOOT_LINUX_ONLY_BSP
	    mrc		p15, 0, r0, c1, c0, 1				// Read CP15 Auxiliary Control Register (ACTLR)
	    bic		r0, r0, #(3 << 13)					// Data prefetching disable
	    #ifdef CO_PROCESSOR_MODE_AMP
		    bic		r0, #(1 << 6)						// AMP Mode
		    bic		r0, #(1 << 0)						// FW Disable
	    #else
		    orr		r0, #(1 << 6)						// SMP Mode
		    orr		r0, #(1 << 0)						// FW Enable
	    #endif
	    mcr		p15, 0, r0, c1, c0, 1				// Write value back
	    dsb
	    isb


		    /*
		     * disable MMU stuff and caches
		     */
	    disable_mem r0

	    // You must invalidate the instruction cache, the data cache, TLB, and BTAC before using them.[TRM]
	    // Invalidate L1 Caches
	    mov		r0, #0
	    mcr		p15, 0, r0, c8, c7, 0		// TLBIALL - Invalidate entire Unifed TLB
	    mcr		p15, 0, r0, c7, c5, 6		// BPIALL  - Invalidate entire branch predictor array
	    mcr		p15, 0, r0, c7, c5, 0		// ICIALLU - Invalidate all instruction cache to PoU

        do_cache_op c6 0
//--Set timer-stamp------------------------------------------------------
	get_cpuid r1
	cmp		r1, #CPU0_NUM
	beq		__timestamp_set
	bne		__timestamp_end

__timestamp_set:
	ldr		r0, = TIMESTAMP_BASE
	ldr		r1, = TIMESTAMP_FID0
	ldr		r2, = 0x0
	ldr		r3, = 0x1
	str		r2, [r0, #0]				// CNTCR
	str		r2, [r0, #0x8]				// CNTCVL
	str		r2, [r0, #0xC]				// CNTCVU
	str		r1, [r0, #0x20]				// CNTFID0
	str		r3, [r0, #0]				// CNTCR

__timestamp_end:
    //---------------------------------------------------------------
	    get_cpuid r1
	    cmp		r1, #CPU0_NUM
	    beq		__stack_cpu0_set
	    cmp		r1, #CPU1_NUM
	    beq		__stack_cpu1_set
	    cmp		r1, #CPU2_NUM
	    beq		__stack_cpu2_set
	    cmp		r1, #CPU3_NUM
	    beq		__stack_cpu3_set

    __stack_cpu0_set:
	    //cpsid	aif, #SVC_MODE		// SVC Mode
	    ldr		sp, =STACK_AREA_SVC_CPU0
	    b		__stack_end
    __stack_cpu1_set:
	    ldr		sp, =STACK_AREA_SVC_CPU1
	    b		__stack_end
    __stack_cpu2_set:
	    ldr		sp, =STACK_AREA_SVC_CPU2
	    b		__stack_end
    __stack_cpu3_set:
	    ldr		sp, =STACK_AREA_SVC_CPU3
	    b		__stack_end
    __stack_end:

    //---------------------------------------------------------------
	    get_cpuid r1
	    cmp		r1, #CPU0_NUM
	    beq		__gic_set
	    bne		__gic_bank_set

    __gic_set:
	    ldr		r0, = GICD_CTLR
	    ldr		r1, = 0x00000000
	    str		r1, [r0]
#ifdef CONFIG_GIC_FIQ 
    	LDR		r0, = GICD_IGROUPR_SPI
    	LDR		r1, = 0xFFFFFFFF
	    MOV		r2, #GICD_IGROUPR_WCNT
igrp_loop:
    	STR		r1, [r0], #4
    	SUBS	r2, r2, #1
    	BNE		igrp_loop
#endif

	    ldr		r0, = GICD_ICENABLER_SPI
	    ldr		r1, = 0xFFFFFFFF
	    mov		r2, #GICD_ICENABLER_WCNT
    icer_loop:
	    str		r1, [r0], #4
	    subs	r2, r2, #1
	    bne		icer_loop

	    ldr		r0, = GICD_ICPENDR_SPI
	    ldr		r1, = 0xFFFFFFFF
	    mov		r2, #GICD_ICPENDR_WCNT
    icpr_loop:
	    str		r1, [r0], #4
	    subs	r2, r2, #1
	    bne		icpr_loop

	    ldr		r0, = GICD_IPRIORITYR_SPI
	    ldr		r1, = 0x00000000
	    mov		r2, #GICD_IPRIORITYR_WCNT
    ipr_loop:
	    str		r1, [r0], #4
	    subs	r2, r2, #1
	    bne		ipr_loop

	    ldr		r0, = GICD_ITARGETSR_SPI
	    ldr		r1, = 0x00000000
	    mov		r2, #GICD_ITARGETSR_WCNT
    iptr_loop:
	    str		r1, [r0], #4
	    subs	r2, r2, #1
	    bne		iptr_loop

	    ldr		r0, = GICD_ICFGR_SPI
	    ldr		r1, = 0x55555555
	    mov		r2, #GICD_ICFGR_WCNT
    icfr_loop:
	    str		r1, [r0], #4
	    subs	r2, r2, #1
	    bne		icfr_loop

	    init_dist_bank_gic
	    init_cpu_bank_gic
	    ldr		r0, = GICD_CTLR
#ifdef CONFIG_GIC_FIQ
    	LDR		r1, = 0x00000003
#else
	    ldr		r1, = 0x00000001
#endif
	    str		r1, [r0]

	    b		__gic_end

    __gic_bank_set:
	    ldr		r0, = GICD_CTLR
	    ldr		r1, [r0]
#ifdef CONFIG_GIC_FIQ
    	CMP		r1, #0x00000003
#else
	    cmp		r1, #0x00000001
#endif
	    bne		__gic_bank_set
	    init_dist_bank_gic
	    init_cpu_bank_gic

    __gic_end:
    #endif	//CONFIG_MLB01_BOOT_LINUX_ONLY
//---------------------------------------------------------------
//  Set unboot flag
	ldr	r0, = TRAPOLINE_ADDRESS
	ldr	r2, = KERNEL_UNBOOT_FLAG
	str	r2, [r0]
	str	r2, [r0,#4]
	str	r2, [r0,#8]
	str	r2, [r0,#12]
	dsb

// Copy secondary CPU code to other DDR
		ldr	r3, =DRAM_MULTI_CORE_CODE_AREA
		adr	r2, start_wait
        adr r5, end_wait
	copy_secondry_code:
		ldr	r0, [r2]
		str	r0, [r3]
		add	r2, #4
		add	r3, #4

		cmp	r5, r2
		bne	copy_secondry_code

	get_cpuid r1
	cmp	r1, #CPU0_NUM
	bne	__jump_wait_load


//	ldr		r0, = EXSTOP_RAM_PD	/* EXSTOP_RAM_PD Clear */
// 	ldr		r1, = 0x00000000	/* Power down Clear */
// 	str		r1, [r0]
//---------------------------------------------------------------

// EXSTOP Setting
#ifndef CONFIG_RTOS_OWNS_IPS
    ldr		r0, = EXSTOP_SOFTRESET2	/* SOFTRESET Clear */
//	ldr		r1, = 0x00000006	/* bit1:SD3(UHS2), bit2:SD4(UHS1) */
    ldr		r1, = 0x000001FF	/* Clear all */
    str		r1, [r0]

    ldr		r0, = EXSTOP_RAM_PD	/* EXSTOP_RAM_PD Clear */
//	ldr		r1, = 0x000007F9	/* bit1:SD3(UHS2), bit2:SD4(UHS1) */
    ldr		r1, = 0x00000000	/* bit1:SD3(UHS2), bit2:SD4(UHS1) */
    str		r1, [r0]

// SD4(UHS1) SOFT_RESET Clear
    ldr		r0, = SD4_UHS1_SOFT_RESET	/* SOFT_RESET Clear */
    ldr		r1, = 0x00000001	/* bit0:SR_RSTX */
    str		r1, [r0]

// PUDERPY, PUDCRPY Setting
    ldr		r0, = PUDERPY		/* Pullup/down Enable */
    ldr		r1, = 0x00000300	/* bit8:CDX, bit9:WP */
    str		r1, [r0]

    ldr		r0, = PUDCRPY		/* Pullup/down Control */
    ldr		r1, = 0x00000300	/* bit8:CDX, bit9:WP */
    str		r1, [r0]

    ldr		r0, = GPVBASE		/* set GPV */
    ldr		r1, = 0x0000207f	/* bit 0..7,D */
    str		r1, [r0]

#endif // CONFIG_RTOS_OWNS_IPS

	b	__jump_lowlevel_init
//---------------------------------------------------------------
__jump_wait_load:
        ldr	r0, = TRAPOLINE_ADDRESS
      	ldr	r2, = KERNEL_UNBOOT_FLAG
		ldr	r3, =DRAM_MULTI_CORE_CODE_AREA
        bx  r3
start_wait:

	/* wait for Primay/CPU0 to have gone past */
    	pri_catchup:
    	ldr	r3, [r0]
    	cmp	r3, r2
    	bne	pri_catchup

        wfe
	wwfi1:
		ldr	r3, [r0, r1, lsl #2]
    	cmp	r3, r2
    	beq wwfi1
        dmb
        bx  r3
end_wait:

#endif	//CONFIG_MULTI_CORE_LINUX

__jump_lowlevel_init:
	b	lowlevel_init

	.ltorg

	.globl STUBBY_DDR_VARS
STUBBY_DDR_VARS = CONFIG_SYS_SDRAM_BASE + OFFSET_STUBBY_DDR_VARS
