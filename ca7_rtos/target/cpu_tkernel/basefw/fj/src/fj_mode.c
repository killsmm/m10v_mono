/**
 * @file		fj_mode.c
 * @brief		mode process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_mode.h"
#include "mode.h"

/******************************************************************************
 Program
******************************************************************************/
FJ_ERR_CODE	FJ_SetMode( FJ_MODE FJ_SetMode )
{
	FJ_ERR_CODE ret;

	ret = (FJ_ERR_CODE)fj_setmode( FJ_SetMode );

	return ret;
}

FJ_ERR_CODE	FJ_StartMode( VOID )
{
	FJ_ERR_CODE ret;

	ret = (FJ_ERR_CODE)fj_startmode();

	return ret;
}

FJ_ERR_CODE	FJ_StopMode( VOID )
{
	FJ_ERR_CODE ret;

	ret = (FJ_ERR_CODE)fj_stopmode();

	return ret;
}

BOOL FJ_Is_ModeStatus_Stop( VOID )
{
	BOOL ret;

	ret = fj_is_modestatus_stop();

	return ret;
}

FJ_ERR_CODE	FJ_WaitModeComp( FJ_MODE wait_mode, UINT32 sleepTime, UINT32 repeatNum )
{
	FJ_ERR_CODE ret;

	ret = (FJ_ERR_CODE)fj_waitmodecomp( wait_mode, sleepTime, repeatNum );

	return ret;
}

FJ_MODE	FJ_Get_Act_Sysmode( VOID )
{
	FJ_MODE mode;

	mode = (FJ_MODE)fj_get_act_sysmode();

	return mode;
}

FJ_ERR_CODE	FJ_QueryStatus( UINT32* pwUSR1, USHORT* pwUSR2 )
{
	FJ_ERR_CODE err_code;

	err_code = (FJ_ERR_CODE)fj_querystatus( pwUSR1, pwUSR2 );

	return err_code;
}

FJ_ERR_CODE	FJ_GetStatusRegister( FJ_STATUS_REGISTER bRegister, USHORT* pwValue )
{
	FJ_ERR_CODE err_code;

	err_code = (FJ_ERR_CODE)fj_getstatusregister( bRegister, pwValue );

	return err_code;
}

