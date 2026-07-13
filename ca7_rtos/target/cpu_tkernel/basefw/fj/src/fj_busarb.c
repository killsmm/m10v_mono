/**
 * @file		fj_busarb.c
 * @brief		BUS Arbitration API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// FJ header
#include "fj_busarb.h"

// System header
#include "bus_arb.h"
#include "mode.h"


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define MODE_CHECK_ALL		(D_BF_MODE_VIEW_CODE|D_BF_MODE_OTHER_CODE|D_BF_MODE_CATEGORY_ALL)


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
// nothing special.


/*==============================================================================*/
/* Local Method Definition														*/
/*==============================================================================*/
// nothing special.


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

FJ_ERR_CODE	FJ_BusArb_Set_MxicOutputPortAll( const T_FJ_BUS_ARB_MXIC_OUTPUT_PORT* const out_port )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	ret = BF_Mode_Check( MODE_CHECK_ALL );
	if( ret == FJ_ERR_OK ) {
		ret = fj_busarb_set_mxicoutputportall( out_port );
	}

	return ret;
}

FJ_ERR_CODE	FJ_BusArb_Set_MxicMasterAllArbiter( const E_FJ_BUS_ARB_MXIC_UNIT unit, const T_FJ_BUS_ARB_MXIC_ALL_ARBITER_ASSIGN* const all_assign )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	ret = BF_Mode_Check( MODE_CHECK_ALL );
	if( ret == FJ_ERR_OK ) {
		ret = fj_busarb_set_mxicmasterallarbiter( unit, all_assign );
	}

	return ret;
}

FJ_ERR_CODE	FJ_BusArb_Set_MxicSlotPriorityAllArbiter( const E_FJ_BUS_ARB_MXIC_UNIT unit, const T_FJ_BUS_ARB_MXIC_ALL_SLOT_PRIORITY_LEVEL* const all_priority )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	ret = BF_Mode_Check( MODE_CHECK_ALL );
	if( ret == FJ_ERR_OK ) {
		ret = fj_busarb_set_mxicslotpriorityallarbiter( unit, all_priority );
	}

	return ret;
}

FJ_ERR_CODE FJ_BusArb_Set_MxicMasterTransaction( const E_FJ_BUS_ARB_MXIC_UNIT unit, const UCHAR master, const UCHAR transaction )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	ret = BF_Mode_Check( MODE_CHECK_ALL );
	if( ret == FJ_ERR_OK ) {
		ret = fj_busarb_set_mxicmastertransaction( unit, master, transaction );
	}

	return ret;
}

FJ_ERR_CODE	FJ_BusArb_Set_MxicBootDefault( VOID )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	ret = BF_Mode_Check( MODE_CHECK_ALL );
	if( ret == FJ_ERR_OK ) {
		ret = fj_busarb_set_mxicbootdefault();
	}

	return ret;
}


FJ_ERR_CODE FJ_BusArb_Set_Sdramc_Cmd_Arbitration_Rate( const E_FJ_SUB_ARB_SDRAMC_CH ch, const T_FJ_SUB_ARB_SDRAMC_CMD_ARB_RATE* const cmd_arb )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	ret = BF_Mode_Check( MODE_CHECK_ALL );
	if( ret == FJ_ERR_OK ) {
		ret = fj_busarb_set_sdramc_cmd_arbitration_rate( ch, cmd_arb );
	}

	return ret;
}

FJ_ERR_CODE FJ_BusArb_Set_Sdramc_Cmd_Arbitration_Priority( const E_FJ_SUB_ARB_SDRAMC_CH ch, const T_FJ_SUB_ARB_SDRAMC_CMD_ARB_PRI* const cmd_arb )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	ret = BF_Mode_Check( MODE_CHECK_ALL );
	if( ret == FJ_ERR_OK ) {
		ret = fj_busarb_set_sdramc_cmd_arbitration_priority( ch, cmd_arb );
	}

	return ret;
}

FJ_ERR_CODE	FJ_BusArb_Set_SdramcBootDefault( VOID )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	ret = BF_Mode_Check( MODE_CHECK_ALL );
	if( ret == FJ_ERR_OK ) {
		ret = fj_busarb_set_sdramcbootdefault();
	}

	return ret;
}

