#ifndef __MPTK_PLATFORM_IF_H__
#define __MPTK_PLATFORM_IF_H__

#include <mptk_typedef.h>

typedef struct {
	void (*initialize_hw_timer)(void);
	void (*clear_hw_timer_interrupt)(void);
	void (*end_of_hw_timer_interrupt)(void);
	void (*terminate_hw_timer)(void);
	UW (*get_hw_timer_nsec)(void);
	INT (*usermain)(void);
	UW gic_cpu_base_addr;
	UW gic_dist_base_addr;
	UW timer_irq_id;
	UW max_interrupt_source;
	UW lowmem_top;
	UW lowmem_limit;
} PLATFORM_INTERFACE;
IMPORT void start_kernel(void);
IMPORT void get_platform_interface(PLATFORM_INTERFACE *pi);
IMPORT UINT get_l1_data_cache_setsize(void);
IMPORT UINT get_l2_cache_setsize(void);

#endif /* __MPTK_PLATFORM_IF_H__ */
