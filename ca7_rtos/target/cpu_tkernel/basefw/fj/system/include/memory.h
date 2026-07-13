/**
 * @file		memory.h
 * @brief		Memory control
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "fj_memory.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	FJ_PARAM_ARRAY_NUM					(2)
#define	FJ_PARAM_ARRAY_SIZE					(100)

#define	FJ_FPARAMS_NUM						(2)
#define	FJ_FPARAMS_SIZE						(100)

#define	FJ_TEXT_OUT_SIZE					(100)
#define	FJ_DRAW_TEXT_SIZE					(100)
#define FJ_ICON_LOAD_SIZE					(SDRAM_ADR_ICON_DATA_END - SDRAM_ADR_ICON_DATA)

#define	FJ_DPOF_GET_JOB_PARAM_STR_NUM		(1)
#define	FJ_DPOF_GET_JOB_PARAM_STR_SIZE		(100)

#define	FJ_OSD_DRAW_BUFFER_VGR_EX_WIDTH     (640)
#define	FJ_OSD_DRAW_BUFFER_VGR_EX_LINES     (480)
#define	FJ_OSD_DRAW_BUFFER_VGR_EX_HWIDTH    (640)
#define	FJ_OSD_DRAW_BUFFER_VGR_EX_HLINES    (480)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

FJ_ERR_CODE fj_setsdrammapmode( FJ_SDRAM_MAP_MODE wSdramMapMode );

UINT32 fj_directpoolalloc( ULONG ulSizeInBytes );
BOOL fj_directpoolfree( UINT32 pbyBuffer );
FJ_ERR_CODE	fj_getmem( FJ_MEMORY_LOCATION wLocation, UINT32* pBuffer, UINT32 *pdwSize );
FJ_ERR_CODE	fj_releasemem( FJ_MEMORY_LOCATION wLocation, UINT32 Buffer );
FJ_ERR_CODE	fj_clearmem( FJ_MEMORY_LOCATION wLocation );

VOID	fj_memory_dmb_poc( VOID );

VOID	fj_memory_dsb_poc( VOID );

VOID	fj_memory_dcache_all( FJ_MEMORY_CACHE_LEVEL level, FJ_MEMORY_CACHE_OPE operation );

VOID	fj_memory_dcache_addr( FJ_MEMORY_CACHE_LEVEL level, FJ_MEMORY_CACHE_OPE operation, UINT32 address, UINT32 size );

VOID	fj_get_memory_attribute( UINT32 addr, U_MEMORY_ATTRIBUTE *attribute );

VOID	fj_memory_dmb_poc( VOID );

VOID	fj_memory_dsb_poc( VOID );

VOID	fj_memory_dsb_pou( VOID );

INT32	fj_get_mpf_fblkcnt( OS_USER_ID mpfid );

INT32	fj_get_mpl_fmplsz( OS_USER_ID mplid );

INT32	fj_get_heap_size( VOID );

FJ_ERR_CODE	fj_memory_copy_sdram_sync(FJ_MEMORY_COPY_DMA_CH ch, ULONG src_addr, ULONG dst_addr, ULONG size, UINT32 wait_mode);

#ifdef __cplusplus
}
#endif

#endif	// _MEMORY_H_
