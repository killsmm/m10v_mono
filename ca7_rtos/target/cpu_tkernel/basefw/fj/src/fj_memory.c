/**
 * @file		fj_memory.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_memory.h"
#include "memory.h"

#ifndef TRUE
#define TRUE 1
#endif

/******************************************************************************
 Program
******************************************************************************/
FJ_ERR_CODE FJ_SetSDRAMMapMode( FJ_SDRAM_MAP_MODE wSdramMapMode )
{
	return fj_setsdrammapmode(wSdramMapMode);
}

UINT32	FJ_DirectPoolAlloc( ULONG ulSizeInBytes )
{
	return fj_directpoolalloc( ulSizeInBytes );
}

BOOL	FJ_DirectPoolFree( UINT32 pbyBuffer )
{
	BOOL ret = FALSE;
	ret = fj_directpoolfree(pbyBuffer);
	return ret;
}

FJ_ERR_CODE	FJ_GetMem( FJ_MEMORY_LOCATION wLocation, UINT32* pBuffer, UINT32 *pdwSize )
{
	FJ_ERR_CODE	ret;

	ret = (FJ_ERR_CODE)fj_getmem( wLocation, pBuffer, pdwSize );

	return ret;
}

FJ_ERR_CODE	FJ_ReleaseMem( FJ_MEMORY_LOCATION wLocation,  UINT32 Buffer )
{
	FJ_ERR_CODE	ret;

	ret = (FJ_ERR_CODE)fj_releasemem( wLocation, Buffer );

	return ret;
}

FJ_ERR_CODE	FJ_ClearMem( FJ_MEMORY_LOCATION wLocation )
{
	return fj_clearmem( wLocation );
}

VOID	FJ_MEMORY_Dcache_All( FJ_MEMORY_CACHE_LEVEL level, FJ_MEMORY_CACHE_OPE operation )
{
	fj_memory_dcache_all( level, operation );
}

VOID	FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_LEVEL level, FJ_MEMORY_CACHE_OPE operation, UINT32 address, UINT32 size )
{
	fj_memory_dcache_addr( level, operation, address, size );
}

VOID	FJ_Get_Memory_Attribute( UINT32 addr, U_MEMORY_ATTRIBUTE *attribute )
{
	fj_get_memory_attribute( addr, attribute );
}

VOID	FJ_MEMORY_DMB_POC( VOID )
{
	fj_memory_dmb_poc();
}

VOID	FJ_MEMORY_DSB_POC( VOID )
{
	fj_memory_dsb_poc();
}

VOID	FJ_MEMORY_DSB_POU( VOID )
{
	fj_memory_dsb_pou();
}

INT32	FJ_Get_MPF_FBLKCNT( OS_USER_ID mpfid )
{
	return fj_get_mpf_fblkcnt(mpfid);
}

INT32	FJ_Get_MPL_FMPLSZ( OS_USER_ID mplid )
{
	return fj_get_mpl_fmplsz(mplid);
}

INT32	FJ_Get_HEAP_SIZE( VOID )
{
	return fj_get_heap_size();
}

FJ_ERR_CODE	FJ_MEMORY_Copy_Sdram_Sync(FJ_MEMORY_COPY_DMA_CH ch, UINT32 src_addr, UINT32 dst_addr, UINT32 size, UINT32 wait_mode)
{
	return fj_memory_copy_sdram_sync(ch, src_addr, dst_addr, size, wait_mode);
}

/*@} --- end of fj_memory_api */
