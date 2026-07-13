/**
 * @file		bf_arm.c
 * @brief		arm process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "bf_arm.h"
#include "dd_arm.h"
#include "dd_timestamp.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
UINT32 BF_Arm_Lock( UINT32* spin_id )
{
	UINT32 sr;
	
	sr = Dd_ARM_DI();
	Dd_ARM_Spin_Lock((ULONG*)spin_id);
	return sr;
}

UINT32 BF_Arm_Lock_Pol( UINT32* spin_id )
{
	UINT32 sr;
	UINT32 ret;
	
	while(1){
		sr = Dd_ARM_DI();
		ret = Dd_ARM_Spin_Lock_Ret((ULONG*)spin_id);
		if( ret==0 ){
			break;
		}
		else{
			Dd_ARM_EI(sr);
		}
	}
	return sr;
}

VOID BF_Arm_Unlock( UINT32* spin_id, UINT32 sr )
{
	Dd_ARM_Spin_Unlock((ULONG*)spin_id);
	Dd_ARM_EI(sr);
}

UINT32 fj_critical_section_start( UINT32* spin_id )
{
	return BF_Arm_Lock(spin_id);
}


UINT32 fj_critical_section_pol_start( UINT32* spin_id )
{
	return BF_Arm_Lock_Pol(spin_id);
}

VOID fj_critical_section_end( UINT32* spin_id, UINT32 sr )
{
	BF_Arm_Unlock(spin_id, sr);
}

UINT64 fj_timestamp_get_counter( VOID )
{
	UINT64 timestamp_counter;

	Dd_TIMESTAMP_Get_Counter( &timestamp_counter );

	return timestamp_counter;
}
