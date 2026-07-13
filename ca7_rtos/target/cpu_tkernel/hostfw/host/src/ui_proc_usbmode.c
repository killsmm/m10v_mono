/**
 * @file		ui_proc_usbmode.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */
	 
#include "fj_host.h"
#ifdef CO_UI_PROC
#include "button.h"
#include "ui_proc_main.h"
#include "ui_proc_usbmode.h"
#include "category_parameter.h"

#include "os_if.h"

#include "fj_message.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define UI_PROC_USBMODE_LINUX_CMD_START_UVC		"sh /usr/bin/uvc_ipcu.sh -usb3 -c 1"
#define UI_PROC_USBMODE_LINUX_CMD_STOP_UVC		"sh /usr/bin/uvc_stop.sh"

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
typedef enum{
	E_UI_USBMODE_SET_INIT = 0,
	E_UI_USBMODE_SET_MODE = 0,
	E_UI_USBMODE_SET_MAX
} E_UI_USBMODE_SET;

typedef enum{
	E_UI_USBMODE_KEY_UP = 0,
	E_UI_USBMODE_KEY_DOWN,
	E_UI_USBMODE_KEY_LEFT,
	E_UI_USBMODE_KEY_RIGHT,
	E_UI_USBMODE_KEY_ENTER,
	E_UI_USBMODE_KEY_ZOOM_IN,
	E_UI_USBMODE_KEY_ZOOM_OUT,
	E_UI_USBMODE_KEY_HALF_SHUTTER,
	E_UI_USBMODE_KEY_FULL_SHUTTER,
	E_UI_USBMODE_KEY_MAX
} E_UI_USBMODE_KEY;

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static BOOL		gUI_UsbMode_ConnectEnable = FALSE;

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static ULONG ui_proc_usbmode_execution_dummy(ULONG param);
static ULONG ui_proc_usbmode_execution_uvc(ULONG param);

/*==============================================================================*/
/* UI Process Table																*/
/*==============================================================================*/
static T_UI_PROC_CUR_TBL gUI_UsbMode_SetCur[E_UI_USBMODE_SET_MAX] = {
	[E_UI_USBMODE_SET_MODE] = {
		.exec.func = ui_proc_usbmode_execution_uvc
	},
};

static T_UI_PROC_FUNC gUI_UsbMode_KeyEventTbl[E_UI_USBMODE_KEY_MAX] = {
	[E_UI_USBMODE_KEY_UP]			= {ui_proc_usbmode_execution_dummy},
	[E_UI_USBMODE_KEY_DOWN] 		= {ui_proc_usbmode_execution_dummy},
	[E_UI_USBMODE_KEY_LEFT] 		= {ui_proc_usbmode_execution_dummy},
	[E_UI_USBMODE_KEY_RIGHT]		= {ui_proc_usbmode_execution_dummy},
	[E_UI_USBMODE_KEY_ENTER]		= {ui_proc_usbmode_execution_dummy},
	[E_UI_USBMODE_KEY_ZOOM_IN]		= {ui_proc_usbmode_execution_dummy},
	[E_UI_USBMODE_KEY_ZOOM_OUT] 	= {ui_proc_usbmode_execution_dummy},
	[E_UI_USBMODE_KEY_HALF_SHUTTER] = {ui_proc_usbmode_execution_dummy},
	#ifdef UI_PROC_AUTO_VIEW_ON
	[E_UI_USBMODE_KEY_FULL_SHUTTER] = {UI_Proc_UsbMode_Exit_StillMsg},
	#else
	[E_UI_USBMODE_KEY_FULL_SHUTTER] = {ui_proc_usbmode_execution_dummy},
	#endif
};

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
ULONG UI_Proc_UsbMode_Init(ULONG param)
{
	INT32 ret = 0;
	ULONG data[16];

	data[0] = 0x00;
	ret = Cate_Set( 0x01, 0x03, 1, data ); // temporary to disable audio
	UPRINTF_ERR(ret);	

	data[0] = 0x1E;
	ret = Cate_Set( 0x01, 0x19, 1, data ); // H264_0_BITRATE = 30Mbps
	UPRINTF_ERR(ret);	

	gUI_UsbMode_ConnectEnable = FALSE;
	UI_Proc_UsbMode_Start(param);

	return TRUE;	
}

ULONG UI_Proc_UsbMode_Start(ULONG param)
{
	if(gUI_UsbMode_ConnectEnable == FALSE){
		if((gUI_UsbMode_SetCur[E_UI_USBMODE_SET_MODE].exec.func) != NULL){
			(gUI_UsbMode_SetCur[E_UI_USBMODE_SET_MODE].exec.func)(param);
			gUI_UsbMode_ConnectEnable = TRUE;
			return TRUE;	
		}
	}

	return FALSE;	
}

ULONG UI_Proc_UsbMode_Stop(ULONG param)
{
	if(gUI_UsbMode_ConnectEnable == TRUE){
		if((gUI_UsbMode_SetCur[E_UI_USBMODE_SET_MODE].exec.func) != NULL){
			(gUI_UsbMode_SetCur[E_UI_USBMODE_SET_MODE].exec.func)(param);
			gUI_UsbMode_ConnectEnable = FALSE;
			return TRUE;	
		}
	}
	
	return FALSE;	
}

ULONG UI_Proc_UsbMode_Exit_StillMsg(ULONG param)
{
	FJ_HostPostMessage(FJ_HM_NEW_MODE, E_UI_PROC_OP_MODE_STILL);
	
	return TRUE;	
}

ULONG UI_Proc_UsbMode_EventTrig(ULONG param)
{
	T_UI_PROC_MSG *msg = (T_UI_PROC_MSG *)param;
	
	switch(msg->msgid)
	{
		case FJ_HM_BUTTON:
			switch(msg->ulPara1){
				case MSG_BUTTON_UP_PUSH:
					(gUI_UsbMode_KeyEventTbl[E_UI_USBMODE_KEY_UP].func)(E_UI_USBMODE_KEY_UP);
				break;
				case MSG_BUTTON_DOWN_PUSH:
					(gUI_UsbMode_KeyEventTbl[E_UI_USBMODE_KEY_DOWN].func)(E_UI_USBMODE_KEY_DOWN);
				break;
				case MSG_BUTTON_LEFT_PUSH:
					(gUI_UsbMode_KeyEventTbl[E_UI_USBMODE_KEY_LEFT].func)(E_UI_USBMODE_KEY_LEFT);
				break;
				case MSG_BUTTON_RIGHT_PUSH:
					(gUI_UsbMode_KeyEventTbl[E_UI_USBMODE_KEY_RIGHT].func)(E_UI_USBMODE_KEY_RIGHT);
				break;
				case MSG_BUTTON_ENTER_PUSH:
					(gUI_UsbMode_KeyEventTbl[E_UI_USBMODE_KEY_ENTER].func)(E_UI_USBMODE_KEY_ENTER);
				break;
				case MSG_BUTTON_ZOOM_IN_PUSH:
					(gUI_UsbMode_KeyEventTbl[E_UI_USBMODE_KEY_ZOOM_IN].func)(E_UI_USBMODE_KEY_ZOOM_IN);
				break;
				case MSG_BUTTON_ZOOM_OUT_PUSH:
					(gUI_UsbMode_KeyEventTbl[E_UI_USBMODE_KEY_ZOOM_OUT].func)(E_UI_USBMODE_KEY_ZOOM_OUT);
				break;
				case MSG_BUTTON_HALF_SHUTTER_PUSH:
					(gUI_UsbMode_KeyEventTbl[E_UI_USBMODE_KEY_HALF_SHUTTER].func)(MSG_BUTTON_HALF_SHUTTER_PUSH);
				break;
				case MSG_BUTTON_HALF_SHUTTER_RELEASE:
					(gUI_UsbMode_KeyEventTbl[E_UI_USBMODE_KEY_HALF_SHUTTER].func)(MSG_BUTTON_HALF_SHUTTER_RELEASE);
				break;
				case MSG_BUTTON_FULL_SHUTTER_PUSH:
					(gUI_UsbMode_KeyEventTbl[E_UI_USBMODE_KEY_FULL_SHUTTER].func)(E_UI_USBMODE_KEY_FULL_SHUTTER);
				break;
				default:
				break;
			}
		break;
		default:
		break;
	}

	return TRUE;	
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static ULONG ui_proc_usbmode_execution_dummy(ULONG param)
{
	
	return TRUE;	
}

static ULONG ui_proc_usbmode_execution_uvc(ULONG param)
{
	#ifdef UI_PROC_AUTO_VIEW_ON
	INT32 ret = 0;
	ULONG data[16];

	data[0] = 0x02;
	ret = Cate_Set( 0x00, 0x0B, 1, data ); // Start View
	UPRINTF_ERR(ret);	
	#else
	UCHAR sync = 0;
	char cmd[128] = {0};
	int size, i;

	OS_USER_ER	ercd;
	OS_USER_FLGPTN flgptn = 0;
	
	if(sync == 0){
		ercd = OS_User_Twai_Flg(FID_LINUX_CMD, 0x1, D_OS_USER_TWF_ANDW, &flgptn, 10000);
		if( ercd != D_OS_USER_E_OK ){
			UPRINTF("%s(): Time out ercd=0x%x\n", __func__, ercd );
		}
	}
	
	if(param == UI_PROC_USBMODE_CONNECT_OFF){
		size = sizeof(UI_PROC_USBMODE_LINUX_CMD_STOP_UVC);
		memcpy(cmd, (char *)UI_PROC_USBMODE_LINUX_CMD_STOP_UVC, size);
	}
	else{
		size = sizeof(UI_PROC_USBMODE_LINUX_CMD_START_UVC);
		memcpy(cmd, (char *)UI_PROC_USBMODE_LINUX_CMD_START_UVC, size);
	}
	OS_IF_Rtos_Linux_CMD(&cmd[0], size, sync);

	if((param == UI_PROC_USBMODE_CONNECT_OFF) && (sync == 0)){
		for(i = 0; (Cate_Get_IPCU_Streaming() != FALSE); i++, OS_User_Dly_Tsk(10));
	}
	#endif
	
	return TRUE;	
}

#endif

