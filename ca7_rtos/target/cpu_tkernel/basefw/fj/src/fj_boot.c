/**
 * @file		fj_boot.c
 * @brief		boot process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_boot.h"
#include "boot.h"
#include "boot_loader.h"
#include "bf_startup.h"

/******************************************************************************
 Program
******************************************************************************/
FJ_ERR_CODE FJ_GetBootCond( LONG* bootCond )
{
	return fj_getbootcond( bootCond );
}


FJ_ERR_CODE FJ_BootLoad( UCHAR areaNo, char* dstStartAdr, ULONG loadSize )
{
	FJ_ERR_CODE err_code;

	err_code = (FJ_ERR_CODE)fj_bootload( areaNo, dstStartAdr, loadSize );

	return err_code;
}


INT32 FJ_BootLoadState( VOID )
{
	INT32 result;

	result = fj_bootloadstate();

	return result;
}

VOID FJ_Reboot( VOID )
{
	fj_reboot();
}

VOID FJ_BootMessagePrint( VOID )
{
	fj_bootmessageprint();
}

ULONG FJ_Get_OK_Version( VOID )
{
	return fj_get_ok_version();
}

VOID FJ_Reboot_Pre( VOID )
{
	fj_reboot_pre();
}
