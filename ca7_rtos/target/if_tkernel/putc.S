/*
 *
 *
 */

// UART0_BASE : 0x01C28000
#define UART0_BASE	0x01C28000

// uarts are 5. each uart has 0x400 bytes for the area
#define UART_OFFSET	0x400

// receive buffer register
#define UART_RBR(n)	(UART0_BASE + (n) * UART_OFFSET + 0x0)
// transmit holding register
#define UART_THR(n)	(UART0_BASE + (n) * UART_OFFSET + 0x0)
// divisor latch low register
#define UART_DLL(n)	(UART0_BASE + (n) * UART_OFFSET + 0x0)

// divisor latch high register
#define UART_DLH(n)	(UART0_BASE + (n) * UART_OFFSET + 0x4)
// interrupt enable register
#define UART_IER(n)	(UART0_BASE + (n) * UART_OFFSET + 0x4)

// interrupt identity register
#define UART_IIR(n)	(UART0_BASE + (n) * UART_OFFSET + 0x8)
// fifo control register
#define UART_FCR(n)	(UART0_BASE + (n) * UART_OFFSET + 0x8)

// line control register
#define UART_LCR(n)	(UART0_BASE + (n) * UART_OFFSET + 0xC)
#define UART_LCR_DLAB	(0x1 << 7)

// line status register
#define UART_LSR(n)	(UART0_BASE + (n) * UART_OFFSET + 0x14)
#define UART_LSR_TEMT	(0x1 << 6)

#define BAUD_115200	(0xd)	// 24 * 1000 * 1000 / 16 / 115200 = 13
#define	NO_PARITY	(0)
#define ONE_STOP_BIT	(0)
#define DAT_LEN_8_BITS	(3)
#define	LC_8_N_1	(NO_PARITY << 3 | ONE_STOP_BIT << 2 | DAT_LEN_8_BITS)

#define TX_READY (*((volatile uint32_t*)UART_LSR(0)) & UART_LSR_TEMT)

//
// This has done by uboot.
//
//void uart_init(void) {
//
//	/* select dll dlh */
//	writel(UART_LCR_DLAB, UART_LCR(UART));
//	/* set baudrate */
//	writel(0, UART_DLH(UART));
//	writel(BAUD_115200, UART_DLL(UART));
//	/* set line control */
//	writel(LC_8_N_1, UART_LCR(UART));
//
//	uart_initialized = 1;
//}

#define CORE0	1
#if CORE0 == 1
#define CORE	bxne
#else
#define CORE	bxeq
#endif

	.align 	4
	.bss
	.comm	uart_putex, 4
	.comm	core1buf, 4096
	.comm	core1w, 4
	.comm	core1r, 4
	.comm 	putsex, 4
	.comm	putxex, 4

	.align	4
	.text
	.global uart_puts_core1
	.type uart_puts_core1, %function
uart_puts_core1:
	push	{lr}
	ldr	r1, =UART0_BASE
2:
	ldr	r2, [r1, #0x14]
	ands	r2, r2, #UART_LSR_TEMT
	beq	2b

	str	r0, [r1, #0x00]
	pop	{pc}


	.type hdr, %function
hdr:
	push	{lr}

	mrc	p15, 0, r1, c0, c0, 5
	ands	r1, r1, #3

	ldreq	r2, =core0hdr
	ldrne	r2, =core1hdr
1:
	ldr	r1, =UART0_BASE
	ldr	r1, [r1, #0x14]
	ands	r1, r1, #UART_LSR_TEMT
	beq	1b
	ldr	r1, =UART0_BASE
2:
	ldrb	r3,[r2],#1
	cmp	r3, #0
	popeq	{pc}

	str	r3,[r1, #0x00]
	b	1b

	.align 4
core0hdr:
	.asciz	"{"
	.align	4
core1hdr:
	.asciz	"<"

	.align	4
	.type footer, %function
footer:
	push {lr}

	mrc	p15, 0, r1, c0, c0, 5
	ands	r1, r1, #3

	ldreq	r2, =core0ftr
	ldrne	r2, =core1ftr
1:
	ldr	r1, =UART0_BASE
	ldr	r1, [r1, #0x14]
	ands	r1, r1, #UART_LSR_TEMT
	beq	1b
	ldr	r1, =UART0_BASE
2:
	ldrb	r3,[r2],#1
	cmp		r3, #0
	popeq	{pc}

	str	r3,[r1, #0x00]
	b	1b

	.align 4
core0ftr:
	.asciz	"}"
	.align	4
core1ftr:
	.asciz	">"

	.align	4	
	.global uart_p_putc
	.type	uart_p_putc, %function
uart_p_putc:	//(char c )
	mov	r3, lr

	ldr	r1, =UART0_BASE
1:
	ldr	r2, [r1, #0x14]
	ands	r2, r2, #UART_LSR_TEMT
	beq	1b

	str	r0, [r1, #0x00]

	bx	r3


	.global uart_putc
	.type	uart_putc, %function
uart_putc:	//(char c ) {
	mov	r3,	lr
	ldr	r1, =UART0_BASE
1:
	ldr	r2, [r1, #0x14]
	ands	r2, r2, #UART_LSR_TEMT
	beq	1b

	str	r0, [r1, #0x00]
	bx r3


	.align 4
	.global	uart_puts
	.type	uart_puts, %function
uart_puts:	//(const char* s)
	push	{lr}
	
	ldr	r1, =UART0_BASE
	mov	r3, #0
1:
	ldrb	r3, [r0], #1
	cmp	r3, #0
	beq	3f
2:
	ldr	r2, [r1, #0x14]
	ands	r2, r2, #UART_LSR_TEMT
	beq	2b

	str	r3, [r1, #0x00]
	b	1b

3:
	pop	{pc}


	.global	uart_putx
	.type	uart_putx, %function
uart_putx:	//(uint32_t v32)
	push	{r10, lr}
	ldr	r1, =UART0_BASE
	mov	r10, #(4 * 7)
1:
	mov	r3, r0, LSR r10
	and	r3, r3, #0x0F
	ldr	r2, =uart_putx_conv_tbl
	ldrb	r3, [r2, r3]
2:
	ldr	r2, [r1, #0x14]
	ands	r2, r2, #UART_LSR_TEMT
	beq	2b

	str	r3, [r1, #0x00]
	subs	r10, r10, #4
	bpl	1b		//unil under 0

	pop	{r10, pc}


	.align	4
uart_putx_conv_tbl:
	.ascii	"0123456789ABCDEF"

	.align	4
	.global	uart_exbgn
	.type	uart_exbgn, %function
uart_exbgn:
	push	{lr}

	// make it exclusive
	ldr	r3, =uart_putex
	mov	r1, #1
	dmb
6:
	ldrex	r2, [r3]
	cmp	r2, #0
	strexeq	r2, r1, [r3]
	cmpeq	r2, #0
	bne	6b		// uart0 is in use by core0
	dmb			// now, uart0 is free to use by core1

	// output the header
	mrc	p15, 0, r1, c0, c0, 5
	ands	r1, r1, #3

	ldreq	r2, =c0hdr
	ldrne	r2, =c1hdr
1:
	ldr	r1, =UART0_BASE
	ldr	r1, [r1, #0x14]
	ands	r1, r1, #UART_LSR_TEMT
	beq	1b
	ldr	r1, =UART0_BASE
2:
	ldrb	r3,[r2],#1
	cmp	r3, #0
	popeq	{pc}

	str	r3,[r1, #0x00]
	b	1b



	.align 4
c0hdr:
	.asciz	"{"
	.align	4
c1hdr:
	.asciz	"<"

	.align	4
	.global	uart_exend
	.type	uart_exend, %function
uart_exend:
	push	{lr}

	// output the footer
	mrc	p15, 0, r1, c0, c0, 5
	ands	r1, r1, #3

	ldreq	r2, =c0ftr
	ldrne	r2, =c1ftr
1:
	ldr	r1, =UART0_BASE
	ldr	r1, [r1, #0x14]
	ands	r1, r1, #UART_LSR_TEMT
	beq	1b
	ldr	r1, =UART0_BASE
2:
	ldrb	r3,[r2],#1
	cmp	r3, #0
	beq	3f

	str	r3,[r1, #0x00]
	b	1b

3:
	// release it from exclusive
	dsb
	isb

	ldr	r3, =uart_putex
	mov	r1, #0
	str	r1, [r3]	// clear the exclusive flag
	dmb

	pop	{pc}



	.align 4
c0ftr:
	.asciz	"}"
	.align	4
c1ftr:	
	.asciz	">"


