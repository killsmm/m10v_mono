/**
 * @file		bus_arb.h
 * @brief		BUS Arbitration control
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _BUS_ARB_H_
#define _BUS_ARB_H_

// FJ header
#include "fj_busarb.h"

// Driver header
#include "ddim_typedef.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


extern	FJ_ERR_CODE	fj_busarb_set_mxicoutputportall( const T_FJ_BUS_ARB_MXIC_OUTPUT_PORT* const out_port );
extern	FJ_ERR_CODE	fj_busarb_set_mxicmasterallarbiter( const E_FJ_BUS_ARB_MXIC_UNIT unit, const T_FJ_BUS_ARB_MXIC_ALL_ARBITER_ASSIGN* const all_assign );
extern	FJ_ERR_CODE	fj_busarb_set_mxicslotpriorityallarbiter( const E_FJ_BUS_ARB_MXIC_UNIT unit, const T_FJ_BUS_ARB_MXIC_ALL_SLOT_PRIORITY_LEVEL* const all_priority );
extern	FJ_ERR_CODE	fj_busarb_set_mxicmastertransaction( const E_FJ_BUS_ARB_MXIC_UNIT unit, const UCHAR master, const UCHAR transaction );
extern	FJ_ERR_CODE	fj_busarb_set_mxicbootdefault( VOID );

extern	FJ_ERR_CODE fj_busarb_set_sdramc_cmd_arbitration_rate( const E_FJ_SUB_ARB_SDRAMC_CH ch, const T_FJ_SUB_ARB_SDRAMC_CMD_ARB_RATE* const cmd_arb );
extern	FJ_ERR_CODE fj_busarb_set_sdramc_cmd_arbitration_priority( const E_FJ_SUB_ARB_SDRAMC_CH ch, const T_FJ_SUB_ARB_SDRAMC_CMD_ARB_PRI* const cmd_arb );
extern	FJ_ERR_CODE	fj_busarb_set_sdramcbootdefault( VOID );


#ifdef __cplusplus
}
#endif

#endif	// _BUS_ARB_H_

