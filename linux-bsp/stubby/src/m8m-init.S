/*
 * this the special init needed for m8m start
 * in addition to the normal Stubby ones
 *
 * secondary cpu start is also handled here
 */

	.section ".initialtext", #alloc, #execinstr
	.align

/*
 * the region from m8m_start_vectors .. m8m_start_end gets copied into SRAM
 * which gets mirrored to 0 on the primary path
 */

m8m_start_vectors:

	b	m8m_start
	.align
	.word	0
	.word	0
	.word	0

	.align
trampoline:
	.word	0	/* 
			 * in the SRAM copy, this will be the trampoline
			 * his PA will be 0x01000010
			 */

/*
 * cold boot enters here
 */

m8m_start:
	.global m8m_start
	.type	m8m_start,#function

#ifndef CONFIG_KILL_SMP
	/*
	 * note none of this code uses any stack and should stay that way
	 */

	mrc    p15, 0, r0, c1, c0, 0
	bic    r0, r0, #(1 << 0)         // Clear MMU enable
	bic    r0, r0, #(1 << 2)         // Disable D-cache
	bic    r0, r0, #(1 << 12)        // Disable I-cache
	mcr    p15, 0, r0, c1, c0, 0

	/* Invalidate L1 Caches */

	mov	r0, #0
	mcr     p15, 0, r0, c8, c7, 0     // TLBIALL - Invalidate entire Unifed TLB
	mcr     p15, 0, r0, c7, c5, 6     // BPIALL - Invalidate all branch predictor
	mcr	p15, 0, r0, c7, c5, 0     // ICIALLU - Invalidate all ICACHE to PoU

	/* configure CPU */

	mrc     p15, 0, r0, c1, c0, 1
	bic     r0, r0, #(3 << 13)  // ERRATA No.745469
#ifdef CO_PROCESSOR_MODE_AMP
	bic     r0, #(1 << 6) //  AMP Mode
	bic     r0, #(1 << 0) //  FW Disable
#else
	orr     r0, #(1 << 6) //  SMP Mode
	orr     r0, #(1 << 0) //  FW Enable
#endif
	mcr     p15, 0, r0, c1, c0, 1 //  write Aux Control Register
	dsb
	isb

	/* invalidate dcache */

        mrc     p15, 1, r4, c0, c0, 1      // read clidr
        ands    r3, r4, #0x7000000         // extract LoC from CLIDR
        mov     r3, r3, LSR #23            // left align loc bit field
        beq     5f                         // if loc is 0, then no need to clean
        mov     r10, #0                    // start clean at cache level 0 (in r10)
1:
        add     r2, r10, r10, LSR #1       // work out 3x current cache level
        mov     r12, r4, LSR r2            // extract cache type bits from clidr
        and     r12, r12, #7               // mask of the bits for current cache only
        cmp     r12, #2                    // see what cache we have at this level
        blt     4f                         // skip if no cache, or just i-cache
        mcr     p15, 2, r10, c0, c0, 0     // select current cache level in cssr
        isb                                // #1
        mrc     p15, 1, r12, c0, c0, 0     // read the new csidr
        and     r2, r12, #7                // extract the length of the cache lines
        add     r2, r2, #4                 // add 4 (line length offset)
        ldr     r6, =0x3ff
        ands    r6, r6, r12, LSR #3        // find maximum number on the way size
        clz	r5, r6                     // find bit position of way size increment
        ldr	r7, =0x7fff
        ands	r7, r7, r12, LSR #13       // extract max number of the index size
2:
        mov     r8, r6                     // create working copy of max way size
3:
        orr     r11, r10, r8, LSL r5       // factor way and cache number into r11
        orr     r11, r11, r7, LSL r2       // factor index number into r11
        mcr     p15, 0, r11, c7, c6, 2   // invalidate/clean/both by set/way
        subs    r8, r8, #1                 // decrement the way
        bge     3b
        subs    r7, r7, #1                 // decrement the index
        bge     2b
4:
        add     r10, r10, #2               // increment cache number
        cmp     r3, r10
        bgt     1b
5:
        dsb

#endif
m8m_start_end:

	/* split based on which core we are */

	mrc 	p15, 0, r1, c0, c0, 5       // Get our cpu id
	and    r1, r1, #0xf
	cmp    r1, #PRIMARY_CPU
	bne    secondary_cold_start

	/* primary core cold start path */

        mvn    r0, #0
        ldr    r1, =SCU_BASE
        str    r0, [r1, #0xc]              // Invalidate all SCU TAG RAMs

        ldr    r0, [r1, #0x0]              // Read SCU Control Register
        orr    r0, r0, #(1 << 6)           // IC standby enable
        orr    r0, r0, #(1 << 5)           // SCU Dynamic clock gating enable
#ifdef CO_PROCESSOR_MODE_AMP
        bic    r0, r0, #0x1                // Set Disable SCU bit
#else
        orr    r0, r0, #0x1                // Set Enable SCU bit
#endif
        str    r0, [r1, #0x0]              // Update SCU Control Regi

	/* primary copies some one-time init to SRAM, it can be trashed afterwards */

	adr	r1, cpu0sram_start
	adr	r2, cpu0sram_end
	ldr	r3, =SRAM_CODE0
cpu0sram_copy:
	ldr	r0, [r1]
	str	r0, [r3]
	add	r1, r1, #4
	add	r3, r3, #4
	cmp	r1, r2
	bls	cpu0sram_copy
	dsb

	/* before trashing DDR (we are running there) cpu0 should spin until cpu1 in WFI */
#ifndef CONFIG_KILL_SMP

	ldr	r0, =MCSWFI_PHY
wwfi1:
	ldr	r1, [r0, #0]
	ands	r1, r1, #1 << (PRIMARY_CPU ^ 1) // CPU1 WFI
	beq	wwfi1
#endif
	/* ...then cpu0 can do the DDR mapping business from SRAM */

	ldr	r3, =SRAM_CODE0
	blx	r3

	b	lowlevel_init_2

	/* 
	 * Some kind of ddr layout must be done from sram
	 * so this is copied to SRAM and executed by the primary from there
	 */

	.align

cpu0sram_start:
#ifndef CONFIG_RTOS_OWNS_IPS
	/* IMG::MXIC module reset. */
	adr    r2, cpu0sram_MXIC_TMIRST
	ldr    r2, [r2]
	mov    r0, #1
	str    r0, [r2]
	dsb
#endif
	/* "address filtering" */
	adr     r1, cpu0sram_L2C_310
	ldr     r1, [r1]
	mov     r0, #0xC0000000
	str     r0, [r1, #0xC04]
	mov     r0, #0x30000000
	orr     r0, r0, #1		/* filtering enable */
	str     r0, [r1, #0xC00]
	dsb

#ifndef CONFIG_RTOS_OWNS_IPS

	/* IMG::MXIC module reset release  */
	mov    r0, #0
	str    r0, [r2]
	dsb

	/* IMG::MXIS DRAM region configuration */
	adr    r2, cpu0sram_MXIC_TREGION
	ldr    r2, [r2]

	/*
	 * Slave1　TSTAD:4000_0000h TSIZE:0x2000_0000h(=512M)  (default設定)
	 * Slave2　TSTAD:6000_0000h TSIZE:0x4000_0000h(=1G)    (TSIZE変更)
	 * Slave3　TSTAD:A000_0000h TSIZE:0x2000_0000h(=512M)  (TSTAD変更)
	 */
	mov    r0, #0x20000000
	str    r0, [r2, #0x00] /* TSIZE_1 */
	str    r0, [r2, #0x10] /* TSIZE_3 */
	mov    r0, #0x40000000
	str    r0, [r2, #0x08] /* TSIZE_2 */
	str    r0, [r2, #0x04] /* TSTAD_1 */
	mov    r0, #0x60000000
	str    r0, [r2, #0x0C] /* TSTAD_2 */
	mov    r0, #0xA0000000
	str    r0, [r2, #0x14] /* TSTAD_3 */

	adr    r2, cpu0sram_MXIC_TSMTRG
	ldr    r2, [r2]
	mov    r0, #1
	str    r0, [r2]
#endif
	mov	pc, lr

cpu0sram_L2C_310:
	.word	L2C_310_BASE
cpu0sram_MXIC_TMIRST:
	.word	IOADR_MXIC_TMIRST
cpu0sram_MXIC_TSMTRG:
	.word	IOADR_MXIC_TSMTRG
cpu0sram_MXIC_TREGION:
	.word	IOADR_MXIC_TREGION

cpu0sram_end:

	.ltorg
	.align

/*
 * trampoline original
 */

reloc_start:

	/* let this core see the interrupts */

	mov	r0, #(CONFIG_ARM_GIC_BASE_ADDRESS & 0xff000000)
	orr	r0, #(CONFIG_ARM_GIC_BASE_ADDRESS & 0xff0000)
	orr	r0, #(CONFIG_ARM_GIC_BASE_ADDRESS & 0xff00)
	orr	r0, #(CONFIG_ARM_GIC_BASE_ADDRESS & 0xff)
        mov     r1, #0xff
        str     r1, [r0, #4] @GICC_PMR
        mov     r1, #0x3
        str     r1, [r0] @GICC_CTRL	
	dsb
	isb

	wfi

	/* Check the entry point actually got set */
	mov	r1, pc
	mov	r2, #0xff000000
	orr	r2, #0x00ff0000
	and	r1, r2
	add	r1, #trampoline - m8m_start_vectors
	ldr	r3, [r1]
	cmp	r3, #0
	beq	reloc_start

	/* go to the kernel's entry point that was set in the trampoline */
	mov	pc, r3

	.ltorg

written:
	.word	0xcafebabe
reloc_end:


/*
 * Every secondary cold starts through here
 * Warm-started secondaries go through the SRAM instead
 * which this code prepares
 */

secondary_cold_start:

	/* 
	 * ensure the SRAM trampoline is prepared
	 */
	ldr	r2, =0xcafebabe
	ldr	r0, =SRAM_CODE1
	add	r0, #(written - reloc_start) + (m8m_start_end - m8m_start_vectors)
	ldr	r1, [r0]
	cmp	r1, r2
	beq	secondary_jump_in_sram

	/*
	 * Prepare the SRAM at 0x01000000
	 *
	 * First copy a vector table + the init from the top of this file
	 * then concatenate reloc_start..reloc_end
	 *
	 * every non-cold reset on every core will go into this SRAM boot flow
	 */
	
	adr	r1, m8m_start_vectors
	adr	r2, m8m_start_end
	ldr	r3, =SRAM_BASE
reloc_copy:
	ldr	r0, [r1]
	str	r0, [r3]
	add	r1, r1, #4
	add	r3, r3, #4
	cmp	r1, r2
	bls	reloc_copy

	adr	r1, reloc_start
	adr	r2, reloc_end
reloc_copy1:
	ldr	r0, [r1]
	str	r0, [r3]
	add	r1, r1, #4
	add	r3, r3, #4
	cmp	r1, r2
	bls	reloc_copy1
	dsb

secondary_jump_in_sram:

	/* and take refuge in the SRAM
	 * this Stubby code in DDR is going away
	 */

	ldr	r0, =SRAM_BASE
	/* we can skip the init we already did */
	add	r0, #m8m_start_end - m8m_start_vectors
	bx	r0

	.ltorg

