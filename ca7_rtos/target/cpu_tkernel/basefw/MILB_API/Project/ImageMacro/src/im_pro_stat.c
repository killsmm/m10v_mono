/**
 * @file		im_pro_stat.c
 * @brief		SRO unit controler of PRO macro.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "jdspro.h"
#include "im_pro.h"
#include "im_pro_common.h"
#include "dd_top.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	D_IM_STAT_STATUS_NONE					(0x00000000UL)				/* All stopped	*/
#define	D_IM_STAT_STATUS_AEAWB_0				((0x00000000UL)&(1<< 0))	/* AEAWB0		*/
#define	D_IM_STAT_STATUS_AEAWB_1				((0x00000000UL)&(1<< 1))	/* AEAWB1		*/
#define	D_IM_STAT_STATUS_AEAWB_2				((0x00000000UL)&(1<< 2))	/* AEAWB2		*/
#define	D_IM_STAT_STATUS_AEAWB_3				((0x00000000UL)&(1<< 3))	/* AEAWB3		*/
#define	D_IM_STAT_STATUS_HIST_0					((0x00000000UL)&(1<< 4))	/* HIST0		*/
#define	D_IM_STAT_STATUS_HIST_1					((0x00000000UL)&(1<< 5))	/* HIST1		*/
#define	D_IM_STAT_STATUS_HIST_2					((0x00000000UL)&(1<< 6))	/* HIST2		*/
#define	D_IM_STAT_STATUS_HIST_3					((0x00000000UL)&(1<< 7))	/* HIST3		*/
#define	D_IM_STAT_STATUS_AF_0					((0x00000000UL)&(1<< 8))	/* AF0			*/
#define	D_IM_STAT_STATUS_AF_1					((0x00000000UL)&(1<< 9))	/* AF1			*/
#define	D_IM_STAT_STATUS_FLCK_0					((0x00000000UL)&(1<<10))	/* FLCK0		*/
#define	D_IM_STAT_STATUS_FLCK_1					((0x00000000UL)&(1<<11))	/* FLCK1		*/

#define	D_IM_PRO_AEAEB_AE_BLK_SIZE				(4)			/* AE block data size		*/
#define	D_IM_PRO_AEAEB_AWB_BLK_SIZE				(16)		/* AWB block data size		*/
#define	D_IM_PRO_AEAEB_IAWB_BLK_SIZE			(128)		/* IAWB block data size		*/
#define	D_IM_PRO_AEAEB_DUMMY_DATA_SIZE			(4)			/* Dummy block data size	*/

#define D_IM_PRO_FLCK0_PWCH			(E_IM_PRO_PWCH_4)
#define D_IM_PRO_FLCK1_PWCH			(E_IM_PRO_PWCH_5)


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// PRO Channel/Macro start/stop status
#define im_pro_stat_set_start_status(st, ch) 	(gIM_STAT_START_Status[((st) >> 28) & 0xf] |=  (((st) & 0x0fffffff) << (ch)))								/* SEN clock status ON	*/
#define im_pro_stat_set_stop_status(st, ch)		(gIM_STAT_START_Status[((st) >> 28) & 0xf] &= ~(((st) & 0x0fffffff) << (ch)))								/* SEN clock status OFF	*/
#define im_pro_stat_get_start_status(st, ch)	(((gIM_STAT_START_Status[((st) >> 28) & 0xf] & (((st) & 0x0fffffff) << (ch))) != 0) ? TRUE : FALSE)			/* started check	*/

#define im_pro_stat_get_stop_all()				((gIM_STAT_START_Status[0] != 0) ? FALSE : TRUE)	/* started check	*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// clock control status
static	volatile	ULONG	gIM_STAT_START_Status[1] = {0x00000000};

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/********************************************************/
/*	STAT Top												*/
/********************************************************/
/**
STATTOP Macro initialize
@remarks	Please Call at the time of system starting.
*/
VOID Im_PRO_STATTOP_Init( VOID )
{
	// Software release
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.STATTOP.SR.bit.SR = D_IM_PRO_SR_RELEASE;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock
}

/**
STATTOP Macro software reset
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: All macro not stopped NG
*/
INT32 Im_PRO_STATTOP_SW_Reset( VOID )
{
#ifdef CO_ACT_PRO_CLOCK
	if (im_pro_stat_get_stop_all() == FALSE){	//not ALL Stopped
		Ddim_Print(("I:Im_PRO_STATTOP_SW_Reset. macro not stopped error. \n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif	/* CO_ACT_PRO_CLOCK */

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock

	// Software reset
	IO_PRO.STAT.STATTOP.SR.bit.SR = D_IM_PRO_SR_RESET;

	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
STATTOP Macro clock control
@param[in]	clk_type	: STATTOP Macro clock type
@param[in]	on_off		: 0:clock on 1:clock off
@param[in]	wait_skip	: 0:non wait 1:wait 1ms. for wait PROCLK/CDK 5 cycle.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_STATTOP_Control_Clock( E_IM_PRO_STATTOP_CLK_TYPE clk_type, UCHAR on_off, UCHAR wait_skip )
{
	UINT32 cpu_clk_hz;
	UINT32 macro_clk_hz;
#ifdef CO_PARAM_CHECK
	if (on_off > D_IM_PRO_CLOCK_OFF){
		Ddim_Assertion(("I:Im_PRO_STATTOP_Control_Clock error. on_off value over!! (%d)\n", on_off));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else {
		// DO NOTHING
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock

	switch( clk_type ) {
		case E_IM_PRO_STATTOP_CLK_TYPE_STATCLK:
			IO_PRO.STAT.STATTOP.CLKSTP.bit.PSTP1 = ( ( on_off == D_IM_PRO_CLOCK_ON ) ? D_IM_PRO_CLOCK_ON : D_IM_PRO_CLOCK_OFF );
			break;

		default:
			break;
	}
	// waiting for 5 cycle
	if( wait_skip == 0 ) {
		im_pro_get_current_clk_hz( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT, &cpu_clk_hz, &macro_clk_hz );
		im_pro_common_wait_by_clk_cycle( 5, macro_clk_hz, cpu_clk_hz );
#ifndef CO_DEBUG_ON_PC
//			__nop();
#endif
	}

	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
STATTOP macro control data setting
@param[in]	ctrl : STATTOP macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_STATTOP_Ctrl( T_IM_PRO_STATTOP_CTRL* ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_STATTOP_Ctrl error. ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	Im_PRO_STATTOP_Control_Clock( E_IM_PRO_STATTOP_CLK_TYPE_STATCLK, D_IM_PRO_CLOCK_OFF, 0 );

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.STATTOP.STATTOPCTL1.bit.AFSW0	= ctrl->af_input_sel[0];
	IO_PRO.STAT.STATTOP.STATTOPCTL1.bit.AFSW1	= ctrl->af_input_sel[1];
	IO_PRO.STAT.STATTOP.STATTOPCTL1.bit.FLSW0	= ctrl->flck_input_sel[0];
	IO_PRO.STAT.STATTOP.STATTOPCTL1.bit.FLSW1	= ctrl->flck_input_sel[1];
	IO_PRO.STAT.STATTOP.STATTOPCTL1.bit.AESW0	= ctrl->aeawb_input_sel[0];
	IO_PRO.STAT.STATTOP.STATTOPCTL1.bit.AESW1	= ctrl->aeawb_input_sel[1];
	IO_PRO.STAT.STATTOP.STATTOPCTL1.bit.AESW2	= ctrl->aeawb_input_sel[2];
	IO_PRO.STAT.STATTOP.STATTOPCTL1.bit.AESW3	= ctrl->aeawb_input_sel[3];
	IO_PRO.STAT.STATTOP.STATTOPCTL1.bit.HISW0	= ctrl->hist_input_sel[0];
	IO_PRO.STAT.STATTOP.STATTOPCTL1.bit.HISW1	= ctrl->hist_input_sel[1];
	IO_PRO.STAT.STATTOP.STATTOPCTL1.bit.HISW2	= ctrl->hist_input_sel[2];
	IO_PRO.STAT.STATTOP.STATTOPCTL1.bit.HISW3	= ctrl->hist_input_sel[3];
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	Im_PRO_STATTOP_Control_Clock( E_IM_PRO_STATTOP_CLK_TYPE_STATCLK, D_IM_PRO_CLOCK_ON, 0 );

	return D_DDIM_OK;
}

/**
AEAWB macro start
@retval		D_DDIM_OK				: Processing OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PWCH not running NG
*/
INT32 Im_PRO_AEAWB_Start( E_IM_PRO_AEAWB_CH ch )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AEAWB[ch].AETRG.bit.AETRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	im_pro_stat_set_start_status(D_IM_STAT_STATUS_AEAWB_0, ch);

	return D_DDIM_OK;
}

/**
AEAWB macro stop
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: PWCH not stopped NG
*/
INT32 Im_PRO_AEAWB_Stop( E_IM_PRO_AEAWB_CH ch, UCHAR force )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.STAT.AEAWB[ch].AETRG.bit.AETRG = D_IM_PRO_TRG_FRAME_STOP;	// stop
	}
	else {
		IO_PRO.STAT.AEAWB[ch].AETRG.bit.AETRG = D_IM_PRO_TRG_FORCE_STOP;	// force stop
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	im_pro_stat_set_stop_status(D_IM_STAT_STATUS_AEAWB_0, ch);

	return D_DDIM_OK;
}

/**
AE/AWB data SDRAM write enable set
@param[in]	aeawb_ctrl : AEAWB Information @ref T_IM_PRO_AEAWB_CTRL
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AEAWB_Ctrl( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_CTRL* aeawb_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (aeawb_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_AEAWB_Ctrl error. aeawb_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AEAWB_AEHBC_MIN, D_IM_PRO_AEAWB_AEHBC_MAX, aeawb_ctrl->h_bc_num, "Im_PRO_AEAWB_Ctrl : h_bc_num" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AEAWB_AEVBC_MIN, D_IM_PRO_AEAWB_AEVBC_MAX, aeawb_ctrl->v_bc_num, "Im_PRO_AEAWB_Ctrl : v_bc_num" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	if( Im_PRO_AEAWB_Set_WD_Mode( ch, aeawb_ctrl->aeawb_mode ) != D_DDIM_OK ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}

	if( Im_PRO_AEAWB_Set_AWBDataFormat( ch, aeawb_ctrl->awb_mode ) != D_DDIM_OK ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}

	if( Im_PRO_AEAWB_Set_WD_Window_Mode( ch, aeawb_ctrl->awb_wd_window_mode ) != D_DDIM_OK ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}

	if( Im_PRO_AEAWB_Set_Area( ch, &aeawb_ctrl->awb_area ) != D_DDIM_OK ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}

	if( Im_PRO_AEAWB_Set_Blocks( ch, aeawb_ctrl->h_bc_num, aeawb_ctrl->v_bc_num ) != D_DDIM_OK ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of AEAWB.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AEAWB_Set_PAEN( E_IM_PRO_AEAWB_CH ch, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.STAT.AEAWB[ch].AETRG.bit.AETRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_AEAWB_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AEAWB[ch].AEAWBPAEN.bit.PAEN = paen_trg;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
AWB detect data format select.
@param[in]	awb_format : AWB detect data format @ref E_IM_PRO_AWB_MODE
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AEAWB_Set_AWBDataFormat( E_IM_PRO_AEAWB_CH ch, E_IM_PRO_AWB_MODE awb_format )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AEAWB[ch].AEAWBCTL.bit.AWBGMD	= awb_format;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
AE/AWB detecting mode set
@param[in]	wb_mode : AEAWB Detecting mode @ref E_IM_PRO_AEAWB_MODE
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AEAWB_Set_WD_Mode( E_IM_PRO_AEAWB_CH ch, E_IM_PRO_AEAWB_MODE wb_mode )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AEAWB[ch].AEAWBCTL.bit.AEAWBWMD	= wb_mode;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
AE/AWB detecting window mode set
@param[in]	wb_win_mode : AEAWB Detecting window mode @ref E_IM_PRO_AEAWB_WD_WINDOW_MODE
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AEAWB_Set_WD_Window_Mode( E_IM_PRO_AEAWB_CH ch, E_IM_PRO_AEAWB_WD_WINDOW_MODE wb_win_mode )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AEAWB[ch].AEAWBCTL.bit.AEEWMD	= wb_win_mode;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set AE/AWB Window size
* @param[in]	aeawb_area :aeawb window information.

@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AEAWB_Set_Area( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_AREA* aeawb_area )
{
#ifdef CO_PARAM_CHECK
	if (aeawb_area == NULL){
		Ddim_Assertion(("I:Im_PRO_AEAWB_Set_Area error. aeawb_area=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AEAWB_AEH_MIN, D_IM_PRO_AEAWB_AEH_MAX, aeawb_area->pos_x, "Im_PRO_AEAWB_Set_Area : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AEAWB_AEV_MIN, D_IM_PRO_AEAWB_AEV_MAX, aeawb_area->pos_y, "Im_PRO_AEAWB_Set_Area : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AEAWB_AEHW_MIN, D_IM_PRO_AEAWB_AEHW_MAX, aeawb_area->width, "Im_PRO_AEAWB_Set_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AEAWB_AEVW_MIN, D_IM_PRO_AEAWB_AEVW_MAX, aeawb_area->lines, "Im_PRO_AEAWB_Set_Area : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AEAWB_AETW_MIN, D_IM_PRO_AEAWB_AETW_MAX, aeawb_area->top_lines, "Im_PRO_AEAWB_Set_Area : top_lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AEAWB_AEBW_MIN, D_IM_PRO_AEAWB_AEBW_MAX, aeawb_area->bottom_lines, "Im_PRO_AEAWB_Set_Area : bottom_lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AEAWB_AELW_MIN, D_IM_PRO_AEAWB_AELW_MAX, aeawb_area->left_width, "Im_PRO_AEAWB_Set_Area : left_width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AEAWB_AERW_MIN, D_IM_PRO_AEAWB_AERW_MAX, aeawb_area->right_width, "Im_PRO_AEAWB_Set_Area : right_width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AEAWB[ch].AEH.bit.AEH	= aeawb_area->pos_x;
	IO_PRO.STAT.AEAWB[ch].AEV.bit.AEV	= aeawb_area->pos_y;
	IO_PRO.STAT.AEAWB[ch].AEHW.bit.AEHW	= aeawb_area->width;
	IO_PRO.STAT.AEAWB[ch].AEVW.bit.AEVW	= aeawb_area->lines;
	IO_PRO.STAT.AEAWB[ch].AETW.bit.AETW	= aeawb_area->top_lines;
	IO_PRO.STAT.AEAWB[ch].AEBW.bit.AEBW	= aeawb_area->bottom_lines;
	IO_PRO.STAT.AEAWB[ch].AELW.bit.AELW	= aeawb_area->left_width;
	IO_PRO.STAT.AEAWB[ch].AERW.bit.AERW	= aeawb_area->right_width;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set AE/AWB Window block number
@param[in]	h_bc_num :horizontal AEAWB Window block number<br>
						value range  :[1 - 64]<br>
						target registor  :@@AEHBC<br>
						Note that if level WB is enabled (aeawb_mode=E_IM_PRO_AEAWB_MODE_LWB~E_IM_PRO_AEAWB_MODE_AE_AWB_LWB),
						the upper limit of this register is 32.

@param[in]	v_bc_num :vertical AEAWB Window block number<br> 
					  value range  :[1 - 64]<br>
					  target registor  :@@AEVBC
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32	Im_PRO_AEAWB_Set_Blocks( E_IM_PRO_AEAWB_CH ch, USHORT h_bc_num, USHORT v_bc_num )
{
#ifdef CO_PARAM_CHECK
	if( im_pro_check_val_range( D_IM_PRO_AEAWB_AEHBC_MIN, D_IM_PRO_AEAWB_AEHBC_MAX, h_bc_num, "Im_PRO_AEAWB_Set_Blocks : h_bc_num" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AEAWB_AEVBC_MIN, D_IM_PRO_AEAWB_AEVBC_MAX, v_bc_num, "Im_PRO_AEAWB_Set_Blocks : v_bc_num" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AEAWB[ch].AEVBC.bit.AEVBC	= v_bc_num;
	IO_PRO.STAT.AEAWB[ch].AEHBC.bit.AEHBC	= h_bc_num;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock
	return D_DDIM_OK;
}

/**
Set calculation permission in AE/AWB Window block.
* @param[in]	aeawb_cw :AE/AWB Addition permission size in a block information<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AEAWB_Set_CW( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_CW* aeawb_cw )
{
#ifdef CO_PARAM_CHECK
	if (aeawb_cw == NULL){
		Ddim_Assertion(("I:Im_PRO_AEAWB_Set_CW error. aeawb_cw=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AEAWB[ch].AECW1.bit.AECVW	= aeawb_cw->aecvw;
	IO_PRO.STAT.AEAWB[ch].AECW1.bit.AECHW	= aeawb_cw->aechw;
	IO_PRO.STAT.AEAWB[ch].AECW2.bit.AECTW	= aeawb_cw->aectw;
	IO_PRO.STAT.AEAWB[ch].AECW3.bit.AECBW	= aeawb_cw->aecbw;
	IO_PRO.STAT.AEAWB[ch].AECW2.bit.AECLW	= aeawb_cw->aeclw;
	IO_PRO.STAT.AEAWB[ch].AECW3.bit.AECRW	= aeawb_cw->aecrw;
	IO_PRO.STAT.AEAWB[ch].AWBCW1.bit.AWBCVW	= aeawb_cw->awcvw;
	IO_PRO.STAT.AEAWB[ch].AWBCW1.bit.AWBCHW	= aeawb_cw->awchw;
	IO_PRO.STAT.AEAWB[ch].AWBCW2.bit.AWBCTW	= aeawb_cw->awctw;
	IO_PRO.STAT.AEAWB[ch].AWBCW3.bit.AWBCBW	= aeawb_cw->awcbw;
	IO_PRO.STAT.AEAWB[ch].AWBCW2.bit.AWBCLW	= aeawb_cw->awclw;
	IO_PRO.STAT.AEAWB[ch].AWBCW3.bit.AWBCRW	= aeawb_cw->awcrw;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock
	return D_DDIM_OK;
}

/**
AWB RGB level gate threshold
@param[in]	lv_gate	:	AWB RGB gate threshold
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AEAWB_Set_Lv_Gate( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_LV_GATE* lv_gate )
{
#ifdef CO_PARAM_CHECK
	if (lv_gate == NULL){
		Ddim_Assertion(("I:Im_PRO_AEAWB_Set_Lv_Gate error. lv_gate=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AEAWB[ch].AWTDY.bit.AWTDUY	= lv_gate->awtduy;
	IO_PRO.STAT.AEAWB[ch].AWTDR.bit.AWTDUR	= lv_gate->awtdur;
	IO_PRO.STAT.AEAWB[ch].AWTDG.bit.AWTDUG	= lv_gate->awtdug;
	IO_PRO.STAT.AEAWB[ch].AWTDB.bit.AWTDUB	= lv_gate->awtdub;
	IO_PRO.STAT.AEAWB[ch].AWTDY.bit.AWTDLY	= lv_gate->awtdly;
	IO_PRO.STAT.AEAWB[ch].AWTDR.bit.AWTDLR	= lv_gate->awtdlr;
	IO_PRO.STAT.AEAWB[ch].AWTDG.bit.AWTDLG	= lv_gate->awtdlg;
	IO_PRO.STAT.AEAWB[ch].AWTDB.bit.AWTDLB	= lv_gate->awtdlb;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
AWB matrix level gate threshold
@param[in]	mt_lv_gate	:AWB Matrix gate
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AEAWB_Set_Matrix_Lv_Gate( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_MATRIX_LV_GATE* mt_lv_gate )
{
#ifdef CO_PARAM_CHECK
	if (mt_lv_gate == NULL){
		Ddim_Assertion(("I:Im_PRO_AEAWB_Set_Matrix_Lv_Gate error. mt_lv_gate=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AEAWB[ch].AWMLCY.bit.AWMLUCY	= mt_lv_gate->awmlucy;
	IO_PRO.STAT.AEAWB[ch].AWMLCY.bit.AWMLLCY	= mt_lv_gate->awmllcy;
	IO_PRO.STAT.AEAWB[ch].AWMLCB.bit.AWMLUCB	= mt_lv_gate->awmlucb;
	IO_PRO.STAT.AEAWB[ch].AWMLCB.bit.AWMLLCB	= mt_lv_gate->awmllcb;
	IO_PRO.STAT.AEAWB[ch].AWMLCR.bit.AWMLUCR	= mt_lv_gate->awmlucr;
	IO_PRO.STAT.AEAWB[ch].AWMLCR.bit.AWMLLCR	= mt_lv_gate->awmllcr;
	im_pro_set_reg_signed( IO_PRO.STAT.AEAWB[ch].AWMC.AWMC1, union io_awmc_1, AWMC00, mt_lv_gate->awmc[0][0] );
	im_pro_set_reg_signed( IO_PRO.STAT.AEAWB[ch].AWMC.AWMC1, union io_awmc_1, AWMC01, mt_lv_gate->awmc[0][1] );
	im_pro_set_reg_signed( IO_PRO.STAT.AEAWB[ch].AWMC.AWMC2, union io_awmc_2, AWMC02, mt_lv_gate->awmc[0][2] );
	im_pro_set_reg_signed( IO_PRO.STAT.AEAWB[ch].AWMC.AWMC2, union io_awmc_2, AWMC10, mt_lv_gate->awmc[1][0] );
	im_pro_set_reg_signed( IO_PRO.STAT.AEAWB[ch].AWMC.AWMC3, union io_awmc_3, AWMC11, mt_lv_gate->awmc[1][1] );
	im_pro_set_reg_signed( IO_PRO.STAT.AEAWB[ch].AWMC.AWMC3, union io_awmc_3, AWMC12, mt_lv_gate->awmc[1][2] );
	im_pro_set_reg_signed( IO_PRO.STAT.AEAWB[ch].AWMC.AWMC4, union io_awmc_4, AWMC20, mt_lv_gate->awmc[2][0] );
	im_pro_set_reg_signed( IO_PRO.STAT.AEAWB[ch].AWMC.AWMC4, union io_awmc_4, AWMC21, mt_lv_gate->awmc[2][1] );
	im_pro_set_reg_signed( IO_PRO.STAT.AEAWB[ch].AWMC.AWMC5, union io_awmc_5, AWMC22, mt_lv_gate->awmc[2][2] );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
AE Y coefficient
@param[in]	ch : Channel No.
@param[in]	aeyk	:Luminosity generation coefficient Information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AEAWB_Set_AEYK( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_STAT_YK* aeyk )
{
#ifdef CO_PARAM_CHECK
	if (aeyk == NULL){
		Ddim_Assertion(("I:Im_PRO_AEAWB_Set_AEYK error. aeyk=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	im_pro_set_reg_signed( IO_PRO.STAT.AEAWB[ch].AEYKRR, union io_aeykrr, AEYKRR, aeyk->ykr );
	im_pro_set_reg_signed( IO_PRO.STAT.AEAWB[ch].AEYKGR, union io_aeykgr, AEYKGR, aeyk->ykgr );
	im_pro_set_reg_signed( IO_PRO.STAT.AEAWB[ch].AEYKGB, union io_aeykgb, AEYKGB, aeyk->ykgb );
	im_pro_set_reg_signed( IO_PRO.STAT.AEAWB[ch].AEYKBB, union io_aeykbb, AEYKBB, aeyk->ykb );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get Luminance generation coefficient.
@param[in]	ch : Channel No.
@param[out]	aeyk	:Luminosity generation coefficient Information
@retval		Luminance generation coefficient.
*/
INT32 Im_PRO_AEAWB_Get_AEYK( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_STAT_YK* aeyk )
{
#ifdef CO_PARAM_CHECK
	if (aeyk == NULL){
		Ddim_Assertion(("I:Im_PRO_AEAWB_Get_AEYK error. aeyk=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	im_pro_get_reg_signed( aeyk->ykr, IO_PRO.STAT.AEAWB[ch].AEYKRR, union io_aeykrr, AEYKRR );
	im_pro_get_reg_signed( aeyk->ykgr, IO_PRO.STAT.AEAWB[ch].AEYKGR, union io_aeykgr, AEYKGR );
	im_pro_get_reg_signed( aeyk->ykgb, IO_PRO.STAT.AEAWB[ch].AEYKGB, union io_aeykgb, AEYKGB );
	im_pro_get_reg_signed( aeyk->ykb, IO_PRO.STAT.AEAWB[ch].AEYKBB, union io_aeykbb, AEYKBB );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Luminosity is integrated from the specified line in the AEAWB detection Window
@param[in]	line_info
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AEAWB_Set_Line_Data_Position( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_LINE_INFO* line_info )
{
#ifdef CO_PARAM_CHECK
	if (line_info == NULL){
		Ddim_Assertion(("I:Im_PRO_AEAWB_Set_Line_Data_Position error. line_info=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AEAWB[ch].LINEV.LINEV1.bit.LINEV_0	= line_info->start_line[0];
	IO_PRO.STAT.AEAWB[ch].LINEV.LINEV1.bit.LINEV_1	= line_info->start_line[1];
	IO_PRO.STAT.AEAWB[ch].LINEV.LINEV2.bit.LINEV_2	= line_info->start_line[2];
	IO_PRO.STAT.AEAWB[ch].LINEV.LINEV2.bit.LINEV_3	= line_info->start_line[3];
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get flicker result line data. Result is ULONG [4] address.
@retval		line data Result address.
*/
INT32 Im_PRO_AEAWB_Get_Line_Data( E_IM_PRO_AEAWB_CH ch, ULONG* line_data )
{
#ifdef CO_PARAM_CHECK
	if (line_data == NULL){
		Ddim_Assertion(("I:Im_PRO_AEAWB_Get_Line_Data error. line_data=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	line_data[0]	= IO_PRO.STAT.AEAWB[ch].LINEDATA.LINEDATA1.bit.LINEDATA_0;
	line_data[1]	= IO_PRO.STAT.AEAWB[ch].LINEDATA.LINEDATA2.bit.LINEDATA_1;
	line_data[2]	= IO_PRO.STAT.AEAWB[ch].LINEDATA.LINEDATA3.bit.LINEDATA_2;
	line_data[3]	= IO_PRO.STAT.AEAWB[ch].LINEDATA.LINEDATA4.bit.LINEDATA_3;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get AE Data
@retval		AE Data Address	:	AE Data Storing Area Address
*/
ULONG Im_PRO_AEAWB_Get_AEAWBData( E_IM_PRO_AEAWB_CH ch )
{
	return im_pro_comm_get_stat_cur_data_addr( ch );
}

/**
Get AE Data last VD for AFE long exposure
@retval		ULONG AE Data Storing Area Address for AFE long exposure
*/
ULONG Im_PRO_AEAWB_Get_LastAEAWBData( E_IM_PRO_AEAWB_CH ch )
{
	return im_pro_comm_get_stat_last_data_addr( ch );
}

/**
Get Wave Detection Data information
@param[in/out]	wd_info : Wave Detection Data information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AEAWB_Get_WD_Info( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_WD_INFO* wd_info )
{
	ULONG data_addr = 0;
	UCHAR detect_mode = 0;
	UCHAR bit1_num = 0;
	UCHAR bit2_num = 0;
	UCHAR bit3_num = 0;
	UCHAR dummy_size = 0;

#ifdef CO_PARAM_CHECK
	if (wd_info == NULL){
		Ddim_Assertion(("I:Im_PRO_AEAWB_Get_WD_Info error. wd_info=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	// For the representation of the mode with the bit( bit1 => ON )
	// IAWB  AWB  AE
	//   0    0    1  = 1d
	//   0    1    0  = 2d
	//   0    1    1  = 3d
	//   1    0    0  = 4d
	//   1    0    1  = 5d
	//   1    1    0  = 6d
	//   1    1    1  = 7d
	detect_mode = wd_info->mode + 1;

	bit1_num = ( detect_mode & 0x1 );		// AE
	bit2_num = ( detect_mode & 0x2 ) >> 1;	// AWB
	bit3_num = ( detect_mode & 0x4 ) >> 2;	// IAWB

	dummy_size = ( ( wd_info->aehbc % 2) * D_IM_PRO_AEAEB_DUMMY_DATA_SIZE );

	// Calc Global data width
	*wd_info->global_data_width = ( bit1_num * wd_info->aehbc * D_IM_PRO_AEAEB_AE_BLK_SIZE )
								  + ( bit2_num * wd_info->aehbc * D_IM_PRO_AEAEB_AWB_BLK_SIZE )
								  + ( bit3_num * wd_info->aehbc * D_IM_PRO_AEAEB_IAWB_BLK_SIZE )
								  + dummy_size;

	// Calc Address
	data_addr = im_pro_comm_get_stat_cur_data_addr( ch );

	switch( wd_info->type ) {
		case E_IM_PRO_AEAWB_TYPE_AWB:
			{
				data_addr = data_addr + ( bit1_num * wd_info->aehbc * D_IM_PRO_AEAEB_AE_BLK_SIZE )
									  +   dummy_size;
			}
			break;

		case E_IM_PRO_AEAWB_TYPE_LAWB:
			{
				data_addr = data_addr + ( bit1_num * wd_info->aehbc * D_IM_PRO_AEAEB_AE_BLK_SIZE )
									  + ( bit2_num * wd_info->aehbc * D_IM_PRO_AEAEB_AWB_BLK_SIZE )
									  +   dummy_size;
			}
			break;

		case E_IM_PRO_AEAWB_TYPE_AE:
		default:
			{
				// to do nothing
			}
			break;
	}
	*wd_info->address = data_addr;

	return D_DDIM_OK;
}

/**
AF macro start
*/
VOID Im_PRO_AF_Start( E_IM_PRO_AF_CH ch )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AF[ch].AFGBAL.AFTRG.bit.AFTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	im_pro_stat_set_start_status(D_IM_STAT_STATUS_AF_0, ch);
}

/**
AF macro stop
@param[in]	force : force stop option
*/
VOID Im_PRO_AF_Stop( E_IM_PRO_AF_CH ch, UCHAR force )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.STAT.AF[ch].AFGBAL.AFTRG.bit.AFTRG = D_IM_PRO_TRG_FRAME_STOP;	// stop
	}
	else {
		IO_PRO.STAT.AF[ch].AFGBAL.AFTRG.bit.AFTRG = D_IM_PRO_TRG_FORCE_STOP;	// force stop
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	im_pro_stat_set_stop_status(D_IM_STAT_STATUS_AF_0, ch);
}

/**
AF Information Setting
@param[in]	af_ctrl	:	Window Information See @ref T_IM_PRO_AF_CTRL<br>
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_AF_INPUT_PARAM_ERROR	: Setting NG
@code
// sample code
	int i;
	//---- Set AF Information ----
	T_IM_PRO_AF_CTRL af_ctrl;

	af_ctrl.eval_calc0_enable = E_IM_PRO_AF_ENABLE_EN;
	af_ctrl.eval_calc1_enable = E_IM_PRO_AF_ENABLE_EN;
	af_ctrl.eval_calc2_enable = E_IM_PRO_AF_ENABLE_EN;
	af_ctrl.vertical_enable   = E_IM_PRO_AF_ENABLE_EN;
	af_ctrl.horizon0_enable   = E_IM_PRO_AF_ENABLE_EN;
	af_ctrl.horizon1_enable   = E_IM_PRO_AF_ENABLE_EN;
	af_ctrl.horizon2_enable   = E_IM_PRO_AF_ENABLE_EN;
	for (i = E_IM_PRO_AF_WINDOW_1; i < E_IM_PRO_AF_WINDOW_MAX; i ++){
		af_ctrl.enable[i] = E_IM_PRO_AF_ENABLE_EN;
	}

	Im_PRO_AF_Ctrl(&af_ctrl);
@endcode
*/
INT32 Im_PRO_AF_Ctrl( E_IM_PRO_AF_CH ch, T_IM_PRO_AF_CTRL* af_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (af_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_AF_Ctrl error. af_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (af_ctrl->eval_calc0_enable == 0){
		Ddim_Assertion(("I:Im_PRO_AF_Ctrl error. setting 0 to eval_calc0_enable is inhibition.\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (af_ctrl->horizon0_enable == 0){
		Ddim_Assertion(("I:Im_PRO_AF_Ctrl error. setting 0 to horizon0_enable is inhibition.\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( ( IO_PRO.STAT.STATTOP.CLKSTP.bit.PSTP1 == D_IM_PRO_CLOCK_OFF ) || ( IO_PRO.STAT.AF[ch].AFGBAL.AFTRG.bit.AFTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_AF_Ctrl error. macro has not stopped or clock stopped.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENM0	= af_ctrl->eval_calc0_enable;
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENM1	= af_ctrl->eval_calc1_enable;
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENM2	= af_ctrl->eval_calc2_enable;
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENV0	= af_ctrl->vertical_enable;
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENH0	= af_ctrl->horizon0_enable;
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENH1	= af_ctrl->horizon1_enable;
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENH2	= af_ctrl->horizon2_enable;
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW0	= af_ctrl->enable[0];
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW1	= af_ctrl->enable[1];
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW2	= af_ctrl->enable[2];
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW3	= af_ctrl->enable[3];
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW4	= af_ctrl->enable[4];
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW5	= af_ctrl->enable[5];
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW6	= af_ctrl->enable[6];
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW7	= af_ctrl->enable[7];
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW8	= af_ctrl->enable[8];
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW9	= af_ctrl->enable[9];
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW10	= af_ctrl->enable[10];
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW11	= af_ctrl->enable[11];
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW12	= af_ctrl->enable[12];
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW13	= af_ctrl->enable[13];
	IO_PRO.STAT.AF[ch].AFGBAL.AFEBEN.bit.ENW14	= af_ctrl->enable[14];
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of AF.
@param[in]	paen_trg : RAM access control<br>
			value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AF_Set_PAEN( E_IM_PRO_AF_CH ch, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.STAT.AF[ch].AFGBAL.AFTRG.bit.AFTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_AF_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AF[ch].AFGBAL.AFPAEN.bit.AFPAEN	= paen_trg;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock
	return D_DDIM_OK;
}

/**
Set AF Global Window 
@param[in]	global_win :AF global area information<br>
							value range :pos_x[0 - 8190](Multiples of 2)<br>
							target registor :@@AFGH<br>
							value range :pos_y[0 - 8190](Multiples of 2)<br>
							target registor :@@AFGV<br>
							value range :width[2 - 4480] 2pixel boundary<br>
							target registor :@@AFGHW<br>
							value range :lines[2 - 8192] 2pixel boundary<br>
							target registor :@@AFGVW<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@attention	None
*/
INT32 Im_PRO_AF_Set_Global_Area( E_IM_PRO_AF_CH ch, T_IM_PRO_AREA_INFO* global_win )
{
#ifdef CO_PARAM_CHECK
	if (global_win == NULL){
		Ddim_Assertion(("I:Im_PRO_AF_Set_Global_Area error. global_win=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AF_AFGH_MIN, D_IM_PRO_AF_AFGH_MAX, global_win->pos_x, "Im_PRO_AF_Set_Global_Area : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AF_AFGV_MIN, D_IM_PRO_AF_AFGV_MAX, global_win->pos_y, "Im_PRO_AF_Set_Global_Area : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AF_AFGHW_MIN, D_IM_PRO_AF_AFGHW_MAX, global_win->width, "Im_PRO_AF_Set_Global_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AF_AFGVW_MIN, D_IM_PRO_AF_AFGVW_MAX, global_win->lines, "Im_PRO_AF_Set_Global_Area : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AF[ch].AFGBAL.AFGH.bit.AFGH		= global_win->pos_x;
	IO_PRO.STAT.AF[ch].AFGBAL.AFGV.bit.AFGV		= global_win->pos_y;
	IO_PRO.STAT.AF[ch].AFGBAL.AFGHW.bit.AFGHW	= global_win->width;
	IO_PRO.STAT.AF[ch].AFGBAL.AFGVW.bit.AFGVW	= global_win->lines;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock
	return D_DDIM_OK;
}

/**
AF Surface color Information
@param[in]	unit_no	:				Evaluated value calculation unit number. See @ref E_IM_PRO_AF_EVAL_CALC_UNIT<br>
@param[in]	af_eval_calc_ctrl	:	Evaluated value calculation unit Information. See @ref T_IM_PRO_AF_EVAL_CALC_CTRL<br>
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_AF_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AF_Ctrl_Eval_Calc( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_EVAL_CALC_UNIT unit_no, T_IM_PRO_AF_EVAL_CALC_CTRL* af_eval_calc_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (af_eval_calc_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_AF_Ctrl_Eval_Calc error. af_eval_calc_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFMTRXRR, union io_afmtrxrr, AFMTRXRR, af_eval_calc_ctrl->r );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFMTRXGR, union io_afmtrxgr, AFMTRXGR, af_eval_calc_ctrl->gr );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFMTRXGB, union io_afmtrxgb, AFMTRXGB, af_eval_calc_ctrl->gb );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFMTRXBB, union io_afmtrxbb, AFMTRXBB, af_eval_calc_ctrl->b );
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFMDEN.bit.AFMDEN				= af_eval_calc_ctrl->median_enable;
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNEN.bit.AFKNEN				= af_eval_calc_ctrl->knee_enable;
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNB.AFKNB1.bit.AFKNB_1			= af_eval_calc_ctrl->knee_boundary[0];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNB.AFKNB1.bit.AFKNB_2			= af_eval_calc_ctrl->knee_boundary[1];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNB.AFKNB2.bit.AFKNB_3			= af_eval_calc_ctrl->knee_boundary[2];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNB.AFKNB2.bit.AFKNB_4			= af_eval_calc_ctrl->knee_boundary[3];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNB.AFKNB3.bit.AFKNB_5			= af_eval_calc_ctrl->knee_boundary[4];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNB.AFKNB3.bit.AFKNB_6			= af_eval_calc_ctrl->knee_boundary[5];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNB.AFKNB4.bit.AFKNB_7			= af_eval_calc_ctrl->knee_boundary[6];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNINC.AFKNINC1.bit.AFKNINC_0	= af_eval_calc_ctrl->knee_slope[0];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNINC.AFKNINC1.bit.AFKNINC_1	= af_eval_calc_ctrl->knee_slope[1];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNINC.AFKNINC2.bit.AFKNINC_2	= af_eval_calc_ctrl->knee_slope[2];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNINC.AFKNINC2.bit.AFKNINC_3	= af_eval_calc_ctrl->knee_slope[3];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNINC.AFKNINC3.bit.AFKNINC_4	= af_eval_calc_ctrl->knee_slope[4];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNINC.AFKNINC3.bit.AFKNINC_5	= af_eval_calc_ctrl->knee_slope[5];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNINC.AFKNINC4.bit.AFKNINC_6	= af_eval_calc_ctrl->knee_slope[6];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNINC.AFKNINC4.bit.AFKNINC_7	= af_eval_calc_ctrl->knee_slope[7];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNOFS.AFKNOFS1.bit.AFKNOFS_0	= af_eval_calc_ctrl->knee_offset[0];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNOFS.AFKNOFS1.bit.AFKNOFS_1	= af_eval_calc_ctrl->knee_offset[1];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNOFS.AFKNOFS2.bit.AFKNOFS_2	= af_eval_calc_ctrl->knee_offset[2];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNOFS.AFKNOFS2.bit.AFKNOFS_3	= af_eval_calc_ctrl->knee_offset[3];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNOFS.AFKNOFS3.bit.AFKNOFS_4	= af_eval_calc_ctrl->knee_offset[4];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNOFS.AFKNOFS3.bit.AFKNOFS_5	= af_eval_calc_ctrl->knee_offset[5];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNOFS.AFKNOFS4.bit.AFKNOFS_6	= af_eval_calc_ctrl->knee_offset[6];
	IO_PRO.STAT.AF[ch].AFMAIN[unit_no].AFKNOFS.AFKNOFS4.bit.AFKNOFS_7	= af_eval_calc_ctrl->knee_offset[7];
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
AF Vertical detection Information
@param[in]	af_vertical_ctrl	:	Vertical detection Information See @ref T_IM_PRO_AF_VERTICAL_CTRL<br>
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_AF_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AF_Ctrl_Vertical( E_IM_PRO_AF_CH ch, T_IM_PRO_AF_VERTICAL_CTRL* af_vertical_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (af_vertical_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_AF_Ctrl_Vertical error. af_vertical_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AF_AFVSH_MIN, D_IM_PRO_AF_AFVSH_MAX, af_vertical_ctrl->start_addr, "Im_PRO_AF_Ctrl_Vertical : start_addr" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AF_AFVSHW_MIN, D_IM_PRO_AF_AFVSHW_MAX, af_vertical_ctrl->horizon_size, "Im_PRO_AF_Ctrl_Vertical : horizon_size" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AF[ch].AFVS.AFVSCTL.bit.AFVSCYC				= af_vertical_ctrl->update_cycle;
	IO_PRO.STAT.AF[ch].AFVS.AFVSCTL.bit.AFSLV				= af_vertical_ctrl->eval_cal_unit;
	IO_PRO.STAT.AF[ch].AFVS.AFVSH.bit.AFVSH					= af_vertical_ctrl->start_addr;
	IO_PRO.STAT.AF[ch].AFVS.AFVSHW.bit.AFVSHW				= af_vertical_ctrl->horizon_size;
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFVS.AFIIRVB.AFIIRVB1, union io_afiirvb_1, AFIIRVK, af_vertical_ctrl->iir_k );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFVS.AFIIRVB.AFIIRVB1, union io_afiirvb_1, AFIIRVA0, af_vertical_ctrl->iir_a0 );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFVS.AFIIRVB.AFIIRVB1, union io_afiirvb_1, AFIIRVA1, af_vertical_ctrl->iir_a1 );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFVS.AFIIRVB.AFIIRVB2, union io_afiirvb_2, AFIIRVA2, af_vertical_ctrl->iir_a2 );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFVS.AFIIRVB.AFIIRVB2, union io_afiirvb_2, AFIIRVB1, af_vertical_ctrl->iir_b1 );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFVS.AFIIRVB.AFIIRVB3, union io_afiirvb_3, AFIIRVB2, af_vertical_ctrl->iir_b2 );
	IO_PRO.STAT.AF[ch].AFVS.AFCOR2V.bit.AFCOR2LVV			= af_vertical_ctrl->cor_threshold;
	IO_PRO.STAT.AF[ch].AFVS.AFCOR2V.bit.AFCOR2V				= af_vertical_ctrl->cor_level;
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFVS.AFFIRVB, union io_affirvb, AFFIRVB1, af_vertical_ctrl->fir_filter[0] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFVS.AFFIRVB, union io_affirvb, AFFIRVB2, af_vertical_ctrl->fir_filter[1] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFVS.AFFIRVB, union io_affirvb, AFFIRVB3, af_vertical_ctrl->fir_filter[2] );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
AF Horizontal detection Information
@param[in]	tap_no	:			Horizontal detection TAP number See @ref E_IM_PRO_AF_TAP<br>
@param[in]	af_horizon_ctrl	:	Horizontal detection Information See @ref T_IM_PRO_AF_HORIZON_CTRL<br>
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_AF_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_AF_Ctrl_Horizon( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_TAP tap_no, T_IM_PRO_AF_HORIZON_CTRL* af_horizon_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (af_horizon_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_AF_Ctrl_Horizon error. af_horizon_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFHSCTLA.bit.AFIHA						= af_horizon_ctrl->group_a.iir_enable;
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFHSCTLA.bit.AFFHA						= af_horizon_ctrl->group_a.fir_enable;
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFHSCTLA.bit.AFSLHA						= af_horizon_ctrl->group_a.input_select;
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFFIRHBA.AFFIRHBA1, union io_affirhba_1, AFFIRHBA_1, af_horizon_ctrl->group_a.fir_coefficient[0] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFFIRHBA.AFFIRHBA1, union io_affirhba_1, AFFIRHBA_2, af_horizon_ctrl->group_a.fir_coefficient[1] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFFIRHBA.AFFIRHBA1, union io_affirhba_1, AFFIRHBA_3, af_horizon_ctrl->group_a.fir_coefficient[2] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFFIRHBA.AFFIRHBA1, union io_affirhba_1, AFFIRHBA_4, af_horizon_ctrl->group_a.fir_coefficient[3] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFFIRHBA.AFFIRHBA2, union io_affirhba_2, AFFIRHBA_5, af_horizon_ctrl->group_a.fir_coefficient[4] );
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCOR1HA.bit.AFCOR1HA					= af_horizon_ctrl->group_a.fir_level;
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFGAINHA.bit.AFGAINHA					= af_horizon_ctrl->group_a.gain;
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFIIRHBA.AFIIRHBA1, union io_afiirhba_1, AFIIRHKA,  af_horizon_ctrl->group_a.iir_k );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFIIRHBA.AFIIRHBA1, union io_afiirhba_1, AFIIRHA0A, af_horizon_ctrl->group_a.iir_a0 );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFIIRHBA.AFIIRHBA1, union io_afiirhba_1, AFIIRHA1A, af_horizon_ctrl->group_a.iir_a1 );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFIIRHBA.AFIIRHBA2, union io_afiirhba_2, AFIIRHA2A, af_horizon_ctrl->group_a.iir_a2 );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFIIRHBA.AFIIRHBA2, union io_afiirhba_2, AFIIRHB1A, af_horizon_ctrl->group_a.iir_b1 );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFIIRHBA.AFIIRHBA3, union io_afiirhba_3, AFIIRHB2A, af_horizon_ctrl->group_a.iir_b2 );
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCORBHA.AFCORBHA1.bit.AFCORBHA_1		= af_horizon_ctrl->group_a.coring_bounder[0];
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCORBHA.AFCORBHA1.bit.AFCORBHA_2		= af_horizon_ctrl->group_a.coring_bounder[1];
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCORBHA.AFCORBHA2.bit.AFCORBHA_3		= af_horizon_ctrl->group_a.coring_bounder[2];
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCORINCHA.AFCORINCHA1, union io_afcorincha_1, AFCORINCHA_0,  af_horizon_ctrl->group_a.coring_slope[0] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCORINCHA.AFCORINCHA1, union io_afcorincha_1, AFCORINCHA_1,  af_horizon_ctrl->group_a.coring_slope[1] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCORINCHA.AFCORINCHA2, union io_afcorincha_2, AFCORINCHA_2,  af_horizon_ctrl->group_a.coring_slope[2] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCORINCHA.AFCORINCHA2, union io_afcorincha_2, AFCORINCHA_3,  af_horizon_ctrl->group_a.coring_slope[3] );
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCOROFSHA.AFCOROFSHA1.bit.AFCOROFSHA_0	= af_horizon_ctrl->group_a.coring_offset[0];
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCOROFSHA.AFCOROFSHA1.bit.AFCOROFSHA_1	= af_horizon_ctrl->group_a.coring_offset[1];
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCOROFSHA.AFCOROFSHA2.bit.AFCOROFSHA_2	= af_horizon_ctrl->group_a.coring_offset[2];
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCOROFSHA.AFCOROFSHA2.bit.AFCOROFSHA_3	= af_horizon_ctrl->group_a.coring_offset[3];

	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFHSCTLB.bit.AFIHB						= af_horizon_ctrl->group_b.iir_enable;
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFHSCTLB.bit.AFFHB						= af_horizon_ctrl->group_b.fir_enable;
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFHSCTLB.bit.AFSLHB						= af_horizon_ctrl->group_b.input_select;
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFFIRHBB.AFFIRHBB1, union io_affirhbb_1, AFFIRHBB_1, af_horizon_ctrl->group_b.fir_coefficient[0] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFFIRHBB.AFFIRHBB1, union io_affirhbb_1, AFFIRHBB_2, af_horizon_ctrl->group_b.fir_coefficient[1] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFFIRHBB.AFFIRHBB1, union io_affirhbb_1, AFFIRHBB_3, af_horizon_ctrl->group_b.fir_coefficient[2] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFFIRHBB.AFFIRHBB1, union io_affirhbb_1, AFFIRHBB_4, af_horizon_ctrl->group_b.fir_coefficient[3] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFFIRHBB.AFFIRHBB2, union io_affirhbb_2, AFFIRHBB_5, af_horizon_ctrl->group_b.fir_coefficient[4] );
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCOR1HB.bit.AFCOR1HB					= af_horizon_ctrl->group_b.fir_level;
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFGAINHB.bit.AFGAINHB					= af_horizon_ctrl->group_b.gain;
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFIIRHBB.AFIIRHBB1, union io_afiirhbb_1, AFIIRHKB,   af_horizon_ctrl->group_b.iir_k );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFIIRHBB.AFIIRHBB1, union io_afiirhbb_1, AFIIRHA0B,  af_horizon_ctrl->group_b.iir_a0 );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFIIRHBB.AFIIRHBB1, union io_afiirhbb_1, AFIIRHA1B,  af_horizon_ctrl->group_b.iir_a1 );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFIIRHBB.AFIIRHBB2, union io_afiirhbb_2, AFIIRHA2B,  af_horizon_ctrl->group_b.iir_a2 );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFIIRHBB.AFIIRHBB2, union io_afiirhbb_2, AFIIRHB1B,  af_horizon_ctrl->group_b.iir_b1 );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFIIRHBB.AFIIRHBB3, union io_afiirhbb_3, AFIIRHB2B,  af_horizon_ctrl->group_b.iir_b2 );
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCORBHB.AFCORBHB1.bit.AFCORBHB_1		= af_horizon_ctrl->group_b.coring_bounder[0];
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCORBHB.AFCORBHB1.bit.AFCORBHB_2		= af_horizon_ctrl->group_b.coring_bounder[1];
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCORBHB.AFCORBHB2.bit.AFCORBHB_3		= af_horizon_ctrl->group_b.coring_bounder[2];
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCORINCHB.AFCORINCHB1, union io_afcorinchb_1, AFCORINCHB_0,  af_horizon_ctrl->group_b.coring_slope[0] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCORINCHB.AFCORINCHB1, union io_afcorinchb_1, AFCORINCHB_1,  af_horizon_ctrl->group_b.coring_slope[1] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCORINCHB.AFCORINCHB2, union io_afcorinchb_2, AFCORINCHB_2,  af_horizon_ctrl->group_b.coring_slope[2] );
	im_pro_set_reg_signed( IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCORINCHB.AFCORINCHB2, union io_afcorinchb_2, AFCORINCHB_3,  af_horizon_ctrl->group_b.coring_slope[3] );
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCOROFSHB.AFCOROFSHB1.bit.AFCOROFSHB_0	= af_horizon_ctrl->group_b.coring_offset[0];
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCOROFSHB.AFCOROFSHB1.bit.AFCOROFSHB_1	= af_horizon_ctrl->group_b.coring_offset[1];
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCOROFSHB.AFCOROFSHB2.bit.AFCOROFSHB_2	= af_horizon_ctrl->group_b.coring_offset[2];
	IO_PRO.STAT.AF[ch].AFHS[tap_no].AFCOROFSHB.AFCOROFSHB2.bit.AFCOROFSHB_3	= af_horizon_ctrl->group_b.coring_offset[3];
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
AF Window Information Setting
@param[in]	win_no		:	Window No. See @ref E_IM_PRO_AF_WINDOW_NO
@param[in]	af_win_ctrl	:	Window Information See @ref T_IM_PRO_AF_WIN_CTRL<br>
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_AF_INPUT_PARAM_ERROR	: Setting NG
@code
// sample code
	//---- Set AF Window Information ----
	T_IM_PRO_AF_WIN_CTRL af_win_ctrl;

	af_win_ctrl.h_sel0           = E_IM_PRO_AF_WIN_H0_A;
	af_win_ctrl.h_sel1           = E_IM_PRO_AF_WIN_H0_A;
	af_win_ctrl.eval_cal_unit    = E_IM_PRO_AF_WIN_C0;
	af_win_ctrl.bright_threshold = 0xFFFF;
	af_win_ctrl.dark_threshold   = 0x0000;
	af_win_ctrl.start_pos_x      = 64;
	af_win_ctrl.start_pos_y      = 128;
	af_win_ctrl.width            = 50;
	af_win_ctrl.height           = 50;

	Im_PRO_AF_Ctrl_Win(E_IM_PRO_AF_WINDOW_1, &af_ctrl);
@endcode
*/
INT32 Im_PRO_AF_Ctrl_Win( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no, T_IM_PRO_AF_WIN_CTRL* af_win_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (af_win_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_AF_Ctrl_Win error. af_win_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AF_AFH_MIN, D_IM_PRO_AF_AFH_MAX, af_win_ctrl->start_pos_x, "Im_PRO_AF_Set_Area : start_pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AF_AFV_MIN, D_IM_PRO_AF_AFV_MAX, af_win_ctrl->start_pos_y, "Im_PRO_AF_Set_Area : start_pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AF_AFHW_MIN, D_IM_PRO_AF_AFHW_MAX, af_win_ctrl->width, "Im_PRO_AF_Set_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_AF_AFVW_MIN, D_IM_PRO_AF_AFVW_MAX, af_win_ctrl->height, "Im_PRO_AF_Set_Area : height" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.AF[ch].AFWIN[win_no].AFWINCTL.bit.AFSLWH0	= af_win_ctrl->h_sel0;
	IO_PRO.STAT.AF[ch].AFWIN[win_no].AFWINCTL.bit.AFSLWH1	= af_win_ctrl->h_sel1;
	IO_PRO.STAT.AF[ch].AFWIN[win_no].AFWINCTL.bit.AFSLWH2	= af_win_ctrl->h_sel2;
	IO_PRO.STAT.AF[ch].AFWIN[win_no].AFWINCTL.bit.AFSLWC	= af_win_ctrl->eval_cal_unit;
	IO_PRO.STAT.AF[ch].AFWIN[win_no].AFCLVGE.bit.AFCLVGE	= af_win_ctrl->bright_threshold;
	IO_PRO.STAT.AF[ch].AFWIN[win_no].AFCLVL.bit.AFCLVL		= af_win_ctrl->dark_threshold;
	IO_PRO.STAT.AF[ch].AFWIN[win_no].AFH.bit.AFH			= af_win_ctrl->start_pos_x;
	IO_PRO.STAT.AF[ch].AFWIN[win_no].AFV.bit.AFV			= af_win_ctrl->start_pos_y;
	IO_PRO.STAT.AF[ch].AFWIN[win_no].AFHW.bit.AFHW			= af_win_ctrl->width;
	IO_PRO.STAT.AF[ch].AFWIN[win_no].AFVW.bit.AFVW			= af_win_ctrl->height;
	IO_PRO.STAT.AF[ch].AFWIN[win_no].AFSLWM.bit.AFSLWM		= af_win_ctrl->max_mix_select;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get AF Surface Data
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* brightness data address pointer
@remarks	The brightness data is acquired. 
@code
// SAMPLE CODE //
	ULONG* win1_ydata;
	
	win1_ydata = Im_PRO_AF_Get_Eval_Calc_Data(E_IM_PRO_AF_WINDOW_1);	// Window 1 Ydata Top Address Get
@endcode
*/
ULONG Im_PRO_AF_Get_Eval_Calc_Data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_EVAL_CALC, E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT );
}

/**
Get AF Surface Bright block Data
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* brightness data address pointer
@remarks	The brightness data is acquired. 
@code
// SAMPLE CODE //
	ULONG* win1_ydata;
	
	win1_ydata = Im_PRO_AF_Get_Bright_Data(E_IM_PRO_AF_WINDOW_1);	// Window 1 Ydata Top Address Get
@endcode
*/
ULONG Im_PRO_AF_Get_Bright_Data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_EVAL_BRIGHT, E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT );
}

/**
Get AF Surface Dark block Data
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* brightness data address pointer
@remarks	The brightness data is acquired. 
*/
ULONG Im_PRO_AF_Get_Dark_Data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_EVAL_DARK, E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT );
}

/**
Get AF Horizontal 0 Data
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* brightness data address pointer
@remarks	The AF Horizontal 0 data is acquired. 
*/
ULONG Im_PRO_AF_Get_Horizontal0_Data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL0, E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT );
}

/**
Get AF Horizontal 1 Data
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* brightness data address pointer
@remarks	The AF Horizontal 1 data is acquired. 
*/
ULONG Im_PRO_AF_Get_Horizontal1_Data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL1, E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT );
}

/**
Get AF Horizontal 2 Data
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* brightness data address pointer
@remarks	The AF Horizontal 2 data is acquired. 
*/
ULONG Im_PRO_AF_Get_Horizontal2_Data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL2, E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT );
}

/**
Get AF Vertical Data
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* brightness data address pointer
@remarks	The AF Vertical data is acquired. 
*/
ULONG Im_PRO_AF_Get_Vertical_Data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_VERTICAL, E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT );
}

/**
Get As a result maximum evaluated calculation value AF detection window
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum color value is acquired. 
*/
ULONG Im_PRO_AF_Get_Max_Eval_Calc_Value( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_EVALUATED_VALUE_MAX, E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT );
}

/**
Get As a result minimum evaluated calculation value AF detection window
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The minimum color value is acquired. 
*/
ULONG Im_PRO_AF_Get_Min_Eval_Calc_Value( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_EVALUATED_VALUE_MIN, E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT );
}

/**
Get As a result maximum assessment value AF detection window
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum assessment value is acquired. 
*/
ULONG Im_PRO_AF_Get_Max_Assess_Value( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_EVALUATION_VALUE_MAX, E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT );
}

/**
Get As a result minimum assessment value AF detection window
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The minimum assessment value is acquired. 
*/
ULONG Im_PRO_AF_Get_Min_Assess_Value( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_EVALUATION_VALUE_MIN, E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT );
}

/**
Get AF Detection Window Horizontal0 Detection Each Line Maximum Value Accumulation result.
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum assessment value is acquired. 
*/
ULONG Im_PRO_AF_Get_Horizontal0_Line_Accum_Max_Value( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL_0_EACH_LINE_MAX, E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT );
}

/**
Get AF Detection Window Horizontal1 Detection Each Line Maximum Value Accumulation result.
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum assessment value is acquired. 
*/
ULONG Im_PRO_AF_Get_Horizontal1_Line_Accum_Max_Value( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL_1_EACH_LINE_MAX, E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT );
}

/**
Get AF Detection Window Horizontal2 Detection Each Line Maximum Value Accumulation result.
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum assessment value is acquired. 
*/
ULONG Im_PRO_AF_Get_Horizontal2_Line_Accum_Max_Value( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL_2_EACH_LINE_MAX, E_IM_PRO_AF_RESULT_DATA_TYPE_CURRENT );
}

/**
Get AF evaluated calculation Data for AFE long exposure
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* brightness data address pointer
@remarks	The brightness data is acquired. 
@code
// SAMPLE CODE //
	ULONG* win1_ydata;
	
	win1_ydata = Im_PRO_AF_Get_Eval_Calc_LastData(E_IM_PRO_AF_WINDOW_1);	// Window 1 Ydata Top Address Get
@endcode
*/
ULONG Im_PRO_AF_Get_Eval_Calc_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_EVAL_CALC, E_IM_PRO_AF_RESULT_DATA_TYPE_LAST );
}

/**
Get AF evaluated calculation Bright block Data for AFE long exposure
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* brightness data address pointer
@remarks	The brightness data is acquired. 
@code
// SAMPLE CODE //
	ULONG* win1_ydata;
	
	win1_ydata = Im_PRO_AF_Get_Bright_LastData(E_IM_PRO_AF_WINDOW_1);	// Window 1 Ydata Top Address Get
@endcode
*/
ULONG Im_PRO_AF_Get_Bright_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_EVAL_BRIGHT, E_IM_PRO_AF_RESULT_DATA_TYPE_LAST );
}

/**
Get AF evaluated calculation Dark block Data for AFE long exposure
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* brightness data address pointer
@remarks	The brightness data is acquired. 
@code
// SAMPLE CODE //
	ULONG* win1_ydata;
	
	win1_ydata = Im_PRO_AF_Get_Dark_LastData(E_IM_PRO_AF_WINDOW_1);	// Window 1 Ydata Top Address Get
@endcode
*/
ULONG Im_PRO_AF_Get_Dark_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_EVAL_DARK, E_IM_PRO_AF_RESULT_DATA_TYPE_LAST );
}

/**
Get AF Horizontal 0 Data for AFE long exposure
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* brightness data address pointer
@remarks	The brightness data is acquired. 
@code
// SAMPLE CODE //
	ULONG* win1_ydata;
	
	win1_ydata = Im_PRO_AF_Get_Horizontal0_LastData(E_IM_PRO_AF_WINDOW_1);	// Window 1 Ydata Top Address Get
@endcode
*/
ULONG Im_PRO_AF_Get_Horizontal0_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL0, E_IM_PRO_AF_RESULT_DATA_TYPE_LAST );
}

/**
Get AF Horizontal 1 Data for AFE long exposure
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* brightness data address pointer
@remarks	The brightness data is acquired. 
@code
// SAMPLE CODE //
	ULONG* win1_ydata;
	
	win1_ydata = Im_PRO_AF_Get_Horizontal1_LastData(E_IM_PRO_AF_WINDOW_1);	// Window 1 Ydata Top Address Get
@endcode
*/
ULONG Im_PRO_AF_Get_Horizontal1_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL1, E_IM_PRO_AF_RESULT_DATA_TYPE_LAST );
}

/**
Get AF Horizontal 2 Data for AFE long exposure
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* brightness data address pointer
@remarks	The brightness data is acquired. 
@code
// SAMPLE CODE //
	ULONG* win1_ydata;
	
	win1_ydata = Im_PRO_AF_Get_Horizontal2_LastData(E_IM_PRO_AF_WINDOW_1);	// Window 1 Ydata Top Address Get
@endcode
*/
ULONG Im_PRO_AF_Get_Horizontal2_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL2, E_IM_PRO_AF_RESULT_DATA_TYPE_LAST );
}

/**
Get AF Vertical Data for AFE long exposure
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* brightness data address pointer
@remarks	The brightness data is acquired. 
@code
// SAMPLE CODE //
	ULONG* win1_ydata;
	
	win1_ydata = Im_PRO_AF_Get_Vertical_LastData(E_IM_PRO_AF_WINDOW_1);	// Window 1 Ydata Top Address Get
@endcode
*/
ULONG Im_PRO_AF_Get_Vertical_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_VERTICAL, E_IM_PRO_AF_RESULT_DATA_TYPE_LAST );
}

/**
Get As a result maximum evaluated calculation value AF detection window for AFE long exposure
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum color value is acquired. 
*/
ULONG Im_PRO_AF_Get_Max_Eval_Calc_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_EVALUATED_VALUE_MAX, E_IM_PRO_AF_RESULT_DATA_TYPE_LAST );
}

/**
Get As a result minimum evaluated calculation value AF detection window for AFE long exposure
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The minimum color value is acquired. 
*/
ULONG Im_PRO_AF_Get_Min_Eval_Calc_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_EVALUATED_VALUE_MIN, E_IM_PRO_AF_RESULT_DATA_TYPE_LAST );
}

/**
Get As a result maximum assessment value AF detection window for AFE long exposure
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum assessment value is acquired. 
*/
ULONG Im_PRO_AF_Get_Max_Assess_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_EVALUATION_VALUE_MAX, E_IM_PRO_AF_RESULT_DATA_TYPE_LAST );
}

/**
Get As a result minimum assessment value AF detection window for AFE long exposure
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The minimum assessment value is acquired. 
*/
ULONG Im_PRO_AF_Get_Min_Assess_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_EVALUATION_VALUE_MIN, E_IM_PRO_AF_RESULT_DATA_TYPE_LAST );
}

/**
Get AF Detection Window Horizontal0 Detection Each Line Maximum Value Accumulation result for AFE long exposure.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum assessment value is acquired. 
*/
ULONG Im_PRO_AF_Get_Horizontal0_Line_Accum_Max_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL_0_EACH_LINE_MAX, E_IM_PRO_AF_RESULT_DATA_TYPE_LAST );
}

/**
Get AF Detection Window Horizontal1 Detection Each Line Maximum Value Accumulation result for AFE long exposure.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum assessment value is acquired. 
*/
ULONG Im_PRO_AF_Get_Horizontal1_Line_Accum_Max_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL_1_EACH_LINE_MAX, E_IM_PRO_AF_RESULT_DATA_TYPE_LAST );
}

/**
Get AF Detection Window Horizontal2 Detection Each Line Maximum Value Accumulation result for AFE long exposure.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum assessment value is acquired. 
*/
ULONG Im_PRO_AF_Get_Horizontal2_Line_Accum_Max_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no )
{
	return im_pro_comm_get_af_data( ch, win_no, E_IM_PRO_AF_RESULT_TYPE_HORIZONTAL_2_EACH_LINE_MAX, E_IM_PRO_AF_RESULT_DATA_TYPE_LAST );
}

/**
HIST macro start
@param[in]	ch : Channel No.
@retval		D_DDIM_OK				: Processing OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PWCH not running NG
*/
INT32 Im_PRO_HIST_Start( E_IM_PRO_HIST_CH ch )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.HIST[ch].HISTTRG.bit.HISTTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	im_pro_stat_set_start_status(D_IM_STAT_STATUS_HIST_0, ch);

	return D_DDIM_OK;
}

/**
HIST macro stop
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: PWCH not stopped NG
*/
INT32 Im_PRO_HIST_Stop( E_IM_PRO_HIST_CH ch, UCHAR force )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.STAT.HIST[ch].HISTTRG.bit.HISTTRG = D_IM_PRO_TRG_FRAME_STOP;	// stop
	}
	else {
		IO_PRO.STAT.HIST[ch].HISTTRG.bit.HISTTRG = D_IM_PRO_TRG_FORCE_STOP;	// force stop
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	im_pro_stat_set_stop_status(D_IM_STAT_STATUS_HIST_0, ch);

	return D_DDIM_OK;
}

/**
HIST detecting information set
@param[in]	ch : Channel No.
@param[in]	hist_ctrl : HIST Information @ref T_IM_PRO_HIST_CTRL
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_HIST_Ctrl( E_IM_PRO_HIST_CH ch, T_IM_PRO_HIST_CTRL* hist_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (hist_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_HIST_Ctrl error. hist_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.HIST[ch].HISTCTL.bit.HISTCT		= hist_ctrl->hist_mode;
	IO_PRO.STAT.HIST[ch].HISTCTL.bit.HISTHCYC	= hist_ctrl->hist_cycle;
	IO_PRO.STAT.HIST[ch].HISTCTL.bit.HISTSUMS	= hist_ctrl->hist_shift_val;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of HIST.
@param[in]	ch : Channel No.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_HIST_Set_PAEN( E_IM_PRO_HIST_CH ch, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.STAT.HIST[ch].HISTTRG.bit.HISTTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_HIST_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.HIST[ch].HISTPAEN.bit.PAEN	= paen_trg;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
HIST detecting mode set
@param[in]	ch : Channel No.
@param[in]	hist_cycle : HIST horizontal subsampling cycle. @ref E_IM_PRO_HIST_CYCLE
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_HIST_Set_H_Cycle( E_IM_PRO_HIST_CH ch, E_IM_PRO_HIST_CYCLE hist_cycle )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.HIST[ch].HISTCTL.bit.HISTHCYC	= hist_cycle;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set HIST Window size
@param[in]	ch : Channel No.
@param[in]	hist_area : HIST window information<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_HIST_Set_Area( E_IM_PRO_HIST_CH ch, T_IM_PRO_HIST_AREA* hist_area )
{
#ifdef CO_PARAM_CHECK
	if (hist_area == NULL){
		Ddim_Assertion(("I:Im_PRO_HIST_Set_Area error. hist_area=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_HIST_HISTH_MIN, D_IM_PRO_HIST_HISTH_MAX, hist_area->pos_x, "Im_PRO_HIST_Set_Area : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_HIST_HISTV_MIN, D_IM_PRO_HIST_HISTV_MAX, hist_area->pos_y, "Im_PRO_HIST_Set_Area : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_HIST_HISTHW_MIN, D_IM_PRO_HIST_HISTHW_MAX, hist_area->width, "Im_PRO_HIST_Set_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_HIST_HISTVW_MIN, D_IM_PRO_HIST_HISTVW_MAX, hist_area->lines, "Im_PRO_HIST_Set_Area : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.HIST[ch].HISTH.bit.HISTH	= hist_area->pos_x;
	IO_PRO.STAT.HIST[ch].HISTV.bit.HISTV	= hist_area->pos_y;
	IO_PRO.STAT.HIST[ch].HISTHW.bit.HISTHW	= hist_area->width;
	IO_PRO.STAT.HIST[ch].HISTVW.bit.HISTVW	= hist_area->lines;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set HIST Window size
@param[in]	ch : Channel No.
@param[in]	hist_gain : HIST window information<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_HIST_Set_Gain( E_IM_PRO_HIST_CH ch, T_IM_PRO_HIST_GAIN* hist_gain )
{
#ifdef CO_PARAM_CHECK
	if (hist_gain == NULL){
		Ddim_Assertion(("I:Im_PRO_HIST_Set_Gain error. hist_gain=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.HIST[ch].HISTWBGR.bit.HISTWBGR	= hist_gain->r_gain;
	IO_PRO.STAT.HIST[ch].HISTWBGG.bit.HISTWBGG	= hist_gain->g_gain;
	IO_PRO.STAT.HIST[ch].HISTWBGB.bit.HISTWBGB	= hist_gain->b_gain;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
HIST Y coefficient
@param[in]	ch : Channel No.
@param[in]	histyk	:Luminosity generation coefficient Information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_HIST_Set_HISTYK( E_IM_PRO_HIST_CH ch, T_IM_PRO_STAT_YK* histyk )
{
#ifdef CO_PARAM_CHECK
	if (histyk == NULL){
		Ddim_Assertion(("I:Im_PRO_HIST_Set_HISTYK error. histyk=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	im_pro_set_reg_signed( IO_PRO.STAT.HIST[ch].HISTYKRR, union io_histykrr, HISTYKRR, histyk->ykr );
	im_pro_set_reg_signed( IO_PRO.STAT.HIST[ch].HISTYKGR, union io_histykgr, HISTYKGR, histyk->ykgr );
	im_pro_set_reg_signed( IO_PRO.STAT.HIST[ch].HISTYKGB, union io_histykgb, HISTYKGB, histyk->ykgb );
	im_pro_set_reg_signed( IO_PRO.STAT.HIST[ch].HISTYKBB, union io_histykbb, HISTYKBB, histyk->ykb );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get HIST result Data
@param[in]	ch : Channel No.
@param[out]	hist_data :Histgram result value inside the HIST window.
*/
INT32 Im_PRO_HIST_Get_HIST_Data( E_IM_PRO_HIST_CH ch, T_IM_PRO_HIST_DATA* hist_data )
{
#ifdef CO_PARAM_CHECK
	if (hist_data == NULL){
		Ddim_Assertion(("I:Im_PRO_HIST_Get_HIST_Data error. hist_data=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_comm_get_hist_result(ch, hist_data);

	return D_DDIM_OK;
}

INT32 Im_PRO_HIST_Get_HIST_Ram( E_IM_PRO_HIST_CH ch, T_IM_PRO_HIST_RAM_DATA* hist_ram_data )
{
#ifdef CO_PARAM_CHECK
	if (hist_ram_data == NULL){
		Ddim_Assertion(("I:Im_PRO_HIST_Get_HIST_Ram error. hist_ram_data=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_comm_get_hist_ram(ch, hist_ram_data);

	return D_DDIM_OK;
}

/**
FLCK macro start
@param[in]	ch : Channel No.
@retval		D_DDIM_OK				: Processing OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PWCH not running NG
*/
INT32 Im_PRO_FLCK_Start( E_IM_PRO_FLCK_CH ch )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.FLCK[ch].FLTRG.bit.FLTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	im_pro_stat_set_start_status(D_IM_STAT_STATUS_FLCK_0, ch);

	return D_DDIM_OK;
}

/**
FLCK macro stop
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: PWCH not stopped NG
*/
INT32 Im_PRO_FLCK_Stop( E_IM_PRO_FLCK_CH ch, UCHAR force )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.STAT.FLCK[ch].FLTRG.bit.FLTRG = D_IM_PRO_TRG_FRAME_STOP;	// stop
	}
	else {
		IO_PRO.STAT.FLCK[ch].FLTRG.bit.FLTRG = D_IM_PRO_TRG_FORCE_STOP;	// force stop
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	im_pro_stat_set_stop_status(D_IM_STAT_STATUS_FLCK_0, ch);

	return D_DDIM_OK;
}

/**
FLCK detecting information set
@param[in]	ch : Channel No.
@param[in]	flck_ctrl : FLCK Information @ref T_IM_PRO_FLCK_CTRL
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FLCK_Ctrl( E_IM_PRO_FLCK_CH ch, T_IM_PRO_FLCK_CTRL* flck_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (flck_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_FLCK_Ctrl error. flck_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FLCK_FLCVW_MIN, D_IM_PRO_FLCK_FLCVW_MAX, flck_ctrl->flcvw, "Im_PRO_FLCK_Ctrl : flcvw" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FLCK_FLBC_MIN, D_IM_PRO_FLCK_FLBC_MAX, flck_ctrl->bc_num, "Im_PRO_FLCK_Ctrl : bc_num" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.FLCK[ch].FLCKCTL.bit.FLWPMD	= flck_ctrl->flck_type;
	IO_PRO.STAT.FLCK[ch].FLCKCTL.bit.FLWLMD	= flck_ctrl->flck_mode;
	IO_PRO.STAT.FLCK[ch].FLVW.bit.FLCVW		= flck_ctrl->flcvw;
	IO_PRO.STAT.FLCK[ch].FLBC.bit.FLBC		= flck_ctrl->bc_num;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
FLCK detect data format select.
@param[in]	ch : Channel No.
@param[in]	flck_format : FLCK detect data format @ref E_IM_PRO_FLCK_TYPE
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FLCK_Set_FLCKDataFormat( E_IM_PRO_FLCK_CH ch, E_IM_PRO_FLCK_TYPE flck_format )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.FLCK[ch].FLCKCTL.bit.FLWPMD	= flck_format;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
FLCK detecting mode set
@param[in]	ch : Channel No.
@param[in]	flck_mode : FLCK Detecting mode @ref E_IM_PRO_FLCK_MODE
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FLCK_Set_WD_Mode( E_IM_PRO_FLCK_CH ch, E_IM_PRO_FLCK_MODE flck_mode )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.FLCK[ch].FLCKCTL.bit.FLWLMD	= flck_mode;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set FLCK Window size
@param[in]	ch : Channel No.
@param[in]	flck_area : flck window information<br>
						value range :[0 - 8190] 2pixel boundary<br>
						target registor :@@FLH<br>
						value range :[0 - 8190] 2pixel boundary<br>
						target registor :@@FLV<br>
						value range :[6 - 8192] 2pixel boundary<br>
						target registor :@@FLHW<br>
						value range :[2 - 1024] 2pixel boundary(FLWLMD=0)<br>
												4pixel boundary(FLWLMD=1)<br>
						target registor :@@FLVW<br>

@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FLCK_Set_Area( E_IM_PRO_FLCK_CH ch, T_IM_PRO_FLCK_AREA* flck_area )
{
#ifdef CO_PARAM_CHECK
	if (flck_area == NULL){
		Ddim_Assertion(("I:Im_PRO_FLCK_Set_Area error. flck_area=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FLCK_FLH_MIN, D_IM_PRO_FLCK_FLH_MAX, flck_area->pos_x, "Im_PRO_FLCK_Set_Area : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FLCK_FLV_MIN, D_IM_PRO_FLCK_FLV_MAX, flck_area->pos_y, "Im_PRO_FLCK_Set_Area : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FLCK_FLHW_MIN, D_IM_PRO_FLCK_FLHW_MAX, flck_area->width, "Im_PRO_FLCK_Set_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FLCK_FLVW_MIN, D_IM_PRO_FLCK_FLVW_MAX, flck_area->lines, "Im_PRO_FLCK_Set_Area : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.FLCK[ch].FLH.bit.FLH	= flck_area->pos_x;
	IO_PRO.STAT.FLCK[ch].FLV.bit.FLV	= flck_area->pos_y;
	IO_PRO.STAT.FLCK[ch].FLHW.bit.FLHW	= flck_area->width;
	IO_PRO.STAT.FLCK[ch].FLVW.bit.FLVW	= flck_area->lines;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set FLCK Window block number
@param[in]	ch : Channel No.
@param[in]	bc_num :FLCK Window block number<br>
					value range :[1 - 4096]<br>
					target registor :@@FLBC<br>

@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FLCK_Set_Blocks( E_IM_PRO_FLCK_CH ch, USHORT bc_num )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.FLCK[ch].FLBC.bit.FLBC	= bc_num;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set calculation permission in FLCK Window block
@param[in]	ch : Channel No.
@param[in]	flcvw : FLCK Addition permission size in a block information<br>
					 value range :[2 - 1024] 2pixel boundary(FLWLMD=0)<br>
											 4pixel boundary(FLWLMD=1)<br>
					 target registor :@@FLCVW<br>

@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FLCK_Set_CW( E_IM_PRO_FLCK_CH ch, USHORT flcvw )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.FLCK[ch].FLVW.bit.FLCVW	= flcvw;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
FLCK RGB level gate threshold
@param[in]	ch : Channel No.
@param[in]	lv_gate	:	FLCK RGB gate threshold
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FLCK_Set_Lv_Gate( E_IM_PRO_FLCK_CH ch, T_IM_PRO_FLCK_LV_GATE* lv_gate )
{
#ifdef CO_PARAM_CHECK
	if (lv_gate == NULL){
		Ddim_Assertion(("I:Im_PRO_FLCK_Set_Lv_Gate error. lv_gate=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.FLCK[ch].FLTDY.bit.FLTDUY	= lv_gate->fltduy;
	IO_PRO.STAT.FLCK[ch].FLTDR.bit.FLTDUR	= lv_gate->fltdur;
	IO_PRO.STAT.FLCK[ch].FLTDG.bit.FLTDUG	= lv_gate->fltdug;
	IO_PRO.STAT.FLCK[ch].FLTDB.bit.FLTDUB	= lv_gate->fltdub;
	IO_PRO.STAT.FLCK[ch].FLTDY.bit.FLTDLY	= lv_gate->fltdly;
	IO_PRO.STAT.FLCK[ch].FLTDR.bit.FLTDLR	= lv_gate->fltdlr;
	IO_PRO.STAT.FLCK[ch].FLTDG.bit.FLTDLG	= lv_gate->fltdlg;
	IO_PRO.STAT.FLCK[ch].FLTDB.bit.FLTDLB	= lv_gate->fltdlb;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
FLCK matrix level gate threshold
@param[in]	ch : Channel No.
@param[in]	mt_lv_gate	:FLCK Matrix gate
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FLCK_Set_Matrix_Lv_Gate( E_IM_PRO_FLCK_CH ch, T_IM_PRO_FLCK_MATRIX_LV_GATE* mt_lv_gate )
{
#ifdef CO_PARAM_CHECK
	if (mt_lv_gate == NULL){
		Ddim_Assertion(("I:Im_PRO_FLCK_Set_Matrix_Lv_Gate error. mt_lv_gate=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	IO_PRO.STAT.FLCK[ch].FLMLCY.bit.FLMLUCY		= mt_lv_gate->flmlucy;
	IO_PRO.STAT.FLCK[ch].FLMLCY.bit.FLMLLCY		= mt_lv_gate->flmllcy;
	IO_PRO.STAT.FLCK[ch].FLMLCB.bit.FLMLUCB		= mt_lv_gate->flmlucb;
	IO_PRO.STAT.FLCK[ch].FLMLCB.bit.FLMLLCB		= mt_lv_gate->flmllcb;
	IO_PRO.STAT.FLCK[ch].FLMLCR.bit.FLMLUCR		= mt_lv_gate->flmlucr;
	IO_PRO.STAT.FLCK[ch].FLMLCR.bit.FLMLLCR		= mt_lv_gate->flmllcr;
	im_pro_set_reg_signed( IO_PRO.STAT.FLCK[ch].FLMC.FLMC1, union io_flmc_1, FLMC00, mt_lv_gate->flmc[0][0] );
	im_pro_set_reg_signed( IO_PRO.STAT.FLCK[ch].FLMC.FLMC1, union io_flmc_1, FLMC01, mt_lv_gate->flmc[0][1] );
	im_pro_set_reg_signed( IO_PRO.STAT.FLCK[ch].FLMC.FLMC2, union io_flmc_2, FLMC02, mt_lv_gate->flmc[0][2] );
	im_pro_set_reg_signed( IO_PRO.STAT.FLCK[ch].FLMC.FLMC2, union io_flmc_2, FLMC10, mt_lv_gate->flmc[1][0] );
	im_pro_set_reg_signed( IO_PRO.STAT.FLCK[ch].FLMC.FLMC3, union io_flmc_3, FLMC11, mt_lv_gate->flmc[1][1] );
	im_pro_set_reg_signed( IO_PRO.STAT.FLCK[ch].FLMC.FLMC3, union io_flmc_3, FLMC12, mt_lv_gate->flmc[1][2] );
	im_pro_set_reg_signed( IO_PRO.STAT.FLCK[ch].FLMC.FLMC4, union io_flmc_4, FLMC20, mt_lv_gate->flmc[2][0] );
	im_pro_set_reg_signed( IO_PRO.STAT.FLCK[ch].FLMC.FLMC4, union io_flmc_4, FLMC21, mt_lv_gate->flmc[2][1] );
	im_pro_set_reg_signed( IO_PRO.STAT.FLCK[ch].FLMC.FLMC5, union io_flmc_5, FLMC22, mt_lv_gate->flmc[2][2] );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
FLCK Y coefficient
@param[in]	ch : Channel No.
@param[in]	flckyk	:Luminosity generation coefficient Information

@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FLCK_Set_FLYK( E_IM_PRO_FLCK_CH ch, T_IM_PRO_STAT_YK* flckyk )
{
#ifdef CO_PARAM_CHECK
	if (flckyk == NULL){
		Ddim_Assertion(("I:Im_PRO_FLCK_Set_FLYK error. flckyk=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Start_Clock
	im_pro_set_reg_signed( IO_PRO.STAT.FLCK[ch].FLYKRR, union io_flykrr, FLYKRR, flckyk->ykr );
	im_pro_set_reg_signed( IO_PRO.STAT.FLCK[ch].FLYKGR, union io_flykgr, FLYKGR, flckyk->ykgr );
	im_pro_set_reg_signed( IO_PRO.STAT.FLCK[ch].FLYKGB, union io_flykgb, FLYKGB, flckyk->ykgb );
	im_pro_set_reg_signed( IO_PRO.STAT.FLCK[ch].FLYKBB, union io_flykbb, FLYKBB, flckyk->ykb );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_STAT );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get FLCK Data
@retval		FLCK Data Address	:	FLCK Data Storing Area Address
*/
ULONG Im_PRO_FLCK_Get_FLCKData( E_IM_PRO_FLCK_CH ch )
{
	UCHAR	flck_pwch_ch = 0;
	if( ch == E_IM_PRO_FLCK_CH0 ) {
		flck_pwch_ch = D_IM_PRO_FLCK0_PWCH;
	}
	else {
		flck_pwch_ch = D_IM_PRO_FLCK1_PWCH;
	}

	return im_pro_comm_get_stat_cur_data_addr( flck_pwch_ch );
}

/**
Get FLCK Last Data
@param[in]	ch : Channel No.
@retval		FLCK Data Address	:	FLCK Data Storing Area Address(Last address)
*/
ULONG Im_PRO_FLCK_Get_LastFLCKData( E_IM_PRO_FLCK_CH ch )
{
	UCHAR	flck_pwch_ch = 0;
	if( ch == E_IM_PRO_FLCK_CH0 ) {
		flck_pwch_ch = D_IM_PRO_FLCK0_PWCH;
	}
	else {
		flck_pwch_ch = D_IM_PRO_FLCK1_PWCH;
	}
	return im_pro_comm_get_stat_last_data_addr( flck_pwch_ch );
}
