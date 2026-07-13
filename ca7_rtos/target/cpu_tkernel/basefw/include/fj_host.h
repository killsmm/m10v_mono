/**
 * @file		fj_host.h
 * @brief		HOST Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_HOST_H
#define _FJ_HOST_H

#include "fj_custom.h"
#include "fj_api.h"

/** @weakgroup fj_message_definition
@{*/
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define CO_HOST_PROC

#ifdef CO_HOST_PROC
#define CO_UI_PROC
#endif
///////////////////////////////////////////////////////////////////////////////
// Definition of titles for message code
///////////////////////////////////////////////////////////////////////////////
#define	FJ_TITLE_HOSTMESSAGE_ON				/**< Host Message ON */

#define	FJ_TITLE_DEVICE		(0x01000000)	/**< Host message for device	*/
#define FJ_TITLE_BOOT		(0x02000000)	/**< Host message for boot		*/
#define FJ_TITLE_MODE		(0x05000000)	/**< Host message for mode		*/
#define FJ_TITLE_INTERRUPT	(0x06000000)	/**< Host message for intertupt	*/
#define FJ_TITLE_HOST		(0x07000000)	/**< Host message for host		*/
#define FJ_TITLE_THREAD		(0x08000000)	/**< Host message for thread	*/
#define FJ_TITLE_PREVIEW	(0x09000000)	/**< Host message for preview	*/
#define FJ_TITLE_CAPTURE	(0x0A000000)	/**< Host message for capture	*/
#define FJ_TITLE_RESTORE	(0x0B000000)	/**< Host message for restore	*/
#define FJ_TITLE_MEMORY		(0x0E000000)	/**< Host message for memory	*/
#define FJ_TITLE_VIDEO		(0x17000000)	/**< Host message for Movie		*/
#define FJ_TITLE_AUDIO		(0x18000000)	/**< Host message for Audio		*/
#define FJ_TITLE_UART		(0x1A000000)	/**< Host message for uart		*/
#define FJ_TITLE_INTERFACE	(0x1C000000)	/**< Host message for interface	*/
#define FJ_TITLE_GENERAL	(0x1D000000)	/**< Host message for general	*/
#define FJ_TITLE_PERI		(0x1E000000)	/**< Host message for peripheral*/
#define FJ_TITLE_AE			(0x30000000)	/**< Host message for AE		*/
#define FJ_TITLE_AF			(0x31000000)	/**< Host message for AF		*/
#define FJ_TITLE_AWB		(0x32000000)	/**< Host message for AWB		*/
#define FJ_TITLE_AFEMODE	(0x34000000)	/**< Host message for AFE mode	*/
#define FJ_TITLE_MEDIA		(0x04000000)	/**< Host message for media		*/
#define FJ_TITLE_PLAYBACK	(0x0F000000)	/**< Host message for PlayBack	*/
#define FJ_TITLE_DCF		(0x16000000)	/**< Host message for DCF		*/
// --- REMOVE_USB BEGIN ---
#define FJ_TITLE_USB		(0x19000000)	/**< Host message for USB		*/
#define FJ_TITLE_PICTBRIDGE (0x33000000)	/**< Host message for PictBridge	*/
// --- REMOVE_USB END ---
#define FJ_TITLE_SUB		(0x40000000)	/**< Host message for sub Task	*/

/*@} --- end of fj_message_definition */

///////////////////////////////////////////////////////////////////////////////
// Definition of detail message code for each function
// : Described a explanation of the following message to each function.
///////////////////////////////////////////////////////////////////////////////
#ifdef FJ_TITLE_HOSTMESSAGE_ON
#define FJ_HM_CREATE							(FJ_TITLE_BOOT | 0x00000001)
#define	FJ_HM_NEW_MODE							(FJ_TITLE_MODE | 0x00000001)
#define	FJ_HM_CALIB_MODE_END					(FJ_TITLE_MODE | 0x00000002)
#define FJ_HM_DISPLAY_WINDOW_COMPLETE			(FJ_TITLE_PREVIEW | 0x00000001)
#define FJ_HM_OPERATION_COMPLETE				(FJ_TITLE_PREVIEW | 0x00000002)		// delete in the future.
#define FJ_HM_VIDEO_RAW_READY					(FJ_TITLE_PREVIEW | 0x00000003)
#define FJ_HM_SCENE_STANDBY						(FJ_TITLE_PREVIEW | 0x0000000A)

#define FJ_HM_STILL_RAW_READY					(FJ_TITLE_CAPTURE | 0x00000001)
#define	FJ_HM_SEND_CONFIRM_STORE				(FJ_TITLE_CAPTURE | 0x00000002)
#define	FJ_HM_STORE_FINISHED					(FJ_TITLE_CAPTURE | 0x00000003)
#define	FJ_HM_BKG_STORE_COMPLETE				(FJ_TITLE_CAPTURE | 0x00000004)
#define	FJ_HM_QVIEW_START						(FJ_TITLE_CAPTURE | 0x00000005)
#define	FJ_HM_QVIEW_END							(FJ_TITLE_CAPTURE | 0x00000006)
#define FJ_HM_SEND_USER_CONFIRM					(FJ_TITLE_CAPTURE | 0x00000007)
#define	FJ_HM_VIDEO_MODE_READY					(FJ_TITLE_CAPTURE | 0x00000008)
#define	FJ_HM_VALID_IMAGE						(FJ_TITLE_RESTORE | 0x00000001)
#define	FJ_HM_PCC_FINISHED						(FJ_TITLE_VIDEO | 0x00000001)
#define	FJ_HM_PLAYBACK_CURR_CHUNK				(FJ_TITLE_VIDEO | 0x00000002)
#define	FJ_HM_MOVIE_H264_PLAY_COMPLETE			(FJ_TITLE_VIDEO | 0x00000003)
#define	FJ_HM_MOVIE_H265_PLAY_COMPLETE			(FJ_TITLE_VIDEO | 0x00000004)
#define	FJ_HM_MOVIE_REACHED_MAXTIME				(FJ_TITLE_VIDEO | 0x00000005)
#define	FJ_HM_MOVIE_REC_BEGIN					(FJ_TITLE_VIDEO | 0x00000006)
#define	FJ_HM_MOVIE_TTR_OPERATION_COMPLETE		(FJ_TITLE_VIDEO | 0x00000007)
#define	FJ_HM_MOVIE_CRITICAL_ERROR				(FJ_TITLE_VIDEO | 0x00000008)
#define	FJ_HM_MOVIE_SETUP_COMPLETE				(FJ_TITLE_VIDEO | 0x00000009)
#define	FJ_HM_MOVIE_H264_CODEC_ERROR			(FJ_TITLE_VIDEO | 0x0000000C)
#define	FJ_HM_AUDIO_OUT_AUTO_STOP				(FJ_TITLE_AUDIO | 0x00000001)
#define	FJ_HM_AUDIO_OUT_BEGIN					(FJ_TITLE_AUDIO | 0x00000002)
#define	FJ_HM_PERI_RTC_ALARM					(FJ_TITLE_PERI | 0x00000001)

#define FJ_HM_FLASH_CARD_DETECT					(FJ_TITLE_MEDIA | 0x00000001)
#define FJ_HM_NOT_ENOUGH_FLASH_MEMORY			(FJ_TITLE_MEDIA | 0x00000002)
#define	FJ_HM_MOUNT_COMPLETED					(FJ_TITLE_MEDIA | 0x00000003)
#define	FJ_HM_MOUNT_DUPLICATED_FOUND			(FJ_TITLE_MEDIA | 0x00000004)
#define	FJ_HM_GET_MEDIA_STATUS_COMPLETED		(FJ_TITLE_MEDIA | 0x00000005)
#define	FJ_HM_NOT_ENOUGH_DCFDB					(FJ_TITLE_MEDIA | 0x00000006)
#define	FJ_HM_MAX_FILENUMBER_DCFDB				(FJ_TITLE_MEDIA | 0x00000007)
#define	FJ_HM_SLOW_MEDIA_ERROR					(FJ_TITLE_MEDIA | 0x00000008)
#define FJ_HM_TITLE_DCF_DB_DATE_UPDATE_COMPLETE	(FJ_TITLE_DCF | 0x00000001)
// --- REMOVE_USB BEGIN ---
#define	FJ_HM_MASS_STORAGE						(FJ_TITLE_USB | 0x00000002)
#define	FJ_HM_TITLE_USB_MODE_CHANGE				(FJ_TITLE_USB | 0x00000014)
#define	FJ_HM_TITLE_USB_BUS_TRANSFER			(FJ_TITLE_USB | 0x00000018)
// --- REMOVE_USB END ---
#define	FJ_HM_NOTICE_SUB_EVENT					(FJ_TITLE_SUB | 0x00000001)
#define	FJ_HM_IPCU_BUFFER_ERROR					(FJ_TITLE_MEMORY | 0x00000001)

#endif	/* FJ_TITLE_HOSTMESSAGE_ON */

/* for uiFirstPixelColor in RAWINFO structure */
#define SENSOR_FIRST_PIXEL_RG	(0x00)	/* Sensor first pixel RG */
#define SENSOR_FIRST_PIXEL_GR	(0x01)	/* Sensor first pixel GR */
#define SENSOR_FIRST_PIXEL_GB	(0x02)	/* Sensor first pixel GB */
#define SENSOR_FIRST_PIXEL_BG	(0x03)	/* Sensor first pixel BG */

#define FJ_3A_INIT_VIDEO			(0)	/**< 1st boot process. */
#define FJ_3A_INIT_AF				(1)	/**< 2nd boot process. */
#define FJ_3A_INIT_STILL			(2)	/**< 3rd boot process. */
#define FJ_3A_INIT_VIDEO_END		(3)	/**< 3rd boot process. */

#define FJ_RAW_SCLR_ERROR_WIDTH		(1 << 0)	/**< raw width of unexpect output from sensor. */
#define FJ_RAW_SCLR_ERROR_LINES		(1 << 1)	/**< raw lines of unexpect output from sensor. */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** RAW manipulation No.(for video)
*/
typedef enum {
	FJ_RAW_MANI_VIDEO_POST_SEN = 0,		/**< Post-SEN	*/
	FJ_RAW_MANI_VIDEO_POST_SRO,			/**< Post-SRO	*/
	FJ_RAW_MANI_VIDEO_POST_B2B,			/**< Post-B2B	*/
	FJ_RAW_MANI_VIDEO_MAX
} FJ_RAW_MANI_VIDEO;

/** RAW manipulation No.(for still)
*/
typedef enum {
	FJ_RAW_MANI_STILL_POST_SEN = 0,		/**< Post-SEN	*/
	FJ_RAW_MANI_STILL_POST_SRO,			/**< Post-SRO	*/
	FJ_RAW_MANI_STILL_POST_B2B,			/**< Post-B2B	*/
	FJ_RAW_MANI_STILL_MAX
} FJ_RAW_MANI_STILL;

/** YUV manipulation No.(for video)
*/
typedef enum {
	FJ_YUV_MANI_VIDEO_PRE_CNR = 0,		/**< Pre-CNR	*/
	FJ_YUV_MANI_VIDEO_POST_CNR,			/**< Post-CNR	*/
	FJ_YUV_MANI_VIDEO_POST_WARP,		/**< Post-WARP	*/
	FJ_YUV_MANI_VIDEO_THUMB,			/**< Post-Video Thumbnail Resize	*/
	FJ_YUV_MANI_VIDEO_POST_DEMOSAIC,	/**< Post-Demosaic	*/
	FJ_YUV_MANI_VIDEO_PRE_ENCODE,		/**< Pre Encode (Encode image ready)	*/
	FJ_YUV_MANI_VIDEO_POST_R2Y,			/**< Post-R2Y	*/
	FJ_YUV_MANI_VIDEO_MAX
} FJ_YUV_MANI_VIDEO;

/** YUV manipulation No.(for still)
*/
typedef enum {
	FJ_YUV_MANI_STILL_PRE_CNR = 0,		/**< Pre-CNR		*/
	FJ_YUV_MANI_STILL_POST_CNR,			/**< Post-CNR		*/
	FJ_YUV_MANI_STILL_POST_LTM_MAP,		/**< Post-LTM MAP	*/
	FJ_YUV_MANI_STILL_POST_LTM_CORE,	/**< Post-LTM CORE	*/
	FJ_YUV_MANI_STILL_POST_WARP,		/**< Post-WARP		*/
	FJ_YUV_MANI_STILL_POST_DEMOSAIC,	/**< Post-Demosaic	*/
	FJ_YUV_MANI_STILL_PRE_ENCODE,		/**< Pre Encode (Encode image ready)	*/
	FJ_YUV_MANI_STILL_MAX
} FJ_YUV_MANI_STILL;

/** YUV manipulation No.(for SHDR)
*/
typedef enum {
	FJ_YUV_MANI_SHDR_PRE_SHDR_1ST = 0,	/**< Pre  SHDR 1st				*/
	FJ_YUV_MANI_SHDR_POST_SHDR_1ST,		/**< Post SHDR 1st				*/
	FJ_YUV_MANI_SHDR_PRE_SHDR_2ND,		/**< Pre  SHDR 2nd				*/
	FJ_YUV_MANI_SHDR_MAX
} FJ_YUV_MANI_SHDR;

/** RGB manipulation No.(for SHDR)
*/
typedef enum {
	FJ_RGB_MANI_SHDR_POST_SHDR = 0,		/**< Post SHDR					*/
	FJ_RGB_MANI_SHDR_MAX
} FJ_RGB_MANI_SHDR;

/** Jpg manipulation No.(for video)
*/
typedef enum {
	FJ_JPG_MANI_VIDEO_THUMB = 0,		/**< Post-Video Thumbnail Resize	*/
	FJ_JPG_MANI_VIDEO_MAX
} FJ_JPG_MANI_VIDEO;

/** Jpg manipulation No.(for still)
*/
typedef enum {
	FJ_JPG_MANI_STILL_THUMB = 0,	/**< Post-Still Thumbnail Jpeg Encode	*/
	FJ_JPG_MANI_STILL_SCREEN,		/**< Post-Still Screennail Jpeg Encode	*/
	FJ_JPG_MANI_STILL_MAIN,			/**< Post-Still Main image Jpeg Encode	*/
	FJ_JPG_MANI_STILL_MAX
} FJ_JPG_MANI_STILL;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** Raw scaler configuration.
@image html fj_still_raw_scaler.png
*/
typedef struct {
	USHORT					in_ofs_y;				/**< Input offset lines of raw scaler. */
	USHORT					in_left_ofs_x;			/**< Input left offset width of raw scaler. */
	USHORT					in_left_width;			/**< Input left width of raw scaler. */
	USHORT					in_right_ofs_x;			/**< Input right offset width of raw scaler. */
	USHORT					in_right_width;			/**< Input right width of raw scaler. */
	USHORT					in_width_overlap;		/**< Overlap width of input image. */
	USHORT					in_lines;				/**< Input lines */
	USHORT					out_left_width;			/**< Output left width of raw scaler. */
	USHORT					out_right_width; 		/**< Output right width of raw scaler. */
	USHORT					out_width_overlap;		/**< Overlap width of output image. */
	USHORT					out_lines;				/**< Output lines */
} FJ_STILL_RAW_SCALER_CFG;

/** Raw scaler error information.
*/
typedef struct {
	ULONG 					err_status;				/**< [in]Error status. @ref FJ_RAW_SCLR_ERROR_WIDTH, @ref FJ_RAW_SCLR_ERROR_LINES  */
	USHORT					err_raw_width;			/**< [in]raw width of unexpect output from sensor. */
	USHORT					err_raw_lines;			/**< [in]raw lines of unexpect output from sensor. */
	BOOL					is_reboot;				/**< [out]Raw scaler reboot flag. */
} FJ_STILL_RAW_SCALER_ERR_INFO;

/** IIP D-Zoom setting parameter. */
typedef struct {
	FJ_IMG_CROP_RECT	src_rect;		/**< Image crop parameter */
	FJ_IMG_RESIZE_MODE	resize_mode;	/**< Interpolation mode for the image resize */
} FJ_IMG_DZOOM_INFO;

/** save file data information. */
typedef struct {
	UCHAR					data_type;				/**< data type */
	ULONG					data_idx;				/**< data index */
	ULONG					data_addr;				/**< data address */
	ULONG					data_size;				/**< data size */
} FJ_STILL_RAW_DATA_INFO;

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
///////////////////////////////////////////////////////////////////////////////
// CallBack APIs
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// ARM IPCU
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_arm_callback
@{*/

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This is callback API.<br>
The Base F/W executes just when the sending with Async finished.<br>
@param[in] id: instance ID, same as parameter of FJ_IPCU_Send API.
@param[in] data: top address of the sending data table, same as parameter of FJ_IPCU_Send API.
@param[in] length: byte size of data, same as parameter of FJ_IPCU_Send API.
*/
VOID FJ_HOST_IPCUFinishCB(UCHAR id, UCHAR *data, UINT32 length);

/*@} --- end of fj_arm_callback */

///////////////////////////////////////////////////////////////////////////////
// Boot
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_boot_callback
@{*/

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This is callback API.<br>
The Base F/W executes Booting Process of Host Side during boot procedure.<br>
@param[in]  ulArgument For FJ past parameter to host.
@retval		None.
@remarks	When Booting Process is started, the function called and the argument is "FJ_PRE_PROC_ENTER_FIRST".<br>
			After then, the function called and the argument is "FJ_PRE_PROC_ENTER_SECOND".<br>
			Finally, the function called and the argument is "FJ_PRE_PROC_ENTER_THIRD".<br>
			Therefore, Host side has to decide to do anything on each stages.<br>
@code
	[Sample Code]
	VOID FJ_HostPreBootProc(ULONG ulArgument)
	{
		switch( ulArgument ){
			case FJ_PRE_PROC_ENTER_FIRST :
				// GPIO Initialization
				// Sensor/AFE Power ON
				// Lens Initialization
				// UI set read from Nand Flash
				// Battery Initialization
				// LCD Initialization
				break;
			case FJ_PRE_PROC_ENTER_SECOND :
				// UART Start
				// Set Sensor window
				break;
			case FJ_PRE_PROC_ENTER_THIRD :
				// Change system mode
				break;
			default:
				break;
		}
	}
@endcode
*/
VOID	FJ_HostPreBootProc( ULONG ulArgument ); // Described a detail in Boot function

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This is callback API.<br>
The Base F/W executes Host Main Process after boot operation.<br>
Using this thread for user procedure all their functions.<br>
@param[in]  ulArgument For FJ past parameter to host.
@retval		None.
@remarks	The BaseFW calls this function to ask host to start host main process.<br>
			It includes starting event observer task(key-push, dial-move, and other<br>
			peripheral devices as external media, HDMI, video jack, etc.), and main loop<br>
			in which host receives messages from the BaseFW and process accordingly.<br>

@code
	[Sample Code]
	VOID FJ_HostProc(ULONG ulArgument)
	{
		ULONG uMsgID;
		ULONG uMsgPara;
		
    	while( 1 ) {
			FJ_HostGetMessage ( &uMsgID, &uMsgPara );
			if( uMsgID != 0 ) {
				// Execution Host Side Code
			}
		}
	}
@endcode
*/
VOID	FJ_HostProc( ULONG ulArgument ); // Described a detail in Boot function


/*@} --- end of fj_boot_callback */


///////////////////////////////////////////////////////////////////////////////
// Mode
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_mode_callback
@{*/
/*@} --- end of fj_mode_callback */


///////////////////////////////////////////////////////////////////////////////
// SR
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_sr_callback
@{*/
/*@} --- end of fj_sr_callback */


///////////////////////////////////////////////////////////////////////////////
// standard
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_std_callback
@{*/
/*@} --- end of fj_std_callback */


///////////////////////////////////////////////////////////////////////////////
// Memory
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_memory_callback
@{*/
/*@} --- end of fj_memory_callback */

///////////////////////////////////////////////////////////////////////////////
// Message
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_message_callback
@{*/
/*@} --- end of fj_message_callback */


///////////////////////////////////////////////////////////////////////////////
// Peripheral
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_peripheral_callback
@{*/
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This callback API is to call back when it receives a data_count that is set in FJ_UART_Receive.
@param[in] ch		Receive channel number
@param[in] cb_info	Callback information
*/
VOID FJ_Host_UART_Receive_CB( UCHAR ch, FJ_UART_CB_INFO cb_info );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This callback API is to call back when it receives a slave_addr that is set in FJ_UART_Set_Slave_Addr.
@param[in] ch		Receive channel number
@param[in] cb_info	Callback information
*/
VOID FJ_Host_UART_recv_Slave_Addr_CB( UCHAR ch, FJ_UART_CB_INFO cb_info );

/*@} --- end of fj_peripheral_callback */

///////////////////////////////////////////////////////////////////////////////
// Video / Still
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_still_callback
@{
	This chapter describes API callback timing chart about Video, Still , SHDR and Clean Bayer.<br>
	For more detailed of callback APIs, please refer to description of each callback APIs.

	- @ref fj_still_callback_timing_chart
		- @ref fj_still_callback_timing_chart_video
		- @ref fj_still_callback_timing_chart_still
		- @ref fj_still_callback_timing_chart_shdr
		- @ref fj_still_callback_timing_chart_clean_bayer

	<hr>
	@section fj_still_callback_timing_chart	Host Callback API Timing chart
	@subsection fj_still_callback_timing_chart_video Video Callback API Timing chart
	@image html fj_still_callback_timing_chart_video.png

	<hr>
	@subsection fj_still_callback_timing_chart_still Still Callback API Timing chart
	@image html fj_still_callback_timing_chart_still.png

	<hr>
	@subsection fj_still_callback_timing_chart_shdr Still HDR Callback API Timing chart
	@image html fj_still_callback_timing_chart_shdr.png

	<hr>
	@subsection fj_still_callback_timing_chart_clean_bayer Clean Bayer Callback API Timing chart
	@image html fj_still_callback_timing_chart_clean_bayer.png
*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function called in Video state.<br>
Host controls 3a parameter from this API.
@param[in]	sro_id : sro instance id ( to know which instance is called this API ).
@param[in]	init_type is 3a init type select parameter.[0]:video [1]:af [2]:still
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_Host3AInitCB( UCHAR sro_id, UCHAR init_type );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function called in Video state.<br>
Host controls Ahip initialize from this API.
@param[in]	demosaic_id : Demosaic instance ID.
@param[in]	input_width : horizontal size of the AHIP input image.
@param[in]	input_lines : vertical size of the AHIP input image.
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostAhipInitCB( UCHAR demosaic_id, UINT32 input_width, UINT32 input_lines );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function called in Video state.<br>
Host configure RAW scaler parameter from this API.
@param[in]	demosaic_id : Demosaic instance ID.
@param[in,out]	raw_scaler_cfg : configuration of RAW scaller.See @ref FJ_STILL_RAW_SCALER_CFG<br>
													in_left_width		: output parameter <br>
													in_right_width		: output parameter <br>
													in_width_overlap	: output parameter <br>
													in_lines			: output parameter <br>
													out_left_width		: input parameter <br>
													out_right_width 	: input parameter <br>
													out_width_overlap	: input parameter <br>
													out_lines			: output parameter <br>
@retval 	TRUE  : Success
@retval 	FALSE : Error
*/

VOID FJ_HostRawScalerConfigCB( UCHAR demosaic_id, FJ_STILL_RAW_SCALER_CFG* raw_scaler_cfg );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function called once in Video state once.<br>
Host initiates RAW scaler from this API.
@param[in]	demosaic_id : Demosaic instance ID.
@retval 	TRUE  : Success
@retval 	FALSE : Error
*/

VOID FJ_HostRawScalerStartCB( UCHAR demosaic_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function called once in Video state once.<br>
Host stops RAW scaler from this API.
@param[in]	demosaic_id : Demosaic instance ID.
@retval 	TRUE  : Success
@retval 	FALSE : Error
*/

VOID FJ_HostRawScalerStopCB( UCHAR demosaic_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function called once in Video state once.<br>
Host stops RAW scaler from this API.
@param[in]		demosaic_id : Demosaic instance ID.
@param[in,out]	err_info : Error information of RAW scaller.See @ref FJ_STILL_RAW_SCALER_ERR_INFO<br>
@retval 	TRUE  : Success
@retval 	FALSE : Error
*/

VOID FJ_HostRawScalerErrorCB( UCHAR demosaic_id, FJ_STILL_RAW_SCALER_ERR_INFO* err_info );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API is set a information on RAW to Host.<br>
@remarks	1.First line of raw data begins in psInfo->pbyBuffer.<br>
			2.Data is 8 bits per pixel.<br>
			3.The stating address of line N of raw data:<br>
			  psInfo->pbyBuffer + N * psInfo->uiLineJumpInBytes<br>
			4.More available info on the viewed image:<br>
			  psInfo->uiSizeX - Number of pixels per image line.<br>
			  psInfo->uiSizeY - Number of image lines.<br>
			  psInfo->uiFirstPixelColor -
				SENSOR_FIRST_PIXEL_RG(0x00)<br>
				SENSOR_FIRST_PIXEL_GR(0x01)<br>
				SENSOR_FIRST_PIXEL_GB(0x02)<br>
				SENSOR_FIRST_PIXEL_BG(0x03)<br>
			  psInfo->puiBleArr - Array holding .<br>
			  psInfo->uiBleArrSize - The size of the array above(should equal 3).<br>
			  psInfo->offset.up - OB top offset area of raw image.<br>
			  psInfo->offset.bottom - OB bottom offset area of raw image.<br>
			  psInfo->offset.right - OB torightp offset area of raw image.<br>
			  psInfo->offset.left - OB left offset area of raw image.<br>
			  psInfo->offset - Offset of effective image area.<br>
@param[in]	inst_id		: Instance ID.<br>
							mani_no = "FJ_RAW_MANI_VIDEO_POST_SEN", SEN instance ID.<br>
							mani_no = "FJ_RAW_MANI_VIDEO_POST_SRO/B2B", Demosaic instance ID.<br>
@param[in]	mani_no		: this number means each image processing progress timing.<br>
							For more detailed, please refer to @ref fj_still_callback_timing_chart.
@param[in]	psInfo		: A structure containing information about the raw data frame<br>
						  that may be accessed(frame size, frame no, etc.)
@param[out]	puwScalers	: A location to return 3 new calculated WB scalers.<br>
						  If WB is not executed in the function, just ignore the parameter.
@retval		TRUE  : if data was changed.
@retval		FALSE : if data was not changed.
 */
BOOL FJ_HostVideoRawManipulation( UCHAR inst_id, FJ_RAW_MANI_VIDEO mani_no, PSRAWINFO psInfo, USHORT* puwScalers );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This API sets a information on YUV to Host.<br>
@param[in]	demosaic_id			: Demosaic instance ID.<br>
@param[in]	output_id	: Demosaic Output ID.<br>
							If mani_no is other than "FJ_YUV_MANI_VIDEO_POST_DEMOSAIC" and "FJ_YUV_MANI_VIDEO_PRE_ENCODE", notify output_id is 0xFF.<br>
							For more detailed, please refer to @ref fj_still_callback_timing_chart and sequences(Ex:@ref fj_still_capture_sequence_section1).
@param[in]	mani_no		: this number means each image processing progress timing.<br>
							For more detailed, please refer to @ref fj_still_callback_timing_chart.
@param[in]	psInfo	: A structure containing information about the YUV data
					 frame that may be accessed(frame size, frame no, stream id, etc.)
@retval		TRUE  : if data was changed.
@retval		FALSE : if data was not changed.
 */
VOID FJ_HostVideoYUVManipulation( UCHAR demosaic_id, UCHAR output_id, FJ_YUV_MANI_VIDEO mani_no, PSYUVINFO psInfo );

/* --- Still --- */
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Function that passed information on RAW data frame.
@remarks	<table border=0>
			<tr><td valign=top>1.</td><td>First line of raw data begins in psInfo->pbyBuffer.<br></td></tr>
			<tr><td valign=top>2.</td><td>Data is 12 bits per sample packed so for R G pixels located in 3 bytes:<br>
				R[11:4]=byte0[7:0], R[3:0]=byte1[7:4], G[11:8]=byte1[3:0], G[7:0]=byte2[7:0]<br></td></tr>
			<tr><td valign=top>3.</td><td>The stating address of line N of raw data is:<br>
				psInfo->pbyBuffer + N * psInfo->uiLineJumpInBytes<br></td></tr>
			<tr><td valign=top>4.</td><td>More available info on the captured image:<br>
				psInfo->uiSizeX - Number of pixels per image line.<br>
				psInfo->uiSizeY - Number of image lines.<br>
				<table border=0>
				<tr><td valign=top nowrap >psInfo->uiFirstPixelColor -</td>
					<td nowrap >@ref SENSOR_FIRST_PIXEL_RG, @ref SENSOR_FIRST_PIXEL_GR<br>
					@ref SENSOR_FIRST_PIXEL_GB, @ref SENSOR_FIRST_PIXEL_BG<br></td></tr>
				</table>
				psInfo->puiBleArr - Array holding the black level evaluation(BLE) 
				value used for black level correction(BLC) of the captured frame. 
				The values are given in the R-G-B order and hold a sum over 512 pixels(each). 
				In case there is more than one field, the array holds the BLE values 
				per each field, and it size equals to 3 * \#fields.<br>
				psInfo->uiBleArrSize - The size of the array above.<br>
				psInfo->offset.up - OB top offset area of raw image.<br>
				psInfo->offset.bottom - OB bottom offset area of raw image.<br>
				psInfo->offset.right - OB torightp offset area of raw image.<br>
				psInfo->offset.left - OB left offset area of raw image.<br>
				psInfo->offset - Offset of effective image area.<br></td></tr>
			<tr><td valign=top>5.</td><td>Do not alter puwScalers unless WB algorithm is executed in this function.<br>
				In that case, new requested scalers should be returned in 
				puwScalers[0](ScaleR), puwScalers[1](ScaleG) and puwScalers[2](ScaleB).<br>
				5. FJ_HostStillRawManipulation() is called right after RAW data is grabed from sensor.<br></td></tr>
			<tr><td valign=top>6.</td><td>RTC data for EXIF is got just after when raw data is ready.<br> 
			</table>
			@image html fj_still_drammanipulation.png
@param[in]	inst_id		: Instance ID.<br>
							mani_no = "FJ_RAW_MANI_STILL_POST_SEN", SEN instance ID.<br>
							mani_no = "FJ_RAW_MANI_STILL_POST_SRO/B2B", Demosaic instance ID.<br>
@param[in]	mani_no		: this number means each image processing progress timing.<br>
							For more detailed, please refer to @ref fj_still_callback_timing_chart.
@param[in]	psInfo		: A structure containing information about the raw data frame<br>
						  that may be accessed
@param[out]	puwScalers	: A location to return 3 new calculated WB scalers.<br>
						  If WB is not executed in the function, just ignore the parameter.
 */
VOID FJ_HostStillRawManipulation( UCHAR inst_id, FJ_RAW_MANI_STILL mani_no, PSRAWINFO psInfo, USHORT* puwScalers );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This API sets a information on YUV to Host.<br>
@remarks	<table border=0>
			<tr><td valign=top>1.</td><td>First line of YUV data begins in psInfo->pbyBuffer.<br></td></tr>
			<tr><td valign=top>2.</td><td>The YUV data is saved by frame sequential method.<br></td></tr>
			<tr><td valign=top>3.</td><td>More available info on the captured image:<br>
				psInfo->uiSizeX - Number of pixels per image line.<br>
				psInfo->uiSizeY - Number of image lines.<br>
			<tr><td valign=top>4.</td><td>Additional information<br>
				psInfo->pbyOptionalBuffer - Optional buffer pointer(for example used in 3rd IP)<br>
				psInfo->uiOptionalSizeX - Width of optional buffer in pixels.<br>
				psInfo->uiOptionalSizeY - Height of optional buffer in pixels.<br>
@param[in]	demosaic_id			: Demosaic instance ID.<br>
@param[in]	output_id	: Demosaic Output ID.<br>
							If mani_no is other than "FJ_YUV_MANI_VIDEO_POST_DEMOSAIC" and "FJ_YUV_MANI_VIDEO_PRE_ENCODE", notify output_id is 0xFF.<br>
							For more detailed, please refer to @ref fj_still_callback_timing_chart and sequences(Ex:@ref fj_still_capture_sequence_section5).
@param[in]	mani_no		: this number means each image processing progress timing.<br>
							For more detailed, please refer to @ref fj_still_callback_timing_chart.
@param[in]	psInfo	: A structure containing information about the YUV data frame
					  that may be accessed.(exposure, gain, frame size, etc)
@retval		0 : Return value currently has no meaning. Must by zero(0).
 */
CHAR FJ_HostStillYUVManipulation( UCHAR demosaic_id, UCHAR output_id, FJ_YUV_MANI_STILL mani_no, PSYUVINFO psInfo );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This callback function for the Stitch execution.<br>
@param[in]	stitch_id	: Stitch instance ID.<br>
@param[in]	psInfo	: A structure containing information about the YUV data frame that may be accessed.
@retval		0 : Return value currently has no meaning. Must by zero(0).
@remarks	For each parameter, please refer to following images.<br>
			@image html fj_still_stitch_config.png
 */
CHAR FJ_HostStitchYUVManipulation( UCHAR stitch_id, PSSTITCHINFO psInfo );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This callback function for raw data format is converted to dng data format.<br>
@param[in]	dcf_raw_data	: raw data information.<br>
@param[out]	dcf_dng_data	: dng data information.<br>
@retval		0 : Return value currently has no meaning. Must by zero(0).
 */
UINT32 FJ_HostStillRaw2Dng( FJ_STILL_RAW_DATA_INFO* dcf_raw_data, FJ_STILL_RAW_DATA_INFO* dcf_dng_data );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This callback function for the Stitch execution.<br>
@param[in]	encode_id	: Encode instance ID.<br>
@param[in]	frame_no	: Number of readout frame.<br>
@param[in]	piv_img_info: PIV image information.<br>
@retval		0 : Return value currently has no meaning. Must by zero(0).
 */
UINT32 FJ_HostStitchSavePIVImg( UCHAR encode_id, ULLONG frame_no, FJ_IMG_INFO piv_img_info );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This API sets a information on YUV to Host for SHDR.<br>
@remarks	<table border=0>
			<tr><td valign=top>1.</td><td>First line of YUV data begins in psInfo->pbyBuffer.<br></td></tr>
			<tr><td valign=top>2.</td><td>The YUV data is saved by frame sequential method.<br></td></tr>
			<tr><td valign=top>3.</td><td>More available info on the captured image:<br>
				psInfo->uiSizeX - Number of pixels per image line.<br>
				psInfo->uiSizeY - Number of image lines.<br>
			<tr><td valign=top>4.</td><td>Additional information<br>
				psInfo->pbyOptionalBuffer - Optional buffer pointer(for example used in 3rd IP)<br>
				psInfo->uiOptionalSizeX - Width of optional buffer in pixels.<br>
				psInfo->uiOptionalSizeY - Height of optional buffer in pixels.<br>
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[in]	output_id	: Demosaic Output ID.<br>
							If mani_no is other than "FJ_YUV_MANI_VIDEO_POST_DEMOSAIC" and "FJ_YUV_MANI_VIDEO_PRE_ENCODE", notify output_id is 0xFF.<br>
							For more detailed, please refer to @ref fj_still_callback_timing_chart and sequences(Ex:@ref fj_still_capture_sequence_section6).
@param[in]	mani_no		: this number means each image processing progress timing.<br>
							For more detailed, please refer to @ref fj_still_callback_timing_chart.
@param[in]	psInfo	: A structure containing information about the YUV data frame
					  that may be accessed.(exposure, gain, frame size, etc)
@retval		0 : Return value currently has no meaning. Must by zero(0).
 */
CHAR FJ_HostSHDRYUVManipulation( UCHAR demosaic_id, UCHAR output_id, FJ_YUV_MANI_SHDR mani_no, PSYUVINFO psInfo );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This API sets a information on RGB to Host for SHDR.<br>
@remarks	<table border=0>
			<tr><td valign=top>1.</td><td>First line of RGB data begins in psInfo->pbyBuffer.<br></td></tr>
			<tr><td valign=top>2.</td><td>The RGB data is saved by frame sequential method.<br></td></tr>
			<tr><td valign=top>3.</td><td>More available info on the captured image:<br>
				psInfo->uiSizeX - Number of pixels per image line.<br>
				psInfo->uiSizeY - Number of image lines.<br>
			<tr><td valign=top>4.</td><td>Additional information<br>
				psInfo->pbyOptionalBuffer - Optional buffer pointer(for example used in 3rd IP)<br>
				psInfo->uiOptionalSizeX - Width of optional buffer in pixels.<br>
				psInfo->uiOptionalSizeY - Height of optional buffer in pixels.<br>
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[in]	output_id	: Demosaic Output ID.<br>
							If mani_no is other than "FJ_YUV_MANI_VIDEO_POST_DEMOSAIC" and "FJ_YUV_MANI_VIDEO_PRE_ENCODE", notify output_id is 0xFF.<br>
							For more detailed, please refer to @ref fj_still_callback_timing_chart and sequences(Ex:@ref fj_still_capture_sequence_section6).
@param[in]	mani_no		: this number means each image processing progress timing.<br>
							For more detailed, please refer to @ref fj_still_callback_timing_chart.
@param[in]	psInfo	: A structure containing information about the RGB data frame
					  that may be accessed.(exposure, gain, frame size, etc)
@retval		0 : Return value currently has no meaning. Must by zero(0).
 */
CHAR FJ_HostSHDRRGBManipulation( UCHAR demosaic_id, UCHAR output_id, FJ_RGB_MANI_SHDR mani_no, PSRGBINFO psInfo );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Function that passes jpeg data and information on YUV data frame for Video record.
@remarks	<table border=0>
			<tr><td valign=top>1.</td><td>First line of YUV data begins in psJpgInfo->pbyYUVBuffer.<br></td></tr>
			<tr><td valign=top>2.</td><td>The stating address of line N of raw data is:<br>
				psJpgInfo->pbyYuvBuffer + N * psJpgInfo->uiYuvLineJumpInBytes<br></td></tr>
			<tr><td valign=top>3.</td><td>More available info on the captured image:<br>
				psJpgInfo->uiYuvSizeX - Number of pixels per image line.<br>
				psJpgInfo->uiYuvSizeY - Number of image lines.<br>
			<tr><td valign=top>4.</td><td>One pixel is 16 bites of YU or YV(YUV format):<br>
				The bytes are organized as follwing(each sample below is 8 bit):<br>
				Y0 U0 Y1 V0 Y2 U2 Y3 V2<br>
			<tr><td valign=top>5.</td><td>Jpeg stream is located at psJpgInfo->pbyJpg<br></td></tr>
			<tr><td valign=top>6.</td><td>Original(maximum) size of the psJpgInfo->pbyJpg buffer is stored in psJpgInfo->uiMaxJpgSize<br></td></tr>
			<tr><td valign=top>7.</td><td>Actual Jpeg stream size is stored in *psJpgInfo->puiActJpgSize 
				and can be changed up to the maximum of psJpgInfo->uiMaxJpgSize<br></td></tr>
			<tr><td valign=top>8.</td><td>Jpeg stream index is stored in *psJpgInfo->uiJpgIdx.<br>
				In case of the last frame, *psJpgInfo->blLastFrameFlag is set at TRUE.<br></td></tr>
			<tr><td valign=top>9.</td><td>The YUV data is provided for reference only, 
				since it is not in use anymore. 
				The buffer and the data will be freed right after return of this function.<br></td></tr>
@param[in]	encode_id	: Encode instance ID.<br>
@param[in]	mani_no		: this number means each image processing progress timing.<br>
							For more detailed, please refer to @ref fj_still_callback_timing_chart.
@param[in]	psJpgInfo	: A structure containing information about the jpeg stream and YUV data frame
@retval		0 : Return value currently has no meaning. Must by zero(0).
*/
UINT32 FJ_HostVideoJpgManipulation( UCHAR encode_id, FJ_JPG_MANI_VIDEO mani_no, PSJPGINFO psJpgInfo );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Function that passes jpeg data and information on YUV data frame for Still.
@remarks	<table border=0>
			<tr><td valign=top>1.</td><td>First line of YUV data begins in psJpgInfo->pbyYUVBuffer.<br></td></tr>
			<tr><td valign=top>2.</td><td>The stating address of line N of raw data is:<br>
				psJpgInfo->pbyYuvBuffer + N * psJpgInfo->uiYuvLineJumpInBytes<br></td></tr>
			<tr><td valign=top>3.</td><td>More available info on the captured image:<br>
				psJpgInfo->uiYuvSizeX - Number of pixels per image line.<br>
				psJpgInfo->uiYuvSizeY - Number of image lines.<br>
			<tr><td valign=top>4.</td><td>One pixel is 16 bites of YU or YV(YUV format):<br>
				The bytes are organized as follwing(each sample below is 8 bit):<br>
				Y0 U0 Y1 V0 Y2 U2 Y3 V2<br>
			<tr><td valign=top>5.</td><td>Jpeg stream is located at psJpgInfo->pbyJpg<br></td></tr>
			<tr><td valign=top>6.</td><td>Original(maximum) size of the psJpgInfo->pbyJpg buffer is stored in psJpgInfo->uiMaxJpgSize<br></td></tr>
			<tr><td valign=top>7.</td><td>Actual Jpeg stream size is stored in *psJpgInfo->puiActJpgSize 
				and can be changed up to the maximum of psJpgInfo->uiMaxJpgSize<br></td></tr>
			<tr><td valign=top>8.</td><td>Jpeg stream index is stored in *psJpgInfo->uiJpgIdx.<br>
				In case of the last frame, *psJpgInfo->blLastFrameFlag is set at TRUE.<br></td></tr>
			<tr><td valign=top>9.</td><td>The YUV data is provided for reference only, 
				since it is not in use anymore. 
				The buffer and the data will be freed right after return of this function.<br></td></tr>
@param[in]	encode_id	: Encode instance ID.<br>
@param[in]	mani_no		: this number means each image processing progress timing.<br>
							For more detailed, please refer to @ref fj_still_callback_timing_chart.
@param[in]	psJpgInfo	: A structure containing information about the jpeg stream and YUV  data frame
@retval		0 : Return value currently has no meaning. Must by zero(0).
*/
UINT32 FJ_HostStillJpgManipulation( UCHAR encode_id, FJ_JPG_MANI_STILL mani_no, PSJPGINFO psJpgInfo );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This Call Back function is called in Still sequence BEFORE the next still in the sequence
@param[in]	sen_id		: SEN instance ID.<br>
@param[in]	still_index	: Index of next still in sequence in range of [0..Num of Captures-1]
@retval		TRUE  : continue Still Sequence.
@retval		FALSE : stop Still Sequence.
*/
BOOL FJ_HostStillSequenceCB( UCHAR sen_id, UINT32 still_index );

// --- REMOVE_PANO BEGIN ---
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This API is set a information on YUV420 to Host.
@remarks	1.First line of YUV 4:1:1 data begins in psInfo->pbyBuffer.<br>
			2.The stating address of line N of raw data:<br>
			  psInfo->pbyBuffer + N * psInfo->uiLineJumpInBytes<br>
			3.More available info on the captured image:<br>
			  psInfo->uiSizeX - Number of pixels per image line.<br>
			  psInfo->uiSizeY - Number of image lines.<br>
			4.One pixel is 16bits of YU or YV(YUV420 format):<br>
			  The bytes are organized as following(each sample below is 8bit):<br>
@param[in]	psInfo A structure containing information about the YUV 4:1:1 data
				  frame that may be accessed(exposure, gain, frame size, etc.)
@retval		TRUE  : if data was changed.
@retval		FALSE : if data was not changed.
*/
BOOL FJ_HostPanoramaManipulation( PSPANOINFO psInfo );
// --- REMOVE_PANO END ---

// @cond
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This CallBack function is called when the timing changes from exposure of Still processing to Readout. 
Inside this CallBack, it assumes that FJ_SgStart() is Called.
@param[in]	sen_id	: SEN instance ID.<br>
retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostStillExposureEndCB( UCHAR sen_id );
// @endcond

// @cond
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This CallBack function is Readout of Still processing, and whenever Readout of each Field is completed, it is Called. 
@param[in]	sen_id		: SEN instance ID.<br>
@param[in]	field_num	: 0:dummy frame  1, 2, ..., 254:readout end field num 255:read out complete.
@retval		TRUE  : Success
@retval		FALSE : Error
@remarks	255 (complete) is set to field_num after Readout completion.
*/
BOOL FJ_HostStillReadCB( UCHAR sen_id, UCHAR field_num );
// @endcond

// @cond
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This Call Back function is called after "FJ_HostStillYUVManipulation(4th)" when HCE want to wait User Confirm.
*/
VOID FJ_Host_UserConfirmCB( VOID );
// @endcond

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This call back function is called before exposure starts.<br>
@param[in]	sensor_id : sensor instance id (to know which sensor instance is this called).
@param[in]	type : 0:Video, 1:Still, 2:Dark.
@param[in]	frame_no : frame number
@retval	wait vd count
*/
USHORT FJ_HostExposureCB( UCHAR sensor_id, UCHAR type, ULLONG frame_no );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This CallBack function is called in boot only once.<br>
Sensor could be initialized regarding HW.<br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostSensorInitCB( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This CallBack function is called when Video mode starts.<br>
Sensor should be initialized regarding SW, or should be resumed from standby/sleep.<br>
@param[in]	sensor_id	: Sensor instance ID.<br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostSensorStartCB( UCHAR sensor_id );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This CallBack function called when Sensor should be standby.<br>
@param[in]	sensor_id	: Sensor instance ID.<br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostSensorStopCB( UCHAR sensor_id );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This CallBack function called when Sensor mode should be changed.<br>
@param[in]	sensor_id	: Sensor instance ID.<br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostSensorModeChangeCB( UCHAR sensor_id );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This CallBack function is called when Sensor mode change is finished and 1st Video image set to buffer.<br>
@param[in]	sensor_id	: Sensor instance ID.<br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostSensorModeChangeCompleteCB( UCHAR sensor_id );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This Call Back function is called when Sensor mode will be changed.<br>
Host sets 3A window for next Sensor mode.<br>
@param[in]	sro_id		: SRO instance ID.<br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_Host3AWindowConfigCB( UCHAR sro_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
The CallBack function for the Image Quality parameter setup for video record.<br>
@param[in]	sen_id		: SEN instance ID.<br>
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[in]	frame_no	: Number of readout frame
@remarks	Please be sure to set up Image Quality parameter within this CallBack. 
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostVideoIqParamUpdateCB( UCHAR sen_id, UCHAR demosaic_id, ULLONG frame_no );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
The CallBack function for the Image Quality parameter setup for still.<br>
@param[in]	sen_id		: SEN instance ID.<br>
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[in]	frame_no	: Number of readout frame
@remarks	Please be sure to set up Image Quality parameter within this CallBack. 
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostStillIqParamUpdateCB( UCHAR sen_id, UCHAR demosaic_id, ULLONG frame_no );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This CallBack function is Called by interruption of VD. <br>
Mounting of the function controlled by timing of VD is assumed. 
@param[in]	sensor_id	: Sensor instance ID.<br>
@param[in]	frame_no	: Number of readout frame
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostVDCB( UCHAR sensor_id, ULLONG frame_no );

// --- REMOVE_EIS BEGIN ---
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This callback function read gyro data.
@param[in]	sensor_id	: Sensor instance ID.<br>
@param[in]	frame_no	: Number of readout frame
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostEISReadGyroCB( UCHAR sensor_id, ULLONG frame_no );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This callback function get exposure time of the frame.
@param[in]	sensor_id		: Sensor instance ID.<br>
@param[in]	frame_no		: Number of readout frame
@param[out]	exposure_time	: Exposure time in microsec
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostEISGetExposureTimeCB( UCHAR sensor_id, ULLONG frame_no, UINT32* exposure_time );
// --- REMOVE_EIS END ---

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This callback function get gyro output data.
@param[in]	frame_no	: Number of readout frame
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostEISGetGyroOutputCB( ULLONG frame_no );

// --- REMOVE_AHIP BEGIN ---
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This callback function is called in interruption handler of AHIP.
@remarks	Please be sure to call AHIP IP(interruption handler) within this callback. 
*/
VOID FJ_HostAHIPIntCB( VOID );
// --- REMOVE_AHIP END ---

// @cond
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function set LPF parameter.
@param[in]	frame_no : Frame number.
@param[in]	stream_idx : Stream number.
@param[in]	img_info : Input image information. (ex. width, lines, etc...).
@param[out]	kernel_param : LPF setting parameter.
@retval		TRUE  : LPF is enable.
@retval		FALSE : LPF is disable.
*/
BOOL FJ_HostPreLpfCB( ULLONG frame_no, UINT32 stream_idx, FJ_IMG_INFO* img_info, FJ_IMG_SPF_KERNEL* kernel_param );

// @endcond
/* @} */	// fj_still_callback group


///////////////////////////////////////////////////////////////////////////////
// 3A(AE)
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_ae_callback
@{*/

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
In Video mode, This Callback function is called whenever Base FW meets the 'e' slot in 
statistics sequence, specified by FJ_SetStatisticPeriod().<br>
Host FW can get AE detection data from argument of AeDataTopAddr.
@param[in]	aeawb_id						: SRO instance ID.<br>
@param[in]	cmn_info					: 3A Common Information. See @ref AAA_Common_Info
@param[in]	AeDataTopAddr				: Address for AE statistics data
@param[in]	AeWinBlockColorPixelCount	: Number of pixels in one AE statistics block x 0.25
@param[in]	frame_no : Number of readout frame.
@retval		TRUE : Success
@retval		FALSE : Error
@remarks	Fig) The string image of AE Data:<br>
			@image html fj_ae_callback_data.png Fig) String image of AE Data
@code
			//Sample code
			BOOL FJ_HostVideoAECB(  UCHAR				aeawb_id,
									AAA_Common_Info*	cmn_info,
									ULONG*				AeDataTopAddr,
									ULONG				AeWinBlockColorPixelCount,
									ULLONG				frame_no )
			{
				//User Function
				HostIf_AE_SetData( AeDataTopAddr, cmn_info->AeWin_Block_NumV, cmn_info->AeWin_Block_NumH );
				return TRUE;

			}
			
			VOID HostIf_AE_SetData( ULONG* AeDataTopAddr, USHORT AEBlockV, USHORT AEBlockH )
			{
				UCHAR idx_v, idx_h;

				for( idx_v = 0, idx_v < AEBlockV; idx_v++ ) {
					for( idx_h = 0, idx_h < AEBlockH; idx_h++ ) {
						AECalcMain( AeDataTopAddr[ idx_v][ idx_h] );//User Function
					}
				}
			}
@endcode

*/
BOOL FJ_HostVideoAECB(  UCHAR				aeawb_id,
						AAA_Common_Info*	cmn_info,
						ULONG*				AeDataTopAddr,
						ULONG				AeWinBlockColorPixelCount,
						ULLONG				frame_no );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
In Still mode, This Callback function is called whenever Base FW meets the 'e' slot in 
statistics sequence, specified by FJ_SetStatisticPeriod().<br>
Host FW can get AE detection data from argument of AeDataTopAddr.
@param[in]	aeawb_id						: SRO instance ID.<br>
@param[in]	cmn_info					: 3A Common Information. See @ref AAA_Common_Info
@param[in]	AeDataTopAddr				: Address for AE statistics data
@param[in]	AeWinBlockColorPixelCount	: Number of pixels in one AE statistics block x 0.25
@param[in]	frame_no					: Number of readout frame.
@retval		TRUE : Success
@retval		FALSE : Error
@remarks	Please refer to FJ_HostVideoAECB(). 
*/
BOOL FJ_HostStillAECB(  UCHAR				aeawb_id,
						AAA_Common_Info*	cmn_info,
						ULONG*				AeDataTopAddr,
						ULONG				AeWinBlockColorPixelCount,
						ULLONG				frame_no );

/* @} --- end of fj_ae_callback */


///////////////////////////////////////////////////////////////////////////////
// 3A(AWB)
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_awb_callback
@{*/

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
In Video mode, This Callback function is called whenever Base FW meets the 'w' slot in 
statistics sequence, specified by FJ_SetStatisticPeriod().<br>
Host FW can get AWB detection data from argument of AwbDataTopAddr.
@param[in]	aeawb_id			: SRO instance ID.<br>
@param[in]	cmn_info		: 3A Common Information
@param[in]	AwbDataTopAddr	: Address for AWB statistics data
@param[in]	frame_no		: Number of readout frame.
@retval		TRUE : Success
@retval		FALSE : Error
@remarks	Fig) The string image of AWB Data:<br>
			@image html fj_awb_callback_data.png Fig) String image of AWB Data
@code
			//Sample code
			BOOL FJ_HostVideoAWBCB(  UCHAR				aeawb_id,
									 AAA_Common_Info*	cmn_info,
									 ULONG*				AwbDataTopAddr,
									 ULLONG				frame_no )
			{
				//User Function
				HostIf_AWB_SetData( AwdDataTopAddr, cmn_info->AeWin_Block_NumV, cmn_info->AeWin_Block_NumH );
				return TRUE;

			}

			typedef struct {
				ULONG gbcnt;
				ULONG r;
				ULONG gr;
				ULONG b;
			} AWBDataFormat;

			typedef struct {
				AWBDataFormat data[64][64];
			} AwbDataInfo;

			VOID HostIf_AWB_SetData( ULONG* AwbDataTopAddr, USHORT AEBlockV, USHORT AEBlockH )
			{
				UCHAR idx_v, idx_h;
				AwbDataInfo* AwbInfo;
				
				AwbInfo = (AwbDataInfo*)AwbDataTopAddr;

				if( dataformat == 0 ) { // AWB Cnt/R/G/B
					for( idx_v = 0, idx_v < AEBlockV; idx_v++ ) {
						for( idx_h = 0, idx_h < AEBlockH; idx_h++ ) {

							AWBSetCount( AwbInfo->data[ idx_v][ idx_h].gbcnt );//User Function

							AWBCalcMain( AwbInfo->data[ idx_v][ idx_h].r,
										 AwbInfo->data[ idx_v][ idx_h].gr,
										 AwbInfo->data[ idx_v][ idx_h].b );//User Function
						}
					}
				}
				else {	// GB/R/GR/B
					ULONG work_g;
					for( idx_v = 0, idx_v < AEBlockV; idx_v++ ) {
						for( idx_h = 0, idx_h < AEBlockH; idx_h++ ) {
							work_g = ( AwbInfo->data[ idx_v][ idx_h].gbcnt +
									   AwbInfo->data[ idx_v][ idx_h].gr ) >> 1;

							AWBCalcMain( AwbInfo->data[ idx_v][ idx_h].r,
										 work_g,
										 AwbInfo->data[ idx_v][ idx_h].b );//User Function
						}
					}
				}
			}
@endcode
*/
BOOL FJ_HostVideoAWBCB(  UCHAR				aeawb_id,
						 AAA_Common_Info*	cmn_info,
						 ULONG*				AwbDataTopAddr,
						 ULLONG				frame_no );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
In Still mode, This Callback function is called whenever Base FW meets the 'w' slot in 
statistics sequence, specified by FJ_SetStatisticPeriod().<br>
Host FW can get AWB detection data from argument of AwbDataTopAddr.
@param[in]	aeawb_id			: SRO instance ID.<br>
@param[in]	cmn_info		: 3A Common Information
@param[in]	AwbDataTopAddr	: Address for AWB statistics data
@param[in]	frame_no		: Number of readout frame.
@retval		TRUE : Success
@retval		FALSE : Error
@remarks	Please refer to FJ_HostVideoAWBCB(). 
*/
BOOL FJ_HostStillAWBCB(  UCHAR				aeawb_id,
						 AAA_Common_Info*	cmn_info,
						 ULONG*				AwbDataTopAddr,
						 ULLONG				frame_no );
/* @} */	// fj_awb_callback group


///////////////////////////////////////////////////////////////////////////////
// 3A(AF)
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_af_callback
@{*/

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This Callback function is called whenever Base FW meets the 'f' slot in 
statistics sequence, specified by FJ_SetStatisticPeriod().<br>
Host FW can use this callback to synchronize the Auto Focus processing.
@param[in]	af_id		: SRO instance ID.<br>
@param[in]	cmn_info	: See @ref AAA_Common_Info
@param[in]	frame_no	: Number of readout frame.
@retval		TRUE : Success
@retval		FALSE : Error
*/
BOOL FJ_HostAFCB( UCHAR				af_id,
				  AAA_Common_Info*	cmn_info,
				  ULLONG			frame_no );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This Callback function is called when AF statistics is finished.<br>
Host FW can get AF statistic data using each @ref fj_af_api get-API's.
@param[in]	af_id		: SRO instance ID.<br>
@param[in]	frame_no	: Number of readout frame.
@retval		TRUE : Success
@retval		FALSE : Error
*/
BOOL FJ_HostAFDataReadyCB( UCHAR af_id, ULLONG frame_no );

/* @} */	// fj_af_callback group


///////////////////////////////////////////////////////////////////////////////
// 3A(FLCK)
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_flck_callback
@{*/

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This Callback function is called whenever Base FW meets the 'l' slot in 
statistics sequence, specified by FJ_SetStatisticPeriod().<br>
Host FW can get FLCK detection data from argument of FlckDataTopAddr.
@param[in]	flck_id			: SRO instance ID.<br>
@param[in]	cmn_info		: 3A Common Information. See @ref AAA_Common_Info
@param[in]	FlckDataTopAddr	: Address for Flicker statistics data
@param[in]	frame_no		: Number of readout frame.
@retval		TRUE : Success
@retval		FALSE : Error
@remarks	Fig) The string image of Flicker Data:<br>
			@image html fj_flck_callback_data.png Fig) String image of Flicker Data
@code
			//Sample code
			BOOL FJ_HostFLCKCB( UCHAR				flck_id,
								AAA_Common_Info*	cmn_info,
								ULONG*				FlckDataTopAddr,
								ULLONG				frame_no )
			{
				//User Function
				HostIf_FLCK_SetData( FlckDataTopAddr, cmn_info->AeWin_Block_NumV );
				return TRUE;

			}

			VOID HostIf_FLCK_SetData( ULONG* FlckDataTopAddr, USHORT FLCKBlockV )
			{
				UCHAR idx_v, idx_h;

				if( flckwdmode == 0 ) {
					FLCKBlockH = 8;
				}
				else {
					FLCKBlockH = 4;
				}

				for( idx_v = 0, idx_v < FLCKBlockV; idx_v++ ) {
					for( idx_h = 0, idx_h < FLCKBlockH; idx_h++ ) {
						FLCKCalcMain( FlckDataTopAddr[ idx_v][ idx_h] );//User Function
					}
				}
			}

@endcode

*/
BOOL FJ_HostFLCKCB(UCHAR			flck_id,
				   AAA_Common_Info*	cmn_info,
				   ULONG*			FlckDataTopAddr,
				   ULLONG			frame_no );

/* @} --- end of fj_flck_callback */

///////////////////////////////////////////////////////////////////////////////
// STAT
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_stat_callback
@{*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
In video mode, this callback function is called when STAT configuration becomes OK to change.<br>
@param[in]	inst_id	: SEN or Demosaic ID depend on STAT location configuration<br>
*/
VOID FJ_HostVideoStatParamUpdateCB( UCHAR inst_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
In burst/continuous capture mode, this callback function is called when STAT configuration becomes OK to change.<br>
@param[in]	inst_id	: SEN or Demosaic ID depend on STAT location configuration<br>
*/
VOID FJ_HostStillStatParamUpdateCB( UCHAR inst_id );

/* @} --- end of fj_stat_callback */

///////////////////////////////////////////////////////////////////////////////
// Image Quality
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_iq_callback
@{*/

/**
 * Parameter structure of FJ_HostNoticeChgToneGammaCB().
 */
typedef struct {
	UCHAR tone_table_resol;			/**< Tone table resolution */
	UCHAR gamma_enable;				/**< Gamma function enable/disable */
	const USHORT* tone_tbl;			/**< Tone table */
	USHORT tone_tbl_end_point;		/**< Tone table end point */
	const USHORT* gamma_tbl_full;	/**< Gamma table full */
	const ULLONG* gamma_tbl_diff;	/**< Gamma table diff */
} T_FJ_HOST_TONE_GAMMA;

/**
This function will be called during replace Tone table and Gamma table.
@param [in]	demosaic_id			Demosaic instance ID.<br>
@param [in] tone_gamma_param	New Tone and Gamma parameter. Do not change this parameter.
*/
extern VOID FJ_HostNoticeChgToneGammaCB( UCHAR demosaic_id, T_FJ_HOST_TONE_GAMMA* tone_gamma_param );

/*@} --- end of fj_iq_callback */

///////////////////////////////////////////////////////////////////////////////
// PlayBack
///////////////////////////////////////////////////////////////////////////////
// @cond
/** @weakgroup fj_playback_callback
@{
	- @ref FJ_Host_NoticeProgressCB
	- @ref FJ_Host_ThumbnailIndexCB
	<hr>
*/

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function get progress number of FileOperation (copy/delete/protect) functions.<br>
@param [out] cur_progress_num	currect progress number (number of files)<br>
@param [out] total_num			total file number for operation (number of files)<br>
@retval		TRUE : Success
@retval		FALSE : Error
@remarks each arguments value are number of files  (not percentage)
*/
BOOL FJ_Host_NoticeProgressCB( USHORT cur_progress_num, USHORT total_num );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function get thumbnail error param of thumbnailindex function.<br>
@param [in] thumb_info			Pointer of array that entered Thumbnail information<br>
@param [in] thumb_total_count	total of thumbnail under display<br>
@retval		TRUE : Success
@retval		FALSE : Error
@remarks The information of Thumbnail enters like the following order in example.<br>
         ThumbnailIndex example)<br>
   - Thumbnail Index 3x3: <br>
             @image html fj_playback_thumbindex_callback_3x3.png
   - Thumbnail Index 4x4: <br>
             @image html fj_playback_thumbindex_callback_4x4.png
*/
BOOL FJ_Host_ThumbnailIndexCB( FJ_T_PLAYBACK_THUMBNAILINDEX_INFO* thumb_info, UCHAR thumb_total_count );

/* @} */	// fj_playback_callback group
// @endcond

///////////////////////////////////////////////////////////////////////////////
// Encoder
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_encoder_callback
@{*/

//@cond
/**
This function is invoked each time the TopImage is play-backed, and before it is actually displayed. <br>
And HOST secure a buffer getting MP4 "free atom" and set address to byBuffer.
@param [out]  byBuffer	The address of buffer to get the "free atom"<br>
@param [in] Size		The size of free atom in MP4 file<br>
                        If Size is 0, there is not the free atom in a MP4 file<br>
@retval		TRUE  : Set byBuffer (When HOST needs free atom)
@retval		FALSE : Not Set byBuffer (When HOST not needs free atom)

*/
BOOL FJ_Host_NotifyFreeAtomCB( ULONG* byBuffer, ULONG Size );

/**
This function is invoked each time the TopImage is play-backed, and before it is actually displayed. <br>
And HOST secure a buffer getting MP4 "udta atom" and set address to byBuffer.
@param [out]  byBuffer	The address of buffer to get the "udta atom"<br>
@param [in] Size		The size of udta atom in MP4 file<br>
                        If Size is 0, there is not the udta atom in a MP4 file<br>
@retval		TRUE  : Set byBuffer (When HOST needs udta atom)
@retval		FALSE : Not Set byBuffer (When HOST not needs udta atom)

*/
BOOL FJ_Host_NotifyUdtaAtomCB( ULONG* byBuffer, ULONG Size );

/**
This function is invoked each time the TopImage is play-backed, and before it is actually displayed. <br>
And HOST edit MP4 "udta atom" to byBuffer.
@param [out]  byBuffer	The address of buffer to get the "udta atom"<br>
@param [in] Size		The size of udta atom in MP4 file<br>
                        If Size is 0, there is not the udta atom in a MP4 file<br>
@retval		TRUE  : Set byBuffer (When HOST needs udta atom)
@retval		FALSE : Not Set byBuffer (When HOST not needs udta atom)

*/
BOOL FJ_Host_NotifyUdtaAtomEditCB( ULONG* byBuffer, ULONG* Size );
//@endcond

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function is notified at the time of Encode start. <br>
Please Host start the Audio at this timing for AV synchronization.<br>
And, If make a THM of 1frame, please Host execute the THM generate request.
@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_Host_VideoEncoderStartCB( UCHAR video_enc_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set H264/H.265 video stream information at the beginning of the GOP. <br>
If there is a dynamic change of Codec parameters, please called FJ_VideoEncoder_ConfigDynamic() in this CallBack.
@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_Host_EncoderSequenceStartCB( UCHAR video_enc_id, INT32 *vop_cnt );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function is notified at the Repeat Frame has occurred. (The API is for Debug.)<br>
@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@retval		TRUE  : Success
@retval		FALSE : Error
*/

BOOL FJ_Host_EncoderRepeatFrameCB(UCHAR video_enc_id);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get H264 video stream vector/brightness information. <br>
This Callback will be notified prior to the encoding completion of every frame.

	@image html brightness.png
	<b>brightness information format (24Byte)</b><br>
	This image is 1MacroBlock.<br>
	If Image Size is FHD, MacroBlock is 8160 of 1frame.<br><br>
	
	@image html vector.png
	<b>vector information format (8Byte)</b><br>
	This image is 1MacroBlock.<br>
	If Image Size is FHD, MacroBlock is 8160 of 1frame.<br>
	MBMV0 is MB perpendicular vector (L0) <br>
	MBMH0 is MB horizontality vector (L0) <br>
	MBMV1 is MB perpendicular vector (L1) <br>
	MBMH1 is MB horizontality vector (L1) <br> <br>
	
	IDR, I, P picture : MBMV0, MBMH0 is valid.<br>
	B pictre : MBMV0, MBMH0, MBMV1, MBMH1 is valid.<br>
	* First IDR picture is invalid this information.<br>
	
	- MBMHx <br>
	  Bit5     : Sign Bit<br>
	  Bit4..0  : Vector value<br>
	- MBMVx <br>
	  Bit5     : Sign Bit<br>
	  Bit4..1  : Vector value<br>
	  Bit0     : Parity (0:same / 1:different)<br>

	The Vector value is the complement on two.<br>

@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@param [in]  info				Get video stream vector information<br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_Host_H264VectorCB( UCHAR video_enc_id, T_FJ_MOVIE_H264_VECTOR_PARAM* info );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get H.264 video stream information. <br>
This function is called at every encoded frame.

	@image html StreamFormat_H264.png
	<b><center>H264 Stream format</center></b><br>

@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@param [in]  info				Get video stream information.<br>
@retval		TURE  : Success
@retval		FALSE : Error
@remarks	FJ_VideoEncoder_FreeIndex function must be called in after this function.
*/
BOOL FJ_Host_H264StreamCB( UCHAR video_enc_id, T_FJ_MOVIE_H264_STREAM_PARAM* info );

// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get H265 video stream vector. <br>
This Callback will be notified to the encoding completion of every frame.

	@image html ctu.png
	<b><center>Sub CTU (CU32)</center></b><br>
	HEVC codec output (CTU in Picture)*4*32 byte Data.
	
	@image html vector_hevc.png
	<b><center>vector information format</center></b><br>
	data[127:0] = {reserve(all 0), CU16_0_mv_data, CU16_1_mv_data}<br>
	data[127:0] = {reserve(all 0), CU16_2_mv_data, CU16_3_mv_data}<br>
	
	@image html vector_hevc2.png
	<b><center>detail of CU16_x_MV_DATA</center></b><br>
	CU16_x_mv_data[49:0] = {CP16_PRED_FLAG_L0, L0_MVX, L0_MVY, CU16_PRED_FLAG_L1, L1_MVX, L1_MVY}<br><br>
	
	CP16_PRED_FLAG_L0 : 1bit predition flag for the reference list0 information<br>
	L0_MVX: mvx for the reference list0<br>
	L0_MVY: mvy for the reference list0<br>
	CU16_PRED_FLAG_L1 : 1bit prediction flag for the reference list1 information<br>
	L1_MVX: mvx for the reference list1<br>
	L1_MVY: mvy for the reference list1<br>
	
@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@param [in]  info				Get video stream vector information<br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_Host_H265VectorCB( UCHAR video_enc_id, T_FJ_MOVIE_H265_VECTOR_PARAM* info );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get H.265 video stream information. <br>
This callback is called at every encoded frame.

// --- REMOVE_ES1_HARDWARE BEGIN ---
	@image html StreamFormat_H265.png
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	@image html StreamFormat_H264.png
// --- REMOVE_ES3_HARDWARE END ---

	<b><center>H265 Stream format</center></b><br>

@param[in]	video_enc_id	Encode instance ID.<br>
							Based on this ID, it will identify the Stream.
@param [in]  info			Get video stream information<br>
@retval		TURE  : Success
@retval		FALSE : Error
@remarks	FJ_VideoEncoder_FreeIndex function must be called in after this function.
*/
BOOL FJ_Host_H265StreamCB( UCHAR video_enc_id, T_FJ_MOVIE_H265_STREAM_PARAM* info );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get PCM stream information, and set PCM stream information if had been handled. <br>

@param [in]  audio_enc_id		Audio Encode ID. Based on this ID, it will identify the Stream.<br>
@param [in]  info				Get audio stream information<br>
@retval		TURE  : Success
@retval		FALSE : Error
@remarks	FJ_Audio_FreeIndexPCM function must be called in this function.
*/
BOOL FJ_Host_PCMStreamCB( UCHAR audio_enc_id, T_FJ_AUDIO_PCM_STREAM_PARAM* info);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get AAC stream information, and set AAC stream information if had been handled. <br>

	@image html StreamFormat_Rec.png
	<b><center>Audio Stream format</center></b><br>

@param [in]  audio_enc_id		Audio Encode ID. Based on this ID, it will identify the Stream.<br>
@param [in]  info				Get audio stream information<br>
@retval		TURE  : Success
@retval		FALSE : Error
@remarks	FJ_AudioEncoder_FreeIndex function must be called in this function.
*/
BOOL FJ_Host_AACStreamCB( UCHAR audio_enc_id, T_FJ_AUDIO_AAC_STREAM_PARAM* info);

/*@} --- end of fj_encoder_callback */

///////////////////////////////////////////////////////////////////////////////
// Decoder
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_decoder_callback
@{*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API is called when Video Stream is required. <br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_Host_VideoPlayback_GetVideoStreamCB( E_FJ_VIDEO_DEC_ENUM codec );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API is called when Audio Stream is required. <br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_Host_VideoPlayback_GetAudioStreamCB( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API is called when the first frame has been decode and display. <br>
@param [in]  decoder_status		State of the decoder. (idle/normal)<br>
@param [in]  display_pts		PTS of display frame.<br>
@param [in]  error				Whether an error has occurred.<br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_Host_VideoPlayback_FirstFrameDisplayCB( E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG display_pts, BOOL error);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API is called when the Video has been decode and display. <br>
@param [in]  decoder_status		State of the decoder. (idle/normal)<br>
@param [in]  display_pts		PTS of display frame.<br>
@param [in]  error				Whether an error has occurred.<br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_Host_VideoPlayback_VideoDecodeCB( E_FJ_VIDEO_DEC_ENUM codec, E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG display_pts, BOOL error);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API is called when the Audio has been decode. <br>
@param [in]  decoder_status		State of the decoder. (idle/normal)<br>
@param [in]  decoded_pts		The number of samples of the decoded PCM.<br>
								For example)<br>
								When sampling rate is 48kHz, decoded_pts at the time of decoding the data of one second is 48000.<br>
@param [in]  error				Whether an error has occurred.<br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_Host_VideoPlayback_AudioDecodeCB( E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG decoded_pts, BOOL error);

/*@} --- end of fj_decoder_callback */

///////////////////////////////////////////////////////////////////////////////
// Audio
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_audio_callback
@{
	This chapter describes API callback timing chart about audio.<br>
	For more detailed of callback APIs, please refer to description of each callback APIs.

	<hr>
	@section fj_audio_callback_timing_chart	Host Callback API Timing chart
	@subsection fj_audio_callback_timing_chart Audio Callback API Timing chart
	@image html fj_audio_callback_timing_chart.png
*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This callback function called, when the pcm data was captured.<br>
The BaseFw expects to receive the processing result.<br>
@param[in]	audio_capture_id	AudioCapture Instance ID.<br>
@param[in]	param  pcm data informations.
@remarks
This function is used in the case of special processing for the audio data.<br>
During audio capture processing, this function is called whenever 1024 pcm samples are captured.<br>
*/
VOID FJ_Host_AudioCapturePlugIn0CB( UCHAR audio_capture_id, T_FJ_AUDIO_CAP_PLUGIN0_CB_PARAM * param );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This callback function called, when the pcm data was captured.<br>
The BaseFw does not receive the processing result.<br>
@param[in]	audio_capture_id	AudioCapture Instance ID.<br>
@param[in]	param  pcm data informations.
@remarks
This function is used in the case of special processing for the audio data.<br>
During audio capture processing, this function is called whenever 1024 pcm samples are captured.<br>
*/
VOID FJ_Host_AudioCapturePlugIn1CB( UCHAR audio_capture_id, T_FJ_AUDIO_CAP_PLUGIN1_CB_PARAM * param );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This callback function called, when the pcm data was decoded.<br>
The BaseFw expects to receive the processing result.<br>
@param[in]	audio_out_id	AudioOut Instance ID.<br>
@param[in]	param  pcm data informations.
@remarks
This function is used in the case of special processing for the audio data.<br>
During audio decoder processing, this function is called whenever 1024 pcm samples are decoded.<br>
*/
VOID FJ_Host_AudioOutPlugInCB( UCHAR audio_out_id, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM * param );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This callback function called, when the WAV output operation is stoped.<br>
@param[in]	audio_out_id	AudioOut Instance ID.<br>
@param[in]	stop_factor  	stop factor.<br>
							FJ_AUDIO_CB_OUT_STOP: when FJ_AudioOut_DirectStopWav() is called.<br>
							FJ_AUDIO_CB_OUT_AUTO_STOP: when WAV output operation is completed.<br>
*/
VOID FJ_Host_AudioOut_StopWavCB( UCHAR audio_out_id, UCHAR stop_factor );

/*@} --- end of fj_audio_callback */

///////////////////////////////////////////////////////////////////////////////
// Media
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_media_callback
@{*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API is called in CMD11, notify the timming of changing voltage. <br>
@param[in]	ch	SD Channel number. (0 or 1)<br>
@param[in]	voltage	SD signal voltage. (0:0.0V, 1:1.8V, 2:3.3V)<br>
@retval		TRUE  : voltage is changed
@retval		FALSE : voltage is NOT changed
*/
BOOL FJ_HostSDChangeVoltageCB( UCHAR ch, UCHAR voltage );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This callback function control the power supply of the SD card.
@param[in]	ch	SD Channel number. (0 or 1)<br>
@param[in]	power	SD power supply. (0:turned off, 1:turned on)<br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostSDPowerSupplyCB( UCHAR ch, UCHAR power );

/*@} --- end of fj_media_callback */

///////////////////////////////////////////////////////////////////////////////
// Mux
///////////////////////////////////////////////////////////////////////////////
// @cond
/** @weakgroup fj_mux_callback
@{*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API is called when Audio Stream Can Free. (After AddSample)<br>
@param[in]	aud_id			Audio ID.<br>
@param[in]	free_index		Free Audio Index.<br>
@param[in]	last_free_flg  	Stream All AddSample Complete Flag.<br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_Host_Mux_AudioFreeRequestCB( UCHAR aud_id, UINT32 free_index, BOOL last_free_flg );

// --- REMOVE_CAMERAIF BEGIN ---
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This callback function called, video add sample.<br>
@param[in]	code	Event code.<br>
					<ul><li>0:OK
						<li>1:Stop by 4GB limitation
						<li>2:Stop by 2hours limitation
						<li>3:Stop by Media full</ul>
*/
VOID FJ_Host_Mux_VideoAddSampleCB( UINT32 code );
// --- REMOVE_CAMERAIF END ---

/*@} --- end of fj_mux_callback */
// @endcond

///////////////////////////////////////////////////////////////////////////////
// Demux
///////////////////////////////////////////////////////////////////////////////
// @cond
/** @weakgroup fj_demux_callback
@{*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API is called when Video Stream Read Complete. <br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_Host_Demux_StreamReadCompleteCB( UINT32 file_id, E_FJ_DEMUX_CODEC codec, T_FJ_DEMUX_STREAM_PARAM* stream_param );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This API is called when want a stream area. <br>
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_Host_Demux_GetStreamAreaCB( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32* stream_size, UINT32* stream_address );

/*@} --- end of fj_demux_callback */
// @endcond

///////////////////////////////////////////////////////////////////////////////
// File System
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_filesystem_callback
@{*/
/*@} --- end of fj_filesystem_callback */

///////////////////////////////////////////////////////////////////////////////
// MEDIA
///////////////////////////////////////////////////////////////////////////////
// @cond
/** @weakgroup fj_media_callback
@{*/
/**
Through this function, Host can set a callback function that will be called when the NAND TL identifies that the number of bad blocks (units) in one zone is too high. <BR>
The parameter is the same as the error status that can be obtained by PRMID_TL_ERROR_STATUS, masked with the current error flag.
@param[in]	ulParam	PRMID_TL_TOO_MANY_BAD_BLOCKS_CALLBACK_FUNC
@retval		TRUE Normal End
@retval		FALSE Abnormal End
@remarks	Nothing Special.
*/
//BOOL	HceCbFuncTooManyBadBlocks( ULONG ulParam );
BOOL	FJ_Host_TooManyBadBlocksCB( ULONG ulParam );

/**
Through this PRMID HCE can set a callback function that will be called<br>
when the NAND TL identifies an error in system data (e.g., root entry or FAT).<br>
The parameter is the same as the error status that can be obtained by PRMID_TL_ERROR_STATUS, masked with the current error flag.<br>
@param[in]	ulParam	Parameter ID  (values for FJ_SetParam)<br>
								  PRMID_TL_SYSTEM_DATA_ERROR_CALLBACK_FUNC
@retval		TRUE  Normal End
@retval		FALSE Abnormal End
@remarks	Nothing Special.
*/
//BOOL HceCbSystemDataError( ULONG ulParam );
BOOL FJ_Host_SystemDataErrorCB( ULONG ulParam );
/*@} --- end of fj_media_callback */
// @endcond

///////////////////////////////////////////////////////////////////////////////
// DCF
///////////////////////////////////////////////////////////////////////////////
// @cond
/** @weakgroup fj_dcf_callback
@{*/
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This Call Back function is called just before a DCF object is created.
@param[in]	uiFileType	: The create file type
@retval		TRUE : Success
@retval		FALSE : Error
@remark
Sample sequence inside FJ_HostDcfCreateFileCB
@image html fj_dcf_HostDcfCreateFileCB_sequence.png<br>
<br>
@code
// Sample code (FJ_HostDcfCreateFileCB)

	BOOL FJ_HostDcfCreateFileCB( UINT32 uiFileType )
	{
		INT32 ercd;
		USHORT dirCnt;
		USHORT objCnt;
		USHORT lastDirNo;
		USHORT lastFileNo;
		CHAR dirName[10];
		USHORT dirNo;
		USHORT fileNo;
		USHORT date;
		USHORT time;
		INT32 ImageIndex;
		USHORT current_dir_index;
		USHORT nextDirIndex;
		LONG lastdirnum;
		USHORT dupli_ope_value;

		// check empty of directory
		ercd = FJ_DirectDcfDirNum( &dirCnt );
		if(0 != ercd) {return FALSE;}

		if(0 == dirCnt) {
			// No directory
			// set directory name
			FJ_SetDCFFreeChars(1, FJ_DIR_SET_ONLY, FJ_CUSTOM_DCF_DIR_NAME);
			lastDirNo = 100;
			lastFileNo = 1;
			goto skip1;
		}

		// search object
		for(;;) {
			// check number of objects
			ercd = FJ_DirectDcfDumpDirIAttr( DCF_DB_CURRENT_DIR, &objCnt, dirName, &date, &time );
			if(0 != ercd) {return FALSE;}
			dirNo = (dirName[0]-'0')*100 + (dirName[1]-'0')*10 + (dirName[2]-'0');
			// set directory name
			FJ_SetDCFFreeChars(1, FJ_DIR_SET_ONLY, dirName+3);

			// get last file number
			ercd = FJ_GetDCFLastFileNumber(DCF_DB_CURRENT_DIR, &fileNo);
			if(0 != ercd) {return FALSE;}

			if(0 == fileNo) {
				// No file in current directory
				lastDirNo = dirNo;
				lastFileNo = 1;
				break;
			}

			if((4000 > objCnt) && (9999 > fileNo)) {
				// can make new file in current dir.
				lastDirNo = dirNo;
				lastFileNo = fileNo + 1;
				break;
			}

			// current dir is checked if it is last dir on DB
			ercd = FJ_Get_Latest_Index( &ImageIndex );
			current_dir_index = ImageIndex>>16;
			FJ_GetDCFDBLongVal( FJ_PARA_DCFDB_DIR_GET, DCF_DB_CURRENT_DIR, (LONG*)&ImageIndex );
			if(0 != ercd) {return FALSE;}
			if(dirCnt != current_dir_index) {
				// not last dir.
				// change next dir.  !!Update DB!!
				nextDirIndex = (dirCnt > current_dir_index) ? (current_dir_index+1) : 1;
				ercd = FJ_SetCurrentDirectory(nextDirIndex);
				if(0 != ercd) {return FALSE;}
				continue;
			}

			// check overflow directory
			if(999 <= dirNo) {
				// can NOT find bigger number
				// try first free file number
				ercd = FJ_GetFirstFreeFileNo( &fileNo );
				if(0 != ercd) {return FALSE;}	// FSDCF_ERR_FILE_FULL
				lastDirNo = 999;
				lastFileNo = fileNo;
				break;
			}

			// need new dirctory 
			// set directory name
			FJ_GetParam(FJ_PARA_DCF_DUPLICATE_OPERATION, &dupli_ope_value);

			if(dupli_ope_value == 0){
				FJ_SetDCFFreeChars(1, FJ_DIR_SET_ONLY, FJ_CUSTOM_DCF_DIR_NAME);
				lastDirNo = dirNo + 1;
				lastFileNo = 1;
				break;
			}
			else{

				FJ_SetDCFFreeChars(1, FJ_DIR_SET_ONLY, FJ_CUSTOM_DCF_DIR_NAME);

				// get latest directory number
				ercd = FJ_GetParamLong(FJ_PARA_DCF_LAST_KEY, &lastdirnum);
				if(0 != ercd) {return FALSE;}
				lastdirnum = lastdirnum >> 14;

				// check overflow directory
				if(lastdirnum < 999){
					lastDirNo = lastdirnum + 1;
					lastFileNo = 1;
					break;
				}
				else{
					ercd = FJ_GetFirstFreeFileNo( &fileNo );
					if(0 != ercd) {return FALSE;}	// FSDCF_ERR_FILE_FULL
					lastDirNo = dirNo;
					lastFileNo = fileNo;
					break;
				}
			}
		}

	skip1:
		FJ_SetNextDCFKey(lastDirNo, lastFileNo);

		return TRUE;
	}
@endcode
*/
BOOL	FJ_HostDcfCreateFileCB( UINT32 uiFileType );

//BOOL	FJ_HostDcfCreateFileCB( FW_UCHAR ucfileType );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function will be called after a DCF object is inserted into DCF Db.<br>
Set filtering information for captured image.
@param[in] full_file_path DCF object's file path
@retval TRUE : Success
@retval FALSE : Error
@code
// Sample code (FJ_Host_SetDcfDbFilterTypeCB)

BOOL FJ_Host_SetDcfDbFilterTypeCB()
{
	
	// sample
	// If detect face, set face filter attribute to DCF object.
	FJ_Dcf_Date_Write_Obj_Filter_Attr( full_file_path, FJ_DCF_DATE_SET_FACE_FILTER );
	
	return TRUE;
}
@endcode
*/
BOOL	FJ_Host_SetDcfDbFilterTypeCB(CHAR* full_file_path);

/*@} --- end of fj_dcf_callback */
// @endcond

///////////////////////////////////////////////////////////////////////////////
// EXIF
///////////////////////////////////////////////////////////////////////////////
// @cond
/** @weakgroup fj_exif_callback
@{*/
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function be called before the EXIF information is saved into a file.<br>
And HOST updates EXIF tag.
@retval TRUE : Success
@retval FALSE : Error
@code
// Sample code (FJ_Host_ExifUpdateCB)

BOOL FJ_Host_ExifUpdateCB()
{
	
	// sample
	// Please set necessary EXIF information. 
	// Set CODE is shown about some EXIF information. 
	
	// Title
	FJ_SetImageEXIF( FJ_IFD_0, 0x010E, FJ_TYPE_ASCII, FJ_CUSTOM_EXIF_IMG_DESCRIPTION_S, 0, FJ_CUSTOM_EXIF_IMG_DESCRIPTION );
	
	// Maker
	FJ_SetImageEXIF( FJ_IFD_0, 0x010F, FJ_TYPE_ASCII, FJ_CUSTOM_EXIF_MAKE_S, 0, FJ_CUSTOM_EXIF_MAKE );
	
	// Model
	FJ_SetImageEXIF( FJ_IFD_0, 0x0110, FJ_TYPE_ASCII, FJ_CUSTOM_EXIF_MODEL_S, 0, FJ_CUSTOM_EXIF_MODEL );
	
	// Software
	FJ_SetImageEXIF( FJ_IFD_0, 0x0131, FJ_TYPE_ASCII, FJ_CUSTOM_EXIF_SOFT_S, 0, FJ_CUSTOM_EXIF_SOFT );
	
	// Copylight
	FJ_SetImageEXIF( FJ_IFD_0, 0x8298, FJ_TYPE_ASCII, FJ_CUSTOM_EXIF_COPYRIGHT_S, 0, FJ_CUSTOM_EXIF_COPYRIGHT );
	
	return TRUE;
}
@endcode
*/
BOOL	FJ_Host_ExifUpdateCB( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function be called before the EXIF information is saved into a file.<br>
And HOST create EXIF MakerNote to byBuffer.
@param[in] byBuffer The address of buffer to set the EXIF MakerNote. 
@param[in] Size The size of the buffer to set the EXIF MakerNote.<br>
                The size can set beforehand by followed API.<br>
                FJ_SetParam( FJ_PARA_MAKER_NOTE_SIZE, xxx );
@param[in] Offset The offset of MakerNote tag from the beginning of the TIFF header in a file.
@param[in] ParamArray Array of optional parameters (not use)
@param[in] ParamNum Size of the parameters' array (not use)
@retval 1 : Success
@retval 0 : Error
@retval 0xFFFFFFFF : No Action was taken
@remarks 
Please set length of the EXIF MakerNote to less than the Size.<br>
Do not rewrite the value of the byBuffer and Size and Offset.<br>
Please refer to the sequence of Normal Capture(Q.View On, Media Save On)-3 on @ref fj_still_capture_sequence
@code
// Sample code (FJ_Host_CreateMakerNoteCB)

LONG FJ_Host_CreateMakerNoteCB( CHAR* byBuffer, INT32 Size, INT32 Offset, INT32* ParamArray, INT32 ParamNum )
{
    // sample
	// "Size" can be set beforehand by "FJ_SetParam( FJ_PARA_MAKER_NOTE_SIZE, xxx );"
	memset( byBuffer, 1, Size );
	return 1;		// Success
}
@endcode
*/
LONG	FJ_Host_CreateMakerNoteCB(CHAR* byBuffer, INT32 Size, INT32 Offset, INT32* ParamArray, INT32 ParamNum);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function is invoked each time the image is play-backed, and before it is actually displayed.<br>
And HOST parse EXIF MakerNote.
@param[in] byBuffer Buffer which holds the MakerNote to be parsed
@param[in] Size The size of the buffer
@param[in] Offset The offset of MakerNote tag from the beginning of the TIFF header
@param[in] ByteOrder The EXIF endian
@param[in] ParamArray Array of optional parameters (not use)
@param[in] ParamNum Size of the parameters' array (not use)
@retval 1 : Success
@retval 0 : Error
@retval 0xFFFFFFFF : No Action was taken
*/
LONG	FJ_Host_ParseMakerNoteCB(CHAR* byBuffer, INT32 Size, INT32 Offset, INT32 ByteOrder, INT32* ParamArray, INT32 ParamNum);


/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function be called before the EXIF information is saved into a file.<br>
And HOST updates EXIF tag for class1 image of the multi picture format.
@retval TRUE : Success
@retval FALSE : Error
*/
BOOL FJ_Host_ExifUpdate_Class1_CB( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function be called before the EXIF information is saved into a file.<br>
And HOST updates EXIF tag for class2 image of the multi picture format.
@retval TRUE  : Success
@retval FALSE : Error
*/
BOOL FJ_Host_ExifUpdate_Class2_CB( VOID );


/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function be called before the EXIF information is saved into a file.<br>
And HOST create EXIF MakerNote for class1 image of the multi picture format from byBuffer.
@param[in] byBuffer The address of buffer to set the EXIF MakerNote.
@param[in] Size The size of the buffer to set the EXIF MakerNote.<br>
                Please call following API in FJ_Host_ExifUpdate_Class1_CB() API, and set the size.<br>
                FJ_SetImageEXIF_Class1( FJ_IFD_EXIF, FJ_EXIF_TAG_MAKERNOTE_, FJ_TYPE_UNDEFINED, xxx, 0, 0 );
@param[in] Offset The offset of MakerNote tag from the beginning of the TIFF header in a file.
@param[in] ParamArray Array of optional parameters (not use)
@param[in] ParamNum Size of the parameters' array (not use)
@retval 1 : Success
@retval 0 : Error
@retval 0xFFFFFFFF : No Action was taken
@remarks 
Please set length of the EXIF MakerNote to less than the Size.<br>
Do not rewrite the value of the byBuffer and Size and Offset.<br>
*/
LONG FJ_Host_CreateMakerNote_Class1_CB(CHAR* byBuffer, INT32 Size, INT32 Offset, INT32* ParamArray, INT32 ParamNum);


/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function be called before the EXIF information is saved into a file.<br>
And HOST create EXIF MakerNote for class2 image of the multi picture format from byBuffer.
@param[in] byBuffer The address of buffer to set the EXIF MakerNote.
@param[in] Size The size of the buffer to set the EXIF MakerNote.<br>
                Please call following API in FJ_Host_ExifUpdate_Class2_CB() API, and set the size.<br>
                FJ_SetImageEXIF_Class2( FJ_IFD_EXIF, FJ_EXIF_TAG_MAKERNOTE_, FJ_TYPE_UNDEFINED, xxx, 0, 0 );
@param[in] Offset The offset of MakerNote tag from the beginning of the TIFF header in a file.
@param[in] ParamArray Array of optional parameters (not use)
@param[in] ParamNum Size of the parameters' array (not use)
@retval 1 : Success
@retval 0 : Error
@retval 0xFFFFFFFF : No Action was taken
@remarks 
Please set length of the EXIF MakerNote to less than the Size.<br>
Do not rewrite the value of the byBuffer and Size and Offset.<br>
*/
LONG FJ_Host_CreateMakerNote_Class2_CB(CHAR* byBuffer, INT32 Size, INT32 Offset, INT32* ParamArray, INT32 ParamNum);

/*@} --- end of fj_exif_callback */
// @endcond

///////////////////////////////////////////////////////////////////////////////
// DPOF
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_dpof_callback
@{
@}*//* --- end of fj_dpof_callback */

///////////////////////////////////////////////////////////////////////////////
// UPDATER
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_updater_callback
@{*/

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This callback function for update progress.
@param[in] state State of partition in current run. See @ref E_FJ_UPDATER_PARTITION<br>
@param[in] progress The progress rate of the update(0-100).<br>
*/
VOID FJ_Updater_Progress_CB(E_FJ_UPDATER_PARTITION state, ULONG progress);

VOID FJ_HostDspIPCU0IntCB( VOID );
VOID FJ_HostDspIPCU1IntCB( VOID );
VOID FJ_HostDspIPCU2IntCB( VOID );
VOID FJ_HostDspIPCU3IntCB( VOID );

/*@} --- end of fj_updater_callback */

#endif
