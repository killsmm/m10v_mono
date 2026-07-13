/**
 * @file		ui_proc_main.c
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
#include "ui_proc_stillmode.h"
#include "category_parameter.h"

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


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static E_UI_PROC_OP_MODE 	gUI_Proc_OpModeIndex = E_UI_PROC_OP_MODE_INIT;
static E_UI_PROC_OP_STATE 	gUI_Proc_OpState = E_UI_PROC_OP_STATE_INIT;

static T_UI_PROC_FUNC gUI_Proc_MangeTbl[E_UI_PROC_OP_MODE_MAX][E_UI_PROC_OP_MANAGE_MAX] = {
	[E_UI_PROC_OP_MODE_INIT] = {
		[E_UI_PROC_OP_MANAGE_INIT]  	= {NULL},
		[E_UI_PROC_OP_MANAGE_TRIG]  	= {NULL},
		[E_UI_PROC_OP_MANAGE_STOP]  	= {NULL},
	},
	[E_UI_PROC_OP_MODE_USB] = {
		[E_UI_PROC_OP_MANAGE_INIT]		= {UI_Proc_UsbMode_Init},
		[E_UI_PROC_OP_MANAGE_TRIG]		= {UI_Proc_UsbMode_EventTrig},
		[E_UI_PROC_OP_MANAGE_STOP]		= {UI_Proc_UsbMode_Stop},
	},
	#ifdef UI_PROC_AUTO_VIEW_ON
	[E_UI_PROC_OP_MODE_STILL] = {
		[E_UI_PROC_OP_MANAGE_INIT]		= {UI_Proc_StillMode_Init},
		[E_UI_PROC_OP_MANAGE_TRIG]		= {UI_Proc_StillMode_EventTrig},
		[E_UI_PROC_OP_MANAGE_STOP]		= {UI_Proc_StillMode_Stop},
	},
	#else
	[E_UI_PROC_OP_MODE_STILL] = {
		[E_UI_PROC_OP_MANAGE_INIT]		= {NULL},
		[E_UI_PROC_OP_MANAGE_TRIG]		= {NULL},
		[E_UI_PROC_OP_MANAGE_STOP]		= {NULL},
	},
	#endif
};

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
VOID UI_Proc_Main_Init(VOID)
{
 	gUI_Proc_OpModeIndex = E_UI_PROC_OP_MODE_INIT;
 	gUI_Proc_OpState = E_UI_PROC_OP_STATE_INIT;

	BF_Debug_Set_Print_Time_Enable(1);
}

VOID UI_Proc_Main_Start(VOID)
{
	FJ_HostPostMessage(FJ_HM_NEW_MODE, E_UI_PROC_OP_MODE_USB);
}

VOID UI_Proc_Main_EventTrig(T_UI_PROC_MSG *msg)
{
	if(gUI_Proc_OpState == E_UI_PROC_OP_STATE_NG){
		return;
	}
		
	switch(msg->msgid)
	{
		case FJ_HM_NEW_MODE:
			if(gUI_Proc_OpState == E_UI_PROC_OP_STATE_RUN){
				switch(gUI_Proc_OpModeIndex)
				{
					case E_UI_PROC_OP_MODE_INIT:
						if((gUI_Proc_MangeTbl[gUI_Proc_OpModeIndex][E_UI_PROC_OP_MANAGE_STOP].func) != NULL){
							(gUI_Proc_MangeTbl[gUI_Proc_OpModeIndex][E_UI_PROC_OP_MANAGE_STOP].func)(0);
						}
					break;
					case E_UI_PROC_OP_MODE_USB:
						if((gUI_Proc_MangeTbl[gUI_Proc_OpModeIndex][E_UI_PROC_OP_MANAGE_STOP].func) != NULL){
							(gUI_Proc_MangeTbl[gUI_Proc_OpModeIndex][E_UI_PROC_OP_MANAGE_STOP].func)(UI_PROC_USBMODE_CONNECT_OFF);
						}
					break;
					case E_UI_PROC_OP_MODE_STILL:
						if((gUI_Proc_MangeTbl[gUI_Proc_OpModeIndex][E_UI_PROC_OP_MANAGE_STOP].func) != NULL){
							(gUI_Proc_MangeTbl[gUI_Proc_OpModeIndex][E_UI_PROC_OP_MANAGE_STOP].func)(0);
						}
					break;
					default:
					break;
				}
			}

			gUI_Proc_OpModeIndex = msg->ulPara1;
			switch(gUI_Proc_OpModeIndex)
			{
				case E_UI_PROC_OP_MODE_INIT:
					if((gUI_Proc_MangeTbl[gUI_Proc_OpModeIndex][E_UI_PROC_OP_MANAGE_INIT].func) != NULL){
						(gUI_Proc_MangeTbl[gUI_Proc_OpModeIndex][E_UI_PROC_OP_MANAGE_INIT].func)(0);
					}
				break;
				case E_UI_PROC_OP_MODE_USB:
					if((gUI_Proc_MangeTbl[gUI_Proc_OpModeIndex][E_UI_PROC_OP_MANAGE_INIT].func) != NULL){
						(gUI_Proc_MangeTbl[gUI_Proc_OpModeIndex][E_UI_PROC_OP_MANAGE_INIT].func)(UI_PROC_USBMODE_CONNECT_ON);
					}
				break;
				case E_UI_PROC_OP_MODE_STILL:
					if((gUI_Proc_MangeTbl[gUI_Proc_OpModeIndex][E_UI_PROC_OP_MANAGE_INIT].func) != NULL){
						(gUI_Proc_MangeTbl[gUI_Proc_OpModeIndex][E_UI_PROC_OP_MANAGE_INIT].func)(0);
					}
				break;
				default:
				break;
			}
			
			gUI_Proc_OpState = E_UI_PROC_OP_STATE_RUN;
		break;
		case FJ_HM_BUTTON:
			if((gUI_Proc_MangeTbl[gUI_Proc_OpModeIndex][E_UI_PROC_OP_MANAGE_TRIG].func) != NULL){
				(gUI_Proc_MangeTbl[gUI_Proc_OpModeIndex][E_UI_PROC_OP_MANAGE_TRIG].func)((ULONG)msg);
			}
		break;
		default:
		break;
	}
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/


#endif

