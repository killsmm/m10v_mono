/**
 * @file		usb.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

#include <string.h>
#include "TKernel_id.h"
#include "fj_custom.h"
#include "fj_host.h"
#include "fj_usb.h"
#include "filesystem.h"
#include "fs_controller.h"
#include "media_state.h"
#include "dd_top.h"
#include "bf_startup.h"
#define USB_EN_SECTION

#include "mwusb_driver4.h"
#include "mwusb_custom4.h" // M7M
#include "msclib.h"
#include "MSCA.h"
#include "msc_wrapos.h"
#include "SIC.h"
#include "DPS.h"
#include "USB_Section.h"
#include "MessageQueue.h"
#include "fj_pictbridge.h"
#include "os_user_custom.h"

#include "media_attr.h"
#include "msca_mediachange.h"

// #ifdef FJ_CUSTOM_USB_PARTITION_ON
// #undef FJ_CUSTOM_USB_PARTITION_ON
// #endif

/*-------- USB Enable Flag --------------------*/
static volatile BOOL boUsbEnable = FALSE;
/*-------- USB Mode ---------------------------*/
#define DEFAULT_FJ_USB_MODE		FJ_USB_MASS_MODE
#define DEFAULT_USB_CLASS		USB_MODE_STORAGE
static volatile FJ_USB_MODE	gUsbMode = FJ_USB_TEMP_MODE;
/*-------- USB attach flag --------------------*/
static volatile BOOL boUsbAttachFlag = FALSE;
/*-------- Mass Storage Access  Flag ----------*/
#define MASS_ACCESS_INIT	(-1)
#define MASS_ACCESS_START	(1)
#define MASS_ACCESS_STOP	(0)

#define USB_CONNECT_WAIT_TIME			(30000)			// WaitTime 30sec.

#define USB_BUFFER_NO_REALOS

static volatile INT32 iAccess = MASS_ACCESS_INIT;

// USB callback function pointer.
static volatile USB_CALLBACK			gUsbDetectCallback	= NULL;

// USB detection status.
static volatile FJ_USB_DETECT_STATUS	gUsbDetectStatus	= FJ_USB_DETECT_STATUS_DISCONNECTED;

extern VOID wait_storage_initialized(VOID);
// M7M	extern VOID Storage_Main_Clear_Init_Flg(VOID);

extern VOID BF_Start_USB(void);

BOOL isMscEnable(VOID);

/**
 *	@brief		Start USB Main Task. 
 *	@return		FJ_USB_RET_OK		OK.
 *	@return		FJ_USB_RET_NG		Error.
 *	@note		None
 *	@attention	None
 */
static INT32 usb_main_task_start( VOID )
{

	INT32			intRet = FJ_USB_RET_OK;
    unsigned int	flg_ptn = FLG_MAIN_USB_Complete | FLG_MAIN_USB_Error;
    OS_USER_FLGPTN	rtn_ptn = 0;

	// Clear Flg
	OS_User_Clr_Flg(FID_USB_API , 0);

	// Start USB task
	BF_Start_USB();

	// Wait Flg
	OS_User_Wai_Flg(FID_USB_API, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);
	OS_User_Clr_Flg(FID_USB_MAIN , ~(flg_ptn));

	if(rtn_ptn == FLG_MAIN_USB_Error)
	{
		BF_Debug_Print_Error(("USB Connect OS_User_Wai_Flg failed: 0x%lx\n\r", rtn_ptn));
		intRet = FJ_USB_RET_NG;
	}
	
	return intRet;
}

/**
 *	@brief		Stop USB Main Task. 
 *	@return		FJ_USB_RET_OK		OK.
 *	@return		FJ_USB_RET_NG		Error.
 *	@note		None
 *	@attention	None
 */
static INT32 usb_main_task_stop( VOID )
{

	INT32			intRet = FJ_USB_RET_OK;
    unsigned int	flg_ptn = FLG_MAIN_USB_Complete | FLG_MAIN_USB_Error;
    OS_USER_FLGPTN	rtn_ptn = 0;

	// Clear Flg
	OS_User_Clr_Flg(FID_USB_API , 0);

	// Set Flag
	OS_User_Set_Flg(FID_USB_MAIN , FLG_MAIN_USB_Disconnect);

	// Wait Flg
	OS_User_Wai_Flg(FID_USB_API, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);
	OS_User_Clr_Flg(FID_USB_MAIN , ~(flg_ptn));

	if(rtn_ptn == FLG_MAIN_USB_Error)
	{
		BF_Debug_Print_Error(("USB Disconnect OS_User_Wai_Flg failed: 0x%lx\n\r", rtn_ptn));
		intRet = FJ_USB_RET_NG;
	}
	
	return intRet;
}

/**
 *	@brief		Connect USB in Mass Storage Mode. 
 *	@param		CHAR drv : drive number.
 *	@return		FJ_USB_RET_OK		OK.
 *	@return		FJ_USB_RET_NG		Error.
 *	@note		None
 *	@attention	None
 */
static INT32 usb_connect_storage_select_drv( VOID )
{
	INT32	intRet = FJ_USB_RET_OK;
	UINT32	uiIndexMedia;
	CHAR	sDrvName[D_BF_FS_IF_DRVNAME_MAX];

	/* Unmount all drives */
	for (uiIndexMedia = 0; uiIndexMedia < (FJ_MEDIA_SD1 + 1); uiIndexMedia++) {
		if (gBF_Controller_drv_name[uiIndexMedia][0] != '\0') {
			
			const char *pDriveName = (char*)gBF_Controller_drv_name[uiIndexMedia];
			strncpy(sDrvName, pDriveName, D_BF_FS_IF_DRVNAME_MAX);
			
			if (FJ_ERR_OK != FJ_UnMount(sDrvName)) {
				BF_Debug_Print_Error(("usb_connect_storage_select_drv(): Fail to unmount drive. Media ID = %d.\n", uiIndexMedia));
				return FJ_USB_RET_NG;
			}
		}
	}
	
	intRet = usb_main_task_start();

	/* wait storage task init */
	wait_storage_initialized();

	return intRet;
}


/**
 *	@brief		Disconnect USB in Mass Storage Mode. 
 *	@param		None.
 *	@return		FJ_USB_RET_OK		OK.
 *	@return		FJ_USB_RET_NG		Error.
 *	@note		None
 *	@attention	None
 */
static INT32 usb_disconnect_storage(VOID)
{
	INT32	intRet = FJ_USB_RET_OK;

	intRet = usb_main_task_stop();

	return intRet;
}

/*>>>> add 2008.10.15  */
extern LONG MSCM_GetUsbStatus(VOID);

/**
 *	@brief		Terminate MSC Task . 
 *	@param		None.
 *	@return		None.
 *	@note		None
 *	@attention	None
 */
static VOID usb_terminate_msc(VOID)
{
	INT32 retrycnt = 100;
	LONG sts;
	T_OS_USER_RTSK RefTsk;
	OS_USER_ER err = D_OS_USER_E_OK;
	T_OS_USER_RSEM RefSem;
	
	while( retrycnt > 0 )
	{
		sts = MSCM_GetUsbStatus();
		if( sts != MSC_STS_TERM )
		{	// Not Terminate
			MSCM_End();
		}
		err = OS_User_Ref_Tsk(TID_USB_STORAGE, &RefTsk);
		if( (err == D_OS_USER_E_OK)&&(RefTsk.tskstat != D_OS_USER_TTS_DMT))
		{	// Task Not Terminate
			OS_User_Tslp_Tsk(10);
			retrycnt--;
		}
		else
		{
			return;
		}
	}

	while(1){
		// check NF semaphore use
#if 1 // M8M_Porting
		OS_User_Ref_Sem(SID_NF, &RefSem);
#endif
		if (RefSem.semcnt != 0){
			if(BF_Media_Attr_Get_Attr_Ext_Media() == E_BF_MEDIA_ATTR_EXT_SD){
				// check SD0 semaphore use
				OS_User_Ref_Sem(SID_SD0, &RefSem);
				if (RefSem.semcnt != 0){
					// force Task Termiate
					OS_User_Ter_Tsk(TID_USB_STORAGE);
// M7M					Storage_Main_Clear_Init_Flg();
					BF_Debug_Print_Information(("MSC Task force terminated.\r\n"));
					break;
				}
				else{
					// wait SD driver process end
					OS_User_Dly_Tsk(10);
				}
			}
			else if(BF_Media_Attr_Get_Attr_Ext_Media() == E_BF_MEDIA_ATTR_EXT_SD1){
				// check SD1 semaphore use
				OS_User_Ref_Sem(SID_SD1, &RefSem);
				if (RefSem.semcnt != 0){
					// force Task Termiate
					OS_User_Ter_Tsk(TID_USB_STORAGE);
// M7M					Storage_Main_Clear_Init_Flg();
					BF_Debug_Print_Information(("MSC Task force terminated.\r\n"));
					break;
				}
				else{
					// wait SD driver process end
					OS_User_Dly_Tsk(10);
				}
			}
		}
		else{
			// wait NF driver process end
			OS_User_Dly_Tsk(10);
		}
	}
}
/*<<<< add 2008.10.15 */

/**
 *	@brief		Connect USB in PTP mode. 
 *	@param		None.
 *	@return		FJ_USB_RET_OK		OK.
 *	@return		FJ_USB_RET_NG		Error.
 *	@note		None
 *	@attention	None
 */
static INT32 usb_connect_ptp(VOID)
{
	INT32	intRet = FJ_USB_RET_OK;
	
	CHAR          sDrvPath[D_BF_FS_IF_DRVNAME_MAX + 1];     // 1: size of ":"
	FJ_MEDIA_ID   mediaId;
	FJ_ERR_CODE   errCode;
	
	memset(sDrvPath, 0, D_BF_FS_IF_DRVNAME_MAX + 1);
	
	// Because of the specification that only SD drive is able to be connected in wdm mode, 
	// change current drive to the SD drive.
	if (gBF_Controller_drv_name[FJ_MEDIA_SD1][0] != '\0') {        // SD1 is mounted. Priority: SD1 > SD0
		mediaId = FJ_MEDIA_SD1;
	}
	else if (gBF_Controller_drv_name[FJ_MEDIA_SD0][0] != '\0') {   // SD0 is mounted.
		mediaId = FJ_MEDIA_SD0;
	}
	else {
		BF_Debug_Print_Error(("usb_connect_ptp(): No SD media is mounted.\n"));
		return FJ_USB_RET_ERR;
	}
	
	{
		const char *pDriveName = (char*)gBF_Controller_drv_name[mediaId];
		strncat(sDrvPath, pDriveName, D_BF_FS_IF_DRVNAME_MAX - 1);            // D_BF_FS_IF_DRVNAME_MAX - 1: Max size of drive name
		strncat(sDrvPath, ":", 1);
	}
	
	errCode = FJ_Fs_Chdir(sDrvPath);
	if (errCode != FJ_ERR_OK) {
		BF_Debug_Print_Error(("usb_connect_ptp(): Fail to change drive.\n"));
		return FJ_USB_RET_ERR;
	}
	
	BF_Debug_Print_Information(("usb_connect_ptp(): Change directory to %s, mediaId = %d\n", sDrvPath, mediaId));
	BF_Debug_Print_Information(("usb_connect_ptp(): BF_Fs_If_Get_Cur_Media() = %d\n", BF_Fs_If_Get_Cur_Media()));
	
	
	// Task Start
	intRet = usb_main_task_start();
	if (intRet == FJ_USB_RET_OK)
	{
		if (gUsbMode == FJ_USB_SIC_MODE) {
			USBSIC_StartPTPTask();
		}
		if (gUsbMode == FJ_USB_WDM_MODE) {
			OS_User_Sta_Tsk(TID_USB_DPS_SUBSYS,FJ_USB_WDM_MODE);
		}
	}
	
	return intRet;
}

/**
 *	@brief		Disconnect USB in PTP mode. 
 *	@param		None.
 *	@return		FJ_USB_RET_OK		OK.
 *	@return		FJ_USB_RET_NG		Error.
 *	@note		None
 *	@attention	None
 */
static INT32 usb_disconnect_ptp(VOID)
{
	INT32	intRet = FJ_USB_RET_OK;

	// Task Stop
	intRet = usb_main_task_stop();
	if (intRet == FJ_USB_RET_OK)
	{
		if (gUsbMode == FJ_USB_SIC_MODE) {
			USBSIC_StopPTPTask();
		}
		if (gUsbMode == FJ_USB_WDM_MODE) {
			OS_User_Sus_Tsk(TID_USB_DPS_SUBSYS);
			DpsSubsystemEnd();
			OS_User_Ter_Tsk(TID_USB_DPS_SUBSYS);
		}
	}

	return intRet;
}

/**
 *	@brief		Convert USB mode to USB Class. 
 *	@param		i:uMode		= USB Mode.
 *	@return		None.
 *	@note		None
 *	@attention	None
 */
static BYTE usb_convert_to_class( FJ_USB_MODE uMode )
{
	static const BYTE convertMap[] = {
		
		USB_MODE_WDM,         // FJ_USB_WDM_MODE	
		USB_MODE_PICTBRIDGE,  // FJ_USB_PICT_BRIDGE_MODE,
		USB_MODE_STORAGE,     // FJ_USB_MASS_MODE,		
		USB_MODE_SIC,         // FJ_USB_SIC_MODE,		
		USB_MODE_NONE,        // else
	};
	
	return convertMap[ uMode < FJ_USB_TEMP_MODE ? uMode : FJ_USB_TEMP_MODE ];
}

//#pragma arm section code="BASEFW_COMM_CODE"

/**
 *	@brief	Regist callback function pointer for hotplug detection.
 *	@param	detect_callback			Callback function pointer for USB detect.
 *	@return	FJ_ERR_OK				Normal End.
 *	@return	FJ_ERR_NG				Abnormal End.
 *	@return	FJ_ERR_NG_INPUT_PARAM	Abnormal End. Input parameter error.
*/
FJ_ERR_CODE FJ_SetUsbDetectCallback( USB_CALLBACK detect_callback )
{
	T_OS_USER_RTSK flag;

	if ( detect_callback == NULL ) {
		if ( gUsbDetectCallback == NULL ) {
			BF_Debug_Print_Error(( "FJ_SetUsbDetectCallback() Error!!:detect_callback is NULL.\n" ));
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			gUsbDetectStatus	= FJ_USB_DETECT_STATUS_DISCONNECTED;
			gUsbDetectCallback	= NULL;

			OS_User_Ter_Tsk( TID_USB_HOTPLUG );

			BF_Debug_Print_Information(("USB Hotplug Task End.....\r\n"));
			return FJ_ERR_OK;
		}
	}

	gUsbDetectCallback = detect_callback;

	// Start USB hotplug detection Task.
	OS_User_Ref_Tsk( TID_USB_HOTPLUG, &flag );
	if ( (flag.tskstat & D_OS_USER_TTS_DMT ) == D_OS_USER_TTS_DMT ) {
		OS_User_Sta_Tsk( TID_USB_HOTPLUG, 0 );
	}

	return FJ_ERR_OK;
}

VOID Usb_Hotplug( INT32 stacd )
{
	FJ_USB_DETECT_STATUS detectStatus_current;

	BF_Debug_Print_Information(("USB Hotplug Task Start.....\r\n"));

	// Check USB connection.
	if ( MWUSBC_CheckConnect( 0 ) == MWUSB_PLUG_RELEASE ) {
		gUsbDetectStatus = FJ_USB_DETECT_STATUS_DISCONNECTED;
	}
	else {
		gUsbDetectStatus = FJ_USB_DETECT_STATUS_CONNECTED;

		// detection callback start.
		if ( gUsbDetectCallback ) {
			(*gUsbDetectCallback)( FJ_USB_DETECT_STATUS_CONNECTED );
		}
	}

	while ( 1 ) {

		OS_User_Tslp_Tsk( FJ_USB_DETECT_CHECK_INTERVAL );

		// Check USB connection.
		if ( MWUSBC_CheckConnect( 0 ) == MWUSB_PLUG_RELEASE ) {
			detectStatus_current = FJ_USB_DETECT_STATUS_DISCONNECTED;
		}
		else {
			detectStatus_current = FJ_USB_DETECT_STATUS_CONNECTED;
		}

		if ( gUsbDetectStatus != detectStatus_current ) {
			gUsbDetectStatus = detectStatus_current;

			// detection callback start.
			if ( gUsbDetectCallback ) {
				(*gUsbDetectCallback)( detectStatus_current );
			}
		}
	}
}

/**
 *	@brief		Enable USB and Start Class Driver. 
 *	@return		FJ_ERR_OK		OK.
 *	@return		FJ_ERR_NG		Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	FJ_UsbConnect( VOID )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;
	INT32 ercd, intRet = FJ_USB_RET_OK;
	
	BF_Debug_Print_Information(("FJ_UsbConnect called.\n"));
	
	/* Check Class */
	if( gUsbMode == FJ_USB_TEMP_MODE )
	{
		BF_Debug_Print_Error(("USB Mode has not been set. \n"));
		return FJ_ERR_NG;
	}
	
	ercd = OS_User_Twai_Sem(SID_USB_W,USB_CONNECT_WAIT_TIME);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}
	
	/* Check already connected? */
	if( boUsbEnable == TRUE )
	{
		OS_User_Sig_Sem(SID_USB_W);
		return FJ_ERR_OK;
	}
	
	// Check USB cable connect
	if(MWUSBC_CheckConnect(0) == MWUSB_PLUG_RELEASE){	// without Chattering(Host side with Chattering)
		OS_User_Sig_Sem(SID_USB_W);
		return FJ_ERR_USB_DISCONNECT_ERROR;	// NG
	}
	
	switch( gUsbMode )
	{
	case FJ_USB_MASS_MODE :
		intRet = usb_connect_storage_select_drv();
		break;
	case FJ_USB_SIC_MODE :
	case FJ_USB_WDM_MODE :
		intRet = usb_connect_ptp();
		break;
	default :	// Unknown mode
		intRet = FJ_USB_RET_NG;
	}
	
	if (intRet == FJ_USB_RET_OK)
	{
		boUsbEnable = TRUE;
		ret = FJ_ERR_OK;
	}
	
	OS_User_Sig_Sem(SID_USB_W);
	
	return ret;
}

/**
 *	@brief		Disable USB and Stop Class Driver. 
 *	@return		FJ_ERR_OK		OK.
 *	@return		FJ_ERR_NG		Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	FJ_UsbDisConnect( VOID )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;
	INT32 ercd, intRet = FJ_USB_RET_OK;
	
	BF_Debug_Print_Information(("FJ_UsbDisConnect called.\n"));
	
	/* Check Class */
	if( gUsbMode == FJ_USB_TEMP_MODE )
	{
		BF_Debug_Print_Error(("USB Mode has not been set. \n"));
		return FJ_ERR_NG;
	}
	
	ercd = OS_User_Twai_Sem(SID_USB_W,USB_CONNECT_WAIT_TIME);
	if (ercd != 0) {
		return FJ_ERR_NG;
	}
	
	/* Check already disconnected? */
	if( boUsbEnable == FALSE )
	{
		OS_User_Sig_Sem(SID_USB_W);
		return FJ_ERR_OK;
	}
	
	switch( gUsbMode )
	{
	case FJ_USB_MASS_MODE :
		intRet = usb_disconnect_storage();
		break;
	case FJ_USB_SIC_MODE :
	case FJ_USB_WDM_MODE :
		intRet = usb_disconnect_ptp();
		break;
	default :
		intRet = FJ_USB_RET_NG;
	}
	
	if (intRet == FJ_USB_RET_OK)
	{
		// Disable USB
		boUsbEnable = FALSE;
		ret = FJ_ERR_OK;
	}
	
	/* Add 2008.10.15  Check & Wait MSC Task Terminate */
	if( gUsbMode == FJ_USB_MASS_MODE )
	{
		usb_terminate_msc();
		ercd = BF_Fs_If_Init_Drv(D_BF_FS_IF_NOT_MOUNT_I);
		if (ercd != 0) {
			BF_Debug_Print_Error(("BF_Fs_If_Init_Drv() failed %d\n", ercd));
		}
	}
	
	OS_User_Sig_Sem(SID_USB_W);
	
	return ret;
}

/**
 *	@brief		Set USB mode.
 *	@param		i:uMode		= USB Mode.
 *	@return		FJ_ERR_OK	OK.
 *	@return		FJ_ERR_NG	Error.
 *	@note		None
 *	@attention	This function must be called before function FJ_UsbConnect.
 */
FJ_ERR_CODE	FJ_SetUsbMode( FJ_USB_MODE uMode )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;
	BYTE byteClass;

	if( boUsbEnable == TRUE )
	{
		BF_Debug_Print_Error(("Invalid USB state, it has been connected.\n"));
		ret = FJ_ERR_NG;
	}
	else if( gUsbMode == uMode )
	{
		BF_Debug_Print_Information(("The same as current USB mode. [%d]\n", uMode));
		ret = FJ_ERR_OK;
	}
	else
	{	// Mode Change
		byteClass = usb_convert_to_class(uMode);
		if( byteClass == USB_MODE_NONE )
		{
			BF_Debug_Print_Error(("Invalid USB mode. [%d]\n", uMode));
			ret = FJ_ERR_NG;
		}
		else
		{
			/* Set USB Class Mode */
			MWUSBC_SetUsbClass(byteClass);
			gUsbMode = uMode;
			
			ret = FJ_ERR_OK;
		}
	}
	
	return ret;
}

//#pragma arm section code="BASEFW_COMM_CODE"

/**
 *	@brief		Check USB Active.
 *	@param		None.
 *	@return		TRUE	USB Enable.
 *	@return		FALSE	USB Disable.
 *	@note		None
 *	@attention	None
 */
BOOL FJ_CheckUSBEnable(VOID)
{
	/* Return Flag */
	return boUsbEnable;
}

/**
 *	@brief		Send FJ_HM_MASS_STORAGE Message..
 *	@param		i:ulAccess.
 *	@return		None.
 *	@note		None
 *	@attention	None
 */
VOID FJ_SendMessageMassStorage( volatile INT32 iAccessNow )
{
	if( iAccess != iAccessNow )
	{	// Change Status
		/* Send Message */
		//fj_hostpostmessage(FJ_HM_MASS_STORAGE, iAccessNow);
		iAccess = iAccessNow;
	}
}

/**
 *	@brief		Cofing Usb Mass Stroage Setting.
 *	@param		i:uConfig	= 1:Write protection (Only this value can be used).
 *				i:uValue	= 0:Not Protected	1:Protected
 *	@return		FJ_ERR_OK	OK.
 *				FJ_ERR_NG	Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	FJ_UMSConfig( INT32 uConfig, INT32 uValue )
{
	
	// Parameter Check
	if (uConfig != FJ_PRMID_MS_MEDIA_WRITE_PROTECT) {
		BF_Debug_Print_Error(("FJ_UMSConfig uConfig is out of range. [%d]\n", uConfig));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if (uValue != FJ_MSC_MEDIA_WPROTECT_NO && uValue != FJ_MSC_MEDIA_WPROTECT_YES) {
		BF_Debug_Print_Error(("FJ_UMSConfig uValue is out of range. [%d]\n", uValue));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	if( (isMscEnable() == TRUE)&&(boUsbAttachFlag == TRUE) )
	{	// MSC Attached
		return FJ_ERR_NG;
	}
	
	if( uValue == FJ_MSC_MEDIA_WPROTECT_YES )
	{	BF_Debug_Print_Information(("FJ_UMSConfig Write Protect ON.\n"));
		MSCA_SetWProtect(0xFF, MSC_WPROTECT_ON);
	}
	else
	{	BF_Debug_Print_Information(("FJ_UMSConfig Write Protect OFF.\n"));
		MSCA_SetWProtect(0xFF, MSC_WPROTECT_OFF);
	}
	
	return FJ_ERR_OK;
}

/**
 *	@brief		Set UsbDescriptor In Place Of Defaults.
 *	@param		i:uCusDescID	= FJ_PRMID_USB_DESC_CUSTOM.
 *				i:uInBuf		= Setting Content.
 *				i:uBufSize		= The Size Of u InBuf.
 *	@return		FJ_ERR_OK	    OK.
 *	@return		FJ_ERR_NG	    Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE   FJ_CustomSetUsbDescriptor(FJ_PRMID_USB_DESC_CUSTOM uCusDescID, UCHAR* uInBuf, USHORT uBufSize)
{
	if(uCusDescID >= FJ_PRMID_USB_DESC_MSIZE) return FJ_ERR_NG;
	return(FJ_ERR_OK);	// M7M
}

/**
 *	@brief		Get current usb cable status.
 *	@param		None
 *	@return		TRUE			Attached.
 *	@return		FALSE		    Detached.
 *	@note		None
 *	@attention	None
 */
BOOL    FJ_GetUsbCableStatus()
{
	long  lCableStatus = MWUSB_PLUG_RELEASE;

	lCableStatus = MWUSB_CheckConnect();
	if (lCableStatus == MWUSB_PLUG_CONNECT)
	{
		// Connect
		return TRUE;
	}
	
	// Disconnect
	return FALSE;
}

/*----------------- for Debug Only Function ----------------*/
BOOL isMscEnable(VOID)
{
	if( (boUsbEnable == TRUE)&&(MWUSBC_GetUsbClass() == USB_MODE_STORAGE) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// ------------------------------------------------
// for Multi Drive mode ---------------------------
// ------------------------------------------------
FJ_ERR_CODE FJ_SetMscDrv(FJ_MEDIA_ID mediaIds[], UINT32 uiMediaIdNum) {
	
	// Parameter Check
	// Because the Media IDs are always within the range, parameter check is unnecessary.
	// Media ID number
	if (uiMediaIdNum == 0 || uiMediaIdNum > MSCS_MAXLUN) {
		BF_Debug_Print_Error(("FJ_SetMscDrv  Media ID Number is out of range. [%d]\n", uiMediaIdNum));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	// Set MSC Drive information.
	MSC_SetMscDrive(mediaIds, uiMediaIdNum);
	
	// Update API Table
	MSC_C_ChangeAPI();
	
	return FJ_ERR_OK;
}

INT32 FJ_SetUsbDriveWprotect(UINT32 uiMediaId, INT32 uiWriteProtect) {
	
	BF_Debug_Print_Information((
		"FJ_SetUsbDriveWprotect uiMediaId = %d, uiWriteProtect = %d\n", uiMediaId, uiWriteProtect));
	
	// Parameter Check
	if (uiMediaId > MSCS_MAXLUN) {
		BF_Debug_Print_Error((
			"FJ_SetUsbDriveWprotect Too large Media ID.\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	if (uiWriteProtect != FJ_MSC_MEDIA_WPROTECT_YES && 
		uiWriteProtect != FJ_MSC_MEDIA_WPROTECT_NO) {
		
		BF_Debug_Print_Error((
			"FJ_SetUsbDriveWprotect Not supported write protection status.\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	MSCA_SetWProtect(uiMediaId, uiWriteProtect);
	
	return FJ_USB_RET_OK;
}

INT32 FJ_UsbChangeExtMedia(UCHAR status, FJ_MEDIA_ID mediaId) {
	
	// Parameter Check
	if (mediaId != FJ_MEDIA_SD0 && mediaId != FJ_MEDIA_SD1) {
		
		BF_Debug_Print_Error(("FJ_UsbChangeExtMedia() failed [mediaId is out of range]\n"));
		return FJ_USB_RET_ERR;
	}
	
	if (status == 1) {
		if (isMscEnable() == FALSE) {
			
			BF_Debug_Print_Error(("FJ_UsbChangeExtMedia() failed [MSC is not enable]\n"));
			return FJ_USB_RET_ERR;
		}
		
		if (MSC_CheckExtMediaState() == FALSE) {
			
			BF_Debug_Print_Error(("FJ_UsbChangeExtMedia() failed [External media state is illegal]\n"));
			return FJ_USB_RET_ERR;
		}
		
		MSC_C_ChangeAPI_ExtDrive(mediaId);
	}
	else {
		BF_Debug_Print_Information(("FJ_UsbChangeExtMedia() External media has been removed. \n"));
	}
	
	return FJ_USB_RET_OK;
}
