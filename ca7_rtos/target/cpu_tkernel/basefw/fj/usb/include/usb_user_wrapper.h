/**
 * @file		usb_user_wrapper.h
 * @brief		USB Driver User Wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
#ifndef _USB_USER_WRAPPER_H_
#define _USB_USER_WRAPPER_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Return value */
#define D_USB_USER_WRAP_OK		(0)
#define D_USB_USER_WRAP_NG		(-1)

/* Flag value */
#define D_USB_USER_WRAP_OFF 	(0)
#define D_USB_USER_WRAP_ON		(1)

/* Flag value */
#define D_USB_USER_PLUG_RELEASE	(0)
#define D_USB_USER_PLUG_CONNECT (1)

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

/**
 USB VBUS connect check.
@param[in]	chattering_flag		: chattering check flag(USB_USER_WRAP_OFF/USB_USER_WRAP_ON).
@return		D_USB_USER_PLUG_RELEASE/D_USB_USER_PLUG_CONNECT
@remarks	
*/
long USB_User_Wrapper_Check_Connect(long chattering_flag);


#ifdef __cplusplus
}
#endif

#endif /* _USB_USER_WRAPPER_H_ */
