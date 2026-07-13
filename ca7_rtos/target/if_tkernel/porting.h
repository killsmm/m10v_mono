#ifndef __PORTING_H__
#define __PORTING_H__

#define MAX_PROC		4		/* The maximum number of CPU cores */

/*
 * Program status register (PSR)
 */
#define PSR_N		0x80000000	/* Condition flag Negative */
#define PSR_Z		0x40000000	/* Zero */
#define PSR_C		0x20000000	/* Carry */
#define PSR_V		0x10000000	/* Overflow */

#define PSR_Q		0x08000000	/* Sticky overflow */
#define PSR_J		0x01000000	/* Jazelle mode */

#define PSR_GE		0x000F0000	/* GE bits for SIMD Instruction */
#define PSR_E		0x00000200	/* Data Endian */
#define PSR_A		0x00000100	/* Unaccuracy Abort */

#define PSR_I		0x00000080	/* Interrupt (IRQ) disable */
#define PSR_F		0x00000040	/* Fast Interrupt (FIQ) disable */
#define PSR_T		0x00000020	/* Thumb mode */

#define PSR_DI		( PSR_I )	/* All Interrupts disable */

#define PSR_M(n)	( n )		/* Processor mode 0-31 */
#define PSR_USR		PSR_M(16)	/* User mode */
#define PSR_FIQ		PSR_M(17)	/* Fast Interrupt (FIQ) mode */
#define PSR_IRQ		PSR_M(18)	/* Interrupt (IRQ) mode */
#define PSR_SVC		PSR_M(19)	/* Supervisor mode */
#define PSR_ABT		PSR_M(23)	/* Abort mode */
#define PSR_UND		PSR_M(27)	/* Undefined order mode */
#define PSR_SYS		PSR_M(31)	/* System mode */

/*
 * System control coprocessor (CP15): Control register (CR1)
 */
#define CR1_M		0x00000001	/* MMU enable */
#define CR1_A		0x00000002	/* Alignment check enable */
#define CR1_C		0x00000004	/* Cache enable */
#define CR1_W		0x00000008	/* Write buffer enable */
#define CR1_S		0x00000100	/* System protection */
#define CR1_R		0x00000200	/* ROM protection */
#define CR1_Z		0x00000800	/* Program Flow Enable */
#define CR1_I		0x00001000	/* Instruction cache enable */
#define CR1_V		0x00002000	/* High vector */
#define CR1_L4		0x00008000	/* ARMv4 compatible mode */
#define CR1_U		0x00400000	/* Unaligned Data Access */
#define CR1_XP		0x00800000	/* Extended Page Table Configuration */
#define CR1_EE		0x02000000	/* CPSR E bit Setting */
#define CR1_NM		0x08000000	/* NMFI bit */
#define CR1_TR		0x10000000	/* TEX Remapping */
#define CR1_FA		0x20000000	/* Force AP bit */

/*
 * Aux control coprocesor control register ACTRL
 */
#define ACTLR_SMP	0x00000040	/* SMP : Enable coherent request to processor */
#define ACTLR_FW	0x00000001	/* FW  : Enable ... */

#define EIT_IRQ(n)	( (n) )

/*
 * Interrupt controller (General Interrupt Controller (GIC))
 */
#define	GIC_DIST_BASE	(0x1D001000)
#define	GIC_CPU_BASE	(0x1D002000)

#define	MAX_INTID		320

/**
 * Timer registers
 */
#define TIMER_BASE		(0x1E0000D0)
#define TIMER_TMR		(TIMER_BASE+0x04)
#define TIMER_TMRLR1	(TIMER_BASE+0x08)
#define TIMER_TMRLR2	(TIMER_BASE+0x0C)

#define VECNO_TMR1		131


#ifndef Asm
#define Asm		__asm__ volatile
#endif

#if _Csym == 0
#define Csym(sym)	sym
#else
#define Csym(sym)	_##sym
#endif

#endif /* __PORTING_H__ */

