/**
 * @file		dynamic_scenario.c
 * @brief		Dynamic scenario mode I/F
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "dynamic_scenario.h"
#include "fs_if.h"
#include "debug.h"

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
static CHAR gDynamic_Scenario_Action[D_BF_DYNAMIC_SCENARIO_ACTION_LIMIT + 1] = {0};	// [0] is not use

/*----------------------------------------------------------------------*/
/* Macro														 		*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static INT32 dynamic_scenario_is_file_exist( const char* filepath )
{
	INT32 result;
	UCHAR attr;

	result = BF_Fs_If_Get_Attribute( (const char*)filepath, &attr);
	BF_Fs_If_End_Task_Mdf();
	
	if (result == 0) {
		return 1; // exist
	}
	return 0; // not exist
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		Dynamic Scenario Load
 *	@param		none
 *	@return		-1		= Fail - Action not registered
 *				0		= Not Scenario Mode
 *				Other	= Number of Action
 *	@attention	This function must call from boot sequence only.
 */
INT32 BF_Dynamic_Scenario( VOID )
{
	INT32 num_of_action = 0;
	INT32 index = 0;
	char act_filepath[16];

	// Key file check
	if( !dynamic_scenario_is_file_exist( D_BF_DYNAMIC_SCENARIO_KEY_FILEPATH ) ){
		return 0;
	}

	// Action file check
	for( index = 1; index <= D_BF_DYNAMIC_SCENARIO_ACTION_LIMIT; index++ ){
		sprintf( (char*)act_filepath, D_BF_DYNAMIC_SCENARIO_ACT_FILEPATH, index );
		if( dynamic_scenario_is_file_exist( act_filepath ) ) {
			gDynamic_Scenario_Action[index] = 1;
			++num_of_action;
		}
	}

	return num_of_action;
}

/**
 *	@brief		Action Scenario Status Check
 *	@param		i:action_no		= Action File No.
 *	@return		-1				= Fail - Parameter error
 *				 0				= Disable
 *				 1				= Enable
 *	@attention	None
 */
INT32 BF_Dynamic_Scenario_Is_Enable( UINT32 action_no )
{
	// parameter check
	if( action_no == 0 ){
		return -1;
	}
	if( action_no > D_BF_DYNAMIC_SCENARIO_ACTION_LIMIT ){
		return -1;
	}

	return gDynamic_Scenario_Action[action_no];
}

/**
 *	@brief		Dynamic Scenario Set Status
 *	@param		i:action_no		= Action File No.
 *	@param		i:status		= 0:Disable, 1:Enable
 *	@return		-1				= Fail - Parameter error
 *				 0				= Nomal End
 *	@attention	None
 */
INT32 BF_Dynamic_Scenario_Set_Status( UINT32 action_no, UINT32 status )
{
	// parameter check
	if( (action_no == 0) || (action_no > D_BF_DYNAMIC_SCENARIO_ACTION_LIMIT) ){
		return -1;
	}
	if( !((status == 0) || (status == 1)) ){
		return -1;
	}

	gDynamic_Scenario_Action[action_no] = status;

	return 0;
}
