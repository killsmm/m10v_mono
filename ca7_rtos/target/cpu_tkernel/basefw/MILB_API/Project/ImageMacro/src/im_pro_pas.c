/**
 * @file		im_pro_pas.c
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
// PRO Channel/Macro Flag for MCLK/ICLK control
#define	D_IM_PAS_STATUS_NONE		(0x00000000UL)			/* All stopped		*/
#define	D_IM_PAS_STATUS_SPT			(0x00000000UL)&(1<<0)	/* SPT			*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// PRO Channel/Macro start/stop status
#define im_pro_pas_set_start_status(st, ch) 	(gIM_PAS_START_Status[((st) >> 28) & 0xf] |=  (((st) & 0x0fffffff) << (ch)))							/* SEN clock status ON	*/
#define im_pro_pas_set_stop_status(st, ch)		(gIM_PAS_START_Status[((st) >> 28) & 0xf] &= ~(((st) & 0x0fffffff) << (ch)))							/* SEN clock status OFF	*/
#define im_pro_pas_get_start_status(st, ch)		(((gIM_PAS_START_Status[((st) >> 28) & 0xf] & (((st) & 0x0fffffff) << (ch))) != 0) ? TRUE : FALSE)		/* started check	*/

#define im_pro_pas_get_stop_all()				((gIM_PAS_START_Status[0] != 0) ? FALSE : TRUE)	/* started check	*/

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
static	volatile	ULONG	gIM_PAS_START_Status[1] = {0x00000000};

static const T_IM_PRO_RDMA_SPT_SHD_TBL_ADDR gIM_PRO_SPT_SHD_TBL_Addr[D_IM_PRO_PAS_SPT_TBL_CNT] = {
	{ 0x29814000 },
	{ 0x29814400 },
};

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
/*	PAS Top												*/
/********************************************************/
/**
PASTOP Macro initialize
@remarks	Please Call at the time of system starting.
*/
VOID Im_PRO_PASTOP_Init( VOID )
{
	// Software release
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Start_Clock
	IO_PRO.PAS.PASTOP.SR.bit.SR = D_IM_PRO_SR_RELEASE;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Stop_Clock
}

/**
PASTOP Macro software reset
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: All macro not stopped NG
*/
INT32 Im_PRO_PASTOP_SW_Reset( VOID )
{
#ifdef CO_ACT_PRO_CLOCK
	if (im_pro_pas_get_stop_all() == FALSE){	//not ALL Stopped
		Ddim_Print(("I:Im_PRO_PASTOP_SW_Reset. macro not stopped error. \n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif	/* CO_ACT_PRO_CLOCK */

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Start_Clock

	// Software reset
	IO_PRO.PAS.PASTOP.SR.bit.SR = D_IM_PRO_SR_RESET;

	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
PASTOP Macro clock control
@param[in]	clk_type	: PASTOP Macro clock type
@param[in]	on_off		: 0:clock on 1:clock off
@param[in]	wait_skip	: 0:non wait 1:wait 1ms. for wait PROCLK/CDK 5 cycle.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_PASTOP_Control_Clock( E_IM_PRO_PASTOP_CLK_TYPE clk_type, UCHAR on_off, UCHAR wait_skip )
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

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Start_Clock
	switch( clk_type ) {
		case E_IM_PRO_PASTOP_CLK_TYPE_PASCLK:
			IO_PRO.PAS.PASTOP.CLKSTP.bit.PSTP1 = ( ( on_off == D_IM_PRO_CLOCK_ON ) ? D_IM_PRO_CLOCK_ON : D_IM_PRO_CLOCK_OFF );
			break;

		default:
			break;
	}
	// waiting for 5 cycle
	if( wait_skip == 0 ) {
		im_pro_get_current_clk_hz( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS, &cpu_clk_hz, &macro_clk_hz );
		im_pro_common_wait_by_clk_cycle( 5, macro_clk_hz, cpu_clk_hz );
#ifndef CO_DEBUG_ON_PC
//			__nop();
#endif
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
PASTOP macro control data setting
@param[in]	ctrl : PASTOP macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_PASTOP_Ctrl( T_IM_PRO_PASTOP_CTRL* ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_PASTOP_Ctrl error. ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Start_Clock
	IO_PRO.PAS.PASTOP.PASTOPCTL1.bit.ORG	= ctrl->org;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
SPT macro start
@retval		D_DDIM_OK				: Processing OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PWCH not running NG
*/
INT32 Im_PRO_SPT_Start( VOID )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Start_Clock
	IO_PRO.PAS.SPT.SPTTRG.bit.SPTTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Start_Clock

	im_pro_pas_set_start_status(D_IM_PAS_STATUS_SPT, 0);


	return D_DDIM_OK;
}


/**
SPT macro stop
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: PWCH not stopped NG
*/
INT32 Im_PRO_SPT_Stop( UCHAR force )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Start_Clock
	if (force == 0){
		IO_PRO.PAS.SPT.SPTTRG.bit.SPTTRG = D_IM_PRO_TRG_FRAME_STOP;	// frame stop
	}
	else{
		IO_PRO.PAS.SPT.SPTTRG.bit.SPTTRG = D_IM_PRO_TRG_FORCE_STOP;	// force stop
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Start_Clock

	im_pro_pas_set_stop_status(D_IM_PAS_STATUS_SPT, 0);

	return D_DDIM_OK;
}

/**
Setup of SPT control parameter.
@param[in]	spt_ctrl : SPT Information @ref T_IM_PRO_SPT_CTRL
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SPT_Ctrl( T_IM_PRO_SPT_CTRL* spt_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (spt_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SPT_Ctrl error. spt_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_SPT_SPTBSH_MIN, D_IM_PRO_SPT_SPTBSH_MAX, spt_ctrl->horizontal_pixel, "Im_PRO_SPT_Ctrl : horizontal_pixel" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_SPT_SPTBSV_MIN, D_IM_PRO_SPT_SPTBSV_MAX, spt_ctrl->vertical_pixel, "Im_PRO_SPT_Ctrl : vertical_pixel" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_SPT_SPTBDE_MIN, D_IM_PRO_SPT_SPTBDE_MAX, spt_ctrl->Inverse_exponent, "Im_PRO_SPT_Ctrl : Inverse_exponent" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_SPT_SPTV_MIN, D_IM_PRO_SPT_SPTV_MAX, spt_ctrl->pos_y, "Im_PRO_SPT_Ctrl : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_SPT_SPTH_MIN, D_IM_PRO_SPT_SPTH_MAX, spt_ctrl->pos_x, "Im_PRO_SPT_Ctrl : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_SPT_SPTVBC_MIN, D_IM_PRO_SPT_SPTVBC_MAX, spt_ctrl->v_block_size, "Im_PRO_SPT_Ctrl : v_block_size" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_SPT_SPTHBC_MIN, D_IM_PRO_SPT_SPTHBC_MAX, spt_ctrl->h_block_size, "Im_PRO_SPT_Ctrl : h_block_size" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Start_Clock
	IO_PRO.PAS.SPT.SPTTBL.bit.SPTTBL	= spt_ctrl->table_sel;
	IO_PRO.PAS.SPT.SPTBS.bit.SPTBSH		= spt_ctrl->horizontal_pixel;
	IO_PRO.PAS.SPT.SPTBS.bit.SPTBSV		= spt_ctrl->vertical_pixel;
	IO_PRO.PAS.SPT.SPTBD.bit.SPTBDM		= spt_ctrl->Inverse_base;
	IO_PRO.PAS.SPT.SPTBD.bit.SPTBDE		= spt_ctrl->Inverse_exponent;
	IO_PRO.PAS.SPT.SPTV.bit.SPTV		= spt_ctrl->pos_y;
	IO_PRO.PAS.SPT.SPTH.bit.SPTH		= spt_ctrl->pos_x;
	IO_PRO.PAS.SPT.SPTVBC.bit.SPTVBC	= spt_ctrl->v_block_size;
	IO_PRO.PAS.SPT.SPTHBC.bit.SPTHBC	= spt_ctrl->h_block_size;
	im_pro_set_reg_signed( IO_PRO.PAS.SPT.SPTCLP, union io_sptclp, SPTCLPH, spt_ctrl->upper_clip_value );
	im_pro_set_reg_signed( IO_PRO.PAS.SPT.SPTCLP, union io_sptclp, SPTCLPL, spt_ctrl->lower_clip_value );
	im_pro_set_reg_signed( IO_PRO.PAS.SPT.SPTNOPV, union io_sptnopv, SPTNOPV, spt_ctrl->correction_through_val );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}


/**
A setup of enable access to the built-in RAM of SPT.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SPT_Set_PAEN( UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.PAS.SPT.SPTTRG.bit.SPTTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_SPT_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Start_Clock
	IO_PRO.PAS.SPT.SPTPAEN.bit.PAEN	= paen_trg;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}


/**
shading compensation table is registered.
@param[in]	tbl_sel : table select.
@param[in]	shd_tbl		: Shading table info
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@note		None
*/
INT32 Im_PRO_SPT_Set_Table( E_IM_PRO_SPT_TBL_SEL tbl_sel, T_IM_PRO_SPT_TABLE* shd_tbl )
{
	UCHAR* pDst_table;

#ifdef CO_PARAM_CHECK
	if (shd_tbl == NULL){
		Ddim_Assertion(("I:Im_PRO_SPT_Set_Table error. spt_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( ( shd_tbl->pshd_tbl == NULL ) || ( shd_tbl->size == 0 ) ) {
		Ddim_Assertion(("I:Im_PRO_SPT_Set_Table error. shd_tbl->pshd_tbl=%lx shd_tbl->size=%u \n", (ULONG)shd_tbl->pshd_tbl, shd_tbl->size));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	if( tbl_sel == E_IM_PRO_SPT_TBL_SEL_SPTTBL0 ) {
		pDst_table = (UCHAR*)IO_PRO_TBL.PAS_TBL.SPTTBL0.word;
	}
	else {
		pDst_table = (UCHAR*)IO_PRO_TBL.PAS_TBL.SPTTBL1.word;
	}

	im_pro_on_hclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Start_Clock
	im_pro_memcpy( pDst_table, shd_tbl->pshd_tbl, shd_tbl->size );
	im_pro_off_hclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_PAS );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
Get the top address of the address array of FSHD control.
@param[in]		tbl_sel : table select.
@param[out]		addr					: Top address of the address array of FSHD Concentric Circles Correction table.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
INT32 Im_PRO_Get_RdmaAddr_SPT_SHD_Tbl( E_IM_PRO_SPT_TBL_SEL tbl_sel, const T_IM_PRO_RDMA_SPT_SHD_TBL_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( addr == NULL ) {
		Ddim_Assertion(("I:Im_PRO_Get_RdmaAddr_SPT_SHD_Tbl. error. addr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	*addr = &gIM_PRO_SPT_SHD_TBL_Addr[tbl_sel];

	return D_DDIM_OK;
}

