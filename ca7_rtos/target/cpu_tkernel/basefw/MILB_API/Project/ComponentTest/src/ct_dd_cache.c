/**
 * @file		ct_dd_cache.c
 * @brief		dd_cache Component Test
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "driver_common.h"
#include "dd_cache.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Test Memory map.
// Please setting of mmu before test.
//
//   0x6500_0000 +-------------+-------------+
//               | INNER_NC    | OUTER_NC    |
//   0x6510_0000 +-------------+-------------+
//               | INNER_WB_WA | OUTER_NC    |
//   0x6520_0000 +-------------+-------------+
//               | INNER_NC    | OUTER_NC    |
//   0x6530_0000 +-------------+-------------+
//               | INNER_WB_WA | OUTER_NC    |
//   0x6540_0000 +-------------+-------------+
//
#define D_CT_DD_CACHE_NONCACHE_START_ADDR		(0x65000000)	// NONCache Test Top Address
#define D_CT_DD_CACHE_L1CACHE_START_ADDR		(0x65100000)	// L1Cache Test Top Address
#define D_CT_DD_CACHE_L2CACHE_START_ADDR		(0x65200000)	// L2Cache Test Top Address
#define D_CT_DD_CACHE_L1L2CACHE_START_ADDR		(0x65300000)	// L1L2Cache Test Top Address
#define D_CT_DD_CACHE_SIZE						(0x00010000)	// Cache Test Size

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static VOID cache_area_write_data(UINT32 start_addr, UINT32 write_data)
{
	UINT32 write_address;
	
	write_address = start_addr;
	while(write_address < (start_addr + D_CT_DD_CACHE_SIZE)) {
		memset((void*)write_address, write_data, 4);
		write_address += 4;
	}
}

static VOID cache_area_write(UINT32 write_data)
{
	cache_area_write_data(D_CT_DD_CACHE_NONCACHE_START_ADDR, write_data);
	cache_area_write_data(D_CT_DD_CACHE_L1CACHE_START_ADDR, write_data);
	cache_area_write_data(D_CT_DD_CACHE_L2CACHE_START_ADDR, write_data);
	cache_area_write_data(D_CT_DD_CACHE_L1L2CACHE_START_ADDR, write_data);
}

static VOID cache_area_memcmp(VOID)
{
	Ddim_Print(("NONCACHE Result = %d\n", memcmp((void*)D_CT_DD_CACHE_NONCACHE_START_ADDR, (void*)D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE)));
	Ddim_Print(("L1CACHE Result = %d\n", memcmp((void*)D_CT_DD_CACHE_L1CACHE_START_ADDR, (void*)D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE)));
	Ddim_Print(("L2CACHE Result = %d\n", memcmp((void*)D_CT_DD_CACHE_L2CACHE_START_ADDR, (void*)D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE)));
	Ddim_Print(("L1L2CACHE Result = %d\n", memcmp((void*)D_CT_DD_CACHE_L1L2CACHE_START_ADDR, (void*)D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE)));
}

static VOID cache_area_init(VOID)
{
	Ddim_Print(("Cache area initialize start\n"));
	memset((void*)D_CT_DD_CACHE_NONCACHE_START_ADDR, 0x00, D_CT_DD_CACHE_SIZE);
	memset((void*)D_CT_DD_CACHE_L1CACHE_START_ADDR, 0x00, D_CT_DD_CACHE_SIZE);
	memset((void*)D_CT_DD_CACHE_L2CACHE_START_ADDR, 0x00, D_CT_DD_CACHE_SIZE);
	memset((void*)D_CT_DD_CACHE_L1L2CACHE_START_ADDR, 0x00, D_CT_DD_CACHE_SIZE);
	Dd_CACHE_L1_Clean_Flush_Dcache_All();
	Dd_CACHE_L2_Clean_Flush_Dcache_All();
	Ddim_Print(("NONCACHE Result = %d\n", memcmp((void*)D_CT_DD_CACHE_NONCACHE_START_ADDR, (void*)D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE)));
	Ddim_Print(("L1CACHE Result = %d\n", memcmp((void*)D_CT_DD_CACHE_L1CACHE_START_ADDR, (void*)D_CT_DD_CACHE_L1CACHE_START_ADDR, D_CT_DD_CACHE_SIZE)));
	Ddim_Print(("L2CACHE Result = %d\n", memcmp((void*)D_CT_DD_CACHE_L2CACHE_START_ADDR, (void*)D_CT_DD_CACHE_L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE)));
	Ddim_Print(("L1L2CACHE Result = %d\n", memcmp((void*)D_CT_DD_CACHE_L1L2CACHE_START_ADDR, (void*)D_CT_DD_CACHE_L1L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE)));
	Ddim_Print(("Cache area initialize end\n"));
}

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
 * @brief  Command main function for CACHE test.
 * @param  int argc	:The number of parameters
 *		   char** argv	:The value of parameters
 * @return VOID
 *
 *			The meaning of parameters
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| P1              | P2   | P3   |Meaning                                                    |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l1all_f         |      |      |L1 Cache Flush All Test.                                   |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l1all_c         |      |      |L1 Cache Clean All Test.                                   |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l1all_cf        |      |      |L1 Cache Clean & Flush All Test.                           |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l2all_f         |      |      |L2 Cache Flush All Test.                                   |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l2all_c         |      |      |L2 Cache Clean All Test.                                   |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l2all_cf        |      |      |L2 Cache Clean & Flush All Test.                           |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l1l2all_f       |      |      |L1L2 Cache Flush All Test.                                 |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l1l2all_c       |      |      |L1L2 Cache Clean All Test.                                 |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l1l2all_cf      |      |      |L1L2 Cache Clean & Flush All Test.                         |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l1addr_f        |      |      |L1 Cache Flush Address/Size Test.                          |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l1addr_c        |      |      |L1 Cache Clean Address/Size Test.                          |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l1addr_cf       |      |      |L1 Cache Clean & Flush Address/Size Test.                  |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l2addr_f        |      |      |L2 Cache Flush Address/Size Test.                          |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l2addr_c        |      |      |L2 Cache Clean Address/Size Test.                          |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l2addr_cf       |      |      |L2 Cache Clean & Flush Address/Size Test.                  |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l1l2addr_f      |      |      |L1L2 Cache Flush Address/Size Test.                        |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l1l2addr_c      |      |      |L1L2 Cache Clean Address/Size Test.                        |
 *		+-----------------+------+------+-----------------------------------------------------------+
 *		| l1l2addr_cf     |      |      |L1L2 Cache Clean & Flush Address/Size Test.                |
 *		+-----------------+------+------+-----------------------------------------------------------+
 */
VOID Ct_Dd_CACHE_Main(int argc, char** argv)
{
	if( argc < 2 ) {
		Ddim_Print(("please check 1st parameter!!\n"));
	}
	else if( 0 == strcmp( argv[1], "l1all_f" ) ) {
		Ddim_Print(("Dd_CACHE_L1_Flush_Dcache_All start\n"));
		cache_area_init();
		cache_area_write(0x11111111);
		Dd_CACHE_L1_Flush_Dcache_All();
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L1_Flush_Dcache_All end\n"));
	}
	else if( 0 == strcmp( argv[1], "l1all_c" ) ) {
		Ddim_Print(("Dd_CACHE_L1_Clean_Dcache_All start\n"));
		cache_area_init();
		cache_area_write(0x22222222);
		Dd_CACHE_L1_Clean_Dcache_All();
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L1_Clean_Dcache_All end\n"));
	}
	else if( 0 == strcmp( argv[1], "l1all_cf" ) ) {
		Ddim_Print(("Dd_CACHE_L1_Clean_Flush_Dcache_All start\n"));
		cache_area_init();
		cache_area_write(0x33333333);
		Dd_CACHE_L1_Clean_Flush_Dcache_All();
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L1_Clean_Flush_Dcache_All end\n"));
	}
	else if( 0 == strcmp( argv[1], "l2all_f" ) ) {
		Ddim_Print(("Dd_CACHE_L2_Flush_Dcache_All start\n"));
		cache_area_init();
		cache_area_write(0x44444444);
		Dd_CACHE_L2_Flush_Dcache_All();
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L2_Flush_Dcache_All end\n"));
	}
	else if( 0 == strcmp( argv[1], "l2all_c" ) ) {
		Ddim_Print(("Dd_CACHE_L2_Clean_Dcache_All start\n"));
		cache_area_init();
		cache_area_write(0x55555555);
		Dd_CACHE_L2_Clean_Dcache_All();
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L2_Clean_Dcache_All end\n"));
	}
	else if( 0 == strcmp( argv[1], "l2all_cf" ) ) {
		Ddim_Print(("Dd_CACHE_L2_Clean_Flush_Dcache_All start\n"));
		cache_area_init();
		cache_area_write(0x66666666);
		Dd_CACHE_L2_Clean_Flush_Dcache_All();
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L2_Clean_Flush_Dcache_All end\n"));
	}
	else if( 0 == strcmp( argv[1], "l1l2all_f" ) ) {
		Ddim_Print(("Dd_CACHE_L1_Flush_Dcache_All / Dd_CACHE_L2_Flush_Dcache_All start\n"));
		cache_area_init();
		cache_area_write(0x77777777);
		Dd_CACHE_L1_Flush_Dcache_All();
		Dd_CACHE_L2_Flush_Dcache_All();
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L1_Flush_Dcache_All / Dd_CACHE_L2_Flush_Dcache_All end\n"));
	}
	else if( 0 == strcmp( argv[1], "l1l2all_c" ) ) {
		Ddim_Print(("Dd_CACHE_L1_Clean_Dcache_All / Dd_CACHE_L2_Clean_Dcache_All start\n"));
		cache_area_init();
		cache_area_write(0x88888888);
		Dd_CACHE_L1_Clean_Dcache_All();
		Dd_CACHE_L2_Clean_Dcache_All();
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L1_Clean_Dcache_All / Dd_CACHE_L2_Clean_Dcache_All end\n"));
	}
	else if( 0 == strcmp( argv[1], "l1l2all_cf" ) ) {
		Ddim_Print(("Dd_CACHE_L1_Clean_Flush_Dcache_All / Dd_CACHE_L2_Clean_Flush_Dcache_All start\n"));
		cache_area_init();
		cache_area_write(0x99999999);
		Dd_CACHE_L1_Clean_Flush_Dcache_All();
		Dd_CACHE_L2_Clean_Flush_Dcache_All();
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L1_Clean_Flush_Dcache_All / Dd_CACHE_L2_Clean_Flush_Dcache_All end\n"));
	}
	else if( 0 == strcmp( argv[1], "l1addr_f" ) ) {
		Ddim_Print(("Dd_CACHE_L1_Flush_Dcache_Addr start\n"));
		cache_area_init();
		cache_area_write(0xAAAAAAAA);
		Dd_CACHE_L1_Flush_Dcache_Addr(D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Flush_Dcache_Addr(D_CT_DD_CACHE_L1CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Flush_Dcache_Addr(D_CT_DD_CACHE_L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Flush_Dcache_Addr(D_CT_DD_CACHE_L1L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L1_Flush_Dcache_Addr end\n"));
	}
	else if( 0 == strcmp( argv[1], "l1addr_c" ) ) {
		Ddim_Print(("Dd_CACHE_L1_Clean_Dcache_Addr start\n"));
		cache_area_init();
		cache_area_write(0xBBBBBBBB);
		Dd_CACHE_L1_Clean_Dcache_Addr(D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Clean_Dcache_Addr(D_CT_DD_CACHE_L1CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Clean_Dcache_Addr(D_CT_DD_CACHE_L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Clean_Dcache_Addr(D_CT_DD_CACHE_L1L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L1_Clean_Dcache_Addr end\n"));
	}
	else if( 0 == strcmp( argv[1], "l1addr_cf" ) ) {
		Ddim_Print(("Dd_CACHE_L1_Clean_Flush_Dcache_Addr start\n"));
		cache_area_init();
		cache_area_write(0xCCCCCCCC);
		Dd_CACHE_L1_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_L1CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_L1L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L1_Clean_Flush_Dcache_Addr end\n"));
	}
	else if( 0 == strcmp( argv[1], "l2addr_f" ) ) {
		Ddim_Print(("Dd_CACHE_L2_Flush_Dcache_Addr start\n"));
		cache_area_init();
		cache_area_write(0xDDDDDDDD);
		Dd_CACHE_L2_Flush_Dcache_Addr(D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Flush_Dcache_Addr(D_CT_DD_CACHE_L1CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Flush_Dcache_Addr(D_CT_DD_CACHE_L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Flush_Dcache_Addr(D_CT_DD_CACHE_L1L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L2_Flush_Dcache_Addr end\n"));
	}
	else if( 0 == strcmp( argv[1], "l2addr_c" ) ) {
		Ddim_Print(("Dd_CACHE_L2_Clean_Dcache_Addr start\n"));
		cache_area_init();
		cache_area_write(0xEEEEEEEE);
		Dd_CACHE_L2_Clean_Dcache_Addr(D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Clean_Dcache_Addr(D_CT_DD_CACHE_L1CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Clean_Dcache_Addr(D_CT_DD_CACHE_L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Clean_Dcache_Addr(D_CT_DD_CACHE_L1L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L2_Clean_Dcache_Addr end\n"));
	}
	else if( 0 == strcmp( argv[1], "l2addr_cf" ) ) {
		Ddim_Print(("Dd_CACHE_L2_Clean_Flush_Dcache_Addr start\n"));
		cache_area_init();
		cache_area_write(0xFFFFFFFF);
		Dd_CACHE_L2_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_L1CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_L1L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L2_Clean_Flush_Dcache_Addr end\n"));
	}
	else if( 0 == strcmp( argv[1], "l1l2addr_f" ) ) {
		Ddim_Print(("Dd_CACHE_L1_Flush_Dcache_Addr / Dd_CACHE_L2_Flush_Dcache_Addr start\n"));
		cache_area_init();
		cache_area_write(0x1F1F1F1F);
		Dd_CACHE_L1_Flush_Dcache_Addr(D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Flush_Dcache_Addr(D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Flush_Dcache_Addr(D_CT_DD_CACHE_L1CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Flush_Dcache_Addr(D_CT_DD_CACHE_L1CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Flush_Dcache_Addr(D_CT_DD_CACHE_L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Flush_Dcache_Addr(D_CT_DD_CACHE_L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Flush_Dcache_Addr(D_CT_DD_CACHE_L1L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Flush_Dcache_Addr(D_CT_DD_CACHE_L1L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L1_Flush_Dcache_Addr / Dd_CACHE_L2_Flush_Dcache_Addr end\n"));
	}
	else if( 0 == strcmp( argv[1], "l1l2addr_c" ) ) {
		Ddim_Print(("Dd_CACHE_L1_Clean_Dcache_Addr / Dd_CACHE_L2_Clean_Dcache_Addr start\n"));
		cache_area_init();
		cache_area_write(0x2F2F2F2F);
		Dd_CACHE_L1_Clean_Dcache_Addr(D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Clean_Dcache_Addr(D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Clean_Dcache_Addr(D_CT_DD_CACHE_L1CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Clean_Dcache_Addr(D_CT_DD_CACHE_L1CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Clean_Dcache_Addr(D_CT_DD_CACHE_L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Clean_Dcache_Addr(D_CT_DD_CACHE_L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Clean_Dcache_Addr(D_CT_DD_CACHE_L1L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Clean_Dcache_Addr(D_CT_DD_CACHE_L1L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L1_Clean_Dcache_Addr / Dd_CACHE_L2_Clean_Dcache_Addr end\n"));
	}
	else if( 0 == strcmp( argv[1], "l1l2addr_cf" ) ) {
		Ddim_Print(("Dd_CACHE_L1_Clean_Flush_Dcache_Addr / Dd_CACHE_L2_Clean_Flush_Dcache_Addr start\n"));
		cache_area_init();
		cache_area_write(0x3F3F3F3F);
		Dd_CACHE_L1_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_NONCACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_L1CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_L1CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L1_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_L1L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		Dd_CACHE_L2_Clean_Flush_Dcache_Addr(D_CT_DD_CACHE_L1L2CACHE_START_ADDR, D_CT_DD_CACHE_SIZE);
		cache_area_memcmp();
		Ddim_Print(("Dd_CACHE_L1_Clean_Flush_Dcache_Addr / Dd_CACHE_L2_Clean_Flush_Dcache_Addr end\n"));
	}
	else
	{
		Ddim_Print(("please check 1st parameter!!\n"));
	}
}

