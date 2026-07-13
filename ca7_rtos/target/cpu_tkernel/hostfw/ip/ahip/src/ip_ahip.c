/**
 * @file		ip_ahip.c
 * @brief		AHIP's APIs wrapper layer
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")
#include "ddim_typedef.h"
#include <string.h>
#include <stdio.h>
#include "ahip.h"
#include "ip_ahip.h"
#include "fj_top.h"
#include "fj_host_debug.h"
#include "fj_debug.h"
#include "measure_time.h"
#include "dd_top.h"
#include "share.h"

#define CO_AHIP_CLOCK

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// #define D_IP_AHIP_DEBUG

// stream max number for playback
#define D_IP_AHIP_STREAM_NUM_MAX		(5)

// instance max number
#define D_IP_AHIP_INSTANCE_NUM_MAX		(10)

// driver not opened status
#define D_IP_AHIP_NOT_OPENED			(0)
#define D_IP_AHIP_OPENED_PREVIEW		(1)
#define D_IP_AHIP_OPENED_PLAYBACK		(2)


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
// nothing special.

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
// nothing special.

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
// nothing special.

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static	volatile	UCHAR	gIP_AHIP_Oepn_State[ D_IP_AHIP_INSTANCE_NUM_MAX ] = {
																					D_IP_AHIP_NOT_OPENED,
																					D_IP_AHIP_NOT_OPENED,
																					D_IP_AHIP_NOT_OPENED,
																					D_IP_AHIP_NOT_OPENED,
																					D_IP_AHIP_NOT_OPENED,
																					D_IP_AHIP_NOT_OPENED,
																					D_IP_AHIP_NOT_OPENED,
																					D_IP_AHIP_NOT_OPENED,
																					D_IP_AHIP_NOT_OPENED,
																					D_IP_AHIP_NOT_OPENED
																				};

static				UCHAR	gIP_AHIP_Fd_Ctrl_Cnt	= 0;
static				UCHAR	gIP_AHIP_Vis_Ctrl_Cnt	= 0;
static				UCHAR	gIP_AHIP_Ahb_Ctrl_Cnt	= 0;
static				UCHAR	gIP_AHIP_Axi_Ctrl_Cnt	= 0;

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static VOID ip_ahip_clock_start( VOID )
{
	Dd_Top_Start_Clock( (UCHAR*)&gIP_AHIP_Fd_Ctrl_Cnt,  &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_IPUFDCK_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&gIP_AHIP_Vis_Ctrl_Cnt, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_IPUVISCK_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&gIP_AHIP_Axi_Ctrl_Cnt, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_IPUAX_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&gIP_AHIP_Ahb_Ctrl_Cnt, &Dd_Top_Get_CLKSTOP7(), ~D_DD_TOP_IPUAH_BIT );
}

static VOID ip_ahip_clock_stop( VOID )
{
	Dd_Top_Stop_Clock( (UCHAR*)&gIP_AHIP_Fd_Ctrl_Cnt,  &Dd_Top_Get_CLKSTOP7(), D_DD_TOP_IPUFDCK_BIT );
	Dd_Top_Stop_Clock( (UCHAR*)&gIP_AHIP_Vis_Ctrl_Cnt, &Dd_Top_Get_CLKSTOP7(), D_DD_TOP_IPUVISCK_BIT );
	Dd_Top_Stop_Clock( (UCHAR*)&gIP_AHIP_Axi_Ctrl_Cnt, &Dd_Top_Get_CLKSTOP7(), D_DD_TOP_IPUAX_BIT );
	Dd_Top_Stop_Clock( (UCHAR*)&gIP_AHIP_Ahb_Ctrl_Cnt, &Dd_Top_Get_CLKSTOP7(), D_DD_TOP_IPUAH_BIT );
}

static INT32 ip_ahip_init( VOID )
{
	INT32 ip_ret;

//	FJ_Top_Set_Clock_Select_IPU( FJ_TOP_IPU_CLKSEL_400MHZ );
//	FJ_Top_Set_Clock_Select_IPU_TME( FJ_TOP_IPU_TME_CLKSEL_500MHZ );

#ifdef CO_AHIP_CLOCK
	ip_ahip_clock_start();
#endif	// CO_AHIP_CLOCK

	// ahip macro initialzie.
#ifdef CO_DEBUG_ON_PC
	ip_ret = GTS_OK;
#else
	ip_ret = AhipInit();
#endif

	if( ip_ret != GTS_OK ) {
		FJ_Debug_Print_Err( "ip_ahip_init(): AhipInit error. ret[%d]\n", ip_ret );

#ifdef CO_AHIP_CLOCK
		ip_ahip_clock_stop();
#endif	// CO_AHIP_CLOCK
	}

	return ip_ret;
}

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

///////////////////////////////////////////////////////////////////////////////
// Common APIs section
///////////////////////////////////////////////////////////////////////////////


INT32 IP_AHIP_OpenPlayback( UCHAR demosaic_id )
{
	INT32 ip_ret;

	// check drivers are already opened.
	if ( gIP_AHIP_Oepn_State[ demosaic_id ] != D_IP_AHIP_NOT_OPENED ) {
		if ( gIP_AHIP_Oepn_State[ demosaic_id ] == D_IP_AHIP_OPENED_PLAYBACK ) {
			return D_IP_AHIP_OK;
		}
		else {
			FJ_Debug_Print_Info( "IP_AHIP_OpenPlayback(): already opened in other mode. id=%d[%d]\n", demosaic_id, gIP_AHIP_Oepn_State[ demosaic_id ] );
			return D_IP_AHIP_BUSY;
		}
	}

	OS_User_Wai_Sem( SID_AHIP );

	ip_ret = ip_ahip_init();
	if( ip_ret != GTS_OK ) {
		OS_User_Sig_Sem( SID_AHIP );
		return D_IP_AHIP_NG;
	}

	// Playback mode.
#ifdef CO_DEBUG_ON_PC
	ip_ret = GTS_OK;
#else
	ip_ret = AhipOpen( NULL );
#endif
	if( ip_ret != GTS_OK ) {
#ifndef CO_DEBUG_ON_PC
		AhipCleanup();
#endif	// CO_DEBUG_ON_PC
#ifdef CO_AHIP_CLOCK
		ip_ahip_clock_stop();
#endif	// CO_AHIP_CLOCK

		OS_User_Sig_Sem( SID_AHIP );

		FJ_Debug_Print_Err( "IP_AHIP_OpenPlayback(): AhipOpen error. ret[%d]\n", ip_ret );
		return D_IP_AHIP_NG;
	}
	else {
		// Success
		gIP_AHIP_Oepn_State[ demosaic_id ] = D_IP_AHIP_OPENED_PLAYBACK;
	}

	OS_User_Sig_Sem( SID_AHIP );

	return D_IP_AHIP_OK;
}

INT32 IP_AHIP_OpenPreview( UCHAR demosaic_id, const T_IP_AHIP_PREVIEW_CFG* const cfg )
{
	INT32				ip_ret;
	AHIP_PREVIEW_CONFIG	prev_cfg;

	if( cfg == NULL ) {
		FJ_Debug_Print_Err( "IP_AHIP_OpenPreview(): cfg is null. \n" );
		return D_IP_AHIP_INVALID_ARG;
	}

	// check drivers are already opened.
	if ( gIP_AHIP_Oepn_State[ demosaic_id ] != D_IP_AHIP_NOT_OPENED ) {
		if ( gIP_AHIP_Oepn_State[ demosaic_id ] == D_IP_AHIP_OPENED_PREVIEW ) {
			return D_IP_AHIP_OK;
		}
		else {
			FJ_Debug_Print_Err( "IP_AHIP_OpenPreview(): already opened in other mode. id=%d[%d]\n", demosaic_id, gIP_AHIP_Oepn_State[ demosaic_id ] );
			return D_IP_AHIP_BUSY;
		}
	}

	OS_User_Wai_Sem( SID_AHIP );

	ip_ret = ip_ahip_init();

	if( ip_ret != GTS_OK ) {
		OS_User_Sig_Sem( SID_AHIP );
		return D_IP_AHIP_NG;
	}

	// Preview mode.
	prev_cfg.flags			= (UINT32)cfg->flags;
	prev_cfg.format			= (UINT32)cfg->fmt;
	prev_cfg.size_a.width	= cfg->size_a.width;
	prev_cfg.size_a.height	= cfg->size_a.lines;
	prev_cfg.size_b.width	= cfg->size_b.width;
	prev_cfg.size_b.height	= cfg->size_b.lines;
	prev_cfg.mode			= cfg->mode;
#ifdef CO_DEBUG_ON_PC
	ip_ret = GTS_OK;
#else
	ip_ret = AhipOpen( &prev_cfg );
//	ip_ret = AhipOpen( NULL );
#endif

	if( ip_ret != GTS_OK ) {
#ifndef CO_DEBUG_ON_PC
		AhipCleanup();
#endif

#ifdef CO_AHIP_CLOCK
		ip_ahip_clock_stop();
#endif	// CO_AHIP_CLOCK

		OS_User_Sig_Sem( SID_AHIP );

		FJ_Debug_Print_Err( "IP_AHIP_OpenPreview(): AhipOpen error. ret[%d]\n", ip_ret );
		return D_IP_AHIP_NG;
	}
	else {
		// Success
		gIP_AHIP_Oepn_State[ demosaic_id ] = D_IP_AHIP_OPENED_PREVIEW;
	}

	OS_User_Sig_Sem( SID_AHIP );

	return ip_ret;
}

VOID IP_AHIP_Close( UCHAR demosaic_id )
{
	OS_User_Wai_Sem( SID_AHIP );

#ifndef CO_DEBUG_ON_PC
	AhipClose();
	AhipCleanup();
#endif

#ifdef CO_AHIP_CLOCK
	ip_ahip_clock_stop();
#endif	// CO_AHIP_CLOCK

	// open mode initialzie.
	gIP_AHIP_Oepn_State[ demosaic_id ] = D_IP_AHIP_NOT_OPENED;

	OS_User_Sig_Sem( SID_AHIP );

	return;
}

VOID IP_AHIP_LockSem( UCHAR demosaic_id )
{
	OS_User_Wai_Sem( SID_AHIP );
}

VOID IP_AHIP_UnlockSem( UCHAR demosaic_id )
{
	OS_User_Sig_Sem( SID_AHIP );
}

UINT32 IP_AHIP_Get_Frame_No( UCHAR demosaic_id )
{
	UINT32 frame_no = 0;

#ifdef CO_DEBUG_ON_PC
	frame_no = 0;
#else
//	frame_no = AhipPreGetFrameNo();
#endif

	return frame_no;
}

BOOL IP_AHIP_Get_Open_Status( UCHAR demosaic_id )
{
	if ( gIP_AHIP_Oepn_State[ demosaic_id ] ) {
		return TRUE;
	}
	return FALSE;
}

#ifndef CO_AHIP_DEVELOPMENT
VOID IP_AHIP_Int_Handler( UCHAR demosaic_id )
{
#ifndef CO_DEBUG_ON_PC
	AhipIntHandler();
#endif

//	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_14, FJ_MBALOG_FUNC_ID_IP_CHECK, FJ_MBALOG_TYPE_POINT, "AHIP_INT"));

	return;
}
#endif

ULONG AHIP_User_Get_Time_us( VOID )
{
    return BF_Get_Time_us();
}
