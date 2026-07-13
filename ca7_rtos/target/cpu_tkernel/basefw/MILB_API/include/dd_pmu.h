/**
 * @file		dd_pmu.h
 * @brief		PMU driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

/** @addtogroup dd_pmu
@{
	CM0 PMU Driver.<br>
@}*//* --- end of dd_pmu */

#ifndef _DD_PMU_H_
#define _DD_PMU_H_

/** @weakgroup dd_pmu
@{*/

#include "pmc.h"
#include "dd_arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Return value of PMU processing result
#define	D_DD_PMU_INPUT_PARAM_ERR		(D_DD_PMU | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Power Domain ID */
typedef enum {
	E_DD_PMU_PD_TYPE_HEVC_PROC0		= 0,						/**< HEVC processor0 */
	E_DD_PMU_PD_TYPE_RESERVED,									/**< Reserved */
	E_DD_PMU_PD_TYPE_HEVC_ENCODE,								/**< HEVC encoder */
	E_DD_PMU_PD_TYPE_HEVC_DECODE,								/**< HEVC decoder */
	E_DD_PMU_PD_TYPE_H264,										/**< H264 */
	E_DD_PMU_PD_TYPE_IMAGE_PIPE0,								/**< Imaging pipe0 */
	E_DD_PMU_PD_TYPE_IMAGE_PIPE1,								/**< Imaging pipe1 */
	E_DD_PMU_PD_TYPE_MAX										/**< Maximum value. */
} E_DD_PMU_PD_TYPE;


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
/**
Power Management Controller .<br>
Get status of PMU PD.
@param [in]		pd_type				: PD type @ref E_DD_PMU_PD_TYPE
@param [out]	value				: Status value
@retval	D_DDIM_OK					: Normal end
@retval	D_DD_PMU_INPUT_PARAM_ERR	: Input Parameter Error
*/
extern INT32 Dd_PMU_Get_Status(E_DD_PMU_PD_TYPE pd_type, ULONG* value);

/**
Power Management Controller .<br>
Power Domain off.
@param [in]		pd_type				: PD type @ref E_DD_PMU_PD_TYPE
@retval	D_DDIM_OK					: Normal end
@retval	D_DD_PMU_INPUT_PARAM_ERR	: Input Parameter Error
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
extern INT32 Dd_PMU_PD_Off(E_DD_PMU_PD_TYPE pd_type);

/**
Power Management Controller .<br>
Power Domain on.
@param [in]		pd_type				: PD type @ref E_DD_PMU_PD_TYPE
@retval	D_DDIM_OK					: Normal end
@retval	D_DD_PMU_INPUT_PARAM_ERR	: Input Parameter Error
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
extern INT32 Dd_PMU_PD_On(E_DD_PMU_PD_TYPE pd_type);

/* @} */	// dd_pmu group

#endif	// _DD_PMU_H_
