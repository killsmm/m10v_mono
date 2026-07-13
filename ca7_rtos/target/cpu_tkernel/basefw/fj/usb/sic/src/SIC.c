/**
 * @file		SIC.c
 * @brief		Still Image Capture Device Class module
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2017</I></B>
 */

#include "ddim_typedef.h"
#include "os_user_custom.h"

#include "PTP.h"

/**
 *	@brief		Start PTP task 
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID USBSIC_StartPTPTask(VOID) {
	
	InitialPTP();
	ResetAllObjectInfo();
	
	OS_User_Sta_Tsk(TID_USB_PTP, 0);
	//while(1)
	//{
	//	ret = GetMessage(&msg, eDpsSubSysMQ, kMsgWaitInfinity); 
	//	if (ret >= 0)
	//	{
	//		AppDPSEvent(msg);
	//	//	if(msg.msgid == ePictBridgeSubCmd_Close){break;}
	//	}
	//	OS_User_Tslp_Tsk(1);
	//}
	//DpsSubsystemEnd(); //Exit DpsSubSystem finally
	//OS_User_Ext_Tsk();
	
	return;
}

/**
 *	@brief		Stop PTP task 
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID USBSIC_StopPTPTask(VOID)
{
	T_OS_USER_RTSK pk_rtsk;
	
	// Terminate PTP task
	OS_User_Set_Flg(FID_PTPTSK, FLG_PTP_TERMINATE);
	while(1)
	{	
 		OS_User_Ref_Tsk(TID_USB_PTP, &pk_rtsk);
		if (pk_rtsk.tskstat == D_OS_USER_TTS_DMT)
			break;
	}
	
	return;
}


