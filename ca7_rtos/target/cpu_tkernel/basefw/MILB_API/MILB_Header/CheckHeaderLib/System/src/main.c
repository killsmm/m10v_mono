#include <rt_misc.h>
#include <stdio.h>
#include "CheckHeaderMain.h"

extern int $Super$$main(void);

extern unsigned int Image$$HEAP_AREA$$Base;
extern unsigned int Image$$STACK_AREA$$Base;

#pragma import(__use_two_region_memory)

__value_in_regs struct __initial_stackheap
__user_initial_stackheap(unsigned R0, unsigned SP, unsigned R2, unsigned SL)
{
    struct __initial_stackheap config;
	
	// note : Relation Stack and Heap
	// upper address
	//---------- config.stack_base
	//  Stack
	//---------- config.heap_limit
	//  Heap 
	//---------- config.heap_base
	// lower address
	
	config.heap_base = (unsigned int)&Image$$HEAP_AREA$$Base;
	config.heap_limit = (unsigned int)&Image$$STACK_AREA$$Base;
	config.stack_base = SP;
	
	// note: When you define "__use_two_region_memory", R3 register is ignored.
	return config;
}

void $Sub$$main(void)
{
	$Super$$main();
}

int main(void)
{
	CheckHeaderMain();
	
	return 0;
}

