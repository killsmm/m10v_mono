/**
 * @file		pictbridge.c
 * @brief		pictbridge module
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

#include "fj_host.h"
#include "fj_pictbridge.h"
#include "PTP.h"
#include "DPS.h"
#include "bf_startup.h"

static FJ_CAPABILITY_PROPERTY CapabilityProperty;
static FJ_PRINT_IMAGE_PROPERTY PrintImageProperty;
/**
 *	@brief		Query printer capability.
 *	@param		i:uCapabilityId		= Capability ID.
 *				i:Buffer		= Data Store Area. Element 0 is Num. of elements.
 *				i:uPaperSize		= Paper Size
 *	@return		FJ_ERR_OK	OK.
 *				FJ_ERR_NG	Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	FJ_PictGetPrinterCapability( FJ_PICT_CAPABILITY_PARAM uCapabilityId, INT32 *Buffer, INT32 uPaperSize )
{
	INT32 rtc;

	rtc = (INT32)GetPrinterCapability(uCapabilityId, (ULONG*)Buffer, (ULONG)uPaperSize);
	if(rtc){
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}


/**
 *	@brief		Setting the supprot capability for printing.
 *	@param		i:uCapabilityId		= Capability ID.
 *				i:uiCapabilityValue	= Capability Value.
 *	@return		FJ_ERR_OK	OK.
 *				FJ_ERR_NG	Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	FJ_PictConfig( FJ_PICT_CAPABILITY_PARAM uCapabilityId, INT32 uiCapabilityValue )
{
	INT32 rtc;
	//MSG RequestMessage = {ePictBridgeSubCmd_ConfigCapability, 0, (VOID*)&CapabilityProperty};
	CapabilityProperty.CapabilityType = uCapabilityId;
	CapabilityProperty.ulCapabilityValue = uiCapabilityValue;
	rtc = (INT32)ConfigPrinterCapability(&CapabilityProperty);
	if(rtc){
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
	//return (INT32)SendMessage(RequestMessage, eDpsSubSysMQ, kMsgWaitInfinity);
}


/**
 *	@brief		Print action.
 *	@param		i:uCommand		= Print action value.
 *	@return		FJ_ERR_OK		OK.
 *				FJ_ERR_NG		Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	FJ_PictAction( FJ_PICT_ACTION uCommand )
{
	INT32 Err = 0;
	switch(uCommand)
	{
		case FJ_PICT_ACTION_START:
			Err = RequestStartJob();
			break;
		case FJ_PICT_ACTION_ABORT:
			Err = Asyn_RequestAbortJob();
			break;
		case FJ_PICT_ACTION_CONTINUE:
			Err = Asyn_RequestContinueJob();
			break;
		case FJ_PICT_ACTION_START_DPOF:
			Err = RequestStartJobDpof();
			break;
	}
	if(Err){
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
	//MSG RequestMessage = {ePictBridgeSubCmd_SetPrintAction, 0, (VOID*)0};
	//RequestMessage.ulPara1 = (ULONG)uCommand;
	//return (INT32)SendMessage(RequestMessage, eDpsSubSysMQ, kMsgWaitInfinity);
}



/**
 *	@brief		Add image for printing.
 *	@param		i:FileID	= Dcf index of image(not 0).
 *				i:uCropX	= Upper left Horizontal coordinate(X).
 *				i:uCropY	= Upper left Vertical coordinate(Y).
 *				i:uCropW	= Width(W).
 *				i:uCropH	= Height(H).0
 *				i:Copies	= Number of copies to be printed.
 *				i:Flag		= Option flag.
 *	@return		FJ_ERR_OK	OK.
 *				FJ_ERR_NG	Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	FJ_PictAddImage( INT32 FileID, INT32 uCropX, INT32 uCropY, INT32 uCropW, INT32 uCropH, INT32 Copies, INT32 Flag)
{
	INT32 rtc;

	//MSG RequestMessage = {ePictBridgeSubCmd_SetPrintImage, 0, (VOID*)&PrintImageProperty};
	PrintImageProperty.ulFileID = (ULONG)FileID;
	PrintImageProperty.usCropCoordinateX = (USHORT)uCropX;
	PrintImageProperty.usCropCoordinateY = (USHORT)uCropY;
	PrintImageProperty.usCropWidth = (USHORT)uCropW;
	PrintImageProperty.usCropHeight = (USHORT)uCropH;
	PrintImageProperty.usCopies = (USHORT)Copies;
	PrintImageProperty.ucPrintOption = (UCHAR)Flag;
	rtc = (INT32)AddPrintedImage(&PrintImageProperty);
	if(rtc){
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
	//return (INT32)SendMessage(RequestMessage, eDpsSubSysMQ, kMsgWaitInfinity);
}


/**
 *	@brief		Get the printer's status.
 *	@param		o:Status	= This array retun the value of printer status .
 *	@return		0			OK.
 *				not 0		Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	FJ_PICTGetDeviceStatus( FJ_DPS_DEVICE_STATUS* Status )
{
	INT32 rtc;

	rtc = (INT32)GetDpsPrinterStatus(Status);
	if(rtc){
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Synchronize PictBridge File DB with DCF File Syatem.
 *	@param		None
 *	@return		FJ_ERR_OK	OK.
 *				FJ_ERR_NG	Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	FJ_PICTUpdateFileDB()
{
	INT32 rtc;

	rtc = (INT32)ResetAllObjectInfo();
	if(rtc){
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Set PictBridge Callback Function Pointer Table.
 *	@param		i:uCbFuncID		    = FJ_DPS_CB_FUNCTION_ID.
 *				i:fnDpsCbFuncEntry	= Function Pointer Entry.
 *	@return		FJ_ERR_OK	OK.
 *				FJ_ERR_NG	Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE   FJ_PICTSetCBFunctionEntry(FJ_DPS_CB_FUNCTION_ID uCbFuncID, lpfnDpsCbFuncEntry fnDpsCbFuncEntry)
{
	INT32 rtc;

	rtc = (INT32)SetCBFunctionEntry(uCbFuncID, fnDpsCbFuncEntry);
	if(rtc){
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Store PTP Model name string.
 *	@param		i:bBuffer		= Model name string.
 *				i:dwSize		= String length.
 *	@return		FJ_ERR_OK	OK.
 *				FJ_ERR_NG	Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE FJ_SetPTPModeString(UINT16 wParmID, UCHAR* bBuffer, UINT32 dwSize )
{
	INT32  rtc = 1;
	
	switch((FJ_PRMID_PTP_MODESTRING)wParmID )
	{
	case FJ_PRMID_PTP_MODEL_STRING :	// Model name
		rtc = (INT32)SetPTPModeString(PRMID_PTP_MODEL_STRING, bBuffer, dwSize);
		break;
	case FJ_PRMID_PTP_SERIAL_NUMBER :	// Serial No
		rtc = (INT32)SetPTPModeString(PRMID_PTP_SERIAL_NUMBER, bBuffer, dwSize);
		break;
	case FJ_PRMID_PTP_VENDOR_NAME_STRING :	// Vendor name
		rtc = (INT32)SetPTPModeString(PRMID_DPS_VENDOR_NAME_STRING, bBuffer, dwSize);
		break;
	}
	if(rtc){
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Set PICT specific string.
 *	@param		i:bBuffer		= Input string.
 *				i:dwSize		= String length.
 *	@return		FJ_ERR_OK	OK.
 *				FJ_ERR_NG	Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE   FJ_PICTSetConfigString(FJ_PRMID_PICT_CONFIGSTRING wParmID, UCHAR* bBuffer, UINT32 dwSize )
{
	INT32 rtc;

	rtc = (INT32)SetPICTConfigString(wParmID, bBuffer, dwSize);
	if(rtc){
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		Initialize playback thumbnail data cache area.
 *	@param		None
 *	@return		FJ_ERR_OK	OK.
 *				FJ_ERR_NG	Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	FJ_PICTInitThumbnailCache()
{
	INT32 rtc;

	rtc = (INT32)InitThumbnailDataCache();
	if(rtc){
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

VOID FJ_PictBridgeSendHostMessage(MSG Message)
{
	ULONG ulHostMessage = 0, ulMessagePapameter = 0;
	switch(Message.msgid)
	{
		case ePictBridgeSubState_Printer_Connected:	
			ulHostMessage = FJ_PICT_HOST_MSG_PRINTER_CONFIGURED;
			break;
		case ePictBridgeSubState_Close:
			break;
		case ePictBridgeSubState_Printer_Progress:
			ulHostMessage = FJ_PICT_HOST_MSG_PRINTING_PROGRESS;
			ulMessagePapameter = Message.ulPara1;
			break;
		case ePictBridgeSubState_Printer_Job_Complete_Ok:
			ulHostMessage = FJ_PICT_HOST_MSG_JOB_COMPLETE_OK;
			break;
		case ePictBridgeSubState_Printer_Job_Continue:
			ulHostMessage = FJ_PICT_HOST_MSG_RESUMED;
			break;
		case ePictBridgeSubState_Printer_Status:
			switch((Message.ulPara1) >> 16)
			{
				case 0x70:/* Service status */
					ulHostMessage = FJ_PICT_DBG_SERVICE_STATUS;
					ulMessagePapameter = Message.ulPara1;
					break;
				case 0x71:/* Job end reason */
					ulHostMessage = FJ_PICT_DBG_JOB_END_REASON;
					ulMessagePapameter = Message.ulPara1;
					break;
				case 0x72:/* Error status */
					ulHostMessage = FJ_PICT_HOST_MSG_IDLE_NO_ERROR;
					ulMessagePapameter = Message.ulPara1;
					break;
				case 0x73:/* Error reason */
					ulHostMessage = FJ_PICT_DBG_ERROR_REASON;
					ulMessagePapameter = Message.ulPara1;
					break;
				case 0x74:/* DisconnectEnable */
					ulHostMessage = FJ_PICT_DBG_DISCONNECT_ENABLE;
					ulMessagePapameter = Message.ulPara1;
					break;
				case 0x75:/* Capability change */
					ulHostMessage = FJ_PICT_DBG_CAPABILITY_CHANGED;
					ulMessagePapameter = Message.ulPara1;
					break;
				case 0x76:/* New job ok */
					ulHostMessage = FJ_PICT_DBG_NEW_JOB_OK;
					ulMessagePapameter = Message.ulPara1;
					break;
			}
			break;
		case ePictBridgeSubState_Error_Cleared:
			ulHostMessage = FJ_PICT_HOST_MSG_ERROR_CLEARED;
			break;
		case ePictBridgeSubState_Unrecognized_Operation:
			ulHostMessage = FJ_PICT_HOST_MSG_UNRECOGNIZED_OPERATION;
			break;
		case ePictBridgeSubState_Printer_Abnormal_Response:
			ulHostMessage = FJ_PICT_HOST_MSG_PRINTER_ABNORMAL_RESPONSE;
			break;
		case ePictBridgeSubState_Printer_Fatal_Error:
			ulHostMessage = FJ_PICT_HOST_MSG_JOB_COMPLETE_FATAL_ERROR;
			ulMessagePapameter = Message.ulPara1;
			break;
		case ePictBridgeSubState_Printer_Recoverable_Error:
			ulHostMessage = FJ_PICT_HOST_MSG_JOB_COMPLETE_RECOVERABLE_ERROR;
			ulMessagePapameter = Message.ulPara1;
			break;
		case ePictBridgeSubState_Ptp_Connected:
			ulHostMessage = FJ_PICT_HOST_MSG_PTP_CONFIGURED;
			break;
		case ePictBridgeSubState_Ptp_SessionClosed:
			ulHostMessage = FJ_PICT_HOST_MSG_PTP_SESSION_CLOSED;
			break;
		default:
			return;
	}
	fj_hostpostmessage(FJ_HM_DPS_MSG, (((ULONG)ulHostMessage << 24)|ulMessagePapameter));
}

