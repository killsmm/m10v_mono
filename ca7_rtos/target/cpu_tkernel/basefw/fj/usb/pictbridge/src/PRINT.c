/**
 * @file		PRINT.c
 * @brief		PTP PRINT MODULE
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

#include "arm.h"
#include "ddim_typedef.h"	//jeff
#include "os_user_custom.h"
#include "DPS.h"
#include "msclib.h"
#include "MSCA.h"
#include "msca_mediachange.h"
#ifdef CO_DCF_EXIF_ON
#include "dcf_if.h"
#endif
#include "mwusb_driver4.h"
#include "MessageQueue.h"
#include "fj_pictbridge.h"

extern DPSSTATUS dpsStatus;
extern SHORT SelectThumbNo;
UCHAR config_flg = 0;	//jeff
UCHAR PTP_Mode = 0;	//jeff
CHAR printLoopStart;

extern UINT32	needCheckJobAgain;//paul
static LONG PtpInit_Flg = 0;
LONG MtpEnd_Flg = 0;
#ifdef CO_USB_MTP_ON
extern UCHAR DPSDiscovery;
#endif
extern INT32 ResetMTPAllObjectInfo(VOID);	// Softronix tony +
VOID wait_Ptp_initialized(VOID)
{
	while (!PtpInit_Flg) OS_User_Dly_Tsk(200);
}

SHORT  DpsSubsystemInit(FJ_USB_MODE mode)
{
	PTP_Mode = 1;
	MsgQueue_init(eMaxMsgQueue);
	InitialPTP();
	ResetMTPAllObjectInfo();  // Softronix tony +
	ResetAllObjectInfo();	//need after ResetMTPAllObjectInfo(),because curr Dir will change

	needCheckJobAgain=0;//paul

	printLoopStart = 0;
	OS_User_Sta_Tsk(TID_USB_PTP, 0);
	OS_User_Sta_Tsk(TID_USB_MTP, 0);
	if(mode == FJ_USB_PICT_BRIDGE_MODE)	
	{
		OS_User_Sta_Tsk(TID_USB_DPSTSK, 0);
	}
	PtpInit_Flg = 1;
	return 0;
}

//#pragma arm section code="BASEFW_COMM_CODE"

SHORT  DpsSubsystemEnd()
{
	T_OS_USER_RTSK pk_rtsk;
//	MSG outmsg = {ePictBridgeSubState_Close, 0, (VOID*)0, 0};
	EndUsbBusTimer();
	OS_User_Ter_Tsk(TID_USB_DPSTSK);
	BF_Debug_Print_Information(("DPS Task End.....\r\n"));
	while(1)
	{	
 		OS_User_Ref_Tsk(TID_USB_DPSTSK, &pk_rtsk);
		if (pk_rtsk.tskstat == D_OS_USER_TTS_DMT)
			break;
	}	
	while(1)
	{
		if (MtpEnd_Flg == 0)
			break;
		else
			OS_User_Dly_Tsk(1);
	}
	
	// Terminate PTP task
	OS_User_Set_Flg(FID_PTPTSK, FLG_PTP_TERMINATE);
	while(1)
	{	
 		OS_User_Ref_Tsk(TID_USB_PTP, &pk_rtsk);
		if (pk_rtsk.tskstat == D_OS_USER_TTS_DMT)
			break;
	}

	// Terminate MTP task
	OS_User_Set_Flg(FID_MTPTSK, FLG_MTP_TERMINATE);
	while(1)
	{	
 		OS_User_Ref_Tsk(TID_USB_MTP, &pk_rtsk);
		if (pk_rtsk.tskstat == D_OS_USER_TTS_DMT)
			break;
	}

//	MWUSB_IntDisconnect();

	/* Clear remained message */
	MsgQueue_ClearAll(eDpsSubSysMQ);
	/* Clear DPS related Info */
	memset(&prtParaValue, 0x00, sizeof(prtParaValue));
	memset(&dpsStatus, 0x00, sizeof(dpsStatus));
	PtpInit_Flg = 0;
#ifdef CO_USB_MTP_ON
	DPSDiscovery = 0;//not discovery now
#endif
	/* Notify DPS UI application to close */
	//AppDPSEvent(msg);
	return 0;
}

//#pragma arm section code="BASEFW_COMM_CODE"

/****************************************************************/
/*  (Function ) DPS SubSystem main routine                      */
/*  (Arguments) INT32 : Reserved                                  */
/*  (Return   ) VOID                                            */
/*  (Description)                                               */
/*       														*/
/*                                                              */
/****************************************************************/
VOID DPSSubSystem(INT32 stacd){	
	MSG msg;
	INT32 ret;
	DpsSubsystemInit((FJ_USB_MODE)stacd); //Start DpsSubSystem firstly
	while(1)
	{
		ret = GetMessage(&msg, eDpsSubSysMQ, kMsgWaitInfinity); 
		if (ret >= 0)
		{
			AppDPSEvent(msg);
		//	if(msg.msgid == ePictBridgeSubCmd_Close){break;}
		}
		OS_User_Tslp_Tsk(1);
	}
	//DpsSubsystemEnd(); //Exit DpsSubSystem finally
	//OS_User_Ext_Tsk();
}

VOID AppDPSEvent(MSG msg)
{
	switch(msg.msgid)
	{
		case ePictBridgeSubCmd_ConfigCapability:
			break;
		case ePictBridgeSubCmd_SetPrintImage:
			break;
		case ePictBridgeSubCmd_SetPrintAction:
			switch((FJ_PICT_ACTION)(msg.ulPara1))
			{
				case FJ_PICT_ACTION_START:
					//RequestStartJob();
					break;
				case FJ_PICT_ACTION_ABORT:
					RequestAbortJob();
					break;
				case FJ_PICT_ACTION_CONTINUE:
					RequestContinueJob();
					break;
				case FJ_PICT_ACTION_START_DPOF:
					//RequestStartJobDpof();
					break;
			}
			break;
		case ePictBridgeSubCmd_ProcessDpsEvent:
			if(msg.ulPara1 == msg.msgid){ProcessDPSEvent();}
			break;
		case ePictBridgeSubCmd_Close:
			if(((FJ_USB_MODE)(msg.ulPara1)) == FJ_USB_PICT_BRIDGE_MODE)
			{
				OS_User_Ter_Tsk(TID_USB_DPSTSK);    //Modified by Andy Jing 061219
				BF_Debug_Print_Information(("DPS Task End.....\r\n"));
			}
			while(1)
			{
				if (MtpEnd_Flg == 0)
					break;
				else
					OS_User_Dly_Tsk(1);
			}
			OS_User_Ter_Tsk(TID_USB_PTP);
			BF_Debug_Print_Information(("PTP Task End.....\r\n"));
			break;
		default:
			break;
	}
}
