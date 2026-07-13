/*
 * (C) Copyright 2003
 * Texas Instruments <www.ti.com>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002-2004
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 *
 * (C) Copyright 2004
 * Philippe Robin, ARM Ltd. <philippe.robin@arm.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <mptk_typedef.h>
#include "porting.h"
extern void uart_putc( char );
extern void uart_puts( const char* );
extern void uart_putx( UW );

#define USR26_MODE	0x00
#define FIQ26_MODE	0x01
#define IRQ26_MODE	0x02
#define SVC26_MODE	0x03
#define USR_MODE	0x10
#define FIQ_MODE	0x11
#define IRQ_MODE	0x12
#define SVC_MODE	0x13
#define ABT_MODE	0x17
#define UND_MODE	0x1b
#define SYSTEM_MODE	0x1f
#define MODE_MASK	0x1f
#define T_BIT		0x20
#define F_BIT		0x40
#define I_BIT		0x80
#define CC_V_BIT	(1 << 28)
#define CC_C_BIT	(1 << 29)
#define CC_Z_BIT	(1 << 30)
#define CC_N_BIT	(1 << 31)
#define PCMASK		0

struct pt_regs {
	long uregs[18];
};

#define ARM_cpsr	uregs[16]
#define ARM_pc		uregs[15]
#define ARM_lr		uregs[14]
#define ARM_sp		uregs[13]
#define ARM_ip		uregs[12]
#define ARM_fp		uregs[11]
#define ARM_r10		uregs[10]
#define ARM_r9		uregs[9]
#define ARM_r8		uregs[8]
#define ARM_r7		uregs[7]
#define ARM_r6		uregs[6]
#define ARM_r5		uregs[5]
#define ARM_r4		uregs[4]
#define ARM_r3		uregs[3]
#define ARM_r2		uregs[2]
#define ARM_r1		uregs[1]
#define ARM_r0		uregs[0]
#define ARM_ORIG_r0	uregs[17]

#define user_mode(regs)	\
	(((regs)->ARM_cpsr & 0xf) == 0)

#ifdef CONFIG_ARM_THUMB
#define thumb_mode(regs) \
	(((regs)->ARM_cpsr & T_BIT))
#else
#define thumb_mode(regs) (0)
#endif

#define processor_mode(regs) \
	((regs)->ARM_cpsr & MODE_MASK)

#define interrupts_enabled(regs) \
	(!((regs)->ARM_cpsr & I_BIT))

#define fast_interrupts_enabled(regs) \
	(!((regs)->ARM_cpsr & F_BIT))

#define condition_codes(regs) \
	((regs)->ARM_cpsr & (CC_V_BIT|CC_C_BIT|CC_Z_BIT|CC_N_BIT))

#define pc_pointer(v) \
	((v) & ~PCMASK)

#define instruction_pointer(regs) \
	(pc_pointer((regs)->ARM_pc))

inline UINT get_prid_excep_ints( void )
{
	UINT	prid;
	Asm("mrc p15, 0, %0, c0, c0, 5":"=r"(prid)::"cc");
	return (prid & 0x03);
}

/* "const char *" needs memcpy(). */
#if USE_NEWLIB
	/* Use Standard Library. */
#else
void* memcpy( void *dst, const void *src, unsigned int n );
void*
memcpy( void *dst, const void *src, unsigned int n )
{
	register unsigned char *cdst, *csrc;

	cdst = (unsigned char *)dst;
	csrc = (unsigned char *)src;
	while (n-- > 0) {
		*cdst++ = *csrc++;
	}

	return dst;
}
#endif

void bad_mode (void)
{
	uart_puts ("CPU looping forever...\n");
	while(1);
}

void show_regs (struct pt_regs *regs)
{
	unsigned int flags;
	int i;

	const char *processor_modes[] = {
	"USER_26",	"FIQ_26",	"IRQ_26",	"SVC_26",
	"UK4_26",	"UK5_26",	"UK6_26",	"UK7_26",
	"UK8_26",	"UK9_26",	"UK10_26",	"UK11_26",
	"UK12_26",	"UK13_26",	"UK14_26",	"UK15_26",
	"USER_32",	"FIQ_32",	"IRQ_32",	"SVC_32",
	"UK4_32",	"UK5_32",	"UK6_32",	"ABT_32",
	"UK8_32",	"UK9_32",	"UK10_32",	"UND_32",
	"UK12_32",	"UK13_32",	"UK14_32",	"SYS_32",
	};

	flags = condition_codes (regs);

	uart_puts("Core#"); uart_putx(get_prid_excep_ints());
	uart_puts("\npc : [<"); uart_putx(instruction_pointer(regs));
	uart_puts(">]     lr : [<"); uart_putx( regs->ARM_lr ); uart_puts(">]\n");
	uart_puts("sp : "); uart_putx(regs->ARM_sp); uart_puts("  ip : "); uart_putx(regs->ARM_ip);
	uart_puts("  fp : "); uart_putx(regs->ARM_fp); uart_putc('\n');

	uart_puts("r10: "); uart_putx(regs->ARM_r10); uart_puts("  r9 : "); uart_putx(regs->ARM_r9);
	uart_puts("  r8 : "); uart_putx(regs->ARM_r8); uart_putc('\n');

	uart_puts("r7 : "); uart_putx(regs->ARM_r7); uart_puts("  r6 : "); uart_putx(regs->ARM_r6);
	uart_puts("  r5 : "); uart_putx(regs->ARM_r5); uart_puts("  r4 : "); uart_putx(regs->ARM_r4);
	uart_putc('\n');

	uart_puts("r3 : "); uart_putx(regs->ARM_r3); uart_puts("  r2 : "); uart_putx(regs->ARM_r2);
	uart_puts("  r1 : "); uart_putx(regs->ARM_r1); uart_puts("  r0 : "); uart_putx(regs->ARM_r0);
	uart_putc('\n');

	uart_puts("Flags: ");
	uart_putc( flags && CC_N_BIT ? 'N' : 'n' );
	uart_putc( flags && CC_Z_BIT ? 'Z' : 'z' );
	uart_putc( flags && CC_C_BIT ? 'C' : 'c' );
	uart_putc( flags && CC_V_BIT ? 'V' : 'v' );

	uart_puts("  IRQs "); uart_puts( interrupts_enabled(regs) ? "on" : "off" );
	uart_puts("  FIQs "); uart_puts( fast_interrupts_enabled(regs) ? "on" : "off" );
	uart_puts("  Mode "); uart_puts( processor_modes[processor_mode(regs)] );
	uart_puts( thumb_mode(regs) ? " (T)" : "");
	uart_putc('\n');

	uart_puts("Stack trace:");
	for( i = -8; i < 12; i++) {
	  if( (i % 4) ) {
	    uart_putc(' ');
	  }
	  else {
	    uart_putc('\n');
	    uart_putx(regs->ARM_sp + (i * 4));
	    uart_putc(':');
	  }
	  uart_putx(*(UW*)(regs->ARM_sp + (i * 4)));
	}
	uart_putc('\n');
}

void do_undefined_instruction (struct pt_regs *pt_regs)
{
	uart_puts("undefined instruction\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_invalid_software_interrupt (struct pt_regs *pt_regs)
{
	uart_puts("software interrupt\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_prefetch_abort (struct pt_regs *pt_regs)
{
	uart_puts("prefetch abort\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_data_abort (struct pt_regs *pt_regs)
{
	uart_puts("data abort\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_not_used (struct pt_regs *pt_regs)
{
	uart_puts("not used\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_fiq (struct pt_regs *pt_regs)
{
	uart_puts("fast interrupt request\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_irq (struct pt_regs *pt_regs)
{
	uart_puts("interrupt request\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_reg_trace (struct pt_regs *pt_regs)
{
	uart_puts("register trace\n");
	show_regs (pt_regs);
}
