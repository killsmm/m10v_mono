/*
 * linux/arch/arm/lib/lib1funcs.S: Optimized ARM division routines
 *
 * Author: Nicolas Pitre <nico@fluxnic.net>
 *   - contributed to gcc-3.4 on Sep 30, 2003
 *   - adapted for the Linux kernel on Oct 2, 2003
 */

/* Copyright 1995, 1996, 1998, 1999, 2000, 2003 Free Software Foundation, Inc.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

In addition to the permissions in the GNU General Public License, the
Free Software Foundation gives you unlimited permission to link the
compiled version of this file into combinations with other programs,
and to distribute those combinations without any restriction coming
from the use of this file.  (The General Public License restrictions
do apply in other respects; for example, they cover modification of
the file, and distribution when not linked into a combine
executable.)

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */


//#include <linux/linkage.h>
//#include <asm/assembler.h>
//#include <asm/unwind.h>

        .irp    c,,eq,ne,cs,cc,mi,pl,vs,vc,hi,ls,ge,lt,gt,le,hs,lo
        .macro  ret\c, reg
        .ifeqs  "\reg", "lr"
        bx\c    \reg
        .else
        mov\c   pc, \reg
        .endif
        .endm
        .endr


.macro ARM_DIV_BODY dividend, divisor, result, curbit

	clz	\curbit, \divisor
	clz	\result, \dividend
	sub	\result, \curbit, \result
	mov	\curbit, #1
	mov	\divisor, \divisor, lsl \result
	mov	\curbit, \curbit, lsl \result
	mov	\result, #0
	
	@ Division loop
1:	cmp	\dividend, \divisor
	subhs	\dividend, \dividend, \divisor
	orrhs	\result,   \result,   \curbit
	cmp	\dividend, \divisor,  lsr #1
	subhs	\dividend, \dividend, \divisor, lsr #1
	orrhs	\result,   \result,   \curbit,  lsr #1
	cmp	\dividend, \divisor,  lsr #2
	subhs	\dividend, \dividend, \divisor, lsr #2
	orrhs	\result,   \result,   \curbit,  lsr #2
	cmp	\dividend, \divisor,  lsr #3
	subhs	\dividend, \dividend, \divisor, lsr #3
	orrhs	\result,   \result,   \curbit,  lsr #3
	cmp	\dividend, #0			@ Early termination?
	movnes	\curbit,   \curbit,  lsr #4	@ No, any more bits to do?
	movne	\divisor,  \divisor, lsr #4
	bne	1b

.endm


.macro ARM_DIV2_ORDER divisor, order

	clz	\order, \divisor
	rsb	\order, \order, #31

.endm


.macro ARM_MOD_BODY dividend, divisor, order, spare


	clz	\order, \divisor
	clz	\spare, \dividend
	sub	\order, \order, \spare
	mov	\divisor, \divisor, lsl \order

	@ Perform all needed substractions to keep only the reminder.
	@ Do comparisons in batch of 4 first.
	subs	\order, \order, #3		@ yes, 3 is intended here
	blt	2f

1:	cmp	\dividend, \divisor
	subhs	\dividend, \dividend, \divisor
	cmp	\dividend, \divisor,  lsr #1
	subhs	\dividend, \dividend, \divisor, lsr #1
	cmp	\dividend, \divisor,  lsr #2
	subhs	\dividend, \dividend, \divisor, lsr #2
	cmp	\dividend, \divisor,  lsr #3
	subhs	\dividend, \dividend, \divisor, lsr #3
	cmp	\dividend, #1
	mov	\divisor, \divisor, lsr #4
	subges	\order, \order, #4
	bge	1b

	tst	\order, #3
	teqne	\dividend, #0
	beq	5f

	@ Either 1, 2 or 3 comparison/substractions are left.
2:	cmn	\order, #2
	blt	4f
	beq	3f
	cmp	\dividend, \divisor
	subhs	\dividend, \dividend, \divisor
	mov	\divisor,  \divisor,  lsr #1
3:	cmp	\dividend, \divisor
	subhs	\dividend, \dividend, \divisor
	mov	\divisor,  \divisor,  lsr #1
4:	cmp	\dividend, \divisor
	subhs	\dividend, \dividend, \divisor
5:
.endm

	.globl	__udivsi3, __aeabi_uidiv
	.type	__aeabi_uidiv, function

__udivsi3:
__aeabi_uidiv:

	subs	r2, r1, #1
	reteq	lr
	bcc	Ldiv0
	cmp	r0, r1
	bls	11f
	tst	r1, r2
	beq	12f

	ARM_DIV_BODY r0, r1, r2, r3

	mov	r0, r2
	ret	lr

11:	moveq	r0, #1
	movne	r0, #0
	ret	lr

12:	ARM_DIV2_ORDER r1, r2

	mov	r0, r0, lsr r2
	ret	lr


__umodsi3:

	subs	r2, r1, #1			@ compare divisor with 1
	bcc	Ldiv0
	cmpne	r0, r1				@ compare dividend with divisor
	moveq   r0, #0
	tsthi	r1, r2				@ see if divisor is power of 2
	andeq	r0, r0, r2
	retls	lr

	ARM_MOD_BODY r0, r1, r2, r3

	ret	lr

	.globl	__aeabi_idiv
	.type	__aeabi_idiv, function

__divsi3:
__aeabi_idiv:

	cmp	r1, #0
	eor	ip, r0, r1			@ save the sign of the result.
	beq	Ldiv0
	rsbmi	r1, r1, #0			@ loops below use unsigned.
	subs	r2, r1, #1			@ division by 1 or -1 ?
	beq	10f
	movs	r3, r0
	rsbmi	r3, r0, #0			@ positive dividend value
	cmp	r3, r1
	bls	11f
	tst	r1, r2				@ divisor is power of 2 ?
	beq	12f

	ARM_DIV_BODY r3, r1, r0, r2

	cmp	ip, #0
	rsbmi	r0, r0, #0
	ret	lr

10:	teq	ip, r0				@ same sign ?
	rsbmi	r0, r0, #0
	ret	lr

11:	movlo	r0, #0
	moveq	r0, ip, asr #31
	orreq	r0, r0, #1
	ret	lr

12:	ARM_DIV2_ORDER r1, r2

	cmp	ip, #0
	mov	r0, r3, lsr r2
	rsbmi	r0, r0, #0
	ret	lr

__modsi3:

	cmp	r1, #0
	beq	Ldiv0
	rsbmi	r1, r1, #0			@ loops below use unsigned.
	movs	ip, r0				@ preserve sign of dividend
	rsbmi	r0, r0, #0			@ if negative make positive
	subs	r2, r1, #1			@ compare divisor with 1
	cmpne	r0, r1				@ compare dividend with divisor
	moveq	r0, #0
	tsthi	r1, r2				@ see if divisor is power of 2
	andeq	r0, r0, r2
	bls	10f

	ARM_MOD_BODY r0, r1, r2, r3

10:	cmp	ip, #0
	rsbmi	r0, r0, #0
	ret	lr

	.globl	__aeabi_uidivmod
	.type	__aeabi_uidivmod, function

__aeabi_uidivmod:

	stmfd	sp!, {r0, r1, ip, lr}
	bl	__aeabi_uidiv
	ldmfd	sp!, {r1, r2, ip, lr}
	mul	r3, r0, r2
	sub	r1, r1, r3
	ret	lr

	.globl	__aeabi_idivmod
	.type	__aeabi_idivmod, function

__aeabi_idivmod:
	stmfd	sp!, {r0, r1, ip, lr}
	bl	__aeabi_idiv
	ldmfd	sp!, {r1, r2, ip, lr}
	mul	r3, r0, r2
	sub	r1, r1, r3
	ret	lr

	.global	__aeabi_uldivmod
	.type	__aeabi_uldivmod, function

A_0	.req	r0
A_1	.req	r1
B_0	.req	r2
B_1	.req	r3
C_0	.req	r4
C_1	.req	r5
D_0	.req	r6
D_1	.req	r7

Q_0	.req	r0
Q_1	.req	r1
R_0	.req	r2
R_1	.req	r3


__aeabi_uldivmod:
	stmfd	sp!, {r4, r5, r6, r7, lr}
	@ Test if B == 0
	orrs	ip, B_0, B_1		@ Z set -> B == 0
	beq	L_div_by_0
	@ Test if B is power of 2: (B & (B - 1)) == 0
	subs	C_0, B_0, #1
	sbc	C_1, B_1, #0
	tst	C_0, B_0
	tsteq	B_1, C_1
	beq	L_pow2
	@ Test if A_1 == B_1 == 0
	orrs	ip, A_1, B_1
	beq	L_div_32_32

L_div_64_64:
	mov	C_0, #1
	mov	C_1, #0
	@ D_0 = clz A
	teq	A_1, #0
	clz	D_0, A_1
	clzeq	ip, A_0
	addeq	D_0, D_0, ip
	@ D_1 = clz B
	teq	B_1, #0
	clz	D_1, B_1
	clzeq	ip, B_0
	addeq	D_1, D_1, ip
	@ if clz B - clz A > 0
	subs	D_0, D_1, D_0
	bls	L_done_shift
	@ B <<= (clz B - clz A)
	subs	D_1, D_0, #32
	rsb	ip, D_0, #32
	movmi	B_1, B_1, lsl D_0
	orrmi	B_1, B_1, B_0, lsr ip
	movpl	B_1, B_0, lsl D_1
	mov	B_0, B_0, lsl D_0
	@ C = 1 << (clz B - clz A)
	movmi	C_1, C_1, lsl D_0
	orrmi	C_1, C_1, C_0, lsr ip
	movpl	C_1, C_0, lsl D_1
	mov	C_0, C_0, lsl D_0
L_done_shift:
	mov	D_0, #0
	mov	D_1, #0
	@ C: current bit; D: result
L_subtract:
	@ if A >= B
	cmp	A_1, B_1
	cmpeq	A_0, B_0
	bcc	L_update
	@ A -= B
	subs	A_0, A_0, B_0
	sbc	A_1, A_1, B_1
	@ D |= C
	orr	D_0, D_0, C_0
	orr	D_1, D_1, C_1
L_update:
	@ if A == 0: break
	orrs	ip, A_1, A_0
	beq	L_exit
	@ C >>= 1
	movs	C_1, C_1, lsr #1
	movs	C_0, C_0, rrx
	@ if C == 0: break
	orrs	ip, C_1, C_0
	beq	L_exit
	@ B >>= 1
	movs	B_1, B_1, lsr #1
	mov	B_0, B_0, rrx
	b	L_subtract
L_exit:
	@ Note: A, B & Q, R are aliases
	mov	R_0, A_0
	mov	R_1, A_1
	mov	Q_0, D_0
	mov	Q_1, D_1
	ldmfd	sp!, {r4, r5, r6, r7, pc}

L_div_32_32:
	@ Note:	A_0 &	r0 are aliases
	@	Q_1	r1
	mov	r1, B_0
	bl	__aeabi_uidivmod
	mov	R_0, r1
	mov	R_1, #0
	mov	Q_1, #0
	ldmfd	sp!, {r4, r5, r6, r7, pc}

L_pow2:
	@ Note: A, B and Q, R are aliases
	@ R = A & (B - 1)
	and	C_0, A_0, C_0
	and	C_1, A_1, C_1
	@ Q = A >> log2(B)
	@ Note: B must not be 0 here!
	clz	D_0, B_0
	add	D_1, D_0, #1
	rsbs	D_0, D_0, #31
	bpl	L_1
	clz	D_0, B_1
	rsb	D_0, D_0, #31
	mov	A_0, A_1, lsr D_0
	add	D_0, D_0, #32
L_1:
	movpl	A_0, A_0, lsr D_0
	orrpl	A_0, A_0, A_1, lsl D_1
	mov	A_1, A_1, lsr D_0
	@ Mov back C to R
	mov	R_0, C_0
	mov	R_1, C_1
	ldmfd	sp!, {r4, r5, r6, r7, pc}

L_div_by_0:
	bl	__div0
	@ As wrong as it could be
	mov	Q_0, #0
	mov	Q_1, #0
	mov	R_0, #0
	mov	R_1, #0
	ldmfd	sp!, {r4, r5, r6, r7, pc}

Ldiv0:
	str	lr, [sp, #-8]!
	bl	__div0
	mov	r0, #0			@ About as wrong as it could be.
	ldr	pc, [sp], #8

#define al r0
#define ah r1

	.globl	__aeabi_llsl
        .type   __aeabi_llsl, function

__ashldi3:
__aeabi_llsl:

        subs    r3, r2, #32
        rsb     ip, r2, #32
        movmi   ah, ah, lsl r2
        movpl   ah, al, lsl r3
// ARM(   orrmi   ah, ah, al, lsr ip      )
// THUMB(
	lsrmi   r3, al, ip
// THUMB(
	orrmi   ah, ah, r3
        mov     al, al, lsl r2
        ret     lr

