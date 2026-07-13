//#include <kernel.h>
//#include <tk/tkernel.h>
//#include <sys/sysinfo.h>
//#include <task.h>
#include <string.h>
#include "../target/if_tkernel/porting.h"
#include "libcsupport.h"

#if 0
IMPORT TCB *ctxtsk[MAX_PROC];
#endif

// Reentrant struct table for interrupt handler.
static struct _reent g_intr_reent_data[MAX_PROC];

/* 
 * This function returns _reent struct from
 * currently running task control block, from currently
 * running CPU.
 *
 * This function active only when Newlib is compiled with
 * __DYNAMIC_REENT__
 *
 * By activating dynamic reentrancy, instead of having shared 
 * global _reent structure (which is defined by Newlib in impure.c)
 * pointed by _global_impure_ptr, each threads will have
 * their own _reent structure, which is destroyed when the
 * thread finishes execution.
 *
 */

/* This function should be left as it is, do not change unless you
 * know what you are doing.  */

// Initialize reentrant struct for interrupt handler.
void newlib_getreent_init_intr_tbl( void )
{
	unsigned int	loopcnt;

	for( loopcnt = 0; loopcnt < MAX_PROC; loopcnt++ ) {
		_REENT_INIT_PTR( &g_intr_reent_data[loopcnt] );
	}
}

#if 0
struct _reent *__getreent( void )
{
	T_RTSK t_rtsk;
	
	if(tk_ref_tsk(TSK_SELF, &t_rtsk) == E_ID){
		// Interrupt context.
		return &g_intr_reent_data[get_prid()];
	}
	else {
		// Task context.
		return &(ctxtsk[get_prid()]->nreent);
	}
}
#endif
