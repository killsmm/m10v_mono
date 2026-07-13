/**
 * @file		cacheops-v7.s
 * @brief		Cache operation
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//---------------------------------------------------------------
//  Condition Assenble Control                                   
//---------------------------------------------------------------
	.text
	.section .CACHE_OPERATION_V7
    .align 2

//---------------------------------------------------------------
//  Macro                                                        
//---------------------------------------------------------------
	.macro	do_cache_op	CRm, lev
                                       // DDI0406C_b_arm_architecture_reference_manual.pdf B2-1286
    MRC     p15, 1, r4, c0, c0, 1      // read clidr
    ANDS    r3, r4, #0x7000000         // extract LoC from CLIDR(Cache Level ID Register)
    MOV     r3, r3, lsr #23            // left align loc bit field
    BEQ     5f				           // if loc is 0, then no need to clean
    MOV     r10, #+\lev                // start clean at cache level 0 (in r10)
1:
    ADD     r2, r10, r10, LSR #1       // work out 3x current cache level
    MOV     r12, r4, LSR r2            // extract cache type bits from clidr
    AND     r12, r12, #7               // mask of the bits for current cache only
    CMP     r12, #2                    // see what cache we have at this level
    BLT     4f			               // skip if no cache, or just i-cache
    MCR     p15, 2, r10, c0, c0, 0     // select current cache level in cssr
    ISB
    MRC     p15, 1, r12, c0, c0, 0     // read the new csidr
    AND     r2, r12, #7                // extract the length of the cache lines
    ADD     r2, r2, #4                 // add 4 (line length offset)
    LDR     r6, =0x3ff
    ANDS    r6, r6, r12, LSR #3        // find maximum number on the way size
    CLZ     r5, r6                     // find bit position of way size increment
    MOV     r8, r6                     // create working copy of max way size
2:
    LDR     r7, =0x7fff
    ANDS    r7, r7, r12, LSR #13       // extract max number of the index size
3:
    ORR     r11, r10, r8, LSL r5       // factor way and cache number into r11
    ORR     r11, r11, r7, LSL r2       // factor index number into r11
    MCR     p15, 0, r11, c7, \CRm, 2   // invalidate/clean/both by set/way
    SUBS    r7, r7, #1                 // decrement the index
    BGE     3b
    SUBS    r8, r8, #1                 // decrement the way
    BGE     2b
4:
// non loop(Level 1 only Or Level 2 only)
//    ADD     r10, r10, #2               // increment cache number
//    CMP     r3, r10
//    BGT     1b
5:
	DSB
	.endm

//---------------------------------------------------------------
//  Global Function                                              
//---------------------------------------------------------------
    .global	invalidate_dcache
invalidate_dcache:
    STMFD    sp!, {r0-r12,lr}
    do_cache_op c6 0
    LDMFD    sp!, {r0-r12,pc}

    .global	clean_dcache
clean_dcache:
    STMFD    sp!, {r0-r12,lr}
    do_cache_op c10 0
    LDMFD    sp!, {r0-r12,pc}

    .global	clean_invalidate_dcache
clean_invalidate_dcache:
    STMFD    sp!, {r0-r12,lr}
    do_cache_op c14 0
    LDMFD    sp!, {r0-r12,pc}

    .global	invalidate_dcache_l2
invalidate_dcache_l2:
    STMFD    sp!, {r0-r12,lr}
    do_cache_op c6 2
    LDMFD    sp!, {r0-r12,pc}

    .global	clean_dcache_l2
clean_dcache_l2:
    STMFD    sp!, {r0-r12,lr}
    do_cache_op c10 2
    LDMFD    sp!, {r0-r12,pc}

    .global	clean_invalidate_dcache_l2
clean_invalidate_dcache_l2:
    STMFD    sp!, {r0-r12,lr}
    do_cache_op c14  2
    LDMFD    sp!, {r0-r12,pc}

    .end
