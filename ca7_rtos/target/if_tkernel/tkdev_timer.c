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
 *	tkdev_timer.h (CA7MPCORE)
 *	Hardware-Dependent Timer Processing
 */

#include <mptk_typedef.h>
#include "porting.h"
#include "task_monitor.h"

IMPORT UINT disint( void );
IMPORT UINT enaint( UINT intsts );
IMPORT UINT TIMER_PERIOD;
IMPORT void udelay(UW);

inline void out_w( UINT port, UW data )
{
	*(_UW*)port = data;
}
inline UW in_w( UINT port )
{
	return *(_UW*)port;
}

/*
 * Set timer
 */
EXPORT void initialize_hw_timer( void )
{
	UW	imask;

	imask = disint();

// MILB_SDK Update Start
#if 0
	/* Stop timer count */
	out_w(TIMER1_CTRL,in_w(TIMER1_CTRL) & ~TMR_EN);
	udelay(2);

	/* Set counter */
	out_w(TIMER1_INTV, TMR_COUNTER);
	/* Start timer count */
	out_w(TIMER1_CTRL,
	      ~TMR_MODE &		// Continuous mode
	      (TMR_CLK_PRES(7) |	// divide by 128
	       TMR_CLK_SRC(1) |	// OSC24M
	       TMR_RELOAD |		// reload interval value automaticaly
	       TMR_EN));		// Enable Timer2 

	out_w(TIMER_IENBLR, TMR1_IRQ_EN);
#endif

    *(volatile unsigned long*)(TIMER_BASE) = 0x00000412;			// TMCSR0  ch13 コントロールステータスレジスタ
    *(volatile unsigned long*)(TIMER_BASE) &= 0xFFFFFFF8;			// TMCSR0  ch13 コントロールステータスレジスタ
    *(volatile unsigned long*)(TIMER_TMRLR1) = 0x00001A5D;			// TMRLR01 ch13 リロードレジスタ1 (6750 - 1):1ms
    *(volatile unsigned long*)(TIMER_TMRLR2) = 0;					// TMRLR02 ch13 リロードレジスタ2
    *(volatile unsigned long*)(TIMER_BASE) |= 0x0000000B;			// INTE/CNTE/TRG
// MILB_SDK Update End

	enaint(imask);
}

#if 0
/*
 * Timer start processing
 *	Initialize the timer and start the periodical timer interrupt.
 */
void start_hw_timer( void )
{
IMPORT	void	timer_handler_startup( void );

	/* Set timer */
	init_hw_timer();

	out_w(GICD_ISACTIVER(VECNO_TMR1), in_w(GICD_ISACTIVER(VECNO_TMR1)) | _BIT1(1, VECNO_TMR1));
	out_w(GICD_ICPENDR(VECNO_TMR1), in_w(GICD_ICPENDR(VECNO_TMR1)) | _BIT1(1, VECNO_TMR1));

	/* Set interrupt Mode */
	SetIntMode(VECNO_TMR1, IM_LEVEL);

	/* Interrupt handler definition */
	define_inthdr(VECNO_TMR1, timer_handler_startup);

	/* Set cpu target */
	SetIntAssprc(VECNO_TMR1, 1 << get_prid());

	/* Timer interrupt enable */
	EnableInt(VECNO_TMR1, TIMER_INTLEVEL);
}
#endif

/*
 * Clear timer interrupt
 *	Clear the timer interrupt request. Depending on the type of
 *	hardware, there are two timings for clearing: at the beginning
 *	and the end of the interrupt handler.
 *	'clear_hw_timer_interrupt()' is called at the beginning of the
 *	timer interrupt handler.
 *	'end_of_hw_timer_interrupt()' is called at the end of the timer
 *	interrupt handler.
 *	Use either or both according to hardware.
 */
void clear_hw_timer_interrupt( void )
{
// MILB_SDK Update Start
#if 0
	/* Clear timer interrupt */
	out_w(TIMER_IPENDR, in_w(TIMER_IPENDR) | TMR1_IRQ_PEND);
	//out_w(GICD_ICPENDR(VECNO_TMR1), in_w(GICD_ICPENDR(VECNO_TMR1)) | _BIT1(1, VECNO_TMR1));
#endif

	*(volatile unsigned long*)(TIMER_BASE) &= 0xFFFFFFFB;
// MILB_SDK Update End
}

void end_of_hw_timer_interrupt( void )
{
	//EndOfInt(VECNO_TMR1);
// MILB_SDK Add Start
#ifdef CO_TASK_MONITOR_ON
	OS_Task_Mointor();
#endif
// MILB_SDK Add End
}

/*
 * Timer stop processing
 *	Stop the timer operation.
 *	Called when system stops.
 */
void terminate_hw_timer( void )
{
// MILB_SDK Update Start
#if 0
	/* Interrupt disable */
	out_w(TIMER_IPENDR, in_w(TIMER_IPENDR) | TMR1_IRQ_PEND);
	out_w(TIMER1_CTRL, in_w(TIMER1_CTRL) & ~TMR_EN);

	//DisableInt(VECNO_TMR1);
#endif

	*(volatile unsigned long*)(TIMER_BASE) &= 0xFFFFFFB8;
// MILB_SDK Update End
}

/*
 * Get processing time from the previous timer interrupt to the
 * current (nanosecond)
 *	Consider the possibility that the timer interrupt occurred
 *	during the interrupt disable and calculate the processing time
 *	within the following
 *	range: 0 <= Processing time < TIMER_PERIOD * 2
 */
UINT get_hw_timer_nsec( void )
{
// MILB_SDK Update Start
#if 0
	UW ofs, imask;
	imask = disint();
	ofs = TMR_COUNTER - in_w(TIMER1_CURR);
	enaint(imask);
	/* ofs * 1000 / TMR_CLK_KHZ : to avoid overflow */
	return ofs * 1000 / TMR_CLK_KHZ;
#endif

	UW ofs, imask;
	imask = disint();
	ofs = *(volatile unsigned long*)(TIMER_TMRLR1) - *(volatile unsigned long*)(TIMER_TMR);
	enaint(imask);
	return ofs * 1000 * 1000 / 0x00001A5D;
// MILB_SDK Update End
}

