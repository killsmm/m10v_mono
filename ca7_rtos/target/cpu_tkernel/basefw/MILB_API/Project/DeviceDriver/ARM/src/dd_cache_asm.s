/**
 * @file		dd_cache_asm.s
 * @brief		Cache operation driver
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//---------------------------------------------------------------*
//  Condition Assenble Control                                   *
//---------------------------------------------------------------*
	.text
	.section .DD_CACHE_ASM
	.align 2

//---------------------------------------------------------------*
//  Include Files                                                *
//---------------------------------------------------------------*
	.extern Dd_ARM_EI
	.extern Dd_ARM_DI
	.extern DDIM_User_Spin_Lock
	.extern DDIM_User_Spin_Unlock

//---------------------------------------------------------------*
//  Definition External Valiable                                 *
//---------------------------------------------------------------*
// Nothing Special

//---------------------------------------------------------------*
//  Definition Constant                                          *
//---------------------------------------------------------------*
// Nothing Special

//---------------------------------------------------------------*
//  Macro                                                        *
//---------------------------------------------------------------*
//-------------------------------------------
// D-cache Clean Flush by Set/Way
//-------------------------------------------
	.macro	CleanFlush_SetWay CRm, lev
                                       // DDI0406C_b_arm_architecture_reference_manual.pdf B2-1286
    MRC     p15, 1, r4, c0, c0, 1      // read clidr
    ANDS    r3, r4, #0x7000000         // extract LoC from CLIDR(Cache Level ID Register)
    MOV     r3, r3, lsr #23            // left align loc bit field
    BEQ     5f				           // if loc is 0, then no need to clean
    MOV     r10, #+\lev                // start clean at cache level 0 (in r10)
1:
    ADD     r2, r10, r10, lsr #1       // work out 3x current cache level
    MOV     r12, r4, lsr r2            // extract cache type bits from clidr
    AND     r12, r12, #7               // mask of the bits for current cache only
    CMP     r12, #2                    // see what cache we have at this level
    BLT     4f			               // skip if no cache, or just i-cache
    MRC     p15, 2, r3, c0, c0, 0      // cache level in cssr
    MCR     p15, 2, r10, c0, c0, 0     // select current cache level in cssr
    ISB
    MRC     p15, 1, r12, c0, c0, 0     // read the new csidr
    AND     r2, r12, #7                // extract the length of the cache lines
    ADD     r2, r2, #4                 // add 4 (line length offset)
    LDR     r6, =0x3ff
    ANDS    r6, r6, r12, lsr #3        // find maximum number on the way size
    CLZ     r5, r6                     // find bit position of way size increment
    MOV     r8, r6                     // create working copy of max way size
2:
    LDR     r7, =0x7fff
    ANDS    r7, r7, r12, lsr #13       // extract max number of the index size
3:
    ORR     r11, r10, r8, LSL r5       // factor way and cache number into r11
    ORR     r11, r11, r7, LSL r2       // factor index number into r11
    MCR     p15, 0, r11, c7, \CRm, 2   // invalidate/clean/both by set/way
    SUBS    r7, r7, #1                 // decrement the index
    BGE     3b
    SUBS    r8, r8, #1                 // decrement the way
    BGE     2b
	MCR     p15, 2, r3, c0, c0, 0      // select current cache level in cssr
4:
// non loop(Level 1 only Or Level 2 only)
//    ADD     r10, r10, #2               // increment cache number
//    CMP     r3, r10
//    BGT     1b
5:
	DSB
	.endm

//---------------------------------------------------------------*
//  Local Function                                               *
//---------------------------------------------------------------*
// Nothing Special

//---------------------------------------------------------------*
//  Global Function                                              *
//---------------------------------------------------------------*
//-------------------------------------------
// Level 1 D-cache Flush Addr Size
//-------------------------------------------
	.type Dd_CACHE_L1_Flush_Dcache_Addr_Asm, %function
	.global Dd_CACHE_L1_Flush_Dcache_Addr_Asm
Dd_CACHE_L1_Flush_Dcache_Addr_Asm:
									// Argument
									// r0 : Address
									// r1 : Size
	PUSH	{r4-r6,lr}
	
	MOV		r4, r0					// Copy r0 to r4
	MOV		r5, r1					// Copy r1 to r5

	ADD		r5, r5, #0x3F
	MOV		r5, r5, LSR #6			// bytes to cashe line

LINE_LOOP1:
	MCR		p15, 0, r4, c7, c6, 1	// flush D-cache line
	ADD		r4, r4, #1 << 6			// +1 next D-cache line address
	SUBS	r5, r5, #1				// -1 D-cache line counter
	BNE		LINE_LOOP1

	DSB

	POP		{r4-r6,pc}

//-------------------------------------------
// Level 1 D-cache Clean Addr Size
//-------------------------------------------
	.type Dd_CACHE_L1_Clean_Dcache_Addr_Asm, %function
	.global Dd_CACHE_L1_Clean_Dcache_Addr_Asm
Dd_CACHE_L1_Clean_Dcache_Addr_Asm:
									// Argument
									// r0 : Address
									// r1 : Size
	PUSH	{r4-r6,lr}

	MOV		r4, r0					// Copy r0 to r4
	MOV		r5, r1					// Copy r1 to r5

	ADD		r5, r5, #0x3F
	MOV		r5, r5, LSR #6			// bytes to cashe line

LINE_LOOP2:
	MCR		p15, 0, r4, c7, c10, 1	// clean D-cache line
	ADD		r4, r4, #1 << 6			// +1 next D-cache line address
	SUBS	r5, r5, #1				// -1 D-cache line counter
	BNE		LINE_LOOP2

	DSB

	POP		{r4-r6,pc}

//-------------------------------------------
// Level 1 D-cache Clean Flush Addr Size
//-------------------------------------------
	.type Dd_CACHE_L1_Clean_Flush_Dcache_Addr_Asm, %function
	.global Dd_CACHE_L1_Clean_Flush_Dcache_Addr_Asm
Dd_CACHE_L1_Clean_Flush_Dcache_Addr_Asm:
									// Argument
									// r0 : Address
									// r1 : Size
	PUSH	{r4-r6,lr}
	
	MOV		r4, r0					// Copy r0 to r4
	MOV		r5, r1					// Copy r1 to r5

	ADD		r5, r5, #0x3F
	MOV		r5, r5, LSR #6			// bytes to cashe line

LINE_LOOP3:
	MCR		p15, 0, r4, c7, c14, 1	// clean and flush D-cache line
	ADD		r4, r4, #1 << 6 		// +1 next D-cache line address
	SUBS	r5, r5, #1 				// -1 D-cache line counter
	BNE		LINE_LOOP3

	DSB

	POP		{r4-r6,pc}

//-------------------------------------------
// Level 2 Dcache Flush Addr Size
//-------------------------------------------
	.type Dd_CACHE_L2_Flush_Dcache_Addr_Asm,%function
	.global Dd_CACHE_L2_Flush_Dcache_Addr_Asm
Dd_CACHE_L2_Flush_Dcache_Addr_Asm:
									// Argument
									// r0 : Address
									// r1 : Size
	PUSH	{r4-r6,lr}
	
	MOV		r4, r0					// Copy r0 to r4
	MOV		r5, r1					// Copy r1 to r5
	
	ADD		r5, r5, #0x3F
	MOV		r5, r5, LSR #6			// bytes to cashe line

LINE_LOOP4:
	MCR		p15, 0, r4, c7, c6, 1	// flush D-cache line
	ADD		r4, r4, #1 << 6 		// +1 next D-cache line address
	SUBS	r5, r5, #1 				// -1 D-cache line counter
	BNE		LINE_LOOP4

	DSB

	POP		{r4-r6,pc}

//-------------------------------------------
// Level 2 Dcache Clean Addr Size
//-------------------------------------------
	.type Dd_CACHE_L2_Clean_Dcache_Addr_Asm,%function
	.global Dd_CACHE_L2_Clean_Dcache_Addr_Asm
Dd_CACHE_L2_Clean_Dcache_Addr_Asm:
									// Argument
									// r0 : Address
									// r1 : Size
	PUSH	{r4-r6,lr}
	
	MOV		r4, r0					// Copy r0 to r4
	MOV		r5, r1					// Copy r1 to r5

	ADD		r5, r5, #0x3F
	MOV		r5, r5, LSR #6			// bytes to cashe line

LINE_LOOP5:
	MCR		p15, 0, r4, c7, c10, 1	// clean D-cache line
	ADD		r4, r4, #1 << 6 		// +1 next D-cache line address
	SUBS	r5, r5, #1 				// -1 D-cache line counter
	BNE		LINE_LOOP5

	DSB

	POP		{r4-r6,pc}

//-------------------------------------------
// Level 2 Dcache Clean Flush Addr Size
//-------------------------------------------
	.type Dd_CACHE_L2_Clean_Flush_Dcache_Addr_Asm,%function
	.global Dd_CACHE_L2_Clean_Flush_Dcache_Addr_Asm
Dd_CACHE_L2_Clean_Flush_Dcache_Addr_Asm:
									// Argument
									// r0 : Address
									// r1 : Size
	PUSH	{r4-r6,lr}
	
	MOV		r4, r0					// Copy r0 to r4
	MOV		r5, r1					// Copy r1 to r5

	ADD		r5, r5, #0x3F
	MOV		r5, r5, LSR #6			// bytes to cashe line

LINE_LOOP6:
	MCR		p15, 0, r4, c7, c14, 1	// clean and flush D-cache line
	ADD		r4, r4, #1 << 6 		// +1 next D-cache line address
	SUBS	r5, r5, #1 				// -1 D-cache line counter
	BNE		LINE_LOOP6

	DSB

	POP		{r4-r6,pc}

//-------------------------------------------
// Level 1 Dcache Flush All
//-------------------------------------------
	.type Dd_CACHE_L1_Flush_Dcache_All,%function
	.global Dd_CACHE_L1_Flush_Dcache_All
Dd_CACHE_L1_Flush_Dcache_All:
	PUSH	{r4-r12,lr}

	LDR		r1, =Dd_ARM_DI			// Disable IRQ
	BLX     r1
	MOV		r9, r0					// Copy r0 to r6 (Copy SR Value)

	CleanFlush_SetWay c6 0			// flush D-cache by set/way

	MOV		r0, r9					// Copy r6 to r0 (Set SR Value)
	LDR		r1, =Dd_ARM_EI			// Enable IRQ
	BLX     r1

	POP		{r4-r12,pc}

//-------------------------------------------
// Level 1 Dcache Clean All
//-------------------------------------------
	.type Dd_CACHE_L1_Clean_Dcache_All,%function
	.global Dd_CACHE_L1_Clean_Dcache_All
Dd_CACHE_L1_Clean_Dcache_All:
	PUSH	{r4-r12,lr}

	LDR		r1, =Dd_ARM_DI			// Disable IRQ
	BLX     r1
	MOV		r9, r0					// Copy r0 to r6 (Copy SR Value)

	CleanFlush_SetWay c10 0			// clean D-cache by set/way

	MOV		r0, r9					// Copy r6 to r0 (Set SR Value)
	LDR		r1, =Dd_ARM_EI			// Enable IRQ
	BLX     r1

	POP		{r4-r12,pc}

//-------------------------------------------
// Level 1 Dcache Clean Flush All
//-------------------------------------------
	.type Dd_CACHE_L1_Clean_Flush_Dcache_All,%function
	.global Dd_CACHE_L1_Clean_Flush_Dcache_All
Dd_CACHE_L1_Clean_Flush_Dcache_All:
	PUSH	{r4-r12,lr}

	LDR		r1, =Dd_ARM_DI			// Disable IRQ
	BLX     r1
	MOV		r9, r0					// Copy r0 to r6 (Copy SR Value)

	CleanFlush_SetWay c14 0			// clean and flush D-cache by set/way

	MOV		r0, r9					// Copy r6 to r0 (Set SR Value)
	LDR		r1, =Dd_ARM_EI			// Enable IRQ
	BLX     r1

	POP		{r4-r12,pc}

//-------------------------------------------
// Level 2 Dcache Flush All
//-------------------------------------------
	.type Dd_CACHE_L2_Flush_Dcache_All,%function
	.global Dd_CACHE_L2_Flush_Dcache_All
Dd_CACHE_L2_Flush_Dcache_All:
	PUSH	{r4-r12,lr}

	CleanFlush_SetWay c6 2			// flush D-cache by set/way

	POP		{r4-r12,pc}

//-------------------------------------------
// Level 2 Dcache Clean All
//-------------------------------------------
	.type Dd_CACHE_L2_Clean_Dcache_All,%function
	.global Dd_CACHE_L2_Clean_Dcache_All
Dd_CACHE_L2_Clean_Dcache_All:
	PUSH	{r4-r12,lr}

	CleanFlush_SetWay c10 2			// clean D-cache by set/way

	POP		{r4-r12,pc}

//-------------------------------------------
// Level 2 Dcache Clean Flush All
//-------------------------------------------
	.type Dd_CACHE_L2_Clean_Flush_Dcache_All,%function
	.global Dd_CACHE_L2_Clean_Flush_Dcache_All
Dd_CACHE_L2_Clean_Flush_Dcache_All:
	PUSH	{r4-r12,lr}

	CleanFlush_SetWay c14 2			// clean and flush D-cache by set/way

	POP		{r4-r12,pc}

	.end
