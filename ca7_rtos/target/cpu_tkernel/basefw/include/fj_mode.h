/**
 * @file		fj_mode.h
 * @brief		mode header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_MODE_H
#define _FJ_MODE_H

#include "fj_std.h"
#include "ddim_typedef.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_mode_overview
@{
This chapter describes API related to mode control and query status register.<br>
Concerning the status transition diagram of mode, please refer to the Mode Control of the Modules's @ref fj_overview.<br>
The register expresses the current status of base firmware.<br>
<br>
There are three ways to monitor the completion of mode transition can monitor.
	-#	To monitor by asynchronous message notification.<br>
	-#	To monitor by @ref FJ_WaitModeComp function.<br>
	-#	To monitor by @ref FJ_QueryStatus function.<br>

For the above sequence, refer to the @ref fj_mode_sequence of @ref FJ_SetMode.<br>
For further information of mode transition, refer to the sequence of each features.<br>

@}*//* --- end of fj_mode_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_mode_sequence
@{
	@ref fj_setmode_seq

	<hr>
	@section fj_setmode_seq Mode setting sample sequence
	The completion of mode transition can monitor by asynchronous message notification.<br>
	@image html fj_mode__asynchronous_message.png

	The completion of mode transition can monitor by @ref FJ_WaitModeComp function.<br>
	@image html fj_mode__WaitModeComp.png

	The completion of mode transition can monitor by @ref FJ_QueryStatus function.<br>
	@image html fj_mode__QueryStatus.png

@}*//* --- end of fj_mode_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_mode_definition
@{*/

/** STATUS REGISTER */
/** Sr1 Table value (lower 8bit data) */
#define FJ_SR1_NULL						0x00	/**< NULL - Nothing to report */
#define FJ_SR1_SUCCESS					0x01	/**< Success */
#define FJ_SR1_FAILURE					0x02	/**< Failure */
#define FJ_SR1_FILE_TRANS_BUSY			0x0A	/**< Not used. (File transfer busy) */
#define FJ_SR1_FILE_TRANS_DONE			0x0B	/**< Not used. (File transfer done) */
#define FJ_SR1_FILE_TRANS_FAILED		0x0C	/**< Not used. (File transfer failed) */
#define FJ_SR1_SLOW_MEDIA_WARNING		0x0E	/**< Not used. (Slow media warning) */
#define FJ_SR1_RECORDING_AUDIO			0x12	/**< Not used. (Recording Audio) */
#define FJ_SR1_PLAYING_AUDIO			0x13	/**< Not used. (Playing Audio) */
#define FJ_SR1_I2C_TRANSACTION_SUCCESS	0x19	/**< Not used. (I2C transaction succeeded) */
#define FJ_SR1_I2C_TRANSACTION_FAILED	0x1A	/**< Not used. (I2C transaction failed) */
#define FJ_SR1_CUR_PATH_SUCCESS			0x1B	/**< Not used. (SetCurrentPath succeeded) */
#define FJ_SR1_CUR_PATH_FAILED			0x1C	/**< Not used. (SetCurrentPath failed) */
#define FJ_SR1_MANUAL_STAMP_STARTED		0x1E	/**< Not used. (Manual data stamping can be started) */

/** Sr2 Table value(bit data) */
#define FJ_SR2_DZOOM_COMPLETED			0		/**< Not used. (Digital Zoom Complete) */
#define FJ_SR2_NO_AUDIO_DRAM			1		/**< Not Enough Audio DRAM.<br>
													 This bit is set to '1' when recording audio, but there is no 
													 sufficient DRAM space for that.<br>
													 This flag is cleared by @ref FJ_QueryStatus. */
#define FJ_SR2_OPE_COMPLETED			2		/**< Operation Complete.<br>
													 This bit is set to '1' when audio playback or 
													 recording has been completed.<br>
													 This flag is cleared by @ref FJ_QueryStatus. */
#define FJ_SR2_NO_FALSHMEM				3		/**< Not used. (Not Enough Flash Memory) */
#define FJ_SR2_NFP						6		/**< Not used. (NFP) */
#define FJ_SR2_PIP_EFF_COMPLETED		7		/**< Not used. (PIP Effect Complete) */
#define FJ_SR2_FLASH_LIGHT_REQ			9		/**< Not used. (Flash Light Required) */
#define FJ_SR2_AE_CONV					10		/**< Not used. (AE Converged, Audio exists or mount completed) */
#define FJ_SR2_CARD_EXIST_IND			13		/**< Flash Card Exists Indication.<br>
													 This bit is set to '1' when the card is physically hooked on 
													 its flash memory interface and to '0' when the card is not.<br>
													 Note that on any card status change, the Base firmware will 
													 set "Flash Card Exists Indication" status <br>
													 and only then set "Flash Card Insertion/Removal Interrupt" to '1'.<br>
													 This flag is not cleared by @ref FJ_QueryStatus. */
#define FJ_SR2_CARD_INS_REM_INT			14		/**< Flash Card Insertion/Removal Interrupt.<br>
													 This bit is to indicate a change in the 
													 flash memory card status(Flash Card Exists Indication).<br>
													 It is set when a memory card is inserted or removed.<br>
													 Note that this bit is set only after "Flash Card Exists Indication" bit is updated.<br>
													 This flag is cleared by @ref FJ_QueryStatus. */
#define FJ_SR2_AWB_CONV					15		/**< Not used. (AWB Converged) */


///////////////////////////////////////////////////////////////////////////////
// enumeration
///////////////////////////////////////////////////////////////////////////////
/** MODE */
/*
 bit 31-28 : reserve
 bit 27- 0 : mode difinition
*/

typedef enum {
	FJ_MODE_PLAYBACK				= 0x02000000,	/**< PLAYBACK mode */
	FJ_MODE_CONTI_CAPTURE			= 0x00200000,	/**< Continuous Capture mode */
	FJ_MODE_PHOTO 					= 0x00100000,	/**< Photo mode */
	FJ_MODE_BURST_CAPTURE			= 0x00100002,	/**< Burst Capture mode */
	FJ_MODE_STILL_HDR 				= 0x00100008,	/**< Still HDR mode */
	FJ_MODE_CALIBRATION				= 0x00080000,	/**< CALIBRATION mode */
	FJ_MODE_VIDEO 					= 0x00040000,	/**< VIDEO mode */
	FJ_MODE_IDLE 					= 0x00020000,	/**< IDLE mode */
	FJ_MODE_BOOT					= 0x00010000,	/**< BOOT mode */
	FJ_MODE_UNMOUNT					= 0x10000000,	/**< UnMount mode */
	FJ_MODE_STOP					= 0x00008000,	/**< STOP mode(for only internal) */

//@cond
	/* Category : Power on :0x08000000 */
	FJ_MODE_POWER_DOWN				= 0x08000000,	/**< POWER DOWN mode */

	/* Category : Menu : 0x04000000 */
	FJ_MODE_MENU					= 0x04000000,	/**< MENU mode */
	FJ_MODE_PLAYBACK_THUMB			= 0x02000001,	/**< PLAYBACK THUM mode */
	FJ_MODE_DISP_THUMB_DRAW			= 0x01000010,	/**< RESTORE THUM mode */
	FJ_MODE_DISP_THUMBINDEX			= 0x01000020,	/**< RESTORE THUM INDEX mode */

	FJ_MODE_DISP_IMAGE_MAIN			= 0x01000000,	/**< RESTORE IMAGE FLASH mode */
	FJ_MODE_DISP_IMAGE_THUMB		= 0x01000001,	/**< RESTORE IMAGE THUMB mode */
	//FJ_MODE_DISP_IMAGE_SCREEN		= 0x01000002,	/**< RESTORE IMAGE SCREEN mode */
	FJ_MODE_DISP_MONCLS1_MAIN		= 0x01000040,	/**< RESTORE MONITOR CLASS1 FLASH mode */
	FJ_MODE_DISP_MONCLS1_THUMB		= 0x01000080,	/**< RESTORE MONITOR CLASS1 THUMB mode */
	FJ_MODE_DISP_MONCLS2_MAIN		= 0x01000100,	/**< RESTORE MONITOR CLASS2 FLASH mode */
	FJ_MODE_DISP_MONCLS2_THUMB		= 0x01000200,	/**< RESTORE MONITOR CLASS2 THUMB mode */
	FJ_MODE_RESTORE_MAIN_DECODE		= 0x01000400,	/**< RESTORE MAIN DECODE mode */

	FJ_MODE_STORE_IMAGE_FLASH		= 0x00800000,	/**< STORE IMAGE FLASH mode */
	FJ_MODE_VIDEO_RECORD			= 0x00400000,	/**< VIDEO RECORD mode */
	FJ_MODE_CAPTURE_H264			= 0x00400001,	/**< CAPTURE H264 mode */
	FJ_MODE_CAPTURE_H265			= 0x00400002,	/**< CAPTURE H265 mode */
	FJ_MODE_CAPTURE_AUDIO			= 0x00400004,	/**< CAPTURE Audio mode */
	FJ_MODE_CAPTURE_H264_AUDIO		= FJ_MODE_CAPTURE_H264|FJ_MODE_CAPTURE_AUDIO,	/**< CAPTURE H264+Audio mode */
	FJ_MODE_CAPTURE_H265_AUDIO		= FJ_MODE_CAPTURE_H265|FJ_MODE_CAPTURE_AUDIO,	/**< CAPTURE H265+Audio mode */
	FJ_MODE_CAPTURE_H264_H265		= FJ_MODE_CAPTURE_H264|FJ_MODE_CAPTURE_H265,	/**< CAPTURE H264+H265 mode */
	FJ_MODE_CAPTURE_H264_H265_Audio = FJ_MODE_CAPTURE_H264|FJ_MODE_CAPTURE_H265|FJ_MODE_CAPTURE_AUDIO,	/**< CAPTURE H264+H265*Audio mode */
	FJ_MODE_CAPTURE					= 0x00400010,	/**< CAPTURE mode(temporary) */
	FJ_MODE_CAPTURE_AEB				= 0x00200001,	/**< CAPTURE AEB mode */
	FJ_MODE_CAPTURE_SEQ				= 0x00200002,	/**< CAPTURE Sequence mode(temporary) */
	FJ_MODE_CAPTURE_NIGHT			= 0x00100001,	/**< CAPTURE NIGHT mode */
	FJ_MODE_CAPTURE_SEQUENCE		= 0x00100004,	/**< CAPTURE Sequence mode */
	FJ_MODE_CAPTURE_DRAFT			= 0x00040001,	/**< CAPTURE DRAFT mode */
	FJ_MODE_PREVIEW_ZOOM			= 0x00040002,	/**< PREIVEW ZOOM mode */
	FJ_MODE_MOVIE_VIEW				= 0x00040004,	/**< MOVIE VIEW mode */
	FJ_MODE_PANORAMA_VIEW			= 0x00040008,	/**< PANORAMA VIEW mode */
	FJ_MODE_PANORAMA_REC			= 0x00040010,	/**< PANORAMA REC mode */
	FJ_MODE_HIGHSPEED			    = 0x00040020,	/**< HS(High-Speed) mode */
	FJ_MODE_VIEW 					= 0x00040080,	/**< VIEW mode(temporary) */
	FJ_MODE_IDLE_H264				= 0x00020010,	/**< IDLE H264 mode */
	FJ_MODE_IDLE_AUDIO				= 0x00020040,	/**< IDLE Audio mode */
	FJ_MODE_IDLE_H264_AUDIO			= FJ_MODE_IDLE_H264|FJ_MODE_IDLE_AUDIO,		/**< IDLE (H264+Audio) mode */
	FJ_MODE_IP_CHECK				= 0x00020001,	/**< IP Check mode */
	FJ_MODE_CHANGE_FAILURE			= 0x000000F0,	/**< MODE change failure (T.B.D) */
	FJ_MODE_INITIALIZER				= 0x000000FF	/**< INITIALIZER mode */
//@endcond
} FJ_MODE;

/** Status Register kind. */
typedef enum {
	FJ_STATUS_REGISTER_SR1			= 1,		/**< Sr1 Status Register */
	FJ_STATUS_REGISTER_SR2			= 2,		/**< Sr2 Status Register */
} FJ_STATUS_REGISTER;

/*@} --- end of fj_mode_definition */

///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// external variable
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// macro
///////////////////////////////////////////////////////////////////////////////

/** @weakgroup fj_mode_api
@{*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function only sets the next mode. But "FJ_MODE_STOP" can not be specified.<br>
The state transition in the specified mode is performed by @ref FJ_StartMode.
@param[in]	bNewMode	New Mode Value. See @ref FJ_MODE
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error
@remarks 
The current mode is not changed by this function.

*/
extern FJ_ERR_CODE	FJ_SetMode( FJ_MODE bNewMode );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Terminates the current mode, and transits to the requested mode by FJ_SetMode().<br>

@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abormal End
@remarks 
BaseFW does not check the correctness of the transition. It is the responsibility of the host.<br>
The host has to wait until mode transition has been completed.<br>

For Example,<br>
Idle -> Preview : FJ_SetMode( View );<br>
After BaseFW change into preview mode, BaseFW updates Sr1 status to "FJ_SR1_SUCCESS".<br>
Preview -> Idle : FJ_SetMode( Idle );<br>
After BaseFW change into idle mode, BaseFW updates Sr1 status to "FJ_SR1_SUCCESS".
*/
extern FJ_ERR_CODE	FJ_StartMode( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Stop the processing of the current mode. But Current mode is not changed.
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abormal End
@remarks
BaseFW updates Sr1 status to "FJ_SR1_SUCCESS".
*/
extern FJ_ERR_CODE	FJ_StopMode( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Check whether the stop status.
@retval		TRUE  : Stop Mode
@retval		FALSE : not Stop Mode
@remarks
A current mode can be confirmed in FJ_Get_Act_Sysmode(). 
*/
extern BOOL FJ_Is_ModeStatus_Stop( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Wait Mode Change finished. Timeout time is sleepTime * repeatNum (ms).<br>
This API is used to wait for state transitions after the @ref FJ_SetMode or @ref FJ_StopMode.
@param[in]	wait_mode	See @ref FJ_MODE
@param[in]	sleepTime	sleeping time(ms)
@param[in]	repeatNum	number of repeat
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abormal End
@remark		Waiting function of specified time is available when sleepTime >= 1ms.<br>
			API can support the "forever waiting" as follows. <br>
 			- sleepTime=0<br>
 			- repeatNum=0 (Don't care)<br>
*/
extern FJ_ERR_CODE	FJ_WaitModeComp( FJ_MODE wait_mode, UINT32 sleepTime, UINT32 repeatNum );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
get Active system mode.
@return		Active system mode
*/
extern FJ_MODE	FJ_Get_Act_Sysmode( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Gets Status Register.
@param[out]	pwUSR1	Sr1 status words.<br>
					Upper 3 bytes shows the mode. Please refer to @ref FJ_MODE.<br>
					Lower 1 byte shows the processing result.<br>
					Please refer to the following definitions.<br>
					<ul><li>@ref FJ_SR1_NULL
						<li>@ref FJ_SR1_SUCCESS
						<li>@ref FJ_SR1_FAILURE</ul>
@param[out]	pwUSR2	Sr2 status words.<br>
					Please refer to the following definitions.<br>
					<ul><li>@ref FJ_SR2_CARD_INS_REM_INT
						<li>@ref FJ_SR2_CARD_EXIST_IND
						<li>@ref FJ_SR2_OPE_COMPLETED
						<li>@ref FJ_SR2_NO_AUDIO_DRAM</ul>
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
*/
extern FJ_ERR_CODE	FJ_QueryStatus( UINT32* pwUSR1, USHORT* pwUSR2 );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Gets Status Register specified by bRegister parameter.
@param[in]	bRegister	The kind of status register to refer to is specified.<br>
						Please refer to @ref FJ_STATUS_REGISTER.<br>
@param[out]	pwValue		Status word.<br>
						Please refer to explanation of @ref FJ_QueryStatus function for status word value. 
@retval		FJ_ERR_OK : Normal End
@retval		FJ_ERR_NG : Abnormal End
*/
extern FJ_ERR_CODE	FJ_GetStatusRegister( FJ_STATUS_REGISTER bRegister, USHORT* pwValue );


/*@} --- end of fj_mode_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_mode_message
@{*/

/**
@def FJ_HM_NEW_MODE
This message is sent upon mode switching complete in base F/W
@param[in]	Mode id	See @ref FJ_MODE
*/

/**
@def FJ_HM_CALIB_MODE_END
Calibratoin mode process end.
@param[in]	0	None. Always 0
*/

/*@} --- end of fj_mode_message */
#endif	/* _FJ_MODE_H */
