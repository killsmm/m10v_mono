/**
 * @file		vg_if.h
 * @brief		GPU IF User I/F
 * @author		hat
 * @since		2016/06/22
 * @note		None
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; Socionext Inc. 2015</I></B>
 */

#ifndef _VG_IF_H_
#define _VG_IF_H_

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Global Data																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Macro																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* API		  																			*/
/*--------------------------------------------------------------------------------------*/
/**
 * @brief         D-cache Clean/Flush
 * @param[in]     unsigned int		addr		: start address
 * @param[in]     unsigned int		size		: size
 * @return        None
 * @note          
 * @attention     
 */
extern void GPU_IF_User_CleanFlush_Dcache(
	unsigned int		addr,				/* StartAddress				*/
	unsigned int		size				/* size						*/
);

/**
 * @brief         Delay Task
 * @param[in]     unsigned int		msec		: delay time(ms)
 * @return        None
 * @note          
 * @attention     
 */
extern void GPU_IF_User_Dly_Tsk(
	unsigned int		msec;
);

/**
 * @brief         HCLK on
 * @param[in]     None
 * @return        None
 * @note          
 * @attention     
 */
extern void GPU_IF_on_hclk(void);

/**
 * @brief         HCLK off
 * @param[in]     None
 * @return        None
 * @note          
 * @attention     
 */
extern void GPU_IF_off_hclk(void);

#endif

