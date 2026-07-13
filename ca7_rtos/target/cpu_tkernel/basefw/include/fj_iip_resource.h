/**
 * @file		fj_iip_resource.h
 * @brief		IIP Resource Management API.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _FJ_IIP_RESOURCE_H_
#define _FJ_IIP_RESOURCE_H_

#include "fj_std.h"
#include "fj_iip_resource_common.h"
#include "ddim_typedef.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_iip_resource_overview
@{
	This is overview.<br>
	The content of this chapter is described about APIs of the IIP resource management.<br>
@}*//* --- end of fj_iip_resource_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_iip_resource_sequence
@{
- @ref fj_iip_resource_sequence_section1
<br>
@section fj_iip_resource_sequence_section1	Lock and release the IIP resources.
@image html fj_iip_resource_sq01_resource_lock.png

@}*//* --- end of fj_iip_resource_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
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
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus
///////////////////////////////////////////////////////////////////////////////
// APIs
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_iip_resource_api
@{*/

/**
 <table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 Lock the IIP resource. (for HOST/IP)
@param[out]	config_out	: IIP resource bitmask.
@param[in]	config_arg	: the IIP resource to use.
@param[in]	config_num	: Number of array. (&ref ocnfig_out and &ref config_arg)
@param[in]	wait_time	: wait time.
@retval		FJ_ERR_OK				: Normal End
@retval		FJ_ERR_NG				: Abnormal End
@retval		FJ_ERR_NG_TMOUT			: Timeout End
@retval		FJ_ERR_NG_INPUT_PARAM	: Parameter Error
@remarks	
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
extern FJ_ERR_CODE	FJ_IIP_Res_Config_Lock_Host( T_FJ_IIP_RES_CONFIG_OUT config_out[], const T_FJ_IIP_RES_CONFIG_ARG config_arg[], const UCHAR config_num, const INT32 wait_time );

/**
 <table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 Release the IIP resource. (for HOST/IP)
@param[in]	config_out	: IIP resource bitmask.
@param[in]	config_num	: Number of array. (&ref ocnfig_out)
@param[in]	wait_time	: wait time.
@retval		FJ_ERR_OK				: Normal End
@retval		FJ_ERR_NG				: Abnormal End
@retval		FJ_ERR_NG_TMOUT			: Timeout End
@retval		FJ_ERR_NG_INPUT_PARAM	: Parameter Error
@remarks	
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
extern FJ_ERR_CODE	FJ_IIP_Res_Config_Free_Host( const T_FJ_IIP_RES_CONFIG_OUT config_out[], const UCHAR config_num, const INT32 wait_time );

/*@} --- end of fj_iip_resource_api */

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _FJ_IIP_RESOURCE_H_

