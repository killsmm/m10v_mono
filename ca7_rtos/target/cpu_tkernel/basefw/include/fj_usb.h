/**
 * @file		fj_usb.h
 * @brief		USB Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
#ifndef _FJ_USB_H
#define _FJ_USB_H

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_filesystem.h"
///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_usb_overview
@{
	The USB controls can be achieved by using this API.
@}*//* --- end of fj_usb_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_usb_sequence
@{
	- @ref USB_Hotplug_Detection
	- @ref Connect_Mass_Storage_Mode_Figure
	- @ref Disconnect_Mass_Storage_Mode_Figure
	- @ref Connect_PTP_MTP_Mode_Figure
	- @ref Disconnect_PTP_MTP_Mode_Figure
	<hr>
	@section USB_Hotplug_Detection USB Hotplug Detection
	@image html fj_usb_hotplug_detection.png "Fig. USB Hotplug Detection"

	@section Connect_Mass_Storage_Mode_Figure Connect Mass Storage Mode
	@image html fj_usb_connect_mass_storage_mode.png "Fig. Connect Mass Storage Mode"

	@code
	[Sample Code]
	// media settings
	Case to connect SD card
		FJ_MEDIA_ID mediaIds[] = {FJ_MEDIA_SD0};
		UINT32 uiMediaIdNum = 1;
	
	Case to connect 3 NF area
		FJ_MEDIA_ID mediaIds[] = {FJ_MEDIA_NF_STORAGE0, FJ_MEDIA_NF_STORAGE2, FJ_MEDIA_NF_STORAGE3};
		UINT32 uiMediaIdNum = 3;
	
	Case to connect Multi media
		FJ_MEDIA_ID mediaIds[] = {FJ_MEDIA_SD1, FJ_MEDIA_NF_STORAGE1, FJ_MEDIA_EMMC_STORAGE1};
		UINT32 uiMediaIdNum = 3;
	
	// function call
		FJ_SetUsbMode(FJ_USB_MASS_MODE);
		FJ_SetMscDrv(mediaIds, uiMediaIdNum);
		FJ_UsbConnectMassStorage(TRUE);
	@endcode

	@section Disconnect_Mass_Storage_Mode_Figure Disconnect Mass Storage Mode
	@image html fj_usb_disconnect_mass_storage_mode.png "Fig. Disconnect Mass Storage Mode"

	@section Connect_PTP_MTP_Mode_Figure Connect PTP/MTP Mode
	@image html fj_usb_connect_ptp_mtp_mode.png "Fig. Connect PTP/MTP Mode"
	@code
	[Sample Code]
		// USB PTP/MTP mode
		FJ_SetUsbMode(FJ_USB_WDM_MODE);

		// USB connect ON
		FJ_UsbConnect(TRUE);
	@endcode

	@section Disconnect_PTP_MTP_Mode_Figure Disconnect PTP/MTP Mode
	@image html fj_usb_disconnect_ptp_mtp_mode.png "Fig. Disconnect PTP/MTP Mode"

@}*//* --- end of fj_usb_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_usb_definition
@{*/	// fj_usb_definition group

/**
USB mode enum
*/
typedef enum{
	FJ_USB_WDM_MODE = 0,			/**< WDM Mode(PTP/MTP Mode) */
	FJ_USB_PICT_BRIDGE_MODE,		/**< PictBridge Mode */
	FJ_USB_MASS_MODE,				/**< MassStorage Mode */
	FJ_USB_SIC_MODE,				/**< SIC Mode(Still Image Capture device class Mode) */
//	FJ_USB_FUNC_MODE,    			/**< Transfer Commands on Windows 2000 */
//	FJ_USB_HID_MODE,				/**< HID Mode */
//	FJ_USB_CHARGER_MODE,			/**< Battery Charger Mode */
	FJ_USB_TEMP_MODE				/**< Temp Mode */
} FJ_USB_MODE;

/**
prmid ms configure enum
*/
typedef enum {
	FJ_PRMID_MS_MEDIA = 0,				/**< Selct current media for mass storage.	*/
	FJ_PRMID_MS_MEDIA_WRITE_PROTECT,	/**< Media write protection for mass storage. */
//	FJ_PRMID_MS_MULTI					/**< Multi drive mode */
} FJ_PRMID_MS_CONFIG;

/**
prmid USB customize description string enum
*/
typedef enum {
	FJ_PRMID_USB_PRODUCT_ID		= 0,	/**< Product Id */
	FJ_PRMID_USB_VENDOR_ID		= 1,	/**< Vendor Id */
	FJ_PRMID_USB_MANU_STRING	= 2,	/**< Manufacturer String */
	FJ_PRMID_USB_PROC_STRING	= 3,	/**< Product String */
	FJ_PRMID_USB_SERI_STRING	= 4,	/**< Serial Num String */
	FJ_PRMID_USB_BCDDEV_VALUE	= 5,	/**< BcdDevice Value */
	FJ_PRMID_USB_MSVEN_STRING	= 6,	/**< MS Vendor String */
	FJ_PRMID_USB_MSPRO_STRING	= 7,	/**< MS Product String */
	FJ_PRMID_USB_DESC_MSIZE		= 8		/**< desc Msize */ 
} FJ_PRMID_USB_DESC_CUSTOM;

/**
mass storage drive
*/
// --- REMOVE_CAMERAIF BEGIN ---
typedef enum {
	FJ_MASS_STORAGE_J	= 0,		/**<  J: Drive */
	FJ_MASS_STORAGE_A	= 1,		/**<  A: Drive */
	FJ_MASS_STORAGE_B	= 2,		/**<  B: Drive */
	FJ_MASS_STORAGE_C	= 3,		/**<  C: Drive */
	FJ_MASS_STORAGE_D	= 4,		/**<  D: Drive */
	FJ_MASS_STORAGE_E	= 5,		/**<  E: Drive */
	FJ_MASS_STORAGE_F	= 6,		/**<  F: Drive */
	FJ_MASS_STORAGE_I	= 7,		/**<  I: Drive */
	FJ_MASS_STORAGE_NC	= 0xFF,		/**<  None connect */
} FJ_MASS_STORAGE_DRIVE;
// --- REMOVE_CAMERAIF END ---

/**
USB port kind enum
*/
typedef enum{
	FJ_USB_PORT_SDP = 0,		/**< :SDP */
	FJ_USB_PORT_CDP,			/**< :CDP */
	FJ_USB_PORT_DCP,			/**< :DCP */
} FJ_USB_PORT_KIND;

/** USB Hotplug Detect Status */
typedef enum {
	FJ_USB_DETECT_STATUS_CONNECTED		= 1,	/**< Detected hotplug connect		*/
	FJ_USB_DETECT_STATUS_DISCONNECTED	= 2		/**< Detected hotplug disconnect	*/
} FJ_USB_DETECT_STATUS;

/** Type is defined to Callback function pointer for USB. */
typedef void (*USB_CALLBACK)( FJ_USB_DETECT_STATUS detect_status );

#define FJ_USB_DETECT_CHECK_INTERVAL	(500)	/**< USB detection check interval(500ms) */

#define FJ_MSC_MEDIA_INTERNAL			(0)		/**< Current media for MSC : internal */
#define FJ_MSC_MEDIA_EXTERNAL			(1)		/**< Current media for MSC : external */

#define FJ_MSC_MEDIA_WPROTECT_NO		(0)		/**< Write protection for MSC : NO */
#define FJ_MSC_MEDIA_WPROTECT_YES		(1)		/**< Write protection for MSC : YES */

#define FJ_USB_MEDIA_CONNECT_OFF		(0)		/**< Media connect ON (for FJ_SetUsbMultiLun() API) */
#define FJ_USB_MEDIA_CONNECT_ON			(1)		/**< Media connect OFF (for FJ_SetUsbMultiLun() API) */

#define FJ_USB_RET_OK					(0)		/**< API Return Value : OK */
#define FJ_USB_RET_ERR					(1)		/**< API Return Value : ERR */
#define FJ_USB_RET_NG					(-1)	/**< API Return Value : NG */
/* @} */	// fj_usb_definition group

///////////////////////////////////////////////////////////////////////////////
// api
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_usb_api
@{*/	// fj_usb_api group

/**	<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
 *	Regist callback function pointer for hotplug detection.<br>
 *	Please refer to the @ref FJ_USB_DETECT_STATUS detection state to be notified in the callback.
 *
 *	@param[in]	detect_callback		Callback function pointer for USB detection.<br>
									If "detect_callback" is NULL and USB detection does not start,
									this function returns "FJ_ERR_NG_INPUT_PARAM".<br>
									If "detect_callback" is NULL and USB detection start,
									this function returns "FJ_ERR_OK" and stops USB detection.<br>
 *	@retval FJ_ERR_OK				Normal End.
 *	@retval FJ_ERR_NG				Abnormal End.
 *	@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Input parameter error.
 *	@remarks Specified callback function is called from task-independent portions.<br>
*/
extern FJ_ERR_CODE FJ_SetUsbDetectCallback( USB_CALLBACK detect_callback );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set USB mode.
@param[in]	uMode		= USB Mode.
			- @ref FJ_USB_WDM_MODE   : WDM Mode(PTP/MTP Mode) 
			- @ref FJ_USB_MASS_MODE  : MassStorage Mode 
@retval		FJ_ERR_OK	OK.
@retval		FJ_ERR_NG	Error.
@remarks 	This function must be called before function FJ_UsbConnect.
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
 */
FJ_ERR_CODE	FJ_SetUsbMode( FJ_USB_MODE uMode );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set drives that is connected in MSC mode.

@param[in]	mediaIds[]		Array of Media IDs of the media to connect in MSC mode.<br>
@param[in]	uiMediaIdNum	Number of Media IDs in mediaIds.<br>
@retval		FJ_USB_RET_OK	    OK.
@retval		FJ_USB_RET_ERR	    Error.
@retval		FJ_ERR_NG_INPUT_PARAM	Error in input parameter. 
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
FJ_ERR_CODE FJ_SetMscDrv(FJ_MEDIA_ID mediaIds[], UINT32 uiMediaIdNum);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Enable USB and Start Class Driver. 
@retval		FJ_ERR_OK	OK.
@retval		FJ_ERR_NG	Error.
@retval		FJ_ERR_USB_DISCONNECT_ERROR	USB cable is not connected.<br>
@remarks 	When using this API to connect the target device to PC in USB Mass Storage mode, <br>
			all mounted drives will be unmounted. Therefore, file access by using the file <br>
			system will be prohibited. Even after USB connection in Mass Storage mode is <br>
			disconnected, the drives will not be mounted automatically. Host firmware must <br>
			call FJ_Mount function before using file system again. <br>
			After connecting the target device to PC in MTP mode with this API, only <br>
			SD card drive will be recognized on PC file system. Therefore, the SD card <br>
			drive must be mounted on file system of the target device before connecting. 
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)			
*/
FJ_ERR_CODE	FJ_UsbConnect( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Disable USB and Stop Class Driver. 
@retval		FJ_ERR_OK	OK.
@retval		FJ_ERR_NG	Error.
@retval		FJ_ERR_USB_DISCONNECT_ERROR	USB cable is not connected.<br>
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)			
*/
FJ_ERR_CODE	FJ_UsbDisConnect( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set Write-Protection to USB Mass-Storage drive.
@param[in]	uiMediaId Media ID to set Write-Protection.
				- @ref FJ_MEDIA_NF_STORAGE0 :  NandFlash Storage 0<br>
				- @ref FJ_MEDIA_NF_STORAGE1 :  NandFlash Storage 1<br>
				- @ref FJ_MEDIA_NF_STORAGE2 :  NandFlash Storage 2<br>
				- @ref FJ_MEDIA_NF_STORAGE3 :  NandFlash Storage 3<br>
				- @ref FJ_MEDIA_NF_STORAGE4 :  NandFlash Storage 4<br>
				- @ref FJ_MEDIA_NF_STORAGE5 :  NandFlash Storage 5<br>
				- @ref FJ_MEDIA_NF_STORAGE6 :  NandFlash Storage 6<br>
				- @ref FJ_MEDIA_SD0 : SD card 0<br>
				- @ref FJ_MEDIA_SD1 : SD card 1<br>
				- @ref FJ_MEDIA_EMMC_STORAGE0 :  eMMC Storage 0<br>
				- @ref FJ_MEDIA_EMMC_STORAGE1 :  eMMC Storage 1<br>
				- @ref FJ_MEDIA_EMMC_STORAGE2 :  eMMC Storage 2<br>
				- @ref FJ_MEDIA_EMMC_STORAGE3 :  eMMC Storage 3<br>
				- @ref FJ_MEDIA_EMMC_STORAGE4 :  eMMC Storage 4<br>
				- @ref FJ_MEDIA_EMMC_STORAGE5 :  eMMC Storage 5<br>
				- @ref FJ_MEDIA_EMMC_STORAGE6 :  eMMC Storage 6<br>
@param[in]	uiWriteProtect Write-Protection Status.
				- @ref FJ_MSC_MEDIA_WPROTECT_NO : Not Write-Protect
				- @ref FJ_MSC_MEDIA_WPROTECT_YES : Write-Protect
@retval		FJ_USB_RET_OK(0) OK.
@retval		FJ_USB_RET_NG(-1) Error.
@remarks
	When host firmware want to set Write-Protection to Mass-Storage drive, <br>
	please call this API before connect Mass-Storage mode. <br>
	When it want to set the same Write-Protection status to all Mass-Storage <br>
	drive, please use FJ_UMSConfig() API.<br>
	<br>
	Sample code for Set Write-Protection to USB Mass-Storage drive.
	@code
		// Write-Protect ON
		FJ_SetUsbDriveWprotect(FJ_MEDIA_NF_STORAGE0, FJ_MSC_MEDIA_WPROTECT_YES);
		// Write-Protect OFF
		FJ_SetUsbDriveWprotect(FJ_MEDIA_NF_STORAGE0, FJ_MSC_MEDIA_WPROTECT_NO);

	@endcode
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)	
 */
INT32 FJ_SetUsbDriveWprotect(UINT32 uiMediaId, INT32 uiWriteProtect);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Cofing Usb Mass Stroage Setting.
@param[in]	uConfig	= 1:Write protection.
@param[in]	uValue	= 0:Not Protected	1:Protected
@retval		FJ_ERR_OK	OK.
@retval		FJ_ERR_NG	Error.
@remarks
	Sample code for Set Write-Protection to all Mass-Storage drives.
	@code
		// Write-Protection ON
		FJ_UMSConfig(FJ_PRMID_MS_MEDIA_WRITE_PROTECT, FJ_MSC_MEDIA_WPROTECT_YES);
		// Write-Protection OFF
		FJ_UMSConfig(FJ_PRMID_MS_MEDIA_WRITE_PROTECT, FJ_MSC_MEDIA_WPROTECT_NO);
		
	@endcode
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
 */
FJ_ERR_CODE	FJ_UMSConfig( INT32 uConfig, INT32 uValue );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API must be called by Host FW when the status of the external media <br>
is changed during USB storage mode. <br>

@param[in]	status Insert or Remove status of the external media.<br>
				0 : Remove external media.<br>
				1 : Insert external media.<br>
@param[in]	mediaId  Media ID of the external media to insert or remove.<br>
				- @ref FJ_MEDIA_SD0 : SD card 0<br>
				- @ref FJ_MEDIA_SD1 : SD card 1<br>
@retval		FJ_USB_RET_OK	    OK.
@retval		FJ_USB_RET_ERR	    Error.
@remarks
	This API can use only USB mass-storage mode connecting.<br>
	If use in the following state, this API will return Error.<br>
	- Not connected USB mass-storage mode.<br>
	- Not inserted the card, and set '1' to status.<br>
	- Not selected the external media to USB storage drive.<br>
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
INT32 FJ_UsbChangeExtMedia(UCHAR status, FJ_MEDIA_ID mediaId);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set UsbDescriptor In Place Of Defaults.
@param[in]	uCusDescID	= FJ_PRMID_USB_DESC_CUSTOM.
@param[in]	uInBuf		= Setting Content.
@param[in]	uBufSize		= The Size Of u InBuf.
@retval		FJ_ERR_OK	    OK.
@retval		FJ_ERR_NG	    Error.
@remarks 	None
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
FJ_ERR_CODE   FJ_CustomSetUsbDescriptor(FJ_PRMID_USB_DESC_CUSTOM uCusDescID, UCHAR* uInBuf, USHORT uBufSize);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get current usb cable status.
@retval		TRUE			Attached.
@retval		FALSE		    Detached.
@remarks 	None
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
 */
BOOL    FJ_GetUsbCableStatus( VOID );

/**
This function sets debug value address for WDM
@param[in]	&DEBUG_ADDRESS : Get form debug tool
@retval		FJ_ERR_OK	OK.
@retval		FJ_ERR_NG	NG.
@remarks 	None
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
 */
FJ_ERR_CODE	FJ_SetWDMDebugValue( INT32* DEBUG_ADDRESS );

/* @} */	// fj_usb_api group
///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_usb_message
@{*/

/**
@def FJ_HM_MASS_STORAGE
USB Mass-Storage message.
@param 1 start access.
@param 0 stop access.
*/
/**
@def FJ_HM_TITLE_USB_MODE_CHANGE
USB Mode change message.
@param 0x00 USB connect is end.
@param 0x10 USB bus is TimeOut.
*/
/**
@def FJ_HM_TITLE_USB_BUS_TRANSFER
Indicate USB Busy Signal.
*/
/*@}*//* --- end of fj_usb_message */
#endif
