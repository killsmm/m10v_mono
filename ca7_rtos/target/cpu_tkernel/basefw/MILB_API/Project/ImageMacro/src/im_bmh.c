/**
 * @file		im_bmh.c
 * @brief		JMLBMH2A Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include <string.h>
#include "im_bmh.h"
#include "driver_common.h"
// #if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
// #endif
#include "dd_arm.h"
#include "ddim_user_custom.h"

#include "jmlbmh.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// #define CO_BMH_DEBUG
// #define CO_BMH_REG_TYPE_CHECK

// Low power consumption option
#ifdef CO_ACT_CLOCK
#define CO_ACT_BMHCLOCK
#endif	// CO_ACT_CLOCK
#ifdef CO_ACT_ICLOCK
#define CO_ACT_BMH_ICLK
#endif	// CO_ACT_ICLOCK

#define D_IM_BMH_SYNC				(0)				// Sync Processing
#define D_IM_BMH_ASYNC				(1)				// Async Processing
#define D_IM_BMH_SRAM_WAIT_USEC		(1)

#define D_IM_BMH_WAIT_FLGPTN		(D_IM_BMH_FLG_BMH_END | D_IM_BMH_FLG_BMH_AXI_ERR)

#define Im_BMH_Dsb()				Dd_ARM_Dsb_Pou()

#define im_bmh_wait_usec( usec )	Dd_ARM_Wait_ns( (usec * 1000) )

#ifdef CO_BMH_REG_TYPE_CHECK
#define im_bmh_check_reg_type( dst, src )	(dst) = (src);
#else	// CO_BMH_REG_TYPE_CHECK
#define im_bmh_check_reg_type( dst, src )
#endif	// CO_BMH_REG_TYPE_CHECK

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* Nothing Special */
// SLimbus channel_no
typedef enum {
	E_IM_BMH_CHANNEL_00 = 0,			// Channel 0
	E_IM_BMH_CHANNEL_01,				// Channel 1
	E_IM_BMH_CHANNEL_END				// End flag
} E_IM_BMH_CHANNEL;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
typedef struct {
	ULONG lock;
	ULONG dummy[(64 / 4) - 1];
} T_IM_BMH_SPIN_LOCK_VAR;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------

static volatile T_IM_BMH_CALLBACK gIM_BMH_CallBack_Func = NULL;
#ifdef CO_ACT_BMHCLOCK
static volatile UCHAR gIM_BMH_Clk_Ctrl_Cnt[2] = {0,0};
#endif	// CO_ACT_BMHCLOCK
#ifdef CO_ACT_BMH_ICLK
static volatile UCHAR gIM_BMH_Iclk_Ctrl_Cnt[2] = {0,0};
#endif	// CO_ACT_BMH_ICLK

#if defined(CO_ACT_BMH_ICLK) || defined(CO_ACT_BMHCLOCK)
// Spin_lock.
static T_IM_BMH_SPIN_LOCK_VAR gImBmh_Spin_Lock_Tbl[2] __attribute__((section(".LOCK_SECTION"), aligned(64)));
#endif	// CO_ACT_BMH_ICLK || CO_ACT_BMHCLOCK

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE


/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------

static VOID im_bmh_set_bmhinte( BYTE channel_no, const UCHAR onoff );
static VOID im_bmh_clear_axif( BYTE channel_no );
static VOID im_bmh_clear_bmhf( BYTE channel_no );
static INT32 im_bmh_start_core( BYTE channel_no, const UCHAR sync, const INT32 wait_time );
static VOID im_BMH_on_bmh_clk( BYTE channel_no );
static VOID im_BMH_off_bmh_clk( BYTE channel_no );


#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE


/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
VOID im_bmh_memcpy_ulong( ULONG* const dst, ULONG* const src, UINT32 cp_size )
{
	UINT32	i;
	ULONG	words;

	words = ( cp_size + sizeof(ULONG) - 1 ) / sizeof(ULONG);

	for ( i = 0; i < words; i++ ){
		dst[i] = src[i];
	}
}

//---------------------------- driver section ----------------------------
// Interrupt Enable/Disable control
static VOID im_bmh_set_bmhinte( BYTE channel_no, const UCHAR onoff )
{
	union io_bmh_bmhinte bmhinte;

	bmhinte.word = 0;
	bmhinte.bit.BMHE = onoff;
	bmhinte.bit.AXIE = onoff;

	IO_BMH[channel_no].BMHINTE.word = bmhinte.word;
}

// AXIF Interrupt flag clear
static VOID im_bmh_clear_axif( BYTE channel_no )
{
#ifndef CO_DEBUG_ON_PC
	union io_bmh_bmhintfs bmhintfs;
#endif	// CO_DEBUG_ON_PC


#ifdef CO_DEBUG_ON_PC
	IO_BMH[channel_no].BMHINTFS.bit.__AXIF = 0;
#else	// !CO_DEBUG_ON_PC
	bmhintfs.word = 0;
	bmhintfs.bit.__AXIF = 1;

	IO_BMH[channel_no].BMHINTFS.word = bmhintfs.word;
#endif	// !CO_DEBUG_ON_PC
}

// BMHF Interrupt flag clear
static VOID im_bmh_clear_bmhf( BYTE channel_no )
{
#ifndef CO_DEBUG_ON_PC
	union io_bmh_bmhintfs bmhintfs;
#endif	// CO_DEBUG_ON_PC

#ifdef CO_DEBUG_ON_PC
	IO_BMH[channel_no].BMHINTFS.bit.__BMHF = 0;
#else	// !CO_DEBUG_ON_PC
	bmhintfs.word = 0;
	bmhintfs.bit.__BMHF = 1;

	IO_BMH[channel_no].BMHINTFS.word = bmhintfs.word;
#endif	// !CO_DEBUG_ON_PC
}

/* Is Macro active
 */
static UCHAR im_bmh_is_act( BYTE channel_no )
{
	union io_bmh_bmhtrg bmhtrg;

	bmhtrg.word = IO_BMH[channel_no].BMHTRG.word;

	if( bmhtrg.bit.BMHTRG == 3 ){			/* pgr0872 */
		return 1;
	}
	else{
		return 0;
	}
}

// BMH Start CORE
static INT32 im_bmh_start_core( BYTE channel_no, const UCHAR sync, const INT32 wait_time )
{
	INT32 ercd;
	UINT32 flg_ptn;
	union io_bmh_bmhtrg bmhtrg;
	T_IM_BMH_BMHRE sram;

	// BMH End Flag disable
	DDIM_User_Clr_Flg( FID_IM_BMH, ~D_IM_BMH_WAIT_FLGPTN );

	im_BMH_on_bmh_clk( channel_no );
	Im_BMH_Dsb();

	sram.bmhre = 1;
	Im_BMH_Set_Bmhre( channel_no, &sram );

	bmhtrg.word = 0;
	bmhtrg.bit.BMHTRG = 1;
	IO_BMH[channel_no].BMHTRG.word = bmhtrg.word;

	Im_BMH_Dsb();
	if( sync == D_IM_BMH_ASYNC ){
		return D_DDIM_OK;
	}

#ifdef CO_DEBUG_ON_PC
	if(( IO_BMH[channel_no].BMHINTFS.bit.__BMHF != 0 )
	|| ( IO_BMH[channel_no].BMHINTFS.bit.__AXIF != 0 )){
		Im_BMH_Int_Handler( channel_no );
	}
#endif	// CO_DEBUG_ON_PC

	// Wait for Process end with Sync
	ercd = Im_BMH_Wait_End( channel_no, &flg_ptn, wait_time );
	if( ercd != D_DDIM_OK ){
		return ercd;
	}
	if(( flg_ptn & D_IM_BMH_FLG_BMH_AXI_ERR ) != 0 ){
		return D_IM_BMH_AXI_ERR;
	}

	return D_DDIM_OK;
}

// Start BMH ICLK, BMHCLK
static VOID im_BMH_on_bmh_clk( BYTE channel_no )
{
#if defined(CO_ACT_BMH_ICLK) || defined(CO_ACT_BMHCLOCK)
	Dd_ARM_Critical_Section_Start( gImBmh_Spin_Lock_Tbl[channel_no] );
#endif	// CO_ACT_BMH_ICLK || CO_ACT_BMHCLOCK

	if( channel_no == E_IM_BMH_CHANNEL_00 ){
#ifdef CO_ACT_BMHCLOCK
		Dd_Top_Start_Clock( (UCHAR*)&gIM_BMH_Clk_Ctrl_Cnt[0], &Dd_Top_Get_CLKSTOP14(), ~D_DD_TOP_BMH0CK_BIT );
#endif	// CO_ACT_BMHCLOCK
#ifdef CO_ACT_BMH_ICLK
		Dd_Top_Start_Clock( (UCHAR*)&gIM_BMH_Iclk_Ctrl_Cnt[0], &Dd_Top_Get_CLKSTOP14(), ~D_DD_TOP_BMH0AX_BIT );
#endif	// CO_ACT_BMH_ICLK
	}

	if( channel_no == E_IM_BMH_CHANNEL_01 ){
#ifdef CO_ACT_BMHCLOCK
		Dd_Top_Start_Clock( (UCHAR*)&gIM_BMH_Clk_Ctrl_Cnt[1], &Dd_Top_Get_CLKSTOP13(), ~D_DD_TOP_BMH1CK_BIT );
#endif	// CO_ACT_BMHCLOCK
#ifdef CO_ACT_BMH_ICLK
		Dd_Top_Start_Clock( (UCHAR*)&gIM_BMH_Iclk_Ctrl_Cnt[1], &Dd_Top_Get_CLKSTOP14(), ~D_DD_TOP_BMH1AX_BIT );
#endif	// CO_ACT_BMH_ICLK
	}

#if defined(CO_ACT_BMH_ICLK) || defined(CO_ACT_BMHCLOCK)
	Dd_ARM_Critical_Section_End( gImBmh_Spin_Lock_Tbl[channel_no] );
#endif	// CO_ACT_BMH_ICLK || CO_ACT_BMHCLOCK
}

// Stop BMH ICLK, BMHCLK
static VOID im_BMH_off_bmh_clk( BYTE channel_no )
{
#if defined(CO_ACT_BMH_ICLK) || defined(CO_ACT_BMHCLOCK)
	Dd_ARM_Critical_Section_Start( gImBmh_Spin_Lock_Tbl[channel_no] );
#endif	// CO_ACT_BMH_ICLK || CO_ACT_BMHCLOCK

	if( channel_no == E_IM_BMH_CHANNEL_00 ){
#ifdef CO_ACT_BMH_ICLK
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_BMH_Iclk_Ctrl_Cnt[0], &Dd_Top_Get_CLKSTOP14(), D_DD_TOP_BMH0AX_BIT );
#endif	// CO_ACT_BMH_ICLK
#ifdef CO_ACT_BMHCLOCK
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_BMH_Clk_Ctrl_Cnt[0], &Dd_Top_Get_CLKSTOP14(), D_DD_TOP_BMH0CK_BIT );
#endif	// CO_ACT_BMHCLOCK
	}

	if( channel_no == E_IM_BMH_CHANNEL_01 ){
#ifdef CO_ACT_BMH_ICLK
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_BMH_Iclk_Ctrl_Cnt[1], &Dd_Top_Get_CLKSTOP14(), D_DD_TOP_BMH1AX_BIT );
#endif	// CO_ACT_BMH_ICLK
#ifdef CO_ACT_BMHCLOCK
		Dd_Top_Stop_Clock( (UCHAR*)&gIM_BMH_Clk_Ctrl_Cnt[1], &Dd_Top_Get_CLKSTOP13(), D_DD_TOP_BMH1CK_BIT );
#endif	// CO_ACT_BMHCLOCK
	}

#if defined(CO_ACT_BMH_ICLK) || defined(CO_ACT_BMHCLOCK)
	Dd_ARM_Critical_Section_End( gImBmh_Spin_Lock_Tbl[channel_no] );
#endif	// CO_ACT_BMH_ICLK || CO_ACT_BMHCLOCK
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/*
The semaphore of BMH is acquired
*/
INT32 Im_BMH_Open( BYTE channel_no, const INT32 tmout )
{
	DDIM_USER_ID sid;
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Open. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( tmout < D_DDIM_USER_SEM_WAIT_FEVR ){
		Ddim_Assertion(("I:Im_BMH_Open. input param error. tmout = %x\n", tmout ));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	sid = SID_IM_BMH( channel_no );
	if( tmout == D_DDIM_USER_SEM_WAIT_POL ){
		ercd = DDIM_User_Pol_Sem( sid );				// pol_sem()
	}
	else{
		ercd = DDIM_User_Twai_Sem( sid, (DDIM_USER_TMO)tmout );	// twai_sem()
	}

	if( ercd == D_DDIM_USER_E_TMOUT ){
		// A semaphore acquisition processing time out
		Ddim_Print(("I:Im_BMH_Open() Error : Semaphore Get Time Out\n"));
		return D_IM_BMH_SEM_TIMEOUT_ERR;
	}

	if( ercd != D_DDIM_USER_E_OK ){
		// BMH processing error
		Ddim_Print(("I:Im_BMH_Open() Error : Semaphore Get Error\n"));
		return D_IM_BMH_SEM_NG;
	}

	return D_DDIM_OK;
}

/*
Set software reset and operating mode
*/
INT32 Im_BMH_Init( BYTE channel_no )
{
	union io_bmh_bmhsr bmhsr;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Init. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK
	im_BMH_on_bmh_clk( channel_no );

	bmhsr.word = IO_BMH[channel_no].BMHSR.word;
	bmhsr.bit.SR = 1;
	IO_BMH[channel_no].BMHSR.word = bmhsr.word;

	Im_BMH_Dsb();

	bmhsr.word = IO_BMH[channel_no].BMHSR.word;
	bmhsr.bit.SR = 0;
	IO_BMH[channel_no].BMHSR.word = bmhsr.word;

	Im_BMH_Dsb();

	im_bmh_clear_bmhf( channel_no );	// Interrupt clear
	im_bmh_clear_axif( channel_no );	// Interrupt clear
	im_bmh_set_bmhinte( channel_no, 1 );

	im_BMH_off_bmh_clk( channel_no );
	Im_BMH_Dsb();

	return D_DDIM_OK;
}

/*
BMH Start Sync
*/
INT32 Im_BMH_Start_Sync( BYTE channel_no, const INT32 wait_time )
{
#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Start_Sync. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	return im_bmh_start_core( channel_no, D_IM_BMH_SYNC, wait_time );
}

/*
BMH Start Async
*/
INT32 Im_BMH_Start_Async( BYTE channel_no )
{
#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Start_Async. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	return im_bmh_start_core( channel_no, D_IM_BMH_ASYNC, 0 );	// wait_time is dummy.
}

/*
BMH Wait End
*/
INT32 Im_BMH_Wait_End( BYTE channel_no, UINT32* const p_wait_factor, const INT32 wait_time )
{
	DDIM_USER_FLGPTN flgptn;
	DDIM_USER_FLGPTN waiptn;
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Wait_End. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( p_wait_factor == NULL ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Wait_End. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	waiptn = D_IM_BMH_WAIT_FLGPTN;
	ercd = DDIM_User_Twai_Flg( FID_IM_BMH, waiptn, D_DDIM_USER_TWF_ORW, &flgptn, wait_time );
	if( ercd == D_DDIM_USER_E_TMOUT ){
		// A semaphore acquisition processing time out
		Ddim_Print(("Im_BMH_Wait_End() time out. ercd = %d\n", ercd ));
		return D_IM_BMH_SEM_TIMEOUT_ERR;
	}

	if( ercd != D_DDIM_USER_E_OK ){
		// A semaphore acquisition processing error
		Ddim_Print(("Im_BMH_Wait_End() system error. ercd = %d\n", ercd ));
		return D_IM_BMH_SEM_NG;
	}

	DDIM_User_Clr_Flg( FID_IM_BMH, ~flgptn );

	*p_wait_factor = (UINT32)flgptn;

	if(( flgptn & D_IM_BMH_AXI_ERR ) != 0 ){
		return D_IM_BMH_AXI_ERR;
	}

	return D_DDIM_OK;
}

/*
BMH Stop
*/
INT32 Im_BMH_Stop( BYTE channel_no )
{
	union io_bmh_bmhtrg bmhtrg;
	union io_bmh_bmhre bmhre;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Stop. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	bmhtrg.word = IO_BMH[channel_no].BMHTRG.word;

	if( bmhtrg.bit.BMHTRG == 3 ){			/* pgr0872 */
		bmhtrg.bit.BMHTRG = 0;
		IO_BMH[channel_no].BMHTRG.word = bmhtrg.word;
#ifdef CO_DEBUG_ON_PC
		IO_BMH[channel_no].BMHTRG.bit.BMHTRG = 2;
#endif	// CO_DEBUG_ON_PC
	}

	// Set SRAM Sleep
	bmhre.word = 0;
	bmhre.bit.BMHRE = 0;
	IO_BMH[channel_no].BMHRE.word = bmhre.word;

	// wait 1us
	im_bmh_wait_usec(D_IM_BMH_SRAM_WAIT_USEC);

	im_BMH_off_bmh_clk( channel_no );
	Im_BMH_Dsb();
//	Ddim_Print("I: [BMH] func disable.\n");

	return D_DDIM_OK;
}

/*
The semaphore of BMH is returned
*/
INT32 Im_BMH_Close( BYTE channel_no )
{
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Close. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_BMH_off_bmh_clk( channel_no );
	Im_BMH_Dsb();

	ercd = DDIM_User_Sig_Sem( SID_IM_BMH( channel_no ) );				// sig_sem()

	if( ercd != D_DDIM_USER_E_OK ){
		// BMH processing error
		Ddim_Print(("I:Im_BMH_Close() Error.\n"));
		return D_IM_BMH_SEM_NG;
	}
	return D_DDIM_OK;
}

/*
BMH Control
*/
INT32 Im_BMH_Ctrl( BYTE channel_no, const T_IM_BMH_CTRL* const bmh_ctrl )
{
	union io_bmh_bmhmd bmhmd;
	union io_bmh_bmhprm bmhprm;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Ctrl. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( bmh_ctrl == NULL ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Ctrl. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if( im_bmh_is_act( channel_no ) ){
		return D_IM_BMH_BUSY_NG;
	}

	bmhmd.word = 0;
	bmhprm.word = 0;

	bmhmd.bit.LSELCNT  = bmh_ctrl->lselcnt;
	bmhmd.bit.RBS      = bmh_ctrl->rbs;
	bmhmd.bit.PSHIFT   = bmh_ctrl->pshift;
	bmhmd.bit.OUTMD    = bmh_ctrl->outmd;
	bmhmd.bit.LUTMD    = bmh_ctrl->lutmd;
	bmhmd.bit.OFFMD    = bmh_ctrl->offmd;
	bmhmd.bit.MODE     = bmh_ctrl->mode;

	bmhprm.bit.POFFSET = bmh_ctrl->poffset;
	bmhprm.bit.SADLV   = bmh_ctrl->sadlv;

	IO_BMH[channel_no].BMHMD.word = bmhmd.word;
	IO_BMH[channel_no].BMHPRM.word = bmhprm.word;

	gIM_BMH_CallBack_Func = bmh_ctrl->pCallBack;

	return D_DDIM_OK;
}

/*
Get BMH Control
*/
INT32 Im_BMH_Get_Ctrl( BYTE channel_no, T_IM_BMH_CTRL* const bmh_ctrl )
{
	union io_bmh_bmhmd bmhmd;
	union io_bmh_bmhprm bmhprm;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Ctrl. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( bmh_ctrl == NULL ){
		// bmh_ctrl setting error
		Ddim_Assertion(("I:Im_BMH_Get_Ctrl. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	bmhmd.word = IO_BMH[channel_no].BMHMD.word;
	bmhprm.word = IO_BMH[channel_no].BMHPRM.word;

	bmh_ctrl->lselcnt = bmhmd.bit.LSELCNT;			/* pgr0872 */
	bmh_ctrl->rbs     = bmhmd.bit.RBS;				/* pgr0872 */
	bmh_ctrl->pshift  = bmhmd.bit.PSHIFT;			/* pgr0872 */
	bmh_ctrl->outmd   = bmhmd.bit.OUTMD;			/* pgr0872 */
	bmh_ctrl->lutmd   = bmhmd.bit.LUTMD;			/* pgr0872 */
	bmh_ctrl->offmd   = bmhmd.bit.OFFMD;			/* pgr0872 */
	bmh_ctrl->mode    = bmhmd.bit.MODE;				/* pgr0872 */

	bmh_ctrl->poffset = bmhprm.bit.POFFSET;			/* pgr0872 */
	bmh_ctrl->sadlv   = bmhprm.bit.SADLV;			/* pgr0872 */

	return D_DDIM_OK;
}

/*
BMH BMHRE set
*/
INT32 Im_BMH_Set_Bmhre( BYTE channel_no, const T_IM_BMH_BMHRE* const sram )
{
	union io_bmh_bmhre bmhre;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Bmhre. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( sram == NULL ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Bmhre. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if( im_bmh_is_act( channel_no ) ){
		return D_IM_BMH_BUSY_NG;
	}

	bmhre.word = 0;

	bmhre.bit.BMHRE = sram->bmhre;

	IO_BMH[channel_no].BMHRE.word = bmhre.word;

	// wait 1us
	im_bmh_wait_usec( D_IM_BMH_SRAM_WAIT_USEC );

	return D_DDIM_OK;
}

/*
Get BMH BMHRE
*/
INT32 Im_BMH_Get_Bmhre( BYTE channel_no, T_IM_BMH_BMHRE* const sram )
{
	union io_bmh_bmhre bmhre;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Bmhre. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( sram == NULL ){
		// cnt setting error
		Ddim_Assertion(("I:Im_BMH_Get_Bmhre. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	bmhre.word = IO_BMH[channel_no].BMHRE.word;

	sram->bmhre = bmhre.bit.BMHRE;			/* pgr0872 */

	return D_DDIM_OK;
}

/*
Get BMH BMHINTFS-RSST
*/
INT32 Im_BMH_Get_Rsst( BYTE channel_no, T_IM_BMH_RSST* const rsst )
{
	union io_bmh_bmhintfs bmhintfs;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Rsst. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( rsst == NULL ){
		// cnt setting error
		Ddim_Assertion(("I:Im_BMH_Get_Rsst. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	bmhintfs.word = IO_BMH[channel_no].BMHINTFS.word;

	rsst->rsst = bmhintfs.bit.__RSST;			/* pgr0872 */

	return D_DDIM_OK;
}

/*
BMH BMHCONT set
*/
INT32 Im_BMH_Set_Bmhcont( BYTE channel_no, const T_IM_BMH_BMHCONT* const cont )
{
	union io_bmh_bmhcont bmhcont;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Bmhcont. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( cont == NULL ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Bmhcont. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	bmhcont.word = 0;

	bmhcont.bit.CONT = cont->cont;

	IO_BMH[channel_no].BMHCONT.word = bmhcont.word;

	return D_DDIM_OK;
}

/*
Get BMH BMHCONT
*/
INT32 Im_BMH_Get_Bmhcont( BYTE channel_no, T_IM_BMH_BMHCONT* const cont )
{
	union io_bmh_bmhcont bmhcont;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Bmhcont. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( cont == NULL ){
		// cnt setting error
		Ddim_Assertion(("I:Im_BMH_Get_Bmhcont. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	bmhcont.word = IO_BMH[channel_no].BMHCONT.word;

	cont->cont = bmhcont.bit.CONT;			/* pgr0872 */

	return D_DDIM_OK;
}

/*
BMH Control set AXI bus I/F Control
*/
INT32 Im_BMH_Set_Bmhaxi( BYTE channel_no, const T_IM_BMH_AXI* const axi_ctrl )
{
	union io_bmh_bmhaxictrl bmhaxictrl;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Bmhaxi. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( axi_ctrl == NULL ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Bmhaxi. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	bmhaxictrl.word = 0;

	bmhaxictrl.bit.AWPROT  = axi_ctrl->write_protection_type;
	bmhaxictrl.bit.AWCACHE = axi_ctrl->write_cache_type;
	bmhaxictrl.bit.ARPROT  = axi_ctrl->read_protection_type;
	bmhaxictrl.bit.ARCACHE = axi_ctrl->read_cache_type;

	IO_BMH[channel_no].BMHAXICTRL.word = bmhaxictrl.word;

	return D_DDIM_OK;
}

/*
Get AXI bus I/F Status
*/
INT32 Im_BMH_Get_Bmhaxierr( BYTE channel_no, T_IM_BMH_AXI_STATUS* const sts )
{
	union io_bmh_bmhaxierr bmhaxierr;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Bmhaxierr. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( sts == NULL ){
		// sts setting error
		Ddim_Assertion(("I:Im_BMH_Get_Bmhaxierr. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	bmhaxierr.word = IO_BMH[channel_no].BMHAXIERR.word;

	sts->write_channel_response = bmhaxierr.bit.WRESP;			/* pgr0872 */
	sts->read_channel_response  = bmhaxierr.bit.RRESP;			/* pgr0872 */

	return D_DDIM_OK;
}

/*
BMH BMHHCNT set
*/
INT32 Im_BMH_Set_Bmhhcnt( BYTE channel_no, const T_IM_BMH_BMHHCNT* const cnt )
{
	union io_bmh_bmhhcnt bmhhcnt;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Bmhhcnt. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( cnt == NULL ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Bmhhcnt. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if( im_bmh_is_act( channel_no ) ){
		return D_IM_BMH_BUSY_NG;
	}

	bmhhcnt.word = 0;

	bmhhcnt.bit.HCNT = cnt->hcnt;
	bmhhcnt.bit.RCNT = cnt->rcnt;

	IO_BMH[channel_no].BMHHCNT.word = bmhhcnt.word;

	return D_DDIM_OK;
}

/*
Get BMH BMHHCNT
*/
INT32 Im_BMH_Get_Bmhhcnt( BYTE channel_no, T_IM_BMH_BMHHCNT* const cnt )
{
	union io_bmh_bmhhcnt bmhhcnt;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Bmhhcnt. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( cnt == NULL ){
		// cnt setting error
		Ddim_Assertion(("I:Im_BMH_Get_Bmhhcnt. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	bmhhcnt.word = IO_BMH[channel_no].BMHHCNT.word;

	cnt->hcnt = bmhhcnt.bit.HCNT;			/* pgr0872 */
	cnt->rcnt = bmhhcnt.bit.RCNT;			/* pgr0872 */

	return D_DDIM_OK;
}

/*
BMH BMHSAD_WR set
*/
INT32 Im_BMH_Set_Bmhsad_wr( BYTE channel_no, const T_IM_BMH_BMHSAD_WR* const sad_wr )
{
	union io_bmh_bmhsad_wr bmhsad_wr;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Bmhsad_wr. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( sad_wr == NULL ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Bmhsad_wr. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if( im_bmh_is_act( channel_no ) ){
		return D_IM_BMH_BUSY_NG;
	}

	bmhsad_wr.word = 0;

	bmhsad_wr.bit.SAD_WR = sad_wr->sad_wr;

	IO_BMH[channel_no].BMHSAD_WR.word = bmhsad_wr.word;

	return D_DDIM_OK;
}

/*
Get BMH BMHSAD_WR
*/
INT32 Im_BMH_Get_Bmhsad_wr( BYTE channel_no, T_IM_BMH_BMHSAD_WR* const sad_wr )
{
	union io_bmh_bmhsad_wr bmhsad_wr;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Bmhsad_wr. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( sad_wr == NULL ){
		// sad_wr setting error
		Ddim_Assertion(("I:Im_BMH_Get_Bmhsad_wr. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	bmhsad_wr.word = IO_BMH[channel_no].BMHSAD_WR.word;

	sad_wr->sad_wr = bmhsad_wr.bit.SAD_WR;			/* pgr0872 */

	return D_DDIM_OK;
}

/*
BMH Dma mode control
*/
INT32 Im_BMH_Ctrl_Dma( BYTE channel_no, const T_IM_BMH_CTRL_DMA* const bmh_ctrl_dma )
{
	union io_bmh_bmhdma    bmhdma;
	union io_bmh_bmhghsize bmhghsize;
	union io_bmh_bmhhsize  bmhhsize;
	union io_bmh_bmhvsize  bmhvsize;
	union io_bmh_bmhcurad  bmhcurad;
	union io_bmh_bmhrefad  bmhrefad;
	union io_bmh_bmhsadad  bmhsadad;
	union io_bmh_bmhsadgh  bmhsadgh;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Ctrl_Dma. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( bmh_ctrl_dma == NULL ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Ctrl_Dma. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if( im_bmh_is_act( channel_no ) ){
		return D_IM_BMH_BUSY_NG;
	}

	bmhdma.word    = 0;
	bmhghsize.word = 0;
	bmhhsize.word  = 0;
	bmhvsize.word  = 0;
	bmhcurad.word  = 0;
	bmhrefad.word  = 0;
	bmhsadad.word  = 0;
	bmhsadgh.word  = 0;

	bmhdma.bit.DMAMD         = bmh_ctrl_dma->dmamd;
	bmhghsize.bit.GHSIZE_CUR = bmh_ctrl_dma->ghsize_cur;
	bmhghsize.bit.GHSIZE_REF = bmh_ctrl_dma->ghsize_ref;
	bmhhsize.bit.HSIZE_REF   = bmh_ctrl_dma->hsize_ref;
	bmhvsize.bit.VSIZE_REF   = bmh_ctrl_dma->vsize_ref;
	bmhcurad.bit.CURSTAD     = bmh_ctrl_dma->curstad;
	bmhrefad.bit.REFSTAD     = bmh_ctrl_dma->refstad;
	bmhsadad.bit.SADSTAD     = bmh_ctrl_dma->sadstad;
	bmhsadgh.bit.SADGHSIZE   = bmh_ctrl_dma->sadghsize;

	IO_BMH[channel_no].BMHDMA.word    = bmhdma.word;
	IO_BMH[channel_no].BMHGHSIZE.word = bmhghsize.word;
	IO_BMH[channel_no].BMHHSIZE.word  = bmhhsize.word;
	IO_BMH[channel_no].BMHVSIZE.word  = bmhvsize.word;
	IO_BMH[channel_no].BMHCURAD.word  = bmhcurad.word;
	IO_BMH[channel_no].BMHREFAD.word  = bmhrefad.word;
	IO_BMH[channel_no].BMHSADAD.word  = bmhsadad.word;
	IO_BMH[channel_no].BMHSADGH.word  = bmhsadgh.word;

	return D_DDIM_OK;
}

/*
Get BMH Dma mode control
*/
INT32 Im_BMH_Get_Ctrl_Dma( BYTE channel_no, T_IM_BMH_CTRL_DMA* const bmh_ctrl_dma )
{
	union io_bmh_bmhdma    bmhdma;
	union io_bmh_bmhghsize bmhghsize;
	union io_bmh_bmhhsize  bmhhsize;
	union io_bmh_bmhvsize  bmhvsize;
	union io_bmh_bmhcurad  bmhcurad;
	union io_bmh_bmhrefad  bmhrefad;
	union io_bmh_bmhsadad  bmhsadad;
	union io_bmh_bmhsadgh  bmhsadgh;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Ctrl_Dma. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( bmh_ctrl_dma == NULL ){
		// bmh_ctrl_dma setting error
		Ddim_Assertion(("I:Im_BMH_Get_Ctrl_Dma. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	bmhdma.word    = IO_BMH[channel_no].BMHDMA.word;
	bmhghsize.word = IO_BMH[channel_no].BMHGHSIZE.word;
	bmhhsize.word  = IO_BMH[channel_no].BMHHSIZE.word;
	bmhvsize.word  = IO_BMH[channel_no].BMHVSIZE.word;
	bmhcurad.word  = IO_BMH[channel_no].BMHCURAD.word;
	bmhrefad.word  = IO_BMH[channel_no].BMHREFAD.word;
	bmhsadad.word  = IO_BMH[channel_no].BMHSADAD.word;
	bmhsadgh.word  = IO_BMH[channel_no].BMHSADGH.word;

	bmh_ctrl_dma->dmamd      = bmhdma.bit.DMAMD;				/* pgr0872 */
	bmh_ctrl_dma->ghsize_cur = bmhghsize.bit.GHSIZE_CUR;		/* pgr0872 */
	bmh_ctrl_dma->ghsize_ref = bmhghsize.bit.GHSIZE_REF;		/* pgr0872 */
	bmh_ctrl_dma->hsize_ref  = bmhhsize.bit.HSIZE_REF;			/* pgr0872 */
	bmh_ctrl_dma->vsize_ref  = bmhvsize.bit.VSIZE_REF;			/* pgr0872 */
	bmh_ctrl_dma->curstad    = bmhcurad.bit.CURSTAD;			/* pgr0872 */
	bmh_ctrl_dma->refstad    = bmhrefad.bit.REFSTAD;			/* pgr0872 */
	bmh_ctrl_dma->sadstad    = bmhsadad.bit.SADSTAD;			/* pgr0872 */
	bmh_ctrl_dma->sadghsize  = bmhsadgh.bit.SADGHSIZE;			/* pgr0872 */

	return D_DDIM_OK;
}

/*
BMH LUT set
*/
INT32 Im_BMH_Set_Lut( BYTE channel_no, const T_IM_BMH_LUT* const lut_data )
{
	ULONG count;
	typedef union {
		unsigned long word;
		struct {
		unsigned long	LUT_0	:12;
		unsigned long			:4;
		unsigned long	LUT_1	:12;
		unsigned long			:4;
		} bit;
	} u_bmh_lut;

	u_bmh_lut lut;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Lut. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( lut_data == NULL ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Lut. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if( im_bmh_is_act( channel_no ) ){
		return D_IM_BMH_BUSY_NG;
	}

	lut.word = 0;

	for( count = 0; count < 1024; count++ ){
		lut.bit.LUT_0 = lut_data->lut[count * 2];
		lut.bit.LUT_1 = lut_data->lut[count * 2 + 1];
		IO_BMH[channel_no].LUT.word[count] = lut.word;
	}

	return D_DDIM_OK;
}


/**
BMH Macro interrupt permission set
 */
INT32 Im_BMH_Set_Interrupt( BYTE channel_no, USHORT interrupt_bit, UCHAR permission_flg )
{
	union io_bmh_bmhinte bmhinte;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Interrupt. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( permission_flg > E_IM_BMH_ONOFF_ENABLE ){
		Ddim_Assertion(("I:Im_BMH_Set_Interrupt error. 'permission_flg' value over!! (%d)\n", permission_flg));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	bmhinte.word = 0;

	// SG VD Interrupt
	if(( interrupt_bit & D_IM_BMH_FLG_BMH_AXI_ERR ) != 0 ){
		bmhinte.bit.AXIE = permission_flg;
	}

	// SG HD Interrupt
	if(( interrupt_bit & D_IM_BMH_FLG_BMH_END) != 0 ){
		bmhinte.bit.BMHE = permission_flg;
	}

	IO_BMH[channel_no].BMHINTE.word = bmhinte.word;

	return D_DDIM_OK;
}

/*
BMH Macro handler
*/
VOID Im_BMH_Int_Handler( BYTE channel_no )
{
	DDIM_USER_FLGPTN flg;
	union io_bmh_bmhintfs bmhintfs;
	union io_bmh_bmhinte bmhinte;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Int_Handler. channel_no is abnormal.\n"));
		return;
	}
#endif	// CO_PARAM_CHECK

	bmhinte.word = IO_BMH[channel_no].BMHINTE.word;
	bmhintfs.word = IO_BMH[channel_no].BMHINTFS.word;

	if(( bmhinte.bit.AXIE != 0 ) && ( bmhintfs.bit.__AXIF != 0 )){		/* pgr0872 */
		// clear interrupt end register
		im_bmh_clear_axif( channel_no );

		flg = D_IM_BMH_FLG_BMH_AXI_ERR;

		// Set End Flag
		DDIM_User_Set_Flg( FID_IM_BMH, D_IM_BMH_FLG_BMH_AXI_ERR );

		// User Function Check
		if( gIM_BMH_CallBack_Func != NULL ){
			gIM_BMH_CallBack_Func( flg );
		}
	}

	if(( bmhinte.bit.BMHE != 0 ) && ( bmhintfs.bit.__BMHF != 0 )){		/* pgr0872 */
		// clear interrupt end register
		im_bmh_clear_bmhf( channel_no );

		im_BMH_off_bmh_clk( channel_no );

		flg = D_IM_BMH_FLG_BMH_END;

		// Set End Flag
		(VOID)DDIM_User_Set_Flg( FID_IM_BMH, flg );

		// User Function Check
		if( gIM_BMH_CallBack_Func != NULL ){
			gIM_BMH_CallBack_Func( flg );
		}
	}

	Im_BMH_Dsb();
}

/*
BMH CUR set
*/
INT32 Im_BMH_Set_Cur( BYTE channel_no, const T_IM_BMH_CUR_BLOCK* const cur_write_data )
{
	ULONG loop1;
	ULONG loop2;
	ULONG loop_max;
	union io_bmh_bmhmd bmhmd;
	UCHAR write_flg = 0;
	typedef union {
		unsigned long word;
		struct {
			unsigned long	CUR_0	:8;
			unsigned long	CUR_1	:8;
			unsigned long	CUR_2	:8;
			unsigned long	CUR_3	:8;
		} bit;
	} u_bmh_cur;

	u_bmh_cur cur;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Cur. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( cur_write_data == NULL ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Cur. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	bmhmd.word = IO_BMH[channel_no].BMHMD.word;
	loop_max = (( bmhmd.bit.LSELCNT % 8 ) == 0 ) ? 8 : ( bmhmd.bit.LSELCNT % 8 );		/* pgr0872 */

	for( loop1 = 0; loop1 < loop_max; loop1++ ){
		for( loop2 = 0; loop2 < 256; loop2 += 4 ){
			cur.word = 0;
			if( cur_write_data->cur_write_flg[loop1][loop2] == 1 ){
				write_flg = 1;
				cur.bit.CUR_0 = cur_write_data->cur_data[loop1][loop2];
			}
			if( cur_write_data->cur_write_flg[loop1][loop2 + 1] == 1 ){
				write_flg = 1;
				cur.bit.CUR_1 = cur_write_data->cur_data[loop1][loop2 + 1];
			}
			if( cur_write_data->cur_write_flg[loop1][loop2 + 2] == 1 ){
				write_flg = 1;
				cur.bit.CUR_2 = cur_write_data->cur_data[loop1][loop2 + 2];
			}
			if( cur_write_data->cur_write_flg[loop1][loop2 + 3] == 1 ){
				write_flg = 1;
				cur.bit.CUR_3 = cur_write_data->cur_data[loop1][loop2 + 3];
			}

			if( write_flg == 1 ){
				IO_BMH[channel_no].CUR.word[loop1][loop2 / 4] = cur.word;
				write_flg = 0;
			}
		}
	}

	for( loop2 = 0; loop2 < 256; loop2 += 4 ){
		cur.word = 0;
		if( cur_write_data->cur_write_flg[8][loop2] == 1 ){
			write_flg = 1;
			cur.bit.CUR_0 = cur_write_data->cur_data[8][loop2];
		}
		if( cur_write_data->cur_write_flg[8][loop2 + 1] == 1 ){
			write_flg = 1;
			cur.bit.CUR_1 = cur_write_data->cur_data[8][loop2 + 1];
		}
		if( cur_write_data->cur_write_flg[8][loop2 + 2] == 1 ){
			write_flg = 1;
			cur.bit.CUR_2 = cur_write_data->cur_data[8][loop2 + 2];
		}
		if( cur_write_data->cur_write_flg[8][loop2 + 3] == 1 ){
			write_flg = 1;
			cur.bit.CUR_3 = cur_write_data->cur_data[8][loop2 + 3];
		}

		if( write_flg == 1 ){
			IO_BMH[channel_no].CUR.word[8][loop2 / 4] = cur.word;
			write_flg = 0;
		}
	}

	return D_DDIM_OK;
}

/*
BMH REF set
*/
INT32 Im_BMH_Set_Ref( BYTE channel_no, const T_IM_BMH_REF_BLOCK* const ref_write_data )
{
	ULONG loop1;
	ULONG loop2;
	ULONG loop_max;
	union io_bmh_bmhmd bmhmd;
	UCHAR write_flg = 0;
	typedef union {
		unsigned long word;
		struct {
			unsigned long	REF_0	:8;
			unsigned long	REF_1	:8;
			unsigned long	REF_2	:8;
			unsigned long	REF_3	:8;
		} bit;
	} u_bmh_ref;

	u_bmh_ref ref;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Ref. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( ref_write_data == NULL ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Set_Ref. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	bmhmd.word = IO_BMH[channel_no].BMHMD.word;
	loop_max = (( bmhmd.bit.LSELCNT % 8 ) == 0 ) ? 8 : ( bmhmd.bit.LSELCNT % 8 );		/* pgr0872 */

	for( loop1 = 0; loop1 < loop_max; loop1++ ){
		for( loop2 = 0; loop2 < 256; loop2 += 4 ){
			ref.word = 0;
			if( ref_write_data->ref_write_flg[loop1][loop2] == 1 ){
				write_flg = 1;
				ref.bit.REF_0 = ref_write_data->ref_data[loop1][loop2];
			}
			if( ref_write_data->ref_write_flg[loop1][loop2 + 1] == 1 ){
				write_flg = 1;
				ref.bit.REF_1 = ref_write_data->ref_data[loop1][loop2 + 1];
			}
			if( ref_write_data->ref_write_flg[loop1][loop2 + 2] == 1 ){
				write_flg = 1;
				ref.bit.REF_2 = ref_write_data->ref_data[loop1][loop2 + 2];
			}
			if( ref_write_data->ref_write_flg[loop1][loop2 + 3] == 1 ){
				write_flg = 1;
				ref.bit.REF_3 = ref_write_data->ref_data[loop1][loop2 + 3];
			}

			if( write_flg == 1 ){
				IO_BMH[channel_no].REF.word[loop1][loop2 / 4] = ref.word;
				write_flg = 0;
			}
		}
	}

	for( loop2 = 0; loop2 < 256; loop2 += 4 ){
		ref.word = 0;
		if( ref_write_data->ref_write_flg[8][loop2] == 1 ){
			write_flg = 1;
			ref.bit.REF_0 = ref_write_data->ref_data[8][loop2];
		}
		if( ref_write_data->ref_write_flg[8][loop2 + 1] == 1 ){
			write_flg = 1;
			ref.bit.REF_1 = ref_write_data->ref_data[8][loop2 + 1];
		}
		if( ref_write_data->ref_write_flg[8][loop2 + 2] == 1 ){
			write_flg = 1;
			ref.bit.REF_2 = ref_write_data->ref_data[8][loop2 + 2];
		}
		if( ref_write_data->ref_write_flg[8][loop2 + 3] == 1 ){
			write_flg = 1;
			ref.bit.REF_3 = ref_write_data->ref_data[8][loop2 + 3];
		}

		if( write_flg == 1 ){
			IO_BMH[channel_no].REF.word[8][loop2 / 4] = ref.word;
			write_flg = 0;
		}
	}

	return D_DDIM_OK;
}

/*
BMH SADR get (BMHMD.OUTMD=000b)
*/
INT32 Im_BMH_Get_Sadr_000( BYTE channel_no, T_IM_BMH_SADR_000* const sadr_read_data )
{
	ULONG loop1;
	ULONG loop2;
	ULONG count = 0;
	union io_bmh_sadr sadr;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Sadr_000. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( sadr_read_data == NULL ){
		// sts setting error
		Ddim_Assertion(("I:Im_BMH_Get_Sadr_000. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_bmh_memcpy_ulong( (ULONG*)&sadr.word, (ULONG*)&IO_BMH[channel_no].SADR.word, sizeof(sadr) );

	for( loop1 = 0; loop1 < 2; loop1++ ){
		count = 0;
		for( loop2 = 0; loop2 < 57; loop2++ ){
			sadr_read_data->sadcmp[loop1][count] = sadr.bit000[loop1][loop2].SADcmp_0;
			sadr_read_data->pos[loop1][count]    = sadr.bit000[loop1][loop2].POS_0;
			count++;
			sadr_read_data->sadcmp[loop1][count] = sadr.bit000[loop1][loop2].SADcmp_1;
			sadr_read_data->pos[loop1][count]    = sadr.bit000[loop1][loop2].POS_1;
			count++;
			sadr_read_data->sadcmp[loop1][count] = sadr.bit000[loop1][loop2].SADcmp_2;
			sadr_read_data->pos[loop1][count]    = sadr.bit000[loop1][loop2].POS_2;
			count++;
			sadr_read_data->sadcmp[loop1][count] = sadr.bit000[loop1][loop2].SADcmp_3;
			sadr_read_data->pos[loop1][count]    = sadr.bit000[loop1][loop2].POS_3;
			count++;
		}
	}

	return D_DDIM_OK;
}

/*
BMH SADR get (BMHMD.OUTMD=001b)
*/
INT32 Im_BMH_Get_Sadr_001( BYTE channel_no, T_IM_BMH_SADR_001* const sadr_read_data )
{
	ULONG loop;
	union io_bmh_sadr sadr;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Sadr_001. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( sadr_read_data == NULL ){
		// sts setting error
		Ddim_Assertion(("I:Im_BMH_Get_Sadr_001. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_bmh_memcpy_ulong( (ULONG*)&sadr.word, (ULONG*)&IO_BMH[channel_no].SADR.word, sizeof(sadr) );

	for( loop = 0; loop < 225; loop++ ){
			sadr_read_data->sadcmp[loop] = sadr.bit001[loop].SADcmp;
			sadr_read_data->pos[loop]    = sadr.bit001[loop].POS;
			sadr_read_data->sadmin[loop] = sadr.bit001[loop].SADmin;
	}

	return D_DDIM_OK;
}

/*
BMH SADR get (BMHMD.OUTMD=010b)
*/
INT32 Im_BMH_Get_Sadr_010( BYTE channel_no, T_IM_BMH_SADR_010* const sadr_read_data )
{
	ULONG loop1;
	ULONG loop2;
	ULONG count = 0;
	union io_bmh_sadr sadr;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Sadr_010. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( sadr_read_data == NULL ){
		// sts setting error
		Ddim_Assertion(("I:Im_BMH_Get_Sadr_010. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_bmh_memcpy_ulong( (ULONG*)&sadr.word, (ULONG*)&IO_BMH[channel_no].SADR.word, sizeof(sadr) );

	for( loop1 = 0; loop1 < 2; loop1++ ){
		count = 0;
		for( loop2 = 0; loop2 < 113; loop2++ ){
			sadr_read_data->sadcmp[loop1][count]       = sadr.bit010[loop1][loop2].SADcmp_0;
			sadr_read_data->pos_integer[loop1][count]  = sadr.bit010[loop1][loop2].POS_Integer_0;
			sadr_read_data->pos_fraction[loop1][count] = sadr.bit010[loop1][loop2].POS_fraction_0;
			count++;
			sadr_read_data->sadcmp[loop1][count]       = sadr.bit010[loop1][loop2].SADcmp_1;
			sadr_read_data->pos_integer[loop1][count]  = sadr.bit010[loop1][loop2].POS_Integer_1;
			sadr_read_data->pos_fraction[loop1][count] = sadr.bit010[loop1][loop2].POS_fraction_1;
			count++;
		}
	}

	return D_DDIM_OK;
}

/*
BMH SADR get (BMHMD.OUTMD=011b)
*/
INT32 Im_BMH_Get_Sadr_011( BYTE channel_no, T_IM_BMH_SADR_011* const sadr_read_data )
{
	ULONG loop;
	union io_bmh_sadr sadr;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Sadr_011. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( sadr_read_data == NULL ){
		// sts setting error
		Ddim_Assertion(("I:Im_BMH_Get_Sadr_011. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_bmh_memcpy_ulong( (ULONG*)&sadr.word, (ULONG*)&IO_BMH[channel_no].SADR.word, sizeof(sadr) );

	for( loop = 0; loop < 225; loop++ ){
			sadr_read_data->sadcmp[loop]       = sadr.bit011[loop].SADcmp;
			sadr_read_data->pos_integer[loop]  = sadr.bit011[loop].POS_Integer;
			sadr_read_data->pos_fraction[loop] = sadr.bit011[loop].POS_fraction;
			sadr_read_data->sadmin[loop]       = sadr.bit011[loop].SADmin;
	}

	return D_DDIM_OK;
}

/*
BMH SADR get (BMHMD.OUTMD=100b)
*/
INT32 Im_BMH_Get_Sadr_100( BYTE channel_no, T_IM_BMH_SADR_100* const sadr_read_data )
{
	ULONG loop1;
	ULONG loop2;
	union io_bmh_sadr sadr;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Sadr_100. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( sadr_read_data == NULL ){
		// sts setting error
		Ddim_Assertion(("I:Im_BMH_Get_Sadr_100. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_bmh_memcpy_ulong( (ULONG*)&sadr.word, (ULONG*)&IO_BMH[channel_no].SADR.word, sizeof(sadr) );

	for( loop1 = 0; loop1 < 2; loop1++ ){
		for( loop2 = 0; loop2 < 112; loop2++ ){
			sadr_read_data->sadcmp[loop1][loop2]       = sadr.bit100[loop1][loop2].SADcmp;
			sadr_read_data->pos_integer[loop1][loop2]  = sadr.bit100[loop1][loop2].POS_Integer;
			sadr_read_data->pos_fraction[loop1][loop2] = sadr.bit100[loop1][loop2].POS_fraction;
			sadr_read_data->sadmin[loop1][loop2]       = sadr.bit100[loop1][loop2].SADmin;
		}
	}

	return D_DDIM_OK;
}

/*
BMH SADR get (BMHMD.OUTMD=101b)
*/
INT32 Im_BMH_Get_Sadr_101( BYTE channel_no, T_IM_BMH_SADR_101* const sadr_read_data )
{
	ULONG loop1;
	ULONG loop2;
	union io_bmh_sadr sadr;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Sadr_101. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( sadr_read_data == NULL ){
		// sts setting error
		Ddim_Assertion(("I:Im_BMH_Get_Sadr_101. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_bmh_memcpy_ulong( (ULONG*)&sadr.word, (ULONG*)&IO_BMH[channel_no].SADR.word, sizeof(sadr) );

	for( loop1 = 0; loop1 < 2; loop1++ ){
		for( loop2 = 0; loop2 < 56; loop2++ ){
			sadr_read_data->sadcmp[loop1][loop2]       = sadr.bit101[loop1][loop2].SADcmp;
			sadr_read_data->pos_integer[loop1][loop2]  = sadr.bit101[loop1][loop2].POS_Integer;
			sadr_read_data->pos_fraction[loop1][loop2] = sadr.bit101[loop1][loop2].POS_fraction;
			sadr_read_data->sadmin[loop1][loop2]       = sadr.bit101[loop1][loop2].SADmin;

			sadr_read_data->weight[loop1][loop2]       = sadr.bit101[loop1][loop2].Weight;
			sadr_read_data->deviation[loop1][loop2]    = sadr.bit101[loop1][loop2].Deviation;
			sadr_read_data->cur_max[loop1][loop2]      = sadr.bit101[loop1][loop2].CUR_MAX;
			sadr_read_data->cur_min[loop1][loop2]      = sadr.bit101[loop1][loop2].CUR_MIN;
		}
	}

	return D_DDIM_OK;
}

/*
BMH SADR get (BMHMD.OUTMD=110b)
*/
INT32 Im_BMH_Get_Sadr_110( BYTE channel_no, T_IM_BMH_SADR_110* const sadr_read_data )
{
	ULONG loop1;
	ULONG loop2;
	union io_bmh_sadr sadr;

#ifdef CO_PARAM_CHECK
	if( channel_no >= E_IM_BMH_CHANNEL_END ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_BMH_Get_Sadr_110. channel_no is abnormal.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
	if( sadr_read_data == NULL ){
		// sts setting error
		Ddim_Assertion(("I:Im_BMH_Get_Sadr_110. Parameter is NULL.\n"));
		return D_IM_BMH_INPUT_PARAMETER_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_bmh_memcpy_ulong( (ULONG*)&sadr.word, (ULONG*)&IO_BMH[channel_no].SADR.word, sizeof(sadr) );

	for( loop1 = 0; loop1 < 2; loop1++ ){
		for( loop2 = 0; loop2 < 28; loop2++ ){
			sadr_read_data->sadcmp[loop1][loop2]        = sadr.bit110[loop1][loop2].SADcmp;
			sadr_read_data->pos[loop1][loop2]           = sadr.bit110[loop1][loop2].POS;
			sadr_read_data->sadmin[loop1][loop2]        = sadr.bit110[loop1][loop2].SADmin;

			sadr_read_data->sadmin1before[loop1][loop2] = sadr.bit110[loop1][loop2].SADmin1before;
			sadr_read_data->sadmin1after[loop1][loop2]  = sadr.bit110[loop1][loop2].SADmin1after;
			sadr_read_data->sadmin2before[loop1][loop2] = sadr.bit110[loop1][loop2].SADmin2before;
			sadr_read_data->sadmin2after[loop1][loop2]  = sadr.bit110[loop1][loop2].SADmin2after;

			sadr_read_data->weight[loop1][loop2]        = sadr.bit110[loop1][loop2].Weight;
			sadr_read_data->deviation[loop1][loop2]     = sadr.bit110[loop1][loop2].Deviation;
			sadr_read_data->cur_max[loop1][loop2]       = sadr.bit110[loop1][loop2].CUR_MAX;
			sadr_read_data->cur_min[loop1][loop2]       = sadr.bit110[loop1][loop2].CUR_MIN;
		}
	}

	return D_DDIM_OK;
}

#ifdef CO_BMH_DEBUG
VOID Im_BMH_Print_ClockStatus( VOID )
{
#ifdef CO_ACT_BMH_ICLK
	Ddim_Print(( "CLKSTOP: AX=%u\n", Dd_Top_Get_CLKSTOP14_BMHAX() ));
#endif	// CO_ACT_BMH_ICLK
#ifdef CO_ACT_BMHCLK
	Ddim_Print(( "CLKSTOP: CK=%u\n", Dd_Top_Get_CLKSTOP14_BMHCK() ));
#endif	// CO_ACT_BMHCLK
#ifdef CO_ACT_BMH_ICLK
	Ddim_Print(( "CLKCNT: AX=%u\n", gIM_BMH_Iclk_Ctrl_Cnt ));
#endif	// CO_ACT_BMH_ICLK
#ifdef CO_ACT_BMHCLK
	Ddim_Print(( "CLKCNT: CK=%u\n", gIM_BMH_Clk_Ctrl_Cnt ));
#endif	// CO_ACT_BMHCLK
}
#endif	// CO_BMH_DEBUG

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

