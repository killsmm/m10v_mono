/**
 * @file		vg_if.c
 * @brief		GPU IF User I/F
 * @author		hat
 * @since		2016/07/15
 * @note		None
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; Socionext Inc. 2015-2016</I></B>
 */

#include "fj_memory.h"
#include "os_user_custom.h"
#include "dd_top.h"

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/
#define D_IM_GPU_HCLK_REG_ADDR		(&Dd_Top_Get_CLKSTOP4())

#define ES1_BUG
#define	GPU_HCLK

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Global Data																			*/
/*--------------------------------------------------------------------------------------*/
volatile UCHAR gIM_Gpu_Hclk_Ctrl_Cnt = 0;

/*--------------------------------------------------------------------------------------*/
/* Macro																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Local Method Definition																*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Table																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Function																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* API																					*/
/*--------------------------------------------------------------------------------------*/
/**
 * @brief         D-cache Clean/Flush
 * @param[in]     unsigned int		addr		: start address
 * @param[in]     unsigned int		size		: size
 * @return        None
 * @note          
 * @attention     
 */
void GPU_IF_User_CleanFlush_Dcache( unsigned int addr, unsigned int size )
{
	FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr, size );
}

/**
 * @brief         Delay Task
 * @param[in]     unsigned int		msec		: delay time(ms)
 * @return        None
 * @note          
 * @attention     
 */
void GPU_IF_User_Dly_Tsk( unsigned int msec )
{
	OS_User_Dly_Tsk( (OS_USER_RELTIM)msec );
}

/**
 * @brief         HCLK on
 * @param[in]     None
 * @return        None
 * @note          
 * @attention     
 */
void GPU_IF_on_hclk(void)
{
#ifdef ES1_BUG
	DDIM_User_AhbReg_SpinLock();	//	<== before HCLK ON
#endif
#ifdef GPU_HCLK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_Gpu_Hclk_Ctrl_Cnt, D_IM_GPU_HCLK_REG_ADDR, ~D_DD_TOP_GPUAH_BIT );
#endif
}

/**
 * @brief         HCLK off
 * @param[in]     None
 * @return        None
 * @note          
 * @attention     
 */
void GPU_IF_off_hclk(void)
{
#ifdef GPU_HCLK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_Gpu_Hclk_Ctrl_Cnt, D_IM_GPU_HCLK_REG_ADDR, D_DD_TOP_GPUAH_BIT );
#endif
#ifdef ES1_BUG
	DDIM_User_AhbReg_SpinUnLock();	//	<== after HCLK OFF
#endif
}
