/**
 * @file		bf_iip_resource.h
 * @brief		Header file of IIP resource management library wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _BF_IIP_RESOURCE_H_
#define _BF_IIP_RESOURCE_H_

#include "fj_iip_resource_common.h"
#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/** Return value of processing result */
#define D_FJ_IIP_ERR_NG						(-1)					/**< Abnormal End */
#define D_FJ_IIP_ERR_OK						(0)						/**< Normal End */
#define D_FJ_IIP_ERR_TMOUT					(1)						/**< Timeout End */
#define D_FJ_IIP_ERR_PARAM_ERR				(2)						/**< Parameter Error */

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
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

/**
 Lock the IIP resource. (for BaseFW)
@param[out]	config_out	: IIP resource bitmask.
@param[in]	config_arg	: the IIP resource to use.
@param[in]	config_num	: Number of array. (&ref ocnfig_out and &ref config_arg)
@param[in]	wait_time	: wait time.
@retval		D_FJ_IIP_ERR_OK			: Normal End
@retval		D_FJ_IIP_ERR_NG			: Abnormal End
@retval		D_FJ_IIP_ERR_TMOUT		: Timeout End
@retval		D_FJ_IIP_ERR_PARAM_ERR	: Parameter Error
@remarks	
*/
extern INT32 BF_IIP_Res_Config_Lock_BaseFW( T_FJ_IIP_RES_CONFIG_OUT config_out[], const T_FJ_IIP_RES_CONFIG_ARG config_arg[], const UCHAR config_num, const INT32 wait_time );

/**
 Release the IIP resource. (for BaseFW)
@param[in]	config_out	: IIP resource bitmask.
@param[in]	config_num	: Number of array. (&ref ocnfig_out)
@param[in]	wait_time	: wait time.
@retval		D_FJ_IIP_ERR_OK			: Normal End
@retval		D_FJ_IIP_ERR_NG			: Abnormal End
@retval		D_FJ_IIP_ERR_TMOUT		: Timeout End
@retval		D_FJ_IIP_ERR_PARAM_ERR	: Parameter Error
@remarks	
*/
extern INT32 BF_IIP_Res_Config_Free_BaseFW( const T_FJ_IIP_RES_CONFIG_OUT config_out[], const UCHAR config_num, const INT32 wait_time );

/**
 Print a table of the IIP Resource Management.
*/
extern VOID BF_IIP_Res_DebugPrint( VOID );

/**
 Lock the IIP resource. (for HOST/IP)
@param[out]	config_out	: IIP resource bitmask.
@param[in]	config_arg	: the IIP resource to use.
@param[in]	config_num	: Number of array. (&ref ocnfig_out and &ref config_arg)
@param[in]	wait_time	: wait time (for semaphore).
@retval		D_FJ_IIP_ERR_OK			: Normal End
@retval		D_FJ_IIP_ERR_NG			: Abnormal End
@retval		D_FJ_IIP_ERR_TMOUT		: Timeout End
@retval		D_FJ_IIP_ERR_PARAM_ERR	: Parameter Error
@remarks	
*/
extern INT32 fj_iipresconfiglockhost( T_FJ_IIP_RES_CONFIG_OUT config_out[], const T_FJ_IIP_RES_CONFIG_ARG config_arg[], const UCHAR config_num, const INT32 wait_time );

/**
 Release the IIP resource. (for HOST/IP)
@param[in]	config_out	: IIP resource bitmask.
@param[in]	config_num	: Number of array. (&ref ocnfig_out)
@param[in]	wait_time	: wait time (for semaphore).
@retval		D_FJ_IIP_ERR_OK			: Normal End
@retval		D_FJ_IIP_ERR_NG			: Abnormal End
@retval		D_FJ_IIP_ERR_TMOUT		: Timeout End
@retval		D_FJ_IIP_ERR_PARAM_ERR	: Parameter Error
@remarks	
*/
extern INT32 fj_iipresconfigfreehost( const T_FJ_IIP_RES_CONFIG_OUT config_out[], const UCHAR config_num, const INT32 wait_time );


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _BF_IIP_RESOURCE_H_

