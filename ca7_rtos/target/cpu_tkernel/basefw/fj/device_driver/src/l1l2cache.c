/**
 * @file		l1l2cache.c
 * @brief		L1 Cache and L2 Cache Clean,Invalidate Control
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include "l1l2cache.h"
#include "dd_arm.h"
#include "dd_cache.h"
#include "dd_gic.h"
#include "debug.h"
#include "bf_arm.h"
#include "os_user_custom.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/

#define D_BF_L1CACHE_LIMIT_SIZE	(0x00004000)	// L1 LIMIT SIZE is 16KByte
#define D_BF_L2CACHE_LIMIT_SIZE	(0x00040000)	// L2 LIMIT SIZE is 256KByte

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
static UINT32 gBF_L1l2cache_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

typedef volatile struct {
	UINT32					l1l2cache_cpu[3];
	E_BF_L1L2CACHE_METHOD	l1l2cache_method;
	char					reserved[48];
} T_BF_L1L2CACHE_ALL;
T_BF_L1L2CACHE_ALL	gBF_Shared_L1l2cache __attribute__((aligned(64)));

static const BOOL g_bf_l1l2cache_active_cpu[D_DD_ARM_NUM_OF_CORES] = {
	TRUE,
	TRUE,
	TRUE,
	FALSE,
};

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
VOID BF_L1l2cache_CallBack( UINT32 mask_status )
{
	if( mask_status & 0x000001fe ){
		//while(1);
		//BF_Debug_Print_Error(("--- L2C Interrupt Status=%08x\n", mask_status ));
	}
}

VOID BF_L1l2cache_Flush_All( VOID )
{
	T_L1L2CACHE_INFORMATION l1l2cache;
	
	l1l2cache.l1l2cache_type = E_BF_L1L2CACHE_ALL;
	l1l2cache.l1l2cache_method = E_BF_L1L2CACHE_FLUSH;
	BF_L1l2cache_Run( l1l2cache );
}

VOID BF_L1l2cache_Clean_All( VOID )
{
	T_L1L2CACHE_INFORMATION l1l2cache;
	
	l1l2cache.l1l2cache_type = E_BF_L1L2CACHE_ALL;
	l1l2cache.l1l2cache_method = E_BF_L1L2CACHE_CLEAN;
	BF_L1l2cache_Run( l1l2cache );
}

VOID BF_L1l2cache_Clean_Flush_All( VOID )
{
	T_L1L2CACHE_INFORMATION l1l2cache;
	
	l1l2cache.l1l2cache_type = E_BF_L1L2CACHE_ALL;
	l1l2cache.l1l2cache_method = E_BF_L1L2CACHE_CLEAN_FLUSH;
	BF_L1l2cache_Run( l1l2cache );
}

VOID BF_L1l2cache_Flush_Addr( UINT32 address, UINT32 size )
{
	T_L1L2CACHE_INFORMATION l1l2cache;
	
	l1l2cache.l1l2cache_type = E_BF_L1L2CACHE_ADDR;
	l1l2cache.l1l2cache_method = E_BF_L1L2CACHE_FLUSH;
	l1l2cache.l1l2cache_address = address;
	l1l2cache.l1l2cache_size = size;
	BF_L1l2cache_Run( l1l2cache );
}

VOID BF_L1l2cache_Clean_Addr( UINT32 address, UINT32 size )
{
	T_L1L2CACHE_INFORMATION l1l2cache;
	
	l1l2cache.l1l2cache_type = E_BF_L1L2CACHE_ADDR;
	l1l2cache.l1l2cache_method = E_BF_L1L2CACHE_CLEAN;
	l1l2cache.l1l2cache_address = address;
	l1l2cache.l1l2cache_size = size;
	BF_L1l2cache_Run( l1l2cache );
}

VOID BF_L1l2cache_Clean_Flush_Addr( UINT32 address, UINT32 size )
{
	T_L1L2CACHE_INFORMATION l1l2cache;
	
	l1l2cache.l1l2cache_type = E_BF_L1L2CACHE_ADDR;
	l1l2cache.l1l2cache_method = E_BF_L1L2CACHE_CLEAN_FLUSH;
	l1l2cache.l1l2cache_address = address;
	l1l2cache.l1l2cache_size = size;
	BF_L1l2cache_Run( l1l2cache );
}

/**
 *	@brief		This function L1 Cache and L2 Cache Clean,Invalidate.\n
 *	@param		i:l1l2cache_info	Cache inforamtion.
 *	@note		None
 *	@attention	None
 */
VOID BF_L1l2cache_Run( T_L1L2CACHE_INFORMATION l1l2cache_info )
{
	volatile UINT32 stack_space[16];
	stack_space[0] = D_BF_L1CACHE_LIMIT_SIZE;
	stack_space[1] = D_BF_L2CACHE_LIMIT_SIZE;
	
	volatile UINT32 cache_level = 0;
	OS_User_Dis_Dsp();			// Ignore during interrupt
	__asm__ __volatile__(
		"MRC     p15, 2, %0, c0, c0, 0\n\t"
		: "+r" (cache_level) : : );

	switch(l1l2cache_info.l1l2cache_type){
		case E_BF_L1L2CACHE_ALL:
			// L1Cache Operation
			BF_L1l2cache_All_Main(l1l2cache_info.l1l2cache_method);
			// L2Cache Operation
			switch(l1l2cache_info.l1l2cache_method){
				case E_BF_L1L2CACHE_FLUSH:
					Dd_CACHE_L2_Flush_Dcache_All();
					break;
				case E_BF_L1L2CACHE_CLEAN:
					Dd_CACHE_L2_Clean_Dcache_All();
					break;
				case E_BF_L1L2CACHE_CLEAN_FLUSH:
					Dd_CACHE_L2_Clean_Flush_Dcache_All();
					break;
			}
			break;
		case E_BF_L1L2CACHE_ADDR:
			// Set&Way Operation
			if ( (l1l2cache_info.l1l2cache_size >= stack_space[1]) &&
					(l1l2cache_info.l1l2cache_method != E_BF_L1L2CACHE_FLUSH) ){
				// L1Cache Operation
				BF_L1l2cache_All_Main(l1l2cache_info.l1l2cache_method);
				// L2Cache Operation
				switch(l1l2cache_info.l1l2cache_method){
					case E_BF_L1L2CACHE_FLUSH:
						Dd_CACHE_L2_Flush_Dcache_All();
						break;
					case E_BF_L1L2CACHE_CLEAN:
						Dd_CACHE_L2_Clean_Dcache_All();
						break;
					case E_BF_L1L2CACHE_CLEAN_FLUSH:
						Dd_CACHE_L2_Clean_Flush_Dcache_All();
						break;
				}
			}
			else {
				// Address&Size Operation
				switch(l1l2cache_info.l1l2cache_method){
					case E_BF_L1L2CACHE_FLUSH:
						Dd_CACHE_L1_Flush_Dcache_Addr(l1l2cache_info.l1l2cache_address, l1l2cache_info.l1l2cache_size);
						break;
					case E_BF_L1L2CACHE_CLEAN:
						Dd_CACHE_L1_Clean_Dcache_Addr(l1l2cache_info.l1l2cache_address, l1l2cache_info.l1l2cache_size);
						break;
					case E_BF_L1L2CACHE_CLEAN_FLUSH:
						Dd_CACHE_L1_Clean_Flush_Dcache_Addr(l1l2cache_info.l1l2cache_address, l1l2cache_info.l1l2cache_size);
						break;
				}
			}
			break;
	}
	
	__asm__ __volatile__(
		"MCR     p15, 2, %0, c0, c0, 0\n\t"
		: "+r" (cache_level) : : );
	OS_User_Ena_Dsp();
}

/**
 *	@brief		This Main function Cache All.\n
 *	@note		None
 *	@attention	None
 */
VOID BF_L1l2cache_All_Main( E_BF_L1L2CACHE_METHOD method )
{
	UINT32 sr;
	UINT32 cpuid;

	sr = BF_Arm_Lock_Pol(&gBF_L1l2cache_Spin_Lock);

	gBF_Shared_L1l2cache.l1l2cache_cpu[0] = 0;
	gBF_Shared_L1l2cache.l1l2cache_cpu[1] = 0;
	gBF_Shared_L1l2cache.l1l2cache_cpu[2] = 0;
	gBF_Shared_L1l2cache.l1l2cache_method = method;
	Dd_ARM_Dsb_Pou();

	cpuid = Dd_ARM_Get_CPU_ID();
	switch(cpuid){
		case 0:
			while(gBF_Shared_L1l2cache.l1l2cache_cpu[1] == 0)
			{
				Dd_GIC_DIST_Set_Sgir(0x0002000F);
			}
#ifndef CO_RTOS_CPUx2
			while(gBF_Shared_L1l2cache.l1l2cache_cpu[2] == 0)
			{
				Dd_GIC_DIST_Set_Sgir(0x0004000F);
			}
#endif			
			break;
		case 1:
			while(gBF_Shared_L1l2cache.l1l2cache_cpu[0] == 0)
			{
				Dd_GIC_DIST_Set_Sgir(0x0001000F);
			}
#ifndef CO_RTOS_CPUx2
			while(gBF_Shared_L1l2cache.l1l2cache_cpu[2] == 0)
			{
				Dd_GIC_DIST_Set_Sgir(0x0004000F);
			}
#endif			
			break;
#ifndef CO_RTOS_CPUx2
		case 2:
			while(gBF_Shared_L1l2cache.l1l2cache_cpu[0] == 0)
			{
				Dd_GIC_DIST_Set_Sgir(0x0001000F);
			}
			while(gBF_Shared_L1l2cache.l1l2cache_cpu[1] == 0)
			{
				Dd_GIC_DIST_Set_Sgir(0x0002000F);
			}
			break;
#endif			
	}

	switch(gBF_Shared_L1l2cache.l1l2cache_method){
		case E_BF_L1L2CACHE_FLUSH:
			Dd_CACHE_L1_Flush_Dcache_All();
			break;
		case E_BF_L1L2CACHE_CLEAN:
			Dd_CACHE_L1_Clean_Dcache_All();
			break;
		case E_BF_L1L2CACHE_CLEAN_FLUSH:
			Dd_CACHE_L1_Clean_Flush_Dcache_All();
			break;
	}

	BF_Arm_Unlock(&gBF_L1l2cache_Spin_Lock, sr);
}

/**
 *	@brief		This function L1Cache All.\n
 *	@note		None
 *	@attention	None
 */
VOID BF_L1cache_All(void)
{
	UINT32 cpuid;

	switch(gBF_Shared_L1l2cache.l1l2cache_method){
		case E_BF_L1L2CACHE_FLUSH:
			Dd_CACHE_L1_Flush_Dcache_All();
			break;
		case E_BF_L1L2CACHE_CLEAN:
			Dd_CACHE_L1_Clean_Dcache_All();
			break;
		case E_BF_L1L2CACHE_CLEAN_FLUSH:
			Dd_CACHE_L1_Clean_Flush_Dcache_All();
			break;
	}

	cpuid = Dd_ARM_Get_CPU_ID();
	gBF_Shared_L1l2cache.l1l2cache_cpu[cpuid] = 1;
	Dd_ARM_Dsb_Pou();
}
