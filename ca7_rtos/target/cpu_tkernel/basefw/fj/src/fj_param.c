/**
 * @file		fj_param.c
 * @brief		param process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_param.h"
#include "system_attr.h"


/******************************************************************************
 Program
******************************************************************************/
FJ_ERR_CODE FJ_GetParam( USHORT wParamID, USHORT* pwValue )
{
	FJ_ERR_CODE err_code;

	err_code = (FJ_ERR_CODE)fj_getparam( wParamID, pwValue );

	return err_code;
}

FJ_ERR_CODE FJ_SetParam( USHORT wParamID, USHORT wValue )
{
	FJ_ERR_CODE err_code;

	err_code = (FJ_ERR_CODE)fj_setparam( wParamID, wValue );

	return err_code;
}

FJ_ERR_CODE	FJ_GetParamLong(USHORT wParamID, LONG* pwValue)
{
	FJ_ERR_CODE err_code;

	err_code = (FJ_ERR_CODE)fj_getparamlong( wParamID, pwValue );

	return err_code;
}
