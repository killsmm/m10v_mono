/**
 * pfdep.h
 *
 *     Copyright (c) 2015 Socionext Inc.
 *     All rights reserved.
 *
 * Sample platform dependent API implementation for micro-ITRON 4.0.
 * THIS SOFTWARE IS PROVIDED WITHOUT ANY WARRANTY.
 */

#ifndef PFDEP_H
#define PFDEP_H

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

/**********************************************************************
 * Include system header files
 **********************************************************************/

/**********************************************************************
 * Elementary type definitions
 **********************************************************************/
typedef char pfdep_int8;
typedef unsigned char pfdep_uint8;
typedef short pfdep_int16;
typedef unsigned short pfdep_uint16;
typedef int pfdep_int32;
typedef unsigned int pfdep_uint32;
typedef long long pfdep_int64;
typedef unsigned long long pfdep_uint64;
typedef int pfdep_bool;
typedef char pfdep_char;

#define PFDEP_TRUE ((pfdep_bool)1)
#define PFDEP_FALSE ((pfdep_bool)0)

/**********************************************************************
 * Complex type definitions
 **********************************************************************/

enum pfdep_err_e {
    PFDEP_ERR_OK = 0,
    PFDEP_ERR_PARAM,
    PFDEP_ERR_ALLOC,
    PFDEP_ERR_INTERRUPT
};

typedef void *pfdep_dev_handle_t;
typedef struct pkt_handle_s pkt_handle_t;
typedef pkt_handle_t *pfdep_pkt_handle_t;

typedef unsigned int pfdep_phys_addr_t;
typedef unsigned int pfdep_cpu_addr_t;

/* Use semaphore for soft lock */
typedef unsigned int pfdep_soft_lock_t;

typedef int pfdep_soft_lock_ctx_t; /* not used */

typedef int pfdep_debug_level_t;

#define PFDEP_DEBUG_LEVEL_FATAL               ((pfdep_debug_level_t)0)
#define PFDEP_DEBUG_LEVEL_WARNING             ((pfdep_debug_level_t)1)
#define PFDEP_DEBUG_LEVEL_NOTICE              ((pfdep_debug_level_t)2)
#define PFDEP_DEBUG_LEVEL_DEBUG               ((pfdep_debug_level_t)3)
#define PFDEP_DEBUG_LEVEL_DEBUG_DETAILED      ((pfdep_debug_level_t)4)
#define PFDEP_DEBUG_LEVEL_DEBUG_MORE_DETAILED ((pfdep_debug_level_t)5)

typedef enum pfdep_err_e pfdep_err_t;

struct pkt_handle_s {
  void *start_addr;
  void *aligned_addr;
  unsigned int pkt_rx_buf_flag;
};

/**********************************************************************
 * Function declarations and definitions
 **********************************************************************/

static __inline void pfdep_iomem_write(void *addr, pfdep_uint32 val)
{
    *((volatile pfdep_uint32 *)addr) = val;
}


static __inline pfdep_uint32 pfdep_iomem_read(void *addr)
{
    return *((volatile pfdep_uint32 *)addr);
}


/*
 * Define functions below properly if your platform requires memory barriers.
 */
#define pfdep_read_mem_barrier()
#define pfdep_write_mem_barrier()
#define pfdep_mem_barrier()

void *pfdep_malloc(pfdep_uint32 len);

void pfdep_free(void *addr);

pfdep_err_t pfdep_dma_malloc (
    pfdep_dev_handle_t dev_handle,
    pfdep_uint32 len,
    void **addr_p,
    pfdep_phys_addr_t *phys_addr_p
    );

void pfdep_dma_free (
    pfdep_dev_handle_t dev_handle,
    pfdep_uint32 len,
    void *addr,
    pfdep_phys_addr_t phys_addr
    );

/*
 * Use semaphore for soft lock.
 */
pfdep_err_t pfdep_init_soft_lock (
    pfdep_soft_lock_t *soft_lock_p
    );

void pfdep_uninit_soft_lock (
    pfdep_soft_lock_t *soft_lock_p
    );

pfdep_err_t pfdep_acquire_soft_lock (
    pfdep_soft_lock_t *soft_lock_p,
    pfdep_soft_lock_ctx_t *ctx_p
    );

void pfdep_release_soft_lock (
    pfdep_soft_lock_t *soft_lock_p,
    pfdep_soft_lock_ctx_t *ctx_p
    );


void pfdep_memcpy(void *dst_p, const void *src_p, pfdep_uint32 len);
void pfdep_memset(void *dst_p, pfdep_uint8 c, pfdep_uint32 len);
pfdep_err_t pfdep_memcmp(unsigned char *data1,
                         unsigned char *data2,
                         pfdep_uint32 len);

pfdep_err_t pfdep_msleep(pfdep_uint32 wait_ms);

pfdep_err_t pfdep_alloc_pkt_buf (
    pfdep_dev_handle_t dev_handle,
    pfdep_uint32 len,
    void **addr_p,
    pfdep_phys_addr_t *phys_addr_p,
    pfdep_pkt_handle_t *pkt_handle_p
    );

void pfdep_free_pkt_buf (
    pfdep_dev_handle_t dev_handle,
    pfdep_uint16 len,
    void *addr,
    pfdep_phys_addr_t phys_addr,
    pfdep_bool last_flag,
    pfdep_pkt_handle_t pkt_handle
    );

void pfdep_reset_mem_pool(pfdep_uint32 mem_start_addr,
						  pfdep_uint32 dma_mem_cpu_start_addr,
						  pfdep_uint32 dma_mem_phys_start_addr);

void pfdep_invalidate_dcache_addr( UINT32 address, UINT32 size );
void pfdep_clean_invalidate_dcache_addr( UINT32 address, UINT32 size );
void pfdep_invalidate_dcache_all(void);
void pfdep_clean_invalidate_dcache_all(void);
	
/*
 * Assume that no debugging functions are required.
 */
/* #define pfdep_print(...) */

#define pfdep_print(level,...)					\
do { \
    if (level <= PFDEP_DEBUG_LEVEL_FATAL) { \
        printf("[OGMA]" __VA_ARGS__); \
    } \
} while (0)

#define pfdep_get_debug_level() (PFDEP_DEBUG_LEVEL_FATAL)
#define pfdep_set_debug_level(level)
#define pfdep_assert(cond)


#endif /* PFDEP_H */
