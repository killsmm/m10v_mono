/**
 * @file		device_config.c
 * @brief		device configure api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dd_top.h"
#include "fj_peripheral.h"
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

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
UCHAR DUMMY_PORT;	// Mean that no exsits or not support

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
/**
Set GPIO function (EPCR)
*/
FJ_ERR_CODE	fj_gpio_set_function( FJ_GPIO_PORT port, UCHAR function )
{
	INT32 ret;

	ret = Dd_Top_Set_Gpio_Function( (E_DD_TOP_GPIO_PORT)port, function );

	if( ret == D_DDIM_OK ) {
		return FJ_ERR_OK;
	}
	else{
		BF_Debug_Print_Error(( "[fj_gpio_set_function() - Error!] Port=%d, Func=%d, ret=%d\n", port, function, ret ));
		return FJ_ERR_NG;
	}
}

/**
Set GPIO direction (DDR)
*/
FJ_ERR_CODE	fj_gpio_set_direction( FJ_GPIO_PORT port, UCHAR direction )
{
	INT32 ret;

	ret = Dd_Top_Set_Gpio_Direction( (E_DD_TOP_GPIO_PORT)port, direction );

	if( ret == D_DDIM_OK ) {
		return FJ_ERR_OK;
	}
	else{
		BF_Debug_Print_Error(( "[fj_gpio_set_direction() - Error!] Port=%d, Dir=%d, ret=%d\n", port, direction, ret ));
		return FJ_ERR_NG;
	}
}

/**
Set GPIO status (PDR)
*/
FJ_ERR_CODE	fj_gpio_set_status( FJ_GPIO_PORT port, UCHAR status )
{
	INT32 ret;

	ret = Dd_Top_Set_Gpio_Status( (E_DD_TOP_GPIO_PORT)port, status );

	if( ret == D_DDIM_OK ) {
		return FJ_ERR_OK;
	}
	else{
		BF_Debug_Print_Error(( "[fj_gpio_set_status() - Error!] Port=%d, Status=%d, ret=%d\n", port, status, ret ));
		return FJ_ERR_NG;
	}
}

/**
Get GPIO status (PDR)
*/
FJ_ERR_CODE	fj_gpio_get_status( FJ_GPIO_PORT port, UCHAR* status )
{
	INT32 ret;

	ret = Dd_Top_Get_Gpio_Status( (E_DD_TOP_GPIO_PORT)port, status );

	if( ret == D_DDIM_OK ) {
		return FJ_ERR_OK;
	}
	else{
		BF_Debug_Print_Error(( "[fj_gpio_get_status() - Error!] Port=%d, ret=%d\n", port, ret ));
		return FJ_ERR_NG;
	}
}

/**
Set GPIO Pull-up/down enable (PUDER)
*/
FJ_ERR_CODE	fj_gpio_set_pull_up_down_enable( FJ_GPIO_PORT port, UCHAR enable )
{
	INT32 ret;

	ret = Dd_Top_Set_Gpio_Pull_Up_Down_Enable( (E_DD_TOP_GPIO_PORT)port, enable );

	if( ret == D_DDIM_OK ) {
		return FJ_ERR_OK;
	}
	else{
		BF_Debug_Print_Error(( "[fj_gpio_set_pull_up_down_enable() - Error!] Port=%d, Enable=%d, ret=%d\n", port, enable, ret ));
		return FJ_ERR_NG;
	}
}

/**
Set GPIO Pull-up/down control (PUDCR)
*/
FJ_ERR_CODE	fj_gpio_set_pull_up_down_ctrl( FJ_GPIO_PORT port, UCHAR control )
{
	INT32 ret;

	ret = Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl( (E_DD_TOP_GPIO_PORT)port, control );

	if( ret == D_DDIM_OK ) {
		return FJ_ERR_OK;
	}
	else{
		BF_Debug_Print_Error(( "[fj_gpio_pud_ctrl() - Error!] Port=%d, Ctrl=%d, ret=%d\n", port, control, ret ));
		return FJ_ERR_NG;
	}
}

