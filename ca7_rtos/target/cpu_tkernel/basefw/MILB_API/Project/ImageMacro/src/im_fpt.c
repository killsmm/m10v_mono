/**
 * @file		im_fpt.c
 * @brief		JDSFPT Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "im_fpt.h"
#include "jdsfpt.h"
#include "dd_arm.h"
#include "dd_top.h"
#include "ddim_user_custom.h"

extern VOID Im_FPTAsm_Wait_CpuCyc( ULONG count );

/*----------------------------------------------------------------------*/
/* Compile option														*/
/*----------------------------------------------------------------------*/
// #define CO_FPT_DISABLE_CLK_CTRL

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
#ifdef CO_ACT_CLOCK
#define CO_ACT_FPT_CLOCK
#endif	// CO_ACT_PCLOCK
#ifdef CO_ACT_HCLOCK
#define CO_ACT_FPT_HCLOCK
#endif	// CO_ACT_HCLOCK
#ifdef CO_ACT_PCLOCK
#define CO_ACT_FPT_PCLOCK
#endif	// CO_ACT_PCLOCK
#ifdef CO_ACT_ICLOCK
#define CO_ACT_FPT_ICLOCK
#endif	// CO_ACT_ICLOCK

// Version info.
#define D_IM_FPT_VERSION 		"FPT Driver Version V01L00\r\n"

// NULL
#ifndef NULL
#define NULL	((VOID*)0x0)
#endif

#define D_IM_FPT0_CLK_REG_ADDR			(&Dd_Top_Get_CLKSTOP4())
#define D_IM_FPT0_HCLK_REG_ADDR			(&Dd_Top_Get_CLKSTOP4())
#define D_IM_FPT0_PCLK_REG_ADDR			(&Dd_Top_Get_CLKSTOP4())
#define D_IM_FPT0_ICLK_REG_ADDR			(&Dd_Top_Get_CLKSTOP4())

#define D_IM_FPT1_CLK_REG_ADDR			(&Dd_Top_Get_CLKSTOP4())
#define D_IM_FPT1_HCLK_REG_ADDR			(&Dd_Top_Get_CLKSTOP4())
#define D_IM_FPT1_PCLK_REG_ADDR			(&Dd_Top_Get_CLKSTOP4())
#define D_IM_FPT1_ICLK_REG_ADDR			(&Dd_Top_Get_CLKSTOP4())

#define D_IM_FPT0_CLK_REG_BIT			(D_DD_TOP_FPT0CK_BIT)
#define D_IM_FPT0_PCLK_REG_BIT			(D_DD_TOP_FPT0AP_BIT)
#define D_IM_FPT0_HCLK_REG_BIT			(D_DD_TOP_FPT0AH_BIT)
#define D_IM_FPT0_ICLK_REG_BIT			(D_DD_TOP_FPT0AX_BIT)

#define D_IM_FPT1_CLK_REG_BIT			(D_DD_TOP_FPT1CK_BIT)
#define D_IM_FPT1_PCLK_REG_BIT			(D_DD_TOP_FPT1AP_BIT)
#define D_IM_FPT1_HCLK_REG_BIT			(D_DD_TOP_FPT1AH_BIT)
#define D_IM_FPT1_ICLK_REG_BIT			(D_DD_TOP_FPT1AX_BIT)

// Clock Stop/Clear
#define D_IM_FPT_IM_FPT_CLK_ADDR_STOP	(0x18001134)
#define D_IM_FPT_IM_FPT_CLK_ADDR_CLEAR	(0x18001234)

// Clock Bit
#define D_IM_FPT_IM_FPTCLK				(0x00000090)	// including FPTAX
#define D_IM_FPT_PCLK					(0x00000020)
#define D_IM_FPT_HCLK					(0x00000040)

// The value of TRG bit.
#define D_IM_FPT_TRG_ABORT				(0)
#define D_IM_FPT_TRG_START				(1)
#define D_IM_FPT_TRG_STOPPED			(2)
#define D_IM_FPT_TRG_RUNNING			(3)

// Event-flag
#define D_IM_FPT_INTERRUPT_ERR			(D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR)
#define D_IM_FPT_ABORT_STOP				(0x00001000)
#define D_IM_FPT_USED_FLG_ALL			( (D_IM_FPT_INTERRUPT_COMPLETE | D_IM_FPT_INTERRUPT_AXI_ERR | D_IM_FPT_INTERRUPT_PARAM_ERR | D_IM_FPT_ABORT_STOP))

// Mode Config
#define D_IM_FPT_IO_MODE_SDRAM			(0)
#define D_IM_FPT_IO_MODE_REGISTER		(1)
#define D_IM_FPT_PIPE_DISABLE			(0)
#define D_IM_FPT_PIPE_ENABLE			(1)
#define D_IM_FPT_NORMAL_MODE			(0)
#define D_IM_FPT_DEBUG_MODE				(1)

// LUT Config
#define D_IM_FPT_LUT_OFF				(0x00000000)
#define D_IM_FPT_LUT_ON					(0x00000001)
#define D_IM_FPT_LUT_BASE				(0x00000000)
#define D_IM_FPT_LUT_TARGET				(0x00000100)
#define D_IM_FPT_LUT_BASE_X__TARGET_Y	(0x00000200)
#define D_IM_FPT_LUT_LUT0				(0x00000000)
#define D_IM_FPT_LUT_LUT1				(0x00010000)

// Offset & Clip Config
#define D_IM_FPT_OFFSET_OFF				(0x0000)
#define D_IM_FPT_CLIP_MAX_OFF			(0xFF)
#define D_IM_FPT_CLIP_MIN_OFF			(0x00)

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global variables														*/
/*----------------------------------------------------------------------*/
static VOID (*gFpt_Callback[2])(UINT32 int_factor, UCHAR ch ) = { NULL, NULL };
#ifdef CO_ACT_FPT_CLOCK
volatile UCHAR gFpt_IM_FPTCLK_Counter[2] = {0,0};
#endif	// CO_ACT_FPT_CLOCK
#ifdef CO_ACT_FPT_PCLOCK
volatile UCHAR gFpt_PCLK_Counter[2] = {0,0};
#endif	// CO_ACT_FPT_PCLOCK
#ifdef CO_ACT_FPT_HCLOCK
volatile UCHAR gFpt_HCLK_Counter[2] = {0,0};
#endif	// CO_ACT_FPT_HCLOCK
#ifdef CO_ACT_FPT_ICLOCK
volatile UCHAR gFpt_ICLK_Counter[2] = {0,0};
#endif	// CO_ACT_FPT_ICLOCK
volatile UINT32 gFpt_RAMSLP_Counter[2] = {0,0};
volatile UCHAR gFpt_RRESP[2] = {0,0};
volatile UCHAR gFpt_WRESP[2] = {0,0};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
#ifdef CO_FPT_DEBUG
#include <stdio.h>
#define im_fpt_printf(arg)			(printf arg)
#else
#define im_fpt_printf(arg)
#endif

#ifdef CO_ARM_V7_ARCH
#define im_fpt_dummy_read(ch)		{ IO_FPT[ch].FPTAXIERR0.word; }
#else
#define im_fpt_dummy_read(ch)
#endif

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* Nothing Special */

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
typedef struct {
	ULONG lock;
	ULONG dummy[(64 / 4) - 1];
} T_IM_FPT_SPIN_LOCK_VAR;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
static T_IM_FPT_SPIN_LOCK_VAR gImFpt_Spin_Lock_Tbl[2] __attribute__((section(".LOCK_SECTION"), aligned(64)));

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* Nothing Special */

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
   Initalize of Managemnet information
 */
static VOID im_fpt_manager_init( UCHAR ch )
{
	// Initialize of PCLK/HCLK/CLK control information
#ifdef CO_ACT_FPT_CLOCK
	gFpt_IM_FPTCLK_Counter[ch] = 0;
#endif	// CO_ACT_FPT_CLOCK
#ifdef CO_ACT_FPT_PCLOCK
	gFpt_PCLK_Counter[ch] = 0;
#endif	// CO_ACT_FPT_PCLOCK
#ifdef CO_ACT_FPT_HCLOCK
	gFpt_HCLK_Counter[ch] = 0;
#endif	// CO_ACT_FPT_HCLOCK
#ifdef CO_ACT_FPT_ICLOCK
	gFpt_ICLK_Counter[ch] = 0;
#endif	// CO_ACT_FPT_ICLOCK

	return;
}

/**
   Start PCLK.
 */
static VOID im_fpt_on_pclk( UCHAR ch )
{
#ifdef CO_ACT_FPT_PCLOCK
	Dd_ARM_Critical_Section_Start(gImFpt_Spin_Lock_Tbl[ch]);

	if( ch == 0 ){
		Dd_Top_Start_Clock( (UCHAR*)&gFpt_PCLK_Counter[0], D_IM_FPT0_PCLK_REG_ADDR, ~D_IM_FPT0_PCLK_REG_BIT );
	}
	else{
		Dd_Top_Start_Clock( (UCHAR*)&gFpt_PCLK_Counter[1], D_IM_FPT1_PCLK_REG_ADDR, ~D_IM_FPT1_PCLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gImFpt_Spin_Lock_Tbl[ch]);
#endif	// CO_ACT_FPT_PCLOCK
}

/**
   Stop PCLK.
 */
static VOID im_fpt_off_pclk( UCHAR ch )
{
#ifdef CO_ACT_FPT_PCLOCK
	Dd_ARM_Critical_Section_Start(gImFpt_Spin_Lock_Tbl[ch]);

	if( ch == 0 ){
		Dd_Top_Stop_Clock( (UCHAR*)&gFpt_PCLK_Counter[0], D_IM_FPT0_PCLK_REG_ADDR, D_IM_FPT0_PCLK_REG_BIT );
	}
	else{
		Dd_Top_Stop_Clock( (UCHAR*)&gFpt_PCLK_Counter[1], D_IM_FPT1_PCLK_REG_ADDR, D_IM_FPT1_PCLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gImFpt_Spin_Lock_Tbl[ch]);
#endif	// CO_ACT_FPT_PCLOCK
}

/**
   Start HCLK.
 */
static VOID im_fpt_on_hclk( UCHAR ch )
{
#ifdef CO_ACT_FPT_HCLOCK
	Dd_ARM_Critical_Section_Start(gImFpt_Spin_Lock_Tbl[ch]);

	if( ch == 0 ){
		Dd_Top_Start_Clock( (UCHAR*)&gFpt_HCLK_Counter[0], D_IM_FPT0_HCLK_REG_ADDR, ~D_IM_FPT0_HCLK_REG_BIT );
	}
	else{
		Dd_Top_Start_Clock( (UCHAR*)&gFpt_HCLK_Counter[1], D_IM_FPT1_HCLK_REG_ADDR, ~D_IM_FPT1_HCLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gImFpt_Spin_Lock_Tbl[ch]);
#endif	// CO_ACT_FPT_HCLOCK
}

/**
   Stop HCLK.
 */
static VOID im_fpt_off_hclk( UCHAR ch )
{
#ifdef CO_ACT_FPT_HCLOCK
	Dd_ARM_Critical_Section_Start(gImFpt_Spin_Lock_Tbl[ch]);

	if( ch == 0 ){
		Dd_Top_Stop_Clock( (UCHAR*)&gFpt_HCLK_Counter[0], D_IM_FPT0_HCLK_REG_ADDR, D_IM_FPT0_HCLK_REG_BIT );
	}
	else{
		Dd_Top_Stop_Clock( (UCHAR*)&gFpt_HCLK_Counter[1], D_IM_FPT1_HCLK_REG_ADDR, D_IM_FPT1_HCLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gImFpt_Spin_Lock_Tbl[ch]);
#endif	// CO_ACT_FPT_HCLOCK
}

/**
   Start ICLK.
 */
static VOID im_fpt_on_iclk( UCHAR ch )
{
#ifdef CO_ACT_FPT_ICLOCK
	Dd_ARM_Critical_Section_Start(gImFpt_Spin_Lock_Tbl[ch]);

	if( ch == 0 ){
		Dd_Top_Start_Clock( (UCHAR*)&gFpt_ICLK_Counter[0], D_IM_FPT0_ICLK_REG_ADDR, ~D_IM_FPT0_ICLK_REG_BIT );
	}
	else{
		Dd_Top_Start_Clock( (UCHAR*)&gFpt_ICLK_Counter[1], D_IM_FPT1_ICLK_REG_ADDR, ~D_IM_FPT1_ICLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gImFpt_Spin_Lock_Tbl[ch]);
#endif	// CO_ACT_FPT_ICLOCK
}

/**
   Stop ICLK.
 */
static VOID im_fpt_off_iclk( UCHAR ch )
{
#ifdef CO_ACT_FPT_ICLOCK
	Dd_ARM_Critical_Section_Start(gImFpt_Spin_Lock_Tbl[ch]);

	if( ch == 0 ){
		Dd_Top_Stop_Clock( (UCHAR*)&gFpt_ICLK_Counter[0], D_IM_FPT0_ICLK_REG_ADDR, D_IM_FPT0_ICLK_REG_BIT );
	}
	else{
		Dd_Top_Stop_Clock( (UCHAR*)&gFpt_ICLK_Counter[1], D_IM_FPT1_ICLK_REG_ADDR, D_IM_FPT1_ICLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gImFpt_Spin_Lock_Tbl[ch]);
#endif	// CO_ACT_FPT_ICLOCK
}

/**
   Start FPTCLK.
 */
static VOID im_fpt_on_fptclk( UCHAR ch )
{
#ifdef CO_ACT_FPT_CLOCK
	Dd_ARM_Critical_Section_Start(gImFpt_Spin_Lock_Tbl[ch]);

	if( ch == 0 ){
		Dd_Top_Start_Clock( (UCHAR*)&gFpt_IM_FPTCLK_Counter[0], D_IM_FPT0_CLK_REG_ADDR, ~D_IM_FPT0_CLK_REG_BIT );
	}
	else{
		Dd_Top_Start_Clock( (UCHAR*)&gFpt_IM_FPTCLK_Counter[1], D_IM_FPT1_CLK_REG_ADDR, ~D_IM_FPT1_CLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gImFpt_Spin_Lock_Tbl[ch]);
#endif	// CO_ACT_FPT_CLOCK
}

/**
   Stop FPTCLK.
 */
static VOID im_fpt_off_fptclk( UCHAR ch )
{
#ifdef CO_ACT_FPT_CLOCK
	Dd_ARM_Critical_Section_Start(gImFpt_Spin_Lock_Tbl[ch]);

	if( ch == 0 ){
		Dd_Top_Stop_Clock( (UCHAR*)&gFpt_IM_FPTCLK_Counter[0], D_IM_FPT0_CLK_REG_ADDR, D_IM_FPT0_CLK_REG_BIT );
	}
	else{
		Dd_Top_Stop_Clock( (UCHAR*)&gFpt_IM_FPTCLK_Counter[1], D_IM_FPT1_CLK_REG_ADDR, D_IM_FPT1_CLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gImFpt_Spin_Lock_Tbl[ch]);
#endif	// CO_ACT_FPT_CLOCK
}

//---------------------------- driver section ----------------------------
/**
   Start RAM.
 */
static VOID im_fpt_start_ram( UCHAR ch )
{
	union io_fpt_fptctl2 fptctl2;

	Dd_ARM_Critical_Section_Start(gImFpt_Spin_Lock_Tbl[ch]);

	gFpt_RAMSLP_Counter[ch]++;
	if( gFpt_RAMSLP_Counter[ch] == 1 ){
		im_fpt_on_pclk( ch );
		fptctl2.word = IO_FPT[ch].FPTCTL2.word;
		fptctl2.bit.RAMSLP = 1;					// active
		IO_FPT[ch].FPTCTL2.word = fptctl2.word;
		im_fpt_dummy_read( ch );

		Im_FPTAsm_Wait_CpuCyc( (660 >> 1) );	// wait 1usec
	}

	Dd_ARM_Critical_Section_End(gImFpt_Spin_Lock_Tbl[ch]);

	im_fpt_off_pclk( ch );
}

/**
   Stop RAM.
 */
static VOID im_fpt_stop_ram( UCHAR ch )
{
	union io_fpt_fptctl2 fptctl2;

	Dd_ARM_Critical_Section_Start(gImFpt_Spin_Lock_Tbl[ch]);

	if( gFpt_RAMSLP_Counter[ch] > 0 ){
		gFpt_RAMSLP_Counter[ch]--;
	}
	if( gFpt_RAMSLP_Counter[ch] == 0 ){
		im_fpt_on_pclk( ch );
		fptctl2.word = IO_FPT[ch].FPTCTL2.word;
		fptctl2.bit.RAMSLP = 0;					// sleep
		IO_FPT[ch].FPTCTL2.word = fptctl2.word;
		im_fpt_dummy_read( ch );
	}

	Dd_ARM_Critical_Section_End(gImFpt_Spin_Lock_Tbl[ch]);

	im_fpt_off_pclk( ch );
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/**
   Initialize driver.
*/
INT32 Im_FPT_Init( UCHAR ch )
{
	union io_fpt_fptaxictl0 fptaxictl0;
	INT32 retval;

	// Initalize of Managemnet information
	im_fpt_manager_init( ch );

	im_fpt_on_pclk(ch);

	if( IO_FPT[ch].FPTCTL1.word == D_IM_FPT_TRG_STOPPED ){
		IO_FPT[ch].FPTCTL0.word = 0x1;			// Software-reset.

		Dd_ARM_Dsb_Pou();

		im_fpt_dummy_read( ch );

		Im_FPTAsm_Wait_CpuCyc( 660 );			// wait 2usec

		IO_FPT[ch].FPTCTL0.word = 0x0;			// Relase reset.

		Dd_ARM_Dsb_Pou();

		fptaxictl0.word = IO_FPT[ch].FPTAXICTL0.word;
		fptaxictl0.bit.ARCACHE = 0x0;
		fptaxictl0.bit.AWCACHE = 0x0;
		IO_FPT[ch].FPTAXICTL0.word = fptaxictl0.word;

		retval = D_IM_FPT_RETVAL_OK;
	}
	else{
		retval = D_IM_FPT_RETVAL_ERR;
	}

	im_fpt_off_pclk( ch );

	return retval;
}

/**
   Opend driver.
 */
INT32 Im_FPT_Open( UCHAR ch, INT32 wait_time )
{
	DDIM_USER_ER ercd;

	if( wait_time < -1 ){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}
	else if( 0 == wait_time ){
		ercd = DDIM_User_Pol_Sem( SID_IM_FPT(ch) );
	}
	else{
		ercd = DDIM_User_Twai_Sem( SID_IM_FPT(ch), wait_time );
	}

	if( D_DDIM_USER_E_OK != ercd ){
		return D_IM_FPT_RETVAL_SYSTEM_CALL_ERR;
	}

	// Start FPTCLK and Release RAMSLEEP.
	im_fpt_on_fptclk( ch );
	im_fpt_start_ram( ch );

	return D_IM_FPT_RETVAL_OK;
}

/**
   Close driver.
 */
INT32 Im_FPT_Close( UCHAR ch )
{
	DDIM_USER_ER ercd;

	im_fpt_on_pclk(ch);

	if( IO_FPT[ch].FPTCTL1.word == D_IM_FPT_TRG_RUNNING ){
		im_fpt_off_pclk( ch );
		return D_IM_FPT_RETVAL_ERR;
	}

	im_fpt_off_pclk( ch );

	// Stop FPTCLK and RAMSLEEP.
	im_fpt_stop_ram( ch );
	im_fpt_off_fptclk( ch );

	ercd = DDIM_User_Sig_Sem( SID_IM_FPT(ch) );

	if( D_DDIM_USER_E_OK == ercd ){
		return D_IM_FPT_RETVAL_OK;
	}
	else{
		return D_IM_FPT_RETVAL_SYSTEM_CALL_ERR;
	}
}

/**
   Configure the mode, "Calculate Feature Amount".
 */
INT32 Im_FPT_Ctrl_Mode0( UCHAR ch, const T_IM_FPT_MODE0* const param )
{
	union io_fpt_fptctl2	fptctl2;
	union io_fpt_fptfqctl1	fptfqctl1;
	union io_fpt_fptfqctl2	fptfqctl2;
	union io_fpt_fptfqctl9	fptfqctl9;

	if( param == NULL ){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	if(( param->down_sample_x < D_IM_FPT_DS_MIN ) || ( D_IM_FPT_DS_MAX < param->down_sample_x )){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}
	if(( param->down_sample_y < D_IM_FPT_DS_MIN ) || ( D_IM_FPT_DS_MAX < param->down_sample_y )){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	im_fpt_on_pclk( ch );

	switch( param->lutsel ){
		case E_IM_FPT_LUTSEL_OFF:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_OFF );
			break;
		case E_IM_FPT_LUTSEL_0:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_ON | D_IM_FPT_LUT_LUT0 );
			break;
		case E_IM_FPT_LUTSEL_1:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_ON | D_IM_FPT_LUT_LUT1 );
			break;
		default:
			return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	fptctl2.word = IO_FPT[ch].FPTCTL2.word;
	fptctl2.bit.MODE = 0;
	IO_FPT[ch].FPTCTL2.word = fptctl2.word;

	IO_FPT[ch].FPTFQCTL0.word = param->global_width;

	fptfqctl1.word = 0;
	fptfqctl1.bit.HSIZE = param->width;
	fptfqctl1.bit.VSIZE = param->lines;
	IO_FPT[ch].FPTFQCTL1.word = fptfqctl1.word;

	fptfqctl2.word = IO_FPT[ch].FPTFQCTL2.word;
	fptfqctl2.bit.SUMW = param->window_size;
	IO_FPT[ch].FPTFQCTL2.word = fptfqctl2.word;

	IO_FPT[ch].FPTFQCTL3.word = param->img_addr;

	IO_FPT[ch].FPTFQCTL4.word = param->feature_amount_addr;

	IO_FPT[ch].FPTFQCTL5.word = param->histgram_addr;

	IO_FPT[ch].FPTFQCTL8.word = param->feature_amount_line_byte;

	if( IO_FPT[ch].FPTCTL1.word == D_IM_FPT_TRG_STOPPED ){
		fptfqctl9.word = 0;
		fptfqctl9.bit.SKIPX_FQ = param->down_sample_x;
		fptfqctl9.bit.SKIPY_FQ = param->down_sample_y;
		IO_FPT[ch].FPTFQCTL9.word = fptfqctl9.word;
	}

	im_fpt_off_pclk( ch );

	return D_IM_FPT_RETVAL_OK;
}

/**
   Configure the mode, "Re-calculate Feature Amount".
 */
INT32 Im_FPT_Ctrl_Mode1( UCHAR ch, const T_IM_FPT_MODE1* const param )
{
	union io_fpt_fptctl2	fptctl2;
	union io_fpt_fptfqctl1	fptfqctl1;
	union io_fpt_fptfqctl2	fptfqctl2;
	union io_fpt_fptfqctl9	fptfqctl9;

	if( param == NULL ){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	if(( param->down_sample_x < D_IM_FPT_DS_MIN ) || ( D_IM_FPT_DS_MAX < param->down_sample_x )){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}
	if(( param->down_sample_y < D_IM_FPT_DS_MIN ) || ( D_IM_FPT_DS_MAX < param->down_sample_y )){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	im_fpt_on_pclk( ch );

	switch( param->lutsel ){
		case E_IM_FPT_LUTSEL_OFF:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_OFF );
			break;
		case E_IM_FPT_LUTSEL_0:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_ON | D_IM_FPT_LUT_LUT0 );
			break;
		case E_IM_FPT_LUTSEL_1:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_ON | D_IM_FPT_LUT_LUT1 );
			break;
		default:
			return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	fptctl2.word = IO_FPT[ch].FPTCTL2.word;
	fptctl2.bit.MODE = 1;
	IO_FPT[ch].FPTCTL2.word = fptctl2.word;

	IO_FPT[ch].FPTFQCTL0.word = param->global_width;

	fptfqctl1.word = 0;
	fptfqctl1.bit.HSIZE = param->width;
	fptfqctl1.bit.VSIZE = param->lines;
	IO_FPT[ch].FPTFQCTL1.word = fptfqctl1.word;

	fptfqctl2.word = 0;
	fptfqctl2.bit.SUMW  = param->window_size;
	fptfqctl2.bit.FPMAX = param->num_feature_point;
	fptfqctl2.bit.OUTEN = param->output;
	IO_FPT[ch].FPTFQCTL2.word = fptfqctl2.word;

	IO_FPT[ch].FPTFQCTL3.word = param->img_addr;

	if( fptctl2.bit.IO_MODE != 1 ){			/* pgr0872 */
		IO_FPT[ch].FPTFQCTL6.word = param->input_coord_addr;
		IO_FPT[ch].FPTFQCTL7.word = param->output_addr;
	}

	if( IO_FPT[ch].FPTCTL1.word == D_IM_FPT_TRG_STOPPED ){
		fptfqctl9.word = 0;
		fptfqctl9.bit.SKIPX_FQ = param->down_sample_x;
		fptfqctl9.bit.SKIPY_FQ = param->down_sample_y;
		IO_FPT[ch].FPTFQCTL9.word = fptfqctl9.word;
	}

	im_fpt_off_pclk( ch );

	return D_IM_FPT_RETVAL_OK;
}

/**
   Configure the mode, "Feature Point Tracking".
 */
INT32 Im_FPT_Ctrl_Mode2( UCHAR ch, const T_IM_FPT_MODE2* const param )
{
	union io_fpt_fptctl2	fptctl2;
	union io_fpt_fptftctl0	fptftctl0;
	union io_fpt_fptftctl1	fptftctl1;
	union io_fpt_fptftctl2	fptftctl2;
	union io_fpt_fptftctl3	fptftctl3;
	union io_fpt_fptftctl4	fptftctl4;
	union io_fpt_fptftctl10	fptftctl10;

	if( param == NULL ){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	im_fpt_on_pclk( ch );

	switch( param->lutcfg ){
		case E_IM_FPT_LUTCFG_OFF:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_OFF );
			break;
		case E_IM_FPT_LUTCFG_BASE_0:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_ON | D_IM_FPT_LUT_LUT0 | D_IM_FPT_LUT_BASE );
			break;
		case E_IM_FPT_LUTCFG_TARGET_1:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_ON | D_IM_FPT_LUT_LUT1 | D_IM_FPT_LUT_TARGET );
			break;
		case E_IM_FPT_LUTCFG_BASE_0__TARGET_1:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_ON | D_IM_FPT_LUT_BASE_X__TARGET_Y );
			break;
		case E_IM_FPT_LUTCFG_BASE_1__TARGET_0:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_ON | D_IM_FPT_LUT_LUT1 | D_IM_FPT_LUT_BASE_X__TARGET_Y );
			break;
		default:
			return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	fptctl2.word = IO_FPT[ch].FPTCTL2.word;
	fptctl2.bit.MODE = 2;
	IO_FPT[ch].FPTCTL2.word = fptctl2.word;

	fptftctl0.word = 0;
	fptftctl0.bit.GHBSIZE = param->base_global_width;
	fptftctl0.bit.GHTSIZE = param->target_global_width;
	IO_FPT[ch].FPTFTCTL0.word = fptftctl0.word;

	fptftctl1.word = 0;
	fptftctl1.bit.FTMAX  = param->num_feature_point;
	fptftctl1.bit.INIT_0 = param->usage_target_coord;
	IO_FPT[ch].FPTFTCTL1.word = fptftctl1.word;

	fptftctl2.word = 0;
	fptftctl2.bit.LEFTLIMIT  = param->search_window.left;
	fptftctl2.bit.RIGHTLIMIT = param->search_window.right;
	IO_FPT[ch].FPTFTCTL2.word = fptftctl2.word;

	fptftctl3.word = 0;
	fptftctl3.bit.TOPLIMIT    = param->search_window.top;
	fptftctl3.bit.BOTTOMLIMIT = param->search_window.bottom;
	IO_FPT[ch].FPTFTCTL3.word = fptftctl3.word;

	fptftctl4.word = 0;
	fptftctl4.bit.MAXERR = param->tracking_threshold;
	fptftctl4.bit.MAXITR = param->num_iteration;
	IO_FPT[ch].FPTFTCTL4.word = fptftctl4.word;

	IO_FPT[ch].FPTFTCTL5.word = param->base_img_addr;

	IO_FPT[ch].FPTFTCTL6.word = param->target_img_addr;

	if( fptctl2.bit.IO_MODE != 1 ){			/* pgr0872 */
		IO_FPT[ch].FPTFTCTL7.word = param->base_coord_addr;
		IO_FPT[ch].FPTFTCTL8.word = param->target_coord_addr;
		IO_FPT[ch].FPTFTCTL9.word = param->tracking_result_addr;
	}

	if( IO_FPT[ch].FPTCTL1.word == D_IM_FPT_TRG_STOPPED ){
		fptftctl10.word = 0;
		fptftctl10.bit.SKIPX_FT = param->down_sample_x;
		fptftctl10.bit.SKIPY_FT = param->down_sample_y;
		IO_FPT[ch].FPTFTCTL10.word = fptftctl10.word;
	}

	im_fpt_off_pclk( ch );

	return D_IM_FPT_RETVAL_OK;
}

/**
   Configure the mode, "Re-calculate Feature Amount and Feature Point Tracking".
 */
INT32 Im_FPT_Ctrl_Mode3( UCHAR ch, const T_IM_FPT_MODE3* const param )
{
	union io_fpt_fptctl2	fptctl2;
	union io_fpt_fptfqctl1	fptfqctl1;
	union io_fpt_fptfqctl2	fptfqctl2;
	union io_fpt_fptfqctl9	fptfqctl9;
	union io_fpt_fptftctl0	fptftctl0;
	union io_fpt_fptftctl1	fptftctl1;
	union io_fpt_fptftctl2	fptftctl2;
	union io_fpt_fptftctl3	fptftctl3;
	union io_fpt_fptftctl4	fptftctl4;
	union io_fpt_fptftctl10	fptftctl10;

	if( param == NULL ){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	if(( param->down_sample_x < D_IM_FPT_DS_MIN ) || ( D_IM_FPT_DS_MAX < param->down_sample_x )){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}
	if(( param->down_sample_y < D_IM_FPT_DS_MIN ) || ( D_IM_FPT_DS_MAX < param->down_sample_y )){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	im_fpt_on_pclk( ch );

	switch( param->lutcfg ){
		case E_IM_FPT_LUTCFG_OFF:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_OFF );
			break;
		case E_IM_FPT_LUTCFG_BASE_0:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_ON | D_IM_FPT_LUT_LUT0 | D_IM_FPT_LUT_BASE );
			break;
		case E_IM_FPT_LUTCFG_TARGET_1:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_ON | D_IM_FPT_LUT_LUT1 | D_IM_FPT_LUT_TARGET );
			break;
		case E_IM_FPT_LUTCFG_BASE_0__TARGET_1:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_ON | D_IM_FPT_LUT_BASE_X__TARGET_Y );
			break;
		case E_IM_FPT_LUTCFG_BASE_1__TARGET_0:
			IO_FPT[ch].FPTCTL3.word = ( D_IM_FPT_LUT_ON | D_IM_FPT_LUT_LUT1 | D_IM_FPT_LUT_BASE_X__TARGET_Y );
			break;
		default:
			return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	fptctl2.word = IO_FPT[ch].FPTCTL2.word;
	fptctl2.bit.MODE = 3;
	IO_FPT[ch].FPTCTL2.word = fptctl2.word;

	fptfqctl1.word = 0;
	fptfqctl1.bit.HSIZE = param->width;
	fptfqctl1.bit.VSIZE = param->lines;
	IO_FPT[ch].FPTFQCTL1.word = fptfqctl1.word;

	fptfqctl2.word = 0;
	fptfqctl2.bit.SUMW  = param->window_size;
	fptfqctl2.bit.FPMAX = param->num_feature_point;
	IO_FPT[ch].FPTFQCTL2.word = fptfqctl2.word;

	if(!( fptctl2.bit.MODE == 0 || fptctl2.bit.IO_MODE == 1 )){			/* pgr0872 */
		IO_FPT[ch].FPTFQCTL7.word = param->output_addr;
	}

	fptfqctl9.word = 0;
	fptfqctl9.bit.SKIPX_FQ = param->down_sample_x;
	fptfqctl9.bit.SKIPY_FQ = param->down_sample_y;
	IO_FPT[ch].FPTFQCTL9.word = fptfqctl9.word;

	fptftctl0.word = 0;
	fptftctl0.bit.GHBSIZE = param->base_global_width;
	fptftctl0.bit.GHTSIZE = param->target_global_width;
	IO_FPT[ch].FPTFTCTL0.word = fptftctl0.word;

	fptftctl1.word = 0;
	fptftctl1.bit.FTMAX  = param->num_feature_point;
	fptftctl1.bit.INIT_0 = param->usage_target_coord;
	IO_FPT[ch].FPTFTCTL1.word = fptftctl1.word;

	fptftctl2.word = 0;
	fptftctl2.bit.LEFTLIMIT  = param->search_window.left;
	fptftctl2.bit.RIGHTLIMIT = param->search_window.right;
	IO_FPT[ch].FPTFTCTL2.word = fptftctl2.word;

	fptftctl3.word = 0;
	fptftctl3.bit.TOPLIMIT    = param->search_window.top;
	fptftctl3.bit.BOTTOMLIMIT = param->search_window.bottom;
	IO_FPT[ch].FPTFTCTL3.word = fptftctl3.word;

	fptftctl4.word = 0;
	fptftctl4.bit.MAXERR = param->tracking_threshold;
	fptftctl4.bit.MAXITR = param->num_iteration;
	IO_FPT[ch].FPTFTCTL4.word = fptftctl4.word;

	IO_FPT[ch].FPTFTCTL5.word = param->base_img_addr;

	IO_FPT[ch].FPTFTCTL6.word = param->target_img_addr;

	if( fptctl2.bit.IO_MODE != 1 ){			/* pgr0872 */
		IO_FPT[ch].FPTFTCTL7.word = param->base_coord_addr;
		IO_FPT[ch].FPTFTCTL8.word = param->target_coord_addr;
		IO_FPT[ch].FPTFTCTL9.word = param->tracking_result_addr;
	}

	if( IO_FPT[ch].FPTCTL1.word == D_IM_FPT_TRG_STOPPED ){
		fptftctl10.word = 0;
		fptftctl10.bit.SKIPX_FT = param->down_sample_x;
		fptftctl10.bit.SKIPY_FT = param->down_sample_y;
		IO_FPT[ch].FPTFTCTL10.word = fptftctl10.word;
	}

	im_fpt_off_pclk( ch );

	return D_IM_FPT_RETVAL_OK;
}

/**
   Configure MODE.
 */
INT32 Im_FPT_Ctrl_Mode_Config( UCHAR ch, const T_IM_FPT_MODE_CONFIG* const param )
{
	union io_fpt_fptctl2	fptctl2;

	if( param == NULL ){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	im_fpt_on_pclk( ch );

	fptctl2.word = IO_FPT[ch].FPTCTL2.word;

	switch( param->io_mode ){
		case E_IM_FPT_IO_MODE_SDRAM:
			fptctl2.bit.IO_MODE = D_IM_FPT_IO_MODE_SDRAM;
			break;
		case E_IM_FPT_IO_MODE_REGISTER:
			fptctl2.bit.IO_MODE = D_IM_FPT_IO_MODE_REGISTER;
			break;
		default:
			return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	switch( param->pipe_en ){
		case E_IM_FPT_PIPE_EN_DISABLE:
			fptctl2.bit.PIPE_EN = D_IM_FPT_PIPE_DISABLE;
			break;
		case E_IM_FPT_PIPE_EN_ENABLE:
			fptctl2.bit.PIPE_EN = D_IM_FPT_PIPE_ENABLE;
			break;
		default:
			return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	switch( param->debug_mode ){
		case E_IM_FPT_DEBUG_MODE_NORMAL:
			fptctl2.bit.DEBUG_MODE = D_IM_FPT_NORMAL_MODE;
			break;
		case E_IM_FPT_DEBUG_MODE_DEBUG:
			fptctl2.bit.DEBUG_MODE = D_IM_FPT_DEBUG_MODE;
			break;
		default:
			return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	IO_FPT[ch].FPTCTL2.word = fptctl2.word;

	im_fpt_off_pclk( ch );

	return D_IM_FPT_RETVAL_OK;
}

/**
   Configure BRC(BRightness Correction) of the base image.
 */
INT32 Im_FPT_Ctrl_BRC_Config_Base_Img( UCHAR ch, const T_IM_FPT_BRC_CONFIG* const param )
{
	union io_fpt_fptctl4 fptctl4;

	if( param == NULL ){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	fptctl4.word = 0;

	im_fpt_on_pclk( ch );

	if( IO_FPT[ch].FPTCTL3.word &= D_IM_FPT_LUT_ON ){
		fptctl4.bit.OFFSET_B = param->img_offset;
		fptctl4.bit.CLPMAX_B = param->img_clip_max;
		fptctl4.bit.CLPMIN_B = param->img_clip_min;
		IO_FPT[ch].FPTCTL4.word = fptctl4.word;
	}
	else{
		fptctl4.bit.OFFSET_B = D_IM_FPT_OFFSET_OFF;
		fptctl4.bit.CLPMAX_B = D_IM_FPT_CLIP_MAX_OFF;
		fptctl4.bit.CLPMIN_B = D_IM_FPT_CLIP_MIN_OFF;
		IO_FPT[ch].FPTCTL4.word = fptctl4.word;
	}

	im_fpt_off_pclk( ch );

	return D_IM_FPT_RETVAL_OK;
}

/**
   Configure BRC(BRightness Correction) of the target image.
 */
INT32 Im_FPT_Ctrl_BRC_Config_Target_Img( UCHAR ch, const T_IM_FPT_BRC_CONFIG* const param )
{
	union io_fpt_fptctl5 fptctl5;

	if( param == NULL ){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	fptctl5.word = 0;

	im_fpt_on_pclk( ch );

	if( IO_FPT[ch].FPTCTL3.word &= D_IM_FPT_LUT_ON ){
		fptctl5.bit.OFFSET_T = param->img_offset;
		fptctl5.bit.CLPMAX_T = param->img_clip_max;
		fptctl5.bit.CLPMIN_T = param->img_clip_min;
		IO_FPT[ch].FPTCTL5.word = fptctl5.word;
	}
	else{
		fptctl5.bit.OFFSET_T = D_IM_FPT_OFFSET_OFF;
		fptctl5.bit.CLPMAX_T = D_IM_FPT_CLIP_MAX_OFF;
		fptctl5.bit.CLPMIN_T = D_IM_FPT_CLIP_MIN_OFF;
		IO_FPT[ch].FPTCTL5.word = fptctl5.word;
	}

	im_fpt_off_pclk( ch );

	return D_IM_FPT_RETVAL_OK;
}

/**
   Configure Interrupt.
 */
INT32 Im_FPT_Ctrl_Interrupt( UCHAR ch, const T_IM_FPT_INTERRUPT* const param )
{
	if( param == NULL ){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	im_fpt_on_pclk( ch );

	IO_FPT[ch].FPTINTE0.word = param->flag;

	im_fpt_off_pclk( ch );

	gFpt_Callback[ch] = param->callback;

	return D_IM_FPT_RETVAL_OK;
}

/**
   Configure LUTRAM.
 */
INT32 Im_FPT_Ctrl_LUTRAM( UCHAR ch, INT32 lut_sel, const UCHAR* lut )
{
	INT32 i;
	ULONG regVal;
	ULONG *pRegAddr;

	if( IO_FPT[ch].FPTCTL1.word == D_IM_FPT_TRG_RUNNING ){
		return D_IM_FPT_RETVAL_BUSY_ERR;
	}

	if((( lut_sel != 0 ) && ( lut_sel != 1 ))
	|| ( lut == NULL )){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	pRegAddr = ( lut_sel == 0 ) ? (ULONG*)&IO_FPT_AHB[ch].FPTLUT0.word[0] : (ULONG*)&IO_FPT_AHB[ch].FPTLUT1.word[0];

	DDIM_User_AhbReg_SpinLock();
	im_fpt_on_hclk( ch );

	for( i = 0; i < 64; i++ ) {// 64*4 = 256
		regVal  = (ULONG)((*lut++) <<  0);
		regVal |= (ULONG)((*lut++) <<  8);
		regVal |= (ULONG)((*lut++) << 16);
		regVal |= (ULONG)((*lut++) << 24);
		*pRegAddr++ = regVal;
	}

	im_fpt_off_hclk( ch );
	DDIM_User_AhbReg_SpinUnLock();

	return D_IM_FPT_RETVAL_OK;
}

/**
   Set REGRAM.
 */
INT32 Im_FPT_Set_REGRAM( UCHAR ch, INT32 reg_sel, const UCHAR* reg )
{
	INT32 i;
	ULONG regVal;
	ULONG *pRegAddr;
	union io_fpt_fptctl2 fptctl2;

	if( IO_FPT[ch].FPTCTL1.word == D_IM_FPT_TRG_RUNNING ){
		return D_IM_FPT_RETVAL_BUSY_ERR;
	}

	fptctl2.word = IO_FPT[ch].FPTCTL2.word;

	if( fptctl2.bit.IO_MODE != 1 ){			/* pgr0872 */
		return D_IM_FPT_RETVAL_ERR;
	}

	if((( reg_sel != 0 ) && ( reg_sel != 1 ))
	|| ( reg == NULL )){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	pRegAddr = ( reg_sel == 0 ) ? (ULONG*)&IO_FPT_AHB[ch].FPTREG0.word[0] : (ULONG*)&IO_FPT_AHB[ch].FPTREG1.word[0];

	DDIM_User_AhbReg_SpinLock();
	im_fpt_on_hclk( ch );

	for( i = 0; i < 128; i++ ){	// 128*4 = 512
		regVal  = (ULONG)((*reg++) <<  0);
		regVal |= (ULONG)((*reg++) <<  8);
		regVal |= (ULONG)((*reg++) << 16);
		regVal |= (ULONG)((*reg++) << 24);
		*pRegAddr++ = regVal;
	}

	im_fpt_off_hclk( ch );
	DDIM_User_AhbReg_SpinUnLock();

	return D_IM_FPT_RETVAL_OK;
}

/**
   Get REGRAM.
 */
INT32 Im_FPT_Get_REGRAM( UCHAR ch, INT32 reg_sel, ULONG* buf_addr )
{
	INT32 i;
	ULONG *pRegAddr;
	union io_fpt_fptctl2 fptctl2;

	if( IO_FPT[ch].FPTCTL1.word == D_IM_FPT_TRG_RUNNING ){
		return D_IM_FPT_RETVAL_BUSY_ERR;
	}

	fptctl2.word = IO_FPT[ch].FPTCTL2.word;

	if( fptctl2.bit.IO_MODE != 1 ){			/* pgr0872 */
		return D_IM_FPT_RETVAL_ERR;
	}

	if((( reg_sel != 0 ) && ( reg_sel != 1 ))
	|| ( buf_addr == NULL )){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	pRegAddr = ( reg_sel == 0 ) ? (ULONG*)&IO_FPT_AHB[ch].FPTREG0.word[0] : (ULONG*)&IO_FPT_AHB[ch].FPTREG1.word[0];

	DDIM_User_AhbReg_SpinLock();
	im_fpt_on_hclk( ch );

	for( i = 0; i < 128; i++ ){	// 128*4 = 512
		*buf_addr++ = (*pRegAddr++);
	}

	im_fpt_off_hclk( ch );
	DDIM_User_AhbReg_SpinUnLock();

	return D_IM_FPT_RETVAL_OK;
}

/**
   Start FPT.
 */
INT32 Im_FPT_Start( UCHAR ch )
{
	INT32 retval;

	im_fpt_on_pclk( ch );
	im_fpt_on_hclk( ch );
	im_fpt_on_iclk( ch );

	if( IO_FPT[ch].FPTCTL1.word == D_IM_FPT_TRG_STOPPED ){

		DDIM_USER_ER ercd;

		ercd = DDIM_User_Clr_Flg( FID_IM_FPT(ch), ~D_IM_FPT_USED_FLG_ALL );

		if( D_DDIM_USER_E_OK != ercd ){
			im_fpt_off_iclk( ch );
			im_fpt_off_hclk( ch );
			im_fpt_off_pclk( ch );
			return D_IM_FPT_RETVAL_SYSTEM_CALL_ERR;
		}

		IO_FPT[ch].FPTCTL1.word = D_IM_FPT_TRG_START;
#ifdef CO_DEBUG_ON_PC
		IO_FPT[ch].FPTCTL1.word = D_IM_FPT_TRG_RUNNING;
#endif

		retval = D_IM_FPT_RETVAL_OK;
	}
	else{
		retval = D_IM_FPT_RETVAL_ERR;
	}

	return retval;
}

/**
   Force stop FPT.
 */
INT32 Im_FPT_Stop( UCHAR ch )
{
	INT32 retval = D_IM_FPT_RETVAL_OK;

	if ( IO_FPT[ch].FPTCTL1.word == D_IM_FPT_TRG_STOPPED ) {
		retval = D_IM_FPT_RETVAL_ERR;
	}
	else{
		DDIM_USER_ER ercd;

		IO_FPT[ch].FPTCTL1.word = D_IM_FPT_TRG_ABORT;
#ifdef CO_DEBUG_ON_PC
		IO_FPT[ch].FPTCTL1.word = D_IM_FPT_TRG_STOPPED;
#endif
		im_fpt_dummy_read( ch );

		ercd = DDIM_User_Set_Flg( FID_IM_FPT(ch), D_IM_FPT_ABORT_STOP );

		if( ercd == D_DDIM_USER_E_OK ){
			retval = D_IM_FPT_RETVAL_OK;
		}
		else{
			retval = D_IM_FPT_RETVAL_SYSTEM_CALL_ERR;
		}
	}

	im_fpt_off_iclk( ch );
	im_fpt_off_hclk( ch );
	im_fpt_off_pclk( ch );

	return retval;
}

/**
   Wait end.
 */
INT32 Im_FPT_WaitEnd( UCHAR ch, UINT32* status, INT32 wait_time )
{
	INT32 retval;

	if(( status == NULL ) || ( wait_time < -1 )){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	*status = 0;

	im_fpt_on_pclk( ch );

	if( IO_FPT[ch].FPTINTE0.word & D_IM_FPT_INTERRUPT_COMPLETE ){
		// interrupt wait (open spec.)

		DDIM_USER_ER ercd;
		DDIM_USER_FLGPTN flgptn;

		ercd = DDIM_User_Twai_Flg( FID_IM_FPT(ch), D_IM_FPT_USED_FLG_ALL, D_DDIM_USER_TWF_ORW, &flgptn, wait_time );
		if( D_DDIM_USER_E_OK == ercd ){
			*status = flgptn;
			retval = D_IM_FPT_RETVAL_OK;
		}
		else{
			retval = D_IM_FPT_RETVAL_SYSTEM_CALL_ERR;
		}
		ercd = DDIM_User_Clr_Flg( FID_IM_FPT(ch), ~flgptn );
	}
	else{
		// polling wait (closed spec.)
		do{
			if( IO_FPT[ch].FPTCTL1.word == D_IM_FPT_TRG_STOPPED ) {
				break;
			}
		}while( 1 );

		retval = D_IM_FPT_RETVAL_OK;
	}

	im_fpt_off_pclk( ch );

	return retval;
}

/**
   Interrupt Handler.
*/
INT32 Im_FPT_Int_Handler( UCHAR ch )
{
	UINT32 inte, intf;
	UINT32 int_factor;
	DDIM_USER_ER ercd;
	union io_fpt_fptaxierr0 fptaxierr0;

	im_fpt_on_pclk( ch );

	inte = IO_FPT[ch].FPTINTE0.word;
	intf = IO_FPT[ch].FPTINTF0.word;
	if( intf & D_IM_FPT_INTERRUPT_AXI_ERR ){
		fptaxierr0.word = IO_FPT[ch].FPTAXIERR0.word;
		gFpt_RRESP[ch] = fptaxierr0.bit.RRESP;			/* pgr0872 */
		gFpt_WRESP[ch] = fptaxierr0.bit.WRESP;			/* pgr0872 */
	}
	else{
		gFpt_RRESP[ch] = 0;
		gFpt_WRESP[ch] = 0;
	}
	IO_FPT[ch].FPTINTF0.word = intf;
	im_fpt_dummy_read( ch );

	im_fpt_off_pclk( ch );

	int_factor = 0;

	if(( inte & D_IM_FPT_INTERRUPT_COMPLETE )
	&& ( intf & D_IM_FPT_INTERRUPT_COMPLETE )){
		int_factor |= D_IM_FPT_INTERRUPT_COMPLETE;
	}

	if( intf & D_IM_FPT_INTERRUPT_ERR ){
		if(( inte & D_IM_FPT_INTERRUPT_AXI_ERR )
		&& ( intf & D_IM_FPT_INTERRUPT_AXI_ERR )){
			int_factor |= D_IM_FPT_INTERRUPT_AXI_ERR;
		}

		if(( inte & D_IM_FPT_INTERRUPT_PARAM_ERR )
		&& ( intf & D_IM_FPT_INTERRUPT_PARAM_ERR )){
			int_factor |= D_IM_FPT_INTERRUPT_PARAM_ERR;
		}
	}

	if( int_factor ){
		ercd = DDIM_User_Set_Flg( FID_IM_FPT(ch), ( int_factor ));
		if( D_DDIM_USER_E_OK != ercd ){
			return D_IM_FPT_RETVAL_SYSTEM_CALL_ERR;
		}
	}

	if( gFpt_Callback[ch] != NULL ){
		gFpt_Callback[ch]( int_factor, ch );
	}

	return D_IM_FPT_RETVAL_OK;
}

/**
   Get AXI response.
 */
INT32 Im_FPT_Get_Axi_Response( UCHAR ch, UCHAR* read_response, UCHAR* write_response )
{
	if(( read_response == NULL ) || ( write_response == NULL )){
		return D_IM_FPT_RETVAL_INVALID_ARG_ERR;
	}

	*read_response = gFpt_RRESP[ch];
	*write_response = gFpt_WRESP[ch];

	return D_IM_FPT_RETVAL_OK;
}

/**
   Dump the value of all FPT's register.
 */
VOID Im_FPT_DebugDump_All_Registers( UCHAR ch, UINT32 buf0_addr, UINT32 buf1_addr )
{
	INT32 i;
	UINT32 *reg_ptr;
	UINT32 *dst_ptr;

	if(( buf0_addr & 0x3 )
	|| ( buf1_addr & 0x3 )
	|| ( buf0_addr == 0x0 )
	|| ( buf1_addr == 0x0 )){
		return;
	}

	// APB
	reg_ptr = (UINT32*)&IO_FPT[ch].FPTCTL0.word;
	dst_ptr = (UINT32*)buf0_addr;

	im_fpt_on_pclk( ch );

	for( i = 0; i < 64; i++ ){
		*dst_ptr++ = *reg_ptr++;
	}

	im_fpt_off_pclk( ch );

	// AHB
	reg_ptr = (UINT32*)&IO_FPT_AHB[ch].FPTLUT0.word[0];
	dst_ptr = (UINT32*)buf1_addr;

	DDIM_User_AhbReg_SpinLock();
	im_fpt_on_hclk( ch );

	for( i = 0; i < 128; i++ ){
		*dst_ptr++ = *reg_ptr++;
	}

	im_fpt_off_hclk( ch );
	DDIM_User_AhbReg_SpinUnLock();
}

/**
   Get version info.
 */
VOID Im_FPT_Get_Version( char** str )
{
	*str = D_IM_FPT_VERSION;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif
