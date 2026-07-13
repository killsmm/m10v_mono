/**
 * @file		iip_res_user_custom.c
 * @brief		wrapper of the OS wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "os_user_custom.h"
#include "iip_res_user_custom.h"
#include "iip_resmng.h"

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
 *	@brief		semaphore get
 *	@param		INT32 tmout
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
INT32 IIP_Res_Custom_Open( INT32 tmout )
{
	INT32 ercd;

	if ( tmout == 0 ) {
		ercd = OS_User_Pol_Sem( (OS_USER_ID)SID_IIP_RES );
	}
	else {
		ercd = OS_User_Twai_Sem( (OS_USER_ID)SID_IIP_RES, tmout );
	}

	if( ercd != D_OS_USER_E_OK ) {
		if( ercd == D_OS_USER_E_TMOUT ) {
			return D_IIP_ERR_TMOUT;
		}
		return D_IIP_ERR_NG;
	}

	return D_IIP_ERR_OK;
}

/**
 *	@brief		semaphore release
 *	@param		None
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
INT32 IIP_Res_Custom_Close( VOID )
{
	INT32 ercd;

	ercd = OS_User_Sig_Sem( (OS_USER_ID)SID_IIP_RES );
	if ( ercd != D_OS_USER_E_OK ) {
		return D_IIP_ERR_NG;
	}

	return D_IIP_ERR_OK;
}


/**
 *	@brief		OS_User_Dly_Tsk wrapper
 *	@param		UINT32 msec
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
INT32 IIP_Res_Custom_Dly_Tsk( UINT32 msec )
{
	return OS_User_Dly_Tsk( (OS_USER_RELTIM)msec );
}

/**
 *	@brief		OS_User_Get_Tim wrapper
 *	@param		ULONG* sys_time
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
INT32 IIP_Res_Custom_Get_Tim( ULLONG* sys_time )
{
	return OS_User_Get_Tim( (OS_USER_SYSTIM*)sys_time );
}

/**
 *	@brief		OS_User_Twai_Flg wrapper
 *	@param		ULONG waiptn
 *	@param		UINT32 wfmode
 *	@param		ULONG* p_flgptn
 *	@param		INT32 tmout
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
INT32 IIP_Res_Custom_Twai_Flg( ULONG waiptn, UINT32 wfmode, ULONG* p_flgptn, INT32 tmout )
{
	INT32 ercd;

	ercd = OS_User_Twai_Flg( (OS_USER_ID)FID_IIP_RES_MNG , (OS_USER_FLGPTN)waiptn, (OS_USER_MODE)wfmode, (OS_USER_FLGPTN*)p_flgptn, (OS_USER_TMO)tmout );

	if( ercd != D_OS_USER_E_OK ) {
		if( ercd == D_OS_USER_E_TMOUT ) {
			return D_IIP_ERR_TMOUT;
		}
		return D_IIP_ERR_NG;
	}

	return D_IIP_ERR_OK;
}

/**
 *	@brief		OS_User_Set_Flg wrapper
 *	@param		ULONG setptn
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
INT32 IIP_Res_Custom_Set_Flg( ULONG setptn )
{
	INT32 ercd;

	ercd = OS_User_Set_Flg( (OS_USER_ID)FID_IIP_RES_MNG, (OS_USER_FLGPTN)setptn );
	if( ercd != D_OS_USER_E_OK ) {
		return D_IIP_ERR_NG;
	}

	return D_IIP_ERR_OK;
}

/**
 *	@brief		OS_User_Clr_Flg wrapper
 *	@param		ULONG clrptn
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
INT32 IIP_Res_Custom_Clr_Flg( ULONG clrptn )
{
	INT32 ercd;

	ercd = OS_User_Clr_Flg( (OS_USER_ID)FID_IIP_RES_MNG, (OS_USER_FLGPTN)clrptn );
	if( ercd != D_OS_USER_E_OK ) {
		return D_IIP_ERR_NG;
	}

	return D_IIP_ERR_OK;
}



