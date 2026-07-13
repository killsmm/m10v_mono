/**
 * @file		usb_user_wrapper.c
 * @brief		USB Driver User Wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "usb_user_wrapper.h"
#include "os_user_custom.h"
#include "fj_peripheral.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_USB_USER_WRAPPER_VBUS_PORT			(FJ_GPIO_PM2)	// VBUS detect GPIO assign
#define D_USB_USER_WRAPPER_VBUS_CHECK_COUNT		(7)				// VBUS check count
#define D_USB_USER_WRAPPER_VBUS_CHECK_INTERVAL	(30)			// VBUS check interval(ms)

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
volatile long gUSB_User_Wrapper_Vbus_State = D_USB_USER_PLUG_RELEASE;	// VBUS old state

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		Get VBUS state.
 *	@param		None
 *	@return		D_USB_USER_PLUG_RELEASE/D_USB_USER_PLUG_CONNECT
 *	@note		None
 *	@attention	None
 */
static long usb_user_wrapper_get_vbus_state( void )
{
	long            retcond;
	unsigned char	connect;
	
	(void)FJ_Gpio_Get_Status( D_USB_USER_WRAPPER_VBUS_PORT, &connect );
	
	if ( connect == 1 ) {
		retcond = D_USB_USER_PLUG_CONNECT;
	}
	else {
		retcond = D_USB_USER_PLUG_RELEASE;
	}
	
	return retcond;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		USB VBUS connect check
 *	@param		chatteringFlag	:chattering check enable or disable
 *	@return		D_USB_USER_PLUG_CONNECT/D_USB_USER_PLUG_RELEASE
 *	@note		None
 *	@attention	None
 */
long USB_User_Wrapper_Check_Connect( long chattering_flag )
{
	long ret = D_USB_USER_WRAP_NG;
	long connect1 = 0;  /* old connect status */
	long connect2 = 0;  /* new connect status */
	unsigned long i;
	
	connect1 = usb_user_wrapper_get_vbus_state();
	
	if ( chattering_flag == D_USB_USER_WRAP_ON ) {
		/* chattering reduction */
		
		for ( i = 0; i < D_USB_USER_WRAPPER_VBUS_CHECK_COUNT; i++ ) {
			/* wait */
			OS_User_Tslp_Tsk( D_USB_USER_WRAPPER_VBUS_CHECK_INTERVAL );
			
			connect2 = usb_user_wrapper_get_vbus_state();
			
			if ( connect1 == connect2 ) {
				/* Set connect status to global variable (gUSB_User_Wrapper_Vbus_State) */
				gUSB_User_Wrapper_Vbus_State = connect1;
				ret = connect1;
				break;
			} else {
				/* Set connect status to previous connect status variable */
				connect1 = connect2;
			}
		}
		
		if ( ret == D_USB_USER_WRAP_NG ) {
			/* Set old connect status to return value */
			ret = gUSB_User_Wrapper_Vbus_State;
		}
		
	} else { /* chattering_flag == D_USB_USER_WRAP_OFF */
		ret = connect1;
	}
	return ret;
}

