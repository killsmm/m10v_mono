/**
 * @file		ui_proc_stillmode.c
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
#include "ui_proc_stillmode.h"
#include "category_parameter.h"
#include "camera_use.h"

#include "fj_message.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
typedef enum{
	E_UI_STILLMODE_SET_INIT = 0,
	E_UI_STILLMODE_SET_CAP = 0,
	E_UI_STILLMODE_SET_MAX
} E_UI_STILLMODE_SET;

typedef enum{
	E_UI_STILLMODE_KEY_UP = 0,
	E_UI_STILLMODE_KEY_DOWN,
	E_UI_STILLMODE_KEY_LEFT,
	E_UI_STILLMODE_KEY_RIGHT,
	E_UI_STILLMODE_KEY_ENTER,
	E_UI_STILLMODE_KEY_ZOOM_IN,
	E_UI_STILLMODE_KEY_ZOOM_OUT,
	E_UI_STILLMODE_KEY_HALF_SHUTTER,
	E_UI_STILLMODE_KEY_FULL_SHUTTER,
	E_UI_STILLMODE_KEY_MAX
} E_UI_STILLMODE_KEY;

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static ULONG ui_proc_stillmode_execution_dummy(ULONG param);
static ULONG ui_proc_stillmode_execution_cap(ULONG param);

/*==============================================================================*/
/* UI Process Table																*/
/*==============================================================================*/

static T_UI_PROC_CUR_TBL gUI_StillMode_SetCur[E_UI_STILLMODE_SET_MAX] = {
	[E_UI_STILLMODE_SET_CAP] = {
		.exec.func = ui_proc_stillmode_execution_cap
	},
};

static T_UI_PROC_FUNC gUI_StillMode_KeyEventTbl[E_UI_STILLMODE_KEY_MAX] = {
	[E_UI_STILLMODE_KEY_UP] 			= {ui_proc_stillmode_execution_dummy},
	[E_UI_STILLMODE_KEY_DOWN] 			= {ui_proc_stillmode_execution_dummy},
	[E_UI_STILLMODE_KEY_LEFT] 			= {ui_proc_stillmode_execution_dummy},
	[E_UI_STILLMODE_KEY_RIGHT] 			= {ui_proc_stillmode_execution_dummy},
	[E_UI_STILLMODE_KEY_ENTER] 			= {ui_proc_stillmode_execution_dummy},
	[E_UI_STILLMODE_KEY_ZOOM_IN]		= {ui_proc_stillmode_execution_dummy},
	[E_UI_STILLMODE_KEY_ZOOM_OUT]		= {ui_proc_stillmode_execution_dummy},
	[E_UI_STILLMODE_KEY_HALF_SHUTTER]	= {ui_proc_stillmode_execution_dummy},
	[E_UI_STILLMODE_KEY_FULL_SHUTTER]	= {ui_proc_stillmode_execution_dummy},
};

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
ULONG UI_Proc_StillMode_Init(ULONG param)
{
	UI_Proc_StillMode_Start(param);

	return TRUE;	
}

ULONG UI_Proc_StillMode_Start(ULONG param)
{
	if((gUI_StillMode_SetCur[E_UI_STILLMODE_SET_CAP].exec.func) != NULL){
		(gUI_StillMode_SetCur[E_UI_STILLMODE_SET_CAP].exec.func)(param);
		return TRUE;	
	}
	
	return FALSE;	
}

ULONG UI_Proc_StillMode_Stop(ULONG param)
{
	INT32 ret = 0;
	ULONG data[16];

	data[0] = 0x01;
	ret = Cate_Set( 0x00, 0x0B, 1, data );
	UPRINTF_ERR(ret);	
	
	return TRUE;	
}

ULONG UI_Proc_StillMode_Exit_UsbMsg(ULONG param)
{
	FJ_HostPostMessage(FJ_HM_NEW_MODE, E_UI_PROC_OP_MODE_USB);
	
	return TRUE;	
}

ULONG UI_Proc_StillMode_EventTrig(ULONG param)
{
	T_UI_PROC_MSG *msg = (T_UI_PROC_MSG *)param;
	
	switch(msg->msgid)
	{
		case FJ_HM_BUTTON:
			switch(msg->ulPara1){
				case MSG_BUTTON_UP_PUSH:
					(gUI_StillMode_KeyEventTbl[E_UI_STILLMODE_KEY_UP].func)(E_UI_STILLMODE_KEY_UP);
				break;
				case MSG_BUTTON_DOWN_PUSH:
					(gUI_StillMode_KeyEventTbl[E_UI_STILLMODE_KEY_DOWN].func)(E_UI_STILLMODE_KEY_DOWN);
				break;
				case MSG_BUTTON_LEFT_PUSH:
					(gUI_StillMode_KeyEventTbl[E_UI_STILLMODE_KEY_LEFT].func)(E_UI_STILLMODE_KEY_LEFT);
				break;
				case MSG_BUTTON_RIGHT_PUSH:
					(gUI_StillMode_KeyEventTbl[E_UI_STILLMODE_KEY_RIGHT].func)(E_UI_STILLMODE_KEY_RIGHT);
				break;
				case MSG_BUTTON_ENTER_PUSH:
					(gUI_StillMode_KeyEventTbl[E_UI_STILLMODE_KEY_ENTER].func)(E_UI_STILLMODE_KEY_ENTER);
				break;
				case MSG_BUTTON_ZOOM_IN_PUSH:
					(gUI_StillMode_KeyEventTbl[E_UI_STILLMODE_KEY_ZOOM_IN].func)(E_UI_STILLMODE_KEY_ZOOM_IN);
				break;
				case MSG_BUTTON_ZOOM_OUT_PUSH:
					(gUI_StillMode_KeyEventTbl[E_UI_STILLMODE_KEY_ZOOM_OUT].func)(E_UI_STILLMODE_KEY_ZOOM_OUT);
				break;
				case MSG_BUTTON_HALF_SHUTTER_PUSH:
					(gUI_StillMode_KeyEventTbl[E_UI_STILLMODE_KEY_HALF_SHUTTER].func)(E_UI_STILLMODE_KEY_HALF_SHUTTER);
				break;
				case MSG_BUTTON_FULL_SHUTTER_PUSH:
					(gUI_StillMode_KeyEventTbl[E_UI_STILLMODE_KEY_FULL_SHUTTER].func)(E_UI_STILLMODE_KEY_FULL_SHUTTER);
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
static ULONG ui_proc_stillmode_execution_dummy(ULONG param)
{
	
	return TRUE;	
}

static ULONG ui_proc_stillmode_execution_cap(ULONG param)
{
	INT32 ret = 0;
	ULONG data[16];

	// Reset IPCU setting
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 0;
	ret = Cate_Set( 0x08, 0x00, 8, data );
	UPRINTF_ERR(ret);	

	// Set full size for capture
	#if USE_SENSOR == SENSOR_IMX269
	data[0] = 5280;
	data[1] = 3956;
	#elif USE_SENSOR == SENSOR_IMX477
	data[0] = 4000;
	data[1] = 3000;
	#else
	data[0] = 1920;
	data[1] = 1080;
	#endif
	ret = Cate_Set( 0x0B, 0x00, 2, data );
	UPRINTF_ERR(ret);	

	// Start capture
	data[0] = 0x03;
	ret = Cate_Set( 0x00, 0x0B, 1, data );
	UPRINTF_ERR(ret);	
	
	UI_Proc_StillMode_Exit_UsbMsg(param);
	
	return TRUE;	
}

#endif

