/**
 * @file		im_pro_sen.c
 * @brief		SEN unit controler of PRO macro.
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
#define D_IM_PRO_SG_TRG_FORCE_STOP		(0)					// SG Trigger Force STOP
#define D_IM_PRO_SG_TRG_FRAME_STOP		(2)					// SG Trigger Frame STOP

#define D_IM_PRO_SLVS_TRG_FORCE_STOP	(0)					// SLVS-ECG Trigger Force STOP
#define D_IM_PRO_SLVS_TRG_FRAME_STOP	(2)					// SLVS-ECG Trigger Frame STOP

#define D_IM_PRO_SLVS_SW_RESET			(1)					// SLVS-EC Software reset
#define D_IM_PRO_SLVS_SW_RESET_RELEASE	(0)					// SLVS-EC Software reset release
#define D_IM_PRO_SLVS_CFG_UPDATE		(1)					// SLVS-EC Configuration update
#define D_IM_PRO_SLVS_PMA_OPT_INIT_DIS	(0)					// SLVS-EC PMA macro optional initialization disabled.

#define D_IM_PRO_SLVS_INIT_WAIT_PD		(5)					// SLVS-EC wait time(us) Between [Termination resistor adjustment circuit PD release] and [PMA internal PLL PD release]
#define D_IM_PRO_DMIPI_WAIT_PHY_STARTUP	(100)				// D-MIPI wait time(us) for start-up stabilization.
#define D_IM_PRO_CMIPI_WAIT_PHY_STARTUP	(100)				// C-MIPI wait time(us) for start-up stabilization.
/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

static const USHORT gim_pro_maximum_size_lvds[E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_3 + 1][E_IM_PRO_LVDS_CH_3 + 1] = {
	{2960,	2960,	2960,	2960},
	{5920,	5920,	0,		1024},
	{11840,	0,		0,		1024},
	{0,		0,		0,		0	},
};

static const USHORT gim_pro_maximum_size_ldiv[E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_3 + 1][E_IM_PRO_LDIV_CH_3 + 1] = {
	{2960,	2960,	2960,	2960},
	{5920,	5920,	0,		1024},
	{11840,	0,		0,		1024},
	{0,		0,		0,		0	},
};

// RDMA I/F
static const T_IM_PRO_RDMA_OBT_ADDR gIM_PRO_OBT_Addr[D_IM_PRO_SEN_OBT_CNT] = {
	// E_IM_PRO_OBT_CH_0_0
	{
		0x2800B8C0,0x2800B8D4,0x2800B8E8,0x2800B8EC,
		0x2800B8F0,0x2800B8F4,
	},
	// E_IM_PRO_OBT_CH_0_1
	{
		0x2800B9C0,0x2800B9D4,0x2800B9E8,0x2800B9EC,
		0x2800B9F0,0x2800B9F4,
	},
	// E_IM_PRO_OBT_CH_1_0
	{
		0x2800BAC0,0x2800BAD4,0x2800BAE8,0x2800BAEC,
		0x2800BAF0,0x2800BAF4,
	},
	// E_IM_PRO_OBT_CH_1_1
	{
		0x2800BBC0,0x2800BBD4,0x2800BBE8,0x2800BBEC,
		0x2800BBF0,0x2800BBF4,
	},
	// E_IM_PRO_OBT_CH_2_0
	{
		0x2800BCC0,0x2800BCD4,0x2800BCE8,0x2800BCEC,
		0x2800BCF0,0x2800BCF4,
	},
	// E_IM_PRO_OBT_CH_2_1
	{
		0x2800BDC0,0x2800BDD4,0x2800BDE8,0x2800BDEC,
		0x2800BDF0,0x2800BDF4,
	},
	// E_IM_PRO_OBT_CH_3_0
	{
		0x2800BEC0,0x2800BED4,0x2800BEE8,0x2800BEEC,
		0x2800BEF0,0x2800BEF4,
	},
	// E_IM_PRO_OBT_CH_3_1
	{
		0x2800BFC0,0x2800BFD4,0x2800BFE8,0x2800BFEC,
		0x2800BFF0,0x2800BFF4,
	},
};


/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
VOID im_pro_sentop_set_cphy_common_ctrl( UCHAR phy_no, T_IM_PRO_SENTOP_CPHY_COMMON_MODE_CTRL* cphy_common )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	switch( phy_no ) {
		case 0:
			IO_PRO.SEN.SENTOP.PHY0MDCTL.bit.PHY0_BITWIDTHSEL			= cphy_common->bitwidthsel;
			IO_PRO.SEN.SENTOP.PHY0MDCTL.bit.PHY0_RXMODE					= cphy_common->rxmode;
			IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL1.bit.PHY0_D0SETSEL	= cphy_common->d0setsel;
			IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL1.bit.PHY0_D1SETSEL	= cphy_common->d1setsel;
			IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL1.bit.PHY0_D2SETSEL	= cphy_common->d2setsel;
			IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL2.bit.PHY0_D3SETSEL	= cphy_common->d3setsel;
			break;

		case 1:
			IO_PRO.SEN.SENTOP.PHY1MDCTL.bit.PHY1_BITWIDTHSEL			= cphy_common->bitwidthsel;
			IO_PRO.SEN.SENTOP.PHY1MDCTL.bit.PHY1_RXMODE					= cphy_common->rxmode;
			IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL1.bit.PHY1_D0SETSEL	= cphy_common->d0setsel;
			IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL1.bit.PHY1_D1SETSEL	= cphy_common->d1setsel;
			IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL1.bit.PHY1_D2SETSEL	= cphy_common->d2setsel;
			IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL2.bit.PHY1_D3SETSEL	= cphy_common->d3setsel;
			break;

		case 2:
			IO_PRO.SEN.SENTOP.PHY2MDCTL.bit.PHY2_BITWIDTHSEL			= cphy_common->bitwidthsel;
			IO_PRO.SEN.SENTOP.PHY2MDCTL.bit.PHY2_RXMODE					= cphy_common->rxmode;
			IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL1.bit.PHY2_D0SETSEL	= cphy_common->d0setsel;
			IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL1.bit.PHY2_D1SETSEL	= cphy_common->d1setsel;
			IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL1.bit.PHY2_D2SETSEL	= cphy_common->d2setsel;
			IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL2.bit.PHY2_D3SETSEL	= cphy_common->d3setsel;
			break;

		//case 3:
		default:
			IO_PRO.SEN.SENTOP.PHY3MDCTL.bit.PHY3_BITWIDTHSEL			= cphy_common->bitwidthsel;
			IO_PRO.SEN.SENTOP.PHY3MDCTL.bit.PHY3_RXMODE					= cphy_common->rxmode;
			IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL1.bit.PHY3_D0SETSEL	= cphy_common->d0setsel;
			IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL1.bit.PHY3_D1SETSEL	= cphy_common->d1setsel;
			IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL1.bit.PHY3_D2SETSEL	= cphy_common->d2setsel;
			IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL2.bit.PHY3_D3SETSEL	= cphy_common->d3setsel;
			break;
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/********************************************************/
/*	SEN Top												*/
/********************************************************/
/**
SENTOP Macro initialize
@remarks	Please Call at the time of system starting.
*/
VOID Im_PRO_SENTOP_Init( VOID )
{
	// Software release
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.SR.bit.SR  = D_IM_PRO_SR_RELEASE;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock
}


/**
SENTOP Macro software reset
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: All macro not stopped NG
*/
INT32 Im_PRO_SENTOP_SW_Reset( VOID )
{
#ifdef CO_ACT_PRO_CLOCK
	if (im_pro_sen_get_stop_all() == FALSE){	//not ALL Stopped
		Ddim_Print(("I:Im_PRO_SENTOP_SW_Reset. macro not stopped error. \n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif	/* CO_ACT_PRO_CLOCK */

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	// Software reset
	IO_PRO.SEN.SENTOP.SR.bit.SR  = D_IM_PRO_SR_RESET;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}


/**
SENTOP Macro clock control
@param[in]	clk_type	: SENTOP Macro clock type
@param[in]	on_off		: 0:clock on 1:clock off
@param[in]	wait_skip	: 0:non wait 1:wait 1ms. for wait PROCLK/CDK 5 cycle.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_Control_Clock( E_IM_PRO_SENTOP_CLK_TYPE clk_type, UCHAR on_off, UCHAR wait_skip )
{
	UINT32 cpu_clk_hz;
	UINT32 macro_clk_hz;
#ifdef CO_PARAM_CHECK
	if (on_off > D_IM_PRO_CLOCK_OFF){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Control_Clock error. on_off value over!! (%d)\n", on_off));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else {
		// DO NOTHING
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	switch( clk_type ) {
		case E_IM_PRO_SENTOP_CLK_TYPE_SENSORCLK:
			IO_PRO.SEN.SENTOP.CLKSTP.CLKSTP1.bit.PSTP1 = ( ( on_off == D_IM_PRO_CLOCK_ON ) ? D_IM_PRO_CLOCK_ON : D_IM_PRO_CLOCK_OFF );
			break;

		case E_IM_PRO_SENTOP_CLK_TYPE_APCLK0:
			IO_PRO.SEN.SENTOP.CLKSTP.CLKSTP1.bit.PSTP2 = ( ( on_off == D_IM_PRO_CLOCK_ON ) ? D_IM_PRO_CLOCK_ON : D_IM_PRO_CLOCK_OFF );
			break;

		case E_IM_PRO_SENTOP_CLK_TYPE_APCLK1:
			IO_PRO.SEN.SENTOP.CLKSTP.CLKSTP1.bit.PSTP3 = ( ( on_off == D_IM_PRO_CLOCK_ON ) ? D_IM_PRO_CLOCK_ON : D_IM_PRO_CLOCK_OFF );
			break;

		case E_IM_PRO_SENTOP_CLK_TYPE_APCLK2:
			IO_PRO.SEN.SENTOP.CLKSTP.CLKSTP1.bit.PSTP4 = ( ( on_off == D_IM_PRO_CLOCK_ON ) ? D_IM_PRO_CLOCK_ON : D_IM_PRO_CLOCK_OFF );
			break;

		//case E_IM_PRO_SENTOP_CLK_TYPE_APCLK3:
		default:
			IO_PRO.SEN.SENTOP.CLKSTP.CLKSTP2.bit.PSTP5 = ( ( on_off == D_IM_PRO_CLOCK_ON ) ? D_IM_PRO_CLOCK_ON : D_IM_PRO_CLOCK_OFF );
			break;
	}
	// waiting for 5 cycle
	if( wait_skip == 0 ) {
		im_pro_get_current_clk_hz( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN, &cpu_clk_hz, &macro_clk_hz );
		im_pro_common_wait_by_clk_cycle( 5, macro_clk_hz, cpu_clk_hz );
#ifndef CO_DEBUG_ON_PC
//			__nop();
#endif
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}


/**
SENTOP macro control data setting
@param[in]	ctrl : SENTOP macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_Ctrl( T_IM_PRO_SENTOP_CTRL* ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_Ctrl error. ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (im_pro_sen_get_stop_all() == FALSE){	//not ALL Stopped
		Ddim_Print(("I:Im_PRO_SENTOP_Ctrl. macro not stopped error. \n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
	if( ctrl->sencore_hmax_size_ctrl > E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_2 ) {
		Ddim_Print(("I:Im_PRO_SENTOP_Ctrl. sencore_hmax_size_ctrl error.(%u) \n", ctrl->sencore_hmax_size_ctrl));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
	if( ctrl->ldiv_hmax_size_ctrl > E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_2 ) {
		Ddim_Print(("I:Im_PRO_SENTOP_Ctrl. ldiv_hmax_size_ctrl error.(%u) \n", ctrl->ldiv_hmax_size_ctrl));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL11.bit.SENIN0		= ctrl->sen_connect_type_sel[0];
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL11.bit.SENIN1		= ctrl->sen_connect_type_sel[1];
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL11.bit.SENIN2		= ctrl->sen_connect_type_sel[2];
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL11.bit.SENIN3		= ctrl->sen_connect_type_sel[3];
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL11.bit.BLOBSET0	= ctrl->blob_input_sel[0];
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL11.bit.BLOBSET1	= ctrl->blob_input_sel[1];
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL12.bit.BLOBSET2	= ctrl->blob_input_sel[2];
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL12.bit.BLOBSET3	= ctrl->blob_input_sel[3];
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL12.bit.PWSW0		= ctrl->sdram_output_type[0];
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL12.bit.PWSW1		= ctrl->sdram_output_type[1];
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.PWSW2		= ctrl->sdram_output_type[2];
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.PWSW3		= ctrl->sdram_output_type[3];
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.PIPE2S		= ctrl->output_sel_sro2;
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.SENRAMSW	= ctrl->sencore_hmax_size_ctrl;
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.LDIVRAMSW	= ctrl->ldiv_hmax_size_ctrl;
	IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.LDIVMCH	= ctrl->ldiv_multi_ch_input_ctrl;


	IO_PRO.SEN.SENTOP.SENTOPCTL2.SENTOPCTL2_1.bit.ORG00		= ctrl->first_pixel[0][0];
	IO_PRO.SEN.SENTOP.SENTOPCTL2.SENTOPCTL2_1.bit.ORG01		= ctrl->first_pixel[0][1];
	IO_PRO.SEN.SENTOP.SENTOPCTL2.SENTOPCTL2_1.bit.ORG10		= ctrl->first_pixel[1][0];
	IO_PRO.SEN.SENTOP.SENTOPCTL2.SENTOPCTL2_1.bit.ORG11		= ctrl->first_pixel[1][1];
	IO_PRO.SEN.SENTOP.SENTOPCTL2.SENTOPCTL2_2.bit.ORG20		= ctrl->first_pixel[2][0];
	IO_PRO.SEN.SENTOP.SENTOPCTL2.SENTOPCTL2_2.bit.ORG21		= ctrl->first_pixel[2][1];
	IO_PRO.SEN.SENTOP.SENTOPCTL2.SENTOPCTL2_2.bit.ORG30		= ctrl->first_pixel[3][0];
	IO_PRO.SEN.SENTOP.SENTOPCTL2.SENTOPCTL2_2.bit.ORG31		= ctrl->first_pixel[3][1];
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}


/**
SENTOP BitShift macro control data setting
@param[in]	ctrl : SENTOP BitShift macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_BS_Ctrl( T_IM_PRO_SENTOP_BS_CTRL* ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_BS_Ctrl error. ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (im_pro_sen_get_stop_all() == FALSE){	//not ALL Stopped
		Ddim_Print(("I:Im_PRO_SENTOP_BS_Ctrl. macro not stopped error. \n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.BSCTL.BSCTL1.bit.BSAMNT0	= ctrl->shift_value[0];
	IO_PRO.SEN.SENTOP.BSCTL.BSCTL1.bit.BSAMNT1	= ctrl->shift_value[1];
	IO_PRO.SEN.SENTOP.BSCTL.BSCTL1.bit.BSAMNT2	= ctrl->shift_value[2];
	IO_PRO.SEN.SENTOP.BSCTL.BSCTL1.bit.BSAMNT3	= ctrl->shift_value[3];
	IO_PRO.SEN.SENTOP.BSCTL.BSCTL1.bit.BSLMD0	= ctrl->sat_compensation[0];
	IO_PRO.SEN.SENTOP.BSCTL.BSCTL1.bit.BSLMD1	= ctrl->sat_compensation[1];
	IO_PRO.SEN.SENTOP.BSCTL.BSCTL1.bit.BSLMD2	= ctrl->sat_compensation[2];
	IO_PRO.SEN.SENTOP.BSCTL.BSCTL1.bit.BSLMD3	= ctrl->sat_compensation[3];
	IO_PRO.SEN.SENTOP.BSCTL.BSCTL2.bit.BSLR0	= ctrl->shift_dir[0];
	IO_PRO.SEN.SENTOP.BSCTL.BSCTL2.bit.BSLR1	= ctrl->shift_dir[1];
	IO_PRO.SEN.SENTOP.BSCTL.BSCTL2.bit.BSLR2	= ctrl->shift_dir[2];
	IO_PRO.SEN.SENTOP.BSCTL.BSCTL2.bit.BSLR3	= ctrl->shift_dir[3];
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}


/**
SENTOP OBT macro setting
@param[in]	ctrl : SENTOP BitShift macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_OBT_Ctrl( T_IM_PRO_SENTOP_OBT_CTRL* ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_OBT_Ctrl error. ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.OBTCDCCTL1.bit.OBTCPAT	= ctrl->obt_comm_perttern;
	IO_PRO.SEN.SENTOP.OBTCDCSET.bit.OBTCDCSET	= 1;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	// waiting for 5 cycle
	im_pro_common_wait_by_clk_cycle( 5, 100*1000*1000, 660*1000*1000 );
#ifndef CO_DEBUG_ON_PC
//	__nop();
#endif

	return D_DDIM_OK;
}


/**
SENTOP OBT macro weight coefficient setting
@param[in]	coeff : SENTOP OBT macro weight coefficient information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_OBT_Set_Weight_Coeff( T_IM_PRO_SENTOP_OBT_WEIGHT_COEFF* coeff )
{
#ifdef CO_PARAM_CHECK
	if (coeff == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_OBT_Set_Weight_Coeff error. coeff=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.OBTCDCCTL2.OBTCDCCTL2_1.bit.OBTDWC00	= coeff->coeff[0][0];
	IO_PRO.SEN.SENTOP.OBTCDCCTL2.OBTCDCCTL2_1.bit.OBTDWC01	= coeff->coeff[0][1];
	IO_PRO.SEN.SENTOP.OBTCDCCTL2.OBTCDCCTL2_1.bit.OBTDWC10	= coeff->coeff[1][0];
	IO_PRO.SEN.SENTOP.OBTCDCCTL2.OBTCDCCTL2_1.bit.OBTDWC11	= coeff->coeff[1][1];
	IO_PRO.SEN.SENTOP.OBTCDCCTL2.OBTCDCCTL2_2.bit.OBTDWC20	= coeff->coeff[2][0];
	IO_PRO.SEN.SENTOP.OBTCDCCTL2.OBTCDCCTL2_2.bit.OBTDWC21	= coeff->coeff[2][1];
	IO_PRO.SEN.SENTOP.OBTCDCCTL2.OBTCDCCTL2_2.bit.OBTDWC30	= coeff->coeff[3][0];
	IO_PRO.SEN.SENTOP.OBTCDCCTL2.OBTCDCCTL2_2.bit.OBTDWC31	= coeff->coeff[3][1];
	IO_PRO.SEN.SENTOP.OBTCDCSET.bit.OBTCDCSET	= 1;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	// waiting for 5 cycle
	im_pro_common_wait_by_clk_cycle( 5, 100*1000*1000, 660*1000*1000 );
#ifndef CO_DEBUG_ON_PC
//	__nop();
#endif

	return D_DDIM_OK;
}

/**
SENTOP LVDS macro setting
@param[in]	ctrl : SENTOP LVDS macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_LVDS_Ctrl( T_IM_PRO_SENTOP_LVDS_CTRL* ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_LVDS_Ctrl error. ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.LVDSCTL.bit.LVDSCKPD	= ctrl->clock_lane_powerdown;
	IO_PRO.SEN.SENTOP.LVDSCTL.bit.LVDSDLPD	= ctrl->data_lane_powerdown;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}


/**
SENTOP SLVS macro setting
@param[in]	ctrl : SENTOP SLVS macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_SLVS_Ctrl( T_IM_PRO_SENTOP_SLVS_CTRL* ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_SLVS_Ctrl error. ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.SLVSECCTL.bit.SLVSECSEL		= ctrl->link_mode;
	IO_PRO.SEN.SENTOP.SLVSECCTL.bit.PHY0_RXPDCK		= ctrl->phy0_rxpdck;
	IO_PRO.SEN.SENTOP.SLVSECCTL.bit.PHY1_RXPDCK		= ctrl->phy1_rxpdck;
	IO_PRO.SEN.SENTOP.SLVSECCTL.bit.PHY2_RXPDCK		= ctrl->phy2_rxpdck;
	IO_PRO.SEN.SENTOP.SLVSECCTL.bit.PHY3_RXPDCK		= ctrl->phy3_rxpdck;
	if( ctrl->clk_ctrl_enabled == TRUE ) {
		IO_PRO.SEN.SENTOP.SLVSECCTL.bit.PHY0_RXPCLK2AEN	= ctrl->phy0_rxpclk2aen;
		IO_PRO.SEN.SENTOP.SLVSECCTL.bit.PHY1_RXPCLK2AEN	= ctrl->phy1_rxpclk2aen;
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}


/**
SENTOP SG macro setting
@param[in]	ctrl : SG macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_SG_Ctrl( T_IM_PRO_SENTOP_SG_CTRL* ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_SG_Ctrl error. ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.SGOUTCTL.bit.SGSEL0	= ctrl->sg_if_output_sel[0];
	IO_PRO.SEN.SENTOP.SGOUTCTL.bit.SGSEL1	= ctrl->sg_if_output_sel[1];
	IO_PRO.SEN.SENTOP.SGOUTCTL.bit.SGSEL2	= ctrl->sg_if_output_sel[2];
	IO_PRO.SEN.SENTOP.SGOUTCTL.bit.SGSEL3	= ctrl->sg_if_output_sel[3];
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}


/**
SENTOP CombpPHY macro setting
@param[in]	cphy_ctrl : CombpPHY control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_CPHY_Ctrl( T_IM_PRO_SENTOP_CPHY_CTRL* cphy_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (cphy_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_CPHY_Ctrl error. cphy_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.PHYCIFCTL.PHYCIFCTL1.bit.PHY_PREGCNT			= cphy_ctrl->common_if_ctrl.pregcnt;
	IO_PRO.SEN.SENTOP.PHYCIFCTL.PHYCIFCTL1.bit.PHY_FREQCNT			= cphy_ctrl->common_if_ctrl.freqcnt;
	IO_PRO.SEN.SENTOP.PHYCIFCTL.PHYCIFCTL1.bit.PHY_PDCCNT			= cphy_ctrl->common_if_ctrl.pdccnt;
	IO_PRO.SEN.SENTOP.PHYCIFCTL.PHYCIFCTL1.bit.PHY_SETDF			= cphy_ctrl->common_if_ctrl.setdf;
	IO_PRO.SEN.SENTOP.PHYCIFCTL.PHYCIFCTL1.bit.PHY_PICODETST		= cphy_ctrl->common_if_ctrl.picodetst;
	IO_PRO.SEN.SENTOP.PHYCIFCTL.PHYCIFCTL2.bit.PHY_PMONSEL			= cphy_ctrl->common_if_ctrl.pmonsel;
	IO_PRO.SEN.SENTOP.PHYCIFCTL.PHYCIFCTL2.bit.PHY_DMONSEL			= cphy_ctrl->common_if_ctrl.dmonsel;
	IO_PRO.SEN.SENTOP.PHYCIFCTL.PHYCIFCTL2.bit.PHY_CNTSEL			= cphy_ctrl->common_if_ctrl.cntsel;
	IO_PRO.SEN.SENTOP.PHYCIFCTL.PHYCIFCTL2.bit.PHY_MULGAINF			= cphy_ctrl->common_if_ctrl.mulgainf;
	IO_PRO.SEN.SENTOP.PHYCIFCTL.PHYCIFCTL2.bit.PHY_MULGAINP			= cphy_ctrl->common_if_ctrl.mulgainp;
	IO_PRO.SEN.SENTOP.PHYCIFCTL.PHYCIFCTL3.bit.PHY_CDRFUNC			= cphy_ctrl->common_if_ctrl.cdrfunc;

	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL1.bit.PHY0_ZEROP3UICP0	= cphy_ctrl->cpmode_ctrl[0].zerop3uicp0;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL1.bit.PHY0_ZEROP4UICP0	= cphy_ctrl->cpmode_ctrl[0].zerop4uicp0;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL1.bit.PHY0_ZEROP5UICP0	= cphy_ctrl->cpmode_ctrl[0].zerop5uicp0;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL1.bit.PHY0_ZEROP6UICP0	= cphy_ctrl->cpmode_ctrl[0].zerop6uicp0;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL1.bit.PHY0_DETCNT_CP0	= cphy_ctrl->cpmode_ctrl[0].detcnt_cp0;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL1.bit.PHY0_EQ_EN_CP0	= cphy_ctrl->cpmode_ctrl[0].eq_en_cp0;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL1.bit.PHY0_EQ_SET_CP0	= cphy_ctrl->cpmode_ctrl[0].eq_set_cp0;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL2.bit.PHY0_ZEROP3UICP1	= cphy_ctrl->cpmode_ctrl[0].zerop3uicp1;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL2.bit.PHY0_ZEROP4UICP1	= cphy_ctrl->cpmode_ctrl[0].zerop4uicp1;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL2.bit.PHY0_ZEROP5UICP1	= cphy_ctrl->cpmode_ctrl[0].zerop5uicp1;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL2.bit.PHY0_ZEROP6UICP1	= cphy_ctrl->cpmode_ctrl[0].zerop6uicp1;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL2.bit.PHY0_DETCNT_CP1	= cphy_ctrl->cpmode_ctrl[0].detcnt_cp1;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL2.bit.PHY0_EQ_EN_CP1	= cphy_ctrl->cpmode_ctrl[0].eq_en_cp1;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL2.bit.PHY0_EQ_SET_CP1	= cphy_ctrl->cpmode_ctrl[0].eq_set_cp1;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL3.bit.PHY0_ZEROP3UICP2	= cphy_ctrl->cpmode_ctrl[0].zerop3uicp2;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL3.bit.PHY0_ZEROP4UICP2	= cphy_ctrl->cpmode_ctrl[0].zerop4uicp2;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL3.bit.PHY0_ZEROP5UICP2	= cphy_ctrl->cpmode_ctrl[0].zerop5uicp2;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL3.bit.PHY0_ZEROP6UICP2	= cphy_ctrl->cpmode_ctrl[0].zerop6uicp2;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL3.bit.PHY0_DETCNT_CP2	= cphy_ctrl->cpmode_ctrl[0].detcnt_cp2;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL3.bit.PHY0_EQ_EN_CP2	= cphy_ctrl->cpmode_ctrl[0].eq_en_cp2;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL3.bit.PHY0_EQ_SET_CP2	= cphy_ctrl->cpmode_ctrl[0].eq_set_cp2;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL1.bit.PHY1_ZEROP3UICP0	= cphy_ctrl->cpmode_ctrl[1].zerop3uicp0;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL1.bit.PHY1_ZEROP4UICP0	= cphy_ctrl->cpmode_ctrl[1].zerop4uicp0;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL1.bit.PHY1_ZEROP5UICP0	= cphy_ctrl->cpmode_ctrl[1].zerop5uicp0;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL1.bit.PHY1_ZEROP6UICP0	= cphy_ctrl->cpmode_ctrl[1].zerop6uicp0;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL1.bit.PHY1_DETCNT_CP0	= cphy_ctrl->cpmode_ctrl[1].detcnt_cp0;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL1.bit.PHY1_EQ_EN_CP0	= cphy_ctrl->cpmode_ctrl[1].eq_en_cp0;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL1.bit.PHY1_EQ_SET_CP0	= cphy_ctrl->cpmode_ctrl[1].eq_set_cp0;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL2.bit.PHY1_ZEROP3UICP1	= cphy_ctrl->cpmode_ctrl[1].zerop3uicp1;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL2.bit.PHY1_ZEROP4UICP1	= cphy_ctrl->cpmode_ctrl[1].zerop4uicp1;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL2.bit.PHY1_ZEROP5UICP1	= cphy_ctrl->cpmode_ctrl[1].zerop5uicp1;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL2.bit.PHY1_ZEROP6UICP1	= cphy_ctrl->cpmode_ctrl[1].zerop6uicp1;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL2.bit.PHY1_DETCNT_CP1	= cphy_ctrl->cpmode_ctrl[1].detcnt_cp1;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL2.bit.PHY1_EQ_EN_CP1	= cphy_ctrl->cpmode_ctrl[1].eq_en_cp1;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL2.bit.PHY1_EQ_SET_CP1	= cphy_ctrl->cpmode_ctrl[1].eq_set_cp1;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL3.bit.PHY1_ZEROP3UICP2	= cphy_ctrl->cpmode_ctrl[1].zerop3uicp2;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL3.bit.PHY1_ZEROP4UICP2	= cphy_ctrl->cpmode_ctrl[1].zerop4uicp2;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL3.bit.PHY1_ZEROP5UICP2	= cphy_ctrl->cpmode_ctrl[1].zerop5uicp2;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL3.bit.PHY1_ZEROP6UICP2	= cphy_ctrl->cpmode_ctrl[1].zerop6uicp2;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL3.bit.PHY1_DETCNT_CP2	= cphy_ctrl->cpmode_ctrl[1].detcnt_cp2;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL3.bit.PHY1_EQ_EN_CP2	= cphy_ctrl->cpmode_ctrl[1].eq_en_cp2;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL1.bit.PHY2_ZEROP3UICP0	= cphy_ctrl->cpmode_ctrl[2].zerop3uicp0;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL1.bit.PHY2_ZEROP4UICP0	= cphy_ctrl->cpmode_ctrl[2].zerop4uicp0;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL1.bit.PHY2_ZEROP5UICP0	= cphy_ctrl->cpmode_ctrl[2].zerop5uicp0;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL1.bit.PHY2_ZEROP6UICP0	= cphy_ctrl->cpmode_ctrl[2].zerop6uicp0;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL1.bit.PHY2_DETCNT_CP0	= cphy_ctrl->cpmode_ctrl[2].detcnt_cp0;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL1.bit.PHY2_EQ_EN_CP0	= cphy_ctrl->cpmode_ctrl[2].eq_en_cp0;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL1.bit.PHY2_EQ_SET_CP0	= cphy_ctrl->cpmode_ctrl[2].eq_set_cp0;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL2.bit.PHY2_ZEROP3UICP1	= cphy_ctrl->cpmode_ctrl[2].zerop3uicp1;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL2.bit.PHY2_ZEROP4UICP1	= cphy_ctrl->cpmode_ctrl[2].zerop4uicp1;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL2.bit.PHY2_ZEROP5UICP1	= cphy_ctrl->cpmode_ctrl[2].zerop5uicp1;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL2.bit.PHY2_ZEROP6UICP1	= cphy_ctrl->cpmode_ctrl[2].zerop6uicp1;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL2.bit.PHY2_DETCNT_CP1	= cphy_ctrl->cpmode_ctrl[2].detcnt_cp1;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL2.bit.PHY2_EQ_EN_CP1	= cphy_ctrl->cpmode_ctrl[2].eq_en_cp1;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL2.bit.PHY2_EQ_SET_CP1	= cphy_ctrl->cpmode_ctrl[2].eq_set_cp1;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL3.bit.PHY2_ZEROP3UICP2	= cphy_ctrl->cpmode_ctrl[2].zerop3uicp2;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL3.bit.PHY2_ZEROP4UICP2	= cphy_ctrl->cpmode_ctrl[2].zerop4uicp2;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL3.bit.PHY2_ZEROP5UICP2	= cphy_ctrl->cpmode_ctrl[2].zerop5uicp2;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL3.bit.PHY2_ZEROP6UICP2	= cphy_ctrl->cpmode_ctrl[2].zerop6uicp2;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL3.bit.PHY2_DETCNT_CP2	= cphy_ctrl->cpmode_ctrl[2].detcnt_cp2;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL3.bit.PHY2_EQ_EN_CP2	= cphy_ctrl->cpmode_ctrl[2].eq_en_cp2;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL3.bit.PHY2_EQ_SET_CP2	= cphy_ctrl->cpmode_ctrl[2].eq_set_cp2;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL1.bit.PHY3_ZEROP3UICP0	= cphy_ctrl->cpmode_ctrl[3].zerop3uicp0;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL1.bit.PHY3_ZEROP4UICP0	= cphy_ctrl->cpmode_ctrl[3].zerop4uicp0;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL1.bit.PHY3_ZEROP5UICP0	= cphy_ctrl->cpmode_ctrl[3].zerop5uicp0;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL1.bit.PHY3_ZEROP6UICP0	= cphy_ctrl->cpmode_ctrl[3].zerop6uicp0;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL1.bit.PHY3_DETCNT_CP0	= cphy_ctrl->cpmode_ctrl[3].detcnt_cp0;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL1.bit.PHY3_EQ_EN_CP0	= cphy_ctrl->cpmode_ctrl[3].eq_en_cp0;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL1.bit.PHY3_EQ_SET_CP0	= cphy_ctrl->cpmode_ctrl[3].eq_set_cp0;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL2.bit.PHY3_ZEROP3UICP1	= cphy_ctrl->cpmode_ctrl[3].zerop3uicp1;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL2.bit.PHY3_ZEROP4UICP1	= cphy_ctrl->cpmode_ctrl[3].zerop4uicp1;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL2.bit.PHY3_ZEROP5UICP1	= cphy_ctrl->cpmode_ctrl[3].zerop5uicp1;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL2.bit.PHY3_ZEROP6UICP1	= cphy_ctrl->cpmode_ctrl[3].zerop6uicp1;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL2.bit.PHY3_DETCNT_CP1	= cphy_ctrl->cpmode_ctrl[3].detcnt_cp1;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL2.bit.PHY3_EQ_EN_CP1	= cphy_ctrl->cpmode_ctrl[3].eq_en_cp1;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL2.bit.PHY3_EQ_SET_CP1	= cphy_ctrl->cpmode_ctrl[3].eq_set_cp1;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL3.bit.PHY3_ZEROP3UICP2	= cphy_ctrl->cpmode_ctrl[3].zerop3uicp2;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL3.bit.PHY3_ZEROP4UICP2	= cphy_ctrl->cpmode_ctrl[3].zerop4uicp2;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL3.bit.PHY3_ZEROP5UICP2	= cphy_ctrl->cpmode_ctrl[3].zerop5uicp2;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL3.bit.PHY3_ZEROP6UICP2	= cphy_ctrl->cpmode_ctrl[3].zerop6uicp2;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL3.bit.PHY3_DETCNT_CP2	= cphy_ctrl->cpmode_ctrl[3].detcnt_cp2;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL3.bit.PHY3_EQ_EN_CP2	= cphy_ctrl->cpmode_ctrl[3].eq_en_cp2;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL3.bit.PHY3_EQ_SET_CP2	= cphy_ctrl->cpmode_ctrl[3].eq_set_cp2;

	IO_PRO.SEN.SENTOP.PHY0DLCTL.bit.PHY0_D0RXSPEED					= cphy_ctrl->delay_ctrl[0].d0rxspeed;
	IO_PRO.SEN.SENTOP.PHY0DLCTL.bit.PHY0_D1RXSPEED					= cphy_ctrl->delay_ctrl[0].d1rxspeed;
	IO_PRO.SEN.SENTOP.PHY0DLCTL.bit.PHY0_D2RXSPEED					= cphy_ctrl->delay_ctrl[0].d2rxspeed;
	IO_PRO.SEN.SENTOP.PHY0DLCTL.bit.PHY0_D3RXSPEED					= cphy_ctrl->delay_ctrl[0].d3rxspeed;
	IO_PRO.SEN.SENTOP.PHY0DLCTL.bit.PHY0_SKWSEL_CP0					= cphy_ctrl->delay_ctrl[0].skwsel_cp0;
	IO_PRO.SEN.SENTOP.PHY0DLCTL.bit.PHY0_SKWSEL_CP1					= cphy_ctrl->delay_ctrl[0].skwsel_cp1;
	IO_PRO.SEN.SENTOP.PHY0DLCTL.bit.PHY0_SKWSEL_CP2					= cphy_ctrl->delay_ctrl[0].skwsel_cp2;
	IO_PRO.SEN.SENTOP.PHY1DLCTL.bit.PHY1_D0RXSPEED					= cphy_ctrl->delay_ctrl[1].d0rxspeed;
	IO_PRO.SEN.SENTOP.PHY1DLCTL.bit.PHY1_D1RXSPEED					= cphy_ctrl->delay_ctrl[1].d1rxspeed;
	IO_PRO.SEN.SENTOP.PHY1DLCTL.bit.PHY1_D2RXSPEED					= cphy_ctrl->delay_ctrl[1].d2rxspeed;
	IO_PRO.SEN.SENTOP.PHY1DLCTL.bit.PHY1_D3RXSPEED					= cphy_ctrl->delay_ctrl[1].d3rxspeed;
	IO_PRO.SEN.SENTOP.PHY1DLCTL.bit.PHY1_SKWSEL_CP0					= cphy_ctrl->delay_ctrl[1].skwsel_cp0;
	IO_PRO.SEN.SENTOP.PHY1DLCTL.bit.PHY1_SKWSEL_CP1					= cphy_ctrl->delay_ctrl[1].skwsel_cp1;
	IO_PRO.SEN.SENTOP.PHY1DLCTL.bit.PHY1_SKWSEL_CP2					= cphy_ctrl->delay_ctrl[1].skwsel_cp2;
	IO_PRO.SEN.SENTOP.PHY2DLCTL.bit.PHY2_D0RXSPEED					= cphy_ctrl->delay_ctrl[2].d0rxspeed;
	IO_PRO.SEN.SENTOP.PHY2DLCTL.bit.PHY2_D1RXSPEED					= cphy_ctrl->delay_ctrl[2].d1rxspeed;
	IO_PRO.SEN.SENTOP.PHY2DLCTL.bit.PHY2_D2RXSPEED					= cphy_ctrl->delay_ctrl[2].d2rxspeed;
	IO_PRO.SEN.SENTOP.PHY2DLCTL.bit.PHY2_D3RXSPEED					= cphy_ctrl->delay_ctrl[2].d3rxspeed;
	IO_PRO.SEN.SENTOP.PHY2DLCTL.bit.PHY2_SKWSEL_CP0					= cphy_ctrl->delay_ctrl[2].skwsel_cp0;
	IO_PRO.SEN.SENTOP.PHY2DLCTL.bit.PHY2_SKWSEL_CP1					= cphy_ctrl->delay_ctrl[2].skwsel_cp1;
	IO_PRO.SEN.SENTOP.PHY2DLCTL.bit.PHY2_SKWSEL_CP2					= cphy_ctrl->delay_ctrl[2].skwsel_cp2;
	IO_PRO.SEN.SENTOP.PHY3DLCTL.bit.PHY3_D0RXSPEED					= cphy_ctrl->delay_ctrl[3].d0rxspeed;
	IO_PRO.SEN.SENTOP.PHY3DLCTL.bit.PHY3_D1RXSPEED					= cphy_ctrl->delay_ctrl[3].d1rxspeed;
	IO_PRO.SEN.SENTOP.PHY3DLCTL.bit.PHY3_D2RXSPEED					= cphy_ctrl->delay_ctrl[3].d2rxspeed;
	IO_PRO.SEN.SENTOP.PHY3DLCTL.bit.PHY3_D3RXSPEED					= cphy_ctrl->delay_ctrl[3].d3rxspeed;
	IO_PRO.SEN.SENTOP.PHY3DLCTL.bit.PHY3_SKWSEL_CP0					= cphy_ctrl->delay_ctrl[3].skwsel_cp0;
	IO_PRO.SEN.SENTOP.PHY3DLCTL.bit.PHY3_SKWSEL_CP1					= cphy_ctrl->delay_ctrl[3].skwsel_cp1;
	IO_PRO.SEN.SENTOP.PHY3DLCTL.bit.PHY3_SKWSEL_CP2					= cphy_ctrl->delay_ctrl[3].skwsel_cp2;

	IO_PRO.SEN.SENTOP.PHY0DSKCTL.bit.PHY0_RXSPDEN					= cphy_ctrl->desknew_ctrl[0].rxspden;
	IO_PRO.SEN.SENTOP.PHY0DSKCTL.bit.PHY0_DCNTNUM 					= cphy_ctrl->desknew_ctrl[0].dcntnum;
	IO_PRO.SEN.SENTOP.PHY0DSKCTL.bit.PHY0_GRDWIDTH					= cphy_ctrl->desknew_ctrl[0].grdwidth;
	IO_PRO.SEN.SENTOP.PHY0DSKCTL.bit.PHY0_RDWAIT					= cphy_ctrl->desknew_ctrl[0].rdwait;
	IO_PRO.SEN.SENTOP.PHY0DSKCTL.bit.PHY0_VLDPRD					= cphy_ctrl->desknew_ctrl[0].vldprd;
	IO_PRO.SEN.SENTOP.PHY1DSKCTL.bit.PHY1_RXSPDEN					= cphy_ctrl->desknew_ctrl[1].rxspden;
	IO_PRO.SEN.SENTOP.PHY1DSKCTL.bit.PHY1_DCNTNUM 					= cphy_ctrl->desknew_ctrl[1].dcntnum;
	IO_PRO.SEN.SENTOP.PHY1DSKCTL.bit.PHY1_GRDWIDTH					= cphy_ctrl->desknew_ctrl[1].grdwidth;
	IO_PRO.SEN.SENTOP.PHY1DSKCTL.bit.PHY1_RDWAIT					= cphy_ctrl->desknew_ctrl[1].rdwait;
	IO_PRO.SEN.SENTOP.PHY1DSKCTL.bit.PHY1_VLDPRD					= cphy_ctrl->desknew_ctrl[1].vldprd;
	IO_PRO.SEN.SENTOP.PHY2DSKCTL.bit.PHY2_RXSPDEN					= cphy_ctrl->desknew_ctrl[2].rxspden;
	IO_PRO.SEN.SENTOP.PHY2DSKCTL.bit.PHY2_DCNTNUM 					= cphy_ctrl->desknew_ctrl[2].dcntnum;
	IO_PRO.SEN.SENTOP.PHY2DSKCTL.bit.PHY2_GRDWIDTH					= cphy_ctrl->desknew_ctrl[2].grdwidth;
	IO_PRO.SEN.SENTOP.PHY2DSKCTL.bit.PHY2_RDWAIT					= cphy_ctrl->desknew_ctrl[2].rdwait;
	IO_PRO.SEN.SENTOP.PHY2DSKCTL.bit.PHY2_VLDPRD					= cphy_ctrl->desknew_ctrl[2].vldprd;
	IO_PRO.SEN.SENTOP.PHY3DSKCTL.bit.PHY3_RXSPDEN					= cphy_ctrl->desknew_ctrl[3].rxspden;
	IO_PRO.SEN.SENTOP.PHY3DSKCTL.bit.PHY3_DCNTNUM 					= cphy_ctrl->desknew_ctrl[3].dcntnum;
	IO_PRO.SEN.SENTOP.PHY3DSKCTL.bit.PHY3_GRDWIDTH					= cphy_ctrl->desknew_ctrl[3].grdwidth;
	IO_PRO.SEN.SENTOP.PHY3DSKCTL.bit.PHY3_RDWAIT					= cphy_ctrl->desknew_ctrl[3].rdwait;
	IO_PRO.SEN.SENTOP.PHY3DSKCTL.bit.PHY3_VLDPRD					= cphy_ctrl->desknew_ctrl[3].vldprd;

	IO_PRO.SEN.SENTOP.PLL0CTL.bit.PLL0_IBCNT						= cphy_ctrl->pll0_ctrl.ibcnt;
	IO_PRO.SEN.SENTOP.PLL0CTL.bit.PLL0_ILFSEL						= cphy_ctrl->pll0_ctrl.ilfsel;
	IO_PRO.SEN.SENTOP.PLL0CTL.bit.PLL0_IOCLKEN						= cphy_ctrl->pll0_ctrl.ioclken;
	IO_PRO.SEN.SENTOP.PLL0CTL.bit.PLL0_ITXPCLK2AEN					= cphy_ctrl->pll0_ctrl.itxpclk2aen;

	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}


/**
SENTOP ComboPHY macro setting
@param[in]	clk_ctrl : ComboPHY clock control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_CPHY_Clk_Ctrl( T_IM_PRO_SENTOP_CPHY_CLK_CTRL* clk_ctrl )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.PHY0CLKCTL.bit.PHY0_CLKLANESEL	= clk_ctrl[0].clklanesel;
	IO_PRO.SEN.SENTOP.PHY0CLKCTL.bit.PHY0_CLKMODESEL	= clk_ctrl[0].clkmodesel;
	IO_PRO.SEN.SENTOP.PHY0CLKCTL.bit.PHY0_CLKOUTREN		= clk_ctrl[0].clkoutren;
	IO_PRO.SEN.SENTOP.PHY0CLKCTL.bit.PHY0_CLKOUTLEN		= clk_ctrl[0].clkoutlen;
	IO_PRO.SEN.SENTOP.PHY1CLKCTL.bit.PHY1_CLKLANESEL	= clk_ctrl[1].clklanesel;
	IO_PRO.SEN.SENTOP.PHY1CLKCTL.bit.PHY1_CLKMODESEL	= clk_ctrl[1].clkmodesel;
	IO_PRO.SEN.SENTOP.PHY1CLKCTL.bit.PHY1_CLKOUTREN		= clk_ctrl[1].clkoutren;
	IO_PRO.SEN.SENTOP.PHY1CLKCTL.bit.PHY1_CLKOUTLEN		= clk_ctrl[1].clkoutlen;
	IO_PRO.SEN.SENTOP.PHY2CLKCTL.bit.PHY2_CLKLANESEL	= clk_ctrl[2].clklanesel;
	IO_PRO.SEN.SENTOP.PHY2CLKCTL.bit.PHY2_CLKMODESEL	= clk_ctrl[2].clkmodesel;
	IO_PRO.SEN.SENTOP.PHY2CLKCTL.bit.PHY2_CLKOUTREN		= clk_ctrl[2].clkoutren;
	IO_PRO.SEN.SENTOP.PHY2CLKCTL.bit.PHY2_CLKOUTLEN		= clk_ctrl[2].clkoutlen;
	IO_PRO.SEN.SENTOP.PHY3CLKCTL.bit.PHY3_CLKLANESEL	= clk_ctrl[3].clklanesel;
	IO_PRO.SEN.SENTOP.PHY3CLKCTL.bit.PHY3_CLKMODESEL	= clk_ctrl[3].clkmodesel;
	IO_PRO.SEN.SENTOP.PHY3CLKCTL.bit.PHY3_CLKOUTREN		= clk_ctrl[3].clkoutren;
	IO_PRO.SEN.SENTOP.PHY3CLKCTL.bit.PHY3_CLKOUTLEN		= clk_ctrl[3].clkoutlen;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
SENTOP ComboPHY macro Bias setting
@param[in]	bias_ctrl : ComboPHY Bias control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_CPHY_Bias_Ctrl( T_IM_PRO_SENTOP_CPHY_BIAS_CTRL* bias_ctrl )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.BIASCTL.bit.PHY0_XBGRPD	= bias_ctrl->phy0_xbgrpd;
	IO_PRO.SEN.SENTOP.BIASCTL.bit.PHY1_XBGRPD	= bias_ctrl->phy1_xbgrpd;
	IO_PRO.SEN.SENTOP.BIASCTL.bit.PHY2_XBGRPD	= bias_ctrl->phy2_xbgrpd;
	IO_PRO.SEN.SENTOP.BIASCTL.bit.PHY3_XBGRPD	= bias_ctrl->phy3_xbgrpd;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
SENTOP ComboPHY SLVS-EC mode setting
@param[in]	mode_ctrl : ComboPHY mode control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_CPHY_Slvs_Mode_Ctrl( T_IM_PRO_SENTOP_CPHY_SLVS_MODE_CTRL* mode_ctrl )
{
	im_pro_sentop_set_cphy_common_ctrl( 0, &mode_ctrl[0].common_ctrl );
	im_pro_sentop_set_cphy_common_ctrl( 1, &mode_ctrl[1].common_ctrl );
	im_pro_sentop_set_cphy_common_ctrl( 2, &mode_ctrl[2].common_ctrl );
	im_pro_sentop_set_cphy_common_ctrl( 3, &mode_ctrl[3].common_ctrl );

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.PHY0MDCTL.bit.PHY0_PIMODE	= mode_ctrl[0].pimode;
	IO_PRO.SEN.SENTOP.PHY0MDCTL.bit.PHY0_PICNT	= mode_ctrl[0].picnt;
	IO_PRO.SEN.SENTOP.PHY1MDCTL.bit.PHY1_PIMODE	= mode_ctrl[1].pimode;
	IO_PRO.SEN.SENTOP.PHY1MDCTL.bit.PHY1_PICNT	= mode_ctrl[1].picnt;
	IO_PRO.SEN.SENTOP.PHY2MDCTL.bit.PHY2_PIMODE	= mode_ctrl[2].pimode;
	IO_PRO.SEN.SENTOP.PHY2MDCTL.bit.PHY2_PICNT	= mode_ctrl[2].picnt;
	IO_PRO.SEN.SENTOP.PHY3MDCTL.bit.PHY3_PIMODE	= mode_ctrl[3].pimode;
	IO_PRO.SEN.SENTOP.PHY3MDCTL.bit.PHY3_PICNT	= mode_ctrl[3].picnt;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
SENTOP ComboPHY Sub-LVDS mode setting
@param[in]	mode_ctrl : ComboPHY mode control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_CPHY_Lvds_Mode_Ctrl( T_IM_PRO_SENTOP_CPHY_LVDS_MODE_CTRL* mode_ctrl )
{
	im_pro_sentop_set_cphy_common_ctrl( 0, &mode_ctrl[0].common_ctrl );
	im_pro_sentop_set_cphy_common_ctrl( 1, &mode_ctrl[1].common_ctrl );
	im_pro_sentop_set_cphy_common_ctrl( 2, &mode_ctrl[2].common_ctrl );
	im_pro_sentop_set_cphy_common_ctrl( 3, &mode_ctrl[3].common_ctrl );

	return D_DDIM_OK;
}

/**
SENTOP ComboPHY MIPI D-PHY mode setting
@param[in]	mode_ctrl : ComboPHY mode control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_CPHY_DMIPI_Mode_Ctrl( T_IM_PRO_SENTOP_CPHY_DMIPI_MODE_CTRL* mode_ctrl )
{
	im_pro_sentop_set_cphy_common_ctrl( 0, &mode_ctrl[0].common_ctrl );
	im_pro_sentop_set_cphy_common_ctrl( 1, &mode_ctrl[1].common_ctrl );
	im_pro_sentop_set_cphy_common_ctrl( 2, &mode_ctrl[2].common_ctrl );
	im_pro_sentop_set_cphy_common_ctrl( 3, &mode_ctrl[3].common_ctrl );

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.PHY0DSKCTL.bit.PHY0_RXSPDEN	= mode_ctrl[0].rxspden;
	IO_PRO.SEN.SENTOP.PHY0DSKCTL.bit.PHY0_DCNTNUM 	= mode_ctrl[0].dcntnum;
	IO_PRO.SEN.SENTOP.PHY0DSKCTL.bit.PHY0_GRDWIDTH	= mode_ctrl[0].grdwidth;
	IO_PRO.SEN.SENTOP.PHY0DSKCTL.bit.PHY0_RDWAIT	= mode_ctrl[0].rdwait;
	IO_PRO.SEN.SENTOP.PHY0DSKCTL.bit.PHY0_VLDPRD	= mode_ctrl[0].vldprd;
	IO_PRO.SEN.SENTOP.PHY1DSKCTL.bit.PHY1_RXSPDEN	= mode_ctrl[1].rxspden;
	IO_PRO.SEN.SENTOP.PHY1DSKCTL.bit.PHY1_DCNTNUM 	= mode_ctrl[1].dcntnum;
	IO_PRO.SEN.SENTOP.PHY1DSKCTL.bit.PHY1_GRDWIDTH	= mode_ctrl[1].grdwidth;
	IO_PRO.SEN.SENTOP.PHY1DSKCTL.bit.PHY1_RDWAIT	= mode_ctrl[1].rdwait;
	IO_PRO.SEN.SENTOP.PHY1DSKCTL.bit.PHY1_VLDPRD	= mode_ctrl[1].vldprd;
	IO_PRO.SEN.SENTOP.PHY2DSKCTL.bit.PHY2_RXSPDEN	= mode_ctrl[2].rxspden;
	IO_PRO.SEN.SENTOP.PHY2DSKCTL.bit.PHY2_DCNTNUM 	= mode_ctrl[2].dcntnum;
	IO_PRO.SEN.SENTOP.PHY2DSKCTL.bit.PHY2_GRDWIDTH	= mode_ctrl[2].grdwidth;
	IO_PRO.SEN.SENTOP.PHY2DSKCTL.bit.PHY2_RDWAIT	= mode_ctrl[2].rdwait;
	IO_PRO.SEN.SENTOP.PHY2DSKCTL.bit.PHY2_VLDPRD	= mode_ctrl[2].vldprd;
	IO_PRO.SEN.SENTOP.PHY3DSKCTL.bit.PHY3_RXSPDEN	= mode_ctrl[3].rxspden;
	IO_PRO.SEN.SENTOP.PHY3DSKCTL.bit.PHY3_DCNTNUM 	= mode_ctrl[3].dcntnum;
	IO_PRO.SEN.SENTOP.PHY3DSKCTL.bit.PHY3_GRDWIDTH	= mode_ctrl[3].grdwidth;
	IO_PRO.SEN.SENTOP.PHY3DSKCTL.bit.PHY3_RDWAIT	= mode_ctrl[3].rdwait;
	IO_PRO.SEN.SENTOP.PHY3DSKCTL.bit.PHY3_VLDPRD	= mode_ctrl[3].vldprd;

//	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL4.bit.PHY0_FUNCSELD0	= mode_ctrl[0].funcseld0;
//	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL5.bit.PHY0_FUNCSELD1	= mode_ctrl[1].funcseld0;
//	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL6.bit.PHY0_FUNCSELD2	= mode_ctrl[2].funcseld0;
//	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL7.bit.PHY0_FUNCSELD3	= mode_ctrl[3].funcseld0;

	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL4.bit.PHY0_FUNCSELD0	= mode_ctrl[0].funcseld0;
	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL5.bit.PHY0_FUNCSELD1	= mode_ctrl[0].funcseld0;
	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL6.bit.PHY0_FUNCSELD2	= mode_ctrl[0].funcseld0;
	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL7.bit.PHY0_FUNCSELD3	= mode_ctrl[0].funcseld0;
	IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL4.bit.PHY1_FUNCSELD0	= mode_ctrl[1].funcseld0;
	IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL5.bit.PHY1_FUNCSELD1	= mode_ctrl[1].funcseld0;
	IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL6.bit.PHY1_FUNCSELD2	= mode_ctrl[1].funcseld0;
	IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL7.bit.PHY1_FUNCSELD3	= mode_ctrl[1].funcseld0;
	IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL4.bit.PHY2_FUNCSELD0	= mode_ctrl[2].funcseld0;
	IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL5.bit.PHY2_FUNCSELD1	= mode_ctrl[2].funcseld0;
	IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL6.bit.PHY2_FUNCSELD2	= mode_ctrl[2].funcseld0;
	IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL7.bit.PHY2_FUNCSELD3	= mode_ctrl[2].funcseld0;
	IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL4.bit.PHY3_FUNCSELD0	= mode_ctrl[3].funcseld0;
	IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL5.bit.PHY3_FUNCSELD1	= mode_ctrl[3].funcseld0;
	IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL6.bit.PHY3_FUNCSELD2	= mode_ctrl[3].funcseld0;
	IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL7.bit.PHY3_FUNCSELD3	= mode_ctrl[3].funcseld0;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
SENTOP ComboPHY MIPI C-PHY mode setting
@param[in]	mode_ctrl : ComboPHY mode control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_CPHY_CMIPI_Mode_Ctrl( T_IM_PRO_SENTOP_CPHY_CMIPI_MODE_CTRL* mode_ctrl )
{
	im_pro_sentop_set_cphy_common_ctrl( 0, &mode_ctrl[0].common_ctrl );
	im_pro_sentop_set_cphy_common_ctrl( 1, &mode_ctrl[1].common_ctrl );
	im_pro_sentop_set_cphy_common_ctrl( 2, &mode_ctrl[2].common_ctrl );
	im_pro_sentop_set_cphy_common_ctrl( 3, &mode_ctrl[3].common_ctrl );

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL1.bit.PHY0_ZEROP3UICP0	= mode_ctrl[0].zerop3uicp0;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL1.bit.PHY0_ZEROP4UICP0	= mode_ctrl[0].zerop4uicp0;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL1.bit.PHY0_ZEROP5UICP0	= mode_ctrl[0].zerop5uicp0;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL1.bit.PHY0_ZEROP6UICP0	= mode_ctrl[0].zerop6uicp0;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL1.bit.PHY0_DETCNT_CP0	= mode_ctrl[0].detcnt_cp0;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL1.bit.PHY0_EQ_EN_CP0	= mode_ctrl[0].eq_en_cp0;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL1.bit.PHY0_EQ_SET_CP0	= mode_ctrl[0].eq_set_cp0;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL2.bit.PHY0_ZEROP3UICP1	= mode_ctrl[0].zerop3uicp1;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL2.bit.PHY0_ZEROP4UICP1	= mode_ctrl[0].zerop4uicp1;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL2.bit.PHY0_ZEROP5UICP1	= mode_ctrl[0].zerop5uicp1;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL2.bit.PHY0_ZEROP6UICP1	= mode_ctrl[0].zerop6uicp1;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL2.bit.PHY0_DETCNT_CP1	= mode_ctrl[0].detcnt_cp1;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL2.bit.PHY0_EQ_EN_CP1	= mode_ctrl[0].eq_en_cp1;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL2.bit.PHY0_EQ_SET_CP1	= mode_ctrl[0].eq_set_cp1;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL3.bit.PHY0_ZEROP3UICP2	= mode_ctrl[0].zerop3uicp2;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL3.bit.PHY0_ZEROP4UICP2	= mode_ctrl[0].zerop4uicp2;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL3.bit.PHY0_ZEROP5UICP2	= mode_ctrl[0].zerop5uicp2;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL3.bit.PHY0_ZEROP6UICP2	= mode_ctrl[0].zerop6uicp2;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL3.bit.PHY0_DETCNT_CP2	= mode_ctrl[0].detcnt_cp2;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL3.bit.PHY0_EQ_EN_CP2	= mode_ctrl[0].eq_en_cp2;
	IO_PRO.SEN.SENTOP.PHY0MDCPCTL.PHY0MDCPCTL3.bit.PHY0_EQ_SET_CP2	= mode_ctrl[0].eq_set_cp2;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL1.bit.PHY1_ZEROP3UICP0	= mode_ctrl[1].zerop3uicp0;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL1.bit.PHY1_ZEROP4UICP0	= mode_ctrl[1].zerop4uicp0;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL1.bit.PHY1_ZEROP5UICP0	= mode_ctrl[1].zerop5uicp0;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL1.bit.PHY1_ZEROP6UICP0	= mode_ctrl[1].zerop6uicp0;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL1.bit.PHY1_DETCNT_CP0	= mode_ctrl[1].detcnt_cp0;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL1.bit.PHY1_EQ_EN_CP0	= mode_ctrl[1].eq_en_cp0;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL1.bit.PHY1_EQ_SET_CP0	= mode_ctrl[1].eq_set_cp0;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL2.bit.PHY1_ZEROP3UICP1	= mode_ctrl[1].zerop3uicp1;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL2.bit.PHY1_ZEROP4UICP1	= mode_ctrl[1].zerop4uicp1;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL2.bit.PHY1_ZEROP5UICP1	= mode_ctrl[1].zerop5uicp1;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL2.bit.PHY1_ZEROP6UICP1	= mode_ctrl[1].zerop6uicp1;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL2.bit.PHY1_DETCNT_CP1	= mode_ctrl[1].detcnt_cp1;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL2.bit.PHY1_EQ_EN_CP1	= mode_ctrl[1].eq_en_cp1;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL2.bit.PHY1_EQ_SET_CP1	= mode_ctrl[1].eq_set_cp1;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL3.bit.PHY1_ZEROP3UICP2	= mode_ctrl[1].zerop3uicp2;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL3.bit.PHY1_ZEROP4UICP2	= mode_ctrl[1].zerop4uicp2;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL3.bit.PHY1_ZEROP5UICP2	= mode_ctrl[1].zerop5uicp2;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL3.bit.PHY1_ZEROP6UICP2	= mode_ctrl[1].zerop6uicp2;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL3.bit.PHY1_DETCNT_CP2	= mode_ctrl[1].detcnt_cp2;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL3.bit.PHY1_EQ_SET_CP2	= mode_ctrl[1].eq_set_cp2;
	IO_PRO.SEN.SENTOP.PHY1MDCPCTL.PHY1MDCPCTL3.bit.PHY1_EQ_EN_CP2	= mode_ctrl[1].eq_en_cp2;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL1.bit.PHY2_ZEROP3UICP0	= mode_ctrl[2].zerop3uicp0;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL1.bit.PHY2_ZEROP4UICP0	= mode_ctrl[2].zerop4uicp0;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL1.bit.PHY2_ZEROP5UICP0	= mode_ctrl[2].zerop5uicp0;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL1.bit.PHY2_ZEROP6UICP0	= mode_ctrl[2].zerop6uicp0;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL1.bit.PHY2_DETCNT_CP0	= mode_ctrl[2].detcnt_cp0;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL1.bit.PHY2_EQ_EN_CP0	= mode_ctrl[2].eq_en_cp0;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL1.bit.PHY2_EQ_SET_CP0	= mode_ctrl[2].eq_set_cp0;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL2.bit.PHY2_ZEROP3UICP1	= mode_ctrl[2].zerop3uicp1;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL2.bit.PHY2_ZEROP4UICP1	= mode_ctrl[2].zerop4uicp1;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL2.bit.PHY2_ZEROP5UICP1	= mode_ctrl[2].zerop5uicp1;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL2.bit.PHY2_ZEROP6UICP1	= mode_ctrl[2].zerop6uicp1;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL2.bit.PHY2_DETCNT_CP1	= mode_ctrl[2].detcnt_cp1;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL2.bit.PHY2_EQ_EN_CP1	= mode_ctrl[2].eq_en_cp1;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL2.bit.PHY2_EQ_SET_CP1	= mode_ctrl[2].eq_set_cp1;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL3.bit.PHY2_ZEROP3UICP2	= mode_ctrl[2].zerop3uicp2;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL3.bit.PHY2_ZEROP4UICP2	= mode_ctrl[2].zerop4uicp2;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL3.bit.PHY2_ZEROP5UICP2	= mode_ctrl[2].zerop5uicp2;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL3.bit.PHY2_ZEROP6UICP2	= mode_ctrl[2].zerop6uicp2;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL3.bit.PHY2_DETCNT_CP2	= mode_ctrl[2].detcnt_cp2;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL3.bit.PHY2_EQ_EN_CP2	= mode_ctrl[2].eq_en_cp2;
	IO_PRO.SEN.SENTOP.PHY2MDCPCTL.PHY2MDCPCTL3.bit.PHY2_EQ_SET_CP2	= mode_ctrl[2].eq_set_cp2;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL1.bit.PHY3_ZEROP3UICP0	= mode_ctrl[3].zerop3uicp0;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL1.bit.PHY3_ZEROP4UICP0	= mode_ctrl[3].zerop4uicp0;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL1.bit.PHY3_ZEROP5UICP0	= mode_ctrl[3].zerop5uicp0;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL1.bit.PHY3_ZEROP6UICP0	= mode_ctrl[3].zerop6uicp0;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL1.bit.PHY3_DETCNT_CP0	= mode_ctrl[3].detcnt_cp0;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL1.bit.PHY3_EQ_EN_CP0	= mode_ctrl[3].eq_en_cp0;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL1.bit.PHY3_EQ_SET_CP0	= mode_ctrl[3].eq_set_cp0;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL2.bit.PHY3_ZEROP3UICP1	= mode_ctrl[3].zerop3uicp1;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL2.bit.PHY3_ZEROP4UICP1	= mode_ctrl[3].zerop4uicp1;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL2.bit.PHY3_ZEROP5UICP1	= mode_ctrl[3].zerop5uicp1;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL2.bit.PHY3_ZEROP6UICP1	= mode_ctrl[3].zerop6uicp1;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL2.bit.PHY3_DETCNT_CP1	= mode_ctrl[3].detcnt_cp1;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL2.bit.PHY3_EQ_EN_CP1	= mode_ctrl[3].eq_en_cp1;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL2.bit.PHY3_EQ_SET_CP1	= mode_ctrl[3].eq_set_cp1;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL3.bit.PHY3_ZEROP3UICP2	= mode_ctrl[3].zerop3uicp2;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL3.bit.PHY3_ZEROP4UICP2	= mode_ctrl[3].zerop4uicp2;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL3.bit.PHY3_ZEROP5UICP2	= mode_ctrl[3].zerop5uicp2;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL3.bit.PHY3_ZEROP6UICP2	= mode_ctrl[3].zerop6uicp2;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL3.bit.PHY3_DETCNT_CP2	= mode_ctrl[3].detcnt_cp2;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL3.bit.PHY3_EQ_EN_CP2	= mode_ctrl[3].eq_en_cp2;
	IO_PRO.SEN.SENTOP.PHY3MDCPCTL.PHY3MDCPCTL3.bit.PHY3_EQ_SET_CP2	= mode_ctrl[3].eq_set_cp2;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
SENTOP ComboPHY Extension Signals setting
@param[in]	ext_signal : ComboPHY extension signals control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_CPHY_Ext_Signal_Ctrl( T_IM_PRO_SENTOP_CPHY_IF_EXT_SIGNAL_CTRL* ext_signal )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL1.bit.PHY0_CLKSETSEL	= ext_signal[0].clksetsel;
	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL2.bit.PHY0_CPHYSET_CP0	= ext_signal[0].cphyset_cp0;
	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL2.bit.PHY0_CPHYSET_CP1	= ext_signal[0].cphyset_cp1;
	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL2.bit.PHY0_CPHYSET_CP2	= ext_signal[0].cphyset_cp2;
	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL3.bit.PHY0_FUNCSELCK	= ext_signal[0].funcselck;
	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL4.bit.PHY0_FUNCSELD0	= ext_signal[0].funcseld0;
	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL5.bit.PHY0_FUNCSELD1	= ext_signal[0].funcseld1;
	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL6.bit.PHY0_FUNCSELD2	= ext_signal[0].funcseld2;
	IO_PRO.SEN.SENTOP.PHY0IFCTL.PHY0IFCTL7.bit.PHY0_FUNCSELD3	= ext_signal[0].funcseld3;
	IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL1.bit.PHY1_CLKSETSEL	= ext_signal[1].clksetsel;
	IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL2.bit.PHY1_CPHYSET_CP0	= ext_signal[1].cphyset_cp0;
	IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL2.bit.PHY1_CPHYSET_CP1	= ext_signal[1].cphyset_cp1;
	IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL2.bit.PHY1_CPHYSET_CP2	= ext_signal[1].cphyset_cp2;
	IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL3.bit.PHY1_FUNCSELCK	= ext_signal[1].funcselck;
	IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL4.bit.PHY1_FUNCSELD0	= ext_signal[1].funcseld0;
	IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL5.bit.PHY1_FUNCSELD1	= ext_signal[1].funcseld1;
	IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL6.bit.PHY1_FUNCSELD2	= ext_signal[1].funcseld2;
	IO_PRO.SEN.SENTOP.PHY1IFCTL.PHY1IFCTL7.bit.PHY1_FUNCSELD3	= ext_signal[1].funcseld3;
	IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL1.bit.PHY2_CLKSETSEL	= ext_signal[2].clksetsel;
	IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL2.bit.PHY2_CPHYSET_CP0	= ext_signal[2].cphyset_cp0;
	IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL2.bit.PHY2_CPHYSET_CP1	= ext_signal[2].cphyset_cp1;
	IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL2.bit.PHY2_CPHYSET_CP2	= ext_signal[2].cphyset_cp2;
	IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL3.bit.PHY2_FUNCSELCK	= ext_signal[2].funcselck;
	IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL4.bit.PHY2_FUNCSELD0	= ext_signal[2].funcseld0;
	IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL5.bit.PHY2_FUNCSELD1	= ext_signal[2].funcseld1;
	IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL6.bit.PHY2_FUNCSELD2	= ext_signal[2].funcseld2;
	IO_PRO.SEN.SENTOP.PHY2IFCTL.PHY2IFCTL7.bit.PHY2_FUNCSELD3	= ext_signal[2].funcseld3;
	IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL1.bit.PHY3_CLKSETSEL	= ext_signal[3].clksetsel;
	IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL2.bit.PHY3_CPHYSET_CP0	= ext_signal[3].cphyset_cp0;
	IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL2.bit.PHY3_CPHYSET_CP1	= ext_signal[3].cphyset_cp1;
	IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL2.bit.PHY3_CPHYSET_CP2	= ext_signal[3].cphyset_cp2;
	IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL3.bit.PHY3_FUNCSELCK	= ext_signal[3].funcselck;
	IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL4.bit.PHY3_FUNCSELD0	= ext_signal[3].funcseld0;
	IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL5.bit.PHY3_FUNCSELD1	= ext_signal[3].funcseld1;
	IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL6.bit.PHY3_FUNCSELD2	= ext_signal[3].funcseld2;
	IO_PRO.SEN.SENTOP.PHY3IFCTL.PHY3IFCTL7.bit.PHY3_FUNCSELD3	= ext_signal[3].funcseld3;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get CPHY DesKnew Monitor pin status.
@param[out]	cphy_moni : CPHY DesKnew Monitor pin status information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_CPHY_Get_DesKnew_Monitor( T_IM_PRO_SENTOP_CPHY_DESKNEW_MONITOR* cphy_moni )
{
#ifdef CO_PARAM_CHECK
	if (cphy_moni == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_CPHY_Get_DesKnew_Monitor error. cphy_moni=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	cphy_moni->desknew_moni[0].dtypemon0	= IO_PRO.SEN.SENTOP.PHY0DSKMONI.PHY0DSKMONI1.bit.PHY0_DTYPEMON0;
	cphy_moni->desknew_moni[0].dtypemon1	= IO_PRO.SEN.SENTOP.PHY0DSKMONI.PHY0DSKMONI1.bit.PHY0_DTYPEMON1;
	cphy_moni->desknew_moni[0].dtypemon2	= IO_PRO.SEN.SENTOP.PHY0DSKMONI.PHY0DSKMONI1.bit.PHY0_DTYPEMON2;
	cphy_moni->desknew_moni[0].dtypemon3	= IO_PRO.SEN.SENTOP.PHY0DSKMONI.PHY0DSKMONI1.bit.PHY0_DTYPEMON3;
	cphy_moni->desknew_moni[0].vldwindow0	= IO_PRO.SEN.SENTOP.PHY0DSKMONI.PHY0DSKMONI1.bit.PHY0_VLDWINDOW0;
	cphy_moni->desknew_moni[0].vldwindow1	= IO_PRO.SEN.SENTOP.PHY0DSKMONI.PHY0DSKMONI1.bit.PHY0_VLDWINDOW1;
	cphy_moni->desknew_moni[0].vldwindow2	= IO_PRO.SEN.SENTOP.PHY0DSKMONI.PHY0DSKMONI2.bit.PHY0_VLDWINDOW2;
	cphy_moni->desknew_moni[0].vldwindow3	= IO_PRO.SEN.SENTOP.PHY0DSKMONI.PHY0DSKMONI2.bit.PHY0_VLDWINDOW3;
	cphy_moni->desknew_moni[1].dtypemon0	= IO_PRO.SEN.SENTOP.PHY1DSKMONI.PHY1DSKMONI1.bit.PHY1_DTYPEMON0;
	cphy_moni->desknew_moni[1].dtypemon1	= IO_PRO.SEN.SENTOP.PHY1DSKMONI.PHY1DSKMONI1.bit.PHY1_DTYPEMON1;
	cphy_moni->desknew_moni[1].dtypemon2	= IO_PRO.SEN.SENTOP.PHY1DSKMONI.PHY1DSKMONI1.bit.PHY1_DTYPEMON2;
	cphy_moni->desknew_moni[1].dtypemon3	= IO_PRO.SEN.SENTOP.PHY1DSKMONI.PHY1DSKMONI1.bit.PHY1_DTYPEMON3;
	cphy_moni->desknew_moni[1].vldwindow0	= IO_PRO.SEN.SENTOP.PHY1DSKMONI.PHY1DSKMONI1.bit.PHY1_VLDWINDOW0;
	cphy_moni->desknew_moni[1].vldwindow1	= IO_PRO.SEN.SENTOP.PHY1DSKMONI.PHY1DSKMONI1.bit.PHY1_VLDWINDOW1;
	cphy_moni->desknew_moni[1].vldwindow2	= IO_PRO.SEN.SENTOP.PHY1DSKMONI.PHY1DSKMONI2.bit.PHY1_VLDWINDOW2;
	cphy_moni->desknew_moni[1].vldwindow3	= IO_PRO.SEN.SENTOP.PHY1DSKMONI.PHY1DSKMONI2.bit.PHY1_VLDWINDOW3;
	cphy_moni->desknew_moni[2].dtypemon0	= IO_PRO.SEN.SENTOP.PHY2DSKMONI.PHY2DSKMONI1.bit.PHY2_DTYPEMON0;
	cphy_moni->desknew_moni[2].dtypemon1	= IO_PRO.SEN.SENTOP.PHY2DSKMONI.PHY2DSKMONI1.bit.PHY2_DTYPEMON1;
	cphy_moni->desknew_moni[2].dtypemon2	= IO_PRO.SEN.SENTOP.PHY2DSKMONI.PHY2DSKMONI1.bit.PHY2_DTYPEMON2;
	cphy_moni->desknew_moni[2].dtypemon3	= IO_PRO.SEN.SENTOP.PHY2DSKMONI.PHY2DSKMONI1.bit.PHY2_DTYPEMON3;
	cphy_moni->desknew_moni[2].vldwindow0	= IO_PRO.SEN.SENTOP.PHY2DSKMONI.PHY2DSKMONI1.bit.PHY2_VLDWINDOW0;
	cphy_moni->desknew_moni[2].vldwindow1	= IO_PRO.SEN.SENTOP.PHY2DSKMONI.PHY2DSKMONI1.bit.PHY2_VLDWINDOW1;
	cphy_moni->desknew_moni[2].vldwindow2	= IO_PRO.SEN.SENTOP.PHY2DSKMONI.PHY2DSKMONI2.bit.PHY2_VLDWINDOW2;
	cphy_moni->desknew_moni[2].vldwindow3	= IO_PRO.SEN.SENTOP.PHY2DSKMONI.PHY2DSKMONI2.bit.PHY2_VLDWINDOW3;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get CPHY DelayLine Monitor pin status.
@param[out]	cphy_moni : CPHY DelayLine Monitor pin status information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_CPHY_Get_DelayLine_Monitor( T_IM_PRO_SENTOP_CPHY_DELAYLINE_MONITOR* cphy_moni )
{
#ifdef CO_PARAM_CHECK
	if (cphy_moni == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_CPHY_Get_DelayLine_Monitor error. cphy_moni=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	cphy_moni->delayline_moni[0].phy_code_cp0	= IO_PRO.SEN.SENTOP.PHY0DLMONI.PHY0DLMONI1.bit.PHY0_CODE_CP0;
	cphy_moni->delayline_moni[0].phy_code_cp1	= IO_PRO.SEN.SENTOP.PHY0DLMONI.PHY0DLMONI2.bit.PHY0_CODE_CP1;
	cphy_moni->delayline_moni[0].phy_code_cp2	= IO_PRO.SEN.SENTOP.PHY0DLMONI.PHY0DLMONI3.bit.PHY0_CODE_CP2;
	cphy_moni->delayline_moni[1].phy_code_cp0	= IO_PRO.SEN.SENTOP.PHY1DLMONI.PHY1DLMONI1.bit.PHY1_CODE_CP0;
	cphy_moni->delayline_moni[1].phy_code_cp1	= IO_PRO.SEN.SENTOP.PHY1DLMONI.PHY1DLMONI2.bit.PHY1_CODE_CP1;
	cphy_moni->delayline_moni[1].phy_code_cp2	= IO_PRO.SEN.SENTOP.PHY1DLMONI.PHY1DLMONI3.bit.PHY1_CODE_CP2;
	cphy_moni->delayline_moni[2].phy_code_cp0	= IO_PRO.SEN.SENTOP.PHY2DLMONI.PHY2DLMONI1.bit.PHY2_CODE_CP0;
	cphy_moni->delayline_moni[2].phy_code_cp1	= IO_PRO.SEN.SENTOP.PHY2DLMONI.PHY2DLMONI2.bit.PHY2_CODE_CP1;
	cphy_moni->delayline_moni[2].phy_code_cp2	= IO_PRO.SEN.SENTOP.PHY2DLMONI.PHY2DLMONI3.bit.PHY2_CODE_CP2;
	cphy_moni->delayline_moni[3].phy_code_cp0	= IO_PRO.SEN.SENTOP.PHY3DLMONI.PHY3DLMONI1.bit.PHY3_CODE_CP0;
	cphy_moni->delayline_moni[3].phy_code_cp1	= IO_PRO.SEN.SENTOP.PHY3DLMONI.PHY3DLMONI2.bit.PHY3_CODE_CP1;
	cphy_moni->delayline_moni[3].phy_code_cp2	= IO_PRO.SEN.SENTOP.PHY3DLMONI.PHY3DLMONI3.bit.PHY3_CODE_CP2;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get CPHY Common I/F Monitor pin status.
@param[out]	cphy_moni : CPHY Common I/F Monitor pin status information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SENTOP_CPHY_Get_Common_If_Monitor( T_IM_PRO_SENTOP_CPHY_COMMON_IF_MONITOR* cphy_moni )
{
#ifdef CO_PARAM_CHECK
	if (cphy_moni == NULL){
		Ddim_Assertion(("I:Im_PRO_SENTOP_CPHY_Get_DelayLine_Monitor error. cphy_moni=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	cphy_moni->phy0_cdrmon	= IO_PRO.SEN.SENTOP.PHYCMONI.PHYCMONI1.bit.PHY0_CDRMON;
	cphy_moni->phy1_cdrmon	= IO_PRO.SEN.SENTOP.PHYCMONI.PHYCMONI2.bit.PHY1_CDRMON;
	cphy_moni->phy2_cdrmon	= IO_PRO.SEN.SENTOP.PHYCMONI.PHYCMONI3.bit.PHY2_CDRMON;
	cphy_moni->phy3_cdrmon	= IO_PRO.SEN.SENTOP.PHYCMONI.PHYCMONI4.bit.PHY3_CDRMON;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
The SG operation(Trigger) started.
@param[in]	ch : SG ch number
*/
INT32 Im_PRO_SG_Start( E_IM_PRO_SG_CH ch )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	switch( ch ) {
		case E_IM_PRO_SG_CH_0:
			if ( IO_PRO.SEN.SENTOP.CLKSTP.CLKSTP1.bit.PSTP2 != 0 ) {
				Im_PRO_SENTOP_Control_Clock( E_IM_PRO_SENTOP_CLK_TYPE_APCLK0 , 0, 1 );
			}
			break;

		case E_IM_PRO_SG_CH_1:
			if ( IO_PRO.SEN.SENTOP.CLKSTP.CLKSTP1.bit.PSTP3 != 0 ) {
				Im_PRO_SENTOP_Control_Clock( E_IM_PRO_SENTOP_CLK_TYPE_APCLK1 , 0, 1 );
			}
			break;

		case E_IM_PRO_SG_CH_2:
			if ( IO_PRO.SEN.SENTOP.CLKSTP.CLKSTP1.bit.PSTP4 != 0 ) {
				Im_PRO_SENTOP_Control_Clock( E_IM_PRO_SENTOP_CLK_TYPE_APCLK2 , 0, 1 );
			}
			break;

		case E_IM_PRO_SG_CH_3:
			if ( IO_PRO.SEN.SENTOP.CLKSTP.CLKSTP2.bit.PSTP5 != 0 ) {
				Im_PRO_SENTOP_Control_Clock( E_IM_PRO_SENTOP_CLK_TYPE_APCLK3 , 0, 1 );
			}
			break;

		default:
			// invalid ch value
			Ddim_Assertion(("I:Im_PRO_SG_Start ch value error!! (%d)\n", ch));
			return D_IM_PRO_INPUT_PARAM_ERROR;
	}

	IO_PRO.SEN.SG[ch].SGTRG.bit.SGTRG = D_IM_PRO_TRG_START;		// start

	im_pro_sen_set_start_status(D_IM_SEN_STATUS_SG0, ch);
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
The SG operation(Trigger) stopped(Frame).
@param[in]	ch : SG ch number
@param[in]	force : force stop option
*/
INT32 Im_PRO_SG_Stop( E_IM_PRO_SG_CH ch, UCHAR force )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	if (force == 0){
		IO_PRO.SEN.SG[ch].SGTRG.bit.SGTRG = D_IM_PRO_SG_TRG_FRAME_STOP;		// stop
	}
	else{
		IO_PRO.SEN.SG[ch].SGTRG.bit.SGTRG = D_IM_PRO_SG_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	im_pro_sen_set_stop_status(D_IM_SEN_STATUS_SG0, ch);
	return D_DDIM_OK;
}

/**
The SG operation condition is set.
@param[in]	ch : SG ch number
@param[in]	sg_ctrl : SG Control info
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SG_Ctrl( E_IM_PRO_SG_CH ch, T_IM_PRO_SG_CTRL* sg_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (sg_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SG_Ctrl error. ch=%u sg_ctrl=NULL\n", ch));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( sg_ctrl->vdocyc < D_IM_PRO_SG_VDOCYC_MIN ) {
		Ddim_Assertion(("I:Im_PRO_SG_Ctrl. vdocyc value error!! ch=%u (%lu)\n", ch, sg_ctrl->vdocyc));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( sg_ctrl->hdocyc > D_IM_PRO_SG_HDOCYC_MAX ) {
		Ddim_Assertion(("I:Im_PRO_SG_Ctrl. hdocyc value error!! ch=%u (%u)\n", ch, sg_ctrl->hdocyc));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( sg_ctrl->hdocyc2 > D_IM_PRO_SG_HDOCYC_MAX ) {
		Ddim_Assertion(("I:Im_PRO_SG_Ctrl. hdocyc2 value error!! ch=%u (%u)\n", ch, sg_ctrl->hdocyc2));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( sg_ctrl->hdow > D_IM_PRO_SG_HDOW_MAX ) {
		Ddim_Assertion(("I:Im_PRO_SG_Ctrl. hdow value error!! ch=%u (%u)\n", ch, sg_ctrl->hdow));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( sg_ctrl->hdow2 > D_IM_PRO_SG_HDOW_MAX ) {
		Ddim_Assertion(("I:Im_PRO_SG_Ctrl. hdow2 value error!! ch=%u (%u)\n", ch, sg_ctrl->hdow2));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( sg_ctrl->vdow > D_IM_PRO_SG_VDOW_MAX ) {
		Ddim_Assertion(("I:Im_PRO_SG_Ctrl. vdow value error!! ch=%u (%u)\n", ch, sg_ctrl->vdow));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( sg_ctrl->vdow >= ( sg_ctrl->vdocyc - 1 ) ) {
		Ddim_Assertion(("I:Im_PRO_SG_Ctrl. vdow value error!! ch=%u (%u)\n", ch, sg_ctrl->vdow));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( sg_ctrl->hdow >= sg_ctrl->hdocyc ) {
		Ddim_Assertion(("I:Im_PRO_SG_Ctrl. hdow value error!! Please set as [hdow < hdocyc]. ch=%u (hdow(%u),hdocyc(%u)\n", ch, sg_ctrl->hdow, sg_ctrl->hdocyc));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( sg_ctrl->hdow2 >= sg_ctrl->hdocyc2 ) {
		Ddim_Assertion(("I:Im_PRO_SG_Ctrl. hdow2 value error!! Please set as [hdow2 < hdocyc2]. ch=%u (hdow2(%u),hdocyc2(%u)\n", ch, sg_ctrl->hdow2, sg_ctrl->hdocyc2));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( sg_ctrl->vddly > D_IM_PRO_SG_VDDLY_MAX ) {
		Ddim_Assertion(("I:Im_PRO_SG_Ctrl. vddly value error!! ch=%u (%u)\n", ch, sg_ctrl->vddly));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SG[ch].SGCTL1.bit.VHDO		= sg_ctrl->vhdo;
	IO_PRO.SEN.SG[ch].SGCTL1.bit.VDOT		= sg_ctrl->vdot;
	IO_PRO.SEN.SG[ch].SGCTL1.bit.VDINV		= sg_ctrl->vdinv;
	IO_PRO.SEN.SG[ch].SGCTL1.bit.HDINV		= sg_ctrl->hdinv;
	IO_PRO.SEN.SG[ch].SGCTL2.bit.HDCHG		= sg_ctrl->hdchg;
	IO_PRO.SEN.SG[ch].SGCTL2.bit.VDDLY		= sg_ctrl->vddly;
	IO_PRO.SEN.SG[ch].VDOCYC.bit.VDOCYC		= sg_ctrl->vdocyc;
	IO_PRO.SEN.SG[ch].VDOW.bit.VDOW			= sg_ctrl->vdow;
	IO_PRO.SEN.SG[ch].HDOCYC.bit.HDOCYC		= sg_ctrl->hdocyc;
	IO_PRO.SEN.SG[ch].HDOW.bit.HDOW			= sg_ctrl->hdow;
	IO_PRO.SEN.SG[ch].HDOCYC2.bit.HDOCYC2	= sg_ctrl->hdocyc2;
	IO_PRO.SEN.SG[ch].HDOW2.bit.HDOW2		= sg_ctrl->hdow2;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
The SG operation condition is get.
@param[in]	ch : SG ch number
@param[out]	sg_ctrl : SG Control info
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SG_Get_Cycle( E_IM_PRO_SG_CH ch, T_IM_PRO_SG_CTRL* sg_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (sg_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SG_Get_Cycle error. ch=%d sg_ctrl=NULL\n", ch));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	sg_ctrl->vhdo = 	IO_PRO.SEN.SG[ch].SGCTL1.bit.VHDO;
	sg_ctrl->vdot = 	IO_PRO.SEN.SG[ch].SGCTL1.bit.VDOT;
	sg_ctrl->vdinv = 	IO_PRO.SEN.SG[ch].SGCTL1.bit.VDINV;
	sg_ctrl->hdinv = 	IO_PRO.SEN.SG[ch].SGCTL1.bit.HDINV;
	sg_ctrl->hdchg = 	IO_PRO.SEN.SG[ch].SGCTL2.bit.HDCHG;
	sg_ctrl->vddly = 	IO_PRO.SEN.SG[ch].SGCTL2.bit.VDDLY;
	sg_ctrl->vdocyc = 	IO_PRO.SEN.SG[ch].VDOCYC.bit.VDOCYC;
	sg_ctrl->vdow = 	IO_PRO.SEN.SG[ch].VDOW.bit.VDOW;
	sg_ctrl->hdocyc = 	IO_PRO.SEN.SG[ch].HDOCYC.bit.HDOCYC;
	sg_ctrl->hdow = 	IO_PRO.SEN.SG[ch].HDOW.bit.HDOW;
	sg_ctrl->hdocyc2 = 	IO_PRO.SEN.SG[ch].HDOCYC2.bit.HDOCYC2;
	sg_ctrl->hdow2 = 	IO_PRO.SEN.SG[ch].HDOW2.bit.HDOW2;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
Get monitoring result of the input VD and HD signals.
@param[in]	ch : SG ch number
@param[out]	sg_moni_info : SG monitor information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_SG_Get_Monitor_Info( E_IM_PRO_SG_CH ch, T_IM_PRO_SG_MONI_INFO* sg_moni_info )
{
#ifdef CO_PARAM_CHECK
	if (sg_moni_info == NULL){
		Ddim_Assertion(("I:Im_PRO_SG_Get_Monitor_Info error. ch=%d sg_moni_info=NULL\n", ch));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	sg_moni_info->input_vd_signal	= IO_PRO.SEN.SG[ch].VDHDMON.bit.VDIN;
	sg_moni_info->input_hd_signal	= IO_PRO.SEN.SG[ch].VDHDMON.bit.HDIN;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}


/**
Software reset of SLVS-EC RX macro(excluding PMA, termination resistor adjustment circuit and CNT block).
@param[in]	stream_type : stream type of SLVS
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
INT32 Im_PRO_SLVS_SR( E_IM_PRO_SLVS_STREAM_TYPE stream_type )
{
#ifdef CO_PARAM_CHECK
	if( stream_type > E_IM_PRO_SLVS_STREAM_TYPE_BOTH ) {
		Ddim_Assertion(("I:Im_PRO_SLVS_SR stream type illegal. stream_type=%d\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	switch( stream_type ) {
		case E_IM_PRO_SLVS_STREAM_TYPE_A:
			IO_PRO.SEN.SLVSEC.COMMON.SR.bit.INITA	= D_IM_PRO_SLVS_SW_RESET;
			break;

		case E_IM_PRO_SLVS_STREAM_TYPE_B:
			IO_PRO.SEN.SLVSEC.COMMON.SR.bit.INITB	= D_IM_PRO_SLVS_SW_RESET;
			break;

		//case E_IM_PRO_SLVS_STREAM_TYPE_BOTH:
		default:
			IO_PRO.SEN.SLVSEC.COMMON.SR.bit.INITA	= D_IM_PRO_SLVS_SW_RESET;
			IO_PRO.SEN.SLVSEC.COMMON.SR.bit.INITB	= D_IM_PRO_SLVS_SW_RESET;
			break;
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	return D_DDIM_OK;
}

/**
Update of the SLVS-EC configuration parameters.
@param[in]	stream_type : stream type of SLVS
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
INT32 Im_PRO_SLVS_Update_Cfg( E_IM_PRO_SLVS_STREAM_TYPE stream_type )
{
#ifdef CO_PARAM_CHECK
	if( stream_type > E_IM_PRO_SLVS_STREAM_TYPE_BOTH ) {
		Ddim_Assertion(("I:Im_PRO_SLVS_SR_Rls_And_Update_Cfg stream type illegal. stream_type=%d\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	switch( stream_type ) {
		case E_IM_PRO_SLVS_STREAM_TYPE_A:
			IO_PRO.SEN.SLVSEC.COMMON.SR.bit.UPDA	= D_IM_PRO_SLVS_CFG_UPDATE;
			break;

		case E_IM_PRO_SLVS_STREAM_TYPE_B:
			IO_PRO.SEN.SLVSEC.COMMON.SR.bit.UPDB	= D_IM_PRO_SLVS_CFG_UPDATE;
			break;

		//case E_IM_PRO_SLVS_STREAM_TYPE_BOTH:
		default:
			IO_PRO.SEN.SLVSEC.COMMON.SR.bit.UPDA	= D_IM_PRO_SLVS_CFG_UPDATE;
			IO_PRO.SEN.SLVSEC.COMMON.SR.bit.UPDB	= D_IM_PRO_SLVS_CFG_UPDATE;
			break;
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	return D_DDIM_OK;
}

/**
Software reset release of SLVS-EC.
@param[in]	stream_type : stream type of SLVS
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
INT32 Im_PRO_SLVS_SR_Rlease( E_IM_PRO_SLVS_STREAM_TYPE stream_type )
{
#ifdef CO_PARAM_CHECK
	if( stream_type > E_IM_PRO_SLVS_STREAM_TYPE_BOTH ) {
		Ddim_Assertion(("I:Im_PRO_SLVS_SR_Rls_And_Update_Cfg stream type illegal. stream_type=%d\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	switch( stream_type ) {
		case E_IM_PRO_SLVS_STREAM_TYPE_A:
			IO_PRO.SEN.SLVSEC.COMMON.SR.bit.INITA	= D_IM_PRO_SLVS_SW_RESET_RELEASE;
			break;

		case E_IM_PRO_SLVS_STREAM_TYPE_B:
			IO_PRO.SEN.SLVSEC.COMMON.SR.bit.INITB	= D_IM_PRO_SLVS_SW_RESET_RELEASE;
			break;

		//case E_IM_PRO_SLVS_STREAM_TYPE_BOTH:
		default:
			IO_PRO.SEN.SLVSEC.COMMON.SR.bit.INITA	= D_IM_PRO_SLVS_SW_RESET_RELEASE;
			IO_PRO.SEN.SLVSEC.COMMON.SR.bit.INITB	= D_IM_PRO_SLVS_SW_RESET_RELEASE;
			break;
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	return D_DDIM_OK;
}


/**
Physical Media Attachment initialization.
@param[in]	pma_init : PMA initialization parameters.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NG					: Processing NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
INT32 Im_PRO_SLVS_Pma_Init( T_IM_PRO_SLVS_PMA_INIT* pma_init )
{
#ifdef CO_PARAM_CHECK
	if (pma_init == NULL){
		Ddim_Assertion(("I:Im_PRO_SLVS_Pma_Init error. pma_init=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SLVSEC.COMMON.PHYCR2.bit.PDX			= E_IM_PRO_SLVS_PD_STATE_NORMAL;
	im_pro_wait_usec( D_IM_PRO_SLVS_INIT_WAIT_PD );

	IO_PRO.SEN.SLVSEC.COMMON.PHYCR1.bit.INVDATA		= ( pma_init->invert_sel.lane0 << 0 |
														pma_init->invert_sel.lane1 << 1 |
														pma_init->invert_sel.lane2 << 2 |
														pma_init->invert_sel.lane3 << 3 |
														pma_init->invert_sel.lane4 << 4 |
														pma_init->invert_sel.lane5 << 5 |
														pma_init->invert_sel.lane6 << 6 |
														pma_init->invert_sel.lane7 << 7 );
	IO_PRO.SEN.SLVSEC.COMMON.PHYCR1.bit.IFVDCNT		= pma_init->multiplication_factor;

	if( pma_init->opt_init_param.enabled != D_IM_PRO_SLVS_PMA_OPT_INIT_DIS ) {
		IO_PRO.SEN.SLVSEC.COMMON.PHYCR0.bit.REFSEL	= pma_init->opt_init_param.refsel;
		IO_PRO.SEN.SLVSEC.COMMON.PHYCR0.bit.ICPCODE	= pma_init->opt_init_param.icpcode;
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	return D_DDIM_OK;
}


/**
Physical Media Attachment Power Down control.
@param[in]	pma_pd : PMA Power Down control parameters.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NG					: Processing NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
INT32 Im_PRO_SLVS_Pma_Power_Down( T_IM_PRO_SLVS_PMA_PD* pma_pd )
{
#ifdef CO_PARAM_CHECK
	if (pma_pd == NULL){
		Ddim_Assertion(("I:Im_PRO_SLVS_Pma_Power_Down error. pma_pd=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SLVSEC.COMMON.PHYCR2.bit.IRXPDX0		= pma_pd->lane0_pd_state;
	IO_PRO.SEN.SLVSEC.COMMON.PHYCR2.bit.IRXPDX1		= pma_pd->lane1_pd_state;
	IO_PRO.SEN.SLVSEC.COMMON.PHYCR2.bit.IRXPDX2		= pma_pd->lane2_pd_state;
	IO_PRO.SEN.SLVSEC.COMMON.PHYCR2.bit.IRXPDX3		= pma_pd->lane3_pd_state;
	IO_PRO.SEN.SLVSEC.COMMON.PHYCR2.bit.IRXPDX4		= pma_pd->lane4_pd_state;
	IO_PRO.SEN.SLVSEC.COMMON.PHYCR2.bit.IRXPDX5		= pma_pd->lane5_pd_state;
	IO_PRO.SEN.SLVSEC.COMMON.PHYCR2.bit.IRXPDX6		= pma_pd->lane6_pd_state;
	IO_PRO.SEN.SLVSEC.COMMON.PHYCR2.bit.IRXPDX7		= pma_pd->lane7_pd_state;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	return D_DDIM_OK;
}

/**
Physical Media Attachment PLL Power Down control.
@param[in]	pd_state : PMA PLL Power Down control parameters.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NG					: Processing NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
INT32 Im_PRO_SLVS_Pma_Pll_Power_Down( E_IM_PRO_SLVS_PD_STATE pd_state )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SLVSEC.COMMON.PHYCR2.bit.IPDPLLX	= pd_state;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	return D_DDIM_OK;
}

/**
SLVS-EC Common Configuration.
@param[in]	common_cfg : Common configuration parameters.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NG					: Processing NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
INT32 Im_PRO_SLVS_Common_Cfg( T_IM_PRO_SLVS_COMMON_CFG* common_cfg )
{
#ifdef CO_PARAM_CHECK
	if (common_cfg == NULL){
		Ddim_Assertion(("I:Im_PRO_SLVS_Common_Cfg error. common_cfg=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SLVSEC.COMMON.ATTR1.bit.StandbyLength		= common_cfg->StandbyLength;
	IO_PRO.SEN.SLVSEC.COMMON.ATTR1.bit.StandbySymbolType	= common_cfg->StandbySymbolType;
	IO_PRO.SEN.SLVSEC.COMMON.ATTR1.bit.StandbySymbol		= common_cfg->StandbySymbol;
	IO_PRO.SEN.SLVSEC.COMMON.ATTR2.bit.DeskewInterval		= common_cfg->DeskewInterval;
	IO_PRO.SEN.SLVSEC.COMMON.ATTR2.bit.DeskewLength			= common_cfg->DeskewLength;
	IO_PRO.SEN.SLVSEC.COMMON.ATTR2.bit.DeskewSymbolType		= common_cfg->DeskewSymbolType;
	IO_PRO.SEN.SLVSEC.COMMON.ATTR2.bit.DeskewSymbol			= common_cfg->DeskewSymbol;
	IO_PRO.SEN.SLVSEC.COMMON.ATTR3.bit.IdleSymbolType1		= common_cfg->IdleSymbolType1;
	IO_PRO.SEN.SLVSEC.COMMON.ATTR3.bit.IdleSymbol1			= common_cfg->IdleSymbol1;
	IO_PRO.SEN.SLVSEC.COMMON.ATTR3.bit.IdleSymbolType0		= common_cfg->IdleSymbolType0;
	IO_PRO.SEN.SLVSEC.COMMON.ATTR3.bit.IdleSymbol0			= common_cfg->IdleSymbol0;
	IO_PRO.SEN.SLVSEC.COMMON.ATTR4.bit.IdleSymbolType3		= common_cfg->IdleSymbolType3;
	IO_PRO.SEN.SLVSEC.COMMON.ATTR4.bit.IdleSymbol3			= common_cfg->IdleSymbol3;
	IO_PRO.SEN.SLVSEC.COMMON.ATTR4.bit.IdleSymbolType2		= common_cfg->IdleSymbolType2;
	IO_PRO.SEN.SLVSEC.COMMON.ATTR4.bit.IdleSymbol2			= common_cfg->IdleSymbol2;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	return D_DDIM_OK;
}


/**
SLVS-EC Configuration.
@param[in]	stream_type : stream type of SLVS
@param[in]	slvs_ctrl : configuration parameters for each stream.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NG					: Processing NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
INT32 Im_PRO_SLVS_Ctrl( E_IM_PRO_SLVS_STREAM_TYPE stream_type, T_IM_PRO_SLVS_CTRL* slvs_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (slvs_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SLVS_Ctrl error. stream_type=%d slvs_ctrl=NULL\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( stream_type > E_IM_PRO_SLVS_STREAM_TYPE_B ) {
		Ddim_Assertion(("I:Im_PRO_SLVS_Ctrl stream type illegal. stream_type=%d\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].RAMPD.bit.RAMPD		= slvs_ctrl->ram_pd_state;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].TRGR.bit.VDEN			= slvs_ctrl->vdout_ctrl;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].CFGR0.bit.ECCCRC		= slvs_ctrl->ecccrc_opt;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].MMODE.bit.LNMAXCLP	= slvs_ctrl->expect_val;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	return D_DDIM_OK;
}


/**
SLVS-EC Mode Configuration.
@param[in]	stream_type : stream type of SLVS
@param[in]	mode_ctrl : mode configuration parameters for each stream.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NG					: Processing NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
INT32 Im_PRO_SLVS_Mode_Cfg( E_IM_PRO_SLVS_STREAM_TYPE stream_type, T_IM_PRO_SLVS_MODE_CFG* mode_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (mode_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SLVS_Mode_Cfg error. stream_type=%d mode_ctrl=NULL\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( stream_type > E_IM_PRO_SLVS_STREAM_TYPE_B ) {
		Ddim_Assertion(("I:Im_PRO_SLVS_Mode_Cfg stream type illegal. stream_type=%d\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].CFGR0.bit.PixelBit	= mode_ctrl->pixelbit;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].CFGR0.bit.LaneNum		= mode_ctrl->lanenum;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].CFGR1.bit.LineLength	= mode_ctrl->linelength;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].OUTMD.bit.OUTMD		= mode_ctrl->output_format;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].LSWAP.bit.L0OUT		= mode_ctrl->swap_lane[0];
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].LSWAP.bit.L1OUT		= mode_ctrl->swap_lane[1];
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].LSWAP.bit.L2OUT		= mode_ctrl->swap_lane[2];
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].LSWAP.bit.L3OUT		= mode_ctrl->swap_lane[3];
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].LSWAP.bit.L4OUT		= mode_ctrl->swap_lane[4];
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].LSWAP.bit.L5OUT		= mode_ctrl->swap_lane[5];
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].LSWAP.bit.L6OUT		= mode_ctrl->swap_lane[6];
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].LSWAP.bit.L7OUT		= mode_ctrl->swap_lane[7];
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].VLANE.bit.LANE0		= mode_ctrl->valid_lane.lane0;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].VLANE.bit.LANE1		= mode_ctrl->valid_lane.lane1;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].VLANE.bit.LANE2		= mode_ctrl->valid_lane.lane2;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].VLANE.bit.LANE3		= mode_ctrl->valid_lane.lane3;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].VLANE.bit.LANE4		= mode_ctrl->valid_lane.lane4;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].VLANE.bit.LANE5		= mode_ctrl->valid_lane.lane5;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].VLANE.bit.LANE6		= mode_ctrl->valid_lane.lane6;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].VLANE.bit.LANE7		= mode_ctrl->valid_lane.lane7;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	return D_DDIM_OK;
}


/**
SLVS-EC Trimming Configuration.
@param[in]	stream_type : stream type of SLVS
@param[in]	trim_ctrl : trimming configuration parameters for each stream.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NG					: Processing NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
INT32 Im_PRO_SLVS_Trim_Cfg( E_IM_PRO_SLVS_STREAM_TYPE stream_type, T_IM_PRO_SLVS_TRIM_CFG* trim_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (trim_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_SLVS_Trim_Cfg error. stream_type=%d trim_ctrl=NULL\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( stream_type > E_IM_PRO_SLVS_STREAM_TYPE_B ) {
		Ddim_Assertion(("I:Im_PRO_SLVS_Trim_Cfg stream type illegal. stream_type=%d\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].OUTMD.bit.TRMG	= trim_ctrl->enabled;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].HSADD.bit.HSADD	= trim_ctrl->start_x;
	IO_PRO.SEN.SLVSEC.STREAM[stream_type].HSIZE.bit.HSIZE	= trim_ctrl->width;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	return D_DDIM_OK;
}



/**
Get SLVS-EC status.
@param[out]	status : status parameter pointer.
*/
INT32 Im_PRO_SLVS_Get_Status( E_IM_PRO_SLVS_STREAM_TYPE stream_type, T_IM_PRO_SLVS_STATUS* status )
{
#ifdef CO_PARAM_CHECK
	if (status == NULL){
		Ddim_Assertion(("I:Im_PRO_SLVS_Get_Status error. stream_type=%d status=NULL\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( stream_type > E_IM_PRO_SLVS_STREAM_TYPE_B ) {
		Ddim_Assertion(("I:Im_PRO_SLVS_Get_Status stream type illegal. stream_type=%d\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	status->embedded_data_detect		= (E_IM_PRO_SLVS_STATUS_EBD)IO_PRO.SEN.SLVSEC.STREAM[stream_type].EBDST.bit.EBD;
	status->ebd_stored_line_num			= IO_PRO.SEN.SLVSEC.STREAM[stream_type].EBDST.bit.LNUM;
	status->expect_value_discontinuity	= IO_PRO.SEN.SLVSEC.STREAM[stream_type].LNEST.bit.EXPLN;
	status->line_num_discontinuity		= IO_PRO.SEN.SLVSEC.STREAM[stream_type].LNEST.bit.DETLN;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}


/**
SLVS-EC macro start.
*/
INT32 Im_PRO_SLVS_Start( E_IM_PRO_SLVS_STREAM_TYPE stream_type )
{
#ifdef CO_PARAM_CHECK
	if( stream_type > E_IM_PRO_SLVS_STREAM_TYPE_BOTH ) {
		Ddim_Assertion(("I:Im_PRO_SLVS_Start stream type illegal. stream_type=%d\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	if( stream_type == E_IM_PRO_SLVS_STREAM_TYPE_BOTH ) {
		IO_PRO.SEN.SLVSEC.STREAM[E_IM_PRO_SLVS_STREAM_TYPE_A].TRGR.bit.TRG	= D_IM_PRO_TRG_START;
		IO_PRO.SEN.SLVSEC.STREAM[E_IM_PRO_SLVS_STREAM_TYPE_B].TRGR.bit.TRG	= D_IM_PRO_TRG_START;
	}
	else {
		IO_PRO.SEN.SLVSEC.STREAM[stream_type].TRGR.bit.TRG	= D_IM_PRO_TRG_START;
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	if( stream_type == E_IM_PRO_SLVS_STREAM_TYPE_BOTH ) {
		im_pro_sen_set_start_status(D_IM_SEN_STATUS_SLVSEC0, 0);
		im_pro_sen_set_start_status(D_IM_SEN_STATUS_SLVSEC1, 0);
	}
	else {
		im_pro_sen_set_start_status(D_IM_SEN_STATUS_SLVSEC0, stream_type);
	}

	return D_DDIM_OK;
}


/**
Stop SLVS-EC information
@param[in]	force : force stop option
*/
INT32 Im_PRO_SLVS_Stop( E_IM_PRO_SLVS_STREAM_TYPE stream_type, UCHAR force )
{
	UCHAR stop_type = D_IM_PRO_SLVS_TRG_FRAME_STOP;

#ifdef CO_PARAM_CHECK
	if( stream_type > E_IM_PRO_SLVS_STREAM_TYPE_BOTH ) {
		Ddim_Assertion(("I:Im_PRO_SLVS_Stop stream type illegal. stream_type=%d\n", stream_type));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	if (force == 0){
		stop_type = D_IM_PRO_SLVS_TRG_FRAME_STOP;	// frame stop
	}
	else{
		stop_type = D_IM_PRO_SLVS_TRG_FORCE_STOP;	// force stop
	}

	if( stream_type == E_IM_PRO_SLVS_STREAM_TYPE_BOTH ) {
		IO_PRO.SEN.SLVSEC.STREAM[E_IM_PRO_SLVS_STREAM_TYPE_A].TRGR.bit.TRG = stop_type;
		IO_PRO.SEN.SLVSEC.STREAM[E_IM_PRO_SLVS_STREAM_TYPE_B].TRGR.bit.TRG = stop_type;
	}
	else {
		IO_PRO.SEN.SLVSEC.STREAM[stream_type].TRGR.bit.TRG = stop_type;
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	if( stream_type == E_IM_PRO_SLVS_STREAM_TYPE_BOTH ) {
		im_pro_sen_set_stop_status(D_IM_SEN_STATUS_SLVSEC0, 0);
		im_pro_sen_set_stop_status(D_IM_SEN_STATUS_SLVSEC1, 0);
	}
	else {
		im_pro_sen_set_stop_status(D_IM_SEN_STATUS_SLVSEC0, stream_type);
	}

	return D_DDIM_OK;
}

/**
wait end of SLVS-EC macro.
@param[in]		waiptn   : waiting end cause. (bit field)
						<ul><li>@ref D_IM_PRO_SLVS_INTST_FSI_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_INTST_FSO_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_INTST_FEI_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_INTST_FEO_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_INTST_RDY_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_INTST_STBY_WAITEND_FLG</ul>
@param[in]		tmout    : timeout.
@param[out]		p_flgptn : end cause. (bit field)
						<ul><li>@ref D_IM_PRO_SLVS_INTST_FSI_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_INTST_FSO_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_INTST_FEI_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_INTST_FEO_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_INTST_RDY_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_INTST_STBY_WAITEND_FLG</ul>
@retval			D_DDIM_OK                  : SLVS-EC normal end. and set end cause to p_flgptn.
@retval			D_IM_PRO_INPUT_PARAM_ERROR : parameter error.
@retval			D_IM_PRO_TIMEOUT     : wait timeout.
*/
INT32 Im_PRO_SLVS_WaitEnd( DDIM_USER_FLGPTN* const p_flgptn, const DDIM_USER_FLGPTN waiptn, const DDIM_USER_TMO tmout )
{
	DDIM_USER_FLGPTN	flgptn;
	DDIM_USER_ER		ercd;

#ifdef CO_PARAM_CHECK
	if( (waiptn & D_IM_PRO_SLVS_INTST_ALL_WAITEND_FLG) != waiptn ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_PRO_SLVS_WaitEnd error. waiptn\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	Im_Pro_Dsb();

	switch( DDIM_User_Twai_Flg( FID_IM_SEN, waiptn, D_DDIM_USER_TWF_ORW, &flgptn, tmout ) ) {
		case D_DDIM_USER_E_OK:
			flgptn = waiptn & flgptn;
			ercd = DDIM_User_Clr_Flg( FID_IM_SEN, ~flgptn );
			if( ercd != D_DDIM_USER_E_OK ) {
				Ddim_Print(( "Im_PRO_SLVS_WaitEnd: invalid FID.\n" ));
				return D_IM_PRO_SYSTEM_ERROR;
			}
			if( p_flgptn != NULL ) {
				*p_flgptn = flgptn;
			}
			return D_DDIM_OK;
		case D_DDIM_USER_E_TMOUT:
			Ddim_Print(( "Im_PRO_SLVS_WaitEnd twai_flg TimeOut return\n" ));
			return D_IM_PRO_TIMEOUT;
		default:
			Ddim_Print(( "Im_PRO_SLVS_WaitEnd twai_flg NG return\n" ));
			return D_IM_PRO_INPUT_PARAM_ERROR;
	}
}

/**
LDIV macro start.
@param[in]	ch : Channel No.
*/
INT32 Im_PRO_LDIV_Start( E_IM_PRO_LDIV_CH ch )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	// check called of the other macro
	IO_PRO.SEN.LDIV[ch].LDIVTRG.bit.LDIVTRG	= D_IM_PRO_TRG_START;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	im_pro_sen_set_start_status(D_IM_SEN_STATUS_LDIV0, ch);

	return D_DDIM_OK;
}

/**
Stop LDIV information
@param[in]	ch : Channel No.
@param[in]	force : force stop option
*/
INT32 Im_PRO_LDIV_Stop( E_IM_PRO_LDIV_CH ch, UCHAR force )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	if (force == 0){
		IO_PRO.SEN.LDIV[ch].LDIVTRG.bit.LDIVTRG	= D_IM_PRO_TRG_FRAME_STOP;		// frame stop
	}
	else{
		IO_PRO.SEN.LDIV[ch].LDIVTRG.bit.LDIVTRG	= D_IM_PRO_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	im_pro_sen_set_stop_status(D_IM_SEN_STATUS_LDIV0, ch);

	return D_DDIM_OK;
}

/**
LDIV macro control parameter is set.
@param[in]	ch : Channel No.
@param[in]	ldiv_ctrl : LDIV control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_LDIV_Ctrl( E_IM_PRO_LDIV_CH ch, T_IM_PRO_LDIV_CTRL* ldiv_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ldiv_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_LDIV_Ctrl error. ch=%d ldiv_ctrl=NULL\n", ch));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.LDIV[ch].LDIVCTL.bit.LDSTMD	= ldiv_ctrl->mode;
	IO_PRO.SEN.LDIV[ch].LDIVCTL.bit.LDIPMD	= ldiv_ctrl->ldipmd;
	IO_PRO.SEN.LDIV[ch].LDIVCTL.bit.LDVDMD	= ldiv_ctrl->ldvdmd;
	IO_PRO.SEN.LDIV[ch].LDIVCTL.bit.LDHDMD	= ldiv_ctrl->hd_insertion_en;
	IO_PRO.SEN.LDIV[ch].LDIVCTL.bit.LDMCH	= ldiv_ctrl->ldmch;
	IO_PRO.SEN.LDIV[ch].LDIVCTL.bit.LDDIVMD	= ldiv_ctrl->lddivmd;
	IO_PRO.SEN.LDIV[ch].LDIVCTL.bit.LDOPMD	= ldiv_ctrl->ldopmd;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
The area for Top OB detection is set up.
@param[in]	ch : Channel No.
@param[in]	ldiv_area	:The area of LDIV processing. See @ref T_IM_PRO_LDIV_AREA.
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
INT32 Im_PRO_LDIV_Set_Area( E_IM_PRO_LDIV_CH ch, T_IM_PRO_LDIV_AREA* ldiv_area )
{
#ifdef CO_PARAM_CHECK
	if (ldiv_area == NULL){
		Ddim_Assertion(("I:Im_PRO_LDIV_Set_Area error. ch=%d ldiv_area=NULL\n", ch));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( ldiv_area->in_width > gim_pro_maximum_size_ldiv[IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.LDIVRAMSW][ch] ) {
		Ddim_Assertion(("I:Im_PRO_LDIV_Set_Area. input_size value over!! limit value:%u (%u)\n",gim_pro_maximum_size_ldiv[IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.LDIVRAMSW][ch] ,ldiv_area->in_width ));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( ( ldiv_area->out_pos_x[0] + ldiv_area->out_width[0] ) > gim_pro_maximum_size_ldiv[IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.LDIVRAMSW][ch] ) {
		Ddim_Assertion(("I:Im_PRO_LDIV_Set_Area. out_pos_x[0] + out_width[0] value over!! limit value:%u (%u + %u)\n",gim_pro_maximum_size_ldiv[IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.LDIVRAMSW][ch] ,ldiv_area->out_pos_x[0], ldiv_area->out_width[0] ));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( ( ldiv_area->out_pos_x[1] + ldiv_area->out_width[1] ) > gim_pro_maximum_size_ldiv[IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.LDIVRAMSW][ch] ) {
		Ddim_Assertion(("I:Im_PRO_LDIV_Set_Area. out_pos_x[1] + out_width[1] value over!! limit value:%u (%u + %u)\n",gim_pro_maximum_size_ldiv[IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.LDIVRAMSW][ch] ,ldiv_area->out_pos_x[0], ldiv_area->out_width[0] ));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( ( ldiv_area->out_pos_x[2] + ldiv_area->out_width[2] ) > gim_pro_maximum_size_ldiv[IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.LDIVRAMSW][ch] ) {
		Ddim_Assertion(("I:Im_PRO_LDIV_Set_Area. out_pos_x[2] + out_width[2] value over!! limit value:%u (%u + %u)\n",gim_pro_maximum_size_ldiv[IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.LDIVRAMSW][ch] ,ldiv_area->out_pos_x[0], ldiv_area->out_width[0] ));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( ( ldiv_area->out_pos_x[3] + ldiv_area->out_width[3] ) > gim_pro_maximum_size_ldiv[IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.LDIVRAMSW][ch] ) {
		Ddim_Assertion(("I:Im_PRO_LDIV_Set_Area. out_pos_x[3] + out_width[3] value over!! limit value:%u (%u + %u)\n",gim_pro_maximum_size_ldiv[IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.LDIVRAMSW][ch] ,ldiv_area->out_pos_x[0], ldiv_area->out_width[0] ));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.LDIV[ch].LDIVIHW.bit.LDIVIHW				= ldiv_area->in_width;
	IO_PRO.SEN.LDIV[ch].LDIVOH.LDIVOH1.bit.LDIVOH_0		= ldiv_area->out_pos_x[0];
	IO_PRO.SEN.LDIV[ch].LDIVOH.LDIVOH1.bit.LDIVOH_1		= ldiv_area->out_pos_x[1];
	IO_PRO.SEN.LDIV[ch].LDIVOH.LDIVOH2.bit.LDIVOH_2		= ldiv_area->out_pos_x[2];
	IO_PRO.SEN.LDIV[ch].LDIVOH.LDIVOH2.bit.LDIVOH_3		= ldiv_area->out_pos_x[3];
	IO_PRO.SEN.LDIV[ch].LDIVOHW.LDIVOHW1.bit.LDIVOHW_0	= ldiv_area->out_width[0];
	IO_PRO.SEN.LDIV[ch].LDIVOHW.LDIVOHW1.bit.LDIVOHW_1	= ldiv_area->out_width[1];
	IO_PRO.SEN.LDIV[ch].LDIVOHW.LDIVOHW2.bit.LDIVOHW_2	= ldiv_area->out_width[2];
	IO_PRO.SEN.LDIV[ch].LDIVOHW.LDIVOHW2.bit.LDIVOHW_3	= ldiv_area->out_width[3];
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of LDIV.
@param[in]	ch : Channel No.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_LDIV_Set_PAEN( E_IM_PRO_LDIV_CH ch, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.SEN.LDIV[ch].LDIVTRG.bit.LDIVTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_LDIV_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.LDIV[ch].LDIVPAEN.bit.LDPAEN = paen_trg;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
Start OBD
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_OBD_Start( E_IM_PRO_OBD_CH ch )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;

	im_pro_comm_get_obd_block_ch( ch, &block_num, &ch_num );

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.OBD[block_num][ch_num].OBDTRG.bit.OBDTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	im_pro_sen_set_start_status(D_IM_SEN_STATUS_OBD00, ch);

	return D_DDIM_OK;
}

/**
Stop OBD
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_OBD_Stop( E_IM_PRO_OBD_CH ch, UCHAR force )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;

	im_pro_comm_get_obd_block_ch( ch, &block_num, &ch_num );

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	if (force == 0){
		IO_PRO.SEN.OBD[block_num][ch_num].OBDTRG.bit.OBDTRG = D_IM_PRO_TRG_FRAME_STOP;	// frame stop
	}
	else{
		IO_PRO.SEN.OBD[block_num][ch_num].OBDTRG.bit.OBDTRG = D_IM_PRO_TRG_FORCE_STOP;	// force stop
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	im_pro_sen_set_stop_status(D_IM_SEN_STATUS_OBD00, ch);

	return D_DDIM_OK;
}

/**
The control parameter of OBD compensation is set.
@param[in]	ch : Channel No.
@param[in]	ob_ctrl	:	OBD Control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_OBD_Ctrl( E_IM_PRO_OBD_CH ch, T_IM_PRO_OBD_CTRL* ob_ctrl )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;
#ifdef CO_PARAM_CHECK
	if (ob_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_OBD_Ctrl error. ob_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_comm_get_obd_block_ch( ch, &block_num, &ch_num );

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.OBD[block_num][ch_num].OBDCORE.OBDTHBIT.bit.OBDLTHBIT	= ob_ctrl->ob_min_value;
	IO_PRO.SEN.OBD[block_num][ch_num].OBDCORE.OBDTHBIT.bit.OBDHTHBIT	= ob_ctrl->ob_max_value;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
The area for OBD detection is set up.
@param[in]	ch : Channel No.
@param[in]	ob_area	:The area for OB detection<br>
					value range :pos_x[0 - 12287]<br>
					target registor :@@OBDH<br>
					value range :pos_y[0 - 8191]<br>
					target registor :@@OBDV<br>
					value range :width[2 - 12288] 2pixel boundary<br>
					target registor :@@OBDHW<br>
					value range :lines[2 - 8192] 2pixel boundary<br>
					target registor :@@OBDVW<br>
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
INT32 Im_PRO_OBD_Set_Area( E_IM_PRO_OBD_CH ch, T_IM_PRO_AREA_INFO* ob_area )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;

#ifdef CO_PARAM_CHECK
	if (ob_area == NULL){
		Ddim_Assertion(("I:Im_PRO_OBD_Set_Area error. ob_area=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_OBD_OBDH_MIN, D_IM_PRO_OBD_OBDH_MAX, ob_area->pos_x , "Im_PRO_OBD_Set_Area : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_OBD_OBDV_MIN, D_IM_PRO_OBD_OBDV_MAX, ob_area->pos_y , "Im_PRO_OBD_Set_Area : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_OBD_OBDHW_MIN, D_IM_PRO_OBD_OBDHW_MAX, ob_area->width , "Im_PRO_OBD_Set_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_OBD_OBDVW_MIN, D_IM_PRO_OBD_OBDVW_MAX, ob_area->lines , "Im_PRO_OBD_Set_Area : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	im_pro_comm_get_obd_block_ch( ch, &block_num, &ch_num );

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.OBD[block_num][ch_num].OBDCORE.OBDH.bit.OBDH		= ob_area->pos_x;
	IO_PRO.SEN.OBD[block_num][ch_num].OBDCORE.OBDV.bit.OBDV		= ob_area->pos_y;
	IO_PRO.SEN.OBD[block_num][ch_num].OBDCORE.OBDHW.bit.OBDHW	= ob_area->width;
	IO_PRO.SEN.OBD[block_num][ch_num].OBDCORE.OBDVW.bit.OBDVW	= ob_area->lines;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
Get OBD count
@param[in]	ch : Channel No.
@param[out]	obcnt :	OBCNT<br>
			each colors count value range :0 - 0xFFFFF<br>
@retval		D_DDIM_OK					: Getting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Getting NG
*/
INT32 Im_PRO_OBD_Get_OBCNT( E_IM_PRO_OBD_CH ch, T_IM_PRO_RGB4* obcnt )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;

	im_pro_comm_get_obd_block_ch( ch, &block_num, &ch_num );
	im_pro_comm_get_obd_cnt( block_num, ch_num, obcnt );
	return D_DDIM_OK;
}

/**
Get OBD data
@param[in]	ch : Channel No.
@param[out]	obdata : OBD Data
@retval		D_DDIM_OK					: Getting OK
@retval		D_IM_PRO_NG					: Getting NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Getting NG
*/
INT32 Im_PRO_OBD_Get_OBData( E_IM_PRO_OBD_CH ch, T_IM_PRO_RGB4* obdata )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;

	im_pro_comm_get_obd_block_ch( ch, &block_num, &ch_num );
	im_pro_comm_get_obd_data( block_num, ch_num, obdata );

	return D_DDIM_OK;
}

/**
Start TOP OB macro.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
INT32 Im_PRO_OBT_Start( E_IM_PRO_OBT_CH ch )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;

	im_pro_comm_get_obt_block_ch( ch, &block_num, &ch_num );

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.OBT[block_num][ch_num].OBTTRG.bit.OBTTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	im_pro_sen_set_start_status(D_IM_SEN_STATUS_OBT00, ch);

	return D_DDIM_OK;
}

/**
Stop TOP OB macro.
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_OBT_Stop( E_IM_PRO_OBT_CH ch, UCHAR force )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;

	im_pro_comm_get_obt_block_ch( ch, &block_num, &ch_num );

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	if (force == 0){
		IO_PRO.SEN.OBT[block_num][ch_num].OBTTRG.bit.OBTTRG = D_IM_PRO_TRG_FRAME_STOP;	// frame stop
	}
	else{
		IO_PRO.SEN.OBT[block_num][ch_num].OBTTRG.bit.OBTTRG = D_IM_PRO_TRG_FORCE_STOP;	// force stop
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	im_pro_sen_set_stop_status(D_IM_SEN_STATUS_OBT00, ch);

	return D_DDIM_OK;
}

/**
The control parameter of TOP OB compensation is set.
@param[in]	ch : Channel No.
@param[in]	obt_ctrl	:	TOP OB Control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_OBT_Ctrl( E_IM_PRO_OBT_CH ch, T_IM_PRO_OBT_CTRL* obt_ctrl )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;

#ifdef CO_PARAM_CHECK
	if (obt_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_OBT_Ctrl error. obt_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_comm_get_obt_block_ch( ch, &block_num, &ch_num );

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.OBT[block_num][ch_num].OBTCORE.OBTCTL.bit.OBTEN			= obt_ctrl->obt_enable;
	IO_PRO.SEN.OBT[block_num][ch_num].OBTCORE.OBTCTL.bit.OBTMD			= obt_ctrl->mode;
	IO_PRO.SEN.OBT[block_num][ch_num].OBTCORE.OBTCTL.bit.OBTSL			= obt_ctrl->sel;
	IO_PRO.SEN.OBT[block_num][ch_num].OBTCORE.OBTCTL.bit.OBTCEN			= obt_ctrl->intercomm_enable;
	IO_PRO.SEN.OBT[block_num][ch_num].OBTCORE.OBTTHBIT.bit.OBTHTHBIT	= obt_ctrl->ob_max_value;
	IO_PRO.SEN.OBT[block_num][ch_num].OBTCORE.OBTTHBIT.bit.OBTLTHBIT	= obt_ctrl->ob_min_value;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
The area for Top OB detection is set up.
@param[in]	ch : Channel No.
@param[in]	obt_area	:The area for Top OB detection. See @ref T_IM_PRO_AREA_INFO<br>
							value range  :pos_x[0 - 12287]<br>
							target registor  :@@OBTH<br>
							value range  :pos_y[0 - 8191]<br>
							target registor  :@@OBTV<br>
							value range  :width[1 - 12288]	1pixel boundary(OBTMD=0)<br>
															2pixel boundary(OBTMD=1)<br>
							target registor  :@@OBTHW<br>
							value range  :lines[1 - 127]	1pixel boundary(OBTMD=0)<br>
															2pixel boundary(OBTMD=1)<br>
							target registor  :@@OBTVW
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
INT32 Im_PRO_OBT_Set_Area( E_IM_PRO_OBT_CH ch, T_IM_PRO_AREA_INFO* obt_area )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;

#ifdef CO_PARAM_CHECK
	if (obt_area == NULL){
		Ddim_Assertion(("I:Im_PRO_OBT_Set_Area error. obt_area=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_OBT_OBTH_MIN, D_IM_PRO_OBT_OBTH_MAX, obt_area->pos_x, "Im_PRO_OBT_Set_Area : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_OBT_OBTV_MIN, D_IM_PRO_OBT_OBTV_MAX, obt_area->pos_y, "Im_PRO_OBT_Set_Area : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_OBT_OBTHW_MIN, D_IM_PRO_OBT_OBTHW_MAX, obt_area->width, "Im_PRO_OBT_Set_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_OBT_OBTVW_MIN, D_IM_PRO_OBT_OBTVW_MAX, obt_area->lines, "Im_PRO_OBT_Set_Area : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	im_pro_comm_get_obt_block_ch( ch, &block_num, &ch_num );

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.OBT[block_num][ch_num].OBTCORE.OBTH.bit.OBTH		= obt_area->pos_x;
	IO_PRO.SEN.OBT[block_num][ch_num].OBTCORE.OBTV.bit.OBTV		= obt_area->pos_y;
	IO_PRO.SEN.OBT[block_num][ch_num].OBTCORE.OBTHW.bit.OBTHW	= obt_area->width;
	IO_PRO.SEN.OBT[block_num][ch_num].OBTCORE.OBTVW.bit.OBTVW	= obt_area->lines;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
Set TOP OB offset
@param[in]	ch : Channel No.<br>
				 value range:[0 - 1]<br>
@param[in]	offset : TOP OB offset<br>
					 value range : R :[0x4000(=-16384) - 0x3FFF(=+16383)]<br>
					 target registor :@@OBTOFSR<br>
					 value range : Gr :[0x4000(=-16384) - 0x3FFF(=+16383)]<br>
					 target registor :@@OBTOFSGR<br>
					 value range : Gb :[0x4000(=-16384) - 0x3FFF(=+16383)]<br>
					 target registor :@@OBTOFSGB<br>
					 value range : B :[0x4000(=-16384) - 0x3FFF(=+16383)]<br>
					 target registor :@@OBTOFSB
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@remarks	Please set up a minus value by a complement. bit15 is sign bit
*/
INT32 Im_PRO_OBT_Set_Offset( E_IM_PRO_OBT_CH ch, T_IM_PRO_RGB2* offset )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;

#ifdef CO_PARAM_CHECK
	if (offset == NULL){
		Ddim_Assertion(("I:Im_PRO_OBT_Set_Offset error. offset=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	im_pro_comm_get_obt_block_ch( ch, &block_num, &ch_num );

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	im_pro_set_reg_signed( IO_PRO.SEN.OBT[block_num][ch_num].OBTCORE.OBTOFSR, union io_obtofsr, OBTOFSR, offset->R );
	im_pro_set_reg_signed( IO_PRO.SEN.OBT[block_num][ch_num].OBTCORE.OBTOFSGR, union io_obtofsgr, OBTOFSGR, offset->Gr );
	im_pro_set_reg_signed( IO_PRO.SEN.OBT[block_num][ch_num].OBTCORE.OBTOFSGB, union io_obtofsgb, OBTOFSGB, offset->Gb );
	im_pro_set_reg_signed( IO_PRO.SEN.OBT[block_num][ch_num].OBTCORE.OBTOFSB, union io_obtofsb, OBTOFSB, offset->B );
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
Get TOP OB detection data
@param[in]	ch : Channel No.<br>
				 value range:[0 - 1]<br>
@param[out]	obtdata : OBTDATA
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_OBT_Get_OBTData( E_IM_PRO_OBT_CH ch, T_IM_PRO_RGB2* obtdata )
{
	UCHAR block_num = 0;
	UCHAR ch_num = 0;

	im_pro_comm_get_obt_block_ch( ch, &block_num, &ch_num );
	im_pro_comm_get_obt_data( block_num, ch_num, obtdata );

	return D_DDIM_OK;
}

/**
Get the top address of the address array of OBT control.
@param[in]		ch : Channel No.
@param[out]		addr					: Top address of the address array of OBT control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
INT32 Im_PRO_Get_RdmaAddr_OBT_Cntl( E_IM_PRO_OBT_CH ch, const T_IM_PRO_RDMA_OBT_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( addr == NULL ) {
		Ddim_Assertion(("I:Im_PRO_Get_RdmaAddr_OBT_Cntl. error. addr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	*addr = &(gIM_PRO_OBT_Addr[ch]);

	return D_DDIM_OK;
}


/**
MIPI-DPHY initialize
@param[in]	ch : MIPI-DPHY channel
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_DMIPI_Init( E_IM_PRO_DMIPI_CH ch )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	// Software release
	IO_PRO.SEN.MIPID[ch].MPISR.bit.SR  = D_IM_PRO_SR_RELEASE;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
Software reset of MIPI-DPHY
@param[in]	ch : MIPI-DPHY channel
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_DMIPI_SR( E_IM_PRO_DMIPI_CH ch )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	// Software reset
	IO_PRO.SEN.MIPID[ch].MPISR.bit.SR  = D_IM_PRO_SR_RESET;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
MIPI-DPHY macro start.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Macro Start OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: D-MIPI has not stopped NG
*/
INT32 Im_PRO_DMIPI_Start( E_IM_PRO_DMIPI_CH ch )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	if( IO_PRO.SEN.MIPID[ch].MPITRG.bit.MPITRG == D_IM_PRO_TRG_STATUS_RUNNING ) {
		Ddim_Print(( "Im_PRO_DMIPI_Start D-MIPI macro is not stopped error.\n" ));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
	IO_PRO.SEN.MIPID[ch].MPITRG.bit.MPITRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	im_pro_sen_set_start_status(D_IM_SEN_STATUS_DMIPI0, ch);

	return D_DDIM_OK;
}

/**
MIPI-DPHY macro stop.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Macro Stop OK
@retval		D_IM_PRO_NG					: Macro Stop NG
*/
INT32 Im_PRO_DMIPI_Stop( E_IM_PRO_DMIPI_CH ch, UCHAR force )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	if (force == 0){
		IO_PRO.SEN.MIPID[ch].MPITRG.bit.MPITRG = 2;	// frame stop
	}
	else{
		IO_PRO.SEN.MIPID[ch].MPITRG.bit.MPITRG = 0;	// force stop
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	im_pro_sen_set_stop_status(D_IM_SEN_STATUS_DMIPI0, ch);

	return D_DDIM_OK;
}

/**
The control parameter of MIPI-DPHY is set.
@param[in]	ch : Channel No.
@param[in]	dmipi_ctrl : MIPI-DPHY control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
INT32 Im_PRO_DMIPI_Ctrl( E_IM_PRO_DMIPI_CH ch, T_IM_PRO_DMIPI_CTRL* dmipi_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (dmipi_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_DMIPI_Ctrl error. dmipi_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (im_pro_sen_get_start_status( D_IM_SEN_STATUS_DMIPI0, ch ) == TRUE) {	//not Stopped
		Ddim_Print(("I:Im_PRO_DMIPI_Ctrl. macro not stopped error. \n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.MIPID[ch].MPILMODE.bit.LANE			= dmipi_ctrl->lane_sel;

	IO_PRO.SEN.MIPID[ch].MPIIOMD0.bit.DTMD0			= dmipi_ctrl->input_data_mode[0];
	IO_PRO.SEN.MIPID[ch].MPIIOMD0.bit.OUTMD0		= dmipi_ctrl->output_data_mode[0];
	IO_PRO.SEN.MIPID[ch].MPIIOMD0.bit.VCOUT0		= dmipi_ctrl->vc_mode_sel[0];
	IO_PRO.SEN.MIPID[ch].MPIIOMD0.bit.DTCOMD0		= dmipi_ctrl->output_dt_sel[0];
	IO_PRO.SEN.MIPID[ch].MPIIOMD0.bit.DTCO0			= dmipi_ctrl->output_dt_setting[0];

	IO_PRO.SEN.MIPID[ch].MPIIOMD1.bit.DTMD1			= dmipi_ctrl->input_data_mode[1];
	IO_PRO.SEN.MIPID[ch].MPIIOMD1.bit.OUTMD1		= dmipi_ctrl->output_data_mode[1];
	IO_PRO.SEN.MIPID[ch].MPIIOMD1.bit.VCOUT1		= dmipi_ctrl->vc_mode_sel[1];
	IO_PRO.SEN.MIPID[ch].MPIIOMD1.bit.DTCOMD1		= dmipi_ctrl->output_dt_sel[1];
	IO_PRO.SEN.MIPID[ch].MPIIOMD1.bit.DTCO1			= dmipi_ctrl->output_dt_setting[1];

	IO_PRO.SEN.MIPID[ch].MPIDPI.bit.DPSMTC			= dmipi_ctrl->phy_ctrl.rxsmt_c_val;
	IO_PRO.SEN.MIPID[ch].MPIDPI.bit.DPSMTD			= dmipi_ctrl->phy_ctrl.rxsmt_d_val;
	IO_PRO.SEN.MIPID[ch].MPIDPI.bit.DPSM			= dmipi_ctrl->phy_ctrl.rxsyncmode;

	IO_PRO.SEN.MIPID[ch].MPIHDPDSEL.bit.HDPDSEL0	= dmipi_ctrl->out_cycle[0];
	IO_PRO.SEN.MIPID[ch].MPIHDPDSEL.bit.HDPDSEL1	= dmipi_ctrl->out_cycle[1];

	IO_PRO.SEN.MIPID[ch].MPIDPI.bit.DPEN			= dmipi_ctrl->phy_ctrl.rxenable;	// start D-PHY
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	im_pro_wait_usec( D_IM_PRO_DMIPI_WAIT_PHY_STARTUP );

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of MIPI-DPHY.
@param[in]	ch : Channel No.
@param[in]	paen0_trg : RAM0 access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@param[in]	paen1_trg : RAM1 access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_DMIPI_Set_PAEN( E_IM_PRO_DMIPI_CH ch, UCHAR paen0_trg, UCHAR paen1_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen0_trg == 0 ) && ( paen1_trg == 0 ) && ( IO_PRO.SEN.MIPID[ch].MPITRG.bit.MPITRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_DMIPI_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.MIPID[ch].MPIPAEN0.bit.PAEN0 = paen0_trg;
	IO_PRO.SEN.MIPID[ch].MPIPAEN1.bit.PAEN1 = paen1_trg;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
Get MIPI D-PHY status..
@param[in]	ch : Channel No.
@param[out]	status : MIPI-DPHY status information
@retval		D_DDIM_OK					: Getting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Getting NG
*/
INT32 Im_PRO_DMIPI_Get_Status( E_IM_PRO_DMIPI_CH ch, T_IM_PRO_DMIPI_STATUS* status )
{
#ifdef CO_PARAM_CHECK
	if (status == NULL){
		Ddim_Assertion(("I:Im_PRO_DMIPI_Get_Status error. status=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	status->input_signal_status		= IO_PRO.SEN.MIPID[ch].MPIDPVR.word;
	status->skew_calibration_status	= IO_PRO.SEN.MIPID[ch].MPISCS.word;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
MIPI-CPHY initialize
@param[in]	ch : MIPI-CPHY channel
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_CMIPI_Init( E_IM_PRO_CMIPI_CH ch )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	// Software release
	IO_PRO.SEN.MIPIC[ch].MPICSR.bit.SR  = D_IM_PRO_SR_RELEASE;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
Software reset of MIPI-CPHY
@param[in]	ch : MIPI-CPHY channel
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_CMIPI_SR( E_IM_PRO_CMIPI_CH ch )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	// Software reset
	IO_PRO.SEN.MIPIC[ch].MPICSR.bit.SR  = D_IM_PRO_SR_RESET;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
MIPI-CPHY macro start.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Macro Start OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: C-MIPI has not stopped NG
*/
INT32 Im_PRO_CMIPI_Start( E_IM_PRO_CMIPI_CH ch )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	if( IO_PRO.SEN.MIPIC[ch].MPICTRG.bit.MPICTRG == D_IM_PRO_TRG_STATUS_RUNNING ) {
		Ddim_Print(( "Im_PRO_CMIPI_Start C-MIPI macro is not stopped error.\n" ));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
	IO_PRO.SEN.MIPIC[ch].MPICTRG.bit.MPICTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	im_pro_sen_set_start_status(D_IM_SEN_STATUS_CMIPI0, ch);

	return D_DDIM_OK;
}

/**
MIPI-CPHY macro stop.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Macro Stop OK
@retval		D_IM_PRO_NG					: Macro Stop NG
*/
INT32 Im_PRO_CMIPI_Stop( E_IM_PRO_CMIPI_CH ch, UCHAR force )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	if (force == 0){
		IO_PRO.SEN.MIPIC[ch].MPICTRG.bit.MPICTRG = D_IM_PRO_TRG_FRAME_STOP;	// frame stop
	}
	else{
		IO_PRO.SEN.MIPIC[ch].MPICTRG.bit.MPICTRG = D_IM_PRO_TRG_FORCE_STOP;	// force stop
	}
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	im_pro_sen_set_stop_status(D_IM_SEN_STATUS_CMIPI0, ch);

	return D_DDIM_OK;
}

/**
The control parameter of MIPI-CPHY is set.
@param[in]	ch : Channel No.
@param[in]	cmipi_ctrl : MIPI-CPHY control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
INT32 Im_PRO_CMIPI_Ctrl( E_IM_PRO_CMIPI_CH ch, T_IM_PRO_CMIPI_CTRL* cmipi_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (cmipi_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_CMIPI_Ctrl error. cmipi_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (im_pro_sen_get_start_status( D_IM_SEN_STATUS_CMIPI0, ch ) == TRUE) {	//not Stopped
		Ddim_Print(("I:Im_PRO_CMIPI_Ctrl. macro not stopped error. \n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.MIPIC[ch].MPICLMODE.bit.LANE					= cmipi_ctrl->lane_sel;
	IO_PRO.SEN.MIPIC[ch].MPICIOMD0.bit.DTMD0				= cmipi_ctrl->input_data_mode[0];
	IO_PRO.SEN.MIPIC[ch].MPICIOMD0.bit.OUTMD0				= cmipi_ctrl->output_data_mode[0];
	IO_PRO.SEN.MIPIC[ch].MPICIOMD0.bit.VCOUT0				= cmipi_ctrl->vc_mode_sel[0];
	IO_PRO.SEN.MIPIC[ch].MPICIOMD0.bit.DTCOMD0				= cmipi_ctrl->output_dt_sel[0];
	IO_PRO.SEN.MIPIC[ch].MPICIOMD0.bit.DTCO0				= cmipi_ctrl->output_dt_setting[0];
	IO_PRO.SEN.MIPIC[ch].MPICIOMD1.bit.DTMD1				= cmipi_ctrl->input_data_mode[1];
	IO_PRO.SEN.MIPIC[ch].MPICIOMD1.bit.OUTMD1				= cmipi_ctrl->output_data_mode[1];
	IO_PRO.SEN.MIPIC[ch].MPICIOMD1.bit.VCOUT1				= cmipi_ctrl->vc_mode_sel[1];
	IO_PRO.SEN.MIPIC[ch].MPICIOMD1.bit.DTCOMD1				= cmipi_ctrl->output_dt_sel[1];
	IO_PRO.SEN.MIPIC[ch].MPICIOMD1.bit.DTCO1				= cmipi_ctrl->output_dt_setting[1];
	IO_PRO.SEN.MIPIC[ch].MPICHDPDSEL.bit.HDPDSEL0			= cmipi_ctrl->out_cycle[0];
	IO_PRO.SEN.MIPIC[ch].MPICHDPDSEL.bit.HDPDSEL1			= cmipi_ctrl->out_cycle[1];
	IO_PRO.SEN.MIPIC[ch].MPICCPI2.bit.CP_HS_SETTLECNT		= cmipi_ctrl->phy_ctrl.hs_settlecnt;
	IO_PRO.SEN.MIPIC[ch].MPICCPI2.bit.CP_HS_STOPCNT			= cmipi_ctrl->phy_ctrl.hs_stopcnt;
	IO_PRO.SEN.MIPIC[ch].MPICCPI2.bit.CP_HS_SYNCMODE		= cmipi_ctrl->phy_ctrl.hs_syncmode;
	IO_PRO.SEN.MIPIC[ch].MPICPRLRLCONF.bit.CP_RLR_LCONFIG	= cmipi_ctrl->phy_ctrl.rlr_lconfig;
	IO_PRO.SEN.MIPIC[ch].MPICPRLRLTPSEL.bit.CP_RLR_TPSEL	= cmipi_ctrl->phy_ctrl.rlr_tpsel;
	IO_PRO.SEN.MIPIC[ch].MPICPRLRPRBS.bit.CP_RLR_PRBSSP		= cmipi_ctrl->phy_ctrl.rlr_prbssp;
	IO_PRO.SEN.MIPIC[ch].MPICPPROGSEQ.bit.CP_TGR_PROGSEQ	= cmipi_ctrl->phy_ctrl.tgr_progseq;
	IO_PRO.SEN.MIPIC[ch].MPICPFRXMODE.bit.CP_FORCERXMODE2	= cmipi_ctrl->phy_ctrl.forcerxmode2;
	IO_PRO.SEN.MIPIC[ch].MPICPFRXMODE.bit.CP_FORCERXMODE1	= cmipi_ctrl->phy_ctrl.forcerxmode1;
	IO_PRO.SEN.MIPIC[ch].MPICPFRXMODE.bit.CP_FORCERXMODE0	= cmipi_ctrl->phy_ctrl.forcerxmode0;

	IO_PRO.SEN.MIPIC[ch].MPICCPI1.bit.CPEN					= cmipi_ctrl->phy_ctrl.rxenable;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	im_pro_wait_usec( D_IM_PRO_CMIPI_WAIT_PHY_STARTUP );


	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of MIPI-CPHY.
@param[in]	ch : Channel No.
@param[in]	paen0_trg : RAM0 access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@param[in]	paen1_trg : RAM1 access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_CMIPI_Set_PAEN( E_IM_PRO_CMIPI_CH ch, UCHAR paen0_trg, UCHAR paen1_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen0_trg == 0 ) && ( paen1_trg == 0 ) && ( IO_PRO.SEN.MIPIC[ch].MPICTRG.bit.MPICTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_CMIPI_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.MIPIC[ch].MPICPAEN0.bit.PAEN0 = paen0_trg;
	IO_PRO.SEN.MIPIC[ch].MPICPAEN1.bit.PAEN1 = paen1_trg;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}


/**
Get MIPI C-PHY status..
@param[in]	ch : Channel No.
@param[out]	status : MIPI-CPHY status information
*/
INT32 Im_PRO_CMIPI_Get_Status( E_IM_PRO_CMIPI_CH ch, T_IM_PRO_CMIPI_STATUS* status )
{
#ifdef CO_PARAM_CHECK
	if (status == NULL){
		Ddim_Assertion(("I:Im_PRO_CMIPI_Get_Status error. status=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (status == NULL){
		Ddim_Assertion(("I:Im_PRO_CMIPI_Get_Status error. status=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	status->rxstopstate_l0	= IO_PRO.SEN.MIPIC[ch].MPICPVR.bit.SSL0;
	status->rxstopstate_l1	= IO_PRO.SEN.MIPIC[ch].MPICPVR.bit.SSL1;
	status->rxstopstate_l2	= IO_PRO.SEN.MIPIC[ch].MPICPVR.bit.SSL2;
	status->lstatus0		= IO_PRO.SEN.MIPIC[ch].MPICPRLRLST.bit.CP_RLR_LSTATUS0;
	status->lstatus1		= IO_PRO.SEN.MIPIC[ch].MPICPRLRLST.bit.CP_RLR_LSTATUS1;
	status->lstatus2		= IO_PRO.SEN.MIPIC[ch].MPICPRLRLST.bit.CP_RLR_LSTATUS2;
	status->werrcnt0		= IO_PRO.SEN.MIPIC[ch].MPICPRLRWECNT0.bit.CP_RLR_WERRCNT0;
	status->werrcnt1		= IO_PRO.SEN.MIPIC[ch].MPICPRLRWECNT1.bit.CP_RLR_WERRCNT1;
	status->werrcnt2		= IO_PRO.SEN.MIPIC[ch].MPICPRLRWECNT2.bit.CP_RLR_WERRCNT2;
	status->wcnt0			= IO_PRO.SEN.MIPIC[ch].MPICPRLRWCNT0.bit.CP_RLR_WCNT0;
	status->wcnt1			= IO_PRO.SEN.MIPIC[ch].MPICPRLRWCNT1.bit.CP_RLR_WCNT1;
	status->wcnt2			= IO_PRO.SEN.MIPIC[ch].MPICPRLRWCNT2.bit.CP_RLR_WCNT2;
	status->syerrcnt0		= IO_PRO.SEN.MIPIC[ch].MPICPRLRSERRCNT0.bit.CP_RLR_SYERRCNT0;
	status->syerrcnt1		= IO_PRO.SEN.MIPIC[ch].MPICPRLRSERRCNT1.bit.CP_RLR_SYERRCNT1;
	status->syerrcnt2		= IO_PRO.SEN.MIPIC[ch].MPICPRLRSERRCNT2.bit.CP_RLR_SYERRCNT2;
	status->syerrloc0		= IO_PRO.SEN.MIPIC[ch].MPICPRLRSERRLOC0.bit.CP_RLR_SYERRLOC0;
	status->syerrloc1		= IO_PRO.SEN.MIPIC[ch].MPICPRLRSERRLOC1.bit.CP_RLR_SYERRLOC1;
	status->syerrloc2		= IO_PRO.SEN.MIPIC[ch].MPICPRLRSERRLOC2.bit.CP_RLR_SYERRLOC2;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
LVDS macro start.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Macro Start OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: LVDS has not stopped NG
*/
INT32 Im_PRO_LVDS_Start( E_IM_PRO_LVDS_CH ch )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.LVDS[ch].LVDSTRG.bit.LVDSTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	im_pro_sen_set_start_status(D_IM_SEN_STATUS_LVDS0, ch);

	return D_DDIM_OK;
}

/**
LVDS macro stop.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Macro Stop OK
@retval		D_IM_PRO_NG					: Macro Stop NG
*/
INT32 Im_PRO_LVDS_Stop( E_IM_PRO_LVDS_CH ch )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.LVDS[ch].LVDSTRG.bit.LVDSTRG = D_IM_PRO_TRG_FRAME_STOP;	// frame stop
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	im_pro_sen_set_stop_status(D_IM_SEN_STATUS_LVDS0, ch);

	return D_DDIM_OK;
}

/**
The control parameter of LVDS is set.
@param[in]	ch : Channel No.
@param[in]	lvds_ctrl : LVDS control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
INT32 Im_PRO_LVDS_Ctrl( E_IM_PRO_LVDS_CH ch, T_IM_PRO_LVDS_CTRL* lvds_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (lvds_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_LVDS_Ctrl error. lvds_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( lvds_ctrl->output_ch_no > E_IM_PRO_LVDS_OUT_1CH ) {
		Ddim_Print(("I:Im_PRO_LVDS_Ctrl. output_ch_no error.(%u) \n", lvds_ctrl->output_ch_no));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.LVDS[ch].LVDSCTL1.bit.LMD	= lvds_ctrl->trans_mode;
	IO_PRO.SEN.LVDS[ch].LVDSCTL1.bit.LCH	= lvds_ctrl->output_ch_no;
	IO_PRO.SEN.LVDS[ch].LVDSCTL1.bit.LANE	= lvds_ctrl->lane;
	IO_PRO.SEN.LVDS[ch].LVDSCTL1.bit.LDW	= lvds_ctrl->data_bit_width;
	IO_PRO.SEN.LVDS[ch].LVDSCTL1.bit.LSFT	= lvds_ctrl->lsft;
	IO_PRO.SEN.LVDS[ch].LVDSCTL1.bit.SYNT	= lvds_ctrl->sync_type;
	IO_PRO.SEN.LVDS[ch].LVDSCTL1.bit.SZEN	= lvds_ctrl->data_input_type;
	IO_PRO.SEN.LVDS[ch].LVDSCTL1.bit.SYN16	= lvds_ctrl->sync_bit_fix;

	IO_PRO.SEN.LVDS[ch].LVDSCTL2.bit.DIVOT	= lvds_ctrl->divide_output;
	IO_PRO.SEN.LVDS[ch].LVDSCTL2.bit.LMIR	= lvds_ctrl->inverted_output;
	IO_PRO.SEN.LVDS[ch].LVDSCTL2.bit.DOLMD	= lvds_ctrl->output_mode;
	IO_PRO.SEN.LVDS[ch].LVDSCTL2.bit.LMSK	= lvds_ctrl->lane_mask;

	IO_PRO.SEN.LVDS[ch].LVDSCLKS.bit.CLKS0	= lvds_ctrl->clock_select_sp0;
	IO_PRO.SEN.LVDS[ch].LVDSCLKS.bit.CLKS1	= lvds_ctrl->clock_select_sp1;
	IO_PRO.SEN.LVDS[ch].LVDSCLKS.bit.CLKS2	= lvds_ctrl->clock_select_sp2;
	IO_PRO.SEN.LVDS[ch].LVDSCLKS.bit.CLKS3	= lvds_ctrl->clock_select_sp3;
	IO_PRO.SEN.LVDS[ch].LVDSCLKS.bit.CLKS4	= lvds_ctrl->clock_select_sp4;
	IO_PRO.SEN.LVDS[ch].LVDSCLKS.bit.CLKS5	= lvds_ctrl->clock_select_sp5;
	IO_PRO.SEN.LVDS[ch].LVDSCLKS.bit.CLKS6	= lvds_ctrl->clock_select_sp6;
	IO_PRO.SEN.LVDS[ch].LVDSCLKS.bit.CLKS7	= lvds_ctrl->clock_select_sp7;

	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS00	= lvds_ctrl->data_select_sp0_0;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS01	= lvds_ctrl->data_select_sp0_1;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS10	= lvds_ctrl->data_select_sp1_0;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS11	= lvds_ctrl->data_select_sp1_1;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS20	= lvds_ctrl->data_select_sp2_0;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS21	= lvds_ctrl->data_select_sp2_1;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS30	= lvds_ctrl->data_select_sp3_0;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS31	= lvds_ctrl->data_select_sp3_1;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS40	= lvds_ctrl->data_select_sp4_0;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS41	= lvds_ctrl->data_select_sp4_1;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS50	= lvds_ctrl->data_select_sp5_0;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS51	= lvds_ctrl->data_select_sp5_1;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS60	= lvds_ctrl->data_select_sp6_0;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS61	= lvds_ctrl->data_select_sp6_1;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS71	= lvds_ctrl->data_select_sp7_0;
	IO_PRO.SEN.LVDS[ch].LVDSDATS.bit.DATS70	= lvds_ctrl->data_select_sp7_1;
	IO_PRO.SEN.LVDS[ch].SYNCSEL.bit.SYNCSEL	= lvds_ctrl->lane_select;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
LVDS Input starting position setup .
@param[in]	ch : Channel No.
@param[in]	input_pos	: LVDS input position<br>
						 value range:[0 - 12288pixel](Multiples of 4)<br>
						 target registor:@@LHSADD
@param[in]	input_size : LVDS input size<br>
						 value range:[4 - ](Multiples of 4)<br>
						 maximum size that can be set by each SENCORE block set by SENTOP.SENRAMSW.<br>
						 target registor:@@LHSIZE
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
@remarks	Only in the case of @@SZEN = 1 (see Im_PRO_LVDS_Ctrl), the value set up by this API is effective.
*/
INT32 Im_PRO_LVDS_Set_Area( E_IM_PRO_LVDS_CH ch, USHORT input_pos, USHORT input_size )
{
#ifdef CO_PARAM_CHECK
	if( input_size < D_IM_PRO_LVDS_LHSIZE_MIN ) {
		Ddim_Assertion(("I:Im_PRO_LVDS_Set_Area. input param:[input_size] value over!! >= %d (%d)\n",D_IM_PRO_LVDS_LHSIZE_MIN ,input_size ));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( input_size > gim_pro_maximum_size_lvds[IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.SENRAMSW][ch] ) {
		Ddim_Assertion(("I:Im_PRO_LVDS_Set_Area. input param:[input_size] value over!! <= %d (%d)\n",gim_pro_maximum_size_lvds[IO_PRO.SEN.SENTOP.SENTOPCTL1.SENTOPCTL13.bit.SENRAMSW][ch] ,input_size ));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else {
		// DO NOTHING
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.LVDS[ch].LHSADD.bit.LHSADD	= input_pos;
	IO_PRO.SEN.LVDS[ch].LHSIZE.bit.LHSIZE	= input_size;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
LVDS output data rearrangement setup.
@param[in]	ch : Channel No.
@param[in]	lvds_order : LVDS pixel order information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
INT32 Im_PRO_LVDS_Set_PixelOrder( E_IM_PRO_LVDS_CH ch, T_IM_PRO_LVDS_PIXEL_ORDER* lvds_order )
{
#ifdef CO_PARAM_CHECK
	if (lvds_order == NULL){
		Ddim_Assertion(("I:Im_PRO_LVDS_Set_PixelOrder error. lvds_order=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELA	= lvds_order->ptn_a;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELB	= lvds_order->ptn_b;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELC	= lvds_order->ptn_c;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELD	= lvds_order->ptn_d;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELE	= lvds_order->ptn_e;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELF	= lvds_order->ptn_f;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELG	= lvds_order->ptn_g;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELH	= lvds_order->ptn_h;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELI	= lvds_order->ptn_i;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELJ	= lvds_order->ptn_j;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELK	= lvds_order->ptn_k;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELL	= lvds_order->ptn_l;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELM	= lvds_order->ptn_m;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELN	= lvds_order->ptn_n;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELO	= lvds_order->ptn_o;
	IO_PRO.SEN.LVDS[ch].LPDCHG.bit.SELP	= lvds_order->ptn_p;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
The sync code parameter of LVDS is set.
@param[in]	ch : Channel No.
@param[in]	sync_code : LVDS sync code information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
INT32 Im_PRO_LVDS_Set_SyncCode( E_IM_PRO_LVDS_CH ch, T_IM_PRO_LVDS_SYNC_CODE_CTRL* sync_code )
{
#ifdef CO_PARAM_CHECK
	if (sync_code == NULL){
		Ddim_Assertion(("I:Im_PRO_Set_SyncCode. sync_code=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.LVDS[ch].SAV0.SAV01.bit.SAV0_0 = sync_code->sync_code[0].sav[0];
	IO_PRO.SEN.LVDS[ch].SAV0.SAV01.bit.SAV0_1 = sync_code->sync_code[0].sav[1];
	IO_PRO.SEN.LVDS[ch].SAV0.SAV02.bit.SAV0_2 = sync_code->sync_code[0].sav[2];
	IO_PRO.SEN.LVDS[ch].SAV0.SAV02.bit.SAV0_3 = sync_code->sync_code[0].sav[3];

	IO_PRO.SEN.LVDS[ch].EAV0.EAV01.bit.EAV0_0 = sync_code->sync_code[0].eav[0];
	IO_PRO.SEN.LVDS[ch].EAV0.EAV01.bit.EAV0_1 = sync_code->sync_code[0].eav[1];
	IO_PRO.SEN.LVDS[ch].EAV0.EAV02.bit.EAV0_2 = sync_code->sync_code[0].eav[2];
	IO_PRO.SEN.LVDS[ch].EAV0.EAV02.bit.EAV0_3 = sync_code->sync_code[0].eav[3];

	IO_PRO.SEN.LVDS[ch].SAB0.SAB01.bit.SAB0_0 = sync_code->sync_code[0].sab[0];
	IO_PRO.SEN.LVDS[ch].SAB0.SAB01.bit.SAB0_1 = sync_code->sync_code[0].sab[1];
	IO_PRO.SEN.LVDS[ch].SAB0.SAB02.bit.SAB0_2 = sync_code->sync_code[0].sab[2];
	IO_PRO.SEN.LVDS[ch].SAB0.SAB02.bit.SAB0_3 = sync_code->sync_code[0].sab[3];

	IO_PRO.SEN.LVDS[ch].EAB0.EAB01.bit.EAB0_0 = sync_code->sync_code[0].eab[0];
	IO_PRO.SEN.LVDS[ch].EAB0.EAB01.bit.EAB0_1 = sync_code->sync_code[0].eab[1];
	IO_PRO.SEN.LVDS[ch].EAB0.EAB02.bit.EAB0_2 = sync_code->sync_code[0].eab[2];
	IO_PRO.SEN.LVDS[ch].EAB0.EAB02.bit.EAB0_3 = sync_code->sync_code[0].eab[3];


	IO_PRO.SEN.LVDS[ch].SAV1.SAV11.bit.SAV1_0 = sync_code->sync_code[1].sav[0];
	IO_PRO.SEN.LVDS[ch].SAV1.SAV11.bit.SAV1_1 = sync_code->sync_code[1].sav[1];
	IO_PRO.SEN.LVDS[ch].SAV1.SAV12.bit.SAV1_2 = sync_code->sync_code[1].sav[2];
	IO_PRO.SEN.LVDS[ch].SAV1.SAV12.bit.SAV1_3 = sync_code->sync_code[1].sav[3];

	IO_PRO.SEN.LVDS[ch].EAV1.EAV11.bit.EAV1_0 = sync_code->sync_code[1].eav[0];
	IO_PRO.SEN.LVDS[ch].EAV1.EAV11.bit.EAV1_1 = sync_code->sync_code[1].eav[1];
	IO_PRO.SEN.LVDS[ch].EAV1.EAV12.bit.EAV1_2 = sync_code->sync_code[1].eav[2];
	IO_PRO.SEN.LVDS[ch].EAV1.EAV12.bit.EAV1_3 = sync_code->sync_code[1].eav[3];

	IO_PRO.SEN.LVDS[ch].SAB1.SAB11.bit.SAB1_0 = sync_code->sync_code[1].sab[0];
	IO_PRO.SEN.LVDS[ch].SAB1.SAB11.bit.SAB1_1 = sync_code->sync_code[1].sab[1];
	IO_PRO.SEN.LVDS[ch].SAB1.SAB12.bit.SAB1_2 = sync_code->sync_code[1].sab[2];
	IO_PRO.SEN.LVDS[ch].SAB1.SAB12.bit.SAB1_3 = sync_code->sync_code[1].sab[3];

	IO_PRO.SEN.LVDS[ch].EAB1.EAB11.bit.EAB1_0 = sync_code->sync_code[1].eab[0];
	IO_PRO.SEN.LVDS[ch].EAB1.EAB11.bit.EAB1_1 = sync_code->sync_code[1].eab[1];
	IO_PRO.SEN.LVDS[ch].EAB1.EAB12.bit.EAB1_2 = sync_code->sync_code[1].eab[2];
	IO_PRO.SEN.LVDS[ch].EAB1.EAB12.bit.EAB1_3 = sync_code->sync_code[1].eab[3];

	IO_PRO.SEN.LVDS[ch].SAV2.SAV21.bit.SAV2_0 = sync_code->sync_code[2].sav[0];
	IO_PRO.SEN.LVDS[ch].SAV2.SAV21.bit.SAV2_1 = sync_code->sync_code[2].sav[1];
	IO_PRO.SEN.LVDS[ch].SAV2.SAV22.bit.SAV2_2 = sync_code->sync_code[2].sav[2];
	IO_PRO.SEN.LVDS[ch].SAV2.SAV22.bit.SAV2_3 = sync_code->sync_code[2].sav[3];

	IO_PRO.SEN.LVDS[ch].EAV2.EAV21.bit.EAV2_0 = sync_code->sync_code[2].eav[0];
	IO_PRO.SEN.LVDS[ch].EAV2.EAV21.bit.EAV2_1 = sync_code->sync_code[2].eav[1];
	IO_PRO.SEN.LVDS[ch].EAV2.EAV22.bit.EAV2_2 = sync_code->sync_code[2].eav[2];
	IO_PRO.SEN.LVDS[ch].EAV2.EAV22.bit.EAV2_3 = sync_code->sync_code[2].eav[3];

	IO_PRO.SEN.LVDS[ch].SAB2.SAB21.bit.SAB2_0 = sync_code->sync_code[2].sab[0];
	IO_PRO.SEN.LVDS[ch].SAB2.SAB21.bit.SAB2_1 = sync_code->sync_code[2].sab[1];
	IO_PRO.SEN.LVDS[ch].SAB2.SAB22.bit.SAB2_2 = sync_code->sync_code[2].sab[2];
	IO_PRO.SEN.LVDS[ch].SAB2.SAB22.bit.SAB2_3 = sync_code->sync_code[2].sab[3];

	IO_PRO.SEN.LVDS[ch].EAB2.EAB21.bit.EAB2_0 = sync_code->sync_code[2].eab[0];
	IO_PRO.SEN.LVDS[ch].EAB2.EAB21.bit.EAB2_1 = sync_code->sync_code[2].eab[1];
	IO_PRO.SEN.LVDS[ch].EAB2.EAB22.bit.EAB2_2 = sync_code->sync_code[2].eab[2];
	IO_PRO.SEN.LVDS[ch].EAB2.EAB22.bit.EAB2_3 = sync_code->sync_code[2].eab[3];
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
LVDS sync code Mask setup.
@param[in]	ch : Channel No.
@param[in]	sync_mask : LVDS sync mask information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
INT32 Im_PRO_LVDS_Set_SyncMask( E_IM_PRO_LVDS_CH ch, T_IM_PRO_LVDS_SYNC_MASK_CTRL* sync_mask )
{
#ifdef CO_PARAM_CHECK
	if (sync_mask == NULL){
		Ddim_Assertion(("I:Im_PRO_Set_SyncMask. sync_mask=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.LVDS[ch].MSAV0.MSAV01.bit.MSAV0_0 = sync_mask->sync_mask[0].msav[0];
	IO_PRO.SEN.LVDS[ch].MSAV0.MSAV01.bit.MSAV0_1 = sync_mask->sync_mask[0].msav[1];
	IO_PRO.SEN.LVDS[ch].MSAV0.MSAV02.bit.MSAV0_2 = sync_mask->sync_mask[0].msav[2];
	IO_PRO.SEN.LVDS[ch].MSAV0.MSAV02.bit.MSAV0_3 = sync_mask->sync_mask[0].msav[3];

	IO_PRO.SEN.LVDS[ch].MEAV0.MEAV01.bit.MEAV0_0 = sync_mask->sync_mask[0].meav[0];
	IO_PRO.SEN.LVDS[ch].MEAV0.MEAV01.bit.MEAV0_1 = sync_mask->sync_mask[0].meav[1];
	IO_PRO.SEN.LVDS[ch].MEAV0.MEAV02.bit.MEAV0_2 = sync_mask->sync_mask[0].meav[2];
	IO_PRO.SEN.LVDS[ch].MEAV0.MEAV02.bit.MEAV0_3 = sync_mask->sync_mask[0].meav[3];

	IO_PRO.SEN.LVDS[ch].MSAB0.MSAB01.bit.MSAB0_0 = sync_mask->sync_mask[0].msab[0];
	IO_PRO.SEN.LVDS[ch].MSAB0.MSAB01.bit.MSAB0_1 = sync_mask->sync_mask[0].msab[1];
	IO_PRO.SEN.LVDS[ch].MSAB0.MSAB02.bit.MSAB0_2 = sync_mask->sync_mask[0].msab[2];
	IO_PRO.SEN.LVDS[ch].MSAB0.MSAB02.bit.MSAB0_3 = sync_mask->sync_mask[0].msab[3];

	IO_PRO.SEN.LVDS[ch].MEAB0.MEAB01.bit.MEAB0_0 = sync_mask->sync_mask[0].meab[0];
	IO_PRO.SEN.LVDS[ch].MEAB0.MEAB01.bit.MEAB0_1 = sync_mask->sync_mask[0].meab[1];
	IO_PRO.SEN.LVDS[ch].MEAB0.MEAB02.bit.MEAB0_2 = sync_mask->sync_mask[0].meab[2];
	IO_PRO.SEN.LVDS[ch].MEAB0.MEAB02.bit.MEAB0_3 = sync_mask->sync_mask[0].meab[3];

	IO_PRO.SEN.LVDS[ch].MSAV1.MSAV11.bit.MSAV1_0 = sync_mask->sync_mask[1].msav[0];
	IO_PRO.SEN.LVDS[ch].MSAV1.MSAV11.bit.MSAV1_1 = sync_mask->sync_mask[1].msav[1];
	IO_PRO.SEN.LVDS[ch].MSAV1.MSAV12.bit.MSAV1_2 = sync_mask->sync_mask[1].msav[2];
	IO_PRO.SEN.LVDS[ch].MSAV1.MSAV12.bit.MSAV1_3 = sync_mask->sync_mask[1].msav[3];

	IO_PRO.SEN.LVDS[ch].MEAV1.MEAV11.bit.MEAV1_0 = sync_mask->sync_mask[1].meav[0];
	IO_PRO.SEN.LVDS[ch].MEAV1.MEAV11.bit.MEAV1_1 = sync_mask->sync_mask[1].meav[1];
	IO_PRO.SEN.LVDS[ch].MEAV1.MEAV12.bit.MEAV1_2 = sync_mask->sync_mask[1].meav[2];
	IO_PRO.SEN.LVDS[ch].MEAV1.MEAV12.bit.MEAV1_3 = sync_mask->sync_mask[1].meav[3];

	IO_PRO.SEN.LVDS[ch].MSAB1.MSAB11.bit.MSAB1_0 = sync_mask->sync_mask[1].msab[0];
	IO_PRO.SEN.LVDS[ch].MSAB1.MSAB11.bit.MSAB1_1 = sync_mask->sync_mask[1].msab[1];
	IO_PRO.SEN.LVDS[ch].MSAB1.MSAB12.bit.MSAB1_2 = sync_mask->sync_mask[1].msab[2];
	IO_PRO.SEN.LVDS[ch].MSAB1.MSAB12.bit.MSAB1_3 = sync_mask->sync_mask[1].msab[3];

	IO_PRO.SEN.LVDS[ch].MEAB1.MEAB11.bit.MEAB1_0 = sync_mask->sync_mask[1].meab[0];
	IO_PRO.SEN.LVDS[ch].MEAB1.MEAB11.bit.MEAB1_1 = sync_mask->sync_mask[1].meab[1];
	IO_PRO.SEN.LVDS[ch].MEAB1.MEAB12.bit.MEAB1_2 = sync_mask->sync_mask[1].meab[2];
	IO_PRO.SEN.LVDS[ch].MEAB1.MEAB12.bit.MEAB1_3 = sync_mask->sync_mask[1].meab[3];

	IO_PRO.SEN.LVDS[ch].MSAV2.MSAV21.bit.MSAV2_0 = sync_mask->sync_mask[2].msav[0];
	IO_PRO.SEN.LVDS[ch].MSAV2.MSAV21.bit.MSAV2_1 = sync_mask->sync_mask[2].msav[1];
	IO_PRO.SEN.LVDS[ch].MSAV2.MSAV22.bit.MSAV2_2 = sync_mask->sync_mask[2].msav[2];
	IO_PRO.SEN.LVDS[ch].MSAV2.MSAV22.bit.MSAV2_3 = sync_mask->sync_mask[2].msav[3];

	IO_PRO.SEN.LVDS[ch].MEAV2.MEAV21.bit.MEAV2_0 = sync_mask->sync_mask[2].meav[0];
	IO_PRO.SEN.LVDS[ch].MEAV2.MEAV21.bit.MEAV2_1 = sync_mask->sync_mask[2].meav[1];
	IO_PRO.SEN.LVDS[ch].MEAV2.MEAV22.bit.MEAV2_2 = sync_mask->sync_mask[2].meav[2];
	IO_PRO.SEN.LVDS[ch].MEAV2.MEAV22.bit.MEAV2_3 = sync_mask->sync_mask[2].meav[3];

	IO_PRO.SEN.LVDS[ch].MSAB2.MSAB21.bit.MSAB2_0 = sync_mask->sync_mask[2].msab[0];
	IO_PRO.SEN.LVDS[ch].MSAB2.MSAB21.bit.MSAB2_1 = sync_mask->sync_mask[2].msab[1];
	IO_PRO.SEN.LVDS[ch].MSAB2.MSAB22.bit.MSAB2_2 = sync_mask->sync_mask[2].msab[2];
	IO_PRO.SEN.LVDS[ch].MSAB2.MSAB22.bit.MSAB2_3 = sync_mask->sync_mask[2].msab[3];

	IO_PRO.SEN.LVDS[ch].MEAB2.MEAB21.bit.MEAB2_0 = sync_mask->sync_mask[2].meab[0];
	IO_PRO.SEN.LVDS[ch].MEAB2.MEAB21.bit.MEAB2_1 = sync_mask->sync_mask[2].meab[1];
	IO_PRO.SEN.LVDS[ch].MEAB2.MEAB22.bit.MEAB2_2 = sync_mask->sync_mask[2].meab[2];
	IO_PRO.SEN.LVDS[ch].MEAB2.MEAB22.bit.MEAB2_3 = sync_mask->sync_mask[2].meab[3];
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
LVDS Input starting position setup .
@param[in]	ch : Channel No.
@param[in]	int_addr	: SOL/EOL interrupt address information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_LVDS_Set_IntAdr( E_IM_PRO_LVDS_CH ch, T_IM_PRO_LVDS_INT_ADDR_CTRL* int_addr )
{
#ifdef CO_PARAM_CHECK
	if (int_addr == NULL){
		Ddim_Assertion(("I:Im_PRO_LVDS_Set_IntAdr. int_addr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	for( UCHAR loopcnt = 0; loopcnt <= IO_PRO.SEN.LVDS[ch].LVDSCTL2.bit.DOLMD; loopcnt++ ) {
		if( int_addr->int_addr[loopcnt].sol_addr > D_IM_PRO_LVDS_LVDSINTADR_MAX ) {
			Ddim_Assertion(("I:Im_PRO_LVDS_Set_IntAdr. input param:[sol_addr%d] value over!! <= %d (%d)\n", loopcnt, D_IM_PRO_LVDS_LVDSINTADR_MAX, int_addr->int_addr[loopcnt].sol_addr ));
			return D_IM_PRO_INPUT_PARAM_ERROR;
		}
		if( int_addr->int_addr[loopcnt].eol_addr > D_IM_PRO_LVDS_LVDSINTADR_MAX ) {
			Ddim_Assertion(("I:Im_PRO_LVDS_Set_IntAdr. input param:[eol_addr%d] value over!! <= %d (%d)\n", loopcnt, D_IM_PRO_LVDS_LVDSINTADR_MAX, int_addr->int_addr[loopcnt].eol_addr ));
			return D_IM_PRO_INPUT_PARAM_ERROR;
		}
	}
#endif

	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.LVDS[ch].LVDSINTADR0.bit.SOLVADR_0	= int_addr->int_addr[0].sol_addr;
	IO_PRO.SEN.LVDS[ch].LVDSINTADR0.bit.EOLVADR_0	= int_addr->int_addr[0].eol_addr;
	IO_PRO.SEN.LVDS[ch].LVDSINTADR1.bit.SOLVADR_1	= int_addr->int_addr[1].sol_addr;
	IO_PRO.SEN.LVDS[ch].LVDSINTADR1.bit.EOLVADR_1	= int_addr->int_addr[1].eol_addr;
	IO_PRO.SEN.LVDS[ch].LVDSINTADR2.bit.SOLVADR_2	= int_addr->int_addr[2].sol_addr;
	IO_PRO.SEN.LVDS[ch].LVDSINTADR2.bit.EOLVADR_2	= int_addr->int_addr[2].eol_addr;
	IO_PRO.SEN.LVDS[ch].LVDSINTSET0.word = 1;
	IO_PRO.SEN.LVDS[ch].LVDSINTSET1.word = 1;
	IO_PRO.SEN.LVDS[ch].LVDSINTSET2.word = 1;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of LVDS.
@param[in]	ch : Channel No.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_LVDS_Set_PAEN( E_IM_PRO_LVDS_CH ch, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.SEN.LVDS[ch].LVDSTRG.bit.LVDSTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_LVDS_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.LVDS[ch].LVDSPAEN.bit.PAEN	= paen_trg;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock

	return D_DDIM_OK;
}

// --- REMOVE_BUZ BEGIN
/**
SENTOP Pseudo VD Issue setting
@retval		D_DDIM_OK					: Setting OK
*/
INT32 Im_PRO_SENTOP_Pseudo_VD_Issue( VOID )
{
	im_pro_on_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	IO_PRO.SEN.SENTOP.PSEUDE_VD.word = 1;
	im_pro_off_pclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_SEN );	// Dd_Top_Start_Clock
	return D_DDIM_OK;
}
// --- REMOVE_BUZ END
