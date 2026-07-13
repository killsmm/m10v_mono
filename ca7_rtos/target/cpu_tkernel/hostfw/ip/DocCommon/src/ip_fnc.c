/**
 * @file		ip_fnc.c
 * @brief		API for Fotonation Common
 * @author		Hattori
 * @since		2016/01/07
 * @date		2016/02/04
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT Inc. 2016</I></B>
 */

// ****************************************************************************
//                          Include
// ****************************************************************************
//#include <string.h>
#include "ddim_typedef.h"
#include "ahip.h"
#include "../include/ip_fnc.h"

// ****************************************************************************
//                          Define
// ****************************************************************************

// ****************************************************************************
//                           Structure
// ****************************************************************************

// ****************************************************************************
//                           Global Data
// ****************************************************************************
static	USHORT		gIpFNC_Status = D_IP_FNC_STATUS_NONE;
static	USHORT		gIpFNC_AhipStatus = D_IP_FNC_AHIP_CLOSE;

// ****************************************************************************
//                        External Method Definition
// ****************************************************************************

// ****************************************************************************
//                        Local Method Definition
// ****************************************************************************

// ****************************************************************************
//                          	Method
// ****************************************************************************

/******************************************************************************
 * @brief	ライブラリOpen
 *
 * @retval	D_IP_FNC_OK
 * @retval	D_IP_FNC_NG_SEQUENCE
 */
INT32	IP_FNC_SetFlag( USHORT setflag )
{
	if( ~D_IP_FNC_STATUS_ALL & setflag ){	// パラメタ・エラー
		return D_IP_FNC_NG_PARAM;
	}
	if( gIpFNC_Status & setflag ){			// 既に使用中
		return D_IP_FNC_NG_ALREADY_USE;
	}
	gIpFNC_Status |= setflag;

	return D_IP_FNC_OK;
}

/******************************************************************************
 * @brief	ライブラリClose
 *
 * @retval	D_IP_FNC_OK
 * @retval	D_IP_FNC_NG_SEQUENCE
 */
INT32	IP_FNC_ResetFlag( USHORT resetflag )
{
	if( ~D_IP_FNC_STATUS_ALL & resetflag ){	// パラメタ・エラー
		return D_IP_FNC_NG_PARAM;
	}
	if( ~gIpFNC_Status & resetflag ){			// 未使用
		return D_IP_FNC_NG_NOT_USE;
	}
	gIpFNC_Status &= ~resetflag;

	return D_IP_FNC_OK;
}

/******************************************************************************
 *	@brief		Get Flag
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
USHORT	IP_FNC_GetFlag( VOID )
{
	return gIpFNC_Status;
}

/******************************************************************************
 *	@brief		Ahip Init
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID	IP_FNC_Ahip_Init( VOID )
{
	AhipInit();

	return ;
}

/******************************************************************************
 *	@brief		Ahip Open
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32	IP_FNC_Ahip_Open( T_IP_AHIP_PREVIEW_CONF *cfg )
{
	AHIP_PREVIEW_CONFIG	prev_cfg;

	if(gIpFNC_AhipStatus == D_IP_FNC_AHIP_OPEN){
		return D_IP_FNC_NG_ALREADY_USE;
	}

	if( IP_AHIP_Get_Open_Status(0) == FALSE ){
		if(cfg==NULL){
			AhipOpen( NULL );
		}else{
			prev_cfg.flags				= cfg->flags;
			prev_cfg.format				= cfg->format;
			prev_cfg.mode 				= cfg->mode;
			prev_cfg.size_a.width		= cfg->width_a;
			prev_cfg.size_a.height		= cfg->height_a;
			prev_cfg.size_b.width		= cfg->width_b;
			prev_cfg.size_b.height		= cfg->height_b;

			AhipOpen( &prev_cfg );
		}
	}

	gIpFNC_AhipStatus = D_IP_FNC_AHIP_OPEN;

	return D_IP_FNC_OK ;
}

/******************************************************************************
 *	@brief		Ahip Close
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32	IP_FNC_Ahip_Close( VOID )
{
	if(gIpFNC_AhipStatus == D_IP_FNC_AHIP_CLOSE){
		return D_IP_FNC_NG_NOT_USE;
	}
	gIpFNC_AhipStatus = D_IP_FNC_AHIP_CLOSE;
	AhipClose();
	AhipCleanup();

	return D_IP_FNC_OK ;
}

/******************************************************************************
 *	@brief		Ahip Interrupu Handler
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID	IP_FNC_Ahip_Int_Handler( VOID )
{
	AhipIntHandler();

	return ;
}

/******************************************************************************
 *	@brief		Get Version
 *	@param		o:**str (version strings)
 *	@return		None
 *	@note		None
 *	@attention	None
 */

INT32 IP_FNC_Get_Version( CHAR **str )
{
	if ( str == NULL ) {
		return D_IP_FNC_NG_PARAM ;
	}

//    *str = (IP_CHAR*)strchr( V_Ip_Fnc_Version, ':' ) + 1;
	*str = (CHAR*)&V_Ip_Fnc_Version[13];
	return D_IP_FNC_OK;
}
