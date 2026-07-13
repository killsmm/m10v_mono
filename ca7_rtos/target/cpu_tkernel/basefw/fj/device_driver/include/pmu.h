/**
 * @file		pmu.h
 * @brief		Performance Monitor Unit header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _PMU_H_
#define _PMU_H_

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// PMU Counter Number
typedef enum {
	E_PMU_COUNTER_NUM0 = 0,						// Counter #0
	E_PMU_COUNTER_NUM1,							// Counter #1
	E_PMU_COUNTER_NUM2,							// Counter #2
	E_PMU_COUNTER_NUM3,							// Counter #3
	E_PMU_COUNTER_NUM_MAX						// Counter MAX
} E_PMU_COUNTER;

// PMU Event Number
typedef enum {									// Number	Description
	E_PMU_EVENT_SW_INCR = 0x00,					// 0x00		Software increment. The register is incremented only on writes to the Software Increment Register.
	E_PMU_EVENT_L1I_CACHE_REFILL,				// 0x01		Instruction fetch that causes a refill at (at least) the lowest level of instruction or unified cache.
												//			Includes the speculative linefills in the count.
	E_PMU_EVENT_L1I_TLB_REFILL,					// 0x02		Instruction fetch that causes a TLB refill at (at least) the lowest level of TLB.
												//			Includes the speculative requests in the count.
	E_PMU_EVENT_L1D_CACHE_REFILL,				// 0x03		Data read or write operation that causes a refill at (at least) the lowest level of data or unified cache.
												//			Counts the number of allocations performed in the Data Cache because of a read or a write.
	E_PMU_EVENT_L1D_CACHE,						// 0x04		Data read or write operation that causes a cache access at (at least) the lowest level of data or unified cache.
												//			This includes speculative reads.
	E_PMU_EVENT_L1D_TLB_REFILL,					// 0x05		Data read or write operation that causes a TLB refill at (at least) the lowest level of TLB.
												//			This does not include micro TLB misses because of PLD, PLI, CP15 Cache operation by MVA and CP15 VA to PA operations.
	E_PMU_EVENT_LD_RETIRED,						// 0x06		Data read architecturally executed. Counts the number of data read instructions accepted by the Load Store Unit.
												//			This includes counting the speculative and aborted LDR/LDM, and the reads because of the SWP instructions.
	E_PMU_EVENT_ST_RETIRED,						// 0x07		Data write architecturally executed. Counts the number of data write instructions accepted by the Load Store Unit.
												//			This includes counting the speculative and aborted STR/STM, and the writes because of the SWP instructions.
	E_PMU_EVENT_INST_RETIRED,					// 0x08		Instruction architecturally executed.
	E_PMU_EVENT_EXC_TAKEN,						// 0x09		Exception taken. Counts the number of exceptions architecturally taken.
	E_PMU_EVENT_EXC_RETURN,						// 0x0A		Exception return architecturally executed.
	E_PMU_EVENT_CID_WRITE_RETIRED,				// 0x0B		Change to ContextID retired. Counts the number of instructions architecturally executed writing into the ContextID Register.
	E_PMU_EVENT_PC_WRITE_RETIRED,				// 0x0C		Software change of PC.
	E_PMU_EVENT_BR_IMMED_RETIRED,				// 0x0D		Immediate branch architecturally executed (taken or not taken).
												//			This includes the branches which are flushed due to a previous load/store which aborts late.
	E_PMU_EVENT_BR_RETURN_RETIRED,				// 0x0E		Procedure return (other than exception returns) architecturally executed.
	E_PMU_EVENT_UNALIGNED_LDST_RETIRED,			// 0x0F		Unaligned load-store.
	E_PMU_EVENT_BR_MIS_PRED,					// 0x10		Branch mispredicted/not predicted. Counts the number of mispredicted or not-predicted branches executed.
												//			This includes the branches which are flushed because of a previous load/store which aborts late.
	E_PMU_EVENT_CPU_CYCLES,						// 0x11		Cycle counter.
	E_PMU_EVENT_BR_PRED,						// 0x12		Branches or other change in program flow that could have been predicted by the branch prediction resources of the processor.
												//			This includes the branches which are flushed because of a previous load/store which aborts late.
	E_PMU_EVENT_MEM_ACCESS,						// 0x13		Data memory access.
	E_PMU_EVENT_L1I_CACHE,						// 0x14		Instruction Cache access.
	E_PMU_EVENT_L1D_CACHE_WB,					// 0x15		Data cache eviction.
	E_PMU_EVENT_L2D_CACHE_ACCESS,				// 0x16		Level 2 data cache access.
	E_PMU_EVENT_L2D_CACHE_REFILL,				// 0x17		Level 2 data cache refill.
	E_PMU_EVENT_L2D_CACHE_WB,					// 0x18		Level 2 data cache write-back.
	E_PMU_EVENT_BUS_ACCESS,						// 0x19		Bus accesses.
	E_PMU_EVENT_BUS_CYCLE = 0x1D,				// 0x1D		Bus cycle.
	E_PMU_EVENT_BUS_ACCESS_READ = 0x60,			// 0x60		Bus accesses read.
	E_PMU_EVENT_BUS_ACCESS_WRITE,				// 0x61		Bus accesses write.
	E_PMU_EVENT_EXC_IRQ = 0x86,					// 0x86		IRQ exception taken.
	E_PMU_EVENT_EXC_FIQ,						// 0x87		FIQ exception taken.
	E_PMU_EVENT_EXT_MEMORY_REQUEST = 0xC0,		// 0xC0		External memory request.
	E_PMU_EVENT_NC_EXT_MEMORY_REQUEST,			// 0xC1		Non-cacheable external memory request.
	E_PMU_EVENT_PREFETCH_LINEFILL,				// 0xC2		Linefill because of prefetch.
	E_PMU_EVENT_PREFETCH_LINEFILL_DROPPED,		// 0xC3		Prefetch linefill dropped.
	E_PMU_EVENT_ENTERING_READ_ALLOCATE_MODE,	// 0xC4		Entering read allocate mode.
	E_PMU_EVENT_READ_ALLOCATE_MODE,				// 0xC5		Read allocate mode.
	E_PMU_EVENT_ETM_OUT0 = 0xC7,				// 0xC7		ETM Ext Out[0].
	E_PMU_EVENT_ETM_OUT1,						// 0xC8		ETM Ext Out[1].
	E_PMU_EVENT_DATA_WRITE_OPERATION,			// 0xC9		Data Write operation that stalls the pipeline because the store buffer is full.
	E_PMU_EVENT_DATA_SNOOP_PROCESS				// 0xCA		Data snooped from other processor.
} E_PMU_EVENT;

// PMU Reset Counter
typedef enum {
	E_PMU_RESET_NO_ACTION = 0,					// No Action
	E_PMU_RESET_EVENT_COUNTERS,					// Reset Evnet Counters to Zero
	E_PMU_RESET_CYCLE_COUNTER,					// Reset Cycle Counter to Zero
	E_PMU_RESET_ALL_COUNTERS					// Reset All Counters to Zero
} E_PMU_RESET;

// PMU Cycle Divider
typedef enum {
	E_PMU_DIVIDER_NO_DIVIDE = 0,				// Count Every Clock Cycle
	E_PMU_DIVIDER_DIVIDE_64						// Count Every 64th Clock Cycle
} E_PMU_DIVIDER;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
VOID BF_PMU_Init(VOID);
VOID BF_PMU_Ctrl_Cycle_Enable(VOID);
VOID BF_PMU_Ctrl_Cycle_Disable(VOID);
VOID BF_PMU_Ctrl_Event_Set(E_PMU_COUNTER counter_num, E_PMU_EVENT event_num);
VOID BF_PMU_Ctrl_Event_Disable(E_PMU_COUNTER counter_num);
VOID BF_PMU_Start(E_PMU_RESET reset_counter, E_PMU_DIVIDER cycle_divider);
VOID BF_PMU_Stop(VOID);
UINT32 BF_PMU_Get_Cycle_Count(VOID);
UINT32 BF_PMU_Get_Event_Count(E_PMU_COUNTER counter_num);
UINT32 BF_PMU_Get_Event_Number(E_PMU_COUNTER counter_num);
#endif	// _PMU_H_
