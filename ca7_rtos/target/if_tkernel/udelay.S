
/*
 * r0  <= 2000
 * lpj <= 0x01ffffff (max. 3355 bogomips)
 * HZ  <= 1000
*/
	.section .text, "ax"
	.align	4	
	.global	udelay
	.type udelay, %function
udelay:
	ldr	r2, =LC1
	ldr	r2, [r2]
	mul	r0, r2, r0
//ENTRY(__const_udelay)			// @ 0 <= r0 <= 0x7fffff06
	mov	r1, #-1
	ldr	r2, =LC0
	ldr	r2, [r2]		// @ max = 0x01ffffff 
	add	r0, r0, r1, lsr #32-14
	mov	r0, r0, lsr #14		// @ max = 0x0001ffff
	add	r2, r2, r1, lsr #32-10
	mov	r2, r2, lsr #10		// @ max = 0x00007fff
	mul	r0, r2, r0		// @ max = 2^32-1
	add	r0, r0, r1, lsr #32-6
	movs	r0, r0, lsr #6
	bxeq	lr

/*
 * loops = r0 * HZ * loops_per_jiffy / 1000000
 *
 * Oh, if only we had a cycle counter...
 */
	.global	delay
	.type delay, %function
	// Delay routine
delay:
	subs	r0, r0, #1
	bhi	delay
	bx	lr

LC0:
	.word	(1 << 12)
LC1:
	.word	((2199023*100)>>11)

