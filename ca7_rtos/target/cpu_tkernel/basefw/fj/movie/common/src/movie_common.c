/**
 * @file		movie_common.c
 * @brief		Movie Common Function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "movie_common.h"
#include "h264_if.h"
#include "TKernel_id.h"
#include "debug.h"
#include "dd_arm.h"
#include "h264_movie.h"
#include "h265_movie.h"
#include "audio.h"
#include "h264_user.h"

#include "dd_top.h"
#include "sdram_map_movie_record.h"

#ifdef CO_DEBUG_ON_PC
#include "tmr32_stub.h"
#endif
#include "mba_if.h"
#include "user_streaming.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// [RELOAD TIMER]
#include "dd_tmr32.h"
#define D_BF_MOVIE_COMMON_RTIMER_ENC_USE_CH		7			/* 32bit Reload Timer ch number */
#define D_BF_MOVIE_COMMON_RTIMER_DEC_USE_CH		7

#define D_BF_MOVIE_COMMON_LOCK()				OS_User_Wai_Sem( SID_MOVIE_COMMON )
#define D_BF_MOVIE_COMMON_UNLOCK()				OS_User_Sig_Sem( SID_MOVIE_COMMON )
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct{
	ULONG	sync_start;
	ULLONG	counter;
	ULONG	counter_cofficient;
	ULONG	flgptn;
	INT32	stream_ch;
	UCHAR	timelapse_flag;
#if defined(CO_H264_ENC_DEBUG_VIEW_DUMMY) || defined(CO_H265_ENC_DEBUG_VIEW_DUMMY)
	ULLONG	dummy_counter;
	ULONG	dummy_flgptn;
#endif
} T_BF_MOVIE_COMMON_TIMER_CONTROL;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static	volatile	E_BF_MOVIE_COMMON_TIMER_FRAME_RATE		gBF_Movie_Common_Timer_Frame_Rate = E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_INITIALIZE;		// 32bit ReloadTimer FrameRate

static 	volatile	ULLONG 		gReload_Timer_Count = 0;					//Timer counter value for Movie Start
static	volatile	UINT32		gVop_Time_Increment = 0;
static	volatile	UINT32		gVop_Time_Inc_Resol = 0;
static	volatile	BOOL		gMovie_Timer_Start_Flag[E_VIDEO_ENOCDE_ID_MAX] = {FALSE, FALSE, FALSE, FALSE, FALSE};			//Timer start flag, TRUE:Timer running/FALSE:Timer stop

static	volatile	T_BF_MOVIE_COMMON_RTIME_CTL	gMovie_Rld_ctl;						// Movie Reload Timer Control
static	volatile	T_BF_MOVIE_COMMON_TIMER_CONTROL	gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_MAX];
static	volatile	UINT32		gBF_Movie_Reload_Timer_Max_FrameRate = 0;

#if defined(CO_H264_ENC_DEBUG_VIEW_DUMMY) || defined(CO_H265_ENC_DEBUG_VIEW_DUMMY)
static	volatile	BOOL		gMovie_Timer_Dummy_Sync_Flag[E_VIDEO_ENOCDE_ID_MAX] = {FALSE, FALSE, FALSE};		//Dummy Sync flag, TRUE:Sync running/FALSE:Sync stop
#endif

static BOOL g_Movie_RecTimer_Enable = TRUE;
static BOOL g_Movie_Timer_Start = FALSE;
static BOOL g_Movie_Timer_First_Flag = TRUE;
/*--------------------------------------------------------------*/
/* Macro														*/
/*--------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static	VOID bf_movie_common_reload_timer_enc_inthandler( VOID );
static	VOID bf_movie_common_reload_timer_dec_inthandler( VOID );

/**
 *	@brief		Reload Timer Handller for Encode.\n
 */
static VOID bf_movie_common_reload_timer_enc_inthandler( VOID )
{
	static ULONG coff[E_VIDEO_ENOCDE_ID_MAX];
	E_VIDEO_ENOCDE_ID enc_id_loop;
	ULONG	min_coff;
#if 0 // CT Debug
	ULONG y_addr, c_addr;
	USHORT g_width;
#endif
	
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "timer handler" ));
	
	gReload_Timer_Count++;
	OS_User_Set_Flg( FID_H264_SYNC, FLG_H264_SYNC_ENC_MASTER );
	OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_ENC_MASTER );
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_ENC_MASTER_2 );	// for CC_2_Main task
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---

	if(g_Movie_Timer_First_Flag == TRUE){
		if(g_Movie_Timer_Start == TRUE){
			min_coff = 0;
			for( enc_id_loop=E_VIDEO_ENOCDE_ID_H264_CH0; enc_id_loop<E_VIDEO_ENOCDE_ID_MAX; enc_id_loop++ ){
				if(gBF_Movie_Common_Timer_Ctrl[enc_id_loop].counter_cofficient > 0){
					if((min_coff == 0) || (gBF_Movie_Common_Timer_Ctrl[enc_id_loop].counter_cofficient < min_coff)){
						min_coff = gBF_Movie_Common_Timer_Ctrl[enc_id_loop].counter_cofficient;
					}
				}
			}
		}else{
			min_coff = 1;
		}
		memset(coff, 0, sizeof(coff));
		for( enc_id_loop=E_VIDEO_ENOCDE_ID_H264_CH0; enc_id_loop<E_VIDEO_ENOCDE_ID_MAX; enc_id_loop++ ){
			coff[enc_id_loop] = gBF_Movie_Common_Timer_Ctrl[enc_id_loop].counter_cofficient / min_coff;
		}
		g_Movie_Timer_First_Flag = FALSE;
	}
	
	for( enc_id_loop=E_VIDEO_ENOCDE_ID_H264_CH0; enc_id_loop<E_VIDEO_ENOCDE_ID_MAX; enc_id_loop++ ){
		if( gBF_Movie_Common_Timer_Ctrl[enc_id_loop].sync_start == D_BF_MOVIE_COMMON_ON ){
			if( gBF_Movie_Common_Timer_Ctrl[enc_id_loop].timelapse_flag == FJ_MOVIE_OFF ){
				if( (gBF_Movie_Common_Timer_Ctrl[enc_id_loop].counter == 0) || 
				  ( (gBF_Movie_Common_Timer_Ctrl[enc_id_loop].counter % coff[enc_id_loop]) == 0)){
					if( enc_id_loop < E_VIDEO_ENOCDE_ID_H265_CH0 ){
						OS_User_Set_Flg( FID_H264_SYNC, gBF_Movie_Common_Timer_Ctrl[enc_id_loop].flgptn );
					}
					else{
						H265_If_Record_Sync_Interrupt_Counter_Increment( (E_H265_IF_STREAM_CH)gBF_Movie_Common_Timer_Ctrl[enc_id_loop].stream_ch );
						OS_User_Set_Flg( FID_H265_SYNC, gBF_Movie_Common_Timer_Ctrl[enc_id_loop].flgptn );
					}
				}
			}
			gBF_Movie_Common_Timer_Ctrl[enc_id_loop].counter++;
//			BF_Debug_Print_Information(("counter[%d][%d]\n", enc_id_loop, gBF_Movie_Common_Timer_Ctrl[enc_id_loop].counter));
		}
		else{
			if( gBF_Movie_Common_Timer_Ctrl[enc_id_loop].counter != 0 ){
				gBF_Movie_Common_Timer_Ctrl[enc_id_loop].counter++;
//				BF_Debug_Print_Information(("counter[%d][%d]\n", enc_id_loop, gBF_Movie_Common_Timer_Ctrl[enc_id_loop].counter));
			}
		}
#if defined(CO_H264_ENC_DEBUG_VIEW_DUMMY) || defined(CO_H265_ENC_DEBUG_VIEW_DUMMY)
		if( gMovie_Timer_Dummy_Sync_Flag[enc_id_loop] ){
			if( (gBF_Movie_Common_Timer_Ctrl[enc_id_loop].dummy_counter == 0) || 
			  ( (gBF_Movie_Common_Timer_Ctrl[enc_id_loop].dummy_counter % coff[enc_id_loop]) == 0) ){
				if( enc_id_loop < E_VIDEO_ENOCDE_ID_H265_CH0 ){
					OS_User_Set_Flg( FID_H264_SYNC, gBF_Movie_Common_Timer_Ctrl[enc_id_loop].dummy_flgptn );
				}
				else{
					OS_User_Set_Flg( FID_H265_SYNC, gBF_Movie_Common_Timer_Ctrl[enc_id_loop].dummy_flgptn );
				}
			}
			gBF_Movie_Common_Timer_Ctrl[enc_id_loop].dummy_counter++;
		}
#endif
	}
}

/**
 *	@brief		Reload Timer Handller for Decode.\n
 */
static VOID bf_movie_common_reload_timer_dec_inthandler( VOID )
{
#if 0	// for Debug
	OS_USER_SYSTIM					time_now;
	
	OS_User_Get_Tim(&time_now);
	H264_Info_Print(("time: %d\n", (UINT32)time_now ));
#endif

	if (User_IsDualStream() && User_IsH265MainStream()) {
		OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_DEC );
		OS_User_Set_Flg( FID_H264_SYNC, FLG_H264_SYNC_DEC );
	} else {
		OS_User_Set_Flg( FID_H264_SYNC, FLG_H264_SYNC_DEC );
		OS_User_Set_Flg( FID_H265_SYNC, FLG_H265_SYNC_DEC );
	}

	return;
}

/*----------------------------------------------------------------------*/
/* ReloadTimer Wrapper													*/
/*----------------------------------------------------------------------*/
VOID BF_Movie_Common_Reload_Enc_Handler( VOID )
{
	if(g_Movie_Timer_Start == TRUE){
		bf_movie_common_reload_timer_enc_inthandler();
	}
}

VOID BF_Movie_Common_RecTimer_Enable( BOOL enable )
{
	g_Movie_RecTimer_Enable = enable;
}

/**
 *	@brief		Calculate Timet Parameter from Frame Rate.\n
 *	@param		time_inc		Time Increment\n
 *	@param		time_inc_resol	Time Scale\n
 *	@return		None
 *	@note		(Time Increment/Time Scale)*(Peripheral Clock/Clock Sel)=Count Source
 *				[32bit TimerUsed]       54MHz
 *				[Dual Input Timer Used] 36MHz
 *	@attention	None
 */
VOID BF_Movie_Common_Timer_Calculate( UINT32 time_inc, UINT32 time_inc_resol, UINT32 scan_mode )
{
#ifndef CO_H264_PORTING_BLOCK
	ULONG		frame_rate;
	ULLONG		tmr_check;
	
	frame_rate = (( time_inc_resol * 100 ) / time_inc );
	
	/* 32bit Timer (RCLK) */
	switch( frame_rate ){
		case 23976 :	/* 239.76fps */		/* ex. 1001/240000 * 54MHz(RCLK)/2(CSL) = 112612   */
		case 11988 :	/* 119.88fps */		/* ex. 1001/120000 * 54MHz(RCLK)/2(CSL) = 225225   */
		case 5994 :		/* 59.94fps */		/* ex. 1001/60000  * 54MHz(RCLK)/2(CSL) = 450450   */
		case 4795 :		/* 47.95fps */		/* ex. 1001/48000  * 54MHz(RCLK)/2(CSL) = 563062   */
		case 2997 :		/* 29.97fps */		/* ex. 1001/30000  * 54MHz(RCLK)/2(CSL) = 900900   */
		case 2397 :		/* 23.97fps */		/* ex. 1001/24000  * 54MHz(RCLK)/2(CSL) = 1126125  */
		case 1498 :		/* 14.98fps */		/* ex. 1001/15000  * 54MHz(RCLK)/2(CSL) = 1801800  */
		case 999 :		/* 9.99fps */		/* ex. 1001/10000  * 54MHz(RCLK)/2(CSL) = 2702700  */
		case 749 :		/* 7.49fps */		/* ex. 1001/7500   * 54MHz(RCLK)/2(CSL) = 3607200  */
		case 499 :		/* 4.99fps */		/* ex. 1001/5000   * 54MHz(RCLK)/2(CSL) = 5405400  */
		case 299 :		/* 2.99fps */		/* ex. 1001/3000   * 54MHz(RCLK)/2(CSL) = 9009000  */
		case 199 :		/* 1.99fps */		/* ex. 1001/2000   * 54MHz(RCLK)/2(CSL) = 13513500 */
		case 99 :		/* 0.99fps */		/* ex. 1001/1000   * 54MHz(RCLK)/2(CSL) = 27027000 */
		case 24000 :	/* 240.00fps */		/* ex. 1000/240000 * 54MHz(RCLK)/2(CSL) = 112500   */
		case 12000 :	/* 120.00fps */		/* ex. 1000/120000 * 54MHz(RCLK)/2(CSL) = 225000   */
		case 6000 :		/* 60.00fps */		/* ex. 1000/60000  * 54MHz(RCLK)/2(CSL) = 450000   */
		case 5000 :		/* 50.00fps */		/* ex. 1000/50000  * 54MHz(RCLK)/2(CSL) = 540000   */
		case 4800 :		/* 48.00fps */		/* ex. 1000/48000  * 54MHz(RCLK)/2(CSL) = 562500   */
		case 3000 :		/* 30.00fps */		/* ex. 1000/30000  * 54MHz(RCLK)/2(CSL) = 900000   */
		case 2500 :		/* 25.00fps */		/* ex. 1000/25000  * 54MHz(RCLK)/2(CSL) = 1080000  */
		case 2400 :		/* 24.00fps */		/* ex. 1000/24000  * 54MHz(RCLK)/2(CSL) = 1125000  */
		case 1600 :		/* 16.00fps */		/* ex. 1000/16000  * 54MHz(RCLK)/2(CSL) = 1687500  */ /* for Trick */
		case 1500 :		/* 15.00fps */		/* ex. 1000/15000  * 54MHz(RCLK)/2(CSL) = 1800000  */
		case 1250 :		/* 10.00fps */		/* ex. 1000/10000  * 54MHz(RCLK)/2(CSL) = 2700000  */
		case 1000 :		/* 10.00fps */		/* ex. 1000/10000  * 54MHz(RCLK)/2(CSL) = 2700000  */
		case 800 :		/* 8.00fps */		/* ex. 1000/8000   * 54MHz(RCLK)/2(CSL) = 3375000  */ /* for Trick */
		case 750 :		/* 7.50fps */		/* ex. 1000/7500   * 54MHz(RCLK)/2(CSL) = 3600000  */
		case 500 :		/* 5.00fps */		/* ex. 1000/5000   * 54MHz(RCLK)/2(CSL) = 5400000  */
		case 400 :		/* 4.00fps */		/* ex. 1000/4000   * 54MHz(RCLK)/2(CSL) = 6750000  */ /* for Trick */
		case 300 :		/* 3.00fps */		/* ex. 1000/3000   * 54MHz(RCLK)/2(CSL) = 9000000  */
		case 200 :		/* 2.00fps */		/* ex. 1000/2000   * 54MHz(RCLK)/2(CSL) = 13500000 */
		case 100 :		/* 1.00fps */		/* ex. 1000/1000   * 54MHz(RCLK)/2(CSL) = 27000000 */
			tmr_check = (((ULLONG)time_inc * Dd_Top_Get_RCLK()) / (time_inc_resol * 2))  - 1;
			if( tmr_check > 0x00000000FFFFFFFF ){
				// 32bit check
				BF_Debug_Print_Error(("--- tmr counter is over 32bit=0x%08X\n", (UINT32)tmr_check ));
			}
			gMovie_Rld_ctl.tmrlr = (ULONG)tmr_check;
			break ;
		default:		/* No Support */
			BF_Debug_Print_Error(("--- Not Support of Timer Count. frame_rate=%ld\n", frame_rate ));
			break ;
	}
	
	gMovie_Rld_ctl.tmcsr_csl		= 0;				/* Clock Sel 2		*/
	gMovie_Rld_ctl.soft_tmrlr		= 1;				/* Soft Counter		*/
	
	/* In the case of the INTERLACE, a half cycle is used.  */
	if( scan_mode == D_BF_MOVIE_COMMON_SCAN_MODE_INTERLACE ){
		gMovie_Rld_ctl.tmrlr = (gMovie_Rld_ctl.tmrlr * 2) + 1;
	}
	
#endif	//CO_H264_PORTING_BLOCK
	
	return;
}

/**
 *	@brief		Start Timer Process for Movie.\n
 *	@param		Channel\n
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_Movie_Common_Timer_Start( E_BF_MOVIE_COMMON_RTIMER_MODE mode )
{
#ifndef CO_H264_PORTING_BLOCK
	UINT32	ch;
	INT32	ret;
	/* 32bit Timer */
#ifndef CO_DEBUG_ON_PC
	T_DD_TMR32_CTRL	tmr_ctrl;
#endif
	VP_CALLBACK			pcallback = NULL;
	
	if(mode == E_BF_MOVIE_COMMON_RTIMER_MODE_ENC){
		g_Movie_Timer_First_Flag = TRUE;
		if(g_Movie_RecTimer_Enable == FALSE){
			if(g_Movie_Timer_Start == FALSE){
				g_Movie_Timer_Start = TRUE;
				pcallback = NULL;
			}
			else{
				BF_Debug_Print_Information((">>>>BF_Movie_Common_Timer_Start: Already Start!\n"));
				return D_BF_MOVIE_COMMON_NG;
			}
		}else{
			g_Movie_Timer_Start = FALSE;
			pcallback = bf_movie_common_reload_timer_enc_inthandler;
		}
	}else if(mode == E_BF_MOVIE_COMMON_RTIMER_MODE_DEC){
		g_Movie_Timer_Start = FALSE;
		pcallback = bf_movie_common_reload_timer_dec_inthandler;
	}
	
	switch( mode ){
		case E_BF_MOVIE_COMMON_RTIMER_MODE_ENC :
			ch = D_BF_MOVIE_COMMON_RTIMER_ENC_USE_CH;
			break;
		case E_BF_MOVIE_COMMON_RTIMER_MODE_DEC :
			ch = D_BF_MOVIE_COMMON_RTIMER_DEC_USE_CH;
			break;
		default :
			BF_Debug_Print_Error(("--- BF_Movie_Common_Timer_Start() : mode error.[mode=%d]\n", mode));
			return D_BF_MOVIE_COMMON_NG;
	}
	
	if(g_Movie_Timer_Start == FALSE){
		/* 32bit Timer */
#ifdef CO_DEBUG_ON_PC
		tmr32_init();
		ret = FJ_TMR32_Open( ch );
#else
		ret = Dd_TMR32_Open(ch, D_DDIM_USER_SEM_WAIT_POL);
#endif
		if( ret != 0 ){
			BF_Debug_Print_Error(("--- BF_Movie_Common_Timer_Start() : Dd_TMR32_Open NG.\n"));
			return D_BF_MOVIE_COMMON_NG;
		}
#ifdef CO_DEBUG_ON_PC
		if( mode == E_BF_MOVIE_COMMON_RTIMER_MODE_ENC ){
			ret = FJ_TMR32_Set( ch, (ULONG)(1000000 / ((DOUBLE)(54000000/2) / (gMovie_Rld_ctl.tmrlr+1))), pcallback );
		}
		else if( mode == E_BF_MOVIE_COMMON_RTIMER_MODE_DEC ){
			ret = FJ_TMR32_Set( ch, (ULONG)(1000000 / ((DOUBLE)(54000000/2) / (gMovie_Rld_ctl.tmrlr+1))), bf_movie_common_reload_timer_dec_inthandler );
		}
		else{
		}
#else
		tmr_ctrl.csr.word		= 0;
		tmr_ctrl.csr.bit.csl	= gMovie_Rld_ctl.tmcsr_csl;
		tmr_ctrl.csr.bit.reld	= 1;
		tmr_ctrl.csr.bit.inte	= 1;
		tmr_ctrl.rlr1			= gMovie_Rld_ctl.tmrlr;
		tmr_ctrl.rlr2			= 0;
		tmr_ctrl.soft_counter	= gMovie_Rld_ctl.soft_tmrlr;
		if( mode == E_BF_MOVIE_COMMON_RTIMER_MODE_ENC ){
			tmr_ctrl.pcallback		= pcallback;	// handller setting
		}
		else if( mode == E_BF_MOVIE_COMMON_RTIMER_MODE_DEC ){
			tmr_ctrl.pcallback		= bf_movie_common_reload_timer_dec_inthandler;	// handller setting
		}
		else{
		}
		
		ret = Dd_TMR32_Ctrl( ch, &tmr_ctrl );
#endif
		if( ret != 0 ){
#ifdef CO_DEBUG_ON_PC
			ret = FJ_TMR32_Close( ch );
#else
			ret = Dd_TMR32_Close( ch );
#endif
			if( ret != 0 ){
				BF_Debug_Print_Error(("Timer32 Close NG. ret=0x%x\n", ret));
			}
			BF_Debug_Print_Error(("--- Movie_Timer_Start() : Dd_TMR32_Ctrl NG.\n"));
			return D_BF_MOVIE_COMMON_NG;
		}
		
#ifdef CO_DEBUG_ON_PC
		ret = FJ_TMR32_Start( ch, TRUE );	// Start reload mode.
#else
		ret = Dd_TMR32_Start( ch );
#endif
		if( ret != 0 ){
#ifdef CO_DEBUG_ON_PC
			ret = FJ_TMR32_Close( ch );
#else
			ret = Dd_TMR32_Close( ch );
#endif
			if( ret != 0 ){
				BF_Debug_Print_Error(("Timer32 Close NG. ret=0x%x\n", ret));
			}
			BF_Debug_Print_Error(("--- Movie_Timer_Start() : Dd_TMR32_Start NG.\n"));
			return D_BF_MOVIE_COMMON_NG;
		}
	}
	
	// In order to be able to operate immediately, set_flg here.
	if( mode == E_BF_MOVIE_COMMON_RTIMER_MODE_ENC ){
		bf_movie_common_reload_timer_enc_inthandler();
	}
	else if( mode == E_BF_MOVIE_COMMON_RTIMER_MODE_DEC ){
		bf_movie_common_reload_timer_dec_inthandler();
	}
	else{
	}


	
#endif	//CO_H264_PORTING_BLOCK
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "timer start mode=%u", mode ));
	
	return 0;
}

/**
 *	@brief		Stop Timer Process for Movie.\n
 *	@param		Channel\n
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Movie_Common_Timer_Stop( E_BF_MOVIE_COMMON_RTIMER_MODE mode )
{

	g_Movie_Timer_Start = FALSE;

#ifndef CO_H264_PORTING_BLOCK
	INT32	ret;
	UINT32	ch;
	
	switch( mode ){
		case E_BF_MOVIE_COMMON_RTIMER_MODE_ENC:
			ch = D_BF_MOVIE_COMMON_RTIMER_ENC_USE_CH;
			break;
		case E_BF_MOVIE_COMMON_RTIMER_MODE_DEC:
			ch = D_BF_MOVIE_COMMON_RTIMER_DEC_USE_CH;
			break;
		default:
			BF_Debug_Print_Error(("--- Movie_Timer_Stop() : mode error.[mode=%d]\n", mode));
			return;
	}
	
	/* 32bit Timer */
#ifdef CO_DEBUG_ON_PC
	ret = FJ_TMR32_Stop( ch );
#else
	ret = Dd_TMR32_Stop( ch );
#endif
	if( ret != 0 ){
		BF_Debug_Print_Error(("--- Movie_Timer_Stop(): Dd_TMR32_Stop NG.\n"));
	}
#ifdef CO_DEBUG_ON_PC
	FJ_TMR32_Close( ch );
#else
	Dd_TMR32_Close( ch );
#endif
#endif	//CO_H264_PORTING_BLOCK
	
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_MOVIE, FJ_MBALOG_TYPE_POINT, "timer stop mode=%u", mode ));
}


/*----------------------------------------------------------------------*/
/* Movie Timer Control													*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		frame rate to Time Increment & Time Increment Resolution convert.\n
 *	@param		i:framerate				frame rate(H264_FR_xx_xxFPS)
 *	@param		o:*vop_time_increment	Time Increment(H264_TIMEINC_xx_xx)
 *	@param		o:*vop_time_inc_resol	Time Increment Resolution(H264_TIMERESO_xx_xx)
 *	@return		None
 *	@note		When framerate is an illegal value, 0 is set. 
 *	@attention	None
*/
INT32 BF_Movie_Common_Convert_Framerate( E_BF_MOVIE_COMMON_TIMER_FRAME_RATE framerate, ULONG *vop_time_increment, ULONG *vop_time_inc_resol )
{
	*vop_time_increment = 0;
	*vop_time_inc_resol = 0;
	
	switch( framerate ){
		case E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_239_76 :
			*vop_time_increment = D_BF_MOVIE_COMMON_TIMEINC_239_76;		// 239.76fps
			*vop_time_inc_resol = D_BF_MOVIE_COMMON_TIMERESO_239_76;
			break;
		case E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_240_0 :
			*vop_time_increment = D_BF_MOVIE_COMMON_TIMEINC_240_0;		// 240fps
			*vop_time_inc_resol = D_BF_MOVIE_COMMON_TIMERESO_240_0;
			break;
		case E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_50_0 :
			*vop_time_increment = D_BF_MOVIE_COMMON_TIMEINC_50_0;		// 50fps
			*vop_time_inc_resol = D_BF_MOVIE_COMMON_TIMERESO_50_0;
			break;
		default :
			BF_Debug_Print_Error(("Movie_Convert_Framerate:timer framerate is not support(%d)\n", framerate));
			return D_BF_MOVIE_COMMON_NG;
	}
	
	return D_BF_MOVIE_COMMON_OK;
}

/**
 *	@brief		Movie Video Record Process.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Movie_Common_Sync_Setting( E_VIDEO_ENOCDE_ID video_enc_id )
{
	if ( BF_Movie_Video_Attr_Get_Enable_Flg( video_enc_id ) == FJ_MOVIE_ON ){
		
		memset((void *)&gBF_Movie_Common_Timer_Ctrl[video_enc_id], 0, sizeof( gBF_Movie_Common_Timer_Ctrl[video_enc_id] ));
		gBF_Movie_Common_Timer_Ctrl[video_enc_id].counter_cofficient	= gBF_Movie_Reload_Timer_Max_FrameRate / BF_Movie_Video_Attr_Get_Frame_Rate_Value( video_enc_id );
		gBF_Movie_Common_Timer_Ctrl[video_enc_id].counter				= 0;
		
		switch( video_enc_id ){
			case E_VIDEO_ENOCDE_ID_H264_CH0:
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H264_CH0].flgptn				= FLG_H264_SYNC_ENC_CH0;
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H264_CH0].stream_ch			= E_H264_IF_STREAM_CH_0;
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H264_CH0].timelapse_flag		= BF_Movie_Video_Attr_Get_Timelapse_Flag( video_enc_id );
#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H264_CH0].dummy_flgptn		= FLG_H264_SYNC_ENC_DUMMY_CH0;
#endif
				break;
			case E_VIDEO_ENOCDE_ID_H264_CH1:
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H264_CH1].flgptn				= FLG_H264_SYNC_ENC_CH1;
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H264_CH1].stream_ch			= E_H264_IF_STREAM_CH_1;
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H264_CH1].timelapse_flag		= BF_Movie_Video_Attr_Get_Timelapse_Flag( video_enc_id );
#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H264_CH1].dummy_flgptn		= FLG_H264_SYNC_ENC_DUMMY_CH1;
#endif
				break;
			case E_VIDEO_ENOCDE_ID_H264_CH2:
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H264_CH2].flgptn				= FLG_H264_SYNC_ENC_CH2;
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H264_CH2].stream_ch			= E_H264_IF_STREAM_CH_2;
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H264_CH2].timelapse_flag		= BF_Movie_Video_Attr_Get_Timelapse_Flag( video_enc_id );
#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H264_CH2].dummy_flgptn		= FLG_H264_SYNC_ENC_DUMMY_CH2;
#endif
				break;
			case E_VIDEO_ENOCDE_ID_H265_CH0:
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H265_CH0].flgptn				= FLG_H265_SYNC_ENC_CH0;
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H265_CH0].stream_ch			= E_H265_IF_STREAM_CH_0;
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H265_CH0].timelapse_flag		= BF_Movie_Video_Attr_Get_Timelapse_Flag( video_enc_id );
#ifdef CO_H265_ENC_DEBUG_VIEW_DUMMY
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H265_CH0].dummy_flgptn		= FLG_H265_SYNC_ENC_DUMMY_CH0;
#endif
				break;
			case E_VIDEO_ENOCDE_ID_H265_CH1:
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H265_CH1].flgptn				= FLG_H265_SYNC_ENC_CH1;
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H265_CH1].stream_ch			= E_H265_IF_STREAM_CH_1;
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H265_CH1].timelapse_flag		= BF_Movie_Video_Attr_Get_Timelapse_Flag( video_enc_id );
#ifdef CO_H265_ENC_DEBUG_VIEW_DUMMY
				gBF_Movie_Common_Timer_Ctrl[E_VIDEO_ENOCDE_ID_H265_CH1].dummy_flgptn		= FLG_H265_SYNC_ENC_DUMMY_CH1;
#endif
				break;
			default:
				// Do Nothing.
				break;
		}
	}
	
	return;
}

/**
 *	@brief		Reload Timer Start for View.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Movie_Common_Timer_Start_View( E_VIDEO_ENOCDE_ID video_enc_id )
{
	INT32	ret;
	
	/* Encode Sync Start */
	if(BF_Movie_Common_Get_Timer_Start_Flag(video_enc_id) == FALSE){
		ret = BF_Movie_Common_Timer_Start( E_BF_MOVIE_COMMON_RTIMER_MODE_ENC );
		if( ret != D_BF_MOVIE_COMMON_OK ){
			BF_Debug_Print_Warning(("BF_Movie_Common_Timer_Start_View: Can't Start Timer!!\n"));
		}
		BF_Movie_Common_Set_Timer_Start_Flag(video_enc_id, TRUE);
	}
	return;
}

/**
 *	@brief		Set TIMER Start flag.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Movie_Common_Set_Timer_Start_Flag(E_VIDEO_ENOCDE_ID video_enc_id, BOOL start)
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );
	gMovie_Timer_Start_Flag[video_enc_id] = start;
	Dd_ARM_Critical_Section_End( critseq );
}

/**
 *	@brief		Get TIMER Start flag.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
BOOL BF_Movie_Common_Get_Timer_Start_Flag( E_VIDEO_ENOCDE_ID video_enc_id )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	BOOL ret;
	
	Dd_ARM_Critical_Section_Start( critseq );
	ret = gMovie_Timer_Start_Flag[video_enc_id];
	Dd_ARM_Critical_Section_End( critseq );
	
	return ret;
}

/**
 *	@brief		ReSet TIMER Counter.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_Movie_Common_Timer_Counter_Reset(VOID)
{
	E_VIDEO_ENOCDE_ID enc_id_loop;
	INT32 ret = D_BF_MOVIE_COMMON_NG;
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	
	for( enc_id_loop=E_VIDEO_ENOCDE_ID_H264_CH0; enc_id_loop<E_VIDEO_ENOCDE_ID_MAX; enc_id_loop++ ){
		if( gBF_Movie_Common_Timer_Ctrl[enc_id_loop].sync_start == D_BF_MOVIE_COMMON_ON ){
			BF_Debug_Print_Information((">>>>BF_Movie_Timer_Control: Use CH exist Timer Not Reset.\n"));
			return D_BF_MOVIE_COMMON_OK;
		}
	}
	
	BF_Movie_Common_Timer_Stop( E_BF_MOVIE_COMMON_RTIMER_MODE_ENC );
	Dd_ARM_Critical_Section_Start( critseq );
	gReload_Timer_Count = 0;
	Dd_ARM_Critical_Section_End( critseq );
	
	for( enc_id_loop=E_VIDEO_ENOCDE_ID_H264_CH0; enc_id_loop<E_VIDEO_ENOCDE_ID_MAX; enc_id_loop++ ){
		BF_Movie_Common_Set_Timer_Start_Flag(enc_id_loop, FALSE);
	}
	
	gBF_Movie_Common_Timer_Frame_Rate = E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_INITIALIZE;
	
	BF_Debug_Print_Information((">>>>BF_Movie_Timer_Control: Timer Reset OK.\n"));
	ret = D_BF_MOVIE_COMMON_OK;
	
	return ret;
}

/**
 *	@brief		Get TIMER Counter.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
ULLONG BF_Movie_Common_Timer_Counter_Get( VOID )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	ULLONG	timer_count;
	
	Dd_ARM_Critical_Section_Start( critseq );
	if(gReload_Timer_Count == 0){
		timer_count = 0;
	}
	else{
		timer_count = gReload_Timer_Count-1;
	}
	Dd_ARM_Critical_Section_End( critseq );
	
	return timer_count;
}

/**
 *	@brief		Movie Timer Setting.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_Movie_Common_Timer_Setting( E_VIDEO_ENOCDE_ID video_enc_id )
{
	E_BF_MOVIE_COMMON_TIMER_FRAME_RATE	timer_frame_rate;
	UINT32	timer_max_frame_rate;
	
	ULONG	vop_time_increment;
	ULONG	vop_time_inc_resol;
	
	if( BF_Movie_Common_Get_Timer_Start_Flag(video_enc_id) == TRUE ){
		BF_Debug_Print_Information(("Reload timer cannot be changed during timer starting. \n"));
	}
	
	// The frame rate type is decide.
	if( video_enc_id < E_VIDEO_ENOCDE_ID_H265_CH0 ){
		if( BF_Movie_Video_Attr_Get_Enable_Flg( video_enc_id ) == FJ_MOVIE_ON ){
			switch( BF_Movie_Video_Attr_Get_Frame_Rate( video_enc_id ) ){
				// H264
				case E_H264_IF_FRAME_RATE_239_76 :	// 239.76fps
				case E_H264_IF_FRAME_RATE_119_88 :	// 119.88fps
				case E_H264_IF_FRAME_RATE_59_94 :	// 59.94fps
				case E_H264_IF_FRAME_RATE_47_95 :	// 47.95fps
				case E_H264_IF_FRAME_RATE_29_97 :	// 29.97fps
				case E_H264_IF_FRAME_RATE_23_97 :	// 23.97fps
				case E_H264_IF_FRAME_RATE_14_98 :	// 14.98fps
				case E_H264_IF_FRAME_RATE_9_99 :	// 9.99fps
				case E_H264_IF_FRAME_RATE_4_99 :	// 4.99fps
				case E_H264_IF_FRAME_RATE_2_99 :	// 2.99fps
				case E_H264_IF_FRAME_RATE_1_99 :	// 1.99fps
				case E_H264_IF_FRAME_RATE_0_99 :	// 0.99fps
					timer_frame_rate = E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_239_76; // Timer  1001/240000
					timer_max_frame_rate = 23976;
					break;
				// H264
				case E_H264_IF_FRAME_RATE_240_0 :	// 240fps
				case E_H264_IF_FRAME_RATE_120_0 :	// 120fps
				case E_H264_IF_FRAME_RATE_60_0 :	// 60fps
				case E_H264_IF_FRAME_RATE_48_0 :	// 48fps
				case E_H264_IF_FRAME_RATE_30_0 :	// 30fps
				case E_H264_IF_FRAME_RATE_24_0 :	// 24fps
				case E_H264_IF_FRAME_RATE_15_0 :	// 15fps
				case E_H264_IF_FRAME_RATE_10_0 :	// 10fps
				case E_H264_IF_FRAME_RATE_7_5 :		// 7.5fps
				case E_H264_IF_FRAME_RATE_5_0 :		// 5fps
				case E_H264_IF_FRAME_RATE_3_0 :		// 3fps
				case E_H264_IF_FRAME_RATE_2_0 :		// 2fps
				case E_H264_IF_FRAME_RATE_1_0 :		// 1fps
					timer_frame_rate = E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_240_0; // Timer  1000/240000
					timer_max_frame_rate = 24000;
					break;
				case E_H264_IF_FRAME_RATE_50_0 :		// 50fps
				case E_H264_IF_FRAME_RATE_25_0 :		// 25fps
				case E_H264_IF_FRAME_RATE_12_5 :		// 12.5fps
					timer_frame_rate = E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_50_0; // Timer  1000/50000
					timer_max_frame_rate = 5000;
					break;
				default :
					BF_Debug_Print_Error(("BF_Movie_Common_Timer_Setting: not support frame rate\n"));
					return D_BF_MOVIE_COMMON_NG;
			}
		}
		else{
			BF_Debug_Print_Error(("BF_Movie_Common_Timer_Setting: This ID is not enable \n"));
			return D_BF_MOVIE_COMMON_NG;
		}
	}
	else{
		if( BF_Movie_Video_Attr_Get_Enable_Flg( video_enc_id ) == FJ_MOVIE_ON ){
			switch( BF_Movie_Video_Attr_Get_Frame_Rate( video_enc_id ) ){
				case E_H265_IF_FRAME_RATE_239_76 :	// 239.76fps
				case E_H265_IF_FRAME_RATE_119_88 :	// 119.88fps
				case E_H265_IF_FRAME_RATE_59_94 :	// 59.94fps
				case E_H265_IF_FRAME_RATE_29_97 :	// 29.97fps
				case E_H265_IF_FRAME_RATE_23_97 :	// 23.97fps
				case E_H265_IF_FRAME_RATE_14_98 :	// 14.98fps
					timer_frame_rate = E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_239_76; // Timer  1001/240000
					timer_max_frame_rate = 23976;
					break;
				case E_H265_IF_FRAME_RATE_240_0 :	// 240fps
				case E_H265_IF_FRAME_RATE_120_0 :	// 120fps
				case E_H265_IF_FRAME_RATE_73_0 :
				case E_H265_IF_FRAME_RATE_72_0 :
				case E_H265_IF_FRAME_RATE_70_0 :
				case E_H265_IF_FRAME_RATE_60_0 :	// 60fps
				case E_H265_IF_FRAME_RATE_50_0 :
				case E_H265_IF_FRAME_RATE_30_0 :	// 30fps
				case E_H265_IF_FRAME_RATE_29_0 :
				case E_H265_IF_FRAME_RATE_25_0 :
				case E_H265_IF_FRAME_RATE_24_0 :	// 24fps
				case E_H265_IF_FRAME_RATE_15_0 :	// 15fps
					timer_frame_rate = E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_240_0; // Timer  1000/240000
					timer_max_frame_rate = 24000;
					break;
				default :
					BF_Debug_Print_Error(("BF_Movie_Common_Timer_Setting: not support frame rate\n"));
					return D_BF_MOVIE_COMMON_NG;
			}
		}
		else{
			BF_Debug_Print_Error(("BF_Movie_Common_Timer_Setting: This ID is not enable \n"));
			return D_BF_MOVIE_COMMON_NG;
		}
	}
	if( gBF_Movie_Common_Timer_Frame_Rate != E_BF_MOVIE_COMMON_TIMER_FRAME_RATE_INITIALIZE ){
		if( gBF_Movie_Common_Timer_Frame_Rate != timer_frame_rate ){
			BF_Debug_Print_Error(("BF_Movie_Common_Timer_Setting: not support frame rate conbination\n"));
			return D_BF_MOVIE_COMMON_NG;
		}
	}
	else{
		gBF_Movie_Common_Timer_Frame_Rate = timer_frame_rate;
		gBF_Movie_Reload_Timer_Max_FrameRate = timer_max_frame_rate;
		
		memset( (void *)gBF_Movie_Common_Timer_Ctrl, 0, sizeof( gBF_Movie_Common_Timer_Ctrl ));
		
		BF_Movie_Common_Convert_Framerate( timer_frame_rate, &vop_time_increment, &vop_time_inc_resol );
		gVop_Time_Increment = vop_time_increment;
		gVop_Time_Inc_Resol = vop_time_inc_resol;
		
		if( BF_Movie_Common_Get_Timer_Start_Flag(video_enc_id) == FALSE ){
			BF_Movie_Common_Timer_Calculate( vop_time_increment, vop_time_inc_resol, D_BF_MOVIE_COMMON_SCAN_MODE_PROGRESSIVE );
		}
	}
	
	return D_BF_MOVIE_COMMON_OK;
}

/**
 *	@brief		H264 Sync Start.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Movie_Common_Sync_Start_H264( ULONG stream_ch )
{
	E_VIDEO_ENOCDE_ID enc_id;
	
	BF_Movie_Video_Attr_Change_StreamCH_to_EncodeID_H264( (E_H264_IF_STREAM_CH)stream_ch, &enc_id );

	gBF_Movie_Common_Timer_Ctrl[enc_id].sync_start = D_BF_MOVIE_COMMON_ON;
	BF_Debug_Print_Information(("%s() Common Sync Start[%d]!!\n", __func__, enc_id));
}

/**
 *	@brief		H264 Sync Stop.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Movie_Common_Sync_Stop_H264( VOID )
{
	E_VIDEO_ENOCDE_ID enc_id_loop;
	
	for( enc_id_loop=E_VIDEO_ENOCDE_ID_H264_CH0; enc_id_loop<E_VIDEO_ENOCDE_ID_H265_CH0; enc_id_loop++ ){
		gBF_Movie_Common_Timer_Ctrl[enc_id_loop].sync_start = D_BF_MOVIE_COMMON_OFF;
	}
	BF_Debug_Print_Information(("%s() Common Sync Stop ALL!!\n", __func__));
}

/**
 *	@brief		H265 Sync Start.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Movie_Common_Sync_Start_H265( ULONG stream_ch )
{
	E_VIDEO_ENOCDE_ID enc_id;
	
	BF_Movie_Video_Attr_Change_StreamCH_to_EncodeID_H265( (E_H265_IF_STREAM_CH)stream_ch, &enc_id );

	gBF_Movie_Common_Timer_Ctrl[enc_id].sync_start = D_BF_MOVIE_COMMON_ON;
	BF_Debug_Print_Information(("%s() Common Sync Start[%d]!!\n", __func__, enc_id));
}

/**
 *	@brief		H265 Sync Stop.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Movie_Common_Sync_Stop_H265( VOID )
{
	E_VIDEO_ENOCDE_ID enc_id_loop;
	
	for( enc_id_loop=E_VIDEO_ENOCDE_ID_H265_CH0; enc_id_loop<E_VIDEO_ENOCDE_ID_MAX; enc_id_loop++ ){
		gBF_Movie_Common_Timer_Ctrl[enc_id_loop].sync_start = D_BF_MOVIE_COMMON_OFF;
	}
	BF_Debug_Print_Information(("%s() Common Sync Stop ALL!!\n", __func__));
}

#if defined(CO_H264_ENC_DEBUG_VIEW_DUMMY) || defined(CO_H265_ENC_DEBUG_VIEW_DUMMY)
VOID BF_Movie_Common_Set_Dummy_Sync( E_VIDEO_ENOCDE_ID enc_id, BOOL flag )
{
	gMovie_Timer_Dummy_Sync_Flag[enc_id] = flag;
	return;
}
#endif
