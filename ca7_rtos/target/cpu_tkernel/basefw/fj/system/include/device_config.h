/**
 * @file		device_config.h
 * @brief		device configure api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _DEVICE_CONFIG_H_
#define _DEVICE_CONFIG_H_

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

FJ_ERR_CODE	fj_gpio_set_function( FJ_GPIO_PORT port_name, UCHAR function );
FJ_ERR_CODE	fj_gpio_set_direction( FJ_GPIO_PORT port_name, UCHAR direction );
FJ_ERR_CODE	fj_gpio_set_status( FJ_GPIO_PORT port_name, UCHAR status );
FJ_ERR_CODE	fj_gpio_get_status( FJ_GPIO_PORT port_name, UCHAR* status );
FJ_ERR_CODE	fj_gpio_set_pull_up_down_enable( FJ_GPIO_PORT port_name, UCHAR enable );
FJ_ERR_CODE	fj_gpio_set_pull_up_down_ctrl( FJ_GPIO_PORT port_name, UCHAR control );

#ifdef __cplusplus
}
#endif

#endif	// _DEVICE_CONFIG_H_
