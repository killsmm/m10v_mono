/**
 * @file		bf_iip_resource.c
 * @brief		IIP resource management processing
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "os_user_custom.h"
#include "fj_iip_resource_common.h"
#include "bf_iip_resource.h"
#include "iip_resmng.h"
#include "iip_resmng_debug.h"
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

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
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		check the request resouce lock counts
 *	@param		[I]T_FJ_IIP_RES_CONFIG_ARG config_arg[]
 *	@param		[I]UCHAR config_num
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
static INT32 iip_resource_check_res_count( const T_FJ_IIP_RES_CONFIG_ARG config_arg[], const UCHAR config_num )
{
	UCHAR loop;

	for (loop = 0; loop < config_num; loop++) {
		if ( config_arg[loop].unit.oned > 8 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.flt > 1 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.afn > 4 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.frect > 2 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.mon > 1 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.csc > 4 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.lut > 1 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.gpc > 1 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.blend > 2 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.cfl > 1 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.mft > 1 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.sl > 9 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.sl_0_2 > 3 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.sl_3_8 > 6 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.sl_master_if_0 > 4 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.sl_master_if_1 > 4 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.sl_master_if_2 > 3 ) {
			return  D_FJ_IIP_ERR_NG;
		}
		if ( config_arg[loop].unit.sl_mon > 8 ) {
			return  D_FJ_IIP_ERR_NG;
		}

		if ( config_arg[loop].pixid > 12 ) {
			return  D_FJ_IIP_ERR_NG;
		}

		if ( config_arg[loop].other > (D_FJ_IIP_RES_OTHER_BICUBIC_0_OR_1 << 1) ) {
			return  D_FJ_IIP_ERR_NG;
		}
	}

	return D_FJ_IIP_ERR_OK;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief	Lock the IIP resource.
 * @param	O: T_FJ_IIP_RES_CONFIG_OUT	config_out[]
 * @param	I: T_FJ_IIP_RES_CONFIG_ARG	config_arg[]
 * @param	I: UCHAR					config_num
 * @param	I: INT32					wait_time
 * @return	INT32
 * @note	None
*/
INT32 BF_IIP_Res_Config_Lock_BaseFW( T_FJ_IIP_RES_CONFIG_OUT config_out[], const T_FJ_IIP_RES_CONFIG_ARG config_arg[], const UCHAR config_num, const INT32 wait_time )
{
	INT32 ret;
	OS_USER_ID tskid;
	T_IIP_RES_USER	user_info;

	// Parameter check
	if ( config_out == NULL ) {
		return D_FJ_IIP_ERR_PARAM_ERR;
	}
	if ( config_arg == NULL ) {
		return D_FJ_IIP_ERR_PARAM_ERR;
	}
	if ( wait_time < -1 ) {
		return D_FJ_IIP_ERR_PARAM_ERR;
	}
	ret = iip_resource_check_res_count( config_arg, config_num );
	if ( ret != D_FJ_IIP_ERR_OK ) {
		return D_FJ_IIP_ERR_PARAM_ERR;
	}

	// Get Task ID
	ret = OS_User_Get_Tid( &tskid );
	if ( ret != D_OS_USER_E_OK ) {
		return D_FJ_IIP_ERR_NG;
	}

	// Set the User Information
	user_info.tskid		= tskid;
	user_info.layer		= D_IIP_RES_LAYER_BFW;

	// Lock process
	ret = IIP_Res_Config_Lock( (T_IIP_RES_CONFIG_OUT*)config_out, (T_IIP_RES_CONFIG_ARG*)config_arg, config_num, wait_time, &user_info );
	if ( ret != D_IIP_ERR_OK ) {
		if ( ret == D_IIP_ERR_TMOUT ) {
			return D_FJ_IIP_ERR_TMOUT;
		}
		return D_FJ_IIP_ERR_NG;
	}
	return D_FJ_IIP_ERR_OK;
}

/**
 * @brief	Release the IIP resource.
 * @param	I: T_FJ_IIP_RES_CONFIG_OUT	config_out[]
 * @param	I: UCHAR					config_num
 * @param	I: INT32					wait_time
 * @return	INT32
 * @note	None
*/
INT32 BF_IIP_Res_Config_Free_BaseFW( const T_FJ_IIP_RES_CONFIG_OUT config_out[], const UCHAR config_num, const INT32 wait_time )
{
	INT32	ret;

	// Parameter check
	if ( config_out == NULL ) {
		return D_FJ_IIP_ERR_PARAM_ERR;
	}

	// Free resource
	ret = IIP_Res_Config_Free( (T_IIP_RES_CONFIG_OUT*)config_out, config_num, wait_time );
	if ( ret != D_IIP_ERR_OK ) {
		if ( ret == D_IIP_ERR_TMOUT ) {
			return D_FJ_IIP_ERR_TMOUT;
		}
		return D_FJ_IIP_ERR_NG;
	}
	return D_FJ_IIP_ERR_OK;
}

/**
 * @brief	Print a table values of the IIP Resource Management.
 * @param	None
 * @return	None
 * @note	None
*/
VOID BF_IIP_Res_DebugPrint( VOID )
{
	IIP_Res_DebugPrint();
	return;
}

/**
 * @brief	Lock the IIP resource.
 * @param	O: T_FJ_IIP_RES_CONFIG_OUT	config_out[]
 * @param	I: T_FJ_IIP_RES_CONFIG_ARG	config_arg[]
 * @param	I: UCHAR					config_num
 * @param	I: INT32					wait_time
 * @return	INT32
 * @note	None
*/
INT32 fj_iipresconfiglockhost( T_FJ_IIP_RES_CONFIG_OUT config_out[], const T_FJ_IIP_RES_CONFIG_ARG config_arg[], const UCHAR config_num, const INT32 wait_time )
{
	INT32 ret;
	OS_USER_ID tskid;
	T_IIP_RES_USER	user_info;

	// Parameter check
	if ( config_out == NULL ) {
		return D_FJ_IIP_ERR_PARAM_ERR;
	}
	if ( config_arg == NULL ) {
		return D_FJ_IIP_ERR_PARAM_ERR;
	}
	if ( wait_time < -1 ) {
		return D_FJ_IIP_ERR_PARAM_ERR;
	}
	ret = iip_resource_check_res_count( config_arg, config_num );
	if ( ret != D_FJ_IIP_ERR_OK ) {
		return D_FJ_IIP_ERR_PARAM_ERR;
	}

	ret = OS_User_Get_Tid( &tskid );
	if ( ret != D_OS_USER_E_OK ) {
		return D_FJ_IIP_ERR_NG;
	}

	// Set the User Information
	user_info.tskid		= tskid;
	user_info.layer		= D_IIP_RES_LAYER_HOST_IP;

	ret = IIP_Res_Config_Lock( (T_IIP_RES_CONFIG_OUT*)config_out, (T_IIP_RES_CONFIG_ARG*)config_arg, config_num, wait_time, &user_info );
	if ( ret != D_IIP_ERR_OK ) {
		if ( ret == D_IIP_ERR_TMOUT ) {
			return D_FJ_IIP_ERR_TMOUT;
		}
		return D_FJ_IIP_ERR_NG;
	}

	return D_FJ_IIP_ERR_OK;
}

/**
 * @brief	Release the IIP resource.
 * @param	I: T_FJ_IIP_RES_CONFIG_OUT	config_out[]
 * @param	I: UCHAR					config_num
 * @param	I: INT32					wait_time
 * @return	INT32
 * @note	None
*/
INT32 fj_iipresconfigfreehost(const T_FJ_IIP_RES_CONFIG_OUT config_out[], const UCHAR config_num, const INT32 wait_time )
{
	INT32	ret;

	// Parameter check
	if ( config_out == NULL ) {
		return D_FJ_IIP_ERR_PARAM_ERR;
	}

	// Free resource
	ret = IIP_Res_Config_Free( (T_IIP_RES_CONFIG_OUT*)config_out, config_num, wait_time );
	if ( ret != D_IIP_ERR_OK ) {
		if ( ret == D_IIP_ERR_TMOUT ) {
			return D_FJ_IIP_ERR_TMOUT;
		}
		return D_FJ_IIP_ERR_NG;
	}

	return D_FJ_IIP_ERR_OK;
}

