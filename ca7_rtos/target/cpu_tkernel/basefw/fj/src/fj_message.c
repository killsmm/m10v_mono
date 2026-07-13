/**
 * @file		fj_message.c
 * @brief		message process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_message.h"
#include "bf_startup.h"

/******************************************************************************
 Program
******************************************************************************/
VOID	FJ_HostGetMessage(  ULONG* pulMessage, ULONG* pulParam )
{
	fj_hostgetmessage( pulMessage, pulParam );
}

VOID	FJ_HostPostMessage( ULONG ulMessage, ULONG ulParam )
{
	fj_hostpostmessage( ulMessage, ulParam );
}

FJ_ERR_CODE	FJ_ConfigHostMessage( USHORT wConfig,  ULONG wValue )
{
	FJ_ERR_CODE	err_code;

	err_code = (FJ_ERR_CODE)fj_confighostmessage( wConfig, wValue );

	return err_code;
}



