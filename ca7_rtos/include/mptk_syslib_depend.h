/*
 *----------------------------------------------------------------------
 *    SMP T-Kernel
 *
 *    Copyright (C) 2007-2010 Ken Sakamura. All Rights Reserved.
 *    SMP T-Kernel is distributed under the T-License for SMP T-Kernel.
 *----------------------------------------------------------------------
 *
 *    Version:   1.00.01
 *    Released by T-Engine Forum(http://www.t-engine.org) at 2010/02/19.
 *
 *----------------------------------------------------------------------
 */

/*
 *	@(#)syslib_depend.h (tk/CA7MPCORE)
 *
 *	T-Kernel/SM CA7MPCORE Library
 */

#ifndef __MPTK_SYSLIB_DEPEND_H__
#define __MPTK_SYSLIB_DEPEND_H__

/*
 for finding semihosting call
#ifdef _in_asm_source_
	IMPORT	__use_no_semihosting
#else // in C
#pragma import(__use_no_semihosting)
#endif // _in_asm_source_ & C
*/

#include <mptk_typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Interrupt vector
 *	The interrupt vector is the index number of the vector table.
 */
typedef UINT	INTVEC;

/* Convert to interrupt definition number */
#define DINTNO(intvec)	(intvec)

/*
 * Set interrupt mode
 *	Set accept mode to 'mode' regarding interrupt specified by 'intvec'.
 *	If 'mode' is not correct value, operation is not guaranteed.
 */
IMPORT void SetIntMode( INTVEC intvec, UINT mode );

#define IM_LEVEL	0x00U	/* level trigger (high level Only) */
#define IM_EDGE		0x02U	/* edge  trigger (rising edge only) */
#define IM_HI		0x00U	/* high level trigger/rising  edge trigger */

#define IM_NN		0x00U	/* N-N Software Model */
#define IM_1N		0x01U	/* 1-N Software Model */


/*
 * Enable interrupt
 *	Enable interrupt specified by 'intvec'.
 *
 *	level := '0'-'15'.
 *	level=15 means lowest priority level, level=0 means highest.
 *	If 'level' is outside this range, operation is not guaranteed.
 *	Set the PRI register with the value 'level'.
 *
 *	UPDATE: 4/19/2016
 *	When level is specified as -1, level is left as it is (no change)
 */
IMPORT void EnableInt( INTVEC intvec, INT level );


/*
 * Update Interrupt Level specified by 'intvec'
 *
 * This will update interrupt level unconditionally.
 *
 */
IMPORT void UpdateIntLevel( INTVEC intvec, INT level );

/*
 * Disable interrupt
 *	Disable interrupt specified by 'intvec'.
 */
IMPORT void DisableInt( INTVEC intvec );

/*
 * Clear interrupt request
 *	Clear interrupt request specified by 'intvec'.
 */
IMPORT void ClearInt( INTVEC intvec );

/*
 * Check for existence of interrupt request
 *	Check whether there is an interrupt request specified by 'intvec'.
 *	If there is an interrupt request, return TRUE (except 0).
 */
IMPORT BOOL CheckInt( INTVEC intvec );

/*
 * Issue EOI(End Of Interrupt)
 */
IMPORT void EndOfInt( INTVEC intvec );


typedef struct regs {
	UW r0;
	UW r1;
	UW r2;
	UW r3;
	UW r4;
	UW r5;
	UW r6;
	UW r7;
	UW r8;
	UW r9;
	UW r10;
	UW r11;
	UW r12;
	UW sp;
	UW lr;
	UW pc;
	UW cpsr;	
} REGS;

IMPORT ER DefineExceptionHandler( UINT excpno, FP excphdr, UINT index );

#define EXCP_UNDEFINED (0)
#define EXCP_DATA_ABORT (1)
#define EXCP_PREFETCH_ABORT (2)
	
#ifdef __cplusplus
}
#endif
#endif /* __MPTK_SYSLIB_DEPEND_H__ */
