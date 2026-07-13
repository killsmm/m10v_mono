/**
 * ogma_basic_access.c
 *
 *     Copyright 2015 Socionext Inc.
 *     All rights reserved.
 *
 **/

#include "pfdep.h"
#include <stdlib.h>
#include <stdio.h>
#include "os_user_custom.h"
#include "memory.h"
#include "debug.h"

#include "ogma_config.h"

/**********************************************************************
 * Local variable definitions
 **********************************************************************/
#define OGMA_MEM_ALLOC_LEN     (0x20000)
#define OGMA_PKT_MEM_ALLOC_LEN (0x80000)
#define OGMA_DMA_MEM_ALLOC_LEN (0x2000)

static pfdep_uint32 pfdep_ogma_mem_start_addr;
static pfdep_uint32 pfdep_ogma_pkt_mem_start_addr;
static pfdep_uint32 pfdep_ogma_dma_mem_start_addr;
static pfdep_uint32 pfdep_ogma_mem_end_addr;
static pfdep_uint32 pfdep_ogma_pkt_mem_end_addr;
static pfdep_uint32 pfdep_ogma_dma_mem_end_addr;
static pfdep_uint32 pfdep_ogma_dma_mem_cpu_addr_offset;

/**
 * Memory pool.
 *
 * We assume that reuse of freed area is not necessary.
 */

pfdep_uint8 *next_mem_pool_p      = 0;
pfdep_uint8 *next_pkt_mem_pool_p  = 0;

/**
 * Cache coherent memory pool.
 *
 * We assume that reuse of freed area is not necessary.
 */
pfdep_uint8 *next_cc_mem_pool_p   = 0;

/**********************************************************************
 * Function definitions
 **********************************************************************/


void *pfdep_malloc(pfdep_uint32 len)
{

    pfdep_uint32 avail_len;
    void *result;

    if (((pfdep_cpu_addr_t)next_mem_pool_p & 0xf) != 0) {
        next_mem_pool_p += 0x10 - (((pfdep_cpu_addr_t)next_mem_pool_p & 0xf));
    }

    avail_len =
        (pfdep_uint32)(((pfdep_cpu_addr_t)pfdep_ogma_mem_end_addr)
					   - (pfdep_cpu_addr_t)next_mem_pool_p);
	
	if(len > avail_len) {
 		BF_Debug_Print_Error(("Error: pfdep_malloc\n"));
		next_mem_pool_p = (pfdep_uint8 *) pfdep_ogma_mem_start_addr;
		return NULL;
	}

    result = (void *)next_mem_pool_p;

    next_mem_pool_p += len;

    return result;

}


void pfdep_free(void *addr /* not used */)
{
  //printf("pfdep_free(0x%p)\n",addr);
}

/*
 * Assume that this platform is not equipped with MMU 
 * and no special setting is necessary for DMA to access the allocated area.
 */
pfdep_err_t pfdep_dma_malloc (
    pfdep_dev_handle_t dev_handle, /* not used */
    pfdep_uint32 len,
    void **addr_p,
    pfdep_phys_addr_t *phys_addr_p
    )
{

    pfdep_uint32 avail_len;
	
    if (((pfdep_cpu_addr_t)next_cc_mem_pool_p & 0xf) != 0) {
        next_cc_mem_pool_p +=
            0x10 - (((pfdep_cpu_addr_t)next_cc_mem_pool_p & 0xf));
    }

    avail_len =
        ((pfdep_cpu_addr_t)pfdep_ogma_dma_mem_end_addr)
        - (pfdep_cpu_addr_t)next_cc_mem_pool_p;

    if (len > avail_len) {
		BF_Debug_Print_Error(("Error: pfdep_dma_malloc\n"));
		next_cc_mem_pool_p = (pfdep_uint8 *) pfdep_ogma_dma_mem_start_addr;
        return PFDEP_ERR_ALLOC;
    }

    *addr_p = (void *)next_cc_mem_pool_p;
    *phys_addr_p = (pfdep_phys_addr_t)(next_cc_mem_pool_p - pfdep_ogma_dma_mem_cpu_addr_offset);
    next_cc_mem_pool_p += len;
    return PFDEP_ERR_OK;

}

void pfdep_dma_free (
    pfdep_dev_handle_t dev_handle, /* not used */
    pfdep_uint32 len, /* not used */
    void *addr,
    pfdep_phys_addr_t phys_addr /* not used */
    )
{
  //printf("pfdep_dma_free(0x%p)\n",addr);
}

pfdep_err_t pfdep_init_soft_lock (
    pfdep_soft_lock_t *soft_lock_p
    )
{
  return PFDEP_ERR_OK;
}

void pfdep_uninit_soft_lock (
    pfdep_soft_lock_t *soft_lock_p
    )
{
    /* No reuse is supported. */
    ;
}

pfdep_err_t pfdep_acquire_soft_lock (
    pfdep_soft_lock_t *soft_lock_p,
    pfdep_soft_lock_ctx_t *ctx_p /* not used */
    )
{
  return PFDEP_ERR_OK;
}

void pfdep_release_soft_lock (
    pfdep_soft_lock_t *soft_lock_p,
    pfdep_soft_lock_ctx_t *ctx_p /* not used */
    )
{
  //sig_sem(*soft_lock_p);
}

void pfdep_memcpy(void *dst_p, const void *src_p, pfdep_uint32 len)
{
    pfdep_uint32 i;
    pfdep_uint8 *work_dst_p = (pfdep_uint8 *)dst_p;
    const pfdep_uint8 *work_src_p = (const pfdep_uint8 *)src_p;

    for (i = 0; i < len; i++) {
        *work_dst_p = *work_src_p;
        ++work_dst_p;
        ++work_src_p;
    }
}

pfdep_err_t pfdep_memcmp(unsigned char *data1,
                         unsigned char *data2,
                         pfdep_uint32 len)
{
  pfdep_uint32 i;
  for (i=0;i<len;i++) {
    if(*(data1+i) != *(data2+i)) {
      return PFDEP_ERR_PARAM;
    }
  }
  return PFDEP_ERR_OK;
}

void pfdep_memset(void *dst_p, pfdep_uint8 c, pfdep_uint32 len)
{
    pfdep_uint32 i;
    pfdep_uint8 *work_dst_p = (pfdep_uint8 *)dst_p;

    for (i = 0; i < len; i++) {
        *work_dst_p = c;
        ++work_dst_p;
    }
}

pfdep_err_t pfdep_msleep(pfdep_uint32 wait_ms)
{
    OS_User_Dly_Tsk(wait_ms);

    return PFDEP_ERR_OK;
}

pfdep_err_t pfdep_alloc_pkt_buf (
    pfdep_dev_handle_t dev_handle,
    pfdep_uint32 len,
    void **addr_p,
    pfdep_phys_addr_t *phys_addr_p,
    pfdep_pkt_handle_t *pkt_handle_p
	)
{
    pfdep_uint32 avail_len;
	pkt_handle_t *pkt_handle;
	pkt_handle = pfdep_malloc(sizeof(pkt_handle_t));
  
	if (((pfdep_cpu_addr_t)next_pkt_mem_pool_p & 0xf) != 0) {
		next_pkt_mem_pool_p += 0x10 - (((pfdep_cpu_addr_t)next_pkt_mem_pool_p & 0xf));
	}
	
	avail_len =
		(pfdep_uint32)(((pfdep_cpu_addr_t)pfdep_ogma_pkt_mem_end_addr)
					   - (pfdep_cpu_addr_t)next_pkt_mem_pool_p);
	
	if(len > avail_len) {
		BF_Debug_Print_Error(("Error: pfdep_alloc_pkt_buf\n"));
		next_pkt_mem_pool_p = (pfdep_uint8 *) pfdep_ogma_pkt_mem_start_addr;
		return PFDEP_ERR_ALLOC;
	}

	*addr_p = (void *)next_pkt_mem_pool_p;
	*phys_addr_p = (pfdep_phys_addr_t) *addr_p;
	pkt_handle->start_addr = (void *)next_pkt_mem_pool_p;
	*pkt_handle_p = pkt_handle;

	pfdep_memset(*addr_p, 0 , len);
	next_pkt_mem_pool_p += len;

	return PFDEP_ERR_OK;
}


void pfdep_free_pkt_buf (
    pfdep_dev_handle_t dev_handle,
    pfdep_uint16 len,
    void *addr,
    pfdep_phys_addr_t phys_addr,
    pfdep_bool last_flag,
    pfdep_pkt_handle_t pkt_handle
    )
{

}

void pfdep_reset_mem_pool(pfdep_uint32 mem_start_addr,
						  pfdep_uint32 dma_mem_cpu_start_addr,
						  pfdep_uint32 dma_mem_phys_start_addr
	)	
{
	pfdep_ogma_mem_start_addr = mem_start_addr;
	pfdep_ogma_pkt_mem_start_addr = mem_start_addr + OGMA_MEM_ALLOC_LEN;
	pfdep_ogma_dma_mem_start_addr = dma_mem_cpu_start_addr;
	pfdep_ogma_dma_mem_cpu_addr_offset = dma_mem_cpu_start_addr - dma_mem_phys_start_addr;
		
	pfdep_ogma_mem_end_addr = pfdep_ogma_mem_start_addr + (pfdep_uint32)OGMA_MEM_ALLOC_LEN;
	pfdep_ogma_pkt_mem_end_addr = pfdep_ogma_pkt_mem_start_addr + (pfdep_uint32)OGMA_PKT_MEM_ALLOC_LEN;
	pfdep_ogma_dma_mem_end_addr = pfdep_ogma_dma_mem_start_addr + (pfdep_uint32)OGMA_DMA_MEM_ALLOC_LEN;
		
	next_mem_pool_p     = (pfdep_uint8 *) pfdep_ogma_mem_start_addr;
	next_pkt_mem_pool_p = (pfdep_uint8 *) pfdep_ogma_pkt_mem_start_addr;
	next_cc_mem_pool_p  = (pfdep_uint8 *) pfdep_ogma_dma_mem_start_addr;
	
}

void pfdep_invalidate_dcache_addr( UINT32 address, UINT32 size )
{
	fj_memory_dcache_addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_FLUSH, address, size );
}

void pfdep_clean_dcache_addr( UINT32 address, UINT32 size )
{
	fj_memory_dcache_addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN, address, size );
}

void pfdep_clean_invalidate_dcache_addr( UINT32 address, UINT32 size )
{
	fj_memory_dcache_addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, address, size );
}

void pfdep_clean_invalidate_dcache_all(void)
{
	fj_memory_dcache_all( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH );
}
