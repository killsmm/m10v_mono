/**
 * @file		ip_fnc.h
 * @brief		Headder for Fotonation Common
 * @author		Hattori
 * @since		2016/02/04
 * @date		2016/06/27
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT 2016</I></B>
 */

#ifndef	_IP_FNC_H_
#define	_IP_FNC_H_
// ***************************************************************************
//                          Include
// ***************************************************************************
#include "ddim_typedef.h"
#include "ip_ahip.h"

// ***************************************************************************
//                          Define
// ***************************************************************************
#define	D_IP_OK					0x00000000
#define	D_IP_NG					0x80000000

#define	D_IP_FNC_STATUS_NONE	 0
#define	D_IP_FNC_STATUS_FD		(1 << 0)	// FD execute
#define	D_IP_FNC_STATUS_PD		(1 << 1)	// OD execute
#define	D_IP_FNC_STATUS_VIS		(1 << 2)	// VIS execute
#define	D_IP_FNC_STATUS_PANO	(1 << 3)	// PANORAMA execute

#define	D_IP_FNC_STATUS_ALL		0x000F		// ALL execute

#define D_IP_FNC_OK					(D_IP_OK)						/**< OK */
#define D_IP_FNC_NG					(D_IP_NG		| 0x001D0000)	/**< NG */
#define D_IP_FNC_NG_ALREADY_USE		(D_IP_FNC_NG	| 0x0100)
#define D_IP_FNC_NG_NOT_USE			(D_IP_FNC_NG	| 0x0200)
#define D_IP_FNC_NG_PARAM			(D_IP_FNC_NG	| 0x0300)

#define	D_IP_FNC_AHIP_CLOSE		0
#define	D_IP_FNC_AHIP_OPEN		1

// ***************************************************************************
//                        Structure
// ***************************************************************************
typedef struct	_T_IP_AHIP_PREVIEW_CONF	{
	UINT32		flags;
	UINT32		format;
    UINT32      mode;
	UINT32		width_a;
	UINT32		height_a;    
    UINT32      width_b;
    UINT32      height_b;
} T_IP_AHIP_PREVIEW_CONF;

// ***************************************************************************
//                          Global Data
// ***************************************************************************

// ***************************************************************************
//                        Global Function
// ***************************************************************************
extern	INT32		IP_FNC_SetFlag( USHORT setflag );
extern	INT32		IP_FNC_ResetFlag( USHORT resetflag );
extern	USHORT		IP_FNC_GetFlag( VOID );
extern	INT32		IP_FNC_Get_Version( CHAR **str );
extern	VOID		IP_FNC_Ahip_Int_Handler( VOID );
extern	VOID		IP_FNC_Ahip_Init( VOID );
extern	INT32		IP_FNC_Ahip_Open( T_IP_AHIP_PREVIEW_CONF *cfg );
extern	INT32		IP_FNC_Ahip_Close( VOID );

// ----------------------------------------------------------------------
//  Version
// ----------------------------------------------------------------------
#define V_Ip_Fnc_Version		"flib_version:V01L01\r\n"

#endif	// _IP_FNC_H_
