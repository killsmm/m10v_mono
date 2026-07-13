#include <mptk_platform.h>
#include "porting.h"

extern void _Reset_Handler(void);

extern UW __kernel_memory_start;
extern UW __kernel_memory_end;
extern UW __data_start;
extern UW __data_end;
extern UW __data_org;

#define kernel_memory_start (&__kernel_memory_start)
#define kernel_memory_end (&__kernel_memory_end)
#define kernel_memory_length ((UW)kernel_memory_end - (UW)kernel_memory_start)
#define data_org (&__data_org)
#define data_start (&__data_start)
#define data_length ((UW)(&__data_end) - (UW)(&__data_start))

inline UINT get_prid( void )
{
	UINT	prid;
	Asm("mrc p15, 0, %0, c0, c0, 5":"=r"(prid)::"cc");
	return (prid & 0x03);
}

inline UW align32(UW n) {
  return (n + 3) & 0xFFFFFFFC;
}

volatile UW abt_stack[MAX_PROC-1] = {
	0, 0, 0
};
volatile UW und_stack[MAX_PROC-1] = {
	0, 0, 0
};
volatile UW irq_stack[MAX_PROC-1] = {
	0, 0, 0
};
volatile UW fiq_stack[MAX_PROC-1] = {
	0, 0, 0
};

UW total_stack_size;
//#define AbtStkSz    64
//#define UndStkSz    64
//#define IrqStkSz    512
//#define FiqStkSz    128
#define AbtStkSz    0x4000
#define UndStkSz    0x4000
#define IrqStkSz    0x4000
#define FiqStkSz    0x4000

/*
 * Set stack by exception mode
 */
LOCAL void setExcStack( UW mode, UW stack )
{
	Asm("msr	cpsr_c, %1	\n\t"
		"mov	sp, %0		\n\t"
		"msr	cpsr_c, %2"
		:: "l"(stack), "r"(mode|PSR_I|PSR_F), "i"(PSR_SVC|PSR_I|PSR_F));
}

void init_stack(INT prid)
{
	UINT i;
	UW n;
	UW lowmem_top = (UW)kernel_memory_start;
	if (prid == 0) {
		/* Set stack for each exception mode */
		lowmem_top = (lowmem_top + 3) & ~0x00000003U; /* Alignment match */

		/* Abort (MMU) */
		n = AbtStkSz;
		lowmem_top += (n + 3) & ~0x00000003U;
		setExcStack(PSR_ABT, lowmem_top);

		for ( i = 0; i < MAX_PROC - 1; i++ ) {
			lowmem_top += (n + 3) & ~0x00000003U;
			abt_stack[i] = lowmem_top;
		}

		/* Undefined order exception */
		n = UndStkSz;
		lowmem_top += (n + 3) & ~0x00000003U;
		setExcStack(PSR_UND, lowmem_top);

		for ( i = 0; i < MAX_PROC - 1; i++ ) {
			lowmem_top += (n + 3) & ~0x00000003U;
			und_stack[i] = lowmem_top;
		}

		/* Interrupt (IRQ) */
		n = IrqStkSz;
		lowmem_top += (n + 3) & ~0x00000003U;
		setExcStack(PSR_IRQ, lowmem_top);

		for ( i = 0; i < MAX_PROC - 1; i++ ) {
			lowmem_top += (n + 3) & ~0x00000003U;
			irq_stack[i] = lowmem_top;
		}

		/* Fast interrupt (FIQ) */
		n = FiqStkSz;
		lowmem_top += (n + 3) & ~0x00000003U;
		setExcStack(PSR_FIQ, lowmem_top);

		for ( i = 0; i < MAX_PROC - 1; i++ ) {
			lowmem_top += (n + 3) & ~0x00000003U;
			fiq_stack[i] = lowmem_top;
		}
		total_stack_size = lowmem_top - (UW)kernel_memory_start;
	} else {
		while( (abt_stack[prid-1] == 0)
			|| (und_stack[prid-1] == 0)
			|| (irq_stack[prid-1] == 0)
			|| (fiq_stack[prid-1] == 0) ) {
			// Wait of stack address was calclated by primary CPU core.
			;
		}
		setExcStack(PSR_ABT, abt_stack[prid-1]);
		setExcStack(PSR_UND, und_stack[prid-1]);
		setExcStack(PSR_IRQ, irq_stack[prid-1]);
		setExcStack(PSR_FIQ, fiq_stack[prid-1]);
	}
}

UW xxx = 0x12345678;
void*
tk_porting_memcpy( void *dst, const void *src, unsigned int n )
{
	register unsigned char *cdst, *csrc;

	cdst = (unsigned char *)dst;
	csrc = (unsigned char *)src;
	while (n-- > 0) {
		*cdst++ = *csrc++;
	}

	return dst;
}
void pre_start_kernel() {
	int prid = get_prid();
	if (prid == 0) {
		//uart_puts("pre_start_kernel CPU0\n");
//		if (data_start != data_org) {
//			/* Initialize the DATA section if LMA and VMA are not the same*/
//			tk_porting_memcpy(data_start, /* VMA */
//				   data_org,   /* LMA */
//				   data_length);
//		}
		init_stack(0);
#if 0
		int i;
		PRINT(xxx);
		for (i = 0; i < 0x400; i+=4) {
			UW *p = (UW*)((UW)data_start + i);
			UW pp = *p;
			uart_putx((UW)p);
			uart_putc(':');
			PRINT(pp);
		}
#endif
//		*((_UW*)CPUCFG_PRIVATE_REG) = (UW)_Reset_Handler;
	} else {
		//uart_puts("pre_start_kernel CPU1\n");
		init_stack(prid);
	}
	start_kernel();
}

void get_platform_interface(PLATFORM_INTERFACE *pi)
{
	IMPORT void initialize_hw_timer(void);
	IMPORT void terminate_hw_timer(void);
	IMPORT void clear_hw_timer_interrupt(void);
	IMPORT void end_of_hw_timer_interrupt(void);
	IMPORT INT usermain(void);
	pi->initialize_hw_timer = initialize_hw_timer;
	pi->terminate_hw_timer = terminate_hw_timer;
	pi->clear_hw_timer_interrupt = clear_hw_timer_interrupt;
	pi->end_of_hw_timer_interrupt = end_of_hw_timer_interrupt;
	pi->usermain = usermain;
	pi->gic_cpu_base_addr = GIC_CPU_BASE;
	pi->gic_dist_base_addr = GIC_DIST_BASE;
	pi->timer_irq_id = VECNO_TMR1;
	pi->max_interrupt_source = MAX_INTID;
	pi->lowmem_top = (UW)kernel_memory_start + total_stack_size;
	pi->lowmem_limit = (UW)kernel_memory_end;
}
