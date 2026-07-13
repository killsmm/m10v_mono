/**
 * @file		fj_iip_resource.c
 * @brief		IIP resource management apis
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_std.h"
#include "fj_iip_resource_common.h"
#include "bf_iip_resource.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/**
 * @brief	Lock the IIP resource.
 * @param	O: T_FJ_IIP_RES_CONFIG_OUT	config_out[]
 * @param	I: T_FJ_IIP_RES_CONFIG_ARG	config_arg[]
 * @param	I: UCHAR					config_num
 * @param	I: INT32					wait_time
 * @return	FJ_ERR_CODE
 * @note	None
*/
FJ_ERR_CODE FJ_IIP_Res_Config_Lock_Host( T_FJ_IIP_RES_CONFIG_OUT config_out[], const T_FJ_IIP_RES_CONFIG_ARG config_arg[], const UCHAR config_num, const INT32 wait_time )
{
	INT32 ret;

	ret = fj_iipresconfiglockhost( config_out, config_arg, config_num, wait_time );
	if ( ret != D_FJ_IIP_ERR_OK ) {
		if ( ret == D_FJ_IIP_ERR_TMOUT ) {
			return FJ_ERR_NG_TMOUT;
		}
		else if ( ret == D_FJ_IIP_ERR_PARAM_ERR ) {
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	return FJ_ERR_OK;
}

/**
 * @brief	Release the IIP resource.
 * @param	I: T_FJ_IIP_RES_CONFIG_OUT	config_out[]
 * @param	I: UCHAR					config_num
 * @param	I: INT32					wait_time
 * @return	FJ_ERR_CODE
 * @note	None
*/
FJ_ERR_CODE FJ_IIP_Res_Config_Free_Host( const T_FJ_IIP_RES_CONFIG_OUT config_out[], const UCHAR config_num, const INT32 wait_time )
{
	INT32 ret;

	ret = fj_iipresconfigfreehost( config_out, config_num, wait_time);
	if ( ret != D_FJ_IIP_ERR_OK ) {
		if ( ret == D_FJ_IIP_ERR_TMOUT ) {
			return FJ_ERR_NG_TMOUT;
		}
		else if ( ret == D_FJ_IIP_ERR_PARAM_ERR ) {
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	return FJ_ERR_OK;
}

