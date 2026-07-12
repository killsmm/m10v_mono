/*
 * code for switching cores into non-secure state and into HYP mode
 *
 * Copyright (c) 2013	Andre Przywara <andre.przywara@linaro.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

 #ifdef CONFIG_NONSECURE_ACCESS

#define __ASSEMBLY__

#include<asm/gic.h>
#include<asm/nonsec.h>

#define F_BIT		0x40
#define I_BIT		0x80
#define A_BIT		0x100
#define T_BIT		0x20
#define SVC_MODE 0x13

#define LENTRY(name) \
	.align 0; \
	name:

#define ENTRY(name) \
	.globl name; \
	LENTRY(name)

#define END(name) \
	.size name, .-name

#define ENDPROC(name) \
	.type name STT_FUNC; \
	END(name)

.arch_extension sec
.arch_extension virt

	.pushsection ._secure.text, "ax"

	.align	5
/* the vector table for secure state and HYP mode */
_monitor_vectors:
	.word 0	/* reset */
	.word 0 /* undef */
	adr pc, _secure_monitor
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0

.macro is_cpu_virt_capable	tmp
	mrc	p15, 0, \tmp, c0, c1, 1		@ read ID_PFR1
	and	\tmp, \tmp, #CPUID_ARM_VIRT_MASK	@ mask virtualization bits
	cmp	\tmp, #(1 << CPUID_ARM_VIRT_SHIFT)
.endm

secondary_entry:
	.word 0
.global secure_boot
secure_boot:
	.word 0xffffffff

/*
 * secure monitor handler
 * U-boot calls this "software interrupt" in start.S
 * This is executed on a "smc" instruction, we use a "smc #0" to switch
 * to non-secure state.
 * r0, r1, r2: passed to the callee
 * ip: target PC
 */
_secure_monitor:
	/* check r0 for purpose */
	add sp, pc, #0x7500
	push {r4-r8}
	
	teq r0, #CMD_ENTER_NONSEC_MODE
	beq enter_nonsec
	
	teq r0, #CMD_SET_SECONDARY_ENTRY
	beq set_secondary_ads

	/* return -EINVAL for bad parameter */
	ldr r0, =-22
	b leave_monitor_mode

enter_nonsec:
	mov lr, r4
	mrc	p15, 0, r5, c1, c1, 0		@ read SCR
	bic	r5, r5, #0x4a			@ clear IRQ, EA, nET bits
	orr	r5, r5, #0x31			@ enable NS, AW, FW bits
						@ FIQ preserved for secure mode
	mov	r6, #SVC_MODE			@ default mode is SVC
	is_cpu_virt_capable r4
#ifdef CONFIG_ARMV7_VIRT
	orreq	r5, r5, #0x100			@ allow HVC instruction
	moveq	r6, #HYP_MODE			@ Enter the kernel as HYP
#endif

	mcr	p15, 0, r5, c1, c1, 0		@ write SCR (with NS bit set)
	isb

	bne	ret_caller

	@ Reset CNTVOFF to 0 before leaving monitor mode
	mrc	p15, 0, r4, c0, c1, 1		@ read ID_PFR1
	ands	r4, r4, #CPUID_ARM_GENTIMER_MASK	@ test arch timer bits
	movne	r4, #0
	mcrrne	p15, 4, r4, r4, c14		@ Reset CNTVOFF to zero
	
ret_caller:
	mov	ip, #(F_BIT | I_BIT | A_BIT)	@ Set A, I and F
	tst	lr, #1				@ Check for Thumb PC
	orrne	ip, ip, #T_BIT			@ Set T if Thumb
	orr	ip, ip, r6			@ Slot target mode in
	msr	spsr_cxfs, ip			@ Set full SPSR
	mov	r0, r1
	mov	r1, r2
	mov	r2, r3

leave_monitor_mode:
	pop {r4-r8}
	movs pc, lr

set_secondary_ads:
	ldr r4, =secondary_entry
	mov r8, pc
	ldr r7, =0xffff8000
	and r8, r7
	add r4, r8
	str r1, [r4, #0]
	ldr r0, =0
	b leave_monitor_mode

ENTRY(_do_nonsec_entry)
	mov	r4, r0
	ldr r0, =0
	smc	#0
ENDPROC(_do_nonsec_entry)

.macro get_cbar_addr	addr
#ifdef CONFIG_ARM_GIC_BASE_ADDRESS
	ldr	\addr, =CONFIG_ARM_GIC_BASE_ADDRESS
#else
	mrc	p15, 4, \addr, c15, c0, 0	@ read CBAR
	bfc	\addr, #0, #15			@ clear reserved bits
#endif
.endm

.macro get_gicd_addr	addr
	get_cbar_addr	\addr
	add	\addr, \addr, #GIC_DIST_OFFSET	@ GIC dist i/f offset
.endm

.macro get_gicc_addr	addr, tmp
	get_cbar_addr	\addr
	is_cpu_virt_capable \tmp
	movne	\tmp, #GIC_CPU_OFFSET_A9	@ GIC CPU offset for A9
	moveq	\tmp, #GIC_CPU_OFFSET_A15	@ GIC CPU offset for A15/A7
	add	\addr, \addr, \tmp
.endm

/*
 * Switch a core to non-secure state.
 *
 *  1. initialize the GIC per-core interface
 *  2. allow coprocessor access in non-secure modes
 *
 * Called from smp_pen by secondary cores and directly by the BSP.
 * Do not assume that the stack is available and only use registers
 * r0-r3 and r12.
 *
 * PERIPHBASE is used to get the GIC address. This could be 40 bits long,
 * though, but we check this in C before calling this function.
 */
ENTRY(_nonsec_init)

	get_gicd_addr	r3

	mvn	r1, #0				@ all bits to 1
	str	r1, [r3, #GICD_IGROUPRn]	@ allow private interrupts

	get_gicc_addr	r3, r1

	mov	r1, #3				@ Enable both groups
	str	r1, [r3, #GICC_CTLR]		@ and clear all other bits
	mov	r1, #0xff
	str	r1, [r3, #GICC_PMR]		@ set priority mask register

	mrc	p15, 0, r0, c1, c1, 2
	movw	r1, #0x3fff
	movt	r1, #0x0004
	orr	r0, r0, r1
	mcr	p15, 0, r0, c1, c1, 2		@ NSACR = all copros to non-sec

/* The CNTFRQ register of the generic timer needs to be
 * programmed in secure state. Some primary bootloaders / firmware
 * omit this, so if the frequency is provided in the configuration,
 * we do this here instead.
 * But first check if we have the generic timer.
 */
#ifdef CONFIG_SYS_CLK_FREQ
	mrc	p15, 0, r0, c0, c1, 1		@ read ID_PFR1
	and	r0, r0, #CPUID_ARM_GENTIMER_MASK	@ mask arch timer bits
	cmp	r0, #(1 << CPUID_ARM_GENTIMER_SHIFT)
	ldreq	r1, =CONFIG_SYS_CLK_FREQ
	mcreq	p15, 0, r1, c14, c0, 0		@ write CNTFRQ
#endif

	adr	r1, _monitor_vectors
	mcr	p15, 0, r1, c12, c0, 1		@ set MVBAR to secure vectors
	isb

#if defined(CONFIG_MB86S70) || defined(CONFIG_MB86S73)
	/* enable nonsecure access for CCI */
	ldr	r0, =(CONFIG_CCI_SECURE_ACCESS_SETVALUE)
	ldr r1, =(CONFIG_CCI_SECURE_ACCESS_REGISTER)
	str	r0, [r1]

	/* enable nonsecure access for system MMU */
	ldr	r0, =(CONFIG_SMMU_SSDR0_SETVALUE)

	ldr	r1, =(CONFIG_SMMU_SSDR0_CSS_GPU_0)
	str	r0, [r1]

	ldr	r1, =(CONFIG_SMMU_SSDR0_CSS_DMA330)
	str	r0, [r1]

#if defined(CONFIG_MB86S73)
	ldr	r1, =(CONFIG_SMMU_SSDR0_HSIO_USB30_1)
	str	r0, [r1]

	ldr	r1, =(CONFIG_SMMU_SSDR0_HSIO_USB20_1)
	str	r0, [r1]
#endif

	ldr	r1, =(CONFIG_SMMU_SSDR0_HSIO_PCIE_0)
	str	r0, [r1]

	ldr	r1, =(CONFIG_SMMU_SSDR0_HSIO_PCIE_1)
	str	r0, [r1]

	ldr	r1, =(CONFIG_SMMU_SSDR0_HSIO_USB30_0)
	str	r0, [r1]

	ldr	r1, =(CONFIG_SMMU_SSDR0_HSIO_USB20_0)
	str	r0, [r1]

	ldr	r1, =(CONFIG_SMMU_SSDR0_MPB)
	str	r0, [r1]
#endif

	mov	r0, r3				@ return GICC address
	bx	lr
ENDPROC(_nonsec_init)


ENTRY(_enter_secondary_entry)
	bl _get_secondary_entry
	ldr 	r1, =0x0
	b _do_nonsec_entry
ENDPROC(_enter_secondary_entry)

ENTRY(_get_secondary_entry)
	ldr r1, =secondary_entry
	mov r8, pc
	ldr r2, =0xffff8000
	and r8, r2
	add r1, r8
	ldr r0, [r1]
	bx lr
ENDPROC(_get_secondary_entry)
	.popsection

#endif
