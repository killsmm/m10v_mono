/**
 * @file		arm.h
 * @brief		arm process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _ARM_H_
#define _ARM_H_

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

UINT32 BF_Arm_Lock( UINT32* spin_id );
UINT32 BF_Arm_Lock_Pol( UINT32* spin_id );
VOID BF_Arm_Unlock( UINT32* spin_id, UINT32 sr );
UINT32 fj_critical_section_start(UINT32* spin_id);
UINT32 fj_critical_section_pol_start(UINT32* spin_id);
VOID fj_critical_section_end(UINT32* spin_id, UINT32 sr);
UINT64 fj_timestamp_get_counter( VOID );

#ifdef __cplusplus
}
#endif

#endif	// _ARM_H_
